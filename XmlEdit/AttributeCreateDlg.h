#include <string>
#include "Xml/XmlStream.h"

#pragma once


// CAttributeCreateDlg �Ի���

class CAttributeCreateDlg : public CDialog
{
	DECLARE_DYNAMIC(CAttributeCreateDlg)

public:
	CAttributeCreateDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CAttributeCreateDlg();

// �Ի�������
	enum { IDD = IDD_ATTRIBUTECREAT_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	// ��������
	std::string m_strName;
	// �������
	std::string m_strType;
	virtual BOOL OnInitDialog();
protected:
	virtual void OnOK();
};
