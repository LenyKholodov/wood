// ==========================================================
// Copy / paste routines
//
// - Floris van den Berg (flvdberg@wxs.nl)
// - Alexander Dymerets (sashad@te.net.ua)
// - Herv� Drolon (drolon@iut.univ-lehavre.fr)
// - Manfred Tausch (manfred.tausch@t-online.de)
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

// ----------------------------------------------------------
//   Macros + structures
// ----------------------------------------------------------

#define RGB555(r, g, b) ((r >> 3) | ((g >> 3) << 5) | ((b >> 3) << 10))
#define RGB565(r, g, b) ((r >> 3) | ((g >> 2) << 5) | ((b >> 3) << 11))

#ifdef WIN32
#pragma pack(push, 1)
#else
#pragma pack(1)
#endif

typedef struct tagRGBTRIPLE { 
  BYTE rgbtBlue; 
  BYTE rgbtGreen; 
  BYTE rgbtRed; 
} RGBTRIPLE; 

#ifdef WIN32
#pragma pack(pop)
#else
#pragma pack()
#endif

// ----------------------------------------------------------
//   Helpers
// ----------------------------------------------------------

/////////////////////////////////////////////////////////////
// Alpha blending / combine functions

// ----------------------------------------------------------
/// 8-bit
static BOOL Combine8(FIBITMAP *dst_dib, FIBITMAP *src_dib, WORD x, WORD y, WORD alpha);
/// 16-bit 555
static BOOL Combine16_555(FIBITMAP *dst_dib, FIBITMAP *src_dib, WORD x, WORD y, WORD alpha);
/// 16-bit 565
static BOOL Combine16_565(FIBITMAP *dst_dib, FIBITMAP *src_dib, WORD x, WORD y, WORD alpha);
/// 24-bit
static BOOL Combine24(FIBITMAP *dst_dib, FIBITMAP *src_dib, WORD x, WORD y, WORD alpha);
/// 32- bit
static BOOL Combine32(FIBITMAP *dst_dib, FIBITMAP *src_dib, WORD x, WORD y, WORD alpha);
// ----------------------------------------------------------

// ----------------------------------------------------------
//   8-bit
// ----------------------------------------------------------

static BOOL 
Combine8(FIBITMAP *dst_dib, FIBITMAP *src_dib, WORD x, WORD y, WORD alpha) {
	// check the bit depth of src and dst images
	if((FreeImage_GetBPP(dst_dib) != 8) || (FreeImage_GetBPP(src_dib) != 8)) {
		return FALSE;
	}

	// check the size of src image
	if((x + FreeImage_GetWidth(src_dib) > FreeImage_GetWidth(dst_dib)) ||	(y + FreeImage_GetHeight(src_dib) > FreeImage_GetHeight(dst_dib))) {
		return FALSE;
	}

	BYTE *dst_bits = FreeImage_GetBits(dst_dib) + ((FreeImage_GetHeight(dst_dib) - FreeImage_GetHeight(src_dib) - y) * FreeImage_GetPitch(dst_dib)) + (x);
	BYTE *src_bits = FreeImage_GetBits(src_dib);	

	if(alpha > 255) {
		// combine images
		for(WORD rows = 0; rows < FreeImage_GetHeight(src_dib); rows++) {
			memcpy(dst_bits, src_bits, FreeImage_GetLine(src_dib));

			dst_bits += FreeImage_GetPitch(dst_dib);
			src_bits += FreeImage_GetPitch(src_dib);
		}
	} else {
		// alpha blend images
		for(WORD rows = 0; rows < FreeImage_GetHeight(src_dib); rows++) {
			for (WORD cols = 0; cols < FreeImage_GetLine(src_dib); cols++) {							
				dst_bits[cols] = (BYTE)(((src_bits[cols] - dst_bits[cols]) * alpha + (dst_bits[cols] << 8)) >> 8);
			}

			dst_bits += FreeImage_GetPitch(dst_dib);
			src_bits += FreeImage_GetPitch(src_dib);
		}
	}

	return TRUE;
}

// ----------------------------------------------------------
//   16-bit
// ----------------------------------------------------------

