<h1 align="center">

Aire

</h1>


<p align="center">
  <img alt="API" src="https://img.shields.io/badge/Api%2021+-50f270?logo=android&logoColor=black&style=for-the-badge"/></a>
  <img alt="Kotlin" src="https://img.shields.io/badge/Kotlin-a503fc?logo=kotlin&logoColor=white&style=for-the-badge"/></a>
  <img alt="Cpp" src="https://img.shields.io/static/v1?style=for-the-badge&message=C%2B%2B&color=00599C&logo=C%2B%2B&logoColor=FFFFFF&label="/></a> 
  <a href="https://hits.sh/github.com/awxkee/aire/"><img alt="Hits" src="https://hits.sh/github.com/awxkee/aire.svg?style=for-the-badge&label=Views&extraCount=10&color=54856b"/></a>
  <img src="https://img.shields.io/github/v/release/awxkee/aire?style=for-the-badge"/>
</p>

<div align="center">
            
Fast & Simple image processing library for Android with 50+ available operations

Image processing speed increased by [libhwy](https://github.com/google/highway)

</div>

## Usage

### 1. Add dependencies

#### Kotlin (kts)
```kotlin
repositories {
  maven { setUrl("https://jitpack.io") } // Add jitpack
}
dependencies {
  implementation("com.github.awxkee:aire:LATEST_VERSION") // Replace "LATEST_VERSION" with preferrend version tag
}
```

#### Groovy
```groovy
repositories {
  maven { url 'https://jitpack.io' } // Add jitpack
}
dependencies {
  implementation 'com.github.awxkee:aire:LATEST_VERSION' // Replace "LATEST_VERSION" with preferrend version tag
}
```

### 2. Add `Aire` call as shown below

```kotlin
Aire.gaussianBlur(
    bitmap = input,
    //Addional parameters
)
```

## Available filters

### Blur:

- [x] Gaussian Blur
- [x] Stack Blur
- [x] Poisson blur
- [x] Tent Blur
- [x] Median Blur
- [x] Bilateral Blur
- [x] Fast Bilateral Approximation
- [x] Anisotropic Diffusion
- [x] Fast Gaussian Approximation
- [x] Zoom Blur

### Base operations:

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
- [x] Convolve 2D
- [x] Scaling
    * Bilinear
    * Nearest Neighbour
    * Bicubic
    * Mitchell
    * Lanczos3 ( Sinc )
    * Catmull
    * Hermite
    * Spline
    * BSpline
    * Hann
    * Lanczos3 ( Jinc )

### Effects:

- [x] Bokeh
- [x] Fractal Effect
- [x] Marble
- [x] Oil
- [x] Water effect
- [x] Glitch
- [x] Radial Tilt Shift
- [x] Horizontal Tilt Shift
- [x] Wind Stagger
- [x] Monochrome
- [x] Grain

### Postprocessing:

- [x] Dehaze dark channel
- [x] Remove shadows

### Compression:

- [x] Mozjpeg compression
- [x] PNG Quantize
- [x] Palette quantize

### Tone Mapping:
- [x] Uchimura
- [x] Hable
- [x] Aces Filmic
- [x] Aces Hill
- [x] Hejl Burgess
- [x] Logarithmic
- [x] Mobius
- [x] Aldridge
- [x] Drago

## Find this repository useful? :heart:
Support it by joining __[stargazers](https://github.com/awxkee/aire/stargazers)__ for this repository. :star: <br>
And __[follow](https://github.com/awxkee)__ me for my next creations! 🤩

## License
```xml
MIT License

Copyright (c) 2024 Radzivon Bartoshyk

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
```
