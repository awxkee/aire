//
// Created by Radzivon Bartoshyk on 03/02/2024.
//

#include "CrystallizeEffect.h"
#include "algo/jc_voronoi.h"
#include <memory>
#include <vector>
#include <random>
#include <algorithm>
#include "algo/MathUtils.hpp"
#include "jni/JNIUtils.h"
#include "color/Blend.h"

namespace aire {

    static inline jcv_point
    remap(const jcv_point *pt, const jcv_point *min, const jcv_point *max, const jcv_point *scale) {
        jcv_point p;
        p.x = (pt->x - min->x) / (max->x - min->x) * scale->x;
        p.y = (pt->y - min->y) / (max->y - min->y) * scale->y;
        return p;
    }

    void
    drawLine(uint8_t *bitmap, int color, int stride, int width, int height, int x1, int y1, int x2,
             int y2) {
        int dx = x2 - x1;
        int dy = y2 - y1;
        int steps = (abs(dx) > abs(dy)) ? abs(dx) : abs(dy);

        float xIncrement = static_cast<float>(dx) / static_cast<float>(steps);
        float yIncrement = static_cast<float>(dy) / static_cast<float>(steps);

        float x = static_cast<float>(x1);
        float y = static_cast<float>(y1);

        uint8_t a1 = color >> 24 & 0xff;
        uint8_t r1 = color >> 16 & 0xff;
        uint8_t g1 = color >> 8 & 0xff;
        uint8_t b1 = color & 0xff;

        for (int i = 0; i <= steps; ++i) {
            if (x >= 0 && x < width && y >= 0 && y < height) {
                uint8_t *src = reinterpret_cast<uint8_t *>((reinterpret_cast<uint8_t *>(bitmap) +
                                                            static_cast<int>(y) * stride));
                int pos = static_cast<int>(x);
                if (pos < 0 || pos >= width) { continue;}
                pos *= 4;
                uint8_t a = src[static_cast<int>(x * 4 + 3)];
                uint8_t b = src[static_cast<int>(x * 4 + 2)];
                uint8_t g = src[static_cast<int>(x * 4 + 1)];
                uint8_t r = src[static_cast<int>(x * 4)];
                uint8_t resultingAlpha = a1 + (a * (255 - a1) + 127) / 255;
                uint8_t newB = blendColor(b1, b, a1);
                uint8_t newR = blendColor(r1, r, a1);
                uint8_t newG = blendColor(g1, g, a1);
                src[static_cast<int>(x * 4)] = newR;
                src[static_cast<int>(x * 4 + 1)] = newG;
                src[static_cast<int>(x * 4 + 2)] = newB;
                src[static_cast<int>(x * 4 + 3)] = resultingAlpha;
            }

            x += xIncrement;
            y += yIncrement;
        }
    }

    void crystallize(uint8_t *data, int stride, int width, int height, int numClusters,
                     int strokeColor) {
        if (numClusters <= 0) {
            std::string message("Num of clusters must be more than 0, but received " +
                                std::to_string(numClusters));
            throw AireError(message);
        }
        std::vector<uint8_t> transient(stride * height);
        jcv_diagram diagram;
        memset(&diagram, 0, sizeof(jcv_diagram));
        std::vector<jcv_point> jcvPoints(numClusters);

        std::default_random_engine generator;
        generator.seed(std::chrono::system_clock::now().time_since_epoch().count());
        std::uniform_int_distribution<> widthGenerator(1, width - 1);
        std::uniform_int_distribution<> heightGenerator(1, height - 1);

        for (int i = 0; i < numClusters; ++i) {
            int x = widthGenerator(generator);
            int y = heightGenerator(generator);
            jcvPoints[i] = {
                    .x = static_cast<float>(x),
                    .y = static_cast<float>(y),
            };
        }

        jcv_diagram_generate(numClusters, jcvPoints.data(), 0, 0, &diagram);

        jcv_point dimensions;
        dimensions.x = (jcv_real) width;
        dimensions.y = (jcv_real) height;

        const jcv_site *sites = jcv_diagram_get_sites(&diagram);
        for (int i = 0; i < diagram.numsites; ++i) {
            const jcv_site *site = &sites[i];

            jcv_point v0 = remap(&site->p, &diagram.min, &diagram.max, &dimensions);

            const jcv_graphedge *e = site->edges;
            while (e) {
                jcv_point v1 = remap(&e->pos[0], &diagram.min, &diagram.max, &dimensions);
                jcv_point v2 = remap(&e->pos[1], &diagram.min, &diagram.max, &dimensions);

                int minX = min3((int) v0.x, (int) v1.x, (int) v2.x);
                int minY = min3((int) v0.y, (int) v1.y, (int) v2.y);
                int maxX = max3((int) v0.x, (int) v1.x, (int) v2.x);
                int maxY = max3((int) v0.y, (int) v1.y, (int) v2.y);

                minX = clamp(minX, 0, width - 1);
                minY = clamp(minY, 0, height - 1);
                maxX = clamp(maxX, 0, width - 1);
                maxY = clamp(maxY, 0, height - 1);

                v0 = {
                        .x = std::clamp(static_cast<float>(ceil(v0.x)), 0.f,
                                        static_cast<float>(width - 1)),
                        .y = std::clamp(ceil(v0.y), 0.f, static_cast<float>(height - 1))
                };

                uint32_t color = reinterpret_cast<uint32_t *>(reinterpret_cast<uint8_t *>(data) +
                                                              static_cast<int>(ceil(v0.y)) *
                                                              stride)[static_cast<int>(ceil(v0.x))];

                jcv_point p;
                for (p.y = (jcv_real) minY; p.y <= (jcv_real) maxY; p.y++) {
                    for (p.x = (jcv_real) minX; p.x <= (jcv_real) maxX; p.x++) {
                        if (p.x >= 0 && p.x < width && p.y >= 0 && p.y < height) {
                            reinterpret_cast<uint32_t *>(
                                    reinterpret_cast<uint8_t *>(transient.data()) +
                                    static_cast<int>(p.y) *
                                    stride)[static_cast<int>(p.x)] = color;
                        }
                    }
                }

                drawLine(transient.data(),
                         strokeColor,
                         stride, width, height,
                         minX,
                         minY,
                         minX,
                         maxY);

                drawLine(transient.data(),
                         strokeColor,
                         stride, width, height,
                         minX,
                         maxY,
                         maxX,
                         maxY);

                drawLine(transient.data(),
                         strokeColor,
                         stride, width, height,
                         maxX,
                         maxY,
                         maxX,
                         minY);

                drawLine(transient.data(),
                         strokeColor,
                         stride, width, height,
                         maxX,
                         minY,
                         minX,
                         minY);

                e = e->next;
            }
        }

        jcv_diagram_free(&diagram);
        std::copy(transient.begin(), transient.end(), data);
    }
}