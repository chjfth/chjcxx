@echo off

REM You can add more bat search location via vspg_USER_BAT_SEARCH_DIRS .
REM set vspg_USER_BAT_SEARCH_DIRS="mydir1" "mydir2"

setlocal EnableDelayedExpansion

set batfilenam=%~n0%~x0
set batdir=%~dp0
set batdir=%batdir:~0,-1%
set _vspgINDENTS=%_vspgINDENTS%.
call :Echos START from %batdir%

call "%bootsdir%\GetParentDir.bat" dirThisLib "%batdir%"


REM We have to endlocal here, bcz the following :SetEnvVar need to change parent bat's env.
endlocal & (
	REM Every batch var definition from now on will affect the parent env.
	REM So be conservative, define as little env-var as possible.
	REM I suggest to prefix them with _tmp_ if you don't use them after this bat exits.
	set _tmp_=%batdir%
)

REM If you have special environment variable(env-var) to set for you own .bat files,
REM you can set it here.

call :Echos0 Loading Env-vars (affecting parent env).


REM ################ KEY CONTENT HERE ################ 

call :SetEnvVar vsps_d_CHEADER_ROOT=%_tmp_%\libsrc\include
REM -- example: D:\gitw\chjcxx\mm_snprintf\libsrc\include

call :SetEnvVar vsps_p_list_CHEADERS=mm_snprintf.h

call "%userbatdir%\Set-SharedEnv.bat"

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

:Echos0
  REM This function preserves %ERRORLEVEL% for the caller,
  REM and, LastError does NOT pollute the caller.
  setlocal & set LastError=%ERRORLEVEL%
  echo %_vspgINDENTS%.[%~n0%~x0] %*
exit /b %LastError%

:SetEnvVar
  call :Echos0 set %*
  set %*
exit /b 0

