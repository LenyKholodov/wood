           typedef  vector3d_t&  (*ComputeFn)(const LightEnvironment&,const vector3d_t&);
           typedef  vector3d_t&  (*ComputeMFn)(const LightEnvironment&,const vector3d_t&,const material_t&);

    static float        computeD         (const LightEnvironment&,const vector3d_t&);
    static float        computeD         (const LightEnvironment&,const vector3d_t&,const material_t&);
    static float        computeA         (const LightEnvironment&,const vector3d_t&);
    static float        computeA         (const LightEnvironment&,const vector3d_t&,const material_t&);
    static float        computeDA        (const LightEnvironment&,const vector3d_t&);
    static float        computeDA        (const LightEnvironment&,const vector3d_t&,const material_t&);
    static float        computeDAS       (const LightEnvironment&,const vector3d_t&);
    static float        computeDAS       (const LightEnvironment&,const vector3d_t&,const material_t&);

  D_Try
  {    
    vector3d_t null (0);
    int        count = 0;

    for (int j=0;j<num;j++)
      color [j] = null;

    switch (mIntParam [COMPUTE_METHOD])
    {
      default:
      case METHOD_DIFFUSE:
      {
        for (int l=SOFTWARE;l<mCount;l++)
          if (mEnable [l])
          {
            light_t&    light = mLightTable [l];
            float       _cos  = cos (light.angle);
            vector3d_t  col   = light.diffuse*light.shiness*2.0f/(1.0f-_cos+K);

            for (int i=0;i<num;i++)
            {
              const vector3d_t&  n     = norm   [i];
              const vector3d_t&  v     = vertex [i];
              float              ang   = ((-light.dir) & n);

              if (ang >= _cos)
              {        
                vector3d_t lDir  = light.pos-v;
                float      len   = lDir.length ();
                ang              = ((lDir/len-light.dir)/2.0f) & n;
                float      coef  = ang/(len+K);
                color [i]       += col*coef;
                count++;
              }              
            }                            
          }
        break;
      }
    }  

    if (count)
    {
      float cnt = count;
      for (j = 0;j<num;j++)
        color [j] /= cnt;
    }
  }
  D_Exception (EXCEPTION_EXECUTE_HANDLER)  
  {
  }

object_t* createObject (Pool* pool)
{
  object_t* obj = (object_t*)pool->alloc (sizeof (object_t));

  if (obj == NULL)
  {
    #ifdef DEBUG
      dout<<"Object3D: No enough memory "<<endl;
    #endif
    D_Error (EXCEPTION_NO_MEMORY);
  }

  obj->parent   = NULL;
//  obj->block    = (block_t*)pool->alloc (sizeof (block_t)); //?????
  obj->block    = NULL;

//  if (obj->ring == NULL || obj->mat == NULL || obj->block == NULL)
//  {
//    #ifdef DEBUG
//      dout<<"Object3D: No enough memory "<<endl;
//    #endif
//    D_Error (EXCEPTION_NO_MEMORY);    
//  }  

  return obj;
}

block_t* createBlock (Pool* pool)
{
  block_t* block = (block_t*)pool->alloc (sizeof (block_t));

  if (block == NULL)
  {
    #ifdef DEBUG
      dout<<"Object3D: No enough memory "<<endl;
    #endif
    D_Error (EXCEPTION_NO_MEMORY);
  }

  block->vertex = NULL;
  block->facet  = NULL;
  block->text   = NULL;

  return block;
} 

        case CHUNK_OBJBLOCK: 
          char objectName [128];
          loadASCIIZ (file,objectName);
          dout<<"Load object: '"<<objectName<<"'"<<endl;

          if (obj)
          {
            obj->ring.insert (x = createObject (pool()) );
            x->parent = obj;
            obj       = x;  
          }
          else
            root = obj = createObject ( pool () );  

          mCount [NODE]++; 

          dout<<"het!"<<endl;

          break;
        case CHUNK_MATERIAL:        
          LoadMaterial (file,chunk.len,matDict,matIDict,matPos++);
          mCount [MATERIAL]++;
          break;
        case CHUNK_TRIMESH:
          LoadTrimesh  (file,chunk.len,obj->block = createBlock ( pool() ) );
          mCount [GROUP]++;
          break;

  CHUNK_KEYFRAMER    = 0xB000, 
  CHUNK_TRACKINFO    = 0xB002, 
  CHUNK_TRACKOBJNAME = 0xB010, 
  CHUNK_TRACKPIVOT   = 0xB013, 
  CHUNK_TRACKPOS     = 0xB020, 
  CHUNK_TRACKROTATE  = 0xB021, 

  CHUNK_TRACKCAMERA  = 0xB003, 
  CHUNK_TRACKFOV     = 0xB023, 
  CHUNK_TRACKROLL    = 0xB024, 
  CHUNK_TRACKCAMTGT  = 0xB004, 

