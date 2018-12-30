// ==========================================================
// FreeImagePlus 3
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

#ifndef FREEIMAGEPLUS_H
#define FREEIMAGEPLUS_H

#ifdef WIN32
#include <windows.h>
#endif // WIN32
#include "FreeImage.h"


// Compiler options ---------------------------------------------------------

#if defined(FREEIMAGE_LIB) || !defined(WIN32)
#define FIP_API 
#else
// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the FIP_EXPORTS
// symbol defined on the command line. this symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// FIP_API functions as being imported from a DLL, wheras this DLL sees symbols
// defined with this macro as being exported.
#ifdef FIP_EXPORTS
#define FIP_API __declspec(dllexport)
#else
#define FIP_API __declspec(dllimport)
#endif // FIP_EXPORTS
#endif // FREEIMAGE_LIB || !WIN32

///////////////////////////////////////////////////////////////////////////////////////////

/**@name Useful constants */
//@{
/// White value (= 255)
const BYTE WHITE = 255;
/// Black value (= 0)
const BYTE BLACK = 0;
//@}

// ----------------------------------------------------------

/** Abstract base class for all image types used by the library.
	@version FreeImage 3.0
	@author Hervé Drolon
*/

class FIP_API fipGenericImage
{
  protected:
	/// Image width
	WORD m_Width;
	/// Image height
	WORD m_Height;
	/// Length of a line (in bytes) rounded to 32-bit
	WORD m_ScanWidth;

  public:

	/**@name Accessors */
	//@{
	/// Returns the image width in pixels
	WORD getWidth()		{ return m_Width; }
	/// Returns the image height in pixels
	WORD getHeight()	{ return m_Height; }
	/// Returns the width of the bitmap in bytes rounded to the nearest DWORD.
	WORD getScanWidth()	{ return m_ScanWidth; }
	//@}

	/**@name Information functions */
	//@{
	/// Returns TRUE if the image is allocated, FALSE otherwise
	virtual BOOL isValid() = 0;
	//@}
};

// ----------------------------------------------------------

/** A class used to manage all photo related images.

	fipImage encapsulates the FIBITMAP format. It relies on the FreeImage library, especially for 
	loading / saving images and for bit depth conversion.
	@version FreeImage 3.0
	@author Hervé Drolon
*/

class FIP_API fipImage : public fipGenericImage
{
protected:
	/// DIB data
	FIBITMAP *m_dib;
	/// Pointer to DIB pixels
	BYTE *m_bits;

public:

	/**@name Creation & Destruction */
	//@{	
	/// Constructor
	fipImage(WORD width = 0, WORD height = 0, WORD bpp = 0);
	/// Destructor
	~fipImage();
	/// Image allocator
	BOOL setSize(WORD width, WORD height, WORD bpp);
	//@}

	/**@name Copying */
	//@{	
	/// Copy constructor
	fipImage(const fipImage& src);
	/// Copy constructor
	fipImage(const FIBITMAP *dib);
	/// Copy constructor
	fipImage& operator=(const fipImage& src);
	/// Assignement operator
	fipImage& operator=(FIBITMAP *dib);


	/// Copy a sub part of the current image and returns it as a fipImage object.
	/**
	The bit depth of the bitmap must be equal to 1, 4, 8, 16, 24 or 32.
	@param dst Output subimage
	@param left Specifies the left position of the cropped rectangle. 
	@param top Specifies the top position of the cropped rectangle. 
	@param right Specifies the right position of the cropped rectangle. 
	@param bottom Specifies the bottom position of the cropped rectangle. 
	@return Returns TRUE if successful, FALSE otherwise.
	*/
	BOOL copySubImage(fipImage& dst, int left, int top, int right, int bottom);

	/**
	@brief Alpha blend or combine a sub part image with the current image.

    The bit depth of dst bitmap must be greater than or equal to the bit depth of src. 
	Upper promotion of src is done internally. Supported bit depth equals to 8, 16, 24 or 32.
	@param src Source subimage
	@param left Specifies the left position of the sub image. 
	@param top Specifies the top position of the sub image. 
	@param alpha Alpha blend factor. The source and destination images are alpha blended if 
	alpha = 0..255. If alpha > 255, then the source image is combined to the destination image.
	@return Returns TRUE if successful, FALSE otherwise.
	*/
	BOOL pasteSubImage(fipImage& src, int left, int top, int alpha = 256);

