#include "stdafx.h"
#include "RedisDataUI.h"
#include <sstream>
#include "String.h"
#include "UserMessage.h"
#include "CharacterSet.h"
#include "RedisResult.h"
#include "RedisClient.h"
#include "rapidjson/reader.h"
#include "rapidjson/prettywriter.h"	// for stringify JSON
#include "rapidjson/stringbuffer.h"
using namespace rapidjson;

static const TCHAR* kKeyEditName  = _T("redisdata_key");
static const TCHAR* kDataTypeName = _T("redisdata_type");
static const TCHAR* kDataSizeName = _T("redisdata_size");
static const TCHAR* kRichEditName = _T("redisdata_value_rich");
static const TCHAR* kDataListName = _T("redisdata_value");
static const TCHAR* kKeysTreeName = _T("redisdata_treenode");
static const TCHAR* kPageName     = _T("redisdata_page");
static const TCHAR* kPageFristName= _T("redisdata_pagefrist");
static const TCHAR* kPageNextName = _T("redisdata_pagenext");
static const TCHAR* kPageCurName  = _T("redisdata_curpage");
static const TCHAR* kPageLastName = _T("redisdata_pagelast");
static const TCHAR* kPageFinalName= _T("redisdata_pagefinal");
static const TCHAR* kCommitBtnName= _T("redisdata_commit");
static const TCHAR* kDataFomatName= _T("redisdata_format_combo");
static const TCHAR* kDefaultText  = _T("");


DUI_BEGIN_MESSAGE_MAP(RedisDataUI, CNotifyPump)
DUI_ON_MSGTYPE(DUI_MSGTYPE_CLICK,OnClick)
DUI_ON_MSGTYPE(DUI_MSGTYPE_SELECTCHANGED,OnSelectChanged)
DUI_ON_MSGTYPE(DUI_MSGTYPE_ITEMCLICK,OnItemClick)
DUI_ON_MSGTYPE(DUI_MSGTYPE_ITEMACTIVATE,OnItemActive)
DUI_END_MESSAGE_MAP()

bool RedisDataUI::m_bIsLoading = false;
RedisResult RedisDataUI::m_Resulsts;
const std::size_t RedisDataUI::m_PageSize = 100;

RedisDataUI::RedisDataUI( const CDuiString& strXML, CPaintManagerUI* pm ):AbstraceUI(pm)
{
	CDialogBuilder builder;
	// 这里必须传入m_PaintManager，不然子XML不能使用默认滚动条等信息。
	CControlUI* pContainer = builder.Create(strXML.GetData(), NULL, NULL, GetPaintMgr(), NULL); 
	if( pContainer ) {
		this->Add(pContainer);
	}
	else {
		this->RemoveAll();
		return;
	}
}

void RedisDataUI::Initialize()
{
	m_ConntactData.hwnd = GetHWND();
	m_ConntactData.paintManger = GetPaintMgr();
	m_pList = static_cast<CListUI*>(GetPaintMgr()->FindControl(kDataListName)); 
	//m_pList->SetDelayedDestroy(false);
	m_pRichEdit = static_cast<CRichEditUI*>(GetPaintMgr()->FindControl(kRichEditName));
	m_pKeyEdit = static_cast<CEditUI*>(GetPaintMgr()->FindControl(kKeyEditName));
	m_PTypeEdit = static_cast<CEditUI*>(GetPaintMgr()->FindControl(kDataTypeName));
	m_pDataSizeEdit = static_cast<CEditUI*>(GetPaintMgr()->FindControl(kDataSizeName));
	m_pHorizontalLayout = static_cast<CHorizontalLayoutUI*>(GetPaintMgr()->FindControl(kPageName));

	m_pPageCur = static_cast<CEditUI*>(GetPaintMgr()->FindControl(kPageCurName));
	m_pPageFrist = static_cast<CButtonUI*>(GetPaintMgr()->FindControl(kPageFristName));
	m_pPageLast = static_cast<CButtonUI*>(GetPaintMgr()->FindControl(kPageLastName));
	m_pPageNext = static_cast<CButtonUI*>(GetPaintMgr()->FindControl(kPageNextName));
	m_pPageFinal = static_cast<CButtonUI*>(GetPaintMgr()->FindControl(kPageFinalName));
	m_pCommit = static_cast<CButtonUI*>(GetPaintMgr()->FindControl(kCommitBtnName));
	m_pComboFormat = static_cast<CComboUI*>(GetPaintMgr()->FindControl(kDataFomatName));
}