////////////////////////////////////////////////////////////////////////////////
///Всё, что относится к загрузке 3DS
////////////////////////////////////////////////////////////////////////////////                
        BOOL            LoadMaterial    (BinFile*,size_t,MatDict*,MatIndexDict*,int); 
        BOOL            LoadTrimesh     (BinFile*,size_t,block_t*);

        int             Convert         (object_t*,
                                         int*    pos,
                                         int     owner,
                                         int     prev,
                                         MatIndexDict*
                                        );
        void            Convert         (MatDict*,int count);

BOOL            Object3D::Load3DS (BinFile* file)
{
  D_Try
  {
/*    chunk_t       chunk;    
    Mesh3DS*      mesh;

    for (;!file->eof ();)
    {
      file->read (&chunk,sizeof (chunk));

      dout<<"POS: "<<file->getpos()-sizeof (chunk_t)<<" ID: "<<hex
          <<chunk.id<<dec<<" LEN: "<<chunk.len<<endl;

      switch (chunk.id)
      {
        case CHUNK_MAIN:
          mCount [NODE]     = 0;
          mCount [GROUP]    = 0;
          mCount [VERTEX]   = 0;
          mCount [FACET]    = 0;
          mCount [MATERIAL] = 0;
          mCount [LIGHT]    = 0;

          break;
        case CHUNK_OBJMESH:
          mesh = new (pool() mesh_t (pool(),file,chunk.len);
          break;
        default:
          #ifdef DEBUG
            dout<<"Object3D: No chunk handler with id: "<<hex<<chunk.id<<dec<<endl;
          #endif
          file->seek (file->getpos()+chunk.len-sizeof (chunk_t));
          break;
      }    
    }

    mMain  = 0;    
    mSpace = (char*)pool()->alloc (mCount [NODE]     * sizeof (objnode_t) +
                                   mCount [GROUP]    * sizeof (group3d_t) +
                                   mCount [MATERIAL] * sizeof (textmat_t) +
                                   mCount [VERTEX]   * sizeof (vertex_t)  +
                                   mCount [FACET]    * sizeof (facet_t)   +
                                   mCount [LIGHT]    * sizeof (light_t)
                                  );   

    if (mSpace == NULL)
    {
      #ifdef DEBUG
        dout<<"Object3D: No enough memory at load3ds "<<this<<endl;
      #endif
      D_Error (EXCEPTION_NO_MEMORY);
    }

    mObjNode = (objnode_t*)mSpace;
    mGroup   = (group3d_t*)((char*)mObjNode + mCount [NODE] * sizeof (objnode_t));
    mVertex  = (vertex_t*)((char*)mGroup + mCount [GROUP] * sizeof (group3d_t));
    mFacet   = (facet_t*)((char*)mVertex + mCount [VERTEX] * sizeof (vertex_t));
    mMat     = (textmat_t*)((char*)mFacet + mCount [FACET] * sizeof (facet_t)); 
            
    int   pos [ELEMENTS_COUNT]; 
    memset (pos,0,sizeof (int)*ELEMENTS_COUNT);

    Convert (root,pos,-1,-1,matIDict);
    Convert (matDict,matPos);

    return TRUE;*/
  }
  D_Exception (EXCEPTION_EXECUTE_HANDLER)
  {
    #ifdef DEBUG
      dout<<"Object3D: Error at Load3DS () "<<this<<endl;
    #endif
    return FALSE;
  }
}

