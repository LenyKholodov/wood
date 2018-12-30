#include <pch.h>
#include <graph\x3f.h>
#include <graph\3ds.h>

#pragma pack(1)
struct   X3FHeader
{
  uchar         id [4];    //must be "X3DF"
  ulong         size;      //Размер объекта    
  int           main;      //Индекс корня дерева
};
#pragma pack(1)

#define mObjNode        objnode ()
#define mGroup          group ()
#define mLight          light ()
#define mMat            material ()
#define mScale          animscale ()
#define mTrans          animtrans ()
#define mRotate         animrot ()
#define mVertex         vertex ()
#define mFacet          facet ()
#define mAnim           animseg ();

static char            X3F_HEADER_ID [] = "X3DF";
static Object3D::Node  error (*(Object3D*)NULL,-1);

size_t      Object3D::mSizeof [Object3D::ELEMENTS_COUNT] = {
 sizeof (vertex_t),     //VERTEX
 sizeof (facet_t),      //FACET
 sizeof (vector3d_t),   //NORMAL
 sizeof (light_t),      //LIGHT
 sizeof (textmat_t),    //MATERIAL
 sizeof (group3d_t),    //GROUP
 sizeof (objnode_t),    //NODE
                                 
 sizeof (animseg_t),    //ANIM_SEG
 sizeof (vecseg_t),     //ANIM_TRANS
 sizeof (vecseg_t),     //ANIM_SCALE
 sizeof (quatseg_t),    //ANIM_ROTATE
};

////////////////////////implementation of class Object3D::Node//////////////////

Object3D::Node&    Object3D::Node::operator = (const Node& node)
{
  mOwner = node.mOwner;
  mNode  = node.mNode;

  return *this;
}

Object3D::Node        Object3D::Node::next   ()
{
  D_Try
  {
    return Node (*mOwner,mNode != -1 ? mOwner->mObjNode [mNode].next : -1);
  }
  D_Exception (EXCEPTION_EXECUTE_HANDLER)
  {
    #ifdef DEBUG
      dout<<"Object3D::Node: Error at next () "<<this<<endl;
    #endif               
    return error;
  }
}

Object3D::Node        Object3D::Node::node   ()
{
  D_Try
  {
    return Node (*mOwner,mNode != -1 ? mOwner->mObjNode [mNode].node : -1);
  }
  D_Exception (EXCEPTION_EXECUTE_HANDLER)
  {
    #ifdef DEBUG
      dout<<"Object3D::Node: Error at node () "<<this<<endl;
    #endif           
    return error;
  }
}

Object3D::Node        Object3D::Node::parent ()
{
  D_Try
  {
    return Node (*mOwner,mNode != -1 ? mOwner->mObjNode [mNode].parent : -1);
  }
  D_Exception (EXCEPTION_EXECUTE_HANDLER)
  {
    #ifdef DEBUG
      dout<<"Object3D::Node: Error at parent () "<<this<<endl;
    #endif          
    return error;
  }
}

const  bound_t&    Object3D::Node::bound () const
{
  static bound_t error;          
  D_Try
  {
    return mNode != -1 ? mOwner->mObjNode [mNode].bound : error;
  }
  D_Exception (EXCEPTION_EXECUTE_HANDLER)
  {
    #ifdef DEBUG
      dout<<"Object3D::Node: Error at bound () "<<this<<endl;
    #endif 
    return error;
  }  
}

const bound_t&   Object3D::Node::_bound ()
{
  D_Try
  {
    vertex_t  Min (MAX_BOUND,MAX_BOUND,MAX_BOUND),
              Max (MIN_BOUND,MIN_BOUND,MIN_BOUND);        
              
    _bound (Min,Max);

    return bound ();
  }
  D_Exception (EXCEPTION_EXECUTE_HANDLER)
  {
    #ifdef DEBUG
      dout<<"Object3D::Node: Error at _bound () "<<this<<endl;
    #endif
    static bound_t error;
    memset (&error,0,sizeof (bound_t));
    return error;
  }  
}

