// XmlEditDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "XmlEdit.h"
#include "XmlEditDlg.h"

#include "Xml/XmlNode.h"
#include "Xml/XmlStream.h"
#include "FileBridge.h"
#include "./xmleditdlg.h"

#include <string>
#include <stack>
#include ".\xmleditdlg.h"
#include "StandardInputDlg.h"
#include "AttributeCreateDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

typedef std::stack<int> STACK_NODE;

std::string cvtIntToString( int val )
{
	char buf[128] = {0};
	itoa(val,buf,10);

	std::string strRet;
	return strRet = buf;
}

void GetTreeItemPath(CTreeCtrl& CtrlTree,HTREEITEM hItem, STACK_NODE& SN)
{
	if( !hItem )return;

	int nIndex = 
		static_cast<int>(CtrlTree.GetItemData(hItem));

	SN.push(nIndex);

	HTREEITEM hParent = CtrlTree.GetParentItem(hItem);
	GetTreeItemPath(CtrlTree,hParent,SN);
}

// 根据当前选中的Item取得XmlNode
xnd::XmlNode* GetXmlNodeFromTreeItem(CTreeCtrl& CtrlTree,HTREEITEM TI,xnd::XmlNode* pNode )
{
	STACK_NODE snNode;

	// 取得结点路径栈
	GetTreeItemPath(CtrlTree,TI,snNode);
	snNode.pop();

	xnd::XmlNode* pRet = pNode;
	while( !snNode.empty() )
	{
		int nTemp = snNode.top();
		pRet = pRet->getChildNodeByIndex( nTemp );
		snNode.pop();
	}

	return pRet;
}

// 刷新树型控件
void RefreshTreeCtrl( CTreeCtrl& CtlTree,HTREEITEM TI, int nIndex,const xnd::XmlNode& curNode )
{
	std::string strName = curNode.getName();
	
	if( strName.empty() )
		strName = "(null)";

	HTREEITEM retTI = CtlTree.InsertItem(strName.c_str(),TI);
	CtlTree.SetItemData(retTI,static_cast<DWORD_PTR>(nIndex));

	int nCount = curNode.getChildNodeCount();
	for( int i=0;i<nCount;i++)
	{
		xnd::XmlNode * pNode = curNode.getChildNodeByIndex(i);
		RefreshTreeCtrl(CtlTree,retTI,i,*pNode);
	}
}

// 刷新列表控件
void RefreshListCtrl( CListCtrl& CtlList,xnd::XmlNode* pNode )
{
	CtlList.DeleteAllItems();

	if( !pNode )
		return;

	int nIndex = 0;
	xnd::XmlAttribute* pAttr = pNode->getFirstAttribute();
	while( pAttr )
	{
		std::string strName  = pAttr->getName();
		std::string strType  = xnd::XmlStream::cvtTypeToString(pAttr->getType());
		std::string strLen   = cvtIntToString( pAttr->getLength() );
		std::string strValue = pAttr->getValueAsString();

		if( xnd::XML_BINARY == pAttr->getType() )
		{
			int   nLen = pAttr->getLength();
			char* pBuf = new char[nLen];

			pAttr->getValueAsBinary(pBuf,nLen);

			strValue = xnd::XmlStream::cvtBinaryToHexString(pBuf,nLen);

			delete[] pBuf;
		}

		CtlList.InsertItem(nIndex,strName.c_str());
		CtlList.SetItemText(nIndex,1,strType.c_str());
		CtlList.SetItemText(nIndex,2,strLen.c_str() );
		CtlList.SetItemText(nIndex,3,strValue.c_str());

		++nIndex;

		pAttr = pNode->getNextAttribute(pAttr);
	}
}

// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


// CXmlEditDlg 对话框



CXmlEditDlg::CXmlEditDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CXmlEditDlg::IDD, pParent)
	, m_pActiveNode(NULL)
	, m_pCopyNode(NULL)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CXmlEditDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_ATTRIBUTES, m_listAttributes);
	DDX_Control(pDX, IDC_TREE_NODES, m_treeNodes);
}

