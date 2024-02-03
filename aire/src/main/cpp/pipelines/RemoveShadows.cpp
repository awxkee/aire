//
// Created by Radzivon Bartoshyk on 02/02/2024.
//

#include "RemoveShadows.h"
#include "base/Grayscale.h"
#include "base/Threshold.h"
#include "base/Channels.h"
#include "base/Dilation.h"
#include "blur/MedianBlur.h"
#include <algorithm>
#include "base/Arithmetics.h"
#include <thread>
#include "MathUtils.hpp"

using namespace std;

namespace aire {

    void removeProcessChannel(uint8_t *src, int width, int height, int kernelSize) {
        std::vector<uint8_t> chan(width * height);
        auto kernel = getStructuringKernel(kernelSize);
        dilate(src, chan.data(), width, height, kernel);
        medianBlurChannel(chan.data(), width, height, 5, MEDIAN_WIRTH);

        std::vector<uint8_t> dstDiff(width * height);
        absDiff(dstDiff.data(), chan.data(), src, width, height);
        diff(chan.data(), 255, dstDiff.data(), width, height);
        normalize(reinterpret_cast<uint8_t *>(chan.data()), width, height, uint8_t (0), uint8_t(255));
        std::copy(chan.begin(), chan.end(), src);
    }

    void removeShadows(uint8_t *src, int stride, int width, int height, int kernelSize) {
        std::vector<uint8_t> rChan(width * height);
        std::vector<uint8_t> gChan(width * height);
        std::vector<uint8_t> bChan(width * height);
        std::vector<uint8_t> aChan(width * height);

        split(src, rChan.data(), gChan.data(), bChan.data(), aChan.data(),
              stride, width, height);

        vector<thread> workers;

        workers.emplace_back([&rChan, width, height, kernelSize]() {
            removeProcessChannel(rChan.data(), width, height, kernelSize);
        });
        workers.emplace_back([&gChan, width, height, kernelSize]() {
            removeProcessChannel(gChan.data(), width, height, kernelSize);
        });
        workers.emplace_back([&bChan, width, height, kernelSize]() {
            removeProcessChannel(bChan.data(), width, height, kernelSize);
        });

        for (std::thread &thread: workers) {
            thread.join();
        }
        merge(src, rChan.data(), gChan.data(), bChan.data(), aChan.data(), stride, width, height);
    }
}