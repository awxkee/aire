/*
 *
 *  * MIT License
 *  *
 *  * Copyright (c) 2024 Radzivon Bartoshyk
 *  * aire [https://github.com/awxkee/aire]
 *  *
 *  * Created by Radzivon Bartoshyk on 09/02/24, 6:13 PM
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

#pragma once

#include "Eigen/Eigen"
#include <queue>
#include <algorithm>

using namespace std;

inline
static uint32_t packRGBA(Eigen::Vector4i color) {
    uint32_t rgba = (color.w() << 24) | (color.z() << 16) | (color.y() << 8) | color.x();
    return rgba;
}

inline
static Eigen::Vector4i unpackRGBA(uint32_t color) {
    uint8_t a = (color >> 24) & 0xFF;
    uint8_t b = (color >> 16) & 0xFF;
    uint8_t g = (color >> 8) & 0xFF;
    uint8_t r = color & 0xFF;
    Eigen::Vector4i vec = {r, g, b, a};
    return vec;
}

static Eigen::Vector4i packERGBA(uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
    Eigen::Vector4i vec = {r, g, b, a};
    return vec;
}

static Eigen::Vector3i rgbToHSV(const Eigen::Vector3i &rgb) {
    double minVal = rgb.minCoeff();
    double maxVal = rgb.maxCoeff();
    double delta = maxVal - minVal;

    double hue = 0.0;
    double saturation = (maxVal > 0.0) ? delta / maxVal : 0.0;
    double value = maxVal;

    if (delta > 0.0) {
        if (maxVal == rgb[0]) { // Red is dominant color
            hue = 60.0 * fmod(((rgb[1] - rgb[2]) / delta), 6.0);
        } else if (maxVal == rgb[1]) { // Green is dominant color
            hue = 60.0 * (((rgb[2] - rgb[0]) / delta) + 2.0);
        } else { // Blue is dominant color
            hue = 60.0 * (((rgb[0] - rgb[1]) / delta) + 4.0);
        }
    }

    if (hue < 0.0) {
        hue += 360.0;
    }

    return Eigen::Vector3i(hue, saturation, value);
}

static Eigen::Matrix3f getPerspectiveTransform(std::vector<Eigen::Vector2f> src,
                                               std::vector<Eigen::Vector2f> dst) {
    const int N_inv = 8;
    Eigen::MatrixXf xx(N_inv, N_inv);
    xx << src[0].x(), src[0].y(), 1, 0, 0, 0, -src[0].x() * dst[0].x(),
            -src[0].y() * dst[0].x(), src[1].x(), src[1].y(), 1, 0, 0, 0,
            -src[1].x() * dst[1].x(), -src[1].y() * dst[1].x(), src[2].x(), src[2].y(), 1, 0, 0,
            0, -src[2].x() * dst[2].x(), -src[2].y() * dst[2].x(), src[3].x(), src[3].y(), 1, 0,
            0, 0, -src[3].x() * dst[3].x(), -src[3].y() * dst[3].x(), 0, 0, 0, src[0].x(),
            src[0].y(), 1, -src[0].x() * dst[0].y(), -src[0].y() * dst[0].y(), 0, 0, 0,
            src[1].x(), src[1].y(), 1, -src[1].x() * dst[1].y(), -src[1].y() * dst[1].y(), 0, 0,
            0, src[2].x(), src[2].y(), 1, -src[2].x() * dst[2].y(), -src[2].y() * dst[2].y(), 0,
            0, 0, src[3].x(), src[3].y(), 1, -src[3].x() * dst[3].y(), -src[3].y() * dst[3].y();
    Eigen::MatrixXf xx_inv = xx.inverse();
    Eigen::MatrixXf input(N_inv, 1);
    input << dst[0].x(), dst[1].x(), dst[2].x(), dst[3].x(), dst[0].y(), dst[1].y(), dst[2].y(), dst[3].y();
    Eigen::MatrixXf output = xx_inv * input;
    output.conservativeResize(output.rows() + 1, output.cols());
    output(8, 0) = 1;
    Eigen::Matrix3f ret;
    ret << output(0,0), output(1,0), output(2,0), output(3,0),
            output(4,0), output(5,0), output(6,0), output(7,0),
            output(8,0);
    return ret;
}

static Eigen::Matrix<float, 4, 2> computeBoundingBox(const Eigen::Matrix3f& transformMatrix, int width, int height) {
    Eigen::Matrix<float, 4, 2> points;
    Eigen::Vector3f corner1 = transformMatrix * Eigen::Vector3f(0, 0, 1);
    Eigen::Vector3f corner2 = transformMatrix * Eigen::Vector3f(width - 1, 0, 1);
    Eigen::Vector3f corner3 = transformMatrix * Eigen::Vector3f(0, height - 1, 1);
    Eigen::Vector3f corner4 = transformMatrix * Eigen::Vector3f(width - 1, height - 1, 1);
    points.row(0) = corner1.hnormalized().head<2>();
    points.row(1) = corner2.hnormalized().head<2>();
    points.row(2) = corner3.hnormalized().head<2>();
    points.row(3) = corner4.hnormalized().head<2>();
    return points;
}

static void floodFill(Eigen::MatrixXi &grid, int startX, int startY, int target, int replacement) {
    int rows = grid.rows();
    if (rows == 0) return;
    int cols = grid.cols();
    if (cols == 0) return;

    if (startX < 0 || startX >= rows || startY < 0 || startY >= cols)
        return;

    std::queue<std::pair<int, int>> pointsQueue;
    pointsQueue.emplace(startX, startY);

    while (!pointsQueue.empty()) {
        auto [x, y] = pointsQueue.front();
        pointsQueue.pop();

        if (x < 0 || x >= rows || y < 0 || y >= cols)
            continue;

        if (grid(y, x) == target) {
            grid(y, x) = replacement;

            if (x + 1 >= 0 && x + 1 < rows) {
                pointsQueue.emplace(x + 1, y);
            }
            if (x - 1 >= 0 && x - 1 < rows) {
                pointsQueue.emplace(x - 1, y);
            }
            if (y + 1 >= 0 && y + 1 < cols) {
                pointsQueue.emplace(x, y + 1);
            }
            if (y - 1 >= 0 && y - 1 < cols) {
                pointsQueue.emplace(x, y - 1);
            }
        }
    }
}

static void revertPixels(Eigen::MatrixXi& matrix) {
    int rows = matrix.rows();
    int cols = matrix.cols();

    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            matrix(i, j) = (matrix(i, j) == 0) ? 1 : 0;
        }
    }
}

static Eigen::MatrixXi getBokehEffect(int kernelSize, int sides) {
    Eigen::MatrixXi kernel(kernelSize, kernelSize);
    kernel = Eigen::MatrixXi::Zero(kernelSize, kernelSize);
    int rows = kernel.rows();
    int cols = kernel.cols();

    float cx = cols / 2.0;
    float cy = rows / 2.0;
    float radius = std::min(cx, cy) * 0.85f;

    for (int i = 0; i < sides; ++i) {
        float angle1 = 2.0f * static_cast<float>(M_PI) * static_cast<float>(i) / static_cast<float>(sides);
        float angle2 = 2.0f * static_cast<float>(M_PI) * static_cast<float>(i + 1) / static_cast<float>(sides);

        int x1 = static_cast<int>(cx + radius * cos(angle1));
        int y1 = static_cast<int>(cy + radius * sin(angle1));

        int x2 = static_cast<int>(cx + radius * cos(angle2));
        int y2 = static_cast<int>(cy + radius * sin(angle2));

        // Bresenham's line algorithm to draw a line between (x1, y1) and (x2, y2)
        int dx = std::abs(x2 - x1);
        int dy = -std::abs(y2 - y1);
        int sx = (x1 < x2) ? 1 : -1;
        int sy = (y1 < y2) ? 1 : -1;
        int err = dx + dy;

        while (true) {
            if (x1 >= 0 && x1 < cols && y1 >= 0 && y1 < rows) {
                kernel(y1, x1) = 1;
            }

            if (x1 == x2 && y1 == y2) break;

            int e2 = 2 * err;

            if (e2 >= dy) {
                err += dy;
                x1 += sx;
            }

            if (e2 <= dx) {
                err += dx;
                y1 += sy;
            }
        }
    }

    floodFill(kernel, kernel.cols() / 2,kernel.rows() / 2, 0, 1);
//    revertPixels(kernel);
    return kernel;
}

template<typename T>
static std::string matrixToString(const Eigen::Matrix<T, Eigen::Dynamic, Eigen::Dynamic>& matrix) {
    std::ostringstream oss;

    for (int i = 0; i < matrix.rows(); ++i) {
        for (int j = 0; j < matrix.cols(); ++j) {
            oss << matrix(i, j);

            // Add a separator (e.g., space) between elements
            if (j < matrix.cols() - 1) {
                oss << " ";
            }
        }

        // Add a newline after each row
        oss << "\n";
    }

    return oss.str();
}

static Eigen::MatrixXi getStructuringKernel(int size) {
    Eigen::MatrixXi kernel(size, size);
    kernel << Eigen::MatrixXi::Ones(size, size);
    return kernel;
}