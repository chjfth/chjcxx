# Microsoft Developer Studio Project File - Name="lib_gadgetlib" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=lib_gadgetlib - Win32 Debug_U
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "lib_gadgetlib.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "lib_gadgetlib.mak" CFG="lib_gadgetlib - Win32 Debug_U"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "lib_gadgetlib - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "lib_gadgetlib - Win32 Debug_U" (based on "Win32 (x86) Static Library")
!MESSAGE "lib_gadgetlib - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "lib_gadgetlib - Win32 Release_U" (based on "Win32 (x86) Static Library")
!MESSAGE "lib_gadgetlib - Win32 Debug export" (based on "Win32 (x86) Static Library")
!MESSAGE "lib_gadgetlib - Win32 Debug_U export" (based on "Win32 (x86) Static Library")
!MESSAGE "lib_gadgetlib - Win32 Release export" (based on "Win32 (x86) Static Library")
!MESSAGE "lib_gadgetlib - Win32 Release_U export" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "lib_gadgetlib - Win32 Debug"

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
# ADD CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /I "../../libsrc/include" /I "../../../../CommonLib/mm_snprintf/libsrc/include" /I "../../../../CommonLib/AnsiC_Tfuncs/libsrc/include" /I "../../../../CommonLib/common-include" /D "_DEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /D "_CRT_SECURE_NO_WARNINGS" /YX /Fd"gadgetlib_D.lib.pdb" /FD /GZ /c
# ADD BASE RSC /l 0x804 /d "_DEBUG"
# ADD RSC /l 0x804 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"gadgetlib_D.lib"
# Begin Custom Build - Sync output binary to binstore folder: copy "$(InputPath)" ..\..\binstore
TargetName=gadgetlib_D
InputPath=.\gadgetlib_D.lib
SOURCE="$(InputPath)"

"..\..\binstore\$(TargetName).lib" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy "$(InputPath)" ..\..\binstore

# End Custom Build

!ELSEIF  "$(CFG)" == "lib_gadgetlib - Win32 Debug_U"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug_U"
# PROP BASE Intermediate_Dir "Debug_U"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug_U"
# PROP Intermediate_Dir "Debug_U"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "../../libsrc/include" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /I "../../libsrc/include" /I "../../../../CommonLib/mm_snprintf/libsrc/include" /I "../../../../CommonLib/AnsiC_Tfuncs/libsrc/include" /I "../../../../CommonLib/common-include" /D "UNICODE" /D "_UNICODE" /D "_DEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /D "_CRT_SECURE_NO_WARNINGS" /YX /Fd"gadgetlib_U_D.lib.pdb" /FD /GZ /c
# ADD BASE RSC /l 0x804 /d "_DEBUG"
# ADD RSC /l 0x804 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"gadgetlib_U_D.lib"
# Begin Custom Build - Sync output binary to binstore folder: copy "$(InputPath)" ..\..\binstore
TargetName=gadgetlib_U_D
InputPath=.\gadgetlib_U_D.lib
SOURCE="$(InputPath)"

"..\..\binstore\$(TargetName).lib" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy "$(InputPath)" ..\..\binstore

# End Custom Build

!ELSEIF  "$(CFG)" == "lib_gadgetlib - Win32 Release"

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
# ADD CPP /nologo /MT /W3 /GX /Zi /O2 /I "../../libsrc/include" /I "../../../../CommonLib/mm_snprintf/libsrc/include" /I "../../../../CommonLib/AnsiC_Tfuncs/libsrc/include" /I "../../../../CommonLib/common-include" /D "NDEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /D "_CRT_SECURE_NO_WARNINGS" /YX /Fd"gadgetlib.lib.pdb" /FD /c
# ADD BASE RSC /l 0x804 /d "NDEBUG"
# ADD RSC /l 0x804 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"gadgetlib.lib"
# Begin Custom Build - Sync output binary to binstore folder: copy "$(InputPath)" ..\..\binstore
TargetName=gadgetlib
InputPath=.\gadgetlib.lib
SOURCE="$(InputPath)"

