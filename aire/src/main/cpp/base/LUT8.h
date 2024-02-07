//
// Created by Radzivon Bartoshyk on 07/02/2024.
//

#pragma once

#include <memory>
#include <cstdint>

namespace aire {
    class LUT8 {
    public:
        LUT8(uint8_t table[256]) {
            memcpy(this->table, table, sizeof(uint8_t) * 256);
        }

        void apply(uint8_t *data, int stride, int width, int height) const;
    private:
        uint8_t table[256];
    };
}