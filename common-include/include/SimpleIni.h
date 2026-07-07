#ifndef __CHHI__SimpleIni_h_
#define __CHHI__SimpleIni_h_
#define __CHHI__SimpleIni_h_created_ 20260416
#define __CHHI__SimpleIni_h_updated_ 20260707

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
		
		E_FileNotExist = -5,
		E_FileIo = -6, 
		E_NoSuchKey = -7,

		// Internal errors:

		E_ClearHashdict = -11,
	};

	static int getversion();

	virtual ReCode_et load(const TCHAR *inifilename);
	// -- Multiple load() calls accumulate INI content. To avoid accumulate, clear() first.

	virtual ReCode_et clear();

	virtual Sdrings sections();

	virtual bool has_section(const TCHAR *section_name);
	virtual Sdrings section_keys(const TCHAR *section_name);

	virtual bool has_key(const TCHAR *section_name, const TCHAR *keyname);
	virtual Sdring get(const TCHAR *section_name, const TCHAR *keyname);
	// -- To distinguish {no-key} and {has-key with empty value}, user need to call has_key().

	virtual Sdring get_default(const TCHAR *section_name, const TCHAR *keyname,
		const TCHAR *default_value);

	virtual ReCode_et set(const TCHAR *section_name, const TCHAR *keyname, const TCHAR *value);

	virtual ReCode_et del_key(const TCHAR *section_name, const TCHAR *keyname);

	virtual ReCode_et load_initext(const TCHAR *initext, int inilen);

	virtual Sdring save_ini_string(const TCHAR *crlf=nullptr); // todo: rename to better name?
	// -- return whole INI string, as appear in INI file.

	virtual ReCode_et save(const TCHAR *savefilename=nullptr, const TCHAR *crlf=nullptr);

public:
	// boilerplate ctor/dtor code, no need to modify >>>
	SimpleIni()                       //////////////
	{                                 //////////////
		_ct0r();                      //////////////
		_init_once_after_ct0r();      //////////////
	}                                 //////////////
	virtual ~SimpleIni()              //////////////
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
	// boilerplate ctor/dtor code, no need to modify <<<

public:
	struct CtorFromChild {};
	SimpleIni(CtorFromChild)
	{
		_ct0r();
		// NOT calling _init_once_after_ct0r(), bcz we are constructing a derived class(CIniOp).
	}

private:
	void _ct0r();
	void _init_once_after_ct0r();
	void _dtor();
	void _copy_from_old(const SimpleIni& old);
	void _steal_from_old(SimpleIni& old) {
		m_pi = old.m_pi;
	}

	//
	// Leave below at end of class body
	//
private: // data members
	iniop::CIniOp *m_pi;

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

#include <string.h>

#include <CxxVerCheck.h>

#ifdef USING_REAL_GCC
#if __GNUC__ < 10
#error This file needs at least GCC 10.0 to compile. GCC 9.4 "returning reference to temporary" bug would crash the generated program.
// Even if GCC 9.4 survive CheckLineCategory()'s `TCHAR* const` site. There are more problematic places. (Evclip 20260505.s1)
#endif 
#endif

#include <msvc_extras.h>
#include <osdiff.h>
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
const TCHAR *VIRTUAL_KEYNAME_of_HeadupBlankLines = _T("_blanklines_");

const int KEYVAL_MULTILINE_MAX = 64*1024*1024; // max continuation lines
const int KEYVAL_MULTILINE_INC = 128; // roughly default to an INI line's text length

const int INIWRITE_MAX = 256*1024*1024; // whole ini file size
const int INIWRITE_INC = 32768; // each increase of write-ini buffer

struct Keval_st  // represent a INI-key(or virtual-key)'s value
{
	enum Type { 
		Comment=0, OneLine=1, _2Lines=2, _3Lines=3, _4Lines=4,

		Headup0Lines=-1, Headup1Lines=-2, // negative for VIRTUAL_KEYNAME_HEADUP_BLANK_LINES
	};

	union
	{
		Type type;
		int totlines; // this keval occupies multiple lines(>=2)
	};
	Sdring  firstline; // used when type=Comment or type=OneLine
	Sdring *ar_exlines; // array size indicated by `totlines-1`

