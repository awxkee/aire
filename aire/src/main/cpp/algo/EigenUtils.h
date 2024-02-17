//
// Created by Radzivon Bartoshyk on 09/02/2024.
//

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
    int rows = grid.size();
    if (rows == 0) return;
    int cols = grid.cols();
    if (cols == 0) return;

    if (startX < 0 || startX >= rows || startY < 0 || startY >= cols)
        return;

    std::queue<std::pair<int, int>> pointsQueue;
    pointsQueue.push({startX, startY});

    while (!pointsQueue.empty()) {
        auto [x, y] = pointsQueue.front();
        pointsQueue.pop();

        if (x < 0 || x >= rows || y < 0 || y >= cols || grid(x, y) != target)
            continue;

        grid(x, y) = replacement;

        pointsQueue.push({x + 1, y});
        pointsQueue.push({x - 1, y});
        pointsQueue.push({x, y + 1});
        pointsQueue.push({x, y - 1});
    }
}


static Eigen::MatrixXi getBokehEffect(int radius, float startAngle, int sides) {
    Eigen::MatrixXi kernel(2 * radius + 1, 2 * radius + 1);
    kernel = Eigen::MatrixXi::Ones(2 * radius + 1, 2 * radius + 1);
    int diameter = (radius * 2) + 1;
    float startRadians = startAngle;
    float endRadians = M_PI * 2 + startAngle;

    float previousX = -1;
    float previousY = -1;

    float angle = ((M_PI * 2.0f) / float(sides));

    for (float radian = startRadians; radian < endRadians + 2; radian += angle) {
        float x = float(radius) + sin(radian) * (radius - 0.01f);
        float y = float(radius) + cos(radian) * (radius - 0.01f);

        if (previousX != -1) {
            float deltaX = 1.0 / std::max(std::abs(previousX - x), std::abs(previousY - y));

            for (float t = 0; t < 1; t += deltaX) {
                float newX = lerp(previousX, x, t);
                float newY = lerp(previousY, y, t);

                int coordX = round(newX);
                int coordY = round(newY);

                if (coordX >= 0 && coordX < diameter && coordY >= 0 && coordY < diameter) {
                    kernel(coordY, coordX) = 0;
                }
            }
        }

        previousX = x;
        previousY = y;
    }

    floodFill(kernel, 0, 0, 1, 0);
    return kernel;
}