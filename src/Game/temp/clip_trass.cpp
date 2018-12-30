//Fixed ClipLine (Fixed& x1,Fixed& y1,Fixed x2,Fixed y2,Fixed width,Fixed height,Fixed len,Fixed& dirx,Fixed& diry)

inline int   OutCode  (Fixed x,Fixed y,Fixed width,Fixed height)
{
  int code = 0;

  if (x < 0)           code |= 1;
  if (y < 0)           code |= 2;
  if (x > width)       code |= 4;
  if (y > height)      code |= 8;

  return code;
}

inline Fixed ClipLine (Fixed& x1,Fixed& y1,Fixed x2,Fixed y2,Fixed width,Fixed height,Fixed len,Fixed& dx,Fixed& dy)
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
      dx = -dx;
      dy = -dy;
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
      x1       = width;
      y1      += mul (y2-y1,k);
      len      = len - mul (len,abs (k));
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
} 

      glColor3f (0,1,1);
      glBegin  (GL_LINES);
        glVertex2f ((float)(fixed2int (x))*step,(float)(fixed2int(y))*step);
        glVertex2f ((float)(fixed2int (x+mul (llen,dx)))*step,(float)(fixed2int(y+mul (llen,dy)))*step);
      glEnd    ();

      x += mul (llen,dx); y += mul (llen,dy);

      {
        glColor3f (1,1,1);
        glBegin (GL_LINE_LOOP);
          glVertex2f ((float)(fixed2int (x))*step,(float)(fixed2int(y))*step);
          glVertex2f ((float)(fixed2int (x+FX_ONE))*step,(float)(fixed2int(y))*step);
          glVertex2f ((float)(fixed2int (x+FX_ONE))*step,(float)(fixed2int(y+FX_ONE))*step);
          glVertex2f ((float)(fixed2int (x))*step,(float)(fixed2int(y+FX_ONE))*step);
        glEnd   ();        
      }      