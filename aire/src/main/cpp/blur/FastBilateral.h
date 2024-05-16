/*
 *
 *  * MIT License
 *  *
 *  * Copyright (c) 2024 Radzivon Bartoshyk
 *  * aire [https://github.com/awxkee/aire]
 *  *
 *  * Created by Radzivon Bartoshyk on 04/02/24, 6:13 PM
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

#include "Eigen/Dense"
#include <memory>
#include <vector>

using namespace std;

namespace aire {
    template<typename PointT>
    class FastBilateralFilter {
    protected:
    public:

        using Ptr = shared_ptr<FastBilateralFilter<PointT> >;
        using ConstPtr = shared_ptr<const FastBilateralFilter<PointT> >;

        /** \brief Empty constructor. */
        FastBilateralFilter() = default;

        /** \brief Empty destructor */
        ~FastBilateralFilter() {

        }

        /** \brief Set the standard deviation of the Gaussian used by the bilateral filter for
          * the spatial neighborhood/window.
          * \param[in] sigma_s the size of the Gaussian bilateral filter window to use
          */
        inline void
        setSigmaS(float sigma_s) { sigma_s_ = sigma_s; }

        /** \brief Get the size of the Gaussian bilateral filter window as set by the user. */
        inline float
        getSigmaS() const { return sigma_s_; }


        /** \brief Set the standard deviation of the Gaussian used to control how much an adjacent
          * pixel is downweighted because of the intensity difference (depth in our case).
          * \param[in] sigma_r the standard deviation of the Gaussian for the intensity difference
          */
        inline void
        setSigmaR(float sigma_r) { sigma_r_ = sigma_r; }

        /** \brief Get the standard deviation of the Gaussian for the intensity difference */
        inline float
        getSigmaR() const { return sigma_r_; }

        /** \brief Filter the input data and store the results into output.
          * \param[out] output the resultant point cloud
          */
        void
        applyFilter(PointT* data, int stride, int width, int height, int channel, int channels);

    protected:
        float sigma_s_{15.0f};
        float sigma_r_{0.05f};
        bool early_division_{false};

        class Array3D {
        public:
            Array3D(const std::size_t width, const std::size_t height, const std::size_t depth)
                    : v_({(width * height * depth), Eigen::Vector2f(0.0f, 0.0f),
                          Eigen::aligned_allocator<Eigen::Vector2f>()}) {
                x_dim_ = width;
                y_dim_ = height;
                z_dim_ = depth;
            }

            inline Eigen::Vector2f &
            operator()(const std::size_t x, const std::size_t y, const std::size_t z) {
                return v_[(x * y_dim_ + y) * z_dim_ + z];
            }

            inline const Eigen::Vector2f &
            operator()(const std::size_t x, const std::size_t y,
                       const std::size_t z) const { return v_[(x * y_dim_ + y) * z_dim_ + z]; }

            inline void
            resize(const std::size_t width, const std::size_t height, const std::size_t depth) {
                x_dim_ = width;
                y_dim_ = height;
                z_dim_ = depth;
                v_.resize(x_dim_ * y_dim_ * z_dim_);
            }

            Eigen::Vector2f
            trilinear_interpolation(const float x,
                                    const float y,
                                    const float z);

            static inline std::size_t
            clamp(const std::size_t min_value,
                  const std::size_t max_value,
                  const std::size_t x);

            inline std::size_t
            x_size() const { return x_dim_; }

            inline std::size_t
            y_size() const { return y_dim_; }

            inline std::size_t
            z_size() const { return z_dim_; }

            inline std::vector<Eigen::Vector2f, Eigen::aligned_allocator<Eigen::Vector2f> >::iterator
            begin() { return v_.begin(); }

            inline std::vector<Eigen::Vector2f, Eigen::aligned_allocator<Eigen::Vector2f> >::iterator
            end() { return v_.end(); }

            inline std::vector<Eigen::Vector2f, Eigen::aligned_allocator<Eigen::Vector2f> >::const_iterator
            begin() const { return v_.begin(); }

            inline std::vector<Eigen::Vector2f, Eigen::aligned_allocator<Eigen::Vector2f> >::const_iterator
            end() const { return v_.end(); }

        private:
            std::vector<Eigen::Vector2f, Eigen::aligned_allocator<Eigen::Vector2f> > v_;
            std::size_t x_dim_, y_dim_, z_dim_;
        };


    };
}