	////
	static Type BlankLinesToTypeVal(int blanklines) {
		assert(blanklines >= 0);
		return (Type)(-(blanklines + 1));
	}
	static int TypeValToBlankLines(Type typeval) {
		assert(typeval < 0);
		return -(typeval + 1);
	}

public:
	// boilerplate code, no need to modify >>>
	Keval_st() { _ct0r(); }           //////////////
	virtual ~Keval_st()               //////////////
	{                                 //////////////
		_dtor();                      //////////////
		_ct0r();                      //////////////
	}                                 //////////////
	Keval_st(const Keval_st& old)            // copy-ctor
	{                                      //////////////
		_copy_from_old(old);               //////////////
	}                                      //////////////
	Keval_st& operator=(const Keval_st& old) // copy-assign
	{                                      //////////////
		if (this != &old) {                //////////////
			_dtor();                       //////////////
			_copy_from_old(old);           //////////////
		}                                  //////////////
		return *this;                      //////////////
	}                                      //////////////
	Keval_st(Keval_st&& old)          // move-ctor
	{                                 //////////////
		_steal_from_old(old);         //////////////
		old._ct0r();                  //////////////
	}                                 //////////////
	Keval_st& operator=(Keval_st&& old) // move-assign
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
	void _copy_from_old(const Keval_st& old) {
		totlines = old.totlines;
		firstline = old.firstline;
		ar_exlines = nullptr;
		if(old.ar_exlines)
		{
			const int FIRST_LINE_DONE = 1;
			ar_exlines = new Sdring[totlines-FIRST_LINE_DONE];
			for(int i=0; i<totlines- FIRST_LINE_DONE; i++)
			{
				ar_exlines[i] = old.ar_exlines[i];
			}
		}
	}

	void _steal_from_old(Keval_st& old) {
		totlines = old.totlines;
		firstline = std::move(old.firstline);
		ar_exlines = old.ar_exlines;
	}

private:
	void _ct0r() {
		totlines = 0;
		ar_exlines = nullptr;
	}

	void _dtor() {
		delete[] ar_exlines;
	}
};

struct BlankLines_St // consecutive blank line info
{
	// We need to cache this info, bcz consecutive blank lines may
	// belong to current ini-section, or, belong to next ini-section as headup blank lines.

	int m_iline_start; // start at which ini-line
	int m_count;

	BlankLines_St() {
		Clear();
	}

	bool IsNull() { return m_count == 0 ? true : false; }
	bool IsFill() { return m_count == 0 ? false : true; }

	int AddOne(int iline)
	{
		if (IsNull()) {
			m_iline_start = iline;
			m_count = 1;
		}
		else {
			assert(iline == m_iline_start + m_count);
			m_count++;
		}
		return m_count;
	}

	void Converge(hashdict<Keval_st> &kvdict, const TCHAR *secname)
	{
		if (IsNull())
			return;

		vaDBG3(_T(".   Converge %d blank lines to [%s], virtual-key ';%d'~';%d' "),
			m_count, secname, m_iline_start, m_iline_start + m_count - 1);

		TCHAR vkey[10] = {};
		for (int i = 0; i < m_count; i++)
		{
			snTprintf(vkey, _T(";%d"), m_iline_start + i);
			Keval_st keval;
			keval.type = Keval_st::Comment;
			keval.firstline.set_empty();
			kvdict.set(vkey, std::move(keval));
		}

		Clear();
	}

	void Clear() {
		m_count = m_iline_start = 0;
	}
};


class CIniOp : public SimpleIni
{
public:
	// boilerplate ctor/dtor code, no need to modify >>>
	CIniOp() : SimpleIni(CtorFromChild()) 
	{
		_ct0r(); 
	}
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
	// boilerplate ctor/dtor code, no need to modify <<<

private:
	void _copy_from_old(const CIniOp& old) {
		m_inidict = old.m_inidict;
	}

	void _steal_from_old(CIniOp& old) {
		m_inidict = std::move(old.m_inidict);
	}

public:
	// ... more public methods here ...

	ReCode_et load(const TCHAR *inifilepath); 
	// -- Multiple load() calls accumulate INI content. To avoid accumulate, clear() first.

	ReCode_et clear();

	Sdrings sections();

	bool has_section(const TCHAR *secname);
	Sdrings section_keys(const TCHAR *secname)
	{
		return _in_section_keys(secname, false);
	}

	bool has_key(const TCHAR *secname, const TCHAR *keyname);
	Sdring get(const TCHAR *secname, const TCHAR *keyname);
	// -- To distinguish {no-key} and {has-key with empty value}, user need to call has_key().

	Sdring get_default(const TCHAR *secname, const TCHAR *keyname, 
		const TCHAR *default_value);

	ReCode_et set(const TCHAR *secname, const TCHAR *keyname, const TCHAR *value);

	ReCode_et del_key(const TCHAR *secname, const TCHAR *keyname);

	ReCode_et load_initext(const TCHAR *initext, int inilen);

	Sdring save_ini_string(const TCHAR *crlf=nullptr); 
	// -- return whole INI string, as appear in INI file.

	ReCode_et save(const TCHAR *savefilename=nullptr, const TCHAR *crlf=nullptr);

private:
	void create_virtual_start_section()
	{
		// We require an empty [_start_] virtual section always exits.
		m_inidict.setdefault(VIRTUAL_SECTION_0, hashdict<Keval_st>());
	}

	hashdict<Keval_st>* create_new_section_with_headup(const TCHAR *secname, int blanklines);

	Sdrings _in_section_keys(const TCHAR *secname, bool want_virtual);

