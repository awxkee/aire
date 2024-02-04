package com.awxkee.aire

import android.graphics.Bitmap

interface TonePipelines {
    fun logarithmicToneMapping(bitmap: Bitmap, exposure: Float = 1.0f): Bitmap

    fun acesFilmicToneMapping(bitmap: Bitmap, exposure: Float = 1.0f): Bitmap
}