#ifndef __CHHI__SimpleIniEx_h_
#define __CHHI__SimpleIniEx_h_
#define __CHHI__SimpleIniEx_h_created_ 20260504
#define __CHHI__SimpleIniEx_h_updated_ 20260515

#include <SimpleIni.h>

class SimpleIniEx : public SimpleIni
{
public:
	bool load_cascade(const TCHAR* const ar_inifiles[], int nfiles);

	bool save_cascade(Sdring *p_out_inipath=nullptr);

public:
	// boilerplate code, no need to modify >>>
	SimpleIniEx() { _ct0r(); }        //////////////
	virtual ~SimpleIniEx()            //////////////
	{                                 //////////////
		_dtor();                      //////////////
		_ct0r();                      //////////////
	}                                 //////////////
	SimpleIniEx(const SimpleIniEx& old)    // copy-ctor
	{                                      //////////////
		_copy_from_old(old);               //////////////
	}                                      //////////////
	SimpleIniEx& operator=(const SimpleIniEx& old) // copy-assign
	{                                      //////////////
		if (this != &old) {                //////////////
			_dtor();                       //////////////
			_copy_from_old(old);           //////////////
		}                                  //////////////
		return *this;                      //////////////
	}                                      //////////////
	SimpleIniEx(SimpleIniEx&& old)    // move-ctor
	{                                 //////////////
		_steal_from_old(old);         //////////////
		old._ct0r();                  //////////////
	}                                 //////////////
	SimpleIniEx& operator=(SimpleIniEx&& old) // move-assign
	{                                 //////////////
		if (this != &old) {           //////////////
			_dtor();                  //////////////
			_steal_from_old(old);     //////////////
			old._ct0r();              //////////////
		}                             //////////////
		return *this;                 //////////////
	}                                 //////////////
	// boilerplate code, no need to modify <<<

private:
	void _copy_from_old(const SimpleIniEx& old) {
	}

	void _steal_from_old(SimpleIniEx& old) {
	}

public:
	// ... more public methods here ...

	//
	// Leave below at end of class body
	//
private: // data members
	Sdrings m_inifiles;
	int m_idxSave;   // which file in m_inifiles[] to save?

private:
	void _ct0r() {
		m_idxSave = 0;
	}

	void _dtor() {
	}
};



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


#if defined(SimpleIniEx_IMPL) || (defined CHHI_ALL_IMPL && !defined CHHI_ALL_IMPL_HIDE_SimpleIniEx) // [IMPL]


// >>> Include headers required by this lib's implementation

#include <commdefs.h> // for Uint, Uint64, enum bitwise-OR etc
#include <ospath.h>

// <<< Include headers required by this lib's implementation


#ifndef SimpleIniEx_DEBUG
#include <CHHI_vaDBG_hide.h> // Suppress/invalidate vaDBG macros, from now on
#endif

bool SimpleIniEx::load_cascade(const TCHAR* const ar_inifiles[], int nfiles)
{
	if(nfiles<=0)
		return false;

	m_inifiles.reset(nfiles);

	for(int i=0; i<nfiles; i++)
	{
		m_inifiles[i] = ospath::fullpath_from_rela(ar_inifiles[i]);

		vaDBG2(_T("SimpleIniEx::load_cascade(), try loading from (%d/%d) '%s'"), 
			i+1, nfiles, m_inifiles[i].c_str());

		auto err = load(ar_inifiles[i]);
		if(!err)
		{
			vaDBG2(_T(".   Load OK."));
			m_idxSave = i;
		}
		// Error case message: Rely on vaDBG() inside load().
	}

	return true;
}

bool SimpleIniEx::save_cascade(Sdring *p_out_inipath)
{
	DEFAULT_PTR_OUTPUT(p_out_inipath, nullptr)

	int idxStart = m_idxSave;

	// Try from the file by `m_idxSave` then downstream, until save() success.
	for(int i=m_idxSave; i<m_inifiles.count(); i++)
	{
		vaDBG2(_T("SimpleIniEx::save_cascade(), try saving to '%s'"), m_inifiles[i].c_str());

		// We set *p_out_inipath first. In case of fail, user knows which INI file finally fails.
		*p_out_inipath = m_inifiles[i];

		SimpleIni::ReCode_et err = save(m_inifiles[i]);
		if(!err)
		{ 
			vaDBG2(_T(".   Success."));

			m_idxSave = i; // update m_idxSave for next save() action.

			return true;
		}
	}

	vaDBG2(_T(".   All above %d files fail to save."), m_idxSave-idxStart+1);

	return false;
}



#ifndef SimpleIniEx_DEBUG
#include <CHHI_vaDBG_show.h> // Now restore vaDBG macros
#endif


#endif // [IMPL]


#endif // include once guard
