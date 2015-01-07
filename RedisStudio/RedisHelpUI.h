#pragma once

#include "AbstractUI.h"

class RedisHelpUI : public AbstraceUI
{
public:
    RedisHelpUI(const CDuiString& strXML, CPaintManagerUI* pm, Environment* env);

    ~RedisHelpUI(void);

    void Initialize();

    int GetIndex();

    CDuiString GetVirtualwndName();

    void RefreshWnd();

    DUI_DECLARE_MESSAGE_MAP()

    void OnClick(TNotifyUI& msg);
};
