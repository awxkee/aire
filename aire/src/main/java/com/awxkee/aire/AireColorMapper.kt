package com.awxkee.aire

import androidx.annotation.Keep

@Keep
enum class AireColorMapper(internal val value: Int) {
    /**
     * The most accurate and incredibly slow
     */
    LINEAR(0),

    /**
     * The fastest and enough accurate, sometimes accuracy for small palettes not very good for ex. ~256 colors
     */
    KD_TREE(1),

    /**
     * Optimal for speed and accuracy
     */
    COVER_TREE(2)
}