#include "Xml/XmlStream.h"

#pragma once

// CAttributeEditDlg 对话框

class CAttributeEditDlg : public CDialog
{
	DECLARE_DYNAMIC(CAttributeEditDlg)

public:
	CAttributeEditDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CAttributeEditDlg();

// 对话框数据
	enum { IDD = IDD_ATTRIBUTE_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	// 当前正在操作的结点
	xnd::XmlNode* m_pCurrentNode;
	// 当前操作的属性名称
	std::string m_strAttributeName;
	// 设置属性
	void SetAttribute(xnd::XmlNode* pNode, std::string strAttribute);
	afx_msg void OnBnClickedOk();
	virtual BOOL OnInitDialog();
	afx_msg void OnCbnSelchangeComboType();
	afx_msg void OnBnClickedButtonImport();
	afx_msg void OnBnClickedButtonExport();
	afx_msg void OnBnClickedButtonApply();
};
