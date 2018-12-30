# Microsoft Developer Studio Project File - Name="Ode" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=Ode - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "Ode.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Ode.mak" CFG="Ode - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Ode - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "Ode - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "Ode - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /D "RELEASE" /D "dSINGLE" /YX /FD /c
# ADD BASE RSC /l 0x419 /d "NDEBUG"
# ADD RSC /l 0x419 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"q:\lib\OdeSP.lib"

!ELSEIF  "$(CFG)" == "Ode - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /D "dSINGLE" /D "DEBUG" /YX /FD /GZ /c
# ADD BASE RSC /l 0x419 /d "_DEBUG"
# ADD RSC /l 0x419 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"q:\lib\OdeSPD.lib"

!ENDIF 

# Begin Target

# Name "Ode - Win32 Release"
# Name "Ode - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=..\..\src\Ode\array.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\Ode\collision_kernel.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\Ode\collision_space.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\Ode\collision_std.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\Ode\collision_transform.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\Ode\collision_util.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\Ode\error.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\Ode\fastdot.c
# End Source File
# Begin Source File

SOURCE=..\..\src\Ode\fastldlt.c
# End Source File
# Begin Source File

SOURCE=..\..\src\Ode\fastlsolve.c
# End Source File
# Begin Source File

SOURCE=..\..\src\Ode\fastltsolve.c
# End Source File
# Begin Source File

SOURCE=..\..\src\Ode\joint.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\Ode\lcp.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\Ode\mass.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\Ode\mat.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\Ode\matrix.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\Ode\memory.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\Ode\misc.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\Ode\obstack.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\Ode\ode.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\Ode\odemath.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\Ode\rotation.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\Ode\step.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\Ode\testing.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\Ode\timer.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=..\..\src\Ode\make\pch.h
# End Source File
# End Group
# End Target
# End Project
