#include "stdafx.h"
#include <sstream>
#include "CharacterSet.h"
#include "RedisInfoUI.h"
#include "RedisClient.h"


//const TCHAR 

DUI_BEGIN_MESSAGE_MAP(RedisInfoUI, CNotifyPump)
DUI_ON_MSGTYPE(DUI_MSGTYPE_CLICK,OnClick)
DUI_ON_MSGTYPE(DUI_MSGTYPE_SELECTCHANGED,OnSelectChanged)
DUI_ON_MSGTYPE(DUI_MSGTYPE_ITEMCLICK,OnItemClick)
DUI_END_MESSAGE_MAP()

bool RedisInfoUI::m_bIsRefresh = false;

RedisInfoUI::RedisInfoUI( const CDuiString& strXML, CPaintManagerUI* pm ):AbstraceUI(pm)
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


void RedisInfoUI::Initialize()
{
	m_RefreshStart = static_cast<CButtonUI*>(GetPaintMgr()->FindControl(_T("btn_refreshinfo_start")));
	m_RefreshStop = static_cast<CButtonUI*>(GetPaintMgr()->FindControl(_T("btn_refreshinfo_stop")));
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
	DoRefreshWork();
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

void RedisInfoUI::DoEvent( TEventUI& event )
{
    if (event.Type == UIEVENT_BUTTONDOWN)
	{

	}
}

void RedisInfoUI::DoRefreshWork()
{
	DWORD dwThreadID = 0;
	HANDLE hThread = CreateThread(NULL, 0, &RedisInfoUI::BackgroundWork, GetPaintMgr(),  0, &dwThreadID);
}

void RedisInfoUI::RefreshInfo(CPaintManagerUI* pm)
{
	if (!RedisClient::GetInstance().IsConnected()) return;

	string info ;
	if (!RedisClient::GetInstance().Info(info)) return;

	if (info.empty()) return;
    std::istringstream istr(info);
	std::string line;
	while (getline(istr, line))
	{
		int pos = line.find(":");
		if (pos == std::string::npos) continue;
		string key = line.substr(0, pos);
		string value = line.substr(pos+1, line.length()-pos);
		std::wstring controlName = CharacterSet::ANSIToUnicode(key);
		std::wstring ControlText = CharacterSet::ANSIToUnicode(value);
		CTextUI* p = static_cast<CTextUI*> (pm->FindControl(controlName.c_str()));
		if (p)
		{
			p->SetText(ControlText.c_str());
			p->NeedUpdate();
		}
	}
}

DWORD WINAPI RedisInfoUI::BackgroundWork(LPVOID lpParameter)
{
    do 
	{
		CPaintManagerUI* pm = (CPaintManagerUI*) lpParameter;
		RefreshInfo(pm);
		Sleep(1000);
	}
	while (m_bIsRefresh);

	return 0;
}
