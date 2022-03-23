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
	call :Echos [ERROR] Unix cp.exe cannot be found in your PATH. I need this program to go. Unix cp.exe can be acquired by installing "Git for Windows", from https://gitforwindows.org/ .
	exit /b 4
)

if not defined vspu_p_list_HEADERS (
	call :Echos [ERROR] vspu_p_list_HEADERS is empty.
	exit /b 4
)
if not defined vspu_d_HEADER_ROOT (
	call :Echos [ERROR] vspu_d_HEADER_ROOT is empty.
	exit /b 4
)

set hdir=%vspu_d_HEADER_ROOT%
set hlist=%vspu_p_list_HEADERS%

if not defined dirSdkoutHeader (
	call :Echos [ERROR] dirSdkoutHeader should have been defined in Set-SharedEnv.bat .
	exit /b 4
)
if not defined dirSdkoutLib (
	call :Echos [ERROR] dirSdkoutLib should have been defined in Set-SharedEnv.bat .
	exit /b 4
)

if not exist "%dirSdkoutHeader%" (
	mkdir "%dirSdkoutHeader%"
	if errorlevel 1 exit /b 4
)
if not exist "%dirSdkoutLib%" (
	mkdir "%dirSdkoutLib%"
)

REM ========= Start copying

REM ==== copy .h

for %%h in (%hlist%) do (
	call :EchoAndExec cp -r "%hdir%\%%~h" "%dirSdkoutHeader%"
	if errorlevel 1 exit /b 4
)

REM ==== copy .lib and .lib.pdb

call :EchoAndExec copy "%TargetDir%\%vso_fStaticLib%" "%dirSdkoutLib%"

if errorlevel 1 exit /b 4

call :EchoAndExec copy "%TargetDir%\%vso_fStaticLibPdb%" "%dirSdkoutLib%"
if errorlevel 1 exit /b 4


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
  call %*
exit /b %ERRORLEVEL%
