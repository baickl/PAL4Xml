// XmlEditDlg.cpp : ʵ���ļ�
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

// ���ݵ�ǰѡ�е�Itemȡ��XmlNode
xnd::XmlNode* GetXmlNodeFromTreeItem(CTreeCtrl& CtrlTree,HTREEITEM TI,xnd::XmlNode* pNode )
{
	STACK_NODE snNode;

	// ȡ�ý��·��ջ
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

// ˢ�����Ϳؼ�
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

// ˢ���б�ؼ�
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

// ����Ӧ�ó��򡰹��ڡ��˵���� CAboutDlg �Ի���

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// �Ի�������
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

// ʵ��
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


// CXmlEditDlg �Ի���



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


// CXmlEditDlg ��Ϣ�������

BOOL CXmlEditDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// ���ô˶Ի����ͼ�ꡣ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	// TODO: �ڴ���Ӷ���ĳ�ʼ������

	// ��ʼ�����Ϳؼ�
	m_treeNodes.SetBkColor(RGB(210,230,190));

	// ����Image
	m_ilImageList.Create(16,16,1,0,2);
	
	m_ilImageList.Add( AfxGetApp()->LoadIcon( IDI_UNSELECT ) );
	m_ilImageList.Add( AfxGetApp()->LoadIcon( IDI_SELECT )   );
	

	// ��ʼ���б�ؼ�
	m_listAttributes.ModifyStyle( 0, LVS_REPORT );
	m_listAttributes.ModifyStyle( LVS_EDITLABELS, 0 );

	m_listAttributes.SetExtendedStyle( LVS_EX_FULLROWSELECT|LVS_EX_ONECLICKACTIVATE|LVS_EX_INFOTIP|
									   LVS_EX_SUBITEMIMAGES|LVS_EX_GRIDLINES );

	m_listAttributes.InsertColumn( 0, "����", LVCFMT_LEFT,  100 );
	m_listAttributes.InsertColumn( 1, "����", LVCFMT_LEFT,  100 );
	m_listAttributes.InsertColumn( 2, "��С", LVCFMT_RIGHT, 100 );
	m_listAttributes.InsertColumn( 3, "����", LVCFMT_LEFT,  200 );


	// ˢ���б������Ϳؼ�
	RefreshTree_Nodes();
	RefreshList_Attributes();

	PostMessage(WM_SYSCOMMAND,SC_MAXIMIZE,0);
	
	return TRUE;  // ���������˿ؼ��Ľ��㣬���򷵻� TRUE
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

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CXmlEditDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ��������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		// ȡ�õ�ǰ���ڴ�С
		RECT rt;
		GetClientRect(&rt);

		// ȡ�����Ϳؼ�����
		RECT rtTree;
		m_treeNodes.GetClientRect(&rtTree);

		rtTree.right = 183;
		rtTree.bottom = rt.bottom - 1;
		m_treeNodes.MoveWindow(&rtTree,TRUE);

		// ȡ���б�ؼ�����
		RECT rtList;
		m_listAttributes.GetClientRect(&rtList);

		rtList.left = rtTree.right + 1;
		rtList.top  = rtTree.top;

		rtList.right  = rt.right-1;
		rtList.bottom = rt.bottom - 1;

		m_listAttributes.MoveWindow(&rtList,TRUE);

		// ����list���б�ͷ
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

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù����ʾ��
HCURSOR CXmlEditDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CXmlEditDlg::OnHelpAbout()
{
	// TODO: �ڴ���������������
	CAboutDlg dlgAbout;
	dlgAbout.DoModal();
}

void CXmlEditDlg::OnFileExit()
{
	// TODO: �ڴ���������������
	if( IDYES != MessageBox("��Ҫ�˳�����ȷ��Ҫ�˳���","�˳�",MB_YESNOCANCEL|MB_ICONQUESTION) )
		return;

	CDialog::OnOK();
}

// �ļ���
void CXmlEditDlg::OnFileOpen32772()
{
	// TODO: �ڴ���������������
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
				strMsg.Format("���ı�xml�ļ�: %s ʧ�ܣ�.",fd.GetFileName().GetBuffer(0));
				MessageBox(strMsg,"��",MB_OK);
			}
		}
		else
		{
			if( !xnd::XmlStream::loadBinaryXml(fd.GetFileName().GetBuffer(0),&m_xnData ) )
			{
				CString strMsg;
				strMsg.Format("�򿪶�����xml�ļ�: %s ʧ�ܣ�.",fd.GetFileName().GetBuffer(0));
				MessageBox(strMsg,"��",MB_OK);
			}
		}

		m_pActiveNode = &m_xnData;
		
		RefreshTree_Nodes();
		RefreshList_Attributes();
	}
}

