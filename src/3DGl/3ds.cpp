#include <pch.h>
#include <graph\3ds.h>
#include <graph\alp.h>
#include <math3d.h>

matrix_t minusX = matrix_t::scale (vector3d_t (-1,1,1));
extern BOOL     CONV3DS_CONVERT_ALPHA_TEXTURES = FALSE; 

/////////////////implementation of class Trimesh3DS/////////////////////////////

vertexlist3ds_t*    Trimesh3DS::createVertexList (BinFile* file)
{
  int16 count;

  file->read (&count,sizeof (int16));

  vertexlist3ds_t* vert = (vertexlist3ds_t*)GetPool ()->alloc (sizeof (vertexlist3ds_t)+
                                                    (long)(count)*sizeof (vertex_t));
  if (vert == NULL)
  {
    D_NoMemory ("Trimesh3DS",sizeof (vertexlist3ds_t)+
                             (long)(count)*sizeof (vertex_t));
    return NULL;                             
//    D_Error (EXCEPTION_NO_MEMORY);
  }

  vert->count = count;
  
  file->read (&vert->data,sizeof (vertex_t) * (long)count);

  return vert;
}

facetlist3ds_t*    Trimesh3DS::createFacetList (BinFile* file)
{
  uint16 count;

  file->read (&count,sizeof (uint16));

  facetlist3ds_t* facet = (facetlist3ds_t*)GetPool ()->alloc (sizeof (facetlist3ds_t)+
                                                   (long)(count)*sizeof (facet3ds_t));
  if (facet == NULL)                               
  {
    
    #ifdef DEBUG
      dout<<"Trimesh3DS: No enough memory "<<endl;
    #endif
    return NULL;
  }  

  facet->count = count;
  
  file->read (&facet->data,sizeof (facet3ds_t) * (long)count);

  return facet;
}

textlist3ds_t*    Trimesh3DS::createTextList (BinFile* file)
{
  int16 count;

  file->read (&count,sizeof (int16));

  textlist3ds_t* text = (textlist3ds_t*)GetPool ()->alloc (sizeof (textlist3ds_t)+
                                                   (long)(count)*sizeof (textcoord_t));
  if (text == NULL)
  {
    #ifdef DEBUG
      dout<<"Trimesh3DS: No enough memory "<<endl;
    #endif
    return NULL;
//    D_Error (EXCEPTION_NO_MEMORY);      
  }

  text->count = count;
  
  file->read (&text->data,sizeof (textcoord_t) * (long)count);

  return text;
}

matlist3ds_t*    Trimesh3DS::createMatList (BinFile* file)
{
  uint16        count;
  char          matName [128];

  LoadASCIIZ (file,matName);
  file->read (&count,sizeof (uint16));

  matlist3ds_t* mat = (matlist3ds_t*)GetPool ()->alloc (sizeof (matlist3ds_t)+
                                                   (long)(count)*sizeof (uint16));
  if (mat == NULL)
   return NULL;
//    D_Error (EXCEPTION_NO_MEMORY);

  mat->count = count;

  strcpy     (mat->name,matName);
  file->read (&mat->data,sizeof (uint16) * (long)count);

  return mat;
}

matrix_t*   Trimesh3DS::createTransMatrix (BinFile* file)
{
  matrix3ds_t   trans3ds;

  file->read (&trans3ds,sizeof (matrix3ds_t));  

  matrix_t*     m = (matrix_t*)GetPool ()->alloc (sizeof (matrix_t));
  
  if (m == NULL)
  {
    #ifdef DEBUG
      dout<<"Trimesh3DS: No enough memory "<<endl;
    #endif
    return NULL;
//    D_Error (EXCEPTION_NO_MEMORY);
  }

  matrix_t&   x   =  *m;

  x [0][0]        = trans3ds.m [0][0];  
  x [0][1]        = trans3ds.m [0][1];  
  x [0][2]        = trans3ds.m [0][2];  
  x [0][3]        = 0;  

  x [1][0]        = trans3ds.m [1][0];  
  x [1][1]        = trans3ds.m [1][1];  
  x [1][2]        = trans3ds.m [1][2];  
  x [1][3]        = 0;  

  x [2][0]        = trans3ds.m [2][0];  
  x [2][1]        = trans3ds.m [2][1];  
  x [2][2]        = trans3ds.m [2][2];  
  x [2][3]        = 0;  

  x [3][0]        = 0;
  x [3][1]        = 0;
  x [3][2]        = 0;
  x [3][3]        = 1;

  for (int i=0;i<4;i++)
    swap (x[i][1],x[i][2]); 

  for (i=0;i<4;i++)
    swap (x[1][i],x[2][i]);

  for (i=0;i<3;i++)
  {
    vector3d_t v (x [i][0],x [i][1],x [i][2]);
    v.normalize ();

    x [i][0] = v.x;
    x [i][1] = v.y;
    x [i][2] = v.z;
  }  

  offs = vector3d_t (-trans3ds.offs.x,-trans3ds.offs.y,-trans3ds.offs.z);

  return m;
}

Trimesh3DS::Trimesh3DS  (Pool* _Pool,BinFile* file,size_t size)
        : MemObject (_Pool),
          vertex    (NULL),
          facet     (NULL),
          text      (NULL),
          trans     (NULL),
          mat       (_Pool),
          offs      (.0f)          
{
  chunk_t       chunk;
  long          pos  = file->getpos () - sizeof (chunk_t);
  
  for (;!file->eof() && file->getpos() - pos < size;)
  {
//    dout<<file->getpos ()<<" "<<file->size ()<<" trimesh "<<endl;   
    file->read (&chunk,sizeof (chunk));

    switch (chunk.id)
    {
        case CHUNK_TRMATRIX:
          trans = createTransMatrix (file);
          break;
        case CHUNK_VERTLIST:        
          vertex    = createVertexList (file);
          break;
        case CHUNK_FACELIST:
          facet    = createFacetList (file);
          break;
        case CHUNK_FACEMAT:
          mat.insert (createMatList (file));         
          break;
        case CHUNK_MAPLIST:
          text = createTextList (file);
          break;
        default:
          file->seek (file->getpos()+chunk.len-sizeof (chunk_t));
          break;
    }
  }      

  file->seek (pos+size);
}

