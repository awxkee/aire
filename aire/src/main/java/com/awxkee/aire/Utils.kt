package com.awxkee.aire

fun argbToRgba(argbColor: Int): Int {
    val alpha = argbColor shr 24 and 0xFF
    val red = argbColor shr 16 and 0xFF
    val green = argbColor shr 8 and 0xFF
    val blue = argbColor and 0xFF

    // Pack components into RGBA format
    return red shl 24 or (green shl 16) or (blue shl 8) or alpha
}
