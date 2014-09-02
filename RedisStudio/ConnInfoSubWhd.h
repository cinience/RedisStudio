#pragma once

#include <map>
#include <vector>

class ConnInfoSubWhd : public WindowImplBase
{
private:
    typedef std::vector<std::string> TSeqColumn;
    typedef std::map<int, TSeqColumn> TDicTables;

public:
    enum OperatorType
    {    
        Type_Add,
        Type_Alt
    };
public:
    ConnInfoSubWhd(TDicTables* pDic, bool &needApply, OperatorType type, int celIdx=0);

    ~ConnInfoSubWhd(void);

    void InitWindow();

    LPCTSTR GetWindowClassName() const;

    UINT GetClassStyle() const;;

    void OnFinalMessage(HWND hWnd);;

    virtual CDuiString GetSkinFolder();

    virtual CDuiString GetSkinFile();

    virtual UILIB_RESOURCETYPE GetResourceType() const;
    
    virtual LRESULT HandleCustomMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

    DUI_DECLARE_MESSAGE_MAP()

    virtual void OnClick(TNotifyUI& msg);

    bool OnAddInfo();

    bool OnAltInfo();

private:
	bool*                         m_needApply;
    TDicTables*                   m_pdicServerInfo;

    CEditUI*                      m_pEditName;
    CEditUI*                      m_pEditHost;
    CEditUI*                      m_pEditPort;
    CEditUI*                      m_pEditAuth;
    int                           m_celIdx;
    OperatorType                  m_type;
};
