@echo off
setlocal

set gmu_DO_SHOW_VERBOSE=1
set gmu_DO_SHOW_COMPILE_CMD=1
set gmu_DO_SHOW_LINK_CMD=1

REM gmb_syncto=... (set SDK output dir)
REM -- will receive default in scalacon-make-sdk.bat
set gmb_syncto=../../sdkout

REM set gmb_compiler_ids=msvc
REM set gmb_msvc_vers=vc100
REM set gmb_wince_vers=

REM [optional] gmu_SC_CHECKOUT_DATETIME=...
REM -- will receive default in scalacon-make-sdk.bat

REM [optional] CMD_GETSDKIN
REM -- used by scalacon-make-sdk.bat, will be NULL if not set.
set CMD_GETSDKIN=scalacon-get-sdkin.py --ini=../../get-sdkin.ini --force

scalacon-make-sdk.bat %*
