#ifndef __CHHI__SimpleIni_h_
#define __CHHI__SimpleIni_h_
#define __CHHI__SimpleIni_h_created_ 20260416
#define __CHHI__SimpleIni_h_updated_ 20260429

#include <ps_TCHAR.h>
#include <sdring.h>

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

	static int getversion();

	ReCode_et read(const TCHAR *inifilename);

	Sdrings sections();

	bool has_section(const TCHAR *section_name);
	Sdrings section_keys(const TCHAR *section_name);

	bool has_key(const TCHAR *section_name, const TCHAR *keyname);
	Sdring get(const TCHAR *section_name, const TCHAR *keyname);
	// -- To distinguish {no-key} and {has-key with empty value}, user need to call has_key().

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
#include <TScalableArray.h>

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

const TCHAR *VIRTUAL_SECTION_0 = _T("_start_");
const int KEYVAL_TSA_INC = 100;

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

	Sdrings sections();

	bool has_section(const TCHAR *secname);
	Sdrings section_keys(const TCHAR *secname);

	bool has_key(const TCHAR *secname, const TCHAR *keyname);
	Sdring get(const TCHAR *secname, const TCHAR *keyname);
	// -- To distinguish {no-key} and {has-key with empty value}, user need to call has_key().

private:
	ReCode_et read_initext(const TCHAR *initext, int inilen);

private:
	using SdringVal = Sdring; // SdringVal imply this is for INI key's value.

	//
	// Leave below at end of class body
	//
private: // data members
	bool m_isUtf8;
	
	Sdring m_inipath;
	Sdring m_inifilenam;
	const TCHAR *m_pfilenam;

	hashdict< hashdict<SdringVal> > m_inidict;

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

CIniOp::ReCode_et
CIniOp::read_initext(const TCHAR *initext, int inilen)
{
	// Prepare a virtual section("_start_" to hold comments at file start.

	// create empty [_start_] kvdict.
	Sdring curSection = VIRTUAL_SECTION_0;
	m_inidict.set(VIRTUAL_SECTION_0, hashdict<SdringVal>());

	hashdict<SdringVal> *pCurKvdict = m_inidict.get(VIRTUAL_SECTION_0);

	Sdring curKey_fr; // fr: friendly key, not in form "keyfoo#1" or "keyfoo;1"
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

		hashdict<SdringVal> &kvdict = *pCurKvdict; // make a short name

		// First check if it is a key=val continuation line(start with a \t)

		if(kvc.is_prevline_keyval && initext[linepos]=='\t')
		{
			assert(!curKey_fr.is_empty());
			
			TCHAR keysuffix[8];
			snTprintf(keysuffix, _T("#%d"), iline);
			Sdring cont(&initext[linepos+1], linelen-1);
			
			Sdring innerKey = curKey_fr + keysuffix;

			vaDBG3(_T("{%s}L#%d See continuation line: '%s' = %s"), m_pfilenam,iline, innerKey.c_str(), cont.c_str());

			kvdict.set(innerKey, std::move(cont));

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

			kvdict.set(innerKey, Sdring());

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

				kvdict.set(innerKey, std::move(linetext));
			}
			else
			{
				TCHAR cmtkey[8] = {};
				snTprintf(cmtkey, _T(";%d"), iline);

				vaDBG3(_T("{%s}L#%d See standalone comment line: '%s' = %s"), m_pfilenam,iline, cmtkey, linetext.c_str());
				
				kvdict.set(cmtkey, std::move(linetext));
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
			pCurKvdict = m_inidict.get(curSection);

			continue;
		}

		// Now it is probably a new key=value line.

		int eqs_pos = linetext.findchar('=');
		if(eqs_pos>=0)
		{
			StringSplitter<
				TCHAR* const, // MUST NOT write `TCHAR const*`, would crash on GCC (eg. gcc 9.4)
				Is_equal_sign, StringSplitter_IsSpaceTab
				>
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

			kvdict.set(curKey_fr, std::move(sd_val));

			kvc.is_prevline_keyval = true;
			continue;
		}

		// An invalid line is encountered, tap a log.

		vaDBG1(_T("{%s}L#%d Meet invalid INI line: %s"), m_pfilenam,iline, linetext.c_str());
		kvc.reset();
	}

	return E_Success;
}


Sdrings CIniOp::sections()
{
	int count = m_inidict.keycount() - 1;
	// -1 to remove VIRTUAL_SECTION_0 [_start_]

	assert(count>=0);
	if(count==0)
		return Sdrings();

	Sdrings rets(count);

	auto enumor = m_inidict.get_enumor();
	const TCHAR *key = enumor.next();
	assert(Sdring::str_match(key, VIRTUAL_SECTION_0));

	int i=0;
	for (;; i++)
	{
		assert(i<=count);

		key = enumor.next();
		if(!key)
			break;

		rets[i] = key;
	}

	assert(i==count);
	return rets;
}

bool CIniOp::has_section(const TCHAR *secname)
{
	if (!secname || !secname[0])
		return false;

	hashdict<SdringVal> *p_kvdict = m_inidict.get(secname);
	if(p_kvdict)
		return true;
	else
		return false;
}


