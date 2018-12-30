#ifndef __ODE_ADVANCED_MISC__
#define __ODE_ADVANCED_MISC__

inline void vector_copy3(dVector3 a,const dVector3 b) 
		{ a[0]=b[0]; a[1]=b[1]; a[2]=b[2]; }

inline void vector_copy3C(dVector3 a,const dReal* b) 
		{ a[0]=b[0]; a[1]=b[4]; a[2]=b[8]; }

inline void vector_neg3(dVector3 a,const dVector3 b) 
		{ a[0]=-b[0]; a[1]=-b[1]; a[2]=-b[2]; }

inline void vector_neg3(dVector3 a) { vector_neg3(a,a); }

inline void vector_add3(dVector3 a,const dVector3 b) 
		{ a[0]+=b[0]; a[1]+=b[1]; a[2]+=b[2]; }

inline void vector_add3(dVector3 c,const dVector3 a,const dVector3 b) 
		{ c[0]=a[0]+b[0]; c[1]=a[1]+b[1]; c[2]=a[2]+b[2]; }

inline void vector_add3mul(dVector3 c,const dVector3 a,const dVector3 b,dReal q) 
		{ c[0]=(a[0]+b[0])*q; c[1]=(a[1]+b[1])*q; c[2]=(a[2]+b[2])*q; }

inline void vector_add3mul2(dVector3 c,const dVector3 a,const dVector3 b,dReal q) 
		{ c[0]=a[0]+b[0]*q; c[1]=a[1]+b[1]*q; c[2]=a[2]+b[2]*q; }

inline void vector_add3mul(dVector3 a,const dVector3 b,dReal q) 
		{ a[0]+=b[0]*q; a[1]+=b[1]*q; a[2]+=b[2]*q; }

inline void vector_sub3(dVector3 c,const dVector3 a,const dVector3 b) 
		{ c[0]=a[0]-b[0]; c[1]=a[1]-b[1]; c[2]=a[2]-b[2]; }

inline void vector_sub3(dVector3 a,const dVector3 b)
		{ a[0]-=b[0]; a[1]-=b[1]; a[2]-=b[2]; }

inline void vector_sub3Cmul(dVector3 a,const dReal* b,dReal q)
		{ a[0]-=b[0]*q; a[1]-=b[4]*q; a[2]-=b[8]*q; }

inline void vector_sub3Cmul(dVector3 a,const dVector3 a0,const dReal* b,dReal q)
		{ a[0]=a0[0]-b[0]*q; a[1]=a0[1]-b[4]*q; a[2]=a0[2]-b[8]*q; }

inline void vector_add3Cmul(dVector3 a,const dReal* b,dReal q)
		{ a[0]+=b[0]*q; a[1]+=b[4]*q; a[2]+=b[8]*q; }

inline void vector_add3Cmul(dVector3 a,const dVector3 a0,const dReal* b,dReal q)
		{ a[0]=a0[0]+b[0]*q; a[1]=a0[1]+b[4]*q; a[2]=a0[2]+b[8]*q; }

inline void vector_mul3(dVector3 a,dReal b) 
		{ a[0]*=b; a[1]*=b; a[2]*=b; }

inline void vector_mul3(dVector3 a,const dVector3 b,dReal q) 
		{ a[0]=b[0]*q; a[1]=b[1]*q; a[2]=b[2]*q; }

inline void vector_mul3Csub(dVector3 a,dReal k,const dReal* R,const dVector3 b)
		{ a[0]=k*R[0]-b[0]; a[1]=k*R[4]-b[1]; a[2]=k*R[8]-b[2]; }

#define CONTACT(p,skip) ((dContactGeom*) (((char*)p) + (skip)))

template <class T> inline T sqr(T x) { return x*x; }

template <class T> inline void swap(T& x,T& y)	{ T t=x; x=y; y=t; }

inline void MatrixTrans3(dMatrix3 m)
{
	swap(m[0*4+1],m[1*4+0]);
	swap(m[0*4+2],m[2*4+0]);
	swap(m[1*4+2],m[2*4+1]);
}

#endif
