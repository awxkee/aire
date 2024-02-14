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

    fun dilate(bitmap: Bitmap, kernel: FloatArray): Bitmap

    fun threshold(bitmap: Bitmap, @IntRange(from = 0, to = 255) level: Int): Bitmap

    fun vibrance(bitmap: Bitmap, vibrance: Float): Bitmap

    fun saturation(bitmap: Bitmap, saturation: Float = 1f): Bitmap

    fun contrast(bitmap: Bitmap, gain: Float = 1.0f): Bitmap

    fun brightness(bitmap: Bitmap, bias: Float = 0.0f): Bitmap

    fun emboss(bitmap: Bitmap, intensity: Float): Bitmap

    fun grain(bitmap: Bitmap, intensity: Float = 0.75f): Bitmap

    fun sharpness(bitmap: Bitmap, intensity: Float = 1f): Bitmap

    fun unsharp(bitmap: Bitmap, intensity: Float = 1f): Bitmap

    fun gamma(bitmap: Bitmap, gamma: Float = 1f): Bitmap

    fun toPNG(
        bitmap: Bitmap,
        maxColors: Int,
        quantize: AireQuantize = AireQuantize.XIAOLING_WU,
        dithering: AirePaletteDithering = AirePaletteDithering.JARVIS_JUDICE_NINKE,
        colorMapper: AireColorMapper = AireColorMapper.KD_TREE,
        @IntRange(from = 0, to = 9) compressionLevel: Int = 7,
    ): ByteArray

    fun palette(
        bitmap: Bitmap,
        maxColors: Int,
        quantize: AireQuantize = AireQuantize.XIAOLING_WU,
        dithering: AirePaletteDithering = AirePaletteDithering.JARVIS_JUDICE_NINKE,
        colorMapper: AireColorMapper = AireColorMapper.KD_TREE,
    ): Bitmap

    fun getStructuringKernel(kernelSize: Int): FloatArray {
        val kern = FloatArray(kernelSize * kernelSize) {
            1f
        }
        return kern
    }

    /**
     * @param colorMatrix - Only 3x3 matrix allowed, some matrices are available in `ColorMatrices`
     */
    fun colorMatrix(bitmap: Bitmap, colorMatrix: FloatArray): Bitmap
}