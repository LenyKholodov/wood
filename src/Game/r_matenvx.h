#ifndef __RENDER_MATERIAL_ENVIRONMENT_X__
#define __RENDER_MATERIAL_ENVIRONMENT_X__

#include <binfile.h>
#include <graph\grtypes.h>
#include <scripts\us.h>

#include "defs.h"

const   size_t     ME_TEXTURE_NAME_LEN = 32;

const   MATERIAL   ME_NO_TEXTURE       = -1;
const   MATERIAL   ME_WRONG_TEXTURE    = -2;

const   uint       ME_DIFFUSE_MAP      = 0;
const   uint       ME_SPECULAR_MAP     = 1;
const   uint       ME_BUMP_MAP         = 2;
const   uint       ME_ALPHA_MAP        = 3;     //only for user load

class   UserMaterialLoader;
struct  matloader_t
{
  UserMaterialLoader*   loader;
  int                   index;
  char                  data [];
};

class   UserMaterialLoader {
  public: virtual bitmap_t* load (const matloader_t*,Pool*) = 0;
};

struct  matdesc_t
{
  material_t    color;

  struct {
    char        diffuse  [ME_TEXTURE_NAME_LEN];
    char        specular [ME_TEXTURE_NAME_LEN];
    char        alpha    [ME_TEXTURE_NAME_LEN];
    char        bump     [ME_TEXTURE_NAME_LEN];
  }text;

  char          name     [ME_TEXTURE_NAME_LEN];

  float         blendKoef;
  char          userLoader [4];  //1: user loader, 0: std loader

                matdesc_t ();
                matdesc_t (const char* diffuse,
                           const char* alpha    = NULL,
                           const char* bump     = NULL,
                           const char* specular = NULL
                          );
};

////////////////////////////////////////////////////////////////////////////////
///Прототип для загрузчика рисунков
////////////////////////////////////////////////////////////////////////////////
typedef bitmap_t* (*BitMapLoaderFn)(BinFile*,Pool*,ostream& = dout);

struct bitmaploader_t
{
  BitMapLoaderFn        fn;
  int                   resType;
  size_t                cacheSize;
};

////////////////////////////////////////////////////////////////////////////////
///Окружение материалов
////////////////////////////////////////////////////////////////////////////////
class MatEnvX: public MemObject
{
  public:
                MatEnvX        (Pool*       _Pool,
                                size_t      _MaterialPoolSize,
                                const char* _MaterialBaseDir,
                                int         _MaterialDynCount = 0 //only static no dynamic
                               );
     virtual   ~MatEnvX        ();

////////////////////////////////////////////////////////////////////////////////
///Установка/поиск материала
////////////////////////////////////////////////////////////////////////////////
      void        SetMat         (MATERIAL);
      MATERIAL    SearchMat      (const char* material_name);
      matdesc_t&  GetMat         (MATERIAL);

      BOOL        bind           (MATERIAL,uint mode = ME_DIFFUSE_MAP);
      void        unbind         ();
      
////////////////////////////////////////////////////////////////////////////////
///Добавление/удаление материала
////////////////////////////////////////////////////////////////////////////////
      MATERIAL  AddMat         (const matdesc_t&,const char* = NULL);
      void      DelMat         (MATERIAL);

////////////////////////////////////////////////////////////////////////////////
///Упрощение для добавления независимых материалов
///Автоматически создаёт группу материалов для добавляемого материала
////////////////////////////////////////////////////////////////////////////////
      MATERIAL  CreateMat      (const matdesc_t&  desc,
                                MATGROUP*         res         = NULL,
                                int               cache_count = 1,   //only for res = NULL
                                BOOL              autodel     = TRUE //only for res = NULL
                               );

////////////////////////////////////////////////////////////////////////////////
///Создание группы материалов и операции с ней
////////////////////////////////////////////////////////////////////////////////
      MATGROUP  CreateMatGroup (MATERIAL*       mtr,
                                int             count = 1,
                                uint            cache_num = 0,  //0:load all
                                BOOL            autodel = FALSE
                               );
      void      AddMat         (MATGROUP,MATERIAL*,int = 1);
      void      DelMat         (MATGROUP,MATERIAL*,int = 1);
      void      DeleteMatGroup (MATGROUP);

////////////////////////////////////////////////////////////////////////////////
///Работа с hardware
////////////////////////////////////////////////////////////////////////////////
      void      refresh        (MATGROUP,uint hw_textures_num);
      void      reload         ();  //for all groups / with reload
      void      reload         (MATGROUP);

////////////////////////////////////////////////////////////////////////////////
///Присоединение загрузчиков текстур
////////////////////////////////////////////////////////////////////////////////
      BOOL      AddLoader      (const char*     ext,
                                BitMapLoaderFn  fn,
                                size_t          file_cache_size = 0
                               );  
      void      DelLoader      (const char*);

////////////////////////////////////////////////////////////////////////////////
///Доступ
////////////////////////////////////////////////////////////////////////////////
      int       count          () const { return mMatCount; }
      int       usecount       () const { return mMatRing.count (); }

  private:
      struct    matgroup_t;  

      void      release        (matgroup_t*,uint text_index);
      inline
      bitmap_t* LoadTexture    (const char*,BOOL);
      BOOL      LoadTexture    (matgroup_t&,
                                const char*   data,
                                BOOL          loader,
                                int&          hw_text,
                                const char*   alpha = NULL,
                                BOOL          aloader = FALSE
                               );
      bitmap_t* LoadTexture    (const char*);

      void      LoadMatTable   (const char* dir_name);
      void      LoadMatFile    (const char* file_name);

      static
      void      _prepare       (USStreamReader*,uint,DWORD);

  private:
      struct      matnode_t
      {
        matdesc_t*      desc;          //Указатель на описание материала
        matgroup_t*     group;
        matnode_t       *prev, *next;  //Участие в группе (или free-list)
        int             difText,       //Индексы hw - текстур
                        specText,      //ПОРЯДОК НЕ МЕНЯТЬ!!!
                        bumpText;
      };  

      struct hwcachenode_t
      {
        hwcachenode_t *prev, *next;
        int*          cbRefresh;
      };   

      struct matgroup_t
      {
        MATGROUP         id;            //Идентификатор группы
        int              count, cache;  //Кол-во, кэш кол-во
        matnode_t        first, last;   //Запасной слон
        BOOL             autodel;       //Нужно ли автоматическое удаление?

        hwcachenode_t    hwFirst, hwLast;  //Кэширование
        hwcachenode_t*   hwAlloc;
        uint*            textures;
      };   

  private:
      Pool*         mTempPool;
      int           mMatCount, mMatPos;

      typedef       Ring<matdesc_t>            MatRing;
      typedef       Ring<matgroup_t>           GroupRing;
      typedef       Dict<uint>           MatDict;
      typedef       Dict<bitmaploader_t> BMPFn;

      MatRing       mMatRing;
      MatDict       mMatDict;
      BMPFn         mBMPFn;
      matnode_t*    mMatNode, *mFirstFree;

      GroupRing     mGroupRing;      
      matgroup_t*   mDefGroup;
      int           mGroupPos;
      uint          mMatUID;

      uint          mNullText;
};

#endif