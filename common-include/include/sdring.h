#ifndef __sdring_h_20251225_
#define __sdring_h_20251225_

//#include <ps_TCHAR.h>

template<typename T_CHAR>
class sdring
{
public:

	static int bufSize_(int nchars){ return nchars+1; }

	sdring(int nchars=0)
	{
		m_nchars = 0;
		m_buf = nullptr;

		if(nchars>0)
		{
			m_buf = new T_CHAR[bufSize_(nchars)];
			m_buf[0] = '\0';
		}
	}

	sdring(const T_CHAR* instr)	{
		_ctor(instr);
	}

	sdring(const sdring& ins) {
		_ctor(ins.get());
	}

	sdring(sdring&& old) // move-ctor
	{
		_steal_from_old(old);
	}

	sdring& operator=(sdring&& old) // move-assign
	{
		if (this != &old) {
			delete this->m_buf;
			_steal_from_old(old);
		}
		return *this;
	}

	~sdring() 
	{
		delete m_buf;
		m_buf = nullptr;
		m_nchars = 0;
	}

	const T_CHAR* get() const { 
		return m_buf;
	}

	T_CHAR* getbuf() {
		return m_buf;
	}

	T_CHAR* c_str() {
		return m_buf;
	}

	sdring& operator=(const TCHAR *instr)
	{
		delete m_buf;
		_ctor(instr);
		return *this;
	}

	sdring& operator=(const sdring& ins)
	{
		delete m_buf;
		_ctor(ins);
		return *this;
	}

	T_CHAR& operator[](int pos) {
		return m_buf[pos];
	}

	const T_CHAR& operator[](int pos) const {
		return m_buf[pos];
	}

	operator T_CHAR*() {
		return m_buf;
	}

	operator const T_CHAR*() const {
		return m_buf;
	}

private:
	void _ctor(const T_CHAR *instr)
	{
		// [2025-12-25] Note: We distinguish btw 
		//	instr==NULL 
		// and
		//	instr[0]=='\0'

		m_nchars = 0;
		m_buf = nullptr;

		if (instr)
		{
			m_nchars = str_len(instr); // m_nchars can be 0

			m_buf = new T_CHAR[bufSize_(m_nchars)];
			str_cpy(m_buf, instr);
		}
	}

	void _steal_from_old(sdring& old)
	{
		this->m_buf = old.m_buf;
		this->m_nchars = old.m_nchars;

		old.m_buf = nullptr;
		old.m_nchars = 0;
	}

	static int str_len(const T_CHAR s[])
	{
		int slen = 0;
		while(s[slen])
			slen++;
		return slen;
	}

	static int str_cpy(T_CHAR dst[], const T_CHAR src[])
	{
		int i = 0;
		for(; src[i]; i++)
			dst[i] = src[i];
		dst[i] = '\0'; // append NUL char
		return i; // i is string len
	}

private:
	int m_nchars;
	T_CHAR* m_buf; // buffer size is m_nchars+1
};


typedef sdring<TCHAR> Sdring;


#endif
