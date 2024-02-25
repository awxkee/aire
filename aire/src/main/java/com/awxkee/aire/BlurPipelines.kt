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

interface BlurPipelines {

    fun gaussianBlur(bitmap: Bitmap, kernelSize: Int, sigma: Float): Bitmap

    fun bilateralBlur(bitmap: Bitmap, kernelSize: Int, rangeSigma: Float, spatialSigma: Float): Bitmap

    fun fastBilateralBlur(bitmap: Bitmap, rangeSigma: Float, spatialSigma: Float): Bitmap

    fun boxBlur(bitmap: Bitmap, kernelSize: Int): Bitmap

    fun poissonBlur(bitmap: Bitmap, kernelSize: Int): Bitmap

    fun stackBlur(bitmap: Bitmap, radius: Int): Bitmap

    /*
        Extended Binomial Filter of the Gaussian Blur 2 degree, extended box level, very fast compare to gaussian
     */
    fun fastGaussian2Degree(bitmap: Bitmap, radius: Int): Bitmap

    /*
        Extended Binomial Filter of the Gaussian Blur 3 degree, almost gaussian level, very fast compare to gaussian
    */
    fun fastGaussian3Degree(bitmap: Bitmap, radius: Int): Bitmap

    /*
        Extended Binomial Filter of the Gaussian Blur 4 degree, very close level to gaussian, slower than regular 1D with separated kernel
    */
    fun fastGaussian4Degree(bitmap: Bitmap, radius: Int): Bitmap

    fun medianBlur(
        bitmap: Bitmap,
        kernelSize: Int
    ): Bitmap

    fun tentBlur(bitmap: Bitmap, kernelSize: Int): Bitmap

    fun anisotropicDiffusion(
        bitmap: Bitmap,
        @IntRange(from = 1) numOfSteps: Int = 20,
        conduction: Float = 0.1f,
        diffusion: Float = 0.01f
    ): Bitmap

}