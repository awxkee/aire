package com.awxkee.aire.pipeline

import android.graphics.Bitmap
import com.awxkee.aire.BitmapScaleMode
import com.awxkee.aire.ScalePipelines

class ScalePipelinesImpl : ScalePipelines {
    override fun scale(
        bitmap: Bitmap,
        dstWidth: Int,
        dstHeight: Int,
        scaleMode: BitmapScaleMode,
        antialias: Boolean
    ): Bitmap {
        return scaleImpl(bitmap, dstWidth, dstHeight, scaleMode.value, antialias)
    }

    private external fun scaleImpl(
        bitmap: Bitmap,
        dstWidth: Int,
        dstHeight: Int,
        scaleMode: Int,
        antialias: Boolean,
    ): Bitmap
}