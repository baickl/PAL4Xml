// StandardInputDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "XmlEdit.h"
#include "StandardInputDlg.h"
#include ".\standardinputdlg.h"


// CStandardInputDlg �Ի���

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


// CStandardInputDlg ��Ϣ�������

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

	// TODO:  �ڴ���Ӷ���ĳ�ʼ��
	CEdit* pEdit = static_cast<CEdit*>(GetDlgItem(IDC_EDIT));
	pEdit->SetWindowText(m_strInputString.c_str());

	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣: OCX ����ҳӦ���� FALSE
}

void CStandardInputDlg::OnOK()
{
	// TODO: �ڴ����ר�ô����/����û���
	CEdit* pEdit = static_cast<CEdit*>(GetDlgItem(IDC_EDIT));

	CString strOut;
	pEdit->GetWindowText(strOut);

	m_strInputString = strOut.GetBuffer(0);

	CDialog::OnOK();
}
