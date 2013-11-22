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
	一些字符串属性数据
---------------------------------------------------------------------------	<*/
const char* XND_ATTR_STR = "Xnd_Attr";
const char* XND_NODE_STR = "Xnd_Node";

const char* XND_NAME_STR = "Name";
const char* XND_TYPE_STR = "Type";
const char* XND_VALU_STR = "Value";

/*>	---------------------------------------------------------------------------
	取得时间日期
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
	当前XmlNode所支持的最高版本
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
	//高半字节
	int b = (_byte & 0xf0) >> 4;

	if (b >=0  && b <= 9 )	out[0] = b+'0';
	if (b >=10 && b <= 15)	out[0] = (b-10)+'a';

	//低半字节
	b = _byte & 0x0f;

	if (b >=0  && b <= 9 )	out[1] = b+'0';
	if (b >=10 && b <= 15)	out[1] = (b-10)+'a';
}


/*!
 *******************************************************************************
 *	属性数据类型到字符串的转换
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
 *	字符串到属性数据类型的转换
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
 *	二进制数据到十六进制字符串的转换
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
 *	十六进制字符串到二进制数据的转换
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
 *	从磁盘文件中载入XML
 *******************************************************************************
 */
bool XmlStream::loadBinaryXml(const char* xmlFile,XmlNode*out)
{
	assert(out && "输出节点不得为空");
	if( !out )
		return false;

	//> 打开文件先
	//>	----------------------------------------------------------------------------
	std::fstream in;
	in.open(xmlFile,std::ios_base::in | std::ios_base::binary );
	if(in.fail()) 
	{
		assert( false && " 打开文件失败 ");
		return (XmlNode*)0;
	}

	//> 检验版本
	//>	----------------------------------------------------------------------------
	int major = 0 ,minor = 0;
	in.read((char*)&major,sizeof(int));
	in.read((char*)&minor,sizeof(int));

	if( major > XML_MAJOR )	//> 版本过高，不能解析
	{
		assert( false && "版本过高或是文件格式不正确");
		return false;
	}
	else
	{
		if( major == XML_MAJOR )
		{
			if( minor > XML_MINOR ) //> 同样还是版本过高，不能解析
			{
				assert( false && "版本过高或是文件格式不正确");
				return false;
			}
		}
	}

	//> 载入节点
	//>	----------------------------------------------------------------------------
	bool ret = loadBinaryXmlImp(0,out,&in);
	in.close();

	return ret;
}

/*!
 *******************************************************************************
 *	载入数据实现
 *******************************************************************************
 */
