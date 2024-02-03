//
// Created by Radzivon Bartoshyk on 02/02/2024.
//

#include "Wirth.h"
#include <algorithm>

template<class V>
V kthSmallest(V a[], int n, int k) {
    V i, j, l, m;
    V x;
    l = 0;
    m = n - 1;
    while (l < m) {
        x = a[k];
        i = l;
        j = m;
        do {
            while (a[i] < x) i++;
            while (x < a[j]) j--;
            if (i <= j) {
                std::swap(a[i], a[j]);
                i++;
                j--;
            }
        } while (i <= j);
        if (j < k) l = i;
        if (k < i) m = j;
    }
    return a[k];
}

template uint32_t kthSmallest(uint32_t a[], int n, int k);

template uint8_t kthSmallest(uint8_t a[], int n, int k);

template<class V>
V wirthMedian(V a[], int n) {
    return kthSmallest(a, n, (((n) & 1) ? ((n) / 2) : (((n) / 2) - 1)));
}

template uint8_t wirthMedian(uint8_t a[], int n);
template uint32_t wirthMedian(uint32_t a[], int n);
template int wirthMedian(int a[], int n);