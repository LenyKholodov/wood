#include "pch.h"
#include "r_hmrender.h"
#include "r_main.h"
#include "g_loader.h"

#include <gl\gl.h>
#include <fixed\tables.h>
#include <stdlib.h>

struct   HMapRender::tilenode_t
{
  hmtile_t*    tile;           //Указатель на описатель тайла
  int          detail;         //Детализация
  tilenode_t*  next;           //Следующая запись
};

struct  HMapRender::textnode_t
{
  int               next;   //Индекс следующего материала
  tilenode_t*       first;  //Первый описатель тайла
  int               num;    //Количество описателей
};  

inline void NewVertex (float x,float y,uchar *h)  
{  
  glTexCoord2f (x,y);
  glVertex3f   (x,*h,y);   
} 

uchar    HMapRender::height    (uchar* h,int dx,int dy)
{
  uchar* hmin = mMap.mMap,
       * hmax = hmin + mMap.mLoadWidth*mMap.mLoadHeight;

  if (h + dy < hmin || h + dy >= hmax || h + dx < hmin || h + dx >= hmax)
    return *h;

  return (*h + *(h+dx) + *(h+dy) + *(h+dx+dy))>>2;
}

void     HMapRender::DrawTile  (int x,int y,ulong s,ulong e,int w,int h,int levels)
{
  int    w1 = w>>1,
         h1 = h>>1;
  ulong  c  = s+h1*mMap.mLoadWidth+w1-1;

  if (levels == 1 || w <= 2 || h <= 2)
  {
    uchar mid = (mMap.mMap [c] + mMap.mMap [c+1] + 
                 mMap.mMap [c-mMap.mLoadWidth] + mMap.mMap [c-mMap.mLoadWidth+1]
                ) >> 2;

    glBegin (GL_TRIANGLE_FAN);
      NewVertex (float (x+w1)+0.5f,float (y+h1)-0.5f,&mid);
      NewVertex (x,y,mMap.mMap+s);
      NewVertex (x,y+h,mMap.mMap+e-w+1+mMap.mLoadWidth);  //wrong
      NewVertex (x+w,y+h,mMap.mMap+e+1+mMap.mLoadWidth);  //wrong
      NewVertex (x+w,y,mMap.mMap+s+w);
      NewVertex (x,y,mMap.mMap+s);
    glEnd   (); 

    mPgCount += 4;
  }
  else 
  {
    DrawTile (x,    y,    s,      c-mMap.mLoadWidth,    w1, h1, levels-1);
    DrawTile (x+w1, y,    s+w1,   c+w1-mMap.mLoadWidth, w1, h1, levels-1);
    DrawTile (x,    y+h1, c-w1+1, e-w1, w1, h1, levels-1);
    DrawTile (x+w1, y+h1, c+1,    e,    w1, h1, levels-1);
  }  
} 

