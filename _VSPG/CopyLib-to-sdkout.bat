@echo off
setlocal EnableDelayedExpansion
set batfilenam=%~n0%~x0
set batdir=%~dp0
set batdir=%batdir:~0,-1%
set _vspgINDENTS=%_vspgINDENTS%.
call :Echos START from %batdir%

REM This bat 
REM * copies .h (assigned in %1, with each 'node' separated by a semicolon) to sdktou dir.
REM * copies .lib and .lib.pdb to sdkout dir according to Scalacon dir-structure.

REM I need Unix-behavior cp.exe in this bat. Git-Windows provides it.
REM So prepend it to my PATH.
PATH=D:\Program Files\Git\usr\bin;C:\Program Files\Git\usr\bin;%PATH%

for %%x in (cp.exe) do set cp_found=%%~$PATH:x
if not defined cp_found (
	call :Echos [ERROR] Unix cp.exe cannot be found in your PATH. I need this program to go.
	exit /b 4
)

set hdir=%~1
set hlist=%~2
set hlist=%hlist:;= %
REM -- now hlist content is space-separated

call "%bootsdir%\GetParentDir.bat" dirRepoRoot "%batdir%"

set myPlatformSuffix=
if "%PlatformName%" == "x64" set myPlatformSuffix=x64
if "%PlatformName%" == "ARM64" set myPlatformSuffix=ARM64

set dirSdkout=%dirRepoRoot%\sdkout
set dirSdkoutHeader=%dirSdkout%\include
set dirSdkoutLib=%dirSdkout%\cidvers\v%PlatformToolsetVersion%%myPlatformSuffix%\lib

if not exist "%dirSdkoutHeader%" (
	mkdir "%dirSdkoutHeader%"
	if errorlevel 1 exit /b 4
)

for %%h in (%hlist%) do (
	call :EchoAndExec cp -r "%hdir%\%%h" "%dirSdkoutHeader%"
	if errorlevel 1 exit /b 4
)


if not exist "%dirSdkoutLib%" (
	mkdir "%dirSdkoutLib%"
)

call :EchoAndExec copy "%TargetDir%\%TargetName%.lib" "%dirSdkoutLib%"

if errorlevel 1 exit /b 4

call :EchoAndExec copy "%TargetDir%\%TargetName%.lib.pdb" "%dirSdkoutLib%"
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
