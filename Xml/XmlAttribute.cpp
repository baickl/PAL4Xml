#include "StdAfx.h"
#include "XmlAttribute.H"
#include "XmlStream.h"
//>	----------------------------------------------------------------------------

namespace xnd
{


/*!
 *******************************************************************************
 *	构造函数
 *******************************************************************************
 */
XmlAttribute::XmlAttribute()
{
	m_Type					=	XML_BINARY;
	m_Name					=	"ATTR_NAME_UNKNOW";
	m_Len					=   0;
	m_Value.binaryVal		=	0;
}

/*!
 *******************************************************************************
 *	构造函数
 *******************************************************************************
 */
XmlAttribute::XmlAttribute(XmlDataType xdt,const char* attrName )
{
	if( !attrName )
		throw std::exception("XmlAttribute's name is null!");
	
	m_Type		=	xdt;
	m_Name		=	attrName;
	m_Len		=	0;
	
	m_Value.intVal = 0;
	if( xdt == XML_STRING )
	{
		m_Value.stringVal = new std::string;
	}
}

/*!
 *******************************************************************************
 *	构造函数
 *******************************************************************************
 */
XmlAttribute::XmlAttribute(const char* attrName , const char& chVal)
{
	if( !attrName )
		throw std::exception("XmlAttribute's name is null!");

	m_Type		=	XML_CHAR;
	m_Name		=	attrName;
	m_Len		=	0;
	
	m_Value.charVal = chVal;
}

/*!
 *******************************************************************************
 *	构造函数
 *******************************************************************************
 */
XmlAttribute::XmlAttribute(const char* attrName,const int& nVal )
{
	if( !attrName )
		throw std::exception("XmlAttribute's name is null!");

	m_Type		=	XML_INT;
	m_Name		=	attrName;
	m_Len		=	0;
	
	m_Value.intVal = nVal;
}

/*!
 *******************************************************************************
 *	构造函数
 *******************************************************************************
 */
XmlAttribute::XmlAttribute(const char* attrName,const float& rVal)
{
	if( !attrName )
		throw std::exception("XmlAttribute's name is null!");

	m_Type		=	XML_FLOAT;
	m_Name		=	attrName;
	m_Len		=	0;
	
	m_Value.floatVal = rVal;
}

/*!
 *******************************************************************************
 *	构造函数
 *******************************************************************************
 */
XmlAttribute::XmlAttribute(const char* attrName,const char* strVal )
{
	if( !attrName )
		throw std::exception("XmlAttribute's name is null!");

	m_Type		=	XML_STRING;
	m_Name		=	attrName;
	m_Len		=	0;
	
	m_Value.stringVal = new std::string;
	*(m_Value.stringVal) = strVal?strVal:"";
}

/*!
 *******************************************************************************
 *	构造函数
 *******************************************************************************
 */
XmlAttribute::XmlAttribute(const char* attrName, const void* pBuff,int nSize)
{
	if( !attrName )
		throw std::exception("XmlAttribute's name is null!");

	m_Type		=	XML_BINARY;
	m_Name		=	attrName;
	m_Len		=	0;
	
	m_Value.intVal = 0;

	if( nSize > 0)
	{
		m_Len = nSize;
		m_Value.binaryVal = malloc(m_Len);
		memcpy(m_Value.binaryVal,pBuff,nSize);
	}
}

/*!
 *******************************************************************************
 *	构造函数
 *******************************************************************************
 */
XmlAttribute::XmlAttribute(const XmlAttribute& xa)
{
	// 拷贝
	m_Type	=	xa.m_Type;
	m_Name	=	xa.m_Name;

	if( xa.m_Type == XML_CHAR )
	{
		m_Value.charVal = xa.m_Value.charVal;
	}
	else if( xa.m_Type == XML_INT )
	{
		m_Value.intVal = xa.m_Value.intVal;
	}
	else if( xa.m_Type == XML_FLOAT )
	{
		m_Value.floatVal = xa.m_Value.floatVal;
	}
	else if( xa.m_Type == XML_STRING)
	{
		m_Value.stringVal = new std::string;
		*(m_Value.stringVal) = xa.m_Value.stringVal->c_str();
	}
	else
	{
		m_Len = xa.m_Len;
		if( m_Len > 0 )
		{
			m_Value.binaryVal = malloc(m_Len);
			memcpy(m_Value.binaryVal,xa.m_Value.binaryVal,m_Len);
		}
	}
}

/*!
 *******************************************************************************
 *	析构函数
 *******************************************************************************
 */
XmlAttribute::~XmlAttribute()
{
	clear();
}

/*!
 *******************************************************************************
 *	清除数据
 *******************************************************************************
 */
void XmlAttribute::clear()
{
	if( m_Type == XML_STRING )
	{
//		if( m_Value.stringVal )
//		{
			delete m_Value.stringVal;
			m_Value.stringVal = 0;
//		}
	}
	else if( m_Type == XML_BINARY )
	{
		if( m_Len > 0 )
		{
			free(m_Value.binaryVal);
			m_Value.binaryVal = 0;
			m_Len = 0;
		}
		else
		{
			m_Value.binaryVal = 0;
		}
	}
	else
	{
		m_Value.intVal = 0;
	}
}

/*!
 *******************************************************************************
 *	取得属性数据长度
 *******************************************************************************
 */
int XmlAttribute::getLength( void ) const
{
	if( m_Type == XML_CHAR )		return sizeof(m_Value.charVal);
	else if(m_Type == XML_INT )		return sizeof(m_Value.intVal);
	else if( m_Type == XML_FLOAT )	return sizeof(m_Value.floatVal);
	else if( m_Type == XML_STRING)	return static_cast<int>(m_Value.stringVal->length());
	else							return m_Len;
}

/*!
 *******************************************************************************
 *	赋值操作
 *******************************************************************************
 */
XmlAttribute& XmlAttribute::operator = (const XmlAttribute& xa)
{
	//> 数据清空
	//>	----------------------------------------------------------------------------
	if( m_Type == XML_STRING )
	{
		delete m_Value.stringVal;
		m_Value.stringVal	=  0;
	}
	else if( m_Type == XML_BINARY )
	{
		if( m_Len > 0 )
		{
			free(m_Value.binaryVal);
			m_Value.binaryVal = 0;
			m_Len = 0;
		}
		else
		{
			m_Value.binaryVal = 0;
		}
	}
	else
	{
		m_Value.intVal = 0;
	}

	//> 开始拷贝
	//>	----------------------------------------------------------------------------
	m_Type	=	xa.m_Type;
	m_Name	=	xa.m_Name;

	if( xa.m_Type == XML_CHAR )
	{
		m_Value.charVal = xa.m_Value.charVal;
	}
	else if( xa.m_Type == XML_INT )
	{
		m_Value.intVal = xa.m_Value.intVal;
	}
	else if( xa.m_Type == XML_FLOAT )
	{
		m_Value.floatVal = xa.m_Value.floatVal;
	}
	else if( xa.m_Type == XML_STRING)
	{
		if( m_Value.stringVal )
		{
			delete m_Value.stringVal;
			m_Value.stringVal = 0;
		}

		m_Value.stringVal = new std::string;
		*(m_Value.stringVal) = xa.m_Value.stringVal->c_str();
	}
	else
	{
		m_Len = xa.m_Len;
		if( m_Len > 0 )
		{
			m_Value.binaryVal = malloc(m_Len);
			memcpy(m_Value.binaryVal,xa.m_Value.binaryVal,m_Len);
		}
	}

	return *this;
}

/*!
 *******************************************************************************
 *	判断两属性是否相同
 *******************************************************************************
 */
bool XmlAttribute::operator ==( const XmlAttribute& xa ) const
{
	bool ret = false;

	if( m_Type == xa.m_Type && m_Name == xa.m_Name )
	{
		if( m_Type == XML_CHAR )
		{
			if( m_Value.charVal == xa.m_Value.charVal )
				ret = true;
		}
		else if( m_Type == XML_INT )
		{
			if( m_Value.intVal == xa.m_Value.intVal)
				ret = true;
		}
		else if( m_Type == XML_FLOAT )
		{
			if( m_Value.floatVal == xa.m_Value.floatVal )
				ret = true;
		}
		else if( m_Type == XML_STRING )
		{
			if( *(m_Value.stringVal) == *(xa.m_Value.stringVal) )
				ret = true;
		}
		else
		{
			if( m_Len == xa.m_Len )
			{
				if( m_Len > 0 )
				{
					if( memcmp(m_Value.binaryVal,xa.m_Value.binaryVal,m_Len) == 0)
						ret = true;
				}
				else
				{
					ret = true;
				}
			}
		}
	}

	return ret;
}

/*!
 *******************************************************************************
 *	取得字符型数据
 *******************************************************************************
 */
char XmlAttribute::getValueAsChar( const char& defval/* = '\0'  */)const
{
	char ret = defval;

	if( m_Type == XML_CHAR )
	{
		ret = m_Value.charVal;
	}
	else if( m_Type == XML_INT )
	{
		ret = (char)m_Value.intVal;
	}
	else if( m_Type == XML_FLOAT )
	{
		ret = (char)m_Value.floatVal;
	}
	else if( m_Type == XML_STRING )
	{
		sscanf(m_Value.stringVal->c_str(),"%c",&ret);
	}
	else
	{
		if( m_Len > 0)
		{
			if( m_Len >= sizeof(ret) )
				memcpy(&ret,m_Value.binaryVal,sizeof(ret));
			else
				memcpy(&ret,m_Value.binaryVal,m_Len);
		}
	}

	return ret;
}

/*!
 *******************************************************************************
 *	取出整型数据
 *******************************************************************************
 */
int XmlAttribute::getValueAsInt( const int& defval /* = 0 */)const
{
	int ret = defval;

	if( m_Type == XML_CHAR )
	{
		ret = (int)m_Value.charVal;
	}
	else if( m_Type == XML_INT )
	{
		ret = m_Value.intVal;
	}
	else if( m_Type == XML_FLOAT )
	{
		ret = (int)m_Value.floatVal;
	}
	else if( m_Type == XML_STRING )
	{
		sscanf(m_Value.stringVal->c_str(),"%d",&ret);
	}
	else
	{
		if( m_Len > 0)
		{
			if( m_Len >= sizeof(ret) )
				memcpy(&ret,m_Value.binaryVal,sizeof(ret));
			else
				memcpy(&ret,m_Value.binaryVal,m_Len);
		}
	}

	return ret;
}

/*!
 *******************************************************************************
 *	取得浮点型数据
 *******************************************************************************
 */
float XmlAttribute::getValueAsFloat( const float& defval /* = 0.0f  */) const
{
	float ret = defval;

	if( m_Type == XML_CHAR )
	{
		ret = (float)m_Value.charVal;
	}
	else if( m_Type == XML_INT )
	{
		ret = (float)m_Value.intVal;
	}
	else if( m_Type == XML_FLOAT )
	{
		ret = m_Value.floatVal;
	}
	else if( m_Type == XML_STRING )
	{
		sscanf(m_Value.stringVal->c_str(),"%f",&ret);
	}
	else
	{
		if( m_Len > 0)
		{
			if( m_Len >= sizeof(ret) )
				memcpy(&ret,m_Value.binaryVal,sizeof(ret));
			else
				memcpy(&ret,m_Value.binaryVal,m_Len);
		}
	}

	return ret;
}

/*!
 *******************************************************************************
 *	取得字符串型数据
 *******************************************************************************
 */
std::string XmlAttribute::getValueAsString( const char* defval /* = "\0"  */) const
{
	std::string ret = defval ;

	if( m_Type == XML_CHAR )
	{
		char tmp[2] = {0};
		tmp[0] = m_Value.charVal;
		ret = tmp;
	}
	else if( m_Type == XML_INT )
	{
		char tmp[32] = {0};
		itoa(m_Value.intVal,tmp,10);
		ret = tmp;
	}
	else if( m_Type == XML_FLOAT )
	{
		char tmp[32] = {0};
		sprintf(tmp,"%f",m_Value.floatVal);
		ret = tmp;
	}
	else if( m_Type == XML_STRING )
	{
		ret = m_Value.stringVal->c_str();
	}
	else
	{
		if( m_Len > 0)
		{
			char * tmp = new char[m_Len+1];
			memcpy(tmp,m_Value.binaryVal,m_Len);
			tmp[m_Len] = 0;
			ret = tmp;
			delete[] tmp;
		}
	}
	
	return ret;
}

/*!
 *******************************************************************************
 *	取得二进制数据
 *******************************************************************************
 */
int XmlAttribute::getValueAsBinary( void* outdata,const int& iSize ) const
{
	int readLen = 0;

	if( m_Type == XML_CHAR )
	{
		readLen = sizeof(m_Value.charVal);
		memcpy(outdata,&m_Value.charVal,readLen);
	}
	else if( m_Type == XML_INT )
	{
		readLen = sizeof(m_Value.intVal);
		memcpy(outdata,&m_Value.intVal,readLen);
	}
	else if( m_Type == XML_FLOAT )
	{
		readLen = sizeof(m_Value.floatVal);
		memcpy(outdata,&m_Value.floatVal,readLen);
		return readLen;
	}
	else if( m_Type == XML_STRING )
	{
		readLen = static_cast<int>(m_Value.stringVal->length());
		memcpy(outdata,m_Value.stringVal->c_str(),readLen);
	}
	else
	{
		if( m_Len > 0 && iSize > 0 )
		{
			if( m_Len < iSize )
			{
				memcpy( outdata,m_Value.binaryVal,m_Len);
				readLen = m_Len;
			}
			else
			{
				memcpy( outdata,m_Value.binaryVal,iSize);
				readLen = iSize;
			}
		}
		else
		{
			readLen = 0;
		}

	}

	//>	返回读取长度
	return readLen;
}

/*!
 *******************************************************************************
 *	设置字符数据
 *******************************************************************************
 */
bool XmlAttribute::setValueByChar( const char& val )
{
	bool ret = false;

	if( m_Type == XML_CHAR )
	{
		m_Value.charVal = val;
		ret = true;
	}
	else if( m_Type == XML_INT )
	{
		m_Value.intVal = (int)val;
		ret = true;
	}
	else if( m_Type == XML_FLOAT )
	{
		m_Value.floatVal = (float)val;
		ret = true;
	}
	else if( m_Type == XML_STRING )
	{
		char tmp[32]={0};

		sprintf(tmp,"%c",val);
		*(m_Value.stringVal)=tmp;

		ret = true;
	}
	else
	{
		if( m_Len > 0)
		{
			free(m_Value.binaryVal);
			m_Len = 0;
			m_Value.binaryVal = NULL;
		}

		m_Len = sizeof(val);
		m_Value.binaryVal = malloc(m_Len);
		memcpy(m_Value.binaryVal,&val,m_Len);

		ret = true;
	}

	return ret;
}

/*!
 *******************************************************************************
 *	设置整型数据
 *******************************************************************************
 */
bool XmlAttribute::setValueByInt( const int& val)
{
	bool ret = false;

	if( m_Type == XML_CHAR )
	{
		m_Value.charVal = (char)val;
		ret = true;
	}
	else if( m_Type == XML_INT )
	{
		m_Value.intVal = val;
		ret = true;
	}
	else if( m_Type == XML_FLOAT )
	{
		m_Value.floatVal = (float)val;
		ret = true;
	}
	else if( m_Type == XML_STRING )
	{
		char tmp[32]={0};

		sprintf(tmp,"%d",val);
		*(m_Value.stringVal)=tmp;

		ret = true;
	}
	else
	{
		if( m_Len > 0)
		{	
			free(m_Value.binaryVal);
			m_Len = 0;
			m_Value.binaryVal = NULL;
		}

		m_Len = sizeof(val);
		m_Value.binaryVal = malloc(m_Len);
		memcpy(m_Value.binaryVal,&val,m_Len);

		ret = true;
	}

	return ret;
}

/*!
 *******************************************************************************
 *	设置整型数据
 *******************************************************************************
 */
bool XmlAttribute::setValueByFloat( const float& val)
{
	bool ret = false;

	if( m_Type == XML_CHAR )
	{
		m_Value.charVal = (char)val;
		ret = true;
	}
	else if( m_Type == XML_INT )
	{
		m_Value.intVal = (int)val;
		ret = true;
	}
	else if( m_Type == XML_FLOAT )
	{
		m_Value.floatVal = val;
		ret = true;
	}
	else if( m_Type == XML_STRING )
	{
		char tmp[64]={0};

		sprintf(tmp,"%f",val);
		*(m_Value.stringVal)=tmp;

		ret = true;
	}
	else
	{
		if( m_Len > 0)
		{
			free(m_Value.binaryVal);
			m_Len = 0;
			m_Value.binaryVal = NULL;
		}

		m_Len = sizeof(val);
		m_Value.binaryVal = malloc(m_Len);
		memcpy(m_Value.binaryVal,&val,m_Len);

		ret = true;
	}

	return ret;
}

/*!
 *******************************************************************************
 *	设置字符型数据
 *******************************************************************************
 */
bool XmlAttribute::setValueByString( const char* val)
{
	bool ret = false;

	if( m_Type == XML_CHAR )
	{
		m_Value.charVal = val[0];
		ret = true;
	}
	else if( m_Type == XML_INT )
	{
		m_Value.intVal = atoi(val);
		ret = true;
	}
	else if( m_Type == XML_FLOAT )
	{
		m_Value.floatVal = (float)atof(val);
		ret = true;
	}
	else if( m_Type == XML_STRING )
	{
		*(m_Value.stringVal)=val;
		ret = true;
	}
	else
	{
		if( m_Len > 0)
		{
			free(m_Value.binaryVal);
			m_Len = 0;
			m_Value.binaryVal = NULL;
		}

		if( val )
		{
			m_Len = static_cast<int>(strlen( val ));
			m_Value.binaryVal = malloc( m_Len );
			memcpy( m_Value.binaryVal , val,m_Len );
		}

		ret = true;
	}

	return ret;
}

/*!
 *******************************************************************************
 *	设置二进制型数据
 *******************************************************************************
 */
bool XmlAttribute::setValueByBinary( const void* indata,const int& iSize )
{
	bool ret = false;

	if( m_Type == XML_CHAR )
	{
		if( iSize > 0 )
		{
			char* p = (char*)indata;
			m_Value.charVal = p[0];
			ret = true;
		}
	}
	else if( m_Type == XML_INT )
	{
		if( iSize > 0 )
		{
			int* p = (int*)indata;
			m_Value.intVal = p[0];
			ret = true;
		}
	}
	else if( m_Type == XML_FLOAT )
	{
		if( iSize > 0 )
		{
			float* p = (float*)indata;
			m_Value.floatVal = p[0];
			ret = true;
		}
	}
	else if( m_Type == XML_STRING )
	{
		if( iSize > 0 )
		{
			char* p = (char*)indata;
			m_Value.stringVal->assign(p);
			ret = true;
		}
	}
	else
	{
		if( m_Len > 0)
		{
			free(m_Value.binaryVal);
			m_Len = 0;
			m_Value.binaryVal = NULL;
		}

		if( iSize > 0 )
		{
			m_Len = iSize;
			m_Value.binaryVal = malloc(m_Len);
			memcpy(m_Value.binaryVal,indata,m_Len);
		}
	}

	return ret;
}

}//namespace