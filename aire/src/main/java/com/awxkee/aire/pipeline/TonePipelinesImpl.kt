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

    override fun exposure(bitmap: Bitmap, exposure: Float): Bitmap {
        return exposureImpl(bitmap, exposure)
    }

    override fun hejlBurgessToneMapping(bitmap: Bitmap, exposure: Float): Bitmap {
        return hejlBurgessToneMappingImpl(bitmap, exposure)
    }

    override fun hableFilmicToneMapping(bitmap: Bitmap, exposure: Float): Bitmap {
        return hableFilmicImpl(bitmap, exposure)
    }

    override fun acesHillToneMapping(bitmap: Bitmap, exposure: Float): Bitmap {
        return acesHillImpl(bitmap, exposure)
    }

    private external fun acesHillImpl(bitmap: Bitmap, exposure: Float): Bitmap

    private external fun hableFilmicImpl(bitmap: Bitmap, exposure: Float): Bitmap

    private external fun exposureImpl(bitmap: Bitmap, exposure: Float): Bitmap

    private external fun logarithmicImpl(bitmap: Bitmap, exposure: Float): Bitmap

    private external fun acesFilmicImpl(bitmap: Bitmap, exposure: Float): Bitmap

    private external fun hejlBurgessToneMappingImpl(bitmap: Bitmap, exposure: Float = 1.0f): Bitmap
}