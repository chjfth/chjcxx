@echo off
setlocal
REM gmb_planets_xxx should be set in env-var, not in command line(the make cmdline overriding problem).
rem for /F "usebackq delims=" %%i IN (`pwd`) DO set CURPATH=%%i
rem set gmi_SYDO_SHOW_COPY_CMD=1

set gmb_planets=allac

IF "%gmb_syncto%" == "" (
    echo gmb_syncto is not defined! Please define it to a directory like C:/sdkoutput .
    echo You can define gmb_syncto in env-var.
  ) ELSE (
    umaketime gmb_compiler_ids="all" gmb_msvc_vers="all" gmb_wince_vers=all %*
  )