	//@}

	/** @name Loading & Saving
	 * Loading and saving is handled by the FreeImage library.
	 */
	//@{	
	/// Loads an image from disk, given its file name and an optional flag.
	/**
	@param lpszPathName Path and file name of the image to load.
	@param flag The signification of this flag depends on the image to be read.
	@return Returns TRUE if successful, FALSE otherwise.
	@see FreeImage.h, FreeImage documentation
	*/
	BOOL load(const char* lpszPathName, int flag = 0);

	/// Loads an image using the specified FreeImageIO struct and fi_handle, and an optional flag.
	/**
	@param io FreeImageIO structure
	@param handle FreeImage fi_handle
	@param flag The signification of this flag depends on the image to be read.
	@return Returns TRUE if successful, FALSE otherwise.
	@see FreeImage.h, FreeImage documentation
	*/
	BOOL loadFromHandle(FreeImageIO *io, fi_handle handle, int flag = 0);

	/// Saves an image to disk, given its file name and an optional flag.
	/**
	@param lpszPathName Path and file name of the image to save.
	@param flag The signification of this flag depends on the image to be saved.
	@return Returns TRUE if successful, FALSE otherwise.
	@see FreeImage.h, FreeImage documentation
	*/
	BOOL save(const char* lpszPathName, int flag = 0);

	/// Saves an image using the specified FreeImageIO struct and fi_handle, and an optional flag.
	/**
	@param fif Format identifier (FreeImage format)
	@param io FreeImageIO structure
	@param handle FreeImage fi_handle
	@param flag The signification of this flag depends on the image to be saved.
	@return Returns TRUE if successful, FALSE otherwise.
	@see FreeImage.h, FreeImage documentation
	*/
	BOOL saveToHandle(FREE_IMAGE_FORMAT fif, FreeImageIO *io, fi_handle handle, int flag = 0);
	//@}

	/**	@name Information functions
	 *  Accessors to the DIB BITMAPINFO structure.
	 */
	//@{	
	/// Returns a pointer to the FIBITMAP data. Used for direct access from FREEIMAGE functions.
	FIBITMAP* getFIBITMAP();

	/// Returns TRUE if the image is allocated, FALSE otherwise
	BOOL  isValid();

	/// Returns a pointer to the bitmap's BITMAPINFO header. 
	BITMAPINFO* getInfo();

	/// Returns a pointer to the bitmap's BITMAPINFOHEADER. 
    BITMAPINFOHEADER* getInfoHeader();

	/// Returns the size of the bitmap in bytes. The size of the bitmap is the BITMAPINFOHEADER + the size of the palette + the size of the bitmap data. 
	LONG getImageSize();
	
	/**
	Returns the bitdepth of the bitmap. <br>
	Valid bitdepth can be 1, 4, 8, 16, 24 or 32.
	*/
	WORD getBitsPerPixel();

	/// Returns the width of the bitmap in bytes. 
	WORD getLine();

	/// Returns the bitmap resolution along the X axis, in pixels / cm
	WORD getHorizontalResolution();
	
	/// Returns the bitmap resolution along the Y axis, in pixels / cm
	WORD getVerticalResolution();

	/// set the bitmap resolution along the X axis, in pixels / cm
	void setHorizontalResolution(LONG value);
	
	/// set the bitmap resolution along the Y axis, in pixels / cm
	void setVerticalResolution(LONG value);
	//@}

	/**@name Palette operations */
	//@{
	/// Returns a pointer to the bitmap's palette. If the bitmap doesn't have a palette, getPalette returns NULL. 
	RGBQUAD* getPalette();
	
	/// Returns the palette size in bytes.
	WORD getPaletteSize();

	/// Retrieves the number of colours used in the bitmap. If the bitmap is non-palletised, 0 is returned. 
	WORD getColorsUsed();

	/** @brief Investigates the colour type of the bitmap.
		@see FREE_IMAGE_COLOR_TYPE
	*/
	FREE_IMAGE_COLOR_TYPE getColorType();

