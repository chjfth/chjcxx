@echo off
setlocal EnableDelayedExpansion
REM Called as this:

set batfilenam=%~n0%~x0
set batdir=%~dp0
set batdir=%batdir:~0,-1%
set _vspgINDENTS=%_vspgINDENTS%.
call :Echos START from %batdir%


REM ==== Prelude Above ====

REM If env-var SDKOUT_DO_CLEAN is empty:
REM This bat will copy .h/.lib/.dll and their corresponding pdb-s to 'sdkout' folder.
REM Parameters influencing the copy operation is prepared in Set-SharedEnv.bat .
REM
REM If env-var SDKOUT_DO_CLEAN is non-empty:
REM This bat will delete ./h/.lib/.dll in 'sdkout' folder accordingly.

if defined SDKOUT_DO_CLEAN (
	set IsCopy=
) else (
	set IsCopy=1
)

set IsLib=
if "%TargetExt%"==".lib" set IsLib=1

set IsDll=
if "%TargetExt%"==".dll" set IsDll=1

set IsDebugBuildConf=
call "%bootsdir%\IsSubStr.bat" IsDebugBuildConf "%BuildConf%" "Debug"
if "%IsDebugBuildConf%"=="1" (
	set sdkout_binDirname=bin-debug
) else (
	set sdkout_binDirname=bin-release
)

REM ========================================================================
REM                         Copy .h to sdkout
REM ========================================================================

if "%IsLib%%IsDll%"=="" goto :DONE_COPY_dotH

if not defined vspu_p_list_HEADERS goto :DONE_COPY_dotH

if not defined vspu_d_HEADER_ROOT (
	call :Echos [ERROR] You defined 'vspu_p_list_HEADERS' but 'vspu_d_HEADER_ROOT' is empty.
	exit /b 4
)

if defined IsCopy (
	call :Echos From [vspu_d_HEADER_ROOT] %vspu_d_HEADER_ROOT%
	call "%bootsdir%\EchoSublines.bat" "Will copy these Filenodes[vspu_p_list_HEADERS] to sdkout folder:" %vspu_p_list_HEADERS%
) else (
	call "%bootsdir%\EchoSublines.bat" "From sdkout folder, will delete header files:" %vspu_p_list_HEADERS%
)

if not defined dirSdkoutHeader (
	call :Echos [ERROR] dirSdkoutHeader should have been defined in Set-SharedEnv.bat .
	exit /b 4
)

REM ==== copy .h list (may be a folder, which contains lots of .h)

for %%h in (%vspu_p_list_HEADERS%) do (
	
	if defined IsCopy (
		call :EchoAndExec cp -r "%vspu_d_HEADER_ROOT%\%%~h" "%dirSdkoutHeader%"
	) else (
		if exist "%dirSdkoutHeader%\%%~h" (
			call :EchoAndExec rm -r "%dirSdkoutHeader%\%%~h"
		)
	)
	if errorlevel 1 exit /b 4
	
	REM if %h has subdir prefix, whether replicate that subdir inside %dirSdkoutHeader% ?
	REM (TODO?)
)


:DONE_COPY_dotH

REM ========================================================================
REM                  Copy .lib(C++ static lib) to sdkout
REM ========================================================================

if "%IsLib%"=="" goto :DONE_COPY_dotLIB

if not defined dirSdkoutLib (
	call :Echos [ERROR] dirSdkoutLib should have been defined in Set-SharedEnv.bat .
	exit /b 4
)
if not exist "%dirSdkoutLib%" (
	mkdir "%dirSdkoutLib%"
	if errorlevel 1 exit /b 4
)

REM ==== copy .lib and .lib.pdb

if defined IsCopy (
	call :EchoAndExec copy "%TargetDir%\%vso_fStaticLib%" "%dirSdkoutLib%"
	if errorlevel 1 exit /b 4

	call :EchoAndExec copy "%TargetDir%\%vso_fStaticLibPdb%" "%dirSdkoutLib%"
	if errorlevel 1 exit /b 4
) else (
	call "%bootsdir%\DelOneFile.bat" "%dirSdkoutLib%\%vso_fStaticLib%"
	if errorlevel 1 exit /b 4

	call "%bootsdir%\DelOneFile.bat" "%dirSdkoutLib%\%vso_fStaticLibPdb%"
	if errorlevel 1 exit /b 4
)


:DONE_COPY_dotLIB

REM ========================================================================
REM                  Copy .dll to sdkout
REM ========================================================================

if "%IsDll%"=="" goto :DONE_COPY_dotDLL

if not defined dirSdkoutLib (
	call :Echos [ERROR] dirSdkoutLib should have been defined in Set-SharedEnv.bat .
	exit /b 4
)
if not exist "%dirSdkoutLib%" (
	mkdir "%dirSdkoutLib%"
	if errorlevel 1 exit /b 4
)

set dirSdkBinNow=%dirSdkoutCidverVCxxx%\%sdkout_binDirname%

if not exist "%dirSdkBinNow%" (
	mkdir "%dirSdkBinNow%"
	if errorlevel 1 exit /b 4
)

if defined IsCopy (
	call :EchoAndExec copy "%TargetDir%\%vso_fDll%" "%dirSdkBinNow%"
	if errorlevel 1 exit /b 4

	call :EchoAndExec copy "%TargetDir%\%vso_fDllPdb%" "%dirSdkBinNow%"
	if errorlevel 1 exit /b 4

	call :EchoAndExec copy "%TargetDir%\%vso_fDllImportlib%" "%dirSdkoutLib%"
	if errorlevel 1 exit /b 4
) else (
	call "%bootsdir%\DelOneFile.bat" "%dirSdkBinNow%\%vso_fDll%"
	if errorlevel 1 exit /b 4

	call "%bootsdir%\DelOneFile.bat" "%dirSdkBinNow%\%vso_fDllPdb%"
	if errorlevel 1 exit /b 4

	call "%bootsdir%\DelOneFile.bat" "%dirSdkoutLib%\%vso_fDllImportlib%"
	if errorlevel 1 exit /b 4
)

:DONE_COPY_dotDLL

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

:SetErrorlevel
  REM Usage example:
  REM call :SetErrorlevel 4
exit /b %1

