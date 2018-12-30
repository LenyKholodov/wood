// ==========================================================
// fipWinImage class implementation
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
#include <math.h>

#ifdef WIN32

///////////////////////////////////////////////////////////////////
// Copying

HANDLE fipWinImage::copyToHandle() {
	HANDLE hMem = NULL;

	if(m_dib) {

		// Get equivalent DIB size
		int dib_size = sizeof(BITMAPINFOHEADER);
		dib_size += FreeImage_GetColorsUsed(m_dib) * sizeof(RGBQUAD);
		dib_size += FreeImage_GetPitch(m_dib) * FreeImage_GetHeight(m_dib);

		// Allocate a DIB
		hMem = GlobalAlloc(GHND, dib_size);
		BYTE *dib = (BYTE*)GlobalLock(hMem);

		memset(dib, 0, dib_size);

		BYTE *p_dib = (BYTE*)dib;

		// Copy the BITMAPINFOHEADER

		BITMAPINFOHEADER *bih = FreeImage_GetInfoHeader(m_dib);
		memcpy(p_dib, bih, sizeof(BITMAPINFOHEADER));
		p_dib += sizeof(BITMAPINFOHEADER);

		// Copy the palette

		RGBQUAD *pal = FreeImage_GetPalette(m_dib);
		memcpy(p_dib, pal, FreeImage_GetColorsUsed(m_dib) * sizeof(RGBQUAD));
		p_dib += FreeImage_GetColorsUsed(m_dib) * sizeof(RGBQUAD);

		// Copy the bitmap

		BYTE *bits = FreeImage_GetBits(m_dib);
		memcpy(p_dib, bits, FreeImage_GetPitch(m_dib) * FreeImage_GetHeight(m_dib));

		GlobalUnlock(hMem);
	}

	return hMem;
}

void fipWinImage::copyFromHandle(HANDLE hMem) {
	// Get a pointer to the bitmap
	BYTE *lpVoid = (BYTE *)GlobalLock(hMem);
	// Get a pointer to the bitmap header
	BITMAPINFOHEADER *pHead = (BITMAPINFOHEADER *)lpVoid;
	// Get a pointer to the palette
	RGBQUAD *pPalette = NULL;
	if(pHead->biBitCount < 16)
		pPalette = (RGBQUAD *)(((BYTE *)pHead) + sizeof(BITMAPINFOHEADER));
	// Get a pointer to the pixels
	BYTE *bits = ((BYTE*)pHead + sizeof(BITMAPINFOHEADER) + sizeof(RGBQUAD) * pHead->biClrUsed);

	if(lpVoid) {

		// Allocate a new FIBITMAP
		if(!setSize((WORD)pHead->biWidth, (WORD)pHead->biHeight, pHead->biBitCount)) {
			GlobalUnlock(lpVoid);
			return;
		}

		// Copy the bitmap header
		memcpy(FreeImage_GetInfoHeader(m_dib), pHead, sizeof(BITMAPINFOHEADER));


		// Copy the palette
		memcpy(FreeImage_GetPalette(m_dib), pPalette, pHead->biClrUsed * sizeof(RGBQUAD));

		// Copy the bitmap
		memcpy(FreeImage_GetBits(m_dib), bits, FreeImage_GetPitch(m_dib) * FreeImage_GetHeight(m_dib));

		GlobalUnlock(lpVoid);
	}
}

void fipWinImage::copyFromBitmap(HBITMAP hbmp) {
	if(hbmp) { 
        BITMAP bm;
		// get informations about the bitmap
        GetObject(hbmp, sizeof(BITMAP), (LPSTR) &bm);
		// create the image
        setSize((WORD)bm.bmWidth, (WORD)bm.bmHeight, (WORD)bm.bmBitsPixel);
		// create a device context for the bitmap
        HDC dc = GetDC(NULL);
		// copy the pixels
		int Success = GetDIBits(dc,							// handle to DC
								hbmp,						// handle to bitmap
								0,							// first scan line to set
								FreeImage_GetHeight(m_dib),	// number of scan lines to copy
								FreeImage_GetBits(m_dib),	// array for bitmap bits
								FreeImage_GetInfo(m_dib),	// bitmap data buffer
								DIB_RGB_COLORS				// RGB 
								);
		if(Success == 0) {
			// Error : GetDIBits failed
			ReleaseDC(NULL, dc);
			return;
        }
        ReleaseDC(NULL, dc);
    }
}


///////////////////////////////////////////////////////////////////
// Painting operations

void fipWinImage::draw(HDC hDC, RECT& rcDest) {
	// Draw the dib
	SetStretchBltMode(hDC, COLORONCOLOR);	
	StretchDIBits(hDC, rcDest.left, rcDest.top, 
		rcDest.right-rcDest.left, rcDest.bottom-rcDest.top, 
		0, 0, FreeImage_GetWidth(m_dib), FreeImage_GetHeight(m_dib),
		FreeImage_GetBits(m_dib), FreeImage_GetInfo(m_dib), DIB_RGB_COLORS, SRCCOPY);
}


#endif // WIN32
