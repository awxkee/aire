package com.awxkee.aire

import android.graphics.Bitmap

interface BlurPipelines {

    fun gaussianBlur(bitmap: Bitmap, radius: Int, sigma: Float): Bitmap

    fun bilateralBlur(bitmap: Bitmap, radius: Int, rangeSigma: Float, spatialSigma: Float): Bitmap

    fun boxBlur(bitmap: Bitmap, radius: Int): Bitmap

    fun stackBlur(bitmap: Bitmap, radius: Int): Bitmap

    fun medianBlur(bitmap: Bitmap, radius: Int): Bitmap

}