static BOOL 
Combine16_555(FIBITMAP *dst_dib, FIBITMAP *src_dib, WORD x, WORD y, WORD alpha) {
	// check the bit depth of src and dst images
	if((FreeImage_GetBPP(dst_dib) != 16) || (FreeImage_GetBPP(src_dib) != 16)) {
		return FALSE;
	}

	// check the size of src image
	if((x + FreeImage_GetWidth(src_dib) > FreeImage_GetWidth(dst_dib)) ||	(y + FreeImage_GetHeight(src_dib) > FreeImage_GetHeight(dst_dib))) {
		return FALSE;
	}

	BYTE *dst_bits = FreeImage_GetBits(dst_dib) + ((FreeImage_GetHeight(dst_dib) - FreeImage_GetHeight(src_dib) - y) * FreeImage_GetPitch(dst_dib)) + (x * 2);
	BYTE *src_bits = FreeImage_GetBits(src_dib);	

	if (alpha > 255) {
		for(WORD rows = 0; rows < FreeImage_GetHeight(src_dib); rows++) {
			memcpy(dst_bits, src_bits, FreeImage_GetLine(src_dib));

			dst_bits += FreeImage_GetPitch(dst_dib);
			src_bits += FreeImage_GetPitch(src_dib);
		}
	} else {
		for(WORD rows = 0; rows < FreeImage_GetHeight(src_dib); rows++) {
			for(WORD cols = 0; cols < FreeImage_GetLine(src_dib); cols += 2) {
				RGBTRIPLE color_s;
				RGBTRIPLE color_t;
				
				WORD *tmp1 = (WORD *)&dst_bits[cols];
				WORD *tmp2 = (WORD *)&src_bits[cols];

				// convert 16-bit colors to 24-bit

				color_s.rgbtRed = ((*tmp1 & 0x7C00) >> 10) << 3;
				color_s.rgbtGreen = ((*tmp1 & 0x3E0) >> 4) << 2;
				color_s.rgbtBlue = (*tmp1 & 0x1F) << 3;

				color_t.rgbtRed = ((*tmp2 & 0x7C00) >> 10) << 3;
				color_t.rgbtGreen = ((*tmp2 & 0x3E0) >> 4) << 2;
				color_t.rgbtBlue = (*tmp2 & 0x1F) << 3;

				// alpha blend

				color_s.rgbtRed = (BYTE)(((color_t.rgbtRed - color_s.rgbtRed) * alpha + (color_s.rgbtRed << 8)) >> 8);
				color_s.rgbtGreen = (BYTE)(((color_t.rgbtGreen - color_s.rgbtGreen) * alpha + (color_s.rgbtGreen << 8)) >> 8);
				color_s.rgbtBlue = (BYTE)(((color_t.rgbtBlue - color_s.rgbtBlue) * alpha + (color_s.rgbtBlue << 8)) >> 8);

				// convert 24-bit color back to 16-bit

				*tmp1 = RGB555(color_s.rgbtRed, color_s.rgbtGreen, color_s.rgbtBlue);
			}

			dst_bits += FreeImage_GetPitch(dst_dib);
			src_bits += FreeImage_GetPitch(src_dib);
		}
	}

	return TRUE;
}

