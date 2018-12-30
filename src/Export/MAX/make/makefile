CPP    = cl.exe
RSC    = rc.exe
MAXDIR = f:\program files\3dsmax5
SDKDIR = $(MAXDIR)\maxsdk
OUTDIR = debug

DEF_FILE = make\deffile.def

ALL : $(OUTDIR) "$(OUTDIR)\woodexp.dle"

CPPFLAGS = /nologo /GA /I"$(SDKDIR)\include" /D "WIN32" /D "_WINDOWS" /YX"pch.h" \
           /c /D"DEBUG" /Fo"$(OUTDIR)\\" /Fd"$(OUTDIR)\\" /Fp"$(OUTDIR)\pch.pch" 
RSCFLAGS = /l 0x409 /fo"$(OUTDIR)\woodexp.res" /i"$(SDKDIR)\include"
        
LINK       = link.exe
LINK_FLAGS = /nologo comctl32.lib kernel32.lib user32.lib gdi32.lib winspool.lib  \
             comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib \
             uuid.lib /LIBPATH:"$(SDKDIR)\lib" core.lib geom.lib gfx.lib \
             mesh.lib maxutil.lib helpsys.lib \
             /nologo /subsystem:windows /dll  \
             /incremental:no   \
             /nodefaultlib:"msvcirt.lib" \
             /nodefaultlib:"msvcrt.lib"  \
             /nodefaultlib:"msvcprt.lib" \
             /def:"$(DEF_FILE)" /out:"$(OUTDIR)\woodexp.dle"
CPP_PROJ   = $(CPP_PROJ) $(CPPFLAGS)

!include "make\list.vc"

"$(OUTDIR)":
    @if not exist $(OUTDIR) mkdir $(OUTDIR)  

"$(OUTDIR)\woodexp.dle" :  $(DEF_FILE) $(LINK_OBJS)
    link $(LINK_FLAGS) $(LINK_OBJS)

.c{$(OUTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(OUTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(OUTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.c{$(OUTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(OUTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(OUTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<
