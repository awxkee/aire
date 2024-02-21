/*
 *
 *  * MIT License
 *  *
 *  * Copyright (c) 2024 Radzivon Bartoshyk
 *  * jxl-coder [https://github.com/awxkee/jxl-coder]
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
import com.awxkee.aire.TonePipelines

class TonePipelinesImpl : TonePipelines {
    override fun logarithmicToneMapping(bitmap: Bitmap, exposure: Float): Bitmap {
        return logarithmicImpl(bitmap, exposure)
    }

    override fun acesFilmicToneMapping(bitmap: Bitmap, exposure: Float): Bitmap {
        return acesFilmicImpl(bitmap, exposure)
    }

    override fun exposure(bitmap: Bitmap, exposure: Float): Bitmap {
        return exposureImpl(bitmap, exposure)
    }

    override fun hejlBurgessToneMapping(bitmap: Bitmap, exposure: Float): Bitmap {
        return hejlBurgessToneMappingImpl(bitmap, exposure)
    }

    override fun hableFilmicToneMapping(bitmap: Bitmap, exposure: Float): Bitmap {
        return hableFilmicImpl(bitmap, exposure)
    }

    override fun acesHillToneMapping(bitmap: Bitmap, exposure: Float): Bitmap {
        return acesHillImpl(bitmap, exposure)
    }

    override fun monochrome(bitmap: Bitmap, color: FloatArray, exposure: Float): Bitmap {
        return monochromeImpl(bitmap, color, exposure)
    }

    override fun whiteBalance(bitmap: Bitmap, temperature: Float, tint: Float): Bitmap {
        return whiteBalanceImpl(bitmap, temperature, tint)
    }

    override fun mobius(bitmap: Bitmap, exposure: Float, transition: Float, peak: Float): Bitmap {
        return mobiusImpl(bitmap, exposure, transition, peak)
    }

    override fun uchimura(bitmap: Bitmap, exposure: Float): Bitmap {
        return uchimuraImpl(bitmap, exposure)
    }

    override fun aldridge(bitmap: Bitmap, exposure: Float, cutoff: Float): Bitmap {
        return aldridgeImpl(bitmap, exposure, cutoff)
    }

    override fun drago(bitmap: Bitmap, exposure: Float, sdrWhitePoint: Float): Bitmap {
        return dragoImpl(bitmap, exposure, sdrWhitePoint)
    }

    private external fun dragoImpl(bitmap: Bitmap, exposure: Float, sdrWhitePoint: Float): Bitmap

    private external fun aldridgeImpl(bitmap: Bitmap, exposure: Float, cutoff: Float): Bitmap

    private external fun uchimuraImpl(bitmap: Bitmap, exposure: Float): Bitmap

    private external fun mobiusImpl(bitmap: Bitmap, exposure: Float, transition: Float, peak: Float): Bitmap

    private external fun whiteBalanceImpl(bitmap: Bitmap, temperature: Float, tint: Float): Bitmap

    private external fun monochromeImpl(bitmap: Bitmap, color: FloatArray, exposure: Float): Bitmap

    private external fun acesHillImpl(bitmap: Bitmap, exposure: Float): Bitmap

    private external fun hableFilmicImpl(bitmap: Bitmap, exposure: Float): Bitmap

    private external fun exposureImpl(bitmap: Bitmap, exposure: Float): Bitmap

    private external fun logarithmicImpl(bitmap: Bitmap, exposure: Float): Bitmap

    private external fun acesFilmicImpl(bitmap: Bitmap, exposure: Float): Bitmap

    private external fun hejlBurgessToneMappingImpl(bitmap: Bitmap, exposure: Float = 1.0f): Bitmap
}