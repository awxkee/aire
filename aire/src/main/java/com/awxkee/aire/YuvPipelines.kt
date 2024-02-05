package com.awxkee.aire

import java.nio.ByteBuffer

interface YuvPipelines {
    fun Yuv420NV21ToRGBA(
        yBuffer: ByteBuffer,
        yStride: Int,
        uvBuffer: ByteBuffer,
        uvStride: Int,
        width: Int,
        height: Int
    ): ByteBuffer

    fun Yuv420NV21ToBGR(
        yBuffer: ByteBuffer,
        yStride: Int,
        uvBuffer: ByteBuffer,
        uvStride: Int,
        width: Int,
        height: Int
    ): ByteBuffer
}