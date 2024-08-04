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
import android.graphics.Color
import androidx.annotation.FloatRange
import androidx.annotation.IntRange

interface EffectsPipelines {

    fun convex(bitmap: Bitmap, strength: Float = 1.7f): Bitmap

    fun bokeh(
        bitmap: Bitmap,
        @IntRange(from = 3) kernelSize: Int,
        @IntRange(from = 3) sides: Int = 6,
        enhance: Boolean,
    ): Bitmap

    fun marble(
        bitmap: Bitmap,
        intensity: Float = 0.02f,
        turbulence: Float = 1f,
        amplitude: Float = 1f
    ): Bitmap

    fun perlinDistortion(
        bitmap: Bitmap,
        intensity: Float = 0.02f,
        turbulence: Float = 1f,
        amplitude: Float = 1f
    ): Bitmap

    fun waterEffect(
        bitmap: Bitmap,
        fractionSize: Float = 0.05f,
        frequencyX: Float = 2f,
        amplitudeX: Float = 0.5f,
        frequencyY: Float = 2f,
        amplitudeY: Float = 0.5f,
    ): Bitmap

    fun fractalGlass(bitmap: Bitmap, glassSize: Float = 0.2f, amplitude: Float = 0.2f): Bitmap

    fun oil(bitmap: Bitmap, radius: Int, levels: Float = 1f): Bitmap

    /* Prefer relative clustering for ex. width*height * 0.01f = numClusters */
    fun crystallize(bitmap: Bitmap, numClusters: Int, strokeColor: Int = Color.TRANSPARENT): Bitmap

    fun equalizeHist(bitmap: Bitmap): Bitmap

    /**
     * @param binsCount - Preferrable 128
     */
    fun equalizeHistHSV(bitmap: Bitmap, @IntRange(from = 2) binsCount: Int): Bitmap

    /**
     * @param binsCount - Preferrable 128
     */
    fun equalizeHistHSL(bitmap: Bitmap, @IntRange(from = 2) binsCount: Int): Bitmap

    /**
     * @param binsCount - Preferrable 128
     */
    fun equalizeHistLAB(bitmap: Bitmap, @IntRange(from = 2) binsCount: Int): Bitmap

    /**
     * @param binsCount - Preferrable 128
     */
    fun equalizeHistLUV(bitmap: Bitmap, @IntRange(from = 2) binsCount: Int): Bitmap

    /**
     * @throws Exception if horizontal grid or vertical <= 0
     */
    fun equalizeHistSquares(
        bitmap: Bitmap,
        gridSizeHorizontal: Int = 8,
        gridSizeVertical: Int = 8
    ): Bitmap

    /**
     * @throws Exception if horizontal grid or vertical <= 0
     */
    fun equalizeHistAdaptive(
        bitmap: Bitmap,
        gridSizeHorizontal: Int = 3,
        gridSizeVertical: Int = 3
    ): Bitmap

    /**
     * @param binsCount - Preferable 128
     * @throws Exception if horizontal grid or vertical <= 0
     */
    fun equalizeHistAdaptiveLUV(
        bitmap: Bitmap,
        gridSizeHorizontal: Int = 3,
        gridSizeVertical: Int = 3,
        @IntRange(from = 2) binsCount: Int
    ): Bitmap

    /**
     * @param binsCount - Preferable 128
     * @throws Exception if horizontal grid or vertical <= 0
     */
    fun equalizeHistAdaptiveHSV(
        bitmap: Bitmap,
        gridSizeHorizontal: Int = 3,
        gridSizeVertical: Int = 3,
        @IntRange(from = 2) binsCount: Int
    ): Bitmap

    /**
     * @param binsCount - Preferable 128
     * @throws Exception if horizontal grid or vertical <= 0
     */
    fun equalizeHistAdaptiveHSL(
        bitmap: Bitmap,
        gridSizeHorizontal: Int = 3,
        gridSizeVertical: Int = 3,
        @IntRange(from = 2) binsCount: Int
    ): Bitmap

    /**
     * @param threshold - negative or positive reasonable values ~-0...+10
     * @throws Exception if horizontal grid or vertical <= 0
     */
    fun clahe(
        bitmap: Bitmap,
        @FloatRange(from = 0.01) threshold: Float = 0.5f,
        gridSizeHorizontal: Int = 8,
        gridSizeVertical: Int = 8
    ): Bitmap

