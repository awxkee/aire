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
import com.awxkee.aire.EffectsPipelines

class EffectsPipelineImpl : EffectsPipelines {
    override fun bokeh(bitmap: Bitmap, radius: Int, angle: Float, sides: Int): Bitmap {
        return bokehImpl(bitmap, radius, angle, sides)
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

    private external fun bokehImpl(bitmap: Bitmap, radius: Int, angle: Float, sides: Int): Bitmap

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
}