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
import com.awxkee.aire.EffectsPipelines

class EffectsPipelineImpl : EffectsPipelines {

    override fun convex(bitmap: Bitmap, strength: Float): Bitmap {
        return convexImpl(bitmap, strength)
    }

    override fun bokeh(
        bitmap: Bitmap,
        @IntRange(from = 3.toLong()) kernelSize: Int,
        @IntRange(from = 3.toLong()) sides: Int,
        enhance: Boolean
    ): Bitmap {
        return bokehImpl(bitmap, kernelSize, sides, enhance)
    }

    override fun marble(
        bitmap: Bitmap,
        intensity: Float,
        turbulence: Float,
        amplitude: Float
    ): Bitmap {
        return marbleImpl(bitmap, intensity, turbulence, amplitude)
    }

    override fun perlinDistortion(
        bitmap: Bitmap,
        intensity: Float,
        turbulence: Float,
        amplitude: Float
    ): Bitmap {
        return perlinDistortionImpl(bitmap, intensity, turbulence, amplitude)
    }

    override fun waterEffect(
        bitmap: Bitmap,
        fractionSize: Float,
        frequencyX: Float,
        amplitudeX: Float,
        frequencyY: Float,
        amplitudeY: Float
    ): Bitmap {
        return waterEffectImpl(bitmap, fractionSize, frequencyX, amplitudeX, frequencyY, amplitudeY)
    }

    override fun fractalGlass(bitmap: Bitmap, glassSize: Float, amplitude: Float): Bitmap {
        return fractalGlassImpl(bitmap, glassSize, amplitude)
    }

    override fun oil(bitmap: Bitmap, radius: Int, levels: Float): Bitmap {
        return oilImpl(bitmap, radius, levels)
    }

    override fun crystallize(bitmap: Bitmap, numClusters: Int, strokeColor: Int): Bitmap {
        return crystallizeImpl(bitmap, numClusters, strokeColor)
    }

    override fun equalizeHist(bitmap: Bitmap): Bitmap {
        return equalizeHistImpl(bitmap)
    }

    override fun equalizeHistHSV(bitmap: Bitmap): Bitmap {
        return equalizeHistHSVImpl(bitmap)
    }

    override fun equalizeHistAdaptive(
        bitmap: Bitmap,
        gridSizeHorizontal: Int,
        gridSizeVertical: Int
    ): Bitmap {
        return equalizeHistAdaptiveImpl(bitmap, gridSizeHorizontal, gridSizeVertical)
    }

    override fun equalizeHistSquares(
        bitmap: Bitmap,
        gridSizeHorizontal: Int,
        gridSizeVertical: Int
    ): Bitmap {
        return equalizeHistSquaresImpl(bitmap, gridSizeHorizontal, gridSizeVertical)
    }

    override fun clahe(
        bitmap: Bitmap,
        threshold: Float,
        gridSizeHorizontal: Int,
        gridSizeVertical: Int
    ): Bitmap {
        return claheImpl(bitmap, threshold, gridSizeHorizontal, gridSizeVertical)
    }

    override fun claheLUV(
        bitmap: Bitmap,
        threshold: Float,
        gridSizeHorizontal: Int,
        gridSizeVertical: Int
    ): Bitmap {
        return claheLUVImpl(bitmap, threshold, gridSizeHorizontal, gridSizeVertical)
    }

    private external fun convexImpl(bitmap: Bitmap, strength: Float): Bitmap

    private external fun bokehImpl(
        bitmap: Bitmap,
        kernelSize: Int,
        sides: Int,
        enhance: Boolean
    ): Bitmap

    private external fun perlinDistortionImpl(
        bitmap: Bitmap,
        intensity: Float,
        turbulence: Float,
        amplitude: Float
    ): Bitmap

    private external fun fractalGlassImpl(
        bitmap: Bitmap,
        glassSize: Float,
        amplitude: Float
    ): Bitmap

    private external fun waterEffectImpl(
        bitmap: Bitmap,
        fractionSize: Float,
        frequencyX: Float,
        amplitudeX: Float,
        frequencyY: Float,
        amplitudeY: Float
    ): Bitmap

    private external fun marbleImpl(
        bitmap: Bitmap,
        intensity: Float = 0.5f,
        turbulence: Float = 1f,
        amplitude: Float = 1f
    ): Bitmap

    private external fun oilImpl(bitmap: Bitmap, radius: Int, levels: Float): Bitmap

    private external fun crystallizeImpl(
        bitmap: Bitmap,
        clustersCount: Int,
        strokeColor: Int
    ): Bitmap

    private external fun equalizeHistImpl(bitmap: Bitmap): Bitmap

    private external fun equalizeHistHSVImpl(bitmap: Bitmap): Bitmap

    private external fun equalizeHistAdaptiveImpl(
        bitmap: Bitmap,
        gridSizeHorizontal: Int,
        gridSizeVertical: Int
    ): Bitmap

    private external fun equalizeHistSquaresImpl(
        bitmap: Bitmap,
        gridSizeHorizontal: Int,
        gridSizeVertical: Int
    ): Bitmap

    private external fun claheImpl(
        bitmap: Bitmap,
        threshold: Float,
        gridSizeHorizontal: Int,
        gridSizeVertical: Int
    ): Bitmap

    private external fun claheLUVImpl(
        bitmap: Bitmap,
        threshold: Float,
        gridSizeHorizontal: Int,
        gridSizeVertical: Int
    ): Bitmap
}