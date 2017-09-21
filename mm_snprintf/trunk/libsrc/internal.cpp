#include <stdlib.h> // free()
#include <string.h>
#include <DlOpe.h> // this .h is only to test my magical .h PDB-sewing
#include "mm_snprintf.h"
#include "internal.h"
#include "libversion.h"

unsigned short
mmsnprintf_getversion(void)
{
	return (mmsnprintf_vmajor<<8) | mmsnprintf_vminor;
}


int mm_free_buf(void *ptr)
{
#ifdef USE_CPP_NEW
	delete ptr;
#else
	free(ptr);
#endif
	return 0;
}

