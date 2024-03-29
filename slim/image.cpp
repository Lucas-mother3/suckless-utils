/* SLiM - Simple Login Manager
 *  Copyright (C) 2004-06 Simone Rota <sip@varlock.com>
 *  Copyright (C) 2004-06 Johannes Winkelmann <jw@tks6.net>
 *  Copyright (C) 2012	Nobuhiro Iwamatsu <iwamatsu@nigauri.org>
 *  Copyright (C) 2022-23 Rob Pearce <slim@flitspace.org.uk>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  The following code has been adapted and extended from
 *  xplanet 1.0.1, Copyright (C) 2002-04 Hari Nair <hari@alumni.caltech.edu>
 */

#include <cctype>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>

using namespace std;

#include "log.h"
#include "const.h"
#include "image.h"

extern "C" {
	#include <jpeglib.h>
	#include <png.h>
}

Image::Image()
	: width(0), height(0), area(0), rgb_data(NULL), png_alpha(NULL)
{
}

Image::Image ( const int w, const int h, const unsigned char *rgb,
		const unsigned char *alpha)
	: width(w), height(h), area(w*h)
{
	width = w;
	height = h;
	area = w * h;

	rgb_data = (unsigned char *) malloc(3 * area);
	memcpy(rgb_data, rgb, 3 * area);

	if (alpha == NULL) {
		png_alpha = NULL;
	} else {
		png_alpha = (unsigned char *) malloc(area);
		memcpy(png_alpha, alpha, area);
	}
}

Image::~Image()
{
	free(rgb_data);
	free(png_alpha);
}

bool Image::Read(const char *filename)
{
	char buf[4];
	unsigned char *ubuf = (unsigned char *) buf;
	int success;
	int nr;

	FILE *file;
	file = fopen(filename, "rb");
	if (file == NULL)
		return(false);

	/* see what kind of file we have */

	nr = fread(buf, 1, 4, file);
	fclose(file);
	if ( nr < 4 )
		return false;	// Failed to read 4 bytes; probably empty file

	if ((ubuf[0] == 0x89) && !strncmp("PNG", buf+1, 3))
		success = readPng(filename, &width, &height, &rgb_data, &png_alpha);
	else if ((ubuf[0] == 0xff) && (ubuf[1] == 0xd8))
		success = readJpeg(filename, &width, &height, &rgb_data);
	else
	{
		fprintf(stderr, "Unknown image format\n");
		success = 0;
	}
	return(success == 1);
}

void Image::Reduce(const int factor)
{
	if (factor < 1)
		return;

	int scale = 1;
	for (int i = 0; i < factor; i++)
		scale *= 2;

	double scale2 = scale*scale;

	int w = width / scale;
	int h = height / scale;
	int new_area = w * h;

	unsigned char *new_rgb = (unsigned char *) malloc(3 * new_area);
	memset(new_rgb, 0, 3 * new_area);

	unsigned char *new_alpha = NULL;
	if (png_alpha != NULL) {
		new_alpha = (unsigned char *) malloc(new_area);
		memset(new_alpha, 0, new_area);
	}

	int ipos = 0;
	for (int j = 0; j < height; j++) {
		int js = j / scale;
		for (int i = 0; i < width; i++) {
			int is = i/scale;
			for (int k = 0; k < 3; k++)
				new_rgb[3*(js * w + is) + k] += static_cast<unsigned char> ((rgb_data[3*ipos + k] + 0.5) / scale2);

			if (png_alpha != NULL)
				new_alpha[js * w + is] += static_cast<unsigned char> (png_alpha[ipos]/scale2);
			ipos++;
		}
	}

	free(rgb_data);
	free(png_alpha);

	rgb_data = new_rgb;
	png_alpha = new_alpha;
	width = w;
	height = h;

	area = w * h;
}