int RedisDataUI::GetIndex()
{
	return 2;
}

CDuiString RedisDataUI::GetVirtualwndName()
{
	return _T("RedisData");
}

void RedisDataUI::RefreshWnd()
{
	CTreeNodeUI* pKeyNode = static_cast<CTreeNodeUI*>(GetPaintMgr()->FindControl(kKeysTreeName));

	static CDuiString oldTitle = pKeyNode->GetItemText();

	DelChildNode(pKeyNode);
	int myIdx = 15;
	int databases = RedisClient::GetInstance().DatabasesNum();
	for (int i=0; i<databases; ++i)
	{
		std::ostringstream os ;
		os << i;
		CDuiString theIndex = CharacterSet::ANSIToUnicode(os.str()).c_str();
		CDuiString newTitle = oldTitle;
		newTitle.Replace(_T("$"), theIndex.GetData());
		CTreeNodeUI* pNode = new CTreeNodeUI();
		
		pNode->SetItemText(newTitle);
		//pNode->SetBkImage(pNodelist->GetBkImage());
		pNode->SetFixedHeight(20);
		//pNode->SetIndex(myIdx--);
		/// 此处用tag先代替treelevel(还未实现)
		
		pNode->SetTag(i+1);
		pNode->SetAttribute(_T("itemattr"), _T("valign=\"vcenter\" font=\"5\" textpadding=\"5,3,0,0\""));
		pNode->SetAttribute(_T("folderattr"), _T("padding=\"0,3,0,0\" width=\"16\" height=\"16\" selectedimage=\"file='1_close.png' source='0,0,0,0'\" normalimage=\"file='1_open.png' source='0,0,0,0'\""));
		/// pNode->Expand(false);
		
		//pKeyNode->AddAt(pNode, i+1);		
		pKeyNode->AddChildNode(pNode);
	}
	
	DoRefreshKeysWork();
	/// pKeyNode->Expand(false);
}


LRESULT RedisDataUI::HandleCustomMessage( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled )
{
	bHandled = FALSE;
	LRESULT lRes = TRUE; 
	switch (uMsg)
	{
	case WM_USER_DATAADD:
		lRes = OnDataAdd(GetHWND(), wParam, lParam);
		break;
	case WM_USER_TREEADD:
		lRes = OnKeyAdd(GetHWND(), wParam, lParam);
	default:
		bHandled = FALSE;
		lRes = FALSE;
		break;
	}

	return lRes;
}

void RedisDataUI::OnClick( TNotifyUI& msg )
{
	if (m_bIsLoading ) return ;

	if (msg.pSender == m_pPageFinal ||
		msg.pSender == m_pPageLast  ||
		msg.pSender == m_pPageNext  ||
		msg.pSender == m_pPageFrist)
	{
		OnPaginate(msg);
	}
	else if (msg.pSender == m_pCommit)
	{
		OnCommit(msg);
	}

}

void RedisDataUI::OnSelectChanged( TNotifyUI &msg )
{

}

void RedisDataUI::OnItemClick( TNotifyUI &msg )
{
	CDuiString name = msg.pSender->GetClass();
	if (name == _T("ListTextElementUI"))
	{
		OnItemActiveForList(msg);
	}
	else 
	{
		OnItemActiveForTree(msg);
	}
}

void RedisDataUI::OnItemActive( TNotifyUI &msg )
{

}

