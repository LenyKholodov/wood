#ifndef __CONVERT_TREE_COMPILER__
#define __CONVERT_TREE_COMPILER__

#include "defs.h"
#include "g_level.h"
#include <misc.h>

struct  tree_t
{
  vector3d_t    pos;    //�����쭠� ������
  TREE          tree;   //���ਯ�� ������ ��ॢ�
};

struct  treegroup_t
{
  vector3d_t     pos;       //��஢�� ������
  TREEGROUP      group;     //���ਯ�� ��᭮�� ���ᨢ�
};

struct  treemapheader_t
{
  struct block_t {
    ulong       offs;           //��������� � 䠩��
    uint        count;          //������⢮ ����⮢
    size_t      size;           //������ �����
  }trees, groups, map;
};

////////////////////////////////////////////////////////////////////////////////
///��������� �ॢ�᭮� �������� ���
////////////////////////////////////////////////////////////////////////////////
class    TreeCompiler: public MemObject
{
  public:
                        TreeCompiler  (Pool*,int _TreeGroupNum,int _TreeTypeNum);
                        TreeCompiler  (Pool*,GameLevel*);
             virtual    ~TreeCompiler ();

////////////////////////////////////////////////////////////////////////////////
///�������� 蠡����� �ॢ���� ��㯯
////////////////////////////////////////////////////////////////////////////////
        TREEGROUP       CreateTreeGroup         ();
        void            DeleteTreeGroup         (TREEGROUP);

        void            SetTreeGroupPosition    (TREEGROUP,float worldX,float worldY);

////////////////////////////////////////////////////////////////////////////////
///�������� ����⥫�� ��ॢ쥢 (�ଠ� ����-������)
////////////////////////////////////////////////////////////////////////////////
        TREE            CreateTree              (const char*); //lod
        void            ChangeTree              (TREE,const char*);
        void            DeleteTree              (TREE);

////////////////////////////////////////////////////////////////////////////////
///���������� ��ॢ쥢 � ��᭮� ���ᨢ
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
///���࠭����/����⠭������� � ��஢�� ����
////////////////////////////////////////////////////////////////////////////////
        BOOL            Save                    (GameLevel*);
        BOOL            Load                    (GameLevel*);

////////////////////////////////////////////////////////////////////////////////
///�����
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