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
import com.awxkee.aire.BlurPipelines
import com.awxkee.aire.EdgeMode

class BlurPipelinesImpl : BlurPipelines {

    override fun gaussianBlur(
        bitmap: Bitmap,
        kernelSize: Int,
        sigma: Float,
        edgeMode: EdgeMode
    ): Bitmap {
        if (kernelSize < 1) {
            throw IllegalStateException("Radius must be more or equal 1")
        }
        if (sigma < 0) {
            throw IllegalStateException("Sigma must be more than 0")
        }
        if (kernelSize % 2 == 0) {
            throw IllegalStateException("Kernel size must be odd")
        }
        return gaussianBlurImpl(bitmap, kernelSize, sigma, edgeMode.value)
    }

    override fun bilateralBlur(
        bitmap: Bitmap,
        kernelSize: Int,
        rangeSigma: Float,
        spatialSigma: Float
    ): Bitmap {
        if (kernelSize < 1) {
            throw IllegalStateException("Radius must be more or equal 1")
        }
        if (rangeSigma < 0 || spatialSigma < 0) {
            throw IllegalStateException("Sigma must be more than 0")
        }
        return bilateralBlurPipeline(bitmap, kernelSize, rangeSigma, spatialSigma)
    }

    override fun fastBilateralBlur(
        bitmap: Bitmap,
        rangeSigma: Float,
        spatialSigma: Float
    ): Bitmap {
        return fastBilateralPipeline(bitmap, rangeSigma, spatialSigma)
    }

    override fun boxBlur(bitmap: Bitmap, kernelSize: Int): Bitmap {
        if (kernelSize < 1) {
            throw IllegalStateException("Radius must be more or equal 1")
        }
        return boxBlurImpl(bitmap, kernelSize)
    }


    override fun poissonBlur(bitmap: Bitmap, kernelSize: Int): Bitmap {
        if (kernelSize < 1) {
            throw IllegalStateException("Radius must be more or equal 1")
        }
        return poissonBlurPipeline(bitmap, kernelSize)
    }

    override fun stackBlur(bitmap: Bitmap, radius: Int): Bitmap {
        if (radius < 1) {
            throw IllegalStateException("Radius must be more or equal 1")
        }
        return stackBlurImpl(bitmap, radius)
    }

    override fun medianBlur(bitmap: Bitmap, kernelSize: Int): Bitmap {
        if (kernelSize < 1) {
            throw IllegalStateException("Radius must be more or equal 1")
        }
        return medianBlurImpl(bitmap, kernelSize)
    }

    override fun fastGaussian2Degree(bitmap: Bitmap, radius: Int): Bitmap {
        return fastGaussianImpl(bitmap, radius)
    }

    override fun fastGaussian3Degree(bitmap: Bitmap, radius: Int): Bitmap {
        return fastGaussianNextImpl(bitmap, radius)
    }

    override fun tentBlur(bitmap: Bitmap, kernelSize: Int): Bitmap {
        if (kernelSize < 3) {
            throw IllegalStateException("Radius must be more or equal 1")
        }
        return tentBlurImpl(bitmap, kernelSize)
    }

    override fun fastGaussian4Degree(bitmap: Bitmap, radius: Int): Bitmap {
        return fastGaussian4DImpl(bitmap, radius)
    }

    override fun anisotropicDiffusion(
        bitmap: Bitmap,
        numOfSteps: Int,
        conduction: Float,
        diffusion: Float
    ): Bitmap {
        return anisotropicDiffusionPipeline(bitmap, numOfSteps, conduction, diffusion)
    }

    override fun zoomBlur(
        bitmap: Bitmap,
        kernelSize: Int,
        sigma: Float,
        centerX: Float,
        centerY: Float,
        strength: Float,
        angle: Float
    ): Bitmap {
        return zoomBlurImpl(bitmap, kernelSize, sigma, centerX, centerY, strength, angle)
    }

    override fun bokehBlur(
        bitmap: Bitmap,
        @IntRange(from = 3.toLong()) kernelSize: Int,
        @IntRange(from = 3.toLong()) sides: Int
    ): Bitmap {
        return bokehBlurImpl(bitmap, kernelSize, sides)
    }

    private external fun bokehBlurImpl(bitmap: Bitmap, kernelSize: Int, sides: Int): Bitmap

    private external fun zoomBlurImpl(
        bitmap: Bitmap,
        kernelSize: Int,
        sigma: Float,
        centerX: Float,
        centerY: Float,
        strength: Float,
        angle: Float
    ): Bitmap

    private external fun fastGaussianImpl(bitmap: Bitmap, radius: Int): Bitmap

    private external fun fastGaussianNextImpl(bitmap: Bitmap, radius: Int): Bitmap

    private external fun fastGaussian4DImpl(bitmap: Bitmap, radius: Int): Bitmap

    private external fun anisotropicDiffusionPipeline(
        bitmap: Bitmap,
        numOfSteps: Int,
        conduction: Float,
        diffusion: Float
    ): Bitmap

    private external fun poissonBlurPipeline(bitmap: Bitmap, radius: Int): Bitmap

    private external fun fastBilateralPipeline(
        bitmap: Bitmap,
        radiusSigma: Float,
        spatialSigma: Float
    ): Bitmap

    private external fun gaussianBlurImpl(bitmap: Bitmap, radius: Int, sigma: Float, kernelMode: Int): Bitmap

    private external fun bilateralBlurPipeline(
        bitmap: Bitmap,
        radius: Int,
        sigma: Float,
        spatialSigma: Float
    ): Bitmap

    private external fun medianBlurImpl(bitmap: Bitmap, radius: Int): Bitmap

    private external fun stackBlurImpl(bitmap: Bitmap, radius: Int): Bitmap

    private external fun boxBlurImpl(bitmap: Bitmap, radius: Int): Bitmap

    private external fun tentBlurImpl(bitmap: Bitmap, radius: Int): Bitmap
}