#ifndef __CHHI__SimpleIni_h_
#define __CHHI__SimpleIni_h_
#define __CHHI__SimpleIni_h_created_ 20260416
#define __CHHI__SimpleIni_h_updated_ 20260419

#include <ps_TCHAR.h>

namespace iniop { class CIniOp; }

class SimpleIni
{
public:
	enum ReCode_et
	{
		E_Success = 0,
		E_Fail = -1,
		
		E_FileNotFound = -5,
		E_FileIo = -6, 
	};

public:
	// boilerplate code, no need to modify >>>
	SimpleIni() { _ct0r(); }          //////////////
	~SimpleIni()                      //////////////
	{                                 //////////////
		_dtor();                      //////////////
		_ct0r();                      //////////////
	}                                 //////////////
	SimpleIni(const SimpleIni& old)        // copy-ctor
	{                                      /////////////
		_copy_from_old(old);               /////////////
	}                                      /////////////
	SimpleIni& operator=(const SimpleIni& old) // copy-assign
	{                                      //////////////
		if (this != &old) {                //////////////
			_dtor();                       //////////////
			_copy_from_old(old);           //////////////
		}                                  //////////////
		return *this;                      //////////////
	}                                      //////////////
	SimpleIni(SimpleIni&& old)        // move-ctor
	{                                 //////////////
		_steal_from_old(old);         //////////////
		old._ct0r();                  //////////////
	}                                 //////////////
	SimpleIni& operator=(SimpleIni&& old) // move-assign
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
	void _copy_from_old(const SimpleIni& old);

	void _steal_from_old(SimpleIni& old) {
		m_pi = old.m_pi;
	}

	bool _create_impl();

public:
	static int getversion();

	ReCode_et read(const TCHAR *inifilename);

	//
	// Leave below at end of class body
	//
private: // data members
	iniop::CIniOp *m_pi;

private:
	void _ct0r();
	void _dtor();
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


#if defined(SimpleIni_IMPL) || (defined CHHI_ALL_IMPL && !defined CHHI_ALL_IMPL_HIDE_SimpleIni) // [IMPL]


// >>> Include headers required by this lib's implementation

#include <commdefs.h> // for Uint, Uint64, enum bitwise-OR etc
#include <snTprintf.h>
#include <fsapi.h>
#include <ospath.h>
#include <EnsureClnup_misc.h>
#include <sdring.h>
#include <hashdict.h>
#include <utf8utils.h>
#include <makeTsdring.h>
#include <StringHelper.h>

// <<< Include headers required by this lib's implementation

#ifndef SimpleIni_DEBUG
#include <CHHI_vaDBG_hide.h> // Suppress/invalidate vaDBG macros, from now on
#endif

/*
    ____       _             __                                                                
   / __ \_____(_)   ______ _/ /____       ____  ____ _____ ___  ___  _________  ____ _________ 
  / /_/ / ___/ / | / / __ `/ __/ _ \     / __ \/ __ `/ __ `__ \/ _ \/ ___/ __ \/ __ `/ ___/ _ \
 / ____/ /  / /| |/ / /_/ / /_/  __/    / / / / /_/ / / / / / /  __(__  ) /_/ / /_/ / /__/  __/
/_/   /_/  /_/ |___/\__,_/\__/\___/    /_/ /_/\__,_/_/ /_/ /_/\___/____/ .___/\__,_/\___/\___/ 
                                                                      /_/                      
*/
// We should enclose this lib's implementation into private namespace.

////////////////////////////////////////////////////////////////////////////
namespace iniop {
////////////////////////////////////////////////////////////////////////////
// (private namespace 'iniop') 


using namespace chjds; // hashdict in `chjds`
using namespace fsapi; // from fsapi.h
using namespace ospath; // from ospath.h

class CIniOp
{
public:
	// boilerplate code, no need to modify >>>
	CIniOp() { _ct0r(); }            //////////////
	virtual ~CIniOp()                //////////////
	{                                //////////////
		_dtor();                     //////////////
		_ct0r();                     //////////////
	}                                //////////////
	CIniOp(const CIniOp& old)              // copy-ctor
	{                                      //////////////
		_copy_from_old(old);               //////////////
	}                                      //////////////
	CIniOp& operator=(const CIniOp& old)   // copy-assign
	{                                      //////////////
		if (this != &old) {                //////////////
			_dtor();                       //////////////
			_copy_from_old(old);           //////////////
		}                                  //////////////
		return *this;                      //////////////
	}                                      //////////////
	CIniOp(CIniOp&& old)              // move-ctor
	{                                 //////////////
		_steal_from_old(old);         //////////////
		old._ct0r();                  //////////////
	}                                 //////////////
	CIniOp& operator=(CIniOp&& old)   // move-assign
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
	void _copy_from_old(const CIniOp& old) {
		m_inidict = old.m_inidict;
	}

