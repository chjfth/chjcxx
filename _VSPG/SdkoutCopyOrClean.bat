@echo off
setlocal EnableDelayedExpansion
REM Called as this:
REM <this>.bat $(SolutionDir) $(ProjectDir) $(BuildConf) $(PlatformName) $(TargetDir) $(TargetFileNam) $(TargetName) $(IntrmDir)

set batfilenam=%~n0%~x0
set batdir=%~dp0
set batdir=%batdir:~0,-1%
set _vspgINDENTS=%_vspgINDENTS%.
call :Echos START from %batdir%


REM _SolutionDir_ has double-quotes around, SolutionDir has no quotes.
set _SolutionDir_=%1&set SolutionDir=%~1
set _ProjectDir_=%2&set ProjectDir=%~2
set _BuildConf_=%3&set BuildConf=%~3
set _PlatformName_=%4&set PlatformName=%~4
set _TargetDir_=%5&set TargetDir=%~5
set _TargetFilenam_=%6&set TargetFilenam=%~6
set _TargetName_=%7&set TargetName=%~7
set _IntrmDir_=%8&set IntrmDir=%~8

REM == debugging purpose ==
REM call :EchoVar SolutionDir
REM call :EchoVar ProjectDir
REM call :EchoVar _BuildConf_
REM call :EchoVar BuildConf
REM call :EchoVar IntrmDir
REM call :EchoVar TargetDir
REM call :EchoVar PlatformName
REM call :EchoVar TargetName

REM ==== Prelude Above ====

REM This bat will copy .h/.lib/.dll and their corresponding pdb-s to 'sdkout' folder.
REM Parameters influencing the copy operation is prepared in Set-SharedEnv.bat .

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

call :Echos From [vspu_d_HEADER_ROOT] %vspu_d_HEADER_ROOT%
call "%bootsdir%\EchoSublines.bat" "Will copy these Filenodes[vspu_p_list_HEADERS] to sdkout folder:" %vspu_p_list_HEADERS%

if not defined dirSdkoutHeader (
	call :Echos [ERROR] dirSdkoutHeader should have been defined in Set-SharedEnv.bat .
	exit /b 4
)

REM ==== copy .h list (may be a folder, which contains lots of .h)

for %%h in (%vspu_p_list_HEADERS%) do (
	call :EchoAndExec cp -r "%vspu_d_HEADER_ROOT%\%%~h" "%dirSdkoutHeader%"
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

call :EchoAndExec copy "%TargetDir%\%vso_fStaticLib%" "%dirSdkoutLib%"

if errorlevel 1 exit /b 4

call :EchoAndExec copy "%TargetDir%\%vso_fStaticLibPdb%" "%dirSdkoutLib%"
if errorlevel 1 exit /b 4


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

call :EchoAndExec copy "%TargetDir%\%vso_fDll%" "%dirSdkBinNow%"

if errorlevel 1 exit /b 4

call :EchoAndExec copy "%TargetDir%\%vso_fDllPdb%" "%dirSdkBinNow%"

if errorlevel 1 exit /b 4

call :EchoAndExec copy "%TargetDir%\%vso_fDllImportlib%" "%dirSdkoutLib%"
if errorlevel 1 exit /b 4


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

