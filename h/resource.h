#ifndef __RESOURCE_REFERENCES__
#define __RESOURCE_REFERENCES__


#include <pool.h>
#include <service\resource.h>

#include <resource\caches.h>

static accessptr_t<ResourceService>    resourceService;

/*
  Загружает службу поддержки ресурсов
    _Pool      - пул для выделения памяти под ресурсы
    cache_size - размер корневого кэша
    temp_size  - размер дополнительного пула (лучше не менять)
*/

ResourceService*  RES_Load  (Pool* = M_GetPool (),size_t = 0); 

/*
  Выгружает службу поддержки ресурсов
*/

void      RES_Done      (ResourceService* = resourceService);

/*
  Добавление имени ресурса и связывание его с ресурсом
*/

#define   RES_AddResource         resourceService->AddResource

/*
  Удаление имени ресурса 
*/

#define   RES_DelResource         resourceService->DelResource

/*
  Взятие ресурса по имени
*/

#define   RES_GetResource         resourceService->GetResource

/*
  Создание группы ресурсов

    Группа ресурсов необходима для итерирования ресурсов с одинаковыми свойствами
*/

#define   R_AddGroup            resourceService->AddResGroup

/*
  Удаление группы ресурсов
*/

#define   R_DelGroup            resourceService->DelResGroup

/*
  Взятие группы ресурсов
*/

#define   R_GetGroup            resourceService->GetResGroup


#endif