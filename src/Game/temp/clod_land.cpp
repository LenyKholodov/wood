const    int    DV      = 150;
const    int    DH      = 500;

void     HMLandscapeRender::Draw      ()
{
  glPushAttrib   (GL_ENABLE_BIT);

  glDisable      (GL_CULL_FACE);
//  glDisable      (GL_TEXTURE_2D);
//  glPolygonMode  (GL_FRONT_AND_BACK,GL_LINE);
  glPushMatrix   ();
  glScalef       (1,0.1,1);
  glColor3f      (1,1,1);
  glDisable      (GL_LIGHTING);
  glTexEnvi      (GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_REPLACE);

  mainrender()->GetMatEnvX()->bind (mMat);
 
  LandscapeMap* mMap = mainrender()->GetLandMap();
  const Camera& eye  = mCamera;

  float    kx       = mMap->step*float(mMap->width)/float (HM_WIDTH),
           ky       = mMap->step*float(mMap->height)/float (HM_WIDTH);
  float    step     = mMap->step;
  int      mapWidth = mMap->width, 
           mapHeight= mMap->height;
  float    fldirx   = eye.dir ().x,
           fldiry   = eye.dir ().z;  
  float    _div     = 1.0f/sqrt (fldirx*fldirx + fldiry*fldiry);
  float    fl_angle = RENDER_VIEW_ANGLE;
  Angle    fx_angle = float2angle (fl_angle);
//  int      depth    = abs(RENDER_VIEW_DEPTH / (cos (fl_angle) * step)); 
  float    mdepth   = sqrt (sqr (mMap->width*step) + sqr (mMap->height*step));
  float    hmdepth  = sqrt (2.0f * sqr (float (HM_WIDTH)));
  float    dvstep   = RENDER_VIEW_DEPTH/mdepth;
  dvstep            = dvstep > 1 ? hmdepth/float (DV) : hmdepth/float (DV) * dvstep;
  float    dhstep   = (2.0f * dvstep * tan (RENDER_VIEW_ANGLE)) / float (DH);    
  dvstep           /= 128.0f;

  fldirx           *= _div;
  fldiry           *= _div;

  Fixed    dirx     = float2fixed (fldirx * dvstep),
           diry     = float2fixed (fldiry * dvstep);      
  Fixed    dhx      = float2fixed (fldiry * dhstep),
           dhy      = float2fixed (-fldirx * dhstep);
  Fixed    _dhx     = dhx,
           _dhy     = dhy;

  Fixed    sx       = float2fixed (eye.pos ().x * float (HM_WIDTH) / (step * float (mMap->width))),
           sy       = float2fixed (eye.pos ().z * float (HM_WIDTH) / (step * float (mMap->height)));

  Fixed    dx1      = mul (dirx,cos(fx_angle)) - mul (diry,sin(fx_angle)),
           dy1      = mul (dirx,sin(fx_angle)) + mul (diry,cos(fx_angle)),
           x1       = sx+dx1,
           y1       = sy+dy1,
           _dx1     = dx1,
           _dy1     = dy1;
  Fixed    w        = int2fixed (HM_WIDTH),
           h        = int2fixed (HM_WIDTH);

  struct xxx{
    int  count;
    vertex_t table [DH];
    textcoord_t tcoord [DH];

    xxx ():count (0) {}
  };

  static xxx vrtx [DV];

//  for (int i=0;i<DV;i++,x1+=dx1,y1+=dy1,dhx+=_dhx,dhy+=_dhy,dx1+=_dx1,dy1+=_dy1)
  for (int i=0;i<DV;i++,x1+=dx1,y1+=dy1,dhx+=_dhx,dhy+=_dhy,dx1+=_dx1,dy1+=_dy1)
  {
    Fixed x     = x1,
          y     = y1;              
    BOOL  flag  = FALSE;

    vrtx [i].count = 0;

    for (int j=0;j<DH;j++,x+=dhx,y+=dhy)
    {
      uint height = 0;

      if (x >= 0 && y >= 0 && x < w-FX_ONE && y < h-FX_ONE)
      {
        int i   = fixed2int(x),
            j   = fixed2int(y);
        height  = (mHeightMap [j][i] + mHeightMap [j][i+1] +
                   mHeightMap [j+1][i] + mHeightMap [j+1][i+1]
                  )>>2;
        flag    = TRUE;        
      }      

      vertex_t&    v = vrtx [i].table [vrtx [i].count++];
      textcoord_t& t = vrtx [i].tcoord [vrtx [i].count-1];
         
      v.x         = fixed2float (x) * kx;
      v.z         = fixed2float (y) * ky;
      v.y         = height;

      t.u         = fixed2float (div (y,w));
      t.v         = fixed2float (div (x,w));
    }             

    if (!flag)
      break;
  }

  int depth_count = i;

  glBegin (GL_TRIANGLES);

  for (i=0;i<depth_count;i++)
  {
    for (int j=0;j<DH-1;j++)
    {
      glTexCoord2fv ((GLfloat*)&vrtx [i].tcoord [j]);
      glVertex3fv   ((GLfloat*)&vrtx [i].table [j]);

      glTexCoord2fv ((GLfloat*)&vrtx [i+1].tcoord [j]);
      glVertex3fv   ((GLfloat*)&vrtx [i+1].table [j]);

      glTexCoord2fv ((GLfloat*)&vrtx [i+1].tcoord [j+1]);
      glVertex3fv   ((GLfloat*)&vrtx [i+1].table [j+1]);

      glTexCoord2fv ((GLfloat*)&vrtx [i+1].tcoord [j+1]);
      glVertex3fv   ((GLfloat*)&vrtx [i+1].table [j+1]);

      glTexCoord2fv ((GLfloat*)&vrtx [i].tcoord [j+1]);
      glVertex3fv   ((GLfloat*)&vrtx [i].table [j+1]);

      glTexCoord2fv ((GLfloat*)&vrtx [i].tcoord [j]);
      glVertex3fv   ((GLfloat*)&vrtx [i].table [j]);
    }  
  }

  glEnd ();

  glPopMatrix    ();
  glPolygonMode  (GL_FRONT_AND_BACK,GL_FILL);
  glPopAttrib    ();
}
