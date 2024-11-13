use colorutils_rs::Rgb;

#[inline]
pub(crate) fn uncharted2_tonemap_partial(x: f32) -> f32 {
    let a = 0.15;
    let b = 0.50;
    let c = 0.10;
    let d = 0.20;
    let e = 0.02;
    let f = 0.30;

    ((x * (a * x + c * b) + d * e) / (x * (a * x + b) + d * f)) - e / f
}

#[inline]
pub(crate) fn uncharted2_filmic(v: f32) -> f32 {
    let exposure_bias = 2.0;
    let curr = uncharted2_tonemap_partial(v * exposure_bias);

    let w = 11.2;
    let white_scale = 1.0 / uncharted2_tonemap_partial(w);

    curr * white_scale
}

#[inline]
pub(crate) fn aces_hill(rgb: Rgb<f32>) -> Rgb<f32> {
    #[inline]
    fn mul_input(color: Rgb<f32>) -> Rgb<f32> {
        let a = 0.59719f32 * color.r + 0.35458f32 * color.g + 0.04823f32 * color.b;
        let b = 0.07600f32 * color.r + 0.90834f32 * color.g + 0.01566f32 * color.b;
        let c = 0.02840f32 * color.r + 0.13383f32 * color.g + 0.83777f32 * color.b;
        Rgb::new(a, b, c)
    }

    #[inline]
    fn mul_output(color: Rgb<f32>) -> Rgb<f32> {
        let a = 1.60475f32 * color.r - 0.53108f32 * color.g - 0.07367f32 * color.b;
        let b = -0.10208f32 * color.r + 1.10813f32 * color.g - 0.00605f32 * color.b;
        let c = -0.00327f32 * color.r - 0.07276f32 * color.g + 1.07602f32 * color.b;
        Rgb::new(a, b, c)
    }

    let color_in = mul_input(rgb);
    let ca = color_in * (color_in + Rgb::dup(0.0245786f32)) - Rgb::dup(0.000090537f32);
    let cb = color_in * (Rgb::dup(0.983729f32) * color_in + Rgb::dup(0.4329510f32))
        + Rgb::dup(0.238081f32);
    let c_out = mul_output(ca / cb);
    c_out
}
