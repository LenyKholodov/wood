#ifndef __XCHNG_3D_FILE_FORMAT__
#define __XCHNG_3D_FILE_FORMAT__

#include <graph\grtypes.h>
#include <pool.h>
#include <binfile.h>
#include <misc.h>

////////////////////////////////////////////////////////////////////////////////
///Описание файлового формата для обмена 3D - объектами
////////////////////////////////////////////////////////////////////////////////

const int       MAT_NAME_LEN                    = 32;
const int       DEFAULT_3DS_TEMP_POOL_SIZE      = 8000000;
const float     MAX_BOUND                       = 1.0e6;
const float     MIN_BOUND                       = -1.0e6;

extern BOOL     CONV3DS_CONVERT_ALPHA_TEXTURES; 

struct  objnode_t
{        
  bound_t       bound;          //Ограничивающее тело для объекта  
  int           parent;         //Родитель
  int           node;           //Первый узел
  int           next;           //Следующий объект на данном уровне иерархии
  int           group;          //Номер порции представления объекта (если она есть)
  struct 
  {
    int    flags;      //Rotate|Translate|Scale;
    int    node;       //Индекс кластера с анимацией для объекта
  }anim;  
};

struct  nodelist_t
{
  int   first;
  int   count;
};

struct  group3d_t
{
  nodelist_t    vertex;
  nodelist_t    facet;
  nodelist_t    normal;
};

struct vecseg_t
{
  vector3d_t    a, b, c, d;
  int           start,end;
  float         locTime;
  float         easeFrom,easeTo;
};

struct rot_t
{
  vector3d_t vec;
  float      angle;
};

struct quatseg_t
{
  quat_t        c1,v1,c2,v2;
  int           start,end;
  float         locTime;
  float         easeFrom,easeTo;
};

struct animseg_t
{
  struct anim_t
  {
    int first, count;
  }rot,trans,scale;

  vector3d_t    pivot;          //Центр поворотов
  matrix_t      start;          //Начальное состояние
  vector3d_t    offs;           //Начальное смещение
};

struct          block_t;
struct          object_t;
typedef         Dict<int>         MatIndexDict;
typedef         Dict<textmat_t*>  MatDict;

////////////////////////////////////////////////////////////////////////////////
///Описание 3D объекта в пространстве (в виде иерархического дерева)
////////////////////////////////////////////////////////////////////////////////
class   Object3D: public MemObject
{ 
  friend class X3FLoader;         
  friend class X3FExproter;
  public:
                            Object3D  (Pool*);
                            Object3D  (Pool*,const char*,size_t = DEFAULT_3DS_TEMP_POOL_SIZE);
                            Object3D  (Pool*,BinFile*,size_t = DEFAULT_3DS_TEMP_POOL_SIZE);
             virtual        ~Object3D ();

////////////////////////////////////////////////////////////////////////////////
///При любом добавлении элементов не забыть mSizeof!!!!!
////////////////////////////////////////////////////////////////////////////////

                            enum {
                                 VERTEX = 0,
                                 FACET,
                                 NORMAL,
                                 LIGHT,
                                 MATERIAL,
                                 GROUP, 
                                 NODE,
                                 
                                 ANIM_SEG,
                                 ANIM_TRANS,
                                 ANIM_SCALE,
                                 ANIM_ROTATE,
                                 
                                 ELEMENTS_COUNT
                            };

////////////////////////////////////////////////////////////////////////////////
///Узел дерева
////////////////////////////////////////////////////////////////////////////////
             class    Node
             {
               public:
                        Node  ();
                        Node  (Object3D&,int _ObjNode);
                        Node  (const Node&);                        

                        Node&       operator = (const Node&);
                        Node&       operator = (int x) { mNode = x; return *this; }

                        Node        next   ();
                        Node        node   ();
                        Node        parent ();
                        BOOL        more   () const;
                        BOOL        valid  () const { return mOwner && mNode != -1; }

                        Object3D*   owner ()  const { return mOwner; }
                        int         index ()  const { return mNode; }

                        const 
                        bound_t&      bound  () const; 
                        const
                        bound_t&      _bound ();

                        vertex_t*     vertex () const;
                        vector3d_t*   normal () const;
                        facet_t*      facet  () const;

                        const
                        animseg_t*    anim   () const;
                        const         
                        vecseg_t*     trans  () const;
                        const         
                        vecseg_t*     scale  () const;
                        const         
                        quatseg_t*    rotate () const;

                        int         count  (int) const;

               protected:
                        void          _bound (vector3d_t&,vector3d_t&); 

