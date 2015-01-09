#include "stdafx.h"
#include "RedisMgrUI.h"

DUI_BEGIN_MESSAGE_MAP(RedisMgrUI, CNotifyPump)
DUI_ON_MSGTYPE(DUI_MSGTYPE_CLICK,OnClick)
DUI_END_MESSAGE_MAP()

RedisMgrUI::RedisMgrUI(const CDuiString& strXML, CPaintManagerUI* pm, Environment* env) : AbstraceUI(pm, env)
{
    CDialogBuilder builder;
    /// 这里必须传入m_PaintManager，不然子XML不能使用默认滚动条等信息。
    CControlUI* pContainer = builder.Create(strXML.GetData(), NULL, NULL, GetPaintMgr(), NULL); 
    if( pContainer ) {
        this->Add(pContainer);
    }
    else {
        this->RemoveAll();
        return;
    }
}

RedisMgrUI::~RedisMgrUI(void)
{
}

void RedisMgrUI::Initialize()
{

}

int RedisMgrUI::GetIndex()
{
    return 4;
}

DuiLib::CDuiString RedisMgrUI::GetVirtualwndName()
{
    return _T("RedisMgr");
}

void RedisMgrUI::RefreshWnd()
{

}

void RedisMgrUI::OnClick( TNotifyUI& msg )
{

}
