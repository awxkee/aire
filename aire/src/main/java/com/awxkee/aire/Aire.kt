package com.awxkee.aire

import androidx.annotation.Keep
import com.awxkee.aire.pipeline.BasePipelinesImpl
import com.awxkee.aire.pipeline.BlurPipelinesImpl
import com.awxkee.aire.pipeline.EffectsPipelineImpl
import com.awxkee.aire.pipeline.ProcessingPipelinesImpl
import com.awxkee.aire.pipeline.ScalePipelinesImpl
import com.awxkee.aire.pipeline.ShiftPipelineImpl
import com.awxkee.aire.pipeline.TonePipelinesImpl

@Keep
object Aire : BlurPipelines by BlurPipelinesImpl(),
    ShiftPipelines by ShiftPipelineImpl(),
    BasePipelines by BasePipelinesImpl(),
    ProcessingPipelines by ProcessingPipelinesImpl(),
    EffectsPipelines by EffectsPipelineImpl(),
    ScalePipelines by ScalePipelinesImpl(),
    TonePipelines by TonePipelinesImpl() {
    init {
        System.loadLibrary("aire")
    }
}