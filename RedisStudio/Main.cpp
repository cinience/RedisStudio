#include "StdAfx.h"
#include "MainFrameWhd.h"
#include "win32fix/winfix.h"

void win32Cleanup(void) {
	/* Clear winsocks */
	WSACleanup();
}



int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE /*hPrevInstance*/, LPSTR /*lpCmdLine*/, int nCmdShow)
{
	if (!w32initWinSock()) {
		printf("Winsock init error %d", WSAGetLastError());
		exit(1);
	};

    atexit((void(*)(void)) win32Cleanup);

	CPaintManagerUI::SetInstance(hInstance);

	HRESULT Hr = ::CoInitialize(NULL);
	if( FAILED(Hr) ) return 0;

	CMainFrameWnd* pFrame = new CMainFrameWnd();
	if( pFrame == NULL ) return 0;
	pFrame->Create(NULL, _T("Redis Studio"), UI_WNDSTYLE_FRAME, 0L, 0, 0, 800, 572);
	pFrame->CenterWindow();
	::ShowWindow(*pFrame, SW_SHOW);

	CPaintManagerUI::MessageLoop();

	::CoUninitialize();
	return 0;
}