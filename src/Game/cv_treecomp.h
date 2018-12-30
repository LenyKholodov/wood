#ifndef __CONVERT_TREE_COMPILER__
#define __CONVERT_TREE_COMPILER__

#include "defs.h"
#include "g_level.h"
#include <misc.h>

struct  tree_t
{
  vector3d_t    pos;    //Локальная позиция
  TREE          tree;   //Дескриптор модели дерева
};

struct  treegroup_t
{
  vector3d_t     pos;       //Мировая позиция
  TREEGROUP      group;     //Дескриптор лесного массива
};

struct  treemapheader_t
{
  struct block_t {
    ulong       offs;           //Положение в файле
    uint        count;          //Количество элементов
    size_t      size;           //Размер блока
  }trees, groups, map;
};

////////////////////////////////////////////////////////////////////////////////
///Компилятор древесной структуры леса
////////////////////////////////////////////////////////////////////////////////
class    TreeCompiler: public MemObject
{
  public:
                        TreeCompiler  (Pool*,int _TreeGroupNum,int _TreeTypeNum);
                        TreeCompiler  (Pool*,GameLevel*);
             virtual    ~TreeCompiler ();

////////////////////////////////////////////////////////////////////////////////
///Создание шаблонов древесных групп
////////////////////////////////////////////////////////////////////////////////
        TREEGROUP       CreateTreeGroup         ();
        void            DeleteTreeGroup         (TREEGROUP);

        void            SetTreeGroupPosition    (TREEGROUP,float worldX,float worldY);

////////////////////////////////////////////////////////////////////////////////
///Создание описателей деревьев (формат мульти-модели)
////////////////////////////////////////////////////////////////////////////////
        TREE            CreateTree              (const char*); //lod
        void            ChangeTree              (TREE,const char*);
        void            DeleteTree              (TREE);

////////////////////////////////////////////////////////////////////////////////
///Добавление деревьев в лесной массив
////////////////////////////////////////////////////////////////////////////////
        void            AddTree                 (TREEGROUP,TREE,float locX,float locY);
        void            DelTree                 (TREEGROUP,TREE,float locX,float locY);
        void            DelTree                 (TREEGROUP,TREE);

        void            AddTree                 (TREE,float locX,float locY);
        void            DelTree                 (TREE,float locX,float locY);
        void            DelTree                 (TREE);

        void            AddTreeGroup            (TREEGROUP,float worldX,float worldY);
        void            DelTreeGroup            (TREEGROUP,float worldX,float worldY);
        void            DelTreeGroup            (TREEGROUP);

////////////////////////////////////////////////////////////////////////////////
///Сохранение/Восстановление в игровой карте
////////////////////////////////////////////////////////////////////////////////
        BOOL            Save                    (GameLevel*);
        BOOL            Load                    (GameLevel*);

////////////////////////////////////////////////////////////////////////////////
///Доступ
////////////////////////////////////////////////////////////////////////////////
        BOOL            GetTree                 (TREE,char*);
        BOOL            GetTreeGroup            (TREEGROUP,RingTmpl<tree_t>&);
        BOOL            GetMap                  (RingTmpl<treegroup_t>&);

        uint            treecount               () const { return mTreeDescCount; }
        uint            groupcount              () const { return mTreeGrCount; }

  protected:
        class    TreeGroup;
        class    Tree;
        friend   class TreeCompiler::TreeGroup;
        friend   class TreeCompiler::Tree;

        typedef  RingTmpl<treegroup_t>    TreeGroupRing;
        typedef  RingTmpl<tree_t>         TreeRing;

        void            reindex                 ();         
        BOOL            load                    (BinFile*);

  private:
        TreeGroup**             mTreeGroup;
        Tree**                  mTreeDesc;
        TreeGroupRing           mGroupMap;   //on map

        int                     mTreeGrCount, mTreeDescCount;
        TREEGROUP               mSingleGroup;
};

#endif