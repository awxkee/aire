#[cfg(target_os = "android")]
#[allow(non_snake_case)]
pub mod android {
    extern crate jni;

    use jni::objects::JObject;
    use jni::sys::{jint, jobject};
    use jni::JNIEnv;
    use palette_transfer::{copy_palette_rgba, TransferColorspace};

    use crate::android_bitmap::copy_image;
    use crate::bitmap_helper::android_bitmap;

    #[no_mangle]
    pub unsafe extern "system" fn Java_com_awxkee_aire_pipeline_EffectsPipelineImpl_copyPaletteImpl(
        mut env: JNIEnv,
        _: jobject,
        source: jobject,
        destination: jobject,
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

        return match new_bitmap_r {
            Ok(new_bitmap) => new_bitmap.as_raw(),
            Err(error_message) => {
                let clazz = env
                    .find_class("java/lang/Exception")
                    .expect("Found exception class");
                env.throw_new(clazz, error_message)
                    .expect("Failed to access JNI");
                JObject::null().as_raw()
            }
        };
    }
}
