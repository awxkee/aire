mod bitmap_helper;
mod equalize_hist;
mod histogram;
mod android_rgba;
mod android_bitmap;
mod hsv;
mod hsl;
mod surface_type;
mod gamma_curves;
mod lab;
mod xyz;

#[cfg(target_os = "android")]
#[allow(non_snake_case)]
pub mod android {
    extern crate jni;

    use crate::bitmap_helper::android_bitmap;
    use jni::sys::jobject;
    use jni::JNIEnv;

    use crate::equalize_hist::{equalize_histogram, equalize_histogram_hsv, EqualizeHistogramChannels};

    #[no_mangle]
    pub unsafe extern "system" fn Java_com_awxkee_aire_pipeline_EffectsPipelineImpl_equalizeHistImpl(
        mut env: JNIEnv,
        _: jobject,
        bitmap: jobject,
    ) -> jobject {
        let bitmap_info = android_bitmap::get_bitmap_rgba8888(&mut env, bitmap);
        match bitmap_info {
            Ok(info) => {
                let mut dst_vec: Vec<u8> = vec![];
                dst_vec.resize(info.stride as usize * info.height as usize, 0u8);

                equalize_histogram::<{ EqualizeHistogramChannels::Channels4 as u8 }>(
                    &info.data,
                    info.stride,
                    &mut dst_vec,
                    info.stride,
                    info.width,
                    info.height,
                );

                let new_bitmap_r = android_bitmap::create_bitmap(
                    &mut env,
                    &dst_vec,
                    info.stride,
                    info.width,
                    info.height,
                );

                return match new_bitmap_r {
                    Ok(new_bitmap) => new_bitmap.as_raw(),
                    Err(error_message) => {
                        let clazz = env
                            .find_class("java/lang/Exception")
                            .expect("Found exception class");
                        env.throw_new(clazz, error_message)
                            .expect("Failed to access JNI");
                        bitmap
                    }
                };
            }
            Err(error_message) => {
                let clazz = env
                    .find_class("java/lang/Exception")
                    .expect("Found exception class");
                env.throw_new(clazz, error_message)
                    .expect("Failed to access JNI");
                bitmap
            }
        }
    }

    #[no_mangle]
    pub unsafe extern "system" fn Java_com_awxkee_aire_pipeline_EffectsPipelineImpl_equalizeHistHSVImpl(
        mut env: JNIEnv,
        _: jobject,
        bitmap: jobject,
    ) -> jobject {
        let bitmap_info = android_bitmap::get_bitmap_rgba8888(&mut env, bitmap);
        match bitmap_info {
            Ok(mut info) => {
                equalize_histogram_hsv(
                    &mut info.data,
                    info.stride,
                    info.width,
                    info.height,
                );

                let new_bitmap_r = android_bitmap::create_bitmap(
                    &mut env,
                    &info.data,
                    info.stride,
                    info.width,
                    info.height,
                );

                return match new_bitmap_r {
                    Ok(new_bitmap) => new_bitmap.as_raw(),
                    Err(error_message) => {
                        let clazz = env
                            .find_class("java/lang/Exception")
                            .expect("Found exception class");
                        env.throw_new(clazz, error_message)
                            .expect("Failed to access JNI");
                        bitmap
                    }
                };
            }
            Err(error_message) => {
                let clazz = env
                    .find_class("java/lang/Exception")
                    .expect("Found exception class");
                env.throw_new(clazz, error_message)
                    .expect("Failed to access JNI");
                bitmap
            }
        }
    }
}

pub use surface_type::aire_reformat_surface_u8_to_linear;
pub use surface_type::aire_reformat_surface_linear_to_u8;
pub use surface_type::aire_reformat_surface_u8_to_laba;
pub use surface_type::aire_reformat_surface_laba_to_u8;