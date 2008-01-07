// test.cpp : Defines the entry point for the console application.
//

#include <windows.h>
#include <tchar.h>
#include <stdio.h>
#include "settings.h"
#include "unlockpolicy.h"



int _tmain(int argc, _TCHAR* argv[])
{
	if(argc > 1) for(int i=1; i<argc; ++i)
	{
		wchar_t passwd[256];
		HANDLE token = 0;

		wprintf(L"Enter password for %s : ", argv[i]);
		if (_getws_s(passwd, 255) == passwd)
		{
			//if(ShouldUnlockForUser(L"HYDRO-YN4PUBYPI", argv[i], passwd))
			if(ShouldUnlockForUser(0, argv[i], passwd))
			{
				wprintf(L"You are in\n");
			}
			else
			{
				wprintf(L"You are OUT\n");
			}
		}
	}

	return 0;
}

