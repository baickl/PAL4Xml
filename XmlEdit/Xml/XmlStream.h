#ifndef _XML_STREAM_H_
#define _XML_STREAM_H_

#include "XmlNode.H"
#include "XmlFileBridge.h"
//>	----------------------------------------------------------------------------


namespace xnd
{

/*!
 *******************************************************************************
 *	
 *******************************************************************************
 *
 *	\brief		XML�ļ���������
 *
 *	\author		���ι�
 *
 *	\version	0.7
 *
 *	\date		2006-06-12
 *	\date		2006-08-03
 *	\date		2006-09-30
 *	\date		2006-10-31
 *	\date		2006-11-15
 *
 *	\bug		����
 *
 *	\warning	����
 *
 ******************************************************************************
 */
class XmlStream
{
public:

	/*!
	 *******************************************************************************
	 *	�Ӵ���������򱣴��ļ�
	 *******************************************************************************
	 */
	static bool			loadTextXml  ( const char* xmlFile, XmlNode* out );
	static bool			loadBinaryXml( const char* xmlFile, XmlNode* out );

	static bool			saveTextXml  ( const char* xmlFile, XmlNode* in );
	static bool			saveBinaryXml( const char* xmlFile, XmlNode* in );

	/*!
	 *******************************************************************************
	 *	�Ӵ���������򱣴��ļ�
	 *******************************************************************************
	 */
	static bool			loadTextXml  ( XmlFileBridge* in, XmlNode* out, bool bReadLen = true );
	static bool			loadBinaryXml( XmlFileBridge* in, XmlNode* out );

	static bool			saveTextXml  ( XmlFileBridge* out, XmlNode* in, bool bSaveLen = true );
	static bool			saveBinaryXml( XmlFileBridge* out, XmlNode* in );

public:

	/*!
	*******************************************************************************
	*	�������͵��ַ������໥ת��
	*******************************************************************************
	*/
	static std::string	cvtTypeToString( const XmlDataType& );
	static XmlDataType	cvtStringToType( const std::string& );

	/*!
	*******************************************************************************
	*	���������ݵ�16�����ַ������໥ת��
	*******************************************************************************
	*/
	static std::string	cvtBinaryToHexString( const void* data, const int& size );
	static int 			cvtHexStringToBinary( const char* str,  void* data,const int& isize );

private:

	/*!
	 *******************************************************************************
	 *	TextXml
	 *******************************************************************************
	 */
	static bool			loadTextXmlImp( XmlNode* root, XmlNode* out, void*ds );
	static bool			saveTextXmlImp( XmlNode* in,   void* ds);

	/*!
	 *******************************************************************************
	 *	BinaryXml
	 *******************************************************************************
	 */
	static bool			loadBinaryXmlImp( XmlNode* root, XmlNode* out, std::fstream * in );
	static bool			loadBinaryXmlImp( XmlNode* root, XmlNode* out, XmlFileBridge* in );

	/*!
	 *******************************************************************************
	 *	BinaryXML
	 *******************************************************************************
	 */
	static bool			saveBinaryXmlImp( XmlNode* in, std::fstream * out );
	static bool			saveBinaryXmlImp( XmlNode* in, XmlFileBridge* out );
	
};

}//namespace

#endif//_XML_STREAM_H_
