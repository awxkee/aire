#[cfg(target_os = "android")]
#[allow(non_snake_case)]
pub mod android {
    extern crate jni;

    use colorutils_rs::{linear_to_rgba, rgba_to_oklab, Oklab, TransferFunction};
    use jni::objects::JObject;
    use jni::sys::{jfloat, jint, jobject};
    use jni::JNIEnv;
    use num_traits::Float;
    use palette_transfer::{copy_palette_rgba, TransferColorspace};

    use crate::android_bitmap::copy_image;
    use crate::bitmap_helper::android_bitmap;
    use crate::tonemap::{aces_hill, uncharted2_filmic};

    #[no_mangle]
    pub unsafe extern "system" fn Java_com_awxkee_aire_pipeline_EffectsPipelineImpl_copyPaletteImpl(
        mut env: JNIEnv,
        _: jobject,
        source: jobject,
        destination: jobject,
        intensity: jfloat,
        colorspace: jint,
    ) -> jobject {
        let transfer = match colorspace {
            0 => TransferColorspace::LALPHABETA,
            1 => TransferColorspace::LAB,
            2 => TransferColorspace::OKLAB,
            3 => TransferColorspace::LUV,
            _ => {
                let clazz = env
                    .find_class("java/lang/Exception")
                    .expect("Found exception class");
                env.throw_new(clazz, "Unknown colorspace was requested")
                    .expect("Failed to access JNI");
                return JObject::null().as_raw();
            }
        };

        let mut source_bitmap_info = match android_bitmap::get_bitmap_rgba8888(&mut env, source) {
            Ok(v) => v,
            Err(err) => {
                let clazz = env
                    .find_class("java/lang/Exception")
                    .expect("Found exception class");
                env.throw_new(clazz, err).expect("Failed to access JNI");
                return JObject::null().as_raw();
            }
        };

        let mut source_bitmap: Vec<u8> =
            vec![0; source_bitmap_info.width as usize * source_bitmap_info.height as usize * 4];

        copy_image::<u8>(
            &source_bitmap_info.data,
            source_bitmap_info.stride,
            &mut source_bitmap,
            (source_bitmap_info.width as usize * 4usize * std::mem::size_of::<u8>()) as u32,
            source_bitmap_info.width,
            source_bitmap_info.height,
            4u32,
        );

        source_bitmap_info.data.resize(0, 0);

        let mut destination_bitmap_info =
            match android_bitmap::get_bitmap_rgba8888(&mut env, destination) {
                Ok(v) => v,
                Err(err) => {
                    let clazz = env
                        .find_class("java/lang/Exception")
                        .expect("Found exception class");
                    env.throw_new(clazz, err).expect("Failed to access JNI");
                    return JObject::null().as_raw();
                }
            };

        let mut dest_bitmap: Vec<u8> = vec![
            0;
            destination_bitmap_info.width as usize
                * destination_bitmap_info.height as usize
                * 4
        ];

        copy_image::<u8>(
            &destination_bitmap_info.data,
            destination_bitmap_info.stride,
            &mut dest_bitmap,
            (destination_bitmap_info.width as usize * 4usize * std::mem::size_of::<u8>()) as u32,
            destination_bitmap_info.width,
            destination_bitmap_info.height,
            4u32,
        );

        destination_bitmap_info.data.resize(0, 0);

        if let Err(string) = copy_palette_rgba(
            &source_bitmap,
            source_bitmap_info.width,
            source_bitmap_info.height,
            &mut dest_bitmap,
            destination_bitmap_info.width,
            destination_bitmap_info.height,
            intensity,
            transfer,
        ) {
            let clazz = env
                .find_class("java/lang/Exception")
                .expect("Found exception class");
            env.throw_new(clazz, string).expect("Failed to access JNI");
            return JObject::null().as_raw();
        }

        let new_bitmap_r = android_bitmap::create_bitmap(
            &mut env,
            &dest_bitmap,
            (destination_bitmap_info.width as usize * 4usize * std::mem::size_of::<u8>()) as u32,
            destination_bitmap_info.width,
            destination_bitmap_info.height,
        );

        match new_bitmap_r {
            Ok(new_bitmap) => new_bitmap.as_raw(),
            Err(error_message) => {
                let clazz = env
                    .find_class("java/lang/Exception")
                    .expect("Found exception class");
                env.throw_new(clazz, error_message)
                    .expect("Failed to access JNI");
                JObject::null().as_raw()
            }
        }
    }

