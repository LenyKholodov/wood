//--------------------------------------------------
// BCPhisComponentObject - ������ ���������� ������ ������� ���. ���
//--------------------------------------------------

#pragma once

#include "BPhisicTypes.h"
#include "BCScenePhisObject.h"
#include "BCPhisicalScene.h"

struct PhisObjectState
{
	dVector3 position;
	dMatrix4 rotation;
	dVector3 LinearVel,AngularVel;
};

struct PhisComponentState
{
	UINT32			signature;	// ��������� ��� ���������
	UINT32			size;		// ������ ������ (��� ����� ���������!!!)
	PhisObjectState		obj[0];		// ������ ������ ��������
};


class BCPhisComponentObject
{
private:

	BCScenePhisObject* m_RootObject;
	BCScenePhisObjects m_SObjects;
	dSpaceID	   m_ObjSpace;
	dJoints            m_JointHinges,  	// ���, ��� ������
			   m_JointSliders,      // ��������
			   m_JointHinges2;	// ����������� ��� (��� � �������� ������� ����)
	BCPhisicalScene*   m_PSOwner;
	void* m_UserData;			// ������ �������� �������
public:
	BCPhisComponentObject(BCPhisicalScene* pso);
	~BCPhisComponentObject();

	void* GetUserData()				 { return m_UserData; }
	void  SetUserData(void* p)			 { m_UserData=p; }
// "�������" ������.
	dWorldID 	 GetWorld();
	BCPhisicalScene* GetScene();
	dSpaceID	 GetSpace(); // ���������� "����" ������������
// ������� ���������� �������
	BCScenePhisObject* GetRootObject()			{ return m_RootObject; }
	void		 SetRootObject(BCScenePhisObject* ro)	{ m_RootObject=ro; }

// ������ ����������� ��������. 
	bool SetRotation(const dMatrix4 R);
	bool GetRotation(dMatrix4 );
	bool SetPosition(float x,float y,float z);
	bool GetPosition(dVector3 p);
		
	bool SetLinearVel(float x, float y, float z);
	
	bool SetForce(float fx,float fy,float fz);

	bool AddForce(float fx, float fy, float fz);

	bool SetGravityMode(GravityMode mode);
	GravityMode GetGravityMode();
	bool SetGravity(float x,float y,float z);

// ����������� ������������� ��������� �������
// Re: ������ ���� ��������� - ������������� � ������
	size_t GetStateDataSize();
	bool   GetState(void*);
	bool   SetState(const void*);

// ������� �������
	BCScenePhisObjects& GetObjects()		{ return m_SObjects; }
//---------------------------------------------------------------------------------------------------------------------
// ������ �� ������� .... 
	dJoints& GetHinges()			{ return m_JointHinges;  }
	dJoints& GetSliders()			{ return m_JointSliders; }
	dJoints& GetHinges2()			{ return m_JointHinges2; }

// return -1 ���� ������ �� ������
// ���� obj2==-1 ���������� �������� �� NULL, �.� � ��������� �����
// ------ hinge
	int	 CreateHinge(int obj1,int obj2,const dVector3 anchor,const dVector3 axis);

	bool	 GetHingeAnchor(int index,dVector3 anchor);
	bool	 SetHingeAnchor(int index,const dVector3 anchor);
	bool	 GetHingeAxis(int index,dVector3 axis);
	bool	 SetHingeAxis(int index,const dVector3 axis);

	float	 GetHingeAngle(int index);
	float	 GetHingeAngleRate(int index);

	bool	 SetHingeParam(int index,int parameter,float value);
	float	 GetHingeParam(int index,int parameter);

// ------ hinge 2
	int	 CreateHinge2(int obj1,int obj2,const dVector3 anchor,const dVector3 axis,const dVector3 axis2);

	bool	 GetHinge2Anchor(int index,dVector3 anchor);
	bool	 SetHinge2Anchor(int index,const dVector3 anchor);
	bool	 GetHinge2Axis1(int index,dVector3 axis);
	bool	 SetHinge2Axis1(int index,const dVector3 axis);
	bool	 GetHinge2Axis2(int index,dVector3 axis);
	bool	 SetHinge2Axis2(int index,const dVector3 axis);

	float	 GetHinge2Angle1(int index);
	float	 GetHinge2Angle1Rate(int index);
	float	 GetHinge2Angle2Rate(int index);

	bool	 SetHinge2Param(int index,int parameter,float value);
	float	 GetHinge2Param(int index,int parameter);

// ------ slider
	int	 CreateSlider(int obj1,int obj2,const dVector3 axis);
	bool	 GetSliderAxis(int index,dVector3 axis);
	bool	 SetSliderAxis(int index,const dVector3 axis);
	float	 GetSliderPosition(int index);
	float	 GetSliderPositionRate(int index);

	bool	 SetSliderParam(int index,int parameter,float value);
	float	 GetSliderParam(int index,int parameter);

//---------------------------------------------------------------------------------------------------------------------
// ����� �������� � ������, � ������ ��������� return -1
	int	 FindHinge(dJointID);
	int	 FindSlider(dJointID);
	int	 FindHinge2(dJointID);
	int	 FindObject(BCScenePhisObject* obj);
	int	 FindObject(dBodyID);

	void	 Draw();
};