	//
	// Leave below at end of class body
	//
private: // data members
	bool m_isUtf8;
	
	Sdring m_inipath;
	Sdring m_inifilenam;
	const TCHAR *m_pfilenam;

	hashdict< hashdict<Keval_st> > m_inidict;

	BlankLines_St m_bls;

private:
	void _ct0r() {
		m_isUtf8 = false;
		m_pfilenam = nullptr;
		m_bls.Clear();
	}

	void _dtor() {
		// nothing
	}
};


CIniOp::ReCode_et
CIniOp::load(const TCHAR *inifilepath)
{
	if(!file_exists(inifilepath))
	{ 
		vaDBG1(_T("CIniOp::load() : File not exist: '%s'"), inifilepath);
		return E_FileNotExist;
	}

	CEC_filehandle_t fh = file_open(inifilepath, open_for_read, 
		open_share_read|open_share_write);

	int filesize = (int)file_getsize(fh);
	if(filesize==0)
		return E_Success; // consider it an empty INI file
	else if(filesize<0)
	{
		vaDBG1(_T("CIniOp::load() : !!file_getsize()=%d: '%s'"), (int)filesize, inifilepath);
		return E_FileIo;
	}

	sdring<char> filebin(filesize);

	int bytesRed = file_read(fh, filebin.getptr(), filesize);
	if(bytesRed!=filesize)
	{
		vaDBG1(_T("CIniOp::load() : !!file_read() get partial content (%d of %d) '%s'"), 
			(int)bytesRed, (int)filesize, inifilepath);
		return E_FileIo;
	}

	int invalid_offset = (int)utf8::find_invalid_utf8seq(filebin, filesize);

	m_isUtf8 = invalid_offset>=0 ? false : true;

	Sdring initext = makeTsdring(std::move(filebin), m_isUtf8 ? mTs_UTF8 : mTs_SysDefault);

	m_inipath = inifilepath;
	ospath::split(inifilepath, m_inifilenam);
	m_pfilenam = m_inifilenam.c_str();

	return load_initext(initext, initext.rawlen());
}

CIniOp::ReCode_et
CIniOp::clear()
{
	bool succ = m_inidict.clear();
	if (!succ)
		return E_ClearHashdict;

	m_isUtf8 = false;
	m_inipath.set_empty();
	m_inifilenam.set_empty();
	m_pfilenam = nullptr;

	return E_Success;
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
	Sdring penkey;   // Pending INI key, for the key-value pair already seen, 
	                 // but whose multi-line keval processing is not finished yet.
	int ini_linestart;
	int idxlastreal; // 0-based index of last-seen real keval(non-comment) line, -1 if none.

	TScalableArray<Sdring> saMixLines;

	KVcontinue()
	{
		saMixLines.SetTrait(KEYVAL_MULTILINE_MAX, KEYVAL_MULTILINE_INC, TSA_no_decrease);
		reset();
	}

	void reset()
	{
		this->penkey.set_empty();
		this->ini_linestart = 0;
		idxlastreal = -1;
	}
	void reset(Sdring&& penkey, int ini_linestart)
	{ 
		this->penkey = std::move(penkey);
		this->ini_linestart = ini_linestart;
		idxlastreal = -1; 
	}

	int accums()
	{
		return saMixLines.CurrentEles();
	}

	bool has_penkey()
	{
		return penkey.is_empty() ? false : true;
	}

	const TCHAR* get_penkey()
	{
		return penkey.is_empty() ? nullptr : penkey.c_str();
	}

	void AppendLine(Sdring&& linetext)
	{
		int nlines = saMixLines.CurrentEles();

		assert(linetext[0]==';' || linetext[0]=='\t');

		if(linetext[0]=='\t')
			idxlastreal = nlines;

		saMixLines.SetEleQuan(nlines+1, true);
		saMixLines[nlines] = std::move(linetext);
	}

	void Converge(hashdict<Keval_st>& kvdict)
	{
		if(penkey.is_empty())
			return;

		int nMixLines = saMixLines.CurrentEles();

		if(nMixLines==0)
		{
			reset();
			return;
		}

		Keval_st *pKeval = kvdict.get(penkey);
		assert(pKeval);
		assert(pKeval->type==Keval_st::OneLine);
		assert(pKeval->ar_exlines==nullptr);

		int nReals = idxlastreal + 1;

		if(nReals>0)
		{
			// Cope with real-value lines. (including embedded comment lines)

			pKeval->totlines = (1+nReals);
			pKeval->ar_exlines = new Sdring[nReals];

			for (int i = 0; i < nReals; i++)
			{
				pKeval->ar_exlines[i] = std::move(saMixLines[i]);
			}
		}

		// Cope with trailing comment lines.

		if(nReals<nMixLines)
		{ 
			vaDBG3(_T(".   Migrate previous %d comment lines(L#%d~L#%d) to standalone virtual-keys"), 
				nMixLines-nReals, 
				ini_linestart+(1+nReals), ini_linestart+nMixLines);
		}

		for(int j=nReals; j<nMixLines; j++)
		{
			TCHAR vkey[10] = {};
			snTprintf(vkey, _T(";%d"), ini_linestart+j);

			Keval_st keval;
			keval.type = Keval_st::Comment;
			keval.firstline = std::move(saMixLines[j]);

			kvdict.set(vkey, std::move(keval));
		}

		saMixLines.SetEleQuan(0);

		reset();
	}
};

