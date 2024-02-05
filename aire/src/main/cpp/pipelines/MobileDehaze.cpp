//
// Created by Radzivon Bartoshyk on 05/02/2024.
//

#include "MobileDehaze.h"
#include <vector>
#include "Eigen/Eigen"

namespace aire {

    typedef struct _pixel {
        int i;
        int j;
        uint8_t val;

        _pixel(int _i, int _j, uint8_t _val) : i(_i), j(_j), val((uint8_t) _val) {}
    } Pixel;

    using namespace std;

    bool sortDehaze(const Pixel &a, const Pixel &b) {
        return a.val > b.val;
    }

    void getDarkChanel(const uint8_t *pSrc, std::vector<Pixel> &tmp_vec, const int stride, const int rows, const int cols, const int channels, int radius) {
        for (int i = 0; i < rows; i++) {
            for (int j = 0; j < cols; j++) {
                int rmin = std::max(0, i - radius);
                int rmax = std::min(i + radius, rows - 1);
                int cmin = std::max(0, j - radius);
                int cmax = std::min(j + radius, cols - 1);
                double min_val = 255;
                for (int x = rmin; x <= rmax; x++) {
                    for (int y = cmin; y <= cmax; y++) {
                        const uint8_t *tmp = reinterpret_cast<const uint8_t *>(reinterpret_cast<const uint8_t *>(pSrc) + stride * y);
                        uint8_t b = tmp[x * channels + 0];
                        uint8_t g = tmp[x * channels + 1];
                        uint8_t r = tmp[x * channels + 2];
                        uint8_t minpixel = b > g ? ((g > r) ? r : g) : ((b > r) ? r : b);
                        min_val = std::min((double) minpixel, min_val);
                    }
                }
                tmp_vec.push_back(Pixel(i, j, uint8_t(min_val)));
            }
        }
        std::sort(tmp_vec.begin(), tmp_vec.end(), sortDehaze);
    }

    void getAirLight(const uint8_t *pSrc, std::vector<Pixel> &tmp_vec, Eigen::Vector3f *p_Alight, const int stride, const int rows, const int cols,
                     const int channels) {
        int num = int(rows * cols * 0.001);
        double A_sum[3] = {0,};
        std::vector<Pixel>::iterator it = tmp_vec.begin();
        for (int cnt = 0; cnt < num; cnt++) {
            const uint8_t *tmp = reinterpret_cast<const uint8_t *>(reinterpret_cast<const uint8_t *>(pSrc) + stride * it->i);
            A_sum[0] += tmp[it->j * channels + 0];
            A_sum[1] += tmp[it->j * channels + 1];
            A_sum[2] += tmp[it->j * channels + 2];
            it++;
        }
        for (int i = 0; i < 3; i++) {
            (*p_Alight)[i] = A_sum[i] / num;
        }
    }

    void getTransmission(const uint8_t *pSrc, double *p_tran, Eigen::Vector3f *p_Alight, const int stride,
                        const int rows, const int cols, const int channels,
                        int radius, double omega) {
        for (int i = 0; i < rows; i++) {
            for (int j = 0; j < cols; j++) {
                int rmin = std::max(0, i - radius);
                int rmax = std::min(i + radius, rows - 1);
                int cmin = std::max(0, j - radius);
                int cmax = std::min(j + radius, cols - 1);
                double min_val = 255.0;

                double *dstTran = reinterpret_cast<double *>(reinterpret_cast<uint8_t *>(p_tran) + stride * i);

                for (int x = rmin; x <= rmax; x++) {
                    for (int y = cmin; y <= cmax; y++) {
                        const uint8_t *tmp = reinterpret_cast<const uint8_t *>(reinterpret_cast<const uint8_t *>(pSrc) + stride * y);
                        double b = (double) tmp[x*channels + 0] / (*p_Alight)[0];
                        double g = (double) tmp[x*channels + 1] / (*p_Alight)[1];
                        double r = (double) tmp[x*channels + 2] / (*p_Alight)[2];
                        double minpixel = b > g ? ((g > r) ? r : g) : ((b > r) ? r : b);
                        min_val = std::min(minpixel, min_val);
                    }
                }
                dstTran[j] = 1 - omega * min_val;
            }
        }
    }


    void dehaze(uint8_t *src, int stride, int width, int height) {
        int radius = 7;
        double omega = 0.95;
        double t0 = 0.1;
        int r = 60;
        double eps = 0.001;
        vector<Pixel> tmp_vec;
        Eigen::Vector3f pAlight;
        std::vector<double> pTran(width * height);
        getDarkChanel(src, tmp_vec, stride, width, height, 4, radius);
        getAirLight(src, tmp_vec, &pAlight, stride, width, height, 4);
        getTransmission(src, pTran.data(), &pAlight, stride, width, height, 4, radius, omega);
    }

//    void guided_filter(const cv::Mat *p_src, const cv::Mat *p_tran, cv::Mat *p_gtran, int r, double eps) {
//        *p_gtran = guidedFilter(*p_src, *p_tran, r, eps);
//    }
}