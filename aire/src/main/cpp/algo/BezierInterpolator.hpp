//
// Created by Radzivon Bartoshyk on 01/03/2024.
//

#pragma once

#include "Eigen/Eigen"
#include <algorithm>

// https://easings.net/

namespace aire {
    class BezierInterpolator {
    public:
        BezierInterpolator(float p0, float p1, float p2, float p3) {
            start << static_cast<float>(p0), static_cast<float>(p1);
            end << p2, p3;
        }

        float getInterpolation(float time) {
            return getBezierCoordinateY(getXForTime(time));
        }

    private:
        Eigen::Vector2f start;
        Eigen::Vector2f end;

        Eigen::Vector2f a, b, c;

        float getXForTime(float time) {
            float x = time;
            float z;
            for (int i = 1; i < 14; i++) {
                z = getBezierCoordinateX(x) - time;
                if (std::abs(z) < 1e-3) {
                    break;
                }
                x -= z / getXDerivate(x);
            }
            return x;
        }

        float getBezierCoordinateY(float time) {
            c.y() = 3 * start.y();
            b.y() = 3 * (end.y() - start.y()) - c.y();
            a.y() = 1 - c.y() - b.y();
            return time * (c.y() + time * (b.y() + time * a.y()));
        }

        float getXDerivate(float t) {
            return c.x() + t * (2 * b.x() + 3 * a.x() * t);
        }

        float getBezierCoordinateX(float time) {
            c.x() = 3 * start.x();
            b.x() = 3 * (end.x() - start.x()) - c.x();
            a.x() = 1 - c.x() - b.x();
            return time * (c.x() + time * (b.x() + time * a.x()));
        }

    };

    static BezierInterpolator getEaseInExpoInterpolator() {
        return BezierInterpolator(0.7f, 0.f, 0.84f, 0.f);
    }

    static BezierInterpolator getEaseOutExpoInterpolator() {
        return BezierInterpolator(0.16f, 1.f, 0.3f, 1.f);
    }

    static BezierInterpolator getEaseInCircInterpolator() {
        return BezierInterpolator(0.55f, 0.f, 1.f, 0.45f);
    }

    static BezierInterpolator getEaseOutQuintInterpolator() {
        return BezierInterpolator(0.22f, 1.f, 0.36f, 1.f);
    }

    static BezierInterpolator getEaseOutCubicInterpolator() {
        return BezierInterpolator(0.33f, 1.f, 0.68f, 1.f);
    }

    static BezierInterpolator getEaseOutQuadInterpolator() {
        return BezierInterpolator(0.45f, 0.f, 0.55f, 1.f);
    }

    static BezierInterpolator getEaseInOutCubicInterpolator() {
        return BezierInterpolator(0.65f, 0.f, 0.35f, 1.f);
    }

    static BezierInterpolator getEaseInOutSineInterpolator() {
        return BezierInterpolator(0.61f, 1.f, 0.88f, 1.f);
    }
}