	void _steal_from_old(CIniOp& old) {
		m_inidict = std::move(old.m_inidict);
	}

public:
	// ... more public methods here ...

#define COPY_SimpleIni_ReCode(errname) errname = SimpleIni::errname
	enum ReCode_et
	{
		COPY_SimpleIni_ReCode(E_Success),
		COPY_SimpleIni_ReCode(E_Fail),

		COPY_SimpleIni_ReCode(E_FileNotFound),
		COPY_SimpleIni_ReCode(E_FileIo),
	};

	ReCode_et read(const TCHAR *inifilepath);

private:
	ReCode_et read_initext(const TCHAR *initext, int inilen);


	//
	// Leave below at end of class body
	//
private: // data members
	bool m_isUtf8;
	
	Sdring m_inipath;
	Sdring m_inifilenam;
	const TCHAR *m_pfilenam;

	hashdict< hashdict<Sdring> > m_inidict;

private:
	void _ct0r() {
		m_isUtf8 = false;
		m_pfilenam = nullptr;
	}

	void _dtor() {
		// nothing
	}
};


CIniOp::ReCode_et
CIniOp::read(const TCHAR *inifilepath)
{
	m_inidict.clear();

	CEC_filehandle_t fh = file_open(inifilepath, open_for_read, 
		open_share_read|open_share_write);

	int filesize = (int)file_getsize(fh);
	if(filesize==0)
		return E_Success; // consider it an empty INI file
	else if(filesize<0)
		return E_FileIo;

	sdring<char> filebin(filesize);

	int bytesRed = file_read(fh, filebin.getptr(), filesize);
	if(bytesRed!=filesize)
		return E_FileIo;

	int invalid_offset = (int)utf8::find_invalid_utf8seq(filebin, filesize);

	m_isUtf8 = invalid_offset>=0 ? false : true;

	Sdring initext = makeTsdring(std::move(filebin), m_isUtf8 ? mTs_UTF8 : mTs_SysDefault);

	m_inipath = inifilepath;
	split(inifilepath, m_inifilenam);
	m_pfilenam = m_inifilenam.c_str();

	return read_initext(initext, initext.rawlen());
}


inline bool Is_leading_blank(int charval)
{
	return (charval == ' ' || charval == '\t') ? true : false;
}

inline bool Is_equal_sign(int charval)
{
	return charval=='=' ? true : false;
}

inline bool IsSplitLf(int charval)
{
	return charval=='\n' ? true : false;
}

inline bool IsTrimCr(int charval)
{
	return charval=='\r' ? true : false;
}

struct KVcontinue // KeyVal line continuation info
{
	bool is_prevline_keyval;
//	int vals; // extra value lines accumulated
//	int cmts; // extra comment lines accumulated

