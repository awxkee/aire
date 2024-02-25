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

interface ShiftPipelines {

    fun horizontalWindStagger(bitmap: Bitmap, windStrength: Float = 0.2f, streamsCount: Int = 90, clearColor: Int = Color.BLACK.toInt()): Bitmap

    fun tiltShift(
        bitmap: Bitmap,
        radius: Int,
        sigma: Float,
        anchorX: Float = 0.5f,
        anchorY: Float = 0.5f,
        tiltRadius: Float = 0.2f,
    ): Bitmap

    fun horizontalTiltShift(
        bitmap: Bitmap,
        radius: Int,
        sigma: Float,
        anchorX: Float,
        anchorY: Float = 0.5f,
        tiltRadius: Float = 0.2f,
        angle: Float = Math.PI.toFloat() / 2,
    ): Bitmap

    fun glitch(
        bitmap: Bitmap,
        channelsShiftX: Float = -0.075f,
        channelsShiftY: Float = -0.075f,
        corruptionSize: Float = 0.009f,
        corruptionCount: Int = 60,
        corruptionShiftX: Float = -0.05f,
        corruptionShiftY: Float = 0.0f,
    ): Bitmap
}