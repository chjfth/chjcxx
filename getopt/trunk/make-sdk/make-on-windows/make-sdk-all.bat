@echo off
setlocal

set batdir=%~dp0
set batdir=%batdir:~0,-1%
set batdir_fs=%batdir:\=/%

set gmu_DO_SHOW_VERBOSE=1
set gmu_DO_SHOW_COMPILE_CMD=1
set gmu_DO_SHOW_LINK_CMD=1

REM gmb_sdkname=... (required, used by Makefile.umk)
set gmb_sdkname=getopt

REM gmb_thisrepo=... (tell the root dir of this SDK) 
set gmb_thisrepo=../..


REM gmb_buildsdk_ini=... (set the INI file required by PI_makesdk_2016)
set gmb_buildsdk_ini=buildsdk.ini

REM gmb_dirname_sdkin=... (set dirname to receive input SDKs)
REM -- default is null. Set it only if you use input SDKs.
set gmb_dirname_sdkin=sdkin

REM gmb_dirname_sdkout=... (set this SDK output dirname)
REM -- will receive default "sdkout" in PI_makesdk_2016
set gmb_dirname_sdkout=sdkout

REM set gmb_compiler_ids=msvc
REM set gmb_msvc_vers=vc100
REM set gmb_planets_msvc_vc100=DUL
REM set gmb_wince_vers=

REM [optional] gmb_run_example_on_compiler_vers
REM -- set this if you need to verify-run some example exe output
REM Caution! When assigning two cvers, DO NOT enclose them in double-quotes. CMD set does not need the quotes.
set gmb_run_example_on_compiler_vers=vc80 vc100x64

REM [optional] CMD_GETSDKIN
REM -- used by scalacon-make-sdk.bat, will be NULL if not set.
set CMD_GETSDKIN=scalacon-get-sdkin.py --ini=../../get-sdkin.ini --force

call scalacon-make-sdk.bat %*
