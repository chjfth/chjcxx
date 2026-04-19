#ifndef __StringHelper_h_
#define __StringHelper_h_
#define __StringHelper_h_created_ 20250426
#define __StringHelper_h_updated_ 20260419

#include <assert.h>
#include <CxxVerCheck.h>
#include <commdefs.h>
//#include <vaDbgTs.h>

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
	// for example, a space-char inside " United States ", it will not be trimmed. 
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

#endif
