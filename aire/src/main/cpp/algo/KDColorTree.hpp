/*
 *
 *  * MIT License
 *  *
 *  * Copyright (c) 2024 Radzivon Bartoshyk
 *  * aire [https://github.com/awxkee/aire]
 *  *
 *  * Created by Radzivon Bartoshyk on 11/02/24, 6:13 PM
 *  *
 *  * Permission is hereby granted, free of charge, to any person obtaining a copy
 *  * of this software and associated documentation files (the "Software"), to deal
 *  * in the Software without restriction, including without limitation the rights
 *  * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 *  * copies of the Software, and to permit persons to whom the Software is
 *  * furnished to do so, subject to the following conditions:
 *  *
 *  * The above copyright notice and this permission notice shall be included in all
 *  * copies or substantial portions of the Software.
 *  *
 *  * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *  * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *  * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 *  * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *  * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 *  * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 *  * SOFTWARE.
 *  *
 *
 */

#pragma once

#include <vector>
#include <cmath>
#include <limits>
#include "EigenUtils.h"
#include "unordered_map.hpp"

namespace aire {

    struct Node {
        Eigen::Vector4i color;
        Node *left;
        Node *right;
        Node *parent;

        Node(const Eigen::Vector4i &c) : color(c), left(nullptr), right(nullptr), parent(nullptr) {}
    };

    class KDColorTree {
    private:
        Node *root;
        const int dimensions = 3;
        const int tolerance = 54;
        ska::unordered_map<uint32_t, Eigen::Vector4i> lut;

        float distance(const Eigen::Vector4i &a, const Eigen::Vector4i &b) {
            Eigen::Vector4i diff = a - b;
            int mask = (a[0] < 128) ? 3 : 2;
            Eigen::Array4i weights;
            weights << 2, 4, mask, 1;
            Eigen::Array4i squaredDiff = diff.array().square() * weights;
            return squaredDiff.sum();
        }

        Eigen::Vector4i determineNearestColor(Node *root,
                                              const Eigen::Vector4i &target,
                                              const int depthIndex,
                                              Eigen::Vector4i &bestMatch,
                                              float& bestDistance) {
            if (!root) {
                return {0, 0, 0, 255};
            }

            int depth = depthIndex % dimensions;

            float newDistance = distance(target, root->color);
            if (bestDistance > newDistance) {
                bestDistance = newDistance;
                bestMatch = root->color;
            }

            if (target[depth] < root->color[depth]) {
                // Firstly visit left leaf
                if (root->left) {
                    bestMatch = determineNearestColor(root->left, target, depthIndex + 1, bestMatch, bestDistance);
                    if (target[depth] + tolerance >= root->color[depth] && root->right) {
                        bestMatch = determineNearestColor(root->right, target, depthIndex + 1, bestMatch, bestDistance);
                    }
                }
            } else {
                if (root->right) {
                    bestMatch = determineNearestColor(root->right, target, depthIndex + 1, bestMatch, bestDistance);
                    if (target[depth] - tolerance <= root->color[depth] && root->left) {
                        bestMatch = determineNearestColor(root->left, target, depthIndex + 1, bestMatch, bestDistance);
                    }
                }
            }

            return bestMatch;
        }

        Node *buildTree(Node *parent, const std::vector<Eigen::Vector4i> &clr, const int depthIndex) {
            if (clr.empty()) {
                return nullptr;
            }
            int depth = depthIndex % dimensions;

            std::vector<Eigen::Vector4i> sub = clr;

            std::sort(sub.begin(), sub.end(),
                      [depth](const Eigen::Vector4i &a,
                              const Eigen::Vector4i &b) {
                          return a[depth] < b[depth];
                      });

            // Find median
            size_t medianIndex = sub.size() / 2;
            Node * newNode = reinterpret_cast<Node *>(malloc(sizeof(Node)));
            newNode->parent = parent;
            newNode->color = clr[clamp(medianIndex, 0, clr.size() - 1)];

            std::vector<Eigen::Vector4i> vecLeft(sub.begin(), sub.begin() + medianIndex);

            newNode->left = buildTree(parent, vecLeft, depth + 1);

            std::vector<Eigen::Vector4i> vecRight(sub.begin() + medianIndex + 1, sub.end());
            newNode->right = buildTree(parent, vecRight, depth + 1);
            return newNode;
        }

    public:
        KDColorTree() : root(nullptr) {}

        ~KDColorTree() {
            clear(root);
        }

        Eigen::Vector4i nearestColor(const Eigen::Vector4i &target) {
            if (!root) {
                return {0, 0, 0, 0};
            }

            auto packed = packRGBA(target);
            auto it = lut.find(packed);
            if (it != lut.end()) {
                return it->second;
            }

            Eigen::Vector4i bestColor = root->color;
            float bestDistance = std::numeric_limits<float>::max();
            auto bestMatch = determineNearestColor(this->root, target, 0, bestColor, bestDistance);
            lut[packed] = bestMatch;
            return bestMatch;
        }

        void createTree(const std::vector<Eigen::Vector4i> &clr) {
            root = buildTree(nullptr, clr, 0);
        }

        void clear(Node *node) {
            if (node == nullptr)
                return;
            clear(node->left);
            clear(node->right);
            free(node);
        }
    };
}