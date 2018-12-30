#include "pch.h"
#include <graph\utils.h>
#include <fixed\tables.h>
#include <gl\gl.h>
#include <gl\glu.h>
#include <string.h>
#include <graph\textenv.h>

#include "r_lenv.h"

bool           TileLightEnv::valid         () const
{
  return mIndex != NULL && mOn != NULL && mLightDesc != NULL && 
         mTileDesc != NULL && mFlags.valid () && mLightEnable.valid () &&
         mLightPresent.valid () && mLightOn.valid ();
}

void           TileLightEnv::Refresh       ()
{
  for (int i=0;i<mVisNum;i++)
    if (mOn [i] != -1)
      ::SetLight (i,mLightDesc [mOn [i]].visLight);
}

void           TileLightEnv::Reset         ()
{
  glEnable (GL_LIGHTING);

  mK [0] = 0.33f;
  mK [1] = 0.33f/mMap->step  * 0.2f;
  mK [2] = 0.33f/(mMap->step * mMap->step) * 0.2f;

  for (int i=0;i<8;i++)
  {
//    glLightf (GL_LIGHT0+i,GL_CONSTANT_ATTENUATION,mK [0]);    
//    glLightf (GL_LIGHT0+i,GL_LINEAR_ATTENUATION,mK [1]);    
//    glLightf (GL_LIGHT0+i,GL_QUADRATIC_ATTENUATION,mK [2]);    
  }
}

BOOL     TileLightEnv::SetLightEnv   (int tile,uint _InterruptDrawing)
{
  LRing::diter    iter     = mGlobalLights;
  int             gcount   = mGlobalLights.count ();
  char            on  [8]  = {0};
  int             off [8]  = {0};
  int             offCount = 0,
                  onCount  = 0;

  for (int i=0;i<gcount;i++,iter++)
    if (!mLightOn [*iter] && mLightEnable [*iter]) 
      off [offCount++] = *iter;
    else if (mLightEnable [*iter])
    {
      on [mLightDesc [*iter].hwlight] = TRUE;
      onCount++;
    }  

  if (onCount < 8)
  {
    int ops = mVisNum - onCount;

    for (tiledesc_t* desc = mIndex [tile].last;desc && ops;desc = desc->prev)
      if (!mLightOn [desc->light] && mLightEnable [desc->light]) 
      {
        off [offCount++] = desc->light;
        ops--;
      }  
      else if (mLightEnable [desc->light])
      {
        on  [mLightDesc [desc->light].hwlight] = TRUE;
        onCount++;
      }  
  }

  int   pos = 0;

  for (i=0;i<offCount;i++)
  {
    for (;pos < mVisNum && on [pos];pos++);

    if (pos < mVisNum)
    {
      if (mOn [pos] == -1) 
        glEnable (pos+GL_LIGHT0);
      else
        mLightOn.clear (mOn [pos]);

      if (_InterruptDrawing)
      {
        glEnd          ();
        if (mOn [pos] == -1)  glEnable (pos+GL_LIGHT0);      
        ::SetLight     (pos,mLightDesc [off [i]].visLight);
        glBegin        (_InterruptDrawing);
      }
      else
        ::SetLight     (pos,mLightDesc [off [i]].visLight);

      mLightOn.set   (mOn [pos]  = off [i]);
      mLightDesc [off [i]].hwlight = pos;
      on [pos]                     = TRUE;

//      dout<<"On light: "<<pos<<" "<<offCount<<" "<<onCount<<endl;
    }
    else
      break;
  }

  BOOL isIntr = FALSE;

  for (i=0;i<mVisNum;i++)
    if (!on [i] && mOn [i] != -1)
    {
//      dout<<"Off light: "<<i<<endl;

      mLightOn.clear (mOn [i]);

      isIntr  = TRUE;
      mOn [i] = -1; 
      if (_InterruptDrawing) glEnd     ();
      glDisable (i+GL_LIGHT0);
    }

  if (isIntr && _InterruptDrawing)
    glBegin   (_InterruptDrawing);

  return offCount != 0;
}