    #[no_mangle]
    pub unsafe extern "system" fn Java_com_awxkee_aire_pipeline_BasePipelinesImpl_saturationImpl(
        mut env: JNIEnv,
        _: jobject,
        source: jobject,
        saturation: jfloat,
        tone_map: bool,
    ) -> jobject {
        let mut source_bitmap_info = match android_bitmap::get_bitmap_rgba8888(&mut env, source) {
            Ok(v) => v,
            Err(err) => {
                let clazz = env
                    .find_class("java/lang/Exception")
                    .expect("Found exception class");
                env.throw_new(clazz, err).expect("Failed to access JNI");
                return JObject::null().as_raw();
            }
        };

        let mut oklab_image =
            vec![0f32; source_bitmap_info.width as usize * source_bitmap_info.height as usize * 4];
        rgba_to_oklab(
            &source_bitmap_info.data,
            source_bitmap_info.stride,
            &mut oklab_image,
            source_bitmap_info.width * 4 * size_of::<f32>() as u32,
            source_bitmap_info.width,
            source_bitmap_info.height,
            TransferFunction::Srgb,
        );

        if tone_map {
            for scan_lane in oklab_image.chunks_exact_mut(source_bitmap_info.width as usize * 4) {
                for in_place in scan_lane.chunks_exact_mut(4) {
                    let mut oklab = Oklab::new(in_place[0], in_place[1], in_place[2]);
                    oklab.a *= saturation;
                    oklab.b *= saturation;
                    let linear_rgb = oklab.to_linear_rgb();
                    let color = aces_hill(linear_rgb);
                    in_place[0] = color.r.min(1.).max(0.);
                    in_place[1] = color.g.min(1.).max(0.);
                    in_place[2] = color.b.min(1.).max(0.);
                }
            }
        } else {
            for scan_lane in oklab_image.chunks_exact_mut(source_bitmap_info.width as usize * 4) {
                for in_place in scan_lane.chunks_exact_mut(4) {
                    let mut oklab = Oklab::new(in_place[0], in_place[1], in_place[2]);
                    oklab.a *= saturation;
                    oklab.b *= saturation;
                    let color = oklab.to_linear_rgb();
                    in_place[0] = color.r.min(1.).max(0.);
                    in_place[1] = color.g.min(1.).max(0.);
                    in_place[2] = color.b.min(1.).max(0.);
                }
            }
        }

        linear_to_rgba(
            &oklab_image,
            source_bitmap_info.width * 4 * size_of::<f32>() as u32,
            &mut source_bitmap_info.data,
            source_bitmap_info.stride,
            source_bitmap_info.width,
            source_bitmap_info.height,
            TransferFunction::Srgb,
        );

        let new_bitmap_r = android_bitmap::create_bitmap(
            &mut env,
            &source_bitmap_info.data,
            source_bitmap_info.stride,
            source_bitmap_info.width,
            source_bitmap_info.height,
        );

        match new_bitmap_r {
            Ok(new_bitmap) => new_bitmap.as_raw(),
            Err(error_message) => {
                let clazz = env
                    .find_class("java/lang/Exception")
                    .expect("Found exception class");
                env.throw_new(clazz, error_message)
                    .expect("Failed to access JNI");
                JObject::null().as_raw()
            }
        }
    }
}
