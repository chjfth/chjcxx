# Microsoft Developer Studio Project File - Name="dll_gadgetlib" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=dll_gadgetlib - Win32 Debug_U export
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "dll_gadgetlib.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "dll_gadgetlib.mak" CFG="dll_gadgetlib - Win32 Debug_U export"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "dll_gadgetlib - Win32 Debug export" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "dll_gadgetlib - Win32 Debug_U export" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "dll_gadgetlib - Win32 Release export" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "dll_gadgetlib - Win32 Release_U export" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "dll_gadgetlib - Win32 Debug export"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /YX /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /I "../../../common-include" /I "../../libsrc/include" /I "$(NLSSDK)/include" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D DLLEXPORT_gadgetlib=__declspec(dllexport) /YX /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x804 /d "_DEBUG"
# ADD RSC /l 0x804 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 mmsnprintf_D.lib AnsiC_Tfuncs_D.lib kernel32.lib user32.lib /nologo /dll /pdb:"Debug/gadgetlib.dll.pdb" /debug /machine:I386 /out:"Debug/gadgetlib.dll" /implib:"gadgetlib--imp.lib" /pdbtype:sept /libpath:"../../binstore"
# SUBTRACT LINK32 /pdb:none
# Begin Custom Build - Sync output binary( $(InputName).dll , $(InputName)--imp.lib ) to ..\..\binstore folder.
ProjDir=.
TargetName=gadgetlib
InputPath=.\Debug\gadgetlib.dll
InputName=gadgetlib
SOURCE="$(InputPath)"

BuildCmds= \
	copy "$(InputPath)" ..\..\binstore \
	copy "$(ProjDir)\$(InputName)--imp.lib" ..\..\binstore \
	

"..\..\binstore\$(TargetName).dll" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\binstore\$(InputName)--imp.lib" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "dll_gadgetlib - Win32 Debug_U export"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug_U"
# PROP BASE Intermediate_Dir "Debug_U"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug_U"
# PROP Intermediate_Dir "Debug_U"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /I "../../../common-include" /I "../../libsrc/include" /I "$(NLSSDK)/include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /YX /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /I "../../../common-include" /I "../../libsrc/include" /I "$(NLSSDK)/include" /D "UNICODE" /D "_UNICODE" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D DLLEXPORT_gadgetlib=__declspec(dllexport) /YX /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x804 /d "_DEBUG"
# ADD RSC /l 0x804 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 mmsnprintf_D.lib AnsiC_Tfuncs_U_D.lib kernel32.lib user32.lib /nologo /dll /pdb:"Debug_U/gadgetlib_U.dll.pdb" /debug /machine:I386 /out:"Debug_U/gadgetlib_U.dll" /implib:"gadgetlib_U--imp.lib" /pdbtype:sept /libpath:"../../binstore"
# SUBTRACT LINK32 /pdb:none
# Begin Custom Build - Sync output binary( $(InputName).dll , $(InputName)--imp.lib ) to ..\..\binstore folder.
ProjDir=.
TargetName=gadgetlib_U
InputPath=.\Debug_U\gadgetlib_U.dll
InputName=gadgetlib_U
SOURCE="$(InputPath)"

BuildCmds= \
	copy "$(InputPath)" ..\..\binstore \
	copy "$(ProjDir)\$(InputName)--imp.lib" ..\..\binstore \
	

"..\..\binstore\$(TargetName).dll" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\binstore\$(InputName)--imp.lib" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "dll_gadgetlib - Win32 Release export"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /YX /FD /c
# ADD CPP /nologo /MT /W3 /GX /O2 /I "../../../common-include" /I "../../libsrc/include" /I "$(NLSSDK)/include" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D DLLEXPORT_gadgetlib=__declspec(dllexport) /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x804 /d "NDEBUG"
# ADD RSC /l 0x804 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386
# ADD LINK32 mmsnprintf.lib AnsiC_Tfuncs.lib kernel32.lib user32.lib /nologo /dll /pdb:"Release/gadgetlib.dll.pdb" /machine:I386 /out:"Release/gadgetlib.dll" /implib:"gadgetlib--imp.lib" /libpath:"../../binstore"
# SUBTRACT LINK32 /pdb:none
# Begin Custom Build - Sync output binary( $(InputName).dll , $(InputName)--imp.lib ) to ..\..\binstore folder.
ProjDir=.
TargetName=gadgetlib
InputPath=.\Release\gadgetlib.dll
InputName=gadgetlib
SOURCE="$(InputPath)"

BuildCmds= \
	copy "$(InputPath)" ..\..\binstore \
	copy "$(ProjDir)\$(InputName)--imp.lib" ..\..\binstore \
	

"..\..\binstore\$(TargetName).dll" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\binstore\$(InputName)--imp.lib" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "dll_gadgetlib - Win32 Release_U export"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release_U"
# PROP BASE Intermediate_Dir "Release_U"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release_U"
# PROP Intermediate_Dir "Release_U"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /I "../../../common-include" /I "../../libsrc/include" /I "$(NLSSDK)/include" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /YX /FD /c
# ADD CPP /nologo /MT /W3 /GX /O2 /I "../../../common-include" /I "../../libsrc/include" /I "$(NLSSDK)/include" /D "UNICODE" /D "_UNICODE" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D DLLEXPORT_gadgetlib=__declspec(dllexport) /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x804 /d "NDEBUG"
# ADD RSC /l 0x804 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386
# ADD LINK32 mmsnprintf.lib AnsiC_Tfuncs_U.lib kernel32.lib user32.lib /nologo /dll /pdb:"Release_U/gadgetlib_U.dll.pdb" /machine:I386 /out:"Release_U/gadgetlib_U.dll" /implib:"gadgetlib_U--imp.lib" /libpath:"../../binstore"
# SUBTRACT LINK32 /pdb:none
# Begin Custom Build - Sync output binary( $(InputName).dll , $(InputName)--imp.lib ) to ..\..\binstore folder.
ProjDir=.
TargetName=gadgetlib_U
InputPath=.\Release_U\gadgetlib_U.dll
InputName=gadgetlib_U
SOURCE="$(InputPath)"

BuildCmds= \
	copy "$(InputPath)" ..\..\binstore \
	copy "$(ProjDir)\$(InputName)--imp.lib" ..\..\binstore \
	

"..\..\binstore\$(TargetName).dll" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\binstore\$(InputName)--imp.lib" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ENDIF 

# Begin Target

# Name "dll_gadgetlib - Win32 Debug export"
# Name "dll_gadgetlib - Win32 Debug_U export"
# Name "dll_gadgetlib - Win32 Release export"
# Name "dll_gadgetlib - Win32 Release_U export"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=..\..\libsrc\_auto_export_stub_.c
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=../../libsrc/mswin/gadgetlib.rc
# End Source File
# End Group
# End Target
# End Project
