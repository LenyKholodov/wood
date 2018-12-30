#include "pch.h"
#include <windows.h>
#include "r_matenvx.h"
#include <graph\utils.h>
#include <misc.h>
#include <binfile.h>
         
#include <graph\jpeg.h>
#include <graph\bmp.h>
#include <graph\tga.h>
#include <graph\alp.h>

#include <gl\gl.h>
#include <gl\glu.h>
#include <stdio.h>

#include <log.h>

void      MatEnvX::refresh        (MATGROUP id,uint cache_num)
{
  int                 count = mGroupRing.count ();
  GroupRing::iterator iter  = mGroupRing;

  for (int i=0;i<count;i++,iter++)
    if (iter.data().id == id)
    {
      matgroup_t& group = iter.data();

      if (group.textures)
      {
        glDeleteTextures (group.cache,group.textures);
        free             (group.textures); 
      }

      for (int i=0;i<group.cache;i++)  
        if (group.hwAlloc [i].cbRefresh) *group.hwAlloc [i].cbRefresh = -1;

      free (group.hwAlloc);  

      int         cache = cache_num ? cache_num : group.count;  

      if (!cache) cache = 1;

      group.cache       = cache;

      group.hwFirst.prev  = group.hwLast.next = NULL;
      group.hwFirst.next  = &group.hwLast;
      group.hwLast.prev   = &group.hwFirst;
      group.hwFirst.cbRefresh = group.hwLast.cbRefresh = NULL;

      group.textures     = (uint*)alloc (cache * sizeof (int));
      group.hwAlloc      = (hwcachenode_t*)alloc (sizeof (hwcachenode_t) * cache);

      if (!group.textures || !group.hwAlloc)
      {
        D_Message      ("MatEnvX: No enough memory fro refresh:"<<cache_num);
        DeleteMatGroup (group.id);
        return ;
      }  

      for (i=0;i<cache;i++)
      {
        group.hwAlloc [i].prev  = &group.hwAlloc [i-1];
        group.hwAlloc [i].next  = &group.hwAlloc [i+1];

        group.hwAlloc [i].cbRefresh = NULL;
      }  

      group.hwAlloc [0].prev       = &group.hwFirst;
      group.hwAlloc [cache-1].next = &group.hwLast;
      group.hwFirst.next           = group.hwAlloc;
      group.hwLast.prev            = &group.hwAlloc [cache-1];

      glGenTextures (cache,group.textures);

      if (!cache_num)
        reload (group.id);
        
      return;
    }  
}

void      MatEnvX::reload         ()
{
  GroupRing::iterator iter  = mGroupRing;
  int                 count = mGroupRing.count ();

  for (int i=0;i<count;i++,iter++)
  {
    matgroup_t& group = iter.data();

    for (matnode_t* m=group.first.next;m!=&group.last;m=m->next)
      bind (ulong (m-mMatNode));
  }
}

void      MatEnvX::reload         (MATGROUP id)
{
  GroupRing::iterator iter  = mGroupRing;
  int                 count = mGroupRing.count ();

  for (int i=0;i<count;i++,iter++)
    if (iter.data().id == id)  
    {
      matgroup_t& group = iter.data();

      for (matnode_t* m=group.first.next;m!=&group.last;m=m->next)
        bind (ulong (m-mMatNode));

      return;    
    }
}

matdesc_t&  MatEnvX::GetMat         (MATERIAL mat)
{
  if (mat >= 0 && mat < mMatCount && mMatNode [mat].desc)
    return *mMatNode [mat].desc;
  else
  {
    static matdesc_t error;
    return error;    
  }
}

void      MatEnvX::SetMat         (MATERIAL mat)
{
  if (mat >= 0 && mat < mMatCount && mMatNode [mat].desc)
    ::SetMaterial (mMatNode [mat].desc->color);
}

