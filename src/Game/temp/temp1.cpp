uint    ConvertVertexBuffer (
  int                    fcount,
  int                    vcount,
  facet_t*               farray,
  vertex_t*              vertex,
  vector3d_t*            normal,
  textcoord_t*           tcoord,
  uint                   chunkSize,
  uint                   rSize,
  vertexbuffer_t**       dest
)
{
  D_Try        
  {
    uint                vbNum = vcount / chunkSize + 16;
    uint                vbPos = 0;
    vertexbuffer_t*     vb    = new vertexbuffer_t [vbNum];
    *dest                     = vb;

    if (!vb)
    {
      #ifdef DEBUG
        dout<<"No enough memory for vertex buffer "<<endl;
      #endif
      return 0;
    }

    int vPos = 0;

    for (int i=0;vPos<vcount;i++,vPos+=chunkSize)
    {
      vb [i].vertex   = new vertex_t [chunkSize + rSize];
      vb [i].normal   = new vector3d_t [chunkSize + rSize];
      vb [i].tcoord   = new textcoord_t [chunkSize + rSize];

      if (!vb [i].vertex || !vb [i].normal || !vb [i].tcoord)
      {
        #ifdef DEBUG
          dout<<"No enough memory for vertex buffer "<<endl;
        #endif

        return i;
      }

      vb [i].count    = chunkSize + rSize;
      vb [i].pos      = vcount - vPos > chunkSize ? chunkSize : vcount - vPos;

      memcpy (vb [i].vertex,vertex + vPos,sizeof (vertex_t) * vb [i].pos);
      memcpy (vb [i].normal,normal + vPos,sizeof (vector3d_t) * vb [i].pos);
      memcpy (vb [i].tcoord,tcoord + vPos,sizeof (textcoord_t) * vb [i].pos);
    }

    facet_t*  facet = farray;

    for (i=0;i<fcount;i++,facet++)
    {
      facet->index [0] = facet->index [0] % chunkSize + (chunkSize + rSize) * (facet->index [0] / chunkSize);
      facet->index [1] = facet->index [1] % chunkSize + (chunkSize + rSize) * (facet->index [1] / chunkSize);
      facet->index [2] = facet->index [2] % chunkSize + (chunkSize + rSize) * (facet->index [2] / chunkSize);

      if (!CheckFacetVB (*facet,chunkSize))
      {
        int vbIndex [3] = {
          facet->index [0] / chunkSize,
          facet->index [1] / chunkSize,
          facet->index [2] / chunkSize
        };

        if (vbIndex [0] != vbIndex [1] && vbIndex [0] != vbIndex [2] && vbIndex [1] != vbIndex [2])
        {
          int mostVB = vbIndex [0];

          for (int k = 1;k<2;k++)
          {
            int index  = vbIndex [k];

            vertex_t&    v1     = vertex [index];
            vertex_t*    v2     = vb [mostVB].vertex;
            int          count  = vb [mostVB].pos;

            for (int j=0;j<count;j++,v2++)
              if (CmpVertex (v1,*v2))
              {
                facet->index [index] = j + mostVB * (chunkSize + rSize);
                
                break;
              }

            if (j == count)
              if (vb [mostVB].pos != vb [mostVB].count)
              {
                int pos = vb [mostVB].pos++;

                vb [mostVB].vertex [pos] = vb [vbIndex [index]].vertex [facet->index [index]];
                vb [mostVB].normal [pos] = vb [vbIndex [index]].normal [facet->index [index]];
                vb [mostVB].tcoord [pos] = vb [vbIndex [index]].tcoord [facet->index [index]];
              }
              else
              {
                #ifdef DEBUG
                  dout<<"No reserved vertex buffer limit for compile facet"<<endl;
                #endif
                return vbNum;
              }
          }
        }
        else if ((vbIndex [0] == vbIndex [1]) || (vbIndex [0] == vbIndex [2]) || (vbIndex [1] == vbIndex [2]))
        {
          int mostVB = vbIndex [0];
          int index  = vbIndex [1];

          if (vbIndex [0] == vbIndex [1]) mostVB = vbIndex [0];
          if (vbIndex [0] == vbIndex [2]) mostVB = vbIndex [0];
          if (vbIndex [2] == vbIndex [1]) mostVB = vbIndex [1];
          
          if (vbIndex [0] != mostVB) index = 0;
          if (vbIndex [1] != mostVB) index = 1;
          if (vbIndex [2] != mostVB) index = 2;

          vertex_t&    v1     = vertex [index];
          vertex_t*    v2     = vb [mostVB].vertex;
          int          count  = vb [mostVB].pos;

          for (int j=0;j<count;j++,v2++)
            if (CmpVertex (v1,*v2))
            {
              facet->index [index] = j + mostVB * (chunkSize + rSize);
              
              break;
            }

          if (j == count)
            if (vb [mostVB].pos != vb [mostVB].count)
            {
              int pos = vb [mostVB].pos++;

              vb [mostVB].vertex [pos] = vb [vbIndex [index]].vertex [facet->index [index]];
              vb [mostVB].normal [pos] = vb [vbIndex [index]].normal [facet->index [index]];
              vb [mostVB].tcoord [pos] = vb [vbIndex [index]].tcoord [facet->index [index]];
            }
            else
            {
              #ifdef DEBUG
                dout<<"No reserved vertex buffer limit for compile facet"<<endl;
              #endif
              return vbNum;
            }
        }
      }
    }

    return vbNum;
  }
  D_Exception (EXCEPTION_EXECUTE_HANDLER)
  {
    #ifdef DEBUG
      dout<<"Error at ConvertVertexBuffer "<<endl;
    #endif
    return 0;
  }
}


