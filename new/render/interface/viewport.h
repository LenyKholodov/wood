#ifndef __RENDER_VIEWPORT_CONTROL__
#define __RENDER_VIEWPORT_CONTROL__

#include <render\viewport.h>
#include <render\manager.h>

////////////////////////////////////////////////////////////////////////////////////////////
///�������� ������� ������
////////////////////////////////////////////////////////////////////////////////////////////
class ViewportCtrl: public RenderManager
{
  public:
////////////////////////////////////////////////////////////////////////////////////////////
///����
////////////////////////////////////////////////////////////////////////////////////////////
    virtual     const char* GetName  () const { return "Viewport control";  }
    virtual     int         GetType  () const { return -1; }
};

#endif