Trimesh3DS::~Trimesh3DS ()
{
  if (vertex) GetPool ()->free (vertex);
  if (facet)  GetPool ()->free (facet);
  if (text)   GetPool ()->free (text);
  if (trans)  GetPool ()->free (trans);

  int                               count = mat.count();
  RingTmpl<matlist3ds_t*>::iterator iter  = mat;
 
  for (int i=0;i<count;i++,iter++)
    GetPool ()->free ( iter.data() );
}

///////////////implementation of class DircetLight3DS///////////////////////////

DirectLight3DS::DirectLight3DS  (Pool* pool,BinFile* file,size_t size)
               : MemObject (pool),
                 angle     (0),
                 exp       (0)
{
  chunk_t       chunk;
  long          _pos  = file->getpos () - sizeof (chunk_t);
  float         temp;

  file->read (&pos,sizeof (vector3d_t));
  
  for (;!file->eof() && file->getpos() - _pos < size;)
  {
//      dout<<file->getpos ()<<" "<<file->size ()<<" light "<<endl;   
    file->read (&chunk,sizeof (chunk));

    switch (chunk.id)
    {  
      case CHUNK_DL_SPOT_LIGHT:                      
        file->read (&target,sizeof (target));     
        file->read (&temp,sizeof (temp));
        file->read (&angle,sizeof (angle));
        break;
      case CHUNK_DL_MULTIPLIER:
        file->read (&exp,sizeof (exp));
        break;
      case CHUNK_COLOR_F:  
      case CHUNK_LIN_COLOR_F:
        LoadColorF (file,color);
        break;
      case CHUNK_COLOR_24:  
      case CHUNK_LIN_COLOR_24:
        LoadColor24 (file,color);
        color /= 255.0f;
        break;
      default:
        file->seek (file->getpos()+chunk.len-sizeof (chunk_t));
        break;
    }
  }      

  file->seek (_pos+size);
}

DirectLight3DS::~DirectLight3DS ()
{
}

//////////////implementation of class Object3DS/////////////////////////////////

Object3DS::Object3DS   (Pool* _Pool,BinFile* file,size_t size,Mesh3DS& mesh)
         : MemObject (_Pool),
           block     (NULL),
//           light     (NULL),
           node      (-1),
           parent    (-1)
{
  chunk_t       chunk;
  long          pos   = file->getpos () - sizeof (chunk_t);

  LoadASCIIZ (file,name);

  #ifdef __3DS_LOAD_LOG__
    dout<<"Object3DS: Load NAMED_OBJECT: "<<name<<" "<<size<<endl;
  #endif
  
  for (;!file->eof() && file->getpos() - pos < size;)
  {
//      dout<<file->getpos ()<<" "<<file->size ()<<" obj3d "<<endl;   
    file->read (&chunk,sizeof (chunk));

    switch (chunk.id)
    {
        case CHUNK_TRIMESH:          
          block = new (GetPool ()) Trimesh3DS (GetPool (),file,chunk.len);
          mesh.ring.insert (this);
          mesh.node.insert (CRC32 (name,strlen(name)),this);
          break;
        case CHUNK_N_DIRECT_LIGHT:
          mesh.light.insert (new (GetPool ()) DirectLight3DS (GetPool (),file,chunk.len));
          break;  
        default:
          file->seek (file->getpos()+chunk.len-sizeof (chunk_t));
          break;
    }
  }          

  file->seek (pos+size);
}

Object3DS::Object3DS  (Pool* _Pool,int _Node,int _Parent)
          : MemObject (_Pool),
            parent    (_Parent),
            node      (_Node),
            block     (NULL),
            anim      (NULL)
//            light     (NULL)
{  
  name [0] = 0;
}

Object3DS::~Object3DS ()
{
  if (block) delete block;  
  if (anim)  delete anim;
//  if (light) delete light;
}

//////////////////implementation of class Material3DS///////////////////////////

Material3DS::Material3DS  (Pool* _Pool,BinFile* file,size_t size)
           : MemObject (_Pool)             
{
  chunk_t       chunk;
  long          pos  = file->getpos () - sizeof (chunk_t);
  vector3d_t*   color;
  float*        flData;
  char*         bmp = NULL;
//  char          alpText [128];
  BOOL          alpPresent = FALSE;

  m.emission = vector3d_t (.0f);
  
  for (;!file->eof() && file->getpos() - pos < size;)
  {
//      dout<<file->getpos ()<<" "<<file->size ()<<" material "<<endl;   
    file->read (&chunk,sizeof (chunk));

    switch (chunk.id)
    {
        case CHUNK_MAT_NAME:
          LoadASCIIZ (file,name);
          #ifdef __3DS_LOAD_LOG__
            dout<<"Material3DS: Load material "<<name<<endl;
          #endif  
          break;
        case CHUNK_MAT_AMBIENT:
          color = &m.ambient;
          break;   
        case CHUNK_MAT_DIFFUSE:
          color = &m.diffuse;
          break;   
        case CHUNK_MAT_SPECULAR:
          color = &m.specular;          
          break;
        case CHUNK_MAT_SHINESS:
          flData = &m.shiness;
          break;   
        case CHUNK_LIN_COLOR_24: 
        case CHUNK_COLOR_24:       
          LoadColor24 (file,*color);
          *color /= 255.0f;
          break;
        case CHUNK_LIN_COLOR_F:   
        case CHUNK_COLOR_F:
          LoadColorF (file,*color);
          break;
        case CHUNK_INT_PERCENTAGE:
          int16 per;
          LoadPercentInt (file,per);
          *flData = per;
          break;
        case CHUNK_TEXTURE:  
          bmp = text;
          break;
        case CHUNK_ALPHA_TEXTURE:
          bmp = alp;
          alpPresent = TRUE;
          break;
        case CHUNK_MAPFILE:
          LoadASCIIZ (file,bmp);
          #ifdef __3DS_LOAD_LOG__
            dout<<"Material3DS: Load texture "<<bmp<<endl;
          #endif
          break;  
        default:
          file->seek (file->getpos()+chunk.len-sizeof (chunk_t));
          break;
    }
  }

  locase (text,text);

  if (alpPresent && CONV3DS_CONVERT_ALPHA_TEXTURES)
    ConvertAlpText (text,alp);

  if (alpPresent)    m.blend = TRUE  ;
  else               m.blend = FALSE;

//  char* ext = strchr (text,'.');

//  if (ext)
//  { 
//    if (!strcmp (ext,".alp"))    m.blend = TRUE;
//    else                         m.blend = FALSE;
//  }
//  else
//    m.blend = FALSE;

  file->seek (pos+size);
}