enum IniLineCat_et  // INI-line category
{
	ILC_empty = 0,    // empty line
	ILC_comment = 1,  // a comment line
	ILC_unknown = 2,  // unknown invalid line(just keep it like comment)
	ILC_section = 3,  // section line [foo]
	ILC_realkey = 4,  // a key=value line
	ILC_keycont = 5,  // a key-continuation line(but not comment)
};

static IniLineCat_et CheckLineCategory(
	bool has_pending_key,
	const TCHAR *initext, int linepos, int linelen,
	Sdring& newkey_real, // if ILC_realkey, outputs the new real key
	Sdring& linetext // output current line content to process further
	)
{
	// linepos: the byte-offset of this line into whole initext

	newkey_real.set_empty();
	linetext.set_empty();

	// First check if it is a key=val continuation line(start with a \t)
	// First check it, bcz it is NOT allowed to have extra leading spaces.

	if (has_pending_key && initext[linepos] == '\t')
	{
		linetext = Sdring(initext+linepos, linelen);
		return ILC_keycont;
	}

	// Skip blank chars(spaces,tabs) at line start.

	int indents = 0;
	while (indents < linelen && Is_leading_blank(initext[linepos + indents]))
		indents;

	if (indents == linelen)
		return ILC_empty;

	linetext = Sdring(initext + linepos + indents, linelen - indents);

	// Check if it is comment line (lead by ;)

	if(initext[linepos]==';')
		return ILC_comment;

	// Check if it is section name [some_section_name]
		
	Sdring linetrimd = linetext.trim(_T(" \t"), 2);
	if(linetrimd[0]=='[' && linetrimd[-1]==']')
	{
		linetext = std::move(linetrimd);
		return ILC_section;
	}

	// Now it is probably a new key=value line.
	// Try to split linetext with '='

	StringSplitter<
		TCHAR* const // MUST NOT write `TCHAR const*`, would crash on GCC (eg. gcc 9.4).
		             // Write decltype(linetext) would be safer.
		,
		Is_equal_sign, StringSplitter_IsSpaceTab,
		true> // want null-split
		spgkeyval(linetext, 0, linetext.rawlen());
			
	int keylen = 0;
	int keypos = spgkeyval.next(&keylen);
	assert(keypos==0);

	if(keylen==0) // starts with a '='
		return ILC_unknown; 

	int vallen = 0; // len2 just debug
	int valpos = spgkeyval.next(&vallen);
	if(valpos==-1) // no '=' found
		return ILC_unknown;

	newkey_real = Sdring(&linetext[keypos], keylen);

	// In this case, linetext outputs the value part, not the whole INI line.
	if(valpos>=0) // can be -1 if val is empty
	{
		int vallen = linetext.rawlen() - valpos;
		linetext = Sdring(&linetext[valpos], vallen);
	}
	else
	{
		linetext = Sdring();
	}
	return ILC_realkey;
}

hashdict<Keval_st>* 
CIniOp::create_new_section_with_headup(const TCHAR *secname, int blanklines)
{
	assert(!m_inidict.has_key(secname));

	hashdict<Keval_st> *p_kvdict = m_inidict.set(secname, hashdict<Keval_st>());

	// Always set '_blanklines_' as new secname's (virtual) first key.

	Keval_st keval;
	keval.type = Keval_st::BlankLinesToTypeVal(blanklines);
	p_kvdict->set(VIRTUAL_KEYNAME_of_HeadupBlankLines, std::move(keval));
	return p_kvdict;
}