void RedisDataUI::OnItemActiveForTree( TNotifyUI &msg )
{
	CTreeNodeUI* pActiveNode = static_cast<CTreeNodeUI*>(msg.pSender);
	if (!pActiveNode) return;
	/// 非叶子节点不处理 
	if (pActiveNode->IsHasChild()) return ;
    /// 首层db节点
	if (pActiveNode->GetTag() > 0) return;

	if (m_bIsLoading) 
	{
		UserMessageBox(GetHWND(), 10012, NULL, MB_ICONINFORMATION);
		return ;
	}
	
	int dbNum = -1;
	CTreeNodeUI* pDBNode = pActiveNode->GetParentNode();
	while (pDBNode)
	{
		if (pDBNode->GetTag()>0)
		{
			dbNum = pDBNode->GetTag()-1;
			break;
		}
		pDBNode = pDBNode->GetParentNode();
	}
	if (dbNum < 0)
	{
		return;
	}

	RedisClient::GetInstance().SelectDB(dbNum);
	std::string key = CharacterSet::UnicodeToANSI(pActiveNode->GetItemText().GetData());
	m_pKeyEdit->SetText(CharacterSet::ANSIToUnicode(key).c_str());
	m_PTypeEdit->SetText(kDefaultText);
	m_pRichEdit->SetText(kDefaultText);
	m_pDataSizeEdit->SetText(kDefaultText);
	m_pPageCur->SetText(_T("1"));
	m_pList->RemoveAll();
	m_pList->GetHeader()->RemoveAll();
	m_pHorizontalLayout->SetVisible(false);
	DoRefreshValuesWork();
}

void RedisDataUI::OnItemActiveForList( TNotifyUI &msg )
{
	CListTextElementUI* pListElement = (CListTextElementUI*)msg.pSender;
	LPCTSTR pstr;
	if ((pstr = pListElement->GetText(1)) == NULL)
	{
		pstr = pListElement->GetText(0);
	}
	std::string text = CharacterSet::UnicodeToANSI(pstr);
	SetRichEditText(text);
}

void RedisDataUI::OnPaginate(TNotifyUI& msg)
{
	std::size_t curPage = atoi(CharacterSet::UnicodeToANSI(m_pPageCur->GetText().GetData()).c_str());

	if (msg.pSender == m_pPageNext)
	{
		curPage += 1;
	}
	else if (msg.pSender == m_pPageLast)
	{
		curPage -= 1;
	}
	else if (msg.pSender == m_pPageFrist)
	{
		curPage = 1;
	}
	else if (msg.pSender == m_pPageFinal)
	{
		curPage = GetMaxPage();
	}
	else 
	{
		return;
	}

	if (curPage==0 || curPage>GetMaxPage())
	{
		return;
	}
	m_pList->RemoveAll();
	if (SetPageValues(m_pList, GetHWND(),curPage, m_Resulsts))
	{
		std::ostringstream ss;
		ss<<curPage;
		m_pPageCur->SetText(CharacterSet::ANSIToUnicode(ss.str()).c_str());
	}
}

void RedisDataUI::OnCommit(TNotifyUI& msg)
{
	UserMessageBox(GetHWND(), 20001, NULL, MB_ICONINFORMATION);
}

void RedisDataUI::DoRefreshKeysWork()
{
	DWORD dwThreadID = 0;
	HANDLE hThread = CreateThread(NULL, 0, &RedisDataUI::BackgroundWorkForRefreshKeys, &m_ConntactData,  0, &dwThreadID);
}

void RedisDataUI::DoRefreshValuesWork()
{
	DWORD dwThreadID = 0;
	HANDLE hThread = CreateThread(NULL, 0, &RedisDataUI::BackgroundWorkForRefreshValues, &m_ConntactData,  0, &dwThreadID);
}

