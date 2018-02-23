#define _CRT_SECURE_NO_WARNINGS // to avoid VS2010 warning on open

#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>

#ifdef WIN32

#include <io.h>

int o_binary = O_BINARY;
#define open _open
#define write _write // to avoid VS2010 warning
#define close _close

#else // assume linux

#include <unistd.h>
int o_binary = 0;

#endif

#include "logtofile.h"

int logfile_create(const char *filename) // create a new file
{
	int hfile = open(filename, 
		O_CREAT|O_APPEND|O_TRUNC|O_WRONLY|o_binary, 
		S_IREAD|S_IWRITE);
	return hfile;
}

void logfile_append(int hfile, const void *pcontent, int bytes)
{
	if(hfile>=0)
		write(hfile, pcontent, bytes);
}

void logfile_close(int hfile)
{
	if(hfile>=0)
		close(hfile);
}

