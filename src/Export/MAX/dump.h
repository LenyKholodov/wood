#ifndef __MAX_DUMP_PRIMITIVES__
#define __MAX_DUMP_PRIMITIVES__

#include "defs.h"

/////////////////////////////////////////////////////////////////////////////////////////////
///Далее идёт вывод базовых структур MAX'а в поток
/////////////////////////////////////////////////////////////////////////////////////////////

ostream& operator << (ostream&,const Color&);
ostream& operator << (ostream&,const Point3&);
ostream& operator << (ostream&,const Matrix3&);
//ostream& operator << (ostream&,const Quat&);
ostream& operator << (ostream&,const AngAxis&);
ostream& operator << (ostream&,const ScaleValue&);

//Для анимации

ostream& operator << (ostream&,const IKey&);
ostream& operator << (ostream&,const ITCBKey&);
ostream& operator << (ostream&,const ITCBPoint3Key&);
ostream& operator << (ostream&,const ITCBRotKey&);
ostream& operator << (ostream&,const ITCBScaleKey&);
ostream& operator << (ostream&,const IBezPoint3Key&);
ostream& operator << (ostream&,const IBezScaleKey&);
ostream& operator << (ostream&,const IBezQuatKey&);
ostream& operator << (ostream&,const ILinPoint3Key&);
ostream& operator << (ostream&,const ILinRotKey&);
ostream& operator << (ostream&,const ILinScaleKey&);

#endif