int             Object3D::Convert (object_t* obj,int* pos,int parent,int prev,MatIndexDict* dict)
{
  if (obj == NULL)
    return -1;

  if (prev != -1)  
    mObjNode [prev].next = pos [NODE];

  objnode_t& node   = mObjNode [pos [NODE]];
  int        index  = pos [NODE]++;

//  node.bound -???
  node.parent       = parent;
  node.next         = -1;    

  if (obj->block && obj->block->vertex && obj->block->facet)
  {
    node.group          = pos [GROUP];

    group3d_t& group    = mGroup [pos [GROUP]];
    group.vertex.count  = obj->block->vertex->count;
    group.vertex.first  = pos [VERTEX];
    group.facet.count   = obj->block->facet->count;
    group.facet.first   = pos [FACET];
    
    int          vCount = obj->block->vertex->count;
    vertex_t*    vertex = &mVertex [pos [VERTEX]];
    vertex3ds_t* src    = obj->block->vertex->data;

      //Копирование с перестановкой порядка                     

    for (int i=0;i<vCount;i++,src++,vertex++)
    {
      vertex->x = src->x;
      vertex->y = src->y;
      vertex->z = src->z;
    }

    int           fCount = obj->block->facet->count;
    int           vFirst = pos [VERTEX];
    int           fFirst = pos [FACET];
    facet3ds_t*   f3ds   = obj->block->facet->data;
    facet_t*      facet  = &mFacet [fFirst];

    if (obj->block->text)
    { 
      textcoord_t*  text   = obj->block->text->data;
      
      for (int j=0;j<fCount;j++,f3ds++,facet++)
      {
        facet->index  [0] = f3ds->v [0] + vFirst;
        facet->index  [1] = f3ds->v [1] + vFirst;
        facet->index  [2] = f3ds->v [2] + vFirst;

        facet->tindex [0] = text [f3ds->v [0]];
        facet->tindex [1] = text [f3ds->v [1]];
        facet->tindex [2] = text [f3ds->v [2]];

        facet->n         = (mVertex [facet->index [1]] - mVertex [facet->index [0]]) ^
                           (mVertex [facet->index [2]] - mVertex [facet->index [0]]);                                              
        facet->texture   = -1; ///ATTENTION!!!!! NO TEXTURE!!!!!!!                                                
      }
    }
    else
    {
      for (int j=0;j<fCount;j++,f3ds++,facet++)
      {
        facet->index  [0] = f3ds->v [0] + vFirst;
        facet->index  [1] = f3ds->v [1] + vFirst;
        facet->index  [2] = f3ds->v [2] + vFirst;

        facet->n         = (mVertex [facet->index [1]] - mVertex [facet->index [0]]) ^
                           (mVertex [facet->index [2]] - mVertex [facet->index [0]]);                                              
        facet->texture   = -1; ///ATTENTION!!!!! NO TEXTURE!!!!!!!                                                
      }

      int                               matCount = obj->block->mat.count ();
      RingTmpl<matlist_t>::iterator     iter     = obj->block->mat;
      
      for (i=0;i<matCount;i++,iter++)
      {
        const matlist_t& list   = iter.data ();
        int              mat    = dict->search (CRC32 (list.name,strlen (list.name)));
        facet                   = &mFacet [list.data [j] + fFirst];

        for (int j=0;j<list.count;j++,facet++)
          facet->texture = mat;
      }
    }

    pos [VERTEX]  += obj->block->vertex->count;
    pos [FACET]   += fCount;      
    pos [GROUP]++;
    
//    pool()->free (obj->block->vertex);
//    pool()->free (obj->block->facet);
//    pool()->free (obj->block->text);    

//    pool()->free (obj->block);
  }

  int                           count = obj->ring.count ();
  RingTmpl<object_t*>::iterator iter  = obj->ring;
  int                           _prev = -1;

  for (int i=0;i<count;i++,iter++)
  {
    _prev = Convert (iter.data (),pos,index,_prev,dict);

    if (i == 0) node.node = _prev;
  }  

  dout<<"NODE: "<<index<<endl;  
  dout<<"Facet count: "<<mGroup [mObjNode [index].group].facet.count<<endl;

//  delete obj->mat;

  return index;
}

void            Object3D::Convert  (MatDict* dict,int count)
{
  if (dict)     
    for (int i=0;i<count;i++)
      mMat [i] = dict->search (i);
}

BOOL            Object3D::LoadTrimesh (BinFile* file,size_t size,block_t* block)
{
  chunk_t       chunk;
  long          pos  = file->getpos () - sizeof (chunk_t);
  
  for (;!file->eof() && file->getpos() - pos < size;)
  {
    file->read (&chunk,sizeof (chunk));

    switch (chunk.id)
    {
        case CHUNK_VERTLIST:
          block->vertex    = createVertexList (file,pool());
          mCount [VERTEX] += block->vertex->count;
          break;
        case CHUNK_FACELIST:
          block->facet    = createFacetList (file,pool());
          mCount [FACET] += block->facet->count;
          break;
        case CHUNK_FACEMAT:
          block->mat.insert (*createMatList (file,pool())); //Delete cretaeMatList!!!!!!
          break;
        case CHUNK_MAPLIST:
          block->text = createTextList (file,pool());
          break;
//        case CHUNK_TRMATRIX:
//          break;
        default:
          file->seek (file->getpos()+chunk.len-sizeof (chunk_t));
    }
  }  

  return TRUE;
}

BOOL            Object3D::LoadMaterial  (BinFile* file,size_t size,MatDict* dict,MatIndexDict* idict,int index)
{
  dout<<"in"<<endl;       
  chunk_t       chunk;
  textmat_t     mat;
  char          name [128];
  long          pos = file->getpos () - sizeof (chunk_t);

  for (;!file->eof () && file->getpos () - pos < size;)
  {
    file->read (&chunk,sizeof (chunk));

    switch (chunk.id)
    {      
        case CHUNK_MATNAME:
          loadASCIIZ   (file,name);
          break;
        case CHUNK_TEXTURE:
          break;
        case CHUNK_MAPFILE:
          loadASCIIZ (file,mat.text);
          break;
        default:
          file->seek (file->getpos()+chunk.len-sizeof (chunk_t));
    }    
  }

  idict->insert (CRC32(name,strlen (name)),index);
  dict->insert  (index,mat);

  dout<<"out"<<endl;

  return TRUE;
}

#include <graph\bmp.h>
#include <misc.h>

DWORD    BMP_SIGN  = 0x4D42;

#pragma pack(1)

struct   palrgb_t
{
  uchar  blue;
  uchar  green;
  uchar  red;
  uchar  res;
};

struct  rgb16_t
{
  uint16        blue  : 5;
  uint16        green : 6;
  uint16        red   : 5;   
};

struct  rgb15_t
{
  uint16        blue  : 5;
  uint16        green : 5;
  uint16        red   : 5;   
};