int            TileLightEnv::NewLight      (const lightsrc_t& light,int mode)
{
  D_Try
  {
    if (mode & TLE_LOCAL_LIGHT && mLocalCount > 0)
    {
      int lindex = AllocLightSrc ();

      if (lindex == -1)
      {
        #ifdef DEBUG
          dout<<"TileLightEnv: No free light sources"<<endl;
        #endif
        return -1;
      }

      lightdesc_t& desc       = mLightDesc [lindex];
      desc.light              = light;
      desc.visLight           = light.light;
      desc.visLight.diffuse  *= light.shiness;
      desc.visLight.specular *= light.shiness;
      desc.visLight.ambient  *= light.shiness;
      desc.mode               = TLE_LOCAL_LIGHT;

      RecalcLightMap (lindex);

      if (mode & TLE_ENABLE_LIGHT) mLightEnable.set (lindex);

      return lindex;
    }
    else
    {
      int lindex = AllocLightSrc ();

      if (lindex == -1)
      {
        #ifdef DEBUG
          dout<<"TileLightEnv: No free light sources"<<endl;
        #endif
        return -1;
      }

      lightdesc_t& desc       = mLightDesc [lindex];
      desc.light              = light;
      desc.visLight           = light.light;
      desc.visLight.diffuse  *= light.shiness;
      desc.visLight.specular *= light.shiness;
      desc.visLight.ambient  *= light.shiness;
      desc.mode               = TLE_GLOBAL_LIGHT;

      mGlobalLights.insert (lindex);
      mLocalCount--;

      if (mode & TLE_ENABLE_LIGHT) mLightEnable.set (lindex);

      return lindex;
    }
  }
  D_Exception (EXCEPTION_EXECUTE_HANDLER)
  {
    #ifdef DEBUG
      dout<<"TileLightEnv: Error at NewLight "<<endl;
    #endif
    return -1;
  }
}

void           TileLightEnv::DeleteLight   (int lindex)
{
  D_Try
  {
    if (lindex < 0 || lindex >= mLightNum || !mLightPresent [lindex])
      return;
      
    lightdesc_t& desc = mLightDesc [lindex];

    if (desc.mode == TLE_GLOBAL_LIGHT)
    {
      mGlobalLights.erase (lindex);
      FreeLightSrc        (lindex);
      mLocalCount++;
    }
    else if (desc.mode == TLE_LOCAL_LIGHT)
    {
      ResetLight    (lindex);
      FreeLightSrc  (lindex);
    }
  }
  D_Exception (EXCEPTION_EXECUTE_HANDLER)
  {
    #ifdef DEBUG
      dout<<"TileLightEnv: Error at DeleteLight: "<<lindex<<endl;
    #endif
  }
}
         
void           TileLightEnv::ChangeLight   (int lindex,const lightsrc_t& light)
{              
  D_Try
  {
    if (lindex < 0 || lindex >= mLightNum || !mLightPresent [lindex])
      return;

    lightdesc_t& desc       = mLightDesc [lindex];

    desc.light              = light;
    desc.visLight           = light.light;
    desc.visLight.diffuse  *= light.shiness;
    desc.visLight.specular *= light.shiness;
    desc.visLight.ambient  *= light.shiness;

    if (desc.mode == TLE_LOCAL_LIGHT)
      RecalcLightMap (lindex);

    if (mLightOn [lindex])
      mLightOn.clear (lindex);
  }
  D_Exception (EXCEPTION_EXECUTE_HANDLER)
  {
    #ifdef DEBUG
      dout<<"TileLightEnv: Error at ChangeLight "<<endl;
    #endif
  }
}        

void           TileLightEnv::ResetLight     (int lindex)
{
  lightdesc_t& desc = mLightDesc [lindex];
  
  if (desc.firstTile)
  {
    tiledesc_t* next = desc.firstTile->nextDesc;

    for (tiledesc_t* node = desc.firstTile;node;node = next,next=next?next->nextDesc:NULL)
      DeleteLight (node);
    
    desc.firstTile   = NULL;
  }
}

float          TileLightEnv::GetPrior      (const boundbox_t& box,const lightsrc_t& light)
{
  float dist  = length ((box.max+box.min)/2.0f - light.light.pos);
  float dist1 = dist * dist;

  return light.shiness / (mK [0] + mK [1] * dist + mK [2] * dist1);
}

