# Microsoft Developer Studio Project File - Name="lib_mm_snprintf" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=lib_mm_snprintf - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "lib_mm_snprintf.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "lib_mm_snprintf.mak" CFG="lib_mm_snprintf - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "lib_mm_snprintf - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "lib_mm_snprintf - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "lib_mm_snprintf - Win32 Release"

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
# ADD CPP /nologo /MT /W3 /GX /O2 /I "../../libsrc/msvc" /I "../../libsrc/include" /I "../../libsrc" /D "NDEBUG" /D "WIN32" /D "_LIB" /D DLLEXPORT_mmsnprintf=__declspec(dllexport) /YX /FD /c
# ADD BASE RSC /l 0x804 /d "NDEBUG"
# ADD RSC /l 0x804 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"mmsnprintf.lib"

!ELSEIF  "$(CFG)" == "lib_mm_snprintf - Win32 Debug"

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
# ADD CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /I "../../libsrc/msvc" /I "../../libsrc/include" /I "../../libsrc" /D "_DEBUG" /D "WIN32" /D "_LIB" /D DLLEXPORT_mmsnprintf=__declspec(dllexport) /YX /FD /GZ /c
# ADD BASE RSC /l 0x804 /d "_DEBUG"
# ADD RSC /l 0x804 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"mmsnprintf_D.lib"

!ENDIF 

# Begin Target

# Name "lib_mm_snprintf - Win32 Release"
# Name "lib_mm_snprintf - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=..\..\libsrc\internal.cpp
# End Source File
# Begin Source File

SOURCE=..\..\libsrc\msvc\mm_psfunc.cpp
# End Source File
# Begin Source File

SOURCE=..\..\libsrc\mm_snprintf.cpp

!IF  "$(CFG)" == "lib_mm_snprintf - Win32 Release"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\libsrc\mm_snprintf.cpp

BuildCmds= \
	copy $(InputPath) .\mm_snprintfA.cpp \
	copy $(InputPath) .\mm_snprintfW.cpp \
	

".\mm_snprintfA.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

".\mm_snprintfW.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "lib_mm_snprintf - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\libsrc\mm_snprintf.cpp

BuildCmds= \
	copy $(InputPath) .\mm_snprintfA.cpp \
	copy $(InputPath) .\mm_snprintfW.cpp \
	

".\mm_snprintfA.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

".\mm_snprintfW.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\mm_snprintfA.cpp
# End Source File
# Begin Source File

SOURCE=.\mm_snprintfW.cpp

!IF  "$(CFG)" == "lib_mm_snprintf - Win32 Release"

# ADD CPP /D "_UNICODE" /D "UNICODE"

!ELSEIF  "$(CFG)" == "lib_mm_snprintf - Win32 Debug"

# ADD CPP /D "_UNICODE" /D "UNICODE"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\libsrc\oldname.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=..\..\libsrc\internal.h
# End Source File
# Begin Source File

SOURCE=..\..\libsrc\mm_psfunc.h
# End Source File
# Begin Source File

SOURCE=..\..\libsrc\include\mm_snprintf.h
# End Source File
# Begin Source File

SOURCE="..\..\..\common-include\msvc\ps_TCHAR.h"

!IF  "$(CFG)" == "lib_mm_snprintf - Win32 Release"

# Begin Custom Build
InputPath=..\..\..\common-include\msvc\ps_TCHAR.h

"..\..\include\ps_TCHAR.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy $(InputPath) ..\..\include

# End Custom Build

!ELSEIF  "$(CFG)" == "lib_mm_snprintf - Win32 Debug"

# Begin Custom Build
InputPath=..\..\..\common-include\msvc\ps_TCHAR.h

"..\..\include\ps_TCHAR.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy $(InputPath) ..\..\include

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\include\ps_TCHAR.h
# End Source File
# End Group
# End Target
# End Project