Material3DS::~Material3DS ()
{
}

//////////////////////implementation of class Anim3DS///////////////////////////

Anim3DS::Anim3DS  (Pool* _Pool)
        : MemObject (_Pool),
          rotCount (0), scaleCount (0), transCount (0),
          trans (NULL), scale (NULL), rot (NULL)          
{  }

Anim3DS::~Anim3DS ()
{
  if (rot)   GetPool ()->free (rot);
  if (scale) GetPool ()->free (scale);
  if (trans) GetPool ()->free (trans);

  rot   = NULL;
  scale = NULL;
  trans = NULL;
}

/////////////////////implementation of class Node3DS////////////////////////////

Node3DS::Node3DS  (Pool* _Pool,BinFile* file,size_t size,Mesh3DS& mesh)
        : MemObject (_Pool)
{
  chunk_t       chunk;
  long          pos    = file->getpos () - sizeof (chunk_t);
  int16         parent = -1;
  int16         node   = -1;
  Object3DS*    obj    = NULL;

//  D_Try
//  {  
    for (;!file->eof() && file->getpos() - pos < size;)
    {
//      dout<<file->getpos ()<<" "<<file->size ()<<" node3d "<<endl;   
      file->read (&chunk,sizeof (chunk));

      switch (chunk.id)
      {
        case CHUNK_NODE_ID:
          file->read (&node,sizeof (int16));
          break;
        case CHUNK_NODE_HDR:
          char    name  [128];
          int16   flag  [2];

          LoadASCIIZ (file,name);
          
          file->read  (flag,sizeof (int16) * 2);
          file->read  (&parent,sizeof (int16));

//          obj       = mesh.node.search (CRC32 (name,strlen(name))); //.data ()
          obj       = mesh.node.search (CRC32 (name,strlen(name))).data ();

          if (obj == NULL)
            mesh.ring.insert (obj = new (GetPool ()) Object3DS (GetPool (),node,parent));
          else
          {  
            obj->node    = node;
            obj->parent  = parent;
          }
          
          obj->anim = new (GetPool ()) Anim3DS (GetPool ());

          break;
        case CHUNK_PIVOT:
          vector3ds_t v;

          file->read (&v,sizeof (vector3ds_t));

          obj->anim->pivot = vector3d_t (v.x,v.y,v.z);
          
          break;  
        case CHUNK_POS_TRACK_TAG:
        {
          obj->anim->trans = LoadVecAnim (file,GetPool (),obj->anim->transCount);
          break;
        }  
        case CHUNK_ROT_TRACK_TAG:
        {
          obj->anim->rot = LoadQuatAnim (file,GetPool (),obj->anim->rotCount);
          break;
        }
        case CHUNK_SCL_TRACK_TAG:
        {
          obj->anim->scale = LoadVecAnim (file,GetPool (),obj->anim->scaleCount);
          break;
        }  
        default:
          file->seek (file->getpos()+chunk.len-sizeof (chunk_t));
          break;
      }
    }   
//  }
//  D_Exception (EXCEPTION_EXECUTE_HANDLER)
//  {
//    #ifdef DEBUG
//      dout<<"Node3DS: Error at construct "<<this<<endl;
//    #endif
//  }

  file->seek (pos+size);
}

Node3DS::~Node3DS ()
{
}

////////////////////implementation of class Mesh3DS/////////////////////////////

Mesh3DS::Mesh3DS  (Pool* _Pool,BinFile* file,size_t size)
       : MemObject (_Pool),
         ring      (_Pool),
         dict      (_Pool),
         idict     (_Pool),
         node      (_Pool),
         light     (_Pool),
         mcount    (0),
         acount    (0)

                   //Считывать длину анимации
{
  chunk_t     chunk;
  long        pos  = file->getpos () - sizeof (chunk_t);
  Material3DS* mat  = NULL;     

  for (;!file->eof() && file->getpos() - pos < size;)
  {
//      dout<<file->getpos ()<<" "<<file->size ()<<" mesh "<<endl;   
    file->read (&chunk,sizeof (chunk));

    switch (chunk.id)
    {
      case CHUNK_MAT_ENTRY:
        mat = new (GetPool ()) Material3DS (GetPool (),file,chunk.len);

        if (mat)
        {
          idict.insert (CRC32 (mat->name,strlen (mat->name)),mcount);
          dict.insert  (mcount,mat);

          mcount++;
        }
        break;  
      case CHUNK_OBJBLOCK:
        new (GetPool ()) Object3DS (GetPool (),file,chunk.len,*this);
        break;
      default:
        file->seek (file->getpos()+chunk.len-sizeof (chunk_t));
        break;        
    }  
  }

  file->seek (pos+size);
}

Mesh3DS::~Mesh3DS ()
{
  RingTmpl<Object3DS*>::iterator iter   = ring;
  int                           count  = ring.count ();

  for (int i=0;i<count;i++,iter++)
    delete iter.data ();

  count                                 = light.count ();
  RingTmpl<DirectLight3DS*>::iterator l = light;
  
  for (i=0;i<count;i++,l++)
    delete l.data ();
}

/////////////////////////implementation of class KFData3DS//////////////////////