void   Object3D::Node::_bound (vector3d_t& Min,vector3d_t& Max)
{
  D_Try
  {
    if (!valid ())
      return;

    vector3d_t  c (.0f);
    vector3d_t  dim (.0f);

    if (mOwner->mObjNode [mNode].group != -1)
    {
      int         cnt     = count (VERTEX);

      if (cnt > 0)
      {
        vertex_t*   vrtx = vertex ();

        for (int i=0;i<cnt;i++)
          c += vrtx [i];
      
        c /= float (cnt);

        for (i=0;i<cnt;i++)
        {
          vector3d_t d = abs (c-vrtx [i]);

          if (d.x > dim.x) dim.x = d.x;
          if (d.y > dim.y) dim.y = d.y;
          if (d.z > dim.z) dim.z = d.z;
        }
      }
    }

    bound_t& b      = mOwner->mObjNode [mNode].bound;    

    if (mOwner->mObjNode [mNode].group != -1)
    {
      b.box.min       = c - dim;
      b.box.max       = c + dim;
    }
    else
    {
      b.box.min       = vector3d_t (MAX_BOUND,MAX_BOUND,MAX_BOUND);
      b.box.max       = vector3d_t (MIN_BOUND,MIN_BOUND,MIN_BOUND);
    }  

    for (Node iter = node ();iter.valid();iter = iter.next())    
      iter._bound (b.box.min,b.box.max);

    b.sphere.center = (b.box.max + b.box.min) / 2.0f;
    b.sphere.r      = length (b.box.max - b.box.min) / 2.0f;

    if (b.box.min.x < Min.x) Min.x = b.box.min.x;
    if (b.box.min.y < Min.y) Min.y = b.box.min.y;
    if (b.box.min.z < Min.z) Min.z = b.box.min.z;

    if (b.box.max.x > Max.x) Max.x = b.box.max.x;
    if (b.box.max.y > Max.y) Max.y = b.box.max.y;
    if (b.box.max.z > Max.z) Max.z = b.box.max.z;
  }
  D_Exception (EXCEPTION_EXECUTE_HANDLER)
  {
    #ifdef DEBUG
      dout<<"Object3D::Node: Error at _bound (vector3d_t&,vector3d_t&) "<<this<<endl;
    #endif    
  }
}

BOOL        Object3D::Node::more   () const
{
  D_Try
  {
    return mOwner && mNode != -1 && mOwner->mObjNode [mNode].next != -1;   
  }
  D_Exception (EXCEPTION_EXECUTE_HANDLER)
  {           
    #ifdef DEBUG
      dout<<"Object3D::Node: Error at more "<<this<<endl;
    #endif
    return FALSE;
  }
}

vertex_t*   Object3D::Node::vertex () const
{
  D_Try
  {
    if (mNode != -1 && mOwner->mObjNode [mNode].group != -1)
      return &mOwner->mVertex [mOwner->mGroup [mOwner->mObjNode [mNode].group].vertex.first];
    else
      return NULL; 
  }
  D_Exception (EXCEPTION_EXECUTE_HANDLER)
  {
    #ifdef DEBUG
      dout<<"Object3D::Node: Error at vertex () "<<this<<endl;
    #endif 
    
    return NULL;
  }  
}

vector3d_t*   Object3D::Node::normal () const
{
  D_Try
  {
    if (mNode != -1 && mOwner->mObjNode [mNode].group != -1)
      return mOwner->normal () + mOwner->mGroup [mOwner->mObjNode [mNode].group].normal.first;
    else
      return NULL; 
  }
  D_Exception (EXCEPTION_EXECUTE_HANDLER)
  {
    #ifdef DEBUG
      dout<<"Object3D::Node: Error at normal () "<<this<<endl;
    #endif 
    
    return NULL;
  }  
}

