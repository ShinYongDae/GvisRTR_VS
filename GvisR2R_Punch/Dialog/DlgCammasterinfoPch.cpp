
// DlgCammasterinfoPch.cpp : 구현 파일
//

#include "stdafx.h"
#include "../GvisR2R_Punch.h"
#include "DlgCammasterinfoPch.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


#include "../MainFrm.h"
#include "../GvisR2R_PunchDoc.h"
#include "../GvisR2R_PunchView.h"

extern CMainFrame* pFrm;
extern CGvisR2R_PunchDoc* pDoc;
extern CGvisR2R_PunchView* pView;

//// 응용 프로그램 정보에 사용되는 CAboutDlg 대화 상자입니다.
//
//class CAboutDlg : public CDialogEx
//{
//public:
//	CAboutDlg();
//
//// 대화 상자 데이터입니다.
//#ifdef AFX_DESIGN_TIME
//	enum { IDD = IDD_ABOUTBOX };
//#endif
//
//	protected:
//	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.
//
//// 구현입니다.
//protected:
//	DECLARE_MESSAGE_MAP()
//};
//
//CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
//{
//}
//
//void CAboutDlg::DoDataExchange(CDataExchange* pDX)
//{
//	CDialogEx::DoDataExchange(pDX);
//}
//
//BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
//END_MESSAGE_MAP()


// CDlgCammasterinfoPch 대화 상자



CDlgCammasterinfoPch::CDlgCammasterinfoPch(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_DLG_CAMMASTERINFO_PCH, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CDlgCammasterinfoPch::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CDlgCammasterinfoPch, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON1, &CDlgCammasterinfoPch::OnBnClickedButton1)
END_MESSAGE_MAP()


// CDlgCammasterinfoPch 메시지 처리기

BOOL CDlgCammasterinfoPch::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	//// 시스템 메뉴에 "정보..." 메뉴 항목을 추가합니다.

	//// IDM_ABOUTBOX는 시스템 명령 범위에 있어야 합니다.
	//ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	//ASSERT(IDM_ABOUTBOX < 0xF000);

	//CMenu* pSysMenu = GetSystemMenu(FALSE);
	//if (pSysMenu != NULL)
	//{
	//	BOOL bNameValid;
	//	CString strAboutMenu;
	//	bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
	//	ASSERT(bNameValid);
	//	if (!strAboutMenu.IsEmpty())
	//	{
	//		pSysMenu->AppendMenu(MF_SEPARATOR);
	//		pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
	//	}
	//}

	//// 이 대화 상자의 아이콘을 설정합니다.  응용 프로그램의 주 창이 대화 상자가 아닐 경우에는
	////  프레임워크가 이 작업을 자동으로 수행합니다.
	//SetIcon(m_hIcon, TRUE);			// 큰 아이콘을 설정합니다.
	//SetIcon(m_hIcon, FALSE);		// 작은 아이콘을 설정합니다.

	//// TODO: 여기에 추가 초기화 작업을 추가합니다.

	CString sPath, sCamSpecLoc, sModel, sLayer;
	sCamSpecLoc = pDoc->WorkingInfo.System.sPathCamSpecDir;
	sModel = pDoc->m_Master[0].GetModel();
	sLayer = pDoc->m_Master[0].GetLayer();

	if (sCamSpecLoc.Right(1) != "\\")
		sPath.Format(_T("%s\\%s\\%s.pch"), sCamSpecLoc, sModel, sLayer);
	else
		sPath.Format(_T("%s%s\\%s.pch"), sCamSpecLoc, sModel, sLayer);

	GetDlgItem(IDC_STATIC_PATH)->SetWindowText(sPath);


	return TRUE;  // 포커스를 컨트롤에 설정하지 않으면 TRUE를 반환합니다.
}

//void CDlgCammasterinfoPch::OnSysCommand(UINT nID, LPARAM lParam)
//{
//	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
//	{
//		CAboutDlg dlgAbout;
//		dlgAbout.DoModal();
//	}
//	else
//	{
//		CDialogEx::OnSysCommand(nID, lParam);
//	}
//}

// 대화 상자에 최소화 단추를 추가할 경우 아이콘을 그리려면
//  아래 코드가 필요합니다.  문서/뷰 모델을 사용하는 MFC 응용 프로그램의 경우에는
//  프레임워크에서 이 작업을 자동으로 수행합니다.

