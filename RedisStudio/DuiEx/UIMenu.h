#ifndef __UIMENU_H__
#define __UIMENU_H__

#ifdef _MSC_VER
#pragma once
#endif

#include "ObserverBase.h"

namespace DuiLib {

/////////////////////////////////////////////////////////////////////////////////////
//
struct ContextMenuParam
{
	
    enum 
    {
        RemoveAll     = 1,
        RemoveSubMenu = 2
    };

	HWND        hWnd;         /// 当前窗口
    int         iType;        /// 事件类型
    CControlUI* pSender;
    bool        bLeaf;        /// 是否叶子节点菜单
    WPARAM      wParam;	    // 1: remove all
                            // 2: remove the sub menu
    LPARAM      lParam;
};

enum MenuAlignment
{
	eMenuAlignment_Left = 1 << 1,
	eMenuAlignment_Top = 1 << 2,
	eMenuAlignment_Right = 1 << 3,
	eMenuAlignment_Bottom = 1 << 4,
};

typedef class ObserverImpl<BOOL, ContextMenuParam> ContextMenuObserver;
typedef class ReceiverImpl<BOOL, ContextMenuParam> ContextMenuReceiver;

class CListUI;
class CMenuUI : public CListUI
{
public:
	CMenuUI();
	~CMenuUI();

    LPCTSTR GetClass() const;
    LPVOID GetInterface(LPCTSTR pstrName);

	virtual void DoEvent(TEventUI& event);

    virtual bool Add(CControlUI* pControl);
    virtual bool AddAt(CControlUI* pControl, int iIndex);

    virtual int  GetItemIndex(CControlUI* pControl) const;
    virtual bool SetItemIndex(CControlUI* pControl, int iIndex);
    virtual bool Remove(CControlUI* pControl);

	SIZE EstimateSize(SIZE szAvailable);

	void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);
};


class CMenuElementUI;
class CMenuWnd : public CWindowWnd, public ContextMenuReceiver
{
public:
	CMenuWnd(HWND hParent = NULL);
    void Init(CMenuElementUI* pOwner, STRINGorID xml, LPCTSTR pSkinType, POINT point);

    LPCTSTR GetWindowClassName() const;

    void OnFinalMessage(HWND hWnd);

    LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);

	BOOL Receive(ContextMenuParam param);

public:
	HWND m_hParent;
	POINT m_BasedPoint;
	STRINGorID      m_xml;
	CDuiString      m_sType;
    CPaintManagerUI m_pm;
    CMenuElementUI* m_pOwner;
    CMenuUI*        m_pLayout;

public:
    CEventSource    OnClick;
};

class CListContainerElementUI;
class CMenuElementUI : public CListContainerElementUI
{
	friend CMenuWnd;
public:
    CMenuElementUI();
	~CMenuElementUI();

    LPCTSTR GetClass() const;
    LPVOID  GetInterface(LPCTSTR pstrName);

    void    DoPaint(HDC hDC, const RECT& rcPaint);

    void    DrawItemText(HDC hDC, const RECT& rcItem);

    SIZE    EstimateSize(SIZE szAvailable);

	bool    Activate();

	void    DoEvent(TEventUI& event);

	CMenuWnd* GetMenuWnd();

	void    CreateMenuWnd();

protected:
	CMenuWnd* m_pWindow;
};

} // namespace DuiLib

#endif // __UIMENU_H__
