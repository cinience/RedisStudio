#pragma once

class MessageBoxWhd : public WindowImplBase
{
public:
	enum MsgType
	{
		Info,
		Warn,
		Error
	};

public:
	MessageBoxWhd(MsgType type, UINT code, CDuiString msg);

	~MessageBoxWhd(void);

	void InitWindow();

	LPCTSTR GetWindowClassName() const;

	UINT GetClassStyle() const;

	void OnFinalMessage(HWND hWnd);;

	virtual CDuiString GetSkinFolder();

	virtual CDuiString GetSkinFile();

	virtual UILIB_RESOURCETYPE GetResourceType() const;

	DUI_DECLARE_MESSAGE_MAP()

	virtual void OnClick(TNotifyUI& msg);

private:
	MsgType m_uType;
	CDuiString m_dstrExMsg;
	UINT m_uMsgCode;
};
