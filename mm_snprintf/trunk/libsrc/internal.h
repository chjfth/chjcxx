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


struct cti_pack_stA
{
	FUNC_mmct_outputA proc;
	void *ctx;
	mmctexi_stA *pcti;
};

struct cti_pack_stW
{
	FUNC_mmct_outputW proc;
	void *ctx;
	mmctexi_stW *pcti;
};

////////////////

#if (defined _UNICODE) || (defined UNICODE)

# define g_mmcrlf_sz g_mmcrlf_szW
# define cti_pack_st cti_pack_stW

#else

# define g_mmcrlf_sz g_mmcrlf_szA
# define cti_pack_st cti_pack_stA

#endif


#endif
