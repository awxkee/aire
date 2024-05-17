#[cfg(target_os = "android")]
#[allow(non_snake_case)]
pub mod android_bitmap {
    use crate::android_bitmap::{
        convert_rgb1010102_to_rgba8888, convert_rgb565_to_rgba8888, convert_rgbaf16_to_rgba8888,
        copy_image,
    };
    use jni::objects::{JObject, JValue};
    use jni::sys::jobject;
    use jni::JNIEnv;
    use ndk_sys::{
        AndroidBitmapFormat, AndroidBitmapInfo, AndroidBitmap_getInfo, AndroidBitmap_lockPixels,
        AndroidBitmap_unlockPixels, ANDROID_BITMAP_FLAGS_IS_HARDWARE,
        ANDROID_BITMAP_RESULT_SUCCESS,
    };
    use std::os::raw::c_uint;
    use std::slice;

    pub struct BitmapRGBA {
        pub data: Vec<u8>,
        pub stride: u32,
        pub width: u32,
        pub height: u32,
    }

    pub fn get_bitmap_rgba8888(env: &mut JNIEnv, bitmap: jobject) -> Result<BitmapRGBA, String> {
        let mut info = AndroidBitmapInfo {
            width: 0,
            height: 0,
            stride: 0,
            format: 0,
            flags: 0,
        };

        let native_env = env.get_native_interface();

        if unsafe { AndroidBitmap_getInfo(native_env, bitmap, &mut info) }
            != ANDROID_BITMAP_RESULT_SUCCESS
        {
            return Err("Can't retrieve bitmap info".parse().unwrap());
        }

        if info.flags as i32 & ANDROID_BITMAP_FLAGS_IS_HARDWARE == 1 {
            return Err("Hardware bitmap are not supported".parse().unwrap());
        }

        let format = AndroidBitmapFormat(info.format as c_uint);

        if format != AndroidBitmapFormat::ANDROID_BITMAP_FORMAT_RGBA_8888
            && format != AndroidBitmapFormat::ANDROID_BITMAP_FORMAT_RGBA_F16
            && format != AndroidBitmapFormat::ANDROID_BITMAP_FORMAT_RGB_565
            && format != AndroidBitmapFormat::ANDROID_BITMAP_FORMAT_RGBA_1010102
        {
            return Err("Can accept only RGBA_8888, RGBA_F16, RGB_565, RGBA_1010102"
                .parse()
                .unwrap());
        }

        let mut origin_ptr: *mut std::os::raw::c_void = 0 as *mut std::os::raw::c_void;

        if unsafe { AndroidBitmap_lockPixels(native_env, bitmap, &mut origin_ptr) }
            != ANDROID_BITMAP_RESULT_SUCCESS
        {
            return Err("Can't lock bitmap pixels".parse().unwrap());
        }

        let pixels: *mut u8 = unsafe { std::mem::transmute(origin_ptr) };
        let original_slice: &mut [u8] = unsafe {
            slice::from_raw_parts_mut(pixels, info.stride as usize * info.height as usize)
        };

        let mut dst_vec: Vec<u8> = vec![];
        let new_stride = if format == AndroidBitmapFormat::ANDROID_BITMAP_FORMAT_RGBA_8888 {
            info.stride as usize
        } else {
            info.width as usize * 4usize * std::mem::size_of::<u8>()
        };
        dst_vec.resize(new_stride * info.height as usize, 0u8);

        if format == AndroidBitmapFormat::ANDROID_BITMAP_FORMAT_RGBA_8888 {
            copy_image::<u8>(
                original_slice,
                info.stride,
                &mut dst_vec,
                new_stride as u32,
                info.width,
                info.height,
                4u32,
            );
        } else if format == AndroidBitmapFormat::ANDROID_BITMAP_FORMAT_RGBA_F16 {
            convert_rgbaf16_to_rgba8888(
                original_slice,
                info.stride,
                &mut dst_vec,
                new_stride as u32,
                info.width,
                info.height,
            );
        } else if format == AndroidBitmapFormat::ANDROID_BITMAP_FORMAT_RGB_565 {
            convert_rgb565_to_rgba8888(
                original_slice,
                info.stride,
                &mut dst_vec,
                new_stride as u32,
                info.width,
                info.height,
            );
        } else if format == AndroidBitmapFormat::ANDROID_BITMAP_FORMAT_RGBA_1010102 {
            convert_rgb1010102_to_rgba8888(
                original_slice,
                info.stride,
                &mut dst_vec,
                new_stride as u32,
                info.width,
                info.height,
            );
        }

        let bitmap_info = BitmapRGBA {
            data: dst_vec,
            stride: new_stride as u32,
            width: info.width,
            height: info.height,
        };

        if unsafe { AndroidBitmap_unlockPixels(native_env, bitmap) }
            != ANDROID_BITMAP_RESULT_SUCCESS
        {
            return Err("Can't unlock bitmap pixels".parse().unwrap());
        }

        return Ok(bitmap_info);
    }

