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
	const int default_digits = 4;
	char *addr_end = (char *)imagine_addr+bufbytes-1;
	if(addr_end==NULL)
		return default_digits;
	
	char addrstr[20];
	sprintf(addrstr, "%p", addr_end);
	// use "%p" so to adapt 32-bit & 64-bit system automatically
	
	// ignore leading zeros from "%p" output
	char *p = addrstr;
	while( *p=='0' ) p++;
	
	int slen = strlen(p);
	return slen>4 ? slen : 4;
}