BEGIN_MESSAGE_MAP(CXmlEditDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_COMMAND(ID_HELP_ABOUT, OnHelpAbout)
	ON_COMMAND(ID_FILE_EXIT, OnFileExit)
	ON_COMMAND(ID_FILE_OPEN32772, OnFileOpen32772)
	ON_COMMAND(ID_FILE_SAVE32773, OnFileSave32773)
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE_NODES, OnTvnSelchangedTreeNodes)
	ON_NOTIFY(TVN_SELCHANGING, IDC_TREE_NODES, OnTvnSelchangingTreeNodes)
	ON_NOTIFY(NM_RCLICK, IDC_TREE_NODES, OnNMRclickTreeNodes)
	ON_NOTIFY(NM_RCLICK, IDC_LIST_ATTRIBUTES, OnNMRclickListAttributes)
	ON_WM_CLOSE()
	ON_COMMAND(ID_F_N, OnFileNew)
	ON_COMMAND(ID_TREEPOPUMENU_A, OnTreepopumenuAddNode)
	ON_COMMAND(ID_TREEPOPUMENU_R, OnTreepopumenuR)
	ON_COMMAND(ID_LISTPOPUMENU_E, OnListpopumenuAttributeEdit)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST_ATTRIBUTES, OnNMDblclkListAttributes)
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
	ON_COMMAND(ID_LISTPOPUMENU_D, OnListpopumenuDeleteAttribute)
	ON_COMMAND(ID_TREEPOPUMENU_D, OnTreepopumenuDeleteNode)
	ON_COMMAND(ID_A_N, OnNewAttribute)
	ON_WM_SIZING()
	ON_WM_SIZE()
	ON_COMMAND(ID_N_C, OnCopyNode)
	ON_COMMAND(ID_N_V, OnPastNode)
	ON_COMMAND(ID_E_R, OnReleaseClipbord)
	ON_COMMAND(ID_E_E, OnExportNode)
	ON_COMMAND(ID_E_I, OnImportExport)
END_MESSAGE_MAP()


// CXmlEditDlg 消息处理程序

BOOL CXmlEditDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码

	// 初始化树型控件
	m_treeNodes.SetBkColor(RGB(210,230,190));

	// 创建Image
	m_ilImageList.Create(16,16,1,0,2);
	
	m_ilImageList.Add( AfxGetApp()->LoadIcon( IDI_UNSELECT ) );
	m_ilImageList.Add( AfxGetApp()->LoadIcon( IDI_SELECT )   );
	

	// 初始化列表控件
	m_listAttributes.ModifyStyle( 0, LVS_REPORT );
	m_listAttributes.ModifyStyle( LVS_EDITLABELS, 0 );

	m_listAttributes.SetExtendedStyle( LVS_EX_FULLROWSELECT|LVS_EX_ONECLICKACTIVATE|LVS_EX_INFOTIP|
									   LVS_EX_SUBITEMIMAGES|LVS_EX_GRIDLINES );

	m_listAttributes.InsertColumn( 0, "名称", LVCFMT_LEFT,  100 );
	m_listAttributes.InsertColumn( 1, "类型", LVCFMT_LEFT,  100 );
	m_listAttributes.InsertColumn( 2, "大小", LVCFMT_RIGHT, 100 );
	m_listAttributes.InsertColumn( 3, "数据", LVCFMT_LEFT,  200 );


	// 刷新列表与树型控件
	RefreshTree_Nodes();
	RefreshList_Attributes();

	PostMessage(WM_SYSCOMMAND,SC_MAXIMIZE,0);
	
	return TRUE;  // 除非设置了控件的焦点，否则返回 TRUE
}

void CXmlEditDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CXmlEditDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		// 取得当前窗口大小
		RECT rt;
		GetClientRect(&rt);

		// 取得树型控件数据
		RECT rtTree;
		m_treeNodes.GetClientRect(&rtTree);

		rtTree.right = 183;
		rtTree.bottom = rt.bottom - 1;
		m_treeNodes.MoveWindow(&rtTree,TRUE);

		// 取得列表控件数据
		RECT rtList;
		m_listAttributes.GetClientRect(&rtList);

		rtList.left = rtTree.right + 1;
		rtList.top  = rtTree.top;

		rtList.right  = rt.right-1;
		rtList.bottom = rt.bottom - 1;

		m_listAttributes.MoveWindow(&rtList,TRUE);

		// 设置list的列表头
		int w = rtList.right - rtList.left;

		w -= 60;
		w -= 80;
		
		m_listAttributes.SetColumnWidth( 0, static_cast<int>(.4f*w    ) );
		m_listAttributes.SetColumnWidth( 1, static_cast<int>( 80      ) );
		m_listAttributes.SetColumnWidth( 2, static_cast<int>( 60      ) );
		m_listAttributes.SetColumnWidth( 3, static_cast<int>(.6f*w-25 ) );
		

		CDialog::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标显示。
HCURSOR CXmlEditDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CXmlEditDlg::OnHelpAbout()
{
	// TODO: 在此添加命令处理程序代码
	CAboutDlg dlgAbout;
	dlgAbout.DoModal();
}

void CXmlEditDlg::OnFileExit()
{
	// TODO: 在此添加命令处理程序代码
	if( IDYES != MessageBox("将要退出程序，确定要退出吗？","退出",MB_YESNOCANCEL|MB_ICONQUESTION) )
		return;

	CDialog::OnOK();
}

// 文件打开
void CXmlEditDlg::OnFileOpen32772()
{
	// TODO: 在此添加命令处理程序代码
	char strFilter[] = "TextXml Files (*.txml)|*.txml|BinaryXml Files (*.bxml)|*.bxml||";
	CFileDialog fd( TRUE,NULL,NULL,OFN_FILEMUSTEXIST| OFN_HIDEREADONLY,strFilter,this);

	if( IDOK == fd.DoModal() )
	{
		int nIndex = fd.GetOFN().nFilterIndex;
		if( nIndex == 1 )
		{
			if( !xnd::XmlStream::loadTextXml(fd.GetFileName().GetBuffer(0),&m_xnData) )
			{
				CString strMsg;
				strMsg.Format("打开文本xml文件: %s 失败！.",fd.GetFileName().GetBuffer(0));
				MessageBox(strMsg,"打开",MB_OK);
			}
		}
		else
		{
			if( !xnd::XmlStream::loadBinaryXml(fd.GetFileName().GetBuffer(0),&m_xnData ) )
			{
				CString strMsg;
				strMsg.Format("打开二进制xml文件: %s 失败！.",fd.GetFileName().GetBuffer(0));
				MessageBox(strMsg,"打开",MB_OK);
			}
		}

		m_pActiveNode = &m_xnData;
		
		RefreshTree_Nodes();
		RefreshList_Attributes();
	}
}

// 文件保存
void CXmlEditDlg::OnFileSave32773()
{
	// TODO: 在此添加命令处理程序代码
	char strFilter[] = "TextXml Files (*.txml)|*.txml|BinaryXml Files (*.bxml)|*.bxml||";
	CFileDialog fd( FALSE,NULL,NULL,OFN_FILEMUSTEXIST| OFN_HIDEREADONLY,strFilter,this);

	if( IDOK == fd.DoModal() )
	{
		CString strFile   = fd.GetFileName();

		int nIndex = fd.GetOFN().nFilterIndex;

		if(  nIndex == 1 )
		{
			if( -1 == strFile.Find(".txml") )
				strFile.Append(".txml");

			if( !xnd::XmlStream::saveTextXml( strFile.GetBuffer(0), &m_xnData ))
				MessageBox("保存为文本xml失败！","保存",MB_OK);
			else
				MessageBox("保存为文本xml成功！","保存",MB_OK);
		}
		else if( nIndex == 2)
		{
			if( -1 == strFile.Find(".bxml") )
				strFile.Append(".bxml");

			if( !xnd::XmlStream::saveBinaryXml( strFile.GetBuffer(0), &m_xnData ))
				MessageBox("保存为二进制xml失败！","保存",MB_OK);
			else
				MessageBox("保存为二进制xml成功！","保存",MB_OK);
		}
	}
}

