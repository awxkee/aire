package com.awxkee.aire

import android.graphics.Bitmap
import androidx.annotation.IntRange

interface BlurPipelines {

    fun gaussianBlur(bitmap: Bitmap, radius: Int, sigma: Float): Bitmap

    fun bilateralBlur(bitmap: Bitmap, radius: Int, rangeSigma: Float, spatialSigma: Float): Bitmap

    fun fastBilateralBlur(bitmap: Bitmap, rangeSigma: Float, spatialSigma: Float): Bitmap

    fun boxBlur(bitmap: Bitmap, radius: Int): Bitmap

    fun poissonBlur(bitmap: Bitmap, radius: Int): Bitmap

    fun stackBlur(bitmap: Bitmap, radius: Int): Bitmap

    fun medianBlur(
        bitmap: Bitmap,
        kernelSize: Int
    ): Bitmap

    fun tentBlur(bitmap: Bitmap, radius: Int): Bitmap

    fun anisotropicDiffusion(
        bitmap: Bitmap,
        @IntRange(from = 1) numOfSteps: Int = 20,
        conduction: Float = 0.1f,
        diffusion: Float = 0.01f
    ): Bitmap

}