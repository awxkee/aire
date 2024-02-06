package com.awxkee.aire

import android.graphics.Bitmap

interface ScalePipelines {
    fun scale(
        bitmap: Bitmap,
        dstWidth: Int,
        dstHeight: Int,
        scaleMode: BitmapScaleMode,
        antialias: Boolean,
    ): Bitmap
}