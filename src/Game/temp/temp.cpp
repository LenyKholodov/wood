  for (int i=0;i<mFCount;i++)
  {
    facet_t& facet  = mFacet [i];
    int      index [3] = {-1};

    for (int j=0;j<3;j++)
    {
      int   tileX = (mVertex [facet.index [j]].x - box.min.x) / step;
      int   tileY = (mVertex [facet.index [j]].z - box.min.z) / step;
      int   pos   = tileY * mWidth + tileX;

      if (index [0] != pos && index [1] != pos && index [2] != pos)
        mNode [pos].fring.insert (facet);
        
      index [j] = pos;
    }
  }


static int mod3 [6] = {0,1,2,0,1,2};

void    GrMapConv::calcCross  (const facet_t& facet,line_t* line,const boundbox_t& box)
{
  const vertex_t v [3] = { mVertex [facet.index[0]]-box.min,
                           mVertex [facet.index[1]]-box.min,
                           mVertex [facet.index[2]]-box.min
                          };
  for (int i=0;i<3;i++)
  {   
    const vector3d_t d = v [i] - v [mod3 [i+1]];

    line [i].a = d.z;
    line [i].b = d.x;
    line [i].c = -(v [i].x * d.z + v [i].z * d.x);
  }                          

  vector3d_t c = (v [0] + v [1] + v [2]) / 3.0f;

  if (line [0].a * c.x + line [0].b * c.z + line [0].c > 0)
  {
    for (int i=0;i<3;i++)
    {
      line [i].a = -line [i].a;
      line [i].b = -line [i].b;
      line [i].c = -line [i].c;
    }  
  }
}

BOOL    GrMapConv::checkCross (const line_t* line,float x1,float y1,float x2,float y2)
{
  for (int i=0;i<3;i++) 
  {
    if (x1 * line [i].a + y1 * line [i].b + line [i].c < 0) continue;
    if (x2 * line [i].a + y1 * line [i].b + line [i].c < 0) continue;
    if (x1 * line [i].a + y2 * line [i].b + line [i].c < 0) continue;
    if (x2 * line [i].a + y2 * line [i].b + line [i].c < 0) continue;

    return FALSE;
  }

  return TRUE;
}


void    GrMapConv::buildPart  (float step,const boundbox_t& box)
{
  for (int i=0;i<mFCount;i++)
  {
    float  v [4];
    int    x1,y1,x2,y2;
    line_t line [3];

    calcBound (mFacet [i],v [0],v [1],v [2],v [3]);    
    calcCross (mFacet [i],line,box);

    x1  = (v [0] - box.min.x) / step;
    y1  = (v [1] - box.min.z) / step;
    x2  = (v [2] - box.min.x) / step;
    y2  = (v [3] - box.min.z) / step;

    if (x1 == x2 && y1 == y2)    
      mNode [y1*mWidth+x1].fring.insert (mFacet [i]);
    else
    {
      float  xmin = float(x1) * step,
             xmax = float(x2+1) * step,
             ymin = float(y1) * step,
             ymax = float(y2+1) * step;

      for (;y1<=y2;y1++,ymin+=step,ymax+=step)      
      {
        int   pos   = y1 * mWidth + x1;
        float _xmin = xmin,
              _xmax = xmax;

        for (int x=x1;x<=x2;x++,_xmin+=step,_xmax+=step,pos++)
          if (checkCross (line,_xmin,ymin,_xmax,ymax))
            mNode [pos].fring.insert (mFacet [i]);
      }  
    }
  }
}

void    GrMapConv::calcBound  (const facet_t& facet,float& x1,float& y1,float& x2,float& y2)
{
  const vertex_t v [3] = {mVertex [facet.index[0]],
                           mVertex [facet.index[1]],
                           mVertex [facet.index[2]]
                          };

  vector3d_t c    = (v [0] + v [1] + v [2]) / 3.0f;
  vector3d_t dim  (0);

  for (int i=0;i<3;i++)
  {
    vector3d_t d = abs (c - v [i]);

    if (d.x > dim.x) dim.x = d.x;
    if (d.z > dim.z) dim.z = d.z;
  }

  x1   = c.x - dim.x;
  y1   = c.z - dim.z;
  x2   = c.x + dim.x;
  y2   = c.z + dim.z;
}

void     Draw     (HDC hdc)
{
  reset ();        

  glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glPushMatrix ();

  gluLookAt    (eye.pos().x,eye.pos().y,eye.pos().z,
                eye.dir().x+eye.pos().x,eye.dir().y+eye.pos().y,eye.dir().z+eye.pos().z,
                0,1,0
               );

  frustum_t frust;
  vertex_t* vertex = lev->mGraphMap->vertex;
      
  CalcGLFrustum     (frust);

  for (int i=0;i<size;i++)
    if (IsInBound (lev->mGraphMap->map [i].box,6,frust.clip))
    {
      grnode_t&     node   = lev->mGraphCache->index (i);
      facetpack_t*  facet  = node.facet;

//      glBegin (GL_TRIANGLES);

      for (int k=0;k<node.count;facet++,k++)
      {
        if (!check (facet->index))  
        {
         if (facet->facet.texture != -1)
         { 
           glColor3fv                  ((GLfloat*)&lev->mTextEnv->GetMaterial (facet->facet.texture).diffuse);
//           lev->mTextEnv->bind         (facet->facet.texture);
         }    

         glBegin (GL_TRIANGLES);
         glNormal3fv   ((GLfloat*)&facet->facet.n);

         for (int j=0;j<3;j++)
         {                
           glVertex3fv   ((GLfloat*)(vertex + facet->facet.index [j]));
           glTexCoord2fv ((GLfloat*)&facet->facet.tindex [j]);
         }            
         glEnd ();

         set (facet->index);
        }

//        glEnd ();
      }
    }  

  glPopMatrix ();

  glFinish ();

  SwapBuffers (hdc);    
}

