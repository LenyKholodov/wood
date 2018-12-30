
ALL : dll.dll

CLEAN :
        -@erase "dll.obj"
        -@erase "DLL.dll"
        -@erase "DLL.lib"
        
LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /incremental:no /machine:I386 /def:"DLL.def" /out:"DLL.dll" /implib:"DLL.lib" 
DEF_FILE= DLL.def
LINK32_OBJS= DLL.obj

"DLL.dll" : $(DEF_FILE) $(LINK32_OBJS)
    link $(LINK32_FLAGS) $(LINK32_OBJS)


