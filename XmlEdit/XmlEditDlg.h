// XmlEditDlg.h : 头文件
//

#pragma once
#include "afxcmn.h"
#include "xml\xmlnode.h"
#include "AttributeEditDlg.h"


// CXmlEditDlg 对话框
class CXmlEditDlg : public CDialog
{
// 构造
public:
	CXmlEditDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_XMLEDIT_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	CListCtrl m_listAttributes;
	CTreeCtrl m_treeNodes;
	xnd::XmlNode m_xnData;
	afx_msg void OnHelpAbout();
	afx_msg void OnFileExit();
	afx_msg void OnFileOpen32772();
	afx_msg void OnFileSave32773();
protected:
	CImageList m_ilImageList;
	xnd::XmlNode* m_pActiveNode;
public:
	afx_msg void OnTvnSelchangedTreeNodes(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnTvnSelchangingTreeNodes(NMHDR *pNMHDR, LRESULT *pResult);
	void RefreshTree_Nodes(void);
	void RefreshList_Attributes(void);
	afx_msg void OnNMRclickTreeNodes(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMRclickListAttributes(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnClose();
protected:
	virtual void OnCancel();
public:
	afx_msg void OnFileNew();
	afx_msg void OnTreepopumenuAddNode();
	afx_msg void OnTreepopumenuR();
	void ShowAttributeDlg(void);
	virtual BOOL DestroyWindow();
	afx_msg void OnListpopumenuAttributeEdit();
	afx_msg void OnNMDblclkListAttributes(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnAppAbout();
	afx_msg void OnListpopumenuDeleteAttribute();
	afx_msg void OnTreepopumenuDeleteNode();
	afx_msg void OnNewAttribute();
	afx_msg void OnSizing(UINT fwSide, LPRECT pRect);
	afx_msg void OnSize(UINT nType, int cx, int cy);
protected:
	virtual void OnOK();
public:
	afx_msg void OnCopyNode();
	afx_msg void OnPastNode();
private:
	// 拷贝的结点
	xnd::XmlNode* m_pCopyNode;
public:
	afx_msg void OnReleaseClipbord();
	afx_msg void OnExportNode();
	afx_msg void OnImportExport();
};