CIniOp::ReCode_et
CIniOp::load_initext(const TCHAR *initext, int inilen)
{
	// Prepare a virtual section("_start_" to hold comments at file start.

	create_virtual_start_section();
	
	Sdring curSection = VIRTUAL_SECTION_0;

	// get back(ptr) the just created empty kvdict inside m_inidict.
	hashdict<Keval_st> *pCurKvdict = m_inidict.get(VIRTUAL_SECTION_0);

	KVcontinue kvc;
	kvc.reset();

	BlankLines_St &bls = m_bls;
	bls.Clear();

	IniLineCat_et linecat = ILC_empty;
	TCHAR vkey[10] = {};

	// Split initext into lines, process them line-by-line.

	StringSplitter<const TCHAR*, IsSplitLf, IsTrimCr, true> 
		spgline(initext, 0, inilen); // spgline: split to get line(s)
	
	int iline = 0;
	while(1)
	{
		int linelen = 0;
		int linepos = spgline.next(&linelen);
		if(linepos==-1)
			break;

		iline++;

		hashdict<Keval_st> &kvdict = *pCurKvdict; // make a short name

		Sdring newkey_real, linetext;

		linecat = CheckLineCategory(kvc.has_penkey(), 
			initext, linepos, linelen, 
			newkey_real, linetext);
		// -- [outparam] newkey_real, linetext may have produced NEW content

		// ..... Check each ILC_xxx .....

		if(linecat==ILC_empty || linecat==ILC_comment || linecat==ILC_unknown)
		{
			const TCHAR *str_ilc = linecat==ILC_empty ? _T("ILC_empty") :
				( linecat==ILC_comment ? _T("ILC_comment") : _T("ILC_unknown") );
			vaDBG3(_T("{%s}L#%d <%s> '%s'"), m_pfilenam,iline, 
				str_ilc,   // <%s>
				linetext.is_empty() ? _T("") : linetext.c_str());

			if(kvc.has_penkey() && linecat==ILC_comment)
			{
				// Assume this comment-line belongs to current key-val(as embedded comment), 
				// may migrate outward in later kvc.Converge().

				vaDBG3(_T(".   '%s' #.%d: assume embedded comment"), kvc.get_penkey(), kvc.accums()+1);
				kvc.AppendLine(std::move(linetext));
			}
			else
			{
				if(linecat==ILC_empty)
				{
					kvc.Converge(kvdict);

					int nowcount = bls.AddOne(iline);
					vaDBG3(_T(".   Cache this blank line (accumed %d)."), nowcount);
				}
				else // meet a comment line
				{
					bls.Converge(kvdict, curSection);

					// Add this comment line as kvdict's new virtual key
					snTprintf(vkey, _T(";%d"), iline);

					vaDBG3(_T(".   Added as virtual-key '%s'"), vkey);

					Keval_st keval;
					keval.type = Keval_st::Comment;
					keval.firstline = std::move(linetext);

					kvdict.set(vkey, std::move(keval));
				}
			}
		}
		else if(linecat==ILC_section)
		{
			Sdring secname = linetext.trim(_T("[]"), 2);
			vaDBG3(_T("{%s}L#%d <ILC_section> [%s]"), m_pfilenam, iline, secname.c_str());

			kvc.Converge(kvdict);

			pCurKvdict = m_inidict.get(secname);
			
			if(!pCurKvdict)
			{
				vaDBG3(_T(".   Attach %d headup blank lines to the new secname."), bls.m_count);
				pCurKvdict = create_new_section_with_headup(secname, bls.m_count);
			}
			else
			{
				// If a secname is loaded from a second file, the blanklines above 
				// that [secname] is discarded. Yes, we only respect the first.
			}

			bls.Clear();

			curSection = std::move(secname);
		}
		else if(linecat==ILC_realkey)
		{
			vaDBG3(_T("{%s}L#%d <ILC_realkey> '%s' = '%s'"), m_pfilenam, iline,
				newkey_real.c_str(), linetext.c_str());

			kvc.Converge(kvdict); // settle pending key-value pair.
			bls.Converge(kvdict, curSection);

			Keval_st keval;
			keval.type = Keval_st::OneLine; // assume it one-line key-val at first seen
			keval.firstline = std::move(linetext);

			kvdict.set(newkey_real, std::move(keval));

			kvc.reset(std::move(newkey_real), iline);
		}
		else
		{
			assert(linecat == ILC_keycont);
			vaDBG3(_T("{%s}L#%d <ILC_keycont> '%s'"), m_pfilenam,iline, linetext.c_str());
			vaDBG3(_T(".   '%s' #.%d: keyval continuation"), kvc.get_penkey(), kvc.accums()+1);

			kvc.AppendLine(std::move(linetext));
		}

	} // while(1) process each INI line

	vaDBG3(_T("{%s}Meet end of INI text, total INI lines: %d"), m_pfilenam, iline);
	kvc.Converge(*pCurKvdict);

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

	hashdict<Keval_st> *p_kvdict = m_inidict.get(secname);
	if(p_kvdict)
		return true;
	else
		return false;
}



Sdrings CIniOp::_in_section_keys(const TCHAR *secname, bool want_virtual)
{
	// Return list of keynames in [secname]

	if(!has_section(secname))
		return Sdrings();

	hashdict<Keval_st> &kvdict = *m_inidict.get(secname);

	// Those rawkeys that do NOT start with ';' are real INI-keys.

	auto enumor = kvdict.get_enumor();
	const TCHAR *rawkey = nullptr;

	int nreal = 0; // count of real keys
	for(;;)
	{
		rawkey = enumor.next();
		if(!rawkey)
			break;

		if (rawkey[0] != ';')
			nreal++;
	}

	assert(nreal>0); // at least we have (virtual) _blanklines_=...

// 	if(nreal==0) // to-test: pure comments section
// 		return Sdrings();
// 
	Sdrings rets(want_virtual ? nreal : nreal-1);

	enumor.reset();

	if(!want_virtual)
	{
		rawkey = enumor.next(); // discard the virtual
		assert(Sdring::str_match(rawkey, VIRTUAL_KEYNAME_of_HeadupBlankLines));
	}

	for(int i=0;;)
	{
		rawkey = enumor.next();
		if (!rawkey)
			break;

		if (rawkey[0] != ';')
			rets[i++] = rawkey;
	}

	return rets;
}


