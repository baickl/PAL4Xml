#pragma once

#include <string>


// CStandardInputDlg �Ի���

class CStandardInputDlg : public CDialog
{
	DECLARE_DYNAMIC(CStandardInputDlg)

public:
	CStandardInputDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CStandardInputDlg();

// �Ի�������
	enum { IDD = IDD_STANDARD_INPUT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
private:
	std::string m_strInputString;
public:
	const std::string& GetInputString(void);
	void SetInitString(const std::string& strInitString);
	virtual BOOL OnInitDialog();
protected:
	virtual void OnOK();
};
