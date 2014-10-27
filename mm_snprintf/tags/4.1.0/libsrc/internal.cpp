#include <string.h>
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
	mm_snprintfA(addrstr, sizeof(addrstr), 
		is64bit ? "%016llX" : "%08X", 
		is64bit ? (unsigned __int64)addr_end : (unsigned int)addr_end
		);
		// avoid using "%p" because Linux will prefix "0x" while MSVC does not
	
	// ignore leading zeros from "%p" output
	char *p = addrstr;
	while( *p=='0' ) p++;
	
	int slen = strlen(p);
	return slen>4 ? slen : 4;
}

