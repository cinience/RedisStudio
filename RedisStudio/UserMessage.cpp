#include "StdAfx.h"
#include "UserMessage.h"

#include "MessageBoxWhd.h"

void UserMessageBox(HWND hWnd,
                    UINT ErrorCode,
                    LPCTSTR lpExMsg,
                    UINT uType)
{
    MessageBoxWhd::MsgType type = MessageBoxWhd::Info;
    if (uType == MB_ICONWARNING) type = MessageBoxWhd::Warn;
    else if (uType == MB_ICONERROR) type = MessageBoxWhd::Error;
    else if (uType == MB_ICONINFORMATION) type = MessageBoxWhd::Info;

    MessageBoxWhd* pWhd = new MessageBoxWhd(type, ErrorCode, lpExMsg);
    if( pWhd == NULL ) { return; }
    pWhd->Create(hWnd, NULL, UI_CLASSSTYLE_DIALOG, 0, 0, 0, 0, 0, NULL);
    pWhd->CenterWindow();
    pWhd->ShowModal();

}