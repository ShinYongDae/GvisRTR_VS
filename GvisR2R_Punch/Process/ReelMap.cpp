// ReelMap.cpp : implementation file
//

#include "stdafx.h"
//#include "gvisr2r.h"
#include "ReelMap.h"
#include "DataFile.h"
#include "IniFile.h"
#include "../Global/GlobalFunc.h"
#include "../Dialog/DlgProgress.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#include "MyFile.h"
#include "..\\GvisR2R_PunchDoc.h"
#include "..\\GvisR2R_PunchView.h"

extern CGvisR2R_PunchDoc* pDoc;
extern CGvisR2R_PunchView* pView;

/////////////////////////////////////////////////////////////////////////////
// CReelMap

CReelMap::CReelMap(int nLayer, int nPnl, int nPcs, int nDir)
{
	int nC, nR;

	m_nLayer = -1;
	nTotPnl = nPnl;
	nTotPcs = nPcs;
	nDir = ROT_NONE;
	m_nBeforeSerial = 0;

	m_dTotLen = 0.0;
	m_bUseLotSep = FALSE;
	m_bContFixDef = FALSE;
	m_dAdjRatio = 1.0;
	m_dTotLen = 0.0;
	m_dPnlLen = 0.0;
	m_dLotLen = 0.0;
	m_dTempPauseLen = 0.0;
	m_dLotCutPosLen = 0.0;
	m_nSerial = 0;
	m_nLastShot = 0;
	m_nCompletedShot = 0;

	//m_nCntFixPcs = 0;
	m_nPrevSerial[0] = 0;	// --
	m_nPrevSerial[1] = 0;	// ++

	m_rgbDef[DEF_NONE]=(RGB_WHITE); // (RGB_GREEN)
	m_rgbDef[DEF_NICK]=(RGB_MAGENTA);
	m_rgbDef[DEF_PROTRUSION]=(RGB_SKYBLUE);
	m_rgbDef[DEF_SPACE]=(RGB_LTGREEN);
	m_rgbDef[DEF_OPEN]=(RGB_LTRED);
	m_rgbDef[DEF_SHORT]=(RGB_RED);
	m_rgbDef[DEF_USHORT]=(RGB_LTCYAN);
	m_rgbDef[DEF_PINHOLE]=(RGB_LLTGREEN);
	m_rgbDef[DEF_HOLE_MISS]=(RGB_LTBLUE);
	m_rgbDef[DEF_EXTRA]=(RGB_CLOUDBLUE);
	m_rgbDef[DEF_PAD]=(RGB_LTPURPLE);
	m_rgbDef[DEF_HOLE_POSITION]=(RGB_PINK);
	m_rgbDef[DEF_POI]=(RGB_LTMAGENTA);
	m_rgbDef[DEF_VH_POSITION]=(RGB_LTYELLOW);
	m_rgbDef[DEF_VH_MISS]=(RGB_BOON);
	m_rgbDef[DEF_HOLE_DEFECT]=(RGB_LTPINK);
	m_rgbDef[DEF_HOLE_OPEN]=(RGB_YELLOWGREEN);
	m_rgbDef[DEF_VH_OPEN]=(RGB_RED);
	m_rgbDef[DEF_VH_DEF]=(RGB_BROWN);
	m_rgbDef[DEF_LIGHT]=(RGB_YELLOW);

	m_cBigDef[0] = '*';		//	NONE
	m_cBigDef[1] = 'N';		//	NICK
	m_cBigDef[2] = 'D';		//	PROTRUSION
	m_cBigDef[3] = 'A';		//	SPACE
	m_cBigDef[4] = 'O';		//	OPEN
	m_cBigDef[5] = 'S';		//	SHORT
	m_cBigDef[6] = 'U';		//	USHORT
	m_cBigDef[7] = 'I';		//	PINHOLE
	m_cBigDef[8] = 'H';		//	HOLE_MISS
	m_cBigDef[9] = 'E';		//	EXTRA
	m_cBigDef[10] = 'P';	//	PAD
	m_cBigDef[11] = 'L';	//	HOLE_POSITION
	m_cBigDef[12] = 'X';	//	POI
	m_cBigDef[13] = 'T';	//	VH_POSITION
	m_cBigDef[14] = 'M';	//	VH_MISS
	m_cBigDef[15] = 'F';	//	HOLE_DEFECT
	m_cBigDef[16] = 'C';	//	HOLE_OPEN
	m_cBigDef[17] = 'G';	//	VH_OPEN
	m_cBigDef[18] = 'V';	//	VH_DEF

	m_cSmallDef[0] = '*';
	m_cSmallDef[1] = 'n';
	m_cSmallDef[2] = 'd';
	m_cSmallDef[3] = 'a';
	m_cSmallDef[4] = 'o';
	m_cSmallDef[5] = 's';
	m_cSmallDef[6] = 'u';
	m_cSmallDef[7] = 'i';
	m_cSmallDef[8] = 'h';
	m_cSmallDef[9] = 'e';
	m_cSmallDef[10] = 'p';
	m_cSmallDef[11] = 'l';
	m_cSmallDef[12] = 'x';
	m_cSmallDef[13] = 't';
	m_cSmallDef[14] = 'm';
	m_cSmallDef[15] = 'f';
	m_cSmallDef[16] = 'c';
	m_cSmallDef[17] = 'g';
	m_cSmallDef[18] = 'v';

	m_pPnlNum = NULL;
	m_pPnlDefNum = NULL;
	pPcsDef = NULL;
	pFrmRgn = NULL;
	pPcsRgn = NULL;
// 	pMkInfo = NULL;

	m_pPnlBuf = NULL;
	m_nPnlBuf = 0;

	if(nPnl>0 && nPcs>0)
	{
		pFrmRgn = new CRect[nPnl];
		m_pPnlNum = new int[nPnl];
		m_pPnlDefNum = new int[nPnl];
	}

	m_bThreadAliveRemakeReelmap = FALSE;
	m_sPathOnThread = _T("");
	
	m_bThreadAliveReloadReelmap = FALSE;
	m_nLastOnThread = 0;
	m_nTotalProgressReloadReelmap = 0;
	m_nProgressReloadReelmap = 0;
	m_bDoneReloadReelmap = FALSE;

	//m_nProgressRemakeReelmapFromPcr = 0;
	//m_nTotalProgressRemakeReelmapFromPcr = 0;
	//m_bDoneRemakeReelmapFromPcr = FALSE;

	m_nSelMarkingPnl = 2;
	m_nWritedSerial = 0;

	m_nLayer = nLayer;
	m_sPathBuf = GetRmapPath(m_nLayer);
	m_sPathYield = GetYieldPath(m_nLayer);


	LoadConfig();
 	InitRst();
	InitPcs();
	ClrPnlNum();	
	ClrPnlDefNum();
	ClrFixPcs();

	m_nStartSerial = 0;

	// ITS
	//m_pPnlBufIts = NULL;
	//m_nPnlBufIts = 0;

	//ResetYield();

	m_bThreadAliveFinalCopyItsFiles = FALSE;

	for (nC = 0; nC < FIX_PCS_COL_MAX; nC++)
		for (nR = 0; nR < FIX_PCS_ROW_MAX; nR++)
			m_FixPcsRpt[nC][nR] = 0;
}

CReelMap::~CReelMap()
{
	int k;

	m_bThreadAliveReloadReelmap = FALSE;
	m_bThreadAliveRemakeReelmap = FALSE;
	m_bThreadAliveFinalCopyItsFiles = FALSE;


	CloseRst();

// 	if(pMkInfo)
// 	{
// 		delete[] pMkInfo;
// 		pMkInfo = NULL;
// 	}

	if(m_pPnlNum)
	{
		delete[] m_pPnlNum;
		m_pPnlNum = NULL;
	}

	if(m_pPnlDefNum)
	{
		delete[] m_pPnlDefNum;
		m_pPnlDefNum = NULL;
	}

	if(pFrmRgn)
	{
		delete[] pFrmRgn;
		pFrmRgn = NULL;
	}

	if(pPcsRgn)
	{
		for(k=0; k<nTotPnl; k++)
		{
			delete[] pPcsRgn[k];
			pPcsRgn[k] = NULL;
		}
		delete[] pPcsRgn;
		pPcsRgn = NULL;
	}

	if(pPcsDef)
	{
		for(k=0; k<nTotPnl; k++)
		{
			delete[] pPcsDef[k];
			pPcsDef[k] = NULL;
		}

		delete[] pPcsDef;
		pPcsDef = NULL;
	}
}


BEGIN_MESSAGE_MAP(CReelMap, CWnd)
	//{{AFX_MSG_MAP(CReelMap)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CReelMap message handlers
void CReelMap::LoadConfig()
{
	TCHAR szData[MAX_PATH];
	TCHAR sep[] = { _T(",;\r\n\t") };
	CString sVal;

	if (0 < ::GetPrivateProfileString(_T("REELMAP"), _T("BackGround"), NULL, szData, sizeof(szData), PATH_CONFIG))
	{
		sVal = _tcstok(szData, sep);
		m_nBkColor[0] = _tstoi(sVal);
		sVal = _tcstok(NULL,sep);
		m_nBkColor[1] = _tstoi(sVal);
		sVal = _tcstok(NULL,sep);
		m_nBkColor[2] = _tstoi(sVal);
	}

	//if (pView && pView->m_pDts)
	//{
	//	if (pView->m_pDts->IsUseDts())
	//	{
	//		LoadDefectTableDB();
	//		return;
	//	}
	//	else
	//	{
	//		LoadDefectTableIni();
	//	}
	//}
	//else
	{
		LoadDefectTableIni();
	}
}

BOOL CReelMap::LoadDefectTableDB()
{
	if (pView && pView->m_pDts)
	{
		COLORREF rgbDef[MAX_PATH];
		int nDefCode[MAX_PATH], nMaxR, nMaxC;
		CString sEngN[MAX_PATH], sKorN[MAX_PATH];

		return pView->m_pDts->LoadDefectTable(nDefCode, rgbDef, sKorN, sEngN, &nMaxR, &nMaxC);
	}

	return FALSE;
}

BOOL CReelMap::LoadDefectTableIni()
{
	TCHAR szData[200];
	TCHAR sep[] = { _T(",;\r\n\t") };
	CString sIdx, sVal;
	int k;

	for(k=1; k < MAX_DEF; k++)
	{
		sIdx.Format(_T("%d"), k);
		if (0 < ::GetPrivateProfileString(_T("DEFECT"), sIdx, NULL, szData, sizeof(szData), PATH_CONFIG))
		{
			sVal = _tcstok(szData, sep);
			m_sEngDef[k].Format(_T("%s"), sVal);
			sVal = _tcstok(NULL, sep);
			m_sKorDef[k].Format(_T("%s"), sVal);
			sVal = _tcstok(NULL, sep);
			m_cBigDef[k] = sVal.GetAt(0);
			sVal = _tcstok(NULL, sep);
			m_cSmallDef[k] = sVal.GetAt(0);
			sVal = _tcstok(NULL, sep);
			m_rgbDef[k] = (COLORREF)_tstoi(sVal);
			sVal = _tcstok(NULL, sep);
			m_nOdr[k] = _tstoi(sVal);
		}
		else
		{
			pView->ClrDispMsg();
			AfxMessageBox(_T("Error - LoadDefectTableIni()"));
			return FALSE;
		}
	}

	return TRUE;
}

void CReelMap::SetRgbDef(int nDef, COLORREF rgbVal)
{
	m_rgbDef[nDef]=rgbVal;
}

void CReelMap::SetPnlNum(int *pPnlNum)
{
	if(!m_pPnlNum)
		return;

	int k;
	if(pPnlNum)
	{
		for(k=0; k<nTotPnl; k++)
			m_pPnlNum[k]=pPnlNum[k];
	}
	else
	{
		for(k=0; k<nTotPnl; k++)
			m_pPnlNum[k]=-1;
	}
}

void CReelMap::SetPnlDefNum(int *pPnlDefNum)
{
	if(!m_pPnlDefNum)
		return;

	int k;
	if(pPnlDefNum)
	{
		for(k=0; k<nTotPnl; k++)
			m_pPnlDefNum[k] = pPnlDefNum[k];
	}
	else
	{
		for(k=0; k<nTotPnl; k++)
			m_pPnlDefNum[k] = -1;
	}
}

void CReelMap::ClrPnlNum()
{
	for(int k=0; k<nTotPnl; k++)
		m_pPnlNum[k]=-1;
}

void CReelMap::ClrPnlDefNum()
{
	for(int k=0; k<nTotPnl; k++)
		m_pPnlDefNum[k] = -1;
}

// Panel Index k increases from 0 to (nTotPnl-1)...
void CReelMap::IncPnlNum()
{
	if(!m_pPnlNum)
		return;

	int k;
	for(k=nTotPnl-1; k>0; k--)
		m_pPnlNum[k] = m_pPnlNum[k-1];
	m_pPnlNum[0]++;
}

void CReelMap::AddPnlDefNum(int nDef)
{
	if(!m_pPnlDefNum)
		return;

	int k;
	for(k=nTotPnl-1; k>0; k--)
		m_pPnlDefNum[k] = m_pPnlDefNum[k-1];
	m_pPnlDefNum[0] = nDef;
}

void CReelMap::AddPnlNum(int nNum)
{
	if(!m_pPnlNum)
		return;

	int k;
	for(k=nTotPnl-1; k>0; k--)
		m_pPnlNum[k] = m_pPnlNum[k-1];
	m_pPnlNum[0] = nNum;
}

void CReelMap::SelMarkingPnl(int nNum)
{
	m_nSelMarkingPnl = nNum;
}

void CReelMap::SetAdjRatio(double dRatio)
{
	if (dRatio < 1.0)
		dRatio = 1.0;

	m_dAdjRatio = dRatio;
}

double CReelMap::GetAdjRatio()
{
	return m_dAdjRatio;
}

BOOL CReelMap::Open(CString sPath)
{
	if (pDoc->GetTestMode() == MODE_OUTER)
		MakeItsReelmapHeader();
		//pDoc->MakeLayerMappingHeader();

	return MakeHeader(sPath);
//	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;
//	int nNodeX = 0, nNodeY = 0;
//#ifndef TEST_MODE
//	nNodeX = pDoc->m_Master[0].m_pPcsRgn->nCol;
//	nNodeY = pDoc->m_Master[0].m_pPcsRgn->nRow;
//#endif
//
//	int k, i;
//	FILE *fp = NULL;
//	char FileName[MAX_PATH];
//
//	BOOL bExist = FALSE;
//	CFileFind findfile;
//	if (findfile.FindFile(sPath))
//	{
//		bExist = TRUE;
//		return TRUE;
//	}
//	else
//		MakeDirRmap();
//
//	StrToChar(sPath, FileName);
//
//	fp = fopen(FileName, "a+");
//	if (fp == NULL)
//	{
//		pView->MsgBox(_T("It is trouble to open ReelMap.txt"));
//// 		AfxMessageBox(_T("It is trouble to open ReelMap.txt"),MB_ICONWARNING|MB_OK);
//		return FALSE; 
//	}
//
//	//m_sPathShare.Format(_T("%s"), sPath);
//
//	char* pRtn = NULL;
//	if(!bExist)
//	{
//		InitRst();
//		InitPcs();
//
//		fprintf(fp, "[Info]\n");
//		fprintf(fp, "��  ��  �� = %s\n", pRtn = StrToChar(pDoc->WorkingInfo.System.sMcName)); if (pRtn) delete pRtn; pRtn = NULL;
//		fprintf(fp, "��  ��  �� = %s\n", pRtn = StrToChar(pDoc->WorkingInfo.LastJob.sSelUserName)); if (pRtn) delete pRtn; pRtn = NULL;
//		fprintf(fp, "��      �� = %s\n", pRtn = StrToChar(pDoc->WorkingInfo.LastJob.sModelUp)); if (pRtn) delete pRtn; pRtn = NULL;
//		fprintf(fp, "��      Ʈ = %s\n", pRtn = StrToChar(pDoc->WorkingInfo.LastJob.sLotUp)); if (pRtn) delete pRtn; pRtn = NULL;
//		fprintf(fp, "��鷹�̾� = %s\n", pRtn = StrToChar(pDoc->WorkingInfo.LastJob.sLayerUp)); if (pRtn) delete pRtn; pRtn = NULL;
//		if (bDualTest)
//		{
//			fprintf(fp, "�ϸ鷹�̾� = %s\n", pRtn = StrToChar(pDoc->WorkingInfo.LastJob.sLayerDn)); if (pRtn) delete pRtn; pRtn = NULL;
//		}
//		fprintf(fp, "\n");
//		fprintf(fp, "Process Code = \n");
//		fprintf(fp, "Entire Speed = \n");
//		fprintf(fp, "\n");
//
//		fprintf(fp, "CamPcsX = %d\n", nNodeX);
//		fprintf(fp, "CamPcsY = %d\n", nNodeY);
//		
//		fprintf(fp, "\n");
//		fprintf(fp, "0 -> ��ǰ\n");
//		for (i = 1; i < MAX_DEF; i++)
//		{
//			fprintf(fp, "%d -> %s\n", i, pRtn = StrToChar(m_sKorDef[i])); if (pRtn) delete pRtn; pRtn = NULL; // m_cBigDef[i]
//		}
//		fprintf(fp, "? - Missed Align Panel(i.e �뱤�ҷ�)\n");
//		fprintf(fp, "\n");
//		fprintf(fp, "Lot Start=\n");
//		fprintf(fp, "Lot Run=\n");
//		fprintf(fp, "Lot End=\n");
//		fprintf(fp, "\n");
//		fprintf(fp, "Marked Shot=\n");
//		fprintf(fp, "Marked Date=\n");
//		fprintf(fp, "\n");
//		fprintf(fp, "\n");
//		fprintf(fp, "// < ���� ���� > \n");
//		fprintf(fp, "\n");
//		fprintf(fp, "Start Serial=\n");
//		fprintf(fp, "End Serial=\n");
//		fprintf(fp, "\n");
//		fprintf(fp, "Total Pcs=\n");
//		fprintf(fp, "Good Pcs=\n");
//		fprintf(fp, "Bad Pcs=\n");
//		fprintf(fp, "\n");
//		fprintf(fp, "Strip0=\n");
//		fprintf(fp, "Strip1=\n");
//		fprintf(fp, "Strip2=\n");
//		fprintf(fp, "Strip3=\n");
//		for(i=1; i<MAX_DEF; i++)
//			fprintf(fp, "%d=\n", i); // m_cBigDef[i]
//		fprintf(fp, "\n");
//
//		for(k=0; k<MAX_STRIP_NUM; k++)
//		{
//			fprintf(fp, "[Strip%d]\n", k);
//			for(i=1; i<MAX_DEF; i++)
//				fprintf(fp, "%d=\n", i); // m_cBigDef[i]
//			fprintf(fp, "\n");
//		}
//
//		fprintf(fp, "[StripOut]\n");
//		fprintf(fp, "Total=\n");
//		for(k=0; k<MAX_STRIP_NUM; k++)
//			fprintf(fp, "%d=\n", k);
//		fprintf(fp, "\n");
//	}
//
//	fclose(fp);
//
//
//	return TRUE;
}

BOOL CReelMap::OpenUser(CString sPath)
{
	if (pDoc->GetTestMode() == MODE_OUTER)
		MakeItsReelmapHeader();

	//	pDoc->MakeLayerMappingHeader();

	return MakeHeader(sPath);
//	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;
//
//	int nNodeX = pDoc->m_Master[0].m_pPcsRgn->nCol;
//	int nNodeY = pDoc->m_Master[0].m_pPcsRgn->nRow;
//
//	int k, i;
//	FILE *fp = NULL;
//	char FileName[MAX_PATH];
//
//
//	CString sFile=_T(""), sUserRmapPath=sPath;
//	int nPos = sUserRmapPath.ReverseFind('\\');
//	if (nPos != -1)
//	{
//		sFile = sUserRmapPath.Right(sUserRmapPath.GetLength()-nPos-1);
//		sUserRmapPath.Delete(nPos, sPath.GetLength() - nPos);
//	}
//
//	BOOL bExist = FALSE;
//	CFileFind findfile;
//	sPath = sUserRmapPath + _T("\\Reelmap");
//	if(findfile.FindFile(sPath))
//		bExist = TRUE;
//	else
//		MakeDirRmap();
//
//// 	if(bExist)
//// 	{
//// 		char szData[100];
//// 		if (0 >= ::GetPrivateProfileString(_T("Info"), _T("��      ��"), NULL, szData, sizeof(szData), sPath))
//// 			bExist = FALSE;
//// 	}
//
//	sPath = sUserRmapPath + _T("\\Reelmap\\") + sFile;
//	//_tcscpy(FileName, sPath);
//	StrToChar(sPath, FileName);
//
//
//	fp = fopen(FileName, "a+");
//	if (fp == NULL)
//	{
//		pView->MsgBox(_T("It is trouble to open ReelMap.txt"));
//		return FALSE; 
//	}
//	char* pRtn = NULL;
//	if(!bExist)
//	{
//		InitRst();
//		InitPcs();
//
//		fprintf(fp, "[Info]\n");
//		fprintf(fp, "��  ��  �� = %s\n", pRtn = StrToChar(pDoc->WorkingInfo.System.sMcName)); if (pRtn) delete pRtn; pRtn = NULL;
//		fprintf(fp, "��  ��  �� = %s\n", pRtn = StrToChar(pDoc->WorkingInfo.LastJob.sSelUserName)); if (pRtn) delete pRtn; pRtn = NULL;
//		fprintf(fp, "��      �� = %s\n", pRtn = StrToChar(pDoc->WorkingInfo.LastJob.sModelUp)); if (pRtn) delete pRtn; pRtn = NULL;
//		fprintf(fp, "��      Ʈ = %s\n", pRtn = StrToChar(pDoc->WorkingInfo.LastJob.sLotUp)); if (pRtn) delete pRtn; pRtn = NULL;
//		fprintf(fp, "��鷹�̾� = %s\n", pRtn = StrToChar(pDoc->WorkingInfo.LastJob.sLayerUp)); if (pRtn) delete pRtn; pRtn = NULL;
//		if (bDualTest)
//		{
//			fprintf(fp, "�ϸ鷹�̾� = %s\n", pRtn = StrToChar(pDoc->WorkingInfo.LastJob.sLayerDn)); if (pRtn) delete pRtn; pRtn = NULL;
//		}
//		fprintf(fp, "\n");
//		fprintf(fp, "Process Code = \n");
//		fprintf(fp, "Entire Speed = \n");
//		fprintf(fp, "���� ��Ʈ�� ����[%%] : %.1f\n", _tstof(pDoc->WorkingInfo.LastJob.sStripOutRatio));
//		fprintf(fp, "\n");
//		
//		fprintf(fp, "CamPcsX = %d\n", nNodeX);
//		fprintf(fp, "CamPcsY = %d\n", nNodeY);
//		
//		fprintf(fp, "\n");
//		fprintf(fp, "0 -> ��ǰ\n");
//		for (i = 1; i < MAX_DEF; i++)
//		{
//			fprintf(fp, "%c -> %s\n", pDoc->m_cBigDefCode[i], pRtn = StrToChar(m_sKorDef[i])); if (pRtn) delete pRtn; pRtn = NULL; // m_cBigDef[i]
//		}
//		fprintf(fp, "? - Missed Align Panel(i.e �뱤�ҷ�)\n");
//		fprintf(fp, "\n");
//		fprintf(fp, "Lot Start=\n");
//		fprintf(fp, "Lot Run=\n");
//		fprintf(fp, "Lot End=\n");
//		fprintf(fp, "\n");
//		fprintf(fp, "Marked Shot=\n");
//		fprintf(fp, "Marked Date=\n");
//		fprintf(fp, "\n");
//		fprintf(fp, "\n");
//		fprintf(fp, "// < ���� ���� > \n");
//		fprintf(fp, "\n");
//		fprintf(fp, "Start Serial=\n");
//		fprintf(fp, "End Serial=\n");
//		fprintf(fp, "\n");
//		fprintf(fp, "Total Pcs=\n");
//		fprintf(fp, "Good Pcs=\n");
//		fprintf(fp, "Bad Pcs=\n");
//		fprintf(fp, "\n");
//		fprintf(fp, "Strip0=\n");
//		fprintf(fp, "Strip1=\n");
//		fprintf(fp, "Strip2=\n");
//		fprintf(fp, "Strip3=\n");
//		for(i=1; i<MAX_DEF; i++)
//			fprintf(fp, "%d=\n", i); // m_cBigDef[i]
//		fprintf(fp, "\n");
//
//		for(k=0; k<MAX_STRIP_NUM; k++)
//		{
//			fprintf(fp, "[Strip%d]\n", k);
//			for(i=1; i<MAX_DEF; i++)
//				fprintf(fp, "%d=\n", i); // m_cBigDef[i]
//			fprintf(fp, "\n");
//		}
//	}
//
//	fclose(fp);
//
//	return TRUE;
}

BOOL CReelMap::Open()
{
	CString sPath = GetRmapPath(m_nLayer);
	//m_sPathShare = sPath;
	if (pDoc->GetTestMode() == MODE_OUTER)
		MakeItsReelmapHeader();

	return MakeHeader(sPath);
//	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;
//
//	int nNodeX = pDoc->m_Master[0].m_pPcsRgn->nCol;
//	int nNodeY = pDoc->m_Master[0].m_pPcsRgn->nRow;
//
//	int k, i;
//	FILE *fp = NULL;
//	char FileName[MAX_PATH];
//
//	BOOL bExist = FALSE;
//	CFileFind findfile;
//	if(findfile.FindFile(sPath))
//		bExist = TRUE;
//	else
//		MakeDirRmap(sModel, sLayer, sLot);
//
//// 	if(bExist)
//// 	{
//// 		char szData[100];
//// 		if (0 >= ::GetPrivateProfileString(_T("Info"), _T("��      ��"), NULL, szData, sizeof(szData), sPath))
//// 			bExist = FALSE;
//// 	}
//
//	//_tcscpy(FileName, sPath);
//	StrToChar(sPath, FileName);
//
//	fp = fopen(FileName, "a+");
//	if (fp == NULL)
//	{
//		pView->MsgBox(_T("It is trouble to open ReelMap.txt"));
//// 		AfxMessageBox(_T("It is trouble to open ReelMap.txt"),MB_ICONWARNING|MB_OK);
//		return FALSE; 
//	}
//
//	m_sPathShare = sPath;
//	char* pRtn = NULL;
//	if(!bExist)
//	{
//		InitRst();
//		InitPcs();
//
//		fprintf(fp, "[Info]\n");
//		fprintf(fp, "��  ��  �� = %s\n", pRtn = StrToChar(pDoc->WorkingInfo.System.sMcName)); if (pRtn) delete pRtn; pRtn = NULL;
//		fprintf(fp, "��  ��  �� = %s\n", pRtn = StrToChar(pDoc->WorkingInfo.LastJob.sSelUserName)); if (pRtn) delete pRtn; pRtn = NULL;
//		fprintf(fp, "��      �� = %s\n", pRtn = StrToChar(pDoc->WorkingInfo.LastJob.sModelUp)); if (pRtn) delete pRtn; pRtn = NULL;
//		fprintf(fp, "��      Ʈ = %s\n", pRtn = StrToChar(pDoc->WorkingInfo.LastJob.sLotUp)); if (pRtn) delete pRtn; pRtn = NULL;
//		fprintf(fp, "��鷹�̾� = %s\n", pRtn = StrToChar(pDoc->WorkingInfo.LastJob.sLayerUp)); if (pRtn) delete pRtn; pRtn = NULL;
//		if (bDualTest)
//		{
//			fprintf(fp, "�ϸ鷹�̾� = %s\n", pRtn = StrToChar(pDoc->WorkingInfo.LastJob.sLayerDn)); if (pRtn) delete pRtn; pRtn = NULL;
//		}
//		fprintf(fp, "\n");
//		fprintf(fp, "Process Code = \n");
//		fprintf(fp, "Entire Speed = \n");
//		fprintf(fp, "\n");
//		
//		fprintf(fp, "CamPcsX = %d\n", nNodeX);
//		fprintf(fp, "CamPcsY = %d\n", nNodeY);
//		
//		fprintf(fp, "\n");
//		fprintf(fp, "0 -> ��ǰ\n");
//		for (i = 1; i < MAX_DEF; i++)
//		{
//			fprintf(fp, "%d -> %s\n", i, pRtn = StrToChar(m_sKorDef[i])); if (pRtn) delete pRtn; pRtn = NULL; // m_cBigDef[i]
//		}
//		fprintf(fp, "? - Missed Align Panel(i.e �뱤�ҷ�)\n");
//		fprintf(fp, "\n");
//		fprintf(fp, "Lot Start=\n");
//		fprintf(fp, "Lot Run=\n");
//		fprintf(fp, "Lot End=\n");
//		fprintf(fp, "\n");
//		fprintf(fp, "Marked Shot=\n");
//		fprintf(fp, "Marked Date=\n");
//		fprintf(fp, "\n");
//		fprintf(fp, "\n");
//		fprintf(fp, "// < ���� ���� > \n");
//		fprintf(fp, "\n");
//		fprintf(fp, "Start Serial=\n");
//		fprintf(fp, "End Serial=\n");
//		fprintf(fp, "\n");
//		fprintf(fp, "Total Pcs=\n");
//		fprintf(fp, "Good Pcs=\n");
//		fprintf(fp, "Bad Pcs=\n");
//		fprintf(fp, "\n");
//		fprintf(fp, "Strip0=\n");
//		fprintf(fp, "Strip1=\n");
//		fprintf(fp, "Strip2=\n");
//		fprintf(fp, "Strip3=\n");
//		for(i=1; i<MAX_DEF; i++)
//			fprintf(fp, "%d=\n", i); // m_cBigDef[i]
//		fprintf(fp, "\n");
//
//		for(k=0; k<MAX_STRIP_NUM; k++)
//		{
//			fprintf(fp, "[Strip%d]\n", k);
//			for(i=1; i<MAX_DEF; i++)
//				fprintf(fp, "%d=\n", i); // m_cBigDef[i]
//			fprintf(fp, "\n");
//		}
//	}
//
//	fclose(fp);
//// 	if(!OpenUser(sPath, sModel, sLayer, sLot))
//// 		return FALSE;
//
//	return TRUE;
}

BOOL CReelMap::OpenUser(CString sPath, CString sModel, CString sLayer, CString sLot)
{
	if (pDoc->GetTestMode() == MODE_OUTER)
		MakeItsReelmapHeader();

	return MakeHeader(sPath);
//	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;
//
//	int nNodeX = pDoc->m_Master[0].m_pPcsRgn->nCol;
//	int nNodeY = pDoc->m_Master[0].m_pPcsRgn->nRow;
//
//	int k, i;
//	FILE *fp = NULL;
//	char FileName[MAX_PATH];
//
//	CString sFile=_T(""), sUserRmapPath=sPath;
//	int nPos = sUserRmapPath.ReverseFind('\\');
//	if (nPos != -1)
//	{
//		sFile = sUserRmapPath.Right(sUserRmapPath.GetLength()-nPos-1);
//		sUserRmapPath.Delete(nPos, sPath.GetLength() - nPos);
//	}
//
//	BOOL bExist = FALSE;
//	CFileFind findfile;
//	sPath = sUserRmapPath + _T("\\Reelmap");
//
//	if(findfile.FindFile(sPath))
//		bExist = TRUE;
//	else
//		MakeDirRmap(sModel, sLayer, sLot);
//
//// 	if(bExist)
//// 	{
//// 		char szData[100];
//// 		if (0 >= ::GetPrivateProfileString(_T("Info"), _T("��      ��"), NULL, szData, sizeof(szData), sPath))
//// 			bExist = FALSE;
//// 	}
//
//	sPath = sUserRmapPath + _T("\\Reelmap\\") + sFile;
//	//_tcscpy(FileName, sPath);
//	StrToChar(sPath, FileName);
//
//	fp = fopen(FileName, "a+");
//	if (fp == NULL)
//	{
//		pView->MsgBox(_T("It is trouble to open ReelMap.txt"));
//		return FALSE; 
//	}
//	char* pRtn = NULL;
//	if(!bExist)
//	{
//		InitRst();
//		InitPcs();
//
//		fprintf(fp, "[Info]\n");
//		fprintf(fp, "��  ��  �� = %s\n", pRtn = StrToChar(pDoc->WorkingInfo.System.sMcName)); if (pRtn) delete pRtn; pRtn = NULL;
//		fprintf(fp, "��  ��  �� = %s\n", pRtn = StrToChar(pDoc->WorkingInfo.LastJob.sSelUserName)); if (pRtn) delete pRtn; pRtn = NULL;
//		fprintf(fp, "��      �� = %s\n", pRtn = StrToChar(pDoc->WorkingInfo.LastJob.sModelUp)); if (pRtn) delete pRtn; pRtn = NULL;
//		fprintf(fp, "��      Ʈ = %s\n", pRtn = StrToChar(pDoc->WorkingInfo.LastJob.sLotUp)); if (pRtn) delete pRtn; pRtn = NULL;
//		fprintf(fp, "��鷹�̾� = %s\n", pRtn = StrToChar(pDoc->WorkingInfo.LastJob.sLayerUp)); if (pRtn) delete pRtn; pRtn = NULL;
//		if (bDualTest)
//		{
//			fprintf(fp, "�ϸ鷹�̾� = %s\n", pRtn = StrToChar(pDoc->WorkingInfo.LastJob.sLayerDn)); if (pRtn) delete pRtn; pRtn = NULL;
//		}
//		fprintf(fp, "\n");
//		fprintf(fp, "Process Code = \n");
//		fprintf(fp, "Entire Speed = \n");
//		fprintf(fp, "���� ��Ʈ�� ����[%%] : %.1f\n", _tstof(pDoc->WorkingInfo.LastJob.sStripOutRatio));
//		fprintf(fp, "\n");
//		
//		fprintf(fp, "CamPcsX = %d\n", nNodeX);
//		fprintf(fp, "CamPcsY = %d\n", nNodeY);
//		
//		fprintf(fp, "\n");
//		fprintf(fp, "0 -> ��ǰ\n");
//		for (i = 1; i < MAX_DEF; i++)
//		{
//			fprintf(fp, "%c -> %s\n", pDoc->m_cBigDefCode[i], pRtn = StrToChar(m_sKorDef[i])); if (pRtn) delete pRtn; pRtn = NULL; // m_cBigDef[i]
//		}
//		fprintf(fp, "? - Missed Align Panel(i.e �뱤�ҷ�)\n");
//		fprintf(fp, "\n");
//		fprintf(fp, "Lot Start=\n");
//		fprintf(fp, "Lot Run=\n");
//		fprintf(fp, "Lot End=\n");
//		fprintf(fp, "\n");
//		fprintf(fp, "Marked Shot=\n");
//		fprintf(fp, "Marked Date=\n");
//		fprintf(fp, "\n");
//		fprintf(fp, "\n");
//		fprintf(fp, "// < ���� ���� > \n");
//		fprintf(fp, "\n");
//		fprintf(fp, "Start Serial=\n");
//		fprintf(fp, "End Serial=\n");
//		fprintf(fp, "\n");
//		fprintf(fp, "Total Pcs=\n");
//		fprintf(fp, "Good Pcs=\n");
//		fprintf(fp, "Bad Pcs=\n");
//		fprintf(fp, "\n");
//		fprintf(fp, "Strip0=\n");
//		fprintf(fp, "Strip1=\n");
//		fprintf(fp, "Strip2=\n");
//		fprintf(fp, "Strip3=\n");
//		for(i=1; i<MAX_DEF; i++)
//			fprintf(fp, "%d=\n", i); // m_cBigDef[i]
//		fprintf(fp, "\n");
//
//		for(k=0; k<MAX_STRIP_NUM; k++)
//		{
//			fprintf(fp, "[Strip%d]\n", k);
//			for(i=1; i<MAX_DEF; i++)
//				fprintf(fp, "%d=\n", i); // m_cBigDef[i]
//			fprintf(fp, "\n");
//		}
//	}
//
//	fclose(fp);
//
//	return TRUE;
}

//int CReelMap::Read(CString &sRead)
//{
//	FILE *fp;
//	char *FileData;
//	char FileD[200];
//	int nFileSize, nRSize, i;
//	CString sMsg;
//
//	CString sPath = GetRmapPath(m_nLayer);
//	StrToChar(sPath, FileD);
//	//_tcscpy(FileD, m_sPathShare);
//	//StrToChar(m_sPathShare, FileD);
//
//	if((fp = fopen(FileD, "r")) != NULL)
//	{
//		fseek(fp, 0, SEEK_END);
//		nFileSize = ftell(fp);
//		fseek(fp, 0, SEEK_SET);
//
//		/* Allocate space for a path name */
//		FileData = (char*)calloc(nFileSize, sizeof(char));
//
//		nRSize = 0;
//		for( i=0; i < nFileSize ; i++ )
//		{
//			if( feof( fp ) != 0 )
//				break;
//			FileData[i] = fgetc(fp);
//			nRSize++;
//		}
//
//		sRead.Format(_T("%s"), CharToString(FileData));
//
//		fclose(fp);
//		free( FileData );
//	}
//	else
//	{
//		sMsg.Format(_T("���������� ã�� ���߽��ϴ�.\r\n%s"), sPath);
//		//sMsg.Format(_T("���������� ã�� ���߽��ϴ�.\r\n%s"), m_sPathShare);
//		pView->MsgBox(sMsg);
//// 		AfxMessageBox(sMsg);
//		return 0;
//	}
//
//	return nRSize;
//}

BOOL CReelMap::Write(int nSerial)
{
	int nLayer = RMAP_NONE;

	if(nSerial <= 0)
	{
		pView->ClrDispMsg();
		AfxMessageBox(_T("Serial Error.62"));
		return 0;
	}

	if (pDoc->GetTestMode() == MODE_OUTER)
		MakeItsReelmapHeader();

	CString sPath = GetRmapPath(m_nLayer);
	MakeHeader(sPath);

	if (pDoc->GetTestMode() == MODE_INNER)
		pDoc->SetItsSerialInfo(nSerial);

	int nIdx = pDoc->GetPcrIdx1(nSerial, pDoc->m_bNewLotShare[1]);
	int nNodeX = pDoc->m_Master[0].m_pPcsRgn->nCol;
	int nNodeY = pDoc->m_Master[0].m_pPcsRgn->nRow;
	int nStripY = pDoc->m_Master[0].m_pPcsRgn->nRow / 4; // Strip(1~4);
	int nTotDefPcs = 0;

	if (m_nLayer == RMAP_UP || m_nLayer == RMAP_DN || m_nLayer == RMAP_ALLUP || m_nLayer == RMAP_ALLDN)
	{
		nLayer = m_nLayer - RMAP_UP;

		if (pDoc->m_pPcr[nLayer])
		{
			if (pDoc->m_pPcr[nLayer][nIdx])
				nTotDefPcs = pDoc->m_pPcr[nLayer][nIdx]->m_nTotDef;
		}
	}
	else if (m_nLayer == RMAP_INNER_UP || m_nLayer == RMAP_INNER_DN || m_nLayer == RMAP_INNER_ALLUP || m_nLayer == RMAP_INNER_ALLDN)
	{
		nLayer = m_nLayer - RMAP_INNER_UP;

		if (pDoc->m_pPcrInner[nLayer])
		{
			if (pDoc->m_pPcrInner[nLayer][nIdx])
				nTotDefPcs = pDoc->m_pPcrInner[nLayer][nIdx]->m_nTotDef;
		}
	}
	else if (m_nLayer == RMAP_ITS)
	{
		if (pDoc->m_pPcrIts)
		{
			if (pDoc->m_pPcrIts[nIdx])
				nTotDefPcs = pDoc->m_pPcrIts[nIdx]->m_nTotDef;
		}
	}
	else
		return 0;

	short **pPnlBuf;
	int i, nC, nR, nPcsId, nDefCode;//, nTot, nDef, nGood;
	pPnlBuf = new short*[nNodeY];
	for (i = 0; i < nNodeY; i++)
	{
		pPnlBuf[i] = new short[nNodeX];
		memset(pPnlBuf[i], 0, sizeof(short)*nNodeX);
		memset(m_pPnlBuf[nSerial - 1][i], 0, sizeof(short)*nNodeX);
	}

	CString strData, strTemp;
	int nTotVerifyed = 0;

	for (i = 0; i < nTotDefPcs; i++)
	{
		if (m_nLayer == RMAP_UP || m_nLayer == RMAP_DN || m_nLayer == RMAP_ALLUP || m_nLayer == RMAP_ALLDN)
		{
			nLayer = m_nLayer - RMAP_UP;

			if (pDoc->m_pPcr[nLayer][nIdx]->m_pMk[i] != -2) // -2 (NoMarking)
			{
				if (pDoc->WorkingInfo.System.bStripPcsRgnBin)	// DTS��
				{
					switch (pDoc->m_Master[0].MasterInfo.nActionCode)	// 0 : Rotation / Mirror ���� ����(CAM Data ����), 1 : �¿� �̷�, 2 : ���� �̷�, 3 : 180 ȸ��, 4 : 270 ȸ��(CCW), 5 : 90 ȸ��(CW)
					{
					case 0:
						nPcsId = pDoc->m_pPcr[nLayer][nIdx]->m_pDefPcs[i];
						break;
					case 1:
						nPcsId = pDoc->MirrorLR(pDoc->m_pPcr[nLayer][nIdx]->m_pDefPcs[i]);
						break;
					case 2:
						nPcsId = pDoc->MirrorUD(pDoc->m_pPcr[nLayer][nIdx]->m_pDefPcs[i]);
						break;
					case 3:
						nPcsId = pDoc->Rotate180(pDoc->m_pPcr[nLayer][nIdx]->m_pDefPcs[i]);
						break;
					default:
						nPcsId = pDoc->m_pPcr[nLayer][nIdx]->m_pDefPcs[i];
						break;
					}
				}
				else
					nPcsId = pDoc->m_pPcr[nLayer][nIdx]->m_pDefPcs[i];

				nDefCode = pDoc->m_pPcr[nLayer][nIdx]->m_pDefType[i];

				nC = int(nPcsId / nNodeY);
				if (nC % 2)	// Ȧ��.
					nR = nNodeY * (nC + 1) - nPcsId - 1;
				else		// ¦��.
					nR = nPcsId - nNodeY * nC;
				pPnlBuf[nR][nC] = (short)nDefCode;	// nPnl�� �� ����.
				if (m_pPnlBuf)
					m_pPnlBuf[nSerial - 1][nR][nC] = pPnlBuf[nR][nC]; // DefCode 3D Array : [nSerial][nRow][nCol] - �������� ������.
			}
			else
			{
				nTotVerifyed++;
			}
		}
		else if (m_nLayer == RMAP_INNER_UP || m_nLayer == RMAP_INNER_DN || m_nLayer == RMAP_INNER_ALLUP || m_nLayer == RMAP_INNER_ALLDN)
		{
			nLayer = m_nLayer - RMAP_INNER_UP;

			if (pDoc->m_pPcrInner[nLayer][nIdx]->m_pMk[i] != -2) // -2 (NoMarking)
			{
				if (pDoc->WorkingInfo.System.bStripPcsRgnBin)	// DTS��
				{
					switch (pDoc->m_MasterInner[0].MasterInfo.nActionCode)	// 0 : Rotation / Mirror ���� ����(CAM Data ����), 1 : �¿� �̷�, 2 : ���� �̷�, 3 : 180 ȸ��, 4 : 270 ȸ��(CCW), 5 : 90 ȸ��(CW)
					{
					case 0:
						nPcsId = pDoc->m_pPcrInner[nLayer][nIdx]->m_pDefPcs[i];
						break;
					case 1:
						nPcsId = pDoc->MirrorLR(pDoc->m_pPcrInner[nLayer][nIdx]->m_pDefPcs[i]);
						break;
					case 2:
						nPcsId = pDoc->MirrorUD(pDoc->m_pPcrInner[nLayer][nIdx]->m_pDefPcs[i]);
						break;
					case 3:
						nPcsId = pDoc->Rotate180(pDoc->m_pPcrInner[nLayer][nIdx]->m_pDefPcs[i]);
						break;
					default:
						nPcsId = pDoc->m_pPcrInner[nLayer][nIdx]->m_pDefPcs[i];
						break;
					}
				}
				else
					nPcsId = pDoc->m_pPcrInner[nLayer][nIdx]->m_pDefPcs[i];

				nDefCode = pDoc->m_pPcrInner[nLayer][nIdx]->m_pDefType[i];

				nC = int(nPcsId / nNodeY);
				if (nC % 2)	// Ȧ��.
					nR = nNodeY * (nC + 1) - nPcsId - 1;
				else		// ¦��.
					nR = nPcsId - nNodeY * nC;
				pPnlBuf[nR][nC] = (short)nDefCode;	// nPnl�� �� ����.
				if (m_pPnlBuf)
					m_pPnlBuf[nSerial - 1][nR][nC] = pPnlBuf[nR][nC]; // DefCode 3D Array : [nSerial][nRow][nCol] - �������� ������.
			}
			else
			{
				nTotVerifyed++;
			}
		}
		else if (m_nLayer == RMAP_ITS)
		{
			if (pDoc->m_pPcrIts[nIdx]->m_pMk[i] != -2) // -2 (NoMarking)
			{
				if (pDoc->WorkingInfo.System.bStripPcsRgnBin)	// DTS��
				{
					switch (pDoc->m_Master[0].MasterInfo.nActionCode)	// 0 : Rotation / Mirror ���� ����(CAM Data ����), 1 : �¿� �̷�, 2 : ���� �̷�, 3 : 180 ȸ��, 4 : 270 ȸ��(CCW), 5 : 90 ȸ��(CW)
					{
					case 0:
						nPcsId = pDoc->m_pPcrIts[nIdx]->m_pDefPcs[i];
						break;
					case 1:
						nPcsId = pDoc->MirrorLR(pDoc->m_pPcrIts[nIdx]->m_pDefPcs[i]);
						break;
					case 2:
						nPcsId = pDoc->MirrorUD(pDoc->m_pPcrIts[nIdx]->m_pDefPcs[i]);
						break;
					case 3:
						nPcsId = pDoc->Rotate180(pDoc->m_pPcrIts[nIdx]->m_pDefPcs[i]);
						break;
					default:
						nPcsId = pDoc->m_pPcrIts[nIdx]->m_pDefPcs[i];
						break;
					}
				}
				else
					nPcsId = pDoc->m_pPcrIts[nIdx]->m_pDefPcs[i];

				nDefCode = pDoc->m_pPcrIts[nIdx]->m_pDefType[i];

				nC = int(nPcsId / nNodeY);
				if (nC % 2)	// Ȧ��.
					nR = nNodeY * (nC + 1) - nPcsId - 1;
				else		// ¦��.
					nR = nPcsId - nNodeY * nC;
				pPnlBuf[nR][nC] = (short)nDefCode;	// nPnl�� �� ����.
				if (m_pPnlBuf)
					m_pPnlBuf[nSerial - 1][nR][nC] = pPnlBuf[nR][nC]; // DefCode 3D Array : [nSerial][nRow][nCol] - �������� ������.
			}
			else
			{
				nTotVerifyed++;
			}
		}
		else
			return 0;
	}

	CString sPnl, sRow;
	i = 0;
	sPnl.Format(_T("%d"), nSerial);
	strData.Format(_T("%d"), nTotDefPcs - nTotVerifyed);
	::WritePrivateProfileString(sPnl, _T("Total Defects"), strData, sPath);
	//::WritePrivateProfileString(sPnl, _T("Total Defects"), strData, m_sPathShare);

	for(int nRow=0; nRow<nNodeX; nRow++)			// ���� Text(90�� �ð�������� ȸ���� ���) ���� Row : Shot�� ù��° Col���� �����ؼ� ������ ������.
	{
		sRow.Format(_T("%02d"), nRow);
// 		sRow.Format(_T("%2d"), nRow+1);
		strData.Format(_T(""));
		strTemp.Format(_T(""));

// 		if(nRow==26)
// 			int brk=0;

		for(int nCol=0; nCol<nNodeY; nCol++)		// ���� Text(90�� �ð�������� ȸ���� ���) ���� Col : 4�� 3�� 2�� 1�� ��Ʈ������ ǥ�õ�.
		{
			nR = (nNodeY-1)-nCol;				// ���ʻ��� Row
			nC = nRow;							// ���ʻ��� Col

			if (m_nLayer == RMAP_UP || m_nLayer == RMAP_DN || m_nLayer == RMAP_ALLUP || m_nLayer == RMAP_ALLDN)
			{
				nLayer = m_nLayer - RMAP_UP;

				if (pDoc->m_pPcr[nLayer][nIdx]->m_nErrPnl == -1 || pDoc->m_pPcr[nLayer][nIdx]->m_nErrPnl == -2)
				{
					nDefCode = DEF_LIGHT;
					m_pPnlBuf[nSerial - 1][nR][nC] = (short)nDefCode;
				}
				else
					nDefCode = (int)pPnlBuf[nR][nC] < 0 ? 0 : (int)pPnlBuf[nR][nC];
			}
			else if (m_nLayer == RMAP_INNER_UP || m_nLayer == RMAP_INNER_DN || m_nLayer == RMAP_INNER_ALLUP || m_nLayer == RMAP_INNER_ALLDN)
			{
				nLayer = m_nLayer - RMAP_INNER_UP;

				if (pDoc->m_pPcrInner[nLayer][nIdx]->m_nErrPnl == -1 || pDoc->m_pPcrInner[nLayer][nIdx]->m_nErrPnl == -2)
				{
					nDefCode = DEF_LIGHT;
					m_pPnlBuf[nSerial - 1][nR][nC] = (short)nDefCode;
				}
				else
					nDefCode = (int)pPnlBuf[nR][nC] < 0 ? 0 : (int)pPnlBuf[nR][nC];
			}
			else if (m_nLayer == RMAP_ITS)
			{
				if (pDoc->m_pPcrIts[nIdx]->m_nErrPnl == -1 || pDoc->m_pPcrIts[nIdx]->m_nErrPnl == -2)
				{
					nDefCode = DEF_LIGHT;
					m_pPnlBuf[nSerial - 1][nR][nC] = (short)nDefCode;
				}
				else
					nDefCode = (int)pPnlBuf[nR][nC] < 0 ? 0 : (int)pPnlBuf[nR][nC];
			}
			else
				return 0;

			strTemp.Format(_T("%2d,"), nDefCode);	// �ҷ��ڵ带 2ĭ���� ����
			
			if(!nCol)								// strData�� ó������ �����͸� �߰�
				strData.Insert(0, strTemp);
			else
			{
				int nLen = strData.GetLength();
				if( !(nCol%nStripY) )				// Separate Strip (��Ʈ�� ����)
				{
					strData.Insert(nLen, _T("  "));
					nLen = strData.GetLength();
				}
				strData.Insert(nLen, strTemp);
			}
		}

		int nPos = strData.ReverseFind(',');		// ���� Text �� ������ ','�� ����
		strData.Delete(nPos, 1);
		::WritePrivateProfileString(sPnl, sRow, strData, sPath); // �� ���ξ� ���� Text�� ���.
		//::WritePrivateProfileString(sPnl, sRow, strData, m_sPathShare); // �� ���ξ� ���� Text�� ���.
	}	// ���� Text(90�� �ð�������� ȸ���� ���) ���� Row : Shot�� ������ Col���� ����ϰ� ����.

	for(i=0; i<nNodeY; i++)
		delete[]  pPnlBuf[i];
	delete[] pPnlBuf;

	m_nWritedSerial = nSerial;

	return TRUE;
}


//BOOL CReelMap::Write(int nSerial, int nLayer, CString sPath)
//{
//	if(nSerial <= 0)
//	{
//		pView->ClrDispMsg();
//		AfxMessageBox(_T("Serial Error.63"));
//		return 0;
//	}
//
//	if (pDoc->GetTestMode() == MODE_OUTER)
//		MakeItsReelmapHeader();
//
//	MakeHeader(sPath);
//
//	if (pDoc->GetTestMode() == MODE_INNER)
//		pDoc->SetItsSerialInfo(nSerial);
//
//	int nIdx = pDoc->GetPcrIdx1(nSerial, pDoc->m_bNewLotShare[1]);
//	int nNodeX = pDoc->m_Master[0].m_pPcsRgn->nCol;
//	int nNodeY = pDoc->m_Master[0].m_pPcsRgn->nRow;
//	int nStripY = pDoc->m_Master[0].m_pPcsRgn->nRow / MAX_STRIP_NUM; // Strip(1~4);
//	int nTotDefPcs = 0;
//
//	if (m_nLayer == RMAP_UP || m_nLayer == RMAP_DN || m_nLayer == RMAP_ALLUP || m_nLayer == RMAP_ALLDN)
//	{
//		if (pDoc->m_pPcr[nLayer])
//		{
//			if (pDoc->m_pPcr[nLayer][nIdx])
//				nTotDefPcs = pDoc->m_pPcr[nLayer][nIdx]->m_nTotDef;
//		}
//	}
//	else if (m_nLayer == RMAP_INNER_UP || m_nLayer == RMAP_INNER_DN || m_nLayer == RMAP_INNER_ALLUP || m_nLayer == RMAP_INNER_ALLDN)
//	{
//		if (pDoc->m_pPcrInner[nLayer])
//		{
//			if (pDoc->m_pPcrInner[nLayer][nIdx])
//				nTotDefPcs = pDoc->m_pPcrInner[nLayer][nIdx]->m_nTotDef;
//		}
//	}
//	else if (m_nLayer == RMAP_ITS)
//	{
//		if (pDoc->m_pPcrIts)
//		{
//			if (pDoc->m_pPcrIts[nIdx])
//				nTotDefPcs = pDoc->m_pPcrIts[nIdx]->m_nTotDef;
//		}
//	}
//	else
//		return 0;
//
//	short **pPnlBuf;
//	int i, nC, nR, nPcsId, nDefCode;//, nTot, nDef, nGood;
//	pPnlBuf = new short*[nNodeY];
//	for(i=0; i<nNodeY; i++)
//	{ 
//		pPnlBuf[i] = new short[nNodeX];
//		memset(pPnlBuf[i], 0, sizeof(short)*nNodeX);
//		memset(m_pPnlBuf[nSerial - 1][i], 0, sizeof(short)*nNodeX);
//	}
//
//	CString strData, strTemp;
//
//	if (m_nLayer == RMAP_UP || m_nLayer == RMAP_DN || m_nLayer == RMAP_ALLUP || m_nLayer == RMAP_ALLDN)
//	{
//		for (i = 0; i < nTotDefPcs; i++)
//		{
//			if (pDoc->m_pPcr[nLayer][nIdx]->m_pMk[i] != -2) // -2 (NoMarking)
//			{
//				if (pDoc->WorkingInfo.System.bStripPcsRgnBin)	// DTS��
//				{
//					switch (pDoc->m_Master[0].MasterInfo.nActionCode)	// 0 : Rotation / Mirror ���� ����(CAM Data ����), 1 : �¿� �̷�, 2 : ���� �̷�, 3 : 180 ȸ��, 4 : 270 ȸ��(CCW), 5 : 90 ȸ��(CW)
//					{
//					case 0:
//						nPcsId = pDoc->m_pPcr[nLayer][nIdx]->m_pDefPcs[i];
//						break;
//					case 1:
//						nPcsId = pDoc->MirrorLR(pDoc->m_pPcr[nLayer][nIdx]->m_pDefPcs[i]);
//						break;
//					case 2:
//						nPcsId = pDoc->MirrorUD(pDoc->m_pPcr[nLayer][nIdx]->m_pDefPcs[i]);
//						break;
//					case 3:
//						nPcsId = pDoc->Rotate180(pDoc->m_pPcr[nLayer][nIdx]->m_pDefPcs[i]);
//						break;
//					default:
//						nPcsId = pDoc->m_pPcr[nLayer][nIdx]->m_pDefPcs[i];
//						break;
//					}
//				}
//				else
//					nPcsId = pDoc->m_pPcr[nLayer][nIdx]->m_pDefPcs[i];
//
//				nDefCode = pDoc->m_pPcr[nLayer][nIdx]->m_pDefType[i];
//
//				nC = int(nPcsId / nNodeY);
//				if (nC % 2)	// Ȧ��.
//					nR = nNodeY * (nC + 1) - nPcsId - 1;
//				else		// ¦��.
//					nR = nPcsId - nNodeY * nC;
//				pPnlBuf[nR][nC] = (short)nDefCode;	// nPnl�� �� ����.
//				if (m_pPnlBuf)
//					m_pPnlBuf[nSerial - 1][nR][nC] = pPnlBuf[nR][nC];   // DefCode 3D Array : [nSerial][nRow][nCol] - �������� ������. #define PNL_TOT
//			}
//		}
//	}
//	else if (m_nLayer == RMAP_INNER_UP || m_nLayer == RMAP_INNER_DN || m_nLayer == RMAP_INNER_ALLUP || m_nLayer == RMAP_INNER_ALLDN)
//	{
//		for (i = 0; i < nTotDefPcs; i++)
//		{
//			if (pDoc->m_pPcrInner[nLayer][nIdx]->m_pMk[i] != -2) // -2 (NoMarking)
//			{
//				if (pDoc->WorkingInfo.System.bStripPcsRgnBin)	// DTS��
//				{
//					switch (pDoc->m_Master[0].MasterInfo.nActionCode)	// 0 : Rotation / Mirror ���� ����(CAM Data ����), 1 : �¿� �̷�, 2 : ���� �̷�, 3 : 180 ȸ��, 4 : 270 ȸ��(CCW), 5 : 90 ȸ��(CW)
//					{
//					case 0:
//						nPcsId = pDoc->m_pPcrInner[nLayer][nIdx]->m_pDefPcs[i];
//						break;
//					case 1:
//						nPcsId = pDoc->MirrorLR(pDoc->m_pPcrInner[nLayer][nIdx]->m_pDefPcs[i]);
//						break;
//					case 2:
//						nPcsId = pDoc->MirrorUD(pDoc->m_pPcrInner[nLayer][nIdx]->m_pDefPcs[i]);
//						break;
//					case 3:
//						nPcsId = pDoc->Rotate180(pDoc->m_pPcrInner[nLayer][nIdx]->m_pDefPcs[i]);
//						break;
//					default:
//						nPcsId = pDoc->m_pPcrInner[nLayer][nIdx]->m_pDefPcs[i];
//						break;
//					}
//				}
//				else
//					nPcsId = pDoc->m_pPcrInner[nLayer][nIdx]->m_pDefPcs[i];
//
//				nDefCode = pDoc->m_pPcrInner[nLayer][nIdx]->m_pDefType[i];
//
//				nC = int(nPcsId / nNodeY);
//				if (nC % 2)	// Ȧ��.
//					nR = nNodeY * (nC + 1) - nPcsId - 1;
//				else		// ¦��.
//					nR = nPcsId - nNodeY * nC;
//				pPnlBuf[nR][nC] = (short)nDefCode;	// nPnl�� �� ����.
//				if (m_pPnlBuf)
//					m_pPnlBuf[nSerial - 1][nR][nC] = pPnlBuf[nR][nC];   // DefCode 3D Array : [nSerial][nRow][nCol] - �������� ������. #define PNL_TOT
//			}
//		}
//	}
//	else if (m_nLayer == RMAP_ITS)
//	{
//		for (i = 0; i < nTotDefPcs; i++)
//		{
//			if (pDoc->m_pPcrIts[nIdx]->m_pMk[i] != -2) // -2 (NoMarking)
//			{
//				if (pDoc->WorkingInfo.System.bStripPcsRgnBin)	// DTS��
//				{
//					switch (pDoc->m_Master[0].MasterInfo.nActionCode)	// 0 : Rotation / Mirror ���� ����(CAM Data ����), 1 : �¿� �̷�, 2 : ���� �̷�, 3 : 180 ȸ��, 4 : 270 ȸ��(CCW), 5 : 90 ȸ��(CW)
//					{
//					case 0:
//						nPcsId = pDoc->m_pPcrIts[nIdx]->m_pDefPcs[i];
//						break;
//					case 1:
//						nPcsId = pDoc->MirrorLR(pDoc->m_pPcrIts[nIdx]->m_pDefPcs[i]);
//						break;
//					case 2:
//						nPcsId = pDoc->MirrorUD(pDoc->m_pPcrIts[nIdx]->m_pDefPcs[i]);
//						break;
//					case 3:
//						nPcsId = pDoc->Rotate180(pDoc->m_pPcrIts[nIdx]->m_pDefPcs[i]);
//						break;
//					default:
//						nPcsId = pDoc->m_pPcrIts[nIdx]->m_pDefPcs[i];
//						break;
//					}
//				}
//				else
//					nPcsId = pDoc->m_pPcrIts[nIdx]->m_pDefPcs[i];
//
//				nDefCode = pDoc->m_pPcrIts[nIdx]->m_pDefType[i];
//
//				nC = int(nPcsId / nNodeY);
//				if (nC % 2)	// Ȧ��.
//					nR = nNodeY * (nC + 1) - nPcsId - 1;
//				else		// ¦��.
//					nR = nPcsId - nNodeY * nC;
//				pPnlBuf[nR][nC] = (short)nDefCode;	// nPnl�� �� ����.
//				if (m_pPnlBuf)
//					m_pPnlBuf[nSerial - 1][nR][nC] = pPnlBuf[nR][nC];   // DefCode 3D Array : [nSerial][nRow][nCol] - �������� ������. #define PNL_TOT
//			}
//		}
//	}
//	else
//		return 0;
//
//
//	CString sPnl, sRow;
//	i = 0;
//	sPnl.Format(_T("%d"), nSerial);
//	strData.Format(_T("%d"), nTotDefPcs - nTotVerifyed);
//	::WritePrivateProfileString(sPnl, _T("Total Defects"), strData, sPath);
//
//	for(int nRow=0; nRow<nNodeX; nRow++)
//	{
//		sRow.Format(_T("%02d"), nRow);
//// 		sRow.Format(_T("%2d"), nRow+1);
//		strData.Format(_T(""));
//		strTemp.Format(_T(""));
//
//// 		if(nRow==26)
//// 			int brk=0;
//
//		for(int nCol=0; nCol<nNodeY; nCol++)
//		{
//			nR = (nNodeY-1)-nCol;
//			nC = nRow;
//
//			if (m_nLayer == RMAP_UP || m_nLayer == RMAP_DN || m_nLayer == RMAP_ALLUP || m_nLayer == RMAP_ALLDN)
//			{
//				if (pDoc->m_pPcr[nLayer][nIdx]->m_nErrPnl == -1 || pDoc->m_pPcr[nLayer][nIdx]->m_nErrPnl == -2)
//				{
//					nDefCode = DEF_LIGHT;
//					m_pPnlBuf[nSerial - 1][nR][nC] = (short)nDefCode;
//				}
//				else
//					nDefCode = (int)pPnlBuf[nR][nC] < 0 ? 0 : (int)pPnlBuf[nR][nC];
//			}
//			else if (m_nLayer == RMAP_INNER_UP || m_nLayer == RMAP_INNER_DN || m_nLayer == RMAP_INNER_ALLUP || m_nLayer == RMAP_INNER_ALLDN)
//			{
//				if (pDoc->m_pPcrInner[nLayer][nIdx]->m_nErrPnl == -1 || pDoc->m_pPcrInner[nLayer][nIdx]->m_nErrPnl == -2)
//				{
//					nDefCode = DEF_LIGHT;
//					m_pPnlBuf[nSerial - 1][nR][nC] = (short)nDefCode;
//				}
//				else
//					nDefCode = (int)pPnlBuf[nR][nC] < 0 ? 0 : (int)pPnlBuf[nR][nC];
//			}
//			else if (m_nLayer == RMAP_ITS)
//			{
//				if (pDoc->m_pPcrIts[nIdx]->m_nErrPnl == -1 || pDoc->m_pPcrIts[nIdx]->m_nErrPnl == -2)
//				{
//					nDefCode = DEF_LIGHT;
//					m_pPnlBuf[nSerial - 1][nR][nC] = (short)nDefCode;
//				}
//				else
//					nDefCode = (int)pPnlBuf[nR][nC] < 0 ? 0 : (int)pPnlBuf[nR][nC];
//			}
//			else
//				return 0;
//
//			strTemp.Format(_T("%2d,"), nDefCode);
//			
//			if(!nCol)
//				strData.Insert(0, strTemp);
//			else
//			{
//				int nLen = strData.GetLength();
//				if( !(nCol%nStripY) ) // Separate Strip
//				{
//					strData.Insert(nLen, _T("  "));
//					nLen = strData.GetLength();
//				}
//				strData.Insert(nLen, strTemp);
//			}
//		}
//
//		int nPos = strData.ReverseFind(',');
//		strData.Delete(nPos, 1);
//		::WritePrivateProfileString(sPnl, sRow, strData, sPath);
//	}
//
//	for(i=0; i<nNodeY; i++)
//		delete[]  pPnlBuf[i];
//	delete[] pPnlBuf;
//
//	m_nWritedSerial = nSerial;
//	
//	//if (pDoc->GetTestMode() == MODE_OUTER)
//	//	pDoc->WriteReelmapIts(nSerial);
//
//	return TRUE;
//}


void CReelMap::InitPcs()
{
	int k, i;
	if(nTotPnl>0 && nTotPcs>0)
	{
		if(!pPcsRgn)
		{
			pPcsRgn = new CRect*[nTotPnl];
			for(k=0; k<nTotPnl; k++)
			{
				pPcsRgn[k] = new CRect[nTotPcs];
			}
		}

		if(!pPcsDef)
		{
			pPcsDef = new int*[nTotPnl];
			for(k=0; k<nTotPnl; k++)
			{
				if (nTotPcs > MAX_PCS)
				{
					pView->ClrDispMsg();
					AfxMessageBox(_T("MAX_PCS �ʰ� Error."));
				}
				pPcsDef[k] = new int[MAX_PCS];
				for(i=0; i<MAX_PCS; i++)
					pPcsDef[k][i] = DEF_NONE;
			}
		}

// 		if(!pMkInfo)
// 			pMkInfo = new CString[nTotPcs];
	}
	else
	{
		pView->ClrDispMsg();
		AfxMessageBox(_T("Error-InitPcs."));
	}
}

// void CReelMap::ResetReelmap()
// {
// 	int k;
// // 	if(pMkInfo)
// // 	{
// // 		delete[] pMkInfo;
// // 		pMkInfo = NULL;
// // 	}
// 
// 	if(pPcsRgn)
// 	{
// 		for(k=0; k<nTotPnl; k++)
// 		{
// 			delete[] pPcsRgn[k];
// 			pPcsRgn[k] = NULL;
// 		}
// 		delete[] pPcsRgn;
// 		pPcsRgn = NULL;
// 	}
// 
// // 	CloseRst();
// 
// // 	if(pPcsDef)
// // 	{
// // 		for(k=0; k<nTotPnl; k++)
// // 		{
// // 			delete[] pPcsDef[k];
// // 			pPcsDef[k] = NULL;
// // 		}
// // 
// // 		delete[] pPcsDef;
// // 		pPcsDef = NULL;
// // 	}
// 
// }

void CReelMap::ClrPcs()
{
// 	int k;
// 	if(pMkInfo)
// 	{
// 		delete[] pMkInfo;
// 		pMkInfo = NULL;
// 	}
// 
// 	if(pPcsRgn)
// 	{
// 		for(k=0; k<nTotPnl; k++)
// 		{
// 			delete[] pPcsRgn[k];
// 			pPcsRgn[k] = NULL;
// 		}
// 		delete[] pPcsRgn;
// 		pPcsRgn = NULL;
// 	}
// 
// 	if(pPcsDef)
// 	{
// 		for(k=0; k<nTotPnl; k++)
// 		{
// 			delete[] pPcsDef[k];
// 			pPcsDef[k] = NULL;
// 		}
// 
// 		delete[] pPcsDef;
// 		pPcsDef = NULL;
// 	}


	for(int k=0; k<nTotPnl; k++)
	{
		for(int i=0; i<nTotPcs; i++)
		{
			pPcsDef[k][i] = DEF_NONE;
		}
	}
}

BOOL CReelMap::Disp(int nMkPnl, BOOL bDumy)
{
	if(!bDumy)
	{
		if(!pPcsDef || !m_pPnlNum || !m_pPnlDefNum)// || !pMkInfo)
		{
			pView->MsgBox(_T("Reelmap Memory Error."));
			return FALSE;
		}
	}

	if(!pDoc->m_Master[0].m_pPcsRgn)
	{
		pView->MsgBox(_T("PCS RGN Error."));
		return FALSE;
	}

	m_sPathBuf = GetRmapPath(m_nLayer);

	CDataFile *pDataFile = new CDataFile;
	CString sMsg;
	int nRepeat = 0;

	while(!pDataFile->Open(m_sPathBuf))
	{
		Sleep(500);
		if (!pDataFile->Open(m_sPathBuf))
		{
			Sleep(500);
			if (!pDataFile->Open(m_sPathBuf))
			{
				Sleep(500);
				if (!pDataFile->Open(m_sPathBuf))
				{
					if (nRepeat > 20)
					{
						sMsg.Format(_T("���������� ���� ���߽��ϴ�.\r\n%s\r\n���������� �ٽ� �����ðڽ��ϱ�?"), m_sPathBuf);
						if (IDNO == pView->MsgBox(sMsg, 0, MB_YESNO))
						{
							;
						}
					}
					else
					{
						nRepeat++;
						Sleep(500);
					}
				}
				else
					break;
			}
			else
				break;
		}
		else
			break;
	}
	
	m_nSerial = nMkPnl; // 8

	int nMkedPnl = nTotPnl-m_nSelMarkingPnl-1; // 8-2-1 = 5
	int nPrevPnl = m_nSelMarkingPnl; // 2

	int nFromPnl = nMkPnl-nMkedPnl-1; // 8-5-1 = 2

	CString sPnl, sRow, sVal;
	TCHAR sep[] = { _T(",/;\r\n\t") };
	TCHAR szData[MAX_PATH];
	int k, i, nR, nC, nP, nInc=0;
	//int nCol, nRow;
	int nLoadPnl, nDefCode;
	int nNodeX = pDoc->m_Master[0].m_pPcsRgn->nCol; // 10
	int nNodeY = pDoc->m_Master[0].m_pPcsRgn->nRow; // 5
	int nActionCode;

	for(k=nTotPnl-1; k>=0; k--) // k = 7 ~ 0
	{
		nInc++;
		nLoadPnl = nInc+nFromPnl; // 1 + 2 = 3 ~ 8 + 2 = 10

		if(pDoc->WorkingInfo.LastJob.bLotSep)
		{
			if(nLoadPnl > pDoc->m_nLotLastShot && !pDoc->m_bDoneChgLot)
			{
				for(int aa=k; aa>=0; aa--)
				{
					m_pPnlNum[aa] = nLoadPnl = -1;
					m_pPnlDefNum[aa] = -1;
				}
				break;
			}
		}

		if (pView->m_bSerialDecrese)
		{
			if (nLoadPnl < (pView->m_nLotEndSerial - pDoc->AoiDummyShot[0]) && pView->m_nLotEndSerial > 0)
			{
				for (int bb = k; bb >= 0; bb--)
				{
					m_pPnlNum[bb] = nLoadPnl = -1;
					m_pPnlDefNum[bb] = -1;
				}
				break;
			}
			else if (pView->ChkLastProc() && (nLoadPnl < pView->m_nLotEndSerial && pView->m_nLotEndSerial > 0))
			{
				for (int bb = k; bb >= 0; bb--)
				{
					m_pPnlNum[bb] = nLoadPnl = -1;
					m_pPnlDefNum[bb] = -1;
				}
				break;
			}
			else if (nLoadPnl < pView->m_nLotEndSerial && pView->m_nLotEndSerial > 0)
			{
				for (int cc = k; cc >= 0; cc--)
				{
					m_pPnlDefNum[cc] = -1;
				}
			}
		}
		else
		{
			if (nLoadPnl > (pView->m_nLotEndSerial + pDoc->AoiDummyShot[0]) && pView->m_nLotEndSerial > 0)
			{
				for (int bb = k; bb >= 0; bb--)
				{
					m_pPnlNum[bb] = nLoadPnl = -1;
					m_pPnlDefNum[bb] = -1;
				}
				break;
			}
			else if (pView->ChkLastProc() && (nLoadPnl > pView->m_nLotEndSerial && pView->m_nLotEndSerial > 0))
			{
				for (int bb = k; bb >= 0; bb--)
				{
					m_pPnlNum[bb] = nLoadPnl = -1;
					m_pPnlDefNum[bb] = -1;
				}
				break;
			}
			else if (nLoadPnl > pView->m_nLotEndSerial && pView->m_nLotEndSerial > 0)
			{
				for (int cc = k; cc >= 0; cc--)
				{
					m_pPnlDefNum[cc] = -1;
				}
			}
		}


		if(nLoadPnl <= 0)
		{
			m_pPnlNum[k] = -1;
			m_pPnlDefNum[k] = -1;
			for(i=0; i<nTotPcs; i++)
				pPcsDef[k][i] = DEF_NONE;
		}
		else
		{
			if (pView->m_bSerialDecrese)
			{
				if (pDoc->WorkingInfo.LastJob.bLotSep && nLoadPnl < pDoc->m_nLotLastShot && !pDoc->m_bDoneChgLot)
				{
					m_pPnlNum[k] = 0;
					m_pPnlDefNum[k] = -1;
					break;
				}
				else if (nLoadPnl < pDoc->m_ListBuf[0].GetLast())
				{
					m_pPnlNum[k] = 0;
					m_pPnlDefNum[k] = -1;
					break;
				}

				m_pPnlNum[k] = nLoadPnl; // 3 ~ 10
				if (nLoadPnl >= pView->m_nLotEndSerial || pView->m_nLotEndSerial == 0)
				{
					if (m_nLayer == RMAP_UP || m_nLayer == RMAP_DN || m_nLayer == RMAP_ALLUP || m_nLayer == RMAP_ALLDN)
					{
						//m_pPnlDefNum[k] = pDoc->m_pPcr[RMAP_ALLUP][pDoc->GetPcrIdx1(nLoadPnl)]->m_nTotDef;
						m_pPnlDefNum[k] = pDoc->m_pPcr[m_nLayer][pDoc->GetPcrIdx1(nLoadPnl)]->m_nTotDef;
					}
					else if (m_nLayer == RMAP_INNER_UP || m_nLayer == RMAP_INNER_DN || m_nLayer == RMAP_INNER_ALLUP || m_nLayer == RMAP_INNER_ALLDN)
					{
						//m_pPnlDefNum[k] = pDoc->m_pPcrInner[RMAP_ALLUP][pDoc->GetPcrIdx1(nLoadPnl)]->m_nTotDef;
						m_pPnlDefNum[k] = pDoc->m_pPcrInner[m_nLayer - RMAP_INNER_UP][pDoc->GetPcrIdx1(nLoadPnl)]->m_nTotDef;
					}
					else if (m_nLayer == RMAP_ITS)
						m_pPnlDefNum[k] = pDoc->m_pPcrIts[pDoc->GetPcrIdx1(nLoadPnl)]->m_nTotDef;
					else
						return 0;
				}
			}
			else
			{
				if (pDoc->WorkingInfo.LastJob.bLotSep && nLoadPnl > pDoc->m_nLotLastShot && !pDoc->m_bDoneChgLot)
				{
					m_pPnlNum[k] = 0;
					m_pPnlDefNum[k] = -1;
					break;
				}
				else if (nLoadPnl > pDoc->m_ListBuf[0].GetLast())
				{
					m_pPnlNum[k] = 0;
					m_pPnlDefNum[k] = -1;
					break;
				}

				m_pPnlNum[k] = nLoadPnl; // 3 ~ 10
				if (nLoadPnl <= pView->m_nLotEndSerial || pView->m_nLotEndSerial == 0)
				{
					if (m_nLayer == RMAP_UP || m_nLayer == RMAP_DN || m_nLayer == RMAP_ALLUP || m_nLayer == RMAP_ALLDN)
					{
						//m_pPnlDefNum[k] = pDoc->m_pPcr[RMAP_ALLUP][pDoc->GetPcrIdx1(nLoadPnl)]->m_nTotDef;
						m_pPnlDefNum[k] = pDoc->m_pPcr[m_nLayer][pDoc->GetPcrIdx1(nLoadPnl)]->m_nTotDef;
					}
					else if (m_nLayer == RMAP_INNER_UP || m_nLayer == RMAP_INNER_DN || m_nLayer == RMAP_INNER_ALLUP || m_nLayer == RMAP_INNER_ALLDN)
					{
						//m_pPnlDefNum[k] = pDoc->m_pPcrInner[RMAP_ALLUP][pDoc->GetPcrIdx1(nLoadPnl)]->m_nTotDef;
						if(pDoc->m_pPcrInner)
							m_pPnlDefNum[k] = pDoc->m_pPcrInner[m_nLayer - RMAP_INNER_UP][pDoc->GetPcrIdx1(nLoadPnl)]->m_nTotDef;
					}
					else if (m_nLayer == RMAP_ITS)
						m_pPnlDefNum[k] = pDoc->m_pPcrIts[pDoc->GetPcrIdx1(nLoadPnl)]->m_nTotDef;
					else
						return 0;
				}
			}

			for(nR=0; nR<nNodeX; nR++)  // nR = 0 ~ 5
			{
				sPnl.Format(_T("%d"), nLoadPnl);
				sRow.Format(_T("%02d"), nR);
				
				if (0 < ::GetPrivateProfileString(sPnl, sRow, NULL, szData, sizeof(szData), m_sPathBuf)) // DefCode 3D Array : [nSerial][nRow][nCol] - ��������
				{
					for(nC=0; nC<nNodeY; nC++)
					{
						if(nC==0)
							sVal = _tcstok(szData,sep);
						else
							sVal = _tcstok(NULL,sep);
						
						if(nR%2)	// Ȧ��.
							nP = nC + nNodeY * nR;
						else		// ¦��.				
							nP = nNodeY * (nR+1) - (nC+1);

						if (pDoc->WorkingInfo.System.bStripPcsRgnBin)	// DTS��
						{
							if (m_nLayer == RMAP_UP || m_nLayer == RMAP_DN || m_nLayer == RMAP_ALLUP || m_nLayer == RMAP_ALLDN)
							{
								nActionCode = pDoc->m_Master[0].MasterInfo.nActionCode;
							}
							else if (m_nLayer == RMAP_INNER_UP || m_nLayer == RMAP_INNER_DN || m_nLayer == RMAP_INNER_ALLUP || m_nLayer == RMAP_INNER_ALLDN)
							{
								nActionCode = pDoc->m_MasterInner[0].MasterInfo.nActionCode;
							}
							else if (m_nLayer == RMAP_ITS)
							{
								nActionCode = pDoc->m_Master[0].MasterInfo.nActionCode;
							}

							//switch (pDoc->m_Master[1].MasterInfo.nActionCode)	// 0 : Rotation / Mirror ���� ����(CAM Data ����), 1 : �¿� �̷�, 2 : ���� �̷�, 3 : 180 ȸ��, 4 : 270 ȸ��(CCW), 5 : 90 ȸ��(CW)
							switch (nActionCode)
							{
							case 0:
								break;
							case 1:
								nP = pDoc->MirrorLR(nP);
								break;
							case 2:
								nP = pDoc->MirrorUD(nP);
								break;
							case 3:
								nP = pDoc->Rotate180(nP);
								break;
							default:
								break;
							}
						}

						nDefCode = _tstoi(sVal);
						pPcsDef[k][nP] = nDefCode; // k=7, nP = PcsIdx : �»�ܿ��� nP 0�� �����Ͽ� ZigZeg�� ���ϴ����� ����

					}
				}
				else
				{
					Sleep(300);

					if (0 < ::GetPrivateProfileString(sPnl, sRow, NULL, szData, sizeof(szData), m_sPathBuf))
					{
						for (nC = 0; nC < nNodeY; nC++)
						{
							if (nC == 0)
								sVal = _tcstok(szData, sep);
							else
								sVal = _tcstok(NULL, sep);

							if (nR % 2)	// Ȧ��.
								nP = nC + nNodeY * nR;
							else		// ¦��.				
								nP = nNodeY * (nR + 1) - (nC + 1);

							if (pDoc->WorkingInfo.System.bStripPcsRgnBin)	// DTS��
							{								
								if (m_nLayer == RMAP_UP || m_nLayer == RMAP_DN || m_nLayer == RMAP_ALLUP || m_nLayer == RMAP_ALLDN)
								{
									nActionCode = pDoc->m_Master[0].MasterInfo.nActionCode;
								}
								else if (m_nLayer == RMAP_INNER_UP || m_nLayer == RMAP_INNER_DN || m_nLayer == RMAP_INNER_ALLUP || m_nLayer == RMAP_INNER_ALLDN)
								{
									nActionCode = pDoc->m_MasterInner[0].MasterInfo.nActionCode;
								}
								else if (m_nLayer == RMAP_ITS)
								{
									nActionCode = pDoc->m_Master[0].MasterInfo.nActionCode;
								}

								//switch (pDoc->m_Master[1].MasterInfo.nActionCode)	// 0 : Rotation / Mirror ���� ����(CAM Data ����), 1 : �¿� �̷�, 2 : ���� �̷�, 3 : 180 ȸ��, 4 : 270 ȸ��(CCW), 5 : 90 ȸ��(CW)
								switch (nActionCode)
								{
								case 0:
									break;
								case 1:
									nP = pDoc->MirrorLR(nP);
									break;
								case 2:
									nP = pDoc->MirrorUD(nP);
									break;
								case 3:
									nP = pDoc->Rotate180(nP);
									break;
								default:
									break;
								}
							}

							nDefCode = _tstoi(sVal);
							pPcsDef[k][nP] = nDefCode; // k=7, nP = PcsIdx : �»�ܿ��� nP 0�� �����Ͽ� ZigZeg�� ���ϴ����� ����
						}
					}
					else
					{
						if (!bDumy)
						{
							if (!pView->m_bLastProc && !pDoc->WorkingInfo.LastJob.bSampleTest)
							{
								{
									m_pPnlNum[k] = -1;
									m_pPnlDefNum[k] = -1;
									for (i = 0; i < nTotPcs; i++)
										pPcsDef[k][i] = DEF_NONE;
								}

								//pView->Stop();
								//sMsg.Format(_T("���� ����Ÿ�� �ҷ� ���� ���߽��ϴ�. - [Pnl: %s] [Row: %d]\r\n%s"), sPnl, nR, m_sPathBuf);
								////sMsg.Format(_T("ReelmapData missed. - [Pnl: %d] [Row: %d]"), k, nR);
								//pView->MsgBox(sMsg); // 20160725-Temp
								//if (pDataFile)
								//{
								//	delete pDataFile;
								//	pDataFile = NULL;
								//}
								////AfxMessageBox(sMsg);
								//return FALSE;
							}
							else
							{
								m_pPnlNum[k] = 0;
								m_pPnlDefNum[k] = -1;

								for (nC = 0; nC < nNodeY; nC++)
								{
									if (nR % 2)	// Ȧ��.
										nP = nC + nNodeY * nR;
									else		// ¦��.				
										nP = nNodeY * (nR + 1) - (nC + 1);

									if (pDoc->WorkingInfo.System.bStripPcsRgnBin)	// DTS��
									{
										if (m_nLayer == RMAP_UP || m_nLayer == RMAP_DN || m_nLayer == RMAP_ALLUP || m_nLayer == RMAP_ALLDN)
										{
											nActionCode = pDoc->m_Master[0].MasterInfo.nActionCode;
										}
										else if (m_nLayer == RMAP_INNER_UP || m_nLayer == RMAP_INNER_DN || m_nLayer == RMAP_INNER_ALLUP || m_nLayer == RMAP_INNER_ALLDN)
										{
											nActionCode = pDoc->m_MasterInner[0].MasterInfo.nActionCode;
										}
										else if (m_nLayer == RMAP_ITS)
										{
											nActionCode = pDoc->m_Master[0].MasterInfo.nActionCode;
										}

										//switch (pDoc->m_Master[1].MasterInfo.nActionCode)	// 0 : Rotation / Mirror ���� ����(CAM Data ����), 1 : �¿� �̷�, 2 : ���� �̷�, 3 : 180 ȸ��, 4 : 270 ȸ��(CCW), 5 : 90 ȸ��(CW)
										switch (nActionCode)
										{
										case 0:
											break;
										case 1:
											nP = pDoc->MirrorLR(nP);
											break;
										case 2:
											nP = pDoc->MirrorUD(nP);
											break;
										case 3:
											nP = pDoc->Rotate180(nP);
											break;
										default:
											break;
										}
									}

									nDefCode = DEF_NONE;
									pPcsDef[k][nP] = nDefCode; // k=7, nP = PcsIdx : �»�ܿ��� nP 0�� �����Ͽ� ZigZeg�� ���ϴ����� ����
								}

							}
						}
						else
						{
							for (nC = 0; nC < nNodeY; nC++)
							{
								if (nC == 0)
									sVal = _tcstok(szData, sep);
								else
									sVal = _tcstok(NULL, sep);

								if (nR % 2)	// Ȧ��.
									nP = nC + nNodeY * nR;
								else		// ¦��.
									nP = nNodeY * (nR + 1) - (nC + 1);

								if (pDoc->WorkingInfo.System.bStripPcsRgnBin)	// DTS��
								{
									if (m_nLayer == RMAP_UP || m_nLayer == RMAP_DN || m_nLayer == RMAP_ALLUP || m_nLayer == RMAP_ALLDN)
									{
										nActionCode = pDoc->m_Master[0].MasterInfo.nActionCode;
									}
									else if (m_nLayer == RMAP_INNER_UP || m_nLayer == RMAP_INNER_DN || m_nLayer == RMAP_INNER_ALLUP || m_nLayer == RMAP_INNER_ALLDN)
									{
										nActionCode = pDoc->m_MasterInner[0].MasterInfo.nActionCode;

										// ���� ���� Display �¿���� ��Ŵ....
										if(nActionCode == 0)
											nActionCode = 1;
										else if(nActionCode == 1)
											nActionCode = 0;
										else if(nActionCode == 2)
											nActionCode = 3;
										else if (nActionCode == 3)
											nActionCode = 2;
									}
									else if (m_nLayer == RMAP_ITS)
									{
										nActionCode = pDoc->m_Master[0].MasterInfo.nActionCode;
									}

									//switch (pDoc->m_Master[1].MasterInfo.nActionCode)	// 0 : Rotation / Mirror ���� ����(CAM Data ����), 1 : �¿� �̷�, 2 : ���� �̷�, 3 : 180 ȸ��, 4 : 270 ȸ��(CCW), 5 : 90 ȸ��(CW)
									switch (nActionCode)
									{
									case 0:
										break;
									case 1:
										nP = pDoc->MirrorLR(nP);
										break;
									case 2:
										nP = pDoc->MirrorUD(nP);
										break;
									case 3:
										nP = pDoc->Rotate180(nP);
										break;
									default:
										break;
									}
								}

								nDefCode = 0;
								pPcsDef[k][nP] = nDefCode; // k=7, nP = PcsIdx : �»�ܿ��� nP 0�� �����Ͽ� ZigZeg�� ���ϴ����� ����

	 							//if(pDoc->WorkingInfo.LastJob.bContFixDef)
	 							//	SetFixPcs(nC, nR);
							}
						}
					}
				}
			}
		}
	}

	if(pDataFile)
	{
		delete pDataFile;
		pDataFile = NULL;
	}

	return TRUE;
}

void CReelMap::SetLotSt()
{
	CString sData;
	int nYear, nMonth, nDay, nHour, nMin, nSec;
	nYear = pDoc->WorkingInfo.Lot.StTime.nYear; 
	nMonth = pDoc->WorkingInfo.Lot.StTime.nMonth; 
	nDay = pDoc->WorkingInfo.Lot.StTime.nDay; 
	nHour = pDoc->WorkingInfo.Lot.StTime.nHour; 
	nMin = pDoc->WorkingInfo.Lot.StTime.nMin; 
	nSec = pDoc->WorkingInfo.Lot.StTime.nSec;

	if(!nYear && !nMonth && !nDay && !nHour && !nMin && !nSec)
		sData = _T("");
	else
		sData.Format(_T("%04d-%02d-%02d, %02d:%02d:%02d"), nYear, nMonth, nDay, nHour, nMin, nSec);
	::WritePrivateProfileString(_T("Info"), _T("Lot Start"), sData, m_sPathBuf);
}

CString CReelMap::GetLotSt()
{
	CString sLot = _T("");
	//char szData[MAX_PATH];
	TCHAR szData[MAX_PATH];
	if (0 < ::GetPrivateProfileString(_T("Info"), _T("Lot Start"), NULL, szData, sizeof(szData), m_sPathBuf))
		sLot = CString(szData);
	return sLot;
}

void CReelMap::SetLotEd()
{
	CString sData;
	int nYear, nMonth, nDay, nHour, nMin, nSec;
	nYear = pDoc->WorkingInfo.Lot.EdTime.nYear; 
	nMonth = pDoc->WorkingInfo.Lot.EdTime.nMonth; 
	nDay = pDoc->WorkingInfo.Lot.EdTime.nDay; 
	nHour = pDoc->WorkingInfo.Lot.EdTime.nHour; 
	nMin = pDoc->WorkingInfo.Lot.EdTime.nMin; 
	nSec = pDoc->WorkingInfo.Lot.EdTime.nSec;

	if(!nYear && !nMonth && !nDay && !nHour && !nMin && !nSec)
		sData = _T("");
	else
		sData.Format(_T("%04d-%02d-%02d, %02d:%02d:%02d"), nYear, nMonth, nDay, nHour, nMin, nSec);
	::WritePrivateProfileString(_T("Info"), _T("Lot End"), sData, m_sPathBuf);

	if((pView->m_dwLotEd-pView->m_dwLotSt) > 0)
	{
		int nDiff = (pView->m_dwLotEd-pView->m_dwLotSt)/1000;
		nHour = int(nDiff/3600);
		nMin = int((nDiff-3600*nHour)/60);
		nSec = nDiff % 60;
		sData.Format(_T("%02d:%02d:%02d"), nHour, nMin, nSec);
		::WritePrivateProfileString(_T("Info"), _T("Lot Run"), sData, m_sPathBuf);
	}
}

CString CReelMap::GetLotEd()
{
	CString sLot = _T("");
	//char szData[MAX_PATH];
	TCHAR szData[MAX_PATH];
	if (0 < ::GetPrivateProfileString(_T("Info"), _T("Lot End"), NULL, szData, sizeof(szData), m_sPathBuf))
		sLot = CString(szData);
	return sLot;
}

void CReelMap::SetLastSerial(int nSerial) 	// ���� �ؽ�Ʈ ������ ���������� ������Ʈ��.
{
	if(nSerial <= 0)
	{
		pView->ClrDispMsg();
		AfxMessageBox(_T("Serial Error.64"));
		return;
	}

	m_nLastShot = nSerial;
	m_sPathBuf = GetRmapPath(m_nLayer);

	CString sData;
	sData.Format(_T("%d"), nSerial);
	::WritePrivateProfileString(_T("Info"), _T("Marked Shot"), sData, m_sPathBuf);

	int nYear, nMonth, nDay, nHour, nMin, nSec;
	nYear = pDoc->WorkingInfo.Lot.CurTime.nYear; 
	nMonth = pDoc->WorkingInfo.Lot.CurTime.nMonth; 
	nDay = pDoc->WorkingInfo.Lot.CurTime.nDay; 
	nHour = pDoc->WorkingInfo.Lot.CurTime.nHour; 
	nMin = pDoc->WorkingInfo.Lot.CurTime.nMin; 
	nSec = pDoc->WorkingInfo.Lot.CurTime.nSec;

	if(!nYear && !nMonth && !nDay && !nHour && !nMin && !nSec)
		sData = _T("");
	else
		sData.Format(_T("%04d-%02d-%02d, %02d:%02d:%02d"), nYear, nMonth, nDay, nHour, nMin, nSec);
	::WritePrivateProfileString(_T("Info"), _T("Marked Date"), sData, m_sPathBuf);

	//UpdateRst();
}

void CReelMap::SetCompletedSerial(int nSerial)
{
	if (nSerial <= 0)
	{
		pView->ClrDispMsg();
		AfxMessageBox(_T("Serial Error.67"));
		return;
	}

	m_nCompletedShot = nSerial;

	CString sData;
	sData.Format(_T("%d"), nSerial);
	::WritePrivateProfileString(_T("Info"), _T("Completed Shot"), sData, m_sPathBuf);

	int nYear, nMonth, nDay, nHour, nMin, nSec;
	nYear = pDoc->WorkingInfo.Lot.CurTime.nYear;
	nMonth = pDoc->WorkingInfo.Lot.CurTime.nMonth;
	nDay = pDoc->WorkingInfo.Lot.CurTime.nDay;
	nHour = pDoc->WorkingInfo.Lot.CurTime.nHour;
	nMin = pDoc->WorkingInfo.Lot.CurTime.nMin;
	nSec = pDoc->WorkingInfo.Lot.CurTime.nSec;

	if (!nYear && !nMonth && !nDay && !nHour && !nMin && !nSec)
		sData = _T("");
	else
		sData.Format(_T("%04d-%02d-%02d, %02d:%02d:%02d"), nYear, nMonth, nDay, nHour, nMin, nSec);
	::WritePrivateProfileString(_T("Info"), _T("Completed Date"), sData, m_sPathBuf);

	//UpdateRst(nSerial);
}

BOOL CReelMap::GetRst(int nFrom, int nTo)
{
	return TRUE;
}

BOOL CReelMap::GetRst(int nSerial)
{
	return TRUE;
}


BOOL CReelMap::InitRst()
{
	int nPnl, nRow;
	int TotPnl = PNL_TOT;
	int nDumyPnl = 20;
// 	int nNodeX = pDoc->m_pPcsRgn->nCol;
// 	int nNodeY = pDoc->m_pPcsRgn->nRow;
	int nNodeX = PNLBUF_X;
	int nNodeY = PNLBUF_Y;

	int k, i;

 	CloseRst();

	m_nPnlBuf = TotPnl+nDumyPnl;
	if(!m_pPnlBuf)
	{
		m_pPnlBuf = new short**[m_nPnlBuf];
		for(nPnl=0; nPnl<m_nPnlBuf; nPnl++)
		{
			m_pPnlBuf[nPnl] = new short*[nNodeY];
			for(nRow=0; nRow<nNodeY; nRow++)
				m_pPnlBuf[nPnl][nRow] = new short[nNodeX];
		}
	}

	m_nTotPcs = 0;
	m_nGoodPcs = 0;
	m_nBadPcs = 0;
	for(i=0; i<MAX_DEF; i++)
		m_nDef[i] = 0;

	m_nTotStOut = 0;
	for(k=0; k<MAX_STRIP_NUM; k++)
	{
		m_nStripOut[k] = 0;
		m_nDefStrip[k] = 0;
		for(i=0; i<MAX_DEF; i++)
			m_nDefPerStrip[k][i] = 0;
	}

	return TRUE;
}

void CReelMap::CloseRst()
{
	int nPnl, nRow;
// 	int nNodeX = pDoc->m_pPcsRgn->nCol;
// 	int nNodeY = pDoc->m_pPcsRgn->nRow;
	int nNodeX = PNLBUF_X;
	int nNodeY = PNLBUF_Y;

	if(m_pPnlBuf)
	{
		for(nPnl=0; nPnl<m_nPnlBuf; nPnl++)
		{
			for(nRow=0; nRow<nNodeY; nRow++)
				delete[] m_pPnlBuf[nPnl][nRow];
			delete[] m_pPnlBuf[nPnl];
		}

		delete[] m_pPnlBuf;
		m_pPnlBuf = NULL;
		m_nPnlBuf = 0;
	}
}

void CReelMap::ClrRst()
{
	m_nTotPcs = 0;
	m_nGoodPcs = 0;
	m_nBadPcs = 0;

	int k, i;
	for(i=1; i<MAX_DEF; i++)
		m_nDef[i] = 0;

	m_nTotStOut = 0;
	for(k=0; k<MAX_STRIP_NUM; k++)
	{
		m_nStripOut[k] = 0;
		m_nDefStrip[k] = 0;
		for(i=1; i<MAX_DEF; i++)
			m_nDefPerStrip[k][i] = 0;
	}

// 	double dN0;
// 	if(m_dTotLen > m_dPnlLen)
// 		dN0 = m_dTotLen / m_dPnlLen;
// 	else
// 		dN0 = (double)PNL_TOT;
// 	int nN0 = int(m_dTotLen / m_dPnlLen);
// 	int TotPnl = (dN0 - (double)nN0) > 0.0 ? (nN0+1) : nN0 ;
// 	if(TotPnl > PNL_TOT)
// 		TotPnl = PNL_TOT;

	int nPnl, nRow, nCol;
// 	int nNodeX = pDoc->m_Master[0].m_pPcsRgn->nCol;
// 	int nNodeY = pDoc->m_Master[0].m_pPcsRgn->nRow;
	int TotPnl = PNL_TOT;
	int nNodeX = PNLBUF_X;
	int nNodeY = PNLBUF_Y;

	if(m_pPnlBuf)
	{
		for(nPnl=0; nPnl<TotPnl; nPnl++)
		{
			for(nRow=0; nRow<nNodeY; nRow++)
			{
				for(nCol=0; nCol<nNodeX; nCol++)
					m_pPnlBuf[nPnl][nRow][nCol] = 0;
			}
		}
	}
}

CString CReelMap::GetYieldPath(int nRmap)
{
	CString sPath;
	//BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;

	CString str;

	//if (bDualTest)
	{
		switch (nRmap)
		{
		case RMAP_UP:
			str = _T("YieldUp.txt");
			sPath.Format(_T("%s%s\\%s\\%s\\%s"), pDoc->WorkingInfo.System.sPathOldFile,
				pDoc->WorkingInfo.LastJob.sModelUp,
				pDoc->WorkingInfo.LastJob.sLotUp,
				pDoc->WorkingInfo.LastJob.sLayerUp,
				str);
			//if (pDoc->m_bDoneChgLot || !pDoc->m_bNewLotShare[0])
			//{
			//	sPath.Format(_T("%s%s\\%s\\%s\\%s"), pDoc->WorkingInfo.System.sPathOldFile,
			//		pDoc->WorkingInfo.LastJob.sModelUp,
			//		pDoc->WorkingInfo.LastJob.sLotUp,
			//		pDoc->WorkingInfo.LastJob.sLayerUp,
			//		str);
			//}
			//else if (!pDoc->m_bDoneChgLot && pDoc->m_bNewLotShare[0])
			//{
			//	sPath.Format(_T("%s%s\\%s\\%s\\%s"), pDoc->WorkingInfo.System.sPathOldFile,
			//		pDoc->WorkingInfo.LastJob.sModelUp,
			//		pDoc->Status.PcrShare[0].sLot,
			//		pDoc->WorkingInfo.LastJob.sLayerUp,
			//		str);
			//}
			break;
		case RMAP_ALLUP:
			str = _T("YieldAll.txt");
			sPath.Format(_T("%s%s\\%s\\%s\\%s"), pDoc->WorkingInfo.System.sPathOldFile,
				pDoc->WorkingInfo.LastJob.sModelUp,
				pDoc->WorkingInfo.LastJob.sLotUp,
				pDoc->WorkingInfo.LastJob.sLayerUp,
				str);
			//if (pDoc->m_bDoneChgLot || !pDoc->m_bNewLotShare[0])
			//{
			//	sPath.Format(_T("%s%s\\%s\\%s\\%s"), pDoc->WorkingInfo.System.sPathOldFile,
			//		pDoc->WorkingInfo.LastJob.sModelUp,
			//		pDoc->WorkingInfo.LastJob.sLotUp,
			//		pDoc->WorkingInfo.LastJob.sLayerUp,
			//		str);
			//}
			//else if (!pDoc->m_bDoneChgLot && pDoc->m_bNewLotShare[0])
			//{
			//	sPath.Format(_T("%s%s\\%s\\%s\\%s"), pDoc->WorkingInfo.System.sPathOldFile,
			//		pDoc->WorkingInfo.LastJob.sModelUp,
			//		pDoc->Status.PcrShare[0].sLot,
			//		pDoc->WorkingInfo.LastJob.sLayerUp,
			//		str);
			//}
			break;
		case RMAP_DN:
			str = _T("YieldDn.txt");
			sPath.Format(_T("%s%s\\%s\\%s\\%s"), pDoc->WorkingInfo.System.sPathOldFile,
				pDoc->WorkingInfo.LastJob.sModelUp,
				pDoc->WorkingInfo.LastJob.sLotUp,
				pDoc->WorkingInfo.LastJob.sLayerDn,
				str);
			//if (pDoc->m_bDoneChgLot || !pDoc->m_bNewLotShare[1])
			//{
			//	sPath.Format(_T("%s%s\\%s\\%s\\%s"), pDoc->WorkingInfo.System.sPathOldFile,
			//		//pDoc->WorkingInfo.LastJob.sModelDn,
			//		//pDoc->WorkingInfo.LastJob.sLotDn,
			//		pDoc->WorkingInfo.LastJob.sModelUp,
			//		pDoc->WorkingInfo.LastJob.sLotUp,
			//		pDoc->WorkingInfo.LastJob.sLayerDn,
			//		str);
			//}
			//else if (!pDoc->m_bDoneChgLot && pDoc->m_bNewLotShare[1])
			//{
			//	sPath.Format(_T("%s%s\\%s\\%s\\%s"), pDoc->WorkingInfo.System.sPathOldFile,
			//		//pDoc->WorkingInfo.LastJob.sModelDn,
			//		//pDoc->Status.PcrShare[1].sLot,
			//		pDoc->WorkingInfo.LastJob.sModelUp,
			//		pDoc->Status.PcrShare[0].sLot,
			//		pDoc->WorkingInfo.LastJob.sLayerDn,
			//		str);
			//}
			break;
		case RMAP_ALLDN:
			str = _T("YieldAll.txt");
			sPath.Format(_T("%s%s\\%s\\%s\\%s"), pDoc->WorkingInfo.System.sPathOldFile,
				pDoc->WorkingInfo.LastJob.sModelUp,
				pDoc->WorkingInfo.LastJob.sLotUp,
				pDoc->WorkingInfo.LastJob.sLayerDn,
				str);
		//if (pDoc->m_bDoneChgLot || !pDoc->m_bNewLotShare[1])
			//{
			//	sPath.Format(_T("%s%s\\%s\\%s\\%s"), pDoc->WorkingInfo.System.sPathOldFile,
			//		//pDoc->WorkingInfo.LastJob.sModelDn,
			//		//pDoc->WorkingInfo.LastJob.sLotDn,
			//		pDoc->WorkingInfo.LastJob.sModelUp,
			//		pDoc->WorkingInfo.LastJob.sLotUp,
			//		pDoc->WorkingInfo.LastJob.sLayerDn,
			//		str);
			//}
			//else if (!pDoc->m_bDoneChgLot && pDoc->m_bNewLotShare[1])
			//{
			//	sPath.Format(_T("%s%s\\%s\\%s\\%s"), pDoc->WorkingInfo.System.sPathOldFile,
			//		//pDoc->WorkingInfo.LastJob.sModelDn,
			//		//pDoc->Status.PcrShare[1].sLot,
			//		pDoc->WorkingInfo.LastJob.sModelUp,
			//		pDoc->Status.PcrShare[0].sLot,
			//		pDoc->WorkingInfo.LastJob.sLayerDn,
			//		str);
			//}
			break;
		case RMAP_INNER_UP:
			str = _T("YieldUp.txt");
				sPath.Format(_T("%s%s\\%s\\%s\\%s\\%s"), pDoc->WorkingInfo.System.sPathItsFile,
					pDoc->WorkingInfo.LastJob.sModelUp, pDoc->WorkingInfo.LastJob.sEngItsCode,
					pDoc->WorkingInfo.LastJob.sInnerLotUp, pDoc->WorkingInfo.LastJob.sInnerLayerUp,
					str);
			break;
		case RMAP_INNER_DN:
			str = _T("YieldDn.txt");
			sPath.Format(_T("%s%s\\%s\\%s\\%s\\%s"), pDoc->WorkingInfo.System.sPathItsFile,
				pDoc->WorkingInfo.LastJob.sModelUp, pDoc->WorkingInfo.LastJob.sEngItsCode,
				pDoc->WorkingInfo.LastJob.sInnerLotUp, pDoc->WorkingInfo.LastJob.sInnerLayerDn,
				str);
			break;
		case RMAP_INNER_ALLUP:
			str = _T("YieldAll.txt");
			sPath.Format(_T("%s%s\\%s\\%s\\%s\\%s"), pDoc->WorkingInfo.System.sPathItsFile,
				pDoc->WorkingInfo.LastJob.sModelUp, pDoc->WorkingInfo.LastJob.sEngItsCode,
				pDoc->WorkingInfo.LastJob.sInnerLotUp, pDoc->WorkingInfo.LastJob.sInnerLayerUp,
				str);
			break;
		case RMAP_INNER_ALLDN:
			str = _T("YieldAll.txt");
			sPath.Format(_T("%s%s\\%s\\%s\\%s\\%s"), pDoc->WorkingInfo.System.sPathItsFile,
				pDoc->WorkingInfo.LastJob.sModelUp, pDoc->WorkingInfo.LastJob.sEngItsCode,
				pDoc->WorkingInfo.LastJob.sInnerLotUp, pDoc->WorkingInfo.LastJob.sInnerLayerDn,
				str);
			break;
		case RMAP_INOUTER_UP:
			break;
		case RMAP_INOUTER_DN:
			break;
		case RMAP_ITS:
			//pDoc->GetCurrentInfoEng();
			str = _T("YieldIts.txt");
			sPath.Format(_T("%s%s\\%s\\%s"), pDoc->WorkingInfo.System.sPathItsFile,
				pDoc->WorkingInfo.LastJob.sModelUp, pDoc->WorkingInfo.LastJob.sEngItsCode, //pDoc->m_sItsCode,
				str);
			break;
		}
	}
	//else
	//{
	//	str = _T("YieldUp.txt");
	//	if (pDoc->m_bDoneChgLot || !pDoc->m_bNewLotShare[0])
	//	{
	//		sPath.Format(_T("%s%s\\%s\\%s\\%s"), pDoc->WorkingInfo.System.sPathOldFile,
	//			pDoc->WorkingInfo.LastJob.sModelUp,
	//			pDoc->WorkingInfo.LastJob.sLotUp,
	//			pDoc->WorkingInfo.LastJob.sLayerUp,
	//			str);
	//	}
	//	else if (!pDoc->m_bDoneChgLot && pDoc->m_bNewLotShare[0])
	//	{
	//		sPath.Format(_T("%s%s\\%s\\%s\\%s"), pDoc->WorkingInfo.System.sPathOldFile,
	//			pDoc->WorkingInfo.LastJob.sModelUp,
	//			pDoc->Status.PcrShare[0].sLot,
	//			pDoc->WorkingInfo.LastJob.sLayerUp,
	//			str);
	//	}
	//}
	return sPath;
}

//CString CReelMap::GetYieldPath(int nRmap)
//{
//	CString sPath;
//#ifdef TEST_MODE
//	sPath = PATH_REELMAP;
//#else
//	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;
//
//	CString str;
//
//	if (bDualTest)
//	{
//		switch (nRmap)
//		{
//		case RMAP_UP:
//			str = _T("YieldUp.txt");
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
//#ifdef TEST_MODE
//			str = _T("YieldAllUp.txt");
//#else
//			str = _T("YieldAll.txt");
//#endif
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
//			str = _T("YieldDn.txt");
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
//#ifdef TEST_MODE
//			str = _T("YieldAllDn.txt");
//#else
//			str = _T("YieldAll.txt");
//#endif
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
//		}
//	}
//	else
//	{
//		str = _T("YieldUp.txt");
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
//	}
//#endif
//	return sPath;
//}

void CReelMap::ResetYield()
{
	m_nBeforeSerial = 0;

	m_stYield.nTot = 0; m_stYield.nGood = 0; m_stYield.nDef = 0;
	m_stYield.nTotSriptOut = 0;
	m_stYield.nDefStrip[0] = 0; m_stYield.nDefStrip[1] = 0; m_stYield.nDefStrip[2] = 0; m_stYield.nDefStrip[3] = 0;
	m_stYield.nStripOut[0] = 0; m_stYield.nStripOut[1] = 0; m_stYield.nStripOut[2] = 0; m_stYield.nStripOut[3] = 0;

	for (int k = 0; k < MAX_DEF; k++)
	{
		m_stYield.nDefA[k] = 0;

		for (int i = 0; i < MAX_STRIP_NUM; i++)
		{
			m_stYield.nDefPerStrip[i][k] = 0;
		}
	}
}

//BOOL CReelMap::WriteYield(int nSerial, CString sPath)
//{
//	int nNodeX = 0, nNodeY = 0;
//#ifndef TEST_MODE
//	nNodeX = pDoc->m_Master[0].m_pPcsRgn->nCol;
//	nNodeY = pDoc->m_Master[0].m_pPcsRgn->nRow;
//#endif
//	CString sDefNum, strData;
//	int nPnl, nRow, nCol, nDefCode, nStrip;
//	int nTotPcs = nNodeX * nNodeY;
//	int nStripPcs = nTotPcs / 4;
//	double dStOutRto = _tstof(pDoc->WorkingInfo.LastJob.sStripOutRatio) / 100.0;
//	nPnl = nSerial - 1;
//
//	int i, k;
//	TCHAR szData[200];
//
//	CString sCode, strMenu, strItem, sMsg;
//	int nTot=0, nGood=0, nDef=0;
//	int nTotSriptOut = 0;
//
//	int nDefStrip[4], nDefA[MAX_DEF], nDefPerStrip[4][MAX_DEF], nStripOut[4];
//	nDefStrip[0] = 0; nDefStrip[1] = 0; nDefStrip[2] = 0; nDefStrip[3] = 0;
//	nStripOut[0] = 0; nStripOut[1] = 0; nStripOut[2] = 0; nStripOut[3] = 0;
//
//	nTot = nNodeX * nNodeY;
//
//	strMenu.Format(_T("%d"), nSerial);
//
//	strData.Format(_T("%d"), nSerial);
//	::WritePrivateProfileString(_T("Info"), _T("Total Shot"), strData, sPath);
//
//	for (k = 0; k < MAX_DEF; k++)
//	{
//		nDefA[k] = 0;
//
//		for (i = 0; i < 4; i++)
//		{
//			nDefPerStrip[i][k] = 0;
//		}
//	}
//
//	for (nRow = 0; nRow < nNodeY; nRow++)
//	{
//		for (nCol = 0; nCol < nNodeX; nCol++)
//		{
//			if (m_pPnlBuf)
//			{
//				nDefCode = (int)m_pPnlBuf[nPnl][nRow][nCol] < 0 ? 0 : (int)m_pPnlBuf[nPnl][nRow][nCol];
//				nDefA[nDefCode]++;
//
//				nStrip = int(nRow / (nNodeY / MAX_STRIP));
//				if (nStrip > -1 && nStrip < MAX_STRIP)
//				{
//					if (nDefCode > 0)
//					{
//						nDefStrip[nStrip]++;
//						nDefPerStrip[nStrip][nDefCode]++;
//					}
//				}
//			}
//		}
//	}
//
//	for (nStrip = 0; nStrip < 4; nStrip++)
//	{
//		if (nDefStrip[nStrip] >= nStripPcs * dStOutRto)
//			nStripOut[nStrip]++;
//	}
//
//	for (i = 1; i < MAX_DEF; i++)
//	{
//		sCode.Format(_T("%d"), i); 
//		m_stYield.nDefA[i] = m_stYield.nDefA[i] + nDefA[i];
//		sDefNum.Format(_T("%d"), m_stYield.nDefA[i]);
//		::WritePrivateProfileString(_T("Info"), sCode, sDefNum, sPath);
//		::WritePrivateProfileString(strMenu, sCode, sDefNum, sPath);
//
//		nDef += nDefA[i];
//	}
//
//	nGood = nTot - nDef;
//
//	m_stYield.nTot = m_stYield.nTot + nTot;
//	strData.Format(_T("%d"), m_stYield.nTot);
//	::WritePrivateProfileString(_T("Info"), _T("Total Pcs"), strData, sPath);
//	::WritePrivateProfileString(strMenu, _T("Total Pcs"), strData, sPath);
//
//	m_stYield.nGood = m_stYield.nGood + nGood;
//	strData.Format(_T("%d"), m_stYield.nGood);
//	::WritePrivateProfileString(_T("Info"), _T("Good Pcs"), strData, sPath);
//	::WritePrivateProfileString(strMenu, _T("Good Pcs"), strData, sPath);
//
//	m_stYield.nDef = m_stYield.nDef + nDef;
//	strData.Format(_T("%d"), m_stYield.nDef);
//	::WritePrivateProfileString(_T("Info"), _T("Bad Pcs"), strData, sPath);
//	::WritePrivateProfileString(strMenu, _T("Bad Pcs"), strData, sPath);
//
//	for (k = 0; k < 4; k++)
//	{
//		strItem.Format(_T("Strip%d"), k);
//		m_stYield.nDefStrip[k] = m_stYield.nDefStrip[k] + nDefStrip[k];
//		strData.Format(_T("%d"), m_stYield.nDefStrip[k]);
//		::WritePrivateProfileString(_T("Info"), strItem, strData, sPath);
//		::WritePrivateProfileString(strMenu, strItem, strData, sPath);
//
//		strItem.Format(_T("StripOut_%d"), k); 
//		m_stYield.nStripOut[k] = m_stYield.nStripOut[k] + nStripOut[k];
//		strData.Format(_T("%d"), m_stYield.nStripOut[k]);
//		::WritePrivateProfileString(_T("Info"), strItem, strData, sPath);
//		::WritePrivateProfileString(strMenu, strItem, strData, sPath);
//
//		nTotSriptOut += nStripOut[k];
//
//		for (i = 1; i < MAX_DEF; i++)
//		{
//			strItem.Format(_T("Strip%d_%d"), k, i);
//			m_stYield.nDefPerStrip[k][i] = m_stYield.nDefPerStrip[k][i] + nDefPerStrip[k][i];
//			strData.Format(_T("%d"), m_stYield.nDefPerStrip[k][i]);
//			::WritePrivateProfileString(_T("Info"), strItem, strData, sPath);
//			::WritePrivateProfileString(strMenu, strItem, strData, sPath);
//		}
//	}
//
//	m_stYield.nTotSriptOut = m_stYield.nTotSriptOut + nTotSriptOut;
//	strData.Format(_T("%d"), m_stYield.nTotSriptOut);
//	::WritePrivateProfileString(_T("Info"), _T("StripOut_Total"), strData, sPath);
//	::WritePrivateProfileString(strMenu, _T("StripOut_Total"), strData, sPath);
//
//	return TRUE;
//}

BOOL CReelMap::UpdateYield(int nSerial)
{
	if (nSerial <= 0)
	{
		AfxMessageBox(_T("Serial Error.68"));
		return 0;
	}

	m_sPathYield = GetYieldPath(m_nLayer);
	CString sPath = m_sPathYield;

	BOOL bExist = FALSE;
	CFileFind findfile;
	if (findfile.FindFile(sPath))
		bExist = TRUE;

	int nPnl = m_nBeforeSerial;//nSerial - 1;

	if (bExist && nPnl) // After first shot
	{
		if(ReadYield(nPnl, sPath))
			WriteYield(nSerial, sPath);
		else
		{
			ResetYield();
			WriteYield(nSerial, sPath);
		}
	}
	else // First Shot
	{
		ResetYield();
		WriteYield(nSerial, sPath);
	}
	Sleep(10);

	return TRUE;
}



BOOL CReelMap::UpdateReelmapYield()
{
	int k, i;
	CString strMenu, strItem, sCode, sDefNum, strData;

	m_nGoodPcs = m_stYield.nGood;
	m_nBadPcs =  m_stYield.nDef;

	strData.Format(_T("%d"), m_stYield.nTot);
	::WritePrivateProfileString(_T("Info"), _T("Total Pcs"), strData, m_sPathBuf);
	strData.Format(_T("%d"), m_nGoodPcs);
	::WritePrivateProfileString(_T("Info"), _T("Good Pcs"), strData, m_sPathBuf);
	strData.Format(_T("%d"), m_nBadPcs);
	::WritePrivateProfileString(_T("Info"), _T("Bad Pcs"), strData, m_sPathBuf);

	for (i = 1; i < MAX_DEF; i++)
	{
		m_nDef[i] = m_stYield.nDefA[i];
		sCode.Format(_T("%d"), i);
		sDefNum.Format(_T("%d"), m_nDef[i]);
		::WritePrivateProfileString(_T("Info"), sCode, sDefNum, m_sPathBuf);
	}

	int nTotStOut = 0;
	for (k = 0; k < MAX_STRIP; k++)
	{
		strItem.Format(_T("Strip%d"), k);
		m_nDefStrip[k] = m_stYield.nDefStrip[k];
		strData.Format(_T("%d"), m_nDefStrip[k]);
		::WritePrivateProfileString(_T("Info"), strItem, strData, m_sPathBuf);

		strItem.Format(_T("%d"), k);
		m_nStripOut[k] = m_stYield.nStripOut[k];
		strData.Format(_T("%d"), m_nStripOut[k]);
		::WritePrivateProfileString(_T("StripOut"), strItem, strData, m_sPathBuf);

		for (i = 1; i < MAX_DEF; i++)
		{
			strMenu.Format(_T("Strip%d"), k);
			strItem.Format(_T("%d"), i);
			m_nDefPerStrip[k][i] = m_stYield.nDefPerStrip[k][i];
			strData.Format(_T("%d"), m_nDefPerStrip[k][i]);
			::WritePrivateProfileString(strMenu, strItem, strData, m_sPathBuf);
		}
	}
	strData.Format(_T("%d"), m_stYield.nTotSriptOut);
	::WritePrivateProfileString(_T("StripOut"), _T("Total"), strData, m_sPathBuf);

	return TRUE;
}

int CReelMap::GetDefNum(int nDefCode)
{
	return m_stYield.nDefA[nDefCode];
	//return m_nDef[nDefCode];
}

int CReelMap::GetDefStrip(int nStrip)
{
	return m_stYield.nDefStrip[nStrip];
	//return m_nDefStrip[nStrip];
}

int CReelMap::GetDefStrip(int nStrip, int nDefCode)
{
	return m_stYield.nDefPerStrip[nStrip][nDefCode];
	//return m_nDefPerStrip[nStrip][nDefCode];
}

int CReelMap::GetStripOut(int nStrip)
{
	return m_stYield.nStripOut[nStrip];
	//return m_nStripOut[nStrip];
}

void CReelMap::GetPcsNum(int &nGood, int &nBad)
{
	nGood = m_stYield.nGood;
	nBad = m_stYield.nDef;
	//nGood = m_nGoodPcs;
	//nBad = m_nBadPcs;
}

void CReelMap::Clear()
{
	m_nSerial = 0;
	m_nLastShot = 0;
	m_nWritedSerial = 0;
	ClrFixPcs();
	ClrRst();
	ClrPcs();
	ClrPnlNum();
	ClrPnlDefNum();
}

int CReelMap::GetLastSerial()
{
	return m_nWritedSerial;
}

void CReelMap::SetFixPcs(int nSerial)
{
// 	if(!pDoc->WorkingInfo.LastJob.bContFixDef)
// 		return;

	int nNodeX = pDoc->m_Master[0].m_pPcsRgn->nCol;
	int nNodeY = pDoc->m_Master[0].m_pPcsRgn->nRow;
	int nR, nC, nDefCode;

	for(nR=0; nR<nNodeY; nR++)
	{
		for(nC=0; nC<nNodeX; nC++)
		{
			nDefCode = m_pPnlBuf[nSerial-1][nR][nC]; // DefCode
			if(nDefCode > 0)
				SetFixPcs(nSerial, nC, nR);
		}
	}
}

void CReelMap::SetFixPcs(int nSerial, int nCol, int nRow) // nShot : 0 ~ 
{
	if(nSerial < 1)
		return;

	int nRange = _tstoi(pDoc->WorkingInfo.LastJob.sNumRangeFixDef);
	int nS = (nSerial-1) % nRange;
	m_FixPcs[nS][nCol][nRow] = TRUE;
}

void CReelMap::ClrFixPcs()
{
	m_nPrevSerial[0] = 0;	// --
	m_nPrevSerial[1] = 0;	// ++

	for(int nS=0; nS<FIX_PCS_SHOT_MAX; nS++)
	{
		if(!nS)
		{
			m_nPrevSerial[0] = 0; // [0] : --
			m_nPrevSerial[1] = 0; // [1] : ++
		}

		for(int nC=0; nC<FIX_PCS_COL_MAX; nC++)
		{
			for(int nR=0; nR<FIX_PCS_ROW_MAX; nR++)
			{
				m_FixPcs[nS][nC][nR] = FALSE;
				if(!nS)
				{
					m_FixPcsPrev[nC][nR] = 0;
					m_FixPcsPrevStSerial[nC][nR] = 0;
				}		
			}
		}
	}
}

void CReelMap::ClrFixPcs(int nCol, int nRow)
{
	for(int nS=0; nS<FIX_PCS_SHOT_MAX; nS++)
	{
		m_FixPcs[nS][nCol][nRow] = FALSE;
	}
	m_FixPcsPrev[nCol][nRow] = 0;
	m_FixPcsPrevStSerial[nCol][nRow] = 0;
}

BOOL CReelMap::IsFixPcs(int nSerial, int &Col, int &Row)
{
	BOOL bRtn = FALSE;
	int nNodeX = pDoc->m_Master[0].m_pPcsRgn->nCol;
	int nNodeY = pDoc->m_Master[0].m_pPcsRgn->nRow;

	BOOL bOn = pDoc->WorkingInfo.LastJob.bContFixDef;
	int nRange = _tstoi(pDoc->WorkingInfo.LastJob.sNumRangeFixDef);
	int nJudge = _tstoi(pDoc->WorkingInfo.LastJob.sNumContFixDef);

// 	if(nSerial < nJudge)
// 		return bRtn;

	if(m_nPrevSerial[0] == nSerial)
		return bRtn;
	m_nPrevSerial[0] = nSerial;

	int nS;

	for(int nC=0; nC<nNodeX; nC++)
	{
		for(int nR=0; nR<nNodeY; nR++)
		{
			if(nSerial >= nRange)
			{
				nS = (nSerial-1) % nRange;
				if(nS == nRange-1)
					nS = 0;
				else
					nS++;

				if(m_FixPcs[nS][nC][nR]) // Defect PCS
				{
					if(m_FixPcsPrev[nC][nR] > 0)
						m_FixPcsPrev[nC][nR]--;
				}
				m_FixPcs[nS][nC][nR] = FALSE;
			}

			nS = (nSerial-1) % nRange;
			if(m_FixPcs[nS][nC][nR]) // Defect PCS
			{
// 				if(m_nPrevSerial[1] != nSerial) // [1] : ++
// 				{
//					m_nPrevSerial[1] = nSerial;
					m_FixPcsPrev[nC][nR]++;
//				}
			}

			if(bOn)
			{
				if(m_FixPcsPrev[nC][nR] >= nJudge)
				{
					m_FixPcsRpt[nC][nR]++;
					Col = nC;
					Row = nR;
					ClrFixPcs(nC, nR);
					bRtn = TRUE;
				}				
			}
		}
	}

	return bRtn;
}

BOOL CReelMap::IsFixPcs(int nSerial, int* pCol, int* pRow, int &nTot, BOOL &bCont) // nTot : total of PCS Over nJudge
{
	BOOL bRtn = FALSE;
	int nNodeX = pDoc->m_Master[0].m_pPcsRgn->nCol;
	int nNodeY = pDoc->m_Master[0].m_pPcsRgn->nRow;

	BOOL bOn = pDoc->WorkingInfo.LastJob.bContFixDef;
	int nRange = _tstoi(pDoc->WorkingInfo.LastJob.sNumRangeFixDef);
	int nJudge = _tstoi(pDoc->WorkingInfo.LastJob.sNumContFixDef);

// 	if(nSerial < nJudge)
// 		return bRtn;

	if(m_nPrevSerial[0] == nSerial)
		return bRtn;
	m_nPrevSerial[0] = nSerial;

	int nS = (nSerial-1) % nRange;
	nTot = 0;

	for(int nC=0; nC<nNodeX; nC++)
	{
		for(int nR=0; nR<nNodeY; nR++)
		{
			if(nSerial >= nRange)
			{
				nS = (nSerial-1) % nRange;
				if(nS == nRange-1)
					nS = 0;
				else
					nS++;

				if(m_FixPcs[nS][nC][nR]) // Defect PCS
				{
					if(m_FixPcsPrev[nC][nR] > 0)
						m_FixPcsPrev[nC][nR]--;
				}
				m_FixPcs[nS][nC][nR] = FALSE;
			}

			nS = (nSerial-1) % nRange;
			if(m_FixPcs[nS][nC][nR]) // Defect PCS
			{
				if (!m_FixPcsPrev[nC][nR])
					m_FixPcsPrevStSerial[nC][nR] = nSerial;
				m_FixPcsPrev[nC][nR]++;
			}

			if(bOn)
			{
				if(m_FixPcsPrev[nC][nR] >= nJudge)
				{
					if (nSerial - m_FixPcsPrevStSerial[nC][nR] == nJudge - 1)
						bCont = TRUE;
					m_FixPcsRpt[nC][nR]++;
					pCol[nTot] = nC;
					pRow[nTot] = nR;
					nTot++;
					ClrFixPcs(nC, nR);
					bRtn = TRUE;
				}				
			}
		}
	}

	return bRtn;
}


BOOL CReelMap::MakeDir()
{
//	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;
//
//#ifndef TEST_MODE
//	CFileFind finder;
//	CString str, sPath, Path[4];
//
//	if(bDualTest)
//	{
//		switch(m_nLayer)
//		{
//		case RMAP_UP:
//			str = _T("ReelMapDataUp.txt");
//			Path[0] = pDoc->WorkingInfo.System.sPathOldFile;
//			Path[1] = pDoc->WorkingInfo.LastJob.sModelUp;
//			Path[2] = pDoc->WorkingInfo.LastJob.sLotUp;
//			Path[3] = pDoc->WorkingInfo.LastJob.sLayerUp;
//			break;
//		case RMAP_ALLUP:
//			str = _T("ReelMapDataAll.txt");
//			Path[0] = pDoc->WorkingInfo.System.sPathOldFile;
//			Path[1] = pDoc->WorkingInfo.LastJob.sModelUp;
//			Path[2] = pDoc->WorkingInfo.LastJob.sLotUp;
//			Path[3] = pDoc->WorkingInfo.LastJob.sLayerUp;
//			break;
//		case RMAP_DN:
//			str = _T("ReelMapDataDn.txt");
//			Path[0] = pDoc->WorkingInfo.System.sPathOldFile;
//			//Path[1] = pDoc->WorkingInfo.LastJob.sModelDn;
//			//Path[2] = pDoc->WorkingInfo.LastJob.sLotDn;
//			Path[1] = pDoc->WorkingInfo.LastJob.sModelUp;
//			Path[2] = pDoc->WorkingInfo.LastJob.sLotUp;
//			Path[3] = pDoc->WorkingInfo.LastJob.sLayerDn;
//			break;
//		case RMAP_ALLDN:
//			str = _T("ReelMapDataAll.txt");
//			Path[0] = pDoc->WorkingInfo.System.sPathOldFile;
//			//Path[1] = pDoc->WorkingInfo.LastJob.sModelDn;
//			//Path[2] = pDoc->WorkingInfo.LastJob.sLotDn;
//			Path[1] = pDoc->WorkingInfo.LastJob.sModelUp;
//			Path[2] = pDoc->WorkingInfo.LastJob.sLotUp;
//			Path[3] = pDoc->WorkingInfo.LastJob.sLayerDn;
//			break;
//		}
//	}
//	else
//	{
//		str = _T("ReelMapDataUp.txt");
//		Path[0] = pDoc->WorkingInfo.System.sPathOldFile;
//		Path[1] = pDoc->WorkingInfo.LastJob.sModelUp;
//		Path[2] = pDoc->WorkingInfo.LastJob.sLotUp;
//		Path[3] = pDoc->WorkingInfo.LastJob.sLayerUp;
//	}
//#endif

	CString str, sPath, Path[5];

	switch(m_nLayer)
	{
	case RMAP_UP:
		str = _T("ReelMapDataUp.txt");
		Path[0] = pDoc->WorkingInfo.System.sPathOldFile;
		Path[1] = pDoc->WorkingInfo.LastJob.sModelUp;
		Path[2] = pDoc->WorkingInfo.LastJob.sLotUp;
		Path[3] = pDoc->WorkingInfo.LastJob.sLayerUp;
		Path[4] = _T("");
		break;
	case RMAP_ALLUP:
		str = _T("ReelMapDataAll.txt");
		Path[0] = pDoc->WorkingInfo.System.sPathOldFile;
		Path[1] = pDoc->WorkingInfo.LastJob.sModelUp;
		Path[2] = pDoc->WorkingInfo.LastJob.sLotUp;
		Path[3] = pDoc->WorkingInfo.LastJob.sLayerUp;
		Path[4] = _T("");
		break;
	case RMAP_DN:
		str = _T("ReelMapDataDn.txt");
		Path[0] = pDoc->WorkingInfo.System.sPathOldFile;
		Path[1] = pDoc->WorkingInfo.LastJob.sModelUp;
		Path[2] = pDoc->WorkingInfo.LastJob.sLotUp;
		Path[3] = pDoc->WorkingInfo.LastJob.sLayerDn;
		Path[4] = _T("");
		break;
	case RMAP_ALLDN:
		str = _T("ReelMapDataAll.txt");
		Path[0] = pDoc->WorkingInfo.System.sPathOldFile;
		Path[1] = pDoc->WorkingInfo.LastJob.sModelUp;
		Path[2] = pDoc->WorkingInfo.LastJob.sLotUp;
		Path[3] = pDoc->WorkingInfo.LastJob.sLayerDn;
		Path[4] = _T("");
		break;
	case RMAP_INNER_UP:
		str = _T("ReelMapDataUp.txt");
		Path[0] = pDoc->WorkingInfo.System.sPathItsFile;
		Path[1] = pDoc->WorkingInfo.LastJob.sModelUp;
		Path[2] = pDoc->WorkingInfo.LastJob.sEngItsCode;
		Path[3] = pDoc->WorkingInfo.LastJob.sInnerLotUp;
		Path[4] = pDoc->WorkingInfo.LastJob.sInnerLayerUp;
		break;
	case RMAP_INNER_DN:
		str = _T("ReelMapDataDn.txt");
		Path[0] = pDoc->WorkingInfo.System.sPathItsFile;
		Path[1] = pDoc->WorkingInfo.LastJob.sModelUp;
		Path[2] = pDoc->WorkingInfo.LastJob.sEngItsCode;
		Path[3] = pDoc->WorkingInfo.LastJob.sInnerLotUp;
		Path[4] = pDoc->WorkingInfo.LastJob.sInnerLayerDn;
		break;
	case RMAP_INNER_ALLUP:
		str = _T("ReelMapDataAll.txt");
		Path[0] = pDoc->WorkingInfo.System.sPathItsFile;
		Path[1] = pDoc->WorkingInfo.LastJob.sModelUp;
		Path[2] = pDoc->WorkingInfo.LastJob.sEngItsCode;
		Path[3] = pDoc->WorkingInfo.LastJob.sInnerLotUp;
		Path[4] = pDoc->WorkingInfo.LastJob.sInnerLayerUp;
		break;
	case RMAP_INNER_ALLDN:
		str = _T("ReelMapDataAll.txt");
		Path[0] = pDoc->WorkingInfo.System.sPathItsFile;
		Path[1] = pDoc->WorkingInfo.LastJob.sModelUp;
		Path[2] = pDoc->WorkingInfo.LastJob.sEngItsCode;
		Path[3] = pDoc->WorkingInfo.LastJob.sInnerLotUp;
		Path[4] = pDoc->WorkingInfo.LastJob.sInnerLayerDn;
		break;
	case RMAP_INOUTER_UP:
		break;
	case RMAP_INOUTER_DN:
		break;
	case RMAP_ITS:
		str = _T("ReelMapDataIts.txt");
		Path[0] = pDoc->WorkingInfo.System.sPathItsFile;
		Path[1] = pDoc->WorkingInfo.LastJob.sModelUp;
		Path[2] = pDoc->WorkingInfo.LastJob.sEngItsCode;
		Path[3] = _T("");
		Path[4] = _T("");
		break;
	}

	if (!Path[0].IsEmpty() && !Path[1].IsEmpty())
	{
		sPath.Format(_T("%s%s"), Path[0], Path[1]);
		if (!pDoc->DirectoryExists(sPath))
			CreateDirectory(sPath, NULL);

		if (!Path[2].IsEmpty())
		{
			sPath.Format(_T("%s%s\\%s"), Path[0], Path[1], Path[2]);
			if (!pDoc->DirectoryExists(sPath))
				CreateDirectory(sPath, NULL);

			if (!Path[3].IsEmpty())
			{
				sPath.Format(_T("%s%s\\%s\\%s"), Path[0], Path[1], Path[2], Path[3]);
				if (!pDoc->DirectoryExists(sPath))
					CreateDirectory(sPath, NULL);

				if (!Path[4].IsEmpty())
				{
					sPath.Format(_T("%s%s\\%s\\%s\\%s"), Path[0], Path[1], Path[2], Path[3], Path[4]);
					if (!pDoc->DirectoryExists(sPath))
						CreateDirectory(sPath, NULL);
				}
			}
		}
	}

	return TRUE;
}

BOOL CReelMap::MakeDirRmap()
{
	CString str, sPath, Path[5];

	switch (m_nLayer)
	{
	case RMAP_UP:
		str = _T("ReelMapDataUp.txt");
		Path[0] = pDoc->WorkingInfo.System.sPathOldFile;
		Path[1] = pDoc->WorkingInfo.LastJob.sModelUp;
		Path[2] = pDoc->WorkingInfo.LastJob.sLotUp;
		Path[3] = pDoc->WorkingInfo.LastJob.sLayerUp;
		Path[4] = _T("");
		break;
	case RMAP_ALLUP:
		str = _T("ReelMapDataAll.txt");
		Path[0] = pDoc->WorkingInfo.System.sPathOldFile;
		Path[1] = pDoc->WorkingInfo.LastJob.sModelUp;
		Path[2] = pDoc->WorkingInfo.LastJob.sLotUp;
		Path[3] = pDoc->WorkingInfo.LastJob.sLayerUp;
		Path[4] = _T("");
		break;
	case RMAP_DN:
		str = _T("ReelMapDataDn.txt");
		Path[0] = pDoc->WorkingInfo.System.sPathOldFile;
		Path[1] = pDoc->WorkingInfo.LastJob.sModelUp;
		Path[2] = pDoc->WorkingInfo.LastJob.sLotUp;
		Path[3] = pDoc->WorkingInfo.LastJob.sLayerDn;
		Path[4] = _T("");
		break;
	case RMAP_ALLDN:
		str = _T("ReelMapDataAll.txt");
		Path[0] = pDoc->WorkingInfo.System.sPathOldFile;
		Path[1] = pDoc->WorkingInfo.LastJob.sModelUp;
		Path[2] = pDoc->WorkingInfo.LastJob.sLotUp;
		Path[3] = pDoc->WorkingInfo.LastJob.sLayerDn;
		Path[4] = _T("");
		break;
	case RMAP_INNER_UP:
		str = _T("YieldUp.txt");
		Path[0] = pDoc->WorkingInfo.System.sPathItsFile;
		Path[1] = pDoc->WorkingInfo.LastJob.sModelUp;
		Path[2] = pDoc->WorkingInfo.LastJob.sEngItsCode;
		Path[3] = pDoc->WorkingInfo.LastJob.sInnerLotUp;
		Path[4] = pDoc->WorkingInfo.LastJob.sInnerLayerUp;
		break;
	case RMAP_INNER_DN:
		str = _T("YieldDn.txt");
		Path[0] = pDoc->WorkingInfo.System.sPathItsFile;
		Path[1] = pDoc->WorkingInfo.LastJob.sModelUp;
		Path[2] = pDoc->WorkingInfo.LastJob.sEngItsCode;
		Path[3] = pDoc->WorkingInfo.LastJob.sInnerLotUp;
		Path[4] = pDoc->WorkingInfo.LastJob.sInnerLayerDn;
		break;
	case RMAP_INNER_ALLUP:
		str = _T("YieldAll.txt");
		Path[0] = pDoc->WorkingInfo.System.sPathItsFile;
		Path[1] = pDoc->WorkingInfo.LastJob.sModelUp;
		Path[2] = pDoc->WorkingInfo.LastJob.sEngItsCode;
		Path[3] = pDoc->WorkingInfo.LastJob.sInnerLotUp;
		Path[4] = pDoc->WorkingInfo.LastJob.sInnerLayerUp;
		break;
	case RMAP_INNER_ALLDN:
		str = _T("YieldAll.txt");
		Path[0] = pDoc->WorkingInfo.System.sPathItsFile;
		Path[1] = pDoc->WorkingInfo.LastJob.sModelUp;
		Path[2] = pDoc->WorkingInfo.LastJob.sEngItsCode;
		Path[3] = pDoc->WorkingInfo.LastJob.sInnerLotUp;
		Path[4] = pDoc->WorkingInfo.LastJob.sInnerLayerDn;
		break;
	case RMAP_INOUTER_UP:
		break;
	case RMAP_INOUTER_DN:
		break;
	case RMAP_ITS:
		str = _T("YieldIts.txt");
		Path[0] = pDoc->WorkingInfo.System.sPathItsFile;
		Path[1] = pDoc->WorkingInfo.LastJob.sModelUp;
		Path[2] = pDoc->WorkingInfo.LastJob.sEngItsCode;
		Path[3] = _T("");
		Path[4] = _T("");
		break;
	}

	if (!Path[0].IsEmpty() && !Path[1].IsEmpty())
	{
		sPath.Format(_T("%s%s"), Path[0], Path[1]);
		if (!pDoc->DirectoryExists(sPath))
			CreateDirectory(sPath, NULL);

		if (!Path[2].IsEmpty())
		{
			sPath.Format(_T("%s%s\\%s"), Path[0], Path[1], Path[2]);
			if (!pDoc->DirectoryExists(sPath))
				CreateDirectory(sPath, NULL);

			if (!Path[3].IsEmpty())
			{
				sPath.Format(_T("%s%s\\%s\\%s"), Path[0], Path[1], Path[2], Path[3]);
				if (!pDoc->DirectoryExists(sPath))
					CreateDirectory(sPath, NULL);

				if (!Path[4].IsEmpty())
				{
					sPath.Format(_T("%s%s\\%s\\%s\\%s"), Path[0], Path[1], Path[2], Path[3], Path[4]);
					if (!pDoc->DirectoryExists(sPath))
						CreateDirectory(sPath, NULL);
				}
			}
		}
	}

	return TRUE;
}

CString CReelMap::MakeDirRmapRestore()
{
//	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;
//	CString sPath=_T("");
//	
//#ifndef TEST_MODE
//	CFileFind finder;
//	CString str, Path[4];
//
//	if(bDualTest)
//	{
//		switch(m_nLayer)
//		{
//		case RMAP_UP:
//			str = _T("ReelMapDataUp.txt");
//			Path[0] = pDoc->WorkingInfo.System.sPathOldFile;
//			Path[1] = pDoc->WorkingInfo.LastJob.sModelUp;
//			Path[2] = pDoc->WorkingInfo.LastJob.sLotUp;
//			Path[3] = pDoc->WorkingInfo.LastJob.sLayerUp;
//			break;
//		case RMAP_ALLUP:
//			str = _T("ReelMapDataAll.txt");
//			Path[0] = pDoc->WorkingInfo.System.sPathOldFile;
//			Path[1] = pDoc->WorkingInfo.LastJob.sModelUp;
//			Path[2] = pDoc->WorkingInfo.LastJob.sLotUp;
//			Path[3] = pDoc->WorkingInfo.LastJob.sLayerUp;
//			break;
//		case RMAP_DN:
//			str = _T("ReelMapDataDn.txt");
//			Path[0] = pDoc->WorkingInfo.System.sPathOldFile;
//			//Path[1] = pDoc->WorkingInfo.LastJob.sModelDn;
//			//Path[2] = pDoc->WorkingInfo.LastJob.sLotDn;
//			Path[1] = pDoc->WorkingInfo.LastJob.sModelUp;
//			Path[2] = pDoc->WorkingInfo.LastJob.sLotUp;
//			Path[3] = pDoc->WorkingInfo.LastJob.sLayerDn;
//			break;
//		case RMAP_ALLDN:
//			str = _T("ReelMapDataAll.txt");
//			Path[0] = pDoc->WorkingInfo.System.sPathOldFile;
//			//Path[1] = pDoc->WorkingInfo.LastJob.sModelDn;
//			//Path[2] = pDoc->WorkingInfo.LastJob.sLotDn;
//			Path[1] = pDoc->WorkingInfo.LastJob.sModelUp;
//			Path[2] = pDoc->WorkingInfo.LastJob.sLotUp;
//			Path[3] = pDoc->WorkingInfo.LastJob.sLayerDn;
//			break;
//		}
//	}
//	else
//	{
//		str = _T("ReelMapDataUp.txt");
//		Path[0] = pDoc->WorkingInfo.System.sPathOldFile;
//		Path[1] = pDoc->WorkingInfo.LastJob.sModelUp;
//		Path[2] = pDoc->WorkingInfo.LastJob.sLotUp;
//		Path[3] = pDoc->WorkingInfo.LastJob.sLayerUp;
//	}
//
//	sPath.Format(_T("%s%s"), Path[0], Path[1]);
//	//if(!finder.FindFile(sPath))
//	if (!pDoc->DirectoryExists(sPath))
//		CreateDirectory(sPath, NULL);
//
//	sPath.Format(_T("%s%s\\%s"), Path[0], Path[1], Path[2]);
//	//if(!finder.FindFile(sPath))
//	if (!pDoc->DirectoryExists(sPath))
//		CreateDirectory(sPath, NULL);
//
//	sPath.Format(_T("%s%s\\%s\\%s"), Path[0], Path[1], Path[2], Path[3]);
//	//if(!finder.FindFile(sPath))
//	if (!pDoc->DirectoryExists(sPath))
//		CreateDirectory(sPath, NULL);
//
//	sPath.Format(_T("%s%s\\%s\\%s\\Reelmap"), Path[0], Path[1], Path[2], Path[3]);
//	//if(!finder.FindFile(sPath))
//	if (!pDoc->DirectoryExists(sPath))
//		CreateDirectory(sPath, NULL);
//
//	sPath.Format(_T("%s%s\\%s\\%s\\Reelmap\\Restore"), Path[0], Path[1], Path[2], Path[3]);
//	//if(!finder.FindFile(sPath))
//	if (!pDoc->DirectoryExists(sPath))
//		CreateDirectory(sPath, NULL);
//#endif

	CString str, sPath, Path[5];

	switch (m_nLayer)
	{
	case RMAP_UP:
		str = _T("ReelMapDataUp.txt");
		Path[0] = pDoc->WorkingInfo.System.sPathOldFile;
		Path[1] = pDoc->WorkingInfo.LastJob.sModelUp;
		Path[2] = pDoc->WorkingInfo.LastJob.sLotUp;
		Path[3] = pDoc->WorkingInfo.LastJob.sLayerUp;
		Path[4] = _T("");
		break;
	case RMAP_ALLUP:
		str = _T("ReelMapDataAll.txt");
		Path[0] = pDoc->WorkingInfo.System.sPathOldFile;
		Path[1] = pDoc->WorkingInfo.LastJob.sModelUp;
		Path[2] = pDoc->WorkingInfo.LastJob.sLotUp;
		Path[3] = pDoc->WorkingInfo.LastJob.sLayerUp;
		Path[4] = _T("");
		break;
	case RMAP_DN:
		str = _T("ReelMapDataDn.txt");
		Path[0] = pDoc->WorkingInfo.System.sPathOldFile;
		Path[1] = pDoc->WorkingInfo.LastJob.sModelUp;
		Path[2] = pDoc->WorkingInfo.LastJob.sLotUp;
		Path[3] = pDoc->WorkingInfo.LastJob.sLayerDn;
		Path[4] = _T("");
		break;
	case RMAP_ALLDN:
		str = _T("ReelMapDataAll.txt");
		Path[0] = pDoc->WorkingInfo.System.sPathOldFile;
		Path[1] = pDoc->WorkingInfo.LastJob.sModelUp;
		Path[2] = pDoc->WorkingInfo.LastJob.sLotUp;
		Path[3] = pDoc->WorkingInfo.LastJob.sLayerDn;
		Path[4] = _T("");
		break;
	case RMAP_INNER_UP:
		str = _T("ReelMapDataUp.txt");
		Path[0] = pDoc->WorkingInfo.System.sPathItsFile;
		Path[1] = pDoc->WorkingInfo.LastJob.sModelUp;
		Path[2] = pDoc->WorkingInfo.LastJob.sEngItsCode;
		Path[3] = pDoc->WorkingInfo.LastJob.sInnerLotUp;
		Path[4] = pDoc->WorkingInfo.LastJob.sInnerLayerUp;
		break;
	case RMAP_INNER_DN:
		str = _T("ReelMapDataDn.txt");
		Path[0] = pDoc->WorkingInfo.System.sPathItsFile;
		Path[1] = pDoc->WorkingInfo.LastJob.sModelUp;
		Path[2] = pDoc->WorkingInfo.LastJob.sEngItsCode;
		Path[3] = pDoc->WorkingInfo.LastJob.sInnerLotUp;
		Path[4] = pDoc->WorkingInfo.LastJob.sInnerLayerDn;
		break;
	case RMAP_INNER_ALLUP:
		str = _T("ReelMapDataAll.txt");
		Path[0] = pDoc->WorkingInfo.System.sPathItsFile;
		Path[1] = pDoc->WorkingInfo.LastJob.sModelUp;
		Path[2] = pDoc->WorkingInfo.LastJob.sEngItsCode;
		Path[3] = pDoc->WorkingInfo.LastJob.sInnerLotUp;
		Path[4] = pDoc->WorkingInfo.LastJob.sInnerLayerUp;
		break;
	case RMAP_INNER_ALLDN:
		str = _T("ReelMapDataAll.txt");
		Path[0] = pDoc->WorkingInfo.System.sPathItsFile;
		Path[1] = pDoc->WorkingInfo.LastJob.sModelUp;
		Path[2] = pDoc->WorkingInfo.LastJob.sEngItsCode;
		Path[3] = pDoc->WorkingInfo.LastJob.sInnerLotUp;
		Path[4] = pDoc->WorkingInfo.LastJob.sInnerLayerDn;
		break;
	case RMAP_INOUTER_UP:
		break;
	case RMAP_INOUTER_DN:
		break;
	case RMAP_ITS:
		str = _T("ReelMapDataIts.txt");
		Path[0] = pDoc->WorkingInfo.System.sPathItsFile;
		Path[1] = pDoc->WorkingInfo.LastJob.sModelUp;
		Path[2] = pDoc->WorkingInfo.LastJob.sEngItsCode;
		Path[3] = _T("");
		Path[4] = _T("");
		break;
	}

	if (!Path[0].IsEmpty() && !Path[1].IsEmpty())
	{
		sPath.Format(_T("%s%s"), Path[0], Path[1]);
		if (!pDoc->DirectoryExists(sPath))
			CreateDirectory(sPath, NULL);

		if (!Path[2].IsEmpty())
		{
			sPath.Format(_T("%s%s\\%s"), Path[0], Path[1], Path[2]);
			if (!pDoc->DirectoryExists(sPath))
				CreateDirectory(sPath, NULL);

			if (!Path[3].IsEmpty())
			{
				sPath.Format(_T("%s%s\\%s\\%s"), Path[0], Path[1], Path[2], Path[3]);
				if (!pDoc->DirectoryExists(sPath))
					CreateDirectory(sPath, NULL);

				if (!Path[4].IsEmpty())
				{
					sPath.Format(_T("%s%s\\%s\\%s\\%s"), Path[0], Path[1], Path[2], Path[3], Path[4]);
					if (!pDoc->DirectoryExists(sPath))
						CreateDirectory(sPath, NULL);

					sPath.Format(_T("%s%s\\%s\\%s\\%s\\Restore"), Path[0], Path[1], Path[2], Path[3], Path[4]);
					if (!pDoc->DirectoryExists(sPath))
						CreateDirectory(sPath, NULL);
				}
				else
				{
					sPath.Format(_T("%s%s\\%s\\%s\\Restore"), Path[0], Path[1], Path[2], Path[3]);
					if (!pDoc->DirectoryExists(sPath))
						CreateDirectory(sPath, NULL);
				}
			}
			else
			{
				sPath.Format(_T("%s%s\\%s\\Restore"), Path[0], Path[1], Path[2]);
				if (!pDoc->DirectoryExists(sPath))
					CreateDirectory(sPath, NULL);
			}
		}
		else
		{
			sPath.Format(_T("%s%s\\Restore"), Path[0], Path[1]);
			if (!pDoc->DirectoryExists(sPath))
				CreateDirectory(sPath, NULL);
		}
	}
	else
	{
		pView->MsgBox(_T("Error- Path[0].IsEmpty() || Path[1].IsEmpty()"));
		sPath = _T("");
	}

	return sPath;
}

BOOL CReelMap::MakeDir(CString sModel, CString sLayer, CString sLot)
{
//	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;
//
//#ifndef TEST_MODE
//	CFileFind finder;
//	CString str, sPath, Path[4];
//
//	if(bDualTest)
//	{
//		switch(m_nLayer)
//		{
//		case RMAP_UP:
//			str = _T("ReelMapDataUp.txt");
//			break;
//		case RMAP_ALLUP:
//			str = _T("ReelMapDataAll.txt");
//			break;
//		case RMAP_DN:
//			str = _T("ReelMapDataDn.txt");
//			break;
//		case RMAP_ALLDN:
//			str = _T("ReelMapDataAll.txt");
//			break;
//		}
//	}
//	else
//		str = _T("ReelMapDataUp.txt");
//	
//	Path[0] = pDoc->WorkingInfo.System.sPathOldFile;
//	Path[1] = sModel;
//	Path[2] = sLot;
//	Path[3] = sLayer;
//
//	sPath.Format(_T("%s%s"), Path[0], Path[1]);
//	//if(!finder.FindFile(sPath))
//	if (!pDoc->DirectoryExists(sPath))
//		CreateDirectory(sPath, NULL);
//
//	sPath.Format(_T("%s%s\\%s"), Path[0], Path[1], Path[2]);
//	//if(!finder.FindFile(sPath))
//	if (!pDoc->DirectoryExists(sPath))
//		CreateDirectory(sPath, NULL);
//
//	sPath.Format(_T("%s%s\\%s\\%s"), Path[0], Path[1], Path[2], Path[3]);
//	//if(!finder.FindFile(sPath))
//	if (!pDoc->DirectoryExists(sPath))
//		CreateDirectory(sPath, NULL);
//#endif
	return TRUE;
}

BOOL CReelMap::MakeDirRmap(CString sModel, CString sLayer, CString sLot)
{
	CString str, sPath, Path[5];

	switch (m_nLayer)
	{
	case RMAP_UP:
		str = _T("ReelMapDataUp.txt");
		Path[0] = pDoc->WorkingInfo.System.sPathOldFile;
		Path[1] = sModel;
		Path[2] = sLot;
		Path[3] = sLayer;
		Path[4] = _T("");
		break;
	case RMAP_ALLUP:
		str = _T("ReelMapDataAll.txt");
		Path[0] = pDoc->WorkingInfo.System.sPathOldFile;
		Path[1] = sModel;
		Path[2] = sLot;
		Path[3] = sLayer;
		Path[4] = _T("");
		break;
	case RMAP_DN:
		str = _T("ReelMapDataDn.txt");
		Path[0] = pDoc->WorkingInfo.System.sPathOldFile;
		Path[1] = sModel;
		Path[2] = sLot;
		Path[3] = sLayer;
		Path[4] = _T("");
		break;
	case RMAP_ALLDN:
		str = _T("ReelMapDataAll.txt");
		Path[0] = pDoc->WorkingInfo.System.sPathOldFile;
		Path[1] = sModel;
		Path[2] = sLot;
		Path[3] = sLayer;
		Path[4] = _T("");
		break;
	case RMAP_INNER_UP:
		str = _T("YieldUp.txt");
		Path[0] = pDoc->WorkingInfo.System.sPathItsFile;
		Path[1] = sModel;
		Path[2] = pDoc->WorkingInfo.LastJob.sEngItsCode;
		Path[3] = sLot;
		Path[4] = sLayer;
		break;
	case RMAP_INNER_DN:
		str = _T("YieldDn.txt");
		Path[0] = pDoc->WorkingInfo.System.sPathItsFile;
		Path[1] = sModel;
		Path[2] = pDoc->WorkingInfo.LastJob.sEngItsCode;
		Path[3] = sLot;
		Path[4] = sLayer;
		break;
	case RMAP_INNER_ALLUP:
		str = _T("YieldAll.txt");
		Path[0] = pDoc->WorkingInfo.System.sPathItsFile;
		Path[1] = sModel;
		Path[2] = pDoc->WorkingInfo.LastJob.sEngItsCode;
		Path[3] = sLot;
		Path[4] = sLayer;
		break;
	case RMAP_INNER_ALLDN:
		str = _T("YieldAll.txt");
		Path[0] = pDoc->WorkingInfo.System.sPathItsFile;
		Path[1] = sModel;
		Path[2] = pDoc->WorkingInfo.LastJob.sEngItsCode;
		Path[3] = sLot;
		Path[4] = sLayer;
		break;
	case RMAP_INOUTER_UP:
		break;
	case RMAP_INOUTER_DN:
		break;
	case RMAP_ITS:
		str = _T("YieldIts.txt");
		Path[0] = pDoc->WorkingInfo.System.sPathItsFile;
		Path[1] = Path[1] = sModel;
		Path[2] = pDoc->WorkingInfo.LastJob.sEngItsCode;
		Path[3] = _T("");
		Path[4] = _T("");
		break;
	}

	if (!Path[0].IsEmpty() && !Path[1].IsEmpty())
	{
		sPath.Format(_T("%s%s"), Path[0], Path[1]);
		if (!pDoc->DirectoryExists(sPath))
			CreateDirectory(sPath, NULL);

		if (!Path[2].IsEmpty())
		{
			sPath.Format(_T("%s%s\\%s"), Path[0], Path[1], Path[2]);
			if (!pDoc->DirectoryExists(sPath))
				CreateDirectory(sPath, NULL);

			if (!Path[3].IsEmpty())
			{
				sPath.Format(_T("%s%s\\%s\\%s"), Path[0], Path[1], Path[2], Path[3]);
				if (!pDoc->DirectoryExists(sPath))
					CreateDirectory(sPath, NULL);

				if (!Path[4].IsEmpty())
				{
					sPath.Format(_T("%s%s\\%s\\%s\\%s"), Path[0], Path[1], Path[2], Path[3], Path[4]);
					if (!pDoc->DirectoryExists(sPath))
						CreateDirectory(sPath, NULL);
				}
			}
		}
	}


//	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;
//
//#ifndef TEST_MODE
//	CFileFind finder;
//	CString str, sPath, Path[4];
//
//	if(bDualTest)
//	{
//		switch(m_nLayer)
//		{
//		case RMAP_UP:
//			str = _T("ReelMapDataUp.txt");
//			break;
//		case RMAP_ALLUP:
//			str = _T("ReelMapDataAll.txt");
//			break;
//		case RMAP_DN:
//			str = _T("ReelMapDataDn.txt");
//			break;
//		case RMAP_ALLDN:
//			str = _T("ReelMapDataAll.txt");
//			break;
//		}
//	}
//	else
//		str = _T("ReelMapDataUp.txt");
//	
//	Path[0] = pDoc->WorkingInfo.System.sPathOldFile;
//	Path[1] = sModel;
//	Path[2] = sLot;
//	Path[3] = sLayer;
//
//	sPath.Format(_T("%s%s"), Path[0], Path[1]);
//	//if(!finder.FindFile(sPath))
//	if (!pDoc->DirectoryExists(sPath))
//		CreateDirectory(sPath, NULL);
//
//	sPath.Format(_T("%s%s\\%s"), Path[0], Path[1], Path[2]);
//	//if(!finder.FindFile(sPath))
//	if (!pDoc->DirectoryExists(sPath))
//		CreateDirectory(sPath, NULL);
//
//	sPath.Format(_T("%s%s\\%s\\%s"), Path[0], Path[1], Path[2], Path[3]);
//	//if(!finder.FindFile(sPath))
//	if (!pDoc->DirectoryExists(sPath))
//		CreateDirectory(sPath, NULL);
//
//	sPath.Format(_T("%s%s\\%s\\%s\\Reelmap"), Path[0], Path[1], Path[2], Path[3]);
//	//if(!finder.FindFile(sPath))
//	if (!pDoc->DirectoryExists(sPath))
//		CreateDirectory(sPath, NULL);
//#endif
	return TRUE;
}

CString CReelMap::MakeDirRmapRestore(CString sModel, CString sLayer, CString sLot)
{
	CString str, sPath, Path[5];

	switch (m_nLayer)
	{
	case RMAP_UP:
		str = _T("ReelMapDataUp.txt");
		Path[0] = pDoc->WorkingInfo.System.sPathOldFile;
		Path[1] = sModel;
		Path[2] = sLot;
		Path[3] = sLayer;
		Path[4] = _T("");
		break;
	case RMAP_ALLUP:
		str = _T("ReelMapDataAll.txt");
		Path[0] = pDoc->WorkingInfo.System.sPathOldFile;
		Path[1] = sModel;
		Path[2] = sLot;
		Path[3] = sLayer;
		Path[4] = _T("");
		break;
	case RMAP_DN:
		str = _T("ReelMapDataDn.txt");
		Path[0] = pDoc->WorkingInfo.System.sPathOldFile;
		Path[1] = sModel;
		Path[2] = sLot;
		Path[3] = sLayer;
		Path[4] = _T("");
		break;
	case RMAP_ALLDN:
		str = _T("ReelMapDataAll.txt");
		Path[0] = pDoc->WorkingInfo.System.sPathOldFile;
		Path[1] = sModel;
		Path[2] = sLot;
		Path[3] = sLayer;
		Path[4] = _T("");
		break;
	case RMAP_INNER_UP:
		str = _T("YieldUp.txt");
		Path[0] = pDoc->WorkingInfo.System.sPathItsFile;
		Path[1] = sModel;
		Path[2] = pDoc->WorkingInfo.LastJob.sEngItsCode;
		Path[3] = sLot;
		Path[4] = sLayer;
		break;
	case RMAP_INNER_DN:
		str = _T("YieldDn.txt");
		Path[0] = pDoc->WorkingInfo.System.sPathItsFile;
		Path[1] = sModel;
		Path[2] = pDoc->WorkingInfo.LastJob.sEngItsCode;
		Path[3] = sLot;
		Path[4] = sLayer;
		break;
	case RMAP_INNER_ALLUP:
		str = _T("YieldAll.txt");
		Path[0] = pDoc->WorkingInfo.System.sPathItsFile;
		Path[1] = sModel;
		Path[2] = pDoc->WorkingInfo.LastJob.sEngItsCode;
		Path[3] = sLot;
		Path[4] = sLayer;
		break;
	case RMAP_INNER_ALLDN:
		str = _T("YieldAll.txt");
		Path[0] = pDoc->WorkingInfo.System.sPathItsFile;
		Path[1] = sModel;
		Path[2] = pDoc->WorkingInfo.LastJob.sEngItsCode;
		Path[3] = sLot;
		Path[4] = sLayer;
		break;
	case RMAP_INOUTER_UP:
		break;
	case RMAP_INOUTER_DN:
		break;
	case RMAP_ITS:
		str = _T("YieldIts.txt");
		Path[0] = pDoc->WorkingInfo.System.sPathItsFile;
		Path[1] = Path[1] = sModel;
		Path[2] = pDoc->WorkingInfo.LastJob.sEngItsCode;
		Path[3] = _T("");
		Path[4] = _T("");
		break;
	}

	if (!Path[0].IsEmpty() && !Path[1].IsEmpty())
	{
		sPath.Format(_T("%s%s"), Path[0], Path[1]);
		if (!pDoc->DirectoryExists(sPath))
			CreateDirectory(sPath, NULL);

		if (!Path[2].IsEmpty())
		{
			sPath.Format(_T("%s%s\\%s"), Path[0], Path[1], Path[2]);
			if (!pDoc->DirectoryExists(sPath))
				CreateDirectory(sPath, NULL);

			if (!Path[3].IsEmpty())
			{
				sPath.Format(_T("%s%s\\%s\\%s"), Path[0], Path[1], Path[2], Path[3]);
				if (!pDoc->DirectoryExists(sPath))
					CreateDirectory(sPath, NULL);

				if (!Path[4].IsEmpty())
				{
					sPath.Format(_T("%s%s\\%s\\%s\\%s"), Path[0], Path[1], Path[2], Path[3], Path[4]);
					if (!pDoc->DirectoryExists(sPath))
						CreateDirectory(sPath, NULL);
				}
			}
		}
	}


//	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;
//	CString sPath;
//#ifndef TEST_MODE
//	CFileFind finder;
//	CString str, Path[4];
//
//	if(bDualTest)
//	{
//		switch(m_nLayer)
//		{
//		case RMAP_UP:
//			str = _T("ReelMapDataUp.txt");
//			break;
//		case RMAP_ALLUP:
//			str = _T("ReelMapDataAll.txt");
//			break;
//		case RMAP_DN:
//			str = _T("ReelMapDataDn.txt");
//			break;
//		case RMAP_ALLDN:
//			str = _T("ReelMapDataAll.txt");
//			break;
//		}
//	}
//	else
//		str = _T("ReelMapDataUp.txt");
//	
//	Path[0] = pDoc->WorkingInfo.System.sPathOldFile;
//	Path[1] = sModel;
//	Path[2] = sLot;
//	Path[3] = sLayer;
//
//	sPath.Format(_T("%s%s"), Path[0], Path[1]);
//	//if(!finder.FindFile(sPath))
//	if (!pDoc->DirectoryExists(sPath))
//		CreateDirectory(sPath, NULL);
//
//	sPath.Format(_T("%s%s\\%s"), Path[0], Path[1], Path[2]);
//	//if(!finder.FindFile(sPath))
//	if (!pDoc->DirectoryExists(sPath))
//		CreateDirectory(sPath, NULL);
//
//	sPath.Format(_T("%s%s\\%s\\%s"), Path[0], Path[1], Path[2], Path[3]);
//	//if(!finder.FindFile(sPath))
//	if (!pDoc->DirectoryExists(sPath))
//		CreateDirectory(sPath, NULL);
//
//	sPath.Format(_T("%s%s\\%s\\%s\\Reelmap"), Path[0], Path[1], Path[2], Path[3]);
//	//if(!finder.FindFile(sPath))
//	if (!pDoc->DirectoryExists(sPath))
//		CreateDirectory(sPath, NULL);
//
//	sPath.Format(_T("%s%s\\%s\\%s\\Reelmap\\Restore"), Path[0], Path[1], Path[2], Path[3]);
//	//if(!finder.FindFile(sPath))
//	if (!pDoc->DirectoryExists(sPath))
//		CreateDirectory(sPath, NULL);
//#endif
	return sPath;
}

void CReelMap::SetPathAtBuf()
{
	m_sPathBuf = GetRmapPath(m_nLayer);
}

//CString CReelMap::GetRmapPath(int nRmap)
//{
//	CString sPath;
//#ifdef TEST_MODE
//	sPath = PATH_REELMAP;
//#else
//	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;
//
//	CString str;
//
//	if(bDualTest)
//	{
//		switch(nRmap)
//		{
//		case RMAP_UP:
//			str = _T("ReelMapDataUp.txt");
//			if(pDoc->m_bDoneChgLot || !pDoc->m_bNewLotShare[0])
//			{
//				sPath.Format(_T("%s%s\\%s\\%s\\%s"), pDoc->WorkingInfo.System.sPathOldFile, 
//												pDoc->WorkingInfo.LastJob.sModelUp, 
//												pDoc->WorkingInfo.LastJob.sLotUp, 
//												pDoc->WorkingInfo.LastJob.sLayerUp,
//												str);
//			}
//			else if(!pDoc->m_bDoneChgLot && pDoc->m_bNewLotShare[0])
//			{
//				sPath.Format(_T("%s%s\\%s\\%s\\%s"), pDoc->WorkingInfo.System.sPathOldFile, 
//												pDoc->WorkingInfo.LastJob.sModelUp, 
//												pDoc->Status.PcrShare[0].sLot, 
//												pDoc->WorkingInfo.LastJob.sLayerUp,
//												str);
//			}
//			break;
//		case RMAP_ALLUP:
//			#ifdef TEST_MODE
//			str = _T("ReelMapDataAllUp.txt");
//			#else
//			str = _T("ReelMapDataAll.txt");
//			#endif
//			if(pDoc->m_bDoneChgLot || !pDoc->m_bNewLotShare[0])
//			{
//				sPath.Format(_T("%s%s\\%s\\%s\\%s"), pDoc->WorkingInfo.System.sPathOldFile, 
//												pDoc->WorkingInfo.LastJob.sModelUp, 
//												pDoc->WorkingInfo.LastJob.sLotUp, 
//												pDoc->WorkingInfo.LastJob.sLayerUp,
//												str);
//			}
//			else if(!pDoc->m_bDoneChgLot && pDoc->m_bNewLotShare[0])
//			{
//				sPath.Format(_T("%s%s\\%s\\%s\\%s"), pDoc->WorkingInfo.System.sPathOldFile, 
//												pDoc->WorkingInfo.LastJob.sModelUp, 
//												pDoc->Status.PcrShare[0].sLot, 
//												pDoc->WorkingInfo.LastJob.sLayerUp,
//												str);
//			}
//			break;
//		case RMAP_DN:
//			str = _T("ReelMapDataDn.txt");
//			if(pDoc->m_bDoneChgLot || !pDoc->m_bNewLotShare[1])
//			{
//				sPath.Format(_T("%s%s\\%s\\%s\\%s"), pDoc->WorkingInfo.System.sPathOldFile, 
//												//pDoc->WorkingInfo.LastJob.sModelDn, 
//												//pDoc->WorkingInfo.LastJob.sLotDn, 
//												pDoc->WorkingInfo.LastJob.sModelUp,
//												pDoc->WorkingInfo.LastJob.sLotUp,
//												pDoc->WorkingInfo.LastJob.sLayerDn,
//												str);
//			}
//			else if(!pDoc->m_bDoneChgLot && pDoc->m_bNewLotShare[1])
//			{
//				sPath.Format(_T("%s%s\\%s\\%s\\%s"), pDoc->WorkingInfo.System.sPathOldFile, 
//												//pDoc->WorkingInfo.LastJob.sModelDn, 
//												//pDoc->Status.PcrShare[1].sLot, 
//												pDoc->WorkingInfo.LastJob.sModelUp,
//												pDoc->Status.PcrShare[0].sLot,
//												pDoc->WorkingInfo.LastJob.sLayerDn,
//												str);
//			}
//			break;
//		case RMAP_ALLDN:
//			#ifdef TEST_MODE
//			str = _T("ReelMapDataAllDn.txt");
//			#else
//			str = _T("ReelMapDataAll.txt");
//			#endif
//			if(pDoc->m_bDoneChgLot || !pDoc->m_bNewLotShare[1])
//			{
//				sPath.Format(_T("%s%s\\%s\\%s\\%s"), pDoc->WorkingInfo.System.sPathOldFile, 
//												//pDoc->WorkingInfo.LastJob.sModelDn, 
//												//pDoc->WorkingInfo.LastJob.sLotDn, 
//												pDoc->WorkingInfo.LastJob.sModelUp,
//												pDoc->WorkingInfo.LastJob.sLotUp,
//												pDoc->WorkingInfo.LastJob.sLayerDn,
//												str);
//			}
//			else if(!pDoc->m_bDoneChgLot && pDoc->m_bNewLotShare[1])
//			{
//				sPath.Format(_T("%s%s\\%s\\%s\\%s"), pDoc->WorkingInfo.System.sPathOldFile, 
//												//pDoc->WorkingInfo.LastJob.sModelDn, 
//												//pDoc->Status.PcrShare[1].sLot, 
//												pDoc->WorkingInfo.LastJob.sModelUp,
//												pDoc->Status.PcrShare[0].sLot,
//												pDoc->WorkingInfo.LastJob.sLayerDn,
//												str);
//			}
//			break;
//		}
//	}
//	else
//	{
//		str = _T("ReelMapDataUp.txt");
//		if(pDoc->m_bDoneChgLot || !pDoc->m_bNewLotShare[0])
//		{
//			sPath.Format(_T("%s%s\\%s\\%s\\%s"), pDoc->WorkingInfo.System.sPathOldFile, 
//											pDoc->WorkingInfo.LastJob.sModelUp, 
//											pDoc->WorkingInfo.LastJob.sLotUp, 
//											pDoc->WorkingInfo.LastJob.sLayerUp,
//											str);
//		}
//		else if(!pDoc->m_bDoneChgLot && pDoc->m_bNewLotShare[0])
//		{
//			sPath.Format(_T("%s%s\\%s\\%s\\%s"), pDoc->WorkingInfo.System.sPathOldFile, 
//											pDoc->WorkingInfo.LastJob.sModelUp, 
//											pDoc->Status.PcrShare[0].sLot, 
//											pDoc->WorkingInfo.LastJob.sLayerUp,
//											str);
//		}
//	}
//#endif
//	return sPath;
//}

CString CReelMap::GetRmapPath(int nRmap)
{
	CString sPath = _T("");
	CString Path[5], str;

	switch (nRmap)
	{
	case RMAP_UP:
		str = _T("ReelMapDataUp.txt");
		if (pDoc->m_bDoneChgLot || !pDoc->m_bNewLotShare[0])
		{
			sPath.Format(_T("%s%s\\%s\\%s\\%s"), pDoc->WorkingInfo.System.sPathOldFile,
				pDoc->WorkingInfo.LastJob.sModelUp,
				pDoc->WorkingInfo.LastJob.sLotUp,
				pDoc->WorkingInfo.LastJob.sLayerUp,
				str);
		}
		else if (!pDoc->m_bDoneChgLot && pDoc->m_bNewLotShare[0])
		{
			sPath.Format(_T("%s%s\\%s\\%s\\%s"), pDoc->WorkingInfo.System.sPathOldFile,
				pDoc->WorkingInfo.LastJob.sModelUp,
				pDoc->Status.PcrShare[0].sLot,
				pDoc->WorkingInfo.LastJob.sLayerUp,
				str);
		}
		break;
	case RMAP_ALLUP:
		str = _T("ReelMapDataAll.txt");
		if (pDoc->m_bDoneChgLot || !pDoc->m_bNewLotShare[0])
		{
			sPath.Format(_T("%s%s\\%s\\%s\\%s"), pDoc->WorkingInfo.System.sPathOldFile,
				pDoc->WorkingInfo.LastJob.sModelUp,
				pDoc->WorkingInfo.LastJob.sLotUp,
				pDoc->WorkingInfo.LastJob.sLayerUp,
				str);
		}
		else if (!pDoc->m_bDoneChgLot && pDoc->m_bNewLotShare[0])
		{
			sPath.Format(_T("%s%s\\%s\\%s\\%s"), pDoc->WorkingInfo.System.sPathOldFile,
				pDoc->WorkingInfo.LastJob.sModelUp,
				pDoc->Status.PcrShare[0].sLot,
				pDoc->WorkingInfo.LastJob.sLayerUp,
				str);
		}
		break;
	case RMAP_DN:
		str = _T("ReelMapDataDn.txt");
		if (pDoc->m_bDoneChgLot || !pDoc->m_bNewLotShare[1])
		{
			sPath.Format(_T("%s%s\\%s\\%s\\%s"), pDoc->WorkingInfo.System.sPathOldFile,
				pDoc->WorkingInfo.LastJob.sModelUp,
				pDoc->WorkingInfo.LastJob.sLotUp,
				pDoc->WorkingInfo.LastJob.sLayerDn,
				str);
		}
		else if (!pDoc->m_bDoneChgLot && pDoc->m_bNewLotShare[1])
		{
			sPath.Format(_T("%s%s\\%s\\%s\\%s"), pDoc->WorkingInfo.System.sPathOldFile,
				pDoc->WorkingInfo.LastJob.sModelUp,
				pDoc->Status.PcrShare[0].sLot,
				pDoc->WorkingInfo.LastJob.sLayerDn,
				str);
		}
		break;
	case RMAP_ALLDN:
		str = _T("ReelMapDataAll.txt");
		if (pDoc->m_bDoneChgLot || !pDoc->m_bNewLotShare[1])
		{
			sPath.Format(_T("%s%s\\%s\\%s\\%s"), pDoc->WorkingInfo.System.sPathOldFile,
				pDoc->WorkingInfo.LastJob.sModelUp,
				pDoc->WorkingInfo.LastJob.sLotUp,
				pDoc->WorkingInfo.LastJob.sLayerDn,
				str);
		}
		else if (!pDoc->m_bDoneChgLot && pDoc->m_bNewLotShare[1])
		{
			sPath.Format(_T("%s%s\\%s\\%s\\%s"), pDoc->WorkingInfo.System.sPathOldFile,
				pDoc->WorkingInfo.LastJob.sModelUp,
				pDoc->Status.PcrShare[0].sLot,
				pDoc->WorkingInfo.LastJob.sLayerDn,
				str);
		}
		break;
	case RMAP_INNER_UP:
		str = _T("ReelMapDataUp.txt");
		Path[0] = pDoc->WorkingInfo.System.sPathItsFile;
		Path[1] = pDoc->WorkingInfo.LastJob.sModelUp;
		Path[2] = pDoc->WorkingInfo.LastJob.sEngItsCode;
		Path[3] = pDoc->WorkingInfo.LastJob.sInnerLotUp;
		Path[4] = pDoc->WorkingInfo.LastJob.sInnerLayerUp;
		sPath.Format(_T("%s%s\\%s\\%s\\%s\\%s"), Path[0], Path[1], Path[2], Path[3], Path[4], str);
		break;
	case RMAP_INNER_DN:
		str = _T("ReelMapDataDn.txt");
		Path[0] = pDoc->WorkingInfo.System.sPathItsFile;
		Path[1] = pDoc->WorkingInfo.LastJob.sModelUp;
		Path[2] = pDoc->WorkingInfo.LastJob.sEngItsCode;
		Path[3] = pDoc->WorkingInfo.LastJob.sInnerLotUp;
		Path[4] = pDoc->WorkingInfo.LastJob.sInnerLayerDn;
		sPath.Format(_T("%s%s\\%s\\%s\\%s\\%s"), Path[0], Path[1], Path[2], Path[3], Path[4], str);
		break;
	case RMAP_INNER_ALLUP:
		str = _T("ReelMapDataAll.txt");
		Path[0] = pDoc->WorkingInfo.System.sPathItsFile;
		Path[1] = pDoc->WorkingInfo.LastJob.sModelUp;
		Path[2] = pDoc->WorkingInfo.LastJob.sEngItsCode;
		Path[3] = pDoc->WorkingInfo.LastJob.sInnerLotUp;
		Path[4] = pDoc->WorkingInfo.LastJob.sInnerLayerUp;
		sPath.Format(_T("%s%s\\%s\\%s\\%s\\%s"), Path[0], Path[1], Path[2], Path[3], Path[4], str);
		break;
	case RMAP_INNER_ALLDN:
		str = _T("ReelMapDataAll.txt");
		Path[0] = pDoc->WorkingInfo.System.sPathItsFile;
		Path[1] = pDoc->WorkingInfo.LastJob.sModelUp;
		Path[2] = pDoc->WorkingInfo.LastJob.sEngItsCode;
		Path[3] = pDoc->WorkingInfo.LastJob.sInnerLotUp;
		Path[4] = pDoc->WorkingInfo.LastJob.sInnerLayerDn;
		sPath.Format(_T("%s%s\\%s\\%s\\%s\\%s"), Path[0], Path[1], Path[2], Path[3], Path[4], str);
		break;
	case RMAP_INOUTER_UP:
		str = _T("ReelMapDataIO.txt");
		sPath.Format(_T("%s%s\\%s\\%s\\%s\\%s"), pDoc->WorkingInfo.System.sPathItsFile,
			pDoc->WorkingInfo.LastJob.sModelUp, pDoc->WorkingInfo.LastJob.sEngItsCode,
			pDoc->WorkingInfo.LastJob.sLotUp, pDoc->WorkingInfo.LastJob.sLayerUp,
			str);
		break;
	case RMAP_INOUTER_DN:
		str = _T("ReelMapDataIO.txt");
		sPath.Format(_T("%s%s\\%s\\%s\\%s\\%s"), pDoc->WorkingInfo.System.sPathItsFile,
			pDoc->WorkingInfo.LastJob.sModelUp, pDoc->WorkingInfo.LastJob.sEngItsCode,
			pDoc->WorkingInfo.LastJob.sLotUp, pDoc->WorkingInfo.LastJob.sLayerDn,
			str);
		break;
	case RMAP_ITS:
		pDoc->GetCurrentInfoEng();
		str = _T("ReelMapDataIts.txt");
		sPath.Format(_T("%s%s\\%s\\%s"), pDoc->WorkingInfo.System.sPathItsFile,
			pDoc->WorkingInfo.LastJob.sModelUp,
			pDoc->WorkingInfo.LastJob.sEngItsCode,	//pDoc->m_sItsCode,
			str);
		break;
	}

	return sPath;
}

void CReelMap::UpdateProcessNum(CString sProcessNum, int nLayer)
{
	::WritePrivateProfileString(_T("Info"), _T("Process Code"), sProcessNum, GetRmapPath(nLayer));
}

void CReelMap::UpdateTotVel(CString sVel, int nLayer)
{
	::WritePrivateProfileString(_T("Info"), _T("Entire Speed"), sVel, GetRmapPath(nLayer));
}



void CReelMap::StartThreadRemakeReelmap() 
{
	m_bRtnThreadRemakeReelmap = FALSE;
	m_bThreadAliveRemakeReelmap = TRUE;	
	m_ThreadTaskRemakeReelmap.Start( GetSafeHwnd(),this,ThreadProcRemakeReelmap);// Start the thread
}

void CReelMap::StopThreadRemakeReelmap() 
{
	m_ThreadTaskRemakeReelmap.Stop();// Stop the thread
}

// Home thread body
BOOL CReelMap::ThreadProcRemakeReelmap( LPVOID lpContext )
{
	// Turn the passed in 'this' pointer back into a CProgressMgr instance
	CReelMap* pThread = reinterpret_cast< CReelMap* >( lpContext );
//	DWORD dwTimePeriod = 10; // 10ms sec sleep

	pThread->m_bRtnThreadRemakeReelmap = TRUE;
	pThread->m_bThreadAliveRemakeReelmap = TRUE;	
	
	//pThread->m_cs.Lock();
	pThread->m_bRtnThreadRemakeReelmap = pThread->RemakeReelmap();
	//pThread->m_cs.Unlock();

	pThread->m_bThreadAliveRemakeReelmap = FALSE;

	return (pThread->m_bRtnThreadRemakeReelmap);
}


BOOL CReelMap::GetNodeXYonRmap(int &nNodeX, int &nNodeY, CString sPath)
{
	TCHAR szData[MAX_PATH];
	CString sPnl, sRow, sLine;
	int nX, nY, nSt = 0;

	nNodeX = 0;
	nNodeY = 0;

	for (nY = 0; nY<10000; nY++)
	{
		sPnl = _T("1");
		sRow.Format(_T("%02d"), nY);
		if (0 >= ::GetPrivateProfileString(sPnl, sRow, NULL, szData, sizeof(szData), sPath))
		{
			nNodeX = nY;
			break;
		}
		else
		{
			sLine = CString(szData);
			for (nX = 0; nX<100000; nX++)
			{
				nSt = sLine.Find(',', nSt + 1);
				if (nSt < 0)
					break;
				else
					nNodeY++;
			}
			nNodeY++;
		}
	}

	if (!nNodeX || !nNodeY)
	{
		nNodeX = pDoc->m_Master[0].m_pPcsRgn->nCol; // on Cam
		nNodeY = pDoc->m_Master[0].m_pPcsRgn->nRow; // on Cam
	}

	return TRUE;
}

//BOOL CReelMap::RemakeReelmap()
//{
//	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;
//
//	CString sPath = m_sPathOnThread;
//	FILE *fp = NULL;
//	char FileName[MAX_PATH];
//	CString strFileName, strPathName;
//	int i;// , j, k, l;
//	//BOOL bDefOn;
//	CFileFind findfile;
//	//BOOL bFileCreate;
//	int nStripNumY, nPieceNumPerStrip;
//
//	CString sModel, sLot, sLayer[2];
//	int nLastShot, nPnl, nRow, nCol, nDefCode, nCompletedShot;// , nStrip;//, nC, nR;
//	CString sPnl, sRow, sVal;
//	TCHAR sep[] = {_T(",/;\r\n\t")};
//	TCHAR szData[MAX_PATH];
//
//	if (0 < ::GetPrivateProfileString(_T("Info"), _T("Completed Shot"), NULL, szData, sizeof(szData), sPath))
//		nCompletedShot = _tstoi(szData);
//	else
//	{
//		nCompletedShot = 0; // Failed.
//		//pView->MsgBox(_T("���ʿ� Completed Shot ������ �����ϴ�."));
//		//return FALSE;
//	}
//
//	if (0 < ::GetPrivateProfileString(_T("Info"), _T("Marked Shot"), NULL, szData, sizeof(szData), sPath))
//		nLastShot = _tstoi(szData); 
//	else
//	{
//		nLastShot = 0; // Failed.
//		pView->MsgBox(_T("���ʿ� Marked Shot ������ �����ϴ�."));
//		return FALSE;
//	}
//
//	if (0 < ::GetPrivateProfileString(_T("Info"), _T("��      ��"), NULL, szData, sizeof(szData), sPath))
//		sModel = CString(szData); 
//	else
//	{
//		pView->MsgBox(_T("Model ������ �����ϴ�."));
//		return FALSE;
//	}
//
//	if (0 < ::GetPrivateProfileString(_T("Info"), _T("��      Ʈ"), NULL, szData, sizeof(szData), sPath))
//		sLot = CString(szData); 
//	else
//	{
//		pView->MsgBox(_T("Lot ������ �����ϴ�."));
//		return FALSE;
//	}
//	
//	if (0 < ::GetPrivateProfileString(_T("Info"), _T("��鷹�̾�"), NULL, szData, sizeof(szData), sPath))
//		sLayer[0] = CString(szData); 
//	else
//	{
//		pView->MsgBox(_T("��鷹�̾� ������ �����ϴ�."));
//		return FALSE;
//	}
//
//	if(bDualTest)
//	{
//		if (0 < ::GetPrivateProfileString(_T("Info"), _T("�ϸ鷹�̾�"), NULL, szData, sizeof(szData), sPath))
//			sLayer[1] = CString(szData); 
//		else
//		{
//			pView->MsgBox(_T("�ϸ鷹�̾� ������ �����ϴ�."));
//			return FALSE;
//		}
//	}
//
//	MakeDirRmap(sModel, sLayer[0], sLot);
//
//	if(bDualTest)
//		MakeDirRmap(sModel, sLayer[1], sLot);
//
//	CString sFile=_T(""), sRmapPath=sPath;
//	int nPos = sRmapPath.ReverseFind('\\');
//	if (nPos != -1)
//	{
//		sFile = sRmapPath.Right(sRmapPath.GetLength()-nPos-1);
//		sRmapPath.Delete(nPos, sPath.GetLength() - nPos);
//	}
//
//	sFile.MakeUpper();
//	if(bDualTest)
//	{
//		if(sFile == _T("REELMAPDATAALL.TXT"))
//			strPathName = sRmapPath + _T("\\Reelmap\\") + _T("ReelMap-TOTAL_SIDE.txt");
//		else if(sFile == _T("REELMAPDATAUP.TXT"))
//			strPathName = sRmapPath + _T("\\Reelmap\\") + _T("ReelMap-TOP_SIDE.txt");
//		else if(sFile == _T("REELMAPDATADN.TXT"))
//			strPathName = sRmapPath + _T("\\Reelmap\\") + _T("ReelMap-BOTTOM_SIDE.txt");
//	}
//	else
//		strPathName = sRmapPath + _T("\\Reelmap\\") + _T("ReelMap-TOP_SIDE.txt");
//
//	if(findfile.FindFile(strPathName))
//	{
//		DeleteFile(strPathName);
//	}
//
//
//	int nNodeX=0;// = pDoc->m_pPcsRgn->nCol; // on Cam
//	int nNodeY=0;// = pDoc->m_pPcsRgn->nRow; // on Cam
//	
//	if (0 < ::GetPrivateProfileString(_T("Info"), _T("CamPcsX"), NULL, szData, sizeof(szData), sPath))
//		nNodeX = _tstoi(szData); 
//	if (0 < ::GetPrivateProfileString(_T("Info"), _T("CamPcsY"), NULL, szData, sizeof(szData), sPath))
//		nNodeY = _tstoi(szData); 
//
//	//int nX, nY, nSt=0;
//	if (!nNodeX || !nNodeY)
//		GetNodeXYonRmap(nNodeX, nNodeY, sPath);
//
//	//if(!nNodeX || !nNodeY)
//	//{
//	//	for(nY=0; nY<10000; nY++)
//	//	{
//	//		sPnl = _T("1");
//	//		sRow.Format(_T("%02d"), nY);
//	//		if(0 >= ::GetPrivateProfileString(sPnl, sRow, NULL, szData, sizeof(szData), sPath))
//	//		{
//	//			nNodeX = nY;
//	//			break;
//	//		}
//	//		else
//	//		{
//	//			if(!nNodeY)
//	//			{
//	//				CString sLine = CString(szData);
//	//				for(nX=0; nX<100000; nX++)
//	//				{
//	//					nSt = sLine.Find(',', nSt+1);
//	//					if(nSt < 0)
//	//						break;
//	//					else
//	//						nNodeY++;
//	//				}
//	//				nNodeY++;
//	//			}
//	//		}
//	//	}
//	//}
//	//if(!nNodeX || !nNodeY)
//	//{
//	//	nNodeX = pDoc->m_Master[0].m_pPcsRgn->nCol; // on Cam
//	//	nNodeY = pDoc->m_Master[0].m_pPcsRgn->nRow; // on Cam
//	//}
//
//	nStripNumY = MAX_STRIP_NUM;
//	nPieceNumPerStrip = nNodeY/nStripNumY;
//
//// 	double dRatio=0.0;
//// 	CDlgProgress dlg;
//// 	sVal.Format(_T("On remaking [%s]"), sFile);
//// 	dlg.Create(sVal);
//	m_dProgressRatio = 0.0;
//
//
//	//_tcscpy(FileName, strPathName);
//	StringToChar(strPathName, FileName);
//
//	fp = fopen(FileName, "a+");
//	char* pRtn = NULL;
//	if (fp != NULL)
//	{
//		fprintf(fp, "���ȣ��� : %s\n", pRtn = StrToChar(pDoc->WorkingInfo.System.sMcName)); if (pRtn) delete pRtn; pRtn = NULL;
//		fprintf(fp, "��  ��  �� : %s\n", pRtn = StrToChar(pDoc->WorkingInfo.LastJob.sSelUserName)); if (pRtn) delete pRtn; pRtn = NULL;
//		fprintf(fp, "��      �� : %s\n", pRtn = StrToChar(sModel)); if (pRtn) delete pRtn; pRtn = NULL;
//		fprintf(fp, "��      Ʈ : %s\n", pRtn = StrToChar(sLot)); if (pRtn) delete pRtn; pRtn = NULL;
//		fprintf(fp, "��鷹�̾� : %s\n", pRtn = StrToChar(sLayer[0])); if (pRtn) delete pRtn; pRtn = NULL;
//		if(bDualTest)
//		{
//			fprintf(fp, "�ϸ鷹�̾� : %s\n", pRtn = StrToChar(sLayer[1])); if (pRtn) delete pRtn; pRtn = NULL;
//		}
//		fprintf(fp, "���� ��Ʈ�� ����[%%] : %.1f\n", _tstof(pDoc->WorkingInfo.LastJob.sStripOutRatio)); if (pRtn) delete pRtn; pRtn = NULL;//atof
//		
//		for(i=1; i<DEF_UPPER; i++)
//		{
//			fprintf(fp, "%c - %s\n", pDoc->m_cBigDefCode[i], pRtn = StrToChar(m_sKorDef[i])); if (pRtn) delete pRtn; pRtn = NULL;
//		}
//		fprintf(fp, "? - Missed Align Panel(i.e �뱤�ҷ�)\r\n\n");
//
//		// Reload Reelmap.....
//		for(nPnl=0; nPnl<nLastShot; nPnl++)
//		{
//			m_dProgressRatio =  double(nPnl+1)/double(nLastShot) * 100.0;
//// 			dRatio = double(nPnl+1)/double(nLastShot) * 100.0;
//// 			if(!(int(dRatio)%10))
//// 				dlg.SetPos(int(dRatio));
//
//			fprintf(fp, "%d\n", nPnl+1);
//			for(nRow=0; nRow<nNodeX; nRow++)
//			{
//				sPnl.Format(_T("%d"), nPnl+1);
//				sRow.Format(_T("%02d"), nRow);
//				if (0 < ::GetPrivateProfileString(sPnl, sRow, NULL, szData, sizeof(szData), sPath))
//				{
//					for(nCol=0; nCol<nNodeY; nCol++)
//					{
//						if(nCol==0)
//							sVal = _tcstok(szData,sep);
//						else
//							sVal = _tcstok(NULL,sep);
//
//						nDefCode = _tstoi(sVal);
//
//// 						nR = (nNodeY-1)-nCol;
//// 						nC = nRow;
//
//						if(nCol % nPieceNumPerStrip == 0)
//							fprintf(fp, " ");
//
//						fprintf(fp, "%c", pDoc->m_cBigDefCode[nDefCode]);
//					}
//					fprintf(fp, "\n");
//				}
//			}
//			fprintf(fp, "\n");
//		}		
//	}
//	else
//	{
//		pView->MsgBox(_T("It is trouble to remake ReelMap."),MB_ICONWARNING|MB_OK);
//		return FALSE;
//	}
//
//// 	dlg.DestroyWindow();
//
//	fclose(fp);
//	return TRUE;
//}

void CReelMap::RestoreReelmap()
{
	CString sPath = MakeDirRmapRestore();
	int nLastDir = GetLastRmapRestoreDir(sPath);

	CString sUserRestoreDir;
	sUserRestoreDir.Format(_T("%s\\%d"), sPath, nLastDir+1);
	
	CFileFind finder;
	if (!pDoc->DirectoryExists(sUserRestoreDir))
		CreateDirectory(sUserRestoreDir, NULL);


	//BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;

	//CString str, Path[4], sSrc, sDest;

	//if(bDualTest)
	//{
	//	switch(m_nLayer)
	//	{
	//	case RMAP_UP:
	//		str = _T("ReelMapDataUp.txt");
	//		Path[0] = pDoc->WorkingInfo.System.sPathOldFile;
	//		Path[1] = pDoc->WorkingInfo.LastJob.sModelUp;
	//		Path[2] = pDoc->WorkingInfo.LastJob.sLotUp;
	//		Path[3] = pDoc->WorkingInfo.LastJob.sLayerUp;
	//		break;
	//	case RMAP_ALLUP:
	//		str = _T("ReelMapDataAll.txt");
	//		Path[0] = pDoc->WorkingInfo.System.sPathOldFile;
	//		Path[1] = pDoc->WorkingInfo.LastJob.sModelUp;
	//		Path[2] = pDoc->WorkingInfo.LastJob.sLotUp;
	//		Path[3] = pDoc->WorkingInfo.LastJob.sLayerUp;
	//		break;
	//	case RMAP_DN:
	//		str = _T("ReelMapDataDn.txt");
	//		Path[0] = pDoc->WorkingInfo.System.sPathOldFile;
	//		//Path[1] = pDoc->WorkingInfo.LastJob.sModelDn;
	//		//Path[2] = pDoc->WorkingInfo.LastJob.sLotDn;
	//		Path[1] = pDoc->WorkingInfo.LastJob.sModelUp;
	//		Path[2] = pDoc->WorkingInfo.LastJob.sLotUp;
	//		Path[3] = pDoc->WorkingInfo.LastJob.sLayerDn;
	//		break;
	//	case RMAP_ALLDN:
	//		str = _T("ReelMapDataAll.txt");
	//		Path[0] = pDoc->WorkingInfo.System.sPathOldFile;
	//		//Path[1] = pDoc->WorkingInfo.LastJob.sModelDn;
	//		//Path[2] = pDoc->WorkingInfo.LastJob.sLotDn;
	//		Path[1] = pDoc->WorkingInfo.LastJob.sModelUp;
	//		Path[2] = pDoc->WorkingInfo.LastJob.sLotUp;
	//		Path[3] = pDoc->WorkingInfo.LastJob.sLayerDn;
	//		break;
	//	}
	//}
	//else
	//{
	//	str = _T("ReelMapDataUp.txt");
	//	Path[0] = pDoc->WorkingInfo.System.sPathOldFile;
	//	Path[1] = pDoc->WorkingInfo.LastJob.sModelUp;
	//	Path[2] = pDoc->WorkingInfo.LastJob.sLotUp;
	//	Path[3] = pDoc->WorkingInfo.LastJob.sLayerUp;
	//}

	//CString sPath = _T("");
	CString Path[5], str, sSrc, sDest;

	switch (m_nLayer)
	{
	case RMAP_UP:
		str = _T("ReelMapDataUp.txt");
		if (pDoc->m_bDoneChgLot || !pDoc->m_bNewLotShare[0])
		{
			sPath.Format(_T("%s%s\\%s\\%s\\%s"), pDoc->WorkingInfo.System.sPathOldFile,
				pDoc->WorkingInfo.LastJob.sModelUp,
				pDoc->WorkingInfo.LastJob.sLotUp,
				pDoc->WorkingInfo.LastJob.sLayerUp,
				str);
		}
		else if (!pDoc->m_bDoneChgLot && pDoc->m_bNewLotShare[0])
		{
			sPath.Format(_T("%s%s\\%s\\%s\\%s"), pDoc->WorkingInfo.System.sPathOldFile,
				pDoc->WorkingInfo.LastJob.sModelUp,
				pDoc->Status.PcrShare[0].sLot,
				pDoc->WorkingInfo.LastJob.sLayerUp,
				str);
		}
		break;
	case RMAP_ALLUP:
		str = _T("ReelMapDataAll.txt");
		if (pDoc->m_bDoneChgLot || !pDoc->m_bNewLotShare[0])
		{
			sPath.Format(_T("%s%s\\%s\\%s\\%s"), pDoc->WorkingInfo.System.sPathOldFile,
				pDoc->WorkingInfo.LastJob.sModelUp,
				pDoc->WorkingInfo.LastJob.sLotUp,
				pDoc->WorkingInfo.LastJob.sLayerUp,
				str);
		}
		else if (!pDoc->m_bDoneChgLot && pDoc->m_bNewLotShare[0])
		{
			sPath.Format(_T("%s%s\\%s\\%s\\%s"), pDoc->WorkingInfo.System.sPathOldFile,
				pDoc->WorkingInfo.LastJob.sModelUp,
				pDoc->Status.PcrShare[0].sLot,
				pDoc->WorkingInfo.LastJob.sLayerUp,
				str);
		}
		break;
	case RMAP_DN:
		str = _T("ReelMapDataDn.txt");
		if (pDoc->m_bDoneChgLot || !pDoc->m_bNewLotShare[1])
		{
			sPath.Format(_T("%s%s\\%s\\%s\\%s"), pDoc->WorkingInfo.System.sPathOldFile,
				pDoc->WorkingInfo.LastJob.sModelUp,
				pDoc->WorkingInfo.LastJob.sLotUp,
				pDoc->WorkingInfo.LastJob.sLayerDn,
				str);
		}
		else if (!pDoc->m_bDoneChgLot && pDoc->m_bNewLotShare[1])
		{
			sPath.Format(_T("%s%s\\%s\\%s\\%s"), pDoc->WorkingInfo.System.sPathOldFile,
				pDoc->WorkingInfo.LastJob.sModelUp,
				pDoc->Status.PcrShare[0].sLot,
				pDoc->WorkingInfo.LastJob.sLayerDn,
				str);
		}
		break;
	case RMAP_ALLDN:
		str = _T("ReelMapDataAll.txt");
		if (pDoc->m_bDoneChgLot || !pDoc->m_bNewLotShare[1])
		{
			sPath.Format(_T("%s%s\\%s\\%s\\%s"), pDoc->WorkingInfo.System.sPathOldFile,
				pDoc->WorkingInfo.LastJob.sModelUp,
				pDoc->WorkingInfo.LastJob.sLotUp,
				pDoc->WorkingInfo.LastJob.sLayerDn,
				str);
		}
		else if (!pDoc->m_bDoneChgLot && pDoc->m_bNewLotShare[1])
		{
			sPath.Format(_T("%s%s\\%s\\%s\\%s"), pDoc->WorkingInfo.System.sPathOldFile,
				pDoc->WorkingInfo.LastJob.sModelUp,
				pDoc->Status.PcrShare[0].sLot,
				pDoc->WorkingInfo.LastJob.sLayerDn,
				str);
		}
		break;
	case RMAP_INNER_UP:
		str = _T("ReelMapDataUp.txt");
		Path[0] = pDoc->WorkingInfo.System.sPathItsFile;
		Path[1] = pDoc->WorkingInfo.LastJob.sModelUp;
		Path[2] = pDoc->WorkingInfo.LastJob.sEngItsCode;
		Path[3] = pDoc->WorkingInfo.LastJob.sInnerLotUp;
		Path[4] = pDoc->WorkingInfo.LastJob.sInnerLayerUp;
		sPath.Format(_T("%s%s\\%s\\%s\\%s\\%s"), Path[0], Path[1], Path[2], Path[3], Path[4], str);
		break;
	case RMAP_INNER_DN:
		str = _T("ReelMapDataDn.txt");
		Path[0] = pDoc->WorkingInfo.System.sPathItsFile;
		Path[1] = pDoc->WorkingInfo.LastJob.sModelUp;
		Path[2] = pDoc->WorkingInfo.LastJob.sEngItsCode;
		Path[3] = pDoc->WorkingInfo.LastJob.sInnerLotUp;
		Path[4] = pDoc->WorkingInfo.LastJob.sInnerLayerDn;
		sPath.Format(_T("%s%s\\%s\\%s\\%s\\%s"), Path[0], Path[1], Path[2], Path[3], Path[4], str);
		break;
	case RMAP_INNER_ALLUP:
		str = _T("ReelMapDataAll.txt");
		Path[0] = pDoc->WorkingInfo.System.sPathItsFile;
		Path[1] = pDoc->WorkingInfo.LastJob.sModelUp;
		Path[2] = pDoc->WorkingInfo.LastJob.sEngItsCode;
		Path[3] = pDoc->WorkingInfo.LastJob.sInnerLotUp;
		Path[4] = pDoc->WorkingInfo.LastJob.sInnerLayerUp;
		sPath.Format(_T("%s%s\\%s\\%s\\%s\\%s"), Path[0], Path[1], Path[2], Path[3], Path[4], str);
		break;
	case RMAP_INNER_ALLDN:
		str = _T("ReelMapDataAll.txt");
		Path[0] = pDoc->WorkingInfo.System.sPathItsFile;
		Path[1] = pDoc->WorkingInfo.LastJob.sModelUp;
		Path[2] = pDoc->WorkingInfo.LastJob.sEngItsCode;
		Path[3] = pDoc->WorkingInfo.LastJob.sInnerLotUp;
		Path[4] = pDoc->WorkingInfo.LastJob.sInnerLayerDn;
		sPath.Format(_T("%s%s\\%s\\%s\\%s\\%s"), Path[0], Path[1], Path[2], Path[3], Path[4], str);
		break;
	case RMAP_INOUTER_UP:
		str = _T("ReelMapDataIO.txt");
		sPath.Format(_T("%s%s\\%s\\%s\\%s\\%s"), pDoc->WorkingInfo.System.sPathItsFile,
			pDoc->WorkingInfo.LastJob.sModelUp, pDoc->WorkingInfo.LastJob.sEngItsCode,
			pDoc->WorkingInfo.LastJob.sLotUp, pDoc->WorkingInfo.LastJob.sLayerUp,
			str);
		break;
	case RMAP_INOUTER_DN:
		str = _T("ReelMapDataIO.txt");
		sPath.Format(_T("%s%s\\%s\\%s\\%s\\%s"), pDoc->WorkingInfo.System.sPathItsFile,
			pDoc->WorkingInfo.LastJob.sModelUp, pDoc->WorkingInfo.LastJob.sEngItsCode,
			pDoc->WorkingInfo.LastJob.sLotUp, pDoc->WorkingInfo.LastJob.sLayerDn,
			str);
		break;
	case RMAP_ITS:
		pDoc->GetCurrentInfoEng();
		str = _T("ReelMapDataIts.txt");
		sPath.Format(_T("%s%s\\%s\\%s"), pDoc->WorkingInfo.System.sPathItsFile,
			pDoc->WorkingInfo.LastJob.sModelUp, pDoc->WorkingInfo.LastJob.sEngItsCode,	//pDoc->m_sItsCode,
			str);
		break;
	}

	sSrc = sPath;
	sDest.Format(_T("%s\\%s"), sUserRestoreDir, str);

	CMyFile *pFile;
	pFile = new CMyFile();

	BOOL bRst = pFile->Copy(sSrc, sDest);			// ������ �����Ѵ�.

	delete pFile;
}

int CReelMap::GetLastRmapRestoreDir(CString strPath)
{
	CString strFileName;

	CFileFind cFile;
	BOOL bExist = cFile.FindFile(strPath);
	if(!bExist)
	{
		return 0; // �������� ����.
	}

	int nFirstFolder = 1000000;
	int nLastFolder = 0;

	strPath += _T("\\*.*");
	bExist = cFile.FindFile(strPath);

	while(bExist)
	{
		bExist = cFile.FindNextFile();
		if(cFile.IsDots()) continue;
		if(cFile.IsDirectory())
		{
			// �������� ����.
			strFileName = cFile.GetFileName();

			int nFileNum = _tstoi(strFileName);
			if(nFileNum < nFirstFolder)
				nFirstFolder = nFileNum;
			if(nFileNum > nLastFolder)
				nLastFolder = nFileNum;
		}
	}

	return nLastFolder;
}




void CReelMap::StartThreadReloadReelmap()
{
	m_bRtnThreadReloadReelmap = FALSE;
	m_bThreadAliveReloadReelmap = TRUE;
	m_ThreadTaskReloadReelmap.Start( GetSafeHwnd(),this,ThreadProcReloadReelmap);// Start the thread
}

void CReelMap::StopThreadReloadReelmap()
{
	m_ThreadTaskReloadReelmap.Stop();// Stop the thread
}

// Home thread body
BOOL CReelMap::ThreadProcReloadReelmap( LPVOID lpContext )
{
	// Turn the passed in 'this' pointer back into a CProgressMgr instance
	CReelMap* pThread = reinterpret_cast< CReelMap* >( lpContext );
//	DWORD dwTimePeriod = 10; // 10ms sec sleep

	pThread->m_bRtnThreadReloadReelmap = FALSE;
	pThread->m_bThreadAliveReloadReelmap = TRUE;	
	
	//pThread->m_cs.Lock();
	int nSerial = pDoc->GetLastShotMk();	// m_pDlgFrameHigh���� ��ų� ������, sPathOldFile������ ReelMapDataDn.txt���� _T("Info"), _T("Marked Shot") ã��.
	pThread->m_bRtnThreadReloadReelmap = pThread->ReloadReelmap(pThread->m_nLastOnThread);
	//pThread->m_cs.Unlock();

	pThread->m_bThreadAliveReloadReelmap = FALSE;

	return (pThread->m_bRtnThreadReloadReelmap);
}


BOOL CReelMap::IsDoneReloadReelmap()
{
	return m_bDoneReloadReelmap;
}

int CReelMap::GetProgressReloadReelmap()
{
	if(m_nTotalProgressReloadReelmap <= 0)
		return 0;

	double dA = (double)m_nProgressReloadReelmap;
	double dB = (double)m_nTotalProgressReloadReelmap;
	double dC = 100.0 * dA / dB;
	int nC = int(dC);

	return nC;
}

BOOL CReelMap::ReloadReelmap()
{
	//int nSerial;
	BOOL bRtn;
	
	//nSerial = pDoc->GetLastShotMk();
	int nLastOnThread = pDoc->GetLastShotMk();	// m_pDlgFrameHigh���� ��ų� ������, sPathOldFile������ ReelMapDataDn.txt���� _T("Info"), _T("Marked Shot") ã��.
	m_nLastOnThread = nLastOnThread;	// m_pDlgFrameHigh���� ��ų� ������, sPathOldFile������ ReelMapDataDn.txt���� _T("Info"), _T("Marked Shot") ã��.

	//if(nSerial > 0)
	if(m_nLastOnThread > 0)
	{
		//bRtn = ReloadReelmap(nSerial);
		m_bDoneReloadReelmap = FALSE;
		m_nProgressReloadReelmap = 0;
		m_nTotalProgressReloadReelmap = 0;
		StartThreadReloadReelmap();
		bRtn = TRUE;
	}
	else 
		bRtn = FALSE;

	return bRtn;
}

BOOL CReelMap::ReloadReelmap(int nTo)
{
	if(!m_pPnlBuf)
	{
		AfxMessageBox(_T("Memory is not allocated.- PnlBuf"));
		m_bDoneReloadReelmap = TRUE;
		return FALSE;
	}

	int nPnl, nRow, nCol, nDefCode, nStrip, nC, nR;
	CString sPnl, sRow, sVal;
	TCHAR sep[] = { _T(",/;\r\n\t") };
	TCHAR szData[MAX_PATH];

	int nNodeX = pDoc->m_Master[0].m_pPcsRgn->nCol;
	int nNodeY = pDoc->m_Master[0].m_pPcsRgn->nRow;
	int nTotPcs = nNodeX * nNodeY;
	int nStripPcs = nTotPcs / 4;

	int nDefStrip[4];
	

//  InitRst();
	ClrRst();
	m_nTotPcs = 0;
	m_nGoodPcs = 0;
	m_nBadPcs = 0;
	
	m_nTotalProgressReloadReelmap = nTo*(nNodeX*nNodeY+4)+4*MAX_DEF+MAX_DEF;
	m_nProgressReloadReelmap=0;

	for(nPnl=0; nPnl<nTo; nPnl++)
	{
		if (!m_bThreadAliveReloadReelmap)
		{
			AfxMessageBox(_T("Thread is stopped on reloading reelmap.- PnlBuf"));
			m_bDoneReloadReelmap = TRUE;
			return FALSE;
		}

		nDefStrip[0] = 0; nDefStrip[1] = 0; nDefStrip[2] = 0; nDefStrip[3] = 0;

		for(nRow=0; nRow<nNodeX; nRow++)
		{
			sPnl.Format(_T("%d"), nPnl+1);
			sRow.Format(_T("%02d"), nRow);
// 			sRow.Format(_T("%d"), nRow+1);
			if (0 < ::GetPrivateProfileString(sPnl, sRow, NULL, szData, sizeof(szData), m_sPathBuf))
			{
				for(nCol=0; nCol<nNodeY; nCol++)
				{
					m_nProgressReloadReelmap++;

					if(nCol==0)
						sVal = _tcstok(szData,sep);
					else
						sVal = _tcstok(NULL,sep);//strtok

					nDefCode = _tstoi(sVal);

					nR = (nNodeY-1)-nCol;
					nC = nRow;

					m_pPnlBuf[nPnl][nR][nC] = (short)nDefCode;
					m_nDef[nDefCode]++;

					nStrip = int(nR / (nNodeY/4));
					if(nStrip > -1 && nStrip < 4)
					{
						if(nDefCode > 0)
						{
							nDefStrip[nStrip]++;
							m_nDefStrip[nStrip]++;
							m_nDefPerStrip[nStrip][nDefCode]++;
						}
					}

					m_nTotPcs++;
					if(nDefCode > 0)
						m_nBadPcs++;
					else
						m_nGoodPcs++;
				}
			}
		}

		double dStOutRto = _tstof(pDoc->WorkingInfo.LastJob.sStripOutRatio) / 100.0; //atof
		for(nStrip=0; nStrip<4; nStrip++)
		{
			m_nProgressReloadReelmap++;

			if(nDefStrip[nStrip] >= nStripPcs * dStOutRto)
				m_nStripOut[nStrip]++;
		}
	}


	// ���� ����Ÿ�� ������.

	// Piece infomation..............
	CString strData, strMenu, strItem;
	int k, i;

	strData.Format(_T("%d"), m_nTotPcs);
	::WritePrivateProfileString(_T("Info"), _T("Total Pcs"), strData, m_sPathBuf);

	strData.Format(_T("%d"), m_nGoodPcs);
	::WritePrivateProfileString(_T("Info"), _T("Good Pcs"), strData, m_sPathBuf);

	strData.Format(_T("%d"), m_nBadPcs);
	::WritePrivateProfileString(_T("Info"), _T("Bad Pcs"), strData, m_sPathBuf);

	int nTotStOut = 0;
	for(k=0; k<4; k++)
	{
		strMenu.Format(_T("Strip%d"), k);
		strData.Format(_T("%d"), m_nDefStrip[k]);
		::WritePrivateProfileString(_T("Info"), strMenu, strData, m_sPathBuf);
		
		strMenu.Format(_T("%d"), k);
		strData.Format(_T("%d"), m_nStripOut[k]);
		::WritePrivateProfileString(_T("StripOut"), strMenu, strData, m_sPathBuf);
		nTotStOut += m_nStripOut[k];

		for(i=1; i<MAX_DEF; i++)
		{
			m_nProgressReloadReelmap++;

			strItem.Format(_T("Strip%d"), k);
			strMenu.Format(_T("%d"), i);
			strData.Format(_T("%d"), m_nDefPerStrip[k][i]);
			::WritePrivateProfileString(strItem, strMenu, strData, m_sPathBuf);
		}			
	}
	strData.Format(_T("%d"), m_nTotStOut=nTotStOut);
	::WritePrivateProfileString(_T("StripOut"), _T("Total"), strData, m_sPathBuf);

	for(i=1; i<MAX_DEF; i++)
	{
		m_nProgressReloadReelmap++;

		strMenu.Format(_T("%d"), i);
		strData.Format(_T("%d"), m_nDef[i]); // �ҷ��̸��� �ҷ���
		::WritePrivateProfileString(_T("Info"), strMenu, strData, m_sPathBuf);
	}

	m_bDoneReloadReelmap = TRUE;
	return TRUE;
}


char* CReelMap::StrToChar(CString str) // char* returned must be deleted... 
{
	char*		szStr = NULL;
	wchar_t*	wszStr;
	int				nLenth;

	USES_CONVERSION;
	//1. CString to wchar_t* conversion
	wszStr = T2W(str.GetBuffer(str.GetLength()));

	//2. wchar_t* to char* conversion
	nLenth = WideCharToMultiByte(CP_ACP, 0, wszStr, -1, NULL, 0, NULL, NULL); //char* ���� ���ѱ��̸� ���� 
	szStr = new char[nLenth];  //�޸� �Ҵ� 

							   //3. wchar_t* to char* conversion
	WideCharToMultiByte(CP_ACP, 0, wszStr, -1, szStr, nLenth, 0, 0);
	return szStr;
}

void CReelMap::StrToChar(CString str, char* pCh) // char* returned must be deleted... 
{
	wchar_t*	wszStr;
	int				nLenth;

	USES_CONVERSION;
	//1. CString to wchar_t* conversion
	wszStr = T2W(str.GetBuffer(str.GetLength()));

	//2. wchar_t* to char* conversion
	nLenth = WideCharToMultiByte(CP_ACP, 0, wszStr, -1, NULL, 0, NULL, NULL); //char* ���� ���ѱ��̸� ���� 

																			  //3. wchar_t* to char* conversion
	WideCharToMultiByte(CP_ACP, 0, wszStr, -1, pCh, nLenth, 0, 0);
	return;
}

BOOL CReelMap::MakeDirYield(CString sPath)
{
	CString str, Path[5];

	switch (m_nLayer)
	{
	case RMAP_UP:
		str = _T("YieldUp.txt");
		Path[0] = pDoc->WorkingInfo.System.sPathOldFile;
		Path[1] = pDoc->WorkingInfo.LastJob.sModelUp;
		Path[2] = pDoc->WorkingInfo.LastJob.sLotUp;
		Path[3] = pDoc->WorkingInfo.LastJob.sLayerUp;
		Path[4] = _T("");
		break;
	case RMAP_ALLUP:
		str = _T("YieldAll.txt");
		Path[0] = pDoc->WorkingInfo.System.sPathOldFile;
		Path[1] = pDoc->WorkingInfo.LastJob.sModelUp;
		Path[2] = pDoc->WorkingInfo.LastJob.sLotUp;
		Path[3] = pDoc->WorkingInfo.LastJob.sLayerUp;
		Path[4] = _T("");
		break;
	case RMAP_DN:
		str = _T("YieldDn.txt");
		Path[0] = pDoc->WorkingInfo.System.sPathOldFile;
		Path[1] = pDoc->WorkingInfo.LastJob.sModelUp;
		Path[2] = pDoc->WorkingInfo.LastJob.sLotUp;
		Path[3] = pDoc->WorkingInfo.LastJob.sLayerDn;
		Path[4] = _T("");
		break;
	case RMAP_ALLDN:
		str = _T("YieldAll.txt");
		Path[0] = pDoc->WorkingInfo.System.sPathOldFile;
		Path[1] = pDoc->WorkingInfo.LastJob.sModelUp;
		Path[2] = pDoc->WorkingInfo.LastJob.sLotUp;
		Path[3] = pDoc->WorkingInfo.LastJob.sLayerDn;
		Path[4] = _T("");
		break;
	case RMAP_INNER_UP:
		str = _T("YieldUp.txt");
		Path[0] = pDoc->WorkingInfo.System.sPathItsFile;
		Path[1] = pDoc->WorkingInfo.LastJob.sModelUp;
		Path[2] = pDoc->WorkingInfo.LastJob.sEngItsCode;
		Path[3] = pDoc->WorkingInfo.LastJob.sInnerLotUp;
		Path[4] = pDoc->WorkingInfo.LastJob.sInnerLayerUp;
		break;
	case RMAP_INNER_DN:
		str = _T("YieldDn.txt");
		Path[0] = pDoc->WorkingInfo.System.sPathItsFile;
		Path[1] = pDoc->WorkingInfo.LastJob.sModelUp;
		Path[2] = pDoc->WorkingInfo.LastJob.sEngItsCode;
		Path[3] = pDoc->WorkingInfo.LastJob.sInnerLotUp;
		Path[4] = pDoc->WorkingInfo.LastJob.sInnerLayerDn;
		break;
	case RMAP_INNER_ALLUP:
		str = _T("YieldAll.txt");
		Path[0] = pDoc->WorkingInfo.System.sPathItsFile;
		Path[1] = pDoc->WorkingInfo.LastJob.sModelUp;
		Path[2] = pDoc->WorkingInfo.LastJob.sEngItsCode;
		Path[3] = pDoc->WorkingInfo.LastJob.sInnerLotUp;
		Path[4] = pDoc->WorkingInfo.LastJob.sInnerLayerUp;
		break;
	case RMAP_INNER_ALLDN:
		str = _T("YieldAll.txt");
		Path[0] = pDoc->WorkingInfo.System.sPathItsFile;
		Path[1] = pDoc->WorkingInfo.LastJob.sModelUp;
		Path[2] = pDoc->WorkingInfo.LastJob.sEngItsCode;
		Path[3] = pDoc->WorkingInfo.LastJob.sInnerLotUp;
		Path[4] = pDoc->WorkingInfo.LastJob.sInnerLayerDn;
		break;
	case RMAP_INOUTER_UP:
		break;
	case RMAP_INOUTER_DN:
		break;
	case RMAP_ITS:
		str = _T("YieldIts.txt");
		Path[0] = pDoc->WorkingInfo.System.sPathItsFile;
		Path[1] = pDoc->WorkingInfo.LastJob.sModelUp;
		Path[2] = pDoc->WorkingInfo.LastJob.sEngItsCode;
		Path[3] = _T("");
		Path[4] = _T("");
		break;
	}

	if (!Path[0].IsEmpty() && !Path[1].IsEmpty())
	{
		sPath.Format(_T("%s%s"), Path[0], Path[1]);
		if (!pDoc->DirectoryExists(sPath))
			CreateDirectory(sPath, NULL);

		if (!Path[2].IsEmpty())
		{
			sPath.Format(_T("%s%s\\%s"), Path[0], Path[1], Path[2]);
			if (!pDoc->DirectoryExists(sPath))
				CreateDirectory(sPath, NULL);

			if (!Path[3].IsEmpty())
			{
				sPath.Format(_T("%s%s\\%s\\%s"), Path[0], Path[1], Path[2], Path[3]);
				if (!pDoc->DirectoryExists(sPath))
					CreateDirectory(sPath, NULL);

				if (!Path[4].IsEmpty())
				{
					sPath.Format(_T("%s%s\\%s\\%s\\%s"), Path[0], Path[1], Path[2], Path[3], Path[4]);
					if (!pDoc->DirectoryExists(sPath))
						CreateDirectory(sPath, NULL);
				}
			}
		}
	}

	return TRUE;
}

//BOOL CReelMap::ReadYield(int nSerial, CString sPath)
//{
//	int dwStart = GetTickCount();
//
//	int i, k;
//	TCHAR szData[200];
//
//	CString sCode, strMenu, strItem, sMsg;
//
//	//std::string strSection, strKey, strVal;
//	//CIniFile iniFile(sPath);
//	std::string strSection, strKey, strVal, strFullPath;
//	strFullPath = CT2CA(sPath);
//	CIniFile iniFile(strFullPath);
//
//	strMenu.Format(_T("%d"), nSerial);
//	strSection = CT2CA(strMenu);
//
//	for (i = 1; i < MAX_DEF; i++)
//	{
//		sCode.Format(_T("%d"), i);
//		strKey = CT2CA(sCode);
//		strVal = iniFile.GetValue(strKey, strSection);
//		if (strVal == "")
//		{
//			if (nSerial == 1)
//			{
//				m_stYield.nDefA[i] = 0;
//			}
//			else
//			{
//				//sMsg.Format(_T("���� ���� �б� ���� : Shot(%d)-�ҷ�(%d)\r\n%s"), nSerial, i, sPath);
//				//AfxMessageBox(sMsg, MB_ICONWARNING | MB_OK);
//				return FALSE;
//			}
//		}
//		else
//		{
//			m_stYield.nDefA[i] = stoi(strVal);
//		}
//	}
//
//	strKey = "Total Pcs";
//	strVal = iniFile.GetValue(strKey, strSection);
//	if (strVal == "")
//	{
//		if (nSerial == 1)
//		{
//			m_stYield.nTot = 0;
//		}
//		else
//		{
//			sMsg.Format(_T("���� ���� �б� ���� : Shot(%d)-Total Pcs\r\n%s"), nSerial, sPath);
//			AfxMessageBox(sMsg, MB_ICONWARNING | MB_OK);
//			return FALSE;
//		}
//	}
//	else
//	{
//		m_stYield.nTot = stoi(strVal);
//	}
//
//	strKey = "Good Pcs";
//	strVal = iniFile.GetValue(strKey, strSection);
//	if (strVal == "")
//	{
//		if (nSerial == 1)
//		{
//			m_stYield.nGood = 0;
//		}
//		else
//		{
//			sMsg.Format(_T("���� ���� �б� ���� : Shot(%d)-Good Pcs\r\n%s"), nSerial, sPath);
//			AfxMessageBox(sMsg, MB_ICONWARNING | MB_OK);
//			return FALSE;
//		}
//	}
//	else
//	{
//		m_stYield.nGood = stoi(strVal);
//	}
//
//	strKey = "Bad Pcs";
//	strVal = iniFile.GetValue(strKey, strSection);
//	if (strVal == "")
//	{
//		if (nSerial == 1)
//		{
//			m_stYield.nDef = 0;
//		}
//		else
//		{
//			sMsg.Format(_T("���� ���� �б� ���� : Shot(%d)-Bad Pcs\r\n%s"), nSerial, sPath);
//			AfxMessageBox(sMsg, MB_ICONWARNING | MB_OK);
//			return FALSE;
//		}
//	}
//	else
//	{
//		m_stYield.nDef = stoi(strVal);
//	}
//
//	for (k = 0; k < MAX_STRIP; k++)
//	{
//		strItem.Format(_T("Strip%d"), k);
//		strKey = CT2CA(strItem);
//		strVal = iniFile.GetValue(strKey, strSection);
//		if (strVal == "")
//		{
//			if (nSerial == 1)
//			{
//				m_stYield.nDefStrip[k] = 0;
//			}
//			else
//			{
//				sMsg.Format(_T("���� ���� �б� ���� : Shot(%d)-Strip%d\r\n%s"), nSerial, k, sPath);
//				AfxMessageBox(sMsg, MB_ICONWARNING | MB_OK);
//				return FALSE;
//			}
//		}
//		else
//		{
//			m_stYield.nDefStrip[k] = stoi(strVal);
//		}
//
//		strItem.Format(_T("StripOut_%d"), k);
//		strKey = CT2CA(strItem);
//		strVal = iniFile.GetValue(strKey, strSection);
//		if (strVal == "")
//		{
//			if (nSerial == 1)
//			{
//				m_stYield.nStripOut[k] = 0;
//			}
//			else
//			{
//				sMsg.Format(_T("���� ���� �б� ���� : Shot(%d)-StripOut_%d\r\n%s"), nSerial, k, sPath);
//				AfxMessageBox(sMsg, MB_ICONWARNING | MB_OK);
//				return FALSE;
//			}
//		}
//		else
//		{
//			m_stYield.nStripOut[k] = stoi(strVal);
//		}
//
//		for (i = 1; i < MAX_DEF; i++)
//		{
//			strItem.Format(_T("Strip%d_%d"), k, i);
//			strKey = CT2CA(strItem);
//			strVal = iniFile.GetValue(strKey, strSection);
//			if (strVal == "")
//			{
//				if (nSerial == 1)
//				{
//					m_stYield.nDefPerStrip[k][i] = 0;
//				}
//				else
//				{
//					sMsg.Format(_T("���� ���� �б� ���� : Shot(%d)-Strip%d_%d\r\n%s"), nSerial, k, i, sPath);
//					AfxMessageBox(sMsg, MB_ICONWARNING | MB_OK);
//					return FALSE;
//				}
//			}
//			else
//			{
//				m_stYield.nDefPerStrip[k][i] = stoi(strVal);
//			}
//		}
//	}
//
//	strKey = "StripOut_Total";
//	strVal = iniFile.GetValue(strKey, strSection);
//	if (strVal == "")
//	{
//		if (nSerial == 1)
//		{
//			m_stYield.nTotSriptOut = 0;
//		}
//		else
//		{
//			sMsg.Format(_T("���� ���� �б� ���� : Shot(%d)-StripOut_Total\r\n%s"), nSerial, sPath);
//			AfxMessageBox(sMsg, MB_ICONWARNING | MB_OK);
//			return FALSE;
//		}
//	}
//	else
//	{
//		m_stYield.nTotSriptOut = stoi(strVal);
//	}
//
//	int dwEnd = GetTickCount();
//	int dwElapsed = dwEnd - dwStart;
//
//	return TRUE;
//}

//BOOL CReelMap::WriteYield(int nSerial, CString sPath)
//{
//	int dwStart = GetTickCount();
//
//	int nNodeX = 0, nNodeY = 0;
//#ifndef TEST_MODE
//	nNodeX = pDoc->m_Master[0].m_pPcsRgn->nCol;
//	nNodeY = pDoc->m_Master[0].m_pPcsRgn->nRow;
//#endif
//	CString sDefNum, strData;
//	int nPnl, nRow, nCol, nDefCode, nStrip;
//	int nTotPcs = nNodeX * nNodeY;
//	int nStripPcs = nTotPcs / 4;
//	double dStOutRto = _tstof(pDoc->WorkingInfo.LastJob.sStripOutRatio) / 100.0;
//	nPnl = nSerial - 1;
//
//	int i, k;
//	TCHAR szData[200];
//
//	CString sCode, strMenu, strItem, sMsg;
//	int nTot = 0, nGood = 0, nDef = 0;
//	int nTotSriptOut = 0;
//
//	int nDefStrip[MAX_STRIP], nDefA[MAX_DEF], nDefPerStrip[4][MAX_DEF], nStripOut[4];
//	nDefStrip[0] = 0; nDefStrip[1] = 0; nDefStrip[2] = 0; nDefStrip[3] = 0;
//	nStripOut[0] = 0; nStripOut[1] = 0; nStripOut[2] = 0; nStripOut[3] = 0;
//
//	nTot = nNodeX * nNodeY;
//
//	for (k = 0; k < MAX_DEF; k++)
//	{
//		nDefA[k] = 0;
//
//		for (i = 0; i < MAX_STRIP; i++)
//		{
//			nDefPerStrip[i][k] = 0;
//		}
//	}
//
//	for (nRow = 0; nRow < nNodeY; nRow++)
//	{
//		for (nCol = 0; nCol < nNodeX; nCol++)
//		{
//			if (m_pPnlBuf)
//			{
//				nDefCode = (int)m_pPnlBuf[nPnl][nRow][nCol] < 0 ? 0 : (int)m_pPnlBuf[nPnl][nRow][nCol];
//				nDefA[nDefCode]++;
//
//				nStrip = int(nRow / (nNodeY / 4));
//				if (nStrip > -1 && nStrip < 4)
//				{
//					if (nDefCode > 0)
//					{
//						nDefStrip[nStrip]++;
//						nDefPerStrip[nStrip][nDefCode]++;
//					}
//				}
//			}
//		}
//	}
//
//	for (nStrip = 0; nStrip < 4; nStrip++)
//	{
//		if (nDefStrip[nStrip] >= nStripPcs * dStOutRto)
//			nStripOut[nStrip]++;
//	}
//
//	for (i = 1; i < MAX_DEF; i++)
//	{
//		m_stYield.nDefA[i] = m_stYield.nDefA[i] + nDefA[i];
//		nDef += nDefA[i];
//	}
//	nGood = nTot - nDef;
//
//	m_stYield.nTot = m_stYield.nTot + nTot;
//	m_stYield.nGood = m_stYield.nGood + nGood;
//	m_stYield.nDef = m_stYield.nDef + nDef;
//
//	for (k = 0; k < MAX_STRIP; k++)
//	{
//		m_stYield.nDefStrip[k] = m_stYield.nDefStrip[k] + nDefStrip[k];
//		m_stYield.nStripOut[k] = m_stYield.nStripOut[k] + nStripOut[k];
//		nTotSriptOut += nStripOut[k];
//		for (i = 1; i < MAX_DEF; i++)
//			m_stYield.nDefPerStrip[k][i] = m_stYield.nDefPerStrip[k][i] + nDefPerStrip[k][i];
//	}
//	m_stYield.nTotSriptOut = m_stYield.nTotSriptOut + nTotSriptOut;
//
//
//	FILE *fp = NULL;
//	char FileName[MAX_PATH];
//
//	BOOL bExist = FALSE;
//	CFileFind findfile;
//	if (findfile.FindFile(sPath))
//		bExist = TRUE;
//	else
//		MakeDirYield(sPath);
//
//	//_tcscpy(FileName, sPath);
//	StrToChar(sPath, FileName);
//
//	fp = fopen(FileName, "a+");
//	if (fp == NULL)
//	{
//		pView->MsgBox(_T("It is trouble to open ReelMap.txt"));
//		//AfxMessageBox(_T("It is trouble to open ReelMap.txt"),MB_ICONWARNING|MB_OK);
//		return FALSE;
//	}
//
//	if (!bExist)
//	{
//		m_nStartSerial = nSerial;
//
//		fprintf(fp, "[Info]\n");
//		fprintf(fp, "Total Shot = \n\n");
//		fprintf(fp, "Total Pcs = \n");
//		fprintf(fp, "Good Pcs = \n");
//		fprintf(fp, "Bad Pcs = \n\n");
//		fprintf(fp, "Start Shot=%d\n", m_nStartSerial);
//		fprintf(fp, "End Shot = \n\n");
//
//		for (i = 1; i <= MAX_DEF; i++)
//			fprintf(fp, "%d=\n", i);
//		fprintf(fp, "\n");
//
//		for (k = 0; k < MAX_STRIP; k++)
//			fprintf(fp, "Strip%d = \n", k);
//		fprintf(fp, "\n");
//
//		for (k = 0; k < MAX_STRIP; k++)
//		{
//			for (i = 1; i <= MAX_DEF; i++)
//				fprintf(fp, "Strip%d_%d = \n", k, i);
//			fprintf(fp, "\n");
//		}
//
//		fprintf(fp, "StripOut_Total = \n");
//		fprintf(fp, "StripOut_0 = \n");
//		fprintf(fp, "StripOut_1 = \n");
//		fprintf(fp, "StripOut_2 = \n");
//		fprintf(fp, "StripOut_3 = \n");
//		fprintf(fp, "\n");
//	}
//
//	fclose(fp);
//
//	// [Serial]
//
//	//std::string strSection, strKey, strVal;
//	//CIniFile iniFile(sPath);
//	std::string strSection, strKey, strVal, strFullPath;
//	strFullPath = CT2CA(sPath);
//	CIniFile iniFile(strFullPath);
//
//	strMenu.Format(_T("%d"), nSerial);
//	strSection = CT2CA(strMenu);
//
//	for (i = 1; i < MAX_DEF; i++)
//	{
//		sCode.Format(_T("%d"), i);
//		strKey = CT2CA(sCode);
//		sDefNum.Format(_T("%d"), m_stYield.nDefA[i]);
//		strVal = CT2CA(sDefNum);
//
//		// [Info]
//		iniFile.SetValue(strKey, strVal, "Info");
//
//		// [Serial]
//		iniFile.SetValue(strKey, strVal, strSection);
//	}
//
//	// [Info]
//	strData.Format(_T("%d"), nSerial);
//	strVal = CT2CA(strData);
//	iniFile.SetValue("End Shot", strVal, "Info");
//
//	strData.Format(_T("%d"), nSerial - m_nStartSerial + 1);
//	strVal = CT2CA(strData);
//	iniFile.SetValue("Total Shot", strVal, "Info");
//
//	strData.Format(_T("%d"), m_stYield.nTot);
//	strVal = CT2CA(strData);
//	iniFile.SetValue("Total Pcs", strVal, "Info");
//
//	strData.Format(_T("%d"), m_stYield.nGood);
//	strVal = CT2CA(strData);
//	iniFile.SetValue("Good Pcs", strVal, "Info");
//
//	strData.Format(_T("%d"), m_stYield.nDef);
//	strVal = CT2CA(strData);
//	iniFile.SetValue("Bad Pcs", strVal, "Info");
//
//	// [Serial]
//	strData.Format(_T("%d"), m_stYield.nTot);
//	strVal = CT2CA(strData);
//	iniFile.SetValue("Total Pcs", strVal, strSection);
//
//	strData.Format(_T("%d"), m_stYield.nGood);
//	strVal = CT2CA(strData);
//	iniFile.SetValue("Good Pcs", strVal, strSection);
//
//	strData.Format(_T("%d"), m_stYield.nDef);
//	strVal = CT2CA(strData);
//	iniFile.SetValue("Bad Pcs", strVal, strSection);
//
//	for (k = 0; k < MAX_STRIP; k++)
//	{
//		strItem.Format(_T("Strip%d"), k);
//		strKey = CT2CA(strItem);
//		strData.Format(_T("%d"), m_stYield.nDefStrip[k]);
//		strVal = CT2CA(strData);
//		// [Info]
//		iniFile.SetValue(strKey, strVal, "Info");
//
//		// [Serial]
//		iniFile.SetValue(strKey, strVal, strSection);
//
//		strItem.Format(_T("StripOut_%d"), k);
//		strKey = CT2CA(strItem);
//		strData.Format(_T("%d"), m_stYield.nStripOut[k]);
//		strVal = CT2CA(strData);
//		// [Info]
//		iniFile.SetValue(strKey, strVal, "Info");
//
//		// [Serial]
//		iniFile.SetValue(strKey, strVal, strSection);
//
//		for (i = 1; i < MAX_DEF; i++)
//		{
//			strItem.Format(_T("Strip%d_%d"), k, i);
//			strKey = CT2CA(strItem);
//			strData.Format(_T("%d"), m_stYield.nDefPerStrip[k][i]);
//			strVal = CT2CA(strData);
//			// [Info]
//			iniFile.SetValue(strKey, strVal, "Info");
//
//			// [Serial]
//			iniFile.SetValue(strKey, strVal, strSection);
//		}
//	}
//
//	strData.Format(_T("%d"), m_stYield.nTotSriptOut);
//	strVal = CT2CA(strData);
//	// [Info]
//	iniFile.SetValue("StripOut_Total", strVal, "Info");
//
//	// [Serial]
//	iniFile.SetValue("StripOut_Total", strVal, strSection);
//
//	int dwEnd = GetTickCount();
//	int dwElapsed = dwEnd - dwStart;
//
//	return TRUE;
//}

BOOL CReelMap::ReadYield(int nSerial, CString sPath)
{
	int dwStart = GetTickCount();

	int i, k;
	TCHAR szData[200];

	CString sCode, strMenu, strItem, sMsg;

	strMenu.Format(_T("%d"), nSerial);
	for (i = 1; i < MAX_DEF; i++)
	{
		sCode.Format(_T("%d"), i);
		if (0 < ::GetPrivateProfileString(strMenu, sCode, NULL, szData, sizeof(szData), sPath))
		{
			m_stYield.nDefA[i] = _ttoi(szData);
		}
		else
		{
			if (nSerial == 1)
			{
				m_stYield.nDefA[i] = 0;
			}
			else
			{
				//sMsg.Format(_T("���� ���� �б� ���� : Shot(%d)-�ҷ�(%d)\r\n%s"), nSerial, i, sPath);
				//AfxMessageBox(sMsg, MB_ICONWARNING | MB_OK);
				return FALSE;
			}
		}
	}

	if (0 < ::GetPrivateProfileString(strMenu, _T("Total Pcs"), NULL, szData, sizeof(szData), sPath))
	{
		m_stYield.nTot = _ttoi(szData);
	}
	else
	{
		if (nSerial == 1)
		{
			m_stYield.nTot = 0;
		}
		else
		{
			sMsg.Format(_T("���� ���� �б� ���� : Shot(%d)-Total Pcs\r\n%s"), nSerial, sPath);
			AfxMessageBox(sMsg, MB_ICONWARNING | MB_OK);
			return FALSE;
		}
	}

	if (0 < ::GetPrivateProfileString(strMenu, _T("Good Pcs"), NULL, szData, sizeof(szData), sPath))
	{
		m_stYield.nGood = _ttoi(szData);
	}
	else
	{
		if (nSerial == 1)
		{
			m_stYield.nGood = 0;
		}
		else
		{
			sMsg.Format(_T("���� ���� �б� ���� : Shot(%d)-Good Pcs\r\n%s"), nSerial, sPath);
			AfxMessageBox(sMsg, MB_ICONWARNING | MB_OK);
			return FALSE;
		}
	}

	if (0 < ::GetPrivateProfileString(strMenu, _T("Bad Pcs"), NULL, szData, sizeof(szData), sPath))
	{
		m_stYield.nDef = _ttoi(szData);
	}
	else
	{
		if (nSerial == 1)
		{
			m_stYield.nDef = 0;
		}
		else
		{
			sMsg.Format(_T("���� ���� �б� ���� : Shot(%d)-Bad Pcs\r\n%s"), nSerial, sPath);
			AfxMessageBox(sMsg, MB_ICONWARNING | MB_OK);
			return FALSE;
		}
	}

	for (k = 0; k < MAX_STRIP; k++)
	{
		strItem.Format(_T("Strip%d"), k);
		if (0 < ::GetPrivateProfileString(strMenu, strItem, NULL, szData, sizeof(szData), sPath))
		{
			m_stYield.nDefStrip[k] = _ttoi(szData);
		}
		else
		{
			if (nSerial == 1)
			{
				m_stYield.nDefStrip[k] = 0;
			}
			else
			{
				sMsg.Format(_T("���� ���� �б� ���� : Shot(%d)-Strip%d\r\n%s"), nSerial, k, sPath);
				AfxMessageBox(sMsg, MB_ICONWARNING | MB_OK);
				return FALSE;
			}
		}

		strItem.Format(_T("StripOut_%d"), k);
		if (0 < ::GetPrivateProfileString(strMenu, strItem, NULL, szData, sizeof(szData), sPath))
		{
			m_stYield.nStripOut[k] = _ttoi(szData);
		}
		else
		{
			if (nSerial == 1)
			{
				m_stYield.nStripOut[k] = 0;
			}
			else
			{
				sMsg.Format(_T("���� ���� �б� ���� : Shot(%d)-StripOut_%d\r\n%s"), nSerial, k, sPath);
				AfxMessageBox(sMsg, MB_ICONWARNING | MB_OK);
				return FALSE;
			}
		}

		for (i = 1; i < MAX_DEF; i++)
		{
			strItem.Format(_T("Strip%d_%d"), k, i);
			if (0 < ::GetPrivateProfileString(strMenu, strItem, NULL, szData, sizeof(szData), sPath))
			{
				m_stYield.nDefPerStrip[k][i] = _ttoi(szData);
			}
			else
			{
				if (nSerial == 1)
				{
					m_stYield.nDefPerStrip[k][i] = 0;
				}
				else
				{
					sMsg.Format(_T("���� ���� �б� ���� : Shot(%d)-Strip%d_%d\r\n%s"), nSerial, k, i, sPath);
					AfxMessageBox(sMsg, MB_ICONWARNING | MB_OK);
					return FALSE;
				}
			}
		}
	}

	if (0 < ::GetPrivateProfileString(strMenu, _T("StripOut_Total"), NULL, szData, sizeof(szData), sPath))
	{
		m_stYield.nTotSriptOut = _ttoi(szData);
	}
	else
	{
		if (nSerial == 1)
		{
			m_stYield.nTotSriptOut = 0;
		}
		else
		{
			sMsg.Format(_T("���� ���� �б� ���� : Shot(%d)-StripOut_Total\r\n%s"), nSerial, sPath);
			AfxMessageBox(sMsg, MB_ICONWARNING | MB_OK);
			return FALSE;
		}
	}
	
	int dwEnd = GetTickCount();
	int dwElapsed = dwEnd - dwStart;

	return TRUE;
}

BOOL CReelMap::WriteYield(int nSerial, CString sPath)
{
	m_nBeforeSerial = nSerial;

	int dwStart = GetTickCount();
	int nNodeX = 0, nNodeY = 0;
#ifdef USE_CAM_MASTER
	nNodeX = pDoc->m_Master[0].m_pPcsRgn->nCol;
	nNodeY = pDoc->m_Master[0].m_pPcsRgn->nRow;
#endif
	CString sDefNum, strData;
	int nPnl, nRow, nCol, nDefCode, nStrip;
	int nTotPcs = nNodeX * nNodeY;
	int nStripPcs = nTotPcs / 4;
	double dStOutRto = _tstof(pDoc->WorkingInfo.LastJob.sStripOutRatio) / 100.0;
	nPnl = nSerial - 1;

	int i, k;
	TCHAR szData[200];

	CString sCode, strMenu, strItem, sMsg;
	int nTot = 0, nGood = 0, nDef = 0;
	int nTotSriptOut = 0;

	int nDefStrip[MAX_STRIP], nDefA[MAX_DEF], nDefPerStrip[MAX_STRIP][MAX_DEF], nStripOut[MAX_STRIP];
	nDefStrip[0] = 0; nDefStrip[1] = 0; nDefStrip[2] = 0; nDefStrip[3] = 0;
	nStripOut[0] = 0; nStripOut[1] = 0; nStripOut[2] = 0; nStripOut[3] = 0;

	nTot = nNodeX * nNodeY;

	for (k = 0; k < MAX_DEF; k++)
	{
		nDefA[k] = 0;

		for (i = 0; i < MAX_STRIP; i++)
		{
			nDefPerStrip[i][k] = 0;
		}
	}

	for (nRow = 0; nRow < nNodeY; nRow++)
	{
		for (nCol = 0; nCol < nNodeX; nCol++)
		{
			if (m_pPnlBuf)
			{
				nDefCode = (int)m_pPnlBuf[nPnl][nRow][nCol] < 0 ? 0 : (int)m_pPnlBuf[nPnl][nRow][nCol];
				nDefA[nDefCode]++;

				nStrip = int(nRow / (nNodeY / MAX_STRIP));
				if (nStrip > -1 && nStrip < MAX_STRIP)
				{
					if (nDefCode > 0)
					{
						nDefStrip[nStrip]++;
						nDefPerStrip[nStrip][nDefCode]++;
					}
				}
			}
		}
	}

	for (nStrip = 0; nStrip < MAX_STRIP; nStrip++)
	{
		if (nDefStrip[nStrip] >= nStripPcs * dStOutRto)
			nStripOut[nStrip]++;
	}

	for (i = 1; i < MAX_DEF; i++)
	{
		m_stYield.nDefA[i] = m_stYield.nDefA[i] + nDefA[i];
		nDef += nDefA[i];
	}
	nGood = nTot - nDef;

	m_stYield.nTot = m_stYield.nTot + nTot;
	m_stYield.nGood = m_stYield.nGood + nGood;
	m_stYield.nDef = m_stYield.nDef + nDef;

	for (k = 0; k < MAX_STRIP; k++)
	{
		m_stYield.nDefStrip[k] = m_stYield.nDefStrip[k] + nDefStrip[k];
		m_stYield.nStripOut[k] = m_stYield.nStripOut[k] + nStripOut[k];
		nTotSriptOut += nStripOut[k];
		for (i = 1; i < MAX_DEF; i++)
			m_stYield.nDefPerStrip[k][i] = m_stYield.nDefPerStrip[k][i] + nDefPerStrip[k][i];
	}
	m_stYield.nTotSriptOut = m_stYield.nTotSriptOut + nTotSriptOut;


	FILE *fp = NULL;
	char FileName[MAX_PATH];

	BOOL bExist = FALSE;
	CFileFind findfile;
	if (findfile.FindFile(sPath))
		bExist = TRUE;
	else
		MakeDirYield(sPath);

	//_tcscpy(FileName, sPath);
	StrToChar(sPath, FileName);

	fp = fopen(FileName, "a+");
	if (fp == NULL)
	{
		pView->MsgBox(_T("It is trouble to open ReelMap.txt"));
		//AfxMessageBox(_T("It is trouble to open ReelMap.txt"),MB_ICONWARNING|MB_OK);
		return FALSE;
	}

	if (!bExist)
	{
		m_nStartSerial = nSerial;

		fprintf(fp, "[Info]\n");
		fprintf(fp, "Total Shot = \n\n");
		fprintf(fp, "Total Pcs = \n");
		fprintf(fp, "Good Pcs = \n");
		fprintf(fp, "Bad Pcs = \n\n");
		fprintf(fp, "Start Shot=%d\n", m_nStartSerial);
		fprintf(fp, "End Shot = \n\n");

		for (i = 1; i <= MAX_DEF; i++)
			fprintf(fp, "%d=\n", i);
		fprintf(fp, "\n");

		for (k = 0; k < MAX_STRIP; k++)
			fprintf(fp, "Strip%d = \n", k);
		fprintf(fp, "\n");

		for (k = 0; k < MAX_STRIP; k++)
		{
			for (i = 1; i <= MAX_DEF; i++)
				fprintf(fp, "Strip%d_%d = \n", k, i);
			fprintf(fp, "\n");
		}

		fprintf(fp, "StripOut_Total = \n");
		fprintf(fp, "StripOut_0 = \n");
		fprintf(fp, "StripOut_1 = \n");
		fprintf(fp, "StripOut_2 = \n");
		fprintf(fp, "StripOut_3 = \n");
		fprintf(fp, "\n");
	}
	
	// [Serial]

	//fprintf(fp, "[%d]\n", nSerial);
	//fprintf(fp, "Total Pcs = %d\n", m_stYield.nTot);
	//fprintf(fp, "Good Pcs = %d\n", m_stYield.nGood);
	//fprintf(fp, "Bad Pcs = %d\n\n", m_stYield.nDef);
	//
	//for (i = 1; i < MAX_DEF; i++)
	//{
	//	fprintf(fp, "%d = %d\n", i, m_stYield.nDefA[i]);
	//}
	//fprintf(fp, "\n");

	//fprintf(fp, "StripOut_Total = %d\n", m_stYield.nTotSriptOut);
	//for (k = 0; k < MAX_STRIP; k++)
	//	fprintf(fp, "StripOut_%d = %d\n", k, m_stYield.nStripOut[k]);
	//fprintf(fp, "\n");

	//for (k = 0; k < MAX_STRIP; k++)
	//{
	//	fprintf(fp, "Strip%d = %d\n", k, m_stYield.nDefStrip[k]);
	//	for (i = 1; i < MAX_DEF; i++)
	//		fprintf(fp, "Strip%d_%d = %d\n", k, i, m_stYield.nDefPerStrip[k][i]);
	//	fprintf(fp, "\n");
	//}

	fclose(fp);

	strMenu.Format(_T("%d"), nSerial);

	for (i = 1; i < MAX_DEF; i++)
	{
		sCode.Format(_T("%d"), i);
		sDefNum.Format(_T("%d"), m_stYield.nDefA[i]);

		// [Info]
		::WritePrivateProfileString(_T("Info"), sCode, sDefNum, sPath);

		// [Serial]
		::WritePrivateProfileString(strMenu, sCode, sDefNum, sPath);
	}

	// [Info]
	strData.Format(_T("%d"), nSerial);
	::WritePrivateProfileString(_T("Info"), _T("End Shot"), strData, sPath);

	strData.Format(_T("%d"), nSerial - m_nStartSerial + 1);
	::WritePrivateProfileString(_T("Info"), _T("Total Shot"), strData, sPath);

	strData.Format(_T("%d"), m_stYield.nTot);
	::WritePrivateProfileString(_T("Info"), _T("Total Pcs"), strData, sPath);

	strData.Format(_T("%d"), m_stYield.nGood);
	::WritePrivateProfileString(_T("Info"), _T("Good Pcs"), strData, sPath);

	strData.Format(_T("%d"), m_stYield.nDef);
	::WritePrivateProfileString(_T("Info"), _T("Bad Pcs"), strData, sPath);

	// [Serial]
	strData.Format(_T("%d"), m_stYield.nTot);
	::WritePrivateProfileString(strMenu, _T("Total Pcs"), strData, sPath);

	strData.Format(_T("%d"), m_stYield.nGood);
	::WritePrivateProfileString(strMenu, _T("Good Pcs"), strData, sPath);

	strData.Format(_T("%d"), m_stYield.nDef);
	::WritePrivateProfileString(strMenu, _T("Bad Pcs"), strData, sPath);

	for (k = 0; k < MAX_STRIP; k++)
	{
		strItem.Format(_T("Strip%d"), k);
		strData.Format(_T("%d"), m_stYield.nDefStrip[k]);
		// [Info]
		::WritePrivateProfileString(_T("Info"), strItem, strData, sPath);
		// [Serial]
		::WritePrivateProfileString(strMenu, strItem, strData, sPath);

		strItem.Format(_T("StripOut_%d"), k);
		strData.Format(_T("%d"), m_stYield.nStripOut[k]);
		// [Info]
		::WritePrivateProfileString(_T("Info"), strItem, strData, sPath);
		// [Serial]
		::WritePrivateProfileString(strMenu, strItem, strData, sPath);

		for (i = 1; i < MAX_DEF; i++)
		{
			strItem.Format(_T("Strip%d_%d"), k, i);
			strData.Format(_T("%d"), m_stYield.nDefPerStrip[k][i]);
			// [Info]
			::WritePrivateProfileString(_T("Info"), strItem, strData, sPath);
			// [Serial]
			::WritePrivateProfileString(strMenu, strItem, strData, sPath);
		}
	}

	strData.Format(_T("%d"), m_stYield.nTotSriptOut);
	// [Info]
	::WritePrivateProfileString(_T("Info"), _T("StripOut_Total"), strData, sPath);
	// [Serial]
	::WritePrivateProfileString(strMenu, _T("StripOut_Total"), strData, sPath);

	int dwEnd = GetTickCount();
	int dwElapsed = dwEnd - dwStart;

	return TRUE;
}

BOOL CReelMap::MakeHeader(CString sPath)
{
	FILE *fp = NULL;
	char FileName[MAX_PATH];
	BOOL bExist = FALSE;

	CFileFind findfile;
	if (findfile.FindFile(sPath))
	{
		bExist = TRUE;
		return TRUE;
	}

	//m_sPathShare = sPath;

	MakeDirRmap();
	StrToChar(sPath, FileName);

	fp = fopen(FileName, "w+");
	if (fp == NULL)
	{
		pView->MsgBox(_T("It is trouble to open ReelMap.txt"));
		return FALSE;
	}

	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;
	char* pRtn = NULL;
	int nNodeX = 0, nNodeY = 0, i = 0, k = 0;
#ifdef USE_CAM_MASTER
	nNodeX = pDoc->m_Master[0].m_pPcsRgn->nCol;
	nNodeY = pDoc->m_Master[0].m_pPcsRgn->nRow;
#endif

	//InitRst();
	//InitPcs();

	fprintf(fp, "[Info]\n");
	fprintf(fp, "��  ��  �� = %s\n", pRtn = StrToChar(pDoc->WorkingInfo.System.sMcName)); if (pRtn) delete pRtn; pRtn = NULL;
	fprintf(fp, "��  ��  �� = %s\n", pRtn = StrToChar(pDoc->WorkingInfo.LastJob.sSelUserName)); if (pRtn) delete pRtn; pRtn = NULL;
	fprintf(fp, "��      �� = %s\n", pRtn = StrToChar(pDoc->WorkingInfo.LastJob.sModelUp)); if (pRtn) delete pRtn; pRtn = NULL;
	fprintf(fp, "��      Ʈ = %s\n", pRtn = StrToChar(pDoc->WorkingInfo.LastJob.sLotUp)); if (pRtn) delete pRtn; pRtn = NULL;
	fprintf(fp, "��鷹�̾� = %s\n", pRtn = StrToChar(pDoc->WorkingInfo.LastJob.sLayerUp)); if (pRtn) delete pRtn; pRtn = NULL;
	if (bDualTest)
	{
		fprintf(fp, "�ϸ鷹�̾� = %s\n", pRtn = StrToChar(pDoc->WorkingInfo.LastJob.sLayerDn)); if (pRtn) delete pRtn; pRtn = NULL;
	}
	fprintf(fp, "\n");
	fprintf(fp, "Process Code = \n");
	fprintf(fp, "Entire Speed = \n");
	fprintf(fp, "\n");

	fprintf(fp, "CamPcsX = %d\n", nNodeX);
	fprintf(fp, "CamPcsY = %d\n", nNodeY);

	fprintf(fp, "\n");
	fprintf(fp, "0 -> ��ǰ\n");
	for (i = 1; i < MAX_DEF; i++)
	{
		fprintf(fp, "%d -> %s\n", i, pRtn = StrToChar(m_sKorDef[i])); if (pRtn) delete pRtn; pRtn = NULL; // m_cBigDef[i]
	}
	fprintf(fp, "? - Missed Align Panel(i.e �뱤�ҷ�)\n");
	fprintf(fp, "\n");
	fprintf(fp, "Lot Start=\n");
	fprintf(fp, "Lot Run=\n");
	fprintf(fp, "Lot End=\n");
	fprintf(fp, "\n");
	fprintf(fp, "Marked Shot=\n");
	fprintf(fp, "Marked Date=\n");
	fprintf(fp, "Completed Shot=\n");
	fprintf(fp, "\n");
	fprintf(fp, "\n");
	fprintf(fp, "// < ���� ���� > \n");
	fprintf(fp, "\n");
	fprintf(fp, "Start Serial=\n");
	fprintf(fp, "End Serial=\n");
	fprintf(fp, "\n");
	fprintf(fp, "Total Pcs=\n");
	fprintf(fp, "Good Pcs=\n");
	fprintf(fp, "Bad Pcs=\n");
	fprintf(fp, "\n");
	fprintf(fp, "Strip0=\n");
	fprintf(fp, "Strip1=\n");
	fprintf(fp, "Strip2=\n");
	fprintf(fp, "Strip3=\n");
	for (i = 1; i < MAX_DEF; i++)
		fprintf(fp, "%d=\n", i); // m_cBigDef[i]
	fprintf(fp, "\n");

	for (k = 0; k < MAX_STRIP_NUM; k++)
	{
		fprintf(fp, "[Strip%d]\n", k);
		for (i = 1; i < MAX_DEF; i++)
			fprintf(fp, "%d=\n", i); // m_cBigDef[i]
		fprintf(fp, "\n");
	}

	fprintf(fp, "[StripOut]\n");
	fprintf(fp, "Total=\n");
	for (k = 0; k < MAX_STRIP_NUM; k++)
		fprintf(fp, "%d=\n", k);
	fprintf(fp, "\n");

	fclose(fp);

	return TRUE;
}


// ITS

CString CReelMap::GetPathReelmapIts()
{
	CString sName = _T("ReelMapDataIts.txt");
	CString str, sPath, Path[3];

	sPath = _T("");
	Path[0] = pDoc->WorkingInfo.System.sPathItsFile;
	Path[1] = pDoc->WorkingInfo.LastJob.sModelUp;
	Path[2] = pDoc->WorkingInfo.LastJob.sEngItsCode; // pDoc->m_sItsCode;

	if (Path[0].IsEmpty() || Path[1].IsEmpty() || Path[2].IsEmpty())
		return _T("");

	sPath.Format(_T("%s%s\\%s\\%s"), Path[0], Path[1], Path[2], sName); // ReelmapIts.txt
	return sPath;
}

BOOL CReelMap::MakeItsReelmapHeader()
{
	FILE *fp = NULL;
	char FileName[MAX_PATH];
	BOOL bExist = FALSE;
	CString sName = _T("ReelMapDataIts.txt");
	CString str, sPath, Path[3];

	sPath = _T("");
	Path[0] = pDoc->WorkingInfo.System.sPathItsFile;
	Path[1] = pDoc->WorkingInfo.LastJob.sModelUp;
	Path[2] = pDoc->WorkingInfo.LastJob.sEngItsCode; // pDoc->m_sItsCode;

	if (Path[0].IsEmpty() || Path[1].IsEmpty() || Path[2].IsEmpty())
		return FALSE;

	sPath.Format(_T("%s%s\\%s\\%s"), Path[0], Path[1], Path[2], sName); // ReelmapIts.txt

	CFileFind findfile;
	if (findfile.FindFile(sPath))
	{
		bExist = TRUE;
		return TRUE;
	}

	sPath.Format(_T("%s%s"), Path[0], Path[1]);					// �� ����
	if (!pDoc->DirectoryExists(sPath))
		CreateDirectory(sPath, NULL);

	sPath.Format(_T("%s%s\\%s"), Path[0], Path[1], Path[2]);	// ITS �ڵ� ����
	if (!pDoc->DirectoryExists(sPath))
		CreateDirectory(sPath, NULL);

	sPath.Format(_T("%s%s\\%s\\%s"), Path[0], Path[1], Path[2], sName); // ReelmapIts.txt

	StrToChar(sPath, FileName);

	fp = fopen(FileName, "a+");
	if (fp == NULL)
	{
		str.Format(_T("It is trouble to open ReelmapIts.txt\r\n%s"), sPath);
		pView->MsgBox(str);
		return FALSE;
	}

	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;
	char* pRtn = NULL;
	int nNodeX = 0, nNodeY = 0, i = 0, k = 0;
#ifdef USE_CAM_MASTER
	nNodeX = pDoc->m_Master[0].m_pPcsRgn->nCol;
	nNodeY = pDoc->m_Master[0].m_pPcsRgn->nRow;
#endif

	fprintf(fp, "[Info]\n");
	fprintf(fp, "��  ��  �� = %s\n", pRtn = StrToChar(pDoc->WorkingInfo.System.sMcName)); if (pRtn) delete pRtn; pRtn = NULL;
	fprintf(fp, "��  ��  �� = %s\n", pRtn = StrToChar(pDoc->WorkingInfo.LastJob.sSelUserName)); if (pRtn) delete pRtn; pRtn = NULL;
	fprintf(fp, "��      �� = %s\n", pRtn = StrToChar(pDoc->WorkingInfo.LastJob.sModelUp)); if (pRtn) delete pRtn; pRtn = NULL;
	fprintf(fp, " ITS Code  = %s\n", pRtn = StrToChar(pDoc->WorkingInfo.LastJob.sEngItsCode)); if (pRtn) delete pRtn; pRtn = NULL;
	//fprintf(fp, " ITS Code  = %s\n", pRtn = StrToChar(pDoc->m_sItsCode)); if (pRtn) delete pRtn; pRtn = NULL;
	fprintf(fp, "\n");
	fprintf(fp, "Process Code = \n");
	fprintf(fp, "Entire Speed = \n");
	fprintf(fp, "\n");

	fprintf(fp, "CamPcsX = %d\n", nNodeX);
	fprintf(fp, "CamPcsY = %d\n", nNodeY);

	fprintf(fp, "\n");
	fprintf(fp, "0 -> ��ǰ\n");
	for (i = 1; i < MAX_DEF; i++)
	{
		fprintf(fp, "%d -> %s\n", i, pRtn = StrToChar(pDoc->m_pReelMapUp->m_sKorDef[i])); if (pRtn) delete pRtn; pRtn = NULL; // m_cBigDef[i]
	}
	fprintf(fp, "? - Missed Align Panel(i.e �뱤�ҷ�)\n");
	fprintf(fp, "\n");
	fprintf(fp, "Lot Start=\n");
	fprintf(fp, "Lot Run=\n");
	fprintf(fp, "Lot End=\n");
	fprintf(fp, "\n");
	fprintf(fp, "Marked Shot=\n");
	fprintf(fp, "Marked Date=\n");
	fprintf(fp, "Completed Shot=\n");
	fprintf(fp, "\n");
	fprintf(fp, "\n");
	fprintf(fp, "// < ���� ���� > \n");
	fprintf(fp, "\n");
	fprintf(fp, "Start Serial=\n");
	fprintf(fp, "End Serial=\n");
	fprintf(fp, "\n");
	fprintf(fp, "Total Pcs=\n");
	fprintf(fp, "Good Pcs=\n");
	fprintf(fp, "Bad Pcs=\n");
	fprintf(fp, "\n");
	fprintf(fp, "Strip0=\n");
	fprintf(fp, "Strip1=\n");
	fprintf(fp, "Strip2=\n");
	fprintf(fp, "Strip3=\n");
	for (i = 1; i < MAX_DEF; i++)
		fprintf(fp, "%d=\n", i); // m_cBigDef[i]
	fprintf(fp, "\n");

	for (k = 0; k < MAX_STRIP_NUM; k++)
	{
		fprintf(fp, "[Strip%d]\n", k);
		for (i = 1; i < MAX_DEF; i++)
			fprintf(fp, "%d=\n", i); // m_cBigDef[i]
		fprintf(fp, "\n");
	}

	fprintf(fp, "[StripOut]\n");
	fprintf(fp, "Total=\n");
	for (k = 0; k < MAX_STRIP_NUM; k++)
		fprintf(fp, "%d=\n", k);
	fprintf(fp, "\n");

	fclose(fp);

	return TRUE;
}

BOOL CReelMap::WriteIts(int nItsSerial)
{
	if (nItsSerial <= 0)
	{
		pView->ClrDispMsg();
		AfxMessageBox(_T("Serial Error.163"));
		return FALSE;
	}

	if (pDoc->GetTestMode() != MODE_OUTER)
		return FALSE;

	CString str, sPath;
	sPath = GetPathReelmapIts(); // pDoc->GetItsReelmapPath();
	if (sPath.IsEmpty())
	{
		str.Format(_T("ReelmapITS Path is emptied."));
		pView->MsgBox(str);
		return FALSE;
	}

	if (!MakeItsReelmapHeader())	// ������ ��¡�� ���� ���
	{
		str.Format(_T("It is trouble to open ReelmapIts.txt\r\n%s"), sPath);
		pView->MsgBox(str);
		return FALSE;
	}

	//BOOL bDualTestInner;
	//CString sLot, sLayerUp, sLayerDn;
	//if (!pDoc->GetItsSerialInfo(nItsSerial, bDualTestInner, sLot, sLayerUp, sLayerDn))
	//{
	//	str.Format(_T("It is trouble to read GetItsSerialInfo()."));
	//	pView->MsgBox(str);
	//	return FALSE;
	//}

	int nIdx = pDoc->GetPcrIdx0(nItsSerial);
	int nNodeX = pDoc->m_MasterInner[0].m_pPcsRgn->nCol;
	int nNodeY = pDoc->m_MasterInner[0].m_pPcsRgn->nRow;
	int nStripY = pDoc->m_MasterInner[0].m_pPcsRgn->nRow / MAX_STRIP_NUM; // Strip(1~4);
	int nTotDefPcs = 0;
	if (pDoc->m_pPcrIts)
	{
		if (pDoc->m_pPcrIts[nIdx])
			nTotDefPcs = pDoc->m_pPcrIts[nIdx]->m_nTotDef;
	}

	short **pPnlBuf;
	int i, nC, nR, nPcsId, nDefCode;//, nTot, nDef, nGood;
	pPnlBuf = new short*[nNodeY];
	for (i = 0; i < nNodeY; i++)
	{
		pPnlBuf[i] = new short[nNodeX];
		memset(pPnlBuf[i], 0, sizeof(short)*nNodeX);
		memset(m_pPnlBuf[nItsSerial - 1][i], 0, sizeof(short)*nNodeX);
	}

	CString strData, strTemp;
	int nTotVerifyed = 0;

	for (i = 0; i < nTotDefPcs; i++)
	{
		if (pDoc->m_pPcrIts[nIdx]->m_pMk[i] != -2) // -2 (NoMarking)
		{
			if (pDoc->WorkingInfo.System.bStripPcsRgnBin)	// DTS��
			{
				switch (pDoc->m_MasterInner[0].MasterInfo.nActionCode)	// 0 : Rotation / Mirror ���� ����(CAM Data ����), 1 : �¿� �̷�, 2 : ���� �̷�, 3 : 180 ȸ��, 4 : 270 ȸ��(CCW), 5 : 90 ȸ��(CW)
				{
				case 0:
					nPcsId = pDoc->m_pPcrIts[nIdx]->m_pDefPcs[i];
					break;
				case 1:
					nPcsId = pDoc->MirrorLR(pDoc->m_pPcrIts[nIdx]->m_pDefPcs[i]);
					break;
				case 2:
					nPcsId = pDoc->MirrorUD(pDoc->m_pPcrIts[nIdx]->m_pDefPcs[i]);
					break;
				case 3:
					nPcsId = pDoc->Rotate180(pDoc->m_pPcrIts[nIdx]->m_pDefPcs[i]);
					break;
				default:
					nPcsId = pDoc->m_pPcrIts[nIdx]->m_pDefPcs[i];
					break;
				}
			}
			else
				nPcsId = pDoc->m_pPcrIts[nIdx]->m_pDefPcs[i];

			nDefCode = pDoc->m_pPcrIts[nIdx]->m_pDefType[i];

			nC = int(nPcsId / nNodeY);
			if (nC % 2)	// Ȧ��.
				nR = nNodeY * (nC + 1) - nPcsId - 1;
			else		// ¦��.
				nR = nPcsId - nNodeY * nC;
			pPnlBuf[nR][nC] = (short)nDefCode;	// nPnl�� �� ����.
			if (m_pPnlBuf)
				m_pPnlBuf[nItsSerial - 1][nR][nC] = pPnlBuf[nR][nC];   // DefCode 3D Array : [nSerial][nRow][nCol] - �������� ������. #define PNL_TOT
		}
		else
		{
			nTotVerifyed++;
		}
	}

	CString sPnl, sRow;
	i = 0;
	sPnl.Format(_T("%d"), nItsSerial);
	strData.Format(_T("%d"), nTotDefPcs - nTotVerifyed);
	::WritePrivateProfileString(sPnl, _T("Total Defects"), strData, sPath);

	for (int nRow = 0; nRow < nNodeX; nRow++)
	{
		sRow.Format(_T("%02d"), nRow);
		//sRow.Format(_T("%2d"), nRow+1);
		strData.Format(_T(""));
		strTemp.Format(_T(""));

		//if(nRow==26)
		//	int brk=0;

		for (int nCol = 0; nCol < nNodeY; nCol++)
		{
			nR = (nNodeY - 1) - nCol;
			nC = nRow;

			if (pDoc->m_pPcrIts[nIdx]->m_nErrPnl == -1 || pDoc->m_pPcrIts[nIdx]->m_nErrPnl == -2)
			{
				nDefCode = DEF_LIGHT;
				m_pPnlBuf[nItsSerial - 1][nR][nC] = (short)nDefCode;
			}
			else
				nDefCode = (int)pPnlBuf[nR][nC] < 0 ? 0 : (int)pPnlBuf[nR][nC];

			strTemp.Format(_T("%2d,"), nDefCode);

			if (!nCol)
				strData.Insert(0, strTemp);
			else
			{
				int nLen = strData.GetLength();
				if (!(nCol%nStripY)) // Separate Strip
				{
					strData.Insert(nLen, _T("  "));
					nLen = strData.GetLength();
				}
				strData.Insert(nLen, strTemp);
			}
		}

		int nPos = strData.ReverseFind(',');
		strData.Delete(nPos, 1);
		::WritePrivateProfileString(sPnl, sRow, strData, sPath);
	}

	for (i = 0; i < nNodeY; i++)
		delete[]  pPnlBuf[i];
	delete[] pPnlBuf;

	pDoc->m_nWritedItsSerial = nItsSerial;
	return TRUE;
}

/*

BOOL CReelMap::InitRstIts()
{
	int nPnl, nRow;
	int TotPnl = PNL_TOT;
	int nDumyPnl = 20;
	// 	int nNodeX = pDoc->m_pPcsRgn->nCol;
	// 	int nNodeY = pDoc->m_pPcsRgn->nRow;
	int nNodeX = PNLBUF_X;
	int nNodeY = PNLBUF_Y;

	int k, i;

	CloseRstIts();

	m_nPnlBufIts = TotPnl + nDumyPnl;
	if (!m_pPnlBufIts)
	{
		m_pPnlBufIts = new short**[m_nPnlBufIts];
		for (nPnl = 0; nPnl < m_nPnlBufIts; nPnl++)
		{
			m_pPnlBufIts[nPnl] = new short*[nNodeY];
			for (nRow = 0; nRow < nNodeY; nRow++)
				m_pPnlBufIts[nPnl][nRow] = new short[nNodeX];
		}
	}

	m_nTotPcsIts = 0;
	m_nGoodPcsIts = 0;
	m_nBadPcsIts = 0;
	for (i = 0; i < MAX_DEF; i++)
		m_nDefIts[i] = 0;

	m_nTotStOutIts = 0;
	for (k = 0; k < MAX_STRIP_NUM; k++)
	{
		m_nStripOutIts[k] = 0;
		m_nDefStripIts[k] = 0;
		for (i = 0; i < MAX_DEF; i++)
			m_nDefPerStripIts[k][i] = 0;
	}

	return TRUE;
}

void CReelMap::CloseRstIts()
{
	int nPnl, nRow;
	// 	int nNodeX = pDoc->m_pPcsRgn->nCol;
	// 	int nNodeY = pDoc->m_pPcsRgn->nRow;
	int nNodeX = PNLBUF_X;
	int nNodeY = PNLBUF_Y;

	if (m_pPnlBufIts)
	{
		for (nPnl = 0; nPnl < m_nPnlBufIts; nPnl++)
		{
			for (nRow = 0; nRow < nNodeY; nRow++)
				delete[] m_pPnlBufIts[nPnl][nRow];
			delete[] m_pPnlBufIts[nPnl];
		}

		delete[] m_pPnlBufIts;
		m_pPnlBufIts = NULL;
		m_nPnlBufIts = 0;
	}
}

void CReelMap::ClrRstIts()
{
m_nTotPcs = 0;
m_nGoodPcs = 0;
m_nBadPcs = 0;

int k, i;
for (i = 1; i < MAX_DEF; i++)
m_nDef[i] = 0;

m_nTotStOut = 0;
for (k = 0; k < MAX_STRIP_NUM; k++)
{
m_nStripOut[k] = 0;
m_nDefStrip[k] = 0;
for (i = 1; i < MAX_DEF; i++)
m_nDefPerStrip[k][i] = 0;
}

// 	double dN0;
// 	if(m_dTotLen > m_dPnlLen)
// 		dN0 = m_dTotLen / m_dPnlLen;
// 	else
// 		dN0 = (double)PNL_TOT;
// 	int nN0 = int(m_dTotLen / m_dPnlLen);
// 	int TotPnl = (dN0 - (double)nN0) > 0.0 ? (nN0+1) : nN0 ;
// 	if(TotPnl > PNL_TOT)
// 		TotPnl = PNL_TOT;

int nPnl, nRow, nCol;
// 	int nNodeX = pDoc->m_Master[0].m_pPcsRgn->nCol;
// 	int nNodeY = pDoc->m_Master[0].m_pPcsRgn->nRow;
int TotPnl = PNL_TOT;
int nNodeX = PNLBUF_X;
int nNodeY = PNLBUF_Y;

if (m_pPnlBuf)
{
for (nPnl = 0; nPnl < TotPnl; nPnl++)
{
for (nRow = 0; nRow < nNodeY; nRow++)
{
for (nCol = 0; nCol < nNodeX; nCol++)
m_pPnlBuf[nPnl][nRow][nCol] = 0;
}
}
}
}

void CReelMap::SetFixPcsIts(int nSerial)
{
	// 	if(!pDoc->WorkingInfo.LastJob.bContFixDef)
	// 		return;

	int nNodeX = pDoc->m_Master[0].m_pPcsRgn->nCol;
	int nNodeY = pDoc->m_Master[0].m_pPcsRgn->nRow;
	int nR, nC, nDefCode;

	for (nR = 0; nR < nNodeY; nR++)
	{
		for (nC = 0; nC < nNodeX; nC++)
		{
			nDefCode = m_pPnlBufIts[nSerial - 1][nR][nC]; // DefCode
			if (nDefCode > 0)
				SetFixPcsIts(nSerial, nC, nR);
		}
	}
}

void CReelMap::SetFixPcsIts(int nSerial, int nCol, int nRow) // nShot : 0 ~ 
{
	if (nSerial < 1)
		return;

	int nRange = _tstoi(pDoc->WorkingInfo.LastJob.sNumRangeFixDef);
	int nS = (nSerial - 1) % nRange;
	m_FixPcsIts[nS][nCol][nRow] = TRUE;
}

void CReelMap::ClrFixPcsIts()
{
	m_nPrevSerial[0] = 0;	// --
	m_nPrevSerial[1] = 0;	// ++

	for (int nS = 0; nS < FIX_PCS_SHOT_MAX; nS++)
	{
		if (!nS)
		{
			m_nPrevSerial[0] = 0; // [0] : --
			m_nPrevSerial[1] = 0; // [1] : ++
		}

		for (int nC = 0; nC < FIX_PCS_COL_MAX; nC++)
		{
			for (int nR = 0; nR < FIX_PCS_ROW_MAX; nR++)
			{
				m_FixPcsIts[nS][nC][nR] = FALSE;
				if (!nS)
				{
					m_FixPcsPrevIts[nC][nR] = 0;
				}
			}
		}
	}
}

void CReelMap::ClrFixPcsIts(int nCol, int nRow)
{
	for (int nS = 0; nS < FIX_PCS_SHOT_MAX; nS++)
	{
		m_FixPcsIts[nS][nCol][nRow] = FALSE;
	}
	m_FixPcsPrevIts[nCol][nRow] = 0;
}

BOOL CReelMap::IsFixPcsIts(int nSerial, int &Col, int &Row)
{
	BOOL bRtn = FALSE;
	int nNodeX = pDoc->m_Master[0].m_pPcsRgn->nCol;
	int nNodeY = pDoc->m_Master[0].m_pPcsRgn->nRow;

	BOOL bOn = pDoc->WorkingInfo.LastJob.bContFixDef;
	int nRange = _tstoi(pDoc->WorkingInfo.LastJob.sNumRangeFixDef);
	int nJudge = _tstoi(pDoc->WorkingInfo.LastJob.sNumContFixDef);

	// 	if(nSerial < nJudge)
	// 		return bRtn;

	if (m_nPrevSerial[0] == nSerial)
		return bRtn;
	m_nPrevSerial[0] = nSerial;

	int nS;

	for (int nC = 0; nC < nNodeX; nC++)
	{
		for (int nR = 0; nR < nNodeY; nR++)
		{
			if (nSerial >= nRange)
			{
				nS = (nSerial - 1) % nRange;
				if (nS == nRange - 1)
					nS = 0;
				else
					nS++;

				if (m_FixPcs[nS][nC][nR]) // Defect PCS
				{
					if (m_FixPcsPrev[nC][nR] > 0)
						m_FixPcsPrev[nC][nR]--;
				}
				m_FixPcs[nS][nC][nR] = FALSE;
			}

			nS = (nSerial - 1) % nRange;
			if (m_FixPcs[nS][nC][nR]) // Defect PCS
			{
				// 				if(m_nPrevSerial[1] != nSerial) // [1] : ++
				// 				{
				//					m_nPrevSerial[1] = nSerial;
				m_FixPcsPrev[nC][nR]++;
				//				}
			}

			if (bOn)
			{
				if (m_FixPcsPrev[nC][nR] >= nJudge)
				{
					Col = nC;
					Row = nR;
					ClrFixPcs(nC, nR);
					bRtn = TRUE;
				}
			}
		}
	}

	return bRtn;
}

BOOL CReelMap::IsFixPcsIts(int nSerial, int* pCol, int* pRow, int &nTot) // nTot : total of PCS Over nJudge
{
	BOOL bRtn = FALSE;
	int nNodeX = pDoc->m_Master[0].m_pPcsRgn->nCol;
	int nNodeY = pDoc->m_Master[0].m_pPcsRgn->nRow;

	BOOL bOn = pDoc->WorkingInfo.LastJob.bContFixDef;
	int nRange = _tstoi(pDoc->WorkingInfo.LastJob.sNumRangeFixDef);
	int nJudge = _tstoi(pDoc->WorkingInfo.LastJob.sNumContFixDef);

	// 	if(nSerial < nJudge)
	// 		return bRtn;

	if (m_nPrevSerial[0] == nSerial)
		return bRtn;
	m_nPrevSerial[0] = nSerial;

	int nS = (nSerial - 1) % nRange;
	nTot = 0;

	for (int nC = 0; nC < nNodeX; nC++)
	{
		for (int nR = 0; nR < nNodeY; nR++)
		{
			if (nSerial >= nRange)
			{
				nS = (nSerial - 1) % nRange;
				if (nS == nRange - 1)
					nS = 0;
				else
					nS++;

				if (m_FixPcs[nS][nC][nR]) // Defect PCS
				{
					if (m_FixPcsPrev[nC][nR] > 0)
						m_FixPcsPrev[nC][nR]--;
				}
				m_FixPcs[nS][nC][nR] = FALSE;
			}

			nS = (nSerial - 1) % nRange;
			if (m_FixPcs[nS][nC][nR]) // Defect PCS
			{
				m_FixPcsPrev[nC][nR]++;
			}

			if (bOn)
			{
				if (m_FixPcsPrev[nC][nR] >= nJudge)
				{
					pCol[nTot] = nC;
					pRow[nTot] = nR;
					nTot++;
					ClrFixPcs(nC, nR);
					bRtn = TRUE;
				}
			}
		}
	}

	return bRtn;
}
*/

BOOL CReelMap::MakeItsFile(int nSerial, int nLayer) // RMAP_UP, RMAP_DN, RMAP_INNER_UP, RMAP_INNER_DN
{
	MakeDirIts();

	CString sMsg;
	CFileFind cFile;
	CString sPath = pDoc->GetItsPath(nSerial, nLayer);

	if (cFile.FindFile(sPath))
		DeleteFile(sPath);

	char FileName[MAX_PATH];
	StringToChar(sPath, FileName);

	char* pRtn = NULL;
	FILE *fp = NULL;
	fp = fopen(FileName, "w+");
	if (fp != NULL)
	{
		fprintf(fp, "%s", pRtn = StringToChar(GetItsFileData(nSerial, nLayer)));
		if (pRtn)
		{
			delete pRtn;
			pRtn = NULL;
		}
	}
	else
	{
		Sleep(300);

		fp = fopen(FileName, "w+");
		if (fp != NULL)
		{
			fprintf(fp, "%s", pRtn = StringToChar(GetItsFileData(nSerial, nLayer)));
			if (pRtn)
			{
				delete pRtn;
				pRtn = NULL;
			}
		}
		else
		{
			sMsg.Format(_T("It is trouble to MakeItsFile.\r\n%s"), FileName);
			pView->MsgBox(sMsg);
			return FALSE;
		}
	}

	fclose(fp);

	//CString sDestPath = pDoc->GetItsTargetPath(nSerial, nLayer);
	//BOOL bRtn = pDoc->m_pFile->Copy(sPath, sDestPath);			// ITS ������ �����Ѵ�.

	return TRUE;
}

CString CReelMap::GetItsFileData(int nSerial, int nLayer) // RMAP_UP, RMAP_DN, RMAP_INNER_UP, RMAP_INNER_DN
{
	CString sPath = pDoc->GetItsPath(nSerial, nLayer);
	CString str = _T(""), sSide = _T(""), sTemp = _T(""), sItsData = _T("");
	CString sItsCode = pDoc->WorkingInfo.LastJob.sEngItsCode;
	//CString sItsCode = pDoc->m_sItsCode;

	int nNodeX = pDoc->m_Master[0].m_pPcsRgn->nCol;
	int nNodeY = pDoc->m_Master[0].m_pPcsRgn->nRow;
	int nStripY = pDoc->m_Master[0].m_pPcsRgn->nRow / 4; // Strip(1~4);
	int nIdx = pDoc->GetPcrIdx0(nSerial);

	int nTotDefPcs = 0;

	switch (nLayer)
	{
	case RMAP_UP:
		sSide = _T("T");
		if (pDoc->m_pPcr[nLayer])
		{
			if (pDoc->m_pPcr[nLayer][nIdx])
				nTotDefPcs = pDoc->m_pPcr[nLayer][nIdx]->m_nTotDef;
		}
		break;
	case RMAP_DN:
		sSide = _T("B");
		if (pDoc->m_pPcr[nLayer])
		{
			if (pDoc->m_pPcr[nLayer][nIdx])
				nTotDefPcs = pDoc->m_pPcr[nLayer][nIdx]->m_nTotDef;
		}
		break;
	case RMAP_INNER_UP:
		nLayer = RMAP_UP;
		sSide = _T("T");
		if (pDoc->m_pPcr[nLayer])
		{
			if (pDoc->m_pPcr[nLayer][nIdx])
				nTotDefPcs = pDoc->m_pPcr[nLayer][nIdx]->m_nTotDef;
		}
		//if (pDoc->m_pPcrInner[0])
		//{
		//	if (pDoc->m_pPcrInner[0][nIdx])
		//		nTotDefPcs = pDoc->m_pPcrInner[0][nIdx]->m_nTotDef;
		//}
		break;
	case RMAP_INNER_DN:
		nLayer = RMAP_DN;
		sSide = _T("B");
		if (pDoc->m_pPcr[nLayer])
		{
			if (pDoc->m_pPcr[nLayer][nIdx])
				nTotDefPcs = pDoc->m_pPcr[nLayer][nIdx]->m_nTotDef;
		}
		//if (pDoc->m_pPcrInner[1])
		//{
		//	if (pDoc->m_pPcrInner[1][nIdx])
		//		nTotDefPcs = pDoc->m_pPcrInner[1][nIdx]->m_nTotDef;
		//}
		break;
	}

	str.Format(_T("%d,%s,%04d\n"), nTotDefPcs, sItsCode, nSerial);
	sItsData = str;

	int nR, nRow, nCol, nDefCode;
	int nTotStrip = 0;

	// Strip A
	for (nRow = 0; nRow < nStripY; nRow++)
	{
		nR = nRow;
		for (nCol = 0; nCol < nNodeX; nCol++)
		{
			nDefCode = m_pPnlBuf[nSerial - 1][nR][nCol]; // DefCode 3D Array : [nSerial][nRow][nCol] - �������� ������.
			if (nDefCode > 0)
			{
				nTotStrip++;
				str.Format(_T("%s,%04d,%s,A,%d,%d,B%d\n"), sItsCode, nSerial, sSide, nCol + 1, nRow + 1, pDoc->GetItsDefCode(nDefCode));
				sTemp += str;
			}
		}
	}
	str.Format(_T("%d,%s,%04d,%s,A\n"), nTotStrip, sItsCode, nSerial, sSide);
	sItsData += str;
	sItsData += sTemp;
	str.Format(_T("%s,%04d,%s,A,EOS\n"), sItsCode, nSerial, sSide);
	sItsData += str;
	sTemp = _T("");
	nTotStrip = 0;

	// Strip B
	for (nRow = 0; nRow < nStripY; nRow++)
	{
		nR = nRow + nStripY;
		for (nCol = 0; nCol < nNodeX; nCol++)
		{
			nDefCode = m_pPnlBuf[nSerial - 1][nR][nCol]; // DefCode 3D Array : [nSerial][nRow][nCol] - �������� ������.
			if (nDefCode > 0)
			{
				nTotStrip++;
				str.Format(_T("%s,%04d,%s,B,%d,%d,B%d\n"), sItsCode, nSerial, sSide, nCol + 1, nRow + 1, pDoc->GetItsDefCode(nDefCode));
				sTemp += str;
			}
		}
	}
	str.Format(_T("%d,%s,%04d,%s,B\n"), nTotStrip, sItsCode, nSerial, sSide);
	sItsData += str;
	sItsData += sTemp;
	str.Format(_T("%s,%04d,%s,B,EOS\n"), sItsCode, nSerial, sSide);
	sItsData += str;
	sTemp = _T("");
	nTotStrip = 0;

	// Strip C
	for (nRow = 0; nRow < nStripY; nRow++)
	{
		nR = nRow + nStripY * 2;
		for (nCol = 0; nCol < nNodeX; nCol++)
		{
			nDefCode = m_pPnlBuf[nSerial - 1][nR][nCol]; // DefCode 3D Array : [nSerial][nRow][nCol] - �������� ������.
			if (nDefCode > 0)
			{
				nTotStrip++;
				str.Format(_T("%s,%04d,%s,B,%d,%d,C%d\n"), sItsCode, nSerial, sSide, nCol + 1, nRow + 1, pDoc->GetItsDefCode(nDefCode));
				sTemp += str;
			}
		}
	}
	str.Format(_T("%d,%s,%04d,%s,C\n"), nTotStrip, sItsCode, nSerial, sSide);
	sItsData += str;
	sItsData += sTemp;
	str.Format(_T("%s,%04d,%s,C,EOS\n"), sItsCode, nSerial, sSide);
	sItsData += str;
	sTemp = _T("");
	nTotStrip = 0;

	// Strip D
	for (nRow = 0; nRow < nStripY; nRow++)
	{
		nR = nRow + nStripY * 3;
		for (nCol = 0; nCol < nNodeX; nCol++)
		{
			nDefCode = m_pPnlBuf[nSerial - 1][nR][nCol]; // DefCode 3D Array : [nSerial][nRow][nCol] - �������� ������.
			if (nDefCode > 0)
			{
				nTotStrip++;
				str.Format(_T("%s,%04d,%s,B,%d,%d,D%d\n"), sItsCode, nSerial, sSide, nCol + 1, nRow + 1, pDoc->GetItsDefCode(nDefCode));
				sTemp += str;
			}
		}
	}
	str.Format(_T("%d,%s,%04d,%s,D\n"), nTotStrip, sItsCode, nSerial, sSide);
	sItsData += str;
	sItsData += sTemp;
	str.Format(_T("%s,%04d,%s,D,EOS\n"), sItsCode, nSerial, sSide);
	sItsData += str;
	str.Format(_T("%s,%04d,%s,EOP\n"), sItsCode, nSerial, sSide);
	sItsData += str;

	return sItsData;
}

BOOL CReelMap::MakeDirIts()
{
	CFileFind finder;
	CString  Path[3], sItsPath;
	CString sPath = _T("");

	sItsPath = pDoc->WorkingInfo.System.sPathIts;
	Path[0] = pDoc->WorkingInfo.System.sPathItsFile;
	Path[1] = pDoc->WorkingInfo.LastJob.sModelUp;
	Path[2] = pDoc->WorkingInfo.LastJob.sEngItsCode;
	//Path[2] = pDoc->m_sItsCode;

	if (Path[0].IsEmpty() || Path[1].IsEmpty() || Path[2].IsEmpty())
		return FALSE;

	sPath.Format(_T("%s%s"), Path[0], Path[1]);
	if (!pDoc->DirectoryExists(sPath))
		CreateDirectory(sPath, NULL);

	sPath.Format(_T("%s%s\\%s"), Path[0], Path[1], Path[2]);
	if (!pDoc->DirectoryExists(sPath))
		CreateDirectory(sPath, NULL);

	sPath.Format(_T("%s%s\\%s\\Inner"), Path[0], Path[1], Path[2]);
	if (!pDoc->DirectoryExists(sPath))
		CreateDirectory(sPath, NULL);

	sPath.Format(_T("%s%s\\%s\\Outer"), Path[0], Path[1], Path[2]);
	if (!pDoc->DirectoryExists(sPath))
		CreateDirectory(sPath, NULL);


	if (sItsPath.IsEmpty())
		return FALSE;

	int pos = sItsPath.ReverseFind('\\');
	if (pos != -1)
		sItsPath.Delete(pos, sItsPath.GetLength() - pos);

	if (!pDoc->DirectoryExists(sItsPath))
		CreateDirectory(sItsPath, NULL);

	return TRUE;
}

void CReelMap::ResetReelmapPath()
{
	m_sPathBuf = GetRmapPath(m_nLayer);
	m_sPathYield = GetYieldPath(m_nLayer);
}


BOOL CReelMap::RemakeReelmap()
{
	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;
	CString sPath = GetRmapPath(m_nLayer);

	FILE *fp = NULL;
	char FileName[MAX_PATH];
	CString strFileName, strPathName;
	CFileFind findfile;
	int nStripNumY, nPieceNumPerStrip;

	CString sModel, sLot, sLayer[2];
	int i, nLastShot, nPnl, nRow, nCol, nDefCode, nCompletedShot;
	CString sPnl, sRow, sVal;
	TCHAR sep[] = { _T(",/;\r\n\t") };
	TCHAR szData[MAX_PATH];

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

	if (0 < ::GetPrivateProfileString(_T("Info"), _T("��      ��"), NULL, szData, sizeof(szData), sPath))
		sModel = CString(szData);
	else
	{
		pView->MsgBox(_T("Model ������ �����ϴ�."));
		return FALSE;
	}

	//if (0 < ::GetPrivateProfileString(_T("Info"), _T("��      Ʈ"), NULL, szData, sizeof(szData), sPath))
	//	sLot = CString(szData);
	//else
	//{
	//	pView->MsgBox(_T("Lot ������ �����ϴ�."));
	//	return FALSE;
	//}

	//if (0 < ::GetPrivateProfileString(_T("Info"), _T("��鷹�̾�"), NULL, szData, sizeof(szData), sPath))
	//	sLayer[0] = CString(szData);
	//else
	//{
	//	pView->MsgBox(_T("��鷹�̾� ������ �����ϴ�."));
	//	return FALSE;
	//}

	//if (bDualTest)
	//{
	//	if (0 < ::GetPrivateProfileString(_T("Info"), _T("�ϸ鷹�̾�"), NULL, szData, sizeof(szData), sPath))
	//		sLayer[1] = CString(szData);
	//	else
	//	{
	//		sLayer[1] = _T("");
	//		//pView->MsgBox(_T("�ϸ鷹�̾� ������ �����ϴ�."));
	//		//return FALSE;
	//	}
	//}

	MakeDirRmap();

	CString sFile = _T(""), sRmapPath = sPath;

	int nPos = sRmapPath.ReverseFind('\\');
	if (nPos != -1)
	{
		sFile = sRmapPath.Right(sRmapPath.GetLength() - nPos - 1);
		sRmapPath.Delete(nPos, sPath.GetLength() - nPos);
	}

	CString sPathRmapF = sRmapPath + _T("\\Reelmap");

	if (!pDoc->DirectoryExists(sPathRmapF))
		CreateDirectory(sPathRmapF, NULL);



	sFile.MakeUpper();

	if (sFile == _T("REELMAPDATAALL.TXT"))
		strPathName = sPathRmapF + _T("\\ReelMap-TOTAL_SIDE.txt");
	else if (sFile == _T("REELMAPDATAUP.TXT"))
		strPathName = sPathRmapF + _T("\\ReelMap-TOP_SIDE.txt");
	else if (sFile == _T("REELMAPDATADN.TXT"))
		strPathName = sPathRmapF + _T("\\ReelMap-BOTTOM_SIDE.txt");
	else if (sFile == _T("REELMAPDATAITS.TXT"))
		strPathName = sPathRmapF + _T("\\ReelMap-ITS.txt");
	else
		strPathName = sPathRmapF + _T("\\ReelMap.txt");

	if (findfile.FindFile(strPathName))
		DeleteFile(strPathName);
	else
	{
		if (!pDoc->DirectoryExists(sPathRmapF))
			CreateDirectory(sPath, NULL);
	}

	int nNodeX = 0;// = pDoc->m_pPcsRgn->nCol; // on Cam
	int nNodeY = 0;// = pDoc->m_pPcsRgn->nRow; // on Cam

	if (0 < ::GetPrivateProfileString(_T("Info"), _T("CamPcsX"), NULL, szData, sizeof(szData), sPath))
		nNodeX = _tstoi(szData);
	if (0 < ::GetPrivateProfileString(_T("Info"), _T("CamPcsY"), NULL, szData, sizeof(szData), sPath))
		nNodeY = _tstoi(szData);

	if (!nNodeX || !nNodeY)
		GetNodeXYonRmap(nNodeX, nNodeY, sPath);

	nStripNumY = MAX_STRIP_NUM;
	nPieceNumPerStrip = nNodeY / nStripNumY;

	m_dProgressRatio = 0.0;

	StringToChar(strPathName, FileName);

	fp = fopen(FileName, "a+");
	char* pRtn = NULL;
	if (fp != NULL)
	{
		fprintf(fp, "���ȣ��� : %s\n", pRtn = StrToChar(pDoc->WorkingInfo.System.sMcName)); if (pRtn) delete pRtn; pRtn = NULL;
		fprintf(fp, "��  ��  �� : %s\n", pRtn = StrToChar(pDoc->WorkingInfo.LastJob.sSelUserName)); if (pRtn) delete pRtn; pRtn = NULL;
		fprintf(fp, "��      �� : %s\n", pRtn = StrToChar(sModel)); if (pRtn) delete pRtn; pRtn = NULL;
		fprintf(fp, "��      Ʈ : %s\n", pRtn = StrToChar(sLot)); if (pRtn) delete pRtn; pRtn = NULL;
		fprintf(fp, "��鷹�̾� : %s\n", pRtn = StrToChar(sLayer[0])); if (pRtn) delete pRtn; pRtn = NULL;
		//if (bDualTest)
		if(!sLayer[1].IsEmpty())
		{
			fprintf(fp, "�ϸ鷹�̾� : %s\n", pRtn = StrToChar(sLayer[1])); if (pRtn) delete pRtn; pRtn = NULL;
		}
		fprintf(fp, "���� ��Ʈ�� ����[%%] : %.1f\n", _tstof(pDoc->WorkingInfo.LastJob.sStripOutRatio)); if (pRtn) delete pRtn; pRtn = NULL;//atof

		for (i = 1; i<DEF_UPPER; i++)
		{
			fprintf(fp, "%c - %s\n", pDoc->m_cBigDefCode[i], pRtn = StrToChar(m_sKorDef[i])); if (pRtn) delete pRtn; pRtn = NULL;
		}
		fprintf(fp, "? - Missed Align Panel(i.e �뱤�ҷ�)\r\n\n");

		// Reload Reelmap.....
		for (nPnl = 0; nPnl<nLastShot; nPnl++)
		{
			if (!m_bThreadAliveRemakeReelmap)
				break;

			m_dProgressRatio = double(nPnl + 1) / double(nLastShot) * 100.0;

			fprintf(fp, "%d\n", nPnl + 1);
			for (nRow = 0; nRow<nNodeX; nRow++)
			{
				sPnl.Format(_T("%d"), nPnl + 1);
				sRow.Format(_T("%02d"), nRow);
				if (0 < ::GetPrivateProfileString(sPnl, sRow, NULL, szData, sizeof(szData), sPath))
				{
					for (nCol = 0; nCol<nNodeY; nCol++)
					{
						if (nCol == 0)
							sVal = _tcstok(szData, sep);
						else
							sVal = _tcstok(NULL, sep);

						nDefCode = _tstoi(sVal);

						if (nCol % nPieceNumPerStrip == 0)
							fprintf(fp, " ");

						fprintf(fp, "%c", pDoc->m_cBigDefCode[nDefCode]);
					}
					fprintf(fp, "\n");
				}
			}
			fprintf(fp, "\n");
		}
	}
	else
	{
		pView->MsgBox(_T("It is trouble to remake ReelMap."));
		return FALSE;
	}

	fclose(fp);

	return TRUE;
}

BOOL CReelMap::GetResult() // TRUE: Make Result, FALSE: Load Result or Failed.
{
	//������ �о��. ======================================================================

	// �۾� �ð��� �о��.
	// 	LoadLotWorkingTime();

	// �ҷ����� ����Ÿ�� �о��.
	int i, k;
	CString strAddedDefectFP;
	CString strMsg, strMenu, strItem;
	TCHAR szData[MAX_PATH];
	CFileFind findfile;

	m_stResult.nEntireStripNum = 0;

	strAddedDefectFP = GetRmapPath(m_nLayer);

	if (!findfile.FindFile(strAddedDefectFP)) // Can not find file.
	{
		strMsg.Format(_T("%s file open is failed"), strAddedDefectFP);
		pView->ClrDispMsg();
		AfxMessageBox(strMsg);
	}
	else
	{
		if (0 < ::GetPrivateProfileString(_T("Info"), _T("��  ��  ��"), NULL, szData, sizeof(szData), strAddedDefectFP))
			m_stResult.sMachin = CString(szData);
		else
			m_stResult.sMachin = _T("");

		if (0 < ::GetPrivateProfileString(_T("Info"), _T("��  ��  ��"), NULL, szData, sizeof(szData), strAddedDefectFP))
			m_stResult.sOpName = CString(szData);
		else
			m_stResult.sOpName = _T("");

		if (0 < ::GetPrivateProfileString(_T("Info"), _T("��      ��"), NULL, szData, sizeof(szData), strAddedDefectFP))
			m_stResult.sModel = CString(szData);
		else
			m_stResult.sModel = _T("");

		if (0 < ::GetPrivateProfileString(_T("Info"), _T("��      Ʈ"), NULL, szData, sizeof(szData), strAddedDefectFP))
			m_stResult.sLot = CString(szData);
		else
			m_stResult.sLot = _T("");

		if (0 < ::GetPrivateProfileString(_T("Info"), _T("��鷹�̾�"), NULL, szData, sizeof(szData), strAddedDefectFP))
			m_stResult.sLayerUp = CString(szData);
		else
			m_stResult.sLayerUp = _T("");

		if (0 < ::GetPrivateProfileString(_T("Info"), _T("�ϸ鷹�̾�"), NULL, szData, sizeof(szData), strAddedDefectFP))
			m_stResult.sLayerDn = CString(szData);
		else
			m_stResult.sLayerDn = _T("");

		if (0 < ::GetPrivateProfileString(_T("Info"), _T("Lot Start"), NULL, szData, sizeof(szData), strAddedDefectFP))
			m_stResult.strLotStartTime = CString(szData);
		else
			m_stResult.strLotStartTime = _T("");

		if (0 < ::GetPrivateProfileString(_T("Info"), _T("Lot Run"), NULL, szData, sizeof(szData), strAddedDefectFP))
			m_stResult.strLotWorkingTime = CString(szData);
		else
			m_stResult.strLotWorkingTime = _T("");

		if (0 < ::GetPrivateProfileString(_T("Info"), _T("Lot End"), NULL, szData, sizeof(szData), strAddedDefectFP))
			m_stResult.strLotEndTime = CString(szData);
		else
			m_stResult.strLotEndTime = _T("");

		// �����ڵ�
		if (0 < ::GetPrivateProfileString(_T("Info"), _T("Process Code"), NULL, szData, sizeof(szData), strAddedDefectFP))
			m_stResult.sProcessNum = CString(szData);
		else
			m_stResult.sProcessNum = _T("");

		// �ӵ�
		if (0 < ::GetPrivateProfileString(_T("Info"), _T("Entire Speed"), NULL, szData, sizeof(szData), strAddedDefectFP))
			m_stResult.dEntireSpeed = _tstof(szData);
		else
			m_stResult.dEntireSpeed = 0.0;

		// ���� ����Ÿ�� �о��.

		// Read Piece infomation..............
		if (0 < ::GetPrivateProfileString(_T("Info"), _T("Total Pcs"), NULL, szData, sizeof(szData), strAddedDefectFP))
			m_stResult.nEntirePieceNum = _tstoi(szData);
		else
			m_stResult.nEntirePieceNum = 0;

		if (0 < ::GetPrivateProfileString(_T("Info"), _T("Good Pcs"), NULL, szData, sizeof(szData), strAddedDefectFP))
			m_stResult.nGoodPieceNum = _tstoi(szData);
		else
			m_stResult.nGoodPieceNum = 0;

		if (0 < ::GetPrivateProfileString(_T("Info"), _T("Bad Pcs"), NULL, szData, sizeof(szData), strAddedDefectFP))
			m_stResult.nDefectPieceNum = _tstoi(szData);
		else
			m_stResult.nDefectPieceNum = 0;

		m_stResult.nTotStOut = 0;
		for (k = 0; k<MAX_STRIP_NUM; k++)
		{
			strMenu.Format(_T("Strip%d"), k);
			if (0 < ::GetPrivateProfileString(_T("Info"), strMenu, NULL, szData, sizeof(szData), strAddedDefectFP))
				m_stResult.nDefStrip[k] = _tstoi(szData);
			else
				m_stResult.nDefStrip[k] = 0;

			strMenu.Format(_T("%d"), k);
			if (0 < ::GetPrivateProfileString(_T("StripOut"), strMenu, NULL, szData, sizeof(szData), strAddedDefectFP))
				m_stResult.nStripOut[k] = _tstoi(szData);
			else
				m_stResult.nStripOut[k] = 0;

			m_stResult.nTotStOut += m_stResult.nStripOut[k];

			for (i = 1; i<MAX_DEF; i++)
			{
				strItem.Format(_T("Strip%d"), k);
				strMenu.Format(_T("%d"), i);
				if (0 < ::GetPrivateProfileString(strItem, strMenu, NULL, szData, sizeof(szData), strAddedDefectFP))
					m_stResult.nDefPerStrip[k][i] = _tstoi(szData);
				else
					m_stResult.nDefPerStrip[k][i] = 0;
			}
		}

		for (i = 1; i<MAX_DEF; i++)
		{
			strMenu.Format(_T("%d"), i);
			if (0 < ::GetPrivateProfileString(_T("Info"), strMenu, NULL, szData, sizeof(szData), strAddedDefectFP))
				m_stResult.nEntireAddedDefect[i] = _tstoi(szData);  // �ҷ��̸��� �ҷ����� ���Ͽ��� �о��.
			else
			{
				m_stResult.nEntireAddedDefect[i] = 0;
			}
		}

		m_stResult.nEntireStripNum = (m_stResult.nSerialEd - m_stResult.nSerialSt + 1) * MAX_STRIP;

	}

	return TRUE;
}

CString CReelMap::GetResultTxt()
{
	CString strFileData, strData;
	int nTot;
	nTot = m_stResult.nDefPerStrip[0][DEF_LIGHT] + m_stResult.nDefPerStrip[1][DEF_LIGHT] + m_stResult.nDefPerStrip[2][DEF_LIGHT] + m_stResult.nDefPerStrip[3][DEF_LIGHT];

	//����Ʈ �ۼ�. =====================================================================

	strFileData = _T("");
	strFileData += _T("1. �Ϲ� ����\r\n");
	strData.Format(_T("    �� �� �� :  %s\r\n"), m_stResult.sModel);
	strFileData += strData;
	strData.Format(_T("    �� Ʈ �� :  %s\r\n"), m_stResult.sLot);
	strFileData += strData;
	strData.Format(_T("    ���۽ð� :  %s\r\n"), m_stResult.strLotStartTime);
	strFileData += strData;
	strData.Format(_T("    ����ð� :  %s\r\n"), m_stResult.strLotWorkingTime);
	strFileData += strData;
	strData.Format(_T("    ����ð� :  %s\r\n"), m_stResult.strLotEndTime);
	strFileData += strData;
	strFileData += _T("\r\n");

	strFileData += _T("2. ��ǰ �˻� ���\r\n");
	strData.Format(_T("    �� �� �� Unit �� : %9d\r\n"), m_stResult.nEntirePieceNum);
	strFileData += strData;
	strData.Format(_T("    ��    ǰ Unit �� : %9d        ��  ǰ  ��(%%) : %9.2f\r\n"), m_stResult.nEntirePieceNum - m_stResult.nDefectPieceNum, 100.0*(double)(m_stResult.nEntirePieceNum - m_stResult.nDefectPieceNum) / (double)m_stResult.nEntirePieceNum);
	strFileData += strData;
	strData.Format(_T("    ��    �� Unit �� : %9d        ��  ��  ��(%%) : %9.2f\r\n"), m_stResult.nDefectPieceNum, 100.0*(double)m_stResult.nDefectPieceNum / (double)m_stResult.nEntirePieceNum);
	strFileData += strData;
	strFileData += _T("\r\n");

	strFileData += _T("3. ���� �˻� ����\r\n");
	strData.Format(_T("    1�� �˻� Unit �� : %9d\r\n"), m_stResult.nEntirePieceNum / MAX_STRIP_NUM);
	strFileData += strData;
	strData.Format(_T("    1�� ��ǰ Unit �� : %9d        1�� ��ǰ��(%%) : %9.2f\r\n"), m_stResult.nEntirePieceNum / MAX_STRIP_NUM - m_stResult.nDefStrip[0], 100.0*(double)(m_stResult.nEntirePieceNum / MAX_STRIP_NUM - m_stResult.nDefStrip[0]) / (double)(m_stResult.nEntirePieceNum / MAX_STRIP_NUM));
	strFileData += strData;
	strData.Format(_T("    1�� �ҷ� Unit �� : %9d        1�� �ҷ���(%%) : %9.2f\r\n"), m_stResult.nDefStrip[0], 100.0*(double)m_stResult.nDefStrip[0] / (double)(m_stResult.nEntirePieceNum / MAX_STRIP_NUM));
	strFileData += strData;
	strFileData += _T("\r\n");
	strData.Format(_T("    2�� �˻� Unit �� : %9d\r\n"), m_stResult.nEntirePieceNum / MAX_STRIP_NUM);
	strFileData += strData;
	strData.Format(_T("    2�� ��ǰ Unit �� : %9d        2�� ��ǰ��(%%) : %9.2f\r\n"), m_stResult.nEntirePieceNum / MAX_STRIP_NUM - m_stResult.nDefStrip[1], 100.0*(double)(m_stResult.nEntirePieceNum / MAX_STRIP_NUM - m_stResult.nDefStrip[1]) / (double)(m_stResult.nEntirePieceNum / MAX_STRIP_NUM));
	strFileData += strData;
	strData.Format(_T("    2�� �ҷ� Unit �� : %9d        2�� �ҷ���(%%) : %9.2f\r\n"), m_stResult.nDefStrip[1], 100.0*(double)m_stResult.nDefStrip[1] / (double)(m_stResult.nEntirePieceNum / MAX_STRIP_NUM));
	strFileData += strData;
	strFileData += _T("\r\n");
	strData.Format(_T("    3�� �˻� Unit �� : %9d\r\n"), m_stResult.nEntirePieceNum / MAX_STRIP_NUM);
	strFileData += strData;
	strData.Format(_T("    3�� ��ǰ Unit �� : %9d        3�� ��ǰ��(%%) : %9.2f\r\n"), m_stResult.nEntirePieceNum / MAX_STRIP_NUM - m_nDefStrip[2], 100.0*(double)(m_stResult.nEntirePieceNum / MAX_STRIP_NUM - m_stResult.nDefStrip[2]) / (double)(m_stResult.nEntirePieceNum / MAX_STRIP_NUM));
	strFileData += strData;
	strData.Format(_T("    3�� �ҷ� Unit �� : %9d        3�� �ҷ���(%%) : %9.2f\r\n"), m_nDefStrip[2], 100.0*(double)m_stResult.nDefStrip[2] / (double)(m_stResult.nEntirePieceNum / MAX_STRIP_NUM));
	strFileData += strData;
	strFileData += _T("\r\n");
	strData.Format(_T("    4�� �˻� Unit �� : %9d\r\n"), m_stResult.nEntirePieceNum / MAX_STRIP_NUM);
	strFileData += strData;
	strData.Format(_T("    4�� ��ǰ Unit �� : %9d        4�� ��ǰ��(%%) : %9.2f\r\n"), m_stResult.nEntirePieceNum / MAX_STRIP_NUM - m_stResult.nDefStrip[3], 100.0*(double)(m_stResult.nEntirePieceNum / MAX_STRIP_NUM - m_stResult.nDefStrip[3]) / (double)(m_stResult.nEntirePieceNum / MAX_STRIP_NUM));
	strFileData += strData;
	strData.Format(_T("    4�� �ҷ� Unit �� : %9d        4�� �ҷ���(%%) : %9.2f\r\n"), m_stResult.nDefStrip[3], 100.0*(double)m_stResult.nDefStrip[3] / (double)(m_stResult.nEntirePieceNum / MAX_STRIP_NUM));
	strFileData += strData;
	strFileData += _T("\r\n");

	strFileData += _T("4. �ҷ� ����\r\n");
	strFileData += _T("    -----------------------------------------------------------------------------\r\n");
	strFileData += _T("    ��ȣ     �ҷ�����       1��       2��       3��       4��       ������ �ҷ���\r\n");
	strFileData += _T("    -----------------------------------------------------------------------------\r\n");
	strData.Format(_T("     1         ����  %10d%10d%10d%10d%20d\r\n"), m_stResult.nDefPerStrip[0][DEF_OPEN], m_stResult.nDefPerStrip[1][DEF_OPEN], m_stResult.nDefPerStrip[2][DEF_OPEN], m_stResult.nDefPerStrip[3][DEF_OPEN], m_stResult.nEntireAddedDefect[DEF_OPEN]);
	strFileData += strData;
	strData.Format(_T("     2         ��Ʈ  %10d%10d%10d%10d%20d\r\n"), m_stResult.nDefPerStrip[0][DEF_SHORT], m_stResult.nDefPerStrip[1][DEF_SHORT], m_stResult.nDefPerStrip[2][DEF_SHORT], m_stResult.nDefPerStrip[3][DEF_SHORT], m_stResult.nEntireAddedDefect[DEF_SHORT]);
	strFileData += strData;
	strData.Format(_T("     3        U-��Ʈ %10d%10d%10d%10d%20d\r\n"), m_stResult.nDefPerStrip[0][DEF_USHORT], m_stResult.nDefPerStrip[1][DEF_USHORT], m_stResult.nDefPerStrip[2][DEF_USHORT], m_stResult.nDefPerStrip[3][DEF_USHORT], m_stResult.nEntireAddedDefect[DEF_USHORT]);
	strFileData += strData;
	strData.Format(_T("     4         ���  %10d%10d%10d%10d%20d\r\n"), m_stResult.nDefPerStrip[0][DEF_NICK], m_stResult.nDefPerStrip[1][DEF_NICK], m_stResult.nDefPerStrip[2][DEF_NICK], m_stResult.nDefPerStrip[3][DEF_NICK], m_stResult.nEntireAddedDefect[DEF_NICK]);
	strFileData += strData;
	strData.Format(_T("     5        ������ %10d%10d%10d%10d%20d\r\n"), m_stResult.nDefPerStrip[0][DEF_SPACE], m_stResult.nDefPerStrip[1][DEF_SPACE], m_stResult.nDefPerStrip[2][DEF_SPACE], m_stResult.nDefPerStrip[3][DEF_SPACE], m_stResult.nEntireAddedDefect[DEF_SPACE]);
	strFileData += strData;
	strData.Format(_T("     6         �ܵ�  %10d%10d%10d%10d%20d\r\n"), m_stResult.nDefPerStrip[0][DEF_EXTRA], m_stResult.nDefPerStrip[1][DEF_EXTRA], m_stResult.nDefPerStrip[2][DEF_EXTRA], m_stResult.nDefPerStrip[3][DEF_EXTRA], m_stResult.nEntireAddedDefect[DEF_EXTRA]);
	strFileData += strData;
	strData.Format(_T("     7         ����  %10d%10d%10d%10d%20d\r\n"), m_stResult.nDefPerStrip[0][DEF_PROTRUSION], m_stResult.nDefPerStrip[1][DEF_PROTRUSION], m_stResult.nDefPerStrip[2][DEF_PROTRUSION], m_stResult.nDefPerStrip[3][DEF_PROTRUSION], m_stResult.nEntireAddedDefect[DEF_PROTRUSION]);
	strFileData += strData;
	strData.Format(_T("     8         ��Ȧ  %10d%10d%10d%10d%20d\r\n"), m_stResult.nDefPerStrip[0][DEF_PINHOLE], m_stResult.nDefPerStrip[1][DEF_PINHOLE], m_stResult.nDefPerStrip[2][DEF_PINHOLE], m_stResult.nDefPerStrip[3][DEF_PINHOLE], m_stResult.nEntireAddedDefect[DEF_PINHOLE]);
	strFileData += strData;
	strData.Format(_T("     9       �е����%10d%10d%10d%10d%20d\r\n"), m_stResult.nDefPerStrip[0][DEF_PAD], m_stResult.nDefPerStrip[1][DEF_PAD], m_stResult.nDefPerStrip[2][DEF_PAD], m_stResult.nDefPerStrip[3][DEF_PAD], m_stResult.nEntireAddedDefect[DEF_PAD]);
	strFileData += strData;
	strData.Format(_T("    10        Ȧ���� %10d%10d%10d%10d%20d\r\n"), m_stResult.nDefPerStrip[0][DEF_HOLE_OPEN], m_stResult.nDefPerStrip[1][DEF_HOLE_OPEN], m_stResult.nDefPerStrip[2][DEF_HOLE_OPEN], m_stResult.nDefPerStrip[3][DEF_HOLE_OPEN], m_stResult.nEntireAddedDefect[DEF_HOLE_OPEN]);
	strFileData += strData;
	strData.Format(_T("    11        Ȧ���� %10d%10d%10d%10d%20d\r\n"), m_stResult.nDefPerStrip[0][DEF_HOLE_MISS], m_stResult.nDefPerStrip[1][DEF_HOLE_MISS], m_stResult.nDefPerStrip[2][DEF_HOLE_MISS], m_stResult.nDefPerStrip[3][DEF_HOLE_MISS], m_stResult.nEntireAddedDefect[DEF_HOLE_MISS]);
	strFileData += strData;
	strData.Format(_T("    12        Ȧ��� %10d%10d%10d%10d%20d\r\n"), m_stResult.nDefPerStrip[0][DEF_HOLE_POSITION], m_stResult.nDefPerStrip[1][DEF_HOLE_POSITION], m_stResult.nDefPerStrip[2][DEF_HOLE_POSITION], m_stResult.nDefPerStrip[3][DEF_HOLE_POSITION], m_stResult.nEntireAddedDefect[DEF_HOLE_POSITION]);
	strFileData += strData;
	strData.Format(_T("    13       Ȧ���ҷ�%10d%10d%10d%10d%20d\r\n"), m_stResult.nDefPerStrip[0][DEF_HOLE_DEFECT], m_stResult.nDefPerStrip[1][DEF_HOLE_DEFECT], m_stResult.nDefPerStrip[2][DEF_HOLE_DEFECT], m_stResult.nDefPerStrip[3][DEF_HOLE_DEFECT], m_stResult.nEntireAddedDefect[DEF_HOLE_DEFECT]);
	strFileData += strData;
	strData.Format(_T("    14          POI  %10d%10d%10d%10d%20d\r\n"), m_stResult.nDefPerStrip[0][DEF_POI], m_stResult.nDefPerStrip[1][DEF_POI], m_stResult.nDefPerStrip[2][DEF_POI], m_stResult.nDefPerStrip[3][DEF_POI], m_stResult.nEntireAddedDefect[DEF_POI]);
	strFileData += strData;
	strData.Format(_T("    15        VH���� %10d%10d%10d%10d%20d\r\n"), m_stResult.nDefPerStrip[0][DEF_VH_OPEN], m_stResult.nDefPerStrip[1][DEF_VH_OPEN], m_stResult.nDefPerStrip[2][DEF_VH_OPEN], m_stResult.nDefPerStrip[3][DEF_VH_OPEN], m_stResult.nEntireAddedDefect[DEF_VH_OPEN]);
	strFileData += strData;
	strData.Format(_T("    16        VH���� %10d%10d%10d%10d%20d\r\n"), m_stResult.nDefPerStrip[0][DEF_VH_MISS], m_stResult.nDefPerStrip[1][DEF_VH_MISS], m_stResult.nDefPerStrip[2][DEF_VH_MISS], m_stResult.nDefPerStrip[3][DEF_VH_MISS], m_stResult.nEntireAddedDefect[DEF_VH_MISS]);
	strFileData += strData;
	strData.Format(_T("    17        VH��� %10d%10d%10d%10d%20d\r\n"), m_stResult.nDefPerStrip[0][DEF_VH_POSITION], m_stResult.nDefPerStrip[1][DEF_VH_POSITION], m_stResult.nDefPerStrip[2][DEF_VH_POSITION], m_stResult.nDefPerStrip[3][DEF_VH_POSITION], m_stResult.nEntireAddedDefect[DEF_VH_POSITION]);
	strFileData += strData;
	strData.Format(_T("    18        VH���� %10d%10d%10d%10d%20d\r\n"), m_stResult.nDefPerStrip[0][DEF_VH_DEF], m_stResult.nDefPerStrip[1][DEF_VH_DEF], m_stResult.nDefPerStrip[2][DEF_VH_DEF], m_stResult.nDefPerStrip[3][DEF_VH_DEF], m_stResult.nEntireAddedDefect[DEF_VH_DEF]);
	strFileData += strData;
	strData.Format(_T("    19         �뱤  %10d%10d%10d%10d%20d\r\n"), m_stResult.nDefPerStrip[0][DEF_LIGHT], m_stResult.nDefPerStrip[1][DEF_LIGHT], m_stResult.nDefPerStrip[2][DEF_LIGHT], m_stResult.nDefPerStrip[3][DEF_LIGHT], nTot);
	strFileData += strData;
	strFileData += _T("    -----------------------------------------------------------------------------\r\n");
	strFileData += _T("                                                                                 \r\n");
	strFileData += _T("                                                                                 \r\n");
	strFileData += _T("                                                                                 \r\n");
	strFileData += _T("\r\n");

	return strFileData;
}

CString CReelMap::GetSapp3Txt()
{
	CString strFileData = _T("");
	CString strData;
	int nSum, nStripPcs;
	double dRateBeforeVerify, dRateAfterVerify;
	nStripPcs = m_stResult.nEntirePieceNum / MAX_STRIP_NUM;

	// ���� �̸�.
	strFileData.Format(_T("FileName : %9s_%4s_%5s.txt\r\n\r\n"), m_stResult.sLot, m_stResult.sProcessNum, m_stResult.sMachin);

	// ���� ����/�ϼ�/���� Data.
	strFileData += _T("1Q\r\n");
	dRateBeforeVerify = 100.0 * (nStripPcs - m_stResult.nDefStrip[0]) / nStripPcs;
	dRateAfterVerify = 100.0 * (nStripPcs - m_stResult.nDefStrip[0]) / nStripPcs;
	strData.Format(_T("%d,%d,%.1f,%.1f\r\n"), nStripPcs, nStripPcs - m_stResult.nDefStrip[0], dRateBeforeVerify, dRateAfterVerify); // ���Լ���, �ϼ�����, Verify�� ����, Verify�� ����
	strFileData += strData;

	strFileData += _T("2Q\r\n");
	dRateBeforeVerify = 100.0 * (nStripPcs - m_stResult.nDefStrip[1]) / nStripPcs;
	dRateAfterVerify = 100.0 * (nStripPcs - m_stResult.nDefStrip[1]) / nStripPcs;
	strData.Format(_T("%d,%d,%.1f,%.1f\r\n"), nStripPcs, nStripPcs - m_stResult.nDefStrip[1], dRateBeforeVerify, dRateAfterVerify); // ���Լ���, �ϼ�����, Verify�� ����, Verify�� ����
	strFileData += strData;

	strFileData += _T("3Q\r\n");
	dRateBeforeVerify = 100.0 * (nStripPcs - m_stResult.nDefStrip[2]) / nStripPcs;
	dRateAfterVerify = 100.0 * (nStripPcs - m_stResult.nDefStrip[2]) / nStripPcs;
	strData.Format(_T("%d,%d,%.1f,%.1f\r\n"), nStripPcs, nStripPcs - m_stResult.nDefStrip[2], dRateBeforeVerify, dRateAfterVerify); // ���Լ���, �ϼ�����, Verify�� ����, Verify�� ����
	strFileData += strData;

	strFileData += _T("4Q\r\n");
	dRateBeforeVerify = 100.0 * (nStripPcs - m_stResult.nDefStrip[3]) / nStripPcs;
	dRateAfterVerify = 100.0 * (nStripPcs - m_stResult.nDefStrip[3]) / nStripPcs;
	strData.Format(_T("%d,%d,%.1f,%.1f\r\n"), nStripPcs, nStripPcs - m_stResult.nDefStrip[3], dRateBeforeVerify, dRateAfterVerify); // ���Լ���, �ϼ�����, Verify�� ����, Verify�� ����
	strFileData += strData;


	strFileData += _T("\r\n");


	// ���� �ҷ� Data.
	strFileData += _T("1X\r\n");

	if (m_stResult.nDefPerStrip[0][DEF_OPEN] > 0 && pDoc->m_nSapp3Code[SAPP3_OPEN] > 0)
	{
		strData.Format(_T("B%d,%d\r\n"), pDoc->m_nSapp3Code[SAPP3_OPEN], m_stResult.nDefPerStrip[0][DEF_OPEN]); // ����(B102)
		strFileData += strData;
	}

	nSum = m_stResult.nDefPerStrip[0][DEF_SHORT];// + m_stResult.nDefPerStrip[0][DEF_USHORT];
	if (nSum > 0 && pDoc->m_nSapp3Code[SAPP3_SHORT] > 0)
	{
		strData.Format(_T("B%d,%d\r\n"), pDoc->m_nSapp3Code[SAPP3_SHORT], nSum); // ��Ʈ(B129) // +u��Ʈ
		strFileData += strData;
	}

	nSum = m_stResult.nDefPerStrip[0][DEF_USHORT];
	if (nSum > 0 && pDoc->m_nSapp3Code[SAPP3_USHORT] > 0)
	{
		strData.Format(_T("B%d,%d\r\n"), pDoc->m_nSapp3Code[SAPP3_USHORT], nSum); // u��Ʈ(B314)
		strFileData += strData;
	}

	if (m_stResult.nDefPerStrip[0][DEF_NICK] > 0 && pDoc->m_nSapp3Code[SAPP3_NICK] > 0)
	{
		strData.Format(_T("B%d,%d\r\n"), pDoc->m_nSapp3Code[SAPP3_NICK], m_stResult.nDefPerStrip[0][DEF_NICK]); // ���(B137)
		strFileData += strData;
	}

	nSum = m_stResult.nDefPerStrip[0][DEF_SPACE] + m_stResult.nDefPerStrip[0][DEF_EXTRA] + m_stResult.nDefPerStrip[0][DEF_PROTRUSION];
	if (nSum > 0 && pDoc->m_nSapp3Code[SAPP3_SPACE_EXTRA_PROTRUSION] > 0)
	{
		strData.Format(_T("B%d,%d\r\n"), pDoc->m_nSapp3Code[SAPP3_SPACE_EXTRA_PROTRUSION], nSum); // ������+�ܵ�+����(B160)
		strFileData += strData;
	}

	nSum = m_stResult.nDefPerStrip[0][DEF_PINHOLE];
	if (nSum > 0 && pDoc->m_nSapp3Code[SAPP3_PINHOLE] > 0)
	{
		strData.Format(_T("B%d,%d\r\n"), pDoc->m_nSapp3Code[SAPP3_PINHOLE], nSum); // ��Ȧ(B134)
		strFileData += strData;
	}

	nSum = m_stResult.nDefPerStrip[0][DEF_PAD];
	if (nSum > 0 && pDoc->m_nSapp3Code[SAPP3_PAD] > 0)
	{
		strData.Format(_T("B%d,%d\r\n"), pDoc->m_nSapp3Code[SAPP3_PAD], nSum); // �е�(B316)
		strFileData += strData;
	}

	if (nSum > 0 && pDoc->m_nSapp3Code[SAPP3_HOPEN] > 0)
	{
		strData.Format(_T("B%d,%d\r\n"), pDoc->m_nSapp3Code[SAPP3_HOPEN], m_stResult.nDefPerStrip[0][DEF_HOLE_OPEN]);
		strFileData += strData;
	}
	nSum = m_stResult.nDefPerStrip[0][DEF_HOLE_MISS] + m_stResult.nDefPerStrip[0][DEF_HOLE_POSITION] + m_stResult.nDefPerStrip[0][DEF_HOLE_DEFECT];
	if (nSum > 0 && pDoc->m_nSapp3Code[SAPP3_HMISS_HPOS_HBAD] > 0)
	{
		strData.Format(_T("B%d,%d\r\n"), pDoc->m_nSapp3Code[SAPP3_HMISS_HPOS_HBAD], nSum);
		strFileData += strData;
	}

	nSum = m_stResult.nDefPerStrip[0][DEF_VH_OPEN] + m_stResult.nDefPerStrip[0][DEF_VH_MISS] + m_stResult.nDefPerStrip[0][DEF_VH_POSITION] + m_stResult.nDefPerStrip[0][DEF_VH_DEF];
	if (nSum > 0 && pDoc->m_nSapp3Code[SAPP3_VHOPEN_NOVH_VHALIGN_VHDEF] > 0)
	{
		strData.Format(_T("B%d,%d\r\n"), pDoc->m_nSapp3Code[SAPP3_VHOPEN_NOVH_VHALIGN_VHDEF], nSum);
		strFileData += strData;
	}

	strFileData += _T("2X\r\n");

	if (m_stResult.nDefPerStrip[1][DEF_OPEN] > 0 && pDoc->m_nSapp3Code[SAPP3_OPEN] > 0)
	{
		strData.Format(_T("B%d,%d\r\n"), pDoc->m_nSapp3Code[SAPP3_OPEN], m_stResult.nDefPerStrip[1][DEF_OPEN]); // ����(B102)
		strFileData += strData;
	}

	nSum = m_stResult.nDefPerStrip[1][DEF_SHORT];// + m_stResult.nDefPerStrip[2][DEF_USHORT];
	if (nSum > 0 && pDoc->m_nSapp3Code[SAPP3_SHORT] > 0)
	{
		strData.Format(_T("B%d,%d\r\n"), pDoc->m_nSapp3Code[SAPP3_SHORT], nSum); // ��Ʈ(B129) // +u��Ʈ
		strFileData += strData;
	}

	nSum = m_stResult.nDefPerStrip[1][DEF_USHORT];
	if (nSum > 0 && pDoc->m_nSapp3Code[SAPP3_USHORT] > 0)
	{
		strData.Format(_T("B%d,%d\r\n"), pDoc->m_nSapp3Code[SAPP3_USHORT], nSum); // u��Ʈ(B314)
		strFileData += strData;
	}

	if (m_stResult.nDefPerStrip[1][DEF_NICK] > 0 && pDoc->m_nSapp3Code[SAPP3_NICK] > 0)
	{
		strData.Format(_T("B%d,%d\r\n"), pDoc->m_nSapp3Code[SAPP3_NICK], m_stResult.nDefPerStrip[1][DEF_NICK]); // ���(B137)
		strFileData += strData;
	}

	nSum = m_stResult.nDefPerStrip[1][DEF_SPACE] + m_stResult.nDefPerStrip[1][DEF_EXTRA] + m_stResult.nDefPerStrip[1][DEF_PROTRUSION];
	if (nSum > 0 && pDoc->m_nSapp3Code[SAPP3_SPACE_EXTRA_PROTRUSION] > 0)
	{
		strData.Format(_T("B%d,%d\r\n"), pDoc->m_nSapp3Code[SAPP3_SPACE_EXTRA_PROTRUSION], nSum); // ������+�ܵ�+����(B160)
		strFileData += strData;
	}

	nSum = m_stResult.nDefPerStrip[1][DEF_PINHOLE];
	if (nSum > 0 && pDoc->m_nSapp3Code[SAPP3_PINHOLE] > 0)
	{
		strData.Format(_T("B%d,%d\r\n"), pDoc->m_nSapp3Code[SAPP3_PINHOLE], nSum); // ��Ȧ(B134)
		strFileData += strData;
	}

	nSum = m_stResult.nDefPerStrip[1][DEF_PAD];
	if (nSum > 0 && pDoc->m_nSapp3Code[SAPP3_PAD] > 0)
	{
		strData.Format(_T("B%d,%d\r\n"), pDoc->m_nSapp3Code[SAPP3_PAD], nSum); // �е�(B316)
		strFileData += strData;
	}

	if (nSum > 0 && pDoc->m_nSapp3Code[SAPP3_HOPEN] > 0)
	{
		strData.Format(_T("B%d,%d\r\n"), pDoc->m_nSapp3Code[SAPP3_HOPEN], m_stResult.nDefPerStrip[2][DEF_HOLE_OPEN]);
		strFileData += strData;
	}

	nSum = m_stResult.nDefPerStrip[1][DEF_HOLE_MISS] + m_stResult.nDefPerStrip[1][DEF_HOLE_POSITION] + m_stResult.nDefPerStrip[1][DEF_HOLE_DEFECT];
	if (nSum > 0 && pDoc->m_nSapp3Code[SAPP3_HMISS_HPOS_HBAD] > 0)
	{
		strData.Format(_T("B%d,%d\r\n"), pDoc->m_nSapp3Code[SAPP3_HMISS_HPOS_HBAD], nSum);
		strFileData += strData;
	}

	nSum = m_stResult.nDefPerStrip[1][DEF_VH_OPEN] + m_stResult.nDefPerStrip[1][DEF_VH_MISS] + m_stResult.nDefPerStrip[1][DEF_VH_POSITION] + m_stResult.nDefPerStrip[1][DEF_VH_DEF];
	if (nSum > 0 && pDoc->m_nSapp3Code[SAPP3_VHOPEN_NOVH_VHALIGN_VHDEF] > 0)
	{
		strData.Format(_T("B%d,%d\r\n"), pDoc->m_nSapp3Code[SAPP3_VHOPEN_NOVH_VHALIGN_VHDEF], nSum);
		strFileData += strData;
	}


	strFileData += _T("3X\r\n");

	if (m_stResult.nDefPerStrip[2][DEF_OPEN] > 0 && pDoc->m_nSapp3Code[SAPP3_OPEN] > 0)
	{
		strData.Format(_T("B%d,%d\r\n"), pDoc->m_nSapp3Code[SAPP3_OPEN], m_stResult.nDefPerStrip[2][DEF_OPEN]); // ����(B102)
		strFileData += strData;
	}

	nSum = m_stResult.nDefPerStrip[2][DEF_SHORT];// + m_stResult.nDefPerStrip[1][DEF_USHORT];
	if (nSum > 0 && pDoc->m_nSapp3Code[SAPP3_SHORT] > 0)
	{
		strData.Format(_T("B%d,%d\r\n"), pDoc->m_nSapp3Code[SAPP3_SHORT], nSum); // ��Ʈ(B129) // +u��Ʈ
		strFileData += strData;
	}

	nSum = m_stResult.nDefPerStrip[2][DEF_USHORT];
	if (nSum > 0 && pDoc->m_nSapp3Code[SAPP3_USHORT] > 0)
	{
		strData.Format(_T("B%d,%d\r\n"), pDoc->m_nSapp3Code[SAPP3_USHORT], nSum); // u��Ʈ(B314)
		strFileData += strData;
	}

	if (m_stResult.nDefPerStrip[2][DEF_NICK] > 0 && pDoc->m_nSapp3Code[SAPP3_NICK] > 0)
	{
		strData.Format(_T("B%d,%d\r\n"), pDoc->m_nSapp3Code[SAPP3_NICK], m_stResult.nDefPerStrip[2][DEF_NICK]); // ���(B137)
		strFileData += strData;
	}

	nSum = m_stResult.nDefPerStrip[2][DEF_SPACE] + m_stResult.nDefPerStrip[2][DEF_EXTRA] + m_stResult.nDefPerStrip[2][DEF_PROTRUSION];
	if (nSum > 0 && pDoc->m_nSapp3Code[SAPP3_SPACE_EXTRA_PROTRUSION] > 0)
	{
		strData.Format(_T("B%d,%d\r\n"), pDoc->m_nSapp3Code[SAPP3_SPACE_EXTRA_PROTRUSION], nSum); // ������+�ܵ�+����(B160)
		strFileData += strData;
	}

	nSum = m_stResult.nDefPerStrip[2][DEF_PINHOLE];
	if (nSum > 0 && pDoc->m_nSapp3Code[SAPP3_PINHOLE] > 0)
	{
		strData.Format(_T("B%d,%d\r\n"), pDoc->m_nSapp3Code[SAPP3_PINHOLE], nSum); // ��Ȧ(B134)
		strFileData += strData;
	}

	nSum = m_stResult.nDefPerStrip[2][DEF_PAD];
	if (nSum > 0 && pDoc->m_nSapp3Code[SAPP3_PAD] > 0)
	{
		strData.Format(_T("B%d,%d\r\n"), pDoc->m_nSapp3Code[SAPP3_PAD], nSum); // �е�(B316)
		strFileData += strData;
	}

	if (nSum > 0 && pDoc->m_nSapp3Code[SAPP3_HOPEN] > 0)
	{
		strData.Format(_T("B%d,%d\r\n"), pDoc->m_nSapp3Code[SAPP3_HOPEN], m_stResult.nDefPerStrip[2][DEF_HOLE_OPEN]);
		strFileData += strData;
	}

	nSum = m_stResult.nDefPerStrip[2][DEF_HOLE_MISS] + m_stResult.nDefPerStrip[2][DEF_HOLE_POSITION] + m_stResult.nDefPerStrip[2][DEF_HOLE_DEFECT];
	if (nSum > 0 && pDoc->m_nSapp3Code[SAPP3_HMISS_HPOS_HBAD] > 0)
	{
		strData.Format(_T("B%d,%d\r\n"), pDoc->m_nSapp3Code[SAPP3_HMISS_HPOS_HBAD], nSum);
		strFileData += strData;
	}

	nSum = m_stResult.nDefPerStrip[2][DEF_VH_OPEN] + m_stResult.nDefPerStrip[2][DEF_VH_MISS] + m_stResult.nDefPerStrip[2][DEF_VH_POSITION] + m_stResult.nDefPerStrip[2][DEF_VH_DEF];
	if (nSum > 0 && pDoc->m_nSapp3Code[SAPP3_VHOPEN_NOVH_VHALIGN_VHDEF] > 0)
	{
		strData.Format(_T("B%d,%d\r\n"), pDoc->m_nSapp3Code[SAPP3_VHOPEN_NOVH_VHALIGN_VHDEF], nSum);
		strFileData += strData;
	}


	strFileData += _T("4X\r\n");

	if (m_stResult.nDefPerStrip[3][DEF_OPEN] > 0 && pDoc->m_nSapp3Code[SAPP3_OPEN] > 0)
	{
		strData.Format(_T("B%d,%d\r\n"), pDoc->m_nSapp3Code[SAPP3_OPEN], m_stResult.nDefPerStrip[3][DEF_OPEN]); // ����(B102)
		strFileData += strData;
	}

	nSum = m_stResult.nDefPerStrip[3][DEF_SHORT];// + m_stResult.nDefPerStrip[3][DEF_USHORT];
	if (nSum > 0 && pDoc->m_nSapp3Code[SAPP3_SHORT] > 0)
	{
		strData.Format(_T("B%d,%d\r\n"), pDoc->m_nSapp3Code[SAPP3_SHORT], nSum); // ��Ʈ(B129) // +u��Ʈ
		strFileData += strData;
	}

	nSum = m_stResult.nDefPerStrip[3][DEF_USHORT];
	if (nSum > 0 && pDoc->m_nSapp3Code[SAPP3_USHORT] > 0)
	{
		strData.Format(_T("B%d,%d\r\n"), pDoc->m_nSapp3Code[SAPP3_USHORT], nSum); // u��Ʈ(B314)
		strFileData += strData;
	}

	if (m_stResult.nDefPerStrip[3][DEF_NICK] > 0 && pDoc->m_nSapp3Code[SAPP3_NICK] > 0)
	{
		strData.Format(_T("B%d,%d\r\n"), pDoc->m_nSapp3Code[SAPP3_NICK], m_stResult.nDefPerStrip[3][DEF_NICK]); // ���(B137)
		strFileData += strData;
	}

	nSum = m_stResult.nDefPerStrip[3][DEF_SPACE] + m_stResult.nDefPerStrip[3][DEF_EXTRA] + m_stResult.nDefPerStrip[3][DEF_PROTRUSION];
	if (nSum > 0 && pDoc->m_nSapp3Code[SAPP3_SPACE_EXTRA_PROTRUSION] > 0)
	{
		strData.Format(_T("B%d,%d\r\n"), pDoc->m_nSapp3Code[SAPP3_SPACE_EXTRA_PROTRUSION], nSum); // ������+�ܵ�+����(B160)
		strFileData += strData;
	}

	nSum = m_stResult.nDefPerStrip[3][DEF_PINHOLE];
	if (nSum > 0 && pDoc->m_nSapp3Code[SAPP3_PINHOLE] > 0)
	{
		strData.Format(_T("B%d,%d\r\n"), pDoc->m_nSapp3Code[SAPP3_PINHOLE], nSum); // ��Ȧ(B134)
		strFileData += strData;
	}

	nSum = m_stResult.nDefPerStrip[3][DEF_PAD];
	if (nSum > 0 && pDoc->m_nSapp3Code[SAPP3_PAD] > 0)
	{
		strData.Format(_T("B%d,%d\r\n"), pDoc->m_nSapp3Code[SAPP3_PAD], nSum); // �е�(B316)
		strFileData += strData;
	}

	if (nSum > 0 && pDoc->m_nSapp3Code[SAPP3_HOPEN] > 0)
	{
		strData.Format(_T("B%d,%d\r\n"), pDoc->m_nSapp3Code[SAPP3_HOPEN], m_stResult.nDefPerStrip[3][DEF_HOLE_OPEN]);
		strFileData += strData;
	}

	nSum = m_stResult.nDefPerStrip[3][DEF_HOLE_MISS] + m_stResult.nDefPerStrip[3][DEF_HOLE_POSITION] + m_stResult.nDefPerStrip[3][DEF_HOLE_DEFECT];
	if (nSum > 0 && pDoc->m_nSapp3Code[SAPP3_HMISS_HPOS_HBAD] > 0)
	{
		strData.Format(_T("B%d,%d\r\n"), pDoc->m_nSapp3Code[SAPP3_HMISS_HPOS_HBAD], nSum);
		strFileData += strData;
	}

	nSum = m_stResult.nDefPerStrip[3][DEF_VH_OPEN] + m_stResult.nDefPerStrip[3][DEF_VH_MISS] + m_stResult.nDefPerStrip[3][DEF_VH_POSITION] + m_stResult.nDefPerStrip[3][DEF_VH_DEF];
	if (nSum > 0 && pDoc->m_nSapp3Code[SAPP3_VHOPEN_NOVH_VHALIGN_VHDEF] > 0)
	{
		strData.Format(_T("B%d,%d\r\n"), pDoc->m_nSapp3Code[SAPP3_VHOPEN_NOVH_VHALIGN_VHDEF], nSum);
		strFileData += strData;
	}

	// �ӵ�.
	strFileData += _T("\r\nS\r\n");
	strData.Format(_T("%.2f"), m_stResult.dEntireSpeed);
	strFileData += strData;
	strFileData += _T("\r\n");


	return strFileData;
}

CString CReelMap::GetSapp3TxtReverse()
{
	CString strFileData = _T("");
	CString strData;
	int nSum, nStripPcs;
	double dRateBeforeVerify, dRateAfterVerify;
	nStripPcs = m_stResult.nEntirePieceNum / MAX_STRIP_NUM;

	// ���� �̸�.
	strFileData.Format(_T("FileName : %9s_%4s_%5s.txt\r\n\r\n"), m_stResult.sLot, m_stResult.sProcessNum, m_stResult.sMachin);

	// ���� ����/�ϼ�/���� Data.
	strFileData += _T("1Q\r\n");
	dRateBeforeVerify = 100.0 * (nStripPcs - m_stResult.nDefStrip[3]) / nStripPcs;
	dRateAfterVerify = 100.0 * (nStripPcs - m_stResult.nDefStrip[3]) / nStripPcs;
	strData.Format(_T("%d,%d,%.1f,%.1f\r\n"), nStripPcs, nStripPcs - m_stResult.nDefStrip[3], dRateBeforeVerify, dRateAfterVerify); // ���Լ���, �ϼ�����, Verify�� ����, Verify�� ����
	strFileData += strData;

	strFileData += _T("2Q\r\n");
	dRateBeforeVerify = 100.0 * (nStripPcs - m_stResult.nDefStrip[2]) / nStripPcs;
	dRateAfterVerify = 100.0 * (nStripPcs - m_stResult.nDefStrip[2]) / nStripPcs;
	strData.Format(_T("%d,%d,%.1f,%.1f\r\n"), nStripPcs, nStripPcs - m_stResult.nDefStrip[2], dRateBeforeVerify, dRateAfterVerify); // ���Լ���, �ϼ�����, Verify�� ����, Verify�� ����
	strFileData += strData;

	strFileData += _T("3Q\r\n");
	dRateBeforeVerify = 100.0 * (nStripPcs - m_stResult.nDefStrip[1]) / nStripPcs;
	dRateAfterVerify = 100.0 * (nStripPcs - m_stResult.nDefStrip[1]) / nStripPcs;
	strData.Format(_T("%d,%d,%.1f,%.1f\r\n"), nStripPcs, nStripPcs - m_stResult.nDefStrip[1], dRateBeforeVerify, dRateAfterVerify); // ���Լ���, �ϼ�����, Verify�� ����, Verify�� ����
	strFileData += strData;

	strFileData += _T("4Q\r\n");
	dRateBeforeVerify = 100.0 * (nStripPcs - m_stResult.nDefStrip[0]) / nStripPcs;
	dRateAfterVerify = 100.0 * (nStripPcs - m_stResult.nDefStrip[0]) / nStripPcs;
	strData.Format(_T("%d,%d,%.1f,%.1f\r\n"), nStripPcs, nStripPcs - m_stResult.nDefStrip[0], dRateBeforeVerify, dRateAfterVerify); // ���Լ���, �ϼ�����, Verify�� ����, Verify�� ����
	strFileData += strData;


	strFileData += _T("\r\n");


	// ���� �ҷ� Data.
	strFileData += _T("1X\r\n");

	if (m_stResult.nDefPerStrip[3][DEF_OPEN] > 0 && pDoc->m_nSapp3Code[SAPP3_OPEN] > 0)
	{
		strData.Format(_T("B%d,%d\r\n"), pDoc->m_nSapp3Code[SAPP3_OPEN], m_stResult.nDefPerStrip[3][DEF_OPEN]); // ����(B102)
		strFileData += strData;
	}

	nSum = m_stResult.nDefPerStrip[3][DEF_SHORT];// + m_stResult.nDefPerStrip[0][DEF_USHORT];
	if (nSum > 0 && pDoc->m_nSapp3Code[SAPP3_SHORT] > 0)
	{
		strData.Format(_T("B%d,%d\r\n"), pDoc->m_nSapp3Code[SAPP3_SHORT], nSum); // ��Ʈ(B129) // +u��Ʈ
		strFileData += strData;
	}

	nSum = m_stResult.nDefPerStrip[3][DEF_USHORT];
	if (nSum > 0 && pDoc->m_nSapp3Code[SAPP3_USHORT] > 0)
	{
		strData.Format(_T("B%d,%d\r\n"), pDoc->m_nSapp3Code[SAPP3_USHORT], nSum); // u��Ʈ(B314)
		strFileData += strData;
	}

	if (m_stResult.nDefPerStrip[3][DEF_NICK] > 0 && pDoc->m_nSapp3Code[SAPP3_NICK] > 0)
	{
		strData.Format(_T("B%d,%d\r\n"), pDoc->m_nSapp3Code[SAPP3_NICK], m_stResult.nDefPerStrip[3][DEF_NICK]); // ���(B137)
		strFileData += strData;
	}

	nSum = m_stResult.nDefPerStrip[3][DEF_SPACE] + m_stResult.nDefPerStrip[3][DEF_EXTRA] + m_stResult.nDefPerStrip[3][DEF_PROTRUSION];
	if (nSum > 0 && pDoc->m_nSapp3Code[SAPP3_SPACE_EXTRA_PROTRUSION] > 0)
	{
		strData.Format(_T("B%d,%d\r\n"), pDoc->m_nSapp3Code[SAPP3_SPACE_EXTRA_PROTRUSION], nSum); // ������+�ܵ�+����(B160)
		strFileData += strData;
	}

	nSum = m_stResult.nDefPerStrip[3][DEF_PINHOLE];
	if (nSum > 0 && pDoc->m_nSapp3Code[SAPP3_PINHOLE] > 0)
	{
		strData.Format(_T("B%d,%d\r\n"), pDoc->m_nSapp3Code[SAPP3_PINHOLE], nSum); // ��Ȧ(B134)
		strFileData += strData;
	}

	nSum = m_stResult.nDefPerStrip[3][DEF_PAD];
	if (nSum > 0 && pDoc->m_nSapp3Code[SAPP3_PAD] > 0)
	{
		strData.Format(_T("B%d,%d\r\n"), pDoc->m_nSapp3Code[SAPP3_PAD], nSum); // �е�(B316)
		strFileData += strData;
	}

	if (nSum > 0 && pDoc->m_nSapp3Code[SAPP3_HOPEN] > 0)
	{
		strData.Format(_T("B%d,%d\r\n"), pDoc->m_nSapp3Code[SAPP3_HOPEN], m_stResult.nDefPerStrip[3][DEF_HOLE_OPEN]);
		strFileData += strData;
	}
	nSum = m_stResult.nDefPerStrip[3][DEF_HOLE_MISS] + m_stResult.nDefPerStrip[3][DEF_HOLE_POSITION] + m_stResult.nDefPerStrip[3][DEF_HOLE_DEFECT];
	if (nSum > 0 && pDoc->m_nSapp3Code[SAPP3_HMISS_HPOS_HBAD] > 0)
	{
		strData.Format(_T("B%d,%d\r\n"), pDoc->m_nSapp3Code[SAPP3_HMISS_HPOS_HBAD], nSum);
		strFileData += strData;
	}

	nSum = m_stResult.nDefPerStrip[3][DEF_VH_OPEN] + m_stResult.nDefPerStrip[3][DEF_VH_MISS] + m_stResult.nDefPerStrip[3][DEF_VH_POSITION] + m_stResult.nDefPerStrip[3][DEF_VH_DEF];
	if (nSum > 0 && pDoc->m_nSapp3Code[SAPP3_VHOPEN_NOVH_VHALIGN_VHDEF] > 0)
	{
		strData.Format(_T("B%d,%d\r\n"), pDoc->m_nSapp3Code[SAPP3_VHOPEN_NOVH_VHALIGN_VHDEF], nSum);
		strFileData += strData;
	}


	strFileData += _T("2X\r\n");

	if (m_stResult.nDefPerStrip[2][DEF_OPEN] > 0 && pDoc->m_nSapp3Code[SAPP3_OPEN] > 0)
	{
		strData.Format(_T("B%d,%d\r\n"), pDoc->m_nSapp3Code[SAPP3_OPEN], m_stResult.nDefPerStrip[2][DEF_OPEN]); // ����(B102)
		strFileData += strData;
	}

	nSum = m_stResult.nDefPerStrip[2][DEF_SHORT];// + m_stResult.nDefPerStrip[2][DEF_USHORT];
	if (nSum > 0 && pDoc->m_nSapp3Code[SAPP3_SHORT] > 0)
	{
		strData.Format(_T("B%d,%d\r\n"), pDoc->m_nSapp3Code[SAPP3_SHORT], nSum); // ��Ʈ(B129) // +u��Ʈ
		strFileData += strData;
	}

	nSum = m_stResult.nDefPerStrip[2][DEF_USHORT];
	if (nSum > 0 && pDoc->m_nSapp3Code[SAPP3_USHORT] > 0)
	{
		strData.Format(_T("B%d,%d\r\n"), pDoc->m_nSapp3Code[SAPP3_USHORT], nSum); // u��Ʈ(B314)
		strFileData += strData;
	}

	if (m_stResult.nDefPerStrip[2][DEF_NICK] > 0 && pDoc->m_nSapp3Code[SAPP3_NICK] > 0)
	{
		strData.Format(_T("B%d,%d\r\n"), pDoc->m_nSapp3Code[SAPP3_NICK], m_stResult.nDefPerStrip[2][DEF_NICK]); // ���(B137)
		strFileData += strData;
	}

	nSum = m_stResult.nDefPerStrip[2][DEF_SPACE] + m_stResult.nDefPerStrip[2][DEF_EXTRA] + m_stResult.nDefPerStrip[2][DEF_PROTRUSION];
	if (nSum > 0 && pDoc->m_nSapp3Code[SAPP3_SPACE_EXTRA_PROTRUSION] > 0)
	{
		strData.Format(_T("B%d,%d\r\n"), pDoc->m_nSapp3Code[SAPP3_SPACE_EXTRA_PROTRUSION], nSum); // ������+�ܵ�+����(B160)
		strFileData += strData;
	}

	nSum = m_stResult.nDefPerStrip[2][DEF_PINHOLE];
	if (nSum > 0 && pDoc->m_nSapp3Code[SAPP3_PINHOLE] > 0)
	{
		strData.Format(_T("B%d,%d\r\n"), pDoc->m_nSapp3Code[SAPP3_PINHOLE], nSum); // ��Ȧ(B134)
		strFileData += strData;
	}

	nSum = m_stResult.nDefPerStrip[2][DEF_PAD];
	if (nSum > 0 && pDoc->m_nSapp3Code[SAPP3_PAD] > 0)
	{
		strData.Format(_T("B%d,%d\r\n"), pDoc->m_nSapp3Code[SAPP3_PAD], nSum); // �е�(B316)
		strFileData += strData;
	}

	if (nSum > 0 && pDoc->m_nSapp3Code[SAPP3_HOPEN] > 0)
	{
		strData.Format(_T("B%d,%d\r\n"), pDoc->m_nSapp3Code[SAPP3_HOPEN], m_stResult.nDefPerStrip[2][DEF_HOLE_OPEN]);
		strFileData += strData;
	}

	nSum = m_stResult.nDefPerStrip[2][DEF_HOLE_MISS] + m_stResult.nDefPerStrip[2][DEF_HOLE_POSITION] + m_stResult.nDefPerStrip[2][DEF_HOLE_DEFECT];
	if (nSum > 0 && pDoc->m_nSapp3Code[SAPP3_HMISS_HPOS_HBAD] > 0)
	{
		strData.Format(_T("B%d,%d\r\n"), pDoc->m_nSapp3Code[SAPP3_HMISS_HPOS_HBAD], nSum);
		strFileData += strData;
	}

	nSum = m_stResult.nDefPerStrip[2][DEF_VH_OPEN] + m_stResult.nDefPerStrip[2][DEF_VH_MISS] + m_stResult.nDefPerStrip[2][DEF_VH_POSITION] + m_stResult.nDefPerStrip[2][DEF_VH_DEF];
	if (nSum > 0 && pDoc->m_nSapp3Code[SAPP3_VHOPEN_NOVH_VHALIGN_VHDEF] > 0)
	{
		strData.Format(_T("B%d,%d\r\n"), pDoc->m_nSapp3Code[SAPP3_VHOPEN_NOVH_VHALIGN_VHDEF], nSum);
		strFileData += strData;
	}


	strFileData += _T("3X\r\n");

	if (m_stResult.nDefPerStrip[1][DEF_OPEN] > 0 && pDoc->m_nSapp3Code[SAPP3_OPEN] > 0)
	{
		strData.Format(_T("B%d,%d\r\n"), pDoc->m_nSapp3Code[SAPP3_OPEN], m_stResult.nDefPerStrip[1][DEF_OPEN]); // ����(B102)
		strFileData += strData;
	}

	nSum = m_stResult.nDefPerStrip[1][DEF_SHORT];// + m_stResult.nDefPerStrip[1][DEF_USHORT];
	if (nSum > 0 && pDoc->m_nSapp3Code[SAPP3_SHORT] > 0)
	{
		strData.Format(_T("B%d,%d\r\n"), pDoc->m_nSapp3Code[SAPP3_SHORT], nSum); // ��Ʈ(B129) // +u��Ʈ
		strFileData += strData;
	}

	nSum = m_stResult.nDefPerStrip[1][DEF_USHORT];
	if (nSum > 0 && pDoc->m_nSapp3Code[SAPP3_USHORT] > 0)
	{
		strData.Format(_T("B%d,%d\r\n"), pDoc->m_nSapp3Code[SAPP3_USHORT], nSum); // u��Ʈ(B314)
		strFileData += strData;
	}

	if (m_stResult.nDefPerStrip[1][DEF_NICK] > 0 && pDoc->m_nSapp3Code[SAPP3_NICK] > 0)
	{
		strData.Format(_T("B%d,%d\r\n"), pDoc->m_nSapp3Code[SAPP3_NICK], m_stResult.nDefPerStrip[1][DEF_NICK]); // ���(B137)
		strFileData += strData;
	}

	nSum = m_stResult.nDefPerStrip[1][DEF_SPACE] + m_stResult.nDefPerStrip[1][DEF_EXTRA] + m_stResult.nDefPerStrip[1][DEF_PROTRUSION];
	if (nSum > 0 && pDoc->m_nSapp3Code[SAPP3_SPACE_EXTRA_PROTRUSION] > 0)
	{
		strData.Format(_T("B%d,%d\r\n"), pDoc->m_nSapp3Code[SAPP3_SPACE_EXTRA_PROTRUSION], nSum); // ������+�ܵ�+����(B160)
		strFileData += strData;
	}

	nSum = m_stResult.nDefPerStrip[1][DEF_PINHOLE];
	if (nSum > 0 && pDoc->m_nSapp3Code[SAPP3_PINHOLE] > 0)
	{
		strData.Format(_T("B%d,%d\r\n"), pDoc->m_nSapp3Code[SAPP3_PINHOLE], nSum); // ��Ȧ(B134)
		strFileData += strData;
	}

	nSum = m_stResult.nDefPerStrip[1][DEF_PAD];
	if (nSum > 0 && pDoc->m_nSapp3Code[SAPP3_PAD] > 0)
	{
		strData.Format(_T("B%d,%d\r\n"), pDoc->m_nSapp3Code[SAPP3_PAD], nSum); // �е�(B316)
		strFileData += strData;
	}

	if (nSum > 0 && pDoc->m_nSapp3Code[SAPP3_HOPEN] > 0)
	{
		strData.Format(_T("B%d,%d\r\n"), pDoc->m_nSapp3Code[SAPP3_HOPEN], m_stResult.nDefPerStrip[1][DEF_HOLE_OPEN]);
		strFileData += strData;
	}

	nSum = m_stResult.nDefPerStrip[1][DEF_HOLE_MISS] + m_stResult.nDefPerStrip[1][DEF_HOLE_POSITION] + m_stResult.nDefPerStrip[1][DEF_HOLE_DEFECT];
	if (nSum > 0 && pDoc->m_nSapp3Code[SAPP3_HMISS_HPOS_HBAD] > 0)
	{
		strData.Format(_T("B%d,%d\r\n"), pDoc->m_nSapp3Code[SAPP3_HMISS_HPOS_HBAD], nSum);
		strFileData += strData;
	}

	nSum = m_stResult.nDefPerStrip[1][DEF_VH_OPEN] + m_stResult.nDefPerStrip[1][DEF_VH_MISS] + m_stResult.nDefPerStrip[1][DEF_VH_POSITION] + m_stResult.nDefPerStrip[1][DEF_VH_DEF];
	if (nSum > 0 && pDoc->m_nSapp3Code[SAPP3_VHOPEN_NOVH_VHALIGN_VHDEF] > 0)
	{
		strData.Format(_T("B%d,%d\r\n"), pDoc->m_nSapp3Code[SAPP3_VHOPEN_NOVH_VHALIGN_VHDEF], nSum);
		strFileData += strData;
	}


	strFileData += _T("4X\r\n");

	if (m_stResult.nDefPerStrip[0][DEF_OPEN] > 0 && pDoc->m_nSapp3Code[SAPP3_OPEN] > 0)
	{
		strData.Format(_T("B%d,%d\r\n"), pDoc->m_nSapp3Code[SAPP3_OPEN], m_stResult.nDefPerStrip[0][DEF_OPEN]); // ����(B102)
		strFileData += strData;
	}

	nSum = m_stResult.nDefPerStrip[0][DEF_SHORT];// + m_stResult.nDefPerStrip[3][DEF_USHORT];
	if (nSum > 0 && pDoc->m_nSapp3Code[SAPP3_SHORT] > 0)
	{
		strData.Format(_T("B%d,%d\r\n"), pDoc->m_nSapp3Code[SAPP3_SHORT], nSum); // ��Ʈ(B129) // +u��Ʈ
		strFileData += strData;
	}

	nSum = m_stResult.nDefPerStrip[0][DEF_USHORT];
	if (nSum > 0 && pDoc->m_nSapp3Code[SAPP3_USHORT] > 0)
	{
		strData.Format(_T("B%d,%d\r\n"), pDoc->m_nSapp3Code[SAPP3_USHORT], nSum); // u��Ʈ(B314)
		strFileData += strData;
	}

	if (m_stResult.nDefPerStrip[0][DEF_NICK] > 0 && pDoc->m_nSapp3Code[SAPP3_NICK] > 0)
	{
		strData.Format(_T("B%d,%d\r\n"), pDoc->m_nSapp3Code[SAPP3_NICK], m_stResult.nDefPerStrip[0][DEF_NICK]); // ���(B137)
		strFileData += strData;
	}

	nSum = m_stResult.nDefPerStrip[0][DEF_SPACE] + m_stResult.nDefPerStrip[0][DEF_EXTRA] + m_stResult.nDefPerStrip[0][DEF_PROTRUSION];
	if (nSum > 0 && pDoc->m_nSapp3Code[SAPP3_SPACE_EXTRA_PROTRUSION] > 0)
	{
		strData.Format(_T("B%d,%d\r\n"), pDoc->m_nSapp3Code[SAPP3_SPACE_EXTRA_PROTRUSION], nSum); // ������+�ܵ�+����(B160)
		strFileData += strData;
	}

	nSum = m_stResult.nDefPerStrip[0][DEF_PINHOLE];
	if (nSum > 0 && pDoc->m_nSapp3Code[SAPP3_PINHOLE] > 0)
	{
		strData.Format(_T("B%d,%d\r\n"), pDoc->m_nSapp3Code[SAPP3_PINHOLE], nSum); // ��Ȧ(B134)
		strFileData += strData;
	}

	nSum = m_stResult.nDefPerStrip[0][DEF_PAD];
	if (nSum > 0 && pDoc->m_nSapp3Code[SAPP3_PAD] > 0)
	{
		strData.Format(_T("B%d,%d\r\n"), pDoc->m_nSapp3Code[SAPP3_PAD], nSum); // �е�(B316)
		strFileData += strData;
	}

	if (nSum > 0 && pDoc->m_nSapp3Code[SAPP3_HOPEN] > 0)
	{
		strData.Format(_T("B%d,%d\r\n"), pDoc->m_nSapp3Code[SAPP3_HOPEN], m_stResult.nDefPerStrip[0][DEF_HOLE_OPEN]);
		strFileData += strData;
	}

	nSum = m_stResult.nDefPerStrip[0][DEF_HOLE_MISS] + m_stResult.nDefPerStrip[0][DEF_HOLE_POSITION] + m_stResult.nDefPerStrip[0][DEF_HOLE_DEFECT];
	if (nSum > 0 && pDoc->m_nSapp3Code[SAPP3_HMISS_HPOS_HBAD] > 0)
	{
		strData.Format(_T("B%d,%d\r\n"), pDoc->m_nSapp3Code[SAPP3_HMISS_HPOS_HBAD], nSum);
		strFileData += strData;
	}

	nSum = m_stResult.nDefPerStrip[0][DEF_VH_OPEN] + m_stResult.nDefPerStrip[0][DEF_VH_MISS] + m_stResult.nDefPerStrip[0][DEF_VH_POSITION] + m_stResult.nDefPerStrip[0][DEF_VH_DEF];
	if (nSum > 0 && pDoc->m_nSapp3Code[SAPP3_VHOPEN_NOVH_VHALIGN_VHDEF] > 0)
	{
		strData.Format(_T("B%d,%d\r\n"), pDoc->m_nSapp3Code[SAPP3_VHOPEN_NOVH_VHALIGN_VHDEF], nSum);
		strFileData += strData;
	}

	// �ӵ�.
	strFileData += _T("\r\nS\r\n");
	strData.Format(_T("%.2f"), m_stResult.dEntireSpeed);
	strFileData += strData;
	strFileData += _T("\r\n");


	return strFileData;
}

CString CReelMap::GetPath()
{
	return m_sPathBuf;
}

CString CReelMap::GetIpPath()
{
	//return m_sIpPath;
	CString sPath = _T("");
	CString Path[5], str;

	switch (m_nLayer)
	{
	case RMAP_UP:
		str = _T("ReelMapDataUp.txt");
		sPath.Format(_T("%s%s\\%s\\%s\\%s"), pDoc->WorkingInfo.System.sIpPathOldFile,
			pDoc->WorkingInfo.LastJob.sModelUp,
			pDoc->WorkingInfo.LastJob.sLotUp,
			pDoc->WorkingInfo.LastJob.sLayerUp,
			str);
		break;
	case RMAP_ALLUP:
		str = _T("ReelMapDataAll.txt");
		sPath.Format(_T("%s%s\\%s\\%s\\%s"), pDoc->WorkingInfo.System.sIpPathOldFile,
			pDoc->WorkingInfo.LastJob.sModelUp,
			pDoc->WorkingInfo.LastJob.sLotUp,
			pDoc->WorkingInfo.LastJob.sLayerUp,
			str);
		break;
	case RMAP_DN:
		str = _T("ReelMapDataDn.txt");
		sPath.Format(_T("%s%s\\%s\\%s\\%s"), pDoc->WorkingInfo.System.sIpPathOldFile,
			pDoc->WorkingInfo.LastJob.sModelUp,
			pDoc->WorkingInfo.LastJob.sLotUp,
			pDoc->WorkingInfo.LastJob.sLayerDn,
			str);
		break;
	case RMAP_ALLDN:
		str = _T("ReelMapDataAll.txt");
		sPath.Format(_T("%s%s\\%s\\%s\\%s"), pDoc->WorkingInfo.System.sIpPathOldFile,
			pDoc->WorkingInfo.LastJob.sModelUp,
			pDoc->WorkingInfo.LastJob.sLotUp,
			pDoc->WorkingInfo.LastJob.sLayerDn,
			str);
		break;
	case RMAP_INNER_UP:
		str = _T("ReelMapDataUp.txt");
		Path[0] = pDoc->WorkingInfo.System.sIpPathItsFile;
		Path[1] = pDoc->WorkingInfo.LastJob.sModelUp;
		Path[2] = pDoc->WorkingInfo.LastJob.sEngItsCode;
		Path[3] = pDoc->WorkingInfo.LastJob.sInnerLotUp;
		Path[4] = pDoc->WorkingInfo.LastJob.sInnerLayerUp;
		sPath.Format(_T("%s%s\\%s\\%s\\%s\\%s"), Path[0], Path[1], Path[2], Path[3], Path[4], str);
		break;
	case RMAP_INNER_DN:
		str = _T("ReelMapDataDn.txt");
		Path[0] = pDoc->WorkingInfo.System.sIpPathItsFile;
		Path[1] = pDoc->WorkingInfo.LastJob.sModelUp;
		Path[2] = pDoc->WorkingInfo.LastJob.sEngItsCode;
		Path[3] = pDoc->WorkingInfo.LastJob.sInnerLotUp;
		Path[4] = pDoc->WorkingInfo.LastJob.sInnerLayerDn;
		sPath.Format(_T("%s%s\\%s\\%s\\%s\\%s"), Path[0], Path[1], Path[2], Path[3], Path[4], str);
		break;
	case RMAP_INNER_ALLUP:
		str = _T("ReelMapDataAll.txt");
		Path[0] = pDoc->WorkingInfo.System.sIpPathItsFile;
		Path[1] = pDoc->WorkingInfo.LastJob.sModelUp;
		Path[2] = pDoc->WorkingInfo.LastJob.sEngItsCode;
		Path[3] = pDoc->WorkingInfo.LastJob.sInnerLotUp;
		Path[4] = pDoc->WorkingInfo.LastJob.sInnerLayerUp;
		sPath.Format(_T("%s%s\\%s\\%s\\%s\\%s"), Path[0], Path[1], Path[2], Path[3], Path[4], str);
		break;
	case RMAP_INNER_ALLDN:
		str = _T("ReelMapDataAll.txt");
		Path[0] = pDoc->WorkingInfo.System.sIpPathItsFile;
		Path[1] = pDoc->WorkingInfo.LastJob.sModelUp;
		Path[2] = pDoc->WorkingInfo.LastJob.sEngItsCode;
		Path[3] = pDoc->WorkingInfo.LastJob.sInnerLotUp;
		Path[4] = pDoc->WorkingInfo.LastJob.sInnerLayerDn;
		sPath.Format(_T("%s%s\\%s\\%s\\%s\\%s"), Path[0], Path[1], Path[2], Path[3], Path[4], str);
		break;
	case RMAP_INOUTER_UP:
		str = _T("ReelMapDataIO.txt");
		sPath.Format(_T("%s%s\\%s\\%s\\%s\\%s"), pDoc->WorkingInfo.System.sIpPathItsFile,
			pDoc->WorkingInfo.LastJob.sModelUp, pDoc->WorkingInfo.LastJob.sEngItsCode,
			pDoc->WorkingInfo.LastJob.sLotUp, pDoc->WorkingInfo.LastJob.sLayerUp,
			str);
		break;
	case RMAP_INOUTER_DN:
		str = _T("ReelMapDataIO.txt");
		sPath.Format(_T("%s%s\\%s\\%s\\%s\\%s"), pDoc->WorkingInfo.System.sIpPathItsFile,
			pDoc->WorkingInfo.LastJob.sModelUp, pDoc->WorkingInfo.LastJob.sEngItsCode,
			pDoc->WorkingInfo.LastJob.sLotUp, pDoc->WorkingInfo.LastJob.sLayerDn,
			str);
		break;
	case RMAP_ITS:
		pDoc->GetCurrentInfoEng();
		str = _T("ReelMapDataIts.txt");
		sPath.Format(_T("%s%s\\%s\\%s"), pDoc->WorkingInfo.System.sIpPathItsFile,
			pDoc->WorkingInfo.LastJob.sModelUp,
			pDoc->WorkingInfo.LastJob.sEngItsCode,	//pDoc->m_sItsCode,
			str);
		break;
	}

	m_sIpPath = sPath;
	return sPath;
}


void CReelMap::StartThreadFinalCopyItsFiles()
{
	m_bRtnThreadFinalCopyItsFiles = FALSE;
	m_bThreadAliveFinalCopyItsFiles = TRUE;
	m_ThreadTaskFinalCopyItsFiles.Start(GetSafeHwnd(), this, ThreadProcFinalCopyItsFiles);// Start the thread
}

void CReelMap::StopThreadFinalCopyItsFiles()
{
	m_ThreadTaskFinalCopyItsFiles.Stop();// Stop the thread
}

// Home thread body
BOOL CReelMap::ThreadProcFinalCopyItsFiles(LPVOID lpContext)
{
	// Turn the passed in 'this' pointer back into a CProgressMgr instance
	CReelMap* pThread = reinterpret_cast< CReelMap* >(lpContext);
	//	DWORD dwTimePeriod = 10; // 10ms sec sleep

	pThread->m_bRtnThreadFinalCopyItsFiles = TRUE;
	pThread->m_bThreadAliveFinalCopyItsFiles = TRUE;

	//pThread->m_cs.Lock();
	pThread->m_bRtnThreadFinalCopyItsFiles = pThread->FinalCopyItsFiles();
	//pThread->m_cs.Unlock();

	pThread->m_bThreadAliveFinalCopyItsFiles = FALSE;

	return (pThread->m_bRtnThreadFinalCopyItsFiles);
}

BOOL CReelMap::FinalCopyItsFiles()
{
	CString sPathSrc, sPathDest;
	CString sPathSrcDir, sPathDestDir;
	CString sItsFolderSrcPath = pDoc->GetItsFolderPath();
	CString sItsFolderDestPath = pDoc->GetItsTargetFolderPath();

	if (pDoc->GetTestMode() == MODE_INNER)
		sPathSrcDir.Format(_T("%s\\Inner"), sItsFolderSrcPath);
	else if (pDoc->GetTestMode() == MODE_OUTER)
		sPathSrcDir.Format(_T("%s\\Outer"), sItsFolderSrcPath);
	else
		return FALSE;

	CFileFind cFile;
	BOOL bExist = cFile.FindFile(sPathSrcDir + _T("\\*.dat"));
	if (!bExist)
	{
		return FALSE; // dat������ �������� ����.
	}

	CString sFileName;
	int nTot = 0;
	while (bExist)
	{
		bExist = cFile.FindNextFile();
		if (cFile.IsDots()) continue;
		if (!cFile.IsDirectory())
		{
			sFileName = cFile.GetFileName();
			sPathSrc.Format(_T("%s\\%s"), sPathSrcDir, sFileName);
			sPathDest.Format(_T("%s\\%s"), sItsFolderDestPath, sFileName);
			CopyItsFile(sPathSrc, sPathDest);
			nTot++;
		}
		if (!m_bThreadAliveFinalCopyItsFiles)
			break;
	}

	pDoc->DelItsAll(sPathSrcDir);
	return TRUE;
}

BOOL CReelMap::CopyItsFile(CString sPathSrc, CString sPathDest)
{
	BOOL bRtn = FALSE;

	CFileFind cFile;

	if (!cFile.FindFile(sPathSrc))
	{
		return FALSE;
	}

	if(pDoc->m_pFile)
		bRtn = pDoc->m_pFile->Copy(sPathSrc, sPathDest);			// ITS ������ �����Ѵ�.

	return bRtn;
}


int CReelMap::GetRptFixPcs(int nCol, int nRow)
{
	return m_FixPcsRpt[nCol][nRow];;
}



void CReelMap::SetLastSerialOnOffline(int nSerial) 
{
	if (nSerial <= 0)
	{
		pView->ClrDispMsg();
		AfxMessageBox(_T("Serial Error.64"));
		return;
	}

	m_nLastShot = nSerial;
	CString sPath = GetRmapPathOnOffline(m_nLayer);

	if (sPath.IsEmpty())
		return;

	CString sData;
	sData.Format(_T("%d"), nSerial);
	::WritePrivateProfileString(_T("Info"), _T("Marked Shot"), sData, sPath);

	int nYear, nMonth, nDay, nHour, nMin, nSec;
	nYear = pDoc->WorkingInfo.Lot.CurTime.nYear;
	nMonth = pDoc->WorkingInfo.Lot.CurTime.nMonth;
	nDay = pDoc->WorkingInfo.Lot.CurTime.nDay;
	nHour = pDoc->WorkingInfo.Lot.CurTime.nHour;
	nMin = pDoc->WorkingInfo.Lot.CurTime.nMin;
	nSec = pDoc->WorkingInfo.Lot.CurTime.nSec;

	if (!nYear && !nMonth && !nDay && !nHour && !nMin && !nSec)
		sData = _T("");
	else
		sData.Format(_T("%04d-%02d-%02d, %02d:%02d:%02d"), nYear, nMonth, nDay, nHour, nMin, nSec);
	::WritePrivateProfileString(_T("Info"), _T("Marked Date"), sData, sPath);
}

CString CReelMap::GetRmapPathOnOffline(int nRmap)
{
	CString sPath = _T("");
	CString Path[5], str;

	if (IsOfflineFolder())
	{
		switch (nRmap)
		{
		case RMAP_UP:
			str = _T("ReelMapDataUp.txt");
			sPath.Format(_T("%s%s\\%s\\%s\\OFFLINE\\%s"),
				pDoc->WorkingInfo.System.sPathOldFile,
				pDoc->WorkingInfo.LastJob.sModelUp,
				pDoc->WorkingInfo.LastJob.sLotUp,
				pDoc->WorkingInfo.LastJob.sLayerUp,
				str);
			break;
		case RMAP_DN:
			str = _T("ReelMapDataDn.txt");
			sPath.Format(_T("%s%s\\%s\\%s\\OFFLINE\\%s"),
				pDoc->WorkingInfo.System.sPathOldFile,
				pDoc->WorkingInfo.LastJob.sModelUp,
				pDoc->WorkingInfo.LastJob.sLotUp,
				pDoc->WorkingInfo.LastJob.sLayerDn,
				str);
			break;
		case RMAP_ALLUP:
			str = _T("ReelMapDataAll.txt");
			sPath.Format(_T("%s%s\\%s\\%s\\OFFLINE\\%s"),
				pDoc->WorkingInfo.System.sPathOldFile,
				pDoc->WorkingInfo.LastJob.sModelUp,
				pDoc->WorkingInfo.LastJob.sLotUp,
				pDoc->WorkingInfo.LastJob.sLayerUp,
				str);
			break;
		case RMAP_ALLDN:
			str = _T("ReelMapDataAll.txt");
			sPath.Format(_T("%s%s\\%s\\%s\\OFFLINE\\%s"),
				pDoc->WorkingInfo.System.sPathOldFile,
				pDoc->WorkingInfo.LastJob.sModelUp,
				pDoc->WorkingInfo.LastJob.sLotUp,
				pDoc->WorkingInfo.LastJob.sLayerDn,
				str);
			break;
		}
	}
	else
	{
		switch (nRmap)
		{
		case RMAP_UP:
			str = _T("ReelMapDataUp.txt");
			sPath.Format(_T("%s%s\\%s\\%s\\%s"),
				pDoc->WorkingInfo.System.sPathOldFile,
				pDoc->WorkingInfo.LastJob.sModelUp,
				pDoc->WorkingInfo.LastJob.sLotUp,
				pDoc->WorkingInfo.LastJob.sLayerUp,
				str);
			break;
		case RMAP_DN:
			str = _T("ReelMapDataDn.txt");
			sPath.Format(_T("%s%s\\%s\\%s\\%s"),
				pDoc->WorkingInfo.System.sPathOldFile,
				pDoc->WorkingInfo.LastJob.sModelUp,
				pDoc->WorkingInfo.LastJob.sLotUp,
				pDoc->WorkingInfo.LastJob.sLayerDn,
				str);
			break;
		case RMAP_ALLUP:
			str = _T("ReelMapDataAll.txt");
			sPath.Format(_T("%s%s\\%s\\%s\\%s"),
				pDoc->WorkingInfo.System.sPathOldFile,
				pDoc->WorkingInfo.LastJob.sModelUp,
				pDoc->WorkingInfo.LastJob.sLotUp,
				pDoc->WorkingInfo.LastJob.sLayerUp,
				str);
			break;
		case RMAP_ALLDN:
			str = _T("ReelMapDataAll.txt");
			sPath.Format(_T("%s%s\\%s\\%s\\%s"),
				pDoc->WorkingInfo.System.sPathOldFile,
				pDoc->WorkingInfo.LastJob.sModelUp,
				pDoc->WorkingInfo.LastJob.sLotUp,
				pDoc->WorkingInfo.LastJob.sLayerDn,
				str);
			break;
		}
	}

	return sPath;
}

int CReelMap::IsOfflineFolder() // 0 : Not exist, 1 : Exist only Up, 2 : Exist only Dn, 3 : Exist Up and Dn
{
	int nRtn = 0;
	CString sPath, str;
	BOOL bWorking;

	//�˻� Ŭ����
	CFileFind finder;

	str = _T("OFFLINE");
	sPath.Format(_T("%s%s\\%s\\%s\\%s\\*.*"), pDoc->WorkingInfo.System.sPathOldFile,
		pDoc->WorkingInfo.LastJob.sModelUp,
		pDoc->WorkingInfo.LastJob.sLotUp,
		pDoc->WorkingInfo.LastJob.sLayerUp,
		str);

	//CFileFind�� ����, ���͸��� �����ϸ� TRUE �� ��ȯ�� 
	bWorking = finder.FindFile(sPath);

	if (bWorking)
		nRtn |= 0x01;

	str = _T("OFFLINE");
	sPath.Format(_T("%s%s\\%s\\%s\\%s\\*.*"), pDoc->WorkingInfo.System.sPathOldFile,
		pDoc->WorkingInfo.LastJob.sModelDn,
		pDoc->WorkingInfo.LastJob.sLotDn,
		pDoc->WorkingInfo.LastJob.sLayerDn,
		str);

	//CFileFind�� ����, ���͸��� �����ϸ� TRUE �� ��ȯ�� 
	bWorking = finder.FindFile(sPath);

	if (bWorking)
		nRtn |= 0x02;

	return nRtn;
}

BOOL CReelMap::WriteOnOffline(int nSerial)
{
	if (nSerial <= 0)
	{
		pView->ClrDispMsg();
		AfxMessageBox(_T("Serial Error.162"));
		return FALSE;
	}

	int nLayer = RMAP_NONE;

	// ������� ===================================================================================================
	int nStripIdx, nDefIdx, nDefA[MAX_DEF], nDefStrip[MAX_STRIP], nDefPerStrip[MAX_STRIP][MAX_DEF], nStripOut[MAX_STRIP];

	for (int nS = 0; nS < MAX_STRIP; nS++)
	{
		nDefStrip[nS] = 0;
		nStripOut[nS] = 0;
	}

	for (int nD = 0; nD < MAX_DEF; nD++)
	{
		nDefA[nD] = 0;

		for (int nS1 = 0; nS1 < MAX_STRIP; nS1++)
		{
			nDefPerStrip[nS1][nD] = 0;
		}
	}
	// ===========================================================================================================


	CString sPath = GetRmapPathOnOffline(m_nLayer);
	MakeHeader(sPath);

	int nIdx = pDoc->GetPcrIdx1(nSerial, pDoc->m_bNewLotShare[1]);
	int nNodeX = pDoc->m_Master[0].m_pPcsRgn->nCol;
	int nNodeY = pDoc->m_Master[0].m_pPcsRgn->nRow;
	int nStripY = pDoc->m_Master[0].m_pPcsRgn->nRow / MAX_STRIP; // Strip(1~4);
	int nTotDefPcs = 0;

	if (m_nLayer == RMAP_UP || m_nLayer == RMAP_DN || m_nLayer == RMAP_ALLUP || m_nLayer == RMAP_ALLDN)
	{
		nLayer = m_nLayer - RMAP_UP;

		if (pDoc->m_pPcr[nLayer])
		{
			if (pDoc->m_pPcr[nLayer][nIdx])
				nTotDefPcs = pDoc->m_pPcr[nLayer][nIdx]->m_nTotDef;
		}
	}
	else if (m_nLayer == RMAP_INNER_UP || m_nLayer == RMAP_INNER_DN || m_nLayer == RMAP_INNER_ALLUP || m_nLayer == RMAP_INNER_ALLDN)
	{
		return FALSE;
		//nLayer = m_nLayer - RMAP_INNER_UP;

		//if (pDoc->m_pPcrInner[nLayer])
		//{
		//	if (pDoc->m_pPcrInner[nLayer][nIdx])
		//		nTotDefPcs = pDoc->m_pPcrInner[nLayer][nIdx]->m_nTotDef;
		//}
	}
	else if (m_nLayer == RMAP_ITS)
	{
		return FALSE;
		//if (pDoc->m_pPcrIts)
		//{
		//	if (pDoc->m_pPcrIts[nIdx])
		//		nTotDefPcs = pDoc->m_pPcrIts[nIdx]->m_nTotDef;
		//}
	}
	else
		return FALSE;

	short **pPnlBuf;
	int i, nC, nR, nPcsId, nDefCode;//, nTot, nDef, nGood;
	pPnlBuf = new short*[nNodeY];
	for (i = 0; i < nNodeY; i++)
	{
		pPnlBuf[i] = new short[nNodeX];
		memset(pPnlBuf[i], 0, sizeof(short)*nNodeX);
		memset(m_pPnlBuf[nSerial - 1][i], 0, sizeof(short)*nNodeX);
	}

	CString strData, strTemp;
	int nTotVerifyed = 0;

	for (i = 0; i < nTotDefPcs; i++)
	{
		nLayer = m_nLayer - RMAP_UP;

		if (pDoc->m_pPcr[nLayer][nIdx]->m_pMk[i] != -2) // -2 (NoMarking)
		{
			if (pDoc->WorkingInfo.System.bStripPcsRgnBin)	// DTS��
			{
				switch (pDoc->m_Master[0].MasterInfo.nActionCode)	// 0 : Rotation / Mirror ���� ����(CAM Data ����), 1 : �¿� �̷�, 2 : ���� �̷�, 3 : 180 ȸ��, 4 : 270 ȸ��(CCW), 5 : 90 ȸ��(CW)
				{
				case 0:
					nPcsId = pDoc->m_pPcr[nLayer][nIdx]->m_pDefPcs[i];
					break;
				case 1:
					nPcsId = pDoc->MirrorLR(pDoc->m_pPcr[nLayer][nIdx]->m_pDefPcs[i]);
					break;
				case 2:
					nPcsId = pDoc->MirrorUD(pDoc->m_pPcr[nLayer][nIdx]->m_pDefPcs[i]);
					break;
				case 3:
					nPcsId = pDoc->Rotate180(pDoc->m_pPcr[nLayer][nIdx]->m_pDefPcs[i]);
					break;
				default:
					nPcsId = pDoc->m_pPcr[nLayer][nIdx]->m_pDefPcs[i];
					break;
				}
			}
			else
				nPcsId = pDoc->m_pPcr[nLayer][nIdx]->m_pDefPcs[i];

			nDefCode = pDoc->m_pPcr[nLayer][nIdx]->m_pDefType[i];

			nC = int(nPcsId / nNodeY);
			if (nC % 2)	// Ȧ��.
				nR = nNodeY * (nC + 1) - nPcsId - 1;
			else		// ¦��.
				nR = nPcsId - nNodeY * nC;
			pPnlBuf[nR][nC] = (short)nDefCode;	// nPnl�� �� ����.
			if (m_pPnlBuf)
				m_pPnlBuf[nSerial - 1][nR][nC] = pPnlBuf[nR][nC]; // DefCode 3D Array : [nSerial][nRow][nCol] - �������� ������.

			// ������� ======================================================
			if (nDefCode > 0)
			{
				nDefA[nDefCode]++;
				m_stYield.nDefA[nDefCode]++;

				nStripIdx = int(nR / (nNodeY / MAX_STRIP));
				if (nStripIdx > -1 && nStripIdx < MAX_STRIP)
				{
					nDefStrip[nStripIdx]++;
					nDefPerStrip[nStripIdx][nDefCode]++;
				}
			}
			// ===============================================================
		}
		else
		{
			nTotVerifyed++;
		}
	}

	CString sPnl, sRow;
	i = 0;
	sPnl.Format(_T("%d"), nSerial);
	strData.Format(_T("%d"), nTotDefPcs - nTotVerifyed);
	::WritePrivateProfileString(sPnl, _T("Total Defects"), strData, sPath);

	for (int nRow = 0; nRow < nNodeX; nRow++)			// ���� Text(90�� �ð�������� ȸ���� ���) ���� Row : Shot�� ù��° Col���� �����ؼ� ������ ������.
	{
		sRow.Format(_T("%02d"), nRow);
		strData.Format(_T(""));
		strTemp.Format(_T(""));

		for (int nCol = 0; nCol < nNodeY; nCol++)		// ���� Text(90�� �ð�������� ȸ���� ���) ���� Col : 4�� 3�� 2�� 1�� ��Ʈ������ ǥ�õ�.
		{
			nR = (nNodeY - 1) - nCol;				// ���ʻ��� Row
			nC = nRow;							// ���ʻ��� Col

			nLayer = m_nLayer - RMAP_UP;
			if (pDoc->m_pPcr[nLayer][nIdx]->m_nErrPnl == -1 || pDoc->m_pPcr[nLayer][nIdx]->m_nErrPnl == -2)
			{
				nDefCode = DEF_LIGHT;
				m_pPnlBuf[nSerial - 1][nR][nC] = (short)nDefCode;
			}
			else
				nDefCode = (int)pPnlBuf[nR][nC] < 0 ? 0 : (int)pPnlBuf[nR][nC];


			strTemp.Format(_T("%2d,"), nDefCode);	// �ҷ��ڵ带 2ĭ���� ����

			if (!nCol)								// strData�� ó������ �����͸� �߰�
				strData.Insert(0, strTemp);
			else
			{
				int nLen = strData.GetLength();
				if (!(nCol%nStripY))				// Separate Strip (��Ʈ�� ����)
				{
					strData.Insert(nLen, _T("  "));
					nLen = strData.GetLength();
				}
				strData.Insert(nLen, strTemp);
			}
		}

		int nPos = strData.ReverseFind(',');		// ���� Text �� ������ ','�� ����
		strData.Delete(nPos, 1);
		::WritePrivateProfileString(sPnl, sRow, strData, sPath); // �� ���ξ� ���� Text�� ���.
																 //::WritePrivateProfileString(sPnl, sRow, strData, m_sPathShare); // �� ���ξ� ���� Text�� ���.
	}	// ���� Text(90�� �ð�������� ȸ���� ���) ���� Row : Shot�� ������ Col���� ����ϰ� ����.


	// ������� ======================================================
	int nTot = nNodeX * nNodeY;
	int nDef = nTotDefPcs - nTotVerifyed;
	int nGood = nTot - nDef;

	m_stYield.nTot = m_stYield.nTot + nTot;
	m_stYield.nGood = m_stYield.nGood + nGood;
	m_stYield.nDef = m_stYield.nDef + nDef;

	double dStOutRto = _tstof(pDoc->WorkingInfo.LastJob.sStripOutRatio) / 100.0;

	int nTotSriptOut = 0;
	for (nStripIdx = 0; nStripIdx < MAX_STRIP; nStripIdx++)
	{
		if (nDefStrip[nStripIdx] >= nStripY * dStOutRto)
		{
			nStripOut[nStripIdx]++;
			nTotSriptOut++;
		}
		m_stYield.nDefStrip[nStripIdx] += nDefStrip[nStripIdx];
		m_stYield.nStripOut[nStripIdx] += nStripOut[nStripIdx];
		for (nDefIdx = 1; nDefIdx < MAX_DEF; nDefIdx++)
			m_stYield.nDefPerStrip[nStripIdx][nDefIdx] += nDefPerStrip[nStripIdx][nDefIdx];
	}	
	m_stYield.nTotSriptOut += nTotSriptOut;
	// ===============================================================

	for (i = 0; i < nNodeY; i++)
		delete[]  pPnlBuf[i];
	delete[] pPnlBuf;

	//m_nWritedSerial = nSerial;

	return TRUE;
}

BOOL CReelMap::GetLastRmapInfo()
{
	CFileFind findfile;
	CString sVal, sMsg, sPath;
	sPath = GetRmapPath(m_nLayer);

	if (!findfile.FindFile(sPath))
	{
		sMsg.Format(_T("Reelmap�� �������� �ʽ��ϴ�.\r\n%s"), sPath);
		pView->MsgBox(sMsg);
		return FALSE;
	}

	CString sProcessCode, sEntireSpeed, sLotStart, sLotRun, sLotEnd;
	int nLastShot, nCompletedShot;
	TCHAR szData[MAX_PATH];

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

	if (0 < ::GetPrivateProfileString(_T("Info"), _T("Lot Start"), NULL, szData, sizeof(szData), sPath))
		sLotStart = CString(szData);
	else
		sLotStart = _T("");

	if (0 < ::GetPrivateProfileString(_T("Info"), _T("Lot Run"), NULL, szData, sizeof(szData), sPath))
		sLotRun = CString(szData);
	else
		sLotRun = _T("");

	if (0 < ::GetPrivateProfileString(_T("Info"), _T("Lot End"), NULL, szData, sizeof(szData), sPath))
		sLotEnd = CString(szData);
	else
		sLotEnd = _T("");

	m_stRmapInfo.sProcessCode = sProcessCode;
	m_stRmapInfo.sEntireSpeed = sEntireSpeed;
	m_stRmapInfo.sLotStart = sLotStart;
	m_stRmapInfo.sLotRun = sLotRun;
	m_stRmapInfo.sLotEnd = sLotEnd;
	m_stRmapInfo.nLastShot = nLastShot;
	m_stRmapInfo.nCompletedShot = nCompletedShot;

	return TRUE;
}

BOOL CReelMap::WriteLastRmapInfoOnOffline()
{
	CFileFind findfile;

	CString sVal, sMsg, sPath;

	CString sProcessCode, sEntireSpeed, sLotStart, sLotRun, sLotEnd;
	int nLastShot, nCompletedShot;
	TCHAR szData[MAX_PATH];

	if (!pView->m_bOnLine)
	{
		sPath = GetRmapPath(m_nLayer);
		if (!findfile.FindFile(sPath))
		{
			sMsg.Format(_T("Reelmap�� �������� �ʽ��ϴ�.\r\n%s"), sPath);
			pView->MsgBox(sMsg);
			return FALSE;
		}

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

		if (0 < ::GetPrivateProfileString(_T("Info"), _T("Lot Start"), NULL, szData, sizeof(szData), sPath))
			sLotStart = CString(szData);
		else
			sLotStart = _T("");

		if (0 < ::GetPrivateProfileString(_T("Info"), _T("Lot Run"), NULL, szData, sizeof(szData), sPath))
			sLotRun = CString(szData);
		else
			sLotRun = _T("");

		if (0 < ::GetPrivateProfileString(_T("Info"), _T("Lot End"), NULL, szData, sizeof(szData), sPath))
			sLotEnd = CString(szData);
		else
			sLotEnd = _T("");
	}
	else
	{
		sProcessCode = m_stRmapInfo.sProcessCode;
		sEntireSpeed = m_stRmapInfo.sEntireSpeed;
		sLotStart = m_stRmapInfo.sLotStart;
		sLotRun = m_stRmapInfo.sLotRun;
		sLotEnd = m_stRmapInfo.sLotEnd;
		nLastShot = m_stRmapInfo.nLastShot;
		nCompletedShot = m_stRmapInfo.nCompletedShot;
		if (!nCompletedShot && nLastShot) nCompletedShot = nLastShot;
		else if (nCompletedShot && !nLastShot) nLastShot = nCompletedShot;
	}

	int k, i;
	CString strMenu, strItem, sCode, sDefNum, strData;

	sPath = GetRmapPathOnOffline(m_nLayer);
	MakeHeader(sPath);

	// ���� ���� ===================================================================
	strData.Format(_T("%d"), m_stYield.nTot);
	::WritePrivateProfileString(_T("Info"), _T("Total Pcs"), strData, sPath);
	strData.Format(_T("%d"), m_stYield.nGood);
	::WritePrivateProfileString(_T("Info"), _T("Good Pcs"), strData, sPath);
	strData.Format(_T("%d"), m_stYield.nDef);
	::WritePrivateProfileString(_T("Info"), _T("Bad Pcs"), strData, sPath);

	for (i = 1; i < MAX_DEF; i++)
	{
		m_nDef[i] = m_stYield.nDefA[i];
		sCode.Format(_T("%d"), i);
		sDefNum.Format(_T("%d"), m_nDef[i]);
		::WritePrivateProfileString(_T("Info"), sCode, sDefNum, sPath);
	}

	int nTotStOut = 0;
	for (k = 0; k < MAX_STRIP; k++)
	{
		strItem.Format(_T("Strip%d"), k);
		m_nDefStrip[k] = m_stYield.nDefStrip[k];
		strData.Format(_T("%d"), m_nDefStrip[k]);
		::WritePrivateProfileString(_T("Info"), strItem, strData, sPath);

		strItem.Format(_T("%d"), k);
		m_nStripOut[k] = m_stYield.nStripOut[k];
		strData.Format(_T("%d"), m_nStripOut[k]);
		::WritePrivateProfileString(_T("StripOut"), strItem, strData, sPath);

		for (i = 1; i < MAX_DEF; i++)
		{
			strMenu.Format(_T("Strip%d"), k);
			strItem.Format(_T("%d"), i);
			m_nDefPerStrip[k][i] = m_stYield.nDefPerStrip[k][i];
			strData.Format(_T("%d"), m_nDefPerStrip[k][i]);
			::WritePrivateProfileString(strMenu, strItem, strData, sPath);
		}
	}
	strData.Format(_T("%d"), m_stYield.nTotSriptOut);
	::WritePrivateProfileString(_T("StripOut"), _T("Total"), strData, sPath);

	//=====================================================================================

	::WritePrivateProfileString(_T("Info"), _T("Process Code"), sProcessCode, sPath);
	::WritePrivateProfileString(_T("Info"), _T("Entire Speed"), sEntireSpeed, sPath);
	::WritePrivateProfileString(_T("Info"), _T("Lot Start"), sLotStart, sPath);
	::WritePrivateProfileString(_T("Info"), _T("Lot Run"), sLotRun, sPath);
	::WritePrivateProfileString(_T("Info"), _T("Lot End"), sLotEnd, sPath);
	sVal.Format(_T("%d"), GetFirstShotFromPcr());
	::WritePrivateProfileString(_T("Info"), _T("Start Serial"), sVal, sPath);
	sVal.Format(_T("%d"), GetLastShotFromPcr());
	::WritePrivateProfileString(_T("Info"), _T("End Serial"), sVal, sPath);
	::WritePrivateProfileString(_T("Info"), _T("Completed Shot"), sVal, sPath);
	//::WritePrivateProfileString(_T("Info"), _T("Marked Shot"), sVal, sPath);

	return TRUE;
}


int CReelMap::GetFirstShotFromPcr()
{
	CString sPath;
	CString sFileN, sRmapPath;

	sRmapPath = GetRmapPathOnOffline(m_nLayer);
	int nPos = sRmapPath.ReverseFind('\\');
	if (nPos != -1)
	{
		sFileN = sRmapPath.Right(sRmapPath.GetLength() - nPos - 1);
		sRmapPath.Delete(nPos, sRmapPath.GetLength() - nPos);
	}

	sPath = sRmapPath + _T("\\*.pcr");

	// �˻��� ��� �� ����
	// Ư�� Ȯ���ڸ� ���� ������ �˻��ϰ� ������ '���/*.pcr' ������ ���

	// �˻� Ŭ����
	CFileFind finder;

	// CFileFind�� ����, ���͸��� �����ϸ� TRUE �� ��ȯ��
	BOOL bWorking = finder.FindFile(sPath);

	CString fileName;
	CString DirName;
	int nSerial = 0, nFirstShot = 100000;

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

		if (nSerial < nFirstShot)
			nFirstShot = nSerial;
	}

	return nFirstShot;
}

int CReelMap::GetLastShotFromPcr()
{
	CString sPath;
	CString sFileN, sRmapPath;

	sRmapPath = GetRmapPathOnOffline(m_nLayer);
	int nPos = sRmapPath.ReverseFind('\\');
	if (nPos != -1)
	{
		sFileN = sRmapPath.Right(sRmapPath.GetLength() - nPos - 1);
		sRmapPath.Delete(nPos, sRmapPath.GetLength() - nPos);
	}

	sPath = sRmapPath + _T("\\*.pcr");

	// �˻��� ��� �� ����
	// Ư�� Ȯ���ڸ� ���� ������ �˻��ϰ� ������ '���/*.pcr' ������ ���

	// �˻� Ŭ����
	CFileFind finder;

	// CFileFind�� ����, ���͸��� �����ϸ� TRUE �� ��ȯ��
	BOOL bWorking = finder.FindFile(sPath);

	CString fileName;
	CString DirName;
	int nSerial = 0, nLastShot = 0;

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
