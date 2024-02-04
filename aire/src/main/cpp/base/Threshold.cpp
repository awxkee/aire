//
// Created by Radzivon Bartoshyk on 02/02/2024.
//

#include "Threshold.h"
#include "hwy/highway.h"
#include "algo/support-inl.h"

namespace aire {

    using namespace std;
    using namespace hwy;
    using namespace hwy::HWY_NAMESPACE;

    template<class V>
    void threshold(V *pixels, int width, int height, V thresholdLevel, V max, V min) {
        const ScalableTag<uint8_t> du;
        using VU = Vec<decltype(du)>;
        const int lanes = du.MaxLanes();

        const auto vThresholdLevel = Set(du, thresholdLevel);
        const auto vMax = Set(du, max);
        const auto vMin = Set(du, min);

        for (int y = 0; y < height; ++y) {
            auto data = reinterpret_cast<V *>(reinterpret_cast<uint8_t *>(pixels) + y * width);
            int x = 0;

            for (; x + lanes < width; x += lanes) {
                auto lane = LoadU(du, data);
                auto mask = lane > vThresholdLevel;
                lane = IfThenElse(mask, vMax, vMin);
                StoreU(lane, du, data);
                data += lanes;
            }
            for (; x < width; ++x) {
                if (data[0] > thresholdLevel) {
                    data[0] = max;
                } else {
                    data[0] = min;
                }
                data += 1;
            }
        }
    }

    template
    void threshold(uint8_t *pixels, int width, int height, uint8_t thresholdLevel, uint8_t max,
                   uint8_t min);
}