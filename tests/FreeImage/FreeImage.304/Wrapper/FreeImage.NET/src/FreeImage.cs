// ==========================================================
// FreeImage.NET 3
//
// Design and implementation by
// - David Boland (davidboland@vodafone.ie)
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

using System;
using System.IO;
using System.Runtime.InteropServices;

namespace FreeImageAPI
{
	using PVOID = IntPtr;
	using FIBITMAP = Int32;
	using FIMULTIBITMAP = Int32;
	
	[StructLayout(LayoutKind.Sequential)]
	public class RGBQUAD
	{ 
		public int rgbBlue;
		public int rgbGreen;
		public int rgbRed;
		public int rgbReserved;
	}
	
/*	[StructLayout(LayoutKind.Sequential)]
	public class FreeImageIO
	{
		public FI_ReadProc readProc;
		public FI_WriteProc writeProc;
		public FI_SeekProc seekProc;
		public FI_TellProc tellProc;
	}
	
	[StructLayout(LayoutKind.Sequential)]
	public class FI_Handle
	{
		public FileStream stream;
	}
	public delegate void FI_ReadProc(IntPtr buffer, uint size, uint count, IntPtr handle);
	public delegate void FI_WriteProc(IntPtr buffer, uint size, uint count, IntPtr handle);
	public delegate int FI_SeekProc(IntPtr handle, long offset, int origin);
	public delegate int FI_TellProc(IntPtr handle);
	*/
	
	[StructLayout(LayoutKind.Sequential)]
	public class BITMAPINFOHEADER
	{
		public int size;
		public int width; 
		public int height; 
		public short biPlanes; 
		public short biBitCount;
		public int biCompression; 
		public int biSizeImage; 
		public int biXPelsPerMeter; 
		public int biYPelsPerMeter; 
		public int biClrUsed; 
		public int biClrImportant;
	}


	[StructLayout(LayoutKind.Sequential)]
	public class BITMAPINFO
	{
	  public BITMAPINFOHEADER bmiHeader; 
	  public RGBQUAD bmiColors;
	}

	public enum FIF
	{
		FIF_UNKNOWN = -1,
		FIF_BMP		= 0,
		FIF_ICO		= 1,
		FIF_JPEG	= 2,
		FIF_JNG		= 3,
		FIF_KOALA	= 4,
		FIF_LBM		= 5,
		FIF_IFF = FIF_LBM,
		FIF_MNG		= 6,
		FIF_PBM		= 7,
		FIF_PBMRAW	= 8,
		FIF_PCD		= 9,
		FIF_PCX		= 10,
		FIF_PGM		= 11,
		FIF_PGMRAW	= 12,
		FIF_PNG		= 13,
		FIF_PPM		= 14,
		FIF_PPMRAW	= 15,
		FIF_RAS		= 16,
		FIF_TARGA	= 17,
		FIF_TIFF	= 18,
		FIF_WBMP	= 19,
		FIF_PSD		= 20,
		FIF_CUT		= 21,
		FIF_XBM		= 22,
		FIF_XPM		= 23
	}
	
	public enum FI_QUANTIZE
	{
		FIQ_WUQUANT = 0,
    	FIQ_NNQUANT = 1		
	}
	
	public enum FI_DITHER
	{
	    FID_FS			= 0,
		FID_BAYER4x4	= 1,
		FID_BAYER8x8	= 2,
		FID_CLUSTER6x6	= 3,
		FID_CLUSTER8x8	= 4,
		FID_CLUSTER16x16= 5
	}
	
	public enum FI_FILTER
	{
		FILTER_BOX		  = 0,
		FILTER_BICUBIC	  = 1,
		FILTER_BILINEAR   = 2,
		FILTER_BSPLINE	  = 3,
		FILTER_CATMULLROM = 4,
		FILTER_LANCZOS3	  = 5
	}

	public enum FI_COLOR_CHANNEL
	{
		FICC_RGB	= 0,
		FICC_RED	= 1,
		FICC_GREEN	= 2,
		FICC_BLUE	= 3,
		FICC_ALPHA	= 4,
		FICC_BLACK	= 5
	}
	