BOOL      MatEnvX::bind           (MATERIAL mat,uint mode)
{
  if (mat >= 0 && mat < mMatCount)
  {
    matnode_t* node   = mMatNode + mat;
    int*       hwtext = &node->difText;
    int&       text   = hwtext [mode];

    if (text >= 0)  
    {
      glBindTexture (GL_TEXTURE_2D,node->group->textures [text]);

      hwcachenode_t* hwnode = node->group->hwAlloc + text;

      hwnode->prev->next    = hwnode->next;
      hwnode->next->prev    = hwnode->prev;
      hwnode->prev          = &node->group->hwFirst;
      hwnode->next          = node->group->hwFirst.next;

      node->group->hwFirst.next->prev = hwnode;
      node->group->hwFirst.next       = hwnode;

      return TRUE;
    }  
    else if (text != ME_WRONG_TEXTURE)
    {
      char* tname [4] = {node->desc->text.diffuse,
                         node->desc->text.specular,
                         node->desc->text.bump,
                         NULL 
                        }; 

      if (mode == ME_DIFFUSE_MAP) 
        tname [3] = node->desc->text.alpha [0] ? node->desc->text.alpha : NULL;

      if (!LoadTexture (*node->group,
                        tname [mode],
                        node->desc->userLoader [mode],
                        text,
                        tname [3],
                        node->desc->userLoader [3]
                       )
         ) 
        text = ME_WRONG_TEXTURE;
      else
        return TRUE;
    }
  }  

  unbind ();

  return FALSE;
}

void        MatEnvX::unbind         ()
{
  glBindTexture (GL_TEXTURE_2D,mNullText);
}

bitmap_t*   MatEnvX::LoadTexture    (const char* buf)
{
  D_Try
  {
    char name [128];

    locase (buf,name);

    int  len  = strlen (name);

    for (const char* ext = name+len-1;ext!=name && *ext != '.';ext--);

    if  (ext == name) 
      return NULL;

    ext++; //'.'

    bitmaploader_t ldr;
    ulong          crc = CRC32 (ext,strlen (ext));

    if (!mBMPFn.search (crc,ldr))
      return NULL;

    BinFile*  file    = F_OpenFile ("textures",name,BF_OPEN_RO);
    BinFile*  cfile   = NULL;

    if (!IsValid (file))
    {
      if (file) delete file;
      return NULL;
    }

    if (ldr.cacheSize)
    {
      cfile = ext_new CacheBinFile (file,GetPool(),ldr.cacheSize);

      if (!IsValid (cfile))
      {
        if (cfile) delete cfile;
      }
      else
        swap (cfile,file);
    }    

    bitmap_t* bmp = (*ldr.fn)(file,GetPool());

    if (file)  delete file;
    if (cfile) delete cfile;

    return bmp;
  }
  D_ExcRet ("MatEnvX: Error at Loadtexture (const char*)",NULL);
/*  char name [128];

  locase (buf,name);

  int  len  = strlen (name);

  for (const char* ext = name+len-1;ext!=name && *ext != '.';ext--);

  if  (ext == name) ext = NULL;

  BinFile*      file    = F_OpenFile (name,BF_OPEN_RO,RES_TEXTURE);
  bitmap_t*     bmp     = NULL;

  if (IsValid (file))
  {  
    if (!ext)
    {
      if (!(bmp = LoadJPEG (file,pool,dout)))
        if (!(bmp = LoadBMP (file,pool,dout)))
          if (!(bmp = LoadTGA (file,pool,dout)))
            bmp = LoadALP (file,pool,dout);
    }
    else
    {
      if      (!strcmp (ext,".bmp"))  
      {
        bmp = LoadBMP  (file,pool,dout);
//        cacheBinFile cbf (pool,file,IMAGE_LOADER_CACHE_SIZE);     
//        bmp =  LoadBMP  (&cbf,pool,dout);
      }
      else if (!strcmp (ext,".jpg"))  
        bmp =  LoadJPEG (file,pool,dout);
      else if (!strcmp (ext,".tga"))  
        bmp =  LoadTGA (file,pool,dout);
      else if (!strcmp (ext,".alp"))
        bmp =  LoadALP (file,pool,dout);
    }    

    delete file;
  }
  else
    return NULL;

  return bmp;*/
}

