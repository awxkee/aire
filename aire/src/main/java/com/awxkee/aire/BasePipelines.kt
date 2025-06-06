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
import androidx.annotation.FloatRange
import androidx.annotation.IntRange

interface BasePipelines {

    /**
     *  Performs morphology on the image
     *
     * @param morphOp - See [MorphOp] for more info
     * @param morphOpMode - See [MorphOpMode] for more info
     * @param borderMode - Edge handling mode
     * @param borderScalar - If [EdgeMode.CONSTANT] selected then this scalar value be used
     * @param kernel - kernel is arbitrary shaped
     */
    fun morphology(
        bitmap: Bitmap,
        morphOp: MorphOp,
        morphOpMode: MorphOpMode,
        borderMode: EdgeMode,
        borderScalar: Scalar,
        kernel: IntArray,
        kernelWidth: Int,
        kernelHeight: Int
    ): Bitmap

    fun getBokehKernel(
        @IntRange(from = 3) kernelSize: Int,
        @IntRange(from = 3) sides: Int = 6,
    ): IntArray

    fun getBokehConvolutionKernel(
        @IntRange(from = 3) kernelSize: Int,
        @IntRange(from = 3) sides: Int = 6,
    ): FloatArray

    fun grayscale(
        bitmap: Bitmap,
        rPrimary: Float = 0.299f,
        gPrimary: Float = 0.587f,
        bPrimary: Float = 0.114f
    ): Bitmap

    fun threshold(bitmap: Bitmap, @IntRange(from = 0, to = 255) level: Int): Bitmap

    fun vibrance(bitmap: Bitmap, vibrance: Float): Bitmap

    /**
     *  Performs saturation on the image
     *
     *  Turn on tone mapping on big values positive, or negative to compress colors in visible range
     */
    fun saturation(
        bitmap: Bitmap,
        @FloatRange(
            from = Double.NEGATIVE_INFINITY,
            to = Double.POSITIVE_INFINITY
        ) saturation: Float = 1f,
        tonemap: Boolean,
    ): Bitmap

    fun contrast(bitmap: Bitmap, gain: Float = 1.0f): Bitmap

    fun brightness(bitmap: Bitmap, bias: Float = 0.0f): Bitmap

    fun emboss(bitmap: Bitmap, intensity: Float): Bitmap

    fun grain(bitmap: Bitmap, intensity: Float = 0.75f): Bitmap

    fun sharpness(bitmap: Bitmap, kernelSize: Int): Bitmap

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