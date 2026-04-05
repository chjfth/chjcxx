#ifndef __vaDbgTs_util_h_20251117_20260402_
#define __vaDbgTs_util_h_20251117_20260402_

#include <vaDbgTs.h>

namespace vaDbgTs_util {

//////////////////////////////////////////////////////////////////////////

#				ifndef hashdict_DEBUG
#				include <CHHI_vaDBG_hide.h> // Suppress/invalidate vaDBG macro, from now on
#				endif

class DbgEnterExit3
{
	// Print debug message on function Enter/Exit.
	// For Visual C++, just write this line as function's first statement:
	//
	//	vaDbgTs_util::DbgEnterExit3 objDbgEnterExit(_T(__FUNCTION__));
	// 
	// or use macro:
	//
	//	vaDBG3_DbgEnterExit;
	//
	// and you get the Enter/Exit message.

public:
	DbgEnterExit3(const TCHAR *funcname)
	{
		m_funcname = funcname;
		vaDBG3(_T("%s() >>>"), m_funcname);
	}

	~DbgEnterExit3()
	{
		vaDBG3(_T("%s() <<<"), m_funcname);
	}

private:
	const TCHAR *m_funcname;
};

#				ifndef hashdict_DEBUG
#				include <CHHI_vaDBG_show.h> // Now restore vaDBG macros
#				endif

//////////////////////////////////////////////////////////////////////////

} // namespace

#define vaDBG3_DbgEnterExit vaDbgTs_util::DbgEnterExit3 objDbgEnterExit(_T(__FUNCTION__));

#endif

