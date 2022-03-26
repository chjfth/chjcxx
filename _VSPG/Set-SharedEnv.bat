@echo off

REM PATH manipulation must be done BEFORE setlocal.
REM
PATH=D:\Program Files\Git\usr\bin;C:\Program Files\Git\usr\bin;%PATH%

setlocal EnableDelayedExpansion

set batfilenam=%~n0%~x0
set batdir=%~dp0
set batdir=%batdir:~0,-1%
set _vspgINDENTS=%_vspgINDENTS%.
call :Echos START from %batdir%


REM ======== ENV checking for other/later bat execution ========

REM I need Unix-behavior cp.exe to do file/folder copy. Check whether it has been in our PATH.
for %%x in (cp.exe) do set cp_found=%%~$PATH:x
if not defined cp_found (
	call :Echos [ERROR] Unix cp.exe cannot be found in your PATH. I need this program to go. Unix cp.exe can be acquired by installing "Git for Windows", from https://gitforwindows.org/ .
	exit /b 4
)

REM I need Unix-behavior rm.exe to do file/folder deletion.
for %%x in (rm.exe) do set rm_found=%%~$PATH:x
if not defined rm_found (
	call :Echos [ERROR] Unix rm.exe cannot be found in your PATH. I need this program to go. Unix rm.exe can be acquired by installing "Git for Windows", from https://gitforwindows.org/ .
	exit /b 4
)

REM ======== ENV checking done ========


call "%bootsdir%\GetParentDir.bat" dirRepoRoot "%batdir%"

REM If $(PlatformToolset) is sth like "WindowsKernelModeDriver10.0",
REM We know that it is building LIB for kernel mode, so we'll prepare extra suffix.
set krnl_libdirsuffix=
call "%bootsdir%\IsSubStr.bat" isKernelMode "%PlatformToolset%" "KernelMode"
if "%isKernelMode%"=="1" (
	call :Echos Seeing KernelMode bcz PlatformToolset="%PlatformToolset%"
	set krnl_libdirsuffix=_krnl
)

set sdkoutPlatformSuffix=
if "%PlatformName%" == "Win32" set sdkoutPlatformSuffix=
if "%PlatformName%" == "x64" set sdkoutPlatformSuffix=x64
if "%PlatformName%" == "ARM64" set sdkoutPlatformSuffix=ARM64

REM :::::: Set our 'sdkout' folder ::::::
REM
set dirSdkout=%dirRepoRoot%\sdkout
set dirSdkoutHeader=%dirSdkout%\include
set dirSdkoutCidverVCxxx=%dirSdkout%\cidvers\vc%PlatformToolsetVersion%%sdkoutPlatformSuffix%%krnl_libdirsuffix%
set dirSdkoutLib=%dirSdkoutCidverVCxxx%\lib

set vso_fStaticLib=%TargetName%.lib
set vso_fStaticLibPdb=%vso_fStaticLib%.pdb

set vso_fDll=%TargetName%.dll
set vso_fDllPdb=%vso_fDll%.pdb
set vso_fDllImportlib=%TargetName%--imp.lib

endlocal & (
	REM Now export these env-vars to parent env.
	REM These env-vars are shared by CopyLib-to-sdkout.bat and VSPU-CleanProject.bat .
	
	set dirSdkout=%dirSdkout%
	set dirSdkoutHeader=%dirSdkoutHeader%
	set dirSdkoutCidverVCxxx=%dirSdkoutCidverVCxxx%
	set dirSdkoutLib=%dirSdkoutLib%
	set vso_fStaticLib=%vso_fStaticLib%
	set vso_fStaticLibPdb=%vso_fStaticLibPdb%
	set vso_fDll=%vso_fDll%
	set vso_fDllPdb=%vso_fDllPdb%
	set vso_fDllImportlib=%vso_fDllImportlib%
)

REM set vspgdebug_SearchAndExecSubbat=1

exit /b 0

REM =============================
REM ====== Functions Below ======
REM =============================

:Echos
  REM This function preserves %ERRORLEVEL% for the caller,
  REM and, LastError does NOT pollute the caller.
  setlocal & set LastError=%ERRORLEVEL%
  echo %_vspgINDENTS%[%batfilenam%] %*
exit /b %LastError%

:EchoAndExec
  echo %_vspgINDENTS%[%batfilenam%] EXEC: %*
  %*
exit /b %ERRORLEVEL%

:EchoVar
  setlocal & set Varname=%~1
  call echo %_vspgINDENTS%[%batfilenam%] %Varname% = %%%Varname%%%
exit /b 0