KFData3DS::KFData3DS  (Pool* _Pool,BinFile* file,size_t size,Mesh3DS* mesh)
          : MemObject (_Pool)            
{
//  D_Try
//  {
    chunk_t     chunk;
    long        pos  = file->getpos () - sizeof (chunk_t);

    for (;!file->eof() && file->getpos() - pos < size;)
    {
//      dout<<file->getpos ()<<" "<<file->size ()<<" kfdata "<<endl;   
      file->read (&chunk,sizeof (chunk));

      switch (chunk.id)
      {
        case CHUNK_OBJECT_NODE_TAG:      
          Node3DS (GetPool (),file,chunk.len,*mesh);
          break;
        default:
          file->seek (file->getpos()+chunk.len-sizeof (chunk_t));
          break;        
      }  
    }

    file->seek (pos+size);
//  }
//  D_Exception (EXCEPTION_EXECUTE_HANDLER) 
//  {
//    #ifdef DEBUG
//      dout<<"KFData: Error at construct "<<this<<endl;
//    #endif
//  }
}

KFData3DS::~KFData3DS ()
{
}

/////////////////////////Utils//////////////////////////////////////////////////

void   ConvertAlpText (char* text,const char* alpText)
{
  char dest [256];

  strcpy (dest,text);

  char* str = strchr (dest,'.');

  strcpy (str,".alp");

  char*         ext        = strchr (text,'.');
  int           format [2] = {FMT_UNKNOWN};

  locase (ext,ext);

  if      (!strcmp (ext,".bmp"))  format [0] = FMT_BMP;
  else if (!strcmp (ext,".jpg"))  format [0] = FMT_JPEG;  
  else if (!strcmp (ext,".tga"))  format [0] = FMT_TGA;

  ext        = strchr (alpText,'.');
  locase (ext,ext);

  if      (!strcmp (ext,".bmp"))  format [1] = FMT_BMP;
  else if (!strcmp (ext,".jpg"))  format [1] = FMT_JPEG;  
  else if (!strcmp (ext,".tga"))  format [1] = FMT_TGA;

  dout<<text<<" "<<alpText<<" "<<dest<<endl;

  BinFile* src  = F_OpenFile ("textures",text,BF_OPEN_RO);
  BinFile* alp  = F_OpenFile ("textures",alpText,BF_OPEN_RO);
  BinFile* file = F_OpenFile ("textures",dest,BF_OPEN_CR);

  SaveALP         (file,src,alp,M_GetPool (),format);

  delete src, file, alp;

  strcpy          (text,dest);
}

void   LoadASCIIZ  (BinFile* file,char* str)
{
  char* pos = str-1;
  do{    
    file->read (++pos,1);
  }while (*pos);
}  

void   LoadColorF (BinFile* file,vector3d_t& color)
{
  file->read (&color,sizeof (color));
}

void   LoadColor24 (BinFile* file,vector3d_t& color)
{
  uchar data [3];

  file->read (data,3*sizeof (char));

  color.x = data [0];
  color.y = data [1];
  color.z = data [2];
}

void   LoadPercentInt (BinFile* file,int16& data)
{
  file->read (&data,sizeof (data));
}  

vecseg_t*   LoadVecAnim    (BinFile* file,Pool* pool,int& count)
{
  D_Try
  {
    typedef vector3d_t  vecarray_t [2];

    trackpos3ds_t   pos;
    vecseg_t*       anim = NULL;
    vecarray_t*     p    = NULL;

    file->read (&pos.main,sizeof (pos.main));
    pos.pos = (position3ds_t*)pool->alloc (sizeof (position3ds_t) * pos.main.num);
    anim    = pos.main.num ? 
              (vecseg3ds_t*)pool->alloc (pos.main.num * sizeof (vecseg3ds_t)) :
              NULL;
    p       = pos.main.num ?
              (vecarray_t*)pool->alloc (pos.main.num * sizeof (vecarray_t)) :
              NULL;
    count   = pos.main.num;

    for (int i=0;i<pos.main.num;i++)
    {
      uint   frame;
      uint16 flags;
      uint   mask = 1;

      file->read (&frame,sizeof (frame));
      file->read (&flags,sizeof (flags));

      if (flags) dout<<"VEC: "<<flags<<endl;

      pos.pos [i].frame = frame;
      pos.pos [i].flags = flags;

      for (int j=0;j<5;j++,mask<<=1)
      {
        if (flags & mask) file->read (&pos.pos [i].spline [j],sizeof (float));
        else              pos.pos [i].spline [j] = 0;
      }  

      file->read (&pos.pos [i].pos,sizeof (vector3ds_t)); 

      if (i)
      {
        p [i][0]   = vector3d_t (pos.pos [i].pos.x,pos.pos [i].pos.y,pos.pos [i].pos.z);
        p [i-1][1] = p [i][0];
      }  
      else
        p [i][0]   = vector3d_t (pos.pos [i].pos.x,pos.pos [i].pos.y,pos.pos [i].pos.z);
    }

    if (pos.main.num)
    {
      p [pos.main.num-1][1] = p [pos.main.num-1][0];

      for (int i=0;i<pos.main.num;i++)
      {
        vecseg3ds_t& seg = anim [i];

        seg.easeFrom     = pos.pos [i].spline [EASE_FROM];
        seg.easeTo       = pos.pos [i].spline [EASE_TO];
        seg.start        = pos.pos [i].frame;

        if (i != pos.main.num - 1)
        {
          vector3d_t vec [2];
          float* r [2] = {(float*)&vec [0],(float*)&vec [1]};
          float  g1,g2,g3;
          float  bias       = pos.pos [i].spline [BIAS],
                 tension    = pos.pos [i].spline [TENSION], 
                 continuity = pos.pos [i].spline [CONTINUITY];

          seg.end        = pos.pos [i+1].frame;
          seg.locTime    = 1.0f/float(seg.end-seg.start);

          if (i)
          {
            float* cur  = (float*)&p [i][0];
            float* next = (float*)&p [i][1];
            float* prev = (float*)&p [i-1][0];

            for (int j=0;j<3;j++)
            {
              g1       = (cur [j] - prev [j]) * (1.0f + bias);
              g2       = (next [j] - cur [j]) * (1.0f - bias);
              g3       = g2 - g1;
              r [0][j] = (1.0f - tension) * (g1 + 0.5f * g3 * (1.0f + continuity));
            }

            if (i+1 != pos.main.num - 1)
            {
              prev = cur;
              cur  = next;
              next = (float*)&p [i+1][1];

              for (j=0;j<3;j++)
              {
                g1       = (cur [j] - prev [j]) * (1.0f + bias);
                g2       = (next [j] - cur [j]) * (1.0f - bias);
                g3       = g2 - g1;
                r [1][j] = (1.0f - tension) * (g1 + 0.5f * g3 * (1.0f - continuity));
              }                
            }  
            else
              vec [1] = *next - *cur;
          }
          else
          {
            float* cur  = (float*)&p [i][0];
            float* next = (float*)&p [i][1];
            float* prev = NULL;

            for (int j=0;j<3;j++)
              r [0][j] = next [j] - cur [j];                  

            if (i+1 != pos.main.num - 1)
            {
              prev = cur;
              cur  = next;
              next = (float*)&p [i+1][1];

              for (j=0;j<3;j++)
              {
                g1       = (cur [j] - prev [j]) * (1.0f + bias);
                g2       = (next [j] - cur [j]) * (1.0f - bias);
                g3       = g2 - g1;
                r [1][j] = (1.0f - tension) * (g1 + 0.5f * g3 * (1.0f - continuity));
              }                
            }  
            else
              vec [1] = *next - *cur;                  
          }    

          seg.a = p [i][0] * 2.0f  - p [i][1] * 2.0f + vec [0] + vec [1];
          seg.b = - p [i][0] * 3.0f  + p [i][1] * 3.0f - vec [0] * 2.0f - vec [1];
          seg.c = vec [0];
          seg.d = p [i][0];
        }
        else
        {
          seg.end        = pos.pos [i].frame;
          seg.locTime    = 1.0f;
          seg.a          = .0f;
          seg.b          = .0f;
          seg.c          = .0f;
          seg.d          = p [i][0];
        }            
      }  
    }

    pool->free (p);
    pool->free (pos.pos);

    return anim;
  }
  D_Exception (EXCEPTION_EXECUTE_HANDLER)
  {
    #ifdef DEBUG
      dout<<"Error at LoadVecAnim "<<endl;
    #endif
    return NULL;
  }
}