bitmap_t* MatEnvX::LoadTexture (const char* data,BOOL loader)
{
  if (!loader) return LoadTexture (data); 
  else
  {
    matloader_t* loader = (matloader_t*)data;
    return loader->loader->load (loader,GetPool());
  }  
}

BOOL  MatEnvX::LoadTexture 
  ( matgroup_t&         group,
    const char*         name,
    BOOL                loader,
    int&                destText,
    const char*         aname,
    BOOL                aloader
  )
{
  if (!loader && strlen (name) < 1)
    return FALSE;  
 
  bitmap_t*  bmp     = NULL;

  if (!aname) bmp    = LoadTexture (name,loader);
  else
  {
    bitmap_t* alp = LoadTexture (aname,aloader); 
    bitmap_t* img = LoadTexture (name,loader); 
    
    if (!img || !alp)
    {
      D_Message ("MatEnvX: Error at LoadTexture '"<<name<<"' (no enough memory)");
      if (img) free (img);
      if (alp) free (alp);
      return FALSE;
    }

    bmp = ConvALP (img,alp,GetPool(),dout);

    free (img);
    free (alp);
  }

  if (!bmp)
  {
    D_Message ("MatEnvX: Error at LoadTexture '"<<name<<"'");
    return FALSE;
  }

  static int fmt [5] = {0,GL_LUMINANCE,0,GL_RGB,GL_RGBA};
  int        format  = fmt [bmp->bpp>>3];

  if (!format)
  {
    D_Message ("MatEnvX: Wrong texture bpp: "<<bmp->bpp);
    free      (bmp);

    return FALSE;
  }

  hwcachenode_t*  node       = group.hwLast.prev;

  if (node == &group.hwFirst)
  {
    D_Message ("MatEnvX: No enough texture limits at group: "<<group.id);
    free      (bmp);

    return FALSE;
  }

  if (node->cbRefresh)
    *node->cbRefresh = ME_NO_TEXTURE;

  destText           = ulong (node-group.hwAlloc);

  node->cbRefresh    = &destText;
  node->prev->next   = node->next;
  node->next->prev   = node->prev;
  node->next         = group.hwFirst.next;
  node->prev         = &group.hwFirst;
  group.hwFirst.next = group.hwFirst.next->prev = node;

  glBindTexture    (GL_TEXTURE_2D,group.textures [destText]);
  glTexParameteri  (GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR_MIPMAP_LINEAR); //!!
  glTexParameteri  (GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_LINEAR); //!!

//  if (TEXTURE_MIN_FILTER < GL_NEAREST_MIPMAP_NEAREST &&
//      TEXTURE_MAG_FILTER < GL_NEAREST_MIPMAP_NEAREST)            
//        glTexImage2D     (GL_TEXTURE_2D,0,bmp->bpp>>3,bmp->width,bmp->height,
//                          0,format,GL_UNSIGNED_BYTE,bmp->data);
//  else
    gluBuild2DMipmaps    (GL_TEXTURE_2D,bmp->bpp>>3,bmp->width,bmp->height,
                          format,GL_UNSIGNED_BYTE,bmp->data);

  free    (bmp);
  
  return TRUE;
}

void      MatEnvX::release        (matgroup_t* group,uint index)
{
  if (group && index >=0 && index < group->cache)
  {
    if (group->hwAlloc [index].cbRefresh) 
      *group->hwAlloc [index].cbRefresh = ME_NO_TEXTURE;

    group->hwAlloc [index].cbRefresh = NULL;
  }  
}

MATERIAL  MatEnvX::SearchMat      (const char* mname)
{
  if (mname)    
  {  
    ulong crc   = CRC32 (mname,strlen (mname));
    uint  index = -1;

    if (!mMatDict.search (crc,index))  return -1;
    else                               return index; //check full name
  }

  return -1;
}

