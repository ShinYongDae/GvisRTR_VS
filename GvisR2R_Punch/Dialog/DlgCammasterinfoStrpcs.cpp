
// DlgCammasterinfoStrpcs.cpp : 구현 파일
//

#include "stdafx.h"
#include "../GvisR2R_Punch.h"
#include "DlgCammasterinfoStrpcs.h"
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


// CDlgCammasterinfoStrpcs 대화 상자



CDlgCammasterinfoStrpcs::CDlgCammasterinfoStrpcs(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_DLG_CAMMASTERINFO_STRPCS, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CDlgCammasterinfoStrpcs::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CDlgCammasterinfoStrpcs, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON1, &CDlgCammasterinfoStrpcs::OnBnClickedButton1)
END_MESSAGE_MAP()


// CDlgCammasterinfoStrpcs 메시지 처리기

BOOL CDlgCammasterinfoStrpcs::OnInitDialog()
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

	CString sPath, sMstLoc, sModel, sLayer;
	sMstLoc = pDoc->m_Master[0].GetMasterLocation();
	sModel = pDoc->m_Master[0].GetModel();
	sLayer = pDoc->m_Master[0].GetLayer();

	if (sMstLoc.Right(1) != "\\")
		sPath.Format(_T("%s\\%s\\%s\\strpcs.bin"), sMstLoc, sModel, sLayer);
	else
		sPath.Format(_T("%s%s\\%s\\strpcs.bin"), sMstLoc, sModel, sLayer);

	GetDlgItem(IDC_STATIC_PATH)->SetWindowText(sPath);


	return TRUE;  // 포커스를 컨트롤에 설정하지 않으면 TRUE를 반환합니다.
}

//void CDlgCammasterinfoStrpcs::OnSysCommand(UINT nID, LPARAM lParam)
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

void CDlgCammasterinfoStrpcs::OnPaint()
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
HCURSOR CDlgCammasterinfoStrpcs::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


BOOL CDlgCammasterinfoStrpcs::FileBrowse(CString& sPath)
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
	static TCHAR BASED_CODE szFilter[] = _T("Mst Files (*.bin)|*.bin|All Files (*.*)|*.*||");

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


void CDlgCammasterinfoStrpcs::AllocPolygonRgnData(CPoint**& PolygonPoints, int nCornerNum, int PieceRgnNum)
{
	int i;
	CPoint	**PolygonPointsLocal = NULL;

	FreePolygonRgnData(PolygonPointsLocal);

	PolygonPointsLocal = (CPoint **)malloc(sizeof(CPoint*) * MAX_PIECE_RGN_NUM);
	int nRefinePieceNum = max(1, PieceRgnNum);
	for (i = 0; i < nRefinePieceNum; i++)
		PolygonPointsLocal[i] = (CPoint *)malloc(sizeof(CPoint) * nCornerNum);

	for (i = nRefinePieceNum; i < MAX_PIECE_RGN_NUM; i++)
	{
		PolygonPointsLocal[i] = NULL;
	}

	PolygonPoints = PolygonPointsLocal;
}

void CDlgCammasterinfoStrpcs::FreePolygonRgnData(CPoint	**PolygonPoints)
{
	int i;
	if (PolygonPoints != NULL)
	{
		for (i = 0; i < MAX_PIECE_RGN_NUM; i++)
		{
			if (PolygonPoints[i] != NULL)
			{
				free(PolygonPoints[i]);
				PolygonPoints[i] = NULL;
			}
		}

		free(PolygonPoints);
		PolygonPoints = NULL;
	}
}

CString sName[25] =
{
	_T("nFrameSize"),								// 0.
	_T("PieceRgnNum"), 								// 1.
	_T("nCornerNum"),								// 2.
	_T("nDummy[0]"), 								// 3.	
	_T("nDummy[1]"), 								// 4	
	_T("nDummy[2]"), 								// 5.		
	_T("nDummy[3]"), 								// 6.	
	_T("nDummy[4]"), 								// 7.	
	_T("FrameRgnID.nId"),						 	// 8.
	_T("FrameRgnID.Row"),							// 9.
	_T("FrameRgnID.Col"),							// 10.
	_T("nDummy[5]"),								// 11.
	_T("FrameRgnPix.iStartX"),						// 12.
	_T("FrameRgnPix.iStartY"),						// 13.
	_T("FrameRgnPix.iEndX"),						// 14.
	_T("FrameRgnPix.iEndY"),						// 15.
	_T("FrameRgnPix.FMirror"),						// 16.
	_T("FrameRgnPix.FRotate"),						// 17.
	_T("nPieceNum"), 								// 18.
	_T("PieceRgnPix.nId"),							// 19.
	_T("PieceRgnPix.Row"),							// 20.
	_T("PieceRgnPix.Col"),							// 21.
	_T("nDummy[6]"),								// 22.
	_T("PieceRgnPix.PolygonPoint"),					// 23. 
	_T("PieceRgnPix.FRotate")						// 24.
};

