// ==========================================================
// fipImage class implementation
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

#include "FreeImagePlus.h"

///////////////////////////////////////////////////////////////////   
// Protected functions

// Update data members. Called after each image resizing
void fipImage::updateInfo() {
	if(m_dib) {
		m_Width = FreeImage_GetWidth(m_dib);
		m_Height = FreeImage_GetHeight(m_dib);
		m_ScanWidth = FreeImage_GetPitch(m_dib);
		m_bits = FreeImage_GetBits(m_dib);
	}
}

BOOL fipImage::replace(FIBITMAP *new_dib) {
	if(new_dib == NULL) 
		return FALSE;
	if(m_dib)
		FreeImage_Unload(m_dib);
	m_dib = new_dib;
	m_Width = FreeImage_GetWidth(m_dib);
	m_Height = FreeImage_GetHeight(m_dib);
	m_ScanWidth = FreeImage_GetPitch(m_dib);
	m_bits = FreeImage_GetBits(m_dib);
	return TRUE;
}

///////////////////////////////////////////////////////////////////
// Creation & Destruction

fipImage::fipImage(WORD width, WORD height, WORD bpp) {
	m_dib = NULL;
	if(width && height && bpp) {
		setSize(width, height, bpp);
	}
	else {
		m_Width = m_Height = m_ScanWidth = 0;
		m_bits = NULL;
	}
}

fipImage::~fipImage() {
	if( m_dib ) FreeImage_Unload(m_dib);
	m_Width = m_Height = m_ScanWidth = 0;
	m_bits = NULL;
}

BOOL fipImage::setSize(WORD width, WORD height, WORD bpp) {
	RGBQUAD *pal;	// pointer to dib palette

	if(m_dib) {
		FreeImage_Unload(m_dib);
		m_Width = m_Height = m_ScanWidth = 0;
		m_bits = NULL;
	}
	if((m_dib = FreeImage_Allocate(width, height, bpp)) == NULL)
		return FALSE;

	updateInfo();

	// Create palette if needed
	switch(bpp)	{
		case 1:
		case 4:
		case 8:
			pal = FreeImage_GetPalette(m_dib);
			for(unsigned i = 0; i < FreeImage_GetColorsUsed(m_dib); i++) {
				pal[i].rgbRed = i;
				pal[i].rgbGreen = i;
				pal[i].rgbBlue = i;
			}
			break;
	}

	return TRUE;
}

///////////////////////////////////////////////////////////////////
// Copying

fipImage::fipImage(const fipImage& Image) {
	m_dib = NULL;
	FIBITMAP *clone = FreeImage_Clone((FIBITMAP*)Image.m_dib);
	replace(clone);
}

fipImage::fipImage(const FIBITMAP *dib) {
	m_dib = NULL;
	FIBITMAP *clone = FreeImage_Clone((FIBITMAP*)dib);
	replace(clone);
}

fipImage& fipImage::operator=(const fipImage& Image) {
	if(this != &Image) {
		FIBITMAP *clone = FreeImage_Clone((FIBITMAP*)Image.m_dib);
		replace(clone);
	}
	return *this;
}

fipImage& fipImage::operator=(FIBITMAP *dib) {
	replace(dib);
	return *this;
}

BOOL fipImage::copySubImage(fipImage& dst, int left, int top, int right, int bottom) {
	if(m_dib) {
		dst = FreeImage_Copy(m_dib, left, top, right, bottom);
		return dst.isValid();
	}
	return FALSE;
}

BOOL fipImage::pasteSubImage(fipImage& src, int left, int top, int alpha) {
	if(m_dib) {
		BOOL bResult = FreeImage_Paste(m_dib, src.m_dib, left, top, alpha);
		updateInfo();
		return bResult;
	}
	return FALSE;
}


///////////////////////////////////////////////////////////////////
// Information functions

FIBITMAP* fipImage::getFIBITMAP() {
	return m_dib;
}

BOOL fipImage::isValid() {
	return (m_dib != NULL) ? TRUE:FALSE;
}

BITMAPINFO* fipImage::getInfo() {
	return FreeImage_GetInfo(m_dib);
}

BITMAPINFOHEADER* fipImage::getInfoHeader() {
	return FreeImage_GetInfoHeader(m_dib);
}

LONG fipImage::getImageSize() {
	return FreeImage_GetDIBSize(m_dib);
}

WORD fipImage::getBitsPerPixel() {
	return FreeImage_GetBPP(m_dib);
}

WORD fipImage::getLine() {
	return FreeImage_GetLine(m_dib);
}