void    LoadBMP24bit (BinFile* file,void* buf,WinBMPHeader& header)
{
  long  width        = header.Width;
  long  height       = header.Height;
  long  Appendix     = (4-width*3) & 3;  
  long  sline        = width * 3;
  char* data         = (char*)buf + sline*height;
  char  temp [4];
              
  while (height-- > 0)
  {
    file->read (data -= sline,sline);
    file->read (temp,Appendix);   
  }  
}

void    LoadBMP32bit (BinFile* file,void* buf,WinBMPHeader& header)
{
  long  width        = header.Width;
  long  height       = header.Height;
  long  sline        = width * 4;
              
  file->read (buf,height*sline);
}

void    LoadBMP16bit (BinFile* file,void* buf,WinBMPHeader& header)
{
  long  width        = header.Width;
  long  height       = header.Height;
  long  Appendix     = (width&1) ? 2 : 0;
  long  sline        = width*2;
  char* data         = (char*)buf + sline*height;
  char  temp [4];  

  while (height-- > 0)
  {
    file->read (data -= sline,sline);
    file->read (temp,Appendix);   
  }               
}

void    LoadBMP8bit (BinFile* file,void* buf,palrgb_t* pal,WinBMPHeader& header,long _BMPOffs)
{
  long  width        = header.Width;
  long  height       = header.Height;
  long  Appendix     = (4-width) & 3;  
  long  sline        = width;
  char* data         = (char*)buf + sline*height;
  char  temp [4];

  file->seek (sizeof (WinBMPFileHeader)+header.Size);

  file->read (pal,_BMPOffs-file->getpos());
  file->seek (_BMPOffs);
              
  while (height-- > 0)
  {
    file->read (data -= sline,sline);
    file->read (temp,Appendix);   
  }               
}

void    LoadBMP8bitComp (BinFile* file,void* buf,palrgb_t* pal,WinBMPHeader& header,long _BMPOffs)
{
  long  width        = header.Width;
  long  height       = header.Height;
  long  Appendix     = (4-width) & 3;  
  long  sline        = width;
  char* data         = (char*)buf;
  int   lump         = 0 ;
  char  temp [8192];
  
  file->seek (sizeof (WinBMPFileHeader)+header.Size);

  file->read (pal,_BMPOffs-file->getpos());
  file->seek (_BMPOffs);

  for (int size=header.Width*header.Height;size;)
  {   
    uchar code;

    file->read (&code,1);

    if (code)
    {
      file->read (data,code);

      data += code;
      size -= code;
    }  
    else
    {
      file->read (&code,1);

      switch (code)
      {
        case 0:
          memset (data,0,lump = long(data-(char*)buf) % header.Width);
          data += lump;
          size -= lump;
          break;
        case 1:
          memset (data,0,size);          
          data += size;
          file->read (data,size);  
          size  =  0;
          break;  
        case 2:
        {
          uchar pos [2];
          file->read (pos,2);
          memset (data,0,lump = long(pos[1])*width+pos [0]);
          data += lump;
          size -= lump;
          break;  
        }  
        default:
          file->read (data,code);
          file->read (temp,code&1);
          data += code;
          size -= code;
          break;
      }
    }    
  }  

  uchar* up   = (uchar*)buf;
  uchar* down = (uchar*)buf + width*height - width;

  for (int i=0;i<height>>1;i++,up+=width,down-=width)
  {
    memcpy (temp,down,width);               
    memcpy (down,up,width);
    memcpy (up,temp,width);
  }
}

void            ConvertFrom32bit (void* buf,bitmap_t* bmp)
{
   //???
//  rgba_t* src   = (rgba_t*)buf;
//  rgb_t*  dest  = (rgb_t*)bmp->data;
//  int     size  = bmp->width*bmp->height;

//  for (int i=0;i<size;i++,src++,dest++)
//  {
//    dest->red   = src->blue;
//    dest->green = src->green;
//    dest->blue  = src->red;
//  }
}

void            ConvertFrom24bit (void* buf,bitmap_t* bmp)
{
  rgba_t* dest  = (rgba_t*)bmp->data;
  rgb_t*  src   = (rgb_t*)buf;
  int     size  = bmp->width*bmp->height;

  for (int i=0;i<size;i++,src++,dest++)
  {
    dest->red   = src->blue;
    dest->green = src->green;
    dest->blue  = src->red;
    dest->alpha = 255;
  }
}

void            ConvertFrom16bit  (void* buf,bitmap_t* bmp)
{
  rgb16_t*  src  = (rgb16_t*)buf;
  rgba_t*   dest = (rgba_t*)bmp->data;
  int       size = bmp->width * bmp->height;

  for (int i=0;i<size;i++,src++,dest++)
  {
    dest->red   = src->red;       
    dest->green = src->green;
    dest->blue  = src->blue;
    dest->alpha = 255;
  } 
}

