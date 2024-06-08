#[derive(Copy, Clone, PartialOrd, PartialEq)]
pub struct ImageSingleHistogram {
    pub bins: [u64; 256],
}

#[derive(Copy, Clone, PartialOrd, PartialEq)]
pub struct ImageHistogram {
    pub r_bins: [u64; 256],
    pub g_bins: [u64; 256],
    pub b_bins: [u64; 256],
}

#[derive(Copy, Clone, PartialOrd, PartialEq)]
pub struct ImageSingle100Histogram {
    pub bins: [u64; 101],
}

pub fn make_histogram_channel_3_region(
    in_place: &[f32],
    stride: u32,
    start_x: u32,
    end_x: u32,
    start_y: u32,
    end_y: u32,
) -> ImageSingle100Histogram {
    let mut bins: [u64; 101] = [0u64; 101];
    let mut y_shift = 0usize;

    for _ in 0..(end_y - start_y) as usize {
        for x in 0..(end_x - start_x) as usize {
            let px = x * 3;
            let vl = (in_place[y_shift + px + 2] * 100f32)
                .round()
                .min(100f32)
                .max(0f32) as usize;
            bins[vl] += 1u64;
        }
        y_shift += stride as usize;
    }

    let hist = ImageSingle100Histogram { bins };
    hist
}