// 结点树选择项改变事件处理
void CXmlEditDlg::OnTvnSelchangedTreeNodes(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	m_treeNodes.SetItemImage( pNMTreeView->itemNew.hItem, 0, 1 );
	m_treeNodes.SetItemImage( pNMTreeView->itemOld.hItem, 0, 0 );

	// 取得当前结点
	m_pActiveNode = 
		GetXmlNodeFromTreeItem(m_treeNodes,pNMTreeView->itemNew.hItem,&m_xnData);
  
	RefreshList_Attributes();

	*pResult = 0;
}

// 结点树选择项正改变事件处理
void CXmlEditDlg::OnTvnSelchangingTreeNodes(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	*pResult = 0;
}

// 刷新结点树
void CXmlEditDlg::RefreshTree_Nodes(void)
{
	VERIFY(m_treeNodes.DeleteAllItems());
	m_treeNodes.SetImageList(&m_ilImageList,LVSIL_NORMAL);

	RefreshTreeCtrl(m_treeNodes,TVI_ROOT,0,m_xnData);
}

// 刷新属性列表
void CXmlEditDlg::RefreshList_Attributes(void)
{
	RefreshListCtrl(m_listAttributes,m_pActiveNode);
}

// 结点树鼠标右击事件
void CXmlEditDlg::OnNMRclickTreeNodes(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: 在此添加控件通知处理程序代码

	// 弹出菜单
	CMenu mu;
	mu.LoadMenu(IDR_MENU_TREE);

	CMenu* pMenu = mu.GetSubMenu(0);

	POINT pt;
	GetCursorPos(&pt);
	pMenu->TrackPopupMenu(TPM_LEFTALIGN |TPM_RIGHTBUTTON,pt.x,pt.y,this);

	*pResult = 0;
}

// 属性列表鼠标右击事件
void CXmlEditDlg::OnNMRclickListAttributes(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: 在此添加控件通知处理程序代码

	CMenu mu;
	mu.LoadMenu(IDR_MENU_LIST);

	CMenu* pMenu = mu.GetSubMenu(0);

	POINT pt;
	GetCursorPos(&pt);
	pMenu->TrackPopupMenu(TPM_LEFTALIGN |TPM_RIGHTBUTTON,pt.x,pt.y,this);

	*pResult = 0;
}

// 程序关闭事件处理
void CXmlEditDlg::OnClose()
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	CDialog::OnClose();
}

// 程序取消事件处理
void CXmlEditDlg::OnCancel()
{
	// TODO: 在此添加专用代码和/或调用基类
	if( IDYES != MessageBox("将要退出程序，确定要退出吗？","退出",MB_YESNOCANCEL|MB_ICONQUESTION) )
		return;

	CDialog::OnCancel();
}

// 新建文件
void CXmlEditDlg::OnFileNew()
{
	// TODO: 在此添加命令处理程序代码

	if( IDYES != MessageBox("新建操作不会自动保存当前的数据，确定要新建数据吗？","新建",MB_YESNO|MB_ICONQUESTION) )
		return;

	m_xnData.release();
	m_pActiveNode = &m_xnData;

	RefreshTree_Nodes();
	RefreshList_Attributes();
}