void CDlgCammasterinfoPch::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 그리기를 위한 디바이스 컨텍스트입니다.

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 클라이언트 사각형에서 아이콘을 가운데에 맞춥니다.
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 아이콘을 그립니다.
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// 사용자가 최소화된 창을 끄는 동안에 커서가 표시되도록 시스템에서
//  이 함수를 호출합니다.
HCURSOR CDlgCammasterinfoPch::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


BOOL CDlgCammasterinfoPch::FileBrowse(CString& sPath)
{
	sPath = _T("");

	/* Load from file */
	CString FilePath;
	CFileFind finder;
	CString SrchPath, strTitleMsg, strErrMsg;

	CWaitCursor mCursor;

	CString DirPath[10];
	CString strWorkDir;
	BOOL bResult;

	CString strMcNum;
	int nAoiMachineNum = 0;

	// File Open Filter 
	static TCHAR BASED_CODE szFilter[] = _T("Pch Files (*.pch)|*.pch|Pch2 Files (*.pch2)|*.pch2|All Files (*.*)|*.*||");

	// CFileDialog 
	CFileDialog dlg(TRUE, NULL, NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, szFilter, NULL);

	// Win2k Style FileDialog Box
	dlg.m_ofn.lStructSize = sizeof(OPENFILENAME) + 12; // +12를 Win2k Style로 다이얼로그 박스가 Open됨.

													   // Open Directory
	//TCHAR strPrevDir[MAX_PATH];
	//DWORD dwLength = GetCurrentDirectory(MAX_PATH, strPrevDir);
	//strWorkDir = strPrevDir;
	//dlg.m_ofn.lpstrInitialDir = strWorkDir;

	GetDlgItem(IDC_STATIC_PATH)->GetWindowText(sPath);
	int nPos = sPath.ReverseFind('\\');
	if (nPos > 0)
		sPath = sPath.Left(nPos);
	dlg.m_ofn.lpstrInitialDir = sPath;

	bResult = 0;

	// Dialog Open
	if (dlg.DoModal() == IDOK)
	{
		sPath = FilePath = dlg.GetPathName();
		return TRUE;
	}

	return FALSE;
}

BOOL CDlgCammasterinfoPch::LoadPchFromCam(CString sPath, CString& sData)
{
	CFileFind findfile;

	if (sPath.Find(_T(".pch2")) >= 0)	// find 4PointAlign file.
	{
		return LoadCadMk4PntAlign(sPath, sData);
	}
	else									// find 2PointAlign file.
	{
		return LoadCadMk2PntAlign(sPath, sData);
	}

	return FALSE;
}

BOOL CDlgCammasterinfoPch::LoadCadMk2PntAlign(CString sPath, CString& sData)
{
	CString sName[4] =
	{
		_T("stAlignMk0"),							// 0. Pos(x, y)
		_T("stAlignMk1"), 							// 1. Pos(x, y)
		_T("nPcsNum"),								// 2. num
		_T("stPcsMk"), 								// 3. Pos(x, y)	
	};

	CFile file;
	int i;
	int nRet = 0;
	int nSzAlignMk = 0, nSzPcsMk = 0;


	stAlignMark stAlignMk;
	stPieceMark stPcsMk[MAX_PCS];
	int nPcsNum = 0;

	// Mark File Load
	if (file.Open((LPCTSTR)sPath, CFile::modeRead))
	{
		CString str = _T("");
		sData = _T("");
		int nN = 0;

		nSzAlignMk = sizeof(stAlignMark);

		nRet = file.Read((void *)&stAlignMk, nSzAlignMk); // stAlignMk0, stAlignMk1
		if (nRet != nSzAlignMk)
		{
			file.Close();
			return(FALSE);
		}

		str.Format(_T("%s: %0.6f, %0.6f"), sName[nN], stAlignMk.X0, stAlignMk.Y0); nN++;
		sData += str;
		sData += _T("\r\n");

		str.Format(_T("%s: %0.6f, %0.6f"), sName[nN], stAlignMk.X1, stAlignMk.Y1); nN++;
		sData += str;
		sData += _T("\r\n");

		nRet = file.Read((void *)&nPcsNum, sizeof(int));	// nPcsNum
		if (nRet != sizeof(int))
		{
			file.Close();
			return(FALSE);
		}

		str.Format(_T("%s: %d"), sName[nN], nPcsNum); nN++;
		sData += str;
		sData += _T("\r\n");
		sData += _T("\r\n");

		nSzPcsMk = nPcsNum * sizeof(stPieceMark);
		nRet = file.Read((void *)&stPcsMk, nSzPcsMk);
		if (nRet != nSzPcsMk)
		{
			file.Close();
			return(FALSE);
		}

		file.Close();

		for(i=0; i<nPcsNum; i++)
		{
			str.Format(_T("%s[%d]: %0.6f, %0.6f"), sName[nN], i, stPcsMk[i].X, stPcsMk[i].Y);
			sData += str;
			sData += _T("\r\n");
		}

		return(TRUE);
	}

	return(FALSE);
}