void            ConvertFrom15bit  (void* buf,bitmap_t* bmp)
{
  rgb15_t* src  = (rgb15_t*)buf;
  rgba_t*  dest = (rgba_t*)bmp->data;
  int      size = bmp->width * bmp->height;

  for (int i=0;i<size;i++,src++,dest++)
  {
    dest->red   = src->red;       
    dest->green = src->green;
    dest->blue  = src->blue;
    dest->alpha = 255;
  } 
}

void            ConvertFrom8bit (void* buf,palrgb_t* pal,bitmap_t* bmp)
{
  uchar* src   = (uchar*)buf;
  rgba_t* dest = (rgba_t*)bmp->data;
  int    size  = bmp->width * bmp->height;
  uchar  index;

  for (int i=0;i<size;i++,src++,dest++)
  {
    index       = *src;

    dest->red   = pal [index].red;
    dest->green = pal [index].green;
    dest->blue  = pal [index].blue;
    dest->alpha = 255;
  }  
}

bitmap_t*       LoadBMP  (BinFile* file,Pool* pool,ostream& os)
{
  bitmap_t* bmp = NULL;
  void*     buf = NULL;
 
  D_Try
  {
    WinBMPFileHeader      fileHeader;
    WinBMPHeader          header;

    file->read (&fileHeader,sizeof (fileHeader));
 
    if (fileHeader.FileType != BMP_SIGN)
    {
      os<<"LoadBMP: Wrong file signature "<<endl;
      return NULL;
    }

    file->read (&header,sizeof (header));  

    bmp = (bitmap_t*)pool->alloc (sizeof (bitmap_t) + 
           header.Width*header.Height*3);
 
    if (bmp == NULL)
    {
      os<<"LoadBMP: Error at alloc bitmap Width: "<<header.Width
        <<" Height: "<<header.Height<<" BPP: "<<header.BPP<<endl;
      return NULL;  
    }

    bmp->width    = header.Width;
    bmp->height   = header.Height;
    bmp->bpp      = header.BPP;

    palrgb_t pal  [256];
    BOOL     Error = 0;

    switch (header.BPP)
    {
      case 32: 
        buf = bmp->data;
        LoadBMP32bit      (file,buf,header);
        ConvertFrom32bit  (buf,bmp);
        break;
      case 24:
        buf = pool->alloc (bmp->width*bmp->height*3);
        LoadBMP24bit     (file,buf,header);
        ConvertFrom24bit (buf,bmp);
        break;  
      case 15:
        buf = pool->alloc (bmp->width*bmp->height*2);
        LoadBMP16bit      (file,buf,header);
        ConvertFrom15bit  (buf,bmp);
        break;  
      case 16:
        buf = pool->alloc (bmp->width*bmp->height*2);
        LoadBMP16bit      (file,buf,header);
        ConvertFrom16bit  (buf,bmp);
        break;  
      case 8:
        buf = pool->alloc (bmp->width*bmp->height);
        if      (header.Compression == 0) LoadBMP8bit     (file,buf,pal,header,fileHeader.BitmapOffset);
        else if (header.Compression == 1) LoadBMP8bitComp (file,buf,pal,header,fileHeader.BitmapOffset);
        else                           Error = 1;
        if (!Error) ConvertFrom8bit (buf,pal,bmp);        
        break;
      default:
        Error = 1;
    }

    if (Error)
    {
      os<<"LoadBMP: Error at load body "<<endl;

      if (buf && bmp && buf != bmp->data) pool->free (buf);
      if (bmp) pool->free (bmp);
      
      return NULL;
    }    

    if (buf && bmp && buf != bmp->data) pool->free (buf);
    
    bmp->bpp = 32; 

    return bmp;
  }
  D_Exception (EXCEPTION_EXECUTE_HANDLER)
  {
    os<<"LoadBMP: Undefined error "<<endl;

    D_Try
    {
      if (buf && bmp && buf != bmp->data) pool->free (buf);
      if (bmp) pool->free (bmp);
    }
    D_Exception (EXCEPTION_EXECUTE_HANDLER)
    {
    }

    return NULL;
  }
}

inline Quaternion& Quaternion::operator  *= (float _x)
{
  x *= _x;
  y *= _x;
  z *= _x;
  w *= _x;
}

inline Quaternion& Quaternion::operator  /= (float v)
{
  if (v != 0.0f)
  {
    float _x = 1.0f / v;  

    x *= _x;
    y *= _x;
    z *= _x;
    w *= _x;
  }  
}
inline Quaternion  operator  *  (const Quaternion& q,float v)
{
  return Quaternion (q.x*v,q.y*v,q.z*v,q.w*v);
}

inline Quaternion  operator  /  (const Quaternion& q,float _x)
{
  if (v!=0.0f)    
  {
    float v = 1.0f / _x;        

    return Quaternion (q.x*v,q.y*v,q.z*v,q.w*v);
  }  
  else            
    return q;  
}

INLINE vector3d_t    Quaternion::transform    (const vector3d_t& v) const
{
  Quaternion q = *this * Quaternion (v.x,v.y,v.z,0) * this->invert () ;
//  Quaternion q = Quaternion (v.x,v.y,v.z,0) * *this * this->invert () ;

  cout<<q.w<<endl;

  return vector3d_t (q.x,q.y,q.z);
}


