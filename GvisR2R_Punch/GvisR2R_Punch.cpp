
// GvisR2R_Punch.cpp : ���� ���α׷��� ���� Ŭ���� ������ �����մϴ�.
//

#include "stdafx.h"
#include "afxwinappex.h"
#include "afxdialogex.h"


#include "GvisR2R_Punch.h"
#include "MainFrm.h"

#include "GvisR2R_PunchDoc.h"
#include "GvisR2R_PunchView.h"

#include <tlhelp32.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CGvisR2R_PunchApp

BEGIN_MESSAGE_MAP(CGvisR2R_PunchApp, CWinApp)
	ON_COMMAND(ID_APP_ABOUT, &CGvisR2R_PunchApp::OnAppAbout)
	// ǥ�� ������ ���ʷ� �ϴ� ���� ����Դϴ�.
	ON_COMMAND(ID_FILE_NEW, &CWinApp::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, &CWinApp::OnFileOpen)
END_MESSAGE_MAP()


// CGvisR2R_PunchApp ����

CGvisR2R_PunchApp::CGvisR2R_PunchApp()
{
	// �ٽ� ���� ������ ����
	m_dwRestartManagerSupportFlags = AFX_RESTART_MANAGER_SUPPORT_ALL_ASPECTS;
#ifdef _MANAGED
	// ���� ���α׷��� ���� ��� ��Ÿ�� ������ ����Ͽ� ������ ���(/clr):
	//     1) �� �߰� ������ �ٽ� ���� ������ ������ ����� �۵��ϴ� �� �ʿ��մϴ�.
	//     2) ������Ʈ���� �����Ϸ��� System.Windows.Forms�� ���� ������ �߰��ؾ� �մϴ�.
	System::Windows::Forms::Application::SetUnhandledExceptionMode(System::Windows::Forms::UnhandledExceptionMode::ThrowException);
#endif

	// TODO: �Ʒ� ���� ���α׷� ID ���ڿ��� ���� ID ���ڿ��� �ٲٽʽÿ�(����).
	// ���ڿ��� ���� ����: CompanyName.ProductName.SubProduct.VersionInformation
	SetAppID(_T("GvisR2R_Punch.AppID.NoVersion"));

	// TODO: ���⿡ ���� �ڵ带 �߰��մϴ�.
	// InitInstance�� ��� �߿��� �ʱ�ȭ �۾��� ��ġ�մϴ�.

}

CGvisR2R_PunchApp::~CGvisR2R_PunchApp()
{
	wsaEndup();
	Sleep(300);

	//_CrtSetReportMode(_CRT_WARN, _CRTDBG_MODE_DEBUG);	// �޸� ���� ���� ��� - ��� â���� �ٽ� ���𷺼�
	//_CrtDumpMemoryLeaks();								// �޸� ���� ���� ���
}

// ������ CGvisR2R_PunchApp ��ü�Դϴ�.

CGvisR2R_PunchApp theApp;

DWORD CGvisR2R_PunchApp::KillProcess(CString strProcName)
{
	HANDLE         hProcessSnap = NULL;
	DWORD          Return = FALSE;
	PROCESSENTRY32 pe32 = { 0 };
	CString strMsg;
	DWORD nCurPID = GetCurrentProcessId();
	hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (hProcessSnap == INVALID_HANDLE_VALUE)
		return (DWORD)INVALID_HANDLE_VALUE;
	pe32.dwSize = sizeof(PROCESSENTRY32);
	if (Process32First(hProcessSnap, &pe32))
	{
		DWORD Code = 0;
		DWORD dwPriorityClass;
		do
		{
			CString name = CString(pe32.szExeFile);

			if (name.CompareNoCase(_T("V3UI.exe")) == 0)
				continue;

			if (name.CompareNoCase(_T("V3APRule.exe")) == 0)
				continue;

			HANDLE hProcess;
			// Get the actual priority class. 
			hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pe32.th32ProcessID);
			dwPriorityClass = GetPriorityClass(hProcess);

			if (nCurPID == pe32.th32ProcessID)
			{
				CloseHandle(hProcess);
				continue;
			}

			CString Temp = pe32.szExeFile;
			Temp.MakeLower();
			strProcName.MakeLower();
			if (Temp == strProcName)
			{
				int checkval = 0;
#ifndef _WIN64
				DWORD ret;
#else
				DWORD_PTR ret;
#endif
				checkval = SendMessageTimeout((HWND)hProcess, WM_NULL, 0,//�� �޼����� ��������
					0, SMTO_ABORTIFHUNG | SMTO_BLOCK, 500, &ret);
				if (checkval == 0)
				{
					//Ÿ�� �ƿ��̳� ��Ÿ ������ �߻���
					int kk = GetLastError();
					strMsg = "Delete ";
					strMsg += strProcName;
					strMsg += " Process";
					//	AfxMessageBox(strMsg);
					if (TerminateProcess(hProcess, 0))
						GetExitCodeProcess(hProcess, &Code);
					else
						return Return;
				}
				else
				{
					CString str;
					str.Format(_T("On running %s Process "), Temp);
					//AfxMessageBox(str);
					OutputDebugString(str);
					if (TerminateProcess(hProcess, 0))
						GetExitCodeProcess(hProcess, &Code);
					else
						return Return;
				}
			}
			if (!Temp.Compare(_T("vcspawn.exe")))
			{	// �̰��� �ܼ� â���� ������ ���δ�
				if (TerminateProcess(hProcess, 0))
					GetExitCodeProcess(hProcess, &Code);
				else
					return GetLastError();
			}
			CloseHandle(hProcess);
		} while (Process32Next(hProcessSnap, &pe32));
		Return = TRUE;
	}
	else
		Return = FALSE; // could not walk the list of processes 

	CloseHandle(hProcessSnap); // Do not forget to clean up the snapshot object. 
	return Return;
}

