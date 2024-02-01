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