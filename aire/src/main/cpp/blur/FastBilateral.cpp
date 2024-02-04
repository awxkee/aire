//
// Created by Radzivon Bartoshyk on 04/02/2024.
//

#ifndef FAST_BILATERAL
#define FAST_BILATERAL

#include "FastBilateral.h"
#include "jni/JNIUtils.h"

namespace aire {

    template<typename PointT>
    void
    FastBilateralFilter<PointT>::applyFilter(PointT *input, int stride, int width, int height, int channel, int channels) {
        std::vector<PointT> output(stride * height);
        std::copy(reinterpret_cast<uint8_t *>(input), reinterpret_cast<uint8_t *>(input) + stride * height, reinterpret_cast<uint8_t *>(output.data()));
        float base_max = -std::numeric_limits<float>::max(),
                base_min = std::numeric_limits<float>::max();
        bool found_finite = false;
        for (const auto &pt: output) {
            if (std::isfinite(pt)) {
                base_max = std::max<float>(pt, base_max);
                base_min = std::min<float>(pt, base_min);
                found_finite = true;
            }
        }
        if (!found_finite) {
            return;
        }

        for (auto &pt: output) {
            if (!std::isfinite(pt)) {
                pt = base_max;
            }
        }

        const float base_delta = base_max - base_min;

        const std::size_t padding_xy = 2;
        const std::size_t padding_z = 2;

        const std::size_t small_width = static_cast<std::size_t> (static_cast<float> (width - 1) / sigma_s_) + 1 + 2 * padding_xy;
        const std::size_t small_height = static_cast<std::size_t> (static_cast<float> (height - 1) / sigma_s_) + 1 + 2 * padding_xy;
        const std::size_t small_depth = static_cast<std::size_t> (base_delta / sigma_r_) + 1 + 2 * padding_z;

        Array3D data(small_width, small_height, small_depth);
        for (std::size_t x = 0; x < width; ++x) {
            const std::size_t small_x = static_cast<std::size_t> (static_cast<float> (x) / sigma_s_ + 0.5f) + padding_xy;
            for (std::size_t y = 0; y < height; ++y) {
                const float z = reinterpret_cast<PointT *>(reinterpret_cast<uint8_t *>(output.data()) + y * stride)[x*channels + channel] - base_min;

                const std::size_t small_y = static_cast<std::size_t> (static_cast<float> (y) / sigma_s_ + 0.5f) + padding_xy;
                const std::size_t small_z = static_cast<std::size_t> (static_cast<float> (z) / sigma_r_ + 0.5f) + padding_z;

                Eigen::Vector2f &d = data(small_x, small_y, small_z);
                d[0] += reinterpret_cast<PointT *>(reinterpret_cast<uint8_t *>(output.data()) + y * stride)[x*channels + channel];
                d[1] += 1.0f;
            }
        }

        std::vector<long int> offset(3);
        offset[0] = &(data(1, 0, 0)) - &(data(0, 0, 0));
        offset[1] = &(data(0, 1, 0)) - &(data(0, 0, 0));
        offset[2] = &(data(0, 0, 1)) - &(data(0, 0, 0));

        Array3D buffer(small_width, small_height, small_depth);

        for (std::size_t dim = 0; dim < 3; ++dim) {
            const long int off = offset[dim];
            for (std::size_t n_iter = 0; n_iter < 2; ++n_iter) {
                std::swap(buffer, data);
                for (std::size_t x = 1; x < small_width - 1; ++x)
                    for (std::size_t y = 1; y < small_height - 1; ++y) {
                        Eigen::Vector2f *d_ptr = &(data(x, y, 1));
                        Eigen::Vector2f *b_ptr = &(buffer(x, y, 1));

                        for (std::size_t z = 1; z < small_depth - 1; ++z, ++d_ptr, ++b_ptr)
                            *d_ptr = (*(b_ptr - off) + *(b_ptr + off) + 2.0 * (*b_ptr)) / 4.0;
                    }
            }
        }

        if (early_division_) {
            for (auto d = data.begin(); d != data.end(); ++d)
                *d /= ((*d)[0] != 0) ? (*d)[1] : 1;

            for (std::size_t x = 0; x < width; x++)
                for (std::size_t y = 0; y < height; y++) {
                    const float z = reinterpret_cast<PointT *>(reinterpret_cast<uint8_t *>(output.data()) + y * stride)[x*channels + channel] - base_min;
                    const Eigen::Vector2f D = data.trilinear_interpolation(static_cast<float> (x) / sigma_s_ + padding_xy,
                                                                           static_cast<float> (y) / sigma_s_ + padding_xy,
                                                                           z / sigma_r_ + padding_z);
                    reinterpret_cast<PointT *>(reinterpret_cast<uint8_t *>(output.data()) + y * stride)[x*channels + channel] = D[0];
                }
        } else {
            for (std::size_t x = 0; x < width; ++x)
                for (std::size_t y = 0; y < height; ++y) {
                    const float z = output[y * stride + x] - base_min;
                    const Eigen::Vector2f D = data.trilinear_interpolation(static_cast<float> (x) / sigma_s_ + padding_xy,
                                                                           static_cast<float> (y) / sigma_s_ + padding_xy,
                                                                           z / sigma_r_ + padding_z);
                    reinterpret_cast<PointT *>(reinterpret_cast<uint8_t *>(output.data()) + y * stride)[x*channels + channel] = D[0] / D[1];
                }
        }
        std::copy(output.begin(), output.end(), input);
    }


