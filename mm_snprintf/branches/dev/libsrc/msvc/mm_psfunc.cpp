#include <string.h>
#include "../mm_psfunc.h"

int 
mmps_i64_type_prefix(char *szPrefix)
{
	strcpy(szPrefix, "I64");
	return 3; // "I64" is 3 characters.
}
