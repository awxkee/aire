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
}