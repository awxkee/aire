#ifndef _COVER_TREE_POINT_H
#define _COVER_TREE_POINT_H

#include <vector>
#include "Eigen/Eigen"

/**
 * A simple point class containing a vector of doubles and a single char name.
 */
class CoverTreePoint {
private:
    Eigen::Vector4i mColor;
public:
    CoverTreePoint(Eigen::Vector4i v) : mColor(v) {}

    double distance(const CoverTreePoint &p) const {
        Eigen::Vector4i diff = mColor - p.mColor;
        int mask = (mColor[0] < 128) ? 3 : 2;
        Eigen::Array4i weights;
        weights << 2, 4, mask, 1;
        Eigen::Array4i squaredDiff = diff.array().square() * weights;
        return squaredDiff.sum();
    }

    Eigen::Vector4i getColor() {
        return mColor;
    }

    void print() const {

    }

    bool operator==(const CoverTreePoint &p) const {
        return p.mColor == mColor;
    }
};

#endif // _COVER_TREE_POINT_H

