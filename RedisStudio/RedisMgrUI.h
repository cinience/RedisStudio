#pragma once

#include "AbstractUI.h"

class RedisMgrUI : public AbstraceUI
{
public:
    RedisMgrUI(const CDuiString& strXML, CPaintManagerUI* pm);

    ~RedisMgrUI(void);

    void Initialize();

    int GetIndex();

    CDuiString GetVirtualwndName();

    void RefreshWnd();

    DUI_DECLARE_MESSAGE_MAP()

    void OnClick(TNotifyUI& msg);
};
