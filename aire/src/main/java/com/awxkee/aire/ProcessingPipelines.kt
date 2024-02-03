package com.awxkee.aire

import android.graphics.Bitmap
import androidx.annotation.IntRange

interface ProcessingPipelines {
    fun removeShadows(bitmap: Bitmap, @IntRange(from = 3, to = 7) kernelSize: Int = 5): Bitmap
}