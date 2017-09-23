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

bool Is_IsoZeros(int v_sep_width, int v_adcol_width)
{
	// iso-zeros means:
	// The v_addr 1.0002 should be left zero-padded to 0001.0002 ,
	// so each sep-section is of same length(isochronous).
	
	assert(v_sep_width>=0);
	if(v_sep_width==0)
		return false;
	
	return v_adcol_width<0 ? true : false;
}

bool Is_RequestIsoZeros(bool v_is_isozeros, int v_adcol_width)
{
	if(v_is_isozeros && v_adcol_width==0)
		return true; // %4.0v
	else
		return false; // %4v
}