	public delegate void FreeImage_OutputMessageFunction(FIF format, string msg);
	
	public class FreeImage
	{
		[DllImport("FreeImage.dll")]
		public static extern void FreeImage_Initialise(bool loadLocalPluginsOnly);
		
		[DllImport("FreeImage.dll")]
		public static extern void FreeImage_DeInitialise();
		
		[DllImport("FreeImage.dll")]
		public static extern string FreeImage_GetVersion();
		
		[DllImport("FreeImage.dll")]
		public static extern string FreeImage_GetCopyrightMessage();
		
		[DllImport("FreeImage.dll")]
		public static extern void FreeImage_SetOutputMessage(FreeImage_OutputMessageFunction omf);
		
		
		
		[DllImport("FreeImage.dll")]
		public static extern FIBITMAP FreeImage_Allocate(int width, int height, int bpp,
            uint red_mask, uint green_mask, uint blue_mask);
		
		[DllImport("FreeImage.dll")]
		public static extern FIBITMAP FreeImage_Load(FIF format, string filename, int flags);
		
		[DllImport("FreeImage.dll")]
		public static extern bool FreeImage_Save(
             FIF format, FIBITMAP dib, string filename, int flags);
		
		[DllImport("FreeImage.dll")]
		public static extern FIBITMAP FreeImage_Clone(FIBITMAP dib);

		[DllImport("FreeImage.dll")]
		public static extern void FreeImage_Unload(FIBITMAP dib);
		
		
		
		[DllImport("FreeImage.dll")]
		public static extern uint FreeImage_GetColorsUsed(FIBITMAP dib);
		
		[DllImport("FreeImage.dll")]
		public static extern IntPtr FreeImage_GetBits(FIBITMAP dib);
		
		[DllImport("FreeImage.dll")]
		public static extern IntPtr FreeImage_GetScanLine(FIBITMAP dib, int scanline);
		
		[DllImport("FreeImage.dll")]
		public static extern uint FreeImage_GetBPP(FIBITMAP dib);
		
		[DllImport("FreeImage.dll")]
		public static extern uint FreeImage_GetWidth(FIBITMAP dib);
		
		[DllImport("FreeImage.dll")]
		public static extern uint FreeImage_GetHeight(FIBITMAP dib);
		
		[DllImport("FreeImage.dll")]
		public static extern uint FreeImage_GetLine(FIBITMAP dib);
		
		[DllImport("FreeImage.dll")]
		public static extern uint FreeImage_GetPitch(FIBITMAP dib);
		
		[DllImport("FreeImage.dll")]
		public static extern uint FreeImage_GetDIBSize(FIBITMAP dib);
		
		[DllImport("FreeImage.dll")]
		[return: MarshalAs(UnmanagedType.LPStruct)]
		public static extern RGBQUAD FreeImage_GetPalette(FIBITMAP dib);
		
		[DllImport("FreeImage.dll")]
		public static extern uint FreeImage_GetDotsPerMeterX(FIBITMAP dib);
		
		[DllImport("FreeImage.dll")]
		public static extern uint FreeImage_GetDotsPerMeterY(FIBITMAP dib);
		
		[DllImport("FreeImage.dll")]
		[return: MarshalAs(UnmanagedType.LPStruct)]
		public static extern BITMAPINFOHEADER FreeImage_GetInfoHeader(FIBITMAP dib);
		
		[DllImport("FreeImage.dll")]
		[return: MarshalAs(UnmanagedType.LPStruct)]
		public static extern BITMAPINFO FreeImage_GetInfo(FIBITMAP dib);
		
		[DllImport("FreeImage.dll")]
		public static extern int FreeImage_GetColorType(FIBITMAP dib);
		
		[DllImport("FreeImage.dll")]
		public static extern uint FreeImage_GetRedMask(FIBITMAP dib);
		
		[DllImport("FreeImage.dll")]
		public static extern uint FreeImage_GetGreenMask(FIBITMAP dib);
		
		[DllImport("FreeImage.dll")]
		public static extern uint FreeImage_GetBlueMask(FIBITMAP dib);
		
