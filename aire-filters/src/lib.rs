mod android_bitmap;
mod android_rgba;
mod bitmap_helper;
mod clahe;
mod equalize_hist;
mod gamma_curves;
mod histogram;
mod hsl;
mod hsv;
mod lab;
mod surface_type;
mod xyz;
mod luv;

#[cfg(target_os = "android")]
#[allow(non_snake_case)]
pub mod android {
    extern crate jni;

    use crate::bitmap_helper::android_bitmap;
    use crate::clahe::{ahe_lab_rgba, ahe_luv_rgba, ahe_yuv_rgba, clahe_lab_rgba, clahe_luv_rgba, clahe_yuv_rgba, ClaheGridSize};
    use jni::sys::{jfloat, jint, jobject};
    use jni::JNIEnv;

    use crate::equalize_hist::{
        equalize_histogram, equalize_histogram_hsv, equalize_histogram_squares_rgba,
        EqualizeHistogramChannels,
    };

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
                equalize_histogram_hsv(&mut info.data, info.stride, info.width, info.height);

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

    #[no_mangle]
    pub unsafe extern "system" fn Java_com_awxkee_aire_pipeline_EffectsPipelineImpl_equalizeHistAdaptiveImpl(
        mut env: JNIEnv,
        _: jobject,
        bitmap: jobject,
        grid_size_w: jint,
        grid_size_h: jint,
    ) -> jobject {
        if grid_size_w <= 0 || grid_size_h <= 0 {
            let clazz = env
                .find_class("java/lang/Exception")
                .expect("Found exception class");
            env.throw_new(clazz, "Grid Size zeros or negative is not supported")
                .expect("Failed to access JNI");
            return bitmap;
        }

        let bitmap_info = android_bitmap::get_bitmap_rgba8888(&mut env, bitmap);
        match bitmap_info {
            Ok(mut info) => {
                ahe_yuv_rgba(
                    &mut info.data,
                    info.stride,
                    info.width,
                    info.height,
                    1f32,
                    ClaheGridSize::new(grid_size_w as u32, grid_size_h as u32),
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

    #[no_mangle]
    pub unsafe extern "system" fn Java_com_awxkee_aire_pipeline_EffectsPipelineImpl_claheImpl(
        mut env: JNIEnv,
        _: jobject,
        bitmap: jobject,
        threshold: jfloat,
        grid_size_w: jint,
        grid_size_h: jint,
    ) -> jobject {
        if grid_size_w <= 0 || grid_size_h <= 0 {
            let clazz = env
                .find_class("java/lang/Exception")
                .expect("Found exception class");
            env.throw_new(clazz, "Grid Size zeros or negative is not supported")
                .expect("Failed to access JNI");
            return bitmap;
        }

        let bitmap_info = android_bitmap::get_bitmap_rgba8888(&mut env, bitmap);
        match bitmap_info {
            Ok(mut info) => {
                clahe_yuv_rgba(
                    &mut info.data,
                    info.stride,
                    info.width,
                    info.height,
                    threshold,
                    ClaheGridSize::new(grid_size_w as u32, grid_size_h as u32),
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

    #[no_mangle]
    pub unsafe extern "system" fn Java_com_awxkee_aire_pipeline_EffectsPipelineImpl_equalizeHistSquaresImpl(
        mut env: JNIEnv,
        _: jobject,
        bitmap: jobject,
        grid_size_w: jint,
        grid_size_h: jint,
    ) -> jobject {
        if grid_size_w <= 0 || grid_size_h <= 0 {
            let clazz = env
                .find_class("java/lang/Exception")
                .expect("Found exception class");
            env.throw_new(clazz, "Grid Size zeros or negative is not supported")
                .expect("Failed to access JNI");
            return bitmap;
        }
        let bitmap_info = android_bitmap::get_bitmap_rgba8888(&mut env, bitmap);
        match bitmap_info {
            Ok(mut info) => {
                equalize_histogram_squares_rgba(
                    &mut info.data,
                    info.stride,
                    info.width,
                    info.height,
                    ClaheGridSize::new(grid_size_w as u32, grid_size_h as u32),
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

    #[no_mangle]
    pub unsafe extern "system" fn Java_com_awxkee_aire_pipeline_EffectsPipelineImpl_claheLUVImpl(
        mut env: JNIEnv,
        _: jobject,
        bitmap: jobject,
        threshold: jfloat,
        grid_size_w: jint,
        grid_size_h: jint,
    ) -> jobject {
        if grid_size_w <= 0 || grid_size_h <= 0 {
            let clazz = env
                .find_class("java/lang/Exception")
                .expect("Found exception class");
            env.throw_new(clazz, "Grid Size zeros or negative is not supported")
                .expect("Failed to access JNI");
            return bitmap;
        }

        let bitmap_info = android_bitmap::get_bitmap_rgba8888(&mut env, bitmap);
        match bitmap_info {
            Ok(mut info) => {
                clahe_luv_rgba(
                    &mut info.data,
                    info.stride,
                    info.width,
                    info.height,
                    threshold,
                    ClaheGridSize::new(grid_size_w as u32, grid_size_h as u32),
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

    #[no_mangle]
    pub unsafe extern "system" fn Java_com_awxkee_aire_pipeline_EffectsPipelineImpl_equalizeHistAdaptiveLUVImpl(
        mut env: JNIEnv,
        _: jobject,
        bitmap: jobject,
        grid_size_w: jint,
        grid_size_h: jint,
    ) -> jobject {
        if grid_size_w <= 0 || grid_size_h <= 0 {
            let clazz = env
                .find_class("java/lang/Exception")
                .expect("Found exception class");
            env.throw_new(clazz, "Grid Size zeros or negative is not supported")
                .expect("Failed to access JNI");
            return bitmap;
        }

        let bitmap_info = android_bitmap::get_bitmap_rgba8888(&mut env, bitmap);
        match bitmap_info {
            Ok(mut info) => {
                ahe_luv_rgba(
                    &mut info.data,
                    info.stride,
                    info.width,
                    info.height,
                    1f32,
                    ClaheGridSize::new(grid_size_w as u32, grid_size_h as u32),
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

    #[no_mangle]
    pub unsafe extern "system" fn Java_com_awxkee_aire_pipeline_EffectsPipelineImpl_claheLABImpl(
        mut env: JNIEnv,
        _: jobject,
        bitmap: jobject,
        threshold: jfloat,
        grid_size_w: jint,
        grid_size_h: jint,
    ) -> jobject {
        if grid_size_w <= 0 || grid_size_h <= 0 {
            let clazz = env
                .find_class("java/lang/Exception")
                .expect("Found exception class");
            env.throw_new(clazz, "Grid Size zeros or negative is not supported")
                .expect("Failed to access JNI");
            return bitmap;
        }

        let bitmap_info = android_bitmap::get_bitmap_rgba8888(&mut env, bitmap);
        match bitmap_info {
            Ok(mut info) => {
                clahe_lab_rgba(
                    &mut info.data,
                    info.stride,
                    info.width,
                    info.height,
                    threshold,
                    ClaheGridSize::new(grid_size_w as u32, grid_size_h as u32),
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

    #[no_mangle]
    pub unsafe extern "system" fn Java_com_awxkee_aire_pipeline_EffectsPipelineImpl_equalizeHistAdaptiveLABImpl(
        mut env: JNIEnv,
        _: jobject,
        bitmap: jobject,
        grid_size_w: jint,
        grid_size_h: jint,
    ) -> jobject {
        if grid_size_w <= 0 || grid_size_h <= 0 {
            let clazz = env
                .find_class("java/lang/Exception")
                .expect("Found exception class");
            env.throw_new(clazz, "Grid Size zeros or negative is not supported")
                .expect("Failed to access JNI");
            return bitmap;
        }

        let bitmap_info = android_bitmap::get_bitmap_rgba8888(&mut env, bitmap);
        match bitmap_info {
            Ok(mut info) => {
                ahe_lab_rgba(
                    &mut info.data,
                    info.stride,
                    info.width,
                    info.height,
                    1f32,
                    ClaheGridSize::new(grid_size_w as u32, grid_size_h as u32),
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

pub use surface_type::aire_reformat_surface_laba_to_u8;
pub use surface_type::aire_reformat_surface_linear_to_u8;
pub use surface_type::aire_reformat_surface_u8_to_laba;
pub use surface_type::aire_reformat_surface_u8_to_linear;
pub use surface_type::aire_reformat_surface_luva_to_u8;
pub use surface_type::aire_reformat_surface_u8_to_luva;
