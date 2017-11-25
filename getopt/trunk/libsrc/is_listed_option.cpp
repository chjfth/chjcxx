#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <ps_TCHAR.h>
#include <gadgetlib/T_string.h>
#include <getopt/sgetopt.h>

bool sgetopt_is_listed_option(const TCHAR *opt, 
	const TCHAR *short_options, const sgetopt_option long_options[])
{
	// Check if opt is a defined(=expected) option.
	// opt is one argv[n] element
	int i=0;
	if(opt[0]==_T('-') && opt[1]==_T('-'))
	{
		if(!long_options)
			return false;

		// long option format
		for(i=0; long_options[i].name!=NULL; i++)
		{
			if( T_strcmp(long_options[i].name, opt+2)==0 )
				return true;
		}
		return false;
	}
	else if(opt[0]==_T('-'))
	{
		if(!short_options)
			return false;

		// short option format
		int len = T_strlen(short_options);
		for(i=0; i<len; i++)
		{
			if( short_options[i]==opt[1] )
				return true;
		}
		return false;
	}
	return false;
}
