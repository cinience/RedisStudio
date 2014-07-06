#pragma once

#include "AbstractUI.h"

#include "Redis/RedisClient.h"

class RedisConfigUI : public AbstraceUI
{
public:
    RedisConfigUI(const CDuiString& strXML, CPaintManagerUI* pm );

    ~RedisConfigUI(void);

    void Initialize();

    virtual CDuiString GetVirtualwndName();

    virtual void RefreshWnd();

    virtual int GetIndex();

    DUI_DECLARE_MESSAGE_MAP()

public:
    virtual void OnClick(TNotifyUI& msg);

    virtual void OnSelectChanged( TNotifyUI &msg );

    virtual void OnItemClick( TNotifyUI &msg );

    virtual void OnItemActive( TNotifyUI &msg );

private:
    bool GetConfig();

    bool SetConfig(const RedisClient::TDicConfig& config);

    void DoFillData();

    void DoApplyData();

private:
    RedisClient::TDicConfig m_dicConfig;
};
