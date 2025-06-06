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

interface ProcessingPipelines {
    fun removeShadows(bitmap: Bitmap, @IntRange(from = 3) kernelSize: Int = 5): Bitmap

    fun dehaze(bitmap: Bitmap, radius: Int = 17, omega: Float = 0.45f): Bitmap

    /**
     * 2D Convolution, only square kernel is supported, Some examples in [ConvolveKernels]
     * @param mode - Use RGB where if there is no alpha, it is faster
     **/
    fun convolve2D(
        bitmap: Bitmap,
        kernel: FloatArray,
        kernelShape: KernelShape,
        edgeMode: EdgeMode,
        scalar: Scalar,
        mode: MorphOpMode
    ): Bitmap

    fun sobel(bitmap: Bitmap, edgeMode: EdgeMode, scalar: Scalar): Bitmap

    fun laplacian(bitmap: Bitmap, edgeMode: EdgeMode, scalar: Scalar): Bitmap
}