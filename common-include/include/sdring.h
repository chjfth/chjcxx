#ifndef __sdring_h_
#define __sdring_h_
#define __sdring_h_created_ 20251225_
#define __sdring_h_updated_ 20260418_

/* Semantic:

A sdring describes a buffer of T_CHAR(typically char or wchar_t), so that user can consider
it a string.

Private member m_buf points to the buffer and m_nchars tells the buffer size.

An extra zero-value is always appended at location m_buf[m_nchars], so that user can always
treat the sdring as a NUL-terminated string.

If user desires it, user can store NUL-char in the middle of m_buf[], bcz m_nchars 
indicate its raw-length.

For operator[], a negative value mean counting backward from sdring end(like Python).
*/

// Hint: Extra helper: makeTstring.h

template<typename T_CHAR>
class sdring
{
public:
	static int bufSize_(int nchars){ return nchars+1; }

	sdring(int nchars)
	{
		_ct0r();
		if(nchars>0)
			setbufsize(nchars);
	}

	sdring(const T_CHAR* instr)	{
		_ctor(instr);
	}

	sdring(const T_CHAR* inchars, int nchars) {
		_ctor(inchars, nchars);
	}

public:
	// boilerplate code, no need to modify >>>
	sdring() { _ct0r(); }
	~sdring()
	{
		_dtor();
		_ct0r();
	}
	sdring(const sdring& old)            // copy-ctor
	{
		_copy_from_old(old); 
	}
	sdring& operator=(const sdring& old) // copy-assign
	{
		if (this != &old) {
			_dtor();
			_copy_from_old(old);
		}
		return *this;
	}
	sdring(sdring&& old)            // move-ctor
	{
		_steal_from_old(old);
		old._ct0r();
	}
	sdring& operator=(sdring&& old) // move-assign
	{
		if (this != &old) {
			_dtor();
			_steal_from_old(old);
			old._ct0r();
		}
		return *this;
	}
	// boilerplate code, no need to modify <<<

private:
	void _copy_from_old(const sdring& old) 
	{
		_ctor(old.m_buf);
	}

	void _steal_from_old(sdring& old) {
		m_buf = old.m_buf;
		m_nchars = old.m_nchars;
	}

public:
	sdring& operator=(const T_CHAR *instr)
	{
		_dtor();
		_ctor(instr);
		return *this;
	}

	const T_CHAR* c_str() const {
		return m_buf;
	}

	T_CHAR* getptr() {
		return m_buf;
	}
//  operator T_CHAR* () {
//		// I don't use this in favor of `operator T_CHAR* & ()`.
//		// Enable both will cause ambiguity in many user cases.
//		return getptr();
//  }

	T_CHAR* & getbuf() {
		// Note: returns a reference(not T_CHAR*'s value)
		// For function prototype, put a `const` exactly before `&` is vital.
		return m_buf; 
	}
	operator T_CHAR* & () { 
		return getbuf();
	}

	const T_CHAR* takeover(T_CHAR *buf, int nchars)
	{
		// [2026-04-06] Used by makeTstring.h
		// Let sdring manage user-provided buf[], buf should be C++-delete-able.

		if(m_buf)
			delete[] m_buf;

		m_buf = buf;
		if(nchars>=0)
			m_nchars = nchars;
		else
			m_nchars = str_len(m_buf);

		return m_buf;
	}

	T_CHAR* setbufsize(int nchars) // to-test
	{
		if(nchars<=0)
			nchars = 0;

		T_CHAR *newbuf = new T_CHAR[bufSize_(nchars)];
		newbuf[0] = newbuf[nchars] = '\0';

		if(m_buf)
		{
			// copy as-many-length of old string to newbuf
			int nchars_to_copy = m_nchars<=nchars ? m_nchars : nchars;
			for(int i=0; i<nchars_to_copy; i++)
				newbuf[i] = m_buf[i];
			
			newbuf[nchars_to_copy] = '\0';

			delete[] m_buf;
			m_buf = newbuf;
			m_nchars = nchars_to_copy;
		}
		else
		{
			m_buf = newbuf;
			m_nchars = nchars;
		}

		return m_buf;
	}

	int rawlen() const {
		return m_nchars;
	}

	int ztlen() const // zero(NUL)-terminated length
	{ 
		if(!m_buf || !m_buf[0])
			return 0;
		else
			return str_len(m_buf);
	}

	bool is_empty() const {
		if (!m_buf || !m_buf[0])
			return true;
		else
			return false;
	}

	T_CHAR& operator[](int pos) {
		if(pos<0)
			pos = m_nchars + pos; // count from backward
		return m_buf[pos];
	}

	const T_CHAR& operator[](int pos) const {
		if (pos < 0)
			pos = m_nchars + pos; // count from backward
		return m_buf[pos];
	}

	operator bool() const {
		return (m_buf && m_buf[0]) ? true : false;
	}

	bool operator==(const T_CHAR *instr) 
	{
		return str_match( this->c_str(), instr );
	}

	bool operator==(const sdring& ins)
	{
		return str_match( this->c_str(), ins.c_str() );
	}

private:
	void _ctor(const T_CHAR *instr)
	{
		// [2025-12-25] Note: We distinguish between 
		//	instr==NULL 
		// and
		//	instr[0]=='\0'

		_ct0r();

		if (instr)
		{
			m_nchars = str_len(instr); // m_nchars can be 0

			m_buf = new T_CHAR[bufSize_(m_nchars)];
			str_cpy(m_buf, instr);
		}
	}

	void _ctor(const T_CHAR *inchars, int nchars)
	{	// inchars[] may contain NUL, we always copy `nchars` count.
		_ct0r();
		if (inchars)
		{
			m_nchars = nchars;

			m_buf = new T_CHAR[bufSize_(m_nchars)];
			m_buf[m_nchars] = '\0';

			for(int i=0; i<nchars; i++)
				m_buf[i] = inchars[i];
		}
	}

public:

