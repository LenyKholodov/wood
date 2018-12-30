/*
 * Warning, this file was automatically created by the TIFF configure script
 * VERSION:	 v3.6.0
 * RELEASE:   
 * DATE:	 dim oct 26 19:14:14 CET 2003
 * TARGET:	 i686-pc-linux-gnu
 * CCOMPILER:	 /usr/bin/gcc-3.2.2-5)
 */
#ifndef _PORT_
#define _PORT_ 1
#ifdef __cplusplus
extern "C" {
#endif
#include <sys/types.h>
#define HOST_FILLORDER FILLORDER_MSB2LSB
#define HOST_BIGENDIAN	0
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
typedef double dblparam_t;
#ifdef __STRICT_ANSI__
#define	INLINE	__inline__
#else
#define	INLINE	inline
#endif
#define GLOBALDATA(TYPE,NAME)	extern TYPE NAME
#ifdef __cplusplus
}
#endif
#endif
