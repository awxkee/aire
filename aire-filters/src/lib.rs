pub use surface_type::aire_reformat_surface_laba_to_u8;
pub use surface_type::aire_reformat_surface_linear_to_u8;
pub use surface_type::aire_reformat_surface_luva_to_u8;
pub use surface_type::aire_reformat_surface_u8_to_laba;
pub use surface_type::aire_reformat_surface_u8_to_linear;
pub use surface_type::aire_reformat_surface_u8_to_luva;

mod android_bitmap;
mod bitmap_helper;
mod clahe;
mod equalize_hist;
mod histogram;
mod surface_type;

#[cfg(target_os = "android")]
#[allow(non_snake_case)]
pub mod android {
    extern crate jni;

    use jni::JNIEnv;
    use jni::objects::JObject;
    use jni::sys::{jfloat, jint, jobject};
    use pic_scale::{ImageSize, ImageStore, LabScaler, LinearScaler, LuvScaler, ResamplingFunction, Scaler, Scaling, ThreadingPolicy};

    use crate::android_bitmap::copy_image;
    use crate::bitmap_helper::android_bitmap;
    use crate::clahe::{ahe_lab_rgba, ahe_luv_rgba, ahe_yuv_rgba, clahe_lab_rgba, clahe_luv_rgba, clahe_yuv_rgba, ClaheGridSize};
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
                let mut dst_vec: Vec<u8> = vec![0u8; info.stride as usize * info.height as usize];

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
            Ok(bmp) => { bmp }
            Err(err) => {
                let clazz = env
                    .find_class("java/lang/Exception")
                    .expect("Found exception class");
                env.throw_new(clazz, err)
                    .expect("Failed to access JNI");
                return JObject::default().as_raw();
            }
        };

        let resampling: ResamplingFunction = (resampler as u32).into();

        let mut scaler: Box<dyn Scaling> = match colorspace {
            0 => Box::new(Scaler::new(resampling)),
            1 => Box::new(LabScaler::new(resampling)),
            2 => Box::new(LinearScaler::new(resampling)),
            3 => Box::new(LuvScaler::new(resampling)),
            _ => {
                let clazz = env
                    .find_class("java/lang/Exception")
                    .expect("Found exception class");
                env.throw_new(clazz, "Unknown resampling function was requested")
                    .expect("Failed to access JNI");
                return JObject::default().as_raw();
            }
        };
        scaler.set_threading_policy(ThreadingPolicy::Single);

        let mut source_buffer = vec![0u8; bitmap_info.width as usize * 4usize * std::mem::size_of::<u8>() * bitmap_info.height as usize];

        copy_image::<u8>(
            &bitmap_info.data,
            bitmap_info.stride,
            &mut source_buffer,
            (bitmap_info.width as usize * 4usize * std::mem::size_of::<u8>()) as u32,
            bitmap_info.width,
            bitmap_info.height,
            4u32,
        );
        let source_store = ImageStore::<u8, 4>::new(source_buffer, bitmap_info.width as usize, bitmap_info.height as usize);
        let new_size = ImageSize::new(new_width as usize, new_height as usize);
        let new_image_store = scaler.resize_rgba(new_size, source_store, true);
        let bytes = new_image_store.as_bytes();
        return match android_bitmap::create_bitmap(
            &mut env,
            &bytes,
            new_size.width as u32 * 4,
            new_size.width as u32,
            new_size.height as u32,
        ) {
            Ok(android_bitmap) => { android_bitmap.as_raw() }
            Err(error) => {
                let clazz = env
                    .find_class("java/lang/Exception")
                    .expect("Found exception class");
                env.throw_new(clazz, error)
                    .expect("Failed to access JNI");
                return JObject::default().as_raw();
            }
        };
    }
}
