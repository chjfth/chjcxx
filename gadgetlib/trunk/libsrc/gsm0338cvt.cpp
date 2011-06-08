#include <gadgetlib/gsm0338cvt.h>

#define DLL_AUTO_EXPORT_STUB
extern"C" void gadgetlib_lib__gsm0338cvt__DLL_AUTO_EXPORT_STUB(void){}

//wchar_t ww = L'\xA3';
	//[2010-01-12] This will cause VC6 SP6 compiler spout error:
	//	error C2002: invalid wide-character constant
	//It seems the L'\x' represented char value must not be 0x81-0xFF,
	//so, I'would use 0xA3 instead of L'\xA3' as a workaround.
	//VS2005 does not have such problem.

struct map_gsmc2uc_st
{
	int gc;
	wchar_t uc;
};

map_gsmc2uc_st g_map_gsmc2uc[] =
{
	0,   '@',
	1,   0xA3, // British pound sign
	2,   '$',
	3,   0xA5, // Chinese Yuan
	4,   0xE8, // e with grave
	5,   0xE9, // e with acute
	6,   0xF9, // u with grave
	7,   0xEC, // i with grave
	8,   0xF2, // o with grave
	9,   0xC7, // C with cedilla
	0xA, '\n', // new line
	0xB, 0xD8, // O slash 
	0xC, 0xF8, // o slash 
	0xD, '\r', // carriage return 
	0xE, 0xC5, // A ring above
	0xF, 0xE5, // a ring above
	0x10,0x0394, // delta
	0x11,'_', //
	0x12,0x03A6, // phi (greek)
	0x13,0x0393, // gama
	0x14,0x039B, // lamda
	0x15,0x03A9, // omega
	0x16,0x03A0, // pi
	0x17,0x03A8, // psi
	0x18,0x03A3, // sigma
	0x19,0x0398, // theta
	0x1A,0x039E, // xi
	0x1B,0x1B, // ( the escape symbol )
	0x1C,0xC6, // AE capital (latin)
	0x1D,0xE6, // ae small
	0x1E,0xDF, // beta
	0x1F,0xC9, // E with acute

//	'$', 0xA4, // 0x24($) maps to the generic currency symbol, 
		// If user wants to process GSM0338 0x24 more explicitly, he should do so himself.

	0x40, 0xA1, // upside-down !
	0x60, 0xBF, // upside-down ?
	
	0x5B, 0xC4, // A with diaeresis
	0x5C, 0xD6, // O with diaeresis
	0x5D, 0xD1, // N with tilde
	0x5E, 0xDC, // U with diaeresis
	0x5F, 0xA7, // section sign

	0x7B, 0xE4, // a with diaeresis
	0x7C, 0xF6, // o with diaeresis
	0x7D, 0xF1, // n with tilde
	0x7E, 0xFC, // u with diaeresis
	0x7F, 0xE0, // a with grave

	0x1B0A, 0x0C, // form feed
	0x1B14, '^',
	0x1B28, '{',
	0x1B29, '}',
	0x1B2F, '\\',
	0x1B3C, '[',
	0x1B3D, '~',
	0x1B3E, ']',
	0x1B40, '|',
	0x1B65, 0x20AC, // Euro
};

const int gn_map_gsmc2uc = sizeof(g_map_gsmc2uc)/sizeof(g_map_gsmc2uc[0]);

static bool
is_gsm_uc_equal(int c)
{
	if(c>=' ' && c<='?') // 0x20~0x3F, including '0'~'9','$'
		return true;
	if(c>='A' && c<='Z' || c>='a' && c<='z')
		return true;

	return false;
}

wchar_t 
ggt_gsm2uc(int gsmc)
{
	int i;
	if(is_gsm_uc_equal(gsmc))
		return gsmc;

	for(i=0; i<gn_map_gsmc2uc; i++)
	{
		if(g_map_gsmc2uc[i].gc==gsmc)
			return g_map_gsmc2uc[i].uc;
	}

	return ggt_INVALID_WCHAR;
}

int 
ggt_uc2gsm(wchar_t uc)
{
	int i;
	if(uc=='$' || uc==0xA4) // check this before is_gsm_uc_equal() to override '$'s mapping
		return 0x02;

	if(is_gsm_uc_equal(uc))
		return uc;
	
	for(i=0; i<gn_map_gsmc2uc; i++)
	{
		if(g_map_gsmc2uc[i].uc==uc)
			return g_map_gsmc2uc[i].gc;
	}
	
	return -1;
}