MATERIAL  MatEnvX::AddMat         (const matdesc_t& desc,const char* name)
{
  ulong crc = 0;

  if (!name)
  {
    char temp [128];

    sprintf (temp,"____UID_MAT:%i",mMatUID++);    

    crc = CRC32 (temp,strlen (temp));    
  }
  else 
    crc = CRC32 (name,strlen (name));

  if (!mFirstFree || !mMatRing.insert (desc)) 
    return -1;

  matnode_t* node = mFirstFree;

  mFirstFree      = mFirstFree->next;

  node->difText   = node->specText = node->bumpText = -1;
  node->desc      = &mMatRing.GetLast ().data ();
  node->prev      = mDefGroup->last.prev;
  node->next      = &mDefGroup->last;
  node->group     = mDefGroup;

  mDefGroup->last.prev  = node;
  node->prev->next      = node;
  mDefGroup->count++;

  MATERIAL mat         = ulong(node-mMatNode);

  if (!mMatDict.insert (crc,mat))
  {
    DelMat (mat);    
    return -1;
  }  

  return mat;
}

MATERIAL  MatEnvX::CreateMat (const matdesc_t& desc,MATGROUP* res,int cache,BOOL autodel)
{
  MATERIAL mi = AddMat (desc,desc.name [0] ? desc.name : NULL);  

  if (mi == -1)
    return -1;

  if (res && *res != -1)
    AddMat (*res,&mi,1);
  else
  {
    MATGROUP mg = CreateMatGroup (&mi,1,cache,autodel);

    if (res) *res = mg;

    if (mg == -1)
    {
      DelMat (mi);
      return -1;
    }  
  }

  return mi;
}

void      MatEnvX::DelMat         (MATERIAL mat)
{
  if (mat < 0 || mat >= mMatCount)
    return;

  matnode_t* node = &mMatNode [mat];
  ulong      crc  = CRC32 (node->desc->name,strlen (node->desc->name));

  mMatDict.erase (crc);
  mMatRing.erase (*node->desc);

  node->prev->next = node->next;
  node->next->prev = node->prev;

  node->group->count--;
  
  release (node->group,node->difText);
  release (node->group,node->specText);
  release (node->group,node->bumpText);

  if (!node->group->count && node->group->autodel)
    DeleteMatGroup (node->group->id);

  node->next = mFirstFree;
  node->prev = NULL;
  mFirstFree = node;  
}

MATGROUP  MatEnvX::CreateMatGroup (MATERIAL* list,int count,uint cache_num,BOOL ad)
{
  if (!list && count > 0)
    return -1;

  if (!mGroupRing.insert (matgroup_t()))
    return -1;

  matgroup_t& group = mGroupRing.GetLast ().data ();

  group.id          = mGroupPos++;
  group.count       = 0;      
  group.autodel     = ad;

  group.first.desc  = group.last.desc  = NULL;
  group.first.group = group.last.group = &group;
  group.first.prev  = group.last.next  = NULL;
  group.first.next  = &group.last;
  group.last.prev   = &group.first;

  for (int i=0;i<count;i++,list++)
    if (*list >= 0 && *list < mMatCount)
    {
      matnode_t* m = mMatNode + *list;

      if (!m->group || m->group == &group)
        continue;

      m->prev->next = m->next;
      m->next->prev = m->prev;
      
      release (m->group,m->difText);
      release (m->group,m->specText);
      release (m->group,m->bumpText);

      m->group->count--;

      m->group     = &group;
      m->difText   = m->specText = m->bumpText = -1;
      m->prev      = group.last.prev;
      m->next      = &group.last;

      group.last.prev = m;
      m->prev->next   = m;

      group.count++;
    }

  int         cache = cache_num ? cache_num : count;  

  if (!cache) cache = 1;

  group.cache       = cache;
  
//  group.hwFirst.first = &group.hwFirst;
  group.hwFirst.prev  = group.hwLast.next = NULL;
  group.hwFirst.next  = &group.hwLast;
  group.hwLast.prev   = &group.hwFirst;
  group.hwFirst.cbRefresh = group.hwLast.cbRefresh = NULL;

  group.textures     = (uint*)alloc (cache * sizeof (int));
  group.hwAlloc      = (hwcachenode_t*)alloc (sizeof (hwcachenode_t) * cache);

  if (!group.textures || !group.hwAlloc)
  {
    DeleteMatGroup (group.id);
    return -1;
  }  

  for (i=0;i<cache;i++)
  {
    group.hwAlloc [i].prev  = &group.hwAlloc [i-1];
    group.hwAlloc [i].next  = &group.hwAlloc [i+1];

    group.hwAlloc [i].cbRefresh = NULL;
  }  

  group.hwAlloc [0].prev       = &group.hwFirst;
  group.hwAlloc [cache-1].next = &group.hwLast;
  group.hwFirst.next           = group.hwAlloc;
  group.hwLast.prev            = &group.hwAlloc [cache-1];

  glGenTextures (cache,group.textures);

  if (!cache_num)
    reload (group.id);

  return group.id;
}