void RedisDataUI::RefreshKeys(ContactData* pContactData)
{
	if (!RedisClient::GetInstance().IsConnected()) return;

	CTreeNodeUI* pParentNode = static_cast<CTreeNodeUI*>(pContactData->paintManger->FindControl(kKeysTreeName));
	for (int nodeIdx=0; nodeIdx<pParentNode->GetCountChild(); ++nodeIdx)
	{
		CTreeNodeUI *pKeyNode = (CTreeNodeUI*) pParentNode->GetChildNode(nodeIdx);

		int k = pKeyNode->GetTag();
		RedisClient::GetInstance().SelectDB(nodeIdx);
		DelChildNode(pKeyNode);

		RedisClient::TSeqArrayResults results;
		if (!RedisClient::GetInstance().keys(results)) return;
		RedisClient::TSeqArrayResults::const_iterator it = results.begin();
		RedisClient::TSeqArrayResults::const_iterator itend = results.end();

		for (; it!=itend; ++it)
		{
			std::string theValue = *it;

			String::TSeqStr seq;
			String::Split(theValue, ":", seq);
			seq[seq.size()-1] = theValue;

			CTreeNodeUI* pPNode = pKeyNode;

			for (std::size_t idx=0; idx<seq.size()-1; ++idx)
			{
				CDuiString duiStr = CharacterSet::ANSIToUnicode(seq[idx]).c_str();
				bool hasFind = false;
				for (int j=0; j<pPNode->GetCountChild(); ++j)
				{
					CTreeNodeUI* pTempNode = pPNode->GetChildNode(j);
					if (pTempNode->GetItemText() == duiStr && pTempNode->IsHasChild())
					{
						pPNode = pTempNode;
						hasFind = true;
					}
				}
				if (hasFind)
				{
					continue;
				}
				else 
				{
					CTreeNodeUI* pnode = NewNode(seq[idx]);
					//pPNode->AddAt(pnode, pPNode->GetCountChild());
					TreeKeyContactData* data = new TreeKeyContactData;
					data->pPNode = pPNode;
					data->pNode = pnode;
					pPNode->AddAt(pnode, pPNode->GetCountChild());
					LPARAM lPm = pPNode->GetCountChild();
					//::PostMessage(pContactData->hwnd, WM_USER_TREEADD, (WPARAM)data, lPm);
					pPNode = pnode;
				}
			}
			TreeKeyContactData* data = new TreeKeyContactData;
			data->pPNode = pPNode;
			//data->pNode = NewNode(theValue);
			LPARAM lPm = pPNode->GetCountChild();
			//::PostMessage(pContactData->hwnd, WM_USER_TREEADD, (WPARAM)data, lPm);
			pPNode->AddAt(NewNode(theValue, true), pPNode->GetCountChild());
		}
	}

	/// 临时解决默认全部展开的问题
	CTreeViewUI* pView = pParentNode->GetTreeView();

	for (int idx=0; idx<pView->GetCount(); ++idx)
	{
		CTreeNodeUI* pTmpNode = static_cast<CTreeNodeUI*> (pView->GetItemAt(idx));
		if (pTmpNode == pParentNode) continue;
		pTmpNode->GetFolderButton()->Selected(true);
	}
	for (int nodeIdx=0; nodeIdx<pParentNode->GetCountChild(); ++nodeIdx)
	{
		CTreeNodeUI *pKeyNode = (CTreeNodeUI*) pParentNode->GetChildNode(nodeIdx);
		pView->SetItemExpand(false, pKeyNode);
	}
}

