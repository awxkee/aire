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

package com.awxkee.aire.pipeline

import android.graphics.Bitmap
import androidx.annotation.IntRange
import com.awxkee.aire.EdgeMode
import com.awxkee.aire.KernelShape
import com.awxkee.aire.MorphOpMode
import com.awxkee.aire.ProcessingPipelines
import com.awxkee.aire.Scalar

class ProcessingPipelinesImpl : ProcessingPipelines {
    override fun removeShadows(
        bitmap: Bitmap,
        @IntRange(from = 3.toLong(), to = 9.toLong()) kernelSize: Int
    ): Bitmap {
        return removeShadowsPipelines(bitmap, kernelSize)
    }

    override fun dehaze(bitmap: Bitmap, radius: Int, omega: Float): Bitmap {
        return dehazeImpl(bitmap, radius, omega)
    }

    override fun convolve2D(
        bitmap: Bitmap,
        kernel: FloatArray,
        kernelShape: KernelShape,
        edgeMode: EdgeMode,
        scalar: Scalar,
        mode: MorphOpMode
    ): Bitmap {
        return convolve2DImpl(
            bitmap,
            kernel,
            kernelShape.width,
            kernelShape.height,
            edgeMode.value,
            scalar,
            mode.value
        )
    }

    override fun sobel(
        bitmap: Bitmap,
        edgeMode: EdgeMode,
        scalar: Scalar,
    ): Bitmap {
        return sobelImpl(bitmap, edgeMode.value, scalar)
    }

    override fun laplacian(bitmap: Bitmap, edgeMode: EdgeMode, scalar: Scalar): Bitmap {
        return laplacianImpl(bitmap, edgeMode.value, scalar)
    }

    private external fun convolve2DImpl(
        bitmap: Bitmap,
        kernel: FloatArray,
        kernelWidth: Int,
        kernelHeight: Int,
        edgeMode: Int,
        scalar: Scalar,
        mode: Int,
    ): Bitmap

    private external fun removeShadowsPipelines(bitmap: Bitmap, kernelSize: Int): Bitmap

    private external fun dehazeImpl(bitmap: Bitmap, radius: Int, omega: Float): Bitmap

    private external fun sobelImpl(bitmap: Bitmap, edgeMode: Int, scalar: Scalar): Bitmap

    private external fun laplacianImpl(bitmap: Bitmap, edgeMode: Int, scalar: Scalar): Bitmap
}