void           TileLightEnv::RecalcLightMap (int lindex)
{
  if (lindex < 0 || lindex >= mLightNum || !mLightPresent [lindex]) 
    return;

  ResetLight (lindex);

  lightsrc_t& light = mLightDesc [lindex].light;
  tiledesc_t* desc  = mLightDesc [lindex].firstTile;

  float    step     = mMap->step;
  int      mapWidth = mMap->width, 
           mapHeight= mMap->height;
  float    fldirx   = light.light.dir.x,
           fldiry   = light.light.dir.z;
  float    _div     = 1.0f/sqrt (fldirx*fldirx + fldiry*fldiry);
  float    fl_depth = mMaxDist * light.shiness * TLE_MAX_DIST;

  fldirx           *= _div;
  fldiry           *= _div;

  Fixed    dirx     = float2fixed (fldirx),
           diry     = float2fixed (fldiry);      

  Fixed    sx       = float2fixed (light.light.pos.x / step) - dirx,
           sy       = float2fixed (light.light.pos.z / step) - diry;
  Angle    fx_angle = float2angle (light.light.angle);
  float    fl_angle = light.light.angle;

  Fixed    len      = 0;
  Fixed    dlen     = sin (fx_angle) << 1;
  Fixed    dx1      = mul (dirx,cos(fx_angle)) - mul (diry,sin(fx_angle)),
           dy1      = mul (dirx,sin(fx_angle)) + mul (diry,cos(fx_angle)),
           dx2      = mul (dirx,cos((Angle)-fx_angle)) - mul (diry,sin((Angle)-fx_angle)),
           dy2      = mul (dirx,sin((Angle)-fx_angle)) + mul (diry,cos((Angle)-fx_angle)),
           x1       = sx,
           y1       = sy;
  Fixed    w        = int2fixed (mapWidth),
           h        = int2fixed (mapHeight); 

  int      depth    = fabs(fl_depth / (cos (fl_angle) * step)) + 2;  
  float    length   = fl_depth / cos (fl_angle);

  for (int i=0;i<depth;i++,x1+=dx1,y1+=dy1,len+=dlen)
  {
    Fixed x     = x1,
          y     = y1;                            
              
    for (Fixed l=0;l<len;l+=FX_ONE,x+=diry,y-=dirx)
      if (x >= 0 && y >= 0 && x < w && y < h)
      {
        int index = fixed2int(y)*mapWidth+fixed2int(x);                

        if (!mFlags.check (index))
        {
          tiledesc_t* temp = ChangeTile (index,
                                         lindex,
                                         GetPrior (mMap->tile (fixed2int(x),
                                                               fixed2int(x)).box,
                                                   light
                                                  )
                                        );

          if (temp)   
          {
            if (desc) desc = desc->nextDesc                = temp;
            else      desc = mLightDesc [lindex].firstTile = temp;

            temp->tile     = index;
          }  

          mFlags.set (index);
        }  
      }  
  }  

  mFlags.reset ();
}

TileLightEnv::tiledesc_t*    TileLightEnv::ChangeTile     (int tile,int light,float prior)
{
  tiledesc_t  *first  = mIndex [tile].first,
              *last   = mIndex [tile].last;
  tiledesc_t  *change = NULL;

  if (first && first->prior < prior) 
    for (tiledesc_t* node = first;node && prior > node->prior;change = node,node = node->next);
  else if (mIndex [tile].count >= mLocalCount || prior < mMinPrior)
    return NULL;

  return InsertLight (change,mIndex [tile],light,prior);
}

TileLightEnv::tiledesc_t* TileLightEnv::InsertLight    
 ( TileLightEnv::tiledesc_t*   prev,
   TileLightEnv::descindex_t&  index,
   int                         lindex,
   float                       prior
 )
{
   //‚αβ Ά«ο¥β ―®α«¥ prev

  tiledesc_t* desc = AllocTileDesc ();

  if (!desc)
  {
    #ifdef DEBUG
      dout<<"TileLightEnv: No free tile descriptors "<<endl;
    #endif
    return NULL;
  }

  if (prev)
  {  
    desc->prev = prev;
    desc->next = prev->next;
    prev->next = desc;
  }
  else
  {
    desc->prev  = NULL;
    desc->next  = index.first;
    index.first = desc;        
  }

  if (desc->next) desc->next->prev = desc;
  else            index.last       = desc;

  desc->light   = lindex;
  desc->prior   = prior;

  index.count++;

  return desc;
}

void           TileLightEnv::DeleteLight    (TileLightEnv::tiledesc_t* desc)
{
  if (desc)
  {
    descindex_t& index = mIndex [desc->tile];

    if (desc->prev) desc->prev->next = desc->next;
    else            index.first      = desc->next;
    if (desc->next) desc->next->prev = desc->prev;
    else            index.last       = desc->prev;

    FreeTileDesc (desc);

    index.count--;
  }
}