BOOL CDlgCammasterinfoStrpcs::LoadStrpcsFromCam(CString& sPath, CString& sData)
{
	m_nCol = 0;
	m_nRow = 0;

	CString str = _T("");
	sData = _T("");
	int nN = 0;
	int nTemp = 0;

	//short FrameRgnNum;
	//int PieceRgnNum;
	int nCornerNum;
	int nDummy[MAX_PATH];
	//int nPieceNum[MAX_PATH];

	REGIONS_FRAME FrameRgnPix[MAX_FRAME_RGN_NUM];
	//REGIONS_FRAME_ID FrameRgnID[MAX_FRAME_RGN_NUM];
	//REGIONS_PIECE_2 PieceRgnPix[MAX_PIECE_RGN_NUM];

	//TCHAR		FileNCam[1024];
	CString		strFileNCam;
	CFileFind	find;

	// 1. Find File "strpcs.rgn"

	if (!find.FindFile(sPath))
	{
		AfxMessageBox(_T("StrPcs.bin파일을 찾지못했습니다."));
		return(FALSE);
	}

	// 2. Read Strip & Piece Info
	int		i, j, k, Dummy;
	int		PieceNum, PieceCount;
	CFile	file;
	TCHAR	FileNLoc[1024];
	_stprintf(FileNLoc, _T("%s"), sPath);

	if (!file.Open(FileNLoc, CFile::modeRead))
	{
		if (!file.Open(FileNLoc, CFile::modeRead))
		{
			AfxMessageBox(_T("캠마스터에 피스정보파일을 Open하지 못했습니다."));
			return(FALSE);
		}
	}

	int nFrameSize = 0;

	file.Read((void *)&nFrameSize, sizeof(int));
	FrameRgnNum = nFrameSize;

	file.Read((void *)&PieceRgnNum, sizeof(int));
	file.Read((void *)&nCornerNum, sizeof(int));
	file.Read((void *)&nDummy[0], sizeof(int));	// reserved
	file.Read((void *)&nDummy[1], sizeof(int));	// reserved
	file.Read((void *)&nDummy[2], sizeof(int));	// reserved
	file.Read((void *)&nDummy[3], sizeof(int));	// reserved
	file.Read((void *)&nDummy[4], sizeof(int));	// reserved

	str.Format(_T("%s: %d"), sName[nN], FrameRgnNum); nN++;
	sData += str;
	sData += _T("\r\n");

	str.Format(_T("%s: %d"), sName[nN], PieceRgnNum); nN++;
	sData += str;
	sData += _T("\r\n");

	str.Format(_T("%s: %d"), sName[nN], nCornerNum); nN++;
	sData += str;
	sData += _T("\r\n");

	str.Format(_T("%s: %d"), sName[nN], nDummy[0]); nN++;
	sData += str;
	sData += _T("\r\n");

	str.Format(_T("%s: %d"), sName[nN], nDummy[1]); nN++;
	sData += str;
	sData += _T("\r\n");

	str.Format(_T("%s: %d"), sName[nN], nDummy[2]); nN++;
	sData += str;
	sData += _T("\r\n");

	str.Format(_T("%s: %d"), sName[nN], nDummy[3]); nN++;
	sData += str;
	sData += _T("\r\n");

	str.Format(_T("%s: %d"), sName[nN], nDummy[4]); nN++;
	sData += str;
	sData += _T("\r\n");

	// Init. Buffers
	sData += _T("\r\n");
	for (i = 0; i < FrameRgnNum; i++)
	{
		FrameRgnPix[i].iStartX = 0;
		FrameRgnPix[i].iStartY = 0;
		FrameRgnPix[i].iEndX = 0;
		FrameRgnPix[i].iEndY = 0;
		FrameRgnPix[i].FRotate = 0;
		FrameRgnPix[i].FMirror = 0;
		FrameRgnID[i].Col = -1;
		FrameRgnID[i].Row = -1;
	}

	for (i = 0; i < MAX_PIECE_RGN_NUM; i++)
	{
		PieceRgnPix[i].iStartX = 0;
		PieceRgnPix[i].iStartY = 0;
		PieceRgnPix[i].iEndX = 0;
		PieceRgnPix[i].iEndY = 0;
		PieceRgnPix[i].FRotate = 0;
		PieceRgnPix[i].FMirror = 0;

		//PieceRgnID[i].nHeight = 0; //140204lgh
		//PieceRgnID[i].nWidth = 0; //140204 lgh

		PieceRgnPix[i].Col = -1;
		PieceRgnPix[i].Row = -1;
	}

	if (PieceRgnNum > MAX_PIECE_RGN_NUM)
	{
		file.Close();
		AfxMessageBox(_T("캠마스터에서 설정한 피스 갯수가 최대치를 초과했습니다."));
		return FALSE;
	}

	//start for previous pcs info.
	CRect rtFrm(10000, 10000, 0, 0);
	double fData1, fData2, fData3, fData4;
	long lTop = 0;
	int nCol = 0, nRow = 0;
	int Size = 0;
	int nPieceRgnNum = 0;

	int nMaxR = 0, nMaxC = 0;
	//double mmPxl = MasterInfo.dPixelSize / 1000.0; // [mm]
	//end for previous pcs info.


	CPoint	**PolygonPoints = NULL;
	AllocPolygonRgnData(PolygonPoints, nCornerNum, PieceRgnNum);

	PieceCount = 0;
	for (j = 0; j < FrameRgnNum; j++)
	{
		// 4. Set Strip Info.
		file.Read((void *)&FrameRgnID[j].nId, sizeof(int));						// Strip ID

		//file.Read((void *)&nDummy, sizeof(int));			// reserved
		//file.Read((void *)&nDummy, sizeof(int));			// reserved
		file.Read((void *)&FrameRgnID[j].Row, sizeof(int));						// Row
		file.Read((void *)&FrameRgnID[j].Col, sizeof(int));						// Col

		file.Read((void *)&nDummy[5], sizeof(int));							// Rotation Info (0 : 0  1 : 90  2 : 180  3 : 270 [Degree])
		file.Read((void *)&(FrameRgnPix[j]), sizeof(REGIONS_FRAME));			// (int * 6)

		// 5. Set Piece Info.
		file.Read((void *)&nPieceNum[j], sizeof(int));


		str.Format(_T("%d-%s: %d"), j, sName[nN + 0], FrameRgnID[j].nId);
		sData += str;
		sData += _T("\r\n");

		str.Format(_T("%d-%s: %d"), j, sName[nN + 1], FrameRgnID[j].Row);
		sData += str;
		sData += _T("\r\n");

		str.Format(_T("%d-%s: %d"), j, sName[nN + 2], FrameRgnID[j].Col);
		sData += str;
		sData += _T("\r\n");

		str.Format(_T("%d-%s: %d"), j, sName[nN + 3], nDummy[5]);
		sData += str;
		sData += _T("\r\n");

		/*
		int iStartX, iStartY;
		int iEndX, iEndY;
		int FMirror;	//0 : 원본 1 : 상하미러  2 : 좌우미러
		int FRotate;	//0 : 0도  1 : 90도  2 : 180도  3 : 270도
		*/
		str.Format(_T("%d-%s: %d"), j, sName[nN + 4], FrameRgnPix[j].iStartX);
		sData += str;
		sData += _T("\r\n");

		str.Format(_T("%d-%s: %d"), j, sName[nN + 5], FrameRgnPix[j].iStartY);
		sData += str;
		sData += _T("\r\n");

		str.Format(_T("%d-%s: %d"), j, sName[nN + 6], FrameRgnPix[j].iEndX);
		sData += str;
		sData += _T("\r\n");

		str.Format(_T("%d-%s: %d"), j, sName[nN + 7], FrameRgnPix[j].iEndY);
		sData += str;
		sData += _T("\r\n");

		str.Format(_T("%d-%s: %d"), j, sName[nN + 8], FrameRgnPix[j].FMirror);
		sData += str;
		sData += _T("\r\n");

		str.Format(_T("%d-%s: %d"), j, sName[nN + 9], FrameRgnPix[j].FRotate);
		sData += str;
		sData += _T("\r\n");


		str.Format(_T("%d-%s: %d"), j, sName[nN + 10], nPieceNum[j]);
		sData += str;
		sData += _T("\r\n");

		if(nPieceNum[j] > 0)
			sData += _T("\r\n");

		for (i = 0; i < nPieceNum[j]; i++)
		{
			file.Read((void *)&PieceRgnPix[i + PieceCount].nId, sizeof(int));	// Piece ID
			file.Read((void *)&PieceRgnPix[i + PieceCount].Row, sizeof(int));	// Row
			file.Read((void *)&PieceRgnPix[i + PieceCount].Col, sizeof(int));	// Col
			//file.Read((void *)&nDummy[6 + i + PieceCount], sizeof(int));		// Rotation Info (0 : 0  1 : 90  2 : 180  3 : 270 [Degree])
			file.Read((void *)&nTemp, sizeof(int));

			PieceRgnPix[i + PieceCount].nId += PieceCount;					// Piece ID : Panel 별로 (0 ~ 시작)

			str.Format(_T("%d-%s: %d"), i, sName[nN + 11], PieceRgnPix[i + PieceCount].nId);
			sData += str;
			sData += _T("\r\n");

			str.Format(_T("%d-%s: %d"), i, sName[nN + 12], PieceRgnPix[i + PieceCount].Row);
			sData += str;
			sData += _T("\r\n");

			str.Format(_T("%d-%s: %d"), i, sName[nN + 13], PieceRgnPix[i + PieceCount].Col);
			sData += str;
			sData += _T("\r\n");

			str.Format(_T("%d-%s: %d"), i, sName[nN + 14], nDummy[6 + i + PieceCount]);
			sData += str;
			sData += _T("\r\n");

			if (nMaxC < PieceRgnPix[i + PieceCount].Col)
				nMaxC = PieceRgnPix[i + PieceCount].Col;

			if (nMaxR < PieceRgnPix[i + PieceCount].Row)
				nMaxR = PieceRgnPix[i + PieceCount].Row;
			////////////////////////////////////////////////////////////////////////////////////////////////
			// Set Piece position
			Dummy = sizeof(CPoint);
			file.Read((void *)PolygonPoints[i + PieceCount], sizeof(CPoint)*nCornerNum);
			file.Read((void *)&(PieceRgnPix[i + PieceCount].FRotate), sizeof(int));

			// Set PieceRgnPix
			PieceRgnPix[i + PieceCount].iStartX = PolygonPoints[i + PieceCount][0].x;
			PieceRgnPix[i + PieceCount].iStartY = PolygonPoints[i + PieceCount][0].y;
			PieceRgnPix[i + PieceCount].iEndX = PolygonPoints[i + PieceCount][0].x;
			PieceRgnPix[i + PieceCount].iEndY = PolygonPoints[i + PieceCount][0].y;
			for (k = 0; k < nCornerNum; k++)
			{
				PieceRgnPix[i + PieceCount].iStartX = min(PieceRgnPix[i + PieceCount].iStartX, PolygonPoints[i + PieceCount][k].x);
				PieceRgnPix[i + PieceCount].iStartY = min(PieceRgnPix[i + PieceCount].iStartY, PolygonPoints[i + PieceCount][k].y);
				PieceRgnPix[i + PieceCount].iEndX = max(PieceRgnPix[i + PieceCount].iEndX, PolygonPoints[i + PieceCount][k].x);
				PieceRgnPix[i + PieceCount].iEndY = max(PieceRgnPix[i + PieceCount].iEndY, PolygonPoints[i + PieceCount][k].y);

				str.Format(_T("%d-%s: %d, %d"), i, sName[nN + 15], PolygonPoints[i + PieceCount][k].x, PolygonPoints[i + PieceCount][k].y);
				sData += str;
				sData += _T("\r\n");
			}

			str.Format(_T("%d-%s: %d"), i, sName[nN + 16], PieceRgnPix[i + PieceCount].FRotate);
			sData += str;
			sData += _T("\r\n");
		}
		sData += _T("\r\n");
		PieceCount += nPieceNum[j];

	}// for(j = 0; j < FrameRgnNum; j++)
	sData += _T("\r\n");
	nN = nN + 17;

	 //start for previous pcs info.
	nPieceRgnNum = PieceRgnNum;

	if (nPieceRgnNum > 0)
	{
		//if (pPcsRgn)
		//	delete pPcsRgn;
		//pPcsRgn = new CPcsRgn(nPieceRgnNum);
	}
	else
	{
		file.Close();
		Size = 0;
		FreePolygonRgnData(PolygonPoints);

		AfxMessageBox(_T("캠마스터에서 설정한 피스 갯수가 없습니다."));
		return(FALSE);
	}

	for (i = 0; i < nPieceRgnNum; i++)
	{

		fData1 = (double)PieceRgnPix[i].iStartX;
		fData2 = (double)PieceRgnPix[i].iStartY;
		fData3 = (double)PieceRgnPix[i].iEndX;
		fData4 = (double)PieceRgnPix[i].iEndY;
		//fData1 = (double)PieceRgnPix[i].iStartX * mmPxl;
		//fData2 = (double)PieceRgnPix[i].iStartY * mmPxl;
		//fData3 = (double)PieceRgnPix[i].iEndX * mmPxl;
		//fData4 = (double)PieceRgnPix[i].iEndY * mmPxl;
		//pPcsRgn->pPcs[i] = CRect(fData1, fData2, fData3, fData4);// (left, top, right, bottom)
		//pPcsRgn->pCenter[i] = CfPoint((fData1 + fData3) / 2.0, (fData2 + fData4) / 2.0);

		//if (i > 0)
		//{
		//	if (lTop == pPcsRgn->pPcs[i].top)
		//		nCol++;
		//}
		//else
		//	lTop = pPcsRgn->pPcs[i].top;

		//if (pPcsRgn->pPcs[i].left < rtFrm.left)
		//	rtFrm.left = pPcsRgn->pPcs[i].left;
		//if (pPcsRgn->pPcs[i].top < rtFrm.top)
		//	rtFrm.top = pPcsRgn->pPcs[i].top;
		//if (pPcsRgn->pPcs[i].right > rtFrm.right)
		//	rtFrm.right = pPcsRgn->pPcs[i].right;
		//if (pPcsRgn->pPcs[i].bottom > rtFrm.bottom)
		//	rtFrm.bottom = pPcsRgn->pPcs[i].bottom;
	}

	//nCol++;

	//pPcsRgn->nCol = nCol;
	//pPcsRgn->nRow = nPieceRgnNum / nCol;

	double dWidth = rtFrm.right - rtFrm.left;
	double dHeight = rtFrm.bottom - rtFrm.top;
	double dMarginW = dWidth * 0.05;
	double dMarginH = dHeight * 0.05;
	double dMargin = (dMarginW > dMarginH) ? dMarginH : dMarginW;

	//pPcsRgn->rtFrm.left = rtFrm.left - dMargin;
	//pPcsRgn->rtFrm.top = rtFrm.top - dMargin;
	//pPcsRgn->rtFrm.right = rtFrm.right + dMargin;
	//pPcsRgn->rtFrm.bottom = rtFrm.bottom + dMargin;
	//end for previous pcs info.

	file.Close();

	FreePolygonRgnData(PolygonPoints);

	m_nRow = nMaxR * FrameRgnNum;
	m_nCol = nMaxC;

	return TRUE;
}

