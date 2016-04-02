#include <ps_TCHAR.h>

#include "share.h"

#ifdef _MSC_VER
int _tmain()
#else
int main()
#endif
{
	verify_timefuncs();
	
	printf("Verify success.\n");
	return 0;
}