int   TileLightEnv::AllocLightSrc   ()
{
  int _free = mFirstLightDesc;

  if (_free == -1) return -1;

  mFirstLightDesc               = mLightDesc [_free].nextDesc;
  mLightDesc [_free].firstTile  = NULL;
  mLightDesc [_free].nextDesc   = -1;

  mLightPresent.set (_free);
 
  return _free;
}

void           TileLightEnv::FreeLightSrc    (int index)
{
  if (index == -1) return;

  mLightDesc [index].nextDesc  = mFirstLightDesc;
  mFirstLightDesc              = index;

  mLightPresent.clear (index);
  mLightEnable.clear  (index);
  mLightOn.clear      (index);
}

TileLightEnv::tiledesc_t*    TileLightEnv::AllocTileDesc   ()
{
  tiledesc_t* _free = mFirstTileDesc;

  if (!_free) return NULL;

  mFirstTileDesc  = _free->nextDesc;
  _free->nextDesc = NULL;
  _free->next     = _free->prev = NULL;
  _free->light    = -1;
  _free->prior    = 0;
  _free->tile     = -1;

  return _free;
}

void           TileLightEnv::FreeTileDesc    (TileLightEnv::tiledesc_t* desc)
{
  if (desc)
  {
    desc->next     = desc->prev = NULL;
    desc->nextDesc = mFirstTileDesc;
    mFirstTileDesc = desc;    
  }
}

vector3d_t     TileLightEnv::GetMidColor   (int tile)
{
  D_Try
  {
    if (tile < 0 || tile >= mMap->width * mMap->height)
      return vector3d_t (.0f);

    vector3d_t ambient (.0f);
    int        count (.0f);

    for (tiledesc_t* desc = mIndex [tile].first;desc;desc=desc->next,count++)
      ambient += mLightDesc [desc->light].light.light.ambient;

    ambient /= float (count);

    return ambient;
  }     
  D_Exception (EXCEPTION_EXECUTE_HANDLER)
  {
    #ifdef DEBUG
      dout<<"TileLightEnv: Error at GetMidColor "<<endl;
    #endif
    return vector3d_t (.0f);
  }
}

int            TileLightEnv::GetLight      (int tile,lightsrc_t* light)
{
  D_Try
  {
    int count = 0;

    for (tiledesc_t* desc = mIndex [tile].first;desc;desc=desc->next,count++)
      light [count] = mLightDesc [desc->light].light;    

    return count;
  }
  D_Exception (EXCEPTION_EXECUTE_HANDLER)
  {
    #ifdef DEBUG
      dout<<"TileLightEnv: Error at GetLight (int,lightsrc_t*) "<<endl;
    #endif
    return 0;
  }
}

BOOL           TileLightEnv::GetLight      (int lindex,lightsrc_t& light)
{
  D_Try
  {
    if (lindex < 0 || lindex >= mLightNum || !mLightPresent [lindex])
      return FALSE;

    light = mLightDesc [lindex].light;

    return TRUE;
  }
  D_Exception (EXCEPTION_EXECUTE_HANDLER)
  {
    #ifdef DEBUG
      dout<<"TileLightEnv: Error at GetLight "<<endl;
    #endif
    return FALSE;
  }
}

void           TileLightEnv::EnableLight   (int lindex)
{
  if (lindex >= 0 && lindex < mLightNum && mLightPresent [lindex]) 
    mLightEnable.set (lindex);
}

void           TileLightEnv::DisableLight  (int lindex)
{
  if (lindex >= 0 && lindex < mLightNum && mLightPresent [lindex]) 
  {
    mLightEnable.clear (lindex);
  }  
}

void           TileLightEnv::Draw ()
{
  D_Try
  {  
    for (int i=0;i<mLightNum;i++)
      if (mLightEnable [i] && IsInBound (mLightDesc [i].light.light.pos,6,mFrustum.clip))
        AddAlpha (length (mCamera.pos-mLightDesc [i].light.light.pos),i);
  }
  D_Exc ("TileLightEnv: Error at Draw");
}

