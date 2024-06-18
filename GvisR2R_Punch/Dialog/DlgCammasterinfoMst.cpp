
// DlgCammasterinfoMst.cpp : 구현 파일
//

#include "stdafx.h"
#include "../GvisR2R_Punch.h"
#include "DlgCammasterinfoMst.h"
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


// CDlgCammasterinfoMst 대화 상자



CDlgCammasterinfoMst::CDlgCammasterinfoMst(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_DLG_CAMMASTERINFO_MST, pParent)
{
	//m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CDlgCammasterinfoMst::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CDlgCammasterinfoMst, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON1, &CDlgCammasterinfoMst::OnBnClickedButton1)
END_MESSAGE_MAP()


// CDlgCammasterinfoMst 메시지 처리기

BOOL CDlgCammasterinfoMst::OnInitDialog()
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

	// TODO: 여기에 추가 초기화 작업을 추가합니다.

	CString sPath, sCamSpecLoc, sModel, sLayer;
	sCamSpecLoc = pDoc->WorkingInfo.System.sPathCamSpecDir;
	sModel = pDoc->m_Master[0].GetModel();
	sLayer = pDoc->m_Master[0].GetLayer();

	if (sCamSpecLoc.Right(1) != "\\")
		sPath.Format(_T("%s\\%s\\%s.mst"), sCamSpecLoc, sModel, sLayer);
	else
		sPath.Format(_T("%s%s\\%s.mst"), sCamSpecLoc, sModel, sLayer);

	GetDlgItem(IDC_STATIC_PATH)->SetWindowText(sPath);

	return TRUE;  // 포커스를 컨트롤에 설정하지 않으면 TRUE를 반환합니다.
}

//void CDlgCammasterinfoMst::OnSysCommand(UINT nID, LPARAM lParam)
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

void CDlgCammasterinfoMst::OnPaint()
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
HCURSOR CDlgCammasterinfoMst::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



BOOL CDlgCammasterinfoMst::FileBrowse(CString& sPath)
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
	static TCHAR BASED_CODE szFilter[] = _T("Mst Files (*.mst)|*.mst|All Files (*.*)|*.*||");

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

