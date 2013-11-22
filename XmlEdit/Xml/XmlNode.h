
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
 *	\brief		Xml�ڵ㴦����
 *
 *	\author		���ι�
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
 *	\bug		����
 *
 *	\warning	����
 *
 ******************************************************************************
 */
class XmlNode
{
public:

	/*!
	 *******************************************************************************
	 *	Ĭ�Ϲ��캯��
	 *******************************************************************************
	 */
	XmlNode( void );

	/*!
	 *******************************************************************************
	 *	���캯��
	 *******************************************************************************
	 *	\param[in] const std::string& nodeName		�ڵ�����
	 *  \param[in] XmlNode* root					�����
	 *******************************************************************************
	 */
	XmlNode( const char* szName, XmlNode* root = NULL );

	/*!
	 *******************************************************************************
	 *	�������캯��
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
	 *	��������
	 *******************************************************************************
	 *	
	 *******************************************************************************
	 */
	~XmlNode( void );

public:

	/*!
	 *******************************************************************************
	 *	�ڵ�����������
	 *******************************************************************************
	 *	\param[in]		strName		�ڵ����ƣ�����Ϊ��
	 *	\param[in]		root		�ڵ�ĸ����
	 *******************************************************************************
	 */
	bool				create( const char* strName , XmlNode* root = NULL );

	/*!
	 *******************************************************************************
	 *	�ͷŵ�ǰ���
	 *******************************************************************************
	 */
	void				release( void );

public:

	/*!
	 *******************************************************************************
	 *	�Ӹ�������Ƴ��Լ�
	 *******************************************************************************
	 */
	void				removeFromParent( void );

	/*!
	 *******************************************************************************
	 *	��¡���,�����㲻���¡���������
	 *******************************************************************************
	 *	\param[in] XmlNode* ����¡�Ľ��
	 *  \return    XmlNode* ��¡�õĽ��
	 *******************************************************************************
	 */
	XmlNode*			clone( void ) const;
	
	/*!
	 *******************************************************************************
	 *	����һ�����
	 *******************************************************************************
	 */
	bool				copy( const XmlNode& xn);

	/*!
	 *******************************************************************************
	 *	�Ƚ������ڵ��Ƿ���ͬ
	 *******************************************************************************
	 */
	bool				compare( const XmlNode& xn) const;

public:


	/*!
	 *******************************************************************************
	 *	ȡ�õ�ǰ���ĸ����
	 *******************************************************************************
	 *	\return XmlNode* ���ص�ǰ�������
	 *******************************************************************************
	 */
	XmlNode*			getRoot( void ) const{ return m_Root; }

	/*!
	 *******************************************************************************
	 *	ȡ�õ�ǰ�������
	 *******************************************************************************
	 *	\return std::string ���ص�ǰ��������
	 *******************************************************************************
	 */
	const char*			getName( void ) const { return m_Name.c_str(); }

	/*!
	 *******************************************************************************
	 *	���õ�ǰ�������
	 *******************************************************************************
	 */
	void				setName( const char* strName) { m_Name = strName; }

	/*!
	 *******************************************************************************
	 *	�������Ե�����
	 *******************************************************************************
	 *	\return int �õ�����
	 *******************************************************************************
	 */
	int					getAttributeCount( void ) const { return m_AttributeCount; }

	/*!
	 *******************************************************************************
	 *	�����ӽ�������
	 *******************************************************************************
	 *	\return int �õ�����
	 *******************************************************************************
	 */
	int					getChildNodeCount( void ) const { return m_ChildNodeCount; }

public:

	/*!
	 *******************************************************************************
	 *	��������
	 *******************************************************************************
	 *	\param[in]	const char* attrName	��������
	 *	\return		bool					�ҵ�����true��û�ҵ�������false
	 *******************************************************************************
	 */
	 bool				findAttribute( const char* attrName )const
	{
		ATTR_CONST_ITERATOR it  = m_Attributes.find(attrName);
		return	(it != m_Attributes.end() );
	}

	/*!
	*******************************************************************************
	*	ȡ������
	*******************************************************************************
	*	\param[in]	const char* attrName	��������
	*	\return 	XmlAttribute&    		���������ֵ
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
	 *	ȡ�����Գ���
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
	 *	��������ֵ
	 *******************************************************************************
	 */
	bool				setAttributeAsChar	( const char* attrName , const char& val);
	bool				setAttributeAsInt	( const char* attrName , const int& val);
	bool				setAttributeAsFloat	( const char* attrName , const float& val);
	bool				setAttributeAsString( const char* attrName , const char* val);
	bool				setAttributeAsBinary( const char* attrName , const void* val,const int& size);
	