// CGvisR2R_PunchApp �ʱ�ȭ

BOOL CGvisR2R_PunchApp::InitInstance()
{
	if (!m_singleInstance.Create(IDR_MAINFRAME))
	{
		AfxMessageBox(_T("GvisR2R is already running!!"));
		return FALSE;
	}

	wsaStartup();

	// ���� ���α׷� �Ŵ��佺Ʈ�� ComCtl32.dll ���� 6 �̻��� ����Ͽ� ���־� ��Ÿ����
	// ����ϵ��� �����ϴ� ���, Windows XP �󿡼� �ݵ�� InitCommonControlsEx()�� �ʿ��մϴ�. 
	// InitCommonControlsEx()�� ������� ������ â�� ���� �� �����ϴ�.
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// ���� ���α׷����� ����� ��� ���� ��Ʈ�� Ŭ������ �����ϵ���
	// �� �׸��� �����Ͻʽÿ�.
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();


	// OLE ���̺귯���� �ʱ�ȭ�մϴ�.
	if (!AfxOleInit())
	{
		AfxMessageBox(IDP_OLE_INIT_FAILED);
		return FALSE;
	}

	AfxEnableControlContainer();

	EnableTaskbarInteraction(FALSE);

	// RichEdit ��Ʈ���� ����Ϸ���  AfxInitRichEdit2()�� �־�� �մϴ�.	
	// AfxInitRichEdit2();

	// ǥ�� �ʱ�ȭ
	// �̵� ����� ������� �ʰ� ���� ���� ������ ũ�⸦ ���̷���
	// �Ʒ����� �ʿ� ���� Ư�� �ʱ�ȭ
	// ��ƾ�� �����ؾ� �մϴ�.
	// �ش� ������ ����� ������Ʈ�� Ű�� �����Ͻʽÿ�.
	// TODO: �� ���ڿ��� ȸ�� �Ǵ� ������ �̸��� ����
	// ������ �������� �����ؾ� �մϴ�.
	SetRegistryKey(_T("���� ���� ���α׷� �����翡�� ������ ���� ���α׷�"));
	LoadStdProfileSettings(4);  // MRU�� �����Ͽ� ǥ�� INI ���� �ɼ��� �ε��մϴ�.


	// ���� ���α׷��� ���� ���ø��� ����մϴ�.  ���� ���ø���
	//  ����, ������ â �� �� ������ ���� ������ �մϴ�.
	CSingleDocTemplate* pDocTemplate;
	pDocTemplate = new CSingleDocTemplate(
		IDR_MAINFRAME,
		RUNTIME_CLASS(CGvisR2R_PunchDoc),
		RUNTIME_CLASS(CMainFrame),       // �� SDI ������ â�Դϴ�.
		RUNTIME_CLASS(CGvisR2R_PunchView));
	if (!pDocTemplate)
		return FALSE;
	AddDocTemplate(pDocTemplate);


	// ǥ�� �� ���, DDE, ���� ���⿡ ���� ������� ���� �м��մϴ�.
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);



	// ����ٿ� ������ ����� ����ġ�մϴ�.
	// ���� ���α׷��� /RegServer, /Register, /Unregserver �Ǵ� /Unregister�� ���۵� ��� FALSE�� ��ȯ�մϴ�.
	if (!ProcessShellCommand(cmdInfo))
		return FALSE;

	// â �ϳ��� �ʱ�ȭ�Ǿ����Ƿ� �̸� ǥ���ϰ� ������Ʈ�մϴ�.
	m_pMainWnd->ShowWindow(SW_SHOWMAXIMIZED);
	m_pMainWnd->UpdateWindow();
	return TRUE;
}

int CGvisR2R_PunchApp::ExitInstance()
{

	//TODO: �߰��� �߰� ���ҽ��� ó���մϴ�.
	AfxOleTerm(FALSE); // MFC ���� Ŭ���� �޸� �κ� ���� �ذ�

	return CWinApp::ExitInstance();
}

// CGvisR2R_PunchApp �޽��� ó����


// ���� ���α׷� ������ ���Ǵ� CAboutDlg ��ȭ �����Դϴ�.

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// ��ȭ ���� �������Դϴ�.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

// �����Դϴ�.
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()

// ��ȭ ���ڸ� �����ϱ� ���� ���� ���α׷� ����Դϴ�.
void CGvisR2R_PunchApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

// CGvisR2R_PunchApp �޽��� ó����


void CGvisR2R_PunchApp::wsaStartup()
{
	WORD wVersionRequested;
	WSADATA wsaData;
	char name[255];
	CString IP;
	PHOSTENT hostinfo;
	wVersionRequested = MAKEWORD(2, 0);

	if (WSAStartup(wVersionRequested, &wsaData) == 0)
	{
		if (gethostname(name, sizeof(name)) == 0)
		{
			if ((hostinfo = gethostbyname(name)) != NULL)
			{
				IP = inet_ntoa(*(struct in_addr*)* hostinfo->h_addr_list);
			}
		}
	}

	m_strHostAddress = IP;
}

void CGvisR2R_PunchApp::wsaEndup()
{
	WSACleanup();
}

CString CGvisR2R_PunchApp::GetHostAddress()
{
	return m_strHostAddress;
}

void CGvisR2R_PunchApp::SetHostAddress(CString sAddr)
{
	m_strHostAddress = sAddr;
}

CString CGvisR2R_PunchApp::GetHostPort()
{
	return m_strHostPort;
}

void CGvisR2R_PunchApp::SetHostPort(CString sPort)
{
	m_strHostPort = sPort;
}

