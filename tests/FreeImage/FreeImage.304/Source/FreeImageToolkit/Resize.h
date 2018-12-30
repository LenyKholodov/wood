// ==========================================================
// Upsampling / downsampling classes
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

#ifndef _RESIZE_H_
#define _RESIZE_H_

#include "FreeImage.h"
#include "Utilities.h"
#include "Filters.h" 

/**
  Filter weights table.<br>
  This class stores contribution information for an entire line (row or column).
*/
class CWeightsTable
{
/** 
  Sampled filter weight table.<br>
  Contribution information for a single pixel
*/
typedef struct {
	/// Normalized weights of neighboring pixels
	double *Weights;
	/// Bounds of source pixels window
	int Left, Right;   
} Contribution;  

public:
	/// Row (or column) of contribution weights 
	Contribution *m_WeightTable;
	/// Filter window size (of affecting source pixels) 
	DWORD m_WindowSize;
	/// Length of line (no. or rows / cols) 
	DWORD m_LineLength;

public:
	/** 
	Constructor<br>
	Allocate and compute the weights table
	@param pFilter Filter used for upsampling or downsampling
	@param uLineSize Length (in pixels) of the destination line buffer
	@param uSrcSize Length (in pixels) of the source line buffer
	@param dScale Scale factor (should be equal to uDstSize / uSrcSize)
	*/
	CWeightsTable(CGenericFilter *pFilter, DWORD uDstSize, DWORD uSrcSize, double dScale) {
		DWORD u;
		double dWidth;
		double dFScale = 1.0;
		double dFilterWidth = pFilter->GetWidth();

		if(dScale < 1.0) {
			// Minification
			dWidth = dFilterWidth / dScale; 
			dFScale = dScale; 
		} else {
			// Magnification
			dWidth= dFilterWidth; 
		}
 
		// Allocate a new line contributions structure
		//
		// Window size is the number of sampled pixels
		m_WindowSize = 2 * (int)ceil(dWidth) + 1; 
		m_LineLength = uDstSize; 
		 // Allocate list of contributions 
		m_WeightTable = new Contribution[m_LineLength];
		for(u = 0 ; u < m_LineLength ; u++) {
			// Allocate contributions for every pixel
			m_WeightTable[u].Weights = new double[m_WindowSize];
		}

 
		for(u = 0; u < m_LineLength; u++) {
			// Scan through line of contributions
			double dCenter = (double)u / dScale;   // Reverse mapping
			// Find the significant edge points that affect the pixel
			int iLeft = MAX (0, (int)floor (dCenter - dWidth)); 
			int iRight = MIN ((int)ceil (dCenter + dWidth), int(uSrcSize) - 1); 
 
			// Cut edge points to fit in filter window in case of spill-off
			if((iRight - iLeft + 1) > int(m_WindowSize)) {
				if(iLeft < (int(uSrcSize) - 1 / 2)) {
					iLeft++; 
				} else {
					iRight--; 
				}
			}

			m_WeightTable[u].Left = iLeft; 
			m_WeightTable[u].Right = iRight;

			int iSrc = 0;
			double dTotalWeight = 0.0;  // Zero sum of weights
			for(iSrc = iLeft; iSrc <= iRight; iSrc++) {
				// Calculate weights
				double weight = dFScale * pFilter->Filter(dFScale * (dCenter - (double)iSrc));
				m_WeightTable[u].Weights[iSrc-iLeft] = weight;
				dTotalWeight += weight;
			}
			if((dTotalWeight > 0) && (dTotalWeight != 1)) {
				// Normalize weight of neighbouring points
				for(iSrc = iLeft; iSrc <= iRight; iSrc++) {
					// Normalize point
					m_WeightTable[u].Weights[iSrc-iLeft] /= dTotalWeight; 
				}
			}
	   } 
	}

	/**
	Destructor<br>
	Destroy the weights table
	*/
	~CWeightsTable() {
		for(DWORD u = 0; u < m_LineLength; u++) {
			// Free contributions for every pixel
			delete [] m_WeightTable[u].Weights;
		}
		// Free list of pixels contributions
		delete [] m_WeightTable;
	}

