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

interface TonePipelines {
    fun logarithmicToneMapping(bitmap: Bitmap, exposure: Float = 1.0f): Bitmap

    fun acesFilmicToneMapping(bitmap: Bitmap, exposure: Float = 1.0f): Bitmap

    fun exposure(bitmap: Bitmap, exposure: Float): Bitmap

    fun hejlBurgessToneMapping(bitmap: Bitmap, exposure: Float = 1.0f): Bitmap

    fun hableFilmicToneMapping(bitmap: Bitmap, exposure: Float = 1.0f): Bitmap

    fun acesHillToneMapping(bitmap: Bitmap, exposure: Float = 1.0f): Bitmap

    fun monochrome(bitmap: Bitmap, color: FloatArray, exposure: Float = 1.0f): Bitmap

    fun whiteBalance(bitmap: Bitmap, temperature: Float = 1f, tint: Float = 0.0f): Bitmap

    fun mobius(
        bitmap: Bitmap,
        exposure: Float = 1.0f,
        transition: Float = 0.9f,
        peak: Float = 1.0f
    ): Bitmap

    fun uchimura(bitmap: Bitmap, exposure: Float = 1.0f): Bitmap

    fun aldridge(bitmap: Bitmap, exposure: Float = 1.0f, cutoff: Float = 0.025f): Bitmap

    fun drago(bitmap: Bitmap, exposure: Float = 1.0f, sdrWhitePoint: Float = 250.0f): Bitmap
}