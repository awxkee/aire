package com.awxkee.aire.pipeline

import android.graphics.Bitmap
import com.awxkee.aire.BlurPipelines
import com.awxkee.aire.MedianSelector

class BlurPipelinesImpl : BlurPipelines {
    override fun gaussianBlur(bitmap: Bitmap, radius: Int, sigma: Float): Bitmap {
        if (radius < 1) {
            throw IllegalStateException("Radius must be more or equal 1")
        }
        if (sigma < 0) {
            throw IllegalStateException("Sigma must be more than 0")
        }
        return gaussianBlurPipeline(bitmap, radius, sigma)
    }

    override fun bilateralBlur(
        bitmap: Bitmap,
        radius: Int,
        rangeSigma: Float,
        spatialSigma: Float
    ): Bitmap {
        if (radius < 1) {
            throw IllegalStateException("Radius must be more or equal 1")
        }
        if (rangeSigma < 0 || spatialSigma < 0) {
            throw IllegalStateException("Sigma must be more than 0")
        }
        return bilateralBlurPipeline(bitmap, radius, rangeSigma, spatialSigma)
    }

    override fun fastBilateralBlur(
        bitmap: Bitmap,
        rangeSigma: Float,
        spatialSigma: Float
    ): Bitmap {
        return fastBilateralPipeline(bitmap, rangeSigma, spatialSigma)
    }

    override fun boxBlur(bitmap: Bitmap, radius: Int): Bitmap {
        if (radius < 1) {
            throw IllegalStateException("Radius must be more or equal 1")
        }
        return boxBlurPipeline(bitmap, radius)
    }

    override fun poissonBlur(bitmap: Bitmap, radius: Int): Bitmap {
        if (radius < 1) {
            throw IllegalStateException("Radius must be more or equal 1")
        }
        return poissonBlurPipeline(bitmap, radius)
    }

    override fun stackBlur(bitmap: Bitmap, radius: Int): Bitmap {
        if (radius < 1) {
            throw IllegalStateException("Radius must be more or equal 1")
        }
        return stackNativeBlurPipeline(bitmap, radius)
    }

    override fun medianBlur(bitmap: Bitmap, kernelSize: Int): Bitmap {
        if (kernelSize < 1) {
            throw IllegalStateException("Radius must be more or equal 1")
        }
        return medianBlurPipeline(bitmap, kernelSize)
    }

    override fun tentBlur(bitmap: Bitmap, radius: Int): Bitmap {
        if (radius < 3) {
            throw IllegalStateException("Radius must be more or equal 1")
        }
        return tentBlurPipeline(bitmap, radius)
    }

    override fun anisotropicDiffusion(
        bitmap: Bitmap,
        numOfSteps: Int,
        conduction: Float,
        diffusion: Float
    ): Bitmap {
        return anisotropicDiffusionPipeline(bitmap, numOfSteps, conduction, diffusion)
    }

    private external fun anisotropicDiffusionPipeline(
        bitmap: Bitmap,
        numOfSteps: Int,
        conduction: Float,
        diffusion: Float
    ): Bitmap

    private external fun poissonBlurPipeline(bitmap: Bitmap, radius: Int): Bitmap

    private external fun fastBilateralPipeline(bitmap: Bitmap, radiusSigma: Float, spatialSigma: Float): Bitmap

    private external fun gaussianBlurPipeline(bitmap: Bitmap, radius: Int, sigma: Float): Bitmap

    private external fun bilateralBlurPipeline(
        bitmap: Bitmap,
        radius: Int,
        sigma: Float,
        spatialSigma: Float
    ): Bitmap

    private external fun medianBlurPipeline(bitmap: Bitmap, radius: Int): Bitmap

    private external fun stackNativeBlurPipeline(bitmap: Bitmap, radius: Int): Bitmap

    private external fun boxBlurPipeline(bitmap: Bitmap, radius: Int): Bitmap

    private external fun tentBlurPipeline(bitmap: Bitmap, radius: Int): Bitmap
}