void Image::Resize(const int w, const int h)
{

	if (width==w && height==h){
		return;
	}

	int new_area = w * h;

	unsigned char *new_rgb = (unsigned char *) malloc(3 * new_area);
	unsigned char *new_alpha = NULL;
	if (png_alpha != NULL)
		new_alpha = (unsigned char *) malloc(new_area);

	const double scale_x = ((double) w) / width;
	const double scale_y = ((double) h) / height;

	int ipos = 0;
	for (int j = 0; j < h; j++) {
		const double y = j / scale_y;
		for (int i = 0; i < w; i++) {
			const double x = i / scale_x;
			if (new_alpha == NULL)
				getPixel(x, y, new_rgb + 3*ipos);
			else
				getPixel(x, y, new_rgb + 3*ipos, new_alpha + ipos);
			ipos++;
		}
	}

	free(rgb_data);
	free(png_alpha);

	rgb_data = new_rgb;
	png_alpha = new_alpha;
	width = w;
	height = h;

	area = w * h;
}

/* Find the color of the desired point using bilinear interpolation. */
/* Assume the array indices refer to the center of the pixel, so each */
/* pixel has corners at (i - 0.5, j - 0.5) and (i + 0.5, j + 0.5) */
void Image::getPixel(double x, double y, unsigned char *pixel)
{
	getPixel(x, y, pixel, NULL);
}

void Image::getPixel(double x, double y, unsigned char *pixel, unsigned char *alpha)
{
	if (x < -0.5)
		x = -0.5;
	if (x >= width - 0.5)
		x = width - 0.5;

	if (y < -0.5)
		y = -0.5;
	if (y >= height - 0.5)
		y = height - 0.5;

	int ix0 = (int) (floor(x));
	int ix1 = ix0 + 1;
	if (ix0 < 0)
		ix0 = width - 1;
	if (ix1 >= width)
		ix1 = 0;

	int iy0 = (int) (floor(y));
	int iy1 = iy0 + 1;
	if (iy0 < 0)
		iy0 = 0;
	if (iy1 >= height)
		iy1 = height - 1;

	const double t = x - floor(x);
	const double u = 1 - (y - floor(y));

	double weight[4];
	weight[1] = t * u;
	weight[0] = u - weight[1];
	weight[2] = 1 - t - u + weight[1];
	weight[3] = t - weight[1];

	unsigned char *pixels[4];
	pixels[0] = rgb_data + 3 * (iy0 * width + ix0);
	pixels[1] = rgb_data + 3 * (iy0 * width + ix1);
	pixels[2] = rgb_data + 3 * (iy1 * width + ix0);
	pixels[3] = rgb_data + 3 * (iy1 * width + ix1);

	memset(pixel, 0, 3);
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 3; j++)
			pixel[j] += (unsigned char) (weight[i] * pixels[i][j]);
	}

	if (alpha != NULL) {
		unsigned char pixels[4];
		pixels[0] = png_alpha[iy0 * width + ix0];
		pixels[1] = png_alpha[iy0 * width + ix1];
		pixels[2] = png_alpha[iy0 * width + ix0];
		pixels[3] = png_alpha[iy1 * width + ix1];

		for (int i = 0; i < 4; i++)
			*alpha = (unsigned char) (weight[i] * pixels[i]);
	}
}


/**
 * Merge the image with a background, taking care of the image Alpha
 * transparency. (background alpha is ignored).
 *
 * The image is merged with the section of background at position (x, y).
 * The background must fully contain the image.
 * If the image does not have any transparency (no alpha data) then this
 * is a no-operation.
 * @note use of double to calculate the new value of a U8
 */
void Image::Merge ( const Image* background, const int x, const int y )
{
	if ( ( x + width > background->Width() )
	  || ( y + height > background->Height() ) )
		return;

	if (png_alpha != NULL)
	{
		unsigned char *new_rgb = (unsigned char *) malloc(3 * width * height);
		const unsigned char *bg_rgb = background->getRGBData();
		double tmp;
		int opos = 0;
		for (int j = 0; j < height; j++)
		{
			int ipos = (y+j) * background->Width() + x;
			for (int i = 0; i < width; i++)
			{
				for (int k = 0; k < 3; k++)
				{
					tmp = rgb_data[3*opos + k]*png_alpha[opos]/255.0
							+ bg_rgb[3*ipos + k]*(1-png_alpha[opos]/255.0);
					new_rgb[3*opos + k] = static_cast<unsigned char> (tmp);
				}
				opos++;
				ipos++;
			}
		}
		free(rgb_data);
		free(png_alpha);
		rgb_data = new_rgb;
		png_alpha = NULL;
	}
}


