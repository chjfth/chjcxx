#ifndef __mmsnprintf_libversion_h
#define __mmsnprintf_libversion_h

#define LIB_VMAJOR 4
#define LIB_VMINOR 4
#define LIB_VBUILD 0

#define LIBVER_STR__(n) #n
#define LIBVER_STR(n) LIBVER_STR__(n)

// LIB_VMAJORs, LIB_VMINORs, LIB_VBUILDs can be used in .rc as (partial) string constant
#define LIB_VMAJORs LIBVER_STR(LIB_VMAJOR)
#define LIB_VMINORs LIBVER_STR(LIB_VMINOR)
#define LIB_VBUILDs LIBVER_STR(LIB_VBUILD)


enum {
	mmsnprintf_vmajor = LIB_VMAJOR,
	mmsnprintf_vminor = LIB_VMINOR,
	mmsnprintf_vbuild = LIB_VBUILD
};




#endif
