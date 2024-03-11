/*
 *
 *  * MIT License
 *  *
 *  * Copyright (c) 2024 Radzivon Bartoshyk
 *  * aire [https://github.com/awxkee/aire]
 *  *
 *  * Created by Radzivon Bartoshyk on 2/21/24, 6:13 PM
 *  *
 *  * Permission is hereby granted, free of charge, to any person obtaining a copy
 *  * of this software and associated documentation files (the "Software"), to deal
 *  * in the Software without restriction, including without limitation the rights
 *  * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 *  * copies of the Software, and to permit persons to whom the Software is
 *  * furnished to do so, subject to the following conditions:
 *  *
 *  * The above copyright notice and this permission notice shall be included in all
 *  * copies or substantial portions of the Software.
 *  *
 *  * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *  * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *  * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 *  * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *  * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 *  * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 *  * SOFTWARE.
 *  *
 *
 */

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

    fun crop(bitmap: Bitmap, baseX: Int, baseY: Int, width: Int, height: Int): Bitmap

    fun rotate(
        bitmap: Bitmap,
        angle: Float,
        anchorPointX: Int,
        anchorPointY: Int,
        newWidth: Int,
        newHeight: Int
    ): Bitmap

    /**
     * @param transform - 3D affine transform 3x3 float array
     */
    fun warpAffine(bitmap: Bitmap, transform: FloatArray, newWidth: Int, newHeight: Int): Bitmap

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

    /**
     * Mozjpeg jpeg compression
     */
    fun mozjpeg(bitmap: Bitmap, quality: Int = 76): ByteArray

    /**
     * Jpegli jpeg compression
     */
    fun jpegli(bitmap: Bitmap, quality: Int = 76): ByteArray

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