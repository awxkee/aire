package com.awxkee.aire

import androidx.annotation.Keep

@Keep
enum class AireQuantize(internal val value: Int) {
    MEDIAN_CUT(1),
    XIAOLING_WU(2)
}