bool XmlStream::loadBinaryXmlImp(XmlNode*root,XmlNode*out,std::fstream* in)
{
	//> 读取节点名称
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

	//> 创建节点
	//>	----------------------------------------------------------------------------
	out->create(strOfName,root);
	delete[] strOfName;

	//> 读取结点所有属性
	//>	----------------------------------------------------------------------------
	int AttributeCount = 0 ;		
	in->read((char*)&AttributeCount,sizeof(int));
	
	if(AttributeCount > 0)
	{
		for(int i=0 ; i<AttributeCount ; i++)
		{
			//> 读取属性类型
			//>	----------------------------------------------------------------------------
			XmlDataType xdt = XmlDataType(0);
			in->read((char*)&xdt,sizeof(XmlDataType));
			
			//> 读取属性名称
			//>	----------------------------------------------------------------------------
			int  lenOfAttributeName = 0;
			char*strOfAttributeName = 0;
			
			in->read((char*)&lenOfAttributeName,sizeof(int));
			strOfAttributeName = new char[lenOfAttributeName+1];
			in->read(strOfAttributeName,lenOfAttributeName);
			strOfAttributeName[lenOfAttributeName]='\0';
			
			//> 创建属性
			//>	----------------------------------------------------------------------------
			XmlAttribute xa(xdt,strOfAttributeName);
			delete[] strOfAttributeName;
			
			//> 读取属性值
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
	
	//> 读取结点的所有子结点
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
 *	载入文本Xml
 *******************************************************************************
 */
bool XmlStream::loadTextXml(const char* xmlFile,XmlNode*out)
{
	assert( out && "待输出指针为空");
	if( !out )
		return false;

	bool ret = false;
	TiXmlDocument txml(xmlFile);
	if( txml.LoadFile())
	{
		// 取得元素节点
		TiXmlElement* elm = txml.FirstChildElement();
		ret = loadTextXmlImp(0,out,elm);
	}
	else
	{
		assert( false && "TinyXml载入文件出错，可能文件不存在或是文件格式不正常。" );
	}
	
	return ret;
}

/*!
 *******************************************************************************
 *	载入文本Xml的实现
 *******************************************************************************
 */
bool  XmlStream::loadTextXmlImp(XmlNode* root ,XmlNode*out,void* ptr)
{
	bool ret = false;

	TiXmlElement* elm = static_cast<TiXmlElement*>(ptr);

	//> 创建结点
	//>	----------------------------------------------------------------------------
	std::string nnme = elm->Attribute(XND_NAME_STR);
	out->create(nnme.c_str(),root);
	
	//> 读取所有属性
	//>	----------------------------------------------------------------------------
	TiXmlElement* txe = elm->FirstChildElement(XND_ATTR_STR);
	while( txe )
	{
		//>	解析一个属性的三个数据：名称，类型，属性值
		//>	----------------------------------------------------------------------------
		const char * nme = txe->Attribute(XND_NAME_STR);
		XmlDataType xdt  = cvtStringToType(txe->Attribute(XND_TYPE_STR));
		std::string val  = txe->Attribute(XND_VALU_STR);

		//> 创建属性
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

		//>	继续下一个子结点的读取
		//>	----------------------------------------------------------------------------
		txe = txe->NextSiblingElement(XND_ATTR_STR);
	}

	ret = true;
	
	//> 读取所有的子结点
	//>	----------------------------------------------------------------------------
	TiXmlElement * curelm = elm->FirstChildElement(XND_NODE_STR);
	while( curelm )
	{
		XmlNode* xnd = new XmlNode;

		if(!loadTextXmlImp(out,xnd,curelm))	
		{
			assert( false && "读取子节点数据时错误");
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
 *	从文件代理中载入二进制XML
 *******************************************************************************
 */
bool XmlStream::loadBinaryXml(XmlFileBridge*fd,XmlNode*out)
{
	assert( out && fd && "文件代理或XML输出不得为空");

	if( !out || !fd  )
		return false;

	//> 检验版本
	//>	----------------------------------------------------------------------------
	int major = 0,minor = 0;
	fd->read((void*)&major,sizeof(int));
	fd->read((void*)&minor,sizeof(int));

	if( major > XML_MAJOR )	//> 版本过高，不能解析
	{
		assert( false && "文件版本过高或是文件并非XML格式" );
		return false;
	}
	else
	{
		if( major == XML_MAJOR )
		{
			if( minor > XML_MINOR ) //> 同样还是版本过高，不能解析
			{
				assert( false && "文件版本过高或是文件并非XML格式" );
				return false;
			}
		}
	}

	//> 载入节点
	//>	----------------------------------------------------------------------------
	return loadBinaryXmlImp(0,out,fd);
}

/*!
 *******************************************************************************
 *	从文件代理中载入二进制XML的实现
 *******************************************************************************
 */
bool XmlStream::loadBinaryXmlImp(XmlNode*root,XmlNode*out,XmlFileBridge*fd)
{
	//> 读取节点名称
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

	//> 创建节点
	//>	----------------------------------------------------------------------------
	out->create(strOfName,root);
	delete[] strOfName;

	//> 读取结点所有属性
	//>	----------------------------------------------------------------------------	
	int AttributeCount = 0;
	fd->read((void*)&AttributeCount,sizeof(int));
	if(AttributeCount > 0)
	{
		for(int i=0 ; i<AttributeCount ; i++)
		{
			//> 读取属性类型
			//>	----------------------------------------------------------------------------
			XmlDataType xdt = XmlDataType(0);
			fd->read((void*)&xdt,sizeof(XmlDataType));
			
			//> 读取属性名称
			//>	----------------------------------------------------------------------------
			int  lenOfAttributeName = 0;
			char*strOfAttributeName = 0;
			
			fd->read((void*)&lenOfAttributeName,sizeof(int));
			strOfAttributeName = new char[lenOfAttributeName+1];
			fd->read(strOfAttributeName,lenOfAttributeName);
			strOfAttributeName[lenOfAttributeName]='\0';
			
			//> 创建属性
			//>	----------------------------------------------------------------------------
			XmlAttribute xa(xdt,strOfAttributeName);
			delete[] strOfAttributeName;
			
			//> 读取属性值
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
	
	//> 读取所有的子结点
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
 *	从文件代理中载入文本XML
 *******************************************************************************
 */
bool XmlStream::loadTextXml(XmlFileBridge*fd,XmlNode*out,bool bReadLen /* = true */)
{
	assert( fd && out || "文件桥或是输出节点为空:LoadTextXml");
	if( !fd || !out)
		return false;

	bool ret = false;

	//>	需要读取长度
	//>	----------------------------------------------------------------------------
	if( bReadLen )
	{
		//>	需要从文件桥中读取数据大小，则表示，此XmlNode仅为文件的一小部分
		//>	----------------------------------------------------------------------------
		int len = 0 ;
		if( !fd->read(&len,sizeof(len)))
		{
			assert( false && "透过文件桥，读取XmlNode数据的长度失败");
			return 0;
		}

		assert( len > 0 && "FileBridge read file len error :LoadTextXml");

		char* buffer = new char[len + 1];
		fd->read(buffer,len);
		buffer[len] = '\0';

		// 解析
		TiXmlDocument txml;
		txml.ClearError();
		txml.Parse(buffer);

		delete[] buffer;
		assert( !txml.Error() && "Tinyxml parse error :LoadTextXml " );

		if( !txml.Error() )
		{
			// 取得元素节点
			TiXmlElement* elm = txml.FirstChildElement();
			ret = loadTextXmlImp(0,out,elm);
		}			
	}
	//>	不需要读取长度
	//>	----------------------------------------------------------------------------
	else
	{
		//>	不需要从文件桥中读取数据大小，那文件桥本身的大小即为数据大小
		//>	----------------------------------------------------------------------------
		fd->setPosition(0);

		//> 取得文件长度
		//>	----------------------------------------------------------------------------
		int len = fd->length();
		assert( len > 0 && "FileBridge read file len error :LoadTextXml");
		
		//> 读取整个文件的数据
		//>	----------------------------------------------------------------------------
		char* buffer = new char[len + 1];
		fd->read(buffer,len);
		buffer[len] = '\0';
		
		//> 解析
		//>	----------------------------------------------------------------------------
		TiXmlDocument txml;
		txml.ClearError();
		txml.Parse(buffer);

		delete[] buffer;
		
		assert( !txml.Error() && "Tinyxml parse error :LoadTextXml" );
		if( !txml.Error() )
		{
			// 取得元素节点
			TiXmlElement* elm = txml.FirstChildElement();
			ret = loadTextXmlImp(0,out,elm);
		}			
	}
	
	return ret;
}


/*!
 *******************************************************************************
 *	保存XML到磁盘文件
 *******************************************************************************
 */
bool XmlStream::saveTextXml( const char* xmlFile,XmlNode* xn )
{
	// 保存成的版本
	TiXmlDocument doc;
	
	TiXmlDeclaration * decl    = new TiXmlDeclaration( "1.0", "GB2312", "" );
	TiXmlElement     * element = new TiXmlElement( XND_NODE_STR );
	element->SetAttribute(XND_NAME_STR,xn->getName());

	bool ret = saveTextXmlImp(xn,element);

	doc.InsertEndChild(*decl);
	doc.InsertEndChild(*element);
	if( !doc.SaveFile( xmlFile ) )
	{
		assert( false && "TinyXml保存数据到磁盘文件时错误。" );
	}

	decl->Clear();
	element->Clear();

	delete decl;
	delete element;
	
	return ret;
}

/*!
 *******************************************************************************
 *	保存XML到磁盘文件的实现
 *******************************************************************************
 */
bool XmlStream::saveTextXmlImp(XmlNode*xn,void*ptr)
{
	TiXmlElement* ele = static_cast<TiXmlElement*>(ptr);

	//> 保存所有属性
	//>	----------------------------------------------------------------------------
	if(xn->getAttributeCount() > 0)
	{
		XmlAttribute* xa = 
			xn->getFirstAttribute();
	
		while( xa )
		{
			//> 取得属性名称和属性数据类型
			//>	----------------------------------------------------------------------------
			std::string attname = xa->getName();
			std::string atttype = cvtTypeToString(xa->getType());
			
			//>	保存属性值
			//>	----------------------------------------------------------------------------
			if( xa->getType() == XML_BINARY )
			{
				//>	创建属性结点
				TiXmlElement* txe = new TiXmlElement(XND_ATTR_STR);
				
				//>	保存属性数据
				txe->SetAttribute(XND_NAME_STR,attname.c_str());
				txe->SetAttribute(XND_TYPE_STR,atttype.c_str());

				//>	开始转换数据成HexString
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
				//>	创建属性结点
				TiXmlElement* txe = new TiXmlElement(XND_ATTR_STR);
				
				//>	保存属性数据
				txe->SetAttribute(XND_NAME_STR,attname.c_str());
				txe->SetAttribute(XND_TYPE_STR,atttype.c_str());
				txe->SetAttribute(XND_VALU_STR,xa->getValueAsString().c_str());
				
				ele->InsertEndChild(*txe);
				
				delete txe;
			}

			//取得下一个xmlAttribute
			xa = xn->getNextAttribute(xa);
		}
	}

	//> 保存所有的子结点
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
				assert( false && "XmlNode:保存子节点时出错");
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
 *	保存XML到磁盘文件
 *******************************************************************************
 */
bool XmlStream::saveBinaryXml( const char* xmlFile,XmlNode* xn )
{
	assert( xn && "输入节点不能为空");
	if( !xn ) 
		return false;

	// 打开文件
	std::fstream out;

	out.open( xmlFile, std::ios_base::out | std::ios_base::binary | std::ios_base::trunc );

	bool ret = false ;

	if(out.good())
	{
		// 写入版本
		out.write((const char*)&XML_MAJOR,sizeof(int));
		out.write((const char*)&XML_MINOR,sizeof(int));

		// 保存结点
		ret = saveBinaryXmlImp(xn,&out);

		if( !ret )
		{
			assert(false && "存储数据出错");
			out.clear();
		}

		out.close();
	}
	else
	{
		assert( false && "打开文件出错，存储数据失败");
	}

	return ret;
}

/*!
 *******************************************************************************
 *	保存XML到磁盘文件实现
 *******************************************************************************
 */
bool XmlStream::saveBinaryXmlImp(XmlNode* xn,std::fstream* out)
{
	//> 写入节点名称
	//>	----------------------------------------------------------------------------
	int lenOfNodeName = (int)strlen(xn->getName());
	out->write((const char*)&lenOfNodeName,sizeof(int));
	if( lenOfNodeName > 0 )
		out->write((const char*)xn->getName(),lenOfNodeName);

	//> 写入节点属性
	//>	----------------------------------------------------------------------------
	if( xn->getAttributeCount() > 0)
	{
		//> 写入节点属性数量
		//>	----------------------------------------------------------------------------
		int AttributeCount = xn->getAttributeCount();
		out->write((const char*)&AttributeCount,sizeof(int));

		//> 依次写入每一个属性
		//>	----------------------------------------------------------------------------
		XmlAttribute* ab = xn->getFirstAttribute();

		while( ab )
		{
			//> 属性类型
			//>	----------------------------------------------------------------------------
			XmlDataType  xdt = ab->getType();
			out->write((const char*)&xdt,sizeof(XmlDataType));

			//> 属性名称
			//>	----------------------------------------------------------------------------
			int          len = static_cast<int>(strlen(ab->getName()));
			const char*  nme = ab->getName();

			out->write((const char*)&len,sizeof(int));
			out->write((const char*)nme,len);

			//> 属性数据
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

	//> 写入结点子结点
	//>	----------------------------------------------------------------------------
	if(xn->getChildNodeCount() > 0)
	{
		//> 写入子结点数量
		//>	----------------------------------------------------------------------------
		int ChildNodeCount = xn->getChildNodeCount();
		out->write((const char*)&ChildNodeCount,sizeof(int));

		//> 依次保存每一个子结点
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
 *	保存XML到文件代理
 *******************************************************************************
 */
bool XmlStream::saveTextXml( XmlFileBridge*fd, XmlNode* xn, bool bSaveLen /* = true */)
{
	assert( xn && "待保存的XmlNode数据指针不能为空指针");
	if( !xn )
		return false;

	//> 先保存成TinyXML能识别的数据格式
	//>	----------------------------------------------------------------------------
	TiXmlDocument      doc;	
	TiXmlDeclaration * decl    = new TiXmlDeclaration( "1.0", "GB2312", "" );
	TiXmlElement     * element = new TiXmlElement( XND_NODE_STR );

	element->SetAttribute(XND_NAME_STR,xn->getName());
	bool ret = saveTextXmlImp(xn,element);

	assert( ret && "生成TinyXml数据时出错。");

	//>	保存成功，接下来需要将TinyXML存成的数据读出来，再写回到文件代理中
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
			assert( false && "TinyXML保存数据到磁盘文件时出错。");
			return false;
		}
		
		//> 根据当前节点名称和当前时间，生成临时文件，用于保存Xml的数据
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

		//	assert( false && "TinyXML保存数据到磁盘文件时出错。");
		//	return false;
		//}
		
		//> 打开临时文件读文件数据，然后再写回到文件代理中，完成文件保存的操作
		//>	----------------------------------------------------------------------------
		//std::fstream fs;
		//fs.open(xmlFile.c_str(),std::ios_base::binary|std::ios_base::in);
		//if(fs.good())
		//{
		//	int len ;

		//	//> 取得文件长度
		//	//>	----------------------------------------------------------------------------
		//	fs.seekg(0,std::ios_base::end);
		//	len = fs.tellg();
		//	fs.seekg(0,std::ios_base::beg);
		//	
		//	//> 读取文件数据
		//	//>	----------------------------------------------------------------------------
		//	char* tmp = new char[len+1];
		//	fs.read(tmp,len);
		//	tmp[len]=0;

		//	//>	关闭文件
		//	//>	----------------------------------------------------------------------------
		//	fs.close();

		//	//>	将读出的数据写到文件代理中
		//	//>	----------------------------------------------------------------------------
		//	if( bSaveLen )
		//		fd->write(&len,sizeof(len));
		//	fd->write((const void*)tmp,len);

		//	//>	删除临时数据
		//	//>	----------------------------------------------------------------------------
		//	delete[] tmp;
		//	ret = true;
		//}

		////> 删除临时文件
		////>	----------------------------------------------------------------------------
		//DeleteFile(xmlFile.c_str());

	}

	//>	清除工作
	//>	----------------------------------------------------------------------------
	decl->Clear();
	element->Clear();

	delete decl;
	delete element;
	
	return ret;
}

/*!
 *******************************************************************************
 *	保存二进制XML到文件代理
 *******************************************************************************
 */
bool XmlStream::saveBinaryXml( XmlFileBridge*fd, XmlNode* xn )
{
	assert( xn && "待保存的XmlNode数据不得为空");
	if( !xn )
		return false;
	
	bool ret = false;

	// 打开文件
	if( fd )
	{
		// 写入版本
		if( !fd->write((const char*)&XML_MAJOR,sizeof(int)) )
			ret = false;

		if( !fd->write((const char*)&XML_MINOR,sizeof(int)) )
			ret = false;

		// 保存结点,必须版本写入操作成功
		if( !ret )
			ret = saveBinaryXmlImp(xn,fd);
	}

	return ret;
}

/*!
 *******************************************************************************
 *	保存二进制XML到文件代理的实现
 *******************************************************************************
 */
bool XmlStream::saveBinaryXmlImp(XmlNode* xn,XmlFileBridge* out)
{
	//> 写入节点名称
	//>	----------------------------------------------------------------------------
	int lenOfNodeName = (int)strlen(xn->getName());
	out->write((const void*)&lenOfNodeName,sizeof(int));
	if( lenOfNodeName > 0)
		out->write((const void*)xn->getName(),lenOfNodeName);

	//> 写入所有属性
	//>	----------------------------------------------------------------------------
	if( xn->getAttributeCount() > 0)
	{
		//> 写入节点属性数量
		//>	----------------------------------------------------------------------------
		int AttributeCount = xn->getAttributeCount();
		out->write((const void*)&AttributeCount,sizeof(int));

		//> 写入每一个属性
		//>	----------------------------------------------------------------------------
		XmlAttribute* ab = xn->getFirstAttribute();
		while( ab )
		{

			//> 属性类型
			//>	----------------------------------------------------------------------------
			XmlDataType  xdt = ab->getType();
			out->write((const void*)&xdt,sizeof(XmlDataType));

			//> 属性名称
			//>	----------------------------------------------------------------------------
			int          len = static_cast<int>(strlen(ab->getName()));
			const char*  nme = ab->getName();

			out->write((const void*)&len,sizeof(int));
			out->write((const void*)nme,len);

			//> 属性数据
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

	//> 写入所有子结点
	//>	----------------------------------------------------------------------------
	if(xn->getChildNodeCount() > 0)
	{
		//> 写入子结点数量
		//>	----------------------------------------------------------------------------
		int ChildNodeCount = xn->getChildNodeCount();
		out->write((const void*)&ChildNodeCount,sizeof(int));

		//> 保存每一个子结点
		//>	----------------------------------------------------------------------------
		for( int i=0; i<ChildNodeCount; i++ )
		{
			XmlNode* xcn = xn->getChildNodeByIndex(i);
			if( !saveBinaryXmlImp(xcn,out) )
			{
				assert(false && "XmlNode在保存子节点数据时出错。");
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