bool CIniOp::has_key(const TCHAR *secname, const TCHAR *keyname)
{
	hashdict<Keval_st> *p_kvdict = m_inidict.get(secname);
	if(!p_kvdict)
		return false;

	Keval_st *pval = p_kvdict->get(keyname);
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
	hashdict<Keval_st> *p_kvdict = m_inidict.get(secname);
	if(!p_kvdict)
		return Sdring();

	Keval_st& keval = *p_kvdict->get(keyname);
	if(&keval==nullptr)
		return Sdring();

	/* Example of a multiline INI key-value pair:

keym = 
	val line one
;	1st embedded comment line in key's value
	val line two
;	2nd embedded comment line in key's value
	val line three

	The keval's firstline can be any string(empty string is possible as in above example).
	From second line on, it must starts with a Tab or a semicolon.
	We only concatenate those lines leading by Tab(but strip off the Tab) for user output.
	*/

	if(keval.type==Keval_st::OneLine)
	{
		assert(keval.ar_exlines == nullptr);
		return keval.firstline; // will call Sdring's copy-ctor, not move-ctor
	}

	// Special since 20260707: check for "_blanklines_" keyname.
	if (int(keval.type) < 0)
	{
		if( _tcscmp(keyname, VIRTUAL_KEYNAME_of_HeadupBlankLines)!=0 )
		{
			vaDBG1(_T("Unexpect! [%s] '%s' Meet negative Keval type(=%d)"), secname, keyname, keval.totlines);
			return Sdring();
		}
		int blanklines = Keval_st::TypeValToBlankLines(keval.type);
		Sdring sret(12);
		snTprintf(sret.getbuf(), sret.rawlen(), _T("%d"), blanklines);
		return sret;
	}

	// Below: Deal with multi-line keval case.

	TScalableArray<TCHAR> sout(KEYVAL_MULTILINE_MAX, KEYVAL_MULTILINE_INC, TSA_no_decrease);

	// Get first line.

	sout.AppendTail(keval.firstline.getptr(), keval.firstline.rawlen());
	EasyDebug_AppendNUL(sout);

	assert(keval.totlines > 1);
	assert(keval.ar_exlines!=nullptr);

	// Get 2nd+ lines.

	for(int i=0; i<keval.totlines-1; i++)
	{
		if(keval.ar_exlines[i][0] != '\t')
			continue;

		sout.AppendTail('\n');
		sout.AppendTail(keval.ar_exlines[i].getptr()+1, keval.ar_exlines[i].rawlen()-1);
		EasyDebug_AppendNUL(sout);
	}

	const TCHAR *psz = sout.GetElePtr();
	int szlen = sout.CurrentEles();
	return Sdring(psz, szlen);
}


Sdring CIniOp::get_default(const TCHAR *secname, const TCHAR *keyname,
	const TCHAR *default_value)
{
	if(!has_key(secname, keyname))
		return Sdring(default_value);

	return get(secname, keyname);
}


CIniOp::ReCode_et
CIniOp::set(const TCHAR *secname, const TCHAR *keyname, const TCHAR *sz_val_lines)
{
	// Note: key_value can be any string, one-line or multi-line;
	// any line can be started by Tab or semicolon(;).
	// But, If a line is started by semicolon(;), it will be considered actual content,
	// not an embedded comment line.
	// An embedded comment line can be introduced ONLY by manually editing the INI file. 

	create_virtual_start_section();

	hashdict<Keval_st> *p_kvdict = m_inidict.get(secname);
	if(!p_kvdict)
	{
		// Create a new kvdict for this secname.
		p_kvdict = create_new_section_with_headup(secname, 0);
	}

	Keval_st keval;

	StringSplitter<const TCHAR*, IsSplitLf, IsTrimCr, true> 
		spgline(sz_val_lines, 0); // spgline: split to get line(s)

	// Count how many lines in val_lines.
	// * If there is no '\n'(including empty string), we count it one lines.
	// * If there is one '\n'(event if val_lines[0]=='\n'), we count it two lines.

	keval.totlines = spgline.count();
	assert(keval.totlines > 0); // even for empty string

	int linelen = 0;
	int linepos = spgline.next(&linelen);

	keval.firstline = std::move( Sdring(sz_val_lines+linepos, linelen) );

	if(keval.totlines>1)
	{
		keval.ar_exlines = new Sdring[keval.totlines - 1];

		int i = 0;
		for (;; i++)
		{
			linepos = spgline.next(&linelen);
			if (linepos == -1)
				break;

			// Need to add an extra Tab char at Sdring head.
			Sdring text(linelen + 1);
			text[0] = '\t';
			Sdring::nchars_cpy(text.getptr() + 1, sz_val_lines + linepos, linelen);

			keval.ar_exlines[i] = std::move(text);
		}

		assert(i == keval.totlines - 1);
	}

	p_kvdict->set(keyname, std::move(keval));

	return E_Success;
}