matrix_t*   Trimesh3DS::createTransMatrix (BinFile* file)
{
  matrix3ds_t   trans3ds;

  file->read (&trans3ds,sizeof (matrix3ds_t));  

  matrix_t*     m = (matrix_t*)pool()->alloc (sizeof (matrix_t));
  
  if (m == NULL)
  {
    #ifdef DEBUG
      dout<<"Trimesh3DS: No enough memory "<<endl;
    #endif
    D_Error (EXCEPTION_NO_MEMORY);
  }

  matrix_t&   x   =  *m;

  x [0][0]        = trans3ds.m [0][0];  
  x [0][1]        = trans3ds.m [0][2];  
  x [0][2]        = trans3ds.m [0][1];  
//  x [0][3]        = -trans3ds.offs.x;  
  x [0][3]        = 0;
  x [1][0]        = trans3ds.m [2][0];  
  x [1][1]        = trans3ds.m [2][2];  
  x [1][2]        = trans3ds.m [2][1];  
  x [1][3]        = 0;
//  x [1][3]        = -trans3ds.offs.z;  
  x [2][0]        = trans3ds.m [1][0];  
  x [2][1]        = trans3ds.m [1][2];  
  x [2][2]        = trans3ds.m [1][1];  
//  x [2][3]        = -trans3ds.offs.y;  
  x [2][3]        = 0;
  x [3][0]        = 0;
  x [3][1]        = 0;
  x [3][2]        = 0;
  x [3][3]        = 1;

//  x = matrix_t::translate (vector3d_t (-trans3ds.offs.x,-trans3ds.offs.z,-trans3ds.offs.y)) * x;
  x = matrix_t::translate (vector3d_t (-trans3ds.offs.x,-trans3ds.offs.y,-trans3ds.offs.z)) * x;

  return m;
}

/*  float A, B, C, D, E, F, G, H;

  A = (w + x) * (q.w + q.x);
  B = (z - y) * (q.y - q.z);
  C = (x - w) * (q.y - q.z);
  D = (y + z) * (q.x - q.w);
  E = (x + z) * (q.x + q.y);
  F = (x - z) * (q.x - q.y);
  G = (w + y) * (q.w - q.z);
  H = (w - y) * (q.w + q.z);

  w = B + (-E - F + G + H) * 0.5f;
  x = A - ( E + F + G + H) * 0.5f; 
  y =-C + ( E - F + G - H) * 0.5f;
  z =-D + ( E - F - G + H) * 0.5f;

  return *this;*/


          trackpos3ds_t  pos;

          file->read (&pos.main,sizeof (pos.main));
          pos.pos = (position3ds_t*)pool()->alloc (sizeof (position3ds_t) * pos.main.num);
          obj->anim->scale = pos.main.num ? 
                             (vecseg3ds_t*)pool()->alloc (pos.main.num * sizeof (vecseg3ds_t)) :
                             NULL;
          obj->anim->scaleCount = pos.main.num;

          for (int i=0;i<pos.main.num;i++)
          {
            uint   frame;
            uint16 flags;
            uint   mask = 1;

            file->read (&frame,sizeof (frame));
            file->read (&flags,sizeof (flags));

            pos.pos [i].frame = frame;
            pos.pos [i].flags = flags;

            for (int j=0;j<5;j++,mask<<=1)
              if (flags & mask) file->read (&pos.pos [i].spline [j],sizeof (float));
              else              pos.pos [i].spline [j] = 0;

            file->read (&pos.pos [i].pos,sizeof (vector3ds_t)); 

            if (i)
            {
              obj->anim->scale [i].p [0]   = vector3d_t (pos.pos [i].pos.x,pos.pos [i].pos.y,pos.pos [i].pos.z);
              obj->anim->scale [i-1].p [1] = obj->anim->scale [i].p [0];
            }  
            else
              obj->anim->scale [i].p [0]   = vector3d_t (pos.pos [i].pos.x,pos.pos [i].pos.y,pos.pos [i].pos.z);          
          }

          obj->anim->scale [pos.main.num-1].p [1] = obj->anim->scale [pos.main.num-1].p [0];

          if (pos.main.num)
          {
            for (int i=0;i<pos.main.num;i++)
            {
              vecseg3ds_t& seg = obj->anim->scale [i];

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
                  float* cur  = (float*)&seg.p [0];
                  float* next = (float*)&seg.p [1];
                  float* prev = (float*)&obj->anim->scale [i-1].p [0];

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
                    next = (float*)&obj->anim->scale [i+1].p [1];

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
                  float* cur  = (float*)&seg.p [0];
                  float* next = (float*)&seg.p [1];
                  float* prev = NULL;

                  for (int j=0;j<3;j++)
                    r [0][j] = next [j] - cur [j];                  

                  if (i+1 != pos.main.num - 1)
                  {
                    prev = cur;
                    cur  = next;
                    next = (float*)&obj->anim->scale [i+1].p [1];

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

                seg.a = seg.p [0] * 2.0f  - seg.p [1] * 2.0f + vec [0] + vec [1];
                seg.b = - seg.p [0] * 3.0f  + seg.p [1] * 3.0f - vec [0] * 2.0f - vec [1];
                seg.c = vec [0];
                seg.d = seg.p [0];
              }
              else
              {
                seg.p [1]      = seg.p [0];
                seg.end        = pos.pos [i].frame;
                seg.locTime    = 1.0f;
                seg.a          = 0;
                seg.b          = 0;
                seg.c          = 0;
                seg.d          = 0;
              }            
            }  
          }

          pool()->free (pos.pos);


