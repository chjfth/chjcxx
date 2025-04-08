#pragma once

#include <windows.h>

inline unsigned __int64 get_qpf()
{
	LARGE_INTEGER li = {};
	BOOL succ = QueryPerformanceFrequency(&li);
	if(!succ)
		return -1;
	else
		return li.QuadPart;
}

inline unsigned __int64 get_qpc()
{
	LARGE_INTEGER li = {};
	BOOL succ = QueryPerformanceCounter(&li);
	if(!succ)
		return -1;
	else
		return li.QuadPart;
}

DWORD trueGetTickCount()
{
	static __int64 s_qpf = get_qpf();

	// We should use QueryPerformanceCounter(), 
	// bcz GetTickCount() only has 15.6 ms resolution.
	DWORD millisec = DWORD(get_qpc()*1000 / s_qpf);
	return millisec;
}

