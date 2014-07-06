#include "stdafx.h"
#include <exdisp.h>
#include <comdef.h>
#include <ShellAPI.h>
#include "resource.h"
#include "MainFrameWhd.h"
#include "UserMessage.h"
#include "Redis/RedisClient.h"
#include "ConnInfoUI.h"
#include "RedisInfoUI.h"
#include "RedisDataUI.h"
#include "RedisConfigUI.h"
#include "RedisMgrUI.h"
#include "RedisHelpUI.h"


DUI_BEGIN_MESSAGE_MAP(CMainFrameWnd, WindowImplBase)
DUI_ON_MSGTYPE(DUI_MSGTYPE_CLICK,OnClick)
DUI_ON_MSGTYPE(DUI_MSGTYPE_SELECTCHANGED,OnSelectChanged)
DUI_ON_MSGTYPE(DUI_MSGTYPE_ITEMCLICK,OnItemClick)
DUI_END_MESSAGE_MAP()


HWND CMainFrameWnd::m_hwnd = NULL;

CMainFrameWnd::CMainFrameWnd()
{
}

CMainFrameWnd::~CMainFrameWnd()
{
    for (int idx=0; idx<m_TabContainer.GetSize(); ++idx)
    {
        RemoveVirtualWnd(m_TabContainer[idx]);
    }
}

void CMainFrameWnd::InitWindow() 
{
    SetIcon(IDI_ICON1);

    m_hwnd = GetHWND();
    RedisClient::GetInstance().SetDisConnectCallback(&CMainFrameWnd::OndisConnectCallback);
    m_pCloseBtn = static_cast<CButtonUI*>(m_PaintManager.FindControl(_T("closebtn")));
    m_pMaxBtn = static_cast<CButtonUI*>(m_PaintManager.FindControl(_T("maxbtn")));
    m_pRestoreBtn = static_cast<CButtonUI*>(m_PaintManager.FindControl(_T("restorebtn")));
    m_pMinBtn = static_cast<CButtonUI*>(m_PaintManager.FindControl(_T("minbtn")));


    m_pConnectControl = static_cast<CTextUI*>(m_PaintManager.FindControl(_T("txt_notice_success")));
    m_pUnConnectControl = static_cast<CTextUI*>(m_PaintManager.FindControl(_T("txt_notice_fail")));
    m_pConnectingControl = static_cast<CTextUI*>(m_PaintManager.FindControl(_T("txt_notice_connectting")));

    m_pLayConnect  = static_cast<CHorizontalLayoutUI*>(m_PaintManager.FindControl(_T("main_notice_success")));
    m_pLayConnecting = static_cast<CHorizontalLayoutUI*>(m_PaintManager.FindControl(_T("main_notice_connectting")));
    m_pLayUnconect  = static_cast<CHorizontalLayoutUI*>(m_PaintManager.FindControl(_T("main_notice_fail")));

    for (int idx=0; idx<m_TabContainer.GetSize(); ++idx)
    {
        AbstraceUI* p = (AbstraceUI*)m_TabContainer.Find(m_TabContainer[idx]);
        p->Initialize();

    }
    //刷新默认窗口加载数据
    for (int idx=0; idx<m_TabContainer.GetSize(); ++idx)
    {
        AbstraceUI* p = (AbstraceUI*)m_TabContainer.Find(m_TabContainer[idx]);
        if (p->GetIndex() == 0)
        {
          p->RefreshWnd();
          break;
        }
    }
}

LPCTSTR CMainFrameWnd::GetWindowClassName() const
{
    return _T("MainFrameWhd");
}

UINT CMainFrameWnd::GetClassStyle() const
{
    return CS_DBLCLKS;
}

void CMainFrameWnd::OnFinalMessage( HWND hWnd )
{
    delete this;
}

DuiLib::CDuiString CMainFrameWnd::GetSkinFolder()
{
    return _T("skin\\");
}

DuiLib::CDuiString CMainFrameWnd::GetSkinFile()
{
    return _T("skin.xml");
}