		[DllImport("FreeImage.dll")]
		public static extern uint FreeImage_GetTransparencyCount(FIBITMAP dib);
		
		[DllImport("FreeImage.dll")]
		public static extern IntPtr FreeImage_GetTransparencyTable(FIBITMAP dib);
		
		[DllImport("FreeImage.dll")]
		public static extern void FreeImage_SetTransparent(FIBITMAP dib, bool enabled);
		
		[DllImport("FreeImage.dll")]
		public static extern bool FreeImage_IsTransparent(FIBITMAP dib);
		
		
		
		[DllImport("FreeImage.dll")]
		public static extern FIF FreeImage_GetFileType(string filename, int size);



		[DllImport("FreeImage.dll")]
		public static extern FIBITMAP FreeImage_ConvertTo8Bits(FIBITMAP dib);
		
		[DllImport("FreeImage.dll")]
		public static extern FIBITMAP FreeImage_ConvertTo16Bits555(FIBITMAP dib);
		
		[DllImport("FreeImage.dll")]
		public static extern FIBITMAP FreeImage_ConvertTo16Bits565(FIBITMAP dib);
		
		[DllImport("FreeImage.dll")]
		public static extern FIBITMAP FreeImage_ConvertTo24Bits(FIBITMAP dib);
		
		[DllImport("FreeImage.dll")]
		public static extern FIBITMAP FreeImage_ConvertTo32Bits(FIBITMAP dib);
		
		[DllImport("FreeImage.dll")]
		public static extern FIBITMAP FreeImage_ColorQuantize(FIBITMAP dib, FI_QUANTIZE quantize);
		
		[DllImport("FreeImage.dll")]
		public static extern FIBITMAP FreeImage_Threshold(FIBITMAP dib, uint t);
		
		[DllImport("FreeImage.dll")]
		public static extern FIBITMAP FreeImage_Dither(FIBITMAP dib, FI_DITHER algorithm);
		
		[DllImport("FreeImage.dll")]
		public static extern FIBITMAP FreeImage_ConvertFromRawBits(byte[] bits, int width, int height,
			int pitch, uint bpp, uint redMask, uint greenMask, uint blueMask, bool topDown);
		
		[DllImport("FreeImage.dll")]
		public static extern void FreeImage_ConvertToRawBits(IntPtr bits, FIBITMAP dib, int pitch,
			uint bpp, uint redMask, uint greenMask, uint blueMask, bool topDown);



		[DllImport("FreeImage.dll")]
		public static extern int FreeImage_GetFIFCount();
		
		[DllImport("FreeImage.dll")]
		public static extern int FreeImage_SetPluginEnabled(FIF format, bool enabled);
		
		[DllImport("FreeImage.dll")]
		public static extern int FreeImage_IsPluginEnabled(FIF format);
		
		[DllImport("FreeImage.dll")]
		public static extern FIF FreeImage_GetFIFFromFormat(string format);
		
		[DllImport("FreeImage.dll")]
		public static extern FIF FreeImage_GetFIFFromMime(string mime);
		
		[DllImport("FreeImage.dll")]
		public static extern string FreeImage_GetFormatFromFIF(FIF format);
		
		[DllImport("FreeImage.dll")]
		public static extern string FreeImage_GetFIFExtensionList(FIF format);
		
		[DllImport("FreeImage.dll")]
		public static extern string FreeImage_GetFIFDescription(FIF format);
		
		[DllImport("FreeImage.dll")]
		public static extern string FreeImage_GetFIFRegExpr(FIF format);
		
		[DllImport("FreeImage.dll")]
		public static extern FIF FreeImage_GetFIFFromFilename(string filename);
		
		[DllImport("FreeImage.dll")]
		public static extern bool FreeImage_FIFSupportsReading(FIF format);
		
		[DllImport("FreeImage.dll")]
		public static extern bool FreeImage_FIFSupportsWriting(FIF format);
		
		[DllImport("FreeImage.dll")]
		public static extern bool FreeImage_FIFSupportsExportBPP(FIF format, int bpp);
		
	

