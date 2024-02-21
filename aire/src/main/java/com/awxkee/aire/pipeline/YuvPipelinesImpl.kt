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

import com.awxkee.aire.YuvPipelines
import java.nio.ByteBuffer

class YuvPipelinesImpl: YuvPipelines {
    override fun Yuv420NV21ToRGBA(
        yBuffer: ByteBuffer,
        yStride: Int,
        uvBuffer: ByteBuffer,
        uvStride: Int,
        width: Int,
        height: Int
    ): ByteBuffer {
        val dstBuffer = ByteBuffer.allocateDirect(4 * width * height)
        return Yuv420nV21ToRGBA(dstBuffer, yBuffer, yStride, uvBuffer, uvStride, width, height)
    }

    override fun Yuv420NV21ToBGR(
        yBuffer: ByteBuffer,
        yStride: Int,
        uvBuffer: ByteBuffer,
        uvStride: Int,
        width: Int,
        height: Int
    ): ByteBuffer {
        val dstBuffer = ByteBuffer.allocateDirect(3 * width * height)
        return Yuv420nV21ToBGRImpl(dstBuffer, yBuffer, yStride, uvBuffer, uvStride, width, height)
    }

    private external fun Yuv420nV21ToRGBA(
        dstBuffer: ByteBuffer,
        yBuffer: ByteBuffer,
        yStride: Int,
        uvBuffer: ByteBuffer,
        uvStride: Int,
        width: Int,
        height: Int
    ): ByteBuffer

    private external fun Yuv420nV21ToBGRImpl(
        dstBuffer: ByteBuffer,
        yBuffer: ByteBuffer,
        yStride: Int,
        uvBuffer: ByteBuffer,
        uvStride: Int,
        width: Int,
        height: Int
    ): ByteBuffer
}