inline int   OutCode  (Fixed x,Fixed y,Fixed width,Fixed height)
{
  int code = 0;

  if (x < 0)           code |= 1;
  if (y < 0)           code |= 2;
  if (x > width)       code |= 4;
  if (y > height)      code |= 8;

  return code;
}

inline int   OutCodeFl  (float x,float y,float width,float height)
{
  int code = 0;

  if (x < 0)           code |= 1;
  if (y < 0)           code |= 2;
  if (x > width-1.0f)       code |= 4;
  if (y > height-1.0f)      code |= 8;

  return code;
}

/*inline Fixed ClipLine (Fixed& x1,Fixed& y1,Fixed x2,Fixed y2,Fixed width,Fixed height,Fixed len)
{
  int  code1   = OutCode (x1,y1,width,height),
       code2   = OutCode (x2,y2,width,height);
  BOOL inside  = (code1 | code2) == 0,
       outside = (code1 & code2) != 0;

  while (!outside && !inside)
  {
    if (code1 == 0)
    {
      swap (x1,x2);
      swap (y1,y2);
      swap (code1,code2);
    }

    if (code1 & 1)
    {
      Fixed k  = div (-x1,x2-x1);

      y1      += mul (y2-y1,k);
      x1       = 0;
      len      = len - mul (len,abs (k));
    }
    else if (code1 & 2)
    {
      Fixed k  = div (-y1,y2-y1);
      y1       = 0;
      x1      += mul (x2-x1,k);
      len      = len - mul (len,abs (k));
    }
    else if (code1 & 4)
    {            
      Fixed k  = div (width-x1,x2-x1);
      len      = len - mul (len,abs (k));
      x1       = width;
      y1      += mul (y2-y1,k);
    }
    else if (code1 & 8)
    {
      Fixed k  = div (height-y1,y2-y1);
      y1       = height;
      x1      += mul (x2-x1,k);
      len      = len - mul (len,abs (k));
    }

    code1   = OutCode (x1,y1,width,height);
    code2   = OutCode (x2,y2,width,height);
    inside  = (code1 | code2) == 0;
    outside = (code1 & code2) != 0;
  }

  return len;
} */

inline float ClipLineFl (float& x1,float& y1,float x2,float y2,float width,float height,float len)
{
  int  code1   = OutCodeFl (x1,y1,width,height),
       code2   = OutCodeFl (x2,y2,width,height);
  BOOL inside  = (code1 | code2) == 0,
       outside = (code1 & code2) != 0;

  while (!outside && !inside)
  {
    if (code1 == 0)
    {
      swap (x1,x2);
      swap (y1,y2);
      swap (code1,code2);
    }

    if (code1 & 1)
    {
      float k  = -x1 / (x2-x1);

      y1      += (y2-y1) * k;
      x1       = 0;
      len      = len - len * k;
    }
    else if (code1 & 2)
    {
      float k  = -y1 / (y2-y1);
      y1       = 0;
      x1      += (x2-x1) * k;
      len      = len - len * k;
    }
    else if (code1 & 4)
    {            
      float k  = (width-x1) / (x2-x1);
      len      = len - len * k;
      x1       = width;
      y1      += (y2-y1) * k;
    }
    else if (code1 & 8)
    {
      float k  = (height-y1) / (y2-y1);
      y1       = height;
      x1      += (x2-x1) * k;
      len      = len - len * k;
    }

    code1   = OutCodeFl (x1,y1,width,height);
    code2   = OutCodeFl (x2,y2,width,height);
    inside  = (code1 | code2) == 0;
    outside = (code1 & code2) != 0;
  }

  return outside ? 0 : len < 0 ? 0 : len;
}