	/** Retrieve a filter weight, given source and destination positions
	@param dst_pos Pixel position in destination line buffer
	@param src_pos Pixel position in source line buffer
	@return Returns the filter weight
	*/
	double GetWeight(int dst_pos, int src_pos) {
		return m_WeightTable[dst_pos].Weights[src_pos];
	}

	/** Retrieve left boundary of source line buffer
	@param dst_pos Pixel position in destination line buffer
	@return Returns the left boundary of source line buffer
	*/
	int GetLeftBoundary(int dst_pos) {
		return m_WeightTable[dst_pos].Left;
	}

	/** Retrieve right boundary of source line buffer
	@param dst_pos Pixel position in destination line buffer
	@return Returns the right boundary of source line buffer
	*/
	int GetRightBoundary(int dst_pos) {
		return m_WeightTable[dst_pos].Right;
	}
};

// ---------------------------------------------

/**
 CResizeEngine<br>
 This class performs filtered zoom. It scales an image to the desired dimensions with 
 any of the CGenericFilter derived filter class.<br>
 It only works with 32-bit buffers of RGBQUAD type (i.e. color images whose color model follows 
 the ARGB convention).<br><br>

 <b>References</b> : <br>
 [1] Paul Heckbert, C code to zoom raster images up or down, with nice filtering. 
 UC Berkeley, August 1989. [online] http://www-2.cs.cmu.edu/afs/cs.cmu.edu/Web/People/ph/heckbert.html
 [2] Eran Yariv, Two Pass Scaling using Filters. The Code Project, December 1999. 
 [online] http://www.codeproject.com/bitmap/2_pass_scaling.asp

*/
class CResizeEngine
{
private:
	/// Pointer to the FIR / IIR filter
	CGenericFilter* m_pFilter;

public:

    /// Constructor
	CResizeEngine(CGenericFilter* filter):m_pFilter(filter) {}

    /// Destructor
	virtual ~CResizeEngine() {}

    /** Scale an image to the desired dimensions
	@param pOrigImage Pointer to the 32-bit source image pixels
	@param uOrigWidth Source image width
	@param uOrigHeight Source image height
	@param pDstImage Pointer to the 32-bit destination image pixels
	@param uNewWidth Destination image width
	@param uNewHeight Destination image height
	*/
	void Scale(RGBQUAD *pOrigImage, DWORD uOrigWidth, DWORD uOrigHeight, RGBQUAD *pDstImage, DWORD uNewWidth, DWORD uNewHeight) { 
		// decide which filtering order (xy or yx) is faster for this mapping by
		// counting convolution multiplies

		if(uNewWidth*uOrigHeight <= uNewHeight*uOrigWidth) {
			// xy filtering
			//
			// scale source image horizontally into temporary image
			RGBQUAD *pTemp = new RGBQUAD[uNewWidth * uOrigHeight];
			HorizontalFilter(pOrigImage, uOrigWidth, uOrigHeight, pTemp, uNewWidth, uOrigHeight);
			// scale temporary image vertically into result image    
			VerticalFilter(pTemp, uNewWidth, uOrigHeight, pDstImage, uNewWidth, uNewHeight);
			delete [] pTemp;
		} else {
			// yx filtering
			//
			// scale source image vertically into temporary image
			RGBQUAD *pTemp = new RGBQUAD[uOrigWidth * uNewHeight];
			VerticalFilter(pOrigImage, uOrigWidth, uOrigHeight, pTemp, uOrigWidth, uNewHeight);

			// scale temporary image horizontally into result image    
			HorizontalFilter(pTemp, uOrigWidth, uNewHeight, pDstImage, uNewWidth, uNewHeight);
			delete [] pTemp;
		}
	} 


private:

