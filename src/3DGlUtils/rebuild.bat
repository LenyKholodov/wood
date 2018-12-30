@if exist striper\debug\*.obj del  striper\debug\*.obj
@if exist striper_nv\debug\*.obj del  striper_nv\debug\*.obj
@if exist GLExtProc\debug\*.obj del  GLExtProc\debug\*.obj
@if exist GLExt\debug\*.obj del  GLExt\debug\*.obj
@call remake.bat
