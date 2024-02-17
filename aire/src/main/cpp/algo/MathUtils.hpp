//
// Created by Radzivon Bartoshyk on 01/02/2024.
//

#pragma once

#include <vector>
#include <queue>

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

static bool isSquareRootInteger(float N) {
    if (N < 0)
        return false;
    float squareRootN = sqrt(N);
    return std::pow(static_cast<int>(squareRootN), 2) == N;
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

static int computeStride(int width, int pixelSize, int components) {
    int lineWidth = width * pixelSize * components;
    int alignment = 64;
    int padding = (alignment - (lineWidth % alignment)) % alignment;
    int dstStride = lineWidth + padding;
    return dstStride;
}

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

inline
static uint32_t packRGBA(uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
    return ((uint32_t) a << 24) | ((uint32_t) b << 16) | ((uint32_t) g << 8) | (uint32_t) r;
}