CIniOp::ReCode_et
CIniOp::del_key(const TCHAR *secname, const TCHAR *keyname)
{
	create_virtual_start_section();

	hashdict<Keval_st> *p_kvdict = m_inidict.get(secname);
	if (p_kvdict)
	{
		bool succ = p_kvdict->del(keyname);
		if(succ)
			return E_Success;
		else
			return E_NoSuchKey;
	}
	else
	{
		return E_NoSuchKey;
	}
}


static void TSA_append_line(TScalableArray<TCHAR>& sout, const Sdring& ins, 
	const TCHAR *crlf, int crlflen)
{
	// Append one line( ins + crlf ) to sout.
	// On input, sout's final element should be '\0'

	int inslen = ins.rawlen();

	int orig_len_ = sout.CurrentEles();
	assert(sout[orig_len_-1]=='\0');

	sout.SetEleQuan(orig_len_ + inslen + crlflen);
	Sdring::str_ncpy(&sout[orig_len_-1], ins.c_str(), inslen);
	Sdring::str_ncpy(&sout[orig_len_-1 + inslen], crlf, crlflen);
	sout[orig_len_-1 + inslen + crlflen] = '\0';
}

static void TSA_append_blank_lines(TScalableArray<TCHAR>& sout, int lines,
	const TCHAR *crlf, int crlflen)
{
	if (lines == 0)
		return;
	assert(lines>0);

	int len0_ = sout.CurrentEles();
	int len1_ = len0_ + lines*crlflen;
	sout.SetEleQuan(len1_);
	for (int i = 0; i < lines; i++)
		sout.AppendAt(len0_-1 + i*crlflen, crlf, crlflen);
	sout[len1_-1] = '\0';
}

Sdring CIniOp::save_ini_string(const TCHAR *crlf)
{
	create_virtual_start_section();

	if(!(crlf && crlf[0]))
		crlf = os_crlf;

	int crlflen = (int)_tcslen(crlf);

	TScalableArray<TCHAR> sout(INIWRITE_MAX, INIWRITE_INC, TSA_no_decrease);
	sout.AppendTail('\0');

	auto en_sections = m_inidict.get_enumor();
	int sec_count = 0;

	for(;; sec_count++)
	{
		hashdict<Keval_st> *p_kvdict = nullptr;
		const TCHAR *secname = en_sections.next(&p_kvdict);
		if(!secname)
			break;

		auto en_kvdict = p_kvdict->get_enumor();

		// Write [section name], but skip the virtual [_start_] section

		if(sec_count > 0)
		{
			// For a [section name], if there is '_blanklines_' key, write those blank lines first.

			Keval_st *p_keval = nullptr;
			const TCHAR *_blanklines_ = en_kvdict.next(&p_keval);

			assert(Sdring::str_match(_blanklines_, VIRTUAL_KEYNAME_of_HeadupBlankLines));

			int blanklines = Keval_st::TypeValToBlankLines(p_keval->type);
			if( blanklines > 0 )
				TSA_append_blank_lines(sout, blanklines, crlf, crlflen);

			// Write actual [section name]

			int seclen = (int)_tcslen(secname); // note: `secname` does not have brackets
			Sdring _secname_(seclen+2);
			snTprintf(_secname_.getptr(), seclen+3, _T("[%s]"), secname);
			TSA_append_line(sout, _secname_, crlf, crlflen);
		}
		else
		{
			// first section MUST be virtual [_start_]
			assert( _tcscmp(secname, VIRTUAL_SECTION_0)==0 );
		}

		// Write each key=value pair in this section.

		for(;;) // for each key=value pair
		{
			Keval_st *p_keval = nullptr; // notice: no-way to decorate const?
			const TCHAR *keyname = en_kvdict.next(&p_keval);
			if(!keyname)
				break;

			const Keval_st& keval = *p_keval;

			// Check whether it's virtual(;3=xxx) or real(foo=bar).

			if(keyname[0]==';') // a virtual key
			{
				TSA_append_line(sout, keval.firstline, crlf, crlflen);
				continue;
			}

			// Now it is a real key=value pair.

			// Write key=value's first line.

			int vallen = keval.firstline.rawlen();

			int len_1st = (int)_tcslen(keyname) + 3 + vallen + crlflen;

			int orig_len_ = sout.CurrentEles();

			sout.SetEleQuan(orig_len_ + len_1st);

			snTprintf(&sout[orig_len_-1], len_1st+1,
				_T("%s = %s%s")
				, 
				keyname, 
				vallen==0 ? _T("") : keval.firstline.c_str(),
				crlf);

			if(keval.totlines==0 || keval.totlines==1)
				continue;

			// Write key=value's remaining lines.

			for(int i=0; i<keval.totlines-1; i++)
			{
				TSA_append_line(sout, keval.ar_exlines[i], crlf, crlflen);
			}
		}

	} // for(;; sec_count++)

	// Append end-of-INI blank lines.
	// But I reduce one CRLF now, bcz above sec_count cycle has always added extra one CRLF.
	if (m_bls.IsFill())
	{
		int addlines = m_bls.m_count - 1;
		TSA_append_blank_lines(sout, addlines, crlf, crlflen);
	}
	else
	{
		// Chop off one CRLF from sout's tail.
		int orig_len_ = sout.CurrentEles();
		assert(sout[orig_len_-1]=='\0');
		if(orig_len_ > crlflen)
			sout[orig_len_ - 1 - crlflen] = '\0';
	}

	// [2026-05-02] Here I have to do a string copy, bcz TSA & Sdring's heap pointers
	// are not compatible.
	return Sdring(sout.GetElePtr());
}


