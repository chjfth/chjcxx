@echo off
setlocal EnableDelayedExpansion
set batfilenam=%~n0%~x0
set batdir=%~dp0
set batdir=%batdir:~0,-1%
set _vspgINDENTS=%_vspgINDENTS%.
call :Echos START from %batdir%

REM This bat copies .lib and .lib.pdb to sdkout dir according to Scalacon dir-structure.

call "%bootsdir%\GetParentDir.bat" dirRepoRoot "%batdir%"

set myPlatformSuffix=
if "%PlatformName%" == "x64" set myPlatformSuffix=x64
if "%PlatformName%" == "ARM64" set myPlatformSuffix=ARM64

set dirSdkout=%dirRepoRoot%\sdkout
set dirSdkoutLib=%dirSdkout%\cidvers\v%PlatformToolsetVersion%%myPlatformSuffix%\lib

if not exist "%dirSdkoutLib%" (
	mkdir "%dirSdkoutLib%"
)

set execmd=copy "%TargetDir%\%TargetName%.lib" "%dirSdkoutLib%"
call :EchoAndExec %execmd%

if errorlevel 1 exit /b 4

set execmd=copy "%TargetDir%\%TargetName%.lib.pdb" "%dirSdkoutLib%"
call :EchoAndExec %execmd%
if errorlevel 1 exit /b 4

exit /b 0

REM =============================
REM ====== Functions Below ======
REM =============================

:Echos
  echo %_vspgINDENTS%[%batfilenam%] %*
exit /b

:EchoExec
  echo %_vspgINDENTS%[%batfilenam%] EXEC: %*
exit /b

:EchoAndExec
  echo %_vspgINDENTS%[%batfilenam%] EXEC: %*
  %*
exit /b %ERRORLEVEL%
