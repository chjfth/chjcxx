#!/usr/bin/sh

curdir=`pwd`
#export gmi_SYDO_SHOW_COPY_CMD=1

if [ "$gmb_syncto" == "" ]; then
	echo  "gmb_syncto=$curdir/nlssdk"
	export gmb_syncto=$curdir/nlssdk
fi

umaketime gmb_compiler_ids="" gmb_linuxgcc_vers="" "$@"