//新增结点
void CXmlEditDlg::OnTreepopumenuAddNode()
{
	// TODO: 在此添加命令处理程序代码
	CStandardInputDlg inputDlg(this);

	if( IDOK == inputDlg.DoModal() )
	{
		std::string strRet = inputDlg.GetInputString();

		HTREEITEM hTreeItem = m_treeNodes.GetSelectedItem();
		if( hTreeItem )
		{
			xnd::XmlNode* pNode = GetXmlNodeFromTreeItem(m_treeNodes,hTreeItem,&m_xnData);
			xnd::XmlNode* pCreateNode = pNode->createChildNode(strRet.c_str());
			if( pCreateNode )
			{
				int nIndex = pNode->getChildNodeCount() - 1;

				if( strRet.empty() )
					strRet = "(null)";

				HTREEITEM hRetItem = m_treeNodes.InsertItem(strRet.c_str(),hTreeItem);
				m_treeNodes.SetItemData(hRetItem,static_cast<DWORD_PTR>(nIndex));
			}
			else
			{
				MessageBox("添加子结点失败！","添加",MB_OK|MB_ICONERROR);
			}
		}
	}
}

//重命名结点
void CXmlEditDlg::OnTreepopumenuR()
{
	// TODO: 在此添加命令处理程序代码

	// 先取到当前结点
	HTREEITEM hTreeItem = m_treeNodes.GetSelectedItem();
	xnd::XmlNode* pNode = GetXmlNodeFromTreeItem(m_treeNodes,hTreeItem,&m_xnData);

	// 执行重新命名
	CStandardInputDlg inputDlg(this);

	inputDlg.SetInitString(pNode->getName());

	if( IDOK == inputDlg.DoModal() )
	{
		std::string strRet = inputDlg.GetInputString();

		pNode->setName(strRet.c_str());

		if( strRet.empty() )
			strRet = "(null)";

		m_treeNodes.SetItemText(hTreeItem,strRet.c_str());
	}
}

// 显示属性编辑对话框
void CXmlEditDlg::ShowAttributeDlg(void)
{
	if( m_listAttributes.GetItemCount() > 0 )
	{
		POSITION pos = m_listAttributes.GetFirstSelectedItemPosition();
		int nIndex = m_listAttributes.GetNextSelectedItem(pos);
		if( -1 != nIndex ) 
		{
			std::string strAttr = m_listAttributes.GetItemText(nIndex,0).GetBuffer(0);

			CAttributeEditDlg attrEditDlg(this);
			attrEditDlg.SetAttribute(m_pActiveNode,strAttr);
			if( IDOK == attrEditDlg.DoModal() )
			{
				RefreshList_Attributes();
			}
		}
	}
}

BOOL CXmlEditDlg::DestroyWindow()
{
	// TODO: 在此添加专用代码和/或调用基类

	if( m_pCopyNode )
	{
		delete m_pCopyNode;
		m_pCopyNode = NULL;
	}

	return CDialog::DestroyWindow();
}

// 属性编辑
void CXmlEditDlg::OnListpopumenuAttributeEdit()
{
	// TODO: 在此添加命令处理程序代码
	ShowAttributeDlg();
}

// 属性列表双击事件
void CXmlEditDlg::OnNMDblclkListAttributes(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: 在此添加控件通知处理程序代码
	ShowAttributeDlg();
	*pResult = 1;
}

// 关于
void CXmlEditDlg::OnAppAbout()
{
	// TODO: 在此添加命令处理程序代码
	CAboutDlg dlgAbout;
	dlgAbout.DoModal();
}

// 删除属性
void CXmlEditDlg::OnListpopumenuDeleteAttribute()
{
	// TODO: 在此添加命令处理程序代码
	if( m_listAttributes.GetItemCount() > 0 )
	{
		POSITION pos = m_listAttributes.GetFirstSelectedItemPosition();
		int nIndex = m_listAttributes.GetNextSelectedItem(pos);
		if( -1 != nIndex ) 
		{
			if( IDYES == MessageBox("是否要删除当前选中的属性？","删除属性",MB_YESNO|MB_ICONQUESTION) )
			{
				std::string strAttr = m_listAttributes.GetItemText(nIndex,0).GetBuffer(0);
				m_pActiveNode->destroyAttribute( strAttr.c_str() );

				RefreshList_Attributes();
			}
		}
	}
}

