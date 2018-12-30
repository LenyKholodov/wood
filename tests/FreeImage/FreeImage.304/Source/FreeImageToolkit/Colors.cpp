// ==========================================================
// Color manipulation routines
//
// Design and implementation by
// - Hervé Drolon (drolon@infonie.fr)
//
// This file is part of FreeImage 3
//
// COVERED CODE IS PROVIDED UNDER THIS LICENSE ON AN "AS IS" BASIS, WITHOUT WARRANTY
// OF ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING, WITHOUT LIMITATION, WARRANTIES
// THAT THE COVERED CODE IS FREE OF DEFECTS, MERCHANTABLE, FIT FOR A PARTICULAR PURPOSE
// OR NON-INFRINGING. THE ENTIRE RISK AS TO THE QUALITY AND PERFORMANCE OF THE COVERED
// CODE IS WITH YOU. SHOULD ANY COVERED CODE PROVE DEFECTIVE IN ANY RESPECT, YOU (NOT
// THE INITIAL DEVELOPER OR ANY OTHER CONTRIBUTOR) ASSUME THE COST OF ANY NECESSARY
// SERVICING, REPAIR OR CORRECTION. THIS DISCLAIMER OF WARRANTY CONSTITUTES AN ESSENTIAL
// PART OF THIS LICENSE. NO USE OF ANY COVERED CODE IS AUTHORIZED HEREUNDER EXCEPT UNDER
// THIS DISCLAIMER.
//
// Use at your own risk!
// ==========================================================

#include "FreeImage.h"
#include "Utilities.h"

/** @brief Inverts each pixel data.

@param src Input image to be processed.
@return Returns TRUE if successful, FALSE otherwise.
*/
BOOL DLL_CALLCONV 
FreeImage_Invert(FIBITMAP *src) {
	unsigned i, x, y, k;
	BYTE *bits;

	if (!src) return FALSE;

	int bpp = FreeImage_GetBPP(src);
	
	switch(bpp) {
        case 1 :
		case 4 :
		case 8 :
		{
			// if the dib has a colormap, just invert it
			// else, keep the linear grayscale

			if (FreeImage_GetColorType(src) == FIC_PALETTE) {
				RGBQUAD *pal = FreeImage_GetPalette(src);

				for(i = 0; i < FreeImage_GetColorsUsed(src); i++) {
					pal[i].rgbRed	= 255 - pal[i].rgbRed;
					pal[i].rgbGreen = 255 - pal[i].rgbGreen;
					pal[i].rgbBlue	= 255 - pal[i].rgbBlue;
				}
			} else {
				for(y = 0; y < FreeImage_GetHeight(src); y++) {
					bits = FreeImage_GetScanLine(src, y);

					for (x = 0; x < FreeImage_GetLine(src); x++) {
						bits[x] = ~bits[x];
					}
				}
			}

			break;
		}		

		case 24 :
		case 32 :
		{
			int bytespp = FreeImage_GetLine(src) / FreeImage_GetWidth(src);

			for(y = 0; y < FreeImage_GetHeight(src); y++) {
				bits =  FreeImage_GetScanLine(src, y);
				for(x = 0; x < FreeImage_GetWidth(src); x++) {
					for(k = 0; k < bytespp; k++) {
						bits[k] = ~bits[k];
					}
					bits += bytespp;
				}
			}

			break;
		}

	}
	return TRUE;
}