/*    offs.grMapOffs   = sizeof (hdr) + sizeof (woodoffs_t);
    offs.grCacheOffs = sizeof (hdr) + sizeof (woodoffs_t) +
                       sizeof (grmapheader_t) +
                       sizeof (vertex_t) * mVCount +
                       sizeof (grtile_t) * mSize;
    offs.textEnvOffs = sizeof (hdr) + sizeof (woodoffs_t) +
                       sizeof (grmapheader_t) + 
                       sizeof (vertex_t) * mVCount +
                       sizeof (grtile_t) * mSize +
                       sizeof (grcacheheader_t) +
                       (sizeof (grnode_t) + fSize * sizeof (facetpack_t)) * mSize;*/


/*void _Line(int xs,int ys,int xe,int ye,int level)
{
  InsertNode (xs,ys,level);
  int dx=xe-xs;
  int dy=abs(ye-ys);
  if(dy<=dx)
  {
    int d,d1,d2=(dy-dx)<<1;
    d=(d1=dy<<1)-dx;
    if(ye>ys)
      for(;dx--;InsertNode(++xs,ys,level))
      {
        if(d>0)
        {
          d+=d2;
          ys++;
        }
        else
          d+=d1;
      }
    else
      for(;dx--;InsertNode(++xs,ys,level))
      {
              if(d>0)
        {
          d+=d2;
          ys--;
        }
        else
          d+=d1;
      }
  }
  else
  {
    int d,d1,d2=(dx-dy)<<1;
    d=(d1=dx<<1)-dy;
    if(ye>ys)
      for(;dy--;InsertNode(xs,++ys,level))
      {
        if(d>0)
        {
          d+=d2;
          xs++;
        }
        else
          d+=d1;
      }
    else
      for(;dy--;InsertNode(xs,--ys,level))
      {
        if(d>0)
        {
          d+=d2;
          xs++;
        }
        else
          d+=d1;
      }
  }
} */

void    _Line ( int x1, int y1, int x2, int y2, int color )
{
        int     dx = abs ( x2 - x1 );
        int     dy = abs ( y2 - y1 );
        int     sx = x2 >= x1 ? 1 : -1;
        int     sy = y2 >= y1 ? 1 : -1;

        if ( dy <= dx )
        {
                int     d  = ( dy << 1 ) - dx;
                int     d1 = dy << 1;
                int     d2 = ( dy - dx ) << 1;

                InsertNode ( x1, y1, color );

                for ( int x = x1 + sx, y = y1, i = 1; i <= dx; i++, x += sx )
                {
                        if ( d > 0 )
                        {
                                d += d2;
                                y += sy;
                        }
                        else
                                d += d1;

                        InsertNode ( x, y, color );
                }
        }
        else
        {
                int     d  = ( dx << 1 ) - dy;
                int     d1 = dx << 1;
                int     d2 = ( dx - dy ) << 1;

                InsertNode ( x1, y1, color );

                for ( int x = x1, y = y1 + sy, i = 1; i <= dy; i++, y += sy )
                {
                        if ( d > 0 )
                        {
                                d += d2;
                                x += sx;
                        }
                        else
                                d += d1;

                        InsertNode ( x, y, color );
                }
        }
}

void Line(int xs,int ys,int xe,int ye,int level)
{
        if(xs>xe)
        {
                swap(xs,xe);
                swap(ys,ye);
        }
        if(xs>=mapWidth || xe<0)
                return;
        if(xs<0)
        {
                ys+=(long)(ye-ys)*(-xs)/(xe-xs);
                xs=0;
        }
        if(xe>=mapWidth)
        {
                ye+=(long)(ye-ys)*(mapWidth-1-xe)/(xe-xs);
                xe=mapWidth-1;

        }
        if(ys<0)
        {
                if(ye<0) return;
                xs+=(long)(xe-xs)*(-ys)/(ye-ys);
                if(xs>xe) return;
                ys=0;               
        }
        else
                if(ys>=mapHeight)
                {
                        if(ye>=mapHeight) return;
                        xs+=(long)(xe-xs)*(mapHeight-1-ys)/(ye-ys);
                        if(xs>xe)         return;
                        ys=mapHeight-1;
                }
        if(ye<0)
        {
                xe+=(long)(xe-xs)*(-ye)/(ye-ys);
                ye=0;
        }
        else
                if(ye>=mapHeight)
                {
                        xe+=(long)(xe-xs)*(mapHeight-1-ye)/(ye-ys);
                        ye=mapHeight-1;
                }

        _Line (xs,ys,xe,ye,level);
}

    if (x1 > x2)
    {
      swap(x1,x2);
      swap(y1,y2);
    }
    if (x1 >= mapWidth || x2 < 0)   continue;
    if (x1 <  0)
    {
      float k  = x2/(x2-x1);
      if (fabs(k) > 1) dout<<"!1"<<endl;
      y1      += (y2-y1)*k;
      length  *= fabs (k);   
      x1       = 0;
    }
    if (x2 >= mapWidth)
    {
      float k  = (x2-float(mapWidth-1))/(x2-x1);
      if (fabs(k) > 1) dout<<"!2"<<endl;
      y2      -= (y2-y1)*k;
      length  *= fabs (k);   
      x2       = mapWidth-1;
    }
    if (y1 < 0)
    {
      if (y2 < 0) continue;

      float k  = -y1/(y2-y1);
      if (fabs(k) > 1) dout<<"!3"<<endl;
      x1      += (x2-x1)*k;
      length  *= fabs (k);   
      y1       = 0;               

      if (x1 > x2) continue;
    }
    else if (y1 >= mapHeight)
    {
      if (y2 >= mapHeight) continue;

      float k  = (float(mapHeight-1)-y2)/(y1-y2);
      if (fabs(k) > 1) dout<<"!4"<<endl;
      x1      -= (x1-x2)*k;
      length  *= fabs (k);   
      y1       = mapHeight-1;

      if (x1 > x2) continue;
    }
    if (y2 < 0)
    {
      float k  = -y2/(y1-y2);
      if (fabs(k) > 1) dout<<"!5"<<endl;
      x2      += (x1-x2)*k;
      length  *= fabs (k);   
      y2       = 0;
    }
    else if (y2 >= mapHeight)
    {
      float k  = (float(mapHeight-1)-y1)/(y2-y1);      
      if (fabs(k) > 1) dout<<"!6"<<endl;
      length  *= fabs (k);   
      x1      -= (x2-x1)*k;
      y2       = mapHeight-1;
    }       


