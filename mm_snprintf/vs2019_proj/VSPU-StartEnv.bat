@echo off
setlocal EnableDelayedExpansion

set batfilenam=%~n0%~x0
set batdir=%~dp0
set batdir=%batdir:~0,-1%
set _vspgINDENTS=%_vspgINDENTS%.

call "%bootsdir%\GetParentDir.bat" parentdir "%batdir%"
call "%bootsdir%\GetParentDir.bat" dirThisLib "%SolutionDir%"



REM We have to endlocal here, bcz the following :SetEnvVar need to change parent bat's env.
endlocal & (
	REM Every batch var definition from now on will affect the parent env.
	REM So be conservative, define as little env-var as possible.
	REM I suggest to prefix them with _tmp_ if you don't use them after this bat exits.
	set _tmp_=%dirThisLib%
)

REM If you have special environment variable(env-var) to set for you own .bat files,
REM you can set it here.

call :Echos0 Loading Env-vars (affecting parent env).


REM ################ KEY CONTENT HERE ################ 

call :SetEnvVar vspu_d_HEADER_ROOT=%_tmp_%\libsrc\include
REM -- example: D:\gitw\chjcxx\mm_snprintf\libsrc\include

call :SetEnvVar vspu_p_list_HEADERS=mm_snprintf.h

call "%userbatdir%\Set-SharedEnv.bat"

exit /b 0

REM =============================
REM ====== Functions Below ======
REM =============================

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

