#include <gadgetlib/gsm0338cvt.h>

//wchar_t ww = L'\xA3';
	//[2010-01-12] This will cause VC6 SP6 compiler spout error:
	//	error C2002: invalid wide-character constant

struct map_gsmc2uc_st
{
	int gc;
	wchar_t uc;
};

map_gsmc2uc_st g_map_gsmc2uc[] =
{
	0,   '@',
	1,   '\xA3', // British pound sign
	2,   '$',
	3,   '\xA5', // Chinese Yuan
	4,   '\xE8', // e with grave
	5,   '\xE9', // e with acute
	6,   '\xF9', // u with grave
	7,   '\xEC', // i with grave
	8,   '\xF2', // o with grave
	9,   '\xC7', // C with cedilla
	0xA, '\n', // new line
	0xB, '\xD8', // O slash 
	0xC, '\xF8', // o slash 
	0xD, '\r', // carriage return 
	0xE, '\xC5', // A ring above
	0xF, '\xE5', // a ring above
	0x10,L'\x0394', // delta
	0x11,'_', //
	0x12,L'\x03A6', // phi (greek)
	0x13,L'\x0393', // gama
	0x14,L'\x039B', // lamda
	0x15,L'\x03A9', // omega
	0x16,L'\x03A0', // pi
	0x17,L'\x03A8', // psi
	0x18,L'\x03A3', // sigma
	0x19,L'\x0398', // theta
	0x1A,L'\x039E', // xi
	0x1B,'\x1B', // ( the escape symbol )
	0x1C,'\xC6', // AE capital (latin)
	0x1D,'\xE6', // ae small
	0x1E,'\xDF', // beta
	0x1F,'\xC9', // E with acute

	0x24, '$', // 0x24 is currency sign, I use $ here for simplicity

	0x40, '\xA1', // upside-down !
	0x60, '\xBF', // upside-down ?
	
	0x5B, '\xC4', // A with diaeresis
	0x5C, '\xD6', // O with diaeresis
	0x5D, '\xD1', // N with tilde
	0x5E, '\xDC', // U with diaeresis
	0x5F, '\xA7', // section sign

	0x7B, '\xE4', // a with diaeresis
	0x7C, '\xF6', // o with diaeresis
	0x7D, '\xF1', // n with tilde
	0x7E, '\xFC', // u with diaeresis
	0x7F, '\xE0', // a with grave

	0x1B0A, '\x0C', // form feed
	0x1B14, '^',
	0x1B28, '{',
	0x1B29, '}',
	0x1B2F, '\\',
	0x1B3C, '[',
	0x1B3D, '~',
	0x1B3E, ']',
	0x1B40, '|',
	0x1B65, L'\x20AC', // Euro
};

const int gn_map_gsmc2uc = sizeof(g_map_gsmc2uc)/sizeof(g_map_gsmc2uc[0]);

static bool
is_gsm_uc_equal(int c)
{
	if(c>=' ' && c<='?') // 0x20~0x3F, including '0'~'9'
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
	if(is_gsm_uc_equal(uc))
		return uc;
	
	for(i=0; i<gn_map_gsmc2uc; i++)
	{
		if(g_map_gsmc2uc[i].uc==uc)
			return g_map_gsmc2uc[i].gc;
	}
	
	return -1;
}
