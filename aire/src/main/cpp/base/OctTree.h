//
// Created by Radzivon Bartoshyk on 11/02/2024.
//

#pragma once

#include <memory>
#include "Eigen/Eigen"

struct OctreeNode {
    Eigen::Vector4i color;
    OctreeNode *children[8];

    OctreeNode() {
        color = {0, 0, 0, 255};
        for (int i = 0; i < 8; ++i)
            children[i] = nullptr;
    }

    ~OctreeNode() {
        for (int i = 0; i < 8; ++i)
            delete children[i];
    }
};

class Octree {
private:
    OctreeNode *root;

    int distance(const Eigen::Vector4i &a, const Eigen::Vector4i &b) {
        int dr = a[0] - b[0];
        int dg = a[1] - b[1];
        int db = a[2] - b[2];
        int da = a[3] - b[3];
        if (a[0] < 128) {
            return 2 * dr * dr + 4 * dg * dg + 3 * db * db;
        } else {
            return 2 * dr * dr + 4 * dg * dg + 2 * db * db;
        }
    }

    int findNearestColorUtil(OctreeNode *node, const Eigen::Vector4i &target, int bestDistance, Eigen::Vector4i &nearest) {
        if (!node)
            return std::numeric_limits<int>::max();

        int dist = distance(node->color, target);
        if (dist < bestDistance) {
            bestDistance = dist;
            nearest = node->color;
        }

        if (dist == 0)
            return bestDistance;

        int index = 0;
        for (int i = 0; i < 8; ++i) {
            if (node->children[i]) {
                int childDist = distance(node->children[i]->color, target);
                if (childDist < bestDistance) {
                    bestDistance = childDist;
                    index = i;
                }
            }
        }

        return findNearestColorUtil(node->children[index], target, bestDistance, nearest);
    }

public:
    Octree() {
        root = new OctreeNode();
    }

    ~Octree() {
        delete root;
    }

    void insert(const Eigen::Vector4i &color) {
        OctreeNode *current = root;
        for (int level = 1 << 7; level; level >>= 1) {
            int bit = (color.y() & level) * 4 + (color.x() & level) * 2 + (color.z() & level);

            if (bit < 8) {
                if (!current->children[bit])
                    current->children[bit] = new OctreeNode();

                current = current->children[bit];
            }
        }
        current->color = color;
    }

    Eigen::Vector4i findNearestColor(const Eigen::Vector4i &target) {
        Eigen::Vector4i nearestColor = target;
        int bestDistance = findNearestColorUtil(root, target, std::numeric_limits<int>::max(), nearestColor);
        return nearestColor;
    }
};
