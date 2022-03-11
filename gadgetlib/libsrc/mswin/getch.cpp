#include "stdafx.h"

#include <conio.h> // _getch()

#include <gadgetlib/getch.h>

#define DLL_AUTO_EXPORT_STUB
extern"C" void gadgetlib_lib__getch__DLL_AUTO_EXPORT_STUB(void){}

int 
ggt_getch(int timeout_millisec)
{
	if(timeout_millisec<0)
	{
		return _getch();
	}
	else if(timeout_millisec==ggt_getch_peek) // ==0
	{
		if(_kbhit())
			return _getch();
		else
			return 0;
	}
	else
	{
		if(_kbhit())
			return _getch();

		int frac_ms = 50;
		int cycles = timeout_millisec/50+1;
		int i;
		for(i=0; i<cycles; i++)
		{
			Sleep(frac_ms);
			if(_kbhit())
				return _getch();
		}

		return 0;
	}
}