WORD fipImage::getHorizontalResolution() {
	return (FreeImage_GetDotsPerMeterX(m_dib) / 100); 
}

WORD fipImage::getVerticalResolution() {
	return (FreeImage_GetDotsPerMeterY(m_dib) / 100);
}

void fipImage::setHorizontalResolution(LONG value) {
	BITMAPINFOHEADER *bmih = FreeImage_GetInfoHeader(m_dib);
	bmih->biXPelsPerMeter = value * 100;
}

void fipImage::setVerticalResolution(LONG value) {
	BITMAPINFOHEADER *bmih = FreeImage_GetInfoHeader(m_dib);
	bmih->biYPelsPerMeter = value * 100;
}


///////////////////////////////////////////////////////////////////
// Palette operations

RGBQUAD* fipImage::getPalette() {
	return FreeImage_GetPalette(m_dib);
}

WORD fipImage::getPaletteSize() {
	return FreeImage_GetColorsUsed(m_dib) * sizeof(RGBQUAD);
}

WORD fipImage::getColorsUsed() {
	return FreeImage_GetColorsUsed(m_dib);
}

FREE_IMAGE_COLOR_TYPE fipImage::getColorType() { 
	return FreeImage_GetColorType(m_dib);
}

BOOL fipImage::isGrayscale() {
	return ((FreeImage_GetBPP(m_dib) == 8) && (FreeImage_GetColorType(m_dib) != FIC_PALETTE)); 
}

///////////////////////////////////////////////////////////////////
// Pixel access

BYTE* fipImage::accessPixels() {
	return FreeImage_GetBits(m_dib); 
}

BYTE*fipImage::getScanLine(int scanline) {
	if((scanline >= 0) && (scanline < m_Height)) {
		return FreeImage_GetScanLine(m_dib, scanline);
	}
	return NULL;
}

///////////////////////////////////////////////////////////////////
// Loading & Saving

BOOL fipImage::load(const char* lpszPathName, int flag) {
	FREE_IMAGE_FORMAT fif = FIF_UNKNOWN;

	// check the file signature and get its format
	// (the second argument is currently not used by FreeImage)
	fif = FreeImage_GetFileType(lpszPathName, 0);
	if(fif == FIF_UNKNOWN) {
		// no signature ?
		// try to guess the file format from the file extension
		fif = FreeImage_GetFIFFromFilename(lpszPathName);
	}
	// check that the plugin has reading capabilities ...
	if((fif != FIF_UNKNOWN) && FreeImage_FIFSupportsReading(fif)) {
		// Free the previous dib
		if(m_dib) {
			FreeImage_Unload(m_dib);
			m_Width = m_Height = m_ScanWidth = 0;
			m_bits = NULL;
		}
		// Load the file
		m_dib = FreeImage_Load(fif, lpszPathName, flag);
		if(m_dib == NULL)
			return FALSE;
		// Update image information
		updateInfo();
		return TRUE;
	}
	return FALSE;
}

BOOL fipImage::loadFromHandle(FreeImageIO *io, fi_handle handle, int flag) {
	FREE_IMAGE_FORMAT fif = FIF_UNKNOWN;

	// check the file signature and get its format
	fif = FreeImage_GetFileTypeFromHandle(io, handle, 16);
	if((fif != FIF_UNKNOWN) && FreeImage_FIFSupportsReading(fif)) {
		// Free the previous dib
		if(m_dib) {
			FreeImage_Unload(m_dib);
			m_Width = m_Height = m_ScanWidth = 0;
			m_bits = NULL;
		}
		// Load the file
		m_dib = FreeImage_LoadFromHandle(fif, io, handle, flag);
		if(m_dib == NULL)
			return FALSE;
		// Update image information
		updateInfo();
		return TRUE;
	}
	return FALSE;
}

BOOL fipImage::save(const char* lpszPathName, int flag) {
	FREE_IMAGE_FORMAT fif = FIF_UNKNOWN;
	BOOL bSuccess = FALSE;

	// Try to guess the file format from the file extension
	fif = FreeImage_GetFIFFromFilename(lpszPathName);
	if(fif != FIF_UNKNOWN ) {
		// Check that the dib can be saved in this format
		WORD bpp = FreeImage_GetBPP(m_dib);
		if(FreeImage_FIFSupportsWriting(fif) && FreeImage_FIFSupportsExportBPP(fif, bpp)) {
			bSuccess = FreeImage_Save(fif, m_dib, lpszPathName, flag);
			return bSuccess;
		}
	}
	return bSuccess;
}

