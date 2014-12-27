#pragma once

class AbstraceUI : public CContainerUI, public CNotifyPump
{
public:
    AbstraceUI(CPaintManagerUI* pm):m_hWnd(NULL),m_pPaintManager(pm){};

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
private:
    CPaintManagerUI* m_pPaintManager;
    HWND    m_hWnd;
};