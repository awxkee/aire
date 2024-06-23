#[cfg(target_os = "android")]
#[allow(non_snake_case)]
pub mod android {
    extern crate jni;

    use jni::sys::{jfloat, jint, jobject};
    use jni::JNIEnv;
    use libblur::{EdgeMode, FastBlurChannels, ThreadingPolicy};

    use crate::bitmap_helper::android_bitmap;
    use crate::transfer_resolve::param_into_transfer;

    #[no_mangle]
    pub unsafe extern "system" fn Java_com_awxkee_aire_pipeline_BlurPipelinesImpl_gaussianBoxBlurImpl(
        mut env: JNIEnv,
        _: jobject,
        bitmap: jobject,
        radius: jint,
    ) -> jobject {
        if radius <= 0 {
            let clazz = env
                .find_class("java/lang/Exception")
                .expect("Found exception class");
            env.throw_new(clazz, "Radius must be more than 0")
                .expect("Failed to access JNI");
            return bitmap;
        }

        let bitmap_info = android_bitmap::get_bitmap_rgba8888(&mut env, bitmap);
        match bitmap_info {
            Ok(info) => {
                let mut dst_vec: Vec<u8> = vec![0u8; info.stride as usize * info.height as usize];
                libblur::gaussian_box_blur(
                    &info.data,
                    info.stride,
                    &mut dst_vec,
                    info.stride,
                    info.width,
                    info.height,
                    radius as u32,
                    FastBlurChannels::Channels4,
                    ThreadingPolicy::Adaptive,
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
    pub unsafe extern "system" fn Java_com_awxkee_aire_pipeline_BlurPipelinesImpl_fastGaussianNextLinearImpl(
        mut env: JNIEnv,
        _: jobject,
        bitmap: jobject,
        radius: jint,
        transfer: jint,
    ) -> jobject {
        if radius <= 0 {
            let clazz = env
                .find_class("java/lang/Exception")
                .expect("Found exception class");
            env.throw_new(clazz, "Radius must be more than 0")
                .expect("Failed to access JNI");
            return bitmap;
        }

        let transfer = match param_into_transfer(transfer) {
            Ok(transfer) => transfer,
            Err(err) => {
                let clazz = env
                    .find_class("java/lang/Exception")
                    .expect("Found exception class");
                env.throw_new(clazz, err.to_string())
                    .expect("Failed to access JNI");
                return bitmap;
            }
        };

        let bitmap_info = android_bitmap::get_bitmap_rgba8888(&mut env, bitmap);
        match bitmap_info {
            Ok(mut info) => {
                libblur::fast_gaussian_next_in_linear(
                    &mut info.data,
                    info.stride,
                    info.width,
                    info.height,
                    radius as u32,
                    FastBlurChannels::Channels4,
                    ThreadingPolicy::Adaptive,
                    transfer,
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
    pub unsafe extern "system" fn Java_com_awxkee_aire_pipeline_BlurPipelinesImpl_fastGaussianLinearImpl(
        mut env: JNIEnv,
        _: jobject,
        bitmap: jobject,
        radius: jint,
        transfer: jint,
    ) -> jobject {
        if radius <= 0 {
            let clazz = env
                .find_class("java/lang/Exception")
                .expect("Found exception class");
            env.throw_new(clazz, "Radius must be more than 0")
                .expect("Failed to access JNI");
            return bitmap;
        }

        let transfer = match param_into_transfer(transfer) {
            Ok(transfer) => transfer,
            Err(err) => {
                let clazz = env
                    .find_class("java/lang/Exception")
                    .expect("Found exception class");
                env.throw_new(clazz, err.to_string())
                    .expect("Failed to access JNI");
                return bitmap;
            }
        };

        let bitmap_info = android_bitmap::get_bitmap_rgba8888(&mut env, bitmap);
        match bitmap_info {
            Ok(mut info) => {
                libblur::fast_gaussian_in_linear(
                    &mut info.data,
                    info.stride,
                    info.width,
                    info.height,
                    radius as u32,
                    FastBlurChannels::Channels4,
                    ThreadingPolicy::Adaptive,
                    transfer,
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
    pub unsafe extern "system" fn Java_com_awxkee_aire_pipeline_BlurPipelinesImpl_gaussianBlurLinearImpl(
        mut env: JNIEnv,
        _: jobject,
        bitmap: jobject,
        kernel_size: jint,
        sigma: jfloat,
        kernel_mode: jint,
        transfer: jint,
    ) -> jobject {
        if kernel_size <= 0 {
            let clazz = env
                .find_class("java/lang/Exception")
                .expect("Found exception class");
            env.throw_new(clazz, "Radius must be more than 0")
                .expect("Failed to access JNI");
            return bitmap;
        }

        if kernel_size % 2 == 0 {
            let clazz = env
                .find_class("java/lang/Exception")
                .expect("Found exception class");
            env.throw_new(clazz, "Kernel size must be odd")
                .expect("Failed to access JNI");
            return bitmap;
        }

        if sigma <= 0f32 {
            let clazz = env
                .find_class("java/lang/Exception")
                .expect("Found exception class");
            env.throw_new(clazz, "Sigma must be more than 0")
                .expect("Failed to access JNI");
            return bitmap;
        }

        let transfer = match param_into_transfer(transfer) {
            Ok(transfer) => transfer,
            Err(err) => {
                let clazz = env
                    .find_class("java/lang/Exception")
                    .expect("Found exception class");
                env.throw_new(clazz, err.to_string())
                    .expect("Failed to access JNI");
                return bitmap;
            }
        };

        let bitmap_info = android_bitmap::get_bitmap_rgba8888(&mut env, bitmap);
        match bitmap_info {
            Ok(info) => {
                let mut dst_vec: Vec<u8> = vec![0u8; info.stride as usize * info.height as usize];
                let edge_mode: EdgeMode = (kernel_mode as usize).into();
                libblur::gaussian_blur_in_linear(
                    &info.data,
                    info.stride,
                    &mut dst_vec,
                    info.stride,
                    info.width,
                    info.height,
                    kernel_size as u32,
                    sigma,
                    FastBlurChannels::Channels4,
                    edge_mode,
                    ThreadingPolicy::Adaptive,
                    transfer,
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
    pub unsafe extern "system" fn Java_com_awxkee_aire_pipeline_BlurPipelinesImpl_stackBlurLinearImpl(
        mut env: JNIEnv,
        _: jobject,
        bitmap: jobject,
        radius: jint,
        transfer: jint,
    ) -> jobject {
        if radius <= 0 {
            let clazz = env
                .find_class("java/lang/Exception")
                .expect("Found exception class");
            env.throw_new(clazz, "Radius must be more than 0")
                .expect("Failed to access JNI");
            return bitmap;
        }

        let transfer = match param_into_transfer(transfer) {
            Ok(transfer) => transfer,
            Err(err) => {
                let clazz = env
                    .find_class("java/lang/Exception")
                    .expect("Found exception class");
                env.throw_new(clazz, err.to_string())
                    .expect("Failed to access JNI");
                return bitmap;
            }
        };

        let bitmap_info = android_bitmap::get_bitmap_rgba8888(&mut env, bitmap);
        match bitmap_info {
            Ok(mut info) => {
                libblur::stack_blur_in_linear(
                    &mut info.data,
                    info.stride,
                    info.width,
                    info.height,
                    radius as u32,
                    FastBlurChannels::Channels4,
                    ThreadingPolicy::Adaptive,
                    transfer,
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
    pub unsafe extern "system" fn Java_com_awxkee_aire_pipeline_BlurPipelinesImpl_stackBlurImpl(
        mut env: JNIEnv,
        _: jobject,
        bitmap: jobject,
        radius: jint,
    ) -> jobject {
        if radius <= 0 {
            let clazz = env
                .find_class("java/lang/Exception")
                .expect("Found exception class");
            env.throw_new(clazz, "Radius must be more than 0")
                .expect("Failed to access JNI");
            return bitmap;
        }

        let bitmap_info = android_bitmap::get_bitmap_rgba8888(&mut env, bitmap);
        match bitmap_info {
            Ok(mut info) => {
                libblur::stack_blur(
                    &mut info.data,
                    info.stride,
                    info.width,
                    info.height,
                    radius as u32,
                    FastBlurChannels::Channels4,
                    ThreadingPolicy::Adaptive,
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
    pub unsafe extern "system" fn Java_com_awxkee_aire_pipeline_BlurPipelinesImpl_boxBlurImpl(
        mut env: JNIEnv,
        _: jobject,
        bitmap: jobject,
        kernel_size: jint,
    ) -> jobject {
        if kernel_size <= 0 {
            let clazz = env
                .find_class("java/lang/Exception")
                .expect("Found exception class");
            env.throw_new(clazz, "Radius must be more than 0")
                .expect("Failed to access JNI");
            return bitmap;
        }

        let bitmap_info = android_bitmap::get_bitmap_rgba8888(&mut env, bitmap);
        match bitmap_info {
            Ok(info) => {
                let mut dst_vec: Vec<u8> = vec![0u8; info.stride as usize * info.height as usize];
                libblur::box_blur(
                    &info.data,
                    info.stride,
                    &mut dst_vec,
                    info.stride,
                    info.width,
                    info.height,
                    kernel_size as u32,
                    FastBlurChannels::Channels4,
                    ThreadingPolicy::Adaptive,
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
    pub unsafe extern "system" fn Java_com_awxkee_aire_pipeline_BlurPipelinesImpl_tentBlurImpl(
        mut env: JNIEnv,
        _: jobject,
        bitmap: jobject,
        radius: jint,
    ) -> jobject {
        if radius <= 0 {
            let clazz = env
                .find_class("java/lang/Exception")
                .expect("Found exception class");
            env.throw_new(clazz, "Radius must be more than 0")
                .expect("Failed to access JNI");
            return bitmap;
        }

        let bitmap_info = android_bitmap::get_bitmap_rgba8888(&mut env, bitmap);
        match bitmap_info {
            Ok(info) => {
                let mut dst_vec: Vec<u8> = vec![0u8; info.stride as usize * info.height as usize];
                libblur::tent_blur(
                    &info.data,
                    info.stride,
                    &mut dst_vec,
                    info.stride,
                    info.width,
                    info.height,
                    radius as u32,
                    FastBlurChannels::Channels4,
                    ThreadingPolicy::Adaptive,
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
    pub unsafe extern "system" fn Java_com_awxkee_aire_pipeline_BlurPipelinesImpl_medianBlurImpl(
        mut env: JNIEnv,
        _: jobject,
        bitmap: jobject,
        radius: jint,
    ) -> jobject {
        if radius <= 0 {
            let clazz = env
                .find_class("java/lang/Exception")
                .expect("Found exception class");
            env.throw_new(clazz, "Radius must be more than 0")
                .expect("Failed to access JNI");
            return bitmap;
        }

        let bitmap_info = android_bitmap::get_bitmap_rgba8888(&mut env, bitmap);
        match bitmap_info {
            Ok(info) => {
                let mut dst_vec: Vec<u8> = vec![0u8; info.stride as usize * info.height as usize];
                libblur::median_blur(
                    &info.data,
                    info.stride,
                    &mut dst_vec,
                    info.stride,
                    info.width,
                    info.height,
                    radius as u32,
                    FastBlurChannels::Channels4,
                    ThreadingPolicy::Adaptive,
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
    pub unsafe extern "system" fn Java_com_awxkee_aire_pipeline_BlurPipelinesImpl_gaussianBlurImpl(
        mut env: JNIEnv,
        _: jobject,
        bitmap: jobject,
        kernel_size: jint,
        sigma: jfloat,
        kernel_mode: jint,
    ) -> jobject {
        if kernel_size <= 0 {
            let clazz = env
                .find_class("java/lang/Exception")
                .expect("Found exception class");
            env.throw_new(clazz, "Radius must be more than 0")
                .expect("Failed to access JNI");
            return bitmap;
        }

        if kernel_size % 2 == 0 {
            let clazz = env
                .find_class("java/lang/Exception")
                .expect("Found exception class");
            env.throw_new(clazz, "Kernel size must be odd")
                .expect("Failed to access JNI");
            return bitmap;
        }

        if sigma <= 0f32 {
            let clazz = env
                .find_class("java/lang/Exception")
                .expect("Found exception class");
            env.throw_new(clazz, "Sigma must be more than 0")
                .expect("Failed to access JNI");
            return bitmap;
        }

        let bitmap_info = android_bitmap::get_bitmap_rgba8888(&mut env, bitmap);
        match bitmap_info {
            Ok(info) => {
                let mut dst_vec: Vec<u8> = vec![0u8; info.stride as usize * info.height as usize];
                let edge_mode: EdgeMode = (kernel_mode as usize).into();
                libblur::gaussian_blur(
                    &info.data,
                    info.stride,
                    &mut dst_vec,
                    info.stride,
                    info.width,
                    info.height,
                    kernel_size as u32,
                    sigma,
                    FastBlurChannels::Channels4,
                    edge_mode,
                    ThreadingPolicy::Adaptive,
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
    pub unsafe extern "system" fn Java_com_awxkee_aire_pipeline_BlurPipelinesImpl_fastGaussianImpl(
        mut env: JNIEnv,
        _: jobject,
        bitmap: jobject,
        radius: jint,
    ) -> jobject {
        if radius <= 0 {
            let clazz = env
                .find_class("java/lang/Exception")
                .expect("Found exception class");
            env.throw_new(clazz, "Radius must be more than 0")
                .expect("Failed to access JNI");
            return bitmap;
        }

        let bitmap_info = android_bitmap::get_bitmap_rgba8888(&mut env, bitmap);
        match bitmap_info {
            Ok(mut info) => {
                libblur::fast_gaussian(
                    &mut info.data,
                    info.stride,
                    info.width,
                    info.height,
                    radius as u32,
                    FastBlurChannels::Channels4,
                    ThreadingPolicy::Adaptive,
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
    pub unsafe extern "system" fn Java_com_awxkee_aire_pipeline_BlurPipelinesImpl_fastGaussianNextImpl(
        mut env: JNIEnv,
        _: jobject,
        bitmap: jobject,
        radius: jint,
    ) -> jobject {
        if radius <= 0 {
            let clazz = env
                .find_class("java/lang/Exception")
                .expect("Found exception class");
            env.throw_new(clazz, "Radius must be more than 0")
                .expect("Failed to access JNI");
            return bitmap;
        }

        let bitmap_info = android_bitmap::get_bitmap_rgba8888(&mut env, bitmap);
        match bitmap_info {
            Ok(mut info) => {
                libblur::fast_gaussian_next(
                    &mut info.data,
                    info.stride,
                    info.width,
                    info.height,
                    radius as u32,
                    FastBlurChannels::Channels4,
                    ThreadingPolicy::Adaptive,
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
