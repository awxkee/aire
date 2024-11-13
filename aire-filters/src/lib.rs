pub use surface_type::aire_reformat_surface_laba_to_u8;
pub use surface_type::aire_reformat_surface_linear_to_u8;
pub use surface_type::aire_reformat_surface_luva_to_u8;
pub use surface_type::aire_reformat_surface_u8_to_laba;
pub use surface_type::aire_reformat_surface_u8_to_linear;
pub use surface_type::aire_reformat_surface_u8_to_luva;

mod android_bitmap;
mod bitmap_helper;
mod blur;
mod equalize_hist;
mod histogram;
mod hists;
mod morph;
mod palette;
mod scalar;
mod surface_type;
mod transfer_resolve;
mod tonemap;

#[cfg(target_os = "android")]
#[allow(non_snake_case)]
pub mod android {
    extern crate jni;

    use colorutils_rs::TransferFunction;
    use jni::objects::JObject;
    use jni::sys::{jint, jobject};
    use jni::JNIEnv;
    use pic_scale::{
        ImageSize, ImageStore, JzazbzScaler, LChScaler, LabScaler, LinearScaler, LuvScaler,
        OklabScaler, ResamplingFunction, Scaler, Scaling, SigmoidalScaler, ThreadingPolicy,
        XYZScaler,
    };

    use crate::android_bitmap::copy_image;
    use crate::bitmap_helper::android_bitmap;

    #[no_mangle]
    pub unsafe extern "system" fn Java_com_awxkee_aire_pipeline_ScalePipelinesImpl_resizeImpl(
        mut env: JNIEnv,
        _: jobject,
        bitmap: jobject,
        new_width: jint,
        new_height: jint,
        resampler: jint,
        colorspace: jint,
    ) -> jobject {
        if new_width <= 0 || new_height <= 0 {
            let clazz = env
                .find_class("java/lang/Exception")
                .expect("Found exception class");
            env.throw_new(clazz, "Negative sizes are not supported")
                .expect("Failed to access JNI");
            return bitmap;
        }

        let bitmap_info = match android_bitmap::get_bitmap_rgba8888(&mut env, bitmap) {
            Ok(bmp) => bmp,
            Err(err) => {
                let clazz = env
                    .find_class("java/lang/Exception")
                    .expect("Found exception class");
                env.throw_new(clazz, err).expect("Failed to access JNI");
                return JObject::default().as_raw();
            }
        };

        let resampling: ResamplingFunction = (resampler as u32).into();

        let mut scaler: Box<dyn Scaling> = match colorspace {
            0 => Box::new(Scaler::new(resampling)),
            1 => Box::new(LabScaler::new(resampling)),
            2 => Box::new(LinearScaler::new(resampling)),
            3 => Box::new(LuvScaler::new(resampling)),
            4 => Box::new(SigmoidalScaler::new(resampling)),
            5 => Box::new(XYZScaler::new(resampling)),
            6 => Box::new(LinearScaler::new_with_transfer(
                resampling,
                TransferFunction::Srgb,
            )),
            7 => Box::new(LinearScaler::new_with_transfer(
                resampling,
                TransferFunction::Rec709,
            )),
            8 => Box::new(LinearScaler::new_with_transfer(
                resampling,
                TransferFunction::Gamma2p2,
            )),
            9 => Box::new(LinearScaler::new_with_transfer(
                resampling,
                TransferFunction::Gamma2p8,
            )),
            10 => Box::new(LChScaler::new(resampling)),
            11 => Box::new(OklabScaler::new(resampling, TransferFunction::Srgb)),
            12 => Box::new(OklabScaler::new(resampling, TransferFunction::Rec709)),
            13 => Box::new(OklabScaler::new(resampling, TransferFunction::Gamma2p2)),
            14 => Box::new(OklabScaler::new(resampling, TransferFunction::Gamma2p8)),
            15 => Box::new(JzazbzScaler::new(resampling, 200., TransferFunction::Srgb)),
            16 => Box::new(JzazbzScaler::new(
                resampling,
                200.,
                TransferFunction::Rec709,
            )),
            17 => Box::new(JzazbzScaler::new(
                resampling,
                200.,
                TransferFunction::Gamma2p2,
            )),
            18 => Box::new(JzazbzScaler::new(
                resampling,
                200.,
                TransferFunction::Gamma2p8,
            )),
            _ => {
                let clazz = env
                    .find_class("java/lang/Exception")
                    .expect("Found exception class");
                env.throw_new(clazz, "Unknown resampling function was requested")
                    .expect("Failed to access JNI");
                return JObject::default().as_raw();
            }
        };
        scaler.set_threading_policy(ThreadingPolicy::Adaptive);

        let mut source_buffer = vec![
            0u8;
            bitmap_info.width as usize
                * 4usize
                * std::mem::size_of::<u8>()
                * bitmap_info.height as usize
        ];

        copy_image::<u8>(
            &bitmap_info.data,
            bitmap_info.stride,
            &mut source_buffer,
            (bitmap_info.width as usize * 4usize * std::mem::size_of::<u8>()) as u32,
            bitmap_info.width,
            bitmap_info.height,
            4u32,
        );
        let source_store = ImageStore::<u8, 4>::new(
            source_buffer,
            bitmap_info.width as usize,
            bitmap_info.height as usize,
        )
        .unwrap();
        let new_size = ImageSize::new(new_width as usize, new_height as usize);
        let new_image_store = scaler.resize_rgba(new_size, source_store, false).unwrap();
        let bytes = new_image_store.as_bytes();
        match android_bitmap::create_bitmap(
            &mut env,
            &bytes,
            new_size.width as u32 * 4,
            new_size.width as u32,
            new_size.height as u32,
        ) {
            Ok(android_bitmap) => android_bitmap.as_raw(),
            Err(error) => {
                let clazz = env
                    .find_class("java/lang/Exception")
                    .expect("Found exception class");
                env.throw_new(clazz, error).expect("Failed to access JNI");
                JObject::default().as_raw()
            }
        }
    }
}