quatseg_t*  LoadQuatAnim   (BinFile* file,Pool* pool,int& count)
{
  D_Try
  {
    trackrot3ds_t  rot;    
    quat_t         q;
    quatseg_t*     anim = NULL;

    file->read (&rot.main,sizeof (rot.main));

    rot.rot    = (rotate3ds_t*)pool->alloc (sizeof (rotate3ds_t) * rot.main.num);
    anim       = rot.main.num ? 
                 (quatseg3ds_t*)pool->alloc (rot.main.num * sizeof (quatseg3ds_t)) :
                 NULL;
    count      = rot.main.num;

    for (int i=0;i<rot.main.num;i++)
    {
      uint   frame;
      uint16 flags;
      uint   mask = 1;

      file->read (&frame,sizeof (frame));
      file->read (&flags,sizeof (flags));

      rot.rot [i].frame = frame;
      rot.rot [i].flags = flags;

      for (int j=0;j<5;j++,mask<<=1)
        if (flags & mask) 
          file->read (&rot.rot [i].spline [j],sizeof (float));
        else              rot.rot [i].spline [j] = 0;

      file->read (&rot.rot [i].angle,sizeof (float)); 
      file->read (&rot.rot [i].ort,sizeof (vector3ds_t)); 

      vector3d_t ort (rot.rot [i].ort.x,rot.rot [i].ort.y,rot.rot [i].ort.z);

      if (i)
      {
        q                    = quat_t (ort,rot.rot [i].angle) * q; 
        anim [i].v1          = q;
        anim [i-1].v2        = q;
      }
      else
      {
        q                    = quat_t (ort,rot.rot [i].angle);
        anim [i].v1          = q;
      }              
    }

    anim [rot.main.num-1].v2 = q;

  //ЏаЁ Ё­вҐаЇ®«пжЁЁ ­  бҐЈ¬Ґ­вҐ Ї®« Ј Ґвбп c1 = v1.rb c2 = v2.ra

  //cur.ra = slerp(cur, g3, (tension-1));
  //cur.rb = slerp(cur, g3, -(tension-1));
  //q0.rb = slerp(p0, p1, (1-tension)*(1+continuity*bias)/3.0);
  //qn.ra = slerp(pn, p(n-1), (1-tension)*(1-continuity*bias)/3.0);


    if (rot.main.num)
    {
      for (int i=0;i<rot.main.num;i++)
      {
        quatseg3ds_t& seg = anim [i];

        seg.easeFrom     = rot.rot [i].spline [EASE_FROM];
        seg.easeTo       = rot.rot [i].spline [EASE_TO];
        seg.start        = rot.rot [i].frame;

        if (i != rot.main.num - 1)
        {
          quat_t g1,g2,g3,g4;
          float  bias       = rot.rot [i].spline [BIAS],
                 tension    = rot.rot [i].spline [TENSION], 
                 continuity = rot.rot [i].spline [CONTINUITY];

          seg.end        = rot.rot [i+1].frame;
          seg.locTime    = 1.0f/float(seg.end-seg.start);

          quat_t* cur  = &seg.v1;
          quat_t* prev = i ? &anim [i-1].v1 : NULL;
          quat_t* next = &seg.v2;

          if (i)
          {
            slerp (g1,*cur,*prev,-(1.0f+bias)/3.0f);
            slerp (g2,*cur,*next,(1.0f-bias)/3.0f);
            slerp (g3,g1,g2,0.5f + 0.5f * continuity);
            slerp (g4,g1,g2,0.5f - 0.5f * continuity);
            slerp (seg.c1,*cur,g3,-(tension-1.0f)); //seg.v1.rb
          }
          else
            slerp (seg.c1,*cur,*next,(1.0f-tension)*(1.0f+continuity*bias)/3.0f);

          if (i+1 != rot.main.num - 1)
          {
            prev = cur;
            cur  = next;
            next = &anim [i+1].v2;

            slerp (g1,*cur,*prev,-(1.0f+bias)/3.0f);
            slerp (g2,*cur,*next,(1.0f-bias)/3.0f);
            slerp (g3,g1,g2,0.5f + 0.5f * continuity);
            slerp (g4,g1,g2,0.5f - 0.5f * continuity);
            slerp (seg.c2,*cur,g3,(tension-1.0f)); //seg.v2.ra
          }
          else
            slerp(seg.c2,*next,*cur,(1.0f-tension)*(1.0f-continuity*bias)/3.0);        
        }
        else
        {
          seg.c1         = quat_t ();
          seg.c2         = quat_t ();
          seg.end        = rot.rot [i].frame;
          seg.locTime    = 1.0f;
        }            
      }  
    }

    pool->free (rot.rot);

    return anim;
  }
  D_Exception (EXCEPTION_EXECUTE_HANDLER)
  {
    #ifdef DEBUG
      dout<<"Error at LoadQuatAnim "<<endl;
    #endif
    return NULL;
  }
}