/* Merge the image with a background, taking care of the
 * image Alpha transparency. (background alpha is ignored).
 * The images is merged on position (x, y) on the
 * background, the background must contain the image.
 */
#define IMG_POS_RGB(p, x) (3 * p + x)
void Image::Merge_non_crop(Image* background, const int x, const int y)
{
	int bg_w = background->Width();
	int bg_h = background->Height();

	if (x + width > bg_w || y + height > bg_h)
		return;

	double tmp;
	unsigned char *new_rgb = (unsigned char *)malloc(3 * bg_w * bg_h);
	const unsigned char *bg_rgb = background->getRGBData();
	int pnl_pos = 0;
	int bg_pos = 0;
	int pnl_w_end = x + width;
	int pnl_h_end = y + height;

	memcpy(new_rgb, bg_rgb, 3 * bg_w * bg_h);

	for (int j = 0; j < bg_h; j++) {
		for (int i = 0; i < bg_w; i++) {
			if (j >= y && i >= x && j < pnl_h_end && i < pnl_w_end ) {
				for (int k = 0; k < 3; k++) {
					if (png_alpha != NULL)
						tmp = rgb_data[IMG_POS_RGB(pnl_pos, k)]
							* png_alpha[pnl_pos]/255.0
							+ bg_rgb[IMG_POS_RGB(bg_pos, k)]
							* (1 - png_alpha[pnl_pos]/255.0);
					else
						tmp = rgb_data[IMG_POS_RGB(pnl_pos, k)];

					new_rgb[IMG_POS_RGB(bg_pos, k)] = static_cast<unsigned char>(tmp);
				}
				pnl_pos++;
			}
			bg_pos++;
		}
	}

	width = bg_w;
	height = bg_h;

	free(rgb_data);
	free(png_alpha);
	rgb_data = new_rgb;
	png_alpha = NULL;
}


/* Tile the image growing its size to the minimum entire
 * multiple of w * h.
 * The new dimensions should be > of the current ones.
 * Note that this flattens image (alpha removed)
 */
void Image::Tile(const int w, const int h)
{
	if (w < width || h < height)
		return;

	int nx = w / width;
	if (w % width > 0)
		nx++;
	int ny = h / height;
	if (h % height > 0)
		ny++;

	int newwidth = nx*width;
	int newheight=ny*height;

	unsigned char *new_rgb = (unsigned char *) malloc(3 * newwidth * newheight);
	memset(new_rgb, 0, 3 * width * height * nx * ny);

	int ipos = 0;
	int opos = 0;

	for (int r = 0; r < ny; r++) {
		for (int c = 0; c < nx; c++) {
			for (int j = 0; j < height; j++) {
				for (int i = 0; i < width; i++) {
					opos = j*width + i;
					ipos = r*width*height*nx + j*newwidth + c*width +i;
					for (int k = 0; k < 3; k++) {
						new_rgb[3*ipos + k] = static_cast<unsigned char> (rgb_data[3*opos + k]);
					}
				}
			}
		}
	}

	free(rgb_data);
	free(png_alpha);
	rgb_data = new_rgb;
	png_alpha = NULL;
	width = newwidth;
	height = newheight;
	area = width * height;
	Crop(0,0,w,h);
}


/* Crop the image
 */
