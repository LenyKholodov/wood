#ifndef __RENDER_TREE_PRECOMPILE__
#define __RENDER_TREE_PRECOMPILE__

#include <pool.h>

#include "r_matdict.h"
#include "cv_treecomp.h"

class    GameLevel;
class    MatDictionary;
struct   treedesc_t;
struct   treechunk_t;
struct   multimodeldata_t;

////////////////////////////////////////////////////////////////////////////////
///Создание precompile для деревьев и работа с ним
////////////////////////////////////////////////////////////////////////////////
class    TreePrecompile: public MemObject
{
  public:
                TreePrecompile  (Pool*,TreeCompiler&,const char* = RENDER_TREE_PRECOMP_NAME);
    virtual     ~TreePrecompile ();

////////////////////////////////////////////////////////////////////////////////
///Подгрузка необходимых ресурсов
////////////////////////////////////////////////////////////////////////////////
       BOOL        LoadVertexBuffer        (TREE,vertexbuffer_t*);
       int         LoadIndexBuffer         (TREE,mesh_t*,size_t maxSize); //ret vb count

////////////////////////////////////////////////////////////////////////////////
///Доступ
////////////////////////////////////////////////////////////////////////////////
       const    
       boundbox_t& GetBound                (TREE);
       size_t      GetIndexBufSize         (TREE);
       uint        GetVertexCount          (TREE);
       uint        GetTreesCount           () const { return mComp.treecount (); }
       uint        GetMatCount             () const { return mMatDict.count (); }

       MatDictionary& GetMatDict           () const { return (MatDictionary&)mMatDict; }

  protected:
       BOOL     BuildTreePrecompile     ();

  private:
       BOOL                    PrecompModels (TreeCompiler&);
       BOOL                    AddModel      (const char*,treechunk_t&,int);
       void                    FreeModel     (treechunk_t&);
       multimodeldata_t*       LoadModel     (const char*);
       void                    CalcBound     (multimodeldata_t*,bound_t&);

  private:
    MatDictionary       mMatDict;          
    treedesc_t*         mTreeDesc;
    BinFile*            mFile;
    TreeCompiler&       mComp;
};

#endif