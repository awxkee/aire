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
import com.awxkee.aire.Aire
import com.awxkee.aire.BlurPipelines
import com.awxkee.aire.EdgeMode
import com.awxkee.aire.GaussianPreciseLevel
import com.awxkee.aire.KernelShape
import com.awxkee.aire.MorphOpMode
import com.awxkee.aire.Scalar
import com.awxkee.aire.TransferFunction

class BlurPipelinesImpl : BlurPipelines {

    override fun linearBoxBlur(
        bitmap: Bitmap,
        radius: Int,
        transferFunction: TransferFunction
    ): Bitmap {
        return boxBlurLinearImpl(bitmap, radius, transferFunction.value)
    }

    override fun linearTentBlur(
        bitmap: Bitmap,
        radius: Int,
        transferFunction: TransferFunction
    ): Bitmap {
        return tentBlurLinearImpl(bitmap, radius, transferFunction.value)
    }

    override fun linearGaussianBoxBlur(
        bitmap: Bitmap,
        radius: Int,
        transferFunction: TransferFunction
    ): Bitmap {
        return gaussianBoxBlurLinearImpl(bitmap, radius, transferFunction.value)
    }

    override fun gaussianBoxBlur(bitmap: Bitmap, radius: Int): Bitmap {
        return gaussianBoxBlurImpl(bitmap, radius)
    }

    override fun linearFastGaussianNext(
        bitmap: Bitmap,
        radius: Int,
        transferFunction: TransferFunction,
        edgeMode: EdgeMode,
    ): Bitmap {
        return fastGaussianNextLinearImpl(bitmap, radius, transferFunction.value, edgeMode.value)
    }

    override fun linearFastGaussian(
        bitmap: Bitmap,
        radius: Int,
        transferFunction: TransferFunction,
        edgeMode: EdgeMode
    ): Bitmap {
        return fastGaussianLinearImpl(bitmap, radius, transferFunction.value, edgeMode.value)
    }

    override fun linearGaussianBlur(
        bitmap: Bitmap,
        kernelSize: Int,
        sigma: Float,
        edgeMode: EdgeMode,
        transferFunction: TransferFunction
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
        return gaussianBlurLinearImpl(
            bitmap,
            kernelSize,
            sigma,
            edgeMode.value,
            transferFunction.value
        )
    }

    override fun linearStackBlur(
        bitmap: Bitmap,
        radius: Int,
        transferFunction: TransferFunction
    ): Bitmap {
        return stackBlurLinearImpl(bitmap, radius, transferFunction.value)
    }

    override fun gaussianBlur(
        bitmap: Bitmap,
        kernelSize: Int,
        sigma: Float,
        edgeMode: EdgeMode,
        gaussianPreciseLevel: GaussianPreciseLevel
    ): Bitmap {
        if (kernelSize < 1) {
            throw IllegalStateException("Radius must be more or equal 1")
        }
        if (sigma < 0) {
            throw IllegalStateException("Sigma must be more than or equal to 0")
        }
        if (kernelSize % 2 == 0) {
            throw IllegalStateException("Kernel size must be odd")
        }
        return gaussianBlurImpl(
            bitmap,
            kernelSize,
            sigma,
            edgeMode.value,
            gaussianPreciseLevel.value
        )
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
        kernelSize: Int,
        spatialSigma: Float,
        rangeSigma: Float,
    ): Bitmap {
        return fastBilateralBlurImpl(bitmap, kernelSize, spatialSigma, rangeSigma)
    }

