
// SFFPlayer.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CSFFPlayerApp:
// �йش����ʵ�֣������ SFFPlayer.cpp
//

class CSFFPlayerApp : public CWinApp
{
public:
	CSFFPlayerApp();

// ��д
public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CSFFPlayerApp theApp;