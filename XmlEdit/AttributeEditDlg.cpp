// AttributeEditDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "XmlEdit.h"
#include "AttributeEditDlg.h"
#include ".\attributeeditdlg.h"


// CAttributeEditDlg 对话框

IMPLEMENT_DYNAMIC(CAttributeEditDlg, CDialog)
CAttributeEditDlg::CAttributeEditDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CAttributeEditDlg::IDD, pParent)
	, m_pCurrentNode(NULL)
{
}

CAttributeEditDlg::~CAttributeEditDlg()
{
}

void CAttributeEditDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CAttributeEditDlg, CDialog)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_CBN_SELCHANGE(IDC_COMBO_TYPE, OnCbnSelchangeComboType)
	ON_BN_CLICKED(IDC_BUTTON_IMPORT, OnBnClickedButtonImport)
	ON_BN_CLICKED(IDC_BUTTON_EXPORT, OnBnClickedButtonExport)
	ON_BN_CLICKED(IDC_BUTTON_APPLY, OnBnClickedButtonApply)
END_MESSAGE_MAP()


// 设置属性

void CAttributeEditDlg::SetAttribute(xnd::XmlNode* pNode, std::string strAttribute)
{
	m_pCurrentNode		= pNode;
	m_strAttributeName	= strAttribute;
}

void CAttributeEditDlg::OnBnClickedOk()
{
	// TODO: 在此添加控件通知处理程序代码

	// 取得界面相关数据
	CEdit*		pEditName		= static_cast<CEdit*>( GetDlgItem(IDC_EDIT_NAME) );
	CComboBox*	pComboBoxType	= static_cast<CComboBox*>( GetDlgItem(IDC_COMBO_TYPE) );
	CEdit*		pEditValue		= static_cast<CEdit*>( GetDlgItem(IDC_EDIT_VALUE) );

	CString _strName,_strType,_strValue;

	pEditName->GetWindowText(_strName);
	pComboBoxType->GetWindowText(_strType);
	pEditValue->GetWindowText(_strValue);

	std::string      strName  = _strName.GetBuffer(0);
	xnd::XmlDataType eType    = xnd::XmlStream::cvtStringToType(_strType.GetBuffer(0)); 
	std::string		 strValue = _strValue.GetBuffer(0);

	// 检查结点是否相同
	// ---------------------------------------------------------------------------------------------
	xnd::XmlAttribute* pAttr = m_pCurrentNode->getAttribute( m_strAttributeName.c_str() );

	if( strName == pAttr->getName() && eType == pAttr->getType() )
	{
		if( eType == xnd::XML_BINARY )
		{
			int   nLen = pAttr->getLength();
			char* pBuf = new char[nLen];
			pAttr->getValueAsBinary(pBuf,nLen);
			
			std::string strOldValue = xnd::XmlStream::cvtBinaryToHexString(pBuf,nLen);

			delete[] pBuf;

			// 名称，类型，数据都相同，那么返回
			if( strOldValue == strValue )
			{
				OnOK();
				return;
			}
		}
		else
		{
			std::string strOldValue = pAttr->getValueAsString();

			// 名称，类型，数据都相同，那么返回
			if( strOldValue == strValue )
			{
				OnOK();
				return;
			}
		}
	}

	// 检查修改后的属性是否与原来的属性重名，如果不重名，是否与其他属性重名
	// ---------------------------------------------------------------------------------------------
	if( strName != pAttr->getName() )
	{
		xnd::XmlAttribute * pCheckAttr = m_pCurrentNode->getAttribute( strName.c_str() );
		if( pCheckAttr )
		{
			MessageBox("当前修改的名称与其他属性名称重复，请检查！","属性名称重复",MB_OK|MB_ICONWARNING);
			return;
		}
	}


	// 设置数据到结点
	// ---------------------------------------------------------------------------------------------
	m_pCurrentNode->destroyAttribute( m_strAttributeName.c_str() );

	if( eType == xnd::XML_BINARY )
	{
		int nLen = (int)strValue.length()/2;
		char*pBuf = new char[nLen];
        xnd::XmlStream::cvtHexStringToBinary(strValue.c_str(),pBuf,nLen);

		m_pCurrentNode->addAttribute( xnd::XmlAttribute( strName.c_str(), pBuf, nLen ) );

		delete[] pBuf;
	}
	else
	{
		xnd::XmlAttribute xaAttr( eType, strName.c_str() );
		m_pCurrentNode->addAttribute( xaAttr );
		m_pCurrentNode->setAttributeAsString( strName.c_str(), strValue.c_str() );
	}

	OnOK();
}