/*    if (x1 > x2)
    {
      swap(x1,x2);
      swap(y1,y2);
    }

    if (x1 >= mapWidth || x2 < 0) continue;

    if (x1 < 0)
    {
      float k   = (-x1)/(x2-x1);
      y1       += (y2-y1)*k;
      length   *= fabs (k);
      x1        = 0;
    }

    if (x2 >= mapWidth)
    {
      float k   = (float(mapWidth-1)-x2)/(x2-x1);
      y2       += (y2-y1)*k;
      x2        = mapWidth-1;
      length   *= fabs (k);
    }

    if (y1 < 0)
    {
      if (y2 < 0) continue;

      float k   = (-y1)/(y2-y1);
      x1       += (x2-x1)*k;
      length   *= fabs (k);

      if (x1 > x2) continue;

      y1        = 0;               
    }
    else if (y1 >= mapHeight)
    {
      if (y2 >= mapHeight) continue;

      float k   = (float(mapHeight-1)-y1)/(y2-y1);
      x1       += (x2-x1)*k;
      length   *= fabs (k);

      if (x1 > x2) continue;

      y1 = mapHeight-1;
    }

    if (y2 < 0)
    {
      float k  = (-y2)/(y2-y1);
      x2      += (x2-x1)*k;
      y2       = 0;
      length  *= fabs (k);
    }
    else if (y2 >= mapHeight)
    {
      float k  = (float(mapHeight-1)-y2)/(y2-y1);
      x2      += (x2-x1)*k;
      length  *= fabs (k);
      y2       = mapHeight-1;
    }

    length = sqrt ((x2-x1)*(x2-x1)+(y2-y1)*(y2-y1));*/


  for (int i=0;i<depth;i++,x1+=dx1,y1+=dy1,x2+=dx2,y2+=dy2)
  {
    modf (x1,&xt1);
    modf (x2,&xt2);
    modf (y1,&yt1);
    modf (y2,&yt2);

    Line (xt1,yt1,xt2,yt2,i);

    for (int j=0;j<nodes [i].count;j++)
    {
      int inode = nodes [i].data [j];
      if (IsInBound (lev->mGraphMap->map [inode].box,6,frust.clip))
      {
        grnode_t&     node   = lev->mGraphCache->index (inode);
        facetpack_t*  facet  = node.facet;    

        glBegin (GL_TRIANGLES);

        for (int k=0;k<node.count;facet++,k++)
        {
//          glColor3f (float (inode) / float (mapWidth*mapHeight),
//                     float (inode) / float (mapWidth*mapHeight),
//                     float (inode) / float (mapWidth*mapHeight)
//                    );
          if (!check (facet->index))  
          {
           if (facet->facet.texture != -1)
             glColor3fv                  ((GLfloat*)&lev->mTextEnv->GetMaterial (facet->facet.texture).diffuse);

           glNormal3fv   ((GLfloat*)&facet->facet.n);
      
           for (int j=0;j<3;j++)
           {                
             glVertex3fv   ((GLfloat*)(vertex + facet->facet.index [j]));
             glTexCoord2fv ((GLfloat*)&facet->facet.tindex [j]);
           }            

           set (facet->index);
          }
        }  
        glEnd ();

      }
    }
  }

