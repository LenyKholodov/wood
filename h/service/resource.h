#ifndef __RESOURCE_SERVICE__
#define __RESOURCE_SERVICE__

#include <kernel\service.h>
#include <kernel\critsect.h>
#include <resource\resource.h>

/////////////////////////////////////////////////////////////////////////////////////////////
///Служба управления ресурсами
/////////////////////////////////////////////////////////////////////////////////////////////
class    ResourceService: public Service
{
  public:
                                ResourceService  (Pool*,size_t = 0);
                    virtual     ~ResourceService ();

/////////////////////////////////////////////////////////////////////////////////////////////
///Именование ресурсов                
/////////////////////////////////////////////////////////////////////////////////////////////
                Resource*       GetResource (const char* res_name) const;

                bool            AddResource (const char*,Resource*);
                void            DelResource (const char*);

/////////////////////////////////////////////////////////////////////////////////////////////
///Именование ресурсных групп
/////////////////////////////////////////////////////////////////////////////////////////////
                ResourceGroup*  GetResGroup (const char* group_name) const;
                
                bool            AddResGroup (const char*,ResourceGroup*);
                void            DelResGroup (const char*);

    virtual     bool            valid       () const { return mMainPool && mResources.valid () &&
                                                       mResGroups.valid ();
                                                     }     

  protected:
                Pool*           GetPool     () const { return mMainPool; }
                void            SetPool     (Pool* pool) { mMainPool = pool; }
                Pool*           M_GetPool   () const { return mMainPool; }

  private:
        autoptr_t<Pool>             mMainPool;
        bool                        mCreatePool;

        StringDict<Resource*>       mResources;
        StringDict<ResourceGroup*>  mResGroups;
};

#endif