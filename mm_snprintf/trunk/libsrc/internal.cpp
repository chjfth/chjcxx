#include <stdlib.h> // free()
#include <string.h>
#include <DlOpe.h> // this .h is only to test my magical .h PDB-sewing
#include "mm_snprintf.h"
#include "internal.h"

unsigned short
mmsnprintf_getversion(void)
{
	return (mmsnprintf_vmajor<<8) | mmsnprintf_vminor;
}


int 
cal_adcol_digits(const void *imagine_addr, int bufbytes)
{
	// Calculate address-column digits after hex formatted,
	// trimming leading zeros

	const int default_digits = 4;
	char *addr_end = (char *)imagine_addr+bufbytes-1;
	if(addr_end==NULL)
		return default_digits;
	
	char addrstr[20];
	if(is64bit)
	{
		mm_snprintfA(addrstr, sizeof(addrstr), "%016llX", (unsigned __int64)addr_end);
		// avoid using "%p" because Linux will prefix "0x" while MSVC does not
	}
	else
	{
		mm_snprintfA(addrstr, sizeof(addrstr), "%08X", 
			(unsigned int)(unsigned __int64)addr_end
			);
		// direct cast from pointer to unsigned int is banned by gcc 4.5 x64
	}

	// ignore leading zeros from "%p" output
	char *p = addrstr;
	while( *p=='0' ) p++;
	
	int slen = (Int)strlen(p);
	return slen>4 ? slen : 4;
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