"..\..\binstore\$(TargetName).lib" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy "$(InputPath)" ..\..\binstore

# End Custom Build

!ELSEIF  "$(CFG)" == "lib_gadgetlib - Win32 Release_U"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release_U"
# PROP BASE Intermediate_Dir "Release_U"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release_U"
# PROP Intermediate_Dir "Release_U"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /I "../../libsrc/include" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /MT /W3 /GX /Zi /O2 /I "../../libsrc/include" /I "../../../../CommonLib/mm_snprintf/libsrc/include" /I "../../../../CommonLib/AnsiC_Tfuncs/libsrc/include" /I "../../../../CommonLib/common-include" /D "UNICODE" /D "_UNICODE" /D "NDEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /D "_CRT_SECURE_NO_WARNINGS" /YX /Fd"gadgetlib_U.lib.pdb" /FD /c
# ADD BASE RSC /l 0x804 /d "NDEBUG"
# ADD RSC /l 0x804 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"gadgetlib_U.lib"
# Begin Custom Build - Sync output binary to binstore folder: copy "$(InputPath)" ..\..\binstore
TargetName=gadgetlib_U
InputPath=.\gadgetlib_U.lib
SOURCE="$(InputPath)"

"..\..\binstore\$(TargetName).lib" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy "$(InputPath)" ..\..\binstore

# End Custom Build

!ELSEIF  "$(CFG)" == "lib_gadgetlib - Win32 Debug export"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug.export"
# PROP BASE Intermediate_Dir "Debug.export"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug.export"
# PROP Intermediate_Dir "Debug.export"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /I "../../libsrc/include" /I "../../../../CommonLib/mm_snprintf/libsrc/include" /I "../../../../CommonLib/AnsiC_Tfuncs/libsrc/include" /I "../../../../CommonLib/common-include" /D "_DEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /D "_CRT_SECURE_NO_WARNINGS" /D DLLEXPORT_gadgetlib=__declspec(dllexport) /YX /Fd"gadgetlib_D.lib.pdb" /FD /GZ /c
# ADD BASE RSC /l 0x804 /d "_DEBUG"
# ADD RSC /l 0x804 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"gadgetlib_D.export.lib"
# Begin Custom Build - Sync output binary to binstore folder: copy "$(InputPath)" ..\..\binstore
TargetName=gadgetlib_D.export
InputPath=.\gadgetlib_D.export.lib
SOURCE="$(InputPath)"

"..\..\binstore\$(TargetName).lib" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy "$(InputPath)" ..\..\binstore

# End Custom Build

!ELSEIF  "$(CFG)" == "lib_gadgetlib - Win32 Debug_U export"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug_U.export"
# PROP BASE Intermediate_Dir "Debug_U.export"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug_U.export"
# PROP Intermediate_Dir "Debug_U.export"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "../../libsrc/include" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /I "../../libsrc/include" /I "../../../../CommonLib/mm_snprintf/libsrc/include" /I "../../../../CommonLib/AnsiC_Tfuncs/libsrc/include" /I "../../../../CommonLib/common-include" /D "UNICODE" /D "_UNICODE" /D "_DEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /D "_CRT_SECURE_NO_WARNINGS" /D DLLEXPORT_gadgetlib=__declspec(dllexport) /YX /Fd"gadgetlib_U_D.lib.pdb" /FD /GZ /c
# ADD BASE RSC /l 0x804 /d "_DEBUG"
# ADD RSC /l 0x804 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"gadgetlib_U_D.export.lib"
# Begin Custom Build - Sync output binary to binstore folder: copy "$(InputPath)" ..\..\binstore
TargetName=gadgetlib_U_D.export
InputPath=.\gadgetlib_U_D.export.lib
SOURCE="$(InputPath)"

