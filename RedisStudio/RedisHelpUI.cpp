#include "stdafx.h"
#include "RedisHelpUI.h"

DUI_BEGIN_MESSAGE_MAP(RedisHelpUI, CNotifyPump)
DUI_ON_MSGTYPE(DUI_MSGTYPE_CLICK,OnClick)
DUI_END_MESSAGE_MAP()

RedisHelpUI::RedisHelpUI(const CDuiString& strXML, CPaintManagerUI* pm) : AbstraceUI(pm)
{
	CDialogBuilder builder;
	// 这里必须传入m_PaintManager，不然子XML不能使用默认滚动条等信息。
	CControlUI* pContainer = builder.Create(strXML.GetData(), NULL, NULL, GetPaintMgr(), NULL); 
	if( pContainer ) {
		this->Add(pContainer);
	}
	else {
		this->RemoveAll();
		return;
	}
}

RedisHelpUI::~RedisHelpUI(void)
{
}

void RedisHelpUI::Initialize()
{
	CActiveXUI* pActiveXUI = static_cast<CActiveXUI*>(GetPaintMgr()->FindControl(_T("ie")));
	if( pActiveXUI ) {
		IWebBrowser2* pWebBrowser = NULL;
		pActiveXUI->GetControl(IID_IWebBrowser2, (void**)&pWebBrowser);
		// 忽略js错误
		pWebBrowser->put_Silent(true);
		if( pWebBrowser != NULL ) {
			pWebBrowser->Navigate(L"http://redis.io/commands",NULL,NULL,NULL,NULL);  
			//pWebBrowser->Navigate(L"about:blank",NULL,NULL,NULL,NULL); 
			pWebBrowser->Release();
		}
	}
}

int RedisHelpUI::GetIndex()
{
	return 5;
}

DuiLib::CDuiString RedisHelpUI::GetVirtualwndName()
{
	return _T("RedisHelp");
}

void RedisHelpUI::RefreshWnd()
{

}

void RedisHelpUI::OnClick( TNotifyUI& msg )
{

}
