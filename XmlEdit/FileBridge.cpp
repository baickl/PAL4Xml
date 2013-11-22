// FileBridge.cpp: implementation of the CFileBridge class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "FileBridge.h"

#include <exception>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CFileBridge::CFileBridge(CFile* pFile):m_pFile(pFile)
{
	if( !m_pFile )
		throw std::exception("打开或读取文件失败！");
}

CFileBridge::~CFileBridge()
{
	m_pFile = NULL;
}

int CFileBridge::length(void ) const
{
	return static_cast<int>(m_pFile->GetLength());
}

int CFileBridge::getPosition(void)const
{
	return static_cast<int>(m_pFile->GetPosition());
}

void CFileBridge::setPosition(int pos )
{
	m_pFile->Seek(pos,CFile::begin);
}

bool CFileBridge::read(void* out,int len )
{
	if( m_pFile->Read(out,len) != (UINT)len )
		return false;
	else
		return true;
}

bool CFileBridge::write(const void* in,int len )
{
	m_pFile->Write(in,len);
	return true;
}
