// AttributeCreateDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "XmlEdit.h"
#include "AttributeCreateDlg.h"
#include ".\attributecreatedlg.h"


// CAttributeCreateDlg 对话框

IMPLEMENT_DYNAMIC(CAttributeCreateDlg, CDialog)
CAttributeCreateDlg::CAttributeCreateDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CAttributeCreateDlg::IDD, pParent)
{
}

CAttributeCreateDlg::~CAttributeCreateDlg()
{
}

void CAttributeCreateDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CAttributeCreateDlg, CDialog)
END_MESSAGE_MAP()


// CAttributeCreateDlg 消息处理程序

BOOL CAttributeCreateDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  在此添加额外的初始化

	// 加入属性类型
	// ---------------------------------------------------------------------------------------------
	CEdit*		pEditName		= static_cast<CEdit*>(GetDlgItem(IDC_EDIT_NAME));
	CComboBox*	pComboBoxType	= static_cast<CComboBox*>( GetDlgItem(IDC_COMBO_TYPE) );

	pComboBoxType->AddString( xnd::XmlStream::cvtTypeToString( xnd::XML_CHAR   ).c_str() );
	pComboBoxType->AddString( xnd::XmlStream::cvtTypeToString( xnd::XML_INT    ).c_str() );
	pComboBoxType->AddString( xnd::XmlStream::cvtTypeToString( xnd::XML_FLOAT  ).c_str() );
	pComboBoxType->AddString( xnd::XmlStream::cvtTypeToString( xnd::XML_STRING ).c_str() );
	pComboBoxType->AddString( xnd::XmlStream::cvtTypeToString( xnd::XML_BINARY ).c_str() );

	pComboBoxType->SetCurSel(0);

	if( !m_strName.empty() )
	{
		pEditName->SetWindowText( m_strName.c_str() );
	}

	if( !m_strType.empty() )
	{
		int nIndex = pComboBoxType->FindString(0,m_strType.c_str() );
		pComboBoxType->SetCurSel(nIndex);
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

void CAttributeCreateDlg::OnOK()
{
	// TODO: 在此添加专用代码和/或调用基类
	CEdit*		pEditName		= static_cast<CEdit*>(GetDlgItem(IDC_EDIT_NAME));
	CComboBox*	pComboBoxType	= static_cast<CComboBox*>( GetDlgItem(IDC_COMBO_TYPE) );

	CString strName,strType;
	pEditName->GetWindowText(strName);
	pComboBoxType->GetWindowText(strType);

	m_strName = strName.GetBuffer(0);
	m_strType = strType.GetBuffer(0);

	if( m_strName.empty() )
	{
		return;
	}

	CDialog::OnOK();
}