void     Draw     (HDC hdc)
{
  reset ();        
  ResetNodes ();

  glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glPushMatrix ();

  gluLookAt    (eye.pos().x,eye.pos().y,eye.pos().z,
                eye.dir().x+eye.pos().x,eye.dir().y+eye.pos().y,eye.dir().z+eye.pos().z,
                0,1,0
               );

  frustum_t frust;
  vertex_t* vertex = lev->mGraphMap->vertex;
      
  CalcGLFrustum     (frust);

  float dirx = eye.dir ().x,
        diry = eye.dir ().z;  
  float _div = 1.0f/sqrt (dirx*dirx + diry*diry);
  dirx      *= _div;
  diry      *= _div;
  float sx   = eye.pos ().x / step - dirx,
        sy   = eye.pos ().z / step - diry;
  float len  = 0;
  float dlen = 2.0f*sin (ANGLE);
  float dx1  = dirx*cos(ANGLE)-diry*sin(ANGLE),
        dy1  = dirx*sin(ANGLE)+diry*cos(ANGLE),
        dx2  = dirx*cos(-ANGLE)-diry*sin(-ANGLE),
        dy2  = dirx*sin(-ANGLE)+diry*cos(-ANGLE),
        x1   = sx,
        y1   = sy,
        x2   = sx,
        y2   = sy;
  float minh = -box.max.y; 

  int   depth= abs(DEPTH / (cos (ANGLE) * step)) + 2; 

  if (sx >= 0 && sy >= 0 && sx < mapWidth && sy < mapHeight)
    InsertNode (sx,sy,0);        

        //"<а_ RбрабRтки таc<а

  for (int i=0;i<depth;i++,x1+=dx1,y1+=dy1,x2+=dx2,y2+=dy2,len+=dlen)
  {
    float x     = x1,
          y     = y1;              
              
    for (float l=0;l<len;l+=1.0f,x+=diry,y-=dirx)        
    {
      if (x >= 0 && y >= 0 && x < mapWidth && y < mapHeight)
        InsertNode (x,y,i);        
    }  

    for (int j=0;j<nodes [i].count;j++)
    {
      int inode = nodes [i].data [j];
      if (IsInBound (lev->mGraphMap->map [inode].box,6,frust.clip))
      {
        grnode_t&     node   = lev->mGraphCache->index (inode);
        facetpack_t*  facet  = node.facet;    

        glBegin (GL_TRIANGLES);

        for (int k=0;k<node.count;facet++,k++)
        {
//          glColor3f (float (inode) / float (mapWidth*mapHeight),
//                     float (inode) / float (mapWidth*mapHeight),
//                     float (inode) / float (mapWidth*mapHeight)
//                    );
          if (!check (facet->index))  
          {
           if (facet->facet.texture != -1)
             glColor3fv                  ((GLfloat*)&lev->mTextEnv->GetMaterial (facet->facet.texture).diffuse);

           glNormal3fv   ((GLfloat*)&facet->facet.n);
      
           for (int j=0;j<3;j++)
           {                
             glVertex3fv   ((GLfloat*)(vertex + facet->facet.index [j]));
             glTexCoord2fv ((GLfloat*)&facet->facet.tindex [j]);
           }            

           set (facet->index);
          }
        }  
        glEnd ();

      }
    }
  }

  glPopMatrix ();

  glFinish ();

  SwapBuffers (hdc);    
}


  if (node.line [0].a != 0 && node.line [0].b != 0)
  {
    float _div       = 1.0f/sqrt (node.line [0].a*node.line [0].a+node.line [0].b*node.line [0].b);

    node.line [0].a *= _div;
    node.line [0].b *= _div;
    node.line [0].c *= _div;
  }
  else
    dout<<"NORMALIZE ERROR!"<<endl;

  if (node.line [1].a != 0 && node.line [1].b != 0)
  {
    float _div       = 1.0f/sqrt (node.line [1].a*node.line [1].a+node.line [1].b*node.line [1].b);

    node.line [1].a *= _div;
    node.line [1].b *= _div;
    node.line [1].c *= _div;
  }
  else
    dout<<"NORMALIZE ERROR!"<<endl;


  int err = 0;

  for (int i=0;i<2;i++)
  {
    if (box.min.x * node.line [i].a + box.min.z * node.line [i].b + node.line [i].c > EPS) 
    {
      dout<<(box.min.x * node.line [i].a + box.min.z * node.line [i].b + node.line [i].c)<<endl;
      dout<<"..."<<(int)code<<" "<<" 1 "<<i<<endl;;
      err = 1;
    }
    if (box.max.x * node.line [i].a + box.min.z * node.line [i].b + node.line [i].c > EPS) 
    {
      dout<<(box.max.x * node.line [i].a + box.min.z * node.line [i].b + node.line [i].c)<<endl;
      dout<<"..."<<(int)code<<" "<<" 2 "<<i<<endl;;
      err = 1;
    }  
    if (box.min.x * node.line [i].a + box.max.z * node.line [i].b + node.line [i].c > EPS) 
    {
      dout<<(box.min.x * node.line [i].a + box.max.z * node.line [i].b + node.line [i].c > EPS)<<endl;
      dout<<"..."<<(int)code<<" "<<" 3 "<<i<<endl;;
      err = 1;
    }  
    if (box.max.x * node.line [i].a + box.max.z * node.line [i].b + node.line [i].c > EPS) 
    {
      dout<<(box.max.x * node.line [i].a + box.max.z * node.line [i].b + node.line [i].c)<<endl;
      dout<<"..."<<(int)code<<" "<<" 4 "<<i<<endl;;
      err = 1;
    }
  }  

  if (!check (node,box))
  {
    dout<<":::ERROR! "<<(int)code<<" "<<dx1<<" "<<dy1<<" "<<dx2<<" "<<dy2<<endl;
    dout<<"Pos/Box "<<mPos.x<<" "<<mPos.z<<"   "<<box.min.x<<" "<<box.min.z<<" "
        <<box.max.x<<" "<<box.max.z<<endl;
//    dout<<"::::::"<<v [0][0]<<" "<<v[0][1]<<" "<<v[1][0]<<" "<<v[1][1]<<endl;

    for (int i=0;i<2;i++)
    {
      dout<<".."<<i<<" "<<node.line [i].a<<" "<<node.line [i].b<<" "<<node.line [i].c<<endl;
      dout<<"....."<<(box.min.x * node.line [i].a + box.min.z * node.line [i].b + node.line [i].c)<<endl;
      dout<<"....."<<(box.max.x * node.line [i].a + box.min.z * node.line [i].b + node.line [i].c)<<endl;
      dout<<"....."<<(box.min.x * node.line [i].a + box.max.z * node.line [i].b + node.line [i].c)<<endl;
      dout<<"....."<<(box.max.x * node.line [i].a + box.max.z * node.line [i].b + node.line [i].c)<<endl;
    }    
  }  

