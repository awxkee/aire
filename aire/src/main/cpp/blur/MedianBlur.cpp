//
// Created by Radzivon Bartoshyk on 31/01/2024.
//

#include "MedianBlur.h"
#include <vector>
#include <thread>

using namespace std;

template<class V>
inline __attribute__((flatten))
V getMedian(const std::vector<V> &data) {
    std::vector<V> copy = data;
    std::nth_element(copy.begin(), copy.begin() + copy.size() / 2, copy.end());
    return copy[copy.size() / 2];
}

template<class V>
void medianBlurU8Runner(std::vector<V> &transient, V *data, int stride, int width,
                        int y, int radius, int height) {
    V *dst = reinterpret_cast<V *>(reinterpret_cast<uint8_t *>(transient.data()) + y * stride);
    for (int x = 0; x < width; ++x) {

        std::vector<V> store;

        for (int j = -radius; j <= radius; ++j) {
            for (int i = -radius; i <= radius; ++i) {
                V *src = reinterpret_cast<V *>(reinterpret_cast<uint8_t *>(data) +
                                               clamp(y + j, 0, height - 1) * stride);
                int pos = clamp((x + i), 0, width - 1);
                store.insert(store.end(), src[pos]);
            }
        }

        dst[0] = getMedian(store);
        dst += 1;
    }
}

template<class V>
void medianBlur(V *data, int stride, int width, int height, int radius) {
    std::vector<V> transient(stride * height);
    int threadCount = clamp(min(static_cast<int>(std::thread::hardware_concurrency()),
                                height * width / (256 * 256)), 1, 12);
    vector<thread> workers;

    int segmentHeight = height / threadCount;

    for (int i = 0; i < threadCount; i++) {
        int start = i * segmentHeight;
        int end = (i + 1) * segmentHeight;
        if (i == threadCount - 1) {
            end = height;
        }
        workers.emplace_back(
                [start, end, width, height, stride, data, radius, &transient]() {
                    for (int y = start; y < end; ++y) {
                        medianBlurU8Runner(transient, data, stride, width, y, radius, height);
                    }
                });
    }

    for (std::thread &thread: workers) {
        thread.join();
    }

    std::copy(transient.begin(), transient.end(), data);
}

template void medianBlur<uint16_t>(uint16_t *data, int stride, int width, int height, int radius);

template void medianBlur<uint32_t>(uint32_t *data, int stride, int width, int height, int radius);

template void medianBlur<uint64_t>(uint64_t *data, int stride, int width, int height, int radius);