	/// Returns TRUE if the bitmap is a 8-bit bitmap with a greyscale palette, FALSE otherwise
	BOOL isGrayscale();
	//@}

	/**@name Pixel access */
	//@{	

	/** @brief Returns a pointer to the bitmap bits.

	It is up to you to interpret these bytes correctly, 
	according to the results of FreeImage_GetBPP and 
	GetRedMask, FreeImage_GetGreenMask and FreeImage_GetBlueMask.
	@see FreeImage_GetBits
	*/
	BYTE* accessPixels();

	/** Returns a pointer to the start of the given scanline in the bitmap’s data-bits.
	@see FreeImage_GetScanLine
	*/
	BYTE *getScanLine(int scanline);

	//@}

	/**	@name Conversion routines
	 *  Bitmaps are always loaded in their default bit depth. If you want the bitmap to be stored in another bit depth, the class provides several conversion functions.
	 */
	//@{	
	/** @brief Converts the bitmap to 1 bit using a threshold T.
		@param T Threshold value in [0..255]
		@return Returns TRUE if successfull, FALSE otherwise. 
	*/
	BOOL threshold(BYTE T);
	
	/** @brief Converts the bitmap to 8 bits. If the bitmap is 24 or 32-bit RGB, the colour values are converted to greyscale.
		@return Returns TRUE if successfull, FALSE otherwise. 
	*/
	BOOL convertTo8Bits();

	/** @brief Converts the bitmap to 16 bits. The resulting bitmap has a layout of 5 bits red, 5 bits green, 5 bits blue and 1 unused bit. 
		@return Returns TRUE if successfull, FALSE otherwise. 
	*/
	BOOL convertTo16Bits555();
	
	/** @brief Converts the bitmap to 16 bits. The resulting bitmap has a layout of 5 bits red, 6 bits green and 5 bits blue. 
		@return Returns TRUE if successfull, FALSE otherwise. 
	*/
	BOOL convertTo16Bits565();
	
	/** @brief Converts the bitmap to 24 bits. 
		@return Returns TRUE if successfull, FALSE otherwise. 
	*/
	BOOL convertTo24Bits();
	
	/** @brief Converts the bitmap to 32 bits. 
		@return Returns TRUE if successfull, FALSE otherwise. 
	*/
	BOOL convertTo32Bits();

	/** @brief Converts the bitmap to 8 bits. 
		
		For palletized bitmaps, the color map is converted to a greyscale ramp.
		@see convertTo8Bits
		@return Returns TRUE if successfull, FALSE otherwise. 
	*/
	BOOL convertToGrayscale();
	
	/** @brief Quantizes a full colour 24-bit bitmap to a palletised 8-bit bitmap.
	
	    The quantize parameter specifies which colour reduction algorithm should be used.
		@param algorithm Color quantization algorithm to use.
		@return Returns TRUE if successfull, FALSE otherwise. 
		@see FREE_IMAGE_QUANTIZE
	*/
	BOOL colorQuantize(FREE_IMAGE_QUANTIZE algorithm);

	/** @brief Converts a 8-bit image to a monochrome image using a dithering algorithm.

		@param algorithm Dithering algorithm to use.
		@return Returns TRUE if successfull, FALSE otherwise. 
		@see FREE_IMAGE_DITHER
	*/
	BOOL dither(FREE_IMAGE_DITHER algorithm);
	//@}

	/**@name Channel processing support */
	//@{	
	/** @brief Retrieves the red, green, blue or alpha channel of a 24- or 32-bit BGR[A] image. 
	@param image Output image to be extracted
	@param channel Color channel to extract
	@return Returns TRUE if successful, FALSE otherwise.
	@see FREE_IMAGE_COLOR_CHANNEL
	*/
	BOOL getChannel(fipImage& image, FREE_IMAGE_COLOR_CHANNEL channel);

	/** @brief Split a 24-bit RGB image into 3 greyscale images corresponding to the red, green and blue channels.
	@param RedChannel Output red channel.
	@param GreenChannel Output green channel.
	@param BlueChannel Output blue channel.
	@return Returns FALSE if the dib isn't a valid image, if it's not a 24-bit image or if 
	one of the output channel can't be allocated. Returns TRUE otherwise.
	*/
	BOOL splitChannels(fipImage& RedChannel, fipImage& GreenChannel, fipImage& BlueChannel);