// �ļ�����
void CXmlEditDlg::OnFileSave32773()
{
	// TODO: �ڴ���������������
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
				MessageBox("����Ϊ�ı�xmlʧ�ܣ�","����",MB_OK);
			else
				MessageBox("����Ϊ�ı�xml�ɹ���","����",MB_OK);
		}
		else if( nIndex == 2)
		{
			if( -1 == strFile.Find(".bxml") )
				strFile.Append(".bxml");

			if( !xnd::XmlStream::saveBinaryXml( strFile.GetBuffer(0), &m_xnData ))
				MessageBox("����Ϊ������xmlʧ�ܣ�","����",MB_OK);
			else
				MessageBox("����Ϊ������xml�ɹ���","����",MB_OK);
		}
	}
}

// �����ѡ����ı��¼�����
void CXmlEditDlg::OnTvnSelchangedTreeNodes(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	m_treeNodes.SetItemImage( pNMTreeView->itemNew.hItem, 0, 1 );
	m_treeNodes.SetItemImage( pNMTreeView->itemOld.hItem, 0, 0 );

	// ȡ�õ�ǰ���
	m_pActiveNode = 
		GetXmlNodeFromTreeItem(m_treeNodes,pNMTreeView->itemNew.hItem,&m_xnData);
  
	RefreshList_Attributes();

	*pResult = 0;
}

// �����ѡ�������ı��¼�����
void CXmlEditDlg::OnTvnSelchangingTreeNodes(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	*pResult = 0;
}

// ˢ�½����
void CXmlEditDlg::RefreshTree_Nodes(void)
{
	VERIFY(m_treeNodes.DeleteAllItems());
	m_treeNodes.SetImageList(&m_ilImageList,LVSIL_NORMAL);

	RefreshTreeCtrl(m_treeNodes,TVI_ROOT,0,m_xnData);
}

// ˢ�������б�
void CXmlEditDlg::RefreshList_Attributes(void)
{
	RefreshListCtrl(m_listAttributes,m_pActiveNode);
}

// ���������һ��¼�
void CXmlEditDlg::OnNMRclickTreeNodes(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������

	// �����˵�
	CMenu mu;
	mu.LoadMenu(IDR_MENU_TREE);

	CMenu* pMenu = mu.GetSubMenu(0);

	POINT pt;
	GetCursorPos(&pt);
	pMenu->TrackPopupMenu(TPM_LEFTALIGN |TPM_RIGHTBUTTON,pt.x,pt.y,this);

	*pResult = 0;
}

// �����б�����һ��¼�
void CXmlEditDlg::OnNMRclickListAttributes(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������

	CMenu mu;
	mu.LoadMenu(IDR_MENU_LIST);

	CMenu* pMenu = mu.GetSubMenu(0);

	POINT pt;
	GetCursorPos(&pt);
	pMenu->TrackPopupMenu(TPM_LEFTALIGN |TPM_RIGHTBUTTON,pt.x,pt.y,this);

	*pResult = 0;
}

// ����ر��¼�����
void CXmlEditDlg::OnClose()
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ

	CDialog::OnClose();
}

// ����ȡ���¼�����
void CXmlEditDlg::OnCancel()
{
	// TODO: �ڴ����ר�ô����/����û���
	if( IDYES != MessageBox("��Ҫ�˳�����ȷ��Ҫ�˳���","�˳�",MB_YESNOCANCEL|MB_ICONQUESTION) )
		return;

	CDialog::OnCancel();
}

// �½��ļ�
void CXmlEditDlg::OnFileNew()
{
	// TODO: �ڴ���������������

	if( IDYES != MessageBox("�½����������Զ����浱ǰ�����ݣ�ȷ��Ҫ�½�������","�½�",MB_YESNO|MB_ICONQUESTION) )
		return;

	m_xnData.release();
	m_pActiveNode = &m_xnData;

	RefreshTree_Nodes();
	RefreshList_Attributes();
}

//�������
void CXmlEditDlg::OnTreepopumenuAddNode()
{
	// TODO: �ڴ���������������
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
				MessageBox("����ӽ��ʧ�ܣ�","���",MB_OK|MB_ICONERROR);
			}
		}
	}
}

//���������
void CXmlEditDlg::OnTreepopumenuR()
{
	// TODO: �ڴ���������������

	// ��ȡ����ǰ���
	HTREEITEM hTreeItem = m_treeNodes.GetSelectedItem();
	xnd::XmlNode* pNode = GetXmlNodeFromTreeItem(m_treeNodes,hTreeItem,&m_xnData);

	// ִ����������
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

// ��ʾ���Ա༭�Ի���
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
	// TODO: �ڴ����ר�ô����/����û���

	if( m_pCopyNode )
	{
		delete m_pCopyNode;
		m_pCopyNode = NULL;
	}

	return CDialog::DestroyWindow();
}

// ���Ա༭
void CXmlEditDlg::OnListpopumenuAttributeEdit()
{
	// TODO: �ڴ���������������
	ShowAttributeDlg();
}

// �����б�˫���¼�
void CXmlEditDlg::OnNMDblclkListAttributes(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	ShowAttributeDlg();
	*pResult = 1;
}

// ����
void CXmlEditDlg::OnAppAbout()
{
	// TODO: �ڴ���������������
	CAboutDlg dlgAbout;
	dlgAbout.DoModal();
}

