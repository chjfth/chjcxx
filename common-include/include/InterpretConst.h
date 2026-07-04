#ifndef __CHHI__InterpretConst_h_
#define __CHHI__InterpretConst_h_
#define __CHHI__InterpretConst_h_created_ 20251208
#define __CHHI__InterpretConst_h_updated_ 20260612


// Note: The API interface code(not implementation code) should ONLY rely on 
// standard C headers, NOT relying on OS platform headers like <windows.h> .
// So that, it can be used standalone with NtStatus.itc.h .

#include <stdarg.h>
#include <stdio.h>

#include <ps_TCHAR.h>
#include <sdring.h>


namespace itc {                        // API namespace


	typedef unsigned int CONSTVAL_t;

	struct Enum2Val_st // ok
	{
		const TCHAR *EnumName;
		CONSTVAL_t ConstVal;
	};

	struct SingleBit2Val_st // oldname Bitfield2Val_st
	{
		const TCHAR *BitfieldName;
		CONSTVAL_t ConstVal;
	};

	struct EnumGroup_st
	{
		CONSTVAL_t GroupMask;
		const Enum2Val_st *arEnum2Val;
		int nEnum2Val; // element count of arEnum2Val[]

		// const TCHAR *zvName; // const-name for zero-value, points to arEnum2Val[x].BitfieldName . (unimplemented yet)
	};

	typedef EnumGroup_st ItcGroup_st;
	// -- CInterpretConst internally sees a single-bit-group as a special case
	//    of an enum-group(GroupMask has only one bit set), so create an alias name
	//    `ItcGroup_st` for internal use.

	struct Error // As C++ exception type
	{
		Error(const TCHAR* msg)
		{
			this->msg = msg;
		}

		const TCHAR* msg;
	};


	struct DisplayTrait_st
	{
		// How to display this segment.

		unsigned segname : 1;	// display segment name itself.
		// -- This is effective only if ItcSeg_st.segname exists.

		unsigned possign : 1;	// for valname, show positive sign('+')

		unsigned negsign : 1;	// want negative valname
		// -- By default, if a bit has value 0, its corresponding valname is not displayed.
		//    But if show_negsign==1, the negative-valname will be displayed. Two cases exit:
		//    (1) If neg_valname exists, that neg_valname is displayed.
		//    (2) No neg_valname exists, '-' prefixed valname is displayed, like "-WS_CHILD".

		unsigned negsign_smallcase : 1; // will show "-ws_child" instead of "-WS_CHILD"
		// -- This is effective only if negsign presentation is effective.

		unsigned numdec : 1; // besides valname, show valnum in decimal
		unsigned numhex : 1; // besides valname, show valnum in hex (0x...)

		unsigned rela_numdec : 1; // show relative valnum in decimal
		unsigned rela_numhex : 1; // show relative valnum in hex (0x...)
	};

	const DisplayTrait_st disp_default0 = {};


	enum DisplayFormat_et
	{
		DF_ValueOnly = 0,
		DF_NameOnly = 1,
		DF_NameAndValue = 2, // name(value)
		DF_ValueAndName = 3, // value(name)

		DF_NVAuto = 8 // If enum-trait, value(name); if bitfield-trait, name(value) 
	};


	class Enum2Val_merge
	{
		// Use this class to merge two or more existing Enum2Val_st arrays into 
		// a single Enum2Val_st array, so that the single array can be used to 
		// construct a CInterpretConst object.

		// Extra memory is allocated to store the new contiguous Enum2Val_st array.

	public:
		Enum2Val_merge(const Enum2Val_st *arEnum2Val, int nEnum2Val, 
			... // more [arEnum2Val, nEnum2Val] pairs, end with [nullptr, 0]
			);

		~Enum2Val_merge() {
			delete mar_enum2val;
			mar_enum2val = nullptr;
			m_count = 0;
		}

		const Enum2Val_st* get_array() const { 
			return mar_enum2val;
		}

		int count() const {
			return m_count;
		}

	private:
		Enum2Val_st *mar_enum2val;
		int m_count;
	};


