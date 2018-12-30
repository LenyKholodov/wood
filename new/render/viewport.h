#ifndef __RENDER_VIEWPORT_DEFINE__
#define __RENDER_VIEWPORT_DEFINE__

#include <render\context.h>
#include <render\camera.h>
#include <render\proj.h>

/*
  ������� ��������� ����� ��������������� ������� ������, �� ������� ��� �������� ����
  � �������� ��������� (�����, ���������). �� ��������� ��������������� ������� ������
  ����� 1000x1000
*/

////////////////////////////////////////////////////////////////////////////////////////////
///����� ��������������� �� ������� ���������
////////////////////////////////////////////////////////////////////////////////////////////
enum ViewportFlags {
  VP_ENABLE_CLIP_RECT   = 1,    //������������ ���������
//  VP_CHANGE_CLIP_RECT   = 2,    //���������� ������� ���������
//  VP_CHANGE_TRANSFORM   = 4,    //���������� �������������
//  VP_CHANGE_VIEWPORT    = 8,    //���������� ������� ���������
  VP_CHANGE_TOTAL       = 16    //���������� ������� ������������� 
};

/////////////////////////////////////////////////////////////////////////////////////////////
///������� ���������
/////////////////////////////////////////////////////////////////////////////////////////////
class    Viewport: public RenderData
{
  friend class Render;
  public:
                            Viewport       ();
                            ~Viewport      ();

/////////////////////////////////////////////////////////////////////////////////////////////
///������
/////////////////////////////////////////////////////////////////////////////////////////////
                Camera*     GetCamera      ();
                Projection& GetProj        ();

/////////////////////////////////////////////////////////////////////////////////////////////
///������� ������� ��������������
/////////////////////////////////////////////////////////////////////////////////////////////
                Matrix&     GetTransform   ();

/////////////////////////////////////////////////////////////////////////////////////////////
///������������� ������
/////////////////////////////////////////////////////////////////////////////////////////////
                void        BindCamera     (Camera&);
                void        UnbindCamera   ();

/////////////////////////////////////////////////////////////////////////////////////////////
///��������� ������� ����������� (������)
/////////////////////////////////////////////////////////////////////////////////////////////
                void        SetViewport    (int left,int bottom,int width,int height); 
                void        GetViewport    (int& left,int& bottom,int& width,int& height) const;

                void        SetViewport    (int width,int height);

                int         width          () const;
                int         height         () const;
                int         left           () const;
                int         bottom         () const;

/////////////////////////////////////////////////////////////////////////////////////////////
///��������� ������� ���������
/////////////////////////////////////////////////////////////////////////////////////////////
                void        SetClipRect    (int left,int bottom,int width,int height);                
                void        GetClipRect    (int& left,int& bottom,int& width,int& height) const;

                void        EnableClip     ();
                void        DisableClip    ();

                bool        GetClipState   () const;
                void        SetClipState   (bool);

////////////////////////////////////////////////////////////////////////////////////////////
///������� ������ (���������������, �����������)
////////////////////////////////////////////////////////////////////////////////////////////
                void        SetScreenSize      (int,int);
                void        GetScreenSize      (int&,int&) const;

/////////////////////////////////////////////////////////////////////////////////////////////
///�������������� ��� ��������� ����������
/////////////////////////////////////////////////////////////////////////////////////////////
                void        Move           (int x_offs,int y_offs);
                void        Scale          (float x_scale,float y_scale);

  private:
/////////////////////////////////////////////////////////////////////////////////////////////
///����� viewport'� � �������� OpenGL
/////////////////////////////////////////////////////////////////////////////////////////////
    virtual     bool        SetCurrent      ();

  private:
    int         mViewWidth, mViewHeight, mViewLeft, mViewBottom;
    int         mClipWidth, mClipHeight, mClipLeft, mClipBottom;
    int         mScreenWidth, mScreenHeight;
    Flags32     mFlags;

    Camera*     mCamera;
    Projection  mProj;
    Matrix      mTotal;
};

inline int Viewport::width () const
{
  return mViewWidth;
}

inline int Viewport::height () const
{
  return mViewHeight;
}

inline int Viewport::left () const
{
  return mViewLeft;
}

inline int Viewport::bottom () const
{
  return mViewBottom;
}

inline Camera* Viewport::GetCamera ()
{
  return mCamera;
}

inline Projection&  Viewport::GetProj ()
{
  return mProj;
}

inline void Viewport::GetScreenSize (int& width,int& height) const
{
  width  = mScreenWidth;
  height = mScreenHeight;
}

#endif