//
// Created by Radzivon Bartoshyk on 04/02/2024.
//

#pragma once

#include "hwy/highway.h"

namespace aire {
    using namespace hwy;
    using namespace hwy::HWY_NAMESPACE;

    template<typename D>
    class ToneMapper {
    public:
        using V = Vec<D>;

        virtual void Execute(V &R, V &G, V &B) = 0;

        virtual void Execute(TFromD <D> &r, TFromD <D> &g, TFromD <D> &b) = 0;

        virtual ~ToneMapper() {

        }
    };
}