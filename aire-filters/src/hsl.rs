use crate::hsv::Hsv;
use crate::lab::Lab;
use crate::luv::Luv;

#[allow(dead_code)]
pub struct Hsl {
    pub h: f32,
    pub s: f32,
    pub l: f32,
}

pub struct Rgb<T> {
    pub r: T,
    pub g: T,
    pub b: T,
}

impl Rgb<u8> {
    #[allow(dead_code)]
    pub fn to_hsl(&self) -> Hsl {
        rgb2hsl(self.r, self.g, self.b)
    }

    #[allow(dead_code)]
    pub fn to_hsv(&self) -> Hsv {
        Hsv::from(self)
    }

    pub fn to_lab(&self) -> Lab {
        Lab::from_rgb(self)
    }

    pub fn to_luv(&self) -> Luv {
        Luv::from_rgb(self)
    }
}

impl<T> Rgb<T> {
    pub(crate) fn new(r: T, g: T, b: T) -> Rgb<T> {
        Rgb { r, g, b }
    }
}

impl Hsl {
    #[allow(dead_code)]
    pub fn new(h: u16, s: u16, l: u16) -> Hsl {
        Hsl { h: h as f32, s: s as f32 / 100f32, l: l as f32 / 100f32 }
    }

    #[allow(dead_code)]
    pub fn to_rgb8(&self) -> Rgb<u8> {
        let c = (1f32 - (2f32 * self.l - 1f32).abs()) * self.s;
        let x = c * (1f32 - ((self.h / 60f32) % 2f32 - 1f32).abs());
        let m = self.l - c / 2f32;

        let (r, g, b) = if self.h >= 0f32 && self.h < 60f32 {
            (c, x, 0f32)
        } else if self.h >= 60f32 && self.h < 120f32 {
            (x, c, 0f32)
        } else if self.h >= 120f32 && self.h < 180f32 {
            (0f32, c, x)
        } else if self.h >= 180f32 && self.h < 240f32 {
            (0f32, x, c)
        } else if self.h >= 240f32 && self.h < 300f32 {
            (x, 0f32, c)
        } else {
            (c, 0f32, x)
        };

        Rgb::<u8> {
            r: ((r + m) * 255f32).round() as u8,
            g: ((g + m) * 255f32).round() as u8,
            b: ((b + m) * 255f32).round() as u8,
        }
    }

    #[allow(dead_code)]
    pub fn get_saturation(&self) -> u16 {
        ((self.s * 100f32) as u16).min(100u16)
    }

    #[allow(dead_code)]
    pub fn get_lightness(&self) -> u16 {
        ((self.l * 100f32) as u16).min(100u16)
    }

    #[allow(dead_code)]
    pub fn get_hue(&self) -> u16 {
        (self.h as u16).min(360)
    }
}

fn rgb2hsl(o_r: u8, o_g: u8, o_b: u8) -> Hsl {
    let r = o_r as f32 / 255f32;
    let g = o_g as f32 / 255f32;
    let b = o_b as f32 / 255f32;

    let c_max = r.max(g).max(b);
    let c_min = r.min(g).min(b);
    let delta = c_max - c_min;

    let mut h = if delta == 0f32 {
        0f32
    } else if c_max == r {
        60f32 * (((g - b) / delta) % 6f32)
    } else if c_max == g {
        60f32 * (((b - r) / delta) + 2f32)
    } else {
        60f32 * (((r - g) / delta) + 4f32)
    };

    if h < 0f32 {
        h += 360f32;
    }

    let l = 0.5f32 * (c_max + c_min);
    let s = if delta == 0f32 {
        0f32
    } else {
        delta / (1f32 - (2f32 * l - 1f32).abs())
    };

    Hsl { h, s, l }
}