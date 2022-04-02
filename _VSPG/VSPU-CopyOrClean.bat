@echo off
setlocal EnableDelayedExpansion

set batfilenam=%~n0%~x0
set batdir=%~dp0
set batdir=%batdir:~0,-1%
set _vspgINDENTS=%_vspgINDENTS%.
call :Echos START from %batdir%


REM ==== Prelude Above ====

REM Check param1 validity, must be 1 or 0
if "%~1"=="1" (
	set SDKOUT_DO_CLEAN=
) else if "%~1"=="0" (
	set SDKOUT_DO_CLEAN=1
) else (
	call :Echos [ERROR] First parameter must be 1 or 0. 1=docopy, 0=doclean.
	exit /b 4
)

REM If env-var SDKOUT_DO_CLEAN is empty(=null):
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

rem call :echos ====================vsps_NO_COPY_CHEADERS=%vsps_NO_COPY_CHEADERS%
if defined vsps_NO_COPY_CHEADERS goto :DONE_COPY_CHEADERS

if "%IsLib%%IsDll%"=="" goto :DONE_COPY_CHEADERS

if not defined vsps_p_list_CHEADERS goto :DONE_COPY_CHEADERS

if not defined vsps_d_CHEADER_ROOT (
	call :Echos [ERROR] You defined 'vsps_p_list_CHEADERS' but 'vsps_d_CHEADER_ROOT' is empty.
	exit /b 4
)

if defined IsCopy (
	call :Echos From [vsps_d_CHEADER_ROOT] %vsps_d_CHEADER_ROOT%
	call "%bootsdir%\EchoSublines.bat" "Sdkout folder Will receive copy of these Filenodes[vsps_p_list_CHEADERS]:" %vsps_p_list_CHEADERS%
) else (
	call "%bootsdir%\EchoSublines.bat" "From sdkout folder, will delete header files[vsps_p_list_CHEADERS]:" %vsps_p_list_CHEADERS%
)

if not defined dirSdkoutHeader (
	call :Echos [ERROR] dirSdkoutHeader should have been defined in Set-SharedEnv.bat .
	exit /b 4
)

REM ==== copy .h list (may be a folder, which contains lots of .h)

for %%h in (%vsps_p_list_CHEADERS%) do (
	
	if defined IsCopy (
		if not exist "%dirSdkoutHeader%" (
			mkdir "%dirSdkoutHeader%"
			if errorlevel 1 exit /b 4
		)
		REM For `cp -r`, do NOT duplicate final nodename on destation-param.
		call :EchoAndExec cp -r -p "%vsps_d_CHEADER_ROOT%\%%~h" "%dirSdkoutHeader%"
	) else (
		if exist "%dirSdkoutHeader%\%%~h" (
			call :EchoAndExec rm -r "%dirSdkoutHeader%\%%~h"
		) else (
			call :Echos Already deleted: "%dirSdkoutHeader%\%%~h"
		)
	)
	if errorlevel 1 exit /b 4
	
	REM if %h has subdir prefix, whether replicate that subdir inside %dirSdkoutHeader% ?
	REM (TODO?)
)


:DONE_COPY_CHEADERS

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
	call :EchoAndExec copy "%TargetDir%\%vso_fStaticLib%" "%dirSdkoutLib%\%vso_fStaticLib%"
	if errorlevel 1 exit /b 4

	call :EchoAndExec copy "%TargetDir%\%vso_fStaticLibPdb%" "%dirSdkoutLib%\%vso_fStaticLibPdb%"
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
	call :EchoAndExec copy "%TargetDir%\%vso_fDll%" "%dirSdkBinNow%\%vso_fDll%"
	if errorlevel 1 exit /b 4

	call :EchoAndExec copy "%TargetDir%\%vso_fDllPdb%" "%dirSdkBinNow%\%vso_fDllPdb%"
	if errorlevel 1 exit /b 4

	call :EchoAndExec copy "%TargetDir%\%vso_fDllImportlib%" "%dirSdkoutLib%\%vso_fDllImportlib%"
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

