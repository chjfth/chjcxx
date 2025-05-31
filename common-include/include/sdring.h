#ifndef __sdring_h_20250512_
#define __sdring_h_20250512_

template<typename T_CHAR>
class sdring
{
public:

	static int bufSize(int nchars){ return nchars+1; }

	sdring(int nchars=0)
	{
		m_nchars = 0;
		m_buf = nullptr;

		if(nchars>0)
			m_buf = new T_CHAR[bufSize(nchars)];
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
		if (this != old) {
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
		m_nchars = 0;
		m_buf = nullptr;

		if (instr)
			m_nchars = (int)_tcslen(instr);

		if (m_nchars > 0)
		{
			m_buf = new T_CHAR[bufSize(m_nchars)];
			_tcscpy_s(m_buf, bufSize(m_nchars), instr);
		}
	}

	void _steal_from_old(sdring& old)
	{
		this->m_buf = old.m_buf;
		this->m_nchars = old.m_nchars;

		old.m_buf = nullptr;
		old.m_nchars = 0;
	}

private:
	int m_nchars;
	T_CHAR* m_buf; // buffer size is m_nchars+1
};



#endif
