# Aire

## Simple image processing in android

Image processing speed increased by [libhwy](https://github.com/google/highway)

# Usage

```kotlin
Aire.gaussianBlur(bitmap, ...)
```

# Done filters

## Blur:

- [x] Gaussian Blur
- [x] Stack Blur
- [x] Poisson blur
- [x] Tent Blur
- [x] Median Blur
- [x] Bilateral Blur
- [x] Fast Bilateral Approximation
- [x] Anisotropic Diffusion

## Base operations:

- [x] Saturation
- [x] Contrast
- [x] Brightness
- [x] Sharp
- [x] Unsharp
- [x] Gamma
- [x] Crop
- [x] Rotate
- [x] Affine transform
- [x] Vibrance
- [x] Dilate
- [x] Erode
- [x] Exposure
- [x] Scaling ( a lot of methods )

## Effects:

- [x] Bokeh
- [x] Fractal Effect
- [x] Marble
- [x] Oil
- [x] Water effect
- [x] Glitch
- [x] Tilt Shift
- [x] Wind Stagger
- [x] Monochrome

## Postprocessing:

- [x] Dehaze dark channel

## Compression:

- [x] Mozjpeg compression
- [x] PNG Quantize
- [x] Palette quantize]

## Tone Mapping:
- [x] Uchimura
- [x] Hable
- [x] Aces Filmic
- [x] Aces Hill
- [x] Hejl Burgess
- [x] Logarithmic
- [x] Mobius
- [x] Aldridge
- [x] Drago