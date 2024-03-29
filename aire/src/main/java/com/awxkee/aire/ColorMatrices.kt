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

import androidx.annotation.Keep
import kotlin.math.cos
import kotlin.math.sin

@Keep
object ColorMatrices {

    val SEPIA = floatArrayOf(0.393f, 0.769f, 0.189f, 0.349f, 0.686f, 0.168f, 0.272f, 0.534f, 0.131f)

    val POLAROID =
        floatArrayOf(1.438f, -0.062f, 0.062f, 0.122f, 1.378f, -0.122f, -0.016f, -0.016f, 1.483f)

    val COOL = floatArrayOf(0.99f, 0.0f, 0.0f, 0f, 0.93f, 0f, 0f, 0f, 1.08f)

    val WARM = floatArrayOf(1.06f, 0f, 0f, 0f, 1.01f, 0f, 0f, 0f, 0.93f)

    val NIGHT_VISION = floatArrayOf(0.1f, 0.4f, 0f, 0.3f, 1f, 0.3f, 0f, 0.4f, 0.1f)

    val CODA_CHROME = floatArrayOf(
        1.1285583f, -0.39673823f, -0.03992559f,
        -0.1640434f, 1.0835252f, -0.054988053f,
        -0.1678601f, -0.56034166f, 1.6014851f
    )

    val BROWNI = floatArrayOf(
        0.59970236f, 0.34553242f, -0.27082986f,
        -0.03770325f, 0.86095774f, 0.15059553f,
        0.24113636f, -0.07441038f, 0.4497218f
    )

    val VINTAGE = floatArrayOf(
        0.6279346f, 0.32021835f, -0.039654084f,
        0.025783977f, 0.64411885f, 0.032591276f,
        0.046605557f, -0.0851233f, 0.5241648f
    )

    val PASTEL = floatArrayOf(
        0.9f, 0.3f, 0.1f,
        0.2f, 0.7f, 0.2f,
        0.1f, 0.2f, 0.8f
    )

    val ORANGE_HAZE = floatArrayOf(
        0.6f, 0.3f, 0.1f,
        0.1f, 0.4f, 0.5f,
        0.1f, 0.1f, 0.7f
    )

    val PINK_DREAM = floatArrayOf(
        1.1f, 0.1f, 0.1f,
        0.1f, 0.8f, 0.1f,
        0.2f, 0.2f, 1.1f
    )

    val GOLDEN_HOUR = floatArrayOf(
        1.2f, 0.5f, 0.1f,
        0.1f, 0.8f, 0.2f,
        0.1f, 0.1f, 1.0f
    )

    val HOT_SUMMER = floatArrayOf(
        1.2f, 0.2f, 0.1f,
        0.1f, 1.1f, 0.1f,
        0.1f, 0.2f, 1.0f
    )

    val PURPLE_MIST = floatArrayOf(
        0.7f, 0.3f, 0.8f,
        0.3f, 0.6f, 0.2f,
        0.1f, 0.1f, 1.0f
    )

    val SUNRISE = floatArrayOf(
        1.0f, 0.2f, 0.1f,
        0.1f, 0.8f, 0.2f,
        0.1f, 0.2f, 1.1f
    )

    val COLORFUL_SWIRL = floatArrayOf(
        0.6f, 0.4f, 0.1f,
        0.1f, 0.7f, 0.4f,
        0.2f, 0.1f, 0.8f
    )

    val SOFT_SPRING_LIGHT = floatArrayOf(
        1.1f, 0.1f, 0.1f,
        0.1f, 1.1f, 0.1f,
        0.1f, 0.1f, 1.1f
    )

    val AUTUMN_TONES = floatArrayOf(
        0.9f, 0.3f, 0.1f,
        0.2f, 0.8f, 0.2f,
        0.1f, 0.1f, 0.8f
    )

    val LAVENDER_DREAM = floatArrayOf(
        0.8f, 0.2f, 0.2f,
        0.1f, 0.7f, 0.3f,
        0.1f, 0.2f, 0.9f
    )

    val CYBERPUNK = floatArrayOf(
        0.8f, 0.2f, 0.5f,
        0.1f, 0.9f, 0.3f,
        0.1f, 0.3f, 0.9f
    )

    val LEMONADE_LIGHT = floatArrayOf(
        1.0f, 0.3f, 0.1f,
        0.1f, 1.0f, 0.1f,
        0.2f, 0.2f, 1.0f
    )

    val SPECTRAL_FIRE = floatArrayOf(
        0.9f, 0.2f, 0.1f,
        0.1f, 0.8f, 0.2f,
        0.1f, 0.2f, 0.9f
    )

    val NIGHT_MAGIC = floatArrayOf(
        0.7f, 0.1f, 0.2f,
        0.2f, 0.7f, 0.1f,
        0.1f, 0.2f, 0.8f
    )

