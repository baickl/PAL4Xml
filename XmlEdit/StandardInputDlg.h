#pragma once

#include <string>


// CStandardInputDlg 对话框

class CStandardInputDlg : public CDialog
{
	DECLARE_DYNAMIC(CStandardInputDlg)

public:
	CStandardInputDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CStandardInputDlg();

// 对话框数据
	enum { IDD = IDD_STANDARD_INPUT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

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