void      MatEnvX::DeleteMatGroup (MATGROUP id)
{
  GroupRing::iterator iter  = mGroupRing;
  int                 count = mGroupRing.count ();

  for (int i=0;i<count;i++,iter++)
    if (iter.data().id == id)
    {
      matgroup_t& group = iter.data();

      if (group.textures)
      {
        glDeleteTextures (group.cache,group.textures);
        free             (group.textures); 
      }

      for (int i=0;i<group.cache;i++)  
        if (group.hwAlloc [i].cbRefresh) *group.hwAlloc [i].cbRefresh = -1;

      free (group.hwAlloc);

      matnode_t* next = group.first.next->next;

      for (matnode_t* m=group.first.next;m!=&group.last;m=next,next=next?next->next:NULL)
      {
        m->group     = mDefGroup;
        m->difText   = m->specText = m->bumpText = -1;
        m->prev      = mDefGroup->last.prev;
        m->next      = &mDefGroup->last;

        mDefGroup->last.prev = m;
        m->prev->next        = m;

        mDefGroup->count++;
      }

      mGroupRing.erase (iter);  
    
      return;
    }
}

void      MatEnvX::AddMat         (MATGROUP id,MATERIAL* list,int count)
{
  if (!list || count < 0)
    return;

  GroupRing::iterator iter  = mGroupRing;
  int                 cnt = mGroupRing.count ();

  for (int i=0;i<cnt;i++,iter++)
    if (iter.data ().id == id)
    {
      matgroup_t& group = iter.data ();

      for (int i=0;i<count;i++,list++)
        if (*list >= 0 && *list < mMatCount)
        {
          matnode_t* m = mMatNode + *list;

          if (!m->group  || m->group == &group)
            continue;

          m->prev->next = m->next;
          m->next->prev = m->prev;
          
          release (m->group,m->difText);
          release (m->group,m->specText);
          release (m->group,m->bumpText);

          m->group->count--;

          m->group     = &group;
          m->difText   = m->specText = m->bumpText = -1;
          m->prev      = group.last.prev;
          m->next      = &group.last;

          group.last.prev = m;
          m->prev->next   = m;

          group.count++;
        }
          
      return;
    }
}

