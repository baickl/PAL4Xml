#include "Xml/XmlStream.h"

#pragma once

// CAttributeEditDlg �Ի���

class CAttributeEditDlg : public CDialog
{
	DECLARE_DYNAMIC(CAttributeEditDlg)

public:
	CAttributeEditDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CAttributeEditDlg();

// �Ի�������
	enum { IDD = IDD_ATTRIBUTE_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	// ��ǰ���ڲ����Ľ��
	xnd::XmlNode* m_pCurrentNode;
	// ��ǰ��������������
	std::string m_strAttributeName;
	// ��������
	void SetAttribute(xnd::XmlNode* pNode, std::string strAttribute);
	afx_msg void OnBnClickedOk();
	virtual BOOL OnInitDialog();
	afx_msg void OnCbnSelchangeComboType();
	afx_msg void OnBnClickedButtonImport();
	afx_msg void OnBnClickedButtonExport();
	afx_msg void OnBnClickedButtonApply();
};
