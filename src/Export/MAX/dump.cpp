#include <pch.h>

#include "dump.h"

ostream& operator << (ostream& os,const Color& p)
{
  return os<<PREC<<p.r<<"\t"<<PREC<<p.g<<"\t"<<PREC<<p.b;
}

ostream& operator << (ostream& os,const IKey& key)
{
  return os<<key.time;
} 

ostream& operator << (ostream& os,const ITCBKey& key)
{
  return os<<key.tens<<"\t"<<key.cont<<"\t"<<key.bias<<"\t"<<key.easeIn<<"\t"<<key.easeOut;
}

ostream& operator << (ostream& os,const ITCBPoint3Key& key)
{
  return os<<(IKey&)key<<"\t\t"<<(Point3&)key.val<<"\t\t"<<(ITCBKey&)key;
}

ostream& operator << (ostream& os,const ITCBRotKey& key)
{
  return os<<(IKey&)key<<"\t\t"<<key.val<<"\t\t"<<(ITCBKey&)key;
}

ostream& operator << (ostream& os,const ITCBScaleKey& key)
{
  return os<<(IKey&)key<<"\t\t"<<key.val<<"\t\t"<<(ITCBKey&)key;
}

ostream& operator << (ostream& os,const IBezPoint3Key& key)
{
  return os<<(IKey&)key<<"\t\t"<<key.val<<"\t\t"<<key.intan<<" "<<key.outtan<<" "<<key.flags;
}

ostream& operator << (ostream& os,const IBezScaleKey& key)
{
  return os<<(IKey&)key<<"\t\t"<<key.val<<"\t\t"<<key.intan<<"\t"<<key.outtan;
}

ostream& operator << (ostream& os,const IBezQuatKey& key)
{
  return os<<(IKey&)key<<"\t\t"<<key.val;
}

ostream& operator << (ostream& os,const ILinPoint3Key& key)
{
  return os<<(IKey&)key<<"\t\t"<<key.val;
}

ostream& operator << (ostream& os,const ILinRotKey& key)
{
  return os<<(IKey&)key<<"\t\t"<<key.val;
}

ostream& operator << (ostream& os,const ILinScaleKey& key)
{
  return os<<(IKey&)key<<"\t\t"<<key.val;
}

//Внимание! Объявлен в maxsdk\include\quat.h. Но реаизация не найдена
//При экспорте возможна ситуация с разной логикой операторов

ostream& operator << (ostream& os,const Quat& q)
{
  Point3 axis;
  float  angle;

  AngAxisFromQ (q,&angle,axis);

  return os<<axis<<"\t\t"<<PREC<<angle;
} 

ostream& operator << (ostream& os,const AngAxis& v)
{
  return os<<v.axis<<"\t"<<PREC<<v.angle;
}

ostream& operator << (ostream& os,const ScaleValue& s)
{
  return os<<s.s<<"\t"<<s.q;
}

ostream& operator << (ostream& os,const Point3& p)
{
//  return os<<PREC<<p.x<<"\t"<<PREC<<p.y<<"\t"<<PREC<<p.z;
  return os<<PREC<<-1.0f*p.x<<"\t"<<PREC<<p.z<<"\t"<<PREC<<p.y;
}


ostream& operator << (ostream& os,const Matrix3& m)
{
  static float d [4][3] = {
    {-1.0,0,0},
    {0,0,1},
    {0,1,0},
    {0,0,0}
  };
  static Matrix3 dm (d);
  static float   x [4] = {0,0,0,1};

  Matrix3 dumpMatrix = dm * m;
//  Matrix3 dumpMatrix = m;

  for (int i=0;i<3;i++)    
  {
    os<<"row\t\t";
    for (int j=0;j<4;j++)       
      os<<PREC<<dumpMatrix.GetRow (j)[i]<<"\t";
    os<<endline;  
  }  
  os<<"row\t\t0\t0\t0\t1"<<endline;
//    os<<PREC<<dumpMatrix.GetRow (i).x<<"\t"
//      <<PREC<<dumpMatrix.GetRow (i).y<<"\t"
//      <<PREC<<dumpMatrix.GetRow (i).z<<"\t"
//      <<PREC<<x [i]
//      <<endl;

  return os;
}
