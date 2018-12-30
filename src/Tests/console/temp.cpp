int    ConsoleBase::printf    (int line,const char* _msg)
{
  D_Try
  {
    if (line >= mHeight)
      return 1;

    char* msg = (char*)_msg;
    int   len = strlen (msg);

    if (len <= mWidth)
    {
      mGrConsole->writeln (line,msg);
      return 1;
    }  
    else
    {    
      char* str   = msg;
      int   pos   = 0,
            first = 0;

      for (int i=0;i<len;pos++)
      { 
        char* lineStr = str;

        for (int j=0;j<mWidth;str++,i++,j++)
          if (*str == ' ')
            first = j;
      
        if (mWidth - first > (mWidth>>2))
        {
          char old         = lineStr [mWidth];
          lineStr [mWidth] = 0;
        
          mGrConsole->writeln (line+pos,lineStr);
          lineStr [mWidth] = old;
        }     
        else 
        {
          char old        = lineStr [first];
          lineStr [first] = 0;
        
          mGrConsole->writeln (line+pos,lineStr);
          lineStr [first] = old;          

          str = lineStr + first;
        }        

        for (;*str == ' ';str++,i++);
      }

      return pos;
    }
  }
  D_ExcRet ("ConsoleBase: Error at printf",1);
}


void     CommandLine::clear        ()
{
  if (mCursor)
    mBuf [--mPos] = 0;
}

void     CommandLine::restore      ()
{
  if (mCursor)
  {
    mBuf [mPos++] = '_';
    mBuf [mPos]   = 0;
  }
}