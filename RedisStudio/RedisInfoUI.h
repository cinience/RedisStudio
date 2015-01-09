#pragma once

#include "stdafx.h"
#include "AbstractUI.h"

#include "Base/RunnableAdapter.h"
#include "Base/Thread.h"

class CTreeView;

class RedisInfoUI : public AbstraceUI
{
public:
    RedisInfoUI(const CDuiString& strXML, CPaintManagerUI* pm, Environment* env);

    ~RedisInfoUI();

    void Initialize();

    int GetIndex();
    
    CDuiString GetVirtualwndName();

    void RefreshWnd();

    DUI_DECLARE_MESSAGE_MAP()

    virtual LRESULT HandleCustomMessage( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled );

    virtual void OnClick(TNotifyUI& msg);

    virtual void OnSelectChanged( TNotifyUI &msg );

    virtual void OnItemClick( TNotifyUI &msg );

public:
    LRESULT OnInfoRefresh(HWND hwnd, WPARAM wParam, LPARAM lParam);

public:
    void  DoRefreshWork();

    void  BackgroundWork();

private:    
    void  RefreshInfo();

private:
    CButtonUI*       m_RefreshStart;
    CButtonUI*       m_RefreshStop;
    CTreeNodeUI*     m_pServerInfoNode;
    bool             m_bIsRefresh;
    Base::Thread     m_Thread;
    std::auto_ptr< Base::RunnableAdapter<RedisInfoUI> > m_pWork;
};
