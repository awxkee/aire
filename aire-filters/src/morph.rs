use fast_morphology::{morphology_rgb, morphology_rgba, BorderMode, ImageSize, KernelShape, MorphExOp, MorphScalar, MorphologyThreadingPolicy};

#[repr(C)]
#[derive(Copy, Clone, PartialOrd, Eq, PartialEq, Hash, Debug)]
pub enum MorphOpMode {
    Rgb = 0,
    Rgba = 1,
}

pub fn morph_op_mode_from_java(value: i32) -> Result<MorphOpMode, String> {
    match value {
        0 => Ok(MorphOpMode::Rgb),
        1 => Ok(MorphOpMode::Rgba),
        _ => Err(format!(
            "Value {} is not supported for op mode in morphology",
            value
        )),
    }
}

pub fn border_mode_from_java(value: i32) -> Result<BorderMode, String> {
    match value {
        0 => Ok(BorderMode::Clamp),
        1 => Ok(BorderMode::Wrap),
        2 => Ok(BorderMode::Reflect),
        3 => Ok(BorderMode::Reflect101),
        4 => Ok(BorderMode::Constant),
        _ => Err(format!(
            "Value {} is not supported for border mode in morphology",
            value
        )),
    }
}

pub fn morph_op_from_java(value: i32) -> Result<MorphExOp, String> {
    match value {
        0 => Ok(MorphExOp::Dilate),
        1 => Ok(MorphExOp::Erode),
        2 => Ok(MorphExOp::Opening),
        3 => Ok(MorphExOp::Closing),
        4 => Ok(MorphExOp::Gradient),
        5 => Ok(MorphExOp::TopHat),
        6 => Ok(MorphExOp::BlackHat),
        _ => Err(format!(
            "Value {} is not supported for op ex in morphology",
            value
        )),
    }
}

pub fn perform_morph(
    rgba_image: &[u8],
    morph_op: i32,
    width: usize,
    height: usize,
    border_mode: i32,
    color_mode: i32,
    se: &[u8],
    se_width: i32,
    se_height: i32,
    scalar: MorphScalar,
) -> Result<Vec<u8>, String> {
    let border_mode = border_mode_from_java(border_mode)?;
    let morph_op_mode = morph_op_mode_from_java(color_mode)?;
    let morph_ex = morph_op_from_java(morph_op)?;

    if se_width < 0 || se_height < 0 || (se_width == 0 && se_height == 0) {
        return Err("Kernel dimensions must be >= 0".parse().unwrap());
    }

    let image_size = ImageSize::new(width, height);
    let kernel_shape = KernelShape::new(se_width as usize, se_height as usize);

    match morph_op_mode {
        MorphOpMode::Rgb => {
            let mut rgb_image = vec![0u8; width * height * 3];
            for (chunk_rgba, chunk_rgb) in rgba_image
                .chunks_exact(4)
                .zip(rgb_image.chunks_exact_mut(3))
            {
                chunk_rgb[0] = chunk_rgba[0];
                chunk_rgb[1] = chunk_rgba[1];
                chunk_rgb[2] = chunk_rgba[2];
            }
            let mut dst_image = vec![0u8; width * height * 3];
            morphology_rgb(
                &rgb_image,
                &mut dst_image,
                morph_ex,
                image_size,
                se,
                kernel_shape,
                border_mode,
                scalar,
                MorphologyThreadingPolicy::default(),
            )?;
            rgb_image.resize(0, 0);
            let mut new_rgba_image = vec![0u8; width * height * 4];
            for (chunk_rgba, chunk_rgb) in new_rgba_image
                .chunks_exact_mut(4)
                .zip(dst_image.chunks_exact(3))
            {
                chunk_rgba[0] = chunk_rgb[0];
                chunk_rgba[1] = chunk_rgb[1];
                chunk_rgba[2] = chunk_rgb[2];
                chunk_rgba[3] = 255;
            }
            Ok(new_rgba_image)
        }
        MorphOpMode::Rgba => {
            let mut dst_image = vec![0u8; width * height * 4];
            morphology_rgba(
                &rgba_image,
                &mut dst_image,
                morph_ex,
                image_size,
                se,
                kernel_shape,
                border_mode,
                scalar,
                MorphologyThreadingPolicy::default(),
            )?;
            Ok(dst_image)
        }
    }
}

