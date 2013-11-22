#include "StdAfx.h"
#include "XmlNode.H"
#include "XmlAttribute.h"
#include "XmlStream.h"
//>	----------------------------------------------------------------------------

namespace xnd
{

/*!
 *******************************************************************************
 *	���캯��
 *******************************************************************************
 */
XmlNode::XmlNode( void )
{
	m_Root	= 0;

	m_AttributeCount = 0;
	m_ChildNodeCount = 0;
}

/*!
 *******************************************************************************
 *	���캯��
 *******************************************************************************
 */
XmlNode::XmlNode(const char* szName,XmlNode*root)
{
	m_Name			 = szName?szName:"";
	m_Root			 = root;
	m_AttributeCount = 0;
	m_ChildNodeCount = 0;

	if(m_Root)
		m_Root->addChildNode(this);
}

/*!
 *******************************************************************************
 *	���캯��
 *******************************************************************************
 */
XmlNode::XmlNode( const XmlNode& xn )
{
	m_Root			 = 0;
	m_AttributeCount = 0;
	m_ChildNodeCount = 0;

	//>	�������
	//>	----------------------------------------------------------------------------
	if( !create(xn.m_Name.c_str()) )
		return ;

	//> ��¡����
	//>	----------------------------------------------------------------------------
	if( xn.m_AttributeCount > 0 )
	{
		ATTR_CONST_ITERATOR it = xn.m_Attributes.begin();
		for( ; it != xn.m_Attributes.end() ; ++it )
		{
			XmlAttribute xa( it->second );				
			addAttribute(xa);
		}
	}
	
	//> ��¡�ӽ��
	//>	----------------------------------------------------------------------------
	if( xn.m_ChildNodeCount > 0 )
	{
		NODE_CONST_ITERATOR it = xn.m_ChildNodes.begin();
		for( ; it != xn.m_ChildNodes.end() ; ++it )
		{
			XmlNode* xn = (*it)->clone();				
			addChildNode( xn );
		}
	}
}

/*!
 *******************************************************************************
 *	��ֵ�����
 *******************************************************************************
 */
XmlNode& XmlNode::operator = ( const XmlNode& xn )
{
	release();

	//>	�������
	//>	----------------------------------------------------------------------------
	if( !create(xn.m_Name.c_str()) )
		return (*this);

	//> ��¡����
	//>	----------------------------------------------------------------------------
	if( xn.m_AttributeCount > 0 )
	{
		ATTR_CONST_ITERATOR it = xn.m_Attributes.begin();
		for( ; it != xn.m_Attributes.end() ; ++it )
		{
			XmlAttribute xa( it->second );				
			addAttribute(xa);
		}
	}
	
	// ��¡�ӽ��
	//>	----------------------------------------------------------------------------
	if( xn.m_ChildNodeCount > 0 )
	{
		NODE_CONST_ITERATOR it = xn.m_ChildNodes.begin();
		for( ; it != xn.m_ChildNodes.end() ; ++it )
		{
			XmlNode* xn = (*it)->clone();				
			addChildNode( xn );
		}
	}

	return (*this);
}

/*!
 *******************************************************************************
 *	��������
 *******************************************************************************
 */
XmlNode::~XmlNode( void )
{
	release();
}

/*!
 *******************************************************************************
 *	�������
 *******************************************************************************
 */
bool XmlNode::create(const char* strName,XmlNode* root /* = NULL */)
{
	release();

	if( strName )
		m_Name = strName;
	
	m_Root = root;

	if(m_Root)
		m_Root->addChildNode(this);

	return true;
}

/*!
 *******************************************************************************
 *	��¡���
 *******************************************************************************
 */
XmlNode* XmlNode::clone( void ) const
{
	XmlNode* ret = new XmlNode(m_Name.c_str() );
	
	// ��¡����
	//>	----------------------------------------------------------------------------
	if( getAttributeCount() > 0 )
	{
		ATTR_CONST_ITERATOR it = m_Attributes.begin();
		for( ; it != m_Attributes.end() ; ++it )
		{
			XmlAttribute xa( it->second );
			ret->addAttribute(xa);
		}
	}
	
	// ��¡�ӽ��
	//>	----------------------------------------------------------------------------
	if( getChildNodeCount() > 0 )
	{
		NODE_CONST_ITERATOR it = m_ChildNodes.begin();
		
		for( ; it != m_ChildNodes.end() ; ++it )
		{
			XmlNode* xn = (*it)->clone();
			ret->addChildNode( xn );
		}
	}
	
	return ret;
}

/*!
 *******************************************************************************
 *	�������
 *******************************************************************************
 */
bool XmlNode::copy( const XmlNode& xn)
{
	release();

	bool result = false;
	//>	�������
	//>	----------------------------------------------------------------------------
	if( !create(xn.getName()) )
		return result;

	//> ��¡����
	//>	----------------------------------------------------------------------------
	if( xn.getAttributeCount() > 0 )
	{
		ATTR_CONST_ITERATOR it = xn.m_Attributes.begin();
		for( ; it != xn.m_Attributes.end() ; ++it )
		{
			XmlAttribute xa( it->second );
			addAttribute(xa);
		}
		result = true;
	}
	
	//> ��¡�ӽ��
	//>	----------------------------------------------------------------------------
	if( xn.getChildNodeCount() > 0 )
	{
		NODE_CONST_ITERATOR it = xn.m_ChildNodes.begin();
		for( ; it != xn.m_ChildNodes.end() ; ++it )
		{
			XmlNode* xn = (*it)->clone();				
			addChildNode( xn );
		}
		result = true;
	}

	return result;
}

/*!
 *******************************************************************************
 *	�Ƚ��������������Ƿ���ͬ
 *******************************************************************************
 */
bool XmlNode::compare(const XmlNode& xn )const
{
	if( getAttributeCount() != xn.getAttributeCount() || 
		getChildNodeCount() != xn.getChildNodeCount() )
	{
		return false;
	}
	

	//>	�Ƚ�����
	//>	----------------------------------------------------------------------------
	MAP_ATTR::const_iterator itAttr  =  m_Attributes.begin();
	MAP_ATTR::const_iterator itAttr2 =  xn.m_Attributes.begin() ;
	while( itAttr != m_Attributes.end() )
	{
		if( (*itAttr) == (*itAttr2) )
		{
			++itAttr;
			++itAttr2;
		}
		else
		{
			return false;
		}
	}

	//>	�Ƚ��ӽ��
	//>	----------------------------------------------------------------------------
	NODE_CONST_ITERATOR itNode  = m_ChildNodes.begin();
	NODE_CONST_ITERATOR itNode2 = xn.m_ChildNodes.begin();
	while( itNode != m_ChildNodes.end() )
	{
		const XmlNode* pTmp  = *itNode;
		const XmlNode* pTmp2 = *itNode2;

		if( !pTmp->compare(*pTmp2) )
			return false;

		++itNode;
		++itNode2;
	}

	return true;
}

/*!
 *******************************************************************************
 *	�ͷŽ�㣬���ͷŵ�ͬʱ�����ͷŵ������ӽ��
 *******************************************************************************
 */
void XmlNode::release( void )
{
	//>	�Ӹ������ɾ����
	//>	----------------------------------------------------------------------------
	if( m_Root )
	{
		NODE_ITERATOR it = m_Root->m_ChildNodes.begin();
		while( it != m_Root->m_ChildNodes.end() )
		{
			if( (*it) == this )
			{
				m_Root->m_ChildNodes.erase(it);
				m_Root->m_ChildNodeCount -- ;
				break;
			}
			
			it ++;
		}
		
		m_Root = 0;
	}
	
	//>	��������
	//>	----------------------------------------------------------------------------
	if( getAttributeCount() > 0)
	{
		ATTR_ITERATOR it = m_Attributes.begin();
		while( it != m_Attributes.end() )
		{
			it->second.clear();
			++it;
		}
		
		m_Attributes.clear();
		m_AttributeCount = 0;
	}
	
	//> �����ӽڵ�
	//>	----------------------------------------------------------------------------
	if( getChildNodeCount() > 0)
	{
		NODE_ITERATOR it = m_ChildNodes.begin();
		while( it != m_ChildNodes.end() )
		{
			XmlNode* nd = *it;
			nd->release();
			delete nd;
			
			it = m_ChildNodes.begin();
		}
		
		m_ChildNodes.clear();
		m_ChildNodeCount = 0;
	}
	
	//> �������
	//>	----------------------------------------------------------------------------
	m_Name.erase(m_Name.begin(),m_Name.end());
}

void XmlNode::removeFromParent( void )
{
	if( m_Root )
	{
		XmlNode::VEC_NODE &vecNode = m_Root->m_ChildNodes;
		XmlNode::NODE_ITERATOR it = vecNode.begin();
		while( it != vecNode.end() )
		{
			XmlNode*pNode = *it;
			if( pNode == this )
			{
				vecNode.erase(it);
				--m_Root->m_ChildNodeCount;
				m_Root = NULL;
			}

			++it;
		}
	}
}

/*!
 *******************************************************************************
 *	ȡ�õ�һ������
 *******************************************************************************
 */
XmlAttribute* XmlNode::getFirstAttribute( void ) 
{
	if( m_AttributeCount <= 0)
		return NULL;

	MAP_ATTR::iterator  it = m_Attributes.begin();	
	return &(it->second);
}

/*!
 *******************************************************************************
 *	ȡ����һ������
 *******************************************************************************
 */
XmlAttribute* XmlNode::getNextAttribute( const XmlAttribute* attr )
{
	if( !attr || m_AttributeCount <= 0 )
		return NULL;

	MAP_ATTR::iterator itFind = m_Attributes.find( attr->getName() );
	if( itFind != m_Attributes.end() )
	{
		++itFind;
		if( itFind != m_Attributes.end() )
			return &(itFind->second );
	}

	return NULL;
}

/*!
 *******************************************************************************
 *	�������
 *******************************************************************************
 */
bool XmlNode::addAttribute(XmlAttribute& xa)
{
	if( !findAttribute(xa.getName()) )
	{
		if( !xa.getName() )
		{
			return false;
		}

		m_Attributes[xa.getName()] = xa;
		m_AttributeCount ++;
	}

	return true;
}

/*!
 *******************************************************************************
 *	��������
 *******************************************************************************
 */
bool XmlNode::createAttribute(XmlDataType xdt,const char* name)
{
	if( !name ) 
	{
		return false;
	}

	if( !findAttribute(name) )
	{
		XmlAttribute xa(xdt,name);
		m_Attributes[name] = xa;
		m_AttributeCount ++;
	}

	return true;
}


/*!
 *******************************************************************************
 *	�Ƴ�����
 *******************************************************************************
 */
bool XmlNode::destroyAttribute(const char* name)
{
	ATTR_ITERATOR it = m_Attributes.find( name );
	
	if( it != m_Attributes.end() )
	{
		it->second.clear();
		m_Attributes.erase(it);

		m_AttributeCount--;
		
		return true;
	}
	
	return false;
}

/*!
 *******************************************************************************
 *	ͨ���ӽڵ�����ȡ���ӽ��
 *******************************************************************************
 */
XmlNode* XmlNode::getChildNodeByIndex(const int& index)const
{
	if( index >= 0 && index <m_ChildNodeCount )		return m_ChildNodes[index];
	else											return NULL ;
}

/*!
 *******************************************************************************
 *	ȡ�õ�һ���ӽ��
 *******************************************************************************
 */
XmlNode* XmlNode::getFirstChildNode(const char* name )const
{
	XmlNode* ret = NULL;
	for( int i=0;i<m_ChildNodeCount;i++ )
	{
		if(m_ChildNodes[i]->m_Name  == name )
		{
			ret = m_ChildNodes[i];
			break;
		}
	}

	return ret;
}

/*!
 *******************************************************************************
 *	ȡ����һ���ӽ��
 *******************************************************************************
 */
XmlNode*XmlNode::getNextChildNode( XmlNode* node )const
{
	bool bAfter  = false;
	XmlNode* ret = NULL;

	for(int i = 0;i<m_ChildNodeCount;i++)
	{
		if( bAfter )
		{
			if(m_ChildNodes[i]->m_Name == node->m_Name )
			{
				ret = m_ChildNodes[i] ;
				break;
			}
		}

		if(node == m_ChildNodes[i] )
		{
			bAfter = true;
		}
	}

	return ret;
}

/*!
 *******************************************************************************
 *	�����ӽ��
 *******************************************************************************
 */
XmlNode* XmlNode::addChildNode( XmlNode * xn )
{
	if( xn )
	{
		XmlNode* ret = xn;

		//> ȡ���丸��㣬�����������ɾ����
		//>	----------------------------------------------------------------------------
		XmlNode* root = xn->m_Root;
		if( root )
		{
			NODE_ITERATOR it = root->m_ChildNodes.begin();
			while( it != root->m_ChildNodes.end() )
			{
				if( (*it) == xn )
				{
					root->m_ChildNodes.erase( it );
					root->m_ChildNodeCount -- ;

					break;
				}

				it++;
			}
		}
		
		//> �����Լ����б���
		//>	----------------------------------------------------------------------------
		m_ChildNodes.push_back(xn);
		m_ChildNodeCount ++;
		
		//> �������ָ���Լ�
		//>	----------------------------------------------------------------------------
		ret = xn;
		xn->m_Root = this;
		
		return ret;
	}
	
	return (XmlNode*)0;
}

/*!
 *******************************************************************************
 *	�����ӽ��
 *******************************************************************************
 */
XmlNode* XmlNode::createChildNode( const char* name )
{
	XmlNode* ret = new XmlNode;
	if( !ret->create(name,this))
	{
		delete ret;
		ret = 0;
	}
	
	return ret;
}

/*!
 *******************************************************************************
 *	�Ƴ��ӽ��
 *******************************************************************************
 */
bool XmlNode::destroyChildNode( XmlNode* xn )
{
	if( xn )
	{
		NODE_ITERATOR it = m_ChildNodes.begin();
		while( it != m_ChildNodes.end())
		{
			if( xn == (*it) )
			{
				//>	release���Զ��Ӹ������ɾ���Լ������Բ���
				//>	�������ٽ���ɾ������
				XmlNode* pNode = *it;
				pNode->release();
				delete pNode;

				return true;
			}
			
			it++;
		}
	}
	
	return false;
}

/*!
 *******************************************************************************
 *	�Ƴ����е��ӽ��
 *******************************************************************************
 */
void XmlNode::destroyAllChildNode( void )
{
	NODE_ITERATOR it = m_ChildNodes.begin();	
	while ( it != m_ChildNodes.end() )
	{
		//>	release���Զ��Ӹ������ɾ���Լ������Բ���
		//>	�������ٽ���ɾ������
		XmlNode* pNode = *it;
		pNode->release();
		delete pNode;

		it = m_ChildNodes.begin();
	}
}

}//namesapce