/////////////implementation of class Loader3DS//////////////////////////////////

void      Loader3DS::recalc (Mesh3DS* mesh)
{
  D_Try
  {
    count (Object3D::NODE,count (Object3D::NODE)+1);

    RingTmpl<Object3DS*>::iterator iter   = mesh->ring;
    int                            count  = mesh->ring.count ();

    for (int i=0;i<count;i++,iter++)
      recalc (iter.data()); 

    this->count (Object3D::MATERIAL,mesh->dict.count ());
    this->count (Object3D::LIGHT,mesh->light.count ());
    this->count (Object3D::ANIM_TRANS,this->count (Object3D::ANIM_TRANS)+1);
    this->count (Object3D::ANIM_SCALE,this->count (Object3D::ANIM_SCALE)+1);
    this->count (Object3D::ANIM_ROTATE,this->count (Object3D::ANIM_ROTATE)+1);
    this->count (Object3D::ANIM_SEG,this->count (Object3D::ANIM_SEG)+1);
  }
  D_Exception (EXCEPTION_EXECUTE_HANDLER)
  {
    #ifdef DEBUG
      dout<<"Loader3DS: Error at recalc () "<<this<<endl;
    #endif
  }  
}

void      Loader3DS::recalc (Object3DS* obj)
{
  if (obj == NULL)
    return;

  count (Object3D::NODE,count (Object3D::NODE)+1);

  Trimesh3DS*      block = obj->block;
  Anim3DS*         anim  = obj->anim;
  
  if (block)
  {
    count (Object3D::GROUP,count (Object3D::GROUP)+1);

    if (block->vertex) 
    {
       count (Object3D::VERTEX,count (Object3D::VERTEX)+block->vertex->count);  
       count (Object3D::NORMAL,count (Object3D::NORMAL)+block->vertex->count);  
    }   
    if (block->facet)  
       count (Object3D::FACET,count (Object3D::FACET)+block->facet->count);   
  }  

  if (anim)
  {
    count (Object3D::ANIM_SEG,count (Object3D::ANIM_SEG)+1);
    
    if (anim && anim->transCount)
      count (Object3D::ANIM_TRANS,count (Object3D::ANIM_TRANS)+anim->transCount);
    if (anim && anim->scaleCount)
      count (Object3D::ANIM_SCALE,count (Object3D::ANIM_SCALE)+anim->scaleCount);
    if (anim && anim->rotCount)
      count (Object3D::ANIM_ROTATE,count (Object3D::ANIM_ROTATE)+anim->rotCount);
  }
}

void    Loader3DS::convert (Mesh3DS* mesh)
{                
  D_Try
  {
    int pos [Object3D::ELEMENTS_COUNT];    
    memset (pos,0,sizeof (int) * Object3D::ELEMENTS_COUNT);    

           //Конвертация узлов

    RingTmpl<Object3DS*>::iterator  iter     = mesh->ring;
    int                             count    = mesh->ring.count ();
    Object3DS**                     objTable = (Object3DS**)GetPool ()->alloc (sizeof (Object3DS*) * count);
    int                             ret      = -1;
    int                             max      = -1;
    Object3D*                       obj      = NULL;

    for (int i=0;i<count;i++,iter++)
    {
      ret                = convert (iter.data (),pos,mesh);
      if (ret!=-1)
        objTable [ret]   = iter.data ();

      if (ret > max) max = ret;
    }  
          //Конвертация материалов

    count            = mesh->dict.count ();
    textmat_t*  mMat = (textmat_t*)get (Object3D::MATERIAL);
 
    for (i=0;i<count;i++)
      mMat [i] = *mesh->dict.search (i).data ();

         //Конвертация источников света
    
    RingTmpl<DirectLight3DS*>::iterator  li = mesh->light;
    count                                   = mesh->light.count ();
    light_t*              mLight            = (light_t*)get (Object3D::LIGHT);    

    for (i=0;i<count;li++,i++)    
    {
      light_t&          light = mLight [i];
      DirectLight3DS&   l     = *li.data ();

      light.pos      = vector3d_t (l.pos.x,l.pos.y,l.pos.z);
      light.dir      = normalize  (
                       vector3d_t (l.target.x-l.pos.x,
                                   l.target.y-l.pos.y,
                                   l.target.z-l.pos.z)
                                  );
      light.ambient  = light.diffuse = light.specular = l.color;
      light.shiness  = l.exp;
      light.angle    = l.angle;
    }

         //Реиндексация

    objnode_t* nodes  = (objnode_t*)get (Object3D::NODE);
    objnode_t& root   = nodes [max+1];
    int        parent = -1;
    
    root.parent       = -1;
    root.node         = -1;
    root.next         = -1;
    root.group        = -1;
    root.anim.node    = -1; 
    root.anim.flags   = 0;

    for (i=0;i<=max;i++)
    {
//      dout<<"in node: "<<i<<" in 3ds: "<<mConv [i]<<endl;

      if (nodes [i].parent != -1)
      {
        nodes [i].parent = parent = mConv [nodes [i].parent];
        insert (parent,i);
      }
      else           
      {             
        nodes [i].parent = max + 1;
        insert (max+1,i);
      }    

      convert (objTable [i],i,pos);
    }

    convert (NULL,max+1,pos);

    main            (max+1);
    mObject->_bound ();

    GetPool ()->free    (objTable);
  }
  D_Exception (EXCEPTION_EXECUTE_HANDLER)
  {
    #ifdef DEBUG
      dout<<"Loader3DS: Error at convert (Mesh3DS*) "<<this<<endl;
    #endif
  }
}