quatseg_t*  LoadQuatAnim   (BinFile* file,Pool* pool,int& count)
{
  D_Try
  {
    typedef rot_t rotarray_t [2];

    trackrot3ds_t  rot;    
    quat_t         q;
    quatseg_t*     anim = NULL;
    rotarray_t*    r    = NULL;

    file->read (&rot.main,sizeof (rot.main));

    rot.rot    = (rotate3ds_t*)pool->alloc (sizeof (rotate3ds_t) * rot.main.num);
    anim       = rot.main.num ? 
                 (quatseg3ds_t*)pool->alloc (rot.main.num * sizeof (quatseg3ds_t)) :
                 NULL;
    r          = rot.main.num ?
                 (rotarray_t*)pool->alloc (rot.main.num * sizeof (rotarray_t)) :
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

        r [i][0].angle   = rot.rot [i].angle;
        r [i][0].vec     = ort;

        r [i-1][1].angle = rot.rot [i].angle;
        r [i-1][1].vec   = ort;
      }
      else
      {
        q                     = quat_t (ort,rot.rot [i].angle);

        anim [i].v1        = q;            

        r [i][0].angle = rot.rot [i].angle;
        r [i][0].vec   = ort;
      }              

      if (i == rot.main.num-1)
      {
        r [i][0].angle = rot.rot [i].angle;
        r [i][0].vec   = ort;

        r [i][1].angle = rot.rot [i].angle;
        r [i][1].vec   = ort;
      }
    }

    anim [rot.main.num-1].v2 = q;

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

//          dout<<"BIAS: "<<bias<<" TENSION: "<<tension<<" CONT: "<<continuity<<endl;

          seg.end        = rot.rot [i+1].frame;
          seg.locTime    = 1.0f/float(seg.end-seg.start);

          if (i)
          {
            quat_t* cur  = &seg.v1;
            quat_t* prev = &anim [i-1].v1;
            quat_t* next = &seg.v2;

            slerp (g1,*cur,*prev,-(1.0f+bias)/3.0f);
            slerp (g2,*cur,*next,(1.0f-bias)/3.0f);
            slerp (g3,g1,g2,0.5f + 0.5f * continuity);
            slerp (g4,g1,g2,0.5f - 0.5f * continuity);
//            slerp (seg.c1,*cur,g3,-(tension-1.0f));
            slerp (seg.c1,*cur,g3,(tension-1.0f));

//cur.ra = slerp(cur, g3, (tension-1));
//cur.rb = slerp(cur, g3, -(tension-1));
//q0.rb = slerp(p0, p1, (1-tension)*(1+continuity*bias)/3.0);
//qn.ra = slerp(pn, p(n-1), (1-tension)*(1-continuity*bias)/3.0);

            if (i+1 != rot.main.num - 1)
            {
              prev = cur;
              cur  = next;
              next = &anim [i+1].v2;

              slerp (g1,*cur,*prev,-(1.0f+bias)/3.0f);
              slerp (g2,*cur,*next,(1.0f-bias)/3.0f);
              slerp (g3,g1,g2,0.5f + 0.5f * continuity);
              slerp (g4,g1,g2,0.5f - 0.5f * continuity);
//              slerp (seg.c2,*cur,g3,(tension-1.0f));
              slerp (seg.c2,*cur,g3,-(tension-1.0f));
            }
            else
              slerp(seg.c2,*next,*cur,(1.0f-tension)*(1.0f-continuity*bias)/3.0);
          }
          else
          {
            quat_t* cur  = &seg.v1;
            quat_t* next = &seg.v2;
            quat_t* prev = NULL;

//q0.rb = slerp(p0, p1, (1-tension)*(1+continuity*bias)/3.0);

//            slerp(seg.c1,*cur,*next,(1.0f-tension)*(1.0f+continuity*bias)/3.0);
            slerp(seg.c2,*cur,*next,(1.0f-tension)*(1.0f+continuity*bias)/3.0);

            if (i+1 != rot.main.num - 1)
            {
              prev = cur;
              cur  = next;
              next = &anim [i+1].v2;

              slerp (g1,*cur,*prev,-(1.0f+bias)/3.0f);
              slerp (g2,*cur,*next,(1.0f-bias)/3.0f);
              slerp (g3,g1,g2,0.5f + 0.5f * continuity);
              slerp (g4,g1,g2,0.5f - 0.5f * continuity);
//              slerp (seg.c2,*cur,g3,(tension-1.0f));
              slerp (seg.c2,*cur,g3,-(tension-1.0f));
            }
            else
              slerp(seg.c2,*next,*cur,(1.0f-tension)*(1.0f-continuity*bias)/3.0);
          }
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

    pool->free (r);
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

