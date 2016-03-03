#!/usr/bin/env python
# -*- coding: UTF-8 -*-

svndatetime = "2016-03-03 21:26:20"
sdkvariant = "mswin"

#========================================

import os
import sys
import getopt
import subprocess
import shlex
import shutil
import stat

mypy = os.path.basename(__file__) # this py's filename
mydir = os.path.dirname(__file__) # this py's dir

try:
	NLSSVN = os.environ['NLSSVN']
except KeyError:
	NLSSVN = "https://nlssvn/svnreps"

#svndatetime = os.environ['gmu_SC_CHECKOUT_DATETIME']
#if not svndatetime:

svnurl = NLSSVN + "/CommonLib/common-include/trunk"
svnlocal = mydir + ("/sdkin/"+sdkvariant+"/CommonLib/common-include").replace('/', os.sep)

optlist, arglist = getopt.getopt(sys.argv[1:], 'f')
optdict = dict(optlist)
optforce = '--force' if ('-f' in optdict) else ''

try:
	if optforce:
		# Remove read-only attribute.
		for root, dirs, files in os.walk(svnlocal):
			for file in files:
				os.chmod(root+os.sep+file, stat.S_IWRITE)

		if os.path.isdir(svnlocal):
			shutil.rmtree(svnlocal)
	
	svncmd = 'svn export %s "%s@{%s}" "%s"'%(optforce, svnurl, svndatetime, svnlocal)

	print "Running cmd: \n  " + svncmd
	ret = subprocess.check_call(shlex.split(svncmd))

	# Make the exported files read-only.
	for root, dirs, files in os.walk(svnlocal):
		for file in files:
#			print 'changing', root+os.sep+file
			os.chmod(root+os.sep+file, stat.S_IREAD)

except OSError as errinfo:
	# Problem: how to distinguish between shutil.rmtree() error or check_call() error?
	print "Cannot execute requested command (%s)"%errinfo
except subprocess.CalledProcessError as cpe:
	print "Error running %s !"%(mypy)
	exit(1)

exit(0)