	class CInterpretConst
	{
	public:
		virtual ~CInterpretConst(){ _dtor(); }

		CInterpretConst(const Enum2Val_st *arEnum2Val, int nEnum2Val, const TCHAR *valfmt=nullptr)
		{
			_ctor(arEnum2Val, nEnum2Val, valfmt);
		}

		template<size_t eles>
		CInterpretConst(const Enum2Val_st (&ar)[eles], const TCHAR *valfmt=nullptr)
		{
			_ctor(ar, eles, valfmt);
		}

		template<size_t eles1, size_t eles2>
		CInterpretConst(
			const Enum2Val_st (&ar1)[eles1], 
			const Enum2Val_st (&ar2)[eles2],
			const TCHAR *valfmt=nullptr)
		{
			// to-test
			Enum2Val_merge *ar_e2vmerge = new Enum2Val_merge(
				ar1, eles1, ar2, eles2,
				nullptr, 0); // this Enum2Val_merge object will not be deleted, by design
			_ctor(ar_e2vmerge->get_array(), ar_e2vmerge->count(), valfmt);
		}

		CInterpretConst(const Enum2Val_merge *ar_e2vmerge, const TCHAR *valfmt=nullptr)
		{
			_ctor(ar_e2vmerge->get_array(), ar_e2vmerge->count(), valfmt);
		}

		//

		CInterpretConst(const SingleBit2Val_st *arSingleBit2Val, int nSingleBit2Val, const TCHAR *valfmt=nullptr)
		{
			_ctor(arSingleBit2Val, nSingleBit2Val, valfmt);
		}

		template<size_t eles>
		CInterpretConst(const SingleBit2Val_st (&ar)[eles], const TCHAR *valfmt=nullptr)
		{
			_ctor(ar, eles, valfmt);
		}

		template<size_t eles1, size_t eles2>
		CInterpretConst(
			const SingleBit2Val_st (&ar1)[eles1],
			const SingleBit2Val_st (&ar2)[eles2],
			const TCHAR *valfmt=nullptr)
		{
			// A facility ctor that combines two SingleBit2Val_st-s
			// Example: winnt.itc: FileRights, DirectoryRights

			_ctor(valfmt, 
				nullptr, 0, // no Enum2Val_st
				ar1, eles1,  ar2, eles2,
				nullptr, 0);
		}

		template<size_t eles1, size_t eles2, size_t eles3>
		CInterpretConst(
			const SingleBit2Val_st (&ar1)[eles1],
			const SingleBit2Val_st (&ar2)[eles2],
			const SingleBit2Val_st (&ar3)[eles3],
			const TCHAR *valfmt=nullptr)
		{
			// A facility ctor that combines three SingleBit2Val_st-s

			_ctor(valfmt, 
				nullptr, 0, // no Enum2Val_st
				ar1, eles1,  ar2, eles2, ar3, eles3,
				nullptr, 0);
		}

		//

		CInterpretConst(const EnumGroup_st *arGroups, int nGroups, const TCHAR *valfmt=nullptr)
		{
			_ctor(arGroups, nGroups, valfmt);
		}

		template<size_t eles>
		CInterpretConst(const EnumGroup_st (&ar)[eles], const TCHAR *valfmt=nullptr)
		{
			_ctor(ar, eles, valfmt);
		}

		//

		CInterpretConst(const TCHAR *valfmt,
			const SingleBit2Val_st *arSinglebit2Val, int nVals,
			... // [arSinglebit2Val, nVals] pairs, end with [nullptr, 0]
			) // VK0
		{
			va_list args;
			va_start(args, valfmt);
			_ctor(valfmt, nullptr, 0, args); // call to the {most generic initor}
			va_end(args);
		}

		CInterpretConst(const TCHAR *valfmt,
			const EnumGroup_st *arGroups, int nGroups, 
			... // [arGroups, nGroups] pairs, end with [nullptr, 0]
			) // VK1
		{
			va_list args;
			va_start(args, nGroups);
			_ctor(valfmt, arGroups, nGroups, args);  // call to the {most generic initor}
			va_end(args);
		}