void     HMapRender::CalcList  (const Camera& eye)
{
  float    mapWidth = float (mLandMap.width) * mLandMap.step, 
           mapHeight= float (mLandMap.height) * mLandMap.step;
  float    fldirx   = eye.dir.x,
           fldiry   = eye.dir.z;  
  float    _div     = 1.0f/sqrt (fldirx*fldirx + fldiry*fldiry);

  fldirx           *= _div;
  fldiry           *= _div;

  Fixed    dirx     = float2fixed (fldirx),
           diry     = float2fixed (fldiry);      

  Fixed    sx       = float2fixed (eye.pos.x / mapWidth * float (mMap.mInfoWidth)) - dirx,
           sy       = float2fixed (eye.pos.z / mapHeight * float (mMap.mInfoHeight)) - diry;
  float    fl_angle = RENDER_VIEW_ANGLE;
  Angle    fx_angle = float2angle (fl_angle);

  Fixed    len      = 0;
  Fixed    dlen     = sin (fx_angle) << 1;
  Fixed    dx1      = mul (dirx,cos(fx_angle)) - mul (diry,sin(fx_angle)),
           dy1      = mul (dirx,sin(fx_angle)) + mul (diry,cos(fx_angle)),
           x1       = sx,
           y1       = sy;
  Fixed    w        = int2fixed (mMap.mInfoWidth),
           h        = int2fixed (mMap.mInfoHeight);

  int      depth    = fabs(mMap.mTViewDepth *  cos (fl_angle)) + 1; 

//  if (sx >= 0 && sy >= 0 && sx < w && sy < h)
//  {
//    int       index           = mMap.index (fixed2int (sx),fixed2int (sy));
//    hmtile_t& tile            = mMap.tile (index);

//    DrawTile (tile.x,tile.y,tile.sOffs,tile.eOffs,mMap.mTileWidth,mMap.mTileHeight,2);
//  }

  for (int i=0;i<depth;i++,x1+=dx1,y1+=dy1,len+=dlen)
  {
    Fixed x     = x1,
          y     = y1;              
    BOOL  flag  = FALSE;

    if (abs (len) < FX_ONE)
      continue;
              
    for (Fixed l=0;l<len;l+=FX_ONE,x+=diry,y-=dirx)
    {
      if (x >= 0 && y >= 0 && x < w && y < h)
      {
        int       index   = mMap.index (fixed2int (x),fixed2int (y));
        hmtile_t& tile    = mMap.tile  (index);      
        flag              = TRUE;

        if (!mSBuffer->insert (tile.box))
          continue;

        int          mtr  = tile.texture;
        textnode_t&  text = mText [mtr];
        tilenode_t&  node = mTileNode [mTilePos++];

        if (mTilePos > mTileNum)
          break;
        
        if (text.first == NULL) 
        {
          text.next       = mTextFirst;
          mTextFirst      = mtr;
          text.first      = &node;
          node.tile       = &tile;
          node.next       = NULL;
          node.detail     = i;
        }
        else
        {          
          node.tile       = &tile;
          node.next       = text.first;
          text.first      = &node;
          node.detail     = i;
        }

        text.num++;
      }  
    }  

    if (!flag)
      break;
  }
}

