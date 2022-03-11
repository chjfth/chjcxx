@echo off
setlocal

set batdir=%~dp0
set batdir=%batdir:~0,-1%
set batdir_fs=%batdir:\=/%

set gmu_DO_SHOW_VERBOSE=1
set gmu_DO_SHOW_COMPILE_CMD=1
set gmu_DO_SHOW_LINK_CMD=1

REM gmb_sdkname=... (required, used by Makefile.umk)
set gmb_sdkname=common-include

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

set gmb_ignore_uxm=1

call scalacon-make-sdk.bat %*
