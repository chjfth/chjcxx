#ifndef __mmsnprintf_libversion_h
#define __mmsnprintf_libversion_h

#define LIB_VMAJOR 4
#define LIB_VMINOR 4
#define LIB_VBUILD 0

#define CONST_NUM_TO_STR__(n) #n
#define CONST_NUM_TO_STR(n) CONST_NUM_TO_STR__(n)

// LIB_VMAJORs, LIB_VMINORs, LIB_VBUILDs can be used in .rc as (partial) string constant
#define LIB_VMAJORs CONST_NUM_TO_STR(LIB_VMAJOR)
#define LIB_VMINORs CONST_NUM_TO_STR(LIB_VMINOR)
#define LIB_VBUILDs CONST_NUM_TO_STR(LIB_VBUILD)

#ifdef gmp_COMPILER_CIDVER
# define gmp_COMPILER_CIDVER_str CONST_NUM_TO_STR(gmp_COMPILER_CIDVER)
#else
# define gmp_COMPILER_CIDVER_str ""
#endif

enum {
	mmsnprintf_vmajor = LIB_VMAJOR,
	mmsnprintf_vminor = LIB_VMINOR,
	mmsnprintf_vbuild = LIB_VBUILD
};




#endif