// ɾ������
void CXmlEditDlg::OnListpopumenuDeleteAttribute()
{
	// TODO: �ڴ���������������
	if( m_listAttributes.GetItemCount() > 0 )
	{
		POSITION pos = m_listAttributes.GetFirstSelectedItemPosition();
		int nIndex = m_listAttributes.GetNextSelectedItem(pos);
		if( -1 != nIndex ) 
		{
			if( IDYES == MessageBox("�Ƿ�Ҫɾ����ǰѡ�е����ԣ�","ɾ������",MB_YESNO|MB_ICONQUESTION) )
			{
				std::string strAttr = m_listAttributes.GetItemText(nIndex,0).GetBuffer(0);
				m_pActiveNode->destroyAttribute( strAttr.c_str() );

				RefreshList_Attributes();
			}
		}
	}
}

// ɾ�����
void CXmlEditDlg::OnTreepopumenuDeleteNode()
{
	// TODO: �ڴ���������������
	HTREEITEM hSelectItem = m_treeNodes.GetSelectedItem();
	if( hSelectItem )
	{
		if( IDYES == MessageBox("�Ƿ�Ҫɾ����ǰѡ�еĽ�㣿","ɾ�����",MB_YESNO|MB_ICONQUESTION) )
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

// ��������
void CXmlEditDlg::OnNewAttribute()
{
	// TODO: �ڴ���������������
	CAttributeCreateDlg dlg(this);

	while( 1 )
	{

		if( IDOK == dlg.DoModal() )
		{
			if( m_pActiveNode->getAttribute( dlg.m_strName.c_str() ) )
			{
				MessageBox("�����Ѿ����ڣ�����������","�ظ�",MB_OK|MB_ICONWARNING);
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

	// TODO: �ڴ˴������Ϣ����������
//	InvalidateRect(NULL,FALSE);
}

void CXmlEditDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);

	// TODO: �ڴ˴������Ϣ����������
	InvalidateRect(NULL,FALSE);
}

void CXmlEditDlg::OnOK()
{
	// TODO: �ڴ����ר�ô����/����û���
	CWnd* pWnd = GetFocus();
	if( pWnd && (pWnd->GetSafeHwnd() == m_listAttributes.GetSafeHwnd() ) )
	{
		ShowAttributeDlg();
	}

	return;//�������һ���س����˳�
	//CDialog::OnOK();
}

// �������
void CXmlEditDlg::OnCopyNode()
{
	// TODO: �ڴ���������������
	if( m_pCopyNode )
	{
		delete m_pCopyNode;
		m_pCopyNode = NULL;
	}

	m_pCopyNode = m_pActiveNode->clone();
}

// ճ�����
void CXmlEditDlg::OnPastNode()
{
	// TODO: �ڴ���������������
	if( m_pCopyNode )
	{
		xnd::XmlNode* pNode = m_pActiveNode->addChildNode( m_pCopyNode->clone() );
		HTREEITEM hItem = m_treeNodes.GetSelectedItem();

		RefreshTreeCtrl( m_treeNodes, hItem, m_pActiveNode->getChildNodeCount()-1, *pNode );
	}
	else
	{
		MessageBox("��ǰ����������Ϊ�գ�","���ƽ��",MB_OK | MB_ICONWARNING);
	}
}

// ���������
void CXmlEditDlg::OnReleaseClipbord()
{
	// TODO: �ڴ���������������
	if( m_pCopyNode )
	{
		delete m_pCopyNode;
		m_pCopyNode = NULL;
	}
}

// �������
void CXmlEditDlg::OnExportNode()
{
	// TODO: �ڴ���������������
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
				MessageBox("������㵽�ı�xml�ɹ���","�������",MB_OK);
			}
			else
			{
				MessageBox("������㵽�ı�xmlʧ�ܣ�","�������",MB_OK|MB_ICONERROR);
			}
		}
		else
		{
			if( -1 == strFile.Find(".bxml") )
				strFile.Append(".bxml");

			if( xnd::XmlStream::saveBinaryXml( strFile.GetBuffer(0),m_pActiveNode ))
			{
				MessageBox("������㵽������xml�ɹ���","�������",MB_OK);
			}
			else
			{
				MessageBox("������㵽������xmlʧ�ܣ�","�������",MB_OK|MB_ICONERROR);
			}
		}
	}

}

// ������
void CXmlEditDlg::OnImportExport()
{
	// TODO: �ڴ���������������
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

				MessageBox("���ı�xml������ɹ���","������",MB_OK);
			}
			else
			{
				delete pNode;
				MessageBox("���ı�xml������ʧ�ܣ�","������",MB_OK|MB_ICONERROR);
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

				MessageBox("�Ӷ�����xml������ɹ���","������",MB_OK);
			}
			else
			{
				delete pNode;
				MessageBox("�Ӷ�����xml������ʧ�ܣ�","������",MB_OK|MB_ICONERROR);
			}
		}
	}
}
