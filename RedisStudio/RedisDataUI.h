#pragma once

#include "stdafx.h"
#include <string>
#include <vector>
#include <map>

#include "AbstractUI.h"
#include "RedisResult.h"

class CTreeView;

struct ContactData
{	
	HWND hwnd;
	CPaintManagerUI* paintManger;
};

struct TreeKeyContactData
{	
	CTreeNodeUI* pPNode;
	CTreeNodeUI* pNode;
};


class RedisDataUI : public AbstraceUI
{
public:
	RedisDataUI(const CDuiString& strXML, CPaintManagerUI* pm);

	void Initialize();

	int GetIndex();
	
	CDuiString GetVirtualwndName();

	void RefreshWnd();

	virtual LRESULT HandleCustomMessage( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled );

	DUI_DECLARE_MESSAGE_MAP()

	virtual void OnClick(TNotifyUI& msg);

	virtual void OnSelectChanged( TNotifyUI &msg );

	virtual void OnItemClick( TNotifyUI &msg );

	virtual void OnItemActive( TNotifyUI &msg );

	void OnItemActiveForTree( TNotifyUI &msg );

	void OnItemActiveForList( TNotifyUI &msg );

	void OnPaginate(TNotifyUI& msg);

	void OnCommit(TNotifyUI& msg);

	LRESULT OnDataAdd(HWND hwnd, WPARAM wParam, LPARAM lParam);

	LRESULT OnKeyAdd(HWND hwnd, WPARAM wParam, LPARAM lParam);

	void         DoRefreshKeysWork();

	void         DoRefreshValuesWork();

private:	

    static void  RefreshKeys(ContactData* pContactData);

	static void  RefreshValues(ContactData* pContactData);

	static bool  SetPageValues(CListUI* pList, HWND hwnd, std::size_t cur, RedisResult& results);

	static std::size_t  GetMaxPage();

	void  SetRichEditText(const std::string& text);

	bool  TryJsonFomat(std::string& text);

	bool  TryXMLFormat(std::string& text);

private:
	static CTreeNodeUI* NewNode(const string& text, bool isLeaf=false);

	static void DelChildNode(CTreeNodeUI* pNode);
public:
    static DWORD WINAPI BackgroundWorkForRefreshKeys(LPVOID lpParameter);

	static DWORD WINAPI BackgroundWorkForRefreshValues(LPVOID lpParameter);

private:
	CListUI* m_pList;
	CHorizontalLayoutUI* m_pHorizontalLayout;
	CEditUI*         m_pKeyEdit;
	CEditUI*         m_PTypeEdit;
	CEditUI*         m_pDataSizeEdit;

	CEditUI*         m_pPageCur;
	CButtonUI*       m_pPageFrist;
	CButtonUI*       m_pPageLast;
	CButtonUI*       m_pPageNext;
	CButtonUI*       m_pPageFinal;
	CButtonUI*       m_pCommit;
	CComboUI*        m_pComboFormat;

	CRichEditUI*     m_pRichEdit;
	CTreeNodeUI*     m_pServerInfoNode;

	ContactData      m_ConntactData;

	static bool      m_bIsLoading;
	static RedisResult m_Resulsts;
	static const std::size_t m_PageSize;
};