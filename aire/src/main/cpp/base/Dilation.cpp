/*
 *
 *  * MIT License
 *  *
 *  * Copyright (c) 2024 Radzivon Bartoshyk
 *  * aire [https://github.com/awxkee/aire]
 *  *
 *  * Created by Radzivon Bartoshyk on 02/02/24, 6:13 PM
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

#include "Dilation.h"
#include <vector>
#include <algorithm>
#include <thread>
#include <queue>
#include "hwy/highway.h"
#include "algo/support-inl.h"
#include "Eigen/Eigen"
#include "base/Channels.h"
#include "concurrency.hpp"
#include "ArbitraryUtil.h"

using namespace std;

namespace aire {

    using namespace hwy;
    using namespace std;
    using namespace hwy::HWY_NAMESPACE;

    // https://stackoverflow.com/questions/25034259/scipy-ndimage-morphology-operators-saturate-my-computer-x-ram-8gb/51159444#51159444

    /*************************************************************/
/* Dilation procedure		                             */
/* ATTENTION: sizeof(in) != sizeof(out) 		     */
/*************************************************************/
    int dilationImage(uint8_t *bloc, int blocWidth, int blocHeight,
                      uint8_t *out, int imageWidth, int imageHeight,
                      uint8_t *se, int bh, int bv,
                      struct front *l, struct front *r, struct front *u, struct front *d,
                      int ox, int oy) {
        uint8_t max, *corner, val;
        int i, col, *pos, lx, n;

        std::vector<uint8_t> histo(256, 0);

/* Build the first histogram */
        for (i = 0; i < bh * bv; i++)
            if (se[i] != 0)
                histo[0]++;

        max = std::numeric_limits<uint8_t>::min();

/* Each line is scanned through */
        for (int line = 0; line < imageHeight + bv; ++line)
            if (line % 2 == 0) {
                lx = line * blocWidth;
                /* From left to right */
                for (col = 0; col < imageWidth + bh; col++) {
                    corner = &bloc[col + lx];
                    /* Updates the histogram */
                    /* 1. Adding "flat" pixels */
                    pos = r->pos;
                    for (n = 0; n < r->size; n++) {
                        int pf = *pos;
                        val = *(corner + (pf + 1));
                        histo[val]++;
                        if (val > max) max = val;
                        pos++;
                    }
                    /* 3. Removing "flat" pixels */
                    pos = l->pos;
                    for (n = 0; n < l->size; n++) {
                        histo[*(corner + *pos)]--;
                        pos++;
                    }
                    /* Recomputes the maximum if necessary */
                    while (histo[max] == 0) max--;
                    /* Puts the value in the picture */
                    if ((col + 1 + ox >= bh) && (col + 1 - bh + ox < imageWidth) &&
                        (line - bv + oy >= 0) && (line - bv + oy < imageHeight))
                        out[col + 1 - bh + ox + (line - bv + oy) * imageWidth] = max;
                }
            } else {
                lx = line * blocWidth;
                /* From right to left */
                for (col = imageWidth + bh; col > 0; col--) {
                    corner = &bloc[col + lx];
                    /* Updates the histogram */
                    /* 1. Adding "flat" pixels */
                    pos = l->pos;
                    for (n = 0; n < l->size; n++) {
                        val = *(corner + ((*pos) - 1));
                        histo[val]++;
                        if (val > max) max = val;
                        pos++;
                    }
                    /* 3. Removing "flat" pixels */
                    pos = r->pos;
                    for (n = 0; n < r->size; n++) {
                        histo[*(corner + *pos)]--;
                        pos++;
                    }
                    /* Recomputes the maximum if necessary */
                    while (histo[max] == 0) max--;
                    /* Put the value in the picture */
                    if ((col - 1 + ox >= bh) && (col - 1 - bh + ox < imageWidth) &&
                        (line - bv + oy >= 0) && (line - bv + oy < imageHeight))
                        out[col - 1 - bh + ox + (line - bv + oy) * imageWidth] = max;
                }
            }

        return MORPHO_SUCCESS;
    }

    int
    dilationArbitrarySE(uint8_t *imageIn, uint8_t *imageOut, int imageWidth, int imageHeight,
                        uint8_t *se, int seWidth, int seHeight, int seHorizontalOrigin,
                        int seVerticalOrigin) {
        char st[200];

        struct front l = {0}, r = {0}, u = {0}, d = {0};
        int i, j, ret;
        int blocWidth, blocHeight;
        int se2HorizontalOrigin, se2VerticalOrigin;

        if (imageWidth <= seWidth) {
            snprintf(st, 200, "ERROR(%s): size(=%d) of the structuring elements should be larger than the image one(=%d).", "dilation_arbitrary_SE", seWidth,
                     imageWidth);
            perror(st);
            return MORPHO_ERROR;
        }

        if (imageHeight <= seHeight) {
            snprintf(st, 200, "ERROR(%s): size(=%d) of the structuring elements should be larger than the image one(=%d).", "dilation_arbitrary_SE", seHeight,
                     imageHeight);
            perror(st);
            return MORPHO_ERROR;
        }

/* First of all we invert the structuring function */
        std::vector<uint8_t> se2(seWidth * seHeight);
        invert_SE(se, se2.data(), seWidth, seHeight);
        se2HorizontalOrigin = seWidth - 1 - seHorizontalOrigin;
        se2VerticalOrigin = seHeight - 1 - seVerticalOrigin;

/* We proceed to the analysis of the structuring element
   and search for an origin */
        if (MORPHO_SUCCESS != analyse_b(se2.data(), seWidth, seHeight, &l, &r, &u, &d, se2HorizontalOrigin, se2VerticalOrigin)
                ) {
            perror("ERROR(dilation_arbitrary_SE): analyse_b did not return a valid code");
            return MORPHO_ERROR;
        }

/* Allocate a new picture with a border */
        blocWidth = imageWidth + seWidth * 2;
        blocHeight = imageHeight + seHeight * 2;
        std::vector<uint8_t> tmpImage(blocWidth * blocHeight);
        for (i = 0; i < blocWidth * blocHeight; i++) tmpImage[i] = std::numeric_limits<uint8_t>::min();
        for (j = 0; j < imageHeight; j++)
            for (i = 0; i < imageWidth; i++)
                tmpImage[i + seWidth + (j + seHeight) * blocWidth] = imageIn[i + j * imageWidth];

/* Transforms the information contained in the front structures */
        if (MORPHO_SUCCESS != transform_b(blocWidth, &l, &r, &u, &d)) {
            perror("ERROR(dilation_arbitrary_SE): transform_b did not return a valid code");
            return MORPHO_ERROR;
        }

/* Proceed to the dilation;
   ATTENTION: sizeof(im_inter->f...) != sizeof(im_out->f...) */
        ret = dilationImage(tmpImage.data(), blocWidth, blocHeight, imageOut,
                            imageWidth, imageHeight, se, seWidth, seHeight, &l, &r, &u, &d,
                            se2HorizontalOrigin, se2VerticalOrigin);

        if (MORPHO_SUCCESS != ret) {
            perror("ERROR(dilation_arbitrary_SE): dilationImage did not return a valid code");
            return MORPHO_ERROR;
        }

/* Free the data */
        free_front(&l);
        free_front(&r);
        free_front(&u);
        free_front(&d);
        return MORPHO_SUCCESS;
    }

    template<class T>
    void dilateRGBA(T *pixels, T *destination, int stride, int width, int height,
                    Eigen::MatrixXi &kernel) {
        std::vector<uint8_t> rVec(width * height);
        std::vector<uint8_t> gVec(width * height);
        std::vector<uint8_t> bVec(width * height);
        std::vector<uint8_t> aVec(width * height);

        split(pixels, rVec.data(), gVec.data(), bVec.data(), aVec.data(), stride, width, height);

        std::vector<uint8_t> rDstVec(width * height);
        std::vector<uint8_t> gDstVec(width * height);
        std::vector<uint8_t> bDstVec(width * height);
        std::vector<uint8_t> aDstVec(width * height);

        Eigen::Matrix<uint8_t, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajorBit> rowMajor(kernel.rows(), kernel.cols());
        for (int i = 0; i < kernel.rows(); ++i) {
            for (int j = 0; j < kernel.cols(); ++j) {
                rowMajor(i, j) = kernel(i, j);
            }
        }

        int cx = kernel.cols() / 2;
        int cy = kernel.rows() / 2;

        if (rowMajor(cy, cx) == 0) {
            for (int i = 0; i < kernel.rows(); ++i) {
                for (int j = 0; j < kernel.cols(); ++j) {
                    if (rowMajor(i, j) != 0) {
                        cx = j;
                        cy = i;
                        break;
                    }
                }
            }
        }

        dilationArbitrarySE(rVec.data(), rDstVec.data(), width, height,
                            rowMajor.data(), rowMajor.rows(), rowMajor.cols(),
                            cx, cy);
        dilationArbitrarySE(gVec.data(), gDstVec.data(), width, height,
                            rowMajor.data(), rowMajor.rows(), rowMajor.cols(),
                            cx, cy);
        dilationArbitrarySE(bVec.data(), bDstVec.data(), width, height,
                            rowMajor.data(), rowMajor.rows(), rowMajor.cols(),
                            cx, cy);
        dilationArbitrarySE(aVec.data(), aDstVec.data(), width, height,
                            rowMajor.data(), rowMajor.rows(), rowMajor.cols(),
                            cx, cy);

        merge(destination, rDstVec.data(), gDstVec.data(), bDstVec.data(), aDstVec.data(), stride, width, height);
    }

    template<class T>
    void dilate(T *pixels, T *destination, int width, int height, Eigen::MatrixXi &kernel) {
        Eigen::Matrix<uint8_t, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajorBit> rowMajor(kernel.rows(), kernel.cols());
        for (int i = 0; i < kernel.rows(); ++i) {
            for (int j = 0; j < kernel.cols(); ++j) {
                rowMajor(i, j) = kernel(i, j);
            }
        }

        int cx = kernel.cols() / 2;
        int cy = kernel.rows() / 2;

        if (rowMajor(cy, cx) == 0) {
            for (int i = 0; i < kernel.rows(); ++i) {
                for (int j = 0; j < kernel.cols(); ++j) {
                    if (rowMajor(i, j) != 0) {
                        cx = j;
                        cy = i;
                        break;
                    }
                }
            }
        }

        dilationArbitrarySE(pixels, destination, width, height,
                            rowMajor.data(), rowMajor.rows(), rowMajor.cols(),
                            cx, cy);
    }

    template void
    dilate(uint8_t *pixels, uint8_t *destination, int width, int height,
           Eigen::MatrixXi &kernel);

    template
    void dilateRGBA(uint8_t *pixels, uint8_t *destination, int stride, int width, int height,
                    Eigen::MatrixXi &kernel);
}