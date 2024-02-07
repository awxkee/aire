//
// Created by Radzivon Bartoshyk on 01/02/2024.
//

#pragma once

#include <vector>

using namespace std;

#define AIRE_MATH_UTILS_AINL inline __attribute__((flatten))

template<class T>
AIRE_MATH_UTILS_AINL
static T clamp(const T value, const T minValue, const T maxValue) {
    return (value < minValue) ? minValue : ((value > maxValue) ? maxValue : value);
}

static int clamp(const int value, const int minValue, const int maxValue) {
    return (value < minValue) ? minValue : ((value > maxValue) ? maxValue : value);
}

static vector<float> compute1DGaussianKernel(float width, float sigma) {
    vector<float> kernel(ceil(width));
    int mean = ceil(width) / 2;
    float sum = 0;
    for (int x = 0; x < width; x++) {
        kernel[x] = (float) exp(-0.5 * pow((x - mean) / sigma, 2.0));
        sum += kernel[x];
    }
    for (int x = 0; x < width; x++)
        kernel[x] /= sum;
    return std::move(kernel);
}

static std::vector<std::vector<int>> getStructuringKernel(int size) {
    std::vector<std::vector<int>> kernel(size, std::vector<int>(size, 1));
    return std::move(kernel);
}

#include <queue>

static void floodFill(std::vector<std::vector<int>>& grid, int startX, int startY, int target, int replacement) {
    int rows = grid.size();
    if (rows == 0) return;
    int cols = grid[0].size();
    if (cols == 0) return;

    if (startX < 0 || startX >= rows || startY < 0 || startY >= cols)
        return;

    std::queue<std::pair<int, int>> pointsQueue;
    pointsQueue.push({startX, startY});

    while (!pointsQueue.empty()) {
        auto [x, y] = pointsQueue.front();
        pointsQueue.pop();

        if (x < 0 || x >= rows || y < 0 || y >= cols || grid[x][y] != target)
            continue;

        grid[x][y] = replacement;

        pointsQueue.push({x + 1, y});
        pointsQueue.push({x - 1, y});
        pointsQueue.push({x, y + 1});
        pointsQueue.push({x, y - 1});
    }
}


static std::vector<std::vector<int>> getBokehEffect(int radius, float startAngle, int sides) {
    std::vector<std::vector<int>> kernel(2 * radius + 1, std::vector<int>(2 * radius + 1, 1));
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
                    kernel[coordY][coordX] = 0;
                }
            }
        }

        previousX = x;
        previousY = y;
    }

    floodFill(kernel, 0, 0, 1, 0);

//    std::queue<std::pair<int, int>> q;
//    q.push({0, 0});
//
//    while (!q.empty()) {
//        int row = q.front().first;
//        int col = q.front().second;
//        q.pop();
//
//        // Check if current cell is within bounds and has oldColor
//        if (row < 0 || row >= diameter || col < 0 || col >= diameter || kernel[row][col] != 1) continue;
//
//        // Fill current cell with newColor
//        kernel[row][col] = 0;
//
//        // Add adjacent cells to the queue
//        q.push({row - 1, col}); // Up
//        q.push({row + 1, col}); // Down
//        q.push({row, col - 1}); // Left
//        q.push({row, col + 1}); // Right
//    }
    return kernel;
}

class LowPassFilter {
public:
    LowPassFilter(double cutoff) {
        mCutoff = cutoff;
        mY1 = 0;
    }

    double process(double x0) {
        double y0 = x0 * (1 - mCutoff) + mY1 * mCutoff;
        mY1 = y0;

        return y0;
    }

private:
    double mCutoff;
    double mY1;
};

template<class T>
static inline T min3(T a, T b, T c) {
    return std::min(a, std::min(b, c));
}

static inline int min3(int a, int b, int c) {
    return std::min(a, std::min(b, c));
}

static inline int max3(int a, int b, int c) {
    return std::max(a, std::max(b, c));
}