/** @brief Perfoms an histogram transformation on a 8, 24 or 32-bit image 
according to the values of a lookup table (LUT).

The transformation is done as follows.<br>
Image 8-bit : if the image has a color palette, the LUT is applied to this palette, 
otherwise, it is applied to the grey values.<br>
Image 24-bit & 32-bit : if channel == FICC_RGB, the same LUT is applied to each color
plane (R,G, and B). Otherwise, the LUT is applied to the specified channel only.
@param src Input image to be processed.
@param LUT Lookup table. <b>The size of 'LUT' is assumed to be 256.</b>
@param channel The color channel to be processed (only used with 24 & 32-bit DIB).
@return Returns TRUE if successful, FALSE otherwise.
@see FREE_IMAGE_COLOR_CHANNEL
*/
BOOL DLL_CALLCONV 
FreeImage_AdjustCurve(FIBITMAP *src, BYTE *LUT, FREE_IMAGE_COLOR_CHANNEL channel) {
	unsigned x, y;
	BYTE *bits = NULL;

	if(!src || !LUT)
		return FALSE;

	int bpp = FreeImage_GetBPP(src);
	if((bpp != 8) && (bpp != 24) && (bpp != 32))
		return FALSE;

	// apply the LUT
	switch(bpp) {

		case 8 :
		{
			// if the dib has a colormap, apply the LUT to it
			// else, apply the LUT to pixel values

			if(FreeImage_GetColorType(src) == FIC_PALETTE) {
				RGBQUAD *rgb = FreeImage_GetPalette(src);
				for (unsigned pal = 0; pal < FreeImage_GetColorsUsed(src); pal++) {
					rgb->rgbRed   = LUT[rgb->rgbRed];
					rgb->rgbGreen = LUT[rgb->rgbGreen];
					rgb->rgbBlue  = LUT[rgb->rgbBlue];
					rgb++;
				}
			}
			else {
				for(y = 0; y < FreeImage_GetHeight(src); y++) {
					bits =  FreeImage_GetScanLine(src, y);
					for(x = 0; x < FreeImage_GetWidth(src); x++) {
						bits[x] = LUT[ bits[x] ];
					}
				}
			}

			break;
		}

		case 24 :
		case 32 :
		{
			int bytespp = FreeImage_GetLine(src) / FreeImage_GetWidth(src);

			switch(channel) {
				case FICC_RGB :
					for(y = 0; y < FreeImage_GetHeight(src); y++) {
						bits =  FreeImage_GetScanLine(src, y);
						for(x = 0; x < FreeImage_GetWidth(src); x++) {
							bits[0] = LUT[ bits[0] ];	// B
							bits[1] = LUT[ bits[1] ];	// G
							bits[2] = LUT[ bits[2] ];	// R
							
							bits += bytespp;
						}
					}
					break;

				case FICC_BLUE :
					for(y = 0; y < FreeImage_GetHeight(src); y++) {
						bits =  FreeImage_GetScanLine(src, y);
						for(x = 0; x < FreeImage_GetWidth(src); x++) {
							bits[0] = LUT[ bits[0] ];	// B
							
							bits += bytespp;
						}
					}
					break;

				case FICC_GREEN :
					for(y = 0; y < FreeImage_GetHeight(src); y++) {
						bits =  FreeImage_GetScanLine(src, y);
						for(x = 0; x < FreeImage_GetWidth(src); x++) {
							bits[1] = LUT[ bits[1] ];	// G
							
							bits += bytespp;
						}
					}
					break;

				case FICC_RED :
					for(y = 0; y < FreeImage_GetHeight(src); y++) {
						bits =  FreeImage_GetScanLine(src, y);
						for(x = 0; x < FreeImage_GetWidth(src); x++) {
							bits[2] = LUT[ bits[2] ];	// R
							
							bits += bytespp;
						}
					}
					break;
					
				case FICC_ALPHA :
					if(bpp = 32) {
						for(y = 0; y < FreeImage_GetHeight(src); y++) {
							bits =  FreeImage_GetScanLine(src, y);
							for(x = 0; x < FreeImage_GetWidth(src); x++) {
								bits[3] = LUT[ bits[3] ];	// A
								
								bits += bytespp;
							}
						}
					}
					break;

				default:
					break;
			}
			break;
		}
	}

	return TRUE;
}

/** @brief Performs gamma correction on a 8, 24 or 32-bit image.

@param src Input image to be processed.
@param gamma Gamma value to use. A value of 1.0 leaves the image alone, 
less than one darkens it, and greater than one lightens it.
@return Returns TRUE if successful, FALSE otherwise.
*/
BOOL DLL_CALLCONV 
FreeImage_AdjustGamma(FIBITMAP *src, double gamma) {
	BYTE LUT[256];		// Lookup table

	if(!src || (gamma <= 0))
		return FALSE;
	
	// Build the lookup table

	double exponent = 1 / gamma;
	double v = 255 * pow(255, -exponent);
	for(int i = 0; i < 256; i++) {
		double color = pow(i, exponent) * v;
		if(color > 255)
			color = 255;
		LUT[i] = (BYTE)floor(color + 0.5);
	}

	// Apply the gamma correction
	return FreeImage_AdjustCurve(src, LUT, FICC_RGB);
}

