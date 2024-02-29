/*
 *
 *  * MIT License
 *  *
 *  * Copyright (c) 2024 Radzivon Bartoshyk
 *  * aire [https://github.com/awxkee/aire]
 *  *
 *  * Created by Radzivon Bartoshyk on 29/02/24, 6:13 PM
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

#include "ArbitraryUtil.h"
#include <cstdint>
#include <memory>

/*******************************************************************/
/* Analyse the fronts of the structuring element 
   and compute the origin */
int analyse_b(uint8_t *se, int seWidth, int seHeight,
              struct front *lf, struct front *rf, struct front *uf, struct front *df,
              int ox, int oy) {
    int i, j, n;
    int *x, *y;
    uint8_t *b;

    b = se;

    if ((ox < 0) || (oy < 0) || (ox > seWidth) || (oy > seHeight)) {
        perror("ERROR(analyse_b): the origin of the structuring element must be included in the structuring element.");
        return MORPHO_ERROR;
    }
    if (se[ox + oy * seWidth] == 0) {
        perror("ERROR(analyse_b): for this function you need an origin of the structuring element that is not null.");
        return MORPHO_ERROR;
    }

/* Horizontal front to the left */
    n = 0;
    /* Counts the number of valid points */
    /* First column */
    for (j = 0; j < seHeight; j++)
        if (b[j * seWidth] != 0) n++;
    /* Other columns */
    for (i = 1; i < seWidth; i++)
        for (j = 0; j < seHeight; j++)
            if ((b[i + j * seWidth] != 0) && (b[i - 1 + j * seWidth] == 0)) n++;
    /* Parameter entry and memory allocation */
    lf->size = n;
    lf->pos = (int *) nullptr;
    lf->x = (int *) malloc(n * sizeof(int));
    x = lf->x;
    lf->y = (int *) malloc(n * sizeof(int));
    y = lf->y;
    lf->value = nullptr;
    /* Coordinate entry */
    n = 0;
    /* First column */
    for (j = 0; j < seHeight; j++)
        if (b[j * seWidth] != 0) {
            x[n] = (int) 0;
            y[n] = (int) j;
            n++;
        }
    /* Other column */
    for (i = 1; i < seWidth; i++)
        for (j = 0; j < seHeight; j++)
            if ((b[i + j * seWidth] != 0) && (b[i - 1 + j * seWidth] == 0)) {
                x[n] = (int) i;
                y[n] = (int) j;
                n++;
            }

/* Horizontal front to the right */
    n = 0;
    /* Counts the number of valid points */
    /* Last column */
    for (j = 0; j < seHeight; j++)
        if (b[seWidth - 1 + j * seWidth] != 0) n++;
    /* Other columns */
    for (i = 0; i < seWidth - 1; i++)
        for (j = 0; j < seHeight; j++)
            if ((b[i + j * seWidth] != 0) && (b[i + 1 + j * seWidth] == 0)) n++;
    /* Parameter entry and memory allocation */
    rf->size = n;
    rf->pos = (int *) NULL;
    rf->x = (int *) malloc(n * sizeof(int));
    x = rf->x;
    rf->y = (int *) malloc(n * sizeof(int));
    y = rf->y;
    rf->value = NULL;
    /* Coordinate entry */
    n = 0;
    /* Last column */
    for (j = 0; j < seHeight; j++)
        if (b[seWidth - 1 + j * seWidth] != 0) {
            x[n] = (int) seWidth - 1;
            y[n] = (int) j;
            n++;
        }
    /* Other columns */
    for (i = 0; i < seWidth - 1; i++)
        for (j = 0; j < seHeight; j++)
            if ((b[i + j * seWidth] != 0) && (b[i + 1 + j * seWidth] == 0)) {
                x[n] = (int) i;
                y[n] = (int) j;
                n++;
            }

/* Upwards vertical front */
    n = 0;
    /* Counts the number of valid points */
    /* First raw */
    for (i = 0; i < seWidth; i++)
        if (b[i] != 0) n++;
    /* Other raws */
    for (j = 1; j < seHeight; j++)
        for (i = 0; i < seWidth; i++)
            if ((b[i + j * seWidth] != 0) && (b[i + (j - 1) * seWidth] == 0)) n++;
    /* Parameter entry and memory allocation */
    uf->size = n;
    uf->pos = (int *) NULL;
    uf->x = (int *) malloc(n * sizeof(int));
    x = uf->x;
    uf->y = (int *) malloc(n * sizeof(int));
    y = uf->y;
    uf->value = NULL;
    /* Coordinate entry */
    n = 0;
    /* First raw */
    for (i = 0; i < seWidth; i++)
        if (b[i] != 0) {
            x[n] = (int) i;
            y[n] = (int) 0;
            n++;
        }
    /* Other raws */
    for (j = 1; j < seHeight; j++)
        for (i = 0; i < seWidth; i++)
            if ((b[i + j * seWidth] != 0) && (b[i + (j - 1) * seWidth] == 0)) {
                x[n] = (int) i;
                y[n] = (int) j;
                n++;
            }

/* Downwards vertical front */
    n = 0;
    /* Counts the number of valid points */
    /* Last raw */
    for (i = 0; i < seWidth; i++)
        if (b[i + (seHeight - 1) * seWidth] != 0) n++;
    /* Other raws */
    for (j = 0; j < seHeight - 1; j++)
        for (i = 0; i < seWidth; i++)
            if ((b[i + j * seWidth] != 0) && (b[i + (j + 1) * seWidth] == 0)) n++;
    /* Parameter entry and memory allocation */
    df->size = n;
    df->pos = (int *) NULL;
    df->x = (int *) malloc(n * sizeof(int));
    x = df->x;
    df->y = (int *) malloc(n * sizeof(int));
    y = df->y;
    df->value = NULL;
    /* Coordinate entry */
    n = 0;
    /* Last raw */
    for (i = 0; i < seWidth; i++)
        if (b[i + (seHeight - 1) * seWidth] != 0) {
            x[n] = (int) i;
            y[n] = (int) seHeight - 1;
            n++;
        }
    /* Other raws */
    for (j = 0; j < seHeight - 1; j++)
        for (i = 0; i < seWidth; i++)
            if ((b[i + j * seWidth] != 0) && (b[i + (j + 1) * seWidth] == 0)) {
                x[n] = (int) i;
                y[n] = (int) j;
                n++;
            }

    return MORPHO_SUCCESS;
}

