
#ifndef _XML_NODE_H_
#define _XML_NODE_H_

#pragma warning( disable : 4786 )

#include<map>
#include<vector>
#include<string>
#include"XmlAttribute.H"
//>	----------------------------------------------------------------------------

namespace xnd
{

/*!
 *******************************************************************************
 *	
 *******************************************************************************
 *
 *	\brief		Xml节点处理类
 *
 *	\author		陈治国
 *
 *	\version	1.2
 *
 *	\date		2006-06-11
 *	\date		2006-09-25
 *	\date		2006-09-30
 *	\date		2006-11-15
 *	\date		2006-12-21
 *	\date		2007-01-12
 *
 *	\bug		暂无
 *
 *	\warning	暂无
 *
 ******************************************************************************
 */
class XmlNode
{
public:

	/*!
	 *******************************************************************************
	 *	默认构造函数
	 *******************************************************************************
	 */
	XmlNode( void );

	/*!
	 *******************************************************************************
	 *	构造函数
	 *******************************************************************************
	 *	\param[in] const std::string& nodeName		节点名称
	 *  \param[in] XmlNode* root					父结点
	 *******************************************************************************
	 */
	XmlNode( const char* szName, XmlNode* root = NULL );

	/*!
	 *******************************************************************************
	 *	拷贝构造函数
	 *******************************************************************************
	 */
	XmlNode( const XmlNode& xn );

	/*!
	 *******************************************************************************
	 *	
	 *******************************************************************************
	 */
	XmlNode& operator = ( const XmlNode& xn );
	
	/*!
	 *******************************************************************************
	 *	析构函数
	 *******************************************************************************
	 *	
	 *******************************************************************************
	 */
	~XmlNode( void );

public:

	/*!
	 *******************************************************************************
	 *	节点自身创建函数
	 *******************************************************************************
	 *	\param[in]		strName		节点名称，不得为空
	 *	\param[in]		root		节点的父结点
	 *******************************************************************************
	 */
	bool				create( const char* strName , XmlNode* root = NULL );

	/*!
	 *******************************************************************************
	 *	释放当前结点
	 *******************************************************************************
	 */
	void				release( void );

public:

	/*!
	 *******************************************************************************
	 *	从父结点中移除自己
	 *******************************************************************************
	 */
	void				removeFromParent( void );

	/*!
	 *******************************************************************************
	 *	克隆结点,这个结点不会克隆父结点数据
	 *******************************************************************************
	 *	\param[in] XmlNode* 待克隆的结点
	 *  \return    XmlNode* 克隆好的结点
	 *******************************************************************************
	 */
	XmlNode*			clone( void ) const;
	
	/*!
	 *******************************************************************************
	 *	拷贝一个结点
	 *******************************************************************************
	 */
	bool				copy( const XmlNode& xn);

	/*!
	 *******************************************************************************
	 *	比较两个节点是否相同
	 *******************************************************************************
	 */
	bool				compare( const XmlNode& xn) const;

public:


	/*!
	 *******************************************************************************
	 *	取得当前结点的根结点
	 *******************************************************************************
	 *	\return XmlNode* 返回当前结点根结点
	 *******************************************************************************
	 */
	XmlNode*			getRoot( void ) const{ return m_Root; }

	/*!
	 *******************************************************************************
	 *	取得当前结点名称
	 *******************************************************************************
	 *	\return std::string 返回当前结点的名称
	 *******************************************************************************
	 */
	const char*			getName( void ) const { return m_Name.c_str(); }

	/*!
	 *******************************************************************************
	 *	设置当前结点名称
	 *******************************************************************************
	 */
	void				setName( const char* strName) { m_Name = strName; }

	/*!
	 *******************************************************************************
	 *	返回属性的数量
	 *******************************************************************************
	 *	\return int 得到数量
	 *******************************************************************************
	 */
	int					getAttributeCount( void ) const { return m_AttributeCount; }

	/*!
	 *******************************************************************************
	 *	返回子结点的数量
	 *******************************************************************************
	 *	\return int 得到数量
	 *******************************************************************************
	 */
	int					getChildNodeCount( void ) const { return m_ChildNodeCount; }

public:

	/*!
	 *******************************************************************************
	 *	查找属性
	 *******************************************************************************
	 *	\param[in]	const char* attrName	属性名称
	 *	\return		bool					找到返回true，没找到，返回false
	 *******************************************************************************
	 */
	 bool				findAttribute( const char* attrName )const
	{
		ATTR_CONST_ITERATOR it  = m_Attributes.find(attrName);
		return	(it != m_Attributes.end() );
	}

