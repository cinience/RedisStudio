#include "stdafx.h"
#include "RedisDataUI.h"
#include "Base/String.h"
#include "UserMessage.h"
#include "Base/CharacterSet.h"
#include "Redis/RedisResult.h"
#include "Redis/RedisClient.h"
#include "rapidjson/reader.h"
#include "rapidjson/reader.h"
#include "rapidjson/prettywriter.h"    // for stringify JSON
#include "rapidjson/stringbuffer.h"

#include "DuiEx/UIMenu.h"

using namespace rapidjson;

static const TCHAR* kDefaultText  = _T("");
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
static const TCHAR* kPageTotalName= _T("redisdata_totalpage");
static const TCHAR* kPageLastName = _T("redisdata_pagelast");
static const TCHAR* kPageFinalName= _T("redisdata_pagefinal");
static const TCHAR* kCommitBtnName= _T("redisdata_commit");
static const TCHAR* kDataFomatName= _T("redisdata_format_combo");

static const TCHAR* kDBOperatorReloadMenuName = _T("menu_keys_reload");
static const TCHAR* kKeyOperatorDelMenuName   = _T("menu_key_del");



DUI_BEGIN_MESSAGE_MAP(RedisDataUI, CNotifyPump)
DUI_ON_MSGTYPE(DUI_MSGTYPE_CLICK,OnClick)
DUI_ON_MSGTYPE(DUI_MSGTYPE_ITEMCLICK,OnItemClick)
DUI_ON_MSGTYPE(DUI_MSGTYPE_ITEMRCLICK, OnMenuWakeup)
DUI_ON_MSGTYPE(DUI_MSGTYPE_ITEMACTIVATE,OnItemActive)
DUI_END_MESSAGE_MAP()


RedisDataUI::RedisDataUI( const CDuiString& strXML, CPaintManagerUI* pm ):AbstraceUI(pm),m_oEventListHeader(true),m_oEventKey(true)
{
    CDialogBuilder builder;
    CControlUI* pContainer = builder.Create(strXML.GetData(), NULL, NULL, GetPaintMgr(), NULL); 
    if( pContainer ) {
        this->Add(pContainer);
    }
    else 
    {
        this->RemoveAll();
        return;
    }
}

void RedisDataUI::Initialize()
{
    m_pList = dynamic_cast<CListUI*>(GetPaintMgr()->FindControl(kDataListName)); 
    m_pRichEdit = dynamic_cast<CRichEditUI*>(GetPaintMgr()->FindControl(kRichEditName));
    m_pKeyEdit = dynamic_cast<CEditUI*>(GetPaintMgr()->FindControl(kKeyEditName));
    m_PTypeEdit = dynamic_cast<CEditUI*>(GetPaintMgr()->FindControl(kDataTypeName));
    m_pDataSizeEdit = dynamic_cast<CEditUI*>(GetPaintMgr()->FindControl(kDataSizeName));
    m_pHorizontalLayout = dynamic_cast<CHorizontalLayoutUI*>(GetPaintMgr()->FindControl(kPageName));

    m_pPageCur = dynamic_cast<CEditUI*>(GetPaintMgr()->FindControl(kPageCurName));
    m_pPageTotal = dynamic_cast<CEditUI*>(GetPaintMgr()->FindControl(kPageTotalName));
    m_pPageFrist = dynamic_cast<CButtonUI*>(GetPaintMgr()->FindControl(kPageFristName));
    m_pPageLast = dynamic_cast<CButtonUI*>(GetPaintMgr()->FindControl(kPageLastName));
    m_pPageNext = dynamic_cast<CButtonUI*>(GetPaintMgr()->FindControl(kPageNextName));
    m_pPageFinal = dynamic_cast<CButtonUI*>(GetPaintMgr()->FindControl(kPageFinalName));
    m_pCommit = dynamic_cast<CButtonUI*>(GetPaintMgr()->FindControl(kCommitBtnName));
    m_pComboFormat = dynamic_cast<CComboUI*>(GetPaintMgr()->FindControl(kDataFomatName));
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
    CTreeNodeUI* pKeyNode = dynamic_cast<CTreeNodeUI*>(GetPaintMgr()->FindControl(kKeysTreeName));
    static CDuiString oldTitle = pKeyNode->GetItemText();
    DelChildNode(pKeyNode);
    int databases = RedisClient::GetInstance().DatabasesNum();
    m_UpdateDbs.clear();
    for (int i=0; i<databases; ++i)
    {
      m_UpdateDbs.insert(i);
      CDuiString theIndex;
      theIndex.Format(_T("%d"), i);
      CDuiString newTitle = oldTitle;
      newTitle.Replace(_T("$"), theIndex.GetData());
      CTreeNodeUI* pNode = new CTreeNodeUI();
      
      pNode->SetItemText(newTitle);
      //pNode->SetBkImage(pNodelist->GetBkImage());
      pNode->SetFixedHeight(20);
      pNode->SetContextMenuUsed(true);

      //pNode->SetIndex(myIdx--);
      /// 此处用tag先代替treelevel(还未实现)
      
      pNode->SetTag(i+1);
      pNode->SetAttribute(_T("itemattr"), _T("valign=\"vcenter\" font=\"5\" textpadding=\"5,3,0,0\""));
      pNode->SetAttribute(_T("folderattr"), _T("padding=\"0,3,0,0\" width=\"16\" height=\"16\" selectedimage=\"file='1_close.png' source='0,0,0,0'\" normalimage=\"file='1_open.png' source='0,0,0,0'\""));

      pKeyNode->AddChildNode(pNode);
    }

    DoRefreshKeysWork();
}