facet_t*    Object3D::Node::facet  () const
{
  D_Try
  {
    if (mNode != -1 && mOwner->mObjNode [mNode].group != -1)
      return &mOwner->mFacet [mOwner->mGroup [mOwner->mObjNode [mNode].group].facet.first];
    else 
      return NULL;
  }
  D_Exception (EXCEPTION_EXECUTE_HANDLER)
  {
    #ifdef DEBUG
      dout<<"Object3D::Node: Error at facet () "<<this<<endl;
    #endif 

    return NULL;
  }  
}

const animseg_t*    Object3D::Node::anim   () const
{
  D_Try
  {
    if (mNode != -1 && mOwner->mObjNode [mNode].anim.node != -1)
      return mOwner->animseg () + mOwner->objnode () [mNode].anim.node;
    else 
      return NULL;
  }
  D_Exception (EXCEPTION_EXECUTE_HANDLER)
  {
    #ifdef DEBUG
      dout<<"Object3D::Node: Error at anim () "<<this<<endl;
    #endif 

    return NULL;
  }      
}

const vecseg_t*     Object3D::Node::trans  () const
{
  D_Try
  {
    const animseg_t* anim = this->anim ();

    if (anim && anim->trans.count)
      return &mOwner->mTrans [anim->trans.first];
    else 
      return NULL;
  }
  D_Exception (EXCEPTION_EXECUTE_HANDLER)
  {
    #ifdef DEBUG
      dout<<"Object3D::Node: Error at trans () "<<this<<endl;
    #endif 

    return NULL;
  }      
}

const vecseg_t*     Object3D::Node::scale  () const
{
  D_Try
  {
    const animseg_t* anim = this->anim ();

    if (anim && anim->scale.count)
      return &mOwner->mScale [anim->scale.first];
    else 
      return NULL;
  }
  D_Exception (EXCEPTION_EXECUTE_HANDLER)
  {
    #ifdef DEBUG
      dout<<"Object3D::Node: Error at scale () "<<this<<endl;
    #endif 

    return NULL;
  }      
}

const quatseg_t*   Object3D::Node::rotate () const
{
  D_Try
  {
    const animseg_t* anim = this->anim ();

    if (anim && anim->rot.count)
      return &mOwner->mRotate [anim->rot.first];
    else 
      return NULL;
  }
  D_Exception (EXCEPTION_EXECUTE_HANDLER)
  {
    #ifdef DEBUG
      dout<<"Object3D::Node: Error at scale () "<<this<<endl;
    #endif 

    return NULL;
  }      
}

int         Object3D::Node::count  (int sel) const
{
  D_Try
  {
    if (mNode != -1 && mOwner->mObjNode [mNode].group != -1)    
    {
      group3d_t& group = mOwner->mGroup [mOwner->mObjNode [mNode].group];
      const
      animseg_t* anim  = this->anim ();

      switch (sel)
      {
        case VERTEX:      return group.vertex.count;
        case FACET:       return group.facet.count;
        case NORMAL:      return group.normal.count;
        case ANIM_SEG:    return anim ? 1 : 0;
        case ANIM_TRANS:  return anim ? anim->trans.count : 0;
        case ANIM_SCALE:  return anim ? anim->scale.count : 0;
        case ANIM_ROTATE: return anim ? anim->rot.count : 0;
//        CASE GROUP:
//        CASE NODE:
        default:       return -1;
      }
    }
    else
      return -1;
  }
  D_Exception (EXCEPTION_EXECUTE_HANDLER)
  {
    #ifdef DEBUG
      dout<<"Object3D::Node: Error count () "<<this<<endl;
    #endif 

    return -1;
  }  
}

Object3D::Node::Node  (Object3D& obj,int _ObjNode)
               : mOwner (&obj),
                 mNode  (_ObjNode)
{
}

Object3D::Node::Node  (const Node& node)
               : mNode  (node.mNode),
                 mOwner (node.mOwner)
{
}

Object3D::Node::Node  ()
               : mNode  (-1),
                 mOwner (NULL)
{
}

///////////////implementation of class Object3D/////////////////////////////////