void    GrMapConv::calcCross  (int findex,float step)
{
  const facet_t& facet = mFacet [findex];
  const vertex_t v [3] = { mVertex [facet.index[0]],
                           mVertex [facet.index[1]],
                           mVertex [facet.index[2]]
                         };

  const point2d_t  p [3] = { {v [0].x / step, v [0].z / step},
                           {v [1].x / step, v [1].z / step},
                           {v [2].x / step, v [2].z / step}
                         };
  int  iMax  = 0,
       iMin  = 0,
       iMid  = 0;
  long dx1,dx2,x1,x2;

  if      (p[1].y < p[iMin].y)  iMin = 1;
  else if (p[1].y > p[iMax].y)  iMax = 1;
  if      (p[2].y < p[iMin].y)  iMin = 2;
  else if (p[2].y > p[iMax].y)  iMax = 2;

  iMid = mod3 [3 - iMin - iMax];

  x1 = x2 = p[iMin].x<<16;

  int ym=p[iMid].y;

  dx1 = frac0 (p[iMax].x - p[iMin].x,p[iMax].y - p[iMin].y);
  dx2 = frac0 (p[iMin].x - p[iMid].x,p[iMin].y - p[iMid].y);

  for(int i=p[iMin].y;i<=ym;i++)
  {
    int pos = i*mWidth + (x1>>16);

    for (int k=0;k<((x2-x1)>>16)+1;pos++,k++)
      mNode [pos].fring.insert (findex);

    x1 += dx1;
    x2 += dx2;
  }

  int dyxn;

  if((dyxn = p[iMax].y - p[iMid].y)!=0)
  {
    dx2 = fraction (p[iMax].x - p[iMid].x,dyxn);

    for(ym = p[iMax].y;i<=ym;i++)
    {
      x1 += dx1;
      x2 += dx2;

      int pos = i*mWidth + (x1>>16);

      for (int k=0;k<((x2-x1)>>16)+1;pos++,k++)
        mNode [pos].fring.insert (findex);
    }
  }
}

  int  iMax  = 0,
       iMin  = 0,
       iMid  = 0;

  if      (p[1].y < p[iMin].y)  iMin = 1;
  else if (p[1].y > p[iMax].y)  iMax = 1;
  if      (p[2].y < p[iMin].y)  iMin = 2;
  else if (p[2].y > p[iMax].y)  iMax = 2;

  iMid = mod3 [3 - iMin - iMax]; //??

  float dx01 = fabs (p [iMax].y - p [iMin].y) > EPS ? 
               (p [iMax].x-p [iMin].x)/(p [iMax].y-p [iMin].y) : 0.0f;
  float dx02 = fabs (p [iMin].y - p [iMid].y) > EPS ? 
               (p [iMid].x-p [iMin].x)/(p [iMid].y-p [iMin].y) : 0.0f;
  float dx21 = fabs (p [iMax].y - p [iMid].y) > EPS ? 
               (p [iMax].x-p [iMid].x)/(p [iMax].y-p [iMid].y) : 0.0f;
  float x1   = p [iMid].x,
        x2   = x1;

  for (float y = p [iMin].y;y<=p [iMid].y;y+=1.0f,x1+=dx01,x2+=dx02)
  {
    int pos = int(y)*mWidth + int(x1);

    D_Try
    {
      for (int x=x1;x<=x2;x++,pos++)
        mNode [pos].fring.insert (findex);
    }
    D_Exception (EXCEPTION_EXECUTE_HANDLER)
    {
      dout<<"ERROR!-1"<<endl;
    }
  }
 
  for (y = p [iMid].y + 1.0f;y<=p [iMax].y;y+=1.0f)
  {
    x1 += dx01;
    x2 += dx21;

    int pos = int(y)*mWidth + int(x1);

    D_Try
    {
      for (int x=x1;x<=x2;x++,pos++)
        mNode [pos].fring.insert (findex);
    }
    D_Exception (EXCEPTION_EXECUTE_HANDLER)
    {
      dout<<"ERROR!-2"<<endl;
    }
  }

void            HeightTree::process (htnode_t* tree,const boundbox_t& box)
{
  char   flag [4] = {0};
  int    ins      = FALSE;
  float  v [4][2] = {{box.min.x,box.min.z},
                     {box.max.x,box.min.z},
                     {box.min.x,box.max.z},
                     {box.max.x,box.max.z}
                    };

  for (int i=tree->list.first;i!=-1;i=mNode [i].list.next)
    if (check (mNode [i],box,flag))
      process (mNode + i,box);
  
  for (i=0;i<4;i++)
  {
//    dout<<"..."<<(int)flag [i]<<endl;
    if (!flag [i])
     if ((tree->line [0].a*v [i][0]+tree->line [0].b*v [i][1]+tree->line [0].c >= -EPS) &&
          (tree->line [1].a*v [i][0]+tree->line [1].b*v [i][1]+tree->line [1].c >= -EPS))
            ins = TRUE;
  }
//  dout<<endl;
    
  if (ins)// && check (*tree,box))
  {
    if (box.max.y - mPos.y >= tree->height) 
      mVisible = TRUE;
    if (box.min.y - mPos.y > tree->height && box.min.y - mPos.y >= 0)
      insert (*tree,box);
  }    
}

      int t [4] = {-1,-1}, 
          pos   = 0;

      dout<<"...."<<flag [0]<<" "<<flag [1]<<" "<<flag [2]<<" "<<flag [3]<<endl;

      if (flag [0] != -1 && flag [0] != t [0] && flag [0] != t [1]) t [pos++] = flag [0];
      if (flag [1] != -1 && flag [1] != t [0] && flag [1] != t [1]) t [pos++] = flag [1];
      if (flag [2] != -1 && flag [2] != t [0] && flag [2] != t [1]) t [pos++] = flag [2];
      if (flag [3] != -1 && flag [3] != t [0] && flag [3] != t [1]) t [pos++] = flag [3];

      dout<<pos<<" "<<t [0]<<" "<<t [1]<<endl;

                   //Сдвинуть индексы

      if (pos == 2)
      {
        int flag = FALSE;

        for (int k=0;k<2;k++)
          for (int l=0;l<2;l++)
            if (fabs (mNode [t [1]].line [k].a + mNode [t [2]].line [l].a) <= EPS &&
                fabs (mNode [t [1]].line [k].b + mNode [t [2]].line [l].b) <= EPS &&
                fabs (mNode [t [1]].line [k].c + mNode [t [2]].line [l].c) <= EPS)
                  flag = TRUE;

        if (!flag) ins = TRUE;          
      }
      else if (pos == 1)
      {
        int cnt [2] = {0};  
        int flag    = FALSE;    
        int edge    = 0;

        for (int i=0;i<2;i++)
        {
          if (box.min.x * mNode [t [1]].line [i].a + box.min.z * mNode [t [1]].line [i].b + mNode [t [1]].line [i].c >= -EPS) cnt [i]++;
          if (box.max.x * mNode [t [1]].line [i].a + box.min.z * mNode [t [1]].line [i].b + mNode [t [1]].line [i].c >= -EPS) cnt [i]++;
          if (box.min.x * mNode [t [1]].line [i].a + box.max.z * mNode [t [1]].line [i].b + mNode [t [1]].line [i].c >= -EPS) cnt [i]++;
          if (box.max.x * mNode [t [1]].line [i].a + box.max.z * mNode [t [1]].line [i].b + mNode [t [1]].line [i].c >= -EPS) cnt [i]++;
        }
                
        edge = cnt [0] == 4 ? 0 : 1;

        for (i=0;i<2;i++)
          if (fabs (mNode [t [1]].line [edge].a - tree->line [i].a) <= EPS &&
              fabs (mNode [t [1]].line [edge].b - tree->line [i].b) <= EPS &&
              fabs (mNode [t [1]].line [edge].c - tree->line [i].c) <= EPS)
                flag = TRUE;                                                     

        if (!flag) ins = TRUE;        
      }

