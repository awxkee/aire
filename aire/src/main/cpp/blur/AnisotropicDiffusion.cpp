//
// Created by Radzivon Bartoshyk on 03/02/2024.
//

#include "AnisotropicDiffusion.h"
#include <vector>
#include "hwy/highway.h"
#include "algo/support-inl.h"

using namespace std;
using namespace hwy;
using namespace hwy::HWY_NAMESPACE;

namespace aire {

    inline float PerconaMalik(float I, float conduction) {
        float K = I / conduction;
        float diffusionTerm = 1.0 / (1.0 + K * K);
        return diffusionTerm;
    }

    void anisotropicDiffusion(uint8_t *data, int stride, int width, int height, float diffusion,
                              float conduction, int noOfTimeSteps) {
        std::vector<uint8_t> transient(stride * height);
        std::copy(data, data + stride * height, transient.begin());
        for (int iteration = 0; iteration < noOfTimeSteps; ++iteration) {
#pragma omp parallel for num_threads(6) schedule(dynamic)
            for (int y = 0; y < height; ++y) {
                auto src = reinterpret_cast<uint8_t *>(
                        reinterpret_cast<uint8_t *>(transient.data()) +
                        y * stride);
                for (int x = 0; x < width; ++x) {
                    int nextX = clamp(x + 1, 0, width - 1);
                    int nextY = clamp(y + 1, 0, height - 1);
                    int previousX = clamp(x - 1, 0, width - 1);
                    int previousY = clamp(y - 1, 0, height - 1);
                    for (int channel = 0; channel < 3; ++channel) {
                        float local = src[x * 4 + channel];

                        float leftTopMagnitude = reinterpret_cast<uint8_t *>(
                                reinterpret_cast<uint8_t *>(transient.data()) +
                                        previousY * stride)[previousX*4 + channel] - local;

                        float rightTopMagnitude = reinterpret_cast<uint8_t *>(
                                                         reinterpret_cast<uint8_t *>(transient.data()) +
                                                         previousY * stride)[nextX*4 + channel] - local;

                        float leftBottomMagnitude = reinterpret_cast<uint8_t *>(
                                                         reinterpret_cast<uint8_t *>(transient.data()) +
                                                         nextY * stride)[previousX*4 + channel] - local;

                        float rightBottomMagnitude = reinterpret_cast<uint8_t *>(
                                                            reinterpret_cast<uint8_t *>(transient.data()) +
                                                            nextY * stride)[nextX*4 + channel] - local;

                        float currentPixel = src[x * 4 + channel];
                        float param = diffusion * (PerconaMalik(leftTopMagnitude, conduction) * leftTopMagnitude +
                                PerconaMalik(rightTopMagnitude, conduction) * rightTopMagnitude +
                                PerconaMalik(leftBottomMagnitude, conduction) * leftBottomMagnitude +
                                PerconaMalik(rightBottomMagnitude, conduction) * rightBottomMagnitude);
                        float newValue = currentPixel * param;
                        float updatedValue = currentPixel + newValue;
                        src[x * 4 + channel] = clamp(ceil(updatedValue), 0.f, 255.f);
                    }
                }
            }
        }

        std::copy(transient.begin(), transient.end(), data);
    }
}