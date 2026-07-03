#ifndef __CHHI__StringHelper_h_
#define __CHHI__StringHelper_h_
#define __CHHI__StringHelper_h_created_ 20250426
#define __CHHI__StringHelper_h_updated_ 20260703


#include <assert.h>
#include <string.h>

#include <CxxVerCheck.h>
#include <ps_TCHAR.h>
#include <sdring.h>
#include <_MINMAX_.h>
#include <commdefs.h>


////////////////////////////////////////////////////////////////////////////
//-- namespace nonamespace { 
////////////////////////////////////////////////////////////////////////////
// Place API function declarations in this namespace.


int shp_stricmp(const TCHAR *s1, const TCHAR *s2);


//////// StringSplitter template class ////////

inline bool StringSplitter_IsCrlf(int charval)
{
	return (charval == '\r' || charval == '\n') ? true : false;
}

inline bool StringSplitter_IsSpacechar(int charval)
{
	return (charval == ' ') ? true : false;
}

inline bool StringSplitter_IsSpaceTab(int charval)
{
	return (charval == ' ' || charval == '\t') ? true : false;
}

inline bool StringSplitter_TrimNone(int)
{
	return false;
}

template<
	typename TString, // can be char* or wchar_t*, or any type that supports operator[] 
	bool IsSplitterChar(int charval) = StringSplitter_IsCrlf,
	bool IsTrimChar(int charval) = StringSplitter_TrimNone,
	bool want_null_split = false
	>
	// Difference of splitter-char and trim-char: If trim-char appears inside a substring,
	// for example, a space-char inside " United States " will not be trimmed. 
	// So the produced substring is "United States".
	// Note: If a char is splitter-char, it should NOT also be a trim-char.
class StringSplitter
{
public:
	StringSplitter(const TString &s, int startpos=0, int scanlen=-1)
		: m_str(s), m_startpos(startpos)
	{
		if(scanlen>=0)
		{
			m_endpos_ = startpos + scanlen;
		}
		else 
		{	// m_endpos determined by NUL char
			m_endpos_ = startpos;
			while( m_str[m_endpos_] != '\0' )
				m_endpos_++;
		}

		reset();
	}

	void reset()
	{
		m_nowpos = m_startpos;
		m_next_done = false;
	}

	int next(int *p_nowlen=nullptr)
	{
		SETTLE_OUTPUT_PTR(int, p_nowlen, 0)

		// Return an offset value that points to the next sub-string.
		// *p_nowlen tells the length of this-return's sub-string.

		if (m_next_done)
			return -1;

// 		assert(!IsTrimChar(m_str[m_nowpos]));
// 		if(!want_null_split)
// 			assert(!IsSplitterChar(m_str[m_nowpos]));

		// We're at a substring start, now scan for word length.

		int word_at_pos = m_nowpos;

		for(;;)
		{
			if( IsEnd() || IsSplitterChar(m_str[m_nowpos]) )
			{
				// Meet substring end.
				// Scan backward and drop those trailing trim-chars.

				int tailpos_ = m_nowpos;

				if(tailpos_ > word_at_pos)
				{
					while (IsTrimChar(m_str[tailpos_ - 1]))
						tailpos_--;
				}

				*p_nowlen = tailpos_ - word_at_pos;

				// Hint: We do early rescan for next substring, so that user can have the freedom
				// to NUL-terminate the current substring @word_at_pos+nowlen .

				if (IsEnd())
				{
					m_next_done = true;

					if(!want_null_split && *p_nowlen==0)
						return -1;
				}
				else
				{
					m_nowpos++;
					SkipSpiltterTrimChars();
				}

				if(!want_null_split && *p_nowlen==0)
				{
					word_at_pos = m_nowpos;
					continue;
				}
				
				return word_at_pos;
			}
			else
			{
				m_nowpos++;
			}
		}
	}

	int count()
	{
		reset();
		int n = 0;
		while (next() != -1)
			n++;

		reset();
		return n;
	}

#ifdef CXX14_OR_NEWER
	decltype(auto) operator[](size_t index) {
		return m_str[index];
	}
	decltype(auto) operator[](size_t index) const {
		return m_str[index];
	}
#endif

private:
	bool IsEnd() { return m_nowpos == m_endpos_; }

	void SkipSpiltterTrimChars() // changes m_nowpos
	{
		for (;;)
		{
			if (IsEnd())
				return;

			if(want_null_split && IsSplitterChar(m_str[m_nowpos]))
				return;

			if (IsTrimChar(m_str[m_nowpos]) || IsSplitterChar(m_str[m_nowpos]))
				m_nowpos++;
			else
				return;
		}
	}

private:
	const TString &m_str;

	int m_startpos;
	int m_nowpos;
	int m_endpos_;

	bool m_next_done;
};

