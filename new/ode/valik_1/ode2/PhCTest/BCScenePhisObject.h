//--------------------------------------------------
// BCScenePhisObject - ...
//--------------------------------------------------
#pragma once

// phisic
#include "BPhisicTypes.h"

// ��������� � ������� ������
	
const int InfitesimalRotateMode=0,
          FiniteRotateMode=1;
enum  GravityMode { NoGravity=0,WorldGravity,PrivateGravity};

class BCScenePhisObject
{
private:	

		int		m_MaterialID;

		dMass	 m_Mass;
		dBodyID  m_Body;  // is Null for static objects
		Geoms	 m_Geoms;
		BodyBackInfo m_bbi;
		BCPhisComponentObject* m_PCOwner; // ����� ���� NULL (!)
		BCPhisicalScene*   m_PSOwner;

					// ����: ����� ��������� ����� �������� ����� � 0
		void* m_UserData;			// ������ �������� �������
public:
		BCScenePhisObject(BCPhisComponentObject*);
		BCScenePhisObject(BCPhisicalScene*);
		~BCScenePhisObject();

		void* GetUserData()		{ return m_UserData; }
		void  SetUserData(void* p)	{ m_UserData=p; }

		int   GetMaterialID()		{ return m_MaterialID; }
		void  SetMaterialID(int m)	{ m_MaterialID=m; }
//--- ������ ������� � �������(body): ��� ���������� ���� ���������� NULL
		dBodyID  GetBody()			{ return m_Body; }
		dWorldID GetWorld();
		dSpaceID GetSpace();
		BCPhisComponentObject* GetComponentObject()	{ return m_PCOwner; }
		BCPhisicalScene* GetScene();
// ��������� � ���������� ������� (��� �������� ���a return false
		bool SetRotation(const dMatrix4 R);
		bool GetRotation(dMatrix4 R);	// ��� ������� � ������� ����� 4(!) ���������� � ������

		bool SetPosition(float x,float y,float z);
		bool GetPosition(dVector3 p);
		
// ��������: �������� � �������
		bool SetLinearVel(float x, float y, float z);
		bool GetLinearVel(dVector3 v);

		bool SetAngularVel(float x, float y, float z);
		bool GetAngularVel(dVector3 v);
// ���� � ��������� �������. �.�.: ���� ������������ ����� �������� ���������
// �.�.� ��������� (Rel) ��������, ��� ���������� ������ ������������ �.�. ����
		bool SetForce(float fx,float fy,float fz);
		bool GetForce(dVector3 f);

		bool AddForce(float fx, float fy, float fz,bool Rel=false);
		bool AddForceAtPos(float fx,float fy, float fz,
					       float px,float py, float pz,
						   bool RelF=false,bool RelPos=false);
		bool SetTorque(float tx,float ty,float tz);
		bool GetTorque(dVector3 t);

		bool AddTorque(float tx, float ty, float tz,bool Rel=false);

// ����������� ������� - �����

		bool SetMass(const dMass* mass);
		bool SetMass(float);
		bool GetMass(dMass* mass);
		float GetMass();		

// ���������� ���� (������ �� ������� ������)
		bool BodyEnable();
		bool BodyDisable();
		bool IsEnabled() const;

// ���������� ��������� �������
		bool SetFiniteRotationMode(int mode);
		bool SetFiniteRotationAxis(float x, float y, float z);
		int  GetFiniteRotationMode() const;
		bool GetFiniteRotationAxis (dVector3 result);
		int  GetNumJoints();
		dJointID GetJoint(int index);
		bool IsConnectedTo(BCScenePhisObject& obj);

// ����������
		bool SetGravityMode(GravityMode mode);
		GravityMode GetGravityMode();
		bool SetGravity(float x,float y,float z);
		bool GetGravity(dVector3 g);
//--- 	������ ������� � ���������
//  �������� �������������� ��������, �������� ��������
//  �������� �� GetGeom ��� ���������� ������� ����������� �� �������������!
		Geoms& GetGeoms()	{ return m_Geoms; }
		bool AddGeom(dGeomID,bool Relative=false);
		
		bool AddGSphere(float r);
		bool AddGCappedCylinder(float r,float l);
		bool AddGCylinder(float r,float l);
		bool AddGBox(float lx,float ly,float lz);

		bool AddGSphere(float r,float x,float y,float z,dMatrix4 R);
		bool AddGCappedCylinder(float r,float l,float x,float y,float z,dMatrix4 R);
		bool AddGCylinder(float r,float l,float x,float y,float z,dMatrix4 R);
		bool AddGBox(float lx,float ly,float lz,float x,float y,float z,dMatrix4 R);

		bool GetDrawData(int index,DrawGeomInfo* dgi);

		void Draw();
};

//-----------------------------------------------------------------------------------
const dMass* MassSetSphere(float density,float r);
const dMass* MassSetCappedCylinder(dReal density,int direction,float r,float l);
const dMass* MassSetCylinder(dReal density,int direction,float r,float l);
const dMass* MassSetBox(float density,float lx,float ly,float lz);
//-----------------------------------------------------------------------------------

