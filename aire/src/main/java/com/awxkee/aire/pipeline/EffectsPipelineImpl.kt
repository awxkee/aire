package com.awxkee.aire.pipeline

import android.graphics.Bitmap
import com.awxkee.aire.EffectsPipelines

class EffectsPipelineImpl : EffectsPipelines {
    override fun marble(
        bitmap: Bitmap,
        intensity: Float,
        turbulence: Float,
        amplitude: Float
    ): Bitmap {
        return marbleImpl(bitmap, intensity, turbulence, amplitude)
    }

    override fun perlinDistortion(
        bitmap: Bitmap,
        intensity: Float,
        turbulence: Float,
        amplitude: Float
    ): Bitmap {
        return perlinDistortionImpl(bitmap, intensity, turbulence, amplitude)
    }

    override fun waterEffect(
        bitmap: Bitmap,
        fractionSize: Float,
        frequencyX: Float,
        amplitudeX: Float,
        frequencyY: Float,
        amplitudeY: Float
    ): Bitmap {
        return waterEffectImpl(bitmap, fractionSize, frequencyX, amplitudeX, frequencyY, amplitudeY)
    }

    override fun fractalGlass(bitmap: Bitmap, glassSize: Float, amplitude: Float): Bitmap {
        return fractalGlassImpl(bitmap, glassSize, amplitude)
    }

    override fun oil(bitmap: Bitmap, radius: Int, levels: Float): Bitmap {
        return oilImpl(bitmap, radius, levels)
    }

    override fun crystallize(bitmap: Bitmap, numClusters: Int, strokeColor: Int): Bitmap {
        return crystallizeImpl(bitmap, numClusters, strokeColor)
    }

    private external fun perlinDistortionImpl(
        bitmap: Bitmap,
        intensity: Float,
        turbulence: Float,
        amplitude: Float
    ): Bitmap

    private external fun fractalGlassImpl(
        bitmap: Bitmap,
        glassSize: Float,
        amplitude: Float
    ): Bitmap

    private external fun waterEffectImpl(
        bitmap: Bitmap,
        fractionSize: Float,
        frequencyX: Float,
        amplitudeX: Float,
        frequencyY: Float,
        amplitudeY: Float
    ): Bitmap

    private external fun marbleImpl(
        bitmap: Bitmap,
        intensity: Float = 0.5f,
        turbulence: Float = 1f,
        amplitude: Float = 1f
    ): Bitmap

    private external fun oilImpl(bitmap: Bitmap, radius: Int, levels: Float): Bitmap

    private external fun crystallizeImpl(
        bitmap: Bitmap,
        clustersCount: Int,
        strokeColor: Int
    ): Bitmap
}