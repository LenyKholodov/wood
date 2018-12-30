  char   buf [128];
  char*  ptr1;  

  if (ptr1=strchr (ptr,'\n'))
  {
    int size1 = long(ptr1-ptr);

    memcpy (buf,ptr,size1);
    memcpy (buf+size1+1,ptr1,size-size1-1);

    buf [size1]    = '\r';
    buf [size1+1]  = '\n';
    buf [size+1]   = '\0';   

    SendMessage(tout->wnd(),EM_REPLACESEL,0,(LPARAM)buf);    
  }
  else
  {
    memcpy (buf,ptr,size);
    buf [size] = 0;

    SendMessage(tout->wnd(),EM_REPLACESEL,0,(LPARAM)buf);
  }