//DuiLib::CDuiString CMainFrameWnd::GetZIPFileName()
//{
//    return _T("");
//}

DuiLib::UILIB_RESOURCETYPE CMainFrameWnd::GetResourceType() const
{
    return UILIB_FILE;
}

CControlUI* CMainFrameWnd::CreateControl(LPCTSTR pstrClassName)
{
    CDuiString     strXML;
    CDialogBuilder builder;
    AbstraceUI* p = NULL;

    if (_tcsicmp(pstrClassName, _T("ConnInfo")) == 0)
    {
        strXML = _T("ConnInfo.xml");
        p = new ConnInfoUI(strXML, &m_PaintManager);
    }
    else if (_tcsicmp(pstrClassName, _T("RedisInfo")) == 0)
    {
        strXML = _T("RedisInfo.xml");
        p = new RedisInfoUI(strXML, &m_PaintManager);
    }
    else if (_tcsicmp(pstrClassName, _T("RedisData")) == 0)
    {
        strXML = _T("RedisData.xml");
        p = new RedisDataUI(strXML, &m_PaintManager);
    }
    else if (_tcsicmp(pstrClassName, _T("RedisConfig")) == 0)
    {
        strXML = _T("RedisConfig.xml");
        p = new RedisConfigUI(strXML, &m_PaintManager);
    }
    else if (_tcsicmp(pstrClassName, _T("RedisMgr")) == 0)
    {
        strXML = _T("RedisMgr.xml");
        p = new RedisMgrUI(strXML, &m_PaintManager);
    }
    else if (_tcsicmp(pstrClassName, _T("RedisHelp")) == 0)
    {
        strXML = _T("RedisHelp.xml");
        p = new RedisHelpUI(strXML, &m_PaintManager);
    }
    //if (! strXML.IsEmpty())
    //{
    //    CControlUI* pUI = builder.Create(strXML.GetData(), NULL, NULL, &m_PaintManager, NULL); // 这里必须传入m_PaintManager，不然子XML不能使用默认滚动条等信息。
    //    return pUI;
    //}

    if (p)
    {
        AddVirtualWnd(p->GetVirtualwndName(), p);
        p->SetHWND(GetHWND());
        m_TabContainer.Set(p->GetVirtualwndName(), p);
        return p;
    }
    return NULL;
}

void CMainFrameWnd::Notify(TNotifyUI& msg)
{
    return WindowImplBase::Notify(msg);
}


void CMainFrameWnd::OnClick( TNotifyUI& msg )
{
    if( msg.pSender == m_pCloseBtn ) 
    {
        PostQuitMessage(0);  
        return; 
    }
    else if( msg.pSender == m_pMinBtn ) 
    { 
        SendMessage(WM_SYSCOMMAND, SC_MINIMIZE, 0); 
        return; 
    }
    else if( msg.pSender == m_pMaxBtn ) 
    { 
        SendMessage(WM_SYSCOMMAND, SC_MAXIMIZE, 0); 
        return; 
    }
    else if( msg.pSender == m_pRestoreBtn )
    { 
        SendMessage(WM_SYSCOMMAND, SC_RESTORE, 0); 
        return; 
    }
    else if (msg.pSender->GetName() == _T("Issue"))
    {
        ::ShellExecute(NULL, NULL, _T("https://github.com/cinience/RedisStudio/issues"), NULL, NULL, NULL); 
    }
    else if (msg.pSender->GetName() == _T("Star"))
    {
        ::ShellExecute(NULL, NULL, _T("https://github.com/cinience/RedisStudio/stargazers"), NULL, NULL, NULL);
    }
}