void CDlgCammasterinfoStrpcs::OnBnClickedButton1()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CString sPath;

	if (!FileBrowse(sPath))
		return;
	GetDlgItem(IDC_STATIC_PATH)->SetWindowText(sPath);

	CString sData = _T("");
	CFileFind	find;
	//sPath = _T("\\\\GM-STORAGE2\\MasterData-PCB\\C1iNRefer\\TOP\\1TS_TEST-025\\strpcs.bin");
	sPath = _T("\\\\gm-storage2\\MasterData-PCB\\C1INRefer\\BTM\\1BS-TEST-025\\strpcs.bin");
	if (!find.FindFile(sPath))
	{
		AfxMessageBox(_T("캠마스터에 피스정보가 설정되지 않았습니다."));
		return;
	}

	if (!LoadStrpcsFromCam(sPath, sData))
		AfxMessageBox(_T("LoadStripRgnFromCam failed!"));
	GetDlgItem(IDC_EDIT1)->SetWindowText(sData);

	SetMasterPanelInfo();
	InitOrederingMk();
	GetDlgItem(IDC_EDIT2)->SetWindowText(WriteOrederingMk());
}

void CDlgCammasterinfoStrpcs::SetShotRowCol(int nR, int nC)
{
	m_nRow = nR;
	m_nCol = nC;
}