/****************************************************************/
/* Modify the front structures to speed up the access operations 
   and look for an origin */
int transform_b(int seWidth, struct front *l, struct front *r, struct front *u, struct front *d) {
    int i, size, *pos;
    int *x, *y;

/* Left front */
    size = (int) l->size;
    x = (int *) l->x;
    y = (int *) l->y;
    l->pos = (int *) malloc(size * sizeof(int));
    pos = (int *) l->pos;
    for (i = size - 1; i >= 0; i--)
        pos[i] = (int) (x[i] + y[i] * seWidth);

/* Right front */
    size = (int) r->size;
    x = (int *) r->x;
    y = (int *) r->y;
    r->pos = (int *) malloc(size * sizeof(int));
    pos = (int *) r->pos;
    for (i = size - 1; i >= 0; i--)
        pos[i] = (int) (x[i] + y[i] * seWidth);

/* Upper front */
    size = (int) u->size;
    x = (int *) u->x;
    y = (int *) u->y;
    u->pos = (int *) malloc(size * sizeof(int));
    pos = (int *) u->pos;
    for (i = size - 1; i >= 0; i--)
        pos[i] = (int) (x[i] + y[i] * seWidth);

/* Down front */
    size = (int) d->size;
    x = (int *) d->x;
    y = (int *) d->y;
    d->pos = (int *) malloc(size * sizeof(int));
    pos = (int *) d->pos;
    for (i = size - 1; i >= 0; i--)
        pos[i] = (int) (x[i] + y[i] * seWidth);

    return MORPHO_SUCCESS;
}

/****************************************************************/
int invert_SE(uint8_t *seIn, uint8_t *seOut, int width, int height) {
    int i, j;

    for (j = 0; j < height; j++)
        for (i = 0; i < width; i++)
            seOut[i + j * width] = seIn[width - i - 1 + (height - j - 1) * width];

    return MORPHO_SUCCESS;
}

/****************************************************************/
void free_front(struct front *p) {
    if (p != NULL) {
        if (p->x != NULL) free(p->x);
        if (p->y != NULL) free(p->y);
        if (p->pos != NULL) free(p->pos);
        if (p->value != NULL) free(p->value);
    }
}
