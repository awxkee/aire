package com.awxkee.aire

import android.graphics.Bitmap
import androidx.annotation.IntRange

interface BasePipelines {
    fun grayscale(
        bitmap: Bitmap,
        rPrimary: Float = 0.299f,
        gPrimary: Float = 0.587f,
        bPrimary: Float = 0.114f
    ): Bitmap

    fun erode(bitmap: Bitmap, kernelSize: Int): Bitmap

    fun dilate(bitmap: Bitmap, kernelSize: Int): Bitmap

    fun threshold(bitmap: Bitmap, @IntRange(from = 0, to = 255) level: Int): Bitmap

    fun vibrance(bitmap: Bitmap, vibrance: Float): Bitmap
}