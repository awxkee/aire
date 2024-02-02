package com.awxkee.aire.pipeline

import android.graphics.Bitmap
import com.awxkee.aire.BlurPipelines

class BlurPipelinesImpl: BlurPipelines {
    override fun gaussianBlur(bitmap: Bitmap, radius: Int, sigma: Float): Bitmap {
        if (radius < 1) {
            throw IllegalStateException("Radius must be more or equal 1")
        }
        if (sigma < 0) {
            throw IllegalStateException("Sigma must be more than 0")
        }
        return gaussianBlurPipeline(bitmap, radius, sigma)
    }

    override fun bilateralBlur(bitmap: Bitmap, radius: Int, rangeSigma: Float, spatialSigma: Float): Bitmap {
        if (radius < 1) {
            throw IllegalStateException("Radius must be more or equal 1")
        }
        if (rangeSigma < 0 || spatialSigma < 0) {
            throw IllegalStateException("Sigma must be more than 0")
        }
        return bilateralBlurPipeline(bitmap, radius, rangeSigma, spatialSigma)
    }

    override fun boxBlur(bitmap: Bitmap, radius: Int): Bitmap {
        if (radius < 1) {
            throw IllegalStateException("Radius must be more or equal 1")
        }
        return boxBlurPipeline(bitmap, radius)
    }

    override fun stackBlur(bitmap: Bitmap, radius: Int): Bitmap {
        if (radius < 1) {
            throw IllegalStateException("Radius must be more or equal 1")
        }
        return stackNativeBlurPipeline(bitmap, radius)
    }

    override fun medianBlur(bitmap: Bitmap, radius: Int): Bitmap {
        if (radius < 1) {
            throw IllegalStateException("Radius must be more or equal 1")
        }
        return medianBlurPipeline(bitmap, radius)
    }

    override fun tentBlur(bitmap: Bitmap, radius: Int): Bitmap {
        if (radius < 1) {
            throw IllegalStateException("Radius must be more or equal 1")
        }
        return tentBlurPipeline(bitmap, radius)
    }

    private external fun gaussianBlurPipeline(bitmap: Bitmap, radius: Int, sigma: Float): Bitmap

    private external fun bilateralBlurPipeline(bitmap: Bitmap, radius: Int, sigma: Float, spatialSigma: Float): Bitmap

    private external fun medianBlurPipeline(bitmap: Bitmap, radius: Int): Bitmap

    private external fun stackNativeBlurPipeline(bitmap: Bitmap, radius: Int): Bitmap

    private external fun boxBlurPipeline(bitmap: Bitmap, radius: Int): Bitmap

    private external fun tentBlurPipeline(bitmap: Bitmap, radius: Int): Bitmap
}