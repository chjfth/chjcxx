@echo off
setlocal
call %DIR_NLS_BUILD_ENV%\gmp-cenv-all.bat

set batdir=%~dp0%
set batdir=%batdir:~0,-1%
set batdir_fs=%batdir:\=/%

REM set gmi_SYDO_SHOW_COPY_CMD=1

IF "%gmb_syncto%" == "" (
	echo SET gmb_syncto=%batdir_fs%/sdkout
         SET gmb_syncto=%batdir_fs%/sdkout
  )

set datecmd=date_ +"%%Y-%%m-%%d %%H:%%M:%%S"
if "%gmu_SC_CHECKOUT_DATETIME%" == "" (
	REM Note: Place less CMD commands here, so to avoid delayed-expansion format !foobar! .
	for /F "usebackq delims=" %%i IN (`%datecmd%`) DO set gmu_SC_CHECKOUT_DATETIME=%%i
)
if "%gmu_SC_CHECKOUT_DATETIME%" == "" (
	echo Get current datetime fail! You probably do not have date_.exe in your PATH. Install GMU to get this file.
	exit /b 1
)

REM echo gmu_SC_CHECKOUT_DATETIME=%gmu_SC_CHECKOUT_DATETIME% (debug)

..\..\get-sdkin.py --force --datetime="%gmu_SC_CHECKOUT_DATETIME%"
if ERRORLEVEL 1 (
	echo Error: get-sdkin.py execution failed! Cannot continue.
	exit /b 1
)

@echo on
umaketime %*
@REM %* may have: gmb_compiler_ids="..." gmb_msvc_vers="..." gmb_wince_vers="..." 