	// Following str_xxx() function apply NUL-terminated string logic.

	static int str_len(const T_CHAR s[])
	{
		int slen = 0;
		while(s[slen])
			slen++;
		return slen;
	}

	static int str_cpy(T_CHAR dst[], const T_CHAR src[])
	{
		int i=0;
		for(; src[i]; i++)
			dst[i] = src[i];
		dst[i] = '\0'; // append NUL char
		return i; // i is string len
	}

	static int str_ncpy(T_CHAR dst[], const T_CHAR src[], int maxlen)
	{
		// dst[] buffer should be maxlen+1
		if(maxlen<0)
			maxlen = 0;
		int i=0;
		for(; src[i] && i<maxlen; i++)
			dst[i] = src[i];
		dst[i] = '\0';
		return i;
	}

	static bool str_match(const T_CHAR *s1, const T_CHAR *s2)
	{
		bool s1null = !s1 || !s1[0];
		bool s2null = !s2 || !s2[0];
		
		if(s1null && s2null)
			return true;
		if(s1null || s2null)
			return false;

		for(int i=0; s1[i] || s2[i]; i++)
		{
			if(s1[i]!=s2[i])
				return false;
		}
		return true;
	}

	// Following nchars_xxx() function apply raw buffer logic.

	bool nchars_match(const sdring& insdr) const
	{
		if(m_nchars!=insdr.m_nchars)
			return false;

		for(int i=0; i<m_nchars; i++)
		{
			if(m_buf[i] != insdr.m_buf[i])
				return false;
		}
		return true;
	}

	static int nchars_cpy(T_CHAR dst[], const T_CHAR src[], int len)
	{
		for(int i=0; i<len; i++)
			dst[i] = src[i];
		return len;
	}

	static int findchar_in(T_CHAR c, const T_CHAR *in_what, int nwhats) // return c's offst
	{
		for (int i = 0; i < nwhats; i++)
		{
			if(c == in_what[i])
				return i;
		}
		return -1;
	}

	static bool belongs(T_CHAR c, const T_CHAR *to_what, int nwhats)
	{
		if(findchar_in(c, to_what, nwhats)>=0)
			return true;
		else
			return false;
	}

public:
	//////////////////////////////
	//    Facility functions   >>>
	//////////////////////////////

	int findchar(T_CHAR c, int start_from=0) const
	{
		if(start_from>=m_nchars)
			return -1; // param out of range

		if(start_from<0) // pos from backward
			start_from = m_nchars + start_from;

		if(start_from<0)
			return -1; // param out of range

		return findchar_in(c, m_buf+start_from, m_nchars-start_from);
	}

	sdring trim_ex(bool trimleft, bool trimright, 
		const T_CHAR *trim_what=nullptr, int nwhat=-1) const // like Python strip()
	{
		static T_CHAR trim_default[] = {' ', '\t', '\r', '\n'};
		const int ndefault = ARRAYSIZE(trim_default);
		if (!trim_what) 
			trim_what = trim_default, nwhat = ndefault;
		
		if(nwhat<0)
			nwhat = str_len(trim_what);

		int nleft = 0, nright = 0;
		
		if(trimleft)
		{
			while( nleft<m_nchars && belongs(m_buf[nleft], trim_what, nwhat) )
				nleft++;
		}

		if(trimright)
		{
			while( nleft+nright<m_nchars && belongs(m_buf[m_nchars-1-nright], trim_what, nwhat) )
				nright++;
		}

		if(nleft+nright == m_nchars)
			return sdring();
		else {
			int outlen = m_nchars - nleft - nright;
			sdring sdout(outlen);
			nchars_cpy(sdout.m_buf, m_buf+nleft, outlen);
			return sdout;
		}
	}

	sdring trim(const T_CHAR *trim_what=nullptr, int nwhat=-1) const
	{
		return trim_ex(true, true, trim_what, nwhat);
	}
	sdring trimleft(const T_CHAR *trim_what=nullptr, int nwhat=-1) const
	{
		return trim_ex(true, false, trim_what, nwhat);
	}
	sdring trimright(const T_CHAR *trim_what=nullptr, int nwhat=-1) const
	{
		return trim_ex(false, true, trim_what, nwhat);
	}

	sdring operator+(const sdring& tail) const
	{
		return this->append(tail.m_buf, tail.m_nchars);
	}

	sdring operator+(const T_CHAR *tail) const
	{
		return this->append(tail, -1);
	}

	sdring append(const T_CHAR *tail, int ntail=-1) const
	{
		if(ntail<0)
			ntail = str_len(tail);

		int totlen = m_nchars + ntail;
		sdring sdret(totlen);
		nchars_cpy(sdret.m_buf, this->m_buf, m_nchars);
		nchars_cpy(sdret.m_buf+m_nchars, tail, ntail);
		return sdret;
	}

	sdring& append_self(const T_CHAR *tail, int ntail=-1)
	{
		*this = this->append(tail, ntail);
		return *this;
	}

	//////////////////////////////
	//    Facility functions   <<<
	//////////////////////////////

	//
	// Place private members at end.
	//
private:
	int m_nchars;
	T_CHAR* m_buf; // buffer size is m_nchars+1

private:
	void _ct0r() 
	{
		m_buf = nullptr;
		m_nchars = 0;
	}
	void _dtor()
	{
		delete[] m_buf;
	}
};


#if defined(UNICODE) || defined(_UNICODE)
typedef sdring<wchar_t> Sdring;
#else
typedef sdring<char> Sdring;
#endif


//////////////////////////////
//    Facility functions   >>>
//////////////////////////////




#endif