Fixed ClipLine (Fixed& x1,Fixed& y1,Fixed x2,Fixed y2,Fixed width,Fixed height,Fixed len,Fixed& dirx,Fixed& diry)
{
  if (x1 > x2)
  {
    swap (x1,x2);
    swap (y1,y2);
    dirx = -dirx;
    diry = -diry;
  }

  if (x2 < 0 || x1 >= width)
    return 0;

  if ((y1 < 0 && y2 < 0) || (y1 >= height && y2 >= height))
    return 0;

  if (x1 < 0)
  {
    Fixed k  = div (x2,x2-x1);

    y1       = y2 - mul (y2-y1,k);
    x1       = 0;
    len      = mul (len,abs (k));
  }

//  if ((y1 < 0 && y2 < 0) || (y1 >= height && y2 >= height))
//    return 0;

  if (y1 < 0)
  {
    Fixed k  = div (-y1,y2-y1);

    y1       = 0;
    x1       = x2 - mul (x2-x1,k);
    len      = len - mul (len,abs (k));
  } 
  else if (y2 < 0)
  {
    Fixed k  = div (y1,y1-y2);

    y2       = 0;
    x2       = x1 + mul (x2-x1,k);
    len      = mul (len,abs (k));
  }

/*  if ((y1 < 0 && y2 < 0) || (y1 >= height && y2 >= height))
    return 0;*/

/*  if (x1 > x2)
  {
    swap (x1,x2);
    swap (y1,y2);
    dirx = -dirx;
    diry = -diry;
  }*/

  if (x1 >= width && x2 >= width)
    return 0;

/*  if (x2 >= width)
  {            
    Fixed k  = div (x2-width,x2-x1);
    x2       = width-FX_ONE;
    y2       = y2 - mul (y2-y1,k);
    len      = len - mul (len,k);
  }
*/
/*  if (y2 >= height)
  {
    Fixed k  = div (height-y1,y2-y1);
    y2       = height - FX_ONE;
    x2       = x1 + mul (x2-x1,k);
    len      = mul (len,k) + FX_ONE; //???
  }*/

  return len;
}


void     SortSB      ()
{
  for (int index=0;index<6;index++)
    for (int i=0;i<6;i++)
      if (textures [i].index == index)
      {
        swap (textures [i],textures [index]);
        break;
      }
}

istream& operator >> (istream& is,textindex_t& temp)
{
  dout<<"OLOL!"<<endl;

  static char indexName [128];
  int    index = 0;
  
  is>>indexName;

  locase (indexName,indexName);

  dout<<"Index: "<<indexName<<endl;

  if      (!strcmp (indexName,"bottom")) index = SB_BOTTOM_ID;
  else if (!strcmp (indexName,"left"))   index = SB_LEFT_ID;
  else if (!strcmp (indexName,"right"))  index = SB_RIGHT_ID;
  else if (!strcmp (indexName,"top"))    index = SB_TOP_ID;
  else if (!strcmp (indexName,"front"))  index = SB_FRONT_ID;
  else if (!strcmp (indexName,"back"))   index = SB_BACK_ID;
  else 
  {
    is>>indexName;
    return is;
  }  

  is>>textures [index].name;

  return is;
}


  boundbox_t   waterbox;
  const char*  watername = "textures/water/water1.jpg";

  waterbox.min    = c - vector3d_t (20,3,20) + vector3d_t (5);
  waterbox.max    = c + vector3d_t (20,3,20) + vector3d_t (5);

//  waterbox.max.y  = 100;  

//  mdl [7] = me->CreateModel (md [9],EFFECT_WAVE_WATER,waterbox,watername);

//  mdl [5] = me->CreateModel (md [0],c+vector3d_t (0,14,0),matrix_t::scale (vector3d_t (1)));
//  mdl [6] = me->CreateModel (md [10],c+vector3d_t (0,384,0),matrix_t::scale (vector3d_t (1)));

//  mdl [6] = me->CreateModel (md [0],c+vector3d_t (0,24,0),matrix_t::rotate (vector3d_t (0,1,0),3.14/2));
//  mdl [6] = me->CreateModel (md [5],c+vector3d_t (0,24,0),matrix_t::scale (vector3d_t (1)));

