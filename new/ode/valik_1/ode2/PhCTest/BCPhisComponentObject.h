//--------------------------------------------------
// BCPhisComponentObject - объект содержащий массив простых физ. тел
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
	UINT32			signature;	// сигнатура физ состояния
	UINT32			size;		// размер данных (без учета заголовка!!!)
	PhisObjectState		obj[0];		// массив данных объектов
};


class BCPhisComponentObject
{
private:

	BCScenePhisObject* m_RootObject;
	BCScenePhisObjects m_SObjects;
	dSpaceID	   m_ObjSpace;
	dJoints            m_JointHinges,  	// ось, или шарнир
			   m_JointSliders,      // ползунок
			   m_JointHinges2;	// управляемая ось (как в передних колесах авто)
	BCPhisicalScene*   m_PSOwner;
	void* m_UserData;			// данные игрового объекта
public:
	BCPhisComponentObject(BCPhisicalScene* pso);
	~BCPhisComponentObject();

	void* GetUserData()				 { return m_UserData; }
	void  SetUserData(void* p)			 { m_UserData=p; }
// "мировые" методы.
	dWorldID 	 GetWorld();
	BCPhisicalScene* GetScene();
	dSpaceID	 GetSpace(); // возвращает "свое" пространство
// задание центрового объекта
	BCScenePhisObject* GetRootObject()			{ return m_RootObject; }
	void		 SetRootObject(BCScenePhisObject* ro)	{ m_RootObject=ro; }

// методы глобального свойства. 
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

// определение динамического состояния объекта
// Re: первые поля структуры - идентификатор и размер
	size_t GetStateDataSize();
	bool   GetState(void*);
	bool   SetState(const void*);

// Простые объекты
	BCScenePhisObjects& GetObjects()		{ return m_SObjects; }
//---------------------------------------------------------------------------------------------------------------------
// Работа со связями .... 
	dJoints& GetHinges()			{ return m_JointHinges;  }
	dJoints& GetSliders()			{ return m_JointSliders; }
	dJoints& GetHinges2()			{ return m_JointHinges2; }

// return -1 если объект не создан
// если obj2==-1 происходит привязка на NULL, т.е к статичной среде
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
// поиск объектов и связей, в случае отсутсвия return -1
	int	 FindHinge(dJointID);
	int	 FindSlider(dJointID);
	int	 FindHinge2(dJointID);
	int	 FindObject(BCScenePhisObject* obj);
	int	 FindObject(dBodyID);

	void	 Draw();
};