    /// Performs horizontal image filtering
	void HorizontalFilter(RGBQUAD *pSrc, DWORD uSrcWidth, DWORD uSrcHeight, RGBQUAD *pDst, DWORD uDstWidth, DWORD uDstHeight) { 
		if(uDstWidth == uSrcWidth) {
			// no scaling required, just copy
			memcpy (pDst, pSrc, sizeof (RGBQUAD) * uSrcHeight * uSrcWidth);
		}
		// allocate and calculate the contributions
		CWeightsTable weightsTable(m_pFilter, uDstWidth, uSrcWidth, double(uDstWidth) / double(uSrcWidth)); 

		// step through rows            
		for(DWORD uRow = 0; uRow < uDstHeight; uRow++) {
			// scale each row 
			RGBQUAD *pSrcRow = &(pSrc[uRow * uSrcWidth]);
			RGBQUAD *pDstRow = &(pDst[uRow * uDstWidth]);
			for(DWORD x = 0; x < uDstWidth; x++) {
				// loop through row
				double r = 0;
				double g = 0;
				double b = 0;
				double a = 0;
				int iLeft = weightsTable.GetLeftBoundary(x);    // retrieve left boundary
				int iRight = weightsTable.GetRightBoundary(x);  // retrieve right boundary
				for(int i = iLeft; i <= iRight; i++) {
					// scan between boundaries
					// accumulate weighted effect of each neighboring pixel
					double weight = weightsTable.GetWeight(x, i-iLeft);
					r += (weight * (double)(pSrcRow[i].rgbRed)); 
					g += (weight * (double)(pSrcRow[i].rgbGreen)); 
					b += (weight * (double)(pSrcRow[i].rgbBlue)); 
					a += (weight * (double)(pSrcRow[i].rgbReserved)); 
				} 
				// place result in destination pixel
				pDstRow[x].rgbRed	= (BYTE)MIN(MAX((int)0, (int)(r + 0.5)), (int)255);
				pDstRow[x].rgbGreen = (BYTE)MIN(MAX((int)0, (int)(g + 0.5)), (int)255);
				pDstRow[x].rgbBlue	= (BYTE)MIN(MAX((int)0, (int)(b + 0.5)), (int)255);
				pDstRow[x].rgbReserved = (BYTE)MIN(MAX((int)0, (int)(a + 0.5)), (int)255);
			} 
		}
	} 

    /// Performs vertical image filtering
    void VerticalFilter(RGBQUAD *pSrc, DWORD uSrcWidth, DWORD uSrcHeight, RGBQUAD *pDst, DWORD uDstWidth, DWORD uDstHeight) { 
		if(uSrcHeight == uDstHeight) {
			// no scaling required, just copy
			memcpy (pDst, pSrc, sizeof (RGBQUAD) * uSrcHeight * uSrcWidth);
		}
		// allocate and calculate the contributions
		CWeightsTable weightsTable(m_pFilter, uDstHeight, uSrcHeight, double(uDstHeight) / double(uSrcHeight)); 

		// step through columns
		for(DWORD uCol = 0; uCol < uDstWidth; uCol++) {
			// scale each column
			for(DWORD y = 0; y < uDstHeight; y++) {
				// loop through column
				double r = 0;
				double g = 0;
				double b = 0;
				double a = 0;
				int iLeft = weightsTable.GetLeftBoundary(y);    // retrieve left boundary
				int iRight = weightsTable.GetRightBoundary(y);  // retrieve right boundary

				for(int i = iLeft; i <= iRight; i++) {
					// scan between boundaries
					// accumulate weighted effect of each neighboring pixel
					RGBQUAD pCurSrc = pSrc[i * uSrcWidth + uCol];
					double weight = weightsTable.GetWeight(y, i-iLeft);
					r += (weight * (double)(pCurSrc.rgbRed)); 
					g += (weight * (double)(pCurSrc.rgbGreen)); 
					b += (weight * (double)(pCurSrc.rgbBlue)); 
					a += (weight * (double)(pCurSrc.rgbReserved)); 
				}
				// clamp and place result in destination pixel
				int index = y * uDstWidth + uCol;
				pDst[index].rgbRed	 = (BYTE)MIN(MAX((int)0, (int)(r + 0.5)), (int)255);
				pDst[index].rgbGreen = (BYTE)MIN(MAX((int)0, (int)(g + 0.5)), (int)255);
				pDst[index].rgbBlue	 = (BYTE)MIN(MAX((int)0, (int)(b + 0.5)), (int)255);
				pDst[index].rgbReserved = (BYTE)MIN(MAX((int)0, (int)(a + 0.5)), (int)255);
			}
		}
	} 
};



#endif //   _RESIZE_H_