void Image::Crop(const int x, const int y, const int w, const int h)
{
	if (x+w > width || y+h > height) {
		return;
	}

	int x2 = x + w;
	int y2 = y + h;
	unsigned char *new_rgb = (unsigned char *) malloc(3 * w * h);
	memset(new_rgb, 0, 3 * w * h);
	unsigned char *new_alpha = NULL;
	if (png_alpha != NULL) {
		new_alpha = (unsigned char *) malloc(w * h);
		memset(new_alpha, 0, w * h);
	}

	int ipos = 0;
	int opos = 0;

	for (int j = 0; j < height; j++) {
		for (int i = 0; i < width; i++) {
			if (j>=y && i>=x && j<y2 && i<x2) {
				for (int k = 0; k < 3; k++) {
					new_rgb[3*ipos + k] = static_cast<unsigned char> (rgb_data[3*opos + k]);
				}
				if (png_alpha != NULL)
					new_alpha[ipos] = static_cast<unsigned char> (png_alpha[opos]);
				ipos++;
			}
			opos++;
		}
	}

	free(rgb_data);
	free(png_alpha);
	rgb_data = new_rgb;
	if (png_alpha != NULL)
		png_alpha = new_alpha;
	width = w;
	height = h;
	area = w * h;
}


/* Center the image in a rectangle of given width and height.
 * Fills the remaining space (if any) with the hex color
 */
void Image::Center(const int w, const int h, const char *hex)
{
	unsigned long packed_rgb;
	sscanf(hex, "%lx", &packed_rgb);

	unsigned long r = packed_rgb>>16;
	unsigned long g = packed_rgb>>8 & 0xff;
	unsigned long b = packed_rgb & 0xff;

	unsigned char *new_rgb = (unsigned char *) malloc(3 * w * h);
	memset(new_rgb, 0, 3 * w * h);

	int x = (w - width) / 2;
	int y = (h - height) / 2;

	if (x<0) {
		Crop((width - w)/2,0,w,height);
		x = 0;
	}
	if (y<0) {
		Crop(0,(height - h)/2,width,h);
		y = 0;
	}
	int x2 = x + width;
	int y2 = y + height;

	int ipos = 0;
	int opos = 0;
	double tmp;

	area = w * h;
	for (int i = 0; i < area; i++) {
		new_rgb[3*i] = r;
		new_rgb[3*i+1] = g;
		new_rgb[3*i+2] = b;
	}

	if (png_alpha != NULL) {
		for (int j = 0; j < h; j++) {
			for (int i = 0; i < w; i++) {
				if (j>=y && i>=x && j<y2 && i<x2) {
					ipos = j*w + i;
					for (int k = 0; k < 3; k++) {
						tmp = rgb_data[3*opos + k]*png_alpha[opos]/255.0
							  + new_rgb[k]*(1-png_alpha[opos]/255.0);
						new_rgb[3*ipos + k] = static_cast<unsigned char> (tmp);
					}
					opos++;
				}
			}
		}
	} else {
		for (int j = 0; j < h; j++) {
			for (int i = 0; i < w; i++) {
				if (j>=y && i>=x && j<y2 && i<x2) {
					ipos = j*w + i;
					for (int k = 0; k < 3; k++) {
						tmp = rgb_data[3*opos + k];
						new_rgb[3*ipos + k] = static_cast<unsigned char> (tmp);
					}
					opos++;
				}
			}
		}
	}

	free(rgb_data);
	free(png_alpha);
	rgb_data = new_rgb;
	png_alpha = NULL;
	width = w;
	height = h;
}


/* Fill the image with the given color and adjust its dimensions
 * to passed values.
 */
void Image::Plain(const int w, const int h, const char *hex)
{
	unsigned long packed_rgb;
	sscanf(hex, "%lx", &packed_rgb);

	unsigned long r = packed_rgb>>16;
	unsigned long g = packed_rgb>>8 & 0xff;
	unsigned long b = packed_rgb & 0xff;

	unsigned char *new_rgb = (unsigned char *) malloc(3 * w * h);
	memset(new_rgb, 0, 3 * w * h);

	area = w * h;
	for (int i = 0; i < area; i++) {
		new_rgb[3*i] = r;
		new_rgb[3*i+1] = g;
		new_rgb[3*i+2] = b;
	}

	free(rgb_data);
	free(png_alpha);
	rgb_data = new_rgb;
	png_alpha = NULL;
	width = w;
	height = h;
}


