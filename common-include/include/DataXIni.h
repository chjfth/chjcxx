#ifndef __CHHI__DataXIni_h_
#define __CHHI__DataXIni_h_
#define __CHHI__DataXIni_h_created_ 20260508
#define __CHHI__DataXIni_h_updated_ 20260511


#include <hashdict.h>
#include <SimpleIniEx.h>
#include <DataXString.h>
#include <TScalableArray.h>


////////////////////////////////////////////////////////////////////////////
//-- namespace datax { 
////////////////////////////////////////////////////////////////////////////
// Place API function declarations in this namespace.

class DataXIni
{
public:
	DataXIni() {}

	void AddItem(const TCHAR *secname, const TCHAR *keyname, IDataXString *pdatax);

	enum LoadSemantic_et { LoadFresh=0, LoadIncrement=1 };

	void LoadIni(const TCHAR* const ar_inifiles[], int nfiles, LoadSemantic_et how=LoadFresh);
	
	Sdring SaveIni(); // return the INI filepath saved(would be one of ar_inifiles[])

	void ResetDefault();

private:
	struct IniItem_st
	{
		// index into msa_sections[], avoid storing duplicate section name string
		int idx_secname; 

		Sdring keyname;       // key-name for an INI item
		IDataXString *pdatax; // dynamic data for this INI item
	};

private:
	SimpleIniEx m_ini;
	
	TScalableArray<Sdring> msa_sections;
	chjds::hashdict<int> m_sec2idx; // query section-name for its idx into msa_sections[]

	TScalableArray<IniItem_st> msa_items;
};


template<typename TU, typename FORMAT = XStringFormatDefault>
class DataXString_AutoSaveIni : public DataXString<TU, FORMAT>
{
	// When assigning TU value to this object, INI save() is automatically called.
private:
	DataXIni& m_xini;

	DataXString_AutoSaveIni(const DataXString_AutoSaveIni&) = delete;
	DataXString_AutoSaveIni(DataXString_AutoSaveIni&&) = delete;

public:
	using Base = DataXString<TU, FORMAT>;

public:
	DataXString_AutoSaveIni(
		DataXIni& xini, const TCHAR *secname, const TCHAR *keyname,
		const TCHAR* default_val) 
		: 
		m_xini(xini), 
		Base(default_val) // the value to use if missing from INI 
	{
		m_xini.AddItem(secname, keyname, this);
	}

	using Base::operator=;

	virtual SetValue_ret SetValue(TU&& val) cxx11_override
	{
		SetValue_ret svret = Base::SetValue(std::move(val));

		if (svret == SetNew)
		{
			m_xini.SaveIni();
		}

		return svret;
	}
};




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


#if defined(DataXIni_IMPL) || (defined CHHI_ALL_IMPL && !defined CHHI_ALL_IMPL_HIDE_DataXIni) // [IMPL]


// >>> Include headers required by this lib's implementation

#include <new>
#include <assert.h>
//#include <commdefs.h> // for Uint, Uint64, enum bitwise-OR etc
//#include <snTprintf.h>

// <<< Include headers required by this lib's implementation



#ifndef DataXIni_DEBUG
#include <CHHI_vaDBG_hide.h> // Suppress/invalidate vaDBG macros, from now on
#endif


////////////////////////////////////////////////////////////////////////////
//-- namespace datax {
////////////////////////////////////////////////////////////////////////////
// Place API function Implementation in this namespace.


void DataXIni::AddItem(const TCHAR *secname, const TCHAR *keyname, IDataXString *pdatax)
{
	int nsecs = msa_sections.CurrentEles();

	// Query secname for idx into msa_sections[]
	int secidx = *m_sec2idx.setdefault(secname, nsecs);

	if(secidx==nsecs) // secname is not recorded yet
	{
		msa_sections.SetEleQuan(secidx + 1, true);
		new(&msa_sections[secidx]) Sdring(secname);
	}

	IniItem_st item = {secidx, Sdring(keyname), pdatax};
	
	// User should avoid adding the same keyname twice.
	int nitems = msa_items.CurrentEles();
	msa_items.SetEleQuan(nitems+1, true);
	new(&msa_items[nitems]) IniItem_st(std::move(item));

	Sdring itemval = m_ini.get_default(secname, keyname, pdatax->GetDefault());

	pdatax->SetValueByString(itemval, false);
}


void DataXIni::LoadIni(const TCHAR* const ar_inifiles[], int nfiles, LoadSemantic_et how)
{
	m_ini.load_cascade(ar_inifiles, nfiles);

	// Cycle through existing items, check if each appears in the INI,
	// load its corresponding INI-stored values.

	int nitems = msa_items.CurrentEles();
	for(int i=0; i<nitems; i++)
	{
		IniItem_st &item = msa_items[i];
		const TCHAR *secname = msa_sections[item.idx_secname].c_str();
		Sdring itemval;

		if(how==LoadFresh)
		{
			// LoadFresh means: If some option is not provided by INI, 
			// we should restore its runtime value to hard-coded default.

			itemval = m_ini.get_default(secname, item.keyname, item.pdatax->GetDefault());
			item.pdatax->SetValueByString(itemval, false);
		}
		else
		{
			assert(how==LoadIncrement);
			// LoadIncrement means: If some option is not provided by INI,
			// we should keep it current runtime value(in `item.pdatax`).

			if (m_ini.has_key(secname, item.keyname))
			{
				Sdring itemval = m_ini.get(secname, item.keyname);
				item.pdatax->SetValueByString(itemval, false);
			}
		}
	} // for
}


Sdring DataXIni::SaveIni()
{
	// Cycle through existing items. For each dirty item, sync its value back into INI.
	
	int nitems = msa_items.CurrentEles();
	for(int i=0; i<nitems; i++)
	{
		IniItem_st &item = msa_items[i];
		const TCHAR *secname = msa_sections[item.idx_secname].c_str();

		if (item.pdatax->IsDirty())
		{
			Sdring itemval = item.pdatax->GetValueAsString();
			m_ini.set(secname, item.keyname, itemval);

			vaDBG2(_T("DataXIni::SaveIni() : Flush to INI dirty key '%s'='%s'"), item.keyname.c_str(), itemval.c_str());
		}
	}

	Sdring outinipath;
	bool succ = m_ini.save_cascade(&outinipath);
	if (!succ)
		return Sdring();

	// Clear dirty flags for items.
	for(int i=0; i<nitems; i++)
	{
		IniItem_st &item = msa_items[i];
		const TCHAR *secname = msa_sections[item.idx_secname].c_str();
		item.pdatax->SetDirty(false);
	}

	return outinipath;
}


void DataXIni::ResetDefault()
{
	// Cycle through existing items, reset each to program Default.
	// INI content is cleared, to indicate that everything is from program Default.
	// Yes, I choose NOT to write default values to INI file. 
	// Benefit of user having an empty INI: 
	// Running new-version of a program will automatically receive new Defaults.

	int nitems = msa_items.CurrentEles();
	for(int i=0; i<nitems; i++)
	{
		IniItem_st &item = msa_items[i];
		const TCHAR *secname = msa_sections[item.idx_secname].c_str();

		item.pdatax->SetDefault(); // dirty cleared internally

		m_ini.del_key(secname, item.keyname);
	}

	m_ini.save_cascade();
}


////////////////////////////////////////////////////////////////////////////
//-- } // namespace datax
////////////////////////////////////////////////////////////////////////////



#ifndef DataXIni_DEBUG
#include <CHHI_vaDBG_show.h> // Now restore vaDBG macros
#endif


#endif // [IMPL]


#endif // include once guard