    override fun boxBlur(bitmap: Bitmap, radius: Int): Bitmap {
        if (radius < 1) {
            throw IllegalStateException("Radius must be more or equal 1")
        }
        return boxBlurImpl(bitmap, radius)
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

    override fun fastGaussian2Degree(bitmap: Bitmap, radius: Int, edgeMode: EdgeMode): Bitmap {
        return fastGaussianImpl(bitmap, radius, edgeMode.value)
    }

    override fun fastGaussian3Degree(bitmap: Bitmap, radius: Int, edgeMode: EdgeMode): Bitmap {
        return fastGaussianNextImpl(bitmap, radius, edgeMode.value)
    }

    override fun tentBlur(bitmap: Bitmap, radius: Int): Bitmap {
        if (radius < 1) {
            throw IllegalStateException("Radius must be more or equal 1")
        }
        return tentBlurImpl(bitmap, radius)
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

    override fun motionBlur(
        bitmap: Bitmap,
        kernelSize: Int,
        angle: Float,
        borderMode: EdgeMode,
        borderScalar: Scalar
    ): Bitmap {
        return motionBlurImpl(bitmap, kernelSize, angle, borderMode.value, borderScalar)
    }

    override fun bokehBlur(
        bitmap: Bitmap,
        @IntRange(from = 3) kernelSize: Int,
        @IntRange(from = 3) sides: Int,
        edgeMode: EdgeMode,
        scalar: Scalar,
        mode: MorphOpMode
    ): Bitmap {
        return Aire.convolve2D(bitmap, Aire.getBokehConvolutionKernel(kernelSize, sides), KernelShape(kernelSize, kernelSize), edgeMode, scalar, mode)
    }

    private external fun motionBlurImpl(
        bitmap: Bitmap,
        kernelSize: Int,
        angle: Float,
        borderMode: Int,
        borderScalar: Scalar
    ): Bitmap

    private external fun zoomBlurImpl(
        bitmap: Bitmap,
        kernelSize: Int,
        sigma: Float,
        centerX: Float,
        centerY: Float,
        strength: Float,
        angle: Float
    ): Bitmap

    private external fun fastGaussianImpl(bitmap: Bitmap, radius: Int, edgeMode: Int): Bitmap

    private external fun fastGaussianLinearImpl(
        bitmap: Bitmap,
        radius: Int,
        transfer: Int,
        edgeMode: Int
    ): Bitmap

    private external fun fastGaussianNextImpl(bitmap: Bitmap, radius: Int, edgeMode: Int): Bitmap

    private external fun fastGaussianNextLinearImpl(
        bitmap: Bitmap,
        radius: Int,
        transfer: Int, edgeMode: Int
    ): Bitmap

    private external fun fastGaussian4DImpl(bitmap: Bitmap, radius: Int): Bitmap

    private external fun anisotropicDiffusionPipeline(
        bitmap: Bitmap,
        numOfSteps: Int,
        conduction: Float,
        diffusion: Float
    ): Bitmap

    private external fun poissonBlurPipeline(bitmap: Bitmap, radius: Int): Bitmap

    private external fun fastBilateralBlurImpl(
        bitmap: Bitmap,
        kernelSize: Int,
        radiusSigma: Float,
        spatialSigma: Float
    ): Bitmap

    private external fun gaussianBlurImpl(
        bitmap: Bitmap,
        radius: Int,
        sigma: Float,
        kernelMode: Int,
        preciseLevel: Int,
    ): Bitmap

    private external fun gaussianBlurLinearImpl(
        bitmap: Bitmap,
        radius: Int,
        sigma: Float,
        kernelMode: Int,
        transfer: Int
    ): Bitmap

    private external fun bilateralBlurPipeline(
        bitmap: Bitmap,
        radius: Int,
        sigma: Float,
        spatialSigma: Float
    ): Bitmap

    private external fun medianBlurImpl(bitmap: Bitmap, radius: Int): Bitmap

    private external fun stackBlurImpl(bitmap: Bitmap, radius: Int): Bitmap

    private external fun stackBlurLinearImpl(bitmap: Bitmap, radius: Int, transfer: Int): Bitmap

    private external fun boxBlurImpl(bitmap: Bitmap, radius: Int): Bitmap

    private external fun tentBlurImpl(bitmap: Bitmap, radius: Int): Bitmap

    private external fun gaussianBoxBlurImpl(bitmap: Bitmap, radius: Int): Bitmap

    private external fun boxBlurLinearImpl(bitmap: Bitmap, radius: Int, transfer: Int): Bitmap

    private external fun tentBlurLinearImpl(bitmap: Bitmap, radius: Int, transfer: Int): Bitmap

    private external fun gaussianBoxBlurLinearImpl(
        bitmap: Bitmap,
        radius: Int,
        transfer: Int
    ): Bitmap
}