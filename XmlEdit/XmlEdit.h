// XmlEdit.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error �ڰ������� PCH �Ĵ��ļ�֮ǰ������stdafx.h��
#endif

#include "resource.h"		// ������


// CXmlEditApp:
// �йش����ʵ�֣������ XmlEdit.cpp
//

class CXmlEditApp : public CWinApp
{
public:
	CXmlEditApp();

// ��д
	public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CXmlEditApp theApp;