Object3D::Node        Object3D::main  ()
{
  D_Try
  {
    return Node (*this,mMain);
  }
  D_Exception (EXCEPTION_EXECUTE_HANDLER)
  {
    #ifdef DEBUG
      dout<<"Object3D: Error at create node (main()) "<<this<<endl;
    #endif
    return error;
  }
}

void  Object3D::SetMatPrefix (const char* prefix)
{
  D_Try
  {
    if (!prefix)
      return;

    textmat_t* m = material ();
    int        c = count    (MATERIAL);
    char       temp [1024];

    for (int i=0;i<c;i++,m++)
    {
      strcpy (temp,m->name);
      strcpy (m->name,prefix);
      strcat (m->name,temp);
    }      
  }
  D_Exc ("Object3D: Error at SetMatPrefix");  
}

const bound_t&    Object3D::_bound    ()
{
  Node node = Node (*this,mMain);

  return node._bound ();
}

const textmat_t&      Object3D::GetMaterial (uint index) const
{
  static textmat_t null (0);

  D_Try
  {
     if (index < mCount [MATERIAL])     return mMat [index];
     else                               return null;
  }
  D_Exception (EXCEPTION_EXECUTE_HANDLER)
  {
    #ifdef DEBUG
      dout<<"Object3D: Error at GetMaterial ("<<index<<") "<<this<<endl;
    #endif
    return null; 
  }
}

const light_t&     Object3D::GetLight    (uint index) const
{
  static light_t null;

  D_Try
  {
     if (index < mCount [LIGHT])     return mLight [index];
     else                            return null;
  }
  D_Exception (EXCEPTION_EXECUTE_HANDLER)
  {
    #ifdef DEBUG
      dout<<"Object3D: Error at GetLight ("<<index<<") "<<this<<endl;
    #endif
    return null; 
  }
}

BOOL               Object3D::GetEnv      (uint _enum,uint index,void* buf,uint count) const
{
  D_Try
  {
    switch (_enum)
    {
      if (_enum < ELEMENTS_COUNT)
        memcpy (buf,mPtr [_enum] + index * mSizeof [_enum],mSizeof [_enum] * count);
      else
        return FALSE;   
    }

    return TRUE;
  }
  D_Exception (EXCEPTION_EXECUTE_HANDLER)
  {
    #ifdef DEBUG
      dout<<"Object3D: Error at GetEnv () "<<this<<endl;
    #endif
    return FALSE;
  }
}

int         Object3D::count (int sel) const
{
  if (sel < ELEMENTS_COUNT)    return mCount [sel];
  else                         return -1;
}

BOOL            Object3D::Load3DS (const char* name,size_t _TempPoolSize)
{
  D_Try
  {
    BinFile* file   = F_OpenFile (name,BF_OPEN_RO);

    if (IsValid (file))
    {
      bool state = Load3DS (file,_TempPoolSize);

      delete file;

      return state;
    }
    else
    {
      #ifdef DEBUG
        dout<<"Object3D: Error at open file '"<<name<<"'"<<endl;
      #endif
      delete file;
      return FALSE;
    }  
  }
  D_Exception (EXCEPTION_EXECUTE_HANDLER)
  {
    return FALSE;    
  }
}

BOOL            Object3D::Load3DS (BinFile* file,size_t _TempPoolSize)
{
//  D_Try
//  {
    Loader3DS load (this,file,&TempPool (GetPool (),_TempPoolSize));

    return TRUE;
//  }
//  D_Exception (EXCEPTION_EXECUTE_HANDLER)
//  {
//    #ifdef DEBUG
//      dout<<"Object3D: Error at Load3DS (BinFile*) "<<this<<endl;
//    #endif

//    return FALSE;
//  }
}