	/** @brief Retrieves the red channel of a 24-bit RGB image.
	@param red Output red channel.
	@return Returns TRUE if successful, FALSE otherwise.
	*/
	BOOL getRedChannel(fipImage& red);
	
	/** @brief Retrieves the green channel of a 24-bit RGB image.
	@param green Output green channel.
	@return Returns TRUE if successful, FALSE otherwise.
	*/
	BOOL getGreenChannel(fipImage& green);
	
	/** @brief Retrieves the blue channel of a 24-bit RGB image.
	@param blue Output blue channel.
	@return Returns TRUE if successful, FALSE otherwise.
	*/
	BOOL getBlueChannel(fipImage& blue);

	/** @brief Retrieves the alpha channel of a 32-bit RGB image.
	@param alpha Output alpha channel.
	@return Returns TRUE if successful, FALSE otherwise.
	*/
	BOOL getAlphaChannel(fipImage& alpha);

	/** @brief Set the alpha channel of a 32-bit RGB image.
	@param alpha Input alpha channel.
	@return Returns TRUE if successful, FALSE otherwise.
	*/
	BOOL setAlphaChannel(fipImage& alpha);

	/** @brief Builds a 24-bit RGB image given its red, green and blue channel.
	@param RedChannel Input red channel.
	@param GreenChannel Input green channel.
	@param BlueChannel Input blue channel.
	@return Returns FALSE if the dib can't be allocated, if the input channels are not 8-bit images. Returns TRUE otherwise.
	*/
	BOOL combineChannels(fipImage& red, fipImage& green, fipImage& blue);
	//@}

	/**@name Rotation and flipping */
	//@{	
	/** @brief Image translation and rotation using B-Splines.

	@param angle Image rotation angle, in degree
	@param x_shift Image horizontal shift
	@param y_shift Image vertical shift
	@param x_origin Origin of the x-axis
	@param y_origin Origin of the y-axis
	@param spline_degree Degree of the B-spline model
	@param use_mask Whether or not to mask the image. Image mirroring is applied when use_mask is set to FALSE
	@return Returns the translated & rotated dib if successful, returns NULL otherwise
	*/
	BOOL rotateEx(double angle, double x_shift, double y_shift, double x_origin, double y_origin, BOOL use_mask);

	/** @brief Image rotation by means of three shears.

	@param angle Image rotation angle, in degree
	@return Returns the translated & rotated dib if successful, returns NULL otherwise
	*/
	BOOL rotate(double angle);

	/// Flip the image horizontally along the vertical axis
	BOOL flipHorizontal();

	/// Flip the image vertically along the horizontal axis
	BOOL flipVertical();
	//@}

	/**@name Color manipulation routines */
	//@{	
	/** @brief Inverts each pixel data.
	@return Returns TRUE if successful, FALSE otherwise.
	*/
	BOOL invert();
	
	/** @brief Perfoms an histogram transformation on a 8, 24 or 32-bit image 
	according to the values of a lookup table (LUT).

	The transformation is done as follows.<br>
	Image 8-bit : if the image has a color palette, the LUT is applied to this palette, 
	otherwise, it is applied to the grey values.<br>
	Image 24-bit & 32-bit : if channel == IPL_CC_RGB, the same LUT is applied to each color
	plane (R,G, and B). Otherwise, the LUT is applied to the specified channel only.
	@param LUT Lookup table. <b>The size of 'LUT' is assumed to be 256.</b>
	@param channel The color channel to be processed (only used with 24 & 32-bit DIB).
	@return Returns TRUE if the operation was successful, FALSE otherwise
	@see FREE_IMAGE_COLOR_CHANNEL
	*/
	BOOL adjustCurve(BYTE *LUT, FREE_IMAGE_COLOR_CHANNEL channel);

	/** @brief Performs gamma correction on a 8, 24 or 32-bit image.
	@param gamma Gamma value to use. A value of 1.0 leaves the image alone, 
	less than one darkens it, and greater than one lightens it.
	@return Returns TRUE if the operation was successful, FALSE otherwise
	@see adjustCurve
	*/
	BOOL adjustGamma(double gamma);