static BOOL 
Combine16_565(FIBITMAP *dst_dib, FIBITMAP *src_dib, WORD x, WORD y, WORD alpha) {
	// check the bit depth of src and dst images
	if((FreeImage_GetBPP(dst_dib) != 16) || (FreeImage_GetBPP(src_dib) != 16)) {
		return FALSE;
	}

	// check the size of src image
	if((x + FreeImage_GetWidth(src_dib) > FreeImage_GetWidth(dst_dib)) ||	(y + FreeImage_GetHeight(src_dib) > FreeImage_GetHeight(dst_dib))) {
		return FALSE;
	}

	BYTE *dst_bits = FreeImage_GetBits(dst_dib) + ((FreeImage_GetHeight(dst_dib) - FreeImage_GetHeight(src_dib) - y) * FreeImage_GetPitch(dst_dib)) + (x * 2);
	BYTE *src_bits = FreeImage_GetBits(src_dib);	

	if (alpha > 255) {
		for(WORD rows = 0; rows < FreeImage_GetHeight(src_dib); rows++) {
			memcpy(dst_bits, src_bits, FreeImage_GetLine(src_dib));

			dst_bits += FreeImage_GetPitch(dst_dib);
			src_bits += FreeImage_GetPitch(src_dib);
		}
	} else {
		for(WORD rows = 0; rows < FreeImage_GetHeight(src_dib); rows++) {
			for(WORD cols = 0; cols < FreeImage_GetLine(src_dib); cols += 2) {
				RGBTRIPLE color_s;
				RGBTRIPLE color_t;
				
				WORD *tmp1 = (WORD *)&dst_bits[cols];
				WORD *tmp2 = (WORD *)&src_bits[cols];

				// convert 16-bit colors to 24-bit

				color_s.rgbtRed = ((*tmp1 & 0xF800) >> 11) << 3;
				color_s.rgbtGreen = ((*tmp1 & 0x7E0) >> 5) << 2;
				color_s.rgbtBlue = (*tmp1 & 0x1F) << 3;

				color_t.rgbtRed = ((*tmp2 & 0xF800) >> 11) << 3;
				color_t.rgbtGreen = ((*tmp2 & 0x7E0) >> 5) << 2;
				color_t.rgbtBlue = (*tmp2 & 0x1F) << 3;

				// alpha blend

				color_s.rgbtRed = (BYTE)(((color_t.rgbtRed - color_s.rgbtRed) * alpha + (color_s.rgbtRed << 8)) >> 8);
				color_s.rgbtGreen = (BYTE)(((color_t.rgbtGreen - color_s.rgbtGreen) * alpha + (color_s.rgbtGreen << 8)) >> 8);
				color_s.rgbtBlue = (BYTE)(((color_t.rgbtBlue - color_s.rgbtBlue) * alpha + (color_s.rgbtBlue << 8)) >> 8);

				// convert 24-bit color back to 16-bit

				*tmp1 = RGB565(color_s.rgbtRed, color_s.rgbtGreen, color_s.rgbtBlue);
			}

			dst_bits += FreeImage_GetPitch(dst_dib);
			src_bits += FreeImage_GetPitch(src_dib);
		}
	}
	
	return TRUE;
}

// ----------------------------------------------------------
//   24-bit
// ----------------------------------------------------------

static BOOL 
Combine24(FIBITMAP *dst_dib, FIBITMAP *src_dib, WORD x, WORD y, WORD alpha) {
	// check the bit depth of src and dst images
	if((FreeImage_GetBPP(dst_dib) != 24) || (FreeImage_GetBPP(src_dib) != 24)) {
		return FALSE;
	}

	// check the size of src image
	if((x + FreeImage_GetWidth(src_dib) > FreeImage_GetWidth(dst_dib)) ||	(y + FreeImage_GetHeight(src_dib) > FreeImage_GetHeight(dst_dib))) {
		return FALSE;
	}

	BYTE *dst_bits = FreeImage_GetBits(dst_dib) + ((FreeImage_GetHeight(dst_dib) - FreeImage_GetHeight(src_dib) - y) * FreeImage_GetPitch(dst_dib)) + (x * 3);
	BYTE *src_bits = FreeImage_GetBits(src_dib);	

	if(alpha > 255) {
		// combine images
		for(WORD rows = 0; rows < FreeImage_GetHeight(src_dib); rows++) {
			memcpy(dst_bits, src_bits, FreeImage_GetLine(src_dib));

			dst_bits += FreeImage_GetPitch(dst_dib);
			src_bits += FreeImage_GetPitch(src_dib);
		}
	} else {
		// alpha blend images
		for(WORD rows = 0; rows < FreeImage_GetHeight(src_dib); rows++) {
			for (WORD cols = 0; cols < FreeImage_GetLine(src_dib); cols++) {							
				dst_bits[cols] = (BYTE)(((src_bits[cols] - dst_bits[cols]) * alpha + (dst_bits[cols] << 8)) >> 8);
			}

			dst_bits += FreeImage_GetPitch(dst_dib);
			src_bits += FreeImage_GetPitch(src_dib);
		}
	}

	return TRUE;
}

// ----------------------------------------------------------
//   32-bit
// ----------------------------------------------------------

