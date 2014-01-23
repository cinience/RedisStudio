#include "stdafx.h"
#include "MessageBoxWhd.h"
#include <sstream>
#include "Base/CharacterSet.h"

DUI_BEGIN_MESSAGE_MAP(MessageBoxWhd, WindowImplBase)
DUI_ON_MSGTYPE(DUI_MSGTYPE_CLICK,OnClick)
DUI_END_MESSAGE_MAP()

MessageBoxWhd::MessageBoxWhd(MsgType type, UINT code, CDuiString msg)
{
	m_uType = type;
	m_uMsgCode = code;
	m_dstrExMsg = msg;
}

MessageBoxWhd::~MessageBoxWhd(void)
{
}

void MessageBoxWhd::InitWindow()
{
	CLabelUI* pLbl = NULL;
	CControlUI* pCtl = NULL;
	if (m_uType == Info)
	{
		 pLbl = static_cast<CLabelUI*> (m_PaintManager.FindControl(_T("lab_msg_info")));
		 pCtl =  static_cast<CLabelUI*> (m_PaintManager.FindControl(_T("ctl_msg_info")));
	}
	else if (m_uType == Warn)
	{
		pLbl = static_cast<CLabelUI*> (m_PaintManager.FindControl(_T("lab_msg_warn")));
		pCtl = static_cast<CLabelUI*> (m_PaintManager.FindControl(_T("ctl_msg_warn")));
	}
	else if (m_uType == Error)
	{
		pLbl = static_cast<CLabelUI*> (m_PaintManager.FindControl(_T("lab_msg_error")));
		pCtl =  static_cast<CLabelUI*> (m_PaintManager.FindControl(_T("ctl_msg_error")));
	}
	pLbl->SetVisible(true);
	pCtl->SetVisible(true);

	std::stringstream ss;
	ss<<"lbl_msg_"<<m_uMsgCode;
	CLabelUI* pTextUI = static_cast<CLabelUI*> (m_PaintManager.FindControl(Base::CharacterSet::ANSIToUnicode(ss.str()).c_str()));
	if (pTextUI)
	{
		pTextUI->SetVisible(true);
		if (!m_dstrExMsg.IsEmpty())
		{
			CDuiString theMsg = pTextUI->GetText();
			theMsg.Replace(_T("$"), m_dstrExMsg);
			pTextUI->SetText(theMsg);
		}
	}
}

LPCTSTR MessageBoxWhd::GetWindowClassName() const
{
	return _T("MessageBoxWhd");
}

UINT MessageBoxWhd::GetClassStyle() const
{
	return UI_CLASSSTYLE_DIALOG;
}


void MessageBoxWhd::OnFinalMessage( HWND hWnd)
{
	delete this;
}

DuiLib::CDuiString MessageBoxWhd::GetSkinFolder()
{
	return _T("skin\\");
}

DuiLib::CDuiString MessageBoxWhd::GetSkinFile()
{
	return _T("MessageBox.xml");
}

DuiLib::UILIB_RESOURCETYPE MessageBoxWhd::GetResourceType() const
{
	return UILIB_FILE;
}

void MessageBoxWhd::OnClick( TNotifyUI& msg )
{
	if (msg.pSender->GetName() == _T("btn_msg_ok"))
	{
		Close();
	}
	if (msg.pSender->GetName() == _T("closebtn"))
	{
		Close();
	}
}