/* Example usage:

void test1()
{
	TCHAR *ws = _T("AB;xyz;;1234");
	StringSplitter<decltype(ws), IsSplitterChar> sp(ws);

	int i=0;
	for(;;i++)
	{
		int len = 0;
		int pos = sp.next(&len);
		if(pos==-1)
			break;

		_tprintf(_T("[#%d] %.*s\n"), i, len, &ws[pos]);
	}

	_tprintf(_T("\n"));

	wstring s2 = ws;
	StringSplitter<wstring, IsSplitterChar> sp2(s2, 0, s2.length());
	for(i=0;; i++)
	{
		int len = 0;
		int pos = sp2.next(&len);
		if(pos==-1)
			break;

		wstring sub = s2.substr(pos, len);
		wcout << L"{#" << i << L"} " << sub << endl;
	}
}

*/

//
// [2026-07-03] Dynamic-param flavor of StringSplitter
//

template<typename T_CHAR>
class IStringSplitter
{
private:
	virtual bool IsSplitterChar(T_CHAR charval) = 0;
	virtual bool IsTrimChar(T_CHAR charval) = 0;

public:
	~IStringSplitter() {}

	IStringSplitter(bool _want_null_split=false)
	{
		want_null_split = _want_null_split;
		m_str = nullptr;
		m_startpos = m_endpos_ = 0;
		reset();
	}

	void input(const T_CHAR *s, int startpos=0, int scanlen=-1)
	{
		m_str = s;
		m_startpos = startpos;

		if(s==nullptr && startpos==0)
			scanlen = 0; // force NULL input to be zero length

		if (scanlen >= 0)
		{
			m_endpos_ = startpos + scanlen;
		}
		else
		{	// m_endpos determined by NUL char
			m_endpos_ = startpos;
			while (m_str[m_endpos_] != '\0')
				m_endpos_++;
		}

		reset();
	}

	void reset()
	{
		m_nowpos = m_startpos;
		m_next_done = false;
	}

	int next(int *p_nowlen=nullptr)
	{
		SETTLE_OUTPUT_PTR(int, p_nowlen, 0)

		// Return an offset value that points to the next sub-string.
		// *p_nowlen tells the length of this-return's sub-string.

		if (m_next_done)
			return -1;

		// We're at a substring start, now scan for word length.

		int word_at_pos = m_nowpos;

		for(;;)
		{
			if( IsEnd() || IsSplitterChar(m_str[m_nowpos]) )
			{
				// Meet substring end.
				// Scan backward and drop those trailing trim-chars.

				int tailpos_ = m_nowpos;

				if(tailpos_ > word_at_pos)
				{
					while (IsTrimChar(m_str[tailpos_ - 1]))
						tailpos_--;
				}

				*p_nowlen = tailpos_ - word_at_pos;

				// Hint: We do early rescan for next substring, so that user can have the freedom
				// to NUL-terminate the current substring @word_at_pos+nowlen .

				if (IsEnd())
				{
					m_next_done = true;

					if(!want_null_split && *p_nowlen==0)
						return -1;
				}
				else
				{
					m_nowpos++;
					SkipSpiltterTrimChars();
				}

				if(!want_null_split && *p_nowlen==0)
				{
					word_at_pos = m_nowpos;
					continue;
				}
				
				return word_at_pos;
			}
			else
			{
				m_nowpos++;
			}
		}
	}

	int count()
	{
		reset();
		int n = 0;
		while (next() != -1)
			n++;

		reset();
		return n;
	}

#ifdef CXX14_OR_NEWER
	decltype(auto) operator[](size_t index) {
		return m_str[index];
	}
	decltype(auto) operator[](size_t index) const {
		return m_str[index];
	}
#endif

private:
	bool IsEnd() { return m_nowpos == m_endpos_; }

	void SkipSpiltterTrimChars() // changes m_nowpos
	{
		for (;;)
		{
			if (IsEnd())
				return;

			if (want_null_split && IsSplitterChar(m_str[m_nowpos]))
				return;

			if (IsTrimChar(m_str[m_nowpos]) || IsSplitterChar(m_str[m_nowpos]))
				m_nowpos++;
			else
				return;
		}
	}
private:
	const T_CHAR *m_str;
	bool want_null_split;

	int m_startpos;
	int m_nowpos;
	int m_endpos_;

	bool m_next_done;
};

