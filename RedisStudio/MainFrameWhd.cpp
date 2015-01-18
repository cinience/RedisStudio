#include "stdafx.h"
#include <exdisp.h>
#include <comdef.h>
#include <ShellAPI.h>
#include "resource.h"
#include "Version.h"
#include "MainFrameWhd.h"
#include "UserMessage.h"
#include "Redis/RedisClient.h"
#include "ConnInfoUI.h"
#include "RedisInfoUI.h"
#include "RedisDataUI.h"
#include "RedisConfigUI.h"
#include "RedisMgrUI.h"
#include "RedisHelpUI.h"

#include "Base/Http.h"
#include "Base/CharacterSet.h"
#include "Base/Util.h"

#include "rapidjson/document.h"     // rapidjson's DOM-style API
#include "rapidjson/prettywriter.h"    // for stringify JSON
#include "rapidjson/stringbuffer.h"
using namespace rapidjson;

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
    for (int idx=0; idx<m_oTabContainer.GetSize(); ++idx)
    {
        RemoveVirtualWnd(m_oTabContainer[idx]);
    }
}

void CMainFrameWnd::InitWindow() 
{
    SetIcon(IDI_ICON1);

    m_hwnd = GetHWND();
    //RedisClient::GetInstance().SetDisConnectCallback(&CMainFrameWnd::OndisConnectCallback);

    m_pCloseBtn = static_cast<CButtonUI*>(m_PaintManager.FindControl(_T("closebtn")));
    m_pMaxBtn = static_cast<CButtonUI*>(m_PaintManager.FindControl(_T("maxbtn")));
    m_pRestoreBtn = static_cast<CButtonUI*>(m_PaintManager.FindControl(_T("restorebtn")));
    m_pMinBtn = static_cast<CButtonUI*>(m_PaintManager.FindControl(_T("minbtn")));

    m_pVersionControl = static_cast<CTextUI*>(m_PaintManager.FindControl(_T("main_version")));
    m_pConnectControl = static_cast<CTextUI*>(m_PaintManager.FindControl(_T("txt_notice_success")));
    m_pUnConnectControl = static_cast<CTextUI*>(m_PaintManager.FindControl(_T("txt_notice_fail")));
    m_pConnectingControl = static_cast<CTextUI*>(m_PaintManager.FindControl(_T("txt_notice_connectting")));

    m_pLayConnect  = static_cast<CHorizontalLayoutUI*>(m_PaintManager.FindControl(_T("main_notice_success")));
    m_pLayConnecting = static_cast<CHorizontalLayoutUI*>(m_PaintManager.FindControl(_T("main_notice_connectting")));
    m_pLayUnconect  = static_cast<CHorizontalLayoutUI*>(m_PaintManager.FindControl(_T("main_notice_fail")));

    /// 设置版本号
    CDuiString theMsg = m_pVersionControl->GetText();
    theMsg.Replace(_T("$version"), _T(VERSION));
    m_pVersionControl->SetText(theMsg);

    for (int idx=0; idx<m_oTabContainer.GetSize(); ++idx)
    {
        AbstraceUI* p = (AbstraceUI*)m_oTabContainer.Find(m_oTabContainer[idx]);
        p->Initialize();

    }
    /// 刷新默认窗口加载数据
    for (int idx=0; idx<m_oTabContainer.GetSize(); ++idx)
    {
        AbstraceUI* p = (AbstraceUI*)m_oTabContainer.Find(m_oTabContainer[idx]);
        if (p->GetIndex() == 0)
        {
          p->RefreshWnd();
          break;
        }
    }

    DWORD dwThreadID = 0;
    HANDLE hThread = CreateThread(NULL, 0, &CMainFrameWnd::BackgroundWork, GetHWND(),  0, &dwThreadID);
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
    return UILIB_ZIPRESOURCE;
}

LPCTSTR CMainFrameWnd::GetResourceID() const
{
    return MAKEINTRESOURCE(IDR_ZIP_SKIN);
}