               protected:
                        Object3D*   mOwner;
                        int         mNode;           
             };

             friend class Object3D::Node;

////////////////////////////////////////////////////////////////////////////////
///Создание узла дерева указывающего на корень
////////////////////////////////////////////////////////////////////////////////
            Node        main      ();
            int         MainIndex () const { return mMain; }

            const
            bound_t&    _bound    ();

////////////////////////////////////////////////////////////////////////////////
///Количество элементов
////////////////////////////////////////////////////////////////////////////////
            int         count (int) const;

////////////////////////////////////////////////////////////////////////////////
///Окружение
////////////////////////////////////////////////////////////////////////////////
        const textmat_t&   GetMaterial (uint) const;
        const light_t&     GetLight    (uint) const;

        BOOL               GetEnv      (uint _enum,uint index,void*,uint count = 1) const;
        void               SetMatPrefix (const char*);

////////////////////////////////////////////////////////////////////////////////
///Загрузка - сохранение
////////////////////////////////////////////////////////////////////////////////
        BOOL            Load3DS (const char*,
                                 size_t = DEFAULT_3DS_TEMP_POOL_SIZE
                                );
        BOOL            Load3DS (BinFile*,
                                 size_t = DEFAULT_3DS_TEMP_POOL_SIZE
                                );

        void            Save   (const char*);
        void            Save   (BinFile*);
        BOOL            Load   (const char*);
        BOOL            Load   (BinFile*);

        bool            valid  () const { return mSpace != NULL; }

  protected:  
        inline objnode_t*       objnode ()   const { return (objnode_t*)mPtr [NODE]; }
        inline group3d_t*       group   ()   const { return (group3d_t*)mPtr [GROUP]; }
        inline vertex_t*        vertex  ()   const { return (vertex_t*)mPtr [VERTEX]; }
        inline vector3d_t*      normal  ()   const { return (vector3d_t*)mPtr [NORMAL]; }
        inline facet_t*         facet   ()   const { return (facet_t*)mPtr [FACET]; }
        inline textmat_t*       material()   const { return (textmat_t*)mPtr [MATERIAL]; }
        inline light_t*         light   ()   const { return (light_t*)mPtr [LIGHT]; }
        inline animseg_t*       animseg ()   const { return (animseg_t*)mPtr [ANIM_SEG]; }
        inline vecseg_t*        animtrans () const { return (vecseg_t*)mPtr [ANIM_TRANS]; }
        inline vecseg_t*        animscale () const { return (vecseg_t*)mPtr [ANIM_SCALE]; }
        inline quatseg_t*       animrot   () const { return (quatseg_t*)mPtr [ANIM_ROTATE]; }
             
  protected:
                int         mMain;                    //Первый узел в дереве представления
                char*       mSpace;                   //Указатель на буфер
                int         mCount [ELEMENTS_COUNT];  //Количество элементов
                char*       mPtr [ELEMENTS_COUNT];    //Указатели на элементы
       static   size_t      mSizeof [ELEMENTS_COUNT];
};

////////////////////////////////////////////////////////////////////////////////
///Абстрактный загрузчик
////////////////////////////////////////////////////////////////////////////////
class X3FLoader: public MemObject
{
  public:
                        X3FLoader  (Object3D*,Pool* = NULL);  
                        X3FLoader  (Object3D*,BinFile*,Pool* = NULL);  
           virtual      ~X3FLoader ();

  protected:
////////////////////////////////////////////////////////////////////////////////
///Доступ к объекту Object3D
////////////////////////////////////////////////////////////////////////////////
           int          count (int) const;
           void         count (int _Enum,int _Count);

           void         alloc ();
           void         free  ();

           void*        get   (int _Enum);
           void         main  (int);
           int          main  () const;

  protected:
     Object3D*          mObject;
     BinFile*           mFile;
};

////////////////////////////////////////////////////////////////////////////////
///Загрузчик 3DS + анимация
////////////////////////////////////////////////////////////////////////////////
class Object3DS;
class Mesh3DS;
class Loader3DS: public X3FLoader
{
  public:
                Loader3DS   (Object3D*,BinFile*,Pool* = NULL);
       virtual  ~Loader3DS  ();

  protected:
      void      recalc      (Mesh3DS*);                  
      void      recalc      (Object3DS*);
      void      convert     (Object3DS*,int,int*);
      void      convert     (Mesh3DS*);
      int       convert     (Object3DS*,int*,Mesh3DS*);
      void      insert      (int node,int child);

  private:
      int16     mConv [0x10000];
};

#endif