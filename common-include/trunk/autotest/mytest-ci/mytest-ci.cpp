// mytest-ci.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#ifdef _MSC_VER
int _tmain(int argc, TCHAR *argv[])
#else
int main(int argc, char *argv[])
#endif
{
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}

