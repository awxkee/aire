pub mod gamma_curve {
    pub fn rec709_to_linear(v: f32) -> f32 {
        if v < 0f32 {
            return 0f32;
        } else if v < 4.5f32 * 0.018053968510807f32 {
            return v / 4.5f32;
        } else if v < 1f32 {
            return ((v + 0.09929682680944f32) / 1.09929682680944f32).powf(1f32 / 0.45f32);
        }
        return 1f32;
    }

    pub fn rec709_from_linear(linear: f32) -> f32 {
        if linear < 0f32 {
            return 0f32;
        } else if linear < 0.018053968510807f32 {
            return linear * 4.5f32;
        } else if linear < 1f32 {
            return 1.09929682680944f32 * linear.powf(0.45f32) - 0.09929682680944f32;
        }
        return 1f32;
    }
}