void Image::computeShift(unsigned long mask,
					unsigned char &left_shift,
					unsigned char &right_shift)
{
	left_shift = 0;
	right_shift = 8;
	if (mask != 0) {
		while ((mask & 0x01) == 0) {
			left_shift++;
			mask >>= 1;
		}
		while ((mask & 0x01) == 1) {
			right_shift--;
			mask >>= 1;
		}
	}
}


Pixmap Image::createPixmap(Display* dpy, int scr, Window win)
{
	int i, j;   /* loop variables */

	const int depth = DefaultDepth(dpy, scr);
	Visual *visual = DefaultVisual(dpy, scr);
	Colormap colormap = DefaultColormap(dpy, scr);

	Pixmap tmp = XCreatePixmap(dpy, win, width, height, depth);

	char *pixmap_data = NULL;
	switch (depth) {
	case 32:
	case 24:
		pixmap_data = new char[4 * width * height];
		break;
	case 16:
	case 15:
		pixmap_data = new char[2 * width * height];
		break;
	case 8:
		pixmap_data = new char[width * height];
		break;
	default:
		break;
	}

	XImage *ximage = XCreateImage(dpy, visual, depth, ZPixmap, 0,
								  pixmap_data, width, height,
								  8, 0);

	int entries;
	XVisualInfo v_template;
	v_template.visualid = XVisualIDFromVisual(visual);
	XVisualInfo *visual_info = XGetVisualInfo(dpy, VisualIDMask,
							   &v_template, &entries);

	unsigned long ipos = 0;
	switch (visual_info->c_class) {
	case PseudoColor: {
			XColor xc;
			xc.flags = DoRed | DoGreen | DoBlue;

			int num_colors = 256;
			XColor *colors = new XColor[num_colors];
			for (i = 0; i < num_colors; i++)
				colors[i].pixel = (unsigned long) i;
			XQueryColors(dpy, colormap, colors, num_colors);

			int *closest_color = new int[num_colors];

			for (i = 0; i < num_colors; i++) {
				xc.red = (i & 0xe0) << 8;		   /* highest 3 bits */
				xc.green = (i & 0x1c) << 11;		/* middle 3 bits */
				xc.blue = (i & 0x03) << 14;		 /* lowest 2 bits */

				/* find the closest color in the colormap */
				double distance, distance_squared, min_distance = 0;
				for (int ii = 0; ii < num_colors; ii++) {
					distance = colors[ii].red - xc.red;
					distance_squared = distance * distance;
					distance = colors[ii].green - xc.green;
					distance_squared += distance * distance;
					distance = colors[ii].blue - xc.blue;
					distance_squared += distance * distance;

					if ((ii == 0) || (distance_squared <= min_distance)) {
						min_distance = distance_squared;
						closest_color[i] = ii;
					}
				}
			}

			for (j = 0; j < height; j++) {
				for (i = 0; i < width; i++) {
					xc.red = (unsigned short) (rgb_data[ipos++] & 0xe0);
					xc.green = (unsigned short) (rgb_data[ipos++] & 0xe0);
					xc.blue = (unsigned short) (rgb_data[ipos++] & 0xc0);

					xc.pixel = xc.red | (xc.green >> 3) | (xc.blue >> 6);
					XPutPixel(ximage, i, j,
							  colors[closest_color[xc.pixel]].pixel);
				}
			}
			delete [] colors;
			delete [] closest_color;
		}
		break;
	case TrueColor: {
			unsigned char red_left_shift;
			unsigned char red_right_shift;
			unsigned char green_left_shift;
			unsigned char green_right_shift;
			unsigned char blue_left_shift;
			unsigned char blue_right_shift;

			computeShift(visual_info->red_mask, red_left_shift,
						 red_right_shift);
			computeShift(visual_info->green_mask, green_left_shift,
						 green_right_shift);
			computeShift(visual_info->blue_mask, blue_left_shift,
						 blue_right_shift);

			unsigned long pixel;
			unsigned long red, green, blue;
			for (j = 0; j < height; j++) {
				for (i = 0; i < width; i++) {
					red = (unsigned long)
						  rgb_data[ipos++] >> red_right_shift;
					green = (unsigned long)
							rgb_data[ipos++] >> green_right_shift;
					blue = (unsigned long)
						   rgb_data[ipos++] >> blue_right_shift;

					pixel = (((red << red_left_shift) & visual_info->red_mask)
							 | ((green << green_left_shift)
								& visual_info->green_mask)
							 | ((blue << blue_left_shift)
								& visual_info->blue_mask));

					XPutPixel(ximage, i, j, pixel);
				}
			}
		}
		break;
	default: {
			logStream << APPNAME << ": could not load image" << endl;
			return(tmp);
		}
	}

	GC gc = XCreateGC(dpy, win, 0, NULL);
	XPutImage(dpy, tmp, gc, ximage, 0, 0, 0, 0, width, height);

	XFreeGC(dpy, gc);

	XFree(visual_info);

	delete [] pixmap_data;

	/* Set ximage data to NULL since pixmap data was deallocated above */
	ximage->data = NULL;
	XDestroyImage(ximage);

	return(tmp);
}