	/*!
	*******************************************************************************
	*	取得属性
	*******************************************************************************
	*	\param[in]	const char* attrName	属性名称
	*	\return 	XmlAttribute&    		输出的属性值
	*******************************************************************************
	*/
	const XmlAttribute* getAttribute(const char* attrName)const
	{

		ATTR_CONST_ITERATOR it = m_Attributes.find(attrName);

		if( it != m_Attributes.end() )	return &it->second;
		else							return NULL;
	}

	 XmlAttribute*		getAttribute(const char* attrName)
	{
		ATTR_ITERATOR it = m_Attributes.find(attrName);

		if( it != m_Attributes.end() )	return &it->second;
		else							return NULL;
	}

	/*!
	 *******************************************************************************
	 *	取得属性长度
	 *******************************************************************************
	 */
	int					getAttributeLength( const char* attrName ) const
	{
		const XmlAttribute* pNode = getAttribute(attrName);

		if( pNode )	return pNode->getLength();
		else		return 0 ;
	}

public:

	/*!
	 *******************************************************************************
	 *	设置属性值
	 *******************************************************************************
	 */
	bool				setAttributeAsChar	( const char* attrName , const char& val);
	bool				setAttributeAsInt	( const char* attrName , const int& val);
	bool				setAttributeAsFloat	( const char* attrName , const float& val);
	bool				setAttributeAsString( const char* attrName , const char* val);
	bool				setAttributeAsBinary( const char* attrName , const void* val,const int& size);
	
	/*!
	 *******************************************************************************
	 *	取得属性值
	 *******************************************************************************
	 */
	char				getAttributeAsChar	( const char* attrName , const char&  defval = '\0'  )const;
	int					getAttributeAsInt	( const char* attrName , const int&   defval = 0     )const;
	float				getAttributeAsFloat	( const char* attrName , const float& defval = 0.0f  )const;
	std::string			getAttributeAsString( const char* attrName , const char*  defval = "\0"  )const;
	int					getAttributeAsBinary( const char* attrName , void* out,const int& outsize)const;

public:

	/*!
	 *******************************************************************************
	 *	取得第一个属性结点
	 *******************************************************************************
	 */
	XmlAttribute*		getFirstAttribute( void ) ;

	/*!
	 *******************************************************************************
	 *	取得下一个属性结点,其名字为的下一个结点
	 *******************************************************************************
	 */
	XmlAttribute*		getNextAttribute( const XmlAttribute* attr );

public:

	/*!
	 *******************************************************************************
	 *	添加属性
	 *******************************************************************************
	 *	\param[in] XmlAttribute& xa		待添加的属性
	 *  \return    bool					添加成功，返回true，失败返回false
	 *******************************************************************************
	 */
	bool				addAttribute( XmlAttribute& xa );

	/*!
	 *******************************************************************************
	 *	创建属性
	 *******************************************************************************
	 *	\param[in] XmlDataType  xdType  属性数据的类型
	 *  \return    const char*	name	属性名称
	 *  \return    bool					添加成功，返回true，失败返回false
	 *******************************************************************************
	 */
	bool				createAttribute( XmlDataType xdType , const char* name );

	/*!
	 *******************************************************************************
	 *	销毁属性
	 *******************************************************************************
	 *	\param[in] const char*name	  待销毁的属性名称
	 *  \param[in] bool               是否清除属性数据
	 *  \return    bool               成功返回true,失败返回false
	 *******************************************************************************
	 */
	bool				destroyAttribute( const char* name );

public:

	/*!
	 *******************************************************************************
	 *	根据子结点索引取得子结点
	 *******************************************************************************
	 */
	XmlNode*			getChildNodeByIndex( const int& index ) const;

	/*!
	 *******************************************************************************
	 *	根据子结点名称取得第一个子结点
	 *******************************************************************************
	 */
	XmlNode*			getFirstChildNode( const char* name ) const;

	/*!
	 *******************************************************************************
	 *	取得node之后的，并且名称和node结点的名称一样的结点
	 *******************************************************************************
	 */
	XmlNode*			getNextChildNode( XmlNode* node ) const;

public:

	/*!
	 *******************************************************************************
	 *	添加子结点
	 *******************************************************************************
	 *	\param[in] XmlNode* 待添加的子结点
	 *  \return    XmlNode* 返回添加成功的子结点
	 *******************************************************************************
	 */
	XmlNode*			addChildNode( XmlNode* pNode );

	/*!
	 *******************************************************************************
	 *	创建子结点
	 *******************************************************************************
	 *	\param[in] const char*strName 待添加的子结点名称
	 *  \return    XmlNode*           返回添加成功的子结点
	 *******************************************************************************
	 */
	XmlNode*			createChildNode( const char* strName );

	/*!
	 *******************************************************************************
	 *	销毁子结点
	 *******************************************************************************
	 *	\param[in] XmlNode* 待销毁的子结点
	 *  \return    bool     成功返回true，失败返回false
	 *******************************************************************************
	 */
	bool				destroyChildNode( XmlNode* pNode );

