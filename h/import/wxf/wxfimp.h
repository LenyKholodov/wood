#ifndef __WXF_IMPORT__
#define __WXF_IMPORT__

#include <resource\resource.h>
#include <import\wxf\wxfmtl.h>
#include <import\wxf\wxfnode.h>

/////////////////////////////////////////////////////////////////////////////////////////////
///������ ������ ������ �� 䠩�� WXF
/////////////////////////////////////////////////////////////////////////////////////////////
class    WXFImport: public Resource
{
  friend class WXFBase;
  public:                        
                        WXFImport  (const char* file_name,ResourceGroup* = NULL);
         virtual        ~WXFImport ();

/////////////////////////////////////////////////////////////////////////////////////////////
///����� � ���ਠ����
/////////////////////////////////////////////////////////////////////////////////////////////
              WXFMaterial*      GetMtl  (const char*) const;
              void              AddMtl  (const char*,WXFMaterial*);
              void              DelMtl  (const char*);

/////////////////////////////////////////////////////////////////////////////////////////////
///����� � 㧫���
/////////////////////////////////////////////////////////////////////////////////////////////
              WXFNode*          GetNode (const char*) const;
              WXFNode*          root    () const { return mRoot; }

              void              AddNode (const char*,WXFNode*);
              void              DelNode (const char*);

  protected:
    virtual  bool       load   ();
    virtual  void       unload ();                        

  private:
        Str128                    mFileName;

        WXFNode*                  mRoot;
        StringDict<WXFNode*>*     mNodeDict;
        StringDict<WXFMaterial*>* mMatDict;
};

#endif