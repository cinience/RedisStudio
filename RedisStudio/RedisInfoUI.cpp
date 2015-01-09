#include "stdafx.h"
#include <sstream>
#include "Base/CharacterSet.h"
#include "RedisInfoUI.h"
#include "Redis/RedisClient.h"
#include "UserMessage.h"

static const TCHAR* kKeyStartButtonName  = _T("btn_refreshinfo_start");
static const TCHAR* kKeyStopButtonName   = _T("btn_refreshinfo_stop");

DUI_BEGIN_MESSAGE_MAP(RedisInfoUI, CNotifyPump)
DUI_ON_MSGTYPE(DUI_MSGTYPE_CLICK,OnClick)
DUI_ON_MSGTYPE(DUI_MSGTYPE_SELECTCHANGED,OnSelectChanged)
DUI_ON_MSGTYPE(DUI_MSGTYPE_ITEMCLICK,OnItemClick)
DUI_END_MESSAGE_MAP()

RedisInfoUI::RedisInfoUI( const CDuiString& strXML, CPaintManagerUI* pm, Environment* env):AbstraceUI(pm, env),m_bIsRefresh(false)
{
    CDialogBuilder builder;
    CControlUI* pContainer = builder.Create(strXML.GetData(), NULL, NULL, GetPaintMgr(), NULL); // 这里必须传入m_PaintManager，不然子XML不能使用默认滚动条等信息。
    if( pContainer ) {
        this->Add(pContainer);
    }
    else {
        this->RemoveAll();
        return;
    }
}

RedisInfoUI::~RedisInfoUI()
{
}

void RedisInfoUI::Initialize()
{
    m_RefreshStart = static_cast<CButtonUI*>(GetPaintMgr()->FindControl(kKeyStartButtonName));    
    m_RefreshStop  = static_cast<CButtonUI*>(GetPaintMgr()->FindControl(kKeyStopButtonName));    
}


int RedisInfoUI::GetIndex()
{
    return 1;
}

CDuiString RedisInfoUI::GetVirtualwndName()
{
    return _T("RedisInfo");
}

void RedisInfoUI::RefreshWnd()
{
    if (!m_Thread.isRunning()) DoRefreshWork();
}

LRESULT RedisInfoUI::HandleCustomMessage( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled )
{
    bHandled = TRUE;
    LRESULT lRes = TRUE; 
    switch(uMsg)
    {
    case WM_USER_INFOUPDATE: lRes = OnInfoRefresh(GetHWND(), wParam, lParam);
        break;
    default:
        lRes = FALSE;
        bHandled= FALSE;
        break;
    }
    return lRes;
}

void RedisInfoUI::OnClick( TNotifyUI& msg )
{
    if (msg.pSender == m_RefreshStart)
    {        
        m_bIsRefresh = true;
        DoRefreshWork();
        m_RefreshStop->SetVisible(true);
        m_RefreshStart->SetVisible(false);
    } 
    else if (msg.pSender == m_RefreshStop)
    {
        m_bIsRefresh = false;
        m_RefreshStop->SetVisible(false);
        m_RefreshStart->SetVisible(true);
    }
}

void RedisInfoUI::OnSelectChanged( TNotifyUI &msg )
{

}

void RedisInfoUI::OnItemClick( TNotifyUI &msg )
{

}

void RedisInfoUI::DoRefreshWork()
{
    m_pWork.reset(new Base::RunnableAdapter<RedisInfoUI>(*this, &RedisInfoUI::BackgroundWork));
    try
    {
         m_Thread.start(*m_pWork);
    }
    catch (std::exception& ex)
    {
        /// who care ?
        (void)(ex);
    }
}

void RedisInfoUI::RefreshInfo()
{
    DBClient* cli = Env()->GetDBClient();
    if (cli == NULL) return;

    if (!cli->IsConnected()) return;

    string info ;
    if (!cli->Info(info)) return;

    if (info.empty()) return;
    std::string* pInfo = new std::string(info);
    PostMessage(GetHWND(), WM_USER_INFOUPDATE, (WPARAM)pInfo, NULL);
}

void RedisInfoUI::BackgroundWork()
{
    const int sleepTime = 1000; /// ms
    do 
    {       
        RefreshInfo();

        /// 便于快速响应
        for (int i=0; m_bIsRefresh && i<sleepTime/10; ++i)
        {
            Base::Thread::sleep(10);
        }        
    } while (m_bIsRefresh);
}

LRESULT RedisInfoUI::OnInfoRefresh( HWND hwnd, WPARAM wParam, LPARAM lParam )
{
    std::string* pInfo = (std::string*)wParam;
    std::string& info = *pInfo;
    std::istringstream istr(info);
    std::string line;
    while (getline(istr, line))
    {
        int pos = line.find(":");
        if (pos == std::string::npos) continue;
        string key = line.substr(0, pos);
        string value = line.substr(pos+1, line.length()-pos);
        std::wstring controlName = Base::CharacterSet::ANSIToUnicode(key);
        std::wstring ControlText = Base::CharacterSet::ANSIToUnicode(value);
        CTextUI* p = static_cast<CTextUI*> (GetPaintMgr()->FindControl(controlName.c_str()));
        if (p)
        {
            p->SetText(ControlText.c_str());
            p->NeedUpdate();
        }
    }
    delete pInfo;
    return TRUE;
}
