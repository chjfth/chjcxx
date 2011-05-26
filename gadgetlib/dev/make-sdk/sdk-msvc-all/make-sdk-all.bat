@echo off
setlocal
REM gmb_planets_xxx should be set in env-var, not in command line(the make cmdline overriding problem).
for /F "usebackq delims=" %%i IN (`pwd`) DO set CURPATH=%%i
rem set gmi_SYDO_SHOW_COPY_CMD=1

IF "%gmb_syncto%" == "" (
	echo SET gmb_syncto=%CURPATH%/nlssdk
         SET gmb_syncto=%CURPATH%/nlssdk
  )

@echo on
umaketime gmb_compiler_ids="" gmb_msvc_vers="" gmb_wince_vers="" %*