BOOL CDlgCammasterinfoMst::LoadStripRgnFromCam(CString sPath, CString& sData)
{
	CString sName[MAX_PATH] = 
	{
		_T("nMode"),									// 0 : Full Mode, 1 : Strip Mode
		_T("nMSwath"), 									// 모터가 움직일 회수.
		_T("NodeNumX"), _T("NodeNumY"),					// 각 축당 셀 갯수
		_T("PanelPixelX"), _T("PanelPixelY"), 			// 전체 판넬 이미지의 크기		
		_T("OvrXPix"), _T("OvrYPix"),					// 각 축으로 오버랩되는 부분의 크기		 
		_T("ProcSizeX"), _T("ProcSizeY"),				// 각 축당 셀의 크기	 
		_T("nCell"),									// 전체 셀 갯수
		_T("nIPU"),										// AOI의 IPU 갯수
		_T("CameraValidPixel"), 						// 실제 검사하기 위해 사용되는 이미지의 X축 크기
		_T("nScanMarginX"), _T("nScanMarginY"), 		// 실제 사용되는 이미지에 각 축방향으로 더해지는 여분 이미지의 크기 Ex) nScanMarginX = (8192 ? CameraValidPixel)/2
		_T("nRegionX"), _T("nRegionY"), 				// Reserved Data
		_T("nCellDivideX"), _T("nCellDivideY"), 		// 한 스트립당 각 축 방향 셀 갯수
		_T("nCADPinPosPixX"), _T("nCADPinPosPixY"), 	// Pin Position x, y
		_T("nProcMarginX"), _T("nProcMarginY"),			// Reserved Data
		_T("StPosX"),									// 각 셀의 왼쪽 윗부분의 X 위치
		_T("StPosY"),									// 각 셀의 왼쪽 윗부분의 Y 위치
		_T("EdPosX"),									// 각 셀의 오른쪽 아래부분의 X 위치
		_T("EdPosY"), 									// 각 셀의 오른쪽 아래부분의 Y 위치
		_T("XSwathPixPos")								// 모터가 움직이기 시작하는 위치
	};

	CFile file;
	int Rsize, RsizeStPosX, RsizeStPosY, RsizeEdPosX, RsizeEdPosY, RsizeXSwathPixPos;
	int SizeI, SizeIXSwathPixPos, i, nData, nN;
	//char FileNCam[200];
	TCHAR FileNCam[200];

	nN = 0;
	sData = _T("");
	CString str;

	//strcpy(FileNCam, sPath);
	wsprintf(FileNCam, TEXT("%s"), sPath);

	if (!file.Open(FileNCam, CFile::modeRead))
	{
		if (!file.Open(FileNCam, CFile::modeRead))
		{
			return(FALSE);
		}
	}

	REGION_STRIP* pCellRgn = new REGION_STRIP;

	SizeI = sizeof(int) * RGN_STRIP_VARIABLE_NUM;
	Rsize = file.Read((void *)pCellRgn, SizeI);

	if (Rsize != SizeI)
	{
		AfxMessageBox(_T("MST File is incorrected."), MB_ICONSTOP | MB_OK);
		return(FALSE);
	}

	for (i = 0; i < RGN_STRIP_VARIABLE_NUM; i++)
	{
		//SizeI = sizeof(int);
		//Rsize = file.Read((void *)pCellRgn, SizeI);
		nData = *((&pCellRgn->nMode) + i);
		//str.Format(_T("%d"), nData);
		str.Format(_T("%s: %d"), sName[nN], nData); nN++;
		sData += str;
		sData += _T("\r\n");
	}
	sData += _T("\r\n");

	int nCell = pCellRgn->NodeNumX * pCellRgn->NodeNumY;
	SizeI = sizeof(int) * nCell;
	RsizeStPosX = file.Read((void *)&pCellRgn->StPosX, SizeI);
	RsizeStPosY = file.Read((void *)&pCellRgn->StPosY, SizeI);
	RsizeEdPosX = file.Read((void *)&pCellRgn->EdPosX, SizeI);
	RsizeEdPosY = file.Read((void *)&pCellRgn->EdPosY, SizeI);

	for (i = 0; i < nCell; i++)
	{
		nData = *((pCellRgn->StPosX) + i);
		//str.Format(_T("%d"), nData);
		str.Format(_T("%s: %d"), sName[nN], nData);
		sData += str;
		sData += _T("\r\n");
	}
	sData += _T("\r\n"); nN++;

	for (i = 0; i < nCell; i++)
	{
		nData = *((pCellRgn->StPosY) + i);
		//str.Format(_T("%d"), nData);
		str.Format(_T("%s: %d"), sName[nN], nData);
		sData += str;
		sData += _T("\r\n");
	}
	sData += _T("\r\n"); nN++;

	for (i = 0; i < nCell; i++)
	{
		nData = *((pCellRgn->EdPosX) + i);
		//str.Format(_T("%d"), nData);
		str.Format(_T("%s: %d"), sName[nN], nData);
		sData += str;
		sData += _T("\r\n");
	}
	sData += _T("\r\n"); nN++;

	for (i = 0; i < nCell; i++)
	{
		nData = *((pCellRgn->EdPosY) + i);
		//str.Format(_T("%d"), nData);
		str.Format(_T("%s: %d"), sName[nN], nData);
		sData += str;
		sData += _T("\r\n");
	}
	sData += _T("\r\n"); nN++;


	SizeIXSwathPixPos = sizeof(int) * pCellRgn->nMSwath;
	RsizeXSwathPixPos = file.Read((void *)&pCellRgn->XSwathPixPos, SizeIXSwathPixPos);

	if (RsizeStPosX != SizeI && RsizeStPosY != SizeI && RsizeEdPosX != SizeI && RsizeEdPosY != SizeI && RsizeXSwathPixPos != SizeIXSwathPixPos)
	{
		AfxMessageBox(_T("MST File is incorrected."),MB_ICONSTOP|MB_OK);
		return(FALSE);
	}
	for (i = 0; i < pCellRgn->nMSwath; i++)
	{
		nData = *((pCellRgn->XSwathPixPos) + i);
		//str.Format(_T("%d"), nData);
		str.Format(_T("%s: %d"), sName[nN], nData);
		sData += str;
		sData += _T("\r\n");
	}
	sData += _T("\r\n");


	if (pCellRgn)
		delete pCellRgn;

	return(TRUE);
}

void CDlgCammasterinfoMst::OnBnClickedButton1()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CString sPath;

	if (!FileBrowse(sPath))
		return;
	GetDlgItem(IDC_STATIC_PATH)->SetWindowText(sPath);

	CString sData;
	if(!LoadStripRgnFromCam(sPath, sData))
		AfxMessageBox(_T("LoadStripRgnFromCam failed!"));
	GetDlgItem(IDC_EDIT1)->SetWindowText(sData);
}
