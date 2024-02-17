package com.awxkee.aire

import android.graphics.Bitmap

interface TonePipelines {
    fun logarithmicToneMapping(bitmap: Bitmap, exposure: Float = 1.0f): Bitmap

    fun acesFilmicToneMapping(bitmap: Bitmap, exposure: Float = 1.0f): Bitmap

    fun exposure(bitmap: Bitmap, exposure: Float): Bitmap

    fun hejlBurgessToneMapping(bitmap: Bitmap, exposure: Float = 1.0f): Bitmap

    fun hableFilmicToneMapping(bitmap: Bitmap, exposure: Float = 1.0f): Bitmap

    fun acesHillToneMapping(bitmap: Bitmap, exposure: Float = 1.0f): Bitmap

    fun monochrome(bitmap: Bitmap, color: FloatArray, exposure: Float = 1.0f): Bitmap

    fun whiteBalance(bitmap: Bitmap, temperature: Float = 1f, tint: Float = 0.0f): Bitmap

    fun mobius(
        bitmap: Bitmap,
        exposure: Float = 1.0f,
        transition: Float = 0.9f,
        peak: Float = 1.0f
    ): Bitmap

    fun uchimura(bitmap: Bitmap, exposure: Float = 1.0f): Bitmap

    fun aldridge(bitmap: Bitmap, exposure: Float = 1.0f, cutoff: Float = 0.025f): Bitmap

    fun drago(bitmap: Bitmap, exposure: Float = 1.0f, sdrWhitePoint: Float = 250.0f): Bitmap
}