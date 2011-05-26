#!/usr/bin/sh

curdir=`pwd`
# set gmi_SYDO_SHOW_COPY_CMD=1


if [ "$gmb_syncto" == "" ]; then
	echo  "gmb_syncto=$curdir/nlssdk"
	export gmb_syncto=$curdir/nlssdk
fi

umake gmb_compiler_ids="" gmb_linuxgcc_vers="" "$@"
