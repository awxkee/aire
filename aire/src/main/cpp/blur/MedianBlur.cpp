//
// Created by Radzivon Bartoshyk on 31/01/2024.
//

#include "MedianBlur.h"
#include <vector>
#include <thread>
#include "algo/median/QuickSelect.h"
#include "algo/median/Wirth.h"
#include "jni/JNIUtils.h"
#include "concurrency.hpp"

using namespace std;

namespace aire {

    struct MedianRGBHistogram {
        int r[256], g[256], b[256], a[256];
        int n;
    };

    struct MedianHistogram {
        int c[256];
        int n;
    };

    inline __attribute__((flatten))
    uint32_t getMedian(const std::vector<uint32_t> &data) {
        std::vector<uint32_t> copy = data;
        std::nth_element(copy.begin(), copy.begin() + copy.size() / 2, copy.end());
        return copy[copy.size() / 2];
    }

    inline __attribute__((flatten))
    uint8_t getMedian(const std::vector<uint8_t> &data) {
        std::vector<uint8_t> copy = data;
        std::nth_element(copy.begin(), copy.begin() + copy.size() / 2, copy.end());
        return copy[copy.size() / 2];
    }

    void addRGBPixels(uint8_t *data, const int stride, const int y, const int x,
                      const int width, const int height,
                      const int size, MedianRGBHistogram *__restrict__ h) {
        int i;

        if (x < 0 || x >= width) return;
        const int px = x * 4;
        for (i = y - size; i <= y + size && i < height; i++) {
            if (i < 0) continue;
            auto dataRow = reinterpret_cast<uint8_t *>(data) + i * stride;
            h->r[dataRow[px]]++;
            h->g[dataRow[px + 1]]++;
            h->b[dataRow[px + 2]]++;
            h->a[dataRow[px + 3]]++;
            h->n++;
        }
    }

    void delRGBPixels(uint8_t *data, const int stride, const int y, const int x,
                      const int width, const int height,
                      const int size, MedianRGBHistogram *__restrict__ h) {
        int i;

        if (x < 0 || x >= width) return;
        const int px = x * 4;
        for (i = y - size; i <= y + size && i < height; i++) {
            if (i < 0) continue;
            auto dataRow = reinterpret_cast<uint8_t *>(data) + i * stride;
            h->r[dataRow[px]]--;
            h->g[dataRow[px + 1]]--;
            h->b[dataRow[px + 2]]--;
            h->a[dataRow[px + 3]]--;
            h->n--;
        }
    }

    void addPixels(uint8_t *data, const int stride, const int y, const int x,
                      const int width, const int height,
                      const int size, MedianHistogram *__restrict__ h) {
        int i;

        if (x < 0 || x >= width) return;
        const int px = x;
        for (i = y - size; i <= y + size && i < height; i++) {
            if (i < 0) continue;
            auto dataRow = reinterpret_cast<uint8_t *>(data) + i * stride;
            h->c[dataRow[px]]++;
            h->n++;
        }
    }

    void delPixels(uint8_t *data, const int stride, const int y, const int x,
                      const int width, const int height,
                      const int size, MedianHistogram *__restrict__ h) {
        int i;

        if (x < 0 || x >= width) return;
        const int px = x;
        for (i = y - size; i <= y + size && i < height; i++) {
            if (i < 0) continue;
            auto dataRow = reinterpret_cast<uint8_t *>(data) + i * stride;
            h->c[dataRow[px]]--;
            h->n--;
        }
    }

    void initHistogram(uint8_t *data, const int stride, const int y,
                          const int width, const int height,
                          const int size, MedianHistogram *__restrict__ h) {
        int j;

        memset(h, 0, sizeof(MedianHistogram));

        for (j = 0; j < size && j < width; j++)
            addPixels(data, stride, y, j, width, height, size, h);
    }

    void initRGBHistogram(uint8_t *data, const int stride, const int y,
                          const int width, const int height,
                          const int size, MedianRGBHistogram *__restrict__ h) {
        int j;

        memset(h, 0, sizeof(MedianRGBHistogram));

        for (j = 0; j < size && j < width; j++)
            addRGBPixels(data, stride, y, j, width, height, size, h);
    }

    int medianFilter(const int *x, int n)
    {
        int i;
        for (n /= 2, i = 0; i < 256 && (n -= x[i]) > 0; i++);
        return i;
    }

    void medianBlurChannel(uint8_t *data, const int width, const int height, const int size) {
        std::vector<uint8_t> transient(width * height);

        MedianHistogram histogram;

        for (int y = 0; y < height; ++y) {
            uint8_t *src = reinterpret_cast<uint8_t *>(reinterpret_cast<uint8_t *>(data) + y * width);
            uint8_t *dst = reinterpret_cast<uint8_t *>(reinterpret_cast<uint8_t *>(transient.data()) + y * width);
            for (int x = 0; x < width; ++x) {
                if (x == 0) {
                    initHistogram(data, width, y, width, height, size, &histogram);
                } else {
                    delPixels(data, width, y, x - size, width, height, size, &histogram);
                    addPixels(data, width, y, x + size, width, height, size, &histogram);
                }
                if (histogram.n > 0) {
                    uint8_t c = 0;
                    c = medianFilter(histogram.c, histogram.n);
                    dst[0] = c;
                } else {
                    dst[0] = src[0];
                }
                src += 1;
                dst += 1;
            }
        }

        std::copy(transient.begin(), transient.end(), data);
    }

    void
    medianBlur(uint8_t *data, const int stride, const int width, const int height, const int size) {
        std::vector<uint8_t> transient(stride * height);

        MedianRGBHistogram histogram;

        for (int y = 0; y < height; ++y) {
            uint8_t *src = reinterpret_cast<uint8_t *>(reinterpret_cast<uint8_t *>(data) + y * stride);
            uint8_t *dst = reinterpret_cast<uint8_t *>(reinterpret_cast<uint8_t *>(transient.data()) + y * stride);
            for (int x = 0; x < width; ++x) {
                if (x == 0) {
                    initRGBHistogram(data, stride, y, width, height, size, &histogram);
                } else {
                    delRGBPixels(data, stride, y, x - size, width, height, size, &histogram);
                    addRGBPixels(data, stride, y, x + size, width, height, size, &histogram);
                }
                if (histogram.n > 0) {
                    uint8_t r = 0, g = 0, b = 0, a = 0;
                    r = medianFilter(histogram.r, histogram.n);
                    g = medianFilter(histogram.g, histogram.n);
                    b = medianFilter(histogram.b, histogram.n);
                    a = medianFilter(histogram.a, histogram.n);
                    dst[0] = r;
                    dst[1] = g;
                    dst[2] = b;
                    dst[3] = a;
                } else {
                    dst[0] = src[0];
                    dst[1] = src[1];
                    dst[2] = src[2];
                    dst[3] = src[3];
                }
                src += 4;
                dst += 4;
            }
        }

        std::copy(transient.begin(), transient.end(), data);
    }
}