void            Object3D::Save   (const char* name)
{  
  D_Try
  {
    BinFile* file   = F_OpenFile (name,BF_OPEN_RW);

    Save        (file);
    F_CloseFile (file);
  }
  D_Exception (EXCEPTION_EXECUTE_HANDLER)
  {
    #ifdef DEBUG
      dout<<"Object3D: Error at Save (const char*) "<<this<<endl;
    #endif    
  }
}

void            Object3D::Save   (BinFile* file)
{
  D_Try
  {
    if (!IsValid (file))
      return;  

    X3FHeader header;  

    memcpy (header.id,X3F_HEADER_ID,4);

    size_t size = 0;

    for (int i=0;i<ELEMENTS_COUNT;size+=mSizeof [i] * mCount [i],i++);

    header.size = size;
    header.main = mMain;

    file->write (&header,sizeof (header));
    file->write (mCount,sizeof (mCount));
    file->write (mSpace,header.size);
  }
  D_Exception (EXCEPTION_EXECUTE_HANDLER)
  {
    #ifdef DEBUG
      dout<<"Object3D: Error at Save (BinFile*) "<<this<<endl;
    #endif    
  }
}

BOOL            Object3D::Load   (const char* name)
{
  D_Try
  {
    BinFile* file   = F_OpenFile (name,BF_OPEN_RO);
    BOOL     state  = Load       (file);

    F_CloseFile (file);

    return state;
  }
  D_Exception (EXCEPTION_EXECUTE_HANDLER)
  {
    return FALSE;
  }    
}

BOOL            Object3D::Load   (BinFile* file)
{
  D_Try
  {
    if (!IsValid (file))
      return FALSE;

    X3FHeader header;  
  
    file->read (&header,sizeof (header));

    if (memcmp (X3F_HEADER_ID,header.id,4))
      return FALSE;

    if (mSpace)
    {
      free (mSpace);
      
      mSpace = NULL;

      for (int i=0;i<ELEMENTS_COUNT;mPtr [i] = NULL,mCount [i] = 0,i++);
    }

    file->read (mCount,sizeof (mCount));
    
    char* buf = (char*)alloc (header.size);

    if (buf == NULL)
    {
      #ifdef DEBUG
        dout<<"Object3D: No enough memory "<<this<<endl;
      #endif      
      return FALSE;
    }

    ulong pos = 0;

    for (int i=0;i<ELEMENTS_COUNT;pos+=mSizeof [i] * mCount [i],i++)
      mPtr [i] = buf + pos;

    mSpace   = buf;
    mMain    = header.main;

    file->read (mSpace,header.size);

    return TRUE;
  }
  D_Exception (EXCEPTION_EXECUTE_HANDLER)
  {
    #ifdef DEBUG
      dout<<"Object3D: Error at Load (BinFile*) "<<this<<endl;
    #endif
    return FALSE;
  }
}

Object3D::Object3D  (Pool* _Pool,const char* name,size_t size)
         : MemObject (_Pool),
           mMain     (-1),
           mSpace    (NULL)
{
  for (int i=0;i<ELEMENTS_COUNT;i++)
  {
    mCount [i] = 0;
    mPtr   [i] = NULL;
  }  

  if (!Load (name))
  {
    if (!Load3DS (name,size))
    {
      #ifdef DEBUG
        dout<<"Object3D: Unknown file format '"<<name<<"' "<<this<<endl;
      #endif
    }
  }  
}

Object3D::Object3D  (Pool* _Pool,BinFile* file,size_t size)
         : MemObject (_Pool),
           mSpace    (NULL),
           mMain     (-1)
{
  for (int i=0;i<ELEMENTS_COUNT;i++)
  {
    mCount [i] = 0;
    mPtr   [i] = NULL;
  }  

  if (!Load (file))
    if (!Load3DS (file,size))
    {
      #ifdef DEBUG
        dout<<"Object3D: Unknown file format file: "<<file<<endl;
      #endif
    }
}

Object3D::Object3D  (Pool* _Pool)
         : MemObject (_Pool),
           mSpace    (NULL),
           mMain     (-1)
{
  for (int i=0;i<ELEMENTS_COUNT;i++)
  {
    mCount [i] = 0;
    mPtr   [i] = NULL;
  }  
}

