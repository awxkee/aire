use colorutils_rs::{Hsv, Rgb};
use histogram_equalization::ClaheGridSize;

use EqualizeHistogramChannels::{Channels3, Channels4};

use crate::histogram::make_histogram_channel_3_region;

#[repr(u8)]
#[derive(Copy, Clone, PartialOrd, PartialEq)]
pub enum EqualizeHistogramChannels {
    Channels3 = 0,
    Channels4 = 1,
}

impl From<u8> for EqualizeHistogramChannels {
    fn from(value: u8) -> Self {
        match value {
            0 => Channels3,
            1 => Channels4,
            _ => panic!("Not implemented"),
        }
    }
}

pub fn minmax(slice: &[u64]) -> (u64, u64) {
    let mut min_value = u64::MAX;
    let mut max_value = u64::MIN;
    let mut i = 0usize;
    let count = slice.len();

    while i < count {
        let value = slice[i];
        if value < min_value {
            min_value = value;
        }
        if value > max_value {
            max_value = value;
        }
        i += 1;
    }
    (min_value, max_value)
}

pub fn cdf(arr: &mut [u64]) {
    let mut sum: u64 = 0u64;
    for i in 0..arr.len() {
        sum += arr[i];
        arr[i] = sum;
    }
}

fn equalize_histogram_region_hsv<const CHANNELS: u8>(
    in_place: &mut [u8],
    stride: u32,
    start_x: u32,
    end_x: u32,
    start_y: u32,
    end_y: u32,
    width: u32,
    _: u32,
) {
    let h_channels: EqualizeHistogramChannels = CHANNELS.into();
    let channels: usize;
    match h_channels {
        Channels3 => {
            channels = 3;
        }
        Channels4 => {
            channels = 4;
        }
    }

    let hist_width = end_x as usize - start_x as usize;
    let hist_height = end_y as usize - start_y as usize;

    let mut hsl_image: Vec<f32> = Vec::new();
    hsl_image.resize(hist_width * 3usize * hist_height, 0f32);

    let mut y_shift = stride as usize * start_y as usize;
    let mut hsl_shift = 0usize;
    for _ in start_y as usize..(end_y as usize) {
        for (x, j) in (start_x as usize..width as usize).zip(0..hist_width) {
            let px = x * channels;
            let h_px = j * 3usize;

            let rgb = Rgb::<u8>::new(
                unsafe { *in_place.get_unchecked(y_shift + px) },
                unsafe { *in_place.get_unchecked(y_shift + px + 1) },
                unsafe { *in_place.get_unchecked(y_shift + px + 2) },
            );
            let hsv = rgb.to_hsv();
            unsafe {
                *hsl_image.get_unchecked_mut(hsl_shift + h_px) = hsv.get_hue();
                *hsl_image.get_unchecked_mut(hsl_shift + h_px + 1) = hsv.get_saturation();
                *hsl_image.get_unchecked_mut(hsl_shift + h_px + 2) = hsv.get_value();
            }
        }
        y_shift += stride as usize;
        hsl_shift += hist_width * 3usize;
    }

    let histogram = make_histogram_channel_3_region(
        &hsl_image,
        hist_width as u32 * 3u32,
        start_x,
        end_x,
        start_y,
        end_y,
    );
    let mut bins = histogram.bins;

    cdf(&mut bins);

    let pixels_count = hist_width * hist_height;

    let (min_bin, _) = minmax(&bins);

    let distance_r = 1f64 / (pixels_count as f64 - min_bin as f64);

    let mut f_bins: [f32; 101] = [0f32; 101];

    for i in 0..101usize {
        if distance_r != 0f64 {
            f_bins[i] = (100f64 * (bins[i] as f64 - min_bin as f64) * distance_r)
                .round()
                .min(100f64) as f32
                / 100f32;
        }
    }

    let mut hsl_shift = 0usize;
    let mut y_shift = stride as usize * start_y as usize;
    for _ in start_y as usize..end_y as usize {
        for (x, j) in (start_x as usize..width as usize).zip(0..hist_width) {
            let px = x * channels;
            let h_px = j * 3;

            let vl = (unsafe { *hsl_image.get_unchecked(hsl_shift + h_px + 2) } * 100f32)
                .round()
                .min(100f32)
                .max(0f32) as usize;
            unsafe {
                *hsl_image.get_unchecked_mut(hsl_shift + h_px + 2) = *f_bins.get_unchecked(vl);
            }

            let hsv = Hsv::from_components(
                unsafe { *hsl_image.get_unchecked(hsl_shift + h_px) },
                unsafe { *hsl_image.get_unchecked(hsl_shift + h_px + 1) },
                unsafe { *hsl_image.get_unchecked(hsl_shift + h_px + 2) },
            );
            let rgb = hsv.to_rgb8();

            unsafe {
                *in_place.get_unchecked_mut(y_shift + px) = rgb.r;
                *in_place.get_unchecked_mut(y_shift + px + 1) = rgb.g;
                *in_place.get_unchecked_mut(y_shift + px + 2) = rgb.b;
            }
        }
        y_shift += stride as usize;
        hsl_shift += hist_width * 3usize;
    }
}

#[allow(dead_code)]
pub fn equalize_histogram_squares<const CHANNELS: u8>(
    in_place: &mut [u8],
    stride: u32,
    width: u32,
    height: u32,
    clahe_grid_size: ClaheGridSize,
) {
    if clahe_grid_size.w == 0 || clahe_grid_size.h == 0 {
        panic!("zero sized grid is not accepted");
    }
    let horizontal_tile_size = width / clahe_grid_size.w;
    let vertical_tile_size = height / clahe_grid_size.h;
    let tiles_horizontal = width / horizontal_tile_size;
    let tiles_vertical = height / vertical_tile_size;
    for w in 0..tiles_horizontal {
        for h in 0..tiles_vertical {
            let start_x = w * horizontal_tile_size;
            let start_y = h * vertical_tile_size;
            let mut end_x = (w + 1) * horizontal_tile_size;
            if w + 1 == tiles_horizontal {
                end_x = width;
            }
            let mut end_y = (h + 1) * vertical_tile_size;
            if h + 1 == tiles_vertical {
                end_y = height;
            }
            equalize_histogram_region_hsv::<CHANNELS>(
                in_place, stride, start_x, end_x, start_y, end_y, width, height,
            );
        }
    }
}

#[allow(dead_code)]
pub fn equalize_histogram_squares_rgba(
    dst: &mut [u8],
    dst_stride: u32,
    width: u32,
    height: u32,
    clahe_grid_size: ClaheGridSize,
) {
    equalize_histogram_squares::<{ Channels4 as u8 }>(
        dst,
        dst_stride,
        width,
        height,
        clahe_grid_size,
    );
}
