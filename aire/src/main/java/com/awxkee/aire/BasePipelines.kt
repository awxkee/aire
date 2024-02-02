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

    /**
     * Image prefer be threshold before dilate
     */
    fun dilate(bitmap: Bitmap, kernelSize: Int): Bitmap

    /**
     * Image prefer be grayscale before the thresholding
     */
    fun threshold(bitmap: Bitmap, @IntRange(from = 0, to = 255) level: Int): Bitmap
}