    /**
     * @param binsCount - Preferable 128
     * @param threshold - negative or positive reasonable values ~0...+10
     * @throws Exception if horizontal grid or vertical <= 0
     */
    fun claheLUV(
        bitmap: Bitmap,
        @FloatRange(from = 0.01) threshold: Float = 0.5f,
        gridSizeHorizontal: Int = 8,
        gridSizeVertical: Int = 8,
        @IntRange(from = 2) binsCount: Int
    ): Bitmap

    /**
     * @param binsCount - Preferable 128
     * @param threshold - negative or positive reasonable values ~0...+10
     * @throws Exception if horizontal grid or vertical <= 0
     */
    fun claheHSV(
        bitmap: Bitmap,
        @FloatRange(from = 0.01) threshold: Float = 0.5f,
        gridSizeHorizontal: Int = 8,
        gridSizeVertical: Int = 8,
        @IntRange(from = 2) binsCount: Int
    ): Bitmap

    /**
     * @param binsCount - Preferable 128
     * @param threshold - negative or positive reasonable values ~0...+10
     * @throws Exception if horizontal grid or vertical <= 0
     */
    fun claheHSL(
        bitmap: Bitmap,
        @FloatRange(from = 0.01) threshold: Float = 0.5f,
        gridSizeHorizontal: Int = 8,
        gridSizeVertical: Int = 8,
        @IntRange(from = 2) binsCount: Int
    ): Bitmap

    /**
     * @param binsCount - Preferable 128
     * @param threshold - negative or positive reasonable values ~-0...+10
     * @throws Exception if horizontal grid or vertical <= 0
     */
    fun claheLAB(
        bitmap: Bitmap,
        @FloatRange(from = 0.01) threshold: Float = 0.5f,
        gridSizeHorizontal: Int = 8,
        gridSizeVertical: Int = 8,
        @IntRange(from = 2) binsCount: Int
    ): Bitmap

    /**
     * Clahe for oklab
     * @param binsCount - Preferable 128
     * @param threshold - negative or positive reasonable values ~-0...+10
     * @throws Exception if horizontal grid or vertical <= 0
     */
    fun claheOklab(
        bitmap: Bitmap,
        @FloatRange(from = 0.01) threshold: Float = 0.5f,
        gridSizeHorizontal: Int = 8,
        gridSizeVertical: Int = 8,
        @IntRange(from = 2) binsCount: Int
    ): Bitmap

    /**
     * Clahe for oklab
     * @param binsCount - Preferable 128
     * @param threshold - negative or positive reasonable values ~-0...+10
     * @throws Exception if horizontal grid or vertical <= 0
     */
    fun claheOklch(
        bitmap: Bitmap,
        @FloatRange(from = 0.01) threshold: Float = 0.5f,
        gridSizeHorizontal: Int = 8,
        gridSizeVertical: Int = 8,
        @IntRange(from = 2) binsCount: Int
    ): Bitmap

    /**
     * Clahe for oklab
     * @param binsCount - Preferable 128
     * @param threshold - negative or positive reasonable values ~-0...+10
     * @throws Exception if horizontal grid or vertical <= 0
     */
    fun claheJzazbz(
        bitmap: Bitmap,
        @FloatRange(from = 0.01) threshold: Float = 0.5f,
        gridSizeHorizontal: Int = 8,
        gridSizeVertical: Int = 8,
        @IntRange(from = 2) binsCount: Int
    ): Bitmap

    /**
     * @param binsCount - Preferable 128
     * @throws Exception if horizontal grid or vertical <= 0
     */
    fun equalizeHistAdaptiveLAB(
        bitmap: Bitmap,
        gridSizeHorizontal: Int = 3,
        gridSizeVertical: Int = 3,
        @IntRange(from = 2) binsCount: Int
    ): Bitmap

    /**
     * Copies palette from one image to another using statistical method
     * lαβ best in common over LAB, OKLAB and LUV
     * @param source - source bitmap
     * @param destination - destination bitmap
     * @param colorspace - copying colorspace
     */
    fun copyPalette(
        source: Bitmap,
        destination: Bitmap,
        intensity: Float,
        colorSpace: PaletteTransferColorspace,
    ): Bitmap

}