BOOL CAttributeEditDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  在此添加额外的初始化
	
	// 相关数据预先获取
	// ---------------------------------------------------------------------------------------------
	CEdit*		pEditName		= static_cast<CEdit*>( GetDlgItem(IDC_EDIT_NAME) );
	CComboBox*	pComboBoxType	= static_cast<CComboBox*>( GetDlgItem(IDC_COMBO_TYPE) );
	CEdit*		pEditValue		= static_cast<CEdit*>( GetDlgItem(IDC_EDIT_VALUE) );

	// 加入属性类型
	// ---------------------------------------------------------------------------------------------
	pComboBoxType->AddString( xnd::XmlStream::cvtTypeToString( xnd::XML_CHAR   ).c_str() );
	pComboBoxType->AddString( xnd::XmlStream::cvtTypeToString( xnd::XML_INT    ).c_str() );
	pComboBoxType->AddString( xnd::XmlStream::cvtTypeToString( xnd::XML_FLOAT  ).c_str() );
	pComboBoxType->AddString( xnd::XmlStream::cvtTypeToString( xnd::XML_STRING ).c_str() );
	pComboBoxType->AddString( xnd::XmlStream::cvtTypeToString( xnd::XML_BINARY ).c_str() );
	

	// 取得属性相关数据
	// ---------------------------------------------------------------------------------------------
	xnd::XmlAttribute* pAttribute = 
		m_pCurrentNode->getAttribute( m_strAttributeName.c_str() );

	std::string strName  = pAttribute->getName();
	std::string strType  = xnd::XmlStream::cvtTypeToString( pAttribute->getType() );
	std::string strValue = pAttribute->getValueAsString();

	// 二进制数据的特殊处理
	// ---------------------------------------------------------------------------------------------
	if( strType == xnd::XmlStream::cvtTypeToString(xnd::XML_BINARY) )
	{
		int nLen = pAttribute->getLength();
		if( nLen != 0 )
		{
			nLen = nLen * 2 + 1;

			char *pBuf = new char[ nLen ];
			memset( pBuf, 0, nLen );

			nLen = pAttribute->getValueAsBinary( pBuf, nLen );
			if( nLen > 0 )
			{
				strValue = xnd::XmlStream::cvtBinaryToHexString(pBuf,nLen );
			}

			delete[] pBuf;
		}

		// 禁用相关控件
		pEditValue->SetReadOnly(TRUE);
	}

	// 设置相关数据到界面
	// ---------------------------------------------------------------------------------------------
	pEditName->SetWindowText( strName.c_str() );
	pComboBoxType->SetCurSel(  pComboBoxType->FindString( 0, strType.c_str() ) );
	pEditValue->SetWindowText( strValue.c_str() );

	// 导入导出按钮状态处理
	// ---------------------------------------------------------------------------------------------
	if( strType == xnd::XmlStream::cvtTypeToString( xnd::XML_BINARY) ||
		strType == xnd::XmlStream::cvtTypeToString( xnd::XML_STRING) )
	{
		CButton* pExport = static_cast<CButton*>( GetDlgItem( IDC_BUTTON_EXPORT) );
		CButton* pImport = static_cast<CButton*>( GetDlgItem( IDC_BUTTON_IMPORT) );

		pExport->EnableWindow( TRUE );
		pImport->EnableWindow( TRUE );
	}
	else
	{
		CButton* pExport = static_cast<CButton*>( GetDlgItem( IDC_BUTTON_EXPORT) );
		CButton* pImport = static_cast<CButton*>( GetDlgItem( IDC_BUTTON_IMPORT) );

		pExport->EnableWindow( FALSE );
		pImport->EnableWindow( FALSE );
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

void CAttributeEditDlg::OnCbnSelchangeComboType()
{
	// TODO: 在此添加控件通知处理程序代码

	// 注：属性的类型是不允许再改动的，所以在这里，不允许改动属性类型
}

// 导入
void CAttributeEditDlg::OnBnClickedButtonImport()
{
	// TODO: 在此添加控件通知处理程序代码
	char strFilter[] = "All Files (*.*)|*.*||";
	CFileDialog fd( TRUE,NULL,NULL,OFN_FILEMUSTEXIST| OFN_HIDEREADONLY,strFilter,this);

	if(  IDOK == fd.DoModal() )
	{
		FILE* fp = fopen(fd.GetFileName().GetBuffer(0),"rb");
		if( fp )
		{
			fseek(fp,0,SEEK_END);
			size_t nLen = ftell(fp);
			fseek(fp,0,SEEK_SET);

			char* pBuf = new char[nLen+1];
			fread( pBuf, nLen ,1,fp);
			pBuf[nLen] = 0;

			fclose(fp);
			fp = NULL;

			// 设置相关数据
			xnd::XmlAttribute*	pAttr		= m_pCurrentNode->getAttribute( m_strAttributeName.c_str() );
			CEdit*				pEditValue	= static_cast<CEdit*>( GetDlgItem(IDC_EDIT_VALUE) );

			// 是否是Binary
			if( xnd::XML_BINARY == pAttr->getType())
			{
				std::string strValue = xnd::XmlStream::cvtBinaryToHexString( pBuf, static_cast<const int&>(nLen) );
				pEditValue->SetWindowText( strValue.c_str() );
			}
			else
			{
				xnd::XmlAttribute attr( pAttr->getType(),"temp");
				attr.setValueByString( pBuf );

				std::string strValue = attr.getValueAsString();
				pEditValue->SetWindowText( strValue.c_str() );
			}

			delete[] pBuf;

			MessageBox("从文件导入数据成功！","导入");
		}

	}

}

// 导出
void CAttributeEditDlg::OnBnClickedButtonExport()
{
	// TODO: 在此添加控件通知处理程序代码
	char strFilter[] = "All Files (*.*)|*.*||";
	CFileDialog fd( FALSE,NULL,NULL,OFN_HIDEREADONLY,strFilter,this);

	if(  IDOK == fd.DoModal() )
	{
		FILE* fp = fopen(fd.GetFileName().GetBuffer(0),"wb");
		if( fp )
		{
			xnd::XmlAttribute*	pAttr		= m_pCurrentNode->getAttribute( m_strAttributeName.c_str() );
			CEdit*				pEditValue	= static_cast<CEdit*>( GetDlgItem(IDC_EDIT_VALUE) );

			CString	strValue;
			pEditValue->GetWindowText( strValue );

			if( xnd::XML_BINARY == pAttr->getType() )
			{
				int  nLen = strValue.GetLength()/2;
				char*pBuf = new char[ nLen ];

				xnd::XmlStream::cvtHexStringToBinary( strValue.GetBuffer(0), pBuf, nLen );
				fwrite(pBuf,nLen,1,fp);

				delete[] pBuf;
			}
			else
			{
				fwrite(strValue.GetBuffer(0),strValue.GetLength(),1,fp);
			}

			fclose(fp);
			MessageBox("导出数据到文件成功！","导出");
		}
	}
}

// 应用按钮
void CAttributeEditDlg::OnBnClickedButtonApply()
{
	// TODO: 在此添加控件通知处理程序代码
	CEdit*		pEditName		= static_cast<CEdit*>( GetDlgItem(IDC_EDIT_NAME) );
	CComboBox*	pComboBoxType	= static_cast<CComboBox*>( GetDlgItem(IDC_COMBO_TYPE) );
	CEdit*		pEditValue		= static_cast<CEdit*>( GetDlgItem(IDC_EDIT_VALUE) );

	CString _strName,_strType,_strValue;

	pEditName->GetWindowText(_strName);
	pComboBoxType->GetWindowText(_strType);
	pEditValue->GetWindowText(_strValue);

	std::string      strName  = _strName.GetBuffer(0);
	xnd::XmlDataType eType    = xnd::XmlStream::cvtStringToType(_strType.GetBuffer(0)); 
	std::string		 strValue = _strValue.GetBuffer(0);

	// 检查结点是否相同
	// ---------------------------------------------------------------------------------------------
	xnd::XmlAttribute* pAttr = m_pCurrentNode->getAttribute( m_strAttributeName.c_str() );

	if( strName == pAttr->getName() && eType == pAttr->getType() )
	{
		if( eType == xnd::XML_BINARY )
		{
			int   nLen = pAttr->getLength();
			char* pBuf = new char[nLen];
			pAttr->getValueAsBinary(pBuf,nLen);

			std::string strOldValue = xnd::XmlStream::cvtBinaryToHexString(pBuf,nLen);

			delete[] pBuf;

			// 名称，类型，数据都相同，那么返回
			if( strOldValue == strValue )
			{
				return;
			}
		}
		else
		{
			std::string strOldValue = pAttr->getValueAsString();

			// 名称，类型，数据都相同，那么返回
			if( strOldValue == strValue )
			{
				return;
			}
		}
	}

	// 检查修改后的属性是否与原来的属性重名，如果不重名，是否与其他属性重名
	// ---------------------------------------------------------------------------------------------
	if( strName != pAttr->getName() )
	{
		xnd::XmlAttribute * pCheckAttr = m_pCurrentNode->getAttribute( strName.c_str() );
		if( pCheckAttr )
		{
			MessageBox("当前修改的名称与其他属性名称重复，请检查！","属性名称重复",MB_OK|MB_ICONWARNING);
			return;
		}
	}


	// 设置数据到结点
	// ---------------------------------------------------------------------------------------------
	m_pCurrentNode->destroyAttribute( m_strAttributeName.c_str() );

	if( eType == xnd::XML_BINARY )
	{
		int nLen = (int)strValue.length()/2;
		char*pBuf = new char[nLen];
		xnd::XmlStream::cvtHexStringToBinary(strValue.c_str(),pBuf,nLen);

		m_pCurrentNode->addAttribute( xnd::XmlAttribute( strName.c_str(), pBuf, nLen ) );

		delete[] pBuf;
	}
	else
	{
		xnd::XmlAttribute xaAttr( eType, strName.c_str() );
		m_pCurrentNode->addAttribute( xaAttr );
		m_pCurrentNode->setAttributeAsString( strName.c_str(), strValue.c_str() );
	}

	// 更新界面
	// ---------------------------------------------------------------------------------------------
	if( eType != xnd::XML_BINARY && eType != xnd::XML_STRING )
	{
		pEditValue->SetWindowText( m_pCurrentNode ->getAttributeAsString(strName.c_str()).c_str() );
	}
}
