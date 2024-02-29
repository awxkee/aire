/* LIBMORPHO
 *
 * arbitraryUtil.h
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU  General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  General Public License for more details.
 *
 * You should have received a copy of the GNU  General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */

#include "cstdint"

#ifndef __ARBITRARYUTIL__
#define __ARBITRARYUTIL__

#define MORPHO_SUCCESS 0
#define MORPHO_ERROR 1

/* Planar front */
struct front {
    int size;
    int *x, *y;
    int *pos;
    uint8_t *value;
};

/* arbritraryUtil.c */
int analyse_b(uint8_t *se, int seWidth, int seHeight, struct front *lf, struct front *rf, struct front *uf, struct front *df, int ox, int oy);

int transform_b(int seWidth, struct front *l, struct front *r, struct front *u, struct front *d);

int invert_SE(uint8_t *seIn, uint8_t *seOut, int width, int height);

void free_front(struct front *p);

#endif