		CInterpretConst(const TCHAR *valfmt,
			const Enum2Val_st *arEnum2Val, int nEnum2Val, 
			... // [arEnum2Val, nEnum2Val] pairs, end with [nullptr, 0]
			) // VK2
		{
			// This is a simplified version of VK1.

			// auto-calculate the mask for Enum2Val-s.
			CONSTVAL_t enum_mask = 0;
			for(int i=0; i<nEnum2Val; i++)
			{
				enum_mask |= arEnum2Val[i].ConstVal;
			}

			EnumGroup_st eg = { enum_mask, arEnum2Val, nEnum2Val };
			// -- this object will get a copy inside the following _ctor()

			va_list args;
			va_start(args, nEnum2Val);

			_ctor(valfmt, &eg, 1, args);

			va_end(args);
		}

		////

		const TCHAR *Interpret_i1(CONSTVAL_t val, DisplayFormat_et dispfmt,
			TCHAR *buf, int bufsize,
			const TCHAR *valfmt=nullptr, // can be ITCF_HEX1B, ITCF_HEX4B etc
			const TCHAR *sep=nullptr     // bit-field separator string, default is "|"
			) const;

		Sdring Interpret(CONSTVAL_t val, 
			DisplayFormat_et dispfmt=DF_NameOnly,
			const TCHAR *valfmt=nullptr, // can be ITCF_HEX1B, ITCF_HEX4B etc
			const TCHAR *sep=nullptr     // bit-field separator string, default is "|"
			) const;

		bool SetValFmt(const TCHAR *fmt);

		Sdring DumpText(const TCHAR *crlf=nullptr) const;

// 		template<size_t eles>
// 		TCHAR *DumpText(TCHAR (&ar)[eles]) const
// 		{
// 			return DumpText(ar, eles, nullptr);
// 		}

		CONSTVAL_t NamesToVal(const TCHAR *names, bool *p_is_err=nullptr) const; // names separated by |
		CONSTVAL_t OneNameToVal(const TCHAR *name, bool *p_is_err=nullptr) const; 

	private:
		enum { 
			WholeDisplayMaxChars = 2000,
			OneDisplayMaxChars = 100,
			FmtSpecMaxChars = 10,
		};

	private:
		void _ctor(const Enum2Val_st *arEnum2Val, int nEnum2Val, 
			const TCHAR *valfmt);

		void _ctor(const SingleBit2Val_st *arBitfield2Val, int nBitfield2Val, 
			const TCHAR *valfmt);

		void _ctor(const EnumGroup_st *arGroups, int nGroups, 
			const TCHAR *valfmt);

		void _ctor(const TCHAR *valfmt,
			const EnumGroup_st *arGroups, int nGroups, 
			va_list args  // [arSinglebit2Val, nSinglebit2Val] pairs, end with [nullptr, nullptr]
			); // {most generic initor}, combine two sets of input

		void _ctor(const TCHAR *valfmt,
			const EnumGroup_st *arGroups, int nGroups, 
			... // [arSinglebit2Val, nSinglebit2Val] pairs, end with [nullptr, 0]
			)
		{
			va_list args;
			va_start(args, nGroups);
			_ctor(valfmt, arGroups, nGroups, args);
			va_end(args);
		}

	private:
		void _dtor();
		void _ct0r(const TCHAR *valfmt);
		bool is_enum_ctor() const { return m_arGroups==&m_EnumC2V; };
		static bool is_unique_mask(CONSTVAL_t oldmasks, CONSTVAL_t newmask);
		bool ensure_unique_masks();

		const TCHAR *valuefmt() const;

		TCHAR *FormatOneDisplay(const TCHAR *szVal, CONSTVAL_t val, DisplayFormat_et dispfmt,
			TCHAR obuf[], int obufsize, const TCHAR *valfmt=nullptr) const;

	private:
		EnumGroup_st m_EnumC2V; // in the case of a single group
		bool m_dtor_delete_groups;

		const TCHAR *m_valfmt; 
		// -- format string when showing value, "%d", "%X", "0x%04X" etc

