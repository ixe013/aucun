// selfserviceshell.cpp : main source file for selfserviceshell.exe
//

#include "stdafx.h"

#include "resource.h"

#include "selfserviceshellView.h"
#include "MainFrm.h"
#include "RESHelper.h"
#include "toolhelp_iterator.h"

CAppModule _Module;

int Run(LPTSTR /*lpstrCmdLine*/ = NULL, int nCmdShow = SW_SHOWDEFAULT)
{
	CMessageLoop theLoop;
	_Module.AddMessageLoop(&theLoop);

	CMainFrame wndMain;

	if(wndMain.CreateEx() == NULL)
	{
		ATLTRACE(_T("Main window creation failed!\n"));
		return 0;
	}

	wndMain.ShowWindow(nCmdShow);

	int nRet = theLoop.Run();

	_Module.RemoveMessageLoop();
	return nRet;
}

int WINAPI _tWinMain(HINSTANCE hInstance, HINSTANCE /*hPrevInstance*/, LPTSTR lpstrCmdLine, int nCmdShow)
{
	HRESULT hRes = ::CoInitialize(NULL);
// If you are running on NT 4.0 or higher you can use the following call instead to 
// make the EXE free threaded. This means that calls come in on a random RPC thread.
//	HRESULT hRes = ::CoInitializeEx(NULL, COINIT_MULTITHREADED);
	ATLASSERT(SUCCEEDED(hRes));

	// this resolves ATL window thunking problem when Microsoft Layer for Unicode (MSLU) is used
	::DefWindowProc(NULL, 0, 0, 0L);

	AtlInitCommonControls(ICC_COOL_CLASSES | ICC_BAR_CLASSES);	// add flags to support other controls

	hRes = _Module.Init(NULL, hInstance);
	ATLASSERT(SUCCEEDED(hRes));

	AtlAxWinInit();

	CRESHelper::Initialize(hInstance);

	int nRet = Run(lpstrCmdLine, nCmdShow);

	_Module.Term();
	::CoUninitialize();

	//If Explorer is not running, chances are we are the shell
	CProcessIterator process(0);
  while(process != CProcessIterator::End)
  {
	  if(_tcsstr(process->szExeFile, _T("explorer")))
	  {
		  //Found Explorer
		  break;
	  }

	  ++process;
  }

  if(process == CProcessIterator::End)
	//explorer.exe was not found, we must be the shell then...
	ExitWindowsEx(EWX_LOGOFF, 0);

	return nRet;
}
