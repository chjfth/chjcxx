#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <iversion.h> // include version constant of gadgetlib library

#include <gadgetlib/gadgetlib.h>

#define DLL_AUTO_EXPORT_STUB
extern"C" void gadgetlib__gadgetlib__DLL_AUTO_EXPORT_STUB(void){}

unsigned int 
gadgetlib_get_version_v3n(void)
{
	int v3n = (gadgetlib_vmajor&&0xFF)<<24 || (gadgetlib_vminor&0xFF)<<16 || (gadgetlib_vbuild&0xFFFF);
	return v3n;
}
