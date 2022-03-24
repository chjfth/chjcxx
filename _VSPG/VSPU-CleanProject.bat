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

set ExeDllDir=%TargetDir%

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


if defined dirSdkoutHeader if defined vspu_p_list_HEADERS (
	for %%h in (%vspu_p_list_HEADERS%) do (
		if exist "%dirSdkoutHeader%\%%~h" (
			call :EchoAndExec rm -r "%dirSdkoutHeader%\%%~h"
			if errorlevel 1 exit /b 4
		)
	)
)

if defined vso_fStaticLib if exist "%dirSdkoutLib%\%vso_fStaticLib%" (
	call :EchoAndExec rm "%dirSdkoutLib%\%vso_fStaticLib%"
	if errorlevel 1 exit /b 4
)

if defined vso_fStaticLibPdb if exist "%dirSdkoutLib%\%vso_fStaticLibPdb%" (
	call :EchoAndExec rm "%dirSdkoutLib%\%vso_fStaticLibPdb%"
	if errorlevel 1 exit /b 4
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

:SetErrorlevel
  REM Usage example:
  REM call :SetErrorlevel 4
exit /b %1

:PackDoubleQuotes
  REM Take whole %* as input, replace each " with "" and return the result string.
  REM The return value is put in global var _vspg_dqpacked .
  set allparams=%*
  set _vspg_dqpacked=%allparams:"=""%
exit /b 0

:UnpackDoubleQuotes
  setlocal & set allparams=%~2
  set unpacked=%allparams:""="%
  endlocal & (set %~1=%unpacked%)
exit /b 0

