use std::mem::transmute;
use std::slice;

use colorutils_rs::{append_alpha, laba_to_srgb, linear_to_rgba, Luv, Rgb, Rgba, rgba_to_laba, rgba_to_linear, TransferFunction};

pub fn reformat_surface_u8_to_linear(
    original: &[u8],
    src_stride: u32,
    destination: &mut [f32],
    dst_stride: u32,
    width: u32,
    height: u32,
) {
    rgba_to_linear(original, src_stride, destination, dst_stride, width, height, TransferFunction::Srgb);
}

pub fn reformat_surface_linear_to_u8(
    src: &[f32],
    src_stride: u32,
    dst: &mut [u8],
    dst_stride: u32,
    width: u32,
    height: u32,
) {
    linear_to_rgba(src, src_stride, dst, dst_stride, width, height, TransferFunction::Srgb);
}

#[no_mangle]
#[allow(dead_code)]
pub extern "C-unwind" fn aire_reformat_surface_u8_to_linear(
    src: *const u8,
    src_stride: u32,
    dst: *const f32,
    dst_stride: u32,
    width: u32,
    height: u32,
) {
    let src_slice = unsafe { slice::from_raw_parts(src, src_stride as usize * height as usize) };
    let dst_slice =
        unsafe { slice::from_raw_parts_mut(dst as *mut f32, dst_stride as usize * height as usize) };
    reformat_surface_u8_to_linear(src_slice, src_stride, dst_slice, dst_stride, width, height);
}

#[no_mangle]
#[allow(dead_code)]
pub extern "C-unwind" fn aire_reformat_surface_linear_to_u8(
    src: *const f32,
    src_stride: u32,
    dst: *const u8,
    dst_stride: u32,
    width: u32,
    height: u32,
) {
    let src_slice = unsafe { slice::from_raw_parts(src, src_stride as usize * height as usize) };
    let dst_slice =
        unsafe { slice::from_raw_parts_mut(dst as *mut u8, dst_stride as usize * height as usize) };
    reformat_surface_linear_to_u8(src_slice, src_stride, dst_slice, dst_stride, width, height);
}

pub fn reformat_surface_u8_to_laba(
    original: &[u8],
    src_stride: u32,
    destination: &mut [f32],
    dst_stride: u32,
    width: u32,
    height: u32,
) {
    let mut store: Vec<f32> = vec![];
    let store_stride = width as usize * 3usize * std::mem::size_of::<f32>();
    store.resize(width as usize * 3usize * height as usize, 0f32);
    let mut alpha_store: Vec<f32> = vec![];
    let alpha_stride = width as usize * std::mem::size_of::<f32>();
    alpha_store.resize(width as usize * height as usize, 0f32);
    rgba_to_laba(original, src_stride, &mut store, store_stride as u32, &mut alpha_store, alpha_stride as u32, width, height);
    append_alpha(destination, dst_stride, &store, store_stride as u32, &alpha_store, alpha_stride as u32, width, height);
}

#[no_mangle]
#[allow(dead_code)]
pub extern "C-unwind" fn aire_reformat_surface_u8_to_laba(
    src: *const u8,
    src_stride: u32,
    dst: *const f32,
    dst_stride: u32,
    width: u32,
    height: u32,
) {
    let src_slice = unsafe { slice::from_raw_parts(src, src_stride as usize * height as usize) };
    let dst_slice =
        unsafe { slice::from_raw_parts_mut(dst as *mut u8, dst_stride as usize * height as usize) };
    reformat_surface_u8_to_laba(src_slice, src_stride, unsafe { transmute(dst_slice) }, dst_stride, width, height);
}

pub fn reformat_surface_laba_to_u8(
    src: &[u8],
    src_stride: u32,
    dst: &mut [u8],
    dst_stride: u32,
    width: u32,
    height: u32,
) {
    let mut a_store = vec![];
    a_store.resize(width as usize * height as usize, 0f32);
    let mut lab_store = vec![];
    lab_store.resize(width as usize * height as usize * 3usize, 0f32);

    let lab_stride = width as usize * 3usize * std::mem::size_of::<f32>();

    let mut src_shift = 0usize;
    let mut a_shift = 0usize;
    let mut lab_shift = 0usize;
    for _ in 0..height as usize {
        let src_ptr = unsafe { (src.as_ptr() as *mut u8).add(src_shift) as *mut f32 };
        let src_slice = unsafe { slice::from_raw_parts(src_ptr, width as usize * 4) };

        for x in 0..width as usize {
            let px = x * 4;
            let lx = x * 3;
            lab_store[lab_shift + lx] = src_slice[px];
            lab_store[lab_shift + lx + 1] = src_slice[px + 1];
            lab_store[lab_shift + lx + 2] = src_slice[px + 2];
            a_store[a_shift + x] = 1f32;
        }
        src_shift += src_stride as usize;
        a_shift += width as usize;
        lab_shift += width as usize * 3usize;
    }

    laba_to_srgb(&lab_store, lab_stride as u32, &a_store, width * std::mem::size_of::<f32>() as u32, dst, dst_stride, width, height);
}

