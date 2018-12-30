#include "zlib\zlib.h"

typedef voidp gzFile;

gzFile  gzopen  OF((const char *path, const char *mode));

gzFile  gzdopen  OF((int fd, const char *mode));

int  gzsetparams OF((gzFile file, int level, int strategy));
int     gzread  OF((gzFile file, voidp buf, unsigned len));

int     gzwrite OF((gzFile file, 
                                 const voidp buf, unsigned len));

int     gzflush OF((gzFile file, int flush));

z_off_t     gzseek OF((gzFile file,
                                    z_off_t offset, int whence));

int     gzrewind OF((gzFile file));

z_off_t     gztell OF((gzFile file));

int  gzeof OF((gzFile file));

int     gzclose OF((gzFile file));

const char *  gzerror OF((gzFile file, int *errnum));
