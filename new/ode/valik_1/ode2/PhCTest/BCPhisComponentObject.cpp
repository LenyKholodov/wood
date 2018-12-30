//--------------------------------------------------
// BCPhisComponentObject - объект содержащий массив простых физ. тел
//--------------------------------------------------

#include "BCPhisComponentObject.h"

BCPhisComponentObject::BCPhisComponentObject(BCPhisicalScene* pso):
			m_RootObject(NULL),
			m_ObjSpace(NULL),
			m_PSOwner(pso),
			m_UserData(NULL)
{
	m_ObjSpace=dSimpleSpaceCreate(GetScene()->GetSpace());//dHashSpaceCreate(GetScene()->GetSpace());
	GetScene()->GetObjects().push_back(this);
}

BCPhisComponentObject::~BCPhisComponentObject()
{
	for(dJoints::iterator j=m_JointHinges.begin();j!=m_JointHinges.end();j++)
		dJointDestroy(*j);
	m_JointHinges.clear();
	for(j=m_JointHinges2.begin();j!=m_JointHinges2.end();j++)
		dJointDestroy(*j);
	m_JointHinges2.clear();
	for(j=m_JointSliders.begin();j!=m_JointSliders.end();j++)
		dJointDestroy(*j);
	m_JointSliders.clear();
	for(BCScenePhisObjects::iterator po=m_SObjects.begin();po!=m_SObjects.end();po++)
		delete *po;
	if(m_ObjSpace)
	{
		dSpaceDestroy(m_ObjSpace);
		m_ObjSpace=NULL;
	}
	m_PSOwner=NULL;
}

dWorldID BCPhisComponentObject::GetWorld()
{
	return m_PSOwner?m_PSOwner->GetWorld():NULL;
}

BCPhisicalScene* BCPhisComponentObject::GetScene()
{
	return m_PSOwner;
}

dSpaceID BCPhisComponentObject::GetSpace()
{
	return m_ObjSpace?m_ObjSpace:m_PSOwner?m_PSOwner->GetSpace():NULL;
}


bool BCPhisComponentObject::SetRotation(const dMatrix3 R)
{
	if(!m_RootObject)
		return false;
	dMatrix3 Rnew,Rt;
	GetRotation(Rt);
	dMULTIPLY1_333(Rnew,Rt,R);
	dVector3 RootPos;
	GetPosition(RootPos);
	for(BCScenePhisObjects::iterator po=m_SObjects.begin();po!=m_SObjects.end();po++)
		if(*po!=m_RootObject)
		{
		 	dVector3 popos,ponewpos;
			(*po)->GetPosition(popos);
			vector_sub3(popos,RootPos);
			dMULTIPLY0_133(ponewpos,popos,Rnew);
			dMatrix3 poR,newpoR;
			(*po)->GetRotation(poR);
			dMULTIPLY0_333(newpoR,poR,Rnew);
			(*po)->SetRotation(newpoR);
			vector_add3(ponewpos,RootPos);
			(*po)->SetPosition(ponewpos[0],ponewpos[1],ponewpos[2]);
		}	
	m_RootObject->SetRotation(R);
	return true;
}

bool BCPhisComponentObject::GetRotation(dMatrix4 R)
{
	if(!m_RootObject)
		return false;
	return m_RootObject->GetRotation(R);
}

bool BCPhisComponentObject::SetPosition(float x,float y,float z)
{
	if(!m_RootObject)
		return false;
	dVector3 v;
	GetPosition(v);
	v[0]=x-v[0];
	v[1]=y-v[1];
	v[2]=z-v[2];
	for(BCScenePhisObjects::iterator po=m_SObjects.begin();po!=m_SObjects.end();po++)
		if(*po!=m_RootObject)
		{			
		 	dVector3 popos;
			(*po)->GetPosition(popos);
			vector_add3(popos,v);
			(*po)->SetPosition(popos[0],popos[1],popos[2]);
		}	
	m_RootObject->SetPosition(x,y,z);
	return true;
}

bool BCPhisComponentObject::GetPosition(dVector3 p)
{
	if(!m_RootObject)
		return false;
	return m_RootObject->GetPosition(p);
}
		
bool BCPhisComponentObject::SetLinearVel(float x, float y, float z)
{
	for(BCScenePhisObjects::iterator po=m_SObjects.begin();po!=m_SObjects.end();po++)
		(*po)->SetLinearVel(x,y,z);
	return true;
}
	