// 删除结点
void CXmlEditDlg::OnTreepopumenuDeleteNode()
{
	// TODO: 在此添加命令处理程序代码
	HTREEITEM hSelectItem = m_treeNodes.GetSelectedItem();
	if( hSelectItem )
	{
		if( IDYES == MessageBox("是否要删除当前选中的结点？","删除结点",MB_YESNO|MB_ICONQUESTION) )
		{
			HTREEITEM hItem = m_treeNodes.GetNextSiblingItem( hSelectItem );
			while( hItem )
			{
				int nIndex =static_cast<int>( m_treeNodes.GetItemData( hItem ) ) - 1;
				m_treeNodes.SetItemData( hItem, static_cast<DWORD_PTR>(nIndex) );

				hItem = m_treeNodes.GetNextSiblingItem( hItem );
			}

			xnd::XmlNode* pNode = GetXmlNodeFromTreeItem( m_treeNodes, hSelectItem, &m_xnData );

			if( pNode->getRoot() )
			{
				pNode->release();
				delete pNode;

				m_treeNodes.DeleteItem(hSelectItem);
				RefreshList_Attributes();
			}
			else
			{
				pNode->release();

				RefreshTree_Nodes();
				RefreshList_Attributes();
			}
		}
	}
}

// 新增属性
void CXmlEditDlg::OnNewAttribute()
{
	// TODO: 在此添加命令处理程序代码
	CAttributeCreateDlg dlg(this);

	while( 1 )
	{

		if( IDOK == dlg.DoModal() )
		{
			if( m_pActiveNode->getAttribute( dlg.m_strName.c_str() ) )
			{
				MessageBox("属性已经存在，请重新输入","重复",MB_OK|MB_ICONWARNING);
			}
			else
			{
				std::string			strName = dlg.m_strName;
				xnd::XmlDataType	eType   = xnd::XmlStream::cvtStringToType(dlg.m_strType);

				m_pActiveNode->createAttribute( eType, strName.c_str() );
				RefreshList_Attributes();

				break;
			}
		}
		else
		{
			break;
		}
	}
}

void CXmlEditDlg::OnSizing(UINT fwSide, LPRECT pRect)
{
	CDialog::OnSizing(fwSide, pRect);

	// TODO: 在此处添加消息处理程序代码
//	InvalidateRect(NULL,FALSE);
}

void CXmlEditDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);

	// TODO: 在此处添加消息处理程序代码
	InvalidateRect(NULL,FALSE);
}

void CXmlEditDlg::OnOK()
{
	// TODO: 在此添加专用代码和/或调用基类
	CWnd* pWnd = GetFocus();
	if( pWnd && (pWnd->GetSafeHwnd() == m_listAttributes.GetSafeHwnd() ) )
	{
		ShowAttributeDlg();
	}

	return;//避免程序一按回车就退出
	//CDialog::OnOK();
}

// 拷贝结点
void CXmlEditDlg::OnCopyNode()
{
	// TODO: 在此添加命令处理程序代码
	if( m_pCopyNode )
	{
		delete m_pCopyNode;
		m_pCopyNode = NULL;
	}

	m_pCopyNode = m_pActiveNode->clone();
}

// 粘贴结点
void CXmlEditDlg::OnPastNode()
{
	// TODO: 在此添加命令处理程序代码
	if( m_pCopyNode )
	{
		xnd::XmlNode* pNode = m_pActiveNode->addChildNode( m_pCopyNode->clone() );
		HTREEITEM hItem = m_treeNodes.GetSelectedItem();

		RefreshTreeCtrl( m_treeNodes, hItem, m_pActiveNode->getChildNodeCount()-1, *pNode );
	}
	else
	{
		MessageBox("当前剪贴板数据为空！","复制结点",MB_OK | MB_ICONWARNING);
	}
}

