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