static BOOL 
Combine32(FIBITMAP *dst_dib, FIBITMAP *src_dib, WORD x, WORD y, WORD alpha) {
	// check the bit depth of src and dst images
	if((FreeImage_GetBPP(dst_dib) != 32) || (FreeImage_GetBPP(src_dib) != 32)) {
		return FALSE;
	}

	// check the size of src image
	if((x + FreeImage_GetWidth(src_dib) > FreeImage_GetWidth(dst_dib)) ||	(y + FreeImage_GetHeight(src_dib) > FreeImage_GetHeight(dst_dib))) {
		return FALSE;
	}

	BYTE *dst_bits = FreeImage_GetBits(dst_dib) + ((FreeImage_GetHeight(dst_dib) - FreeImage_GetHeight(src_dib) - y) * FreeImage_GetPitch(dst_dib)) + (x * 4);
	BYTE *src_bits = FreeImage_GetBits(src_dib);	

	if (alpha > 255) {
		// combine images
		for(WORD rows = 0; rows < FreeImage_GetHeight(src_dib); rows++) {
			memcpy(dst_bits, src_bits, FreeImage_GetLine(src_dib));

			dst_bits += FreeImage_GetPitch(dst_dib);
			src_bits += FreeImage_GetPitch(src_dib);
		}
	} else {
		// alpha blend images
		for(WORD rows = 0; rows < FreeImage_GetHeight(src_dib); rows++) {
			for(WORD cols = 0; cols < FreeImage_GetLine(src_dib); cols++) {
				dst_bits[cols] = (BYTE)(((src_bits[cols] - dst_bits[cols]) * alpha + (dst_bits[cols] << 8)) >> 8);
			}

			dst_bits += FreeImage_GetPitch(dst_dib);
			src_bits += FreeImage_GetPitch(src_dib);
		}
	}

	return TRUE;
}

// ----------------------------------------------------------
//   FreeImage interface
// ----------------------------------------------------------

/**
Copy a sub part of the current image and returns it as a FIBITMAP*.
The bit depth of the bitmap must be equal to 1, 4, 8, 16, 24 or 32.
@param left Specifies the left position of the cropped rectangle. 
@param top Specifies the top position of the cropped rectangle. 
@param right Specifies the right position of the cropped rectangle. 
@param bottom Specifies the bottom position of the cropped rectangle. 
@return Returns subimage if successful, NULL otherwise.
*/
FIBITMAP * DLL_CALLCONV 
FreeImage_Copy(FIBITMAP *src, int left, int top, int right, int bottom) {

	if(!src) 
		return NULL;

	// normalize the rectangle
	if(right < left) {
		INPLACESWAP(left, right);
	}
	if(bottom < top) {
		INPLACESWAP(top, bottom);
	}
	// check the size of the sub image
	int src_width  = FreeImage_GetWidth(src);
	int src_height = FreeImage_GetHeight(src);
	if((left < 0) || (right > src_width) || (top < 0) || (bottom > src_height)) {
		return NULL;
	}

	// allocate the sub image
	unsigned bpp = FreeImage_GetBPP(src);
	int dst_width = (right - left);
	int dst_height = (bottom - top);
	FIBITMAP *dst = FreeImage_Allocate(dst_width, dst_height, bpp, 0xFF, 0xFF00, 0xFF0000);

	// get the dimensions
	int dst_line = FreeImage_GetLine(dst);
	int dst_pitch = FreeImage_GetPitch(dst);
	int src_pitch = FreeImage_GetPitch(src);

	// get the pointers to the bits and such

	BYTE *src_bits = FreeImage_GetScanLine(src, src_height - top - dst_height);
	switch(bpp) {
		case 1:
			// point to x = 0
			break;

		case 4:
			src_bits += (left * sizeof(BYTE)) / 2;
			break;

		case 8 :
			src_bits += left * sizeof(BYTE);
			break;

		case 16 :
			src_bits += left * sizeof(WORD);
			break;

		case 24 :
			src_bits += left * sizeof(RGBTRIPLE);
			break;

		case 32 :
			src_bits += left * sizeof(RGBQUAD);
			break;

		default:
			FreeImage_Unload(dst);
			return NULL;
	}

	BYTE *dst_bits = FreeImage_GetBits(dst);

	// copy the palette

	memcpy(FreeImage_GetPalette(dst), FreeImage_GetPalette(src), FreeImage_GetColorsUsed(src) * sizeof(RGBQUAD));

	// copy the bits
	if(bpp == 1) {
		BOOL value;
		unsigned y_src, y_dst;

		for(int y = 0; y < dst_height; y++) {
			y_src = y * src_pitch;
			y_dst = y * dst_pitch;
			for(int x = 0; x < dst_width; x++) {
				// get bit at (y, x) in src image
				value = (src_bits[y_src + ((left+x) >> 3)] & (0x80 >> ((left+x) & 0x07))) != 0;
				// set bit at (y, x) in dst image
				value ? dst_bits[y_dst + (x >> 3)] |= (0x80 >> (x & 0x7)) : dst_bits[y_dst + (x >> 3)] &= (0xff7f >> (x & 0x7));
			}
		}
	}

	else if(bpp == 4) {
		// TODO : case when x is odd
		for(int y = 0; y < dst_height; y++) {
			memcpy(dst_bits + (y * dst_pitch), src_bits + (y * src_pitch), dst_line);
		}
	}

	else if(bpp >= 8) {
		for(int y = 0; y < dst_height; y++) {
			memcpy(dst_bits + (y * dst_pitch), src_bits + (y * src_pitch), dst_line);
		}
	}

	return dst;
}

