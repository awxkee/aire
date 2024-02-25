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

#include "QuickSelect.h"
#include <algorithm>

template<class V>
V QuickSelect(V arr[], int n) {
    int low, high;
    int median;
    int middle, ll, hh;
    low = 0;
    high = n - 1;
    median = (low + high) / 2;
    for (;;) {
        if (high <= low) /* One element only */
            return arr[median];
        if (high == low + 1) { /* Two elements only */
            if (arr[low] > arr[high]) std::swap(arr[low], arr[high]);
            return arr[median];
        }
/* Find median of low, middle and high items; swap into position low */
        middle = (low + high) / 2;
        if (arr[middle] > arr[high]) std::swap(arr[middle], arr[high]);
        if (arr[low] > arr[high]) std::swap(arr[low], arr[high]);
        if (arr[middle] > arr[low]) std::swap(arr[middle], arr[low]);
/* Swap low item (now in position middle) into position (low+1) */
        std::swap(arr[middle], arr[low + 1]);
/* Nibble from each end towards middle, swapping items when stuck */
        ll = low + 1;
        hh = high;
        for (;;) {
            do ll++; while (arr[low] > arr[ll]);
            do hh--; while (arr[hh] > arr[low]);
            if (hh < ll)
                break;
            std::swap(arr[ll], arr[hh]);
        }
/* Swap middle item (in position low) back into correct position */
        std::swap(arr[low], arr[hh]);
/* Re-set active partition */
        if (hh <= median)
            low = ll;
        if (hh >= median)
            high = hh - 1;
    }
}

template uint32_t QuickSelect(uint32_t arr[], int n);
template uint8_t QuickSelect(uint8_t arr[], int n);
template int QuickSelect(int arr[], int n);