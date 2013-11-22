#include <string>
#include "Xml/XmlStream.h"

#pragma once


// CAttributeCreateDlg 对话框

class CAttributeCreateDlg : public CDialog
{
	DECLARE_DYNAMIC(CAttributeCreateDlg)

public:
	CAttributeCreateDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CAttributeCreateDlg();

// 对话框数据
	enum { IDD = IDD_ATTRIBUTECREAT_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	// 属性名称
	std::string m_strName;
	// 属性类别
	std::string m_strType;
	virtual BOOL OnInitDialog();
protected:
	virtual void OnOK();
};
