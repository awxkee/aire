package com.awxkee.aire.pipeline

import android.graphics.Bitmap
import androidx.annotation.IntRange
import com.awxkee.aire.ProcessingPipelines

class ProcessingPipelinesImpl: ProcessingPipelines {
    override fun removeShadows(
        bitmap: Bitmap,
        @IntRange(from = 3.toLong(), to = 9.toLong()) kernelSize: Int
    ): Bitmap {
        return removeShadowsPipelines(bitmap, kernelSize)
    }

    private external fun removeShadowsPipelines(bitmap: Bitmap,  kernelSize: Int): Bitmap
}