#[cfg(target_os = "android")]
#[allow(non_snake_case)]
pub mod android {
    extern crate jni;

    use fast_morphology::MorphScalar;
    use crate::android_bitmap::copy_image;
    use crate::bitmap_helper::android_bitmap;
    use crate::morph::perform_morph;
    use jni::objects::{JIntArray, JObject};
    use jni::sys::{jint, jintArray, jobject};
    use jni::JNIEnv;
    use crate::scalar::android::get_scalar_from_java;

    #[no_mangle]
    pub unsafe extern "system" fn Java_com_awxkee_aire_pipeline_BasePipelinesImpl_morphologyImpl(
        mut env: JNIEnv,
        _: jobject,
        bitmap: jobject,
        morph_op: jint,
        morph_op_mode: jint,
        border_mode: jint,
        border_constant: jobject,
        kernel: jintArray,
        kernel_width: jint,
        kernel_height: jint,
    ) -> jobject {
        let array = JIntArray::from_raw(kernel);
        let array_length = match env.get_array_length(&array) {
            Ok(length) => length,
            Err(_) => {
                let clazz = env
                    .find_class("java/lang/Exception")
                    .expect("Found exception class");
                env.throw_new(clazz, "Can't get kernel length")
                    .expect("Failed to access JNI");
                return JObject::null().as_raw();
            }
        };
        if kernel_width * kernel_height != array_length {
            let clazz = env
                .find_class("java/lang/Exception")
                .expect("Found exception class");
            env.throw_new(clazz, "Kernel length do not matches provided bounds")
                .expect("Failed to access JNI");
            return JObject::null().as_raw();
        }
        let mut se_java = vec![0i32; array_length as usize];
        env.get_int_array_region(&array, 0, &mut se_java)
            .expect("Successfully copied origin");
        let mut bitmap_info = match android_bitmap::get_bitmap_rgba8888(&mut env, bitmap) {
            Ok(info) => info,
            Err(err) => {
                let clazz = env
                    .find_class("java/lang/Exception")
                    .expect("Found exception class");
                env.throw_new(clazz, err).expect("Failed to access JNI");
                return JObject::null().as_raw();
            }
        };

        let reworked_se = se_java.iter().map(|&x| x as u8).collect::<Vec<_>>();

        let mut new_bitmap =
            vec![0u8; bitmap_info.height as usize * bitmap_info.width as usize * 4];
        copy_image::<u8>(
            &bitmap_info.data,
            bitmap_info.stride,
            &mut new_bitmap,
            (bitmap_info.width as usize * 4) as u32,
            bitmap_info.width,
            bitmap_info.height,
            4u32,
        );
        bitmap_info.data.resize(0, 0);

        let border_scalar = get_scalar_from_java(&mut env, border_constant);

        let morph_applied_image = match perform_morph(
            &new_bitmap,
            morph_op,
            bitmap_info.width as usize,
            bitmap_info.height as usize,
            border_mode,
            morph_op_mode,
            &reworked_se,
            kernel_width,
            kernel_height,
            MorphScalar::new(border_scalar.v0, border_scalar.v1, border_scalar.v2, border_scalar.v3),
        ) {
            Ok(img) => img,
            Err(err) => {
                let clazz = env
                    .find_class("java/lang/Exception")
                    .expect("Found exception class");
                env.throw_new(clazz, err).expect("Failed to access JNI");
                return JObject::null().as_raw();
            }
        };

        let new_bitmap_r = android_bitmap::create_bitmap(
            &mut env,
            &morph_applied_image,
            (bitmap_info.width as usize * 4usize * std::mem::size_of::<u8>()) as u32,
            bitmap_info.width,
            bitmap_info.height,
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
