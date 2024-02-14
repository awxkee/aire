package com.awxkee.aire

import androidx.annotation.Keep

@Keep
enum class AirePaletteDithering(internal val value: Int) {
    SKIP(0),
    FLOYD_STEINBERG(1),
    JARVIS_JUDICE_NINKE(2),
}