	/** @brief Adjusts the brightness of a 8, 24 or 32-bit image by a certain amount.
	@param percentage Where -100 <= percentage <= 100<br>
	A value 0 means no change, less than 0 will make the image darker 
	and greater than 0 will make the image brighter.
	@return Returns TRUE if the operation was succesful, FALSE otherwise
	@see adjustCurve
	*/
	BOOL adjustBrightness(double percentage);

	/** @brief Adjusts the contrast of a 8, 24 or 32-bit image by a certain amount.
	@param percentage Where -100 <= percentage <= 100<br>
	A value 0 means no change, less than 0 will decrease the contrast 
	and greater than 0 will increase the contrast of the image.
	@return Returns TRUE if the operation was succesfull, FALSE otherwise
	@see adjustCurve
	*/
	BOOL adjustContrast(double percentage);

	/** @brief Computes image histogram
	
	For 24-bit and 32-bit images, histogram can be computed from red, green, blue and 
	black channels. For 8-bit images, histogram is computed from the black channel. Other 
	bit depth is not supported.
	@param histo pointer to an histogram array. <b>Size of this array is assumed to be 256</b>.
	@param channel Color channel to use
	@return Returns TRUE if the operation was succesfull, FALSE otherwise
	*/
	BOOL getHistogram(DWORD *histo, FREE_IMAGE_COLOR_CHANNEL channel = FICC_BLACK);
	//@}

	/**@name Upsampling / downsampling */
	//@{	

	/** @brief Rescale the image to a new width / height.

	@param new_width New image width
	@param new_height New image height
	@param filter The filter parameter specifies which resampling filter should be used.
	@return Returns TRUE if the operation was successful, FALSE otherwise
	@see FREE_IMAGE_FILTER
	*/
	BOOL rescale(WORD new_width, WORD new_height, FREE_IMAGE_FILTER filter);
	//@}


  protected:
	/**@name Internal use */
	//@{
	  void updateInfo();
	  BOOL replace(FIBITMAP *new_dib);
	  BOOL replace(fipImage& img) {
		  return replace(img.getFIBITMAP());
	  }
	//@}

};

// ----------------------------------------------------------

/** A class designed for MS Windows (TM) platforms.

    fipWinImage provides methods used to :
	<ul>
	<li>Display a DIB on the screen
	<li>Copy / Paste a DIB to/from Windows devices (HANDLE, HBITMAP, Clipboard)
	</ul>
	@version FreeImage 3.0
	@author Hervé Drolon
*/
#ifdef WIN32

class FIP_API fipWinImage : public fipImage
{
public:
	/**@name Creation & Destruction */
	//@{	
	/// Constructor
	fipWinImage(WORD width = 0, WORD height = 0, WORD bpp = 0) : fipImage(width, height, bpp) { }

	/// Destructor
	~fipWinImage() { }
	//@}

	/**@name Copying */
	//@{	
	/// Copy constructor
	fipWinImage(const fipImage& Img) : fipImage(Img) { }

	/// Copy constructor
	fipWinImage(const FIBITMAP *dib) : fipImage(dib) { }

	/** @brief Clone function used for clipboard copy.

	Convert the FIBITMAP image to a DIB, 
	and transfer the DIB in a global bitmap handle.
	*/
	HANDLE copyToHandle();

	/** @brief Copy constructor used for clipboard paste.

	Converts a global object to a FIBITMAP. The clipboard format must be CF_DIB.
	*/
	void copyFromHandle(HANDLE hMem);

	/** @brief Copy constructor

	Converts a HBITMAP object to a FIBITMAP.
	*/
	void copyFromBitmap(HBITMAP hbmp);

	//@}

	/**@name Painting operations */
	//@{	

	/** @brief Draw (stretch) the image on a HDC, using StretchDIBits.

	@param hDC Handle to the device context
	@param rcDest Destination rectangle
	*/
	void draw(HDC hDC, RECT& rcDest);

	//@}
};

#endif // WIN32

#endif	// FREEIMAGEPLUS_H
