// FileBridge.h: interface for the CFileBridge class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FILEBRIDGE_H__C8173EDB_BB08_4F1B_9F84_BF3F9F7254C3__INCLUDED_)
#define AFX_FILEBRIDGE_H__C8173EDB_BB08_4F1B_9F84_BF3F9F7254C3__INCLUDED_

#include "xml/XmlFileBridge.h"

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CFileBridge:public xnd::XmlFileBridge 
{
public:

	explicit CFileBridge(CFile* pFile);
	virtual ~CFileBridge();

	virtual int	 length( void ) const; 
	virtual int	 getPosition( void ) const;
	virtual void setPosition( int pos );
	virtual bool read( void* out,int len );
	virtual bool write( const void* in,int len );

protected:
	CFile* m_pFile;

};

#endif // !defined(AFX_FILEBRIDGE_H__C8173EDB_BB08_4F1B_9F84_BF3F9F7254C3__INCLUDED_)
