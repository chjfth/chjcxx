#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <getopt/sgetopt.h>

#include "t_string.h"

#define DLL_AUTO_EXPORT_STUB
extern"C" void sgetopt_lib__is_valid_option__DLL_AUTO_EXPORT_STUB(void){}

bool 
sgetopt_is_valid_short(TCHAR short_opt, const TCHAR *short_list)
{
	assert(short_opt!=_T('-'));

	if(short_opt==_T('-'))
		return false;

	int len = t_strlen(short_list);
	for(int i=0; i<len; i++)
	{
		if( short_list[i]==short_opt )
			return true;
	}

	return false;
}

bool 
sgetopt_is_valid_long(const TCHAR *long_opt, const sgetopt_option long_list[])
{
	if(!long_opt || !long_opt[0])
		return false;

	assert(long_opt[0]=='-' && long_opt[1]=='-');

	for(int i=0; long_list[i].name!=NULL; i++)
	{
		if( t_strcmp(long_list[i].name, long_opt+2)==0 )
			return true;
	}

	return false;
}