/*  shadow  = render->GetShadowEnv ()->CreateShadow (sky->GetLight(),mdl [6]);

  render->GetShadowEnv ()->EnableShadow (shadow);
  render->GetShadowEnv ()->refresh      (shadow);
*/
/*  SHADOW   shadow1  = render->GetShadowEnv ()->CreateShadow (light,mdl [6]);

  render->GetShadowEnv ()->EnableShadow (shadow1);
  render->GetShadowEnv ()->refresh      (shadow1);      */

#include <memory\cache.h>
#include <gl\gl.h>
#include <gl\glext.h>
//#include <gl\glprocs.h>

#include "r_vbuffer.h"

PFNGLLOCKARRAYSEXTPROC     glLockArraysFn    = NULL;
PFNGLUNLOCKARRAYSEXTPROC   glUnlockArraysFn  = NULL;

void     glLockArraysEXT   (GLint first,GLsizei count) { (*glLockArraysFn)(first,count); }
void     glUnlockArraysEXT (void) { (*glUnlockArraysFn)(); }

void          VertexBuffer::set           (int index)
{
  if (mCurVBuffer != index)
  {  
    vertexbuffer_t* vb = (vertexbuffer_t*)mCache->data (index);

    glUnlockArraysEXT ();

    glVertexPointer   (3,GL_FLOAT,0,vb->vertex);
    glNormalPointer   (GL_FLOAT,0,vb->normal);
    glTexCoordPointer (2,GL_FLOAT,0,vb->tcoord);
    
    glLockArraysEXT   (0,vb->pos);

    mCurVBuffer = index;
  }  
}

void          VertexBuffer::LoadData (DWORD par,int index,void* buf,size_t size)
{
  D_Try
  {
    VertexBuffer* vb   = (VertexBuffer*)par;
    ulong         offs = vb->mOffs + (vb->mVBSize + sizeof (vertexbuffer_t)) * index;
    
    vb->mFile->seek (offs);
    vb->mFile->read (buf,vb->mVBSize);

    vertexbuffer_t* vbuf = (vertexbuffer_t*)buf;
    char*           data = (char*)buf + sizeof (vertexbuffer_t);

    vbuf->vertex         = (vertex_t*)data;
    vbuf->normal         = (vector3d_t*)(data + sizeof (vertex_t) * vbuf->count);
    vbuf->tcoord         = (textcoord_t*)((char*)vbuf->normal + sizeof (vector3d_t) * vbuf->count);
  }
  D_Exception (EXCEPTION_EXECUTE_HANDLER)
  {
    #ifdef DEBUG
      dout<<"VertexBuffer: Error at LoadData "<<endl;
    #endif
  }
}

VertexBuffer::VertexBuffer  (Pool* _Pool,GameLevel* level,uint _VBCacheNum)
             : MemObject (_Pool),
               mCache    (NULL),
               mFile     (NULL),
               mOffs     (0),
               mVBSize   (0),
               mCurVBuffer (-1)
{
  D_Try
  {
    vbheader_t hdr;

    mFile = level->CreateLump (LL_VERTEX_BUFFER);

    mFile->read (&hdr,sizeof (hdr));

    mCache  = new (pool()) Cache (pool(),
                                  hdr.count, //????
                                  hdr.vbSize,
                                  _VBCacheNum<hdr.count?_VBCacheNum:hdr.count,
                                  LoadData,
                                  (DWORD)this
                                 );
    mOffs   = hdr.offs;
    mVBSize = hdr.vbSize;

    if (!IsValid (mCache))
    {
      #ifdef DEBUG
        dout<<"VertexBuffer: Error at creat cache "<<endl;
      #endif
      return;
    }

    glEnableClientState (GL_VERTEX_ARRAY);
    glEnableClientState (GL_TEXTURE_COORD_ARRAY);
    glEnableClientState (GL_NORMAL_ARRAY);

    glLockArraysFn    = (PFNGLLOCKARRAYSEXTPROC)wglGetProcAddress ("glLockArraysEXT");
    glUnlockArraysFn  = (PFNGLUNLOCKARRAYSEXTPROC)wglGetProcAddress ("glUnlockArraysEXT");
  }
  D_Exception (EXCEPTION_EXECUTE_HANDLER)       
  {
    #ifdef DEBUG
      dout<<"VertexBuffer: Error at construct "<<endl;
    #endif
  }
}

VertexBuffer::~VertexBuffer ()
{
  D_Try
  {
    if (mCache) delete mCache;
  }
  D_Exception (EXCEPTION_EXECUTE_HANDLER)
  {
    #ifdef DEBUG
      dout<<"VertexBuffer: Error at destruct "<<endl;
    #endif
  }
}