		[DllImport("FreeImage.dll")]
		public static extern FIMULTIBITMAP FreeImage_OpenMultiBitmap(
			FIF format, string filename, bool createNew, bool readOnly, bool keepCacheInMemory);
		
		[DllImport("FreeImage.dll")]
		public static extern long FreeImage_CloseMultiBitmap(FIMULTIBITMAP bitmap, int flags);
		
		[DllImport("FreeImage.dll")]
		public static extern int FreeImage_GetPageCount(FIMULTIBITMAP bitmap);
		
		[DllImport("FreeImage.dll")]
		public static extern void FreeImage_AppendPage(FIMULTIBITMAP bitmap, FIBITMAP data);
		
		[DllImport("FreeImage.dll")]
		public static extern void FreeImage_InsertPage(FIMULTIBITMAP bitmap, int page, FIBITMAP data);
		
		[DllImport("FreeImage.dll")]
		public static extern void FreeImage_DeletePage(FIMULTIBITMAP bitmap, int page);
		
		[DllImport("FreeImage.dll")]
		public static extern void FreeImage_LockPage(FIMULTIBITMAP bitmap, int page);
		
		[DllImport("FreeImage.dll")]
		public static extern void FreeImage_UnlockPage(FIMULTIBITMAP bitmap, int page, bool changed);
		
		[DllImport("FreeImage.dll")]
		public static extern void FreeImage_MovePage(FIMULTIBITMAP bitmap, int target, int source);
		
		[DllImport("FreeImage.dll")]
		public static extern bool FreeImage_GetLockedPageNumbers(FIMULTIBITMAP bitmap, IntPtr pages, IntPtr count);
		


		[DllImport("FreeImage.dll")]
		public static extern FIBITMAP FreeImage_RotateClassic(FIBITMAP dib, Double angle);
		
		[DllImport("FreeImage.dll")]
		public static extern FIBITMAP FreeImage_RotateEx(
			FIBITMAP dib, Double angle, Double xShift, Double yShift, Double xOrigin, Double yOrigin, bool useMask);
		
		[DllImport("FreeImage.dll")]
		public static extern bool FreeImage_FlipHorizontal(FIBITMAP dib);
		
		[DllImport("FreeImage.dll")]
		public static extern bool FreeImage_FlipVertical(FIBITMAP dib);
		
		
		
		[DllImport("FreeImage.dll")]
		public static extern FIBITMAP FreeImage_Rescale(FIBITMAP dib, int dst_width, int dst_height, FI_FILTER filter);
		
		
		
		[DllImport("FreeImage.dll")]
		public static extern bool FreeImage_AdjustCurve(FIBITMAP dib, byte[] lut, FI_COLOR_CHANNEL channel);
		
		[DllImport("FreeImage.dll")]
		public static extern bool FreeImage_AdjustGamma(FIBITMAP dib, Double gamma);
		
		[DllImport("FreeImage.dll")]
		public static extern bool FreeImage_AdjustBrightness(FIBITMAP dib, Double percentage);
		
		[DllImport("FreeImage.dll")]
		public static extern bool FreeImage_AdjustContrast(FIBITMAP dib, Double percentage);
		
		[DllImport("FreeImage.dll")]
		public static extern bool FreeImage_Invert(FIBITMAP dib);
		
		[DllImport("FreeImage.dll")]
		public static extern bool FreeImage_GetHistogram(FIBITMAP dib, int histo, FI_COLOR_CHANNEL channel);
		


		[DllImport("FreeImage.dll")]
		public static extern bool FreeImage_GetChannel(FIBITMAP dib, FI_COLOR_CHANNEL channel);
		
		[DllImport("FreeImage.dll")]
		public static extern bool FreeImage_SetChannel(FIBITMAP dib, FIBITMAP dib8, FI_COLOR_CHANNEL channel);
		
		
		
		[DllImport("FreeImage.dll")]
		public static extern FIBITMAP FreeImage_Copy(FIBITMAP dib, int left, int top, int right, int bottom);
		
		[DllImport("FreeImage.dll")]
		public static extern bool FreeImage_Paste(FIBITMAP dst, FIBITMAP src, int left, int top, int alpha);
	}
}
