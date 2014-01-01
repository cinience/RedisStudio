#pragma once

#include "stdafx.h"
#include "AbstractUI.h"

class CTreeView;

class RedisInfoUI : public AbstraceUI
{
public:
	RedisInfoUI(const CDuiString& strXML, CPaintManagerUI* pm);

	void Initialize();

	int GetIndex();
	
	CDuiString GetVirtualwndName();

	void RefreshWnd();

	DUI_DECLARE_MESSAGE_MAP()

	virtual void OnClick(TNotifyUI& msg);

	virtual void OnSelectChanged( TNotifyUI &msg );

	virtual void OnItemClick( TNotifyUI &msg );

	void         DoEvent(TEventUI& event);

	void         DoRefreshWork();

private:	
    static void  RefreshInfo(CPaintManagerUI* pm);

public:
    static DWORD WINAPI BackgroundWork(LPVOID lpParameter);

private:
	CButtonUI*       m_RefreshStart;
	CButtonUI*       m_RefreshStop;
	CPaintManagerUI* m_pPaintManager;
	CTreeNodeUI*     m_pServerInfoNode;
    static bool      m_bIsRefresh;
};