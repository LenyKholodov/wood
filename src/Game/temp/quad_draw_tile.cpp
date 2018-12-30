void     HMapRender::DrawTile  (int x,int y,ulong s,ulong e,int w,int h,int levels)
{
  int    w1 = w>>1,
         h1 = h>>1;
  ulong  c  = (s+e)>>1;

  if (levels == 1 || w <= 2 || h <= 2)
  {
    glBegin (GL_TRIANGLE_FAN);
      NewVertex (x+w1,y+h1,mMap.mMap+c);
      NewVertex (x,y,mMap.mMap+s);
      NewVertex (x,y+h,mMap.mMap+e-w);
      NewVertex (x+w,y+h,mMap.mMap+e);
      NewVertex (x+w,y,mMap.mMap+s+w);
      NewVertex (x,y,mMap.mMap+s);
    glEnd   ();

    mPgCount += 4;
  }
  else 
  {
    DrawTile (x,    y,    s,    c,    w1, h1, levels-1);
    DrawTile (x+w1, y,    s+w1, c+w1, w1, h1, levels-1);
    DrawTile (x,    y+h1, c-w1, e-w1, w1, h1, levels-1);
    DrawTile (x+w1, y+h1, c,    e,    w1, h1, levels-1);
  }  
}