#include <assert.h>
#include <stdio.h>

#include <gadgetlib/gadgetlib.h>

int _tmain()
{
	unsigned int v = gadgetlib_get_version_v3n();
#ifdef UNICODE
	wprintf(L"gadgetlib version: %d.%d.%d\n", v>>24, (v>>16)&0xFF, v&0xFFFF);
#else
	printf("gadgetlib version: %d.%d.%d\n", v>>24, (v>>16)&0xFF, v&0xFFFF);
#endif
	return 0;
}