BOOL fipImage::saveToHandle(FREE_IMAGE_FORMAT fif, FreeImageIO *io, fi_handle handle, int flag) {
	BOOL bSuccess = FALSE;

	if(fif != FIF_UNKNOWN ) {
		// Check that the dib can be saved in this format
		WORD bpp = FreeImage_GetBPP(m_dib);
		if(FreeImage_FIFSupportsWriting(fif) && FreeImage_FIFSupportsExportBPP(fif, bpp)) {
			bSuccess = FreeImage_SaveToHandle(fif, m_dib, io, handle, flag);
			return bSuccess;
		}
	}
	return bSuccess;
}

///////////////////////////////////////////////////////////////////   
// Conversion routines

BOOL fipImage::threshold(BYTE T) {
	if(m_dib) {
		FIBITMAP *dib1 = FreeImage_Threshold(m_dib, T);
		return replace(dib1);
	}
	return FALSE;
}

BOOL fipImage::convertTo8Bits() {
	if(m_dib) {
		FIBITMAP *dib8 = FreeImage_ConvertTo8Bits(m_dib);
		return replace(dib8);
	}
	return FALSE;
}

BOOL fipImage::convertTo16Bits555() {
	if(m_dib) {
		FIBITMAP *dib16_555 = FreeImage_ConvertTo16Bits555(m_dib);
		return replace(dib16_555);
	}
	return FALSE;
}

BOOL fipImage::convertTo16Bits565() {
	if(m_dib) {
		FIBITMAP *dib16_565 = FreeImage_ConvertTo16Bits565(m_dib);
		return replace(dib16_565);
	}
	return FALSE;
}

BOOL fipImage::convertTo24Bits() {
	if(m_dib) {
		FIBITMAP *dibRGB = FreeImage_ConvertTo24Bits(m_dib);
		return replace(dibRGB);
	}
	return FALSE;
}

BOOL fipImage::convertTo32Bits() {
	if(m_dib) {
		FIBITMAP *dib32 = FreeImage_ConvertTo32Bits(m_dib);
		return replace(dib32);
	}
	return FALSE;
}

BOOL fipImage::convertToGrayscale() {
	if(m_dib) {
		if(FreeImage_GetColorType(m_dib) == FIC_PALETTE) {
			// Convert the palette to 24-bit, then to 8-bit
			BOOL bResult;
			bResult = convertTo24Bits();
			bResult &= convertTo8Bits();
			return bResult;
		} else if(FreeImage_GetBPP(m_dib) != 8) {
			// Convert the bitmap to 8-bit greyscale
			return convertTo8Bits();
		}
	}
	return FALSE;
}

BOOL fipImage::colorQuantize(FREE_IMAGE_QUANTIZE algorithm) {
	if(m_dib) {
		FIBITMAP *dib8 = FreeImage_ColorQuantize(m_dib, algorithm);
		return replace(dib8);
	}
	return FALSE;
}

BOOL fipImage::dither(FREE_IMAGE_DITHER algorithm) {
	if(m_dib) {
		FIBITMAP *dib = FreeImage_Dither(m_dib, algorithm);
		return replace(dib);
	}
	return FALSE;
}

///////////////////////////////////////////////////////////////////   
// Channel processing support

BOOL fipImage::splitChannels(fipImage& RedChannel, fipImage& GreenChannel, fipImage& BlueChannel) {
	if(m_dib) {
		RedChannel = FreeImage_GetChannel(m_dib, FICC_RED);
		GreenChannel = FreeImage_GetChannel(m_dib, FICC_GREEN);
		BlueChannel = FreeImage_GetChannel(m_dib, FICC_BLUE);

		return (RedChannel.isValid() && GreenChannel.isValid() && BlueChannel.isValid());
	}
	return FALSE;
}

BOOL fipImage::getChannel(fipImage& image, FREE_IMAGE_COLOR_CHANNEL channel) {
	if(m_dib) {
		image = FreeImage_GetChannel(m_dib, channel);
		return image.isValid();
	}
	return FALSE;
}

BOOL fipImage::getRedChannel(fipImage& red) {
	if(m_dib) {
		red = FreeImage_GetChannel(m_dib, FICC_RED);
		return red.isValid();
	}
	return FALSE;
}

BOOL fipImage::getGreenChannel(fipImage& green) {
	if(m_dib) {
		green = FreeImage_GetChannel(m_dib, FICC_GREEN);
		return green.isValid();
	}
	return FALSE;
}