/** @brief Adjusts the brightness of a 8, 24 or 32-bit image by a certain amount.

@param src Input image to be processed.
@param percentage Where -100 <= percentage <= 100<br>
A value 0 means no change, less than 0 will make the image darker 
and greater than 0 will make the image brighter.
@return Returns TRUE if successful, FALSE otherwise.
*/
BOOL DLL_CALLCONV 
FreeImage_AdjustBrightness(FIBITMAP *src, double percentage) {
	BYTE LUT[256];		// Lookup table
	double value;

	if(!src)
		return FALSE;
	
	// Build the lookup table
	for(int i = 0; i < 256; i++) {
		value = (i * (100 + percentage)) / 100;
		value = MAX(0.0, MIN(value, 255.0));
		LUT[i] = (BYTE)floor(value + 0.5);
	}
	return FreeImage_AdjustCurve(src, LUT, FICC_RGB);
}

/** @brief Adjusts the contrast of a 8, 24 or 32-bit image by a certain amount.

@param src Input image to be processed.
@param percentage Where -100 <= percentage <= 100<br>
A value 0 means no change, less than 0 will decrease the contrast 
and greater than 0 will increase the contrast of the image.
@return Returns TRUE if successful, FALSE otherwise.
*/
BOOL DLL_CALLCONV 
FreeImage_AdjustContrast(FIBITMAP *src, double percentage) {
	BYTE LUT[256];		// Lookup table
	double value;

	if(!src)
		return FALSE;
	
	// Build the lookup table
	for(int i = 0; i < 256; i++) {
		value = 128 + (i - 128) * (100 + percentage) / 100;
		value = MAX(0.0, MIN(value, 255.0));
		LUT[i] = (BYTE)floor(value + 0.5);
	}
	return FreeImage_AdjustCurve(src, LUT, FICC_RGB);
}

/** @brief Computes image histogram

For 24-bit and 32-bit images, histogram can be computed from red, green, blue and 
black channels. For 8-bit images, histogram is computed from the black channel. Other 
bit depth is not supported (nothing is done).
@param src Input image to be processed.
@param histo Histogram array to fill. <b>The size of 'histo' is assumed to be 256.</b>
@param channel Color channel to use
@return Returns TRUE if succesful, returns FALSE if the image bit depth isn't supported.
*/
BOOL DLL_CALLCONV 
FreeImage_GetHistogram(FIBITMAP *src, DWORD *histo, FREE_IMAGE_COLOR_CHANNEL channel) {
	BYTE pixel;
	BYTE *bits = NULL;
	unsigned x, y;

	if(!src || !histo) return FALSE;

	unsigned width  = FreeImage_GetWidth(src);
	unsigned height = FreeImage_GetHeight(src);
	unsigned bpp    = FreeImage_GetBPP(src);

	if(bpp == 8) {
		// clear histogram array
		memset(histo, 0, 256 * sizeof(DWORD));
		// compute histogram for black channel
		for(y = 0; y < height; y++) {
			bits = FreeImage_GetScanLine(src, y);
			for(x = 0; x < width; x++) {
				// get pixel value
				pixel = bits[x];
				histo[pixel]++;
			}
		}
		return TRUE;
	}
	else if((bpp == 24) || (bpp == 32)) {
		int bytespp = bpp / 8;	// bytes / pixel

		// clear histogram array
		memset(histo, 0, 256 * sizeof(DWORD));

		switch(channel) {
			case FICC_RED:
				// compute histogram for red channel
				for(y = 0; y < height; y++) {
					bits =  FreeImage_GetScanLine(src, y);
					for(x = 0; x < width; x++) {
						pixel = bits[2];	// R
						histo[pixel]++;
						bits += bytespp;
					}
				}
				return TRUE;

			case FICC_GREEN:
				// compute histogram for green channel
				for(y = 0; y < height; y++) {
					bits =  FreeImage_GetScanLine(src, y);
					for(x = 0; x < width; x++) {
						pixel = bits[1];	// G
						histo[pixel]++;
						bits += bytespp;
					}
				}
				return TRUE;

			case FICC_BLUE:
				// compute histogram for blue channel
				for(y = 0; y < height; y++) {
					bits =  FreeImage_GetScanLine(src, y);
					for(x = 0; x < width; x++) {
						pixel = bits[0];	// B
						histo[pixel]++;
						bits += bytespp;
					}
				}
				return TRUE;

			case FICC_BLACK:
				// compute histogram for black channel
				for(y = 0; y < height; y++) {
					bits =  FreeImage_GetScanLine(src, y);
					for(x = 0; x < width; x++) {
						// RGB to GREY conversion
						pixel = GREY(bits[2], bits[1], bits[0]);
						histo[pixel]++;
						bits += bytespp;
					}
				}
				return TRUE;
				
			default:
				return FALSE;
		}
	}

	return FALSE;
}

