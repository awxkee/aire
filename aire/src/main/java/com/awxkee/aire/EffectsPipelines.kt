package com.awxkee.aire

import android.graphics.Bitmap
import android.graphics.Color

interface EffectsPipelines {
    fun marble(
        bitmap: Bitmap,
        intensity: Float = 0.02f,
        turbulence: Float = 1f,
        amplitude: Float = 1f
    ): Bitmap

    fun perlinDistortion(
        bitmap: Bitmap,
        intensity: Float = 0.02f,
        turbulence: Float = 1f,
        amplitude: Float = 1f
    ): Bitmap

    fun waterEffect(
        bitmap: Bitmap,
        fractionSize: Float = 0.05f,
        frequencyX: Float = 2f,
        amplitudeX: Float = 0.5f,
        frequencyY: Float = 2f,
        amplitudeY: Float = 0.5f,
    ): Bitmap

    fun fractalGlass(bitmap: Bitmap, glassSize: Float = 0.2f, amplitude: Float = 0.2f): Bitmap

    fun oil(bitmap: Bitmap, radius: Int, levels: Float = 1f): Bitmap

    /* Prefer relative clustering for ex. width*height * 0.01f = numClusters */
    fun crystallize(bitmap: Bitmap, numClusters: Int, strokeColor: Int = Color.TRANSPARENT): Bitmap
}