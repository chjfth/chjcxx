#ifndef __vaDbgTs_util_h_20251117_20260402_
#define __vaDbgTs_util_h_20251117_20260402_

#include <vaDbgTs.h>

namespace vaDbgTs_util {

//////////////////////////////////////////////////////////////////////////

//enum { vaDbgTs_util_nothing = 0 };

template<int Lv>
class DbgEnterExit
{
	// Print debug message on function Enter/Exit.
	// For Visual C++, write this line as function's first statement:
	//
	//	vaDbgTs_util::DbgEnterExit(_T(__FUNCTION__));
	//
	// and you get the Enter/Exit message.

public:
	DbgEnterExit(const TCHAR *funcname)
	{
		m_funcname = funcname;
		vaDbgTsl(_T("%s() >>>"), m_funcname);
	}

	~DbgEnterExit()
	{
		vaDbgTs(_T("%s() <<<"), m_funcname);
	}

private:
	const TCHAR *m_funcname;
};





//////////////////////////////////////////////////////////////////////////

} // namespace

#endif

