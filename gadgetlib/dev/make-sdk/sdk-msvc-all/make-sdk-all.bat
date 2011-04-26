@echo off
setlocal
REM gmb_planets_xxx should be set in env-var, not in command line(the make cmdline overriding problem).
for /F "usebackq delims=" %%i IN (`pwd`) DO set CURPATH=%%i
rem set gmi_SYDO_SHOW_COPY_CMD=1

set gmb_planets=all
set gmb_planets_wince=alluc
set gmb_planets_armcpp=allac

IF "%gmb_syncto%" == "" (
	echo SET gmb_syncto=%CURPATH%/nlssdk
         SET gmb_syncto=%CURPATH%/nlssdk
  )

@echo on
umaketime gmb_compiler_ids="all" gmb_msvc_vers="all" gmb_wince_vers=all %*
