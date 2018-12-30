#include <pch.h>
#include "export.h"

Pool* BaseExport::mExportPool = NULL;

void     BaseExport::OpenFrame (const char* tag)
{
  dump.OpenFrame (tag);
  mIndent++;
}

bool   BaseExport::CloseFrame ()
{
  if (mOwner)
  {
    if (mIndent >= mOwner->mIndent + 1)
    {
      mIndent--;
      return dump.CloseFrame ();
    }
    else return false;    
  }
  else if (mIndent)
  {
    mIndent--;
    return dump.CloseFrame ();
  }
  else return false;
}

BaseExport::BaseExport  (FrameWriter& os,ExportInfo& i)
           : dump    (os),
             mIndent (0),
             info    (i),
             mOwner  (NULL)
{   }

BaseExport::BaseExport  (const char* name,BaseExport& owner)
           : dump     (owner.dump),
             mIndent  (owner.mIndent),
             info     (owner.info),
             mOwner   (&owner)
{   
  if (name)  
    OpenFrame (name);
}

BaseExport::~BaseExport ()
{   
  int count = 0;

  if (mOwner) count = mIndent - mOwner->mIndent;
  else        count = mIndent;

  for (int i=0;i<count;i++)
    dump.CloseFrame ();
}
