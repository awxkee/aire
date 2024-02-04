//
// Created by Radzivon Bartoshyk on 04/02/2024.
//

#include "PoissonBlur.h"
#include "base/Convolve1D.h"
#include <vector>
#include <random>

namespace aire {
    void poissonBlur(uint8_t *data, int stride, int width, int height, int radius) {
        auto kernel = generatePoissonBlur(radius);
        convolve1D(data, stride, width, height, kernel, kernel);
    }

    std::vector<float> generatePoissonBlur(int radius) {
        int kernelSize = 2 * radius + 1;
        std::poisson_distribution<> d(radius);
        std::vector<float> kernel(kernelSize, 1.0f / kernelSize);

        std::random_device rd;
        std::mt19937 gen(rd());

        for (int i = 0; i < kernelSize; ++i) {
            kernel[i] = d(gen);
        }

        float sum = 0.f;
        for (int i = 0; i < kernelSize; ++i) {
            sum += kernel[i];
        }

        int maxIter = 0;
        while (sum == 0.f && maxIter < 50) {
            kernel = generatePoissonBlur(radius);
            maxIter++;
        }

        if (sum != 0.f) {
            for (int i = 0; i < kernelSize; ++i) {
                kernel[i] /= sum;
            }
        }

        return std::move(kernel);

    }
}
