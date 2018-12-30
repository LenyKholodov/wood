@rem @cl makefx.cpp /link /NODEFAULTLIB:"math3dd.lib" 
@rem @del makefx.obj
@rem @del makefx.map
@makefx.exe
@rem del runlog
@call ..\build\remake.bat
@rem @del makefx.exe
@del table.dat