/*
 *
 *  * MIT License
 *  *
 *  * Copyright (c) 2024 Radzivon Bartoshyk
 *  * aire [https://github.com/awxkee/aire]
 *  *
 *  * Created by Radzivon Bartoshyk on 24/02/24, 6:13 PM
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

#ifndef FACTORIZE_H
#define FACTORIZE_H

#include <iostream>
#include <thread>

static std::mutex fftSharedRcLock;

// Code adapted from gsl/fft/factorize.c
static void factorize(const int n,
                      int *n_factors,
                      int factors[],
                      int *implemented_factors) {
    int nf = 0;
    int ntest = n;
    int factor;
    int i = 0;

    if (n == 0) {
        printf("Length n must be positive integer\n");
        return;
    }

    if (n == 1) {
        factors[0] = 1;
        *n_factors = 1;
        return;
    }

    /* deal with the implemented factors */

    while (implemented_factors[i] && ntest != 1) {
        factor = implemented_factors[i];
        while ((ntest % factor) == 0) {
            ntest = ntest / factor;
            factors[nf] = factor;
            nf++;
        }
        i++;
    }

    // Ok that's it
    if (ntest != 1) {
        factors[nf] = ntest;
        nf++;
    }

    /* check that the factorization is correct */
    {
        int product = 1;

        for (i = 0; i < nf; i++) {
            product *= factors[i];
        }

        if (product != n) {
            printf("factorization failed");
        }
    }

    *n_factors = nf;
}

static bool is_optimal(int n, int *implemented_factors) {
    // We check that n is not a multiple of 4*4*4*2
    if (n % 4 * 4 * 4 * 2 == 0)
        return false;

    int nf = 0;
    int factors[64];
    int i = 0;
    factorize(n, &nf, factors, implemented_factors);

    // We just have to check if the last factor belongs to GSL_FACTORS
    while (implemented_factors[i]) {
        if (factors[nf - 1] == implemented_factors[i])
            return true;
        ++i;
    }
    return false;
}

static int find_closest_factor(int n, int *implemented_factor) {
    int j;
    if (is_optimal(n, implemented_factor))
        return n;
    else {
        j = n + 1;
        while (!is_optimal(j, implemented_factor))
            ++j;
        return j;
    }
}

static size_t fft_next_good_size(size_t N) {
    if (N <= 2)
        return 2;
    while (true) {
        size_t m = N;
        while ((m % 2) == 0)
            m /= 2;
        while ((m % 3) == 0)
            m /= 3;
        while ((m % 5) == 0)
            m /= 5;
        if (m <= 1)
            return N;
        N++;
    }
}

#endif
