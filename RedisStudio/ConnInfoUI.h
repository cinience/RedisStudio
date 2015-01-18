#pragma once

#include "stdafx.h"
#include <string>
#include <sstream>
#include <map>
#include "AbstractUI.h"
#include "hiredis/hiredis.h"
#include "Base/Event.h"
#include "Base/Thread.h"
#include "Base/RunnableAdapter.h"

class ConnInfoUI : public AbstraceUI,public IListCallbackUI 
{    
public:
    ConnInfoUI(const CDuiString& strXML, CPaintManagerUI* pm, Environment* env);

    ~ConnInfoUI();

    void Initialize();

    int GetIndex();

    CDuiString GetVirtualwndName();

    void RefreshWnd();

    bool LoadConfig(const std::string& path);

    bool SaveConfig(const std::string& path);

    LPCTSTR GetItemText(CControlUI* pControl, int iIndex, int iSubItem);

    DUI_DECLARE_MESSAGE_MAP()

    virtual void OnClick(TNotifyUI& msg);

    virtual void OnSelectChanged( TNotifyUI &msg );

    virtual void OnItemClick( TNotifyUI &msg );

    virtual void OnItemActive( TNotifyUI &msg );

    void OnAddConnInfo(TNotifyUI& msg);

    void OnDelConnInfo(TNotifyUI& msg);

    void OnAltConnInfo(TNotifyUI& msg);

    void OnRfhConnInfo(TNotifyUI& msg);

    void DoConnect();

private:
    typedef std::vector<std::string> TSeqColumn;
    typedef std::map<int, TSeqColumn> TDicTables;
    TDicTables m_dicServerInfo;

    CListUI* m_pListUI;

    Base::Thread     m_Thread;
    Base::Event      m_Event;
    std::auto_ptr< Base::RunnableAdapter<ConnInfoUI> > m_pWork;

public:
    void BackgroundWork(void);

public:
    static const int kServerNameIndex = 0;
    static const int kServerIpIndex =   1;
    static const int kServerPortIndex = 2;
    static const int kServerAuthIndex = 3;

private:
    CButtonUI* m_pAddBtn;
    CButtonUI* m_pDelBtn;
    CButtonUI* m_pAltBtn;
    CButtonUI* m_pRfhBtn;
};