int    Loader3DS::convert (Object3DS* obj,int* pos,Mesh3DS* mesh)
{
  D_Try
  {
    if (obj == NULL)
      return -1;

    group3d_t*  mGroup   = (group3d_t*)get (Object3D::GROUP);  
    objnode_t*  mObjNode = (objnode_t*)get (Object3D::NODE);
    vertex_t*   mVertex  = (vertex_t*)get  (Object3D::VERTEX);
    facet_t*    mFacet   = (facet_t*)get   (Object3D::FACET);
    vector3d_t* mNorm    = (vector3d_t*)get(Object3D::NORMAL);

    if (obj->node < 0)
    {
      #ifdef DEBUG
        dout<<"Loader3DS: Error node: "<<obj->node<<endl;
      #endif
      return -1;
    }

    int        index     = pos [Object3D::NODE]++;//obj->node;

    objnode_t& node      = mObjNode [index];
    mConv [obj->node]    = index;

    node.parent          = obj->parent;
    node.next            = -1;
    node.node            = -1;
    node.group           = -1;

    if (obj->block && obj->block->vertex && obj->block->facet)
    {
      node.group          = pos [Object3D::GROUP];

      group3d_t& group    = mGroup [pos [Object3D::GROUP]];
      group.vertex.count  = obj->block->vertex->count;
      group.vertex.first  = pos [Object3D::VERTEX];
      group.facet.count   = obj->block->facet->count;
      group.facet.first   = pos [Object3D::FACET];
      group.normal.count  = obj->block->vertex->count;
      group.normal.first  = pos [Object3D::NORMAL];
    
      int          vCount = obj->block->vertex->count;
      vertex_t*    vertex = &mVertex [pos [Object3D::VERTEX]];
      vector3d_t*  norm   = &mNorm   [pos [Object3D::NORMAL]];
      vertex3ds_t* src    = obj->block->vertex->data; 
      matrix_t*    trans  = obj->block->trans;

      memset (norm,0,sizeof (vector3d_t) * vCount);

      for (int i=0;i<vCount;i++,src++,vertex++)
      {        
        vertex->x = src->x;
        vertex->y = src->y;
        vertex->z = src->z;
      }
      
      int           fCount = obj->block->facet->count;
      int           vFirst = pos [Object3D::VERTEX];
      int           fFirst = pos [Object3D::FACET];
      facet3ds_t*   f3ds   = obj->block->facet->data;
      facet_t*      facet  = &mFacet [fFirst];
      vertex               = &mVertex [pos [Object3D::VERTEX]];

      if (obj->block->text)
      { 
        textcoord_t*  text   = obj->block->text->data;
      
        for (int j=0;j<fCount;j++,f3ds++,facet++)
        {
          facet->index  [0] = f3ds->v [0];
          facet->index  [1] = f3ds->v [1];
          facet->index  [2] = f3ds->v [2];
  
          facet->tindex [0] = text [f3ds->v [0]];
          facet->tindex [1] = text [f3ds->v [1]];
          facet->tindex [2] = text [f3ds->v [2]];

          facet->n         = (vertex [facet->index [0]] - vertex [facet->index [2]]) ^
                             (vertex [facet->index [2]] - vertex [facet->index [1]]);
          facet->n.normalize ();                                                          
          facet->texture   = -1; 

          norm [facet->index [0]] += facet->n;
          norm [facet->index [1]] += facet->n;
          norm [facet->index [2]] += facet->n;
        }
      }
      else
      {
        for (int j=0;j<fCount;j++,f3ds++,facet++)
        {
          facet->index  [0] = f3ds->v [0];
          facet->index  [1] = f3ds->v [1];
          facet->index  [2] = f3ds->v [2];
  
          facet->n         = (vertex [facet->index [0]] - vertex [facet->index [2]]) ^
                             (vertex [facet->index [2]] - vertex [facet->index [1]]);
          facet->n.normalize ();
          facet->texture   = -1; 

          norm [facet->index [0]] += facet->n;
          norm [facet->index [1]] += facet->n;
          norm [facet->index [2]] += facet->n;
        }
      }

      vector3d_t vSum(0.0, 0.0, 0.0);
      vector3d_t vZero = vSum;
      int shared=0;

      for (i=0;i<vCount;i++)
        norm [i].normalize ();

      if (obj->block->mat.count ())
      {
        int                                matCount = obj->block->mat.count ();
        RingTmpl<matlist3ds_t*>::iterator  iter     = obj->block->mat;
        
        for (int i=0;i<matCount;i++,iter++)
        {
          const matlist3ds_t& list   = *iter.data ();          
          int                 mat    = mesh->idict.search (CRC32 (list.name,strlen (list.name)));
          facet                      = &mFacet [list.data [0] + fFirst];
  
          for (int j=0;j<list.count;j++,facet++)
            facet->texture = mat;
        }
      }

      pos [Object3D::VERTEX]  += obj->block->vertex->count;
      pos [Object3D::NORMAL]  += obj->block->vertex->count;
      pos [Object3D::FACET]   += fCount;      
      pos [Object3D::GROUP]++;   
    }

    return index;
  }
  D_Exception (EXCEPTION_EXECUTE_HANDLER)
  {
    #ifdef DEBUG
      dout<<"Loader3DS: Error at convert () "<<this<<endl;
    #endif
    return -1;
  }
}

