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

    override fun erode(bitmap: Bitmap, kernelSize: Int): Bitmap {
        return erodePipeline(bitmap, kernelSize)
    }

    override fun dilate(bitmap: Bitmap, kernelSize: Int): Bitmap {
        return dilatePipeline(bitmap, kernelSize)
    }

    override fun threshold(bitmap: Bitmap, level: Int): Bitmap {
        return thresholdPipeline(bitmap, level)
    }

    override fun vibrance(bitmap: Bitmap, vibrance: Float): Bitmap {
        return vibrancePipeline(bitmap, vibrance)
    }

    override fun saturation(bitmap: Bitmap, saturation: Float): Bitmap {
        return saturationImpl(bitmap, saturation)
    }

    override fun contrast(bitmap: Bitmap, gain: Float): Bitmap {
        return contrastImpl(bitmap, gain)
    }

    override fun colorMatrix(bitmap: Bitmap, colorMatrix: FloatArray): Bitmap {
        return colorMatrixImpl(bitmap, colorMatrix)
    }

    override fun brightness(bitmap: Bitmap, bias: Float): Bitmap {
        return brightnessImpl(bitmap, bias)
    }

    override fun emboss(bitmap: Bitmap, intensity: Float): Bitmap {
        return embossImpl(bitmap, intensity)
    }

    override fun grain(bitmap: Bitmap, intensity: Float): Bitmap {
        return grainImpl(bitmap, intensity)
    }

    override fun sharpness(bitmap: Bitmap, intensity: Float): Bitmap {
        return sharpnessImpl(bitmap, intensity)
    }

    override fun unsharp(bitmap: Bitmap, intensity: Float): Bitmap {
        return unsharpImpl(bitmap, intensity)
    }

    private external fun unsharpImpl(bitmap: Bitmap, intensity: Float = 1f): Bitmap

    private external fun sharpnessImpl(bitmap: Bitmap, intensity: Float = 1f): Bitmap

    private external fun grainImpl(bitmap: Bitmap, intensity: Float): Bitmap

    private external fun embossImpl(bitmap: Bitmap, intensity: Float): Bitmap

    private external fun colorMatrixImpl(bitmap: Bitmap, colorMatrix: FloatArray): Bitmap

    private external fun contrastImpl(bitmap: Bitmap, gain: Float): Bitmap

    private external fun brightnessImpl(bitmap: Bitmap, bias: Float): Bitmap

    private external fun grayscalePipeline(
        bitmap: Bitmap, rPrimary: Float,
        gPrimary: Float,
        bPrimary: Float
    ): Bitmap

    private external fun saturationImpl(bitmap: Bitmap, saturation: Float): Bitmap

    private external fun vibrancePipeline(bitmap: Bitmap, vibrance: Float): Bitmap

    private external fun erodePipeline(bitmap: Bitmap, kernelSize: Int): Bitmap

    private external fun dilatePipeline(bitmap: Bitmap, kernelSize: Int): Bitmap

    private external fun thresholdPipeline(bitmap: Bitmap, level: Int): Bitmap
}