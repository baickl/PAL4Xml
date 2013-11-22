#include "StdAfx.h"
#include "XmlStream.H"
#include "tinyXml\tinyxml.h"
#include <ctime>
#include <fstream>
#include <cstring>
#include <Windows.h>
//>	----------------------------------------------------------------------------

namespace xnd
{

/*>	---------------------------------------------------------------------------
	һЩ�ַ�����������
---------------------------------------------------------------------------	<*/
const char* XND_ATTR_STR = "Xnd_Attr";
const char* XND_NODE_STR = "Xnd_Node";

const char* XND_NAME_STR = "Name";
const char* XND_TYPE_STR = "Type";
const char* XND_VALU_STR = "Value";

/*>	---------------------------------------------------------------------------
	ȡ��ʱ������
---------------------------------------------------------------------------	<*/
std::string getDateTime( void )
{
	time_t t;
	tm *area;
	
	t=time(NULL);
	area=localtime(&t);
	
	std::string ret;
	
	char tmp[128] = {0};
	sprintf(tmp,"%d%02d%02d%02d%02d%02d",
		area->tm_year+1900,
		area->tm_mon+1,area->tm_mday,
		area->tm_hour ,area->tm_min ,area->tm_sec);
	
	return (ret = tmp);
}

/*>	---------------------------------------------------------------------------
	��ǰXmlNode��֧�ֵ���߰汾
---------------------------------------------------------------------------	<*/
const int XML_MAJOR = 1;
const int XML_MINOR = 1;

inline int	getByteFromHex( const char& h )
{
	if (h >= '0' && h <='9')	return h-'0';
	if (h >= 'a' && h <='f')	return h-'a' + 10;
	else						return 0;
}

inline void	getHexStrFromByte( const char& _byte, char* out )
{
	//�߰��ֽ�
	int b = (_byte & 0xf0) >> 4;

	if (b >=0  && b <= 9 )	out[0] = b+'0';
	if (b >=10 && b <= 15)	out[0] = (b-10)+'a';

	//�Ͱ��ֽ�
	b = _byte & 0x0f;

	if (b >=0  && b <= 9 )	out[1] = b+'0';
	if (b >=10 && b <= 15)	out[1] = (b-10)+'a';
}


/*!
 *******************************************************************************
 *	�����������͵��ַ�����ת��
 *******************************************************************************
 */
std::string XmlStream::cvtTypeToString(const XmlDataType& xdt)
{
	std::string typestr;

	if( xdt == XML_CHAR )			typestr = "XML_CHAR";
	else if( xdt == XML_INT )		typestr = "XML_INT";
	else if( xdt == XML_FLOAT )		typestr = "XML_FLOAT";
	else if( xdt == XML_STRING )	typestr = "XML_STRING";
	else if( xdt == XML_BINARY )	typestr = "XML_BINARY";

	return typestr;
}

/*!
 *******************************************************************************
 *	�ַ����������������͵�ת��
 *******************************************************************************
 */
XmlDataType XmlStream::cvtStringToType(const std::string& typestr )
{
	XmlDataType xdt ;

	if( typestr == "XML_CHAR")			xdt = XML_CHAR;
	else if( typestr == "XML_INT")		xdt = XML_INT;
	else if( typestr == "XML_FLOAT")	xdt = XML_FLOAT;
	else if( typestr == "XML_STRING")	xdt = XML_STRING;
	else if( typestr == "XML_BINARY")	xdt = XML_BINARY;

	return xdt;

}

/*!
 *******************************************************************************
 *	���������ݵ�ʮ�������ַ�����ת��
 *******************************************************************************
 */
std::string XmlStream::cvtBinaryToHexString( const void* data,const int& size)
{
	int   dataSize	= size;
	char* datac8	= (char*)(data);
	char  tmp[3]	= {0};
	
	std::string str ;
	if( dataSize > 0 )
		str.reserve(dataSize<<1);

	for (int b=0; b<dataSize; b++)
	{
		getHexStrFromByte(datac8[b], tmp);
		str+=tmp;
	}

	return str;
}

/*!
 *******************************************************************************
 *	ʮ�������ַ��������������ݵ�ת��
 *******************************************************************************
 */
int	XmlStream::cvtHexStringToBinary( const char* str,void* data,const int& isize)
{
	int dataSize = isize;
	char* datac8 = (char*)(data);
	int p = 0;
	const char* dataString = str;

	if( !str )
		return 0;
	
	for (int i=0; i<dataSize; i++)
		datac8[i] = 0;
	
	int readLen = 0;
	while(dataString[p] && p<dataSize)
	{
		int v = getByteFromHex(dataString[p<<1]) << 4;
		
		if (dataString[(p<<1)+1])
			v += getByteFromHex(dataString[(p<<1)+1]);
		
		datac8[p] = v;
		++p;
		++readLen;
	}

	return readLen;
}

/*!
 *******************************************************************************
 *	�Ӵ����ļ�������XML
 *******************************************************************************
 */
bool XmlStream::loadBinaryXml(const char* xmlFile,XmlNode*out)
{
	assert(out && "����ڵ㲻��Ϊ��");
	if( !out )
		return false;

	//> ���ļ���
	//>	----------------------------------------------------------------------------
	std::fstream in;
	in.open(xmlFile,std::ios_base::in | std::ios_base::binary );
	if(in.fail()) 
	{
		assert( false && " ���ļ�ʧ�� ");
		return (XmlNode*)0;
	}

	//> ����汾
	//>	----------------------------------------------------------------------------
	int major = 0 ,minor = 0;
	in.read((char*)&major,sizeof(int));
	in.read((char*)&minor,sizeof(int));

	if( major > XML_MAJOR )	//> �汾���ߣ����ܽ���
	{
		assert( false && "�汾���߻����ļ���ʽ����ȷ");
		return false;
	}
	else
	{
		if( major == XML_MAJOR )
		{
			if( minor > XML_MINOR ) //> ͬ�����ǰ汾���ߣ����ܽ���
			{
				assert( false && "�汾���߻����ļ���ʽ����ȷ");
				return false;
			}
		}
	}

	//> ����ڵ�
	//>	----------------------------------------------------------------------------
	bool ret = loadBinaryXmlImp(0,out,&in);
	in.close();

	return ret;
}

/*!
 *******************************************************************************
 *	��������ʵ��
 *******************************************************************************
 */
bool XmlStream::loadBinaryXmlImp(XmlNode*root,XmlNode*out,std::fstream* in)
{
	//> ��ȡ�ڵ�����
	//>	----------------------------------------------------------------------------
	int  lenOfName = 0;
	char*strOfName = NULL;

	in->read((char*)&lenOfName,sizeof(int));
	if(lenOfName > 0)
	{
		strOfName = new char[lenOfName+1];
		in->read(strOfName,lenOfName);
		strOfName[lenOfName]='\0';
	}		

	//> �����ڵ�
	//>	----------------------------------------------------------------------------
	out->create(strOfName,root);
	delete[] strOfName;

	//> ��ȡ�����������
	//>	----------------------------------------------------------------------------
	int AttributeCount = 0 ;		
	in->read((char*)&AttributeCount,sizeof(int));
	
	if(AttributeCount > 0)
	{
		for(int i=0 ; i<AttributeCount ; i++)
		{
			//> ��ȡ��������
			//>	----------------------------------------------------------------------------
			XmlDataType xdt = XmlDataType(0);
			in->read((char*)&xdt,sizeof(XmlDataType));
			
			//> ��ȡ��������
			//>	----------------------------------------------------------------------------
			int  lenOfAttributeName = 0;
			char*strOfAttributeName = 0;
			
			in->read((char*)&lenOfAttributeName,sizeof(int));
			strOfAttributeName = new char[lenOfAttributeName+1];
			in->read(strOfAttributeName,lenOfAttributeName);
			strOfAttributeName[lenOfAttributeName]='\0';
			
			//> ��������
			//>	----------------------------------------------------------------------------
			XmlAttribute xa(xdt,strOfAttributeName);
			delete[] strOfAttributeName;
			
			//> ��ȡ����ֵ
			//>	----------------------------------------------------------------------------
			if( xdt == XML_CHAR )
			{
				char val = 0;
				
				in->read((char*)&val,sizeof(char));
				xa.setValueByChar(val);
			}
			else if( xdt == XML_INT ) 
			{
				int val = 0;
				
				in->read((char*)&val,sizeof(int));
				xa.setValueByInt(val);
			}
			else if( xdt == XML_FLOAT )
			{
				float val = 0;
				
				in->read((char*)&val,sizeof(float));
				xa.setValueByFloat(val);
			}
			else if( xdt == XML_STRING )
			{
				int  lenOfVal = 0;
				char*strOfVal = 0;
				
				in->read((char*)&lenOfVal,sizeof(int));
				if( lenOfVal > 0)
				{
					strOfVal = new char[lenOfVal+1];
					in->read(strOfVal,lenOfVal);
					strOfVal[lenOfVal]='\0';
					xa.setValueByString(strOfVal);
					delete[] strOfVal;
				}
				else
				{
					xa.setValueByString("\0");
				}
			}
			else if( xdt == XML_BINARY )
			{
				int  len=0;
				
				in->read((char*)&len,sizeof(int));
				if( len > 0 )
				{
					void* data = (char*)new char[len];
					in->read((char*)data,len);
					
					xa.setValueByBinary(data,len);
					delete[] (char*)data;
				}
			}
			
			out->addAttribute(xa);
		}
	}
	
	//> ��ȡ���������ӽ��
	//>	----------------------------------------------------------------------------
	int ChildNodeCount = 0;
	in->read((char*)&ChildNodeCount,sizeof(int));
	if( ChildNodeCount> 0)
	{
		for( int i=0; i<ChildNodeCount; i++)
		{
			XmlNode*nd =new XmlNode;
			loadBinaryXmlImp(out,nd,in);
		}
	}
	
	return true;
}

/*!
 *******************************************************************************
 *	�����ı�Xml
 *******************************************************************************
 */
bool XmlStream::loadTextXml(const char* xmlFile,XmlNode*out)
{
	assert( out && "�����ָ��Ϊ��");
	if( !out )
		return false;

	bool ret = false;
	TiXmlDocument txml(xmlFile);
	if( txml.LoadFile())
	{
		// ȡ��Ԫ�ؽڵ�
		TiXmlElement* elm = txml.FirstChildElement();
		ret = loadTextXmlImp(0,out,elm);
	}
	else
	{
		assert( false && "TinyXml�����ļ����������ļ������ڻ����ļ���ʽ��������" );
	}
	
	return ret;
}

/*!
 *******************************************************************************
 *	�����ı�Xml��ʵ��
 *******************************************************************************
 */
bool  XmlStream::loadTextXmlImp(XmlNode* root ,XmlNode*out,void* ptr)
{
	bool ret = false;

	TiXmlElement* elm = static_cast<TiXmlElement*>(ptr);

	//> �������
	//>	----------------------------------------------------------------------------
	std::string nnme = elm->Attribute(XND_NAME_STR);
	out->create(nnme.c_str(),root);
	
	//> ��ȡ��������
	//>	----------------------------------------------------------------------------
	TiXmlElement* txe = elm->FirstChildElement(XND_ATTR_STR);
	while( txe )
	{
		//>	����һ�����Ե��������ݣ����ƣ����ͣ�����ֵ
		//>	----------------------------------------------------------------------------
		const char * nme = txe->Attribute(XND_NAME_STR);
		XmlDataType xdt  = cvtStringToType(txe->Attribute(XND_TYPE_STR));
		std::string val  = txe->Attribute(XND_VALU_STR);

		//> ��������
		//>	----------------------------------------------------------------------------
		XmlAttribute xa(xdt,nme);
		if( xdt == XML_BINARY )
		{
			int nLen  = static_cast<int>(val.length()>>1);
			if( nLen > 0)
			{
				void* pData = (void*)(new char[nLen]);
				cvtHexStringToBinary(val.c_str(),pData,nLen);
				xa.setValueByBinary(pData,nLen);

				delete[] (char*)pData;
			}
		}
		else
		{
			xa.setValueByString(val.c_str());
		}
		out->addAttribute(xa);

		//>	������һ���ӽ��Ķ�ȡ
		//>	----------------------------------------------------------------------------
		txe = txe->NextSiblingElement(XND_ATTR_STR);
	}

	ret = true;
	
	//> ��ȡ���е��ӽ��
	//>	----------------------------------------------------------------------------
	TiXmlElement * curelm = elm->FirstChildElement(XND_NODE_STR);
	while( curelm )
	{
		XmlNode* xnd = new XmlNode;

		if(!loadTextXmlImp(out,xnd,curelm))	
		{
			assert( false && "��ȡ�ӽڵ�����ʱ����");
			ret = false;
		}
		else
			ret = true;

		if(!ret)
			break;

		curelm = curelm->NextSiblingElement(XND_NODE_STR);
	}
	
	return ret;
}


/*!
 *******************************************************************************
 *	���ļ����������������XML
 *******************************************************************************
 */
bool XmlStream::loadBinaryXml(XmlFileBridge*fd,XmlNode*out)
{
	assert( out && fd && "�ļ������XML�������Ϊ��");

	if( !out || !fd  )
		return false;

	//> ����汾
	//>	----------------------------------------------------------------------------
	int major = 0,minor = 0;
	fd->read((void*)&major,sizeof(int));
	fd->read((void*)&minor,sizeof(int));

	if( major > XML_MAJOR )	//> �汾���ߣ����ܽ���
	{
		assert( false && "�ļ��汾���߻����ļ�����XML��ʽ" );
		return false;
	}
	else
	{
		if( major == XML_MAJOR )
		{
			if( minor > XML_MINOR ) //> ͬ�����ǰ汾���ߣ����ܽ���
			{
				assert( false && "�ļ��汾���߻����ļ�����XML��ʽ" );
				return false;
			}
		}
	}

	//> ����ڵ�
	//>	----------------------------------------------------------------------------
	return loadBinaryXmlImp(0,out,fd);
}

/*!
 *******************************************************************************
 *	���ļ����������������XML��ʵ��
 *******************************************************************************
 */
bool XmlStream::loadBinaryXmlImp(XmlNode*root,XmlNode*out,XmlFileBridge*fd)
{
	//> ��ȡ�ڵ�����
	//>	----------------------------------------------------------------------------
	int  lenOfName = 0;
	char*strOfName = NULL;

	fd->read((void*)&lenOfName,sizeof(int));
	if( lenOfName > 0 )
	{
		strOfName = new char[lenOfName+1];
		fd->read(strOfName,lenOfName);
		strOfName[lenOfName]='\0';
	}

	//> �����ڵ�
	//>	----------------------------------------------------------------------------
	out->create(strOfName,root);
	delete[] strOfName;

	//> ��ȡ�����������
	//>	----------------------------------------------------------------------------	
	int AttributeCount = 0;
	fd->read((void*)&AttributeCount,sizeof(int));
	if(AttributeCount > 0)
	{
		for(int i=0 ; i<AttributeCount ; i++)
		{
			//> ��ȡ��������
			//>	----------------------------------------------------------------------------
			XmlDataType xdt = XmlDataType(0);
			fd->read((void*)&xdt,sizeof(XmlDataType));
			
			//> ��ȡ��������
			//>	----------------------------------------------------------------------------
			int  lenOfAttributeName = 0;
			char*strOfAttributeName = 0;
			
			fd->read((void*)&lenOfAttributeName,sizeof(int));
			strOfAttributeName = new char[lenOfAttributeName+1];
			fd->read(strOfAttributeName,lenOfAttributeName);
			strOfAttributeName[lenOfAttributeName]='\0';
			
			//> ��������
			//>	----------------------------------------------------------------------------
			XmlAttribute xa(xdt,strOfAttributeName);
			delete[] strOfAttributeName;
			
			//> ��ȡ����ֵ
			//>	----------------------------------------------------------------------------
			if( xdt == XML_CHAR )
			{
				char val = '\0';
				
				fd->read((void*)&val,sizeof(char));
				xa.setValueByChar(val);
			}
			else if( xdt == XML_INT ) 
			{
				int  val = 0;
				
				fd->read((void*)&val,sizeof(int));
				xa.setValueByInt(val);
			}
			else if( xdt == XML_FLOAT )
			{
				float val = 0;
				
				fd->read((void*)&val,sizeof(float));
				xa.setValueByFloat(val);
			}
			else if( xdt == XML_STRING )
			{
				int  lenOfVal = 0;
				char*strOfVal = 0;
				
				fd->read((char*)&lenOfVal,sizeof(int));
				if( lenOfVal > 0)
				{
					strOfVal = new char[lenOfVal+1];
					fd->read(strOfVal,lenOfVal);
					strOfVal[lenOfVal]='\0';
					xa.setValueByString(strOfVal);
					delete[] strOfVal;
				}
				else
				{
					xa.setValueByString("\0");
				}
			}
			else if( xdt == XML_BINARY )
			{
				int  len = 0;
				
				fd->read(&len,sizeof(int));
				if( len > 0 )
				{
					void* data = (char*)new char[len];
					fd->read(data,len);
					
					xa.setValueByBinary(data,len);
					delete[] (char*)data;
				}
			}
			
			out->addAttribute(xa);
		}
	}
	
	//> ��ȡ���е��ӽ��
	//>	----------------------------------------------------------------------------
	int ChildNodeCount = 0;	
	fd->read((void*)&ChildNodeCount,sizeof(int));
	if( ChildNodeCount> 0)
	{
		for( int i=0; i<ChildNodeCount; i++)
		{
			XmlNode* xnd = new XmlNode;
			loadBinaryXmlImp(out,xnd,fd);
		}
	}
	
	return true;
}

/*!
 *******************************************************************************
 *	���ļ������������ı�XML
 *******************************************************************************
 */
bool XmlStream::loadTextXml(XmlFileBridge*fd,XmlNode*out,bool bReadLen /* = true */)
{
	assert( fd && out || "�ļ��Ż�������ڵ�Ϊ��:LoadTextXml");
	if( !fd || !out)
		return false;

	bool ret = false;

	//>	��Ҫ��ȡ����
	//>	----------------------------------------------------------------------------
	if( bReadLen )
	{
		//>	��Ҫ���ļ����ж�ȡ���ݴ�С�����ʾ����XmlNode��Ϊ�ļ���һС����
		//>	----------------------------------------------------------------------------
		int len = 0 ;
		if( !fd->read(&len,sizeof(len)))
		{
			assert( false && "͸���ļ��ţ���ȡXmlNode���ݵĳ���ʧ��");
			return 0;
		}

		assert( len > 0 && "FileBridge read file len error :LoadTextXml");

		char* buffer = new char[len + 1];
		fd->read(buffer,len);
		buffer[len] = '\0';

		// ����
		TiXmlDocument txml;
		txml.ClearError();
		txml.Parse(buffer);

		delete[] buffer;
		assert( !txml.Error() && "Tinyxml parse error :LoadTextXml " );

		if( !txml.Error() )
		{
			// ȡ��Ԫ�ؽڵ�
			TiXmlElement* elm = txml.FirstChildElement();
			ret = loadTextXmlImp(0,out,elm);
		}			
	}
	//>	����Ҫ��ȡ����
	//>	----------------------------------------------------------------------------
	else
	{
		//>	����Ҫ���ļ����ж�ȡ���ݴ�С�����ļ��ű���Ĵ�С��Ϊ���ݴ�С
		//>	----------------------------------------------------------------------------
		fd->setPosition(0);

		//> ȡ���ļ�����
		//>	----------------------------------------------------------------------------
		int len = fd->length();
		assert( len > 0 && "FileBridge read file len error :LoadTextXml");
		
		//> ��ȡ�����ļ�������
		//>	----------------------------------------------------------------------------
		char* buffer = new char[len + 1];
		fd->read(buffer,len);
		buffer[len] = '\0';
		
		//> ����
		//>	----------------------------------------------------------------------------
		TiXmlDocument txml;
		txml.ClearError();
		txml.Parse(buffer);

		delete[] buffer;
		
		assert( !txml.Error() && "Tinyxml parse error :LoadTextXml" );
		if( !txml.Error() )
		{
			// ȡ��Ԫ�ؽڵ�
			TiXmlElement* elm = txml.FirstChildElement();
			ret = loadTextXmlImp(0,out,elm);
		}			
	}
	
	return ret;
}


/*!
 *******************************************************************************
 *	����XML�������ļ�
 *******************************************************************************
 */
bool XmlStream::saveTextXml( const char* xmlFile,XmlNode* xn )
{
	// ����ɵİ汾
	TiXmlDocument doc;
	
	TiXmlDeclaration * decl    = new TiXmlDeclaration( "1.0", "GB2312", "" );
	TiXmlElement     * element = new TiXmlElement( XND_NODE_STR );
	element->SetAttribute(XND_NAME_STR,xn->getName());

	bool ret = saveTextXmlImp(xn,element);

	doc.InsertEndChild(*decl);
	doc.InsertEndChild(*element);
	if( !doc.SaveFile( xmlFile ) )
	{
		assert( false && "TinyXml�������ݵ������ļ�ʱ����" );
	}

	decl->Clear();
	element->Clear();

	delete decl;
	delete element;
	
	return ret;
}

/*!
 *******************************************************************************
 *	����XML�������ļ���ʵ��
 *******************************************************************************
 */
bool XmlStream::saveTextXmlImp(XmlNode*xn,void*ptr)
{
	TiXmlElement* ele = static_cast<TiXmlElement*>(ptr);

	//> ������������
	//>	----------------------------------------------------------------------------
	if(xn->getAttributeCount() > 0)
	{
		XmlAttribute* xa = 
			xn->getFirstAttribute();
	
		while( xa )
		{
			//> ȡ���������ƺ�������������
			//>	----------------------------------------------------------------------------
			std::string attname = xa->getName();
			std::string atttype = cvtTypeToString(xa->getType());
			
			//>	��������ֵ
			//>	----------------------------------------------------------------------------
			if( xa->getType() == XML_BINARY )
			{
				//>	�������Խ��
				TiXmlElement* txe = new TiXmlElement(XND_ATTR_STR);
				
				//>	������������
				txe->SetAttribute(XND_NAME_STR,attname.c_str());
				txe->SetAttribute(XND_TYPE_STR,atttype.c_str());

				//>	��ʼת�����ݳ�HexString
				int nLen  = xa->getLength();
				std::string str;
				
				if( nLen )
				{
					void* pData = (void*)(new char[nLen]);
					xa->getValueAsBinary(pData,nLen);

					str = cvtBinaryToHexString(pData,nLen);

					delete[] (char*)pData;
				}

				txe->SetAttribute(XND_VALU_STR,str.c_str());
				ele->InsertEndChild(*txe);
				
				delete txe;
			}
			else
			{
				//>	�������Խ��
				TiXmlElement* txe = new TiXmlElement(XND_ATTR_STR);
				
				//>	������������
				txe->SetAttribute(XND_NAME_STR,attname.c_str());
				txe->SetAttribute(XND_TYPE_STR,atttype.c_str());
				txe->SetAttribute(XND_VALU_STR,xa->getValueAsString().c_str());
				
				ele->InsertEndChild(*txe);
				
				delete txe;
			}

			//ȡ����һ��xmlAttribute
			xa = xn->getNextAttribute(xa);
		}
	}

	//> �������е��ӽ��
	//>	----------------------------------------------------------------------------
	if(xn->getChildNodeCount() > 0)
	{
		int ChildNodeCount = xn->getChildNodeCount();
		for( int i=0; i<ChildNodeCount; i++)
		{
			XmlNode* xcn = xn->getChildNodeByIndex(i);

			TiXmlElement * element = new TiXmlElement( XND_NODE_STR );
			element->SetAttribute(XND_NAME_STR,xcn->getName());
			
			if(!saveTextXmlImp(xcn,element))
			{
				delete element;
				assert( false && "XmlNode:�����ӽڵ�ʱ����");
				return false;
			}

			ele->InsertEndChild(*element);
			delete element;
		}
	}

	return true;
}

/*!
 *******************************************************************************
 *	����XML�������ļ�
 *******************************************************************************
 */
bool XmlStream::saveBinaryXml( const char* xmlFile,XmlNode* xn )
{
	assert( xn && "����ڵ㲻��Ϊ��");
	if( !xn ) 
		return false;

	// ���ļ�
	std::fstream out;

	out.open( xmlFile, std::ios_base::out | std::ios_base::binary | std::ios_base::trunc );

	bool ret = false ;

	if(out.good())
	{
		// д��汾
		out.write((const char*)&XML_MAJOR,sizeof(int));
		out.write((const char*)&XML_MINOR,sizeof(int));

		// ������
		ret = saveBinaryXmlImp(xn,&out);

		if( !ret )
		{
			assert(false && "�洢���ݳ���");
			out.clear();
		}

		out.close();
	}
	else
	{
		assert( false && "���ļ������洢����ʧ��");
	}

	return ret;
}

/*!
 *******************************************************************************
 *	����XML�������ļ�ʵ��
 *******************************************************************************
 */
bool XmlStream::saveBinaryXmlImp(XmlNode* xn,std::fstream* out)
{
	//> д��ڵ�����
	//>	----------------------------------------------------------------------------
	int lenOfNodeName = (int)strlen(xn->getName());
	out->write((const char*)&lenOfNodeName,sizeof(int));
	if( lenOfNodeName > 0 )
		out->write((const char*)xn->getName(),lenOfNodeName);

	//> д��ڵ�����
	//>	----------------------------------------------------------------------------
	if( xn->getAttributeCount() > 0)
	{
		//> д��ڵ���������
		//>	----------------------------------------------------------------------------
		int AttributeCount = xn->getAttributeCount();
		out->write((const char*)&AttributeCount,sizeof(int));

		//> ����д��ÿһ������
		//>	----------------------------------------------------------------------------
		XmlAttribute* ab = xn->getFirstAttribute();

		while( ab )
		{
			//> ��������
			//>	----------------------------------------------------------------------------
			XmlDataType  xdt = ab->getType();
			out->write((const char*)&xdt,sizeof(XmlDataType));

			//> ��������
			//>	----------------------------------------------------------------------------
			int          len = static_cast<int>(strlen(ab->getName()));
			const char*  nme = ab->getName();

			out->write((const char*)&len,sizeof(int));
			out->write((const char*)nme,len);

			//> ��������
			//>	----------------------------------------------------------------------------
			if(xdt == XML_CHAR)
			{
				char val = ab->getValueAsChar();
				out->write(&val,sizeof(char));
			}
			else if( xdt == XML_INT )
			{
				int val = ab->getValueAsInt();
				out->write((const char*)&val,sizeof(int));
			}
			else if( xdt == XML_FLOAT )
			{
				float val = ab->getValueAsFloat();
				out->write((const char*)&val,sizeof(float));
			}
			else if( xdt == XML_STRING )
			{
				std::string val = ab->getValueAsString();
				int len = static_cast<int>(val.length());

				out->write((const char*)&len,sizeof(int));
				if( len > 0 )
					out->write((const char*)val.c_str(),len);
			}
			else if( xdt == XML_BINARY )
			{
				int len = ab->getLength();
				
				out->write((const char*)&len,sizeof(int));
				if( len > 0 )
				{
					void* data = (void*)(new char[len]);
					ab->getValueAsBinary(data,len);

					out->write((const char*)data,len);
					delete[] (char*)data;
				}
			}

			ab = xn->getNextAttribute(ab);
		}
	}
	else
	{
		int AttributeCount = 0;
		out->write((const char*)&AttributeCount,sizeof(int));
	}

	//> д�����ӽ��
	//>	----------------------------------------------------------------------------
	if(xn->getChildNodeCount() > 0)
	{
		//> д���ӽ������
		//>	----------------------------------------------------------------------------
		int ChildNodeCount = xn->getChildNodeCount();
		out->write((const char*)&ChildNodeCount,sizeof(int));

		//> ���α���ÿһ���ӽ��
		//>	----------------------------------------------------------------------------
		for( int i=0; i<ChildNodeCount; i++ )
		{
			XmlNode* xcn = xn->getChildNodeByIndex(i);
			bool ret = saveBinaryXmlImp(xcn,out);

			if( !ret )
				return false;
		}
	}
	else
	{
		int ChildNodeCount = 0;
		out->write((const char*)&ChildNodeCount,sizeof(int));
	}

	return true;
}

/*!
 *******************************************************************************
 *	����XML���ļ�����
 *******************************************************************************
 */
bool XmlStream::saveTextXml( XmlFileBridge*fd, XmlNode* xn, bool bSaveLen /* = true */)
{
	assert( xn && "�������XmlNode����ָ�벻��Ϊ��ָ��");
	if( !xn )
		return false;

	//> �ȱ����TinyXML��ʶ������ݸ�ʽ
	//>	----------------------------------------------------------------------------
	TiXmlDocument      doc;	
	TiXmlDeclaration * decl    = new TiXmlDeclaration( "1.0", "GB2312", "" );
	TiXmlElement     * element = new TiXmlElement( XND_NODE_STR );

	element->SetAttribute(XND_NAME_STR,xn->getName());
	bool ret = saveTextXmlImp(xn,element);

	assert( ret && "����TinyXml����ʱ����");

	//>	����ɹ�����������Ҫ��TinyXML��ɵ����ݶ���������д�ص��ļ�������
	//>	----------------------------------------------------------------------------
	if( ret )
	{
		ret = false;
		doc.InsertEndChild(*decl);
		doc.InsertEndChild(*element);

		TiXmlPrinter printer;
		printer.SetIndent( "\t" );

		if( doc.Accept( &printer ) )
		{
			const char*strBuf = printer.CStr();
			int len = static_cast<int>( strlen( strBuf ) );

			if( bSaveLen )
				fd->write( &len, sizeof(len) );
			fd->write( (const void*)strBuf, len );
		}
		else
		{
			assert( false && "TinyXML�������ݵ������ļ�ʱ����");
			return false;
		}
		
		//> ���ݵ�ǰ�ڵ����ƺ͵�ǰʱ�䣬������ʱ�ļ������ڱ���Xml������
		//>	----------------------------------------------------------------------------
		//std::string xmlFile = xn->getName();
		//xmlFile += "_";
		//xmlFile += getDateTime();
		//xmlFile += ".xml";

		//if( !doc.SaveFile(xmlFile.c_str()) )
		//{
		//	decl->Clear();
		//	element->Clear();

		//	delete decl;
		//	delete element;

		//	assert( false && "TinyXML�������ݵ������ļ�ʱ����");
		//	return false;
		//}
		
		//> ����ʱ�ļ����ļ����ݣ�Ȼ����д�ص��ļ������У�����ļ�����Ĳ���
		//>	----------------------------------------------------------------------------
		//std::fstream fs;
		//fs.open(xmlFile.c_str(),std::ios_base::binary|std::ios_base::in);
		//if(fs.good())
		//{
		//	int len ;

		//	//> ȡ���ļ�����
		//	//>	----------------------------------------------------------------------------
		//	fs.seekg(0,std::ios_base::end);
		//	len = fs.tellg();
		//	fs.seekg(0,std::ios_base::beg);
		//	
		//	//> ��ȡ�ļ�����
		//	//>	----------------------------------------------------------------------------
		//	char* tmp = new char[len+1];
		//	fs.read(tmp,len);
		//	tmp[len]=0;

		//	//>	�ر��ļ�
		//	//>	----------------------------------------------------------------------------
		//	fs.close();

		//	//>	������������д���ļ�������
		//	//>	----------------------------------------------------------------------------
		//	if( bSaveLen )
		//		fd->write(&len,sizeof(len));
		//	fd->write((const void*)tmp,len);

		//	//>	ɾ����ʱ����
		//	//>	----------------------------------------------------------------------------
		//	delete[] tmp;
		//	ret = true;
		//}

		////> ɾ����ʱ�ļ�
		////>	----------------------------------------------------------------------------
		//DeleteFile(xmlFile.c_str());

	}

	//>	�������
	//>	----------------------------------------------------------------------------
	decl->Clear();
	element->Clear();

	delete decl;
	delete element;
	
	return ret;
}

/*!
 *******************************************************************************
 *	���������XML���ļ�����
 *******************************************************************************
 */
bool XmlStream::saveBinaryXml( XmlFileBridge*fd, XmlNode* xn )
{
	assert( xn && "�������XmlNode���ݲ���Ϊ��");
	if( !xn )
		return false;
	
	bool ret = false;

	// ���ļ�
	if( fd )
	{
		// д��汾
		if( !fd->write((const char*)&XML_MAJOR,sizeof(int)) )
			ret = false;

		if( !fd->write((const char*)&XML_MINOR,sizeof(int)) )
			ret = false;

		// ������,����汾д������ɹ�
		if( !ret )
			ret = saveBinaryXmlImp(xn,fd);
	}

	return ret;
}

/*!
 *******************************************************************************
 *	���������XML���ļ������ʵ��
 *******************************************************************************
 */
bool XmlStream::saveBinaryXmlImp(XmlNode* xn,XmlFileBridge* out)
{
	//> д��ڵ�����
	//>	----------------------------------------------------------------------------
	int lenOfNodeName = (int)strlen(xn->getName());
	out->write((const void*)&lenOfNodeName,sizeof(int));
	if( lenOfNodeName > 0)
		out->write((const void*)xn->getName(),lenOfNodeName);

	//> д����������
	//>	----------------------------------------------------------------------------
	if( xn->getAttributeCount() > 0)
	{
		//> д��ڵ���������
		//>	----------------------------------------------------------------------------
		int AttributeCount = xn->getAttributeCount();
		out->write((const void*)&AttributeCount,sizeof(int));

		//> д��ÿһ������
		//>	----------------------------------------------------------------------------
		XmlAttribute* ab = xn->getFirstAttribute();
		while( ab )
		{

			//> ��������
			//>	----------------------------------------------------------------------------
			XmlDataType  xdt = ab->getType();
			out->write((const void*)&xdt,sizeof(XmlDataType));

			//> ��������
			//>	----------------------------------------------------------------------------
			int          len = static_cast<int>(strlen(ab->getName()));
			const char*  nme = ab->getName();

			out->write((const void*)&len,sizeof(int));
			out->write((const void*)nme,len);

			//> ��������
			//>	----------------------------------------------------------------------------
			if(xdt == XML_CHAR)
			{
				char val = ab->getValueAsChar();
				out->write(&val,sizeof(char));
			}
			else if( xdt == XML_INT )
			{
				int val = ab->getValueAsInt();
				out->write((const void*)&val,sizeof(int));
			}
			else if( xdt == XML_FLOAT )
			{
				float val = ab->getValueAsFloat();
				out->write((const void*)&val,sizeof(float));
			}
			else if( xdt == XML_STRING )
			{
				std::string val = ab->getValueAsString();
				int len = static_cast<int>(val.length());

				out->write((const void*)&len,sizeof(int));
				if( len > 0 )
					out->write((const void*)val.c_str(),len);
			}
			else if( xdt == XML_BINARY )
			{
				int len = ab->getLength();

				out->write((const char*)&len,sizeof(int));
				if( len > 0 )
				{
					void* data = (void*)(new char[len]);
					ab->getValueAsBinary(data,len);

					out->write((const char*)data,len);
					delete[] (char*)data;
				}
			}

			ab = xn->getNextAttribute(ab);
		}
	}
	else
	{
		int AttributeCount = 0;
		out->write((const void*)&AttributeCount,sizeof(int));
	}

	//> д�������ӽ��
	//>	----------------------------------------------------------------------------
	if(xn->getChildNodeCount() > 0)
	{
		//> д���ӽ������
		//>	----------------------------------------------------------------------------
		int ChildNodeCount = xn->getChildNodeCount();
		out->write((const void*)&ChildNodeCount,sizeof(int));

		//> ����ÿһ���ӽ��
		//>	----------------------------------------------------------------------------
		for( int i=0; i<ChildNodeCount; i++ )
		{
			XmlNode* xcn = xn->getChildNodeByIndex(i);
			if( !saveBinaryXmlImp(xcn,out) )
			{
				assert(false && "XmlNode�ڱ����ӽڵ�����ʱ����");
				return false;
			}
		}
	}
	else
	{
		int ChildNodeCount = 0;
		out->write((const void*)&ChildNodeCount,sizeof(int));
	}

	return true;
}

}///>namespace