void           TileLightEnv::DrawAlpha   (DWORD i,const Camera& eye)
{
/*  static flareTexture = TextEnvironment::CreateTexture ("flare.alp",pool());

  vector3d_t        pos   = mLightDesc [i].light.light.pos;
  vector3d_t&       col   = mLightDesc [i].light.light.diffuse;

  vector3d_t        dir     = pos - eye.pos ();
//  vector3d_t        up      = eye.up () * 8.0f;  //flare height
  vector3d_t        up      = matrix_t::rotate (normalize(eye.dir()^eye.up()),3.14/2) *
                              eye.dir () * 16.0f * mLightDesc [i].light.shiness;  //flare height
  float             dist    = dir.length ();
  dir                      /= dist;

//  if (dist >= RENDER_SPRITE_MIN_DIST)
  {
    glBindTexture (GL_TEXTURE_2D,flareTexture);
    glPushAttrib  (GL_COLOR_BUFFER_BIT);
    glBlendFunc   (GL_SRC_COLOR,GL_ONE);

    glEnable (GL_COLOR_MATERIAL);

    glColor4f     (col.x,col.y,col.z,1); ///alpha

                   //¨αγ¥¬

    vector3d_t        flang = up ^ eye.dir (); //flare width
    vector3d_t&       c     = pos;
    vector3d_t        v [4] = {
      c + up - flang,
      c - up - flang,
      c - up + flang,
      c + up + flang
    };                     

    dir                     = -dir;

    glBegin (GL_QUADS);
      glNormal3fv   ((GLfloat*)&dir);

      glTexCoord2f  (1,0);  
      glVertex3fv   ((GLfloat*)&v [3]);
      glTexCoord2f  (1,1);  
      glVertex3fv   ((GLfloat*)&v [2]);
      glTexCoord2f  (0,1);  
      glVertex3fv   ((GLfloat*)&v [1]);
      glTexCoord2f  (0,0);  
      glVertex3fv   ((GLfloat*)&v [0]);
    glEnd   ();
  }                           

  glDisable (GL_COLOR_MATERIAL);  

  glPopAttrib   ();

  mPgCount += 2;*/
}

TileLightEnv::TileLightEnv  (Pool* _Pool,LandscapeMap* _Map,int _LightCount)
             : Render        (_Pool),
               mMap          (_Map),
               mFlags        (_Pool,_Map->width*_Map->height),
               mLightEnable  (_Pool,_LightCount),
               mLightPresent (_Pool,_LightCount),
               mLightOn      (_Pool,_LightCount),
               mLightDesc    (NULL),
               mFirstTileDesc(NULL),
               mIndex        (NULL),
               mOn           (NULL),
               mLocalCount   (_LightCount),
               mLightNum     (_LightCount),
               mVisNum       (8),
               mFirstLightDesc (0),
               mGlobalLights (_Pool)
{
  M_PushPool (GetPool());

  D_Try
  {
    mIndex     = new descindex_t [mMap->width * mMap->height];
    mOn        = new int [mVisNum];
    mTileDesc  = new tiledesc_t [mMap->width * mMap->height * mVisNum];
    mLightDesc = new lightdesc_t [_LightCount]; 
    
    if (!mIndex || !mOn || !mTileDesc || !mLightDesc)
    {
      #ifdef DEBUG
        dout<<"TileLightEnv: No enough memory "<<endl;
      #endif
      return;      
    }

    for (int i=0;i<mVisNum;mOn [i++] = -1);

    memset (mIndex,0,sizeof (descindex_t) * mMap->width * mMap->height);

    tiledesc_t* desc     = mTileDesc;
    int         mapSize  = mMap->width * mMap->height * mVisNum;

    for (i=0;i<mapSize;i++,desc++)
    {
      desc->light    = -1;
      desc->prior    = 0;
      desc->next     = desc->prev = NULL;
      desc->nextDesc = desc + 1;
      desc->tile     = -1;
    }

    (desc-1)->nextDesc = NULL;
    mFirstTileDesc     = mTileDesc;

    lightdesc_t* l = mLightDesc;

    for (i=0;i<mLightNum;i++,l++)
      l->nextDesc = i+1;

    (l-1)->nextDesc = -1;        

    mMinPrior       = 0;
    mMaxDist        = sqrt (mMap->width*mMap->width + mMap->height*mMap->height) * mMap->step;

    Reset ();        
  }
  D_Exception (EXCEPTION_EXECUTE_HANDLER)
  {
    #ifdef DEBUG
      dout<<"TileLightEnv: Error at construct "<<endl;
    #endif
  }

  M_PopPool();
}

TileLightEnv::~TileLightEnv ()
{
  D_Try
  { 
    if (mIndex)         delete [] mIndex;
    if (mOn)            delete [] mOn;
    if (mLightDesc)     delete [] mLightDesc;
    if (mTileDesc)      delete [] mTileDesc;
  } 
  D_Exception (EXCEPTION_EXECUTE_HANDLER)
  {
    #ifdef DEBUG
      dout<<"TileLightEnv: Error at destruct "<<endl;
    #endif
  }
}
