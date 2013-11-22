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
 *	\brief		XML文件流控制类
 *
 *	\author		陈治国
 *
 *	\version	0.7
 *
 *	\date		2006-06-12
 *	\date		2006-08-03
 *	\date		2006-09-30
 *	\date		2006-10-31
 *	\date		2006-11-15
 *
 *	\bug		暂无
 *
 *	\warning	暂无
 *
 ******************************************************************************
 */
class XmlStream
{
public:

	/*!
	 *******************************************************************************
	 *	从磁盘中载入或保存文件
	 *******************************************************************************
	 */
	static bool			loadTextXml  ( const char* xmlFile, XmlNode* out );
	static bool			loadBinaryXml( const char* xmlFile, XmlNode* out );

	static bool			saveTextXml  ( const char* xmlFile, XmlNode* in );
	static bool			saveBinaryXml( const char* xmlFile, XmlNode* in );

	/*!
	 *******************************************************************************
	 *	从代理中载入或保存文件
	 *******************************************************************************
	 */
	static bool			loadTextXml  ( XmlFileBridge* in, XmlNode* out, bool bReadLen = true );
	static bool			loadBinaryXml( XmlFileBridge* in, XmlNode* out );

	static bool			saveTextXml  ( XmlFileBridge* out, XmlNode* in, bool bSaveLen = true );
	static bool			saveBinaryXml( XmlFileBridge* out, XmlNode* in );

public:

	/*!
	*******************************************************************************
	*	数据类型到字符串的相互转换
	*******************************************************************************
	*/
	static std::string	cvtTypeToString( const XmlDataType& );
	static XmlDataType	cvtStringToType( const std::string& );

	/*!
	*******************************************************************************
	*	二进制数据到16进制字符串的相互转换
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
