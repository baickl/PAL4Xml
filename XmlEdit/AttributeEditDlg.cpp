// AttributeEditDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "XmlEdit.h"
#include "AttributeEditDlg.h"
#include ".\attributeeditdlg.h"


// CAttributeEditDlg �Ի���

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


// ��������

void CAttributeEditDlg::SetAttribute(xnd::XmlNode* pNode, std::string strAttribute)
{
	m_pCurrentNode		= pNode;
	m_strAttributeName	= strAttribute;
}

void CAttributeEditDlg::OnBnClickedOk()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������

	// ȡ�ý����������
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

	// ������Ƿ���ͬ
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

			// ���ƣ����ͣ����ݶ���ͬ����ô����
			if( strOldValue == strValue )
			{
				OnOK();
				return;
			}
		}
		else
		{
			std::string strOldValue = pAttr->getValueAsString();

			// ���ƣ����ͣ����ݶ���ͬ����ô����
			if( strOldValue == strValue )
			{
				OnOK();
				return;
			}
		}
	}

	// ����޸ĺ�������Ƿ���ԭ��������������������������Ƿ���������������
	// ---------------------------------------------------------------------------------------------
	if( strName != pAttr->getName() )
	{
		xnd::XmlAttribute * pCheckAttr = m_pCurrentNode->getAttribute( strName.c_str() );
		if( pCheckAttr )
		{
			MessageBox("��ǰ�޸ĵ��������������������ظ������飡","���������ظ�",MB_OK|MB_ICONWARNING);
			return;
		}
	}


	// �������ݵ����
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

	// TODO:  �ڴ���Ӷ���ĳ�ʼ��
	
	// �������Ԥ�Ȼ�ȡ
	// ---------------------------------------------------------------------------------------------
	CEdit*		pEditName		= static_cast<CEdit*>( GetDlgItem(IDC_EDIT_NAME) );
	CComboBox*	pComboBoxType	= static_cast<CComboBox*>( GetDlgItem(IDC_COMBO_TYPE) );
	CEdit*		pEditValue		= static_cast<CEdit*>( GetDlgItem(IDC_EDIT_VALUE) );

	// ������������
	// ---------------------------------------------------------------------------------------------
	pComboBoxType->AddString( xnd::XmlStream::cvtTypeToString( xnd::XML_CHAR   ).c_str() );
	pComboBoxType->AddString( xnd::XmlStream::cvtTypeToString( xnd::XML_INT    ).c_str() );
	pComboBoxType->AddString( xnd::XmlStream::cvtTypeToString( xnd::XML_FLOAT  ).c_str() );
	pComboBoxType->AddString( xnd::XmlStream::cvtTypeToString( xnd::XML_STRING ).c_str() );
	pComboBoxType->AddString( xnd::XmlStream::cvtTypeToString( xnd::XML_BINARY ).c_str() );
	

	// ȡ�������������
	// ---------------------------------------------------------------------------------------------
	xnd::XmlAttribute* pAttribute = 
		m_pCurrentNode->getAttribute( m_strAttributeName.c_str() );

	std::string strName  = pAttribute->getName();
	std::string strType  = xnd::XmlStream::cvtTypeToString( pAttribute->getType() );
	std::string strValue = pAttribute->getValueAsString();

	// ���������ݵ����⴦��
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

		// ������ؿؼ�
		pEditValue->SetReadOnly(TRUE);
	}

	// ����������ݵ�����
	// ---------------------------------------------------------------------------------------------
	pEditName->SetWindowText( strName.c_str() );
	pComboBoxType->SetCurSel(  pComboBoxType->FindString( 0, strType.c_str() ) );
	pEditValue->SetWindowText( strValue.c_str() );

	// ���뵼����ť״̬����
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
	// �쳣: OCX ����ҳӦ���� FALSE
}

void CAttributeEditDlg::OnCbnSelchangeComboType()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������

	// ע�����Ե������ǲ������ٸĶ��ģ����������������Ķ���������
}

// ����
void CAttributeEditDlg::OnBnClickedButtonImport()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
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

			// �����������
			xnd::XmlAttribute*	pAttr		= m_pCurrentNode->getAttribute( m_strAttributeName.c_str() );
			CEdit*				pEditValue	= static_cast<CEdit*>( GetDlgItem(IDC_EDIT_VALUE) );

			// �Ƿ���Binary
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

			MessageBox("���ļ��������ݳɹ���","����");
		}

	}

}

// ����
void CAttributeEditDlg::OnBnClickedButtonExport()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
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
			MessageBox("�������ݵ��ļ��ɹ���","����");
		}
	}
}

// Ӧ�ð�ť
void CAttributeEditDlg::OnBnClickedButtonApply()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
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

	// ������Ƿ���ͬ
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

			// ���ƣ����ͣ����ݶ���ͬ����ô����
			if( strOldValue == strValue )
			{
				return;
			}
		}
		else
		{
			std::string strOldValue = pAttr->getValueAsString();

			// ���ƣ����ͣ����ݶ���ͬ����ô����
			if( strOldValue == strValue )
			{
				return;
			}
		}
	}

	// ����޸ĺ�������Ƿ���ԭ��������������������������Ƿ���������������
	// ---------------------------------------------------------------------------------------------
	if( strName != pAttr->getName() )
	{
		xnd::XmlAttribute * pCheckAttr = m_pCurrentNode->getAttribute( strName.c_str() );
		if( pCheckAttr )
		{
			MessageBox("��ǰ�޸ĵ��������������������ظ������飡","���������ظ�",MB_OK|MB_ICONWARNING);
			return;
		}
	}


	// �������ݵ����
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

	// ���½���
	// ---------------------------------------------------------------------------------------------
	if( eType != xnd::XML_BINARY && eType != xnd::XML_STRING )
	{
		pEditValue->SetWindowText( m_pCurrentNode ->getAttributeAsString(strName.c_str()).c_str() );
	}
}
