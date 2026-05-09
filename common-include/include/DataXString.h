#ifndef __CHHI__DataXString_h_
#define __CHHI__DataXString_h_
#define __CHHI__DataXString_h_created_ 20260507
#define __CHHI__DataXString_h_updated_ 20260509

// DataXString: C++ object Data eXchange via/by the form of a String.

#include <utility>

#include <CxxVerCheck.h>
#include <ps_TCHAR.h>
#include <sdring.h>
#include <StringHelper.h>

////////////////////////////////////////////////////////////////////////////
//-- namespace datax { 
////////////////////////////////////////////////////////////////////////////
// Place API function declarations in this namespace.


class IDataXString
{
public:
	IDataXString() { m_is_dirty = false; }
	virtual ~IDataXString() {}

	virtual void   SetValueByString(const TCHAR *valsz, bool is_mark_dirty=true) = 0;
	virtual Sdring GetValueAsString() = 0;

	virtual void SetToDefault() = 0;

	void SetDirty(bool is_dirty) { m_is_dirty = is_dirty; }
	bool IsDirty() { return m_is_dirty; }

	const TCHAR* GetDefault() { return m_default.c_str(); }

private:
	bool m_is_dirty;
protected:
	Sdring m_default;
};


// Declare two-parameter DataXTraits(primary template), the second has a default.
struct XStringFormatDefault {};
template<typename TU, typename FORMAT=XStringFormatDefault> struct DataXTraits;


template<typename TU, typename FORMAT=XStringFormatDefault>
class DataXString : public IDataXString
{
#ifdef CXX11_OR_NEWER
	static_assert(std::is_copy_constructible<TU>::value,
		"TU must be copy constructible");

	static_assert(std::is_move_constructible<TU>::value,
		"TU must be move constructible");
#endif

public:
	DataXString(const TCHAR* default=nullptr)
	{
		m_default = default;
		SetToDefault();
	}

	enum SetValue_ret { NoChange = 0, SetNew = 1 };

	virtual SetValue_ret SetValue(TU&& val)
	{
		if(val==m_val)
			return NoChange;

		SetDirty(true);
		m_val = std::move(val);
		return SetNew;
	}

	SetValue_ret SetValue(const TU& val)
	{
		// No need to make this virtual, bcz virtual SetValue(TU&& val) is enough.
		TU copy = val;
		return this->SetValue( std::move(copy) );
	}

	DataXString& operator= (const TU& val)
	{
		SetValue(val);
		return *this;
	}

	virtual const TU& GetValue()
	{
		return m_val;
	}

	operator const TU&()
	{
		return GetValue();
	}

	virtual void SetValueByString(const TCHAR *valsz, bool is_mark_dirty=true) cxx11_override
	{
		m_val = DataXTraits<TU, FORMAT>::FromString(valsz);
		SetDirty(is_mark_dirty);
	}

	virtual Sdring GetValueAsString() cxx11_override
	{
		return DataXTraits<TU, FORMAT>::ToString(m_val);
	}

	virtual void SetToDefault() cxx11_override
	{
		m_val = DataXTraits<TU, FORMAT>::FromString(m_default.c_str());
	}

private:
	TU m_val;
};



//////////////////////////////////////////////////////////////////////////
// Specialization for common C++ types(int, bool, float, etc)
//////////////////////////////////////////////////////////////////////////

template<>
struct DataXTraits<int, XStringFormatDefault>
{
	static int FromString(const TCHAR* s)
	{
		if(!s)
			return 0;

		return _tcstol(s, nullptr, 0);
	}

	static Sdring ToString(int val)
	{
		TCHAR sz[32];
		snTprintf(sz, _T("%d"), val);
		return Sdring(sz);
	}
};

struct XString0xHex {};

template<>
struct DataXTraits<int, XString0xHex>
{
	static int FromString(const TCHAR* s)
	{
		if(!s)
			return 0;

		// Value from a string, we still recognize (no-prefix)decimal 
		// as well as 0x... hexadecimal.
		return _tcstol(s, nullptr, 0);
	}

	static Sdring ToString(int val)
	{
		// Always output string in 0x... form.
		TCHAR sz[32];
		snTprintf(sz, _T("0x%X"), val);
		return Sdring(sz);
	}
};

template<typename FORMAT>
struct DataXTraits<bool, FORMAT>
{
	static bool FromString(const TCHAR* s)
	{
		if(!s)
			return false;

		if( shp_stricmp(s, _T("true"))==0 || (s[0]=='1' && s[1]=='\0') )
			return true;
		else
			return false;
	}

	static Sdring ToString(bool val)
	{
		return val==true ? _T("true") : _T("false");
	}
};


template<typename FORMAT>
struct DataXTraits<float, FORMAT>
{
	static float FromString(const TCHAR* s)
	{
		if(!s)
			return 0;

		return (float)_ttof(s);
	}

	static Sdring ToString(float val)
	{
		TCHAR sz[32];
		snTprintf(sz, _T("%g"), val);
		return Sdring(sz);
	}
};

template<typename FORMAT>
struct DataXTraits<double, FORMAT>
{
	static double FromString(const TCHAR* s)
	{
		if(!s) 
			return 0;
		
		return _ttof(s);
	}

	static Sdring ToString(double val)
	{
		TCHAR sz[32];
		snTprintf(sz, _T("%g"), val);
		return Sdring(sz);
	}
};


////////




////////////////////////////////////////////////////////////////////////////
//-- } // namespace datax
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


#if defined(DataXString_IMPL) || (defined CHHI_ALL_IMPL && !defined CHHI_ALL_IMPL_HIDE_DataXString) // [IMPL]


// >>> Include headers required by this lib's implementation

//#include <commdefs.h> // for Uint, Uint64, enum bitwise-OR etc
//#include <snTprintf.h>

// <<< Include headers required by this lib's implementation




#ifndef DataXString_DEBUG
#include <CHHI_vaDBG_hide.h> // Suppress/invalidate vaDBG macros, from now on
#endif


////////////////////////////////////////////////////////////////////////////
//-- namespace datax {
////////////////////////////////////////////////////////////////////////////
// Place API function Implementation in this namespace.


int getversion()
{
	return 1;
}


////////////////////////////////////////////////////////////////////////////
//-- } // namespace datax
////////////////////////////////////////////////////////////////////////////



#ifndef DataXString_DEBUG
#include <CHHI_vaDBG_show.h> // Now restore vaDBG macros
#endif


#endif // [IMPL]


#endif // include once guard