"..\..\binstore\$(TargetName).lib" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy "$(InputPath)" ..\..\binstore

# End Custom Build

!ELSEIF  "$(CFG)" == "lib_gadgetlib - Win32 Release export"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release.export"
# PROP BASE Intermediate_Dir "Release.export"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release.export"
# PROP Intermediate_Dir "Release.export"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /MT /W3 /GX /Zi /O2 /I "../../libsrc/include" /I "../../../../CommonLib/mm_snprintf/libsrc/include" /I "../../../../CommonLib/AnsiC_Tfuncs/libsrc/include" /I "../../../../CommonLib/common-include" /D "NDEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /D "_CRT_SECURE_NO_WARNINGS" /D DLLEXPORT_gadgetlib=__declspec(dllexport) /YX /Fd"gadgetlib.lib.pdb" /FD /c
# ADD BASE RSC /l 0x804 /d "NDEBUG"
# ADD RSC /l 0x804 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"gadgetlib.export.lib"
# Begin Custom Build - Sync output binary to binstore folder: copy "$(InputPath)" ..\..\binstore
TargetName=gadgetlib.export
InputPath=.\gadgetlib.export.lib
SOURCE="$(InputPath)"

"..\..\binstore\$(TargetName).lib" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy "$(InputPath)" ..\..\binstore

# End Custom Build

!ELSEIF  "$(CFG)" == "lib_gadgetlib - Win32 Release_U export"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release_U.export"
# PROP BASE Intermediate_Dir "Release_U.export"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release_U.export"
# PROP Intermediate_Dir "Release_U.export"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /I "../../libsrc/include" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /MT /W3 /GX /Zi /O2 /I "../../libsrc/include" /I "../../../../CommonLib/mm_snprintf/libsrc/include" /I "../../../../CommonLib/AnsiC_Tfuncs/libsrc/include" /I "../../../../CommonLib/common-include" /D "UNICODE" /D "_UNICODE" /D "NDEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /D "_CRT_SECURE_NO_WARNINGS" /D DLLEXPORT_gadgetlib=__declspec(dllexport) /YX /Fd"gadgetlib_U.lib.pdb" /FD /c
# ADD BASE RSC /l 0x804 /d "NDEBUG"
# ADD RSC /l 0x804 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"gadgetlib_U.export.lib"
# Begin Custom Build - Sync output binary to binstore folder: copy "$(InputPath)" ..\..\binstore
TargetName=gadgetlib_U.export
InputPath=.\gadgetlib_U.export.lib
SOURCE="$(InputPath)"

"..\..\binstore\$(TargetName).lib" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy "$(InputPath)" ..\..\binstore

# End Custom Build

!ENDIF 

# Begin Target

# Name "lib_gadgetlib - Win32 Debug"
# Name "lib_gadgetlib - Win32 Debug_U"
# Name "lib_gadgetlib - Win32 Release"
# Name "lib_gadgetlib - Win32 Release_U"
# Name "lib_gadgetlib - Win32 Debug export"
# Name "lib_gadgetlib - Win32 Debug_U export"
# Name "lib_gadgetlib - Win32 Release export"
# Name "lib_gadgetlib - Win32 Release_U export"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE="..\..\libsrc\charenccvt-common.cpp"
# End Source File
# Begin Source File

SOURCE="..\..\libsrc\mswin\charenccvt-win.cpp"
# End Source File
# Begin Source File

SOURCE=..\..\libsrc\mswin\dirfuncs.cpp
# End Source File
# Begin Source File

SOURCE="..\..\libsrc\fstream-aux.cpp"
# End Source File
# Begin Source File

SOURCE=..\..\libsrc\mswin\fstream64.cpp
# End Source File
# Begin Source File

SOURCE=..\..\libsrc\gadgetlib.cpp
# End Source File
# Begin Source File

