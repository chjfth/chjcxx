#ifndef __gadgetlib_iversion_h_20110426_
#define __gadgetlib_iversion_h_20110426_


#define THISLIB_VMAJOR 0
#define THISLIB_VMINOR 6
#define THISLIB_VBUILD 2

#define THISLIBstr__(n) #n
#define THISLIBstr(n) THISLIBstr__(n)

// The following 3 are used in .rc
#define THISLIB_VMAJORs THISLIBstr(THISLIB_VMAJOR)
#define THISLIB_VMINORs THISLIBstr(THISLIB_VMINOR)
#define THISLIB_VBUILDs THISLIBstr(THISLIB_VBUILD)

#define THISLIB_NAME "gadgetlib"

enum {
	gadgetlib_vmajor = THISLIB_VMAJOR,
	gadgetlib_vminor = THISLIB_VMINOR,
	gadgetlib_vbuild = THISLIB_VBUILD,
};


#endif