    val FANTASY_LANDSCAPE = floatArrayOf(
        0.8f, 0.3f, 0.2f,
        0.1f, 0.8f, 0.2f,
        0.1f, 0.2f, 0.9f
    )

    val COLOR_EXPLOSION = floatArrayOf(
        1.0f, 0.5f, 0.2f,
        0.2f, 0.8f, 0.3f,
        0.1f, 0.2f, 1.0f
    )

    val ELECTRIC_GRADIENT = floatArrayOf(
        1.0f, 0.1f, 0.1f,
        0.1f, 1.0f, 0.1f,
        0.1f, 0.1f, 1.0f
    )

    val CARAMEL_DARKNESS = floatArrayOf(
        0.8f, 0.3f, 0.2f,
        0.1f, 0.7f, 0.1f,
        0.2f, 0.1f, 0.8f
    )

    val FUTURISTIC_GRADIENT = floatArrayOf(
        0.9f, 0.1f, 0.3f,
        0.2f, 0.8f, 0.1f,
        0.1f, 0.3f, 1.0f
    )

    val GREEN_SUN = floatArrayOf(
        0.7f, 0.2f, 0.1f,
        0.1f, 1.1f, 0.1f,
        0.1f, 0.3f, 0.9f
    )

    val RAINBOW_WORLD = floatArrayOf(
        1.0f, 0.0f, 0.0f,
        0.8f, 0.8f, 0.0f,
        0.0f, 0.0f, 1.0f
    )

    val DEEP_PURPLE = floatArrayOf(
        0.9f, 0.2f, 0.5f,
        0.1f, 0.8f, 0.1f,
        0.3f, 0.5f, 1.0f
    )

    val SPACE_PORTAL = floatArrayOf(
        0.2f, 0.1f, 0.8f,
        0.1f, 1.0f, 0.1f,
        0.8f, 0.1f, 0.2f
    )

    val RED_SWIRL = floatArrayOf(
        1.0f, 0.5f, 0.2f,
        0.2f, 0.3f, 0.8f,
        0.1f, 0.8f, 0.5f
    )

    val DIGITAL_CODE = floatArrayOf(
        0.1f, 0.8f, 0.1f,
        0.5f, 0.2f, 0.5f,
        0.1f, 0.8f, 0.1f
    )

    fun getTemperature(v: Float): FloatArray {
        return floatArrayOf(1 + v, 0f, 0f, 0f, 1f, 0f, 0f, 0f, 1f - v)
    }

    fun getTint(v: Float): FloatArray {
        return floatArrayOf(1 + v, 0f, 0f, 0f, 1f, 0f, 0f, 0f, 1f + v)
    }

    fun hueRotation(radian: Float): FloatArray {
        val cos = cos(radian)
        val sin = sin(radian)
        return floatArrayOf(
            0.213f + (cos * 0.787f) - (sin * 0.213f),
            (0.715f) - (cos * 0.715f) - (sin * 0.715f),
            (0.072f) - (cos * 0.072f) + (sin * 0.928f),
            (0.213f) - (cos * 0.213f) + (sin * 0.143f),
            (0.715f) + (cos * 0.285f) + (sin * 0.140f),
            (0.072f) - (cos * 0.072f) - (sin * 0.283f),
            (0.213f) - (cos * 0.213f) - (sin * 0.787f),
            (0.715f) - (cos * 0.715f) + (sin * 0.715f),
            (0.072f) + (cos * 0.928f) + (sin * 0.072f)
        )
    }

    object Assistance {
        val PROTANOMALY = floatArrayOf(0.817f, 0.183f, 0f, 0.333f, 0.667f, 0f, 0f, 0.125f, 0.875f)
        val DEUTAROMALY = floatArrayOf(0.8f, 0.2f, 0f, 0.258f, 0.742f, 0f, 0f, 0.142f, 0.858f)
        val TRITONOMALY = floatArrayOf(.967f, 0.033f, 0f, 0f, 0.733f, 0.267f, 0f, 0.183f, 0.817f)
        val PROTANOPIA = floatArrayOf(0.567f, 0.433f, 0f, 0.558f, 0.442f, 0f, 0f, 0.242f, 0.758f)
        val DEUTARONOPIA = floatArrayOf(0.625f, 0.375f, 0f, 0.7f, 0.3f, 0f, 0f, 0.3f, 0.7f)
        val TRITANOPIA = floatArrayOf(0.95f, 0.05f, 0f, 0f, 0.433f, 0.567f, 0f, 0.475f, 0.525f)
        val ACHROMATOPSIA = floatArrayOf(0.299f, 0.587f, 0.114f, 0.299f, 0.587f, 0.114f, 0.299f, 0.587f, 0.114f)
        val ACHROMATOMALY = floatArrayOf(0.618f, 0.320f, 0.062f, 0.163f, 0.775f, 0.062f, 0.163f, 0.320f, 0.516f)
    }

}