bool BCPhisComponentObject::SetForce(float fx,float fy,float fz)
{
	for(BCScenePhisObjects::iterator po=m_SObjects.begin();po!=m_SObjects.end();po++)
		(*po)->SetForce(fx,fy,fz);
	return true;
}

bool BCPhisComponentObject::AddForce(float fx, float fy, float fz)
{
	for(BCScenePhisObjects::iterator po=m_SObjects.begin();po!=m_SObjects.end();po++)
		(*po)->AddForce(fx,fy,fz);
	return true;
}

bool BCPhisComponentObject::SetGravityMode(GravityMode mode)
{
	for(BCScenePhisObjects::iterator po=m_SObjects.begin();po!=m_SObjects.end();po++)
		(*po)->SetGravityMode(mode);
	return true;
}

GravityMode BCPhisComponentObject::GetGravityMode()
{
	return m_RootObject?m_RootObject->GetGravityMode():NoGravity;
}

bool BCPhisComponentObject::SetGravity(float x,float y,float z)
{
	for(BCScenePhisObjects::iterator po=m_SObjects.begin();po!=m_SObjects.end();po++)
		(*po)->SetGravity(x,y,z);
	return true;
}

// определение динамического состояния объекта
// Re: первые поля структуры - идентификатор и размер
size_t BCPhisComponentObject::GetStateDataSize()
{
	return sizeof(PhisObjectState)*m_SObjects.size()+sizeof(PhisComponentState);
}

bool   BCPhisComponentObject::GetState(void* ptr)
{	
	// выгрузка заголовка
	PhisComponentState* p=(PhisComponentState*)ptr;
	p->signature=0x54534850;	// (PH)is(ST)ate
	p->size=sizeof(PhisObjectState)*m_SObjects.size();
	// выгрузка данных
	for(int i=0;i<m_SObjects.size();i++)
	{
		m_SObjects[i]->GetRotation(p->obj[i].rotation);
		m_SObjects[i]->GetPosition(p->obj[i].position);
		m_SObjects[i]->GetLinearVel(p->obj[i].LinearVel);
		m_SObjects[i]->GetAngularVel(p->obj[i].AngularVel);
	}
	return true;
}

bool   BCPhisComponentObject::SetState(const void* ptr)
{
	// проверка заголовка
	const PhisComponentState* p=(const PhisComponentState*)ptr;
	if(p->signature!=0x54534850 ||				// (PH)is(ST)ate
           p->size!=sizeof(PhisObjectState)*m_SObjects.size())
           	return false;
	// загрузка данных
	for(int i=0;i<m_SObjects.size();i++)
	{
		m_SObjects[i]->SetRotation(p->obj[i].rotation);
		m_SObjects[i]->SetPosition(p->obj[i].position[0],p->obj[i].position[1],p->obj[i].position[2]);
		m_SObjects[i]->SetLinearVel(p->obj[i].LinearVel[0],p->obj[i].LinearVel[1],p->obj[i].LinearVel[2]);
		m_SObjects[i]->SetAngularVel(p->obj[i].AngularVel[0],p->obj[i].AngularVel[1],p->obj[i].AngularVel[2]);
	}
	return true;
}

// Работа со связями .... 
int BCPhisComponentObject::CreateHinge(int obj1,int obj2,const dVector3 anchor,const dVector3 axis)
{
	if(GetWorld()==NULL || obj1>=m_SObjects.size() || obj1<0
			    || obj2>=m_SObjects.size())
		return -1;
	dJointID joint=dJointCreateHinge(GetWorld(),0);
	if(joint==NULL)
		return -1;
	dJointAttach(joint,m_SObjects[obj1]->GetBody(),obj2<0?NULL:m_SObjects[obj2]->GetBody());
	dJointSetHingeAnchor(joint,anchor[0],anchor[1],anchor[2]);
	dJointSetHingeAxis(joint,axis[0],axis[1],axis[2]);
	m_JointHinges.push_back(joint);
	return m_JointHinges.size()-1;
}

bool	 BCPhisComponentObject::GetHingeAnchor(int index,dVector3 anchor)
{
	if(index<0 || index>=m_JointHinges.size())
		return false;
	dJointGetHingeAnchor(m_JointHinges[index],anchor);
	return true;
}

bool	 BCPhisComponentObject::SetHingeAnchor(int index,const dVector3 anchor)
{
	if(index<0 || index>=m_JointHinges.size())
		return false;
	dJointSetHingeAnchor(m_JointHinges[index],anchor[0],anchor[1],anchor[2]);
	return true;
}