void CMainFrameWnd::OnSelectChanged( TNotifyUI &msg )
{
    CDuiString name = msg.pSender->GetName();
    name.Replace(_T("btn_"), _T(""));
    CTabLayoutUI* pControl = static_cast<CTabLayoutUI*>(m_PaintManager.FindControl(_T("switch")));
    
    if (name != _T("ConnInfo") &&
        name != _T("RedisHelp") &&
        !RedisClient::GetInstance().IsConnected())
    {
        pControl->SelectItem(0);
        COptionUI* pBut = static_cast<COptionUI*>(m_PaintManager.FindControl(_T("btn_ConnInfo")));
        pBut->Selected(true);
        UserMessageBox(GetHWND(), 10010, NULL, MB_ICONINFORMATION);
        return;
    }

    for (int idx=0; idx<m_TabContainer.GetSize(); ++idx)
    {
        if (name == m_TabContainer[idx])
        {
            AbstraceUI* p = (AbstraceUI*)m_TabContainer.Find(name);
            p->RefreshWnd();
            pControl->SelectItem(p->GetIndex());
            break;
        }        
    }
}

void CMainFrameWnd::OnItemClick( TNotifyUI &msg )
{

}

void CMainFrameWnd::DoConnectEvent( void* data )
{
    SendMessage(WM_USER_CONNECTED, (WPARAM)data);
}

void CMainFrameWnd::DoUnConnectEvent( void* data )
{
    SendMessage(WM_USER_UNCONNECT, (WPARAM)data);
}

LRESULT CMainFrameWnd::OnConnected( HWND hwnd, WPARAM wParam, LPARAM lParam )
{    
    static const CDuiString OldText = m_pConnectControl->GetText();

    CDuiString* s = (CDuiString*)(wParam);
    CDuiString newText = OldText;
    newText.Replace(_T("$"), s->GetData());
    delete s;
    m_pConnectControl->SetText(newText);
    // m_pConnectControl->SetVisible(true);
    /*m_pConnectControl->NeedUpdate();*/
   
    m_pLayConnect->SetVisible(true);
    m_pLayConnecting->SetVisible(false);
    m_pLayUnconect->SetVisible(false);

    return TRUE;
}

LRESULT CMainFrameWnd::OnUnConnected( HWND hwnd, WPARAM wParam, LPARAM lParam )
{
    m_pLayConnect->SetVisible(false);
    m_pLayConnecting->SetVisible(false);
    m_pLayUnconect->SetVisible(true);
    return TRUE;
}

LRESULT CMainFrameWnd::OnConnecting( HWND hwnd, WPARAM wParam, LPARAM lParam )
{
    static const CDuiString OldText = m_pConnectingControl->GetText();

    CDuiString* s = (CDuiString*)(wParam);
    CDuiString newText = OldText;
    newText.Replace(_T("$"), s->GetData());
    delete s;
    m_pConnectingControl->SetText(newText);
    m_pLayConnect->SetVisible(false);
    m_pLayConnecting->SetVisible(true);
    m_pLayUnconect->SetVisible(false);
    return TRUE;
}

void CMainFrameWnd::OndisConnectCallback( const CDuiString& name )
{
    CDuiString* s = new CDuiString(name);
    ::PostMessage(m_hwnd, WM_USER_UNCONNECT, (WPARAM) s, NULL);
}

LRESULT CMainFrameWnd::HandleCustomMessage( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled )
{
    LRESULT lRes = TRUE;
    bHandled = TRUE;
    switch(uMsg)
    {
    case WM_USER_CONNECTED:
        lRes = OnConnected(*this, wParam, lParam);
        break;
    case WM_USER_UNCONNECT:
        lRes = OnUnConnected(*this, wParam, lParam);
        break;
    case WM_USER_CONNECTING:
        lRes = OnConnecting(*this, wParam, lParam);
        break;
    default:
        bHandled = FALSE;
        break;
    }
    if (!bHandled)
    {
        for (int idx=0; idx<m_TabContainer.GetSize(); ++idx)
        {
            AbstraceUI* p = (AbstraceUI*)m_TabContainer.Find(m_TabContainer[idx]);
            p->HandleCustomMessage(uMsg, wParam, lParam, bHandled);
        }
    }
    return lRes;
}