void     HMapRender::Draw      ()
{
  D_Try
  {
    Camera&  eye      = mCamera;

    mMap.SetPos    (eye.pos);

    glPushAttrib   (GL_ENABLE_BIT);
    glDisable      (GL_LIGHTING);
    glEnable       (GL_TEXTURE_2D);
    glEnable       (GL_COLOR_MATERIAL);
    glPushMatrix   ();
    glScalef       (1.0f/mMap.mKx,1.0f/mMap.mKy,1.0f/mMap.mKz);
    glColor3f      (1,1,1);

    CalcList       (eye);

    hmtile_t* tile = NULL;
    int       tw   = mMap.mTextTileWidth * mMap.mTextXLen,
              th   = mMap.mTextTileHeight * mMap.mTextYLen;
    float     kx   = 1.0f / float (mMap.mTextTileWidth),
              ky   = 1.0f / float (mMap.mTextTileHeight),
              ckx  = float (tw)   / float (mMap.mWidth),
              cky  = float (th)   / float (mMap.mHeight);

//    glTexEnvi    (GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_REPLACE); 
    glMatrixMode (GL_TEXTURE);

    for (int index=mTextFirst;index!=-1;index=mText [index].next)
    {
      mMatEnv->bind  (mMatReindex [index]);

      int xs  = (index % mMap.mTextXLen) * mMap.mTextTileWidth,
          ys  = (index / mMap.mTextXLen) * mMap.mTextTileHeight;  

      glTexParameteri  (GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
      glTexParameteri  (GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

      glPushMatrix ();
      glScalef     (kx,ky,1);
      glTranslatef (-xs,-ys,0);
      glScalef     (ckx,cky,1);

      for (tilenode_t* node = mText [index].first;node;node=node->next)
      {
        tile = node->tile;
        DrawTile (tile->x,tile->y,tile->sOffs,tile->eOffs,mMap.mTileWidth,mMap.mTileHeight,mLevel [node->detail]);

//////////test

        if (mLevel [node->detail] > 1 && mLevel [node->detail] < mLevel [0])
          DrawTile (tile->x,tile->y,tile->sOffs,tile->eOffs,mMap.mTileWidth,mMap.mTileHeight,1);

//////////
      }

      mText [index].first = NULL;
      mText [index].num   = 0;

      glPopMatrix ();
    }

    glMatrixMode (GL_MODELVIEW);

    mTilePos   = 0;
    mTextFirst = -1;

    glPopMatrix    ();
    glPopAttrib    ();

//    glTexEnvi    (GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_DECAL); 
  }
  D_Exc ("HMapRender: Error at Draw");
}

void     HMapRender::BuildMat  ()
{
  matdesc_t    desc;
  char         matName [] = "HMapTexture__";
  int          xlen  = mMap.mTextXLen,
               ylen  = mMap.mTextXLen;
               xlen  = xlen ? xlen : 1;
               ylen  = ylen ? ylen : 1;
  int          index = 0;
  matloader_t* ldr   = (matloader_t*)desc.text.diffuse;
  MATGROUP     group = -1;
  int          num   = xlen * ylen;
  int          len   = strlen (matName);

  strcpy (desc.name,matName);

  mMatReindex        = (MATERIAL*)alloc   (sizeof (MATERIAL) * num);
  mText              = (textnode_t*)alloc (sizeof (textnode_t) * num);

  if (!mMatReindex || !mText)
  {
    D_Message ("HMapRender: No enough memory for mat tables");
    return;
  }

  memset  (mText,0,num * sizeof (textnode_t));
  
  desc.userLoader [ME_DIFFUSE_MAP] = TRUE;
  ldr->loader                      = &mMap;
  ldr->index                       = 0;

  mMatReindex [0] = mMatEnv->CreateMat (desc,&group,RENDER_HM_TEXT_CACHE_COUNT);

  for (int y=0;y<ylen;y++)
    for (int x=0;x<xlen;x++,index++)
    {
      ldr->index = index;
      
      itoa (index,desc.name+len,10);

                 //exclude index = 0

      mMatReindex [index] = mMatEnv->CreateMat (desc,&group);
    }
}

void    HMapRender::SetDrawDepth (int max_depth)
{
  float dk    = float (max_depth) / float (mLevelCount);
  float level = max_depth;

  for (int i=0;i<mLevelCount;mLevel [i] = level < 1 ? 1 : level,i++,level-=dk);
}

HMapRender::HMapRender (Pool* pool,MainRender* mr,int draw_depth)
           : Render           (pool,mr),
             mMap             (pool,mr->GetLoader().level,mr->GetLandMap()),
             mLandMap         (*mr->GetLandMap ()), 
             mMatEnv          (mr->GetMatEnvX()),
             mMatReindex      (NULL), mText (NULL), mTileNode (NULL),
             mTilePos         (0), mTileNum (0), mTextFirst (-1),
             mLevel           (NULL)
{   
  mMap.SetViewDepth (RENDER_VIEW_DEPTH);
  
  mTileNode = (tilenode_t*)alloc (sizeof (tilenode_t) * mMap.mInfoWidth * mMap.mInfoHeight);
  mLevel    = (int*)alloc        (sizeof (int) * (mLevelCount = mMap.mTViewDepth + 1));

  if (!mTileNode || !mLevel)
  {
    D_Message ("HMapRender: No enough memory for sys data");
    return;
  }

  mTileNum = mMap.mInfoWidth * mMap.mInfoHeight;

  BuildMat     ();
  SetDrawDepth (draw_depth);
}

HMapRender::~HMapRender()
{   
  int  xlen  = mMap.mTextXLen,
       ylen  = mMap.mTextYLen;  
  int  num   = xlen * ylen;

  for (int i=0;i<num;mMatEnv->DelMat (mMatReindex [i++]));

  if (mMatReindex)          free (mMatReindex);
  if (mText)                free (mText);
  if (mTileNode)            free (mTileNode);
  if (mLevel)               free (mLevel);
}