void     Draw3D     (HDC hdc)
{  
  reset ();        
  ResetNodes ();

  glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glPushMatrix ();

  gluLookAt    (eye.pos().x,eye.pos().y,eye.pos().z,
                eye.dir().x+eye.pos().x,eye.dir().y+eye.pos().y,eye.dir().z+eye.pos().z,
                0,1,0
               );

  frustum_t frust;
  vertex_t* vertex = lev->mGraphMap->vertex;
      
  CalcGLFrustum     (frust);

  float dirx = eye.dir ().x,
        diry = eye.dir ().z;  
  float _div = 1.0f/sqrt (dirx*dirx + diry*diry);
  dirx      *= _div;
  diry      *= _div;
  float sx   = eye.pos ().x / step - dirx,
        sy   = eye.pos ().z / step - diry;
//  float sx   = eye.pos ().x / step,
//        sy   = eye.pos ().z / step;
  float len  = 0;
  float dlen = 2.0f*sin (ANGLE);
  float dx1  = dirx*cos(ANGLE)-diry*sin(ANGLE),
        dy1  = dirx*sin(ANGLE)+diry*cos(ANGLE),
        dx2  = dirx*cos(-ANGLE)-diry*sin(-ANGLE),
        dy2  = dirx*sin(-ANGLE)+diry*cos(-ANGLE),
        x1   = sx,
        y1   = sy,
        x2   = sx,
        y2   = sy;
  float minh = -box.max.y; 

  int   depth= abs(DEPTH / (cos (ANGLE) * step)) + 2; 

  if (sx >= 0 && sy >= 0 && sx < mapWidth && sy < mapHeight)
    InsertNode (sx,sy,0); 

  float delta [4] = {dx1*step,dy1*step,dx2*step,dy2*step};

//  htree->set  (vector3d_t (sx * step,eye.pos().y,sy * step),delta);
  htree->set  (eye.pos(),delta);

  int errIndex = int (sx)+int(sy)*mapWidth;

               //TILE flag

  int total = 0;

  for (int i=0;i<depth;i++,x1+=dx1,y1+=dy1,x2+=dx2,y2+=dy2,len+=dlen)
  {
    float x     = x1,
          y     = y1;              
              
    for (float l=0;l<len;l+=1.0f,x+=diry,y-=dirx)        
    {
      if (x >= 0 && y >= 0 && x < mapWidth && y < mapHeight)
        InsertNode (x,y,i);        
    }  

    for (int j=0;j<nodes [i].count;j++)
    {
      htree->insert (nodes [i].data [j],i,lev->mGraphMap->map [nodes [i].data [j]].box);
      total++;
    }  
  }



  if (mSpanPos)   
  {
    int prev = -1, next = mSpan [i].next;

    for (int i=0;i!=-1;prev=i,i=next,next=next!=-1?mNode [next].next:-1)
    {
      if (span.x2 <  mSpan [i].x1) continue;
      if (span.x1 <= mSpan [i].x1)
      {
      }
      else if (span.x1 <= mSpan [i].x2 && 
    }
  }
  else
  {
  }  

  line_t test;
  float  x,y;
  span_t span;

  test.a = -(mPos.z-seg [1+2]);
  test.b = mPos.x-seg [0+2];
  test.c = -(mPos.x*test.a+mPos.z*test.b);

  cross (test,x,y);

  span.x1    = (x-mPos.x) * mCos - (y-mPos.z) * mSin + (mSegSize>>1);

  dout<<span.x1<<endl;


////////////////////////////////////////////////////////////////////////////////
///Идентификаторы дополнительных переменных инициализации
////////////////////////////////////////////////////////////////////////////////

static uint       ___GameLevelDinId     = 0;
const  int        GAME_LEVEL_ID_COUNT   = 256;
class  GLIDSetParam { public: GLIDSetParam (uint,size_t,void*); };

#define  NEW_ID(NAME,TYPE,DEFAULT) static uint NAME ## _ID = ___GameLevelDinId++; \
                                   static TYPE NAME = DEFAULT; \
                                   static GLIDSetParam NAME ## TYPE (NAME ## _ID,sizeof (TYPE),&NAME);

////////////////////////////////////////////////////////////////////////////////

NEW_ID      (DEFAULT_LEVEL_POOL_SIZE,size_t,64*1024*1024); //16Mb
NEW_ID      (DEFAULT_GRCACHE_SIZE,int,16);
NEW_ID      (DEFAULT_GRMAP_FLAGS,uint,(uint)-1);
NEW_ID      (DEFAULT_TEXTURES_CACHE_SIZE,uint,4);
NEW_ID      (DEFAULT_LMR_MAT_NUM,uint,256);
NEW_ID      (DEFAULT_LMR_LIST_NUM,uint,256);

struct glevparam_t
{
  uchar         def;         
  size_t        size;
  void*         param;
};

glevparam_t  glevParam [GAME_LEVEL_ID_COUNT] = {0};

GLIDSetParam::GLIDSetParam (uint index,size_t size,void* param) 
{        
  if (index < GAME_LEVEL_ID_COUNT)
  {
    glevParam [index].def    = TRUE;  
    glevParam [index].size   = size;  
    glevParam [index].param  = param;
  }  
}

    va_list list;

    va_start (list,count);

    while (count--)
    {
      uint paramId = va_arg (list,uint);

      if (paramId >= GAME_LEVEL_ID_COUNT || !glevParam [paramId].def)
      {
        #ifdef DEBUG
          dout<<"GameLevel: Wrong param id: "<<paramId<<". Param search failed "<<endl;
        #endif
        break;
      }      
      
      memcpy (glevParam [paramId].param,list,glevParam [paramId].size);

//      dout<<glevParam [paramId].param<<" "<<&DEFAULT_LEVEL_POOL_SIZE<<endl;
    }

    va_end (list);


const  uint      LL_ALL                       = (uint)-1;
const  uint      LL_NULL                      = 0;
const  uint      LL_NUM                       = 32;

  public:
      LandscapeMap*             mLandMap;
      LandMeshRender*           mLandMeshRender; /////!!!!!!out
      TextEnvironment*          mTextures;      

  protected:
      BOOL                      LoadLevel (uint _Id);
      BOOL                      LoadLandscape ();
      BOOL                      LoadTextures  ();
      BOOL                      LoadLandMeshes ();

BOOL      GameLevel::LoadLandscape ()
{
  mLandMap = new LandscapeMap (M_GetPool(),
                               CreateLump (LL_LANDSCAPE),
                               DEFAULT_GRMAP_FLAGS,
                               DEFAULT_GRCACHE_SIZE
                              );

  return IsValid (mLandMap);
}

BOOL      GameLevel::LoadTextures  ()
{
  BinFile* file = CreateLump (LL_TEXTURES);

  mTextures = new TextEnvironment (M_GetPool(),file,DEFAULT_TEXTURES_CACHE_SIZE);

  delete file;

  return IsValid (mTextures);
}

BOOL      GameLevel::LoadLandMeshes ()
{
  mLandMeshRender = new LandMeshRender (M_GetPool(),
                                        CreateLump (LL_LAND_MESH),
                                        mLandMap,
                                        DEFAULT_LMR_MAT_NUM,
                                        4096,
                                        256
                                       ); 

  return IsValid (mLandMeshRender);
}

BOOL      GameLevel::LoadLevel (uint id)
{
  D_Try
  {
    if (id & LL_LANDSCAPE_BIT) 
      if (!LoadLandscape ())
        return FALSE;

    if (id & LL_TEXTURES_BIT) 
      if (!LoadTextures ())
        return FALSE;

    if (id & LL_LAND_MESH)
      if (!LoadLandMeshes ())
        return FALSE;

    return TRUE;
  }
  D_Exception (EXCEPTION_EXECUTE_HANDLER)
  {
    #ifdef DEBUG
      dout<<"GameLevel: Error at LoadLevel id: "<<id<<endl;
    #endif
    return FALSE;
  }
}

    if (mLandMeshRender) delete mLandMeshRender;
    if (mLandMap)  delete mLandMap;
    if (mTextures) delete mTextures;

    M_PushPool (mLevelPool);

    if (!LoadLevel (_Id))
    {
      #ifdef DEBUG
        dout<<"GameLevel: Error load level "<<this<<endl;
      #endif 
    }

    M_PopPool ();

    mLevelPool = new (pool()) FloatPool (pool(),DEFAULT_LEVEL_POOL_SIZE);

    if (!mLevelPool)
    {
      #ifdef DEBUG
        dout<<"GameLevel: No enough memory "<<this<<endl;
      #endif
      return;
    }

    if (mLevelPool)        delete mLevelPool;

    size_t cacheSize = float(cachePer)/100.0f*pool()->maxavail ();

    if (cacheSize < MIN_LR_MEM_LIMIT) cacheSize = MIN_LR_MEM_LIMIT;
    if (cacheSize > MAX_LR_MEM_LIMIT) cacheSize = MAX_LR_MEM_LIMIT;

    mCache  = new (pool()) LandscapeCache (pool(),
                                           _Level,
                                           cacheSize,
                                           mMap->cacheWidth,
                                           mMap->cacheHeight
                                          );

    if (!mCache)
    {
      #ifdef DEBUG
        dout<<"LandMeshRender: No enough memory (alloc cache) "<<endl;
      #endif
      return;
    }

//  me->ChangeProperties (mdl,MODEL_MULT_MATRIX,matrix_t::scale (vector3d_t (1,1.4,1)));
//  me->ChangeProperties (mdl,MODEL_CHANGE_MATRIX,trans);
//  me->SetPosition      (mdl,c + vector3d_t (12));

/*  glLineWidth           (1000);

  gluLookAt           (camera.pos().x,camera.pos ().y,camera.pos().z,
                       camera.dir().x+camera.pos().x,camera.dir().y+camera.pos ().y,camera.dir().z+camera.pos().z,
                       camera.up().x,camera.up().y,camera.up().z
                      );


  glColor3f (1,0,0); 

  glBegin (GL_LINES);
    glVertex3f (c.x,c.y,c.z);
    glVertex3f (c.x+100,c.y,c.z);
    glVertex3f (c.x,c.y,c.z);
    glVertex3f (c.x,c.y+100,c.z);
    glVertex3f (c.x,c.y,c.z);
    glVertex3f (c.x,c.y,c.z+100);
  glEnd   ();*/


void   GrMapConv::convert (Object3D::Node node,int* pos)
{  
  D_Try
  {  
    if (node.count (Object3D::FACET) > 0)
    {
      int      count   = node.count (Object3D::FACET);
      facet_t* facet   = node.facet ();
      facet_t* dest    = &mFacet [pos [0]];
      int      fvertex = pos [1];

      for (int i=0;i<count;i++,facet++,dest++)
      {
        *dest = *facet;

        for (int j=0;j<3;j++)
          dest->index [j] = facet->index [j] + fvertex;
      } 

      pos [0]  += count;
    }

    if (node.count (Object3D::NORMAL) > 0)
      memcpy (mNormal + pos [1],node.normal(),node.count (Object3D::NORMAL) * sizeof (vector3d_t));

    if (node.count (Object3D::VERTEX) > 0)        
    {
      memcpy (mVertex + pos [1],node.vertex(),node.count (Object3D::VERTEX) * sizeof (vertex_t));
      pos [1] += node.count (Object3D::VERTEX);
    }  

    for (Object3D::Node iter = node.node();iter.valid();iter = iter.next())
      convert (iter,pos);
  }
  D_Exception (EXCEPTION_EXECUTE_HANDLER)
  {
    #ifdef DEBUG
      dout<<"GrMapConv: Error at convert (Object3D::Node,int*) "<<this<<endl;
    #endif
  }
}

int     GrMapConv::buildMesh  (mesh_t* mesh,FacetRing& fring,size_t& meshSize,int& meshIndex)
{
  FacetRing::iterator iter      = fring;
  int                 count     = fring.count ();
  int                 mat       = 0;
  static int          material [0x10000];
  int                 meshOldIndex = meshIndex;

  if (count)
  {

    for (int i=0;i<count;i++,iter++)
    {
      for (int j=mat;j && mFacet [iter.data()].texture!=material [j-1];j--);

      if  (!j)
      {
        if (mat < 0x10000)     material [mat++] = mFacet [iter.data ()].texture;
        else    
          dout<<"GrMapConv: Too many materials in 1 mesh. Call to DadyCool "<<endl; 
      } 
    }  

    meshSize = 0;

    for (i=0;i<mat;i++)
    {
      facetpack_t* facet  = (facetpack_t*)mesh->data;
      uint         fcount = 0;
     
      for (int j=0;j<count;j++,iter++)
        if (mFacet [iter.data ()].texture == material [i])
        {
          memcpy (&facet->facet,mFacet + iter.data (),sizeof (facet_t));
          facet->index = iter.data ();
          fcount++;
          facet++;
        }  

      mesh->material  = material [i];
      mesh->index     = meshIndex++;
      mesh->count     = fcount;
      mesh->size      = sizeof (mesh_t) + sizeof (facetpack_t) * fcount;

      meshSize       += mesh->size;

      mesh            = (mesh_t*)((char*)mesh+mesh->size);
    }
  }
  else
  {
    mesh->material  = -1;
    mesh->index     = -1;
    mesh->count     = 0;
    mesh->size      = sizeof (mesh_t);

    meshSize       += mesh->size;    
  }

  return meshIndex - meshOldIndex;
}

void    GrMapConv::calcSize  (int& fSize,int& matSize)
{
  fSize   = 0;
  matSize = 0;

  for (int i=0;i<mSize;i++)
  {
    if (mNode [i].fring.count () > fSize) fSize = mNode [i].fring.count ();

    FacetRing::iterator iter      = mNode [i].fring;
    int                 count     = mNode [i].fring.count ();
    int                 mat       = 0;
    static int          material [0x10000];

    for (int i=0;i<count;i++,iter++)
    {
      for (int j=mat;j && mFacet [iter.data()].texture!=material [j-1];j--);

      if  (!j)
      {
        if (mat < 0x10000)     material [mat++] = mFacet [iter.data ()].texture;
        else    
          dout<<"GrMapConv: Too many materials in 1 mesh. Call to DadyCool "<<endl; 
      } 
    }  
    
    if (matSize < mat) matSize = mat;
  }  
}

void    LandMeshRender::DrawMesh (mesh_t* mesh)
{
  D_Try
  {
    vector3d_t*    normal = mMap->normal;
    vertex_t*      vertex = mMap->vertex;

    if (normal)
    {
      int           count = mesh->count;
      facetpack_t*  facet = (facetpack_t*)mesh->data;

      for (int k=0;k<count;k++,facet++)
        if (!mFlag [facet->index])
        {
          glTexCoord2fv  ((GLfloat*)&facet->facet.tindex [0]);
          glArrayElement (facet->facet.index [0]);

          glTexCoord2fv  ((GLfloat*)&facet->facet.tindex [1]);
          glArrayElement (facet->facet.index [1]);

          glTexCoord2fv  ((GLfloat*)&facet->facet.tindex [2]);
          glArrayElement (facet->facet.index [2]);

          mFlag.set (facet->index);  

          mPgCount++;
        } //else add to blend
    }   
    else
    {
      int           count = mesh->count;
      facetpack_t*  facet = (facetpack_t*)mesh->data;

      for (int k=0;k<count;k++,facet++)
        if (!mFlag [facet->index])
        {
          glNormal3fv    ((GLfloat*)&facet->facet.n);

          glTexCoord2fv  ((GLfloat*)&facet->facet.tindex [0]);
          glArrayElement (facet->facet.index [0]);

          glTexCoord2fv  ((GLfloat*)&facet->facet.tindex [1]);
          glArrayElement (facet->facet.index [1]);

          glTexCoord2fv  ((GLfloat*)&facet->facet.tindex [2]);
          glArrayElement (facet->facet.index [2]);

          mPgCount++;

          mFlag.set (facet->index);
        } 
    }
  }
  D_Exception (EXCEPTION_EXECUTE_HANDLER)
  {
    #ifdef DEBUG
      dout<<"LandMeshRender: Error at DrawMesh: "<<mesh->count<<endl;
    #endif
  }
}