SOURCE=..\..\libsrc\mswin\gadgetlib.rc
# End Source File
# Begin Source File

SOURCE=..\..\libsrc\gsm0338cvt.cpp
# End Source File
# Begin Source File

SOURCE="..\..\libsrc\mbcs-ops.cpp"
# End Source File
# Begin Source File

SOURCE="..\..\libsrc\time-common.cpp"
# End Source File
# Begin Source File

SOURCE=..\..\libsrc\mswin\wintime.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=..\..\libsrc\include\gadgetlib\gadgetlib.h
# End Source File
# Begin Source File

SOURCE="..\..\..\..\CommonLib\common-include\msvc\ps_TCHAR.h"

!IF  "$(CFG)" == "lib_gadgetlib - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - Performing Custom Build: copy $(InputPath) ..\..\libsrc\include
InputPath="..\..\..\..\CommonLib\common-include\msvc\ps_TCHAR.h"

"..\..\libsrc\include\ps_TCHAR.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy $(InputPath) ..\..\libsrc\include

# End Custom Build

!ELSEIF  "$(CFG)" == "lib_gadgetlib - Win32 Debug_U"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - Performing Custom Build: copy $(InputPath) ..\..\libsrc\include
InputPath="..\..\..\..\CommonLib\common-include\msvc\ps_TCHAR.h"

"..\..\libsrc\include\ps_TCHAR.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy $(InputPath) ..\..\libsrc\include

# End Custom Build

!ELSEIF  "$(CFG)" == "lib_gadgetlib - Win32 Release"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - Performing Custom Build: copy $(InputPath) ..\..\libsrc\include
InputPath="..\..\..\..\CommonLib\common-include\msvc\ps_TCHAR.h"

"..\..\libsrc\include\ps_TCHAR.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy $(InputPath) ..\..\libsrc\include

# End Custom Build

!ELSEIF  "$(CFG)" == "lib_gadgetlib - Win32 Release_U"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - Performing Custom Build: copy $(InputPath) ..\..\libsrc\include
InputPath="..\..\..\..\CommonLib\common-include\msvc\ps_TCHAR.h"

"..\..\libsrc\include\ps_TCHAR.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy $(InputPath) ..\..\libsrc\include

# End Custom Build

!ELSEIF  "$(CFG)" == "lib_gadgetlib - Win32 Debug export"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - Performing Custom Build: copy $(InputPath) ..\..\libsrc\include
InputPath="..\..\..\..\CommonLib\common-include\msvc\ps_TCHAR.h"

"..\..\libsrc\include\ps_TCHAR.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy $(InputPath) ..\..\libsrc\include

# End Custom Build

!ELSEIF  "$(CFG)" == "lib_gadgetlib - Win32 Debug_U export"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - Performing Custom Build: copy $(InputPath) ..\..\libsrc\include
InputPath="..\..\..\..\CommonLib\common-include\msvc\ps_TCHAR.h"

"..\..\libsrc\include\ps_TCHAR.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy $(InputPath) ..\..\libsrc\include

# End Custom Build

!ELSEIF  "$(CFG)" == "lib_gadgetlib - Win32 Release export"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - Performing Custom Build: copy $(InputPath) ..\..\libsrc\include
InputPath="..\..\..\..\CommonLib\common-include\msvc\ps_TCHAR.h"

"..\..\libsrc\include\ps_TCHAR.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy $(InputPath) ..\..\libsrc\include

# End Custom Build

!ELSEIF  "$(CFG)" == "lib_gadgetlib - Win32 Release_U export"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - Performing Custom Build: copy $(InputPath) ..\..\libsrc\include
InputPath="..\..\..\..\CommonLib\common-include\msvc\ps_TCHAR.h"

"..\..\libsrc\include\ps_TCHAR.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy $(InputPath) ..\..\libsrc\include

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\libsrc\include\ps_TCHAR.h
# End Source File
# End Group
# End Target
# End Project