void      Loader3DS::insert      (int parent,int child)
{
  D_Try
  {
    objnode_t*    mNode   = (objnode_t*)get (Object3D::NODE);
    objnode_t&    mParent = mNode [parent];
    objnode_t&    mChild  = mNode [child];

    if (mParent.node != -1)
    {
      for (int node = mParent.node;mNode [node].next != -1;node = mNode [node].next);
        
      mNode [node].next = child;
      mChild.parent     = parent;
    }
    else
    {
      mParent.node  = child;
      mChild.parent = parent;
    }
  }
  D_Exception (EXCEPTION_EXECUTE_HANDLER)
  {
    #ifdef DEBUG        
      dout<<"Loader3DS: Error at insert node: "<<parent<<" child: "<<child
          <<" "<<this<<endl;
    #endif
  }
}

void      Loader3DS::convert     (Object3DS* obj,int inode,int* pos)
{
  D_Try
  {
    objnode_t& node  = ((objnode_t*)get (Object3D::NODE))[inode];
    animseg_t& seg   = ((animseg_t*)get (Object3D::ANIM_SEG))[pos [Object3D::ANIM_SEG]++];

    vecseg_t*  trans = ((vecseg_t*)get (Object3D::ANIM_TRANS)) + pos [Object3D::ANIM_TRANS];
    vecseg_t*  scale = ((vecseg_t*)get (Object3D::ANIM_SCALE)) + pos [Object3D::ANIM_SCALE];
    quatseg_t* rot   = ((quatseg_t*)get (Object3D::ANIM_ROTATE)) + pos [Object3D::ANIM_ROTATE];

    node.anim.node   = pos [Object3D::ANIM_SEG] - 1;
    node.anim.flags  = 1|2|4;  //не корректно

    if (obj)
    {
      seg.pivot        = obj->anim->pivot;

      if (obj->block) seg.start        = *obj->block->trans * matrix_t::translate (obj->block->offs);
      else            seg.start        = matrix_t (1.0f);

      seg.offs         = obj->block ? obj->block->offs : vector3d_t (.0f);
      seg.rot.first    = pos [Object3D::ANIM_ROTATE];
      seg.trans.first  = pos [Object3D::ANIM_TRANS];
      seg.scale.first  = pos [Object3D::ANIM_SCALE];

      seg.rot.count    = obj->anim->rotCount;
      seg.trans.count  = obj->anim->transCount;
      seg.scale.count  = obj->anim->scaleCount;

      pos [Object3D::ANIM_ROTATE] += obj->anim->rotCount;
      pos [Object3D::ANIM_TRANS ] += obj->anim->transCount;
      pos [Object3D::ANIM_SCALE ] += obj->anim->scaleCount;    

      memcpy (trans,obj->anim->trans,sizeof (vecseg_t) * obj->anim->transCount);
      memcpy (scale,obj->anim->scale,sizeof (vecseg_t) * obj->anim->scaleCount);
      memcpy (rot,obj->anim->rot,sizeof (quatseg_t) * obj->anim->rotCount);

      if (obj->anim->scaleCount > 1)
      {
        vector3d_t v = obj->anim->scale->d;    
        v            = vector3d_t (1.0f/v.x,1.0f/v.y,1.0f/v.z);

        seg.start    = *obj->block->trans * matrix_t::scale (v) * matrix_t::translate (obj->block->offs);
      }
    }
    else
    {
      seg.pivot        = vector3d_t (.0f);

      seg.start        = matrix_t::scale (vector3d_t (-1,1,1));

      seg.offs         = vector3d_t (.0f);
      seg.rot.first    = pos [Object3D::ANIM_ROTATE];
      seg.trans.first  = pos [Object3D::ANIM_TRANS];
      seg.scale.first  = pos [Object3D::ANIM_SCALE];

      seg.rot.count    = 1;
      seg.trans.count  = 1;
      seg.scale.count  = 1;

      pos [Object3D::ANIM_ROTATE]++;
      pos [Object3D::ANIM_TRANS ]++;
      pos [Object3D::ANIM_SCALE ]++;

      trans->a = trans->b = trans->c = trans->d = vector3d_t (.0f);
      trans->start = trans->end = 0;
      trans->locTime = 1.0f;
      trans->easeFrom = trans->easeTo = 0;

      scale->a = scale->b = scale->c = vector3d_t (.0f);
      scale->d = vector3d_t (-1,1,1);
      scale->start = scale->end = 0;
      scale->locTime = 1.0f;
      scale->easeFrom = scale->easeTo = 0;

      rot->c1 = rot->v1 = rot->c2 = rot->v2 = quat_t (vector3d_t (.0f),0);
      rot->start = rot->end = 0;
      rot->locTime = 1.0f;
      rot->easeFrom = rot->easeTo = 0;
    }
  }
  D_Exception (EXCEPTION_EXECUTE_HANDLER)
  {           
    #ifdef DEBUG
      dout<<"Loader3DS: Error at convert (Anim3D*) node: "<<inode<<endl;
    #endif
  }
}

Loader3DS::Loader3DS   (Object3D* _Object3D,BinFile* _File,Pool* _Pool)
          : X3FLoader (_Object3D,_File,_Pool)
{
//  D_Try
//  {
    memset (mConv,-1,sizeof (mConv));

    chunk_t     chunk;
    Mesh3DS*    mesh;
    
    for (;!mFile->eof();)
    {
//      dout<<mFile->getpos ()<<" "<<mFile->size ()<<" loader "<<endl;   
      mFile->read (&chunk,sizeof (chunk));

      switch (chunk.id)
      {
        case CHUNK_MAIN:
          break;
        case CHUNK_OBJMESH:  
          mesh = new (GetPool ()) Mesh3DS (GetPool (),mFile,chunk.len);
          break;
        case CHUNK_KF_DATA:
          KFData3DS (GetPool (),mFile,chunk.len,mesh);
          break;  
        default:
          mFile->seek (mFile->getpos()+chunk.len-sizeof (chunk_t));          
          break;
      }
    }  

    if (mesh)
    {
      recalc  (mesh);
      alloc   ();
      convert (mesh);

      delete mesh;
    }    
//  }
//  D_Exception (EXCEPTION_EXECUTE_HANDLER)
//  {
//    #ifdef DEBUG
  //    dout<<"Loader3DS: Error at construct "<<this<<endl;
//    #endif    
//  }
}

Loader3DS::~Loader3DS  ()
{
}