BOOL fipImage::getBlueChannel(fipImage& blue) {
	if(m_dib) {
		blue = FreeImage_GetChannel(m_dib, FICC_BLUE);
		return blue.isValid();
	}
	return FALSE;
}

BOOL fipImage::getAlphaChannel(fipImage& alpha) {
	if(m_dib) {
		alpha = FreeImage_GetChannel(m_dib, FICC_ALPHA);
		return alpha.isValid();
	}
	return FALSE;
}

BOOL fipImage::setAlphaChannel(fipImage& alpha) {
	if(!m_dib) {
		int width = alpha.getWidth();
		int height = alpha.getHeight();
		m_dib = FreeImage_Allocate(width, height, 32, 0xFF, 0xFF00, 0xFF0000);
	}

	if(m_dib) {
		return FreeImage_SetChannel(m_dib, alpha.m_dib, FICC_ALPHA);
	}
	return FALSE;
}

BOOL fipImage::combineChannels(fipImage& red, fipImage& green, fipImage& blue) {
	if(!m_dib) {
		int width = red.getWidth();
		int height = red.getHeight();
		m_dib = FreeImage_Allocate(width, height, 24, 0xFF, 0xFF00, 0xFF0000);
	}

	if(m_dib) {
		BOOL bResult = FreeImage_SetChannel(m_dib, red.m_dib, FICC_RED);
		bResult &= FreeImage_SetChannel(m_dib, green.m_dib, FICC_GREEN);
		bResult &= FreeImage_SetChannel(m_dib, blue.m_dib, FICC_BLUE);
		updateInfo();
		return bResult;
	}
	return FALSE;
}

///////////////////////////////////////////////////////////////////   
// Rotation and flipping

BOOL fipImage::rotateEx(double angle, double x_shift, double y_shift, double x_origin, double y_origin, BOOL use_mask) {
	if(m_dib) {
		if(FreeImage_GetBPP(m_dib) >= 8) {
			FIBITMAP *rotated = FreeImage_RotateEx(m_dib, angle, x_shift, y_shift, x_origin, y_origin, use_mask);
			return replace(rotated);
		}
	}
	return FALSE;
}

BOOL fipImage::rotate(double angle) {
	if(m_dib) {
		if(FreeImage_GetBPP(m_dib) >= 8) {
			FIBITMAP *rotated = FreeImage_RotateClassic(m_dib, angle);
			return replace(rotated);
		}
	}
	return FALSE;
}

BOOL fipImage::flipVertical() {
	if(m_dib) {
		return FreeImage_FlipVertical(m_dib);
	}
	return FALSE;
}

BOOL fipImage::flipHorizontal() {
	if(m_dib) {
		return FreeImage_FlipHorizontal(m_dib);
	}
	return FALSE;
}

///////////////////////////////////////////////////////////////////   
// Color manipulation routines

BOOL fipImage::invert() {
	if(m_dib) {
		return FreeImage_Invert(m_dib);
	}
	return FALSE;
}

BOOL fipImage::adjustCurve(BYTE *LUT, FREE_IMAGE_COLOR_CHANNEL channel) {
	if(m_dib) {
		return FreeImage_AdjustCurve(m_dib, LUT, channel);
	}
	return FALSE;
}

BOOL fipImage::adjustGamma(double gamma) {
	if(m_dib) {
		return FreeImage_AdjustGamma(m_dib, gamma);
	}
	return FALSE;
}

BOOL fipImage::adjustBrightness(double percentage) {
	if(m_dib) {
		return FreeImage_AdjustBrightness(m_dib, percentage);
	}
	return FALSE;
}

BOOL fipImage::adjustContrast(double percentage) {
	if(m_dib) {
		return FreeImage_AdjustContrast(m_dib, percentage);
	}
	return FALSE;
}

BOOL fipImage::getHistogram(DWORD *histo, FREE_IMAGE_COLOR_CHANNEL channel) {
	if(m_dib) {
		return FreeImage_GetHistogram(m_dib, histo, channel);
	}
	return FALSE;
}

///////////////////////////////////////////////////////////////////
// Upsampling / downsampling routine

BOOL fipImage::rescale(WORD new_width, WORD new_height, FREE_IMAGE_FILTER filter) {
	FIBITMAP *src = NULL;
	if(m_dib) {
		// Convert to 32-bit
		if(FreeImage_GetBPP(m_dib) != 32) {
			if(!convertTo32Bits())
				return FALSE;
		}
		// Perform upsampling / downsampling
		FIBITMAP *dst = FreeImage_Rescale(m_dib, new_width, new_height, filter);
		return replace(dst);
	}
	return FALSE;
}