struct rawkey_st
{
	int frlen;      // friendly-key len
	TCHAR splitter; // # or ; or \0
	int numsuffix;

	rawkey_st(const TCHAR* rawkey)
	{
		// sth like 'keym#21' will be split into 'keym', '#', 21 .

		const TCHAR *psharp = _tcschr(rawkey, '#');
		const TCHAR *psemco = _tcschr(rawkey, ';');
		if (!psharp && !psemco)
		{
			frlen = -1;
			splitter = '\0';
			numsuffix = 0;
			return;
		}

		if(psemco)
		{
			frlen = psemco - rawkey;
			splitter = ';';
			numsuffix = _ttoi(psemco+1);
		}
		else
		{
			frlen = psharp - rawkey;
			splitter = '#';
			numsuffix = _ttoi(psharp+1);
		}
	}
};

Sdrings CIniOp::section_keys(const TCHAR *secname)
{
	if(!has_section(secname))
		return Sdrings();

	hashdict<SdringVal> &kvdict = *m_inidict.get(secname);

	auto enumor = kvdict.get_enumor();
	const TCHAR *rawkey = nullptr;

	int count_fr = 0; // count of friendly keys
	for(;;)
	{
		rawkey = enumor.next();
		if(!rawkey)
			break;

		rawkey_st rks(rawkey);
		if (rks.numsuffix == '\0')
			count_fr++;
	}

	if(count_fr==0)
		return Sdrings();

	Sdrings rets(count_fr);
	enumor.reset();
	int i=0;
	for(;;)
	{
		rawkey = enumor.next();
		if (!rawkey)
			break;

		rawkey_st rks(rawkey);
		if (rks.numsuffix == '\0')
			rets[i++] = rawkey;
	}

	return rets;
}


bool CIniOp::has_key(const TCHAR *secname, const TCHAR *keyname)
{
	hashdict<SdringVal> *psec = m_inidict.get(secname);
	if(!psec)
		return false;

	Sdring *pval = psec->get(keyname);
	if(!pval)
		return false;

	return true;
}

inline void EasyDebug_AppendNUL(TScalableArray<TCHAR>& sout)
{
	if (sout.CurrentEles() < sout.CurrentStorage())
		sout.GetElePtr()[sout.CurrentEles()] = '\0';
}

Sdring CIniOp::get(const TCHAR *secname, const TCHAR *keyname)
{
	hashdict<SdringVal> *psec = m_inidict.get(secname);
	if (!psec)
		return Sdring();

	/* Example: for an ini key like this(key='keym'):

keym = 
	val line one
;	1st embedded comment line in key's value
	val line two
;	2nd embedded comment line in key's value
	val line three

	The enumor will see these raw keys (actual number suffix may vary):

	keym    =
	keym#17 = val line one
	keym;18 = ;	1st embedded comment line in key's value
	keym#19 = val line two
	keym;20 = ;	2nd embedded comment line in key's value
	keym#21 = val line three
	keym;22 =
	...
	keyN    = ...

	Only after we see keyN appears, we know that keym's value has ended.

	*/

	//	int val_lines_accum = 0;
	TScalableArray<TCHAR> sout(INT32_MAX, KEYVAL_TSA_INC, KEYVAL_TSA_INC);

	auto enumor = psec->get_enumor(keyname);

	Sdring* pval = nullptr;
	const TCHAR *rawkey = enumor.next(&pval);
	if(!rawkey)
		return Sdring();

	sout.AppendTail(pval->getptr(), pval->rawlen());
	EasyDebug_AppendNUL(sout);

	int accu_lines = 1; // debug purpose

	for(;; accu_lines++)
	{
		Sdring* pval = nullptr;
		rawkey = enumor.next(&pval);
		if(!rawkey)
			break;

		rawkey_st rks(rawkey);
		
		if(rks.splitter=='\0') // meet a different key
			break;
		else if(rks.splitter=='#')
		{
			sout.AppendTail('\n');
			sout.AppendTail(pval->getptr(), pval->rawlen());
			EasyDebug_AppendNUL(sout);
		}
		else
			assert(rks.splitter==';');
	}

	const TCHAR *psz = sout.GetElePtr();
	int szlen = sout.CurrentEles();
	return Sdring(psz, szlen);
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

Sdrings SimpleIni::sections()
{
	if (!_create_impl())
		return E_Fail;

	return m_pi->sections();
}

bool SimpleIni::has_section(const TCHAR *section_name)
{
	if (!_create_impl())
		return false;

	return m_pi->has_section(section_name);
}

Sdrings SimpleIni::section_keys(const TCHAR *section_name)
{
	if (!_create_impl())
		return Sdrings();

	return m_pi->section_keys(section_name);
}

bool SimpleIni::has_key(const TCHAR *section_name, const TCHAR *keyname)
{
	if (!_create_impl())
		return false;

	return m_pi->has_key(section_name, keyname);
}

Sdring SimpleIni::get(const TCHAR *section_name, const TCHAR *keyname)
{
	if (!_create_impl())
		return false;

	return m_pi->get(section_name, keyname);
}

//
//
//

#ifndef SimpleIni_DEBUG
#include <CHHI_vaDBG_show.h> // Now restore vaDBG macros
#endif


#endif // [IMPL]

#endif // include once guard