int Image::readJpeg(const char *filename, int *width, int *height,
				unsigned char **rgb)
{
	int ret = 0;
	struct jpeg_decompress_struct cinfo;
	struct jpeg_error_mgr jerr;
	unsigned char *ptr = NULL;

	FILE *infile = fopen(filename, "rb");
	if (infile == NULL) {
		logStream << APPNAME << "Cannot fopen file: " << filename << endl;
		return ret;
	}

	cinfo.err = jpeg_std_error(&jerr);
	jpeg_create_decompress(&cinfo);
	jpeg_stdio_src(&cinfo, infile);
	jpeg_read_header(&cinfo, TRUE);
	jpeg_start_decompress(&cinfo);

	/* Prevent against integer overflow */
	if ( cinfo.output_width >= MAX_DIMENSION
	   || cinfo.output_height >= MAX_DIMENSION)
	{
		logStream << APPNAME << "Unreasonable dimension found in file: "
				  << filename << endl;
		goto close_file;
	}

	*width = cinfo.output_width;
	*height = cinfo.output_height;

	rgb[0] = (unsigned char*)
				malloc(3 * cinfo.output_width * cinfo.output_height);
	if (rgb[0] == NULL) {
		logStream << APPNAME << ": Can't allocate memory for JPEG file."
				  << endl;
		goto close_file;
	}

	if (cinfo.output_components == 3) {
		ptr = rgb[0];
		while (cinfo.output_scanline < cinfo.output_height) {
			jpeg_read_scanlines(&cinfo, &ptr, 1);
			ptr += 3 * cinfo.output_width;
		}
	} else if (cinfo.output_components == 1) {
		ptr = (unsigned char*) malloc(cinfo.output_width);
		if (ptr == NULL) {
			logStream << APPNAME << ": Can't allocate memory for JPEG file."
					  << endl;
			goto rgb_free;
		}

		unsigned int ipos = 0;
		while (cinfo.output_scanline < cinfo.output_height) {
			jpeg_read_scanlines(&cinfo, &ptr, 1);

			for (unsigned int i = 0; i < cinfo.output_width; i++) {
				memset(rgb[0] + ipos, ptr[i], 3);
				ipos += 3;
			}
		}

		free(ptr);
	}

	jpeg_finish_decompress(&cinfo);

	ret = 1;
	goto close_file;

rgb_free:
	free(rgb[0]);

close_file:
	jpeg_destroy_decompress(&cinfo);
	fclose(infile);

	return(ret);
}

int Image::readPng(const char *filename, int *width, int *height,
			   unsigned char **rgb, unsigned char **alpha)
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
		logStream << APPNAME << "Can not fopen file: " << filename << endl;
		return ret;
	}

	png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING,
									 (png_voidp) NULL,
									 (png_error_ptr) NULL,
									 (png_error_ptr) NULL);
	if (!png_ptr) {
		goto file_close;
	}

	info_ptr = png_create_info_struct(png_ptr);
	if (!info_ptr) {
		png_destroy_read_struct(&png_ptr, (png_infopp) NULL,
								(png_infopp) NULL);
	}