void      MatEnvX::DelMat         (MATGROUP id,MATERIAL* list,int count)
{
  if (!list || count < 0)
    return;

  matgroup_t& group  = *mDefGroup;

  for (int i=0;i<count;i++,list++)
    if (*list >= 0 && *list < mMatCount)
    {
      matnode_t* m = mMatNode + *list;

      if (!m->group || m->group->id != id)
        continue;
  
      m->prev->next = m->next;
      m->next->prev = m->prev;
      
      release (m->group,m->difText);
      release (m->group,m->specText);
      release (m->group,m->bumpText);

      m->group->count--;

      m->group     = &group;
      m->difText   = m->specText = m->bumpText = -1;
      m->prev      = group.last.prev;
      m->next      = &group.last;

      group.last.prev = m;
      m->prev->next   = m;

      group.count++;
    }

  GroupRing::iterator iter = mGroupRing;
  int                 cnt  = mGroupRing.count ();

  for (i=0;i<cnt;i++,iter++)
    if (iter.data ().id == id)
    {
      matgroup_t& group = iter.data ();

      if (!group.count && group.autodel)
        DeleteMatGroup (id);

      break;  
    }
}

BOOL   MatEnvX::AddLoader (const char* ext,BitMapLoaderFn fn,size_t size)
{
  bitmaploader_t bmp;
  char           buf [32];
  locase         (ext,buf);
  ulong          crc = CRC32 (buf,strlen (buf));

  bmp.fn         = fn;
  bmp.cacheSize  = size;

  if (!mBMPFn.insert (crc,bmp))
  {
    D_Message ("MatEnvX: Error at AddLoader '"<<buf<<"'");    
    return FALSE;
  }

  return TRUE;
}

void   MatEnvX::DelLoader      (const char* ext)
{
  mBMPFn.erase (CRC32 (ext,strlen (ext)));
}

static matdesc_t __mat;

void      MatEnvX::_prepare       (USStreamReader* stream,uint mode,DWORD par)
{
  switch (mode)
  {
    case US_PRECALL: 
      __mat = matdesc_t (); 
      break;
    case US_POSTCALL:   
    {
      MatEnvX* env = (MatEnvX*)par;  

      __mat.color.diffuse  /= 256.0f;
      __mat.color.specular /= 256.0f;
      __mat.color.ambient  /= 256.0f;
      __mat.color.emission /= 256.0f;

      ulong crc             = CRC32 (__mat.name,strlen (__mat.name));
      uint  index           =  0;

      if (!env->mMatDict.search (crc,index))
      {
        env->mMatRing.insert (__mat);
        env->mMatDict.insert (crc,env->mMatPos++);
      }  

      break;
    }
  }
}

void      MatEnvX::LoadMatFile (const char* file_name)
{
  USStreamReader reader (GetPool(),dout); //log
  USStreamReader loc    (GetPool(),dout);

  loc.Bind              ("diffuse",        __mat.color.diffuse);
  loc.Bind              ("specular",       __mat.color.specular);
  loc.Bind              ("ambient",        __mat.color.ambient);
  loc.Bind              ("emisson",        __mat.color.emission);

  loc.Bind              ("alpha_present",  __mat.color.blend);
  loc.Bind              ("shiness",        __mat.color.shiness);

  loc.BindString        ("diffuse_map",    __mat.text.diffuse);
  loc.BindString        ("specular_map",   __mat.text.specular);
  loc.BindString        ("bump_map",       __mat.text.bump);
  loc.BindString        ("alpha_map",      __mat.text.alpha);
  loc.Bind              ("alpha_koef",     __mat.blendKoef);

  loc.BindString        ("name",           __mat.name);
 
  reader.BindReader     ("material",&loc,_prepare,(DWORD)this);

  reader.read           (file_name);

  LogMessage ("MatEnvX: Load material: "<<file_name);
  dout<<"MatEnvX: Load material: "<<file_name<<endl;
}

void      MatEnvX::LoadMatTable   (const char* dir_name)
{
  WIN32_FIND_DATA  data;    
  char             name [MAX_PATH], fname [MAX_PATH];
  BOOL             flag = TRUE;

  strcpy (name,dir_name);
  strcat (name,"/");
  strcat (name,"*.mat");

  for (HANDLE handle = FindFirstFile (name,&data);handle!=INVALID_HANDLE_VALUE && flag;
       flag          = FindNextFile  (handle,&data))     
  {
    strcpy      (fname,dir_name);
    strcat      (fname,"/");
    strcat      (fname,data.cFileName);

    LoadMatFile (fname);
  }

  FindClose (handle);  
}

