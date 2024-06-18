
// GvisR2R_PunchView.cpp : CGvisR2R_PunchView Ŭ������ ����
//

#include "stdafx.h"
// SHARED_HANDLERS�� �̸� ����, ����� �׸� �� �˻� ���� ó���⸦ �����ϴ� ATL ������Ʈ���� ������ �� ������
// �ش� ������Ʈ�� ���� �ڵ带 �����ϵ��� �� �ݴϴ�.
#ifndef SHARED_HANDLERS
#include "GvisR2R_Punch.h"
#endif

#include "GvisR2R_PunchDoc.h"
#include "GvisR2R_PunchView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


#include "MainFrm.h"

#include "Dialog/DlgOption01.h"
#include "Dialog/DlgUtil04.h"
#include "Dialog/DlgUtil07.h"
#include "Dialog/DlgMyPassword.h"
#include "Dialog/DlgProgress.h"
#include "Dialog/DlgMyMsgSub00.h"
#include "Dialog/DlgMyMsgSub01.h"
#include "Dialog/DlgMyMsgSub02.h"

#include "Device/SR1000WDEF.h"
#include "Process/DataFile.h"

#include "Dialog/DlgCammasterinfoMst.h"
#include "Dialog/DlgCammasterinfoPch.h"
#include "Dialog/DlgCammasterinfoStrpcs.h"

extern CMainFrame* pFrm;
extern CGvisR2R_PunchDoc* pDoc;
CGvisR2R_PunchView* pView;


// CGvisR2R_PunchView

IMPLEMENT_DYNCREATE(CGvisR2R_PunchView, CFormView)

BEGIN_MESSAGE_MAP(CGvisR2R_PunchView, CFormView)
	ON_WM_TIMER()
	ON_MESSAGE(WM_DLG_INFO, OnDlgInfo)
	ON_MESSAGE(WM_BUF_THREAD_DONE, OnBufThreadDone)
	ON_MESSAGE(WM_MYMSG_EXIT, OnMyMsgExit)
	ON_MESSAGE(WM_CLIENT_RECEIVED_SR, wmClientReceivedSr)
	ON_MESSAGE(WM_CLIENT_RECEIVED, wmClientReceived)
END_MESSAGE_MAP()

// CGvisR2R_PunchView ����/�Ҹ�

CGvisR2R_PunchView::CGvisR2R_PunchView()
	: CFormView(IDD_GVISR2R_PUNCH_FORM)
{
	// TODO: ���⿡ ���� �ڵ带 �߰��մϴ�.
	pView = this;

	int i = 0;
	for (i = 0; i < _SigInx::_EndIdx; i++)
	{
		m_bRcvSig[i] = FALSE;
	}

	m_sAoiUpAlarmReStartMsg = _T(""); m_sAoiDnAlarmReStartMsg = _T("");
	m_sAoiUpAlarmReTestMsg = _T(""); m_sAoiDnAlarmReTestMsg = _T("");

	m_bShift2Mk = FALSE;

	m_bBufEmpty[0] = FALSE;
	m_bBufEmpty[1] = FALSE;

	m_nDebugStep = 0;
	m_nWatiDispMain = 0;
	m_nNewLot = 0;

	m_bStopFromThread = FALSE;
	m_bBuzzerFromThread = FALSE;

	m_nRtnMyMsgBoxIdx = -1;
	m_bDispMyMsgBox = FALSE;
	m_bDispMain = FALSE;
	m_bProbDn[0] = m_bProbDn[1] = FALSE;

	m_nSelRmap = RMAP_UP;
	m_nSelRmapInner = RMAP_INNER_UP;

	m_bTIM_INIT_VIEW = FALSE;
	m_nStepInitView = 0;
	m_nLotEndSerial = 0;

	m_nDummy[0] = 0;
	m_nDummy[1] = 0;
	m_nAoiLastSerial[0] = 0;
	m_nAoiLastSerial[1] = 0;
	m_bChkLastProcVs = FALSE;

	m_dTempPauseLen = 0.0;
	m_dElecChkVal = 0.0;

	m_bInit = FALSE;
	m_bDispMsg = FALSE;
	for (i = 0; i < 10; i++)
	{
		m_bDispMsgDoAuto[i] = FALSE;
		m_nStepDispMsg[i] = 0;
	}
	m_sFixMsg[0] = _T("");
	m_sFixMsg[1] = _T("");

	m_bWaitClrDispMsg = FALSE;
	//m_bOpenShareUp = TRUE;
	//m_bOpenShareDn = TRUE;

	m_bStopFeeding = FALSE;

	// 	m_pMpe = NULL;
	m_pDlgMyMsg = NULL;
	m_pDlgMsgBox = NULL;

	m_pDlgInfo = NULL;
	m_pDlgFrameHigh = NULL;
	m_pDlgMenu01 = NULL;
	m_pDlgMenu02 = NULL;
	m_pDlgMenu03 = NULL;
	m_pDlgMenu04 = NULL;
	m_pDlgMenu05 = NULL;
	m_pDlgMenu06 = NULL;
	//m_pDlgUtil01 = NULL;
	//m_pDlgUtil02 = NULL;
	//m_pDlgUtil03 = NULL;

	m_nCntTowerWinker = 0;
	m_bTimTowerWinker = FALSE;
	m_bTimBtnWinker = FALSE;
	m_bTimBuzzerWarn = FALSE;
	m_bTowerWinker[0] = FALSE; // R
	m_bTowerWinker[1] = FALSE; // G
	m_bTowerWinker[2] = FALSE; // Y
	m_bBtnWinker[0] = FALSE; // Ready
	m_bBtnWinker[1] = FALSE; // Reset
	m_bBtnWinker[2] = FALSE; // Run
	m_bBtnWinker[3] = FALSE; // Stop
	for (i = 0; i < 4; i++)
	{
		m_nCntBtnWinker[i] = 0;
		m_nDlyWinker[i] = 5;
	}

	m_bAoiFdWriteF[0] = FALSE;
	m_bAoiFdWriteF[1] = FALSE;
	m_bAoiTest[0] = FALSE;
	m_bAoiTest[1] = FALSE;

	m_bEngFdWrite = FALSE; 
	m_bEngFdWriteF = FALSE;
	m_bEngTest = FALSE;
	m_bEngTestF = FALSE;


	// H/W Device �ʱ�ȭ.....
	m_pMpe = NULL;
	m_pMotion = NULL;
	m_pVoiceCoil[0] = NULL;
	m_pVoiceCoil[1] = NULL;
	m_pLight = NULL;
#ifdef USE_FLUCK
	m_pFluck = NULL;
#endif

#ifdef USE_VISION
	m_pVision[0] = NULL;
	m_pVision[1] = NULL;
	m_pVisionInner[0] = NULL;
	m_pVisionInner[1] = NULL;
#endif

	m_bTIM_DISP_STATUS = FALSE;

	for (i = 0; i < MAX_THREAD; i++)
	{
		m_bThread[i] = FALSE;
		m_dwThreadTick[i] = 0;
	}

	m_bTIM_MPE_IO = FALSE;


	m_nStepAuto = 0;
	m_nPrevStepAuto = 0;
	m_nPrevMkStAuto = 0;
	m_nStepMk[0] = 0;
	m_nStepMk[1] = 0;

	m_sShare[0] = _T("");
	m_sBuf[0] = _T("");
	m_sShare[1] = _T("");
	m_sBuf[1] = _T("");

	m_sTick = _T("");
	m_sDispTime = _T("");

	m_bChkMpeIoOut = FALSE;

	m_bMkTmpStop = FALSE;
	m_bAoiLdRun = TRUE;
	m_bAoiLdRunF = FALSE;

	m_dwCycSt = 0;
	m_dwCycTim = 0;

	m_bTHREAD_DISP_DEF = FALSE;
	m_nStepTHREAD_DISP_DEF = 0;
	m_bTHREAD_DISP_DEF_INNER = FALSE;
	m_nStepTHREAD_DISP_DEF_INNER = 0;

	m_bTHREAD_UPDATAE_YIELD[0] = FALSE;
	m_bTHREAD_UPDATAE_YIELD[1] = FALSE;
	m_nSerialTHREAD_UPDATAE_YIELD[0] = 0;
	m_nSerialTHREAD_UPDATAE_YIELD[1] = 0;
	//m_bTHREAD_SHIFT2MK[0] = FALSE;
	//m_bTHREAD_SHIFT2MK[1] = FALSE;
	m_bTHREAD_SHIFT2MK = FALSE;
	m_bTHREAD_UPDATE_REELMAP_UP = FALSE;
	m_bTHREAD_UPDATE_REELMAP_ALLUP = FALSE;
	m_bTHREAD_UPDATE_REELMAP_DN = FALSE;
	m_bTHREAD_UPDATE_REELMAP_ALLDN = FALSE;
	m_bTHREAD_UPDATE_REELMAP_ITS = FALSE;
	m_bTHREAD_UPDATE_REELMAP_INNER_UP = FALSE;
	m_bTHREAD_UPDATE_REELMAP_INNER_ALLUP = FALSE;
	m_bTHREAD_UPDATE_REELMAP_INNER_DN = FALSE;
	m_bTHREAD_UPDATE_REELMAP_INNER_ALLDN = FALSE;

	m_nSnTHREAD_UPDATAE_YIELD = 0;

	m_bTHREAD_REELMAP_YIELD_UP = FALSE;
	m_bTHREAD_REELMAP_YIELD_ALLUP = FALSE;
	m_bTHREAD_REELMAP_YIELD_DN = FALSE;
	m_bTHREAD_REELMAP_YIELD_ALLDN = FALSE;
	m_bTHREAD_REELMAP_YIELD_ITS = FALSE;

	m_bTHREAD_UPDATE_YIELD_UP = FALSE;
	m_bTHREAD_UPDATE_YIELD_ALLUP = FALSE;
	m_bTHREAD_UPDATE_YIELD_DN = FALSE;
	m_bTHREAD_UPDATE_YIELD_ALLDN = FALSE;
	m_bTHREAD_UPDATE_YIELD_ITS = FALSE;

	m_bTHREAD_UPDATE_YIELD_INNER_UP = FALSE;
	m_bTHREAD_UPDATE_YIELD_INNER_ALLUP = FALSE;
	m_bTHREAD_UPDATE_YIELD_INNER_DN = FALSE;
	m_bTHREAD_UPDATE_YIELD_INNER_ALLDN = FALSE;

	m_bTHREAD_RELOAD_RST_UP = FALSE;
	m_bTHREAD_RELOAD_RST_ALLUP = FALSE;
	m_bTHREAD_RELOAD_RST_DN = FALSE;
	m_bTHREAD_RELOAD_RST_ALLDN = FALSE;
	m_bTHREAD_RELOAD_RST_ITS = FALSE;
	m_bTHREAD_RELOAD_RST_UP_INNER = FALSE;
	m_bTHREAD_RELOAD_RST_ALLUP_INNER = FALSE;
	m_bTHREAD_RELOAD_RST_DN_INNER = FALSE;
	m_bTHREAD_RELOAD_RST_ALLDN_INNER = FALSE;

	m_bTHREAD_UPDATE_REELMAP_UP_OFFLINE = FALSE;
	m_bTHREAD_UPDATE_REELMAP_ALLUP_OFFLINE = FALSE;
	m_bTHREAD_UPDATE_REELMAP_DN_OFFLINE = FALSE;
	m_bTHREAD_UPDATE_REELMAP_ALLDN_OFFLINE = FALSE;

	m_bTHREAD_LOAD_PCR_UP_OFFLINE = FALSE;
	m_bTHREAD_LOAD_PCR_ALLUP_OFFLINE = FALSE;
	m_bTHREAD_LOAD_PCR_DN_OFFLINE = FALSE;
	m_bTHREAD_LOAD_PCR_ALLDN_OFFLINE = FALSE;

	m_bTHREAD_WRITE_LAST_INFO_REELMAP_UP_OFFLINE = FALSE;
	m_bTHREAD_WRITE_LAST_INFO_REELMAP_DN_OFFLINE = FALSE;
	m_bTHREAD_WRITE_LAST_INFO_REELMAP_ALLUP_OFFLINE = FALSE;
	m_bTHREAD_WRITE_LAST_INFO_REELMAP_ALLDN_OFFLINE = FALSE;

	m_bTHREAD_MK[0] = FALSE;
	m_bTHREAD_MK[1] = FALSE;
	m_bTHREAD_MK[2] = FALSE;
	m_bTHREAD_MK[3] = FALSE;
	m_nMkPcs[0] = 0;
	m_nMkPcs[1] = 0;
	m_nMkPcs[2] = 0;
	m_nMkPcs[3] = 0;

	m_nErrCnt = 0;

	m_bAuto = FALSE;
	m_bManual = FALSE;
	m_bOneCycle = FALSE;

	m_bSwRun = FALSE; m_bSwRunF = FALSE;
	m_bSwStop = FALSE; m_bSwStopF = FALSE;
	m_bSwReset = FALSE; m_bSwResetF = FALSE;
	m_bSwReady = FALSE; m_bSwReadyF = FALSE;

	m_bSwStopNow = FALSE;

	for (int nAxis = 0; nAxis < MAX_AXIS; nAxis++)
		m_dEnc[nAxis] = 0.0;

	for (int i = 0; i < 10; i++)
		m_sDispMsg[i] = _T("");

	m_bNewModel = FALSE;
	m_dTotVel = 0.0; m_dPartVel = 0.0;
	m_bTIM_CHK_TEMP_STOP = FALSE;
	m_bTIM_SAFTY_STOP = FALSE;
	m_bTIM_CAMMASTER_UPDATE = FALSE;
	m_sMyMsg = _T("");
	m_nTypeMyMsg = IDOK;

	m_dwLotSt = 0; m_dwLotEd = 0;

	m_lFuncId = 0;

	m_bDrawGL = TRUE;
	m_bDrawGL_Menu01 = TRUE;
	m_bDrawGL_Menu06 = TRUE;
	m_bCont = FALSE;
	m_bCam = FALSE;
	m_bReview = FALSE;

	m_bChkBufIdx[0] = TRUE;
	m_nChkBufIdx[0] = 0;
	m_bChkBufIdx[1] = TRUE;
	m_nChkBufIdx[1] = 0;

	m_dwStMkDn[0] = 0;
	m_dwStMkDn[1] = 0;
	m_nVsBufLastSerial[0] = 0;
	m_nVsBufLastSerial[1] = 0;
	m_bShowModalMyPassword = FALSE;

	// 	m_bFurgeRun = FALSE;
	// 	m_nFurgeRun = 0;

	m_nRstNum = 0;
	m_bBufHomeDone = FALSE;
	m_bReadyDone = FALSE;

	ResetPriority();
	m_Flag = 0L;
	m_AoiLdRun = 0L;

	m_bCollision[0] = FALSE;
	m_bCollision[1] = FALSE;

	m_bDoneDispMkInfo[0][0] = FALSE; // Cam0, Up
	m_bDoneDispMkInfo[0][1] = FALSE; // Cam0, Dn
	m_bDoneDispMkInfo[1][0] = FALSE; // Cam1, Up
	m_bDoneDispMkInfo[1][1] = FALSE; // Cam1, Dn

	m_nShareUpS = 0; m_nShareUpSprev = 0;
	m_nShareUpSerial[0] = 0;
	m_nShareUpSerial[1] = 0;
	m_nShareUpCnt = 0;

	m_nShareDnS = 0; m_nShareDnSprev = 0;
	m_nShareDnSerial[0] = 0;
	m_nShareDnSerial[1] = 0;
	m_nShareDnCnt = 0;

	m_nBufSerial[0][0] = 0; // Up-Cam0
	m_nBufSerial[0][1] = 0; // Up-Cam1
	m_nBufSerial[1][0] = 0; // Dn-Cam0
	m_nBufSerial[1][1] = 0; // Dn-Cam0

	m_bReAlign[0][0] = FALSE; // [nCam][nPos] 
	m_bReAlign[0][1] = FALSE; // [nCam][nPos] 
	m_bReAlign[0][2] = FALSE; // [nCam][nPos] 
	m_bReAlign[0][3] = FALSE; // [nCam][nPos] 
	m_bReAlign[1][0] = FALSE; // [nCam][nPos] 
	m_bReAlign[1][1] = FALSE; // [nCam][nPos] 
	m_bReAlign[1][2] = FALSE; // [nCam][nPos] 
	m_bReAlign[1][3] = FALSE; // [nCam][nPos] 

	m_bSkipAlign[0][0] = FALSE; // [nCam][nPos] 
	m_bSkipAlign[0][1] = FALSE; // [nCam][nPos] 
	m_bSkipAlign[0][2] = FALSE; // [nCam][nPos] 
	m_bSkipAlign[0][3] = FALSE; // [nCam][nPos] 
	m_bSkipAlign[1][0] = FALSE; // [nCam][nPos] 
	m_bSkipAlign[1][1] = FALSE; // [nCam][nPos] 
	m_bSkipAlign[1][2] = FALSE; // [nCam][nPos] 
	m_bSkipAlign[1][3] = FALSE; // [nCam][nPos] 

	m_bFailAlign[0][0] = FALSE; // [nCam][nPos] 
	m_bFailAlign[0][1] = FALSE; // [nCam][nPos] 
	m_bFailAlign[0][2] = FALSE; // [nCam][nPos] 
	m_bFailAlign[0][3] = FALSE; // [nCam][nPos] 
	m_bFailAlign[1][0] = FALSE; // [nCam][nPos] 
	m_bFailAlign[1][1] = FALSE; // [nCam][nPos] 
	m_bFailAlign[1][2] = FALSE; // [nCam][nPos] 
	m_bFailAlign[1][3] = FALSE; // [nCam][nPos] 

	m_bDoMk[0] = TRUE;
	m_bDoMk[1] = TRUE;
	m_bDoneMk[0] = FALSE;
	m_bDoneMk[1] = FALSE;
	m_bReMark[0] = FALSE;
	m_bReMark[1] = FALSE;

	m_nMonAlmF = 0;
	m_nClrAlmF = 0;

	m_bMkSt = FALSE;
	m_bMkStSw = FALSE;
	m_nMkStAuto = 0;

	m_bEngSt = FALSE;
	m_bEngStSw = FALSE;
	m_nEngStAuto = FALSE;

	m_bEng2dSt = FALSE;
	m_bEng2dStSw = FALSE;
	m_nEng2dStAuto = 0;

	m_bLotEnd = FALSE;
	m_nLotEndAuto = 0;

	m_bLastProc = FALSE;
	m_bLastProcFromUp = TRUE;
	m_bLastProcFromEng = TRUE;
	m_nLastProcAuto = 0;

	m_bLoadShare[0] = FALSE;
	m_bLoadShare[1] = FALSE;

	m_sNewLotUp = _T("");
	m_sNewLotDn = _T("");

	m_bAoiFdWrite[0] = FALSE;
	m_bAoiFdWrite[1] = FALSE;
	m_bAoiFdWriteF[0] = FALSE;
	m_bAoiFdWriteF[1] = FALSE;

	m_bCycleStop = FALSE;

	for (int a = 0; a < 2; a++)
	{
		for (int b = 0; b < 4; b++)
		{
			m_nMkStrip[a][b] = 0;
			m_bRejectDone[a][b] = FALSE;
		}
	}

	m_sDispMain = _T("");

	m_bReMk = FALSE;

	m_bWaitPcr[0] = FALSE;
	m_bWaitPcr[1] = FALSE;

	m_bShowMyMsg = FALSE;
	m_pMyMsgForeground = NULL;
	m_bContDiffLot = FALSE;

	m_nStepElecChk = 0;
	// 	m_nMsgShiftX = 0;
	// 	m_nMsgShiftY = 0;

	for (int nAns = 0; nAns < 10; nAns++)
		m_bAnswer[nAns] = FALSE;

	m_bChkLightErr = FALSE;

	// client for SR-1000W
	m_pSr1000w = NULL;

	// client for engrave
	m_pEngrave = NULL;
	m_pDts = NULL;

	m_bDestroyedView = FALSE;
	m_bContEngraveF = FALSE;

	m_nSaveMk0Img = 0;
	m_nSaveMk1Img = 0;

	m_bStopF_Verify = FALSE;
	m_bInitAuto = TRUE;
	m_bInitAutoLoadMstInfo = FALSE;

	m_bLoadMstInfo = FALSE;
	m_bLoadMstInfoF = FALSE;

	m_sGet2dCodeLot = _T("");
	m_nGet2dCodeSerial = 0;
	m_nReloadReelmapSerial = 0;

	m_bSetSig = FALSE;
	m_bSetSigF = FALSE;
	m_bSetData = FALSE;
	m_bSetDataF = FALSE;

	m_bTIM_START_UPDATE = FALSE;
	m_bEscape = FALSE;
	m_bSerialDecrese = FALSE;

	pView->m_bIsBuf[0] = FALSE;
	pView->m_bIsBuf[1] = FALSE;

	m_bUpdateYield = FALSE;
	m_bUpdateYieldOnRmap = FALSE;
}

CGvisR2R_PunchView::~CGvisR2R_PunchView()
{
	//DestroyView();

#ifdef USE_VISION
	if (m_pVisionInner[1])
	{
		delete m_pVisionInner[1];
		m_pVisionInner[1] = NULL;
	}

	if (m_pVisionInner[0])
	{
		delete m_pVisionInner[0];
		m_pVisionInner[0] = NULL;
	}
	if (m_pVision[1])
	{
		delete m_pVision[1];
		m_pVision[1] = NULL;
	}

	if (m_pVision[0])
	{
		delete m_pVision[0];
		m_pVision[0] = NULL;
	}
#endif

	m_bTIM_MPE_IO = FALSE;
	m_bTIM_DISP_STATUS = FALSE;
	m_bTIM_INIT_VIEW = FALSE;
	Sleep(100);


	InitIoWrite();
	SetMainMc(FALSE);

	// H/W Device �Ҹ�.....
	HwKill();

	// 	if(m_pDlgMyMsg != NULL) 
	// 	{
	// 		m_pDlgMyMsg->DestroyWindow();
	// 		delete m_pDlgMyMsg;
	// 		m_pDlgMyMsg = NULL;
	// 	}
	CloseMyMsg();

	if (m_ArrayMyMsgBox.GetSize() > 0)
	{
		m_ArrayMyMsgBox.RemoveAll();
	}

}

void CGvisR2R_PunchView::DestroyView()
{
	CString sData;
	sData.Format(_T("%d"), m_nMkStAuto);
	::WritePrivateProfileString(_T("Last Job"), _T("MkStAuto"), sData, PATH_WORKING_INFO);

	if (!m_bDestroyedView)
	{
		m_bDestroyedView = TRUE;

		ThreadKill();
		Sleep(30);

		DelAllDlg();
		Sleep(100);

		Buzzer(FALSE, 0);
		Buzzer(FALSE, 1);
	}
}

void CGvisR2R_PunchView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
}

BOOL CGvisR2R_PunchView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: CREATESTRUCT cs�� �����Ͽ� ���⿡��
	//  Window Ŭ���� �Ǵ� ��Ÿ���� �����մϴ�.

	return CFormView::PreCreateWindow(cs);
}

void CGvisR2R_PunchView::OnInitialUpdate()
{
	CFormView::OnInitialUpdate();
	GetParentFrame()->RecalcLayout();
	ResizeParentToFit();

	InitMyMsg();

	pDoc->LoadWorkingInfo();
	pDoc->LoadIoInfo();
	pDoc->LoadSignalInfo();
	pDoc->LoadDataInfo();
	if (!LoadMySpec())
		LoadMySpec();
	pDoc->GetMkInfo();

#ifdef USE_CAM_MASTER
	CFileFind finder;
	CString sDir, sMsg;
	sDir = pDoc->WorkingInfo.System.sPathCamSpecDir;
	sDir.Delete(sDir.GetLength() - 1, 1);
	sDir.ReleaseBuffer();

	//TCHAR szFile[MAX_PATH] = { 0, };
	//_stprintf(szFile, _T("%s\\*.*"), sDir);
	//if (!finder.FindFile(sDir)) // Check 1st Spec folder is

	//if (!finder.FindFile(sDir)) // Check 1st Spec folder is
	if(!pDoc->DirectoryExists(sDir))
	{
		sMsg.Format(_T("ķ�����Ϳ� ���������� �����ϴ�. : \n 1.SpecFolder : %s"), sDir);
		pView->ClrDispMsg();
		AfxMessageBox(sMsg, MB_ICONSTOP | MB_OK);
		ExitProgram();
		return;
	}
#endif

	if (!m_bTIM_INIT_VIEW)
	{
		m_nStepInitView = 0;
		m_bTIM_INIT_VIEW = TRUE;
		SetTimer(TIM_INIT_VIEW, 300, NULL);
	}

	pDoc->SetMonDispMain(_T(""));
}


// CGvisR2R_PunchView ����

#ifdef _DEBUG
void CGvisR2R_PunchView::AssertValid() const
{
	CFormView::AssertValid();
}

void CGvisR2R_PunchView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}

CGvisR2R_PunchDoc* CGvisR2R_PunchView::GetDocument() const // ����׵��� ���� ������ �ζ������� �����˴ϴ�.
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CGvisR2R_PunchDoc)));
	return (CGvisR2R_PunchDoc*)m_pDocument;
}
#endif //_DEBUG


// CGvisR2R_PunchView �޽��� ó����


void CGvisR2R_PunchView::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰� ��/�Ǵ� �⺻���� ȣ���մϴ�.
	int nInc = 0, nSrl = 0;
	CString str, sMsg, sPath, sLot, sLayerUp, sLayerDn;
	BOOL bDualTestInner;
	BOOL bExist = FALSE; 
	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;
	BOOL bRtn = FALSE;

	if (nIDEvent == TIM_INIT_VIEW)
	
	{
		KillTimer(TIM_INIT_VIEW);

		switch (m_nStepInitView)
		{
		case 0:
			m_nStepInitView++;
			DispMsg(_T("���α׷��� �ʱ�ȭ�մϴ�."), _T("�˸�"), RGB_GREEN, DELAY_TIME_MSG);
			DtsInit();

			// H/W Device �ʱ�ȭ.....
			HwInit();

			break;
		case 1:
			m_nStepInitView++;
			InitIO();
			break;
		case 2:
			m_nStepInitView++;
			InitIoWrite();
			SetMainMc(TRUE);
			m_nMonAlmF = 0;
			m_nClrAlmF = 0;
			break;
		case 3:
			m_nStepInitView++;
			ThreadInit();
			break;
		case 4:
			m_nStepInitView++;
 			//if(m_pMotion)
 			//	TowerLamp(RGB_YELLOW, TRUE, TRUE);

			break;
		case 5:
			m_nStepInitView++;
			DispMsg(_T("ȭ�鱸���� �����մϴ�.- MENU_02"), _T("�˸�"), RGB_GREEN, DELAY_TIME_MSG);
			ShowDlg(IDD_DLG_MENU_02);
 			//ShowDlg(IDD_DLG_UTIL_01);
			break;
		case 6:
			m_nStepInitView++;
 			//ShowDlg(IDD_DLG_UTIL_02);
			break;
		case 7:
			m_nStepInitView++;
			//DispMsg(_T("��ŷ��ġ������ ȭ�鱸���� �����մϴ�.- 2"), _T("�˸�"), RGB_GREEN, DELAY_TIME_MSG);
			//ShowDlg(IDD_DLG_UTIL_03);
			break;
		case 8:
			m_nStepInitView++;
			DispMsg(_T("ȭ�鱸���� �����մϴ�.-MENU_01"), _T("�˸�"), RGB_GREEN, DELAY_TIME_MSG);
			ShowDlg(IDD_DLG_MENU_01);
			break;
		case 9:
			m_nStepInitView++;
			//DispMsg(_T("ȭ�鱸���� �����մϴ�.- 3"), _T("�˸�"), RGB_GREEN, DELAY_TIME_MSG);
 			//ShowDlg(IDD_DLG_MENU_02);
			break;
		case 10:
			m_nStepInitView++;
			DispMsg(_T("ȭ�鱸���� �����մϴ�.- MENU_03"), _T("�˸�"), RGB_GREEN, DELAY_TIME_MSG);
			ShowDlg(IDD_DLG_MENU_03);
			break;
		case 11:
			m_nStepInitView++;
			DispMsg(_T("ȭ�鱸���� �����մϴ�.- MENU_04"), _T("�˸�"), RGB_GREEN, DELAY_TIME_MSG);
			ShowDlg(IDD_DLG_MENU_04);
			break;
		case 12:
			m_nStepInitView++;
			DispMsg(_T("ȭ�鱸���� �����մϴ�.- MENU_05"), _T("�˸�"), RGB_GREEN, DELAY_TIME_MSG);
			ShowDlg(IDD_DLG_MENU_05);
			break;
		case 13:
			m_nStepInitView++;
			DispMsg(_T("ȭ�鱸���� �����մϴ�.- MENU_06"), _T("�˸�"), RGB_GREEN, DELAY_TIME_MSG);
			ShowDlg(IDD_DLG_MENU_06);
			break;
		case 14:
			m_nStepInitView++;
			DispMsg(_T("ȭ�鱸���� �����մϴ�.- FRAME_HIGH"), _T("�˸�"), RGB_GREEN, DELAY_TIME_MSG);
			ShowDlg(IDD_DLG_FRAME_HIGH);
			if (m_pDlgFrameHigh)
				m_pDlgFrameHigh->ChkMenu01();
			SetDualTest(pDoc->WorkingInfo.LastJob.bDualTest);

			//if (pDoc->GetCurrentInfoEng())
			//{
			//	if (pDoc->GetTestMode() == MODE_OUTER)
			//	{
			//		if (pDoc->GetItsSerialInfo(0, bDualTestInner, sLot, sLayerUp, sLayerDn, 0))
			//		{
			//			//if (pDoc->GetTestMode() == MODE_OUTER)
			//			//if (pDoc->m_MasterInner[0].IsMstSpec(pDoc->WorkingInfo.System.sPathCamSpecDir, pDoc->WorkingInfo.LastJob.sModelUp, sLayerUp))
			//			if (pDoc->m_MasterInner[0].IsMstSpec(pDoc->WorkingInfo.System.sPathCamSpecDir, pDoc->m_sEngModel, sLayerUp))
			//			{
			//				if (m_pDlgMenu06)
			//					m_pDlgMenu06->RedrawWindow();
			//			}
			//		}
			//	}
			//}

			Init();
			Sleep(300);
			break;
		case 15:
			m_nStepInitView++;
			m_bLoadMstInfo = TRUE;
			DispMsg(_T("H/W�� �ʱ�ȭ�մϴ�."), _T("�˸�"), RGB_GREEN, DELAY_TIME_MSG);
			InitAct();
			m_bStopFeeding = TRUE;
			m_pMpe->Write(_T("MB440115"), 1); // ��ŷ��Feeding����
			Sleep(300);
			break;
		case 16:
			if (m_pMotion)
			{
				DispMsg(_T("Homming"), _T("Searching Home Position..."), RGB_GREEN, 2000, TRUE);
				m_pMotion->SearchHome();
				m_nStepInitView++;
			}
			else
			{
				m_bTIM_INIT_VIEW = FALSE;
				pView->ClrDispMsg();
				AfxMessageBox(_T("Motion is failed."));
				PostMessage(WM_CLOSE);
			}
			break;
		case 17:
			if (m_pMotion)
			{
				if (m_pMotion->IsHomeDone())// && m_pMotion->IsHomeDone(MS_MKFD))
				{
					m_nStepInitView++;
				}

				sMsg.Format(_T("X0(%s) , Y0(%s)\r\nX1(%s) , Y1(%s)"), m_pMotion->IsHomeDone(MS_X0) ? _T("Done") : _T("Doing"),
					m_pMotion->IsHomeDone(MS_Y0) ? _T("Done") : _T("Doing"),
					m_pMotion->IsHomeDone(MS_X1) ? _T("Done") : _T("Doing"),
					m_pMotion->IsHomeDone(MS_Y1) ? _T("Done") : _T("Doing"));
				DispMsg(sMsg, _T("Homming"), RGB_GREEN, 2000, TRUE);
				Sleep(300);
			}
			else
			{
				m_bTIM_INIT_VIEW = FALSE;
				pView->ClrDispMsg();
				AfxMessageBox(_T("Motion is failed."));
				PostMessage(WM_CLOSE);
			}
			break;
		case 18:
			m_nStepInitView++;
			//m_bLoadMstInfo = TRUE;
			//if (m_pMotion)
			//{
			//	m_pMotion->ResetEStopTime(MS_X0Y0);
			//	Sleep(30);
			//	m_pMotion->ResetEStopTime(MS_X1Y1);
			//}
			break;
		case 19:
			m_nStepInitView++;
			DispMsg(_T("Completed Searching Home Pos..."), _T("Homming"), RGB_GREEN, 2000, TRUE);
			m_pMotion->ObjectMapping();
			m_bEscape = FALSE;

			if (m_pVoiceCoil[0])
			{
				m_pVoiceCoil[0]->SetMarkFinalData(0);
				//m_pVoiceCoil[0]->SetProbFinalData(0);
			}

			if (m_pVoiceCoil[1])
			{
				m_pVoiceCoil[1]->SetMarkFinalData(1);
				//m_pVoiceCoil[1]->SetProbFinalData(1);
			}

			break;
		case 20:
			m_bStopFeeding = FALSE;
			m_pMpe->Write(_T("MB440115"), 0); // ��ŷ��Feeding����
			m_nStepInitView++;
			if(m_pDlgMenu02)
				m_pDlgMenu02->SetJogSpd(_tstoi(pDoc->WorkingInfo.LastJob.sJogSpd));
			if (m_pDlgMenu03)
				m_pDlgMenu03->InitRelation();
			//m_pMotion->SetFeedRate(MS_X0Y0, 1.0);
			//Sleep(30);
			//m_pMotion->SetFeedRate(MS_X1Y1, 1.0);
			m_pMotion->SetR2RConf();
 			//m_pMotion->SetTorque(AXIS_MKTQ, _tstof(pDoc->WorkingInfo.Motion.sMkTq));
 			//m_pMotion->SetTorque(AXIS_AOITQ, _tstof(pDoc->WorkingInfo.Motion.sAoiTq));
 			//TowerLamp(RGB_YELLOW, FALSE, TRUE);
			TowerLamp(RGB_YELLOW, TRUE);

			if (!SetCollision(-1.0*_tstof(pDoc->WorkingInfo.Motion.sCollisionLength)))
			{
				DispMsg(_T("Collision"), _T("Failed to Set Collision ..."), RGB_GREEN, 2000, TRUE);
			}

			break;
		case 21:
			m_nStepInitView++;
			//MoveMkInitPos();
			InitPLC();
			SetPlcParam();
			GetPlcParam();
			TcpIpInit();
			DtsInit();
			m_bTIM_DISP_STATUS = TRUE;
			SetTimer(TIM_DISP_STATUS, 100, NULL);

			m_bTIM_START_UPDATE = TRUE;
			SetTimer(TIM_START_UPDATE, 500, NULL);
			break;
		case 22:
			if (m_bLoadMstInfo || m_bLoadMstInfoF)
				break;

			m_nStepInitView++;
			ClrDispMsg();

			//bRtn = LoadPcrFromBuf();

			if (m_pDlgMenu01)
			{
				m_pDlgMenu01->SetStripAllMk();
				m_pDlgMenu01->RefreshRmap();
				m_pDlgMenu01->ResetLastProc();
			}

			if (!MemChk())
			{
				pView->ClrDispMsg();
				AfxMessageBox(_T("Memory Error - Cam Spec Data : PCR[0] or PCR[1] or Reelmap"));
			}
			else
			{
				if (pDoc->m_pReelMap && bRtn)
				{
					ReloadReelmap();
					UpdateRst();
					UpdateLotTime();
				}

				if (pDoc->GetCurrentInfoEng())
				{
					if (pDoc->GetTestMode() == MODE_OUTER)
					{
						if (pDoc->GetItsSerialInfo(0, bDualTestInner, sLot, sLayerUp, sLayerDn, 3))
						{
							//if (pDoc->m_MasterInner[0].IsMstSpec(pDoc->WorkingInfo.System.sPathCamSpecDir, pDoc->WorkingInfo.LastJob.sModelUp, sLayerUp))
							if (pDoc->m_MasterInner[0].IsMstSpec(pDoc->WorkingInfo.System.sPathCamSpecDir, pDoc->m_sEngModel, sLayerUp))
							{
								if (m_pDlgMenu06)
								{
									m_pDlgMenu06->SetStripAllMk();
									m_pDlgMenu06->RefreshRmap();
									m_pDlgMenu06->ResetLastProc();
								}

								if (pDoc->m_pReelMapInner)
								{
									ReloadReelmapInner();
									UpdateRstInner();
								}
							}
						}
					}
				}

			}

			// ChkCollision
			if (!m_bThread[1])
				m_Thread[1].Start(GetSafeHwnd(), this, ThreadProc1);

			// DispDefImg
			if (!m_bThread[2])
				m_Thread[2].Start(GetSafeHwnd(), this, ThreadProc2);

			// UpdateYield
			if (!m_bThread[3])
				m_Thread[3].Start(GetSafeHwnd(), this, ThreadProc3);

			// DoShift2Mk
			if (!m_bThread[4])
				m_Thread[4].Start(GetSafeHwnd(), this, ThreadProc4);

			// Engrave Auto Sequence - Response Check
			if (!m_bThread[5])
				m_Thread[5].Start(GetSafeHwnd(), this, ThreadProc5);

			// UpdataeReelmapUp
			if (!m_bThread[6])
				m_Thread[6].Start(GetSafeHwnd(), this, ThreadProc6);

			// UpdataeReelmapAllUp
			if (!m_bThread[7])
				m_Thread[7].Start(GetSafeHwnd(), this, ThreadProc7);

			// UpdataeReelmapDn
			if (!m_bThread[8])
				m_Thread[8].Start(GetSafeHwnd(), this, ThreadProc8);

			// UpdataeReelmapAllDn
			if (!m_bThread[9])
				m_Thread[9].Start(GetSafeHwnd(), this, ThreadProc9);

			// UpdataeRstUp
			if (!m_bThread[10])
				m_Thread[10].Start(GetSafeHwnd(), this, ThreadProc10);

			// UpdataeRstDn
			if (!m_bThread[11])
				m_Thread[11].Start(GetSafeHwnd(), this, ThreadProc11);

			// UpdataeRstAllUp
			if (!m_bThread[12])
				m_Thread[12].Start(GetSafeHwnd(), this, ThreadProc12);

			// UpdataeRstAllDn
			if (!m_bThread[13])
				m_Thread[13].Start(GetSafeHwnd(), this, ThreadProc13);

			// UpdataeRstUp
			if (!m_bThread[14])
				m_Thread[14].Start(GetSafeHwnd(), this, ThreadProc14);

			// UpdataeRstDn
			if (!m_bThread[15])
				m_Thread[15].Start(GetSafeHwnd(), this, ThreadProc15);

			// UpdataeRstAllUp
			if (!m_bThread[16])
				m_Thread[16].Start(GetSafeHwnd(), this, ThreadProc16);

			// UpdataeRstAllDn
			if (!m_bThread[17])
				m_Thread[17].Start(GetSafeHwnd(), this, ThreadProc17);

			// UpdataeReelmapITS
			if (!m_bThread[18])
				m_Thread[18].Start(GetSafeHwnd(), this, ThreadProc18);

			// DispDefImgInner
			if (!m_bThread[19])
				m_Thread[19].Start(GetSafeHwnd(), this, ThreadProc19);

			// UpdateRstIts
			if (!m_bThread[20])
				m_Thread[20].Start(GetSafeHwnd(), this, ThreadProc20);

			// ReloadReelmapUpInner
			if (!m_bThread[21])
				m_Thread[21].Start(GetSafeHwnd(), this, ThreadProc21);

			// ReloadReelmapDnInner
			if (!m_bThread[22])
				m_Thread[22].Start(GetSafeHwnd(), this, ThreadProc22);

			// ReloadReelmapAllUpInner
			if (!m_bThread[23])
				m_Thread[23].Start(GetSafeHwnd(), this, ThreadProc23);

			// ReloadReelmapAllUpInner
			if (!m_bThread[24])
				m_Thread[24].Start(GetSafeHwnd(), this, ThreadProc24);

			// ReloadReelmapIts
			if (!m_bThread[25])
				m_Thread[25].Start(GetSafeHwnd(), this, ThreadProc25);

			// UpdataeReelmapUp
			if (!m_bThread[26])
				m_Thread[26].Start(GetSafeHwnd(), this, ThreadProc26);

			// UpdataeReelmapAllUp
			if (!m_bThread[27])
				m_Thread[27].Start(GetSafeHwnd(), this, ThreadProc27);

			// UpdataeReelmapDn
			if (!m_bThread[28])
				m_Thread[28].Start(GetSafeHwnd(), this, ThreadProc28);

			// UpdataeReelmapAllDn
			if (!m_bThread[29])
				m_Thread[29].Start(GetSafeHwnd(), this, ThreadProc29);

			// UpdataeYieldUp
			if (!m_bThread[30])
				m_Thread[30].Start(GetSafeHwnd(), this, ThreadProc30);

			// UpdataeYieldDn
			if (!m_bThread[31])
				m_Thread[31].Start(GetSafeHwnd(), this, ThreadProc31);

			// UpdataeYieldAllUp
			if (!m_bThread[32])
				m_Thread[32].Start(GetSafeHwnd(), this, ThreadProc32);

			// UpdataeYieldAllDn
			if (!m_bThread[33])
				m_Thread[33].Start(GetSafeHwnd(), this, ThreadProc33);

			// UpdataeYieldInnerUp
			if (!m_bThread[34])
				m_Thread[34].Start(GetSafeHwnd(), this, ThreadProc34);

			// UpdataeYieldInnerDn
			if (!m_bThread[35])
				m_Thread[35].Start(GetSafeHwnd(), this, ThreadProc35);

			// UpdataeYieldInnerAllUp
			if (!m_bThread[36])
				m_Thread[36].Start(GetSafeHwnd(), this, ThreadProc36);

			// UpdataeYieldInnerAllDn
			if (!m_bThread[37])
				m_Thread[37].Start(GetSafeHwnd(), this, ThreadProc37);

			// UpdataeYieldIts
			if (!m_bThread[38])
				m_Thread[38].Start(GetSafeHwnd(), this, ThreadProc38);

			MoveInitPos1();
			Sleep(30);
			MoveInitPos0();

			SetLotLastShot();
			StartLive();

			if (pDoc->GetTestMode() == MODE_OUTER)
			{
				if (m_pDlgMenu01)
					m_pDlgMenu01->EnableItsMode();
			}

			m_bTIM_MPE_IO = TRUE;
			SetTimer(TIM_MPE_IO, 50, NULL);

			if(m_bMkSt)
				LoadSerial();

			m_bTIM_INIT_VIEW = FALSE;
			break;
		}

		if (m_bTIM_INIT_VIEW)
			SetTimer(TIM_INIT_VIEW, 100, NULL);
	}

	if (nIDEvent == TIM_MPE_IO)
	{
		KillTimer(TIM_MPE_IO);

		CntMk();
		GetMpeIO();
		GetMpeSignal();
		//GetMpeData();
		DoIO();
		ChkMyMsg();

		//if (m_pDlgMenu03)
		//	m_pDlgMenu03->UpdateSignal();

		if (m_bTIM_MPE_IO)
			SetTimer(TIM_MPE_IO, 100, NULL);
	}

	if (nIDEvent == TIM_TOWER_WINKER)
	{
		KillTimer(TIM_TOWER_WINKER);
		DispTowerWinker();
		if (m_bTimTowerWinker)
			SetTimer(TIM_TOWER_WINKER, 100, NULL);
	}

	if (nIDEvent == TIM_BUZZER_WARN)
	{
		KillTimer(TIM_BUZZER_WARN);
		m_nCntBz++;
		if (m_nCntBz > BUZZER_DELAY)
		{
			m_bTimBuzzerWarn = FALSE;
			Buzzer(FALSE);
		}
		if (m_bTimBuzzerWarn)
			SetTimer(TIM_BUZZER_WARN, 100, NULL);
	}

	if (nIDEvent == TIM_DISP_STATUS)
	{
		KillTimer(TIM_DISP_STATUS);

		DispStsBar();
		DoDispMain();
		//DispMyMsgBox();

		if (m_bStopFromThread)
		{
			m_bStopFromThread = FALSE;
			Stop();
		}
		if (m_bBuzzerFromThread)
		{
			m_bBuzzerFromThread = FALSE;
			Buzzer(TRUE, 0);
		}


		//		SetMpeIO();

		ChkEmg();
		ChkSaftySen();
		ChkDoor();
		ChkRcvSig();

		if (m_bTIM_DISP_STATUS)
			SetTimer(TIM_DISP_STATUS, 100, NULL);
	}


	if (nIDEvent == TIM_SHOW_MENU01)
	{
		KillTimer(TIM_SHOW_MENU01);
		if (m_pDlgFrameHigh)
			m_pDlgFrameHigh->ChkMenu01();
	}

	if (nIDEvent == TIM_SHOW_MENU02)
	{
		KillTimer(TIM_SHOW_MENU02);
		if (m_pDlgFrameHigh)
			m_pDlgFrameHigh->ChkMenu02();
	}

	if (nIDEvent == TIM_CHK_TEMP_STOP)
	{
		KillTimer(TIM_CHK_TEMP_STOP);
#ifdef USE_MPE
		if (!(pDoc->m_pMpeSignal[7] & (0x01 << 3)))	// �Ͻ��������(PC�� On��Ű��, PLC�� Ȯ���ϰ� Off��Ŵ)-20141031
		{
			pDoc->LogAuto(_T("PC: �Ͻ����� OFF"));
			m_bTIM_CHK_TEMP_STOP = FALSE;
			m_pDlgMenu01->SetTempStop(FALSE);
		}
#endif
		if (m_bTIM_CHK_TEMP_STOP)
			SetTimer(TIM_CHK_TEMP_STOP, 500, NULL);
	}

	if (nIDEvent == TIM_SAFTY_STOP)
	{
		KillTimer(TIM_SAFTY_STOP);
		MsgBox(_T("�Ͻ����� - ��ŷ�� ���������� �����Ǿ����ϴ�."));
		m_bTIM_SAFTY_STOP = FALSE;
	}

	if (nIDEvent == TIM_START_UPDATE)
	{
		KillTimer(TIM_START_UPDATE);

		if (m_bLoadMstInfo && !m_bLoadMstInfoF)
		{
			if (!pDoc->WorkingInfo.LastJob.sModelUp.IsEmpty() && !pDoc->WorkingInfo.LastJob.sLayerUp.IsEmpty())
			{
				m_bLoadMstInfoF = TRUE;
				SetTimer(TIM_CAMMASTER_UPDATE, 500, NULL);
			}
			else
				m_bLoadMstInfo = FALSE;
		}

		if (m_bSetSig && !m_bSetSigF)
		{
			m_bSetSigF = TRUE;

			if (m_pEngrave->m_bGetOpInfo || m_pEngrave->m_bGetInfo)
			{
				if (m_pDlgInfo)
					m_pDlgInfo->UpdateData();

				if (m_pDlgMenu01)
					m_pDlgMenu01->UpdateData();

				m_pEngrave->m_bGetOpInfo = FALSE;
				m_pEngrave->m_bGetInfo = FALSE;
			}
			else
			{
				if (m_pDlgMenu03)
					m_pDlgMenu03->UpdateSignal();
			}

			m_bSetSig = FALSE;
		}
		else if (!m_bSetSig && m_bSetSigF)
		{
			m_bSetSigF = FALSE;
		}

		if (m_bSetData && !m_bSetDataF)
		{
			m_bSetDataF = TRUE;

			if (m_pEngrave->m_bGetOpInfo || m_pEngrave->m_bGetInfo)
			{
				if (m_pDlgInfo)
					m_pDlgInfo->UpdateData();

				if (m_pDlgMenu01)
					m_pDlgMenu01->UpdateData();

				m_pEngrave->m_bGetOpInfo = FALSE;
				m_pEngrave->m_bGetInfo = FALSE;
			}

			if (m_pDlgMenu02)
				m_pDlgMenu02->UpdateData();

			if (m_pDlgMenu03)
				m_pDlgMenu03->UpdateData();

			if (m_pDlgMenu04)
				m_pDlgMenu04->UpdateData();

			m_bSetData = FALSE;
		}
		else if (!m_bSetData && m_bSetDataF)
		{
			m_bSetDataF = FALSE;
		}

		if (m_bTIM_START_UPDATE)
			SetTimer(TIM_START_UPDATE, 100, NULL);
	}

	if (nIDEvent == TIM_CAMMASTER_UPDATE)
	{
		KillTimer(TIM_CAMMASTER_UPDATE);
		if (LoadMstInfo())
		{
			if (m_pDlgMenu01)
				m_pDlgMenu01->UpdateData();
		}
		m_bLoadMstInfoF = FALSE;
		m_bLoadMstInfo = FALSE;
	}


	CFormView::OnTimer(nIDEvent);
}


void CGvisR2R_PunchView::InitMyMsg()
{
	if (m_pDlgMyMsg)
		CloseMyMsg();

	m_pDlgMyMsg = new CDlgMyMsg(this);
	m_pDlgMyMsg->Create();
}

void CGvisR2R_PunchView::CloseMyMsg()
{
	if (m_pDlgMyMsg)
	{
		delete m_pDlgMyMsg;
		m_pDlgMyMsg = NULL;
	}
}

LRESULT CGvisR2R_PunchView::OnMyMsgExit(WPARAM wPara, LPARAM lPara)
{
	Buzzer(FALSE, 0);
	ClrAlarm();
	return 0L;
}

int CGvisR2R_PunchView::MsgBox(CString sMsg, int nThreadIdx, int nType, int nTimOut, BOOL bEngave)
{
	int nRtnVal = -1; // Reply(-1) is None.

	if (bEngave)
	{
		if (m_pEngrave)
		{
			pDoc->m_sMsgBox = sMsg;
			if (pDoc->m_sIsMsgBox != pDoc->m_sMsgBox)
			{
				if (m_pEngrave)
					m_pEngrave->SetMsgBox(pDoc->m_sMsgBox, nType);
			}
		}
	}

	if (m_pDlgMyMsg)
		nRtnVal = m_pDlgMyMsg->SyncMsgBox(sMsg, nThreadIdx, nType, nTimOut);

	return nRtnVal;
}

int CGvisR2R_PunchView::AsyncMsgBox(CString sMsg, int nThreadIdx, int nType, int nTimOut)
{
	int nRtn = -1;
	if (m_pDlgMyMsg)
		m_pDlgMyMsg->AsyncMsgBox(sMsg, nThreadIdx, nType, nTimOut);
	return nRtn;
}

int CGvisR2R_PunchView::WaitRtnVal(int nThreadIdx)
{
	int nRtn = -1;
	if (m_pDlgMyMsg)
		nRtn = m_pDlgMyMsg->WaitRtnVal(nThreadIdx);
	return nRtn;
}



void CGvisR2R_PunchView::ChkMyMsg()
{
	return;

	//HWND hwndForeground;
	CWnd *pWndForeground;

	if (m_bShowMyMsg && m_pMyMsgForeground)
	{
		pWndForeground = pFrm->GetForegroundWindow();
		if (pWndForeground != m_pMyMsgForeground)
			m_pMyMsgForeground->SetForegroundWindow();
	}
}

void CGvisR2R_PunchView::UpdateLotTime()
{
	m_dwLotSt = (DWORD)pDoc->WorkingInfo.Lot.dwStTick;

	if (m_pDlgMenu01)
		m_pDlgMenu01->UpdateLotTime();
}

void CGvisR2R_PunchView::DispStsBar(CString sMsg, int nIdx)
{
	if (m_sDispMsg[nIdx] != sMsg)
		m_sDispMsg[nIdx] = sMsg;
	sMsg.Empty();
}

void CGvisR2R_PunchView::DispStsBar()
{
	DispStsMainMsg(); // 0
	//DispStsMainMsg(6); // 6
	//DispStsMainMsg(5); // 5
	//DispThreadTick(); // 5, 6
	DispTime(); // 7
	ChkShare(); // 2, 4
	if (!m_bShift2Mk && !m_bTHREAD_SHIFT2MK)// && (m_nMkStAuto < MK_ST + (Mk2PtIdx::DoneMk) || m_nMkStAuto > MK_ST + (Mk2PtIdx::DoneMk) + 5 || m_bTIM_INIT_VIEW))
	{
		ChkBuf(); // 1, 3
		if (m_bTIM_INIT_VIEW)
			SetListBuf();
	}
}

BOOL CGvisR2R_PunchView::MemChk() // ����ȭ�鿡 ǥ���� �����͸� ���� ����ü �Ҵ��� Ȯ����.
{
	if (!pDoc->m_pPcr[0] || !pDoc->m_pPcr[1])// || !pDoc->m_pReelMap)
		return FALSE;
	return TRUE;
}

void CGvisR2R_PunchView::SetMainMc(BOOL bOn)
{
	if (bOn)
	{
		if (m_pMpe)
			m_pMpe->Write(_T("MB440159"), 1); // ��ŷ�� Door Interlock ON
			//m_pMpe->Write(_T("MB440159"), 1); // ��ŷ�� MC ON
	}
	else
	{
		if (m_pMpe)
			m_pMpe->Write(_T("MB440158"), 0); // ��ŷ�� Door Interlock OFF
			//m_pMpe->Write(_T("MB440159"), 0); // ��ŷ�� MC OFF
	}
}

void CGvisR2R_PunchView::ExitProgram()
{
	long lParam = 0;
	long lData = 1;
	lParam = lParam | lData;
	lData = 0x00 << 16;
	lParam = lParam | lData;
	lData = 1 << 29;
	lParam = lParam | lData;
	AfxGetMainWnd()->PostMessage(WM_COMMAND, ID_APP_EXIT);
}

void CGvisR2R_PunchView::Init()
{
	int nAxis;
	if (m_pMotion)
	{
		// 		for(int nMsId=0; nMsId<m_pMotion->m_ParamCtrl.nTotMotion; nMsId++)
		for (nAxis = 0; nAxis<m_pMotion->m_ParamCtrl.nTotAxis; nAxis++)
		{
			// 			m_pMotion->AmpReset(nMsId);
			m_pMotion->AmpReset(nAxis);
			Sleep(30);
		}
	}
}

void CGvisR2R_PunchView::InitIO()
{
	int i, k;

#ifdef USE_MPE
	pDoc->m_nMpeIo = pDoc->MkIo.MpeIo.nMaxSeg;
	pDoc->m_nMpeIb = pDoc->MkIo.MpeIo.nMaxSeg;

	if (!pDoc->m_pMpeIb)
	{
		if (pDoc->m_nMpeIb > 0)
		{
			pDoc->m_pMpeIb = new unsigned short[pDoc->m_nMpeIb];
			for (i = 0; i < pDoc->m_nMpeIb; i++)
				pDoc->m_pMpeIb[i] = 0;
		}
	}

	if (!pDoc->m_pMpeIF)
	{
		if (pDoc->m_nMpeIb>0)
		{
			pDoc->m_pMpeIF = new unsigned short[pDoc->m_nMpeIb];
			for (i = 0; i<pDoc->m_nMpeIb; i++)
				pDoc->m_pMpeIF[i] = 0;
		}
	}

	if (!pDoc->m_pMpeIo)
	{
		if (pDoc->m_nMpeIo>0)
		{
			pDoc->m_pMpeIo = new unsigned short[pDoc->m_nMpeIo];
			for (i = 0; i<pDoc->m_nMpeIo; i++)
				pDoc->m_pMpeIo[i] = 0;
		}
	}

	if (!pDoc->m_pMpeIoF)
	{
		if (pDoc->m_nMpeIo>0)
		{
			pDoc->m_pMpeIoF = new unsigned short[pDoc->m_nMpeIo];
			for (i = 0; i<pDoc->m_nMpeIo; i++)
				pDoc->m_pMpeIoF[i] = 0;
		}
	}


	pDoc->m_nMpeSignal = pDoc->MkIo.MpeSignal.nMaxSeg;
	if (!pDoc->m_pMpeSignal)
	{
		if (pDoc->m_nMpeSignal>0)
		{
			pDoc->m_pMpeSignal = new unsigned short[pDoc->m_nMpeSignal];
			for (i = 0; i<pDoc->m_nMpeSignal; i++)
				pDoc->m_pMpeSignal[i] = 0;
		}
	}


	pDoc->m_nMpeData = pDoc->MkIo.MpeData.nMaxSeg;
	if (!pDoc->m_pMpeData)
	{
		if (pDoc->m_nMpeData>0)
		{
			pDoc->m_pMpeData = new long*[pDoc->m_nMpeData];
			for (i = 0; i<pDoc->m_nMpeData; i++)
			{
				pDoc->m_pMpeData[i] = new long[16];
				for (k = 0; k<16; k++)
					pDoc->m_pMpeData[i][k] = 0;
			}
		}
	}
#endif
}

BOOL CGvisR2R_PunchView::InitAct()
{
#ifdef USE_XMP
	if (!m_pMotion)
		return FALSE;
#endif

#ifdef USE_LIGHT
	if (!m_pLight)
		return FALSE;
#endif

#ifdef USE_FLUCK
	if (!m_pFluck)
		return FALSE;
#endif

	int nAxis;

	if (m_pMotion)
	{
		// Motor On
		for (nAxis = 0; nAxis<m_pMotion->m_ParamCtrl.nTotAxis; nAxis++)
		{
			m_pMotion->ServoOnOff(nAxis, TRUE);
			Sleep(100);
		}

		double dX[2], dY[2];

		if (pDoc->m_pSpecLocal && IsPinData())
		{
			dX[0] = pDoc->m_pSpecLocal->m_dPinPosX[0];
			dY[0] = pDoc->m_pSpecLocal->m_dPinPosY[0];
			dX[1] = pDoc->m_pSpecLocal->m_dPinPosX[1];
			dY[1] = pDoc->m_pSpecLocal->m_dPinPosY[1];
		}
		else
		{
			dX[0] = _tstof(pDoc->WorkingInfo.Motion.sPinPosX[0]);
			dY[0] = _tstof(pDoc->WorkingInfo.Motion.sPinPosY[0]);
			dX[1] = _tstof(pDoc->WorkingInfo.Motion.sPinPosX[1]);
			dY[1] = _tstof(pDoc->WorkingInfo.Motion.sPinPosY[1]);
		}
		m_pMotion->SetPinPos(0, dX[0], dY[0]);
		m_pMotion->SetPinPos(1, dX[1], dY[1]);
		m_pMotion->m_dStBufPos = _tstof(pDoc->WorkingInfo.Motion.sStBufPos);


		CfPoint ptPnt0(dX[0], dY[0]);
		if (pDoc->m_Master[0].m_pPcsRgn)
			pDoc->m_Master[0].m_pPcsRgn->SetPinPos(0, ptPnt0);

		CfPoint ptPnt1(dX[1], dY[1]);
		if (pDoc->m_Master[0].m_pPcsRgn)
			pDoc->m_Master[0].m_pPcsRgn->SetPinPos(1, ptPnt1);

		if (pDoc->m_pSpecLocal)// && IsMkOffsetData())
		{
			// 			m_pDlgMenu02->SetPcsOffset();
			// 			CfPoint ptOfst(0.0, 0.0);
			pDoc->SetMkPnt(CAM_BOTH);
		}

		double dPos = _tstof(pDoc->WorkingInfo.Motion.sStBufPos);
		SetBufInitPos(dPos);
		double dVel = _tstof(pDoc->WorkingInfo.Motion.sBufHomeSpd);
		double dAcc = _tstof(pDoc->WorkingInfo.Motion.sBufHomeAcc);
		SetBufHomeParam(dVel, dAcc);
		// 		m_pMotion->SetStBufPos(dPos);

	}

	// Light On
	if (m_pDlgMenu02)
	{
		m_pDlgMenu02->SetLight(_tstoi(pDoc->WorkingInfo.Light.sVal[0]));
		m_pDlgMenu02->SetLight2(_tstoi(pDoc->WorkingInfo.Light.sVal[1]));
	}

	// Homming
	if (m_pVoiceCoil[0])
		m_pVoiceCoil[0]->SearchHomeSmac(0);
	if (m_pVoiceCoil[1])
		m_pVoiceCoil[1]->SearchHomeSmac(1);

	return TRUE;
}

BOOL CGvisR2R_PunchView::TcpIpInit()
{
#ifdef USE_SR1000W
	if (!m_pSr1000w)
	{
		m_pSr1000w = new CSr1000w(pDoc->WorkingInfo.System.sIpClient[ID_SR1000W], pDoc->WorkingInfo.System.sIpServer[ID_SR1000W], pDoc->WorkingInfo.System.sPort[ID_SR1000W], this);
		//m_pSr1000w->SetHwnd(this->GetSafeHwnd());
	}
#endif	

#ifdef USE_TCPIP
	if (!m_pEngrave)
	{
		m_pEngrave = new CEngrave(pDoc->WorkingInfo.System.sIpClient[ID_PUNCH], pDoc->WorkingInfo.System.sIpServer[ID_ENGRAVE], pDoc->WorkingInfo.System.sPort[ID_ENGRAVE], this);
		m_pEngrave->SetHwnd(this->GetSafeHwnd());
	}
#endif

	return TRUE;
}

void CGvisR2R_PunchView::DtsInit()
{
#ifdef USE_DTS
	if (pDoc->m_bUseDts)
	{
		if (!m_pDts)
		{
			m_pDts = new CDts(this);
		}
	}
#endif
}

BOOL CGvisR2R_PunchView::HwInit()
{
	if (m_pLight)
	{
		delete m_pLight;
		m_pLight = NULL;
	}
	m_pLight = new CLight(this);
	m_pLight->Init();
#ifdef USE_FLUCK
	if (pDoc->WorkingInfo.Fluck.bUse)
	{
		if (m_pFluck)
		{
			delete m_pFluck;
			m_pFluck = NULL;
		}
		m_pFluck = new CFluck(this);
		m_pFluck->Init();
	}
#endif
	if (m_pVoiceCoil[0])
	{
		delete m_pVoiceCoil[0];
		m_pVoiceCoil[0] = NULL;
	}

	m_pVoiceCoil[0] = new CSmac(this);
	m_pVoiceCoil[0]->SetCh(VOICE_COIL_FIRST_CAM);
	m_pVoiceCoil[0]->Init();

	if (m_pVoiceCoil[1])
	{
		delete m_pVoiceCoil[1];
		m_pVoiceCoil[1] = NULL;
	}

	m_pVoiceCoil[1] = new CSmac(this);
	m_pVoiceCoil[1]->SetCh(VOICE_COIL_SECOND_CAM);
	m_pVoiceCoil[1]->Init();

	if (m_pMotion)
	{
		delete m_pMotion;
		m_pMotion = NULL;
	}
	m_pMotion = new CMotion(this);
	if (!m_pMotion->InitBoard())
	{
		//		DoMyMsgBox(_T("XMP ���� �ʱ�ȭ ����, �ٽ� �����ϼ���.!!!"));
		MsgBox(_T("XMP ���� �ʱ�ȭ ����, �ٽ� �����ϼ���.!!!"));
		PostMessage(WM_CLOSE);
		return FALSE;
	}

	// 	if(m_pVision[1])
	// 	{
	// 		delete m_pVision[1];
	// 		m_pVision[1] = NULL;
	// 	}
	// 	if(m_pVision[0])
	// 	{
	// 		delete m_pVision[0];
	// 		m_pVision[0] = NULL;
	// 	}
	// 
	// 	HWND hCtrlV0[4] = {0};
	// 	hCtrlV0[0] = GetDlgItem(IDC_STC_VISION)->GetSafeHwnd();
	// 	m_pVision[0] = new CVision(0, hCtrlV0, this);
	// 
	//  	HWND hCtrlV1[4] = {0};
	// 	hCtrlV1[0] = GetDlgItem(IDC_STC_VISION_2)->GetSafeHwnd();
	// 	m_pVision[1] = new CVision(1, hCtrlV1, this);
#ifdef USE_MPE
	if (!m_pMpe)
		m_pMpe = new CMpDevice(this);
	if (!m_pMpe->Init(1, 1))
	{
		//		DoMyMsgBox(_T("��īƮ�θ�ũ(MC0)�� ������α׷��� ���� ��, �ٽ� �����ϼ���.!!!"));
		MsgBox(_T("��īƮ�θ�ũ(MC0)�� ������α׷��� ���� ��, �ٽ� �����ϼ���.!!!"));
		PostMessage(WM_CLOSE);
		return FALSE;
	}
#endif
//#ifdef USE_SR1000W
//	if (!m_pSr1000w)
//	{
//		m_pSr1000w = new CSr1000w(pDoc->WorkingInfo.System.sIpClient[ID_SR1000W], pDoc->WorkingInfo.System.sIpServer[ID_SR1000W], pDoc->WorkingInfo.System.sPort[ID_SR1000W], this);
//		//m_pSr1000w->SetHwnd(this->GetSafeHwnd());
//	}
//#endif	
//
//#ifdef USE_TCPIP
//	if (!m_pEngrave)
//	{
//		m_pEngrave = new CEngrave(pDoc->WorkingInfo.System.sIpClient[ID_PUNCH], pDoc->WorkingInfo.System.sIpServer[ID_ENGRAVE], pDoc->WorkingInfo.System.sPort[ID_ENGRAVE], this);
//		m_pEngrave->SetHwnd(this->GetSafeHwnd());
//	}
//#endif

	return TRUE;
}

void CGvisR2R_PunchView::HwKill()
{
	// 	if(m_pVision[1])
	// 	{
	// 		delete m_pVision[1];
	// 		m_pVision[1] = NULL;
	// 	}
	// 
	// 	if(m_pVision[0])
	// 	{
	// 		delete m_pVision[0];
	// 		m_pVision[0] = NULL;
	// 	}

	if (m_pMotion)
	{
		delete m_pMotion;
		m_pMotion = NULL;
	}

	if (m_pMpe)
	{
		delete m_pMpe;
		m_pMpe = NULL;
	}

	if (m_pVoiceCoil[0])
	{
		delete m_pVoiceCoil[0];
		m_pVoiceCoil[0] = NULL;
	}

	if (m_pVoiceCoil[1])
	{
		delete m_pVoiceCoil[1];
		m_pVoiceCoil[1] = NULL;
	}

	if (m_pLight)
	{
		m_pLight->Close();
		delete m_pLight;
		m_pLight = NULL;
	}

	if (m_pEngrave)
	{
		m_pEngrave->Close();
		delete m_pEngrave;
		m_pEngrave = NULL;
	}

	if (m_pSr1000w)
	{
		m_pSr1000w->Close();
		delete m_pSr1000w;
		m_pSr1000w = NULL;
	}

	if (m_pDts)
	{
		delete m_pDts;
		m_pDts = NULL;
	}

#ifdef USE_FLUCK
	if (m_pFluck)
	{
		m_pFluck->Close();
		delete m_pFluck;
		m_pFluck = NULL;
	}
#endif
}

void CGvisR2R_PunchView::GetDispMsg(CString &strMsg, CString &strTitle)
{
	if (m_pDlgMsgBox)
		m_pDlgMsgBox->GetDispMsg(strMsg, strTitle);
}

void CGvisR2R_PunchView::DispMsg(CString strMsg, CString strTitle, COLORREF color, DWORD dwDispTime, BOOL bOverWrite)
{
	if (m_bDispMsg)
		return;

	if (m_bAuto)
	{
		return;
	}

	m_bDispMsg = TRUE;

	if (dwDispTime == 0)
	{
		dwDispTime = 24 * 3600 * 1000;
	}

	if (m_pDlgMsgBox != NULL)
	{
		if (bOverWrite)
		{
			if(m_pDlgMsgBox)
				m_pDlgMsgBox->SetDispMsg(strMsg, strTitle, dwDispTime, color);
		}
		if (m_pDlgMsgBox)
			m_pDlgMsgBox->ShowWindow(SW_SHOW);
		if (m_pDlgMsgBox)
			m_pDlgMsgBox->SetFocus();
		if (m_pDlgMsgBox)
			((CButton*)m_pDlgMsgBox->GetDlgItem(IDOK))->SetCheck(TRUE);
	}
	else
	{
		m_pDlgMsgBox = new CDlgMsgBox(this, strTitle, strMsg, dwDispTime, color);
		if (m_pDlgMsgBox->GetSafeHwnd() == 0)
		{
			m_pDlgMsgBox->Create();
			m_pDlgMsgBox->ShowWindow(SW_SHOW);
			m_pDlgMsgBox->SetDispMsg(strMsg, strTitle, dwDispTime, color);
			m_pDlgMsgBox->SetFocus();
			((CButton*)m_pDlgMsgBox->GetDlgItem(IDOK))->SetCheck(TRUE);
		}
	}

	m_bDispMsg = FALSE;
}

void CGvisR2R_PunchView::ClrDispMsg()
{
	OnQuitDispMsg(NULL, NULL);
}

BOOL CGvisR2R_PunchView::WaitClrDispMsg()
{
	m_bWaitClrDispMsg = TRUE;
	MSG message;

	DWORD dwMilliseconds = 0; // 100ms sec sleep
	while (WAIT_OBJECT_0 != ::WaitForSingleObject(m_evtWaitClrDispMsg, dwMilliseconds) && m_pDlgMsgBox != NULL)
	{
		if (::PeekMessage(&message, NULL, 0, 0, PM_REMOVE))
		{
			::TranslateMessage(&message);
			::DispatchMessage(&message);
		}
	};
	Sleep(10);
	m_bWaitClrDispMsg = FALSE;
	return TRUE;
}

LONG CGvisR2R_PunchView::OnQuitDispMsg(UINT wParam, LONG lParam)
{
	if (m_pDlgMsgBox)
	{
		if (m_pDlgMsgBox->GetSafeHwnd())
			m_pDlgMsgBox->DestroyWindow();
		delete m_pDlgMsgBox;
		m_pDlgMsgBox = NULL;
	}

	return 0L;
}

void CGvisR2R_PunchView::ShowDlg(int nID)
{
	HideAllDlg();

	switch (nID)
	{
	//case IDD_DLG_INFO:
	//	if (!m_pDlgInfo)
	//	{
	//		m_pDlgInfo = new CDlgInfo(this);
	//		if (m_pDlgInfo->GetSafeHwnd() == 0)
	//		{
	//			m_pDlgInfo->Create();
	//			m_pDlgInfo->ShowWindow(SW_SHOW);
	//		}
	//	}
	//	else
	//	{
	//		m_pDlgInfo->ShowWindow(SW_SHOW);
	//	}
	//	break;

	case IDD_DLG_FRAME_HIGH:
		if (!m_pDlgFrameHigh)
		{
			m_pDlgFrameHigh = new CDlgFrameHigh(this);
			if (m_pDlgFrameHigh->GetSafeHwnd() == 0)
			{
				m_pDlgFrameHigh->Create();
				m_pDlgFrameHigh->ShowWindow(SW_SHOW);
			}
		}
		else
		{
			m_pDlgFrameHigh->ShowWindow(SW_SHOW);
		}
		break;

	case IDD_DLG_MENU_01:
		if (!m_pDlgMenu01)
		{
			m_pDlgMenu01 = new CDlgMenu01(this);
			if (m_pDlgMenu01->GetSafeHwnd() == 0)
			{
				m_pDlgMenu01->Create();
				m_pDlgMenu01->ShowWindow(SW_SHOW);
			}
		}
		else
		{
			m_pDlgMenu01->ShowWindow(SW_SHOW);
		}
		break;

	case IDD_DLG_MENU_02:
		if (!m_pDlgMenu02)
		{
			m_pDlgMenu02 = new CDlgMenu02(this);
			if (m_pDlgMenu02->GetSafeHwnd() == 0)
			{
				m_pDlgMenu02->Create();
				m_pDlgMenu02->ShowWindow(SW_SHOW);
			}
		}
		else
		{
			m_pDlgMenu02->ShowWindow(SW_SHOW);
		}
		break;

	case IDD_DLG_MENU_03:
		if (!m_pDlgMenu03)
		{
			m_pDlgMenu03 = new CDlgMenu03(this);
			if (m_pDlgMenu03->GetSafeHwnd() == 0)
			{
				m_pDlgMenu03->Create();
				m_pDlgMenu03->ShowWindow(SW_SHOW);
			}
		}
		else
		{
			m_pDlgMenu03->ShowWindow(SW_SHOW);
		}
		break;

	case IDD_DLG_MENU_04:
		if (!m_pDlgMenu04)
		{
			m_pDlgMenu04 = new CDlgMenu04(this);
			if (m_pDlgMenu04->GetSafeHwnd() == 0)
			{
				m_pDlgMenu04->Create();
				m_pDlgMenu04->ShowWindow(SW_SHOW);
			}
		}
		else
		{
			m_pDlgMenu04->ShowWindow(SW_SHOW);
		}
		break;

	case IDD_DLG_MENU_05:
		if (!m_pDlgMenu05)
		{
			m_pDlgMenu05 = new CDlgMenu05(this);
			if (m_pDlgMenu05->GetSafeHwnd() == 0)
			{
				m_pDlgMenu05->Create();
				m_pDlgMenu05->ShowWindow(SW_SHOW);
			}
		}
		else
		{
			m_pDlgMenu05->ShowWindow(SW_SHOW);
		}
		break;

	case IDD_DLG_MENU_06:
		if (!m_pDlgMenu06)
		{
			m_pDlgMenu06 = new CDlgMenu06(this);
			if (m_pDlgMenu06->GetSafeHwnd() == 0)
			{
				m_pDlgMenu06->Create();
				m_pDlgMenu06->ShowWindow(SW_SHOW);
			}
		}
		else
		{
			m_pDlgMenu06->ShowWindow(SW_SHOW);
		}
		break;

	//case IDD_DLG_UTIL_01:
	//	if (!m_pDlgUtil01)
	//	{
	//		m_pDlgUtil01 = new CDlgUtil01(this);
	//		if (m_pDlgUtil01->GetSafeHwnd() == 0)
	//		{
	//			m_pDlgUtil01->Create();
	//			m_pDlgUtil01->ShowWindow(SW_SHOW);
	//		}
	//	}
	//	else
	//	{
	//		m_pDlgUtil01->ShowWindow(SW_SHOW);
	//	}
	//	break;

	//case IDD_DLG_UTIL_02:
	//	if (!m_pDlgUtil02)
	//	{
	//		m_pDlgUtil02 = new CDlgUtil02(this);
	//		if (m_pDlgUtil02->GetSafeHwnd() == 0)
	//		{
	//			m_pDlgUtil02->Create();
	//			m_pDlgUtil02->ShowWindow(SW_SHOW);
	//		}
	//	}
	//	else
	//	{
	//		m_pDlgUtil02->ShowWindow(SW_SHOW);
	//	}
	//	break;

		// 	case IDD_DLG_UTIL_03:
		// 		if(!m_pDlgUtil03)
		// 		{
		// 			m_pDlgUtil03 = new CDlgUtil03(this);
		// 			if(m_pDlgUtil03->GetSafeHwnd() == 0)
		// 			{
		// 				m_pDlgUtil03->Create();
		// 				m_pDlgUtil03->ShowWindow(SW_SHOW);
		// 			}
		// 		}
		// 		else
		// 		{
		// 			m_pDlgUtil03->ShowWindow(SW_SHOW);
		// 		}
		// 		break;		
	}
}

void CGvisR2R_PunchView::HideAllDlg()
{
	//if (m_pDlgInfo && m_pDlgInfo->GetSafeHwnd())
	//{
	//	if (m_pDlgInfo->IsWindowVisible())
	//		m_pDlgInfo->ShowWindow(SW_HIDE);
	//}
	if (m_pDlgMenu01 && m_pDlgMenu01->GetSafeHwnd())
	{
		if (m_pDlgMenu01->IsWindowVisible())
			m_pDlgMenu01->ShowWindow(SW_HIDE);
	}
	if (m_pDlgMenu02 && m_pDlgMenu02->GetSafeHwnd())
	{
		if (m_pDlgMenu02->IsWindowVisible())
			m_pDlgMenu02->ShowWindow(SW_HIDE);
	}
	if (m_pDlgMenu03 && m_pDlgMenu03->GetSafeHwnd())
	{
		if (m_pDlgMenu03->IsWindowVisible())
			m_pDlgMenu03->ShowWindow(SW_HIDE);
	}
	if (m_pDlgMenu04 && m_pDlgMenu04->GetSafeHwnd())
	{
		if (m_pDlgMenu04->IsWindowVisible())
			m_pDlgMenu04->ShowWindow(SW_HIDE);
	}
	if (m_pDlgMenu05 && m_pDlgMenu05->GetSafeHwnd())
	{
		if (m_pDlgMenu05->IsWindowVisible())
			m_pDlgMenu05->ShowWindow(SW_HIDE);
	}
	if (m_pDlgMenu06 && m_pDlgMenu06->GetSafeHwnd())
	{
		if (m_pDlgMenu06->IsWindowVisible())
			m_pDlgMenu06->ShowWindow(SW_HIDE);
	}

	//if (m_pDlgUtil01 && m_pDlgUtil01->GetSafeHwnd())
	//{
	//	if (m_pDlgUtil01->IsWindowVisible())
	//		m_pDlgUtil01->ShowWindow(SW_HIDE);
	//}
	//if (m_pDlgUtil02 && m_pDlgUtil02->GetSafeHwnd())
	//{
	//	if (m_pDlgUtil02->IsWindowVisible())
	//		m_pDlgUtil02->ShowWindow(SW_HIDE);
	//}
	// 	if(m_pDlgUtil03 && m_pDlgUtil03->GetSafeHwnd())
	// 	{
	// 		if(m_pDlgUtil03->IsWindowVisible())
	// 			m_pDlgUtil03->ShowWindow(SW_HIDE);
	// 	}
}

void CGvisR2R_PunchView::DelAllDlg()
{
	//if (m_pDlgInfo != NULL)
	//{
	//	delete m_pDlgInfo;
	//	m_pDlgInfo = NULL;
	//}
	// 	if(m_pDlgUtil03 != NULL) 
	// 	{
	// 		delete m_pDlgUtil03;
	// 		m_pDlgUtil03 = NULL;
	// 	}
	//if (m_pDlgUtil02 != NULL)
	//{
	//	delete m_pDlgUtil02;
	//	m_pDlgUtil02 = NULL;
	//}
	//if (m_pDlgUtil01 != NULL)
	//{
	//	delete m_pDlgUtil01;
	//	m_pDlgUtil01 = NULL;
	//}

	if (m_pDlgMenu06 != NULL)
	{
		delete m_pDlgMenu06;
		m_pDlgMenu06 = NULL;
	}
	if (m_pDlgMenu05 != NULL)
	{
		delete m_pDlgMenu05;
		m_pDlgMenu05 = NULL;
	}
	if (m_pDlgMenu04 != NULL)
	{
		delete m_pDlgMenu04;
		m_pDlgMenu04 = NULL;
	}
	if (m_pDlgMenu03 != NULL)
	{
		delete m_pDlgMenu03;
		m_pDlgMenu03 = NULL;
	}
	if (m_pDlgMenu02 != NULL)
	{
		delete m_pDlgMenu02;
		m_pDlgMenu02 = NULL;
	}
	if (m_pDlgMenu01 != NULL)
	{
		delete m_pDlgMenu01;
		m_pDlgMenu01 = NULL;
	}
	if (m_pDlgFrameHigh != NULL)
	{
		delete m_pDlgFrameHigh;
		m_pDlgFrameHigh = NULL;
	}

	if (m_pDlgMsgBox != NULL)
	{
		if (m_pDlgMsgBox->GetSafeHwnd())
			m_pDlgMsgBox->DestroyWindow();
		delete m_pDlgMsgBox;
		m_pDlgMsgBox = NULL;
	}
}

LRESULT CGvisR2R_PunchView::OnDlgInfo(WPARAM wParam, LPARAM lParam)
{
	ClrDispMsg();
	CDlgInfo Dlg;
	m_pDlgInfo = &Dlg;
	Dlg.DoModal();
	m_pDlgInfo = NULL;

	if (m_pDlgMenu01)
		m_pDlgMenu01->ChkUserInfo(FALSE);

	return 0L;
}

void CGvisR2R_PunchView::DispIo()
{
	ClrDispMsg();
	CDlgUtil04 Dlg;
	Dlg.DoModal();
}

void CGvisR2R_PunchView::DispDatabaseConnection()
{
	//ClrDispMsg();
	//CDlgUtil07 Dlg;
	//Dlg.DoModal();
}

void CGvisR2R_PunchView::Option01()
{
	ClrDispMsg();
	CDlgOption01 Dlg;
	Dlg.DoModal();
}

void CGvisR2R_PunchView::CamMasterInfoMst()
{
	ClrDispMsg();
	CDlgCammasterinfoMst Dlg;
	Dlg.DoModal();
}

void CGvisR2R_PunchView::CamMasterInfoPch()
{
	ClrDispMsg();
	CDlgCammasterinfoPch Dlg;
	Dlg.DoModal();
}

void CGvisR2R_PunchView::CamMasterInfoStrPcs()
{
	ClrDispMsg();
	CDlgCammasterinfoStrpcs Dlg;
	Dlg.DoModal();
}

void CGvisR2R_PunchView::TowerLamp(COLORREF color, BOOL bOn, BOOL bWink)
{
	if (bWink)
	{
		if (color == RGB_RED)
		{
			m_bTowerWinker[0] = bOn;
			if (bOn)
			{
				m_bTowerWinker[1] = FALSE;
				m_bTowerWinker[2] = FALSE;
			}
		}
		if (color == RGB_GREEN)
		{
			m_bTowerWinker[1] = bOn;
			if (bOn)
			{
				m_bTowerWinker[0] = FALSE;
				m_bTowerWinker[2] = FALSE;
			}
		}
		if (color == RGB_YELLOW)
		{
			m_bTowerWinker[2] = bOn;
			if (bOn)
			{
				m_bTowerWinker[1] = FALSE;
				m_bTowerWinker[0] = FALSE;
			}
		}

		if (!m_bTowerWinker[0] && !m_bTowerWinker[1] && !m_bTowerWinker[2])
			m_bTimTowerWinker = FALSE;
		else
		{
			if (!m_bTimTowerWinker)
			{
				m_bTimTowerWinker = TRUE;
				SetTimer(TIM_TOWER_WINKER, 100, NULL);
			}
		}
	}
	else
	{
		m_bTimTowerWinker = FALSE;

#ifdef USE_MPE
		if (!pDoc->m_pMpeIo)
			return;
#endif
	}
}

void CGvisR2R_PunchView::DispTowerWinker()
{
	m_nCntTowerWinker++;
	if (m_nCntTowerWinker > LAMP_DELAY)
	{
		m_nCntTowerWinker = 0;
	}

}

void CGvisR2R_PunchView::BuzzerFromThread(BOOL bOn, int nCh)
{
	m_bBuzzerFromThread = TRUE;
}

void CGvisR2R_PunchView::Buzzer(BOOL bOn, int nCh)
{
	//	return; // PLC����
#ifdef USE_MPE
	if (!m_pMpe)
		return;

	if (!bOn)
	{
		switch (nCh)
		{
		case 0:
			//IoWrite(_T("MB44015E"), 0); // ����1 On  (PC�� ON, OFF) - 20141020
			m_pMpe->Write(_T("MB44015E"), 0);
			break;
		case 1:
			//IoWrite(_T("MB44015F"), 0); // ����2 On  (PC�� ON, OFF) - 20141020
			m_pMpe->Write(_T("MB44015F"), 0);
			break;
		}
	}
	else
	{
		switch (nCh)
		{
		case 0:
			//IoWrite(_T("MB44015E"), 1); // ����1 On  (PC�� ON, OFF) - 20141020
			pView->m_pMpe->Write(_T("MB44015E"), 0);
			Sleep(300);
			pView->m_pMpe->Write(_T("MB44015E"), 1);
			break;
		case 1:
			//IoWrite(_T("MB44015F"), 1); // ����2 On  (PC�� ON, OFF) - 20141020
			pView->m_pMpe->Write(_T("MB44015E"), 0);
			Sleep(300);
			pView->m_pMpe->Write(_T("MB44015F"), 1);
			break;
		}
	}
#endif
}



void CGvisR2R_PunchView::ThreadInit()
{
	if (!m_bThread[0])
		m_Thread[0].Start(GetSafeHwnd(), this, ThreadProc0);
	// 	if(!m_bThread[1])
	// 		m_Thread[1].Start(GetSafeHwnd(), this, ThreadProc1);
	// 	if(!m_bThread[2])
	// 		m_Thread[2].Start(GetSafeHwnd(), this, ThreadProc2);
}

void CGvisR2R_PunchView::ThreadKill()
{
	if (m_bThread[0])
	{
		m_Thread[0].Stop();
		Sleep(20);
		while (m_bThread[0])
		{
			Sleep(20);
		}
	}
	if (m_bThread[1])
	{
		m_Thread[1].Stop();
		Sleep(20);
		while (m_bThread[1])
		{
			Sleep(20);
		}
	}
	if (m_bThread[2])
	{
		m_Thread[2].Stop();
		Sleep(20);
		while (m_bThread[2])
		{
			Sleep(20);
		}
	}
	if (m_bThread[3])
	{
		m_Thread[3].Stop();
		Sleep(20);
		while (m_bThread[3])
		{
			Sleep(20);
		}
	}
	if (m_bThread[4])
	{
		m_Thread[4].Stop();
		Sleep(20);
		while (m_bThread[4])
		{
			Sleep(20);
		}
	}
	if (m_bThread[5])
	{
		CWinThread* pPointer = m_Thread[5].GetPointer();
		m_Thread[5].Stop();
		Sleep(20);
		//while (m_bThread[5])
		//{
		//	Sleep(20);
		//}
	}
	if (m_bThread[6])
	{
		m_Thread[6].Stop();
		Sleep(20);
		while (m_bThread[6])
		{
			Sleep(20);
		}
	}
	if (m_bThread[7])
	{
		m_Thread[7].Stop();
		Sleep(20);
		while (m_bThread[7])
		{
			Sleep(20);
		}
	}
	if (m_bThread[8])
	{
		m_Thread[8].Stop();
		Sleep(20);
		while (m_bThread[8])
		{
			Sleep(20);
		}
	}
	if (m_bThread[9])
	{
		m_Thread[9].Stop();
		Sleep(20);
		while (m_bThread[9])
		{
			Sleep(20);
		}
	}
	if (m_bThread[10])
	{
		m_Thread[10].Stop();
		Sleep(20);
		while (m_bThread[10])
		{
			Sleep(20);
		}
	}
	if (m_bThread[11])
	{
		m_Thread[11].Stop();
		Sleep(20);
		while (m_bThread[11])
		{
			Sleep(20);
		}
	}
	if (m_bThread[12])
	{
		m_Thread[12].Stop();
		Sleep(20);
		while (m_bThread[12])
		{
			Sleep(20);
		}
	}
	if (m_bThread[13])
	{
		m_Thread[13].Stop();
		Sleep(20);
		while (m_bThread[13])
		{
			Sleep(20);
		}
	}
	if (m_bThread[14])
	{
		m_Thread[14].Stop();
		Sleep(20);
		while (m_bThread[14])
		{
			Sleep(20);
		}
	}
	if (m_bThread[15])
	{
		m_Thread[15].Stop();
		Sleep(20);
		while (m_bThread[15])
		{
			Sleep(20);
		}
	}
	if (m_bThread[16])
	{
		m_Thread[16].Stop();
		Sleep(20);
		while (m_bThread[16])
		{
			Sleep(20);
		}
	}
	if (m_bThread[17])
	{
		m_Thread[17].Stop();
		Sleep(20);
		while (m_bThread[17])
		{
			Sleep(20);
		}
	}
	if (m_bThread[18])
	{
		m_Thread[18].Stop();
		Sleep(20);
		while (m_bThread[18])
		{
			Sleep(20);
		}
	}
	if (m_bThread[19])
	{
		m_Thread[19].Stop();
		Sleep(20);
		while (m_bThread[19])
		{
			Sleep(20);
		}
	}
	if (m_bThread[20])
	{
		m_Thread[20].Stop();
		Sleep(20);
		while (m_bThread[20])
		{
			Sleep(20);
		}
	}
	if (m_bThread[21])
	{
		m_Thread[21].Stop();
		Sleep(20);
		while (m_bThread[21])
		{
			Sleep(20);
		}
	}
	if (m_bThread[22])
	{
		m_Thread[22].Stop();
		Sleep(20);
		while (m_bThread[22])
		{
			Sleep(20);
		}
	}
	if (m_bThread[23])
	{
		m_Thread[23].Stop();
		Sleep(20);
		while (m_bThread[23])
		{
			Sleep(20);
		}
	}
	if (m_bThread[24])
	{
		m_Thread[24].Stop();
		Sleep(20);
		while (m_bThread[24])
		{
			Sleep(20);
		}
	}
	if (m_bThread[25])
	{
		m_Thread[25].Stop();
		Sleep(20);
		while (m_bThread[25])
		{
			Sleep(20);
		}
	}
	if (m_bThread[26])
	{
		m_Thread[26].Stop();
		Sleep(20);
		while (m_bThread[26])
		{
			Sleep(20);
		}
	}
	if (m_bThread[27])
	{
		m_Thread[27].Stop();
		Sleep(20);
		while (m_bThread[27])
		{
			Sleep(20);
		}
	}
	if (m_bThread[28])
	{
		m_Thread[28].Stop();
		Sleep(20);
		while (m_bThread[28])
		{
			Sleep(20);
		}
	}
	if (m_bThread[29])
	{
		m_Thread[29].Stop();
		Sleep(20);
		while (m_bThread[29])
		{
			Sleep(20);
		}
	}
	if (m_bThread[30])
	{
		m_Thread[30].Stop();
		Sleep(20);
		while (m_bThread[30])
		{
			Sleep(20);
		}
	}
	if (m_bThread[31])
	{
		m_Thread[31].Stop();
		Sleep(20);
		while (m_bThread[31])
		{
			Sleep(20);
		}
	}
	if (m_bThread[32])
	{
		m_Thread[32].Stop();
		Sleep(20);
		while (m_bThread[32])
		{
			Sleep(20);
		}
	}
	if (m_bThread[33])
	{
		m_Thread[33].Stop();
		Sleep(20);
		while (m_bThread[33])
		{
			Sleep(20);
		}
	}
	if (m_bThread[34])
	{
		m_Thread[34].Stop();
		Sleep(20);
		while (m_bThread[34])
		{
			Sleep(20);
		}
	}
	if (m_bThread[35])
	{
		m_Thread[35].Stop();
		Sleep(20);
		while (m_bThread[35])
		{
			Sleep(20);
		}
	}
	if (m_bThread[36])
	{
		m_Thread[36].Stop();
		Sleep(20);
		while (m_bThread[36])
		{
			Sleep(20);
		}
	}
	if (m_bThread[37])
	{
		m_Thread[37].Stop();
		Sleep(20);
		while (m_bThread[37])
		{
			Sleep(20);
		}
	}
	if (m_bThread[38])
	{
		m_Thread[38].Stop();
		Sleep(20);
		while (m_bThread[38])
		{
			Sleep(20);
		}
	}
}

UINT CGvisR2R_PunchView::ThreadProc0(LPVOID lpContext)	// DoMark0(), DoMark1()
{
	// Turn the passed in 'this' pointer back into a CProgressMgr instance
	CGvisR2R_PunchView* pThread = reinterpret_cast< CGvisR2R_PunchView* >(lpContext);

	BOOL bLock = FALSE;
	DWORD dwTick = GetTickCount();
	DWORD dwShutdownEventCheckPeriod = 0; // thread shutdown event check period

	pThread->m_bThread[0] = TRUE;
	while (WAIT_OBJECT_0 != WaitForSingleObject(pThread->m_Thread[0].GetShutdownEvent(), dwShutdownEventCheckPeriod))
	{
		pThread->m_dwThreadTick[0] = GetTickCount() - dwTick;
		dwTick = GetTickCount();

		if (!bLock)
		{
			bLock = TRUE;
#ifndef TEST_MODE
			if (pDoc->m_Master[0].MasterInfo.nActionCode == 1 || pDoc->m_Master[0].MasterInfo.nActionCode == 3)	// 0 : Rotation / Mirror ���� ����(CAM Data ����), 1 : �¿� �̷�, 2 : ���� �̷�, 3 : 180 ȸ��, 4 : 270 ȸ��(CCW), 5 : 90 ȸ��(CW)
			{
				if (pThread->m_bTHREAD_MK[0])
				{
					if (pThread->m_nBufUpSerial[0] > 0)
					{
						if (pDoc->GetTestMode() == MODE_OUTER)
							pThread->DoMark0Its();
						else
							pThread->DoMark0();
					}
					else
					{
						pThread->m_bDoneMk[0] = TRUE;
						pThread->m_bTHREAD_MK[0] = FALSE;
					}
				}
				if (pThread->m_bTHREAD_MK[1])
				{
					if (pThread->m_nBufUpSerial[1] > 0)
					{
						if (pDoc->GetTestMode() == MODE_OUTER)
							pThread->DoMark1Its();
						else
							pThread->DoMark1();
					}
					else
					{
						pThread->m_bDoneMk[1] = TRUE;
						pThread->m_bTHREAD_MK[1] = FALSE;
					}
				}
			}
			else
			{
				if (pThread->m_bTHREAD_MK[1])
				{
					if (pThread->m_nBufUpSerial[1] > 0)
						pThread->DoMark1();
					else
					{
						pThread->m_bDoneMk[1] = TRUE;
						pThread->m_bTHREAD_MK[1] = FALSE;
					}
				}
				if (pThread->m_bTHREAD_MK[0])
				{
					if (pThread->m_nBufUpSerial[0] > 0)
						pThread->DoMark0();
					else
					{
						pThread->m_bDoneMk[0] = TRUE;
						pThread->m_bTHREAD_MK[0] = FALSE;
					}
				}
			}
#else
			pThread->DoMark1();
			pThread->DoMark0();
			Sleep(100);
#endif
			if (pThread->m_bTHREAD_MK[3])
				pThread->DoReject1();
			// 				pThread->DoMark1All();
			if (pThread->m_bTHREAD_MK[2])
				pThread->DoReject0();
			// 				pThread->DoMark0All();

			if (!pThread->m_bTHREAD_MK[0] && !pThread->m_bTHREAD_MK[1] &&
				!pThread->m_bTHREAD_MK[2] && !pThread->m_bTHREAD_MK[3])
				Sleep(50);
			bLock = FALSE;
		}
		Sleep(10);
	}
	pThread->m_bThread[0] = FALSE;

	return 0;
}

UINT CGvisR2R_PunchView::ThreadProc1(LPVOID lpContext)	// ChkCollision()
{
	// Turn the passed in 'this' pointer back into a CProgressMgr instance
	CGvisR2R_PunchView* pThread = reinterpret_cast< CGvisR2R_PunchView* >(lpContext);

	BOOL bLock = FALSE, bEStop = FALSE, bCollision = FALSE;
	DWORD dwTick = GetTickCount();
	DWORD dwShutdownEventCheckPeriod = 0; // thread shutdown event check period

	pThread->m_bThread[1] = TRUE;
	while (WAIT_OBJECT_0 != WaitForSingleObject(pThread->m_Thread[1].GetShutdownEvent(), dwShutdownEventCheckPeriod))
	{
		pThread->m_dwThreadTick[1] = GetTickCount() - dwTick;
		dwTick = GetTickCount();

		if (!bLock)
		{
			bLock = TRUE;

			//pThread->ChkCollision();
			pThread->GetEnc();
			//pThread->GetInput();

			if (!pThread->m_bTHREAD_MK[0] && !pThread->m_bTHREAD_MK[1] &&
				!pThread->m_bTHREAD_MK[2] && !pThread->m_bTHREAD_MK[3])
			{
				if (pThread->ChkCollision() && !bEStop)
				{
					if (pThread->IsRunAxisX())
					{
						bEStop = TRUE;
						pThread->EStop();
					}
				}
				else if (!pThread->ChkCollision() && bEStop)
				{
					//if(pThread->IsRunAxisX())
					bEStop = FALSE;
				}
			}
			else
			{
				if (pThread->m_bCollision[0] && pThread->m_bCollision[1])
				{
					if (!bCollision)
					{
						bCollision = TRUE;
						pThread->SetPriority();
					}
				}
				else
					bCollision = FALSE;
			}

			bLock = FALSE;
		}
		Sleep(10);
	}
	pThread->m_bThread[1] = FALSE;

	return 0;
}

UINT CGvisR2R_PunchView::ThreadProc2(LPVOID lpContext)	// DispDefImg()
{
	// Turn the passed in 'this' pointer back into a CProgressMgr instance
	CGvisR2R_PunchView* pThread = reinterpret_cast< CGvisR2R_PunchView* >(lpContext);

	BOOL bLock = FALSE;
	DWORD dwTick = GetTickCount();
	DWORD dwShutdownEventCheckPeriod = 0; // thread shutdown event check period

	pThread->m_bThread[2] = TRUE;
	while (WAIT_OBJECT_0 != WaitForSingleObject(pThread->m_Thread[2].GetShutdownEvent(), dwShutdownEventCheckPeriod))
	{
		pThread->m_dwThreadTick[2] = GetTickCount() - dwTick;
		dwTick = GetTickCount();

		if (!bLock)
		{
			bLock = TRUE;

			if (pThread->m_bTHREAD_DISP_DEF)
			{
				pThread->DispDefImg();
				Sleep(0);
			}
			else
				Sleep(30);
			

			bLock = FALSE;
		}
	}
	pThread->m_bThread[2] = FALSE;

	return 0;
}

UINT CGvisR2R_PunchView::ThreadProc3(LPVOID lpContext)	// UpdateYield()
{
	// Turn the passed in 'this' pointer back into a CProgressMgr instance
	CGvisR2R_PunchView* pThread = reinterpret_cast<CGvisR2R_PunchView*>(lpContext);

	BOOL bLock = FALSE;
	DWORD dwTick = GetTickCount();
	DWORD dwShutdownEventCheckPeriod = 0; // thread shutdown event check period

	pThread->m_bThread[3] = TRUE;
	while (WAIT_OBJECT_0 != WaitForSingleObject(pThread->m_Thread[3].GetShutdownEvent(), dwShutdownEventCheckPeriod))
	{
		pThread->m_dwThreadTick[3] = GetTickCount() - dwTick;
		dwTick = GetTickCount();

		if (pThread->m_bTHREAD_UPDATAE_YIELD[0])
		{
			pThread->UpdateYield(pThread->m_nSerialTHREAD_UPDATAE_YIELD[0]);
			pThread->m_bTHREAD_UPDATAE_YIELD[0] = FALSE;
			Sleep(0);
		}
		else if(pThread->m_bTHREAD_UPDATAE_YIELD[1] && !pThread->m_bTHREAD_UPDATE_YIELD_UP)
		{
			if (pDoc->WorkingInfo.LastJob.bDualTest)
			{
				if (!pThread->m_bTHREAD_UPDATE_YIELD_DN && !pThread->m_bTHREAD_UPDATE_YIELD_ALLUP && !pThread->m_bTHREAD_UPDATE_YIELD_ALLDN)
				{
					pThread->UpdateYield(pThread->m_nSerialTHREAD_UPDATAE_YIELD[1]);
					pThread->m_bTHREAD_UPDATAE_YIELD[1] = FALSE;
					Sleep(0);
				}
			}
			else
			{
				pThread->m_bTHREAD_UPDATAE_YIELD[1] = FALSE;
				pThread->UpdateYield(pThread->m_nSerialTHREAD_UPDATAE_YIELD[1]);
				Sleep(0);
			}
		}
		else
			Sleep(30);
	}

	pThread->m_bThread[3] = FALSE;

	return 0;
}


UINT CGvisR2R_PunchView::ThreadProc4(LPVOID lpContext)	// RunShift2Mk()
{
	// Turn the passed in 'this' pointer back into a CProgressMgr instance
	CGvisR2R_PunchView* pThread = reinterpret_cast<CGvisR2R_PunchView*>(lpContext);

	BOOL bLock = FALSE;
	DWORD dwTick = GetTickCount();
	DWORD dwShutdownEventCheckPeriod = 0; // thread shutdown event check period

	pThread->m_bThread[4] = TRUE;
	while (WAIT_OBJECT_0 != WaitForSingleObject(pThread->m_Thread[4].GetShutdownEvent(), dwShutdownEventCheckPeriod))
	{
		pThread->m_dwThreadTick[4] = GetTickCount() - dwTick;
		dwTick = GetTickCount();

		if (pThread->m_bTHREAD_SHIFT2MK)
		{
			if (!pThread->m_bTHREAD_UPDATAE_YIELD[0] && !pThread->m_bTHREAD_UPDATAE_YIELD[1])
			{
				pThread->RunShift2Mk();
				pThread->m_bTHREAD_SHIFT2MK = FALSE;
				Sleep(0);
			}
			else
				Sleep(30);

		}
		else
			Sleep(30);
	}

	pThread->m_bThread[4] = FALSE;

	return 0;
}


UINT CGvisR2R_PunchView::ThreadProc5(LPVOID lpContext)	// GetCurrentInfoSignal()
{
	// Turn the passed in 'this' pointer back into a CProgressMgr instance
	CGvisR2R_PunchView* pThread = reinterpret_cast<CGvisR2R_PunchView*>(lpContext);

	BOOL bLock = FALSE;
	DWORD dwTick = GetTickCount();
	DWORD dwShutdownEventCheckPeriod = 0; // thread shutdown event check period

	pThread->m_bThread[5] = TRUE;
	while (WAIT_OBJECT_0 != WaitForSingleObject(pThread->m_Thread[5].GetShutdownEvent(), dwShutdownEventCheckPeriod))
	{
		pThread->m_dwThreadTick[5] = GetTickCount() - dwTick;
		dwTick = GetTickCount();

		if (!pThread->m_bDestroyedView)
		{
			if (!bLock)
			{
				bLock = TRUE;
				pThread->GetCurrentInfoSignal();
				bLock = FALSE;
			}
		}
		Sleep(100);
	}
	pThread->m_bThread[5] = FALSE;

	return 0;
}

void CGvisR2R_PunchView::DispStsMainMsg(int nIdx)
{
	CString str;
	str = m_sDispMsg[nIdx];
	pFrm->DispStatusBar(str, nIdx);
}

void CGvisR2R_PunchView::DispThreadTick()
{
	if (m_bDestroyedView)
		return;

	CString str;
	//str.Format(_T("%d"), m_dwThreadTick[1]);//, m_dwThreadTick[1], m_dwThreadTick[2]);
	//str.Format(_T("%d,%d,%d"), m_dwThreadTick[0], m_dwThreadTick[1], m_dwThreadTick[2]); // MK, Collision, Enc
	//if (m_sTick != str)
	{
		//m_sTick = str;
		//str.Format(_T("%d"), pDoc->m_nShotNum); // "m_sOrderNum-m_sShotNum" : "9-3"
		//str.Format(_T("%d%d%d%d:%d%d%d%d"),
		//	m_bTHREAD_UPDATE_REELMAP_UP ? 1 : 0, m_bTHREAD_UPDATE_REELMAP_DN ? 1 : 0, 
		//	m_bTHREAD_UPDATE_REELMAP_ALLUP ? 1 : 0, m_bTHREAD_UPDATE_REELMAP_ALLDN ? 1 : 0,
		//	m_bTHREAD_REELMAP_YIELD_UP ? 1 : 0, m_bTHREAD_REELMAP_YIELD_DN ? 1 : 0, 
		//	m_bTHREAD_REELMAP_YIELD_ALLUP ? 1 : 0, m_bTHREAD_REELMAP_YIELD_ALLDN ? 1 : 0);
		str.Format(_T("%d%d%d%d:%d%d"),
			m_bTHREAD_UPDATE_REELMAP_UP ? 1 : 0, m_bTHREAD_UPDATE_REELMAP_DN ? 1 : 0,
			m_bTHREAD_UPDATE_REELMAP_ALLUP ? 1 : 0, m_bTHREAD_UPDATE_REELMAP_ALLDN ? 1 : 0,
			m_bTHREAD_REELMAP_YIELD_UP ? 1 : 0, m_bTHREAD_REELMAP_YIELD_DN ? 1 : 0);
		pFrm->DispStatusBar(str, 5);
#ifdef USE_IDS
		double dFPS[2];
		if (m_pVision[0])
			m_pVision[0]->GetFramesPerSecond(&dFPS[0]);
		if (m_pVision[1])
			m_pVision[1]->GetFramesPerSecond(&dFPS[1]);
		str.Format(_T("%.1f,%.1f"), dFPS[0], dFPS[1]);
		pFrm->DispStatusBar(str, 6);
#else
		//str.Format(_T("%d"), m_nDebugStep);
		str.Format(_T("%d,%d,%d,%d"), m_nStepAuto, m_nMkStAuto, m_nStepMk[0], m_nStepMk[1]);//pView->m_nLotEndAuto
		pFrm->DispStatusBar(str, 6);
#endif
	}
}

void CGvisR2R_PunchView::SwJog(int nAxisID, int nDir, BOOL bOn)
{
	if (!pView->m_pMotion)// || pDoc->Status.bAuto)
		return;

	double fVel, fAcc, fJerk;
	double dStep;
	if (pDoc->Status.bSwJogFast)
		dStep = 0.5;
	else
		dStep = 0.1;

	if (pDoc->Status.bSwJogStep)
	{
		if (bOn)
		{
			double dPos = pView->m_dEnc[nAxisID]; // m_pMotion->GetActualPosition(nAxisID);
			if (nDir == M_CW)
				dPos += dStep;
			else if (nDir == M_CCW)
				dPos -= dStep;

			if (nAxisID == AXIS_X0)
			{
				if (m_bAuto && m_bTHREAD_MK[0] && m_bTHREAD_MK[1] && IsReview())
				{
					if (nDir == M_CW) // �� Jog ��ư.
						return;
				}

				if (m_pMotion->IsLimit(MS_X0, nDir))
					return;
				m_pMotion->GetSpeedProfile(TRAPEZOIDAL, AXIS_X0, dStep, fVel, fAcc, fJerk);
				m_pMotion->Move(MS_X0, dPos, fVel, fAcc, fAcc);
			}
			else if (nAxisID == AXIS_Y0)
			{
				if (m_pMotion->IsLimit(MS_Y0, nDir))
					return;
				m_pMotion->GetSpeedProfile(TRAPEZOIDAL, AXIS_Y0, dStep, fVel, fAcc, fJerk);
				m_pMotion->Move(MS_Y0, dPos, fVel, fAcc, fAcc);
			}
			else if (nAxisID == AXIS_X1)
			{
				if (m_bAuto && m_bTHREAD_MK[0] && m_bTHREAD_MK[1] && IsReview())
				{
					if (nDir == M_CW) // �� Jog ��ư.
						return;
				}

				if (m_pMotion->IsLimit(MS_X1, nDir))
					return;
				m_pMotion->GetSpeedProfile(TRAPEZOIDAL, AXIS_X1, dStep, fVel, fAcc, fJerk);
				m_pMotion->Move(MS_X1, dPos, fVel, fAcc, fAcc);
			}
			else if (nAxisID == AXIS_Y1)
			{
				if (m_pMotion->IsLimit(MS_Y1, nDir))
					return;
				m_pMotion->GetSpeedProfile(TRAPEZOIDAL, AXIS_Y1, dStep, fVel, fAcc, fJerk);
				m_pMotion->Move(MS_Y1, dPos, fVel, fAcc, fAcc);
			}
		}
	}
	else	// Jog Mode
	{
		if (!m_pDlgMenu02)
			return;

		if (nAxisID == AXIS_Y0)
		{
			if (nDir == M_CCW)		// Up
			{
				if (bOn)
					m_pDlgMenu02->SwMyBtnDown(IDC_BTN_JOG_UP);
				else
					m_pDlgMenu02->SwMyBtnUp(IDC_BTN_JOG_UP);
			}
			else if (nDir == M_CW)	// Dn
			{
				if (bOn)
					m_pDlgMenu02->SwMyBtnDown(IDC_BTN_JOG_DN);
				else
					m_pDlgMenu02->SwMyBtnUp(IDC_BTN_JOG_DN);
			}
		}
		else if (nAxisID == AXIS_X0)
		{
			if (m_bAuto && m_bTHREAD_MK[0] && m_bTHREAD_MK[1] && IsReview())
			{
				if (nDir == M_CW) // �� Jog ��ư.
					return;
			}

			if (nDir == M_CW)		// Rt
			{
				if (bOn)
					m_pDlgMenu02->SwMyBtnDown(IDC_BTN_JOG_RT);
				else
					m_pDlgMenu02->SwMyBtnUp(IDC_BTN_JOG_RT);
			}
			else if (nDir == M_CCW)	// Lf
			{
				if (bOn)
					m_pDlgMenu02->SwMyBtnDown(IDC_BTN_JOG_LF);
				else
					m_pDlgMenu02->SwMyBtnUp(IDC_BTN_JOG_LF);
			}
		}
		else if (nAxisID == AXIS_Y1)
		{
			if (nDir == M_CCW)		// Up
			{
				if (bOn)
					m_pDlgMenu02->SwMyBtnDown(IDC_BTN_JOG_UP2);
				else
					m_pDlgMenu02->SwMyBtnUp(IDC_BTN_JOG_UP2);
			}
			else if (nDir == M_CW)	// Dn
			{
				if (bOn)
					m_pDlgMenu02->SwMyBtnDown(IDC_BTN_JOG_DN2);
				else
					m_pDlgMenu02->SwMyBtnUp(IDC_BTN_JOG_DN2);
			}
		}
		else if (nAxisID == AXIS_X1)
		{
			if (m_bAuto && m_bTHREAD_MK[0] && m_bTHREAD_MK[1] && IsReview())
			{
				if (nDir == M_CW) // �� Jog ��ư.
					return;
			}

			if (nDir == M_CW)		// Rt
			{
				if (bOn)
					m_pDlgMenu02->SwMyBtnDown(IDC_BTN_JOG_RT2);
				else
					m_pDlgMenu02->SwMyBtnUp(IDC_BTN_JOG_RT2);
			}
			else if (nDir == M_CCW)	// Lf
			{
				if (bOn)
					m_pDlgMenu02->SwMyBtnDown(IDC_BTN_JOG_LF2);
				else
					m_pDlgMenu02->SwMyBtnUp(IDC_BTN_JOG_LF2);
			}
		}
	}
}


BOOL CGvisR2R_PunchView::ChkShareIdx(int *pBufSerial, int nBufTot, int nShareSerial)
{
	if (nBufTot<1)
		return TRUE;
	for (int i = 0; i<nBufTot; i++)
	{
		if (pBufSerial[i] == nShareSerial)
			return FALSE;
	}
	return TRUE;
}

BOOL CGvisR2R_PunchView::ChkVsShare(int &nSerial)
{
	int nS0, nS1;
	BOOL b0 = ChkVsShareUp(nS0);
	BOOL b1 = ChkVsShareDn(nS1);

	if (!b0 || !b1)
	{
		nSerial = -1;
		return FALSE;
	}
	else if (nS0 != nS1)
	{
		nSerial = -1;
		return FALSE;
	}

	nSerial = nS0;
	return TRUE;
}

BOOL CGvisR2R_PunchView::ChkVsShareUp(int &nSerial)
{
	CFileFind cFile;
	BOOL bExist = cFile.FindFile(pDoc->WorkingInfo.System.sPathVsShareUp + _T("*.pcr"));
	if (!bExist)
		return FALSE; // pcr������ �������� ����.

	int nPos;
	CString sFileName, sSerial;
	while (bExist)
	{
		bExist = cFile.FindNextFile();
		if (cFile.IsDots()) continue;
		if (!cFile.IsDirectory())
		{
			// ���ϸ��� ����.
			sFileName = cFile.GetFileName();
			nPos = sFileName.ReverseFind('.');
			if (nPos > 0)
				sSerial = sFileName.Left(nPos);

			nSerial = _tstoi(sSerial);
			if (nSerial > 0)
				return TRUE;
			else
			{
				nSerial = 0;
				return FALSE;
			}
		}
	}

	return FALSE;
}

BOOL CGvisR2R_PunchView::ChkVsShareDn(int &nSerial)
{
	CFileFind cFile;
	BOOL bExist = cFile.FindFile(pDoc->WorkingInfo.System.sPathVsShareDn + _T("*.pcr"));
	if (!bExist)
		return FALSE; // pcr������ �������� ����.

	int nPos;
	CString sFileName, sSerial;
	while (bExist)
	{
		bExist = cFile.FindNextFile();
		if (cFile.IsDots()) continue;
		if (!cFile.IsDirectory())
		{
			// ���ϸ��� ����.
			sFileName = cFile.GetFileName();
			nPos = sFileName.ReverseFind('.');
			if (nPos > 0)
				sSerial = sFileName.Left(nPos);

			nSerial = _tstoi(sSerial);
			if (nSerial > 0)
				return TRUE;
			else
			{
				nSerial = 0;
				return FALSE;
			}
		}
	}

	return FALSE;
}

BOOL CGvisR2R_PunchView::ChkShare(int &nSerial)
{
	int nS0, nS1;
	BOOL b0 = ChkShareUp(nS0);
	BOOL b1 = ChkShareDn(nS1);

	if (!b0 || !b1)
	{
		nSerial = -1;
		return FALSE;
	}
	else if (nS0 != nS1)
	{
		nSerial = -1;
		return FALSE;
	}

	nSerial = nS0;
	return TRUE;
}

BOOL CGvisR2R_PunchView::ChkShareUp(int &nSerial)
{
	CFileFind cFile;
	BOOL bExist = cFile.FindFile(pDoc->WorkingInfo.System.sPathVrsShareUp + _T("*.pcr"));
	if (!bExist)
		return FALSE; // pcr������ �������� ����.

	int nPos;
	CString sFileName, sSerial;
	while (bExist)
	{
		bExist = cFile.FindNextFile();
		if (cFile.IsDots()) continue;
		if (!cFile.IsDirectory())
		{
			// ���ϸ��� ����.
			sFileName = cFile.GetFileName();
			nPos = sFileName.ReverseFind('.');
			if (nPos > 0)
				sSerial = sFileName.Left(nPos);

			nSerial = _tstoi(sSerial);
			if (nSerial > 0)
				return TRUE;
			else
			{
				nSerial = 0;
				return FALSE;
			}
		}
	}

	return FALSE;
}

BOOL CGvisR2R_PunchView::ChkShareDn(int &nSerial)
{
	CFileFind cFile;
	BOOL bExist = cFile.FindFile(pDoc->WorkingInfo.System.sPathVrsShareDn + _T("*.pcr"));
	if (!bExist)
		return FALSE; // pcr������ �������� ����.

	int nPos;
	CString sFileName, sSerial;
	while (bExist)
	{
		bExist = cFile.FindNextFile();
		if (cFile.IsDots()) continue;
		if (!cFile.IsDirectory())
		{
			// ���ϸ��� ����.
			sFileName = cFile.GetFileName();
			nPos = sFileName.ReverseFind('.');
			if (nPos > 0)
				sSerial = sFileName.Left(nPos);

			nSerial = _tstoi(sSerial);
			if (nSerial > 0)
				return TRUE;
			else
			{
				nSerial = 0;
				return FALSE;
			}
		}
	}

	return FALSE;
}

void CGvisR2R_PunchView::ChkShare()
{
	ChkShareUp();
	ChkShareDn();
}

void CGvisR2R_PunchView::ChkShareUp()
{
	CString str, str2;
	int nSerial;
	if (ChkShareUp(nSerial))
	{
		str.Format(_T("US: %d"), nSerial);
		pDoc->Status.PcrShare[0].bExist = TRUE;
		pDoc->Status.PcrShare[0].nSerial = nSerial;
		if (m_pMpe)
		{
			str2.Format(_T("PCR���� Received - US: %d"), nSerial);
			pDoc->LogAuto(str2);

			if (IsVs())
			{
				if (IsSetLotEnd())
				{
					if ((long)GetAoiUpAutoSerial() - 1 <= GetLotEndSerial())
						pView->m_pMpe->Write(_T("ML45112"), (long)GetAoiUpAutoSerial() - 1);	// �˻��� Panel�� AOI �� Serial
				}
				else
					pView->m_pMpe->Write(_T("ML45112"), (long)GetAoiUpAutoSerial() - 1);	// �˻��� Panel�� AOI �� Serial


				//if(m_bLastProcFromEng)
				//	pView->m_pMpe->Write(_T("ML45112"), (long)nSerial + 3);	// �˻��� Panel�� AOI �� Serial
				//else

				//if (IsSetLotEnd())
				//{
				//	if ((long)nSerial + 3 <= GetLotEndSerial())
				//		pView->m_pMpe->Write(_T("ML45112"), (long)nSerial + 3);	// �˻��� Panel�� AOI �� Serial
				//}
				//else
				//	pView->m_pMpe->Write(_T("ML45112"), (long)nSerial + 3);	// �˻��� Panel�� AOI �� Serial
			}
			else
				pView->m_pMpe->Write(_T("ML45112"), (long)nSerial);	// �˻��� Panel�� AOI �� Serial

			m_pMpe->Write(_T("MB44012B"), 1); // AOI �� : PCR���� Received
		}
	}
	else
	{
		pDoc->Status.PcrShare[0].bExist = FALSE;
		pDoc->Status.PcrShare[0].nSerial = -1;
		str.Format(_T("US: "));
	}
	if (pFrm)
	{
		if (m_sShare[0] != str)
		{
			m_sShare[0] = str;
			pFrm->DispStatusBar(str, 4);
		}
	}
}

void CGvisR2R_PunchView::ChkShareDn()
{
	CString str, str2;
	int nSerial;
	if (ChkShareDn(nSerial))
	{
		str.Format(_T("DS: %d"), nSerial);
		pDoc->Status.PcrShare[1].bExist = TRUE;
		pDoc->Status.PcrShare[1].nSerial = nSerial;
		if (m_pMpe)
		{
			str2.Format(_T("PCR���� Received - DS: %d"), nSerial);
			pDoc->LogAuto(str2);

			if (IsVs())
			{
				if (IsSetLotEnd())
				{
					if ((long)GetAoiDnAutoSerial() - 1 <= GetLotEndSerial())
						pView->m_pMpe->Write(_T("ML45114"), (long)GetAoiDnAutoSerial() - 1);	// �˻��� Panel�� AOI �� Serial
				}
				else
					pView->m_pMpe->Write(_T("ML45114"), (long)GetAoiDnAutoSerial() - 1);	// �˻��� Panel�� AOI �� Serial


				//if (m_bLastProcFromEng)
				//	pView->m_pMpe->Write(_T("ML45114"), (long)nSerial + 3);	// �˻��� Panel�� AOI �� Serial
				//else

				//if (IsSetLotEnd())
				//{
				//	if ((long)nSerial + 4 <= GetLotEndSerial())
				//		pView->m_pMpe->Write(_T("ML45114"), (long)nSerial + 4);	// �˻��� Panel�� AOI �� Serial
				//}
				//else
				//	pView->m_pMpe->Write(_T("ML45114"), (long)nSerial + 4);	// �˻��� Panel�� AOI �� Serial
			}
			else
				pView->m_pMpe->Write(_T("ML45114"), (long)nSerial);	// �˻��� Panel�� AOI �� Serial

			m_pMpe->Write(_T("MB44012C"), 1); // AOI �� : PCR���� Received
		}
	}
	else
	{
		pDoc->Status.PcrShare[1].bExist = FALSE;
		pDoc->Status.PcrShare[1].nSerial = -1;
		str.Format(_T("DS: "));
	}
	if (pFrm)
	{
		if (m_sShare[1] != str)
		{
			m_sShare[1] = str;
			pFrm->DispStatusBar(str, 2);
		}
	}
}

BOOL CGvisR2R_PunchView::ChkBufIdx(int* pSerial, int nTot)
{
	if (nTot < 2)
		return TRUE;

	for (int i = 0; i<(nTot - 1); i++)
	{
		if (pSerial[i + 1] != pSerial[i] + 1)
			return FALSE;
	}
	return TRUE;
}


void CGvisR2R_PunchView::SwapUp(__int64 *num1, __int64 *num2) 	// ��ġ �ٲٴ� �Լ�
{
	__int64 temp;

	temp = *num2;
	*num2 = *num1;
	*num1 = temp;
}

BOOL CGvisR2R_PunchView::SortingInUp(CString sPath, int nIndex)
{
	struct _stat buf;
	struct tm *t;

	CString sMsg, sFileName, sSerial;
	int nPos, nSerial;
	char filename[MAX_PATH];
	StringToChar(sPath, filename);

	if (_stat(filename, &buf) != 0)
	{
		sMsg.Format(_T("�Ͻ����� - Failed getting information."));
		//MsgBox(sMsg);
		pView->ClrDispMsg();
		AfxMessageBox(sMsg);
		return FALSE;
	}
	else
	{
		sFileName = sPath;
		nPos = sFileName.ReverseFind('.');
		if (nPos > 0)
		{
			sSerial = sFileName.Left(nPos);
			sSerial = sSerial.Right(4);
		}

		nSerial = _tstoi(sSerial);

		t = localtime(&buf.st_ctime);
		//t = localtime(&buf.st_mtime);

		//sFileName.Format(_T("%04d%02d%02d%02d%02d%02d%04d"), t->tm_year + 1900, t->tm_mon + 1, t->tm_mday, 
		//														t->tm_hour, t->tm_min, t->tm_sec, nSerial);
		//m_nBufSerialSorting[0][nIndex] = _tstoi(sFileName);

		CString sYear, sMonth, sDay, sHour, sMin, sSec;
		sYear.Format(_T("%04d"), t->tm_year + 1900);
		sMonth.Format(_T("%02d"), t->tm_mon + 1);
		sDay.Format(_T("%02d"), t->tm_mday);
		sHour.Format(_T("%02d"), t->tm_hour);
		sMin.Format(_T("%02d"), t->tm_min);
		sSec.Format(_T("%02d"), t->tm_sec);

		__int64 nYear = _tstoi(sYear);
		__int64 nMonth = _tstoi(sMonth);
		__int64 nDay = _tstoi(sDay);
		__int64 nHour = _tstoi(sHour);
		__int64 nMin = _tstoi(sMin);
		__int64 nSec = _tstoi(sSec);

		m_nBufSerialSorting[0][nIndex] = nYear * 100000000000000 + nMonth * 1000000000000 + nDay * 10000000000 +
			nHour * 100000000 + nMin * 1000000 + nSec * 10000 + nSerial;
	}

	return TRUE;
}

BOOL CGvisR2R_PunchView::SortingOutUp(int* pSerial, int nTot)
{
	int i, k;

	for (k = 0; k < nTot; k++) 			// ���� ���� �ҽ� ����
	{
		for (i = 0; i < (nTot - 1) - k; i++)
		{

			if (m_nBufSerialSorting[0][i] > m_nBufSerialSorting[0][i + 1])
			{
				SwapUp(&m_nBufSerialSorting[0][i + 1], &m_nBufSerialSorting[0][i]);
			}
		}
	}									// ���� ���� �ҽ� ��

	for (i = 0; i < nTot; i++)
	{
		pSerial[i] = (int)(m_nBufSerialSorting[0][i] % 10000);
	}
	return TRUE;
}

BOOL CGvisR2R_PunchView::ChkBufUp(int* pSerial, int &nTot)
{
	CFileFind cFile;
	BOOL bExist = cFile.FindFile(pDoc->WorkingInfo.System.sPathVrsBufUp + _T("*.pcr"));
	if (!bExist)
	{
		pDoc->m_bBufEmpty[0] = TRUE;
		if (!pDoc->m_bBufEmptyF[0])
			pDoc->m_bBufEmptyF[0] = TRUE;		// ���� �ѹ� ���۰� ���������(�ʱ�ȭ�� �ϰ� �� ����) TRUE.

		return FALSE; // pcr������ �������� ����.
	}

	int nPos, nSerial;

	CString sFileName, sSerial;
	CString sNewName;

	nTot = 0;
	while (bExist)
	{
		bExist = cFile.FindNextFile();
		if (cFile.IsDots()) continue;
		if (!cFile.IsDirectory())
		{
			sFileName = cFile.GetFileName();

			if (!SortingInUp(pDoc->WorkingInfo.System.sPathVrsBufUp + sFileName, nTot))
				return FALSE;

			nTot++;
		}
	}

	BOOL bRtn = SortingOutUp(pSerial, nTot);

	if (nTot == 0)
		pDoc->m_bBufEmpty[0] = TRUE;
	else
	{
		pDoc->m_bBufEmpty[0] = FALSE;
		m_bIsBuf[0] = TRUE;
	}

	return (bRtn);
}

BOOL CGvisR2R_PunchView::ChkBufDn(int* pSerial, int &nTot)
{
	CFileFind cFile;
	BOOL bExist = cFile.FindFile(pDoc->WorkingInfo.System.sPathVrsBufDn + _T("*.pcr"));
	if (!bExist)
	{
		pDoc->m_bBufEmpty[1] = TRUE;
		if (!pDoc->m_bBufEmptyF[1])
			pDoc->m_bBufEmptyF[1] = TRUE;
		return FALSE; // pcr������ �������� ����.
	}

	int nPos, nSerial;

	CString sFileName, sSerial;
	nTot = 0;
	while (bExist)
	{
		bExist = cFile.FindNextFile();
		if (cFile.IsDots()) continue;
		if (!cFile.IsDirectory())
		{
			sFileName = cFile.GetFileName();
			//nPos = sFileName.ReverseFind('.');
			//if (nPos > 0)
			//	sSerial = sFileName.Left(nPos);

			//nSerial = _tstoi(sSerial);
			//if (nSerial > 0)
			//{
			//	pSerial[nTot] = nSerial;
			//	nTot++;
			//}

			if (!SortingInDn(pDoc->WorkingInfo.System.sPathVrsBufDn + sFileName, nTot))
				return FALSE;

			nTot++;
		}
	}


	BOOL bRtn = SortingOutDn(pSerial, nTot);

	if (nTot == 0)
		pDoc->m_bBufEmpty[1] = TRUE;
	else
	{
		pDoc->m_bBufEmpty[1] = FALSE;
		m_bIsBuf[1] = TRUE;
	}

	return (bRtn);
	//return TRUE;
}

void CGvisR2R_PunchView::SwapDn(__int64 *num1, __int64 *num2) 	// ��ġ �ٲٴ� �Լ�
{
	__int64 temp;

	temp = *num2;
	*num2 = *num1;
	*num1 = temp;
}

BOOL CGvisR2R_PunchView::SortingInDn(CString sPath, int nIndex)
{
	struct _stat buf;
	struct tm *t;

	CString sMsg, sFileName, sSerial;
	int nPos, nSerial;
	char filename[MAX_PATH];
	StringToChar(sPath, filename);

	if (_stat(filename, &buf) != 0)
	{
		sMsg.Format(_T("�Ͻ����� - Failed getting information."));
		//MsgBox(sMsg);
		pView->ClrDispMsg();
		AfxMessageBox(sMsg);
		return FALSE;
	}
	else
	{
		sFileName = sPath;
		nPos = sFileName.ReverseFind('.');
		if (nPos > 0)
		{
			sSerial = sFileName.Left(nPos);
			sSerial = sSerial.Right(4);
		}

		nSerial = _tstoi(sSerial);

		t = localtime(&buf.st_ctime);
		//t = localtime(&buf.st_mtime);

		//sFileName.Format(_T("%04d%02d%02d%02d%02d%02d%04d"), t->tm_year + 1900, t->tm_mon + 1, t->tm_mday, 
		//														t->tm_hour, t->tm_min, t->tm_sec, nSerial);
		//m_nBufSerialSorting[1][nIndex] = _tstoi(sFileName);


		CString sYear, sMonth, sDay, sHour, sMin, sSec;
		sYear.Format(_T("%04d"), t->tm_year + 1900);
		sMonth.Format(_T("%02d"), t->tm_mon + 1);
		sDay.Format(_T("%02d"), t->tm_mday);
		sHour.Format(_T("%02d"), t->tm_hour);
		sMin.Format(_T("%02d"), t->tm_min);
		sSec.Format(_T("%02d"), t->tm_sec);

		__int64 nYear = _tstoi(sYear);
		__int64 nMonth = _tstoi(sMonth);
		__int64 nDay = _tstoi(sDay);
		__int64 nHour = _tstoi(sHour);
		__int64 nMin = _tstoi(sMin);
		__int64 nSec = _tstoi(sSec);

		m_nBufSerialSorting[1][nIndex] = nYear * 100000000000000 + nMonth * 1000000000000 + nDay * 10000000000 +
			nHour * 100000000 + nMin * 1000000 + nSec * 10000 + nSerial;

	}

	return TRUE;
}

BOOL CGvisR2R_PunchView::SortingOutDn(int* pSerial, int nTot)
{
	int i, k;

	for (k = 0; k < nTot; k++) 			// ���� ���� �ҽ� ����
	{
		for (i = 0; i < (nTot - 1) - k; i++)
		{

			if (m_nBufSerialSorting[1][i] > m_nBufSerialSorting[1][i + 1])
			{
				SwapUp(&m_nBufSerialSorting[1][i + 1], &m_nBufSerialSorting[1][i]);
			}
		}
	}									// ���� ���� �ҽ� ��

	for (i = 0; i < nTot; i++)
	{
		pSerial[i] = (int)(m_nBufSerialSorting[1][i] % 10000);
	}
	return TRUE;
}

BOOL CGvisR2R_PunchView::ChkYield() // ���� ��ȣ : TRUE , ���� �ҷ� : FALSE
{
	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;

	int nGood = 0, nBad = 0, nTot;
	double dRatio;
	CString sMsg;

	if (bDualTest)
	{
		if (pDoc->m_pReelMapAllDn)
			pDoc->m_pReelMapAllDn->GetPcsNum(nGood, nBad);
		else
		{
			Buzzer(TRUE, 0);
			TowerLamp(RGB_RED, TRUE);
			Stop();
			sMsg.Format(_T("�Ͻ����� - Failed ChkYield()."));
			MsgBox(sMsg);
			return FALSE;
		}
	}
	else
	{
		if (pDoc->m_pReelMapUp)
			pDoc->m_pReelMapUp->GetPcsNum(nGood, nBad);
		else
		{
			Buzzer(TRUE, 0);
			TowerLamp(RGB_RED, TRUE);
			Stop();
			sMsg.Format(_T("�Ͻ����� - Failed ChkYield()."));
			MsgBox(sMsg);
			return FALSE;
		}
	}

	nTot = (nGood + nBad);
	double dTotLmt = _tstof(pDoc->WorkingInfo.LastJob.sLmtTotYld);
	if (dTotLmt > 0.0)
	{
		if (nTot > 0)
			dRatio = ((double)nGood / (double)nTot) * 100.0;
		else
			dRatio = 0.0;

		if (dRatio < dTotLmt)
		{
			Buzzer(TRUE, 0);
			TowerLamp(RGB_RED, TRUE);
			Stop();
			sMsg.Format(_T("�Ͻ����� - ��ü ���� ���ѹ��� : %.1f �̴� ( %.1f )"), dTotLmt, dRatio);
			MsgBox(sMsg);
			return FALSE;
		}
	}

	// 	int nNodeX = pDoc->m_pPcsRgn->nCol;
	// 	int nNodeY = pDoc->m_pPcsRgn->nRow;
	// 	nTot = (nNodeX*nNodeY);
	// 	int nSerial = m_pDlgMenu01->GetCurSerial();
	// 	int nIdx = pDoc->GetPcrIdx(nSerial);
	// 	int nDef = pDoc->m_pPcr[nIdx]->m_nTotRealDef;

	double dPrtLmt = _tstof(pDoc->WorkingInfo.LastJob.sLmtPatlYld);
	if (dPrtLmt > 0.0)
	{
		// 		if(nDef > 0)
		// 			dRatio = ((double)(nTot-nDef)/(double)nTot) * 100.0;
		// 		else
		// 			dRatio = 0.0;

		if (bDualTest)
		{
			if (pDoc->m_Yield[2].IsValid())
			{
				dRatio = pDoc->m_Yield[2].GetYield();

				if (dRatio < dPrtLmt)
				{
					Buzzer(TRUE, 0);
					TowerLamp(RGB_RED, TRUE);
					Stop();
					sMsg.Format(_T("�Ͻ����� - ���� ���� ���ѹ��� : %.1f �̴� ( %.1f )"), dPrtLmt, dRatio);
					MsgBox(sMsg);
					return FALSE;
				}
			}
		}
		else
		{
			if (pDoc->m_Yield[0].IsValid())
			{
				dRatio = pDoc->m_Yield[0].GetYield();

				if (dRatio < dPrtLmt)
				{
					Buzzer(TRUE, 0);
					TowerLamp(RGB_RED, TRUE);
					Stop();
					sMsg.Format(_T("�Ͻ����� - ���� ���� ���ѹ��� : %.1f �̴� ( %.1f )"), dPrtLmt, dRatio);
					MsgBox(sMsg);
					return FALSE;
				}
			}
		}
	}

	return TRUE;
}

BOOL CGvisR2R_PunchView::ChkSaftySen() // ���� : TRUE , ���� : FALSE
{
	if (pDoc->WorkingInfo.LastJob.bMkSftySen)
	{
		if (pDoc->Status.bSensSaftyMk && !pDoc->Status.bSensSaftyMkF)
		{
			pDoc->Status.bSensSaftyMkF = TRUE;
			m_bSwStopNow = TRUE;
			m_bSwRunF = FALSE;
			Buzzer(TRUE, 0);
			TowerLamp(RGB_RED, TRUE);
			Stop();
			//pView->DispStsBar(_T("����-4"), 0);
			DispMain(_T("�� ��"), RGB_RED);			
			m_bTIM_SAFTY_STOP = TRUE;//MsgBox(_T("�Ͻ����� - ��ŷ�� ���������� �����Ǿ����ϴ�."));
			SetTimer(TIM_SAFTY_STOP, 100, NULL);
		}
		else if (!pDoc->Status.bSensSaftyMk && pDoc->Status.bSensSaftyMkF)
		{
			pDoc->Status.bSensSaftyMkF = FALSE;
			Buzzer(FALSE, 0);
			Sleep(300);
		}
	}

	return (pDoc->Status.bSensSaftyMk);
}

void CGvisR2R_PunchView::ResetMotion()
{
	for (int i = 0; i<MAX_MS; i++)
	{
		ResetMotion(i);

		if (i<MAX_AXIS)
		{
			while (!m_pMotion->IsServoOn(i))
			{
				if (i == MS_X0 || i == MS_Y0)
					pView->m_pMotion->Clear(MS_X0Y0);
				else if (i == MS_X1 || i == MS_Y1)
					pView->m_pMotion->Clear(MS_X1Y1);
				else
					pView->m_pMotion->Clear(i);
				// 				for(int k=0; k<MAX_MS; k++)
				// 					m_pMotion->Clear(k);
				Sleep(30);
				m_pMotion->ServoOnOff(i, TRUE);
				Sleep(30);
			}
		}
	}
}

void CGvisR2R_PunchView::ResetMotion(int nMsId)
{
	if (m_pDlgMenu02)
		m_pDlgMenu02->ResetMotion(nMsId);
}

unsigned long CGvisR2R_PunchView::ChkDoor() // 0: All Closed , Open Door Index : Doesn't all closed. (Bit3: F, Bit2: L, Bit1: R, Bit0; B)
{
	unsigned long ulOpenDoor = 0;

	if (pDoc->WorkingInfo.LastJob.bAoiUpDrSen)
	{
		if (pDoc->Status.bDoorAoi[DOOR_FM_AOI_UP] && !pDoc->Status.bDoorAoiF[DOOR_FM_AOI_UP])
		{
			ulOpenDoor |= (0x01 << 0);
			pDoc->Status.bDoorAoiF[DOOR_FM_AOI_UP] = TRUE;
			Buzzer(FALSE, 0);
			Sleep(300);
		}
		else if (!pDoc->Status.bDoorAoi[DOOR_FM_AOI_UP] && pDoc->Status.bDoorAoiF[DOOR_FM_AOI_UP])
		{
			ulOpenDoor &= ~(0x01 << 0);
			pDoc->Status.bDoorAoiF[DOOR_FM_AOI_UP] = FALSE;
			m_bSwStopNow = TRUE;
			m_bSwRunF = FALSE;
			Buzzer(TRUE, 0);
			TowerLamp(RGB_RED, TRUE);
			Stop();
			//pView->DispStsBar(_T("����-5"), 0);
			DispMain(_T("�� ��"), RGB_RED);
			MsgBox(_T("�Ͻ����� - �˻�� �� ���� �߾� ���� Open"));
		}

		if (pDoc->Status.bDoorAoi[DOOR_FL_AOI_UP] && !pDoc->Status.bDoorAoiF[DOOR_FL_AOI_UP])
		{
			ulOpenDoor |= (0x01 << 1);
			pDoc->Status.bDoorAoiF[DOOR_FL_AOI_UP] = TRUE;
			Buzzer(FALSE, 0);
			Sleep(300);
		}
		else if (!pDoc->Status.bDoorAoi[DOOR_FL_AOI_UP] && pDoc->Status.bDoorAoiF[DOOR_FL_AOI_UP])
		{
			ulOpenDoor &= ~(0x01 << 1);
			pDoc->Status.bDoorAoiF[DOOR_FL_AOI_UP] = FALSE;
			m_bSwStopNow = TRUE;
			m_bSwRunF = FALSE;
			Buzzer(TRUE, 0);
			TowerLamp(RGB_RED, TRUE);
			Stop();
			//pView->DispStsBar(_T("����-6"), 0);
			DispMain(_T("�� ��"), RGB_RED);
			MsgBox(_T("�Ͻ����� - �˻�� �� ���� ���� ���� Open"));
		}

		if (pDoc->Status.bDoorAoi[DOOR_FR_AOI_UP] && !pDoc->Status.bDoorAoiF[DOOR_FR_AOI_UP])
		{
			ulOpenDoor |= (0x01 << 2);
			pDoc->Status.bDoorAoiF[DOOR_FR_AOI_UP] = TRUE;
			Buzzer(FALSE, 0);
			Sleep(300);
		}
		else if (!pDoc->Status.bDoorAoi[DOOR_FR_AOI_UP] && pDoc->Status.bDoorAoiF[DOOR_FR_AOI_UP])
		{
			ulOpenDoor &= ~(0x01 << 2);
			pDoc->Status.bDoorAoiF[DOOR_FR_AOI_UP] = FALSE;
			m_bSwStopNow = TRUE;
			m_bSwRunF = FALSE;
			Buzzer(TRUE, 0);
			TowerLamp(RGB_RED, TRUE);
			Stop();
			//pView->DispStsBar(_T("����-7"), 0);
			DispMain(_T("�� ��"), RGB_RED);
			MsgBox(_T("�Ͻ����� - �˻�� �� ���� ���� ���� Open"));
		}

		if (pDoc->Status.bDoorAoi[DOOR_BM_AOI_UP] && !pDoc->Status.bDoorAoiF[DOOR_BM_AOI_UP])
		{
			ulOpenDoor |= (0x01 << 3);
			pDoc->Status.bDoorAoiF[DOOR_BM_AOI_UP] = TRUE;
			Buzzer(FALSE, 0);
			Sleep(300);
		}
		else if (!pDoc->Status.bDoorAoi[DOOR_BM_AOI_UP] && pDoc->Status.bDoorAoiF[DOOR_BM_AOI_UP])
		{
			ulOpenDoor &= ~(0x01 << 3);
			pDoc->Status.bDoorAoiF[DOOR_BM_AOI_UP] = FALSE;
			m_bSwStopNow = TRUE;
			m_bSwRunF = FALSE;
			Buzzer(TRUE, 0);
			TowerLamp(RGB_RED, TRUE);
			Stop();
			//pView->DispStsBar(_T("����-8"), 0);
			DispMain(_T("�� ��"), RGB_RED);
			MsgBox(_T("�Ͻ����� - �˻�� �� �ĸ� �߾� ���� Open"));
		}

		if (pDoc->Status.bDoorAoi[DOOR_BL_AOI_UP] && !pDoc->Status.bDoorAoiF[DOOR_BL_AOI_UP])
		{
			ulOpenDoor |= (0x01 << 4);
			pDoc->Status.bDoorAoiF[DOOR_BL_AOI_UP] = TRUE;
			Buzzer(FALSE, 0);
			Sleep(300);
		}
		else if (!pDoc->Status.bDoorAoi[DOOR_BL_AOI_UP] && pDoc->Status.bDoorAoiF[DOOR_BL_AOI_UP])
		{
			ulOpenDoor &= ~(0x01 << 4);
			pDoc->Status.bDoorAoiF[DOOR_BL_AOI_UP] = FALSE;
			m_bSwStopNow = TRUE;
			m_bSwRunF = FALSE;
			Buzzer(TRUE, 0);
			TowerLamp(RGB_RED, TRUE);
			Stop();
			//pView->DispStsBar(_T("����-9"), 0);
			DispMain(_T("�� ��"), RGB_RED);
			MsgBox(_T("�Ͻ����� - �˻�� �� �ĸ� ���� ���� Open"));
		}

		if (pDoc->Status.bDoorAoi[DOOR_BR_AOI_UP] && !pDoc->Status.bDoorAoiF[DOOR_BR_AOI_UP])
		{
			ulOpenDoor |= (0x01 << 5);
			pDoc->Status.bDoorAoiF[DOOR_BR_AOI_UP] = TRUE;
			Buzzer(FALSE, 0);
			Sleep(300);
		}
		else if (!pDoc->Status.bDoorAoi[DOOR_BR_AOI_UP] && pDoc->Status.bDoorAoiF[DOOR_BR_AOI_UP])
		{
			ulOpenDoor &= ~(0x01 << 5);
			pDoc->Status.bDoorAoiF[DOOR_BR_AOI_UP] = FALSE;
			m_bSwStopNow = TRUE;
			m_bSwRunF = FALSE;
			Buzzer(TRUE, 0);
			TowerLamp(RGB_RED, TRUE);
			Stop();
			//pView->DispStsBar(_T("����-10"), 0);
			DispMain(_T("�� ��"), RGB_RED);
			MsgBox(_T("�Ͻ����� - �˻�� �� �ĸ� ���� ���� Open"));
		}
	}

	if (pDoc->WorkingInfo.LastJob.bAoiDnDrSen)
	{
		if (pDoc->Status.bDoorAoi[DOOR_FM_AOI_DN] && !pDoc->Status.bDoorAoiF[DOOR_FM_AOI_DN])
		{
			ulOpenDoor |= (0x01 << 0);
			pDoc->Status.bDoorAoiF[DOOR_FM_AOI_DN] = TRUE;
			Buzzer(FALSE, 0);
			Sleep(300);
		}
		else if (!pDoc->Status.bDoorAoi[DOOR_FM_AOI_DN] && pDoc->Status.bDoorAoiF[DOOR_FM_AOI_DN])
		{
			ulOpenDoor &= ~(0x01 << 0);
			pDoc->Status.bDoorAoiF[DOOR_FM_AOI_DN] = FALSE;
			m_bSwStopNow = TRUE;
			m_bSwRunF = FALSE;
			Buzzer(TRUE, 0);
			TowerLamp(RGB_RED, TRUE);
			Stop();
			//pView->DispStsBar(_T("����-5"), 0);
			DispMain(_T("�� ��"), RGB_RED);
			MsgBox(_T("�Ͻ����� - �˻�� �� ���� �߾� ���� Open"));
		}

		if (pDoc->Status.bDoorAoi[DOOR_FL_AOI_DN] && !pDoc->Status.bDoorAoiF[DOOR_FL_AOI_DN])
		{
			ulOpenDoor |= (0x01 << 1);
			pDoc->Status.bDoorAoiF[DOOR_FL_AOI_DN] = TRUE;
			Buzzer(FALSE, 0);
			Sleep(300);
		}
		else if (!pDoc->Status.bDoorAoi[DOOR_FL_AOI_DN] && pDoc->Status.bDoorAoiF[DOOR_FL_AOI_DN])
		{
			ulOpenDoor &= ~(0x01 << 1);
			pDoc->Status.bDoorAoiF[DOOR_FL_AOI_DN] = FALSE;
			m_bSwStopNow = TRUE;
			m_bSwRunF = FALSE;
			Buzzer(TRUE, 0);
			TowerLamp(RGB_RED, TRUE);
			Stop();
			//pView->DispStsBar(_T("����-6"), 0);
			DispMain(_T("�� ��"), RGB_RED);
			MsgBox(_T("�Ͻ����� - �˻�� �� ���� ���� ���� Open"));
		}

		if (pDoc->Status.bDoorAoi[DOOR_FR_AOI_DN] && !pDoc->Status.bDoorAoiF[DOOR_FR_AOI_DN])
		{
			ulOpenDoor |= (0x01 << 2);
			pDoc->Status.bDoorAoiF[DOOR_FR_AOI_DN] = TRUE;
			Buzzer(FALSE, 0);
			Sleep(300);
		}
		else if (!pDoc->Status.bDoorAoi[DOOR_FR_AOI_DN] && pDoc->Status.bDoorAoiF[DOOR_FR_AOI_DN])
		{
			ulOpenDoor &= ~(0x01 << 2);
			pDoc->Status.bDoorAoiF[DOOR_FR_AOI_DN] = FALSE;
			m_bSwStopNow = TRUE;
			m_bSwRunF = FALSE;
			Buzzer(TRUE, 0);
			TowerLamp(RGB_RED, TRUE);
			Stop();
			//pView->DispStsBar(_T("����-7"), 0);
			DispMain(_T("�� ��"), RGB_RED);
			MsgBox(_T("�Ͻ����� - �˻�� �� ���� ���� ���� Open"));
		}

		if (pDoc->Status.bDoorAoi[DOOR_BM_AOI_DN] && !pDoc->Status.bDoorAoiF[DOOR_BM_AOI_DN])
		{
			ulOpenDoor |= (0x01 << 3);
			pDoc->Status.bDoorAoiF[DOOR_BM_AOI_DN] = TRUE;
			Buzzer(FALSE, 0);
			Sleep(300);
		}
		else if (!pDoc->Status.bDoorAoi[DOOR_BM_AOI_DN] && pDoc->Status.bDoorAoiF[DOOR_BM_AOI_DN])
		{
			ulOpenDoor &= ~(0x01 << 3);
			pDoc->Status.bDoorAoiF[DOOR_BM_AOI_DN] = FALSE;
			m_bSwStopNow = TRUE;
			m_bSwRunF = FALSE;
			Buzzer(TRUE, 0);
			TowerLamp(RGB_RED, TRUE);
			Stop();
			//pView->DispStsBar(_T("����-8"), 0);
			DispMain(_T("�� ��"), RGB_RED);
			MsgBox(_T("�Ͻ����� - �˻�� �� �ĸ� �߾� ���� Open"));
		}

		if (pDoc->Status.bDoorAoi[DOOR_BL_AOI_DN] && !pDoc->Status.bDoorAoiF[DOOR_BL_AOI_DN])
		{
			ulOpenDoor |= (0x01 << 4);
			pDoc->Status.bDoorAoiF[DOOR_BL_AOI_DN] = TRUE;
			Buzzer(FALSE, 0);
			Sleep(300);
		}
		else if (!pDoc->Status.bDoorAoi[DOOR_BL_AOI_DN] && pDoc->Status.bDoorAoiF[DOOR_BL_AOI_DN])
		{
			ulOpenDoor &= ~(0x01 << 4);
			pDoc->Status.bDoorAoiF[DOOR_BL_AOI_DN] = FALSE;
			m_bSwStopNow = TRUE;
			m_bSwRunF = FALSE;
			Buzzer(TRUE, 0);
			TowerLamp(RGB_RED, TRUE);
			Stop();
			//pView->DispStsBar(_T("����-9"), 0);
			DispMain(_T("�� ��"), RGB_RED);
			MsgBox(_T("�Ͻ����� - �˻�� �� �ĸ� ���� ���� Open"));
		}

		if (pDoc->Status.bDoorAoi[DOOR_BR_AOI_DN] && !pDoc->Status.bDoorAoiF[DOOR_BR_AOI_DN])
		{
			ulOpenDoor |= (0x01 << 5);
			pDoc->Status.bDoorAoiF[DOOR_BR_AOI_DN] = TRUE;
			Buzzer(FALSE, 0);
			Sleep(300);
		}
		else if (!pDoc->Status.bDoorAoi[DOOR_BR_AOI_DN] && pDoc->Status.bDoorAoiF[DOOR_BR_AOI_DN])
		{
			ulOpenDoor &= ~(0x01 << 5);
			pDoc->Status.bDoorAoiF[DOOR_BR_AOI_DN] = FALSE;
			m_bSwStopNow = TRUE;
			m_bSwRunF = FALSE;
			Buzzer(TRUE, 0);
			TowerLamp(RGB_RED, TRUE);
			Stop();
			//pView->DispStsBar(_T("����-10"), 0);
			DispMain(_T("�� ��"), RGB_RED);
			MsgBox(_T("�Ͻ����� - �˻�� �� �ĸ� ���� ���� Open"));
		}
	}

	if (pDoc->WorkingInfo.LastJob.bMkDrSen)
	{
		if (pDoc->Status.bDoorMk[DOOR_FL_MK] && !pDoc->Status.bDoorMkF[DOOR_FL_MK])
		{
			ulOpenDoor |= (0x01 << 6);
			pDoc->Status.bDoorMkF[DOOR_FL_MK] = TRUE;
			Buzzer(FALSE, 0);
			Sleep(300);
		}
		else if (!pDoc->Status.bDoorMk[DOOR_FL_MK] && pDoc->Status.bDoorMkF[DOOR_FL_MK])
		{
			ulOpenDoor &= ~(0x01 << 6);
			pDoc->Status.bDoorMkF[DOOR_FL_MK] = FALSE;
			m_bSwStopNow = TRUE;
			m_bSwRunF = FALSE;
			Buzzer(TRUE, 0);
			TowerLamp(RGB_RED, TRUE);
			Stop();
			//pView->DispStsBar(_T("����-11"), 0);
			DispMain(_T("�� ��"), RGB_RED);
			MsgBox(_T("�Ͻ����� - ��ŷ�� ���� ���� ���� Open"));
		}

		if (pDoc->Status.bDoorMk[DOOR_FR_MK] && !pDoc->Status.bDoorMkF[DOOR_FR_MK])
		{
			ulOpenDoor |= (0x01 << 7);
			pDoc->Status.bDoorMkF[DOOR_FR_MK] = TRUE;
			Buzzer(FALSE, 0);
			Sleep(300);
		}
		else if (!pDoc->Status.bDoorMk[DOOR_FR_MK] && pDoc->Status.bDoorMkF[DOOR_FR_MK])
		{
			ulOpenDoor &= ~(0x01 << 7);
			pDoc->Status.bDoorMkF[DOOR_FR_MK] = FALSE;
			m_bSwStopNow = TRUE;
			m_bSwRunF = FALSE;
			Buzzer(TRUE, 0);
			TowerLamp(RGB_RED, TRUE);
			Stop();
			//pView->DispStsBar(_T("����-12"), 0);
			DispMain(_T("�� ��"), RGB_RED);
			MsgBox(_T("�Ͻ����� - ��ŷ�� ���� ���� ���� Open"));
		}

		if (pDoc->Status.bDoorMk[DOOR_BL_MK] && !pDoc->Status.bDoorMkF[DOOR_BL_MK])
		{
			ulOpenDoor |= (0x01 << 8);
			pDoc->Status.bDoorMkF[DOOR_BL_MK] = TRUE;
			Buzzer(FALSE, 0);
			Sleep(300);
		}
		else if (!pDoc->Status.bDoorMk[DOOR_BL_MK] && pDoc->Status.bDoorMkF[DOOR_BL_MK])
		{
			ulOpenDoor &= ~(0x01 << 8);
			pDoc->Status.bDoorMkF[DOOR_BL_MK] = FALSE;
			m_bSwStopNow = TRUE;
			m_bSwRunF = FALSE;
			Buzzer(TRUE, 0);
			TowerLamp(RGB_RED, TRUE);
			Stop();
			//pView->DispStsBar(_T("����-13"), 0);
			DispMain(_T("�� ��"), RGB_RED);
			MsgBox(_T("�Ͻ����� - ��ŷ�� �ĸ� ���� ���� Open"));
		}

		if (pDoc->Status.bDoorMk[DOOR_BR_MK] && !pDoc->Status.bDoorMkF[DOOR_BR_MK])
		{
			ulOpenDoor |= (0x01 << 9);
			pDoc->Status.bDoorMkF[DOOR_BR_MK] = TRUE;
			Buzzer(FALSE, 0);
			Sleep(300);
		}
		else if (!pDoc->Status.bDoorMk[DOOR_BR_MK] && pDoc->Status.bDoorMkF[DOOR_BR_MK])
		{
			ulOpenDoor &= ~(0x01 << 9);
			pDoc->Status.bDoorMkF[DOOR_BR_MK] = FALSE;
			m_bSwStopNow = TRUE;
			m_bSwRunF = FALSE;
			Buzzer(TRUE, 0);
			TowerLamp(RGB_RED, TRUE);
			Stop();
			//pView->DispStsBar(_T("����-14"), 0);
			DispMain(_T("�� ��"), RGB_RED);
			MsgBox(_T("�Ͻ����� - ��ŷ�� �ĸ� ���� ���� Open"));
		}
	}

	if (pDoc->WorkingInfo.LastJob.bEngvDrSen)
	{
		if (pDoc->Status.bDoorEngv[DOOR_FL_ENGV] && !pDoc->Status.bDoorEngvF[DOOR_FL_ENGV])
		{
			ulOpenDoor |= (0x01 << 6);
			pDoc->Status.bDoorEngvF[DOOR_FL_ENGV] = TRUE;
			Buzzer(FALSE, 0);
			Sleep(300);
		}
		else if (!pDoc->Status.bDoorEngv[DOOR_FL_ENGV] && pDoc->Status.bDoorEngvF[DOOR_FL_ENGV])
		{
			ulOpenDoor &= ~(0x01 << 6);
			pDoc->Status.bDoorEngvF[DOOR_FL_ENGV] = FALSE;
			m_bSwStopNow = TRUE;
			m_bSwRunF = FALSE;
			Buzzer(TRUE, 0);
			TowerLamp(RGB_RED, TRUE);
			Stop();
			//pView->DispStsBar(_T("����-11"), 0);
			DispMain(_T("�� ��"), RGB_RED);
			MsgBox(_T("�Ͻ����� - ���κ� ���� ���� ���� Open"));
		}

		if (pDoc->Status.bDoorEngv[DOOR_FR_ENGV] && !pDoc->Status.bDoorEngvF[DOOR_FR_ENGV])
		{
			ulOpenDoor |= (0x01 << 7);
			pDoc->Status.bDoorEngvF[DOOR_FR_ENGV] = TRUE;
			Buzzer(FALSE, 0);
			Sleep(300);
		}
		else if (!pDoc->Status.bDoorEngv[DOOR_FR_ENGV] && pDoc->Status.bDoorEngvF[DOOR_FR_ENGV])
		{
			ulOpenDoor &= ~(0x01 << 7);
			pDoc->Status.bDoorEngvF[DOOR_FR_ENGV] = FALSE;
			m_bSwStopNow = TRUE;
			m_bSwRunF = FALSE;
			Buzzer(TRUE, 0);
			TowerLamp(RGB_RED, TRUE);
			Stop();
			//pView->DispStsBar(_T("����-12"), 0);
			DispMain(_T("�� ��"), RGB_RED);
			MsgBox(_T("�Ͻ����� - ���κ� ���� ���� ���� Open"));
		}

		if (pDoc->Status.bDoorEngv[DOOR_BL_ENGV] && !pDoc->Status.bDoorEngvF[DOOR_BL_ENGV])
		{
			ulOpenDoor |= (0x01 << 8);
			pDoc->Status.bDoorEngvF[DOOR_BL_ENGV] = TRUE;
			Buzzer(FALSE, 0);
			Sleep(300);
		}
		else if (!pDoc->Status.bDoorEngv[DOOR_BL_ENGV] && pDoc->Status.bDoorEngvF[DOOR_BL_ENGV])
		{
			ulOpenDoor &= ~(0x01 << 8);
			pDoc->Status.bDoorEngvF[DOOR_BL_ENGV] = FALSE;
			m_bSwStopNow = TRUE;
			m_bSwRunF = FALSE;
			Buzzer(TRUE, 0);
			TowerLamp(RGB_RED, TRUE);
			Stop();
			//pView->DispStsBar(_T("����-13"), 0);
			DispMain(_T("�� ��"), RGB_RED);
			MsgBox(_T("�Ͻ����� - ���κ� �ĸ� ���� ���� Open"));
		}

		if (pDoc->Status.bDoorEngv[DOOR_BR_ENGV] && !pDoc->Status.bDoorEngvF[DOOR_BR_ENGV])
		{
			ulOpenDoor |= (0x01 << 9);
			pDoc->Status.bDoorEngvF[DOOR_BR_ENGV] = TRUE;
			Buzzer(FALSE, 0);
			Sleep(300);
		}
		else if (!pDoc->Status.bDoorEngv[DOOR_BR_ENGV] && pDoc->Status.bDoorEngvF[DOOR_BR_ENGV])
		{
			ulOpenDoor &= ~(0x01 << 9);
			pDoc->Status.bDoorEngvF[DOOR_BR_ENGV] = FALSE;
			m_bSwStopNow = TRUE;
			m_bSwRunF = FALSE;
			Buzzer(TRUE, 0);
			TowerLamp(RGB_RED, TRUE);
			Stop();
			//pView->DispStsBar(_T("����-14"), 0);
			DispMain(_T("�� ��"), RGB_RED);
			MsgBox(_T("�Ͻ����� - ���κ� �ĸ� ���� ���� Open"));
		}
	}

	if (pDoc->WorkingInfo.LastJob.bUclDrSen)
	{
		if (pDoc->Status.bDoorUc[DOOR_FL_UC] && !pDoc->Status.bDoorUcF[DOOR_FL_UC])
		{
			ulOpenDoor |= (0x01 << 10);
			pDoc->Status.bDoorUcF[DOOR_FL_UC] = TRUE;
			Buzzer(FALSE, 0);
			Sleep(300);
		}
		else if (!pDoc->Status.bDoorUc[DOOR_FL_UC] && pDoc->Status.bDoorUcF[DOOR_FL_UC])
		{
			ulOpenDoor &= ~(0x01 << 10);
			pDoc->Status.bDoorUcF[DOOR_FL_UC] = FALSE;
			m_bSwStopNow = TRUE;
			m_bSwRunF = FALSE;
			Buzzer(TRUE, 0);
			TowerLamp(RGB_RED, TRUE);
			Stop();
			//pView->DispStsBar(_T("����-15"), 0);
			DispMain(_T("�� ��"), RGB_RED);
			MsgBox(_T("�Ͻ����� - �����Ϸ��� ���� ���� ���� Open"));
		}

		if (pDoc->Status.bDoorUc[DOOR_FR_UC] && !pDoc->Status.bDoorUcF[DOOR_FR_UC])
		{
			ulOpenDoor |= (0x01 << 11);
			pDoc->Status.bDoorUcF[DOOR_FR_UC] = TRUE;
			Buzzer(FALSE, 0);
			Sleep(300);
		}
		else if (!pDoc->Status.bDoorUc[DOOR_FR_UC] && pDoc->Status.bDoorUcF[DOOR_FR_UC])
		{
			ulOpenDoor &= ~(0x01 << 11);
			pDoc->Status.bDoorUcF[DOOR_FR_UC] = FALSE;
			m_bSwStopNow = TRUE;
			m_bSwRunF = FALSE;
			Buzzer(TRUE, 0);
			TowerLamp(RGB_RED, TRUE);
			Stop();
			//pView->DispStsBar(_T("����-16"), 0);
			DispMain(_T("�� ��"), RGB_RED);
			MsgBox(_T("�Ͻ����� - �����Ϸ��� ���� ���� Open"));
		}

		if (pDoc->Status.bDoorUc[DOOR_BL_UC] && !pDoc->Status.bDoorUcF[DOOR_BL_UC])
		{
			ulOpenDoor |= (0x01 << 12);
			pDoc->Status.bDoorUcF[DOOR_BL_UC] = TRUE;
			Buzzer(FALSE, 0);
			Sleep(300);
		}
		else if (!pDoc->Status.bDoorUc[DOOR_BL_UC] && pDoc->Status.bDoorUcF[DOOR_BL_UC])
		{
			ulOpenDoor &= ~(0x01 << 12);
			pDoc->Status.bDoorUcF[DOOR_BL_UC] = FALSE;
			m_bSwStopNow = TRUE;
			m_bSwRunF = FALSE;
			Buzzer(TRUE, 0);
			TowerLamp(RGB_RED, TRUE);
			Stop();
			//pView->DispStsBar(_T("����-17"), 0);
			DispMain(_T("�� ��"), RGB_RED);
			MsgBox(_T("�Ͻ����� - �����Ϸ��� �ĸ� ���� ���� Open"));
		}

		if (pDoc->Status.bDoorUc[DOOR_BR_UC] && !pDoc->Status.bDoorUcF[DOOR_BR_UC])
		{
			ulOpenDoor |= (0x01 << 13);
			pDoc->Status.bDoorUcF[DOOR_BR_UC] = TRUE;
			Buzzer(FALSE, 0);
			Sleep(300);
		}
		else if (!pDoc->Status.bDoorUc[DOOR_BR_UC] && pDoc->Status.bDoorUcF[DOOR_BR_UC])
		{
			ulOpenDoor &= ~(0x01 << 13);
			pDoc->Status.bDoorUcF[DOOR_BR_UC] = FALSE;
			m_bSwStopNow = TRUE;
			m_bSwRunF = FALSE;
			Buzzer(TRUE, 0);
			TowerLamp(RGB_RED, TRUE);
			Stop();
			//pView->DispStsBar(_T("����-18"), 0);
			DispMain(_T("�� ��"), RGB_RED);
			MsgBox(_T("�Ͻ����� - �����Ϸ��� �ĸ� ���� ���� Open"));
		}
	}

	if (pDoc->WorkingInfo.LastJob.bRclDrSen)
	{
		// 		if(pDoc->Status.bDoorRe[DOOR_FL_RC] && !pDoc->Status.bDoorReF[DOOR_FL_RC])
		// 		{
		// 			ulOpenDoor |= (0x01<<14);
		// 			pDoc->Status.bDoorReF[DOOR_FL_RC] = TRUE;
		// 			Stop();
		// 			MsgBox(_T("�Ͻ����� - �����Ϸ��� ���� ���� ���� Open"));
		// 			TowerLamp(RGB_RED, TRUE);
		// 			Buzzer(TRUE, 0);
		// 		}
		// 		else if(!pDoc->Status.bDoorRe[DOOR_FL_RC] && pDoc->Status.bDoorReF[DOOR_FL_RC])
		// 		{
		// 			ulOpenDoor &= ~(0x01<<14);
		// 			pDoc->Status.bDoorReF[DOOR_FL_RC] = FALSE;
		// 			Buzzer(FALSE, 0);
		// 			Sleep(300);
		// 		}

		if (pDoc->Status.bDoorRe[DOOR_FR_RC] && !pDoc->Status.bDoorReF[DOOR_FR_RC])
		{
			ulOpenDoor |= (0x01 << 15);
			pDoc->Status.bDoorReF[DOOR_FR_RC] = TRUE;
			Buzzer(FALSE, 0);
			Sleep(300);
		}
		else if (!pDoc->Status.bDoorRe[DOOR_FR_RC] && pDoc->Status.bDoorReF[DOOR_FR_RC])
		{
			ulOpenDoor &= ~(0x01 << 15);
			pDoc->Status.bDoorReF[DOOR_FR_RC] = FALSE;
			m_bSwStopNow = TRUE;
			m_bSwRunF = FALSE;
			Buzzer(TRUE, 0);
			TowerLamp(RGB_RED, TRUE);
			Stop();
			pView->DispStsBar(_T("����-19"), 0);
			DispMain(_T("�� ��"), RGB_RED);
			MsgBox(_T("�Ͻ����� - �����Ϸ��� ���� ���� ���� Open"));
		}

		if (pDoc->Status.bDoorRe[DOOR_S_RC] && !pDoc->Status.bDoorReF[DOOR_S_RC])
		{
			ulOpenDoor |= (0x01 << 16);
			pDoc->Status.bDoorReF[DOOR_S_RC] = TRUE;
			Buzzer(FALSE, 0);
			Sleep(300);
		}
		else if (!pDoc->Status.bDoorRe[DOOR_S_RC] && pDoc->Status.bDoorReF[DOOR_S_RC])
		{
			ulOpenDoor &= ~(0x01 << 16);
			pDoc->Status.bDoorReF[DOOR_S_RC] = FALSE;
			m_bSwStopNow = TRUE;
			m_bSwRunF = FALSE;
			Buzzer(TRUE, 0);
			TowerLamp(RGB_RED, TRUE);
			Stop();
			//pView->DispStsBar(_T("����-20"), 0);
			DispMain(_T("�� ��"), RGB_RED);
			MsgBox(_T("�Ͻ����� - �����Ϸ��� ���� ���� Open"));
		}

		if (pDoc->Status.bDoorRe[DOOR_BL_RC] && !pDoc->Status.bDoorReF[DOOR_BL_RC])
		{
			ulOpenDoor |= (0x01 << 17);
			pDoc->Status.bDoorReF[DOOR_BL_RC] = TRUE;
			Buzzer(FALSE, 0);
			Sleep(300);
		}
		else if (!pDoc->Status.bDoorRe[DOOR_BL_RC] && pDoc->Status.bDoorReF[DOOR_BL_RC])
		{
			ulOpenDoor &= ~(0x01 << 17);
			pDoc->Status.bDoorReF[DOOR_BL_RC] = FALSE;
			m_bSwStopNow = TRUE;
			m_bSwRunF = FALSE;
			Buzzer(TRUE, 0);
			TowerLamp(RGB_RED, TRUE);
			Stop();
			//pView->DispStsBar(_T("����-21"), 0);
			DispMain(_T("�� ��"), RGB_RED);
			MsgBox(_T("�Ͻ����� - �����Ϸ��� �ĸ� ���� ���� Open"));
		}

		if (pDoc->Status.bDoorRe[DOOR_BR_RC] && !pDoc->Status.bDoorReF[DOOR_BR_RC])
		{
			ulOpenDoor |= (0x01 << 18);
			pDoc->Status.bDoorReF[DOOR_BR_RC] = TRUE;
			Buzzer(FALSE, 0);
			Sleep(300);
		}
		else if (!pDoc->Status.bDoorRe[DOOR_BR_RC] && pDoc->Status.bDoorReF[DOOR_BR_RC])
		{
			ulOpenDoor &= ~(0x01 << 18);
			pDoc->Status.bDoorReF[DOOR_BR_RC] = FALSE;
			m_bSwStopNow = TRUE;
			m_bSwRunF = FALSE;
			Buzzer(TRUE, 0);
			TowerLamp(RGB_RED, TRUE);
			Stop();
			//pView->DispStsBar(_T("����-22"), 0);
			DispMain(_T("�� ��"), RGB_RED);
			MsgBox(_T("�Ͻ����� - �����Ϸ��� �ĸ� ���� ���� Open"));
		}
	}

	return ulOpenDoor;
}

void CGvisR2R_PunchView::ChkEmg()
{
	if (pDoc->Status.bEmgAoi[EMG_F_AOI_UP] && !pDoc->Status.bEmgAoiF[EMG_F_AOI_UP])
	{
		//SwAoiEmg(TRUE);
		pDoc->Status.bEmgAoiF[EMG_F_AOI_UP] = TRUE;
		m_bSwStopNow = TRUE;
		m_bSwRunF = FALSE;
		Buzzer(TRUE, 0);
		TowerLamp(RGB_RED, TRUE);
		Stop();
		//pView->DispStsBar(_T("����-23"), 0);
		DispMain(_T("�� ��"), RGB_RED);
		MsgBox(_T("������� - �˻�� �� ���� ����ġ"));
	}
	else if (!pDoc->Status.bEmgAoi[EMG_F_AOI_UP] && pDoc->Status.bEmgAoiF[EMG_F_AOI_UP])
	{
		//SwAoiEmg(FALSE);
		pDoc->Status.bEmgAoiF[EMG_F_AOI_UP] = FALSE;
		Buzzer(FALSE, 0);
		Sleep(300);
		ResetMotion();
	}

	if (pDoc->Status.bEmgAoi[EMG_B_AOI_UP] && !pDoc->Status.bEmgAoiF[EMG_B_AOI_UP])
	{
		//SwAoiEmg(TRUE);
		pDoc->Status.bEmgAoiF[EMG_B_AOI_UP] = TRUE;
		m_bSwStopNow = TRUE;
		m_bSwRunF = FALSE;
		Buzzer(TRUE, 0);
		TowerLamp(RGB_RED, TRUE);
		Stop();
		//pView->DispStsBar(_T("����-24"), 0);
		DispMain(_T("�� ��"), RGB_RED);
		MsgBox(_T("������� - �˻�� �� �ĸ� ����ġ"));
	}
	else if (!pDoc->Status.bEmgAoi[EMG_B_AOI_UP] && pDoc->Status.bEmgAoiF[EMG_B_AOI_UP])
	{
		//SwAoiEmg(FALSE);
		pDoc->Status.bEmgAoiF[EMG_B_AOI_UP] = FALSE;
		Buzzer(FALSE, 0);
		Sleep(300);
		ResetMotion();
	}

	if (pDoc->Status.bEmgAoi[EMG_F_AOI_DN] && !pDoc->Status.bEmgAoiF[EMG_F_AOI_DN])
	{
		//SwAoiEmg(TRUE);
		pDoc->Status.bEmgAoiF[EMG_F_AOI_DN] = TRUE;
		m_bSwStopNow = TRUE;
		m_bSwRunF = FALSE;
		Buzzer(TRUE, 0);
		TowerLamp(RGB_RED, TRUE);
		Stop();
		//pView->DispStsBar(_T("����-23"), 0);
		DispMain(_T("�� ��"), RGB_RED);
		MsgBox(_T("������� - �˻�� �� ���� ����ġ"));
	}
	else if (!pDoc->Status.bEmgAoi[EMG_F_AOI_DN] && pDoc->Status.bEmgAoiF[EMG_F_AOI_DN])
	{
		//SwAoiEmg(FALSE);
		pDoc->Status.bEmgAoiF[EMG_F_AOI_DN] = FALSE;
		Buzzer(FALSE, 0);
		Sleep(300);
		ResetMotion();
	}

	if (pDoc->Status.bEmgAoi[EMG_B_AOI_DN] && !pDoc->Status.bEmgAoiF[EMG_B_AOI_DN])
	{
		//SwAoiEmg(TRUE);
		pDoc->Status.bEmgAoiF[EMG_B_AOI_DN] = TRUE;
		m_bSwStopNow = TRUE;
		m_bSwRunF = FALSE;
		Buzzer(TRUE, 0);
		TowerLamp(RGB_RED, TRUE);
		Stop();
		//pView->DispStsBar(_T("����-24"), 0);
		DispMain(_T("�� ��"), RGB_RED);
		MsgBox(_T("������� - �˻�� �� �ĸ� ����ġ"));
	}
	else if (!pDoc->Status.bEmgAoi[EMG_B_AOI_DN] && pDoc->Status.bEmgAoiF[EMG_B_AOI_DN])
	{
		//SwAoiEmg(FALSE);
		pDoc->Status.bEmgAoiF[EMG_B_AOI_DN] = FALSE;
		Buzzer(FALSE, 0);
		Sleep(300);
		ResetMotion();
	}

	if (pDoc->Status.bEmgMk[EMG_M_MK] && !pDoc->Status.bEmgMkF[EMG_M_MK])
	{
		//SwAoiEmg(TRUE);
		pDoc->Status.bEmgMkF[EMG_M_MK] = TRUE;
		m_bSwStopNow = TRUE;
		m_bSwRunF = FALSE;
		Buzzer(TRUE, 0);
		TowerLamp(RGB_RED, TRUE);
		Stop();
		//pView->DispStsBar(_T("����-25"), 0);
		DispMain(_T("�� ��"), RGB_RED);
		MsgBox(_T("������� - ��ŷ�� ���� ����ġ"));
	}
	else if (!pDoc->Status.bEmgMk[EMG_M_MK] && pDoc->Status.bEmgMkF[EMG_M_MK])
	{
		//SwAoiEmg(FALSE);
		pDoc->Status.bEmgMkF[EMG_M_MK] = FALSE;
		Buzzer(FALSE, 0);
		Sleep(300);
		ResetMotion();
	}

	if (pDoc->Status.bEmgMk[EMG_B_MK] && !pDoc->Status.bEmgMkF[EMG_B_MK])
	{
		//SwAoiEmg(TRUE);
		pDoc->Status.bEmgMkF[EMG_B_MK] = TRUE;
		m_bSwStopNow = TRUE;
		m_bSwRunF = FALSE;
		Buzzer(TRUE, 0);
		TowerLamp(RGB_RED, TRUE);
		Stop();
		//pView->DispStsBar(_T("����-26"), 0);
		DispMain(_T("�� ��"), RGB_RED);
		MsgBox(_T("������� - ��ŷ�� ����ġ"));
	}
	else if (!pDoc->Status.bEmgMk[EMG_B_MK] && pDoc->Status.bEmgMkF[EMG_B_MK])
	{
		//SwAoiEmg(FALSE);
		pDoc->Status.bEmgMkF[EMG_B_MK] = FALSE;
		Buzzer(FALSE, 0);
		Sleep(300);
		ResetMotion();
	}

	if (pDoc->Status.bEmgUc && !pDoc->Status.bEmgUcF)
	{
		//SwAoiEmg(TRUE);
		pDoc->Status.bEmgUcF = TRUE;
		m_bSwStopNow = TRUE;
		m_bSwRunF = FALSE;
		Buzzer(TRUE, 0);
		TowerLamp(RGB_RED, TRUE);
		Stop();
		//pView->DispStsBar(_T("����-27"), 0);
		DispMain(_T("�� ��"), RGB_RED);
		MsgBox(_T("������� - �����Ϸ��� ����ġ"));
	}
	else if (!pDoc->Status.bEmgUc && pDoc->Status.bEmgUcF)
	{
		//SwAoiEmg(FALSE);
		pDoc->Status.bEmgUcF = FALSE;
		Buzzer(FALSE, 0);
		Sleep(300);
		ResetMotion();
	}

	if (pDoc->Status.bEmgRc && !pDoc->Status.bEmgRcF)
	{
		//SwAoiEmg(TRUE);
		pDoc->Status.bEmgRcF = TRUE;
		m_bSwStopNow = TRUE;
		m_bSwRunF = FALSE;
		Buzzer(TRUE, 0);
		TowerLamp(RGB_RED, TRUE);
		Stop();
		//pView->DispStsBar(_T("����-28"), 0);
		DispMain(_T("�� ��"), RGB_RED);
		MsgBox(_T("������� - �����Ϸ��� ����ġ"));
	}
	else if (!pDoc->Status.bEmgRc && pDoc->Status.bEmgRcF)
	{
		//SwAoiEmg(FALSE);
		pDoc->Status.bEmgRcF = FALSE;
		Buzzer(FALSE, 0);
		Sleep(300);
		ResetMotion();
	}

	if (pDoc->Status.bEmgEngv[0] && !pDoc->Status.bEmgEngvF[0])
	{
		pDoc->Status.bEmgEngvF[0] = TRUE;
		m_bSwStopNow = TRUE;
		m_bSwRunF = FALSE;
		Buzzer(TRUE, 0);
		TowerLamp(RGB_RED, TRUE);
		Stop();
		//pView->DispStsBar(_T("����-29"), 0);
		DispMain(_T("�� ��"), RGB_RED);
		MsgBox(_T("������� - ���κ� �����"));
	}
	else if (!pDoc->Status.bEmgEngv[0] && pDoc->Status.bEmgEngvF[0])
	{
		pDoc->Status.bEmgEngvF[0] = FALSE;
		Buzzer(FALSE, 0);
		Sleep(300);
		ResetMotion();
	}

	if (pDoc->Status.bEmgEngv[1] && !pDoc->Status.bEmgEngvF[1])
	{
		pDoc->Status.bEmgEngvF[1] = TRUE;
		m_bSwStopNow = TRUE;
		m_bSwRunF = FALSE;
		Buzzer(TRUE, 0);
		TowerLamp(RGB_RED, TRUE);
		Stop();
		//pView->DispStsBar(_T("����-30"), 0);
		DispMain(_T("�� ��"), RGB_RED);
		MsgBox(_T("������� - ���κ� ����ġ"));
	}
	else if (!pDoc->Status.bEmgEngv[1] && pDoc->Status.bEmgEngvF[1])
	{
		pDoc->Status.bEmgEngvF[1] = FALSE;
		Buzzer(FALSE, 0);
		Sleep(300);
		ResetMotion();
	}
}

int CGvisR2R_PunchView::ChkSerial() // // 0: Continue, -: Previous, +: Discontinue --> 0: Same Serial, -: Decrese Serial, +: Increase Serial
{
	int nSerial0 = GetBuffer0(); // ù��° ���� �ø��� : ���� �ø����� �ٸ��� 0
	int nSerial1 = GetBuffer1(); // �ι�° ���� �ø��� : ���� �ø����� �ٸ��� 0
	int nLastShot = pDoc->GetLastShotMk();

	// Last shot ���� ���� �ø��� üũ
	//if (nSerial0 == nLastShot + 1 || nSerial1 == nLastShot + 2)
	//	return 0;

	//if (pDoc->WorkingInfo.LastJob.bLotSep || pDoc->m_bDoneChgLot)
	//{
	//	if (nLastShot == pDoc->m_nLotLastShot && (nSerial0 == 1 || nSerial1 == 2))
	//		return 0;
	//}
	//if (m_bLastProc && nSerial0 == m_nLotEndSerial)
	//	return 0;

	return (nSerial0 - nLastShot);
}

void CGvisR2R_PunchView::ChkBuf()
{
	if (!m_bShift2Mk)
		ChkBufUp();
	if (!m_bShift2Mk)
		ChkBufDn();
}

void CGvisR2R_PunchView::ChkBufUp()
{
	CString str, sTemp;

	str = _T("UB: ");
	if (ChkBufUp(m_pBufSerial[0], m_nBufTot[0]))
	{
		for (int i = 0; i<m_nBufTot[0]; i++)
		{
			if (m_bShift2Mk)
				return;

			DelOverLotEndSerialUp(m_pBufSerial[0][i]);

			if (i == m_nBufTot[0] - 1)
				sTemp.Format(_T("%d"), m_pBufSerial[0][i]);
			else
				sTemp.Format(_T("%d,"), m_pBufSerial[0][i]);
			str += sTemp;
		}
	}
	else
	{
		m_nBufTot[0] = 0;
	}

	if (pFrm)
	{
		if (m_sBuf[0] != str)
		{
			m_sBuf[0] = str;
			pFrm->DispStatusBar(str, 3);

			pDoc->SetCurrentInfoBufUpTot(m_nBufTot[0]);
			for (int k = 0; k<m_nBufTot[0]; k++)
				pDoc->SetCurrentInfoBufUp(k, m_pBufSerial[0][k]);

			if (m_nBufTot[0] == 1)
			{
				pDoc->m_nAoiCamInfoStrPcs[0] = GetAoiUpCamMstInfo();
			}
		}
	}
}

void CGvisR2R_PunchView::ChkBufDn()
{
	CString str, sTemp;

	str = _T("DB: ");
	if (ChkBufDn(m_pBufSerial[1], m_nBufTot[1]))
	{
		for (int i = 0; i<m_nBufTot[1]; i++)
		{
			if (m_bShift2Mk)
				return;

			DelOverLotEndSerialDn(m_pBufSerial[1][i]);

			if (i == m_nBufTot[1] - 1)
				sTemp.Format(_T("%d"), m_pBufSerial[1][i]);
			else
				sTemp.Format(_T("%d,"), m_pBufSerial[1][i]);
			str += sTemp;
		}
	}
	else
	{
		m_nBufTot[1] = 0;
	}

	if (pFrm)
	{
		if (m_sBuf[1] != str)
		{
			m_sBuf[1] = str;
			pFrm->DispStatusBar(str, 1);

			pDoc->SetCurrentInfoBufDnTot(m_nBufTot[1]);
			for (int k = 0; k < m_nBufTot[1]; k++)
				pDoc->SetCurrentInfoBufDn(k, m_pBufSerial[1][k]);

			if (m_nBufTot[1] == 1)
			{
				pDoc->m_nAoiCamInfoStrPcs[1] = GetAoiDnCamMstInfo();
			}
		}
	}
}

int CGvisR2R_PunchView::GetAoiUpAutoSerial()
{
	int nSerial = 0;
	CString sPath = pDoc->WorkingInfo.System.sPathAoiUpStatusInfo;
	TCHAR szData[512];

	if (0 < ::GetPrivateProfileString(_T("Auto"), _T("nSerial"), NULL, szData, sizeof(szData), sPath))
		nSerial = _ttoi(szData);
	else
		nSerial = -1;

	return nSerial;
}

void CGvisR2R_PunchView::SetAoiUpAutoSerial(int nSerial)
{
	CString sPath = pDoc->WorkingInfo.System.sPathAoiUpStatusInfo;
	CString str;
	str.Format(_T("%d"), nSerial);
	::WritePrivateProfileString(_T("Auto"), _T("nSerial"), str, sPath);
}

int CGvisR2R_PunchView::GetAoiUpAutoStep()
{
	TCHAR szData[512];
	CString sPath = pDoc->WorkingInfo.System.sPathAoiUpStatusInfo;
	if (0 < ::GetPrivateProfileString(_T("Auto"), _T("nStep"), NULL, szData, sizeof(szData), sPath))
		return _ttoi(szData);

	return (-1);
}

void CGvisR2R_PunchView::SetAoiUpAutoStep(int nStep)
{
	CString sPath = pDoc->WorkingInfo.System.sPathAoiUpStatusInfo;
	CString str;
	str.Format(_T("%d"), nStep);
	::WritePrivateProfileString(_T("Auto"), _T("nStep"), str, sPath);
}

int CGvisR2R_PunchView::GetAoiDnAutoSerial()
{
	int nSerial = 0;
	CString sPath = pDoc->WorkingInfo.System.sPathAoiDnStatusInfo;
	TCHAR szData[512];

	if (0 < ::GetPrivateProfileString(_T("Auto"), _T("nSerial"), NULL, szData, sizeof(szData), sPath))
		nSerial = _ttoi(szData);
	else
		nSerial = -1;

	return nSerial;
}

void CGvisR2R_PunchView::SetAoiDnAutoSerial(int nSerial)
{
	CString sPath = pDoc->WorkingInfo.System.sPathAoiDnStatusInfo;
	CString str;
	str.Format(_T("%d"), nSerial);
	::WritePrivateProfileString(_T("Auto"), _T("nSerial"), str, sPath);
}

int CGvisR2R_PunchView::GetAoiDnAutoStep()
{
	TCHAR szData[512];
	CString sPath = pDoc->WorkingInfo.System.sPathAoiDnStatusInfo;
	if (0 < ::GetPrivateProfileString(_T("Auto"), _T("nStep"), NULL, szData, sizeof(szData), sPath))
		return _ttoi(szData);

	return (-1);
}

void CGvisR2R_PunchView::SetAoiDnAutoStep(int nStep)
{
	CString sPath = pDoc->WorkingInfo.System.sPathAoiDnStatusInfo;
	CString str;
	str.Format(_T("%d"), nStep);
	::WritePrivateProfileString(_T("Auto"), _T("nStep"), str, sPath);
}

CString CGvisR2R_PunchView::GetAoiUpAlarmRestartMsg()
{
	CString sMsg = _T("Not Find Message.");
	TCHAR szData[512];
	CString sPath = PATH_ALARM;
	if (0 < ::GetPrivateProfileString(_T("11"), _T("27"), NULL, szData, sizeof(szData), sPath))
		sMsg = CString(szData);

	return sMsg;
}

CString CGvisR2R_PunchView::GetAoiUpAlarmReTestMsg()
{
	CString sMsg = _T("Not Find Message.");
	TCHAR szData[512];
	CString sPath = PATH_ALARM;
	if (0 < ::GetPrivateProfileString(_T("20"), _T("9"), NULL, szData, sizeof(szData), sPath))
		sMsg = CString(szData);

	return sMsg;
}

CString CGvisR2R_PunchView::GetAoiDnAlarmRestartMsg()
{
	CString sMsg = _T("Not Find Message.");
	TCHAR szData[512];
	CString sPath = PATH_ALARM;
	if (0 < ::GetPrivateProfileString(_T("11"), _T("28"), NULL, szData, sizeof(szData), sPath))
		sMsg = CString(szData);

	return sMsg;
}

CString CGvisR2R_PunchView::GetAoiDnAlarmReTestMsg()
{
	CString sMsg = _T("Not Find Message.");
	TCHAR szData[512];
	CString sPath = PATH_ALARM;
	if (0 < ::GetPrivateProfileString(_T("20"), _T("10"), NULL, szData, sizeof(szData), sPath))
		sMsg = CString(szData);

	return sMsg;
}

void CGvisR2R_PunchView::DoIO()
{
	//	ChkEmg();

	DoEmgSens();	//20220607
	DoSaftySens();	//20220603
	DoDoorSens();	//20220607

	DoModeSel();
	DoMainSw();
	DoEngraveSens();

	DoInterlock();

	MonPlcAlm();
	MonDispMain();
	MonPlcSignal();

	if (m_bCycleStop)
	{
		m_bCycleStop = FALSE;
		Buzzer(TRUE);
		TowerLamp(RGB_YELLOW, TRUE);
		//Stop();
		//MyMsgBox(pDoc->m_sAlmMsg);
		if (!pDoc->m_sAlmMsg.IsEmpty())
		{
			pDoc->LogAuto(pDoc->m_sAlmMsg);
			MsgBox(pDoc->m_sAlmMsg, 0, 0, DEFAULT_TIME_OUT, FALSE);

			if (pDoc->m_sAlmMsg == m_sAoiUpAlarmReStartMsg || pDoc->m_sAlmMsg == m_sAoiUpAlarmReTestMsg)
			{
				ChkReTestAlarmOnAoiUp();
				//SetAoiUpAutoStep(2); // Wait for AOI �˻���� ��ȣ.
				//Sleep(300);
				//if(m_pMpe)
				//	m_pMpe->Write(_T("MB44013B"), 1); // �˻�� ��� ���۾� (���۽�ȣ) : PC�� On��Ű�� PLC�� Off
			}
			else if(pDoc->m_sAlmMsg == m_sAoiDnAlarmReStartMsg || pDoc->m_sAlmMsg == m_sAoiDnAlarmReTestMsg)
			{
				ChkReTestAlarmOnAoiDn();
				//SetAoiDnAutoStep(2); // Wait for AOI �˻���� ��ȣ.
				//Sleep(300);
				//if(m_pMpe)
				//	m_pMpe->Write(_T("MB44013C"), 1); // �˻�� �Ϻ� ���۾� (���۽�ȣ) : PC�� On��Ű�� PLC�� Off
			}
		}
		pDoc->m_sAlmMsg = _T("");
		pDoc->m_sIsAlmMsg = _T("");
		pDoc->m_sPrevAlmMsg = _T("");
	}

	if (pDoc->Status.bManual)
	{
		DoBoxSw();
	}
	else if (pDoc->Status.bAuto)
	{
		DoAutoEng();
		DoAuto();
	}

	// 	DoSignal();
	// 	DoSens();
	// 	DoEmgSw();

	if (IsRun())
	{
		if (m_pDlgMenu01)
		{
			if (m_pDlgMenu01->IsEnableBtn())
				m_pDlgMenu01->EnableBtn(FALSE);
		}
	}
	else
	{
		if (m_pDlgMenu01)
		{
			if (!m_pDlgMenu01->IsEnableBtn())
				m_pDlgMenu01->EnableBtn(TRUE);
		}
	}

}

void CGvisR2R_PunchView::DoSens()
{
	//DoEmgSens();		// SliceIo[4]
	//DoMkSens();			// SliceIo[2]
	// 	DoUcSens1();		// MpeIo[1]
	// 	DoUcSens2();		// MpeIo[2]
	// 	DoUcSens3();		// MpeIo[3]
	// 	DoRcSens1();		// MpeIo[5]
	// 	DoRcSens2();		// MpeIo[6]
	// 	DoRcSens3();		// MpeIo[7]
}

void CGvisR2R_PunchView::DoBoxSw()
{
	// 	DoRcBoxSw();		// MpeIo[4]
	// 	DoAoiBoxSw();		// SliceIo[3]
	// 	DoUcBoxSw();		// MpeIo[0]
}

void CGvisR2R_PunchView::DoEmgSw()
{
#ifdef USE_MPE
	if (!pDoc->m_pMpeIo)// || !pDoc->m_pMpeIoF)
		return;

	// 	unsigned short usIn = pDoc->m_pMpeIo[4];
	// 	unsigned short *usInF = &pDoc->m_pMpeIoF[4];
	// 
	// 	if((usIn & (0x01<<0)) && !(*usInF & (0x01<<0)))
	// 	{
	// 		*usInF |= (0x01<<0);								// �����Ϸ� ������� ����ġ
	// 		pDoc->Status.bEmgRc = TRUE;
	// 	}
	// 	else if(!(usIn & (0x01<<0)) && (*usInF & (0x01<<0)))
	// 	{
	// 		*usInF &= ~(0x01<<0);								
	// 		pDoc->Status.bEmgRc = FALSE;
	// 	}
	// 
	// 	usIn = pDoc->m_pMpeIo[0];
	// 	usInF = &pDoc->m_pMpeIoF[0];
	// 
	// 	if((usIn & (0x01<<0)) && !(*usInF & (0x01<<0)))
	// 	{
	// 		*usInF |= (0x01<<0);								// �����Ϸ� ������� ����ġ
	// 		pDoc->Status.bEmgUc = TRUE;
	// 	}
	// 	else if(!(usIn & (0x01<<0)) && (*usInF & (0x01<<0)))
	// 	{
	// 		*usInF &= ~(0x01<<0);	
	// 		pDoc->Status.bEmgUc = FALSE;
	// 	}
#endif
}

void CGvisR2R_PunchView::GetInput()
{
	GetEnc();
	GetMpeSignal();
	DoMainSw();
}

void CGvisR2R_PunchView::GetEnc()
{
	if (!m_pMotion)
		return;

	m_dEnc[AXIS_X0] = m_pMotion->GetActualPosition(AXIS_X0);
	m_dEnc[AXIS_Y0] = m_pMotion->GetActualPosition(AXIS_Y0);
	m_dEnc[AXIS_X1] = m_pMotion->GetActualPosition(AXIS_X1);
	m_dEnc[AXIS_Y1] = m_pMotion->GetActualPosition(AXIS_Y1);
}

void CGvisR2R_PunchView::ChkMRegOut()
{
	m_bChkMpeIoOut = TRUE;
}

void CGvisR2R_PunchView::GetMpeIO()
{
#ifdef USE_MPE
	if (!m_pMpe || !pDoc->m_pMpeIo)
		return;

	m_pMpe->GetMpeIO();
#endif
}

void CGvisR2R_PunchView::GetMpeSignal()
{
#ifdef USE_MPE
	if (!m_pMpe || !pDoc->m_pMpeSignal)
		return;
	m_pMpe->GetMpeSignal();
#endif
}


void CGvisR2R_PunchView::GetMpeData()
{
#ifdef USE_MPE
	if (!m_pMpe || !pDoc->m_pMpeData)
		return;

	// MpeData
	int nSize, nIdx, nLoop, nSt, k;
	int nInSeg = pDoc->MkIo.MpeData.nInSeg;
	int nOutSeg = pDoc->MkIo.MpeData.nOutSeg;

	// MpeData - [In]
	int *pSeg0 = new int[nInSeg];
	long **pData0 = new long*[nInSeg];
	for (nIdx = 0; nIdx<nInSeg; nIdx++)
	{
		pData0[nIdx] = new long[16];
		memset(pData0[nIdx], 0, sizeof(long) * 16);
	}

	// Group Read...
	int nGrpStep = pDoc->MkIo.MpeData.nGrpStep;
	nSt = pDoc->MkIo.MpeData.nGrpInSt;
	for (nLoop = 0; nLoop<pDoc->MkIo.MpeData.nGrpIn; nLoop++)
	{
		for (nIdx = 0; nIdx<nGrpStep; nIdx++)
			pSeg0[nIdx] = nIdx + nSt;
		nSize = m_pMpe->ReadData(pSeg0, nGrpStep, pData0);

		for (nIdx = 0; nIdx<nGrpStep; nIdx++)
		{
			for (k = 0; k<16; k++)
				pDoc->m_pMpeData[nIdx + nSt][k] = pData0[nIdx][k];
		}

		nSt += nGrpStep;
	}
	for (nIdx = 0; nIdx<nInSeg; nIdx++)
		delete[] pData0[nIdx];
	delete[] pData0;
	delete[] pSeg0;

	m_bChkMpeIoOut = FALSE;

	// MpeData - [Out]
	int *pSeg2 = new int[nOutSeg];
	long **pData2 = new long*[nOutSeg];
	for (nIdx = 0; nIdx<nOutSeg; nIdx++)
	{
		pData2[nIdx] = new long[16];
		memset(pData2[nIdx], 0, sizeof(unsigned long) * 16);
	}

	// Group Read...
	nSt = pDoc->MkIo.MpeData.nGrpOutSt;
	for (nLoop = 0; nLoop<pDoc->MkIo.MpeData.nGrpOut; nLoop++)
	{
		for (nIdx = 0; nIdx<nGrpStep; nIdx++)
			pSeg2[nIdx] = nIdx + nSt;
		nSize = m_pMpe->ReadData(pSeg2, nGrpStep, pData2);

		for (nIdx = 0; nIdx<nGrpStep; nIdx++)
		{
			for (k = 0; k<16; k++)
				pDoc->m_pMpeData[nIdx + nSt][k] = pData2[nIdx][k];
		}

		nSt += nGrpStep;
	}
	for (nIdx = 0; nIdx<nOutSeg; nIdx++)
		delete[] pData2[nIdx];
	delete[] pData2;
	delete[] pSeg2;
#endif
}

int CGvisR2R_PunchView::GetTime(int nSel)
{

	time_t osBinTime;			// C run-time time (defined in <time.h>)
	time(&osBinTime);		// Get the current time from the 
							// operating system.
	CTime Tim(osBinTime);

	switch (nSel)
	{
	case YEAR:
		return Tim.GetYear();
	case MONTH:
		return Tim.GetMonth();
	case DAY:
		return Tim.GetDay();
	case HOUR:
		return Tim.GetHour();
	case MINUTE:
		return Tim.GetMinute();
	case SEC:
		return Tim.GetSecond();
	}

	return 0;
}

CString CGvisR2R_PunchView::GetTime()
{
	CString strVal;
	time_t osBinTime;			// C run-time time (defined in <time.h>)
	time(&osBinTime);		// Get the current time from the 
							// operating system.
	CTime Tim(osBinTime);

	int nYear = Tim.GetYear();
	int nMonth = Tim.GetMonth();
	int nDay = Tim.GetDay();
	int nHour = Tim.GetHour();
	int nMinute = Tim.GetMinute();
	int nSec = Tim.GetSecond();

	strVal.Format(_T("%04d-%02d-%02d, %02d:%02d:%02d"), nYear, nMonth, nDay, nHour, nMinute, nSec);
	return strVal;
}

CString CGvisR2R_PunchView::GetTime(stLotTime &LotTime)
{
	CString strVal;
	time_t osBinTime;			// C run-time time (defined in <time.h>)
	time(&osBinTime);		// Get the current time from the 
							// operating system.
	CTime Tim(osBinTime);

	LotTime.nYear = Tim.GetYear();
	LotTime.nMonth = Tim.GetMonth();
	LotTime.nDay = Tim.GetDay();
	LotTime.nHour = Tim.GetHour();
	LotTime.nMin = Tim.GetMinute();
	LotTime.nSec = Tim.GetSecond();

	strVal.Format(_T("%04d-%02d-%02d,%02d:%02d:%02d"), LotTime.nYear, LotTime.nMonth, LotTime.nDay,
		LotTime.nHour, LotTime.nMin, LotTime.nSec);
	return strVal;
}


CString CGvisR2R_PunchView::GetTime(int &nHour, int &nMinute, int &nSec)
{
	CString strVal;
	time_t osBinTime;			// C run-time time (defined in <time.h>)
	time(&osBinTime);		// Get the current time from the 
							// operating system.
	CTime Tim(osBinTime);

	int nYear = Tim.GetYear();
	int nMonth = Tim.GetMonth();
	int nDay = Tim.GetDay();
	nHour = Tim.GetHour();
	nMinute = Tim.GetMinute();
	nSec = Tim.GetSecond();

	strVal.Format(_T("%04d-%02d-%02d, %02d:%02d:%02d"), nYear, nMonth, nDay, nHour, nMinute, nSec);
	return strVal;
}

void CGvisR2R_PunchView::DispTime()
{
	stLotTime LotTime;
	CString str;
	str = GetTime(LotTime);
	if (m_sDispTime != str)
	{
		m_sDispTime = str;
		pFrm->DispStatusBar(str, 7);

		pDoc->WorkingInfo.Lot.CurTime.nYear = LotTime.nYear;
		pDoc->WorkingInfo.Lot.CurTime.nMonth = LotTime.nMonth;
		pDoc->WorkingInfo.Lot.CurTime.nDay = LotTime.nDay;
		pDoc->WorkingInfo.Lot.CurTime.nHour = LotTime.nHour;
		pDoc->WorkingInfo.Lot.CurTime.nMin = LotTime.nMin;
		pDoc->WorkingInfo.Lot.CurTime.nSec = LotTime.nSec;

		if (m_pDlgMenu01)
			m_pDlgMenu01->DispRunTime();
	}
}

// System Input IO .......................................................................
void CGvisR2R_PunchView::DoSaftySens()
{
#ifdef USE_MPE
	if (!pDoc->m_pMpeIb || !pDoc->m_pMpeIF)
		return;

	if (!IsRun())
		return;
	//BOOL bOn = pDoc->m_pMpeIb[7] & (0x01 << 8) ? TRUE : FALSE;	// ��ŷ�� ���� ���� 1
	//BOOL bOnF = pDoc->m_pMpeIF[7] & (0x01 << 8) ? TRUE : FALSE;	// ��ŷ�� ���� ���� 1

	unsigned short usIn = pDoc->m_pMpeIb[7];
	unsigned short *usInF = &pDoc->m_pMpeIF[7];


	if ((usIn & (0x01 << 8)) && !(*usInF & (0x01 << 8)))		// ��ŷ�� ���� ����
	{
		*usInF |= (0x01 << 8);								
		pDoc->Status.bSensSaftyMk = TRUE;
	}
	else if (!(usIn & (0x01 << 8)) && (*usInF & (0x01 << 8)))	// ��ŷ�� ���� ����
	{
		*usInF &= ~(0x01 << 8);
		pDoc->Status.bSensSaftyMk = FALSE;
	}
#endif
}

void CGvisR2R_PunchView::DoDoorSens()
{
#ifdef USE_MPE
	unsigned short usIn;
	unsigned short *usInF;

	if (!pDoc->m_pMpeIb || !pDoc->m_pMpeIF)
		return;

	if (!IsRun())
		return;

	usIn = pDoc->m_pMpeIb[1];
	usInF = &pDoc->m_pMpeIF[1];

	if ((usIn & (0x01 << 12)) && !(*usInF & (0x01 << 12)))		// �����Ϸ� ���� ���� ����
	{
		*usInF |= (0x01 << 12);
		pDoc->Status.bDoorUc[DOOR_FL_UC] = TRUE;
	}
	else if (!(usIn & (0x01 << 12)) && (*usInF & (0x01 << 12)))	// �����Ϸ� ���� ���� ����
	{
		*usInF &= ~(0x01 << 12);
		pDoc->Status.bDoorUc[DOOR_FL_UC] = FALSE;
	}

	if ((usIn & (0x01 << 13)) && !(*usInF & (0x01 << 13)))		// �����Ϸ� ���� ���� ����
	{
		*usInF |= (0x01 << 13);
		pDoc->Status.bDoorUc[DOOR_FR_UC] = TRUE;
	}
	else if (!(usIn & (0x01 << 13)) && (*usInF & (0x01 << 13)))	// �����Ϸ� ���� ���� ����
	{
		*usInF &= ~(0x01 << 13);
		pDoc->Status.bDoorUc[DOOR_FR_UC] = FALSE;
	}

	if ((usIn & (0x01 << 14)) && !(*usInF & (0x01 << 14)))		// �����Ϸ� �ĸ� ���� ����(��)
	{
		*usInF |= (0x01 << 14);
		pDoc->Status.bDoorUc[DOOR_BL_UC] = TRUE;
	}
	else if (!(usIn & (0x01 << 14)) && (*usInF & (0x01 << 14)))	// �����Ϸ� �ĸ� ���� ����(��)
	{
		*usInF &= ~(0x01 << 14);
		pDoc->Status.bDoorUc[DOOR_BL_UC] = FALSE;
	}

	if ((usIn & (0x01 << 15)) && !(*usInF & (0x01 << 15)))		// �����Ϸ� �ĸ� ���� ����(��)
	{
		*usInF |= (0x01 << 15);
		pDoc->Status.bDoorUc[DOOR_BR_UC] = TRUE;
	}
	else if (!(usIn & (0x01 << 15)) && (*usInF & (0x01 << 15)))	// �����Ϸ� �ĸ� ���� ����(��)
	{
		*usInF &= ~(0x01 << 15);
		pDoc->Status.bDoorUc[DOOR_BR_UC] = FALSE;
	}

	usIn = pDoc->m_pMpeIb[7];
	usInF = &pDoc->m_pMpeIF[7];

	if ((usIn & (0x01 << 10)) && !(*usInF & (0x01 << 10)))		// ��ŷ�� ���� ���� 1 
	{
		*usInF |= (0x01 << 10);
		pDoc->Status.bDoorMk[DOOR_FL_MK] = TRUE;
	}
	else if (!(usIn & (0x01 << 10)) && (*usInF & (0x01 << 10)))	// ��ŷ�� ���� ���� 1 
	{
		*usInF &= ~(0x01 << 10);
		pDoc->Status.bDoorMk[DOOR_FL_MK] = FALSE;
	}

	if ((usIn & (0x01 << 11)) && !(*usInF & (0x01 << 11)))		// ��ŷ�� ���� ���� 2 
	{
		*usInF |= (0x01 << 11);
		pDoc->Status.bDoorMk[DOOR_FR_MK] = TRUE;
	}
	else if (!(usIn & (0x01 << 11)) && (*usInF & (0x01 << 11)))	// ��ŷ�� ���� ���� 2
	{
		*usInF &= ~(0x01 << 11);
		pDoc->Status.bDoorMk[DOOR_FR_MK] = FALSE;
	}

	if ((usIn & (0x01 << 12)) && !(*usInF & (0x01 << 12)))		// ��ŷ�� ���� ���� 3 
	{
		*usInF |= (0x01 << 12);
		pDoc->Status.bDoorMk[DOOR_BL_MK] = TRUE;
	}
	else if (!(usIn & (0x01 << 12)) && (*usInF & (0x01 << 12)))	// ��ŷ�� ���� ���� 3
	{
		*usInF &= ~(0x01 << 12);
		pDoc->Status.bDoorMk[DOOR_BL_MK] = FALSE;
	}

	if ((usIn & (0x01 << 13)) && !(*usInF & (0x01 << 13)))		// ��ŷ�� ���� ���� 4 
	{
		*usInF |= (0x01 << 13);
		pDoc->Status.bDoorMk[DOOR_BR_MK] = TRUE;
	}
	else if (!(usIn & (0x01 << 13)) && (*usInF & (0x01 << 13)))	// ��ŷ�� ���� ���� 4
	{
		*usInF &= ~(0x01 << 13);
		pDoc->Status.bDoorMk[DOOR_BR_MK] = FALSE;
	}

	usIn = pDoc->m_pMpeIb[11];
	usInF = &pDoc->m_pMpeIF[11];

	if ((usIn & (0x01 << 10)) && !(*usInF & (0x01 << 10)))		// �˻�� �� ���� ���� 1 
	{
		*usInF |= (0x01 << 10);
		pDoc->Status.bDoorMk[DOOR_FL_AOI_UP] = TRUE;
	}
	else if (!(usIn & (0x01 << 10)) && (*usInF & (0x01 << 10)))	// �˻�� �� ���� ���� 1
	{
		*usInF &= ~(0x01 << 10);
		pDoc->Status.bDoorMk[DOOR_FL_AOI_UP] = FALSE;
	}

	if ((usIn & (0x01 << 11)) && !(*usInF & (0x01 << 11)))		// �˻�� �� ���� ���� 2 
	{
		*usInF |= (0x01 << 11);
		pDoc->Status.bDoorMk[DOOR_FR_AOI_UP] = TRUE;
	}
	else if (!(usIn & (0x01 << 11)) && (*usInF & (0x01 << 11)))	// �˻�� �� ���� ���� 2
	{
		*usInF &= ~(0x01 << 11);
		pDoc->Status.bDoorMk[DOOR_FR_AOI_UP] = FALSE;
	}

	if ((usIn & (0x01 << 12)) && !(*usInF & (0x01 << 12)))		// �˻�� �� ���� ���� 3 
	{
		*usInF |= (0x01 << 12);
		pDoc->Status.bDoorMk[DOOR_BL_AOI_UP] = TRUE;
	}
	else if (!(usIn & (0x01 << 12)) && (*usInF & (0x01 << 12)))	// �˻�� �� ���� ���� 3
	{
		*usInF &= ~(0x01 << 12);
		pDoc->Status.bDoorMk[DOOR_BL_AOI_UP] = FALSE;
	}

	if ((usIn & (0x01 << 13)) && !(*usInF & (0x01 << 13)))		// �˻�� �� ���� ���� 4 
	{
		*usInF |= (0x01 << 13);
		pDoc->Status.bDoorMk[DOOR_BR_AOI_UP] = TRUE;
	}
	else if (!(usIn & (0x01 << 13)) && (*usInF & (0x01 << 13)))	// �˻�� �� ���� ���� 4
	{
		*usInF &= ~(0x01 << 13);
		pDoc->Status.bDoorMk[DOOR_BR_AOI_UP] = FALSE;
	}

	usIn = pDoc->m_pMpeIb[15];
	usInF = &pDoc->m_pMpeIF[15];

	if ((usIn & (0x01 << 10)) && !(*usInF & (0x01 << 10)))		// �˻�� �� ���� ���� 1 
	{
		*usInF |= (0x01 << 10);
		pDoc->Status.bDoorAoi[DOOR_FL_AOI_UP] = TRUE;
	}
	else if (!(usIn & (0x01 << 10)) && (*usInF & (0x01 << 10)))	// �˻�� �� ���� ���� 1
	{
		*usInF &= ~(0x01 << 11);
		pDoc->Status.bDoorAoi[DOOR_FL_AOI_UP] = FALSE;
	}

	if ((usIn & (0x01 << 11)) && !(*usInF & (0x01 << 11)))		// �˻�� �� ���� ���� 2 
	{
		*usInF |= (0x01 << 11);
		pDoc->Status.bDoorAoi[DOOR_FR_AOI_UP] = TRUE;
	}
	else if (!(usIn & (0x01 << 11)) && (*usInF & (0x01 << 11)))	// �˻�� �� ���� ���� 2
	{
		*usInF &= ~(0x01 << 11);
		pDoc->Status.bDoorAoi[DOOR_FR_AOI_UP] = FALSE;
	}

	if ((usIn & (0x01 << 12)) && !(*usInF & (0x01 << 12)))		// �˻�� �� ���� ���� 3 
	{
		*usInF |= (0x01 << 12);
		pDoc->Status.bDoorAoi[DOOR_BL_AOI_UP] = TRUE;
	}
	else if (!(usIn & (0x01 << 12)) && (*usInF & (0x01 << 12)))	// �˻�� �� ���� ���� 3
	{
		*usInF &= ~(0x01 << 12);
		pDoc->Status.bDoorAoi[DOOR_BL_AOI_UP] = FALSE;
	}

	if ((usIn & (0x01 << 13)) && !(*usInF & (0x01 << 13)))		// �˻�� �� ���� ���� 4 
	{
		*usInF |= (0x01 << 13);
		pDoc->Status.bDoorAoi[DOOR_BR_AOI_UP] = TRUE;
	}
	else if (!(usIn & (0x01 << 13)) && (*usInF & (0x01 << 13)))	// �˻�� �� ���� ���� 4
	{
		*usInF &= ~(0x01 << 13);
		pDoc->Status.bDoorAoi[DOOR_BR_AOI_UP] = FALSE;
	}

	usIn = pDoc->m_pMpeIb[15];
	usInF = &pDoc->m_pMpeIF[15];

	if ((usIn & (0x01 << 10)) && !(*usInF & (0x01 << 10)))		// �˻�� �� ���� ���� 1 
	{
		*usInF |= (0x01 << 10);
		pDoc->Status.bDoorAoi[DOOR_FL_AOI_DN] = TRUE;
	}
	else if (!(usIn & (0x01 << 10)) && (*usInF & (0x01 << 10)))	// �˻�� �� ���� ���� 1
	{
		*usInF &= ~(0x01 << 10);
		pDoc->Status.bDoorAoi[DOOR_FL_AOI_DN] = FALSE;
	}

	if ((usIn & (0x01 << 11)) && !(*usInF & (0x01 << 11)))		// �˻�� �� ���� ���� 2 
	{
		*usInF |= (0x01 << 11);
		pDoc->Status.bDoorAoi[DOOR_FR_AOI_DN] = TRUE;
	}
	else if (!(usIn & (0x01 << 11)) && (*usInF & (0x01 << 11)))	// �˻�� �� ���� ���� 2
	{
		*usInF &= ~(0x01 << 11);
		pDoc->Status.bDoorAoi[DOOR_FR_AOI_DN] = FALSE;
	}

	if ((usIn & (0x01 << 12)) && !(*usInF & (0x01 << 12)))		// �˻�� �� ���� ���� 3 
	{
		*usInF |= (0x01 << 12);
		pDoc->Status.bDoorAoi[DOOR_BL_AOI_DN] = TRUE;
	}
	else if (!(usIn & (0x01 << 12)) && (*usInF & (0x01 << 12)))	// �˻�� �� ���� ���� 3
	{
		*usInF &= ~(0x01 << 12);
		pDoc->Status.bDoorAoi[DOOR_BL_AOI_DN] = FALSE;
	}

	if ((usIn & (0x01 << 13)) && !(*usInF & (0x01 << 13)))		// �˻�� �� ���� ���� 4 
	{
		*usInF |= (0x01 << 13);
		pDoc->Status.bDoorAoi[DOOR_BR_AOI_DN] = TRUE;
	}
	else if (!(usIn & (0x01 << 13)) && (*usInF & (0x01 << 13)))	// �˻�� �� ���� ���� 4
	{
		*usInF &= ~(0x01 << 13);
		pDoc->Status.bDoorAoi[DOOR_BR_AOI_DN] = FALSE;
	}

	usIn = pDoc->m_pMpeIb[17];
	usInF = &pDoc->m_pMpeIF[17];

	if ((usIn & (0x01 << 12)) && !(*usInF & (0x01 << 12)))		// �����Ϸ� ���� ���� ����
	{
		*usInF |= (0x01 << 12);
		pDoc->Status.bDoorRe[DOOR_FL_RC] = TRUE;
	}
	else if (!(usIn & (0x01 << 12)) && (*usInF & (0x01 << 12)))	// �����Ϸ� ���� ���� ����
	{
		*usInF &= ~(0x01 << 12);
		pDoc->Status.bDoorRe[DOOR_FL_RC] = FALSE;
	}

	if ((usIn & (0x01 << 13)) && !(*usInF & (0x01 << 13)))		// �����Ϸ� ���� ���� ����
	{
		*usInF |= (0x01 << 13);
		pDoc->Status.bDoorRe[DOOR_FR_RC] = TRUE;
	}
	else if (!(usIn & (0x01 << 13)) && (*usInF & (0x01 << 13)))	// �����Ϸ� ���� ���� ����
	{
		*usInF &= ~(0x01 << 13);
		pDoc->Status.bDoorRe[DOOR_FR_RC] = FALSE;
	}

	if ((usIn & (0x01 << 14)) && !(*usInF & (0x01 << 14)))		// �����Ϸ� �ĸ� ���� ����
	{
		*usInF |= (0x01 << 14);
		pDoc->Status.bDoorRe[DOOR_BL_RC] = TRUE;
	}
	else if (!(usIn & (0x01 << 14)) && (*usInF & (0x01 << 14)))	// �����Ϸ� �ĸ� ���� ����(��)
	{
		*usInF &= ~(0x01 << 14);
		pDoc->Status.bDoorRe[DOOR_BL_RC] = FALSE;
	}

	if ((usIn & (0x01 << 15)) && !(*usInF & (0x01 << 15)))		// �����Ϸ� �ĸ� ���� ����(��)
	{
		*usInF |= (0x01 << 15);
		pDoc->Status.bDoorRe[DOOR_BR_RC] = TRUE;
	}
	else if (!(usIn & (0x01 << 15)) && (*usInF & (0x01 << 15)))	// �����Ϸ� �ĸ� ���� ����(��)
	{
		*usInF &= ~(0x01 << 15);
		pDoc->Status.bDoorRe[DOOR_BR_RC] = FALSE;
	}

	usIn = pDoc->m_pMpeIb[27];
	usInF = &pDoc->m_pMpeIF[27];

	if ((usIn & (0x01 << 10)) && !(*usInF & (0x01 << 10)))		// ���κ� ���� ���� 1
	{
		*usInF |= (0x01 << 10);
		pDoc->Status.bDoorEngv[DOOR_FL_ENGV] = TRUE;
	}
	else if (!(usIn & (0x01 << 10)) && (*usInF & (0x01 << 10)))	// ���κ� ���� ���� 1
	{
		*usInF &= ~(0x01 << 10);
		pDoc->Status.bDoorEngv[DOOR_FL_ENGV] = FALSE;
	}

	if ((usIn & (0x01 << 11)) && !(*usInF & (0x01 << 11)))		// ���κ� ���� ���� 2
	{
		*usInF |= (0x01 << 11);
		pDoc->Status.bDoorEngv[DOOR_FR_ENGV] = TRUE;
	}
	else if (!(usIn & (0x01 << 11)) && (*usInF & (0x01 << 11)))	// ���κ� ���� ���� 2
	{
		*usInF &= ~(0x01 << 11);
		pDoc->Status.bDoorEngv[DOOR_FR_ENGV] = FALSE;
	}

	if ((usIn & (0x01 << 12)) && !(*usInF & (0x01 << 12)))		// ���κ� ���� ���� 3
	{
		*usInF |= (0x01 << 12);
		pDoc->Status.bDoorEngv[DOOR_BL_ENGV] = TRUE;
	}
	else if (!(usIn & (0x01 << 12)) && (*usInF & (0x01 << 12)))	// ���κ� ���� ���� 3
	{
		*usInF &= ~(0x01 << 12);
		pDoc->Status.bDoorEngv[DOOR_BL_ENGV] = FALSE;
	}

	if ((usIn & (0x01 << 13)) && !(*usInF & (0x01 << 13)))		// ���κ� ���� ���� 4
	{
		*usInF |= (0x01 << 13);
		pDoc->Status.bDoorEngv[DOOR_BR_ENGV] = TRUE;
	}
	else if (!(usIn & (0x01 << 13)) && (*usInF & (0x01 << 13)))	// ���κ� ���� ���� 4
	{
		*usInF &= ~(0x01 << 13);
		pDoc->Status.bDoorEngv[DOOR_BR_ENGV] = FALSE;
	}

	usIn = pDoc->m_pMpeIb[28];
	usInF = &pDoc->m_pMpeIF[28];


	if ((usIn & (0x01 << 4)) && !(*usInF & (0x01 << 4)))
	{
		*usInF |= (0x01 << 4);								// 2D ���� ����ġ
		m_bSwStopNow = FALSE;
		if (m_pDlgMenu03)
			m_pDlgMenu03->SwReset();
	}
	else if (!(usIn & (0x01 << 4)) && (*usInF & (0x01 << 4)))
	{
		*usInF &= ~(0x01 << 4);
	}

#endif
}

void CGvisR2R_PunchView::DoEngraveSens()
{
#ifdef USE_MPE
	unsigned short usIn;
	unsigned short *usInF;

	if (!pDoc->m_pMpeIb || !pDoc->m_pMpeIF)
		return;

	usIn = pDoc->m_pMpeIb[28];
	usInF = &pDoc->m_pMpeIF[28];


	if ((usIn & (0x01 << 4)) && !(*usInF & (0x01 << 4)))
	{
		*usInF |= (0x01 << 4);								// 2D ���� ����ġ
		m_bSwStopNow = FALSE;
		if (m_pDlgMenu03)
			m_pDlgMenu03->SwReset();
	}
	else if (!(usIn & (0x01 << 4)) && (*usInF & (0x01 << 4)))
	{
		*usInF &= ~(0x01 << 4);
	}
#endif
}

void CGvisR2R_PunchView::DoModeSel()
{
#ifdef USE_MPE
	if (!pDoc->m_pMpeIo)
		return;

	BOOL bMode;// [2];
	//bMode = pDoc->m_pMpeIb[36] & (0x01 << 5) ? TRUE : FALSE;	// ��ŷ�� �ڵ� ���� ����ġ ����
	bMode = pDoc->m_pMpeIo[36] & (0x01 << 5) ? TRUE : FALSE;	// ��ŷ�� �ڵ� ���� ����ġ ����
	if (bMode)
	{
		pDoc->Status.bAuto = TRUE;
		pDoc->Status.bManual = FALSE;
		pDoc->Status.bOneCycle = FALSE;

		m_bAuto = TRUE;
		m_bManual = FALSE;
		m_bOneCycle = FALSE;
	}
	else
	{
		pDoc->Status.bManual = TRUE;
		pDoc->Status.bAuto = FALSE;
		pDoc->Status.bOneCycle = FALSE;

		m_bManual = TRUE;
		m_bAuto = FALSE;
		m_bOneCycle = FALSE;
	}

	//bMode[0] = pDoc->m_pMpeIb[4] & (0x01 << 5) ? TRUE : FALSE;	// ��ŷ�� �ڵ�/����/1ȸ���� 1
	//bMode[1] = pDoc->m_pMpeIb[4] & (0x01 << 6) ? TRUE : FALSE;	// ��ŷ�� �ڵ�/����/1ȸ���� 2

	//if (bMode[0] && !bMode[1])		 // ��ŷ�� �ڵ�/����/1ȸ���� (1,2)
	//{
	//	pDoc->Status.bAuto = TRUE;
	//	pDoc->Status.bManual = FALSE;
	//	pDoc->Status.bOneCycle = FALSE;
	//}
	//else if (!bMode[0] && bMode[1])
	//{
	//	pDoc->Status.bAuto = FALSE;
	//	pDoc->Status.bManual = FALSE;
	//	pDoc->Status.bOneCycle = TRUE;
	//}
	//else if (!bMode[0] && !bMode[1])
	//{
	//	pDoc->Status.bAuto = FALSE;
	//	pDoc->Status.bManual = TRUE;
	//	pDoc->Status.bOneCycle = FALSE;
	//}
	//else
	//{
	//	pDoc->Status.bAuto = FALSE;
	//	pDoc->Status.bManual = FALSE;
	//	pDoc->Status.bOneCycle = FALSE;
	//}
#else
	pDoc->Status.bAuto = FALSE;
	pDoc->Status.bManual = TRUE;
	pDoc->Status.bOneCycle = FALSE;
#endif

	//if (pDoc->Status.bManual && !m_bManual)
	//{
	//	m_bManual = TRUE;
	//	m_bAuto = FALSE;
	//	m_bOneCycle = FALSE;

	//	if (m_pDlgMenu03)
	//	{
	//		m_pDlgMenu03->DoManual();
	//	}
	//}
	//else if (pDoc->Status.bAuto && !m_bAuto)
	//{
	//	m_bManual = FALSE;
	//	m_bAuto = TRUE;
	//	m_bOneCycle = FALSE;

	//	if (m_pDlgMenu03)
	//	{
	//		m_pDlgMenu03->DoAuto();
	//	}
	//}
	//else if (pDoc->Status.bOneCycle && !m_bOneCycle)
	//{
	//	m_bManual = FALSE;
	//	m_bAuto = FALSE;
	//	m_bOneCycle = TRUE;
	//}

}

void CGvisR2R_PunchView::DoMainSw()
{
#ifdef USE_MPE
	// 	if(!pDoc->m_pSliceIo || !pDoc->m_pSliceIoF)
	// 		return;
	// 
	// 	unsigned short usIn = pDoc->m_pSliceIo[0];
	// 	unsigned short *usInF = &pDoc->m_pSliceIoF[0];

	if (!pDoc->m_pMpeIb || !pDoc->m_pMpeIF)
		return;

	unsigned short usIn = pDoc->m_pMpeIb[4];
	unsigned short *usInF = &pDoc->m_pMpeIF[4];

	if ((usIn & (0x01 << 0)) && !(*usInF & (0x01 << 0)))
	{
		*usInF |= (0x01 << 0);								// ��ŷ�� ������� ����ġ(����ͺ�)
	}
	else if (!(usIn & (0x01 << 0)) && (*usInF & (0x01 << 0)))
	{
		*usInF &= ~(0x01 << 0);
	}

	if ((usIn & (0x01 << 1)) && !(*usInF & (0x01 << 1)))
	{
		*usInF |= (0x01 << 1);								// ��ŷ�� ���� ����ġ
		m_bSwStopNow = FALSE;
		if (m_pDlgMenu03)
			m_pDlgMenu03->SwRun();
	}
	else if (!(usIn & (0x01 << 1)) && (*usInF & (0x01 << 1)))
	{
		*usInF &= ~(0x01 << 1);
	}

	if ((usIn & (0x01 << 2)) && !(*usInF & (0x01 << 2)))
	{
		*usInF |= (0x01 << 2);								// ��ŷ�� ���� ����ġ
															// 		m_bSwStopNow = TRUE;
															// 		if(m_pDlgMenu03)
															// 			m_pDlgMenu03->SwStop();
	}
	else if (!(usIn & (0x01 << 2)) && (*usInF & (0x01 << 2)))
	{
		*usInF &= ~(0x01 << 2);
	}

	if ((usIn & (0x01 << 3)) && !(*usInF & (0x01 << 3)))
	{
		*usInF |= (0x01 << 3);								// ��ŷ�� �����غ� ����ġ
		m_bSwStopNow = FALSE;
		if (m_pDlgMenu03)
			m_pDlgMenu03->
			SwReady();
	}
	else if (!(usIn & (0x01 << 3)) && (*usInF & (0x01 << 3)))
	{
		*usInF &= ~(0x01 << 3);
	}

	if ((usIn & (0x01 << 4)) && !(*usInF & (0x01 << 4)))
	{
		*usInF |= (0x01 << 4);								// ��ŷ�� ���� ����ġ
		m_bSwStopNow = FALSE;
		if (m_pDlgMenu03)
			m_pDlgMenu03->SwReset();
	}
	else if (!(usIn & (0x01 << 4)) && (*usInF & (0x01 << 4)))
	{
		*usInF &= ~(0x01 << 4);
	}

	// 	if((usIn & (0x01<<6)) && !(*usInF & (0x01<<6)))
	// 	{
	// 		*usInF |= (0x01<<6);								// ��ŷ�� ��ŷ ����ġ
	// 
	// 		BOOL bOn = pDoc->m_pSliceIo[7] & (0x01<<10) ? TRUE : FALSE;	// ��ŷ�� ��ũ Ŭ���� ����ġ ���� -> ��ŷ�� ��ŷ �Ǹ��� SOL
	// 		if(!bOn)
	// 		{
	// 			if(m_pDlgMenu03)
	// 				m_pDlgMenu03->SwMkDnSol(TRUE);
	// 			Sleep(300);
	// 		}
	// 		if(m_pDlgMenu02)
	// 		{
	// 			m_pDlgMenu02->m_bMkDnSolOff = TRUE;
	// 			m_pDlgMenu02->SwMarking();
	// 		}
	// 	}
	// 	else if(!(usIn & (0x01<<6)) && (*usInF & (0x01<<6)))
	// 	{
	// 		*usInF &= ~(0x01<<6);								
	// 	}

	if ((usIn & (0x01 << 7)) && !(*usInF & (0x01 << 7)))
	{
		*usInF |= (0x01 << 7);								// ��ŷ�� JOG ��ư(��)
		if (pDoc->Status.bSwJogLeft)
			SwJog(AXIS_Y0, M_CCW, TRUE);
		else
			SwJog(AXIS_Y1, M_CCW, TRUE);
	}
	else if (!(usIn & (0x01 << 7)) && (*usInF & (0x01 << 7)))
	{
		*usInF &= ~(0x01 << 7);
		if (pDoc->Status.bSwJogLeft)
			SwJog(AXIS_Y0, M_CCW, FALSE);
		else
			SwJog(AXIS_Y1, M_CCW, FALSE);
	}

	if ((usIn & (0x01 << 8)) && !(*usInF & (0x01 << 8)))
	{
		*usInF |= (0x01 << 8);								// ��ŷ�� JOG ��ư(��)
		if (pDoc->Status.bSwJogLeft)
			SwJog(AXIS_Y0, M_CW, TRUE);
		else
			SwJog(AXIS_Y1, M_CW, TRUE);
	}
	else if (!(usIn & (0x01 << 8)) && (*usInF & (0x01 << 8)))
	{
		*usInF &= ~(0x01 << 8);
		if (pDoc->Status.bSwJogLeft)
			SwJog(AXIS_Y0, M_CW, FALSE);
		else
			SwJog(AXIS_Y1, M_CW, FALSE);
	}

	if ((usIn & (0x01 << 9)) && !(*usInF & (0x01 << 9)))
	{
		*usInF |= (0x01 << 9);								// ��ŷ�� JOG ��ư(��)
		if (pDoc->Status.bSwJogLeft)
			SwJog(AXIS_X0, M_CCW, TRUE);
		else
			SwJog(AXIS_X1, M_CCW, TRUE);
	}
	else if (!(usIn & (0x01 << 9)) && (*usInF & (0x01 << 9)))
	{
		*usInF &= ~(0x01 << 9);
		if (pDoc->Status.bSwJogLeft)
			SwJog(AXIS_X0, M_CCW, FALSE);
		else
			SwJog(AXIS_X1, M_CCW, FALSE);
	}

	if ((usIn & (0x01 << 10)) && !(*usInF & (0x01 << 10)))
	{
		*usInF |= (0x01 << 10);								// ��ŷ�� JOG ��ư(��)
		if (pDoc->Status.bSwJogLeft)
			SwJog(AXIS_X0, M_CW, TRUE);
		else
			SwJog(AXIS_X1, M_CW, TRUE);
	}
	else if (!(usIn & (0x01 << 10)) && (*usInF & (0x01 << 10)))
	{
		*usInF &= ~(0x01 << 10);
		if (pDoc->Status.bSwJogLeft)
			SwJog(AXIS_X0, M_CW, FALSE);
		else
			SwJog(AXIS_X1, M_CW, FALSE);
	}

	if ((usIn & (0x01 << 11)) && !(*usInF & (0x01 << 11)))
	{
		*usInF |= (0x01 << 11);								// ��ŷ�� ��� ����(LEFT)
		pDoc->Status.bSwJogLeft = TRUE;
	}
	else if (!(usIn & (0x01 << 11)) && (*usInF & (0x01 << 11)))
	{
		*usInF &= ~(0x01 << 11);
		pDoc->Status.bSwJogLeft = FALSE;
	}

	if ((usIn & (0x01 << 12)) && !(*usInF & (0x01 << 12)))
	{
		*usInF |= (0x01 << 12);								// ��ŷ�� �ӵ� ����
		pDoc->Status.bSwJogFast = TRUE;
	}
	else if (!(usIn & (0x01 << 12)) && (*usInF & (0x01 << 12)))
	{
		*usInF &= ~(0x01 << 12);
		pDoc->Status.bSwJogFast = FALSE;
	}

	if ((usIn & (0x01 << 13)) && !(*usInF & (0x01 << 13)))
	{
		*usInF |= (0x01 << 13);								// ��ŷ�� ���� ����(INDEX)
		pDoc->Status.bSwJogStep = TRUE;
	}
	else if (!(usIn & (0x01 << 13)) && (*usInF & (0x01 << 13)))
	{
		*usInF &= ~(0x01 << 13);
		pDoc->Status.bSwJogStep = FALSE;
	}

	if ((usIn & (0x01 << 14)) && !(*usInF & (0x01 << 14)))
	{
		*usInF |= (0x01 << 14);								// SPARE	
															// No Use....
	}
	else if (!(usIn & (0x01 << 14)) && (*usInF & (0x01 << 14)))
	{
		*usInF &= ~(0x01 << 14);
		// No Use....
	}

	if ((usIn & (0x01 << 15)) && !(*usInF & (0x01 << 15)))
	{
		*usInF |= (0x01 << 15);								// SPARE	
															// No Use....
	}
	else if (!(usIn & (0x01 << 15)) && (*usInF & (0x01 << 15)))
	{
		*usInF &= ~(0x01 << 15);
		// No Use....
	}

	if (!pDoc->m_pMpeIo)// || !pDoc->m_pMpeIoF)
		return;


	// 	usIn = pDoc->m_pMpeIo[0];
	// 	usInF = &pDoc->m_pMpeIoF[0];
	// 
	// 	if((usIn & (0x01<<14)) && !(*usInF & (0x01<<14)))
	// 	{
	// 		*usInF |= (0x01<<14);								// �����Ϸ� ��ǰ EPC���� ����ġ
	// 		m_bSwStopNow = TRUE;
	// 		if(m_pDlgMenu03)
	// 			m_pDlgMenu03->SwStop();
	// 	}
	// 	else if(!(usIn & (0x01<<14)) && (*usInF & (0x01<<14)))
	// 	{
	// 		*usInF &= ~(0x01<<14);								
	// 	}
	// 
	// 
	// 	usIn = pDoc->m_pMpeIo[4];
	// 	usInF = &pDoc->m_pMpeIoF[4];
	// 
	// 	if((usIn & (0x01<<14)) && !(*usInF & (0x01<<14)))
	// 	{
	// 		*usInF |= (0x01<<14);								// �����Ϸ� ��ǰ EPC���� ����ġ
	// 		m_bSwStopNow = TRUE;
	// 		if(m_pDlgMenu03)
	// 			m_pDlgMenu03->SwStop();
	// 	}
	// 	else if(!(usIn & (0x01<<14)) && (*usInF & (0x01<<14)))
	// 	{
	// 		*usInF &= ~(0x01<<14);								
	// 	}
#endif
}


void CGvisR2R_PunchView::DoMkSens()
{
	// 	if(!pDoc->m_pSliceIo || !pDoc->m_pSliceIoF)
	// 		return;
	// 
	// 	unsigned short usIn = pDoc->m_pSliceIo[2];
	// 	unsigned short *usInF = &pDoc->m_pSliceIoF[2];
	unsigned short usIn = 0;
	unsigned short *usInF = NULL;

	if ((usIn & (0x01 << 0)) && !(*usInF & (0x01 << 0)))
	{
		*usInF |= (0x01 << 0);								// ��ŷ�� ���̺� ���� ����
	}
	else if (!(usIn & (0x01 << 0)) && (*usInF & (0x01 << 0)))
	{
		*usInF &= ~(0x01 << 0);
	}

	if ((usIn & (0x01 << 1)) && !(*usInF & (0x01 << 1)))
	{
		*usInF |= (0x01 << 1);								// ��ŷ�� ���̺� �з� ����ġ
	}
	else if (!(usIn & (0x01 << 1)) && (*usInF & (0x01 << 1)))
	{
		*usInF &= ~(0x01 << 1);
	}

	if ((usIn & (0x01 << 2)) && !(*usInF & (0x01 << 2)))
	{
		*usInF |= (0x01 << 2);								// ��ŷ�� �ǵ� ���� ����
	}
	else if (!(usIn & (0x01 << 2)) && (*usInF & (0x01 << 2)))
	{
		*usInF &= ~(0x01 << 2);
	}

	if ((usIn & (0x01 << 3)) && !(*usInF & (0x01 << 3)))
	{
		*usInF |= (0x01 << 3);								// ��ŷ�� �ǵ� Ŭ���� ��� ����
	}
	else if (!(usIn & (0x01 << 3)) && (*usInF & (0x01 << 3)))
	{
		*usInF &= ~(0x01 << 3);
	}

	if ((usIn & (0x01 << 4)) && !(*usInF & (0x01 << 4)))
	{
		*usInF |= (0x01 << 4);								// ��ŷ�� �ǵ� Ŭ���� �ϰ� ����
	}
	else if (!(usIn & (0x01 << 4)) && (*usInF & (0x01 << 4)))
	{
		*usInF &= ~(0x01 << 4);
	}

	if ((usIn & (0x01 << 5)) && !(*usInF & (0x01 << 5)))
	{
		*usInF |= (0x01 << 5);								// ��ŷ�� ��ũ ���� ����
	}
	else if (!(usIn & (0x01 << 5)) && (*usInF & (0x01 << 5)))
	{
		*usInF &= ~(0x01 << 5);
	}

	if ((usIn & (0x01 << 6)) && !(*usInF & (0x01 << 6)))
	{
		*usInF |= (0x01 << 6);								// ��ŷ�� ��ũ Ŭ���� ��� ����
	}
	else if (!(usIn & (0x01 << 6)) && (*usInF & (0x01 << 6)))
	{
		*usInF &= ~(0x01 << 6);
	}

	if ((usIn & (0x01 << 7)) && !(*usInF & (0x01 << 7)))
	{
		*usInF |= (0x01 << 7);								// ��ŷ�� ��ũ Ŭ���� �ϰ� ����
	}
	else if (!(usIn & (0x01 << 7)) && (*usInF & (0x01 << 7)))
	{
		*usInF &= ~(0x01 << 7);
	}

	if ((usIn & (0x01 << 8)) && !(*usInF & (0x01 << 8)))
	{
		*usInF |= (0x01 << 8);								// ��ŷ�� ���� ��� ����
	}
	else if (!(usIn & (0x01 << 8)) && (*usInF & (0x01 << 8)))
	{
		*usInF &= ~(0x01 << 8);
	}

	if ((usIn & (0x01 << 9)) && !(*usInF & (0x01 << 9)))
	{
		*usInF |= (0x01 << 9);								// ��ŷ�� ���� �ϰ� ����
	}
	else if (!(usIn & (0x01 << 9)) && (*usInF & (0x01 << 9)))
	{
		*usInF &= ~(0x01 << 9);
	}

	if ((usIn & (0x01 << 10)) && !(*usInF & (0x01 << 10)))
	{
		*usInF |= (0x01 << 10);								// �˻�� �ǵ� ���� ����
	}
	else if (!(usIn & (0x01 << 10)) && (*usInF & (0x01 << 10)))
	{
		*usInF &= ~(0x01 << 10);
	}

	if ((usIn & (0x01 << 11)) && !(*usInF & (0x01 << 11)))
	{
		*usInF |= (0x01 << 11);								// �˻�� �ǵ� Ŭ���� ��� ����
	}
	else if (!(usIn & (0x01 << 11)) && (*usInF & (0x01 << 11)))
	{
		*usInF &= ~(0x01 << 11);
	}

	if ((usIn & (0x01 << 12)) && !(*usInF & (0x01 << 12)))
	{
		*usInF |= (0x01 << 12);								// �˻�� �ǵ� Ŭ���� �ϰ� ����
	}
	else if (!(usIn & (0x01 << 12)) && (*usInF & (0x01 << 12)))
	{
		*usInF &= ~(0x01 << 12);
	}

	if ((usIn & (0x01 << 13)) && !(*usInF & (0x01 << 13)))
	{
		*usInF |= (0x01 << 13);								// �˻�� ��ũ ���� ����
	}
	else if (!(usIn & (0x01 << 13)) && (*usInF & (0x01 << 13)))
	{
		*usInF &= ~(0x01 << 13);
	}

	if ((usIn & (0x01 << 14)) && !(*usInF & (0x01 << 14)))
	{
		*usInF |= (0x01 << 14);								// �˻�� ��ũ Ŭ���� ��� ����
	}
	else if (!(usIn & (0x01 << 14)) && (*usInF & (0x01 << 14)))
	{
		*usInF &= ~(0x01 << 14);
	}

	if ((usIn & (0x01 << 15)) && !(*usInF & (0x01 << 15)))
	{
		*usInF |= (0x01 << 15);								// �˻�� ��ũ Ŭ���� �ϰ� ����
	}
	else if (!(usIn & (0x01 << 15)) && (*usInF & (0x01 << 15)))
	{
		*usInF &= ~(0x01 << 15);
	}
}

void CGvisR2R_PunchView::DoAoiBoxSw()
{
	// 	if(!pDoc->m_pSliceIo || !pDoc->m_pSliceIoF)
	// 		return;
	// 
	// 	unsigned short usIn = pDoc->m_pSliceIo[3];
	// 	unsigned short *usInF = &pDoc->m_pSliceIoF[3];
	unsigned short usIn = 0;
	unsigned short *usInF = NULL;

	if ((usIn & (0x01 << 0)) && !(*usInF & (0x01 << 0)))
	{
		*usInF |= (0x01 << 0);								// �˻�� ���� ��/���� ����ġ
		if (m_pDlgMenu03)
			m_pDlgMenu03->SwAoiRelation();
	}
	else if (!(usIn & (0x01 << 0)) && (*usInF & (0x01 << 0)))
	{
		*usInF &= ~(0x01 << 0);
	}

	if ((usIn & (0x01 << 1)) && !(*usInF & (0x01 << 1)))
	{
		*usInF |= (0x01 << 1);								// �˻�� ���̺� ��ο� ����ġ
		if (m_pDlgMenu03)
			m_pDlgMenu03->SwAoiTblBlw();
	}
	else if (!(usIn & (0x01 << 1)) && (*usInF & (0x01 << 1)))
	{
		*usInF &= ~(0x01 << 1);
	}

	if ((usIn & (0x01 << 2)) && !(*usInF & (0x01 << 2)))
	{
		*usInF |= (0x01 << 2);								// �˻�� �ǵ� ��ȸ�� ����ġ
															// 		if(m_pDlgMenu03)
															// 			m_pDlgMenu03->SwMyBtnDown(IDC_CHK_18);
	}
	else if (!(usIn & (0x01 << 2)) && (*usInF & (0x01 << 2)))
	{
		*usInF &= ~(0x01 << 2);
		// 		if(m_pDlgMenu03)
		// 			m_pDlgMenu03->SwMyBtnUp(IDC_CHK_18);
	}

	if ((usIn & (0x01 << 3)) && !(*usInF & (0x01 << 3)))
	{
		*usInF |= (0x01 << 3);								// �˻�� �ǵ� ��ȸ�� ����ġ
															// 		if(m_pDlgMenu03)
															// 			m_pDlgMenu03->SwMyBtnDown(IDC_CHK_19);
	}
	else if (!(usIn & (0x01 << 3)) && (*usInF & (0x01 << 3)))
	{
		*usInF &= ~(0x01 << 3);
		// 		if(m_pDlgMenu03)
		// 			m_pDlgMenu03->SwMyBtnUp(IDC_CHK_19);
	}

	if ((usIn & (0x01 << 4)) && !(*usInF & (0x01 << 4)))
	{
		*usInF |= (0x01 << 4);								// �˻�� �ǵ� ���� ����ġ
		if (m_pDlgMenu03)
			m_pDlgMenu03->SwAoiFdVac();
	}
	else if (!(usIn & (0x01 << 4)) && (*usInF & (0x01 << 4)))
	{
		*usInF &= ~(0x01 << 4);
	}

	if ((usIn & (0x01 << 5)) && !(*usInF & (0x01 << 5)))
	{
		*usInF |= (0x01 << 5);								// �˻�� ��ũ ���� ����ġ
		if (m_pDlgMenu03)
			m_pDlgMenu03->SwAoiTqVac();
	}
	else if (!(usIn & (0x01 << 5)) && (*usInF & (0x01 << 5)))
	{
		*usInF &= ~(0x01 << 5);
	}

	if ((usIn & (0x01 << 6)) && !(*usInF & (0x01 << 6)))
	{
		*usInF |= (0x01 << 6);								// �˻�� ���̺� ���� ����ġ
		if (m_pDlgMenu03)
			m_pDlgMenu03->SwAoiTblVac();
	}
	else if (!(usIn & (0x01 << 6)) && (*usInF & (0x01 << 6)))
	{
		*usInF &= ~(0x01 << 6);
	}

	if ((usIn & (0x01 << 7)) && !(*usInF & (0x01 << 7)))
	{
		*usInF |= (0x01 << 7);								// �˻�� ������ ������ ����ġ
		if (m_pDlgMenu03)
			m_pDlgMenu03->SwAoiLsrPt(TRUE);
	}
	else if (!(usIn & (0x01 << 7)) && (*usInF & (0x01 << 7)))
	{
		*usInF &= ~(0x01 << 7);
		if (m_pDlgMenu03)
			m_pDlgMenu03->SwAoiLsrPt(FALSE);
	}

	if ((usIn & (0x01 << 8)) && !(*usInF & (0x01 << 8)))
	{
		*usInF |= (0x01 << 8);								// �˻�� �ǵ� Ŭ���� ����ġ
		if (m_pDlgMenu03)
			m_pDlgMenu03->SwAoiFdClp();
	}
	else if (!(usIn & (0x01 << 8)) && (*usInF & (0x01 << 8)))
	{
		*usInF &= ~(0x01 << 8);
	}

	if ((usIn & (0x01 << 9)) && !(*usInF & (0x01 << 9)))
	{
		*usInF |= (0x01 << 9);								// �˻�� ��ũ Ŭ���� ����ġ
		if (m_pDlgMenu03)
			m_pDlgMenu03->SwAoiTqClp();
	}
	else if (!(usIn & (0x01 << 9)) && (*usInF & (0x01 << 9)))
	{
		*usInF &= ~(0x01 << 9);
	}

	// 	if((usIn & (0x01<<10)) && !(*usInF & (0x01<<10)))
	// 	{
	// 		*usInF |= (0x01<<10);								// �˻�� ���� ���� ����(��)
	// 		pDoc->Status.bDoorAoi[DOOR_FM_AOI] = TRUE;
	// 	}
	// 	else if(!(usIn & (0x01<<10)) && (*usInF & (0x01<<10)))
	// 	{
	// 		*usInF &= ~(0x01<<10);								
	// 		pDoc->Status.bDoorAoi[DOOR_FM_AOI] = FALSE;
	// 	}
	// 
	// 	if((usIn & (0x01<<11)) && !(*usInF & (0x01<<11)))
	// 	{
	// 		*usInF |= (0x01<<11);								// �˻�� ���� ���� ����(��)
	// 		pDoc->Status.bDoorAoi[DOOR_FL_AOI] = TRUE;
	// 	}
	// 	else if(!(usIn & (0x01<<11)) && (*usInF & (0x01<<11)))
	// 	{
	// 		*usInF &= ~(0x01<<11);								
	// 		pDoc->Status.bDoorAoi[DOOR_FL_AOI] = FALSE;
	// 	}
	// 
	// 	if((usIn & (0x01<<12)) && !(*usInF & (0x01<<12)))
	// 	{
	// 		*usInF |= (0x01<<12);								// �˻�� ���� ���� ����(��)
	// 		pDoc->Status.bDoorAoi[DOOR_FR_AOI] = TRUE;
	// 	}
	// 	else if(!(usIn & (0x01<<12)) && (*usInF & (0x01<<12)))
	// 	{
	// 		*usInF &= ~(0x01<<12);								
	// 		pDoc->Status.bDoorAoi[DOOR_FR_AOI] = FALSE;
	// 	}
	// 
	// 	if((usIn & (0x01<<13)) && !(*usInF & (0x01<<13)))
	// 	{
	// 		*usInF |= (0x01<<13);								// �˻�� �ĸ� ���� ����(��)
	// 		pDoc->Status.bDoorAoi[DOOR_BM_AOI] = TRUE;
	// 	}
	// 	else if(!(usIn & (0x01<<13)) && (*usInF & (0x01<<13)))
	// 	{
	// 		*usInF &= ~(0x01<<13);								
	// 		pDoc->Status.bDoorAoi[DOOR_BM_AOI] = FALSE;
	// 	}
	// 
	// 	if((usIn & (0x01<<14)) && !(*usInF & (0x01<<14)))
	// 	{
	// 		*usInF |= (0x01<<14);								// �˻�� �ĸ� ���� ����(��)
	// 		pDoc->Status.bDoorAoi[DOOR_BL_AOI] = TRUE;
	// 	}
	// 	else if(!(usIn & (0x01<<14)) && (*usInF & (0x01<<14)))
	// 	{
	// 		*usInF &= ~(0x01<<14);								
	// 		pDoc->Status.bDoorAoi[DOOR_BL_AOI] = FALSE;
	// 	}
	// 
	// 	if((usIn & (0x01<<15)) && !(*usInF & (0x01<<15)))
	// 	{
	// 		*usInF |= (0x01<<15);								// �˻�� �ĸ� ���� ����(��)
	// 		pDoc->Status.bDoorAoi[DOOR_BR_AOI] = TRUE;
	// 	}
	// 	else if(!(usIn & (0x01<<15)) && (*usInF & (0x01<<15)))
	// 	{
	// 		*usInF &= ~(0x01<<15);								
	// 		pDoc->Status.bDoorAoi[DOOR_BR_AOI] = FALSE;
	// 	}
}

void CGvisR2R_PunchView::DoEmgSens()
{
#ifdef USE_MPE
	// 	if(!pDoc->m_pSliceIo || !pDoc->m_pSliceIoF)
	// 		return;

	// 	unsigned short usIn = pDoc->m_pSliceIo[4];
	// 	unsigned short *usInF = &pDoc->m_pSliceIoF[4];

	//unsigned short usIn = 0;
	//unsigned short *usInF = NULL;

	unsigned short usIn, *usInF;

	if (!pDoc->m_pMpeIb || !pDoc->m_pMpeIF)
		return;

	usIn = pDoc->m_pMpeIb[0];
	usInF = &pDoc->m_pMpeIF[0];

	if ((usIn & (0x01 << 0)) && !(*usInF & (0x01 << 0)))		// �����Ϸ� ������� ����ġ
	{
		*usInF |= (0x01 << 0);						
		pDoc->Status.bEmgUc = TRUE;
	}
	else if (!(usIn & (0x01 << 0)) && (*usInF & (0x01 << 0)))
	{
		*usInF &= ~(0x01 << 0);
		pDoc->Status.bEmgUc = FALSE;
	}

	usIn = pDoc->m_pMpeIb[4];
	usInF = &pDoc->m_pMpeIF[4];

	if ((usIn & (0x01 << 0)) && !(*usInF & (0x01 << 0)))		// ��ŷ�� ������� ����ġ(����ͺ�)
	{
		*usInF |= (0x01 << 0);						
		pDoc->Status.bEmgMk[EMG_M_MK] = TRUE;
	}
	else if (!(usIn & (0x01 << 0)) && (*usInF & (0x01 << 0)))
	{
		*usInF &= ~(0x01 << 0);
		pDoc->Status.bEmgMk[EMG_M_MK] = FALSE;
	}

	usIn = pDoc->m_pMpeIb[5];
	usInF = &pDoc->m_pMpeIF[5];

	if ((usIn & (0x01 << 0)) && !(*usInF & (0x01 << 0)))		// ��ŷ�� ������� ����ġ(����ġ��)	
	{
		*usInF |= (0x01 << 0);						
		pDoc->Status.bEmgMk[EMG_B_MK] = TRUE;
	}
	else if (!(usIn & (0x01 << 0)) && (*usInF & (0x01 << 0)))
	{
		*usInF &= ~(0x01 << 0);
		pDoc->Status.bEmgMk[EMG_B_MK] = FALSE;
	}

	usIn = pDoc->m_pMpeIb[8];
	usInF = &pDoc->m_pMpeIF[8];

	if ((usIn & (0x01 << 0)) && !(*usInF & (0x01 << 0)))		// �˻�� �� ������� ����ġ(�ĸ�) 
	{
		*usInF |= (0x01 << 0);						
		pDoc->Status.bEmgAoi[EMG_B_AOI_UP] = TRUE;
	}
	else if (!(usIn & (0x01 << 0)) && (*usInF & (0x01 << 0)))
	{
		*usInF &= ~(0x01 << 0);
		pDoc->Status.bEmgAoi[EMG_B_AOI_UP] = FALSE;
	}

	usIn = pDoc->m_pMpeIb[12];
	usInF = &pDoc->m_pMpeIF[12];

	if ((usIn & (0x01 << 0)) && !(*usInF & (0x01 << 0)))		// �˻�� �� ������� ����ġ(�ĸ�) 
	{
		*usInF |= (0x01 << 0);						
		pDoc->Status.bEmgAoi[EMG_B_AOI_DN] = TRUE;
	}
	else if (!(usIn & (0x01 << 0)) && (*usInF & (0x01 << 0)))
	{
		*usInF &= ~(0x01 << 0);
		pDoc->Status.bEmgAoi[EMG_B_AOI_DN] = FALSE;
	}

	usIn = pDoc->m_pMpeIb[16];
	usInF = &pDoc->m_pMpeIF[16];

	if ((usIn & (0x01 << 0)) && !(*usInF & (0x01 << 0)))		// �����Ϸ� ������� ����ġ
	{
		*usInF |= (0x01 << 0);
		pDoc->Status.bEmgRc = TRUE;
	}
	else if (!(usIn & (0x01 << 0)) && (*usInF & (0x01 << 0)))
	{
		*usInF &= ~(0x01 << 0);
		pDoc->Status.bEmgRc = FALSE;
	}

	//usIn = pDoc->m_pMpeIb[24];
	//usInF = &pDoc->m_pMpeIF[24];

	//if ((usIn & (0x01 << 0)) && !(*usInF & (0x01 << 0)))		// ���κ� ������� ����ġ(����ͺ�)
	//{
	//	*usInF |= (0x01 << 0);
	//	pDoc->Status.bEmgEngv[0] = TRUE;
	//}
	//else if (!(usIn & (0x01 << 0)) && (*usInF & (0x01 << 0)))
	//{
	//	*usInF &= ~(0x01 << 0);
	//	pDoc->Status.bEmgEngv[0] = FALSE;
	//}

	//usIn = pDoc->m_pMpeIb[25];
	//usInF = &pDoc->m_pMpeIF[25];

	//if ((usIn & (0x01 << 0)) && !(*usInF & (0x01 << 0)))		// ���κ� ������� ����ġ(����ġ��)
	//{
	//	*usInF |= (0x01 << 0);
	//	pDoc->Status.bEmgEngv[1] = TRUE;
	//}
	//else if (!(usIn & (0x01 << 0)) && (*usInF & (0x01 << 0)))
	//{
	//	*usInF &= ~(0x01 << 0);
	//	pDoc->Status.bEmgEngv[1] = FALSE;
	//}
#endif
}

//{
//	if ((usIn & (0x01 << 1)) && !(*usInF & (0x01 << 1)))
//	{
//		*usInF |= (0x01 << 1);								// �˻�� ������� ����ġ(�ĸ�)
//		pDoc->Status.bEmgAoi[EMG_B_AOI] = TRUE;
//		// 		pDoc->m_pSliceIo[9] |= (0x01<<15);
//	}
//	else if (!(usIn & (0x01 << 1)) && (*usInF & (0x01 << 1)))
//	{
//		*usInF &= ~(0x01 << 1);
//		pDoc->Status.bEmgAoi[EMG_B_AOI] = FALSE;
//		// 		pDoc->m_pSliceIo[9] &= ~(0x01<<15);
//	}
//
//	if ((usIn & (0x01 << 2)) && !(*usInF & (0x01 << 2)))
//	{
//		*usInF |= (0x01 << 2);								// �˻�� ���� ����
//		pDoc->Status.bMainAirAoi = TRUE;
//	}
//	else if (!(usIn & (0x01 << 2)) && (*usInF & (0x01 << 2)))
//	{
//		*usInF &= ~(0x01 << 2);
//		pDoc->Status.bMainAirAoi = FALSE;
//	}
//
//	if ((usIn & (0x01 << 3)) && !(*usInF & (0x01 << 3)))
//	{
//		*usInF |= (0x01 << 3);								// �˻�� ���̺� ���� ����
//		pDoc->Status.bSensTblVacAoi = TRUE;
//	}
//	else if (!(usIn & (0x01 << 3)) && (*usInF & (0x01 << 3)))
//	{
//		*usInF &= ~(0x01 << 3);
//		pDoc->Status.bSensTblVacAoi = FALSE;
//	}
//
//	if ((usIn & (0x01 << 4)) && !(*usInF & (0x01 << 4)))
//	{
//		*usInF |= (0x01 << 4);								// �˻�� ���̺� �з� ����ġ
//		pDoc->Status.bSensTblPrsAoi = TRUE;
//	}
//	else if (!(usIn & (0x01 << 4)) && (*usInF & (0x01 << 4)))
//	{
//		*usInF &= ~(0x01 << 4);
//		pDoc->Status.bSensTblPrsAoi = FALSE;
//	}
//
//	if ((usIn & (0x01 << 5)) && !(*usInF & (0x01 << 5)))
//	{
//		*usInF |= (0x01 << 5);								// ��ŷ�� ������� ����ġ(�����)
//		pDoc->Status.bEmgMk[EMG_M_MK] = TRUE;
//		// 		pDoc->m_pSliceIo[6] |= (0x01<<0);					// ��ŷ�� ������� ����ġ(�����) ����
//	}
//	else if (!(usIn & (0x01 << 5)) && (*usInF & (0x01 << 5)))
//	{
//		*usInF &= ~(0x01 << 5);
//		pDoc->Status.bEmgMk[EMG_M_MK] = FALSE;
//		// 		pDoc->m_pSliceIo[6] &= ~(0x01<<0);
//	}
//
//	if ((usIn & (0x01 << 6)) && !(*usInF & (0x01 << 6)))
//	{
//		*usInF |= (0x01 << 6);								// ��ŷ�� ������� ����ġ(����ġ)
//		pDoc->Status.bEmgMk[EMG_B_MK] = TRUE;
//	}
//	else if (!(usIn & (0x01 << 6)) && (*usInF & (0x01 << 6)))
//	{
//		*usInF &= ~(0x01 << 6);
//		pDoc->Status.bEmgMk[EMG_B_MK] = FALSE;
//	}
//
//	if ((usIn & (0x01 << 7)) && !(*usInF & (0x01 << 7)))
//	{
//		*usInF |= (0x01 << 7);								// ��ŷ�� ���� ����
//		pDoc->Status.bSensSaftyMk = TRUE;
//	}
//	else if (!(usIn & (0x01 << 7)) && (*usInF & (0x01 << 7)))
//	{
//		*usInF &= ~(0x01 << 7);
//		pDoc->Status.bSensSaftyMk = FALSE;
//	}
//
//	if ((usIn & (0x01 << 8)) && !(*usInF & (0x01 << 8)))
//	{
//		*usInF |= (0x01 << 8);								// ��ŷ�� ���� �ѷ� POS ����Ʈ ����
//		pDoc->Status.bSensLmtBufMk[LMT_POS] = TRUE;
//	}
//	else if (!(usIn & (0x01 << 8)) && (*usInF & (0x01 << 8)))
//	{
//		*usInF &= ~(0x01 << 8);
//		pDoc->Status.bSensLmtBufMk[LMT_POS] = FALSE;
//	}
//
//	if ((usIn & (0x01 << 9)) && !(*usInF & (0x01 << 9)))
//	{
//		*usInF |= (0x01 << 9);								// ��ŷ�� ���� �ѷ� NEG ����Ʈ ����
//		pDoc->Status.bSensLmtBufMk[LMT_NEG] = TRUE;
//	}
//	else if (!(usIn & (0x01 << 9)) && (*usInF & (0x01 << 9)))
//	{
//		*usInF &= ~(0x01 << 9);
//		pDoc->Status.bSensLmtBufMk[LMT_NEG] = FALSE;
//	}
//
//	if ((usIn & (0x01 << 10)) && !(*usInF & (0x01 << 10)))
//	{
//		*usInF |= (0x01 << 10);								// SPARE	
//															// No Use....
//	}
//	else if (!(usIn & (0x01 << 10)) && (*usInF & (0x01 << 10)))
//	{
//		*usInF &= ~(0x01 << 10);
//		// No Use....
//	}
//
//	if ((usIn & (0x01 << 11)) && !(*usInF & (0x01 << 11)))
//	{
//		*usInF |= (0x01 << 11);								// ��ŷ�� ���� ���� ����(��)
//		pDoc->Status.bDoorMk[DOOR_FL_MK] = TRUE;
//	}
//	else if (!(usIn & (0x01 << 11)) && (*usInF & (0x01 << 11)))
//	{
//		*usInF &= ~(0x01 << 11);
//		pDoc->Status.bDoorMk[DOOR_FL_MK] = FALSE;
//	}
//
//	if ((usIn & (0x01 << 12)) && !(*usInF & (0x01 << 12)))
//	{
//		*usInF |= (0x01 << 12);								// ��ŷ�� ���� ���� ����(��)
//		pDoc->Status.bDoorMk[DOOR_FR_MK] = TRUE;
//	}
//	else if (!(usIn & (0x01 << 12)) && (*usInF & (0x01 << 12)))
//	{
//		*usInF &= ~(0x01 << 12);
//		pDoc->Status.bDoorMk[DOOR_FR_MK] = FALSE;
//	}
//
//	if ((usIn & (0x01 << 13)) && !(*usInF & (0x01 << 13)))
//	{
//		*usInF |= (0x01 << 13);								// ��ŷ�� ���� ����
//		pDoc->Status.bMainAirMk = TRUE;
//	}
//	else if (!(usIn & (0x01 << 13)) && (*usInF & (0x01 << 13)))
//	{
//		*usInF &= ~(0x01 << 13);
//		pDoc->Status.bMainAirMk = FALSE;
//	}
//
//	if ((usIn & (0x01 << 14)) && !(*usInF & (0x01 << 14)))
//	{
//		*usInF |= (0x01 << 14);								// ��ŷ�� �ĸ� ���� ����(��)
//		pDoc->Status.bDoorMk[DOOR_BL_MK] = TRUE;
//	}
//	else if (!(usIn & (0x01 << 14)) && (*usInF & (0x01 << 14)))
//	{
//		*usInF &= ~(0x01 << 14);
//		pDoc->Status.bDoorMk[DOOR_BL_MK] = FALSE;
//	}
//
//	if ((usIn & (0x01 << 15)) && !(*usInF & (0x01 << 15)))
//	{
//		*usInF |= (0x01 << 15);								// ��ŷ�� �ĸ� ���� ����(��)
//		pDoc->Status.bDoorMk[DOOR_BR_MK] = TRUE;
//	}
//	else if (!(usIn & (0x01 << 15)) && (*usInF & (0x01 << 15)))
//	{
//		*usInF &= ~(0x01 << 15);
//		pDoc->Status.bDoorMk[DOOR_BR_MK] = FALSE;
//	}
//
//	// 	if(!pDoc->m_pSliceIo || !pDoc->m_pSliceIoF)
//	// 		return;
//	// 
//	// 
//	// 
//	// 	usIn = pDoc->m_pSliceIo[3];
//	// 	usInF = &pDoc->m_pSliceIoF[3];
//
//	if ((usIn & (0x01 << 10)) && !(*usInF & (0x01 << 10)))
//	{
//		*usInF |= (0x01 << 10);								// �˻�� ���� ���� ����(��)
//		pDoc->Status.bDoorAoi[DOOR_FM_AOI] = TRUE;
//	}
//	else if (!(usIn & (0x01 << 10)) && (*usInF & (0x01 << 10)))
//	{
//		*usInF &= ~(0x01 << 10);
//		pDoc->Status.bDoorAoi[DOOR_FM_AOI] = FALSE;
//	}
//
//	if ((usIn & (0x01 << 11)) && !(*usInF & (0x01 << 11)))
//	{
//		*usInF |= (0x01 << 11);								// �˻�� ���� ���� ����(��)
//		pDoc->Status.bDoorAoi[DOOR_FL_AOI] = TRUE;
//	}
//	else if (!(usIn & (0x01 << 11)) && (*usInF & (0x01 << 11)))
//	{
//		*usInF &= ~(0x01 << 11);
//		pDoc->Status.bDoorAoi[DOOR_FL_AOI] = FALSE;
//	}
//
//	if ((usIn & (0x01 << 12)) && !(*usInF & (0x01 << 12)))
//	{
//		*usInF |= (0x01 << 12);								// �˻�� ���� ���� ����(��)
//		pDoc->Status.bDoorAoi[DOOR_FR_AOI] = TRUE;
//	}
//	else if (!(usIn & (0x01 << 12)) && (*usInF & (0x01 << 12)))
//	{
//		*usInF &= ~(0x01 << 12);
//		pDoc->Status.bDoorAoi[DOOR_FR_AOI] = FALSE;
//	}
//
//	if ((usIn & (0x01 << 13)) && !(*usInF & (0x01 << 13)))
//	{
//		*usInF |= (0x01 << 13);								// �˻�� �ĸ� ���� ����(��)
//		pDoc->Status.bDoorAoi[DOOR_BM_AOI] = TRUE;
//	}
//	else if (!(usIn & (0x01 << 13)) && (*usInF & (0x01 << 13)))
//	{
//		*usInF &= ~(0x01 << 13);
//		pDoc->Status.bDoorAoi[DOOR_BM_AOI] = FALSE;
//	}
//
//	if ((usIn & (0x01 << 14)) && !(*usInF & (0x01 << 14)))
//	{
//		*usInF |= (0x01 << 14);								// �˻�� �ĸ� ���� ����(��)
//		pDoc->Status.bDoorAoi[DOOR_BL_AOI] = TRUE;
//	}
//	else if (!(usIn & (0x01 << 14)) && (*usInF & (0x01 << 14)))
//	{
//		*usInF &= ~(0x01 << 14);
//		pDoc->Status.bDoorAoi[DOOR_BL_AOI] = FALSE;
//	}
//
//	if ((usIn & (0x01 << 15)) && !(*usInF & (0x01 << 15)))
//	{
//		*usInF |= (0x01 << 15);								// �˻�� �ĸ� ���� ����(��)
//		pDoc->Status.bDoorAoi[DOOR_BR_AOI] = TRUE;
//	}
//	else if (!(usIn & (0x01 << 15)) && (*usInF & (0x01 << 15)))
//	{
//		*usInF &= ~(0x01 << 15);
//		pDoc->Status.bDoorAoi[DOOR_BR_AOI] = FALSE;
//	}
//
//}

void CGvisR2R_PunchView::DoSignal()
{
	// 	if(!pDoc->m_pSliceIo || !pDoc->m_pSliceIoF)
	// 		return;
	// 
	// 	unsigned short usIn = pDoc->m_pSliceIo[5];
	// 	unsigned short *usInF = &pDoc->m_pSliceIoF[5];
	unsigned short usIn = 0;
	unsigned short *usInF = NULL;

	if ((usIn & (0x01 << 0)) && !(*usInF & (0x01 << 0)))
	{
		*usInF |= (0x01 << 0);								// �˻�� �˻� �Ϸ�
		pDoc->Status.bSigTestDoneAoi = TRUE;
	}
	else if (!(usIn & (0x01 << 0)) && (*usInF & (0x01 << 0)))
	{
		*usInF &= ~(0x01 << 0);
		pDoc->Status.bSigTestDoneAoi = FALSE;
	}

	if ((usIn & (0x01 << 1)) && !(*usInF & (0x01 << 1)))
	{
		*usInF |= (0x01 << 1);								// �˻�� ���̺� ���� �Ϸ�
		pDoc->Status.bSigTblAirAoi = TRUE;
	}
	else if (!(usIn & (0x01 << 1)) && (*usInF & (0x01 << 1)))
	{
		*usInF &= ~(0x01 << 1);
		pDoc->Status.bSigTblAirAoi = FALSE;
	}

	if ((usIn & (0x01 << 2)) && !(*usInF & (0x01 << 2)))
	{
		*usInF |= (0x01 << 2);								// SPARE
															// No Use....
	}
	else if (!(usIn & (0x01 << 2)) && (*usInF & (0x01 << 2)))
	{
		*usInF &= ~(0x01 << 2);
		// No Use....
	}

	if ((usIn & (0x01 << 3)) && !(*usInF & (0x01 << 3)))
	{
		*usInF |= (0x01 << 3);								// SPARE
															// No Use....
	}
	else if (!(usIn & (0x01 << 3)) && (*usInF & (0x01 << 3)))
	{
		*usInF &= ~(0x01 << 3);
		// No Use....
	}

	if ((usIn & (0x01 << 4)) && !(*usInF & (0x01 << 4)))
	{
		*usInF |= (0x01 << 4);								// SPARE
															// No Use....
	}
	else if (!(usIn & (0x01 << 4)) && (*usInF & (0x01 << 4)))
	{
		*usInF &= ~(0x01 << 4);
		// No Use....
	}

	if ((usIn & (0x01 << 5)) && !(*usInF & (0x01 << 5)))
	{
		*usInF |= (0x01 << 5);								// SPARE
															// No Use....
	}
	else if (!(usIn & (0x01 << 5)) && (*usInF & (0x01 << 5)))
	{
		*usInF &= ~(0x01 << 5);
		// No Use....
	}

	if ((usIn & (0x01 << 6)) && !(*usInF & (0x01 << 6)))
	{
		*usInF |= (0x01 << 6);								// SPARE
															// No Use....
	}
	else if (!(usIn & (0x01 << 6)) && (*usInF & (0x01 << 6)))
	{
		*usInF &= ~(0x01 << 6);
		// No Use....
	}

	if ((usIn & (0x01 << 7)) && !(*usInF & (0x01 << 7)))
	{
		*usInF |= (0x01 << 7);								// SPARE
															// No Use....
	}
	else if (!(usIn & (0x01 << 7)) && (*usInF & (0x01 << 7)))
	{
		*usInF &= ~(0x01 << 7);
		// No Use....
	}

	if ((usIn & (0x01 << 8)) && !(*usInF & (0x01 << 8)))
	{
		*usInF |= (0x01 << 8);								// �˻�� �ñ׳� Ÿ��-����
		TowerLamp(RGB_RED, TRUE, FALSE);
	}
	else if (!(usIn & (0x01 << 8)) && (*usInF & (0x01 << 8)))
	{
		*usInF &= ~(0x01 << 8);
		TowerLamp(RGB_RED, FALSE, FALSE);
	}

	if ((usIn & (0x01 << 9)) && !(*usInF & (0x01 << 9)))
	{
		*usInF |= (0x01 << 9);								// �˻�� �ñ׳� Ÿ��-Ȳ��
		TowerLamp(RGB_YELLOW, TRUE, FALSE);
	}
	else if (!(usIn & (0x01 << 9)) && (*usInF & (0x01 << 9)))
	{
		*usInF &= ~(0x01 << 9);
		TowerLamp(RGB_YELLOW, FALSE, FALSE);
	}

	if ((usIn & (0x01 << 10)) && !(*usInF & (0x01 << 10)))
	{
		*usInF |= (0x01 << 10);								// �˻�� �ñ׳� Ÿ��-���
		TowerLamp(RGB_GREEN, TRUE, FALSE);
	}
	else if (!(usIn & (0x01 << 10)) && (*usInF & (0x01 << 10)))
	{
		*usInF &= ~(0x01 << 10);
		TowerLamp(RGB_GREEN, FALSE, FALSE);
	}

	if ((usIn & (0x01 << 11)) && !(*usInF & (0x01 << 11)))
	{
		*usInF |= (0x01 << 11);								// �˻�� ���� 1
		Buzzer(TRUE, 0);
	}
	else if (!(usIn & (0x01 << 11)) && (*usInF & (0x01 << 11)))
	{
		*usInF &= ~(0x01 << 11);
		Buzzer(FALSE, 0);
	}

	if ((usIn & (0x01 << 12)) && !(*usInF & (0x01 << 12)))
	{
		*usInF |= (0x01 << 12);								// �˻�� ���� 2
		Buzzer(TRUE, 1);
	}
	else if (!(usIn & (0x01 << 12)) && (*usInF & (0x01 << 12)))
	{
		*usInF &= ~(0x01 << 12);
		Buzzer(FALSE, 1);
	}

	if ((usIn & (0x01 << 13)) && !(*usInF & (0x01 << 13)))
	{
		*usInF |= (0x01 << 13);								// SPARE
															// No Use....
	}
	else if (!(usIn & (0x01 << 13)) && (*usInF & (0x01 << 13)))
	{
		*usInF &= ~(0x01 << 13);
		// No Use....
	}

	if ((usIn & (0x01 << 14)) && !(*usInF & (0x01 << 14)))
	{
		*usInF |= (0x01 << 14);								// SPARE
															// No Use....
	}
	else if (!(usIn & (0x01 << 14)) && (*usInF & (0x01 << 14)))
	{
		*usInF &= ~(0x01 << 14);
		// No Use....
	}

	if ((usIn & (0x01 << 15)) && !(*usInF & (0x01 << 15)))
	{
		*usInF |= (0x01 << 15);								// SPARE
															// No Use....
	}
	else if (!(usIn & (0x01 << 15)) && (*usInF & (0x01 << 15)))
	{
		*usInF &= ~(0x01 << 15);
		// No Use....
	}
}

void CGvisR2R_PunchView::DoUcBoxSw()
{
#ifdef USE_MPE
	if (!pDoc->m_pMpeIo)// || !pDoc->m_pMpeIoF)
		return;

	// 	unsigned short usIn = pDoc->m_pMpeIo[0];
	// 	unsigned short *usInF = &pDoc->m_pMpeIoF[0];
	unsigned short usIn = 0;
	unsigned short *usInF = NULL;

	// 	if((usIn & (0x01<<0)) && !(*usInF & (0x01<<0)))
	// 	{
	// 		*usInF |= (0x01<<0);								// �����Ϸ� ������� ����ġ
	// 		pDoc->Status.bEmgUc = TRUE;
	// 	}
	// 	else if(!(usIn & (0x01<<0)) && (*usInF & (0x01<<0)))
	// 	{
	// 		*usInF &= ~(0x01<<0);	
	// 		pDoc->Status.bEmgUc = FALSE;
	// 	}

	if ((usIn & (0x01 << 1)) && !(*usInF & (0x01 << 1)))
	{
		*usInF |= (0x01 << 1);								// �����Ϸ� ���� ��/���� ����ġ
		if (m_pDlgMenu03)
			m_pDlgMenu03->SwUcRelation();
	}
	else if (!(usIn & (0x01 << 1)) && (*usInF & (0x01 << 1)))
	{
		*usInF &= ~(0x01 << 1);
	}

	if ((usIn & (0x01 << 2)) && !(*usInF & (0x01 << 2)))
	{
		*usInF |= (0x01 << 2);								// �����Ϸ� ���� ���/�ϰ� ����ġ
		if (m_pDlgMenu03)
			m_pDlgMenu03->SwUcDcRlUpDn();
	}
	else if (!(usIn & (0x01 << 2)) && (*usInF & (0x01 << 2)))
	{
		*usInF &= ~(0x01 << 2);
	}

	if ((usIn & (0x01 << 3)) && !(*usInF & (0x01 << 3)))
	{
		*usInF |= (0x01 << 3);								// �����Ϸ� Ŭ���ѷ� ���/�ϰ� ����ġ
		if (m_pDlgMenu03)
			m_pDlgMenu03->SwUcClRlUpDn();
	}
	else if (!(usIn & (0x01 << 3)) && (*usInF & (0x01 << 3)))
	{
		*usInF &= ~(0x01 << 3);
	}

	if ((usIn & (0x01 << 4)) && !(*usInF & (0x01 << 4)))
	{
		*usInF |= (0x01 << 4);								// �����Ϸ� Ŭ���ѷ����� ���/�ϰ� ����ġ
		if (m_pDlgMenu03)
			m_pDlgMenu03->SwUcClRlPshUpDn();
	}
	else if (!(usIn & (0x01 << 4)) && (*usInF & (0x01 << 4)))
	{
		*usInF &= ~(0x01 << 4);
	}

	if ((usIn & (0x01 << 5)) && !(*usInF & (0x01 << 5)))
	{
		*usInF |= (0x01 << 5);								// �����Ϸ� ��ǰ ������(��) ����ġ
		if (m_pDlgMenu03)
			m_pDlgMenu03->SwUcReelJoinL();
	}
	else if (!(usIn & (0x01 << 5)) && (*usInF & (0x01 << 5)))
	{
		*usInF &= ~(0x01 << 5);
	}

	if ((usIn & (0x01 << 6)) && !(*usInF & (0x01 << 6)))
	{
		*usInF |= (0x01 << 6);								// �����Ϸ� ��ǰ ������(��) ����ġ	
		if (m_pDlgMenu03)
			m_pDlgMenu03->SwUcReelJoinR();
	}
	else if (!(usIn & (0x01 << 6)) && (*usInF & (0x01 << 6)))
	{
		*usInF &= ~(0x01 << 6);
	}

	if ((usIn & (0x01 << 7)) && !(*usInF & (0x01 << 7)))
	{
		*usInF |= (0x01 << 7);								// �����Ϸ� ��ǰ�� ���� ����ġ
		if (m_pDlgMenu03)
			m_pDlgMenu03->SwUcReelWheel();
	}
	else if (!(usIn & (0x01 << 7)) && (*usInF & (0x01 << 7)))
	{
		*usInF &= ~(0x01 << 7);
	}

	if ((usIn & (0x01 << 8)) && !(*usInF & (0x01 << 8)))
	{
		*usInF |= (0x01 << 8);								// �����Ϸ� ����ô Ŭ���� ����ġ
		if (m_pDlgMenu03)
			m_pDlgMenu03->SwUcPprChuck();
	}
	else if (!(usIn & (0x01 << 8)) && (*usInF & (0x01 << 8)))
	{
		*usInF &= ~(0x01 << 8);
	}

	if ((usIn & (0x01 << 9)) && !(*usInF & (0x01 << 9)))
	{
		*usInF |= (0x01 << 9);								// �����Ϸ� ������ ��ȸ�� ����ġ
															// 		if(m_pDlgMenu03)
															// 			m_pDlgMenu03->SwMyBtnDown(IDC_CHK_32);
	}
	else if (!(usIn & (0x01 << 9)) && (*usInF & (0x01 << 9)))
	{
		*usInF &= ~(0x01 << 9);
		// 		if(m_pDlgMenu03)
		// 			m_pDlgMenu03->SwMyBtnUp(IDC_CHK_32);
	}

	if ((usIn & (0x01 << 10)) && !(*usInF & (0x01 << 10)))
	{
		*usInF |= (0x01 << 10);								// �����Ϸ� ������ ��ȸ�� ����ġ
															// 		if(m_pDlgMenu03)
															// 			m_pDlgMenu03->SwMyBtnDown(IDC_CHK_39);
	}
	else if (!(usIn & (0x01 << 10)) && (*usInF & (0x01 << 10)))
	{
		*usInF &= ~(0x01 << 10);
		// 		if(m_pDlgMenu03)
		// 			m_pDlgMenu03->SwMyBtnUp(IDC_CHK_39);
	}

	if ((usIn & (0x01 << 11)) && !(*usInF & (0x01 << 11)))
	{
		*usInF |= (0x01 << 11);								// �����Ϸ� ��ǰô Ŭ���� ����ġ
		if (m_pDlgMenu03)
			m_pDlgMenu03->SwUcReelChuck();
	}
	else if (!(usIn & (0x01 << 11)) && (*usInF & (0x01 << 11)))
	{
		*usInF &= ~(0x01 << 11);
	}

	if ((usIn & (0x01 << 12)) && !(*usInF & (0x01 << 12)))
	{
		*usInF |= (0x01 << 12);								// �����Ϸ� ��ǰ�� ��ȸ�� ����ġ
															// 		if(m_pDlgMenu03)
															// 			m_pDlgMenu03->SwMyBtnDown(IDC_CHK_26);
	}
	else if (!(usIn & (0x01 << 12)) && (*usInF & (0x01 << 12)))
	{
		*usInF &= ~(0x01 << 12);
		// 		if(m_pDlgMenu03)
		// 			m_pDlgMenu03->SwMyBtnUp(IDC_CHK_26);
	}

	if ((usIn & (0x01 << 13)) && !(*usInF & (0x01 << 13)))
	{
		*usInF |= (0x01 << 13);								// �����Ϸ� ��ǰ�� ��ȸ�� ����ġ
															// 		if(m_pDlgMenu03)
															// 			m_pDlgMenu03->SwMyBtnDown(IDC_CHK_27);
	}
	else if (!(usIn & (0x01 << 13)) && (*usInF & (0x01 << 13)))
	{
		*usInF &= ~(0x01 << 13);
		// 		if(m_pDlgMenu03)
		// 			m_pDlgMenu03->SwMyBtnUp(IDC_CHK_27);
	}

	// 	if((usIn & (0x01<<14)) && !(*usInF & (0x01<<14)))
	// 	{
	// 		*usInF |= (0x01<<14);								// �����Ϸ� ��ǰ EPC���� ����ġ
	// 		m_bSwStopNow = TRUE;
	// 		if(m_pDlgMenu03)
	// 			m_pDlgMenu03->SwStop();
	// 	}
	// 	else if(!(usIn & (0x01<<14)) && (*usInF & (0x01<<14)))
	// 	{
	// 		*usInF &= ~(0x01<<14);								
	// 	}

	if ((usIn & (0x01 << 15)) && !(*usInF & (0x01 << 15)))
	{
		*usInF |= (0x01 << 15);								// SPARE
															// No Use....
	}
	else if (!(usIn & (0x01 << 15)) && (*usInF & (0x01 << 15)))
	{
		*usInF &= ~(0x01 << 15);
		// No Use....
	}
#endif
}

void CGvisR2R_PunchView::DoUcSens1()
{
#ifdef USE_MPE
	if (!pDoc->m_pMpeIo)// || !pDoc->m_pMpeIoF)
		return;

	unsigned short usIn = 0;
	unsigned short *usInF = NULL;
	// 	unsigned short usIn = pDoc->m_pMpeIo[1];
	// 	unsigned short *usInF = &pDoc->m_pMpeIoF[1];

	if ((usIn & (0x01 << 0)) && !(*usInF & (0x01 << 0)))
	{
		*usInF |= (0x01 << 0);								// �����Ϸ� ������������ 1	
															// Late....
	}
	else if (!(usIn & (0x01 << 0)) && (*usInF & (0x01 << 0)))
	{
		*usInF &= ~(0x01 << 0);
	}

	if ((usIn & (0x01 << 1)) && !(*usInF & (0x01 << 1)))
	{
		*usInF |= (0x01 << 1);								// �����Ϸ� ������������ 2	
															// Late....
	}
	else if (!(usIn & (0x01 << 1)) && (*usInF & (0x01 << 1)))
	{
		*usInF &= ~(0x01 << 1);
	}

	if ((usIn & (0x01 << 2)) && !(*usInF & (0x01 << 2)))
	{
		*usInF |= (0x01 << 2);								// �����Ϸ� ������������ 3	
															// Late....
	}
	else if (!(usIn & (0x01 << 2)) && (*usInF & (0x01 << 2)))
	{
		*usInF &= ~(0x01 << 2);
	}

	if ((usIn & (0x01 << 3)) && !(*usInF & (0x01 << 3)))
	{
		*usInF |= (0x01 << 3);								// �����Ϸ� ������������ 4	
															// Late....
	}
	else if (!(usIn & (0x01 << 3)) && (*usInF & (0x01 << 3)))
	{
		*usInF &= ~(0x01 << 3);
	}

	if ((usIn & (0x01 << 4)) && !(*usInF & (0x01 << 4)))
	{
		*usInF |= (0x01 << 4);								// �����Ϸ� ��ǰ�������� 1	
															// Late....
	}
	else if (!(usIn & (0x01 << 4)) && (*usInF & (0x01 << 4)))
	{
		*usInF &= ~(0x01 << 4);
	}

	if ((usIn & (0x01 << 5)) && !(*usInF & (0x01 << 5)))
	{
		*usInF |= (0x01 << 5);								// �����Ϸ� ��ǰ�������� 2
															// Late....
	}
	else if (!(usIn & (0x01 << 5)) && (*usInF & (0x01 << 5)))
	{
		*usInF &= ~(0x01 << 5);
	}

	if ((usIn & (0x01 << 6)) && !(*usInF & (0x01 << 6)))
	{
		*usInF |= (0x01 << 6);								// �����Ϸ� ��ǰ�������� 3	
															// Late....
	}
	else if (!(usIn & (0x01 << 6)) && (*usInF & (0x01 << 6)))
	{
		*usInF &= ~(0x01 << 6);
	}

	if ((usIn & (0x01 << 7)) && !(*usInF & (0x01 << 7)))
	{
		*usInF |= (0x01 << 7);								// �����Ϸ� ��ǰ�������� 4	
															// Late....
	}
	else if (!(usIn & (0x01 << 7)) && (*usInF & (0x01 << 7)))
	{
		*usInF &= ~(0x01 << 7);
	}

	if ((usIn & (0x01 << 8)) && !(*usInF & (0x01 << 8)))
	{
		*usInF |= (0x01 << 8);								// �����Ϸ� ��ǰ EPC POS ����Ʈ ����
															// Late....
	}
	else if (!(usIn & (0x01 << 8)) && (*usInF & (0x01 << 8)))
	{
		*usInF &= ~(0x01 << 8);
	}

	if ((usIn & (0x01 << 9)) && !(*usInF & (0x01 << 9)))
	{
		*usInF |= (0x01 << 9);								// �����Ϸ� ��ǰ EPC NEG ����Ʈ ����
															// Late....
	}
	else if (!(usIn & (0x01 << 9)) && (*usInF & (0x01 << 9)))
	{
		*usInF &= ~(0x01 << 9);
	}

	if ((usIn & (0x01 << 10)) && !(*usInF & (0x01 << 10)))
	{
		*usInF |= (0x01 << 10);								// �����Ϸ� ��ǰ EPC ���� ���� 
															// Late....
	}
	else if (!(usIn & (0x01 << 10)) && (*usInF & (0x01 << 10)))
	{
		*usInF &= ~(0x01 << 10);
	}

	if ((usIn & (0x01 << 11)) && !(*usInF & (0x01 << 11)))
	{
		*usInF |= (0x01 << 11);								// �����Ϸ� ���� ���� ����(��)
		pDoc->Status.bDoorUc[DOOR_FL_UC] = TRUE;
	}
	else if (!(usIn & (0x01 << 11)) && (*usInF & (0x01 << 11)))
	{
		*usInF &= ~(0x01 << 11);
		pDoc->Status.bDoorUc[DOOR_FL_UC] = FALSE;
	}

	if ((usIn & (0x01 << 12)) && !(*usInF & (0x01 << 12)))
	{
		*usInF |= (0x01 << 12);								// �����Ϸ� ���� ���� ����(��)
															// 		pDoc->Status.bDoorUc[DOOR_FR_UC] = TRUE;
	}
	else if (!(usIn & (0x01 << 12)) && (*usInF & (0x01 << 12)))
	{
		*usInF &= ~(0x01 << 12);
		// 		pDoc->Status.bDoorUc[DOOR_FR_UC] = FALSE;
	}

	if ((usIn & (0x01 << 13)) && !(*usInF & (0x01 << 13)))
	{
		*usInF |= (0x01 << 13);								// �����Ϸ� ���� ���� ����
		pDoc->Status.bDoorUc[DOOR_FR_UC] = TRUE;
	}
	else if (!(usIn & (0x01 << 13)) && (*usInF & (0x01 << 13)))
	{
		*usInF &= ~(0x01 << 13);
		pDoc->Status.bDoorUc[DOOR_FR_UC] = FALSE;
	}

	if ((usIn & (0x01 << 14)) && !(*usInF & (0x01 << 14)))
	{
		*usInF |= (0x01 << 14);								// �����Ϸ� �ĸ� ���� ����(��)
		pDoc->Status.bDoorUc[DOOR_BL_UC] = TRUE;
	}
	else if (!(usIn & (0x01 << 14)) && (*usInF & (0x01 << 14)))
	{
		*usInF &= ~(0x01 << 14);
		pDoc->Status.bDoorUc[DOOR_BL_UC] = FALSE;
	}

	if ((usIn & (0x01 << 15)) && !(*usInF & (0x01 << 15)))
	{
		*usInF |= (0x01 << 15);								// �����Ϸ� �ĸ� ���� ����(��)
		pDoc->Status.bDoorUc[DOOR_BR_UC] = TRUE;
	}
	else if (!(usIn & (0x01 << 15)) && (*usInF & (0x01 << 15)))
	{
		*usInF &= ~(0x01 << 15);
		pDoc->Status.bDoorUc[DOOR_BR_UC] = FALSE;
	}
#endif
}

void CGvisR2R_PunchView::DoUcSens2()
{
#ifdef USE_MPE
	if (!pDoc->m_pMpeIo)// || !pDoc->m_pMpeIoF)
		return;

	unsigned short usIn = 0;
	unsigned short *usInF = NULL;
	// 	unsigned short usIn = pDoc->m_pMpeIo[2];
	// 	unsigned short *usInF = &pDoc->m_pMpeIoF[2];

	if ((usIn & (0x01 << 0)) && !(*usInF & (0x01 << 0)))
	{
		*usInF |= (0x01 << 0);								// �����Ϸ� �����ټ� POS ����Ʈ ����
															// Late....
	}
	else if (!(usIn & (0x01 << 0)) && (*usInF & (0x01 << 0)))
	{
		*usInF &= ~(0x01 << 0);
	}

	if ((usIn & (0x01 << 1)) && !(*usInF & (0x01 << 1)))
	{
		*usInF |= (0x01 << 1);								// �����Ϸ� �����ټ� ���� ����
															// Late....
	}
	else if (!(usIn & (0x01 << 1)) && (*usInF & (0x01 << 1)))
	{
		*usInF &= ~(0x01 << 1);
	}

	if ((usIn & (0x01 << 2)) && !(*usInF & (0x01 << 2)))
	{
		*usInF |= (0x01 << 2);								// �����Ϸ� �����ټ� 2/3 ���� ����
															// Late....
	}
	else if (!(usIn & (0x01 << 2)) && (*usInF & (0x01 << 2)))
	{
		*usInF &= ~(0x01 << 2);
	}

	if ((usIn & (0x01 << 3)) && !(*usInF & (0x01 << 3)))
	{
		*usInF |= (0x01 << 3);								// �����Ϸ� �����ټ� 1/3 ���� ����
															// Late....
	}
	else if (!(usIn & (0x01 << 3)) && (*usInF & (0x01 << 3)))
	{
		*usInF &= ~(0x01 << 3);
	}

	if ((usIn & (0x01 << 4)) && !(*usInF & (0x01 << 4)))
	{
		*usInF |= (0x01 << 4);								// �����Ϸ� �����ټ� ���� ����
															// Late....
	}
	else if (!(usIn & (0x01 << 4)) && (*usInF & (0x01 << 4)))
	{
		*usInF &= ~(0x01 << 4);
	}

	if ((usIn & (0x01 << 5)) && !(*usInF & (0x01 << 5)))
	{
		*usInF |= (0x01 << 5);								// �����Ϸ� �����ټ� NEG ����Ʈ ����
															// Late....
	}
	else if (!(usIn & (0x01 << 5)) && (*usInF & (0x01 << 5)))
	{
		*usInF &= ~(0x01 << 5);
	}

	if ((usIn & (0x01 << 6)) && !(*usInF & (0x01 << 6)))
	{
		*usInF |= (0x01 << 6);								// �����Ϸ� ����ô ���� ����ġ
															// Late....
	}
	else if (!(usIn & (0x01 << 6)) && (*usInF & (0x01 << 6)))
	{
		*usInF &= ~(0x01 << 6);
	}

	if ((usIn & (0x01 << 7)) && !(*usInF & (0x01 << 7)))
	{
		*usInF |= (0x01 << 7);								// �����Ϸ� �������� �з� ����ġ	
															// Late....
	}
	else if (!(usIn & (0x01 << 7)) && (*usInF & (0x01 << 7)))
	{
		*usInF &= ~(0x01 << 7);
	}

	if ((usIn & (0x01 << 8)) && !(*usInF & (0x01 << 8)))
	{
		*usInF |= (0x01 << 8);								// �����Ϸ� Ŭ���ѷ� ��� ����
															// Late....
	}
	else if (!(usIn & (0x01 << 8)) && (*usInF & (0x01 << 8)))
	{
		*usInF &= ~(0x01 << 8);
	}

	if ((usIn & (0x01 << 9)) && !(*usInF & (0x01 << 9)))
	{
		*usInF |= (0x01 << 9);								// �����Ϸ� Ŭ���ѷ� �ϰ� ����
															// Late....
	}
	else if (!(usIn & (0x01 << 9)) && (*usInF & (0x01 << 9)))
	{
		*usInF &= ~(0x01 << 9);
	}

	if ((usIn & (0x01 << 10)) && !(*usInF & (0x01 << 10)))
	{
		*usInF |= (0x01 << 10);								// �����Ϸ� Ŭ���ѷ� ����(����) ��� ����
															// Late....
	}
	else if (!(usIn & (0x01 << 10)) && (*usInF & (0x01 << 10)))
	{
		*usInF &= ~(0x01 << 10);
	}

	if ((usIn & (0x01 << 11)) && !(*usInF & (0x01 << 11)))
	{
		*usInF |= (0x01 << 11);								// �����Ϸ� Ŭ���ѷ� ����(����) �ϰ� ����
															// Late....
	}
	else if (!(usIn & (0x01 << 11)) && (*usInF & (0x01 << 11)))
	{
		*usInF &= ~(0x01 << 11);
	}

	if ((usIn & (0x01 << 12)) && !(*usInF & (0x01 << 12)))
	{
		*usInF |= (0x01 << 12);								// �����Ϸ� Ŭ���ѷ� ����(�ĸ�) ��� ����
															// Late....
	}
	else if (!(usIn & (0x01 << 12)) && (*usInF & (0x01 << 12)))
	{
		*usInF &= ~(0x01 << 12);
	}

	if ((usIn & (0x01 << 13)) && !(*usInF & (0x01 << 13)))
	{
		*usInF |= (0x01 << 13);								// �����Ϸ� Ŭ���ѷ� ����(�ĸ�) �ϰ� ����
															// Late....
	}
	else if (!(usIn & (0x01 << 13)) && (*usInF & (0x01 << 13)))
	{
		*usInF &= ~(0x01 << 13);
	}

	if ((usIn & (0x01 << 14)) && !(*usInF & (0x01 << 14)))
	{
		*usInF |= (0x01 << 14);								// �����Ϸ� ���� ��� ����
															// Late....
	}
	else if (!(usIn & (0x01 << 14)) && (*usInF & (0x01 << 14)))
	{
		*usInF &= ~(0x01 << 14);
	}

	if ((usIn & (0x01 << 15)) && !(*usInF & (0x01 << 15)))
	{
		*usInF |= (0x01 << 15);								// �����Ϸ� ���� �ϰ� ����
															// Late....
	}
	else if (!(usIn & (0x01 << 15)) && (*usInF & (0x01 << 15)))
	{
		*usInF &= ~(0x01 << 15);
	}
#endif
}

void CGvisR2R_PunchView::DoUcSens3()
{
#ifdef USE_MPE
	if (!pDoc->m_pMpeIo)// || !pDoc->m_pMpeIoF)
		return;

	// 	unsigned short usIn = pDoc->m_pMpeIo[3];
	// 	unsigned short *usInF = &pDoc->m_pMpeIoF[3];
	unsigned short usIn = 0;
	unsigned short *usInF = NULL;

	if ((usIn & (0x01 << 0)) && !(*usInF & (0x01 << 0)))
	{
		*usInF |= (0x01 << 0);								// �����Ϸ� ���� POS ����Ʈ ����
		pDoc->Status.bSensLmtBufUc[LMT_POS] = TRUE;
	}
	else if (!(usIn & (0x01 << 0)) && (*usInF & (0x01 << 0)))
	{
		*usInF &= ~(0x01 << 0);
		pDoc->Status.bSensLmtBufUc[LMT_POS] = FALSE;
	}

	if ((usIn & (0x01 << 1)) && !(*usInF & (0x01 << 1)))
	{
		*usInF |= (0x01 << 1);								// �����Ϸ� ���� ���� ����
															// Late....
	}
	else if (!(usIn & (0x01 << 1)) && (*usInF & (0x01 << 1)))
	{
		*usInF &= ~(0x01 << 1);
	}

	if ((usIn & (0x01 << 2)) && !(*usInF & (0x01 << 2)))
	{
		*usInF |= (0x01 << 2);								// �����Ϸ� ���� 2/3 ���� ����
															// Late....
	}
	else if (!(usIn & (0x01 << 2)) && (*usInF & (0x01 << 2)))
	{
		*usInF &= ~(0x01 << 2);
	}

	if ((usIn & (0x01 << 3)) && !(*usInF & (0x01 << 3)))
	{
		*usInF |= (0x01 << 3);								// �����Ϸ� ���� 1/3 ���� ����
															// Late....
	}
	else if (!(usIn & (0x01 << 3)) && (*usInF & (0x01 << 3)))
	{
		*usInF &= ~(0x01 << 3);
	}

	if ((usIn & (0x01 << 4)) && !(*usInF & (0x01 << 4)))
	{
		*usInF |= (0x01 << 4);								// �����Ϸ� ���� ���� ����
															// Late....
	}
	else if (!(usIn & (0x01 << 4)) && (*usInF & (0x01 << 4)))
	{
		*usInF &= ~(0x01 << 4);
	}

	if ((usIn & (0x01 << 5)) && !(*usInF & (0x01 << 5)))
	{
		*usInF |= (0x01 << 5);								// �����Ϸ� ���� NEG ����Ʈ ����
		pDoc->Status.bSensLmtBufUc[LMT_NEG] = TRUE;
	}
	else if (!(usIn & (0x01 << 5)) && (*usInF & (0x01 << 5)))
	{
		*usInF &= ~(0x01 << 5);
		pDoc->Status.bSensLmtBufUc[LMT_NEG] = FALSE;
	}

	if ((usIn & (0x01 << 6)) && !(*usInF & (0x01 << 6)))
	{
		*usInF |= (0x01 << 6);								// �����Ϸ� ��ǰô ���� ����ġ
															// Late....
	}
	else if (!(usIn & (0x01 << 6)) && (*usInF & (0x01 << 6)))
	{
		*usInF &= ~(0x01 << 6);
	}

	if ((usIn & (0x01 << 7)) && !(*usInF & (0x01 << 7)))
	{
		*usInF |= (0x01 << 7);								// �����Ϸ� ���� ���� 
															// Late....
	}
	else if (!(usIn & (0x01 << 7)) && (*usInF & (0x01 << 7)))
	{
		*usInF &= ~(0x01 << 7);
	}

	if ((usIn & (0x01 << 8)) && !(*usInF & (0x01 << 8)))
	{
		*usInF |= (0x01 << 8);								// �����Ϸ� ��ǰ�� ���� ��� ����
															// Late....
	}
	else if (!(usIn & (0x01 << 8)) && (*usInF & (0x01 << 8)))
	{
		*usInF &= ~(0x01 << 8);
	}

	if ((usIn & (0x01 << 9)) && !(*usInF & (0x01 << 9)))
	{
		*usInF |= (0x01 << 9);								// �����Ϸ� ��ǰ�� ���� �ϰ� ����
															// Late....
	}
	else if (!(usIn & (0x01 << 9)) && (*usInF & (0x01 << 9)))
	{
		*usInF &= ~(0x01 << 9);
	}

	if ((usIn & (0x01 << 10)) && !(*usInF & (0x01 << 10)))
	{
		*usInF |= (0x01 << 10);								// �����Ϸ� ��ǰ ������(��) ��� ����
															// Late....
	}
	else if (!(usIn & (0x01 << 10)) && (*usInF & (0x01 << 10)))
	{
		*usInF &= ~(0x01 << 10);
	}

	if ((usIn & (0x01 << 11)) && !(*usInF & (0x01 << 11)))
	{
		*usInF |= (0x01 << 11);								// �����Ϸ� ��ǰ ������(��) �ϰ� ����
															// Late....
	}
	else if (!(usIn & (0x01 << 11)) && (*usInF & (0x01 << 11)))
	{
		*usInF &= ~(0x01 << 11);
	}

	if ((usIn & (0x01 << 12)) && !(*usInF & (0x01 << 12)))
	{
		*usInF |= (0x01 << 12);								// �����Ϸ� ��ǰ ������(��) ��� ����
															// Late....
	}
	else if (!(usIn & (0x01 << 12)) && (*usInF & (0x01 << 12)))
	{
		*usInF &= ~(0x01 << 12);
	}

	if ((usIn & (0x01 << 13)) && !(*usInF & (0x01 << 13)))
	{
		*usInF |= (0x01 << 13);								// �����Ϸ� ��ǰ ������(��) �ϰ� ����
															// Late....
	}
	else if (!(usIn & (0x01 << 13)) && (*usInF & (0x01 << 13)))
	{
		*usInF &= ~(0x01 << 13);
	}

	if ((usIn & (0x01 << 14)) && !(*usInF & (0x01 << 14)))
	{
		*usInF |= (0x01 << 14);								// SPARE	
															// No Use....
	}
	else if (!(usIn & (0x01 << 14)) && (*usInF & (0x01 << 14)))
	{
		*usInF &= ~(0x01 << 14);
		// No Use....
	}

	if ((usIn & (0x01 << 15)) && !(*usInF & (0x01 << 15)))
	{
		*usInF |= (0x01 << 15);								// �����Ϸ� ��ǰ�� ���� ����ġ ����
															// Late....
	}
	else if (!(usIn & (0x01 << 15)) && (*usInF & (0x01 << 15)))
	{
		*usInF &= ~(0x01 << 15);
	}
#endif
}

void CGvisR2R_PunchView::DoRcBoxSw()
{
#ifdef USE_MPE
	if (!pDoc->m_pMpeIo)// || !pDoc->m_pMpeIoF)
		return;

	// 	unsigned short usIn = pDoc->m_pMpeIo[4];
	// 	unsigned short *usInF = &pDoc->m_pMpeIoF[4];
	unsigned short usIn = 0;
	unsigned short *usInF = NULL;

	// 	if((usIn & (0x01<<0)) && !(*usInF & (0x01<<0)))
	// 	{
	// 		*usInF |= (0x01<<0);								// �����Ϸ� ������� ����ġ
	// 		pDoc->Status.bEmgRc = TRUE;
	// 	}
	// 	else if(!(usIn & (0x01<<0)) && (*usInF & (0x01<<0)))
	// 	{
	// 		*usInF &= ~(0x01<<0);								
	// 		pDoc->Status.bEmgRc = FALSE;
	// 	}

	if ((usIn & (0x01 << 1)) && !(*usInF & (0x01 << 1)))
	{
		*usInF |= (0x01 << 1);								// �����Ϸ� ���� ��/���� ����ġ
															// 		if(m_pDlgMenu03)
															// 			m_pDlgMenu03->SwRcRelation();
	}
	else if (!(usIn & (0x01 << 1)) && (*usInF & (0x01 << 1)))
	{
		*usInF &= ~(0x01 << 1);
	}

	if ((usIn & (0x01 << 2)) && !(*usInF & (0x01 << 2)))
	{
		*usInF |= (0x01 << 2);								// �����Ϸ� ���� ���/�ϰ� ����ġ
															// 		if(m_pDlgMenu03)
															// 			m_pDlgMenu03->SwRcDcRlUpDn();
	}
	else if (!(usIn & (0x01 << 2)) && (*usInF & (0x01 << 2)))
	{
		*usInF &= ~(0x01 << 2);
	}

	if ((usIn & (0x01 << 3)) && !(*usInF & (0x01 << 3)))
	{
		*usInF |= (0x01 << 3);								// SPARE
															// No Use....
	}
	else if (!(usIn & (0x01 << 3)) && (*usInF & (0x01 << 3)))
	{
		*usInF &= ~(0x01 << 3);
		// No Use....
	}

	if ((usIn & (0x01 << 4)) && !(*usInF & (0x01 << 4)))
	{
		*usInF |= (0x01 << 4);								// SPARE
															// No Use....
	}
	else if (!(usIn & (0x01 << 4)) && (*usInF & (0x01 << 4)))
	{
		*usInF &= ~(0x01 << 4);
		// No Use....
	}

	if ((usIn & (0x01 << 5)) && !(*usInF & (0x01 << 5)))
	{
		*usInF |= (0x01 << 5);								// �����Ϸ� ��ǰ ������(��) ����ġ
															// 		if(m_pDlgMenu03)
															// 			m_pDlgMenu03->SwRcReelJoinL();
	}
	else if (!(usIn & (0x01 << 5)) && (*usInF & (0x01 << 5)))
	{
		*usInF &= ~(0x01 << 5);
	}

	if ((usIn & (0x01 << 6)) && !(*usInF & (0x01 << 6)))
	{
		*usInF |= (0x01 << 6);								// �����Ϸ� ��ǰ ������(��) ����ġ
															// 		if(m_pDlgMenu03)
															// 			m_pDlgMenu03->SwRcReelJoinR();
	}
	else if (!(usIn & (0x01 << 6)) && (*usInF & (0x01 << 6)))
	{
		*usInF &= ~(0x01 << 6);
	}

	if ((usIn & (0x01 << 7)) && !(*usInF & (0x01 << 7)))
	{
		*usInF |= (0x01 << 7);								// �����Ϸ� ��ǰ�� ���� ����ġ
															// 		if(m_pDlgMenu03)
															// 			m_pDlgMenu03->SwRcReelWheel();
	}
	else if (!(usIn & (0x01 << 7)) && (*usInF & (0x01 << 7)))
	{
		*usInF &= ~(0x01 << 7);
	}

	if ((usIn & (0x01 << 8)) && !(*usInF & (0x01 << 8)))
	{
		*usInF |= (0x01 << 8);								// �����Ϸ� ����ô Ŭ���� ����ġ
															// 		if(m_pDlgMenu03)
															// 			m_pDlgMenu03->SwRcPprChuck();
	}
	else if (!(usIn & (0x01 << 8)) && (*usInF & (0x01 << 8)))
	{
		*usInF &= ~(0x01 << 8);
	}

	if ((usIn & (0x01 << 9)) && !(*usInF & (0x01 << 9)))
	{
		*usInF |= (0x01 << 9);								// �����Ϸ� ������ ��ȸ�� ����ġ
															// 		if(m_pDlgMenu03)
															// 			m_pDlgMenu03->SwMyBtnDown(IDC_CHK_45);
	}
	else if (!(usIn & (0x01 << 9)) && (*usInF & (0x01 << 9)))
	{
		*usInF &= ~(0x01 << 9);
		// 		if(m_pDlgMenu03)
		// 			m_pDlgMenu03->SwMyBtnUp(IDC_CHK_45);
	}

	if ((usIn & (0x01 << 10)) && !(*usInF & (0x01 << 10)))
	{
		*usInF |= (0x01 << 10);								// �����Ϸ� ������ ��ȸ�� ����ġ
															// 		if(m_pDlgMenu03)
															// 			m_pDlgMenu03->SwMyBtnDown(IDC_CHK_46);
	}
	else if (!(usIn & (0x01 << 10)) && (*usInF & (0x01 << 10)))
	{
		*usInF &= ~(0x01 << 10);
		// 		if(m_pDlgMenu03)
		// 			m_pDlgMenu03->SwMyBtnUp(IDC_CHK_46);
	}

	if ((usIn & (0x01 << 11)) && !(*usInF & (0x01 << 11)))
	{
		*usInF |= (0x01 << 11);								// �����Ϸ� ��ǰô Ŭ���� ����ġ
															// 		if(m_pDlgMenu03)
															// 			m_pDlgMenu03->SwRcReelChuck();
	}
	else if (!(usIn & (0x01 << 11)) && (*usInF & (0x01 << 11)))
	{
		*usInF &= ~(0x01 << 11);
	}

	if ((usIn & (0x01 << 12)) && !(*usInF & (0x01 << 12)))
	{
		*usInF |= (0x01 << 12);								// �����Ϸ� ��ǰ�� ��ȸ�� ����ġ
															// 		if(m_pDlgMenu03)
															// 			m_pDlgMenu03->SwMyBtnDown(IDC_CHK_5);
	}
	else if (!(usIn & (0x01 << 12)) && (*usInF & (0x01 << 12)))
	{
		*usInF &= ~(0x01 << 12);
		// 		if(m_pDlgMenu03)
		// 			m_pDlgMenu03->SwMyBtnUp(IDC_CHK_5);
	}

	if ((usIn & (0x01 << 13)) && !(*usInF & (0x01 << 13)))
	{
		*usInF |= (0x01 << 13);								// �����Ϸ� ��ǰ�� ��ȸ�� ����ġ
															// 		if(m_pDlgMenu03)
															// 			m_pDlgMenu03->SwMyBtnDown(IDC_CHK_6);
	}
	else if (!(usIn & (0x01 << 13)) && (*usInF & (0x01 << 13)))
	{
		*usInF &= ~(0x01 << 13);
		// 		if(m_pDlgMenu03)
		// 			m_pDlgMenu03->SwMyBtnUp(IDC_CHK_6);
	}

	// 	if((usIn & (0x01<<14)) && !(*usInF & (0x01<<14)))
	// 	{
	// 		*usInF |= (0x01<<14);								// �����Ϸ� ��ǰ EPC���� ����ġ
	// 		m_bSwStopNow = TRUE;
	// 		if(m_pDlgMenu03)
	// 			m_pDlgMenu03->SwStop();
	// 	}
	// 	else if(!(usIn & (0x01<<14)) && (*usInF & (0x01<<14)))
	// 	{
	// 		*usInF &= ~(0x01<<14);								
	// 	}

	if ((usIn & (0x01 << 15)) && !(*usInF & (0x01 << 15)))
	{
		*usInF |= (0x01 << 15);								// SPARE
															// No Use....
	}
	else if (!(usIn & (0x01 << 15)) && (*usInF & (0x01 << 15)))
	{
		*usInF &= ~(0x01 << 15);
		// No Use....
	}
#endif
}

void CGvisR2R_PunchView::DoRcSens1()
{
#ifdef USE_MPE
	if (!pDoc->m_pMpeIo)// || !pDoc->m_pMpeIoF)
		return;

	// 	unsigned short usIn = pDoc->m_pMpeIo[5];
	// 	unsigned short *usInF = &pDoc->m_pMpeIoF[5];
	unsigned short usIn = 0;
	unsigned short *usInF = NULL;

	if ((usIn & (0x01 << 0)) && !(*usInF & (0x01 << 0)))
	{
		*usInF |= (0x01 << 0);								// �����Ϸ� ������������ 1
															// Late....
	}
	else if (!(usIn & (0x01 << 0)) && (*usInF & (0x01 << 0)))
	{
		*usInF &= ~(0x01 << 0);
	}

	if ((usIn & (0x01 << 1)) && !(*usInF & (0x01 << 1)))
	{
		*usInF |= (0x01 << 1);								// �����Ϸ� ������������ 2
															// Late....
	}
	else if (!(usIn & (0x01 << 1)) && (*usInF & (0x01 << 1)))
	{
		*usInF &= ~(0x01 << 1);
	}

	if ((usIn & (0x01 << 2)) && !(*usInF & (0x01 << 2)))
	{
		*usInF |= (0x01 << 2);								// �����Ϸ� ������������ 3
															// Late....
	}
	else if (!(usIn & (0x01 << 2)) && (*usInF & (0x01 << 2)))
	{
		*usInF &= ~(0x01 << 2);
	}

	if ((usIn & (0x01 << 3)) && !(*usInF & (0x01 << 3)))
	{
		*usInF |= (0x01 << 3);								// �����Ϸ� ������������ 4
															// Late....
	}
	else if (!(usIn & (0x01 << 3)) && (*usInF & (0x01 << 3)))
	{
		*usInF &= ~(0x01 << 3);
	}

	if ((usIn & (0x01 << 4)) && !(*usInF & (0x01 << 4)))
	{
		*usInF |= (0x01 << 4);								// �����Ϸ� ��ǰ�������� 1
															// Late....
	}
	else if (!(usIn & (0x01 << 4)) && (*usInF & (0x01 << 4)))
	{
		*usInF &= ~(0x01 << 4);
	}

	if ((usIn & (0x01 << 5)) && !(*usInF & (0x01 << 5)))
	{
		*usInF |= (0x01 << 5);								// �����Ϸ� ��ǰ�������� 2
															// Late....
	}
	else if (!(usIn & (0x01 << 5)) && (*usInF & (0x01 << 5)))
	{
		*usInF &= ~(0x01 << 5);
	}

	if ((usIn & (0x01 << 6)) && !(*usInF & (0x01 << 6)))
	{
		*usInF |= (0x01 << 6);								// �����Ϸ� ��ǰ�������� 3
															// Late....
	}
	else if (!(usIn & (0x01 << 6)) && (*usInF & (0x01 << 6)))
	{
		*usInF &= ~(0x01 << 6);
	}

	if ((usIn & (0x01 << 7)) && !(*usInF & (0x01 << 7)))
	{
		*usInF |= (0x01 << 7);								// �����Ϸ� ��ǰ�������� 4
															// Late....
	}
	else if (!(usIn & (0x01 << 7)) && (*usInF & (0x01 << 7)))
	{
		*usInF &= ~(0x01 << 7);
	}

	if ((usIn & (0x01 << 8)) && !(*usInF & (0x01 << 8)))
	{
		*usInF |= (0x01 << 8);								// �����Ϸ� ��ǰ EPC POS ����Ʈ ����
															// Late....
	}
	else if (!(usIn & (0x01 << 8)) && (*usInF & (0x01 << 8)))
	{
		*usInF &= ~(0x01 << 8);
	}

	if ((usIn & (0x01 << 9)) && !(*usInF & (0x01 << 9)))
	{
		*usInF |= (0x01 << 9);								// �����Ϸ� ��ǰ EPC NEG ����Ʈ ����
															// Late....
	}
	else if (!(usIn & (0x01 << 9)) && (*usInF & (0x01 << 9)))
	{
		*usInF &= ~(0x01 << 9);
	}

	if ((usIn & (0x01 << 10)) && !(*usInF & (0x01 << 10)))
	{
		*usInF |= (0x01 << 10);								// �����Ϸ� ��ǰ EPC ���� ����
															// Late....
	}
	else if (!(usIn & (0x01 << 10)) && (*usInF & (0x01 << 10)))
	{
		*usInF &= ~(0x01 << 10);
	}

	if ((usIn & (0x01 << 11)) && !(*usInF & (0x01 << 11)))
	{
		*usInF |= (0x01 << 11);								// �����Ϸ� ���� ���� ����(��)
		pDoc->Status.bDoorRe[DOOR_FL_RC] = TRUE;
	}
	else if (!(usIn & (0x01 << 11)) && (*usInF & (0x01 << 11)))
	{
		*usInF &= ~(0x01 << 11);
		pDoc->Status.bDoorRe[DOOR_FL_RC] = FALSE;
	}

	if ((usIn & (0x01 << 12)) && !(*usInF & (0x01 << 12)))
	{
		*usInF |= (0x01 << 12);								// �����Ϸ� ���� ���� ����(��)
		pDoc->Status.bDoorRe[DOOR_FR_RC] = TRUE;
	}
	else if (!(usIn & (0x01 << 12)) && (*usInF & (0x01 << 12)))
	{
		*usInF &= ~(0x01 << 12);
		pDoc->Status.bDoorRe[DOOR_FR_RC] = FALSE;
	}

	if ((usIn & (0x01 << 13)) && !(*usInF & (0x01 << 13)))
	{
		*usInF |= (0x01 << 13);								// �����Ϸ� ���� ���� ����
		pDoc->Status.bDoorRe[DOOR_S_RC] = TRUE;
	}
	else if (!(usIn & (0x01 << 13)) && (*usInF & (0x01 << 13)))
	{
		*usInF &= ~(0x01 << 13);
		pDoc->Status.bDoorRe[DOOR_S_RC] = FALSE;
	}

	if ((usIn & (0x01 << 14)) && !(*usInF & (0x01 << 14)))
	{
		*usInF |= (0x01 << 14);								// �����Ϸ� �ĸ� ���� ����(��)
		pDoc->Status.bDoorRe[DOOR_BL_RC] = TRUE;
	}
	else if (!(usIn & (0x01 << 14)) && (*usInF & (0x01 << 14)))
	{
		*usInF &= ~(0x01 << 14);
		pDoc->Status.bDoorRe[DOOR_BL_RC] = FALSE;
	}

	if ((usIn & (0x01 << 15)) && !(*usInF & (0x01 << 15)))
	{
		*usInF |= (0x01 << 15);								// �����Ϸ� �ĸ� ���� ����(��)
		pDoc->Status.bDoorRe[DOOR_BR_RC] = TRUE;
	}
	else if (!(usIn & (0x01 << 15)) && (*usInF & (0x01 << 15)))
	{
		*usInF &= ~(0x01 << 15);
		pDoc->Status.bDoorRe[DOOR_BR_RC] = FALSE;
	}
#endif
}

void CGvisR2R_PunchView::DoRcSens2()
{
#ifdef USE_MPE
	if (!pDoc->m_pMpeIo)// || !pDoc->m_pMpeIoF)
		return;

	// 	unsigned short usIn = pDoc->m_pMpeIo[6];
	// 	unsigned short *usInF = &pDoc->m_pMpeIoF[6];
	unsigned short usIn = 0;
	unsigned short *usInF = NULL;

	if ((usIn & (0x01 << 0)) && !(*usInF & (0x01 << 0)))
	{
		*usInF |= (0x01 << 0);								// �����Ϸ� �����ټ� POS ����Ʈ ����
															// Late....
	}
	else if (!(usIn & (0x01 << 0)) && (*usInF & (0x01 << 0)))
	{
		*usInF &= ~(0x01 << 0);
	}

	if ((usIn & (0x01 << 1)) && !(*usInF & (0x01 << 1)))
	{
		*usInF |= (0x01 << 1);								// �����Ϸ� �����ټ� ���� ����
															// Late....
	}
	else if (!(usIn & (0x01 << 1)) && (*usInF & (0x01 << 1)))
	{
		*usInF &= ~(0x01 << 1);
	}

	if ((usIn & (0x01 << 2)) && !(*usInF & (0x01 << 2)))
	{
		*usInF |= (0x01 << 2);								// �����Ϸ� �����ټ� 2/3 ���� ����
															// Late....
	}
	else if (!(usIn & (0x01 << 2)) && (*usInF & (0x01 << 2)))
	{
		*usInF &= ~(0x01 << 2);
	}

	if ((usIn & (0x01 << 3)) && !(*usInF & (0x01 << 3)))
	{
		*usInF |= (0x01 << 3);								// �����Ϸ� �����ټ� 1/3 ���� ����
															// Late....
	}
	else if (!(usIn & (0x01 << 3)) && (*usInF & (0x01 << 3)))
	{
		*usInF &= ~(0x01 << 3);
	}

	if ((usIn & (0x01 << 4)) && !(*usInF & (0x01 << 4)))
	{
		*usInF |= (0x01 << 4);								// �����Ϸ� �����ټ� ���� ����
															// Late....
	}
	else if (!(usIn & (0x01 << 4)) && (*usInF & (0x01 << 4)))
	{
		*usInF &= ~(0x01 << 4);
	}

	if ((usIn & (0x01 << 5)) && !(*usInF & (0x01 << 5)))
	{
		*usInF |= (0x01 << 5);								// �����Ϸ� �����ټ� NEG ����Ʈ ����
															// Late....
	}
	else if (!(usIn & (0x01 << 5)) && (*usInF & (0x01 << 5)))
	{
		*usInF &= ~(0x01 << 5);
	}

	if ((usIn & (0x01 << 6)) && !(*usInF & (0x01 << 6)))
	{
		*usInF |= (0x01 << 6);								// �����Ϸ� ����ô ���� ����ġ
															// Late....
	}
	else if (!(usIn & (0x01 << 6)) && (*usInF & (0x01 << 6)))
	{
		*usInF &= ~(0x01 << 6);
	}

	if ((usIn & (0x01 << 7)) && !(*usInF & (0x01 << 7)))
	{
		*usInF |= (0x01 << 7);								// �����Ϸ� �������� �з� ����ġ
															// Late....
	}
	else if (!(usIn & (0x01 << 7)) && (*usInF & (0x01 << 7)))
	{
		*usInF &= ~(0x01 << 7);
	}

	if ((usIn & (0x01 << 8)) && !(*usInF & (0x01 << 8)))
	{
		*usInF |= (0x01 << 8);								// SPARE
															// No Use....
	}
	else if (!(usIn & (0x01 << 8)) && (*usInF & (0x01 << 8)))
	{
		*usInF &= ~(0x01 << 8);
		// No Use....
	}

	if ((usIn & (0x01 << 9)) && !(*usInF & (0x01 << 9)))
	{
		*usInF |= (0x01 << 9);								// SPARE
															// No Use....
	}
	else if (!(usIn & (0x01 << 9)) && (*usInF & (0x01 << 9)))
	{
		*usInF &= ~(0x01 << 9);
		// No Use....
	}

	if ((usIn & (0x01 << 10)) && !(*usInF & (0x01 << 10)))
	{
		*usInF |= (0x01 << 10);								// SPARE
															// No Use....
	}
	else if (!(usIn & (0x01 << 10)) && (*usInF & (0x01 << 10)))
	{
		*usInF &= ~(0x01 << 10);
		// No Use....
	}

	if ((usIn & (0x01 << 11)) && !(*usInF & (0x01 << 11)))
	{
		*usInF |= (0x01 << 11);								// SPARE
															// No Use....
	}
	else if (!(usIn & (0x01 << 11)) && (*usInF & (0x01 << 11)))
	{
		*usInF &= ~(0x01 << 11);
		// No Use....
	}

	if ((usIn & (0x01 << 12)) && !(*usInF & (0x01 << 12)))
	{
		*usInF |= (0x01 << 12);								// SPARE
															// No Use....
	}
	else if (!(usIn & (0x01 << 12)) && (*usInF & (0x01 << 12)))
	{
		*usInF &= ~(0x01 << 12);
		// No Use....
	}

	if ((usIn & (0x01 << 13)) && !(*usInF & (0x01 << 13)))
	{
		*usInF |= (0x01 << 13);								// SPARE
															// No Use....
	}
	else if (!(usIn & (0x01 << 13)) && (*usInF & (0x01 << 13)))
	{
		*usInF &= ~(0x01 << 13);
		// No Use....
	}

	if ((usIn & (0x01 << 14)) && !(*usInF & (0x01 << 14)))
	{
		*usInF |= (0x01 << 14);								// �����Ϸ� ���� ��� ����
															// Late....
	}
	else if (!(usIn & (0x01 << 14)) && (*usInF & (0x01 << 14)))
	{
		*usInF &= ~(0x01 << 14);
	}

	if ((usIn & (0x01 << 15)) && !(*usInF & (0x01 << 15)))
	{
		*usInF |= (0x01 << 15);								// �����Ϸ� ���� �ϰ� ����
															// Late....
	}
	else if (!(usIn & (0x01 << 15)) && (*usInF & (0x01 << 15)))
	{
		*usInF &= ~(0x01 << 15);
	}
#endif
}

void CGvisR2R_PunchView::DoRcSens3()
{
#ifdef USE_MPE
	if (!pDoc->m_pMpeIo)// || !pDoc->m_pMpeIoF)
		return;

	// 	unsigned short usIn = pDoc->m_pMpeIo[7];
	// 	unsigned short *usInF = &pDoc->m_pMpeIoF[7];
	unsigned short usIn = 0;
	unsigned short *usInF = NULL;

	if ((usIn & (0x01 << 0)) && !(*usInF & (0x01 << 0)))
	{
		*usInF |= (0x01 << 0);								// �����Ϸ� ���� POS ����Ʈ ����
		pDoc->Status.bSensLmtBufRc[LMT_POS] = TRUE;
	}
	else if (!(usIn & (0x01 << 0)) && (*usInF & (0x01 << 0)))
	{
		*usInF &= ~(0x01 << 0);
		pDoc->Status.bSensLmtBufRc[LMT_POS] = FALSE;
	}

	if ((usIn & (0x01 << 1)) && !(*usInF & (0x01 << 1)))
	{
		*usInF |= (0x01 << 1);								// �����Ϸ� ���� ���� ����
															// Late....
	}
	else if (!(usIn & (0x01 << 1)) && (*usInF & (0x01 << 1)))
	{
		*usInF &= ~(0x01 << 1);
	}

	if ((usIn & (0x01 << 2)) && !(*usInF & (0x01 << 2)))
	{
		*usInF |= (0x01 << 2);								// �����Ϸ� ���� 2/3 ���� ����
															// Late....
	}
	else if (!(usIn & (0x01 << 2)) && (*usInF & (0x01 << 2)))
	{
		*usInF &= ~(0x01 << 2);
	}

	if ((usIn & (0x01 << 3)) && !(*usInF & (0x01 << 3)))
	{
		*usInF |= (0x01 << 3);								// �����Ϸ� ���� 1/3 ���� ����
															// Late....
	}
	else if (!(usIn & (0x01 << 3)) && (*usInF & (0x01 << 3)))
	{
		*usInF &= ~(0x01 << 3);
	}

	if ((usIn & (0x01 << 4)) && !(*usInF & (0x01 << 4)))
	{
		*usInF |= (0x01 << 4);								// �����Ϸ� ���� ���� ����
															// Late....
	}
	else if (!(usIn & (0x01 << 4)) && (*usInF & (0x01 << 4)))
	{
		*usInF &= ~(0x01 << 4);
	}

	if ((usIn & (0x01 << 5)) && !(*usInF & (0x01 << 5)))
	{
		*usInF |= (0x01 << 5);								// �����Ϸ� ���� NEG ����Ʈ ����
		pDoc->Status.bSensLmtBufRc[LMT_NEG] = TRUE;
	}
	else if (!(usIn & (0x01 << 5)) && (*usInF & (0x01 << 5)))
	{
		*usInF &= ~(0x01 << 5);
		pDoc->Status.bSensLmtBufRc[LMT_NEG] = FALSE;
	}

	if ((usIn & (0x01 << 6)) && !(*usInF & (0x01 << 6)))
	{
		*usInF |= (0x01 << 6);								// �����Ϸ� ��ǰô ���� ����ġ
															// Late....
	}
	else if (!(usIn & (0x01 << 6)) && (*usInF & (0x01 << 6)))
	{
		*usInF &= ~(0x01 << 6);
	}

	if ((usIn & (0x01 << 7)) && !(*usInF & (0x01 << 7)))
	{
		*usInF |= (0x01 << 7);								// �����Ϸ� ���� ���� 
															// Late....
	}
	else if (!(usIn & (0x01 << 7)) && (*usInF & (0x01 << 7)))
	{
		*usInF &= ~(0x01 << 7);
	}

	if ((usIn & (0x01 << 8)) && !(*usInF & (0x01 << 8)))
	{
		*usInF |= (0x01 << 8);								// �����Ϸ� ��ǰ�� ���� ��� ����
															// Late....
	}
	else if (!(usIn & (0x01 << 8)) && (*usInF & (0x01 << 8)))
	{
		*usInF &= ~(0x01 << 8);
	}

	if ((usIn & (0x01 << 9)) && !(*usInF & (0x01 << 9)))
	{
		*usInF |= (0x01 << 9);								// �����Ϸ� ��ǰ�� ���� �ϰ� ����
															// Late....
	}
	else if (!(usIn & (0x01 << 9)) && (*usInF & (0x01 << 9)))
	{
		*usInF &= ~(0x01 << 9);
	}

	if ((usIn & (0x01 << 10)) && !(*usInF & (0x01 << 10)))
	{
		*usInF |= (0x01 << 10);								// �����Ϸ� ��ǰ ������(��) ��� ����
															// Late....
	}
	else if (!(usIn & (0x01 << 10)) && (*usInF & (0x01 << 10)))
	{
		*usInF &= ~(0x01 << 10);
	}

	if ((usIn & (0x01 << 11)) && !(*usInF & (0x01 << 11)))
	{
		*usInF |= (0x01 << 11);								// �����Ϸ� ��ǰ ������(��) �ϰ� ����
															// Late....
	}
	else if (!(usIn & (0x01 << 11)) && (*usInF & (0x01 << 11)))
	{
		*usInF &= ~(0x01 << 11);
	}

	if ((usIn & (0x01 << 12)) && !(*usInF & (0x01 << 12)))
	{
		*usInF |= (0x01 << 12);								// �����Ϸ� ��ǰ ������(��) ��� ����
															// Late....
	}
	else if (!(usIn & (0x01 << 12)) && (*usInF & (0x01 << 12)))
	{
		*usInF &= ~(0x01 << 12);
	}

	if ((usIn & (0x01 << 13)) && !(*usInF & (0x01 << 13)))
	{
		*usInF |= (0x01 << 13);								// �����Ϸ� ��ǰ ������(��) �ϰ� ����
															// Late....
	}
	else if (!(usIn & (0x01 << 13)) && (*usInF & (0x01 << 13)))
	{
		*usInF &= ~(0x01 << 13);
	}

	if ((usIn & (0x01 << 14)) && !(*usInF & (0x01 << 14)))
	{
		*usInF |= (0x01 << 14);								// �����Ϸ� ��ǰ �ܷ� ���� ����
															// Late....
	}
	else if (!(usIn & (0x01 << 14)) && (*usInF & (0x01 << 14)))
	{
		*usInF &= ~(0x01 << 14);
	}

	if ((usIn & (0x01 << 15)) && !(*usInF & (0x01 << 15)))
	{
		*usInF |= (0x01 << 15);								// �����Ϸ� ��ǰ�� ���� ����ġ ����
															// Late....
	}
	else if (!(usIn & (0x01 << 15)) && (*usInF & (0x01 << 15)))
	{
		*usInF &= ~(0x01 << 15);
	}
#endif
}

void CGvisR2R_PunchView::SetAoiFdPitch(double dPitch)
{
	pDoc->SetAoiFdPitch(dPitch);
	// 	m_pMotion->SetLeadPitch(AXIS_AOIFD, dPitch);
//	long lData = long(dPitch*1000.0);
//#ifdef USE_MPE
//	pView->m_pMpe->Write(_T("ML45012"), lData); // �˻�� Feeding �ѷ� Lead Pitch
//	pView->m_pMpe->Write(_T("ML45020"), lData); // ���κ� Feeding �ѷ� Lead Pitch
//#endif

//#ifdef USE_ENGRAVE
//	if (pView && pView->m_pEngrave)
//		pView->m_pEngrave->SetAoiLeadPitch();	//_ItemInx::_AoiLeadPitch
//#endif
}

void CGvisR2R_PunchView::SetMkFdPitch(double dPitch)
{
	pDoc->SetMkFdPitch(dPitch);
	// 	m_pMotion->SetLeadPitch(AXIS_MKFD, dPitch);
	long lData = long(dPitch*1000.0);
#ifdef USE_MPE
	pView->m_pMpe->Write(_T("ML45014"), lData); // ��ŷ�� Feeding �ѷ� Lead Pitch
#endif

//#ifdef USE_ENGRAVE
//	if (pView && pView->m_pEngrave)
//		pView->m_pEngrave->SetMkLeadPitch();	//_ItemInx::_MkLeadPitch
//#endif
}

void CGvisR2R_PunchView::SetBufInitPos(double dPos)
{
	// 	if(m_pMotion)
	// 		m_pMotion->SetBufInitPos(dPos);
	//	long lData = long(dPos*1000.0);
	//	pView->m_pMpe->Write(_T("ML00000"), lData); // ��ŷ�� ���� �ʱ� ��ġ

	pDoc->SetBufInitPos(dPos);
}

void CGvisR2R_PunchView::SetEngBufInitPos(double dPos)
{
	// 	if(m_pMotion)
	// 		m_pMotion->SetBufInitPos(dPos);
	//	long lData = long(dPos*1000.0);
	//	pView->m_pMpe->Write(_T("ML00000"), lData); // ��ŷ�� ���� �ʱ� ��ġ

	pDoc->SetEngBufInitPos(dPos);
}

void CGvisR2R_PunchView::SetBufHomeParam(double dVel, double dAcc)
{
	long lVel = long(dVel*1000.0);
	long lAcc = long(dAcc*1000.0);
	//	pView->m_pMpe->Write(_T("ML00000"), lVel); // ��ŷ�� ���� Ȩ �ӵ�
	//	pView->m_pMpe->Write(_T("ML00000"), lAcc); // ��ŷ�� ���� Ȩ ���ӵ�
	//	pDoc->SetBufInitPos(dVel, dAcc);
}

LRESULT CGvisR2R_PunchView::OnBufThreadDone(WPARAM wPara, LPARAM lPara)
{
	// 	if(m_pMotion)
	// 	{
	// 		m_pMotion->SetOriginPos(AXIS_AOIFD);
	// 		m_pMotion->SetOriginPos(AXIS_BUF);
	// 		m_pMotion->SetOriginPos(AXIS_RENC);
	// 	}
	// 
	// 	if(m_pVision[0])
	// 		m_pVision[0]->SetClrOverlay();
	// 
	// 	if(m_pVision[1])
	// 		m_pVision[1]->SetClrOverlay();

	return 0L;
}

//.........................................................................................

BOOL CGvisR2R_PunchView::WatiDispMain(int nDelay)
{
	if (m_nWatiDispMain % nDelay)
	{
		m_nWatiDispMain++;
		return TRUE;
	}

	m_nWatiDispMain = 0;
	m_nWatiDispMain++;
	return FALSE;
}

void CGvisR2R_PunchView::DispMain(CString sMsg, COLORREF rgb)
{
	pDoc->SetMonDispMain(sMsg);

	m_csDispMain.Lock();
	m_bDispMain = FALSE;
	stDispMain stData(sMsg, rgb);
	m_ArrayDispMain.Add(stData);
	m_bDispMain = TRUE;

	if (sMsg == _T("�� ��"))
	{
		//m_bStopF_Verify = TRUE;
		pDoc->SetMkMenu03(_T("Main"), _T("Stop"), TRUE);
		pDoc->SetMkMenu03(_T("Main"), _T("Run"), FALSE);
	}
	else
	{
		pDoc->SetMkMenu03(_T("Main"), _T("Run"), TRUE);
		pDoc->SetMkMenu03(_T("Main"), _T("Stop"), FALSE);
	}

	sMsg.Empty();
	m_csDispMain.Unlock();
}

int CGvisR2R_PunchView::DoDispMain()
{
	int nRtn = -1;

	if (!m_bDispMain)
		return nRtn;

	int nCount = m_ArrayDispMain.GetSize();
	if (nCount>0)
	{
		stDispMain stDispMsg;

		m_csDispMain.Lock();
		stDispMsg = m_ArrayDispMain.GetAt(0);
		m_ArrayDispMain.RemoveAt(0);
		m_csDispMain.Unlock();

		if (m_pDlgMenu01)
		{
			CString sMsg = stDispMsg.strMsg;
			COLORREF rgb = stDispMsg.rgb;
			m_sDispMain = sMsg;
			m_pDlgMenu01->DispMain(sMsg, rgb);
			return 0;
		}
	}

	return nRtn;
}

// CString CGvisR2R_PunchView::GetDispMain()
// {
// // 	m_cs.Lock();
// // 	CString sRtn;
// // 	sRtn = m_sDispMain;
// // 	m_cs.Unlock();
// // 	return sRtn;
// 	return m_sDispMain;
// }

BOOL CGvisR2R_PunchView::IsReady()
{
	if (m_pDlgMenu03)
		return m_pDlgMenu03->GetReady();

	return FALSE;
}

BOOL CGvisR2R_PunchView::IsAuto()
{
	if (pDoc->Status.bAuto)
		return TRUE;
	return FALSE;
}

void CGvisR2R_PunchView::Shift2Buf()	// ���������� ������ �ø���� Share������ �ø����� �������� Ȯ�� �� �ű�.
{
	int nLastListBuf;
	if (m_nShareUpS > 0)
	{
		//nLastListBuf = pDoc->m_ListBuf[0].GetLast();
		//if (nLastListBuf > 0 && m_nShareUpS > 1)
		//{
		//	if (nLastListBuf != m_nShareUpS - 1)
		//	{
		//		Stop();
		//		DispMsg(_T("�ø��� ���Ӽ� ����"), _T("���"), RGB_RED, DELAY_TIME_MSG);
		//	}
		//}

		m_bLoadShare[0] = TRUE;
		pDoc->m_ListBuf[0].Push(m_nShareUpS);

	}

	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;
	if (bDualTest)
	{
		if (m_nShareDnS > 0)
		{
			//nLastListBuf = pDoc->m_ListBuf[1].GetLast();
			//if (nLastListBuf > 0 && m_nShareDnS > 1)
			//{
			//	if(nLastListBuf != m_nShareDnS-1)
			//	{
			//		Stop();
			//		DispMsg(_T("�ø��� ���Ӽ� ����"), _T("���"), RGB_RED, DELAY_TIME_MSG);
			//	}
			//}

			m_bLoadShare[1] = TRUE;
			pDoc->m_ListBuf[1].Push(m_nShareDnS);

			//if(m_bChkLastProcVs)
			//{
				//if(m_nShareDnS == GetLotEndSerial()-3)

				//if (m_nShareDnS == m_nAoiLastSerial[0] - 3 && m_nAoiLastSerial[0] > 0)
				//{
				//	if (IsVsDn())
				//	{
				//		SetDummyDn();
				//		Sleep(30);
				//		SetDummyDn();
				//		Sleep(30);
				//		SetDummyDn();
				//		Sleep(30);
				//	}
				//}
			//}
		}
	}

	pDoc->CopyPcrAll();
	pDoc->DelSharePcr();
}


void CGvisR2R_PunchView::CompletedMk(int nCam) // 0: Only Cam0, 1: Only Cam1, 2: Cam0 and Cam1, 3: None
{
	int nSerial = -1;

	switch (nCam)
	{
	case 0:
		nSerial = m_nBufUpSerial[0];
		break;
	case 1:
		nSerial = m_nBufUpSerial[1];
		break;
	case 2:
		nSerial = m_nBufUpSerial[1];
		break;
	case 3:
		return;
	default:
		return;
	}

	pDoc->SetCompletedSerial(nSerial);
}

//void CGvisR2R_PunchView::Shift2Mk()
//{
//	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;
//	int nSerial = m_nBufUpSerial[0];
//
//	if (bDualTest)
//	{
//		if (m_bLastProc && m_nBufDnSerial[0] == m_nLotEndSerial)
//		{
//			nSerial = m_nBufDnSerial[0];
//			if (nSerial > 0 && (nSerial % 2))
//			{
//				//pDoc->UpdateYield(nSerial);
//				//m_nSerialTHREAD_UPDATAE_YIELD[0] = nSerial;
//				//m_bTHREAD_UPDATAE_YIELD[0] = TRUE;
//
//				pDoc->Shift2Mk(nSerial);	// Cam0
//				if (m_pDlgFrameHigh)
//					m_pDlgFrameHigh->SetMkLastShot(nSerial);
//				pDoc->UpdateYieldOnRmap();
//			}
//		}
//		else
//		{
//			nSerial = m_nBufDnSerial[0];
//			if (!m_bCont)
//			{
//				if (nSerial > 0 && (nSerial % 2)) // First Shot number must be odd.
//				{
//					//pDoc->UpdateYield(nSerial);
//					//m_nSerialTHREAD_UPDATAE_YIELD[0] = nSerial;
//					//m_bTHREAD_UPDATAE_YIELD[0] = TRUE;
//
//					pDoc->Shift2Mk(nSerial);	// Cam0
//
//					//pDoc->UpdateYield(nSerial + 1);
//					//m_nSerialTHREAD_UPDATAE_YIELD[1] = nSerial + 1;
//					//m_bTHREAD_UPDATAE_YIELD[1] = TRUE;
//
//					if (m_nBufDnSerial[1] > 0)
//					{
//					pDoc->Shift2Mk(m_nBufDnSerial[1]);	// Cam1
//					pDoc->UpdateYieldOnRmap();
//					if (m_pDlgFrameHigh)
//						m_pDlgFrameHigh->SetMkLastShot(m_nBufDnSerial[1]);
//				}
//				else
//				{
//						if (m_pDlgFrameHigh)
//							m_pDlgFrameHigh->SetMkLastShot(nSerial);
//					}
//					pDoc->UpdateYieldOnRmap();
//				}
//				else
//				{
//					Stop();
//				}
//			}
//			else
//			{
//				if (nSerial > 0)
//				{
//					//pDoc->UpdateYield(nSerial);
//					//m_nSerialTHREAD_UPDATAE_YIELD[0] = nSerial;
//					//m_bTHREAD_UPDATAE_YIELD[0] = TRUE;
//
//					pDoc->Shift2Mk(nSerial);	// Cam0
//
//					//pDoc->UpdateYield(nSerial + 1);
//					//m_nSerialTHREAD_UPDATAE_YIELD[1] = nSerial + 1;
//					//m_bTHREAD_UPDATAE_YIELD[1] = TRUE;
//
//					if (m_nBufDnSerial[1] > 0)
//					{
//					pDoc->Shift2Mk(m_nBufDnSerial[1]);	// Cam1
//					if (m_pDlgFrameHigh)
//						m_pDlgFrameHigh->SetMkLastShot(m_nBufDnSerial[1]);
//				}
//				else
//				{
//						if (m_pDlgFrameHigh)
//							m_pDlgFrameHigh->SetMkLastShot(nSerial);
//					}
//					pDoc->UpdateYieldOnRmap();
//				}
//				else
//				{
//					Stop();
//				}
//			}
//		}
//	}
//	else
//	{
//		if (m_bLastProc && m_nBufUpSerial[0] == m_nLotEndSerial)
//		{
//			nSerial = m_nBufUpSerial[0];
//			if (!m_bCont)
//			{
//				if (nSerial > 0 && (nSerial % 2)) // First Shot number must be odd.
//				{
//					//pDoc->UpdateYield(nSerial);
//					//m_nSerialTHREAD_UPDATAE_YIELD[0] = nSerial;
//					//m_bTHREAD_UPDATAE_YIELD[0] = TRUE;
//
//					pDoc->Shift2Mk(nSerial);	// Cam0
//					if (m_pDlgFrameHigh)
//						m_pDlgFrameHigh->SetMkLastShot(nSerial);
//					pDoc->UpdateYieldOnRmap();
//				}
//				else
//				{
//					Stop();
//				}
//			}
//			else
//			{
//				if (nSerial > 0)
//				{
//					//pDoc->UpdateYield(nSerial);
//					//m_nSerialTHREAD_UPDATAE_YIELD[0] = nSerial;
//					//m_bTHREAD_UPDATAE_YIELD[0] = TRUE;
//
//					pDoc->Shift2Mk(nSerial);	// Cam0
//					if (m_pDlgFrameHigh)
//						m_pDlgFrameHigh->SetMkLastShot(nSerial);
//					pDoc->UpdateYieldOnRmap();
//				}
//				else
//				{
//					Stop();
//				}
//			}
//		}
//		else
//		{
//			nSerial = m_nBufUpSerial[0];
//			if (!m_bCont)
//			{
//				if (nSerial > 0 && (nSerial % 2)) // First Shot number must be odd.
//				{
//					//pDoc->UpdateYield(nSerial);
//					//m_nSerialTHREAD_UPDATAE_YIELD[0] = nSerial;
//					//m_bTHREAD_UPDATAE_YIELD[0] = TRUE;
//
//					pDoc->Shift2Mk(nSerial);	// Cam0
//
//					//pDoc->UpdateYield(nSerial + 1);
//					//m_nSerialTHREAD_UPDATAE_YIELD[1] = nSerial + 1;
//					//m_bTHREAD_UPDATAE_YIELD[1] = TRUE;
//
//					if (m_nBufUpSerial[1] > 0)
//					{
//					pDoc->Shift2Mk(m_nBufUpSerial[1]);	// Cam1
//					if (m_pDlgFrameHigh)
//						m_pDlgFrameHigh->SetMkLastShot(m_nBufUpSerial[1]);
//				}
//				else
//				{
//						if (m_pDlgFrameHigh)
//							m_pDlgFrameHigh->SetMkLastShot(nSerial);
//					}
//					pDoc->UpdateYieldOnRmap();
//				}
//				else
//				{
//					Stop();
//				}
//			}
//			else
//			{
//				if (nSerial > 0)
//				{
//					//pDoc->UpdateYield(nSerial);
//					//m_nSerialTHREAD_UPDATAE_YIELD[0] = nSerial;
//					//m_bTHREAD_UPDATAE_YIELD[0] = TRUE;
//
//					pDoc->Shift2Mk(nSerial);	// Cam0
//
//					//pDoc->UpdateYield(nSerial + 1);
//					//m_nSerialTHREAD_UPDATAE_YIELD[1] = nSerial + 1;
//					//m_bTHREAD_UPDATAE_YIELD[1] = TRUE;
//
//					if (m_nBufUpSerial[1] > 0)
//					{
//					pDoc->Shift2Mk(m_nBufUpSerial[1]);	// Cam1
//					if (m_pDlgFrameHigh)
//						m_pDlgFrameHigh->SetMkLastShot(m_nBufUpSerial[1]);
//				}
//				else
//				{
//						if (m_pDlgFrameHigh)
//							m_pDlgFrameHigh->SetMkLastShot(nSerial);
//					}
//					pDoc->UpdateYieldOnRmap();
//				}
//				else
//				{
//					Stop();
//				}
//			}
//		}
//	}
//
//	m_bShift2Mk = FALSE;
//}

void CGvisR2R_PunchView::SetTestSts(int nStep)
{
	// 	if(!m_pDlgMenu03)
	// 		return;
	// 
	// 	// �˻�� - TBL�ı� OFF, TBL���� ON, FD/TQ ���� OFF, 
	// 	switch(nStep)
	// 	{
	// 	case 0:
	// 		m_pDlgMenu03->SwAoiDustBlw(FALSE);
	// 		m_pDlgMenu03->SwAoiTblBlw(FALSE);
	// 		m_pDlgMenu03->SwAoiFdVac(FALSE);
	// 		m_pDlgMenu03->SwAoiTqVac(FALSE);
	// 		break;
	// 	case 1:
	// 		m_pDlgMenu03->SwAoiTblVac(TRUE);
	// 		break;
	// 	}
}

void CGvisR2R_PunchView::SetTestSts0(BOOL bOn)
{
#ifdef USE_MPE
	if (bOn)
		pView->m_pMpe->Write(_T("MB003829"), 1);
	//IoWrite(_T("MB003829"), 1); // �˻�� �� �˻� ���̺� ���� SOL <-> Y4369 I/F
	else
		pView->m_pMpe->Write(_T("MB003829"), 0);
	//IoWrite(_T("MB003829"), 0); // �˻�� �� �˻� ���̺� ���� SOL <-> Y4369 I/F
#endif
}

void CGvisR2R_PunchView::SetTestSts1(BOOL bOn)
{
#ifdef USE_MPE
	if (bOn)
		pView->m_pMpe->Write(_T("MB003929"), 1);
	//IoWrite(_T("MB003929"), 1); // �˻�� �� �˻� ���̺� ���� SOL <-> Y4369 I/F
	else
		pView->m_pMpe->Write(_T("MB003929"), 0);
	//IoWrite(_T("MB003929"), 0); // �˻�� �� �˻� ���̺� ���� SOL <-> Y4369 I/F
#endif
}

void CGvisR2R_PunchView::SetAoiStopSts()
{
	if (!m_pDlgMenu03)
		return;

	// �˻�� - FD/TQ ���� ON, TBL���� OFF, TBL�ı� ON,
	m_pDlgMenu03->SwAoiDustBlw(FALSE);
	m_pDlgMenu03->SwAoiTblBlw(FALSE);
	m_pDlgMenu03->SwAoiTblVac(FALSE);
	m_pDlgMenu03->SwAoiFdVac(FALSE);
	// 	m_pDlgMenu03->SwAoiTqVac(FALSE);
}

void CGvisR2R_PunchView::SetAoiFdSts()
{
	if (!m_pDlgMenu03)
		return;

	// �˻�� - FD/TQ ���� ON, TBL���� OFF, TBL�ı� ON, 
	m_pDlgMenu03->SwAoiFdVac(TRUE);
	// 	m_pDlgMenu03->SwAoiTqVac(TRUE);
	m_pDlgMenu03->SwAoiTblVac(FALSE);
	m_pDlgMenu03->SwAoiTblBlw(TRUE);
	m_pDlgMenu03->SwAoiDustBlw(TRUE);
}

void CGvisR2R_PunchView::SetMkSts(int nStep)
{
	if (!m_pDlgMenu03)
		return;

	// ��ŷ�� - TBL�ı� OFF, TBL���� ON, FD/TQ ���� OFF, 
	switch (nStep)
	{
	case 0:
		m_pDlgMenu03->SwMkTblBlw(FALSE);
		m_pDlgMenu03->SwMkFdVac(FALSE);
		m_pDlgMenu03->SwMkTqVac(FALSE);
		break;
	case 1:
		m_pDlgMenu03->SwMkTblVac(TRUE);
		break;
	}
}

void CGvisR2R_PunchView::SetMkStopSts()
{
	if (!m_pDlgMenu03)
		return;

	// ��ŷ�� - FD/TQ ���� ON, TBL���� OFF, TBL�ı� ON, 
	m_pDlgMenu03->SwMkTblBlw(FALSE);
	m_pDlgMenu03->SwMkTblVac(FALSE);
	m_pDlgMenu03->SwMkFdVac(FALSE);
	// 	m_pDlgMenu03->SwMkTqVac(FALSE);
}

void CGvisR2R_PunchView::SetMkFdSts()
{
	if (!m_pDlgMenu03)
		return;

	// ��ŷ�� - FD/TQ ���� ON, TBL���� OFF, TBL�ı� ON, 
	m_pDlgMenu03->SwMkFdVac(TRUE);
	// 	m_pDlgMenu03->SwMkTqVac(TRUE);
	m_pDlgMenu03->SwMkTblVac(FALSE);
	m_pDlgMenu03->SwMkTblBlw(TRUE);
}

BOOL CGvisR2R_PunchView::IsMkFdSts()
{
	if (!m_pDlgMenu03)
		return FALSE;

	BOOL bOn[4] = { 0 };
	// ��ŷ�� - FD/TQ ���� ON, TBL���� OFF, TBL�ı� ON, 
	bOn[0] = m_pDlgMenu03->IsMkFdVac(); // TRUE
	bOn[1] = m_pDlgMenu03->IsMkTqVac(); // TRUE
	bOn[2] = m_pDlgMenu03->IsMkTblVac(); // FALSE
	bOn[3] = m_pDlgMenu03->IsMkTblBlw(); // TRUE

	if (bOn[0] && bOn[1] && !bOn[2] && bOn[3])
		return TRUE;

	return FALSE;
}

void CGvisR2R_PunchView::SetDelay(int mSec, int nId)
{
	if (nId > 10)
		nId = 9;
	else if (nId < 0)
		nId = 0;

	m_dwSetDlySt[nId] = GetTickCount();
	m_dwSetDlyEd[nId] = m_dwSetDlySt[nId] + mSec;
}

void CGvisR2R_PunchView::SetDelay0(int mSec, int nId)
{
	if (nId > 10)
		nId = 9;
	else if (nId < 0)
		nId = 0;

	m_dwSetDlySt0[nId] = GetTickCount();
	m_dwSetDlyEd0[nId] = m_dwSetDlySt0[nId] + mSec;
}

void CGvisR2R_PunchView::SetDelay1(int mSec, int nId)
{
	if (nId > 10)
		nId = 9;
	else if (nId < 0)
		nId = 0;

	m_dwSetDlySt1[nId] = GetTickCount();
	m_dwSetDlyEd1[nId] = m_dwSetDlySt1[nId] + mSec;
}

BOOL CGvisR2R_PunchView::WaitDelay(int nId) // F:Done, T:On Waiting....
{
	if (nId > 10)
		nId = 9;
	else if (nId < 0)
		nId = 0;

	DWORD dwCur = GetTickCount();
	if (dwCur < m_dwSetDlyEd[nId])
		return TRUE;
	return FALSE;
}

BOOL CGvisR2R_PunchView::WaitDelay0(int nId) // F:Done, T:On Waiting....
{
	if (nId > 10)
		nId = 9;
	else if (nId < 0)
		nId = 0;

	DWORD dwCur = GetTickCount();
	if (dwCur < m_dwSetDlyEd0[nId])
		return TRUE;
	return FALSE;
}

BOOL CGvisR2R_PunchView::WaitDelay1(int nId) // F:Done, T:On Waiting....
{
	if (nId > 10)
		nId = 9;
	else if (nId < 0)
		nId = 0;

	DWORD dwCur = GetTickCount();
	if (dwCur < m_dwSetDlyEd1[nId])
		return TRUE;
	return FALSE;
}

BOOL CGvisR2R_PunchView::GetDelay(int &mSec, int nId) // F:Done, T:On Waiting....
{
	DWORD dwCur = GetTickCount();
	mSec = int(dwCur - m_dwSetDlySt[nId]);
	if (dwCur < m_dwSetDlyEd[nId])
		return TRUE;
	return FALSE;
}

BOOL CGvisR2R_PunchView::GetDelay0(int &mSec, int nId) // F:Done, T:On Waiting....
{
	DWORD dwCur = GetTickCount();
	mSec = int(dwCur - m_dwSetDlySt0[nId]);
	if (dwCur < m_dwSetDlyEd0[nId])
		return TRUE;
	return FALSE;
}

BOOL CGvisR2R_PunchView::GetDelay1(int &mSec, int nId) // F:Done, T:On Waiting....
{
	DWORD dwCur = GetTickCount();
	mSec = int(dwCur - m_dwSetDlySt1[nId]);
	if (dwCur < m_dwSetDlyEd1[nId])
		return TRUE;
	return FALSE;
}

void CGvisR2R_PunchView::SetAoiFd()
{
	CfPoint OfSt;
	if (GetAoiUpOffset(OfSt))
	{
		if (m_pDlgMenu02)
		{
			m_pDlgMenu02->m_dAoiUpFdOffsetX = OfSt.x;
			m_pDlgMenu02->m_dAoiUpFdOffsetY = OfSt.y;
		}
	}

	MoveAoi(-1.0*OfSt.x);
	if (m_pDlgMenu03)
		m_pDlgMenu03->ChkDoneAoi();

	if (!pDoc->WorkingInfo.LastJob.bAoiOnePnl)
	{
#ifdef USE_MPE
		//IoWrite(_T("MB440151"), 1);	// ���ǳ� �̼ۻ��� ON (PC�� ON, OFF)
		pView->m_pMpe->Write(_T("MB440151"), 1);
#endif
		CString sData, sPath = PATH_WORKING_INFO;
		pDoc->WorkingInfo.LastJob.bMkOnePnl = pDoc->WorkingInfo.LastJob.bAoiOnePnl = TRUE;
		sData.Format(_T("%d"), pDoc->WorkingInfo.LastJob.bMkOnePnl ? 1 : 0);
		::WritePrivateProfileString(_T("Last Job"), _T("Marking One Pannel Move On"), sData, sPath);
		::WritePrivateProfileString(_T("Last Job"), _T("AOI One Pannel Move On"), sData, sPath);
	}
}

void CGvisR2R_PunchView::MoveAoi(double dOffset)
{
	//	long lData = (long)0;
	long lData = (long)(dOffset * 1000.0);
	//IoWrite(_T("MB440160"), 1);	// �˻�� �ǵ� ON (PLC�� �ǵ��Ϸ� �� OFF)
	pView->m_pMpe->Write(_T("MB440160"), 1);
	//IoWrite(_T("ML45064"), lData);	// �˻�� Feeding �ѷ� Offset(*1000, +:�� ����, -�� ����)
	pView->m_pMpe->Write(_T("ML45064"), lData);
}

void CGvisR2R_PunchView::SetMkFd()
{
	CfPoint OfSt;
	GetMkOffset(OfSt);

	if (m_nShareDnCnt > 0)
	{
		if (!(m_nShareDnCnt % 2))
			MoveMk(-1.0*OfSt.x);
	}
	else
	{
		if (m_nShareUpCnt > 0)
		{
			if (!(m_nShareUpCnt % 2))
				MoveMk(-1.0*OfSt.x);
		}
	}
	if (m_pDlgMenu03)
		m_pDlgMenu03->ChkDoneMk();

	if (!pDoc->WorkingInfo.LastJob.bAoiOnePnl)
	{
		//IoWrite(_T("MB440151"), 1);	// ���ǳ� �̼ۻ��� ON (PC�� ON, OFF)
		pView->m_pMpe->Write(_T("MB440151"), 1);

		CString sData, sPath = PATH_WORKING_INFO;
		pDoc->WorkingInfo.LastJob.bMkOnePnl = pDoc->WorkingInfo.LastJob.bAoiOnePnl = TRUE;
		sData.Format(_T("%d"), pDoc->WorkingInfo.LastJob.bMkOnePnl ? 1 : 0);
		::WritePrivateProfileString(_T("Last Job"), _T("Marking One Pannel Move On"), sData, sPath);
		::WritePrivateProfileString(_T("Last Job"), _T("AOI One Pannel Move On"), sData, sPath);
	}
}

void CGvisR2R_PunchView::MoveMk(double dOffset)
{
#ifdef USE_MPE
	long lData = (long)(dOffset * 1000.0);
	//IoWrite(_T("MB440161"), 1);		// ��ŷ�� �ǵ� CW ON (PLC�� �ǵ��Ϸ� �� OFF))
	pView->m_pMpe->Write(_T("MB440161"), 1);
	//IoWrite(_T("ML45066"), lData);	// ��ŷ�� Feeding �ѷ� Offset(*1000, +:�� ����, -�� ����)
	pView->m_pMpe->Write(_T("ML45066"), lData);
#endif
}

BOOL CGvisR2R_PunchView::IsMkFd()
{
	if (m_nShareDnCnt > 0)
	{
		if (!(m_nShareDnCnt % 2))
		{
#ifdef USE_MPE
			if (pDoc->m_pMpeSignal[5] & (0x01 << 1))	// ��ŷ�� �ǵ� CW ON (PLC�� �ǵ��Ϸ� �� OFF)
			{
				pDoc->LogAuto(_T("PLC: ��ŷ�� �ǵ� CW ON (PLC�� �ǵ��Ϸ� �� OFF)"));
				return TRUE;
			}
#endif
			return FALSE;
		}
	}
	else
	{
		if (m_nShareUpCnt > 0)
		{
			if (!(m_nShareUpCnt % 2))
			{
#ifdef USE_MPE
				if (pDoc->m_pMpeSignal[5] & (0x01 << 1))	// ��ŷ�� �ǵ� CW ON (PLC�� �ǵ��Ϸ� �� OFF)
				{
					pDoc->LogAuto(_T("PLC: ��ŷ�� �ǵ� CW ON (PLC�� �ǵ��Ϸ� �� OFF)"));
					return TRUE;
				}
#endif
				return FALSE;
			}
		}
	}

	return TRUE;
}

BOOL CGvisR2R_PunchView::IsAoiFd()
{
#ifdef USE_MPE
	if (pDoc->m_pMpeSignal[5] & (0x01 >> 0))	// �˻�� �ǵ� ON (PLC�� �ǵ��Ϸ� �� OFF)
		return TRUE;
#endif
	return FALSE;
}

void CGvisR2R_PunchView::SetMkFd(double dDist)
{
	double fLen = pDoc->GetOnePnlLen();
	double dOffset = dDist - (fLen*2.0);
	MoveMk(dOffset);
}

void CGvisR2R_PunchView::SetCycTime()
{
	DWORD dCur = GetTickCount();
	if (m_dwCycSt > 0)
	{
		m_dwCycTim = (double)(dCur - m_dwCycSt);
		if (m_dwCycTim < 0.0)
			m_dwCycTim *= (-1.0);
	}
	else
		m_dwCycTim = 0.0;
}

int CGvisR2R_PunchView::GetCycTime()
{
	if (m_dwCycTim < 0)
		m_dwCycTim = 0;

	int nTim = int(m_dwCycTim);
	return nTim;
}

BOOL CGvisR2R_PunchView::IsMkFdDone()
{
	if (m_nShareDnCnt > 0)
	{
		if (m_nShareDnCnt % 2)
			return TRUE;
	}
#ifdef USE_MPE
	if (!(pDoc->m_pMpeSignal[5] & (0x01 << 1)))	// ��ŷ�� �ǵ� ON (PLC�� �ǵ��Ϸ� �� OFF)
	{
		pDoc->LogAuto(_T("PLC: ��ŷ�� �ǵ� OFF (PLC�� �ǵ��Ϸ� �� OFF)"));
		return TRUE;
	}
#endif
	return FALSE;

	// 	if(!pView->m_pMotion)
	// 		return FALSE;
	// 
	// 	if(pView->m_pMotion->IsMotionDone(MS_MKFD))
	// 	{
	// // 		pDoc->m_pSliceIo[7] &= ~(0x01<<3);	// ��ŷ�� �ǵ� ��ȸ�� ����ġ ����
	// // 		pView->m_pMpe->Write(_T("MB003853"), 0);
	// 		return TRUE;
	// 	}
	// 	return FALSE;
}

BOOL CGvisR2R_PunchView::IsAoiFdDone()
{
#ifdef USE_MPE
	if (!(pDoc->m_pMpeSignal[5] & (0x01 << 0)))	// �˻�� �ǵ� ON (PLC�� �ǵ��Ϸ� �� OFF)
	{
		pDoc->LogAuto(_T("PLC: �˻�� �ǵ� ON (PLC�� �ǵ��Ϸ� �� OFF)"));
		return TRUE;
	}
#endif
	return FALSE;

	// 	if(!pView->m_pMotion)
	// 		return FALSE;
	// 
	// 	if(pView->m_pMotion->IsMotionDone(MS_AOIFD))
	// 	{
	// // 		pDoc->m_pSliceIo[9] &= ~(0x01<<2);	// �˻�� �ǵ� ��ȸ�� ����ġ ����
	// // 		pView->m_pMpe->Write(_T("MB003872"), 0);
	// 		return TRUE;
	// 	}
	// 	return FALSE;
}

double CGvisR2R_PunchView::GetAoi2InitDist()
{
	// 	double dInitD = _tstof(pDoc->WorkingInfo.LastJob.sDistAoiMk)-_tstof(pDoc->WorkingInfo.LastJob.sOnePnlLen);
	double dInitD = (_tstof(pDoc->WorkingInfo.Motion.sFdAoiAoiDistShot) - 1.0) * _tstof(pDoc->WorkingInfo.LastJob.sOnePnlLen);
	return dInitD;
}

double CGvisR2R_PunchView::GetMkInitDist()
{
	double dInitD0 = _tstof(pDoc->WorkingInfo.Motion.sFdAoiAoiDistShot) * _tstof(pDoc->WorkingInfo.LastJob.sOnePnlLen);
	double dInitD1 = (_tstof(pDoc->WorkingInfo.Motion.sFdMkAoiInitDist) + dInitD0) - (_tstof(pDoc->WorkingInfo.LastJob.sOnePnlLen)*2.0);
	return dInitD1;
}

double CGvisR2R_PunchView::GetRemain()
{
#ifdef USE_MPE
	// 	double dRemain = _tstof(pDoc->WorkingInfo.LastJob.sDistAoiMk) - m_dEnc[AXIS_RENC];
	double dCurPosMkFd = (double)pDoc->m_pMpeData[0][0];	// ��ŷ�� Feeding ���ڴ� ��(���� mm )
	double dInitD0 = _tstof(pDoc->WorkingInfo.Motion.sFdAoiAoiDistShot) * _tstof(pDoc->WorkingInfo.LastJob.sOnePnlLen);
	double dRemain = _tstof(pDoc->WorkingInfo.Motion.sFdMkAoiInitDist) + dInitD0 - dCurPosMkFd;
	return dRemain;
#else
	return 0.0;
#endif
}

void CGvisR2R_PunchView::UpdateWorking()
{
	if (m_pDlgMenu01)
		m_pDlgMenu01->UpdateWorking();
	if (m_pEngrave)
		m_pEngrave->SwMenu01UpdateWorking(TRUE);
}

void CGvisR2R_PunchView::StopFromThread()
{
	m_bStopFromThread = TRUE;
}

void CGvisR2R_PunchView::Stop()
{
	//StartLive();
	CString sMsg;
	if (m_pDlgMenu03)
	{
		if (IsAuto() && IsRun())
		{
			m_bStopF_Verify = TRUE;
		}

		m_pDlgMenu03->SwStop();
	}
}

BOOL CGvisR2R_PunchView::IsStop()
{
	if (m_sDispMain == _T("�� ��"))
	{
		return TRUE;
	}
	return FALSE;

	// 	BOOL bOn=FALSE;
	// 	if(m_pDlgMenu03)
	// 		bOn = m_pDlgMenu03->IsStop();
	// 	return bOn;
}

BOOL CGvisR2R_PunchView::IsRun()
{
	//return TRUE; // AlignTest
	if (m_sDispMain == _T("������") || m_sDispMain == _T("�ʱ����") || m_sDispMain == _T("�ܸ����")
		|| m_sDispMain == _T("�ܸ�˻�") || m_sDispMain == _T("�����˻�") || m_sDispMain == _T("�����˻�")
		|| m_sDispMain == _T("���˻�") || m_sDispMain == _T("������"))
		return TRUE;
	return FALSE;
	//return m_bSwRun;
}

void CGvisR2R_PunchView::ShowLive(BOOL bShow)
{
	if (bShow)
	{
		if (!IsShowLive())
			SetTimer(TIM_SHOW_MENU02, 30, NULL);
	}
	else
	{
		if (IsShowLive())
			SetTimer(TIM_SHOW_MENU01, 30, NULL);
	}
}

BOOL CGvisR2R_PunchView::IsShowLive()
{
	if (m_pDlgMenu02)
	{
		if (m_pDlgMenu02->IsWindowVisible())
			return TRUE;
	}

	return FALSE;
}

void CGvisR2R_PunchView::SetLotSt()
{
	stLotTime LotTime;
	GetTime(LotTime);

	pDoc->WorkingInfo.Lot.StTime.nYear = LotTime.nYear;
	pDoc->WorkingInfo.Lot.StTime.nMonth = LotTime.nMonth;
	pDoc->WorkingInfo.Lot.StTime.nDay = LotTime.nDay;
	pDoc->WorkingInfo.Lot.StTime.nHour = LotTime.nHour;
	pDoc->WorkingInfo.Lot.StTime.nMin = LotTime.nMin;
	pDoc->WorkingInfo.Lot.StTime.nSec = LotTime.nSec;

	pDoc->WorkingInfo.Lot.CurTime.nYear = LotTime.nYear;
	pDoc->WorkingInfo.Lot.CurTime.nMonth = LotTime.nMonth;
	pDoc->WorkingInfo.Lot.CurTime.nDay = LotTime.nDay;
	pDoc->WorkingInfo.Lot.CurTime.nHour = LotTime.nHour;
	pDoc->WorkingInfo.Lot.CurTime.nMin = LotTime.nMin;
	pDoc->WorkingInfo.Lot.CurTime.nSec = LotTime.nSec;

	pDoc->WorkingInfo.Lot.EdTime.nYear = 0;
	pDoc->WorkingInfo.Lot.EdTime.nMonth = 0;
	pDoc->WorkingInfo.Lot.EdTime.nDay = 0;
	pDoc->WorkingInfo.Lot.EdTime.nHour = 0;
	pDoc->WorkingInfo.Lot.EdTime.nMin = 0;
	pDoc->WorkingInfo.Lot.EdTime.nSec = 0;

	m_dwLotSt = GetTickCount();
	pDoc->SaveLotTime(m_dwLotSt);
	DispLotTime();

	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;

	if (pDoc->m_pReelMap)
		pDoc->m_pReelMap->SetLotSt();
	if (pDoc->m_pReelMapUp)
		pDoc->m_pReelMapUp->SetLotSt();
	if (bDualTest)
	{
		if (pDoc->m_pReelMapDn)
			pDoc->m_pReelMapDn->SetLotSt();
		if (pDoc->m_pReelMapAllUp)
			pDoc->m_pReelMapAllUp->SetLotSt();
		if (pDoc->m_pReelMapAllDn)
			pDoc->m_pReelMapAllDn->SetLotSt();
	}
}

void CGvisR2R_PunchView::SetLotEd()
{
	stLotTime LotTime;
	GetTime(LotTime);

	pDoc->WorkingInfo.Lot.EdTime.nYear = LotTime.nYear;
	pDoc->WorkingInfo.Lot.EdTime.nMonth = LotTime.nMonth;
	pDoc->WorkingInfo.Lot.EdTime.nDay = LotTime.nDay;
	pDoc->WorkingInfo.Lot.EdTime.nHour = LotTime.nHour;
	pDoc->WorkingInfo.Lot.EdTime.nMin = LotTime.nMin;
	pDoc->WorkingInfo.Lot.EdTime.nSec = LotTime.nSec;

	pDoc->WorkingInfo.Lot.CurTime.nYear = LotTime.nYear;
	pDoc->WorkingInfo.Lot.CurTime.nMonth = LotTime.nMonth;
	pDoc->WorkingInfo.Lot.CurTime.nDay = LotTime.nDay;
	pDoc->WorkingInfo.Lot.CurTime.nHour = LotTime.nHour;
	pDoc->WorkingInfo.Lot.CurTime.nMin = LotTime.nMin;
	pDoc->WorkingInfo.Lot.CurTime.nSec = LotTime.nSec;

	m_dwLotEd = GetTickCount();

	pDoc->SaveLotTime(pDoc->WorkingInfo.Lot.dwStTick);
	DispLotTime();

	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;

	if (pDoc->m_pReelMap)
		pDoc->m_pReelMap->SetLotEd();
	if (pDoc->m_pReelMapUp)
		pDoc->m_pReelMapUp->SetLotEd();
	if (bDualTest)
	{
		if (pDoc->m_pReelMapDn)
			pDoc->m_pReelMapDn->SetLotEd();
		if (pDoc->m_pReelMapAllUp)
			pDoc->m_pReelMapAllUp->SetLotEd();
		if (pDoc->m_pReelMapAllDn)
			pDoc->m_pReelMapAllDn->SetLotEd();
	}

}

void CGvisR2R_PunchView::DispLotTime()
{
	if (m_pDlgMenu01)
		m_pDlgMenu01->DispLotTime();
}

BOOL CGvisR2R_PunchView::IsTest()
{
#ifdef USE_MPE
	if (!pDoc->m_pMpeIo)
		return FALSE;

	// MpeIO
	int nInSeg = pDoc->MkIo.MpeIo.nInSeg;
	int nOutSeg = pDoc->MkIo.MpeIo.nOutSeg;

	BOOL bOn0 = (pDoc->m_pMpeIo[nInSeg + 10] & 0x01 << 8) ? TRUE : FALSE; //[34] �˻�� �� �˻� ���� <-> Y4368 I/F
	BOOL bOn1 = (pDoc->m_pMpeIo[nInSeg + 14] & 0x01 << 8) ? TRUE : FALSE; //[38] �˻�� �� �˻� ���� <-> Y4468 I/F

	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;
	if (bDualTest)
	{
		if (bOn0 || bOn1)
			return TRUE;
	}
	else
	{
		if (bOn0)
			return TRUE;
	}
#endif
	return FALSE;
}

BOOL CGvisR2R_PunchView::IsTestUp()
{
#ifdef USE_MPE
	if (!pDoc->m_pMpeIo)
		return FALSE;

	// MpeIO
	int nInSeg = pDoc->MkIo.MpeIo.nInSeg;
	int nOutSeg = pDoc->MkIo.MpeIo.nOutSeg;

	BOOL bOn0 = (pDoc->m_pMpeIo[nInSeg + 10] & 0x01 << 8) ? TRUE : FALSE; //[34] �˻�� �� �˻� ���� <-> Y4368 I/F
	if (bOn0)
		return TRUE;
#endif
	return FALSE;
}

BOOL CGvisR2R_PunchView::IsTestDn()
{
#ifdef USE_MPE
	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;
	if (!bDualTest)
		return FALSE;

	if (!pDoc->m_pMpeIo)
		return FALSE;

	// MpeIO
	int nInSeg = pDoc->MkIo.MpeIo.nInSeg;
	int nOutSeg = pDoc->MkIo.MpeIo.nOutSeg;

	BOOL bOn1 = (pDoc->m_pMpeIo[nInSeg + 14] & 0x01 << 8) ? TRUE : FALSE; //[38] �˻�� �� �˻� ���� <-> Y4468 I/F
	if (bOn1)
		return TRUE;
#endif
	return FALSE;
}

BOOL CGvisR2R_PunchView::IsAoiTblVac()
{
#ifdef USE_MPE
	if (!pDoc->m_pMpeIo)
		return FALSE;

	// MpeIO
	int nInSeg = pDoc->MkIo.MpeIo.nInSeg;
	int nOutSeg = pDoc->MkIo.MpeIo.nOutSeg;

	BOOL bOn0 = (pDoc->m_pMpeIo[nInSeg + 10] & 0x01 << 9) ? TRUE : FALSE; //[34] �˻�� �� �˻� ���̺� ���� SOL <-> Y4469 I/F
	BOOL bOn1 = (pDoc->m_pMpeIo[nInSeg + 14] & 0x01 << 9) ? TRUE : FALSE; //[38] �˻�� �� �˻� ���̺� ���� SOL <-> Y4469 I/F
	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;
	if (bDualTest)
	{
		if (bOn0 || bOn1)
			return TRUE;
	}
	else
	{
		if (bOn0)
			return TRUE;
	}
#endif
	return FALSE;
}

void CGvisR2R_PunchView::SetTest(BOOL bOn)
{
#ifdef USE_MPE
	if (bOn)
	{
		//IoWrite(_T("MB003828"), 1); // �˻�� �� �˻� ���� <-> Y4368 I/F
		pView->m_pMpe->Write(_T("MB003828"), 1);
		//IoWrite(_T("MB003928"), 1); // �˻�� �� �˻� ���� <-> Y4468 I/F
		pView->m_pMpe->Write(_T("MB003928"), 1);
	}
	else
	{
		//IoWrite(_T("MB003828"), 0); // �˻�� �� �˻� ���� <-> Y4368 I/F
		pView->m_pMpe->Write(_T("MB003828"), 0);
		//IoWrite(_T("MB003928"), 0); // �˻�� �� �˻� ���� <-> Y4468 I/F
		pView->m_pMpe->Write(_T("MB003928"), 0);
	}
#endif
}

void CGvisR2R_PunchView::SetTest0(BOOL bOn)
{
#ifdef USE_MPE
	if (bOn)
		pView->m_pMpe->Write(_T("MB003828"), 1);
	//IoWrite(_T("MB003828"), 1); // �˻�� �� �˻� ���� <-> Y4368 I/F
	else
		pView->m_pMpe->Write(_T("MB003828"), 0);
	//IoWrite(_T("MB003828"), 0); // �˻�� �� �˻� ���� <-> Y4368 I/F
#endif
}

void CGvisR2R_PunchView::SetTest1(BOOL bOn)
{
#ifdef USE_MPE
	if (bOn)
		pView->m_pMpe->Write(_T("MB003928"), 1);
	//IoWrite(_T("MB003928"), 1); // �˻�� �� �˻� ���� <-> Y4468 I/F
	else
		pView->m_pMpe->Write(_T("MB003928"), 0);
	//IoWrite(_T("MB003928"), 0); // �˻�� �� �˻� ���� <-> Y4468 I/F
#endif
}

BOOL CGvisR2R_PunchView::IsTestDone()
{
#ifdef USE_MPE
	BOOL bOn0 = (pDoc->m_pMpeIb[10] & (0x01 << 8)) ? TRUE : FALSE;	// �˻�� �� �˻� �Ϸ� <-> X4328 I/F
	BOOL bOn1 = (pDoc->m_pMpeIb[14] & (0x01 << 8)) ? TRUE : FALSE;	// �˻�� �� �˻� �Ϸ� <-> X4428 I/F
	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;
	if (bDualTest)
	{
		if (bOn0 && bOn1)
		{
			pDoc->LogAuto(_T("PLC: �˻�� ��(X4328) & ��(X4428) �˻� �Ϸ�"));
			return TRUE;
		}
	}
	else
	{
		if (bOn0)
		{
			pDoc->LogAuto(_T("PLC: �˻�� ��(X4328) �˻� �Ϸ�"));
			return TRUE;
		}
	}

	double dCurPosMkFd = (double)pDoc->m_pMpeData[0][0];	// ��ŷ�� Feeding ���ڴ� ��(���� mm )
	double dTgtFd = _tstof(pDoc->WorkingInfo.Motion.sFdAoiAoiDistShot) * _tstof(pDoc->WorkingInfo.Motion.sAoiFdDist);
	if (dCurPosMkFd + 10.0 < dTgtFd)//-_tstof(pDoc->WorkingInfo.Motion.sAoiFdDist))
	{
		if (bOn0)
			return TRUE;
	}
#endif
	return FALSE;
}

BOOL CGvisR2R_PunchView::IsAoiTblVacDone()
{
#ifdef USE_MPE
	BOOL bOn0 = (pDoc->m_pMpeIb[10] & (0x01 << 9)) ? TRUE : FALSE;	// �˻�� �� ���̺� ���� �Ϸ� <-> X4329 I/F
	BOOL bOn1 = (pDoc->m_pMpeIb[14] & (0x01 << 9)) ? TRUE : FALSE;	// �˻�� �� ���̺� ���� �Ϸ� <-> X4329 I/F
	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;
	if (bDualTest)
	{
		if (bOn0 && bOn1)
		{
			pDoc->LogAuto(_T("PLC: �˻�� ��(X4329) & ��(X4329) ���̺� ���� �Ϸ�"));
			return TRUE;
		}
	}
	else
	{
		if (bOn0)
		{
			pDoc->LogAuto(_T("PLC: �˻�� ��(X4329) ���̺� ���� �Ϸ�"));
			return TRUE;
		}
	}

	double dCurPosMkFd = (double)pDoc->m_pMpeData[0][0];	// ��ŷ�� Feeding ���ڴ� ��(���� mm )
	double dTgtFd = _tstof(pDoc->WorkingInfo.Motion.sFdAoiAoiDistShot) * _tstof(pDoc->WorkingInfo.Motion.sAoiFdDist);
	if (dCurPosMkFd + 10.0 < dTgtFd)//-_tstof(pDoc->WorkingInfo.Motion.sAoiFdDist))
	{
		if (bOn0)
			return TRUE;
	}
#endif
	return FALSE;
}

BOOL CGvisR2R_PunchView::IsTestDoneUp()
{
#ifdef USE_MPE
	BOOL bOn0 = (pDoc->m_pMpeIb[10] & (0x01 << 8)) ? TRUE : FALSE;	// �˻�� �� �˻� �Ϸ� <-> X4328 I/F
	if (bOn0)
	{
		pDoc->LogAuto(_T("PLC: �˻�� ��(X4328) �˻� �Ϸ�"));
		return TRUE;
	}
#endif
	return TRUE;
}

BOOL CGvisR2R_PunchView::IsTestDoneDn()
{
#ifdef USE_MPE
	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;
	if (!bDualTest)
		return TRUE;

	BOOL bOn1 = (pDoc->m_pMpeIb[14] & (0x01 << 8)) ? TRUE : FALSE;	// �˻�� �� �˻� �Ϸ� <-> X4428 I/F
	if (bOn1)
	{
		pDoc->LogAuto(_T("PLC: �˻�� ��(X4428) �˻� �Ϸ�"));
		return TRUE;
	}
#endif
	return TRUE;
}

void CGvisR2R_PunchView::SetReMk(BOOL bMk0, BOOL bMk1)
{
	CfPoint ptPnt;
	int nSerial, nTot;

	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;

	if (bMk0)
	{
		if (!m_bTHREAD_MK[0])
		{
			m_nStepMk[0] = 0;
			m_nMkPcs[0] = 0;
			m_bDoneMk[0] = FALSE;
			//m_bReMark[0] = FALSE;
			m_bTHREAD_MK[0] = TRUE;

			nSerial = m_nBufUpSerial[0]; // Cam0

			nTot = GetTotDefPcs0(nSerial);
			if (nTot>0)
			{
				ptPnt = GetMkPnt0(nSerial, 0);
				m_dTarget[AXIS_X0] = ptPnt.x;
				m_dTarget[AXIS_Y0] = ptPnt.y;
				if (nTot>1)
				{
					ptPnt = GetMkPnt0(nSerial, 1);
					m_dNextTarget[AXIS_X0] = ptPnt.x;
					m_dNextTarget[AXIS_Y0] = ptPnt.y;
				}
				else
				{
					m_dNextTarget[AXIS_X0] = -1.0;
					m_dNextTarget[AXIS_Y0] = -1.0;
				}
			}
			else
			{
				m_dTarget[AXIS_X0] = -1.0;
				m_dTarget[AXIS_Y0] = -1.0;
				m_dNextTarget[AXIS_X0] = -1.0;
				m_dNextTarget[AXIS_Y0] = -1.0;
			}
		}
	}

	if (bMk1)
	{
		if (!m_bTHREAD_MK[1])
		{
			m_nStepMk[1] = 0;
			m_nMkPcs[1] = 0;
			m_bDoneMk[1] = FALSE;
			//m_bReMark[1] = FALSE;
			m_bTHREAD_MK[1] = TRUE;

			nSerial = m_nBufUpSerial[1]; // Cam1

			nTot = GetTotDefPcs1(nSerial);
			if (nTot>0)
			{
				ptPnt = GetMkPnt1(nSerial, 0);
				m_dTarget[AXIS_X1] = ptPnt.x;
				m_dTarget[AXIS_Y1] = ptPnt.y;
				if (nTot>1)
				{
					ptPnt = GetMkPnt1(nSerial, 1);
					m_dNextTarget[AXIS_X1] = ptPnt.x;
					m_dNextTarget[AXIS_Y1] = ptPnt.y;
				}
				else
				{
					m_dNextTarget[AXIS_X1] = -1.0;
					m_dNextTarget[AXIS_Y1] = -1.0;
				}
			}
			else
			{
				m_dTarget[AXIS_X1] = -1.0;
				m_dTarget[AXIS_Y1] = -1.0;
				m_dNextTarget[AXIS_X1] = -1.0;
				m_dNextTarget[AXIS_Y1] = -1.0;
			}
		}
	}
}

BOOL CGvisR2R_PunchView::SetMk(BOOL bRun)	// Marking Start
{
	CfPoint ptPnt;
	int nSerial, nTot, a, b;

	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;

	for (a = 0; a<2; a++)
	{
		for (b = 0; b<MAX_STRIP_NUM; b++)
		{
			m_nMkStrip[a][b] = 0;			// [nCam][nStrip]:[2][4] - [��/��][] : ��Ʈ���� ��Ī�� �ǽ� �� count
			m_bRejectDone[a][b] = FALSE;	// [nCam][nStrip]:[2][4] - [��/��][] : ��Ʈ���� ��Ī�� �ǽ� �� count�� ��Ʈ�� ��� ������ �Ϸ� ���� 
		}
	}

	if (bRun)
	{
		if (m_bDoMk[0])
		{
			if (!m_bTHREAD_MK[0])
			{
				m_nStepMk[0] = 0;
				m_nMkPcs[0] = 0;
				m_bDoneMk[0] = FALSE;
				//m_bReMark[0] = FALSE;
				m_bTHREAD_MK[0] = TRUE;

				nSerial = m_nBufUpSerial[0]; // Cam0

				m_nTotMk[0] = nTot = GetTotDefPcs0(nSerial);
				m_nCurMk[0] = 0;
				if (nTot>0)
				{
					ptPnt = GetMkPnt0(nSerial, 0);
					m_dTarget[AXIS_X0] = ptPnt.x;
					m_dTarget[AXIS_Y0] = ptPnt.y;
					if (nTot>1)
					{
						ptPnt = GetMkPnt0(nSerial, 1);
						m_dNextTarget[AXIS_X0] = ptPnt.x;
						m_dNextTarget[AXIS_Y0] = ptPnt.y;
					}
					else
					{
						m_dNextTarget[AXIS_X0] = -1.0;
						m_dNextTarget[AXIS_Y0] = -1.0;
					}
				}
				else
				{
					m_dTarget[AXIS_X0] = -1.0;
					m_dTarget[AXIS_Y0] = -1.0;
					m_dNextTarget[AXIS_X0] = -1.0;
					m_dNextTarget[AXIS_Y0] = -1.0;
				}
			}
		}

		if (m_bDoMk[1])
		{
			if (!m_bTHREAD_MK[1])
			{
				m_nStepMk[1] = 0;
				m_nMkPcs[1] = 0;
				m_bDoneMk[1] = FALSE;
				//m_bReMark[1] = FALSE;
				m_bTHREAD_MK[1] = TRUE;

				nSerial = m_nBufUpSerial[1]; // Cam1

				m_nTotMk[1] = nTot = GetTotDefPcs1(nSerial);
				m_nCurMk[1] = 0;
				if (nTot>0)
				{
					ptPnt = GetMkPnt1(nSerial, 0);
					m_dTarget[AXIS_X1] = ptPnt.x;
					m_dTarget[AXIS_Y1] = ptPnt.y;
					if (nTot>1)
					{
						ptPnt = GetMkPnt1(nSerial, 1);
						m_dNextTarget[AXIS_X1] = ptPnt.x;
						m_dNextTarget[AXIS_Y1] = ptPnt.y;
					}
					else
					{
						m_dNextTarget[AXIS_X1] = -1.0;
						m_dNextTarget[AXIS_Y1] = -1.0;
					}
				}
				else
				{
					m_dTarget[AXIS_X1] = -1.0;
					m_dTarget[AXIS_Y1] = -1.0;
					m_dNextTarget[AXIS_X1] = -1.0;
					m_dNextTarget[AXIS_Y1] = -1.0;
				}
			}
		}
	}
	else
	{
		m_bTHREAD_MK[0] = FALSE;
		m_bTHREAD_MK[1] = FALSE;
	}
	return TRUE;
}

//BOOL CGvisR2R_PunchView::IsMk()
//{
//	if (m_bTHREAD_MK[0] && m_bTHREAD_MK[1] && m_nStepMk[0] != MK_END && m_nStepMk[1] != MK_END)
//		return TRUE;
//
//	return FALSE;
//}

BOOL CGvisR2R_PunchView::IsReMk()
{
	if (IsMoveDone())
		return FALSE;

	if (m_bReMark[0] && m_bDoneMk[1])
	{
		m_bReMark[0] = FALSE;
		SetReMk(TRUE, FALSE);
		return TRUE;
	}
	else if (m_bDoneMk[0] && m_bReMark[1])
	{
		m_bReMark[1] = FALSE;
		SetReMk(FALSE, TRUE);
		return TRUE;
	}
	else if (m_bReMark[0] && m_bReMark[1])
	{
		m_bReMark[0] = FALSE;
		m_bReMark[1] = FALSE;
		SetReMk(TRUE, TRUE);
		return TRUE;
	}

	return FALSE;
}

BOOL CGvisR2R_PunchView::IsMkDone()
{
	if (m_bDoneMk[0] && m_bDoneMk[1] && !m_bTHREAD_DISP_DEF)
		return TRUE;
	// 	if(m_bDoneMk[0] && m_bDoneMk[1])
	// 		return TRUE;

	// 	if( (!m_bTHREAD_MK[0] && !m_bTHREAD_MK[1] && m_nStepMk[0] >= MK_END+2 && m_nStepMk[1] >= MK_END+2) ||
	// 		(!m_bTHREAD_MK[0] && !m_bTHREAD_MK[1] && !m_nStepMk[0] && !m_nStepMk[1]) )
	// 		return TRUE;

	return FALSE;
}

int CGvisR2R_PunchView::GetSerial()
{
	int nSerial = 0;
	if (m_pDlgMenu01)
		nSerial = m_pDlgMenu01->GetSerial();
	return nSerial;
}

void CGvisR2R_PunchView::SetMkFdLen()
{
	// 	int nLast = pDoc->GetLastSerial();
	int nLast = pDoc->GetLastShotMk();
	pDoc->WorkingInfo.Motion.sMkFdTotLen.Format(_T("%.1f"), (double)nLast * _tstof(pDoc->WorkingInfo.LastJob.sOnePnlLen));

	CString sData, sPath = PATH_WORKING_INFO;
	sData = pDoc->WorkingInfo.Motion.sMkFdTotLen;
	::WritePrivateProfileString(_T("Motion"), _T("MARKING_FEEDING_SERVO_TOTAL_DIST"), sData, sPath);
}

double CGvisR2R_PunchView::GetMkFdLen()
{
	//	int nLast;
	//  	int nLast = pDoc->GetLastSerial();
	int nLast = pDoc->GetLastShotMk();

	// 	if(m_pDlgFrameHigh)
	// 		nLast = m_pDlgFrameHigh->m_nLastShot;
	// 	else
	// 		nLast = pDoc->m_pReelMapDn->GetLastSerial();

	double dLen = (double)nLast * _tstof(pDoc->WorkingInfo.LastJob.sOnePnlLen);

	return dLen;
}

// double CGvisR2R_PunchView::GetAoiFdLen()
// {
// 	int nLast =  pDoc->m_pReelMap->GetLastSerial();
// 	double dLen = (double)nLast * _tstof(pDoc->WorkingInfo.LastJob.sOnePnlLen);
// 	return dLen;
// }

double CGvisR2R_PunchView::GetAoiUpFdLen()
{
	// 	int nLast =  pDoc->m_pReelMapUp->GetLastSerial();
	// 	if(m_nBufTot[0] > 0)
	// 		nLast = m_pBufSerial[0][m_nBufTot[0]-1];

	int nLast = pDoc->GetLastShotUp();

	double dLen = (double)nLast * _tstof(pDoc->WorkingInfo.LastJob.sOnePnlLen);
	return dLen;
}

double CGvisR2R_PunchView::GetAoiDnFdLen()
{
	// 	int nLast = pDoc->m_pReelMapDn->GetLastSerial();
	// 	if(m_nBufTot[1] > 0)
	// 		nLast = m_pBufSerial[1][m_nBufTot[1]-1];

	int nLast = pDoc->GetLastShotDn();

	double dLen = (double)nLast * _tstof(pDoc->WorkingInfo.LastJob.sOnePnlLen);
	return dLen;
}

double CGvisR2R_PunchView::GetTotVel()
{
	// 	if(!pView->IsRun() || !pView->IsAuto() || GetAutoStep() < 22) // ���ο� ��Ʈ�� ���۽ð��� ������.
	// 		return m_dTotVel;

	CString str, sPrev;
	int nDiff;
	int nHour, nMin, nSec;
	int nStYear, nStMonth, nStDay, nStHour, nStMin, nStSec;
	int nCurYear, nCurMonth, nCurDay, nCurHour, nCurMin, nCurSec;
	int nEdYear, nEdMonth, nEdDay, nEdHour, nEdMin, nEdSec;

	nStYear = pDoc->WorkingInfo.Lot.StTime.nYear;
	nStMonth = pDoc->WorkingInfo.Lot.StTime.nMonth;
	nStDay = pDoc->WorkingInfo.Lot.StTime.nDay;
	nStHour = pDoc->WorkingInfo.Lot.StTime.nHour;
	nStMin = pDoc->WorkingInfo.Lot.StTime.nMin;
	nStSec = pDoc->WorkingInfo.Lot.StTime.nSec;

	nCurYear = pDoc->WorkingInfo.Lot.CurTime.nYear;
	nCurMonth = pDoc->WorkingInfo.Lot.CurTime.nMonth;
	nCurDay = pDoc->WorkingInfo.Lot.CurTime.nDay;
	nCurHour = pDoc->WorkingInfo.Lot.CurTime.nHour;
	nCurMin = pDoc->WorkingInfo.Lot.CurTime.nMin;
	nCurSec = pDoc->WorkingInfo.Lot.CurTime.nSec;

	nEdYear = pDoc->WorkingInfo.Lot.EdTime.nYear;
	nEdMonth = pDoc->WorkingInfo.Lot.EdTime.nMonth;
	nEdDay = pDoc->WorkingInfo.Lot.EdTime.nDay;
	nEdHour = pDoc->WorkingInfo.Lot.EdTime.nHour;
	nEdMin = pDoc->WorkingInfo.Lot.EdTime.nMin;
	nEdSec = pDoc->WorkingInfo.Lot.EdTime.nSec;

	int nTotSec = 0;
	double dMkFdLen = GetMkFdLen();
	if (!nStYear && !nStMonth && !nStDay && !nStHour && !nStMin && !nStSec)
	{
		return 0.0;
	}
	else if (!nEdYear && !nEdMonth && !nEdDay && !nEdHour && !nEdMin && !nEdSec)
	{
		nDiff = (GetTickCount() - pView->m_dwLotSt) / 1000;
		nHour = int(nDiff / 3600);
		nMin = int((nDiff - 3600 * nHour) / 60);
		nSec = nDiff % 60;
	}
	else
	{
		nDiff = (pView->m_dwLotEd - pView->m_dwLotSt) / 1000;
		nHour = int(nDiff / 3600);
		nMin = int((nDiff - 3600 * nHour) / 60);
		nSec = nDiff % 60;
	}

	nTotSec = nHour * 3600 + nMin * 60 + nSec;
	double dVel = 0.0;
	if (nTotSec > 0)
		dVel = dMkFdLen / (double)nTotSec; // [mm/sec]
										   // 		dVel = (dMkFdLen*60.0) / ((double)nTotSec*1000.0); // [M/min]

	m_dTotVel = dVel;
	return dVel;
}

double CGvisR2R_PunchView::GetPartVel()
{
	// 	if(!pView->IsRun() || !pView->IsAuto() || GetAutoStep() < 22) // ���ο� ��Ʈ�� ���۽ð��� ������.
	// 		return m_dPartVel;

	double dLen = _tstof(pDoc->WorkingInfo.LastJob.sOnePnlLen) * 2.0;
	double dSec = (double)GetCycTime() / 1000.0;
	double dVel = 0.0;
	if (dSec > 0.0)
		dVel = dLen / dSec; // [mm/sec]
							// 		dVel = (dLen*60.0) / (dSec*1000.0); // [M/min]
	m_dPartVel = dVel;
	return dVel;
}


BOOL CGvisR2R_PunchView::IsBuffer(int nNum)
{
	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;
	//	if(m_nBufTot[0] > 1 && m_nBufTot[1] > 1)
	if (bDualTest)
	{
		if (m_nBufTot[0] > nNum && m_nBufTot[1] > nNum) // [0]: AOI-Up , [1]: AOI-Dn
			return TRUE;

		if (m_bLastProc)
		{
			if ((m_nBufTot[0] > nNum || !m_bIsBuf[0]) && (m_nBufTot[1] > nNum || !m_bIsBuf[1])) // [0]: AOI-Up , [1]: AOI-Dn
				return TRUE;
		}
	}
	else
	{
		if (m_nBufTot[0] > nNum) // [0]: AOI-Up
			return TRUE;
	}

	return FALSE;
}

BOOL CGvisR2R_PunchView::IsBufferUp()
{
	if (m_nBufTot[0] > 0)
		return TRUE;
	return FALSE;
}

BOOL CGvisR2R_PunchView::IsBufferDn()
{
	if (m_nBufTot[1] > 0)
		return TRUE;
	return FALSE;
}

int CGvisR2R_PunchView::GetBuffer(int *pPrevSerial)
{
	int nS0 = GetBufferUp(pPrevSerial);
	int nS1 = GetBufferDn(pPrevSerial);
	if (nS0 != nS1)
		return 0;
	return nS0;
}

int CGvisR2R_PunchView::GetBufferUp(int *pPrevSerial)
{
	if (IsBufferUp())
		return m_pBufSerial[0][0];
	else if (pPrevSerial)
		*pPrevSerial = m_pBufSerial[0][0];
	return 0;
}

int CGvisR2R_PunchView::GetBufferDn(int *pPrevSerial)
{
	if (IsBufferDn())
		return m_pBufSerial[1][0];
	else if (pPrevSerial)
		*pPrevSerial = m_pBufSerial[1][0];
	return 0;
}

BOOL CGvisR2R_PunchView::IsBuffer0()
{
	if (m_nBufTot[0] > 0 && m_nBufTot[1] > 0)
		return TRUE;
	return FALSE;
}

BOOL CGvisR2R_PunchView::IsBufferUp0()
{
	if (m_nBufTot[0] > 0)
		return TRUE;
	return FALSE;
}

BOOL CGvisR2R_PunchView::IsBufferDn0()
{
	if (m_nBufTot[1] > 0)
		return TRUE;
	return FALSE;
}

int CGvisR2R_PunchView::GetBuffer0(int *pPrevSerial)
{
	int nS0 = GetBufferUp0(pPrevSerial);
	int nS1 = GetBufferDn0(pPrevSerial);
	if (nS0 != nS1)
		return 0;
	return nS0;
}

int CGvisR2R_PunchView::GetBufferUp0(int *pPrevSerial)
{
	if (IsBufferUp0())
		return m_pBufSerial[0][0];
	else if (pPrevSerial)
		*pPrevSerial = m_pBufSerial[0][0];
	return 0;
}

int CGvisR2R_PunchView::GetBufferDn0(int *pPrevSerial)
{
	if (IsBufferDn0())
		return m_pBufSerial[1][0];
	else if (pPrevSerial)
		*pPrevSerial = m_pBufSerial[1][0];
	return 0;
}



BOOL CGvisR2R_PunchView::IsBuffer1()
{
	if (m_nBufTot[0] > 1 && m_nBufTot[1] > 1)
		return TRUE;
	return FALSE;
}

BOOL CGvisR2R_PunchView::IsBufferUp1()
{
	if (m_nBufTot[0] > 1)
		return TRUE;
	return FALSE;
}

BOOL CGvisR2R_PunchView::IsBufferDn1()
{
	if (m_nBufTot[1] > 1)
		return TRUE;
	return FALSE;
}

int CGvisR2R_PunchView::GetBuffer1(int *pPrevSerial)
{
	int nS0 = GetBufferUp1(pPrevSerial);
	int nS1 = GetBufferDn1(pPrevSerial);
	if (nS0 != nS1)
		return 0;
	return nS0;
}

int CGvisR2R_PunchView::GetBufferUp1(int *pPrevSerial)
{
	if (IsBufferUp1())
		return m_pBufSerial[0][1];
	else if (pPrevSerial)
		*pPrevSerial = m_pBufSerial[0][1];
	return 0;
}

int CGvisR2R_PunchView::GetBufferDn1(int *pPrevSerial)
{
	if (IsBufferDn1())
		return m_pBufSerial[1][1];
	else if (pPrevSerial)
		*pPrevSerial = m_pBufSerial[1][1];
	return 0;
}



BOOL CGvisR2R_PunchView::IsShare()
{
	// 	if(IsShareUp() || IsShareDn())
	// 		return TRUE;
	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;

	if (bDualTest)
	{
		if (m_bWaitPcr[0] && m_bWaitPcr[1])
		{
			if (IsShareUp() && IsShareDn())
			{
				m_bWaitPcr[0] = FALSE;
				m_bWaitPcr[1] = FALSE;
				return TRUE;
			}
		}
		else if (m_bWaitPcr[0] && !m_bWaitPcr[1])
		{
			if (IsShareUp())
			{
				m_bWaitPcr[0] = FALSE;
				return TRUE;
			}
		}
		else if (!m_bWaitPcr[0] && m_bWaitPcr[1])
		{
			if (IsShareDn())
			{
				m_bWaitPcr[1] = FALSE;
				return TRUE;
			}
		}
		else
		{
			if (IsShareUp() || IsShareDn())
				return TRUE;
		}

		//	double dCurPosMkFd = (double)pDoc->m_pMpeData[0][0];	// ��ŷ�� Feeding ���ڴ� ��(���� mm )
		// 	double dTgtFd = _tstof(pDoc->WorkingInfo.Motion.sFdAoiAoiDistShot) * _tstof(pDoc->WorkingInfo.Motion.sAoiFdDist);
		// 	if(dCurPosMkFd < dTgtFd-_tstof(pDoc->WorkingInfo.Motion.sAoiFdDist) + 10.0)
		// 	{
		// 		if(IsShareUp())
		// 			return TRUE;
		// 	}
	}
	else
	{
		if (m_bWaitPcr[0])
		{
			if (IsShareUp())
			{
				m_bWaitPcr[0] = FALSE;
				return TRUE;
			}
		}
		else
		{
			if (IsShareUp())
				return TRUE;
		}
	}
	return FALSE;
}

BOOL CGvisR2R_PunchView::IsShareUp()
{
	return pDoc->Status.PcrShare[0].bExist;
}

BOOL CGvisR2R_PunchView::IsShareDn()
{
	return pDoc->Status.PcrShare[1].bExist;
}

// BOOL CGvisR2R_PunchView::IsRst()
// {
// 	int nRstNum = GetAoiSerial();
// 	if(m_nRstNum != nRstNum)
// 	{
// 		m_nRstNum = nRstNum;
// 		return TRUE;
// 	}
// 	
// 	return FALSE;
// }

BOOL CGvisR2R_PunchView::IsVsShare()
{
	int nSerial;
	BOOL bExist = ChkVsShare(nSerial);
	return bExist;
}

// int CGvisR2R_PunchView::GetShare()
// {
// 	int nS0 = GetShareUp();
// 	int nS1 = GetShareDn();
// 	if(nS0 == nS1)
// 		return nS0;
// 
// 
// 	return -1;
// }

int CGvisR2R_PunchView::GetShareUp()
{
	return pDoc->Status.PcrShare[0].nSerial;
}
int CGvisR2R_PunchView::GetShareDn()
{
	return pDoc->Status.PcrShare[1].nSerial;
}


BOOL CGvisR2R_PunchView::ChkLastProc()
{
	BOOL bRtn = FALSE;
	if (m_pDlgMenu01)
		bRtn = (m_pDlgMenu01->m_bLastProc);
	return bRtn;
}

BOOL CGvisR2R_PunchView::ChkLastProcFromUp()
{
	BOOL bRtn = TRUE;
	if (m_pDlgMenu01)
		bRtn = (m_pDlgMenu01->m_bLastProcFromUp);
	return bRtn;
}

BOOL CGvisR2R_PunchView::ChkLastProcFromEng()
{
	BOOL bRtn = TRUE;
	if (m_pDlgMenu01)
		bRtn = (m_pDlgMenu01->m_bLastProcFromEng);
	return bRtn;
}

BOOL CGvisR2R_PunchView::ChkLotEnd(int nSerial)
{
	if (nSerial <= 0)
	{
		pView->ClrDispMsg();
		AfxMessageBox(_T("Serial Error.25"));
		return FALSE;
	}

	BOOL bEnd0 = ChkLotEndUp(nSerial);
	BOOL bEnd1 = ChkLotEndDn(nSerial);
	if (bEnd0 || bEnd1)
		return TRUE;
	return FALSE;
}

BOOL CGvisR2R_PunchView::ChkLotEndUp(int nSerial)
{
	if (nSerial <= 0)
	{
		pView->ClrDispMsg();
		AfxMessageBox(_T("Serial Error.26"));
		return 0;
	}

	CString sPath;
	sPath.Format(_T("%s%04d.pcr"), pDoc->WorkingInfo.System.sPathVrsBufUp, nSerial);
	// 	sPath.Format(_T("%s%04d.pcr"), pDoc->WorkingInfo.System.sPathVrsShareUp, nSerial); 
	return pDoc->ChkLotEnd(sPath);
}

BOOL CGvisR2R_PunchView::ChkLotEndDn(int nSerial)
{
	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;
	if (!bDualTest)
		return FALSE;

	if (nSerial <= 0)
	{
		pView->ClrDispMsg();
		AfxMessageBox(_T("Serial Error.27"));
		return 0;
	}

	CString sPath;
	sPath.Format(_T("%s%04d.pcr"), pDoc->WorkingInfo.System.sPathVrsBufDn, nSerial);
	// 	sPath.Format(_T("%s%04d.pcr"), pDoc->WorkingInfo.System.sPathVrsShareDn, nSerial); 
	return pDoc->ChkLotEnd(sPath);
}

BOOL CGvisR2R_PunchView::ChkMkTmpStop() // �����������.
{
	//if (IsStop() && IsMk())
	//{
	//	m_bMkTmpStop = TRUE;
	//	SetMk(FALSE);	// Marking �Ͻ�����
	//}
	//else if (IsRun() && m_bMkTmpStop)
	//{
	//	m_bMkTmpStop = FALSE;
	//	SetMk(TRUE);	// Marking Start
	//}

	//return m_bMkTmpStop;
	return FALSE;
}

BOOL CGvisR2R_PunchView::IsMkTmpStop()
{
	return m_bMkTmpStop;
}

BOOL CGvisR2R_PunchView::SetSerial(int nSerial, BOOL bDumy)
{
	if (nSerial <= 0)
	{
		pView->ClrDispMsg();
		AfxMessageBox(_T("Serial Error.28"));
		return 0;
	}
//	else if (nSerial > pDoc->m_ListBuf[0].GetLast())
//	{
//		return 0;
//	}

	if (!m_pDlgMenu01)
		return FALSE;

	int nPrevSerial = m_pDlgMenu01->GetCurSerial();

	if (nPrevSerial == nSerial)
		return TRUE;

	BOOL bRtn[2] = {1};
	bRtn[0] = m_pDlgMenu01->SetSerial(nSerial, bDumy);
	if (pDoc->GetTestMode() == MODE_OUTER)
	{
		if (!m_pDlgMenu06)
			bRtn[1] = m_pDlgMenu06->SetSerial(nSerial, bDumy);
	}

	return (bRtn[0] && bRtn[1]);
}

BOOL CGvisR2R_PunchView::SetSerialReelmap(int nSerial, BOOL bDumy)
{
	if (!m_pDlgMenu01)
	{
		pView->ClrDispMsg();
		AfxMessageBox(_T("Error - SetSerialReelmap : m_pDlgMenu01 is NULL."));
		return FALSE;
	}

	//if (pDoc->GetTestMode() == MODE_OUTER)
	//{
	//	if (!m_pDlgMenu06)
	//	{
	//		pView->ClrDispMsg();
	//		AfxMessageBox(_T("Error - SetSerialReelmap : m_pDlgMenu06 is NULL."));
	//		return FALSE;
	//	}

	//	m_pDlgMenu06->SetSerialReelmap(nSerial, bDumy);
	//}

	return m_pDlgMenu01->SetSerialReelmap(nSerial, bDumy);
}

BOOL CGvisR2R_PunchView::SetSerialMkInfo(int nSerial, BOOL bDumy)
{
	if (!m_pDlgMenu01)
		return FALSE;
	return m_pDlgMenu01->SetSerialMkInfo(nSerial, bDumy);
}

void CGvisR2R_PunchView::InitAuto(BOOL bInit)
{
	if (!pDoc->WorkingInfo.LastJob.bSampleTest)
	{
		::WritePrivateProfileString(_T("Infomation"), _T("Lot End"), _T("0"), pDoc->WorkingInfo.System.sPathMkCurrInfo);
		::WritePrivateProfileString(_T("Infomation"), _T("Last Shot"), _T("10000"), pDoc->WorkingInfo.System.sPathMkCurrInfo);
	}

	m_sAoiUpAlarmReStartMsg = GetAoiUpAlarmRestartMsg();
	m_sAoiDnAlarmReStartMsg = GetAoiDnAlarmRestartMsg();
	m_sAoiUpAlarmReTestMsg = GetAoiUpAlarmReTestMsg();
	m_sAoiDnAlarmReTestMsg = GetAoiDnAlarmReTestMsg();

	SetAoiUpAutoStep(0);
	SetAoiDnAutoStep(0);

	pView->m_nDebugStep = 10; pView->DispThreadTick();
	int nCam, nPoint, kk, a, b;
	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;

	for (kk = 0; kk < 10; kk++)
	{
		m_bDispMsgDoAuto[kk] = FALSE;
		m_nStepDispMsg[kk] = 0;
	}
	m_sFixMsg[0] = _T("");
	m_sFixMsg[1] = _T("");

	m_bReadyDone = FALSE;
	m_bChkLastProcVs = FALSE;
	m_nDummy[0] = 0;
	m_nDummy[1] = 0;
	m_nAoiLastSerial[0] = 0;
	m_nAoiLastSerial[1] = 0;
	m_nStepAuto = 0;
	m_nPrevStepAuto = 0;
	m_nPrevMkStAuto = 0;
	m_bAoiLdRun = TRUE;
	m_bAoiLdRunF = FALSE;
	m_bNewModel = FALSE;
	m_nLotEndSerial = 0;
	if(m_pDlgMenu01)
		m_pDlgMenu01->DispLotEndSerial(0);
	m_bCam = FALSE;
	m_bReview = FALSE;
	m_bChkBufIdx[0] = TRUE;
	m_bChkBufIdx[0] = TRUE;

	m_nErrCnt = 0;

	m_nStepMk[0] = 0;
	m_nStepMk[1] = 0;
	m_nStepMk[2] = 0;
	m_nStepMk[3] = 0;
	m_bTHREAD_MK[0] = FALSE;
	m_bTHREAD_MK[1] = FALSE;
	m_bTHREAD_MK[2] = FALSE;
	m_bTHREAD_MK[3] = FALSE;
	m_nMkPcs[0] = 0;
	m_nMkPcs[1] = 0;
	m_nMkPcs[2] = 0;
	m_nMkPcs[3] = 0;

	m_bMkTmpStop = FALSE;

	m_bWaitPcr[0] = FALSE;
	m_bWaitPcr[1] = FALSE;


	m_nShareUpS = 0;
	m_nShareUpSerial[0] = 0;
	m_nShareUpSerial[1] = 0;
	m_nShareUpCnt = 0;

	m_nShareDnS = 0;
	m_nShareDnSerial[0] = 0;
	m_nShareDnSerial[1] = 0;
	m_nShareDnCnt = 0;

	m_nBufUpSerial[0] = 0;
	m_nBufUpSerial[1] = 0;
	m_nBufUpCnt = 0;

	m_nBufDnSerial[0] = 0;
	m_nBufDnSerial[1] = 0;
	m_nBufDnCnt = 0;

	for (nCam = 0; nCam < 2; nCam++)
	{
		for (nPoint = 0; nPoint < 4; nPoint++)
		{
			m_pDlgMenu02->m_dMkFdOffsetX[nCam][nPoint] = 0.0;
			m_pDlgMenu02->m_dMkFdOffsetY[nCam][nPoint] = 0.0;
		}
	}


	m_pDlgMenu02->m_dAoiUpFdOffsetX = 0.0;
	m_pDlgMenu02->m_dAoiUpFdOffsetY = 0.0;
	m_pDlgMenu02->m_dAoiDnFdOffsetX = 0.0;
	m_pDlgMenu02->m_dAoiDnFdOffsetY = 0.0;

	m_bReAlign[0][0] = FALSE;	// [nCam][nPos]
	m_bReAlign[0][1] = FALSE;	// [nCam][nPos]
	m_bReAlign[0][2] = FALSE;	// [nCam][nPos]
	m_bReAlign[0][3] = FALSE;	// [nCam][nPos]
	m_bReAlign[1][0] = FALSE;	// [nCam][nPos]
	m_bReAlign[1][1] = FALSE;	// [nCam][nPos]
	m_bReAlign[1][2] = FALSE;	// [nCam][nPos]
	m_bReAlign[1][3] = FALSE;	// [nCam][nPos]

	m_bSkipAlign[0][0] = FALSE;	// [nCam][nPos]
	m_bSkipAlign[0][1] = FALSE;	// [nCam][nPos]
	m_bSkipAlign[0][2] = FALSE;	// [nCam][nPos]
	m_bSkipAlign[0][3] = FALSE;	// [nCam][nPos]
	m_bSkipAlign[1][0] = FALSE;	// [nCam][nPos]
	m_bSkipAlign[1][1] = FALSE;	// [nCam][nPos]
	m_bSkipAlign[1][2] = FALSE;	// [nCam][nPos]
	m_bSkipAlign[1][3] = FALSE;	// [nCam][nPos]

	m_bFailAlign[0][0] = FALSE;	// [nCam][nPos]
	m_bFailAlign[0][1] = FALSE;	// [nCam][nPos]
	m_bFailAlign[0][2] = FALSE;	// [nCam][nPos]
	m_bFailAlign[0][3] = FALSE;	// [nCam][nPos]
	m_bFailAlign[1][0] = FALSE;	// [nCam][nPos]
	m_bFailAlign[1][1] = FALSE;	// [nCam][nPos]
	m_bFailAlign[1][2] = FALSE;	// [nCam][nPos]
	m_bFailAlign[1][3] = FALSE;	// [nCam][nPos]

	m_bDoMk[0] = TRUE;			// [nCam]
	m_bDoMk[1] = TRUE;			// [nCam]
	m_bDoneMk[0] = FALSE;		// [nCam]
	m_bDoneMk[1] = FALSE;		// [nCam]
	m_bReMark[0] = FALSE;		// [nCam]
	m_bReMark[1] = FALSE;		// [nCam]

	m_nTotMk[0] = 0;
	m_nCurMk[0] = 0;
	m_nTotMk[1] = 0;
	m_nCurMk[1] = 0;
	m_nPrevTotMk[0] = 0;
	m_nPrevCurMk[0] = 0;
	m_nPrevTotMk[1] = 0;
	m_nPrevCurMk[1] = 0;


	m_bMkSt = FALSE;
	::WritePrivateProfileString(_T("Last Job"), _T("MkSt"), _T("0"), PATH_WORKING_INFO);
	m_bMkStSw = FALSE;
	m_nMkStAuto = 0;

	m_bEngSt = FALSE;
	m_bEngStSw = FALSE;
	m_nEngStAuto = 0;

	m_bEng2dSt = FALSE;
	m_bEng2dStSw = FALSE;
	m_nEng2dStAuto = 0;

	m_bLotEnd = FALSE;
	m_nLotEndAuto = 0;

	m_bLastProc = FALSE;
	if(MODE_INNER != pDoc->GetTestMode())
		m_bLastProcFromUp = TRUE;
	else
		m_bLastProcFromEng = TRUE;

	m_nLastProcAuto = 0;

	pDoc->m_sAlmMsg = _T("");
	pDoc->m_sIsAlmMsg = _T("");
	pDoc->m_sPrevAlmMsg = _T("");

	m_dwCycSt = 0;
	m_sNewLotUp = _T("");
	m_sNewLotDn = _T("");

	m_nStop = 0;

	m_nStepTHREAD_DISP_DEF = 0;
	m_bTHREAD_DISP_DEF = FALSE;				// CopyDefImg Stop
	m_nStepTHREAD_DISP_DEF_INNER = 0;
	m_bTHREAD_DISP_DEF_INNER = FALSE;		// DispDefImg Stop

	pDoc->SetMkMenu01(_T("Signal"), _T("DispDefImg"), _T("0"));

	pView->m_nDebugStep = 11; pView->DispThreadTick();
	for (a = 0; a<2; a++)
	{
		for (b = 0; b<4; b++)
		{
			m_nMkStrip[a][b] = 0;
			m_bRejectDone[a][b] = FALSE;
		}
	}

	pView->m_nDebugStep = 12; pView->DispThreadTick();
	m_pMpe->Write(_T("MB440100"), 0); // PLC �����غ� �Ϸ�(PC�� Ȯ���ϰ� Reset��Ŵ.)
	m_pMpe->Write(_T("MB440110"), 0); // ��ŷ����(PC�� Ȯ���ϰ� Reset��Ŵ.)-20141029
	m_pMpe->Write(_T("MB440150"), 0); // ��ŷ�� ��ŷ�� ON (PC�� ON, OFF)
	m_pMpe->Write(_T("MB440170"), 0); // ��ŷ�Ϸ�(PLC�� Ȯ���ϰ� Reset��Ŵ.)-20141029

	InitAutoEngSignal();

	pView->m_nDebugStep = 13; pView->DispThreadTick();
	MoveInitPos1();
	Sleep(30);
	MoveInitPos0();

	pView->m_nDebugStep = 14; pView->DispThreadTick();
	InitIoWrite();
	//pView->m_nDebugStep = 15; pView->DispThreadTick();
	//OpenShareUp();
	//pView->m_nDebugStep = 16; pView->DispThreadTick();
	//OpenShareDn();
	pView->m_nDebugStep = 17; pView->DispThreadTick();
	SetTest(FALSE);	// �˻�� ��/�� �˻� ���� (Off)
	pView->m_nDebugStep = 18; pView->DispThreadTick();
	if (m_pDlgMenu01)
	{
		m_pDlgMenu01->m_bLastProc = FALSE;

		if (MODE_INNER != pDoc->GetTestMode())
			m_pDlgMenu01->m_bLastProcFromUp = TRUE;
		else
			m_pDlgMenu01->m_bLastProcFromEng = TRUE;

		m_pDlgMenu01->ResetSerial();
		m_pDlgMenu01->ResetLastProc();
	}


	m_bTHREAD_UPDATAE_YIELD[0] = FALSE;
	m_bTHREAD_UPDATAE_YIELD[1] = FALSE;
	m_nSerialTHREAD_UPDATAE_YIELD[0] = 0;
	m_nSerialTHREAD_UPDATAE_YIELD[1] = 0;

	m_bTHREAD_UPDATE_REELMAP_UP = FALSE;
	m_bTHREAD_UPDATE_REELMAP_ALLUP = FALSE;
	m_bTHREAD_UPDATE_REELMAP_DN = FALSE;
	m_bTHREAD_UPDATE_REELMAP_ALLDN = FALSE;
	m_bTHREAD_UPDATE_REELMAP_ITS = FALSE;
	m_bTHREAD_UPDATE_REELMAP_INNER_UP = FALSE;
	m_bTHREAD_UPDATE_REELMAP_INNER_ALLUP = FALSE;
	m_bTHREAD_UPDATE_REELMAP_INNER_DN = FALSE;
	m_bTHREAD_UPDATE_REELMAP_INNER_ALLDN = FALSE;

	m_bTHREAD_REELMAP_YIELD_UP = FALSE;
	m_bTHREAD_REELMAP_YIELD_ALLUP = FALSE;
	m_bTHREAD_REELMAP_YIELD_DN = FALSE;
	m_bTHREAD_REELMAP_YIELD_ALLDN = FALSE;
	m_bTHREAD_REELMAP_YIELD_ITS = FALSE;

	m_bTHREAD_UPDATE_REELMAP_UP_OFFLINE = FALSE;
	m_bTHREAD_UPDATE_REELMAP_ALLUP_OFFLINE = FALSE;
	m_bTHREAD_UPDATE_REELMAP_DN_OFFLINE = FALSE;
	m_bTHREAD_UPDATE_REELMAP_ALLDN_OFFLINE = FALSE;

	m_bTHREAD_LOAD_PCR_UP_OFFLINE = FALSE;
	m_bTHREAD_LOAD_PCR_ALLUP_OFFLINE = FALSE;
	m_bTHREAD_LOAD_PCR_DN_OFFLINE = FALSE;
	m_bTHREAD_LOAD_PCR_ALLDN_OFFLINE = FALSE;

	m_nSnTHREAD_UPDATAE_YIELD = 0;

	pDoc->m_nEjectBufferLastShot = -1;
	m_bSerialDecrese = FALSE;
	m_bStopF_Verify = TRUE;
	m_bInitAuto = TRUE;
	m_bInitAutoLoadMstInfo = TRUE;

	if (bInit) // �̾�Ⱑ �ƴѰ��.
	{
		m_pMpe->Write(_T("MB440187"), 0); // �̾��(PC�� On��Ű��, PLC�� Ȯ���ϰ� Off��Ŵ)-20141121
		DispLotEnd(FALSE);

		m_nRstNum = 0;
		m_bCont = FALSE;
		m_dTotVel = 0.0;
		m_dPartVel = 0.0;
		m_dwCycSt = 0;
		m_dwCycTim = 0;

		pDoc->m_nPrevSerial = 0;
		pDoc->m_bNewLotShare[0] = FALSE;
		pDoc->m_bNewLotShare[1] = FALSE;
		pDoc->m_bNewLotBuf[0] = FALSE;
		pDoc->m_bNewLotBuf[1] = FALSE;
		pDoc->m_bDoneChgLot = FALSE;

		m_pDlgFrameHigh->m_nMkLastShot = 0;
		m_pDlgFrameHigh->m_nAoiLastShot[0] = 0;
		m_pDlgFrameHigh->m_nAoiLastShot[1] = 0;

		pView->m_nDebugStep = 20; pView->DispThreadTick();
		if (m_pDlgMenu01)
			m_pDlgMenu01->ResetLotTime();

		//ClrMkInfo();
		pView->m_nDebugStep = 21; pView->DispThreadTick();

		//ResetMkInfo(0); // 0 : AOI-Up , 1 : AOI-Dn , 2 : AOI-UpDn
		//pView->m_nDebugStep = 22; pView->DispThreadTick();
		//if (bDualTest)
		//	ResetMkInfo(1);

		pView->m_nDebugStep = 23; pView->DispThreadTick();
		ClrMkInfo(); // 20220420 - Happen Release Trouble

		pView->m_nDebugStep = 24; pView->DispThreadTick();
		if (m_pDlgFrameHigh)
		{
			m_pDlgFrameHigh->SetMkLastShot(0);
			m_pDlgFrameHigh->SetAoiLastShot(0, 0);
			m_pDlgFrameHigh->SetAoiLastShot(1, 0);
			m_pDlgFrameHigh->SetEngraveLastShot(0);
		}
		pView->m_nDebugStep = 25; pView->DispThreadTick();

		//pDoc->m_ListBuf[0].Clear();
		//pDoc->m_ListBuf[1].Clear();
	}
	else
	{
		pView->m_nDebugStep = 26; pView->DispThreadTick();
		//if (pDoc->GetTestMode() == MODE_INNER && pDoc->WorkingInfo.LastJob.bDispLotEnd)
		if (pDoc->GetTestMode() == MODE_INNER && pDoc->WorkingInfo.LastJob.bDispLotEnd)
		{
			if (IDYES == pView->MsgBox(_T("���κο��� ������ ���κ��� �����Ͻðڽ��ϱ�?"), 0, MB_YESNO, DEFAULT_TIME_OUT, FALSE))
			{
				m_pMpe->Write(_T("MB440187"), 0); // �̾��(PC�� On��Ű��, PLC�� Ȯ���ϰ� Off��Ŵ)-������ �������� ����
			}
			else
			{
				if (IDYES == pView->MsgBox(_T("���κο��� 2D �ڵ带 �а� �� �� Last Shot�� Ȯ���Ͻðڽ��ϱ�?"), 0, MB_YESNO, DEFAULT_TIME_OUT, FALSE))
				{
					m_pMpe->Write(_T("MB440187"), 1); // �̾��(PC�� On��Ű��, PLC�� Ȯ���ϰ� Off��Ŵ)-2D �ڵ� �б���� ����
				}
				else
				{
					m_pMpe->Write(_T("MB440187"), 0); // �̾��(PC�� On��Ű��, PLC�� Ȯ���ϰ� Off��Ŵ)-������ �������� ����
				}
			}
		}
		else if (pDoc->GetTestMode() == MODE_INNER && !pDoc->WorkingInfo.LastJob.bDispLotEnd)
		{
			if (IDYES == pView->MsgBox(_T("���κο��� 2D �ڵ带 �а� �� �� Last Shot�� Ȯ���Ͻðڽ��ϱ�?"), 0, MB_YESNO, DEFAULT_TIME_OUT, FALSE))
			{
				m_pMpe->Write(_T("MB440187"), 1); // �̾��(PC�� On��Ű��, PLC�� Ȯ���ϰ� Off��Ŵ)-2D �ڵ� �б���� ����
			}
			else
			{
				if (IDYES == pView->MsgBox(_T("���κο��� ������ ���κ��� �����Ͻðڽ��ϱ�?"), 0, MB_YESNO, DEFAULT_TIME_OUT, FALSE))
				{
					m_pMpe->Write(_T("MB440187"), 0); // �̾��(PC�� On��Ű��, PLC�� Ȯ���ϰ� Off��Ŵ)-������ �������� ����
				}
				else
				{
					m_pMpe->Write(_T("MB440187"), 1); // �̾��(PC�� On��Ű��, PLC�� Ȯ���ϰ� Off��Ŵ)-2D �ڵ� �б���� ����
				}
			}

		}
		else
		{
			m_pMpe->Write(_T("MB440187"), 1); // �̾��(PC�� On��Ű��, PLC�� Ȯ���ϰ� Off��Ŵ)-20141121
		}
		//DispStsBar("�̾��");

		pView->m_nDebugStep = 27; pView->DispThreadTick();
		if (pDoc->m_pReelMap)
			pDoc->m_pReelMap->ClrFixPcs();
		if (pDoc->m_pReelMapUp)
			pDoc->m_pReelMapUp->ClrFixPcs();

		if (bDualTest)
		{
			if (pDoc->m_pReelMapDn)
				pDoc->m_pReelMapDn->ClrFixPcs();
			if (pDoc->m_pReelMapAllUp)
				pDoc->m_pReelMapAllUp->ClrFixPcs();
			if (pDoc->m_pReelMapAllDn)
				pDoc->m_pReelMapAllDn->ClrFixPcs();
		}

		if (pDoc->GetTestMode() == MODE_OUTER)
		{
			if (pDoc->m_pReelMapInnerUp)
				pDoc->m_pReelMapInnerUp->ClrFixPcs();

			if (pDoc->WorkingInfo.LastJob.bDualTestInner)
			{
				if (pDoc->m_pReelMapInnerDn)
					pDoc->m_pReelMapInnerDn->ClrFixPcs();
				if (pDoc->m_pReelMapInnerAllUp)
					pDoc->m_pReelMapInnerAllUp->ClrFixPcs();
				if (pDoc->m_pReelMapInnerAllDn)
					pDoc->m_pReelMapInnerAllDn->ClrFixPcs();
			}
		}

#ifndef TEST_MODE
		ReloadReelmap();
		UpdateRst();
#endif
		DispLotStTime();
		pView->m_nDebugStep = 29; pView->DispThreadTick();
		RestoreReelmap();
	}

	pView->m_nDebugStep = 30; pView->DispThreadTick();

}

void CGvisR2R_PunchView::SetListBuf()	// pDoc->m_ListBuf�� ���� ������ �ø����ȣ�� ������ �簻����.
{
	pDoc->m_ListBuf[0].Clear();
	if (ChkBufUp(m_pBufSerial[0], m_nBufTot[0]))
	{
		for (int i = 0; i<m_nBufTot[0]; i++)
			pDoc->m_ListBuf[0].Push(m_pBufSerial[0][i]);
	}

	pDoc->m_ListBuf[1].Clear();
	if (ChkBufDn(m_pBufSerial[1], m_nBufTot[1]))
	{
		for (int i = 0; i<m_nBufTot[1]; i++)
			pDoc->m_ListBuf[1].Push(m_pBufSerial[1][i]);
	}
}

void CGvisR2R_PunchView::DispLotStTime()
{
	//char szData[MAX_PATH];
	TCHAR szData[MAX_PATH];
	CString sPath = PATH_WORKING_INFO;
	// [Lot]
	if (0 < ::GetPrivateProfileString(_T("Lot"), _T("Start Tick"), NULL, szData, sizeof(szData), sPath))
		pDoc->WorkingInfo.Lot.dwStTick = _tstoi(szData);
	else
		pDoc->WorkingInfo.Lot.dwStTick = 0;

	if (0 < ::GetPrivateProfileString(_T("Lot"), _T("Start Year"), NULL, szData, sizeof(szData), sPath))
		pDoc->WorkingInfo.Lot.StTime.nYear = _tstoi(szData);
	else
		pDoc->WorkingInfo.Lot.StTime.nYear = 0;

	if (0 < ::GetPrivateProfileString(_T("Lot"), _T("Start Month"), NULL, szData, sizeof(szData), sPath))
		pDoc->WorkingInfo.Lot.StTime.nMonth = _tstoi(szData);
	else
		pDoc->WorkingInfo.Lot.StTime.nMonth = 0;

	if (0 < ::GetPrivateProfileString(_T("Lot"), _T("Start Day"), NULL, szData, sizeof(szData), sPath))
		pDoc->WorkingInfo.Lot.StTime.nDay = _tstoi(szData);
	else
		pDoc->WorkingInfo.Lot.StTime.nDay = 0;

	if (0 < ::GetPrivateProfileString(_T("Lot"), _T("Start Hour"), NULL, szData, sizeof(szData), sPath))
		pDoc->WorkingInfo.Lot.StTime.nHour = _tstoi(szData);
	else
		pDoc->WorkingInfo.Lot.StTime.nHour = 0;

	if (0 < ::GetPrivateProfileString(_T("Lot"), _T("Start Minute"), NULL, szData, sizeof(szData), sPath))
		pDoc->WorkingInfo.Lot.StTime.nMin = _tstoi(szData);
	else
		pDoc->WorkingInfo.Lot.StTime.nMin = 0;

	if (0 < ::GetPrivateProfileString(_T("Lot"), _T("Start Second"), NULL, szData, sizeof(szData), sPath))
		pDoc->WorkingInfo.Lot.StTime.nSec = _tstoi(szData);
	else
		pDoc->WorkingInfo.Lot.StTime.nSec = 0;

	m_dwLotSt = (DWORD)pDoc->WorkingInfo.Lot.dwStTick;
	DispLotTime();
}

void CGvisR2R_PunchView::ClrMkInfo()
{
	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;
	pView->m_nDebugStep = 30; pView->DispThreadTick();
	pDoc->ClrPcr();
	pView->m_nDebugStep = 31; pView->DispThreadTick();
	if (pDoc->m_pReelMap)
		pDoc->m_pReelMap->Clear();
	pView->m_nDebugStep = 32; pView->DispThreadTick();
	if (pDoc->m_pReelMapUp)
		pDoc->m_pReelMapUp->Clear();
	if (bDualTest)
	{
		pView->m_nDebugStep = 33; pView->DispThreadTick();
		if (pDoc->m_pReelMapDn)
			pDoc->m_pReelMapDn->Clear();
		pView->m_nDebugStep = 34; pView->DispThreadTick();
		if (pDoc->m_pReelMapAllUp)
			pDoc->m_pReelMapAllUp->Clear();
		pView->m_nDebugStep = 35; pView->DispThreadTick();
		if (pDoc->m_pReelMapAllDn)
			pDoc->m_pReelMapAllDn->Clear();
	}
	pView->m_nDebugStep = 36; pView->DispThreadTick();
	if (m_pDlgMenu01)
	{
		pView->m_nDebugStep = 37; pView->DispThreadTick();
		m_pDlgMenu01->ResetMkInfo();
		pView->m_nDebugStep = 38; pView->DispThreadTick();
		m_pDlgMenu01->SetPnlNum();
		pView->m_nDebugStep = 39; pView->DispThreadTick();
		m_pDlgMenu01->SetPnlDefNum();
		pView->m_nDebugStep = 40; pView->DispThreadTick();
		m_pDlgMenu01->RefreshRmap();
		pView->m_nDebugStep = 41; pView->DispThreadTick();
		m_pDlgMenu01->UpdateRst();
		pView->m_nDebugStep = 42; pView->DispThreadTick();
		//m_pDlgMenu01->UpdateWorking();
		//pView->m_nDebugStep = 43; pView->DispThreadTick();
	}


	CString sLot, sLayerUp, sLayerDn;
	BOOL bDualTestInner;
	if (pDoc->GetCurrentInfoEng())
	{
		if (pDoc->GetTestMode() == MODE_OUTER)
		{
			if (pDoc->GetItsSerialInfo(0, bDualTestInner, sLot, sLayerUp, sLayerDn, 3))
			{
				if (pDoc->m_MasterInner[0].IsMstSpec(pDoc->WorkingInfo.System.sPathCamSpecDir, pDoc->WorkingInfo.LastJob.sModelUp, sLayerUp))
				{
					if (m_pDlgMenu06)
					{
						m_pDlgMenu06->ResetMkInfo();
						//m_pDlgMenu06->SetPnlNum();
						//m_pDlgMenu06->SetPnlDefNum();
						m_pDlgMenu06->RefreshRmap();
						m_pDlgMenu06->UpdateRst();
						//m_pDlgMenu06->UpdateWorking();
					}
				}
			}
		}
	}

	UpdateWorking();
}

void CGvisR2R_PunchView::ModelChange(int nAoi) // 0 : AOI-Up , 1 : AOI-Dn 
{
	if (nAoi == 0)
	{
		pDoc->SetModelInfoUp();
		pView->OpenReelmapUp(); // At Start...
		pView->SetPathAtBufUp();
		if (pView->m_pDlgMenu01)
		{
			pView->m_pDlgMenu01->UpdateData();
			//if (pView->m_nSelRmap == RMAP_UP || pView->m_nSelRmap == RMAP_ALLUP)
			//	pView->m_pDlgMenu01->OpenReelmap(pView->m_nSelRmap);
		}

		pDoc->m_pSpecLocal->MakeDir(pDoc->Status.PcrShare[0].sModel, pDoc->Status.PcrShare[0].sLayer);

		if (pDoc->GetTestMode() == MODE_OUTER)
		{
			OpenReelmapInner();
		}
	}
	else if (nAoi == 1)
	{
		pDoc->SetModelInfoDn();
		pView->OpenReelmapDn(); // At Start...
		pView->SetPathAtBufDn();
		if (pView->m_pDlgMenu01)
		{
			pView->m_pDlgMenu01->UpdateData();
			//if (pView->m_nSelRmap == RMAP_DN || pView->m_nSelRmap == RMAP_ALLDN)
			//	pView->m_pDlgMenu01->OpenReelmap(pView->m_nSelRmap);
		}
		pDoc->m_pSpecLocal->MakeDir(pDoc->Status.PcrShare[1].sModel, pDoc->Status.PcrShare[1].sLayer);
	}
}

void CGvisR2R_PunchView::ResetMkInfo(int nAoi) // 0 : AOI-Up , 1 : AOI-Dn , 2 : AOI-UpDn
{
	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;
	BOOL bDualTestInner, bGetCurrentInfoEng;
	CString sLot, sLayerUp, sLayerDn;
	bGetCurrentInfoEng = GetCurrentInfoEng();

	// CamMst Info...
	pDoc->GetCamPxlRes();

	//if(!pDoc->m_pReelMap)
	//	InitReelmap();

	if (nAoi == 0 || nAoi == 2)
	{
		if (!bDualTest)
		{
			m_bDrawGL_Menu01 = FALSE;
			if (m_pDlgMenu01)
				m_pDlgMenu01->ResetMkInfo();
		}

		if (bGetCurrentInfoEng)
		{
			if (pDoc->GetTestMode() == MODE_OUTER)
			{
				if (pDoc->GetItsSerialInfo(0, bDualTestInner, sLot, sLayerUp, sLayerDn, 3))
				{
					//if (pDoc->GetTestMode() == MODE_OUTER)
					if (pDoc->m_MasterInner[0].IsMstSpec(pDoc->WorkingInfo.System.sPathCamSpecDir, pDoc->WorkingInfo.LastJob.sModelUp, sLayerUp))
					{
						if (!bDualTestInner)
						{
							m_bDrawGL_Menu06 = FALSE;
							if (m_pDlgMenu06)
								m_pDlgMenu06->ResetMkInfo();
						}
					}
				}
			}
		}

		if (IsLastJob(0)) // Up
		{
			pDoc->m_Master[0].Init(pDoc->WorkingInfo.System.sPathCamSpecDir,
				pDoc->WorkingInfo.LastJob.sModelUp,
				pDoc->WorkingInfo.LastJob.sLayerUp);
			pDoc->m_Master[0].LoadMstInfo();
			//pDoc->m_Master[0].WriteStripPieceRegion_Text(pDoc->WorkingInfo.System.sPathOldFile, pDoc->WorkingInfo.LastJob.sLotUp);

			if (m_pEngrave)
				m_pEngrave->SwMenu01UpdateWorking(TRUE);

			if (bGetCurrentInfoEng)
			{
				if (pDoc->GetTestMode() == MODE_OUTER)
				{
					if (pDoc->GetItsSerialInfo(0, bDualTestInner, sLot, sLayerUp, sLayerDn, 0))
					{
						if (pDoc->m_MasterInner[0].IsMstSpec(pDoc->WorkingInfo.System.sPathCamSpecDir, pDoc->WorkingInfo.LastJob.sModelUp, sLayerUp))
						{
							pDoc->m_MasterInner[0].Init(pDoc->WorkingInfo.System.sPathCamSpecDir,
								pDoc->WorkingInfo.LastJob.sModelUp,
								sLayerUp);
							pDoc->m_MasterInner[0].LoadMstInfo();
							//pDoc->m_MasterInner[0].WriteStripPieceRegion_Text(pDoc->WorkingInfo.System.sPathOldFile, pDoc->m_sEngLotNum);

							if (bDualTestInner)
							{
								//GetCurrentInfoEng();
								pDoc->m_MasterInner[1].Init(pDoc->WorkingInfo.System.sPathCamSpecDir,
									pDoc->WorkingInfo.LastJob.sModelUp,
									sLayerDn);
								pDoc->m_MasterInner[1].LoadMstInfo();

								m_bDrawGL_Menu06 = FALSE;
								if (m_pDlgMenu06)
									m_pDlgMenu06->ResetMkInfo();
							}
						}
					}
				}
			}
		}
		else
		{
			pView->ClrDispMsg();
			AfxMessageBox(_T("Error - IsLastJob(0)..."));
		}

		InitReelmapUp();

		if (bGetCurrentInfoEng)
		{
			if (pDoc->GetTestMode() == MODE_OUTER)
			{
				if (pDoc->GetItsSerialInfo(0, bDualTestInner, sLot, sLayerUp, sLayerDn, 0))
				{
					if (pDoc->m_MasterInner[0].IsMstSpec(pDoc->WorkingInfo.System.sPathCamSpecDir, pDoc->WorkingInfo.LastJob.sModelUp, sLayerUp))
					{
						InitReelmapInnerUp();
						if (bDualTestInner)
							InitReelmapInnerDn();
					}
				}
			}
		}

		OpenReelmap();
		SetAlignPosUp();

		if (m_pDlgMenu02)
		{
			m_pDlgMenu02->ChgModelUp();

			if (bDualTest)
				m_pDlgMenu02->ChgModelDn();
		}
		
		if (m_pDlgMenu01)
		{
			m_pDlgMenu01->InitCadImgUp();

			//if (!bDualTest && pDoc->GetTestMode() != MODE_OUTER)
			//{
			//	m_pDlgMenu01->InitGL();
			//	m_bDrawGL = TRUE;
			//	m_pDlgMenu01->RefreshRmap();
			//}
		}

		if (bGetCurrentInfoEng)
		{
			if (pDoc->GetTestMode() == MODE_OUTER)
			{
				if (pDoc->GetItsSerialInfo(0, bDualTestInner, sLot, sLayerUp, sLayerDn, 0))
				{
					if (pDoc->m_MasterInner[0].IsMstSpec(pDoc->WorkingInfo.System.sPathCamSpecDir, pDoc->WorkingInfo.LastJob.sModelUp, sLayerUp))
					{
						if (m_pDlgMenu06)
						{
							m_pDlgMenu06->InitCadImgUp();
							if (bDualTestInner)
								m_pDlgMenu06->InitCadImgDn();

							m_pDlgMenu06->InitGL();
							m_bDrawGL_Menu06 = TRUE;
							m_pDlgMenu06->RefreshRmap();
						}

					}
				}
			}
		}

		if (m_pDlgMenu01)
		{
			if (!bDualTest)
			{
				m_pDlgMenu01->InitGL();
				m_bDrawGL_Menu01 = TRUE;
				m_pDlgMenu01->RefreshRmap();
			}
		}
	}


	if (bDualTest)
	{
		if (nAoi == 1 || nAoi == 2)
		{
			m_bDrawGL_Menu01 = FALSE;
			if (m_pDlgMenu01)
				m_pDlgMenu01->ResetMkInfo();

			//ResetReelmap();

			if (IsLastJob(1)) // Dn
			{
				pDoc->m_Master[1].Init(pDoc->WorkingInfo.System.sPathCamSpecDir,
					pDoc->WorkingInfo.LastJob.sModelUp,
					//pDoc->WorkingInfo.LastJob.sModelDn,
					pDoc->WorkingInfo.LastJob.sLayerDn,
					pDoc->WorkingInfo.LastJob.sLayerUp);

				pDoc->m_Master[1].LoadMstInfo();
				//pDoc->m_Master[1].WriteStripPieceRegion_Text(pDoc->WorkingInfo.System.sPathOldFile, pDoc->WorkingInfo.LastJob.sLotDn);

				//if (pDoc->GetTestMode() == MODE_OUTER)
				//{
				//	//GetCurrentInfoEng();
				//	pDoc->m_MasterInner[1].Init(pDoc->WorkingInfo.System.sPathCamSpecDir,
				//		pDoc->WorkingInfo.LastJob.sModelUp,
				//		pDoc->m_sEngLayerDn);
				//	pDoc->m_MasterInner[1].LoadMstInfo();
				//}
			}
			else
			{
				AfxMessageBox(_T("Error - IsLastJob(1)..."));
			}

			InitReelmapDn();
			SetAlignPosDn();

			if (m_pDlgMenu02)
				m_pDlgMenu02->ChgModelDn();

			if (m_pDlgMenu01)
			{
				m_pDlgMenu01->InitCadImgDn();
				if (nAoi == 1)
				{
					if (bGetCurrentInfoEng)
					{
						if (pDoc->GetTestMode() == MODE_OUTER)
						{
							if (pDoc->GetItsSerialInfo(0, bDualTestInner, sLot, sLayerUp, sLayerDn, 0))
							{
								if (pDoc->m_MasterInner[0].IsMstSpec(pDoc->WorkingInfo.System.sPathCamSpecDir, pDoc->WorkingInfo.LastJob.sModelUp, sLayerUp))
								{
									if (m_pDlgMenu06)
									{
										m_pDlgMenu06->InitCadImgUp();
										if (bDualTestInner)
											m_pDlgMenu06->InitCadImgDn();

										m_pDlgMenu06->InitGL();
										m_bDrawGL_Menu06 = TRUE;
										m_pDlgMenu06->RefreshRmap();
									}

								}
							}
						}
					}

					m_pDlgMenu01->InitGL();
					m_bDrawGL_Menu01 = TRUE;
					m_pDlgMenu01->RefreshRmap();
				}
			}

			// 		m_bDrawGL = TRUE;
		}
	}
	// 	else
	// 	{
	// 		if(m_pDlgMenu01)
	// 			m_pDlgMenu01->ResetMkInfo();
	// 	}

}

void CGvisR2R_PunchView::SetAlignPos()
{
	if (m_pMotion)
	{
		m_pMotion->m_dAlignPosX[0][0] = pDoc->m_Master[0].m_stAlignMk.X0 + pView->m_pMotion->m_dPinPosX[0];
		m_pMotion->m_dAlignPosY[0][0] = pDoc->m_Master[0].m_stAlignMk.Y0 + pView->m_pMotion->m_dPinPosY[0];
		m_pMotion->m_dAlignPosX[0][1] = pDoc->m_Master[0].m_stAlignMk.X1 + pView->m_pMotion->m_dPinPosX[0];
		m_pMotion->m_dAlignPosY[0][1] = pDoc->m_Master[0].m_stAlignMk.Y1 + pView->m_pMotion->m_dPinPosY[0];

		m_pMotion->m_dAlignPosX[1][0] = pDoc->m_Master[0].m_stAlignMk.X0 + pView->m_pMotion->m_dPinPosX[1];
		m_pMotion->m_dAlignPosY[1][0] = pDoc->m_Master[0].m_stAlignMk.Y0 + pView->m_pMotion->m_dPinPosY[1];
		m_pMotion->m_dAlignPosX[1][1] = pDoc->m_Master[0].m_stAlignMk.X1 + pView->m_pMotion->m_dPinPosX[1];
		m_pMotion->m_dAlignPosY[1][1] = pDoc->m_Master[0].m_stAlignMk.Y1 + pView->m_pMotion->m_dPinPosY[1];
	}
}

void CGvisR2R_PunchView::SetAlignPosUp()
{
	if (m_pMotion)
	{
		m_pMotion->m_dAlignPosX[0][0] = pDoc->m_Master[0].m_stAlignMk.X0 + pView->m_pMotion->m_dPinPosX[0];
		m_pMotion->m_dAlignPosY[0][0] = pDoc->m_Master[0].m_stAlignMk.Y0 + pView->m_pMotion->m_dPinPosY[0];
		m_pMotion->m_dAlignPosX[0][1] = pDoc->m_Master[0].m_stAlignMk.X1 + pView->m_pMotion->m_dPinPosX[0];
		m_pMotion->m_dAlignPosY[0][1] = pDoc->m_Master[0].m_stAlignMk.Y1 + pView->m_pMotion->m_dPinPosY[0];
	}
}

void CGvisR2R_PunchView::SetAlignPosDn()
{
	if (m_pMotion)
	{
		m_pMotion->m_dAlignPosX[1][0] = pDoc->m_Master[0].m_stAlignMk.X0 + pView->m_pMotion->m_dPinPosX[1];
		m_pMotion->m_dAlignPosY[1][0] = pDoc->m_Master[0].m_stAlignMk.Y0 + pView->m_pMotion->m_dPinPosY[1];
		m_pMotion->m_dAlignPosX[1][1] = pDoc->m_Master[0].m_stAlignMk.X1 + pView->m_pMotion->m_dPinPosX[1];
		m_pMotion->m_dAlignPosY[1][1] = pDoc->m_Master[0].m_stAlignMk.Y1 + pView->m_pMotion->m_dPinPosY[1];
	}
}

BOOL CGvisR2R_PunchView::InitMk()
{
	m_nStepElecChk = 0;
	int nRSer = ChkSerial(); // (nSerial0 - nLastShot) -> 0: Same Serial, -: Decrese Serial, +: Increase Serial 
	int nSerial = GetBuffer0();

	if (nRSer)
	{
		// syd - 20130224 : ���� �ʿ�
		//if(nRSer < 0)
		//	m_bSerialDecrese = TRUE;
		//else
			m_bSerialDecrese = FALSE;

		if (pDoc->m_bUseRTRYShiftAdjust)
		{
			if (nSerial > 2)
				AdjPinPos();
		}

		return TRUE;
	}
	//if (!nRSer) // 0: Continue, -: Previous, +: Discontinue
	//{
	//	if (pDoc->m_bUseRTRYShiftAdjust)
	//	{
	//		if (nSerial > 2)
	//			AdjPinPos();
	//	}

	//	return TRUE;
	//}
	//else if (nRSer<0)
	//{
	//	if (nSerial > 2)
	//	{
	//		ReloadReelmap(nSerial - 1);
	//		UpdateRst();
	//	}
	//	return TRUE;
	//}

	return FALSE;
}

int CGvisR2R_PunchView::GetErrCode(int nSerial) // 1(����), -1(Align Error, �뱤�ҷ�), -2(Lot End)
{
	if (nSerial <= 0)
	{
		pView->ClrDispMsg();
		AfxMessageBox(_T("Serial Error.29"));
		return 0;
	}

#ifdef USE_CAM_MASTER
	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;

	int nErr[2];
	nErr[0] = GetErrCodeUp(nSerial);
	if (nErr[0] != 1)
		return nErr[0];
	if (bDualTest)
	{
		nErr[1] = GetErrCodeDn(nSerial);
		if (nErr[1] != 1)
			return nErr[1];
	}
#endif

	return 1;
}

int CGvisR2R_PunchView::GetErrCodeUp(int nSerial) // 1(����), -1(Align Error, �뱤�ҷ�), -2(Lot End)
{
	if (nSerial <= 0)
	{
		pView->ClrDispMsg();
		AfxMessageBox(_T("Serial Error.30"));
		return 0;
	}

	int nIdx = pDoc->GetPcrIdx0(nSerial);
	int nErrCode = 1;

#ifdef USE_CAM_MASTER
	if (pDoc->m_pPcr[0])
	{
		if (pDoc->m_pPcr[0][nIdx])
			nErrCode = pDoc->m_pPcr[0][nIdx]->m_nErrPnl;
		else
			return 2;	// Code Setting Error.
	}
	else
		return 2;	// Code Setting Error.
#endif

	return nErrCode;
}

int CGvisR2R_PunchView::GetErrCodeDn(int nSerial) // 1(����), -1(Align Error, �뱤�ҷ�), -2(Lot End)
{
	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;
	if (!bDualTest)
		return 1;

	if (nSerial <= 0)
	{
		AfxMessageBox(_T("Serial Error.31"));
		return 0;
	}

	int nIdx = pDoc->GetPcrIdx1(nSerial);
	int nErrCode = 1;

#ifdef USE_CAM_MASTER
	if (pDoc->m_pPcr[1])
	{
		if (pDoc->m_pPcr[1][nIdx])
			nErrCode = pDoc->m_pPcr[1][nIdx]->m_nErrPnl;
	}
#endif

	return nErrCode;
}


int CGvisR2R_PunchView::GetErrCode0(int nSerial) // 1(����), -1(Align Error, �뱤�ҷ�), -2(Lot End)
{
	if (nSerial <= 0)
	{
		pView->ClrDispMsg();
		AfxMessageBox(_T("Serial Error.32"));
		return 0;
	}

	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;

#ifdef USE_CAM_MASTER
	int nErr[2];
	nErr[0] = GetErrCodeUp0(nSerial);
	if (nErr[0] != 1)
		return nErr[0];
	if (bDualTest)
	{
		nErr[1] = GetErrCodeDn0(nSerial);
		if (nErr[1] != 1)
			return nErr[1];
	}
#endif

	return 1;
}

int CGvisR2R_PunchView::GetErrCodeUp0(int nSerial) // 1(����), -1(Align Error, �뱤�ҷ�), -2(Lot End)
{
	if (nSerial <= 0)
	{
		pView->ClrDispMsg();
		AfxMessageBox(_T("Serial Error.33"));
		return 0;
	}

	int nIdx = pDoc->GetPcrIdx0(nSerial);
	int nErrCode = 1;

#ifdef USE_CAM_MASTER
	if (pDoc->m_pPcr[0])
	{
		if (pDoc->m_pPcr[0][nIdx])
			nErrCode = pDoc->m_pPcr[0][nIdx]->m_nErrPnl;
		else
			return 2;	// Code Setting Error.
	}
	else
		return 2;	// Code Setting Error.
#endif

	return nErrCode;
}

int CGvisR2R_PunchView::GetErrCodeDn0(int nSerial) // 1(����), -1(Align Error, �뱤�ҷ�), -2(Lot End)
{
	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;
	if (!bDualTest)
		return 1;

	if (nSerial <= 0)
	{
		pView->ClrDispMsg();
		AfxMessageBox(_T("Serial Error.34"));
		return 0;
	}

	int nIdx = pDoc->GetPcrIdx0(nSerial);
	int nErrCode = 1;

#ifdef USE_CAM_MASTER
	if (pDoc->m_pPcr[1])
	{
		if (pDoc->m_pPcr[1][nIdx])
			nErrCode = pDoc->m_pPcr[1][nIdx]->m_nErrPnl;
	}
#endif

	return nErrCode;
}


int CGvisR2R_PunchView::GetErrCode1(int nSerial) // 1(����), -1(Align Error, �뱤�ҷ�), -2(Lot End)
{
	if (nSerial <= 0)
	{
		pView->ClrDispMsg();
		AfxMessageBox(_T("Serial Error.35"));
		return 0;
	}

#ifdef USE_CAM_MASTER
	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;

	int nErr[2];
	nErr[0] = GetErrCodeUp1(nSerial);
	if (nErr[0] != 1)
		return nErr[0];

	if (bDualTest)
	{
		nErr[1] = GetErrCodeDn1(nSerial);
		if (nErr[1] != 1)
			return nErr[1];
	}
#endif

	return 1;
}

int CGvisR2R_PunchView::GetErrCodeUp1(int nSerial) // 1(����), -1(Align Error, �뱤�ҷ�), -2(Lot End)
{
	if (nSerial <= 0)
	{
		pView->ClrDispMsg();
		AfxMessageBox(_T("Serial Error.36"));
		return 0;
	}

	int nIdx = pDoc->GetPcrIdx1(nSerial);
	int nErrCode = 1;

#ifdef USE_CAM_MASTER
	if (pDoc->m_pPcr[0])
	{
		if (pDoc->m_pPcr[0][nIdx])
			nErrCode = pDoc->m_pPcr[0][nIdx]->m_nErrPnl;
		else
			return 2;	// Code Setting Error.
	}
	else
		return 2;	// Code Setting Error.
#endif

	return nErrCode;
}

int CGvisR2R_PunchView::GetErrCodeDn1(int nSerial) // 1(����), -1(Align Error, �뱤�ҷ�), -2(Lot End)
{
	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;
	if (!bDualTest)
		return 1;

	if (nSerial <= 0)
	{
		pView->ClrDispMsg();
		AfxMessageBox(_T("Serial Error.37"));
		return 0;
	}

	int nIdx = pDoc->GetPcrIdx1(nSerial);
	int nErrCode = 1;

#ifdef USE_CAM_MASTER
	if (pDoc->m_pPcr[1])
	{
		if (pDoc->m_pPcr[1][nIdx])
			nErrCode = pDoc->m_pPcr[1][nIdx]->m_nErrPnl;
		else
			return 2;	// Code Setting Error.
	}
	else
		return 2;	// Code Setting Error.
#endif

	return nErrCode;
}


int CGvisR2R_PunchView::GetTotDefPcs(int nSerial)
{
	if (nSerial <= 0)
	{
		pView->ClrDispMsg();
		AfxMessageBox(_T("Serial Error.38"));
		return 0;
	}

	int nTotDef = 0;
#ifdef USE_CAM_MASTER
	nTotDef = GetTotDefPcsUp(nSerial) + GetTotDefPcsDn(nSerial);
#else
	nTotDef = 1;
#endif

	return nTotDef;
}

int CGvisR2R_PunchView::GetTotDefPcsUp(int nSerial)
{
	if (nSerial <= 0)
	{
		pView->ClrDispMsg();
		AfxMessageBox(_T("Serial Error.39"));
		return 0;
	}

	int nIdx = pDoc->GetPcrIdx0(nSerial);
	int nTotDef = 0;

#ifdef USE_CAM_MASTER
	if (pDoc->m_pPcr[0])
	{
		if (pDoc->m_pPcr[0][nIdx])
			nTotDef = pDoc->m_pPcr[0][nIdx]->m_nTotDef;
	}
#else
	nTotDef = 1;
#endif

	return nTotDef;
}

int CGvisR2R_PunchView::GetTotDefPcsDn(int nSerial)
{
	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;
	if (!bDualTest)
		return 0;

	if (nSerial <= 0)
	{
		pView->ClrDispMsg();
		AfxMessageBox(_T("Serial Error.40"));
		return 0;
	}

	int nIdx = pDoc->GetPcrIdx1(nSerial);
	int nTotDef = 0;

#ifdef USE_CAM_MASTER
	if (pDoc->m_pPcr[1])
	{
		if (pDoc->m_pPcr[1][nIdx])
			nTotDef = pDoc->m_pPcr[1][nIdx]->m_nTotDef;
	}
#else
	nTotDef = 1;
#endif

	return nTotDef;
}


int CGvisR2R_PunchView::GetTotDefPcs0(int nSerial)
{
	if (nSerial <= 0)
	{
		pView->ClrDispMsg();
		AfxMessageBox(_T("Serial Error.41"));
		return 0;
	}

	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;

	int nIdx = pDoc->GetPcrIdx0(nSerial);
	int nTotDef = 0;

#ifdef USE_CAM_MASTER
	if (bDualTest)
	{
		if (pDoc->m_pPcr[2])
		{
			if (pDoc->m_pPcr[2][nIdx])
				nTotDef = pDoc->m_pPcr[2][nIdx]->m_nTotDef;
		}
	}
	else
	{
		if (pDoc->m_pPcr[0])
		{
			if (pDoc->m_pPcr[0][nIdx])
				nTotDef = pDoc->m_pPcr[0][nIdx]->m_nTotDef;
		}
	}
#else
	nTotDef = 1;
#endif

	return nTotDef;
}

int CGvisR2R_PunchView::GetTotDefPcsUp0(int nSerial)
{
	if (nSerial <= 0)
	{
		pView->ClrDispMsg();
		AfxMessageBox(_T("Serial Error.42"));
		return 0;
	}

	int nIdx = pDoc->GetPcrIdx0(nSerial);
	int nTotDef = 0;

#ifdef USE_CAM_MASTER
	if (pDoc->m_pPcr[0])
	{
		if (pDoc->m_pPcr[0][nIdx])
			nTotDef = pDoc->m_pPcr[0][nIdx]->m_nTotDef;
	}
#else
	nTotDef = 1;
#endif

	return nTotDef;
}

int CGvisR2R_PunchView::GetTotDefPcsDn0(int nSerial)
{
	if (nSerial <= 0)
	{
		pView->ClrDispMsg();
		AfxMessageBox(_T("Serial Error.43"));
		return 0;
	}

	int nIdx = pDoc->GetPcrIdx0(nSerial);
	int nTotDef = 0;

#ifdef USE_CAM_MASTER
	if (pDoc->m_pPcr[1])
	{
		if (pDoc->m_pPcr[1][nIdx])
			nTotDef = pDoc->m_pPcr[1][nIdx]->m_nTotDef;
	}
#else
	nTotDef = 1;
#endif

	return nTotDef;
}


int CGvisR2R_PunchView::GetTotDefPcs1(int nSerial)
{
	if (nSerial <= 0)
	{
		pView->ClrDispMsg();
		AfxMessageBox(_T("Serial Error.44"));
		return 0;
	}

	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;
	int nIdx = pDoc->GetPcrIdx1(nSerial);
	int nTotDef = 0;

#ifdef USE_CAM_MASTER
	if (bDualTest)
	{
		if (pDoc->m_pPcr[2])
		{
			if (pDoc->m_pPcr[2][nIdx])
				nTotDef = pDoc->m_pPcr[2][nIdx]->m_nTotDef;
		}
	}
	else
	{
		if (pDoc->m_pPcr[0])
		{
			if (pDoc->m_pPcr[0][nIdx])
				nTotDef = pDoc->m_pPcr[0][nIdx]->m_nTotDef;
		}
	}
#else
	nTotDef = 1;
#endif

	return nTotDef;
}

int CGvisR2R_PunchView::GetTotDefPcsUp1(int nSerial)
{
	if (nSerial <= 0)
	{
		pView->ClrDispMsg();
		AfxMessageBox(_T("Serial Error.45"));
		return 0;
	}

	int nIdx = pDoc->GetPcrIdx1(nSerial);
	int nTotDef = 0;

#ifdef USE_CAM_MASTER
	if (pDoc->m_pPcr[0])
	{
		if (pDoc->m_pPcr[0][nIdx])
			nTotDef = pDoc->m_pPcr[0][nIdx]->m_nTotDef;
	}
#else
	nTotDef = 1;
#endif

	return nTotDef;
}

int CGvisR2R_PunchView::GetTotDefPcsDn1(int nSerial)
{
	if (nSerial <= 0)
	{
		pView->ClrDispMsg();
		AfxMessageBox(_T("Serial Error.46"));
		return 0;
	}

	int nIdx = pDoc->GetPcrIdx1(nSerial);
	int nTotDef = 0;

#ifdef USE_CAM_MASTER
	if (pDoc->m_pPcr[1])
	{
		if (pDoc->m_pPcr[1][nIdx])
			nTotDef = pDoc->m_pPcr[1][nIdx]->m_nTotDef;
	}
#else
	nTotDef = 1;
#endif

	return nTotDef;
}


CfPoint CGvisR2R_PunchView::GetMkPnt(int nMkPcs)
{
	CfPoint ptPnt;
	ptPnt.x = -1.0;
	ptPnt.y = -1.0;

#ifdef USE_CAM_MASTER
	if (pDoc->m_Master[0].m_pPcsRgn)
		ptPnt = pDoc->m_Master[0].m_pPcsRgn->GetMkPnt(nMkPcs); // Cam0�� Mk ����Ʈ.
#else
	ptPnt.x = 1.0;
	ptPnt.y = 1.0;
#endif

	return ptPnt;
}

CfPoint CGvisR2R_PunchView::GetMkPnt0(int nMkPcs)
{
	CfPoint ptPnt;
	ptPnt.x = -1.0;
	ptPnt.y = -1.0;

#ifdef USE_CAM_MASTER
	if (pDoc->m_Master[0].m_pPcsRgn)
		ptPnt = pDoc->m_Master[0].m_pPcsRgn->GetMkPnt0(nMkPcs); // Cam0�� Mk ����Ʈ.
#else
	ptPnt.x = 1.0;
	ptPnt.y = 1.0;
#endif

	return ptPnt;
}

CfPoint CGvisR2R_PunchView::GetMkPnt1(int nMkPcs) // pcr �ҷ� �ǽ� ���� ���� �ε���
{
	CfPoint ptPnt;
	ptPnt.x = -1.0;
	ptPnt.y = -1.0;

#ifdef USE_CAM_MASTER
	if (pDoc->m_Master[0].m_pPcsRgn)
		ptPnt = pDoc->m_Master[0].m_pPcsRgn->GetMkPnt1(nMkPcs); // Cam1�� Mk ����Ʈ.
#else
	ptPnt.x = 1.0;
	ptPnt.y = 1.0;
#endif

	return ptPnt;
}

BOOL CGvisR2R_PunchView::CheckMkPnt()
{
	if (GetMkPnt(0).x == GetMkPnt(1).x && GetMkPnt(0).y == GetMkPnt(1).y)
		return FALSE;

	return TRUE;
}

// CfPoint CGvisR2R_PunchView::GetMkPnt(int nSerial, int nMkPcs)
// {
// 	if(nSerial <= 0)
// 	{
// 		AfxMessageBox(_T("Serial Error."));
// 		return 0;
// 	}
// 
// 	int nIdx = pDoc->GetPcrIdx(nSerial);
// 	int nDefPcsId;
// 	CfPoint ptPnt;
// 	ptPnt.x = -1.0;
// 	ptPnt.y = -1.0;
// 	if(pDoc->m_pPcr[2])	// [0]:AOI-Up , [1]:AOI-Dn , [2]:AOI-AllUp , [3]:AOI-AllDn
// 	{
// 		if(pDoc->m_pPcr[2][nIdx])
// 		{
// 			if(pDoc->m_pPcr[2][nIdx]->m_pDefPcs)
// 			{
// 				if(pDoc->m_pPcr[2][nIdx]->m_pMk[nMkPcs] != -2) // -2 (NoMarking)
// 				{
// 					nDefPcsId = pDoc->m_pPcr[2][nIdx]->m_pDefPcs[nMkPcs];
// 					if(pDoc->m_Master[0].m_pPcsRgn)
// 						ptPnt = pDoc->m_Master[0].m_pPcsRgn->GetMkPnt(nDefPcsId); // Cam0�� Mk ����Ʈ.
// 				}
// 			}
// 		}
// 	}
// 	return ptPnt;
// }

CfPoint CGvisR2R_PunchView::GetMkPnt0(int nSerial, int nMkPcs) // pcr �ø���, pcr �ҷ� �ǽ� ���� ���� �ε���
{
	if (nSerial <= 0)
	{
		pView->ClrDispMsg();
		AfxMessageBox(_T("Serial Error.47"));
		return 0;
	}

	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;
	int nIdx = pDoc->GetPcrIdx0(nSerial);
	CfPoint ptPnt;
	ptPnt.x = -1.0;
	ptPnt.y = -1.0;

#ifdef USE_CAM_MASTER
	int nDefPcsId = 0;

	if (bDualTest)
	{
		if (pDoc->m_pPcr[2])	// [0]:AOI-Up , [1]:AOI-Dn , [2]:AOI-AllUp , [3]:AOI-AllDn
		{
			if (pDoc->m_pPcr[2][nIdx])
			{
				if (pDoc->m_pPcr[2][nIdx]->m_pDefPcs)
				{
					if (pDoc->m_pPcr[2][nIdx]->m_pMk[nMkPcs] != -2) // -2 (NoMarking)
					{
						nDefPcsId = pDoc->m_pPcr[2][nIdx]->m_pDefPcs[nMkPcs];
						if (pDoc->m_Master[0].m_pPcsRgn)
							ptPnt = pDoc->m_Master[0].m_pPcsRgn->GetMkPnt0(nDefPcsId); // Cam0�� Mk ����Ʈ.
					}
				}
			}
		}
	}
	else
	{
		if (pDoc->m_pPcr[0])	// [0]:AOI-Up , [1]:AOI-Dn , [2]:AOI-AllUp , [3]:AOI-AllDn
		{
			if (pDoc->m_pPcr[0][nIdx])
			{
				if (pDoc->m_pPcr[0][nIdx]->m_pDefPcs)
				{
					if (pDoc->m_pPcr[0][nIdx]->m_pMk[nMkPcs] != -2) // -2 (NoMarking)
					{
						nDefPcsId = pDoc->m_pPcr[0][nIdx]->m_pDefPcs[nMkPcs];
						if (pDoc->m_Master[0].m_pPcsRgn)
							ptPnt = pDoc->m_Master[0].m_pPcsRgn->GetMkPnt0(nDefPcsId); // Cam0�� Mk ����Ʈ.
					}
				}
			}
		}
	}
#else
	ptPnt.x = 1.0;
	ptPnt.y = 1.0;
#endif

	return ptPnt;
}

int CGvisR2R_PunchView::GetMkStripIdx0(int nDefPcsId) // 0 : Fail , 1~4 : Strip Idx
{
	int nNodeX = pDoc->m_Master[0].m_pPcsRgn->nCol;
	int nNodeY = pDoc->m_Master[0].m_pPcsRgn->nRow;
	int nStripY = int(nNodeY / 4);
	int nStripIdx = 0; 

#ifdef USE_CAM_MASTER
	int nRow = 0, nNum = 0, nMode = 0;
	nNum = int(nDefPcsId / nNodeY);
	nMode = nDefPcsId % nNodeY;
	if (nNum % 2) 	// Ȧ��.
		nRow = nNodeY - (nMode + 1);
	else		// ¦��.
		nRow = nMode;

	nStripIdx = int(nRow / nStripY) + 1;
#else
	nStripIdx = 1;
#endif

	return nStripIdx;
}

int CGvisR2R_PunchView::GetMkStripIdx1(int nDefPcsId) // 0 : Fail , 1~4 : Strip Idx
{
	int nNodeX = pDoc->m_Master[0].m_pPcsRgn->nCol;
	int nNodeY = pDoc->m_Master[0].m_pPcsRgn->nRow;
	int nStripY = int(nNodeY / 4);
	int nStripIdx = 0;

#ifdef USE_CAM_MASTER
	int nRow = 0, nNum = 0, nMode = 0;
	nNum = int(nDefPcsId / nNodeY);
	nMode = nDefPcsId % nNodeY;
	if (nNum % 2) 	// Ȧ��.
		nRow = nNodeY - (nMode + 1);
	else		// ¦��.
		nRow = nMode;

	nStripIdx = int(nRow / nStripY) + 1;
#else
	nStripIdx = 1;
#endif

	return nStripIdx;
}

CString CGvisR2R_PunchView::GetMkInfo0(int nSerial, int nMkPcs) // return Cam0 : "Serial_Strip_Col_Row"
{
	if (nSerial <= 0)
	{
		AfxMessageBox(_T("Serial Error.48"));
		return 0;
	}

	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;

	CString sInfo;
	int nStrip = -1, nCol = -1, nRow = -1;
	int nPcsIdx, nDefCode;
	int nPcrIdx = pDoc->GetPcrIdx0(nSerial);
	if (bDualTest)
	{
		nPcsIdx = pDoc->m_pPcr[2][nPcrIdx]->m_pDefPcs[nMkPcs];
		nDefCode = pDoc->m_pPcr[2][nPcrIdx]->m_pDefType[nMkPcs];
	}
	else
	{
		nPcsIdx = pDoc->m_pPcr[0][nPcrIdx]->m_pDefPcs[nMkPcs];
		nDefCode = pDoc->m_pPcr[0][nPcrIdx]->m_pDefType[nMkPcs];
	}

	if (pDoc->m_Master[0].m_pPcsRgn)
		pDoc->m_Master[0].m_pPcsRgn->GetMkMatrix(nPcsIdx, nStrip, nCol, nRow);

	sInfo.Format(_T("%04d_%c_%d_%d"), nSerial, nStrip + 'A', nCol + 1, nRow + 1);

	return sInfo;
}

CString CGvisR2R_PunchView::GetMkInfo1(int nSerial, int nMkPcs) // return Cam1 : "Serial_Strip_Col_Row"
{
	if (nSerial <= 0)
	{
		AfxMessageBox(_T("Serial Error.48"));
		return 0;
	}

	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;

	CString sInfo;
	int nPcsIdx, nDefCode;
	int nStrip = -1, nCol = -1, nRow = -1;
	int nPcrIdx = pDoc->GetPcrIdx1(nSerial);
	if (bDualTest)
	{
		nPcsIdx = pDoc->m_pPcr[2][nPcrIdx]->m_pDefPcs[nMkPcs];
		nDefCode = pDoc->m_pPcr[2][nPcrIdx]->m_pDefType[nMkPcs];
	}
	else
	{
		nPcsIdx = pDoc->m_pPcr[0][nPcrIdx]->m_pDefPcs[nMkPcs];
		nDefCode = pDoc->m_pPcr[0][nPcrIdx]->m_pDefType[nMkPcs];
	}
	if (pDoc->m_Master[0].m_pPcsRgn)
		pDoc->m_Master[0].m_pPcsRgn->GetMkMatrix(nPcsIdx, nStrip, nCol, nRow);

	sInfo.Format(_T("%04d_%c_%d_%d"), nSerial, nStrip + 'A', nCol + 1, nRow + 1);

	return sInfo;
}


int CGvisR2R_PunchView::GetMkStripIdx0(int nSerial, int nMkPcs) // 0 : Fail , 1~4 : Strip Idx
{
	if (nSerial <= 0)
	{
		AfxMessageBox(_T("Serial Error.48"));
		return 0;
	}

	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;
	int nIdx = pDoc->GetPcrIdx0(nSerial);
	int nNodeX = pDoc->m_Master[0].m_pPcsRgn->nCol;
	int nNodeY = pDoc->m_Master[0].m_pPcsRgn->nRow;
	int nStripY = int(nNodeY / MAX_STRIP_NUM);
	int nStripIdx = 0;

#ifdef USE_CAM_MASTER
	int nDefPcsId = 0, nNum = 0, nMode = 0, nRow = 0;

	if (bDualTest)
	{
		if (pDoc->m_pPcr[2])	// [0]:AOI-Up , [1]:AOI-Dn , [2]:AOI-AllUp , [3]:AOI-AllDn
		{
			if (pDoc->m_pPcr[2][nIdx])
			{
				if (pDoc->m_pPcr[2][nIdx]->m_pDefPcs)
				{
					nDefPcsId = pDoc->m_pPcr[2][nIdx]->m_pDefPcs[nMkPcs];
					nNum = int(nDefPcsId / nNodeY);
					nMode = nDefPcsId % nNodeY;
					if (nNum % 2) 	// Ȧ��.
						nRow = nNodeY - (nMode + 1);
					else		// ¦��.
						nRow = nMode;

					nStripIdx = int(nRow / nStripY) + 1;
				}
			}
		}
	}
	else
	{
		if (pDoc->m_pPcr[0])	// [0]:AOI-Up , [1]:AOI-Dn , [2]:AOI-AllUp , [3]:AOI-AllDn
		{
			if (pDoc->m_pPcr[0][nIdx])
			{
				if (pDoc->m_pPcr[0][nIdx]->m_pDefPcs)
				{
					nDefPcsId = pDoc->m_pPcr[0][nIdx]->m_pDefPcs[nMkPcs];
					nNum = int(nDefPcsId / nNodeY);
					nMode = nDefPcsId % nNodeY;
					if (nNum % 2) 	// Ȧ��.
						nRow = nNodeY - (nMode + 1);
					else		// ¦��.
						nRow = nMode;

					nStripIdx = int(nRow / nStripY) + 1;
				}
			}
		}
	}
#else
	nStripIdx = 1;
#endif

	return nStripIdx;
}

CfPoint CGvisR2R_PunchView::GetMkPnt1(int nSerial, int nMkPcs)
{
	if (nSerial <= 0)
	{
		pView->ClrDispMsg();
		AfxMessageBox(_T("Serial Error.50"));
		return 0;
	}

	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;
	int nIdx = pDoc->GetPcrIdx1(nSerial);
	CfPoint ptPnt;
	ptPnt.x = -1.0;
	ptPnt.y = -1.0;

#ifdef USE_CAM_MASTER
	int nDefPcsId = 0;
	if (bDualTest)
	{
		if (pDoc->m_pPcr[2])	// [0]:AOI-Up , [1]:AOI-Dn , [2]:AOI-AllUp , [3]:AOI-AllDn
		{
			if (pDoc->m_pPcr[2][nIdx])
			{
				if (pDoc->m_pPcr[2][nIdx]->m_pDefPcs)
				{
					if (pDoc->m_pPcr[2][nIdx]->m_pMk[nMkPcs] != -2) // -2 (NoMarking)
					{
						nDefPcsId = pDoc->m_pPcr[2][nIdx]->m_pDefPcs[nMkPcs];
						if (pDoc->m_Master[0].m_pPcsRgn)
						{
							ptPnt = pDoc->m_Master[0].m_pPcsRgn->GetMkPnt1(nDefPcsId); // Cam1�� Mk ����Ʈ.
						}
					}
				}
			}
		}
	}
	else
	{
		if (pDoc->m_pPcr[0])	// [0]:AOI-Up , [1]:AOI-Dn , [2]:AOI-AllUp , [3]:AOI-AllDn
		{
			if (pDoc->m_pPcr[0][nIdx])
			{
				if (pDoc->m_pPcr[0][nIdx]->m_pDefPcs)
				{
					if (pDoc->m_pPcr[0][nIdx]->m_pMk[nMkPcs] != -2) // -2 (NoMarking)
					{
						nDefPcsId = pDoc->m_pPcr[0][nIdx]->m_pDefPcs[nMkPcs];
						if (pDoc->m_Master[0].m_pPcsRgn)
						{
							ptPnt = pDoc->m_Master[0].m_pPcsRgn->GetMkPnt1(nDefPcsId); // Cam1�� Mk ����Ʈ.
						}
					}
				}
			}
		}
	}
#else
	ptPnt.x = 1.0;
	ptPnt.y = 1.0;
#endif

	return ptPnt;
}

int CGvisR2R_PunchView::GetMkStripIdx1(int nSerial, int nMkPcs) // 0 : Fail , 1~4 : Strip Idx
{
	if (nSerial <= 0)
	{
		pView->ClrDispMsg();
		AfxMessageBox(_T("Serial Error.51"));
		return 0;
	}

	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;
	int nIdx = pDoc->GetPcrIdx1(nSerial);
	int nNodeX = pDoc->m_Master[0].m_pPcsRgn->nCol;
	int nNodeY = pDoc->m_Master[0].m_pPcsRgn->nRow;
	int nStripY = int(nNodeY / MAX_STRIP_NUM);
	int nStripIdx = 0;

#ifdef USE_CAM_MASTER
	int nDefPcsId = 0, nNum = 0, nMode = 0, nRow = 0;
	if (bDualTest)
	{
		if (pDoc->m_pPcr[2])	// [0]:AOI-Up , [1]:AOI-Dn , [2]:AOI-AllUp , [3]:AOI-AllDn
		{
			if (pDoc->m_pPcr[2][nIdx])
			{
				if (pDoc->m_pPcr[2][nIdx]->m_pDefPcs)
				{
					nDefPcsId = pDoc->m_pPcr[2][nIdx]->m_pDefPcs[nMkPcs];
					nNum = int(nDefPcsId / nNodeY);
					nMode = nDefPcsId % nNodeY;
					if (nNum % 2) 	// Ȧ��.
						nRow = nNodeY - (nMode + 1);
					else		// ¦��.
						nRow = nMode;

					nStripIdx = int(nRow / nStripY) + 1;
				}
			}
		}
	}
	else
	{
		if (pDoc->m_pPcr[0])	// [0]:AOI-Up , [1]:AOI-Dn , [2]:AOI-AllUp , [3]:AOI-AllDn
		{
			if (pDoc->m_pPcr[0][nIdx])
			{
				if (pDoc->m_pPcr[0][nIdx]->m_pDefPcs)
				{
					nDefPcsId = pDoc->m_pPcr[0][nIdx]->m_pDefPcs[nMkPcs];
					nNum = int(nDefPcsId / nNodeY);
					nMode = nDefPcsId % nNodeY;
					if (nNum % 2) 	// Ȧ��.
						nRow = nNodeY - (nMode + 1);
					else		// ¦��.
						nRow = nMode;

					nStripIdx = int(nRow / nStripY) + 1;
				}
			}
		}
	}
#else
	nStripIdx = 1;
#endif

	return nStripIdx;
}

void CGvisR2R_PunchView::Move0(CfPoint pt, BOOL bCam)
{
	if (m_bProbDn[0])
	{
		if (m_pVoiceCoil[0])
		{
			m_pVoiceCoil[0]->SearchHomeSmac(0);
			m_pVoiceCoil[0]->MoveSmacShiftPos(0);
			m_bProbDn[0] = FALSE;
			//if (m_pDlgMenu02->m_pDlgUtil06)
			//	m_pDlgMenu02->m_pDlgUtil06->myBtn[2].SetCheck(FALSE);
		}
	}

	double fLen, fVel, fAcc, fJerk;

	double pPos[2];

	if (pDoc->WorkingInfo.System.bNoMk || bCam)
	{
		pPos[0] = pt.x;
		pPos[1] = pt.y;
	}
	else
	{
		pPos[0] = pt.x + _tstof(pDoc->WorkingInfo.Vision[0].sMkOffsetX);
		pPos[1] = pt.y + _tstof(pDoc->WorkingInfo.Vision[0].sMkOffsetY);
	}

	if (pPos[0] < 0.0)
		pPos[0] = 0.0;
	if (pPos[1] < 0.0)
		pPos[1] = 0.0;

	double dCurrX = pView->m_dEnc[AXIS_X0];
	double dCurrY = pView->m_dEnc[AXIS_Y0];
	fLen = sqrt(((pPos[0] - dCurrX) * (pPos[0] - dCurrX)) + ((pPos[1] - dCurrY) * (pPos[1] - dCurrY)));
	if (fLen > 0.001)
	{
		m_pMotion->GetSpeedProfile0(TRAPEZOIDAL, AXIS_X0, fLen, fVel, fAcc, fJerk);
		m_pMotion->Move0(MS_X0Y0, pPos, fVel, fAcc, fAcc, ABS, NO_WAIT);
	}
}

void CGvisR2R_PunchView::Move1(CfPoint pt, BOOL bCam)
{
	if (m_bProbDn[1])
	{
		if (m_pVoiceCoil[1])
		{
			m_pVoiceCoil[1]->SearchHomeSmac(1);
			m_pVoiceCoil[1]->MoveSmacShiftPos(1);
			m_bProbDn[1] = FALSE;
			//if (m_pDlgMenu02->m_pDlgUtil06)
			//	m_pDlgMenu02->m_pDlgUtil06->myBtn[6].SetCheck(FALSE);
		}
	}

	double fLen, fVel, fAcc, fJerk;

	double pPos[2];

	if (pDoc->WorkingInfo.System.bNoMk || bCam)
	{
		pPos[0] = pt.x;
		pPos[1] = pt.y;
	}
	else
	{
		pPos[0] = pt.x + _tstof(pDoc->WorkingInfo.Vision[1].sMkOffsetX);
		pPos[1] = pt.y + _tstof(pDoc->WorkingInfo.Vision[1].sMkOffsetY);
	}

	if (pPos[0] < 0.0)
		pPos[0] = 0.0;
	if (pPos[1] < 0.0)
		pPos[1] = 0.0;

	double dCurrX = pView->m_dEnc[AXIS_X1];
	double dCurrY = pView->m_dEnc[AXIS_Y1];
	fLen = sqrt(((pPos[0] - dCurrX) * (pPos[0] - dCurrX)) + ((pPos[1] - dCurrY) * (pPos[1] - dCurrY)));
	if (fLen > 0.001)
	{
		m_pMotion->GetSpeedProfile1(TRAPEZOIDAL, AXIS_X1, fLen, fVel, fAcc, fJerk);
		m_pMotion->Move1(MS_X1Y1, pPos, fVel, fAcc, fAcc, ABS, NO_WAIT);
	}
}

BOOL CGvisR2R_PunchView::IsMoveDone()
{
	if (!m_pMotion)
		return FALSE;

	if (IsMoveDone0() && IsMoveDone1())
		return TRUE;
	return FALSE;
}

BOOL CGvisR2R_PunchView::IsMoveDone0()
{
	if (!m_pMotion)
		return FALSE;

	if (m_pMotion->IsMotionDone(MS_X0) && m_pMotion->IsMotionDone(MS_Y0))
	{
		//	Sleep(50);
		return TRUE;
	}
	return FALSE;
}

BOOL CGvisR2R_PunchView::IsMoveDone1()
{
	if (!m_pMotion)
		return FALSE;

	if (m_pMotion->IsMotionDone(MS_X1) && m_pMotion->IsMotionDone(MS_Y1))
	{
		//	Sleep(300);
		return TRUE;
	}
	return FALSE;
}

void CGvisR2R_PunchView::Mk0()
{
	if (pView->m_pVoiceCoil[0])
	{
		pView->m_pVoiceCoil[0]->SetMark(0);
		pDoc->AddMkCntL();
		m_nCurMk[0]++;
	}
}

void CGvisR2R_PunchView::Mk1()
{
	if (pView->m_pVoiceCoil[1])
	{
		pView->m_pVoiceCoil[1]->SetMark(1);
		pDoc->AddMkCntR();
		m_nCurMk[1]++;
	}
}

void CGvisR2R_PunchView::Ink(BOOL bOn)
{
	if (bOn)
	{
		// Ink Marking ON
		// 		pDoc->m_pSliceIo[7] |= (0x01<<13);	// ��ŷ�� ��ŷ ��Ʈ�ѷ� ��� 2(��������) -> Sen
	}
	else
	{
		// Ink Marking OFF
		// 		pDoc->m_pSliceIo[7] &= ~(0x01<<13);	// ��ŷ�� ��ŷ ��Ʈ�ѷ� ��� 2(��������) -> Sen
	}
}

//CString CGvisR2R_PunchView::GetRmapPath(int nRmap, stModelInfo stInfo)
//{
//	CString str, sPath, Path[5];
//
//	switch (nRmap)
//	{
//	case RMAP_UP:
//		str = _T("ReelMapDataUp.txt");
//		Path[0] = pDoc->WorkingInfo.System.sPathOldFile;
//		Path[1] = stInfo.sModel;
//		Path[2] = stInfo.sLot;
//		Path[3] = stInfo.sLayer;
//		Path[4] = _T("");
//		break;
//	case RMAP_ALLUP:
//		str = _T("ReelMapDataAll.txt");
//		Path[0] = pDoc->WorkingInfo.System.sPathOldFile;
//		Path[1] = stInfo.sModel;
//		Path[2] = stInfo.sLot;
//		Path[3] = stInfo.sLayer;
//		Path[4] = _T("");
//		break;
//	case RMAP_DN:
//		str = _T("ReelMapDataDn.txt");
//		Path[0] = pDoc->WorkingInfo.System.sPathOldFile;
//		Path[1] = stInfo.sModel;
//		Path[2] = stInfo.sLot;
//		Path[3] = stInfo.sLayer;
//		Path[4] = _T("");
//		break;
//	case RMAP_ALLDN:
//		str = _T("ReelMapDataAll.txt");
//		Path[0] = pDoc->WorkingInfo.System.sPathOldFile;
//		Path[1] = stInfo.sModel;
//		Path[2] = stInfo.sLot;
//		Path[3] = stInfo.sLayer;
//		Path[4] = _T("");
//		break;
//	case RMAP_INNER_UP:
//		str = _T("ReelMapDataUp.txt");
//		Path[0] = pDoc->WorkingInfo.System.sPathItsFile;
//		Path[1] = stInfo.sModel;
//		Path[2] = stInfo.sLot;
//		Path[3] = stInfo.sLayer;
//		Path[4] = pDoc->WorkingInfo.LastJob.sInnerLayerUp;
//		break;
//	case RMAP_INNER_DN:
//		str = _T("ReelMapDataDn.txt");
//		Path[0] = pDoc->WorkingInfo.System.sPathItsFile;
//		Path[1] = pDoc->WorkingInfo.LastJob.sModelUp;
//		Path[2] = pDoc->WorkingInfo.LastJob.sEngItsCode;
//		Path[3] = pDoc->WorkingInfo.LastJob.sInnerLotUp;
//		Path[4] = pDoc->WorkingInfo.LastJob.sInnerLayerDn;
//		break;
//	case RMAP_INNER_ALLUP:
//		str = _T("YieldAll.txt");
//		Path[0] = pDoc->WorkingInfo.System.sPathItsFile;
//		Path[1] = pDoc->WorkingInfo.LastJob.sModelUp;
//		Path[2] = pDoc->WorkingInfo.LastJob.sEngItsCode;
//		Path[3] = pDoc->WorkingInfo.LastJob.sInnerLotUp;
//		Path[4] = pDoc->WorkingInfo.LastJob.sInnerLayerUp;
//		break;
//	case RMAP_INNER_ALLDN:
//		str = _T("ReelMapDataAll.txt");
//		Path[0] = pDoc->WorkingInfo.System.sPathItsFile;
//		Path[1] = stInfo.sModel;
//		Path[2] = stInfo.sLot;
//		Path[3] = stInfo.sLayer;
//		Path[4] = pDoc->WorkingInfo.LastJob.sInnerLayerDn;
//		break;
//	case RMAP_INOUTER_UP:
//		break;
//	case RMAP_INOUTER_DN:
//		break;
//	case RMAP_ITS:
//		str = _T("ReelMapDataIts.txt");
//		Path[0] = pDoc->WorkingInfo.System.sPathItsFile;
//		Path[1] = stInfo.sModel;
//		Path[2] = pDoc->WorkingInfo.LastJob.sEngItsCode;
//		Path[3] = _T("");
//		Path[4] = _T("");
//		break;
//	}
//
//	if (!Path[0].IsEmpty() && !Path[1].IsEmpty())
//	{
//		sPath.Format(_T("%s%s"), Path[0], Path[1]);
//		if (!pDoc->DirectoryExists(sPath))
//			CreateDirectory(sPath, NULL);
//
//		if (!Path[2].IsEmpty())
//		{
//			sPath.Format(_T("%s%s\\%s"), Path[0], Path[1], Path[2]);
//			if (!pDoc->DirectoryExists(sPath))
//				CreateDirectory(sPath, NULL);
//
//			if (!Path[3].IsEmpty())
//			{
//				sPath.Format(_T("%s%s\\%s\\%s"), Path[0], Path[1], Path[2], Path[3]);
//				if (!pDoc->DirectoryExists(sPath))
//					CreateDirectory(sPath, NULL);
//
//				if (!Path[4].IsEmpty())
//				{
//					sPath.Format(_T("%s%s\\%s\\%s\\%s"), Path[0], Path[1], Path[2], Path[3], Path[4]);
//					if (!pDoc->DirectoryExists(sPath))
//						CreateDirectory(sPath, NULL);
//				}
//			}
//		}
//	}
//
//
////	CString sPath;
////#ifdef TEST_MODE
////	sPath = PATH_REELMAP;
////#else
////	CString str;
////	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;
////
////	if (bDualTest)
////	{
////		switch (nRmap)
////		{
////		case RMAP_UP:
////			str = _T("ReelMapDataUp.txt");
////			break;
////		case RMAP_ALLUP:
//////#ifdef TEST_MODE
//////			str = _T("ReelMapDataAllUp.txt");
//////#else
////			str = _T("ReelMapDataAll.txt");
//////#endif
////			break;
////		case RMAP_DN:
////			str = _T("ReelMapDataDn.txt");
////			break;
////		case RMAP_ALLDN:
//////#ifdef TEST_MODE
//////			str = _T("ReelMapDataAllDn.txt");
//////#else
////			str = _T("ReelMapDataAll.txt");
//////#endif
////			break;
////		}
////	}
////	else
////		str = _T("ReelMapDataUp.txt");
////
////	sPath.Format(_T("%s%s\\%s\\%s\\%s"), pDoc->WorkingInfo.System.sPathOldFile,
////		stInfo.sModel,
////		stInfo.sLot,
////		stInfo.sLayer,
////		str);
////#endif
//	return sPath;
//}

//CString CGvisR2R_PunchView::GetRmapPath(int nRmap)
//{
//	CString sPath = _T("");
//	CString Path[4], str;
//
//	switch (nRmap)
//	{
//	case RMAP_UP:
//		str = _T("ReelMapDataUp.txt");
//		if (pDoc->m_bDoneChgLot || !pDoc->m_bNewLotShare[0])
//		{
//			sPath.Format(_T("%s%s\\%s\\%s\\%s"), pDoc->WorkingInfo.System.sPathOldFile,
//				pDoc->WorkingInfo.LastJob.sModelUp,
//				pDoc->WorkingInfo.LastJob.sLotUp,
//				pDoc->WorkingInfo.LastJob.sLayerUp,
//				str);
//		}
//		else if (!pDoc->m_bDoneChgLot && pDoc->m_bNewLotShare[0])
//		{
//			sPath.Format(_T("%s%s\\%s\\%s\\%s"), pDoc->WorkingInfo.System.sPathOldFile,
//				pDoc->WorkingInfo.LastJob.sModelUp,
//				pDoc->Status.PcrShare[0].sLot,
//				pDoc->WorkingInfo.LastJob.sLayerUp,
//				str);
//		}
//		break;
//	case RMAP_ALLUP:
//		str = _T("ReelMapDataAll.txt");
//		if (pDoc->m_bDoneChgLot || !pDoc->m_bNewLotShare[0])
//		{
//			sPath.Format(_T("%s%s\\%s\\%s\\%s"), pDoc->WorkingInfo.System.sPathOldFile,
//				pDoc->WorkingInfo.LastJob.sModelUp,
//				pDoc->WorkingInfo.LastJob.sLotUp,
//				pDoc->WorkingInfo.LastJob.sLayerUp,
//				str);
//		}
//		else if (!pDoc->m_bDoneChgLot && pDoc->m_bNewLotShare[0])
//		{
//			sPath.Format(_T("%s%s\\%s\\%s\\%s"), pDoc->WorkingInfo.System.sPathOldFile,
//				pDoc->WorkingInfo.LastJob.sModelUp,
//				pDoc->Status.PcrShare[0].sLot,
//				pDoc->WorkingInfo.LastJob.sLayerUp,
//				str);
//		}
//		break;
//	case RMAP_DN:
//		str = _T("ReelMapDataDn.txt");
//		if (pDoc->m_bDoneChgLot || !pDoc->m_bNewLotShare[1])
//		{
//			sPath.Format(_T("%s%s\\%s\\%s\\%s"), pDoc->WorkingInfo.System.sPathOldFile,
//				//pDoc->WorkingInfo.LastJob.sModelDn,
//				//pDoc->WorkingInfo.LastJob.sLotDn,
//				pDoc->WorkingInfo.LastJob.sModelUp,
//				pDoc->WorkingInfo.LastJob.sLotUp,
//				pDoc->WorkingInfo.LastJob.sLayerDn,
//				str);
//		}
//		else if (!pDoc->m_bDoneChgLot && pDoc->m_bNewLotShare[1])
//		{
//			sPath.Format(_T("%s%s\\%s\\%s\\%s"), pDoc->WorkingInfo.System.sPathOldFile,
//				//pDoc->WorkingInfo.LastJob.sModelDn,
//				//pDoc->Status.PcrShare[1].sLot,
//				pDoc->WorkingInfo.LastJob.sModelUp,
//				pDoc->Status.PcrShare[0].sLot,
//				pDoc->WorkingInfo.LastJob.sLayerDn,
//				str);
//		}
//		break;
//	case RMAP_ALLDN:
//		str = _T("ReelMapDataAll.txt");
//		if (pDoc->m_bDoneChgLot || !pDoc->m_bNewLotShare[1])
//		{
//			sPath.Format(_T("%s%s\\%s\\%s\\%s"), pDoc->WorkingInfo.System.sPathOldFile,
//				pDoc->WorkingInfo.LastJob.sModelUp,
//				pDoc->WorkingInfo.LastJob.sLotUp,
//				//pDoc->WorkingInfo.LastJob.sModelDn,
//				//pDoc->WorkingInfo.LastJob.sLotDn,
//				pDoc->WorkingInfo.LastJob.sLayerDn,
//				str);
//		}
//		else if (!pDoc->m_bDoneChgLot && pDoc->m_bNewLotShare[1])
//		{
//			sPath.Format(_T("%s%s\\%s\\%s\\%s"), pDoc->WorkingInfo.System.sPathOldFile,
//				pDoc->WorkingInfo.LastJob.sModelUp,
//				pDoc->Status.PcrShare[0].sLot,
//				//pDoc->WorkingInfo.LastJob.sModelDn,
//				//pDoc->Status.PcrShare[1].sLot,
//				pDoc->WorkingInfo.LastJob.sLayerDn,
//				str);
//		}
//		break;
//	case RMAP_INNER_UP:
//		str = _T("ReelMapDataUp.txt");
//		Path[0] = pDoc->WorkingInfo.System.sPathOldFile;
//		Path[1] = pDoc->WorkingInfo.LastJob.sModelUp;
//		Path[2] = pDoc->WorkingInfo.LastJob.sInnerLotUp;
//		Path[3] = pDoc->WorkingInfo.LastJob.sInnerLayerUp;
//		sPath.Format(_T("%s%s\\%s\\%s\\%s"), Path[0], Path[1], Path[2], Path[3], str);
//		break;
//	case RMAP_INNER_DN:
//		str = _T("ReelMapDataDn.txt");
//		Path[0] = pDoc->WorkingInfo.System.sPathOldFile;
//		//Path[1] = pDoc->WorkingInfo.LastJob.sInnerModelDn;
//		//Path[2] = pDoc->WorkingInfo.LastJob.sInnerLotDn;
//		Path[1] = pDoc->WorkingInfo.LastJob.sModelUp;
//		Path[2] = pDoc->WorkingInfo.LastJob.sInnerLotUp;
//		Path[3] = pDoc->WorkingInfo.LastJob.sInnerLayerDn;
//		sPath.Format(_T("%s%s\\%s\\%s\\%s"), Path[0], Path[1], Path[2], Path[3], str);
//		break;
//	case RMAP_INNER_ALLUP:
//		str = _T("ReelMapDataAll.txt");
//		Path[0] = pDoc->WorkingInfo.System.sPathOldFile;
//		Path[1] = pDoc->WorkingInfo.LastJob.sModelUp;
//		Path[2] = pDoc->WorkingInfo.LastJob.sInnerLotUp;
//		Path[3] = pDoc->WorkingInfo.LastJob.sInnerLayerUp;
//		sPath.Format(_T("%s%s\\%s\\%s\\%s"), Path[0], Path[1], Path[2], Path[3], str);
//		break;
//	case RMAP_INNER_ALLDN:
//		str = _T("ReelMapDataAll.txt");
//		Path[0] = pDoc->WorkingInfo.System.sPathOldFile;
//		//Path[1] = pDoc->WorkingInfo.LastJob.sInnerModelDn;
//		//Path[2] = pDoc->WorkingInfo.LastJob.sInnerLotDn;
//		Path[1] = pDoc->WorkingInfo.LastJob.sModelUp;
//		Path[2] = pDoc->WorkingInfo.LastJob.sInnerLotUp;
//		Path[3] = pDoc->WorkingInfo.LastJob.sInnerLayerDn;
//		sPath.Format(_T("%s%s\\%s\\%s\\%s"), Path[0], Path[1], Path[2], Path[3], str);
//		break;
//	case RMAP_INOUTER_UP:
//		str = _T("ReelMapDataIO.txt");
//		sPath.Format(_T("%s%s\\%s\\%s\\%s"), pDoc->WorkingInfo.System.sPathOldFile,
//			pDoc->WorkingInfo.LastJob.sModelUp,
//			pDoc->WorkingInfo.LastJob.sLotUp,
//			pDoc->WorkingInfo.LastJob.sLayerUp,
//			str);
//		break;
//	case RMAP_INOUTER_DN:
//		str = _T("ReelMapDataIO.txt");
//		sPath.Format(_T("%s%s\\%s\\%s\\%s"), pDoc->WorkingInfo.System.sPathOldFile,
//			//pDoc->WorkingInfo.LastJob.sModelDn,
//			//pDoc->WorkingInfo.LastJob.sLotDn,
//			pDoc->WorkingInfo.LastJob.sModelUp,
//			pDoc->WorkingInfo.LastJob.sLotUp,
//			pDoc->WorkingInfo.LastJob.sLayerDn,
//			str);
//		break;
//	case RMAP_ITS:
//		pDoc->GetCurrentInfoEng();
//		str = _T("ReelMapDataIts.txt");
//		sPath.Format(_T("%s%s\\%s\\%s"), pDoc->WorkingInfo.System.sPathItsFile,
//			pDoc->WorkingInfo.LastJob.sModelUp,
//			pDoc->WorkingInfo.LastJob.sEngItsCode,
//			//pDoc->m_sItsCode,
//			str);
//		break;
//	}
//
//	return sPath;
//}
//CString CGvisR2R_PunchView::GetRmapPath(int nRmap)
//{
//	CString sPath;
//#ifdef TEST_MODE
//	sPath = PATH_REELMAP;
//#else
//	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;
//
//	CString str;
//	if (bDualTest)
//	{
//		switch (nRmap)
//		{
//		case RMAP_UP:
//			str = _T("ReelMapDataUp.txt");
//			if (pDoc->m_bDoneChgLot || !pDoc->m_bNewLotShare[0])
//			{
//				sPath.Format(_T("%s%s\\%s\\%s\\%s"), pDoc->WorkingInfo.System.sPathOldFile,
//					pDoc->WorkingInfo.LastJob.sModelUp,
//					pDoc->WorkingInfo.LastJob.sLotUp,
//					pDoc->WorkingInfo.LastJob.sLayerUp,
//					str);
//			}
//			else if (!pDoc->m_bDoneChgLot && pDoc->m_bNewLotShare[0])
//			{
//				sPath.Format(_T("%s%s\\%s\\%s\\%s"), pDoc->WorkingInfo.System.sPathOldFile,
//					pDoc->WorkingInfo.LastJob.sModelUp,
//					pDoc->Status.PcrShare[0].sLot,
//					pDoc->WorkingInfo.LastJob.sLayerUp,
//					str);
//			}
//			break;
//		case RMAP_ALLUP:
////#ifdef TEST_MODE
////			str = _T("ReelMapDataAllUp.txt");
////#else
//			str = _T("ReelMapDataAll.txt");
////#endif
//			if (pDoc->m_bDoneChgLot || !pDoc->m_bNewLotShare[0])
//			{
//				sPath.Format(_T("%s%s\\%s\\%s\\%s"), pDoc->WorkingInfo.System.sPathOldFile,
//					pDoc->WorkingInfo.LastJob.sModelUp,
//					pDoc->WorkingInfo.LastJob.sLotUp,
//					pDoc->WorkingInfo.LastJob.sLayerUp,
//					str);
//			}
//			else if (!pDoc->m_bDoneChgLot && pDoc->m_bNewLotShare[0])
//			{
//				sPath.Format(_T("%s%s\\%s\\%s\\%s"), pDoc->WorkingInfo.System.sPathOldFile,
//					pDoc->WorkingInfo.LastJob.sModelUp,
//					pDoc->Status.PcrShare[0].sLot,
//					pDoc->WorkingInfo.LastJob.sLayerUp,
//					str);
//			}
//			break;
//		case RMAP_DN:
//			str = _T("ReelMapDataDn.txt");
//			if (pDoc->m_bDoneChgLot || !pDoc->m_bNewLotShare[1])
//			{
//				sPath.Format(_T("%s%s\\%s\\%s\\%s"), pDoc->WorkingInfo.System.sPathOldFile,
//					//pDoc->WorkingInfo.LastJob.sModelDn,
//					//pDoc->WorkingInfo.LastJob.sLotDn,
//					pDoc->WorkingInfo.LastJob.sModelUp,
//					pDoc->WorkingInfo.LastJob.sLotUp,
//					pDoc->WorkingInfo.LastJob.sLayerDn,
//					str);
//			}
//			else if (!pDoc->m_bDoneChgLot && pDoc->m_bNewLotShare[1])
//			{
//				sPath.Format(_T("%s%s\\%s\\%s\\%s"), pDoc->WorkingInfo.System.sPathOldFile,
//					//pDoc->WorkingInfo.LastJob.sModelDn,
//					//pDoc->Status.PcrShare[1].sLot,
//					pDoc->WorkingInfo.LastJob.sModelUp,
//					pDoc->Status.PcrShare[0].sLot,
//					pDoc->WorkingInfo.LastJob.sLayerDn,
//					str);
//			}
//			break;
//		case RMAP_ALLDN:
////#ifdef TEST_MODE
////			str = _T("ReelMapDataAllDn.txt");
////#else
//			str = _T("ReelMapDataAll.txt");
////#endif
//			if (pDoc->m_bDoneChgLot || !pDoc->m_bNewLotShare[1])
//			{
//				sPath.Format(_T("%s%s\\%s\\%s\\%s"), pDoc->WorkingInfo.System.sPathOldFile,
//					pDoc->WorkingInfo.LastJob.sModelUp,
//					pDoc->WorkingInfo.LastJob.sLotUp,
//					//pDoc->WorkingInfo.LastJob.sModelDn,
//					//pDoc->WorkingInfo.LastJob.sLotDn,
//					pDoc->WorkingInfo.LastJob.sLayerDn,
//					str);
//			}
//			else if (!pDoc->m_bDoneChgLot && pDoc->m_bNewLotShare[1])
//			{
//				sPath.Format(_T("%s%s\\%s\\%s\\%s"), pDoc->WorkingInfo.System.sPathOldFile,
//					pDoc->WorkingInfo.LastJob.sModelUp,
//					pDoc->Status.PcrShare[0].sLot,
//					//pDoc->WorkingInfo.LastJob.sModelDn,
//					//pDoc->Status.PcrShare[1].sLot,
//					pDoc->WorkingInfo.LastJob.sLayerDn,
//					str);
//			}
//			break;
//		}
//	}
//	else
//	{
//		if (pDoc->GetTestMode() != MODE_OUTER)
//		{
//			str = _T("ReelMapDataUp.txt");
//			if (pDoc->m_bDoneChgLot || !pDoc->m_bNewLotShare[0])
//			{
//				sPath.Format(_T("%s%s\\%s\\%s\\%s"), pDoc->WorkingInfo.System.sPathOldFile,
//					pDoc->WorkingInfo.LastJob.sModelUp,
//					pDoc->WorkingInfo.LastJob.sLotUp,
//					pDoc->WorkingInfo.LastJob.sLayerUp,
//					str);
//			}
//			else if (!pDoc->m_bDoneChgLot && pDoc->m_bNewLotShare[0])
//			{
//				sPath.Format(_T("%s%s\\%s\\%s\\%s"), pDoc->WorkingInfo.System.sPathOldFile,
//					pDoc->WorkingInfo.LastJob.sModelUp,
//					pDoc->Status.PcrShare[0].sLot,
//					pDoc->WorkingInfo.LastJob.sLayerUp,
//					str);
//			}
//
//			return sPath;
//		}
//	}
//
//	//	pDoc->m_pReelMap->m_nLayer = nRmap;
//
//	if (pDoc->GetTestMode() == MODE_OUTER)
//	{
//		int nIdx;
//		CString Path[4];
//
//		//bDualTest = pDoc->WorkingInfo.LastJob.bInnerDualTest;
//		//if (bDualTest)
//		{
//			switch (nRmap)
//			{
//			case RMAP_INNER_UP:
//				str = _T("ReelMapDataUp.txt");
//				Path[0] = pDoc->WorkingInfo.System.sPathOldFile;
//				Path[1] = pDoc->WorkingInfo.LastJob.sInnerModelUp;
//				Path[2] = pDoc->WorkingInfo.LastJob.sInnerLotUp;
//				Path[3] = pDoc->WorkingInfo.LastJob.sInnerLayerUp;
//				sPath.Format(_T("%s%s\\%s\\%s\\%s"), Path[0], Path[1], Path[2], Path[3], str);
//				break;
//			case RMAP_INNER_DN:
//				str = _T("ReelMapDataDn.txt");
//				Path[0] = pDoc->WorkingInfo.System.sPathOldFile;
//				//Path[1] = pDoc->WorkingInfo.LastJob.sInnerModelDn;
//				//Path[2] = pDoc->WorkingInfo.LastJob.sInnerLotDn;
//				Path[1] = pDoc->WorkingInfo.LastJob.sInnerModelUp;
//				Path[2] = pDoc->WorkingInfo.LastJob.sInnerLotUp;
//				Path[3] = pDoc->WorkingInfo.LastJob.sInnerLayerDn;
//				sPath.Format(_T("%s%s\\%s\\%s\\%s"), Path[0], Path[1], Path[2], Path[3], str);
//				break;
//			case RMAP_INNER_ALLUP:
//				str = _T("ReelMapDataAll.txt");
//				Path[0] = pDoc->WorkingInfo.System.sPathOldFile;
//				Path[1] = pDoc->WorkingInfo.LastJob.sInnerModelUp;
//				Path[2] = pDoc->WorkingInfo.LastJob.sInnerLotUp;
//				Path[3] = pDoc->WorkingInfo.LastJob.sInnerLayerUp;
//				sPath.Format(_T("%s%s\\%s\\%s\\%s"), Path[0], Path[1], Path[2], Path[3], str);
//				break;
//			case RMAP_INNER_ALLDN:
//				str = _T("ReelMapDataAll.txt");
//				Path[0] = pDoc->WorkingInfo.System.sPathOldFile;
//				//Path[1] = pDoc->WorkingInfo.LastJob.sInnerModelDn;
//				//Path[2] = pDoc->WorkingInfo.LastJob.sInnerLotDn;
//				Path[1] = pDoc->WorkingInfo.LastJob.sInnerModelUp;
//				Path[2] = pDoc->WorkingInfo.LastJob.sInnerLotUp;
//				Path[3] = pDoc->WorkingInfo.LastJob.sInnerLayerDn;
//				sPath.Format(_T("%s%s\\%s\\%s\\%s"), Path[0], Path[1], Path[2], Path[3], str);
//				break;
//			case RMAP_INOUTER_UP:
//				str = _T("ReelMapDataIO.txt");
//				sPath.Format(_T("%s%s\\%s\\%s\\%s"), pDoc->WorkingInfo.System.sPathOldFile,
//					pDoc->WorkingInfo.LastJob.sModelUp,
//					pDoc->WorkingInfo.LastJob.sLotUp,
//					pDoc->WorkingInfo.LastJob.sLayerUp,
//					str);
//				break;
//			case RMAP_INOUTER_DN:
//				str = _T("ReelMapDataIO.txt");
//				sPath.Format(_T("%s%s\\%s\\%s\\%s"), pDoc->WorkingInfo.System.sPathOldFile,
//					//pDoc->WorkingInfo.LastJob.sModelDn,
//					//pDoc->WorkingInfo.LastJob.sLotDn,
//					pDoc->WorkingInfo.LastJob.sModelUp,
//					pDoc->WorkingInfo.LastJob.sLotUp,
//					pDoc->WorkingInfo.LastJob.sLayerDn,
//					str);
//				break;
//			}
//		}
//	}
//#endif
//	return sPath;
//}

BOOL CGvisR2R_PunchView::LoadPcrUp(int nSerial, BOOL bFromShare)
{
	if (nSerial <= 0)
	{
		pView->ClrDispMsg();
		AfxMessageBox(_T("Serial Error.52"));
		return 0;
	}

	int nHeadInfo = pDoc->LoadPCR0(nSerial); // 2(Failed), 1(����), -1(Align Error, �뱤�ҷ�), -2(Lot End)
	if (nHeadInfo >= 2)
	{
		MsgBox(_T("Error-LoadPCR0()"));
		return FALSE;
	}
	return TRUE;
}

BOOL CGvisR2R_PunchView::LoadPcrDn(int nSerial, BOOL bFromShare)
{
	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;
	if (!bDualTest)
		return 0;

	if (nSerial <= 0)
	{
		pView->ClrDispMsg();
		AfxMessageBox(_T("Serial Error.53"));
		return 0;
	}

	int nHeadInfo = pDoc->LoadPCR1(nSerial); // 2(Failed), 1(����), -1(Align Error, �뱤�ҷ�), -2(Lot End)
	if (nHeadInfo >= 2)
	{
		MsgBox(_T("Error-LoadPCR1()"));
		return FALSE;
	}
	return TRUE;
}

//BOOL CGvisR2R_PunchView::UpdateReelmap(int nSerial)
//{
//	if (nSerial <= 0)
//	{
//		pView->ClrDispMsg();
//		AfxMessageBox(_T("Serial Error.54"));
//		return 0;
//	}
//
//	if (!pDoc->MakeMkDir())
//		return FALSE;
//
//	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;
//
//	CString str;
//	CString sPathRmap[4], sPathPcr[2]; //[Up/Dn]
//
//	if (pDoc->m_pReelMap)
//	{
//		stModelInfo stInfo;
//		sPathPcr[0].Format(_T("%s%04d.pcr"), pDoc->WorkingInfo.System.sPathVrsBufUp, nSerial);
//		if (bDualTest)
//			sPathPcr[1].Format(_T("%s%04d.pcr"), pDoc->WorkingInfo.System.sPathVrsBufDn, nSerial);
//
//		if (!pDoc->GetPcrInfo(sPathPcr[0], stInfo)) // Up
//		{
//			pView->DispStsBar(_T("E(4)"), 5);
//			pView->ClrDispMsg();
//			AfxMessageBox(_T("Error-GetPcrInfo(4)"));
//			return FALSE;
//		}
//
//		if (!pDoc->MakeMkDir(stInfo.sModel, stInfo.sLot, stInfo.sLayer))
//			return FALSE;
//
//		str = _T("ReelMapDataUp.txt"); // [0]:AOI-Up
//		sPathRmap[0].Format(_T("%s%s\\%s\\%s\\%s"), pDoc->WorkingInfo.System.sPathOldFile,
//			stInfo.sModel,
//			stInfo.sLot,
//			stInfo.sLayer,
//			str);
//
//		if (bDualTest)
//		{
//			str = _T("ReelMapDataAll.txt"); // [2]:AOI-AllUp
//			sPathRmap[2].Format(_T("%s%s\\%s\\%s\\%s"), pDoc->WorkingInfo.System.sPathOldFile,
//				stInfo.sModel,
//				stInfo.sLot,
//				stInfo.sLayer,
//				str);
//
//			if (!pDoc->GetPcrInfo(sPathPcr[1], stInfo)) // Dn
//			{
//				pView->DispStsBar(_T("E(5)"), 5);
//				pView->ClrDispMsg();
//				AfxMessageBox(_T("Error-GetPcrInfo(5)"));
//				return FALSE;
//			}
//
//			if (!pDoc->MakeMkDir(stInfo.sModel, stInfo.sLot, stInfo.sLayer))
//				return FALSE;
//
//			str = _T("ReelMapDataDn.txt"); // [1]:AOI-Dn
//			sPathRmap[1].Format(_T("%s%s\\%s\\%s\\%s"), pDoc->WorkingInfo.System.sPathOldFile,
//				stInfo.sModel,
//				stInfo.sLot,
//				stInfo.sLayer,
//				str);
//			str = _T("ReelMapDataAll.txt"); // [3]:AOI-AllDn
//			sPathRmap[3].Format(_T("%s%s\\%s\\%s\\%s"), pDoc->WorkingInfo.System.sPathOldFile,
//				stInfo.sModel,
//				stInfo.sLot,
//				stInfo.sLayer,
//				str);
//		}
//
//		m_nSerialRmapUpdate = nSerial;
//		m_sPathRmapUpdate[0] = sPathRmap[0];
//		m_sPathRmapUpdate[1] = sPathRmap[1];
//		m_sPathRmapUpdate[2] = sPathRmap[2];
//		m_sPathRmapUpdate[3] = sPathRmap[3];
//
//		m_bTHREAD_UPDATE_REELMAP_UP = TRUE;
//		if (bDualTest)
//		{
//			m_bTHREAD_UPDATE_REELMAP_DN = TRUE;
//			m_bTHREAD_UPDATE_REELMAP_ALLUP = TRUE;
//			m_bTHREAD_UPDATE_REELMAP_ALLDN = TRUE;
//		}
//
//		if (pDoc->GetTestMode() == MODE_OUTER)
//			UpdateReelmapInner(nSerial);
//
//		Sleep(100);
//		return TRUE;
//	}
//	else
//		MsgBox(_T("Error-ReelMapWrite()"));
//
//	return FALSE;
//}

void CGvisR2R_PunchView::UpdateRMapUp()
{
	if (pDoc->m_pReelMapUp)
	{
		pDoc->m_pReelMapUp->Write(m_nSerialRmapUpdate); // [0]:AOI-Up , [1]:AOI-Dn , [2]:AOI-AllUp , [3]:AOI-AllDn
		//pDoc->m_pReelMapUp->Write(m_nSerialRmapUpdate, 0, m_sPathRmapUpdate[0]); // [0]:AOI-Up , [1]:AOI-Dn , [2]:AOI-AllUp , [3]:AOI-AllDn

		if (pDoc->m_pReelMap == pDoc->m_pReelMapUp)
		{
			DuplicateRmap(RMAP_UP);
		}
	}
}

void CGvisR2R_PunchView::UpdateRMapDn()
{
	if (pDoc->m_pReelMapDn)
	{
		pDoc->m_pReelMapDn->Write(m_nSerialRmapUpdate); // [0]:AOI-Up , [1]:AOI-Dn , [2]:AOI-AllUp , [3]:AOI-AllDn
		//pDoc->m_pReelMapDn->Write(m_nSerialRmapUpdate, 1, m_sPathRmapUpdate[1]); // [0]:AOI-Up , [1]:AOI-Dn , [2]:AOI-AllUp , [3]:AOI-AllDn
	}
}

void CGvisR2R_PunchView::UpdateRMapAllUp()
{
	if (pDoc->m_pReelMapAllUp)
	{
		pDoc->m_pReelMapAllUp->Write(m_nSerialRmapUpdate); // [0]:AOI-Up , [1]:AOI-Dn , [2]:AOI-AllUp , [3]:AOI-AllDn
		//pDoc->m_pReelMapAllUp->Write(m_nSerialRmapUpdate, 2, m_sPathRmapUpdate[2]); // [0]:AOI-Up , [1]:AOI-Dn , [2]:AOI-AllUp , [3]:AOI-AllDn

		if (pDoc->m_pReelMap == pDoc->m_pReelMapAllUp)
		{
			DuplicateRmap(RMAP_ALLUP);
		}
	}
}

void CGvisR2R_PunchView::UpdateRMapAllDn()
{
	if (pDoc->m_pReelMapAllDn)
	{
		pDoc->m_pReelMapAllDn->Write(m_nSerialRmapUpdate); // [0]:AOI-Up , [1]:AOI-Dn , [2]:AOI-AllUp , [3]:AOI-AllDn
		//pDoc->m_pReelMapAllDn->Write(m_nSerialRmapUpdate, 3, m_sPathRmapUpdate[3]); // [0]:AOI-Up , [1]:AOI-Dn , [2]:AOI-AllUp , [3]:AOI-AllDn
	}
}

UINT CGvisR2R_PunchView::ThreadProc6(LPVOID lpContext)	// UpdateRMapUp()
{
	// Turn the passed in 'this' pointer back into a CProgressMgr instance
	CGvisR2R_PunchView* pThread = reinterpret_cast<CGvisR2R_PunchView*>(lpContext);

	BOOL bLock = FALSE;
	DWORD dwTick = GetTickCount();
	DWORD dwShutdownEventCheckPeriod = 0; // thread shutdown event check period

	pThread->m_bThread[6] = TRUE;
	while (WAIT_OBJECT_0 != WaitForSingleObject(pThread->m_Thread[6].GetShutdownEvent(), dwShutdownEventCheckPeriod))
	{
		pThread->m_dwThreadTick[6] = GetTickCount() - dwTick;
		dwTick = GetTickCount();

		if (pThread->m_bTHREAD_UPDATE_REELMAP_UP) // Write Reelmap
		{
			if (!pThread->m_bTHREAD_REELMAP_YIELD_UP) // Yield Reelmap
			{
				pThread->UpdateRMapUp(); // Write Reelmap
				pThread->m_bTHREAD_UPDATE_REELMAP_UP = FALSE;
				Sleep(0);
			}
			else
				Sleep(30);
		}
		else
			Sleep(30);
	}

	pThread->m_bThread[6] = FALSE;

	return 0;
}

UINT CGvisR2R_PunchView::ThreadProc7(LPVOID lpContext)	// UpdateRMapDn()
{
	// Turn the passed in 'this' pointer back into a CProgressMgr instance
	CGvisR2R_PunchView* pThread = reinterpret_cast<CGvisR2R_PunchView*>(lpContext);

	BOOL bLock = FALSE;
	DWORD dwTick = GetTickCount();
	DWORD dwShutdownEventCheckPeriod = 0; // thread shutdown event check period

	pThread->m_bThread[7] = TRUE;
	while (WAIT_OBJECT_0 != WaitForSingleObject(pThread->m_Thread[7].GetShutdownEvent(), dwShutdownEventCheckPeriod))
	{
		pThread->m_dwThreadTick[7] = GetTickCount() - dwTick;
		dwTick = GetTickCount();

		if (pThread->m_bTHREAD_UPDATE_REELMAP_DN) // Write Reelmap
		{
			//pThread->m_bTHREAD_UPDATE_REELMAP_ALLUP = TRUE;
			//pThread->m_bTHREAD_UPDATE_REELMAP_ALLDN = TRUE;
			if (!pThread->m_bTHREAD_REELMAP_YIELD_DN) // Yield Reelmap
			{
				pThread->UpdateRMapDn(); // Write Reelmap
				pThread->m_bTHREAD_UPDATE_REELMAP_DN = FALSE;
				Sleep(0);
			}
			else
				Sleep(30);
		}
		else
			Sleep(30);
	}

	pThread->m_bThread[7] = FALSE;

	return 0;
}

UINT CGvisR2R_PunchView::ThreadProc8(LPVOID lpContext)	// UpdateRMapAllUp()
{
	// Turn the passed in 'this' pointer back into a CProgressMgr instance
	CGvisR2R_PunchView* pThread = reinterpret_cast<CGvisR2R_PunchView*>(lpContext);

	BOOL bLock = FALSE;
	DWORD dwTick = GetTickCount();
	DWORD dwShutdownEventCheckPeriod = 0; // thread shutdown event check period

	pThread->m_bThread[8] = TRUE;
	while (WAIT_OBJECT_0 != WaitForSingleObject(pThread->m_Thread[8].GetShutdownEvent(), dwShutdownEventCheckPeriod))
	{
		pThread->m_dwThreadTick[8] = GetTickCount() - dwTick;
		dwTick = GetTickCount();

		if (pThread->m_bTHREAD_UPDATE_REELMAP_ALLUP) // Write Reelmap
		{
			if (!pThread->m_bTHREAD_REELMAP_YIELD_ALLUP) // Yield Reelmap
			{
				pThread->UpdateRMapAllUp(); // Write Reelmap
				pThread->m_bTHREAD_UPDATE_REELMAP_ALLUP = FALSE;
				Sleep(0);
			}
			else
				Sleep(30);
		}
		else
			Sleep(30);
	}

	pThread->m_bThread[8] = FALSE;

	return 0;
}

UINT CGvisR2R_PunchView::ThreadProc9(LPVOID lpContext)	// UpdateRMapAllDn()
{
	// Turn the passed in 'this' pointer back into a CProgressMgr instance
	CGvisR2R_PunchView* pThread = reinterpret_cast<CGvisR2R_PunchView*>(lpContext);

	BOOL bLock = FALSE;
	DWORD dwTick = GetTickCount();
	DWORD dwShutdownEventCheckPeriod = 0; // thread shutdown event check period

	pThread->m_bThread[9] = TRUE;
	while (WAIT_OBJECT_0 != WaitForSingleObject(pThread->m_Thread[9].GetShutdownEvent(), dwShutdownEventCheckPeriod))
	{
		pThread->m_dwThreadTick[9] = GetTickCount() - dwTick;
		dwTick = GetTickCount();

		if (pThread->m_bTHREAD_UPDATE_REELMAP_ALLDN) // Write Reelmap
		{
			if (!pThread->m_bTHREAD_REELMAP_YIELD_ALLDN) // Yield Reelmap
			{
				pThread->UpdateRMapAllDn(); // Write Reelmap
				pThread->m_bTHREAD_UPDATE_REELMAP_ALLDN = FALSE;
				Sleep(0);
			}
			else
				Sleep(30);
		}
		else
			Sleep(30);
	}

	pThread->m_bThread[9] = FALSE;

	return 0;
}

void CGvisR2R_PunchView::InitInfo()
{
	if (m_pDlgMenu01)
		m_pDlgMenu01->UpdateData();

	if (m_pDlgMenu05)
	{
		m_pDlgMenu05->InitModel();
		if (m_pDlgMenu05->IsWindowVisible())
			m_pDlgMenu05->AtDlgShow();
	}
}

// void CGvisR2R_PunchView::ResetReelmap()
// {
// 	if(m_pDlgMenu01)
// 		m_pDlgMenu01->ResetGL();
// 	//pDoc->ResetReelmap();
// }

void CGvisR2R_PunchView::InitReelmap()
{
	pDoc->InitReelmap();
	pDoc->SetReelmap(ROT_NONE);
	// 	pDoc->SetReelmap(ROT_CCW_90);
	pDoc->UpdateData();

	//pDoc->GetCurrentInfoEng();
	////if (pDoc->GetTestMode() == MODE_OUTER)
	//if (pDoc->m_Master[0].IsMstSpec(pDoc->WorkingInfo.System.sPathCamSpecDir, pDoc->WorkingInfo.LastJob.sModelUp, pDoc->m_sEngLayerUp))
	//{
	//	pDoc->InitReelmapInner();
	//	pDoc->SetReelmap(ROT_NONE);
	//}
}

void CGvisR2R_PunchView::InitReelmapUp()
{
	pDoc->InitReelmapUp();
	pDoc->SetReelmap(ROT_NONE);
	// 	pDoc->SetReelmap(ROT_CCW_90);
	pDoc->UpdateData();

	//if (pDoc->GetTestMode() == MODE_OUTER)
	//pDoc->GetCurrentInfoEng();
	//if (pDoc->m_Master[0].IsMstSpec(pDoc->WorkingInfo.System.sPathCamSpecDir, pDoc->WorkingInfo.LastJob.sModelUp, pDoc->m_sEngLayerUp))
	//{
	//	pDoc->InitReelmapInnerUp();

	//	pDoc->GetCurrentInfoEng();
	//	if (pDoc->m_bEngDualTest)
	//			pDoc->InitReelmapInnerDn();
	//	pDoc->SetReelmapInner(ROT_NONE);
	//}
}

void CGvisR2R_PunchView::InitReelmapDn()
{
	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;
	//if (!bDualTest)
	//	return;

	if (bDualTest)
	{
		pDoc->InitReelmapDn();
		pDoc->SetReelmap(ROT_NONE);
		// 	pDoc->SetReelmap(ROT_CCW_90);
		pDoc->UpdateData();
	}
}

// void CGvisR2R_PunchView::LoadMstInfo()
// {
// 	CString sPath;
// 
// 	pDoc->LoadMasterSpec();
// 	pDoc->LoadPinImg();
// 	pDoc->LoadAlignImg();
// 	pDoc->GetCamPxlRes();
// 	pDoc->LoadStripRgnFromCam();
// 
// 	pDoc->LoadPcsRgnFromCam();
// 	pDoc->LoadPcsImg();
// 	pDoc->LoadCadImg();
// 
// 	pDoc->LoadCadMk(); //.pch
// }

BOOL CGvisR2R_PunchView::LoadMstInfo()
{
	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;
	CString sLot, sLayerUp, sLayerDn;
	BOOL bDualTestInner, bGetCurrentInfoEng;

	bGetCurrentInfoEng = GetCurrentInfoEng(); // TRUE: MODE_INNER or MODE_OUTER
	pDoc->GetCamPxlRes();

	if (IsLastJob(0)) // Up
	{
		if (bGetCurrentInfoEng)
		{
			pDoc->m_Master[0].Init(pDoc->WorkingInfo.System.sPathCamSpecDir,
				pDoc->m_sEngModel,
				pDoc->m_sEngLayerUp);
			pDoc->m_Master[0].LoadMstInfo();
			//pDoc->m_Master[0].WriteStripPieceRegion_Text(pDoc->WorkingInfo.System.sPathOldFile, pDoc->WorkingInfo.LastJob.sLotUp);

			if (pDoc->GetTestMode() == MODE_OUTER)
			{
				if (pDoc->GetItsSerialInfo(0, bDualTestInner, sLot, sLayerUp, sLayerDn, 3))
				{
					if (pDoc->m_MasterInner[0].IsMstSpec(pDoc->WorkingInfo.System.sPathCamSpecDir, pDoc->m_sEngModel, sLayerUp))
					{
						pDoc->m_MasterInner[0].Init(pDoc->WorkingInfo.System.sPathCamSpecDir,
							pDoc->m_sEngModel,
							sLayerUp);
						pDoc->m_MasterInner[0].LoadMstInfo();
					}
				}
			}
		}
		else
		{
			pDoc->m_Master[0].Init(pDoc->WorkingInfo.System.sPathCamSpecDir,
				pDoc->WorkingInfo.LastJob.sModelUp,
				pDoc->WorkingInfo.LastJob.sLayerUp);
			pDoc->m_Master[0].LoadMstInfo();
			//pDoc->m_Master[0].WriteStripPieceRegion_Text(pDoc->WorkingInfo.System.sPathOldFile, pDoc->WorkingInfo.LastJob.sLotUp);
		}
	}

	if (IsLastJob(1)) // Dn
	{
		if (bGetCurrentInfoEng)
		{
			pDoc->m_Master[1].Init(pDoc->WorkingInfo.System.sPathCamSpecDir,
				pDoc->m_sEngModel,
				//pDoc->WorkingInfo.LastJob.sModelDn,
				pDoc->m_sEngLayerDn,
				pDoc->m_sEngLayerUp);

			pDoc->m_Master[1].LoadMstInfo();
			//pDoc->m_Master[1].WriteStripPieceRegion_Text(pDoc->WorkingInfo.System.sPathOldFile, pDoc->WorkingInfo.LastJob.sLotDn);
			
			if (pDoc->GetTestMode() == MODE_OUTER)
			{
				if (pDoc->GetItsSerialInfo(0, bDualTestInner, sLot, sLayerUp, sLayerDn, 0))
				{
					//if (pDoc->m_MasterInner[0].IsMstSpec(pDoc->WorkingInfo.System.sPathCamSpecDir, pDoc->WorkingInfo.LastJob.sModelUp, sLayerDn))
					if (pDoc->m_MasterInner[0].IsMstSpec(pDoc->WorkingInfo.System.sPathCamSpecDir, pDoc->m_sEngModel, sLayerDn))
					{
						//GetCurrentInfoEng();
						//pDoc->m_MasterInner[1].Init(pDoc->WorkingInfo.System.sPathCamSpecDir,
						//	pDoc->WorkingInfo.LastJob.sModelUp,
						//	sLayerDn,
						//	sLayerUp);
						pDoc->m_MasterInner[1].Init(pDoc->WorkingInfo.System.sPathCamSpecDir,
							pDoc->m_sEngModel,
							sLayerDn,
							sLayerUp);
						pDoc->m_MasterInner[1].LoadMstInfo();
					}
				}
			}
		}
		else
		{
			pDoc->m_Master[1].Init(pDoc->WorkingInfo.System.sPathCamSpecDir,
				pDoc->WorkingInfo.LastJob.sModelUp,
				//pDoc->WorkingInfo.LastJob.sModelDn,
				pDoc->WorkingInfo.LastJob.sLayerDn,
				pDoc->WorkingInfo.LastJob.sLayerUp);

			pDoc->m_Master[1].LoadMstInfo();
			//pDoc->m_Master[1].WriteStripPieceRegion_Text(pDoc->WorkingInfo.System.sPathOldFile, pDoc->WorkingInfo.LastJob.sLotDn);
		}
	}

	//if (m_pDts)
	//{
	//	if (m_pDts->IsUseDts())
	//	{
	//		pDoc->m_MasterDB.WriteStripPieceRegion_Text(pDoc->WorkingInfo.System.sPathOldFile, pDoc->WorkingInfo.LastJob.sLotUp);
	//	}
	//}

	SetAlignPos();

	// Reelmap ���� Loading.....
	InitReelmap(); // Delete & New

	if (bGetCurrentInfoEng)
	{
		if (pDoc->GetTestMode() == MODE_OUTER)
		{
			if (pDoc->GetItsSerialInfo(0, bDualTestInner, sLot, sLayerUp, sLayerDn, 0))
			{
				//if (pDoc->m_MasterInner[0].IsMstSpec(pDoc->WorkingInfo.System.sPathCamSpecDir, pDoc->WorkingInfo.LastJob.sModelUp, sLayerUp))
				if (pDoc->m_MasterInner[0].IsMstSpec(pDoc->WorkingInfo.System.sPathCamSpecDir, pDoc->m_sEngModel, sLayerUp))
				{
					// Reelmap ���� Loading.....
					InitReelmapInner(); // Delete & New

					if (m_pDlgMenu06)
					{
						m_pDlgMenu06->InitGL();
						m_pDlgMenu06->RefreshRmap();
						m_pDlgMenu06->InitCadImg();
						m_pDlgMenu06->SetPnlNum();
						m_pDlgMenu06->SetPnlDefNum();

						if (bDualTestInner)
							m_pDlgMenu06->SelMap(ALL);
						else
							m_pDlgMenu06->SelMap(UP);
					}
				}
				else
				{
					MsgBox(_T("InitReelmapInner()�� ���� GetItsSerialInfo�� ������ �����ϴ�.")); // syd-20231127
					return FALSE;
				}
			}
		}
	}

	if (m_pDlgMenu01)
	{
		m_pDlgMenu01->InitGL();
		m_bDrawGL_Menu01 = TRUE;
		m_pDlgMenu01->RefreshRmap();
		m_pDlgMenu01->InitCadImg();
		m_pDlgMenu01->SetPnlNum();
		m_pDlgMenu01->SetPnlDefNum();
	}

	if (m_pDlgMenu02)
	{
		m_pDlgMenu02->ChgModelUp(); // PinImg, AlignImg�� Display��.
		m_pDlgMenu02->InitCadImg();
	}
#ifndef TEST_MODE
	if (m_pDlgMenu01)
		m_pDlgMenu01->RedrawWindow();

	DispMsg(_T("������ �ʱ�ȭ�մϴ�."), _T("�˸�"), RGB_GREEN, DELAY_TIME_MSG);
	OpenReelmap();
#endif
	SetPathAtBuf(); // Reelmap path�� ������.
	//LoadPcrFromBuf();

	int nSrl = pDoc->GetLastShotMk();
	SetMkFdLen();
	if (nSrl >= 0)
	{
		if (bDualTest)
			m_pDlgMenu01->SelMap(ALL);
		else
			m_pDlgMenu01->SelMap(UP);
	}

	//if (m_pDlgMenu01)
	//{
	//	m_pDlgMenu01->InitGL();
	//	m_pDlgMenu01->SetPnlNum();
	//	m_pDlgMenu01->SetPnlDefNum();
	//}

	return TRUE;
}

BOOL CGvisR2R_PunchView::IsPinMkData()
{
	if (pDoc->IsPinMkData())
	{
		// 		if(m_pDlgMenu02)
		// 			m_pDlgMenu02->SetPcsOffset();
		return TRUE;
	}
	return FALSE;
}

BOOL CGvisR2R_PunchView::IsPinData()
{
	return pDoc->IsPinData();
}

// BOOL CGvisR2R_PunchView::IsMkOffsetData()
// {
// 	return pDoc->IsMkOffsetData();
// }

BOOL CGvisR2R_PunchView::CopyDefImg(int nSerial)
{
	if (nSerial <= 0)
	{
		pView->ClrDispMsg();
		AfxMessageBox(_T("Serial Error.55"));
		return 0;
	}

	return pDoc->CopyDefImg(nSerial);
}

BOOL CGvisR2R_PunchView::CopyDefImg(int nSerial, CString sNewLot)
{
	if (nSerial <= 0)
	{
		pView->ClrDispMsg();
		AfxMessageBox(_T("Serial Error.56"));
		return 0;
	}

	return pDoc->CopyDefImg(nSerial, sNewLot);
}

BOOL CGvisR2R_PunchView::CopyDefImgUp(int nSerial, CString sNewLot)
{
	if (nSerial <= 0)
	{
		pView->ClrDispMsg();
		AfxMessageBox(_T("Serial Error.57"));
		return 0;
	}

	return pDoc->CopyDefImgUp(nSerial, sNewLot);
}

BOOL CGvisR2R_PunchView::CopyDefImgDn(int nSerial, CString sNewLot)
{
	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;
	if (!bDualTest)
		return FALSE;

	if (nSerial <= 0)
	{
		pView->ClrDispMsg();
		AfxMessageBox(_T("Serial Error.58"));
		return FALSE;
	}

	return pDoc->CopyDefImgDn(nSerial, sNewLot);
}

BOOL CGvisR2R_PunchView::MovePinPos()
{
	if (!m_pDlgMenu02)
		return FALSE;

	return m_pDlgMenu02->MovePinPos();
}


BOOL CGvisR2R_PunchView::TwoPointAlign0(int nPos)
{
	if (!m_pDlgMenu02)
		return FALSE;

	return m_pDlgMenu02->Do2PtAlign0(nPos);
}

BOOL CGvisR2R_PunchView::TwoPointAlign1(int nPos)
{
	if (!m_pDlgMenu02)
		return FALSE;

	return m_pDlgMenu02->Do2PtAlign1(nPos);
}

BOOL CGvisR2R_PunchView::FourPointAlign0(int nPos)
{
	if (!m_pDlgMenu02)
		return FALSE;

	return m_pDlgMenu02->Do4PtAlign0(nPos);
}

BOOL CGvisR2R_PunchView::FourPointAlign1(int nPos)
{
	if (!m_pDlgMenu02)
		return FALSE;

	return m_pDlgMenu02->Do4PtAlign1(nPos);
}

BOOL CGvisR2R_PunchView::OnePointAlign(CfPoint &ptPnt)
{
	if (!m_pDlgMenu02)
		return FALSE;

	return m_pDlgMenu02->OnePointAlign(ptPnt);
}

BOOL CGvisR2R_PunchView::TwoPointAlign() // return FALSE; �ΰ�� ����.
{
	if (!m_pDlgMenu02)
		return FALSE;

	BOOL bRtn[2];

	// TwoPointAlign(0)
	bRtn[0] = m_pDlgMenu02->TwoPointAlign(0);

	if (!bRtn[0])
		return FALSE;

	// TwoPointAlign(1)
	bRtn[1] = m_pDlgMenu02->TwoPointAlign(1); // Align ����.

	if (bRtn[0] && bRtn[1])
		return TRUE;

	return FALSE;
}

BOOL CGvisR2R_PunchView::IsNoMk()
{
	BOOL bNoMk = (pDoc->WorkingInfo.System.bNoMk | m_bCam); // pDoc->WorkingInfo.LastJob.bVerify
	return bNoMk;
}

BOOL CGvisR2R_PunchView::IsNoMk0()
{
	BOOL bNoMk = (pDoc->WorkingInfo.System.bNoMk | m_bCam); // pDoc->WorkingInfo.LastJob.bVerify
	return bNoMk;
}

BOOL CGvisR2R_PunchView::IsNoMk1()
{
	BOOL bNoMk = (pDoc->WorkingInfo.System.bNoMk | m_bCam); // pDoc->WorkingInfo.LastJob.bVerify
	return bNoMk;
}

BOOL CGvisR2R_PunchView::IsHomeDone(int nMsId)
{
	if (!m_pMotion)
		return FALSE;

	return m_pMotion->IsHomeDone(nMsId);
}

BOOL CGvisR2R_PunchView::GetAoiUpInfo(int nSerial, int *pNewLot, BOOL bFromBuf)
{
	if (nSerial <= 0)
	{
		pView->ClrDispMsg();
		AfxMessageBox(_T("Serial Error.59"));
		return 0;
	}

	return pDoc->GetAoiUpInfo(nSerial, pNewLot, bFromBuf);
}

BOOL CGvisR2R_PunchView::GetAoiDnInfo(int nSerial, int *pNewLot, BOOL bFromBuf)
{
	if (nSerial <= 0)
	{
		pView->ClrDispMsg();
		AfxMessageBox(_T("Serial Error.60"));
		return 0;
	}

	return pDoc->GetAoiDnInfo(nSerial, pNewLot, bFromBuf);
}

BOOL CGvisR2R_PunchView::LoadMySpec()
{
	return pDoc->LoadMySpec();
}

CString CGvisR2R_PunchView::GetProcessNum()
{
	return pDoc->GetProcessNum();
}

BOOL CGvisR2R_PunchView::GetAoiUpOffset(CfPoint &OfSt)
{
	return pDoc->GetAoiUpOffset(OfSt);
}

BOOL CGvisR2R_PunchView::GetAoiDnOffset(CfPoint &OfSt)
{
	return pDoc->GetAoiDnOffset(OfSt);
}

BOOL CGvisR2R_PunchView::GetMkOffset(CfPoint &OfSt)
{
	if (m_pDlgMenu02)
	{
		OfSt.x = m_pDlgMenu02->m_dMkFdOffsetX[0][0]; // -: ��ǰ ����, +: ��ǰ ����.
		OfSt.y = m_pDlgMenu02->m_dMkFdOffsetY[0][0]; // -: ��ǰ ����, +: ��ǰ ����.
	}

	return TRUE;
}

BOOL CGvisR2R_PunchView::IsAoiLdRun()
{
	return TRUE;

	BOOL bRtn = FALSE;
	if (m_pDlgMenu03)
		bRtn = m_pDlgMenu03->IsAoiLdRun();

	return bRtn;
}

BOOL CGvisR2R_PunchView::IsInitPos0()
{
	if (!m_pMotion)
		return FALSE;

	double pTgtPos[2];
	pTgtPos[0] = _tstof(pDoc->WorkingInfo.Motion.sStPosX[0]);
	pTgtPos[1] = _tstof(pDoc->WorkingInfo.Motion.sStPosY[0]);
	double dCurrX = m_dEnc[AXIS_X0];
	double dCurrY = m_dEnc[AXIS_Y0];

	if (dCurrX < pTgtPos[0] - 2.0 || dCurrX > pTgtPos[0] + 2.0)
		return FALSE;
	if (dCurrY < pTgtPos[1] - 2.0 || dCurrY > pTgtPos[1] + 2.0)
		return FALSE;

	return TRUE;
}

void CGvisR2R_PunchView::MoveInitPos0(BOOL bWait)
{
	if (!m_pMotion)
		return;

	double pTgtPos[2];
	pTgtPos[0] = _tstof(pDoc->WorkingInfo.Motion.sStPosX[0]);
	pTgtPos[1] = _tstof(pDoc->WorkingInfo.Motion.sStPosY[0]);
	double dCurrX = m_dEnc[AXIS_X0];
	double dCurrY = m_dEnc[AXIS_Y0];

	double fLen, fVel, fAcc, fJerk;
	fLen = sqrt(((pTgtPos[0] - dCurrX) * (pTgtPos[0] - dCurrX)) + ((pTgtPos[1] - dCurrY) * (pTgtPos[1] - dCurrY)));
	if (fLen > 0.001)
	{
		m_pMotion->GetSpeedProfile0(TRAPEZOIDAL, AXIS_X0, fLen, fVel, fAcc, fJerk);
		if(bWait)
			m_pMotion->Move0(MS_X0Y0, pTgtPos, fVel, fAcc, fAcc, ABS, WAIT);
		else
			m_pMotion->Move0(MS_X0Y0, pTgtPos, fVel, fAcc, fAcc, ABS, NO_WAIT);
	}
}

BOOL CGvisR2R_PunchView::IsInitPos1()
{
	if (!m_pMotion)
		return FALSE;

	double pTgtPos[2];
	pTgtPos[0] = _tstof(pDoc->WorkingInfo.Motion.sStPosX[1]);
	pTgtPos[1] = _tstof(pDoc->WorkingInfo.Motion.sStPosY[1]);
	double dCurrX = m_dEnc[AXIS_X1];
	double dCurrY = m_dEnc[AXIS_Y1];

	if (dCurrX < pTgtPos[0] - 2.0 || dCurrX > pTgtPos[0] + 2.0)
		return FALSE;
	if (dCurrY < pTgtPos[1] - 2.0 || dCurrY > pTgtPos[1] + 2.0)
		return FALSE;

	return TRUE;
}

BOOL CGvisR2R_PunchView::IsMkEdPos1()
{
	if (!m_pMotion)
		return FALSE;

	double pTgtPos[2];
	pTgtPos[0] = _tstof(pDoc->WorkingInfo.Motion.sMkEdPosX[1]);
	pTgtPos[1] = _tstof(pDoc->WorkingInfo.Motion.sMkEdPosY[1]);
	double dCurrX = m_dEnc[AXIS_X1];
	double dCurrY = m_dEnc[AXIS_Y1];

	if (dCurrX < pTgtPos[0] - 2.0 || dCurrX > pTgtPos[0] + 2.0)
		return FALSE;
	if (dCurrY < pTgtPos[1] - 2.0 || dCurrY > pTgtPos[1] + 2.0)
		return FALSE;

	return TRUE;
}

void CGvisR2R_PunchView::MoveInitPos1(BOOL bWait)
{
	if (!m_pMotion)
		return;

	double pTgtPos[2];
	pTgtPos[0] = _tstof(pDoc->WorkingInfo.Motion.sStPosX[1]);
	pTgtPos[1] = _tstof(pDoc->WorkingInfo.Motion.sStPosY[1]);
	double dCurrX = m_dEnc[AXIS_X1];
	double dCurrY = m_dEnc[AXIS_Y1];

	double fLen, fVel, fAcc, fJerk;
	fLen = sqrt(((pTgtPos[0] - dCurrX) * (pTgtPos[0] - dCurrX)) + ((pTgtPos[1] - dCurrY) * (pTgtPos[1] - dCurrY)));
	if (fLen > 0.001)
	{
		m_pMotion->GetSpeedProfile1(TRAPEZOIDAL, AXIS_X1, fLen, fVel, fAcc, fJerk);
		if(bWait)
			m_pMotion->Move1(MS_X1Y1, pTgtPos, fVel, fAcc, fAcc, ABS, WAIT);
		else
			m_pMotion->Move1(MS_X1Y1, pTgtPos, fVel, fAcc, fAcc, ABS, NO_WAIT);
	}
}

void CGvisR2R_PunchView::MoveMkEdPos1()
{
	if (!m_pMotion)
		return;

	double pTgtPos[2];
	pTgtPos[0] = _tstof(pDoc->WorkingInfo.Motion.sMkEdPosX[1]);
	pTgtPos[1] = _tstof(pDoc->WorkingInfo.Motion.sMkEdPosY[1]);
	double dCurrX = m_dEnc[AXIS_X1];
	double dCurrY = m_dEnc[AXIS_Y1];

	double fLen, fVel, fAcc, fJerk;
	fLen = sqrt(((pTgtPos[0] - dCurrX) * (pTgtPos[0] - dCurrX)) + ((pTgtPos[1] - dCurrY) * (pTgtPos[1] - dCurrY)));
	if (fLen > 0.001)
	{
		m_pMotion->GetSpeedProfile1(TRAPEZOIDAL, AXIS_X1, fLen, fVel, fAcc, fJerk);
		m_pMotion->Move1(MS_X1Y1, pTgtPos, fVel, fAcc, fAcc, ABS, NO_WAIT);
	}
}


void CGvisR2R_PunchView::LotEnd()
{
	if (m_pDlgMenu01)
		m_pDlgMenu01->LotEnd();
	if (m_pDlgMenu03)
		m_pDlgMenu03->SwAoiLotEnd(TRUE);

	m_bCont = FALSE;
	SetLotEd();

	MakeResultMDS();

	if (pDoc->GetTestMode() == MODE_INNER || pDoc->GetTestMode() == MODE_OUTER)
	{
		FinalCopyItsFiles();
	}

	if (pDoc->m_pReelMap == pDoc->m_pReelMapUp)
	{
		DuplicateRmap(RMAP_UP);
	}
	else if (pDoc->m_pReelMap == pDoc->m_pReelMapAllUp)
	{
		DuplicateRmap(RMAP_ALLUP);
	}
	else if (pDoc->m_pReelMap == pDoc->m_pReelMapIts)
	{
		DuplicateRmap(RMAP_ITS);
	}

	if (m_pEngrave)
		m_pEngrave->SwMenu01DispDefImg(TRUE);
}

void CGvisR2R_PunchView::TimWinker(int nId, int nDly) // 0:Ready, 1:Reset, 2:Run, 3:Stop
{
	m_bBtnWinker[nId] = TRUE;
	if (!m_bTimBtnWinker)
	{
		m_bTimBtnWinker = TRUE;
		m_nDlyWinker[nId] = nDly;
		SetTimer(TIM_BTN_WINKER, 100, NULL);
	}
	// 	DispBtnWinker();
}

void CGvisR2R_PunchView::StopTimWinker(int nId) // 0:Ready, 1:Reset, 2:Run, 3:Stop
{
	m_bBtnWinker[nId] = FALSE;
	m_bTimBtnWinker = FALSE;
	for (int i = 0; i<4; i++)
	{
		if (m_bBtnWinker[i])
			m_bTimBtnWinker = TRUE;
	}
	// 	DispBtnWinker();
}

void CGvisR2R_PunchView::Winker(int nId, int nDly) // 0:Ready, 1:Reset, 2:Run, 3:Stop
{
#ifdef USE_MPE
	if (nId == MN_RUN)
	{
		if (pView->m_pMpe)
		{
			m_bBtnWinker[nId] = TRUE;
			//IoWrite(_T("MB44015D"), 1); // �ڵ� �ʱ� ��������(PC�� On/Off ��Ŵ, PLC�� ���������� ��ũ����, on->off�� �������� on, �ٽ� ��������ġ�� �������� off) - 20141017
			pView->m_pMpe->Write(_T("MB44015D"), 1); // �ڵ� �ʱ� ��������(PC�� On/Off ��Ŵ, PLC�� ���������� ��ũ����, on->off�� �������� on, �ٽ� ��������ġ�� �������� off) - 20141017
		}
	}
	// 	DispBtnWinker(nDly);
#endif
}

void CGvisR2R_PunchView::ResetWinker() // 0:Ready, 1:Reset, 2:Run, 3:Stop
{
#ifdef USE_MPE
	//IoWrite(_T("MB44015D"), 0); // �ڵ� �ʱ� ��������(PC�� On/Off ��Ŵ, PLC�� ���������� ��ũ����, on->off�� �������� on, �ٽ� ��������ġ�� �������� off) - 20141017
	pView->m_pMpe->Write(_T("MB44015D"), 0);
#endif
	for (int i = 0; i<4; i++)
	{
		m_bBtnWinker[i] = FALSE;
		m_nCntBtnWinker[i] = FALSE;
	}
}

void CGvisR2R_PunchView::SetOrigin()
{
}

BOOL CGvisR2R_PunchView::IsSetLotEnd()
{
	if (m_nLotEndSerial > 0)
		return TRUE;
	return FALSE;
}

void CGvisR2R_PunchView::SetLotEnd(int nSerial)
{
	if (nSerial <= 0)
	{
		pView->ClrDispMsg();
		AfxMessageBox(_T("Serial Error.61"));
		return;
	}

	m_nLotEndSerial = nSerial;
	m_nAoiLastSerial[0] = nSerial;

	CString str;
	str.Format(_T("%d"), m_nLotEndSerial);
	DispStsBar(str, 0);
	if (m_pDlgMenu01)
		m_pDlgMenu01->DispLotEndSerial(m_nLotEndSerial);
}

int CGvisR2R_PunchView::GetLotEndSerial()
{
	return m_nLotEndSerial; // ���̺�� �����ϴ� Serial.
}

BOOL CGvisR2R_PunchView::StartLive()
{
	if (StartLive0() && StartLive1())
		return TRUE;

	return  FALSE;
}

BOOL CGvisR2R_PunchView::StartLive0()
{
	BOOL bRtn0 = FALSE;

#ifdef USE_VISION
	if (m_pVision[0])
		bRtn0 = m_pVision[0]->StartLive();
#endif
	if (bRtn0)
		return TRUE;

	return  FALSE;
}

BOOL CGvisR2R_PunchView::StartLive1()
{
	BOOL bRtn1 = FALSE;

#ifdef USE_VISION
	if (m_pVision[1])
		bRtn1 = m_pVision[1]->StartLive();
#endif

	if (bRtn1)
		return TRUE;

	return  FALSE;
}

BOOL CGvisR2R_PunchView::StopLive()
{
#ifdef TEST_MODE
	return TRUE;
#endif

	if (StopLive0() && StopLive1())
		return TRUE;

	return FALSE;
}

BOOL CGvisR2R_PunchView::StopLive0()
{
#ifdef TEST_MODE
	return TRUE;
#endif

	BOOL bRtn0 = FALSE;

#ifdef USE_VISION
	if (m_pVision[0])
		bRtn0 = m_pVision[0]->StopLive();
#endif
	if (bRtn0)
		return TRUE;

	return FALSE;
}

BOOL CGvisR2R_PunchView::StopLive1()
{
#ifdef TEST_MODE
	return TRUE;
#endif

	BOOL bRtn1 = FALSE;

#ifdef USE_VISION
	if (m_pVision[1])
		bRtn1 = m_pVision[1]->StopLive();
#endif

	if (bRtn1)
		return TRUE;

	return FALSE;
}

void CGvisR2R_PunchView::UpdateRst()
{
	if (m_pDlgMenu01)
		m_pDlgMenu01->UpdateRst();
}

// BOOL CGvisR2R_PunchView::IsChkTmpStop()
// {
// 	if(pDoc->WorkingInfo.LastJob.bTempPause)
// 	{
// 		if(m_dEnc[AXIS_RENC] >= m_dTempPauseLen)
// 		{
// 			m_dTempPauseLen += m_dTempPauseLen;
// 			return TRUE;
// 		}
// 	}
// 	return FALSE;
// }

BOOL CGvisR2R_PunchView::IsChkTmpStop()
{
	if (pDoc->WorkingInfo.LastJob.bTempPause)
	{
		double dFdLen = GetMkFdLen();
		if (dFdLen >= _tstof(pDoc->WorkingInfo.LastJob.sTempPauseLen)*1000.0)
		{
			// 			m_dTempPauseLen += m_dTempPauseLen;
			pDoc->WorkingInfo.LastJob.bTempPause = FALSE;
			if (m_pDlgMenu01)
				m_pDlgMenu01->UpdateData();
			return TRUE;
		}
	}
	return FALSE;
}

BOOL CGvisR2R_PunchView::IsVerify()
{
	BOOL bVerify = FALSE;
	int nSerial0 = m_nBufUpSerial[0]; // Cam0
	int nSerial1 = m_nBufUpSerial[1]; // Cam1
	int nPeriod = pDoc->WorkingInfo.LastJob.nVerifyPeriod;
	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;


	if (pDoc->WorkingInfo.LastJob.bVerify)
	{
		double dFdLen = GetMkFdLen();
		double dVerifyLen = _tstof(pDoc->WorkingInfo.LastJob.sVerifyLen)*1000.0;

		if (dFdLen < dVerifyLen)
		{
			if (nSerial0 == 1 || nPeriod == 0 || nPeriod == 1 || nPeriod == 2 || m_bStopF_Verify)
			{
				m_bStopF_Verify = FALSE;
				bVerify = TRUE;
			}
			else
			{
				if(!(nSerial0 % nPeriod) || !(nSerial1 % nPeriod))
					bVerify = TRUE;
			}
		}
		else
		{
			pDoc->WorkingInfo.LastJob.bVerify = FALSE;
			if (m_pDlgMenu01)
				m_pDlgMenu01->UpdateData();
		}
	}

	return bVerify;
}

int CGvisR2R_PunchView::GetVsBufLastSerial()
{
	int nLastShot = pDoc->GetLastShotMk();
	if (nLastShot > 0 && m_bCont)
		return (nLastShot + 4);

	return 4;
}

int CGvisR2R_PunchView::GetVsUpBufLastSerial()
{
	int nLastShot = pDoc->GetLastShotUp();
	if (nLastShot > 0 && m_bCont)
		return (nLastShot + 4);

	return 4;
}

int CGvisR2R_PunchView::GetVsDnBufLastSerial()
{
	int nLastShot = pDoc->GetLastShotDn();
	if (nLastShot > 0 && m_bCont)
		return (nLastShot + 4);

	return 4;
}

BOOL CGvisR2R_PunchView::IsFixPcsUp(int nSerial)
{
	if (!pDoc->m_pReelMapUp)
		return FALSE;

	CString sMsg = _T(""), str = _T("");
	int nStrip, pCol[2500], pRow[2500], nTot, nRepeat;
	BOOL bCont = FALSE;

	if (pDoc->m_pReelMapUp->IsFixPcs(nSerial, pCol, pRow, nTot, bCont))
	{
		int nNodeX = pDoc->m_Master[0].m_pPcsRgn->nCol;
		int nNodeY = pDoc->m_Master[0].m_pPcsRgn->nRow;
		int nStPcsY = nNodeY / MAX_STRIP_NUM;

		if(bCont)
			sMsg.Format(_T("��� ���� �����ҷ� �߻�"));
		else
			sMsg.Format(_T("��� ���� �����ҷ� �߻�"));

		for (int i = 0; i<nTot; i++)
		{
			nStrip = int(pRow[i] / nStPcsY);
			nRepeat = pDoc->m_pReelMapUp->GetRptFixPcs(pCol[i], pRow[i]);
			if (!(i % 5))
			{
				if(nRepeat > 1)
					str.Format(_T("\r\n[%d:%c-%d,%d] (%dȸ)"), nSerial, 'A' + nStrip, pCol[i] + 1, (pRow[i] % nStPcsY) + 1, nRepeat);
				else
					str.Format(_T("\r\n[%d:%c-%d,%d]"), nSerial, 'A' + nStrip, pCol[i] + 1, (pRow[i] % nStPcsY) + 1);
			}
			else
			{
				if (nRepeat > 1)
					str.Format(_T(" , [%d:%c-%d,%d] (%dȸ)"), nSerial, 'A' + nStrip, pCol[i] + 1, (pRow[i] % nStPcsY) + 1, nRepeat);
				else
					str.Format(_T(" , [%d:%c-%d,%d]"), nSerial, 'A' + nStrip, pCol[i] + 1, (pRow[i] % nStPcsY) + 1);
			}

			sMsg += str;
		}
		m_sFixMsg[0] = sMsg;
		// 		TowerLamp(RGB_RED, TRUE);
		// 		Buzzer(TRUE, 0);
		// 		MsgBox(sMsg);
		return TRUE;
	}

	return FALSE;
}

BOOL CGvisR2R_PunchView::IsFixPcsDn(int nSerial)
{
	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;
	if (!bDualTest)
		return FALSE;

	if (!pDoc->m_pReelMapUp)
		return FALSE;

	CString sMsg = _T(""), str = _T("");
	int nStrip, pCol[2500], pRow[2500], nTot, nRepeat;
	BOOL bCont = FALSE;

	if (pDoc->m_pReelMapDn->IsFixPcs(nSerial, pCol, pRow, nTot, bCont))
	{
		int nNodeX = pDoc->m_Master[0].m_pPcsRgn->nCol;
		int nNodeY = pDoc->m_Master[0].m_pPcsRgn->nRow;
		int nStPcsY = nNodeY / MAX_STRIP_NUM;

		if (bCont)
			sMsg.Format(_T("�ϸ� ���� �����ҷ� �߻�"));
		else
			sMsg.Format(_T("�ϸ� ���� �����ҷ� �߻�"));

		for (int i = 0; i<nTot; i++)
		{
			nStrip = int(pRow[i] / nStPcsY);
			nRepeat = pDoc->m_pReelMapDn->GetRptFixPcs(pCol[i], pRow[i]);

			if (!(i % 5))
			{
				if (nRepeat > 1)
					str.Format(_T("\r\n[%d:%c-%d,%d] (%dȸ)"), nSerial, 'A' + nStrip, pCol[i] + 1, (pRow[i] % nStPcsY) + 1, nRepeat);
				else
					str.Format(_T("\r\n[%d:%c-%d,%d]"), nSerial, 'A' + nStrip, pCol[i] + 1, (pRow[i] % nStPcsY) + 1);
			}
			else
			{
				if (nRepeat > 1)
					str.Format(_T(" , [%d:%c-%d,%d] (%dȸ)"), nSerial, 'A' + nStrip, pCol[i] + 1, (pRow[i] % nStPcsY) + 1, nRepeat);
				else
					str.Format(_T(" , [%d:%c-%d,%d]"), nSerial, 'A' + nStrip, pCol[i] + 1, (pRow[i] % nStPcsY) + 1);
			}

			sMsg += str;
		}
		// 		TowerLamp(RGB_RED, TRUE);
		// 		Buzzer(TRUE, 0);
		// 		MsgBox(sMsg);
		m_sFixMsg[1] = sMsg;
		return TRUE;
	}

	return FALSE;
}

BOOL CGvisR2R_PunchView::IsReview()
{
	return (pDoc->WorkingInfo.LastJob.bReview);
}

BOOL CGvisR2R_PunchView::IsReview0()
{
	return (pDoc->WorkingInfo.LastJob.bReview);
}

BOOL CGvisR2R_PunchView::IsReview1()
{
	return (pDoc->WorkingInfo.LastJob.bReview);
}


BOOL CGvisR2R_PunchView::IsJogRtDn()
{
	BOOL bOn = FALSE;
#ifdef USE_MPE
	bOn = pDoc->m_pMpeIb[4] & (0x01 << 10) ? TRUE : FALSE;	// ��ŷ�� JOG ��ư(��)
#endif
	return bOn;
}

BOOL CGvisR2R_PunchView::IsJogRtUp()
{
	BOOL bOn = FALSE;
#ifdef USE_MPE
	bOn = pDoc->m_pMpeIb[4] & (0x01 << 10) ? FALSE : TRUE;	// ��ŷ�� JOG ��ư(��)
#endif
	return bOn;
}

BOOL CGvisR2R_PunchView::IsJogRtDn0()
{
	BOOL bOn = FALSE;
#ifdef USE_MPE
	bOn = pDoc->m_pMpeIb[4] & (0x01 << 10) ? TRUE : FALSE;	// ��ŷ�� JOG ��ư(��)
#endif
	return bOn;
}

BOOL CGvisR2R_PunchView::IsJogRtUp0()
{
	BOOL bOn = FALSE;
#ifdef USE_MPE
	bOn = pDoc->m_pMpeIb[4] & (0x01 << 10) ? FALSE : TRUE;	// ��ŷ�� JOG ��ư(��)
#endif
	return bOn;
}

BOOL CGvisR2R_PunchView::IsJogRtDn1()
{
	BOOL bOn = FALSE;
#ifdef USE_MPE
	bOn = pDoc->m_pMpeIb[4] & (0x01 << 10) ? TRUE : FALSE;	// ��ŷ�� JOG ��ư(��)
#endif
	return bOn;
}

BOOL CGvisR2R_PunchView::IsJogRtUp1()
{
	BOOL bOn = FALSE;
#ifdef USE_MPE
	bOn = pDoc->m_pMpeIo[4] & (0x01 << 10) ? FALSE : TRUE;	// ��ŷ�� JOG ��ư(��)
#endif
	return bOn;
}

//void CGvisR2R_PunchView::OpenShareUp(BOOL bOpen)
//{
//	m_bOpenShareUp = bOpen;
//}

//BOOL CGvisR2R_PunchView::IsOpenShareUp()
//{
//	return m_bOpenShareUp;
//}

//void CGvisR2R_PunchView::OpenShareDn(BOOL bOpen)
//{
//	m_bOpenShareDn = bOpen;
//}

//BOOL CGvisR2R_PunchView::IsOpenShareDn()
//{
//	return m_bOpenShareDn;
//}


void CGvisR2R_PunchView::SwAoiEmg(BOOL bOn)
{
	if (m_pDlgMenu03)
		m_pDlgMenu03->SwAoiEmg(bOn);

	if (bOn)
	{
		// 		pDoc->m_pMpeIo[8] |= (0x01<<0);		// �����Ϸ� ������� ����ġ ����
		// 		pDoc->m_pMpeIo[12] |= (0x01<<0);	// �����Ϸ� ������� ����ġ ����

		// 		pDoc->m_pSliceIo[9] |= (0x01<<14);	// �˻�� ������� ����ġ(����ġ)
		// 		pDoc->m_pSliceIo[9] |= (0x01<<15);	// �˻�� ������� ����ġ(�ĸ�)
		// 		pDoc->m_pSliceIo[6] |= (0x01<<0);	// ��ŷ�� ������� ����ġ(�����)
		// 		pDoc->m_pSliceIo[7] |= (0x01<<0);	// ��ŷ�� ������� ����ġ(����ġ)
	}
	else
	{
		// 		pDoc->m_pMpeIo[8] &= ~(0x01<<0);	// �����Ϸ� ������� ����ġ ����
		// 		pDoc->m_pMpeIo[12] &= ~(0x01<<0);	// �����Ϸ� ������� ����ġ ����

		// 		pDoc->m_pSliceIo[9] &= ~(0x01<<14);	// �˻�� ������� ����ġ(����ġ)
		// 		pDoc->m_pSliceIo[9] &= ~(0x01<<15);	// �˻�� ������� ����ġ(�ĸ�)
		// 		pDoc->m_pSliceIo[6] &= ~(0x01<<0);	// ��ŷ�� ������� ����ġ(�����)
		// 		pDoc->m_pSliceIo[7] &= ~(0x01<<0);	// ��ŷ�� ������� ����ġ(����ġ)
	}

}

BOOL CGvisR2R_PunchView::IsVs()
{
	if (!m_bChkLastProcVs)
	{
		// 		int nTotPnl = int(_tstof(pDoc->WorkingInfo.Motion.sFdMkAoiInitDist) / _tstof(pDoc->WorkingInfo.LastJob.sOnePnlLen));;

		BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;
		if (bDualTest)
		{
			if (GetAoiDnVsStatus())
				return TRUE;
			else if (GetAoiUpVsStatus())
				return TRUE;

			// 			if(m_nBufTot[1] > 2 && m_nBufTot[1] < nTotPnl-1)	// AOI �ϸ� ���� ����.
			// 				return TRUE;
			// 			else 
			// 			{
			// 				nTotPnl += _tstoi(pDoc->WorkingInfo.Motion.sFdAoiAoiDistShot);
			// 				if(m_nBufTot[0] > 2 && m_nBufTot[0] < nTotPnl-1)	// AOI ��� ���� ����. 20160807
			// 					return TRUE;
			// 			}
		}
		else
		{
			if (GetAoiUpVsStatus())
				return TRUE;
			// 			nTotPnl += _tstoi(pDoc->WorkingInfo.Motion.sFdAoiAoiDistShot);
			// 			if(m_nBufTot[0] > 2 && m_nBufTot[0] < nTotPnl-1)	// AOI ��� ���� ����.
			// 				return TRUE;
		}
	}

	return FALSE;
}

BOOL CGvisR2R_PunchView::IsVsUp()
{
	// 	int nTotPnl = int(_tstof(pDoc->WorkingInfo.Motion.sFdMkAoiInitDist) / _tstof(pDoc->WorkingInfo.LastJob.sOnePnlLen));
	// 
	// 	nTotPnl += _tstoi(pDoc->WorkingInfo.Motion.sFdAoiAoiDistShot);
	// 	if(m_nBufTot[0] > 2 && m_nBufTot[0] < nTotPnl-1)	// AOI ��� ���� ����.
	// 		return TRUE;
	// 	
	// 	return FALSE;

	return GetAoiUpVsStatus();
}

BOOL CGvisR2R_PunchView::IsVsDn()
{
	// 	int nTotPnl = int(_tstof(pDoc->WorkingInfo.Motion.sFdMkAoiInitDist) / _tstof(pDoc->WorkingInfo.LastJob.sOnePnlLen));
	// 
	// 	if(m_nBufTot[1] > 2 && m_nBufTot[1] < nTotPnl-1)	// AOI �ϸ� ���� ����.
	// 		return TRUE;
	// 	
	// 	return FALSE;

	return GetAoiDnVsStatus();
}

void CGvisR2R_PunchView::SetDummyUp()
{
	m_nDummy[0]++;
	if (m_nDummy[0] == 3)
	{
		if (!MakeDummyUp(-2))
		{
			pView->ClrDispMsg();
			AfxMessageBox(_T("Error - MakeDummyUp(-2)"));
		}
	}
	else
	{
		if (!MakeDummyUp(-1))
		{
			pView->ClrDispMsg();
			AfxMessageBox(_T("Error - MakeDummyUp(-1)"));
		}
	}
}

void CGvisR2R_PunchView::SetDummyDn()
{
	m_nDummy[1]++;
	if (m_nDummy[1] == 3)
	{
		if (!MakeDummyDn(-2))
		{
			pView->ClrDispMsg();
			AfxMessageBox(_T("Error - MakeDummyDn(-2)"));
	}
	}
	else
	{
		if (!MakeDummyDn(-1))
		{
			pView->ClrDispMsg();
			AfxMessageBox(_T("Error - MakeDummyDn(-1)"));
		}
	}
}

BOOL CGvisR2R_PunchView::MakeDummyUp(int nErr) // AOI ��� ����.
{
	int nSerial = m_nAoiLastSerial[0] + m_nDummy[0];
	FILE *fpPCR = NULL;

	CString sMsg, strRstPath, strRstPath2, sDummyRst;
	strRstPath.Format(_T("%s%s\\%s\\%s\\%04d.RST"), pDoc->WorkingInfo.System.sPathAoiUpVrsData,
		pDoc->WorkingInfo.LastJob.sModelUp,
		pDoc->WorkingInfo.LastJob.sLayerUp,
		pDoc->WorkingInfo.LastJob.sLotUp,
		m_nAoiLastSerial[0]);

	CDataFile *pDataFile = new CDataFile;

	if (!pDataFile->Open(strRstPath))
	{
		sMsg.Format(_T("%s File not found."), strRstPath);
		pView->MsgBox(sMsg);
		delete pDataFile;
		return FALSE;
	}
	CString sLine, sRemain, sNewLine;
	sLine = pDataFile->GetLineString(1);
	int nPos = sLine.Find(',', 0);
	sLine.Delete(0, nPos);
	sNewLine.Format(_T("%d%s"), 0, sLine);
	pDataFile->ReplaceLine(1, sNewLine);

	strRstPath2.Format(_T("%s%s\\%s\\%s\\%04d.RST"), pDoc->WorkingInfo.System.sPathAoiUpVrsData,
		pDoc->WorkingInfo.LastJob.sModelUp,
		pDoc->WorkingInfo.LastJob.sLayerUp,
		pDoc->WorkingInfo.LastJob.sLotUp,
		nSerial);
	char* pRtn = NULL;
	fpPCR = fopen(pRtn = StringToChar(strRstPath2), "w+"); if(pRtn) delete pRtn; pRtn = NULL;
	if (fpPCR == NULL)
	{
		pView->ClrDispMsg();
		AfxMessageBox(_T("TROUBLE_SAVE_PIECEOUT_VRS"), MB_ICONWARNING | MB_OK);
	}
	fprintf(fpPCR, pRtn = StringToChar(pDataFile->GetAllString())); if (pRtn) delete pRtn; pRtn = NULL;
	if (fpPCR != NULL)
		fclose(fpPCR);

	delete pDataFile;


	CString sDummyPath;
	sDummyPath.Format(_T("%s%s\\%s\\%s\\%04d.PCR"), pDoc->WorkingInfo.System.sPathAoiUpVrsData,
		pDoc->WorkingInfo.LastJob.sModelUp,
		pDoc->WorkingInfo.LastJob.sLayerUp,
		pDoc->WorkingInfo.LastJob.sLotUp,
		nSerial);

	fpPCR = fopen(pRtn = StringToChar(sDummyPath), "w+"); if (pRtn) delete pRtn; pRtn = NULL;
	if (fpPCR == NULL)
	{
		pView->ClrDispMsg();
		AfxMessageBox(_T("TROUBLE_SAVE_PIECEOUT_VRS"), MB_ICONWARNING | MB_OK);
	}

	fprintf(fpPCR, "%04d,%s,%s,%s\n", nErr, pDoc->WorkingInfo.LastJob.sModelUp,
		pDoc->WorkingInfo.LastJob.sLayerUp,
		pDoc->WorkingInfo.LastJob.sLotUp);

	fprintf(fpPCR, "%04d\n", 0);

	fprintf(fpPCR, "%s", pRtn = StringToChar(_T("���ϸ�����"))); if (pRtn) delete pRtn; pRtn = NULL;
	fprintf(fpPCR, "\n");

	if (fpPCR != NULL)
		fclose(fpPCR);



	sDummyPath.Format(_T("%s%04d.pcr"), pDoc->WorkingInfo.System.sPathVsShareUp, nSerial);

	fpPCR = fopen(pRtn = StringToChar(sDummyPath), "w+"); if(pRtn) delete pRtn; pRtn = NULL;
	if (fpPCR == NULL)
	{
		pView->ClrDispMsg();
		AfxMessageBox(_T("TROUBLE_SAVE_PIECEOUT_VRS"), MB_ICONWARNING | MB_OK);
	}

	fprintf(fpPCR, "%04d,%s,%s,%s\n", nErr, pDoc->WorkingInfo.LastJob.sModelUp,
		pDoc->WorkingInfo.LastJob.sLayerUp,
		pDoc->WorkingInfo.LastJob.sLotUp);

	fprintf(fpPCR, "%04d\n", 0);

	fprintf(fpPCR, "%s", pRtn = StringToChar(_T("���ϸ�����"))); if (pRtn) delete pRtn; pRtn = NULL;
	fprintf(fpPCR, "\n");

	if (fpPCR != NULL)
		fclose(fpPCR);

	return TRUE;
}

BOOL CGvisR2R_PunchView::MakeDummyDn(int nErr) // AOI ��� ����.
{
	m_nAoiLastSerial[1] = m_nAoiLastSerial[0] - 3;
	int nSerial = m_nAoiLastSerial[0] + m_nDummy[1];
	FILE *fpPCR = NULL;

	CString sMsg, strRstPath, strRstPath2, sDummyRst;
	strRstPath.Format(_T("%s%s\\%s\\%s\\%04d.RST"), pDoc->WorkingInfo.System.sPathAoiDnVrsData,
		pDoc->WorkingInfo.LastJob.sModelUp,
		//pDoc->WorkingInfo.LastJob.sModelDn,
		pDoc->WorkingInfo.LastJob.sLayerDn,
		pDoc->WorkingInfo.LastJob.sLotUp,
		//pDoc->WorkingInfo.LastJob.sLotDn,
		m_nAoiLastSerial[1]);

	CDataFile *pDataFile = new CDataFile;
	char* pRtn = NULL;
	if (!pDataFile->Open(strRstPath))
	{
		sMsg.Format(_T("%s File not found."), strRstPath);
		pView->MsgBox(sMsg);
		delete pDataFile;
		return FALSE;
	}
	CString sLine, sRemain, sNewLine;
	sLine = pDataFile->GetLineString(1);
	int nPos = sLine.Find(',', 0);
	sLine.Delete(0, nPos);
	sNewLine.Format(_T("%d%s"), 0, sLine);
	pDataFile->ReplaceLine(1, sNewLine);

	strRstPath2.Format(_T("%s%s\\%s\\%s\\%04d.RST"), pDoc->WorkingInfo.System.sPathAoiDnVrsData,
		pDoc->WorkingInfo.LastJob.sModelUp,
		//pDoc->WorkingInfo.LastJob.sModelDn,
		pDoc->WorkingInfo.LastJob.sLayerDn,
		pDoc->WorkingInfo.LastJob.sLotUp,
		//pDoc->WorkingInfo.LastJob.sLotDn,
		nSerial);
	fpPCR = fopen(pRtn = StringToChar(strRstPath2), "w+"); if (pRtn) delete pRtn; pRtn = NULL;
	if (fpPCR == NULL)
	{
		pView->ClrDispMsg();
		AfxMessageBox(_T("TROUBLE_SAVE_PIECEOUT_VRS"), MB_ICONWARNING | MB_OK);
	}
	fprintf(fpPCR, pRtn = StringToChar(pDataFile->GetAllString())); if (pRtn) delete pRtn; pRtn = NULL;
	if (fpPCR != NULL)
		fclose(fpPCR);

	delete pDataFile;


	CString sDummyPath;
	sDummyPath.Format(_T("%s%s\\%s\\%s\\%04d.PCR"), pDoc->WorkingInfo.System.sPathAoiDnVrsData,
		pDoc->WorkingInfo.LastJob.sModelUp,
		//pDoc->WorkingInfo.LastJob.sModelDn,
		pDoc->WorkingInfo.LastJob.sLayerDn,
		pDoc->WorkingInfo.LastJob.sLotUp,
		//pDoc->WorkingInfo.LastJob.sLotDn,
		nSerial);

	fpPCR = fopen(pRtn = StringToChar(sDummyPath), "w+"); if (pRtn) delete pRtn; pRtn = NULL;
	if (fpPCR == NULL)
	{
		pView->ClrDispMsg();
		AfxMessageBox(_T("TROUBLE_SAVE_PIECEOUT_VRS"), MB_ICONWARNING | MB_OK);
	}

	fprintf(fpPCR, "%04d,%s,%s,%s\n", nErr, pDoc->WorkingInfo.LastJob.sModelUp,
		pDoc->WorkingInfo.LastJob.sLayerDn,
		pDoc->WorkingInfo.LastJob.sLotUp);
	//fprintf(fpPCR, "%04d,%s,%s,%s\n", nErr, pDoc->WorkingInfo.LastJob.sModelDn,
	//	pDoc->WorkingInfo.LastJob.sLayerDn,
	//	pDoc->WorkingInfo.LastJob.sLotDn);

	fprintf(fpPCR, "%04d\n", 0);

	fprintf(fpPCR, "%s", pRtn = StringToChar(_T("���ϸ�����"))); if (pRtn) delete pRtn; pRtn = NULL;
	fprintf(fpPCR, "\n");

	if (fpPCR != NULL)
		fclose(fpPCR);



	sDummyPath.Format(_T("%s%04d.pcr"), pDoc->WorkingInfo.System.sPathVsShareDn, nSerial);

	fpPCR = fopen(pRtn = StringToChar(sDummyPath), "w+"); delete pRtn;
	if (fpPCR == NULL)
	{
		pView->ClrDispMsg();
		AfxMessageBox(_T("TROUBLE_SAVE_PIECEOUT_VRS"), MB_ICONWARNING | MB_OK);
	}

	fprintf(fpPCR, "%04d,%s,%s,%s\n", nErr, pDoc->WorkingInfo.LastJob.sModelUp,
		pDoc->WorkingInfo.LastJob.sLayerDn,
		pDoc->WorkingInfo.LastJob.sLotUp);
	//fprintf(fpPCR, "%04d,%s,%s,%s\n", nErr, pDoc->WorkingInfo.LastJob.sModelDn,
	//	pDoc->WorkingInfo.LastJob.sLayerDn,
	//	pDoc->WorkingInfo.LastJob.sLotDn);

	fprintf(fpPCR, "%04d\n", 0);

	fprintf(fpPCR, "%s", pRtn = StringToChar(_T("���ϸ�����"))); if (pRtn) delete pRtn; pRtn = NULL;
	fprintf(fpPCR, "\n");

	if (fpPCR != NULL)
		fclose(fpPCR);

	return TRUE;
}

void CGvisR2R_PunchView::SetAoiDummyShot(int nAoi, int nDummy)
{
	pDoc->AoiDummyShot[nAoi] = 0;
	switch (nAoi)
	{
	case 0:
		if (m_pMpe)
			m_pMpe->Write(_T("ML45068"), (long)0);	// �˻�� (��) �۾����� ���� Shot�� - 20141104
		break;
	case 1:
		if (m_pMpe)
			m_pMpe->Write(_T("ML45070"), (long)0);	// �˻�� (��) �۾����� ���� Shot�� - 20141104
		break;
	}
	return;

	//pDoc->AoiDummyShot[nAoi] = nDummy;
	//switch (nAoi)
	//{
	//case 0:
	//	if (m_pMpe)
	//		m_pMpe->Write(_T("ML45068"), (long)nDummy);	// �˻�� (��) �۾����� ���� Shot�� - 20141104
	//	break;
	//case 1:
	//	if (m_pMpe)
	//		m_pMpe->Write(_T("ML45070"), (long)nDummy);	// �˻�� (��) �۾����� ���� Shot�� - 20141104
	//	break;
	//}
}

int CGvisR2R_PunchView::GetAoiUpDummyShot()
{
	int nDummy = 0;
	//char szData[200];
	TCHAR szData[200];
	CString sPath = pDoc->WorkingInfo.System.sPathAoiUpCurrInfo;
	if (0 < ::GetPrivateProfileString(_T("Dummy"), _T("Shot"), NULL, szData, sizeof(szData), sPath))
		nDummy = _tstoi(szData);
	else
		nDummy = 0;

	return nDummy;
}

int CGvisR2R_PunchView::GetAoiDnDummyShot()
{
	int nDummy = 0;
	//char szData[200];
	TCHAR szData[200];
	CString sPath = pDoc->WorkingInfo.System.sPathAoiDnCurrInfo;
	if (0 < ::GetPrivateProfileString(_T("Dummy"), _T("Shot"), NULL, szData, sizeof(szData), sPath))
		nDummy = _tstoi(szData);
	else
		nDummy = 0;

	return nDummy;
}

int CGvisR2R_PunchView::GetAoiUpSerial()
{
	int nSerial = 0;
	//char szData[200];
	TCHAR szData[200];
	CString sPath = pDoc->WorkingInfo.System.sPathAoiUpCurrInfo;
	if (0 < ::GetPrivateProfileString(_T("Infomation"), _T("Current Serial"), NULL, szData, sizeof(szData), sPath))
		nSerial = _tstoi(szData);
	else
		nSerial = 0;
	if (nSerial > 1)
		nSerial--;

	return nSerial;
}

BOOL CGvisR2R_PunchView::GetAoiUpVsStatus()
{
	BOOL bVsStatus = FALSE;
	//char szData[200];
	TCHAR szData[200];
	CString sPath = pDoc->WorkingInfo.System.sPathAoiUpCurrInfo;
	if (0 < ::GetPrivateProfileString(_T("Infomation"), _T("Current VS Status"), NULL, szData, sizeof(szData), sPath))
		bVsStatus = _tstoi(szData) > 0 ? TRUE : FALSE;

	return bVsStatus;
}

int CGvisR2R_PunchView::GetAoiDnSerial()
{
	int nSerial = 0;
	//char szData[200];
	TCHAR szData[200];
	CString sPath = pDoc->WorkingInfo.System.sPathAoiDnCurrInfo;
	if (0 < ::GetPrivateProfileString(_T("Infomation"), _T("Current Serial"), NULL, szData, sizeof(szData), sPath))
		nSerial = _tstoi(szData);
	else
		nSerial = 0;
	if (nSerial > 1)
		nSerial--;

	return nSerial;
}

BOOL CGvisR2R_PunchView::GetAoiDnVsStatus()
{
	BOOL bVsStatus = FALSE;
	//char szData[200];
	TCHAR szData[200];
	CString sPath = pDoc->WorkingInfo.System.sPathAoiDnCurrInfo;
	if (0 < ::GetPrivateProfileString(_T("Infomation"), _T("Current VS Status"), NULL, szData, sizeof(szData), sPath))
		bVsStatus = _tstoi(szData) > 0 ? TRUE : FALSE;

	return bVsStatus;
}

BOOL CGvisR2R_PunchView::IsDoneDispMkInfo()
{
	BOOL bRtn = FALSE;
	if (m_pDlgMenu01)
		bRtn = m_pDlgMenu01->IsDoneDispMkInfo();
	return bRtn;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void CGvisR2R_PunchView::Marking()
{
}

void CGvisR2R_PunchView::DoReject0()
{
#ifdef TEST_MODE
	return;
#endif

	//BOOL bOn;
	int nIdx, nRtn;//nSerial, 
	CfPoint ptPnt;

	if (!IsRun() && m_bAuto)
	{
		if (IsOnMarking0())
		{
			if (m_pMotion->IsEnable(MS_X0) && m_pMotion->IsEnable(MS_Y0))
			{
				if (m_pMotion->IsMotionDone(MS_X0) && m_pMotion->IsMotionDone(MS_Y0))
				{
					if (!IsInitPos0() && !IsPinPos0())
						MoveInitPos0();
				}
			}
		}
		return;
	}


	switch (m_nStepMk[2])
	{
	case 0:
		if (IsNoMk())
			ShowLive();
		m_nMkStrip[0][0] = 0;
		m_nMkStrip[0][1] = 0;
		m_nMkStrip[0][2] = 0;
		m_nMkStrip[0][3] = 0;
		m_bRejectDone[0][0] = FALSE;
		m_bRejectDone[0][1] = FALSE;
		m_bRejectDone[0][2] = FALSE;
		m_bRejectDone[0][3] = FALSE;
		m_nStepMk[2]++;
		break;
	case 1:
		if (m_nMkPcs[2] < pDoc->m_Master[0].m_pPcsRgn->nTotPcs)
		{
			m_nStepMk[2]++;
		}
		else
		{
			m_nStepMk[2] = MK_END;
		}
		break;
	case 2:
		m_nStepMk[2]++;
		break;
	case 3:
		if (m_nMkPcs[2] < pDoc->m_Master[0].m_pPcsRgn->nTotPcs)	// ��ŷ�Ϸ�Check
		{
			ptPnt = GetMkPnt0(m_nMkPcs[2]);

			if (ptPnt.x < 0.0 && ptPnt.y < 0.0) // ��ǰȭ.
			{
				m_nMkPcs[2]++;
				break;
			}

			nIdx = GetMkStripIdx0(m_nMkPcs[2]);
			if (nIdx > 0)
			{
				if (!IsMkStrip(nIdx)) // Strip[] Mk Off
				{
					m_nMkPcs[2]++;
					break;
				}
				else
				{
					if (m_nMkStrip[0][nIdx - 1] > pDoc->GetStripRejectMkNum())
					{
						m_bRejectDone[0][nIdx - 1] = TRUE;
						m_nMkPcs[2]++;
						break;
					}
					else
						m_nMkStrip[0][nIdx - 1]++;
				}
			}
			else
			{
				pView->ClrDispMsg();
				AfxMessageBox(_T("Strip Index Failed."));
				break;
			}

			if (m_bRejectDone[0][0] && m_bRejectDone[0][1] &&
				m_bRejectDone[0][2] && m_bRejectDone[0][3])
			{
				m_nStepMk[2] = MK_END;
				break;
			}

			m_dTarget[AXIS_X0] = ptPnt.x;

			// Cam0 : m_bPriority[0], m_bPriority[3]
			// Cam1 : m_bPriority[1], m_bPriority[2]

			m_bCollision[0] = ChkCollision(AXIS_X0, ptPnt.x);
			if (!m_bCollision[0])
			{
				Move0(ptPnt, m_bCam);
				m_nStepMk[2]++;
			}
			else if (m_bPriority[0])
			{
				m_bPriority[0] = FALSE;
				Move0(ptPnt, m_bCam);
				m_nStepMk[2]++;
			}
			else if (m_bPriority[2])
			{
				m_bPriority[2] = FALSE;
				ptPnt.x = 0.0;
				ptPnt.y = 0.0;//m_dEnc[AXIS_Y0];
				Move0(ptPnt, m_bCam);
			}
		}
		else
		{
			m_nStepMk[2] = MK_END;
		}
		break;
	case 4:
		m_nStepMk[2]++;
		break;
	case 5:
		m_nStepMk[2]++;
		break;
	case 6:
		if (IsMoveDone0())
			m_nStepMk[2]++;
		break;
	case 7:
		if (!IsNoMk0())
		{
			m_dwStMkDn[0] = GetTickCount();
			Mk0();
		}
		else
			SetDelay0(pDoc->m_nDelayShow, 1);		// [mSec]
		m_nStepMk[2]++;
		break;
	case 8:
		if (IsNoMk0())
		{
			if (!WaitDelay0(1))		// F:Done, T:On Waiting....
				m_nStepMk[2]++;
		}
		else
			m_nStepMk[2]++;
		break;
	case 9:
		m_nStepMk[2]++;
		break;
	case 10:
		if (!IsNoMk0())
		{
			//Mk0(FALSE);
			if (IsMk0Done())
			{
				m_nMkPcs[2]++;
				m_nStepMk[2]++;
				m_nStepMk[2]++;
			}
			else
			{
				if (m_dwStMkDn[0] + 5000 < GetTickCount())
				{
					Buzzer(TRUE, 0);
					//pView->DispStsBar(_T("����-29"), 0);
					DispMain(_T("�� ��"), RGB_RED);
					m_pVoiceCoil[0]->SearchHomeSmac0();

					//nRtn = AsyncMsgBox(_T("���̽�����(��) ��ſϷᰡ �ʵ˴ϴ�.\r\n��ŷ�� �ٽ� �õ��Ͻðڽ��ϱ�?"), 1, MB_YESNO);
					nRtn = MsgBox(_T("���̽�����(��) ��ſϷᰡ �ʵ˴ϴ�.\r\n��ŷ�� �ٽ� �õ��Ͻðڽ��ϱ�?"), 1, MB_YESNO);
					if (IDYES == nRtn)
					{
						DispMain(_T("������"), RGB_RED);
						m_nStepMk[2] = 7;
					}
					else if (nRtn < 0)
						m_nStepMk[2]++;
					else
					{
						m_nMkPcs[2]++;
						m_nStepMk[2]++;
						m_nStepMk[2]++;
						Stop();
					}
				}
			}
		}
		else
		{
			m_nMkPcs[2]++;
			m_nStepMk[2]++;
			m_nStepMk[2]++;
		}
		break;
	case 11:
		//m_nStepMk[2]++;
		//if(m_bRtnMyMsgBox[2])
		if ((nRtn = WaitRtnVal(3)) > -1)
		{
			//if(IDYES == m_nRtnMyMsgBox[2])
			if (IDYES == nRtn)
			{
				DispMain(_T("������"), RGB_RED);
				m_nStepMk[2] = 7;
			}
			else
			{
				m_nMkPcs[2]++;
				m_nStepMk[2]++;
				Stop();
			}
		}
		break;
	case 12:
		m_nStepMk[2]++;
		break;
	case 13:
		if (m_nMkPcs[2] < pDoc->m_Master[0].m_pPcsRgn->nTotPcs)	// ��ŷ�Ϸ�Check
		{
			m_nStepMk[2] = 3;
		}
		else
		{
			m_nStepMk[2] = MK_END;
		}
		break;
	case MK_END:
		m_pVoiceCoil[0]->SearchHomeSmac0();
		SetDelay0(500, 1);		// [mSec]
		m_nStepMk[2]++;
		break;
	case 101:
		if (!WaitDelay0(1))		// F:Done, T:On Waiting....
		{
			MoveInitPos0();
			m_nStepMk[2]++;
		}
		break;
	case 102:
		if (IsMoveDone0())
		{
			m_bTHREAD_MK[2] = FALSE;
			m_bDoneMk[0] = TRUE;
			m_nStepMk[2] = 0;
			m_nMkStrip[0][0] = 0;
			m_nMkStrip[0][1] = 0;
			m_nMkStrip[0][2] = 0;
			m_nMkStrip[0][3] = 0;
		}
		break;
	}
}

void CGvisR2R_PunchView::DoReject1()
{
#ifdef TEST_MODE
	return;
#endif

	//BOOL bOn;
	int nIdx, nRtn;//nSerial, 
	CfPoint ptPnt;

	if (!IsRun() && m_bAuto)
	{
		if (IsOnMarking1())
		{
			if (m_pMotion->IsEnable(MS_X1) && m_pMotion->IsEnable(MS_Y1))
			{
				if (m_pMotion->IsMotionDone(MS_X1) && m_pMotion->IsMotionDone(MS_Y1))
				{
					if (!IsMkEdPos1() && !IsPinPos1())
						MoveMkEdPos1();
				}
			}
		}
		return;
	}


	switch (m_nStepMk[3])
	{
	case 0:
		if (IsNoMk())
			ShowLive();
		m_nMkStrip[1][0] = 0;
		m_nMkStrip[1][1] = 0;
		m_nMkStrip[1][2] = 0;
		m_nMkStrip[1][3] = 0;
		m_bRejectDone[1][0] = FALSE;
		m_bRejectDone[1][1] = FALSE;
		m_bRejectDone[1][2] = FALSE;
		m_bRejectDone[1][3] = FALSE;
		m_nStepMk[3]++;
		break;
	case 1:
		if (m_nMkPcs[3] < pDoc->m_Master[0].m_pPcsRgn->nTotPcs)
		{
			m_nStepMk[3]++;
		}
		else
		{
			m_nStepMk[3] = MK_END;
		}
		break;
	case 2:
		m_nStepMk[3]++;
		break;
	case 3:
		if (m_nMkPcs[3] < pDoc->m_Master[0].m_pPcsRgn->nTotPcs)	// ��ŷ�Ϸ�Check
		{
			ptPnt = GetMkPnt1(m_nMkPcs[3]);

			if (ptPnt.x < 0.0 && ptPnt.y < 0.0) // ��ǰȭ.
			{
				m_nMkPcs[3]++;
				break;
			}

			nIdx = GetMkStripIdx1(m_nMkPcs[3]);
			if (nIdx > 0)
			{
				if (!IsMkStrip(nIdx)) // Strip[] Mk Off
				{
					m_nMkPcs[3]++;
					break;
				}
				else
				{
					if (m_nMkStrip[1][nIdx - 1] > pDoc->GetStripRejectMkNum())
					{
						m_bRejectDone[1][nIdx - 1] = TRUE;
						m_nMkPcs[3]++;
						break;
					}
					else
						m_nMkStrip[1][nIdx - 1]++;
				}
			}
			else
			{
				pView->ClrDispMsg();
				AfxMessageBox(_T("Strip Index Failed."));
				break;
			}

			if (m_bRejectDone[1][0] && m_bRejectDone[1][1] &&
				m_bRejectDone[1][2] && m_bRejectDone[1][3])
			{
				m_nStepMk[2] = MK_END;
				break;
			}

			m_dTarget[AXIS_X1] = ptPnt.x;

			// Cam0 : m_bPriority[0], m_bPriority[3]
			// Cam1 : m_bPriority[1], m_bPriority[2]

			m_bCollision[1] = ChkCollision(AXIS_X1, ptPnt.x);
			if (!m_bCollision[1])
			{
				Move1(ptPnt, m_bCam);
				m_nStepMk[3]++;
			}
			else if (m_bPriority[1])
			{
				m_bPriority[1] = FALSE;
				Move1(ptPnt, m_bCam);
				m_nStepMk[3]++;
			}
			else if (m_bPriority[3])
			{
				m_bPriority[3] = FALSE;
				ptPnt.x = _tstof(pDoc->WorkingInfo.Motion.sSafeZone);
				ptPnt.y = 0.0;//m_dEnc[AXIS_Y1];
				Move1(ptPnt, m_bCam);
			}
		}
		else
		{
			m_nStepMk[3] = MK_END;
		}
		break;
	case 4:
		m_nStepMk[3]++;
		break;
	case 5:
		m_nStepMk[3]++;
		break;
	case 6:
		if (IsMoveDone1())
			m_nStepMk[3]++;
		break;
	case 7:
		if (!IsNoMk1())
		{
			m_dwStMkDn[1] = GetTickCount();
			Mk1();
		}
		else
			SetDelay1(pDoc->m_nDelayShow, 1);		// [mSec]
		m_nStepMk[3]++;
		break;
	case 8:
		if (IsNoMk1())
		{
			if (!WaitDelay1(1))		// F:Done, T:On Waiting....
				m_nStepMk[3]++;
		}
		else
			m_nStepMk[3]++;
		break;
	case 9:
		m_nStepMk[3]++;
		break;
	case 10:
		if (!IsNoMk1())
		{
			if (IsMk1Done())
			{
				m_nMkPcs[3]++;
				m_nStepMk[3]++;
				m_nStepMk[3]++;
			}
			else
			{
				if (m_dwStMkDn[1] + 5000 < GetTickCount())
				{
					Buzzer(TRUE, 0);
					//pView->DispStsBar(_T("����-30"), 0);
					DispMain(_T("�� ��"), RGB_RED);
					m_pVoiceCoil[1]->SearchHomeSmac1();

					//nRtn = AsyncMsgBox(_T("���̽�����(��) ��ſϷᰡ �ʵ˴ϴ�.\r\n��ŷ�� �ٽ� �õ��Ͻðڽ��ϱ�?"), 2, MB_YESNO);
					nRtn = MsgBox(_T("���̽�����(��) ��ſϷᰡ �ʵ˴ϴ�.\r\n��ŷ�� �ٽ� �õ��Ͻðڽ��ϱ�?"), 2, MB_YESNO);
					if (IDYES == nRtn)
					{
						DispMain(_T("������"), RGB_RED);
						m_nStepMk[3] = 7;
					}
					else if (nRtn < 0)
						m_nStepMk[3]++;
					else
					{
						m_nMkPcs[3]++;
						m_nStepMk[3]++;
						m_nStepMk[3]++;
						Stop();
					}
				}
			}
		}
		else
		{
			m_nMkPcs[3]++;
			m_nStepMk[3]++;
			m_nStepMk[3]++;
		}
		break;
	case 11:
		//m_nStepMk[3]++;
		//if(m_bRtnMyMsgBox[3])
		if ((nRtn = WaitRtnVal(4)) > -1)
		{
			//if(IDYES == m_nRtnMyMsgBox[3])
			if (IDYES == nRtn)
			{
				DispMain(_T("������"), RGB_RED);
				m_nStepMk[3] = 7;
			}
			else
			{
				m_nMkPcs[3]++;
				m_nStepMk[3]++;
				Stop();
			}
		}
		break;
	case 12:
		m_nStepMk[3]++;
		break;
	case 13:
		if (m_nMkPcs[3] < pDoc->m_Master[0].m_pPcsRgn->nTotPcs)	// ��ŷ�Ϸ�Check
		{
			m_nStepMk[3] = 3;
		}
		else
		{
			m_nStepMk[3] = MK_END;
		}
		break;
	case MK_END:
		m_pVoiceCoil[1]->SearchHomeSmac1();
		SetDelay1(500, 1);		// [mSec]
		m_nStepMk[3]++;
		break;
	case 101:
		if (!WaitDelay1(1))		// F:Done, T:On Waiting....
		{
			MoveMkEdPos1();
			m_nStepMk[3]++;
		}
		break;
	case 102:
		if (IsMoveDone1())
		{
			m_bTHREAD_MK[3] = FALSE;
			m_bDoneMk[1] = TRUE;
			m_nStepMk[3] = 0;
			m_nMkStrip[1][0] = 0;
			m_nMkStrip[1][1] = 0;
			m_nMkStrip[1][2] = 0;
			m_nMkStrip[1][3] = 0;
		}
		break;
	}
}

void CGvisR2R_PunchView::DoMark0All()
{
	//BOOL bOn;
	//int nSerial;
	CfPoint ptPnt;

	if (!IsRun())
	{
		if (IsOnMarking0())
		{
			if (m_pMotion->IsEnable(MS_X0) && m_pMotion->IsEnable(MS_Y0))
			{
				if (m_pMotion->IsMotionDone(MS_X0) && m_pMotion->IsMotionDone(MS_Y0))
				{
					if (!IsInitPos0() && !IsPinPos0())
						MoveInitPos0();
				}
			}

			if (m_nStepMk[2] < 5 && m_nStepMk[2] > 3) // Mk0();
			{
				m_nStepMk[2] = 3;	// ��ŷ�Ϸ�Check
			}
		}
		return;
	}


	switch (m_nStepMk[2])
	{
	case 0:
		m_nStepMk[2]++;
		break;
	case 1:
		if (m_nMkPcs[2] < pDoc->m_Master[0].m_pPcsRgn->nTotPcs)
		{
			m_nStepMk[2]++;
		}
		else
		{
			m_nStepMk[2] = MK_END;
		}
		break;
	case 2:
		m_nStepMk[2]++;
		break;
	case 3:
		if (m_nMkPcs[2] < pDoc->m_Master[0].m_pPcsRgn->nTotPcs)	// ��ŷ�Ϸ�Check
		{
			ptPnt = GetMkPnt0(m_nMkPcs[2]);

			if (ptPnt.x < 0.0 && ptPnt.y < 0.0) // ��ǰȭ.
			{
				m_nMkPcs[2]++;
				break;
			}

			m_dTarget[AXIS_X0] = ptPnt.x;

			// Cam0 : m_bPriority[0] <-, m_bPriority[3] ->
			// Cam1 : m_bPriority[1] ->, m_bPriority[2] <-

			m_bCollision[0] = ChkCollision(AXIS_X0, ptPnt.x);
			if (!m_bCollision[0])
			{
				if (IsMoveDone0())
				{
					Move0(ptPnt, m_bCam);
					m_nStepMk[2]++;
				}
			}
			else if (m_bPriority[0])
			{
				if (IsMoveDone0())
				{
					m_bPriority[0] = FALSE;
					Move0(ptPnt, m_bCam);
					m_nStepMk[2]++;
				}
			}
			else if (m_bPriority[2])
			{
				if (IsMoveDone0())
				{
					m_bPriority[2] = FALSE;
					ptPnt.x = 0.0;
					ptPnt.y = 0.0;//m_dEnc[AXIS_Y0];
					Move0(ptPnt, FALSE);
				}
			}
		}
		else
		{
			m_nStepMk[2] = MK_END;
		}
		break;
	case 4:
		if (IsMoveDone0())
			m_nStepMk[2]++;
		break;
	case 5:
		if (!IsNoMk0())
			Mk0();
		else
			SetDelay0(pDoc->m_nDelayShow, 1);		// [mSec]
		m_nStepMk[2]++;
		break;
	case 6:
		if (IsNoMk0())
		{
			if (!WaitDelay0(1))		// F:Done, T:On Waiting....
				m_nStepMk[2]++;
		}
		else
			m_nStepMk[2]++;
		break;
	case 7:
		m_nMkPcs[2]++;
		m_nStepMk[2]++;
		break;
	case 8:
		if (m_nMkPcs[2] < pDoc->m_Master[0].m_pPcsRgn->nTotPcs)	// ��ŷ�Ϸ�Check
		{
			m_nStepMk[2] = 3;
		}
		else
		{
			m_nStepMk[2] = MK_END;
		}
		break;
	case MK_END:
		SetDelay0(100, 1);		// [mSec]
		m_nStepMk[2]++;
		break;
	case 101:
		if (!WaitDelay0(1))		// F:Done, T:On Waiting....
		{
			if (IsMoveDone0())
			{
				MoveInitPos0();
				m_nStepMk[2]++;
			}
		}
		break;
	case 102:
		if (IsMoveDone0())
			m_nStepMk[2]++;
		break;
	case 103:
		m_bDoneMk[0] = TRUE;
		m_bTHREAD_MK[2] = FALSE;
		break;
	}
}

void CGvisR2R_PunchView::DoMark1All()
{
	//BOOL bOn;
	//int nSerial;
	CfPoint ptPnt;

	if (!IsRun())
	{
		if (IsOnMarking1())
		{
			if (m_pMotion->IsEnable(MS_X1) && m_pMotion->IsEnable(MS_Y1))
			{
				if (m_pMotion->IsMotionDone(MS_X1) && m_pMotion->IsMotionDone(MS_Y1))
				{
					if (!IsMkEdPos1() && !IsPinPos1())
						MoveMkEdPos1();
				}
			}

			if (m_nStepMk[3] < 5 && m_nStepMk[3] > 3) // Mk1();
			{
				m_nStepMk[3] = 3;	// ��ŷ�Ϸ�Check
			}
		}
		return;
	}


	switch (m_nStepMk[3])
	{
	case 0:
		m_nStepMk[3]++;
		break;
	case 1:
		if (m_nMkPcs[3] < pDoc->m_Master[0].m_pPcsRgn->nTotPcs)
		{
			m_nStepMk[3]++;
		}
		else
		{
			m_nStepMk[3] = MK_END;
		}
		break;
	case 2:
		m_nStepMk[3]++;
		break;
	case 3:
		if (m_nMkPcs[3] < pDoc->m_Master[0].m_pPcsRgn->nTotPcs)	// ��ŷ�Ϸ�Check
		{
			ptPnt = GetMkPnt1(m_nMkPcs[3]);

			if (ptPnt.x < 0.0 && ptPnt.y < 0.0) // ��ǰȭ.
			{
				m_nMkPcs[3]++;
				break;
			}

			m_dTarget[AXIS_X1] = ptPnt.x;

			// Cam0 : m_bPriority[0] <-, m_bPriority[3] ->
			// Cam1 : m_bPriority[1] ->, m_bPriority[2] <-

			m_bCollision[1] = ChkCollision(AXIS_X1, ptPnt.x);
			if (!m_bCollision[1])
			{
				if (IsMoveDone1())
				{
					Move1(ptPnt, m_bCam);
					m_nStepMk[3]++;
				}
			}
			else if (m_bPriority[1])
			{
				if (IsMoveDone1())
				{
					m_bPriority[1] = FALSE;
					Move1(ptPnt, m_bCam);
					m_nStepMk[3]++;
				}
			}
			else if (m_bPriority[3])
			{
				if (IsMoveDone1())
				{
					m_bPriority[3] = FALSE;
					ptPnt.x = _tstof(pDoc->WorkingInfo.Motion.sSafeZone);
					ptPnt.y = 0.0;//m_dEnc[AXIS_Y1];
					Move1(ptPnt, FALSE);
				}
			}
		}
		else
		{
			m_nStepMk[3] = MK_END;
		}
		break;
	case 4:
		if (IsMoveDone1())
			m_nStepMk[3]++;
		break;
	case 5:
		if (!IsNoMk1())
			Mk1();
		else
			SetDelay1(pDoc->m_nDelayShow, 1);		// [mSec]
		m_nStepMk[3]++;
		break;
	case 6:
		if (IsNoMk1())
		{
			if (!WaitDelay1(1))		// F:Done, T:On Waiting....
				m_nStepMk[3]++;
		}
		else
			m_nStepMk[3]++;
		break;
	case 7:
		m_nMkPcs[3]++;
		m_nStepMk[3]++;
		break;
	case 8:
		if (m_nMkPcs[3] < pDoc->m_Master[0].m_pPcsRgn->nTotPcs)	// ��ŷ�Ϸ�Check
		{
			m_nStepMk[3] = 3;
		}
		else
		{
			//StopLive1();
			m_nStepMk[3] = MK_END;
		}
		break;
	case MK_END:
		SetDelay1(100, 1);		// [mSec]
		m_nStepMk[3]++;
		break;
	case 101:
		if (!WaitDelay1(1))		// F:Done, T:On Waiting....
		{
			if (m_nMkPcs[2] < pDoc->m_Master[0].m_pPcsRgn->nTotPcs)
			{
				if (m_nMkPcs[2] > 0)
				{
					if (!(m_Flag & (0x01 << 0)))
					{
						if (IsMoveDone1())
						{
							m_Flag |= (0x01 << 0);
							ptPnt.x = _tstof(pDoc->WorkingInfo.Motion.sSafeZone);
							ptPnt.y = 0.0;//m_dEnc[AXIS_Y1];
							Move1(ptPnt, FALSE);
						}
					}
				}
				else
				{
					if (IsMoveDone1())
					{
						m_Flag &= ~(0x01 << 0);
						MoveMkEdPos1();
						m_nStepMk[3]++;
					}
				}
			}
			else
			{
				if (IsMoveDone1())
				{
					m_Flag &= ~(0x01 << 0);
					MoveMkEdPos1();
					m_nStepMk[3]++;
				}
			}
		}
		break;
	case 102:
		if (IsMoveDone0())
			m_nStepMk[3]++;
		break;
	case 103:
		m_bDoneMk[1] = TRUE;
		m_bTHREAD_MK[3] = FALSE;
		break;
	}
}

void CGvisR2R_PunchView::DoMark0()
{
#ifdef TEST_MODE
	return;
#endif

	if (!m_bAuto)
		return;


	//BOOL bOn;
	int nSerial, nIdx, nErrCode, nRtn;
	CfPoint ptPnt;
	CString sMsg;
	double dStripOut = (pDoc->m_Master[0].m_pPcsRgn->nTotPcs / MAX_STRIP_NUM) * _tstof(pDoc->WorkingInfo.LastJob.sStripOutRatio) / 100.0;
	int nStripOut = int(dStripOut);
	if (dStripOut > nStripOut)
		nStripOut++;			// ��Ʈ�� ���� ����

	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;

	if (!IsRun())																		// �������¿���
	{
		if (IsOnMarking0())																// ��ŷ�߿�
		{
			if (m_pMotion->IsEnable(MS_X0) && m_pMotion->IsEnable(MS_Y0))				// ����� Enable�����̰�
			{
				if (m_pMotion->IsMotionDone(MS_X0) && m_pMotion->IsMotionDone(MS_Y0))	// ��� Done�����̸�,
				{
					if (!IsInitPos0() && !IsPinPos0())									// �ʱ���ġ�� �ƴϰų�, ����ġ�� �ƴҶ�
						MoveInitPos0();													// �ʱ���ġ�� �̵�
				}
			}

			if (m_nStepMk[0] < 13 && m_nStepMk[0] > 8) // Mk0();
			{
				m_nStepMk[0] = 8;	// ��ŷ�Ϸ�Check
			}
		}
		return;
	}

	m_sDispSts[0].Format(_T("%d"), m_nStepMk[0]);

	switch (m_nStepMk[0])
	{
	case 0:
		if (IsNoMk())
			ShowLive();
		m_nStepMk[0]++;
		break;
	case 1:
		if (!IsInitPos0())
			MoveInitPos0();
		m_nStepMk[0]++;
		break;
	case 2:

		nSerial = m_nBufUpSerial[0]; // Cam0

		//nSerial = 1;

		if (nSerial > 0)
		{
			if ((nErrCode = GetErrCode0(nSerial)) != 1)
			{
				m_nMkPcs[0] = GetTotDefPcs0(nSerial);
				m_nStepMk[0] = MK_END;
			}
			else
			{
				m_nStepMk[0]++;
			}
		}
		else
		{
			StopFromThread();
			//AsyncMsgBox(_T("������ �ø����� �����ʽ��ϴ�."), 1);
			MsgBox(_T("������ �ø����� �����ʽ��ϴ�."), 1);
			BuzzerFromThread(TRUE, 0);
			//pView->DispStsBar(_T("����-31"), 0);
			DispMain(_T("�� ��"), RGB_RED);
		}
		break;
	case 3:
		m_nStepMk[0]++;
		break;
	case 4:
		m_nStepMk[0]++;
		break;
	case 5:
		m_nStepMk[0]++;
		break;
	case 6:

		nSerial = m_nBufUpSerial[0]; // Cam0

		if (m_nMkPcs[0] < GetTotDefPcs0(nSerial))
		{
			if (!IsNoMk0())
			{
				;
			}
			else
			{
				if (!IsReview0())
				{
					if (m_bReview)
					{
						m_nMkPcs[0] = GetTotDefPcs0(nSerial);
						m_nStepMk[0] = MK_END;
						break;
					}
				}
			}
			// Punching On�̰ų� Review�̸� �������� ����
			SetDelay0(100, 1);		// [mSec]
			m_nStepMk[0]++;
		}
		else
		{
			m_nStepMk[0] = MK_END;
		}
		break;
	case 7:
		nSerial = m_nBufUpSerial[0]; // Cam0

		if (!WaitDelay0(1))		// F:Done, T:On Waiting....		// Delay�Ŀ�
		{
			m_nMkPcs[0] = 0;

			if (!IsNoMk0())										// Punching On�̸�
			{
				m_nStepMk[0]++;
			}
			else												// Punching�� Off�̰�
			{
				if (IsReview0())								// Review�̸� ��������
				{
					m_nStepMk[0]++;
				}
				else											// Review�� �ƴϸ�
				{
					if (m_bReview)
					{
						m_nMkPcs[0] = GetTotDefPcs0(nSerial);
						m_nStepMk[0] = MK_END;
					}
					else
						m_nStepMk[0]++;
				}
			}
		}
		break;
	case 8:
		nSerial = m_nBufUpSerial[0]; // Cam0

		if (m_nMkPcs[0] < GetTotDefPcs0(nSerial))	// ��ŷ�Ϸ�Check
		{
			if (m_nMkPcs[0] + 1 < GetTotDefPcs0(nSerial))		// ���� ��ŷ��ġ�� ������
			{
				ptPnt = GetMkPnt0(nSerial, m_nMkPcs[0] + 1);	// ���� ��ŷ��ġ
				m_dNextTarget[AXIS_X0] = ptPnt.x;
				m_dNextTarget[AXIS_Y0] = ptPnt.y;
			}
			else												// ���� ��ŷ��ġ�� ������
			{
				m_dNextTarget[AXIS_X0] = -1.0;
				m_dNextTarget[AXIS_Y0] = -1.0;
			}

			ptPnt = GetMkPnt0(nSerial, m_nMkPcs[0]);			// �̹� ��ŷ��ġ
			if (ptPnt.x < 0.0 && ptPnt.y < 0.0) // ��ǰȭ. (��ŷ���� ����)
			{
				m_nMkPcs[0]++;
				m_nStepMk[0] = MK_DONE_CHECK;
				break;
			}

			nIdx = GetMkStripIdx0(nSerial, m_nMkPcs[0]);		// 1 ~ 4 : strip index
			if (nIdx > 0)										// Strip index�� �����̸�,
			{
				if (!IsMkStrip(nIdx)) // Strip[] Mk Off
				{
					m_nMkPcs[0]++;
					m_nStepMk[0] = MK_DONE_CHECK;
					break;
				}
				else
				{
					if (m_nMkStrip[0][nIdx - 1] >= nStripOut)
					{
						m_nMkPcs[0]++;
						m_nStepMk[0] = MK_DONE_CHECK;
						break;
					}
					else
						m_nMkStrip[0][nIdx - 1]++;
				}
			}
			else
			{
				pView->ClrDispMsg();
				AfxMessageBox(_T("Strip Index Failed."));
				break;
			}

			m_dTarget[AXIS_X0] = ptPnt.x;
			m_dTarget[AXIS_Y0] = ptPnt.y;

			m_nStepMk[0]++;
		}
		else
		{
			m_nStepMk[0] = MK_END;
		}
		break;
	case 9:
		// Cam0 : m_bPriority[0] <-, m_bPriority[3] ->
		// Cam1 : m_bPriority[1] ->, m_bPriority[2] <-
		ptPnt.x = m_dTarget[AXIS_X0];
		ptPnt.y = m_dTarget[AXIS_Y0];

		if (m_dNextTarget[AXIS_X0] < 0)
			m_bCollision[0] = ChkCollision(AXIS_X0, m_dTarget[AXIS_X0]);
		else
			m_bCollision[0] = ChkCollision(AXIS_X0, m_dTarget[AXIS_X0], m_dNextTarget[AXIS_X0]);

		if (!m_bCollision[0])
		{
			if (IsMoveDone0())
			{
				Move0(ptPnt, m_bCam);
				m_nStepMk[0]++;
			}
		}
		else if (m_bPriority[0])
		{
			if (IsMoveDone0())
			{
				m_bCollision[0] = FALSE;
				m_bPriority[0] = FALSE;
				Move0(ptPnt, m_bCam);
				m_nStepMk[0]++;
			}
		}
		else if (m_bPriority[2])
		{
			if (IsMoveDone0())
			{
				m_bCollision[0] = FALSE;
				m_bPriority[2] = FALSE;
				ptPnt.x = 0.0;
				ptPnt.y = 0.0;//m_dEnc[AXIS_Y0];
				Move0(ptPnt, FALSE);
			}
		}
		break;
	case 10:
		m_nStepMk[0]++;
		break;
	case 11:
		m_nStepMk[0]++;
		break;
	case 12:
		if (IsMoveDone0())
			m_nStepMk[0]++;
		break;
	case 13:
		if (!IsNoMk0())
		{
			m_dwStMkDn[0] = GetTickCount();
			Mk0();
		}
		else
		{
			// Verify - Mk0
			SetDelay0(pDoc->m_nDelayShow, 1);		// [mSec]
			if(!SaveMk0Img(m_nMkPcs[0]))
			{
				pView->ClrDispMsg();
				AfxMessageBox(_T("Error-SaveMk0Img()"));
			}
			//m_nDebugStep = m_nMkPcs[0]; DispThreadTick();
		}
		m_nStepMk[0]++;
		break;
	case 14:
		m_nStepMk[0]++;
		break;
	case 15:
		m_nStepMk[0]++;
		break;
	case 16:
		if (IsNoMk0())
		{
			if (!WaitDelay0(1))		// F:Done, T:On Waiting....
				m_nStepMk[0]++;
		}
		else
			m_nStepMk[0]++;
		break;
	case 17:
		if (!IsNoMk0())
		{
			if (IsMk0Done())
			{
				// One more MK On Start....
				if (!m_nMkPcs[0] /*&& pDoc->WorkingInfo.Probing[0].bUse*/ && !m_bAnswer[2])
				{
					m_bAnswer[2] = TRUE;
					Mk0();
				}
				else
				{
					m_bAnswer[2] = FALSE;
					m_nMkPcs[0]++;
					m_nStepMk[0]++;
					m_nStepMk[0]++;
				}
			}
			else
			{
				if (m_dwStMkDn[0] + 5000 < GetTickCount())
				{
					BuzzerFromThread(TRUE, 0);
					//pView->DispStsBar(_T("����-32"), 0);
					DispMain(_T("�� ��"), RGB_RED);
					m_pVoiceCoil[0]->SearchHomeSmac0();

					//nRtn = AsyncMsgBox(_T("���̽�����(��) ��ſϷᰡ �ʵ˴ϴ�.\r\n��ŷ�� �ٽ� �õ��Ͻðڽ��ϱ�?"), 1, MB_YESNO);
					nRtn = MsgBox(_T("���̽�����(��) ��ſϷᰡ �ʵ˴ϴ�.\r\n��ŷ�� �ٽ� �õ��Ͻðڽ��ϱ�?"), 1, MB_YESNO);
					if (IDYES == nRtn)
					{
						DispMain(_T("������"), RGB_RED);
						m_nStepMk[0] = 13;
					}
					else if (nRtn < 0)
						m_nStepMk[0]++; // Wait...
					else
					{
						m_bAnswer[2] = FALSE;
						m_nMkPcs[0]++;
						m_nStepMk[0]++;
						m_nStepMk[0]++;
						Stop();
					}
				}
			}
		}
		else
		{
			m_nMkPcs[0]++;
			m_nStepMk[0]++;
			m_nStepMk[0]++;
		}
		break;
	case 18:
		if ((nRtn = WaitRtnVal(1)) > -1)
		{
			if (IDYES == nRtn)
			{
				DispMain(_T("������"), RGB_RED);
				m_nStepMk[0] = 13;
			}
			else
			{
				m_bAnswer[2] = FALSE;
				m_nMkPcs[0]++;
				m_nStepMk[0]++;
				StopFromThread();
			}
		}
		break;
	case 19:
		m_nStepMk[0] = MK_DONE_CHECK;
		break;
	case MK_DONE_CHECK:
		nSerial = m_nBufUpSerial[0]; // Cam0

		if (m_nMkPcs[0] < GetTotDefPcs0(nSerial))
		{
			if (IsNoMk0())
			{
				if (!pDoc->WorkingInfo.System.bNoMk)
				{
					if (IsReview0())
					{
						if (IsJogRtDn0())
							m_nStepMk[0]++;
					}
					else if (!pDoc->WorkingInfo.LastJob.bVerify)
					{
						m_nMkPcs[0] = GetTotDefPcs0(nSerial);
						m_nStepMk[0] = MK_END;
					}
					else
						m_nStepMk[0] = 8;	// ��ŷ�Ϸ�Check
				}
				else
					m_nStepMk[0] = 8;	// ��ŷ�Ϸ�Check
			}
			else
				m_nStepMk[0] = 8;	// ��ŷ�Ϸ�Check
		}
		else
		{
			if (IsNoMk0())
			{
				if (IsReview0())
				{
					if (IsJogRtDn0())
						m_nStepMk[0] = MK_END;
				}
				else
				{
					m_nStepMk[0] = MK_END;
				}
			}
			else
				m_nStepMk[0] = MK_END;
		}
		break;
	case MK_DONE_CHECK + 1:
		nSerial = m_nBufUpSerial[0]; // Cam0

		if (m_nMkPcs[0] < GetTotDefPcs0(nSerial))
		{
			if (IsNoMk0())
			{
				if (IsReview0())
				{
					if (IsJogRtUp0())
						m_nStepMk[0] = 8;	// ��ŷ�Ϸ�Check
				}
				else
				{
					m_nMkPcs[0] = GetTotDefPcs0(nSerial);
					m_nStepMk[0] = MK_END;
				}
			}
			else
				m_nStepMk[0] = 8;	// ��ŷ�Ϸ�Check
		}
		else
		{
			m_nStepMk[0] = MK_END;
		}
		break;

	case MK_END:
		if (IsMoveDone0())
			m_nStepMk[0]++;
		break;
	case 101:
		SetDelay0(100, 1);		// [mSec]
		m_nStepMk[0]++;
		break;
	case 102:
		if (!WaitDelay0(1))		// F:Done, T:On Waiting....
		{
			m_nStepMk[0]++;
		}
		break;
	case 103:
		if (!IsInitPos0())
		{
			m_dTarget[AXIS_X0] = _tstof(pDoc->WorkingInfo.Motion.sStPosX[0]);
			m_dTarget[AXIS_Y0] = _tstof(pDoc->WorkingInfo.Motion.sStPosY[0]);
			m_dNextTarget[AXIS_X0] = _tstof(pDoc->WorkingInfo.Motion.sStPosX[0]);
			m_dNextTarget[AXIS_Y0] = _tstof(pDoc->WorkingInfo.Motion.sStPosY[0]);

			MoveInitPos0();
		}

		pDoc->SaveMkCntL();
		m_nStepMk[0]++;
		break;
	case 104:
		if (IsMoveDone0())
		{
			m_nStepMk[0]++;
			SetDelay0(10000, 1);		// [mSec]
		}
		break;
	case 105:
		if (IsInitPos0())
		{
			m_nStepMk[0]++;
		}
		else
		{
			if (!WaitDelay0(1))		// F:Done, T:On Waiting....
			{
				m_nStepMk[0] = ERR_PROC;
			}
		}
		break;
	case 106: // MK Done....
		m_bDoneMk[0] = TRUE;
		m_bTHREAD_MK[0] = FALSE;
		break;

	case ERR_PROC:
		DispMain(_T("�� ��"), RGB_RED);
		m_pVoiceCoil[0]->SearchHomeSmac0();
		//AsyncMsgBox(_T("���̽�����(��) �ʱ���ġ �̵��� ���� �ʽ��ϴ�.\r\n��ŷ���¸� Ȯ���ϼ���."), 1);
		MsgBox(_T("���̽�����(��) �ʱ���ġ �̵��� ���� �ʽ��ϴ�.\r\n��ŷ���¸� Ȯ���ϼ���."), 1);
		m_nStepMk[0]++;
		break;
	case ERR_PROC + 1:
		nSerial = m_nBufUpSerial[0]; // Cam0

		if (m_bCam)
			sMsg.Format(_T("%d�� Shot�� �ٽ� �ҷ�Ȯ���� �Ͻðڽ��ϱ�?"), nSerial);
		else
			sMsg.Format(_T("%d�� Shot�� �ٽ� ��ŷ�Ͻðڽ��ϱ�?"), nSerial);

		m_nRtnMyMsgBoxIdx = 0;
		m_bRtnMyMsgBox[0] = FALSE;
		m_nRtnMyMsgBox[0] = -1;
		//pView->AsyncMsgBox(sMsg, 1, MB_YESNO);
		pView->MsgBox(sMsg, 1, MB_YESNO);
		sMsg.Empty();
		m_nStepMk[0]++;
		break;
	case ERR_PROC + 2:
		if ((nRtn = WaitRtnVal(1)) > -1)
		{
			if (IDYES == nRtn)
			{
				m_nStepMk[0] = ERR_PROC + 10;
			}
			else
			{
				m_nRtnMyMsgBoxIdx = 0;
				m_bRtnMyMsgBox[0] = FALSE;
				m_nRtnMyMsgBox[0] = -1;
				sMsg.Format(_T("��� ���� �۾��� �����Ͻðڽ��ϱ�?"), nSerial);
				//pView->AsyncMsgBox(sMsg, 1, MB_YESNO);
				pView->MsgBox(sMsg, 1, MB_YESNO);
				sMsg.Empty();

				m_nStepMk[0]++;
			}
		}
		break;
	case ERR_PROC + 3:
		if ((nRtn = WaitRtnVal(1)) > -1)
		{
			if (IDYES == nRtn)
			{
				m_nStepMk[0] = ERR_PROC + 20;
			}
			else
			{
				m_bDispMsgDoAuto[8] = TRUE;
				m_nStepDispMsg[8] = FROM_DOMARK0;
			}
		}
		break;
	case ERR_PROC + 10:
		m_bReMark[0] = TRUE;
		m_bTHREAD_MK[0] = FALSE;
		m_nStepMk[0] = 0;
		break;
	case ERR_PROC + 20: // MK Done....
		m_bDoneMk[0] = TRUE;
		m_bTHREAD_MK[0] = FALSE;
		break;
	}
}

BOOL CGvisR2R_PunchView::SaveMk0Img(int nMkPcsIdx) // Cam0
{
	if (!pDoc->WorkingInfo.System.bSaveMkImg)
		return TRUE;

	int nSerial;
	nSerial = m_nBufUpSerial[0]; // Cam0

	CString sSrc, sDest, sPath;
	stModelInfo stInfo;

	sSrc.Format(_T("%s%04d.pcr"), pDoc->WorkingInfo.System.sPathVrsBufUp, nSerial);
	if (!pDoc->GetPcrInfo(sSrc, stInfo))
	{
		pView->DispStsBar(_T("E(2)"), 5);
		pView->ClrDispMsg();
		AfxMessageBox(_T("Error-GetPcrInfo(2)"));
		return FALSE;
	}

	if (!pDoc->MakeMkDir(stInfo))
	{
		pView->ClrDispMsg();
		AfxMessageBox(_T("Error-MakeMkDir()"));
		return FALSE;
	}

	//sDest.Format(_T("%s%s\\%s\\%s\\Punching\\%04d"), pDoc->WorkingInfo.System.sPathOldFile, stInfo.sModel,
	//	stInfo.sLot, stInfo.sLayer, nSerial);

	//if (!pDoc->DirectoryExists(sDest))
	//	CreateDirectory(sDest, NULL);
	//sPath.Format(_T("%s\\%d.tif"), sDest, ++m_nSaveMk0Img);

	sDest.Format(_T("%s%s\\%s\\%s\\Punching"), pDoc->WorkingInfo.System.sPathOldFile, stInfo.sModel,
		stInfo.sLot, stInfo.sLayer);

	if (!pDoc->DirectoryExists(sDest))
		CreateDirectory(sDest, NULL);

	if (GetTotDefPcs0(nSerial) > 0)
	{
		sPath.Format(_T("%s\\%s.tif"), sDest, GetMkInfo0(nSerial, nMkPcsIdx));

#ifdef USE_VISION
		if (m_pVision[0])
			return m_pVision[0]->SaveMkImg(sPath);
#endif
	}
	else
		return TRUE;

	return FALSE;
}

void CGvisR2R_PunchView::DoMark1()
{
#ifdef TEST_MODE
	return;
#endif

	if (!m_bAuto)
		return;

	//BOOL bOn;
	int nSerial, nIdx, nErrCode, nRtn;
	CfPoint ptPnt;
	CString sMsg;
	double dStripOut = (pDoc->m_Master[0].m_pPcsRgn->nTotPcs / MAX_STRIP_NUM) * _tstof(pDoc->WorkingInfo.LastJob.sStripOutRatio) / 100.0;
	int nStripOut = int(dStripOut);
	if (dStripOut > nStripOut)
		nStripOut++;

	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;

	if (!IsRun())
	{
		if (IsOnMarking1())
		{
			if (m_pMotion->IsEnable(MS_X1) && m_pMotion->IsEnable(MS_Y1))
			{
				if (m_pMotion->IsMotionDone(MS_X1) && m_pMotion->IsMotionDone(MS_Y1))
				{
					if (!IsMkEdPos1() && !IsPinPos1())
						MoveMkEdPos1();
				}
			}

			if (m_nStepMk[1] < 13 && m_nStepMk[1] > 8) // Mk1();
			{
				m_nStepMk[1] = 8;	// ��ŷ�Ϸ�Check
			}
		}
		return;
	}


	m_sDispSts[1].Format(_T("%d"), m_nStepMk[1]);

	switch (m_nStepMk[1])
	{
	case 0:
		if (IsNoMk())
			ShowLive();
		m_nStepMk[1]++;
		break;
	case 1:
		if (!IsInitPos1())
			MoveInitPos1();
		m_nStepMk[1]++;
		break;
	case 2:

		nSerial = m_nBufUpSerial[1]; // Cam1

		if (nSerial > 0)
		{
			if ((nErrCode = GetErrCode1(nSerial)) != 1)
			{
				m_nMkPcs[1] = GetTotDefPcs1(nSerial);
				m_nStepMk[1] = MK_END;
			}
			else
			{
				m_nStepMk[1]++;
			}
		}
		else
		{
			StopFromThread();
			//AsyncMsgBox(_T("������ �ø����� �����ʽ��ϴ�."), 2);
			MsgBox(_T("������ �ø����� �����ʽ��ϴ�."), 2);
			BuzzerFromThread(TRUE, 0);
			DispMain(_T("�� ��"), RGB_RED);
		}
		break;
	case 3:
		m_nStepMk[1]++;
		break;
	case 4:
		m_nStepMk[1]++;
		break;
	case 5:
		m_nStepMk[1]++;
		break;
	case 6:
		nSerial = m_nBufUpSerial[1]; // Cam1

		if (m_nMkPcs[1] < GetTotDefPcs1(nSerial))
		{
			if (!IsNoMk1())
			{
				;
			}
			else
			{
				if (!IsReview1())
				{
					if (m_bReview)
					{
						m_nMkPcs[1] = GetTotDefPcs1(nSerial);
						m_nStepMk[1] = MK_END;
						break;
					}
				}
			}
			SetDelay1(100, 6);		// [mSec]
			m_nStepMk[1]++;
		}
		else
		{
			m_nStepMk[1] = MK_END;
		}
		break;
	case 7:
		nSerial = m_nBufUpSerial[1]; // Cam1

		if (!WaitDelay1(6))		// F:Done, T:On Waiting....
		{
			m_nMkPcs[1] = 0;

			if (!IsNoMk1())
			{
				m_nStepMk[1]++;
			}
			else
			{
				if (IsReview1())
				{
					m_nStepMk[1]++;
				}
				else
				{
					if (m_bReview)
					{
						m_nMkPcs[1] = GetTotDefPcs1(nSerial);
						m_nStepMk[1] = MK_END;
					}
					else
						m_nStepMk[1]++;
				}
			}
		}
		break;
	case 8:
		nSerial = m_nBufUpSerial[1]; // Cam1

		if (m_nMkPcs[1] < GetTotDefPcs1(nSerial))	// ��ŷ�Ϸ�Check
		{
			if (m_nMkPcs[1] + 1 < GetTotDefPcs1(nSerial))
			{
				ptPnt = GetMkPnt1(nSerial, m_nMkPcs[1] + 1);
				m_dNextTarget[AXIS_X1] = ptPnt.x;
				m_dNextTarget[AXIS_Y1] = ptPnt.y;
			}
			else
			{
				m_dNextTarget[AXIS_X1] = -1.0;
				m_dNextTarget[AXIS_Y1] = -1.0;
			}

			ptPnt = GetMkPnt1(nSerial, m_nMkPcs[1]);
			if (ptPnt.x < 0.0 && ptPnt.y < 0.0) // ��ǰȭ.
			{
				m_nMkPcs[1]++;
				m_nStepMk[1] = MK_DONE_CHECK;
				break;
			}

			nIdx = GetMkStripIdx1(nSerial, m_nMkPcs[1]);
			if (nIdx > 0)
			{
				if (!IsMkStrip(nIdx)) // Strip[] Mk Off
				{
					m_nMkPcs[1]++;
					m_nStepMk[1] = MK_DONE_CHECK;
					break;
				}
				else
				{
					if (m_nMkStrip[1][nIdx - 1] >= nStripOut)
					{
						m_nMkPcs[1]++;
						m_nStepMk[1] = MK_DONE_CHECK;
						break;
					}
					else
						m_nMkStrip[1][nIdx - 1]++;
				}
			}
			else
			{
				pView->ClrDispMsg();
				AfxMessageBox(_T("Strip Index Failed."));
				break;
			}

			m_dTarget[AXIS_X1] = ptPnt.x;
			m_dTarget[AXIS_Y1] = ptPnt.y;

			m_nStepMk[1]++;
		}
		else
		{
			m_nStepMk[1] = MK_END;
		}
		break;
	case 9:
		// Cam0 : m_bPriority[0] <-, m_bPriority[3] ->
		// Cam1 : m_bPriority[1] ->, m_bPriority[2] <-
		ptPnt.x = m_dTarget[AXIS_X1];
		ptPnt.y = m_dTarget[AXIS_Y1];

		if (m_dNextTarget[AXIS_X1] < 0)
			m_bCollision[1] = ChkCollision(AXIS_X1, m_dTarget[AXIS_X1]);
		else
			m_bCollision[1] = ChkCollision(AXIS_X1, m_dTarget[AXIS_X1], m_dNextTarget[AXIS_X1]);

		if (!m_bCollision[1])
		{
			if (IsMoveDone1())
			{
				Move1(ptPnt, m_bCam);
				m_nStepMk[1]++;
			}
		}
		else if (m_bPriority[1])
		{
			if (IsMoveDone1())
			{
				m_bCollision[1] = FALSE;
				m_bPriority[1] = FALSE;
				Move1(ptPnt, m_bCam);
				m_nStepMk[1]++;
			}
		}
		else if (m_bPriority[3])
		{
			if (IsMoveDone1())
			{
				m_bCollision[1] = FALSE;
				m_bPriority[3] = FALSE;
				ptPnt.x = _tstof(pDoc->WorkingInfo.Motion.sSafeZone);
				ptPnt.y = 0.0;//m_dEnc[AXIS_Y1];
				Move1(ptPnt, FALSE);
			}
		}
		break;
	case 10:
		m_nStepMk[1]++;
		break;
	case 11:
		m_nStepMk[1]++;
		break;
	case 12:
		if (IsMoveDone1())
			m_nStepMk[1]++;
		break;
	case 13:
		if (!IsNoMk1())
		{
			m_dwStMkDn[1] = GetTickCount();
			Mk1();
		}
		else
		{
			// Verify - Mk1
			SetDelay1(pDoc->m_nDelayShow, 6);		// [mSec]
			if(!SaveMk1Img(m_nMkPcs[1]))
			{
				pView->ClrDispMsg();
				AfxMessageBox(_T("Error-SaveMk1Img()"));
			}
		}
		m_nStepMk[1]++;
		break;
	case 14:
		m_nStepMk[1]++;
		break;
	case 15:
		m_nStepMk[1]++;
		break;
	case 16:
		if (IsNoMk1())
		{
			if (!WaitDelay1(6))		// F:Done, T:On Waiting....
				m_nStepMk[1]++;
		}
		else
			m_nStepMk[1]++;
		break;
	case 17:
		if (!IsNoMk1())
		{
			if (IsMk1Done())
			{
				// One more MK On Start....
				if (!m_nMkPcs[1] && !m_bAnswer[3])
				{
					m_bAnswer[3] = TRUE;
					Mk1();
				}
				else
				{
					m_bAnswer[3] = FALSE;
					m_nMkPcs[1]++;
					m_nStepMk[1]++;
					m_nStepMk[1]++;
				}

			}
			else
			{
				if (m_dwStMkDn[1] + 5000 < GetTickCount())
				{
					BuzzerFromThread(TRUE, 0);
					DispMain(_T("�� ��"), RGB_RED);
					m_pVoiceCoil[1]->SearchHomeSmac1();

					//nRtn = AsyncMsgBox(_T("���̽�����(��) ��ſϷᰡ �ʵ˴ϴ�.\r\n��ŷ�� �ٽ� �õ��Ͻðڽ��ϱ�?"), 2, MB_YESNO);
					nRtn = MsgBox(_T("���̽�����(��) ��ſϷᰡ �ʵ˴ϴ�.\r\n��ŷ�� �ٽ� �õ��Ͻðڽ��ϱ�?"), 2, MB_YESNO);
					if (IDYES == nRtn)
					{
						DispMain(_T("������"), RGB_RED);
						m_nStepMk[1] = 13;
					}
					else if (nRtn < 0)
						m_nStepMk[1]++; // Wait...
					else
					{
						m_bAnswer[3] = FALSE;
						m_nMkPcs[1]++;
						m_nStepMk[1]++;
						m_nStepMk[1]++;
						Stop();
					}
				}
			}
		}
		else
		{
			m_nMkPcs[1]++;
			m_nStepMk[1]++;
			m_nStepMk[1]++;
		}
		break;
	case 18:
		if ((nRtn = WaitRtnVal(2)) > -1)
		{
			if (IDYES == nRtn)
			{
				DispMain(_T("������"), RGB_RED);
				m_nStepMk[1] = 13;
			}
			else
			{
				m_bAnswer[3] = FALSE;
				m_nMkPcs[1]++;
				m_nStepMk[1]++;
				StopFromThread();
			}
		}
		break;
	case 19:
		m_nStepMk[1] = MK_DONE_CHECK;
		break;
	case MK_DONE_CHECK:
		nSerial = m_nBufUpSerial[1]; // Cam1

		if (m_nMkPcs[1] < GetTotDefPcs1(nSerial))
		{
			if (IsNoMk1())
			{
				if (!pDoc->WorkingInfo.System.bNoMk)
				{
					if (IsReview1())
					{
						if (IsJogRtDn1())
							m_nStepMk[1]++;
					}
					else if (!pDoc->WorkingInfo.LastJob.bVerify)
					{
						m_nMkPcs[1] = GetTotDefPcs1(nSerial);
						m_nStepMk[1] = MK_END;
					}
					else
						m_nStepMk[1] = 8;	// ��ŷ�Ϸ�Check
				}
				else
					m_nStepMk[1] = 8;	// ��ŷ�Ϸ�Check
			}
			else
				m_nStepMk[1] = 8;	// ��ŷ�Ϸ�Check
		}
		else
		{
			if (IsNoMk1())
			{
				if (IsReview1())
				{
					if (IsJogRtDn1())
						m_nStepMk[1] = MK_END;
				}
				else
				{
					m_nStepMk[1] = MK_END;
				}
			}
			else
				m_nStepMk[1] = MK_END;
		}
		break;
	case MK_DONE_CHECK + 1:
		nSerial = m_nBufUpSerial[1]; // Cam1

		if (m_nMkPcs[1] < GetTotDefPcs1(nSerial))
		{
			if (IsNoMk1())
			{
				if (IsReview1())
				{
					if (IsJogRtUp1())
						m_nStepMk[1] = 8;	// ��ŷ�Ϸ�Check
				}
				else
				{
					m_nMkPcs[1] = GetTotDefPcs1(nSerial);
					m_nStepMk[1] = MK_END;
				}
			}
			else
				m_nStepMk[1] = 8;	// ��ŷ�Ϸ�Check
		}
		else
		{
			m_nStepMk[1] = MK_END;
		}
		break;


	case MK_END:
		if (IsMoveDone1())
			m_nStepMk[1]++;
		break;
	case 101:
		SetDelay1(100, 6);		// [mSec]
		m_nStepMk[1]++;
		break;
	case 102:
		if (!WaitDelay1(6))		// F:Done, T:On Waiting....
			m_nStepMk[1]++;
		break;
	case 103:
		if (!IsMkEdPos1())
		{
			m_dTarget[AXIS_X1] = _tstof(pDoc->WorkingInfo.Motion.sStPosX[1]);
			m_dTarget[AXIS_Y1] = _tstof(pDoc->WorkingInfo.Motion.sStPosY[1]);
			m_dNextTarget[AXIS_X1] = _tstof(pDoc->WorkingInfo.Motion.sStPosX[1]);
			m_dNextTarget[AXIS_Y1] = _tstof(pDoc->WorkingInfo.Motion.sStPosY[1]);

			MoveMkEdPos1();
		}

		pDoc->SaveMkCntR();
		m_nStepMk[1]++;
		break;
	case 104:
		if (IsMoveDone1())
		{
			m_nStepMk[1]++;
			SetDelay1(10000, 6);		// [mSec]
		}
		break;
	case 105:
		if (IsMkEdPos1())
		{
			m_nStepMk[1]++;
		}
		else
		{
			if (!WaitDelay1(6))		// F:Done, T:On Waiting....
			{
				m_nStepMk[1] = ERR_PROC;
			}
		}
		break;
	case 106: // MK Done....
		m_bDoneMk[1] = TRUE;
		m_bTHREAD_MK[1] = FALSE;
		break;

	case ERR_PROC:
		DispMain(_T("�� ��"), RGB_RED);
		m_pVoiceCoil[1]->SearchHomeSmac1();
		//AsyncMsgBox(_T("���̽�����(��) �ʱ���ġ �̵��� ���� �ʽ��ϴ�.\r\n��ŷ���¸� Ȯ���ϼ���."), 2);
		MsgBox(_T("���̽�����(��) �ʱ���ġ �̵��� ���� �ʽ��ϴ�.\r\n��ŷ���¸� Ȯ���ϼ���."), 2);
		m_nStepMk[1]++;
		break;
	case ERR_PROC + 1:
		nSerial = m_nBufUpSerial[1]; // Cam1

		if (m_bCam)
			sMsg.Format(_T("%d�� Shot�� �ٽ� �ҷ�Ȯ���� �Ͻðڽ��ϱ�?"), nSerial);
		else
			sMsg.Format(_T("%d�� Shot�� �ٽ� ��ŷ�Ͻðڽ��ϱ�?"), nSerial);

		m_nRtnMyMsgBoxIdx = 1;
		m_bRtnMyMsgBox[1] = FALSE;
		m_nRtnMyMsgBox[1] = -1;
		//pView->AsyncMsgBox(sMsg, 2, MB_YESNO);
		pView->MsgBox(sMsg, 2, MB_YESNO);
		sMsg.Empty();
		m_nStepMk[1]++;
		break;
	case ERR_PROC + 2:
		if ((nRtn = WaitRtnVal(2)) > -1)
		{
			if (IDYES == nRtn)
			{
				m_nStepMk[1] = ERR_PROC + 10;
			}
			else
			{
				m_nRtnMyMsgBoxIdx = 1;
				m_bRtnMyMsgBox[1] = FALSE;
				m_nRtnMyMsgBox[1] = -1;
				sMsg.Format(_T("��� ���� �۾��� �����Ͻðڽ��ϱ�?"), nSerial);
				//pView->AsyncMsgBox(sMsg, 2, MB_YESNO);
				pView->MsgBox(sMsg, 2, MB_YESNO);
				sMsg.Empty();

				m_nStepMk[1]++;

			}
		}
		break;
	case ERR_PROC + 3:
		if ((nRtn = WaitRtnVal(2)) > -1)
		{
			if (IDYES == nRtn)
			{
				m_nStepMk[1] = ERR_PROC + 20;
			}
			else
			{
				m_bDispMsgDoAuto[9] = TRUE;
				m_nStepDispMsg[9] = FROM_DOMARK1;
			}
		}
		break;
	case ERR_PROC + 10:
		m_bReMark[1] = TRUE;
		m_bTHREAD_MK[1] = FALSE;
		m_nStepMk[1] = 0;
		break;
	case ERR_PROC + 20: // MK Done....
		m_bDoneMk[1] = TRUE;
		m_bTHREAD_MK[1] = FALSE;
		break;
	}
}

BOOL CGvisR2R_PunchView::SaveMk1Img(int nMkPcsIdx) // Cam1
{
	if (!pDoc->WorkingInfo.System.bSaveMkImg)
		return TRUE;

	int nSerial;
	nSerial = m_nBufUpSerial[1]; // Cam1

	CString sSrc, sDest, sPath;
	stModelInfo stInfo;

	sSrc.Format(_T("%s%04d.pcr"), pDoc->WorkingInfo.System.sPathVrsBufUp, nSerial);
	if (!pDoc->GetPcrInfo(sSrc, stInfo))
	{
		pView->DispStsBar(_T("E(2)"), 5);
		pView->ClrDispMsg();
		AfxMessageBox(_T("Error-GetPcrInfo(2)"));
		return FALSE;
	}

	if (!pDoc->MakeMkDir(stInfo))
	{
		pView->ClrDispMsg();
		AfxMessageBox(_T("Error-MakeMkDir()"));
		return FALSE;
	}

	//sDest.Format(_T("%s%s\\%s\\%s\\Punching\\%04d"), pDoc->WorkingInfo.System.sPathOldFile, stInfo.sModel,
	//	stInfo.sLot, stInfo.sLayer, nSerial);

	//if (!pDoc->DirectoryExists(sDest))
	//	CreateDirectory(sDest, NULL);

	//sPath.Format(_T("%s\\%d.tif"), sDest, ++m_nSaveMk1Img);

	sDest.Format(_T("%s%s\\%s\\%s\\Punching"), pDoc->WorkingInfo.System.sPathOldFile, stInfo.sModel,
		stInfo.sLot, stInfo.sLayer);

	if (!pDoc->DirectoryExists(sDest))
		CreateDirectory(sDest, NULL);

	if (GetTotDefPcs1(nSerial) > 0) // Cam1
	{
		sPath.Format(_T("%s\\%s.tif"), sDest, GetMkInfo1(nSerial, nMkPcsIdx));

#ifdef USE_VISION
		if (m_pVision[1])
			return m_pVision[1]->SaveMkImg(sPath);
#endif
	}
	else
		return TRUE;

	return FALSE;
}

void CGvisR2R_PunchView::DispDefImg()
{
	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;

	int nSerialL = m_nBufUpSerial[0]; // ���� Camera
	int nSerialR = m_nBufUpSerial[1]; // ���� Camera
	CString sNewLot = m_sNewLotUp;;
	CString str;
	BOOL bNewModel = FALSE;
	int nNewLot = 0;
	int nBreak = 0;
	int nLastShot = 0;

	switch (m_nStepTHREAD_DISP_DEF)
	{
	// CopyDefImg Start ============================================
	case 0:
		m_nStepTHREAD_DISP_DEF++;

		if (nSerialL == pView->m_nLotEndSerial)
			nBreak = 1;

		if (nSerialL > 0)
		{
			if (!CopyDefImg(nSerialL, sNewLot)) // ���� Camera
			{
				sNewLot.Empty();
				m_bDispMsgDoAuto[7] = TRUE;
				m_nStepDispMsg[7] = FROM_DISPDEFIMG + 7;
				break;
			}

			if (pView->m_bSerialDecrese)
			{
				if (m_bLastProc && nSerialR < m_nLotEndSerial)
				{
					nLastShot = nSerialL;
				}
				else
				{
					if (nSerialR > 0)
					{
						if (!CopyDefImg(nSerialR, sNewLot)) // ���� Camera
						{
							sNewLot.Empty();
							m_bDispMsgDoAuto[6] = TRUE;
							m_nStepDispMsg[6] = FROM_DISPDEFIMG + 6;
							break;
						}
					}
				}
			}
			else
			{
				if (m_bLastProc && nSerialR > m_nLotEndSerial)
				{
					nLastShot = nSerialL;
				}
				else
				{
					if (nSerialR > 0)
					{
						if (!CopyDefImg(nSerialR, sNewLot)) // ���� Camera
						{
							sNewLot.Empty();
							m_bDispMsgDoAuto[6] = TRUE;
							m_nStepDispMsg[6] = FROM_DISPDEFIMG + 6;
							break;
						}
					}
				}
			}

		}
		else
		{
			if (!m_bLastProc)
			{
				m_bDispMsgDoAuto[5] = TRUE;
				m_nStepDispMsg[5] = FROM_DISPDEFIMG + 5;
			}
		}
		sNewLot.Empty();
		break;
	case 1:
		//Sleep(300);
		m_nStepTHREAD_DISP_DEF++;
		if (pDoc->m_pReelMap)
		{
			str = pDoc->m_pReelMap->GetIpPath();
			pDoc->SetMkMenu01(_T("DispDefImg"), _T("ReelmapPath"), str);
		}
		str.Format(_T("%d"), nSerialL);
		pDoc->SetMkMenu01(_T("DispDefImg"), _T("SerialL"), str);
		str.Format(_T("%d"), nSerialR);
		pDoc->SetMkMenu01(_T("DispDefImg"), _T("SerialR"), str);
		break;
	case 2:
		if (IsDoneDispMkInfo())	 // Check �ҷ��̹��� Display End
		{
			if (pView->m_bSerialDecrese)
			{
				if (ChkLastProc() && (nSerialL < m_nLotEndSerial))
				{
					nLastShot = nSerialL;
					SetSerialReelmap(nSerialL, TRUE);	// Reelmap(��) Display Start
					SetSerialMkInfo(nSerialL, TRUE);		// �ҷ��̹���(��) Display Start
				}
				else if (ChkLastProc() && nSerialL < 1)
				{
					SetSerialReelmap(m_nLotEndSerial + 1, TRUE);		// Reelmap(��) Display Start
					SetSerialMkInfo(m_nLotEndSerial + 1, TRUE);		// �ҷ��̹���(��) Display Start
				}
				else
				{
					SetFixPcs(nSerialL);
					SetSerialReelmap(nSerialL);	// Reelmap(��) Display Start
					SetSerialMkInfo(nSerialL);	// �ҷ��̹���(��) Display Start
				}
			}
			else
			{
				if (ChkLastProc() && (nSerialL > m_nLotEndSerial))
				{
					nLastShot = nSerialL;
					SetSerialReelmap(nSerialL, TRUE);	// Reelmap(��) Display Start
					SetSerialMkInfo(nSerialL, TRUE);		// �ҷ��̹���(��) Display Start
				}
				else if (ChkLastProc() && nSerialL < 1)
				{
					SetSerialReelmap(m_nLotEndSerial + 1, TRUE);		// Reelmap(��) Display Start
					SetSerialMkInfo(m_nLotEndSerial + 1, TRUE);		// �ҷ��̹���(��) Display Start
				}
				else
				{
					SetFixPcs(nSerialL);
					SetSerialReelmap(nSerialL);	// Reelmap(��) Display Start
					SetSerialMkInfo(nSerialL);	// �ҷ��̹���(��) Display Start
				}
			}


			if (IsFixPcsUp(nSerialL))
			{
				m_bDispMsgDoAuto[2] = TRUE;
				m_nStepDispMsg[2] = FROM_DISPDEFIMG + 2;
			}
			if (IsFixPcsDn(nSerialL))
			{
				m_bDispMsgDoAuto[3] = TRUE;
				m_nStepDispMsg[3] = FROM_DISPDEFIMG + 3;
			}

			m_nStepTHREAD_DISP_DEF++;
		}

		break;

	case 3:
		if (IsDoneDispMkInfo())	 // Check �ҷ��̹���(��) Display End
		{
			if (nSerialR > 0) // ���� Camera
			{
				if (pView->m_bSerialDecrese)
				{
					if (ChkLastProc() && (nSerialR < m_nLotEndSerial))
						SetSerialMkInfo(nSerialR, TRUE);	// �ҷ��̹���(��) Display Start
					else
						SetSerialMkInfo(nSerialR);		// �ҷ��̹���(��) Display Start
				}
				else
				{
					if (ChkLastProc() && (nSerialR > m_nLotEndSerial))
						SetSerialMkInfo(nSerialR, TRUE);	// �ҷ��̹���(��) Display Start
					else
						SetSerialMkInfo(nSerialR);		// �ҷ��̹���(��) Display Start
				}
			}
			else
			{
				if (ChkLastProc())
				{
					if (pView->m_bSerialDecrese)
						SetSerialMkInfo(m_nLotEndSerial - 1, TRUE);	// �ҷ��̹���(��) Display Start
					else
						SetSerialMkInfo(m_nLotEndSerial + 1, TRUE);	// �ҷ��̹���(��) Display Start
				}
			}
			m_nStepTHREAD_DISP_DEF++;
		}
		break;
	case 4:
		if (nSerialR > 0) // ���� Camera
		{
			m_nStepTHREAD_DISP_DEF++;

			if (pView->m_bSerialDecrese)
			{
				if (ChkLastProc() && (nSerialR < m_nLotEndSerial))
				{
					SetSerialReelmap(nSerialR, TRUE);	// Reelmap(��) Display Start
				}
				else
				{
					SetFixPcs(nSerialR);
					SetSerialReelmap(nSerialR);			// Reelmap(��) Display Start
				}
			}
			else
			{
				if (ChkLastProc() && (nSerialR > m_nLotEndSerial))
				{
					SetSerialReelmap(nSerialR, TRUE);	// Reelmap(��) Display Start
				}
				else
				{
					SetFixPcs(nSerialR);
					SetSerialReelmap(nSerialR);			// Reelmap(��) Display Start
				}
			}

			if (bDualTest)
			{
				if (IsFixPcsUp(nSerialR))
				{
					m_bDispMsgDoAuto[2] = TRUE;
					m_nStepDispMsg[2] = FROM_DISPDEFIMG + 2;
				}
				if (IsFixPcsDn(nSerialR))
				{
					m_bDispMsgDoAuto[3] = TRUE;
					m_nStepDispMsg[3] = FROM_DISPDEFIMG + 3;
				}
			}
			else
			{
				if (IsFixPcsUp(nSerialR))
				{
					m_bDispMsgDoAuto[2] = TRUE;
					m_nStepDispMsg[2] = FROM_DISPDEFIMG + 2;
				}
			}
		}
		else
		{
			if (ChkLastProc())
			{
				m_nStepTHREAD_DISP_DEF++;
				if (pView->m_bSerialDecrese)
					SetSerialReelmap(m_nLotEndSerial - 1, TRUE);	// �ҷ��̹���(��) Display Start
				else
					SetSerialReelmap(m_nLotEndSerial + 1, TRUE);	// �ҷ��̹���(��) Display Start
			}
			else
			{
				if (m_bLastProc && nSerialL == m_nLotEndSerial)
					m_nStepTHREAD_DISP_DEF++;
				else
				{
					m_nStepTHREAD_DISP_DEF++;
				}
			}
		}
		break;
	case 5:
		m_nStepTHREAD_DISP_DEF++;
		break;
	case 6:
		m_nStepTHREAD_DISP_DEF++;
		break;
	case 7:
		m_nStepTHREAD_DISP_DEF++;
		break;
	case 8:
		m_nStepTHREAD_DISP_DEF++;
		break;
	case 9:
		m_nStepTHREAD_DISP_DEF++;
		break;
	case 10:
		m_nStepTHREAD_DISP_DEF++;
		break;

	case 11:
		if (IsDoneDispMkInfo() && IsRun())	 // Check �ҷ��̹���(��) Display End
			m_nStepTHREAD_DISP_DEF++;
		break;
	case 12:
		m_bTHREAD_DISP_DEF = FALSE;
		pDoc->SetMkMenu01(_T("Signal"), _T("DispDefImg"), _T("0"));
		if (m_pEngrave)
			m_pEngrave->SwMenu01DispDefImg(TRUE);

		break;
		// CopyDefImg End ============================================
	}

}

void CGvisR2R_PunchView::DoAuto()
{
	if (!IsAuto())
		return;

	CString str;
	str.Format(_T("%d : %d"), m_nStepTHREAD_DISP_DEF, m_bTHREAD_DISP_DEF ? 1 : 0);
	pView->DispStsBar(str, 6);

	//BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;

	// LotEnd Start
	if (DoAutoGetLotEndSignal())
	{
		return;
	}

	// ��ŷ���� ��ȣ�� Ȯ��
	DoAtuoGetMkStSignal();

	// LastProc Start
	DoAutoSetLastProcAtPlc();

	// AOI Feeding Offset Start on LastProc
	DoAutoSetFdOffsetLastProc();

	// AOI Feeding Offset Start
	DoAutoSetFdOffset();

	// Engrave Feeding Offset Start
	DoAutoSetFdOffsetEngrave();

	// CycleStop
	DoAutoChkCycleStop();

	// DispMsg
	DoAutoDispMsg();

	// Check Share Folder Start
	DoAutoChkShareFolder();

	// Marking Start
	DoAutoMarking();

	// Engrave Marking Start
	DoAutoMarkingEngrave();
}

BOOL CGvisR2R_PunchView::DoAutoGetLotEndSignal()
{
	int nSerial;

	if (m_pDlgMenu01)
	{
		if (m_pDlgMenu01->m_bLotEnd && m_nStepAuto < LOT_END)
		{
			m_bLotEnd = TRUE;
			m_nLotEndAuto = LOT_END;
		}
	}

	if (!IsBuffer(0) && m_bLastProc && m_nLotEndAuto < LOT_END)
	{
		m_bLotEnd = TRUE;
		m_nLotEndAuto = LOT_END;
	}
	else if(!IsBuffer(0) && m_nMkStAuto > MK_ST + (Mk2PtIdx::DoneMk) + 4)
	{
		m_nMkStAuto = 0;
		m_bLotEnd = TRUE;
		m_nLotEndAuto = LOT_END;
	}


	if (m_bLotEnd)
	{
		nSerial = pDoc->GetLastShotMk();

		switch (m_nLotEndAuto)
		{
		case LOT_END:
			if (!m_bTHREAD_REELMAP_YIELD_UP && !m_bTHREAD_REELMAP_YIELD_DN && !m_bTHREAD_REELMAP_YIELD_ALLUP && !m_bTHREAD_REELMAP_YIELD_ALLDN) // Yield Reelmap
			{
				ReloadReelmap(nSerial);
				UpdateRst();
				//pDoc->UpdateYieldOnRmap(); // 20230614
				m_nLotEndAuto++;
			}
			break;
		case LOT_END + 1:
			m_pMpe->Write(_T("MB440180"), 1);			// �۾�����(PC�� On��Ű��, PLC�� Ȯ���ϰ� Off��Ŵ)-20141031
			DispMain(_T("�۾�����"), RGB_RED);
			m_nLotEndAuto++;
			break;
		case LOT_END + 2:
			Buzzer(TRUE, 0);
			TowerLamp(RGB_YELLOW, TRUE);
			Stop();
			LotEnd();									// MakeResultMDS
			m_nLotEndAuto++;
			break;

		case LOT_END + 3:
			MsgBox(_T("�۾��� ����Ǿ����ϴ�."));
			//m_nStepAuto = 0; // �ڵ�����
			m_nLotEndAuto++;
			//m_bLotEnd = FALSE;
			m_bLastProc = FALSE;
			m_bMkSt = FALSE;
			::WritePrivateProfileString(_T("Last Job"), _T("MkSt"), _T("0"), PATH_WORKING_INFO);
			break;
		case LOT_END + 4:
			break;
		}
	}

	return m_bLotEnd;
}

void CGvisR2R_PunchView::DoAtuoGetMkStSignal()
{
#ifdef USE_MPE
	if (m_pMpe)
	{
		if (!m_bMkSt)
		{
			if (IsRun())
			{
				if (pDoc->m_pMpeSignal[1] & (0x01 << 0) || m_bMkStSw) // AlignTest		// ��ŷ����(PC�� Ȯ���ϰ� Reset��Ŵ.)-20141029
				{
					pDoc->LogAuto(_T("PLC: ��ŷ����(PC�� Ȯ���ϰ� Reset��Ŵ.)"));
					m_bMkStSw = FALSE;

					m_pMpe->Write(_T("MB440110"), 0);			// ��ŷ����(PC�� Ȯ���ϰ� Reset��Ŵ.)-20141029

					if (pDoc->m_pMpeSignal[0] & (0x01 << 1))	// ��ŷ�� Feeding�Ϸ�(PLC�� On��Ű�� PC�� Ȯ���ϰ� Reset��Ŵ.)-20141030
					{
						pDoc->LogAuto(_T("PLC: Feeding�Ϸ�(PLC�� On��Ű�� PC�� Ȯ���ϰ� Reset��Ŵ.)"));
						m_pMpe->Write(_T("MB440101"), 0);		// ��ŷ�� Feeding�Ϸ�
					}

					m_bMkSt = TRUE;
					m_nMkStAuto = MK_ST;
					::WritePrivateProfileString(_T("Last Job"), _T("MkSt"), _T("1"), PATH_WORKING_INFO);

					if (!pDoc->GetLastShotMk())
						SetLotSt();		// ���ο� ��Ʈ�� ���۽ð��� ������. // ��ü�ӵ��� ó�� ���۵Ǵ� �ð� ����.
				}
			}
		}
	}
#endif
}

void CGvisR2R_PunchView::DoAutoSetLastProcAtPlc()
{
	if (m_bLastProc)
	{
		switch (m_nLastProcAuto)
		{
		case LAST_PROC:	// �ܷ�ó�� 1
			if (IsRun())
			{
				if (MODE_INNER != pDoc->GetTestMode())
				{
					if (ChkLastProcFromUp())
					{
						pDoc->LogAuto(_T("PC: �ܷ�ó�� AOI(��) ����(PC�� On��Ű��, PLC�� Ȯ���ϰ� Off��Ŵ)"));
						m_pMpe->Write(_T("MB440185"), 1);			// �ܷ�ó�� AOI(��) ����(PC�� On��Ű��, PLC�� Ȯ���ϰ� Off��Ŵ)-20141112
						m_pMpe->Write(_T("MB44012B"), 1);			// AOI �� : PCR���� Received
					}
					else
					{
						pDoc->LogAuto(_T("PC: �ܷ�ó�� AOI(��) ����(PC�� On��Ű��, PLC�� Ȯ���ϰ� Off��Ŵ)"));
						m_pMpe->Write(_T("MB440186"), 1);			// �ܷ�ó�� AOI(��) ����(PC�� On��Ű��, PLC�� Ȯ���ϰ� Off��Ŵ)-20141112
					}
				}
				else
				{
					if (ChkLastProcFromEng())
					{
						pDoc->LogAuto(_T("PC: �ܷ�ó�� ���κ� ����(PC�� On��Ű��, PLC�� Ȯ���ϰ� Off��Ŵ)"));
						m_pMpe->Write(_T("MB44019D"), 1);			// �ܷ�ó�� ���κ� ����(PC�� On��Ű��, PLC�� Ȯ���ϰ� Off��Ŵ)-20141112
					}
					else if (ChkLastProcFromUp())
					{
						pDoc->LogAuto(_T("PC: �ܷ�ó�� AOI(��) ����(PC�� On��Ű��, PLC�� Ȯ���ϰ� Off��Ŵ)"));
						m_pMpe->Write(_T("MB440185"), 1);			// �ܷ�ó�� AOI(��) ����(PC�� On��Ű��, PLC�� Ȯ���ϰ� Off��Ŵ)-20141112
						m_pMpe->Write(_T("MB44012B"), 1);			// AOI �� : PCR���� Received
					}
					else
					{
						pDoc->LogAuto(_T("PC: �ܷ�ó�� AOI(��) ����(PC�� On��Ű��, PLC�� Ȯ���ϰ� Off��Ŵ)"));
						m_pMpe->Write(_T("MB440186"), 1);			// �ܷ�ó�� AOI(��) ����(PC�� On��Ű��, PLC�� Ȯ���ϰ� Off��Ŵ)-20141112
					}
				}

				m_nLastProcAuto++;
			}
			break;
		case LAST_PROC + 1:
			pDoc->LogAuto(_T("PC: �ܷ�ó��(PC�� On��Ű��, PLC�� Ȯ���ϰ� Off��Ŵ)"));
			m_pMpe->Write(_T("MB440181"), 1);			// �ܷ�ó��(PC�� On��Ű��, PLC�� Ȯ���ϰ� Off��Ŵ)-20141031
			m_nLastProcAuto++;
			break;
		case LAST_PROC + 2:
			;
			break;
		}
	}
}

void CGvisR2R_PunchView::DoAutoSetFdOffsetLastProc()
{
#ifdef USE_MPE
	BOOL bOn0 = (pDoc->m_pMpeIb[10] & (0x01 << 11)) ? TRUE : FALSE;		// �˻�� �� �ڵ� ���� <-> X432B I/F
	BOOL bOn1 = (pDoc->m_pMpeIb[14] & (0x01 << 11)) ? TRUE : FALSE;		// �˻�� �� �ڵ� ���� <-> X442B I/F

	if (bOn0 && !(m_Flag & (0x01 << 2)))
	{
		m_Flag |= (0x01 << 2);
	}
	else if (!bOn0 && (m_Flag & (0x01 << 2)))
	{
		m_Flag &= ~(0x01 << 2);

		m_bAoiTest[0] = FALSE;
		m_bWaitPcr[0] = FALSE;
		m_bAoiTestF[0] = FALSE;
		m_bAoiFdWriteF[0] = FALSE;
		pView->m_pMpe->Write(_T("MB440111"), 0); // �˻��(��) Feeding Offset Write �Ϸ�(PC�� Ȯ���ϰ� Reset��Ŵ.)-20141103
		pDoc->LogAuto(_T("PLC: �˻��(��) Feeding Offset Write �Ϸ�(PC�� Ȯ���ϰ� MB440111 Reset��Ŵ.)"));
	}

	if (bOn1 && !(m_Flag & (0x01 << 3)))
	{
		m_Flag |= (0x01 << 3);
	}
	else if (!bOn1 && (m_Flag & (0x01 << 3)))
	{
		m_Flag &= ~(0x01 << 3);

		m_bAoiTest[1] = FALSE;
		m_bWaitPcr[1] = FALSE;
		m_bAoiTestF[1] = FALSE;
		m_bAoiFdWriteF[1] = FALSE;
		pView->m_pMpe->Write(_T("MB440112"), 0); // �˻��(��) Feeding Offset Write �Ϸ�(PC�� Ȯ���ϰ� Reset��Ŵ.)-20141103
		pDoc->LogAuto(_T("PLC: �˻��(��) Feeding Offset Write �Ϸ�(PC�� Ȯ���ϰ� MB440112 Reset��Ŵ.)"));
	}
#endif
}

void CGvisR2R_PunchView::DoAutoSetFdOffset()
{
#ifdef USE_MPE
	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;
	double dAveX, dAveY;
	CfPoint OfStUp, OfStDn;

	if (pDoc->m_pMpeSignal[1] & (0x01 << 3) && !m_bAoiTestF[0])		// �˻��(��) �˻���-20141103
	{
		m_bAoiTestF[0] = TRUE;
		m_bAoiTest[0] = TRUE;
		m_bWaitPcr[0] = TRUE;
	}
	else if (!(pDoc->m_pMpeSignal[1] & (0x01 << 3)) && m_bAoiTestF[0])
	{
		m_bAoiTestF[0] = FALSE;
		m_bAoiTest[0] = FALSE;//
		m_bAoiFdWriteF[0] = FALSE;

		pView->m_pMpe->Write(_T("MB440111"), 0); // �˻��(��) Feeding Offset Write �Ϸ�(PC�� Ȯ���ϰ� Reset��Ŵ.)-20141103
		pDoc->LogAuto(_T("PLC: �˻��(��) Feeding Offset Write �Ϸ�(PC�� Ȯ���ϰ� MB440111 Reset��Ŵ.)"));
	}

	if (pDoc->m_pMpeSignal[1] & (0x01 << 4) && !m_bAoiTestF[1])		// �˻��(��) �˻���-20141103
	{
		m_bAoiTestF[1] = TRUE;
		m_bAoiTest[1] = TRUE;
		m_bWaitPcr[1] = TRUE;
	}
	else if (!(pDoc->m_pMpeSignal[1] & (0x01 << 4)) && m_bAoiTestF[1])
	{
		m_bAoiTestF[1] = FALSE;
		m_bAoiTest[1] = FALSE;//
		m_bAoiFdWriteF[1] = FALSE;
		pView->m_pMpe->Write(_T("MB440112"), 0); // �˻��(��) Feeding Offset Write �Ϸ�(PC�� Ȯ���ϰ� Reset��Ŵ.)-20141103
		pDoc->LogAuto(_T("PLC: �˻��(��) Feeding Offset Write �Ϸ�(PC�� Ȯ���ϰ� MB440112 Reset��Ŵ.)"));
	}


	if (pDoc->m_pMpeSignal[1] & (0x01 << 1) && !m_bAoiFdWrite[0])		// �˻��(��) Feeding Offset Write �Ϸ�-20141103
		m_bAoiFdWrite[0] = TRUE;
	else if (!(pDoc->m_pMpeSignal[1] & (0x01 << 1)) && m_bAoiFdWrite[0])
		m_bAoiFdWrite[0] = FALSE;

	if (pDoc->m_pMpeSignal[1] & (0x01 << 2) && !m_bAoiFdWrite[1])		// �˻��(��) Feeding Offset Write �Ϸ�-20141103
		m_bAoiFdWrite[1] = TRUE;
	else if (!(pDoc->m_pMpeSignal[1] & (0x01 << 2)) && m_bAoiFdWrite[1])
		m_bAoiFdWrite[1] = FALSE;


	if (bDualTest) // 20160721-syd-temp
	{
		if ((m_bAoiFdWrite[0] && m_bAoiFdWrite[1]) && (!m_bAoiFdWriteF[0] && !m_bAoiFdWriteF[1]))
		{
			m_bAoiFdWriteF[0] = TRUE;
			m_bAoiFdWriteF[1] = TRUE;

			GetAoiUpOffset(OfStUp);
			GetAoiDnOffset(OfStDn);

			dAveX = OfStUp.x;
			dAveY = OfStUp.y; // syd - 20230327
			//dAveX = (OfStUp.x + OfStDn.x) / 2.0;
			//dAveY = (OfStUp.y + OfStDn.y) / 2.0;

			if (m_pDlgMenu02)
			{
				m_pDlgMenu02->m_dAoiUpFdOffsetX = OfStUp.x;
				m_pDlgMenu02->m_dAoiUpFdOffsetY = OfStUp.y;
				m_pDlgMenu02->m_dAoiDnFdOffsetX = OfStDn.x;
				m_pDlgMenu02->m_dAoiDnFdOffsetY = OfStDn.y;
			}

			pView->m_pMpe->Write(_T("ML45064"), (long)(-1.0*dAveX*1000.0));
			pView->m_pMpe->Write(_T("MB440111"), 0); // �˻��(��) Feeding Offset Write �Ϸ�(PC�� Ȯ���ϰ� Reset��Ŵ.)-20141103
			pView->m_pMpe->Write(_T("MB440112"), 0); // �˻��(��) Feeding Offset Write �Ϸ�(PC�� Ȯ���ϰ� Reset��Ŵ.)-20141103
			pDoc->LogAuto(_T("PLC: �˻��(��MB440111,��MB440112) Feeding Offset Write �Ϸ�(PC�� Ȯ���ϰ� Reset��Ŵ.)"));
		}
		else if ((!m_bAoiFdWrite[0] && !m_bAoiFdWrite[1]) && (m_bAoiFdWriteF[0] && m_bAoiFdWriteF[1]))
		{
			m_bAoiFdWriteF[0] = FALSE;
			m_bAoiFdWriteF[1] = FALSE;
			m_bAoiTest[0] = FALSE;
			m_bAoiTest[1] = FALSE;
		}

		if (m_bAoiTest[0] && !m_bAoiTest[1])
		{
			if (m_bAoiFdWrite[0] && !m_bAoiFdWriteF[0])
			{
				m_bAoiFdWriteF[0] = TRUE;

				GetAoiUpOffset(OfStUp);

				if (m_pDlgMenu02)
				{
					m_pDlgMenu02->m_dAoiUpFdOffsetX = OfStUp.x;
					m_pDlgMenu02->m_dAoiUpFdOffsetY = OfStUp.y;
					m_pDlgMenu02->m_dAoiDnFdOffsetX = 0.0;
					m_pDlgMenu02->m_dAoiDnFdOffsetY = 0.0;
				}

				pView->m_pMpe->Write(_T("ML45064"), (long)(-1.0*OfStUp.x*1000.0));
				pView->m_pMpe->Write(_T("MB440111"), 0); // �˻��(��) Feeding Offset Write �Ϸ�(PC�� Ȯ���ϰ� Reset��Ŵ.)-20141103
				pDoc->LogAuto(_T("PLC: �˻��(��MB440111) Feeding Offset Write �Ϸ�(PC�� Ȯ���ϰ� Reset��Ŵ.)"));
			}
			else if (!m_bAoiFdWrite[0] && m_bAoiFdWriteF[0])
			{
				m_bAoiFdWriteF[0] = FALSE;
				m_bAoiTest[0] = FALSE;
			}
		}

		if (!m_bAoiTest[0] && m_bAoiTest[1])
		{
			if (m_bAoiFdWrite[1] && !m_bAoiFdWriteF[1])
			{
				m_bAoiFdWriteF[1] = TRUE;

				GetAoiDnOffset(OfStDn);

				if (m_pDlgMenu02)
				{
					m_pDlgMenu02->m_dAoiUpFdOffsetX = 0.0;
					m_pDlgMenu02->m_dAoiUpFdOffsetY = 0.0;
					m_pDlgMenu02->m_dAoiDnFdOffsetX = OfStDn.x;
					m_pDlgMenu02->m_dAoiDnFdOffsetY = OfStDn.y;
				}

				pView->m_pMpe->Write(_T("ML45064"), (long)(-1.0*OfStDn.x*1000.0));
				pView->m_pMpe->Write(_T("MB440112"), 0); // �˻��(��) Feeding Offset Write �Ϸ�(PC�� Ȯ���ϰ� Reset��Ŵ.)-20141103
				pDoc->LogAuto(_T("PLC: �˻��(��MB440112) Feeding Offset Write �Ϸ�(PC�� Ȯ���ϰ� Reset��Ŵ.)"));
			}
			else if (!m_bAoiFdWrite[1] && m_bAoiFdWriteF[1])
			{
				m_bAoiFdWriteF[1] = FALSE;
				m_bAoiTest[1] = FALSE;
			}
		}
	}
	else // 20160721-syd-temp
	{
		if (m_bAoiFdWrite[0] && !m_bAoiFdWriteF[0])
		{
			m_bAoiFdWriteF[0] = TRUE;

			GetAoiUpOffset(OfStUp);

			dAveX = (OfStUp.x);
			dAveY = (OfStUp.y);

			if (m_pDlgMenu02)
			{
				m_pDlgMenu02->m_dAoiUpFdOffsetX = dAveX;
				m_pDlgMenu02->m_dAoiUpFdOffsetY = dAveY;
			}

			pView->m_pMpe->Write(_T("ML45064"), (long)(-1.0*dAveX*1000.0));	// �˻�� Feeding �ѷ� Offset(*1000, +:�� ����, -�� ����)
			pView->m_pMpe->Write(_T("MB440111"), 0); // �˻��(��) Feeding Offset Write �Ϸ�(PC�� Ȯ���ϰ� Reset��Ŵ.)-20141103
			pView->m_pMpe->Write(_T("MB440112"), 0); // �˻��(��) Feeding Offset Write �Ϸ�(PC�� Ȯ���ϰ� Reset��Ŵ.)-20141103  // 20160721-syd-temp
			pDoc->LogAuto(_T("PLC: �˻��(��MB440111,��MB440112) Feeding Offset Write �Ϸ�(PC�� Ȯ���ϰ� Reset��Ŵ.)"));
		}
		else if (!m_bAoiFdWrite[0] && m_bAoiFdWriteF[0])
		{
			m_bAoiFdWriteF[0] = FALSE;
			m_bAoiTest[0] = FALSE;
		}
	}
#endif
}

void CGvisR2R_PunchView::DoAutoChkCycleStop()
{
	if (m_bCycleStop)
	{
		m_bCycleStop = FALSE;
		Buzzer(TRUE);
		TowerLamp(RGB_YELLOW, TRUE);
		//MyMsgBox(pDoc->m_sAlmMsg);
		if (!pDoc->m_sAlmMsg.IsEmpty())
		{
			pDoc->LogAuto(pDoc->m_sAlmMsg);
			MsgBox(pDoc->m_sAlmMsg, 0, 0, DEFAULT_TIME_OUT, FALSE);

			if (pDoc->m_sAlmMsg == m_sAoiUpAlarmReStartMsg || pDoc->m_sAlmMsg == m_sAoiUpAlarmReTestMsg)
			{
				ChkReTestAlarmOnAoiUp();
				//SetAoiUpAutoStep(2); // Wait for AOI �˻���� ��ȣ.
				//Sleep(300);
				//if (m_pMpe)
				//{
				//	m_pMpe->Write(_T("MB44013B"), 1); // �˻�� ��� ���۾� (���۽�ȣ) : PC�� On��Ű�� PLC�� Off
				//	pDoc->LogAuto(_T("PC: �˻�� ��� ���۾� (���۽�ȣ) : PC�� On��Ű�� PLC�� Off"));
				//}
			}
			else if (pDoc->m_sAlmMsg == m_sAoiDnAlarmReStartMsg || pDoc->m_sAlmMsg == m_sAoiDnAlarmReTestMsg)
			{
				ChkReTestAlarmOnAoiDn();
				//SetAoiDnAutoStep(2); // Wait for AOI �˻���� ��ȣ.
				//Sleep(300);
				//if (m_pMpe)
				//{
				//	m_pMpe->Write(_T("MB44013C"), 1); // �˻�� �Ϻ� ���۾� (���۽�ȣ) : PC�� On��Ű�� PLC�� Off
				//	pDoc->LogAuto(_T("PC: �˻�� �Ϻ� ���۾� (���۽�ȣ) : PC�� On��Ű�� PLC�� Off"));
				//}
			}
		}
		pDoc->m_sAlmMsg = _T("");
		pDoc->m_sIsAlmMsg = _T("");
		pDoc->m_sPrevAlmMsg = _T("");
	}
}

void CGvisR2R_PunchView::DoAutoDispMsg()
{
	BOOL bDispMsg = FALSE;
	int idx, nStepDispMsg;

	// [2] : �����ҷ�-��, [3] : �����ҷ�-��, [5] : nSerialL <= 0, [6] : CopyDefImg ���� Camera,  
	// [7] : CopyDefImg ���� Camera, [8] : ���̽�����(��) �ʱ���ġ �̵�, [9] : ���̽�����(��) �ʱ���ġ �̵�
	for (idx = 0; idx < 10; idx++)
	{
		if (m_bDispMsgDoAuto[idx]) 
		{
			bDispMsg = TRUE;
			nStepDispMsg = m_nStepDispMsg[idx];
			break;
		}
	}
	if (bDispMsg && IsRun())
	{
		switch (nStepDispMsg)
		{
		case 0:
			break;
		case FROM_DOMARK0:
			m_bDispMsgDoAuto[8] = FALSE;
			m_nStepDispMsg[8] = 0;
			Buzzer(TRUE, 0);
			TowerLamp(RGB_YELLOW, TRUE);
			Stop();
			//pView->DispStsBar(_T("����-37"), 0);
			DispMain(_T("�� ��"), RGB_RED);
			break;
		case FROM_DOMARK1:
			m_bDispMsgDoAuto[9] = FALSE;
			m_nStepDispMsg[9] = 0;
			Buzzer(TRUE, 0);
			TowerLamp(RGB_YELLOW, TRUE);
			Stop();
			//pView->DispStsBar(_T("����-38"), 0);
			DispMain(_T("�� ��"), RGB_RED);
			break;
		case FROM_DISPDEFIMG:
			m_bDispMsgDoAuto[0] = FALSE;
			m_nStepDispMsg[0] = 0;
			Stop();
			MsgBox(_T("����(��) Serial�� �����ʽ��ϴ�."));
			TowerLamp(RGB_YELLOW, TRUE);
			break;
		case FROM_DISPDEFIMG + 1:
			m_bDispMsgDoAuto[1] = FALSE;
			m_nStepDispMsg[1] = 0;
			Stop();
			MsgBox(_T("����(��) Serial�� �����ʽ��ϴ�."));
			TowerLamp(RGB_YELLOW, TRUE);
			break;
		case FROM_DISPDEFIMG + 2: // IsFixUp
			m_bDispMsgDoAuto[2] = FALSE;
			m_nStepDispMsg[2] = 0;
			Buzzer(TRUE, 0);
			TowerLamp(RGB_RED, TRUE);
			Stop();
			m_bSwStopNow = TRUE;
			m_bSwRunF = FALSE;
			//pView->DispStsBar(_T("����-39"), 0);
			DispMain(_T("�� ��"), RGB_RED);
			MsgBox(m_sFixMsg[0]);
			m_sFixMsg[0] = _T("");
			break;
		case FROM_DISPDEFIMG + 3: // IsFixDn
			m_bDispMsgDoAuto[3] = FALSE;
			m_nStepDispMsg[3] = 0;
			Buzzer(TRUE, 0);
			TowerLamp(RGB_RED, TRUE);
			Stop();
			m_bSwStopNow = TRUE;
			m_bSwRunF = FALSE;
			//pView->DispStsBar(_T("����-40"), 0);
			DispMain(_T("�� ��"), RGB_RED);
			MsgBox(m_sFixMsg[1]);
			m_sFixMsg[1] = _T("");
			break;
		case FROM_DISPDEFIMG + 4:
			// 			m_bDispMsgDoAuto[4] = FALSE;
			// 			m_nStepDispMsg[4] = 0;
			// 			Stop();
			// 			TowerLamp(RGB_RED, TRUE);
			// 			Buzzer(TRUE, 0);
			// 			m_bSwStopNow = TRUE;
			// 			m_bSwRunF = FALSE;
			// 			pView->DispStsBar(_T("����-41"), 0);
			// 			DispMain(_T("�� ��"), RGB_RED);	
			// 			MsgBox(m_sFixMsg);
			// 			m_sFixMsg = _T("");
			break;
		case FROM_DISPDEFIMG + 5:
			m_bDispMsgDoAuto[5] = FALSE;
			m_nStepDispMsg[5] = 0;
			Stop();
			MsgBox(_T("����(��) Serial�� �����ʽ��ϴ�."));
			TowerLamp(RGB_YELLOW, TRUE);
			break;
		case FROM_DISPDEFIMG + 6:
			m_bDispMsgDoAuto[6] = FALSE;
			m_nStepDispMsg[6] = 0;
			Buzzer(TRUE, 0);
			TowerLamp(RGB_RED, TRUE);
			Stop();
			m_bSwStopNow = TRUE;
			m_bSwRunF = FALSE;
			//pView->DispStsBar(_T("����-42"), 0);
			DispMain(_T("�� ��"), RGB_RED);
			break;
		case FROM_DISPDEFIMG + 7:
			m_bDispMsgDoAuto[7] = FALSE;
			m_nStepDispMsg[7] = 0;
			Buzzer(TRUE, 0);
			TowerLamp(RGB_RED, TRUE);
			Stop();
			m_bSwStopNow = TRUE;
			m_bSwRunF = FALSE;
			//pView->DispStsBar(_T("����-43"), 0);
			DispMain(_T("�� ��"), RGB_RED);
			break;
		}
	}
}

void CGvisR2R_PunchView::DoAutoChkShareFolder()	// 20170727-�ܷ�ó�� �� ��������� �ݺ��ؼ� ���Լ��� ȣ������� �¿� ��ŷ �ε��� ���� ���� �߻�.(case AT_LP + 8:)
{
	CString sLot, sLayerUp, sLayerDn;
	BOOL bDualTestInner;

	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;
	int nSerial = 0;
	CString sNewLot;
	int nNewLot = 0;
	BOOL bPcrInShare[2];
	BOOL bNewModel = FALSE;

	switch (m_nStepAuto)
	{
	case 0:
		m_bSwRun = FALSE;
		m_bSwStop = TRUE;
		m_nStepAuto++;
		break;
	case 1:
		if (IsReady() || IsAuto())		// �����غ�
		{
			TowerLamp(RGB_YELLOW, TRUE, TRUE);
			m_nStepAuto++;
		}
		break;
	case 2:
		if (IsRun())
			m_nStepAuto++;
		break;
	case 3:
		ClrDispMsg();
		TowerLamp(RGB_YELLOW, TRUE, FALSE);
		m_nStepAuto++;
		break;
	case 4:
		if (IsRun())		// �ʱ����
		{
			if (!IsAoiLdRun())
			{
				Stop();
				TowerLamp(RGB_YELLOW, TRUE);
			}
			else
			{
				ResetWinker(); // 20151126 : ��������ġ ���� ���� ������ ����.

				TowerLamp(RGB_GREEN, TRUE, TRUE);
				if (pDoc->WorkingInfo.LastJob.bSampleTest)
				{
					if (pDoc->WorkingInfo.LastJob.bDualTest)
					{
						if (m_sDispMain != _T("������"))
							DispMain(_T("������"), RGB_GREEN);
					}
					else
					{
						if (m_sDispMain != _T("�ܸ����"))
							DispMain(_T("�ܸ����"), RGB_GREEN);
					}
				}
				else if(pDoc->GetTestMode() == MODE_INNER)
				{
					if (m_sDispMain != _T("�����˻�"))
						DispMain(_T("�����˻�"), RGB_GREEN);
				}
				else if (pDoc->GetTestMode() == MODE_OUTER)
				{
					if (m_sDispMain != _T("�����˻�"))
						DispMain(_T("�����˻�"), RGB_GREEN);
				}
				else if (pDoc->WorkingInfo.LastJob.bDualTest)
				{
					if (m_sDispMain != _T("���˻�"))
						DispMain(_T("���˻�"), RGB_GREEN);
				}
				else
				{
					if (m_sDispMain != _T("�ܸ�˻�"))
						DispMain(_T("�ܸ�˻�"), RGB_GREEN);
 					//if(m_sDispMain != _T("�ʱ����")
 					//	DispMain(_T("�ʱ����", RGB_GREEN);
				}
				m_nVsBufLastSerial[0] = GetVsUpBufLastSerial();
				if (bDualTest)
					m_nVsBufLastSerial[1] = GetVsDnBufLastSerial();

				SetListBuf();

				if (MODE_INNER == pDoc->GetTestMode() || MODE_OUTER == pDoc->GetTestMode()) // Please modify for outer mode.-20221226
				{
					GetCurrentInfoEng();
					if (m_pDlgMenu01)
						m_pDlgMenu01->UpdateData();
				}

				
				m_nStepAuto = AT_LP;
			}
		}
		else
			Winker(MN_RUN); // Run Button - 20151126 : ��������ġ ���� ���� ������ ����.
		break;

	case AT_LP:
		if (IsShare())
		{
			bPcrInShare[0] = FALSE;
			bPcrInShare[1] = FALSE;

			//if (IsShareUp() && IsTestDoneUp() && !m_bAoiTestF[0]) // �˻��(��) �˻���
			if (IsShareUp())
			{
				nSerial = GetShareUp();
				if (nSerial > 0)
				{
					if (pView->m_bSerialDecrese)
					{
						if (m_nLotEndSerial > 0 && nSerial < m_nLotEndSerial)
						{
							// Delete PCR File
							pDoc->DelSharePcrUp();
						}
						else
						{
							m_nShareUpS = nSerial;
							bPcrInShare[0] = TRUE;
						}
					}
					else
					{
						if (m_nLotEndSerial > 0 && nSerial > m_nLotEndSerial)
						{
							// Delete PCR File
							pDoc->DelSharePcrUp();
						}
						else
						{
							m_nShareUpS = nSerial;
							bPcrInShare[0] = TRUE;
						}
					}
				}
				else
				{
					m_bLoadShare[0] = FALSE;
				}
			}
			else
				m_bLoadShare[0] = FALSE;


			if (bDualTest)
			{
				//if (IsShareDn() && IsTestDoneDn() && !m_bAoiTestF[1]) // �˻��(��) �˻���
				if (IsShareDn())
				{
					nSerial = GetShareDn();
					if (nSerial > 0)
					{
						if (pView->m_bSerialDecrese)
						{
							if (m_nLotEndSerial > 0 && nSerial < m_nLotEndSerial)
							{
								// Delete PCR File
								pDoc->DelSharePcrDn();
							}
							else
							{
								m_nShareDnS = nSerial;
								bPcrInShare[1] = TRUE;
							}
						}
						else
						{
							if (m_nLotEndSerial > 0 && nSerial > m_nLotEndSerial)
							{
								// Delete PCR File
								pDoc->DelSharePcrDn();
							}
							else
							{
								m_nShareDnS = nSerial;
								bPcrInShare[1] = TRUE;
							}
						}
					}
					else
					{
						m_bLoadShare[1] = FALSE;
					}
				}
				else
					m_bLoadShare[1] = FALSE;

				if (bPcrInShare[0] || bPcrInShare[1])
					m_nStepAuto++;
			}
			else
			{
				if (bPcrInShare[0])
					m_nStepAuto++;
			}
		}
		else
		{
			if (!m_bLastProc)
			{
				if (ChkLastProc())
				{
					m_nLastProcAuto = LAST_PROC;
					m_bLastProc = TRUE;
					nSerial = GetShareUp();

					//if (IsVs())
					//{
					//	if (m_nAoiLastSerial[0] < 1)
					//		m_nAoiLastSerial[0] = nSerial;

					//	m_nPrevStepAuto = m_nStepAuto;
					//	m_nStepAuto = LAST_PROC_VS_ALL;		 // �ܷ�ó�� 3
					//	break;
					//}
					//else
					{
						if (bDualTest)
						{
							if (ChkLastProcFromEng())
							{
								if (IsSetLotEnd())
									nSerial = GetLotEndSerial();
								else
									nSerial = pDoc->GetCurrentInfoEngShotNum();
							}
							else if (ChkLastProcFromUp())
							{
								if (IsSetLotEnd())
									nSerial = GetLotEndSerial();
								else
									nSerial = pDoc->m_ListBuf[0].GetLast();
							}
							else
							{
								if (IsSetLotEnd())
									nSerial = GetLotEndSerial();
								else
									nSerial = pDoc->m_ListBuf[1].GetLast();
							}
						}
						else
						{
							if (ChkLastProcFromEng())
							{
								if (IsSetLotEnd())
									nSerial = GetLotEndSerial();
								else
									nSerial = pDoc->GetCurrentInfoEngShotNum();
							}
							else
							{
								if (IsSetLotEnd())
									nSerial = GetLotEndSerial();
								else
									nSerial = pDoc->m_ListBuf[0].GetLast();
							}
						}

						if (!IsSetLotEnd()) // 20160810
						{
							SetLotEnd(nSerial);//+pDoc->AoiDummyShot[1]); // 3
						//if (m_nAoiLastSerial[0] < 1)
							//m_nAoiLastSerial[0] = nSerial;
						}

						m_nStepAuto++;
					}
				}
			}
			else
			{
				if (ChkLastProcFromEng())
				{
					if (IsSetLotEnd())
						nSerial = GetLotEndSerial();
					else
						nSerial = pDoc->GetCurrentInfoEngShotNum();

					//if (m_nLotEndSerial != nSerial)
					if (!IsSetLotEnd())
					{
						SetLotEnd(nSerial);
						//if (m_nAoiLastSerial[0] < 1)
						//m_nAoiLastSerial[0] = nSerial;
					}
				}

				m_bWaitPcr[0] = FALSE;
				m_bWaitPcr[1] = FALSE;
				m_nStepAuto++;
			}
			
			if (MODE_INNER == pDoc->GetTestMode())
			{
				if (IsSetLotEnd())
					nSerial = GetLotEndSerial();
				else
					nSerial = pDoc->GetCurrentInfoEngShotNum();
				//SetLastSerialEng(nSerial);

				if (ChkLastProc())
				{
					if (ChkLastProcFromEng())
					{
						//if (m_nLotEndSerial != nSerial)
						if (!IsSetLotEnd())
							SetLotEnd(nSerial);
					}
				}
			}

		}
		break;

	case AT_LP + 1:
		if (!m_bCont) // �̾�� �ƴ� ���.
		{
			if (!ChkStShotNum())
			{
				Stop();
				TowerLamp(RGB_YELLOW, TRUE);
			}
		}
		else
		{
			//if (!ChkContShotNum())
			//{
			//	Stop();
			//	TowerLamp(RGB_YELLOW, TRUE);
			//}
		}
		m_nStepAuto++;
		break;
	case AT_LP + 2:
		if (IsRun())
		{
			m_bBufEmpty[0] = pDoc->m_bBufEmpty[0]; // Up
			m_nStepAuto++;
		}
		break;

	case AT_LP + 3:
		Shift2Buf();			// PCR �̵�(Share->Buffer)
		m_nStepAuto++;
		break;

	case AT_LP + 4:
		if (!IsRun())
			break;

		if (m_bTHREAD_UPDATE_REELMAP_UP || m_bTHREAD_UPDATE_REELMAP_DN || m_bTHREAD_UPDATE_REELMAP_ALLUP || m_bTHREAD_UPDATE_REELMAP_ALLDN) // Write Reelmap
		{
			Sleep(100);
			break;
		}

		if (m_bTHREAD_UPDATE_YIELD_UP || m_bTHREAD_UPDATE_YIELD_DN || m_bTHREAD_UPDATE_YIELD_ALLUP || m_bTHREAD_UPDATE_YIELD_ALLDN) // Write Reelmap
		{
			Sleep(100);
			break;
		}

		if (!bDualTest)
		{
			if (pDoc->GetTestMode() == MODE_OUTER)
			{
				if (m_bTHREAD_UPDATE_REELMAP_UP || m_bTHREAD_UPDATE_REELMAP_INNER_UP) // Write Reelmap
					break;

				if (pDoc->WorkingInfo.LastJob.bDualTestInner)
				{
					if (m_bTHREAD_UPDATE_REELMAP_INNER_DN || m_bTHREAD_UPDATE_REELMAP_INNER_ALLUP || m_bTHREAD_UPDATE_REELMAP_INNER_ALLDN) // Write Reelmap
						break;
				}
			}
		}

		m_nStepAuto++;

		if (m_nShareUpS > 0)
		{
			//if (m_nShareUpS % 2)
			//	m_nShareUpSerial[0] = m_nShareUpS; // Ȧ��
			//else
			//	m_nShareUpSerial[1] = m_nShareUpS; // ¦��
			m_nShareUpCnt++;

			if (pDoc->GetCurrentInfoEng())
			{
				if (pDoc->GetTestMode() == MODE_OUTER)
					pDoc->GetItsSerialInfo(m_nShareUpS, bDualTestInner, sLot, sLayerUp, sLayerDn, 0);
			}

			bNewModel = GetAoiUpInfo(m_nShareUpS, &nNewLot); // Buffer���� PCR������ ��� ������ ����.

			if (bNewModel)	// AOI ����(AoiCurrentInfoPath) -> AOI Feeding Offset
			{
				m_bNewModel = TRUE;
				InitInfo();
				ResetMkInfo(0); // 0 : AOI-Up , 1 : AOI-Dn , 2 : AOI-UpDn	

				pDoc->GetCamPxlRes();

				if (IsLastJob(0)) // Up
				{
					pDoc->m_Master[0].Init(pDoc->WorkingInfo.System.sPathCamSpecDir,
						pDoc->WorkingInfo.LastJob.sModelUp,
						pDoc->WorkingInfo.LastJob.sLayerUp);
					pDoc->m_Master[0].LoadMstInfo();
					//pDoc->m_Master[0].WriteStripPieceRegion_Text(pDoc->WorkingInfo.System.sPathOldFile, pDoc->WorkingInfo.LastJob.sLotUp);


					if (m_pEngrave)
						m_pEngrave->SwMenu01UpdateWorking(TRUE);


					if (pDoc->GetTestMode() == MODE_OUTER)
					{
						//GetCurrentInfoEng();
						pDoc->m_MasterInner[0].Init(pDoc->WorkingInfo.System.sPathCamSpecDir,
							pDoc->WorkingInfo.LastJob.sModelUp,
							pDoc->WorkingInfo.LastJob.sInnerLayerUp);
						pDoc->m_MasterInner[0].LoadMstInfo();
					}
				}

				if (IsLastJob(1)) // Dn
				{
					pDoc->m_Master[1].Init(pDoc->WorkingInfo.System.sPathCamSpecDir,
						pDoc->WorkingInfo.LastJob.sModelDn,
						pDoc->WorkingInfo.LastJob.sLayerDn,
						pDoc->WorkingInfo.LastJob.sLayerUp);
					pDoc->m_Master[1].LoadMstInfo();
					//pDoc->m_Master[1].WriteStripPieceRegion_Text(pDoc->WorkingInfo.System.sPathOldFile, pDoc->WorkingInfo.LastJob.sLotDn);

					if (pDoc->GetTestMode() == MODE_OUTER)
					{
						//GetCurrentInfoEng();
						pDoc->m_MasterInner[0].Init(pDoc->WorkingInfo.System.sPathCamSpecDir,
							pDoc->WorkingInfo.LastJob.sModelUp,
							pDoc->WorkingInfo.LastJob.sInnerLayerDn,
							pDoc->WorkingInfo.LastJob.sInnerLayerUp);
						pDoc->m_MasterInner[0].LoadMstInfo();
					}
				}

				SetAlignPos();

				// 20220502 - end
				InitReelmap();	// 20220421


				if (pDoc->m_Master[0].IsMstSpec(pDoc->WorkingInfo.System.sPathCamSpecDir, pDoc->WorkingInfo.LastJob.sModelUp, pDoc->WorkingInfo.LastJob.sInnerLayerUp))
					InitReelmapInner();	// 20220421


				ModelChange(0); // 0 : AOI-Up , 1 : AOI-Dn

				if (m_pDlgMenu01)
				{
					m_pDlgMenu01->InitGL();
					m_bDrawGL_Menu01 = TRUE;
					m_pDlgMenu01->RefreshRmap();
					m_pDlgMenu01->InitCadImg();
					m_pDlgMenu01->SetPnlNum();
					m_pDlgMenu01->SetPnlDefNum();
				}

				if (m_pDlgMenu02)
				{
					m_pDlgMenu02->ChgModelUp(); // PinImg, AlignImg�� Display��.
					m_pDlgMenu02->InitCadImg();
				}

				if (pDoc->GetTestMode() == MODE_OUTER) // syd-20231121
				{
					if (m_pDlgMenu06)
					{
						m_pDlgMenu06->InitGL();
						m_bDrawGL_Menu06 = TRUE;
						m_pDlgMenu06->RefreshRmap();
						m_pDlgMenu06->InitCadImg();
						m_pDlgMenu06->SetPnlNum();
						m_pDlgMenu06->SetPnlDefNum();
					}
				}

			}
			else
			{
				if (m_nShareUpS == 1)
				{
					pDoc->m_nAoiCamInfoStrPcs[0] = GetAoiUpCamMstInfo();
					if ((pDoc->m_nAoiCamInfoStrPcs[0] == 1 ? TRUE : FALSE) != pDoc->WorkingInfo.System.bStripPcsRgnBin)
					{
						if (pDoc->m_nAoiCamInfoStrPcs[0] == 1 ? TRUE : FALSE)
							pView->MsgBox(_T("���� ��ŷ�δ� �Ϲ� ��� �ε�, \r\n��� AOI�� DTS ��忡�� �˻縦 �����Ͽ����ϴ�."));
						else
							pView->MsgBox(_T("���� ��ŷ�δ� DTS ��� �ε�, \r\n��� AOI�� �Ϲ� ��忡�� �˻縦 �����Ͽ����ϴ�."));

						Stop();
						TowerLamp(RGB_RED, TRUE);
						break;

					}
				}
			}
			
			if (nNewLot)
			{
				if (!pDoc->m_bNewLotShare[0])
				{
					pDoc->m_bNewLotShare[0] = TRUE;// Lot Change.
					if (!bDualTest)
						OpenReelmapFromBuf(m_nShareUpS);
				}
			}
			
			LoadPcrUp(m_nShareUpS);				// Default: From Buffer, TRUE: From Share

			if (!bDualTest)
			{
				if (m_nShareUpS != m_nShareUpSprev)
				{
					m_nShareUpSprev = m_nShareUpS;
					UpdateReelmap(m_nShareUpS); // �ø��������� ������ ������ ���� 
				}
			}

			if (!m_bLastProc)
			{
				//if (!IsSetLotEnd())
				//{
				//	if (ChkLotEndUp(m_nShareUpS))// ������ ���� �߿� Lot End (-2) �ܷ�ó���� üũ��. (���� 3Pnl:-2) -> ��Ʈ�Ϸ� 
				//	{
				//		SetLotEnd(m_nShareUpS - pDoc->AoiDummyShot[0]);
				//		if (m_nAoiLastSerial[0] < 1)
				//			m_nAoiLastSerial[0] = m_nShareUpS;

				//		if (!bDualTest)
				//		{
				//			m_bLastProc = TRUE;
				//			m_nLastProcAuto = LAST_PROC;
				//		}
				//	}
				//}

				if (ChkLastProc())
				{
					m_nLastProcAuto = LAST_PROC;
					m_bLastProc = TRUE;

					//if (IsVs())
					//{
					//	if (m_nAoiLastSerial[0] < 1)
					//		m_nAoiLastSerial[0] = m_nShareUpS;

					//	m_nPrevStepAuto = m_nStepAuto;
					//	m_nStepAuto = LAST_PROC_VS_ALL;		 // �ܷ�ó�� 3
					//	break;
					//}
					//else
					{
						if (bDualTest)
						{
							if (ChkLastProcFromEng())
							{
								if (IsSetLotEnd())
									nSerial = GetLotEndSerial();
								else
									nSerial = pDoc->GetCurrentInfoEngShotNum();
							}
							else if (ChkLastProcFromUp())
							{
								if (IsSetLotEnd())
									nSerial = GetLotEndSerial();
								else
									nSerial = pDoc->m_ListBuf[0].GetLast();
							}
							else
							{
								if (IsSetLotEnd())
									nSerial = GetLotEndSerial();
								else
									nSerial = pDoc->m_ListBuf[1].GetLast();
							}
						}
						else
						{
							if (ChkLastProcFromEng())
							{
								if (IsSetLotEnd())
									nSerial = GetLotEndSerial();
								else
									nSerial = pDoc->GetCurrentInfoEngShotNum();
							}
							else
							{
								if (IsSetLotEnd())
									nSerial = GetLotEndSerial();
								else
									nSerial = pDoc->m_ListBuf[0].GetLast();
							}
						}

						if (!IsSetLotEnd()) // 20160810
						{
							SetLotEnd(nSerial);//+pDoc->AoiDummyShot[1]); // 3
							//if (m_nAoiLastSerial[0] < 1)
							//m_nAoiLastSerial[0] = nSerial;
						}
					}
				}
			}
			else
			{
				if (ChkLastProcFromEng())
				{
					if (IsSetLotEnd())
						nSerial = GetLotEndSerial();
					else
						nSerial = pDoc->GetCurrentInfoEngShotNum();

					//if (m_nLotEndSerial != nSerial)
					if (!IsSetLotEnd())
					{
						SetLotEnd(nSerial);
						//if (m_nAoiLastSerial[0] < 1)
						//m_nAoiLastSerial[0] = nSerial;
					}
				}
			}
		}
		break;

	case AT_LP + 5:
		if (!IsRun())
			break;

		if (!bDualTest)
		{
			m_nStepAuto++;
			break;
		}

		if (m_bTHREAD_UPDATE_REELMAP_DN || m_bTHREAD_UPDATE_REELMAP_ALLUP || m_bTHREAD_UPDATE_REELMAP_ALLDN) // Write Reelmap
		{
			Sleep(100);
			break;
		}

		if (m_bTHREAD_REELMAP_YIELD_DN || m_bTHREAD_REELMAP_YIELD_ALLUP || m_bTHREAD_REELMAP_YIELD_ALLDN) // Write Reelmap
		{
			Sleep(100);
			break;
		}

		m_nStepAuto++;

		//if (m_bChkLastProcVs)
		//{
		//	if (m_nShareDnS > m_nAoiLastSerial[0] && m_nAoiLastSerial[0] > 0)
		//		break;
		//}
		//else
		{
			if (IsSetLotEnd())
			{
				//if (m_nShareDnS > m_nAoiLastSerial[0] && m_nAoiLastSerial[0] > 0)
				if (m_nShareDnS > GetLotEndSerial())
					break;
			}
		}


		if (m_nShareDnS > 0)
		{
			if (m_nShareDnS % 2)
				m_nShareDnSerial[0] = m_nShareDnS; // Ȧ��
			else
				m_nShareDnSerial[1] = m_nShareDnS; // ¦��
			m_nShareDnCnt++;


			bNewModel = GetAoiDnInfo(m_nShareDnS, &nNewLot);

			if (bNewModel)	// AOI ����(AoiCurrentInfoPath) -> AOI Feeding Offset
			{
				//MsgBox(_T("�ű� �𵨿� ���� AOI(��)���� ��Ʈ �и��� �Ǿ����ϴ�.\r\n���� ��Ʈ�� �ܷ�ó�� �մϴ�.");
				InitInfo();
				ResetMkInfo(1); // 0 : AOI-Up , 1 : AOI-Dn , 2 : AOI-UpDn	
				ModelChange(1); // 0 : AOI-Up , 1 : AOI-Dn

			}
			else
			{
				if (m_nShareDnS == 1)
				{
					pDoc->m_nAoiCamInfoStrPcs[1] = GetAoiDnCamMstInfo();
					if ((pDoc->m_nAoiCamInfoStrPcs[1] == 1 ? TRUE : FALSE) != pDoc->WorkingInfo.System.bStripPcsRgnBin)
					{
						if (pDoc->m_nAoiCamInfoStrPcs[1] == 1 ? TRUE : FALSE)
							pView->MsgBox(_T("���� ��ŷ�δ� �Ϲ� ��� �ε�, \r\n�ϸ� AOI�� DTS ��忡�� �˻縦 �����Ͽ����ϴ�."));
						else
							pView->MsgBox(_T("���� ��ŷ�δ� DTS ��� �ε�, \r\n�ϸ� AOI�� �Ϲ� ��忡�� �˻縦 �����Ͽ����ϴ�."));

						Stop();
						TowerLamp(RGB_RED, TRUE);
						break;
					}
				}
			}

			if (nNewLot)
			{
				if (!pDoc->m_bNewLotShare[1])
				{
					pDoc->m_bNewLotShare[1] = TRUE;// Lot Change.				
					if (bDualTest)
						OpenReelmapFromBuf(m_nShareDnS);
				}
			}

			LoadPcrDn(m_nShareDnS);

			if (bDualTest)
			{
				if (m_nShareDnS != m_nShareDnSprev)
				{
					m_nShareDnSprev = m_nShareDnS;
					UpdateReelmap(m_nShareDnS);  // �ø��������� ������ ������ ����  // After inspect bottom side.
				}
			}


			if (!m_bLastProc)
			{
				//if (!IsSetLotEnd())
				//{
				//	if (ChkLotEndDn(m_nShareDnS))// ������ ���� �߿� Lot End (-2) �ܷ�ó���� üũ��. (���� 3Pnl:-2) -> ��Ʈ�Ϸ� 
				//	{
				//		if (!IsSetLotEnd())
				//			SetLotEnd(m_nShareDnS - pDoc->AoiDummyShot[1]);
				//		if (m_nAoiLastSerial[0] < 1)
				//			m_nAoiLastSerial[0] = m_nShareDnS;
				//		if (bDualTest)
				//		{
				//			m_bLastProc = TRUE;
				//			m_nLastProcAuto = LAST_PROC;
				//		}
				//	}
				//}

				if (ChkLastProc())
				{
					m_nLastProcAuto = LAST_PROC;
					m_bLastProc = TRUE;

					//if (IsVs())
					//{
					//	if (m_nAoiLastSerial[0] < 1)
					//		m_nAoiLastSerial[0] = m_nShareDnS;

					//	m_nPrevStepAuto = m_nStepAuto;
					//	m_nStepAuto = LAST_PROC_VS_ALL;		 // �ܷ�ó�� 3
					//	break;
					//}
					//else
					{
						if (ChkLastProcFromEng())
						{
							if (IsSetLotEnd())
								nSerial = GetLotEndSerial();
							else
								nSerial = pDoc->GetCurrentInfoEngShotNum();
						}
						else if (ChkLastProcFromUp())
						{
							if (IsSetLotEnd())
								nSerial = GetLotEndSerial();
							else
								nSerial = pDoc->m_ListBuf[0].GetLast();
						}
						else
						{
							if (IsSetLotEnd())
								nSerial = GetLotEndSerial();
							else
								nSerial = pDoc->m_ListBuf[1].GetLast();
						}

						if (!IsSetLotEnd()) // 20160810
						{
							SetLotEnd(nSerial);//+pDoc->AoiDummyShot[1]); // 3
						//if (m_nAoiLastSerial[0] < 1)
						//m_nAoiLastSerial[0] = nSerial;
						}
					}
				}
			}
			else
			{
				if (ChkLastProcFromEng())
				{
					if (IsSetLotEnd())
						nSerial = GetLotEndSerial();
					else
						nSerial = pDoc->GetCurrentInfoEngShotNum();

					//if (m_nLotEndSerial != nSerial)
					if (!IsSetLotEnd())
					{
						SetLotEnd(nSerial);
						//if (m_nAoiLastSerial[0] < 1)
						//m_nAoiLastSerial[0] = nSerial;
					}
				}
			}
		}
		break;

	case AT_LP + 6:
		m_nStepAuto++;
		if (m_nShareUpS > 0)
		{
			if (pView->m_pDlgFrameHigh)
				pView->m_pDlgFrameHigh->SetAoiLastShot(0, m_nShareUpS);
		}
		if (bDualTest)
		{
			if (m_nShareDnS > 0)
			{
				//if (m_bChkLastProcVs)
				//{
				//	if (m_nShareDnS > m_nAoiLastSerial[0] && m_nAoiLastSerial[0] > 0)
				//		break;
				//}
				//else
				{
					if (IsSetLotEnd())
					{
						//if (m_nShareDnS > m_nAoiLastSerial[0] && m_nAoiLastSerial[0] > 0)
						if (m_nShareDnS > GetLotEndSerial())
							break;
					}
				}

				if (pView->m_pDlgFrameHigh)
					pView->m_pDlgFrameHigh->SetAoiLastShot(1, m_nShareDnS);
			}
		}
		break;

	case AT_LP + 7:
		if (IsRun())
		{
			//if (pDoc->GetTestMode() == MODE_OUTER)
			//{
			//	if (m_bTHREAD_UPDATE_REELMAP_INNER_UP) // Write Reelmap
			//		break;

			//	if (pDoc->WorkingInfo.LastJob.bDualTestInner)
			//	{
			//		if (m_bTHREAD_UPDATE_REELMAP_INNER_DN || m_bTHREAD_UPDATE_REELMAP_INNER_ALLUP || m_bTHREAD_UPDATE_REELMAP_INNER_ALLDN) // Write Reelmap
			//			break;
			//	}
			//}

			if (bDualTest)
			{
				if (m_bTHREAD_UPDATE_REELMAP_UP || m_bTHREAD_UPDATE_REELMAP_DN || m_bTHREAD_UPDATE_REELMAP_ALLUP || m_bTHREAD_UPDATE_REELMAP_ALLDN) // Write Reelmap
					break;
			}
			else
			{
				if (m_bTHREAD_UPDATE_REELMAP_UP) // Write Reelmap
					break;
			}
			//SetListBuf(); // 20170727-�ܷ�ó�� �� ��������� �ݺ��ؼ� ���Լ��� ȣ������� �¿� ��ŷ �ε��� ���� ���� �߻�.(case AT_LP + 8:)
			m_nStepAuto++;
		}
		break;


	case AT_LP + 8:
		if (IsRun())
		{
			if (pDoc->GetTestMode() == MODE_INNER || pDoc->GetTestMode() == MODE_OUTER)
			{
				if (!bDualTest)
				{
					if (m_nShareUpS > 0)
						UpdateReelmapInner(m_nShareUpS);
				}
				else
				{
					if (m_nShareDnS > 0)
						UpdateReelmapInner(m_nShareDnS);
				}
			}

			m_nStepAuto++;
		}
		break;

	case AT_LP + 9:
		if (pDoc->GetTestMode() == MODE_INNER || pDoc->GetTestMode() == MODE_OUTER)
		{
			if (m_bTHREAD_UPDATE_REELMAP_INNER_UP) // Write Reelmap
				break;

			if (pDoc->GetTestMode() == MODE_OUTER)
			{
				if (pDoc->WorkingInfo.LastJob.bDualTestInner)
				{
					if (m_bTHREAD_UPDATE_REELMAP_INNER_DN || m_bTHREAD_UPDATE_REELMAP_INNER_ALLUP || m_bTHREAD_UPDATE_REELMAP_INNER_ALLDN) // Write Reelmap
						break;
				}
			}
			else
			{
				if (pDoc->WorkingInfo.LastJob.bDualTest)
				{
					if (m_bTHREAD_UPDATE_REELMAP_INNER_DN || m_bTHREAD_UPDATE_REELMAP_INNER_ALLUP || m_bTHREAD_UPDATE_REELMAP_INNER_ALLDN) // Write Reelmap
						break;
				}
			}
		}
		m_nStepAuto++;
		break;

	case AT_LP + 10:
		m_nShareUpS = 0;
		m_nShareDnS = 0;
		m_nStepAuto++;
		break;

	case AT_LP + 11:
		m_bLoadShare[0] = FALSE;
		m_bLoadShare[1] = FALSE;
		m_nStepAuto = AT_LP;
		break;


	case LAST_PROC_VS_ALL:			 // �ܷ�ó�� 3
		m_nDummy[0] = 0;
		m_nDummy[1] = 0;
		m_bChkLastProcVs = TRUE;
		TowerLamp(RGB_GREEN, TRUE);
		DispMain(_T("���VS�ܷ�"), RGB_GREEN);
		if (m_nAoiLastSerial[0] < 1)
			m_nAoiLastSerial[0] = GetAoiUpSerial();
		if (!IsSetLotEnd())
			SetLotEnd(m_nAoiLastSerial[0]);
		//m_nAoiLastSerial[1] = GetAoiDnSerial();
		m_nStepAuto++;
		break;

	case LAST_PROC_VS_ALL + 1:
		if (IsVsUp())
			m_nStepAuto++;
		else
			m_nStepAuto = m_nPrevStepAuto;
		break;

	case LAST_PROC_VS_ALL + 2:
		SetDummyUp();
		m_nStepAuto++;
		break;

	case LAST_PROC_VS_ALL + 3:
		SetDummyUp();
		m_nStepAuto++;
		break;

	case LAST_PROC_VS_ALL + 4:
		SetDummyUp();
		m_nStepAuto++;
		break;

	case LAST_PROC_VS_ALL + 5:
		m_nStepAuto = m_nPrevStepAuto;
		break;
	}
}

void CGvisR2R_PunchView::DoAutoMarking()
{
	if(pDoc->WorkingInfo.LastJob.nAlignMethode == TWO_POINT)
		MarkingWith2PointAlign();
	else if(pDoc->WorkingInfo.LastJob.nAlignMethode == FOUR_POINT)
		MarkingWith4PointAlign();
	else
	{
		pView->ClrDispMsg();
		AfxMessageBox(_T("��ŷ�� ���� Align����� �������� �ʾҽ��ϴ�."));
	}
}

void CGvisR2R_PunchView::Mk2PtReady()
{
	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;
	CString sMsg;

	if (m_bMkSt)
	{
		switch (m_nMkStAuto)
		{
		case MK_ST:	// PLC MK ��ȣ Ȯ��	
			if (IsRun())
			{
				pDoc->LogAuto(_T("PC: ��ŷ�� ��ŷ�� ON (PC�� ON, OFF)"));
				m_pMpe->Write(_T("MB440150"), 1);// ��ŷ�� ��ŷ�� ON (PC�� ON, OFF)
#ifdef USE_SR1000W
				if (pDoc->GetTestMode() == MODE_INNER || pDoc->GetTestMode() == MODE_OUTER)
				{
					if (m_pSr1000w && m_pSr1000w->IsConnected())
					{
						m_sGet2dCodeLot = _T("");
						m_nGet2dCodeSerial = 0;
						//Set2dRead(TRUE);
					}
					else
					{
						Stop();
						pView->ClrDispMsg();
						sMsg.Format(_T("2D �����Ⱑ ������ ���� �ʾҽ��ϴ�."));
						AfxMessageBox(sMsg);
						m_nMkStAuto = MK_ST;
						break;
					}
				}
#endif
				m_nMkStAuto++;
			}
			break;
		case MK_ST + 1:
			if (!m_bTHREAD_SHIFT2MK)
			{
				SetListBuf();
				m_nMkStAuto = MK_ST + (Mk2PtIdx::Start);
			}
			break;
		case MK_ST + (Mk2PtIdx::Start) :	// 2
			if (bDualTest)
			{
				if (pDoc->m_ListBuf[1].nTot > 0) // AOI-Dn
				{
					m_nMkStAuto++;

					m_nBufDnSerial[0] = pDoc->m_ListBuf[1].Pop();
					m_nBufUpSerial[0] = m_nBufDnSerial[0];
					if (pDoc->m_ListBuf[1].nTot > 0) // AOI-Dn
					{
						m_nBufDnSerial[1] = pDoc->m_ListBuf[1].Pop();
						m_nBufUpSerial[1] = m_nBufDnSerial[1];
					}
					else
					{
						m_nBufDnSerial[1] = 0;
						m_nBufUpSerial[1] = 0;
					}
				}
				else
				{
					m_bLotEnd = TRUE;
					m_nLotEndAuto = LOT_END;
				}

				if (pDoc->WorkingInfo.LastJob.bSampleTest)
				{
					if (m_nBufUpSerial[0] == 1)
					{
						m_nLotEndSerial = _tstoi(pDoc->WorkingInfo.LastJob.sSampleTestShotNum);
						m_bLastProcFromUp = FALSE;
						m_bLastProcFromEng = FALSE;
						m_bLastProc = TRUE;
						if (m_pDlgMenu01)
							m_pDlgMenu01->m_bLastProc = TRUE;
						if (m_pMpe)
						{
							m_pMpe->Write(_T("MB440186"), 1);			// �ܷ�ó�� AOI(��) ����(PC�� On��Ű��, PLC�� Ȯ���ϰ� Off��Ŵ)-20141112
							m_pMpe->Write(_T("MB440181"), 1);			// �ܷ�ó��(PC�� On��Ű��, PLC�� Ȯ���ϰ� Off��Ŵ)-20141031
							pDoc->LogAuto(_T("PC: �ܷ�ó�� AOI(��) ����(PC�� On��Ű��, PLC�� Ȯ���ϰ� Off��Ŵ)"));
						}
					}
				}
			}
			else
			{
				if (pDoc->m_ListBuf[0].nTot > 0) // AOI-Up
				{
					m_nMkStAuto++;
					m_nBufUpSerial[0] = pDoc->m_ListBuf[0].Pop();
					if (pDoc->m_ListBuf[0].nTot > 0) // AOI-Up
						m_nBufUpSerial[1] = pDoc->m_ListBuf[0].Pop();
					else
						m_nBufUpSerial[1] = 0;
				}
				else
				{
					m_bLotEnd = TRUE;
					m_nLotEndAuto = LOT_END;
				}

				if (pDoc->WorkingInfo.LastJob.bSampleTest)
				{
					if (m_nBufUpSerial[0] == 1)
					{
						m_nLotEndSerial = _tstoi(pDoc->WorkingInfo.LastJob.sSampleTestShotNum);
						m_bLastProcFromUp = FALSE;
						m_bLastProcFromEng = FALSE;
						m_bLastProc = TRUE;
						if (m_pDlgMenu01)
							m_pDlgMenu01->m_bLastProc = TRUE;
						if (m_pMpe)
						{
							m_pMpe->Write(_T("MB440186"), 1);			// �ܷ�ó�� AOI(��) ����(PC�� On��Ű��, PLC�� Ȯ���ϰ� Off��Ŵ)-20141112
							m_pMpe->Write(_T("MB440181"), 1);			// �ܷ�ó��(PC�� On��Ű��, PLC�� Ȯ���ϰ� Off��Ŵ)-20141031
							pDoc->LogAuto(_T("PC: �ܷ�ó�� AOI(��) ����(PC�� On��Ű��, PLC�� Ȯ���ϰ� Off��Ŵ)"));
						}
					}
				}
			}
			break;
		case MK_ST + (Mk2PtIdx::Start) + 1:
			if (pDoc->GetTestMode() == MODE_OUTER)
			{
				if (!m_bTHREAD_UPDATE_REELMAP_UP && !m_bTHREAD_UPDATE_REELMAP_DN && !m_bTHREAD_UPDATE_REELMAP_ALLUP && !m_bTHREAD_UPDATE_REELMAP_ALLDN
					&& !m_bTHREAD_UPDATE_REELMAP_INNER_UP && !m_bTHREAD_UPDATE_REELMAP_INNER_DN && !m_bTHREAD_UPDATE_REELMAP_INNER_ALLUP && !m_bTHREAD_UPDATE_REELMAP_INNER_ALLDN) // Write Reelmap
					m_nMkStAuto = MK_ST + (Mk2PtIdx::ChkSn);
			}
			else
			{
				if (!m_bTHREAD_UPDATE_REELMAP_UP && !m_bTHREAD_UPDATE_REELMAP_DN && !m_bTHREAD_UPDATE_REELMAP_ALLUP && !m_bTHREAD_UPDATE_REELMAP_ALLDN) // Write Reelmap
					m_nMkStAuto = MK_ST + (Mk2PtIdx::ChkSn);
			}
			break;
		}
	}
}

void CGvisR2R_PunchView::Mk2PtChkSerial()
{
	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;
	CString sNewLot;
	BOOL bNewModel = FALSE;
	int nSerial = 0;
	int nNewLot = 0;
	int nLastShot = 0;
	double dFdEnc;
	CString sLot, sMsg;

	if (m_bTHREAD_SHIFT2MK)
		return;

	if (m_bMkSt)
	{
		switch (m_nMkStAuto)
		{
		case MK_ST + (Mk2PtIdx::ChkSn) :
			if (m_nBufUpSerial[0] == m_nBufUpSerial[1])
			{
				Stop();
				pView->ClrDispMsg();
				AfxMessageBox(_T("��/�� ��ŷ �ø����� �����ϴ�."));
				SetListBuf();
				m_nMkStAuto = MK_ST + (Mk2PtIdx::Start);
				break;
			}

			if (pDoc->GetTestMode() == MODE_OUTER)
			{
				if (!m_bTHREAD_UPDATE_REELMAP_UP && !m_bTHREAD_UPDATE_REELMAP_DN && !m_bTHREAD_UPDATE_REELMAP_ALLUP && !m_bTHREAD_UPDATE_REELMAP_ALLDN
					&& !m_bTHREAD_UPDATE_REELMAP_INNER_UP && !m_bTHREAD_UPDATE_REELMAP_INNER_DN && !m_bTHREAD_UPDATE_REELMAP_INNER_ALLUP && !m_bTHREAD_UPDATE_REELMAP_INNER_ALLDN) // Write Reelmap
				{
					if (!m_bTHREAD_DISP_DEF && !m_bTHREAD_DISP_DEF_INNER)
					{
						m_nMkStAuto++;
						m_nStepTHREAD_DISP_DEF = 0;
						m_nStepTHREAD_DISP_DEF_INNER = 0;

						m_bTHREAD_DISP_DEF = TRUE;				// DispDefImg() : CopyDefImg Start -> Disp Reelmap Start
						m_bTHREAD_DISP_DEF_INNER = TRUE;		// DispDefImgInner() : Disp Reelmap Start

						pDoc->SetMkMenu01(_T("Signal"), _T("DispDefImg"), _T("1"));
					}
				}
			}
			else
			{
				if (!m_bTHREAD_UPDATE_REELMAP_UP && !m_bTHREAD_UPDATE_REELMAP_DN && !m_bTHREAD_UPDATE_REELMAP_ALLUP && !m_bTHREAD_UPDATE_REELMAP_ALLDN) // Write Reelmap
				{
					if (!m_bTHREAD_DISP_DEF)
					{
						m_nMkStAuto++;
						m_nStepTHREAD_DISP_DEF = 0;

						m_bTHREAD_DISP_DEF = TRUE;		// DispDefImg() : CopyDefImg Start -> Disp Reelmap Start
						pDoc->SetMkMenu01(_T("Signal"), _T("DispDefImg"), _T("1"));
					}
				}
			}

			break;

		case MK_ST + (Mk2PtIdx::ChkSn) + 1:
			m_nMkStAuto++;
			nSerial = m_nBufUpSerial[0];
			sNewLot = m_sNewLotUp;

			if (nSerial > 0)
			{
				if (pView->m_bSerialDecrese)
				{
					if (m_bLastProc && m_nBufUpSerial[1] < m_nLotEndSerial)
						nLastShot = m_nBufUpSerial[0];
				}
				else
				{
					if (m_bLastProc && m_nBufUpSerial[1] > m_nLotEndSerial)
					{
						nLastShot = m_nBufUpSerial[0];
						//nSerial = m_nBufUpSerial[0];
					}
				}

				bNewModel = GetAoiUpInfo(nSerial, &nNewLot, TRUE);
				if (bDualTest)
				{
					bNewModel = GetAoiDnInfo(nSerial, &nNewLot, TRUE);

					if (!IsSameUpDnLot() && !m_bContDiffLot)
					{
						m_nMkStAuto = MK_ST + (Mk2PtIdx::LotDiff);
						break;
					}
				}

				if (bNewModel)	// AOI ����(AoiCurrentInfoPath) -> AOI Feeding Offset
				{
					;
				}
				if (nNewLot)
				{
					// Lot Change.
					ChgLot();
#ifdef USE_MPE
					dFdEnc = (double)pDoc->m_pMpeData[0][0];	// ��ŷ�� Feeding ���ڴ� ��(���� mm )
					if ((pDoc->WorkingInfo.LastJob.bLotSep || pDoc->m_bDoneChgLot) && (dFdEnc + _tstof(pDoc->WorkingInfo.LastJob.sOnePnlLen)*2.0) > _tstof(pDoc->WorkingInfo.LastJob.sLotSepLen)*1000.0)
					{
						pDoc->m_bDoneChgLot = TRUE;
						SetPathAtBuf();
					}
#endif
				}
			}
			else
			{
				Stop();
				MsgBox(_T("����(��) Serial�� �����ʽ��ϴ�."));
				TowerLamp(RGB_YELLOW, TRUE);
			}
			//sNewLot.Empty();
			break;

		case MK_ST + (Mk2PtIdx::ChkSn) + 2:
			//pDoc->UpdateYieldOnRmap(); // 20230614

#ifdef USE_SR1000W
			if (pDoc->GetTestMode() == MODE_INNER || pDoc->GetTestMode() == MODE_OUTER)
			{
				if (m_pSr1000w && m_pSr1000w->IsConnected())
				{
					//Set2dRead(TRUE);
					//Get2dCode(sLot, nSerial);
					if (m_sGet2dCodeLot != _T("") && m_nGet2dCodeSerial != 0)
					{
						if (m_nBufUpSerial[0] == m_nGet2dCodeSerial)
						{
							m_nMkStAuto = MK_ST + (Mk2PtIdx::InitMk);			// InitMk()
						}
						else
						{
							Stop();
							pView->ClrDispMsg();
							sMsg.Format(_T("2D���� �ø���(%d)�� �˻����� �ø���(%d)�� �ٸ��ϴ�."), m_nGet2dCodeSerial, m_nBufUpSerial[0]);
							AfxMessageBox(sMsg);
							m_nMkStAuto = MK_ST + (Mk2PtIdx::Start);
							break;
						}
					}
					else
						m_nMkStAuto = MK_ST + (Mk2PtIdx::InitMk);			// InitMk()

				}
				else
				{
					Stop();
					pView->ClrDispMsg();
					sMsg.Format(_T("2D �����Ⱑ ������ ���� �ʾҽ��ϴ�."));
					AfxMessageBox(sMsg);
					m_nMkStAuto = MK_ST + (Mk2PtIdx::Start);
					break;
				}
			}
			else
				m_nMkStAuto = MK_ST + (Mk2PtIdx::InitMk);					// InitMk()
#else
			m_nMkStAuto = MK_ST + (Mk2PtIdx::InitMk);					// InitMk()
#endif
			break;
		}
	}
}

void CGvisR2R_PunchView::Mk2PtInit()
{
	if (m_bTHREAD_SHIFT2MK)
		return;

	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;

	if (m_bMkSt)
	{
		switch (m_nMkStAuto)
		{
		case MK_ST + (Mk2PtIdx::InitMk) :
			if (InitMk()) // �ø����� ���� �Ǵ� ���� : TRUE �ƴϰ� ������ : FALSE
			{
				if (pView->m_bSerialDecrese)
				{
					if ((m_nBufUpSerial[0] <= m_nLotEndSerial || m_nBufUpSerial[1] <= m_nLotEndSerial) && m_nLotEndSerial > 0)
					{
						m_pMpe->Write(_T("MB440171"), 1); // ��ŷ�� �۾��Ϸ�.(PC�� On, PLC�� Ȯ�� �� Off) - 20160718
						pDoc->LogAuto(_T("PC: ��ŷ�� �۾��Ϸ�.(PC�� On, PLC�� Ȯ�� �� Off)"));
					}
				}
				else
				{
					if ((m_nBufUpSerial[0] >= m_nLotEndSerial || m_nBufUpSerial[1] >= m_nLotEndSerial) && m_nLotEndSerial > 0)
					{
						m_pMpe->Write(_T("MB440171"), 1); // ��ŷ�� �۾��Ϸ�.(PC�� On, PLC�� Ȯ�� �� Off) - 20160718
						pDoc->LogAuto(_T("PC: ��ŷ�� �۾��Ϸ�.(PC�� On, PLC�� Ȯ�� �� Off)"));
					}
				}
			}
			else // Same Serial
			{
				Stop();
				MsgBox(_T("Serial ���� �����ʽ��ϴ�."));
				TowerLamp(RGB_YELLOW, TRUE);
			}
			m_nMkStAuto++;
			break;

		case MK_ST + (Mk2PtIdx::InitMk) + 1:
			if (IsRun())
			{
				m_nMkStAuto = MK_ST + (Mk2PtIdx::Move0Cam1);	// Move - Cam1 - Pt0
				//if (MODE_INNER != pDoc->GetTestMode())
				//	m_nMkStAuto = MK_ST + (Mk2PtIdx::Move0Cam1);	// Move - Cam1 - Pt0
				//else
				//	m_nMkStAuto = MK_ST + (Mk2PtIdx::MoveInitPt);
			}
			break;
		}
	}
}

void CGvisR2R_PunchView::Mk2PtAlignPt0()
{
	if (!IsRun())
		return;

	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;

	if (m_bMkSt)
	{
		switch (m_nMkStAuto)
		{
		case MK_ST + (Mk2PtIdx::Move0Cam1) :	// Move - Cam1 - Pt0
			if (bDualTest)
			{
				if (pView->m_bSerialDecrese)
				{
					if (m_bLastProc && m_nBufDnSerial[1] < m_nLotEndSerial)	// AOI�ϸ� Serial
					{
						m_bSkipAlign[1][0] = TRUE;
						m_bSkipAlign[1][1] = TRUE;
						m_bSkipAlign[1][2] = TRUE;
						m_bSkipAlign[1][3] = TRUE;
						m_bDoMk[1] = FALSE;
						m_bDoneMk[1] = TRUE;
						m_nMkStAuto++;
					}
					else if (m_nBufDnSerial[1] == 0)
					{
						m_bSkipAlign[1][0] = TRUE;
						m_bSkipAlign[1][1] = TRUE;
						m_bSkipAlign[1][2] = TRUE;
						m_bSkipAlign[1][3] = TRUE;
						m_bDoMk[1] = FALSE;
						m_bDoneMk[1] = TRUE;
						m_nMkStAuto++;
					}
					else
					{
						if (MoveAlign1(0))	// Move - Cam1 - Pt0
							m_nMkStAuto++;
					}
				}
				else
				{
					if (m_bLastProc && m_nBufDnSerial[1] > m_nLotEndSerial)	// AOI�ϸ� Serial
					{
						m_bSkipAlign[1][0] = TRUE;
						m_bSkipAlign[1][1] = TRUE;
						m_bSkipAlign[1][2] = TRUE;
						m_bSkipAlign[1][3] = TRUE;
						m_bDoMk[1] = FALSE;
						m_bDoneMk[1] = TRUE;
						m_nMkStAuto++;
					}
					else if (m_nBufDnSerial[1] == 0)
					{
						m_bSkipAlign[1][0] = TRUE;
						m_bSkipAlign[1][1] = TRUE;
						m_bSkipAlign[1][2] = TRUE;
						m_bSkipAlign[1][3] = TRUE;
						m_bDoMk[1] = FALSE;
						m_bDoneMk[1] = TRUE;
						m_nMkStAuto++;
					}
					else
					{
						if (MoveAlign1(0))	// Move - Cam1 - Pt0
							m_nMkStAuto++;
					}
				}
			}
			else
			{
				if (pView->m_bSerialDecrese)
				{
					if (m_bLastProc && m_nBufUpSerial[1] < m_nLotEndSerial)	// AOI��� Serial
					{
						m_bSkipAlign[1][0] = TRUE;
						m_bSkipAlign[1][1] = TRUE;
						m_bSkipAlign[1][2] = TRUE;
						m_bSkipAlign[1][3] = TRUE;
						m_bDoMk[1] = FALSE;
						m_bDoneMk[1] = TRUE;
						m_nMkStAuto++;
					}
					else if (m_nBufUpSerial[1] == 0)
					{
						m_bSkipAlign[1][0] = TRUE;
						m_bSkipAlign[1][1] = TRUE;
						m_bSkipAlign[1][2] = TRUE;
						m_bSkipAlign[1][3] = TRUE;
						m_bDoMk[1] = FALSE;
						m_bDoneMk[1] = TRUE;
						m_nMkStAuto++;
					}
					else
					{
						if (MoveAlign1(0)) 	// Move - Cam1 - Pt0
							m_nMkStAuto++;
					}
				}
				else
				{
					if (m_bLastProc && m_nBufUpSerial[1] > m_nLotEndSerial)	// AOI��� Serial
					{
						m_bSkipAlign[1][0] = TRUE;
						m_bSkipAlign[1][1] = TRUE;
						m_bSkipAlign[1][2] = TRUE;
						m_bSkipAlign[1][3] = TRUE;
						m_bDoMk[1] = FALSE;
						m_bDoneMk[1] = TRUE;
						m_nMkStAuto++;
					}
					else if (m_nBufUpSerial[1] == 0)
					{
						m_bSkipAlign[1][0] = TRUE;
						m_bSkipAlign[1][1] = TRUE;
						m_bSkipAlign[1][2] = TRUE;
						m_bSkipAlign[1][3] = TRUE;
						m_bDoMk[1] = FALSE;
						m_bDoneMk[1] = TRUE;
						m_nMkStAuto++;
					}
					else
					{
						if (MoveAlign1(0)) 	// Move - Cam1 - Pt0
							m_nMkStAuto++;
					}
				}
			}
			break;
		case MK_ST + (Mk2PtIdx::Move0Cam1) + 1:
			if (IsRun())
				m_nMkStAuto = MK_ST + (Mk2PtIdx::Move0Cam0);
			break;
		case MK_ST + (Mk2PtIdx::Move0Cam0) :	// Move - Cam0 - Pt0
			if (MoveAlign0(0))
				m_nMkStAuto++;
			break;
		case MK_ST + (Mk2PtIdx::Move0Cam0) + 1:
			if (IsRun())
				m_nMkStAuto++;
			break;
		case MK_ST + (Mk2PtIdx::Move0Cam0) + 2:
			if (IsMoveDone())
			{
				Sleep(100);
				m_nMkStAuto = MK_ST + (Mk2PtIdx::Align1_0);
			}
			break;
		case MK_ST + (Mk2PtIdx::Align1_0) :	// 2PtAlign - Cam1 - Pt0
			if (!m_bSkipAlign[1][0])
			{
				if (TwoPointAlign1(0))
					m_bFailAlign[1][0] = FALSE;
				else
					m_bFailAlign[1][0] = TRUE;
			}
			m_nMkStAuto = MK_ST + (Mk2PtIdx::Align0_0);
			break;
		case MK_ST + (Mk2PtIdx::Align0_0) :	// 2PtAlign - Cam0 - Pt0
			if (!m_bSkipAlign[0][0])
			{
				if (TwoPointAlign0(0))
					m_bFailAlign[0][0] = FALSE;
				else
					m_bFailAlign[0][0] = TRUE;
			}
			m_nMkStAuto++;
			break;
		case MK_ST + (Mk2PtIdx::Align0_0) + 1:
			if (m_bFailAlign[0][0])
			{
				Buzzer(TRUE, 0);
				TowerLamp(RGB_YELLOW, TRUE);
				if (IDNO == MsgBox(_T("ī�޶�(��)�� �˻��ǳ��� �ٽ� �����Ͻðڽ��ϱ�?"), 0, MB_YESNO))
				{
					Buzzer(FALSE, 0);

					if (IDYES == MsgBox(_T("ī�޶�(��)�� �˻��ǳ� ������ ���� ���Ͻðڽ��ϱ�?"), 0, MB_YESNO))
					{
						m_bReAlign[0][0] = FALSE;
						m_bSkipAlign[0][0] = TRUE;
						m_bSkipAlign[0][1] = TRUE;
						m_bSkipAlign[0][2] = TRUE;
						m_bSkipAlign[0][3] = TRUE;
						if (IDNO == MsgBox(_T("ī�޶�(��)�� �˻��ǳ��� �ҷ���ŷ �Ͻðڽ��ϱ�?"), 0, MB_YESNO))
						{
							m_bDoMk[0] = FALSE;
							m_bDoneMk[0] = TRUE;
						}
						else
						{
							m_bDoMk[0] = TRUE;
							m_bDoneMk[0] = FALSE;
						}
					}
					else
					{
						m_bReAlign[0][0] = TRUE;
						m_bSkipAlign[0][0] = FALSE;
						m_bSkipAlign[0][1] = FALSE;
						m_bSkipAlign[0][2] = FALSE;
						m_bSkipAlign[0][3] = FALSE;
						m_nMkStAuto = MK_ST + (Mk2PtIdx::Move0Cam0); // TwoPointAlign0(0) ���� ����. - ī�޶� ������
						Stop();
						//TowerLamp(RGB_YELLOW, TRUE);
					}
				}
				else
				{
					Buzzer(FALSE, 0);

					m_bReAlign[0][0] = TRUE;
					m_bSkipAlign[0][0] = FALSE;
					m_bSkipAlign[0][1] = FALSE;
					m_nMkStAuto = MK_ST + (Mk2PtIdx::Move0Cam0); // TwoPointAlign0(0) ���� ����. - ī�޶� ������
					Stop();
					//TowerLamp(RGB_YELLOW, TRUE);
				}
			}
			if (m_bFailAlign[1][0])
			{
				Buzzer(TRUE, 0);

				if (IDNO == MsgBox(_T("ī�޶�(��)�� �˻��ǳ��� �ٽ� �����Ͻðڽ��ϱ�?"), 0, MB_YESNO))
				{
					Buzzer(FALSE, 0);

					if (IDYES == MsgBox(_T("ī�޶�(��)�� �˻��ǳ� ������ ���� ���Ͻðڽ��ϱ�?"), 0, MB_YESNO))
					{
						m_bReAlign[1][0] = FALSE;
						m_bSkipAlign[1][0] = TRUE;
						m_bSkipAlign[1][1] = TRUE;
						m_bSkipAlign[1][2] = TRUE;
						m_bSkipAlign[1][3] = TRUE;
						if (IDNO == MsgBox(_T("ī�޶�(��)�� �˻��ǳ��� �ҷ���ŷ �Ͻðڽ��ϱ�?"), 0, MB_YESNO))
						{
							m_bDoMk[1] = FALSE;
							m_bDoneMk[1] = TRUE;
						}
						else
						{
							m_bDoMk[1] = TRUE;
							m_bDoneMk[1] = FALSE;
						}
					}
					else
					{
						m_bReAlign[1][0] = TRUE;
						m_bSkipAlign[1][0] = FALSE;
						m_bSkipAlign[1][1] = FALSE;
						m_bSkipAlign[1][2] = FALSE;
						m_bSkipAlign[1][3] = FALSE;
						m_nMkStAuto = MK_ST + (Mk2PtIdx::Move0Cam1); // TwoPointAlign1(0) ���� ����. - ī�޶� ������
						Stop();
						TowerLamp(RGB_YELLOW, TRUE);
					}
				}
				else
				{
					Buzzer(FALSE, 0);

					m_bReAlign[1][0] = TRUE;
					m_bSkipAlign[1][0] = FALSE;
					m_bSkipAlign[1][1] = FALSE;
					m_bSkipAlign[1][2] = FALSE;
					m_bSkipAlign[1][3] = FALSE;
					m_nMkStAuto = MK_ST + (Mk2PtIdx::Move0Cam1); // TwoPointAlign1(0) ���� ����. - ī�޶� ������
					Stop();
					TowerLamp(RGB_YELLOW, TRUE);
				}
			}

			if (m_bFailAlign[0][0] || m_bFailAlign[1][0])
			{
				if (!m_bReAlign[0][0] && !m_bReAlign[1][0])
				{
					if (m_bDoMk[0] || m_bDoMk[1])
						m_nMkStAuto++; //m_nMkStAuto = MK_ST + 27; // MoveInitPos0()
					else
					{
						if (!IsInitPos0())
							MoveInitPos0();
						if (!IsInitPos1())
							MoveInitPos1();

						m_nMkStAuto = MK_ST + (Mk2PtIdx::DoneMk); // // �ҷ��̹��� Display, Align���� �ʱ�ȭ (Skip 65 : Mk())
					}
				}
				else
				{
					m_nMkStAuto = MK_ST + (Mk2PtIdx::Move0Cam1); // TwoPointAlign1(0) ���� ����. - ī�޶� ������
				}
			}
			else
				m_nMkStAuto++;

			break;
		case MK_ST + (Mk2PtIdx::Align0_0) + 2:
			if (IsRun())
				m_nMkStAuto = MK_ST + (Mk2PtIdx::Move1Cam1);
			break;
		}
	}
}

void CGvisR2R_PunchView::Mk2PtAlignPt1()
{
	if (!IsRun())
		return;

	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;

	if (m_bMkSt)
	{
		switch (m_nMkStAuto)
		{
		case MK_ST + (Mk2PtIdx::Move1Cam1) :
			if (bDualTest)
			{
				if (pView->m_bSerialDecrese)
				{
					if (m_bLastProc && m_nBufDnSerial[1] < m_nLotEndSerial)	// AOI�ϸ� Serial
					{
						m_bSkipAlign[1][0] = TRUE;
						m_bSkipAlign[1][1] = TRUE;
						m_bSkipAlign[1][2] = TRUE;
						m_bSkipAlign[1][3] = TRUE;
						m_bDoMk[1] = FALSE;
						m_bDoneMk[1] = TRUE;
						m_nMkStAuto++;
					}
					else if (m_nBufDnSerial[1] == 0)
					{
						m_bSkipAlign[1][0] = TRUE;
						m_bSkipAlign[1][1] = TRUE;
						m_bSkipAlign[1][2] = TRUE;
						m_bSkipAlign[1][3] = TRUE;
						m_bDoMk[1] = FALSE;
						m_bDoneMk[1] = TRUE;
						m_nMkStAuto++;
					}
					else
					{
						if (MoveAlign1(1))	// Move - Cam1 - Pt1
							m_nMkStAuto++;
					}
				}
				else
				{
					if (m_bLastProc && m_nBufDnSerial[1] > m_nLotEndSerial)	// AOI�ϸ� Serial
					{
						m_bSkipAlign[1][0] = TRUE;
						m_bSkipAlign[1][1] = TRUE;
						m_bSkipAlign[1][2] = TRUE;
						m_bSkipAlign[1][3] = TRUE;
						m_bDoMk[1] = FALSE;
						m_bDoneMk[1] = TRUE;
						m_nMkStAuto++;
					}
					else if (m_nBufDnSerial[1] == 0)
					{
						m_bSkipAlign[1][0] = TRUE;
						m_bSkipAlign[1][1] = TRUE;
						m_bSkipAlign[1][2] = TRUE;
						m_bSkipAlign[1][3] = TRUE;
						m_bDoMk[1] = FALSE;
						m_bDoneMk[1] = TRUE;
						m_nMkStAuto++;
					}
					else
					{
						if (MoveAlign1(1))	// Move - Cam1 - Pt1
							m_nMkStAuto++;
					}
				}
			}
			else
			{
				if (pView->m_bSerialDecrese)
				{
					if (m_bLastProc && m_nBufUpSerial[1] < m_nLotEndSerial)	// AOI��� Serial
					{
						m_bSkipAlign[1][0] = TRUE;
						m_bSkipAlign[1][1] = TRUE;
						m_bSkipAlign[1][2] = TRUE;
						m_bSkipAlign[1][3] = TRUE;
						m_bDoMk[1] = FALSE;
						m_bDoneMk[1] = TRUE;
						m_nMkStAuto++;
					}
					else if (m_nBufUpSerial[1] == 0)
					{
						m_bSkipAlign[1][0] = TRUE;
						m_bSkipAlign[1][1] = TRUE;
						m_bSkipAlign[1][2] = TRUE;
						m_bSkipAlign[1][3] = TRUE;
						m_bDoMk[1] = FALSE;
						m_bDoneMk[1] = TRUE;
						m_nMkStAuto++;
					}
					else
					{
						if (MoveAlign1(1))	// Move - Cam1 - Pt1
							m_nMkStAuto++;
					}
				}
				else
				{
					if (m_bLastProc && m_nBufUpSerial[1] > m_nLotEndSerial)	// AOI��� Serial
					{
						m_bSkipAlign[1][0] = TRUE;
						m_bSkipAlign[1][1] = TRUE;
						m_bSkipAlign[1][2] = TRUE;
						m_bSkipAlign[1][3] = TRUE;
						m_bDoMk[1] = FALSE;
						m_bDoneMk[1] = TRUE;
						m_nMkStAuto++;
					}
					else if (m_nBufUpSerial[1] == 0)
					{
						m_bSkipAlign[1][0] = TRUE;
						m_bSkipAlign[1][1] = TRUE;
						m_bSkipAlign[1][2] = TRUE;
						m_bSkipAlign[1][3] = TRUE;
						m_bDoMk[1] = FALSE;
						m_bDoneMk[1] = TRUE;
						m_nMkStAuto++;
					}
					else
					{
						if (MoveAlign1(1))	// Move - Cam1 - Pt1
							m_nMkStAuto++;
					}
				}
			}
		   break;
		case MK_ST + (Mk2PtIdx::Move1Cam1) + 1:
			if (IsRun())
				m_nMkStAuto++;
			break;
		case MK_ST + (Mk2PtIdx::Move1Cam0) :
			if (MoveAlign0(1))	// Move - Cam0 - Pt1
				m_nMkStAuto++;
			break;
		case MK_ST + (Mk2PtIdx::Move1Cam0) + 1:
			if (IsRun())
				m_nMkStAuto++;
			break;
		case MK_ST + (Mk2PtIdx::Move1Cam0) + 2:
			if (IsMoveDone())
			{
				Sleep(100);
				m_nMkStAuto = MK_ST + (Mk2PtIdx::Align1_1);
			}
			break;
		case MK_ST + (Mk2PtIdx::Align1_1) :	// 2PtAlign - Cam1 - Pt1
			if (!m_bFailAlign[1][0])
			{
				if (!m_bSkipAlign[1][1])
				{
					if (!TwoPointAlign1(1))
						m_bFailAlign[1][1] = TRUE;
					else
						m_bFailAlign[1][1] = FALSE;
				}
				else
					m_bFailAlign[1][1] = FALSE;
			}
			else
				m_bFailAlign[1][1] = FALSE;

			m_nMkStAuto = MK_ST + (Mk2PtIdx::Align0_1);
			break;
		case MK_ST + (Mk2PtIdx::Align0_1) :	// 2PtAlign - Cam0 - Pt1
			if (!m_bFailAlign[0][0])
			{
				if (!m_bSkipAlign[0][1])
				{
					if (!TwoPointAlign0(1))
						m_bFailAlign[0][1] = TRUE;
					else
						m_bFailAlign[0][1] = FALSE;
				}
				else
					m_bFailAlign[0][1] = FALSE;
			}
			else
				m_bFailAlign[0][1] = FALSE;

			m_nMkStAuto++;
			break;
		case MK_ST + (Mk2PtIdx::Align0_1) + 1:
			if (m_bFailAlign[0][1])
			{
				Buzzer(TRUE, 0);
				TowerLamp(RGB_YELLOW, TRUE);

				if (IDNO == MsgBox(_T("ī�޶�(��)�� �˻��ǳ��� �ٽ� �����Ͻðڽ��ϱ�?"), 0, MB_YESNO))
				{
					Buzzer(FALSE, 0);

					if (IDYES == MsgBox(_T("ī�޶�(��)�� �˻��ǳ� ������ ���� ���Ͻðڽ��ϱ�?"), 0, MB_YESNO))
					{
						m_bReAlign[0][1] = FALSE;
						m_bSkipAlign[0][1] = TRUE;
						if (IDNO == MsgBox(_T("ī�޶�(��)�� �˻��ǳ��� �ҷ���ŷ �Ͻðڽ��ϱ�?"), 0, MB_YESNO))
						{
							m_bDoMk[0] = FALSE;
							m_bDoneMk[0] = TRUE;
						}
						else
						{
							m_bDoMk[0] = TRUE;
							m_bDoneMk[0] = FALSE;
						}
					}
					else
					{
						m_bReAlign[0][1] = TRUE;
						m_bSkipAlign[0][1] = FALSE;
						m_nMkStAuto = MK_ST + (Mk2PtIdx::Move1Cam0); // TwoPointAlign0(1) ���� ����. - ī�޶� ������
						Stop();
						//TowerLamp(RGB_YELLOW, TRUE);
					}
				}
				else
				{
					Buzzer(FALSE, 0);

					m_bReAlign[0][1] = TRUE;
					m_bSkipAlign[0][1] = FALSE;
					m_nMkStAuto = MK_ST + (Mk2PtIdx::Move1Cam0); // TwoPointAlign1(1) ���� ����. - ī�޶� ������
					Stop();
					//TowerLamp(RGB_YELLOW, TRUE);
				}
			}
			if (m_bFailAlign[1][1])
			{
				Buzzer(TRUE, 0);
				TowerLamp(RGB_YELLOW, TRUE);

				if (IDNO == MsgBox(_T("ī�޶�(��)�� �˻��ǳ��� �ٽ� �����Ͻðڽ��ϱ�?"), 0, MB_YESNO))
				{
					Buzzer(FALSE, 0);

					if (IDYES == MsgBox(_T("ī�޶�(��)�� �˻��ǳ� ������ ���� ���Ͻðڽ��ϱ�?"), 0, MB_YESNO))
					{
						m_bReAlign[1][1] = FALSE;
						m_bSkipAlign[1][1] = TRUE;
						if (IDNO == MsgBox(_T("ī�޶�(��)�� �˻��ǳ��� �ҷ���ŷ �Ͻðڽ��ϱ�?"), 0, MB_YESNO))
						{
							m_bDoMk[1] = FALSE;
							m_bDoneMk[1] = TRUE;
						}
						else
						{
							m_bDoMk[1] = TRUE;
							m_bDoneMk[1] = FALSE;
						}
					}
					else
					{
						m_bReAlign[1][1] = TRUE;
						m_bSkipAlign[1][1] = FALSE;
						m_nMkStAuto = MK_ST + (Mk2PtIdx::Move1Cam1); // TwoPointAlign1(1) ���� ����. - ī�޶� ������
						Stop();
						//TowerLamp(RGB_YELLOW, TRUE);
					}
				}
				else
				{
					Buzzer(FALSE, 0);

					m_bReAlign[1][1] = TRUE;
					m_bSkipAlign[1][1] = FALSE;
					m_nMkStAuto = MK_ST + (Mk2PtIdx::Move1Cam1); // TwoPointAlign1(1) ���� ����. - ī�޶� ������
					Stop();
					//TowerLamp(RGB_YELLOW, TRUE);
				}
			}

			if (m_bFailAlign[0][1] || m_bFailAlign[1][1])
			{
				if (!m_bReAlign[0][1] && !m_bReAlign[1][1])
				{
					if (m_bDoMk[0] || m_bDoMk[1])
						m_nMkStAuto = MK_ST + (Mk2PtIdx::MoveInitPt); //m_nMkStAuto = MK_ST + 29;  // MoveInitPos0()
					else
					{
						if (!IsInitPos0())
							MoveInitPos0();
						if (!IsInitPos1())
							MoveInitPos1();

						m_nMkStAuto = MK_ST + (Mk2PtIdx::DoneMk); // �ҷ��̹��� Display, Align���� �ʱ�ȭ (Skip 65 : Mk())
					}
				}
				else
					m_nMkStAuto = MK_ST + (Mk2PtIdx::Move1Cam1); // TwoPointAlign1(1) ���� ����. - ī�޶� ������
			}
			else
				m_nMkStAuto = MK_ST + (Mk2PtIdx::MoveInitPt);

			break;
		}
	}
}

void CGvisR2R_PunchView::Mk2PtMoveInitPos()
{
	if (m_bMkSt)
	{
		switch (m_nMkStAuto)
		{
		case MK_ST + (Mk2PtIdx::MoveInitPt) :
			MoveInitPos0(FALSE);
			MoveInitPos1(FALSE); // 20220526
			m_nMkStAuto++;
			break;
		case MK_ST + (Mk2PtIdx::MoveInitPt) + 1:
			m_nMkStAuto++;
			break;
		case MK_ST + (Mk2PtIdx::MoveInitPt) + 2:
			if (IsMoveDone())
				m_nMkStAuto = MK_ST + (Mk2PtIdx::ChkElec);
			break;
		}
	}
}

void CGvisR2R_PunchView::Mk2PtElecChk()
{
	CString sRst;

	if (m_bMkSt)
	{
		switch (m_nMkStAuto)
		{
		case MK_ST + (Mk2PtIdx::ChkElec) : // DoElecChk
			if (DoElecChk(sRst))
			{
				if (pDoc->WorkingInfo.Probing[0].bUse)
				{
					if (sRst == _T("Open"))
					{
						if (pDoc->WorkingInfo.Probing[0].bStopOnOpen)
							m_nMkStAuto = REJECT_ST;
						else
							m_nMkStAuto++;
					}
					else if (sRst == _T("Error"))
					{
						m_nMkStAuto = ERROR_ST;
					}
					else
					{
						m_nMkStAuto++;
					}
				}
				else
					m_nMkStAuto++;
			}
		   break;

		case MK_ST + (Mk2PtIdx::ChkElec) + 1:
			if (ChkLightErr())
			{
				m_bChkLightErr = FALSE;
				m_nMkStAuto++;
			}
			else
			{
				if (MODE_INNER != pDoc->GetTestMode())
					m_nMkStAuto = MK_ST + (Mk2PtIdx::DoMk); 	// Mk ��ŷ ����
				else
					m_nMkStAuto = MK_ST + (Mk2PtIdx::Shift2Mk);
			}
			break;

		case MK_ST + (Mk2PtIdx::ChkElec) + 2:
			if (IsRun())
			{
				if (m_pMotion->IsEnable(MS_X0) && m_pMotion->IsEnable(MS_Y0) &&
					m_pMotion->IsEnable(MS_X1) && m_pMotion->IsEnable(MS_Y1))
				{
					if (MODE_INNER != pDoc->GetTestMode())
						m_nMkStAuto = MK_ST + (Mk2PtIdx::DoMk); 	// Mk ��ŷ ����
					else
						m_nMkStAuto = MK_ST + (Mk2PtIdx::Shift2Mk);
				}
				else
				{
					Stop();
					MsgBox(_T("��ŷ�� ����� ��Ȱ��ȭ �Ǿ����ϴ�."));
					TowerLamp(RGB_RED, TRUE);
				}
			}
			else
			{
				if (!m_bChkLightErr)
				{
					m_bChkLightErr = TRUE;
					MsgBox(_T("�뱤�ҷ� ���� - ������ Ȯ���ϼ���.\r\n��������Ϸ��� ��������ġ�� ��������."));
				}
			}
			break;
		}
	}
}

void CGvisR2R_PunchView::Mk2PtDoMarking()
{
	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;
	CString sRst, sMsg;
	int a, b, nSerial, nPrevSerial;

	if (m_bMkSt)
	{
		switch (m_nMkStAuto)
		{
		case MK_ST + (Mk2PtIdx::DoMk) :				// Mk ��ŷ ����
			if (CheckMkPnt())
			{
				if (pDoc->GetTestMode() == MODE_OUTER)
					SetMkIts(TRUE);						// ITS ��ŷ ����
				else
					SetMk(TRUE);						// Mk ��ŷ ����

				m_nMkStAuto++;
			}
			else
			{
				MsgBox(_T("ķ�������� ��ŷ��ġ��ǥ�� �����Ǿ� �����ʽ��ϴ�.\r\nȮ���ϼ���."));
				Stop();
				TowerLamp(RGB_RED, TRUE);
			}
			break;

		case MK_ST + (Mk2PtIdx::DoMk) + 1:
			if (!m_bUpdateYield)
			{
				if (!m_bTHREAD_UPDATAE_YIELD[0] && !m_bTHREAD_UPDATAE_YIELD[1])
				{
					m_bUpdateYield = TRUE;
					UpdateYield(); // Cam[0],  Cam[1]
					m_nMkStAuto++;
				}
			}
			else
			{
				Sleep(100);
				m_nMkStAuto++;
			}
			break;

		case MK_ST + (Mk2PtIdx::DoMk) + 2:
			if (!m_bTHREAD_UPDATAE_YIELD[0] && !m_bTHREAD_UPDATAE_YIELD[1])
			{
				if (!m_bUpdateYieldOnRmap)
				{
					if (!m_bTHREAD_UPDATE_REELMAP_UP && !m_bTHREAD_UPDATE_REELMAP_DN && !m_bTHREAD_UPDATE_REELMAP_ALLUP && !m_bTHREAD_UPDATE_REELMAP_ALLDN)
					{
						if (!m_bTHREAD_UPDATE_YIELD_UP && !m_bTHREAD_UPDATE_YIELD_DN && !m_bTHREAD_UPDATE_YIELD_ALLUP && !m_bTHREAD_UPDATE_YIELD_ALLDN)
						{
							m_bUpdateYieldOnRmap = TRUE;
							pDoc->UpdateYieldOnRmap(); // 20230614
							m_nMkStAuto++;
						}
						else
							Sleep(100);
					}
					else
						Sleep(100);
				}
				else
				{
					Sleep(100);
					m_nMkStAuto++; // ��ŷ �� verify�� ������ ������ ���� ���.
				}
			}
			break;

		case MK_ST + (Mk2PtIdx::Verify) :
			if (IsMkDone() && IsMoveDone())
			{
				if (IsVerify() && !m_nPrevMkStAuto)
				{
					m_nPrevMkStAuto = MK_ST + (Mk2PtIdx::Verify);
					m_nMkStAuto = MK_ST + (Mk2PtIdx::DoMk);		// Mk ��ŷ ����
					m_bCam = TRUE;
				}
				else
				{
					if (IsReview())
					{
						if (!m_bCam)
						{
							m_nPrevStepAuto = MK_ST + (Mk2PtIdx::Verify);
							m_nMkStAuto = MK_ST + (Mk2PtIdx::DoMk);		// Mk ��ŷ ����
							m_bCam = TRUE;
							MsgBox(_T("�� Jog ��ư�� �̿��Ͽ� ��ŷ��ġ�� Ȯ���Ͽ� �ּ���."));
						}
						else
						{
							m_bCam = FALSE;
							m_nMkStAuto = MK_ST + (Mk2PtIdx::DoneMk);	// Mk ��ŷ �Ϸ�
						}
					}
					else
					{
						m_nMkStAuto = MK_ST + (Mk2PtIdx::DoneMk);	// Mk ��ŷ �Ϸ�
					}
				}
			}
			else if (IsReMk())
			{
				m_nPrevMkStAuto = MK_ST + (Mk2PtIdx::Verify);
				m_nMkStAuto = MK_ST + (Mk2PtIdx::DoMk);		// Mk �����
			}
			else
			{
				sMsg = _T("");
				sMsg += m_sDispSts[0];
				sMsg += _T(",");
				sMsg += m_sDispSts[1];
			}
			break;

		case MK_ST + (Mk2PtIdx::DoneMk) :	 // Align���� �ʱ�ȭ
			if (!IsRun()) 
				break;

			//if (m_bTHREAD_UPDATE_YIELD_UP || m_bTHREAD_UPDATE_YIELD_DN || m_bTHREAD_UPDATE_YIELD_ALLUP || m_bTHREAD_UPDATE_YIELD_ALLDN)
			//	break;

			if (m_bInitAuto)
			{
				m_bInitAuto = FALSE;
				MsgBox(_T("��ŷ��ġ�� Ȯ���ϼ���."));
				Stop();
				TowerLamp(RGB_YELLOW, TRUE);
				break;
			}

			if (m_nBufUpSerial[0] == 0)
			{
				m_bSkipAlign[0][0] = TRUE;
				m_bSkipAlign[0][1] = TRUE;
			}
			if (m_nBufUpSerial[1] == 0)
			{
				m_bSkipAlign[1][0] = TRUE;
				m_bSkipAlign[1][1] = TRUE;
			}

			if( (!m_bSkipAlign[0][0] && !m_bSkipAlign[0][1]) && (!m_bSkipAlign[1][0] && !m_bSkipAlign[1][1]) )
				CompletedMk(2); // 0: Only Cam0, 1: Only Cam1, 2: Cam0 and Cam1, 3: None
			else if( (m_bSkipAlign[0][0] || m_bSkipAlign[0][1]) && (!m_bSkipAlign[1][0] && !m_bSkipAlign[1][1]) )
				CompletedMk(1); // 0: Only Cam0, 1: Only Cam1, 2: Cam0 and Cam1, 3: None
			else if( (!m_bSkipAlign[0][0] && !m_bSkipAlign[0][1]) && (m_bSkipAlign[1][0] || m_bSkipAlign[1][1]) )
				CompletedMk(0); // 0: Only Cam0, 1: Only Cam1, 2: Cam0 and Cam1, 3: None
			else
				CompletedMk(3); // 0: Only Cam0, 1: Only Cam1, 2: Cam0 and Cam1, 3: None

			m_nMkStAuto++;
			break;
		case MK_ST + (Mk2PtIdx::DoneMk) + 1:  // Mk���� �ʱ�ȭ
			m_bReAlign[0][0] = FALSE; // [nCam][nPos] 
			m_bReAlign[0][1] = FALSE; // [nCam][nPos] 
			m_bReAlign[1][0] = FALSE; // [nCam][nPos] 
			m_bReAlign[1][1] = FALSE; // [nCam][nPos] 

			m_bSkipAlign[0][0] = FALSE; // [nCam][nPos] 
			m_bSkipAlign[0][1] = FALSE; // [nCam][nPos] 
			m_bSkipAlign[1][0] = FALSE; // [nCam][nPos] 
			m_bSkipAlign[1][1] = FALSE; // [nCam][nPos] 

			m_bFailAlign[0][0] = FALSE; // [nCam][nPos] 
			m_bFailAlign[0][1] = FALSE; // [nCam][nPos] 
			m_bFailAlign[1][0] = FALSE; // [nCam][nPos] 
			m_bFailAlign[1][1] = FALSE; // [nCam][nPos] 

			m_bDoMk[0] = TRUE;
			m_bDoMk[1] = TRUE;
			m_bDoneMk[0] = FALSE;
			m_bDoneMk[1] = FALSE;
			m_bReMark[0] = FALSE;
			m_bReMark[1] = FALSE;
			m_bCam = FALSE;
			m_nPrevMkStAuto = 0;

			m_bUpdateYield = FALSE;
			m_bUpdateYieldOnRmap = FALSE;

			for (a = 0; a < 2; a++)
			{
				for (b = 0; b < MAX_STRIP_NUM; b++)
				{
					m_nMkStrip[a][b] = 0;
					m_bRejectDone[a][b] = FALSE;
				}
			}

			m_nSaveMk0Img = 0;
			m_nSaveMk1Img = 0;

			m_pMpe->Write(_T("MB440150"), 0);	// ��ŷ�� ��ŷ�� ON (PC�� ON, OFF)
			m_pMpe->Write(_T("MB440170"), 1);	// ��ŷ�Ϸ�(PLC�� Ȯ���ϰ� Reset��Ŵ.)-20141029
			pDoc->LogAuto(_T("PC: ��ŷ�Ϸ�(PLC�� Ȯ���ϰ� Reset��Ŵ.)"));
			if (IsNoMk() || IsShowLive())
				ShowLive(FALSE);

			m_nMkStAuto++;
			break;

		case MK_ST + (Mk2PtIdx::DoneMk) + 2:
			if (pDoc->m_pMpeSignal[0] & (0x01 << 1))	// ��ŷ�� Feeding�Ϸ�(PLC�� On��Ű�� PC�� Ȯ���ϰ� Reset��Ŵ.)-20141030
			{
				pDoc->LogAuto(_T("PLC: ��ŷ�� Feeding�Ϸ�(PLC�� On��Ű�� PC�� Ȯ���ϰ� Reset��Ŵ.)"));
				m_nMkStAuto++;
				//if (!m_bTHREAD_UPDATAE_YIELD[0] && !m_bTHREAD_UPDATAE_YIELD[1])
				//{
				//	UpdateYield(); // Cam[0],  Cam[1]
				//	m_nMkStAuto++;
				//}
			}
			break;

		case MK_ST + (Mk2PtIdx::DoneMk) + 3:
#ifdef USE_MPE
			//if (pDoc->m_pMpeSignal[0] & (0x01 << 1))	// ��ŷ�� Feeding�Ϸ�(PLC�� On��Ű�� PC�� Ȯ���ϰ� Reset��Ŵ.)-20141030
			{
				if (!m_bTHREAD_SHIFT2MK)
				{
					m_pMpe->Write(_T("MB440101"), 0);	// ��ŷ�� Feeding�Ϸ�
					pDoc->LogAuto(_T("PC: ��ŷ�� Feeding�Ϸ� OFF"));

					//Shift2Mk();			// PCR �̵�(Buffer->Marked) // ���(WorkingInfo.LastJob.sSerial)
					m_bShift2Mk = TRUE;
					DoShift2Mk();

					SetMkFdLen();
					SetCycTime();
					m_dwCycSt = GetTickCount();

					UpdateRst();

					//UpdateWorking();	// Update Working Info...
					m_nMkStAuto++;
				}
			}
#endif
			break;
		case MK_ST + (Mk2PtIdx::DoneMk) + 4:
			//sMsg.Format(_T("%d%d%d%d%d"), m_bTHREAD_SHIFT2MK ? 1 : 0, m_bTHREAD_REELMAP_YIELD_UP ? 1 : 0, m_bTHREAD_REELMAP_YIELD_DN ? 1 : 0, m_bTHREAD_REELMAP_YIELD_ALLUP ? 1 : 0, m_bTHREAD_REELMAP_YIELD_ALLDN ? 1 : 0);
			//DispStsBar(sMsg, 0);

			if (!m_bTHREAD_SHIFT2MK && !m_bTHREAD_REELMAP_YIELD_UP && !m_bTHREAD_REELMAP_YIELD_DN && !m_bTHREAD_REELMAP_YIELD_ALLUP && !m_bTHREAD_REELMAP_YIELD_ALLDN) // Yield Reelmap
			{
				if (pDoc->GetTestMode() == MODE_OUTER)
				{
					if (m_bTHREAD_REELMAP_YIELD_ITS) // Yield Reelmap
						break;
				}

				m_nMkStAuto++;
				//UpdateRst();
				//UpdateWorking();	// Update Working Info...
				ChkYield();
			}
			break;
		case MK_ST + (Mk2PtIdx::DoneMk) + 5:
			if (!m_bTHREAD_SHIFT2MK)
			{
				SetListBuf();
				ChkLotCutPos();
				UpdateWorking();	// Update Working Info...
				m_nMkStAuto++;
			}
			break;
		case MK_ST + (Mk2PtIdx::DoneMk) + 6:
			m_nMkStAuto++;
			if (!IsBuffer(0))
			{
				m_bLotEnd = TRUE;
				m_nLotEndAuto = LOT_END;
			}
			else
			{
				// [0]: AOI-Up , [1]: AOI-Dn
				if (bDualTest)
				{
					nSerial = pDoc->m_ListBuf[0].Pop(); // Up (��)
					nSerial = pDoc->m_ListBuf[0].Pop(); // Up (��)

					SetListBuf();
				}

				if (m_nLotEndSerial > 0)
				{
					nSerial = GetBufferUp(&nPrevSerial);

					if (pView->m_bSerialDecrese)
					{
						if (nSerial < m_nLotEndSerial || nSerial <= 0) // �뱤�ҷ� 3��° Lot End ( -1, -1, -2)
						{
							if (IsDoneDispMkInfo())
							{
								if (IsBuffer() && nSerial > 0)
									SetSerial(nSerial);
								else
									SetSerial(nPrevSerial - 1, TRUE);

								if (IsBuffer() && nSerial > 0)
									SetSerial(nSerial - 1);
								else
									SetSerial(nPrevSerial - 2, TRUE);

								//m_nStepAuto = LOT_END;
								m_bLotEnd = TRUE;
								m_nLotEndAuto = LOT_END;
							}
						}
					}
					else
					{
						if (nSerial > m_nLotEndSerial || nSerial <= 0) // �뱤�ҷ� 3��° Lot End ( -1, -1, -2)
						{
							if (IsDoneDispMkInfo())
							{
								if (IsBuffer() && nSerial > 0)
									SetSerial(nSerial);
								else
									SetSerial(nPrevSerial + 1, TRUE);

								if (IsBuffer() && nSerial > 0)
									SetSerial(nSerial + 1);
								else
									SetSerial(nPrevSerial + 2, TRUE);

								//m_nStepAuto = LOT_END;
								m_bLotEnd = TRUE;
								m_nLotEndAuto = LOT_END;
							}
						}
					}

					if (bDualTest)
					{
						nSerial = GetBufferDn(&nPrevSerial);

						if (pView->m_bSerialDecrese)
						{
							if (nSerial < m_nLotEndSerial || nSerial <= 0) // �뱤�ҷ� 3��° Lot End ( -1, -1, -2)
							{
								if (IsDoneDispMkInfo())
								{
									if (IsBuffer() && nSerial > 0)
										SetSerial(nSerial);
									else
										SetSerial(nPrevSerial - 1, TRUE);

									if (IsBuffer() && nSerial > 0)
										SetSerial(nSerial - 1);
									else
										SetSerial(nPrevSerial - 2, TRUE);

									m_bLotEnd = TRUE;
									m_nLotEndAuto = LOT_END;
								}
							}
						}
						else
						{
							if (nSerial > m_nLotEndSerial || nSerial <= 0) // �뱤�ҷ� 3��° Lot End ( -1, -1, -2)
							{
								if (IsDoneDispMkInfo())
								{
									if (IsBuffer() && nSerial > 0)
										SetSerial(nSerial);
									else
										SetSerial(nPrevSerial + 1, TRUE);

									if (IsBuffer() && nSerial > 0)
										SetSerial(nSerial + 1);
									else
										SetSerial(nPrevSerial + 2, TRUE);

									m_bLotEnd = TRUE;
									m_nLotEndAuto = LOT_END;
								}
							}
						}
					}
				}
			}

			break;
		case MK_ST + (Mk2PtIdx::DoneMk) + 7:
			m_bMkSt = FALSE;
			::WritePrivateProfileString(_T("Last Job"), _T("MkSt"), _T("0"), PATH_WORKING_INFO);
			break;
		}
	}
}

void CGvisR2R_PunchView::Mk2PtShift2Mk() // MODE_INNER
{
	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;
	CString sRst, sMsg;
	int a, b, nSerial, nPrevSerial;

	if (m_bMkSt)
	{
		switch (m_nMkStAuto)
		{
		case MK_ST + (Mk2PtIdx::Shift2Mk) :
			//if (!m_bUpdateYield)
			//{
				if (!m_bTHREAD_UPDATAE_YIELD[0] && !m_bTHREAD_UPDATAE_YIELD[1])
				{
					//m_bUpdateYield = TRUE;
					UpdateYield(); // Cam[0],  Cam[1]
					m_nMkStAuto++;
				}
			//}
			//else
			//{
			//	Sleep(100);
			//	m_nMkStAuto++;
			//}
			break;

		case MK_ST + (Mk2PtIdx::Shift2Mk) + 1:
			if (!m_bTHREAD_UPDATAE_YIELD[0] && !m_bTHREAD_UPDATAE_YIELD[1])
			{
				//if (!m_bUpdateYieldOnRmap)
				//{
					if (!m_bTHREAD_UPDATE_REELMAP_UP && !m_bTHREAD_UPDATE_REELMAP_DN && !m_bTHREAD_UPDATE_REELMAP_ALLUP && !m_bTHREAD_UPDATE_REELMAP_ALLDN)
					{
						if (!m_bTHREAD_UPDATE_YIELD_UP && !m_bTHREAD_UPDATE_YIELD_DN && !m_bTHREAD_UPDATE_YIELD_ALLUP && !m_bTHREAD_UPDATE_YIELD_ALLDN)
						{
							//m_bUpdateYieldOnRmap = TRUE;
							pDoc->UpdateYieldOnRmap(); // 20230614
							m_nMkStAuto++;
						}
						else
							Sleep(100);
					}
					else
						Sleep(100);
				//}
				//else
				//{
				//	Sleep(100);
				//	m_nMkStAuto++; // ��ŷ �� verify�� ������ ������ ���� ���.
				//}
			}
			break;

		case MK_ST + (Mk2PtIdx::Shift2Mk) + 2:
			m_pMpe->Write(_T("MB440150"), 0);	// ��ŷ�� ��ŷ�� ON (PC�� ON, OFF)
			m_pMpe->Write(_T("MB440170"), 1);	// ��ŷ�Ϸ�(PLC�� Ȯ���ϰ� Reset��Ŵ.)-20141029
			pDoc->LogAuto(_T("PC: ��ŷ�Ϸ�(PLC�� Ȯ���ϰ� Reset��Ŵ.)"));
			m_nMkStAuto++;
			break;

		case MK_ST + (Mk2PtIdx::Shift2Mk) + 3:
			if (pDoc->m_pMpeSignal[0] & (0x01 << 1))	// ��ŷ�� Feeding�Ϸ�(PLC�� On��Ű�� PC�� Ȯ���ϰ� Reset��Ŵ.)-20141030
			{
				pDoc->LogAuto(_T("PLC: ��ŷ�� Feeding�Ϸ�(PLC�� On��Ű�� PC�� Ȯ���ϰ� Reset��Ŵ.)"));
				m_nMkStAuto++;
				//if (!m_bTHREAD_UPDATAE_YIELD[0] && !m_bTHREAD_UPDATAE_YIELD[1])
				//{
				//	UpdateYield(); // Cam[0],  Cam[1]
				//	m_nMkStAuto++;
				//}
			}
			break;

		case MK_ST + (Mk2PtIdx::Shift2Mk) + 4:
#ifdef USE_MPE
			//if (pDoc->m_pMpeSignal[0] & (0x01 << 1))	// ��ŷ�� Feeding�Ϸ�(PLC�� On��Ű�� PC�� Ȯ���ϰ� Reset��Ŵ.)-20141030
			{
				if (!m_bTHREAD_SHIFT2MK)
				{
					pDoc->LogAuto(_T("PC: ��ŷ�� Feeding�Ϸ� OFF"));
					m_pMpe->Write(_T("MB440101"), 0);	// ��ŷ�� Feeding�Ϸ�
					m_bShift2Mk = TRUE;
					DoShift2Mk();

					SetMkFdLen();
					SetCycTime();
					m_dwCycSt = GetTickCount();

					m_nMkStAuto++;
				}
			}
#endif
			break;
		case MK_ST + (Mk2PtIdx::Shift2Mk) + 5:
			if (!m_bTHREAD_SHIFT2MK && !m_bTHREAD_REELMAP_YIELD_UP && !m_bTHREAD_REELMAP_YIELD_DN && !m_bTHREAD_REELMAP_YIELD_ALLUP && !m_bTHREAD_REELMAP_YIELD_ALLDN) // Yield Reelmap
			{
				if (pDoc->GetTestMode() == MODE_OUTER)
				{
					if (m_bTHREAD_REELMAP_YIELD_ITS) // Yield Reelmap
						break;
				}

				m_nMkStAuto++;
				UpdateRst();
				UpdateWorking();	// Update Working Info...
				ChkYield();
			}
			break;
		case MK_ST + (Mk2PtIdx::Shift2Mk) + 6:
			if (!m_bTHREAD_SHIFT2MK)
			{
				SetListBuf();
				ChkLotCutPos();
				m_nMkStAuto++;
			}
			break;
		case MK_ST + (Mk2PtIdx::Shift2Mk) + 7:
			m_nMkStAuto++;
			if (!IsBuffer(0))
			{
				m_bLotEnd = TRUE;
				m_nLotEndAuto = LOT_END;
			}
			else
			{
				// [0]: AOI-Up , [1]: AOI-Dn
				nSerial = pDoc->m_ListBuf[0].Pop(); // Up (��)
				nSerial = pDoc->m_ListBuf[0].Pop(); // Up (��)

				SetListBuf();

				if (pView->m_bSerialDecrese)
				{
					if (m_nLotEndSerial > 0)
					{
						nSerial = GetBufferUp(&nPrevSerial); // m_pBufSerial[0][0] : ���� ù��° �ø���

						if (nSerial < m_nLotEndSerial || nSerial <= 0) // �뱤�ҷ� 3��° Lot End ( -1, -1, -2)
						{
							if (IsDoneDispMkInfo())
							{
								if (IsBuffer() && nSerial > 0)
									SetSerial(nSerial);
								//else
								//	SetSerial(nPrevSerial + 1, TRUE);

								nSerial = m_pBufSerial[0][1]; // m_pBufSerial[0][1] : ���� �ι�° �ø���
								if (IsBuffer() && nSerial > 0)
									SetSerial(nSerial);	//SetSerial(nSerial + 1);
								//else
								//	SetSerial(nPrevSerial + 2, TRUE);

								m_bLotEnd = TRUE;
								m_nLotEndAuto = LOT_END;
							}
						}
					}
				}
				else
				{
					if (m_nLotEndSerial > 0)
					{
						nSerial = GetBufferUp(&nPrevSerial); // m_pBufSerial[0][0] : ���� ù��° �ø���

						if (nSerial > m_nLotEndSerial || nSerial <= 0) // �뱤�ҷ� 3��° Lot End ( -1, -1, -2)
						{
							if (IsDoneDispMkInfo())
							{
								if (IsBuffer() && nSerial > 0)
									SetSerial(nSerial);
								//else
								//	SetSerial(nPrevSerial + 1, TRUE);

								nSerial = m_pBufSerial[0][1]; // m_pBufSerial[0][1] : ���� �ι�° �ø���
								if (IsBuffer() && nSerial > 0)
									SetSerial(nSerial);	//SetSerial(nSerial + 1);
								//else
								//	SetSerial(nPrevSerial + 2, TRUE);

								m_bLotEnd = TRUE;
								m_nLotEndAuto = LOT_END;
							}
						}
					}
				}
			}

			break;
		case MK_ST + (Mk2PtIdx::Shift2Mk) + 8:
			m_bMkSt = FALSE;
			::WritePrivateProfileString(_T("Last Job"), _T("MkSt"), _T("0"), PATH_WORKING_INFO);
			break;
		}
	}
}

void CGvisR2R_PunchView::Mk2PtLotDiff()
{
	if (m_bMkSt)
	{
		switch (m_nMkStAuto)
		{
		case MK_ST + (Mk2PtIdx::LotDiff) :
			Stop();
			TowerLamp(RGB_YELLOW, TRUE);
			m_nMkStAuto++;
			break;
		case MK_ST + (Mk2PtIdx::LotDiff) + 1:
			//if(IDYES == DoMyMsgBox(_T("���� �ϸ��� Lot�� �ٸ��ϴ�.\r\n��� �۾��� �����Ͻðڽ��ϱ�?"), MB_YESNO))
			if (IDYES == MsgBox(_T("���� �ϸ��� Lot�� �ٸ��ϴ�.\r\n��� �۾��� �����Ͻðڽ��ϱ�?"), 0, MB_YESNO))
			{
				m_bContDiffLot = TRUE;
			}
			else
			{
				m_bContDiffLot = FALSE;
				m_bLotEnd = TRUE;
				m_nLotEndAuto = LOT_END;
			}
			m_nMkStAuto++;
			break;
		case MK_ST + (Mk2PtIdx::LotDiff) + 2:
			if (IsRun())
			{
				if (m_bContDiffLot)
					m_nMkStAuto = MK_ST + (Mk2PtIdx::ChkSn);
				else
					m_nMkStAuto++;
			}
			break;
		case MK_ST + (Mk2PtIdx::LotDiff) + 3:
			m_bContDiffLot = FALSE;
			m_bLotEnd = TRUE;
			m_nLotEndAuto = LOT_END;
			break;
		}
	}
}

void CGvisR2R_PunchView::Mk2PtReject()
{
	int a, b;

	if (m_bMkSt)
	{
		switch (m_nMkStAuto)
		{
		case REJECT_ST:
			Buzzer(TRUE, 0);
			TowerLamp(RGB_RED, TRUE);
			Stop();

			//if(IDYES == DoMyMsgBox(_T("��Ʈ üũ �ҷ��Դϴ�.\r\n���� ó���� �����Ͻðڽ��ϱ�?"), MB_YESNO))
			if (IDYES == MsgBox(_T("��Ʈ üũ �ҷ��Դϴ�.\r\n���� ó���� �����Ͻðڽ��ϱ�?"), 0, MB_YESNO))
			{
				m_bAnswer[0] = TRUE;
				m_nMkStAuto++;
			}
			else
			{
				//if(IDYES == DoMyMsgBox(_T("���� ó���� ����Ͽ����ϴ�.\r\n�ҷ��� ��ŷ ó���� �����Ͻðڽ��ϱ�?"), MB_YESNO))
				if (IDYES == MsgBox(_T("���� ó���� ����Ͽ����ϴ�.\r\n�ҷ��� ��ŷ ó���� �����Ͻðڽ��ϱ�?"), 0, MB_YESNO))
				{
					m_bAnswer[1] = TRUE;
					m_nMkStAuto++;
				}
			}

			Buzzer(FALSE, 0);
			break;
		case REJECT_ST + 1:
			if (IsRun())
			{
				if (m_bAnswer[0])
				{
					m_bAnswer[0] = FALSE;
					m_nMkStAuto++;
				}
				else if (m_bAnswer[1])
				{
					m_bAnswer[1] = FALSE;
					m_nMkStAuto = MK_ST + (Mk2PtIdx::DoMk);	// Mk ��ŷ ����
				}
				else
					m_nMkStAuto = REJECT_ST;
			}
			break;
		case REJECT_ST + 2:
			SetReject();
			m_nMkStAuto++;
			break;
		case REJECT_ST + 3:
			m_nMkStAuto++;
			break;
		case REJECT_ST + 4:
			if (IsMkDone() && IsMoveDone())
			{
				if (IsVerify() && !m_nPrevMkStAuto)
				{
					m_nPrevMkStAuto = REJECT_ST + 4;
					m_nMkStAuto = REJECT_ST + 2;		// Mk ��ŷ ����
					m_bCam = TRUE;

					m_bDoneMk[0] = FALSE;
					m_bDoneMk[1] = FALSE;

					for (a = 0; a < 2; a++)
					{
						for (b = 0; b < MAX_STRIP_NUM; b++)
						{
							m_nMkStrip[a][b] = 0;
							m_bRejectDone[a][b] = FALSE;
						}
					}
				}
				else
				{
					if (IsReview())
					{
						if (!m_bCam)
						{
							m_nPrevStepAuto = REJECT_ST + 4;
							m_nMkStAuto = REJECT_ST + 2;		// Mk ��ŷ ����
							m_bCam = TRUE;
							MsgBox(_T("�� Jog ��ư�� �̿��Ͽ� ��ŷ��ġ�� Ȯ���Ͽ� �ּ���."));
						}
						else
						{
							m_bCam = FALSE;
							m_nMkStAuto++;	// Mk ��ŷ �Ϸ�

											//sMsg = _T("");
											//DispStsBar(sMsg, 0);
						}
					}
					else
					{
						m_nMkStAuto++;	// Mk ��ŷ �Ϸ�

										//sMsg = _T("");
										//DispStsBar(sMsg, 0);
					}
				}
			}
			break;
		case REJECT_ST + 5:
			m_nMkStAuto = MK_ST + (Mk2PtIdx::DoneMk);				// Align���� �ʱ�ȭ
			break;
		}
	}
}

void CGvisR2R_PunchView::Mk2PtErrStop()
{
	if (m_bMkSt)
	{
		switch (m_nMkStAuto)
		{
		case ERROR_ST:
			Stop();
			TowerLamp(RGB_RED, TRUE);

			//if(IDYES == DoMyMsgBox(_T("��Ʈ üũ Error�Դϴ�.\r\n�ٽ� ��Ʈ üũ�� �����Ͻðڽ��ϱ�?"), MB_YESNO))
			if (IDYES == MsgBox(_T("��Ʈ üũ Error�Դϴ�.\r\n�ٽ� ��Ʈ üũ�� �����Ͻðڽ��ϱ�?"), 0, MB_YESNO))
			{
				m_bAnswer[0] = TRUE;
				m_nMkStAuto++;
			}
			else
			{
				//if(IDYES == DoMyMsgBox(_T("��Ʈ üũ�� ����Ͽ����ϴ�.\r\n�ҷ��� ��ŷ ó���� �����Ͻðڽ��ϱ�?"), MB_YESNO))
				if (IDYES == MsgBox(_T("��Ʈ üũ�� ����Ͽ����ϴ�.\r\n�ҷ��� ��ŷ ó���� �����Ͻðڽ��ϱ�?"), 0, MB_YESNO))
				{
					m_bAnswer[1] = TRUE;
					m_nMkStAuto++;
				}
				else
					m_nMkStAuto++;
			}
			break;
		case ERROR_ST + 1:
			m_nMkStAuto++;
			break;
		case ERROR_ST + 2:
			if (IsRun())
			{
				if (m_bAnswer[0])
				{
					m_bAnswer[0] = FALSE;
					m_nMkStAuto = MK_ST + (Mk2PtIdx::ChkElec); // DoElecChk
				}
				else if (m_bAnswer[1])
				{
					m_bAnswer[1] = FALSE;
					m_nMkStAuto = MK_ST + (Mk2PtIdx::DoMk);	// Mk ��ŷ ����
				}
				else
					m_nMkStAuto++;
			}
			break;
		case ERROR_ST + 3:
			m_nMkStAuto = ERROR_ST;
			break;
		}
	}
}

void CGvisR2R_PunchView::MarkingWith2PointAlign()
{
	Mk2PtReady();
	Mk2PtChkSerial();
	Mk2PtInit();
	Mk2PtAlignPt0();
	Mk2PtAlignPt1();
	Mk2PtMoveInitPos();
	Mk2PtElecChk();
	if (MODE_INNER != pDoc->GetTestMode())
		Mk2PtDoMarking();
	else
		Mk2PtShift2Mk();
	Mk2PtLotDiff();
	Mk2PtReject();
	Mk2PtErrStop();
}

void CGvisR2R_PunchView::Mk4PtReady()
{
	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;

	if (m_bMkSt)
	{
		switch (m_nMkStAuto)
		{
		case MK_ST:	// PLC MK ��ȣ Ȯ��	
			if (IsRun())
			{
				SetListBuf();
				m_nMkStAuto++;
			}
			break;
		case MK_ST + 1:
			pDoc->LogAuto(_T("PC: ��ŷ�� ��ŷ�� ON (PC�� ON, OFF)"));
			m_pMpe->Write(_T("MB440150"), 1);// ��ŷ�� ��ŷ�� ON (PC�� ON, OFF)
			m_nMkStAuto++;
			break;
		case MK_ST + (Mk4PtIdx::Start) :	// 2
			if (bDualTest)
			{
				if (pDoc->m_ListBuf[1].nTot > 0) // AOI-Dn
				{
					m_nMkStAuto++;

					m_nBufDnSerial[0] = pDoc->m_ListBuf[1].Pop();
					m_nBufUpSerial[0] = m_nBufDnSerial[0];
					if (pDoc->m_ListBuf[1].nTot > 0) // AOI-Dn
					{
						m_nBufDnSerial[1] = pDoc->m_ListBuf[1].Pop();
						m_nBufUpSerial[1] = m_nBufDnSerial[1];
					}
					else
					{
						m_nBufDnSerial[1] = 0;
						m_nBufUpSerial[1] = 0;
					}
				}
				else
				{
					m_bLotEnd = TRUE;
					m_nLotEndAuto = LOT_END;
				}

				if (pDoc->WorkingInfo.LastJob.bSampleTest)
				{
					if (m_nBufUpSerial[0] == 1)
					{
						m_nLotEndSerial = _tstoi(pDoc->WorkingInfo.LastJob.sSampleTestShotNum);
						m_bLastProcFromUp = FALSE;
						m_bLastProcFromEng = FALSE;
						m_bLastProc = TRUE;
						if (m_pDlgMenu01)
							m_pDlgMenu01->m_bLastProc = TRUE;
						if (m_pMpe)
						{
							m_pMpe->Write(_T("MB440186"), 1);			// �ܷ�ó�� AOI(��) ����(PC�� On��Ű��, PLC�� Ȯ���ϰ� Off��Ŵ)-20141112
							m_pMpe->Write(_T("MB440181"), 1);			// �ܷ�ó��(PC�� On��Ű��, PLC�� Ȯ���ϰ� Off��Ŵ)-20141031
							pDoc->LogAuto(_T("PC: �ܷ�ó�� AOI(��) ����(PC�� On��Ű��, PLC�� Ȯ���ϰ� Off��Ŵ)"));
						}
					}
				}
			}
			else
			{
				if (pDoc->m_ListBuf[0].nTot > 0) // AOI-Up
				{
					m_nMkStAuto++;
					m_nBufUpSerial[0] = pDoc->m_ListBuf[0].Pop();
					if (pDoc->m_ListBuf[0].nTot > 0) // AOI-Up
						m_nBufUpSerial[1] = pDoc->m_ListBuf[0].Pop();
					else
						m_nBufUpSerial[1] = 0;
				}
				else
				{
					m_bLotEnd = TRUE;
					m_nLotEndAuto = LOT_END;
				}

				if (pDoc->WorkingInfo.LastJob.bSampleTest)
				{
					if (m_nBufUpSerial[0] == 1)
					{
						m_nLotEndSerial = _tstoi(pDoc->WorkingInfo.LastJob.sSampleTestShotNum);
						m_bLastProcFromUp = FALSE;
						m_bLastProcFromEng = FALSE;
						m_bLastProc = TRUE;
						if (m_pDlgMenu01)
							m_pDlgMenu01->m_bLastProc = TRUE;
						if (m_pMpe)
						{
							m_pMpe->Write(_T("MB440186"), 1);			// �ܷ�ó�� AOI(��) ����(PC�� On��Ű��, PLC�� Ȯ���ϰ� Off��Ŵ)-20141112
							m_pMpe->Write(_T("MB440181"), 1);			// �ܷ�ó��(PC�� On��Ű��, PLC�� Ȯ���ϰ� Off��Ŵ)-20141031
							pDoc->LogAuto(_T("PC: �ܷ�ó�� AOI(��) ����(PC�� On��Ű��, PLC�� Ȯ���ϰ� Off��Ŵ)"));
						}
					}
				}
			}
			break;
		case MK_ST + (Mk4PtIdx::Start) + 1:
			m_nMkStAuto++;
			break;
		}
	}
}

void CGvisR2R_PunchView::Mk4PtChkSerial()
{
	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;
	CString sNewLot;
	BOOL bNewModel = FALSE;
	int nSerial = 0;
	int nNewLot = 0;
	double dFdEnc;

	if (m_bMkSt)
	{
		switch (m_nMkStAuto)
		{
		case MK_ST + (Mk4PtIdx::ChkSn) :
			if (m_nBufUpSerial[0] == m_nBufUpSerial[1])
			{
				Stop();
				pView->ClrDispMsg();
				AfxMessageBox(_T("��/�� ��ŷ �ø����� �����ϴ�."));
				SetListBuf();
				m_nMkStAuto = MK_ST + (Mk4PtIdx::Start);
				break;
			}

			if (!m_bTHREAD_DISP_DEF)
			{
				m_nMkStAuto++;
				m_nStepTHREAD_DISP_DEF = 0;
				m_bTHREAD_DISP_DEF = TRUE;		// DispDefImg() : CopyDefImg Start
				pDoc->SetMkMenu01(_T("Signal"), _T("DispDefImg"), _T("1"));
			}
			break;

		case MK_ST + (Mk4PtIdx::ChkSn) + 1:
			m_nMkStAuto = MK_ST + (Mk4PtIdx::InitMk);			// InitMk()
			nSerial = m_nBufUpSerial[0];
			sNewLot = m_sNewLotUp;

			if (nSerial > 0)
			{
				if (pView->m_bSerialDecrese)
				{
					if (m_bLastProc && m_nBufUpSerial[1] < m_nLotEndSerial)
					{
						nSerial = m_nBufUpSerial[0]; // Test
					}
				}
				else
				{
					if (m_bLastProc && m_nBufUpSerial[1] > m_nLotEndSerial)
					{
						nSerial = m_nBufUpSerial[0]; // Test
					}
				}

				bNewModel = GetAoiUpInfo(nSerial, &nNewLot, TRUE);
				if (bDualTest)
				{
					bNewModel = GetAoiDnInfo(nSerial, &nNewLot, TRUE);

					if (!IsSameUpDnLot() && !m_bContDiffLot)
					{
						//sNewLot.Empty();
						m_nMkStAuto = MK_ST + (Mk4PtIdx::LotDiff);
						break;
					}
				}

				if (bNewModel)	// AOI ����(AoiCurrentInfoPath) -> AOI Feeding Offset
				{
					;
				}
				if (nNewLot)
				{
					// Lot Change.
					ChgLot();
#ifdef USE_MPE
					dFdEnc = (double)pDoc->m_pMpeData[0][0];	// ��ŷ�� Feeding ���ڴ� ��(���� mm )
					if ((pDoc->WorkingInfo.LastJob.bLotSep || pDoc->m_bDoneChgLot) && (dFdEnc + _tstof(pDoc->WorkingInfo.LastJob.sOnePnlLen)*2.0) > _tstof(pDoc->WorkingInfo.LastJob.sLotSepLen)*1000.0)
					{
						pDoc->m_bDoneChgLot = TRUE;
						SetPathAtBuf();
					}
#endif
				}
			}
			else
			{
				Stop();
				MsgBox(_T("����(��) Serial�� �����ʽ��ϴ�."));
				TowerLamp(RGB_YELLOW, TRUE);
			}
			//sNewLot.Empty();
			break;
		}
	}
}

void CGvisR2R_PunchView::Mk4PtInit()
{
	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;

	if (m_bMkSt)
	{
		switch (m_nMkStAuto)
		{
		case MK_ST + (Mk4PtIdx::InitMk) :
			if (InitMk())	// �ø����� ���� �Ǵ� ���� : TRUE �ƴϰ� ������ : FALSE
			{
				if (pView->m_bSerialDecrese)
				{
					if ((m_nBufUpSerial[0] <= m_nLotEndSerial || m_nBufUpSerial[1] <= m_nLotEndSerial) && m_nLotEndSerial > 0)
					{
						m_pMpe->Write(_T("MB440171"), 1); // ��ŷ�� �۾��Ϸ�.(PC�� On, PLC�� Ȯ�� �� Off) - 20160718
						pDoc->LogAuto(_T("PC: ��ŷ�� �۾��Ϸ�.(PC�� On, PLC�� Ȯ�� �� Off)"));
					}
				}
				else
				{
					if ((m_nBufUpSerial[0] >= m_nLotEndSerial || m_nBufUpSerial[1] >= m_nLotEndSerial) && m_nLotEndSerial > 0)
					{
						m_pMpe->Write(_T("MB440171"), 1); // ��ŷ�� �۾��Ϸ�.(PC�� On, PLC�� Ȯ�� �� Off) - 20160718
						pDoc->LogAuto(_T("PC: ��ŷ�� �۾��Ϸ�.(PC�� On, PLC�� Ȯ�� �� Off)"));
					}
				}
			}
			else
			{
				Stop();
				MsgBox(_T("Serial ���� �����ʽ��ϴ�."));
				TowerLamp(RGB_YELLOW, TRUE);
			}
			m_nMkStAuto++;
			break;

		case MK_ST + (Mk4PtIdx::InitMk) + 1:
			if (IsRun())
				m_nMkStAuto++;
			break;
		}
	}
}

void CGvisR2R_PunchView::Mk4PtAlignPt0()
{
	if (!IsRun())
		return;

	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;

	if (m_bMkSt)
	{
		switch (m_nMkStAuto)
		{
		case MK_ST + (Mk4PtIdx::Move0Cam1) :	// Move - Cam1 - Pt0
			if (bDualTest)
			{
				if (pView->m_bSerialDecrese)
				{
					if (m_bLastProc && m_nBufDnSerial[0] - 1 < m_nLotEndSerial)	// AOI�ϸ� Serial
					{
						m_bSkipAlign[1][0] = TRUE;
						m_bSkipAlign[1][1] = TRUE;
						m_bSkipAlign[1][2] = TRUE;
						m_bSkipAlign[1][3] = TRUE;
						m_bDoMk[1] = FALSE;
						m_bDoneMk[1] = TRUE;
						m_nMkStAuto++;
					}
					else
					{
						if (MoveAlign1(0))
							m_nMkStAuto++;
					}
				}
				else
				{
					if (m_bLastProc && m_nBufDnSerial[0] + 1 > m_nLotEndSerial)	// AOI�ϸ� Serial
					{
						m_bSkipAlign[1][0] = TRUE;
						m_bSkipAlign[1][1] = TRUE;
						m_bSkipAlign[1][2] = TRUE;
						m_bSkipAlign[1][3] = TRUE;
						m_bDoMk[1] = FALSE;
						m_bDoneMk[1] = TRUE;
						m_nMkStAuto++;
					}
					else
					{
						if (MoveAlign1(0))
							m_nMkStAuto++;
					}
				}
			}
			else
			{
				if (pView->m_bSerialDecrese)
				{
					if (m_bLastProc && m_nBufUpSerial[0] - 1 < m_nLotEndSerial)	// AOI��� Serial
					{
						m_bSkipAlign[1][0] = TRUE;
						m_bSkipAlign[1][1] = TRUE;
						m_bSkipAlign[1][2] = TRUE;
						m_bSkipAlign[1][3] = TRUE;
						m_bDoMk[1] = FALSE;
						m_bDoneMk[1] = TRUE;
						m_nMkStAuto++;
					}
					else
					{
						if (MoveAlign1(0))
							m_nMkStAuto++;
					}
				}
				else
				{
					if (m_bLastProc && m_nBufUpSerial[0] + 1 > m_nLotEndSerial)	// AOI��� Serial
					{
						m_bSkipAlign[1][0] = TRUE;
						m_bSkipAlign[1][1] = TRUE;
						m_bSkipAlign[1][2] = TRUE;
						m_bSkipAlign[1][3] = TRUE;
						m_bDoMk[1] = FALSE;
						m_bDoneMk[1] = TRUE;
						m_nMkStAuto++;
					}
					else
					{
						if (MoveAlign1(0))
							m_nMkStAuto++;
					}
				}
			}
			break;
		case MK_ST + (Mk4PtIdx::Move0Cam1) + 1:
			if (IsRun())
				m_nMkStAuto++;
			break;
		case MK_ST + (Mk4PtIdx::Move0Cam0) :	// Move - Cam0 - Pt0
			if (MoveAlign0(0))
				m_nMkStAuto++;
			break;
		case MK_ST + (Mk4PtIdx::Move0Cam0) + 1:
			if (IsRun())
				m_nMkStAuto++;
			break;
		case MK_ST + (Mk4PtIdx::Move0Cam0) + 2:
			if (IsMoveDone())
			{
				Sleep(100);
				m_nMkStAuto++;
			}
			break;
		case MK_ST + (Mk4PtIdx::Align1_0) :	// 4PtAlign - Cam1 - Pt0
			if (!m_bSkipAlign[1][0])
			{
				if (FourPointAlign1(0))
					m_bFailAlign[1][0] = FALSE;
				else
					m_bFailAlign[1][0] = TRUE;
			}
			m_nMkStAuto++;
			break;
		case MK_ST + (Mk4PtIdx::Align0_0) :	// 4PtAlign - Cam0 - Pt0
			if (!m_bSkipAlign[0][0])
			{
				if (FourPointAlign0(0))
					m_bFailAlign[0][0] = FALSE;
				else
					m_bFailAlign[0][0] = TRUE;
			}
			m_nMkStAuto++;
			break;
		case MK_ST + (Mk4PtIdx::Align0_0) + 1:
			if (m_bFailAlign[0][0])
			{
				Buzzer(TRUE, 0);
				TowerLamp(RGB_YELLOW, TRUE);

				//if(IDNO == DoMyMsgBox(_T("ī�޶�(��)�� �˻��ǳ��� �ٽ� �����Ͻðڽ��ϱ�?"), MB_YESNO))
				if (IDNO == MsgBox(_T("ī�޶�(��)�� �˻��ǳ��� �ٽ� �����Ͻðڽ��ϱ�?"), 0, MB_YESNO))
				{
					Buzzer(FALSE, 0);

					//if(IDYES == DoMyMsgBox(_T("ī�޶�(��)�� �˻��ǳ� ������ ���� ���Ͻðڽ��ϱ�?"), MB_YESNO))
					if (IDYES == MsgBox(_T("ī�޶�(��)�� �˻��ǳ� ������ ���� ���Ͻðڽ��ϱ�?"), 0, MB_YESNO))
					{
						m_bReAlign[0][0] = FALSE;
						m_bSkipAlign[0][0] = TRUE;
						m_bSkipAlign[0][1] = TRUE;
						m_bSkipAlign[0][2] = TRUE;
						m_bSkipAlign[0][3] = TRUE;
						//if(IDNO == DoMyMsgBox(_T("ī�޶�(��)�� �˻��ǳ��� �ҷ���ŷ �Ͻðڽ��ϱ�?"), MB_YESNO))
						if (IDNO == MsgBox(_T("ī�޶�(��)�� �˻��ǳ��� �ҷ���ŷ �Ͻðڽ��ϱ�?"), 0, MB_YESNO))
						{
							m_bDoMk[0] = FALSE;
							m_bDoneMk[0] = TRUE;
						}
						else
						{
							m_bDoMk[0] = TRUE;
							m_bDoneMk[0] = FALSE;
						}
					}
					else
					{
						m_bReAlign[0][0] = TRUE;
						m_bSkipAlign[0][0] = FALSE;
						m_bSkipAlign[0][1] = FALSE;
						m_bSkipAlign[0][2] = FALSE;
						m_bSkipAlign[0][3] = FALSE;
						//m_nMkStAuto = MK_ST + (Mk4PtIdx::Align0_0); // FourPointAlign0(0) ���� ����. - ī�޶� ������
						m_nMkStAuto = MK_ST + (Mk4PtIdx::Move0Cam0); // FourPointAlign0(0) ���� ����. - ī�޶� ������
						Stop();
						//TowerLamp(RGB_YELLOW, TRUE);
					}
				}
				else
				{
					Buzzer(FALSE, 0);

					m_bReAlign[0][0] = TRUE;
					m_bSkipAlign[0][0] = FALSE;
					m_bSkipAlign[0][1] = FALSE;
					//m_nMkStAuto = MK_ST + (Mk4PtIdx::Align0_0); // FourPointAlign0(0) ���� ����. - ī�޶� ������
					m_nMkStAuto = MK_ST + (Mk4PtIdx::Move0Cam0); // FourPointAlign0(0) ���� ����. - ī�޶� ������
					Stop();
					//TowerLamp(RGB_YELLOW, TRUE);
				}
			}
			if (m_bFailAlign[1][0])
			{
				Buzzer(TRUE, 0);
				TowerLamp(RGB_YELLOW, TRUE);

				//if(IDNO == DoMyMsgBox(_T("ī�޶�(��)�� �˻��ǳ��� �ٽ� �����Ͻðڽ��ϱ�?"), MB_YESNO))
				if (IDNO == MsgBox(_T("ī�޶�(��)�� �˻��ǳ��� �ٽ� �����Ͻðڽ��ϱ�?"), 0, MB_YESNO))
				{
					Buzzer(FALSE, 0);

					//if(IDYES == DoMyMsgBox(_T("ī�޶�(��)�� �˻��ǳ� ������ ���� ���Ͻðڽ��ϱ�?"), MB_YESNO))
					if (IDYES == MsgBox(_T("ī�޶�(��)�� �˻��ǳ� ������ ���� ���Ͻðڽ��ϱ�?"), 0, MB_YESNO))
					{
						m_bReAlign[1][0] = FALSE;
						m_bSkipAlign[1][0] = TRUE;
						m_bSkipAlign[1][1] = TRUE;
						m_bSkipAlign[1][2] = TRUE;
						m_bSkipAlign[1][3] = TRUE;
						//if(IDNO == DoMyMsgBox(_T("ī�޶�(��)�� �˻��ǳ��� �ҷ���ŷ �Ͻðڽ��ϱ�?"), MB_YESNO))
						if (IDNO == MsgBox(_T("ī�޶�(��)�� �˻��ǳ��� �ҷ���ŷ �Ͻðڽ��ϱ�?"), 0, MB_YESNO))
						{
							m_bDoMk[1] = FALSE;
							m_bDoneMk[1] = TRUE;
						}
						else
						{
							m_bDoMk[1] = TRUE;
							m_bDoneMk[1] = FALSE;
						}
					}
					else
					{
						m_bReAlign[1][0] = TRUE;
						m_bSkipAlign[1][0] = FALSE;
						m_bSkipAlign[1][1] = FALSE;
						m_bSkipAlign[1][2] = FALSE;
						m_bSkipAlign[1][3] = FALSE;
						//m_nMkStAuto = MK_ST + (Mk4PtIdx::Align1_0); // FourPointAlign1(0) ���� ����. - ī�޶� ������
						m_nMkStAuto = MK_ST + (Mk4PtIdx::Move0Cam1); // FourPointAlign1(0) ���� ����. - ī�޶� ������
						Stop();
						//TowerLamp(RGB_YELLOW, TRUE);
					}
				}
				else
				{
					Buzzer(FALSE, 0);

					m_bReAlign[1][0] = TRUE;
					m_bSkipAlign[1][0] = FALSE;
					m_bSkipAlign[1][1] = FALSE;
					m_bSkipAlign[1][2] = FALSE;
					m_bSkipAlign[1][3] = FALSE;
					//m_nMkStAuto = MK_ST + (Mk4PtIdx::Align1_0); // FourPointAlign1(0) ���� ����. - ī�޶� ������
					m_nMkStAuto = MK_ST + (Mk4PtIdx::Move0Cam1); // FourPointAlign1(0) ���� ����. - ī�޶� ������
					Stop();
					//TowerLamp(RGB_YELLOW, TRUE);
				}
			}

			if (m_bFailAlign[0][0] || m_bFailAlign[1][0])
			{
				if (!m_bReAlign[0][0] && !m_bReAlign[1][0])
				{
					if (m_bDoMk[0] || m_bDoMk[1])
						m_nMkStAuto++; //m_nMkStAuto = MK_ST + 27; // MoveInitPos0()
					else
					{
						if (!IsInitPos0())
							MoveInitPos0();
						if (!IsInitPos1())
							MoveInitPos1();

						m_nMkStAuto = MK_ST + (Mk4PtIdx::DoneMk); // // �ҷ��̹��� Display, Align���� �ʱ�ȭ (Skip 65 : Mk())
					}
				}
				else
				{
					//m_nMkStAuto = MK_ST + (Mk4PtIdx::Align1_0); // TwoPointAlign1(0) ���� ����. - ī�޶� ������
					m_nMkStAuto = MK_ST + (Mk4PtIdx::Move0Cam1); // TwoPointAlign1(0) ���� ����. - ī�޶� ������
				}
			}
			else
				m_nMkStAuto++;

			break;
		case MK_ST + (Mk4PtIdx::Align0_0) + 2:
			if (IsRun())
				m_nMkStAuto++;
			break;
		}
	}
}

void CGvisR2R_PunchView::Mk4PtAlignPt1()
{
	if (!IsRun())
		return;

	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;

	if (m_bMkSt)
	{
		switch (m_nMkStAuto)
		{
		case MK_ST + (Mk4PtIdx::Move1Cam1) :
			if (bDualTest)
			{
				if (pView->m_bSerialDecrese)
				{
					if (m_bLastProc && m_nBufDnSerial[0] - 1 < m_nLotEndSerial)	// AOI�ϸ� Serial
					{
						m_bSkipAlign[1][0] = TRUE;
						m_bSkipAlign[1][1] = TRUE;
						m_bSkipAlign[1][2] = TRUE;
						m_bSkipAlign[1][3] = TRUE;
						m_bDoMk[1] = FALSE;
						m_bDoneMk[1] = TRUE;
						m_nMkStAuto++;
					}
					else
					{
						if (MoveAlign1(1))	// Move - Cam1 - Pt1
							m_nMkStAuto++;
					}
				}
				else
				{
					if (m_bLastProc && m_nBufDnSerial[0] + 1 > m_nLotEndSerial)	// AOI�ϸ� Serial
					{
						m_bSkipAlign[1][0] = TRUE;
						m_bSkipAlign[1][1] = TRUE;
						m_bSkipAlign[1][2] = TRUE;
						m_bSkipAlign[1][3] = TRUE;
						m_bDoMk[1] = FALSE;
						m_bDoneMk[1] = TRUE;
						m_nMkStAuto++;
					}
					else
					{
						if (MoveAlign1(1))	// Move - Cam1 - Pt1
							m_nMkStAuto++;
					}
				}
			}
			else
			{
				if (pView->m_bSerialDecrese)
				{
					if (m_bLastProc && m_nBufUpSerial[0] - 1 < m_nLotEndSerial)	// AOI��� Serial
					{
						m_bSkipAlign[1][0] = TRUE;
						m_bSkipAlign[1][1] = TRUE;
						m_bSkipAlign[1][2] = TRUE;
						m_bSkipAlign[1][3] = TRUE;
						m_bDoMk[1] = FALSE;
						m_bDoneMk[1] = TRUE;
						m_nMkStAuto++;
					}
					else
					{
						if (MoveAlign1(1))	// Move - Cam1 - Pt1
							m_nMkStAuto++;
					}
				}
				else
				{
					if (m_bLastProc && m_nBufUpSerial[0] + 1 > m_nLotEndSerial)	// AOI��� Serial
					{
						m_bSkipAlign[1][0] = TRUE;
						m_bSkipAlign[1][1] = TRUE;
						m_bSkipAlign[1][2] = TRUE;
						m_bSkipAlign[1][3] = TRUE;
						m_bDoMk[1] = FALSE;
						m_bDoneMk[1] = TRUE;
						m_nMkStAuto++;
					}
					else
					{
						if (MoveAlign1(1))	// Move - Cam1 - Pt1
							m_nMkStAuto++;
					}
				}
			}
										   break;
		case MK_ST + (Mk4PtIdx::Move1Cam1) + 1:
			if (IsRun())
				m_nMkStAuto++;
			break;
		case MK_ST + (Mk4PtIdx::Move1Cam0) :
			if (MoveAlign0(1))	// Move - Cam0 - Pt1
				m_nMkStAuto++;
			break;
		case MK_ST + (Mk4PtIdx::Move1Cam0) + 1:
			if (IsRun())
				m_nMkStAuto++;
			break;
		case MK_ST + (Mk4PtIdx::Move1Cam0) + 2:
			if (IsMoveDone())
			{ 
				Sleep(100);
				m_nMkStAuto++;
			}
			break;
		case MK_ST + (Mk4PtIdx::Align1_1) :	// 4PtAlign - Cam1 - Pt1
			if (!m_bFailAlign[1][0])
			{
				if (!m_bSkipAlign[1][1])
				{
					if (!FourPointAlign1(1))
						m_bFailAlign[1][1] = TRUE;
					else
						m_bFailAlign[1][1] = FALSE;
				}
				else
					m_bFailAlign[1][1] = FALSE;
			}
			else
				m_bFailAlign[1][1] = FALSE;

			m_nMkStAuto++;
			break;
		case MK_ST + (Mk4PtIdx::Align0_1) :	// 4PtAlign - Cam0 - Pt1
			if (!m_bFailAlign[0][0])
			{
				if (!m_bSkipAlign[0][1])
				{
					if (!FourPointAlign0(1))
						m_bFailAlign[0][1] = TRUE;
					else
						m_bFailAlign[0][1] = FALSE;
				}
				else
					m_bFailAlign[0][1] = FALSE;
			}
			else
				m_bFailAlign[0][1] = FALSE;

			m_nMkStAuto++;
			break;
		case MK_ST + (Mk4PtIdx::Align0_1) + 1:
			if (m_bFailAlign[0][1])
			{
				Buzzer(TRUE, 0);
				TowerLamp(RGB_YELLOW, TRUE);

				//if(IDNO == DoMyMsgBox(_T("ī�޶�(��)�� �˻��ǳ��� �ٽ� �����Ͻðڽ��ϱ�?"), MB_YESNO))
				if (IDNO == MsgBox(_T("ī�޶�(��)�� �˻��ǳ��� �ٽ� �����Ͻðڽ��ϱ�?"), 0, MB_YESNO))
				{
					Buzzer(FALSE, 0);

					//if(IDYES == DoMyMsgBox(_T("ī�޶�(��)�� �˻��ǳ� ������ ���� ���Ͻðڽ��ϱ�?"), MB_YESNO))
					if (IDYES == MsgBox(_T("ī�޶�(��)�� �˻��ǳ� ������ ���� ���Ͻðڽ��ϱ�?"), 0, MB_YESNO))
					{
						m_bReAlign[0][1] = FALSE;
						m_bSkipAlign[0][1] = TRUE;
						//if(IDNO == DoMyMsgBox(_T("ī�޶�(��)�� �˻��ǳ��� �ҷ���ŷ �Ͻðڽ��ϱ�?"), MB_YESNO))
						if (IDNO == MsgBox(_T("ī�޶�(��)�� �˻��ǳ��� �ҷ���ŷ �Ͻðڽ��ϱ�?"), 0, MB_YESNO))
						{
							m_bDoMk[0] = FALSE;
							m_bDoneMk[0] = TRUE;
						}
						else
						{
							m_bDoMk[0] = TRUE;
							m_bDoneMk[0] = FALSE;
						}
					}
					else
					{
						m_bReAlign[0][1] = TRUE;
						m_bSkipAlign[0][1] = FALSE;
						//m_nMkStAuto = MK_ST + (Mk4PtIdx::Align0_1); // FourPointAlign0(1) ���� ����. - ī�޶� ������
						m_nMkStAuto = MK_ST + (Mk4PtIdx::Move1Cam0); // FourPointAlign0(1) ���� ����. - ī�޶� ������
						Stop();
						//TowerLamp(RGB_YELLOW, TRUE);
					}
				}
				else
				{
					Buzzer(FALSE, 0);

					m_bReAlign[0][1] = TRUE;
					m_bSkipAlign[0][1] = FALSE;
					//m_nMkStAuto = MK_ST + (Mk4PtIdx::Align0_1); // FourPointAlign1(1) ���� ����. - ī�޶� ������
					m_nMkStAuto = MK_ST + (Mk4PtIdx::Move1Cam0); // FourPointAlign1(1) ���� ����. - ī�޶� ������
					Stop();
					//TowerLamp(RGB_YELLOW, TRUE);
				}
			}
			if (m_bFailAlign[1][1])
			{
				Buzzer(TRUE, 0);
				TowerLamp(RGB_YELLOW, TRUE);

				//if(IDNO == DoMyMsgBox(_T("ī�޶�(��)�� �˻��ǳ��� �ٽ� �����Ͻðڽ��ϱ�?"), MB_YESNO))
				if (IDNO == MsgBox(_T("ī�޶�(��)�� �˻��ǳ��� �ٽ� �����Ͻðڽ��ϱ�?"), 0, MB_YESNO))
				{
					Buzzer(FALSE, 0);

					//if(IDYES == DoMyMsgBox(_T("ī�޶�(��)�� �˻��ǳ� ������ ���� ���Ͻðڽ��ϱ�?"), MB_YESNO))
					if (IDYES == MsgBox(_T("ī�޶�(��)�� �˻��ǳ� ������ ���� ���Ͻðڽ��ϱ�?"), 0, MB_YESNO))
					{
						m_bReAlign[1][1] = FALSE;
						m_bSkipAlign[1][1] = TRUE;
						//if(IDNO == DoMyMsgBox(_T("ī�޶�(��)�� �˻��ǳ��� �ҷ���ŷ �Ͻðڽ��ϱ�?"), MB_YESNO))
						if (IDNO == MsgBox(_T("ī�޶�(��)�� �˻��ǳ��� �ҷ���ŷ �Ͻðڽ��ϱ�?"), 0, MB_YESNO))
						{
							m_bDoMk[1] = FALSE;
							m_bDoneMk[1] = TRUE;
						}
						else
						{
							m_bDoMk[1] = TRUE;
							m_bDoneMk[1] = FALSE;
						}
					}
					else
					{
						m_bReAlign[1][1] = TRUE;
						m_bSkipAlign[1][1] = FALSE;
						//m_nMkStAuto = MK_ST + (Mk4PtIdx::Align1_1); // FourPointAlign1(1) ���� ����. - ī�޶� ������
						m_nMkStAuto = MK_ST + (Mk4PtIdx::Move1Cam1); // FourPointAlign1(1) ���� ����. - ī�޶� ������
						Stop();
						//TowerLamp(RGB_YELLOW, TRUE);
					}
				}
				else
				{
					Buzzer(FALSE, 0);

					m_bReAlign[1][1] = TRUE;
					m_bSkipAlign[1][1] = FALSE;
					//m_nMkStAuto = MK_ST + (Mk4PtIdx::Align1_1); // FourPointAlign1(1) ���� ����. - ī�޶� ������
					m_nMkStAuto = MK_ST + (Mk4PtIdx::Move1Cam1); // FourPointAlign1(1) ���� ����. - ī�޶� ������
					Stop();
					//TowerLamp(RGB_YELLOW, TRUE);
				}
			}

			if (m_bFailAlign[0][1] || m_bFailAlign[1][1])
			{
				if (!m_bReAlign[0][1] && !m_bReAlign[1][1])
				{
					if (m_bDoMk[0] || m_bDoMk[1])
						m_nMkStAuto++;//m_nMkStAuto = MK_ST + 29;  // MoveInitPos0()
					else
					{
						if (!IsInitPos0())
							MoveInitPos0();
						if (!IsInitPos1())
							MoveInitPos1();

						m_nMkStAuto = MK_ST + (Mk4PtIdx::DoneMk); // // �ҷ��̹��� Display, Align���� �ʱ�ȭ (Skip 65 : Mk())
					}
				}
				else
					m_nMkStAuto = MK_ST + (Mk4PtIdx::Move1Cam1); // FourPointAlign1(1) ���� ����. - ī�޶� ������
					//m_nMkStAuto = MK_ST + (Mk4PtIdx::Align1_1); // FourPointAlign1(1) ���� ����. - ī�޶� ������
			}
			else
				m_nMkStAuto++;

			break;
		}
	}
}

void CGvisR2R_PunchView::Mk4PtAlignPt2()
{
	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;

	if (m_bMkSt)
	{
		switch (m_nMkStAuto)
		{
		case MK_ST + (Mk4PtIdx::Move2Cam1) :
			if (bDualTest)
			{
				if (pView->m_bSerialDecrese)
				{
					if (m_bLastProc && m_nBufDnSerial[0] - 1 < m_nLotEndSerial)	// AOI�ϸ� Serial
					{
						m_bSkipAlign[1][0] = TRUE;
						m_bSkipAlign[1][1] = TRUE;
						m_bSkipAlign[1][2] = TRUE;
						m_bSkipAlign[1][3] = TRUE;
						m_bDoMk[1] = FALSE;
						m_bDoneMk[1] = TRUE;
						m_nMkStAuto++;
					}
					else
					{
						if (MoveAlign1(2))	// Move - Cam1 - Pt2
							m_nMkStAuto++;
					}
				}
				else
				{
					if (m_bLastProc && m_nBufDnSerial[0] + 1 > m_nLotEndSerial)	// AOI�ϸ� Serial
					{
						m_bSkipAlign[1][0] = TRUE;
						m_bSkipAlign[1][1] = TRUE;
						m_bSkipAlign[1][2] = TRUE;
						m_bSkipAlign[1][3] = TRUE;
						m_bDoMk[1] = FALSE;
						m_bDoneMk[1] = TRUE;
						m_nMkStAuto++;
					}
					else
					{
						if (MoveAlign1(2))	// Move - Cam1 - Pt2
							m_nMkStAuto++;
					}
				}
			}
			else
			{
				if (pView->m_bSerialDecrese)
				{
					if (m_bLastProc && m_nBufUpSerial[0] - 1 < m_nLotEndSerial)	// AOI��� Serial
					{
						m_bSkipAlign[1][0] = TRUE;
						m_bSkipAlign[1][1] = TRUE;
						m_bSkipAlign[1][2] = TRUE;
						m_bSkipAlign[1][3] = TRUE;
						m_bDoMk[1] = FALSE;
						m_bDoneMk[1] = TRUE;
						m_nMkStAuto++;
					}
					else
					{
						if (MoveAlign1(2))	// Move - Cam1 - Pt2
							m_nMkStAuto++;
					}
				}
				else
				{
					if (m_bLastProc && m_nBufUpSerial[0] + 1 > m_nLotEndSerial)	// AOI��� Serial
					{
						m_bSkipAlign[1][0] = TRUE;
						m_bSkipAlign[1][1] = TRUE;
						m_bSkipAlign[1][2] = TRUE;
						m_bSkipAlign[1][3] = TRUE;
						m_bDoMk[1] = FALSE;
						m_bDoneMk[1] = TRUE;
						m_nMkStAuto++;
					}
					else
					{
						if (MoveAlign1(2))	// Move - Cam1 - Pt2
							m_nMkStAuto++;
					}
				}
			}
			break;
		case MK_ST + (Mk4PtIdx::Move2Cam1) + 1:
			if (IsRun())
				m_nMkStAuto++;
			break;
		case MK_ST + (Mk4PtIdx::Move2Cam0) :
			if (MoveAlign0(2))	// Move - Cam0 - Pt2
				m_nMkStAuto++;
			break;
		case MK_ST + (Mk4PtIdx::Move2Cam0) + 1:
			if (IsRun())
				m_nMkStAuto++;
			break;
		case MK_ST + (Mk4PtIdx::Move2Cam0) + 2:
			if (IsMoveDone())
			{
				Sleep(100);
				m_nMkStAuto++;
			}
			break;
		case MK_ST + (Mk4PtIdx::Align1_2) :	// 4PtAlign - Cam1 - Pt2
			if (!m_bFailAlign[1][0])
			{
				if (!m_bSkipAlign[1][1])
				{
					if (!m_bSkipAlign[1][2])
					{
						if (!FourPointAlign1(2))
							m_bFailAlign[1][2] = TRUE;
						else
							m_bFailAlign[1][2] = FALSE;
					}
					else
						m_bFailAlign[1][2] = FALSE;
				}
				else
					m_bFailAlign[1][2] = FALSE;
			}
			else
				m_bFailAlign[1][2] = FALSE;

			m_nMkStAuto++;
			break;
		case MK_ST + (Mk4PtIdx::Align0_2) :	// 4PtAlign - Cam0 - Pt2
			if (!m_bFailAlign[0][0])
			{
				if (!m_bSkipAlign[0][1])
				{
					if (!m_bSkipAlign[0][2])
					{
						if (!FourPointAlign0(2))
							m_bFailAlign[0][2] = TRUE;
						else
							m_bFailAlign[0][2] = FALSE;
					}
					else
						m_bFailAlign[0][2] = FALSE;
				}
				else
					m_bFailAlign[0][2] = FALSE;
			}
			else
				m_bFailAlign[0][2] = FALSE;

			m_nMkStAuto++;
			break;
		case MK_ST + (Mk4PtIdx::Align0_2) + 1:
			if (m_bFailAlign[0][2])
			{
				Buzzer(TRUE, 0);
				TowerLamp(RGB_YELLOW, TRUE);

				//if(IDNO == DoMyMsgBox(_T("ī�޶�(��)�� �˻��ǳ��� �ٽ� �����Ͻðڽ��ϱ�?"), MB_YESNO))
				if (IDNO == MsgBox(_T("ī�޶�(��)�� �˻��ǳ��� �ٽ� �����Ͻðڽ��ϱ�?"), 0, MB_YESNO))
				{
					Buzzer(FALSE, 0);

					//if(IDYES == DoMyMsgBox(_T("ī�޶�(��)�� �˻��ǳ� ������ ���� ���Ͻðڽ��ϱ�?"), MB_YESNO))
					if (IDYES == MsgBox(_T("ī�޶�(��)�� �˻��ǳ� ������ ���� ���Ͻðڽ��ϱ�?"), 0, MB_YESNO))
					{
						m_bReAlign[0][2] = FALSE;
						m_bSkipAlign[0][2] = TRUE;
						//if(IDNO == DoMyMsgBox(_T("ī�޶�(��)�� �˻��ǳ��� �ҷ���ŷ �Ͻðڽ��ϱ�?"), MB_YESNO))
						if (IDNO == MsgBox(_T("ī�޶�(��)�� �˻��ǳ��� �ҷ���ŷ �Ͻðڽ��ϱ�?"), 0, MB_YESNO))
						{
							m_bDoMk[0] = FALSE;
							m_bDoneMk[0] = TRUE;
						}
						else
						{
							m_bDoMk[0] = TRUE;
							m_bDoneMk[0] = FALSE;
						}
					}
					else
					{
						m_bReAlign[0][2] = TRUE;
						m_bSkipAlign[0][2] = FALSE;
						//m_nMkStAuto = MK_ST + (Mk4PtIdx::Align0_2); // FourPointAlign0(2) ���� ����. - ī�޶� ������
						m_nMkStAuto = MK_ST + (Mk4PtIdx::Move2Cam0); // FourPointAlign0(2) ���� ����. - ī�޶� ������
						Stop();
						//TowerLamp(RGB_YELLOW, TRUE);
					}
				}
				else
				{
					Buzzer(FALSE, 0);

					m_bReAlign[0][2] = TRUE;
					m_bSkipAlign[0][2] = FALSE;
					//m_nMkStAuto = MK_ST + (Mk4PtIdx::Align0_2); // FourPointAlign0(2) ���� ����. - ī�޶� ������
					m_nMkStAuto = MK_ST + (Mk4PtIdx::Move2Cam0); // FourPointAlign0(2) ���� ����. - ī�޶� ������
					Stop();
					//TowerLamp(RGB_YELLOW, TRUE);
				}
			}
			if (m_bFailAlign[1][2])
			{
				Buzzer(TRUE, 0);
				TowerLamp(RGB_YELLOW, TRUE);

				//if(IDNO == DoMyMsgBox(_T("ī�޶�(��)�� �˻��ǳ��� �ٽ� �����Ͻðڽ��ϱ�?"), MB_YESNO))
				if (IDNO == MsgBox(_T("ī�޶�(��)�� �˻��ǳ��� �ٽ� �����Ͻðڽ��ϱ�?"), 0, MB_YESNO))
				{
					Buzzer(FALSE, 0);

					//if(IDYES == DoMyMsgBox(_T("ī�޶�(��)�� �˻��ǳ� ������ ���� ���Ͻðڽ��ϱ�?"), MB_YESNO))
					if (IDYES == MsgBox(_T("ī�޶�(��)�� �˻��ǳ� ������ ���� ���Ͻðڽ��ϱ�?"), 0, MB_YESNO))
					{
						m_bReAlign[1][2] = FALSE;
						m_bSkipAlign[1][2] = TRUE;
						//if(IDNO == DoMyMsgBox(_T("ī�޶�(��)�� �˻��ǳ��� �ҷ���ŷ �Ͻðڽ��ϱ�?"), MB_YESNO))
						if (IDNO == MsgBox(_T("ī�޶�(��)�� �˻��ǳ��� �ҷ���ŷ �Ͻðڽ��ϱ�?"), 0, MB_YESNO))
						{
							m_bDoMk[1] = FALSE;
							m_bDoneMk[1] = TRUE;
						}
						else
						{
							m_bDoMk[1] = TRUE;
							m_bDoneMk[1] = FALSE;
						}
					}
					else
					{
						m_bReAlign[1][2] = TRUE;
						m_bSkipAlign[1][2] = FALSE;
						//m_nMkStAuto = MK_ST + (Mk4PtIdx::Align1_2); // FourPointAlign1(2) ���� ����. - ī�޶� ������
						m_nMkStAuto = MK_ST + (Mk4PtIdx::Move2Cam1); // FourPointAlign1(2) ���� ����. - ī�޶� ������
						Stop();
						//TowerLamp(RGB_YELLOW, TRUE);
					}
				}
				else
				{
					Buzzer(FALSE, 0);

					m_bReAlign[1][2] = TRUE;
					m_bSkipAlign[1][2] = FALSE;
					//m_nMkStAuto = MK_ST + (Mk4PtIdx::Align1_2); // FourPointAlign1(2) ���� ����. - ī�޶� ������
					m_nMkStAuto = MK_ST + (Mk4PtIdx::Move2Cam1); // FourPointAlign1(2) ���� ����. - ī�޶� ������
					Stop();
					//TowerLamp(RGB_YELLOW, TRUE);
				}
			}

			if (m_bFailAlign[0][2] || m_bFailAlign[1][2])
			{
				if (!m_bReAlign[0][2] && !m_bReAlign[1][2])
				{
					if (m_bDoMk[0] || m_bDoMk[1])
						m_nMkStAuto++;//m_nMkStAuto = MK_ST + 29;  // MoveInitPos0()
					else
					{
						if (!IsInitPos0())
							MoveInitPos0();
						if (!IsInitPos1())
							MoveInitPos1();

						m_nMkStAuto = MK_ST + (Mk4PtIdx::DoneMk); // // �ҷ��̹��� Display, Align���� �ʱ�ȭ (Skip 65 : Mk())
					}
				}
				else
					m_nMkStAuto = MK_ST + (Mk4PtIdx::Move2Cam1); // FourPointAlign1(2) ���� ����. - ī�޶� ������
					//m_nMkStAuto = MK_ST + (Mk4PtIdx::Align1_2); // FourPointAlign1(2) ���� ����. - ī�޶� ������
			}
			else
				m_nMkStAuto++;

			break;
		}
	}
}

void CGvisR2R_PunchView::Mk4PtAlignPt3()
{
	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;

	if (m_bMkSt)
	{
		switch (m_nMkStAuto)
		{
		case MK_ST + (Mk4PtIdx::Move3Cam1) :
			if (bDualTest)
			{
				if (pView->m_bSerialDecrese)
				{
					if (m_bLastProc && m_nBufDnSerial[0] - 1 < m_nLotEndSerial)	// AOI�ϸ� Serial
					{
						m_bSkipAlign[1][0] = TRUE;
						m_bSkipAlign[1][1] = TRUE;
						m_bSkipAlign[1][2] = TRUE;
						m_bSkipAlign[1][3] = TRUE;
						m_bDoMk[1] = FALSE;
						m_bDoneMk[1] = TRUE;
						m_nMkStAuto++;
					}
					else
					{
						if (MoveAlign1(3))	// Move - Cam1 - Pt3
							m_nMkStAuto++;
					}
				}
				else
				{
					if (m_bLastProc && m_nBufDnSerial[0] + 1 > m_nLotEndSerial)	// AOI�ϸ� Serial
					{
						m_bSkipAlign[1][0] = TRUE;
						m_bSkipAlign[1][1] = TRUE;
						m_bSkipAlign[1][2] = TRUE;
						m_bSkipAlign[1][3] = TRUE;
						m_bDoMk[1] = FALSE;
						m_bDoneMk[1] = TRUE;
						m_nMkStAuto++;
					}
					else
					{
						if (MoveAlign1(3))	// Move - Cam1 - Pt3
							m_nMkStAuto++;
					}
				}
			}
			else
			{
				if (pView->m_bSerialDecrese)
				{
					if (m_bLastProc && m_nBufUpSerial[0] - 1 < m_nLotEndSerial)	// AOI��� Serial
					{
						m_bSkipAlign[1][0] = TRUE;
						m_bSkipAlign[1][1] = TRUE;
						m_bSkipAlign[1][2] = TRUE;
						m_bSkipAlign[1][3] = TRUE;
						m_bDoMk[1] = FALSE;
						m_bDoneMk[1] = TRUE;
						m_nMkStAuto++;
					}
					else
					{
						if (MoveAlign1(3))	// Move - Cam1 - Pt3
							m_nMkStAuto++;
					}
				}
				else
				{
					if (m_bLastProc && m_nBufUpSerial[0] + 1 > m_nLotEndSerial)	// AOI��� Serial
					{
						m_bSkipAlign[1][0] = TRUE;
						m_bSkipAlign[1][1] = TRUE;
						m_bSkipAlign[1][2] = TRUE;
						m_bSkipAlign[1][3] = TRUE;
						m_bDoMk[1] = FALSE;
						m_bDoneMk[1] = TRUE;
						m_nMkStAuto++;
					}
					else
					{
						if (MoveAlign1(3))	// Move - Cam1 - Pt3
							m_nMkStAuto++;
					}
				}
			}
			break;
		case MK_ST + (Mk4PtIdx::Move3Cam1) + 1:
			if (IsRun())
				m_nMkStAuto++;
			break;
		case MK_ST + (Mk4PtIdx::Move3Cam0) :
			if (MoveAlign0(3))	// Move - Cam0 - Pt3
				m_nMkStAuto++;
			break;
		case MK_ST + (Mk4PtIdx::Move3Cam0) + 1:
			if (IsRun())
				m_nMkStAuto++;
			break;
		case MK_ST + (Mk4PtIdx::Move3Cam0) + 2:
			if (IsMoveDone())
			{
				Sleep(100);
				m_nMkStAuto++;
			}
			break;
		case MK_ST + (Mk4PtIdx::Align1_3) :	// 4PtAlign - Cam1 - Pt3
			if (!m_bFailAlign[1][0])
			{
				if (!m_bSkipAlign[1][1])
				{
					if (!m_bSkipAlign[1][2])
					{
						if (!m_bSkipAlign[1][3])
						{
							if (!FourPointAlign1(3))
								m_bFailAlign[1][3] = TRUE;
							else
								m_bFailAlign[1][3] = FALSE;
						}
						else
							m_bFailAlign[1][3] = FALSE;
					}
					else
						m_bFailAlign[1][3] = FALSE;
				}
				else
					m_bFailAlign[1][3] = FALSE;
			}
			else
				m_bFailAlign[1][3] = FALSE;

			m_nMkStAuto++;
			break;
		case MK_ST + (Mk4PtIdx::Align0_3) :	// 4PtAlign - Cam0 - Pt3
			if (!m_bFailAlign[0][0])
			{
				if (!m_bSkipAlign[0][1])
				{
					if (!m_bSkipAlign[0][2])
					{
						if (!m_bSkipAlign[0][3])
						{
							if (!FourPointAlign0(3))
								m_bFailAlign[0][3] = TRUE;
							else
								m_bFailAlign[0][3] = FALSE;
						}
						else
							m_bFailAlign[0][3] = FALSE;
					}
					else
						m_bFailAlign[0][3] = FALSE;
				}
				else
					m_bFailAlign[0][3] = FALSE;
			}
			else
				m_bFailAlign[0][3] = FALSE;

			m_nMkStAuto++;
			break;
		case MK_ST + (Mk4PtIdx::Align0_3) + 1:
			if (m_bFailAlign[0][3])
			{
				Buzzer(TRUE, 0);

				//if(IDNO == DoMyMsgBox(_T("ī�޶�(��)�� �˻��ǳ��� �ٽ� �����Ͻðڽ��ϱ�?"), MB_YESNO))
				if (IDNO == MsgBox(_T("ī�޶�(��)�� �˻��ǳ��� �ٽ� �����Ͻðڽ��ϱ�?"), 0, MB_YESNO))
				{
					Buzzer(FALSE, 0);

					//if(IDYES == DoMyMsgBox(_T("ī�޶�(��)�� �˻��ǳ� ������ ���� ���Ͻðڽ��ϱ�?"), MB_YESNO))
					if (IDYES == MsgBox(_T("ī�޶�(��)�� �˻��ǳ� ������ ���� ���Ͻðڽ��ϱ�?"), 0, MB_YESNO))
					{
						m_bReAlign[0][3] = FALSE;
						m_bSkipAlign[0][3] = TRUE;
						//if(IDNO == DoMyMsgBox(_T("ī�޶�(��)�� �˻��ǳ��� �ҷ���ŷ �Ͻðڽ��ϱ�?"), MB_YESNO))
						if (IDNO == MsgBox(_T("ī�޶�(��)�� �˻��ǳ��� �ҷ���ŷ �Ͻðڽ��ϱ�?"), 0, MB_YESNO))
						{
							m_bDoMk[0] = FALSE;
							m_bDoneMk[0] = TRUE;
						}
						else
						{
							m_bDoMk[0] = TRUE;
							m_bDoneMk[0] = FALSE;
						}
					}
					else
					{
						m_bReAlign[0][3] = TRUE;
						m_bSkipAlign[0][3] = FALSE;
						//m_nMkStAuto = MK_ST + (Mk4PtIdx::Align0_3); // FourPointAlign0(3) ���� ����. - ī�޶� ������
						m_nMkStAuto = MK_ST + (Mk4PtIdx::Move3Cam0); // FourPointAlign0(3) ���� ����. - ī�޶� ������
						Stop();
						TowerLamp(RGB_YELLOW, TRUE);
					}
				}
				else
				{
					Buzzer(FALSE, 0);

					m_bReAlign[0][3] = TRUE;
					m_bSkipAlign[0][3] = FALSE;
					//m_nMkStAuto = MK_ST + (Mk4PtIdx::Align0_3); // FourPointAlign0(3) ���� ����. - ī�޶� ������
					m_nMkStAuto = MK_ST + (Mk4PtIdx::Move3Cam0); // FourPointAlign0(3) ���� ����. - ī�޶� ������
					Stop();
					TowerLamp(RGB_YELLOW, TRUE);
				}
			}
			if (m_bFailAlign[1][3])
			{
				Buzzer(TRUE, 0);

				//if(IDNO == DoMyMsgBox(_T("ī�޶�(��)�� �˻��ǳ��� �ٽ� �����Ͻðڽ��ϱ�?"), MB_YESNO))
				if (IDNO == MsgBox(_T("ī�޶�(��)�� �˻��ǳ��� �ٽ� �����Ͻðڽ��ϱ�?"), 0, MB_YESNO))
				{
					Buzzer(FALSE, 0);

					//if(IDYES == DoMyMsgBox(_T("ī�޶�(��)�� �˻��ǳ� ������ ���� ���Ͻðڽ��ϱ�?"), MB_YESNO))
					if (IDYES == MsgBox(_T("ī�޶�(��)�� �˻��ǳ� ������ ���� ���Ͻðڽ��ϱ�?"), 0, MB_YESNO))
					{
						m_bReAlign[1][3] = FALSE;
						m_bSkipAlign[1][3] = TRUE;
						//if(IDNO == DoMyMsgBox(_T("ī�޶�(��)�� �˻��ǳ��� �ҷ���ŷ �Ͻðڽ��ϱ�?"), MB_YESNO))
						if (IDNO == MsgBox(_T("ī�޶�(��)�� �˻��ǳ��� �ҷ���ŷ �Ͻðڽ��ϱ�?"), 0, MB_YESNO))
						{
							m_bDoMk[1] = FALSE;
							m_bDoneMk[1] = TRUE;
						}
						else
						{
							m_bDoMk[1] = TRUE;
							m_bDoneMk[1] = FALSE;
						}
					}
					else
					{
						m_bReAlign[1][3] = TRUE;
						m_bSkipAlign[1][3] = FALSE;
						//m_nMkStAuto = MK_ST + (Mk4PtIdx::Align1_3); // FourPointAlign1(3) ���� ����. - ī�޶� ������
						m_nMkStAuto = MK_ST + (Mk4PtIdx::Move3Cam1); // FourPointAlign1(3) ���� ����. - ī�޶� ������
						Stop();
						TowerLamp(RGB_YELLOW, TRUE);
					}
				}
				else
				{
					Buzzer(FALSE, 0);

					m_bReAlign[1][3] = TRUE;
					m_bSkipAlign[1][3] = FALSE;
					//m_nMkStAuto = MK_ST + (Mk4PtIdx::Align1_3); // FourPointAlign1(3) ���� ����. - ī�޶� ������
					m_nMkStAuto = MK_ST + (Mk4PtIdx::Move3Cam1); // FourPointAlign1(3) ���� ����. - ī�޶� ������
					Stop();
					TowerLamp(RGB_YELLOW, TRUE);
				}
			}

			if (m_bFailAlign[0][3] || m_bFailAlign[1][3])
			{
				if (!m_bReAlign[0][3] && !m_bReAlign[1][3])
				{
					if (m_bDoMk[0] || m_bDoMk[1])
						m_nMkStAuto++;//m_nMkStAuto = MK_ST + 29;  // MoveInitPos0()
					else
					{
						if (!IsInitPos0())
							MoveInitPos0();
						if (!IsInitPos1())
							MoveInitPos1();

						m_nMkStAuto = MK_ST + (Mk4PtIdx::DoneMk); // // �ҷ��̹��� Display, Align���� �ʱ�ȭ (Skip 65 : Mk())
					}
				}
				else
					m_nMkStAuto = MK_ST + (Mk4PtIdx::Move3Cam1); // FourPointAlign1(3) ���� ����. - ī�޶� ������
					//m_nMkStAuto = MK_ST + (Mk4PtIdx::Align1_3); // FourPointAlign1(3) ���� ����. - ī�޶� ������
			}
			else
				m_nMkStAuto++;

			break;
		}
	}
}

void CGvisR2R_PunchView::Mk4PtMoveInitPos()
{
	if (m_bMkSt)
	{
		switch (m_nMkStAuto)
		{
		case MK_ST + (Mk4PtIdx::MoveInitPt) :
			MoveInitPos0();
			m_nMkStAuto++;
			break;
		case MK_ST + (Mk4PtIdx::MoveInitPt) + 1:
			if (m_bDoMk[1])
				MoveInitPos1();
			else
				MoveMkEdPos1();
			m_nMkStAuto++;
			break;
		case MK_ST + (Mk4PtIdx::MoveInitPt) + 2:
			if (IsMoveDone())
				m_nMkStAuto++;
			break;
		}
	}
}

void CGvisR2R_PunchView::Mk4PtElecChk()
{
	CString sRst;

	if (m_bMkSt)
	{
		switch (m_nMkStAuto)
		{
		case MK_ST + (Mk4PtIdx::ChkElec) : // DoElecChk
			if (DoElecChk(sRst))
			{
				if (pDoc->WorkingInfo.Probing[0].bUse)
				{
					if (sRst == _T("Open"))
					{
						if (pDoc->WorkingInfo.Probing[0].bStopOnOpen)
							m_nMkStAuto = REJECT_ST;
						else
							m_nMkStAuto++;
					}
					else if (sRst == _T("Error"))
					{
						m_nMkStAuto = ERROR_ST;
					}
					else
					{
						m_nMkStAuto++;
					}
				}
				else
					m_nMkStAuto++;
			}
		   break;

		case MK_ST + (Mk4PtIdx::ChkElec) + 1:
			if (ChkLightErr())
			{
				m_bChkLightErr = FALSE;
				m_nMkStAuto++;
			}
			else
				m_nMkStAuto = MK_ST + (Mk4PtIdx::DoMk);	// Mk ��ŷ ����
			break;

		case MK_ST + (Mk4PtIdx::ChkElec) + 2:
			if (IsRun())
			{
				if (m_pMotion->IsEnable(MS_X0) && m_pMotion->IsEnable(MS_Y0) &&
					m_pMotion->IsEnable(MS_X1) && m_pMotion->IsEnable(MS_Y1))
				{
					if (MODE_INNER != pDoc->GetTestMode())
						m_nMkStAuto = MK_ST + (Mk4PtIdx::DoMk); 	// Mk ��ŷ ����
					else
						m_nMkStAuto = MK_ST + (Mk4PtIdx::Shift2Mk);
				}
				else
				{
					Stop();
					MsgBox(_T("��ŷ�� ����� ��Ȱ��ȭ �Ǿ����ϴ�."));
					TowerLamp(RGB_RED, TRUE);
				}
			}
			else
			{
				if (!m_bChkLightErr)
				{
					m_bChkLightErr = TRUE;
					MsgBox(_T("�뱤�ҷ� ���� - ������ Ȯ���ϼ���.\r\n��������Ϸ��� ��������ġ�� ��������."));
				}
			}
			break;
		}
	}
}

void CGvisR2R_PunchView::Mk4PtDoMarking()
{
	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;
	CString sRst, sMsg;
	int a, b, nSerial, nPrevSerial;

	if (m_bMkSt)
	{
		switch (m_nMkStAuto)
		{
		case MK_ST + (Mk4PtIdx::DoMk) :				// Mk ��ŷ ����
			if (pDoc->GetTestMode() == MODE_OUTER)
				SetMkIts(TRUE);						// ITS ��ŷ ����
			else
				SetMk(TRUE);						// Mk ��ŷ ����

			m_nMkStAuto++;
			break;

		case MK_ST + (Mk4PtIdx::DoMk) + 1:
			Sleep(100);
			m_nMkStAuto++;
			break;

		case MK_ST + (Mk4PtIdx::Verify) :
			if (IsMkDone() && IsMoveDone())
			{
				if (IsVerify() && !m_nPrevMkStAuto)
				{
					m_nPrevMkStAuto = MK_ST + (Mk4PtIdx::Verify);
					m_nMkStAuto = MK_ST + (Mk4PtIdx::DoMk);		// Mk ��ŷ ����
					m_bCam = TRUE;
				}
				else
				{
					if (IsReview())
					{
						if (!m_bCam)
						{
							m_nPrevStepAuto = MK_ST + (Mk4PtIdx::Verify);
							m_nMkStAuto = MK_ST + (Mk4PtIdx::DoMk);		// Mk ��ŷ ����
							m_bCam = TRUE;
							MsgBox(_T("�� Jog ��ư�� �̿��Ͽ� ��ŷ��ġ�� Ȯ���Ͽ� �ּ���."));
						}
						else
						{
							m_bCam = FALSE;
							m_nMkStAuto++;	// Mk ��ŷ �Ϸ�

											//sMsg = _T("");
											//DispStsBar(sMsg, 0);
						}
					}
					else
					{
						m_nMkStAuto++;	// Mk ��ŷ �Ϸ�

										//sMsg = _T("");
										//DispStsBar(sMsg, 0);
					}
				}
			}
			else if (IsReMk())
			{
				m_nPrevMkStAuto = MK_ST + (Mk4PtIdx::Verify);
				m_nMkStAuto = MK_ST + (Mk4PtIdx::DoMk);		// Mk �����
			}
			else
			{
				sMsg = _T("");
				sMsg += m_sDispSts[0];
				sMsg += _T(",");
				sMsg += m_sDispSts[1];
				//DispStsBar(sMsg, 0);
			}
										break;

		case MK_ST + (Mk4PtIdx::DoneMk) :	 // Align���� �ʱ�ȭ
			m_bReAlign[0][0] = FALSE; // [nCam][nPos] 
			m_bReAlign[0][1] = FALSE; // [nCam][nPos] 
			m_bReAlign[1][0] = FALSE; // [nCam][nPos] 
			m_bReAlign[1][1] = FALSE; // [nCam][nPos] 

			m_bSkipAlign[0][0] = FALSE; // [nCam][nPos] 
			m_bSkipAlign[0][1] = FALSE; // [nCam][nPos] 
			m_bSkipAlign[1][0] = FALSE; // [nCam][nPos] 
			m_bSkipAlign[1][1] = FALSE; // [nCam][nPos] 

			m_bFailAlign[0][0] = FALSE; // [nCam][nPos] 
			m_bFailAlign[0][1] = FALSE; // [nCam][nPos] 
			m_bFailAlign[1][0] = FALSE; // [nCam][nPos] 
			m_bFailAlign[1][1] = FALSE; // [nCam][nPos] 
			m_nMkStAuto++;
			break;
		case MK_ST + (Mk4PtIdx::DoneMk) + 1:  // Mk���� �ʱ�ȭ
			m_bDoMk[0] = TRUE;
			m_bDoMk[1] = TRUE;
			m_bDoneMk[0] = FALSE;
			m_bDoneMk[1] = FALSE;
			m_bReMark[0] = FALSE;
			m_bReMark[1] = FALSE;
			m_bCam = FALSE;
			m_nPrevMkStAuto = 0;

			for (a = 0; a < 2; a++)
			{
				for (b = 0; b < MAX_STRIP_NUM; b++)
				{
					m_nMkStrip[a][b] = 0;
					m_bRejectDone[a][b] = FALSE;
				}
			}

			m_nMkStAuto++;
			break;

		case MK_ST + (Mk4PtIdx::DoneMk) + 2:
			if (IsVs() && bDualTest)
			{
				if (m_nBufTot[1] < 4 && m_pBufSerial[1][m_nBufTot[1] - 1] < GetLotEndSerial())
				{
					Sleep(300);
					break;
				}
			}

			m_pMpe->Write(_T("MB440150"), 0);	// ��ŷ�� ��ŷ�� ON (PC�� ON, OFF)
			m_pMpe->Write(_T("MB440170"), 1);	// ��ŷ�Ϸ�(PLC�� Ȯ���ϰ� Reset��Ŵ.)-20141029
			pDoc->LogAuto(_T("PC: ��ŷ�Ϸ�(PLC�� Ȯ���ϰ� Reset��Ŵ.)"));
			if (IsNoMk() || IsShowLive())
				ShowLive(FALSE);			

			m_nMkStAuto++;
			break;

		case MK_ST + (Mk4PtIdx::DoneMk) + 3:
#ifdef USE_MPE
			if (pDoc->m_pMpeSignal[0] & (0x01 << 1))	// ��ŷ�� Feeding�Ϸ�(PLC�� On��Ű�� PC�� Ȯ���ϰ� Reset��Ŵ.)-20141030
			{
				pDoc->LogAuto(_T("PLC: ��ŷ�� Feeding�Ϸ�(PLC�� On��Ű�� PC�� Ȯ���ϰ� Reset��Ŵ.)"));
				m_pMpe->Write(_T("MB440101"), 0);	// ��ŷ�� Feeding�Ϸ�

				Shift2Mk();			// PCR �̵�(Buffer->Marked) // ���(WorkingInfo.LastJob.sSerial)
				UpdateRst();
				SetMkFdLen();

				SetCycTime();
				m_dwCycSt = GetTickCount();

				UpdateWorking();	// Update Working Info...
				ChkYield();
				m_nMkStAuto++;
			}
#endif
			break;
		case MK_ST + (Mk4PtIdx::DoneMk) + 4:
			ChkLotCutPos();
			m_nMkStAuto++;
			break;
		case MK_ST + (Mk4PtIdx::DoneMk) + 5:
			m_nMkStAuto++;
			break;
		case MK_ST + (Mk4PtIdx::DoneMk) + 6:
			m_nMkStAuto++;
			if (!IsBuffer(0))
			{
				m_bLotEnd = TRUE;
				m_nLotEndAuto = LOT_END;
			}
			else
			{
				// [0]: AOI-Up , [1]: AOI-Dn
				if (bDualTest)
				{
					nSerial = pDoc->m_ListBuf[0].Pop(); // Up (��)
					nSerial = pDoc->m_ListBuf[0].Pop(); // Up (��)

					SetListBuf();
				}

				if (m_nLotEndSerial > 0)
				{
					nSerial = GetBufferUp(&nPrevSerial);

					if (pView->m_bSerialDecrese)
					{
						if (nSerial < m_nLotEndSerial || nSerial <= 0) // �뱤�ҷ� 3��° Lot End ( -1, -1, -2)
						{
							if (IsDoneDispMkInfo())
							{
								if (IsBuffer() && nSerial > 0)
									SetSerial(nSerial);
								else
									SetSerial(nPrevSerial - 1, TRUE);

								if (IsBuffer() && nSerial > 0)
									SetSerial(nSerial + 1);
								else
									SetSerial(nPrevSerial - 2, TRUE);

								//m_nStepAuto = LOT_END;
								m_bLotEnd = TRUE;
								m_nLotEndAuto = LOT_END;
							}
						}
					}
					else
					{
						if (nSerial > m_nLotEndSerial || nSerial <= 0) // �뱤�ҷ� 3��° Lot End ( -1, -1, -2)
						{
							if (IsDoneDispMkInfo())
							{
								if (IsBuffer() && nSerial > 0)
									SetSerial(nSerial);
								else
									SetSerial(nPrevSerial + 1, TRUE);

								if (IsBuffer() && nSerial > 0)
									SetSerial(nSerial + 1);
								else
									SetSerial(nPrevSerial + 2, TRUE);

								//m_nStepAuto = LOT_END;
								m_bLotEnd = TRUE;
								m_nLotEndAuto = LOT_END;
							}
						}
					}

					if (bDualTest)
					{
						nSerial = GetBufferDn(&nPrevSerial);

						if (pView->m_bSerialDecrese)
						{
							if (nSerial < m_nLotEndSerial || nSerial <= 0) // �뱤�ҷ� 3��° Lot End ( -1, -1, -2)
							{
								if (IsDoneDispMkInfo())
								{
									if (IsBuffer() && nSerial > 0)
										SetSerial(nSerial);
									else
										SetSerial(nPrevSerial - 1, TRUE);

									if (IsBuffer() && nSerial > 0)
										SetSerial(nSerial + 1);
									else
										SetSerial(nPrevSerial - 2, TRUE);

									m_bLotEnd = TRUE;
									m_nLotEndAuto = LOT_END;
								}
							}
						}
						else
						{
							if (nSerial > m_nLotEndSerial || nSerial <= 0) // �뱤�ҷ� 3��° Lot End ( -1, -1, -2)
							{
								if (IsDoneDispMkInfo())
								{
									if (IsBuffer() && nSerial > 0)
										SetSerial(nSerial);
									else
										SetSerial(nPrevSerial + 1, TRUE);

									if (IsBuffer() && nSerial > 0)
										SetSerial(nSerial + 1);
									else
										SetSerial(nPrevSerial + 2, TRUE);

									m_bLotEnd = TRUE;
									m_nLotEndAuto = LOT_END;
								}
							}
						}
					}
				}
			}

			break;
		case MK_ST + (Mk4PtIdx::DoneMk) + 7:
			m_bMkSt = FALSE;
			::WritePrivateProfileString(_T("Last Job"), _T("MkSt"), _T("0"), PATH_WORKING_INFO);
			break;
		}
	}
}

void CGvisR2R_PunchView::Mk4PtLotDiff()
{
	if (m_bMkSt)
	{
		switch (m_nMkStAuto)
		{
		case MK_ST + (Mk4PtIdx::LotDiff) :
			Stop();
			TowerLamp(RGB_YELLOW, TRUE);
			m_nMkStAuto++;
			break;
		case MK_ST + (Mk4PtIdx::LotDiff) + 1:
			//if(IDYES == DoMyMsgBox(_T("���� �ϸ��� Lot�� �ٸ��ϴ�.\r\n��� �۾��� �����Ͻðڽ��ϱ�?"), MB_YESNO))
			if (IDYES == MsgBox(_T("���� �ϸ��� Lot�� �ٸ��ϴ�.\r\n��� �۾��� �����Ͻðڽ��ϱ�?"), 0, MB_YESNO))
			{
				m_bContDiffLot = TRUE;
			}
			else
			{
				m_bContDiffLot = FALSE;
				m_bLotEnd = TRUE;
				m_nLotEndAuto = LOT_END;
			}
			m_nMkStAuto++;
			break;
		case MK_ST + (Mk4PtIdx::LotDiff) + 2:
			if (IsRun())
			{
				if (m_bContDiffLot)
					m_nMkStAuto = MK_ST + (Mk4PtIdx::ChkSn);
				else
					m_nMkStAuto++;
			}
			break;
		case MK_ST + (Mk4PtIdx::LotDiff) + 3:
			m_bContDiffLot = FALSE;
			m_bLotEnd = TRUE;
			m_nLotEndAuto = LOT_END;
			break;
		}
	}
}

void CGvisR2R_PunchView::Mk4PtReject()
{
	int a, b;

	if (m_bMkSt)
	{
		switch (m_nMkStAuto)
		{
		case REJECT_ST:
			Buzzer(TRUE, 0);
			TowerLamp(RGB_RED, TRUE);
			Stop();

			//if(IDYES == DoMyMsgBox(_T("��Ʈ üũ �ҷ��Դϴ�.\r\n���� ó���� �����Ͻðڽ��ϱ�?"), MB_YESNO))
			if (IDYES == MsgBox(_T("��Ʈ üũ �ҷ��Դϴ�.\r\n���� ó���� �����Ͻðڽ��ϱ�?"), 0, MB_YESNO))
			{
				m_bAnswer[0] = TRUE;
				m_nMkStAuto++;
			}
			else
			{
				//if(IDYES == DoMyMsgBox(_T("���� ó���� ����Ͽ����ϴ�.\r\n�ҷ��� ��ŷ ó���� �����Ͻðڽ��ϱ�?"), MB_YESNO))
				if (IDYES == MsgBox(_T("���� ó���� ����Ͽ����ϴ�.\r\n�ҷ��� ��ŷ ó���� �����Ͻðڽ��ϱ�?"), 0, MB_YESNO))
				{
					m_bAnswer[1] = TRUE;
					m_nMkStAuto++;
				}
			}

			Buzzer(FALSE, 0);
			break;
		case REJECT_ST + 1:
			if (IsRun())
			{
				if (m_bAnswer[0])
				{
					m_bAnswer[0] = FALSE;
					m_nMkStAuto++;
				}
				else if (m_bAnswer[1])
				{
					m_bAnswer[1] = FALSE;
					m_nMkStAuto = MK_ST + (Mk4PtIdx::DoMk);	// Mk ��ŷ ����
				}
				else
					m_nMkStAuto = REJECT_ST;
			}
			break;
		case REJECT_ST + 2:
			SetReject();
			m_nMkStAuto++;
			break;
		case REJECT_ST + 3:
			m_nMkStAuto++;
			break;
		case REJECT_ST + 4:
			if (IsMkDone() && IsMoveDone())
			{
				if (IsVerify() && !m_nPrevMkStAuto)
				{
					m_nPrevMkStAuto = REJECT_ST + 4;
					m_nMkStAuto = REJECT_ST + 2;		// Mk ��ŷ ����
					m_bCam = TRUE;

					m_bDoneMk[0] = FALSE;
					m_bDoneMk[1] = FALSE;

					for (a = 0; a < 2; a++)
					{
						for (b = 0; b < MAX_STRIP_NUM; b++)
						{
							m_nMkStrip[a][b] = 0;
							m_bRejectDone[a][b] = FALSE;
						}
					}
				}
				else
				{
					if (IsReview())
					{
						if (!m_bCam)
						{
							m_nPrevStepAuto = REJECT_ST + 4;
							m_nMkStAuto = REJECT_ST + 2;		// Mk ��ŷ ����
							m_bCam = TRUE;
							MsgBox(_T("�� Jog ��ư�� �̿��Ͽ� ��ŷ��ġ�� Ȯ���Ͽ� �ּ���."));
						}
						else
						{
							m_bCam = FALSE;
							m_nMkStAuto++;	// Mk ��ŷ �Ϸ�

											//sMsg = _T("");
											//DispStsBar(sMsg, 0);
						}
					}
					else
					{
						m_nMkStAuto++;	// Mk ��ŷ �Ϸ�

										//sMsg = _T("");
										//DispStsBar(sMsg, 0);
					}
				}
			}
			break;
		case REJECT_ST + 5:
			m_nMkStAuto = MK_ST + (Mk4PtIdx::DoneMk);				// Align���� �ʱ�ȭ
			break;
		}
	}
}

void CGvisR2R_PunchView::Mk4PtErrStop()
{
	if (m_bMkSt)
	{
		switch (m_nMkStAuto)
		{
		case ERROR_ST:
			Stop();
			TowerLamp(RGB_RED, TRUE);

			//if(IDYES == DoMyMsgBox(_T("��Ʈ üũ Error�Դϴ�.\r\n�ٽ� ��Ʈ üũ�� �����Ͻðڽ��ϱ�?"), MB_YESNO))
			if (IDYES == MsgBox(_T("��Ʈ üũ Error�Դϴ�.\r\n�ٽ� ��Ʈ üũ�� �����Ͻðڽ��ϱ�?"), 0, MB_YESNO))
			{
				m_bAnswer[0] = TRUE;
				m_nMkStAuto++;
			}
			else
			{
				//if(IDYES == DoMyMsgBox(_T("��Ʈ üũ�� ����Ͽ����ϴ�.\r\n�ҷ��� ��ŷ ó���� �����Ͻðڽ��ϱ�?"), MB_YESNO))
				if (IDYES == MsgBox(_T("��Ʈ üũ�� ����Ͽ����ϴ�.\r\n�ҷ��� ��ŷ ó���� �����Ͻðڽ��ϱ�?"), 0, MB_YESNO))
				{
					m_bAnswer[1] = TRUE;
					m_nMkStAuto++;
				}
				else
					m_nMkStAuto++;
			}
			break;
		case ERROR_ST + 1:
			m_nMkStAuto++;
			break;
		case ERROR_ST + 2:
			if (IsRun())
			{
				if (m_bAnswer[0])
				{
					m_bAnswer[0] = FALSE;
					m_nMkStAuto = MK_ST + (Mk4PtIdx::ChkElec); // DoElecChk
				}
				else if (m_bAnswer[1])
				{
					m_bAnswer[1] = FALSE;
					m_nMkStAuto = MK_ST + (Mk4PtIdx::DoMk);	// Mk ��ŷ ����
				}
				else
					m_nMkStAuto++;
			}
			break;
		case ERROR_ST + 3:
			m_nMkStAuto = ERROR_ST;
			break;
		}
	}
}

void CGvisR2R_PunchView::MarkingWith4PointAlign()
{
	Mk4PtReady();
	Mk4PtChkSerial();
	Mk4PtInit();
	Mk4PtAlignPt0();
	Mk4PtAlignPt1();
	Mk4PtAlignPt2();
	Mk4PtAlignPt3();
	Mk4PtMoveInitPos();
	Mk4PtElecChk();
	Mk4PtDoMarking();
	Mk4PtLotDiff();
	Mk4PtReject();
	Mk4PtErrStop();
}

BOOL CGvisR2R_PunchView::IsSameUpDnLot()
{
	if (pDoc->Status.PcrShare[0].sLot == pDoc->Status.PcrShare[1].sLot)
		return TRUE;

	return FALSE;
}

int CGvisR2R_PunchView::GetAutoStep()
{
	return m_nStepAuto;
}

void CGvisR2R_PunchView::MoveMkInitPos()
{
	MoveMk0InitPos();
	MoveMk1InitPos();
}

void CGvisR2R_PunchView::MoveMk0InitPos()
{
	//BOOL bOn;
	// 	bOn = pDoc->m_pSliceIo[7] & (0x01<<10) ? TRUE : FALSE;	// ��ŷ�� ��ũ Ŭ���� ����ġ ���� -> ��ŷ�� ��ŷ �Ǹ��� SOL
	// 	if(bOn)
	// 	{
	// 		if(pView->m_pDlgMenu03)
	// 			pView->m_pDlgMenu03->SwMkDnSol(FALSE);
	// 		Sleep(300);
	// 	}

	double pTgtPos[2];
	pTgtPos[1] = _tstof(pDoc->WorkingInfo.Motion.sStPosY[0]);
	pTgtPos[0] = _tstof(pDoc->WorkingInfo.Motion.sStPosX[0]);
	double dCurrX = pView->m_dEnc[AXIS_X0];
	double dCurrY = pView->m_dEnc[AXIS_Y0];

	double fLen, fVel, fAcc, fJerk;
	fLen = sqrt(((pTgtPos[0] - dCurrX) * (pTgtPos[0] - dCurrX)) + ((pTgtPos[1] - dCurrY) * (pTgtPos[1] - dCurrY)));
	if (fLen > 0.001)
	{
		pView->m_pMotion->GetSpeedProfile0(TRAPEZOIDAL, AXIS_X0, fLen, fVel, fAcc, fJerk);
		if (!pView->m_pMotion->Move0(MS_X0Y0, pTgtPos, fVel / 2.0, fAcc / 2.0, fAcc / 2.0))
		{
			if (!pView->m_pMotion->Move0(MS_X0Y0, pTgtPos, fVel / 2.0, fAcc / 2.0, fAcc / 2.0))
			{
				pView->ClrDispMsg();
				AfxMessageBox(_T("Move X0Y0 Error..."));
			}
		}
	}
}

void CGvisR2R_PunchView::MoveMk1InitPos()
{
	//BOOL bOn;
	// 	bOn = pDoc->m_pSliceIo[7] & (0x01<<10) ? TRUE : FALSE;	// ��ŷ�� ��ũ Ŭ���� ����ġ ���� -> ��ŷ�� ��ŷ �Ǹ��� SOL
	// 	if(bOn)
	// 	{
	// 		if(pView->m_pDlgMenu03)
	// 			pView->m_pDlgMenu03->SwMkDnSol(FALSE);
	// 		Sleep(300);
	// 	}

	double pTgtPos[2];
	pTgtPos[1] = _tstof(pDoc->WorkingInfo.Motion.sStPosY[1]);
	pTgtPos[0] = _tstof(pDoc->WorkingInfo.Motion.sStPosX[1]);
	double dCurrX = pView->m_dEnc[AXIS_X1];
	double dCurrY = pView->m_dEnc[AXIS_Y1];

	double fLen, fVel, fAcc, fJerk;
	fLen = sqrt(((pTgtPos[0] - dCurrX) * (pTgtPos[0] - dCurrX)) + ((pTgtPos[1] - dCurrY) * (pTgtPos[1] - dCurrY)));
	if (fLen > 0.001)
	{
		pView->m_pMotion->GetSpeedProfile1(TRAPEZOIDAL, AXIS_X1, fLen, fVel, fAcc, fJerk);
		if (!pView->m_pMotion->Move1(MS_X1Y1, pTgtPos, fVel / 2.0, fAcc / 2.0, fAcc / 2.0))
		{
			if (!pView->m_pMotion->Move1(MS_X1Y1, pTgtPos, fVel / 2.0, fAcc / 2.0, fAcc / 2.0))
			{
				pView->ClrDispMsg();
				AfxMessageBox(_T("Move X1Y1 Error..."));
			}
		}
	}
}


BOOL CGvisR2R_PunchView::PreTranslateMessage(MSG* pMsg)
{
	// TODO: Add your specialized code here and/or call the base class
	if (pMsg->message != WM_KEYDOWN)
		return CFormView::PreTranslateMessage(pMsg);

	if ((pMsg->lParam & 0x40000000) == 0)
	{
		switch (pMsg->wParam)
		{
		case VK_RETURN:
			return TRUE;
		case VK_ESCAPE:
			m_bEscape = TRUE;
			return TRUE;
		case 'S':
		case 's':
			if (GetKeyState(VK_CONTROL) < 0) // Ctrl Ű�� ������ ����
			{
				WINDOWPLACEMENT wndPlace;
				AfxGetMainWnd()->GetWindowPlacement(&wndPlace);
				wndPlace.showCmd |= SW_MAXIMIZE;
				AfxGetMainWnd()->SetWindowPlacement(&wndPlace);
			}
			break;
		}
	}

	return CFormView::PreTranslateMessage(pMsg);
}


int CGvisR2R_PunchView::MyPassword(CString strMsg, int nCtrlId)
{
	// 	if(m_pDlgMyMsg != NULL) 
	// 	{
	// 		m_pDlgMyMsg->DestroyWindow();
	// 		delete m_pDlgMyMsg;
	// 		m_pDlgMyMsg = NULL;
	// 	}

	// 	CString msg = Filtering(strMsg);

	CDlgMyPassword dlg1(this);
	dlg1.SetMsg(strMsg, nCtrlId);
	dlg1.DoModal();
	return (dlg1.m_nRtnVal);

}

BOOL CGvisR2R_PunchView::ReloadReelmapInner()
{
	double dRatio = 0.0;
	CString sVal = _T("");

	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTestInner;

	BOOL bRtn[7] = { 1 };
	//if (pDoc->m_pReelMapInner)
	//	bRtn[0] = pDoc->m_pReelMapInner->ReloadReelmap();

	if (pDoc->m_pReelMapInnerUp)
		bRtn[1] = pDoc->m_pReelMapInnerUp->ReloadReelmap();

	if (pDoc->m_pReelMapIts)
		bRtn[2] = pDoc->m_pReelMapIts->ReloadReelmap();


	if (bDualTest)
	{
		if (pDoc->m_pReelMapInnerDn)
			bRtn[4] = pDoc->m_pReelMapInnerDn->ReloadReelmap();
		if (pDoc->m_pReelMapInnerAllUp)
			bRtn[5] = pDoc->m_pReelMapInnerAllUp->ReloadReelmap();
		if (pDoc->m_pReelMapInnerAllDn)
			bRtn[6] = pDoc->m_pReelMapInnerAllDn->ReloadReelmap();
	}

	for (int i = 1; i < 7; i++)
	{
		if (!bRtn[i])
			return FALSE;
	}

	return TRUE;
}


BOOL CGvisR2R_PunchView::IsDoneReloadReelmapInner(int& nProc)
{
	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTestInner;

	int nRatio[7] = { 0 };
	BOOL bDone[7] = { 0 };

	bDone[0] = TRUE;

	if (pDoc->m_pReelMapInnerUp)
	{
		nRatio[1] = pDoc->m_pReelMapInnerUp->GetProgressReloadReelmap();
		bDone[1] = pDoc->m_pReelMapInnerUp->IsDoneReloadReelmap();
	}
	else
		bDone[1] = TRUE;

	if (pDoc->m_pReelMapIts)
	{
		nRatio[2] = pDoc->m_pReelMapIts->GetProgressReloadReelmap();
		bDone[2] = pDoc->m_pReelMapIts->IsDoneReloadReelmap();
	}
	else
		bDone[2] = TRUE;

	bDone[3] = TRUE;

	if (bDualTest)
	{
		if (pDoc->m_pReelMapInnerDn)
		{
			nRatio[4] = pDoc->m_pReelMapInnerDn->GetProgressReloadReelmap();
			bDone[4] = pDoc->m_pReelMapInnerDn->IsDoneReloadReelmap();
		}
		else
			bDone[4] = TRUE;

		if (pDoc->m_pReelMapInnerAllUp)
		{
			nRatio[5] = pDoc->m_pReelMapInnerAllUp->GetProgressReloadReelmap();
			bDone[5] = pDoc->m_pReelMapInnerAllUp->IsDoneReloadReelmap();
		}
		else
			bDone[5] = TRUE;

		if (pDoc->m_pReelMapAllDn)
		{
			nRatio[6] = pDoc->m_pReelMapInnerAllDn->GetProgressReloadReelmap();
			bDone[6] = pDoc->m_pReelMapInnerAllDn->IsDoneReloadReelmap();
		}
		else
			bDone[6] = TRUE;
	}
	else
	{
		bDone[4] = TRUE;
		bDone[5] = TRUE;
		bDone[6] = TRUE;
	}

	nProc = nRatio[0] + nRatio[1] + nRatio[2] + nRatio[3] + nRatio[4] + nRatio[5] + nRatio[6];

	if (bDone[0] && bDone[1] && bDone[2] && bDone[3] && bDone[4] && bDone[5] && bDone[6])
		return TRUE;

	return FALSE;
}

BOOL CGvisR2R_PunchView::ReloadReelmap()
{
	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;
	BOOL bDualTestInner = pDoc->WorkingInfo.LastJob.bDualTestInner;


	BOOL bRtn[5] = { 1 };
	int nSt = 0;

	if (pDoc->m_pReelMapUp)
		bRtn[1] = pDoc->m_pReelMapUp->ReloadReelmap();

	if (bDualTest)
	{
		if (pDoc->m_pReelMapDn)
			bRtn[2] = pDoc->m_pReelMapDn->ReloadReelmap();

		if (pDoc->m_pReelMapAllUp)
			bRtn[3] = pDoc->m_pReelMapAllUp->ReloadReelmap();

		if (pDoc->m_pReelMapAllDn)
			bRtn[4] = pDoc->m_pReelMapAllDn->ReloadReelmap();
	}

	if (pDoc->GetTestMode() == MODE_OUTER)
	{
		bRtn[0] = ReloadReelmapInner();
	}
	else
		nSt = 1;

	for (int i = nSt; i < 5; i++)
	{
		if (!bRtn[i])
		{
			AfxMessageBox(_T("ReloadReelmap() is Failed."));
			return FALSE;
		}
	}

	double dRatio = 0.0;
	CString sVal = _T("");
	CDlgProgress dlg;
	sVal.Format(_T("On Reloading Reelmap."));
	dlg.Create(sVal);

	int nRatio[5] = { 0 };
	BOOL bDone[5] = { 0 };
	int nTo = 0;
	if (bDualTest)
	{
		if (pDoc->GetTestMode() == MODE_OUTER)
		{
			if (bDualTestInner)
				nTo = 400 + 500; //[%]
			else
				nTo = 400 + 200; //[%]
		}
		else
		{
			nTo = 400; //[%]
		}
	}
	else
	{
		if (pDoc->GetTestMode() == MODE_OUTER)
		{
			if (bDualTestInner)
				nTo = 100 + 500; //[%]
			else
				nTo = 100 + 200; //[%]
		}
		else
		{
			nTo = 100; //[%]
		}
	}


	dlg.SetRange(0, nTo);
	dlg.SetPos(1);

	int nProc=0, nProcOutter=0, nProcInner=0;
	for (nProc = 0; nProc < nTo;)
	{
		if (pDoc->GetTestMode() == MODE_OUTER)
		{
			if ( (bDone[0] = IsDoneReloadReelmap(nProcOutter)) && (bDone[1] = IsDoneReloadReelmapInner(nProcInner)) )
				break;
			nProc = nProcOutter + nProcInner;
		}
		else
		{
			if (bDone[0] = IsDoneReloadReelmap(nProc))
				break;
		}
		dlg.SetPos(nProc);
		Sleep(100);
	}

	dlg.DestroyWindow();	


	if (pDoc->GetTestMode() == MODE_OUTER)
	{
		if (!bDone[0] || !bDone[1])
		{
			AfxMessageBox(_T("IsDoneReloadReelmap or IsDoneReloadReelmapInner is FALSE."));
			return FALSE;
		}
	}
	else
	{
		if (!bDone[0])
		{
			AfxMessageBox(_T("IsDoneReloadReelmap is FALSE."));
			return FALSE;
		}
	}

	return TRUE;
}

BOOL CGvisR2R_PunchView::IsDoneReloadReelmap(int& nProc)
{
	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;

	int nRatio[5] = { 0 };
	BOOL bDone[5] = { 0 };

	if (pDoc->m_pReelMapUp)
	{
		nRatio[1] = pDoc->m_pReelMapUp->GetProgressReloadReelmap();
		bDone[1] = pDoc->m_pReelMapUp->IsDoneReloadReelmap();
	}
	else
		bDone[1] = TRUE;

	if (bDualTest)
	{
		if (pDoc->m_pReelMapDn)
		{
			nRatio[2] = pDoc->m_pReelMapDn->GetProgressReloadReelmap();
			bDone[2] = pDoc->m_pReelMapDn->IsDoneReloadReelmap();
		}
		else
			bDone[2] = TRUE;
		if (pDoc->m_pReelMapAllUp)
		{
			nRatio[3] = pDoc->m_pReelMapAllUp->GetProgressReloadReelmap();
			bDone[3] = pDoc->m_pReelMapAllUp->IsDoneReloadReelmap();
		}
		else
			bDone[3] = TRUE;

		if (pDoc->m_pReelMapAllDn)
		{
			nRatio[4] = pDoc->m_pReelMapAllDn->GetProgressReloadReelmap();
			bDone[4] = pDoc->m_pReelMapAllDn->IsDoneReloadReelmap();
		}
		else
			bDone[4] = TRUE;
	}

	nProc = nRatio[1] + nRatio[2] + nRatio[3] + nRatio[4];

	if (bDone[1] && bDone[2] && bDone[3] && bDone[4])
		return TRUE;

	return FALSE;
}

void CGvisR2R_PunchView::ReloadReelmapUp()
{
	if (pDoc->m_pReelMapUp)
		pDoc->m_pReelMapUp->ReloadReelmap();
}

void CGvisR2R_PunchView::ReloadReelmapAllUp()
{
	if (pDoc->m_pReelMapAllUp)
		pDoc->m_pReelMapAllUp->ReloadReelmap();
}

void CGvisR2R_PunchView::ReloadReelmapDn()
{
	if (pDoc->m_pReelMapDn)
		pDoc->m_pReelMapDn->ReloadReelmap();
}

void CGvisR2R_PunchView::ReloadReelmapAllDn()
{
	if (pDoc->m_pReelMapAllDn)
		pDoc->m_pReelMapAllDn->ReloadReelmap();
}

BOOL CGvisR2R_PunchView::ReloadReelmap(int nSerial)
{
	m_nReloadReelmapSerial = nSerial;

	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;

	m_bTHREAD_RELOAD_RST_UP = TRUE;
	if (bDualTest)
	{
		m_bTHREAD_RELOAD_RST_DN = TRUE;
		m_bTHREAD_RELOAD_RST_ALLUP = TRUE;
		m_bTHREAD_RELOAD_RST_ALLDN = TRUE;
	}

	if (pDoc->GetTestMode() == MODE_OUTER)
	{
		m_bTHREAD_RELOAD_RST_UP_INNER = TRUE;
		m_bTHREAD_RELOAD_RST_ITS = TRUE;
		if (pDoc->WorkingInfo.LastJob.bDualTestInner)
		{
			m_bTHREAD_RELOAD_RST_DN_INNER = TRUE;
			m_bTHREAD_RELOAD_RST_ALLUP_INNER = TRUE;
			m_bTHREAD_RELOAD_RST_ALLDN_INNER = TRUE;
		}
	}

	Sleep(100);

	return TRUE;
}


void CGvisR2R_PunchView::ReloadReelmapUpInner()
{
	if (pDoc->m_pReelMapInnerUp)
		pDoc->m_pReelMapInnerUp->ReloadReelmap();
}

void CGvisR2R_PunchView::ReloadReelmapAllUpInner()
{
	if (pDoc->m_pReelMapInnerAllUp)
		pDoc->m_pReelMapInnerAllUp->ReloadReelmap();
}

void CGvisR2R_PunchView::ReloadReelmapDnInner()
{
	if (pDoc->m_pReelMapInnerDn)
		pDoc->m_pReelMapInnerDn->ReloadReelmap();
}

void CGvisR2R_PunchView::ReloadReelmapAllDnInner()
{
	if (pDoc->m_pReelMapInnerAllDn)
		pDoc->m_pReelMapInnerAllDn->ReloadReelmap();
}

void CGvisR2R_PunchView::ReloadReelmapIts()
{
	if (pDoc->m_pReelMapIts)
		pDoc->m_pReelMapIts->ReloadReelmap();
}


BOOL CGvisR2R_PunchView::OpenReelmapFromBuf(int nSerial)
{
	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;

	stModelInfo stInfoUp, stInfoDn;
	CString sSrc;
	sSrc.Format(_T("%s%04d.pcr"), pDoc->WorkingInfo.System.sPathVrsBufUp, nSerial);
	if (!pDoc->GetPcrInfo(sSrc, stInfoUp))
	{
		pView->DispStsBar(_T("E(6)"), 5);
		pView->ClrDispMsg();
		AfxMessageBox(_T("Error-GetPcrInfo(6)"));
		return FALSE;
	}
	if (bDualTest) 
	{
		sSrc.Format(_T("%s%04d.pcr"), pDoc->WorkingInfo.System.sPathVrsBufDn, nSerial);
		if (!pDoc->GetPcrInfo(sSrc, stInfoDn))
		{
			pView->DispStsBar(_T("E(7)"), 5);
			pView->ClrDispMsg();
			AfxMessageBox(_T("Error-GetPcrInfo(7)"));
			return FALSE;
		}
	}

	if (pDoc->m_pReelMapUp)
		pDoc->m_pReelMapUp->Open();

	if (bDualTest)
	{
		if (pDoc->m_pReelMapDn)
			pDoc->m_pReelMapDn->Open();
		if (pDoc->m_pReelMapAllUp)
			pDoc->m_pReelMapAllUp->Open();
		if (pDoc->m_pReelMapAllDn)
			pDoc->m_pReelMapAllDn->Open();
	}

	return TRUE;
}

void CGvisR2R_PunchView::OpenReelmap()
{
	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;

	if (pDoc->m_pReelMapUp)
		pDoc->m_pReelMapUp->Open();

	if (bDualTest)
	{
		if (pDoc->m_pReelMapDn)
			pDoc->m_pReelMapDn->Open();
		if (pDoc->m_pReelMapAllUp)
			pDoc->m_pReelMapAllUp->Open();
		if (pDoc->m_pReelMapAllDn)
			pDoc->m_pReelMapAllDn->Open();
	}
}

void CGvisR2R_PunchView::OpenReelmapUp()
{
	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;

	if (pDoc->m_pReelMapUp)
		pDoc->m_pReelMapUp->Open();

	if (bDualTest)
	{
		if (pDoc->m_pReelMapAllUp)
			pDoc->m_pReelMapAllUp->Open();
	}

	//if (pDoc->m_pReelMap)
	//{
	//	if (pDoc->m_pReelMap->m_nLayer < 0)
	//		pDoc->m_pReelMap->m_nLayer = pView->m_nSelRmap;

	//	if (bDualTest)
	//	{
	//		if (pDoc->m_pReelMap->m_nLayer == RMAP_UP || pDoc->m_pReelMap->m_nLayer == RMAP_ALLUP)
	//			pDoc->m_pReelMap->Open();
	//	}
	//	else
	//	{
	//		if (pDoc->m_pReelMap->m_nLayer == RMAP_UP)
	//			pDoc->m_pReelMap->Open();
	//	}
	//}
}

void CGvisR2R_PunchView::OpenReelmapDn()
{
	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;
	if (!bDualTest)
		return;

	if (pDoc->m_pReelMapDn)
		pDoc->m_pReelMapDn->Open();
	if (pDoc->m_pReelMapAllDn)
		pDoc->m_pReelMapAllDn->Open();

	//if (pDoc->m_pReelMap)
	//{
	//	if (pDoc->m_pReelMap->m_nLayer < 0)
	//		pDoc->m_pReelMap->m_nLayer = pView->m_nSelRmap;

	//	if (pDoc->m_pReelMap->m_nLayer == RMAP_DN || pDoc->m_pReelMap->m_nLayer == RMAP_ALLDN)
	//		pDoc->m_pReelMap->Open();
	//}
}

void CGvisR2R_PunchView::EStop()
{
	if (m_pMotion)
	{
		m_pMotion->EStop(MS_X0Y0);
		m_pMotion->EStop(MS_X1Y1);
		Sleep(30);
		ResetMotion(MS_X0Y0);
		Sleep(30);
		ResetMotion(MS_X1Y1);
		Sleep(30);
		// 		DispMsg(_T("X�� �浹 ������ ���� �����Դϴ�."), _T("�˸�"), RGB_GREEN, DELAY_TIME_MSG);
		pView->ClrDispMsg();
		AfxMessageBox(_T("X�� �浹 ������ ���� �����Դϴ�."));

		double dCurrX = pView->m_dEnc[AXIS_X1];
		double dCurrY = pView->m_dEnc[AXIS_Y1];
		double pPos[2], fVel, fAcc, fJerk;
		double fLen = 2.0;
		pPos[0] = dCurrX + fLen;
		pPos[1] = dCurrY;
		m_pMotion->GetSpeedProfile1(TRAPEZOIDAL, AXIS_X1, fLen, fVel, fAcc, fJerk);
		m_pMotion->Move1(MS_X1Y1, pPos, fVel, fAcc, fAcc, ABS, NO_WAIT);
	}
}

void CGvisR2R_PunchView::ResetPriority()
{
	m_bPriority[0] = FALSE;
	m_bPriority[1] = FALSE;
	m_bPriority[2] = FALSE;
	m_bPriority[3] = FALSE;
}

void CGvisR2R_PunchView::SetPriority()
{
	int nDir[2];
	nDir[0] = (m_dTarget[AXIS_X0] - m_dEnc[AXIS_X0]) >= 0.0 ? 1 : -1;
	nDir[1] = (m_dTarget[AXIS_X1] - m_dEnc[AXIS_X1]) >= 0.0 ? 1 : -1;

	// Cam0 : m_bPriority[0], m_bPriority[3]
	// Cam1 : m_bPriority[1], m_bPriority[2]
	if (nDir[1] > 0) // Cam1 ->
	{
		m_bPriority[0] = FALSE;
		m_bPriority[1] = TRUE;
		m_bPriority[2] = FALSE;
		m_bPriority[3] = FALSE;
	}
	else if (nDir[0] < 0) // Cam0 <-
	{
		m_bPriority[0] = TRUE;
		m_bPriority[1] = FALSE;
		m_bPriority[2] = FALSE;
		m_bPriority[3] = FALSE;
	}
	else if (nDir[1] < 0) // Cam1 <-
	{
		m_bPriority[0] = FALSE;
		m_bPriority[1] = FALSE;
		m_bPriority[2] = TRUE;
		m_bPriority[3] = FALSE;
	}
	else // Cam0 ->
	{
		m_bPriority[0] = FALSE;
		m_bPriority[1] = FALSE;
		m_bPriority[2] = FALSE;
		m_bPriority[3] = TRUE;
	}
}

BOOL CGvisR2R_PunchView::IsRunAxisX()
{
	if (m_pMotion->IsMotionDone(MS_X0) && m_pMotion->IsMotionDone(MS_X1))
		return FALSE;
	return TRUE;
}

BOOL CGvisR2R_PunchView::ChkCollision()
{
	double dMg = _tstof(pDoc->WorkingInfo.Motion.sCollisionLength) - _tstof(pDoc->WorkingInfo.Motion.sCollisionMargin);

	if (m_bTHREAD_MK[0] || m_bTHREAD_MK[1] || m_bTHREAD_MK[2] || m_bTHREAD_MK[3])
	{
		if (pDoc->WorkingInfo.System.bNoMk || m_bCam)
			dMg += (_tstof(pDoc->WorkingInfo.Vision[0].sMkOffsetX) - _tstof(pDoc->WorkingInfo.Vision[1].sMkOffsetX));
	}

	if ((m_dEnc[AXIS_X0] - dMg) > m_dEnc[AXIS_X1])
		return TRUE;

	if (m_dEnc[AXIS_X0] < -1.0 || m_dEnc[AXIS_X1] < -1.0)
		return TRUE;

	return FALSE;
}

BOOL CGvisR2R_PunchView::ChkCollision(int nAxisId, double dTgtPosX)
{
	double dMg = _tstof(pDoc->WorkingInfo.Motion.sCollisionLength) - _tstof(pDoc->WorkingInfo.Motion.sCollisionMargin);


	if (m_bTHREAD_MK[0] || m_bTHREAD_MK[1] || m_bTHREAD_MK[2] || m_bTHREAD_MK[3])
	{
		if (pDoc->WorkingInfo.System.bNoMk || m_bCam)
			dMg += (_tstof(pDoc->WorkingInfo.Vision[0].sMkOffsetX) - _tstof(pDoc->WorkingInfo.Vision[1].sMkOffsetX));
	}

	if (nAxisId == AXIS_X0)
	{
		if (dTgtPosX > m_dEnc[AXIS_X1] + dMg)
			return TRUE;
		if (m_bDoMk[1] && !m_bDoneMk[1])
		{
			if (!m_bDoneMk[0] && m_bTHREAD_MK[0])
			{
				if (m_dTarget[AXIS_X1] > 0.0)
				{
					if (dTgtPosX > m_dTarget[AXIS_X1] + dMg && m_dTarget[AXIS_X1] > 0.0)
						return TRUE;
					else if (m_dNextTarget[AXIS_X1] > 0.0)
					{
						if (dTgtPosX > m_dNextTarget[AXIS_X1] + dMg)
							return TRUE;
					}
				}
			}
		}
	}
	else if (nAxisId == AXIS_X1)
	{
		if (dTgtPosX < m_dEnc[AXIS_X0] - dMg)
			return TRUE;
	}

	return FALSE;
}

BOOL CGvisR2R_PunchView::ChkCollision(int nAxisId, double dTgtPosX, double dTgtNextPosX)
{
	double dMg = _tstof(pDoc->WorkingInfo.Motion.sCollisionLength) - _tstof(pDoc->WorkingInfo.Motion.sCollisionMargin);

	if (m_bTHREAD_MK[0] || m_bTHREAD_MK[1] || m_bTHREAD_MK[2] || m_bTHREAD_MK[3])
	{
		if (pDoc->WorkingInfo.System.bNoMk || m_bCam)
			dMg += (_tstof(pDoc->WorkingInfo.Vision[0].sMkOffsetX) - _tstof(pDoc->WorkingInfo.Vision[1].sMkOffsetX));
	}

	if (nAxisId == AXIS_X0)
	{
		if (dTgtPosX > m_dEnc[AXIS_X1] + dMg)
			return TRUE;
		if (m_bDoMk[1] && !m_bDoneMk[1])
		{
			if (!m_bDoneMk[0] && m_bTHREAD_MK[0])
			{
				if (dTgtPosX > m_dTarget[AXIS_X1] + dMg && m_dTarget[AXIS_X1] > 0.0)
					return TRUE;
				else if (dTgtPosX > m_dNextTarget[AXIS_X1] + dMg && m_dNextTarget[AXIS_X1] > 0.0)
					return TRUE;
			}
		}
	}
	else if (nAxisId == AXIS_X1)
	{
		if (dTgtPosX < m_dEnc[AXIS_X0] - dMg)
			return TRUE;
		if (m_bDoMk[0])
		{
			if (!m_bDoneMk[1] && m_bTHREAD_MK[1])
			{
				if (!m_pMotion->IsMotionDone(MS_X0))
				{
					if (dTgtPosX < m_dTarget[AXIS_X0] - dMg && m_dTarget[AXIS_X0] > 0.0)
						return TRUE;
					else if (dTgtPosX < m_dNextTarget[AXIS_X0] - dMg && m_dNextTarget[AXIS_X0] > 0.0)
						return TRUE;
				}
			}
		}
	}

	return FALSE;
}

void CGvisR2R_PunchView::StopAllMk()
{
	// Mk0
	m_nStepMk[2] = MK_END;

	// Mk1
	m_nStepMk[3] = MK_END;
}

void CGvisR2R_PunchView::DoAllMk(int nCam)
{
	if (nCam == CAM_LF)
	{
		double dCurrX = pView->m_dEnc[AXIS_X1];
		double dCurrY = pView->m_dEnc[AXIS_Y1];

		double pPos[2];
		pPos[0] = _tstof(pDoc->WorkingInfo.Motion.sSafeZone);
		pPos[1] = 0.0;

		double fLen, fVel, fAcc, fJerk;
		fLen = sqrt(((pPos[0] - dCurrX) * (pPos[0] - dCurrX)) + ((pPos[1] - dCurrY) * (pPos[1] - dCurrY)));
		if (fLen > 0.001)
		{
			pView->m_pMotion->GetSpeedProfile(TRAPEZOIDAL, AXIS_X1, fLen, fVel, fAcc, fJerk);
			if (!pView->m_pMotion->Move(MS_X1Y1, pPos, fVel, fAcc, fAcc))
			{
				if (!pView->m_pMotion->Move(MS_X1Y1, pPos, fVel, fAcc, fAcc))
				{
					pView->ClrDispMsg();
					AfxMessageBox(_T("Move X1Y1 Error..."));
				}
			}
		}
		if (!m_bTHREAD_MK[2])
		{
			m_nStepMk[2] = 0;
			m_nMkPcs[2] = 0;
			m_bTHREAD_MK[2] = TRUE;
		}
		else
		{
			m_bTHREAD_MK[2] = FALSE;
		}
	}
	else if (nCam == CAM_RT)
	{
		double dCurrX = pView->m_dEnc[AXIS_X0];
		double dCurrY = pView->m_dEnc[AXIS_Y0];

		double pPos[2];
		pPos[0] = 0.0;
		pPos[1] = 0.0;

		double fLen, fVel, fAcc, fJerk;
		fLen = sqrt(((pPos[0] - dCurrX) * (pPos[0] - dCurrX)) + ((pPos[1] - dCurrY) * (pPos[1] - dCurrY)));
		if (fLen > 0.001)
		{
			pView->m_pMotion->GetSpeedProfile(TRAPEZOIDAL, AXIS_X0, fLen, fVel, fAcc, fJerk);
			if (!pView->m_pMotion->Move(MS_X0Y0, pPos, fVel, fAcc, fAcc))
			{
				if (!pView->m_pMotion->Move(MS_X0Y0, pPos, fVel, fAcc, fAcc))
				{
					pView->ClrDispMsg();
					AfxMessageBox(_T("Move X0Y0 Error..."));
			}
		}
		}
		if (!m_bTHREAD_MK[3])
		{
			m_nStepMk[3] = 0;
			m_nMkPcs[3] = 0;
			m_bTHREAD_MK[3] = TRUE;
		}
		else
		{
			m_bTHREAD_MK[3] = FALSE;
		}
	}
	else if (nCam == CAM_BOTH)
	{
		if (!m_bTHREAD_MK[2])
		{
			m_nStepMk[2] = 0;
			m_nMkPcs[2] = 0;
			m_bTHREAD_MK[2] = TRUE;
		}
		else
		{
			m_bTHREAD_MK[2] = FALSE;
		}

		if (!m_bTHREAD_MK[3])
		{
			m_nStepMk[3] = 0;
			m_nMkPcs[3] = 0;
			m_bTHREAD_MK[3] = TRUE;
		}
		else
		{
			m_bTHREAD_MK[3] = FALSE;
		}
	}
}


void CGvisR2R_PunchView::InitPLC()
{
#ifdef USE_MPE
	long lData;
	lData = (long)(_tstof(pDoc->WorkingInfo.Lot.sTotalReelDist) * 1000.0);
	m_pMpe->Write(_T("ML45000"), lData);	// ��ü Reel ���� (���� M * 1000)
	lData = (long)(_tstof(pDoc->WorkingInfo.Lot.sSeparateDist) * 1000.0);
	m_pMpe->Write(_T("ML45002"), lData);	// Lot �и� ���� (���� M * 1000)
	lData = (long)(_tstof(pDoc->WorkingInfo.Lot.sCuttingDist) * 1000.0);
	m_pMpe->Write(_T("ML45004"), lData);	// Lot �и� �� ������ġ (���� M * 1000)
	lData = (long)(_tstof(pDoc->WorkingInfo.Lot.sStopDist) * 1000.0);
	m_pMpe->Write(_T("ML45006"), lData);	// �Ͻ����� ���� (���� M * 1000)
	lData = (long)(_tstof(pDoc->WorkingInfo.Motion.sAoiTq) * 1000.0);
	m_pMpe->Write(_T("ML45042"), lData);	// �˻�� Tension ���� ��ũ�� (���� Kgf * 1000)
	lData = (long)(_tstof(pDoc->WorkingInfo.Motion.sMkTq) * 1000.0);
	m_pMpe->Write(_T("ML45044"), lData);	// ��ŷ�� Tension ���� ��ũ�� (���� Kgf * 1000)
	lData = (long)(_tstof(pDoc->WorkingInfo.Motion.sEngraveTq) * 1000.0);
	m_pMpe->Write(_T("ML45050"), lData);	// ���κ� Tension ���� ��ũ�� (���� Kgf * 1000)
	lData = (long)(_tstof(pDoc->WorkingInfo.LastJob.sOnePnlLen) * 1000.0);
	m_pMpe->Write(_T("ML45032"), lData);	// �� �ǳ� ���� (���� mm * 1000)
	lData = (long)(_tstof(pDoc->WorkingInfo.Motion.sMkFdVel) * 1000.0);
	m_pMpe->Write(_T("ML45034"), lData);	// �� �ǳ� Feeding �ӵ� (���� mm/sec * 1000)
	lData = (long)(_tstof(pDoc->WorkingInfo.Motion.sMkJogVel) * 1000.0);
	m_pMpe->Write(_T("ML45038"), lData);	// ���Ӱ��� �ӵ� (���� mm/sec * 1000)
	lData = (long)(_tstof(pDoc->WorkingInfo.Motion.sAoiJogAcc) * 1000.0);
	m_pMpe->Write(_T("ML45040"), lData);	// ���Ӱ��� ���ӵ� (���� mm/s^2 * 1000)
	lData = (long)(_tstof(pDoc->WorkingInfo.Motion.sMkFdAcc) * 1000.0);
	m_pMpe->Write(_T("ML45036"), lData);	// �� �ǳ� Feeding ���ӵ� (���� mm/s^2 * 1000)
	lData = (long)(_tstof(pDoc->WorkingInfo.Motion.sAoiFdLead) * 1000.0);
	m_pMpe->Write(_T("ML45012"), lData);	// �˻�� Feeding �ѷ� Lead Pitch (���� mm * 1000)
	m_pMpe->Write(_T("ML45020"), lData);	// ���κ� Feeding �ѷ� Lead Pitch (���� mm * 1000)
	lData = (long)(_tstof(pDoc->WorkingInfo.Motion.sMkFdLead) * 1000.0);
	m_pMpe->Write(_T("ML45014"), lData);	// ��ŷ�� Feeding �ѷ� Lead Pitch (���� mm * 1000)
	lData = (long)(_tstof(pDoc->WorkingInfo.Motion.sStBufPos) * 1000.0);
	pView->m_pMpe->Write(_T("ML45016"), lData);	// ���� ���� ���� �ѷ� �ʱ���ġ(���� mm * 1000)
	lData = (long)(_tstof(pDoc->WorkingInfo.Motion.sFdMkAoiInitDist) * 1000.0);
	pView->m_pMpe->Write(_T("ML45008"), lData);	// AOI(��)���� ��ŷ���� �Ÿ� (���� mm * 1000)
	lData = (long)(_tstoi(pDoc->WorkingInfo.Motion.sFdAoiAoiDistShot) * 1000);
	pView->m_pMpe->Write(_T("ML45010"), lData);	// AOI(��)���� AOI(��) Shot�� (���� Shot�� * 1000)
	lData = (long)(_tstof(pDoc->WorkingInfo.Motion.sFdEngraveAoiInitDist) * 1000.0);
	pView->m_pMpe->Write(_T("ML45024"), lData);	// ���κο��� AOI(��)���� �Ÿ� (���� mm * 1000)
	lData = (long)(_tstof(pDoc->WorkingInfo.Motion.sFdBarcodeOffsetVel) * 1000.0);
	m_pMpe->Write(_T("ML45060"), lData);	// 2D ���ڵ� ��������ġ���� Feeding �ӵ� (���� mm/sec * 1000)
	lData = (long)(_tstof(pDoc->WorkingInfo.Motion.sFdBarcodeOffsetAcc) * 1000.0);
	m_pMpe->Write(_T("ML45062"), lData);	// 2D ���ڵ� ��������ġ���� Feeding ���ӵ� (���� mm/s^2 * 1000)

	lData = (long)(_tstof(pDoc->WorkingInfo.LastJob.sUltraSonicCleannerStTim) * 100.0);
	m_pMpe->Write(_T("MW05940"), lData);	// AOI_Dn (���� [��] * 100) : 1 is 10 mSec.
	m_pMpe->Write(_T("MW05942"), lData);	// AOI_Up (���� [��] * 100) : 1 is 10 mSec.
	
	m_pMpe->Write(_T("MB440177"), pDoc->WorkingInfo.LastJob.bUse380mm ? 1 : 0);	// EPC�Ǹ���(��ǰ��->OFF/��ǰ��->ON)
#endif
}

BOOL CGvisR2R_PunchView::SetCollision(double dCollisionMargin)
{
	BOOL bRtn = FALSE;
	if (m_pMotion)
		bRtn = m_pMotion->SetCollision(dCollisionMargin);
	return bRtn;
}

//BOOL CGvisR2R_PunchView::ChkXmpCollision()
//{
//	CString sM;
//	long addressActPos1, addressActPos2, addressDifferenceStored;
//	m_pMotion->GetData(&addressActPos1, &addressActPos2, &addressDifferenceStored);
//	sM.Format(_T("%d, %d, %d"), (int)addressActPos1, (int)addressActPos2, (int)addressDifferenceStored);
//	//DispStsBar(sM);
//	return TRUE;
//}

void CGvisR2R_PunchView::MpeWrite()
{
	for (int i = TOT_M_IO - 1; i >= 0; i--)
	{
		if (pDoc->m_pIo[i].nIdx > -1)
		{
			if (m_pMpe)
				m_pMpe->Write(pDoc->m_pIo[i].sAddr, pDoc->m_pIo[i].lData);
			pDoc->m_pIo[i].nIdx = -1;
			pDoc->m_pIo[i].sAddr = _T("");
			pDoc->m_pIo[i].lData = 0;
		}
	}
}

void CGvisR2R_PunchView::IoWrite(CString sMReg, long lData)
{
	int i = 0;
	for (i = 0; i<TOT_M_IO; i++)
	{
		if (pDoc->m_pIo[i].nIdx < 0)
		{
			pDoc->m_pIo[i].nIdx = i;
			pDoc->m_pIo[i].sAddr = sMReg;
			pDoc->m_pIo[i].lData = lData;
			break;
		}
	}
	if (i >= TOT_M_IO)
	{
		pView->ClrDispMsg();
		AfxMessageBox(_T("Not enought TOT_M_IO Num!!!"));
	}
}

BOOL CGvisR2R_PunchView::IsRdyTest()
{
	BOOL bOn0 = IsRdyTest0();
	BOOL bOn1 = IsRdyTest1();

	if (bOn0 && bOn1)
		return TRUE;
	return FALSE;
}

BOOL CGvisR2R_PunchView::IsRdyTest0()
{
#ifdef USE_MPE
	BOOL bOn0 = (pDoc->m_pMpeIb[10] & (0x01 << 11)) ? TRUE : FALSE;		// �˻�� �� �ڵ� ���� <-> X432B I/F
	BOOL bOn1 = (pDoc->m_pMpeIb[10] & (0x01 << 9)) ? TRUE : FALSE;		// �˻�� �� ���̺� ���� �Ϸ� <-> X4329 I/F

	if (bOn0 && bOn1)
	{
		pDoc->LogAuto(_T("PLC: �˻�� �� �ڵ�(X432B) ���� & ���̺� ����(X4329) �Ϸ�"));
		return TRUE;
	}
#endif
	return FALSE;
}

BOOL CGvisR2R_PunchView::IsRdyTest1()
{
#ifdef USE_MPE
	BOOL bOn0 = (pDoc->m_pMpeIb[14] & (0x01 << 11)) ? TRUE : FALSE;		// �˻�� �� �ڵ� ���� <-> X442B I/F
	BOOL bOn1 = (pDoc->m_pMpeIb[14] & (0x01 << 9)) ? TRUE : FALSE;		// �˻�� �� ���̺� ���� �Ϸ� <-> X4329 I/F
	double dCurPosMkFd = (double)pDoc->m_pMpeData[0][0];				// ��ŷ�� Feeding ���ڴ� ��(���� mm )
	double dTgtFd = _tstof(pDoc->WorkingInfo.Motion.sFdAoiAoiDistShot) * _tstof(pDoc->WorkingInfo.Motion.sAoiFdDist);
	if (dCurPosMkFd > dTgtFd - _tstof(pDoc->WorkingInfo.Motion.sAoiFdDist) / 2.0)
	{
		BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;
		if (bDualTest)
		{
			if (bOn0 && bOn1)
			{
				pDoc->LogAuto(_T("PLC: �˻�� �� �ڵ�(X442B) ���� & ���̺� ����(X4329) �Ϸ�"));
				return TRUE;
			}
		}
		else
			return TRUE;
		//else
		//{
		//	if (bOn0)
		//		return TRUE;
		//}
	}
#endif
	return FALSE;
}

BOOL CGvisR2R_PunchView::IsMk0Done()
{
	BOOL bDone = FALSE;

	if (pView->m_pVoiceCoil[0])
		bDone = pView->m_pVoiceCoil[0]->IsDoneMark(0);

	return bDone;
}

BOOL CGvisR2R_PunchView::IsMk1Done()
{
	BOOL bDone = FALSE;

	if (pView->m_pVoiceCoil[1])
		bDone = pView->m_pVoiceCoil[1]->IsDoneMark(1);

	return bDone;
}

void CGvisR2R_PunchView::SetPlcParam()
{
#ifdef USE_MPE
	if (!m_pMpe)
		return;

	//long lData;
	//IoWrite(_T("ML45006"), long(_tstof(pDoc->WorkingInfo.LastJob.sTempPauseLen)*1000.0));	// �Ͻ����� ���� (���� M * 1000)
	m_pMpe->Write(_T("ML45006"), long(_tstof(pDoc->WorkingInfo.LastJob.sTempPauseLen)*1000.0));
	//IoWrite(_T("ML45002"), long(_tstof(pDoc->WorkingInfo.LastJob.sLotSepLen)*1000.0));	// Lot �и� ���� (���� M * 1000)
	m_pMpe->Write(_T("ML45002"), long(_tstof(pDoc->WorkingInfo.LastJob.sLotSepLen)*1000.0));
	//IoWrite(_T("ML45004"), long(_tstof(pDoc->WorkingInfo.LastJob.sLotCutPosLen)*1000.0));	// Lot �и� �� ������ġ (���� M * 1000)
	m_pMpe->Write(_T("ML45004"), long(_tstof(pDoc->WorkingInfo.LastJob.sLotCutPosLen)*1000.0));
	m_pMpe->Write(_T("ML45126"), (long)_tstoi(pDoc->WorkingInfo.LastJob.sSampleTestShotNum));	// ���ð˻� Shot��

	if (pDoc->WorkingInfo.LastJob.bTempPause)
	{
		//pView->IoWrite(_T("MB440183"), 1);	// �Ͻ��������(PC�� On��Ű��, PLC�� Ȯ���ϰ� Off��Ŵ)-20141031
		m_pMpe->Write(_T("MB440183"), 1);
		ChkTempStop(TRUE);
	}
	else
	{
		//IoWrite(_T("MB440183"), 0);	// �Ͻ��������(PC�� On��Ű��, PLC�� Ȯ���ϰ� Off��Ŵ)-20141031
		m_pMpe->Write(_T("MB440183"), 0);
		ChkTempStop(FALSE);
	}

	if (pDoc->WorkingInfo.LastJob.bLotSep)
	{
		SetLotLastShot();
		m_pMpe->Write(_T("MB440184"), 1);
		//IoWrite(_T("MB440184"), 1);	// ��Ʈ�и����(PC�� On��Ű��, PC�� Ȯ���ϰ� Off��Ŵ)-20141031
	}
	else
		m_pMpe->Write(_T("MB440184"), 0);
	//IoWrite(_T("MB440184"), 0);	// ��Ʈ�и����(PC�� On��Ű��, PC�� Ȯ���ϰ� Off��Ŵ)-20141031

	m_pMpe->Write(_T("MB44017A"), (pDoc->WorkingInfo.LastJob.bDualTest) ? 0 : 1);		// �ܸ� �˻� On
	m_pMpe->Write(_T("MB44017B"), (pDoc->WorkingInfo.LastJob.bSampleTest) ? 1 : 0);		// Sample �˻� On
	m_pMpe->Write(_T("MB44017D"), (pDoc->WorkingInfo.LastJob.bOneMetal) ? 1 : 0);		// One Metal On
	m_pMpe->Write(_T("MB44017C"), (pDoc->WorkingInfo.LastJob.bTwoMetal) ? 1 : 0);		// Two Metal On
	m_pMpe->Write(_T("MB44017E"), (pDoc->WorkingInfo.LastJob.bCore150Recoiler) ? 1 : 0);	// Recoiler Core 150mm On
	m_pMpe->Write(_T("MB44017F"), (pDoc->WorkingInfo.LastJob.bCore150Uncoiler) ? 1 : 0);	// Uncoiler Core 150mm On

	m_pMpe->Write(_T("MB44010E"), (pDoc->WorkingInfo.LastJob.bUseAoiUpCleanRoler ? 1 : 0));
	m_pMpe->Write(_T("MB44010F"), (pDoc->WorkingInfo.LastJob.bUseAoiDnCleanRoler ? 1 : 0));
#endif
}

void CGvisR2R_PunchView::GetPlcParam()
{
	if (!m_pMpe)
		return;
#ifdef USE_MPE
	// Main
	pDoc->BtnStatus.Main.Ready = m_pMpe->Read(_T("MB005503")) ? TRUE : FALSE;	// ��ŷ�� �����غ� ����ġ
	pDoc->BtnStatus.Main.Run = m_pMpe->Read(_T("MB005501")) ? TRUE : FALSE;		// ��ŷ�� ���� ����ġ
	pDoc->BtnStatus.Main.Reset = m_pMpe->Read(_T("MB005504")) ? TRUE : FALSE;	// ��ŷ�� ���� ����ġ
	pDoc->BtnStatus.Main.Stop = m_pMpe->Read(_T("MB005502")) ? TRUE : FALSE;	// ��ŷ�� ���� ����ġ
	//pDoc->BtnStatus.Main.Auto = m_pMpe->Read(_T("MB005505")) ? TRUE : FALSE;	// ��ŷ�� �ڵ�/���� (ON)
	//pDoc->BtnStatus.Main.Manual = m_pMpe->Read(_T("MB005505")) ? FALSE : TRUE;	// ��ŷ�� �ڵ�/���� (OFF)
	pDoc->BtnStatus.Main.Auto = m_pMpe->Read(_T("MB003705")) ? TRUE : FALSE;	// ��ŷ�� �ڵ� ���� ����ġ ����
	pDoc->BtnStatus.Main.Manual = m_pMpe->Read(_T("MB003705")) ? FALSE : TRUE;	// ��ŷ�� �ڵ� ���� ����ġ ����

	if (pDoc->BtnStatus.Main.PrevReady != pDoc->BtnStatus.Main.Ready)
	{
		pDoc->BtnStatus.Main.PrevReady = pDoc->BtnStatus.Main.Ready;
		pDoc->SetMkMenu03(_T("Main"), _T("Ready"), pDoc->BtnStatus.Main.Ready);
	}
	if (pDoc->BtnStatus.Main.PrevRun != pDoc->BtnStatus.Main.Run)
	{
		pDoc->BtnStatus.Main.PrevRun = pDoc->BtnStatus.Main.Run;
		pDoc->SetMkMenu03(_T("Main"), _T("Run"), pDoc->BtnStatus.Main.Run);
		pDoc->SetMkMenu03(_T("Main"), _T("Stop"), !pDoc->BtnStatus.Main.Run);
	}
	if (pDoc->BtnStatus.Main.PrevReset != pDoc->BtnStatus.Main.Reset)
	{
		pDoc->BtnStatus.Main.PrevReset = pDoc->BtnStatus.Main.Reset;
		pDoc->SetMkMenu03(_T("Main"), _T("Reset"), pDoc->BtnStatus.Main.Reset);
	}
	if (pDoc->BtnStatus.Main.PrevStop != pDoc->BtnStatus.Main.Stop)
	{
		pDoc->BtnStatus.Main.PrevStop = pDoc->BtnStatus.Main.Stop;
		pDoc->SetMkMenu03(_T("Main"), _T("Stop"), pDoc->BtnStatus.Main.Stop);
		pDoc->SetMkMenu03(_T("Main"), _T("Run"), !pDoc->BtnStatus.Main.Stop);
	}
	if (pDoc->BtnStatus.Main.PrevAuto != pDoc->BtnStatus.Main.Auto)
	{
		pDoc->BtnStatus.Main.PrevAuto = pDoc->BtnStatus.Main.Auto;
		pDoc->SetMkMenu03(_T("Main"), _T("Auto"), pDoc->BtnStatus.Main.Auto);
		pDoc->SetMkMenu03(_T("Main"), _T("Manual"), !pDoc->BtnStatus.Main.Auto);
	}
	if (pDoc->BtnStatus.Main.PrevManual != pDoc->BtnStatus.Main.Manual)
	{
		pDoc->BtnStatus.Main.PrevManual = pDoc->BtnStatus.Main.Manual;
		pDoc->SetMkMenu03(_T("Main"), _T("Manual"), pDoc->BtnStatus.Main.Manual);
		pDoc->SetMkMenu03(_T("Main"), _T("Auto"), !pDoc->BtnStatus.Main.Manual);
	}

	// TorqueMotor
	pDoc->BtnStatus.Tq.Mk = m_pMpe->Read(_T("MB440155")) ? TRUE : FALSE;
	pDoc->BtnStatus.Tq.Aoi = m_pMpe->Read(_T("MB440156")) ? TRUE : FALSE;
	pDoc->BtnStatus.Tq.Eng = m_pMpe->Read(_T("MB440154")) ? TRUE : FALSE;

	if (pDoc->BtnStatus.Tq.PrevMk != pDoc->BtnStatus.Tq.Mk)
	{
		pDoc->BtnStatus.Tq.PrevMk = pDoc->BtnStatus.Tq.Mk;
		pDoc->SetMkMenu03(_T("TqMotor"), _T("MkTq"), pDoc->BtnStatus.Tq.Mk);
	}
	if (pDoc->BtnStatus.Tq.PrevAoi != pDoc->BtnStatus.Tq.Aoi)
	{
		pDoc->BtnStatus.Tq.PrevAoi = pDoc->BtnStatus.Tq.Aoi;
		pDoc->SetMkMenu03(_T("TqMotor"), _T("AoiTq"), pDoc->BtnStatus.Tq.Aoi);
	}
	if (pDoc->BtnStatus.Tq.PrevEng != pDoc->BtnStatus.Tq.Eng)
	{
		pDoc->BtnStatus.Tq.PrevEng = pDoc->BtnStatus.Tq.Eng;
		pDoc->SetMkMenu03(_T("TqMotor"), _T("EngTq"), pDoc->BtnStatus.Tq.Eng);
	}

	// InductionMotor
	pDoc->BtnStatus.Induct.Rc = m_pMpe->Read(_T("MB44017D")) ? TRUE : FALSE;	//pView->SetTwoMetal(FALSE, TRUE);	// One Metal IDC_CHK_68		
	pDoc->BtnStatus.Induct.Uc = m_pMpe->Read(_T("MB44017C")) ? TRUE : FALSE;	//pView->SetTwoMetal(TRUE, TRUE);	// Two Metal IDC_CHK_69

	if (pDoc->BtnStatus.Induct.PrevRc != pDoc->BtnStatus.Induct.Rc)
	{
		pDoc->BtnStatus.Induct.PrevRc = pDoc->BtnStatus.Induct.Rc;
		pDoc->SetMkMenu03(_T("Induction"), _T("RcCcw"), pDoc->BtnStatus.Induct.Rc);
	}
	if (pDoc->BtnStatus.Induct.PrevUc != pDoc->BtnStatus.Induct.Uc)
	{
		pDoc->BtnStatus.Induct.PrevUc = pDoc->BtnStatus.Induct.Uc;
		pDoc->SetMkMenu03(_T("Induction"), _T("UcCcw"), pDoc->BtnStatus.Induct.Uc);
	}

	// Core150mm
	pDoc->BtnStatus.Core150.Rc = m_pMpe->Read(_T("MB44017E")) ? TRUE : FALSE;	// SetCore150mmRecoiler(TRUE);	// Recoiler IDC_CHK_70	
	pDoc->BtnStatus.Core150.Uc = m_pMpe->Read(_T("MB44017F")) ? TRUE : FALSE;	// SetCore150mmUncoiler(TRUE);	// Uncoiler IDC_CHK_71	

	if (pDoc->BtnStatus.Core150.PrevRc != pDoc->BtnStatus.Core150.Rc)
	{
		pDoc->BtnStatus.Core150.PrevRc = pDoc->BtnStatus.Core150.Rc;
		pDoc->SetMkMenu03(_T("Core"), _T("Rc150"), pDoc->BtnStatus.Core150.Rc);
	}
	if (pDoc->BtnStatus.Core150.PrevUc != pDoc->BtnStatus.Core150.Uc)
	{
		pDoc->BtnStatus.Core150.PrevUc = pDoc->BtnStatus.Core150.Uc;
		pDoc->SetMkMenu03(_T("Core"), _T("Uc150"), pDoc->BtnStatus.Core150.Uc);
	}

	// Etc
	//pDoc->BtnStatus.Etc.EmgAoi = m_pMpe->Read(_T("")) ? TRUE : FALSE;

	// Recoiler
	pDoc->BtnStatus.Rc.Relation = m_pMpe->Read(_T("MB004001")) ? TRUE : FALSE;		// �����Ϸ� ���� ��/���� ����ġ ���� 
	pDoc->BtnStatus.Rc.FdCw = m_pMpe->Read(_T("MB00400C")) ? TRUE : FALSE;			// �����Ϸ� ��ǰ�� ��ȸ�� ����ġ ����
	pDoc->BtnStatus.Rc.FdCcw = m_pMpe->Read(_T("MB00400D")) ? TRUE : FALSE;			// �����Ϸ� ��ǰ�� ��ȸ�� ����ġ ����
	pDoc->BtnStatus.Rc.ReelChuck = m_pMpe->Read(_T("MB00400B")) ? TRUE : FALSE;		// �����Ϸ� ��ǰô Ŭ���� ����ġ ����
	pDoc->BtnStatus.Rc.DcRlUpDn = m_pMpe->Read(_T("MB004002")) ? TRUE : FALSE;		// �����Ϸ� ���� ���/�ϰ� ����ġ ����
	pDoc->BtnStatus.Rc.ReelJoinL = m_pMpe->Read(_T("MB004005")) ? TRUE : FALSE;		// �����Ϸ� ��ǰ ������(��/��) ����ġ ����
	pDoc->BtnStatus.Rc.ReelJoinR = m_pMpe->Read(_T("MB004006")) ? TRUE : FALSE;		// �����Ϸ� ��ǰ ������(��/��) ����ġ ����
	pDoc->BtnStatus.Rc.ReelJoinVac = m_pMpe->Read(_T("MB00400F")) ? TRUE : FALSE;	// �����Ϸ� ��ǰ ������ ���� ����ġ ����
	pDoc->BtnStatus.Rc.PprChuck = m_pMpe->Read(_T("MB004008")) ? TRUE : FALSE;		// �����Ϸ� ����ô Ŭ���� ����ġ ����
	pDoc->BtnStatus.Rc.PprCw = m_pMpe->Read(_T("MB004009")) ? TRUE : FALSE;			// �����Ϸ� ������ ��ȸ�� ����ġ ����
	pDoc->BtnStatus.Rc.PprCcw = m_pMpe->Read(_T("MB00400A")) ? TRUE : FALSE;		// �����Ϸ� ������ ��ȸ�� ����ġ ����
	pDoc->BtnStatus.Rc.Rewine = m_pMpe->Read(_T("MB004003")) ? TRUE : FALSE;		// �����Ϸ� Rewinder ���� ����ġ ����
	pDoc->BtnStatus.Rc.RewineReelPpr = m_pMpe->Read(_T("MB004004")) ? TRUE : FALSE;	// �����Ϸ� Rewinder ��ǰ & ���� ����ġ ����

	if (pDoc->BtnStatus.Rc.PrevRelation != pDoc->BtnStatus.Rc.Relation)
	{
		pDoc->BtnStatus.Rc.PrevRelation = pDoc->BtnStatus.Rc.Relation;
		pDoc->SetMkMenu03(_T("Main"), _T("Relation"), pDoc->BtnStatus.Rc.Relation);
	}
	if (pDoc->BtnStatus.Rc.PrevFdCw != pDoc->BtnStatus.Rc.FdCw)
	{
		pDoc->BtnStatus.Rc.PrevFdCw = pDoc->BtnStatus.Rc.FdCw;
		pDoc->SetMkMenu03(_T("Recoiler"), _T("MvCw"), pDoc->BtnStatus.Rc.FdCw);
	}
	if (pDoc->BtnStatus.Rc.PrevFdCcw != pDoc->BtnStatus.Rc.FdCcw)
	{
		pDoc->BtnStatus.Rc.PrevFdCcw = pDoc->BtnStatus.Rc.FdCcw;
		pDoc->SetMkMenu03(_T("Recoiler"), _T("MvCcw"), pDoc->BtnStatus.Rc.FdCcw);
	}
	if (pDoc->BtnStatus.Rc.PrevReelChuck != pDoc->BtnStatus.Rc.ReelChuck)
	{
		pDoc->BtnStatus.Rc.PrevReelChuck = pDoc->BtnStatus.Rc.ReelChuck;
		pDoc->SetMkMenu03(_T("Recoiler"), _T("PrdChuck"), pDoc->BtnStatus.Rc.ReelChuck);
	}
	if (pDoc->BtnStatus.Rc.PrevDcRlUpDn != pDoc->BtnStatus.Rc.DcRlUpDn)
	{
		pDoc->BtnStatus.Rc.PrevDcRlUpDn = pDoc->BtnStatus.Rc.DcRlUpDn;
		pDoc->SetMkMenu03(_T("Recoiler"), _T("DancerUp"), pDoc->BtnStatus.Rc.DcRlUpDn);
	}
	if (pDoc->BtnStatus.Rc.PrevReelJoinL != pDoc->BtnStatus.Rc.ReelJoinL)
	{
		pDoc->BtnStatus.Rc.PrevReelJoinL = pDoc->BtnStatus.Rc.ReelJoinL;
		pDoc->SetMkMenu03(_T("Recoiler"), _T("PasteUpLf"), pDoc->BtnStatus.Rc.ReelJoinL);
	}
	if (pDoc->BtnStatus.Rc.PrevReelJoinR != pDoc->BtnStatus.Rc.ReelJoinR)
	{
		pDoc->BtnStatus.Rc.PrevReelJoinR = pDoc->BtnStatus.Rc.ReelJoinR;
		pDoc->SetMkMenu03(_T("Recoiler"), _T("PasteUpRt"), pDoc->BtnStatus.Rc.ReelJoinR);
	}
	if (pDoc->BtnStatus.Rc.PrevReelJoinVac != pDoc->BtnStatus.Rc.ReelJoinVac)
	{
		pDoc->BtnStatus.Rc.PrevReelJoinVac = pDoc->BtnStatus.Rc.ReelJoinVac;
		pDoc->SetMkMenu03(_T("Recoiler"), _T("PasteVac"), pDoc->BtnStatus.Rc.ReelJoinVac);
	}
	if (pDoc->BtnStatus.Rc.PrevPprChuck != pDoc->BtnStatus.Rc.PprChuck)
	{
		pDoc->BtnStatus.Rc.PrevPprChuck = pDoc->BtnStatus.Rc.PprChuck;
		pDoc->SetMkMenu03(_T("Recoiler"), _T("PprChuck"), pDoc->BtnStatus.Rc.PprChuck);
	}
	if (pDoc->BtnStatus.Rc.PrevPprCw != pDoc->BtnStatus.Rc.PprCw)
	{
		pDoc->BtnStatus.Rc.PrevPprCw = pDoc->BtnStatus.Rc.PprCw;
		pDoc->SetMkMenu03(_T("Recoiler"), _T("PprCw"), pDoc->BtnStatus.Rc.PprCw);
	}
	if (pDoc->BtnStatus.Rc.PrevPprCcw != pDoc->BtnStatus.Rc.PprCcw)
	{
		pDoc->BtnStatus.Rc.PrevPprCcw = pDoc->BtnStatus.Rc.PprCcw;
		pDoc->SetMkMenu03(_T("Recoiler"), _T("PprCcw"), pDoc->BtnStatus.Rc.PprCcw);
	}
	if (pDoc->BtnStatus.Rc.PrevRewine != pDoc->BtnStatus.Rc.Rewine)
	{
		pDoc->BtnStatus.Rc.PrevRewine = pDoc->BtnStatus.Rc.Rewine;
		pDoc->SetMkMenu03(_T("Recoiler"), _T("DoRewind"), pDoc->BtnStatus.Rc.Rewine);
	}
	if (pDoc->BtnStatus.Rc.PrevRewineReelPpr != pDoc->BtnStatus.Rc.RewineReelPpr)
	{
		pDoc->BtnStatus.Rc.PrevRewineReelPpr = pDoc->BtnStatus.Rc.RewineReelPpr;
		pDoc->SetMkMenu03(_T("Recoiler"), _T("PrdPprRewind"), pDoc->BtnStatus.Rc.RewineReelPpr);
	}

	//pDoc->BtnStatus.Rc.Relation = m_pMpe->Read(_T("MB005801")) ? TRUE : FALSE;
	//pDoc->BtnStatus.Rc.FdCw = m_pMpe->Read(_T("MB00580C")) ? TRUE : FALSE;
	//pDoc->BtnStatus.Rc.FdCcw = m_pMpe->Read(_T("MB00580D")) ? TRUE : FALSE;
	//pDoc->BtnStatus.Rc.ReelChuck = m_pMpe->Read(_T("MB00580B")) ? TRUE : FALSE;
	//pDoc->BtnStatus.Rc.DcRlUpDn = m_pMpe->Read(_T("MB005802")) ? TRUE : FALSE;
	//pDoc->BtnStatus.Rc.ReelJoinL = m_pMpe->Read(_T("MB005805")) ? TRUE : FALSE;
	//pDoc->BtnStatus.Rc.ReelJoinR = m_pMpe->Read(_T("MB005806")) ? TRUE : FALSE;
	//pDoc->BtnStatus.Rc.ReelJoinVac = m_pMpe->Read(_T("MB00580F")) ? TRUE : FALSE;
	//pDoc->BtnStatus.Rc.PprChuck = m_pMpe->Read(_T("MB005808")) ? TRUE : FALSE;
	//pDoc->BtnStatus.Rc.PprCw = m_pMpe->Read(_T("MB005809")) ? TRUE : FALSE;
	//pDoc->BtnStatus.Rc.PprCcw = m_pMpe->Read(_T("MB00580A")) ? TRUE : FALSE;
	//pDoc->BtnStatus.Rc.Rewine = m_pMpe->Read(_T("MB005803")) ? TRUE : FALSE;
	//pDoc->BtnStatus.Rc.RewineReelPpr = m_pMpe->Read(_T("MB005804")) ? TRUE : FALSE;

	// Punch
	pDoc->BtnStatus.Mk.Relation = m_pMpe->Read(_T("MB003711")) ? TRUE : FALSE;	// ��ŷ�� ���� ��/���� ����ġ ����
	pDoc->BtnStatus.Mk.FdCw = m_pMpe->Read(_T("MB003713")) ? TRUE : FALSE;		// ��ŷ�� �ǵ� ��ȸ�� ����ġ ����
	pDoc->BtnStatus.Mk.FdCcw = m_pMpe->Read(_T("MB003714")) ? TRUE : FALSE;		// ��ŷ�� �ǵ� ��ȸ�� ����ġ ����
	pDoc->BtnStatus.Mk.FdVac = m_pMpe->Read(_T("MB003715")) ? TRUE : FALSE;		// ��ŷ�� �ǵ� ���� ����ġ ����
	pDoc->BtnStatus.Mk.PushUp = m_pMpe->Read(_T("MB003712")) ? TRUE : FALSE;	// ��ŷ�� ���̺� ��ο� ����ġ ����
	pDoc->BtnStatus.Mk.TblBlw = m_pMpe->Read(_T("MB003717")) ? TRUE : FALSE;	// ��ŷ�� ���̺� ���� ����ġ ����
	pDoc->BtnStatus.Mk.TblVac = m_pMpe->Read(_T("MB003716")) ? TRUE : FALSE;	// ��ŷ�� ��ũ ���� ����ġ ����
	pDoc->BtnStatus.Mk.FdClp = m_pMpe->Read(_T("MB003719")) ? TRUE : FALSE;		// ��ŷ�� �ǵ� Ŭ���� ����ġ ����  
	pDoc->BtnStatus.Mk.TqClp = m_pMpe->Read(_T("MB00371A")) ? TRUE : FALSE;		// ��ŷ�� �ټ� Ŭ���� ����ġ ����
	pDoc->BtnStatus.Mk.MvOne = m_pMpe->Read(_T("MB440151")) ? TRUE : FALSE;
	pDoc->BtnStatus.Mk.LsrPt = m_pMpe->Read(_T("MB003718")) ? TRUE : FALSE;		// ��ŷ�� ������ ������ ����ġ ����
	pDoc->BtnStatus.Mk.DcRSol = m_pMpe->Read(_T("MB00371B")) ? TRUE : FALSE;	// ��ŷ�� ���� ���/�ϰ� ����ġ ����

	if (pDoc->BtnStatus.Mk.PrevRelation != pDoc->BtnStatus.Mk.Relation)
	{
		pDoc->BtnStatus.Mk.PrevRelation = pDoc->BtnStatus.Mk.Relation;
		pDoc->SetMkMenu03(_T("Main"), _T("Relation"), pDoc->BtnStatus.Mk.Relation);
	}
	if (pDoc->BtnStatus.Mk.PrevFdCw != pDoc->BtnStatus.Mk.FdCw)
	{
		pDoc->BtnStatus.Mk.PrevFdCw = pDoc->BtnStatus.Mk.FdCw;
		pDoc->SetMkMenu03(_T("Punching"), _T("MvCw"), pDoc->BtnStatus.Mk.FdCw);
	}
	if (pDoc->BtnStatus.Mk.PrevFdCcw != pDoc->BtnStatus.Mk.FdCcw)
	{
		pDoc->BtnStatus.Mk.PrevFdCcw = pDoc->BtnStatus.Mk.FdCcw;
		pDoc->SetMkMenu03(_T("Punching"), _T("MvCcw"), pDoc->BtnStatus.Mk.FdCcw);
	}
	if (pDoc->BtnStatus.Mk.PrevFdVac != pDoc->BtnStatus.Mk.FdVac)
	{
		pDoc->BtnStatus.Mk.PrevFdVac = pDoc->BtnStatus.Mk.FdVac;
		pDoc->SetMkMenu03(_T("Punching"), _T("FdVac"), pDoc->BtnStatus.Mk.FdVac);
	}
	if (pDoc->BtnStatus.Mk.PrevPushUp != pDoc->BtnStatus.Mk.PushUp)
	{
		pDoc->BtnStatus.Mk.PrevPushUp = pDoc->BtnStatus.Mk.PushUp;
		pDoc->SetMkMenu03(_T("Punching"), _T("PushUp"), pDoc->BtnStatus.Mk.PushUp);
	}
	if (pDoc->BtnStatus.Mk.PrevTblBlw != pDoc->BtnStatus.Mk.TblBlw)
	{
		pDoc->BtnStatus.Mk.PrevTblBlw = pDoc->BtnStatus.Mk.TblBlw;
		pDoc->SetMkMenu03(_T("Punching"), _T("TblBlw"), pDoc->BtnStatus.Mk.TblBlw);
	}
	if (pDoc->BtnStatus.Mk.PrevTblVac != pDoc->BtnStatus.Mk.TblVac)
	{
		pDoc->BtnStatus.Mk.PrevTblVac = pDoc->BtnStatus.Mk.TblVac;
		pDoc->SetMkMenu03(_T("Punching"), _T("TblVac"), pDoc->BtnStatus.Mk.TblVac);
	}
	if (pDoc->BtnStatus.Mk.PrevFdClp != pDoc->BtnStatus.Mk.FdClp)
	{
		pDoc->BtnStatus.Mk.PrevFdClp = pDoc->BtnStatus.Mk.FdClp;
		pDoc->SetMkMenu03(_T("Punching"), _T("FdClamp"), pDoc->BtnStatus.Mk.FdClp);
	}
	if (pDoc->BtnStatus.Mk.PrevTqClp != pDoc->BtnStatus.Mk.TqClp)
	{
		pDoc->BtnStatus.Mk.PrevTqClp = pDoc->BtnStatus.Mk.TqClp;
		pDoc->SetMkMenu03(_T("Punching"), _T("TensClamp"), pDoc->BtnStatus.Mk.TqClp);
	}
	if (pDoc->BtnStatus.Mk.PrevMvOne != pDoc->BtnStatus.Mk.MvOne)
	{
		pDoc->BtnStatus.Mk.PrevMvOne = pDoc->BtnStatus.Mk.MvOne;
		pDoc->SetMkMenu03(_T("Punching"), _T("OnePnl"), pDoc->BtnStatus.Mk.MvOne);
	}
	if (pDoc->BtnStatus.Mk.PrevLsrPt != pDoc->BtnStatus.Mk.LsrPt)
	{
		pDoc->BtnStatus.Mk.PrevLsrPt = pDoc->BtnStatus.Mk.LsrPt;
		pDoc->SetMkMenu03(_T("Punching"), _T("Lsr"), pDoc->BtnStatus.Mk.LsrPt);
	}
	if (pDoc->BtnStatus.Mk.PrevDcRSol != pDoc->BtnStatus.Mk.DcRSol)
	{
		pDoc->BtnStatus.Mk.PrevDcRSol = pDoc->BtnStatus.Mk.DcRSol;
		pDoc->SetMkMenu03(_T("Punching"), _T("DancerUp"), pDoc->BtnStatus.Mk.DcRSol);
	}

	//pDoc->BtnStatus.Mk.Relation = m_pMpe->Read(_T("MB005511")) ? TRUE : FALSE;
	//pDoc->BtnStatus.Mk.FdCw = m_pMpe->Read(_T("MB005513")) ? TRUE : FALSE;
	//pDoc->BtnStatus.Mk.FdCcw = m_pMpe->Read(_T("MB005514")) ? TRUE : FALSE;
	//pDoc->BtnStatus.Mk.FdVac = m_pMpe->Read(_T("MB005515")) ? TRUE : FALSE;
	//pDoc->BtnStatus.Mk.PushUp = m_pMpe->Read(_T("MB005516")) ? TRUE : FALSE;
	//pDoc->BtnStatus.Mk.TblBlw = m_pMpe->Read(_T("MB005512")) ? TRUE : FALSE;
	//pDoc->BtnStatus.Mk.TblVac = m_pMpe->Read(_T("MB005517")) ? TRUE : FALSE;
	//pDoc->BtnStatus.Mk.FdClp = m_pMpe->Read(_T("MB005519")) ? TRUE : FALSE;
	//pDoc->BtnStatus.Mk.TqClp = m_pMpe->Read(_T("MB00551A")) ? TRUE : FALSE;
	//pDoc->BtnStatus.Mk.MvOne = m_pMpe->Read(_T("MB440151")) ? TRUE : FALSE;
	//pDoc->BtnStatus.Mk.LsrPt = m_pMpe->Read(_T("MB005518")) ? TRUE : FALSE;
	//pDoc->BtnStatus.Mk.DcRSol = m_pMpe->Read(_T("MB00551B")) ? TRUE : FALSE;

	// AOIDn
	pDoc->BtnStatus.AoiDn.Relation = m_pMpe->Read(_T("MB003901")) ? TRUE : FALSE;	// �˻�� �� ���� ��/���� ����ġ ����
	pDoc->BtnStatus.AoiDn.FdCw = m_pMpe->Read(_T("MB003903")) ? TRUE : FALSE;		// �˻�� �� �ǵ� ��ȸ�� ����ġ ����
	pDoc->BtnStatus.AoiDn.FdCcw = m_pMpe->Read(_T("MB003904")) ? TRUE : FALSE;		// �˻�� �� �ǵ� ��ȸ�� ����ġ ����
	pDoc->BtnStatus.AoiDn.FdVac = m_pMpe->Read(_T("MB003905")) ? TRUE : FALSE;		// �˻�� �� �ǵ� ���� ����ġ ����
	pDoc->BtnStatus.AoiDn.PushUp = m_pMpe->Read(_T("MB003906")) ? TRUE : FALSE;		// �˻�� �� ��ũ ���� ����ġ ����
	pDoc->BtnStatus.AoiDn.TblBlw = m_pMpe->Read(_T("MB003902")) ? TRUE : FALSE;		// �˻�� �� ���̺� ��ο� ����ġ ����
	pDoc->BtnStatus.AoiDn.TblVac = m_pMpe->Read(_T("MB003907")) ? TRUE : FALSE;		// �˻�� �� ���̺� ���� ����ġ ����
	pDoc->BtnStatus.AoiDn.FdClp = m_pMpe->Read(_T("MB003909")) ? TRUE : FALSE;		// �˻�� �� �ǵ� Ŭ���� ����ġ ����
	pDoc->BtnStatus.AoiDn.TqClp = m_pMpe->Read(_T("MB00390A")) ? TRUE : FALSE;		// �˻�� �� �ټ� Ŭ���� ����ġ ����
	pDoc->BtnStatus.AoiDn.MvOne = m_pMpe->Read(_T("MB440151")) ? TRUE : FALSE;
	pDoc->BtnStatus.AoiDn.LsrPt = m_pMpe->Read(_T("MB003908")) ? TRUE : FALSE;		// �˻�� �� ������ ������ ����ġ ����
	pDoc->BtnStatus.AoiDn.VelSonicBlw = m_pMpe->Read(_T("MB44014F")) ? TRUE : FALSE;

	if (pDoc->BtnStatus.AoiDn.PrevRelation != pDoc->BtnStatus.AoiDn.Relation)
	{
		pDoc->BtnStatus.AoiDn.PrevRelation = pDoc->BtnStatus.AoiDn.Relation;
		pDoc->SetMkMenu03(_T("Main"), _T("Relation"), pDoc->BtnStatus.AoiDn.Relation);
	}
	if (pDoc->BtnStatus.AoiDn.PrevFdCw != pDoc->BtnStatus.AoiDn.FdCw)
	{
		pDoc->BtnStatus.AoiDn.PrevFdCw = pDoc->BtnStatus.AoiDn.FdCw;
		pDoc->SetMkMenu03(_T("AoiDn"), _T("MvCw"), pDoc->BtnStatus.AoiDn.FdCw);
	}
	if (pDoc->BtnStatus.AoiDn.PrevFdCcw != pDoc->BtnStatus.AoiDn.FdCcw)
	{
		pDoc->BtnStatus.AoiDn.PrevFdCcw = pDoc->BtnStatus.AoiDn.FdCcw;
		pDoc->SetMkMenu03(_T("AoiDn"), _T("MvCcw"), pDoc->BtnStatus.AoiDn.FdCcw);
	}
	if (pDoc->BtnStatus.AoiDn.PrevFdVac != pDoc->BtnStatus.AoiDn.FdVac)
	{
		pDoc->BtnStatus.AoiDn.PrevFdVac = pDoc->BtnStatus.AoiDn.FdVac;
		pDoc->SetMkMenu03(_T("AoiDn"), _T("FdVac"), pDoc->BtnStatus.AoiDn.FdVac);
	}
	if (pDoc->BtnStatus.AoiDn.PrevPushUp != pDoc->BtnStatus.AoiDn.PushUp)
	{
		pDoc->BtnStatus.AoiDn.PrevPushUp = pDoc->BtnStatus.AoiDn.PushUp;
		pDoc->SetMkMenu03(_T("AoiDn"), _T("PushUp"), pDoc->BtnStatus.AoiDn.PushUp);
	}
	if (pDoc->BtnStatus.AoiDn.PrevTblBlw != pDoc->BtnStatus.AoiDn.TblBlw)
	{
		pDoc->BtnStatus.AoiDn.PrevTblBlw = pDoc->BtnStatus.AoiDn.TblBlw;
		pDoc->SetMkMenu03(_T("AoiDn"), _T("TblBlw"), pDoc->BtnStatus.AoiDn.TblBlw);
	}
	if (pDoc->BtnStatus.AoiDn.PrevTblVac != pDoc->BtnStatus.AoiDn.TblVac)
	{
		pDoc->BtnStatus.AoiDn.PrevTblVac = pDoc->BtnStatus.AoiDn.TblVac;
		pDoc->SetMkMenu03(_T("AoiDn"), _T("TblVac"), pDoc->BtnStatus.AoiDn.TblVac);
	}
	if (pDoc->BtnStatus.AoiDn.PrevFdClp != pDoc->BtnStatus.AoiDn.FdClp)
	{
		pDoc->BtnStatus.AoiDn.PrevFdClp = pDoc->BtnStatus.AoiDn.FdClp;
		pDoc->SetMkMenu03(_T("AoiDn"), _T("FdClamp"), pDoc->BtnStatus.AoiDn.FdClp);
	}
	if (pDoc->BtnStatus.AoiDn.PrevTqClp != pDoc->BtnStatus.AoiDn.TqClp)
	{
		pDoc->BtnStatus.AoiDn.PrevTqClp = pDoc->BtnStatus.AoiDn.TqClp;
		pDoc->SetMkMenu03(_T("AoiDn"), _T("TensClamp"), pDoc->BtnStatus.AoiDn.TqClp);
	}
	if (pDoc->BtnStatus.AoiDn.PrevMvOne != pDoc->BtnStatus.AoiDn.MvOne)
	{
		pDoc->BtnStatus.AoiDn.PrevMvOne = pDoc->BtnStatus.AoiDn.MvOne;
		pDoc->SetMkMenu03(_T("AoiDn"), _T("OnePnl"), pDoc->BtnStatus.AoiDn.MvOne);
	}
	if (pDoc->BtnStatus.AoiDn.PrevLsrPt != pDoc->BtnStatus.AoiDn.LsrPt)
	{
		pDoc->BtnStatus.AoiDn.PrevLsrPt = pDoc->BtnStatus.AoiDn.LsrPt;
		pDoc->SetMkMenu03(_T("AoiDn"), _T("Lsr"), pDoc->BtnStatus.AoiDn.LsrPt);
	}
	if (pDoc->BtnStatus.AoiDn.PrevVelSonicBlw != pDoc->BtnStatus.AoiDn.VelSonicBlw)
	{
		pDoc->BtnStatus.AoiDn.PrevVelSonicBlw = pDoc->BtnStatus.AoiDn.VelSonicBlw;
		pDoc->SetMkMenu03(_T("AoiDn"), _T("VelClrSonic"), pDoc->BtnStatus.AoiDn.VelSonicBlw);
	}

	//pDoc->BtnStatus.AoiDn.Relation = m_pMpe->Read(_T("MB005701")) ? TRUE : FALSE;
	//pDoc->BtnStatus.AoiDn.FdCw = m_pMpe->Read(_T("MB005703")) ? TRUE : FALSE;
	//pDoc->BtnStatus.AoiDn.FdCcw = m_pMpe->Read(_T("MB005704")) ? TRUE : FALSE;
	//pDoc->BtnStatus.AoiDn.FdVac = m_pMpe->Read(_T("MB005705")) ? TRUE : FALSE;
	//pDoc->BtnStatus.AoiDn.PushUp = m_pMpe->Read(_T("MB005706")) ? TRUE : FALSE;
	//pDoc->BtnStatus.AoiDn.TblBlw = m_pMpe->Read(_T("MB005702")) ? TRUE : FALSE;
	//pDoc->BtnStatus.AoiDn.TblVac = m_pMpe->Read(_T("MB005707")) ? TRUE : FALSE;
	//pDoc->BtnStatus.AoiDn.FdClp = m_pMpe->Read(_T("MB005709")) ? TRUE : FALSE;
	//pDoc->BtnStatus.AoiDn.TqClp = m_pMpe->Read(_T("MB00570A")) ? TRUE : FALSE;
	//pDoc->BtnStatus.AoiDn.MvOne = m_pMpe->Read(_T("MB440151")) ? TRUE : FALSE;
	//pDoc->BtnStatus.AoiDn.LsrPt = m_pMpe->Read(_T("MB005708")) ? TRUE : FALSE;
	//pDoc->BtnStatus.AoiDn.VelSonicBlw = m_pMpe->Read(_T("MB44014F")) ? TRUE : FALSE;

	// AOIUp
	pDoc->BtnStatus.AoiUp.Relation = m_pMpe->Read(_T("MB003801")) ? TRUE : FALSE;	// �˻�� �� ���� ��/���� ����ġ ����
	pDoc->BtnStatus.AoiUp.FdCw = m_pMpe->Read(_T("MB003803")) ? TRUE : FALSE;		// �˻�� �� �ǵ� ��ȸ�� ����ġ ����
	pDoc->BtnStatus.AoiUp.FdCcw = m_pMpe->Read(_T("MB003804")) ? TRUE : FALSE;		// �˻�� �� �ǵ� ��ȸ�� ����ġ ����
	pDoc->BtnStatus.AoiUp.FdVac = m_pMpe->Read(_T("MB003805")) ? TRUE : FALSE;		// �˻�� �� �ǵ� ���� ����ġ ����
	pDoc->BtnStatus.AoiUp.PushUp = m_pMpe->Read(_T("MB003806")) ? TRUE : FALSE;		// �˻�� �� ��ũ ���� ����ġ ����
	pDoc->BtnStatus.AoiUp.TblBlw = m_pMpe->Read(_T("MB003802")) ? TRUE : FALSE;		// �˻�� �� ���̺� ��ο� ����ġ ����
	pDoc->BtnStatus.AoiUp.TblVac = m_pMpe->Read(_T("MB003807")) ? TRUE : FALSE;		// �˻�� �� ���̺� ���� ����ġ ����
	pDoc->BtnStatus.AoiUp.FdClp = m_pMpe->Read(_T("MB003809")) ? TRUE : FALSE;		// �˻�� �� �ǵ� Ŭ���� ����ġ ����
	pDoc->BtnStatus.AoiUp.TqClp = m_pMpe->Read(_T("MB00380A")) ? TRUE : FALSE;		// �˻�� �� �ټ� Ŭ���� ����ġ ����
	pDoc->BtnStatus.AoiUp.MvOne = m_pMpe->Read(_T("MB440151")) ? TRUE : FALSE;
	pDoc->BtnStatus.AoiUp.LsrPt = m_pMpe->Read(_T("MB003808")) ? TRUE : FALSE;		// �˻�� �� ������ ������ ����ġ ����

	if (pDoc->BtnStatus.AoiUp.PrevRelation != pDoc->BtnStatus.AoiUp.Relation)
	{
		pDoc->BtnStatus.AoiUp.PrevRelation = pDoc->BtnStatus.AoiUp.Relation;
		pDoc->SetMkMenu03(_T("Main"), _T("Relation"), pDoc->BtnStatus.AoiUp.Relation);
	}
	if (pDoc->BtnStatus.AoiUp.PrevFdCw != pDoc->BtnStatus.AoiUp.FdCw)
	{
		pDoc->BtnStatus.AoiUp.PrevFdCw = pDoc->BtnStatus.AoiUp.FdCw;
		pDoc->SetMkMenu03(_T("AoiUp"), _T("MvCw"), pDoc->BtnStatus.AoiUp.FdCw);
	}
	if (pDoc->BtnStatus.AoiUp.PrevFdCcw != pDoc->BtnStatus.AoiUp.FdCcw)
	{
		pDoc->BtnStatus.AoiUp.PrevFdCcw = pDoc->BtnStatus.AoiUp.FdCcw;
		pDoc->SetMkMenu03(_T("AoiUp"), _T("MvCcw"), pDoc->BtnStatus.AoiUp.FdCcw);
	}
	if (pDoc->BtnStatus.AoiUp.PrevFdVac != pDoc->BtnStatus.AoiUp.FdVac)
	{
		pDoc->BtnStatus.AoiUp.PrevFdVac = pDoc->BtnStatus.AoiUp.FdVac;
		pDoc->SetMkMenu03(_T("AoiUp"), _T("FdVac"), pDoc->BtnStatus.AoiUp.FdVac);
	}
	if (pDoc->BtnStatus.AoiUp.PrevPushUp != pDoc->BtnStatus.AoiUp.PushUp)
	{
		pDoc->BtnStatus.AoiUp.PrevPushUp = pDoc->BtnStatus.AoiUp.PushUp;
		pDoc->SetMkMenu03(_T("AoiUp"), _T("PushUp"), pDoc->BtnStatus.AoiUp.PushUp);
	}
	if (pDoc->BtnStatus.AoiUp.PrevTblBlw != pDoc->BtnStatus.AoiUp.TblBlw)
	{
		pDoc->BtnStatus.AoiUp.PrevTblBlw = pDoc->BtnStatus.AoiUp.TblBlw;
		pDoc->SetMkMenu03(_T("AoiUp"), _T("TblBlw"), pDoc->BtnStatus.AoiUp.TblBlw);
	}
	if (pDoc->BtnStatus.AoiUp.PrevTblVac != pDoc->BtnStatus.AoiUp.TblVac)
	{
		pDoc->BtnStatus.AoiUp.PrevTblVac = pDoc->BtnStatus.AoiUp.TblVac;
		pDoc->SetMkMenu03(_T("AoiUp"), _T("TblVac"), pDoc->BtnStatus.AoiUp.TblVac);
	}
	if (pDoc->BtnStatus.AoiUp.PrevFdClp != pDoc->BtnStatus.AoiUp.FdClp)
	{
		pDoc->BtnStatus.AoiUp.PrevFdClp = pDoc->BtnStatus.AoiUp.FdClp;
		pDoc->SetMkMenu03(_T("AoiUp"), _T("FdClamp"), pDoc->BtnStatus.AoiUp.FdClp);
	}
	if (pDoc->BtnStatus.AoiUp.PrevTqClp != pDoc->BtnStatus.AoiUp.TqClp)
	{
		pDoc->BtnStatus.AoiUp.PrevTqClp = pDoc->BtnStatus.AoiUp.TqClp;
		pDoc->SetMkMenu03(_T("AoiUp"), _T("TensClamp"), pDoc->BtnStatus.AoiUp.TqClp);
	}
	if (pDoc->BtnStatus.AoiUp.PrevMvOne != pDoc->BtnStatus.AoiUp.MvOne)
	{
		pDoc->BtnStatus.AoiUp.PrevMvOne = pDoc->BtnStatus.AoiUp.MvOne;
		pDoc->SetMkMenu03(_T("AoiUp"), _T("OnePnl"), pDoc->BtnStatus.AoiUp.MvOne);
	}
	if (pDoc->BtnStatus.AoiUp.PrevLsrPt != pDoc->BtnStatus.AoiUp.LsrPt)
	{
		pDoc->BtnStatus.AoiUp.PrevLsrPt = pDoc->BtnStatus.AoiUp.LsrPt;
		pDoc->SetMkMenu03(_T("AoiUp"), _T("Lsr"), pDoc->BtnStatus.AoiUp.LsrPt);
	}

	//pDoc->BtnStatus.AoiUp.Relation = m_pMpe->Read(_T("MB005601")) ? TRUE : FALSE;
	//pDoc->BtnStatus.AoiUp.FdCw = m_pMpe->Read(_T("MB005603")) ? TRUE : FALSE;
	//pDoc->BtnStatus.AoiUp.FdCcw = m_pMpe->Read(_T("MB005604")) ? TRUE : FALSE;
	//pDoc->BtnStatus.AoiUp.FdVac = m_pMpe->Read(_T("MB005605")) ? TRUE : FALSE;
	//pDoc->BtnStatus.AoiUp.PushUp = m_pMpe->Read(_T("MB005606")) ? TRUE : FALSE;
	//pDoc->BtnStatus.AoiUp.TblBlw = m_pMpe->Read(_T("MB005602")) ? TRUE : FALSE;
	//pDoc->BtnStatus.AoiUp.TblVac = m_pMpe->Read(_T("MB005607")) ? TRUE : FALSE;
	//pDoc->BtnStatus.AoiUp.FdClp = m_pMpe->Read(_T("MB005609")) ? TRUE : FALSE;
	//pDoc->BtnStatus.AoiUp.TqClp = m_pMpe->Read(_T("MB00560A")) ? TRUE : FALSE;
	//pDoc->BtnStatus.AoiUp.MvOne = m_pMpe->Read(_T("MB440151")) ? TRUE : FALSE;
	//pDoc->BtnStatus.AoiUp.LsrPt = m_pMpe->Read(_T("MB005608")) ? TRUE : FALSE;

	// Engrave
	pDoc->BtnStatus.Eng.Relation = m_pMpe->Read(_T("MB004511")) ? TRUE : FALSE;
	pDoc->BtnStatus.Eng.FdCw = m_pMpe->Read(_T("MB004513")) ? TRUE : FALSE;
	pDoc->BtnStatus.Eng.FdCcw = m_pMpe->Read(_T("MB004514")) ? TRUE : FALSE;
	pDoc->BtnStatus.Eng.FdVac = m_pMpe->Read(_T("MB004515")) ? TRUE : FALSE;
	pDoc->BtnStatus.Eng.PushUp = m_pMpe->Read(_T("MB004516")) ? TRUE : FALSE;
	pDoc->BtnStatus.Eng.TblBlw = m_pMpe->Read(_T("MB00451F")) ? TRUE : FALSE;
	pDoc->BtnStatus.Eng.TblVac = m_pMpe->Read(_T("MB004517")) ? TRUE : FALSE;
	pDoc->BtnStatus.Eng.FdClp = m_pMpe->Read(_T("MB004519")) ? TRUE : FALSE;
	pDoc->BtnStatus.Eng.TqClp = m_pMpe->Read(_T("MB00451A")) ? TRUE : FALSE;
	pDoc->BtnStatus.Eng.MvOne = m_pMpe->Read(_T("MB440151")) ? TRUE : FALSE;
	pDoc->BtnStatus.Eng.LsrPt = m_pMpe->Read(_T("MB004518")) ? TRUE : FALSE;
	pDoc->BtnStatus.Eng.DcRSol = m_pMpe->Read(_T("MB00451B")) ? TRUE : FALSE;
	pDoc->BtnStatus.Eng.VelSonicBlw = m_pMpe->Read(_T("MB44014E")) ? TRUE : FALSE;

	if (pDoc->BtnStatus.Eng.PrevRelation != pDoc->BtnStatus.Eng.Relation)
	{
		pDoc->BtnStatus.Eng.PrevRelation = pDoc->BtnStatus.Eng.Relation;
		pDoc->SetMkMenu03(_T("Main"), _T("Relation"), pDoc->BtnStatus.Eng.Relation);
	}
	if (pDoc->BtnStatus.Eng.PrevFdCw != pDoc->BtnStatus.Eng.FdCw)
	{
		pDoc->BtnStatus.Eng.PrevFdCw = pDoc->BtnStatus.Eng.FdCw;
		pDoc->SetMkMenu03(_T("Engraving"), _T("MvCw"), pDoc->BtnStatus.Eng.FdCw);
	}
	if (pDoc->BtnStatus.Eng.PrevFdCcw != pDoc->BtnStatus.Eng.FdCcw)
	{
		pDoc->BtnStatus.Eng.PrevFdCcw = pDoc->BtnStatus.Eng.FdCcw;
		pDoc->SetMkMenu03(_T("Engraving"), _T("MvCcw"), pDoc->BtnStatus.Eng.FdCcw);
	}
	if (pDoc->BtnStatus.Eng.PrevFdVac != pDoc->BtnStatus.Eng.FdVac)
	{
		pDoc->BtnStatus.Eng.PrevFdVac = pDoc->BtnStatus.Eng.FdVac;
		pDoc->SetMkMenu03(_T("Engraving"), _T("FdVac"), pDoc->BtnStatus.Eng.FdVac);
	}
	if (pDoc->BtnStatus.Eng.PrevPushUp != pDoc->BtnStatus.Eng.PushUp)
	{
		pDoc->BtnStatus.Eng.PrevPushUp = pDoc->BtnStatus.Eng.PushUp;
		pDoc->SetMkMenu03(_T("Engraving"), _T("PushUp"), pDoc->BtnStatus.Eng.PushUp);
	}
	if (pDoc->BtnStatus.Eng.PrevTblBlw != pDoc->BtnStatus.Eng.TblBlw)
	{
		pDoc->BtnStatus.Eng.PrevTblBlw = pDoc->BtnStatus.Eng.TblBlw;
		pDoc->SetMkMenu03(_T("Engraving"), _T("TblBlw"), pDoc->BtnStatus.Eng.TblBlw);
	}
	if (pDoc->BtnStatus.Eng.PrevTblVac != pDoc->BtnStatus.Eng.TblVac)
	{
		pDoc->BtnStatus.Eng.PrevTblVac = pDoc->BtnStatus.Eng.TblVac;
		pDoc->SetMkMenu03(_T("Engraving"), _T("TblVac"), pDoc->BtnStatus.Eng.TblVac);
	}
	if (pDoc->BtnStatus.Eng.PrevFdClp != pDoc->BtnStatus.Eng.FdClp)
	{
		pDoc->BtnStatus.Eng.PrevFdClp = pDoc->BtnStatus.Eng.FdClp;
		pDoc->SetMkMenu03(_T("Engraving"), _T("FdClamp"), pDoc->BtnStatus.Eng.FdClp);
	}
	if (pDoc->BtnStatus.Eng.PrevTqClp != pDoc->BtnStatus.Eng.TqClp)
	{
		pDoc->BtnStatus.Eng.PrevTqClp = pDoc->BtnStatus.Eng.TqClp;
		pDoc->SetMkMenu03(_T("Engraving"), _T("TensClamp"), pDoc->BtnStatus.Eng.TqClp);
	}
	if (pDoc->BtnStatus.Eng.PrevMvOne != pDoc->BtnStatus.Eng.MvOne)
	{
		pDoc->BtnStatus.Eng.PrevMvOne = pDoc->BtnStatus.Eng.MvOne;
		pDoc->SetMkMenu03(_T("Engraving"), _T("OnePnl"), pDoc->BtnStatus.Eng.MvOne);
	}
	if (pDoc->BtnStatus.Eng.PrevLsrPt != pDoc->BtnStatus.Eng.LsrPt)
	{
		pDoc->BtnStatus.Eng.PrevLsrPt = pDoc->BtnStatus.Eng.LsrPt;
		pDoc->SetMkMenu03(_T("Engraving"), _T("Lsr"), pDoc->BtnStatus.Eng.LsrPt);
	}
	if (pDoc->BtnStatus.Eng.PrevVelSonicBlw != pDoc->BtnStatus.Eng.VelSonicBlw)
	{
		pDoc->BtnStatus.Eng.PrevVelSonicBlw = pDoc->BtnStatus.Eng.VelSonicBlw;
		pDoc->SetMkMenu03(_T("Engraving"), _T("VelClrSonic"), pDoc->BtnStatus.Eng.VelSonicBlw);
	}
	if (pDoc->BtnStatus.Eng.PrevDcRSol != pDoc->BtnStatus.Eng.DcRSol)
	{
		pDoc->BtnStatus.Eng.PrevDcRSol = pDoc->BtnStatus.Eng.DcRSol;
		pDoc->SetMkMenu03(_T("Engraving"), _T("DancerUp"), pDoc->BtnStatus.Eng.DcRSol);
	}

	// Uncoiler
	pDoc->BtnStatus.Uc.Relation = m_pMpe->Read(_T("MB003601")) ? TRUE : FALSE;	// �����Ϸ� ���� ��/���� ����ġ ����
	pDoc->BtnStatus.Uc.FdCw = m_pMpe->Read(_T("MB00360C")) ? TRUE : FALSE;	// �����Ϸ� ��ǰ�� ��ȸ�� ����ġ ����
	pDoc->BtnStatus.Uc.FdCcw = m_pMpe->Read(_T("MB00360D")) ? TRUE : FALSE;	// �����Ϸ� ��ǰ�� ��ȸ�� ����ġ ����
	pDoc->BtnStatus.Uc.ReelChuck = m_pMpe->Read(_T("MB00360B")) ? TRUE : FALSE;	// �����Ϸ� ��ǰô Ŭ���� ����ġ ����
	pDoc->BtnStatus.Uc.DcRlUpDn = m_pMpe->Read(_T("MB003602")) ? TRUE : FALSE;	// �����Ϸ� ���� ���/�ϰ� ����ġ ����
	pDoc->BtnStatus.Uc.ReelJoinL = m_pMpe->Read(_T("MB003605")) ? TRUE : FALSE;	// �����Ϸ� ��ǰ ������(��/��) ����ġ ����
	pDoc->BtnStatus.Uc.ReelJoinR = m_pMpe->Read(_T("MB003606")) ? TRUE : FALSE;	// �����Ϸ� ��ǰ ������(��/��) ����ġ ����
	pDoc->BtnStatus.Uc.ReelJoinVac = m_pMpe->Read(_T("MB00360F")) ? TRUE : FALSE;	// �����Ϸ� ��ǰ ������ ���� ����ġ ����
	pDoc->BtnStatus.Uc.PprChuck = m_pMpe->Read(_T("MB003608")) ? TRUE : FALSE;	// �����Ϸ� ����ô Ŭ���� ����ġ ����
	pDoc->BtnStatus.Uc.PprCw = m_pMpe->Read(_T("MB003609")) ? TRUE : FALSE;	// �����Ϸ� ������ ��ȸ�� ����ġ ����
	pDoc->BtnStatus.Uc.PprCcw = m_pMpe->Read(_T("MB00360A")) ? TRUE : FALSE;	// �����Ϸ� ������ ��ȸ�� ����ġ ����
	pDoc->BtnStatus.Uc.ClRlUpDn = m_pMpe->Read(_T("MB003603")) ? TRUE : FALSE;	// �����Ϸ� Ŭ���ѷ� ���/�ϰ� ����ġ ����
	pDoc->BtnStatus.Uc.ClRlPshUpDn = m_pMpe->Read(_T("MB003604")) ? TRUE : FALSE;	// �����Ϸ� Ŭ���ѷ����� ���/�ϰ� ����ġ ����

	if (pDoc->BtnStatus.Uc.PrevRelation != pDoc->BtnStatus.Uc.Relation)
	{
		pDoc->BtnStatus.Uc.PrevRelation = pDoc->BtnStatus.Uc.Relation;
		pDoc->SetMkMenu03(_T("Main"), _T("Relation"), pDoc->BtnStatus.Uc.Relation);
	}
	if (pDoc->BtnStatus.Uc.PrevFdCw != pDoc->BtnStatus.Uc.FdCw)
	{
		pDoc->BtnStatus.Uc.PrevFdCw = pDoc->BtnStatus.Uc.FdCw;
		pDoc->SetMkMenu03(_T("Uncoiler"), _T("MvCw"), pDoc->BtnStatus.Uc.FdCw);
	}
	if (pDoc->BtnStatus.Uc.PrevFdCcw != pDoc->BtnStatus.Uc.FdCcw)
	{
		pDoc->BtnStatus.Uc.PrevFdCcw = pDoc->BtnStatus.Uc.FdCcw;
		pDoc->SetMkMenu03(_T("Uncoiler"), _T("MvCcw"), pDoc->BtnStatus.Uc.FdCcw);
	}
	if (pDoc->BtnStatus.Uc.PrevReelChuck != pDoc->BtnStatus.Uc.ReelChuck)
	{
		pDoc->BtnStatus.Uc.PrevReelChuck = pDoc->BtnStatus.Uc.ReelChuck;
		pDoc->SetMkMenu03(_T("Uncoiler"), _T("PrdChuck"), pDoc->BtnStatus.Uc.ReelChuck);
	}
	if (pDoc->BtnStatus.Uc.PrevDcRlUpDn != pDoc->BtnStatus.Uc.DcRlUpDn)
	{
		pDoc->BtnStatus.Uc.PrevDcRlUpDn = pDoc->BtnStatus.Uc.DcRlUpDn;
		pDoc->SetMkMenu03(_T("Uncoiler"), _T("DancerUp"), pDoc->BtnStatus.Uc.DcRlUpDn);
	}
	if (pDoc->BtnStatus.Uc.PrevReelJoinL != pDoc->BtnStatus.Uc.ReelJoinL)
	{
		pDoc->BtnStatus.Uc.PrevReelJoinL = pDoc->BtnStatus.Uc.ReelJoinL;
		pDoc->SetMkMenu03(_T("Uncoiler"), _T("PasteUpLf"), pDoc->BtnStatus.Uc.ReelJoinL);
	}
	if (pDoc->BtnStatus.Uc.PrevReelJoinR != pDoc->BtnStatus.Uc.ReelJoinR)
	{
		pDoc->BtnStatus.Uc.PrevReelJoinR = pDoc->BtnStatus.Uc.ReelJoinR;
		pDoc->SetMkMenu03(_T("Uncoiler"), _T("PasteUpRt"), pDoc->BtnStatus.Uc.ReelJoinR);
	}
	if (pDoc->BtnStatus.Uc.PrevReelJoinVac != pDoc->BtnStatus.Uc.ReelJoinVac)
	{
		pDoc->BtnStatus.Uc.PrevReelJoinVac = pDoc->BtnStatus.Uc.ReelJoinVac;
		pDoc->SetMkMenu03(_T("Uncoiler"), _T("PasteVac"), pDoc->BtnStatus.Uc.ReelJoinVac);
	}
	if (pDoc->BtnStatus.Uc.PrevPprChuck != pDoc->BtnStatus.Uc.PprChuck)
	{
		pDoc->BtnStatus.Uc.PrevPprChuck = pDoc->BtnStatus.Uc.PprChuck;
		pDoc->SetMkMenu03(_T("Uncoiler"), _T("PprChuck"), pDoc->BtnStatus.Uc.PprChuck);
	}
	if (pDoc->BtnStatus.Uc.PrevPprCw != pDoc->BtnStatus.Uc.PprCw)
	{
		pDoc->BtnStatus.Rc.PrevPprCw = pDoc->BtnStatus.Uc.PprCw;
		pDoc->SetMkMenu03(_T("Uncoiler"), _T("PprCw"), pDoc->BtnStatus.Uc.PprCw);
	}
	if (pDoc->BtnStatus.Uc.PrevPprCcw != pDoc->BtnStatus.Uc.PprCcw)
	{
		pDoc->BtnStatus.Uc.PrevPprCcw = pDoc->BtnStatus.Uc.PprCcw;
		pDoc->SetMkMenu03(_T("Uncoiler"), _T("PprCcw"), pDoc->BtnStatus.Uc.PprCcw);
	}
	if (pDoc->BtnStatus.Uc.PrevClRlUpDn != pDoc->BtnStatus.Uc.ClRlUpDn)
	{
		pDoc->BtnStatus.Uc.PrevClRlUpDn = pDoc->BtnStatus.Uc.ClRlUpDn;
		pDoc->SetMkMenu03(_T("Uncoiler"), _T("ClrRollUp"), pDoc->BtnStatus.Uc.ClRlUpDn);
	}
	if (pDoc->BtnStatus.Uc.PrevClRlPshUpDn != pDoc->BtnStatus.Uc.ClRlPshUpDn)
	{
		pDoc->BtnStatus.Uc.PrevClRlPshUpDn = pDoc->BtnStatus.Uc.ClRlPshUpDn;
		pDoc->SetMkMenu03(_T("Uncoiler"), _T("ClrRollPush"), pDoc->BtnStatus.Uc.ClRlPshUpDn);
	}

	//pDoc->BtnStatus.Uc.Relation = m_pMpe->Read(_T("MB005401")) ? TRUE : FALSE;
	//pDoc->BtnStatus.Uc.FdCw = m_pMpe->Read(_T("MB00540C")) ? TRUE : FALSE;
	//pDoc->BtnStatus.Uc.FdCcw = m_pMpe->Read(_T("MB00540D")) ? TRUE : FALSE;
	//pDoc->BtnStatus.Uc.ReelChuck = m_pMpe->Read(_T("MB00540B")) ? TRUE : FALSE;
	//pDoc->BtnStatus.Uc.DcRlUpDn = m_pMpe->Read(_T("MB005402")) ? TRUE : FALSE;
	//pDoc->BtnStatus.Uc.ReelJoinL = m_pMpe->Read(_T("MB005405")) ? TRUE : FALSE;
	//pDoc->BtnStatus.Uc.ReelJoinR = m_pMpe->Read(_T("MB005406")) ? TRUE : FALSE;
	//pDoc->BtnStatus.Uc.ReelJoinVac = m_pMpe->Read(_T("MB00540F")) ? TRUE : FALSE;
	//pDoc->BtnStatus.Uc.PprChuck = m_pMpe->Read(_T("MB005408")) ? TRUE : FALSE;
	//pDoc->BtnStatus.Uc.PprCw = m_pMpe->Read(_T("MB005409")) ? TRUE : FALSE;
	//pDoc->BtnStatus.Uc.PprCcw = m_pMpe->Read(_T("MB00540A")) ? TRUE : FALSE;
	//pDoc->BtnStatus.Uc.ClRlUpDn = m_pMpe->Read(_T("MB005403")) ? TRUE : FALSE;
	//pDoc->BtnStatus.Uc.ClRlPshUpDn = m_pMpe->Read(_T("MB005404")) ? TRUE : FALSE;

#else
	pDoc->BtnStatus.Main.Manual = TRUE;
#endif
}

void CGvisR2R_PunchView::InitIoWrite()
{
#ifdef USE_MPE
	if (!m_pMpe)
		return;

	//IoWrite(_T("MB44015E"), 0); // ����1 On  (PC�� ON, OFF) - 20141020
	m_pMpe->Write(_T("MB44015E"), 0);
	//IoWrite(_T("MB44015F"), 0); // ����2 On  (PC�� ON, OFF) - 20141020
	m_pMpe->Write(_T("MB44015F"), 0);

	//IoWrite(_T("MB003828"), 0); // �˻�� �� �˻� ���� <-> Y4368 I/F
	m_pMpe->Write(_T("MB003828"), 0);
	//IoWrite(_T("MB003829"), 0); // �˻�� �� �˻� ���̺� ���� SOL <-> Y4369 I/F
	m_pMpe->Write(_T("MB003829"), 0);
	//IoWrite(_T("MB00382A"), 0); // �˻�� �� Reset <-> Y436A I/F
	m_pMpe->Write(_T("MB00382A"), 0);
	//IoWrite(_T("MB00382B"), 0); // ��ŷ�� Lot End <-> Y436B I/F
	m_pMpe->Write(_T("MB00382B"), 0);

	//IoWrite(_T("MB003928"), 0); // �˻�� �� �˻� ���� <-> Y4468 I/F
	m_pMpe->Write(_T("MB003928"), 0);
	//IoWrite(_T("MB003929"), 0); // �˻�� �� �˻� ���̺� ���� SOL <-> Y4369 I/F
	m_pMpe->Write(_T("MB003929"), 0);
	//IoWrite(_T("MB00392A"), 0); // �˻�� �� Reset <-> Y436A I/F
	m_pMpe->Write(_T("MB00392A"), 0);
	//IoWrite(_T("MB00392B"), 0); // ��ŷ�� Lot End <-> Y436B I/F
	m_pMpe->Write(_T("MB00392B"), 0);

	//IoWrite(_T("MB44015D"), 0); // �ڵ� �ʱ� ��������(PC�� On/Off ��Ŵ, PLC�� ���������� ��ũ����, on->off�� �������� on, �ٽ� ��������ġ�� �������� off) - 20141017
	m_pMpe->Write(_T("MB44015D"), 0);
	//IoWrite(_T("ML45064"), 0); // �˻�� Feeding �ѷ� Offset(*1000, +:�� ����, -�� ����, PC�� ���� PLC���� ����)
	m_pMpe->Write(_T("ML45064"), 0);
	//IoWrite(_T("ML45066"), 0); // ��ŷ�� Feeding �ѷ� Offset(*1000, +:�� ����, -�� ����, PC�� ���� PLC���� ����)
	m_pMpe->Write(_T("ML45066"), 0);

	//IoWrite(_T("MB600000"), 0); // PC�� PLC�� Alarm �߻����θ� Ȯ��
	m_pMpe->Write(_T("MB600000"), 0);
	//IoWrite(_T("MB600008"), 0); // PC�� PLC�� Alarm �߻����θ� Ȯ��
	m_pMpe->Write(_T("MB600008"), 0);
	//IoWrite(_T("ML60002"), 0); // �˶��� �߻��� ������ ����(PLC�� ǥ�� �� �˶��� �������� ����Ŵ).
	m_pMpe->Write(_T("ML60002"), 0);
#endif
}


BOOL CGvisR2R_PunchView::MoveAlign0(int nPos)
{
	if (!m_pMotion)
		return FALSE;

	if (m_pDlgMenu02)
		m_pDlgMenu02->SetLight();

	if (m_pMotion->m_dPinPosY[0] > 0.0 && m_pMotion->m_dPinPosX[0] > 0.0)
	{
		double dCurrX = m_dEnc[AXIS_X0];
		double dCurrY = m_dEnc[AXIS_Y0];

		double pPos[2];
		if (nPos == 0)
		{
			pPos[0] = pDoc->m_Master[0].m_stAlignMk.X0 + m_pMotion->m_dPinPosX[0];
			pPos[1] = pDoc->m_Master[0].m_stAlignMk.Y0 + m_pMotion->m_dPinPosY[0];
		}
		else if (nPos == 1)
		{
			pPos[0] = pDoc->m_Master[0].m_stAlignMk.X1 + m_pMotion->m_dPinPosX[0];
			pPos[1] = pDoc->m_Master[0].m_stAlignMk.Y1 + m_pMotion->m_dPinPosY[0];
		}

		if (ChkCollision(AXIS_X0, pPos[0]))
			return FALSE;

		double fLen, fVel, fAcc, fJerk;
		fLen = sqrt(((pPos[0] - dCurrX) * (pPos[0] - dCurrX)) + ((pPos[1] - dCurrY) * (pPos[1] - dCurrY)));
		if (fLen > 0.001)
		{
			pView->m_pMotion->GetSpeedProfile(TRAPEZOIDAL, AXIS_X0, fLen, fVel, fAcc, fJerk);
			if (!pView->m_pMotion->Move(MS_X0Y0, pPos, fVel, fAcc, fAcc, ABS, NO_WAIT))
			{
				if (!pView->m_pMotion->Move(MS_X0Y0, pPos, fVel, fAcc, fAcc, ABS, NO_WAIT))
				{
					pView->ClrDispMsg();
					AfxMessageBox(_T("Error - Move MoveAlign0 ..."));
					return FALSE;
				}
			}
		}

		return TRUE;
	}

	return FALSE;
}

BOOL CGvisR2R_PunchView::MoveAlign1(int nPos)
{
	if (!m_pMotion)
		return FALSE;

	if (m_pDlgMenu02)
		m_pDlgMenu02->SetLight2();

	if (m_pMotion->m_dPinPosY[1] > 0.0 && m_pMotion->m_dPinPosX[1] > 0.0)
	{
		double dCurrX = m_dEnc[AXIS_X1];
		double dCurrY = m_dEnc[AXIS_Y1];

		double pPos[2];
		if (nPos == 0)
		{
			pPos[0] = pDoc->m_Master[0].m_stAlignMk.X0 + m_pMotion->m_dPinPosX[1];
			pPos[1] = pDoc->m_Master[0].m_stAlignMk.Y0 + m_pMotion->m_dPinPosY[1];
		}
		else if (nPos == 1)
		{
			pPos[0] = pDoc->m_Master[0].m_stAlignMk.X1 + m_pMotion->m_dPinPosX[1];
			pPos[1] = pDoc->m_Master[0].m_stAlignMk.Y1 + m_pMotion->m_dPinPosY[1];
		}

		if (ChkCollision(AXIS_X1, pPos[0]))
			return FALSE;

		double fLen, fVel, fAcc, fJerk;
		fLen = sqrt(((pPos[0] - dCurrX) * (pPos[0] - dCurrX)) + ((pPos[1] - dCurrY) * (pPos[1] - dCurrY)));
		if (fLen > 0.001)
		{
			m_pMotion->GetSpeedProfile(TRAPEZOIDAL, AXIS_X1, fLen, fVel, fAcc, fJerk);
			if (!m_pMotion->Move(MS_X1Y1, pPos, fVel, fAcc, fAcc, ABS, NO_WAIT))
			{
				if (!m_pMotion->Move(MS_X1Y1, pPos, fVel, fAcc, fAcc, ABS, NO_WAIT))
				{
					pView->ClrDispMsg();
					AfxMessageBox(_T("Error - Move MoveAlign1 ..."));
					return FALSE;
				}
			}
		}

		return TRUE;
	}

	return FALSE;
}

void CGvisR2R_PunchView::SetLastProc()
{

	if (m_pDlgMenu01)
		m_pDlgMenu01->SetLastProc();
}

BOOL CGvisR2R_PunchView::IsLastProc()
{
	BOOL bRtn = FALSE;
	if (m_pDlgMenu01)
		bRtn = m_pDlgMenu01->IsLastProc();
	else
		bRtn = FALSE;

	return bRtn;
}

BOOL CGvisR2R_PunchView::IsLastJob(int nAoi) // 0 : AOI-Up , 1 : AOI-Dn , 2 : AOI-UpDn
{
	switch (nAoi)
	{
	case 0: // AOI-Up
		if (pDoc->WorkingInfo.System.sPathCamSpecDir.IsEmpty() ||
			pDoc->WorkingInfo.LastJob.sModelUp.IsEmpty() ||
			pDoc->WorkingInfo.LastJob.sLayerUp.IsEmpty())
			return FALSE;
		break;
	case 1: // AOI-Dn
		if (pDoc->WorkingInfo.System.sPathCamSpecDir.IsEmpty() ||
			pDoc->WorkingInfo.LastJob.sModelUp.IsEmpty() ||
			//pDoc->WorkingInfo.LastJob.sModelDn.IsEmpty() ||
			pDoc->WorkingInfo.LastJob.sLayerDn.IsEmpty())
			return FALSE;
		break;
	case 2: // AOI-UpDn
		if (pDoc->WorkingInfo.System.sPathCamSpecDir.IsEmpty() ||
			pDoc->WorkingInfo.LastJob.sModelUp.IsEmpty() ||
			pDoc->WorkingInfo.LastJob.sLayerUp.IsEmpty())
			return FALSE;
		if (pDoc->WorkingInfo.System.sPathCamSpecDir.IsEmpty() ||
			pDoc->WorkingInfo.LastJob.sModelUp.IsEmpty() ||
			//pDoc->WorkingInfo.LastJob.sModelDn.IsEmpty() ||
			pDoc->WorkingInfo.LastJob.sLayerDn.IsEmpty())
			return FALSE;
		break;
	}

	return TRUE;
}

void CGvisR2R_PunchView::MonPlcSignal()
{
#ifdef USE_MPE
	if (pDoc->m_pMpeSignal[3] & (0x01 << 0))		// ���κ� 2D Leading �۾��Ϸ�(PLC�� ON/OFF) - MB440130
	{
		if (m_pDlgMenu03)
			m_pDlgMenu03->SetLed(0, TRUE);
	}
	else
	{
		if (m_pDlgMenu03)
			m_pDlgMenu03->SetLed(0, FALSE);
	}

	if (pDoc->m_pMpeSignal[3] & (0x01 << 2))		// ���κ� Laser �۾��Ϸ�(PLC�� ON/OFF) - MB440132
	{
		if (m_pDlgMenu03)
			m_pDlgMenu03->SetLed(1, TRUE);
	}
	else
	{
		if (m_pDlgMenu03)
			m_pDlgMenu03->SetLed(1, FALSE);
	}

	if (pDoc->m_pMpeSignal[3] & (0x01 << 3))		// �˻�� ��� �˻� �۾��Ϸ�(PLC�� ON/OFF) - MB440133
	{
		if (m_pDlgMenu03)
			m_pDlgMenu03->SetLed(2, TRUE);
	}
	else
	{
		if (m_pDlgMenu03)
			m_pDlgMenu03->SetLed(2, FALSE);
	}

	if (pDoc->m_pMpeSignal[3] & (0x01 << 4))		// �˻�� �ϸ� �˻� �۾��Ϸ�(PLC�� ON/OFF) - MB440134
	{
		if (m_pDlgMenu03)
			m_pDlgMenu03->SetLed(3, TRUE);
	}
	else
	{
		if (m_pDlgMenu03)
			m_pDlgMenu03->SetLed(3, FALSE);
	}

	if (pDoc->m_pMpeSignal[3] & (0x01 << 5))		// ��ŷ�� ��ŷ �۾��Ϸ�(PLC�� ON/OFF) - MB440135
	{
		if (m_pDlgMenu03)
			m_pDlgMenu03->SetLed(4, TRUE);
	}
	else
	{
		if (m_pDlgMenu03)
			m_pDlgMenu03->SetLed(4, FALSE);
	}

	if (pDoc->m_pMpeSignal[2] & (0x01 << 5))		// ���� ��ǰ�� �̾�� ���� ǥ�� - MB440125
	{
		DispContRun(TRUE);
	}
	else
	{
		DispContRun(FALSE);
	}

#endif
}


void CGvisR2R_PunchView::DispContRun(BOOL bOn)
{
	if (pDoc->WorkingInfo.LastJob.bDispContRun != bOn)
	{
		pDoc->WorkingInfo.LastJob.bDispContRun = bOn;
		pDoc->SetMkInfo(_T("Signal"), _T("DispContRun"), bOn);

#ifdef USE_ENGRAVE
		if (pView && pView->m_pEngrave)
			pView->m_pEngrave->SetDispContRun();	//_stSigInx::_DispContRun
#endif

	}
}

void CGvisR2R_PunchView::DispLotEnd(BOOL bOn)
{
	if (pDoc->WorkingInfo.LastJob.bDispLotEnd != bOn)
	{
		pDoc->WorkingInfo.LastJob.bDispLotEnd = bOn;
		pDoc->SetMkInfo(_T("Signal"), _T("DispLotEnd"), bOn);

#ifdef USE_ENGRAVE
		if (pView && pView->m_pEngrave)
			pView->m_pEngrave->SetDispLotEnd();	//_stSigInx::_DispLotEnd
#endif

	}
}

void CGvisR2R_PunchView::MonDispMain()
{
	BOOL bDispStop = TRUE;

#ifdef USE_MPE
	if (pDoc->m_pMpeSignal[2] & (0x01 << 0))		// ������(PLC�� PC�� �˷��ִ� ���� ����) - 20141031
	{
		if (m_sDispMain != _T("������"))
		{
			DispMain(_T("������"), RGB_GREEN);	
			pDoc->SetMkMenu03(_T("Main"), _T("Run"), TRUE);
			pDoc->SetMkMenu03(_T("Main"), _T("Stop"), FALSE);

			//if (m_pEngrave && m_pEngrave->IsConnected())
			//{
			//	m_pEngrave->SwRun(TRUE);
			//	Sleep(100);

			//	pDoc->BtnStatus.Disp.IsRun = FALSE;
			//	m_pEngrave->SetDispRun();
			//	Sleep(100);
			//}
		}
		else
		{
			//if (m_pEngrave && m_pEngrave->IsConnected())
			//{
			//	m_pEngrave->IsSetDispRun();
			//	if (!pDoc->BtnStatus.Disp.IsRun)
			//	{
			//		m_pEngrave->SetDispRun();
			//		Sleep(100);
			//	}
			//}
		}
	}

	if (pDoc->m_pMpeSignal[2] & (0x01 << 2))		// �����غ�(PLC�� PC�� �˷��ִ� ���� ����) - 20141031
	{
		bDispStop = FALSE;
		//if(!WatiDispMain(10))
		{
			if (m_sDispMain != _T("�����غ�"))
			{
				DispMain(_T("�����غ�"), RGB_GREEN);
				pDoc->SetMkMenu03(_T("Main"), _T("Ready"), TRUE);
				//if (m_pEngrave && m_pEngrave->IsConnected())
				//{
				//	m_pEngrave->SwReady(TRUE);
				//	Sleep(100);

				//	pDoc->BtnStatus.Disp.IsReady = FALSE;
				//	m_pEngrave->SetDispReady();
				//	Sleep(100);
				//}
			}
			else
			{
				//if (m_pEngrave && m_pEngrave->IsConnected())
				//{
				//	m_pEngrave->IsSetDispReady();
				//	if (!pDoc->BtnStatus.Disp.IsReady)
				//	{
				//		m_pEngrave->SetDispReady();
				//		Sleep(100);
				//	}
				//}
			}
		}
	}
	else
	{
		if (pDoc->m_pMpeSignal[2] & (0x01 << 3))		// �ʱ����(PLC�� PC�� �˷��ִ� ���� ����) - 20141031
		{
			bDispStop = FALSE;
			//if(!WatiDispMain(10))
			{
				if (pDoc->WorkingInfo.LastJob.bSampleTest)
				{
					if (pDoc->WorkingInfo.LastJob.bDualTest)
					{
						if (m_sDispMain != _T("������"))
						{
							DispMain(_T("������"), RGB_GREEN);
							//if (m_pEngrave && m_pEngrave->IsConnected())
							//{
							//	m_pEngrave->SetSampleTest();
							//	Sleep(100);
							//	m_pEngrave->SetDualTest();
							//	Sleep(100);

							//	pDoc->BtnStatus.Disp.IsDualSample = FALSE;
							//	m_pEngrave->SetDispDualSample();
							//	Sleep(100);
							//}
						}
						else
						{
							//if (m_pEngrave && m_pEngrave->IsConnected())
							//{
							//	m_pEngrave->IsSetDispDualSample();
							//	if (!pDoc->BtnStatus.Disp.IsDualSample)
							//	{
							//		m_pEngrave->SetDispDualSample();
							//		Sleep(100);
							//	}
							//}
						}
					}
					else
					{
						if (m_sDispMain != _T("�ܸ����"))
						{
							DispMain(_T("�ܸ����"), RGB_GREEN);
							//if (m_pEngrave && m_pEngrave->IsConnected())
							//{
							//	m_pEngrave->SetSampleTest();
							//	Sleep(100);
							//	m_pEngrave->SetDualTest();
							//	Sleep(100);

							//	pDoc->BtnStatus.Disp.IsSingleSample = FALSE;
							//	m_pEngrave->SetDispSingleSample();
							//	Sleep(100);
							//}
						}
						else
						{
							//if (m_pEngrave && m_pEngrave->IsConnected())
							//{
							//	m_pEngrave->IsSetDispSingleSample();
							//	if (!pDoc->BtnStatus.Disp.IsSingleSample)
							//	{
							//		m_pEngrave->SetDispSingleSample();
							//		Sleep(100);
							//	}
							//}
						}
					}
				}
				else if (pDoc->GetTestMode() == MODE_INNER)
				{
					if (m_sDispMain != _T("�����˻�"))
						DispMain(_T("�����˻�"), RGB_GREEN);
				}
				else if (pDoc->GetTestMode() == MODE_OUTER)
				{
					if (m_sDispMain != _T("�����˻�"))
						DispMain(_T("�����˻�"), RGB_GREEN);
				}
				else if (pDoc->WorkingInfo.LastJob.bDualTest)
				{
					if (m_sDispMain != _T("���˻�"))
					{
						DispMain(_T("���˻�"), RGB_GREEN);
						//if (m_pEngrave && m_pEngrave->IsConnected())
						//{
						//	m_pEngrave->SetSampleTest();
						//	Sleep(100);
						//	m_pEngrave->SetDualTest();
						//	Sleep(100);

						//	pDoc->BtnStatus.Disp.IsDualTest = FALSE;
						//	m_pEngrave->SetDispDualTest();
						//	Sleep(100);
						//}
					}
					else
					{
						//if (m_pEngrave && m_pEngrave->IsConnected())
						//{
						//	m_pEngrave->IsSetDispDualTest();
						//	if (!pDoc->BtnStatus.Disp.IsDualTest)
						//	{
						//		m_pEngrave->SetDispDualTest();
						//		Sleep(100);
						//	}
						//}
					}
				}
				else
				{
					if (m_sDispMain != _T("�ܸ�˻�"))
					{
						DispMain(_T("�ܸ�˻�"), RGB_GREEN);
						//if (m_pEngrave && m_pEngrave->IsConnected())
						//{
						//	m_pEngrave->SetSampleTest();
						//	Sleep(100);
						//	m_pEngrave->SetDualTest();
						//	Sleep(100);

						//	pDoc->BtnStatus.Disp.IsSingleTest = FALSE;
						//	m_pEngrave->SetDispSingleTest();
						//	Sleep(100);
						//}
					}
					else
					{
						//if (m_pEngrave && m_pEngrave->IsConnected())
						//{
						//	m_pEngrave->IsSetDispSingleTest();
						//	if (!pDoc->BtnStatus.Disp.IsSingleTest)
						//	{
						//		m_pEngrave->SetDispSingleTest();
						//		Sleep(100);
						//	}
						//}
					}
					//if(m_sDispMain != _T("�ʱ����")
					//DispMain(_T("�ʱ����", RGB_GREEN);
				}
			}
		}
		else
		{
			if (m_sDispMain != _T("�����غ�"))
			{
				bDispStop = TRUE;
				//pDoc->SetMkMenu03(_T("Main"), _T("Stop"), FALSE);
				//pDoc->SetMkMenu03(_T("Main"), _T("Run"), TRUE);
			}
			else
			{
				bDispStop = FALSE;
				//pDoc->SetMkMenu03(_T("Main"), _T("Stop"), FALSE);
			}
		}
	}


	if (pDoc->m_pMpeSignal[2] & (0x01 << 1))		// ����(PLC�� PC�� �˷��ִ� ���� ����) - 20141031
	{
		if (bDispStop)
		{
			if (m_sDispMain != _T("�� ��"))
			{
				//pView->DispStsBar(_T("����-44"), 0);
				DispMain(_T("�� ��"), RGB_RED);
				pDoc->SetMkMenu03(_T("Main"), _T("Stop"), TRUE);
				pDoc->SetMkMenu03(_T("Main"), _T("Run"), FALSE);
				//if (m_pEngrave && m_pEngrave->IsConnected())
				//{
				//	m_pEngrave->SwStop(TRUE);
				//	Sleep(100);

				//	pDoc->BtnStatus.Disp.IsStop = FALSE;
				//	m_pEngrave->SetDispStop();
				//	Sleep(100);
				//}
			}
			else
			{
				//if (m_pEngrave && m_pEngrave->IsConnected())
				//{
				//	m_pEngrave->IsSetDispStop();
				//	if (!pDoc->BtnStatus.Disp.IsStop)
				//	{
				//		m_pEngrave->SetDispStop();
				//		Sleep(100);
				//	}
				//}
			}
		}
	}
#endif
}

void CGvisR2R_PunchView::MonPlcAlm()
{
	BOOL bMon, bClr;
	long lOn = m_pMpe->Read(_T("ML60000"));

	bMon = lOn & (0x01 << 0);
	bClr = lOn & (0x01 << 1);
	if (bMon)
		PlcAlm(bMon, 0);
	else if (bClr)
		PlcAlm(0, bClr);
	else
		PlcAlm(bMon, bClr);

	if (!pDoc->m_sAlmMsg.IsEmpty())
	{
		if (pDoc->m_sAlmMsg != pDoc->m_sPrevAlmMsg)
		{
			pDoc->m_sPrevAlmMsg = pDoc->m_sAlmMsg;
			CycleStop();
		}
	}
}

void CGvisR2R_PunchView::PlcAlm(BOOL bMon, BOOL bClr)
{
	if (bMon && !m_nMonAlmF)
	{
		m_nMonAlmF = 1;
		//ResetMonAlm();
		FindAlarm();
		if (pView->m_pEngrave)
		{
			pDoc->m_sIsAlmMsg = _T("");
			pView->m_pEngrave->SetAlarm(pDoc->m_sAlmMsg);
		}

		Sleep(300);
		m_pMpe->Write(_T("MB600008"), 1);
	}
	else if (!bMon && m_nMonAlmF)
	{
		m_nMonAlmF = 0;
		ResetMonAlm();
	}
	else
	{
		if (pView->m_pEngrave)
		{
			if (pDoc->m_sIsAlmMsg != pDoc->m_sAlmMsg)
			{
				if(pView->m_pEngrave)
					pView->m_pEngrave->SetAlarm(pDoc->m_sAlmMsg);
			}
		}
	}


	if (bClr && !m_nClrAlmF)
	{
		m_nClrAlmF = 1;
		ClrAlarm();
		if (pView->m_pEngrave)
		{
			pDoc->m_sAlmMsg = _T("");
			pView->m_pEngrave->SetAlarm(pDoc->m_sAlmMsg);
			//Sleep(100);
			//pView->m_pEngrave->IsSetAlarm(_T(""));
		}
		Sleep(300);
		m_pMpe->Write(_T("MB600009"), 1);
		//		ResetClear();

	}
	else if (!bClr && m_nClrAlmF)
	{
		m_nClrAlmF = 0;
		ResetClear();
	}
	else
	{
		if (pView->m_pEngrave)
		{
			if (pDoc->m_sIsAlmMsg != pDoc->m_sAlmMsg)
			{
				if (pView->m_pEngrave)
					pView->m_pEngrave->SetAlarm(pDoc->m_sAlmMsg);
			}
		}
	}
}

void CGvisR2R_PunchView::FindAlarm()
{
	// �˶��� �߻��� ������
	long lAlmPage = -1;
	lAlmPage = m_pMpe->Read(_T("ML60002"));

	//char szData[200];
	TCHAR szData[200];
	CString str1, str2, str3, strM, str, strH = _T("");

	str1.Format(_T("%d"), lAlmPage);
	str2 = _T("Address");
	if (0 < ::GetPrivateProfileString(str1, str2, NULL, szData, sizeof(szData), PATH_ALARM))
		strM = CString(szData);
	else
		strM = _T("");

	if (strM.IsEmpty())
		return;

	long lAlm = m_pMpe->Read(strM); // lAlm : (32Bits << Row)
	for (int i = 0; i<32; i++)
	{
		if (lAlm & (0x01 << i))
		{
			str3.Format(_T("%d"), i);
			if (0 < ::GetPrivateProfileString(str1, str3, NULL, szData, sizeof(szData), PATH_ALARM))
				strH = CString(szData);
			else
				strH.Format(_T("%s = %d"), strM, lAlm);
				//strH = _T("");

			if (str.IsEmpty())
				str = strH;
			else
			{
				str += _T("\r\n");
				str += strH;
			}
		}
	}

	pDoc->m_sAlmMsg = strH;
}

void CGvisR2R_PunchView::ResetMonAlm()
{
	m_pMpe->Write(_T("MB600008"), 0);
}

void CGvisR2R_PunchView::ClrAlarm()
{
	if (!pDoc->m_sAlmMsg.IsEmpty())
	{
		pDoc->m_sAlmMsg = _T("");
		pDoc->m_sIsAlmMsg = _T("");
		pDoc->m_sPrevAlmMsg = _T("");
	}
}

void CGvisR2R_PunchView::ResetClear()
{
	m_pMpe->Write(_T("MB600009"), 0);
}

void CGvisR2R_PunchView::ChkTempStop(BOOL bChk)
{
	if (bChk)
	{
		if (!m_bTIM_CHK_TEMP_STOP)
		{
			m_bTIM_CHK_TEMP_STOP = TRUE;
			SetTimer(TIM_CHK_TEMP_STOP, 500, NULL);
		}
	}
	else
	{
		m_bTIM_CHK_TEMP_STOP = FALSE;
	}
}

void CGvisR2R_PunchView::ChgLot()
{
	pDoc->WorkingInfo.LastJob.sLotUp = pDoc->Status.PcrShare[0].sLot;
	pDoc->SetModelInfoUp();

	pDoc->WorkingInfo.LastJob.sLotDn = pDoc->Status.PcrShare[1].sLot;
	pDoc->SetModelInfoDn();

	//	pDoc->m_bDoneChgLot = TRUE;

	SetPathAtBuf();
}

BOOL CGvisR2R_PunchView::LoadPcrFromBuf() // ���α׷� ó�� ���۽� ���ۿ� ����� ������ ���ʿ� ���ʷ� ������Ʈ ��.
{
	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;
	CString str, sTemp;
	BOOL bRtn = FALSE;

	if (ChkBufUp(m_pBufSerial[0], m_nBufTot[0]))
	{
		for (int i = 0; i < m_nBufTot[0]; i++)
		{
			LoadPcrUp(m_pBufSerial[0][i]);
			if (!bDualTest)
			{
				UpdateReelmap(m_pBufSerial[0][i]);
				bRtn = TRUE;
			}
		}
	}

	if (bDualTest)
	{
		if (ChkBufDn(m_pBufSerial[1], m_nBufTot[1]))
		{
			for (int i = 0; i < m_nBufTot[1]; i++)
			{
				LoadPcrDn(m_pBufSerial[1][i]);
				UpdateReelmap(m_pBufSerial[1][i]); // After inspect bottom side.
				bRtn = TRUE;
			}
		}
	}

	return bRtn;
}

void CGvisR2R_PunchView::SetPathAtBuf()
{
	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;

	if (pDoc->m_pReelMapUp)
		pDoc->m_pReelMapUp->SetPathAtBuf();
	if (bDualTest)
	{
		if (pDoc->m_pReelMapDn)
			pDoc->m_pReelMapDn->SetPathAtBuf();
		if (pDoc->m_pReelMapAllUp)
			pDoc->m_pReelMapAllUp->SetPathAtBuf();
		if (pDoc->m_pReelMapAllDn)
			pDoc->m_pReelMapAllDn->SetPathAtBuf();
	}

	//if (pDoc->m_pReelMap)
	//{
	//	if (pDoc->m_pReelMap->m_nLayer < 0)
	//		pDoc->m_pReelMap->m_nLayer = pView->m_nSelRmap;

	//	pDoc->m_pReelMap->SetPathAtBuf();
	//}

	if (pDoc->GetTestMode() == MODE_OUTER)
	{
		//SetInnerPathAtBuf();
		if (pDoc->m_pReelMapIts)
			pDoc->m_pReelMapIts->SetPathAtBuf();
	}
}

void CGvisR2R_PunchView::SetPathAtBufUp()
{
	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;

	if (pDoc->m_pReelMapUp)
		pDoc->m_pReelMapUp->SetPathAtBuf();

	if (bDualTest)
	{
		if (pDoc->m_pReelMapAllUp)
			pDoc->m_pReelMapAllUp->SetPathAtBuf();
	}

	//if (pDoc->m_pReelMap)
	//{
	//	if (pDoc->m_pReelMap->m_nLayer < 0)
	//		pDoc->m_pReelMap->m_nLayer = pView->m_nSelRmap;

	//	if (pDoc->m_pReelMap->m_nLayer == RMAP_UP || pDoc->m_pReelMap->m_nLayer == RMAP_ALLUP)
	//		pDoc->m_pReelMap->SetPathAtBuf();
	//}

	if (pDoc->GetTestMode() == MODE_OUTER)
	{
		SetInnerPathAtBufUp();
	}
}

void CGvisR2R_PunchView::SetPathAtBufDn()
{
	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;
	if (!bDualTest)
		return;

	if (pDoc->m_pReelMapDn)
		pDoc->m_pReelMapDn->SetPathAtBuf();
	if (pDoc->m_pReelMapAllDn)
		pDoc->m_pReelMapAllDn->SetPathAtBuf();

	//if (pDoc->m_pReelMap)
	//{
	//	if (pDoc->m_pReelMap->m_nLayer < 0)
	//		pDoc->m_pReelMap->m_nLayer = pView->m_nSelRmap;

	//	if (pDoc->m_pReelMap->m_nLayer == RMAP_DN || pDoc->m_pReelMap->m_nLayer == RMAP_ALLDN)
	//		pDoc->m_pReelMap->SetPathAtBuf();
	//}

	if (pDoc->GetTestMode() == MODE_OUTER)
	{
		SetInnerPathAtBufDn();
	}
}


void CGvisR2R_PunchView::SetInnerPathAtBuf()
{
	//BOOL bDualTest = pDoc->WorkingInfo.LastJob.bInnerDualTest;

	//if (pDoc->m_pReelMapInnerUp)
	//	pDoc->m_pReelMapInnerUp->SetPathAtBuf(GetRmapPath(RMAP_INNER_UP));
	//if (bDualTest)
	//{
	//	if (pDoc->m_pReelMapInnerDn)
	//		pDoc->m_pReelMapInnerDn->SetPathAtBuf(GetRmapPath(RMAP_INNER_DN));
	//	if (pDoc->m_pReelMapInnerAllUp)
	//		pDoc->m_pReelMapInnerAllUp->SetPathAtBuf(GetRmapPath(RMAP_INNER_ALLUP));
	//	if (pDoc->m_pReelMapInnerAllDn)
	//		pDoc->m_pReelMapInnerAllDn->SetPathAtBuf(GetRmapPath(RMAP_INNER_ALLDN));
	//}

	//bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;

	//if (pDoc->m_pReelMapInOuterUp)
	//	pDoc->m_pReelMapInOuterUp->SetPathAtBuf(GetRmapPath(RMAP_INOUTER_UP));
	//if (bDualTest)
	//{
	//	if (pDoc->m_pReelMapInOuterDn)
	//		pDoc->m_pReelMapInOuterDn->SetPathAtBuf(GetRmapPath(RMAP_INOUTER_DN));
	//}

}

void CGvisR2R_PunchView::SetInnerPathAtBufUp()
{
	//BOOL bDualTest = pDoc->WorkingInfo.LastJob.bInnerDualTest;

	//if (pDoc->m_pReelMapInnerUp)
	//	pDoc->m_pReelMapInnerUp->SetPathAtBuf(GetRmapPath(RMAP_INNER_UP));

	//if (bDualTest)
	//{
	//	if (pDoc->m_pReelMapInnerAllUp)
	//		pDoc->m_pReelMapInnerAllUp->SetPathAtBuf(GetRmapPath(RMAP_INNER_ALLUP));
	//}

	//if (pDoc->m_pReelMapInOuterUp)
	//	pDoc->m_pReelMapInOuterUp->SetPathAtBuf(GetRmapPath(RMAP_INOUTER_UP));

}

void CGvisR2R_PunchView::SetInnerPathAtBufDn()
{
	//BOOL bDualTest = pDoc->WorkingInfo.LastJob.bInnerDualTest;

	//if (bDualTest)
	//{
	//	if (pDoc->m_pReelMapInnerDn)
	//		pDoc->m_pReelMapInnerDn->SetPathAtBuf(GetRmapPath(RMAP_INNER_DN));
	//	if (pDoc->m_pReelMapInnerAllDn)
	//		pDoc->m_pReelMapInnerAllDn->SetPathAtBuf(GetRmapPath(RMAP_INNER_ALLDN));
	//}

	//bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;

	//if (pDoc->m_pReelMapInOuterUp)
	//	pDoc->m_pReelMapInOuterUp->SetPathAtBuf(GetRmapPath(RMAP_INOUTER_UP));

	//if (bDualTest)
	//{
	//	if (pDoc->m_pReelMapInOuterDn)
	//		pDoc->m_pReelMapInOuterDn->SetPathAtBuf(GetRmapPath(RMAP_INOUTER_DN));
	//}

}



void  CGvisR2R_PunchView::SetLotLastShot()
{
	pDoc->m_nLotLastShot = int(_tstof(pDoc->WorkingInfo.LastJob.sLotSepLen)*1000.0 / _tstof(pDoc->WorkingInfo.LastJob.sOnePnlLen));
}

BOOL CGvisR2R_PunchView::IsMkStrip(int nStripIdx)
{
	if (!m_pDlgMenu01 || nStripIdx < 1 || nStripIdx > MAX_STRIP_NUM)
		return TRUE;

	return (m_pDlgMenu01->GetChkStrip(nStripIdx - 1));
}

void CGvisR2R_PunchView::CycleStop()
{
	m_bCycleStop = TRUE;
}

BOOL CGvisR2R_PunchView::ChkLotCutPos()
{
	if (pDoc->WorkingInfo.LastJob.bLotSep && pDoc->m_bDoneChgLot)
	{
		double dFdTotLen = GetMkFdLen();
		double dLotCutPos = _tstof(pDoc->WorkingInfo.LastJob.sLotCutPosLen)*1000.0;
		if (dFdTotLen >= dLotCutPos)
		{
			pDoc->WorkingInfo.LastJob.bLotSep = FALSE;
#ifdef USE_MPE
			pView->m_pMpe->Write(_T("MB440184"), 0);	// ��Ʈ�и����(PC�� On��Ű��, PC�� Ȯ���ϰ� Off��Ŵ)-20141031
			//pView->IoWrite(_T("MB440184"), 1);
#endif
			if (pDoc->m_pReelMap)
				pDoc->m_pReelMap->m_bUseLotSep = FALSE;

			::WritePrivateProfileString(_T("Last Job"), _T("Use Lot seperate"), _T("0"), PATH_WORKING_INFO);

			if (m_pDlgMenu01)
				m_pDlgMenu01->UpdateData();

			return TRUE;
		}
	}

	return FALSE;
}

BOOL CGvisR2R_PunchView::ChkStShotNum()
{
	CString sMsg;
	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;

	if (pDoc->m_ListBuf[0].nTot == 0)
	{
		if (m_nShareUpS > 0 && !(m_nShareUpS % 2))
		{
			sMsg.Format(_T("AOI ����� �ø����� ¦���� �����Ͽ����ϴ�.\r\n- �ø��� ��ȣ: %d"), m_nShareUpS);
			//MyMsgBox(sMsg);
			MsgBox(sMsg);
			return FALSE;
		}
	}

	if (bDualTest)
	{
		if (pDoc->m_ListBuf[1].nTot == 0)
		{
			if (m_nShareDnS > 0 && !(m_nShareDnS % 2))
			{
				sMsg.Format(_T("AOI �ϸ��� �ø����� ¦���� �����Ͽ����ϴ�.\r\n- �ø��� ��ȣ: %d"), m_nShareDnS);
				//MyMsgBox(sMsg);
				MsgBox(sMsg);
				return FALSE;
			}
		}
	}

	return TRUE;
}

BOOL CGvisR2R_PunchView::ChkContShotNum()
{
	CString sMsg;
	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;

	if (!m_pDlgFrameHigh)
		return FALSE;

	if (pDoc->m_ListBuf[0].nTot == 0)
	{
		if (m_nShareUpS > 0 && m_pDlgFrameHigh->m_nMkLastShot + 1 != m_nShareUpS)
		{
			sMsg.Format(_T("AOI ����� ����Shot(%d)�� ������Shot(%d)�� �ҿ����Դϴ�.\r\n��� �����Ͻðڽ��ϱ�?"), m_nShareUpS, m_pDlgFrameHigh->m_nMkLastShot);
			//if(IDNO==DoMyMsgBox(sMsg, MB_YESNO))
			if (IDNO == MsgBox(sMsg, 0, MB_YESNO))
				return FALSE;
		}
	}

	return TRUE;
}

void CGvisR2R_PunchView::SetFixPcs(int nSerial)
{
	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;

	if (pDoc->m_pReelMapUp)
		pDoc->m_pReelMapUp->SetFixPcs(nSerial);
	if (bDualTest)
	{
		if (pDoc->m_pReelMapDn)
			pDoc->m_pReelMapDn->SetFixPcs(nSerial);
	}
}

//void CGvisR2R_PunchView::MakeResultMDS()
//{
//	CString sPath, strMsg;
//	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;
//
//	pDoc->WorkingInfo.LastJob.sProcessNum = pDoc->GetProcessNum();
//	pDoc->UpdateProcessNum(pDoc->WorkingInfo.LastJob.sProcessNum);
//
//	if (m_pDlgMenu05)
//	{
//		m_pDlgMenu05->m_sModel = pDoc->WorkingInfo.LastJob.sModelUp;
//		m_pDlgMenu05->m_sLot = pDoc->WorkingInfo.LastJob.sLotUp;
//		m_pDlgMenu05->m_sLayer = pDoc->WorkingInfo.LastJob.sLayerUp;
//		m_pDlgMenu05->m_sProcessNum = pDoc->WorkingInfo.LastJob.sProcessNum;
//		sPath.Format(_T("%s%s\\%s\\%s"), pDoc->WorkingInfo.System.sPathOldFile,
//			pDoc->WorkingInfo.LastJob.sModelUp,
//			pDoc->WorkingInfo.LastJob.sLotUp,
//			pDoc->WorkingInfo.LastJob.sLayerUp);
//
//		if (bDualTest)
//			m_pDlgMenu05->m_sRmapPath.Format(_T("%s\\ReelMapDataAll.txt"), sPath);
//		else
//			m_pDlgMenu05->m_sRmapPath.Format(_T("%s\\ReelMapDataUp.txt"), sPath);
//
//		m_pDlgMenu05->GetResult();
//
//		MakeResult(); // Result.txt
//		MakeSapp3();
//	}
//	else
//	{
//		strMsg.Format(_T("It is trouble to open file.\r\n%s"), sPath);
//		pView->ClrDispMsg();
//		AfxMessageBox(strMsg, MB_ICONWARNING | MB_OK);
//	}
//
//	RemakeReelmap();
//	if (pDoc->GetTestMode() == MODE_OUTER)
//		RemakeReelmapInner();
//}

//void CGvisR2R_PunchView::MakeResult()
//{
//	// TODO: Add your control notification handler code here
//
//	// File Save......
//	CString strDestPath;
//	strDestPath.Format(_T("%s%s\\%s\\%s.txt"), pDoc->WorkingInfo.System.sPathOldFile,
//		pDoc->WorkingInfo.LastJob.sModelUp,
//		pDoc->WorkingInfo.LastJob.sLotUp,
//		_T("Result"));
//
//	CFileFind cFile;
//	if (cFile.FindFile(strDestPath))
//		DeleteFile(strDestPath);
//
//	//////////////////////////////////////////////////////////
//	// Directory location of Work file
//	CString strData;
//	strData = m_pDlgMenu05->TxtDataMDS();
//	//char lpszCurDirPathFile[MAX_PATH];
//	//strcpy(lpszCurDirPathFile, strDestPath);
//	TCHAR lpszCurDirPathFile[MAX_PATH];
//	_stprintf(lpszCurDirPathFile, _T("%s"), strDestPath);
//
//	CFile file;
//	CFileException pError;
//	if (!file.Open(lpszCurDirPathFile, CFile::modeWrite, &pError))
//	{
//		if (!file.Open(lpszCurDirPathFile, CFile::modeCreate | CFile::modeWrite, &pError))
//		{
//			// ���� ���¿� ���н� 
//#ifdef _DEBUG
//			afxDump << _T("File could not be opened ") << pError.m_cause << _T("\n");
//#endif
//			return;
//		}
//	}
//	//������ ������ file�� �����Ѵ�.
//	char* pRtn = NULL;
//	file.SeekToBegin();
//	file.Write(pRtn = StringToChar(strData), strData.GetLength());
//	file.Close();
//	if (pRtn)
//		delete pRtn;
//}

//void CGvisR2R_PunchView::MakeSapp3()
//{
//	if (pDoc->WorkingInfo.LastJob.sProcessNum.IsEmpty() || pDoc->WorkingInfo.System.sPathSapp3.IsEmpty())
//		return;
//
//	FILE *fp = NULL;
//	//char FileName[MAX_PATH];
//	TCHAR FileName[MAX_PATH];
//	CString sPath, sVal, strMsg;
//
//	sPath.Format(_T("%s%9s_%4s_%5s.txt"), pDoc->WorkingInfo.System.sPathSapp3,
//		pDoc->WorkingInfo.LastJob.sLotUp,
//		pDoc->WorkingInfo.LastJob.sProcessNum,
//		pDoc->WorkingInfo.System.sMcName);
//	//strcpy(FileName, sPath);
//	_stprintf(FileName, _T("%s"), sPath);
//	char* pRtn = NULL;
//	fp = fopen(pRtn=TCHARToChar(FileName), "w+");
//	if(pRtn) delete pRtn; 
//	pRtn = NULL;
//
//	if (fp != NULL)
//	{
//		fprintf( fp, "%s\n", pRtn = StringToChar(m_pDlgMenu05->Sapp3Data()) );
//	}
//	else
//	{
//		strMsg.Format(_T("It is trouble to open file.\r\n%s"), sPath);
//		pView->ClrDispMsg();
//		AfxMessageBox(strMsg, MB_ICONWARNING | MB_OK);
//	}
//
//	if(pRtn)
//		delete pRtn;
//	fclose(fp);
//}

//BOOL CGvisR2R_PunchView::RemakeReelmap()
//{
//	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;
//	CString sReelmapSrc, str;
//
//	str = _T("ReelMapDataUp.txt");
//	sReelmapSrc.Format(_T("%s%s\\%s\\%s\\%s"), pDoc->WorkingInfo.System.sPathOldFile,
//		pDoc->WorkingInfo.LastJob.sModelUp,
//		pDoc->WorkingInfo.LastJob.sLotUp,
//		pDoc->WorkingInfo.LastJob.sLayerUp,
//		str);
//	if (pDoc->m_pReelMapUp)
//		pDoc->m_pReelMapUp->RemakeReelmap(sReelmapSrc); // StartThreadRemakeReelmap()
//
//	if (bDualTest)
//	{
//		str = _T("ReelMapDataDn.txt");
//		sReelmapSrc.Format(_T("%s%s\\%s\\%s\\%s"), pDoc->WorkingInfo.System.sPathOldFile,
//			//pDoc->WorkingInfo.LastJob.sModelDn,
//			//pDoc->WorkingInfo.LastJob.sLotDn,
//			pDoc->WorkingInfo.LastJob.sModelUp,
//			pDoc->WorkingInfo.LastJob.sLotUp,
//			pDoc->WorkingInfo.LastJob.sLayerDn,
//			str);
//		if (pDoc->m_pReelMapDn)
//			pDoc->m_pReelMapDn->RemakeReelmap(sReelmapSrc); // StartThreadRemakeReelmap()
//
//		str = _T("ReelMapDataAll.txt");
//		sReelmapSrc.Format(_T("%s%s\\%s\\%s\\%s"), pDoc->WorkingInfo.System.sPathOldFile,
//			pDoc->WorkingInfo.LastJob.sModelUp,
//			pDoc->WorkingInfo.LastJob.sLotUp,
//			pDoc->WorkingInfo.LastJob.sLayerUp,
//			str);
//		if (pDoc->m_pReelMapAllUp)
//			pDoc->m_pReelMapAllUp->RemakeReelmap(sReelmapSrc); // StartThreadRemakeReelmap()
//	}
//
//	return TRUE;
//}
//
//BOOL CGvisR2R_PunchView::IsDoneRemakeReelmap()
//{
//	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;
//	BOOL bSuccess[3] = { FALSE };
//	DWORD dwSt = GetTickCount();
//
//	do
//	{
//		if (GetTickCount() - dwSt > 600000)
//			break;
//	} while (!pDoc->m_pReelMapUp->m_bThreadAliveRemakeReelmap && !pDoc->m_pReelMapDn->m_bThreadAliveRemakeReelmap && !pDoc->m_pReelMapDn->m_bThreadAliveRemakeReelmap);
//
//	if (bDualTest)
//	{
//		bSuccess[0] = pDoc->m_pReelMapUp->m_bRtnThreadRemakeReelmap;
//		bSuccess[1] = pDoc->m_pReelMapDn->m_bRtnThreadRemakeReelmap;
//		bSuccess[2] = pDoc->m_pReelMapAllUp->m_bRtnThreadRemakeReelmap;
//
//		if (!bSuccess[0] || !bSuccess[2] || !bSuccess[1])
//		{
//			MsgBox(_T("ReelMap Converting Failed."));
//			return FALSE;
//		}
//	}
//	else
//	{
//		if (!pDoc->m_pReelMapUp->m_bRtnThreadRemakeReelmap)
//		{
//			MsgBox(_T("ReelMap Converting Failed."));
//			return FALSE;
//		}
//	}
//
//	return TRUE;
//}

BOOL CGvisR2R_PunchView::DoElecChk(CString &sRst)
{
	BOOL bDone = FALSE;
#ifdef USE_FLUCK
	double dVal;
	if (!pDoc->WorkingInfo.Fluck.bUse)
		return TRUE;

	if (!pDoc->WorkingInfo.Probing[0].bUse)
		return TRUE;

	if (!m_pFluck || !m_pVoiceCoil[0] || !m_pVoiceCoil[1])
		return FALSE;

	switch (m_nStepElecChk)
	{
	case 0: // Move to left point for measuring.
		pDoc->m_sElecChk = _T("");
		m_dElecChkVal = 0.0;
		pDoc->WorkingInfo.Probing[0].nRepeatTest = 0;
		MoveMeasPos(0);
		m_nStepElecChk++;
		break;
	case 1: // Check move done
		if (m_pMotion->IsMotionDone(MS_X0))
			m_nStepElecChk++;
		break;
	case 2: // Move to right point for measuring.
		MoveMeasPos(1);
		m_nStepElecChk++;
		break;
	case 3: // Check move done
		if (m_pMotion->IsMotionDone(MS_X1))
			m_nStepElecChk++;
		break;
	case 4: // Delay
		m_nStepElecChk++;
		break;
	case 5: // left & right Prob down
		if (m_pVoiceCoil[0])
			m_pVoiceCoil[0]->SetProbing(0);
		m_nStepElecChk++;
		break;
	case 6: // Delay
		Sleep(100);
		m_nStepElecChk++;
		break;
	case 7: // left & right Prob down
		if (m_pVoiceCoil[1])
			m_pVoiceCoil[1]->SetProbing(1);
		m_nStepElecChk++;
		break;
	case 8: // Delay
		Sleep(30);
		m_nStepElecChk++;
		break;
	case 9: // Delay
		m_pFluck->Clear();
		Sleep(100);
		m_nStepElecChk++;
		break;
	case 10: // Delay
		m_nStepElecChk++;
		break;
	case 11: // Measure registance
		m_pFluck->Trig();
		m_nStepElecChk++;
		break;
	case 12: // Delay
		Sleep(100);
		if (m_pFluck->Get(dVal))
		{
			m_dElecChkVal += dVal;
			m_nStepElecChk++;
		}
		m_nStepElecChk++;
		break;
	case 13: // Measure registance
			 //m_pFluck->Clear();
		m_pFluck->Trig();
		m_nStepElecChk++;
		break;
	case 14: // Delay

		Sleep(100);
		if (m_pFluck->Get(dVal))
		{
			m_dElecChkVal += dVal;
			m_nStepElecChk++;
		}
		break;
	case 15: // Measure registance
			 //m_pFluck->Clear();
		m_pFluck->Trig();
		m_nStepElecChk++;
		break;
	case 16: // Delay
		Sleep(100);
		if (m_pFluck->Get(dVal))
		{
			m_dElecChkVal += dVal;
			m_nStepElecChk++;
		}
		break;
	case 17: // Result OK , NG
		dVal = m_dElecChkVal / 3.0;

		if (dVal >= FLUCK_OVER)
		{
			sRst.Format(_T("Open"), dVal);
			if (pDoc->WorkingInfo.Probing[0].nRepeatTest < _tstoi(pDoc->WorkingInfo.Probing[0].sRepeatTestNum))
			{
				pDoc->WorkingInfo.Probing[0].nRepeatTest++;
				// 				m_pVoiceCoil[0]->SearchHomeSmac0();
				// 				Sleep(100);
				// 				m_pVoiceCoil[1]->SearchHomeSmac1();
				Sleep(100);
				m_nStepElecChk = 4;
				break;
			}
		}
		else if (dVal == FLUCK_ERROR)
		{
			sRst.Format(_T("Error"), dVal);
			if (pDoc->WorkingInfo.Probing[0].nRepeatTest < _tstoi(pDoc->WorkingInfo.Probing[0].sRepeatTestNum))
			{
				pDoc->WorkingInfo.Probing[0].nRepeatTest++;
				// 				m_pVoiceCoil[0]->SearchHomeSmac0();
				// 				Sleep(100);
				// 				m_pVoiceCoil[1]->SearchHomeSmac1();
				Sleep(100);
				m_nStepElecChk = 4;
				break;
			}
		}
		else
		{
			sRst.Format(_T("%8f"), dVal);
		}

		pDoc->m_sElecChk = sRst;
		m_nStepElecChk++;
		break;
	case 18: // Write Data
		pDoc->WriteElecData(pDoc->m_sElecChk);
		m_nStepElecChk++;
		break;
	case 19: // left & right Prob up
			 //		m_pVoiceCoil[0]->MoveSmacShiftPos0();
		m_pVoiceCoil[1]->SearchHomeSmac1();
		m_nStepElecChk++;
		break;
	case 20: // Delay
		m_nStepElecChk++;
		break;
	case 21: // left & right Prob up
			 //		m_pVoiceCoil[1]->MoveSmacShiftPos1();
		m_pVoiceCoil[0]->SearchHomeSmac0();
		m_nStepElecChk++;
		break;
	case 22: // Delay
		m_nStepElecChk++;
		break;
	case 23: // Move to left init pos
			 //		if(m_pVoiceCoil[0]->IsDoneMoveSmacShiftPos0() && m_pVoiceCoil[1]->IsDoneMoveSmacShiftPos1())
		if (m_pVoiceCoil[0]->IsDoneSearchHomeSmac0() && m_pVoiceCoil[1]->IsDoneSearchHomeSmac1())
		{
			if (!IsInitPos0())
				MoveInitPos0();
			if (!IsInitPos1())
				MoveInitPos1();
			m_nStepElecChk++;
		}
		break;
	case 24: // Move to right init pos
		if (IsMoveDone0() && IsMoveDone1())
		{
			//Sleep(500);
			m_nStepElecChk++;
		}
		break;
	case 25: // Move to left init pos
		m_pVoiceCoil[1]->MoveSmacShiftPos1();
		m_nStepElecChk++;
		break;
	case 26: // Delay
		m_nStepElecChk++;
		break;
	case 27: // Move to left init pos
		m_pVoiceCoil[0]->MoveSmacShiftPos0();
		m_nStepElecChk++;
		break;
	case 28: // Delay
		m_nStepElecChk++;
		break;
	case 29: // Move to left init pos
		if (m_pVoiceCoil[0]->IsDoneMoveSmacShiftPos0() && m_pVoiceCoil[1]->IsDoneMoveSmacShiftPos1())
		{
			m_nStepElecChk++;
		}
		break;
	case 30: // Delay
			 //Sleep(100);
		m_nStepElecChk++;
		break;
	case 31: // Done Elec Check.
		bDone = TRUE;
		break;
	}

	sRst = pDoc->m_sElecChk;
#else
	bDone = TRUE;
#endif
	return bDone;
}

BOOL CGvisR2R_PunchView::MoveMeasPos(int nId)
{
	if (!m_pMotion)
		return FALSE;

	if (!m_pLight)
		return FALSE;

	if (nId == 0)
	{
		if (m_pDlgMenu02)
			m_pDlgMenu02->SetLight();

		double dMkOffsetX, dMkOffsetY;
		if (pDoc->WorkingInfo.Vision[0].sMkOffsetX.IsEmpty())
			dMkOffsetX = 0.0;
		else
			dMkOffsetX = _tstof(pDoc->WorkingInfo.Vision[0].sMkOffsetX);

		if (pDoc->WorkingInfo.Vision[0].sMkOffsetY.IsEmpty())
			dMkOffsetY = 0.0;
		else
			dMkOffsetY = _tstof(pDoc->WorkingInfo.Vision[0].sMkOffsetY);


		double pPos[2];
		if (m_bFailAlign[0][0] || m_bFailAlign[0][1])
		{
			pPos[0] = _tstof(pDoc->WorkingInfo.Probing[nId].sMeasurePosX) + dMkOffsetX;
			pPos[1] = _tstof(pDoc->WorkingInfo.Probing[nId].sMeasurePosY) + dMkOffsetY;
		}
		else
		{
			pPos[0] = _tstof(pDoc->WorkingInfo.Probing[nId].sMeasurePosX) + dMkOffsetX - m_pDlgMenu02->m_dMkFdOffsetX[0][0];
			pPos[1] = _tstof(pDoc->WorkingInfo.Probing[nId].sMeasurePosY) + dMkOffsetY - m_pDlgMenu02->m_dMkFdOffsetY[0][0];

			// 			pPos[0] = pDoc->WorkingInfo.Fluck.dMeasPosX[0] + dMkOffsetX;
			// 			pPos[1] = pDoc->WorkingInfo.Fluck.dMeasPosY[0] + dMkOffsetY;
		}

		if (pPos[1] > 0.0 && pPos[0] > 0.0)
		{
			double dCurrX = m_dEnc[AXIS_X0];
			double dCurrY = m_dEnc[AXIS_Y0];

			if (ChkCollision(AXIS_X0, pPos[0]))
			{
				CfPoint ptPnt;
				ptPnt.x = _tstof(pDoc->WorkingInfo.Motion.sSafeZone);
				ptPnt.y = 0.0; // m_dEnc[AXIS_Y1];
				Move1(ptPnt);
			}

			double fLen, fVel, fAcc, fJerk;
			fLen = sqrt(((pPos[0] - dCurrX) * (pPos[0] - dCurrX)) + ((pPos[1] - dCurrY) * (pPos[1] - dCurrY)));
			if (fLen > 0.001)
			{
				m_pMotion->GetSpeedProfile(TRAPEZOIDAL, AXIS_X0, fLen, fVel, fAcc, fJerk);
				if (!m_pMotion->Move(MS_X0Y0, pPos, fVel, fAcc, fAcc))
				{
					if (!m_pMotion->Move(MS_X0Y0, pPos, fVel, fAcc, fAcc))
					{
						pView->ClrDispMsg();
						AfxMessageBox(_T("Move XY Error..."));
					}
				}
			}

			return TRUE;
		}
	}
	else if (nId == 1)
	{
		if (m_pDlgMenu02)
			m_pDlgMenu02->SetLight2();

		double dMkOffsetX, dMkOffsetY;
		if (pDoc->WorkingInfo.Vision[1].sMkOffsetX.IsEmpty())
			dMkOffsetX = 0.0;
		else
			dMkOffsetX = _tstof(pDoc->WorkingInfo.Vision[1].sMkOffsetX);

		if (pDoc->WorkingInfo.Vision[1].sMkOffsetY.IsEmpty())
			dMkOffsetY = 0.0;
		else
			dMkOffsetY = _tstof(pDoc->WorkingInfo.Vision[1].sMkOffsetY);


		double pPos[2];
		if (m_bFailAlign[1][0] || m_bFailAlign[1][1])
		{
			pPos[0] = _tstof(pDoc->WorkingInfo.Probing[nId].sMeasurePosX) + dMkOffsetX;
			pPos[1] = _tstof(pDoc->WorkingInfo.Probing[nId].sMeasurePosY) + dMkOffsetY;
		}
		else
		{
			pPos[0] = _tstof(pDoc->WorkingInfo.Probing[nId].sMeasurePosX) + dMkOffsetX - m_pDlgMenu02->m_dMkFdOffsetX[1][0];
			pPos[1] = _tstof(pDoc->WorkingInfo.Probing[nId].sMeasurePosY) + dMkOffsetY - m_pDlgMenu02->m_dMkFdOffsetY[1][0];

			// 			pPos[0] = pDoc->WorkingInfo.Fluck.dMeasPosX[1] + dMkOffsetX;
			// 			pPos[1] = pDoc->WorkingInfo.Fluck.dMeasPosY[1] + dMkOffsetY;
		}

		if (pPos[1] > 0.0 && pPos[0] > 0.0)
		{
			double dCurrX = m_dEnc[AXIS_X1];
			double dCurrY = m_dEnc[AXIS_Y1];

			if (ChkCollision(AXIS_X1, pPos[0]))
			{
				CfPoint ptPnt;
				ptPnt.x = 0.0;
				ptPnt.y = 0.0;//m_dEnc[AXIS_Y0];
				Move0(ptPnt);
			}

			double fLen, fVel, fAcc, fJerk;
			fLen = sqrt(((pPos[0] - dCurrX) * (pPos[0] - dCurrX)) + ((pPos[1] - dCurrY) * (pPos[1] - dCurrY)));
			if (fLen > 0.001)
			{
				m_pMotion->GetSpeedProfile(TRAPEZOIDAL, AXIS_X0, fLen, fVel, fAcc, fJerk);
				if (!m_pMotion->Move(MS_X1Y1, pPos, fVel, fAcc, fAcc))
				{
					if (!m_pMotion->Move(MS_X1Y1, pPos, fVel, fAcc, fAcc))
					{
						pView->ClrDispMsg();
						AfxMessageBox(_T("Move XY Error..."));
					}
				}
			}

			return TRUE;
		}
	}

	return FALSE;
}

void CGvisR2R_PunchView::SetReject()
{
	CfPoint ptPnt;
	//int nSerial, nTot;

	if (m_bDoMk[0])
	{
		if (!m_bTHREAD_MK[2])
		{
			m_bDoneMk[0] = FALSE;
			m_nStepMk[2] = 0;
			m_nMkPcs[2] = 0;
			m_bTHREAD_MK[2] = TRUE;
		}
	}

	if (m_bDoMk[1])
	{
		if (!m_bTHREAD_MK[3])
		{
			m_bDoneMk[1] = FALSE;
			m_nStepMk[3] = 0;
			m_nMkPcs[3] = 0;
			m_bTHREAD_MK[3] = TRUE;
		}
	}
}

void CGvisR2R_PunchView::DoInterlock()
{
	if (m_dEnc[AXIS_Y0] < 20.0 && m_dEnc[AXIS_Y1] < 20.0)
	{
		if (m_bStopFeeding)
		{
			m_bStopFeeding = FALSE;
			m_pMpe->Write(_T("MB440115"), 0); // ��ŷ��Feeding����
		}
	}
	else
	{
		if (!m_bStopFeeding)
		{
			m_bStopFeeding = TRUE;
			m_pMpe->Write(_T("MB440115"), 1); // ��ŷ��Feeding����
		}
	}
}

BOOL CGvisR2R_PunchView::ChkLightErr()
{
	int nSerial, nErrCode;
	BOOL bError = FALSE;
	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;

	nSerial = m_nBufUpSerial[0]; // Cam0

	if (pView->m_bSerialDecrese)
	{
		if (nSerial > 0 && nSerial >= m_nLotEndSerial)
		{
			if ((nErrCode = GetErrCode0(nSerial)) != 1)
			{
				if (nErrCode == -1) // -1(Align Error, �뱤�ҷ�)
					bError = TRUE;
			}
		}

		nSerial = m_nBufUpSerial[1]; // Cam1

		if (nSerial > 0 && nSerial >= m_nLotEndSerial)
		{
			if ((nErrCode = GetErrCode1(nSerial)) != 1)
			{
				if (nErrCode == -1) // -1(Align Error, �뱤�ҷ�)
					bError = TRUE;
			}
		}
	}
	else
	{
		if (nSerial > 0 && nSerial <= m_nLotEndSerial)
		{
			if ((nErrCode = GetErrCode0(nSerial)) != 1)
			{
				if (nErrCode == -1) // -1(Align Error, �뱤�ҷ�)
					bError = TRUE;
			}
		}

		nSerial = m_nBufUpSerial[1]; // Cam1

		if (nSerial > 0 && nSerial <= m_nLotEndSerial)
		{
			if ((nErrCode = GetErrCode1(nSerial)) != 1)
			{
				if (nErrCode == -1) // -1(Align Error, �뱤�ҷ�)
					bError = TRUE;
			}
		}
	}

	if (bError)
	{
		Buzzer(TRUE, 0);
		TowerLamp(RGB_RED, TRUE);
		Stop();
		DispMain(_T("�� ��"), RGB_RED);
	}

	return bError;
}

void CGvisR2R_PunchView::CntMk()
{
#ifdef USE_MPE
	if (m_nPrevTotMk[0] != m_nTotMk[0])
	{
		m_nPrevTotMk[0] = m_nTotMk[0];
		pView->m_pMpe->Write(_T("ML45096"), (long)m_nTotMk[0]);	// ��ŷ�� (��) �� ��ŷ�� 
	}
	if (m_nPrevCurMk[0] != m_nMkPcs[0])//m_nCurMk[0])
	{
		m_nPrevCurMk[0] = m_nMkPcs[0];//m_nCurMk[0];
		pView->m_pMpe->Write(_T("ML45098"), (long)m_nMkPcs[0]);	// ��ŷ�� (��) ���� ��ŷ�� ��
	}

	if (m_nPrevTotMk[1] != m_nTotMk[1])
	{
		m_nPrevTotMk[1] = m_nTotMk[1];
		pView->m_pMpe->Write(_T("ML45100"), (long)m_nTotMk[1]);	// ��ŷ�� (��) �� ��ŷ�� 
	}
	if (m_nPrevCurMk[1] != m_nMkPcs[1])//m_nCurMk[1])
	{
		m_nPrevCurMk[1] = m_nMkPcs[1];//m_nCurMk[1];
		pView->m_pMpe->Write(_T("ML45102"), (long)m_nMkPcs[1]);	// ��ŷ�� (��) ���� ��ŷ�� ��
	}
#endif
}

BOOL CGvisR2R_PunchView::IsOnMarking0()
{
	if (m_nMkPcs[0] < pDoc->m_Master[0].m_pPcsRgn->nTotPcs)	// ��ŷ�Ϸ�Check
		return TRUE;

	return FALSE;
}

BOOL CGvisR2R_PunchView::IsOnMarking1()
{
	if (m_nMkPcs[1] < pDoc->m_Master[0].m_pPcsRgn->nTotPcs)	// ��ŷ�Ϸ�Check
		return TRUE;

	return FALSE;
}

void CGvisR2R_PunchView::SetDualTest(BOOL bOn)
{
#ifdef USE_MPE
	if (pView->m_pMpe)
		pView->m_pMpe->Write(_T("MB44017A"), bOn ? 0 : 1);		// �ܸ� �˻� On
#endif
	if (m_pDlgFrameHigh)
		m_pDlgFrameHigh->SetDualTest(bOn);
	if (m_pDlgMenu01)
		m_pDlgMenu01->SetDualTest(bOn);
	if (m_pDlgMenu03)
		m_pDlgMenu03->SetDualTest(bOn);
	if (m_pDlgMenu06)
		m_pDlgMenu06->SetDualTest(bOn);
}

void CGvisR2R_PunchView::SetTwoMetal(BOOL bSel, BOOL bOn)
{
	if (bSel)
	{
		if (bOn)
		{
			pDoc->WorkingInfo.LastJob.bTwoMetal = TRUE;
			m_pMpe->Write(_T("MB44017C"), 1);
			::WritePrivateProfileString(_T("Last Job"), _T("Two Metal On"), _T("1"), PATH_WORKING_INFO);// IDC_CHK_TWO_METAL - Uncoiler\r������ ON : TRUE	
		}
		else
		{
			pDoc->WorkingInfo.LastJob.bTwoMetal = FALSE;
			m_pMpe->Write(_T("MB44017C"), 0);
			::WritePrivateProfileString(_T("Last Job"), _T("Two Metal On"), _T("0"), PATH_WORKING_INFO);// IDC_CHK_TWO_METAL - Uncoiler\r������ ON : TRUE	
		}

//#ifdef USE_ENGRAVE
//		if (pView && pView->m_pEngrave)
//			pView->m_pEngrave->SetUncoilerCcw();	//_stSigInx::_UncoilerCcw
//#endif
	}
	else
	{
		if (bOn)
		{
			pDoc->WorkingInfo.LastJob.bOneMetal = TRUE;
			m_pMpe->Write(_T("MB44017D"), 1);
			::WritePrivateProfileString(_T("Last Job"), _T("One Metal On"), _T("1"), PATH_WORKING_INFO);// IDC_CHK_ONE_METAL - Recoiler\r������ CW : FALSE
		}
		else
		{
			pDoc->WorkingInfo.LastJob.bOneMetal = FALSE;
			m_pMpe->Write(_T("MB44017D"), 0);
			::WritePrivateProfileString(_T("Last Job"), _T("One Metal On"), _T("0"), PATH_WORKING_INFO);// IDC_CHK_ONE_METAL - Recoiler\r������ CW : FALSE
		}

//#ifdef USE_ENGRAVE
//		if (pView && pView->m_pEngrave)
//			pView->m_pEngrave->SetRecoilerCcw();	//_stSigInx::_RecoilerCcw
//#endif
	}
}

void CGvisR2R_PunchView::RestoreReelmap()
{
	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;

	if (pDoc->m_pReelMapUp)
		pDoc->m_pReelMapUp->RestoreReelmap();

	if (bDualTest)
	{
		if (pDoc->m_pReelMapDn)
			pDoc->m_pReelMapDn->RestoreReelmap();
		if (pDoc->m_pReelMapAllUp)
			pDoc->m_pReelMapAllUp->RestoreReelmap();
		if (pDoc->m_pReelMapAllDn)
			pDoc->m_pReelMapAllDn->RestoreReelmap();
	}
}

void CGvisR2R_PunchView::AdjPinPos()
{
	if (m_pDlgMenu02)
	{
		if (m_pDlgMenu02->m_dMkFdOffsetY[0][0] > -2.0 && m_pDlgMenu02->m_dMkFdOffsetY[1][0] > -2.0 &&
			m_pDlgMenu02->m_dMkFdOffsetY[0][0] < 2.0 && m_pDlgMenu02->m_dMkFdOffsetY[1][0] < 2.0)
		{
			double dOffsetY0 = -1.0 * m_pDlgMenu02->m_dMkFdOffsetY[0][0];
			dOffsetY0 *= pDoc->m_dShiftAdjustRatio;
			double dOffsetY1 = -1.0 * m_pDlgMenu02->m_dMkFdOffsetY[1][0];
			dOffsetY1 *= pDoc->m_dShiftAdjustRatio;
			//double dOffsetY = -1.0*(m_pDlgMenu02->m_dMkFdOffsetY[0][0] + m_pDlgMenu02->m_dMkFdOffsetY[1][0]) / 2.0;
			//dOffsetY *= pDoc->m_dShiftAdjustRatio;

			CfPoint ptPnt[2];
			ptPnt[0].x = _tstof(pDoc->WorkingInfo.Motion.sPinPosX[0]);
			ptPnt[0].y = _tstof(pDoc->WorkingInfo.Motion.sPinPosY[0]) + dOffsetY0;
			//ptPnt[0].y = _tstof(pDoc->WorkingInfo.Motion.sPinPosY[0]) + dOffsetY;
			ptPnt[1].x = _tstof(pDoc->WorkingInfo.Motion.sPinPosX[1]);
			ptPnt[1].y = _tstof(pDoc->WorkingInfo.Motion.sPinPosY[1]) + dOffsetY1;
			//ptPnt[1].y = _tstof(pDoc->WorkingInfo.Motion.sPinPosY[1]) + dOffsetY;

			m_pDlgMenu02->SetPinPos(0, ptPnt[0]);
			m_pDlgMenu02->SetPinPos(1, ptPnt[1]);

			CString sData, sPath = PATH_WORKING_INFO;
			pDoc->WorkingInfo.Fluck.dMeasPosY[0] = _tstof(pDoc->WorkingInfo.Probing[0].sMeasurePosY) + dOffsetY0;
			//pDoc->WorkingInfo.Fluck.dMeasPosY[0] = _tstof(pDoc->WorkingInfo.Probing[0].sMeasurePosY) + dOffsetY;
			sData.Format(_T("%.2f"), pDoc->WorkingInfo.Fluck.dMeasPosY[0]);
			pDoc->WorkingInfo.Probing[0].sMeasurePosY = sData;
			::WritePrivateProfileString(_T("Probing0"), _T("PROBING_MEASURE_POSY"), sData, sPath);

			pDoc->WorkingInfo.Fluck.dMeasPosY[1] = _tstof(pDoc->WorkingInfo.Probing[1].sMeasurePosY) + dOffsetY1;
			//pDoc->WorkingInfo.Fluck.dMeasPosY[1] = _tstof(pDoc->WorkingInfo.Probing[1].sMeasurePosY) + dOffsetY;
			sData.Format(_T("%.2f"), pDoc->WorkingInfo.Fluck.dMeasPosY[1]);
			pDoc->WorkingInfo.Probing[1].sMeasurePosY = sData;
			::WritePrivateProfileString(_T("Probing1"), _T("PROBING_MEASURE_POSY"), sData, sPath);
		}
	}
}


// Engrave
void CGvisR2R_PunchView::SetEngraveSts(int nStep)
{
	if (!m_pDlgMenu03)
		return;

	// ��ŷ�� - TBL�ı� OFF, TBL���� ON, FD/TQ ���� OFF, 
	switch (nStep)
	{
	case 0:
		m_pDlgMenu03->SwMkTblBlw(FALSE);
		m_pDlgMenu03->SwMkFdVac(FALSE);
		m_pDlgMenu03->SwMkTqVac(FALSE);
		break;
	case 1:
		m_pDlgMenu03->SwMkTblVac(TRUE);
		break;
	}
}

void CGvisR2R_PunchView::SetEngraveStopSts()
{
	if (!m_pDlgMenu03)
		return;

	// ��ŷ�� - FD/TQ ���� ON, TBL���� OFF, TBL�ı� ON, 
	m_pDlgMenu03->SwMkTblBlw(FALSE);
	m_pDlgMenu03->SwMkTblVac(FALSE);
	m_pDlgMenu03->SwMkFdVac(FALSE);
	// 	m_pDlgMenu03->SwMkTqVac(FALSE);
}

void CGvisR2R_PunchView::SetEngraveFdSts()
{
	if (!m_pDlgMenu03)
		return;

	// ��ŷ�� - FD/TQ ���� ON, TBL���� OFF, TBL�ı� ON, 
	m_pDlgMenu03->SwMkFdVac(TRUE);
	// 	m_pDlgMenu03->SwMkTqVac(TRUE);
	m_pDlgMenu03->SwMkTblVac(FALSE);
	m_pDlgMenu03->SwMkTblBlw(TRUE);
}

BOOL CGvisR2R_PunchView::IsEngraveFdSts()
{
	if (!m_pDlgMenu03)
		return FALSE;

	BOOL bOn[4] = { 0 };
	// ��ŷ�� - FD/TQ ���� ON, TBL���� OFF, TBL�ı� ON, 
	bOn[0] = m_pDlgMenu03->IsMkFdVac(); // TRUE
	bOn[1] = m_pDlgMenu03->IsMkTqVac(); // TRUE
	bOn[2] = m_pDlgMenu03->IsMkTblVac(); // FALSE
	bOn[3] = m_pDlgMenu03->IsMkTblBlw(); // TRUE

	if (bOn[0] && bOn[1] && !bOn[2] && bOn[3])
		return TRUE;

	return FALSE;
}


void CGvisR2R_PunchView::SetEngraveFd()
{
	/*
	CfPoint OfSt;
	GetMkOffset(OfSt);

	if (m_nShareDnCnt > 0)
	{
		if (!(m_nShareDnCnt % 2))
		MoveMk(-1.0*OfSt.x);
	}
	else
	{
		if (m_nShareUpCnt > 0)
		{
			if (!(m_nShareUpCnt % 2))
				MoveMk(-1.0*OfSt.x);
		}
	}
	if (m_pDlgMenu03)
		m_pDlgMenu03->ChkDoneMk();
	*/

	if (!pDoc->WorkingInfo.LastJob.bAoiOnePnl)
	{
#ifdef USE_MPE
		//IoWrite(_T("MB440151"), 1);	// ���ǳ� �̼ۻ��� ON (PC�� ON, OFF)
		pView->m_pMpe->Write(_T("MB440151"), 1);
#endif
		CString sData, sPath = PATH_WORKING_INFO;
		pDoc->WorkingInfo.LastJob.bMkOnePnl = pDoc->WorkingInfo.LastJob.bAoiOnePnl = TRUE;
		sData.Format(_T("%d"), pDoc->WorkingInfo.LastJob.bMkOnePnl ? 1 : 0);
		::WritePrivateProfileString(_T("Last Job"), _T("Marking One Pannel Move On"), sData, sPath);
		::WritePrivateProfileString(_T("Last Job"), _T("AOI One Pannel Move On"), sData, sPath);
	}
}

void CGvisR2R_PunchView::SetEngraveFd(double dDist)
{
	double fLen = pDoc->GetOnePnlLen();
	double dOffset = dDist - (fLen*2.0);
	MoveMk(dOffset);
}

void CGvisR2R_PunchView::MoveEngrave(double dOffset)
{
#ifdef USE_MPE
	long lData = (long)(dOffset * 1000.0);
	pView->m_pMpe->Write(_T("MB440199"), 1);		// ���κ� �ǵ� CW ON (PLC�� �ǵ��Ϸ� �� OFF)
	pView->m_pMpe->Write(_T("ML45078"), lData);		// ���κ� Feeding �ѷ� Offset(*1000, +:�� ����, -�� ����, PC�� ���� PLC���� ����)
#endif
}

BOOL CGvisR2R_PunchView::IsEngraveFd()
{
	if (m_nShareDnCnt > 0)
	{
		if (!(m_nShareDnCnt % 2))
		{
#ifdef USE_MPE
			if ((pDoc->m_pMpeSignal[7] & (0x01 << 13)) || (pDoc->m_pMpeSignal[7] & (0x01 << 15)))	// ���κ� �ǵ� CW ON (PLC�� �ǵ��Ϸ� �� OFF)
			{
				pDoc->LogAuto(_T("PLC: ���κ� �ǵ� CW ON (PLC�� �ǵ��Ϸ� �� OFF)"));
				return TRUE;
			}
#endif
			return FALSE;
		}
	}
	else
	{
		if (m_nShareUpCnt > 0)
		{
			if (!(m_nShareUpCnt % 2))
			{
#ifdef USE_MPE
				if ((pDoc->m_pMpeSignal[7] & (0x01 << 13)) || (pDoc->m_pMpeSignal[7] & (0x01 << 15)))	// ���κ� �ǵ� CW ON (PLC�� �ǵ��Ϸ� �� OFF)
				{
					pDoc->LogAuto(_T("PLC: ���κ� �ǵ� CW ON (PLC�� �ǵ��Ϸ� �� OFF)"));
					return TRUE;
				}
#endif
				return FALSE;
			}
		}
	}

	return TRUE;
}


double CGvisR2R_PunchView::GetEngraveFdLen()
{
	int nLast = pDoc->GetLastShotEngrave();

	double dLen = (double)nLast * _tstof(pDoc->WorkingInfo.LastJob.sOnePnlLen);
	return dLen;
}

double CGvisR2R_PunchView::GetAoiInitDist()
{
	double dInitD1 = _tstof(pDoc->WorkingInfo.Motion.sFdEngraveAoiInitDist) - _tstof(pDoc->WorkingInfo.LastJob.sOnePnlLen);
	return dInitD1;
}

double CGvisR2R_PunchView::GetAoiRemain()
{
#ifdef USE_MPE
	double dCurPosEngraveFd = (double)pDoc->m_pMpeData[1][0];	// ML44052	,	���κ� Feeding ���ڴ� ��(���� mm)
	double dRemain = _tstof(pDoc->WorkingInfo.Motion.sFdEngraveAoiInitDist) - dCurPosEngraveFd;
	return dRemain;
#else
	return 0.0;
#endif
}


LRESULT CGvisR2R_PunchView::wmClientReceived(WPARAM wParam, LPARAM lParam)
{
	if (!m_pEngrave)
		return (LRESULT)0;

	//int nCmd = (int)wParam;
	//SOCKET_DATA stSockData = m_pEngrave->GetSocketData();

	int nAcceptId = (int)wParam;
	SOCKET_DATA sSockData;
	SOCKET_DATA *pSocketData = (SOCKET_DATA*)lParam;
	SOCKET_DATA rSockData = *pSocketData;
	int nCmdCode = rSockData.nCmdCode;
	int nMsgId = rSockData.nMsgID;

	switch (nCmdCode)
	{
	case _GetSig:
		break;
	case _GetData:
		break;
	case _SetSig:
		if (m_pEngrave && m_pEngrave->IsConnected())
			m_pEngrave->GetSysSignal(rSockData);

		pView->m_bSetSig = TRUE;
		//if (m_pEngrave->m_bGetOpInfo)
		//{
		//	if (m_pDlgInfo)
		//		m_pDlgInfo->UpdateData();

		//	if (m_pDlgMenu01)
		//		m_pDlgMenu01->UpdateData();
		//}

		//if (m_pDlgMenu03)
		//	m_pDlgMenu03->UpdateSignal();
		break;
	case _SetData:
		if (m_pEngrave && m_pEngrave->IsConnected())
			m_pEngrave->GetSysData(rSockData);

		pView->m_bSetData = TRUE;
		//if (m_pEngrave->m_bGetOpInfo)
		//{
		//	if (m_pDlgMenu01)
		//		m_pDlgMenu01->UpdateData();
		//}

		////if (m_pDlgMenu02)
		////	m_pDlgMenu02->UpdateData();

		//if (m_pDlgMenu03)
		//	m_pDlgMenu03->UpdateData();

		//if (m_pDlgMenu04)
		//	m_pDlgMenu04->UpdateData();
		break;
	default:
		break;
	}


	return (LRESULT)1;
}

LRESULT CGvisR2R_PunchView::wmClientReceivedSr(WPARAM wParam, LPARAM lParam)
{
	int nCmd = (int)wParam;
	CString* sReceived = (CString*)lParam;

	switch (nCmd)
	{
	case SrTriggerInputOn:
		Get2dCode(m_sGet2dCodeLot, m_nGet2dCodeSerial);
		//if (m_pDlgMenu02)
		//{
		//	m_pDlgMenu02->Disp2dCode();
		//}
		break;
	default:
		break;
	}

	return (LRESULT)1;
}
void CGvisR2R_PunchView::SetEngraveFdPitch(double dPitch)
{
	pDoc->SetEngraveFdPitch(dPitch);
}

BOOL CGvisR2R_PunchView::IsConnectedEng()
{
	if (m_bDestroyedView)
		return FALSE;

#ifdef USE_ENGRAVE
	if (m_pEngrave)
	{
		if(m_pEngrave->IsConnected())
		{
			if(!m_bContEngraveF)
			{
				m_bContEngraveF = TRUE;
				DWORD dwStartTick = GetTickCount();

				//while(!m_pEngrave->SetSysData())
				//{
				//	Sleep(100);
				//	if (GetTickCount() >= (dwStartTick + DELAY_RESPONSE))
				//	{
				//		pView->ClrDispMsg();
				//		AfxMessageBox(_T(" WaitResponse() Time Out. \r\n m_pEngrave->IsConnected() !!!"));
				//		break;
				//	}
				//}
				//while (!m_pEngrave->SetSysSignal())
				//{
				//	Sleep(100);
				//	if (GetTickCount() >= (dwStartTick + DELAY_RESPONSE))
				//	{
				//		pView->ClrDispMsg();
				//		AfxMessageBox(_T(" WaitResponse() Time Out. \r\n m_pEngrave->IsConnected() !!!"));
				//		break;
				//	}
				//}
			}
			return TRUE;
		}
		else
		{
			if (m_bContEngraveF)
			{
				m_bContEngraveF = FALSE;
			}
			return FALSE;
		}
	}
#endif
	return FALSE;
}


BOOL CGvisR2R_PunchView::IsPinPos0()
{
	if (!m_pMotion)
		return FALSE;

	double pPos[2];
	pPos[0] = m_pMotion->m_dPinPosX[0];
	pPos[1] = m_pMotion->m_dPinPosY[0];

	double dCurrX = m_dEnc[AXIS_X0];
	double dCurrY = m_dEnc[AXIS_Y0];

	if (dCurrX < pPos[0] - 4.0 || dCurrX > pPos[0] + 4.0)
		return FALSE;
	if (dCurrY < pPos[1] - 4.0 || dCurrY > pPos[1] + 4.0)
		return FALSE;

	return TRUE;
}

BOOL CGvisR2R_PunchView::IsPinPos1()
{
	if (!m_pMotion)
		return FALSE;

	double pPos[2];
	pPos[0] = m_pMotion->m_dPinPosX[1];
	pPos[1] = m_pMotion->m_dPinPosY[1];

	double dCurrX = m_dEnc[AXIS_X1];
	double dCurrY = m_dEnc[AXIS_Y1];

	if (dCurrX < pPos[0] - 4.0 || dCurrX > pPos[0] + 4.0)
		return FALSE;
	if (dCurrY < pPos[1] - 4.0 || dCurrY > pPos[1] + 4.0)
		return FALSE;

	return TRUE;
}

void CGvisR2R_PunchView::RunShift2Mk()
{
	Shift2Mk();			// PCR �̵�(Buffer->Marked) // ���(WorkingInfo.LastJob.sSerial)
}

//BOOL CGvisR2R_PunchView::LoadAoiSpec()
//{
//	return  pDoc->LoadAoiSpec();
//}

BOOL CGvisR2R_PunchView::LoadMasterSpec()
{
	//int i;
	//CString sPath, sItem;
	//CString sSpecDir = pDoc->WorkingInfo.System.sPathCamSpecDir;
	//CString sModel = pDoc->WorkingInfo.LastJob.sModelUp;
	//CString sLayer = pDoc->WorkingInfo.LastJob.sLayerUp;

	//sPath.Format(_T("%s%s\\%s.ini"), sSpecDir, sModel, sLayer);

	return TRUE;
}

//void CGvisR2R_PunchView::SetMkPcsIdx(int nSerial)
//{
//	pDoc->SetMkPcsIdx(nSerial);
//}


// Call from Thread Function
void CGvisR2R_PunchView::UpdateYield(int nSerial)
{
	//pDoc->UpdateYield(nSerial);

	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;
	//int nId = nSerial % 2; // Ȧ��(1), ¦��(0)

	m_nSnTHREAD_UPDATAE_YIELD = nSerial;
	m_bTHREAD_UPDATE_YIELD_UP = TRUE;
	if(bDualTest)
		m_bTHREAD_UPDATE_YIELD_DN = TRUE;

	//if (pDoc->GetTestMode() == MODE_OUTER)
	//{
	//	m_bTHREAD_UPDATE_YIELD_ITS = TRUE;
	//	m_bTHREAD_UPDATE_YIELD_INNER_UP = TRUE;
	//	if (pDoc->WorkingInfo.LastJob.bDualTestInner)
	//	{
	//		m_bTHREAD_UPDATE_YIELD_INNER_DN = TRUE;
	//	}
	//}

}

void CGvisR2R_PunchView::UpdateYieldUp(int nSerial)
{
	pDoc->UpdateYieldUp(nSerial);
}

void CGvisR2R_PunchView::UpdateYieldDn(int nSerial)
{
	pDoc->UpdateYieldDn(nSerial);
}

void CGvisR2R_PunchView::UpdateYieldAllUp(int nSerial)
{
	pDoc->UpdateYieldAllUp(nSerial);
}

void CGvisR2R_PunchView::UpdateYieldAllDn(int nSerial)
{
	pDoc->UpdateYieldAllDn(nSerial);
}

void CGvisR2R_PunchView::UpdateYieldInnerUp(int nSerial)
{
	pDoc->UpdateYieldInnerUp(nSerial);
}

void CGvisR2R_PunchView::UpdateYieldInnerDn(int nSerial)
{
	pDoc->UpdateYieldInnerDn(nSerial);
}

void CGvisR2R_PunchView::UpdateYieldInnerAllUp(int nSerial)
{
	pDoc->UpdateYieldInnerAllUp(nSerial);
}

void CGvisR2R_PunchView::UpdateYieldInnerAllDn(int nSerial)
{
	pDoc->UpdateYieldInnerAllDn(nSerial);
}

void CGvisR2R_PunchView::UpdateYieldIts(int nSerial)
{
	pDoc->UpdateYieldIts(nSerial);
}

void CGvisR2R_PunchView::UpdateYield()
{
	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;
	int nSerial = m_nBufUpSerial[0];

	if (nSerial < 1)
	{
		Stop();
		MsgBox(_T("Error-UpdateYield() : m_nBufUpSerial[0] < 1"));
		return;
	}
	
	if (m_bLastProc && m_nBufUpSerial[0] == m_nLotEndSerial)
	{
			m_nSerialTHREAD_UPDATAE_YIELD[0] = nSerial;
			m_bTHREAD_UPDATAE_YIELD[0] = TRUE; // Cam[0]
			Sleep(30);
	}
	else
	{
		m_nSerialTHREAD_UPDATAE_YIELD[0] = nSerial;
		m_bTHREAD_UPDATAE_YIELD[0] = TRUE; // Cam[0]
		Sleep(30);

		if (m_nBufUpSerial[1] > 0)
		{
			m_nSerialTHREAD_UPDATAE_YIELD[1] = m_nBufUpSerial[1];
			m_bTHREAD_UPDATAE_YIELD[1] = TRUE; // Cam[1]
			Sleep(30);
		}
		else if(m_nBufUpSerial[1] < 0)
		{
			//Stop();
			//MsgBox(_T("Error-UpdateYield() : m_nBufUpSerial[1] < 1"));
			return;
		}
	}
}


void CGvisR2R_PunchView::DoShift2Mk()
{
	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;
	int nSerial;

	if (m_bLastProc && m_nBufUpSerial[0] == m_nLotEndSerial)
	{
		nSerial = m_nBufUpSerial[0];
		//if (nSerial > 0 && (nSerial % 2)) // First Shot number must be odd.
		if (nSerial > 0) // 20130202
			m_bTHREAD_SHIFT2MK = TRUE;
	}
	else
	{
		nSerial = m_nBufUpSerial[0];
		if (!m_bCont)
		{
			//if (nSerial > 0 && (nSerial % 2)) // First Shot number must be odd.
			if (nSerial > 0)
				m_bTHREAD_SHIFT2MK = TRUE;
			else
				Stop();
		}
		else
		{
			if (nSerial > 0)
				m_bTHREAD_SHIFT2MK = TRUE;
			else
				Stop();
		}
	}
}

void CGvisR2R_PunchView::DoAutoEng()
{
	//if (!IsAuto() || (MODE_INNER != pDoc->WorkingInfo.LastJob.nTestMode))
	if (!IsAuto() || (MODE_INNER != pDoc->GetTestMode()))
		return;

	// ���κ� ��ŷ���� ��ȣ�� Ȯ��
	DoAtuoGetEngStSignal();

	// ���κ� 2D �ڵ� Reading��ȣ�� Ȯ��
	DoAtuoGet2dReadStSignal();

/*	// LastProc Start
	DoAutoSetLastProcAtPlc();

	// CycleStop
	DoAutoChkCycleStop();

	// DispMsg
	DoAutoDispMsg();

	// ���κ� Marking Start
	DoAutoMarking();

	// ���κ� 2D �ڵ� Reading Start
	DoAuto2dReading(); */
}



//void CGvisR2R_PunchView::DoAtuoGetEngStSignal()
//{
//#ifdef USE_MPE
//	if (m_pMpe)
//	{
//		if (!pDoc->BtnStatus.EngAuto.MkSt && pDoc->BtnStatus.EngAuto.MkStF)
//		{
//			pDoc->BtnStatus.EngAuto.MkStF = FALSE;
//
//			m_pMpe->Write(_T("MB440103"), 0);			// 2D(GUI) ���� ���� Start��ȣ(PLC On->PC Off)
//
//			if (pDoc->m_pMpeSignal[0] & (0x01 << 2))	// ���κ� Feeding�Ϸ�(PLC�� On��Ű�� PC�� Ȯ���ϰ� Reset��Ŵ.)
//				m_pMpe->Write(_T("MB440102"), 0);		// ���κ� Feeding�Ϸ�
//
//			//m_bEngSt = TRUE;
//			//m_nEngStAuto = ENG_ST;
//		}
//
//		if ((pDoc->m_pMpeSignal[0] & (0x01 << 3) || m_bEngStSw) && !pDoc->BtnStatus.EngAuto.MkStF)// 2D(GUI) ���� ���� Start��ȣ(PLC On->PC Off)
//		{
//			pDoc->BtnStatus.EngAuto.MkStF = TRUE;
//			m_bEngStSw = FALSE;
//
//			if (m_pEngrave && m_pEngrave->IsConnected())
//			{ 
//				pDoc->BtnStatus.EngAuto.IsMkSt = FALSE;
//				m_pEngrave->SwEngAutoMkSt(TRUE);
//				Sleep(100);
//			}
//		}
//		else if ((pDoc->m_pMpeSignal[0] & (0x01 << 3) || m_bEngStSw) && pDoc->BtnStatus.EngAuto.MkStF)
//		{
//			if (m_pEngrave && m_pEngrave->IsConnected())
//			{
//				m_pEngrave->IsSwEngAutoMkSt(TRUE);
//				if (!pDoc->BtnStatus.EngAuto.IsMkSt)
//				{
//					m_pEngrave->SwEngAutoMkSt(TRUE);
//					Sleep(100);
//				}
//			}
//		}
//	}
//
//#endif
//}

//void CGvisR2R_PunchView::DoAtuoGet2dReadStSignal()
//{
//#ifdef USE_MPE
//	if (m_pMpe)
//	{
//		//if (IsRun())
//		//{
//			if (!pDoc->BtnStatus.EngAuto.Read2dSt)// && pDoc->BtnStatus.EngAuto.Read2dStF)
//			{
//				pDoc->BtnStatus.EngAuto.Read2dStF = FALSE;
//
//				m_pMpe->Write(_T("MB440105"), 0);			// ���κ� 2D ���� ���۽�ȣ(PLC On->PC Off)
//
//				if (pDoc->m_pMpeSignal[0] & (0x01 << 2))	// ���κ� Feeding�Ϸ�(PLC�� On��Ű�� PC�� Ȯ���ϰ� Reset��Ŵ.)
//					m_pMpe->Write(_T("MB440102"), 0);		// ���κ� Feeding�Ϸ�
//
//				//m_bEng2dSt = TRUE;
//				//m_nEng2dStAuto = ENG_2D_ST;
//			}
//		//}
//
//		//if (!m_bEng2dSt)
//		{
//			if ((pDoc->m_pMpeSignal[0] & (0x01 << 5) || m_bEng2dStSw))// && !pDoc->BtnStatus.EngAuto.Read2dStF)// ���κ� 2D ���� ���۽�ȣ(PLC On->PC Off)
//			{
//				pDoc->BtnStatus.EngAuto.Read2dStF = TRUE;
//				m_bEng2dStSw = FALSE;
//
//				if (m_pEngrave)
//				{
//					m_pEngrave->SwEngAuto2dReadSt(TRUE);
//				}
//			}
//		}
//	}
//
//#endif
//}


//void CGvisR2R_PunchView::DoAtuoGet2dReadStSignal()
//{
//#ifdef USE_MPE
//	if (m_pMpe)
//	{
//		if (!pDoc->BtnStatus.EngAuto.Read2dSt && pDoc->BtnStatus.EngAuto.Read2dStF)
//		{
//			pDoc->BtnStatus.EngAuto.Read2dStF = FALSE;
//
//			m_pMpe->Write(_T("MB440105"), 0);			// ���κ� 2D ���� ���۽�ȣ(PLC On->PC Off)
//
//			if (pDoc->m_pMpeSignal[0] & (0x01 << 2))	// ���κ� Feeding�Ϸ�(PLC�� On��Ű�� PC�� Ȯ���ϰ� Reset��Ŵ.)
//				m_pMpe->Write(_T("MB440102"), 0);		// ���κ� Feeding�Ϸ�
//
//			//m_bEng2dSt = TRUE;
//			//m_nEng2dStAuto = ENG_2D_ST;
//		}
//
//		if ((pDoc->m_pMpeSignal[0] & (0x01 << 5) || m_bEng2dStSw) && !pDoc->BtnStatus.EngAuto.Read2dStF)// ���κ� 2D ���� ���۽�ȣ(PLC On->PC Off)
//		{
//			pDoc->BtnStatus.EngAuto.Read2dStF = TRUE;
//			m_bEng2dStSw = FALSE;
//
//			if (m_pEngrave && m_pEngrave->IsConnected())
//			{
//				pDoc->BtnStatus.EngAuto.IsRead2dSt = FALSE;
//				m_pEngrave->SwEngAuto2dReadSt(TRUE);
//				Sleep(100);
//			}
//		}
//		else if ((pDoc->m_pMpeSignal[0] & (0x01 << 5) || m_bEng2dStSw) && pDoc->BtnStatus.EngAuto.Read2dStF)
//		{
//			if (m_pEngrave && m_pEngrave->IsConnected())
//			{
//				m_pEngrave->IsSwEngAuto2dReadSt(TRUE);
//				if (!pDoc->BtnStatus.EngAuto.IsRead2dSt)
//				{
//					m_pEngrave->SwEngAuto2dReadSt(TRUE);
//					Sleep(100);
//				}
//			}
//		}		
//	}
//#endif
//}

//void CGvisR2R_PunchView::DoAutoSetFdOffsetEngrave()
//{
//#ifdef USE_MPE
//	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;
//	double dAveX, dAveY;
//	CfPoint OfSt;
//	
//	//if (pDoc->m_pMpeSignal[1] & (0x01 << 6) && !m_bEngTestF)
//	//else if (!(pDoc->m_pMpeSignal[1] & (0x01 << 6)) && m_bEngTestF)
//	if ((pDoc->BtnStatus.EngAuto.OnMking && !m_bEngTestF) || (pDoc->BtnStatus.EngAuto.OnRead2d && !m_bEngTestF)) // ���κ� �˻���
//	{
//		m_bEngTestF = TRUE;
//		m_bEngTest = TRUE;
//	}
//	else if((!pDoc->BtnStatus.EngAuto.OnMking && m_bEngTestF) || (!pDoc->BtnStatus.EngAuto.OnRead2d && m_bEngTestF))
//	{
//		m_bEngTestF = FALSE;
//		m_bEngTest = FALSE;
//		m_bEngFdWriteF = FALSE;
//		pView->m_pMpe->Write(_T("MB44011A"), 0);					// ���κ� Feeding Offset Write �Ϸ�(PC�� Ȯ���ϰ� Reset��Ŵ.)
//	}
//
//	if (pDoc->m_pMpeSignal[1] & (0x01 << 10) && !m_bEngFdWrite)		// ���κ� Feeding Offset Write �Ϸ�(PC�� Ȯ���ϰ� Reset��Ŵ.)
//	{
//		m_bEngFdWrite = TRUE;
//	}
//	else if (!(pDoc->m_pMpeSignal[1] & (0x01 << 10)) && m_bEngFdWrite)
//	{
//		m_bEngFdWrite = FALSE;
//	}
//
//
//	if (m_bEngFdWrite&& !m_bEngFdWriteF)
//	{
//		m_bEngFdWriteF = TRUE;
//
//		GetEngOffset(OfSt);
//
//		dAveX = OfSt.x;
//		dAveY = OfSt.y;
//
//		if (m_pDlgMenu02)
//		{
//			m_pDlgMenu02->m_dEngFdOffsetX = OfSt.x;
//			m_pDlgMenu02->m_dEngFdOffsetY = OfSt.y;
//		}
//
//		pView->m_pMpe->Write(_T("ML45078"), (long)(dAveX*1000.0));	// ���κ� Feeding �ѷ� Offset(*1000, +:�� ����, -�� ����, PC�� ���� PLC���� ����)
//		pView->m_pMpe->Write(_T("MB44011A"), 0);					// ���κ� Feeding Offset Write �Ϸ�(PC�� Ȯ���ϰ� Reset��Ŵ.)
//		Sleep(100);
//	}
//	else if (!m_bEngFdWrite && m_bEngFdWriteF)
//	{
//		m_bEngFdWriteF = FALSE;
//		m_bEngTest = FALSE;
//	}
//#endif
//}

void CGvisR2R_PunchView::DoAtuoGetEngStSignal()
{
#ifdef USE_MPE
	if ((pDoc->m_pMpeSignal[0] & (0x01 << 3) || m_bEngStSw) && !pDoc->BtnStatus.EngAuto.MkStF)// 2D(GUI) ���� ���� Start��ȣ(PLC On->PC Off)
	{
		pDoc->BtnStatus.EngAuto.MkStF = TRUE;
		m_bEngStSw = FALSE;

		pDoc->BtnStatus.EngAuto.IsMkSt = FALSE;
		pDoc->SetCurrentInfoSignal(_SigInx::_EngAutoSeqMkSt, TRUE);
	}
	else if (pDoc->BtnStatus.EngAuto.IsMkSt && pDoc->BtnStatus.EngAuto.MkStF)
	{
		pDoc->BtnStatus.EngAuto.MkStF = FALSE;
		pDoc->SetCurrentInfoSignal(_SigInx::_EngAutoSeqMkSt, FALSE);
		if (m_pMpe)
		{
			m_pMpe->Write(_T("MB440103"), 0);			// 2D(GUI) ���� ���� Start��ȣ(PLC On->PC Off)

			//if (pDoc->m_pMpeSignal[0] & (0x01 << 2))	// ���κ� Feeding�Ϸ�(PLC�� On��Ű�� PC�� Ȯ���ϰ� Reset��Ŵ.)
			//	m_pMpe->Write(_T("MB440102"), 0);		// ���κ� Feeding�Ϸ�
		}
	}

	if (pDoc->m_pMpeSignal[0] & (0x01 << 2) && !pDoc->BtnStatus.EngAuto.FdDoneF)	// ���κ� Feeding�Ϸ�(PLC�� On��Ű�� PC�� Ȯ���ϰ� Reset��Ŵ.)
	{
		pDoc->BtnStatus.EngAuto.FdDoneF = TRUE;

		pDoc->BtnStatus.EngAuto.IsFdDone = FALSE;
		pDoc->SetCurrentInfoSignal(_SigInx::_EngAutoSeqFdDone, TRUE);
	}
	else if (pDoc->BtnStatus.EngAuto.IsFdDone && pDoc->BtnStatus.EngAuto.FdDoneF)
	{
		pDoc->BtnStatus.EngAuto.FdDoneF = FALSE;
		pDoc->SetCurrentInfoSignal(_SigInx::_EngAutoSeqFdDone, FALSE);
		if (m_pMpe)
		{
			m_pMpe->Write(_T("MB440102"), 0);		// ���κ� Feeding�Ϸ�(PLC�� On��Ű�� PC�� Ȯ���ϰ� Reset��Ŵ.)
		}
	}

#endif
}

void CGvisR2R_PunchView::DoAtuoGet2dReadStSignal()
{
	if ((pDoc->m_pMpeSignal[0] & (0x01 << 5) || m_bEng2dStSw) && !pDoc->BtnStatus.EngAuto.Read2dStF)// ���κ� 2D ���� ���۽�ȣ(PLC On->PC Off)
	{
		pDoc->BtnStatus.EngAuto.Read2dStF = TRUE;
		m_bEng2dStSw = FALSE;

		pDoc->BtnStatus.EngAuto.IsRead2dSt = FALSE;
		pDoc->SetCurrentInfoSignal(_SigInx::_EngAutoSeq2dReadSt, TRUE);
	}
	else if (pDoc->BtnStatus.EngAuto.IsRead2dSt && pDoc->BtnStatus.EngAuto.Read2dStF)
	{
		pDoc->BtnStatus.EngAuto.Read2dStF = FALSE;
		pDoc->SetCurrentInfoSignal(_SigInx::_EngAutoSeq2dReadSt, FALSE);
#ifdef USE_MPE
		if (m_pMpe)
		{
			m_pMpe->Write(_T("MB440105"), 0);			// ���κ� 2D ���� ���۽�ȣ(PLC On->PC Off)
			//if (pDoc->m_pMpeSignal[0] & (0x01 << 2))	// ���κ� Feeding�Ϸ�(PLC�� On��Ű�� PC�� Ȯ���ϰ� Reset��Ŵ.)
			//	m_pMpe->Write(_T("MB440102"), 0);		// ���κ� Feeding�Ϸ�
		}
#endif
	}
}


void CGvisR2R_PunchView::DoAutoSetFdOffsetEngrave()
{
	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;
	double dAveX, dAveY;
	CfPoint OfSt;

	if ((pDoc->BtnStatus.EngAuto.IsOnMking && !m_bEngTestF) || (pDoc->BtnStatus.EngAuto.IsOnRead2d && !m_bEngTestF)) // ���κ� �˻���
	{
		m_bEngTestF = TRUE;
		m_bEngTest = TRUE;
	}
	else if ((!pDoc->BtnStatus.EngAuto.IsOnMking && m_bEngTestF) || (!pDoc->BtnStatus.EngAuto.IsOnRead2d && m_bEngTestF))
	{
		m_bEngTestF = FALSE;
		m_bEngTest = FALSE;
		m_bEngFdWriteF = FALSE;
#ifdef USE_MPE
		if (m_pMpe)
		{
			m_pMpe->Write(_T("MB44011A"), 0);					// ���κ� Feeding Offset Write �Ϸ�(PC�� Ȯ���ϰ� Reset��Ŵ.)
			pDoc->LogAuto(_T("PC: ���κ� Feeding Offset Write �Ϸ�(PC�� Ȯ���ϰ� Reset��Ŵ.)"));
		}
#endif
	}

	if (pDoc->m_pMpeSignal[1] & (0x01 << 10) && !m_bEngFdWrite)		// ���κ� Feeding Offset Write �Ϸ�(PC�� Ȯ���ϰ� Reset��Ŵ.)
	{
		m_bEngFdWrite = TRUE;
	}
	else if (!(pDoc->m_pMpeSignal[1] & (0x01 << 10)) && m_bEngFdWrite)
	{
		m_bEngFdWrite = FALSE;
	}


	if (m_bEngFdWrite && !m_bEngFdWriteF)
	{
		m_bEngFdWriteF = TRUE;

		//if (MODE_INNER == pDoc->GetTestMode())
		//	GetCurrentInfoEng();

		GetEngOffset(OfSt);

		dAveX = OfSt.x;
		dAveY = OfSt.y;

		if (m_pDlgMenu02)
		{
			m_pDlgMenu02->m_dEngFdOffsetX = OfSt.x;
			m_pDlgMenu02->m_dEngFdOffsetY = OfSt.y;
		}

#ifdef USE_MPE
		if (m_pMpe)
		{
			pDoc->LogAuto(_T("PC: ���κ� Feeding Offset Write �Ϸ�(PC�� Ȯ���ϰ� Reset��Ŵ.)"));
			m_pMpe->Write(_T("ML45078"), (long)(dAveX*1000.0));	// ���κ� Feeding �ѷ� Offset(*1000, +:�� ����, -�� ����, PC�� ���� PLC���� ����)
			m_pMpe->Write(_T("MB44011A"), 0);					// ���κ� Feeding Offset Write �Ϸ�(PC�� Ȯ���ϰ� Reset��Ŵ.)
		}
		Sleep(10);
#endif
	}
	else if (!m_bEngFdWrite && m_bEngFdWriteF)
	{
		m_bEngFdWriteF = FALSE;
		m_bEngTest = FALSE;
	}

}

void CGvisR2R_PunchView::DoAutoMarkingEngrave()
{	
	// ���κ� ��ŷ�� ON (PC�� ON, OFF)
	if ( pDoc->BtnStatus.EngAuto.IsOnMking && !(pDoc->m_pMpeSignal[6] & (0x01 << 3)) ) // ���κ� ��ŷ�� ON (PC�� ON, OFF)
	{
		pDoc->SetCurrentInfoSignal(_SigInx::_EngAutoSeqOnMkIng, TRUE);
#ifdef USE_MPE
		if (m_pMpe)
		{
			pDoc->LogAuto(_T("PC: 2D(GUI) ���� ����Running��ȣ ON (PC On->PC Off)"));
			m_pMpe->Write(_T("MB440173"), 1); // 2D(GUI) ���� ����Running��ȣ(PC On->PC Off)
		}
#endif
	}
	else if ( !pDoc->BtnStatus.EngAuto.IsOnMking && (pDoc->m_pMpeSignal[6] & (0x01 << 3)) )
	{
		pDoc->SetCurrentInfoSignal(_SigInx::_EngAutoSeqOnMkIng, FALSE);
#ifdef USE_MPE
		if (m_pMpe)
		{
			pDoc->LogAuto(_T("PC: 2D(GUI) ���� ����Running��ȣ OFF (PC On->PC Off)"));
			m_pMpe->Write(_T("MB440173"), 0); // 2D(GUI) ���� ����Running��ȣ(PC On->PC Off)
		}
#endif
	}

	// ���κ� ��ŷ�Ϸ� ON (PC�� ON, OFF)
	if (pDoc->BtnStatus.EngAuto.IsMkDone && !(pDoc->m_pMpeSignal[6] & (0x01 << 4))) // ���κ� �۾��Ϸ�.(PC�� On, PLC�� Ȯ�� �� Off)
	{
		pDoc->SetCurrentInfoSignal(_SigInx::_EngAutoSeqMkDone, TRUE);
#ifdef USE_MPE
		if (m_pMpe)
		{
			pDoc->LogAuto(_T("PC: ���κ� �۾��Ϸ� ON (PC�� On, PLC�� Ȯ�� �� Off)"));
			m_pMpe->Write(_T("MB440174"), 1); // ���κ� �۾��Ϸ�.(PC�� On, PLC�� Ȯ�� �� Off)
		}
#endif
	}
	else if (!pDoc->BtnStatus.EngAuto.IsMkDone && (pDoc->m_pMpeSignal[6] & (0x01 << 4))) // ���κ� �۾��Ϸ�.(PC�� On, PLC�� Ȯ�� �� Off)
	{
		pDoc->SetCurrentInfoSignal(_SigInx::_EngAutoSeqMkDone, FALSE);
//#ifdef USE_MPE
//		if (m_pMpe)
//			m_pMpe->Write(_T("MB440174"), 0); // ���κ� �۾��Ϸ�.(PC�� On, PLC�� Ȯ�� �� Off)
//#endif
	}

	// ���κ� 2D ���� �۾��� ��ȣ
	if (pDoc->BtnStatus.EngAuto.IsOnRead2d && !(pDoc->m_pMpeSignal[6] & (0x01 << 8))) // ���κ� 2D ���� �۾��� ��ȣ(PC On->PC Off)
	{
		pDoc->SetCurrentInfoSignal(_SigInx::_EngAutoSeqOnReading2d, TRUE);
#ifdef USE_MPE
		if (m_pMpe)
		{
			pDoc->LogAuto(_T("PC: ���κ� 2D ���� �۾��� ��ȣ ON (PC On->PC Off)"));
			m_pMpe->Write(_T("MB440178"), 1); // ���κ� 2D ���� �۾��� ��ȣ(PC On->PC Off)
		}
#endif
	}
	else if (!pDoc->BtnStatus.EngAuto.IsOnRead2d && (pDoc->m_pMpeSignal[6] & (0x01 << 8)))
	{
		pDoc->SetCurrentInfoSignal(_SigInx::_EngAutoSeqOnReading2d, FALSE);
#ifdef USE_MPE
		if (m_pMpe)
		{
			pDoc->LogAuto(_T("PC: ���κ� 2D ���� �۾��� ��ȣ OFF (PC On->PC Off)"));
			m_pMpe->Write(_T("MB440178"), 0); // ���κ� 2D ���� �۾��� ��ȣ(PC On->PC Off)
		}
#endif
	}

	// ���κ� 2D ���� �۾��Ϸ� ��ȣ
	if (pDoc->BtnStatus.EngAuto.IsRead2dDone && !(pDoc->m_pMpeSignal[6] & (0x01 << 9))) // ���κ� 2D ���� �۾��Ϸ� ��ȣ.(PC�� On, PLC�� Ȯ�� �� Off)
	{
		pDoc->SetCurrentInfoSignal(_SigInx::_EngAutoSeq2dReadDone, TRUE);
#ifdef USE_MPE
		if (m_pMpe)
		{
			pDoc->LogAuto(_T("PC: ���κ� 2D ���� �۾��Ϸ� ��ȣ ON (PC�� On, PLC�� Ȯ�� �� Off)"));
			m_pMpe->Write(_T("MB440179"), 1); // ���κ� 2D ���� �۾��Ϸ� ��ȣ.(PC�� On, PLC�� Ȯ�� �� Off)
		}
#endif
	}
	else if (!pDoc->BtnStatus.EngAuto.IsRead2dDone && (pDoc->m_pMpeSignal[6] & (0x01 << 9)))
	{
		pDoc->SetCurrentInfoSignal(_SigInx::_EngAutoSeq2dReadDone, FALSE);
//#ifdef USE_MPE
//		if (m_pMpe)
//			m_pMpe->Write(_T("MB440179"), 0); // ���κ� 2D ���� �۾��Ϸ� ��ȣ.(PC�� On, PLC�� Ȯ�� �� Off)
//#endif
	}

}

void CGvisR2R_PunchView::SetEngFd()
{
	CfPoint OfSt;
	if (GetEngOffset(OfSt))
	{
		if (m_pDlgMenu02)
		{
			m_pDlgMenu02->m_dEngFdOffsetX = OfSt.x;
			m_pDlgMenu02->m_dEngFdOffsetY = OfSt.y;
		}
	}

	MoveEng(-1.0*OfSt.x);
	if (m_pDlgMenu03)
		m_pDlgMenu03->ChkDoneEngrave();

	if (!pDoc->WorkingInfo.LastJob.bAoiOnePnl)
	{
#ifdef USE_MPE
		pView->m_pMpe->Write(_T("MB440151"), 1);	// ���ǳ� �̼ۻ��� ON (PC�� ON, OFF)
#endif
		CString sData, sPath = PATH_WORKING_INFO;
		pDoc->WorkingInfo.LastJob.bMkOnePnl = pDoc->WorkingInfo.LastJob.bAoiOnePnl = pDoc->WorkingInfo.LastJob.bEngraveOnePnl = TRUE;
		sData.Format(_T("%d"), pDoc->WorkingInfo.LastJob.bMkOnePnl ? 1 : 0);
		::WritePrivateProfileString(_T("Last Job"), _T("Marking One Pannel Move On"), sData, sPath);
		::WritePrivateProfileString(_T("Last Job"), _T("AOI One Pannel Move On"), sData, sPath);
		::WritePrivateProfileString(_T("Last Job"), _T("Engrave One Pannel Move On"), sData, sPath);
	}
}

BOOL CGvisR2R_PunchView::GetEngOffset(CfPoint &OfSt)
{
	return pDoc->GetEngOffset(OfSt);
}


void CGvisR2R_PunchView::MoveEng(double dOffset)
{
	long lData = (long)(dOffset * 1000.0);
	pView->m_pMpe->Write(_T("MB440160"), 1);	// �˻�� �ǵ� ON (PLC�� �ǵ��Ϸ� �� OFF)
	pView->m_pMpe->Write(_T("ML45064"), lData);	// �˻�� Feeding �ѷ� Offset(*1000, +:�� ����, -�� ����)
}

void CGvisR2R_PunchView::SetMyMsgYes()
{
	if (m_pDlgMyMsg)
	{
		if (m_pDlgMyMsg->m_pDlgMyMsgSub01)
		{
			((CDlgMyMsgSub01*)(m_pDlgMyMsg->m_pDlgMyMsgSub01))->ClickYes();
		}
	}
}

void CGvisR2R_PunchView::SetMyMsgNo()
{
	if (m_pDlgMyMsg)
	{
		if (m_pDlgMyMsg->m_pDlgMyMsgSub01)
		{
			((CDlgMyMsgSub01*)(m_pDlgMyMsg->m_pDlgMyMsgSub01))->ClickNo();
		}
	}
}

void CGvisR2R_PunchView::SetMyMsgOk()
{
	if (m_pDlgMyMsg)
	{
		if (m_pDlgMyMsg->m_pDlgMyMsgSub02)
		{
			((CDlgMyMsgSub02*)(m_pDlgMyMsg->m_pDlgMyMsgSub02))->ClickOk();
		}
	}
}

void CGvisR2R_PunchView::InitAutoEngSignal()
{
	pDoc->BtnStatus.EngAuto._Init();

	m_bEngFdWrite = FALSE;
	m_bEngFdWriteF = FALSE;
	m_bEngTest = FALSE;
	m_bEngTestF = FALSE;

	m_pMpe->Write(_T("MB440103"), 0); // 2D(GUI) ���� ���� Start��ȣ(PLC On->PC Off)
	m_pMpe->Write(_T("MB440173"), 0); // 2D(GUI) ���� ����Running��ȣ(PC On->PC Off)
	m_pMpe->Write(_T("MB440174"), 0); // ���κ� �۾��Ϸ�.(PC�� On, PLC�� Ȯ�� �� Off)

	m_pMpe->Write(_T("MB440105"), 0); // ���κ� 2D ���� ���۽�ȣ(PLC On->PC Off)
	m_pMpe->Write(_T("MB440178"), 0); // ���κ� 2D ���� �۾��� ��ȣ(PC On->PC Off)
	m_pMpe->Write(_T("MB440179"), 0); // ���κ� 2D ���� �۾��Ϸ� ��ȣ.(PC�� On, PLC�� Ȯ�� �� Off)

	m_pMpe->Write(_T("MB440102"), 0); // ���κ� Feeding�Ϸ�(PLC�� On��Ű�� PC�� Ȯ���ϰ� Reset��Ŵ.)

	pDoc->SetCurrentInfoSignal(_SigInx::_EngAutoSeqMkSt, FALSE);
	pDoc->SetCurrentInfoSignal(_SigInx::_EngAutoSeq2dReadSt, FALSE);
	pDoc->SetCurrentInfoSignal(_SigInx::_EngAutoSeqOnMkIng, FALSE);
	pDoc->SetCurrentInfoSignal(_SigInx::_EngAutoSeqMkDone, FALSE);
	pDoc->SetCurrentInfoSignal(_SigInx::_EngAutoSeqOnReading2d, FALSE);
	pDoc->SetCurrentInfoSignal(_SigInx::_EngAutoSeq2dReadDone, FALSE);
	pDoc->SetCurrentInfoSignal(_SigInx::_EngAutoSeqFdDone, FALSE);
}

BOOL CGvisR2R_PunchView::GetCurrentInfoSignal()
{
	pDoc->BtnStatus.EngAuto.IsInit = pDoc->GetCurrentInfoSignal(_SigInx::_EngAutoInit);
	pDoc->BtnStatus.EngAuto.IsMkSt = pDoc->GetCurrentInfoSignal(_SigInx::_EngAutoSeqMkSt);
	pDoc->BtnStatus.EngAuto.IsOnMking = pDoc->GetCurrentInfoSignal(_SigInx::_EngAutoSeqOnMkIng);
	pDoc->BtnStatus.EngAuto.IsMkDone = pDoc->GetCurrentInfoSignal(_SigInx::_EngAutoSeqMkDone);
	pDoc->BtnStatus.EngAuto.IsRead2dSt = pDoc->GetCurrentInfoSignal(_SigInx::_EngAutoSeq2dReadSt);
	pDoc->BtnStatus.EngAuto.IsOnRead2d = pDoc->GetCurrentInfoSignal(_SigInx::_EngAutoSeqOnReading2d);
	pDoc->BtnStatus.EngAuto.IsRead2dDone = pDoc->GetCurrentInfoSignal(_SigInx::_EngAutoSeq2dReadDone);
	pDoc->BtnStatus.EngAuto.IsFdDone = pDoc->GetCurrentInfoSignal(_SigInx::_EngAutoSeqFdDone);
	
	return TRUE;
}

void CGvisR2R_PunchView::SetLastSerialEng(int nSerial)
{
	if (m_pDlgFrameHigh)
		m_pDlgFrameHigh->SetEngraveLastShot(nSerial);

	pDoc->SetLastSerialEng(nSerial);
}

BOOL CGvisR2R_PunchView::GetCurrentInfoEng()
{
	return pDoc->GetCurrentInfoEng();
}

UINT CGvisR2R_PunchView::ThreadProc10(LPVOID lpContext)	// UpdateReelmapYieldUp()
{
	// Turn the passed in 'this' pointer back into a CProgressMgr instance
	CGvisR2R_PunchView* pThread = reinterpret_cast<CGvisR2R_PunchView*>(lpContext);

	BOOL bLock = FALSE;
	DWORD dwTick = GetTickCount();
	DWORD dwShutdownEventCheckPeriod = 0; // thread shutdown event check period

	pThread->m_bThread[10] = TRUE;
	while (WAIT_OBJECT_0 != WaitForSingleObject(pThread->m_Thread[10].GetShutdownEvent(), dwShutdownEventCheckPeriod))
	{
		pThread->m_dwThreadTick[10] = GetTickCount() - dwTick;
		dwTick = GetTickCount();

		if (pThread->m_bTHREAD_REELMAP_YIELD_UP) // Yield Reelmap
		{
			if (!pThread->m_bTHREAD_UPDATE_REELMAP_UP) // Write Reelmap
			{
				pThread->UpdateReelmapYieldUp(); // Yield Reelmap
				pThread->m_bTHREAD_REELMAP_YIELD_UP = FALSE;
				if (!pDoc->WorkingInfo.LastJob.bDualTest)
				{
					if (pDoc->GetTestMode() == MODE_OUTER)
						pThread->m_bTHREAD_REELMAP_YIELD_ITS = TRUE;
				}

				Sleep(0);
			}
			else
				Sleep(30);
		}
		else
			Sleep(30);
	}

	pThread->m_bThread[10] = FALSE;

	return 0;
}

UINT CGvisR2R_PunchView::ThreadProc11(LPVOID lpContext)	// UpdateReelmapYieldDn()
{
	// Turn the passed in 'this' pointer back into a CProgressMgr instance
	CGvisR2R_PunchView* pThread = reinterpret_cast<CGvisR2R_PunchView*>(lpContext);

	BOOL bLock = FALSE;
	DWORD dwTick = GetTickCount();
	DWORD dwShutdownEventCheckPeriod = 0; // thread shutdown event check period

	pThread->m_bThread[11] = TRUE;
	while (WAIT_OBJECT_0 != WaitForSingleObject(pThread->m_Thread[11].GetShutdownEvent(), dwShutdownEventCheckPeriod))
	{
		pThread->m_dwThreadTick[11] = GetTickCount() - dwTick;
		dwTick = GetTickCount();

		if (pThread->m_bTHREAD_REELMAP_YIELD_DN) // Yield Reelmap
		{
			if (!pThread->m_bTHREAD_UPDATE_REELMAP_DN && !pThread->m_bTHREAD_UPDATE_REELMAP_ALLUP && !pThread->m_bTHREAD_UPDATE_REELMAP_ALLDN) // Write Reelmap
			{
				pThread->UpdateReelmapYieldDn(); // Yield Reelmap
				pThread->m_bTHREAD_REELMAP_YIELD_DN = FALSE;
				pThread->m_bTHREAD_REELMAP_YIELD_ALLUP = TRUE;
				pThread->m_bTHREAD_REELMAP_YIELD_ALLDN = TRUE;
				Sleep(0);
			}
			else
				Sleep(30);
		}
		else
			Sleep(30);
	}

	pThread->m_bThread[11] = FALSE;

	return 0;
}

UINT CGvisR2R_PunchView::ThreadProc12(LPVOID lpContext)	// UpdateReelmapYieldAllUp()
{
	// Turn the passed in 'this' pointer back into a CProgressMgr instance
	CGvisR2R_PunchView* pThread = reinterpret_cast<CGvisR2R_PunchView*>(lpContext);

	BOOL bLock = FALSE;
	DWORD dwTick = GetTickCount();
	DWORD dwShutdownEventCheckPeriod = 0; // thread shutdown event check period

	pThread->m_bThread[12] = TRUE;
	while (WAIT_OBJECT_0 != WaitForSingleObject(pThread->m_Thread[12].GetShutdownEvent(), dwShutdownEventCheckPeriod))
	{
		pThread->m_dwThreadTick[12] = GetTickCount() - dwTick;
		dwTick = GetTickCount();

		if (pThread->m_bTHREAD_REELMAP_YIELD_ALLUP) // Yield Reelmap
		{
			if (!pThread->m_bTHREAD_UPDATE_REELMAP_ALLUP) // Write Reelmap
			{
				pThread->UpdateReelmapYieldAllUp(); // Yield Reelmap
				pThread->m_bTHREAD_REELMAP_YIELD_ALLUP = FALSE;
				if (pDoc->GetTestMode() == MODE_OUTER)
					pThread->m_bTHREAD_REELMAP_YIELD_ITS = TRUE;
				Sleep(0);
			}
			else
				Sleep(30);
		}
		else
			Sleep(30);
	}

	pThread->m_bThread[12] = FALSE;

	return 0;
}

UINT CGvisR2R_PunchView::ThreadProc13(LPVOID lpContext)	// UpdateReelmapYieldAllDn()
{
	// Turn the passed in 'this' pointer back into a CProgressMgr instance
	CGvisR2R_PunchView* pThread = reinterpret_cast<CGvisR2R_PunchView*>(lpContext);

	BOOL bLock = FALSE;
	DWORD dwTick = GetTickCount();
	DWORD dwShutdownEventCheckPeriod = 0; // thread shutdown event check period

	pThread->m_bThread[13] = TRUE;
	while (WAIT_OBJECT_0 != WaitForSingleObject(pThread->m_Thread[13].GetShutdownEvent(), dwShutdownEventCheckPeriod))
	{
		pThread->m_dwThreadTick[13] = GetTickCount() - dwTick;
		dwTick = GetTickCount();

		if (pThread->m_bTHREAD_REELMAP_YIELD_ALLDN) // Yield Reelmap
		{
			if (!pThread->m_bTHREAD_UPDATE_REELMAP_ALLDN) // Write Reelmap
			{
				pThread->UpdateReelmapYieldAllDn(); // Yield Reelmap
				pThread->m_bTHREAD_REELMAP_YIELD_ALLDN = FALSE;
				Sleep(0);
			}
			else
				Sleep(30);
		}
		else
			Sleep(30);
	}

	pThread->m_bThread[13] = FALSE;

	return 0;
}

UINT CGvisR2R_PunchView::ThreadProc14(LPVOID lpContext)	// ReloadReelmapUp()
{
	// Turn the passed in 'this' pointer back into a CProgressMgr instance
	CGvisR2R_PunchView* pThread = reinterpret_cast<CGvisR2R_PunchView*>(lpContext);

	BOOL bLock = FALSE;
	DWORD dwTick = GetTickCount();
	DWORD dwShutdownEventCheckPeriod = 0; // thread shutdown event check period

	pThread->m_bThread[14] = TRUE;
	while (WAIT_OBJECT_0 != WaitForSingleObject(pThread->m_Thread[14].GetShutdownEvent(), dwShutdownEventCheckPeriod))
	{
		pThread->m_dwThreadTick[14] = GetTickCount() - dwTick;
		dwTick = GetTickCount();

		if (pThread->m_bTHREAD_RELOAD_RST_UP)
		{
			pThread->ReloadReelmapUp();
			pThread->m_bTHREAD_RELOAD_RST_UP = FALSE;
			Sleep(0);
		}
		else
			Sleep(30);
	}

	pThread->m_bThread[14] = FALSE;

	return 0;
}

UINT CGvisR2R_PunchView::ThreadProc15(LPVOID lpContext)	// ReloadReelmapDn()
{
	// Turn the passed in 'this' pointer back into a CProgressMgr instance
	CGvisR2R_PunchView* pThread = reinterpret_cast<CGvisR2R_PunchView*>(lpContext);

	BOOL bLock = FALSE;
	DWORD dwTick = GetTickCount();
	DWORD dwShutdownEventCheckPeriod = 0; // thread shutdown event check period

	pThread->m_bThread[15] = TRUE;
	while (WAIT_OBJECT_0 != WaitForSingleObject(pThread->m_Thread[15].GetShutdownEvent(), dwShutdownEventCheckPeriod))
	{
		pThread->m_dwThreadTick[15] = GetTickCount() - dwTick;
		dwTick = GetTickCount();

		if (pThread->m_bTHREAD_RELOAD_RST_DN)
		{
			pThread->ReloadReelmapDn();
			pThread->m_bTHREAD_RELOAD_RST_DN = FALSE;
			Sleep(0);
		}
		else
			Sleep(30);
	}

	pThread->m_bThread[15] = FALSE;

	return 0;
}

UINT CGvisR2R_PunchView::ThreadProc16(LPVOID lpContext)	// ReloadReelmapAllUp()
{
	// Turn the passed in 'this' pointer back into a CProgressMgr instance
	CGvisR2R_PunchView* pThread = reinterpret_cast<CGvisR2R_PunchView*>(lpContext);

	BOOL bLock = FALSE;
	DWORD dwTick = GetTickCount();
	DWORD dwShutdownEventCheckPeriod = 0; // thread shutdown event check period

	pThread->m_bThread[16] = TRUE;
	while (WAIT_OBJECT_0 != WaitForSingleObject(pThread->m_Thread[16].GetShutdownEvent(), dwShutdownEventCheckPeriod))
	{
		pThread->m_dwThreadTick[16] = GetTickCount() - dwTick;
		dwTick = GetTickCount();

		if (pThread->m_bTHREAD_RELOAD_RST_ALLUP)
		{
			pThread->ReloadReelmapAllUp();
			pThread->m_bTHREAD_RELOAD_RST_ALLUP = FALSE;
			Sleep(0);
		}
		else
			Sleep(30);
	}

	pThread->m_bThread[16] = FALSE;

	return 0;
}

UINT CGvisR2R_PunchView::ThreadProc17(LPVOID lpContext)	// ReloadReelmapAllDn()
{
	// Turn the passed in 'this' pointer back into a CProgressMgr instance
	CGvisR2R_PunchView* pThread = reinterpret_cast<CGvisR2R_PunchView*>(lpContext);

	BOOL bLock = FALSE;
	DWORD dwTick = GetTickCount();
	DWORD dwShutdownEventCheckPeriod = 0; // thread shutdown event check period

	pThread->m_bThread[17] = TRUE;
	while (WAIT_OBJECT_0 != WaitForSingleObject(pThread->m_Thread[17].GetShutdownEvent(), dwShutdownEventCheckPeriod))
	{
		pThread->m_dwThreadTick[17] = GetTickCount() - dwTick;
		dwTick = GetTickCount();

		if (pThread->m_bTHREAD_RELOAD_RST_ALLDN)
		{
			pThread->ReloadReelmapAllDn();
			pThread->m_bTHREAD_RELOAD_RST_ALLDN = FALSE;
			Sleep(0);
		}
		else
			Sleep(30);
	}

	pThread->m_bThread[17] = FALSE;

	return 0;
}

void CGvisR2R_PunchView::UpdateReelmapYieldUp()
{
	if (pDoc->m_pReelMapUp)
		pDoc->m_pReelMapUp->UpdateReelmapYield();					// ���� �ؽ�Ʈ ������ ���������� ������Ʈ��.
}

void CGvisR2R_PunchView::UpdateReelmapYieldAllUp()
{
	if (pDoc->m_pReelMapAllUp)
		pDoc->m_pReelMapAllUp->UpdateReelmapYield();					// ���� �ؽ�Ʈ ������ ���������� ������Ʈ��.
}

void CGvisR2R_PunchView::UpdateReelmapYieldDn()
{
	if (pDoc->m_pReelMapDn)
		pDoc->m_pReelMapDn->UpdateReelmapYield();					// ���� �ؽ�Ʈ ������ ���������� ������Ʈ��.
}

void CGvisR2R_PunchView::UpdateReelmapYieldAllDn()
{
	if (pDoc->m_pReelMapAllDn)
		pDoc->m_pReelMapAllDn->UpdateReelmapYield();					// ���� �ؽ�Ʈ ������ ���������� ������Ʈ��.
}

void CGvisR2R_PunchView::UpdateReelmapYieldIts()
{
	if (pDoc->m_pReelMapIts)
		pDoc->m_pReelMapIts->UpdateReelmapYield();					// ���� �ؽ�Ʈ ������ ���������� ������Ʈ��.
}

BOOL CGvisR2R_PunchView::IsConnectedSr()
{
	if (m_bDestroyedView)
		return FALSE;

	if (m_pSr1000w)
	{
		return m_pSr1000w->IsConnected();
	}

	return FALSE;
}

BOOL CGvisR2R_PunchView::Set2dRead(BOOL bRun)	// Marking Start
{
	if (!pView || !pView->m_pSr1000w)
		return FALSE;

	return (pView->m_pSr1000w->DoRead2DCode());
}

BOOL  CGvisR2R_PunchView::Is2dReadDone()
{
	if (!pView || !pView->m_pSr1000w)
		return FALSE;

	return (!pView->m_pSr1000w->IsRunning());
}

BOOL CGvisR2R_PunchView::Get2dCode(CString &sLot, int &nSerial)
{
	if (!m_pSr1000w)
		return FALSE;

	CString sData;
	if (m_pSr1000w->Get2DCode(sData))
	{
		int nPos = sData.ReverseFind('-');
		if (nPos != -1)
		{
			pDoc->m_sOrderNum = sData.Left(nPos);
			pDoc->m_sShotNum = sData.Right(sData.GetLength() - nPos - 1);
			pDoc->m_nShotNum = _tstoi(pDoc->m_sShotNum);
			sLot = pDoc->m_sOrderNum;
			nSerial = pDoc->m_nShotNum;
			//GetDlgItem(IDC_STC_32)->SetWindowText(pDoc->m_sOrderNum);
			//GetDlgItem(IDC_STC_34)->SetWindowText(pDoc->m_sShotNum);
		}
		else
		{
			pView->MsgBox(sData);
			//AfxMessageBox(sData);
		}

		return TRUE;
	}

	return FALSE;
}
CString CGvisR2R_PunchView::GetCurrentDBName()
{
	BOOL bRtn = FALSE;
	CString sName = _T("");

	if (m_pDts)
	{
		if (m_pDts->IsUseDts())
		{
			bRtn = m_pDts->GetCurrentDBName(sName);
			if (!bRtn)
			{
				pView->ClrDispMsg();
				AfxMessageBox(_T("Error - GetCurrentDBName()."), MB_ICONSTOP | MB_OK);
			}
		}
	}

	return sName;
}

BOOL CGvisR2R_PunchView::GetDtsPieceOut(int nSerial, int* pPcsOutIdx, int& nTotPcsOut)
{
	BOOL bRtn = FALSE;
	int nIdx = pDoc->GetPcrIdx0(nSerial);					// ����ȭ����� �ε���
	CString sLot = pDoc->m_pPcr[0][nIdx]->m_sLot;

	if (m_pDts)
	{
		if (m_pDts->IsUseDts())
		{
			bRtn = m_pDts->LoadPieceOut(sLot, nSerial, pPcsOutIdx, nTotPcsOut);
			if (!bRtn)
			{
				pView->ClrDispMsg();
				AfxMessageBox(_T("Error - GetDtsPieceOut()."), MB_ICONSTOP | MB_OK);
			}
		}
	}

	return bRtn;
}

UINT CGvisR2R_PunchView::ThreadProc18(LPVOID lpContext) // WriteReelmapIts()
{
	// Turn the passed in 'this' pointer back into a CProgressMgr instance
	CGvisR2R_PunchView* pThread = reinterpret_cast<CGvisR2R_PunchView*>(lpContext);

	BOOL bLock = FALSE;
	DWORD dwTick = GetTickCount();
	DWORD dwShutdownEventCheckPeriod = 0; // thread shutdown event check period

	pThread->m_bThread[18] = TRUE;
	while (WAIT_OBJECT_0 != WaitForSingleObject(pThread->m_Thread[18].GetShutdownEvent(), dwShutdownEventCheckPeriod))
	{
		pThread->m_dwThreadTick[18] = GetTickCount() - dwTick;
		dwTick = GetTickCount();

		if (pThread->m_bTHREAD_UPDATE_REELMAP_ITS) // Write Reelmap
		{
			pThread->WriteReelmapIts();
			pThread->m_bTHREAD_UPDATE_REELMAP_ITS = FALSE;
			Sleep(0);
		}
		else
			Sleep(30);
	}

	pThread->m_bThread[18] = FALSE;

	return 0;
}


UINT CGvisR2R_PunchView::ThreadProc19(LPVOID lpContext) // DispDefImgInner()
{
	// Turn the passed in 'this' pointer back into a CProgressMgr instance
	CGvisR2R_PunchView* pThread = reinterpret_cast<CGvisR2R_PunchView*>(lpContext);

	BOOL bLock = FALSE;
	DWORD dwTick = GetTickCount();
	DWORD dwShutdownEventCheckPeriod = 0; // thread shutdown event check period

	pThread->m_bThread[19] = TRUE;
	while (WAIT_OBJECT_0 != WaitForSingleObject(pThread->m_Thread[19].GetShutdownEvent(), dwShutdownEventCheckPeriod))
	{
		pThread->m_dwThreadTick[19] = GetTickCount() - dwTick;
		dwTick = GetTickCount();

		if (!bLock)
		{
			bLock = TRUE;

			if (pThread->m_bTHREAD_DISP_DEF_INNER)
			{
				pThread->DispDefImgInner();
				Sleep(0);
			}
			else
				Sleep(30);


			bLock = FALSE;
		}
	}
	pThread->m_bThread[19] = FALSE;

	return 0;
}


UINT CGvisR2R_PunchView::ThreadProc20(LPVOID lpContext)	// UpdateReelmapYieldIts()
{
	// Turn the passed in 'this' pointer back into a CProgressMgr instance
	CGvisR2R_PunchView* pThread = reinterpret_cast<CGvisR2R_PunchView*>(lpContext);

	BOOL bLock = FALSE;
	DWORD dwTick = GetTickCount();
	DWORD dwShutdownEventCheckPeriod = 0; // thread shutdown event check period

	pThread->m_bThread[20] = TRUE;
	while (WAIT_OBJECT_0 != WaitForSingleObject(pThread->m_Thread[20].GetShutdownEvent(), dwShutdownEventCheckPeriod))
	{
		pThread->m_dwThreadTick[20] = GetTickCount() - dwTick;
		dwTick = GetTickCount();

		if (pThread->m_bTHREAD_REELMAP_YIELD_ITS) // Yield Reelmap
		{
			pThread->UpdateReelmapYieldIts(); // Yield Reelmap
			pThread->m_bTHREAD_REELMAP_YIELD_ITS = FALSE;
			Sleep(0);
		}
		else
			Sleep(30);
	}

	pThread->m_bThread[20] = FALSE;

	return 0;
}


UINT CGvisR2R_PunchView::ThreadProc21(LPVOID lpContext)	// ReloadReelmapUpInner()
{
	// Turn the passed in 'this' pointer back into a CProgressMgr instance
	CGvisR2R_PunchView* pThread = reinterpret_cast<CGvisR2R_PunchView*>(lpContext);

	BOOL bLock = FALSE;
	DWORD dwTick = GetTickCount();
	DWORD dwShutdownEventCheckPeriod = 0; // thread shutdown event check period

	pThread->m_bThread[21] = TRUE;
	while (WAIT_OBJECT_0 != WaitForSingleObject(pThread->m_Thread[21].GetShutdownEvent(), dwShutdownEventCheckPeriod))
	{
		pThread->m_dwThreadTick[21] = GetTickCount() - dwTick;
		dwTick = GetTickCount();

		if (pThread->m_bTHREAD_RELOAD_RST_UP_INNER)
		{
			pThread->ReloadReelmapUpInner();
			pThread->m_bTHREAD_RELOAD_RST_UP_INNER = FALSE;
			Sleep(0);
		}
		else
			Sleep(30);
	}

	pThread->m_bThread[21] = FALSE;

	return 0;
}

UINT CGvisR2R_PunchView::ThreadProc22(LPVOID lpContext)	// ReloadReelmapDnInner()
{
	// Turn the passed in 'this' pointer back into a CProgressMgr instance
	CGvisR2R_PunchView* pThread = reinterpret_cast<CGvisR2R_PunchView*>(lpContext);

	BOOL bLock = FALSE;
	DWORD dwTick = GetTickCount();
	DWORD dwShutdownEventCheckPeriod = 0; // thread shutdown event check period

	pThread->m_bThread[22] = TRUE;
	while (WAIT_OBJECT_0 != WaitForSingleObject(pThread->m_Thread[22].GetShutdownEvent(), dwShutdownEventCheckPeriod))
	{
		pThread->m_dwThreadTick[22] = GetTickCount() - dwTick;
		dwTick = GetTickCount();

		if (pThread->m_bTHREAD_RELOAD_RST_DN_INNER)
		{
			pThread->ReloadReelmapDnInner();
			pThread->m_bTHREAD_RELOAD_RST_DN_INNER = FALSE;
			Sleep(0);
		}
		else
			Sleep(30);
	}

	pThread->m_bThread[22] = FALSE;

	return 0;
}

UINT CGvisR2R_PunchView::ThreadProc23(LPVOID lpContext)	// ReloadReelmapAllUpInner()
{
	// Turn the passed in 'this' pointer back into a CProgressMgr instance
	CGvisR2R_PunchView* pThread = reinterpret_cast<CGvisR2R_PunchView*>(lpContext);

	BOOL bLock = FALSE;
	DWORD dwTick = GetTickCount();
	DWORD dwShutdownEventCheckPeriod = 0; // thread shutdown event check period

	pThread->m_bThread[23] = TRUE;
	while (WAIT_OBJECT_0 != WaitForSingleObject(pThread->m_Thread[23].GetShutdownEvent(), dwShutdownEventCheckPeriod))
	{
		pThread->m_dwThreadTick[23] = GetTickCount() - dwTick;
		dwTick = GetTickCount();

		if (pThread->m_bTHREAD_RELOAD_RST_ALLUP_INNER)
		{
			pThread->ReloadReelmapAllUpInner();
			pThread->m_bTHREAD_RELOAD_RST_ALLUP_INNER = FALSE;
			Sleep(0);
		}
		else
			Sleep(30);
	}

	pThread->m_bThread[23] = FALSE;

	return 0;
}

UINT CGvisR2R_PunchView::ThreadProc24(LPVOID lpContext)	// ReloadReelmapAllDnInner()
{
	// Turn the passed in 'this' pointer back into a CProgressMgr instance
	CGvisR2R_PunchView* pThread = reinterpret_cast<CGvisR2R_PunchView*>(lpContext);

	BOOL bLock = FALSE;
	DWORD dwTick = GetTickCount();
	DWORD dwShutdownEventCheckPeriod = 0; // thread shutdown event check period

	pThread->m_bThread[24] = TRUE;
	while (WAIT_OBJECT_0 != WaitForSingleObject(pThread->m_Thread[24].GetShutdownEvent(), dwShutdownEventCheckPeriod))
	{
		pThread->m_dwThreadTick[24] = GetTickCount() - dwTick;
		dwTick = GetTickCount();

		if (pThread->m_bTHREAD_RELOAD_RST_ALLDN_INNER)
		{
			pThread->ReloadReelmapAllDnInner();
			pThread->m_bTHREAD_RELOAD_RST_ALLDN_INNER = FALSE;
			Sleep(0);
		}
		else
			Sleep(30);
	}

	pThread->m_bThread[24] = FALSE;

	return 0;
}

UINT CGvisR2R_PunchView::ThreadProc25(LPVOID lpContext)	// ReloadReelmapIts()
{
	// Turn the passed in 'this' pointer back into a CProgressMgr instance
	CGvisR2R_PunchView* pThread = reinterpret_cast<CGvisR2R_PunchView*>(lpContext);

	BOOL bLock = FALSE;
	DWORD dwTick = GetTickCount();
	DWORD dwShutdownEventCheckPeriod = 0; // thread shutdown event check period

	pThread->m_bThread[25] = TRUE;
	while (WAIT_OBJECT_0 != WaitForSingleObject(pThread->m_Thread[25].GetShutdownEvent(), dwShutdownEventCheckPeriod))
	{
		pThread->m_dwThreadTick[25] = GetTickCount() - dwTick;
		dwTick = GetTickCount();

		if (pThread->m_bTHREAD_RELOAD_RST_ITS)
		{
			pThread->ReloadReelmapIts();
			pThread->m_bTHREAD_RELOAD_RST_ITS = FALSE;
			Sleep(0);
		}
		else
			Sleep(30);
	}

	pThread->m_bThread[25] = FALSE;

	return 0;
}



BOOL CGvisR2R_PunchView::WriteReelmapIts()
{
	//return pDoc->WriteReelmapIts(m_nSerialRmapUpdate);
	BOOL bRtn = pDoc->m_pReelMapIts->WriteIts(m_nSerialRmapUpdate);

	if (pDoc->m_pReelMap == pDoc->m_pReelMapIts)
	{
		DuplicateRmap(RMAP_ITS);
	}

	return bRtn;
}


void CGvisR2R_PunchView::DoMark0Its()
{
#ifdef TEST_MODE
	return;
#endif

	if (!m_bAuto)
		return;

	int nSerial, nIdx, nErrCode, nRtn;
	CfPoint ptPnt;
	CString sMsg;
	double dStripOut = (pDoc->m_Master[0].m_pPcsRgn->nTotPcs / MAX_STRIP_NUM) * _tstof(pDoc->WorkingInfo.LastJob.sStripOutRatio) / 100.0;
	int nStripOut = int(dStripOut);
	if (dStripOut > nStripOut)
		nStripOut++;			// ��Ʈ�� ���� ����

	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;

	if (!IsRun())																		// �������¿���
	{
		if (IsOnMarking0())																// ��ŷ�߿�
		{
			if (m_pMotion->IsEnable(MS_X0) && m_pMotion->IsEnable(MS_Y0))				// ����� Enable�����̰�
			{
				if (m_pMotion->IsMotionDone(MS_X0) && m_pMotion->IsMotionDone(MS_Y0))	// ��� Done�����̸�,
				{
					if (!IsInitPos0() && !IsPinPos0())									// �ʱ���ġ�� �ƴϰų�, ����ġ�� �ƴҶ�
						MoveInitPos0();													// �ʱ���ġ�� �̵�
				}
			}

			if (m_nStepMk[0] < 13 && m_nStepMk[0] > 8) // Mk0();
			{
				m_nStepMk[0] = 8;	// ��ŷ�Ϸ�Check
			}
		}
		return;
	}

	m_sDispSts[0].Format(_T("%d"), m_nStepMk[0]);

	switch (m_nStepMk[0])
	{
	case 0:
		if (IsNoMk())
			ShowLive();
		m_nStepMk[0]++;
		break;
	case 1:
		if (!IsInitPos0())
			MoveInitPos0();
		m_nStepMk[0]++;
		break;
	case 2:

		nSerial = m_nBufUpSerial[0]; // Cam0

		if (nSerial > 0)
		{
			if ((nErrCode = GetErrCode0Its(nSerial)) != 1)
			{
				m_nMkPcs[0] = GetTotDefPcs0Its(nSerial);
				m_nStepMk[0] = MK_END;
			}
			else
			{
				m_nStepMk[0]++;
			}
		}
		else
		{
			StopFromThread();
			//AsyncMsgBox(_T("������ �ø����� �����ʽ��ϴ�."), 1);
			MsgBox(_T("������ �ø����� �����ʽ��ϴ�."), 1);
			BuzzerFromThread(TRUE, 0);
			DispMain(_T("�� ��"), RGB_RED);
		}
		break;
	case 3:
		m_nStepMk[0]++;
		break;
	case 4:
		m_nStepMk[0]++;
		break;
	case 5:
		m_nStepMk[0]++;
		break;
	case 6:
		nSerial = m_nBufUpSerial[0]; // Cam0

		if (m_nMkPcs[0] < GetTotDefPcs0Its(nSerial))
		{
			if (!IsNoMk0())
			{
				;
			}
			else
			{
				if (!IsReview0())
				{
					if (m_bReview)
					{
						m_nMkPcs[0] = GetTotDefPcs0Its(nSerial);
						m_nStepMk[0] = MK_END;
						break;
					}
				}
			}
			// Punching On�̰ų� Review�̸� �������� ����
			SetDelay0(100, 1);		// [mSec]
			m_nStepMk[0]++;
		}
		else
		{
			m_nStepMk[0] = MK_END;
		}
		break;
	case 7:
		nSerial = m_nBufUpSerial[0]; // Cam0

		if (!WaitDelay0(1))		// F:Done, T:On Waiting....		// Delay�Ŀ�
		{
			m_nMkPcs[0] = 0;

			if (!IsNoMk0())										// Punching On�̸�
			{
				m_nStepMk[0]++;
			}
			else												// Punching�� Off�̰�
			{
				if (IsReview0())								// Review�̸� ��������
				{
					m_nStepMk[0]++;
				}
				else											// Review�� �ƴϸ�
				{
					if (m_bReview)
					{
						m_nMkPcs[0] = GetTotDefPcs0Its(nSerial);
						m_nStepMk[0] = MK_END;
					}
					else
						m_nStepMk[0]++;
				}
			}
		}
		break;
	case 8:
		nSerial = m_nBufUpSerial[0]; // Cam0

		if (m_nMkPcs[0] < GetTotDefPcs0Its(nSerial))	// ��ŷ�Ϸ�Check
		{
			if (m_nMkPcs[0] + 1 < GetTotDefPcs0Its(nSerial))		// ���� ��ŷ��ġ�� ������
			{
				ptPnt = GetMkPnt0Its(nSerial, m_nMkPcs[0] + 1);	// ���� ��ŷ��ġ
				m_dNextTarget[AXIS_X0] = ptPnt.x;
				m_dNextTarget[AXIS_Y0] = ptPnt.y;
			}
			else												// ���� ��ŷ��ġ�� ������
			{
				m_dNextTarget[AXIS_X0] = -1.0;
				m_dNextTarget[AXIS_Y0] = -1.0;
			}

			ptPnt = GetMkPnt0Its(nSerial, m_nMkPcs[0]);			// �̹� ��ŷ��ġ
			if (ptPnt.x < 0.0 && ptPnt.y < 0.0) // ��ǰȭ. (��ŷ���� ����)
			{
				m_nMkPcs[0]++;
				m_nStepMk[0] = MK_DONE_CHECK;
				break;
			}

			nIdx = GetMkStripIdx0Its(nSerial, m_nMkPcs[0]);		// 1 ~ 4 : strip index
			if (nIdx > 0)										// Strip index�� �����̸�,
			{
				if (!IsMkStrip(nIdx)) // Strip[] Mk Off
				{
					m_nMkPcs[0]++;
					m_nStepMk[0] = MK_DONE_CHECK;
					break;
				}
				else
				{
					if (m_nMkStrip[0][nIdx - 1] >= nStripOut)
					{
						m_nMkPcs[0]++;
						m_nStepMk[0] = MK_DONE_CHECK;
						break;
					}
					else
						m_nMkStrip[0][nIdx - 1]++;
				}
			}
			else
			{
				pView->ClrDispMsg();
				AfxMessageBox(_T("Strip Index Failed."));
				break;
			}

			m_dTarget[AXIS_X0] = ptPnt.x;
			m_dTarget[AXIS_Y0] = ptPnt.y;

			m_nStepMk[0]++;
		}
		else
		{
			m_nStepMk[0] = MK_END;
		}
		break;
	case 9:
		// Cam0 : m_bPriority[0] <-, m_bPriority[3] ->
		// Cam1 : m_bPriority[1] ->, m_bPriority[2] <-
		ptPnt.x = m_dTarget[AXIS_X0];
		ptPnt.y = m_dTarget[AXIS_Y0];

		if (m_dNextTarget[AXIS_X0] < 0)
			m_bCollision[0] = ChkCollision(AXIS_X0, m_dTarget[AXIS_X0]);
		else
			m_bCollision[0] = ChkCollision(AXIS_X0, m_dTarget[AXIS_X0], m_dNextTarget[AXIS_X0]);

		if (!m_bCollision[0])
		{
			if (IsMoveDone0())
			{
				Move0(ptPnt, m_bCam);
				m_nStepMk[0]++;
			}
		}
		else if (m_bPriority[0])
		{
			if (IsMoveDone0())
			{
				m_bCollision[0] = FALSE;
				m_bPriority[0] = FALSE;
				Move0(ptPnt, m_bCam);
				m_nStepMk[0]++;
			}
		}
		else if (m_bPriority[2])
		{
			if (IsMoveDone0())
			{
				m_bCollision[0] = FALSE;
				m_bPriority[2] = FALSE;
				ptPnt.x = 0.0;
				ptPnt.y = 0.0;//m_dEnc[AXIS_Y0];
				Move0(ptPnt, FALSE);
			}
		}
		break;
	case 10:
		m_nStepMk[0]++;
		break;
	case 11:
		m_nStepMk[0]++;
		break;
	case 12:
		if (IsMoveDone0())
			m_nStepMk[0]++;
		break;
	case 13:
		if (!IsNoMk0())
		{
			m_dwStMkDn[0] = GetTickCount();
			Mk0();
		}
		else
		{
			// Verify - Mk0
			SetDelay0(pDoc->m_nDelayShow, 1);		// [mSec]
			if (!SaveMk0Img(m_nMkPcs[0]))
			{
				pView->ClrDispMsg();
				AfxMessageBox(_T("Error-SaveMk0Img()"));
			}
		}
		m_nStepMk[0]++;
		break;
	case 14:
		m_nStepMk[0]++;
		break;
	case 15:
		m_nStepMk[0]++;
		break;
	case 16:
		if (IsNoMk0())
		{
			if (!WaitDelay0(1))		// F:Done, T:On Waiting....
				m_nStepMk[0]++;
		}
		else
			m_nStepMk[0]++;
		break;
	case 17:
		if (!IsNoMk0())
		{
			if (IsMk0Done())
			{
				// One more MK On Start....
				if (!m_nMkPcs[0] && !m_bAnswer[2])
				{
					m_bAnswer[2] = TRUE;
					Mk0();
				}
				else
				{
					m_bAnswer[2] = FALSE;
					m_nMkPcs[0]++;
					m_nStepMk[0]++;
					m_nStepMk[0]++;
				}
			}
			else
			{
				if (m_dwStMkDn[0] + 5000 < GetTickCount())
				{
					BuzzerFromThread(TRUE, 0);
					DispMain(_T("�� ��"), RGB_RED);
					m_pVoiceCoil[0]->SearchHomeSmac0();

					//nRtn = AsyncMsgBox(_T("���̽�����(��) ��ſϷᰡ �ʵ˴ϴ�.\r\n��ŷ�� �ٽ� �õ��Ͻðڽ��ϱ�?"), 1, MB_YESNO);
					nRtn = MsgBox(_T("���̽�����(��) ��ſϷᰡ �ʵ˴ϴ�.\r\n��ŷ�� �ٽ� �õ��Ͻðڽ��ϱ�?"), 1, MB_YESNO);
					if (IDYES == nRtn)
					{
						DispMain(_T("������"), RGB_RED);
						m_nStepMk[0] = 13;
					}
					else if (nRtn < 0)
						m_nStepMk[0]++; // Wait...
					else
					{
						m_bAnswer[2] = FALSE;
						m_nMkPcs[0]++;
						m_nStepMk[0]++;
						m_nStepMk[0]++;
						Stop();
					}
				}
			}
		}
		else
		{
			m_nMkPcs[0]++;
			m_nStepMk[0]++;
			m_nStepMk[0]++;
		}
		break;
	case 18:
		if ((nRtn = WaitRtnVal(1)) > -1)
		{
			if (IDYES == nRtn)
			{
				DispMain(_T("������"), RGB_RED);
				m_nStepMk[0] = 13;
			}
			else
			{
				m_bAnswer[2] = FALSE;
				m_nMkPcs[0]++;
				m_nStepMk[0]++;
				StopFromThread();
			}
		}
		break;
	case 19:
		m_nStepMk[0] = MK_DONE_CHECK;
		break;
	case MK_DONE_CHECK:
		nSerial = m_nBufUpSerial[0]; // Cam0

		if (m_nMkPcs[0] < GetTotDefPcs0Its(nSerial))
		{
			if (IsNoMk0())
			{
				if (!pDoc->WorkingInfo.System.bNoMk)
				{
					if (IsReview0())
					{
						if (IsJogRtDn0())
							m_nStepMk[0]++;
					}
					else if (!pDoc->WorkingInfo.LastJob.bVerify)
					{
						m_nMkPcs[0] = GetTotDefPcs0Its(nSerial);
						m_nStepMk[0] = MK_END;
					}
					else
						m_nStepMk[0] = 8;	// ��ŷ�Ϸ�Check
				}
				else
					m_nStepMk[0] = 8;	// ��ŷ�Ϸ�Check
			}
			else
				m_nStepMk[0] = 8;	// ��ŷ�Ϸ�Check
		}
		else
		{
			if (IsNoMk0())
			{
				if (IsReview0())
				{
					if (IsJogRtDn0())
						m_nStepMk[0] = MK_END;
				}
				else
				{
					m_nStepMk[0] = MK_END;
				}
			}
			else
				m_nStepMk[0] = MK_END;
		}
		break;
	case MK_DONE_CHECK + 1:
		nSerial = m_nBufUpSerial[0]; // Cam0

		if (m_nMkPcs[0] < GetTotDefPcs0Its(nSerial))
		{
			if (IsNoMk0())
			{
				if (IsReview0())
				{
					if (IsJogRtUp0())
						m_nStepMk[0] = 8;	// ��ŷ�Ϸ�Check
				}
				else
				{
					m_nMkPcs[0] = GetTotDefPcs0Its(nSerial);
					m_nStepMk[0] = MK_END;
				}
			}
			else
				m_nStepMk[0] = 8;	// ��ŷ�Ϸ�Check
		}
		else
		{
			m_nStepMk[0] = MK_END;
		}
		break;

	case MK_END:
		if (IsMoveDone0())
			m_nStepMk[0]++;
		break;
	case 101:
		SetDelay0(100, 1);		// [mSec]
		m_nStepMk[0]++;
		break;
	case 102:
		if (!WaitDelay0(1))		// F:Done, T:On Waiting....
		{
			m_nStepMk[0]++;
		}
		break;
	case 103:
		if (!IsInitPos0())
		{
			m_dTarget[AXIS_X0] = _tstof(pDoc->WorkingInfo.Motion.sStPosX[0]);
			m_dTarget[AXIS_Y0] = _tstof(pDoc->WorkingInfo.Motion.sStPosY[0]);
			m_dNextTarget[AXIS_X0] = _tstof(pDoc->WorkingInfo.Motion.sStPosX[0]);
			m_dNextTarget[AXIS_Y0] = _tstof(pDoc->WorkingInfo.Motion.sStPosY[0]);

			MoveInitPos0();
		}

		pDoc->SaveMkCntL();
		m_nStepMk[0]++;
		break;
	case 104:
		if (IsMoveDone0())
		{
			m_nStepMk[0]++;
			SetDelay0(10000, 1);		// [mSec]
		}
		break;
	case 105:
		if (IsInitPos0())
		{
			m_nStepMk[0]++;
		}
		else
		{
			if (!WaitDelay0(1))		// F:Done, T:On Waiting....
			{
				m_nStepMk[0] = ERR_PROC;
			}
		}
		break;
	case 106: // MK Done....
		m_bDoneMk[0] = TRUE;
		m_bTHREAD_MK[0] = FALSE;
		break;

	case ERR_PROC:
		DispMain(_T("�� ��"), RGB_RED);
		m_pVoiceCoil[0]->SearchHomeSmac0();
		//AsyncMsgBox(_T("���̽�����(��) �ʱ���ġ �̵��� ���� �ʽ��ϴ�.\r\n��ŷ���¸� Ȯ���ϼ���."), 1);
		MsgBox(_T("���̽�����(��) �ʱ���ġ �̵��� ���� �ʽ��ϴ�.\r\n��ŷ���¸� Ȯ���ϼ���."), 1);
		m_nStepMk[0]++;
		break;
	case ERR_PROC + 1:
		nSerial = m_nBufUpSerial[0];

		if (m_bCam)
			sMsg.Format(_T("%d�� Shot�� �ٽ� �ҷ�Ȯ���� �Ͻðڽ��ϱ�?"), nSerial);
		else
			sMsg.Format(_T("%d�� Shot�� �ٽ� ��ŷ�Ͻðڽ��ϱ�?"), nSerial);

		m_nRtnMyMsgBoxIdx = 0;
		m_bRtnMyMsgBox[0] = FALSE;
		m_nRtnMyMsgBox[0] = -1;
		//pView->AsyncMsgBox(sMsg, 1, MB_YESNO);
		pView->MsgBox(sMsg, 1, MB_YESNO);
		sMsg.Empty();
		m_nStepMk[0]++;
		break;
	case ERR_PROC + 2:
		if ((nRtn = WaitRtnVal(1)) > -1)
		{
			if (IDYES == nRtn)
			{
				m_nStepMk[0] = ERR_PROC + 10;
			}
			else
			{
				m_nRtnMyMsgBoxIdx = 0;
				m_bRtnMyMsgBox[0] = FALSE;
				m_nRtnMyMsgBox[0] = -1;
				sMsg.Format(_T("��� ���� �۾��� �����Ͻðڽ��ϱ�?"), nSerial);
				//pView->AsyncMsgBox(sMsg, 1, MB_YESNO);
				pView->MsgBox(sMsg, 1, MB_YESNO);
				sMsg.Empty();

				m_nStepMk[0]++;
			}
		}
		break;
	case ERR_PROC + 3:
		if ((nRtn = WaitRtnVal(1)) > -1)
		{
			if (IDYES == nRtn)
			{
				m_nStepMk[0] = ERR_PROC + 20;
			}
			else
			{
				m_bDispMsgDoAuto[8] = TRUE;
				m_nStepDispMsg[8] = FROM_DOMARK0;
			}
		}
		break;
	case ERR_PROC + 10:
		m_bReMark[0] = TRUE;
		m_bTHREAD_MK[0] = FALSE;
		m_nStepMk[0] = 0;
		break;
	case ERR_PROC + 20: // MK Done....
		m_bDoneMk[0] = TRUE;
		m_bTHREAD_MK[0] = FALSE;
		break;
	}
}


void CGvisR2R_PunchView::DoMark1Its()
{
#ifdef TEST_MODE
	return;
#endif

	if (!m_bAuto)
		return;

	//BOOL bOn;
	int nSerial, nIdx, nErrCode, nRtn;
	CfPoint ptPnt;
	CString sMsg;
	double dStripOut = (pDoc->m_Master[0].m_pPcsRgn->nTotPcs / MAX_STRIP_NUM) * _tstof(pDoc->WorkingInfo.LastJob.sStripOutRatio) / 100.0;
	int nStripOut = int(dStripOut);
	if (dStripOut > nStripOut)
		nStripOut++;

	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;


	if (!IsRun())
	{
		if (IsOnMarking1())
		{
			if (m_pMotion->IsEnable(MS_X1) && m_pMotion->IsEnable(MS_Y1))
			{
				if (m_pMotion->IsMotionDone(MS_X1) && m_pMotion->IsMotionDone(MS_Y1))
				{
					if (!IsMkEdPos1() && !IsPinPos1())
						MoveMkEdPos1();
				}
			}

			if (m_nStepMk[1] < 13 && m_nStepMk[1] > 8) // Mk1();
			{
				m_nStepMk[1] = 8;	// ��ŷ�Ϸ�Check
			}
		}
		return;
	}


	m_sDispSts[1].Format(_T("%d"), m_nStepMk[1]);

	switch (m_nStepMk[1])
	{
	case 0:
		if (IsNoMk())
			ShowLive();
		m_nStepMk[1]++;
		break;
	case 1:
		if (!IsInitPos1())
			MoveInitPos1();
		m_nStepMk[1]++;
		break;
	case 2:

		nSerial = m_nBufUpSerial[1]; // Cam1

		if (nSerial > 0)
		{
			if ((nErrCode = GetErrCode1Its(nSerial)) != 1)
			{
				m_nMkPcs[1] = GetTotDefPcs1Its(nSerial);
				m_nStepMk[1] = MK_END;
			}
			else
			{
				m_nStepMk[1]++;
			}
		}
		else
		{
			StopFromThread();
			//AsyncMsgBox(_T("������ �ø����� �����ʽ��ϴ�."), 2);
			MsgBox(_T("������ �ø����� �����ʽ��ϴ�."), 2);
			BuzzerFromThread(TRUE, 0);
			//pView->DispStsBar(_T("����-34"), 0);
			DispMain(_T("�� ��"), RGB_RED);
		}
		break;
	case 3:
		m_nStepMk[1]++;
		break;
	case 4:
		m_nStepMk[1]++;
		break;
	case 5:
		m_nStepMk[1]++;
		break;
	case 6:

		nSerial = m_nBufUpSerial[1]; // Cam1

		if (m_nMkPcs[1] < GetTotDefPcs1Its(nSerial))
		{
			if (!IsNoMk1())
			{
				;
			}
			else
			{
				if (!IsReview1())
				{
					if (m_bReview)
					{
						m_nMkPcs[1] = GetTotDefPcs1Its(nSerial);
						m_nStepMk[1] = MK_END;
						break;
					}
				}
			}
			SetDelay1(100, 6);		// [mSec]
			m_nStepMk[1]++;
		}
		else
		{
			m_nStepMk[1] = MK_END;
		}
		break;
	case 7:
		nSerial = m_nBufUpSerial[1]; // Cam1

		if (!WaitDelay1(6))		// F:Done, T:On Waiting....
		{
			m_nMkPcs[1] = 0;

			if (!IsNoMk1())
			{
				m_nStepMk[1]++;
			}
			else
			{
				if (IsReview1())
				{
					m_nStepMk[1]++;
				}
				else
				{
					if (m_bReview)
					{
						m_nMkPcs[1] = GetTotDefPcs1Its(nSerial);
						m_nStepMk[1] = MK_END;
					}
					else
						m_nStepMk[1]++;
				}
			}
		}
		break;
	case 8:
		nSerial = m_nBufUpSerial[1]; // Cam1

		if (m_nMkPcs[1] < GetTotDefPcs1Its(nSerial))	// ��ŷ�Ϸ�Check
		{
			if (m_nMkPcs[1] + 1 < GetTotDefPcs1Its(nSerial))
			{
				ptPnt = GetMkPnt1Its(nSerial, m_nMkPcs[1] + 1);
				m_dNextTarget[AXIS_X1] = ptPnt.x;
				m_dNextTarget[AXIS_Y1] = ptPnt.y;
			}
			else
			{
				m_dNextTarget[AXIS_X1] = -1.0;
				m_dNextTarget[AXIS_Y1] = -1.0;
			}

			ptPnt = GetMkPnt1Its(nSerial, m_nMkPcs[1]);
			if (ptPnt.x < 0.0 && ptPnt.y < 0.0) // ��ǰȭ.
			{
				m_nMkPcs[1]++;
				m_nStepMk[1] = MK_DONE_CHECK;
				break;
			}

			nIdx = GetMkStripIdx1Its(nSerial, m_nMkPcs[1]);
			if (nIdx > 0)
			{
				if (!IsMkStrip(nIdx)) // Strip[] Mk Off
				{
					m_nMkPcs[1]++;
					m_nStepMk[1] = MK_DONE_CHECK;
					break;
				}
				else
				{
					if (m_nMkStrip[1][nIdx - 1] >= nStripOut)
					{
						m_nMkPcs[1]++;
						m_nStepMk[1] = MK_DONE_CHECK;
						break;
					}
					else
						m_nMkStrip[1][nIdx - 1]++;
				}
			}
			else
			{
				pView->ClrDispMsg();
				AfxMessageBox(_T("Strip Index Failed."));
				break;
			}

			m_dTarget[AXIS_X1] = ptPnt.x;
			m_dTarget[AXIS_Y1] = ptPnt.y;

			m_nStepMk[1]++;
		}
		else
		{
			m_nStepMk[1] = MK_END;
		}
		break;
	case 9:
		// Cam0 : m_bPriority[0] <-, m_bPriority[3] ->
		// Cam1 : m_bPriority[1] ->, m_bPriority[2] <-
		ptPnt.x = m_dTarget[AXIS_X1];
		ptPnt.y = m_dTarget[AXIS_Y1];

		if (m_dNextTarget[AXIS_X1] < 0)
			m_bCollision[1] = ChkCollision(AXIS_X1, m_dTarget[AXIS_X1]);
		else
			m_bCollision[1] = ChkCollision(AXIS_X1, m_dTarget[AXIS_X1], m_dNextTarget[AXIS_X1]);

		if (!m_bCollision[1])
		{
			if (IsMoveDone1())
			{
				Move1(ptPnt, m_bCam);
				m_nStepMk[1]++;
			}
		}
		else if (m_bPriority[1])
		{
			if (IsMoveDone1())
			{
				m_bCollision[1] = FALSE;
				m_bPriority[1] = FALSE;
				Move1(ptPnt, m_bCam);
				m_nStepMk[1]++;
			}
		}
		else if (m_bPriority[3])
		{
			if (IsMoveDone1())
			{
				m_bCollision[1] = FALSE;
				m_bPriority[3] = FALSE;
				ptPnt.x = _tstof(pDoc->WorkingInfo.Motion.sSafeZone);
				ptPnt.y = 0.0;//m_dEnc[AXIS_Y1];
				Move1(ptPnt, FALSE);
			}
		}
		break;
	case 10:
		m_nStepMk[1]++;
		break;
	case 11:
		m_nStepMk[1]++;
		break;
	case 12:
		if (IsMoveDone1())
			m_nStepMk[1]++;
		break;
	case 13:
		if (!IsNoMk1())
		{
			m_dwStMkDn[1] = GetTickCount();
			Mk1();
		}
		else
		{
			// Verify - Mk1
			SetDelay1(pDoc->m_nDelayShow, 6);		// [mSec]
			if (!SaveMk1Img(m_nMkPcs[1]))
			{
				pView->ClrDispMsg();
				AfxMessageBox(_T("Error-SaveMk1Img()"));
			}
		}
		m_nStepMk[1]++;
		break;
	case 14:
		m_nStepMk[1]++;
		break;
	case 15:
		m_nStepMk[1]++;
		break;
	case 16:
		if (IsNoMk1())
		{
			if (!WaitDelay1(6))		// F:Done, T:On Waiting....
				m_nStepMk[1]++;
		}
		else
			m_nStepMk[1]++;
		break;
	case 17:
		if (!IsNoMk1())
		{
			if (IsMk1Done())
			{
				// One more MK On Start....
				if (!m_nMkPcs[1] && !m_bAnswer[3])
				{
					m_bAnswer[3] = TRUE;
					Mk1();
				}
				else
				{
					m_bAnswer[3] = FALSE;
					m_nMkPcs[1]++;
					m_nStepMk[1]++;
					m_nStepMk[1]++;
				}

			}
			else
			{
				if (m_dwStMkDn[1] + 5000 < GetTickCount())
				{
					BuzzerFromThread(TRUE, 0);
					DispMain(_T("�� ��"), RGB_RED);
					m_pVoiceCoil[1]->SearchHomeSmac1();

					//nRtn = AsyncMsgBox(_T("���̽�����(��) ��ſϷᰡ �ʵ˴ϴ�.\r\n��ŷ�� �ٽ� �õ��Ͻðڽ��ϱ�?"), 2, MB_YESNO);
					nRtn = MsgBox(_T("���̽�����(��) ��ſϷᰡ �ʵ˴ϴ�.\r\n��ŷ�� �ٽ� �õ��Ͻðڽ��ϱ�?"), 2, MB_YESNO);
					if (IDYES == nRtn)
					{
						DispMain(_T("������"), RGB_RED);
						m_nStepMk[1] = 13;
					}
					else if (nRtn < 0)
						m_nStepMk[1]++; // Wait...
					else
					{
						m_bAnswer[3] = FALSE;
						m_nMkPcs[1]++;
						m_nStepMk[1]++;
						m_nStepMk[1]++;
						Stop();
					}
				}
			}
		}
		else
		{
			m_nMkPcs[1]++;
			m_nStepMk[1]++;
			m_nStepMk[1]++;
		}
		break;
	case 18:
		if ((nRtn = WaitRtnVal(2)) > -1)
		{
			if (IDYES == nRtn)
			{
				DispMain(_T("������"), RGB_RED);
				m_nStepMk[1] = 13;
			}
			else
			{
				m_bAnswer[3] = FALSE;
				m_nMkPcs[1]++;
				m_nStepMk[1]++;
				StopFromThread();
			}
		}
		break;
	case 19:
		m_nStepMk[1] = MK_DONE_CHECK;
		break;
	case MK_DONE_CHECK:
		nSerial = m_nBufUpSerial[1]; // Cam1

		if (m_nMkPcs[1] < GetTotDefPcs1Its(nSerial))
		{
			if (IsNoMk1())
			{
				if (!pDoc->WorkingInfo.System.bNoMk)
				{
					if (IsReview1())
					{
						if (IsJogRtDn1())
							m_nStepMk[1]++;
					}
					else if (!pDoc->WorkingInfo.LastJob.bVerify)
					{
						m_nMkPcs[1] = GetTotDefPcs1Its(nSerial);
						m_nStepMk[1] = MK_END;
					}
					else
						m_nStepMk[1] = 8;	// ��ŷ�Ϸ�Check
				}
				else
					m_nStepMk[1] = 8;	// ��ŷ�Ϸ�Check
			}
			else
				m_nStepMk[1] = 8;	// ��ŷ�Ϸ�Check
		}
		else
		{
			if (IsNoMk1())
			{
				if (IsReview1())
				{
					if (IsJogRtDn1())
						m_nStepMk[1] = MK_END;
				}
				else
				{
					m_nStepMk[1] = MK_END;
				}
			}
			else
				m_nStepMk[1] = MK_END;
		}
		break;
	case MK_DONE_CHECK + 1:
		nSerial = m_nBufUpSerial[1]; // Cam1

		if (m_nMkPcs[1] < GetTotDefPcs1Its(nSerial))
		{
			if (IsNoMk1())
			{
				if (IsReview1())
				{
					if (IsJogRtUp1())
						m_nStepMk[1] = 8;	// ��ŷ�Ϸ�Check
				}
				else
				{
					m_nMkPcs[1] = GetTotDefPcs1Its(nSerial);
					m_nStepMk[1] = MK_END;
				}
			}
			else
				m_nStepMk[1] = 8;	// ��ŷ�Ϸ�Check
		}
		else
		{
			m_nStepMk[1] = MK_END;
		}
		break;


	case MK_END:
		if (IsMoveDone1())
			m_nStepMk[1]++;
		break;
	case 101:
		SetDelay1(100, 6);		// [mSec]
		m_nStepMk[1]++;
		break;
	case 102:
		if (!WaitDelay1(6))		// F:Done, T:On Waiting....
			m_nStepMk[1]++;
		break;
	case 103:
		if (!IsMkEdPos1())
		{
			m_dTarget[AXIS_X1] = _tstof(pDoc->WorkingInfo.Motion.sStPosX[1]);
			m_dTarget[AXIS_Y1] = _tstof(pDoc->WorkingInfo.Motion.sStPosY[1]);
			m_dNextTarget[AXIS_X1] = _tstof(pDoc->WorkingInfo.Motion.sStPosX[1]);
			m_dNextTarget[AXIS_Y1] = _tstof(pDoc->WorkingInfo.Motion.sStPosY[1]);

			MoveMkEdPos1();
		}

		pDoc->SaveMkCntR();
		m_nStepMk[1]++;
		break;
	case 104:
		if (IsMoveDone1())
		{
			m_nStepMk[1]++;
			SetDelay1(10000, 6);		// [mSec]
		}
		break;
	case 105:
		if (IsMkEdPos1())
		{
			m_nStepMk[1]++;
		}
		else
		{
			if (!WaitDelay1(6))		// F:Done, T:On Waiting....
			{
				m_nStepMk[1] = ERR_PROC;
			}
		}
		break;
	case 106: // MK Done....
		m_bDoneMk[1] = TRUE;
		m_bTHREAD_MK[1] = FALSE;
		break;

	case ERR_PROC:
		DispMain(_T("�� ��"), RGB_RED);
		m_pVoiceCoil[1]->SearchHomeSmac1();
		//AsyncMsgBox(_T("���̽�����(��) �ʱ���ġ �̵��� ���� �ʽ��ϴ�.\r\n��ŷ���¸� Ȯ���ϼ���."), 2);
		MsgBox(_T("���̽�����(��) �ʱ���ġ �̵��� ���� �ʽ��ϴ�.\r\n��ŷ���¸� Ȯ���ϼ���."), 2);
		m_nStepMk[1]++;
		break;
	case ERR_PROC + 1:
		nSerial = m_nBufUpSerial[1]; // Cam1

		if (m_bCam)
			sMsg.Format(_T("%d�� Shot�� �ٽ� �ҷ�Ȯ���� �Ͻðڽ��ϱ�?"), nSerial);
		else
			sMsg.Format(_T("%d�� Shot�� �ٽ� ��ŷ�Ͻðڽ��ϱ�?"), nSerial);

		m_nRtnMyMsgBoxIdx = 1;
		m_bRtnMyMsgBox[1] = FALSE;
		m_nRtnMyMsgBox[1] = -1;
		//pView->AsyncMsgBox(sMsg, 2, MB_YESNO);
		pView->MsgBox(sMsg, 2, MB_YESNO);
		sMsg.Empty();
		m_nStepMk[1]++;
		break;
	case ERR_PROC + 2:
		if ((nRtn = WaitRtnVal(2)) > -1)
		{
			if (IDYES == nRtn)
			{
				m_nStepMk[1] = ERR_PROC + 10;
			}
			else
			{
				m_nRtnMyMsgBoxIdx = 1;
				m_bRtnMyMsgBox[1] = FALSE;
				m_nRtnMyMsgBox[1] = -1;
				sMsg.Format(_T("��� ���� �۾��� �����Ͻðڽ��ϱ�?"), nSerial);
				//pView->AsyncMsgBox(sMsg, 2, MB_YESNO);
				pView->MsgBox(sMsg, 2, MB_YESNO);
				sMsg.Empty();

				m_nStepMk[1]++;

			}
		}
		break;
	case ERR_PROC + 3:
		if ((nRtn = WaitRtnVal(2)) > -1)
		{
			if (IDYES == nRtn)
			{
				m_nStepMk[1] = ERR_PROC + 20;
			}
			else
			{
				m_bDispMsgDoAuto[9] = TRUE;
				m_nStepDispMsg[9] = FROM_DOMARK1;
			}
		}
		break;
	case ERR_PROC + 10:
		m_bReMark[1] = TRUE;
		m_bTHREAD_MK[1] = FALSE;
		m_nStepMk[1] = 0;
		break;
	case ERR_PROC + 20: // MK Done....
		m_bDoneMk[1] = TRUE;
		m_bTHREAD_MK[1] = FALSE;
		break;
	}
}


int CGvisR2R_PunchView::GetErrCode0Its(int nSerial) // 1(����), -1(Align Error, �뱤�ҷ�), -2(Lot End)
{
	if (nSerial <= 0)
	{
		pView->ClrDispMsg();
		AfxMessageBox(_T("Serial Error.32"));
		return 0;
	}

	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;

#ifdef USE_CAM_MASTER
	int nErr[2];
	nErr[0] = GetErrCodeUp0Its(nSerial);
	if (nErr[0] != 1)
		return nErr[0];
#endif

	return 1;
}

int CGvisR2R_PunchView::GetErrCodeUp0Its(int nSerial) // 1(����), -1(Align Error, �뱤�ҷ�), -2(Lot End)
{
	if (nSerial <= 0)
	{
		pView->ClrDispMsg();
		AfxMessageBox(_T("Serial Error.33"));
		return 0;
	}

	int nIdx = pDoc->GetPcrIdx0(nSerial);
	int nErrCode = 1;

#ifdef USE_CAM_MASTER
	if (pDoc->m_pPcrIts)
	{
		if (pDoc->m_pPcrIts[nIdx])
			nErrCode = pDoc->m_pPcrIts[nIdx]->m_nErrPnl;
		else
			return 2;	// Code Setting Error.
	}
	else
		return 2;	// Code Setting Error.
#endif

	return nErrCode;
}

int CGvisR2R_PunchView::GetErrCodeDn0Its(int nSerial) // 1(����), -1(Align Error, �뱤�ҷ�), -2(Lot End)
{
	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;
	if (!bDualTest)
		return 1;

	if (nSerial <= 0)
	{
		pView->ClrDispMsg();
		AfxMessageBox(_T("Serial Error.34"));
		return 0;
	}

	int nIdx = pDoc->GetPcrIdx0(nSerial);
	int nErrCode = 1;

#ifdef USE_CAM_MASTER
	if (pDoc->m_pPcrIts)
	{
		if (pDoc->m_pPcrIts[nIdx])
			nErrCode = pDoc->m_pPcrIts[nIdx]->m_nErrPnl;
	}
#endif

	return nErrCode;
}


int CGvisR2R_PunchView::GetErrCode1Its(int nSerial) // 1(����), -1(Align Error, �뱤�ҷ�), -2(Lot End)
{
	if (nSerial <= 0)
	{
		pView->ClrDispMsg();
		AfxMessageBox(_T("Serial Error.35"));
		return 0;
	}

#ifdef USE_CAM_MASTER
	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;

	int nErr[2];
	nErr[0] = GetErrCodeUp1Its(nSerial);
	if (nErr[0] != 1)
		return nErr[0];
#endif

	return 1;
}

int CGvisR2R_PunchView::GetErrCodeUp1Its(int nSerial) // 1(����), -1(Align Error, �뱤�ҷ�), -2(Lot End)
{
	if (nSerial <= 0)
	{
		pView->ClrDispMsg();
		AfxMessageBox(_T("Serial Error.36"));
		return 0;
	}

	int nIdx = pDoc->GetPcrIdx1(nSerial);
	int nErrCode = 1;

#ifdef USE_CAM_MASTER
	if (pDoc->m_pPcrIts)
	{
		if (pDoc->m_pPcrIts[nIdx])
			nErrCode = pDoc->m_pPcrIts[nIdx]->m_nErrPnl;
		else
			return 2;	// Code Setting Error.
	}
	else
		return 2;	// Code Setting Error.
#endif

	return nErrCode;
}

int CGvisR2R_PunchView::GetErrCodeDn1Its(int nSerial) // 1(����), -1(Align Error, �뱤�ҷ�), -2(Lot End)
{
	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;
	if (!bDualTest)
		return 1;

	if (nSerial <= 0)
	{
		pView->ClrDispMsg();
		AfxMessageBox(_T("Serial Error.37"));
		return 0;
	}

	int nIdx = pDoc->GetPcrIdx1(nSerial);
	int nErrCode = 1;

#ifdef USE_CAM_MASTER
	if (pDoc->m_pPcrIts)
	{
		if (pDoc->m_pPcrIts[nIdx])
			nErrCode = pDoc->m_pPcrIts[nIdx]->m_nErrPnl;
		else
			return 2;	// Code Setting Error.
	}
	else
		return 2;	// Code Setting Error.
#endif

	return nErrCode;
}


int CGvisR2R_PunchView::GetTotDefPcs0Its(int nSerial)
{
	if (nSerial <= 0)
	{
		pView->ClrDispMsg();
		AfxMessageBox(_T("Serial Error.41"));
		return 0;
	}

	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;

	int nIdx = pDoc->GetPcrIdx0(nSerial);
	int nTotDef = 0;

#ifdef USE_CAM_MASTER
	if (bDualTest)
	{
		if (pDoc->m_pPcrIts)
		{
			if (pDoc->m_pPcrIts[nIdx])
				nTotDef = pDoc->m_pPcrIts[nIdx]->m_nTotDef;
		}
	}
	else
	{
		if (pDoc->m_pPcrIts)
		{
			if (pDoc->m_pPcrIts[nIdx])
				nTotDef = pDoc->m_pPcrIts[nIdx]->m_nTotDef;
		}
	}
#else
	nTotDef = 1;
#endif

	return nTotDef;
}

int CGvisR2R_PunchView::GetTotDefPcsUp0Its(int nSerial)
{
	if (nSerial <= 0)
	{
		pView->ClrDispMsg();
		AfxMessageBox(_T("Serial Error.42"));
		return 0;
	}

	int nIdx = pDoc->GetPcrIdx0(nSerial);
	int nTotDef = 0;

#ifdef USE_CAM_MASTER
	if (pDoc->m_pPcrIts)
	{
		if (pDoc->m_pPcrIts[nIdx])
			nTotDef = pDoc->m_pPcrIts[nIdx]->m_nTotDef;
	}
#else
	nTotDef = 1;
#endif

	return nTotDef;
}

int CGvisR2R_PunchView::GetTotDefPcsDn0Its(int nSerial)
{
	if (nSerial <= 0)
	{
		pView->ClrDispMsg();
		AfxMessageBox(_T("Serial Error.43"));
		return 0;
	}

	int nIdx = pDoc->GetPcrIdx0(nSerial);
	int nTotDef = 0;

#ifdef USE_CAM_MASTER
	if (pDoc->m_pPcrIts)
	{
		if (pDoc->m_pPcrIts[nIdx])
			nTotDef = pDoc->m_pPcrIts[nIdx]->m_nTotDef;
	}
#else
	nTotDef = 1;
#endif

	return nTotDef;
}


int CGvisR2R_PunchView::GetTotDefPcs1Its(int nSerial)
{
	if (nSerial <= 0)
	{
		pView->ClrDispMsg();
		AfxMessageBox(_T("Serial Error.44"));
		return 0;
	}

	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;
	int nIdx = pDoc->GetPcrIdx1(nSerial);
	int nTotDef = 0;

#ifdef USE_CAM_MASTER
	if (bDualTest)
	{
		if (pDoc->m_pPcrIts)
		{
			if (pDoc->m_pPcrIts[nIdx])
				nTotDef = pDoc->m_pPcrIts[nIdx]->m_nTotDef;
		}
	}
	else
	{
		if (pDoc->m_pPcrIts)
		{
			if (pDoc->m_pPcrIts[nIdx])
				nTotDef = pDoc->m_pPcrIts[nIdx]->m_nTotDef;
		}
	}
#else
	nTotDef = 1;
#endif

	return nTotDef;
}

int CGvisR2R_PunchView::GetTotDefPcsUp1Its(int nSerial)
{
	if (nSerial <= 0)
	{
		pView->ClrDispMsg();
		AfxMessageBox(_T("Serial Error.45"));
		return 0;
	}

	int nIdx = pDoc->GetPcrIdx1(nSerial);
	int nTotDef = 0;

#ifdef USE_CAM_MASTER
	if (pDoc->m_pPcrIts)
	{
		if (pDoc->m_pPcrIts[nIdx])
			nTotDef = pDoc->m_pPcrIts[nIdx]->m_nTotDef;
	}
#else
	nTotDef = 1;
#endif

	return nTotDef;
}

int CGvisR2R_PunchView::GetTotDefPcsDn1Its(int nSerial)
{
	if (nSerial <= 0)
	{
		pView->ClrDispMsg();
		AfxMessageBox(_T("Serial Error.46"));
		return 0;
	}

	int nIdx = pDoc->GetPcrIdx1(nSerial);
	int nTotDef = 0;

#ifdef USE_CAM_MASTER
	if (pDoc->m_pPcrIts)
	{
		if (pDoc->m_pPcrIts[nIdx])
			nTotDef = pDoc->m_pPcrIts[nIdx]->m_nTotDef;
	}
#else
	nTotDef = 1;
#endif

	return nTotDef;
}


CfPoint CGvisR2R_PunchView::GetMkPnt0Its(int nSerial, int nMkPcs) // pcr �ø���, pcr �ҷ� �ǽ� ���� ���� �ε���
{
	if (nSerial <= 0)
	{
		pView->ClrDispMsg();
		AfxMessageBox(_T("Serial Error.47"));
		return 0;
	}

	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;
	int nIdx = pDoc->GetPcrIdx0(nSerial);
	CfPoint ptPnt;
	ptPnt.x = -1.0;
	ptPnt.y = -1.0;

#ifdef USE_CAM_MASTER
	int nDefPcsId = 0;

	if (bDualTest)
	{
		if (pDoc->m_pPcrIts)	// [0]:AOI-Up , [1]:AOI-Dn , [2]:AOI-AllUp , [3]:AOI-AllDn
		{
			if (pDoc->m_pPcrIts[nIdx])
			{
				if (pDoc->m_pPcrIts[nIdx]->m_pDefPcs)
				{
					if (pDoc->m_pPcrIts[nIdx]->m_pMk[nMkPcs] != -2) // -2 (NoMarking)
					{
						nDefPcsId = pDoc->m_pPcrIts[nIdx]->m_pDefPcs[nMkPcs];
						if (pDoc->m_Master[0].m_pPcsRgn)
							ptPnt = pDoc->m_Master[0].m_pPcsRgn->GetMkPnt0(nDefPcsId); // Cam0�� Mk ����Ʈ.
					}
				}
			}
		}
	}
	else
	{
		if (pDoc->m_pPcrIts)	// [0]:AOI-Up , [1]:AOI-Dn , [2]:AOI-AllUp , [3]:AOI-AllDn
		{
			if (pDoc->m_pPcrIts[nIdx])
			{
				if (pDoc->m_pPcrIts[nIdx]->m_pDefPcs)
				{
					if (pDoc->m_pPcrIts[nIdx]->m_pMk[nMkPcs] != -2) // -2 (NoMarking)
					{
						nDefPcsId = pDoc->m_pPcrIts[nIdx]->m_pDefPcs[nMkPcs];
						if (pDoc->m_Master[0].m_pPcsRgn)
							ptPnt = pDoc->m_Master[0].m_pPcsRgn->GetMkPnt0(nDefPcsId); // Cam0�� Mk ����Ʈ.
					}
				}
			}
		}
	}
#else
	ptPnt.x = 1.0;
	ptPnt.y = 1.0;
#endif

	return ptPnt;
}

CfPoint CGvisR2R_PunchView::GetMkPnt1Its(int nSerial, int nMkPcs)
{
	if (nSerial <= 0)
	{
		pView->ClrDispMsg();
		AfxMessageBox(_T("Serial Error.50"));
		return 0;
	}

	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;
	int nIdx = pDoc->GetPcrIdx1(nSerial);
	CfPoint ptPnt;
	ptPnt.x = -1.0;
	ptPnt.y = -1.0;

#ifdef USE_CAM_MASTER
	int nDefPcsId = 0;
	if (bDualTest)
	{
		if (pDoc->m_pPcrIts)	// [0]:AOI-Up , [1]:AOI-Dn , [2]:AOI-AllUp , [3]:AOI-AllDn
		{
			if (pDoc->m_pPcrIts[nIdx])
			{
				if (pDoc->m_pPcrIts[nIdx]->m_pDefPcs)
				{
					if (pDoc->m_pPcrIts[nIdx]->m_pMk[nMkPcs] != -2) // -2 (NoMarking)
					{
						nDefPcsId = pDoc->m_pPcrIts[nIdx]->m_pDefPcs[nMkPcs];
						if (pDoc->m_Master[0].m_pPcsRgn)
						{
							ptPnt = pDoc->m_Master[0].m_pPcsRgn->GetMkPnt1(nDefPcsId); // Cam1�� Mk ����Ʈ.
						}
					}
				}
			}
		}
	}
	else
	{
		if (pDoc->m_pPcrIts)	// [0]:AOI-Up , [1]:AOI-Dn , [2]:AOI-AllUp , [3]:AOI-AllDn
		{
			if (pDoc->m_pPcrIts[nIdx])
			{
				if (pDoc->m_pPcrIts[nIdx]->m_pDefPcs)
				{
					if (pDoc->m_pPcrIts[nIdx]->m_pMk[nMkPcs] != -2) // -2 (NoMarking)
					{
						nDefPcsId = pDoc->m_pPcrIts[nIdx]->m_pDefPcs[nMkPcs];
						if (pDoc->m_Master[0].m_pPcsRgn)
						{
							ptPnt = pDoc->m_Master[0].m_pPcsRgn->GetMkPnt1(nDefPcsId); // Cam1�� Mk ����Ʈ.
						}
					}
				}
			}
		}
	}
#else
	ptPnt.x = 1.0;
	ptPnt.y = 1.0;
#endif

	return ptPnt;
}


int CGvisR2R_PunchView::GetMkStripIdx0Its(int nSerial, int nMkPcs) // 0 : Fail , 1~4 : Strip Idx
{
	if (nSerial <= 0)
	{
		AfxMessageBox(_T("Serial Error.48"));
		return 0;
	}

	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;
	int nIdx = pDoc->GetPcrIdx0(nSerial);
	int nNodeX = pDoc->m_Master[0].m_pPcsRgn->nCol;
	int nNodeY = pDoc->m_Master[0].m_pPcsRgn->nRow;
	int nStripY = int(nNodeY / MAX_STRIP_NUM);
	int nStripIdx = 0;

#ifdef USE_CAM_MASTER
	int nDefPcsId = 0, nNum = 0, nMode = 0, nRow = 0;

	if (bDualTest)
	{
		if (pDoc->m_pPcrIts)	// [0]:AOI-Up , [1]:AOI-Dn , [2]:AOI-AllUp , [3]:AOI-AllDn
		{
			if (pDoc->m_pPcrIts[nIdx])
			{
				if (pDoc->m_pPcrIts[nIdx]->m_pDefPcs)
				{
					nDefPcsId = pDoc->m_pPcrIts[nIdx]->m_pDefPcs[nMkPcs];
					nNum = int(nDefPcsId / nNodeY);
					nMode = nDefPcsId % nNodeY;
					if (nNum % 2) 	// Ȧ��.
						nRow = nNodeY - (nMode + 1);
					else		// ¦��.
						nRow = nMode;

					nStripIdx = int(nRow / nStripY) + 1;
				}
			}
		}
	}
	else
	{
		if (pDoc->m_pPcrIts)	// [0]:AOI-Up , [1]:AOI-Dn , [2]:AOI-AllUp , [3]:AOI-AllDn
		{
			if (pDoc->m_pPcrIts[nIdx])
			{
				if (pDoc->m_pPcrIts[nIdx]->m_pDefPcs)
				{
					nDefPcsId = pDoc->m_pPcrIts[nIdx]->m_pDefPcs[nMkPcs];
					nNum = int(nDefPcsId / nNodeY);
					nMode = nDefPcsId % nNodeY;
					if (nNum % 2) 	// Ȧ��.
						nRow = nNodeY - (nMode + 1);
					else		// ¦��.
						nRow = nMode;

					nStripIdx = int(nRow / nStripY) + 1;
				}
			}
		}
	}
#else
	nStripIdx = 1;
#endif

	return nStripIdx;
}

int CGvisR2R_PunchView::GetMkStripIdx1Its(int nSerial, int nMkPcs) // 0 : Fail , 1~4 : Strip Idx
{
	if (nSerial <= 0)
	{
		pView->ClrDispMsg();
		AfxMessageBox(_T("Serial Error.51"));
		return 0;
	}

	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;
	int nIdx = pDoc->GetPcrIdx1(nSerial);
	int nNodeX = pDoc->m_Master[0].m_pPcsRgn->nCol;
	int nNodeY = pDoc->m_Master[0].m_pPcsRgn->nRow;
	int nStripY = int(nNodeY / MAX_STRIP_NUM);
	int nStripIdx = 0;

#ifdef USE_CAM_MASTER
	int nDefPcsId = 0, nNum = 0, nMode = 0, nRow = 0;
	if (bDualTest)
	{
		if (pDoc->m_pPcrIts)	// [0]:AOI-Up , [1]:AOI-Dn , [2]:AOI-AllUp , [3]:AOI-AllDn
		{
			if (pDoc->m_pPcrIts[nIdx])
			{
				if (pDoc->m_pPcrIts[nIdx]->m_pDefPcs)
				{
					nDefPcsId = pDoc->m_pPcrIts[nIdx]->m_pDefPcs[nMkPcs];
					nNum = int(nDefPcsId / nNodeY);
					nMode = nDefPcsId % nNodeY;
					if (nNum % 2) 	// Ȧ��.
						nRow = nNodeY - (nMode + 1);
					else		// ¦��.
						nRow = nMode;

					nStripIdx = int(nRow / nStripY) + 1;
				}
			}
		}
	}
	else
	{
		if (pDoc->m_pPcrIts)	// [0]:AOI-Up , [1]:AOI-Dn , [2]:AOI-AllUp , [3]:AOI-AllDn
		{
			if (pDoc->m_pPcrIts[nIdx])
			{
				if (pDoc->m_pPcrIts[nIdx]->m_pDefPcs)
				{
					nDefPcsId = pDoc->m_pPcrIts[nIdx]->m_pDefPcs[nMkPcs];
					nNum = int(nDefPcsId / nNodeY);
					nMode = nDefPcsId % nNodeY;
					if (nNum % 2) 	// Ȧ��.
						nRow = nNodeY - (nMode + 1);
					else		// ¦��.
						nRow = nMode;

					nStripIdx = int(nRow / nStripY) + 1;
				}
			}
		}
	}
#else
	nStripIdx = 1;
#endif

	return nStripIdx;
}

BOOL CGvisR2R_PunchView::SetMkIts(BOOL bRun)	// Marking Start
{
	CfPoint ptPnt;
	int nSerial, nTot, a, b;

	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;

	for (a = 0; a < 2; a++)
	{
		for (b = 0; b < MAX_STRIP_NUM; b++)
		{
			m_nMkStrip[a][b] = 0;			// [nCam][nStrip]:[2][4] - [��/��][] : ��Ʈ���� ��Ī�� �ǽ� �� count
			m_bRejectDone[a][b] = FALSE;	// [nCam][nStrip]:[2][4] - [��/��][] : ��Ʈ���� ��Ī�� �ǽ� �� count�� ��Ʈ�� ��� ������ �Ϸ� ���� 
		}
	}

	if (bRun)
	{
		if (m_bDoMk[0])
		{
			if (!m_bTHREAD_MK[0])
			{
				m_nStepMk[0] = 0;
				m_nMkPcs[0] = 0;
				m_bDoneMk[0] = FALSE;
				m_bTHREAD_MK[0] = TRUE;

				nSerial = m_nBufUpSerial[0]; // Cam0

				m_nTotMk[0] = nTot = GetTotDefPcs0Its(nSerial);
				m_nCurMk[0] = 0;
				if (nTot > 0)
				{
					ptPnt = GetMkPnt0Its(nSerial, 0);
					m_dTarget[AXIS_X0] = ptPnt.x;
					m_dTarget[AXIS_Y0] = ptPnt.y;
					if (nTot > 1)
					{
						ptPnt = GetMkPnt0Its(nSerial, 1);
						m_dNextTarget[AXIS_X0] = ptPnt.x;
						m_dNextTarget[AXIS_Y0] = ptPnt.y;
					}
					else
					{
						m_dNextTarget[AXIS_X0] = -1.0;
						m_dNextTarget[AXIS_Y0] = -1.0;
					}
				}
				else
				{
					m_dTarget[AXIS_X0] = -1.0;
					m_dTarget[AXIS_Y0] = -1.0;
					m_dNextTarget[AXIS_X0] = -1.0;
					m_dNextTarget[AXIS_Y0] = -1.0;
				}
			}
		}

		if (m_bDoMk[1])
		{
			if (!m_bTHREAD_MK[1])
			{
				m_nStepMk[1] = 0;
				m_nMkPcs[1] = 0;
				m_bDoneMk[1] = FALSE;
				m_bTHREAD_MK[1] = TRUE;

				nSerial = m_nBufUpSerial[1]; // Cam1

				m_nTotMk[1] = nTot = GetTotDefPcs1Its(nSerial);
				m_nCurMk[1] = 0;
				if (nTot > 0)
				{
					ptPnt = GetMkPnt1Its(nSerial, 0);
					m_dTarget[AXIS_X1] = ptPnt.x;
					m_dTarget[AXIS_Y1] = ptPnt.y;
					if (nTot > 1)
					{
						ptPnt = GetMkPnt1Its(nSerial, 1);
						m_dNextTarget[AXIS_X1] = ptPnt.x;
						m_dNextTarget[AXIS_Y1] = ptPnt.y;
					}
					else
					{
						m_dNextTarget[AXIS_X1] = -1.0;
						m_dNextTarget[AXIS_Y1] = -1.0;
					}
				}
				else
				{
					m_dTarget[AXIS_X1] = -1.0;
					m_dTarget[AXIS_Y1] = -1.0;
					m_dNextTarget[AXIS_X1] = -1.0;
					m_dNextTarget[AXIS_Y1] = -1.0;
				}
			}
		}
	}
	else
	{
		m_bTHREAD_MK[0] = FALSE;
		m_bTHREAD_MK[1] = FALSE;
	}
	return TRUE;
}


//BOOL CGvisR2R_PunchView::ReloadReelmapInner()
//{
//	double dRatio = 0.0;
//	CString sVal = _T("");
//	CDlgProgress dlg;
//	sVal.Format(_T("On Reloading InnerReelmap."));
//	dlg.Create(sVal);
//	//dlg.SetRange(0, 500);
//
//	//GetCurrentInfoEng();
//	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTestInner;
//
//
//	BOOL bRtn[7];
//	if (pDoc->m_pReelMapInner)
//		bRtn[0] = pDoc->m_pReelMapInner->ReloadReelmap();
//	//dlg.SetPos(1);
//	if (pDoc->m_pReelMapInnerUp)
//		bRtn[1] = pDoc->m_pReelMapInnerUp->ReloadReelmap();
//	//dlg.SetPos(2);
//
//	if (pDoc->m_pReelMapIts)
//		bRtn[2] = pDoc->m_pReelMapIts->ReloadReelmap();
//
//	//if (pDoc->m_pReelMapInOuterUp)
//	//	bRtn[2] = pDoc->m_pReelMapInOuterUp->ReloadReelmap();
//
//	//if (pDoc->m_pReelMapInOuterDn)
//	//	bRtn[3] = pDoc->m_pReelMapInOuterDn->ReloadReelmap();
//
//	if (bDualTest)
//	{
//		if (pDoc->m_pReelMapInnerDn)
//			bRtn[4] = pDoc->m_pReelMapInnerDn->ReloadReelmap();
//		//dlg.SetPos(3);
//		if (pDoc->m_pReelMapInnerAllUp)
//			bRtn[5] = pDoc->m_pReelMapInnerAllUp->ReloadReelmap();
//		//dlg.SetPos(4);
//		if (pDoc->m_pReelMapInnerAllDn)
//			bRtn[6] = pDoc->m_pReelMapInnerAllDn->ReloadReelmap();
//		//dlg.SetPos(5);
//	}
//
//	int nRatio[7] = { 0 };
//	BOOL bDone[7] = { 0 };
//	int nTo = 0;
//	if (bDualTest)
//		nTo = 600; //[%]
//	else
//		nTo = 300; //[%]
//
//	dlg.SetRange(0, nTo);
//
//	for (int nProc = 0; nProc < nTo;)
//	{
//		if (pDoc->m_pReelMapInner)
//		{
//			nRatio[0] = pDoc->m_pReelMapInner->GetProgressReloadReelmap();
//			bDone[0] = pDoc->m_pReelMapInner->IsDoneReloadReelmap();
//		}
//		else
//			bDone[0] = TRUE;
//		if (!bRtn[0])
//			bDone[0] = TRUE;
//
//		if (pDoc->m_pReelMapInnerUp)
//		{
//			nRatio[1] = pDoc->m_pReelMapInnerUp->GetProgressReloadReelmap();
//			bDone[1] = pDoc->m_pReelMapInnerUp->IsDoneReloadReelmap();
//		}
//		else
//			bDone[1] = TRUE;
//		if (!bRtn[1])
//			bDone[1] = TRUE;
//
//		//if (pDoc->m_pReelMapInOuterUp)
//		//{
//		//	nRatio[2] = pDoc->m_pReelMapInOuterUp->GetProgressReloadReelmap();
//		//	bDone[2] = pDoc->m_pReelMapInOuterUp->IsDoneReloadReelmap();
//		//}
//		//else
//		//	bDone[2] = TRUE;
//		//if (!bRtn[2])
//		//	bDone[2] = TRUE;
//
//		//if (pDoc->m_pReelMapInOuterDn)
//		//{
//		//	nRatio[3] = pDoc->m_pReelMapInOuterDn->GetProgressReloadReelmap();
//		//	bDone[3] = pDoc->m_pReelMapInOuterDn->IsDoneReloadReelmap();
//		//}
//		//else
//		//	bDone[3] = TRUE;
//		//if (!bRtn[3])
//			bDone[3] = TRUE;
//
//		if (pDoc->m_pReelMapIts)
//		{
//			nRatio[2] = pDoc->m_pReelMapIts->GetProgressReloadReelmap();
//			bDone[2] = pDoc->m_pReelMapIts->IsDoneReloadReelmap();
//		}
//		else
//			bDone[2] = TRUE;
//		if (!bRtn[2])
//			bDone[2] = TRUE;
//
//		if (bDualTest)
//		{
//			if (pDoc->m_pReelMapInnerDn)
//			{
//				nRatio[4] = pDoc->m_pReelMapInnerDn->GetProgressReloadReelmap();
//				bDone[4] = pDoc->m_pReelMapInnerDn->IsDoneReloadReelmap();
//			}
//			else
//				bDone[4] = TRUE;
//			if (!bRtn[4])
//				bDone[4] = TRUE;
//
//			if (pDoc->m_pReelMapInnerAllUp)
//			{
//				nRatio[5] = pDoc->m_pReelMapInnerAllUp->GetProgressReloadReelmap();
//				bDone[5] = pDoc->m_pReelMapInnerAllUp->IsDoneReloadReelmap();
//			}
//			else
//				bDone[5] = TRUE;
//			if (!bRtn[5])
//				bDone[5] = TRUE;
//
//			if (pDoc->m_pReelMapAllDn)
//			{
//				nRatio[6] = pDoc->m_pReelMapInnerAllDn->GetProgressReloadReelmap();
//				bDone[6] = pDoc->m_pReelMapInnerAllDn->IsDoneReloadReelmap();
//			}
//			else
//				bDone[6] = TRUE;
//			if (!bRtn[6])
//				bDone[6] = TRUE;
//
//		}
//		else
//		{
//			bDone[4] = TRUE;
//			bDone[5] = TRUE;
//			bDone[6] = TRUE;
//		}
//
//		nProc = nRatio[0] + nRatio[1] + nRatio[2] + nRatio[3] + nRatio[4] + nRatio[5] + nRatio[6];
//
//		if (bDone[0] && bDone[1] && bDone[2] && bDone[3] && bDone[4] && bDone[5] && bDone[6])
//			break;
//		else
//		{
//			dlg.SetPos(nProc);
//			Sleep(30);
//		}
//	}
//
//	dlg.DestroyWindow();
//
//	if (bDualTest)
//	{
//		for (int i = 0; i < 7; i++)
//		{
//			if (!bRtn[i])
//				return FALSE;
//		}
//	}
//	else
//	{
//		for (int i = 0; i < 3; i++)
//		{
//			if (!bRtn[i])
//				return FALSE;
//		}
//	}
//
//	return TRUE;
//}

BOOL CGvisR2R_PunchView::ReloadReelmapInner(int nSerial)
{
	//GetCurrentInfoEng();
	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTestInner;

	BOOL bRtn[7] = { 0 };
	if (pDoc->m_pReelMapInner)
		bRtn[0] = pDoc->m_pReelMapInner->ReloadReelmap(nSerial);
	if (pDoc->m_pReelMapInnerUp)
		bRtn[1] = pDoc->m_pReelMapInnerUp->ReloadReelmap(nSerial);

	//if (pDoc->m_pReelMapInOuterUp)
	//	bRtn[2] = pDoc->m_pReelMapInOuterUp->ReloadReelmap(nSerial);

	//if (pDoc->m_pReelMapInOuterDn)
	//	bRtn[3] = pDoc->m_pReelMapInOuterDn->ReloadReelmap(nSerial);
	bRtn[3] = TRUE;

	if (pDoc->m_pReelMapIts)
		bRtn[2] = pDoc->m_pReelMapIts->ReloadReelmap(nSerial);

	if (bDualTest)
	{
		if (pDoc->m_pReelMapInnerDn)
			bRtn[4] = pDoc->m_pReelMapInnerDn->ReloadReelmap(nSerial);
		if (pDoc->m_pReelMapInnerAllUp)
			bRtn[5] = pDoc->m_pReelMapInnerAllUp->ReloadReelmap(nSerial);
		if (pDoc->m_pReelMapInnerAllDn)
			bRtn[6] = pDoc->m_pReelMapInnerAllDn->ReloadReelmap(nSerial);

		for (int i = 0; i < 7; i++)
		{
			if (!bRtn[i])
				return FALSE;
		}
	}
	else
	{
		for (int i = 0; i < 3; i++)
		{
			if (!bRtn[i])
				return FALSE;
		}
	}

	return TRUE;
}

void CGvisR2R_PunchView::OpenReelmapInner()
{
	//GetCurrentInfoEng();
	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTestInner;

	if (pDoc->m_pReelMapInnerUp)
		pDoc->m_pReelMapInnerUp->Open();

	if (pDoc->m_pReelMapIts)
		pDoc->m_pReelMapIts->Open();

	if (bDualTest)
	{
		if (pDoc->m_pReelMapInnerDn)
			pDoc->m_pReelMapInnerDn->Open();
		if (pDoc->m_pReelMapInnerAllUp)
			pDoc->m_pReelMapInnerAllUp->Open();
		if (pDoc->m_pReelMapInnerAllDn)
			pDoc->m_pReelMapInnerAllDn->Open();
	}

	//if (pDoc->m_pReelMapInner)
	//{
	//	if (pDoc->m_pReelMapInner->m_nLayer < 0)
	//		pDoc->m_pReelMapInner->m_nLayer = pView->m_nSelRmapInner;
	//	pDoc->m_pReelMapInner->Open();
	//}

	//if (pDoc->m_pReelMapInOuterUp)
	//	pDoc->m_pReelMapInOuterUp->Open(GetRmapPath(RMAP_INOUTER_UP));

	//bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;
	//if (bDualTest)
	//{
	//	if (pDoc->m_pReelMapInOuterDn)
	//		pDoc->m_pReelMapInOuterDn->Open(GetRmapPath(RMAP_INOUTER_UP));
	//}
}

void CGvisR2R_PunchView::OpenReelmapInnerUp()
{
	//GetCurrentInfoEng();
	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTestInner;

	if (pDoc->m_pReelMapInnerUp)
		pDoc->m_pReelMapInnerUp->Open();

	if (bDualTest)
	{
		if (pDoc->m_pReelMapInnerAllUp)
			pDoc->m_pReelMapInnerAllUp->Open();
	}

	//if (pDoc->m_pReelMapInner)
	//{
	//	if (pDoc->m_pReelMapInner->m_nLayer < 0)
	//		pDoc->m_pReelMapInner->m_nLayer = pView->m_nSelRmapInner;

	//	if (bDualTest)
	//	{
	//		if (pDoc->m_pReelMapInner->m_nLayer == RMAP_UP || pDoc->m_pReelMapInner->m_nLayer == RMAP_ALLUP)
	//			pDoc->m_pReelMapInner->Open();
	//	}
	//	else
	//	{
	//		if (pDoc->m_pReelMapInner->m_nLayer == RMAP_UP)
	//			pDoc->m_pReelMapInner->Open();
	//	}
	//}
}

void CGvisR2R_PunchView::OpenReelmapInnerDn()
{
	//GetCurrentInfoEng();
	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTestInner;

	if (!bDualTest)
		return;

	if (pDoc->m_pReelMapInnerDn)
		pDoc->m_pReelMapInnerDn->Open();
	if (pDoc->m_pReelMapInnerAllDn)
		pDoc->m_pReelMapInnerAllDn->Open();

	//if (pDoc->m_pReelMapInner)
	//{
	//	if (pDoc->m_pReelMapInner->m_nLayer < 0)
	//		pDoc->m_pReelMapInner->m_nLayer = pView->m_nSelRmapInner;

	//	if (pDoc->m_pReelMapInner->m_nLayer == RMAP_DN || pDoc->m_pReelMapInner->m_nLayer == RMAP_ALLDN)
	//		pDoc->m_pReelMapInner->Open();
	//}
}


void CGvisR2R_PunchView::UpdateRstInner()
{
	if (m_pDlgMenu06)
		m_pDlgMenu06->UpdateRst();
}

void CGvisR2R_PunchView::InitReelmapInner()
{
	pDoc->InitReelmapInner();
	pDoc->SetReelmapInner(ROT_NONE);
	// 	pDoc->SetReelmap(ROT_CCW_90);
	//pDoc->UpdateData();

	//pDoc->GetCurrentInfoEng();
	////if (pDoc->GetTestMode() == MODE_OUTER)
	//if (pDoc->m_Master[0].IsMstSpec(pDoc->WorkingInfo.System.sPathCamSpecDir, pDoc->WorkingInfo.LastJob.sModelUp, pDoc->m_sEngLayerUp))
	//{
	//	pDoc->InitReelmapInner();
	//	pDoc->SetReelmapInner(ROT_NONE);
	//}
}

void CGvisR2R_PunchView::InitReelmapInnerUp()
{
	pDoc->InitReelmapInnerUp();
	pDoc->SetReelmapInner(ROT_NONE);
	// 	pDoc->SetReelmap(ROT_CCW_90);
	//pDoc->UpdateData();

	//if (pDoc->GetTestMode() == MODE_OUTER)
	//pDoc->GetCurrentInfoEng();
	//if (pDoc->m_Master[0].IsMstSpec(pDoc->WorkingInfo.System.sPathCamSpecDir, pDoc->WorkingInfo.LastJob.sModelUp, pDoc->m_sEngLayerUp))
	//{
	//	pDoc->InitReelmapInnerUp();

	//	pDoc->GetCurrentInfoEng();
	//	if (pDoc->m_bEngDualTest)
	//		pDoc->InitReelmapInnerDn();
	//}
}

void CGvisR2R_PunchView::InitReelmapInnerDn()
{
	//pDoc->GetCurrentInfoEng();
	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTestInner;
	//if (!bDualTest)
	//	return;

	if (bDualTest)
	{
		pDoc->InitReelmapInnerDn();
		pDoc->SetReelmapInner(ROT_NONE);
		// 	pDoc->SetReelmap(ROT_CCW_90);
		//pDoc->UpdateData();
	}
}


void CGvisR2R_PunchView::DispDefImgInner()
{
	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTestInner;

	CString sNewLot;
	BOOL bNewModel = FALSE;
	int nSerial = 0;
	int nNewLot = 0;
	int nBreak = 0;

	switch (m_nStepTHREAD_DISP_DEF_INNER)
	{
		// CopyDefImg Start ============================================
	case 0:
		m_nStepTHREAD_DISP_DEF_INNER++;

		//if (bDualTest)
		//{
		//	nSerial = m_nBufDnSerial[0]; // ���� Camera
		//	sNewLot = m_sNewLotDn;
		//}
		//else
		//{
		//	nSerial = m_nBufUpSerial[0]; // ���� Camera
		//	sNewLot = m_sNewLotUp;
		//}

		//if (nSerial == pView->m_nLotEndSerial)
		//	nBreak = 1;

		//if (nSerial > 0)
		//{
		//	//if (!CopyDefImg(nSerial, sNewLot)) // ���� Camera
		//	//{
		//	//	sNewLot.Empty();
		//	//	m_bDispMsgDoAuto[7] = TRUE;
		//	//	m_nStepDispMsg[7] = FROM_DISPDEFIMG + 7;
		//	//	break;
		//	//}

		//	if (m_bLastProc && nSerial + 1 > m_nLotEndSerial)
		//	{
		//		if (bDualTest)
		//			nSerial = m_nBufDnSerial[0]; // Test
		//		else
		//			nSerial = m_nBufUpSerial[0]; // Test
		//	}
		//	else
		//	{
		//		if (bDualTest)
		//			nSerial = m_nBufDnSerial[1]; // ���� Camera
		//		else
		//			nSerial = m_nBufUpSerial[1]; // ���� Camera

		//		//if (nSerial > 0)
		//		//{
		//		//	if (!CopyDefImg(nSerial, sNewLot)) // ���� Camera
		//		//	{
		//		//		sNewLot.Empty();
		//		//		m_bDispMsgDoAuto[6] = TRUE;
		//		//		m_nStepDispMsg[6] = FROM_DISPDEFIMG + 6;
		//		//		break;
		//		//	}
		//		//}
		//	}

		//}
		//else
		//{
		//	if (!m_bLastProc)
		//	{
		//		m_bDispMsgDoAuto[5] = TRUE;
		//		m_nStepDispMsg[5] = FROM_DISPDEFIMG + 5;
		//	}
		//}
		//sNewLot.Empty();
		break;
	case 1:
		Sleep(300);
		m_nStepTHREAD_DISP_DEF_INNER++;
		break;
	case 2:
		nSerial = m_nBufUpSerial[0]; // ���� Camera

		if (IsDoneDispMkInfoInner())	 // Check �ҷ��̹��� Display End
		{
			if (pView->m_bSerialDecrese)
			{
				if (ChkLastProc() && (nSerial < m_nLotEndSerial))
				{
					SetSerialReelmapInner(nSerial, TRUE);	// Reelmap(��) Display Start
					SetSerialMkInfoInner(nSerial, TRUE);		// �ҷ��̹���(��) Display Start
				}
				else if (ChkLastProc() && nSerial < 1)
				{
					SetSerialReelmapInner(m_nLotEndSerial - 1, TRUE);		// Reelmap(��) Display Start
					SetSerialMkInfoInner(m_nLotEndSerial - 1, TRUE);		// �ҷ��̹���(��) Display Start
				}
				else
				{
					//SetFixPcs(nSerial);
					SetSerialReelmapInner(nSerial);	// Reelmap(��) Display Start
					SetSerialMkInfoInner(nSerial);	// �ҷ��̹���(��) Display Start
				}
			}
			else
			{
				if (ChkLastProc() && (nSerial > m_nLotEndSerial))
				{
					SetSerialReelmapInner(nSerial, TRUE);	// Reelmap(��) Display Start
					SetSerialMkInfoInner(nSerial, TRUE);		// �ҷ��̹���(��) Display Start
				}
				else if (ChkLastProc() && nSerial < 1)
				{
					SetSerialReelmapInner(m_nLotEndSerial + 1, TRUE);		// Reelmap(��) Display Start
					SetSerialMkInfoInner(m_nLotEndSerial + 1, TRUE);		// �ҷ��̹���(��) Display Start
				}
				else
				{
					//SetFixPcs(nSerial);
					SetSerialReelmapInner(nSerial);	// Reelmap(��) Display Start
					SetSerialMkInfoInner(nSerial);	// �ҷ��̹���(��) Display Start
				}
			}

			//if (IsFixPcsUp(nSerial))
			//{
			//	m_bDispMsgDoAuto[2] = TRUE;
			//	m_nStepDispMsg[2] = FROM_DISPDEFIMG + 2;
			//}
			//if (IsFixPcsDn(nSerial))
			//{
			//	m_bDispMsgDoAuto[3] = TRUE;
			//	m_nStepDispMsg[3] = FROM_DISPDEFIMG + 3;
			//}

			m_nStepTHREAD_DISP_DEF_INNER++;
		}

		break;

	case 3:
		if (IsDoneDispMkInfoInner())	 // Check �ҷ��̹���(��) Display End
		{
			nSerial = m_nBufUpSerial[1]; // ���� Camera

			if (nSerial > 0)
			{
				if (pView->m_bSerialDecrese)
				{
					if (ChkLastProc() && (nSerial < m_nLotEndSerial))
						SetSerialMkInfoInner(nSerial, TRUE);	// �ҷ��̹���(��) Display Start
					else
						SetSerialMkInfoInner(nSerial);		// �ҷ��̹���(��) Display Start
				}
				else
				{
					if (ChkLastProc() && (nSerial > m_nLotEndSerial))
						SetSerialMkInfoInner(nSerial, TRUE);	// �ҷ��̹���(��) Display Start
					else
						SetSerialMkInfoInner(nSerial);		// �ҷ��̹���(��) Display Start
				}
			}
			else
			{
				if (pView->m_bSerialDecrese)
				{
					if (ChkLastProc())
						SetSerialMkInfoInner(m_nLotEndSerial - 1, TRUE);	// �ҷ��̹���(��) Display Start
				}
				else
				{
					if (ChkLastProc())
						SetSerialMkInfoInner(m_nLotEndSerial + 1, TRUE);	// �ҷ��̹���(��) Display Start
				}
			}
			m_nStepTHREAD_DISP_DEF_INNER++;
		}
		break;
	case 4:
		nSerial = m_nBufUpSerial[1]; // ���� Camera

		if (nSerial > 0)
		{
			m_nStepTHREAD_DISP_DEF_INNER++;

			if (pView->m_bSerialDecrese)
			{
				if (ChkLastProc() && (nSerial < m_nLotEndSerial))
				{
					SetSerialReelmapInner(nSerial, TRUE);	// Reelmap(��) Display Start
				}
				else
				{
					//SetFixPcs(nSerial);
					SetSerialReelmapInner(nSerial);			// Reelmap(��) Display Start
				}
			}
			else
			{
				if (ChkLastProc() && (nSerial > m_nLotEndSerial))
				{
					SetSerialReelmapInner(nSerial, TRUE);	// Reelmap(��) Display Start
				}
				else
				{
					//SetFixPcs(nSerial);
					SetSerialReelmapInner(nSerial);			// Reelmap(��) Display Start
				}
			}
		}
		else
		{
			if (ChkLastProc())
			{
				m_nStepTHREAD_DISP_DEF_INNER++;
				if (pView->m_bSerialDecrese)
					SetSerialReelmapInner(m_nLotEndSerial - 1, TRUE);	// �ҷ��̹���(��) Display Start
				else
					SetSerialReelmapInner(m_nLotEndSerial + 1, TRUE);	// �ҷ��̹���(��) Display Start
			}
			else
			{
				if (bDualTest)
				{
					if (m_bLastProc && m_nBufDnSerial[0] == m_nLotEndSerial)
						m_nStepTHREAD_DISP_DEF_INNER++;
					else
					{
						m_nStepTHREAD_DISP_DEF_INNER++;
					}
				}
				else
				{
					if (m_bLastProc && m_nBufUpSerial[0] == m_nLotEndSerial)
						m_nStepTHREAD_DISP_DEF_INNER++;
					else
					{
						m_nStepTHREAD_DISP_DEF_INNER++;
					}
				}
			}
		}
		break;
	case 5:
		m_nStepTHREAD_DISP_DEF_INNER++;
		break;
	case 6:
		m_nStepTHREAD_DISP_DEF_INNER++;
		break;
	case 7:
		m_nStepTHREAD_DISP_DEF_INNER++;
		break;
	case 8:
		m_nStepTHREAD_DISP_DEF_INNER++;
		break;
	case 9:
		m_nStepTHREAD_DISP_DEF_INNER++;
		break;
	case 10:
		m_nStepTHREAD_DISP_DEF_INNER++;
		break;

	case 11:
		if (IsDoneDispMkInfoInner() && IsRun())	 // Check �ҷ��̹���(��) Display End
			m_nStepTHREAD_DISP_DEF_INNER++;
		break;
	case 12:
		m_bTHREAD_DISP_DEF_INNER = FALSE;
		break;
		// Disp DefImg End ============================================
	}

}

BOOL CGvisR2R_PunchView::IsDoneDispMkInfoInner()
{
	BOOL bRtn = FALSE;
	if (m_pDlgMenu06)
		bRtn = m_pDlgMenu06->IsDoneDispMkInfo();
	return bRtn;
}

BOOL CGvisR2R_PunchView::SetSerialReelmapInner(int nSerial, BOOL bDumy)
{
	if (!m_pDlgMenu06)
	{
		pView->ClrDispMsg();
		AfxMessageBox(_T("Error - SetSerialReelmap : m_pDlgMenu06 is NULL."));
		return FALSE;
	}

	//if (pDoc->GetTestMode() == MODE_OUTER)
	//{
	//	if (!m_pDlgMenu06)
	//	{
	//		pView->ClrDispMsg();
	//		AfxMessageBox(_T("Error - SetSerialReelmap : m_pDlgMenu06 is NULL."));
	//		return FALSE;
	//	}

	//	m_pDlgMenu06->SetSerialReelmap(nSerial, bDumy);
	//}

	return m_pDlgMenu06->SetSerialReelmap(nSerial, bDumy);
}

BOOL CGvisR2R_PunchView::SetSerialMkInfoInner(int nSerial, BOOL bDumy)
{
	if (!m_pDlgMenu06)
		return FALSE;
	return m_pDlgMenu06->SetSerialMkInfo(nSerial, bDumy);
}

CString CGvisR2R_PunchView::GetTimeIts()
{
	stLotTime ItsTime;

	CString strVal;
	time_t osBinTime;			// C run-time time (defined in <time.h>)
	time(&osBinTime);		// Get the current time from the 
							// operating system.
	CTime Tim(osBinTime);

	ItsTime.nYear = Tim.GetYear();
	ItsTime.nMonth = Tim.GetMonth();
	ItsTime.nDay = Tim.GetDay();
	ItsTime.nHour = Tim.GetHour();
	ItsTime.nMin = Tim.GetMinute();
	ItsTime.nSec = Tim.GetSecond();

	strVal.Format(_T("%04d%02d%02d%02d%02d%02d"),
		ItsTime.nYear, ItsTime.nMonth, ItsTime.nDay,
		ItsTime.nHour, ItsTime.nMin, ItsTime.nSec);

	return strVal;
}

//BOOL CGvisR2R_PunchView::UpdateReelmapInner(int nSerial)
//{
//	if (nSerial <= 0)
//	{
//		pView->ClrDispMsg();
//		AfxMessageBox(_T("Serial Error.54"));
//		return 0;
//	}
//
//	if (!pDoc->MakeMkDir())
//		return FALSE;
//
//	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTestInner;
//
//	CString str;
//	CString sPathRmap[4], sPathPcr[2]; //[Up/Dn]
//	if (pDoc->m_pReelMap)
//	{
//		stModelInfo stInfo;
//		CString sPath, sUpPath, sDnPath;
//		if (!pDoc->GetInnerFolderPath(nSerial, sUpPath, sDnPath))
//		{
//			str.Format(_T("GetInnerFolderPath�� �������� �ʾҽ��ϴ�."));
//			pView->MsgBox(str);
//			//AfxMessageBox(strFileData);
//			return FALSE;
//		}
//		sPathPcr[0].Format(_T("%s%04d.pcr"), sUpPath, nSerial);
//		if (pDoc->WorkingInfo.LastJob.bDualTestInner)
//			sPathPcr[1].Format(_T("%s%04d.pcr"), sDnPath, nSerial);
//
//
//		if (!pDoc->GetPcrInfo(sPathPcr[0], stInfo)) // Up
//		{
//			pView->DispStsBar(_T("E(4)"), 5);
//			pView->ClrDispMsg();
//			AfxMessageBox(_T("Error-GetPcrInfo(4)"));
//			return FALSE;
//		}
//
//		if (!pDoc->MakeItsDir(stInfo.sModel, stInfo.sLot, stInfo.sLayer))
//			return FALSE;
//
//		str = _T("ReelMapDataUp.txt"); // [0]:AOI-Up
//		sPathRmap[0].Format(_T("%s%s\\%s\\%s\\%s\\%s"), pDoc->WorkingInfo.System.sPathItsFile,stInfo.sModel, 
//			pDoc->WorkingInfo.LastJob.sEngItsCode, stInfo.sLot, stInfo.sLayer, str);
//
//		if (bDualTest)
//		{
//			str = _T("ReelMapDataAll.txt"); // [2]:AOI-AllUp
//			sPathRmap[2].Format(_T("%s%s\\%s\\%s\\%s\\%s"), pDoc->WorkingInfo.System.sPathItsFile, stInfo.sModel,
//				pDoc->WorkingInfo.LastJob.sEngItsCode, stInfo.sLot, stInfo.sLayer, str);
//
//			if (!pDoc->GetPcrInfo(sPathPcr[1], stInfo)) // Dn
//			{
//				pView->DispStsBar(_T("E(5)"), 5);
//				pView->ClrDispMsg();
//				AfxMessageBox(_T("Error-GetPcrInfo(5)"));
//				return FALSE;
//			}
//
//			str = _T("ReelMapDataDn.txt"); // [1]:AOI-Dn
//			sPathRmap[1].Format(_T("%s%s\\%s\\%s\\%s\\%s"), pDoc->WorkingInfo.System.sPathItsFile, stInfo.sModel,
//				pDoc->WorkingInfo.LastJob.sEngItsCode, stInfo.sLot, stInfo.sLayer, str);
//
//			str = _T("ReelMapDataAll.txt"); // [3]:AOI-AllDn
//			sPathRmap[3].Format(_T("%s%s\\%s\\%s\\%s\\%s"), pDoc->WorkingInfo.System.sPathItsFile, stInfo.sModel,
//				pDoc->WorkingInfo.LastJob.sEngItsCode, stInfo.sLot, stInfo.sLayer, str);
//		}
//
//		m_nSerialRmapInnerUpdate = nSerial;
//		m_sPathRmapInnerUpdate[0] = sPathRmap[0];
//		m_sPathRmapInnerUpdate[1] = sPathRmap[1];
//		m_sPathRmapInnerUpdate[2] = sPathRmap[2];
//		m_sPathRmapInnerUpdate[3] = sPathRmap[3];
//
//		m_bTHREAD_UPDATE_REELMAP_INNER_UP = TRUE;
//		if (bDualTest)
//		{
//			m_bTHREAD_UPDATE_REELMAP_INNER_DN = TRUE;
//			m_bTHREAD_UPDATE_REELMAP_INNER_ALLUP = TRUE;
//			m_bTHREAD_UPDATE_REELMAP_INNER_ALLDN = TRUE;
//		}
//
//		if (pDoc->GetTestMode() == MODE_OUTER)
//			m_bTHREAD_UPDATE_REELMAP_ITS = TRUE;
//
//		Sleep(100);
//		return TRUE;
//	}
//	else
//		MsgBox(_T("Error-ReelMapWrite()"));
//
//	return FALSE;
//}


void CGvisR2R_PunchView::UpdateRMapInnerUp()
{
	if (pDoc->GetTestMode() == MODE_INNER)
	{
		//if (pDoc->m_pReelMapInnerUp)
		//	pDoc->m_pReelMapInnerUp->MakeItsFile(m_nSerialRmapInnerUpdate, RMAP_UP);
		if (pDoc->m_pReelMapUp)
			pDoc->m_pReelMapUp->MakeItsFile(m_nSerialRmapInnerUpdate, RMAP_INNER_UP);
	}

	if (pDoc->GetTestMode() == MODE_OUTER)
	{
		if (pDoc->m_pReelMapUp)
			pDoc->m_pReelMapUp->MakeItsFile(m_nSerialRmapInnerUpdate, RMAP_UP);
		//if(pDoc->m_pReelMapInnerUp)
		//	pDoc->m_pReelMapInnerUp->Write(m_nSerialRmapInnerUpdate); // [0]:AOI-Up , [1]:AOI-Dn , [2]:AOI-AllUp , [3]:AOI-AllDn
			//pDoc->m_pReelMapInnerUp->Write(m_nSerialRmapInnerUpdate, 0, m_sPathRmapInnerUpdate[0]); // [0]:AOI-Up , [1]:AOI-Dn , [2]:AOI-AllUp , [3]:AOI-AllDn

		//if (pDoc->m_pReelMapInnerUp)
		//	pDoc->m_pReelMapInnerUp->MakeItsFile(m_nSerialRmapInnerUpdate, RMAP_INNER_UP);
		//if (pDoc->m_pReelMapInnerUp)
		//	pDoc->m_pReelMapInnerUp->MakeItsFile(m_nSerialRmapInnerUpdate, RMAP_UP);
	}
}

void CGvisR2R_PunchView::UpdateRMapInnerDn()
{
	if (pDoc->GetTestMode() == MODE_INNER)
	{
		//if (pDoc->m_pReelMapInnerDn)
		//	pDoc->m_pReelMapInnerDn->MakeItsFile(m_nSerialRmapInnerUpdate, RMAP_INNER_DN);
		if (pDoc->m_pReelMapDn)
			pDoc->m_pReelMapDn->MakeItsFile(m_nSerialRmapInnerUpdate, RMAP_INNER_DN);
	}

	if (pDoc->GetTestMode() == MODE_OUTER)
	{
		if (pDoc->m_pReelMapDn)
			pDoc->m_pReelMapDn->MakeItsFile(m_nSerialRmapInnerUpdate, RMAP_DN);
		//if (pDoc->m_pReelMapInnerDn)
		//	pDoc->m_pReelMapInnerDn->Write(m_nSerialRmapInnerUpdate); // [0]:AOI-Up , [1]:AOI-Dn , [2]:AOI-AllUp , [3]:AOI-AllDn
			//pDoc->m_pReelMapInnerDn->Write(m_nSerialRmapInnerUpdate, 1, m_sPathRmapInnerUpdate[1]); // [0]:AOI-Up , [1]:AOI-Dn , [2]:AOI-AllUp , [3]:AOI-AllDn

		//if (pDoc->m_pReelMapInnerDn)
		//	pDoc->m_pReelMapInnerDn->MakeItsFile(m_nSerialRmapInnerUpdate, RMAP_INNER_DN);
		//if (pDoc->m_pReelMapInnerDn)
		//	pDoc->m_pReelMapInnerDn->MakeItsFile(m_nSerialRmapInnerUpdate, RMAP_DN);
	}
}

void CGvisR2R_PunchView::UpdateRMapInnerAllUp()
{
	if (pDoc->GetTestMode() == MODE_OUTER)
	{
		if (pDoc->m_pReelMapInnerAllUp)
			pDoc->m_pReelMapInnerAllUp->Write(m_nSerialRmapInnerUpdate); // [0]:AOI-Up , [1]:AOI-Dn , [2]:AOI-AllUp , [3]:AOI-AllDn
			//pDoc->m_pReelMapInnerAllUp->Write(m_nSerialRmapInnerUpdate, 2, m_sPathRmapInnerUpdate[2]); // [0]:AOI-Up , [1]:AOI-Dn , [2]:AOI-AllUp , [3]:AOI-AllDn
	}
}

void CGvisR2R_PunchView::UpdateRMapInnerAllDn()
{
	if (pDoc->GetTestMode() == MODE_OUTER)
	{
		if (pDoc->m_pReelMapInnerAllDn)
			pDoc->m_pReelMapInnerAllDn->Write(m_nSerialRmapInnerUpdate); // [0]:AOI-Up , [1]:AOI-Dn , [2]:AOI-AllUp , [3]:AOI-AllDn
			//pDoc->m_pReelMapInnerAllDn->Write(m_nSerialRmapInnerUpdate, 3, m_sPathRmapInnerUpdate[3]); // [0]:AOI-Up , [1]:AOI-Dn , [2]:AOI-AllUp , [3]:AOI-AllDn
	}
}

UINT CGvisR2R_PunchView::ThreadProc26(LPVOID lpContext)	// UpdateRMapUp()
{
	// Turn the passed in 'this' pointer back into a CProgressMgr instance
	CGvisR2R_PunchView* pThread = reinterpret_cast<CGvisR2R_PunchView*>(lpContext);

	BOOL bLock = FALSE;
	DWORD dwTick = GetTickCount();
	DWORD dwShutdownEventCheckPeriod = 0; // thread shutdown event check period

	pThread->m_bThread[26] = TRUE;
	while (WAIT_OBJECT_0 != WaitForSingleObject(pThread->m_Thread[26].GetShutdownEvent(), dwShutdownEventCheckPeriod))
	{
		pThread->m_dwThreadTick[26] = GetTickCount() - dwTick;
		dwTick = GetTickCount();

		if (pThread->m_bTHREAD_UPDATE_REELMAP_INNER_UP) // Write Reelmap
		{
			pThread->UpdateRMapInnerUp();
			pThread->m_bTHREAD_UPDATE_REELMAP_INNER_UP = FALSE;
			Sleep(0);
		}
		else
			Sleep(30);
	}

	pThread->m_bThread[26] = FALSE;

	return 0;
}

UINT CGvisR2R_PunchView::ThreadProc27(LPVOID lpContext)	// UpdateRMapDn()
{
	// Turn the passed in 'this' pointer back into a CProgressMgr instance
	CGvisR2R_PunchView* pThread = reinterpret_cast<CGvisR2R_PunchView*>(lpContext);

	BOOL bLock = FALSE;
	DWORD dwTick = GetTickCount();
	DWORD dwShutdownEventCheckPeriod = 0; // thread shutdown event check period

	pThread->m_bThread[27] = TRUE;
	while (WAIT_OBJECT_0 != WaitForSingleObject(pThread->m_Thread[27].GetShutdownEvent(), dwShutdownEventCheckPeriod))
	{
		pThread->m_dwThreadTick[27] = GetTickCount() - dwTick;
		dwTick = GetTickCount();

		if (pThread->m_bTHREAD_UPDATE_REELMAP_INNER_DN) // Write Reelmap
		{
			//pThread->m_bTHREAD_UPDATE_REELMAP_ALLUP = TRUE;
			//pThread->m_bTHREAD_UPDATE_REELMAP_ALLDN = TRUE;
			pThread->UpdateRMapInnerDn();
			pThread->m_bTHREAD_UPDATE_REELMAP_INNER_DN = FALSE;
			Sleep(0);
		}
		else
			Sleep(30);
	}

	pThread->m_bThread[27] = FALSE;

	return 0;
}

UINT CGvisR2R_PunchView::ThreadProc28(LPVOID lpContext)	// UpdateRMapAllUp()
{
	// Turn the passed in 'this' pointer back into a CProgressMgr instance
	CGvisR2R_PunchView* pThread = reinterpret_cast<CGvisR2R_PunchView*>(lpContext);

	BOOL bLock = FALSE;
	DWORD dwTick = GetTickCount();
	DWORD dwShutdownEventCheckPeriod = 0; // thread shutdown event check period

	pThread->m_bThread[28] = TRUE;
	while (WAIT_OBJECT_0 != WaitForSingleObject(pThread->m_Thread[28].GetShutdownEvent(), dwShutdownEventCheckPeriod))
	{
		pThread->m_dwThreadTick[28] = GetTickCount() - dwTick;
		dwTick = GetTickCount();

		if (pThread->m_bTHREAD_UPDATE_REELMAP_INNER_ALLUP) // Write Reelmap
		{
			pThread->UpdateRMapInnerAllUp();
			pThread->m_bTHREAD_UPDATE_REELMAP_INNER_ALLUP = FALSE;
			Sleep(0);
		}
		else
			Sleep(30);
	}

	pThread->m_bThread[28] = FALSE;

	return 0;
}

UINT CGvisR2R_PunchView::ThreadProc29(LPVOID lpContext)	// UpdateRMapInnerAllDn()
{
	// Turn the passed in 'this' pointer back into a CProgressMgr instance
	CGvisR2R_PunchView* pThread = reinterpret_cast<CGvisR2R_PunchView*>(lpContext);

	BOOL bLock = FALSE;
	DWORD dwTick = GetTickCount();
	DWORD dwShutdownEventCheckPeriod = 0; // thread shutdown event check period

	pThread->m_bThread[29] = TRUE;
	while (WAIT_OBJECT_0 != WaitForSingleObject(pThread->m_Thread[29].GetShutdownEvent(), dwShutdownEventCheckPeriod))
	{
		pThread->m_dwThreadTick[29] = GetTickCount() - dwTick;
		dwTick = GetTickCount();
		
		if (pThread->m_bTHREAD_UPDATE_REELMAP_INNER_ALLDN) // Write Reelmap
		{
			pThread->UpdateRMapInnerAllDn();
			pThread->m_bTHREAD_UPDATE_REELMAP_INNER_ALLDN = FALSE;
			Sleep(0);
		}
		else
			Sleep(30);
	}

	pThread->m_bThread[29] = FALSE;

	return 0;
}


void CGvisR2R_PunchView::MakeResultMDS()
{
	CString sPath, strMsg;
	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;

	pDoc->WorkingInfo.LastJob.sProcessNum = pDoc->GetProcessNum();
	pDoc->UpdateProcessNum(pDoc->WorkingInfo.LastJob.sProcessNum);

	//if (m_pDlgMenu05)
	//{
	//	m_pDlgMenu05->m_sModel = pDoc->WorkingInfo.LastJob.sModelUp;
	//	m_pDlgMenu05->m_sLot = pDoc->WorkingInfo.LastJob.sLotUp;
	//	m_pDlgMenu05->m_sLayer = pDoc->WorkingInfo.LastJob.sLayerUp;
	//	m_pDlgMenu05->m_sProcessNum = pDoc->WorkingInfo.LastJob.sProcessNum;
	//	sPath.Format(_T("%s%s\\%s\\%s"), pDoc->WorkingInfo.System.sPathOldFile,
	//		pDoc->WorkingInfo.LastJob.sModelUp,
	//		pDoc->WorkingInfo.LastJob.sLotUp,
	//		pDoc->WorkingInfo.LastJob.sLayerUp);

	//	if (bDualTest)
	//		m_pDlgMenu05->m_sRmapPath.Format(_T("%s\\ReelMapDataAll.txt"), sPath);
	//	else
	//		m_pDlgMenu05->m_sRmapPath.Format(_T("%s\\ReelMapDataUp.txt"), sPath);

	//	m_pDlgMenu05->GetResult();

	//	MakeResult(); // Result.txt
	//	MakeSapp3();
	//}
	//else
	//{
	//	strMsg.Format(_T("It is trouble to open file.\r\n%s"), sPath);
	//	pView->ClrDispMsg();
	//	AfxMessageBox(strMsg, MB_ICONWARNING | MB_OK);
	//}


	// for SAPP3
	GetResult();		// Load data From Reelmap.txt
	MakeResult();		// Result.txt
	MakeResultIts();	// Result.txt
	MakeSapp3();		// GetSapp3Txt()

	RemakeReelmap();	// MDS(�ؼ�DS) Style�� �������� �����

	if (pDoc->GetTestMode() == MODE_OUTER)
		RemakeReelmapInner();
}

BOOL CGvisR2R_PunchView:: RemakeReelmap()
{
	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;
	CString sReelmapSrc, str;

	if (pDoc->m_pReelMapUp)
		pDoc->m_pReelMapUp->StartThreadRemakeReelmap();

	if (bDualTest)
	{
		if (pDoc->m_pReelMapDn)
			pDoc->m_pReelMapDn->StartThreadRemakeReelmap();

		if (pDoc->m_pReelMapAllUp)
			pDoc->m_pReelMapAllUp->StartThreadRemakeReelmap();
	}

	return TRUE;
}

BOOL CGvisR2R_PunchView::IsDoneRemakeReelmap()
{
	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;
	BOOL bSuccess[3] = { FALSE };
	DWORD dwSt = GetTickCount();

	Sleep(100);

	do
	{
		if (GetTickCount() - dwSt > 600000)
			break;
	} while (!pDoc->m_pReelMapUp->m_bThreadAliveRemakeReelmap && !pDoc->m_pReelMapDn->m_bThreadAliveRemakeReelmap && !pDoc->m_pReelMapAllUp->m_bThreadAliveRemakeReelmap);

	if (bDualTest)
	{
		bSuccess[0] = pDoc->m_pReelMapUp->m_bRtnThreadRemakeReelmap;
		bSuccess[1] = pDoc->m_pReelMapDn->m_bRtnThreadRemakeReelmap;
		bSuccess[2] = pDoc->m_pReelMapAllUp->m_bRtnThreadRemakeReelmap;

		if (!bSuccess[0] || !bSuccess[2] || !bSuccess[1])
		{
			MsgBox(_T("ReelMap Converting Failed."));
			return FALSE;
		}
	}
	else
	{
		if (!pDoc->m_pReelMapUp->m_bRtnThreadRemakeReelmap)
		{
			MsgBox(_T("ReelMap Converting Failed."));
			return FALSE;
		}
	}

	return TRUE;
}

BOOL CGvisR2R_PunchView::RemakeReelmapInner()
{
	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;
	CString sReelmapSrc, str;

	//if (pDoc->m_pReelMapInnerUp)
	//	pDoc->m_pReelMapInnerUp->StartThreadRemakeReelmap();

	if (pDoc->m_pReelMapIts)
		pDoc->m_pReelMapIts->StartThreadRemakeReelmap();

	//if (pDoc->WorkingInfo.LastJob.bDualTestInner)
	//{
	//	if (pDoc->m_pReelMapInnerDn)
	//		pDoc->m_pReelMapInnerDn->StartThreadRemakeReelmap();

	//	if (pDoc->m_pReelMapInnerAllUp)
	//		pDoc->m_pReelMapInnerAllUp->StartThreadRemakeReelmap();
	//}

	return TRUE;
}

BOOL CGvisR2R_PunchView::IsDoneRemakeReelmapInner()
{
	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTestInner;
	BOOL bAlive[4] = { FALSE };
	BOOL bSuccess[4] = { TRUE };
	BOOL bWait = TRUE;
	DWORD dwSt = GetTickCount();

	Sleep(100);

	do
	{
		if (GetTickCount() - dwSt > 600000)
			break;

		bAlive[0] = pDoc->m_pReelMapInnerUp->m_bThreadAliveRemakeReelmap;
		bAlive[1] = pDoc->m_pReelMapInnerDn->m_bThreadAliveRemakeReelmap;
		bAlive[2] = pDoc->m_pReelMapInnerAllUp->m_bThreadAliveRemakeReelmap;
		bAlive[3] = pDoc->m_pReelMapIts->m_bThreadAliveRemakeReelmap;

		if (bAlive[0] || bAlive[1] || bAlive[2] || bAlive[3])
			bWait = TRUE;
		else
			bWait = FALSE;

	} while (bWait);

	if (bDualTest)
	{
		bSuccess[0] = pDoc->m_pReelMapInnerUp->m_bRtnThreadRemakeReelmap;
		bSuccess[1] = pDoc->m_pReelMapInnerDn->m_bRtnThreadRemakeReelmap;
		bSuccess[2] = pDoc->m_pReelMapInnerAllUp->m_bRtnThreadRemakeReelmap;
		bSuccess[3] = pDoc->m_pReelMapIts->m_bRtnThreadRemakeReelmap;

		if (!bSuccess[0] || !bSuccess[1] || !bSuccess[2] || !bSuccess[3])
		{
			MsgBox(_T("ReelMap Converting Failed."));
			return FALSE;
		}
	}
	else
	{
		bSuccess[0] = pDoc->m_pReelMapInnerUp->m_bRtnThreadRemakeReelmap;
		bSuccess[1] = pDoc->m_pReelMapIts->m_bRtnThreadRemakeReelmap;

		if (!bSuccess[0] || !bSuccess[1])
		{
			MsgBox(_T("ReelMap Converting Failed."));
			return FALSE;
		}
	}

	return TRUE;
}

void CGvisR2R_PunchView::GetResult()
{
	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;

	//if (pDoc->m_pReelMapUp)
	//	pDoc->m_pReelMapUp->GetResult();
	if (bDualTest)
	{
		if (pDoc->m_pReelMapAllUp)
			pDoc->m_pReelMapAllUp->GetResult();
		//if (pDoc->m_pReelMapDn)
		//	pDoc->m_pReelMapDn->GetResult();
		//if (pDoc->m_pReelMapAllDn)
		//	pDoc->m_pReelMapAllDn->GetResult();
	}
	else
	{
		if (pDoc->m_pReelMapUp)
			pDoc->m_pReelMapUp->GetResult();
	}

	if (pDoc->GetTestMode() == MODE_OUTER)
	{
		//if (pDoc->m_pReelMapInnerUp)
		//	pDoc->m_pReelMapInnerUp->GetResult();
		if (pDoc->m_pReelMapIts)
			pDoc->m_pReelMapIts->GetResult();

		//if (pDoc->WorkingInfo.LastJob.bDualTestInner)
		//{
		//	if (pDoc->m_pReelMapInnerDn)
		//		pDoc->m_pReelMapInnerDn->GetResult();
		//	if (pDoc->m_pReelMapInnerAllUp)
		//		pDoc->m_pReelMapInnerAllUp->GetResult();
		//	if (pDoc->m_pReelMapInnerAllDn)
		//		pDoc->m_pReelMapInnerAllDn->GetResult();
		//}
	}
}

void CGvisR2R_PunchView::MakeResult()
{	
	// TODO: Add your control notification handler code here
	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;

	// File Save......

	CString strDestPath;
	strDestPath.Format(_T("%s%s\\%s\\%s.txt"), pDoc->WorkingInfo.System.sPathOldFile,
		pDoc->WorkingInfo.LastJob.sModelUp,
		pDoc->WorkingInfo.LastJob.sLotUp,
		_T("Result"));

	CFileFind cFile;
	if (cFile.FindFile(strDestPath))
		DeleteFile(strDestPath);

	//////////////////////////////////////////////////////////
	// Directory location of Work file
	CString strData;
	//strData = m_pDlgMenu05->TxtDataMDS();
	if (bDualTest)
	{
		if (pDoc->m_pReelMapAllUp)
			strData = pDoc->m_pReelMapAllUp->GetResultTxt();
	}
	else
	{
		if (pDoc->m_pReelMapUp)
			strData = pDoc->m_pReelMapUp->GetResultTxt();
	}


	TCHAR lpszCurDirPathFile[MAX_PATH];
	_stprintf(lpszCurDirPathFile, _T("%s"), strDestPath);

	CFile file;
	CFileException pError;
	if (!file.Open(lpszCurDirPathFile, CFile::modeWrite, &pError))
	{
		if (!file.Open(lpszCurDirPathFile, CFile::modeCreate | CFile::modeWrite, &pError))
		{
			// ���� ���¿� ���н� 
#ifdef _DEBUG
			afxDump << _T("File could not be opened ") << pError.m_cause << _T("\n");
#endif
			return;
		}
	}
	//������ ������ file�� �����Ѵ�.
	char* pRtn = NULL;
	file.SeekToBegin();
	file.Write(pRtn = StringToChar(strData), strData.GetLength());
	file.Close();
	if (pRtn)
		delete pRtn;
}

void CGvisR2R_PunchView::MakeResultIts()
{
	// TODO: Add your control notification handler code here
	if (pDoc->GetTestMode() != MODE_OUTER)
		return;

	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTestInner;

	// File Save......

	CString strDestPath;
	strDestPath.Format(_T("%s%s\\%s\\%s.txt"), pDoc->WorkingInfo.System.sPathItsFile,
		pDoc->WorkingInfo.LastJob.sModelUp,
		pDoc->WorkingInfo.LastJob.sEngItsCode,
		_T("Result"));

	CFileFind cFile;
	if (cFile.FindFile(strDestPath))
		DeleteFile(strDestPath);

	//////////////////////////////////////////////////////////
	// Directory location of Work file
	CString strData;
	if (pDoc->m_pReelMapIts)
		strData = pDoc->m_pReelMapIts->GetResultTxt();


	TCHAR lpszCurDirPathFile[MAX_PATH];
	_stprintf(lpszCurDirPathFile, _T("%s"), strDestPath);

	CFile file;
	CFileException pError;
	if (!file.Open(lpszCurDirPathFile, CFile::modeWrite, &pError))
	{
		if (!file.Open(lpszCurDirPathFile, CFile::modeCreate | CFile::modeWrite, &pError))
		{
			// ���� ���¿� ���н� 
#ifdef _DEBUG
			afxDump << _T("File could not be opened ") << pError.m_cause << _T("\n");
#endif
			return;
		}
	}
	//������ ������ file�� �����Ѵ�.
	char* pRtn = NULL;
	file.SeekToBegin();
	file.Write(pRtn = StringToChar(strData), strData.GetLength());
	file.Close();
	if (pRtn)
		delete pRtn;
}

void CGvisR2R_PunchView::MakeSapp3()
{
	if (pDoc->WorkingInfo.LastJob.sProcessNum.IsEmpty() || pDoc->WorkingInfo.System.sPathSapp3.IsEmpty())
		return;

	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;

	FILE *fp = NULL;
	TCHAR FileName[MAX_PATH];
	CString sPath, sVal, strMsg, sSapp3Data;

	if (bDualTest)
	{
		if (pDoc->m_pReelMapAllUp)
			sSapp3Data = pDoc->m_pReelMapAllUp->GetSapp3Txt();
	}
	else
	{
		if (pDoc->m_pReelMapUp)
			sSapp3Data = pDoc->m_pReelMapUp->GetSapp3Txt();
	}

	sPath.Format(_T("%s%9s_%4s_%5s.txt"), pDoc->WorkingInfo.System.sPathSapp3,
		pDoc->WorkingInfo.LastJob.sLotUp,
		pDoc->WorkingInfo.LastJob.sProcessNum,
		pDoc->WorkingInfo.System.sMcName);

	_stprintf(FileName, _T("%s"), sPath);
	char* pRtn = NULL;
	fp = fopen(pRtn = TCHARToChar(FileName), "w+");
	if (pRtn) delete pRtn;
	pRtn = NULL;

	if (fp != NULL)
	{
		fprintf(fp, "%s\n", pRtn = StringToChar(sSapp3Data));
	}
	else
	{
		strMsg.Format(_T("It is trouble to open file.\r\n%s"), sPath);
		pView->ClrDispMsg();
		AfxMessageBox(strMsg, MB_ICONWARNING | MB_OK);
	}

	if (pRtn)
		delete pRtn;
	fclose(fp);
}

BOOL CGvisR2R_PunchView::UpdateReelmap(int nSerial)
{
	if (nSerial <= 0)
	{
		pView->ClrDispMsg();
		AfxMessageBox(_T("Serial Error.54"));
		return 0;
	}

	if (!pDoc->MakeMkDir())
		return FALSE;

	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;

	m_nSerialRmapUpdate = nSerial;

	m_bTHREAD_UPDATE_REELMAP_UP = TRUE;
	if (bDualTest)
	{
		m_bTHREAD_UPDATE_REELMAP_DN = TRUE;
		m_bTHREAD_UPDATE_REELMAP_ALLUP = TRUE;
		m_bTHREAD_UPDATE_REELMAP_ALLDN = TRUE;
	}

	//if (pDoc->GetTestMode() == MODE_INNER || pDoc->GetTestMode() == MODE_OUTER)
	//	UpdateReelmapInner(nSerial);

	Sleep(100);
	return TRUE;
}

BOOL CGvisR2R_PunchView::UpdateReelmapInner(int nSerial)
{
	if (nSerial <= 0)
	{
		pView->ClrDispMsg();
		AfxMessageBox(_T("Serial Error.54"));
		return 0;
	}

	if (!pDoc->MakeMkDir())
		return FALSE;

	//BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTestInner;
	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;

	m_nSerialRmapInnerUpdate = nSerial;

	m_bTHREAD_UPDATE_REELMAP_INNER_UP = TRUE;
	if (bDualTest)
	{
		m_bTHREAD_UPDATE_REELMAP_INNER_DN = TRUE;
		m_bTHREAD_UPDATE_REELMAP_INNER_ALLUP = TRUE;
		m_bTHREAD_UPDATE_REELMAP_INNER_ALLDN = TRUE;
	}

	if(pDoc->GetTestMode() == MODE_OUTER)
		m_bTHREAD_UPDATE_REELMAP_ITS = TRUE;

	Sleep(100);
	return TRUE;
}

void CGvisR2R_PunchView::Shift2Mk()
{
	//BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;
	int nSerialL = m_nBufUpSerial[0];
	int nSerialR = m_nBufUpSerial[1];
	int nLastShot = 0;

	if (m_bLastProc && nSerialL == m_nLotEndSerial)
	{
		if (nSerialL > 0)
			pDoc->Shift2Mk(nSerialL);	// Cam0

		if (m_pDlgFrameHigh)
			m_pDlgFrameHigh->SetMkLastShot(nSerialL);

		//pDoc->UpdateYieldOnRmap(); // 20230614
	}
	else
	{
		if (nSerialL > 0)
		{
			pDoc->Shift2Mk(nSerialL);	// Cam0
			nLastShot = nSerialL;

			if (nSerialR > 0)
			{
				pDoc->Shift2Mk(nSerialR);	// Cam1
				nLastShot = nSerialR;
			}

			if (m_pDlgFrameHigh)
				m_pDlgFrameHigh->SetMkLastShot(nLastShot);

			//pDoc->UpdateYieldOnRmap(); // 20230614
		}
	}

	m_bShift2Mk = FALSE;
}

void CGvisR2R_PunchView::DelOverLotEndSerialUp(int nSerial)
{
	CString sSrc;

	if (nSerial > 0)
	{
		sSrc.Format(_T("%s%04d.pcr"), pDoc->WorkingInfo.System.sPathVrsBufUp, nSerial);

		if (pView->m_bSerialDecrese)
		{
			if (m_nLotEndSerial > 0 && nSerial < m_nLotEndSerial)
			{
				// Delete PCR File
				pDoc->m_pFile->DeleteFolerOrFile(sSrc);
			}
		}
		else
		{
			if (m_nLotEndSerial > 0 && nSerial > m_nLotEndSerial)
			{
				// Delete PCR File
				pDoc->m_pFile->DeleteFolerOrFile(sSrc);
			}
		}
	}

}


void CGvisR2R_PunchView::DelOverLotEndSerialDn(int nSerial)
{
	CString sSrc;

	if (nSerial > 0)
	{
		sSrc.Format(_T("%s%04d.pcr"), pDoc->WorkingInfo.System.sPathVrsBufDn, nSerial);

		if (pView->m_bSerialDecrese)
		{
			if (m_nLotEndSerial > 0 && nSerial < m_nLotEndSerial)
			{
				// Delete PCR File
				pDoc->m_pFile->DeleteFolerOrFile(sSrc);
			}
		}
		else
		{
			if (m_nLotEndSerial > 0 && nSerial > m_nLotEndSerial)
			{
				// Delete PCR File
				pDoc->m_pFile->DeleteFolerOrFile(sSrc);
			}
		}
	}

}



void CGvisR2R_PunchView::ChkReTestAlarmOnAoiUp()
{
	CString sMsg;
	sMsg.Format(_T("U%03d"), GetAoiUpAutoSerial());
	DispStsBar(sMsg, 0);

	int nSerial = m_pBufSerial[0][m_nBufTot[0] - 1];

	if (pView->m_bSerialDecrese)
	{
		if (m_nLotEndSerial > 0 && nSerial > m_nLotEndSerial)
		{
			SetAoiUpAutoStep(2); // Wait for AOI �˻���� ��ȣ.
			Sleep(300);
			if (m_pMpe)
				m_pMpe->Write(_T("MB44013B"), 1); // �˻�� ��� ���۾� (���۽�ȣ) : PC�� On��Ű�� PLC�� Off : PLC�� ó������ �ٽ� ����
			//pDoc->LogAuto(_T("PC: �˻�� ��� ���۾� (���۽�ȣ) : PC�� On��Ű�� PLC�� Off"));
		}
		else if(m_nLotEndSerial > 0 && nSerial <= m_nLotEndSerial)
		{
			if (m_pMpe)
				m_pMpe->Write(_T("MB44012B"), 1); // AOI �� : PCR���� Received
		}
	}
	else
	{
		if (m_nLotEndSerial > 0 && nSerial < m_nLotEndSerial)
		{
			SetAoiUpAutoStep(2); // Wait for AOI �˻���� ��ȣ.
			Sleep(300);
			if (m_pMpe)
				m_pMpe->Write(_T("MB44013B"), 1); // �˻�� ��� ���۾� (���۽�ȣ) : PC�� On��Ű�� PLC�� Off : PLC�� ó������ �ٽ� ����
			//pDoc->LogAuto(_T("PC: �˻�� ��� ���۾� (���۽�ȣ) : PC�� On��Ű�� PLC�� Off"));
		}
		else if (m_nLotEndSerial > 0 && nSerial >= m_nLotEndSerial)
		{
			if (m_pMpe)
				m_pMpe->Write(_T("MB44012B"), 1); // AOI �� : PCR���� Received
			//pDoc->LogAuto(_T("PC: �˻�� ��� ���۾� (���۽�ȣ) PCR���� Received : PC�� On��Ű�� PLC�� Off"));
		}
	}

}


void CGvisR2R_PunchView::ChkReTestAlarmOnAoiDn()
{
	CString sMsg;
	sMsg.Format(_T("D%03d"), GetAoiDnAutoSerial());
	DispStsBar(sMsg, 0);

	int nSerial = m_pBufSerial[1][m_nBufTot[1] - 1];

	if (pView->m_bSerialDecrese)
	{
		if (m_nLotEndSerial > 0 && nSerial > m_nLotEndSerial)
		{
			SetAoiDnAutoStep(2); // Wait for AOI �˻���� ��ȣ.
			Sleep(300);
			if (m_pMpe)
				m_pMpe->Write(_T("MB44013C"), 1); // �˻�� �Ϻ� ���۾� (���۽�ȣ) : PC�� On��Ű�� PLC�� Off : PLC�� ó������ �ٽ� ����
			//pDoc->LogAuto(_T("PC: �˻�� �Ϻ� ���۾� (���۽�ȣ) : PC�� On��Ű�� PLC�� Off"));
		}
		else if(m_nLotEndSerial > 0 && nSerial <= m_nLotEndSerial)
		{
			//if (m_pMpe)
			//	m_pMpe->Write(_T("MB44012C"), 1); // AOI �� : PCR���� Received
		}
	}
	else
	{
		if (m_nLotEndSerial > 0 && nSerial < m_nLotEndSerial)
		{
				SetAoiDnAutoStep(2); // Wait for AOI �˻���� ��ȣ.
				Sleep(300);
				if (m_pMpe)
					m_pMpe->Write(_T("MB44013C"), 1); // �˻�� �Ϻ� ���۾� (���۽�ȣ) : PC�� On��Ű�� PLC�� Off : PLC�� ó������ �ٽ� ����
				//pDoc->LogAuto(_T("PC: �˻�� �Ϻ� ���۾� (���۽�ȣ) : PC�� On��Ű�� PLC�� Off"));
		}
		else if (m_nLotEndSerial > 0 && nSerial >= m_nLotEndSerial)
		{
			//if (m_pMpe)
			//	m_pMpe->Write(_T("MB44012C"), 1); // AOI �� : PCR���� Received
		}
	}

}

void CGvisR2R_PunchView::ShowMsgBox()
{
	if (m_pDlgMyMsg)
	{
		m_pDlgMyMsg->ShowWindow(SW_SHOW);
		m_pDlgMyMsg->SetForegroundWindow();
		m_pDlgMyMsg->RedrawWindow();
	}
}

BOOL CGvisR2R_PunchView::FinalCopyItsFiles()
{
	if (pDoc->m_pReelMapUp)
		pDoc->m_pReelMapUp->StartThreadFinalCopyItsFiles();

	return TRUE;
}

int CGvisR2R_PunchView::GetAoiUpCamMstInfo()
{
	return pDoc->GetAoiUpCamMstInfo();
}

int CGvisR2R_PunchView::GetAoiDnCamMstInfo()
{
	return pDoc->GetAoiDnCamMstInfo();
}

void CGvisR2R_PunchView::DuplicateRmap(int nRmap)
{
	CFileFind finder;
	CString strTemp, sSrcPath, sDstPath;
	sDstPath = _T("C:\\PunchWork\\Reelmap.txt");

	switch (nRmap)
	{
	case RMAP_UP:
		sSrcPath = pDoc->m_pReelMapUp->GetRmapPath(RMAP_UP);
		break;
	case RMAP_ALLUP:
		sSrcPath = pDoc->m_pReelMapAllUp->GetRmapPath(RMAP_ALLUP);
		break;
	case RMAP_ITS:
		sSrcPath = pDoc->m_pReelMapUp->GetRmapPath(RMAP_ITS);
		break;
	}

	if (finder.FindFile(sSrcPath))
	{
		if (!CopyFile((LPCTSTR)sSrcPath, (LPCTSTR)sDstPath, FALSE))
		{
			Sleep(100);
			if (!CopyFile((LPCTSTR)sSrcPath, (LPCTSTR)sDstPath, FALSE))
			{
				Sleep(300);
				if (!CopyFile((LPCTSTR)sSrcPath, (LPCTSTR)sDstPath, FALSE))
				{
					Sleep(500);
					//strTemp.Format(_T("%s \r\n: Reelmap File Copy Fail"), sSrcPath);
					//pView->MsgBox(strTemp);
					if (!CopyFile((LPCTSTR)sSrcPath, (LPCTSTR)sDstPath, FALSE))
					{
						return;
					}
				}
			}					
		}
	}
	else
	{
		Sleep(30);
		strTemp.Format(_T("%s \r\n: Reelmap File Not Exist"), sSrcPath);
		//AfxMessageBox(strTemp);
		pView->MsgBox(strTemp);
		return;
	}

}

int CGvisR2R_PunchView::GetMkStAuto()
{
	int nStep = 0;
	TCHAR szData[512];

	CString strFolder, strTemp;
	strFolder.Format(_T("%s\\AutoStep.ini"), pDoc->WorkingInfo.System.sPathMkWork);

	if (0 < ::GetPrivateProfileString(_T("Auto"), _T("nStep"), NULL, szData, sizeof(szData), strFolder))
		nStep = _ttoi(szData);
	else
		nStep = 0;

	m_nMkStAuto = nStep;

	return nStep;
}

void CGvisR2R_PunchView::SetMkStAuto()
{
	CString strFolder, strTemp;
	strFolder.Format(_T("%s\\AutoStep.ini"), pDoc->WorkingInfo.System.sPathMkWork);
	strTemp.Format(_T("%d"), m_nMkStAuto);
	::WritePrivateProfileString(_T("Auto"), _T("nStep"), strTemp, strFolder);
}

BOOL CGvisR2R_PunchView::GetMkStSignal()
{
	BOOL bMkSt = FALSE;
	TCHAR szData[512];

	CString strFolder, strTemp;
	strFolder.Format(_T("%s\\AutoStep.ini"), pDoc->WorkingInfo.System.sPathMkWork);

	if (0 < ::GetPrivateProfileString(_T("Auto"), _T("bMkStart"), NULL, szData, sizeof(szData), strFolder))
		bMkSt = _ttoi(szData) ? TRUE : FALSE;
	else
		bMkSt = FALSE;

	m_bMkSt = bMkSt;

	return bMkSt;
}

void CGvisR2R_PunchView::SetMkStSignal()
{
	CString strFolder, strTemp;
	strFolder.Format(_T("%s\\AutoStep.ini"), pDoc->WorkingInfo.System.sPathMkWork);
	strTemp.Format(_T("%d"), m_bMkSt ? 1 : 0);
	::WritePrivateProfileString(_T("Auto"), _T("bMkStart"), strTemp, strFolder);
}

void CGvisR2R_PunchView::LoadSerial()
{
	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;

	if (bDualTest)
	{
		if (pDoc->m_ListBuf[1].nTot > 0) // AOI-Dn
		{
			m_nBufDnSerial[0] = pDoc->m_ListBuf[1].Pop();
			m_nBufUpSerial[0] = m_nBufDnSerial[0];
			if (pDoc->m_ListBuf[1].nTot > 0) // AOI-Dn
			{
				m_nBufDnSerial[1] = pDoc->m_ListBuf[1].Pop();
				m_nBufUpSerial[1] = m_nBufDnSerial[1];
			}
			else
			{
				m_nBufDnSerial[1] = 0;
				m_nBufUpSerial[1] = 0;
			}
		}
		else
		{
			m_bLotEnd = TRUE;
			m_nLotEndAuto = LOT_END;
		}

		if (pDoc->WorkingInfo.LastJob.bSampleTest)
		{
			if (m_nBufUpSerial[0] == 1)
			{
				m_nLotEndSerial = _tstoi(pDoc->WorkingInfo.LastJob.sSampleTestShotNum);
				m_bLastProcFromUp = FALSE;
				m_bLastProcFromEng = FALSE;
				m_bLastProc = TRUE;
				if (m_pDlgMenu01)
					m_pDlgMenu01->m_bLastProc = TRUE;
				if (m_pMpe)
				{
					m_pMpe->Write(_T("MB440186"), 1);			// �ܷ�ó�� AOI(��) ����(PC�� On��Ű��, PLC�� Ȯ���ϰ� Off��Ŵ)-20141112
					m_pMpe->Write(_T("MB440181"), 1);			// �ܷ�ó��(PC�� On��Ű��, PLC�� Ȯ���ϰ� Off��Ŵ)-20141031
				}
			}
		}
	}
	else
	{
		if (pDoc->m_ListBuf[0].nTot > 0) // AOI-Up
		{
			m_nBufUpSerial[0] = pDoc->m_ListBuf[0].Pop();
			if (pDoc->m_ListBuf[0].nTot > 0) // AOI-Up
				m_nBufUpSerial[1] = pDoc->m_ListBuf[0].Pop();
			else
				m_nBufUpSerial[1] = 0;
		}
		else
		{
			m_bLotEnd = TRUE;
			m_nLotEndAuto = LOT_END;
		}

		if (pDoc->WorkingInfo.LastJob.bSampleTest)
		{
			if (m_nBufUpSerial[0] == 1)
			{
				m_nLotEndSerial = _tstoi(pDoc->WorkingInfo.LastJob.sSampleTestShotNum);
				m_bLastProcFromUp = FALSE;
				m_bLastProcFromEng = FALSE;
				m_bLastProc = TRUE;
				if (m_pDlgMenu01)
					m_pDlgMenu01->m_bLastProc = TRUE;
				if (m_pMpe)
				{
					m_pMpe->Write(_T("MB440186"), 1);			// �ܷ�ó�� AOI(��) ����(PC�� On��Ű��, PLC�� Ȯ���ϰ� Off��Ŵ)-20141112
					m_pMpe->Write(_T("MB440181"), 1);			// �ܷ�ó��(PC�� On��Ű��, PLC�� Ȯ���ϰ� Off��Ŵ)-20141031
				}
			}
		}
	}
}


UINT CGvisR2R_PunchView::ThreadProc30(LPVOID lpContext)	// UpdateYieldUp()
{
	// Turn the passed in 'this' pointer back into a CProgressMgr instance
	CGvisR2R_PunchView* pThread = reinterpret_cast<CGvisR2R_PunchView*>(lpContext);

	BOOL bLock = FALSE;
	DWORD dwTick = GetTickCount();
	DWORD dwShutdownEventCheckPeriod = 0; // thread shutdown event check period

	pThread->m_bThread[30] = TRUE;
	while (WAIT_OBJECT_0 != WaitForSingleObject(pThread->m_Thread[30].GetShutdownEvent(), dwShutdownEventCheckPeriod))
	{
		pThread->m_dwThreadTick[30] = GetTickCount() - dwTick;
		dwTick = GetTickCount();

		if (pThread->m_bTHREAD_UPDATE_YIELD_UP)
		{
			pThread->UpdateYieldUp(pThread->m_nSnTHREAD_UPDATAE_YIELD);
			pThread->m_bTHREAD_UPDATE_YIELD_UP = FALSE;
			Sleep(0);
		}
		else
			Sleep(30);
	}

	pThread->m_bThread[30] = FALSE;

	return 0;
}

UINT CGvisR2R_PunchView::ThreadProc31(LPVOID lpContext)	// UpdateYieldUp()
{
	// Turn the passed in 'this' pointer back into a CProgressMgr instance
	CGvisR2R_PunchView* pThread = reinterpret_cast<CGvisR2R_PunchView*>(lpContext);

	BOOL bLock = FALSE;
	DWORD dwTick = GetTickCount();
	DWORD dwShutdownEventCheckPeriod = 0; // thread shutdown event check period

	pThread->m_bThread[31] = TRUE;
	while (WAIT_OBJECT_0 != WaitForSingleObject(pThread->m_Thread[31].GetShutdownEvent(), dwShutdownEventCheckPeriod))
	{
		pThread->m_dwThreadTick[31] = GetTickCount() - dwTick;
		dwTick = GetTickCount();

		if (pThread->m_bTHREAD_UPDATE_YIELD_DN)
		{
			pThread->UpdateYieldDn(pThread->m_nSnTHREAD_UPDATAE_YIELD);
			pThread->m_bTHREAD_UPDATE_YIELD_DN = FALSE;
			pThread->m_bTHREAD_UPDATE_YIELD_ALLUP = TRUE;
			pThread->m_bTHREAD_UPDATE_YIELD_ALLDN = TRUE;
			Sleep(0);
		}
		else
			Sleep(30);
	}

	pThread->m_bThread[31] = FALSE;

	return 0;
}

UINT CGvisR2R_PunchView::ThreadProc32(LPVOID lpContext)	// UpdateYieldUp()
{
	// Turn the passed in 'this' pointer back into a CProgressMgr instance
	CGvisR2R_PunchView* pThread = reinterpret_cast<CGvisR2R_PunchView*>(lpContext);

	BOOL bLock = FALSE;
	DWORD dwTick = GetTickCount();
	DWORD dwShutdownEventCheckPeriod = 0; // thread shutdown event check period

	pThread->m_bThread[32] = TRUE;
	while (WAIT_OBJECT_0 != WaitForSingleObject(pThread->m_Thread[32].GetShutdownEvent(), dwShutdownEventCheckPeriod))
	{
		pThread->m_dwThreadTick[32] = GetTickCount() - dwTick;
		dwTick = GetTickCount();

		if (pThread->m_bTHREAD_UPDATE_YIELD_ALLUP)
		{
			pThread->UpdateYieldAllUp(pThread->m_nSnTHREAD_UPDATAE_YIELD);
			pThread->m_bTHREAD_UPDATE_YIELD_ALLUP = FALSE;

			if (pDoc->GetTestMode() == MODE_OUTER)
			{
				pThread->m_bTHREAD_UPDATE_YIELD_ITS = TRUE;
				//pThread->m_bTHREAD_UPDATE_YIELD_INNER_UP = TRUE;
				//if (pDoc->WorkingInfo.LastJob.bDualTestInner)
				//{
				//	pThread->m_bTHREAD_UPDATE_YIELD_INNER_DN = TRUE;
				//}
			}

			Sleep(0);
		}
		else
			Sleep(30);
	}

	pThread->m_bThread[32] = FALSE;

	return 0;
}

UINT CGvisR2R_PunchView::ThreadProc33(LPVOID lpContext)	// UpdateYieldUp()
{
	// Turn the passed in 'this' pointer back into a CProgressMgr instance
	CGvisR2R_PunchView* pThread = reinterpret_cast<CGvisR2R_PunchView*>(lpContext);

	BOOL bLock = FALSE;
	DWORD dwTick = GetTickCount();
	DWORD dwShutdownEventCheckPeriod = 0; // thread shutdown event check period

	pThread->m_bThread[33] = TRUE;
	while (WAIT_OBJECT_0 != WaitForSingleObject(pThread->m_Thread[33].GetShutdownEvent(), dwShutdownEventCheckPeriod))
	{
		pThread->m_dwThreadTick[33] = GetTickCount() - dwTick;
		dwTick = GetTickCount();

		if (pThread->m_bTHREAD_UPDATE_YIELD_ALLDN)
		{
			pThread->UpdateYieldAllDn(pThread->m_nSnTHREAD_UPDATAE_YIELD);
			pThread->m_bTHREAD_UPDATE_YIELD_ALLDN = FALSE;
			Sleep(0);
		}
		else
			Sleep(30);
	}

	pThread->m_bThread[33] = FALSE;

	return 0;
}

UINT CGvisR2R_PunchView::ThreadProc34(LPVOID lpContext)	// UpdateYieldUp()
{
	// Turn the passed in 'this' pointer back into a CProgressMgr instance
	CGvisR2R_PunchView* pThread = reinterpret_cast<CGvisR2R_PunchView*>(lpContext);

	BOOL bLock = FALSE;
	DWORD dwTick = GetTickCount();
	DWORD dwShutdownEventCheckPeriod = 0; // thread shutdown event check period

	pThread->m_bThread[34] = TRUE;
	while (WAIT_OBJECT_0 != WaitForSingleObject(pThread->m_Thread[34].GetShutdownEvent(), dwShutdownEventCheckPeriod))
	{
		pThread->m_dwThreadTick[34] = GetTickCount() - dwTick;
		dwTick = GetTickCount();

		if (pThread->m_bTHREAD_UPDATE_YIELD_INNER_UP)
		{
			pThread->UpdateYieldInnerUp(pThread->m_nSnTHREAD_UPDATAE_YIELD);
			pThread->m_bTHREAD_UPDATE_YIELD_INNER_UP = FALSE;
			Sleep(0);
		}
		else
			Sleep(30);
	}

	pThread->m_bThread[34] = FALSE;

	return 0;
}

UINT CGvisR2R_PunchView::ThreadProc35(LPVOID lpContext)	// UpdateYieldUp()
{
	// Turn the passed in 'this' pointer back into a CProgressMgr instance
	CGvisR2R_PunchView* pThread = reinterpret_cast<CGvisR2R_PunchView*>(lpContext);

	BOOL bLock = FALSE;
	DWORD dwTick = GetTickCount();
	DWORD dwShutdownEventCheckPeriod = 0; // thread shutdown event check period

	pThread->m_bThread[35] = TRUE;
	while (WAIT_OBJECT_0 != WaitForSingleObject(pThread->m_Thread[35].GetShutdownEvent(), dwShutdownEventCheckPeriod))
	{
		pThread->m_dwThreadTick[35] = GetTickCount() - dwTick;
		dwTick = GetTickCount();

		if (pThread->m_bTHREAD_UPDATE_YIELD_INNER_DN)
		{
			pThread->UpdateYieldInnerDn(pThread->m_nSnTHREAD_UPDATAE_YIELD);
			pThread->m_bTHREAD_UPDATE_YIELD_INNER_DN = FALSE;
			pThread->m_bTHREAD_UPDATE_YIELD_INNER_ALLUP = TRUE;
			pThread->m_bTHREAD_UPDATE_YIELD_INNER_ALLDN = TRUE;
			Sleep(0);
		}
		else
			Sleep(30);
	}

	pThread->m_bThread[35] = FALSE;

	return 0;
}

UINT CGvisR2R_PunchView::ThreadProc36(LPVOID lpContext)	// UpdateYieldUp()
{
	// Turn the passed in 'this' pointer back into a CProgressMgr instance
	CGvisR2R_PunchView* pThread = reinterpret_cast<CGvisR2R_PunchView*>(lpContext);

	BOOL bLock = FALSE;
	DWORD dwTick = GetTickCount();
	DWORD dwShutdownEventCheckPeriod = 0; // thread shutdown event check period

	pThread->m_bThread[36] = TRUE;
	while (WAIT_OBJECT_0 != WaitForSingleObject(pThread->m_Thread[36].GetShutdownEvent(), dwShutdownEventCheckPeriod))
	{
		pThread->m_dwThreadTick[36] = GetTickCount() - dwTick;
		dwTick = GetTickCount();

		if (pThread->m_bTHREAD_UPDATE_YIELD_INNER_ALLUP)
		{
			pThread->UpdateYieldInnerAllUp(pThread->m_nSnTHREAD_UPDATAE_YIELD);
			pThread->m_bTHREAD_UPDATE_YIELD_INNER_ALLUP = FALSE;
			Sleep(0);
		}
		else
			Sleep(30);
	}

	pThread->m_bThread[36] = FALSE;

	return 0;
}

UINT CGvisR2R_PunchView::ThreadProc37(LPVOID lpContext)	// UpdateYieldUp()
{
	// Turn the passed in 'this' pointer back into a CProgressMgr instance
	CGvisR2R_PunchView* pThread = reinterpret_cast<CGvisR2R_PunchView*>(lpContext);

	BOOL bLock = FALSE;
	DWORD dwTick = GetTickCount();
	DWORD dwShutdownEventCheckPeriod = 0; // thread shutdown event check period

	pThread->m_bThread[37] = TRUE;
	while (WAIT_OBJECT_0 != WaitForSingleObject(pThread->m_Thread[37].GetShutdownEvent(), dwShutdownEventCheckPeriod))
	{
		pThread->m_dwThreadTick[37] = GetTickCount() - dwTick;
		dwTick = GetTickCount();

		if (pThread->m_bTHREAD_UPDATE_YIELD_INNER_ALLDN)
		{
			pThread->UpdateYieldInnerAllDn(pThread->m_nSnTHREAD_UPDATAE_YIELD);
			pThread->m_bTHREAD_UPDATE_YIELD_INNER_ALLDN = FALSE;
			Sleep(0);
		}
		else
			Sleep(30);
	}

	pThread->m_bThread[37] = FALSE;

	return 0;
}

UINT CGvisR2R_PunchView::ThreadProc38(LPVOID lpContext)	// UpdateYieldUp()
{
	// Turn the passed in 'this' pointer back into a CProgressMgr instance
	CGvisR2R_PunchView* pThread = reinterpret_cast<CGvisR2R_PunchView*>(lpContext);

	BOOL bLock = FALSE;
	DWORD dwTick = GetTickCount();
	DWORD dwShutdownEventCheckPeriod = 0; // thread shutdown event check period

	pThread->m_bThread[38] = TRUE;
	while (WAIT_OBJECT_0 != WaitForSingleObject(pThread->m_Thread[38].GetShutdownEvent(), dwShutdownEventCheckPeriod))
	{
		pThread->m_dwThreadTick[38] = GetTickCount() - dwTick;
		dwTick = GetTickCount();

		if (pThread->m_bTHREAD_UPDATE_YIELD_ITS)
		{
			pThread->UpdateYieldIts(pThread->m_nSnTHREAD_UPDATAE_YIELD);
			pThread->m_bTHREAD_UPDATE_YIELD_ITS = FALSE;
			Sleep(0);
		}
		else
			Sleep(30);
	}

	pThread->m_bThread[38] = FALSE;

	return 0;
}

void CGvisR2R_PunchView::ChkRcvSig()
{
	long lData = 0;
	int i = 0;

	for (i = 0; i < _SigInx::_EndIdx; i++)
	{
		if (m_bRcvSig[i])
		{
			m_bRcvSig[i] = FALSE;
			switch (i)
			{
			case _SigInx::_Ready:
				if (m_pMpe)
					m_pMpe->Write(_T("MB440162"), (long)0); // ��ŷ�� ���� ����ġ ���� ON(PC�� On, PLC�� Ȯ���ϰ� Off��Ŵ)
				break;
			case _SigInx::_Run:
				if (m_pMpe)
					m_pMpe->Write(_T("MB440162"), (long)0); // ��ŷ�� ���� ����ġ ���� ON(PC�� On, PLC�� Ȯ���ϰ� Off��Ŵ)
				break;
			case _SigInx::_Reset:
				if (m_pMpe)
					m_pMpe->Write(_T("MB440162"), (long)0); // ��ŷ�� ���� ����ġ ���� ON(PC�� On, PLC�� Ȯ���ϰ� Off��Ŵ)
				break;
			case _SigInx::_Stop:
				if (m_pMpe)
					m_pMpe->Write(_T("MB440162"), (long)(pDoc->BtnStatus.Main.Stop ? 1 : 0)); // ��ŷ�� ���� ����ġ ���� ON(PC�� On, PLC�� Ȯ���ϰ� Off��Ŵ)
				break;
			case _SigInx::_MkTq:
				if (m_pMpe)
					m_pMpe->Write(_T("MB440155"), (long)(pDoc->BtnStatus.Tq.Mk ? 1 : 0)); // ��ŷ�� �ټ� ON (PC�� ON/OFF��Ŵ)
				break;
			case _SigInx::_AoiTq:
				if (m_pMpe)
					m_pMpe->Write(_T("MB440156"), (long)(pDoc->BtnStatus.Tq.Aoi ? 1 : 0)); // �˻��/���κ� �ټ� ON (PC�� ON/OFF��Ŵ)
				break;
			case _SigInx::_EngTq:
				if (m_pMpe)
					m_pMpe->Write(_T("MB440156"), (long)(pDoc->BtnStatus.Tq.Eng ? 1 : 0)); // �˻��/���κ� �ټ� ON (PC�� ON/OFF��Ŵ)
				break;
			case _SigInx::_CcwModRe:
				if (m_pMpe)
					m_pMpe->Write(_T("MB44017D"), (long)(pDoc->BtnStatus.Induct.Rc ? 1 : 0)); // ��ŷ(GUI) �����Ϸ� ������(Off->CW/On->CCW)
				break;
			case _SigInx::_CcwModUn:
				if (m_pMpe)
					m_pMpe->Write(_T("MB44017C"), (long)(pDoc->BtnStatus.Induct.Uc ? 1 : 0)); // ��ŷ(GUI) �����Ϸ� ������(Off->CW/On->CCW)
				break;
			case _SigInx::_Core150Re:
				if (m_pMpe)
					m_pMpe->Write(_T("MB44017E"), (long)(pDoc->BtnStatus.Core150.Rc ? 1 : 0)); // Marking GUI Recoiler Core 150[mm](PC On/Off)
				break;
			case _SigInx::_Core150Un:
				if (m_pMpe)
					m_pMpe->Write(_T("MB44017F"), (long)(pDoc->BtnStatus.Core150.Uc ? 1 : 0)); // Marking GUI Uncoiler Core 150[mm](PC On/Off)
				break;
			case _SigInx::_MvCwRe:
				if (m_pDlgMenu03)
					m_pDlgMenu03->SwMpeBtn(IDC_CHK_5, 1);	// �����Ϸ� ��ǰ�� ��ȸ�� ����ġ
				break;
			case _SigInx::_MvCcwRe:
				if (m_pDlgMenu03)
					m_pDlgMenu03->SwMpeBtn(IDC_CHK_6, 1);	// �����Ϸ� ��ǰ�� ��ȸ�� ����ġ
				break;
			case _SigInx::_PrdChuckRe:
				if (m_pDlgMenu03)
					m_pDlgMenu03->SwMpeBtn(IDC_CHK_41, 1);	// �����Ϸ� ��ǰô Ŭ���� ����ġ
				break;
			case _SigInx::_DancerUpRe:
				if (m_pDlgMenu03)
					m_pDlgMenu03->SwMpeBtn(IDC_CHK_42, 1);	// �����Ϸ� ���� ���/�ϰ� ����ġ
				break;
			case _SigInx::_PasteUpLfRe:
				if (m_pDlgMenu03)
					m_pDlgMenu03->SwMpeBtn(IDC_CHK_43, 1);	// �����Ϸ� ��ǰ ������(��/��) ����ġ
				break;
			case _SigInx::_PasteUpRtRe:
				if (m_pDlgMenu03)
					m_pDlgMenu03->SwMpeBtn(IDC_CHK_7, 1);	// �����Ϸ� ��ǰ ������(��/��) ����ġ
				break;
			case _SigInx::_PasteVacRe:
				if (m_pDlgMenu03)
					m_pDlgMenu03->SwMpeBtn(IDC_CHK_8, 1);	// �����Ϸ� ��ǰ ������ ���� ����ġ
				break;
			case _SigInx::_PprChuckRe:
				if (m_pDlgMenu03)
					m_pDlgMenu03->SwMpeBtn(IDC_CHK_44, 1);	// �����Ϸ� ����ô Ŭ���� ����ġ
				break;
			case _SigInx::_PprCwRe:
				if (m_pDlgMenu03)
					m_pDlgMenu03->SwMpeBtn(IDC_CHK_45, 1);	// �����Ϸ� ������ ��ȸ�� ����ġ
				break;
			case _SigInx::_PprCcwRe:
				if (m_pDlgMenu03)
					m_pDlgMenu03->SwMpeBtn(IDC_CHK_46, 1);	// �����Ϸ� ������ ��ȸ�� ����ġ
				break;
			case _SigInx::_DoRe:
				if (m_pDlgMenu03)
					m_pDlgMenu03->SwMpeBtn(IDC_CHK_66, 1);	// �����Ϸ� Rewinder ���� ����ġ
				break;
			case _SigInx::_PrdPprRe:
				if (m_pDlgMenu03)
					m_pDlgMenu03->SwMpeBtn(IDC_CHK_67, 1);	// �����Ϸ� Rewinder ��ǰ & ���� ����ġ
				break;
			case _SigInx::_Relation:
				if (m_pDlgMenu03)
					m_pDlgMenu03->SwMpeBtn(IDC_CHK_9, 1);	// ��ŷ�� ���� ��/���� ����ġ
				break;
			case _SigInx::_MvCwMk:
				if (m_pDlgMenu03)
					m_pDlgMenu03->SwMpeBtn(IDC_CHK_10, 1);	// ��ŷ�� �ǵ� ��ȸ�� ����ġ
				break;
			case _SigInx::_MvCcwMk:
				if (m_pDlgMenu03)
					m_pDlgMenu03->SwMpeBtn(IDC_CHK_11, 1);	// ��ŷ�� �ǵ� ��ȸ�� ����ġ
				break;
			case _SigInx::_FdVacMk:
				if (m_pDlgMenu03)
					m_pDlgMenu03->SwMpeBtn(IDC_CHK_12, 1);	// ��ŷ�� �ǵ� ���� ����ġ
				break;
			case _SigInx::_PushUpMk:
				if (m_pDlgMenu03)
					m_pDlgMenu03->SwMpeBtn(IDC_CHK_13, 1);	// ��ŷ�� ��ǰǪ�� ����ġ // (��ũ ���� ����ġ) - X
				break;
			case _SigInx::_TblBlwMk:
				if (m_pDlgMenu03)
					m_pDlgMenu03->SwMpeBtn(IDC_CHK_14, 1);	// ��ŷ�� ���̺� ��ο� ����ġ
				break;
			case _SigInx::_TblVacMk:
				if (m_pDlgMenu03)
					m_pDlgMenu03->SwMpeBtn(IDC_CHK_15, 1);	// ��ŷ�� ���̺� ���� ����ġ
				break;
			case _SigInx::_FdClampMk:
				if (m_pDlgMenu03)
					m_pDlgMenu03->SwMpeBtn(IDC_CHK_51, 1);	// ��ŷ�� �ǵ� Ŭ���� ����ġ
				break;
			case _SigInx::_TensClampMk:
				if (m_pDlgMenu03)
					m_pDlgMenu03->SwMpeBtn(IDC_CHK_52, 1);	// ��ŷ�� �ټ� Ŭ���� ����ġ
				break;
			case _SigInx::_OnePnlMk:
				if (m_pMpe)
					m_pMpe->Write(_T("MB440151"), (long)(pDoc->BtnStatus.Mk.MvOne ? 1: 0));
				break;
			case _SigInx::_DancerUpMk:
				if (m_pDlgMenu03)
					m_pDlgMenu03->SwMpeBtn(IDC_CHK_48, 1);	// ��ŷ�� ���� ���/�ϰ� ����ġ
				break;
			case _SigInx::_MvCwAoiDn:
				if (m_pDlgMenu03)
					m_pDlgMenu03->SwMpeBtn(IDC_CHK_56, 1);	// �˻�� �� �ǵ� ��ȸ�� ����ġ
				break;
			case _SigInx::_MvCcwAoiDn:
				if (m_pDlgMenu03)
					m_pDlgMenu03->SwMpeBtn(IDC_CHK_57, 1);	// �˻�� �� �ǵ� ��ȸ�� ����ġ
				break;
			case _SigInx::_FdVacAoiDn:
				if (m_pDlgMenu03)
					m_pDlgMenu03->SwMpeBtn(IDC_CHK_58, 1);	// �˻�� �� �ǵ� ���� ����ġ
				break;
			case _SigInx::_PushUpAoiDn:
				if (m_pDlgMenu03)
					m_pDlgMenu03->SwMpeBtn(IDC_CHK_59, 1);	// �˻�� �� ��ǰǪ�� ����ġ // (��ũ ���� ����ġ) - X
				break;
			case _SigInx::_TblBlwAoiDn:
				if (m_pDlgMenu03)
					m_pDlgMenu03->SwMpeBtn(IDC_CHK_60, 1);	// �˻�� �� ���̺� ��ο� ����ġ
				break;
			case _SigInx::_TblVacAoiDn:
				if (m_pDlgMenu03)
					m_pDlgMenu03->SwMpeBtn(IDC_CHK_61, 1);	// �˻�� �� ���̺� ���� ����ġ
				break;
			case _SigInx::_FdClampAoiDn:
				if (m_pDlgMenu03)
					m_pDlgMenu03->SwMpeBtn(IDC_CHK_64, 1);	// �˻�� �� �ǵ� Ŭ���� ����ġ
				break;
			case _SigInx::_TensClampAoiDn:
				if (m_pDlgMenu03)
					m_pDlgMenu03->SwMpeBtn(IDC_CHK_65, 1);	// �˻�� �� �ټ� Ŭ���� ����ġ
				break;
			case _SigInx::_OnePnlAoiDn:
				if (m_pMpe)
					m_pMpe->Write(_T("MB440151"), (long)(pDoc->BtnStatus.AoiDn.MvOne ? 1 : 0)); // ���ǳ� �̼ۻ��� ON (PC�� ON, OFF)
				break;
			case _SigInx::_VelClrSonicAoiDn:
				if (m_pDlgMenu03)
					m_pDlgMenu03->SwMpeBtn(IDC_CHK_88, 1);	// AOI(��) ������ ������ �ӵ� ON (PC�� ON/OFF��Ŵ)
				break;
			case _SigInx::_OnePnlAoiUp:
				if (m_pMpe)
					m_pMpe->Write(_T("MB440151"), (long)(pDoc->BtnStatus.AoiUp.MvOne ? 1 : 0)); // ���ǳ� �̼ۻ��� ON (PC�� ON, OFF)
				break;
			case _SigInx::_OnePnlEng:
				if (m_pMpe)
					m_pMpe->Write(_T("MB440151"), (long)(pDoc->BtnStatus.Eng.MvOne ? 1 : 0)); // ���ǳ� �̼ۻ��� ON (PC�� ON, OFF)
				break;
			case _SigInx::_VelClrSonicEng:
				if (m_pDlgMenu03)
					m_pDlgMenu03->SwMpeBtn(IDC_CHK_87, 1);	// ���κ� ������ ������ �ӵ� ON (PC�� ON/OFF��Ŵ)
				break;
			case _SigInx::_MyMsgYes:
				SetMyMsgYes();
				break;
			case _SigInx::_MyMsgNo:
				SetMyMsgNo();
				break;
			case _SigInx::_MyMsgOk:
				SetMyMsgOk();
				break;
			case _SigInx::_DualTest:
				SetDualTest(pDoc->WorkingInfo.LastJob.bDualTest);
				break;
			case _SigInx::_SampleTest:
				if(m_pMpe)
					m_pMpe->Write(_T("MB44017B"), (pDoc->WorkingInfo.LastJob.bSampleTest) ? 1 : 0);		// Sample �˻� On
				break;
			case _SigInx::_TestMode:
				pDoc->SetTestMode(pDoc->WorkingInfo.LastJob.nTestMode); // MODE_NONE = 0, MODE_INNER = 1, MODE_OUTER = 2
				break;
			case _SigInx::_RecoilerCcw:
				if (m_pMpe)
					m_pMpe->Write(_T("MB44017D"), (long)(pDoc->BtnStatus.Induct.Rc ? 1 : 0));
				::WritePrivateProfileString(_T("Last Job"), _T("One Metal On"), pDoc->BtnStatus.Induct.Rc ? _T("1") : _T("0"), PATH_WORKING_INFO);// IDC_CHK_ONE_METAL - Recoiler\r������ CW : FALSE
				break;
			case _SigInx::_UncoilerCcw:
				if (m_pMpe)
					m_pMpe->Write(_T("MB44017C"), (long)(pDoc->BtnStatus.Induct.Uc ? 1 : 0));
				::WritePrivateProfileString(_T("Last Job"), _T("Two Metal On"), pDoc->BtnStatus.Induct.Uc ? _T("1") : _T("0"), PATH_WORKING_INFO);// IDC_CHK_ONE_METAL - Recoiler\r������ CW : FALSE
				break;
			case _SigInx::_DoorRecoiler:
				if (m_pMpe)
					m_pMpe->Write(_T("MB440163"), (long)(pDoc->WorkingInfo.LastJob.bRclDrSen ? 1 : 0));	// �����Ϸ�Door���� ���
				break;
			case _SigInx::_DoorAoiUp:
				if (m_pMpe)
					m_pMpe->Write(_T("MB440166"), (long)(pDoc->WorkingInfo.LastJob.bAoiUpDrSen ? 1 : 0));	// AOI(��) Door���� ���
				break;
			case _SigInx::_DoorAoiDn:
				if (m_pMpe)
					m_pMpe->Write(_T("MB440167"), (long)(pDoc->WorkingInfo.LastJob.bAoiDnDrSen ? 1 : 0));	// AOI(��) Door���� ���
				break;
			case _SigInx::_DoorMk:
				if (m_pMpe)
					m_pMpe->Write(_T("MB440164"), (long)(pDoc->WorkingInfo.LastJob.bMkDrSen ? 1 : 0));	// ��ŷDoor���� ���
				break;
			case _SigInx::_DoorEngrave:
				if (m_pMpe)
					m_pMpe->Write(_T("MB44019B"), (long)(pDoc->WorkingInfo.LastJob.bEngvDrSen ? 1 : 0));	// ���κ� Door���� ���
				break;
			case _SigInx::_DoorUncoiler:
				if (m_pMpe)
					m_pMpe->Write(_T("MB440168"), (long)(pDoc->WorkingInfo.LastJob.bUclDrSen ? 1 : 0));	// �����Ϸ�Door���� ���
				break;
			case _SigInx::_CleannerAoiUp:
				if (m_pMpe)
					m_pMpe->Write(_T("MB44010E"), (long)(pDoc->WorkingInfo.LastJob.bUseAoiUpCleanRoler ? 1 : 0));
				break;
			case _SigInx::_CleannerAoiDn:
				if (m_pMpe)
					m_pMpe->Write(_T("MB44010F"), (long)(pDoc->WorkingInfo.LastJob.bUseAoiDnCleanRoler ? 1 : 0));
				break;
			case _SigInx::_UltraSonicAoiDn:
				if (m_pMpe)
					m_pMpe->Write(_T("MB44016F"), (long)(pDoc->WorkingInfo.LastJob.bUseAoiDnUltrasonic ? 1 : 0));
				break;
			case _SigInx::_UltraSonicEngrave:
				if (m_pMpe)
					m_pMpe->Write(_T("MB44016E"), (long)(pDoc->WorkingInfo.LastJob.bUseEngraveUltrasonic ? 1 : 0));
				break;
			case _SigInx::_Use380mm:
				if (m_pMpe)
					m_pMpe->Write(_T("MB440177"), (long)(pDoc->WorkingInfo.LastJob.bUse380mm ? 1 : 0));	// EPC�Ǹ���(��ǰ��->OFF/��ǰ��->ON)
				break;
			case _SigInx::_TempPause:
				if (m_pMpe)
					m_pMpe->Write(_T("MB440183"), (long)(pDoc->WorkingInfo.LastJob.bTempPause ? 1 : 0));	// �Ͻ��������(PC�� On��Ű��, PLC�� Ȯ���ϰ� Off��Ŵ)
				::WritePrivateProfileString(_T("Last Job"), _T("Use Temporary Pause"), pDoc->WorkingInfo.LastJob.bTempPause ? _T("1") : _T("0"), PATH_WORKING_INFO);
				break;
			case _SigInx::_2DOffsetInitPos:
				lData = (long)(pDoc->GetOffsetInitPos() * 1000.0); // WorkingInfo.Motion.sOffsetInitPos
				if (m_pMpe)
					m_pMpe->Write(_T("ML44046"), lData);	// ���κ�, �˻��, ��ŷ�� offset �̼� �� (���� mm * 1000)
				break;
			case _SigInx::_2DOffsetInitPosMove:
				if (m_pMpe)
					m_pMpe->Write(_T("MB44013F"), (long)1); // ���κ�, �˻��, ��ŷ�� offset �̼� ON(PC�� On��Ű��, PLC�� Ȯ���ϰ� Off��Ŵ)
				break;
			default:
				break;
			}
		}
	}
}


BOOL CGvisR2R_PunchView::RemakeReelmapFromPcr(CString sModel, CString sLot, CString sLayerUp, CString sLayerDn)
{
	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;
	CString sReelmapSrc, str, sMsg;

	if (!pDoc->m_pReelMapUp)
	{
		sMsg.Format(_T("m_pReelMapUp�� Create�����ʾҽ��ϴ�.\r\n%s"));
		pView->MsgBox(sMsg);
		return FALSE;
	}

	pDoc->WorkingInfo.LastJob.sModelUp = sModel;
	pDoc->WorkingInfo.LastJob.sLotUp = pDoc->WorkingInfo.LastJob.sLotDn = sLot;
	pDoc->WorkingInfo.LastJob.sLayerUp = sLayerUp;
	pDoc->WorkingInfo.LastJob.sLayerDn = sLayerDn;

	int nOffline = IsOfflineFolder(); // 0 : Not exist, 1 : Exist only Up, 2 : Exist only Dn, 3 : Exist Up and Dn

	if (nOffline == 0)
	{
		sMsg.Format(_T("OFFLINE ������ �������� �ʽ��ϴ�."));
		pView->MsgBox(sMsg);
		return FALSE;
	}

	//CopyRmapToOffline(nOffline);

	StartThreadRemakeRmapFromPcr();

	int nLastShot = GetLastShotFromPcr(nOffline);

	if (nOffline & 0x02)
	{
		bDualTest = TRUE;
		ResetMkInfo(2); // 0 : AOI-Up , 1 : AOI-Dn , 2 : AOI-UpDn
	}
	else
	{
		bDualTest = FALSE;
		ResetMkInfo(0); // 0 : AOI-Up , 1 : AOI-Dn , 2 : AOI-UpDn
	}

	ClrDispMsg();

	RemakeRmapFromPcr(nLastShot, nOffline);

	StopThreadRemakeRmapFromPcr();


	if (m_pDlgMenu05)
	{
		int nIndex = m_pDlgMenu05->GetIdxTopLayer();
		if (nIndex > -1)
		{
			m_pDlgMenu05->SelchangeComboLayer(nIndex);
		}
	}

	return TRUE;

	/*
	CString sPath = pDoc->m_pReelMapUp->GetRmapPath(RMAP_UP);

	FILE *fp = NULL;
	char FileName[MAX_PATH];
	CString strFileName, strPathName;
	CFileFind findfile;
	int nStripNumY, nPieceNumPerStrip;

	int i, nLastShot, nPnl, nRow, nCol, nDefCode, nCompletedShot;
	CString sPnl, sRow, sVal;
	TCHAR sep[] = { _T(",/;\r\n\t") };
	TCHAR szData[MAX_PATH];

	if (!findfile.FindFile(sPath))
	{
		sMsg.Format(_T("Reelmap�� �������� �ʽ��ϴ�.\r\n%s"), sPath);
		pView->MsgBox(sMsg);
		return FALSE;
	}

	CString sProcessCode, sEntireSpeed, sLotRun, sLotEnd;
	
	if (0 < ::GetPrivateProfileString(_T("Info"), _T("Completed Shot"), NULL, szData, sizeof(szData), sPath))
		nCompletedShot = _tstoi(szData);
	else
		nCompletedShot = 0; // Failed.
	
	if (0 < ::GetPrivateProfileString(_T("Info"), _T("Marked Shot"), NULL, szData, sizeof(szData), sPath))
		nLastShot = _tstoi(szData);
	else
	{
		nLastShot = 0; // Failed.
		pView->MsgBox(_T("���ʿ� Marked Shot ������ �����ϴ�."));
		return FALSE;
	}

	// �����ڵ�
	if (0 < ::GetPrivateProfileString(_T("Info"), _T("Process Code"), NULL, szData, sizeof(szData), sPath))
		sProcessCode = CString(szData);
	else
		sProcessCode = _T("");

	// �ӵ�
	if (0 < ::GetPrivateProfileString(_T("Info"), _T("Entire Speed"), NULL, szData, sizeof(szData), sPath))
		sEntireSpeed = CString(szData);
	else
		sEntireSpeed = _T("0.0");

	if (0 < ::GetPrivateProfileString(_T("Info"), _T("Lot Run"), NULL, szData, sizeof(szData), sPath))
		sLotRun = CString(szData);
	else
		sLotRun = _T("");

	if (0 < ::GetPrivateProfileString(_T("Info"), _T("Lot End"), NULL, szData, sizeof(szData), sPath))
		sLotEnd = CString(szData);
	else
		sLotEnd = _T("");


	CString sFile = _T(""), sUpPath = _T(""), sDnPath = _T(""), sRmapUpPath = sPath, sRmapDnPath;

	int nPos = sRmapUpPath.ReverseFind('\\');
	if (nPos != -1)
	{
		sFile = sRmapUpPath.Right(sRmapUpPath.GetLength() - nPos - 1);
		sRmapUpPath.Delete(nPos, sRmapUpPath.GetLength() - nPos);
	}


	DeleteFile(sPath);
	sPath = pDoc->m_pReelMapUp->GetYieldPath(RMAP_UP);
	DeleteFile(sPath);
	
	sPath = pDoc->m_pReelMapUp->GetRmapPath(RMAP_DN);
	if (findfile.FindFile(sPath))
	{
		bDualTest = TRUE;
		sRmapDnPath = sPath;

		DeleteFile(sPath);
		sPath = pDoc->m_pReelMapUp->GetYieldPath(RMAP_DN);
		DeleteFile(sPath);

		sPath = pDoc->m_pReelMapUp->GetRmapPath(RMAP_ALLUP);
		if (findfile.FindFile(sPath))
		{
			DeleteFile(sPath);
			sPath = pDoc->m_pReelMapUp->GetYieldPath(RMAP_ALLUP);
			DeleteFile(sPath);
		}

		sPath = pDoc->m_pReelMapUp->GetRmapPath(RMAP_ALLDN);
		if (findfile.FindFile(sPath))
		{
			DeleteFile(sPath);
			sPath = pDoc->m_pReelMapUp->GetYieldPath(RMAP_ALLDN);
			DeleteFile(sPath);
		}

		nPos = sRmapDnPath.ReverseFind('\\');
		if (nPos != -1)
		{
			sFile = sRmapDnPath.Right(sRmapDnPath.GetLength() - nPos - 1);
			sRmapDnPath.Delete(nPos, sRmapDnPath.GetLength() - nPos);
		}


		ResetMkInfo(2); // 0 : AOI-Up , 1 : AOI-Dn , 2 : AOI-UpDn
	}
	else
	{
		ResetMkInfo(0); // 0 : AOI-Up , 1 : AOI-Dn , 2 : AOI-UpDn
		bDualTest = FALSE;
	}

	ClrDispMsg();
	double dProgressRatio = 0.0;
	int nProgress = 0, nSerial = 0;
	CDlgProgress dlg;
	sVal.Format(_T("On Remaking Reelmap from PCR."));
	dlg.Create(sVal);
	dlg.SetRange(0, 100);
	dlg.SetPos(1);

	for (nPnl = 0; nPnl < nLastShot; nPnl++)
	{
		dProgressRatio = double(nPnl + 1) / double(nLastShot) * 100.0;
		nProgress = int(dProgressRatio);
		dlg.SetPos(nProgress);

		nSerial = nPnl + 1;
		sUpPath.Format(_T("%s\\%04d.pcr"), sRmapUpPath, nSerial);

		if (findfile.FindFile(sUpPath))
		{
			pDoc->LoadPcrUp(sUpPath);

			if (bDualTest)
			{
				sDnPath.Format(_T("%s\\%04d.pcr"), sRmapDnPath, nSerial);
				if (findfile.FindFile(sDnPath))
				{
					pDoc->LoadPcrDn(sDnPath);
					pDoc->LoadPcrAllUp(sDnPath);
					//pDoc->LoadPcrAllDn(sDnPath);

					// �ø��������� ������ ������ ���� 
					if (pDoc->m_pReelMapUp)
						pDoc->m_pReelMapUp->Write(nSerial); // [0]:AOI-Up , [1]:AOI-Dn , [2]:AOI-AllUp , [3]:AOI-AllDn
					if (pDoc->m_pReelMapDn)
						pDoc->m_pReelMapDn->Write(nSerial); // [0]:AOI-Up , [1]:AOI-Dn , [2]:AOI-AllUp , [3]:AOI-AllDn
					if (pDoc->m_pReelMapAllUp)
						pDoc->m_pReelMapAllUp->Write(nSerial); // [0]:AOI-Up , [1]:AOI-Dn , [2]:AOI-AllUp , [3]:AOI-AllDn
					//if (pDoc->m_pReelMapAllDn)
					//	pDoc->m_pReelMapAllDn->Write(nSerial); // [0]:AOI-Up , [1]:AOI-Dn , [2]:AOI-AllUp , [3]:AOI-AllDn

					pDoc->SetLastSerial(nSerial);

					//pDoc->UpdateYieldUp(nSerial);
					//pDoc->UpdateYieldDn(nSerial);
					//pDoc->UpdateYieldAllUp(nSerial);
					//pDoc->UpdateYieldAllDn(nSerial);

					//UpdateReelmapYieldUp();
					//UpdateReelmapYieldDn();
					//UpdateReelmapYieldAllUp();
					//UpdateReelmapYieldAllDn();
				}
				else
				{
					sMsg.Format(_T("�ϸ��� PCR������ �������� �ʽ��ϴ�.\r\n%s"), sDnPath);
					pView->MsgBox(sMsg);
					return FALSE;
				}
			}
			else
			{
				//UpdateReelmap(nSerial); // �ø��������� ������ ������ ���� 
				if (pDoc->m_pReelMapUp)
					pDoc->m_pReelMapUp->Write(nSerial); // [0]:AOI-Up , [1]:AOI-Dn , [2]:AOI-AllUp , [3]:AOI-AllDn

				pDoc->SetLastSerial(nSerial);

				//pDoc->UpdateYieldUp(nSerial);
			}
		}
	}

	sPath = pDoc->m_pReelMapUp->GetRmapPath(RMAP_UP);
	::WritePrivateProfileString(_T("Info"), _T("Process Code"), sProcessCode, sPath);
	::WritePrivateProfileString(_T("Info"), _T("Entire Speed"), sEntireSpeed, sPath);
	::WritePrivateProfileString(_T("Info"), _T("Lot Run"), sLotRun, sPath);
	::WritePrivateProfileString(_T("Info"), _T("Lot End"), sLotEnd, sPath);
	sVal.Format(_T("%d"), nLastShot);
	::WritePrivateProfileString(_T("Info"), _T("End Serial"), sVal, sPath);

	if (bDualTest)
	{
		sPath = pDoc->m_pReelMapUp->GetRmapPath(RMAP_DN);
		::WritePrivateProfileString(_T("Info"), _T("Process Code"), sProcessCode, sPath);
		::WritePrivateProfileString(_T("Info"), _T("Entire Speed"), sEntireSpeed, sPath);
		::WritePrivateProfileString(_T("Info"), _T("Lot Run"), sLotRun, sPath);
		::WritePrivateProfileString(_T("Info"), _T("Lot End"), sLotEnd, sPath);
		sVal.Format(_T("%d"), nLastShot);
		::WritePrivateProfileString(_T("Info"), _T("End Serial"), sVal, sPath);

		sPath = pDoc->m_pReelMapUp->GetRmapPath(RMAP_ALLUP);
		::WritePrivateProfileString(_T("Info"), _T("Process Code"), sProcessCode, sPath);
		::WritePrivateProfileString(_T("Info"), _T("Entire Speed"), sEntireSpeed, sPath);
		::WritePrivateProfileString(_T("Info"), _T("Lot Run"), sLotRun, sPath);
		::WritePrivateProfileString(_T("Info"), _T("Lot End"), sLotEnd, sPath);
		sVal.Format(_T("%d"), nLastShot);
		::WritePrivateProfileString(_T("Info"), _T("End Serial"), sVal, sPath);

		sPath = pDoc->m_pReelMapUp->GetRmapPath(RMAP_ALLDN);
		::WritePrivateProfileString(_T("Info"), _T("Process Code"), sProcessCode, sPath);
		::WritePrivateProfileString(_T("Info"), _T("Entire Speed"), sEntireSpeed, sPath);
		::WritePrivateProfileString(_T("Info"), _T("Lot Run"), sLotRun, sPath);
		::WritePrivateProfileString(_T("Info"), _T("Lot End"), sLotEnd, sPath);
		sVal.Format(_T("%d"), nLastShot);
		::WritePrivateProfileString(_T("Info"), _T("End Serial"), sVal, sPath);
	}

	dlg.SetPos(100);
	dlg.DestroyWindow();

	if (m_pDlgMenu05)
	{
		int nIndex = m_pDlgMenu05->GetIdxTopLayer();
		if (nIndex > -1)
		{
			m_pDlgMenu05->SelchangeComboLayer(nIndex);
		}
	}
	*/
}

int CGvisR2R_PunchView::IsOfflineFolder() // 0 : Not exist, 1 : Exist only Up, 2 : Exist only Dn, 3 : Exist Up and Dn
{
	int nRtn = 0;
	CString sPath, str;
	BOOL bWorking;

	//�˻� Ŭ����
	CFileFind finder;

	str = _T("OFFLINE");
	sPath.Format(_T("%s%s\\%s\\%s\\%s"), pDoc->WorkingInfo.System.sPathOldFile,
		pDoc->WorkingInfo.LastJob.sModelUp,
		pDoc->WorkingInfo.LastJob.sLotUp,
		pDoc->WorkingInfo.LastJob.sLayerUp,
		str);

	//CFileFind�� ����, ���͸��� �����ϸ� TRUE �� ��ȯ�� 
	bWorking = finder.FindFile(sPath);

	if (bWorking)
		nRtn |= 0x01;

	str = _T("OFFLINE");
	sPath.Format(_T("%s%s\\%s\\%s\\%s"), pDoc->WorkingInfo.System.sPathOldFile,
		pDoc->WorkingInfo.LastJob.sModelUp,
		pDoc->WorkingInfo.LastJob.sLotUp,
		pDoc->WorkingInfo.LastJob.sLayerDn,
		str);

	//CFileFind�� ����, ���͸��� �����ϸ� TRUE �� ��ȯ�� 
	bWorking = finder.FindFile(sPath);

	if (bWorking)
		nRtn |= 0x02;

	return nRtn;
}

void CGvisR2R_PunchView::StartThreadRemakeRmapFromPcr()
{
	// LoadPcrUpOnOffline
	if (!m_bThread[39])
		m_Thread[39].Start(GetSafeHwnd(), this, ThreadProc39);

	// LoadPcrDnOnOffline
	if (!m_bThread[40])
		m_Thread[40].Start(GetSafeHwnd(), this, ThreadProc40);

	// LoadPcrUpAllOnOffline
	if (!m_bThread[41])
		m_Thread[41].Start(GetSafeHwnd(), this, ThreadProc41);

	// LoadPcrDnAllOnOffline
	if (!m_bThread[42])
		m_Thread[42].Start(GetSafeHwnd(), this, ThreadProc42);

	// UpdateRMapUpOnOffline
	if (!m_bThread[43])
		m_Thread[43].Start(GetSafeHwnd(), this, ThreadProc43);

	// UpdateRMapDnOnOffline
	if (!m_bThread[44])
		m_Thread[44].Start(GetSafeHwnd(), this, ThreadProc44);

	// UpdateRMapUpAllOnOffline
	if (!m_bThread[45])
		m_Thread[45].Start(GetSafeHwnd(), this, ThreadProc45);

	// UpdateRMapDnAllOnOffline
	if (!m_bThread[46])
		m_Thread[46].Start(GetSafeHwnd(), this, ThreadProc46);
}

void CGvisR2R_PunchView::CopyRmapToOffline(int nOffline) // 0 : Not exist, 1 : Exist only Up, 2 : Exist only Dn, 3 : Exist Up and Dn
{
	CString sPathSource, sPathDest, str;
	BOOL bRst[4], bWorking;
	//�˻� Ŭ����
	CFileFind finder;

	if (nOffline & 0x01)
	{
		str = _T("ReelMapDataUp.txt");
		sPathSource.Format(_T("%s%s\\%s\\%s\\%s"), pDoc->WorkingInfo.System.sPathOldFile,
			pDoc->WorkingInfo.LastJob.sModelUp,
			pDoc->WorkingInfo.LastJob.sLotUp,
			pDoc->WorkingInfo.LastJob.sLayerUp,
			str);
		//CFileFind�� ����, ���͸��� �����ϸ� TRUE �� ��ȯ�� 
		bWorking = finder.FindFile(sPathSource);

		if (bWorking)
		{
			sPathDest.Format(_T("%s%s\\%s\\%s\\OFFLINE\\%s"), pDoc->WorkingInfo.System.sPathOldFile,
				pDoc->WorkingInfo.LastJob.sModelUp,
				pDoc->WorkingInfo.LastJob.sLotUp,
				pDoc->WorkingInfo.LastJob.sLayerUp,
				str);
			bRst[0] = pDoc->m_pFile->Copy(sPathSource, sPathDest);			// ���� ������ �����Ѵ�.
		}
		else
		{
			bRst[0] = FALSE;
		}

		str = _T("ReelMapDataAll.txt");
		sPathSource.Format(_T("%s%s\\%s\\%s\\%s"), pDoc->WorkingInfo.System.sPathOldFile,
			pDoc->WorkingInfo.LastJob.sModelUp,
			pDoc->WorkingInfo.LastJob.sLotUp,
			pDoc->WorkingInfo.LastJob.sLayerUp,
			str);
		//CFileFind�� ����, ���͸��� �����ϸ� TRUE �� ��ȯ�� 
		bWorking = finder.FindFile(sPathSource);

		if (bWorking)
		{
			sPathDest.Format(_T("%s%s\\%s\\%s\\OFFLINE\\%s"), pDoc->WorkingInfo.System.sPathOldFile,
				pDoc->WorkingInfo.LastJob.sModelUp,
				pDoc->WorkingInfo.LastJob.sLotUp,
				pDoc->WorkingInfo.LastJob.sLayerUp,
				str);
			bRst[1] = pDoc->m_pFile->Copy(sPathSource, sPathDest);			// ���� ������ �����Ѵ�.
		}
		else
		{
			bRst[1] = FALSE;
		}

		if (!bRst[0] && !bRst[1])
		{
			pView->MsgBox(_T("��鸱��(ReelMapDataUp.txt�� ReelMapDataAll.txt)�� ������� �ʾҽ��ϴ�."));
		}
		else if (!bRst[0])
		{
			pView->MsgBox(_T("��鸱��(ReelMapDataUp.txt)�� ������� �ʾҽ��ϴ�."));
		}
		else if (!bRst[1])
		{
			if(nOffline & 0x02)
				pView->MsgBox(_T("��鸱��(ReelMapDataAll.txt)�� ������� �ʾҽ��ϴ�."));
		}
	}

	if (nOffline & 0x02)
	{
		str = _T("ReelMapDataDn.txt");
		sPathSource.Format(_T("%s%s\\%s\\%s\\%s"), pDoc->WorkingInfo.System.sPathOldFile,
			pDoc->WorkingInfo.LastJob.sModelUp,
			pDoc->WorkingInfo.LastJob.sLotUp,
			pDoc->WorkingInfo.LastJob.sLayerDn,
			str);

		//CFileFind�� ����, ���͸��� �����ϸ� TRUE �� ��ȯ�� 
		bWorking = finder.FindFile(sPathSource);

		if (bWorking)
		{
			sPathDest.Format(_T("%s%s\\%s\\%s\\OFFLINE\\%s"), pDoc->WorkingInfo.System.sPathOldFile,
				pDoc->WorkingInfo.LastJob.sModelUp,
				pDoc->WorkingInfo.LastJob.sLotUp,
				pDoc->WorkingInfo.LastJob.sLayerDn,
				str);
			bRst[2] = pDoc->m_pFile->Copy(sPathSource, sPathDest);			// ���� ������ �����Ѵ�.
		}
		else
		{
			bRst[2] = FALSE;
		}

		str = _T("ReelMapDataAll.txt");
		sPathSource.Format(_T("%s%s\\%s\\%s\\%s"), pDoc->WorkingInfo.System.sPathOldFile,
			pDoc->WorkingInfo.LastJob.sModelUp,
			pDoc->WorkingInfo.LastJob.sLotUp,
			pDoc->WorkingInfo.LastJob.sLayerUp,
			str);

		//CFileFind�� ����, ���͸��� �����ϸ� TRUE �� ��ȯ�� 
		bWorking = finder.FindFile(sPathSource);

		if (bWorking)
		{
			sPathDest.Format(_T("%s%s\\%s\\%s\\OFFLINE\\%s"), pDoc->WorkingInfo.System.sPathOldFile,
				pDoc->WorkingInfo.LastJob.sModelUp,
				pDoc->WorkingInfo.LastJob.sLotUp,
				pDoc->WorkingInfo.LastJob.sLayerUp,
				str);
			bRst[3] = pDoc->m_pFile->Copy(sPathSource, sPathDest);			// ���� ������ �����Ѵ�.
		}
		else
		{
			bRst[3] = FALSE;
		}

		if (!bRst[2] && !bRst[3])
		{
			pView->MsgBox(_T("�ϸ鸱��(ReelMapDataDn.txt�� ReelMapDataAll.txt)�� ������� �ʾҽ��ϴ�."));
		}
		else if (!bRst[2])
		{
			pView->MsgBox(_T("�ϸ鸱��(ReelMapDataDn.txt)�� ������� �ʾҽ��ϴ�."));
		}
		else if (!bRst[3])
		{
			pView->MsgBox(_T("�ϸ鸱��(ReelMapDataAll.txt)�� ������� �ʾҽ��ϴ�."));
		}
	}
}

int CGvisR2R_PunchView::GetLastShotFromPcr(int nOffline)
{
	CString sPath = _T("���� ���/*.*");

	if (nOffline & 0x01)
	{
		sPath.Format(_T("%s%s\\%s\\%s\\OFFLINE\\*.pcr"),
			pDoc->WorkingInfo.System.sPathOldFile,
			pDoc->WorkingInfo.LastJob.sModelUp,
			pDoc->WorkingInfo.LastJob.sLotUp,
			pDoc->WorkingInfo.LastJob.sLayerUp);
	}
	else if (nOffline & 0x02)
	{
		sPath.Format(_T("%s%s\\%s\\%s\\OFFLINE\\*.pcr"), 
			pDoc->WorkingInfo.System.sPathOldFile,
			pDoc->WorkingInfo.LastJob.sModelUp,
			pDoc->WorkingInfo.LastJob.sLotUp,
			pDoc->WorkingInfo.LastJob.sLayerDn);
	}

	// �˻��� ��� �� ����
	// Ư�� Ȯ���ڸ� ���� ������ �˻��ϰ� ������ '���/*.pcr' ������ ���

	// �˻� Ŭ����
	CFileFind finder;

	// CFileFind�� ����, ���͸��� �����ϸ� TRUE �� ��ȯ��
	BOOL bWorking = finder.FindFile(sPath); 

	CString fileName;
	CString DirName;
	int nSerial=0, nLastShot=0, nPos=-1;

	while (bWorking)
	{
		//���� ���� or ���� �� �����ϸ�ٸ� TRUE ��ȯ
		bWorking = finder.FindNextFile();

		// folder �� ���� continue
		if (finder.IsDirectory() || finder.IsDots())
			continue;

		// ���� �϶�
		// ������ �̸�
		CString _fileName = finder.GetFileName();

		// �������� �������� ����������� ����
		if (_fileName == _T("Thumbs.db")) continue;

		fileName = finder.GetFileTitle();

		//�о�� ���� �̸����� LastShot ��ȣ�� ã�´�.
		nSerial = _tstoi(fileName);

		if (nSerial > nLastShot)
			nLastShot = nSerial;
	}

	return nLastShot;
}

BOOL CGvisR2R_PunchView::RemakeRmapFromPcr(int nLastShot, int nOffline)
{
	CString sMsg, sVal, sUpPath, sDnPath, sPcrUpPath, sPcrDnPath;
	int nPnl;

	sPcrUpPath.Format(_T("%s%s\\%s\\%s\\OFFLINE"), 
		pDoc->WorkingInfo.System.sPathOldFile,
		pDoc->WorkingInfo.LastJob.sModelUp,
		pDoc->WorkingInfo.LastJob.sLotUp,
		pDoc->WorkingInfo.LastJob.sLayerUp);

	sPcrDnPath.Format(_T("%s%s\\%s\\%s\\OFFLINE"),
		pDoc->WorkingInfo.System.sPathOldFile,
		pDoc->WorkingInfo.LastJob.sModelUp,
		pDoc->WorkingInfo.LastJob.sLotUp,
		pDoc->WorkingInfo.LastJob.sLayerDn);

	ResetYield(nOffline);
	DeleteReelmapOnOffline();

	// �˻� Ŭ����
	CFileFind findfile;

	double dProgressRatio = 0.0;
	int nProgress = 0, nSerial = 0;
	CDlgProgress dlg;
	sVal.Format(_T("On Remaking Reelmap from PCR."));
	dlg.Create(sVal);
	dlg.SetRange(0, 100);
	dlg.SetPos(1);

	for (nPnl = 0; nPnl < nLastShot; nPnl++)
	{
		dProgressRatio = double(nPnl + 1) / double(nLastShot) * 100.0;
		nProgress = int(dProgressRatio);
		dlg.SetPos(nProgress);

		nSerial = nPnl + 1;

		LoadPcrOnOffline(nSerial, nOffline);

		while (m_bTHREAD_LOAD_PCR_UP_OFFLINE || m_bTHREAD_LOAD_PCR_DN_OFFLINE)
		{
			Sleep(100);
		}

		LoadPcrAllOnOffline(nSerial, nOffline);

		while (m_bTHREAD_LOAD_PCR_ALLUP_OFFLINE || m_bTHREAD_LOAD_PCR_ALLDN_OFFLINE)
		{
			Sleep(100);
		}

		UpdateRMapOnOffline(nSerial, nOffline);

		while (m_bTHREAD_UPDATE_REELMAP_UP_OFFLINE || m_bTHREAD_UPDATE_REELMAP_DN_OFFLINE)
		{
			Sleep(100);
		}

		UpdateRMapAllOnOffline(nSerial, nOffline);

		while (m_bTHREAD_UPDATE_REELMAP_ALLUP_OFFLINE || m_bTHREAD_UPDATE_REELMAP_ALLDN_OFFLINE)
		{
			Sleep(100);
		}
	}

	WriteLastRmapInfo(nLastShot, nOffline);

	//while (m_bTHREAD_WRITE_LAST_INFO_REELMAP_UP_OFFLINE || m_bTHREAD_WRITE_LAST_INFO_REELMAP_DN_OFFLINE
	//	|| m_bTHREAD_WRITE_LAST_INFO_REELMAP_ALLUP_OFFLINE || m_bTHREAD_WRITE_LAST_INFO_REELMAP_ALLDN_OFFLINE)
	//{
	//	Sleep(100);
	//}

	//SetLastSerialOnOffline(nLastShot, nOffline);

	return TRUE;
}

void CGvisR2R_PunchView::ResetYield(int nOffline)
{
	BOOL bDualTest = nOffline & 0x02;

	if (pDoc->m_pReelMapUp)
		pDoc->m_pReelMapUp->ResetYield();
	if (bDualTest)
	{
		if (pDoc->m_pReelMapDn)
			pDoc->m_pReelMapDn->ResetYield();
		if (pDoc->m_pReelMapAllUp)
			pDoc->m_pReelMapAllUp->ResetYield();
		if (pDoc->m_pReelMapAllDn)
			pDoc->m_pReelMapAllDn->ResetYield();
	}
}

BOOL CGvisR2R_PunchView::LoadPcrOnOffline(int nSerial, int nOffline)
{
	if (nSerial <= 0)
	{
		pView->ClrDispMsg();
		AfxMessageBox(_T("Serial Error.55"));
		return FALSE;
	}

	if (!pDoc->MakeMkDir())
		return FALSE;

	BOOL bDualTest = nOffline & 0x02;

	m_nSerialRmapUpdateOnOffline = nSerial;

	m_bTHREAD_LOAD_PCR_UP_OFFLINE = TRUE;
	if (bDualTest)
	{
		m_bTHREAD_LOAD_PCR_DN_OFFLINE = TRUE;
	}

	return TRUE;
}

BOOL CGvisR2R_PunchView::LoadPcrAllOnOffline(int nSerial, int nOffline)
{
	if (nSerial <= 0)
	{
		pView->ClrDispMsg();
		AfxMessageBox(_T("Serial Error.55"));
		return FALSE;
	}

	if (!pDoc->MakeMkDir())
		return FALSE;

	BOOL bDualTest = nOffline & 0x02;

	m_nSerialRmapUpdateOnOffline = nSerial;

	if (bDualTest)
	{
		m_bTHREAD_LOAD_PCR_ALLUP_OFFLINE = TRUE;
		m_bTHREAD_LOAD_PCR_ALLDN_OFFLINE = TRUE;
	}

	return TRUE;
}

BOOL CGvisR2R_PunchView::UpdateRMapOnOffline(int nSerial, int nOffline)
{
	if (nSerial <= 0)
	{
		pView->ClrDispMsg();
		AfxMessageBox(_T("Serial Error.56"));
		return FALSE;
	}

	if (!pDoc->MakeMkDir())
		return FALSE;

	BOOL bDualTest = nOffline & 0x02;

	m_nSerialRmapUpdateOnOffline = nSerial;

	m_bTHREAD_UPDATE_REELMAP_UP_OFFLINE = TRUE;
	if (bDualTest)
	{
		m_bTHREAD_UPDATE_REELMAP_DN_OFFLINE = TRUE;
	}

	return TRUE;
}

BOOL CGvisR2R_PunchView::UpdateRMapAllOnOffline(int nSerial, int nOffline)
{
	if (nSerial <= 0)
	{
		pView->ClrDispMsg();
		AfxMessageBox(_T("Serial Error.58"));
		return FALSE;
	}

	if (!pDoc->MakeMkDir())
		return FALSE;

	BOOL bDualTest = nOffline & 0x02;

	m_nSerialRmapUpdateOnOffline = nSerial;

	if (bDualTest)
	{
		m_bTHREAD_UPDATE_REELMAP_ALLUP_OFFLINE = TRUE;
		m_bTHREAD_UPDATE_REELMAP_ALLDN_OFFLINE = TRUE;
	}

	return TRUE;
}

UINT CGvisR2R_PunchView::ThreadProc39(LPVOID lpContext)	// LoadPcrUpOnOffline
{
	// Turn the passed in 'this' pointer back into a CProgressMgr instance
	CGvisR2R_PunchView* pThread = reinterpret_cast<CGvisR2R_PunchView*>(lpContext);

	BOOL bLock = FALSE;
	DWORD dwTick = GetTickCount();
	DWORD dwShutdownEventCheckPeriod = 0; // thread shutdown event check period

	pThread->m_bThread[39] = TRUE;
	while (WAIT_OBJECT_0 != WaitForSingleObject(pThread->m_Thread[39].GetShutdownEvent(), dwShutdownEventCheckPeriod))
	{
		pThread->m_dwThreadTick[39] = GetTickCount() - dwTick;
		dwTick = GetTickCount();

		if (pThread->m_bTHREAD_LOAD_PCR_UP_OFFLINE)
		{
			pThread->LoadPcrUpOnOffline();
			pThread->m_bTHREAD_LOAD_PCR_UP_OFFLINE = FALSE;
			Sleep(0);
		}
		else
			Sleep(30);
	}

	pThread->m_bThread[39] = FALSE;

	return 0;
}

UINT CGvisR2R_PunchView::ThreadProc40(LPVOID lpContext)	// LoadPcrDnOnOffline
{
	// Turn the passed in 'this' pointer back into a CProgressMgr instance
	CGvisR2R_PunchView* pThread = reinterpret_cast<CGvisR2R_PunchView*>(lpContext);

	BOOL bLock = FALSE;
	DWORD dwTick = GetTickCount();
	DWORD dwShutdownEventCheckPeriod = 0; // thread shutdown event check period

	pThread->m_bThread[40] = TRUE;
	while (WAIT_OBJECT_0 != WaitForSingleObject(pThread->m_Thread[40].GetShutdownEvent(), dwShutdownEventCheckPeriod))
	{
		pThread->m_dwThreadTick[40] = GetTickCount() - dwTick;
		dwTick = GetTickCount();

		if (pThread->m_bTHREAD_LOAD_PCR_DN_OFFLINE)
		{
			pThread->LoadPcrDnOnOffline();
			pThread->m_bTHREAD_LOAD_PCR_DN_OFFLINE = FALSE;
			Sleep(0);
		}
		else
			Sleep(30);
	}

	pThread->m_bThread[40] = FALSE;

	return 0;
}

UINT CGvisR2R_PunchView::ThreadProc41(LPVOID lpContext)	// LoadPcrUpAllOnOffline
{
	// Turn the passed in 'this' pointer back into a CProgressMgr instance
	CGvisR2R_PunchView* pThread = reinterpret_cast<CGvisR2R_PunchView*>(lpContext);

	BOOL bLock = FALSE;
	DWORD dwTick = GetTickCount();
	DWORD dwShutdownEventCheckPeriod = 0; // thread shutdown event check period

	pThread->m_bThread[41] = TRUE;
	while (WAIT_OBJECT_0 != WaitForSingleObject(pThread->m_Thread[41].GetShutdownEvent(), dwShutdownEventCheckPeriod))
	{
		pThread->m_dwThreadTick[41] = GetTickCount() - dwTick;
		dwTick = GetTickCount();

		if (pThread->m_bTHREAD_LOAD_PCR_ALLUP_OFFLINE)
		{
			pThread->LoadPcrUpAllOnOffline();
			pThread->m_bTHREAD_LOAD_PCR_ALLUP_OFFLINE = FALSE;
			Sleep(0);
		}
		else
			Sleep(30);
	}

	pThread->m_bThread[41] = FALSE;

	return 0;
}

UINT CGvisR2R_PunchView::ThreadProc42(LPVOID lpContext)	// LoadPcrDnAllOnOffline
{
	// Turn the passed in 'this' pointer back into a CProgressMgr instance
	CGvisR2R_PunchView* pThread = reinterpret_cast<CGvisR2R_PunchView*>(lpContext);

	BOOL bLock = FALSE;
	DWORD dwTick = GetTickCount();
	DWORD dwShutdownEventCheckPeriod = 0; // thread shutdown event check period

	pThread->m_bThread[42] = TRUE;
	while (WAIT_OBJECT_0 != WaitForSingleObject(pThread->m_Thread[42].GetShutdownEvent(), dwShutdownEventCheckPeriod))
	{
		pThread->m_dwThreadTick[42] = GetTickCount() - dwTick;
		dwTick = GetTickCount();

		if (pThread->m_bTHREAD_LOAD_PCR_ALLDN_OFFLINE)
		{
			pThread->LoadPcrDnAllOnOffline();
			pThread->m_bTHREAD_LOAD_PCR_ALLDN_OFFLINE = FALSE;
			Sleep(0);
		}
		else
			Sleep(30);
	}

	pThread->m_bThread[42] = FALSE;

	return 0;
}

UINT CGvisR2R_PunchView::ThreadProc43(LPVOID lpContext)	// m_pReelMapUp->WriteOnOffline()
{
	// Turn the passed in 'this' pointer back into a CProgressMgr instance
	CGvisR2R_PunchView* pThread = reinterpret_cast<CGvisR2R_PunchView*>(lpContext);

	BOOL bLock = FALSE;
	DWORD dwTick = GetTickCount();
	DWORD dwShutdownEventCheckPeriod = 0; // thread shutdown event check period

	pThread->m_bThread[43] = TRUE;
	while (WAIT_OBJECT_0 != WaitForSingleObject(pThread->m_Thread[43].GetShutdownEvent(), dwShutdownEventCheckPeriod))
	{
		pThread->m_dwThreadTick[43] = GetTickCount() - dwTick;
		dwTick = GetTickCount();

		if (pThread->m_bTHREAD_UPDATE_REELMAP_UP_OFFLINE)
		{
			pThread->UpdateRMapUpOnOffline();
			pThread->m_bTHREAD_UPDATE_REELMAP_UP_OFFLINE = FALSE;
			Sleep(0);
		}
		else if (pThread->m_bTHREAD_WRITE_LAST_INFO_REELMAP_UP_OFFLINE) // WriteLastRMapUpInfoOnOffline()
		{
			pThread->WriteLastRMapUpInfoOnOffline();
			pThread->m_bTHREAD_WRITE_LAST_INFO_REELMAP_UP_OFFLINE = FALSE;
			Sleep(0);
		}
		else
			Sleep(30);
	}

	pThread->m_bThread[43] = FALSE;

	return 0;
}

UINT CGvisR2R_PunchView::ThreadProc44(LPVOID lpContext)	// m_pReelMapDn->WriteOnOffline()
{
	// Turn the passed in 'this' pointer back into a CProgressMgr instance
	CGvisR2R_PunchView* pThread = reinterpret_cast<CGvisR2R_PunchView*>(lpContext);

	BOOL bLock = FALSE;
	DWORD dwTick = GetTickCount();
	DWORD dwShutdownEventCheckPeriod = 0; // thread shutdown event check period

	pThread->m_bThread[44] = TRUE;
	while (WAIT_OBJECT_0 != WaitForSingleObject(pThread->m_Thread[44].GetShutdownEvent(), dwShutdownEventCheckPeriod))
	{
		pThread->m_dwThreadTick[44] = GetTickCount() - dwTick;
		dwTick = GetTickCount();

		if (pThread->m_bTHREAD_UPDATE_REELMAP_DN_OFFLINE)
		{
			pThread->UpdateRMapDnOnOffline();
			pThread->m_bTHREAD_UPDATE_REELMAP_DN_OFFLINE = FALSE;
			Sleep(0);
		}
		else if (pThread->m_bTHREAD_WRITE_LAST_INFO_REELMAP_DN_OFFLINE) // WriteLastRMapDnInfoOnOffline()
		{
			pThread->WriteLastRMapDnInfoOnOffline();
			pThread->m_bTHREAD_WRITE_LAST_INFO_REELMAP_DN_OFFLINE = FALSE;
			Sleep(0);
		}
		else
			Sleep(30);
	}

	pThread->m_bThread[44] = FALSE;

	return 0;
}

UINT CGvisR2R_PunchView::ThreadProc45(LPVOID lpContext)	// m_pReelMapAllUp->WriteOnOffline()
{
	// Turn the passed in 'this' pointer back into a CProgressMgr instance
	CGvisR2R_PunchView* pThread = reinterpret_cast<CGvisR2R_PunchView*>(lpContext);

	BOOL bLock = FALSE;
	DWORD dwTick = GetTickCount();
	DWORD dwShutdownEventCheckPeriod = 0; // thread shutdown event check period

	pThread->m_bThread[45] = TRUE;
	while (WAIT_OBJECT_0 != WaitForSingleObject(pThread->m_Thread[45].GetShutdownEvent(), dwShutdownEventCheckPeriod))
	{
		pThread->m_dwThreadTick[45] = GetTickCount() - dwTick;
		dwTick = GetTickCount();

		if (pThread->m_bTHREAD_UPDATE_REELMAP_ALLUP_OFFLINE)
		{
			pThread->UpdateRMapUpAllOnOffline();
			pThread->m_bTHREAD_UPDATE_REELMAP_ALLUP_OFFLINE = FALSE;
			Sleep(0);
		}
		else if (pThread->m_bTHREAD_WRITE_LAST_INFO_REELMAP_ALLUP_OFFLINE) // WriteLastRMapUpAllInfoOnOffline()
		{
			pThread->WriteLastRMapUpAllInfoOnOffline();
			pThread->m_bTHREAD_WRITE_LAST_INFO_REELMAP_ALLUP_OFFLINE = FALSE;
			Sleep(0);
		}
		else
			Sleep(30);
	}

	pThread->m_bThread[45] = FALSE;

	return 0;
}

UINT CGvisR2R_PunchView::ThreadProc46(LPVOID lpContext)	// m_pReelMapAllDn->WriteOnOffline()
{
	// Turn the passed in 'this' pointer back into a CProgressMgr instance
	CGvisR2R_PunchView* pThread = reinterpret_cast<CGvisR2R_PunchView*>(lpContext);

	BOOL bLock = FALSE;
	DWORD dwTick = GetTickCount();
	DWORD dwShutdownEventCheckPeriod = 0; // thread shutdown event check period

	pThread->m_bThread[46] = TRUE;
	while (WAIT_OBJECT_0 != WaitForSingleObject(pThread->m_Thread[46].GetShutdownEvent(), dwShutdownEventCheckPeriod))
	{
		pThread->m_dwThreadTick[46] = GetTickCount() - dwTick;
		dwTick = GetTickCount();

		if (pThread->m_bTHREAD_UPDATE_REELMAP_ALLDN_OFFLINE)
		{
			pThread->UpdateRMapDnAllOnOffline();
			pThread->m_bTHREAD_UPDATE_REELMAP_ALLDN_OFFLINE = FALSE;
			Sleep(0);
		}
		else if (pThread->m_bTHREAD_WRITE_LAST_INFO_REELMAP_ALLDN_OFFLINE) // WriteLastRMapDnAllInfoOnOffline()
		{
			pThread->WriteLastRMapDnAllInfoOnOffline();
			pThread->m_bTHREAD_WRITE_LAST_INFO_REELMAP_ALLDN_OFFLINE = FALSE;
			Sleep(0);
		}
		else
			Sleep(30);
	}

	pThread->m_bThread[46] = FALSE;

	return 0;
}


void CGvisR2R_PunchView::LoadPcrUpOnOffline()
{
	int nSerial = m_nSerialRmapUpdateOnOffline;
	CString sPath;
	sPath.Format(_T("%s%s\\%s\\%s\\OFFLINE\\%04d.pcr"),
		pDoc->WorkingInfo.System.sPathOldFile,
		pDoc->WorkingInfo.LastJob.sModelUp,
		pDoc->WorkingInfo.LastJob.sLotUp,
		pDoc->WorkingInfo.LastJob.sLayerUp,
		nSerial);

	// �˻� Ŭ����
	CFileFind findfile;

	if (findfile.FindFile(sPath))
	{
		pDoc->LoadPcrUp(sPath);
	}
}

void CGvisR2R_PunchView::LoadPcrDnOnOffline()
{
	int nSerial = m_nSerialRmapUpdateOnOffline;
	CString sPath;
	sPath.Format(_T("%s%s\\%s\\%s\\OFFLINE\\%04d.pcr"),
		pDoc->WorkingInfo.System.sPathOldFile,
		pDoc->WorkingInfo.LastJob.sModelUp,
		pDoc->WorkingInfo.LastJob.sLotUp,
		pDoc->WorkingInfo.LastJob.sLayerDn,
		nSerial);

	// �˻� Ŭ����
	CFileFind findfile;

	if (findfile.FindFile(sPath))
	{
		pDoc->LoadPcrDn(sPath);
	}
}

void CGvisR2R_PunchView::LoadPcrUpAllOnOffline()
{
	int nSerial = m_nSerialRmapUpdateOnOffline;
	CString sPath;
	sPath.Format(_T("%s%s\\%s\\%s\\OFFLINE\\%04d.pcr"),
		pDoc->WorkingInfo.System.sPathOldFile,
		pDoc->WorkingInfo.LastJob.sModelUp,
		pDoc->WorkingInfo.LastJob.sLotUp,
		pDoc->WorkingInfo.LastJob.sLayerUp,
		nSerial);

	// �˻� Ŭ����
	CFileFind findfile;

	if (findfile.FindFile(sPath))
	{
		pDoc->LoadPcrAllUp(sPath);
	}
}

void CGvisR2R_PunchView::LoadPcrDnAllOnOffline()
{
	int nSerial = m_nSerialRmapUpdateOnOffline;
	CString sPath;
	sPath.Format(_T("%s%s\\%s\\%s\\OFFLINE\\%04d.pcr"),
		pDoc->WorkingInfo.System.sPathOldFile,
		pDoc->WorkingInfo.LastJob.sModelUp,
		pDoc->WorkingInfo.LastJob.sLotUp,
		pDoc->WorkingInfo.LastJob.sLayerDn,
		nSerial);

	// �˻� Ŭ����
	CFileFind findfile;

	if (findfile.FindFile(sPath))
	{
		pDoc->LoadPcrAllDn(sPath);
	}
}


void CGvisR2R_PunchView::UpdateRMapUpOnOffline()
{
	int nSerial = m_nSerialRmapUpdateOnOffline;
	if (pDoc->m_pReelMapUp)
	{
		pDoc->m_pReelMapUp->WriteOnOffline(nSerial); // [0]:AOI-Up , [1]:AOI-Dn , [2]:AOI-AllUp , [3]:AOI-AllDn
	}
}

void CGvisR2R_PunchView::UpdateRMapDnOnOffline()
{
	int nSerial = m_nSerialRmapUpdateOnOffline;
	if (pDoc->m_pReelMapDn)
	{
		pDoc->m_pReelMapDn->WriteOnOffline(nSerial); // [0]:AOI-Up , [1]:AOI-Dn , [2]:AOI-AllUp , [3]:AOI-AllDn
	}
}

void CGvisR2R_PunchView::UpdateRMapUpAllOnOffline()
{
	int nSerial = m_nSerialRmapUpdateOnOffline;
	if (pDoc->m_pReelMapAllUp)
	{
		pDoc->m_pReelMapAllUp->WriteOnOffline(nSerial); // [0]:AOI-Up , [1]:AOI-Dn , [2]:AOI-AllUp , [3]:AOI-AllDn
	}
}

void CGvisR2R_PunchView::UpdateRMapDnAllOnOffline()
{
	int nSerial = m_nSerialRmapUpdateOnOffline;
	if (pDoc->m_pReelMapAllDn)
	{
		pDoc->m_pReelMapAllDn->WriteOnOffline(nSerial); // [0]:AOI-Up , [1]:AOI-Dn , [2]:AOI-AllUp , [3]:AOI-AllDn
	}
}


void CGvisR2R_PunchView::SetLastSerialOnOffline(int nSerial, int nOffline)
{
	if (nSerial <= 0)
	{
		ClrDispMsg();
		AfxMessageBox(_T("Serial Error.57"));
		return;
	}

	BOOL bDualTest = nOffline & 0x02;

	if (nSerial > 0)
	{
		if (pDoc->m_pReelMapUp)
			pDoc->m_pReelMapUp->SetLastSerialOnOffline(nSerial);					// ���� �ؽ�Ʈ ������ ���������� ������Ʈ��.
		if (bDualTest)
		{
			if (pDoc->m_pReelMapDn)
				pDoc->m_pReelMapDn->SetLastSerialOnOffline(nSerial);					// ���� �ؽ�Ʈ ������ ���������� ������Ʈ��.
			if (pDoc->m_pReelMapAllUp)
				pDoc->m_pReelMapAllUp->SetLastSerialOnOffline(nSerial);					// ���� �ؽ�Ʈ ������ ���������� ������Ʈ��.
			if (pDoc->m_pReelMapAllDn)
				pDoc->m_pReelMapAllDn->SetLastSerialOnOffline(nSerial);					// ���� �ؽ�Ʈ ������ ���������� ������Ʈ��.
		}
	}
}


void CGvisR2R_PunchView::StopThreadRemakeRmapFromPcr()
{
	if (m_bThread[39])
	{
		m_Thread[39].Stop();
		Sleep(20);
		while (m_bThread[39])
		{
			Sleep(20);
		}
	}

	if (m_bThread[40])
	{
		m_Thread[40].Stop();
		Sleep(20);
		while (m_bThread[40])
		{
			Sleep(20);
		}
	}

	if (m_bThread[41])
	{
		m_Thread[41].Stop();
		Sleep(20);
		while (m_bThread[41])
		{
			Sleep(20);
		}
	}

	if (m_bThread[42])
	{
		m_Thread[42].Stop();
		Sleep(20);
		while (m_bThread[42])
		{
			Sleep(20);
		}
	}

	if (m_bThread[43])
	{
		m_Thread[43].Stop();
		Sleep(20);
		while (m_bThread[43])
		{
			Sleep(20);
		}
	}

	if (m_bThread[44])
	{
		m_Thread[44].Stop();
		Sleep(20);
		while (m_bThread[44])
		{
			Sleep(20);
		}
	}

	if (m_bThread[45])
	{
		m_Thread[45].Stop();
		Sleep(20);
		while (m_bThread[45])
		{
			Sleep(20);
		}
	}

	if (m_bThread[46])
	{
		m_Thread[46].Stop();
		Sleep(20);
		while (m_bThread[46])
		{
			Sleep(20);
		}
	}
}

BOOL CGvisR2R_PunchView::WriteLastRmapInfo(int nLastShot, int nOffline)
{
	if (!pDoc->MakeMkDir())
		return FALSE;

	m_nLastShotOnOffline = nLastShot;
	BOOL bDualTest = nOffline & 0x02;
	//m_bTHREAD_WRITE_LAST_INFO_REELMAP_UP_OFFLINE = TRUE;
	WriteLastRMapUpInfoOnOffline();
	if (bDualTest)
	{
		WriteLastRMapDnInfoOnOffline();
		WriteLastRMapUpAllInfoOnOffline();
		WriteLastRMapDnAllInfoOnOffline();
		//m_bTHREAD_WRITE_LAST_INFO_REELMAP_DN_OFFLINE = TRUE;
		//m_bTHREAD_WRITE_LAST_INFO_REELMAP_ALLUP_OFFLINE = TRUE;
		//m_bTHREAD_WRITE_LAST_INFO_REELMAP_ALLDN_OFFLINE = TRUE;
	}

	return TRUE;
}

void CGvisR2R_PunchView::WriteLastRMapUpInfoOnOffline()
{
	if (m_nLastShotOnOffline <= 0)
	{
		ClrDispMsg();
		AfxMessageBox(_T("Serial Error.150"));
		return;
	}

	if (pDoc->m_pReelMapUp)
	{
		pDoc->m_pReelMapUp->WriteLastRmapInfoOnOffline(); // [0]:AOI-Up , [1]:AOI-Dn , [2]:AOI-AllUp , [3]:AOI-AllDn
		pDoc->m_pReelMapUp->SetLastSerialOnOffline(m_nLastShotOnOffline);
	}
}

void CGvisR2R_PunchView::WriteLastRMapDnInfoOnOffline()
{
	if (m_nLastShotOnOffline <= 0)
	{
		ClrDispMsg();
		AfxMessageBox(_T("Serial Error.151"));
		return;
	}

	if (pDoc->m_pReelMapDn)
	{
		pDoc->m_pReelMapDn->WriteLastRmapInfoOnOffline(); // [0]:AOI-Up , [1]:AOI-Dn , [2]:AOI-AllUp , [3]:AOI-AllDn
		pDoc->m_pReelMapDn->SetLastSerialOnOffline(m_nLastShotOnOffline);
	}
}

void CGvisR2R_PunchView::WriteLastRMapUpAllInfoOnOffline()
{
	if (m_nLastShotOnOffline <= 0)
	{
		ClrDispMsg();
		AfxMessageBox(_T("Serial Error.152"));
		return;
	}

	if (pDoc->m_pReelMapAllUp)
	{
		pDoc->m_pReelMapAllUp->WriteLastRmapInfoOnOffline(); // [0]:AOI-Up , [1]:AOI-Dn , [2]:AOI-AllUp , [3]:AOI-AllDn
		pDoc->m_pReelMapAllUp->SetLastSerialOnOffline(m_nLastShotOnOffline);
	}
}

void CGvisR2R_PunchView::WriteLastRMapDnAllInfoOnOffline()
{
	if (m_nLastShotOnOffline <= 0)
	{
		ClrDispMsg();
		AfxMessageBox(_T("Serial Error.153"));
		return;
	}

	if (pDoc->m_pReelMapAllDn)
	{
		pDoc->m_pReelMapAllDn->WriteLastRmapInfoOnOffline(); // [0]:AOI-Up , [1]:AOI-Dn , [2]:AOI-AllUp , [3]:AOI-AllDn
		pDoc->m_pReelMapAllDn->SetLastSerialOnOffline(m_nLastShotOnOffline);
	}
}

void CGvisR2R_PunchView::DeleteReelmapOnOffline()
{
	CFileFind findfile;
	CString sPath = _T("");

	if(pDoc->m_pReelMapUp)
		sPath = pDoc->m_pReelMapUp->GetRmapPathOnOffline(RMAP_UP);
	if (!sPath.IsEmpty())
	{
		if (findfile.FindFile(sPath))
			DeleteFile(sPath);
		sPath = _T("");
	}

	if (pDoc->m_pReelMapDn)
		sPath = pDoc->m_pReelMapDn->GetRmapPathOnOffline(RMAP_DN);
	if (!sPath.IsEmpty())
	{
		if (findfile.FindFile(sPath))
			DeleteFile(sPath);
		sPath = _T("");
	}

	if (pDoc->m_pReelMapAllUp)
		sPath = pDoc->m_pReelMapAllUp->GetRmapPathOnOffline(RMAP_ALLUP);
	if (!sPath.IsEmpty())
	{
		if (findfile.FindFile(sPath))
			DeleteFile(sPath);
		sPath = _T("");
	}

	if (pDoc->m_pReelMapAllDn)
		sPath = pDoc->m_pReelMapAllDn->GetRmapPathOnOffline(RMAP_ALLDN);
	if (!sPath.IsEmpty())
	{
		if (findfile.FindFile(sPath))
			DeleteFile(sPath);
		sPath = _T("");
	}
}