    template<typename PointT>
    std::size_t
    FastBilateralFilter<PointT>::Array3D::clamp(const std::size_t min_value,
                                                const std::size_t max_value,
                                                const std::size_t x) {
        if (x >= min_value && x <= max_value) {
            return x;
        }
        if (x < min_value) {
            return (min_value);
        }
        return (max_value);
    }


    template<typename PointT>
    Eigen::Vector2f
    FastBilateralFilter<PointT>::Array3D::trilinear_interpolation(const float x,
                                                                  const float y,
                                                                  const float z) {
        const std::size_t x_index = clamp(0, x_dim_ - 1, static_cast<std::size_t> (x));
        const std::size_t xx_index = clamp(0, x_dim_ - 1, x_index + 1);

        const std::size_t y_index = clamp(0, y_dim_ - 1, static_cast<std::size_t> (y));
        const std::size_t yy_index = clamp(0, y_dim_ - 1, y_index + 1);

        const std::size_t z_index = clamp(0, z_dim_ - 1, static_cast<std::size_t> (z));
        const std::size_t zz_index = clamp(0, z_dim_ - 1, z_index + 1);

        const float x_alpha = x - static_cast<float> (x_index);
        const float y_alpha = y - static_cast<float> (y_index);
        const float z_alpha = z - static_cast<float> (z_index);

        return
                (1.0f - x_alpha) * (1.0f - y_alpha) * (1.0f - z_alpha) * (*this)(x_index, y_index, z_index) +
                x_alpha * (1.0f - y_alpha) * (1.0f - z_alpha) * (*this)(xx_index, y_index, z_index) +
                (1.0f - x_alpha) * y_alpha * (1.0f - z_alpha) * (*this)(x_index, yy_index, z_index) +
                x_alpha * y_alpha * (1.0f - z_alpha) * (*this)(xx_index, yy_index, z_index) +
                (1.0f - x_alpha) * (1.0f - y_alpha) * z_alpha * (*this)(x_index, y_index, zz_index) +
                x_alpha * (1.0f - y_alpha) * z_alpha * (*this)(xx_index, y_index, zz_index) +
                (1.0f - x_alpha) * y_alpha * z_alpha * (*this)(x_index, yy_index, zz_index) +
                x_alpha * y_alpha * z_alpha * (*this)(xx_index, yy_index, zz_index);
    }

    template
    class FastBilateralFilter<uint8_t>;

    template
    class FastBilateralFilter<float>;
} // namespace pcl

#endif /* FAST_BILATERAL */
