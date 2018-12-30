#ifndef __MAX_EXPORT_ANIM__
#define __MAX_EXPORT_ANIM__

#include "export.h"

class INodeExport;

/////////////////////////////////////////////////////////////////////////////////////////////
///��ᯮ�� �����樮���� ���祩
/////////////////////////////////////////////////////////////////////////////////////////////
class   AnimKeysExport: public BaseExport
{
  public:
                AnimKeysExport (INodeExport&);

  private:
     void       ExportPosKeys   (Control&);
     void       ExportRotKeys   (Control&);
     void       ExportScaleKeys (Control&);

     bool       TestAnim        (Control*);
     bool       IsKnownController (Control*);
     bool       TestAnim        (bool&,bool&,bool&);

     template <class KeyType>
     class EnumKeys {
       public:  EnumKeys (IKeyControl&,const char* type,ostream&);
     };    

  private:
    INode&  node;
};

/////////////////////////////////////////////////////////////////////////////////////////////
///��ᯮ�� �����஢����� ��襩
/////////////////////////////////////////////////////////////////////////////////////////////
class  AnimMeshExport: public BaseExport
{
  public:
                AnimMeshExport (INodeExport&);

  private:
    INode&      node;                
};

#endif