void CDlgCammasterinfoStrpcs::GetShotRowCol(int& nR, int& nC)
{
	nR = m_nRow;
	nC = m_nCol;
}

int CDlgCammasterinfoStrpcs::GetTotPcs()
{
	return PieceRgnNum;
}

void CDlgCammasterinfoStrpcs::SetMasterPanelInfo()
{
	//if (!m_pPcsRgn)
	//	return;

	int i, j, k;
	int nR, nC, nRow, nCol, nSMaxR, nSMaxC, nPMaxR, nPMaxC;
	int nPieceCount = 0;
	int nMstPcsIdx = -1;

	//double mmPxl = MasterInfo.dPixelSize / 1000.0; // [mm]

	MstPnl.nTotalStrip = FrameRgnNum;
	MstPnl.nTotalPiece = PieceRgnNum;
	//MstPnl.nPcsCorner = m_nCornerNum;
	//MstPnl.nTotalAlignPos = MasterInfo.nNumOfAlignPoint;

	//CRect rt = m_pPcsRgn->GetShotRgn();
	//MstPnl.Area.dLeft = rt.left;		// [mm]
	//MstPnl.Area.dTop = rt.top;		// [mm]
	//MstPnl.Area.dRight = rt.right;	// [mm]
	//MstPnl.Area.dBottom = rt.bottom;	// [mm]
										//MstPnl.Area.dLeft = m_pPcsRgn->rtFrm.left;		// [mm]
										//MstPnl.Area.dTop = m_pPcsRgn->rtFrm.top;		// [mm]
										//MstPnl.Area.dRight = m_pPcsRgn->rtFrm.right;	// [mm]
										//MstPnl.Area.dBottom = m_pPcsRgn->rtFrm.bottom;	// [mm]

	nSMaxR = 0; nSMaxC = 0;
	for (j = 0; j < FrameRgnNum; j++)
	{
		nRow = FrameRgnID[j].Row - 1; // Cammaster Row, Col : (1, 1) 부터시작
		nCol = FrameRgnID[j].Col - 1; // Cammaster Row, Col : (1, 1) 부터시작
		if (nSMaxR < nRow) nSMaxR = nRow;
		if (nSMaxC < nCol) nSMaxC = nCol;

		MstPnl.Strip[nRow].nMstStripIdx = FrameRgnID[j].nId;

		//MstPnl.Strip[nRow].Area.dLeft = (double)FrameRgnPix[j].iStartX * mmPxl;
		//MstPnl.Strip[nRow].Area.dTop = (double)FrameRgnPix[j].iStartY * mmPxl;
		//MstPnl.Strip[nRow].Area.dRight = (double)FrameRgnPix[j].iEndX * mmPxl;
		//MstPnl.Strip[nRow].Area.dBottom = (double)FrameRgnPix[j].iEndY * mmPxl;

		MstPnl.Strip[nRow].nTotalPiece = nPieceNum[j];
		nPMaxR = 0; nPMaxC = 0;
		for (i = 0; i < nPieceNum[j]; i++)
		{
			nR = PieceRgnPix[i + nPieceCount].Row - 1; // Cammaster Row, Col : (1, 1) 부터시작
			nC = PieceRgnPix[i + nPieceCount].Col - 1; // Cammaster Row, Col : (1, 1) 부터시작
			if (nPMaxR < nR) nPMaxR = nR;
			if (nPMaxC < nC) nPMaxC = nC;

			nMstPcsIdx = PieceRgnPix[i + nPieceCount].nId;												// Cammaster ID : 0 부터시작
			MstPnl.Strip[nRow].Piece[nR][nC].nMstPcsIdx = nMstPcsIdx;									// Cammaster ID : 0 부터시작
			MstPnl.Strip[nRow].Piece[nR][nC].nMstStripIdx = FrameRgnID[j].nId;							// Cammaster ID : 0 부터시작
			MstPnl.Strip[nRow].Piece[nR][nC].nMstStripRow = nRow;										// Cammaster ID : 0 부터시작
			MstPnl.Strip[nRow].Piece[nR][nC].nMstStripCol = nCol;										// Cammaster ID : 0 부터시작
			MstPnl.Strip[nRow].Piece[nR][nC].nMstPcsRow = nR;											// Cammaster ID : 0 부터시작
			MstPnl.Strip[nRow].Piece[nR][nC].nMstPcsCol = nC;											// Cammaster ID : 0 부터시작
			//MstPnl.Strip[nRow].Piece[nR][nC].Area.dLeft = PieceRgnPix[i + nPieceCount].iStartX * mmPxl;
			//MstPnl.Strip[nRow].Piece[nR][nC].Area.dTop = PieceRgnPix[i + nPieceCount].iStartY * mmPxl;
			//MstPnl.Strip[nRow].Piece[nR][nC].Area.dRight = PieceRgnPix[i + nPieceCount].iEndX * mmPxl;
			//MstPnl.Strip[nRow].Piece[nR][nC].Area.dBottom = PieceRgnPix[i + nPieceCount].iEndY * mmPxl;

			MstPnl.Piece[nMstPcsIdx].nMstPcsIdx = nMstPcsIdx;									// Cammaster ID : 0 부터시작
			MstPnl.Piece[nMstPcsIdx].nMstStripIdx = FrameRgnID[j].nId;							// Cammaster ID : 0 부터시작
			MstPnl.Piece[nMstPcsIdx].nMstStripRow = nRow;										// Cammaster ID : 0 부터시작
			MstPnl.Piece[nMstPcsIdx].nMstStripCol = nCol;										// Cammaster ID : 0 부터시작
			MstPnl.Piece[nMstPcsIdx].nMstPcsRow = nR;											// Cammaster ID : 0 부터시작
			MstPnl.Piece[nMstPcsIdx].nMstPcsCol = nC;											// Cammaster ID : 0 부터시작
			//MstPnl.Piece[nMstPcsIdx].Area.dLeft = PieceRgnPix[i + nPieceCount].iStartX * mmPxl;	// Cammaster ID : 0 부터시작
			//MstPnl.Piece[nMstPcsIdx].Area.dTop = PieceRgnPix[i + nPieceCount].iStartY * mmPxl;	// Cammaster ID : 0 부터시작
			//MstPnl.Piece[nMstPcsIdx].Area.dRight = PieceRgnPix[i + nPieceCount].iEndX * mmPxl;	// Cammaster ID : 0 부터시작
			//MstPnl.Piece[nMstPcsIdx].Area.dBottom = PieceRgnPix[i + nPieceCount].iEndY * mmPxl;	// Cammaster ID : 0 부터시작
		}
		nPieceCount += nPieceNum[j];

		MstPnl.Strip[nRow].nTotalPieceCol = nPMaxC + 1;
		MstPnl.Strip[nRow].nTotalPieceRow = nPMaxR + 1;
	}

	MstPnl.nTotalStripCol = nSMaxC + 1;
	MstPnl.nTotalStripRow = nSMaxR + 1;
/*
	CString sPath;
	if (MasterInfo.strMasterLocation.Right(1) != "\\")
		sPath.Format(_T("%s\\%s\\%s\\Piece.tif"), MasterInfo.strMasterLocation, m_sModel, m_sLayer);
	else
		sPath.Format(_T("%s%s\\%s\\Piece.tif"), MasterInfo.strMasterLocation, m_sModel, m_sLayer);
	MstPnl.sPathPcsImg = sPath;

	if (m_sPathCamSpecDir.Right(1) != "\\")
		sPath.Format(_T("%s\\%s\\%s.TIF"), m_sPathCamSpecDir, m_sModel, m_sLayer);
	else
		sPath.Format(_T("%s%s\\%s.TIF"), m_sPathCamSpecDir, m_sModel, m_sLayer);
	MstPnl.sPathPinImg = sPath;

	for (i = 0; i < 4; i++)
	{
		if (m_sPathCamSpecDir.Right(1) != "\\")
			sPath.Format(_T("%s\\%s\\%s-md%d.tif"), m_sPathCamSpecDir, m_sModel, m_sLayer, i);
		else
			sPath.Format(_T("%s%s\\%s-md%d.tif"), m_sPathCamSpecDir, m_sModel, m_sLayer, i);

		MstPnl.sPathAlignImg[i] = sPath;
	}

	if (m_sPathCamSpecDir.Right(1) != "\\")
		sPath.Format(_T("%s\\%s\\%s.pch"), m_sPathCamSpecDir, m_sModel, m_sLayer);
	else
		sPath.Format(_T("%s%s\\%s.pch"), m_sPathCamSpecDir, m_sModel, m_sLayer);
	MstPnl.sPath2ptAlignAndMkPos = sPath;

	if (m_sPathCamSpecDir.Right(1) != "\\")
		sPath.Format(_T("%s\\%s\\%s.pch2"), m_sPathCamSpecDir, m_sModel, m_sLayer);
	else
		sPath.Format(_T("%s%s\\%s.pch2"), m_sPathCamSpecDir, m_sModel, m_sLayer);
	MstPnl.sPath4ptAlignAndMkPos = sPath;

	if (m_sPathCamSpecDir.Right(1) != "\\")
		sPath.Format(_T("%s\\%s\\%s.ini"), m_sPathCamSpecDir, m_sModel, m_sLayer);
	else
		sPath.Format(_T("%s%s\\%s.ini"), m_sPathCamSpecDir, m_sModel, m_sLayer);
	MstPnl.sPathPinPos = sPath; // [ORIGIN COORD] (원점): PX, PY ; (검사영역 좌상): MX, MY
								// [PANEL INFO] (검사영역 W, H): InspectionWidth, InspectionHeight
*/
}