/////////////////////////////////////////////////////////////////////////////////////////////
///Вершинный буфер
/////////////////////////////////////////////////////////////////////////////////////////////
class VertexBuffer
{
  public:
                 VertexBuffer  ();
                 ~VertexBuffer () { release (); }
        
        struct   VertexType        
        {
          uint   elCount;
          uint   elType;
          ulong  offs;
        };

        struct  VBHeader
        {
          uint       vertex_count;
          VBTypes    type;
        };

/////////////////////////////////////////////////////////////////////////////////////////////
///Информация 
/////////////////////////////////////////////////////////////////////////////////////////////
       uint                   count   () const { return mCount; } 
       VBTypes                type    () const { return mType;  }
       size_t                 vsize   () const { return mVertexSize; }
       void*                  data    () const { return mData;  }

       void*                  operator [] (uint i) const { return (void*)((uchar*)mData + mVertexSize * i); }

/////////////////////////////////////////////////////////////////////////////////////////////
///Получение данных по вершинному типу //i - порядковый номер в VBTypes
/////////////////////////////////////////////////////////////////////////////////////////////
       const VertexType&      get     ( i) const  { return mVertexTypes [i&31]; }
       void                   set     (uint i,VertexType& v)  { mVertexTypes [i&31] = v; }

       static inline uint     index   (VBTypes t);

/////////////////////////////////////////////////////////////////////////////////////////////
///Загрузка / сохранение
/////////////////////////////////////////////////////////////////////////////////////////////
       bool                   load    (BinFile*,Pool*); //стандартная схема
       bool                   load    (const VBHeader&,void* data,Pool* = NULL); //pool = NULL - ссылка
       void                   save    (BinFile*);

/////////////////////////////////////////////////////////////////////////////////////////////
///Удаление буффера
/////////////////////////////////////////////////////////////////////////////////////////////
       void                   release ();

  private:
       VBTypes          mType;
       uint             mCount;
       size_t           mVertexSize;
       bool             mIsOwner;
       VertexType*      mVertexTypes; 
       void*            mData;
};

////////////////////////implementation of class VertexBuffer/////////////////////////////////

bool  VertexBuffer::load    (BinFile* file,Pool* pool)
{
  if (!IsValid (pool) || !IsValid (file))
    return false;
  
  release ();

  VBHeader hdr;

  file->read (&hdr,sizeof (VBHeader));
  
  mType        = hdr.type;
  mCount       = hdr.count;
  mVertexTypes = hdr.info;
  mVertexSize  = 0;
  mIsOwner     = true;

  for (int i=0;i<32;mVertexSize+=glTypesSize [mVertexTypes [i++].type]); 

  mData        = new (pool) char [mVertexSize * mCount];

  if (!mData)
  {
    mType        = 0;
    mCount       = 0;
    mVertexSize  = 0;
    mIsOwner     = false;

    return false; 
  }  

  file->read (mData,mVertexSize * mCount);

  return true;
}

bool  VertexBuffer::load    (const VBHeader& hdr,void* data,Pool* pool)
{
  release ();

  if (!pool)
  {
    mType        = hdr.type;
    mCount       = hdr.count;
    mVertexTypes = hdr.info;
    mVertexSize  = 0;
    mIsOwner     = false;
    mData        = data;

    for (int i=0;i<32;mVertexSize+=glTypesSize [mVertexTypes [i++].type]); 

    return true;
  }
  else
  {
    mData = new (pool) char [mVertexSize * mCount];

    if (!mData)
      return false; 

    mType        = hdr.type;
    mCount       = hdr.count;
    mVertexTypes = hdr.info;
    mVertexSize  = 0;
    mIsOwner     = true;

    for (int i=0;i<32;mVertexSize+=glTypesSize [mVertexTypes [i++].type]); 

    memcpy (mData,data,mVertexSize * mCount);

    return true;      
  }
}

void  VertexBuffer::release ()
{
  if (mIsOwner && mData)
    delete [] mData;

  mData       = NULL;
  mType       = 0;
  mCount      = 0;
  mVertexSize = 0;
  mIsOwner    = false;
}

VertexBuffer::VertexBuffer  ()
{
  mData       = NULL;
  mType       = 0;
  mCount      = 0;
  mVertexSize = 0;
  mIsOwner    = false;
}


static uint glTypesSize [] = {
    1, //GL_BYTE                    
    1, //GL_UNSIGNED_BYTE           
    2, //GL_SHORT                   
    2, //GL_UNSIGNED_SHORT          
    4, //GL_INT                     
    4, //GL_UNSIGNED_INT            
    4, //GL_FLOAT                   
    2, //GL_2_BYTES                 
    3, //GL_3_BYTES                 
    4, //GL_4_BYTES                 
    8, //GL_DOUBLE                  
};