	private:
		ItcGroup_st *m_arGroups;
		int m_nGroups;

		void *ptr_unused1, *ptr_unused2, *ptr_unused3, *ptr_unused4;
	};


#define ITC_NAMEPAIR(macroname) { _T( #macroname ) , (CONSTVAL_t)macroname }


#define ITC_MAKE_OBJECT(itcobj_name, ...) \
	const CInterpretConst& itcobj_name() \
	{ \
		static const CInterpretConst sobj(__VA_ARGS__); \
		return sobj; \
	}


#define ITCS( val, itcobj) \
	(itcobj()).Interpret((itc::CONSTVAL_t)(val), itc::DF_NameOnly).getptr()
#define ITCS_(val, itcobj, valfmt) \
	(itcobj()).Interpret((itc::CONSTVAL_t)(val), itc::DF_NameOnly, valfmt).getptr()
#define ITCS__(val, itcobj, valfmt, sep) \
	(itcobj()).Interpret((itc::CONSTVAL_t)(val), itc::DF_NameOnly, valfmt, sep).getptr()
	// -- the "return" of ITCS() macro can be passed as snprintf's variadic params
	// Note: ITCS() cannot be used in __try{} block, otherwise we'll get Compiler Error C2712.

#define ITCSnv( val, itcobj) \
	(itcobj()).Interpret((itc::CONSTVAL_t)(val), itc::DF_NameAndValue).getptr()
#define ITCSnv_(val, itcobj, valfmt) \
	(itcobj()).Interpret((itc::CONSTVAL_t)(val), itc::DF_NameAndValue, valfmt).getptr()
#define ITCSnv__(val, itcobj, valfmt, sep) \
	(itcobj()).Interpret((itc::CONSTVAL_t)(val), itc::DF_NameAndValue, valfmt, sep).getptr()

#define ITCSvn( val, itcobj) \
	(itcobj()).Interpret((itc::CONSTVAL_t)(val), itc::DF_ValueAndName).getptr()
#define ITCSvn_(val, itcobj, valfmt) \
	(itcobj()).Interpret((itc::CONSTVAL_t)(val), itc::DF_ValueAndName, valfmt).getptr()
#define ITCSvn__(val, itcobj, valfmt, sep) \
	(itcobj()).Interpret((itc::CONSTVAL_t)(val), itc::DF_ValueAndName, valfmt, sep).getptr()

#define ITCSv( val, itcobj) \
	(itcobj()).Interpret((itc::CONSTVAL_t)(val), itc::DF_NVAuto).getptr()
#define ITCSv_(val, itcobj, valfmt) \
	(itcobj()).Interpret((itc::CONSTVAL_t)(val), itc::DF_NVAuto, valfmt).getptr()
#define ITCSv__(val, itcobj, valfmt, sep) \
	(itcobj()).Interpret((itc::CONSTVAL_t)(val), itc::DF_NVAuto, valfmt, sep).getptr()



#define ITCF_UINT _T("%u") // unsigned int (default)
#define ITCF_SINT _T("%d") // signed int

#define ITCF_HEX1B _T("0x%02X")
#define ITCF_HEX2B _T("0x%04X")
#define ITCF_HEX4B _T("0x%08X")


#ifndef InterpretConst_DEBUG
#undef vaDBG       // revoke empty effect
#endif


} // API namespace itc



#ifndef ARRAY_SIZE 
  // Clone of Visual C++ ARRAYSIZE(), but with a different name to use it on Linux as well.
extern"C++"
template <typename TElement, int N>
char(*
	RtlpNumberOf_commdefs(TElement(&rparam)[N])
	)[N];
#define ARRAY_SIZE(arr)  ( sizeof(*RtlpNumberOf_commdefs(arr)) )
#endif




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


#if defined(InterpretConst_IMPL) || (defined CHHI_ALL_IMPL && !defined CHHI_ALL_IMPL_HIDE_InterpretConst) // [IMPL]

#include "InterpretConst.IMPL.h"

#endif // [IMPL]


#endif // __InterpretConst_h_20251208_