bool	 BCPhisComponentObject::GetHingeAxis(int index,dVector3 axis)
{
	if(index<0 || index>=m_JointHinges.size())
		return false;
	dJointGetHingeAxis(m_JointHinges[index],axis);
	return true;
}

bool	 BCPhisComponentObject::SetHingeAxis(int index,const dVector3 axis)
{
	if(index<0 || index>=m_JointHinges.size())
		return false;
	dJointSetHingeAxis(m_JointHinges[index],axis[0],axis[1],axis[2]);
	return true;
}


float	 BCPhisComponentObject::GetHingeAngle(int index)
{
	if(index<0 || index>=m_JointHinges.size())
		return dInfinity;
	return dJointGetHingeAngle(m_JointHinges[index]);
}

float	 BCPhisComponentObject::GetHingeAngleRate(int index)
{
	if(index<0 || index>=m_JointHinges.size())
		return dInfinity;
	return dJointGetHingeAngleRate(m_JointHinges[index]);
}

bool	 BCPhisComponentObject::SetHingeParam(int index,int parameter,float value)
{
	if(index<0 || index>=m_JointHinges.size())
		return false;
	dJointSetHingeParam(m_JointHinges[index],parameter,value);
	return true;
}

float	 BCPhisComponentObject::GetHingeParam(int index,int parameter)
{
	if(index<0 || index>=m_JointHinges.size())
		return dInfinity;
	return dJointGetHingeParam(m_JointHinges[index],parameter);;
}

// ------ hinge 2
int	 BCPhisComponentObject::CreateHinge2(int obj1,int obj2,const dVector3 anchor,const dVector3 axis,const dVector3 axis2)
{
	if(GetWorld()==NULL || obj1>=m_SObjects.size() || obj1<0
			    || obj2>=m_SObjects.size())
		return -1;
	dJointID joint=dJointCreateHinge2(GetWorld(),0);
	if(joint==NULL)
		return -1;
	dJointAttach(joint,m_SObjects[obj1]->GetBody(),obj2<0?NULL:m_SObjects[obj2]->GetBody());
	dJointSetHinge2Anchor(joint,anchor[0],anchor[1],anchor[2]);
	dJointSetHinge2Axis1(joint,axis[0],axis[1],axis[2]);
	dJointSetHinge2Axis2(joint,axis2[0],axis2[1],axis2[2]);
	m_JointHinges2.push_back(joint);
	return m_JointHinges2.size()-1;
}

bool	 BCPhisComponentObject::GetHinge2Anchor(int index,dVector3 anchor)
{
	if(index<0 || index>=m_JointHinges2.size())
		return false;
	dJointGetHingeAnchor(m_JointHinges2[index],anchor);
	return true;
}

bool	 BCPhisComponentObject::SetHinge2Anchor(int index,const dVector3 anchor)
{
	if(index<0 || index>=m_JointHinges2.size())
		return false;
	dJointSetHingeAnchor(m_JointHinges2[index],anchor[0],anchor[1],anchor[2]);
	return true;
}

bool	 BCPhisComponentObject::GetHinge2Axis1(int index,dVector3 axis)
{
	if(index<0 || index>=m_JointHinges2.size())
		return false;
	dJointGetHinge2Axis1(m_JointHinges2[index],axis);
	return true;
}

bool	 BCPhisComponentObject::SetHinge2Axis1(int index,const dVector3 axis)
{
	if(index<0 || index>=m_JointHinges2.size())
		return false;
	dJointSetHinge2Axis1(m_JointHinges2[index],axis[0],axis[1],axis[2]);
	return true;
}

bool	 BCPhisComponentObject::GetHinge2Axis2(int index,dVector3 axis)
{
	if(index<0 || index>=m_JointHinges2.size())
		return false;
	dJointGetHinge2Axis2(m_JointHinges2[index],axis);
	return true;
}

bool	 BCPhisComponentObject::SetHinge2Axis2(int index,const dVector3 axis)
{
	if(index<0 || index>=m_JointHinges2.size())
		return false;
	dJointSetHinge2Axis2(m_JointHinges2[index],axis[0],axis[1],axis[2]);
	return true;
}

float	 BCPhisComponentObject::GetHinge2Angle1(int index)
{
	if(index<0 || index>=m_JointHinges2.size())
		return dInfinity;
	return dJointGetHinge2Angle1(m_JointHinges2[index]);
}

