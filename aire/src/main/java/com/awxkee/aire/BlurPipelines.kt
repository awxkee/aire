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

    fun bokehBlur(
        bitmap: Bitmap,
        @IntRange(from = 3) kernelSize: Int,
        @IntRange(from = 3) sides: Int = 6,
        edgeMode: EdgeMode,
        scalar: Scalar,
        mode: MorphOpMode,
    ): Bitmap

    /**
     * Default [gaussian blur](https://en.wikipedia.org/wiki/Gaussian_filter) in perceptual colorspace.
     * O(R) complexity, very slow.
     *
     * @param kernelSize - size of blurring kernel, must be odd, kernel may be almost any reasonable size
     * @param sigma - controlling kernel flattening level, default 0 ( will compute preferred automatically ), higher sigma creates more flat kernel
     * @param edgeMode - edge controlling mode
     */
    fun gaussianBlur(
        bitmap: Bitmap,
        horizontalKernelSize: Int,
        verticalKernelSize: Int,
        horizontalSigma: Float = 0f,
        verticalSigma: Float = 0f,
        edgeMode: EdgeMode,
        gaussianPreciseLevel: GaussianPreciseLevel
    ): Bitmap

    /**
     * Default gaussian blur, [see more](https://en.wikipedia.org/wiki/Gaussian_filter).
     * O(R) complexity, very slow.
     *
     * @param kernelSize - size of blurring kernel, must be odd, kernel may be almost any reasonable size
     * @param sigma - controlling kernel flattening level, default (kernelSize / 6), higher sigma creates more flat kernel
     * @param edgeMode - edge controlling mode
     */
    fun linearGaussianBlur(
        bitmap: Bitmap,
        horizontalKernelSize: Int,
        verticalKernelSize: Int,
        horizontalSigma: Float,
        verticalSigma: Float,
        edgeMode: EdgeMode,
        transferFunction: TransferFunction,
    ): Bitmap

    fun bilateralBlur(
        bitmap: Bitmap,
        kernelSize: Int,
        rangeSigma: Float,
        spatialSigma: Float
    ): Bitmap

    fun fastBilateralBlur(
        bitmap: Bitmap,
        kernelSize: Int,
        spatialSigma: Float,
        rangeSigma: Float
    ): Bitmap

    /***
     * Performs motion blur on the image
     *
     * @param kernelSize - Kernel size, must be odd!
     * @param angle - Angle in *degrees*
     * @param borderMode - See [EdgeMode]
     * @param borderScalar - See [Scalar]
     */
    fun motionBlur(
        bitmap: Bitmap,
        kernelSize: Int,
        angle: Float,
        borderMode: EdgeMode,
        borderScalar: Scalar
    ): Bitmap

    /**
     * Box blur averages all pixels to make a blur in perceptual colorspace.
     * Produces box filter with very noticeable ringing.
     * Convergence of this function is high so strong box effect appears very fast.
     * Made in *perceptual* colorspace.
     * O(1) complexity, medium speed.
     *
     * @param kernelSize - radius of blurring kernel, almost any size is supported
     */
    fun boxBlur(bitmap: Bitmap, kernelSize: Int): Bitmap

    /**
     * @param angle - default is PI / 2
     */
    fun zoomBlur(
        bitmap: Bitmap,
        kernelSize: Int,
        sigma: Float,
        centerX: Float = 0.5f,
        centerY: Float = 0.5f,
        strength: Float,
        angle: Float
    ): Bitmap

    fun poissonBlur(bitmap: Bitmap, kernelSize: Int): Bitmap

    /**
     * The fastest gaussian blur approximation.
     * Made in *perceptual* colorspace.
     * O(1) complexity, the fastest
     *
     * @param radius - blurring radius, radius ~[2, 254]
     */
    fun stackBlur(bitmap: Bitmap, horizontalRadius: Int, verticalRadius: Int): Bitmap

    /**
     * The fastest gaussian blur approximation in linear colorspace, this is much slower
     * than perceptual approximation since converting into linear required significant computing,
     * using f32 as storage and accumulator, however mathematically it is more correct.
     * O(1) complexity, the fastest.
     *
     * @param radius - blurring radius, blurring radius almost unlimited since transformation is in f32
     * @param transferFunction - transfer function in linear and its inverse
     */
    fun linearStackBlur(
        bitmap: Bitmap,
        horizontalRadius: Int,
        verticalRadius: Int,
        transferFunction: TransferFunction
    ): Bitmap

    /**
     *  Extended Binomial Filter of the Gaussian Blur 2 degree, extended box level, very fast compare to gaussian.
     *  The fastest gaussian blur approximation in linear colorspace, this is much slower
     *  than perceptual approximation since converting into linear required significant computing,
     *  using f32 as storage and accumulator, however mathematically it is more correct.
     *  Result close to stack blur, slightly better and slightly slower
     *  O(log(R)) complexity, fast.
     *
     * @param radius - blurring radius, blurring radius almost unlimited since transformation is in f32
     * @param transferFunction - transfer function in linear and its inverse
     * @param edgeMode - Edge handling mode, *Kernel clip* is not supported here!
     */
    fun linearFastGaussian(
        bitmap: Bitmap,
        horizontalRadius: Int,
        verticalRadius: Int,
        transferFunction: TransferFunction,
        edgeMode: EdgeMode
    ): Bitmap

    /**
     *  Extended Binomial Filter of the Gaussian Blur 3 degree, extended box level, very fast compare to gaussian.
     *  The fastest gaussian blur approximation in linear colorspace, this is much slower
     *  than perceptual approximation since converting into linear required significant computing,
     *  using f32 as storage and accumulator, however mathematically it is more correct.
     *  Results much better than 2 level and stack blur, slower
     *  O(log(R)) complexity, fast.
     *
     * @param radius - blurring radius, blurring radius almost unlimited since transformation is in f32
     * @param transferFunction - transfer function in linear and its inverse
     * @param edgeMode - Edge handling mode, *Kernel clip* is not supported here!
     */
    fun linearFastGaussianNext(
        bitmap: Bitmap,
        horizontalRadius: Int,
        verticalRadius: Int,
        transferFunction: TransferFunction,
        edgeMode: EdgeMode,
    ): Bitmap

    /**
     *  Extended Binomial Filter of the Gaussian Blur 2 degree, extended box level, very fast compare to gaussian.
     *  Results close to stack blur.
     *  Made in *perceptual* colorspace.
     *  O(log(R)) complexity, fast.
     *
     * @param radius - blurring radius, radius ~[1, 319]
     * @param edgeMode - Edge handling mode, *Kernel clip* is not supported here!
     */
    fun fastGaussian2Degree(
        bitmap: Bitmap,
        horizontalRadius: Int,
        verticalRadius: Int,
        edgeMode: EdgeMode
    ): Bitmap

    /**
     *  Extended Binomial Filter of the Gaussian Blur 3 degree, very fast compare to gaussian.
     *  Results much better than 2 level and stack blur.
     *  Made in *perceptual* colorspace.
     *  O(log(R)) complexity, fast.
     *
     * @param radius - blurring radius, radius ~[1, 280]
     * @param edgeMode - Edge handling mode, *Kernel clip* is not supported here!
     */
    fun fastGaussian3Degree(
        bitmap: Bitmap, horizontalRadius: Int,
        verticalRadius: Int, edgeMode: EdgeMode
    ): Bitmap

    /**
     *  Extended Binomial Filter of the Gaussian Blur 4 degree, very close level to gaussian,
     *  very fast compare to gaussian.
     *  Made in *perceptual* colorspace.
     *  @param radius - blurring radius
     **/
    fun fastGaussian4Degree(bitmap: Bitmap, radius: Int): Bitmap

    fun medianBlur(
        bitmap: Bitmap,
        kernelSize: Int
    ): Bitmap

    /**
     * Box blur averages all pixels to make a blur.
     * Produces box filter with very noticeable ringing.
     * Convergence of this function is high so strong box effect appears very fast.
     * Made in *linear* colorspace.
     * O(1) complexity, slow.
     *
     * @param kernelSize - radius of blurring kernel, almost any size is supported
     * @param transferFunction - transfer function in linear and its inverse
     */
    fun linearBoxBlur(bitmap: Bitmap, kernelSize: Int, transferFunction: TransferFunction): Bitmap

    /**
     * Tent blur just make 2 passes of box blur as per [Central limit theorem](https://en.wikipedia.org/wiki/Central_limit_theorem).
     * Produces tent filter with noticeable ringing.
     * Convergence of this function is high so strong box effect appears very fast.
     * Made in *linear* colorspace.
     * O(1) complexity, slow.
     *
     * @param sigma - flattening level
     * @param transferFunction - transfer function in linear and its inverse
     */
    fun linearTentBlur(bitmap: Bitmap, sigma: Float, transferFunction: TransferFunction): Bitmap

    /**
     * Gaussian blur just make 3 passes of box blur as per [Central limit theorem](https://en.wikipedia.org/wiki/Central_limit_theorem).
     * Produces ideal gaussian as stated in [Central limit theorem](https://en.wikipedia.org/wiki/Central_limit_theorem)
     * Convergence of this function is high so strong box effect appears very fast.
     * Made in *linear* colorspace.
     * O(1) complexity, slow.
     *
     * @param sigma - flattening level
     * @param transferFunction - transfer function in linear and its inverse
     */
    fun linearGaussianBoxBlur(
        bitmap: Bitmap,
        sigma: Float,
        transferFunction: TransferFunction
    ): Bitmap

    /**
     * Tent blur just make 2 passes of box blur as per [Central limit theorem](https://en.wikipedia.org/wiki/Central_limit_theorem).
     * Produces tent filter with noticeable ringing.
     * Convergence of this function is high so strong tent effect appears very fast
     * Made in *perceptual* colorspace.
     * O(1) complexity, medium speed.
     *
     * @param sigma - flattening level
     */
    fun tentBlur(bitmap: Bitmap, sigma: Float): Bitmap

    /**
     * Gaussian blur just make 3 passes of box blur as per [Central limit theorem](https://en.wikipedia.org/wiki/Central_limit_theorem).
     * Produces ideal gaussian as stated in [Central limit theorem](https://en.wikipedia.org/wiki/Central_limit_theorem)
     * Convergence of this function is high so strong gaussian effect appears very fast
     * Made in *perceptual* colorspace.
     * O(1) complexity, medium speed.
     *
     * @param sigma - flattening level
     */
    fun gaussianBoxBlur(bitmap: Bitmap, sigma: Float): Bitmap

    fun anisotropicDiffusion(
        bitmap: Bitmap,
        @IntRange(from = 1) numOfSteps: Int = 20,
        conduction: Float = 0.1f,
        diffusion: Float = 0.01f
    ): Bitmap

}