CControlUI* CMainFrameWnd::CreateControl(LPCTSTR pstrClassName)
{
    CDuiString     strXML;
    CDialogBuilder builder;
    AbstraceUI* p = NULL;
    
    if (_tcsicmp(pstrClassName, _T("ConnInfo")) == 0)
    {
        strXML = _T("ConnInfo.xml");
        p = new ConnInfoUI(strXML, &m_PaintManager, &m_pEnv);
    }
    else if (_tcsicmp(pstrClassName, _T("RedisInfo")) == 0)
    {
        strXML = _T("RedisInfo.xml");
        p = new RedisInfoUI(strXML, &m_PaintManager, &m_pEnv);
    }
    else if (_tcsicmp(pstrClassName, _T("RedisData")) == 0)
    {
        strXML = _T("RedisData.xml");
        p = new RedisDataUI(strXML, &m_PaintManager, &m_pEnv);
    }
    else if (_tcsicmp(pstrClassName, _T("RedisConfig")) == 0)
    {
        strXML = _T("RedisConfig.xml");
        p = new RedisConfigUI(strXML, &m_PaintManager, &m_pEnv);
    }
    else if (_tcsicmp(pstrClassName, _T("RedisMgr")) == 0)
    {
        strXML = _T("RedisMgr.xml");
        p = new RedisMgrUI(strXML, &m_PaintManager, &m_pEnv);
    }
    else if (_tcsicmp(pstrClassName, _T("RedisHelp")) == 0)
    {
        strXML = _T("RedisHelp.xml");
        p = new RedisHelpUI(strXML, &m_PaintManager, &m_pEnv);
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
        m_oTabContainer.Set(p->GetVirtualwndName(), p);
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
    DBClient* cli = m_pEnv.GetDBClient();
    if (name != _T("ConnInfo") &&
        name != _T("RedisHelp") &&(
        !cli ||
        !cli->IsConnected()))
    {
        pControl->SelectItem(0);
        COptionUI* pBut = static_cast<COptionUI*>(m_PaintManager.FindControl(_T("btn_ConnInfo")));
        pBut->Selected(true);
        UserMessageBox(GetHWND(), 10010, NULL, MB_ICONINFORMATION);
        return;
    }
    static COptionUI* pLastButton = NULL;
    static AbstraceUI* pLastTab = NULL;
    AbstraceUI* p = NULL;
    /// 除了tab的按钮的消息外，还有其它消息，所以此处只判断tab相关的消息
    for (int idx=0; idx<m_oTabContainer.GetSize(); ++idx)
    {
        if (name == m_oTabContainer[idx])
        {
            p = (AbstraceUI*)m_oTabContainer.Find(name);
        }
    }
    if (p == NULL) return;

    if (pLastTab != NULL && p != pLastTab && !pLastTab->CanChange()) 
    {
        if (pLastButton) pLastButton->Selected(true);
        UserMessageBox(GetHWND(), 10012, NULL, MB_ICONINFORMATION);
        return;
    }
    
    if (p && p!=pLastTab) 
    {
        p->RefreshWnd();
        pControl->SelectItem(p->GetIndex());
        pLastTab = p;
        pLastButton = static_cast<COptionUI*>(m_PaintManager.FindControl(msg.pSender->GetName()));
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
    // m_pConnectControl->NeedUpdate();
   
    m_pLayConnect->SetVisible(true);
    m_pLayConnecting->SetVisible(false);
    m_pLayUnconect->SetVisible(false);

    return TRUE;
}

LRESULT CMainFrameWnd::OnUnConnected( HWND hwnd, WPARAM wParam, LPARAM lParam )
{
    CDuiString *s = (CDuiString*)wParam;
    if (s != NULL)
    UserMessageBox(GetHWND(), 10016, s->GetData(), MB_ICONINFORMATION);
    m_pLayConnect->SetVisible(false);
    m_pLayConnecting->SetVisible(false);
    m_pLayUnconect->SetVisible(true);
    delete s;
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

LRESULT CMainFrameWnd::OnUpdate(HWND hwnd, WPARAM wParam, LPARAM lParam)
{
    std::string *data = (std::string*) (wParam);
    StringStream ss(data->c_str());
    Document document;
    if (document.ParseStream<0>(ss).HasParseError())
    {
        delete data; 
        return TRUE;
    }
    bool needNotice = false;
    if (document.HasMember("Notice")) 
    {
        needNotice = document["Notice"].GetBool();
    }
    if (needNotice) 
    {
        std::string version;
        if (document.HasMember("Version")) 
        {
            version = document["Version"].GetString();
        }
        if (version > VERSION) 
        {
            UserMessageBox(GetHWND(), 20000, Base::CharacterSet::ANSIToUnicode(version).c_str(), MB_ICONINFORMATION);
            //::ShellExecute(NULL, NULL, _T("https://github.com/cinience/RedisStudio/releases"), NULL, NULL, NULL); 
        }
    }
    delete data;
    return TRUE;
}

DWORD WINAPI CMainFrameWnd::BackgroundWork( LPVOID lpParameter )
{
    std::string code = Base::Util::GetUniqueMachineID();
    StringBuffer buffer;
    PrettyWriter<StringBuffer> writer(buffer);
    writer.StartObject();
    writer.String("Version");
    writer.String(VERSION);

    writer.String("MCode");
    writer.String(code.c_str());

    writer.EndObject();
    Base::Http http("hiredis.com", 80);
    Base::Http::Response rep = {0, ""};
    if (http.ping()) 
    {
        rep = http.post("/redisstudio/version", buffer.GetString());
    }

    if (rep.status == 200 && rep.data.size() > 0) 
    {
        std::string *data = new std::string(rep.data);
        ::PostMessage(m_hwnd, WM_USER_UPDATE, (WPARAM)data, NULL);
    }
    return 0;
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
    case WM_USER_UPDATE:
        lRes = OnUpdate(*this, wParam, lParam);
    default:
        bHandled = FALSE;
        break;
    }
    if (!bHandled)
    {
        for (int idx=0; idx<m_oTabContainer.GetSize(); ++idx)
        {
            AbstraceUI* p = (AbstraceUI*)m_oTabContainer.Find(m_oTabContainer[idx]);
            p->HandleCustomMessage(uMsg, wParam, lParam, bHandled);
        }
    }
    return lRes;
}
