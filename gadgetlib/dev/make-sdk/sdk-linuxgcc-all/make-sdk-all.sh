#!/usr/bin/sh

curdir=`pwd`
# set gmi_SYDO_SHOW_COPY_CMD=1

export gmb_planets=all
export gmb_planets_linuxgcc=allac

if [ "$gmb_syncto" == "" ]; then
	echo  "gmb_syncto=$curdir/nlssdk"
	export gmb_syncto=$curdir/nlssdk
fi

umake gmb_compiler_ids="all" gmb_linuxgcc_vers="all" "$@"
