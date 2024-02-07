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

static std::vector<std::vector<int>> getBokehEffect(int radius, float startAngle, int sides) {
    std::vector<std::vector<int>> kernel(2 * radius + 1, std::vector<int>(2 * radius + 1, 1));
    int diameter = (radius * 2) + 1;
    float startRadians = startAngle;
    float endRadians = M_PI * 2 + startAngle;

    float previousX = radius + sin(startRadians) * (radius - 0.01);
    float previousY = radius + cos(startRadians) * (radius - 0.01);

    float angle = ((M_PI * 2.0f) / float(sides));

    for (float radian = startRadians; radian <= endRadians; radian += angle) {
        float x = radius + sin(radian) * (radius - 0.01);
        float y = radius + cos(radian) * (radius - 0.01);

        if (previousX != -1) {
            float deltaX = 1.0 / std::max(std::abs(previousX - x), std::abs(previousY - y));

            for (float t = 0; t <= 1; t += deltaX) {
                float newX = previousX + t * (x - previousX);
                float newY = previousY + t * (y - previousY);

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