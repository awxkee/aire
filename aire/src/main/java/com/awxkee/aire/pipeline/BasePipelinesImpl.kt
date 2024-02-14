package com.awxkee.aire.pipeline

import android.graphics.Bitmap
import androidx.annotation.IntRange
import com.awxkee.aire.AireColorMapper
import com.awxkee.aire.AirePaletteDithering
import com.awxkee.aire.AireQuantize
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

    override fun dilate(bitmap: Bitmap, kernel: FloatArray): Bitmap {
        return dilatePipeline(bitmap, kernel)
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

    override fun gamma(bitmap: Bitmap, gamma: Float): Bitmap {
        return gammaImpl(bitmap, gamma)
    }

    override fun toPNG(
        bitmap: Bitmap,
        maxColors: Int,
        quantize: AireQuantize,
        dithering: AirePaletteDithering,
        colorMapper: AireColorMapper,
        @IntRange(from = 0.toLong(), to = 9.toLong()) compressionLevel: Int
    ): ByteArray {
        return toPNGImpl(
            bitmap,
            maxColors,
            quantize.value,
            dithering.value,
            colorMapper.value,
            compressionLevel
        )
    }

    override fun palette(
        bitmap: Bitmap,
        maxColors: Int,
        quantize: AireQuantize,
        dithering: AirePaletteDithering,
        colorMapper: AireColorMapper
    ): Bitmap {
        return paletteImpl(bitmap, maxColors, quantize.value, dithering.value, colorMapper.value)
    }

    private external fun paletteImpl(
        bitmap: Bitmap,
        maxColors: Int,
        quantize: Int,
        dithering: Int,
        mappingStrategy: Int,
    ): Bitmap

    private external fun toPNGImpl(
        bitmap: Bitmap,
        maxColors: Int,
        quantize: Int,
        dithering: Int,
        mappingStrategy: Int,
        compressionLevel: Int,
    ): ByteArray

    private external fun gammaImpl(bitmap: Bitmap, gamma: Float): Bitmap

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

    private external fun dilatePipeline(bitmap: Bitmap, kernel: FloatArray): Bitmap

    private external fun thresholdPipeline(bitmap: Bitmap, level: Int): Bitmap
}