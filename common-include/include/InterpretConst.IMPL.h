// >>> Include headers required by this lib's implementation

//#include <commdefs.h> // for Uint, Uint64, enum bitwise-OR etc
#include <assert.h>
#include <stdio.h>
#include <StringHelper.h>
#include <snTprintf.h>

// <<< Include headers required by this lib's implementation


#ifndef InterpretConst_DEBUG
#include <CHHI_vaDBG_hide.h> // Now hide vaDBG macro
#endif



namespace itc {


Enum2Val_merge::Enum2Val_merge(const Enum2Val_st *arEnum2Val, int nEnum2Val, 
	... // more [arEnum2Val, nEnum2Val] pairs, end with [nullptr, 0]
	)
{
	va_list args;

	// First pass, scan through input params to get total Enum2Val count.
	// Note: This is not count of [arEnum2Val, nEnum2Val] pairs, but total of Enum2Val_st objects.

	int count = nEnum2Val;

	va_start(args, nEnum2Val);

	for(;;)
	{
		const Enum2Val_st *p = va_arg(args, const Enum2Val_st *);
		int n = va_arg(args, int);

		if(!p)
			break;

		count += n;
	}

	va_end(args);
	
	m_count = count;
	mar_enum2val = new Enum2Val_st[count];

	// Second pass, copy the Enum2Val_st content into the new array.

	va_start(args, nEnum2Val);

	const Enum2Val_st *ar_now = arEnum2Val;
	int n = nEnum2Val;
	int idx = 0;

	for(;;)
	{
		for(int i=0; i<n; i++)
		{
			mar_enum2val[idx+i].ConstVal = ar_now[i].ConstVal;
			mar_enum2val[idx+i].EnumName = ar_now[i].EnumName;
		}

		idx += n;

		ar_now = va_arg(args, const Enum2Val_st *);
		n = va_arg(args, int);

		if(!ar_now)
			break;
	}

	va_end(args);
}


void CInterpretConst::_reset(const TCHAR *valfmt)
{
	m_EnumC2V.GroupMask=0; 
	m_EnumC2V.arEnum2Val=nullptr; 
	m_EnumC2V.nEnum2Val=0; 

	m_dtor_delete_groups = false;

	SetValFmt( valfmt );

	m_arGroups = nullptr;
	m_nGroups = 0;
}

bool CInterpretConst::SetValFmt(const TCHAR *fmt)
{
	// fmt example: "%d", "%X", "0x%02X", "0x%04X"

	if(fmt==nullptr || fmt[0]=='\0')
	{
		m_valfmt = nullptr;
		return true;
	}

	// simple fmt format checking:
	// try to format a value 0x3F, check whether we get "3F", "3f" or "63" in output string. 

	bool is_correct_valfmt = true;

	if(fmt)
	{
		TCHAR tbuf[OneDisplayMaxChars] = {};
		_sntprintf_s(tbuf, _TRUNCATE, fmt, 0x3F);

		if(_tcsstr(tbuf, _T("3F"))==nullptr 
			&& _tcsstr(tbuf, _T("3f"))==nullptr
			&& _tcsstr(tbuf, _T("63"))==nullptr
			)
			is_correct_valfmt = false;

		assert(is_correct_valfmt==true);
	}

	m_valfmt = is_correct_valfmt ? fmt : nullptr; // can be nullptr, means default

	return is_correct_valfmt;
}

CInterpretConst::~CInterpretConst()
{
	if(m_dtor_delete_groups)
		delete []m_arGroups;
}

void CInterpretConst::_ctor(const Enum2Val_st *arEnum2Val, int nEnum2Val,
	const TCHAR *valfmt)
{
	_reset(valfmt);

	m_EnumC2V.GroupMask = 0xFFFFffff;
	m_EnumC2V.arEnum2Val = arEnum2Val;
	m_EnumC2V.nEnum2Val = nEnum2Val;

	m_arGroups = &m_EnumC2V;
	m_nGroups = 1;

	ensure_unique_masks();
}

void CInterpretConst::_ctor(const SingleBit2Val_st *arBitfield2Val, int nBitfield2Val,
	const TCHAR *valfmt)
{
	_reset(valfmt);

	m_arGroups = new ItcGroup_st[nBitfield2Val];
	if(!m_arGroups)
		return;

	m_nGroups = nBitfield2Val;

	int i;
	for(i=0; i<nBitfield2Val; i++)
	{
//		assert(arBitfield2Val[i].ConstVal != 0); // move this assert into Interpret_i1()

		m_arGroups[i].GroupMask = arBitfield2Val[i].ConstVal;
		m_arGroups[i].arEnum2Val = reinterpret_cast<const Enum2Val_st*>(arBitfield2Val+i);
		m_arGroups[i].nEnum2Val = 1;
	}

	m_dtor_delete_groups = true;

	ensure_unique_masks();
}

void CInterpretConst::_ctor(const ItcGroup_st *arGroups, int nGroups,
	const TCHAR *valfmt)
{
	_reset(valfmt);

	m_arGroups = const_cast<ItcGroup_st*>(arGroups);
	m_nGroups = nGroups;

	ensure_unique_masks();
}

#ifdef _MSC_VER
#ifndef va_copy
// VC2010 lacks va_copy, so manual supply it
#define va_copy(destination, source) ((destination) = (source))
#endif
#endif 

void CInterpretConst::_ctor(const TCHAR *valfmt,
	const EnumGroup_st *arGroups, int nGroups, 
	va_list args // [arSinglebit2Val, nSinglebit2Val] pairs, end with [nullptr, nullptr]
	) // {most generic initor}
{
	_reset(valfmt);

	va_list args_copy;
	va_copy(args_copy, args); // save a copy for 2nd-iteration


	//// 1st iteration ////

	// check quantity of input bitfields chunks >>>

	int nBitfieldsChunk = 0; // debug
	int nBitfieldsAll = 0;

	for(; ; nBitfieldsChunk++)
	{
		const SingleBit2Val_st *p = va_arg(args, const SingleBit2Val_st *);
		int n = va_arg(args, int);
		
		if(!p)
			break;

		nBitfieldsAll += n;
	}

	// check quantity of input bitfields chunks <<< 
	// (Result in nBitfieldsAll.)

	m_nGroups = nGroups + nBitfieldsAll;

	m_arGroups = new ItcGroup_st[m_nGroups];
	if(!m_arGroups)
		return;

	//// 2nd iteration ////

	// Copy input Enum-groups to internal buffer.

	for(int i=0; i<nGroups; i++)
	{
		m_arGroups[i].GroupMask = arGroups[i].GroupMask;
		m_arGroups[i].arEnum2Val = arGroups[i].arEnum2Val;
		m_arGroups[i].nEnum2Val  = arGroups[i].nEnum2Val;
	}

	// Copy input bitfields to internal buffer.

	int bfall_verify = 0;

	int advGroup = nGroups;
	
	for(; ;)
	{
		assert(bfall_verify<=nBitfieldsAll);

		const SingleBit2Val_st *pBF = va_arg(args_copy, const SingleBit2Val_st *);
		int nBF = va_arg(args_copy, int);

		if(!pBF)
			break;

		for(int j=0; j<nBF; j++)
		{
			m_arGroups[advGroup+j].GroupMask = pBF[j].ConstVal;
			m_arGroups[advGroup+j].arEnum2Val = reinterpret_cast<const Enum2Val_st*>(pBF+j);
			m_arGroups[advGroup+j].nEnum2Val  = 1;
		}

		advGroup   += nBF;
		bfall_verify += nBF;
	}

	assert(bfall_verify==nBitfieldsAll);

	va_end(args_copy);

	m_dtor_delete_groups = true;

	ensure_unique_masks();
}

bool CInterpretConst::is_unique_mask(CONSTVAL_t oldmasks, CONSTVAL_t newmask)
{
	// oldmasks and newmask must NOT have overlapped bits set.

	if( (oldmasks^newmask) == (oldmasks|newmask) )
		return true;
	else
		return false;
}

bool CInterpretConst::ensure_unique_masks()
{
	CONSTVAL_t accum_masks = 0;
	int sec;
	for(sec=0; sec<m_nGroups; sec++)
	{
		assert(m_arGroups[sec].GroupMask != 0);

		bool ok_unique_mask	= is_unique_mask(accum_masks, m_arGroups[sec].GroupMask);
		assert(ok_unique_mask);

		if(!ok_unique_mask)
			return false;

		accum_masks |= m_arGroups[sec].GroupMask;
	}

	return true;
}

const TCHAR* CInterpretConst::valuefmt() const
{
	const TCHAR *pfmt = m_valfmt;

	if(!pfmt)
	{
		pfmt = is_enum_ctor() ? _T("%u") : _T("0x%X");
	}

	return pfmt; // always point to a persistent string
}

TCHAR* CInterpretConst::FormatOneDisplay(
	const TCHAR *szVal, CONSTVAL_t val, DisplayFormat_et dispfmt, 
	TCHAR obuf[], int obufsize,
	const TCHAR *valfmt // to override valuefmt()
	) const
{
	if(!valfmt || !valfmt[0])
		valfmt = valuefmt(); // "0x%08X" etc

	if(dispfmt==DF_NameOnly)
	{
		// Example: ERROR_PRIVILEGE_NOT_HELD

		_sntprintf_s(obuf, obufsize, _TRUNCATE, _T("%s"), szVal);
	}
	else if(dispfmt==DF_NameAndValue)
	{
		// Example: ERROR_PRIVILEGE_NOT_HELD(1314)

		TCHAR _fmt_[FmtSpecMaxChars+2] = {};
		_sntprintf_s(_fmt_, _TRUNCATE, _T("%%s(%s)"), valfmt);

		_sntprintf_s(obuf, obufsize, _TRUNCATE, _fmt_, szVal, val);
	}
	else if(dispfmt==DF_ValueAndName)
	{
		// Example: 1314(ERROR_PRIVILEGE_NOT_HELD)

		TCHAR _fmt_[FmtSpecMaxChars+2] = {};
		_sntprintf_s(_fmt_, _TRUNCATE, _T("%s(%%s)"), valfmt);

		_sntprintf_s(obuf, obufsize, _TRUNCATE, _fmt_, val, szVal);
	}
	else // consider it DF_ValueOnly
	{
		// Example: 1314

		_sntprintf_s(obuf, obufsize, _TRUNCATE, valfmt, val);
	}

	return obuf;
}

const TCHAR *CInterpretConst::Interpret_i1(
	CONSTVAL_t input_val, DisplayFormat_et dispfmt,
	TCHAR *buf, int bufsize, 
	const TCHAR *valfmt,
	const TCHAR *sep) const
{
	if(bufsize<=0)
		return NULL;

	if(!valfmt || !valfmt[0])
		valfmt = valuefmt(); // "0x%08X" etc

	if(!sep || !sep[0])
		sep = _T("|");

	if(dispfmt==DF_NVAuto)
		dispfmt = m_nGroups==1 ? DF_ValueAndName : DF_NameAndValue;

	buf[0] = 0;

	CONSTVAL_t remain_val = input_val;

	int sec = 0;
	for(sec=0; sec<m_nGroups; sec++) // iterate each section(group)
	{
		// Process each group.
		const ItcGroup_st &nowgroup = m_arGroups[sec];

		CONSTVAL_t sec_val = input_val & nowgroup.GroupMask;

		auto c2v = nowgroup.arEnum2Val;

		if(nowgroup.nEnum2Val==1)
		{
			// [2025-04-17] User must not pass in 0 value when defining a SingleBit2Val_st[]
			assert(c2v[0].ConstVal != 0); // TODO: need improvement
		}

		int i;
		for(i=0; i<nowgroup.nEnum2Val; i++)
		{
			if(c2v[i].ConstVal==sec_val)
			{
				if(c2v[i].EnumName)
				{
					TCHAR szbuf[OneDisplayMaxChars] = {};
					_sntprintf_s(buf, bufsize, _TRUNCATE, _T("%s%s%s"), 
						buf, 
						FormatOneDisplay(c2v[i].EnumName, c2v[i].ConstVal, 
							dispfmt, szbuf, ARRAYSIZE(szbuf), valfmt),
						sep
						);
				}

				break;
			}
		}

		if(i==nowgroup.nEnum2Val)
		{
			// No designated name exists, we consider it an unrecognized value from
			// ITC interpreter's knowledge, so should present its HEXRR instead of mute on it.

			if( sec_val!=0 || i>1 )
			{
				TCHAR szfmt_concat[20] = {};
				_sntprintf_s(szfmt_concat, _TRUNCATE, _T("%%s%s"), valfmt);
				
				_sntprintf_s(buf, bufsize, _TRUNCATE, szfmt_concat, buf, sec_val);
			}
			else
			{
				// It's a single-bit group and secval==0, 
				// this 0-value is of course not considered unrecognized, so mute it here. 

				assert(sec_val==0);
			}
		}

		remain_val &= ~m_arGroups[sec].GroupMask;
	}

	if(remain_val)
	{
		// present unrecognized value to user
		_sntprintf_s(buf, bufsize, _TRUNCATE, _T("%s0x%X%s"), buf, remain_val, sep);
	}

	// Remove trailing sep
	int slen = (int)_tcslen(buf);
	int seplen = (int)_tcslen(sep);

	if(slen>=seplen && _tcscmp(buf+slen-seplen, sep)==0)
	{
		slen -= seplen;
		buf[slen] = '\0';
	}

	// If output string empty, fill a '0' to indicate zero value.
	if(buf[0]=='\0' && bufsize>=2)
		buf[0] = '0', buf[1] = 0;

	return buf;
}

String CInterpretConst::Interpret(CONSTVAL_t input_val, 
	DisplayFormat_et dispfmt, 
	const TCHAR *valfmt,
	const TCHAR *sep) const
{
/*
	if(!valfmt)
	{
		// This assert() alerts the case that user unintentionally use `sep` as `valfmt`.
		// In typical usage, user passes valfmt="0x%08X" or "%08X" and sep="\r\n".
		assert(sep!=nullptr);
	}
*/

	String itcs(WholeDisplayMaxChars);
	Interpret_i1(input_val, dispfmt, itcs.getbuf(), itcs.bufsize(), valfmt, sep);
	return itcs;
}

TCHAR * 
CInterpretConst::DumpText(TCHAR userbuf[], int nbufchars, int *pReqBufsize) const
{
	// Return userbuf.

	TCHAR tbuf[WholeDisplayMaxChars+500] = _T("");

	int i, j;
	TCHAR szfmt[40] = _T("");

	// First scan: pick out only SingleBit2Val

	for(i=0; i<m_nGroups; i++)
	{
		ItcGroup_st &nowgroup = m_arGroups[i];

		if(nowgroup.nEnum2Val==1)
		{
			_sntprintf_s(szfmt, _TRUNCATE, _T("%%s%s %%s\r\n"), valuefmt());

			_sntprintf_s(tbuf, _TRUNCATE, szfmt, tbuf,
				nowgroup.arEnum2Val[0].ConstVal, nowgroup.arEnum2Val[0].EnumName);
		}
	}

	// Second scan: check how many true-EnumGroups .
	int true_enumgroups = 0;

	for(i=0; i<m_nGroups; i++)
	{
		if(m_arGroups[i].nEnum2Val>1)
			true_enumgroups++;
	}

	// Third scan: pick out those true-EnumGroups .

	for(i=0; i<m_nGroups; i++)
	{
		ItcGroup_st &nowgroup = m_arGroups[i];

		if(nowgroup.nEnum2Val==1)
			continue;

		if(true_enumgroups>1)
		{
			// and I'll prepend a mask-value text line.

			_sntprintf_s(szfmt, _TRUNCATE, _T("%%s\r\n[Mask: %s]\r\n"), valuefmt());

			_sntprintf_s(tbuf, _TRUNCATE, szfmt, tbuf,
				nowgroup.GroupMask);
		}

		for(j=0; j<nowgroup.nEnum2Val; j++)
		{
			_sntprintf_s(szfmt, _TRUNCATE, _T("%%s%s %%s\r\n"), valuefmt());

			_sntprintf_s(tbuf, _TRUNCATE, szfmt, tbuf,
				nowgroup.arEnum2Val[j].ConstVal, nowgroup.arEnum2Val[j].EnumName);
		}
	}

	int textlen = (int)_tcslen(tbuf);
	if(pReqBufsize)
		*pReqBufsize = textlen;

	_sntprintf_s(userbuf, nbufchars, _TRUNCATE, _T("%s"), tbuf);
	return userbuf;
}


CONSTVAL_t CInterpretConst::OneNameToVal(const TCHAR *name, bool *p_is_err)
{
	CONSTVAL_t retval = 0;
	
	bool is_err = false;
	if(!p_is_err)
		p_is_err = &is_err;

	int i, j;
	for(i=0; i<m_nGroups; i++)
	{
		ItcGroup_st &nowgroup = m_arGroups[i];

		for(j=0; j<nowgroup.nEnum2Val; j++)
		{
			if( _tcscmp(name, nowgroup.arEnum2Val[j].EnumName)==0 )
			{
				*p_is_err = false;
				return nowgroup.arEnum2Val[j].ConstVal;
			}
		}
	}

	*p_is_err = true;
	return 0;
}

inline bool IsPipeChar(int charval)
{
	return charval=='|' ? true : false;
}

CONSTVAL_t CInterpretConst::NamesToVal(const TCHAR *names, bool *p_is_err)
{
	StringSplitter<decltype(names), IsPipeChar, StringSplitter_TrimSpacechar> sp(names);
	CONSTVAL_t retval = 0;

	bool is_err = false;
	if(!p_is_err)
		p_is_err = &is_err;
	*p_is_err = false;

	for(;;)
	{
		int len = 0;
		int offset = sp.next(&len);
		if(offset==-1)
			break;

		TCHAR onename[80] = _T("");
		_sntprintf_s(onename, _TRUNCATE, _T("%.*s"), len, names+offset);

		bool one_err = false;
		CONSTVAL_t oneval = OneNameToVal(onename, &one_err);
		
		retval |= oneval;
		*p_is_err |= one_err;
	}

	return retval;
}



} // namespace itc



#ifndef InterpretConst_DEBUG
#include <CHHI_vaDBG_show.h> // Now restore vaDBG macro
#endif

