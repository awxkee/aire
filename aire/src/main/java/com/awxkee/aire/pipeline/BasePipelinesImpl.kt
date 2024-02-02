package com.awxkee.aire.pipeline

import android.graphics.Bitmap
import com.awxkee.aire.BasePipelines

class BasePipelinesImpl : BasePipelines {
    override fun grayscale(
        bitmap: Bitmap,
        rPrimary: Float,
        gPrimary: Float,
        bPrimary: Float
    ): Bitmap {
        return grayscalePipeline(bitmap, rPrimary, gPrimary, bPrimary)
    }

    override fun dilate(bitmap: Bitmap, kernelSize: Int): Bitmap {
        return dilatePipeline(bitmap, kernelSize)
    }

    override fun threshold(bitmap: Bitmap, level: Int): Bitmap {
        return thresholdPipeline(bitmap, level)
    }

    private external fun grayscalePipeline(
        bitmap: Bitmap, rPrimary: Float,
        gPrimary: Float,
        bPrimary: Float
    ): Bitmap

    private external fun dilatePipeline(bitmap: Bitmap, kernelSize: Int): Bitmap

    private external fun thresholdPipeline(bitmap: Bitmap, level: Int): Bitmap
}