CIniOp::ReCode_et 
CIniOp::save(const TCHAR *savefilename, const TCHAR *crlf)
{
	const TCHAR *inipath = savefilename ? savefilename : m_inipath.c_str();

	Sdring initext = save_ini_string(crlf);

	sdring<char> initextA = makeAsdring(initext, m_isUtf8 ? mTs_UTF8 : mTs_SysDefault);

	filehandle_t fh = file_open(inipath, open_for_write, open_share_none);
	if(fh<0)
	{
		vaDBG1(_T("CIniOp::save() : Cannot open file for write: '%s' (fsapi-error:%d)"), inipath, (int)fh);
		return E_FileIo;
	}

	CEC_filehandle_t cec_fh = fh;

	fserror_et fserr = file_setsize(fh, 0); // empty the file
	if(fserr)
		return E_FileIo;

	int bytes_to_write = initextA.rawlen();
	int bytesWtn = file_write(fh, initextA.c_str(), bytes_to_write);
	if(bytesWtn!=bytes_to_write)
		return E_FileIo;

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
	// High 16-bit is major ver, low 16-bit is minor ver.
	return 0x10001; 
}

void SimpleIni::_ct0r()
{
	m_pi = nullptr;
}
void SimpleIni::_init_once_after_ct0r()
{
	m_pi = new iniop::CIniOp;
}
void SimpleIni::_dtor()
{
	delete m_pi;
}
void SimpleIni::_copy_from_old(const SimpleIni& old)
{
	m_pi = new iniop::CIniOp(*old.m_pi);
}


SimpleIni::ReCode_et 
SimpleIni::load(const TCHAR *inifilename)
{
	return m_pi->load(inifilename);
}

SimpleIni::ReCode_et
SimpleIni::clear()
{
	return m_pi->clear();
}

Sdrings SimpleIni::sections()
{
	return m_pi->sections();
}

bool SimpleIni::has_section(const TCHAR *section_name)
{
	return m_pi->has_section(section_name);
}

Sdrings SimpleIni::section_keys(const TCHAR *section_name)
{
	return m_pi->section_keys(section_name);
}

bool SimpleIni::has_key(const TCHAR *section_name, const TCHAR *keyname)
{
	return m_pi->has_key(section_name, keyname);
}

Sdring SimpleIni::get(const TCHAR *section_name, const TCHAR *keyname)
{
	return m_pi->get(section_name, keyname);
}

Sdring SimpleIni::get_default(const TCHAR *section_name, const TCHAR *keyname,
	const TCHAR *default_value)
{
	return m_pi->get_default(section_name, keyname, default_value);
}

SimpleIni::ReCode_et
SimpleIni::set(const TCHAR *section_name, const TCHAR *keyname, const TCHAR *value)
{
	return m_pi->set(section_name, keyname, value);
}

SimpleIni::ReCode_et
SimpleIni::del_key(const TCHAR *section_name, const TCHAR *keyname)
{
	return (ReCode_et)m_pi->del_key(section_name, keyname);
}

SimpleIni::ReCode_et
SimpleIni::load_initext(const TCHAR *initext, int inilen)
{
	return m_pi->load_initext(initext, inilen);
}

Sdring SimpleIni::save_ini_string(const TCHAR *crlf)
{
	return m_pi->save_ini_string(crlf);
}

SimpleIni::ReCode_et 
SimpleIni::save(const TCHAR *savefilename, const TCHAR *crlf)
{
	return m_pi->save(savefilename, crlf);
}

//
//
//

#ifndef SimpleIni_DEBUG
#include <CHHI_vaDBG_show.h> // Now restore vaDBG macros
#endif


#endif // [IMPL]

#endif // include once guard