template<typename T_CHAR>
class CStringSplitter : public IStringSplitter<T_CHAR>
	// this class depicts normal splitter traits
{
public:
	CStringSplitter(
		const T_CHAR arSplitterChars[], int nSplitterChars,
		const T_CHAR arTrimChars[], int nTrimChars,
		bool want_null_split=false) 
		: IStringSplitter(want_null_split)
	{
		int i;
		mar_SplitterChars[0] = mar_TrimChars[0] = T_CHAR(0);

		m_nSplitterChars = _MIN_(nSplitterChars, MaxCheck);
		for(i=0; i<m_nSplitterChars; i++)
			mar_SplitterChars[i] = arSplitterChars[i];

		m_nTrimChars = _MIN_(nTrimChars, MaxCheck);
		for(i=0; i<m_nTrimChars; i++)
			mar_TrimChars[i] = arTrimChars[i];
	}

	bool IsSplitterChar(T_CHAR charval) cxx11_override
	{
		for(int i=0; i<m_nSplitterChars; i++)
		{
			if(charval == mar_SplitterChars[i])
				return true;
		}
		return false;
	}
	
	bool IsTrimChar(T_CHAR charval) cxx11_override
	{
		for(int i=0; i<m_nTrimChars; i++)
		{
			if(charval == mar_TrimChars[i])
				return true;
		}
		return false;
	}

private:
	enum { MaxCheck = 8 };
	T_CHAR mar_SplitterChars[MaxCheck];
	int m_nSplitterChars;
	T_CHAR mar_TrimChars[MaxCheck];
	int m_nTrimChars;
};



inline IStringSplitter<TCHAR>* tcsCreateStringSplitter(bool want_null_split,
	const TCHAR *inputs, int startpos=0, int scanlen=-1,
	const TCHAR* szSplitterChars=_T("\r\n"), const TCHAR* szTrimChars=_T(" \t"))
{
	int slen = (int)_tcslen(szSplitterChars);
	int tlen = (int)_tcslen(szTrimChars);

	IStringSplitter<TCHAR> *pss = new CStringSplitter<TCHAR>(
		szSplitterChars, slen,  szTrimChars, tlen,
		want_null_split);

	pss->input(inputs, startpos, scanlen);

	return pss; // Caller should `delete pss`
}


inline Sdrings SplitToSdrings(const TCHAR *szinput, 
	bool want_null_split=false,
	const TCHAR* szSplitterChars=_T("\r\n"), const TCHAR* szTrimChars=_T(" \t"))
{
	IStringSplitter<TCHAR> *pss = tcsCreateStringSplitter(want_null_split,
		szinput, 0, -1,
		szSplitterChars, szTrimChars);

	int parts = pss->count();
	Sdrings ss(parts);

	int i = 0;
	for(;; i++)
	{
		int len = 0;
		int pos = pss->next(&len);
		if(pos==-1)
			break;

		ss[i] = Sdring(szinput+pos, len);
	}

	assert(i==parts);
	delete pss;

	return ss;
}

////////////////////////////////////////////////////////////////////////////
//-- } // namespace nonamespace
////////////////////////////////////////////////////////////////////////////


/*
////////////////////////////////////////////////////////////////////////////
 ___                 _                           _        _   _             
|_ _|_ __ ___  _ __ | | ___ _ __ ___   ___ _ __ | |_ __ _| |_(_) ___  _ __  
 | || '_ ` _ \| '_ \| |/ _ \ '_ ` _ \ / _ \ '_ \| __/ _` | __| |/ _ \| '_ \ 
 | || | | | | | |_) | |  __/ | | | | |  __/ | | | || (_| | |_| | (_) | | | |
|___|_| |_| |_| .__/|_|\___|_| |_| |_|\___|_| |_|\__\__,_|\__|_|\___/|_| |_|
              |_|                                                           
////////////////////////////////////////////////////////////////////////////
*/
// ++++++++++++++++++ Implementation Below ++++++++++++++++++


#if defined(StringHelper_IMPL) || (defined CHHI_ALL_IMPL && !defined CHHI_ALL_IMPL_HIDE_StringHelper) // [IMPL]


// >>> Include headers required by this lib's implementation

//#include <commdefs.h> // for Uint, Uint64, enum bitwise-OR etc
//#include <snTprintf.h>

#include <ctype.h> // shp_stricmp

// <<< Include headers required by this lib's implementation




#ifndef StringHelper_DEBUG
#include <CHHI_vaDBG_hide.h> // Suppress/invalidate vaDBG macros, from now on
#endif


////////////////////////////////////////////////////////////////////////////
//-- namespace nonamespace {
////////////////////////////////////////////////////////////////////////////
// Place API function Implementation in this namespace.


int shp_stricmp(const TCHAR *s1, const TCHAR *s2)
{
	bool s1null = !s1 || !s1[0];
	bool s2null = !s2 || !s2[0];
	
	if(s1null && s2null)
		return true;
	if(s1null || s2null)
		return false;
		
	for(int i=0; s1[i] || s2[i]; i++)
	{
		if(toupper(s1[i]) < toupper(s2[i]))
			return -(i+1);
		else if(toupper(s1[i]) > toupper(s2[i]))
			return i+1;
	}
	
	return 0;
}




////////////////////////////////////////////////////////////////////////////
//-- } // namespace nonamespace
////////////////////////////////////////////////////////////////////////////



#ifndef StringHelper_DEBUG
#include <CHHI_vaDBG_show.h> // Now restore vaDBG macros
#endif


#endif // [IMPL]


#endif // include once guard
