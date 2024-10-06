/*
 *
 *  * MIT License
 *  *
 *  * Copyright (c) 2024 Radzivon Bartoshyk
 *  * aire [https://github.com/awxkee/aire]
 *  *
 *  * Created by Radzivon Bartoshyk on 6/12/24, 11:56 PM
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

enum class EdgeMode(internal val value: Int) {
    /**
     *  If kernel goes out of bounds it will be clipped to an edge and edge pixel replicated across filter
     */
    CLAMP(0),

    /**
     *  If kernel goes out of bounds it will be clipped, this is a slightly faster than clamp, however have different visual effects at the edge
     */
    WRAP(1),

    /**
     *  If filter goes out of bounds image will be replicated with rule fedcba|abcdefgh|hgfedcb
     */
    REFLECT(2),

    /**
     *  If filter goes out of bounds image will be replicated with rule fedcba|abcdefgh|hgfedcb
     */
    REFLECT_101(3),
    /**
     *  If filter goes out of bounds image will be replicated with provided constant value, this filter is not everywhere supported
     */
    CONSTANT(4),

}