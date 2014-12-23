#pragma once

#include "stdafx.h"
#include <exdisp.h>
#include <comdef.h>

class ConnInfoUI;
class RedisInfoUI;

class CMainFrameWnd : public WindowImplBase
{
public:
    CMainFrameWnd();

    ~CMainFrameWnd();

    LPCTSTR GetWindowClassName() const;

    UINT GetClassStyle() const;

    void OnFinalMessage(HWND hWnd);

    virtual CDuiString GetSkinFolder();

    virtual CDuiString GetSkinFile();

    virtual UILIB_RESOURCETYPE GetResourceType() const;

    void InitWindow() ;

    CControlUI* CreateControl(LPCTSTR pstrClassName);

    void Notify(TNotifyUI& msg);

    virtual LRESULT HandleCustomMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

    DUI_DECLARE_MESSAGE_MAP()

    virtual void OnClick(TNotifyUI& msg);

    virtual void OnSelectChanged( TNotifyUI &msg );

    virtual void OnItemClick( TNotifyUI &msg );

    LRESULT OnConnected(HWND hwnd, WPARAM wParam, LPARAM lParam);

    LRESULT OnUnConnected(HWND hwnd, WPARAM wParam, LPARAM lParam);

    LRESULT OnConnecting(HWND hwnd, WPARAM wParam, LPARAM lParam);

	LRESULT OnUpdate(HWND hwnd, WPARAM wParam, LPARAM lParam);

public:
    void DoConnectEvent(void* data);

    void DoUnConnectEvent(void* data);

public:
	static DWORD WINAPI BackgroundWork(LPVOID lpParameter);
    static HWND m_hwnd;
    static void OndisConnectCallback(const CDuiString& name);

private:
    CButtonUI* m_pCloseBtn;
    CButtonUI* m_pMaxBtn;
    CButtonUI* m_pRestoreBtn;
    CButtonUI* m_pMinBtn;

    CHorizontalLayoutUI* m_pLayConnect;
    CHorizontalLayoutUI* m_pLayConnecting;
    CHorizontalLayoutUI* m_pLayUnconect;
    CTextUI* m_pConnectControl ;
    CTextUI* m_pUnConnectControl ;
    CTextUI* m_pConnectingControl ;
	CTextUI* m_pVersionControl;

    CStdStringPtrMap m_TabContainer;
};