	/*!
	 *******************************************************************************
	 *	ȡ������ֵ
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
	 *	ȡ�õ�һ�����Խ��
	 *******************************************************************************
	 */
	XmlAttribute*		getFirstAttribute( void ) ;

	/*!
	 *******************************************************************************
	 *	ȡ����һ�����Խ��,������Ϊ����һ�����
	 *******************************************************************************
	 */
	XmlAttribute*		getNextAttribute( const XmlAttribute* attr );

public:

	/*!
	 *******************************************************************************
	 *	�������
	 *******************************************************************************
	 *	\param[in] XmlAttribute& xa		����ӵ�����
	 *  \return    bool					��ӳɹ�������true��ʧ�ܷ���false
	 *******************************************************************************
	 */
	bool				addAttribute( XmlAttribute& xa );

	/*!
	 *******************************************************************************
	 *	��������
	 *******************************************************************************
	 *	\param[in] XmlDataType  xdType  �������ݵ�����
	 *  \return    const char*	name	��������
	 *  \return    bool					��ӳɹ�������true��ʧ�ܷ���false
	 *******************************************************************************
	 */
	bool				createAttribute( XmlDataType xdType , const char* name );

	/*!
	 *******************************************************************************
	 *	��������
	 *******************************************************************************
	 *	\param[in] const char*name	  �����ٵ���������
	 *  \param[in] bool               �Ƿ������������
	 *  \return    bool               �ɹ�����true,ʧ�ܷ���false
	 *******************************************************************************
	 */
	bool				destroyAttribute( const char* name );

public:

	/*!
	 *******************************************************************************
	 *	�����ӽ������ȡ���ӽ��
	 *******************************************************************************
	 */
	XmlNode*			getChildNodeByIndex( const int& index ) const;

	/*!
	 *******************************************************************************
	 *	�����ӽ������ȡ�õ�һ���ӽ��
	 *******************************************************************************
	 */
	XmlNode*			getFirstChildNode( const char* name ) const;

	/*!
	 *******************************************************************************
	 *	ȡ��node֮��ģ��������ƺ�node��������һ���Ľ��
	 *******************************************************************************
	 */
	XmlNode*			getNextChildNode( XmlNode* node ) const;

public:

	/*!
	 *******************************************************************************
	 *	����ӽ��
	 *******************************************************************************
	 *	\param[in] XmlNode* ����ӵ��ӽ��
	 *  \return    XmlNode* ������ӳɹ����ӽ��
	 *******************************************************************************
	 */
	XmlNode*			addChildNode( XmlNode* pNode );

	/*!
	 *******************************************************************************
	 *	�����ӽ��
	 *******************************************************************************
	 *	\param[in] const char*strName ����ӵ��ӽ������
	 *  \return    XmlNode*           ������ӳɹ����ӽ��
	 *******************************************************************************
	 */
	XmlNode*			createChildNode( const char* strName );

	/*!
	 *******************************************************************************
	 *	�����ӽ��
	 *******************************************************************************
	 *	\param[in] XmlNode* �����ٵ��ӽ��
	 *  \return    bool     �ɹ�����true��ʧ�ܷ���false
	 *******************************************************************************
	 */
	bool				destroyChildNode( XmlNode* pNode );

	/*!
	 *******************************************************************************
	 *	�������е��ӽ��
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

	std::string			m_Name;								///< �ڵ�����
	XmlNode*			m_Root;								///< �����		

	MAP_ATTR			m_Attributes;						///< �����б�
	VEC_NODE			m_ChildNodes;						///< �ӽ���б�

	int					m_AttributeCount;					///< ��������
	int					m_ChildNodeCount;					///< �ӽ������
};



/*>	---------------------------------------------------------------------------
	               inline function implement
---------------------------------------------------------------------------	<*/

/*!
 *******************************************************************************
 *	�����ַ�������ֵ
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
 *	������������ֵ
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
 *	���ø���������ֵ
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
 *	�����ַ���������ֵ
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
 *	���ö�����������ֵ
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
 *	ȡ���ַ�������ֵ
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
 *	ȡ����������ֵ
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
 *	ȡ�ø���������ֵ
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
 *	ȡ���ַ���������ֵ
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
 *	ȡ�ö�����������ֵ
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