void RedisDataUI::RefreshValues(ContactData* pContactData)
{
	CPaintManagerUI* pm = pContactData->paintManger;
	m_bIsLoading = true;
	m_Resulsts.Clear();
	bool retVal;
	CEditUI* pKeyEdit = static_cast<CEditUI*>(pm->FindControl(kKeyEditName));
	CEditUI* pDataTypeEdit = static_cast<CEditUI*>(pm->FindControl(kDataTypeName));
	CEditUI* pDataSizeEdit = static_cast<CEditUI*>(pm->FindControl(kDataSizeName));
	CRichEditUI* pRichEdit = static_cast<CRichEditUI*>(pm->FindControl(kRichEditName));
	CListUI* pList = static_cast<CListUI*>(pm->FindControl(kDataListName));
	CHorizontalLayoutUI* pHorizontalLayout = static_cast<CHorizontalLayoutUI*>(pm->FindControl(kPageName));
	if (!(pKeyEdit && pDataTypeEdit)) return ;
	
	string key = CharacterSet::UnicodeToANSI(pKeyEdit->GetText().GetData());
    string type;
	retVal = RedisClient::GetInstance().Type(key, type);
	if (!retVal) return;
	pDataTypeEdit->SetText(CharacterSet::ANSIToUnicode(type).c_str());

	retVal = RedisClient::GetInstance().GetData(key, type, m_Resulsts);
	if (!retVal) return;
	pDataTypeEdit->SetText(CharacterSet::ANSIToUnicode(type).c_str());
	
	std::ostringstream ss;
	ss<<m_Resulsts.RowSize();
	pDataSizeEdit->SetText(CharacterSet::ANSIToUnicode(ss.str()).c_str());
	//pDataTypeEdit->NeedUpdate();

	for (std::size_t idx=0; idx<m_Resulsts.ColumnSize(); ++idx)
	{
		CListHeaderItemUI* pHeaderItem = new CListHeaderItemUI();
		
		pHeaderItem->SetText(CharacterSet::ANSIToUnicode(m_Resulsts.ColumnName(idx)).c_str());
		pHeaderItem->SetHotImage(_T("file='list_header_hot.png'"));
		pHeaderItem->SetPushedImage(_T("file='list_header_pushed.png'"));
		pHeaderItem->SetSepImage(_T("file='list_header_sep.png'"));
		pHeaderItem->SetSepWidth(1);
		pList->Add(pHeaderItem);
		// 怪事何其多，用消息传递方式导致有时增加一列会失败（为空）
		//::PostMessage(pContactData->hwnd, WM_USER_DATAADD, (WPARAM)pHeaderItem, NULL);
	}
	
	SetPageValues(pList, pContactData->hwnd, 1, m_Resulsts);

	if (m_Resulsts.RowSize() > m_PageSize)
	{
		pHorizontalLayout->SetVisible(true);
	}
}


std::size_t RedisDataUI::GetMaxPage()
{
	std::size_t maxPage = m_Resulsts.RowSize()%m_PageSize ? (m_Resulsts.RowSize()/m_PageSize)+1 : m_Resulsts.RowSize()/m_PageSize;
	return maxPage;
}


CTreeNodeUI* RedisDataUI::NewNode(const string& text, bool isLeaf)
{
	CTreeNodeUI* pNodeTmp = new CTreeNodeUI;
	pNodeTmp->SetItemTextColor(0XFF000000);
	pNodeTmp->SetItemHotTextColor(0XFF000000);
	pNodeTmp->SetSelItemTextColor(0XFF000000);
	pNodeTmp->SetItemText(CharacterSet::ANSIToUnicode(text).c_str());  
	pNodeTmp->SetAttribute(_T("height"), _T("22"));
	pNodeTmp->SetAttribute(_T("inset"), _T("0,0,0,0"));
	pNodeTmp->SetAttribute(_T("itemattr"), _T("valign=\"vcenter\" font=\"5\" textpadding=\"5,3,0,0\""));
	if (isLeaf) pNodeTmp->SetAttribute(_T("folderattr"), _T("padding=\"0,3,0,0\" width=\"16\" height=\"16\" selectedimage=\"file='TreeStandard.png' source='112,32,128,48'\" normalimage=\"file='TreeStandard.png' source='112,32,128,48'\""));
	else pNodeTmp->SetAttribute(_T("folderattr"), _T("padding=\"0,3,0,0\" width=\"16\" height=\"16\" selectedimage=\"file='TreeStandard.png' source='112,0,128,16'\" normalimage=\"file='TreeStandard.png' source='112,16,128,32'\""));
	return pNodeTmp;
}