    pub fn create_bitmap<'a>(
        env: &mut JNIEnv<'a>,
        source_data: &[u8],
        stride: u32,
        width: u32,
        height: u32,
    ) -> Result<JObject<'a>, String> {
        let native_env = env.get_native_interface();
        let bitmap_config = env
            .find_class("android/graphics/Bitmap$Config")
            .expect("Found bitmap config");
        let rgba8888 = env
            .get_static_field(
                &bitmap_config,
                "ARGB_8888",
                "Landroid/graphics/Bitmap$Config;",
            )
            .expect("Found ARGB_8888 config");
        let bitmap_class = env
            .find_class("android/graphics/Bitmap")
            .expect("Found bitmap class");
        let rgba8888_field = rgba8888.l().expect("Accessed field");
        let new_bitmap_value = env
            .call_static_method(
                &bitmap_class,
                "createBitmap",
                "(IILandroid/graphics/Bitmap$Config;)Landroid/graphics/Bitmap;",
                &[
                    JValue::Int(width as i32),
                    JValue::Int(height as i32),
                    JValue::Object(&rgba8888_field),
                ],
            )
            .expect("Created bitmap");
        let new_bitmap = new_bitmap_value.l().expect("Successfully created bitmap");
        let mut new_info = AndroidBitmapInfo {
            width: 0,
            height: 0,
            stride: 0,
            format: 0,
            flags: 0,
        };

        if unsafe { AndroidBitmap_getInfo(native_env, new_bitmap.as_raw(), &mut new_info) }
            != ANDROID_BITMAP_RESULT_SUCCESS
        {
            return Err("Can't retrieve bitmap info".parse().unwrap());
        }

        let new_format = AndroidBitmapFormat(new_info.format as c_uint);

        if new_format != AndroidBitmapFormat::ANDROID_BITMAP_FORMAT_RGBA_8888 {
            return Err("Can create only RGBA_8888".parse().unwrap());
        }

        let mut src_ptr: *mut std::os::raw::c_void = 0 as *mut std::os::raw::c_void;

        if unsafe { AndroidBitmap_lockPixels(native_env, new_bitmap.as_raw(), &mut src_ptr) }
            != ANDROID_BITMAP_RESULT_SUCCESS
        {
            return Err("Can't lock bitmap pixels".parse().unwrap());
        }

        let dst_pixels: *mut u8 = unsafe { std::mem::transmute(src_ptr) };
        let dst_slice: &mut [u8] = unsafe {
            slice::from_raw_parts_mut(
                dst_pixels,
                new_info.stride as usize * new_info.height as usize,
            )
        };

        let dst_stride = new_info.stride as usize;
        let src_stride = stride as usize;
        let o_slice = source_data;

        for y in 0..new_info.height as usize {
            let dst_ptr = unsafe { dst_slice.as_mut_ptr().add(y * dst_stride) };
            let src_ptr = unsafe { o_slice.as_ptr().add(y * src_stride) };
            let row_length = new_info.width as usize * 4usize * std::mem::size_of::<u8>();
            unsafe {
                std::ptr::copy_nonoverlapping(src_ptr, dst_ptr, row_length);
            };
        }

        if unsafe { AndroidBitmap_unlockPixels(native_env, new_bitmap.as_raw()) }
            != ANDROID_BITMAP_RESULT_SUCCESS
        {
            return Err("Can't unlock image pixels".parse().unwrap());
        }

        Ok(new_bitmap)
    }
}
