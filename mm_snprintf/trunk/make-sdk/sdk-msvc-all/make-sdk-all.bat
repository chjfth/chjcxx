@echo off
setlocal
call %DIR_NLS_BUILD_ENV%\gmp-cenv-all.bat

for /F "usebackq delims=" %%i IN (`cd`) DO set CURPATH=%%i
set CURPATH=%CURPATH:\=/%

:set gmi_SYDO_SHOW_COPY_CMD=1

IF "%gmb_syncto%" == "" (
	echo SET gmb_syncto=%CURPATH%/nlssdk
         SET gmb_syncto=%CURPATH%/nlssdk
  )

@echo on
umaketime gmb_compiler_ids="" gmb_msvc_vers="" gmb_wince_vers="" %*
