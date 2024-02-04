package com.awxkee.aire.pipeline

import android.graphics.Bitmap
import com.awxkee.aire.TonePipelines

class TonePipelinesImpl : TonePipelines {
    override fun logarithmicToneMapping(bitmap: Bitmap, exposure: Float): Bitmap {
        return logarithmicImpl(bitmap, exposure)
    }

    override fun acesFilmicToneMapping(bitmap: Bitmap, exposure: Float): Bitmap {
        return acesFilmicImpl(bitmap, exposure)
    }

    private external fun logarithmicImpl(bitmap: Bitmap, exposure: Float): Bitmap

    private external fun acesFilmicImpl(bitmap: Bitmap, exposure: Float): Bitmap
}