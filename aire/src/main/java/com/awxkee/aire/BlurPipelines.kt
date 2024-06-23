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
    ): Bitmap

    /**
     * Default [gaussian blur](https://en.wikipedia.org/wiki/Gaussian_filter) in perceptual colorspace.
     * O(R) complexity, very slow.
     *
     * @param kernelSize - size of blurring kernel, must be odd, kernel may be almost any reasonable size
     * @param sigma - controlling kernel flattening level, default (kernelSize / 6), higher sigma creates more flat kernel
     * @param edgeMode - edge controlling mode
     */
    fun gaussianBlur(bitmap: Bitmap, kernelSize: Int, sigma: Float, edgeMode: EdgeMode): Bitmap

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
        kernelSize: Int,
        sigma: Float,
        edgeMode: EdgeMode,
        transferFunction: TransferFunction,
    ): Bitmap

    fun bilateralBlur(
        bitmap: Bitmap,
        kernelSize: Int,
        rangeSigma: Float,
        spatialSigma: Float
    ): Bitmap

    fun fastBilateralBlur(bitmap: Bitmap, rangeSigma: Float, spatialSigma: Float): Bitmap

    /**
     * Box blur averages all pixels to make a blur in perceptual colorspace.
     * Produces box filter with very noticeable ringing.
     * Convergence of this function is high so strong box effect appears very fast.
     * Made in *perceptual* colorspace.
     * O(1) complexity, medium speed.
     *
     * @param radius - radius of blurring kernel, almost any size is supported
     */
    fun boxBlur(bitmap: Bitmap, radius: Int): Bitmap

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
    fun stackBlur(bitmap: Bitmap, radius: Int): Bitmap

    /**
     * The fastest gaussian blur approximation in linear colorspace, this is much slower
     * than perceptual approximation since converting into linear required significant computing,
     * using f32 as storage and accumulator, however mathematically it is more correct.
     * O(1) complexity, the fastest.
     *
     * @param radius - blurring radius, blurring radius almost unlimited since transformation is in f32
     * @param transferFunction - transfer function in linear and its inverse
     */
    fun linearStackBlur(bitmap: Bitmap, radius: Int, transferFunction: TransferFunction): Bitmap

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
     */
    fun linearFastGaussian(bitmap: Bitmap, radius: Int, transferFunction: TransferFunction): Bitmap

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
     */
    fun linearFastGaussianNext(
        bitmap: Bitmap,
        radius: Int,
        transferFunction: TransferFunction
    ): Bitmap

    /**
     *  Extended Binomial Filter of the Gaussian Blur 2 degree, extended box level, very fast compare to gaussian.
     *  Results close to stack blur.
     *  Made in *perceptual* colorspace.
     *  O(log(R)) complexity, fast.
     *
     * @param radius - blurring radius, radius ~[1, 319]
     */
    fun fastGaussian2Degree(bitmap: Bitmap, radius: Int): Bitmap

    /**
     *  Extended Binomial Filter of the Gaussian Blur 3 degree, very fast compare to gaussian.
     *  Results much better than 2 level and stack blur.
     *  Made in *perceptual* colorspace.
     *  O(log(R)) complexity, fast.
     *
     * @param radius - blurring radius, radius ~[1, 280]
     */
    fun fastGaussian3Degree(bitmap: Bitmap, radius: Int): Bitmap

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
     * Tent blur just make 2 passes of box blur as per [Central limit theorem](https://en.wikipedia.org/wiki/Central_limit_theorem).
     * Produces tent filter with noticeable ringing.
     * Convergence of this function is high so strong tent effect appears very fast
     * Made in *perceptual* colorspace.
     * O(1) complexity, medium speed.
     *
     * @param radius - radius of blurring kernel, almost any size is supported
     */
    fun tentBlur(bitmap: Bitmap, radius: Int): Bitmap

    /**
     * Gaussian blur just make 3 passes of box blur as per [Central limit theorem](https://en.wikipedia.org/wiki/Central_limit_theorem).
     * Produces ideal gaussian as stated in [Central limit theorem](https://en.wikipedia.org/wiki/Central_limit_theorem)
     * Convergence of this function is high so strong gaussian effect appears very fast
     * Made in *perceptual* colorspace.
     * O(1) complexity, medium speed.
     *
     * @param radius - radius of blurring kernel, almost any size is supported
     */
    fun gaussianBoxBlur(bitmap: Bitmap, radius: Int): Bitmap

    fun anisotropicDiffusion(
        bitmap: Bitmap,
        @IntRange(from = 1) numOfSteps: Int = 20,
        conduction: Float = 0.1f,
        diffusion: Float = 0.01f
    ): Bitmap

}