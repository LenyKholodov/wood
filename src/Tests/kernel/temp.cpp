  else
  {
    mThreadId = GetCurrentThreadId ();
    mTotalThreadCount++;

//    D_Try
//    {
      if (!DuplicateHandle (GetCurrentProcess (),
                            GetCurrentThread (),
                            GetCurrentProcess (),
                            &mHandle,
                            0,
                            FALSE,
                            DUPLICATE_SAME_ACCESS
                           )
         )
        D_Message ("Thread: Error duplicate thread HANDLE "<<id());
//    }
//    D_Exception (EXCEPTION_EXECUTE_HANDLER)
//    {  
//    }
  }