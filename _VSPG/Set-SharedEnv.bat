@echo off
setlocal EnableDelayedExpansion

set batfilenam=%~n0%~x0
set batdir=%~dp0
set batdir=%batdir:~0,-1%
set _vspgINDENTS=%_vspgINDENTS%.

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

set dirSdkout=%dirRepoRoot%\sdkout
set dirSdkoutHeader=%dirSdkout%\include
set dirSdkoutLib=%dirSdkout%\cidvers\vc%PlatformToolsetVersion%%sdkoutPlatformSuffix%%krnl_libdirsuffix%\lib

set vso_fStaticLib=%TargetName%.lib
set vso_fStaticLibPdb=%vso_fStaticLib%.pdb

endlocal & (
	REM Now export these env-vars to parent env.
	REM These env-vars are shared by CopyLib-to-sdkout.bat and VSPU-CleanProject.bat .
	
	set dirSdkout=%dirSdkout%
	set dirSdkoutHeader=%dirSdkoutHeader%
	set dirSdkoutLib=%dirSdkoutLib%
	set vso_fStaticLib=%vso_fStaticLib%
	set vso_fStaticLibPdb=%vso_fStaticLibPdb%
)

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