#[no_mangle]
#[allow(dead_code)]
pub extern "C-unwind" fn aire_reformat_surface_laba_to_u8(
    src: *const f32,
    src_stride: u32,
    dst: *const u8,
    dst_stride: u32,
    width: u32,
    height: u32,
) {
    let src_slice = unsafe { slice::from_raw_parts(src as *const u8, src_stride as usize * height as usize) };
    let dst_slice =
        unsafe { slice::from_raw_parts_mut(dst as *mut u8, dst_stride as usize * height as usize) };
    reformat_surface_laba_to_u8(src_slice, src_stride, dst_slice, dst_stride, width, height);
}

pub fn reformat_surface_u8_to_luva(
    original: &[u8],
    src_stride: u32,
    destination: &mut [u8],
    dst_stride: u32,
    width: u32,
    height: u32,
) {
    let mut src_shift = 0usize;
    let mut dst_shift = 0usize;
    for _ in 0..height as usize {
        let src_ptr = unsafe { original.as_ptr().add(src_shift) };
        let src_slice = unsafe { slice::from_raw_parts(src_ptr, width as usize * 4) };
        let dst_ptr = unsafe { destination.as_mut_ptr().add(dst_shift) as *mut f32 };
        let dst_slice = unsafe { slice::from_raw_parts_mut(dst_ptr, width as usize * 4) };

        for x in 0..width as usize {
            let px = x * 4;
            let src_pixel = Rgb::new(
                src_slice[px],
                src_slice[px + 1],
                src_slice[px + 2],
            );
            let lab = src_pixel.to_luv();
            let a_f32 = src_slice[px + 3] as f32 / 255f32;
            let laba_pixel = Rgba::<f32>::new(
                lab.l,
                lab.u,
                lab.v,
                a_f32,
            );
            dst_slice[px] = laba_pixel.r;
            dst_slice[px + 1] = laba_pixel.g;
            dst_slice[px + 2] = laba_pixel.b;
            dst_slice[px + 3] = laba_pixel.a;
        }
        dst_shift += dst_stride as usize;
        src_shift += src_stride as usize;
    }
}

#[no_mangle]
#[allow(dead_code)]
pub extern "C-unwind" fn aire_reformat_surface_u8_to_luva(
    src: *const u8,
    src_stride: u32,
    dst: *const f32,
    dst_stride: u32,
    width: u32,
    height: u32,
) {
    let src_slice = unsafe { slice::from_raw_parts(src, src_stride as usize * height as usize) };
    let dst_slice =
        unsafe { slice::from_raw_parts_mut(dst as *mut u8, dst_stride as usize * height as usize) };
    reformat_surface_u8_to_luva(src_slice, src_stride, dst_slice, dst_stride, width, height);
}

pub fn reformat_surface_luva_to_u8(
    src: &[u8],
    src_stride: u32,
    dst: &mut [u8],
    dst_stride: u32,
    width: u32,
    height: u32,
) {
    let mut src_shift = 0usize;
    let mut dst_shift = 0usize;
    for _ in 0..height as usize {
        let src_ptr = unsafe { (src.as_ptr() as *mut u8).add(src_shift) as *mut f32 };
        let src_slice = unsafe { slice::from_raw_parts(src_ptr, width as usize * 4) };
        let dst_ptr = unsafe { dst.as_mut_ptr().add(dst_shift) };
        let dst_slice = unsafe { slice::from_raw_parts_mut(dst_ptr, width as usize * 4) };

        for x in 0..width as usize {
            let px = x * 4;
            let lab = Luv::new(src_slice[px], src_slice[px + 1], src_slice[px + 2]);
            let rgb_pixel = lab.to_rgb();
            let alpha = (src_slice[px + 3] * 255f32).min(255f32).max(0f32) as u8;
            dst_slice[px] = rgb_pixel.r;
            dst_slice[px + 1] = rgb_pixel.g;
            dst_slice[px + 2] = rgb_pixel.b;
            dst_slice[px + 3] = alpha;
        }
        dst_shift += dst_stride as usize;
        src_shift += src_stride as usize;
    }
}

#[no_mangle]
#[allow(dead_code)]
pub extern "C-unwind" fn aire_reformat_surface_luva_to_u8(
    src: *const f32,
    src_stride: u32,
    dst: *const u8,
    dst_stride: u32,
    width: u32,
    height: u32,
) {
    let src_slice = unsafe { slice::from_raw_parts(src as *const u8, src_stride as usize * height as usize) };
    let dst_slice =
        unsafe { slice::from_raw_parts_mut(dst as *mut u8, dst_stride as usize * height as usize) };
    reformat_surface_luva_to_u8(src_slice, src_stride, dst_slice, dst_stride, width, height);
}