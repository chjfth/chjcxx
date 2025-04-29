#ifndef __StringHelper_h_20250426_
#define __StringHelper_h_20250426_

inline bool StringSplitter_IsCrlf(int charval)
{
	return (charval == '\r' || charval == '\n') ? true : false;
}

inline bool StringSplitter_TrimSpacechar(int charval)
{
	return (charval == ' ') ? true : false;
}

inline bool StringSplitter_TrimNone(int)
{
	return false;
}

template<typename TString, 
	bool IsSplitterChar(int charval) = StringSplitter_IsCrlf,
	bool IsTrimChar(int charval) = StringSplitter_TrimNone>
class StringSplitter
{
public:
	StringSplitter(const TString &s, int startpos=0, int scanlen=-1)
		: m_str(s), m_startpos(startpos)
	{
		m_nowpos = m_startpos;

		if(scanlen>=0)
		{
			m_endpos_ = m_nowpos + scanlen;
		}
		else 
		{	// m_endpos determined by NUL char
			m_endpos_ = m_nowpos;
			while( m_str[m_endpos_] )
				m_endpos_++;
		}		
	}

	void reset()
	{
		m_nowpos = m_startpos;
	}

	int next(int *p_nowlen=nullptr)
	{
		// Skip consecutive splitter-chars and trim-chars.

		for(;;)
		{
			if(m_nowpos == m_endpos_)
			{
				if(p_nowlen)
					*p_nowlen = 0;

				return -1; // scan ended
			}

			if( IsSplitterChar(m_str[m_nowpos]) || IsTrimChar(m_str[m_nowpos]))
				m_nowpos++;
			else
				break;
		}

		// Check if scan end.

		if (m_nowpos == m_endpos_)
		{
			if (*p_nowlen)
				*p_nowlen = 0;

			return -1;
		}

		// Seen a word start, Scan for word length.

		int word_at_pos = m_nowpos;

		for(;;)
		{
			if( m_nowpos==m_endpos_ || IsSplitterChar(m_str[m_nowpos]) )
			{
				// Scan backward and drop those consecutive trim-chars.

				int tailpos = m_nowpos;
				while (IsTrimChar(m_str[tailpos - 1]))
					tailpos--;

				if(p_nowlen)
					*p_nowlen = tailpos - word_at_pos;
				
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

#if (__cplusplus>=201402L) || (_MSC_VER>=1900) 	// C++14 or VC2015+
	decltype(auto) operator[](size_t index) {
		return m_str[index];
	}
	decltype(auto) operator[](size_t index) const {
		return m_str[index];
	}
#endif

private:
	const TString &m_str;

	int m_startpos;
	int m_nowpos;
	int m_endpos_;
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