// 清除剪贴板
void CXmlEditDlg::OnReleaseClipbord()
{
	// TODO: 在此添加命令处理程序代码
	if( m_pCopyNode )
	{
		delete m_pCopyNode;
		m_pCopyNode = NULL;
	}
}

// 导出结点
void CXmlEditDlg::OnExportNode()
{
	// TODO: 在此添加命令处理程序代码
	char strFilter[] = "TextXml Files (*.txml)|*.txml|BinaryXml Files (*.bxml)|*.bxml||";
	CFileDialog fd( FALSE,NULL,NULL, OFN_HIDEREADONLY,strFilter,this);

	if(  IDOK == fd.DoModal() )
	{
		CString strFile   = fd.GetFileName();

		int nIndex = fd.GetOFN().nFilterIndex;
		if( nIndex == 1 )
		{
			if( -1 == strFile.Find(".txml") )
				strFile.Append(".txml");

			if( xnd::XmlStream::saveTextXml( strFile.GetBuffer(0),m_pActiveNode ))
			{
				MessageBox("导出结点到文本xml成功！","导出结点",MB_OK);
			}
			else
			{
				MessageBox("导出结点到文本xml失败！","导出结点",MB_OK|MB_ICONERROR);
			}
		}
		else
		{
			if( -1 == strFile.Find(".bxml") )
				strFile.Append(".bxml");

			if( xnd::XmlStream::saveBinaryXml( strFile.GetBuffer(0),m_pActiveNode ))
			{
				MessageBox("导出结点到二进制xml成功！","导出结点",MB_OK);
			}
			else
			{
				MessageBox("导出结点到二进制xml失败！","导出结点",MB_OK|MB_ICONERROR);
			}
		}
	}

}

// 导入结点
void CXmlEditDlg::OnImportExport()
{
	// TODO: 在此添加命令处理程序代码
	char strFilter[] = "TextXml Files (*.txml)|*.txml|BinaryXml Files (*.bxml)|*.bxml||";
	CFileDialog fd( TRUE,NULL,NULL,OFN_FILEMUSTEXIST| OFN_HIDEREADONLY,strFilter,this);

	if(  IDOK == fd.DoModal() )
	{
		int nIndex = fd.GetOFN().nFilterIndex;
		if( nIndex == 1 )
		{
			xnd::XmlNode* pNode = new xnd::XmlNode;

			if( xnd::XmlStream::loadTextXml(fd.GetFileName().GetBuffer(0),pNode) )
			{
				xnd::XmlNode* _pNode = m_pActiveNode->addChildNode( pNode );
				HTREEITEM hItem = m_treeNodes.GetSelectedItem();

				RefreshTreeCtrl( m_treeNodes, hItem, m_pActiveNode->getChildNodeCount()-1, *_pNode );

				MessageBox("从文本xml导入结点成功！","导入结点",MB_OK);
			}
			else
			{
				delete pNode;
				MessageBox("从文本xml导入结点失败！","导入结点",MB_OK|MB_ICONERROR);
			}
		}
		else
		{
			xnd::XmlNode* pNode = new xnd::XmlNode;

			if( xnd::XmlStream::loadBinaryXml(fd.GetFileName().GetBuffer(0),pNode) )
			{
				xnd::XmlNode* _pNode = m_pActiveNode->addChildNode( pNode );
				HTREEITEM hItem = m_treeNodes.GetSelectedItem();

				RefreshTreeCtrl( m_treeNodes, hItem, m_pActiveNode->getChildNodeCount()-1, *_pNode );

				MessageBox("从二进制xml导入结点成功！","导入结点",MB_OK);
			}
			else
			{
				delete pNode;
				MessageBox("从二进制xml导入结点失败！","导入结点",MB_OK|MB_ICONERROR);
			}
		}
	}
}
