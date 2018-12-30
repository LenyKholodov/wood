#include <ode/addons/advShapes.h>

// define ripped from ode\src\mass.cpp
#define _I(i,j) I[(i)*4+(j)]

void dMassSetCylinder (dMass *m, dReal density, int direction,dReal r, dReal l)
{
  dReal M1,Ia,Ib;
  dAASSERT (m);
  dUASSERT (direction >= 1 && direction <= 3,"bad direction number");
  dMassSetZero (m);
  M1 = M_PI*r*r*l*density;		// cylinder mass
  m->mass = M1;
  Ia = M1*(REAL(0.25)*r*r + (REAL(1.0)/REAL(12.0))*l*l);
  Ib = M1*REAL(0.5)*r*r;
  m->_I(0,0) = Ia;
  m->_I(1,1) = Ia;
  m->_I(2,2) = Ia;
  m->_I(direction-1,direction-1) = Ib;
//no good!
//# ifndef dNODEBUG
//  checkMass (m);
//#endif
}
