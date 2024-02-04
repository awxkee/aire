package com.awxkee.aire

enum class BitmapScaleMode(internal val value: Int) {
    Bilinear(1),
    Nearest(2),
    Cubic(3),
    MitchellNetravali(4),
    Lanczos(5),
    CatmullRom(6),
    Hermite(7),
    BSpline(8),
    Hann(9),
    BiCubic(10)
}