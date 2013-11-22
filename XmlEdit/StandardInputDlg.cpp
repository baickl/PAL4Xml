// StandardInputDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "XmlEdit.h"
#include "StandardInputDlg.h"
#include ".\standardinputdlg.h"


// CStandardInputDlg 对话框

IMPLEMENT_DYNAMIC(CStandardInputDlg, CDialog)
CStandardInputDlg::CStandardInputDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CStandardInputDlg::IDD, pParent)
{
}

CStandardInputDlg::~CStandardInputDlg()
{
}

void CStandardInputDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CStandardInputDlg, CDialog)
END_MESSAGE_MAP()


// CStandardInputDlg 消息处理程序

const std::string& CStandardInputDlg::GetInputString(void)
{
	return m_strInputString;
}

void CStandardInputDlg::SetInitString(const std::string& strInitString)
{
	m_strInputString = strInitString;
}

BOOL CStandardInputDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  在此添加额外的初始化
	CEdit* pEdit = static_cast<CEdit*>(GetDlgItem(IDC_EDIT));
	pEdit->SetWindowText(m_strInputString.c_str());

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

void CStandardInputDlg::OnOK()
{
	// TODO: 在此添加专用代码和/或调用基类
	CEdit* pEdit = static_cast<CEdit*>(GetDlgItem(IDC_EDIT));

	CString strOut;
	pEdit->GetWindowText(strOut);

	m_strInputString = strOut.GetBuffer(0);

	CDialog::OnOK();
}
