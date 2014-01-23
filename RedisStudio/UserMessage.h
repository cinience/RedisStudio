#pragma once

#define WM_USER_CONNECTED         WM_USER + 1     // 成功连接
#define WM_USER_UNCONNECT         WM_USER + 2     // 失败连接
#define WM_USER_CONNECTING        WM_USER + 3     // 失败连接


#define WM_USER_INFOUPDATE        WM_USER + 5     // info get 


#define WM_USER_TREEADD           WM_USER + 10     // 
#define WM_USER_DATAADD           WM_USER + 11     // 

#define WM_USER_DATAVERBOSE       WM_USER + 21

#define WM_USER_MENU_KEYDEL       WM_USER + 31
#define WM_USER_MENU_KEYRENAME    WM_USER + 32

void UserMessageBox(HWND hWnd,
					UINT ErrorCode,
					LPCTSTR lpExMsg,
					UINT uType);