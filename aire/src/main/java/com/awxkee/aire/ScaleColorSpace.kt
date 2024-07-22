/*
 *
 *  * MIT License
 *  *
 *  * Copyright (c) 2024 Radzivon Bartoshyk
 *  * aire [https://github.com/awxkee/aire]
 *  *
 *  * Created by Radzivon Bartoshyk on 5/17/24, 6:01 PM
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

enum class ScaleColorSpace(internal val value: Int) {
    SRGB(0),

    /**
     * CIELAB
     */
    LAB(1),
    LINEAR(2),
    LUV(3),
    SIGMOIDAL(4),

    /**
     * XYZ uses sRGB transfer function and D65 observer point
     */
    XYZ(5),

    /**
     * Scaling in linear f32 slower than 8-bit approximation.
     * sRGB transfer function
     */
    LINEAR_F32_SRGB(6),

    /**
     * Scaling in linear f32 slower than 8-bit approximation.
     * Rec.709 transfer function
     */
    LINEAR_F32_REC709(7),

    /**
     * Scaling in linear f32 slower than 8-bit approximation.
     * Pure gamma 2.2 transfer function
     */
    LINEAR_F32_GAMMA_2_2(8),

    /**
     * Scaling in linear f32 slower than 8-bit approximation.
     * Pure gamma 2.8 transfer function
     */
    LINEAR_F32_GAMMA_2_8(9),

    /**
     * CIE LCh color space, scaling in f32
     */
    LCH(10),

    /**
     * Oklab
     * sRGB transfer function
     */
    OKLAB_SRGB(11),

    /**
     * Oklab
     * Rec.709 transfer function
     */
    OKLAB_REC709(12),

    /**
     * Oklab
     * Pure gamma 2.2 transfer function
     */
    OKLAB_GAMMA_2_2(13),

    /**
     * Oklab
     * Pure gamma 2.8 transfer function
     */
    OKLAB_GAMMA_2_8(14),

    /**
     * Jzazbz
     * sRGB transfer function
     */
    JZAZBZ_SRGB(15),

    /**
     * Jzazbz
     * Rec.709 transfer function
     */
    JZAZBZ_REC709(16),

    /**
     * Jzazbz
     * Pure gamma 2.2 transfer function
     */
    JZAZBZ_GAMMA_2_2(17),

    /**
     * Jzazbz
     * Pure gamma 2.8 transfer function
     */
    JZAZBZ_GAMMA_2_8(18),
}