MatEnvX::MatEnvX (Pool* pool,size_t _PoolSize,const char* _Dir,int _DynCount)
        : mTempPool  (pool=new (pool) FloatPool (pool,_PoolSize)),
          MemObject  (pool),
          mMatCount  (_DynCount),
          mBMPFn     (pool), //??? why GetPool() || mTempPool ????
          mMatNode   (NULL), mGroupPos (0), mDefGroup (NULL), mFirstFree (NULL),
          mGroupRing (pool), mMatDict (pool), mMatRing (pool),
          mNullText  (0), mMatPos (0), mMatUID (0)
{
  D_Try
  {
    LoadMatTable (_Dir);

    mMatCount    = mMatRing.count () + _DynCount;
    mMatNode     = (matnode_t*)alloc (sizeof (matnode_t) * mMatCount);

    if (!mMatNode)
    {
      D_Message ("MatEnvX: No enough memory for sys data");
      return;
    }

    mFirstFree              = mMatNode;
    
    for (int i=0;i<mMatCount;i++)
    {
      matnode_t& node = mMatNode [i];

      node.desc       = NULL;
      node.group      = NULL;
      node.difText    = node.specText = node.bumpText = -1;
      node.prev       = NULL;
      node.next       = &node + 1; 
    }

    mMatNode [mMatCount-1].next = NULL;

    CreateMatGroup (NULL,0,RENDER_GLOBAL_TEXTURES_CACHE_NUM); 

    mDefGroup               = &mGroupRing.GetFirst ().data ();

    MatRing::iterator iter  = mMatRing;
    int               count = mMatRing.count ();

    for (i=0;i<count;i++,iter++)
    {
      if (!iter.data().name [0])
        continue;

      ulong crc = CRC32 (iter.data().name,strlen (iter.data().name));

      if (!mFirstFree)
        break;

      matnode_t* node = mFirstFree;

      mFirstFree      = mFirstFree->next;

      node->desc      = &iter.data();
      node->difText   = node->specText = node->bumpText = -1;
      node->prev      = mDefGroup->last.prev;
      node->next      = &mDefGroup->last;
      node->group     = mDefGroup;

      mDefGroup->last.prev = node;
      node->prev->next     = node;
      mDefGroup->count++;

//      MATERIAL mat         = ulong(node-mMatNode);

//      if (!mMatDict.insert (crc,mat))
//      {
//        DelMat (mat);    
//        break;
//      }  
    }

    glGenTextures (1,&mNullText);

    AddLoader     ("bmp",LoadBMP);
    AddLoader     ("tga",LoadTGA);
    AddLoader     ("jpg",LoadJPEG);
    AddLoader     ("alp",LoadALP);
  }
  D_Exc ("MatEnvX: Error at construct");
}

MatEnvX::~MatEnvX        ()
{
  if (mTempPool) delete mTempPool;
}

matdesc_t::matdesc_t ()
          : blendKoef (1) 
{ 
  text.diffuse  [0] = 0; 
  text.specular [0] = 0; 
  text.bump     [0] = 0; 
  text.alpha    [0] = 0; 
  name          [0] = 0; 
  userLoader    [0] = userLoader [1] = userLoader [2] = userLoader [3] = 0;
}

matdesc_t::matdesc_t (
  const char* diffuse,
  const char* alpha,
  const char* bump,
  const char* specular )
{
  name          [0] = 0;
  text.diffuse  [0] = 0; 
  text.specular [0] = 0; 
  text.bump     [0] = 0; 
  text.alpha    [0] = 0; 
  userLoader    [0] = userLoader [1] = userLoader [2] = userLoader [3] = 0;

  if (diffuse)  strcpy (text.diffuse,diffuse);    
  if (alpha)    strcpy (text.alpha,alpha);    
  if (bump)     strcpy (text.bump,bump);
  if (specular) strcpy (text.specular,specular);
}

