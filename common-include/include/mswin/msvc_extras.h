#ifndef __msvc_extras_h_
#define __msvc_extras_h_
#define __msvc_extras_h_created_ 20251111
#define __msvc_extras_h_updated_ 20260526


#include <stdio.h>
#include <tchar.h> // to provide _tprintf() etc

#include <crtdbg.h> // _CrtMemCheckpoint()

#define MSVCRT_MemCheckStart(varprefix) \
	_CrtMemState varprefix##_state1 = {}, varprefix##_state2 = {}, varprefix##_stateDiff = {}; \
	_CrtMemCheckpoint(&varprefix##_state1); \
	// Take CRT MEM snapshot into state1

inline bool _msvcrt_MemCheckEnd_IsLeak(_CrtMemState &state1, _CrtMemState &state2, _CrtMemState &diff)
{
	// Take CRT MEM snapshot into state2
	_CrtMemCheckpoint(&state2);

	// Compare CRT MEM snapshot state1 & state2
	if (_CrtMemDifference(&diff, &state1, &state2))
	{
		printf("MSVCRT_MemCheckEnd_IsLeak() leak detected!!! See debug channel for details.\n");
		_CrtMemDumpStatistics(&diff);
		_CrtMemDumpAllObjectsSince(&state1);
		return true;
	}
	else
		return false;
}

#define MSVCRT_MemCheckEnd_IsLeak(varprefix) \
	_msvcrt_MemCheckEnd_IsLeak(varprefix##_state1, varprefix##_state2, varprefix##_stateDiff)



#endif // include once guard