#if PNG_LIBPNG_VER_MAJOR >= 1 && PNG_LIBPNG_VER_MINOR >= 4
	if (setjmp(png_jmpbuf((png_ptr))))
		goto png_destroy;
#else
	if (setjmp(png_ptr->jmpbuf))
		goto png_destroy;
#endif


	png_init_io(png_ptr, infile);
	png_read_info(png_ptr, info_ptr);

	png_get_IHDR(png_ptr, info_ptr, &w, &h, &bit_depth, &color_type,
				 &interlace_type, (int *) NULL, (int *) NULL);

	/* Prevent against integer overflow */
	if(w >= MAX_DIMENSION || h >= MAX_DIMENSION) {
		logStream << APPNAME << "Unreasonable dimension found in file: "
				  << filename << endl;
		goto png_destroy;
	}

	*width = (int) w;
	*height = (int) h;

	if (color_type == PNG_COLOR_TYPE_RGB_ALPHA
		|| color_type == PNG_COLOR_TYPE_GRAY_ALPHA)
	{
		alpha[0] = (unsigned char *) malloc(*width * *height);
		if (alpha[0] == NULL) {
			logStream << APPNAME
					<< ": Can't allocate memory for alpha channel in PNG file."
					<< endl;
			goto png_destroy;
		}
	}

	/* Change a paletted/grayscale image to RGB */
	if (color_type == PNG_COLOR_TYPE_PALETTE && bit_depth <= 8)
	{
		png_set_expand(png_ptr);
	}

	/* Change a grayscale image to RGB */
	if (color_type == PNG_COLOR_TYPE_GRAY
		|| color_type == PNG_COLOR_TYPE_GRAY_ALPHA)
	{
		png_set_gray_to_rgb(png_ptr);
	}

	/* If the PNG file has 16 bits per channel, strip them down to 8 */
	if (bit_depth == 16) {
	  png_set_strip_16(png_ptr);
	}

	/* use 1 byte per pixel */
	png_set_packing(png_ptr);

	row_pointers = (png_byte **) malloc(*height * sizeof(png_bytep));
	if (row_pointers == NULL) {
		logStream << APPNAME << ": Can't allocate memory for PNG file." << endl;
		goto png_destroy;
	}

	for (i = 0; i < *height; i++) {
		row_pointers[i] = (png_byte*) malloc(4 * *width);
		if (row_pointers == NULL) {
			logStream << APPNAME << ": Can't allocate memory for PNG file."
					  << endl;
			goto rows_free;
		}
	}

	png_read_image(png_ptr, row_pointers);

	rgb[0] = (unsigned char *) malloc(3 * (*width) * (*height));
	if (rgb[0] == NULL) {
		logStream << APPNAME << ": Can't allocate memory for PNG file." << endl;
		goto rows_free;
	}

	if (alpha[0] == NULL) {
		ptr = rgb[0];
		for (i = 0; i < *height; i++) {
			memcpy(ptr, row_pointers[i], 3 * (*width));
			ptr += 3 * (*width);
		}
	} else {
		ptr = rgb[0];
		for (i = 0; i < *height; i++) {
			unsigned int ipos = 0;
			for (int j = 0; j < *width; j++) {
				*ptr++ = row_pointers[i][ipos++];
				*ptr++ = row_pointers[i][ipos++];
				*ptr++ = row_pointers[i][ipos++];
				alpha[0][i * (*width) + j] = row_pointers[i][ipos++];
			}
		}
	}

	ret = 1; /* data reading is OK */

rows_free:
	for (i = 0; i < *height; i++) {
		if (row_pointers[i] != NULL ) {
			free(row_pointers[i]);
		}
	}

	free(row_pointers);

png_destroy:
	png_destroy_read_struct(&png_ptr, &info_ptr, (png_infopp) NULL);

file_close:
	fclose(infile);
	return(ret);
}