BOOL CDlgCammasterinfoPch::LoadCadMk4PntAlign(CString sPath, CString& sData)
{
	CString sName[6] =
	{
		_T("stAlignMk0"),							// 0. Pos(x, y)
		_T("stAlignMk1"), 							// 1. Pos(x, y)
		_T("stAlignMk2"),							// 2. Pos(x, y)
		_T("stAlignMk3"), 							// 3. Pos(x, y)
		_T("nPcsNum"),								// 4. num
		_T("stPcsMk"), 								// 5. Pos(x, y)	
	};

	CFile file;
	int i;
	int nRet = 0;
	int nSzAlignMk = 0, nSzPcsMk = 0;


	stAlignMark2 stAlignMk;
	stPieceMark stPcsMk[MAX_PCS];
	int nPcsNum = 0;

	// Mark File Load
	if (file.Open((LPCTSTR)sPath, CFile::modeRead))
	{
		CString str = _T("");
		sData = _T("");
		int nN = 0;

		nSzAlignMk = sizeof(stAlignMark2);

		nRet = file.Read((void *)&stAlignMk, nSzAlignMk);
		if (nRet != nSzAlignMk)
		{
			file.Close();
			return(FALSE);
		}

		str.Format(_T("%s: %0.6f, %0.6f"), sName[nN], stAlignMk.X0, stAlignMk.Y0); nN++;
		sData += str;
		sData += _T("\r\n");

		str.Format(_T("%s: %0.6f, %0.6f"), sName[nN], stAlignMk.X1, stAlignMk.Y1); nN++;
		sData += str;
		sData += _T("\r\n");

		str.Format(_T("%s: %0.6f, %0.6f"), sName[nN], stAlignMk.X2, stAlignMk.Y2); nN++;
		sData += str;
		sData += _T("\r\n");

		str.Format(_T("%s: %0.6f, %0.6f"), sName[nN], stAlignMk.X3, stAlignMk.Y3); nN++;
		sData += str;
		sData += _T("\r\n");

		nRet = file.Read((void *)&nPcsNum, sizeof(int));
		if (nRet != sizeof(int))
		{
			file.Close();
			return(FALSE);
		}

		str.Format(_T("%s: %d"), sName[nN], nPcsNum); nN++;
		sData += str;
		sData += _T("\r\n");
		sData += _T("\r\n");

		nSzPcsMk = nPcsNum * sizeof(stPieceMark);
		nRet = file.Read((void *)&stPcsMk, nSzPcsMk);
		if (nRet != nSzPcsMk)
		{
			file.Close();
			return(FALSE);
		}

		file.Close();

		for (i = 0; i < nPcsNum; i++)
		{
			str.Format(_T("%s[%d]: %0.6f, %0.6f"), sName[nN], i, stPcsMk[i].X, stPcsMk[i].Y);
			sData += str;
			sData += _T("\r\n");
		}

		return(TRUE);
	}
	else
		return(FALSE);
}

void CDlgCammasterinfoPch::OnBnClickedButton1()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CString sPath;

	if (!FileBrowse(sPath))
		return;
	GetDlgItem(IDC_STATIC_PATH)->SetWindowText(sPath);

	CString sData = _T("");
	if (!LoadPchFromCam(sPath, sData))
		AfxMessageBox(_T("LoadPchFromCam failed!"));
	GetDlgItem(IDC_EDIT1)->SetWindowText(sData);
}