	void reset() { is_prevline_keyval=false; /*vals=cmts=0;*/ }
};

using SdringVal = Sdring; // SdringVal imply this is for INI key's value.

CIniOp::ReCode_et
CIniOp::read_initext(const TCHAR *initext, int inilen)
{
	// Prepare a virtual section("_start_" to hold comments at file start.
	Sdring curSection = _T("_start_");
	Sdring curKey_fr; // fr: friendly key, not in form "keyfoo#1" or "keyfoo;1"

	hashdict<SdringVal> dict_key_val; // an empty key-val dict
	m_inidict.set(curSection, dict_key_val);

	KVcontinue kvc = {};

	// Split initext into lines, process them line-by-line.

	StringSplitter<const TCHAR*, IsSplitLf, IsTrimCr, true> 
		spgline(initext, 0, inilen);
	// -- spgline: split to get line(s)

	for (int iline=0;;)
	{
		int linelen = 0;
		int linepos = spgline.next(&linelen);
		if(linepos==-1)
			break;

		iline++;

		// First check if it is a key=val continuation line(start with a \t)

		if(kvc.is_prevline_keyval && initext[linepos]=='\t')
		{
			assert(!curKey_fr.is_empty());
			
			TCHAR keysuffix[8];
			snTprintf(keysuffix, _T("#%d"), iline);
			Sdring cont(&initext[linepos+1], linelen-1);
			
			Sdring innerKey = curKey_fr + keysuffix;

			vaDBG3(_T("{%s}L#%d See continuation line: '%s' = %s"), m_pfilenam,iline, innerKey.c_str(), cont.c_str());

			dict_key_val.set(innerKey, std::move(cont));

			kvc.is_prevline_keyval = true;
			continue;
		}

		// Skip blank chars at line start.

		int indents = 0;
		while(indents<linelen && Is_leading_blank(initext[linepos+indents]))
			indents++;

		if(indents == linelen) // meet an empty line
		{
			// To preserve user's empty line, we consider it a virtual empty comment line

			TCHAR keysuffix[8] = {};
			snTprintf(keysuffix, _T(";%d"), iline);
			Sdring innerKey = curKey_fr + keysuffix;

			vaDBG3(_T("{%s}L#%d Empty line: '%s' ="), m_pfilenam,iline, innerKey.c_str());

			dict_key_val.set(innerKey, Sdring());

			kvc.is_prevline_keyval = false;
			continue; 
		}

		Sdring linetext(initext + linepos + indents, linelen - indents);

		// Check if it is comment line (lead by ;)

		if(linetext[0]==';')
		{
			// Meet a comment line.
			// We add(assume) this as a virtual key-val pair into curSection.

			if(kvc.is_prevline_keyval)
			{
				TCHAR keysuffix[8] = {};
				snTprintf(keysuffix, _T(";%d"), iline);
				Sdring innerKey = curKey_fr + keysuffix;

				vaDBG3(_T("{%s}L#%d See embedded comment line: '%s' = %s"), m_pfilenam,iline, innerKey.c_str(), linetext.c_str());

				dict_key_val.set(innerKey, std::move(linetext));
			}
			else
			{
				TCHAR cmtkey[8] = {};
				snTprintf(cmtkey, _T(";%d"), iline);

				vaDBG3(_T("{%s}L#%d See standalone comment line: '%s' = %s"), m_pfilenam,iline, cmtkey, linetext.c_str());
				
				dict_key_val.set(cmtkey, std::move(linetext));
			}

			// note: kvc.is_prevline_keyval no change
			continue;
		}

		// Check if it is section name [some_section_name]
		
		Sdring linetrimd = linetext.trim(_T(" \t"), 2);
		if(linetrimd[0]=='[' && linetrimd[-1]==']')
		{
			curSection = linetrimd.trim(_T("[]"), 2);
			m_inidict.setdefault(curSection, hashdict<SdringVal>());

			vaDBG3(_T("{%s}L#%d See section line: [%s]"), m_pfilenam,iline, curSection.c_str());

			curKey_fr = nullptr;
			kvc.reset();
			continue;
		}

		// Check if it is key=value pair

		int eqs_pos = linetext.findchar('=');
		if(eqs_pos>=0)
		{
			StringSplitter<const TCHAR*, Is_equal_sign, StringSplitter_IsSpaceTab>
				spgkeyval(linetext, 0, linetext.rawlen());
			
			int keylen = 0;
			int keypos = spgkeyval.next(&keylen);
			assert(keypos == 0);
			curKey_fr = Sdring(&linetext[keypos], keylen);

			int len2 = 0;
			int valpos = spgkeyval.next(&len2);
			int vallen = 0;
			if(valpos>=0) // can be -1 if val is empty
				vallen = linetext.rawlen() - valpos;
			
			Sdring sd_val(&linetext[valpos], vallen);

			vaDBG3(_T("{%s}L#%d See key-val line: '%s' = %s"), m_pfilenam,iline, curKey_fr.c_str(), sd_val.c_str());

			dict_key_val.set(&linetext[keypos], std::move(sd_val));

			kvc.is_prevline_keyval = true;
			continue;
		}

		// An invalid line is encountered, tap a log.

		vaDBG1(_T("{%s}L#%d Meet invalid INI line: %s"), m_pfilenam,iline, linetext.c_str());
		kvc.reset();
	}

	return E_Success;
}





////////////////////////////////////////////////////////////////////////////
} // namespace iniop
////////////////////////////////////////////////////////////////////////////



/*
 ,-. .     .       .                          ,.  ;-.  ,
/    |     |       |                         /  \ |  ) |
| -. | ,-. |-. ,-: |   ,-. ;-. ,-: ,-. ,-.   |--| |-'  |
\  | | | | | | | | |   `-. | | | | |   |-'   |  | |    |
 `-' ' `-' `-' `-` '   `-' |-' `-` `-' `-'   '  ' '    '
*/
// Global space API implementation wrapper:


int SimpleIni::getversion() 
{ 
	return 1; 
}

void SimpleIni::_copy_from_old(const SimpleIni& old)
{
	m_pi = new iniop::CIniOp(*old.m_pi);
}

void SimpleIni::_ct0r()
{
	m_pi = nullptr;
}
void SimpleIni::_dtor() 
{
	delete m_pi;
}

bool SimpleIni::_create_impl()
{
	if(m_pi)
		return true;
	else
	{ 
		m_pi = new iniop::CIniOp;
		return m_pi ? true: false;
	}
}

SimpleIni::ReCode_et 
SimpleIni::read(const TCHAR *inifilename)
{
	if(!_create_impl())
		return E_Fail;

	return (ReCode_et)m_pi->read(inifilename);
}


//
//
//

#ifndef SimpleIni_DEBUG
#include <CHHI_vaDBG_show.h> // Now restore vaDBG macros
#endif


#endif // [IMPL]

#endif // include once guard
