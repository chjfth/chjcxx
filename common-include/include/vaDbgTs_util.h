#ifndef __vaDbgTs_util_h_20251117_
#define __vaDbgTs_util_h_20251117_

#include <vaDbgTs.h>

namespace vaDbgTs_util {

//////////////////////////////////////////////////////////////////////////

enum { vaDbgTs_util_nothing = 0 };

class DbgEnterExit
{
	// Print debug message on function Enter/Exit.
	// For Visual C++, write this line as function's first statement:
	//
	//	vaDbgTs_util::DbgEnterExit(_T(__FUNCTION__));
	//
	// and you get the Enter/Exit message.

public:
	DbgEnterExit(const TCHAR *msg)
	{
		m_msg = msg;
		vaDbgTs(_T("%s() >>>"), m_msg);
	}

	~DbgEnterExit()
	{
		vaDbgTs(_T("%s() <<<"), m_msg);
	}

private:
	const TCHAR *m_msg;
};

//////////////////////////////////////////////////////////////////////////

} // namespace

#endif

