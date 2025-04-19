#[cfg(target_os = "android")]
#[allow(non_snake_case)]
pub mod android {
    extern crate jni;

    use crate::android_bitmap::copy_image;
    use crate::bitmap_helper::android_bitmap;
    use crate::scalar::android::get_scalar_from_java;
    use crate::transfer_resolve::param_into_transfer;
    use jni::objects::{JFloatArray, JObject};
    use jni::sys::{jfloat, jfloatArray, jint, jobject};
    use jni::JNIEnv;
    use libblur::{
        BlurImage, BlurImageMut, BufferStore, ConvolutionMode, EdgeMode, FastBlurChannels,
        ImageSize, KernelShape, ThreadingPolicy,
    };
    #[no_mangle]
    pub unsafe extern "system" fn Java_com_awxkee_aire_pipeline_BlurPipelinesImpl_gaussianBoxBlurLinearImpl(
        mut env: JNIEnv,
        _: jobject,
        bitmap: jobject,
        sigma: jfloat,
        transfer: jint,
    ) -> jobject {
        if sigma <= 0. {
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
            Ok(info) => {
                let mut dst_vec: Vec<u8> = vec![0u8; info.stride as usize * info.height as usize];

                let src_image = BlurImage {
                    data: std::borrow::Cow::Borrowed(&info.data),
                    width: info.width,
                    height: info.height,
                    stride: info.stride,
                    channels: FastBlurChannels::Channels4,
                };

                let mut dst_image = BlurImageMut {
                    data: BufferStore::Borrowed(&mut dst_vec),
                    width: info.width,
                    height: info.height,
                    stride: info.stride,
                    channels: FastBlurChannels::Channels4,
                };

                libblur::gaussian_box_blur_in_linear(
                    &src_image,
                    &mut dst_image,
                    sigma,
                    ThreadingPolicy::Adaptive,
                    transfer,
                )
                .unwrap();

                let new_bitmap_r = android_bitmap::create_bitmap(
                    &mut env,
                    &dst_vec,
                    info.stride,
                    info.width,
                    info.height,
                );

                match new_bitmap_r {
                    Ok(new_bitmap) => new_bitmap.as_raw(),
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
    pub unsafe extern "system" fn Java_com_awxkee_aire_pipeline_BlurPipelinesImpl_tentBlurLinearImpl(
        mut env: JNIEnv,
        _: jobject,
        bitmap: jobject,
        sigma: jfloat,
        transfer: jint,
    ) -> jobject {
        if sigma <= 0. {
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
            Ok(info) => {
                let mut dst_vec: Vec<u8> = vec![0u8; info.stride as usize * info.height as usize];
                let mut dst_image = BlurImageMut {
                    data: BufferStore::Borrowed(&mut dst_vec),
                    width: info.width,
                    height: info.height,
                    stride: info.stride,
                    channels: FastBlurChannels::Channels4,
                };
                let src_image = BlurImage {
                    data: std::borrow::Cow::Borrowed(&info.data),
                    width: info.width,
                    height: info.height,
                    stride: info.stride,
                    channels: FastBlurChannels::Channels4,
                };
                libblur::tent_blur_in_linear(
                    &src_image,
                    &mut dst_image,
                    sigma,
                    ThreadingPolicy::Adaptive,
                    transfer,
                )
                .unwrap();

                let new_bitmap_r = android_bitmap::create_bitmap(
                    &mut env,
                    &dst_vec,
                    info.stride,
                    info.width,
                    info.height,
                );

                match new_bitmap_r {
                    Ok(new_bitmap) => new_bitmap.as_raw(),
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
    pub unsafe extern "system" fn Java_com_awxkee_aire_pipeline_BlurPipelinesImpl_boxBlurLinearImpl(
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
            Ok(info) => {
                let mut dst_vec: Vec<u8> = vec![0u8; info.stride as usize * info.height as usize];

                let src_image = BlurImage {
                    data: std::borrow::Cow::Borrowed(&info.data),
                    width: info.width,
                    height: info.height,
                    stride: info.stride,
                    channels: FastBlurChannels::Channels4,
                };

                let mut dst_image = BlurImageMut {
                    data: BufferStore::Borrowed(&mut dst_vec),
                    width: info.width,
                    height: info.height,
                    stride: info.stride,
                    channels: FastBlurChannels::Channels4,
                };

                libblur::box_blur_in_linear(
                    &src_image,
                    &mut dst_image,
                    radius as u32,
                    ThreadingPolicy::Adaptive,
                    transfer,
                )
                .unwrap();

                let new_bitmap_r = android_bitmap::create_bitmap(
                    &mut env,
                    &dst_vec,
                    info.stride,
                    info.width,
                    info.height,
                );

                match new_bitmap_r {
                    Ok(new_bitmap) => new_bitmap.as_raw(),
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
    pub unsafe extern "system" fn Java_com_awxkee_aire_pipeline_BlurPipelinesImpl_gaussianBoxBlurImpl(
        mut env: JNIEnv,
        _: jobject,
        bitmap: jobject,
        sigma: jfloat,
    ) -> jobject {
        if sigma <= 0. {
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

                let src_image = BlurImage {
                    data: std::borrow::Cow::Borrowed(&info.data),
                    width: info.width,
                    height: info.height,
                    stride: info.stride,
                    channels: FastBlurChannels::Channels4,
                };

                let mut dst_image = BlurImageMut {
                    data: BufferStore::Borrowed(&mut dst_vec),
                    width: info.width,
                    height: info.height,
                    stride: info.stride,
                    channels: FastBlurChannels::Channels4,
                };

                libblur::gaussian_box_blur(
                    &src_image,
                    &mut dst_image,
                    sigma,
                    ThreadingPolicy::Adaptive,
                )
                .unwrap();

                let new_bitmap_r = android_bitmap::create_bitmap(
                    &mut env,
                    &dst_vec,
                    info.stride,
                    info.width,
                    info.height,
                );

                match new_bitmap_r {
                    Ok(new_bitmap) => new_bitmap.as_raw(),
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
        edge_mode: jint,
    ) -> jobject {
        if radius <= 0 {
            let clazz = env
                .find_class("java/lang/Exception")
                .expect("Found exception class");
            env.throw_new(clazz, "Radius must be more than 0")
                .expect("Failed to access JNI");
            return bitmap;
        }

        let edge_mode: EdgeMode = (edge_mode as usize).into();
        if edge_mode == EdgeMode::Constant {
            let clazz = env
                .find_class("java/lang/Exception")
                .expect("Found exception class");
            env.throw_new(clazz, "Constant is not supported in this blur")
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
                let mut dst_image = BlurImageMut {
                    data: BufferStore::Borrowed(&mut info.data),
                    width: info.width,
                    height: info.height,
                    stride: info.stride,
                    channels: FastBlurChannels::Channels4,
                };

                libblur::fast_gaussian_next_in_linear(
                    &mut dst_image,
                    radius as u32,
                    ThreadingPolicy::Adaptive,
                    transfer,
                    edge_mode,
                )
                .unwrap();

                let new_bitmap_r = android_bitmap::create_bitmap(
                    &mut env,
                    &info.data,
                    info.stride,
                    info.width,
                    info.height,
                );

                match new_bitmap_r {
                    Ok(new_bitmap) => new_bitmap.as_raw(),
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
        edge_mode: jint,
    ) -> jobject {
        if radius <= 0 {
            let clazz = env
                .find_class("java/lang/Exception")
                .expect("Found exception class");
            env.throw_new(clazz, "Radius must be more than 0")
                .expect("Failed to access JNI");
            return bitmap;
        }

        let edge_mode: EdgeMode = (edge_mode as usize).into();
        if edge_mode == EdgeMode::Constant {
            let clazz = env
                .find_class("java/lang/Exception")
                .expect("Found exception class");
            env.throw_new(clazz, "Constant is not supported in this blur")
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
                let mut dst_image = BlurImageMut {
                    data: BufferStore::Borrowed(&mut info.data),
                    width: info.width,
                    height: info.height,
                    stride: info.stride,
                    channels: FastBlurChannels::Channels4,
                };

                libblur::fast_gaussian_in_linear(
                    &mut dst_image,
                    radius as u32,
                    ThreadingPolicy::Adaptive,
                    transfer,
                    edge_mode,
                )
                .unwrap();

                let new_bitmap_r = android_bitmap::create_bitmap(
                    &mut env,
                    &info.data,
                    info.stride,
                    info.width,
                    info.height,
                );

                match new_bitmap_r {
                    Ok(new_bitmap) => new_bitmap.as_raw(),
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

                let src_image = BlurImage {
                    data: std::borrow::Cow::Borrowed(&info.data),
                    width: info.width,
                    height: info.height,
                    stride: info.stride,
                    channels: FastBlurChannels::Channels4,
                };

                let mut dst_image = BlurImageMut {
                    data: BufferStore::Borrowed(&mut dst_vec),
                    width: info.width,
                    height: info.height,
                    stride: info.stride,
                    channels: FastBlurChannels::Channels4,
                };

                libblur::gaussian_blur_in_linear(
                    &src_image,
                    &mut dst_image,
                    kernel_size as u32,
                    sigma,
                    edge_mode,
                    ThreadingPolicy::Adaptive,
                    transfer,
                )
                .unwrap();

                let new_bitmap_r = android_bitmap::create_bitmap(
                    &mut env,
                    &dst_vec,
                    info.stride,
                    info.width,
                    info.height,
                );

                match new_bitmap_r {
                    Ok(new_bitmap) => new_bitmap.as_raw(),
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
                let mut dst_image = BlurImageMut {
                    data: BufferStore::Borrowed(&mut info.data),
                    width: info.width,
                    height: info.height,
                    stride: info.stride,
                    channels: FastBlurChannels::Channels4,
                };

                libblur::stack_blur_in_linear(
                    &mut dst_image,
                    radius as u32,
                    ThreadingPolicy::Adaptive,
                    transfer,
                )
                .unwrap();

                let new_bitmap_r = android_bitmap::create_bitmap(
                    &mut env,
                    &info.data,
                    info.stride,
                    info.width,
                    info.height,
                );

                match new_bitmap_r {
                    Ok(new_bitmap) => new_bitmap.as_raw(),
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
                let mut dst_image = BlurImageMut {
                    data: BufferStore::Borrowed(&mut info.data),
                    width: info.width,
                    height: info.height,
                    stride: info.stride,
                    channels: FastBlurChannels::Channels4,
                };

                libblur::stack_blur(&mut dst_image, radius as u32, ThreadingPolicy::Adaptive)
                    .unwrap();

                let new_bitmap_r = android_bitmap::create_bitmap(
                    &mut env,
                    &info.data,
                    info.stride,
                    info.width,
                    info.height,
                );

                match new_bitmap_r {
                    Ok(new_bitmap) => new_bitmap.as_raw(),
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

                let src_image = BlurImage {
                    data: std::borrow::Cow::Borrowed(&info.data),
                    width: info.width,
                    height: info.height,
                    stride: info.stride,
                    channels: FastBlurChannels::Channels4,
                };

                let mut dst_image = BlurImageMut {
                    data: BufferStore::Borrowed(&mut dst_vec),
                    width: info.width,
                    height: info.height,
                    stride: info.stride,
                    channels: FastBlurChannels::Channels4,
                };

                libblur::box_blur(
                    &src_image,
                    &mut dst_image,
                    kernel_size as u32,
                    ThreadingPolicy::Adaptive,
                )
                .unwrap();

                let new_bitmap_r = android_bitmap::create_bitmap(
                    &mut env,
                    &dst_vec,
                    info.stride,
                    info.width,
                    info.height,
                );

                match new_bitmap_r {
                    Ok(new_bitmap) => new_bitmap.as_raw(),
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
        sigma: jfloat,
    ) -> jobject {
        if sigma <= 0. {
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

                let src_image = BlurImage {
                    data: std::borrow::Cow::Borrowed(&info.data),
                    width: info.width,
                    height: info.height,
                    stride: info.stride,
                    channels: FastBlurChannels::Channels4,
                };

                let mut dst_image = BlurImageMut {
                    data: BufferStore::Borrowed(&mut dst_vec),
                    width: info.width,
                    height: info.height,
                    stride: info.stride,
                    channels: FastBlurChannels::Channels4,
                };

                libblur::tent_blur(&src_image, &mut dst_image, sigma, ThreadingPolicy::Adaptive)
                    .unwrap();

                let new_bitmap_r = android_bitmap::create_bitmap(
                    &mut env,
                    &dst_vec,
                    info.stride,
                    info.width,
                    info.height,
                );

                match new_bitmap_r {
                    Ok(new_bitmap) => new_bitmap.as_raw(),
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

                let src_image = BlurImage {
                    data: std::borrow::Cow::Borrowed(&info.data),
                    width: info.width,
                    height: info.height,
                    stride: info.stride,
                    channels: FastBlurChannels::Channels4,
                };

                let mut dst_image = BlurImageMut {
                    data: BufferStore::Borrowed(&mut dst_vec),
                    width: info.width,
                    height: info.height,
                    stride: info.stride,
                    channels: FastBlurChannels::Channels4,
                };

                libblur::median_blur(
                    &src_image,
                    &mut dst_image,
                    radius as u32,
                    ThreadingPolicy::Adaptive,
                )
                .unwrap();

                let new_bitmap_r = android_bitmap::create_bitmap(
                    &mut env,
                    &dst_vec,
                    info.stride,
                    info.width,
                    info.height,
                );

                match new_bitmap_r {
                    Ok(new_bitmap) => new_bitmap.as_raw(),
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
        edge_mode: jint,
        precise_level: jint,
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

        let precise_level = match precise_level {
            0 => ConvolutionMode::Exact,
            1 => ConvolutionMode::FixedPoint,
            _ => {
                let clazz = env
                    .find_class("java/lang/Exception")
                    .expect("Found exception class");
                env.throw_new(
                    clazz,
                    format!(
                        "Unknown approximation level {} was requested",
                        precise_level
                    ),
                )
                .expect("Failed to access JNI");
                return bitmap;
            }
        };

        let bitmap_info = android_bitmap::get_bitmap_rgba8888(&mut env, bitmap);
        match bitmap_info {
            Ok(info) => {
                let mut src_vec = vec![0u8; info.width as usize * 4 * info.height as usize];

                for (src_chunk, dst_chunk) in info
                    .data
                    .chunks_exact(info.stride as usize)
                    .zip(src_vec.chunks_exact_mut(4 * info.width as usize))
                {
                    for (src_layout, dst_layout) in src_chunk.iter().zip(dst_chunk.iter_mut()) {
                        *dst_layout = *src_layout;
                    }
                }

                let mut dst_vec: Vec<u8> =
                    vec![0u8; info.width as usize * 4 * info.height as usize];
                let edge_mode: EdgeMode = (edge_mode as usize).into();

                let src_image = BlurImage {
                    data: std::borrow::Cow::Borrowed(&info.data),
                    width: info.width,
                    height: info.height,
                    stride: info.stride,
                    channels: FastBlurChannels::Channels4,
                };

                let mut dst_image = BlurImageMut {
                    data: BufferStore::Borrowed(&mut dst_vec),
                    width: info.width,
                    height: info.height,
                    stride: info.stride,
                    channels: FastBlurChannels::Channels4,
                };

                libblur::gaussian_blur(
                    &src_image,
                    &mut dst_image,
                    kernel_size as u32,
                    sigma,
                    edge_mode,
                    ThreadingPolicy::Adaptive,
                    precise_level,
                )
                .unwrap();

                let new_bitmap_r = android_bitmap::create_bitmap(
                    &mut env,
                    &dst_vec,
                    info.width * 4,
                    info.width,
                    info.height,
                );

                match new_bitmap_r {
                    Ok(new_bitmap) => new_bitmap.as_raw(),
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
        edge_mode: jint,
    ) -> jobject {
        if radius <= 0 {
            let clazz = env
                .find_class("java/lang/Exception")
                .expect("Found exception class");
            env.throw_new(clazz, "Radius must be more than 0")
                .expect("Failed to access JNI");
            return bitmap;
        }

        let edge_mode: EdgeMode = (edge_mode as usize).into();
        if edge_mode == EdgeMode::Constant {
            let clazz = env
                .find_class("java/lang/Exception")
                .expect("Found exception class");
            env.throw_new(clazz, "Kernel clip is not supported in this blur")
                .expect("Failed to access JNI");
            return bitmap;
        }

        let bitmap_info = android_bitmap::get_bitmap_rgba8888(&mut env, bitmap);
        match bitmap_info {
            Ok(mut info) => {
                let mut dst_image = BlurImageMut {
                    data: BufferStore::Borrowed(&mut info.data),
                    width: info.width,
                    height: info.height,
                    stride: info.stride,
                    channels: FastBlurChannels::Channels4,
                };

                libblur::fast_gaussian(
                    &mut dst_image,
                    radius as u32,
                    ThreadingPolicy::Adaptive,
                    edge_mode,
                )
                .unwrap();

                let new_bitmap_r = android_bitmap::create_bitmap(
                    &mut env,
                    &info.data,
                    info.stride,
                    info.width,
                    info.height,
                );

                match new_bitmap_r {
                    Ok(new_bitmap) => new_bitmap.as_raw(),
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
        edge_mode: jint,
    ) -> jobject {
        if radius <= 0 {
            let clazz = env
                .find_class("java/lang/Exception")
                .expect("Found exception class");
            env.throw_new(clazz, "Radius must be more than 0")
                .expect("Failed to access JNI");
            return bitmap;
        }

        let edge_mode: EdgeMode = (edge_mode as usize).into();
        if edge_mode == EdgeMode::Constant {
            let clazz = env
                .find_class("java/lang/Exception")
                .expect("Found exception class");
            env.throw_new(clazz, "Constant is not supported in this blur")
                .expect("Failed to access JNI");
            return bitmap;
        }

        let bitmap_info = android_bitmap::get_bitmap_rgba8888(&mut env, bitmap);
        match bitmap_info {
            Ok(mut info) => {
                let mut dst_image = BlurImageMut {
                    data: BufferStore::Borrowed(&mut info.data),
                    width: info.width,
                    height: info.height,
                    stride: info.stride,
                    channels: FastBlurChannels::Channels4,
                };

                libblur::fast_gaussian_next(
                    &mut dst_image,
                    radius as u32,
                    ThreadingPolicy::Adaptive,
                    edge_mode,
                )
                .unwrap();

                let new_bitmap_r = android_bitmap::create_bitmap(
                    &mut env,
                    &info.data,
                    info.stride,
                    info.width,
                    info.height,
                );

                match new_bitmap_r {
                    Ok(new_bitmap) => new_bitmap.as_raw(),
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
    pub unsafe extern "system" fn Java_com_awxkee_aire_pipeline_BlurPipelinesImpl_fastBilateralBlurImpl(
        mut env: JNIEnv,
        _: jobject,
        bitmap: jobject,
        kernel_size: jint,
        spatial_sigma: jfloat,
        range_sigma: jfloat,
    ) -> jobject {
        if kernel_size & 1 == 0 {
            let clazz = env
                .find_class("java/lang/Exception")
                .expect("Found exception class");
            env.throw_new(
                clazz,
                format!("Kernel size must be odd but got {}", kernel_size),
            )
            .expect("Failed to access JNI");
            return bitmap;
        }
        if kernel_size < 1 {
            let clazz = env
                .find_class("java/lang/Exception")
                .expect("Found exception class");
            env.throw_new(
                clazz,
                format!("Kernel size must be >= 1 but got {}", kernel_size),
            )
            .expect("Failed to access JNI");
            return bitmap;
        }
        if spatial_sigma <= 0. || range_sigma <= 0. {
            let clazz = env
                .find_class("java/lang/Exception")
                .expect("Found exception class");
            env.throw_new(clazz, "Spatial and range sigmas must be >= 0")
                .expect("Failed to access JNI");
            return bitmap;
        }

        let bitmap_info = android_bitmap::get_bitmap_rgba8888(&mut env, bitmap);
        match bitmap_info {
            Ok(mut info) => {
                let mut bilateral_dst_image =
                    vec![0u8; info.height as usize * info.width as usize * 4];

                let src_image = BlurImage {
                    data: std::borrow::Cow::Borrowed(&info.data),
                    width: info.width,
                    height: info.height,
                    stride: info.stride,
                    channels: FastBlurChannels::Channels4,
                };

                let mut dst_image = BlurImageMut {
                    data: BufferStore::Borrowed(&mut bilateral_dst_image),
                    width: info.width,
                    height: info.height,
                    stride: info.width * 4,
                    channels: FastBlurChannels::Channels4,
                };

                libblur::fast_bilateral_filter(
                    &src_image,
                    &mut dst_image,
                    kernel_size as u32,
                    spatial_sigma,
                    range_sigma,
                )
                .unwrap();

                let new_bitmap_r = android_bitmap::create_bitmap(
                    &mut env,
                    &bilateral_dst_image,
                    info.width * 4,
                    info.width,
                    info.height,
                );

                match new_bitmap_r {
                    Ok(new_bitmap) => new_bitmap.as_raw(),
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
    pub unsafe extern "system" fn Java_com_awxkee_aire_pipeline_BlurPipelinesImpl_motionBlurImpl(
        mut env: JNIEnv,
        _: jobject,
        bitmap: jobject,
        kernel_size: jint,
        angle: jfloat,
        edge_mode: jint,
        border_scalar: jobject,
    ) -> jobject {
        if kernel_size <= 0 {
            let clazz = env
                .find_class("java/lang/Exception")
                .expect("Found exception class");
            env.throw_new(clazz, "Radius must be more than 0")
                .expect("Failed to access JNI");
            return bitmap;
        }

        if kernel_size & 1 == 0 {
            let clazz = env
                .find_class("java/lang/Exception")
                .expect("Found exception class");
            env.throw_new(
                clazz,
                format!("Kernel size must be odd but got {}", kernel_size),
            )
            .expect("Failed to access JNI");
            return bitmap;
        }

        let edge_mode: EdgeMode = (edge_mode as usize).into();

        let bitmap_info = android_bitmap::get_bitmap_rgba8888(&mut env, bitmap);
        match bitmap_info {
            Ok(info) => {
                let mut dst_vec: Vec<u8> =
                    vec![0u8; info.width as usize * 4 * info.height as usize];

                let scalar = get_scalar_from_java(&mut env, border_scalar);

                let src_image = BlurImage {
                    data: std::borrow::Cow::Borrowed(&info.data),
                    width: info.width,
                    height: info.height,
                    stride: info.stride,
                    channels: FastBlurChannels::Channels4,
                };

                let mut dst_image = BlurImageMut {
                    data: BufferStore::Borrowed(&mut dst_vec),
                    width: info.width,
                    height: info.height,
                    stride: info.width * 4,
                    channels: FastBlurChannels::Channels4,
                };

                libblur::motion_blur(
                    &src_image,
                    &mut dst_image,
                    angle,
                    kernel_size as usize,
                    edge_mode,
                    scalar,
                    ThreadingPolicy::default(),
                )
                .unwrap();

                let new_bitmap_r = android_bitmap::create_bitmap(
                    &mut env,
                    &dst_vec,
                    info.width * 4,
                    info.width,
                    info.height,
                );

                match new_bitmap_r {
                    Ok(new_bitmap) => new_bitmap.as_raw(),
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
    pub unsafe extern "system" fn Java_com_awxkee_aire_pipeline_ProcessingPipelinesImpl_convolve2DImpl(
        mut env: JNIEnv,
        _: jobject,
        bitmap: jobject,
        kernel: jfloatArray,
        kernel_width: jint,
        kernel_height: jint,
        edge_mode: jint,
        border_scalar: jobject,
        convolve_mode: jint,
    ) -> jobject {
        let j_array = unsafe { JFloatArray::from_raw(kernel) };
        let kernel_length = env
            .get_array_length(&j_array)
            .expect("Failed to get array length") as usize;

        if kernel_width & 1 == 0 || kernel_height & 1 == 0 {
            let clazz = env
                .find_class("java/lang/Exception")
                .expect("Found exception class");
            env.throw_new(
                clazz,
                format!(
                    "Kernel size must be odd but got {}, {}",
                    kernel_width, kernel_height
                ),
            )
            .expect("Failed to access JNI");
            return bitmap;
        }

        let use_rgba = match convolve_mode {
            0 => false,
            1 => true,
            _ => {
                let clazz = env
                    .find_class("java/lang/Exception")
                    .expect("Found exception class");
                env.throw_new(
                    clazz,
                    format!(
                        "Value {} is not supported for op mode in morphology",
                        convolve_mode
                    ),
                )
                .expect("Failed to access JNI");
                return bitmap;
            }
        };

        if kernel_width < 1 || kernel_height < 1 {
            let clazz = env
                .find_class("java/lang/Exception")
                .expect("Found exception class");
            env.throw_new(
                clazz,
                format!(
                    "Kernel size must be >= 1 but got {}, {}",
                    kernel_width, kernel_height
                ),
            )
            .expect("Failed to access JNI");
            return bitmap;
        }

        if kernel_length != (kernel_height * kernel_height) as usize {
            let clazz = env
                .find_class("java/lang/Exception")
                .expect("Found exception class");
            env.throw_new(clazz, "Kernel dimensions does not match provided kernel")
                .expect("Failed to access JNI");
            return bitmap;
        }

        let edge_mode: EdgeMode = (edge_mode as usize).into();

        let mut local_kernel = vec![0f32; kernel_length];
        match env.get_float_array_region(&j_array, 0, &mut local_kernel) {
            Ok(_) => {}
            Err(err) => {
                let clazz = env
                    .find_class("java/lang/Exception")
                    .expect("Found exception class");
                env.throw_new(clazz, err.to_string())
                    .expect("Failed to access JNI");
                return JObject::null().as_raw();
            }
        }

        let border_scalar = get_scalar_from_java(&mut env, border_scalar);

        let bitmap_info = android_bitmap::get_bitmap_rgba8888(&mut env, bitmap);
        match bitmap_info {
            Ok(info) => {
                if use_rgba {
                    let mut dst_vec: Vec<u8> =
                        vec![0u8; info.width as usize * 4 * info.height as usize];

                    let src_image = BlurImage {
                        data: std::borrow::Cow::Borrowed(&info.data),
                        width: info.width,
                        height: info.height,
                        stride: info.stride,
                        channels: FastBlurChannels::Channels4,
                    };

                    let mut dst_image = BlurImageMut {
                        data: BufferStore::Borrowed(&mut dst_vec),
                        width: info.width,
                        height: info.height,
                        stride: info.width * 4,
                        channels: FastBlurChannels::Channels4,
                    };

                    if kernel_width < 10 && kernel_height < 10 {
                        match libblur::filter_2d_rgba::<u8, f32>(
                            &src_image,
                            &mut dst_image,
                            &local_kernel,
                            KernelShape::new(kernel_width as usize, kernel_height as usize),
                            edge_mode,
                            border_scalar,
                            ThreadingPolicy::Adaptive,
                        ) {
                            Ok(_) => {}
                            Err(err) => {
                                let clazz = env
                                    .find_class("java/lang/Exception")
                                    .expect("Found exception class");
                                env.throw_new(clazz, err.to_string())
                                    .expect("Failed to access JNI");
                                return JObject::null().as_raw();
                            }
                        }
                    } else {
                        match libblur::filter_2d_rgba_fft::<u8, f32, f32>(
                            &src_image,
                            &mut dst_image,
                            &local_kernel,
                            KernelShape::new(kernel_width as usize, kernel_height as usize),
                            edge_mode,
                            border_scalar,
                        ) {
                            Ok(_) => {}
                            Err(err) => {
                                let clazz = env
                                    .find_class("java/lang/Exception")
                                    .expect("Found exception class");
                                env.throw_new(clazz, err.to_string())
                                    .expect("Failed to access JNI");
                                return JObject::null().as_raw();
                            }
                        }
                    }

                    let new_bitmap_r = android_bitmap::create_bitmap(
                        &mut env,
                        &dst_vec,
                        info.width * 4,
                        info.width,
                        info.height,
                    );
                    match new_bitmap_r {
                        Ok(new_bitmap) => new_bitmap.as_raw(),
                        Err(error_message) => {
                            let clazz = env
                                .find_class("java/lang/Exception")
                                .expect("Found exception class");
                            env.throw_new(clazz, error_message)
                                .expect("Failed to access JNI");
                            bitmap
                        }
                    }
                } else {
                    let mut src_vec = vec![0u8; info.width as usize * 3 * info.height as usize];

                    for (src_chunk, dst_chunk) in info
                        .data
                        .chunks_exact(info.stride as usize)
                        .zip(src_vec.chunks_exact_mut(3 * info.width as usize))
                    {
                        for (src_layout, dst_layout) in
                            src_chunk.chunks_exact(4).zip(dst_chunk.chunks_exact_mut(3))
                        {
                            dst_layout[0] = src_layout[0];
                            dst_layout[1] = src_layout[1];
                            dst_layout[2] = src_layout[2];
                        }
                    }

                    let mut dst_vec: Vec<u8> =
                        vec![0u8; info.width as usize * 3 * info.height as usize];

                    let src_image = BlurImage {
                        data: std::borrow::Cow::Borrowed(&src_vec),
                        width: info.width,
                        height: info.height,
                        stride: info.width * 3,
                        channels: FastBlurChannels::Channels3,
                    };

                    let mut dst_image = BlurImageMut {
                        data: BufferStore::Borrowed(&mut dst_vec),
                        width: info.width,
                        height: info.height,
                        stride: info.width * 3,
                        channels: FastBlurChannels::Channels3,
                    };

                    if kernel_width < 10 && kernel_height < 10 {
                        match libblur::filter_2d_rgb::<u8, f32>(
                            &src_image,
                            &mut dst_image,
                            &local_kernel,
                            KernelShape::new(kernel_width as usize, kernel_height as usize),
                            edge_mode,
                            border_scalar,
                            ThreadingPolicy::Adaptive,
                        ) {
                            Ok(_) => {}
                            Err(err) => {
                                let clazz = env
                                    .find_class("java/lang/Exception")
                                    .expect("Found exception class");
                                env.throw_new(clazz, err.to_string())
                                    .expect("Failed to access JNI");
                                return JObject::null().as_raw();
                            }
                        }
                    } else {
                        match libblur::filter_2d_rgb_fft::<u8, f32, f32>(
                            &src_image,
                            &mut dst_image,
                            &local_kernel,
                            KernelShape::new(kernel_width as usize, kernel_height as usize),
                            edge_mode,
                            border_scalar,
                        ) {
                            Ok(_) => {}
                            Err(err) => {
                                let clazz = env
                                    .find_class("java/lang/Exception")
                                    .expect("Found exception class");
                                env.throw_new(clazz, err.to_string())
                                    .expect("Failed to access JNI");
                                return JObject::null().as_raw();
                            }
                        }
                    }

                    src_vec.resize(0, 0u8);

                    let mut new_image: Vec<u8> =
                        vec![0u8; info.width as usize * 4 * info.height as usize];

                    for (src_layout, dst_layout) in
                        dst_vec.chunks_exact(3).zip(new_image.chunks_exact_mut(4))
                    {
                        dst_layout[0] = src_layout[0];
                        dst_layout[1] = src_layout[1];
                        dst_layout[2] = src_layout[2];
                        dst_layout[3] = 255;
                    }

                    dst_vec.resize(0, 0);

                    let new_bitmap_r = android_bitmap::create_bitmap(
                        &mut env,
                        &new_image,
                        info.width * 4,
                        info.width,
                        info.height,
                    );
                    match new_bitmap_r {
                        Ok(new_bitmap) => new_bitmap.as_raw(),
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
    pub unsafe extern "system" fn Java_com_awxkee_aire_pipeline_ProcessingPipelinesImpl_sobelImpl(
        mut env: JNIEnv,
        _: jobject,
        bitmap: jobject,
        edge_mode: jint,
        border_scalar: jobject,
    ) -> jobject {
        let edge_mode: EdgeMode = (edge_mode as usize).into();

        let border_scalar = get_scalar_from_java(&mut env, border_scalar);

        let bitmap_info = android_bitmap::get_bitmap_rgba8888(&mut env, bitmap);
        match bitmap_info {
            Ok(info) => {
                let mut src_vec = vec![0u8; info.width as usize * info.height as usize];

                let kr = 0.2126f32;
                let kb = 0.0722f32;
                let kg = 1. - kr - kb;

                const SCALE: f32 = (1i32 << 10i32) as f32;
                let v_kr = (kr * SCALE).ceil() as i32;
                let v_kb = (kb * SCALE).ceil() as i32;
                let v_kg = (kg * SCALE).ceil() as i32;

                for (src_chunk, dst_chunk) in info
                    .data
                    .chunks_exact(info.stride as usize)
                    .zip(src_vec.chunks_exact_mut(info.width as usize))
                {
                    for (src_layout, dst_layout) in
                        src_chunk.chunks_exact(4).zip(dst_chunk.iter_mut())
                    {
                        *dst_layout = (((src_layout[0] as i32 * v_kr)
                            + (src_layout[1] as i32 * v_kg)
                            + (src_layout[2] as i32 * v_kb))
                            >> 10)
                            .max(0)
                            .min(255) as u8;
                    }
                }

                let mut dst_vec: Vec<u8> = vec![0u8; info.width as usize * info.height as usize];

                let src_image = BlurImage {
                    data: std::borrow::Cow::Borrowed(&src_vec),
                    width: info.width,
                    height: info.height,
                    stride: info.width,
                    channels: FastBlurChannels::Plane,
                };

                let mut dst_image = BlurImageMut {
                    data: BufferStore::Borrowed(&mut dst_vec),
                    width: info.width,
                    height: info.height,
                    stride: info.width,
                    channels: FastBlurChannels::Plane,
                };

                libblur::sobel(
                    &src_image,
                    &mut dst_image,
                    edge_mode,
                    border_scalar,
                    ThreadingPolicy::Adaptive,
                )
                .unwrap();

                src_vec.resize(0, 0u8);

                let mut new_image: Vec<u8> =
                    vec![0u8; info.width as usize * 4 * info.height as usize];

                for (src_layout, dst_layout) in dst_vec.iter().zip(new_image.chunks_exact_mut(4)) {
                    let val = *src_layout;
                    dst_layout[0] = val;
                    dst_layout[1] = val;
                    dst_layout[2] = val;
                    dst_layout[3] = 255;
                }

                dst_vec.resize(0, 0);

                let new_bitmap_r = android_bitmap::create_bitmap(
                    &mut env,
                    &new_image,
                    info.width * 4,
                    info.width,
                    info.height,
                );
                match new_bitmap_r {
                    Ok(new_bitmap) => new_bitmap.as_raw(),
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
    pub unsafe extern "system" fn Java_com_awxkee_aire_pipeline_ProcessingPipelinesImpl_laplacianImpl(
        mut env: JNIEnv,
        _: jobject,
        bitmap: jobject,
        edge_mode: jint,
        border_scalar: jobject,
    ) -> jobject {
        let edge_mode: EdgeMode = (edge_mode as usize).into();

        let border_scalar = get_scalar_from_java(&mut env, border_scalar);

        let bitmap_info = android_bitmap::get_bitmap_rgba8888(&mut env, bitmap);
        match bitmap_info {
            Ok(info) => {
                let mut src_vec = vec![0u8; info.width as usize * info.height as usize];

                let kr = 0.2126f32;
                let kb = 0.0722f32;
                let kg = 1. - kr - kb;

                const SCALE: f32 = (1i32 << 10i32) as f32;
                let v_kr = (kr * SCALE).ceil() as i32;
                let v_kb = (kb * SCALE).ceil() as i32;
                let v_kg = (kg * SCALE).ceil() as i32;

                for (src_chunk, dst_chunk) in info
                    .data
                    .chunks_exact(info.stride as usize)
                    .zip(src_vec.chunks_exact_mut(info.width as usize))
                {
                    for (src_layout, dst_layout) in
                        src_chunk.chunks_exact(4).zip(dst_chunk.iter_mut())
                    {
                        *dst_layout = (((src_layout[0] as i32 * v_kr)
                            + (src_layout[1] as i32 * v_kg)
                            + (src_layout[2] as i32 * v_kb))
                            >> 10)
                            .max(0)
                            .min(255) as u8;
                    }
                }

                let mut dst_vec: Vec<u8> = vec![0u8; info.width as usize * info.height as usize];

                let src_image = BlurImage {
                    data: std::borrow::Cow::Borrowed(&src_vec),
                    width: info.width,
                    height: info.height,
                    stride: info.width,
                    channels: FastBlurChannels::Plane,
                };

                let mut dst_image = BlurImageMut {
                    data: BufferStore::Borrowed(&mut dst_vec),
                    width: info.width,
                    height: info.height,
                    stride: info.width,
                    channels: FastBlurChannels::Plane,
                };

                libblur::laplacian(
                    &src_image,
                    &mut dst_image,
                    edge_mode,
                    border_scalar,
                    ThreadingPolicy::Adaptive,
                )
                .unwrap();

                src_vec.resize(0, 0u8);

                let mut new_image: Vec<u8> =
                    vec![0u8; info.width as usize * 4 * info.height as usize];

                for (src_layout, dst_layout) in dst_vec.iter().zip(new_image.chunks_exact_mut(4)) {
                    let val = *src_layout;
                    dst_layout[0] = val;
                    dst_layout[1] = val;
                    dst_layout[2] = val;
                    dst_layout[3] = 255;
                }

                dst_vec.resize(0, 0);

                let new_bitmap_r = android_bitmap::create_bitmap(
                    &mut env,
                    &new_image,
                    info.width * 4,
                    info.width,
                    info.height,
                );
                match new_bitmap_r {
                    Ok(new_bitmap) => new_bitmap.as_raw(),
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
