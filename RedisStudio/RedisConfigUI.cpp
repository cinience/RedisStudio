#include "stdafx.h"
#include "RedisConfigUI.h"
#include "Redis/RedisClient.h"
#include "Base/CharacterSet.h"

DUI_BEGIN_MESSAGE_MAP(RedisConfigUI, CNotifyPump)
DUI_ON_MSGTYPE(DUI_MSGTYPE_CLICK,OnClick)
DUI_ON_MSGTYPE(DUI_MSGTYPE_SELECTCHANGED,OnSelectChanged)
DUI_ON_MSGTYPE(DUI_MSGTYPE_ITEMCLICK,OnItemClick)
DUI_ON_MSGTYPE(DUI_MSGTYPE_ITEMACTIVATE,OnItemActive)
DUI_END_MESSAGE_MAP()

RedisConfigUI::RedisConfigUI(const CDuiString& strXML, CPaintManagerUI* pm, Environment* env):AbstraceUI(pm, env)
{    
    CDialogBuilder builder;
    /// 这里必须传入m_PaintManager，不然子XML不能使用默认滚动条等信息。
    CControlUI* pContainer = builder.Create(strXML.GetData(), NULL, NULL, GetPaintMgr(), NULL); 
    if (pContainer) 
    {
        this->Add(pContainer);
    }
    else {
        this->RemoveAll();
        return;
    }
}

RedisConfigUI::~RedisConfigUI(void)
{
}

void RedisConfigUI::Initialize()
{
}

DuiLib::CDuiString RedisConfigUI::GetVirtualwndName()
{
    return _T("RedisConfig");
}

void RedisConfigUI::RefreshWnd()
{
    if (GetConfig()) 
    {
        DoFillData();
    }
}    

int RedisConfigUI::GetIndex()
{
    return 3;
}

void RedisConfigUI::OnClick(TNotifyUI& msg)
{
    if (msg.pSender->GetName() == _T("btn_config_submit"))
    {
        DoApplyData();
    }
    else if (msg.pSender->GetName() == _T("btn_config_sync"))
    {
		Env()->GetDBClient()->ReWriteConfig();
    }
}

void RedisConfigUI::OnSelectChanged( TNotifyUI &msg )
{

}

void RedisConfigUI::OnItemClick( TNotifyUI &msg )
{

}

void RedisConfigUI::OnItemActive( TNotifyUI &msg )
{

}

bool RedisConfigUI::GetConfig()
{
    //if (!RedisClient::GetInstance().GetConfig(m_dicConfig)) return false;
	if (!Env()->GetDBClient()->GetConfig(m_dicConfig)) return false;
    return true;
}

bool RedisConfigUI::SetConfig(const RedisClient::TDicConfig& config)
{
    return Env()->GetDBClient()->SetConfig(config);
}

void RedisConfigUI::DoFillData()
{
    CEditUI exampleEditUI;
    CComboUI exampleComboUI;

    RedisClient::TDicConfig::const_iterator it = m_dicConfig.begin();
    RedisClient::TDicConfig::const_iterator itend = m_dicConfig.end();

    for ( ; it!=itend; ++it)
    {
        CDuiString name = _T("redisconfig_");
        CDuiString key = Base::CharacterSet::ANSIToUnicode(it->first).c_str();
        CDuiString val = Base::CharacterSet::ANSIToUnicode(it->second).c_str();
        name.Append(key);
        CControlUI* pCtl = GetPaintMgr()->FindControl(name);

        /// xml中未配置该项
        if (pCtl == NULL) continue;

        if (pCtl->GetClass() == exampleEditUI.GetClass())
        {
            CEditUI* pEdit = static_cast<CEditUI*> (pCtl);
            pEdit->SetText(val);
        }
        else if (pCtl->GetClass() == exampleComboUI.GetClass())
        {
            CComboUI* pCombo = static_cast<CComboUI*> (pCtl);
            int idx = 0;
            if (val == _T("yes")) idx = 0;
            else if (val == _T("no")) idx = 1;
            else if (it->first == "loglevel")
            {
                string loglevel = it->second;
                if (loglevel == "debug") idx = 0;
                else if (loglevel == "verbose") idx = 1;
                else if (loglevel == "notice") idx = 2;
                else if (loglevel == "warning") idx = 3;
            }
            pCombo->SetInternVisible(true);
            pCombo->SelectItem(idx);
            pCombo->SetFocus();
        }
    }
}

void RedisConfigUI::DoApplyData()
{
    CEditUI exampleEditUI;
    CComboUI exampleComboUI;

    RedisClient::TDicConfig::const_iterator it = m_dicConfig.begin();
    RedisClient::TDicConfig::const_iterator itend = m_dicConfig.end();
    RedisClient::TDicConfig config;
    for ( ; it!=itend; ++it)
    {
        CDuiString name = _T("redisconfig_");
        CDuiString key = Base::CharacterSet::ANSIToUnicode(it->first).c_str();    
        name.Append(key);
        CControlUI* pCtl = GetPaintMgr()->FindControl(name);
        if (pCtl == NULL) continue;
        string val ;
        if (pCtl->GetClass() == exampleEditUI.GetClass())
        {
            CEditUI* pEdit = static_cast<CEditUI*> (pCtl);
            val = Base::CharacterSet::UnicodeToANSI(pEdit->GetText().GetData());
        }
        else if (pCtl->GetClass() == exampleComboUI.GetClass())
        {
            CComboUI* pCombo = static_cast<CComboUI*> (pCtl);
            int idx = pCombo->GetCurSel();
            if (it->first == "loglevel")
            {
                if (idx == 0) val = "debug";
                else if (idx == 1) val = "verbose";
                else if (idx == 2) val = "notice";
                else if (idx == 3) val = "warning";
            }
            else 
            {
                if (idx == 0) val = "yes";
                else if (idx == 1) val = "no";
            }
        }
        if (val != it->second)
        {
            config.insert(std::make_pair(it->first, val));
        }
    }
    if (SetConfig(config) && GetConfig())
    {
        DoFillData();
    }
}
