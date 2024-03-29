/****************************************************************************
	png.c - read and write png images using libpng routines.
	Distributed with Xplanet.
	Copyright (C) 2002 Hari Nair <hari@alumni.caltech.edu>

	This program is free software; you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation; either version 2 of the License, or
	(at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program; if not, write to the Free Software
	Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
****************************************************************************/

#include <stdio.h>
#include <stdlib.h>

#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <png.h>
#include "const.h"

int read_png(const char *filename, int *width, int *height, unsigned char **rgb,
	 unsigned char **alpha)
{
	int ret = 0;

	png_structp png_ptr;
	png_infop info_ptr;
	png_bytepp row_pointers;

	unsigned char *ptr = NULL;
	png_uint_32 w, h;
	int bit_depth, color_type, interlace_type;
	int i;

	FILE *infile = fopen(filename, "rb");
	if (infile == NULL) {
		fprintf(stderr, "Can not fopen file: %s\n", filename);
		return ret;
	}

	png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING,
						(png_voidp)NULL,
						(png_error_ptr)NULL,
						(png_error_ptr)NULL);
	if (!png_ptr)
		goto file_close;

	info_ptr = png_create_info_struct(png_ptr);
	if (!info_ptr) {
		png_destroy_read_struct(&png_ptr,
						(png_infopp)NULL,
						(png_infopp)NULL);
	}

#if PNG_LIBPNG_VER_MAJOR >= 1 && PNG_LIBPNG_VER_MINOR >= 4
	if (setjmp(png_jmpbuf((png_ptr))))
#else
	if (setjmp(png_ptr->jmpbuf))
#endif
		goto png_destroy;

	png_init_io(png_ptr, infile);
	png_read_info(png_ptr, info_ptr);

	png_get_IHDR(png_ptr, info_ptr, &w, &h, &bit_depth, &color_type,
				 &interlace_type, (int *) NULL, (int *) NULL);

	/* Prevent against integer overflow */
	if (w >= MAX_DIMENSION || h >= MAX_DIMENSION) {
		fprintf(stderr,
			"Unreasonable dimension found in file: %s\n", filename);
		goto png_destroy;
	}

	*width = (int) w;
	*height = (int) h;

	if (color_type == PNG_COLOR_TYPE_RGB_ALPHA
		|| color_type == PNG_COLOR_TYPE_GRAY_ALPHA) {
		alpha[0] = malloc(*width * *height);
		if (alpha[0] == NULL) {
			fprintf(stderr,
				"Can't allocate memory for alpha channel in PNG file.\n");
			goto png_destroy;
		}
	}

	/* Change a paletted/grayscale image to RGB */
	if (color_type == PNG_COLOR_TYPE_PALETTE && bit_depth <= 8)
		png_set_expand(png_ptr);

	/* Change a grayscale image to RGB */
	if (color_type == PNG_COLOR_TYPE_GRAY ||
		color_type == PNG_COLOR_TYPE_GRAY_ALPHA)
		png_set_gray_to_rgb(png_ptr);

	/* If the PNG file has 16 bits per channel, strip them down to 8 */
	if (bit_depth == 16)
		png_set_strip_16(png_ptr);

	/* use 1 byte per pixel */
	png_set_packing(png_ptr);

	row_pointers = malloc(*height * sizeof(png_bytep));
	if (row_pointers == NULL) {
		fprintf(stderr, "Can't allocate memory for PNG file.\n");
		goto png_destroy;
	}

	for (i = 0; i < *height; i++) {
		row_pointers[i] = malloc(4 * *width);
		if (row_pointers == NULL) {
			fprintf(stderr,
				"Can't allocate memory for PNG line.\n");
			goto rows_free;
		}
	}

	png_read_image(png_ptr, row_pointers);

	rgb[0] = malloc(3 * *width * *height);
	if (rgb[0] == NULL) {
		fprintf(stderr, "Can't allocate memory for PNG file.\n");
		goto rows_free;
	}

	if (alpha[0] == NULL) {
		ptr = rgb[0];
		for (i = 0; i < *height; i++) {
			memcpy(ptr, row_pointers[i], 3 * *width);
			ptr += 3 * *width;
		}
	} else {
		int j;
		ptr = rgb[0];
		for (i = 0; i < *height; i++) {
			int ipos = 0;
			for (j = 0; j < *width; j++) {
				*ptr++ = row_pointers[i][ipos++];
				*ptr++ = row_pointers[i][ipos++];
				*ptr++ = row_pointers[i][ipos++];
				alpha[0][i * *width + j]
					= row_pointers[i][ipos++];
			}
		}
	}

	ret = 1; /* data reading is OK */

rows_free:
	for (i = 0; i < *height; i++) {
		if (row_pointers[i] != NULL)
			free(row_pointers[i]);
	}

	free(row_pointers);

png_destroy:
	png_destroy_read_struct(&png_ptr, &info_ptr, (png_infopp) NULL);

file_close:
	fclose(infile);
	return ret;
}
