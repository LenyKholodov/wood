// ==========================================================
// Load From Handle Example
//
// Design and implementation by Floris van den Berg
//
// This file is part of FreeImage 3
//
// Use at own risk!
// ==========================================================
//
//   This example shows how to load a bitmap from a
//   user allocated FILE pointer.
//
// ==========================================================

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "FreeImage.h"

// ----------------------------------------------------------

unsigned _stdcall
_ReadProc(void *buffer, unsigned size, unsigned count, fi_handle handle) {
	return fread(buffer, size, count, (FILE *)handle);
}

unsigned _stdcall
_WriteProc(void *buffer, unsigned size, unsigned count, fi_handle handle) {
	return fwrite(buffer, size, count, (FILE *)handle);
}

int _stdcall
_SeekProc(fi_handle handle, long offset, int origin) {
	return fseek((FILE *)handle, offset, origin);
}

long _stdcall
_TellProc(fi_handle handle) {
	return ftell((FILE *)handle);
}

// ----------------------------------------------------------

int __cdecl
main(int argc, char *argv[]) {
	FreeImageIO io;

	io.read_proc = _ReadProc;
	io.write_proc = _WriteProc;
	io.seek_proc = _SeekProc;
	io.tell_proc = _TellProc;

	FILE *file = fopen("d:\\images\\money-256.bmp", "rb");

	if (file != NULL) {
		// find the buffer format (say its a bmp file)
		FREE_IMAGE_FORMAT fif = FIF_BMP;
		// load from the file handle
		FIBITMAP *dib = FreeImage_LoadFromHandle(fif, &io, (fi_handle)file, BMP_DEFAULT);
		// free the loaded FIBITMAP
		FreeImage_Unload(dib);

		fclose(file);
	}

	return 0;
}