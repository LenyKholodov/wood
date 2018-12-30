#ifndef __DISK_PAK_BINFILE_EMUL__
#define __DISK_PAK_BINFILE_EMUL__

#include <binfile\packfile.h>

/////////////////////////////////////////////////////////////////////////////////////////////
///Ёмул€ци€ пака дл€ дискового устройства
/////////////////////////////////////////////////////////////////////////////////////////////
class  DiskPackFile: public PackFile
{
  public:
                   DiskPackFile  (Pool* = Memory::M_GetPool(),size_t = 0);
        virtual    ~DiskPackFile ();

        virtual BinFile*        open  (const char*,int);

  protected:
        virtual SearchFile*     _OpenSearch (const char*);
};

#endif