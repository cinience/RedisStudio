#include "StdAfx.h"
#include <shellapi.h>
#include "MainFrameWhd.h"
#include "win32fix/winfix.h"


void win32Cleanup(void) {
    /* Clear winsocks */
    WSACleanup();
}

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE /*hPrevInstance*/, LPSTR /*lpCmdLine*/, int nCmdShow)
{
#if 0
    /* avoid UAC */
    TCHAR szFileName[_MAX_PATH] = {0};

    GetModuleFileName(NULL, szFileName, _MAX_PATH);
    for (int i = sizeof(szFileName)-1; i>=0; i--)
    {
         if (szFileName[i]=='\\')
         {
             szFileName[i] = 0;
             SetCurrentDirectory(szFileName);
             break;
         }
    }
#endif

    if (!w32initWinSock()) {
        printf("Winsock init error %d", WSAGetLastError());
        exit(1);
    };
    
    atexit((void(*)(void)) win32Cleanup);

    CPaintManagerUI::SetInstance(hInstance);

    HRESULT Hr = ::CoInitialize(NULL);
    if ( FAILED(Hr) ) return 0;

    CMainFrameWnd* pFrame = new CMainFrameWnd();
    
    if ( pFrame == NULL ) return 0;
    pFrame->Create(NULL, _T("Redis Studio"), UI_WNDSTYLE_FRAME, 0L, 0, 0, 800, 572);
    pFrame->CenterWindow();
    ::ShowWindow(*pFrame, SW_SHOW);

    CPaintManagerUI::MessageLoop();

    ::CoUninitialize();
    return 0;
}