/**
Alpha blend or combine a sub part image with the current image.
The bit depth of dst bitmap must be greater than or equal to the bit depth of src. 
Upper promotion of src is done internally. Supported bit depth equals to 8, 16, 24 or 32.
@param src Source subimage
@param left Specifies the left position of the sub image. 
@param top Specifies the top position of the sub image. 
@param alpha Alpha blend factor. The source and destination images are alpha blended if 
alpha = 0..255. If alpha > 255, then the source image is combined to the destination image.
@return Returns TRUE if successful, FALSE otherwise.
*/
BOOL DLL_CALLCONV 
FreeImage_Paste(FIBITMAP *dst, FIBITMAP *src, int left, int top, int alpha) {
	BOOL bResult = FALSE;
	FIBITMAP *clone = NULL;

	if(!src || !dst) return FALSE;

	// check the size of src image
	if((left + FreeImage_GetWidth(src) > FreeImage_GetWidth(dst)) ||	(top + FreeImage_GetHeight(src) > FreeImage_GetHeight(dst))) {
		return FALSE;
	}

	// check the bit depth of src and dst images
	unsigned bpp_src = FreeImage_GetBPP(src);
	unsigned bpp_dst = FreeImage_GetBPP(dst);
	BOOL isRGB565 = FALSE;

	if ((FreeImage_GetRedMask(dst) == 0x1F) && (FreeImage_GetGreenMask(dst) == 0x7E0) && (FreeImage_GetBlueMask(dst) == 0xF800)) {
		isRGB565 = TRUE;
	} else {
		// includes case where all the masks are 0
		isRGB565 = FALSE;
	}

	// perform promotion if needed
	if(bpp_dst == bpp_src) {
		clone = src;
	} else if(bpp_dst > bpp_src) {
		// perform promotion
		switch(bpp_dst) {
			case 8:
				clone = FreeImage_ConvertTo8Bits(src);
				break;
			case 16:
				if (isRGB565) {
					clone = FreeImage_ConvertTo16Bits565(src);
				} else {
					// includes case where all the masks are 0
					clone = FreeImage_ConvertTo16Bits555(src);
				}
				break;
			case 24:
				clone = FreeImage_ConvertTo24Bits(src);
				break;
			case 32:
				clone = FreeImage_ConvertTo32Bits(src);
				break;
			default:
				return FALSE;
		}
	} else {
		return FALSE;
	}

	if(!clone) return FALSE;

	// paste src to dst
	switch(FreeImage_GetBPP(dst)) {
		case 8:
			bResult = Combine8(dst, clone, left, top, alpha);
			break;
		case 16:
			if (isRGB565) {
				bResult = Combine16_565(dst, clone, left, top, alpha);
			} else {
				// includes case where all the masks are 0
				bResult = Combine16_555(dst, clone, left, top, alpha);
			}
			break;
		case 24:
			bResult = Combine24(dst, clone, left, top, alpha);
			break;
		case 32:
			bResult = Combine32(dst, clone, left, top, alpha);
			break;
	}

	if(clone != src)
		FreeImage_Unload(clone);

	return bResult;
}


