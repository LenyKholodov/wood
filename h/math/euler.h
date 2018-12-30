#ifndef __MATH3D__EULER_ANGLES__
#define __MATH3D__EULER_ANGLES__

#include <types.h>

template <class Type> class QuaternionTmpl;

#ifndef M_PI
#define M_PI  3.14159265358979323846f
#endif

#define Deg2Rad(a) ((a) * (M_PI / 180.0f))
#define Rad2Deg(a) ((a) * (180.0f / M_PI))

/////////////////////////////////////////////////////////////////////////////////////////////
///���� �����
///��������! ���������� � �ࠤ��� (�����᪨)
/////////////////////////////////////////////////////////////////////////////////////////////
struct EulerAngle
{
  public:
        float pitch,  //⠭��� (����� �� X)
              yaw,    //��᪠��� (����� �� Y)
              bank;   //�७ (����� �� Z)

  public:
                        EulerAngle ();
                        EulerAngle (float,float,float);
                        EulerAngle (QuaternionTmpl<float>&);

    EulerAngle&         operator = (const QuaternionTmpl<float>& q);
};

#endif