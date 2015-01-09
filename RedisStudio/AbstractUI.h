#pragma once

#include "Environment.h"

class AbstraceUI : public CContainerUI, public CNotifyPump
{
public:
    AbstraceUI(CPaintManagerUI* pm, Environment* env):m_hWnd(NULL),m_pPaintManager(pm),m_pEnv(env){};

    virtual ~AbstraceUI(){};

    virtual void Initialize() {};

    virtual CDuiString GetVirtualwndName() = 0;

    virtual void RefreshWnd() = 0;

    virtual int GetIndex() = 0;

    virtual bool CanChange() 
    {
        return true;
    }

    virtual LRESULT HandleCustomMessage( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled )
    {
        bHandled = FALSE;
        return 0;
    }

    /// 设置窗口句柄
    inline void SetHWND(HWND hwnd)
    {
        m_hWnd = hwnd;
    }

    inline HWND GetHWND() 
    {
        return m_hWnd;
    }

    inline CPaintManagerUI* GetPaintMgr()
    {
        return m_pPaintManager;
    }

    inline Environment* Env()
    {
        return m_pEnv;
    }
private:
    CPaintManagerUI* m_pPaintManager;
    Environment* m_pEnv;
    HWND    m_hWnd;
};