/** @brief Retrieves the red, green, blue or alpha channel of a 24- or 32-bit BGR[A] image. 
@param src Input image to be processed.
@param channel Color channel to extract
@return Returns the extracted channel if successful, returns NULL otherwise.
*/
FIBITMAP * DLL_CALLCONV 
FreeImage_GetChannel(FIBITMAP *src, FREE_IMAGE_COLOR_CHANNEL channel) {
	int c;

	if(!src) return NULL;

	unsigned bpp = FreeImage_GetBPP(src);
	if((bpp == 24) || (bpp == 32)) {
		// select the channel to extract
		switch(channel) {
			case FICC_BLUE:
				c = 0;
				break;
			case FICC_GREEN:
				c = 1;
				break;
			case FICC_RED: 
				c = 2;
				break;
			case FICC_ALPHA:
				if(bpp != 32) return NULL;
				c = 3;
				break;
			default:
				return NULL;
		}

		// allocate a 8-bit dib
		unsigned width  = FreeImage_GetWidth(src);
		unsigned height = FreeImage_GetHeight(src);
		FIBITMAP *dst = FreeImage_Allocate(width, height, 8) ;
		if(!dst) return NULL;
		// build a greyscale palette
		RGBQUAD *pal = FreeImage_GetPalette(dst);
		for(int i = 0; i < 256; i++) {
			pal[i].rgbBlue = pal[i].rgbGreen = pal[i].rgbRed = i;
		}

		// perform extraction

		int bytespp = bpp / 8;	// bytes / pixel

		for(unsigned y = 0; y < height; y++) {
			BYTE *src_bits = FreeImage_GetScanLine(src, y);
			BYTE *dst_bits = FreeImage_GetScanLine(dst, y);
			for(unsigned x = 0; x < width; x++) {
				dst_bits[x] = src_bits[c];
				src_bits += bytespp;
			}
		}

		return dst;
	}

	return NULL;
}

/** @brief Insert a 8-bit dib into a 24- or 32-bit image. 
Both src and dst must have the same width and height.
@param dst Image to modify (24- or 32-bit)
@param src Input 8-bit image to insert
@param channel Color channel to modify
@return Returns TRUE if successful, FALSE otherwise.
*/
BOOL DLL_CALLCONV 
FreeImage_SetChannel(FIBITMAP *dst, FIBITMAP *src, FREE_IMAGE_COLOR_CHANNEL channel) {
	int c;

	if(!src || !dst) return FALSE;

	// src image should be grayscale, dst image should be 24- or 32-bit
	unsigned src_bpp = FreeImage_GetBPP(src);
	unsigned dst_bpp = FreeImage_GetBPP(dst);
	if((src_bpp != 8) || (dst_bpp != 24) && (dst_bpp != 32))
		return FALSE;

	// src and dst images should have the same width and height
	unsigned src_width  = FreeImage_GetWidth(src);
	unsigned src_height = FreeImage_GetHeight(src);
	unsigned dst_width  = FreeImage_GetWidth(dst);
	unsigned dst_height = FreeImage_GetHeight(dst);
	if((src_width != dst_width) || (src_height != dst_height))
		return FALSE;

	// select the channel to modify
	switch(channel) {
		case FICC_BLUE:
			c = 0;
			break;
		case FICC_GREEN:
			c = 1;
			break;
		case FICC_RED: 
			c = 2;
			break;
		case FICC_ALPHA:
			if(dst_bpp != 32) return FALSE;
			c = 3;
			break;
		default:
			return FALSE;
	}

	// perform insertion

	int bytespp = dst_bpp / 8;	// bytes / pixel

	for(unsigned y = 0; y < dst_height; y++) {
		BYTE *src_bits = FreeImage_GetScanLine(src, y);
		BYTE *dst_bits = FreeImage_GetScanLine(dst, y);
		for(unsigned x = 0; x < dst_width; x++) {
			dst_bits[c] = src_bits[x];
			dst_bits += bytespp;
		}
	}

	return TRUE;
}