BOOL CDlgCammasterinfoStrpcs::GetMkMatrix(int nPcsId, int &nC, int &nR)	// nC:0~ , nR:0~
{
	BOOL bRtn = FALSE;
	int nStrip, nPMaxR;

	//if (!m_pPcsRgn)
	//{
	//	AfxMessageBox(_T("m_pPcsRgn is NULL on GetMkMatrix()"));
	//	return bRtn;
	//}

	//if (pDoc->WorkingInfo.System.bStripPcsRgnBin)
	{
		nStrip = MstPnl.Piece[nPcsId].nMstStripRow;
		nPMaxR = MstPnl.Strip[0].nTotalPieceRow;
		nR = MstPnl.Piece[nPcsId].nMstPcsRow + nStrip * nPMaxR;
		nC = MstPnl.Piece[nPcsId].nMstPcsCol;
	}
	//else
	//	bRtn = m_pPcsRgn->GetMkMatrix(nPcsId, nC, nR);

	return bRtn;
}

void CDlgCammasterinfoStrpcs::InitOrederingMk()
{
	int nPcsIdx, nCol, nRow, nInc, nRrev;
	int nArrangTable[MAX_PCE_ROW][MAX_PCE_COL] = { -1 };
	//int nTotPcs = GetTotPcs();
	int nTotPcs = PieceRgnNum;

	int nNodeY, nNodeX;
	GetShotRowCol(nNodeY, nNodeX);

	//if (pDoc->WorkingInfo.System.bStripPcsRgnBin)
	{
		for (nPcsIdx = 0; nPcsIdx < nTotPcs; nPcsIdx++)						// 상면 총 피스 수
		{
			GetMkMatrix(nPcsIdx, nCol, nRow);
			nArrangTable[nRow][nCol] = nPcsIdx;								// ArrangTable에 불량 피스의 인덱스를 펼쳐 놓음.
		}

		nInc = 0;															// 마킹순서 인덱스
		for (nCol = 0; nCol < nNodeX; nCol++)
		{
			for (nRow = 0; nRow < nNodeY; nRow++)
			{
				if (nCol % 2)												// NodeY방향으로 인덱스가 증가하도록 정렬할 때 
				{
					nRrev = nNodeY - nRow - 1;
					if (nArrangTable[nRrev][nCol] > -1)
					{
						m_MkOrder2PnlPcsIdx[nInc] = nArrangTable[nRrev][nCol];	// Y축 -방향(nRow 감소방향)으로 마킹순서의 피스 인덱스를 정렬 : 마킹순서인덱스(nInc)별 CamMaster의 피스인덱스(nArrangTable[nRow][nCol])
						m_PnlPcsIdx2MkOrder[m_MkOrder2PnlPcsIdx[nInc]] = nInc;		// CamMaster의 피스인덱스(m_MkOrder2PnlPcsIdx[nInc])에 해당하는 마킹순서인덱스(nInc)
						nInc++;
					}
				}
				else														// NodeY방향으로 인덱스가 감소하도록 정렬할 때 
				{
					if (nArrangTable[nRow][nCol] > -1)
					{
						m_MkOrder2PnlPcsIdx[nInc] = nArrangTable[nRow][nCol];		// Y축 +방향(nRow 증가방향)으로 마킹순서의 피스 인덱스를 정렬 : 마킹순서인덱스(nInc)별 CamMaster의 피스인덱스(nArrangTable[nRow][nCol])
						m_PnlPcsIdx2MkOrder[m_MkOrder2PnlPcsIdx[nInc]] = nInc;		// CamMaster의 피스인덱스(m_MkOrder2PnlPcsIdx[nInc])에 해당하는 마킹순서인덱스(nInc)
						nInc++;
					}
				}
			}
		}
	}
	//else
	//{
	//	for (nPcsIdx = 0; nPcsIdx < nTotPcs; nPcsIdx++)						// 상면 총 피스 수
	//	{
	//		m_MkOrder2PnlPcsIdx[nPcsIdx] = nPcsIdx;
	//		m_PnlPcsIdx2MkOrder[m_MkOrder2PnlPcsIdx[nPcsIdx]] = nPcsIdx;
	//	}
	//}
}

