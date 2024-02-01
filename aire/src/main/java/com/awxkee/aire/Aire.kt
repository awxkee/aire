package com.awxkee.aire

import com.awxkee.aire.pipeline.BlurPipelinesImpl
import com.awxkee.aire.pipeline.ShiftPipelineImpl

object Aire : BlurPipelines by BlurPipelinesImpl(), ShiftPipelines by ShiftPipelineImpl() {
    init {
        System.loadLibrary("aire")
    }
}