float	 BCPhisComponentObject::GetHinge2Angle1Rate(int index)
{
	if(index<0 || index>=m_JointHinges2.size())
		return dInfinity;
	return dJointGetHinge2Angle1Rate(m_JointHinges2[index]);
}

float	 BCPhisComponentObject::GetHinge2Angle2Rate(int index)
{
	if(index<0 || index>=m_JointHinges2.size())
		return dInfinity;
	return dJointGetHinge2Angle2Rate(m_JointHinges2[index]);
}

bool BCPhisComponentObject::SetHinge2Param(int index,int parameter,float value)
{
	if(index<0 || index>=m_JointHinges2.size())
		return false;
	dJointSetHinge2Param(m_JointHinges2[index],parameter,value);
	return true;
}

float BCPhisComponentObject::GetHinge2Param(int index,int parameter)
{
	if(index<0 || index>=m_JointHinges2.size())
		return dInfinity;
	return dJointGetHinge2Param(m_JointHinges2[index],parameter);
}

// ------ slider
int BCPhisComponentObject::CreateSlider(int obj1,int obj2,const dVector3 axis)
{
	if(GetWorld()==NULL || obj1>=m_SObjects.size() || obj1<0
			    || obj2>=m_SObjects.size())
		return -1;
	dJointID joint=dJointCreateSlider(GetWorld(),0);
	if(joint==NULL)
		return -1;
	dJointAttach(joint,m_SObjects[obj1]->GetBody(),obj2<0?NULL:m_SObjects[obj2]->GetBody());
	dJointSetSliderAxis(joint,axis[0],axis[1],axis[2]);
	m_JointSliders.push_back(joint);
	return m_JointSliders.size()-1;
}

bool BCPhisComponentObject::GetSliderAxis(int index,dVector3 axis)
{
	if(index<0 || index>=m_JointSliders.size())
		return false;
	dJointGetSliderAxis(m_JointSliders[index],axis);
	return true;
}

bool BCPhisComponentObject::SetSliderAxis(int index,const dVector3 axis)
{
	if(index<0 || index>=m_JointSliders.size())
		return false;
	dJointSetSliderAxis(m_JointSliders[index],axis[0],axis[1],axis[2]);
	return true;
}

float BCPhisComponentObject::GetSliderPosition(int index)
{
	if(index<0 || index>=m_JointSliders.size())
		return dInfinity;
	return dJointGetSliderPosition(m_JointSliders[index]);
}

float BCPhisComponentObject::GetSliderPositionRate(int index)
{
	if(index<0 || index>=m_JointSliders.size())
		return dInfinity;
	return dJointGetSliderPositionRate(m_JointSliders[index]);
}

bool BCPhisComponentObject::SetSliderParam(int index,int parameter,float value)
{
	if(index<0 || index>=m_JointHinges.size())
		return false;
	dJointSetSliderParam(m_JointHinges[index],parameter,value);
	return true;
}

float BCPhisComponentObject::GetSliderParam(int index,int parameter)
{
	if(index<0 || index>=m_JointHinges.size())
		return dInfinity;
	return dJointGetSliderParam(m_JointHinges[index],parameter);
}

//---------------------------------------------------------------------------------------------------------------------
// поиск объектов и связей, в случае отсутсвия return -1
int BCPhisComponentObject::FindHinge(dJointID jid)
{
	for(int i=m_JointHinges.size();i-- && m_JointHinges[i]!=jid;);
	return i;
}

int BCPhisComponentObject::FindSlider(dJointID jid)
{
	for(int i=m_JointSliders.size();i-- && m_JointSliders[i]!=jid;);
	return i;
}

int BCPhisComponentObject::FindHinge2(dJointID jid)
{
	for(int i=m_JointHinges2.size();i-- && m_JointHinges2[i]!=jid;);
	return i;
}

int BCPhisComponentObject::FindObject(BCScenePhisObject* obj)
{
	for(int i=m_SObjects.size();i-- && m_SObjects[i]!=obj;);
	return i;
}

int BCPhisComponentObject::FindObject(dBodyID bid)
{
	for(int i=m_SObjects.size();i-- && m_SObjects[i]->GetBody()!=bid;);
	return i;
}

void BCPhisComponentObject::Draw()
{
	for(int i=m_SObjects.size();i--;m_SObjects[i]->Draw());
}
