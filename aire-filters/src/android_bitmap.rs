use colorutils_rs::{Rgb565, Rgba, Rgba1010102, ToRgba8};
use half::f16;
use num_traits::FromPrimitive;
#[cfg(all(target_arch = "aarch64", target_feature = "neon"))]
use std::arch::aarch64::*;

#[allow(dead_code)]
pub fn convert_rgbaf16_to_rgba8888(
    source: &[u8],
    src_stride: u32,
    dst: &mut [u8],
    dst_stride: u32,
    width: u32,
    height: u32,
) {
    let mut src_shift = 0usize;
    let mut dst_shift = 0usize;
    for _ in 0..height as usize {
        let src_ptr = unsafe { source.as_ptr().add(src_shift) };
        let src_slice =
            unsafe { std::slice::from_raw_parts(src_ptr as *const f16, width as usize * 4) };
        let dst_ptr = unsafe { dst.as_mut_ptr().add(dst_shift) };
        let dst_slice = unsafe { std::slice::from_raw_parts_mut(dst_ptr, width as usize * 4) };

        for x in 0..width as usize {
            let px = x * 4;
            let src_pixel = Rgba::<f16>::new(
                unsafe { *src_slice.get_unchecked(px) },
                unsafe { *src_slice.get_unchecked(px + 1) },
                unsafe { *src_slice.get_unchecked(px + 2) },
                unsafe { *src_slice.get_unchecked(px + 3) },
            );
            let dst_pixel = src_pixel.to_rgba8();
            unsafe {
                *dst_slice.get_unchecked_mut(px) = dst_pixel.r;
                *dst_slice.get_unchecked_mut(px + 1) = dst_pixel.g;
                *dst_slice.get_unchecked_mut(px + 2) = dst_pixel.b;
                *dst_slice.get_unchecked_mut(px + 3) = dst_pixel.a;
            }
        }
        dst_shift += dst_stride as usize;
        src_shift += src_stride as usize;
    }
}

#[allow(dead_code)]
pub fn convert_rgb565_to_rgba8888(
    source: &[u8],
    src_stride: u32,
    dst: &mut [u8],
    dst_stride: u32,
    width: u32,
    height: u32,
) {
    let mut src_shift = 0usize;
    let mut dst_shift = 0usize;
    for _ in 0..height as usize {
        let src_ptr = unsafe { source.as_ptr().add(src_shift) };
        let src_slice =
            unsafe { std::slice::from_raw_parts(src_ptr as *const u16, width as usize) };
        let dst_ptr = unsafe { dst.as_mut_ptr().add(dst_shift) };
        let dst_slice = unsafe { std::slice::from_raw_parts_mut(dst_ptr, width as usize * 4) };

        for x in 0..width as usize {
            let px = x * 4;
            let src_pixel = Rgb565::new(unsafe { *src_slice.get_unchecked(x) });
            let dst_pixel = src_pixel.to_rgba8();
            unsafe {
                *dst_slice.get_unchecked_mut(px) = dst_pixel.r;
                *dst_slice.get_unchecked_mut(px + 1) = dst_pixel.g;
                *dst_slice.get_unchecked_mut(px + 2) = dst_pixel.b;
                *dst_slice.get_unchecked_mut(px + 3) = dst_pixel.a;
            }
        }
        dst_shift += dst_stride as usize;
        src_shift += src_stride as usize;
    }
}

#[allow(dead_code)]
pub fn convert_rgb1010102_to_rgba8888(
    source: &[u8],
    src_stride: u32,
    dst: &mut [u8],
    dst_stride: u32,
    width: u32,
    height: u32,
) {
    let mut src_shift = 0usize;
    let mut dst_shift = 0usize;
    for _ in 0..height as usize {
        let src_ptr = unsafe { source.as_ptr().add(src_shift) };
        let src_slice =
            unsafe { std::slice::from_raw_parts(src_ptr as *const u32, width as usize) };
        let dst_ptr = unsafe { dst.as_mut_ptr().add(dst_shift) };
        let dst_slice = unsafe { std::slice::from_raw_parts_mut(dst_ptr, width as usize * 4) };

        for x in 0..width as usize {
            let px = x * 4;
            let src_pixel = Rgba1010102::new(unsafe { *src_slice.get_unchecked(x) });
            let dst_pixel = src_pixel.to_rgba8();
            unsafe {
                *dst_slice.get_unchecked_mut(px) = dst_pixel.r;
                *dst_slice.get_unchecked_mut(px + 1) = dst_pixel.g;
                *dst_slice.get_unchecked_mut(px + 2) = dst_pixel.b;
                *dst_slice.get_unchecked_mut(px + 3) = dst_pixel.a;
            }
        }
        dst_shift += dst_stride as usize;
        src_shift += src_stride as usize;
    }
}

#[allow(dead_code)]
pub fn copy_image<T: FromPrimitive + Copy>(
    source: &[T],
    src_stride: u32,
    dst: &mut [T],
    dst_stride: u32,
    width: u32,
    height: u32,
    components_count: u32,
) {
    for y in 0..height as usize {
        let dst_ptr = unsafe { (dst.as_mut_ptr() as *mut u8).add(y * dst_stride as usize) };
        let src_ptr = unsafe { (source.as_ptr() as *const u8).add(y * src_stride as usize) };
        let row_length = width as usize * components_count as usize * std::mem::size_of::<T>();
        let mut cx = 0usize;
        #[cfg(all(target_arch = "aarch64", target_feature = "neon"))]
        while cx + 64 < row_length {
            let row = unsafe { vld1q_u8_x4(src_ptr.add(cx)) };
            unsafe {
                vst1q_u8_x4(dst_ptr.add(cx), row);
            }
            cx += 64;
        }
        #[cfg(all(target_arch = "aarch64", target_feature = "neon"))]
        while cx + 32 < row_length {
            let row = unsafe { vld1q_u8_x2(src_ptr.add(cx)) };
            unsafe {
                vst1q_u8_x2(dst_ptr.add(cx), row);
            }
            cx += 32;
        }
        #[cfg(all(target_arch = "aarch64", target_feature = "neon"))]
        while cx + 16 < row_length {
            let row = unsafe { vld1q_u8(src_ptr.add(cx)) };
            unsafe {
                vst1q_u8(dst_ptr.add(cx), row);
            }
            cx += 16;
        }
        #[cfg(all(target_arch = "aarch64", target_feature = "neon"))]
        while cx + 8 < row_length {
            let row = unsafe { vld1_u8(src_ptr.add(cx)) };
            unsafe {
                vst1_u8(dst_ptr.add(cx), row);
            }
            cx += 8;
        }
        while cx < row_length {
            unsafe {
                dst_ptr
                    .add(cx)
                    .write_unaligned(src_ptr.add(cx).read_unaligned());
            }
            cx += 1;
        }
    }
}