	/*!
	 *******************************************************************************
	 *	销毁所有的子结点
	 *******************************************************************************
	 */
	void				destroyAllChildNode( void );

private:

	typedef				std::map<std::string,XmlAttribute>  MAP_ATTR;
	typedef				std::vector<XmlNode*>				VEC_NODE;

	typedef				MAP_ATTR::iterator					ATTR_ITERATOR;
	typedef				VEC_NODE::iterator					NODE_ITERATOR;

	typedef				MAP_ATTR::const_iterator			ATTR_CONST_ITERATOR;
	typedef				VEC_NODE::const_iterator			NODE_CONST_ITERATOR;

private:

	std::string			m_Name;								///< 节点名称
	XmlNode*			m_Root;								///< 父结点		

	MAP_ATTR			m_Attributes;						///< 属性列表
	VEC_NODE			m_ChildNodes;						///< 子结点列表

	int					m_AttributeCount;					///< 属性数量
	int					m_ChildNodeCount;					///< 子结点数量
};



/*>	---------------------------------------------------------------------------
	               inline function implement
---------------------------------------------------------------------------	<*/

/*!
 *******************************************************************************
 *	设置字符型属性值
 *******************************************************************************
 */
inline bool XmlNode::setAttributeAsChar(const char* attrName,const char& val)
{
	XmlAttribute* xa = getAttribute(attrName);

	if( xa )	return xa->setValueByChar(val);
	else		return addAttribute(XmlAttribute(attrName,val));
}

/*!
 *******************************************************************************
 *	设置整型属性值
 *******************************************************************************
 */
inline bool XmlNode::setAttributeAsInt(const char* attrName,const int& val)
{
	XmlAttribute* xa = getAttribute(attrName);

	if( xa )	return xa->setValueByInt(val);
	else		return addAttribute(XmlAttribute(attrName,val));
}

/*!
 *******************************************************************************
 *	设置浮点型属性值
 *******************************************************************************
 */
inline bool XmlNode::setAttributeAsFloat(const char* attrName,const float& val)
{
	XmlAttribute* xa = getAttribute(attrName);

	if( xa )	return xa->setValueByFloat(val);
	else		return addAttribute(XmlAttribute(attrName,val));
}

/*!
 *******************************************************************************
 *	设置字符串型属性值
 *******************************************************************************
 */
inline bool XmlNode::setAttributeAsString(const char* attrName,const char* val)
{
	XmlAttribute* xa = getAttribute(attrName);

	if( xa )	return xa->setValueByString(val);
	else		return addAttribute(XmlAttribute(attrName,val));
}

/*!
 *******************************************************************************
 *	设置二进制型属性值
 *******************************************************************************
 */
inline bool XmlNode::setAttributeAsBinary(const char* attrName,const void* val,const int& size)
{
	XmlAttribute* xa = getAttribute(attrName);

	if( xa )	return xa->setValueByBinary(val,size);
	else		return addAttribute(XmlAttribute(attrName,val,size));
}

/*!
 *******************************************************************************
 *	取得字符型属性值
 *******************************************************************************
 */
inline char XmlNode::getAttributeAsChar(const char* attrName,const char& defval)const
{
	const XmlAttribute* xa = getAttribute(attrName);

	if( xa )	return xa->getValueAsChar(defval);
	else		return defval;
}

/*!
 *******************************************************************************
 *	取得整型属性值
 *******************************************************************************
 */
inline int XmlNode::getAttributeAsInt(const char* attrName,const int& defval)const
{
	const XmlAttribute* xa = getAttribute(attrName);
	
	if( xa )	return xa->getValueAsInt(defval);
	else		return defval;
}

/*!
 *******************************************************************************
 *	取得浮点型属性值
 *******************************************************************************
 */
inline float XmlNode::getAttributeAsFloat(const char* attrName,const float& defval)const
{
	const XmlAttribute* xa = getAttribute(attrName);
	
	if( xa )	return xa->getValueAsFloat(defval);
	else		return defval;
}

/*!
 *******************************************************************************
 *	取得字符串型属性值
 *******************************************************************************
 */
inline std::string XmlNode::getAttributeAsString(const char* attrName,const char* defval)const
{
	const XmlAttribute* xa = getAttribute(attrName);
	
	if( xa )	return xa->getValueAsString(defval);
	else		return defval;
}

/*!
 *******************************************************************************
 *	取得二进制型属性值
 *******************************************************************************
 */
inline int XmlNode::getAttributeAsBinary(const char* attrName,void* out,const int& outsize)const
{
	const XmlAttribute* xa = getAttribute(attrName);
	
	if( xa )	return xa->getValueAsBinary(out,outsize);
	else		return 0;
}

}//namespace

#endif//_XML_NODE_H_