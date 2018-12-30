#ifndef __RESOURCE_REFERENCES__
#define __RESOURCE_REFERENCES__


#include <pool.h>
#include <service\resource.h>

#include <resource\caches.h>

static accessptr_t<ResourceService>    resourceService;

/*
  ����㦠�� �㦡� �����প� ����ᮢ
    _Pool      - �� ��� �뤥����� ����� ��� ������
    cache_size - ࠧ��� ��୥���� ���
    temp_size  - ࠧ��� �������⥫쭮�� �㫠 (���� �� ������)
*/

ResourceService*  RES_Load  (Pool* = M_GetPool (),size_t = 0); 

/*
  ���㦠�� �㦡� �����প� ����ᮢ
*/

void      RES_Done      (ResourceService* = resourceService);

/*
  ���������� ����� ����� � ��뢠��� ��� � ����ᮬ
*/

#define   RES_AddResource         resourceService->AddResource

/*
  �������� ����� ����� 
*/

#define   RES_DelResource         resourceService->DelResource

/*
  ���⨥ ����� �� �����
*/

#define   RES_GetResource         resourceService->GetResource

/*
  �������� ��㯯� ����ᮢ

    ��㯯� ����ᮢ ����室��� ��� ���஢���� ����ᮢ � ��������묨 ᢮��⢠��
*/

#define   R_AddGroup            resourceService->AddResGroup

/*
  �������� ��㯯� ����ᮢ
*/

#define   R_DelGroup            resourceService->DelResGroup

/*
  ���⨥ ��㯯� ����ᮢ
*/

#define   R_GetGroup            resourceService->GetResGroup


#endif