package com.awxkee.aire.pipeline

import android.graphics.Bitmap
import com.awxkee.aire.ShiftPipelines
import java.security.InvalidParameterException

class ShiftPipelineImpl : ShiftPipelines {
    override fun tiltShift(
        bitmap: Bitmap,
        radius: Float,
        sigma: Float,
        anchorX: Float,
        anchorY: Float,
        tiltRadius: Float
    ): Bitmap {
        if (radius <= 0) throw InvalidParameterException("radius cannot be less or equal zero")
        return tiltShiftImpl(bitmap, radius, sigma, anchorX, anchorY, tiltRadius)
    }

    override fun glitch(
        bitmap: Bitmap,
        channelsShiftX: Float,
        channelsShiftY: Float,
        corruptionSize: Float,
        corruptionCount: Int,
        corruptionShiftX: Float,
        corruptionShiftY: Float,
    ): Bitmap {
        return glitchImpl(
            bitmap,
            channelsShiftX,
            channelsShiftX,
            corruptionSize,
            corruptionCount,
            corruptionShiftX,
            corruptionShiftY
        )
    }

    private external fun tiltShiftImpl(
        bitmap: Bitmap,
        radius: Float,
        sigma: Float,
        anchorX: Float = 0.5f,
        anchorY: Float = 0.5f,
        tiltRadius: Float = 0.2f,
    ): Bitmap

    private external fun glitchImpl(
        bitmap: Bitmap,
        channelsShiftX: Float,
        channelsShiftY: Float,
        corruptionSize: Float,
        corruptionCount: Int,
        corruptionShiftX: Float,
        corruptionShiftY: Float,
    ): Bitmap
}