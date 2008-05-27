#include "internal.h"

unsigned short
mmsnprintf_getversion(void)
{
	return (PORTABLE_SNPRINTF_VERSION_MAJOR<<8) | PORTABLE_SNPRINTF_VERSION_MINOR;
}
