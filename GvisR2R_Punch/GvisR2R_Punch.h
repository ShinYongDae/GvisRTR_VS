
// GvisR2R_Punch.h : GvisR2R_Punch ���� ���α׷��� ���� �� ��� ����
//
#pragma once

#ifndef __AFXWIN_H__
	#error "PCH�� ���� �� ������ �����ϱ� ���� 'stdafx.h'�� �����մϴ�."
#endif

#include "resource.h"       // �� ��ȣ�Դϴ�.

#include "SingleInstance.h"

//== �޸� ���� ���� ��� ========================================================

#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

#ifdef _DEBUG
#define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ )
// Replace _NORMAL_BLOCK with _CLIENT_BLOCK if you want the
// allocations to be of _CLIENT_BLOCK type
#else
#define DBG_NEW new
#endif

//=================================================================================

// CGvisR2R_PunchApp:
// �� Ŭ������ ������ ���ؼ��� GvisR2R_Punch.cpp�� �����Ͻʽÿ�.
//

class CGvisR2R_PunchApp : public CWinApp
{
	CString m_strHostAddress, m_strHostPort;

	void wsaStartup();
	void wsaEndup();

public:
	CGvisR2R_PunchApp();
	~CGvisR2R_PunchApp();

	CSingleInstance m_singleInstance;
	DWORD KillProcess(CString strProcName);

// �������Դϴ�.
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

// �����Դϴ�.
	afx_msg void OnAppAbout();

	CString GetHostAddress();
	CString GetHostPort();
	void SetHostAddress(CString sAddr);
	void SetHostPort(CString sPort);

	DECLARE_MESSAGE_MAP()
};

extern CGvisR2R_PunchApp theApp;
