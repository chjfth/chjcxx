#ifndef __CHHI__LiveUic_h_20260313_
#define __CHHI__LiveUic_h_20260313_


// Include OS headers to provide OS-specific data-types used in API prototype.
// But do not include Implementation-code depending headers.
// Example:
// <windows.h> provides DWORD
// <unistd.h> provides pid_t, off64_t

#ifdef _WIN32
# include <windows.h>
#else // consider it Linux
# include <unistd.h>
#endif

#include <assert.h>

#include <_MINMAX_.h>

//
// Check current compiler 
//

#ifdef _MSC_VER
// ...
#else // consider it GNU GCC
// ...
#endif


////////////////////////////////////////////////////////////////////////////
namespace liveuic { 
////////////////////////////////////////////////////////////////////////////
// Place API function declarations in this namespace.

class LiveUic      // abstract base-class
{
	// LiveUic may be associated with an editbox, a checkbox etc.

public:
	~LiveUic() {}

	virtual void Reset() = 0;
	virtual void DataToUic() = 0;
	virtual void DataFromUic() = 0;
};


template<typename T>
struct Convert;

template<>
struct Convert<int>
{
	static int FromString(const TCHAR* s) { return _ttoi(s); }
	static void ToString(int val, TCHAR buf[], int bufsize) { 
		_sntprintf_s(buf, bufsize, _TRUNCATE, _T("%d"), val); 
	}
};

template<>
struct Convert<float>
{
	static float FromString(const TCHAR* s) { return (float)_ttof(s); }
	static void ToString(float val, TCHAR buf[], int bufsize) { 
		_sntprintf_s(buf, bufsize, _TRUNCATE, _T("%g"), val); 
	}
};

template<>
struct Convert<double>
{
	static double FromString(const TCHAR* s) { return _ttof(s); }
	static void ToString(double val, TCHAR buf[], int bufsize) { 
		_sntprintf_s(buf, bufsize, _TRUNCATE, _T("%g"), val); 
	}
};


template<typename T> // T as user data type in the editbox
class CEditValue : public LiveUic
{
	T m_val;

	T m_default_val, m_min_val, m_max_val;

	HWND m_hedit;

public:
	CEditValue()
	{
		m_val = 0;
		m_default_val = m_min_val = m_max_val = 0;
	}

	void Init(HWND hedit, T default_val, T min_val, T max_val)
	{
		assert(IsWindow(hedit));
		assert(max_val>=min_val);
		assert(default_val>=min_val && default_val<=max_val);

		m_hedit = hedit;

		m_val = default_val;
		m_default_val = default_val;
		m_min_val = min_val;
		m_max_val = max_val;
	}

	void SetData(T new_val)
	{
		m_val = _MID_(m_min_val, new_val, m_max_val);
	}

	T GetData()
	{
		return m_val;
	}

	virtual void Reset()
	{
		m_val = m_default_val;
		DataToUic();
	}

	virtual void DataToUic()
	{
		TCHAR buf[32] = {};
		Convert<T>::ToString(m_val, buf, ARRAYSIZE(buf));
		SetWindowText(m_hedit, buf);
	}

	virtual void DataFromUic()
	{
		TCHAR buf[32] = {};
		GetWindowText(m_hedit, buf, ARRAYSIZE(buf));
		m_val = Convert<T>::FromString(buf);
	}
};


////////////////////////////////////////////////////////////////////////////
} // namespace liveuic
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


#if defined(LiveUic_IMPL) || (defined CHHI_ALL_IMPL && !defined CHHI_ALL_IMPL_HIDE_LiveUic) // [IMPL]


// >>> Include headers required by this lib's implementation
#include <commdefs.h> // for Uint, Uint64, enum bitwise-OR etc
// <<< Include headers required by this lib's implementation




#ifndef LiveUic_DEBUG
#include <CHHI_vaDBG_hide.h> // Suppress/invalidate vaDBG macro, from now on
#endif


////////////////////////////////////////////////////////////////////////////
namespace liveuic {
////////////////////////////////////////////////////////////////////////////
// Place API function Implementation in this namespace.





////////////////////////////////////////////////////////////////////////////
} // namespace liveuic
////////////////////////////////////////////////////////////////////////////



#ifndef LiveUic_DEBUG
#include <CHHI_vaDBG_show.h> // Now restore vaDBG macro
#endif


#endif // [IMPL]


#endif // CHHI__LiveUic_h_20260313_
