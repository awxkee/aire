package com.awxkee.aire

import android.graphics.Bitmap

interface ShiftPipelines {
    fun tiltShift(
        bitmap: Bitmap,
        radius: Float,
        sigma: Float,
        anchorX: Float = 0.5f,
        anchorY: Float = 0.5f,
        tiltRadius: Float = 0.2f,
    ): Bitmap

    fun glitch(
        bitmap: Bitmap,
        channelsShiftX: Float = -0.075f,
        channelsShiftY: Float = -0.075f,
        corruptionSize: Float = 0.009f,
        corruptionCount: Int = 60,
        corruptionShiftX: Float = -0.05f,
        corruptionShiftY: Float = 0.0f,
    ): Bitmap
}