LRESULT RedisDataUI::HandleCustomMessage( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled )
{
    bHandled = TRUE;
    LRESULT lRes = TRUE; 
    switch (uMsg)
    {
    case WM_USER_DATAADD:
      lRes = OnDataAdd(GetHWND(), wParam, lParam);
      break;
    case WM_USER_TREEADD:
      lRes = OnKeyAdd(GetHWND(), wParam, lParam);
      break;
    case WM_USER_DATAVERBOSE:
      lRes = OnDataVerbose(GetHWND(), wParam, lParam);
      break;
    case WM_USER_MENU_KEYDEL:
      lRes = OnKeyDel(GetHWND(), wParam, lParam);
      break;
    default:
      bHandled = FALSE;
      lRes = FALSE;
      break;
    }
    return lRes;
}

void RedisDataUI::OnClick( TNotifyUI& msg )
{
    if (m_Thread.isRunning()) return;

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

void RedisDataUI::OnItemClick( TNotifyUI &msg )
{
    CDuiString name = msg.pSender->GetClass();
    CListTextElementUI listTextElementUI ;
    CTreeNodeUI        treeNodeUI;

    if (name == listTextElementUI.GetClass())
    {
        OnItemActiveForList(msg);
    }
    else if (name == treeNodeUI.GetClass())
    {
        OnItemActiveForTree(msg);
    }
}

void RedisDataUI::OnItemActive( TNotifyUI &msg )
{

}

void RedisDataUI::OnMenuWakeup( TNotifyUI &msg )
{    
    CTreeNodeUI* pTreeNodeUI = dynamic_cast<CTreeNodeUI*> (msg.pSender);
    if (!pTreeNodeUI) return;

    bool needMenu = false;
    STRINGorID res(_T(""));
    /// 父节点
    if (pTreeNodeUI->GetTag() > 0)
    {
        STRINGorID xml(_T("DBOperatorMenu.xml"));
        res = xml;
    }
    else if (!pTreeNodeUI->IsHasChild())
    {
        STRINGorID xml(_T("KeyOperatorMenu.xml"));
        res = xml;
    }
    CDuiString resStr = res.m_lpstr;
    m_pAssistNode = pTreeNodeUI;
    if (resStr.GetLength())
    {
        CMenuWnd* pMenu = new CMenuWnd(GetHWND());
        pMenu->OnClick += MakeDelegate(this, &RedisDataUI::OnMenuClick);
        CPoint point = msg.ptMouse;
        ClientToScreen(GetHWND(), &point);
        pMenu->Init(NULL, res, _T("xml"), point);   
    }
}

void RedisDataUI::OnItemActiveForTree( TNotifyUI &msg )
{
    CTreeNodeUI* pActiveNode = dynamic_cast<CTreeNodeUI*>(msg.pSender);
    if (!pActiveNode) return;
    /// 非叶子节点不处理 
    if (pActiveNode->IsHasChild()) return ;
    /// 首层db节点
    if (pActiveNode->GetTag() > 0) return;
    if (m_Thread.isRunning())
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
    if (dbNum < 0) return ;
    m_RedisData.db = dbNum;
    GetResult().Clear();    
    std::string key = Base::CharacterSet::UnicodeToANSI(pActiveNode->GetItemText().GetData());
    m_pKeyEdit->SetText(Base::CharacterSet::ANSIToUnicode(key).c_str());
    m_PTypeEdit->SetText(kDefaultText);
    m_pRichEdit->SetText(kDefaultText);
    m_pDataSizeEdit->SetText(kDefaultText);
    m_pPageCur->SetText(_T("1"));
    m_pPageTotal->SetText(_T("1"));
    m_pList->RemoveAll();
    m_pList->GetHeader()->RemoveAll();
    m_pHorizontalLayout->SetVisible(false);
    DoRefreshValuesWork();
}

void RedisDataUI::OnItemActiveForList(  TNotifyUI &msg  )
{
    CListTextElementUI* pListElement = dynamic_cast<CListTextElementUI*>(msg.pSender);
    ///LPCTSTR pstr;
    ///if ((pstr = pListElement->GetText(1)) == NULL)
    ///{
    ///   pstr = pListElement->GetText(0);
    ///}
    ///std::string text = Base::CharacterSet::UnicodeToANSI(pstr);
    
    std::size_t curPage = atoi(Base::CharacterSet::UnicodeToANSI(m_pPageCur->GetText().GetData()).c_str());
    
    int curCel = m_pList->GetItemIndex(pListElement);
    /// why ? 行数不对
    ///std::size_t curIndex = m_pList->GetCurSel();
    int realIndex = (curPage-1)*m_PageSize + curCel;
    int colIdx = m_pList->GetHeader()->GetCount() > 1 ? 1 : 0 ;
    string myValue = GetResult().Value(realIndex, colIdx);
    CDuiString myDuiStr = Base::CharacterSet::UTF8ToUnicode(myValue).c_str();
    /// 特殊的数据
    if (myValue.size() !=0 && myDuiStr.GetLength()==0)
    {
        TryHexFormat(myValue);
        myDuiStr  = Base::CharacterSet::UTF8ToUnicode(myValue).c_str();
    }
    std::string text = Base::CharacterSet::UnicodeToANSI(myDuiStr.GetData());
    SetRichEditText(text);
}

void RedisDataUI::OnPaginate(TNotifyUI& msg)
{
    std::size_t curPage = atoi(Base::CharacterSet::UnicodeToANSI(m_pPageCur->GetText().GetData()).c_str());

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
    CDuiString pageStr, totalpageStr ;
    pageStr.Format(_T("%u"), curPage);
    totalpageStr.Format(_T("%u"), GetMaxPage());
    m_pPageCur->SetText(pageStr.GetData());
    m_pPageTotal->SetText(totalpageStr.GetData());
    DoPaginateWork();
}

void RedisDataUI::OnCommit(TNotifyUI& msg)
{    
    if (m_pList->GetCurSel() < 0)
    {
        UserMessageBox(GetHWND(), 20002, NULL, MB_ICONINFORMATION);
        return;
    }
    CListTextElementUI* textElement = dynamic_cast<CListTextElementUI*>(m_pList->GetItemAt(m_pList->GetCurSel()));
    std::string curPageStr = Base::CharacterSet::UnicodeToANSI(m_pPageCur->GetText().GetData());
    int curPage = atoi(curPageStr.c_str());
    int idx = (curPage-1)*m_PageSize + m_pList->GetCurSel();
    std::string key = Base::CharacterSet::UnicodeToANSI(m_pKeyEdit->GetText().GetData()); 
    std::string oldValue = Base::CharacterSet::UnicodeToANSI(textElement->GetText(0));
    std::string field;
    if (m_pList->GetHeader()->GetCount() > 1)
    {        
        field = Base::CharacterSet::UnicodeToANSI(textElement->GetText(0));
        oldValue = Base::CharacterSet::UnicodeToANSI(textElement->GetText(1));
    }
    std::string newValue = Base::CharacterSet::UnicodeToANSI(m_pRichEdit->GetText().GetData()); 
    if (!RedisClient::GetInstance().UpdateData(key, oldValue, newValue, idx, field))
    {       
        /// todo 此处应该提示错误详情
        UserMessageBox(GetHWND(), 10031, RedisClient::GetInstance().GetLastError().GetData() , MB_ICONERROR);
        return;
    }

    int valueIdx = m_pList->GetHeader()->GetCount() > 1 ? 1 : 0 ;
    textElement->SetText(valueIdx, m_pRichEdit->GetText().GetData());
}

void RedisDataUI::DoPaginateWork()
{
    m_pWork.reset(new Base::RunnableAdapter<RedisDataUI>(*this, &RedisDataUI::SetPageValues));
    try
    {
        m_Thread.start(*m_pWork);
    }
    catch (std::exception& ex)
    {
        /// who care ?
        (void)(ex);
    }
}

void RedisDataUI::DoRefreshKeysWork()
{
    m_pWork.reset(new Base::RunnableAdapter<RedisDataUI>(*this, &RedisDataUI::BackgroundWorkForRefreshKeys));
    try
    {
        m_Thread.start(*m_pWork);
    }
    catch (std::exception& ex)
    {
        /// who care ?
        (void)(ex);
    }
}

void RedisDataUI::DoRefreshValuesWork()
{
    m_pWork.reset(new Base::RunnableAdapter<RedisDataUI>(*this, &RedisDataUI::BackgroundWorkForRefreshValues));
    try
    {
        m_Thread.start(*m_pWork);
    }
    catch (std::exception& ex)
    {
        /// who care ?
        (void)(ex);
    }
}

std::size_t RedisDataUI::GetMaxPage()
{
    std::size_t maxPage = GetResult().RowSize()%m_PageSize ? (GetResult().RowSize()/m_PageSize)+1 : GetResult().RowSize()/m_PageSize;
    return maxPage;
}

RedisResult& RedisDataUI::GetResult()
{
    return m_RedisData.result;
}

void RedisDataUI::BackgroundWorkForRefreshKeys(void)
{
    if (!RedisClient::GetInstance().IsConnected()) return;

    CTreeNodeUI* pParentNode = static_cast<CTreeNodeUI*>(GetPaintMgr()->FindControl(kKeysTreeName));
    for (int nodeIdx=0; nodeIdx<pParentNode->GetCountChild(); ++nodeIdx)
    {
        CTreeNodeUI *pKeyNode = (CTreeNodeUI*) pParentNode->GetChildNode(nodeIdx);
        
        if (m_UpdateDbs.find(nodeIdx) == m_UpdateDbs.end()) continue;
        DelChildNode(pKeyNode);
        if (!RedisClient::GetInstance().SelectDB(nodeIdx)) continue;
        
        RedisClient::TSeqArrayResults results;
        //results.sort();
        if (!RedisClient::GetInstance().keys(results)) return;
        RedisClient::TSeqArrayResults::const_iterator it    = results.begin();
        RedisClient::TSeqArrayResults::const_iterator itend = results.end();

        for (; it!=itend; ++it)
        {
            std::string theValue = *it;

            Base::String::TSeqStr seq;
            Base::String::Split(theValue, ":", seq);
            seq[seq.size()-1] = theValue;

            CTreeNodeUI* pPNode = pKeyNode;

            for (std::size_t idx=0; idx<seq.size()-1; ++idx)
            {
                CDuiString duiStr = Base::CharacterSet::ANSIToUnicode(seq[idx]).c_str();
                bool hasFind = false;
                for (int j=0; j<pPNode->GetCountChild(); ++j)
                {
                    CTreeNodeUI* pTempNode = pPNode->GetChildNode(j);
                    if (pTempNode->GetItemText() == duiStr && pTempNode->IsHasChild())
                    {
                        pPNode = pTempNode;
                        hasFind = true;
                        break;
                    }
                }
                if (hasFind)
                {
                    continue;
                }
                else 
                {
                    CTreeNodeUI* pnode = NewNode(seq[idx]);
                    TreeKeyContactData* pData = new TreeKeyContactData;
                    pData->pPNode = pPNode;
                    pData->pNode = pnode;
                    ::PostMessage(GetHWND(), WM_USER_TREEADD, (WPARAM)pData, NULL);
                    m_oEventKey.wait();
                    pPNode = pnode;
                }
            }
            TreeKeyContactData* pData = new TreeKeyContactData;
            pData->pPNode = pPNode;
            pData->pNode = NewNode(theValue, true);
            ::PostMessage(GetHWND(), WM_USER_TREEADD, (WPARAM)pData, NULL);
            m_oEventKey.wait();
        }
    }
}

void RedisDataUI::BackgroundWorkForRefreshValues(void)
{
    RedisClient::GetInstance().SelectDB(m_RedisData.db);
    std::string key, type;
    CEditUI*     pKeyEdit = static_cast<CEditUI*>(GetPaintMgr()->FindControl(kKeyEditName));
    key = Base::CharacterSet::UnicodeToANSI(pKeyEdit->GetText().GetData());
    if (!RedisClient::GetInstance().Exists(key))
    {
        /// 如果该键已经不存在，则提示用户重新加载
       ::PostMessage(GetHWND(), WM_USER_DATAVERBOSE, 10030, NULL);
       return;
    }
    m_RedisData.key = Base::CharacterSet::ANSIToUnicode(key).c_str();

    if (!RedisClient::GetInstance().GetData(key, type, GetResult())) return;
    m_RedisData.type = Base::CharacterSet::ANSIToUnicode(type).c_str();
    m_RedisData.size.Format(_T("%u"),GetResult().RowSize() );
    /// 预先显示数据类型等信息
    ::PostMessage(GetHWND(), WM_USER_DATAVERBOSE, NULL, NULL);

    for (std::size_t idx=0; idx<GetResult().ColumnSize(); ++idx)
    {
        CListHeaderItemUI* pHeaderItem = new CListHeaderItemUI();

        pHeaderItem->SetText(Base::CharacterSet::ANSIToUnicode(GetResult().ColumnName(idx)).c_str());
        pHeaderItem->SetHotImage(_T("file='list_header_hot.png'"));
        pHeaderItem->SetPushedImage(_T("file='list_header_pushed.png'"));
        pHeaderItem->SetSepImage(_T("file='list_header_sep.png'"));
        pHeaderItem->SetSepWidth(1);

        /// list在插入值时会根据listhead来作响应判断，所以需要在head完全update后再插入值
        LPARAM lparam = 0;
        if (idx+1 == GetResult().ColumnSize()) lparam = 1; 
        ::PostMessage(GetHWND(), WM_USER_DATAADD, (WPARAM)pHeaderItem, lparam);
    }
    m_oEventListHeader.wait();    
    SetPageValues();
}

void RedisDataUI::SetPageValues( )
{
    const std::size_t pagesize = m_PageSize;
    std::string pageStr = Base::CharacterSet::UnicodeToANSI(m_pPageCur->GetText().GetData());
    const std::size_t cur = atoi(pageStr.c_str());
    if (cur<=0 || cur>GetMaxPage())
    {
      return ;
    }

    std::size_t maxIndex = cur*pagesize>GetResult().RowSize() ? GetResult().RowSize(): cur*pagesize;
    int ipos = 0;
    for (std::size_t idx= (cur-1)*pagesize; idx<maxIndex; ++idx)
    {
        CListTextElementUI* pListTextElement = new CListTextElementUI;
        pListTextElement->SetOwner(m_pList);
        pListTextElement->SetTag(idx);
        for (std::size_t colidx=0; colidx<GetResult().ColumnSize(); ++colidx)
        {
            string myValue = GetResult().Value(idx, colidx);
            CDuiString myDuiStr = Base::CharacterSet::UTF8ToUnicode(myValue).c_str();
            /// 特殊的数据
            if (myValue.size() !=0 && myDuiStr.GetLength()==0)
            {
                TryHexFormat(myValue);
                myDuiStr  = Base::CharacterSet::UTF8ToUnicode(myValue).c_str();
            }

            /// 数据过长，不显示全部，否则会list拖动卡顿
            if (myDuiStr.GetLength() > 50) 
            {
                myDuiStr = myDuiStr.Left(50);
                myDuiStr.Append(_T("..."));
            }
            pListTextElement->SetText(colidx, myDuiStr.GetData());            
      }
      ::PostMessage(GetHWND(), WM_USER_DATAADD, (WPARAM)pListTextElement, NULL);
    }
}

LRESULT RedisDataUI::OnDataAdd( HWND hwnd, WPARAM wParam, LPARAM lParam )
{
    CControlUI* pListElement = reinterpret_cast<CControlUI*> (wParam);
    pListElement->NeedUpdate();
    m_pList->Add(pListElement);
    m_pList->NeedUpdate();
    if (lParam != NULL) m_oEventListHeader.set();
    return TRUE;
}

LRESULT RedisDataUI::OnKeyAdd( HWND hwnd, WPARAM wParam, LPARAM lParam )
{
    std::auto_ptr<TreeKeyContactData> pData(reinterpret_cast<TreeKeyContactData*>(wParam));
    CTreeNodeUI* pPNode = pData->pPNode;
    CTreeNodeUI* pNode  = pData->pNode;
    pPNode->AddChildNode(pNode);    
    /// 曲线救国，TreeView默认不展开节点
    pPNode->GetFolderButton()->Selected(true);
    pPNode->GetTreeView()->SetItemExpand(false, pPNode);
    pPNode->NeedUpdate();
    m_oEventKey.set();
    return TRUE;
}

LRESULT RedisDataUI::OnKeyDel( HWND hwnd, WPARAM wParam, LPARAM lParam )
{
    CTreeNodeUI* pNode = reinterpret_cast<CTreeNodeUI*>(wParam);
    CTreeNodeUI* pPNode;
    CDuiString a = pNode->GetItemText();
    std::string key = Base::CharacterSet::UnicodeToANSI(pNode->GetItemText().GetData());
    /// 暂时用同步方法删除，大数据时会有卡顿
    if (!RedisClient::GetInstance().DelKey(key)) return FALSE;
    while (pNode)
    {
        if (pNode->IsHasChild() || pNode->GetTag()>0) break;
        pPNode = pNode->GetParentNode();
        pPNode->Remove(pNode);
        pNode = pPNode;
    }
    return TRUE;
}

LRESULT RedisDataUI::OnDataVerbose( HWND hwnd, WPARAM wParam, LPARAM lParam )
{
    if (wParam != 0)
    {
        UserMessageBox(GetHWND(), 10030, NULL, MB_ICONERROR);
        return FALSE;
    }
    m_pKeyEdit->SetText(m_RedisData.key.GetData());
    m_PTypeEdit->SetText(m_RedisData.type.GetData());
    m_pDataSizeEdit->SetText(m_RedisData.size.GetData());
    m_pRichEdit->SetText(kDefaultText);    
    /// 如果是单元素(如string)，则一并直接更新到富文本框内 
    if (GetResult().RowSize() == 1 && GetResult().ColumnSize()==1)
    {
		string myValue = GetResult().Value(0, 0);
        CDuiString myDuiStr = Base::CharacterSet::UTF8ToUnicode(myValue).c_str();
        /// 特殊的数据
        if (myValue.size() !=0 && myDuiStr.GetLength()==0)
        {
            TryHexFormat(myValue);
            myDuiStr  = Base::CharacterSet::UTF8ToUnicode(myValue).c_str();
        }
        std::string text = Base::CharacterSet::UnicodeToANSI(myDuiStr.GetData());
        SetRichEditText(text);
    }

    if (GetResult().RowSize() > m_PageSize)
    {
        /// 设置总页数
        CDuiString totalpageStr;
        totalpageStr.Format(_T("%u"), GetMaxPage());
        m_pPageTotal->SetText(totalpageStr.GetData());
        m_pHorizontalLayout->SetVisible(true);
    }    
    return TRUE;
}

bool RedisDataUI::OnMenuClick( void* param )
{
	if (m_Thread.isRunning())
    {
         UserMessageBox(GetHWND(), 10012, NULL, MB_ICONINFORMATION);
         return false;
    }
    TEventUI* pTEventUI = static_cast<TEventUI*> (param);
    CDuiString name = pTEventUI->pSender->GetName();
    if (name == kDBOperatorReloadMenuName)
    {
        m_UpdateDbs.clear();
        m_UpdateDbs.insert(m_pAssistNode->GetTag() - 1);
        DoRefreshKeysWork();
    }
    else if (name == kKeyOperatorDelMenuName)
    {
        ::PostMessage(GetHWND(), WM_USER_MENU_KEYDEL, (WPARAM)(m_pAssistNode), NULL);
    }
    return true;
}

void RedisDataUI::SetRichEditText(const std::string& text)
{
    enum DataFomat
    {
      kAutoFomat = 0,
      kNormalFormat = 1,
      kJsonFormat = 2,
      kXMLFormat = 3,
      kHexFormat = 4
    };
    std::string styleText = text;
    int curSel = m_pComboFormat->GetCurSel();

    /// 没必要用面向对象的设计模式，直接if else搞定，简单直接
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
    else if (curSel == kHexFormat)
    {
        TryHexFormat(styleText);
    }
    m_pRichEdit->SetText(Base::CharacterSet::ANSIToUnicode(styleText).c_str());
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

bool RedisDataUI::TryHexFormat( std::string& text )
{
    std::string styleText;
    std::string::const_iterator it = text.begin();
    std::string::const_iterator itend = text.end();
    for ( ;it!=itend; ++it)
    {
        styleText+=Base::String::Chrtos(*it);
    }
    text = styleText;
    return true;
}

CTreeNodeUI* RedisDataUI::NewNode(const string& text, bool isLeaf)
{
    CTreeNodeUI* pNodeTmp = new CTreeNodeUI;
    pNodeTmp->SetItemTextColor(0XFF000000);
    pNodeTmp->SetItemHotTextColor(0XFF000000);
    pNodeTmp->SetSelItemTextColor(0XFF000000);
    pNodeTmp->SetItemText(Base::CharacterSet::ANSIToUnicode(text).c_str());  
    pNodeTmp->SetAttribute(_T("height"), _T("22"));
    pNodeTmp->SetAttribute(_T("inset"), _T("0,0,0,0"));
    if (isLeaf)
    {
        pNodeTmp->SetAttribute(_T("itemattr"), _T("valign=\"left\" font=\"5\" textpadding=\"5,3,0,0\""));
        pNodeTmp->SetAttribute(_T("folderattr"), _T("padding=\"21,3,0,0\" width=\"16\" height=\"16\" selectedimage=\"file='TreeStandard.png' source='112,32,128,48'\" normalimage=\"file='TreeStandard.png' source='112,32,128,48'\""));
    }
    else 
    {
        pNodeTmp->SetAttribute(_T("folderattr"), _T("padding=\"0,3,0,0\" width=\"16\" height=\"16\" selectedimage=\"file='tree_expand.png' source='0,0,16,16'\" normalimage=\"file='tree_expand.png' source='16,0,32,16'\""));
        pNodeTmp->SetAttribute(_T("itemattr"), _T("bkimage=\"file='TreeStandard.png' source='112,0,128,16' dest='5,3,21,19'\" valign=\"left\" font=\"5\" textpadding=\"25,3,0,0\""));
    }
    
    // if (isLeaf) pNodeTmp->SetAttribute(_T("folderattr"), _T("padding=\"0,3,0,0\" width=\"16\" height=\"16\" selectedimage=\"file='TreeStandard.png' source='112,32,128,48'\" normalimage=\"file='TreeStandard.png' source='112,32,128,48'\""));
    // else pNodeTmp->SetAttribute(_T("folderattr"), _T("padding=\"0,3,0,0\" width=\"16\" height=\"16\" selectedimage=\"file='TreeStandard.png' source='112,0,128,16'\" normalimage=\"file='TreeStandard.png' source='112,16,128,32'\""));
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
     //    pNodelist->Remove(pNodelist->GetChildNode(i));
     //}
     //cs = pNodelist->GetCountChild();
     /// 删除所有节点
     CStdPtrArray myArray = pNode->GetTreeNodes();
     for (int i=0; i<myArray.GetSize(); ++i)
     {
         pNode->RemoveAt((CTreeNodeUI*)myArray[i]);
     }
 }