Object3D::~Object3D ()
{
  if (mSpace) free (mSpace);
}

/////////////////implementation of class Loader3D///////////////////////////////

int          X3FLoader::count (int _Enum) const
{
  D_Try
  {
    return mObject->mCount [_Enum];
  }
  D_Exception (EXCEPTION_EXECUTE_HANDLER)
  {
    #ifdef DEBUG
      dout<<"X3FLoader: Error at count (int) "<<this<<endl;
    #endif
    return -1;
  }
}

void         X3FLoader::count (int _Enum,int _Count)
{
  D_Try
  {
    mObject->mCount [_Enum] = _Count;
  }
  D_Exception (EXCEPTION_EXECUTE_HANDLER)
  {
    #ifdef DEBUG
      dout<<"X3FLoader: Error at count (int,int) "<<this<<endl;
    #endif
  }
}

void         X3FLoader::alloc ()
{
  D_Try
  {  
    free ();

    size_t size = 0;
    ulong  pos  = 0;

    for (int i=0;i<Object3D::ELEMENTS_COUNT;size+=Object3D::mSizeof [i] * mObject->mCount [i],i++);

    mObject->mMain    = 0;
    mObject->mSpace   = (char*)mObject->alloc (size);

    for (i=0;i<Object3D::ELEMENTS_COUNT;pos+=Object3D::mSizeof [i] * mObject->mCount [i],i++)
      mObject->mPtr [i] = mObject->mSpace + pos;
  }
  D_Exception (EXCEPTION_EXECUTE_HANDLER)
  {
    #ifdef DEBUG
      dout << "X3FLoader: Error at alloc () "<<this<<endl;
    #endif
  }
}

void         X3FLoader::free  ()
{
  D_Try
  {
    if (mObject->mSpace)
      mObject->free (mObject->mSpace);
    
    mObject->mMain    = -1;  
    mObject->mSpace   = NULL;

    for (int i=0;i<Object3D::ELEMENTS_COUNT;mObject->mPtr [i++] = NULL);

      //Count не очищать!
  }
  D_Exception  (EXCEPTION_EXECUTE_HANDLER)
  {            
    #ifdef DEBUG
      dout<<"X3FLoader: Error at free () "<<this<<endl;
    #endif
  }
}

void*        X3FLoader::get   (int _Enum)
{
  D_Try
  {
    if (_Enum < Object3D::ELEMENTS_COUNT) return mObject->mPtr [_Enum];
    else                                  return NULL;    
  }
  D_Exception (EXCEPTION_EXECUTE_HANDLER)
  {
    #ifdef DEBUG
      dout<<"X3FLoader: Error at get "<<this<<endl;
    #endif
    return NULL;
  }
}

void         X3FLoader::main  (int _Main)
{
  D_Try
  {
    mObject->mMain = _Main;
  }
  D_Exception (EXCEPTION_EXECUTE_HANDLER)
  {
    #ifdef DEBUG
      dout<<"X3FLoader: Error at main (int) "<<this<<endl;
    #endif
  }
}

int          X3FLoader::main  () const
{
  D_Try
  {
    return mObject->mMain;
  }
  D_Exception (EXCEPTION_EXECUTE_HANDLER)
  {
    #ifdef DEBUG
      dout<<"X3FLoader: Error at main () "<<this<<endl;
    #endif
    return -1;
  }
}

X3FLoader::X3FLoader  (Object3D* _Object,BinFile* _File,Pool* _Pool)
          : MemObject (_Pool ? _Pool : _Object->GetPool ()),
            mObject   (_Object),
            mFile     (_File)
{       }

X3FLoader::X3FLoader  (Object3D* _Object,Pool* _Pool)
          : MemObject (_Pool ? _Pool : _Object->GetPool ()),
            mObject   (_Object),
            mFile     (NULL)
{       }


X3FLoader::~X3FLoader ()
{       }
