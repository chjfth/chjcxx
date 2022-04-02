@echo off
setlocal EnableDelayedExpansion
set batfilenam=%~n0%~x0
set batdir=%~dp0
set batdir=%batdir:~0,-1%
set _vspgINDENTS=%_vspgINDENTS%.
call :Echos START from %batdir%

REM Chj memo: cmdtimeit buildsdk.bat 

if not defined vsps_OUTPUT_ROOT (
	call :SetEnvVar vsps_OUTPUT_ROOT=%batdir%\gf
)

call :Echos Using vsps_OUTPUT_ROOT=%vsps_OUTPUT_ROOT%

set singlevariant_log=msbuild.singlevariant.log
set singlevariant_old=msbuild.singlevariant.log.old
set allvariant_log=msbuild.allvariant.log
set allvariant_old=msbuild.allvariant.log.old

REM Backup MSBuild logfile from previous run.
REM
if exist "%singlevariant_old%" del "%singlevariant_old%"
if exist "%allvariant_old%"    del "%allvariant_old%"
REM
if exist "%singlevariant_log%" ren "%singlevariant_log%" "%singlevariant_old%"
if exist "%allvariant_log%"    ren "%allvariant_log%" "%allvariant_old%"


REM ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
REM We execute buildsdk.proj twice, first with no vsps_NO_COPY_CHEADERS,
REM second, with vsps_NO_COPY_CHEADERS=1 and /m for parallel build.
REM ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

call :EchoAndExec msbuild "%batdir%\buildsdk.proj" /t:SingleVariant ^
	/fl /flp:Verbosity=normal;logfile=%singlevariant_log%  %*
if errorlevel 1 exit /b 4

pause

set vsps_NO_COPY_CHEADERS=1
REM
call :EchoAndExec msbuild "%batdir%\buildsdk.proj" /m /t:AllVariant ^
	/fl /flp:Verbosity=normal;logfile=%allvariant_log%     %*

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

:SetEnvVar
  call :Echos set %*
  set %*
exit /b 0
