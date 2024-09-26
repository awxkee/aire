/*
 *
 *  * MIT License
 *  *
 *  * Copyright (c) 2024 Radzivon Bartoshyk
 *  * aire [https://github.com/awxkee/aire]
 *  *
 *  * Created by Radzivon Bartoshyk on 9/26/24, 8:13 PM
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

/**
 *  If you're not familiar with morphological ops then read https://docs.opencv.org/4.x/d9/d61/tutorial_py_morphological_ops.html
 */
enum class MorphOp(internal val value: Int) {
    DILATE(0),
    ERODE(1),

    /**
     *  It is the dilation followed by erosion
     */
    OPENING(2),

    /**
     *  It is the erosion followed by dilation
     */
    CLOSING(3),

    /**
     *  It is the difference between dilation and erosion of an image.
     *  *NOTE*: Since this is subtractive if you use RGBA mode with constant alpha the output image will be fully transparent
     */
    GRADIENT(4),

    /**
     *  It is the difference between input image and Opening of the image
     *  *NOTE*: Since this is subtractive if you use RGBA mode with constant alpha the output image will be fully transparent
     */
    TOPHAT(5),

    /**
     *  It is the difference between the closing of the input image and input image
     *  *NOTE*: Since this is subtractive if you use RGBA mode with constant alpha the output image will be fully transparent
     */
    BLACKHAT(6)
}