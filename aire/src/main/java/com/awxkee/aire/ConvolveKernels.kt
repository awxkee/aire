/*
 *
 *  * MIT License
 *  *
 *  * Copyright (c) 2024 Radzivon Bartoshyk
 *  * jxl-coder [https://github.com/awxkee/jxl-coder]
 *  *
 *  * Created by Radzivon Bartoshyk on 2/21/24, 6:13 PM
 *  *
 *  * Permission is hereby granted, free of charge, to any person obtaining a copy
 *  * of this software and associated documentation files (the "Software"), to deal
 *  * in the Software without restriction, including without limitation the rights
 *  * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 *  * copies of the Software, and to permit persons to whom the Software is
 *  * furnished to do so, subject to the following conditions:
 *  *
 *  * The above copyright notice and this permission notice shall be included in all
 *  * copies or substantial portions of the Software.
 *  *
 *  * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *  * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *  * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 *  * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *  * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 *  * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 *  * SOFTWARE.
 *  *
 *
 */

package com.awxkee.aire

import java.security.InvalidParameterException
import kotlin.math.exp
import kotlin.math.pow
import kotlin.math.sqrt

object ConvolveKernels {

    /**
     * Box blur. Just for understanding how it works
     */
    fun box(kernelSize: Int): FloatArray {
        var kernel = FloatArray(kernelSize * kernelSize) { _ -> 1.0f }
        val sum = kernel.reduce { acc, fl -> acc + fl }
        if (sum != 0.0f) {
            kernel = kernel.map { (it / sum).toFloat() }.toFloatArray()
        }
        return kernel
    }

    /**
     * Just regular gaussian blur 2D kernel
     */
    fun gaussian(kernelSize: Int, sigma: Float): FloatArray {
        var kernel = FloatArray(kernelSize * kernelSize) { _ -> 1.0f }
        var sum: Double = 0.0
        for (row in 0..<kernelSize) {
            for (col in 0..<kernelSize) {
                val x =
                    exp(-((row * row).toDouble() + (col * col).toDouble()) / (2 * sigma * sigma))
                kernel[row * kernelSize + col] = x.toFloat()
                sum += x
            }
        }
        // normalize the kernel
        if (sum != 0.0) {
            kernel = kernel.map { (it / sum).toFloat() }.toFloatArray()
        }
        return kernel
    }

    /**
     * Kernel that produces a ring, some interesting effects appears after, it will form a ring inside and blur by a ring
     */
    fun ring(kernelSize: Int): FloatArray {
        if (kernelSize < 1) throw InvalidParameterException("Only kernelSize > 1 is supported")
        var kernel = FloatArray(kernelSize * kernelSize) { _ -> 0.0f }
        var sum: Double = 0.0
        for (row in 0..<kernelSize) {
            for (col in 0..<kernelSize) {
                val distance = sqrt(
                    (row - kernelSize).toDouble().pow(2.0) + (col - kernelSize).toDouble()
                        .pow(2.0)
                )
                val ringWidth = 1.0f // Adjust this value to control the width of the ring

                // Form a ring by checking if the distance is within a specified range
                if (distance > kernelSize - ringWidth / 2 && distance < kernelSize + ringWidth / 2) {
                    kernel[row * kernelSize + col] = 1.0f
                    sum += 1.0
                } else {
                    kernel[row * kernelSize + col] = 0.0f
                }
            }
        }
        // normalize the kernel
        if (sum != 0.0) {
            kernel = kernel.map { (it / sum).toFloat() }.toFloatArray()
        }
        return kernel
    }

    /**
     * Kernel that produces a cross, some interesting effects appears after, it will form a cross inside and blur by a cross shape
     */
    fun cross(kernelSize: Int): FloatArray {
        if (kernelSize < 1) throw InvalidParameterException("Only kernelSize > 1 is supported")
        var kernel = FloatArray(kernelSize * kernelSize) { _ -> 1.0f }
        var sum: Double = 0.0
        for (row in 0..<kernelSize) {
            for (col in 0..<kernelSize) {

                val distance = sqrt(
                    (row - kernelSize / 2).toDouble().pow(2.0) + (col - kernelSize / 2).toDouble()
                        .pow(2.0)
                )

                if (row == col && distance < kernelSize / 2 + 1) {
                    kernel[row * kernelSize + col] = 1.0f
                    sum += 1.0
                } else {
                    kernel[row * kernelSize + col] = 0.0f
                }
            }
        }
        // normalize the kernel
        if (sum != 0.0) {
            kernel = kernel.map { (it / sum).toFloat() }.toFloatArray()
        }
        return kernel
    }

    /**
     * Kernel that produces a circle, some interesting effects appears after, it will form a circle inside and blur by a circle
     */
    fun circle(kernelSize: Int): FloatArray {
        if (kernelSize < 5) throw InvalidParameterException("Only kernelSize >= 3 is supported")
        var kernel = FloatArray(kernelSize * kernelSize) { _ -> 0.0f }
        var sum: Double = 0.0

        val centerX = kernelSize / 2
        val centerY = kernelSize / 2

        val radius = kernelSize.toFloat() / 3.0f

        for (row in 0..<kernelSize) {
            for (col in 0..<kernelSize) {
                val distance = sqrt(
                    (row - centerX).toDouble().pow(2.0) + (col - centerY).toDouble()
                        .pow(2.0)
                )

                if (distance <= radius) {
                    kernel[row * kernelSize + col] = 1.0f
                    sum += 1.0
                } else {
                    kernel[row * kernelSize + col] = 0.0f
                }
            }
        }
        // normalize the kernel
        if (sum != 0.0) {
            kernel = kernel.map { (it / sum).toFloat() }.toFloatArray()
        }
        return kernel
    }

    /**
     * Kernel that produces a star, some interesting effects appears after, it will form a star inside and blur by a star
     */
    fun star(kernelSize: Int): FloatArray {
        if (kernelSize < 3) throw InvalidParameterException("Only radius > 1 is supported")
        var kernel = FloatArray(kernelSize * kernelSize) { _ -> 0.0f }
        var sum: Double = 0.0

        val centerX = kernelSize / 2
        val centerY = kernelSize / 2

        for (row in 0..<kernelSize) {
            for (col in 0..<kernelSize) {

                if (row == centerX || col == centerY) {
                    kernel[row * kernelSize + col] = 1.0f
                    sum += 1.0
                } else {
                    kernel[row * kernelSize + col] = 0.0f
                }
            }
        }
        // normalize the kernel
        if (sum != 0.0) {
            kernel = kernel.map { (it / sum).toFloat() }.toFloatArray()
        }
        return kernel
    }
}