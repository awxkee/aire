#[cfg(target_os = "android")]
#[allow(non_snake_case)]
pub mod android {
    extern crate jni;

    use histogram_equalization::*;
    use jni::sys::{jfloat, jint, jobject};
    use jni::JNIEnv;

    use crate::bitmap_helper::android_bitmap;
    use crate::equalize_hist::equalize_histogram_squares_rgba;

    #[no_mangle]
    pub unsafe extern "system" fn Java_com_awxkee_aire_pipeline_EffectsPipelineImpl_equalizeHistImpl(
        mut env: JNIEnv,
        _: jobject,
        bitmap: jobject,
    ) -> jobject {
        let bitmap_info = android_bitmap::get_bitmap_rgba8888(&mut env, bitmap);
        match bitmap_info {
            Ok(info) => {
                let mut dst_vec: Vec<u8> = vec![0u8; info.stride as usize * info.height as usize];

                hist_equal_yuv_rgba(
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
        bins_count: jint,
    ) -> jobject {
        let bitmap_info = android_bitmap::get_bitmap_rgba8888(&mut env, bitmap);
        match bitmap_info {
            Ok(info) => {
                let mut dst_vec: Vec<u8> = vec![0u8; info.stride as usize * info.height as usize];
                hist_equal_hsv_rgba(
                    &info.data,
                    info.stride,
                    &mut dst_vec,
                    info.stride,
                    info.width,
                    info.height,
                    bins_count as usize,
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
    pub unsafe extern "system" fn Java_com_awxkee_aire_pipeline_EffectsPipelineImpl_equalizeHistLABImpl(
        mut env: JNIEnv,
        _: jobject,
        bitmap: jobject,
        bins_count: jint,
    ) -> jobject {
        let bitmap_info = android_bitmap::get_bitmap_rgba8888(&mut env, bitmap);
        match bitmap_info {
            Ok(info) => {
                let mut dst_vec: Vec<u8> = vec![0u8; info.stride as usize * info.height as usize];
                hist_equal_lab_rgba(
                    &info.data,
                    info.stride,
                    &mut dst_vec,
                    info.stride,
                    info.width,
                    info.height,
                    bins_count as usize,
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
    pub unsafe extern "system" fn Java_com_awxkee_aire_pipeline_EffectsPipelineImpl_equalizeHistLUVImpl(
        mut env: JNIEnv,
        _: jobject,
        bitmap: jobject,
        bins_count: jint,
    ) -> jobject {
        let bitmap_info = android_bitmap::get_bitmap_rgba8888(&mut env, bitmap);
        match bitmap_info {
            Ok(info) => {
                let mut dst_vec: Vec<u8> = vec![0u8; info.stride as usize * info.height as usize];
                hist_equal_lab_rgba(
                    &info.data,
                    info.stride,
                    &mut dst_vec,
                    info.stride,
                    info.width,
                    info.height,
                    bins_count as usize,
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
    pub unsafe extern "system" fn Java_com_awxkee_aire_pipeline_EffectsPipelineImpl_equalizeHistHSLImpl(
        mut env: JNIEnv,
        _: jobject,
        bitmap: jobject,
        bins_count: jint,
    ) -> jobject {
        let bitmap_info = android_bitmap::get_bitmap_rgba8888(&mut env, bitmap);
        match bitmap_info {
            Ok(info) => {
                let mut dst_vec: Vec<u8> = vec![0u8; info.stride as usize * info.height as usize];
                hist_equal_hsl_rgba(
                    &info.data,
                    info.stride,
                    &mut dst_vec,
                    info.stride,
                    info.width,
                    info.height,
                    bins_count as usize,
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
            Ok(info) => {
                let mut dst_vec: Vec<u8> = vec![0u8; info.stride as usize * info.height as usize];
                ahe_yuv_rgba(
                    &info.data,
                    info.stride,
                    &mut dst_vec,
                    info.stride,
                    info.width,
                    info.height,
                    ClaheGridSize::new(grid_size_w as u32, grid_size_h as u32),
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
            Ok(info) => {
                let mut dst_vec: Vec<u8> = vec![0u8; info.stride as usize * info.height as usize];
                clahe_yuv_rgba(
                    &info.data,
                    info.stride,
                    &mut dst_vec,
                    info.stride,
                    info.width,
                    info.height,
                    threshold,
                    ClaheGridSize::new(grid_size_w as u32, grid_size_h as u32),
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
        bins_count: jint,
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
            Ok(info) => {
                let mut dst_vec: Vec<u8> = vec![0u8; info.stride as usize * info.height as usize];
                clahe_luv_rgba(
                    &info.data,
                    info.stride,
                    &mut dst_vec,
                    info.stride,
                    info.width,
                    info.height,
                    threshold,
                    ClaheGridSize::new(grid_size_w as u32, grid_size_h as u32),
                    bins_count as usize,
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
    pub unsafe extern "system" fn Java_com_awxkee_aire_pipeline_EffectsPipelineImpl_claheHSVImpl(
        mut env: JNIEnv,
        _: jobject,
        bitmap: jobject,
        threshold: jfloat,
        grid_size_w: jint,
        grid_size_h: jint,
        bins_count: jint,
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
            Ok(info) => {
                let mut dst_vec: Vec<u8> = vec![0u8; info.stride as usize * info.height as usize];
                clahe_hsv_rgba(
                    &info.data,
                    info.stride,
                    &mut dst_vec,
                    info.stride,
                    info.width,
                    info.height,
                    threshold,
                    ClaheGridSize::new(grid_size_w as u32, grid_size_h as u32),
                    bins_count as usize,
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
    pub unsafe extern "system" fn Java_com_awxkee_aire_pipeline_EffectsPipelineImpl_claheHSLImpl(
        mut env: JNIEnv,
        _: jobject,
        bitmap: jobject,
        threshold: jfloat,
        grid_size_w: jint,
        grid_size_h: jint,
        bins_count: jint,
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
            Ok(info) => {
                let mut dst_vec: Vec<u8> = vec![0u8; info.stride as usize * info.height as usize];
                clahe_hsl_rgba(
                    &info.data,
                    info.stride,
                    &mut dst_vec,
                    info.stride,
                    info.width,
                    info.height,
                    threshold,
                    ClaheGridSize::new(grid_size_w as u32, grid_size_h as u32),
                    bins_count as usize,
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
    pub unsafe extern "system" fn Java_com_awxkee_aire_pipeline_EffectsPipelineImpl_equalizeHistAdaptiveLUVImpl(
        mut env: JNIEnv,
        _: jobject,
        bitmap: jobject,
        grid_size_w: jint,
        grid_size_h: jint,
        bins_count: jint,
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
            Ok(info) => {
                let mut dst_vec: Vec<u8> = vec![0u8; info.stride as usize * info.height as usize];
                ahe_luv_rgba(
                    &info.data,
                    info.stride,
                    &mut dst_vec,
                    info.stride,
                    info.width,
                    info.height,
                    ClaheGridSize::new(grid_size_w as u32, grid_size_h as u32),
                    bins_count as usize,
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
    pub unsafe extern "system" fn Java_com_awxkee_aire_pipeline_EffectsPipelineImpl_claheLABImpl(
        mut env: JNIEnv,
        _: jobject,
        bitmap: jobject,
        threshold: jfloat,
        grid_size_w: jint,
        grid_size_h: jint,
        bins_count: jint,
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
            Ok(info) => {
                let mut dst_vec: Vec<u8> = vec![0u8; info.stride as usize * info.height as usize];
                clahe_lab_rgba(
                    &info.data,
                    info.stride,
                    &mut dst_vec,
                    info.stride,
                    info.width,
                    info.height,
                    threshold,
                    ClaheGridSize::new(grid_size_w as u32, grid_size_h as u32),
                    bins_count as usize,
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
    pub unsafe extern "system" fn Java_com_awxkee_aire_pipeline_EffectsPipelineImpl_equalizeHistAdaptiveLABImpl(
        mut env: JNIEnv,
        _: jobject,
        bitmap: jobject,
        grid_size_w: jint,
        grid_size_h: jint,
        bins_count: jint,
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
            Ok(info) => {
                let mut dst_vec: Vec<u8> = vec![0u8; info.stride as usize * info.height as usize];
                ahe_lab_rgba(
                    &info.data,
                    info.stride,
                    &mut dst_vec,
                    info.stride,
                    info.width,
                    info.height,
                    ClaheGridSize::new(grid_size_w as u32, grid_size_h as u32),
                    bins_count as usize,
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
    pub unsafe extern "system" fn Java_com_awxkee_aire_pipeline_EffectsPipelineImpl_equalizeHistAdaptiveHSLImpl(
        mut env: JNIEnv,
        _: jobject,
        bitmap: jobject,
        grid_size_w: jint,
        grid_size_h: jint,
        bins_count: jint,
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
            Ok(info) => {
                let mut dst_vec: Vec<u8> = vec![0u8; info.stride as usize * info.height as usize];
                ahe_hsl_rgba(
                    &info.data,
                    info.stride,
                    &mut dst_vec,
                    info.stride,
                    info.width,
                    info.height,
                    ClaheGridSize::new(grid_size_w as u32, grid_size_h as u32),
                    bins_count as usize,
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
    pub unsafe extern "system" fn Java_com_awxkee_aire_pipeline_EffectsPipelineImpl_equalizeHistAdaptiveHSVImpl(
        mut env: JNIEnv,
        _: jobject,
        bitmap: jobject,
        grid_size_w: jint,
        grid_size_h: jint,
        bins_count: jint,
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
            Ok(info) => {
                let mut dst_vec: Vec<u8> = vec![0u8; info.stride as usize * info.height as usize];
                ahe_hsv_rgba(
                    &info.data,
                    info.stride,
                    &mut dst_vec,
                    info.stride,
                    info.width,
                    info.height,
                    ClaheGridSize::new(grid_size_w as u32, grid_size_h as u32),
                    bins_count as usize,
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
}
