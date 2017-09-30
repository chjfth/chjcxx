#ifndef __mm_snprintf_internal_h_
#define __mm_snprintf_internal_h_

#include <ps_TCHAR.h>

typedef int Int;

enum Fmt_et { fmt_unset, fmt_ptr, fmt_decimal_signed, fmt_decimal_unsigned, fmt_float };

extern char g_mmcrlf_szA[];
extern wchar_t g_mmcrlf_szW[];


#define is64bit (sizeof(void*)==8)

#define TMM_strmembytes(chars) ((chars)*sizeof(TCHAR))

int mmsnprintf_IsFloatingType(TCHAR fmt_spec);

int mm_free_buf(void *ptr);

bool Is_IsoZeros(int v_sep_width, int v_adcol_width);
bool Is_RequestIsoZeros(bool v_is_isozeros, int v_adcol_width);

#define CTI_SETVAL(cti, member, val) { \
	cti.member=val; \
	cti.valsize=sizeof(cti.member); \
	cti.outpos = str_l; \
	}

struct cti_pack_stA
{
	FUNC_mmct_outputA *ct_proc;
	void *ct_ctx;
	mmctexi_stA *pcti;

	bool suppress_dbginfo;

	int call_count; // meaningful to one mm_snprintf session
};

struct cti_pack_stW
{
	FUNC_mmct_outputW *ct_proc;
	void *ct_ctx;
	mmctexi_stW *pcti;

	bool suppress_dbginfo;

	int call_count; // meaningful to one mm_snprintf session
};

int in_snprintfA(char *buf, size_t bufsize, const char *fmt, ...); // internal use
int in_snprintfW(wchar_t *buf, size_t bufsize, const wchar_t *fmt, ...); // internal use


////////////////

#if (defined _UNICODE) || (defined UNICODE)

# define g_mmcrlf_sz g_mmcrlf_szW
# define cti_pack_st cti_pack_stW
//# define ctipack_null_output ctipack_null_outputW
# define mmct_DebugStub mmct_DebugStubW
# define ctipack_output ctipack_outputW
# define g_procUserDebug g_procUserDebugW
# define g_cbexUserDebug g_cbexUserDebugW

# define in_snprintf in_snprintfW

#else

# define g_mmcrlf_sz g_mmcrlf_szA
# define cti_pack_st cti_pack_stA
//# define ctipack_null_output ctipack_null_outputA
# define mmct_DebugStub mmct_DebugStubA
# define ctipack_output ctipack_outputA
# define g_procUserDebug g_procUserDebugA
# define g_cbexUserDebug g_cbexUserDebugA

# define in_snprintf in_snprintfA

#endif


#endif
