#include "internal.h"

int 
mmsnprintf_IsFloatingType(char fmt_spec)
{
	if(fmt_spec=='f'||fmt_spec=='g'||fmt_spec=='G'||fmt_spec=='e'||fmt_spec=='E')
		return 1; // yes
	else
		return 0; // no
}

unsigned short
mmsnprintf_getversion(void)
{
	return (PORTABLE_SNPRINTF_VERSION_MAJOR<<8) | PORTABLE_SNPRINTF_VERSION_MINOR;
}
