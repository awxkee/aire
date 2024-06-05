/*
 *
 *  * MIT License
 *  *
 *  * Copyright (c) 2024 Radzivon Bartoshyk
 *  * aire [https://github.com/awxkee/aire]
 *  *
 *  * Created by Radzivon Bartoshyk on 2/21/24 6:13 PM
 *  *
 *  * Permission is hereby granted free of charge to any person obtaining a copy
 *  * of this software and associated documentation files (the "Software") to deal
 *  * in the Software without restriction including without limitation the rights
 *  * to use copy modify merge publish distribute sublicense and/or sell
 *  * copies of the Software and to permit persons to whom the Software is
 *  * furnished to do so subject to the following conditions:
 *  *
 *  * The above copyright notice and this permission notice shall be included in all
 *  * copies or substantial portions of the Software.
 *  *
 *  * THE SOFTWARE IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND EXPRESS OR
 *  * IMPLIED INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY
 *  * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 *  * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM DAMAGES OR OTHER
 *  * LIABILITY WHETHER IN AN ACTION OF CONTRACT TORT OR OTHERWISE ARISING FROM
 *  * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 *  * SOFTWARE.
 *  *
 *
 */

package com.awxkee.aire

enum class ResizeFunction(internal val value: Int) {
    Bilinear(0),
    Nearest(1),
    Cubic(2),
    MitchellNetravalli(3),
    CatmullRom(4),
    Hermite(5),
    BSpline(6),
    Hann(7),
    Bicubic(8),
    Hamming(9),
    Hanning(10),
    Blackman(11),
    Welch(12),
    Quadric(13),
    Gaussian(14),
    Sphinx(15),
    Bartlett(16),
    Robidoux(17),
    RobidouxSharp(18),
    Spline16(19),
    Spline36(20),
    Spline64(21),
    Kaiser(22),
    BartlettHann(23),
    Box(24),
    Bohman(25),
    Lanczos2(26),
    Lanczos3(27),
    Lanczos4(28),
    Lanczos2Jinc(29),
    Lanczos3Jinc(30),
    Lanczos4Jinc(31),
}