void RedisDataUI::DelChildNode( CTreeNodeUI* pNode )
{
	/// nm,用这种方式都删不掉，叼代码
	//pNodelist->SetDelayedDestroy(false);
	//pNodelist->RemoveAll();
	//int cs = pNodelist->GetCountChild();
	//for (int i=0; i<cs; ++i)
	//{
	//	pNodelist->Remove(pNodelist->GetChildNode(i));
	//}
	//cs = pNodelist->GetCountChild();
	/// 删除所有节点
	CStdPtrArray myArray = pNode->GetTreeNodes();
	for (int i=0; i<myArray.GetSize(); ++i)
	{
		pNode->RemoveAt((CTreeNodeUI*)myArray[i]);
	}
}

DWORD WINAPI RedisDataUI::BackgroundWorkForRefreshKeys(LPVOID lpParameter)
{
	ContactData* contactData = (ContactData*) lpParameter;
	RefreshKeys(contactData);
	return 0;
}

DWORD WINAPI RedisDataUI::BackgroundWorkForRefreshValues(LPVOID lpParameter)
{
	ContactData* contactData = (ContactData*) lpParameter;
	
	RefreshValues(contactData);
	m_bIsLoading = false;
	return 0;
}

bool RedisDataUI::SetPageValues( CListUI* pList, HWND hwnd, std::size_t cur, RedisResult& results )
{	
	const std::size_t pagesize = m_PageSize;
	if (cur<=0 || cur>GetMaxPage())
	{
		return false;
	}
	
	std::size_t maxIndex = cur*pagesize>results.RowSize() ? results.RowSize(): cur*pagesize;
    int ipos = 0;
	for (std::size_t idx= (cur-1)*pagesize; idx<maxIndex; ++idx)
	{
		CListTextElementUI* pListTextElement = new CListTextElementUI;
		pListTextElement->SetOwner(pList);
		pListTextElement->SetTag(idx);
		for (std::size_t colidx=0; colidx<results.ColumnSize(); ++colidx)
		{
			string myValue = results.Value(idx, colidx);
			pListTextElement->SetText(colidx, CharacterSet::UTF8ToUnicode(myValue).c_str());			
		}
		::PostMessage(hwnd, WM_USER_DATAADD, (WPARAM)pListTextElement, NULL);
	}
	return true;
}

LRESULT RedisDataUI::OnDataAdd( HWND hwnd, WPARAM wParam, LPARAM lParam )
{
	CControlUI* pListElement = (CControlUI*) (wParam);
	m_pList->Add(pListElement);
	m_pList->NeedUpdate();
	return TRUE;
}

LRESULT RedisDataUI::OnKeyAdd( HWND hwnd, WPARAM wParam, LPARAM lParam )
{
	long idx = lParam;
	TreeKeyContactData* data = (TreeKeyContactData*)wParam;
	CTreeNodeUI* pPNode = data->pPNode;
	CTreeNodeUI* pnode = data->pNode;
	pPNode->AddAt(pnode, idx);
	m_pList->NeedUpdate();
	delete data;
	return TRUE;
}

void RedisDataUI::SetRichEditText(const std::string& text)
{
	enum DataFomat
	{
		kAutoFomat = 0,
		kNormalFormat = 1,
		kJsonFormat = 2,
		kXMLFormat = 3
	};
	std::string styleText = text;
	int curSel = m_pComboFormat->GetCurSel();

	if ( curSel == kAutoFomat )
	{
		if (TryJsonFomat(styleText) || TryXMLFormat(styleText)) {} 
	}
	else if (curSel == kJsonFormat)
	{
		TryJsonFomat(styleText);
	}
	else if (curSel == kXMLFormat)
	{
		TryXMLFormat(styleText);
	}
	m_pRichEdit->SetText(CharacterSet::ANSIToUnicode(styleText).c_str());
}

bool RedisDataUI::TryJsonFomat( std::string& text )
{
	std::string& styleText = text;

	StringStream s(styleText.c_str());
	StringBuffer buffer;
	PrettyWriter<StringBuffer> writer(buffer);
	Reader reader;
	if (reader.Parse<0>(s, writer))
	{
		styleText = buffer.GetString();
		return true;
	}
	return false;
}

bool RedisDataUI::TryXMLFormat( std::string& text )
{
	return true;
}