CString CDlgCammasterinfoStrpcs::WriteOrederingMk()
{
	CFile file;
	CFileException pError;
	if (!file.Open(PATH_ORDERING_Mk, CFile::modeWrite, &pError))
	{
		if (!file.Open(PATH_ORDERING_Mk, CFile::modeCreate | CFile::modeWrite, &pError))
		{
			// 파일 오픈에 실패시 
#ifdef _DEBUG
			afxDump << _T("File could not be opened ") << pError.m_cause << _T("\n");
#endif
			return _T("");
		}
	}

	CString sTemp = _T(""), sData = _T("");
	//int nTotPcs = GetTotPcs();
	int nTotPcs = PieceRgnNum;
	int nMkIdx, nNodeY, nNodeX;
	GetShotRowCol(nNodeY, nNodeX);

	//for (nMkIdx = 0; nMkIdx < nTotPcs; nMkIdx++)						// 상면 총 피스 수
	//{
	//	int nCol = int(nMkIdx / nNodeY);
	//	if(!(nCol % 2))
	//		sTemp.Format(_T("%d\t"), m_MkOrder2PnlPcsIdx[nMkIdx]);
	//	else
	//	{
	//		int nRevIdx = 2 * (nNodeY*nCol) + nNodeY - (nMkIdx+1);
	//		sTemp.Format(_T("%d\t"), m_MkOrder2PnlPcsIdx[nRevIdx]);
	//	}

	//	sData += sTemp;
	//	if (!((nMkIdx + 1) % (nNodeY/MAX_STRIP_NUM)) && nMkIdx)
	//		sData += _T("\t");
	//	if (!((nMkIdx+1)%nNodeY) && nMkIdx)
	//		sData += _T("\r\n"); 
	//}
	for (nMkIdx = nTotPcs - 1; nMkIdx >= 0; nMkIdx--)					// 상면 총 피스 수
	{
		int nCol = int(nMkIdx / nNodeY);
		if (nNodeY % 2)
		{
			if (!(nCol % 2))
				sTemp.Format(_T("%d\t"), m_MkOrder2PnlPcsIdx[nMkIdx]);
			else
			{
				int nRevIdx = 2 * (nNodeY*nCol) + nNodeY - (nMkIdx + 1);
				sTemp.Format(_T("%d\t"), m_MkOrder2PnlPcsIdx[nRevIdx]);
			}
		}
		else
		{
			if ((nCol % 2))
				sTemp.Format(_T("%d\t"), m_MkOrder2PnlPcsIdx[nMkIdx]);
			else
			{
				int nRevIdx = 2 * (nNodeY*nCol) + nNodeY - (nMkIdx + 1);
				sTemp.Format(_T("%d\t"), m_MkOrder2PnlPcsIdx[nRevIdx]);
			}
		}

		sData += sTemp;
		if (!((nMkIdx) % (nNodeY / MAX_STRIP_NUM)) && nMkIdx)
			sData += _T("\t");
		if (!((nMkIdx) % nNodeY))
			sData += _T("\r\n");
	}

	//버퍼의 내용을 file에 복사한다.
	char* pRtn = NULL;
	file.SeekToBegin();
	file.Write(pRtn = StringToChar(sData), sData.GetLength());
	file.Close();
	delete pRtn;

	return sData;
}

void CDlgCammasterinfoStrpcs::StringToChar(CString str, char *szStr)
{
	int nLen = str.GetLength();
	strcpy(szStr, CT2A(str));
	szStr[nLen] = _T('\0');
}


char* CDlgCammasterinfoStrpcs::StringToChar(CString str) // char* returned must be deleted... 
{
	char*		szStr = NULL;
	wchar_t*	wszStr;
	int				nLenth;

	USES_CONVERSION;
	//1. CString to wchar_t* conversion
	wszStr = T2W(str.GetBuffer(str.GetLength()));

	//2. wchar_t* to char* conversion
	nLenth = WideCharToMultiByte(CP_ACP, 0, wszStr, -1, NULL, 0, NULL, NULL); //char* 형에 대한길이를 구함 
	szStr = new char[nLenth];  //메모리 할당 

							   //3. wchar_t* to char* conversion
	WideCharToMultiByte(CP_ACP, 0, wszStr, -1, szStr, nLenth, 0, 0);
	return szStr;
}
