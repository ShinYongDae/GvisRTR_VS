#if !defined(AFX_REELMAP_H__F5AB69D1_0DF4_4D15_985A_1421D64577D1__INCLUDED_)
#define AFX_REELMAP_H__F5AB69D1_0DF4_4D15_985A_1421D64577D1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ReelMap.h : header file
//

#define MAX_DEFINFO		12
// #define MAX_PCS			10000

#include "ThreadTask.h"
#include "../Global/GlobalDefine.h"

#define FIX_PCS_SHOT_MAX	500
#define FIX_PCS_COL_MAX		100
#define FIX_PCS_ROW_MAX		100

#pragma warning(disable: 4244)
#pragma warning(disable: 4996)
#pragma warning(disable: 4477)



struct stResult
{
	CString sMachin, sOpName, sProcessNum;
	CString sModel, sLot, sLayerUp, sLayerDn;
	CString strLotStartTime, strLotEndTime, strLotWorkingTime;
	int nEntirePieceNum, nEntireStripNum;
	int nEntireStripDef[MAX_STRIP];
	int nGoodPieceNum, nDefectPieceNum;
	int nDefStrip[MAX_STRIP], nDefPerStrip[MAX_STRIP][MAX_DEF];
	int nEntireAddedDefect[MAX_DEF];
	int nSerialSt, nSerialEd;
	int nStripOut[MAX_STRIP], nTotStOut;
	double dEntireSpeed;

	stResult()
	{
		sMachin = _T(""); sOpName = _T(""); sProcessNum = _T("");
		sModel = _T(""); sLot = _T(""); sLayerUp = _T(""); sLayerDn = _T("");
		strLotStartTime = _T(""); strLotEndTime = _T(""); strLotWorkingTime = _T("");
		nEntirePieceNum = 0; nEntireStripNum = 0;
		nGoodPieceNum = 0; nDefectPieceNum = 0;
		nSerialSt = 0; nSerialEd = 0;
		nTotStOut = 0;
		dEntireSpeed = 0.0;

		for (int nStrip = 0; nStrip < MAX_STRIP; nStrip++)
		{
			nEntireStripDef[nStrip] = 0;
			nDefStrip[nStrip] = 0;
			nStripOut[nStrip] = 0;

			for (int nDef = 0; nDef < MAX_DEF; nDef++)
			{
				nDefPerStrip[nStrip][nDef] = 0;
				if(!nStrip)
					nEntireAddedDefect[nDef] = 0;
			}
		}
	}
};


/////////////////////////////////////////////////////////////////////////////
// CReelMap window

class CReelMap : public CWnd
{
	BOOL m_FixPcs[FIX_PCS_SHOT_MAX][FIX_PCS_COL_MAX][FIX_PCS_ROW_MAX]; // [Col][Row]
	int m_FixPcsPrev[FIX_PCS_COL_MAX][FIX_PCS_ROW_MAX]; // [Col][Row]
	int m_FixPcsPrevStSerial[FIX_PCS_COL_MAX][FIX_PCS_ROW_MAX]; // [Col][Row]
	int m_FixPcsRpt[FIX_PCS_COL_MAX][FIX_PCS_ROW_MAX]; // [Col][Row]
	int m_nPrevSerial[2]; // [0] : -- , [1] : ++
	int m_nPnlBuf;															// �޸𸮿� �Ҵ�� �� Shot��
	short ***m_pPnlBuf;	// DefCode 3D Array : [nSerial-1][nRow][nCol] on File -> [nSerial-1][NodeX][NodeY] : Rotated Cw 90 
	int m_nTotPcs, m_nGoodPcs, m_nBadPcs, m_nDef[MAX_DEF];	// [DefCode] : Total Num.
	int m_nDefStrip[MAX_STRIP_NUM], m_nDefPerStrip[MAX_STRIP_NUM][MAX_DEF];
	int m_nStripOut[MAX_STRIP_NUM], m_nTotStOut;
	CString m_sPathShare, m_sPathBuf, m_sIpPath;
	CString m_sPathYield;
	//double m_dAdjRatio; // Master Image�� Pixel �ػ󵵿� ���� Reelmap������ �ĺ��� ���� ����.
	int m_nIdxDefInfo;	// MAX_DEFINFO�� ���� ������ Index.
	int m_nWritedSerial; // In Share folder Serial.
	BOOL m_bContFixDef;
	int m_nBeforeSerial;

	//int m_nCntFixPcs;

	void LoadConfig();
	BOOL MakeDir();
	BOOL MakeDir(CString sModel, CString sLayer, CString sLot);
	BOOL MakeDirRmap();
	BOOL MakeDirRmap(CString sModel, CString sLayer, CString sLot);
	CString MakeDirRmapRestore();
	CString MakeDirRmapRestore(CString sModel, CString sLayer, CString sLot);
	int GetLastRmapRestoreDir(CString strPath);

	char* StrToChar(CString str);
	void StrToChar(CString str, char* pCh);
	
	BOOL ReadYield(int nSerial, CString sPath);
	BOOL WriteYield(int nSerial, CString sPath);
	
	BOOL LoadDefectTableIni();
	BOOL LoadDefectTableDB();

	BOOL MakeDirYield(CString sPath);

// Construction
public:
	CReelMap(int nLayer, int nPnl=0, int nPcs=0, int nDir=0);

// Attributes
public:
	CCriticalSection m_cs;

	double m_dAdjRatio; // Master Image�� Pixel �ػ󵵿� ���� Reelmap������ �ĺ��� ���� ����.
	int m_nLayer;
	CString m_sMc, m_sUser;
// 	CString m_sModel, m_sLayer, m_sLot;
	int m_nSerial; // On marking Serial.

	int nTotPnl;
	int nTotPcs;
	int nDir;

	int *m_pPnlNum, *m_pPnlDefNum;
	CRect *pFrmRgn;
	CRect **pPcsRgn;

	int **pPcsDef; // [DispPnlIdx][PcsID] : �ҷ��ڵ�.
	CString m_sKorDef[MAX_DEF], m_sEngDef[MAX_DEF];
	char m_cBigDef[MAX_DEF], m_cSmallDef[MAX_DEF];
	COLORREF m_rgbDef[MAX_DEF];
	int m_nOdr[MAX_DEF];
	int m_nBkColor[3]; //RGB

// 	CString *pMkInfo;

	int m_nSelMarkingPnl;
	double m_dTotLen, m_dPnlLen, m_dLotLen, m_dTempPauseLen, m_dLotCutPosLen;
	BOOL m_bUseLotSep, m_bUseTempPause;
	int m_nLastShot, m_nCompletedShot;
	double m_dProgressRatio;

	stYield m_stYield;
	int m_nStartSerial;

// Operations
public:
	void SetRgbDef(int nDef, COLORREF rgbVal);
	void SetPnlNum(int *pPnlNum=NULL);
	void IncPnlNum();
	void AddPnlNum(int nNum);
	void SelMarkingPnl(int nNum);
	void SetAdjRatio(double dRatio);
	double GetAdjRatio();

	CString GetLotSt();
	CString GetLotEd();

	BOOL Open(CString sPath);
	BOOL OpenUser(CString sPath);
	BOOL Open();
	BOOL OpenUser(CString sPath, CString sModel, CString sLayer, CString sLot);
	//int Read(CString &sRead);
	BOOL Write(int nSerial);
	BOOL Disp(int nMkPnl, BOOL bDumy=FALSE);
	void SetLastSerial(int nSerial);
	void SetCompletedSerial(int nSerial);
	void SetLotSt();
	void SetLotEd();
	BOOL InitRst();
	void CloseRst();
	void ClrRst();
// 	BOOL SetRst(int nSerial);
	BOOL GetRst(int nFrom, int nTo);
	BOOL GetRst(int nSerial);
	int GetDefNum(int nDefCode);
	int GetDefStrip(int nStrip);
	int GetDefStrip(int nStrip, int nDefCode);
	void GetPcsNum(int &nGood, int &nBad);
	void ClrPnlNum();
	void Clear();
	int GetLastSerial();
	void InitPcs();
	void ClrPcs();
//	void ResetReelmap();

	void ClrFixPcs();
	void ClrFixPcs(int nCol, int nRow);
	void SetFixPcs(int nSerial, int nCol, int nRow); // nShot : 0 ~ 
	BOOL IsFixPcs(int nSerial, int &Col, int &Row);
	BOOL IsFixPcs(int nSerial, int* pCol, int* pRow, int &nTot, BOOL &bCont);
	int GetRptFixPcs(int nCol, int nRow);

	//BOOL Write(int nSerial, int nLayer, CString sPath);
	void SetPathAtBuf();
	CString GetRmapPath(int nRmap);
	CString GetYieldPath(int nRmap);

	void SetPnlDefNum(int *pPnlDefNum);
	void ClrPnlDefNum();
	void AddPnlDefNum(int nDef);
	void UpdateTotVel(CString sVel, int nLayer);
	void UpdateProcessNum(CString sProcessNum, int nLayer);
	int GetStripOut(int nStrip);
	void SetFixPcs(int nSerial);

	BOOL m_bThreadAliveRemakeReelmap, m_bRtnThreadRemakeReelmap;
	CString m_sPathOnThread;
	CThreadTask m_ThreadTaskRemakeReelmap; // CThreadTask class, handles the threading code
	BOOL RemakeReelmap();
	void StartThreadRemakeReelmap();
	static BOOL ThreadProcRemakeReelmap(LPVOID lpContext);
	void StopThreadRemakeReelmap();

	void RestoreReelmap();

	BOOL m_bThreadAliveReloadReelmap, m_bRtnThreadReloadReelmap, m_bDoneReloadReelmap;
	int m_nLastOnThread, m_nProgressReloadReelmap, m_nTotalProgressReloadReelmap;
	CThreadTask m_ThreadTaskReloadReelmap; // CThreadTask class, handles the threading code
	BOOL ReloadReelmap();
	BOOL ReloadReelmap(int nTo);
	BOOL IsDoneReloadReelmap();
	int GetProgressReloadReelmap();
	void StartThreadReloadReelmap();
	static BOOL ThreadProcReloadReelmap(LPVOID lpContext);
	void StopThreadReloadReelmap();

	BOOL UpdateYield(int nSerial);
	BOOL UpdateReelmapYield();
	BOOL MakeHeader(CString sPath);

	BOOL GetNodeXYonRmap(int &nNodeX, int &nNodeY, CString sPath);

	BOOL m_bThreadAliveFinalCopyItsFiles, m_bRtnThreadFinalCopyItsFiles;
	CThreadTask m_ThreadTaskFinalCopyItsFiles; // CThreadTask class, handles the threading code
	void StartThreadFinalCopyItsFiles();
	static BOOL ThreadProcFinalCopyItsFiles(LPVOID lpContext);
	void StopThreadFinalCopyItsFiles();
	BOOL FinalCopyItsFiles();
	BOOL CopyItsFile(CString sPathSrc, CString sPathDest);

	// ITS
	CString GetPathReelmapIts();
	BOOL MakeItsReelmapHeader();	// ������ ��¡�� ���� ���
	BOOL WriteIts(int nItsSerial);
	BOOL MakeItsFile(int nSerial, int nLayer);		// RMAP_UP, RMAP_DN, RMAP_INNER_UP, RMAP_INNER_DN
	CString GetItsFileData(int nSerial, int nLayer);	// RMAP_UP, RMAP_DN, RMAP_INNER_UP, RMAP_INNER_DN
	BOOL MakeDirIts();

	stResult m_stResult;
	void ResetReelmapPath();
	BOOL GetResult(); // TRUE: Make Result, FALSE: Load Result or Failed.
	CString GetResultTxt();
	CString GetSapp3Txt();
	CString GetSapp3TxtReverse();
	CString GetPath();
	CString GetIpPath();

	void ResetYield();

	//short ***m_pPnlBufIts;		// DefCode 3D Array : [nSerial-1][nRow][nCol] on File -> [nSerial-1][NodeX][NodeY] : Rotated Cw 90 
	//int m_nPnlBufIts;			// �޸𸮿� �Ҵ�� �� Shot��
	//int m_nTotPcsIts, m_nGoodPcsIts, m_nBadPcsIts, m_nDefIts[MAX_DEF];	// [DefCode] : Total Num.
	//int m_nDefStripIts[MAX_STRIP_NUM], m_nDefPerStripIts[MAX_STRIP_NUM][MAX_DEF];
	//int m_nStripOutIts[MAX_STRIP_NUM], m_nTotStOutIts;
	//BOOL m_FixPcsIts[FIX_PCS_SHOT_MAX][FIX_PCS_COL_MAX][FIX_PCS_ROW_MAX]; // [Col][Row]
	//int m_FixPcsPrevIts[FIX_PCS_COL_MAX][FIX_PCS_ROW_MAX]; // [Col][Row]

	//void CloseRstIts();
	//BOOL InitRstIts();
	//BOOL IsFixPcsIts(int nSerial, int* pCol, int* pRow, int &nTot);
	//BOOL IsFixPcsIts(int nSerial, int &Col, int &Row);
	//void ClrFixPcsIts(int nCol, int nRow);
	//void ClrFixPcsIts();
	//void SetFixPcsIts(int nSerial, int nCol, int nRow);
	//void SetFixPcsIts(int nSerial);
	//BOOL InitRstIts();
	//void CloseRstIts();

	//int m_nProgressRemakeReelmapFromPcr, m_nTotalProgressRemakeReelmapFromPcr;
	//BOOL m_bThreadAliveRemakeReelmapFromPcr, m_bRtnThreadRemakeReelmapFromPcr, m_bDoneRemakeReelmapFromPcr;
	//CThreadTask m_ThreadTaskRemakeReelmapFromPcr; // CThreadTask class, handles the threading code
	//BOOL RemakeReelmapFromPcrWithThread();
	//BOOL RemakeReelmapFromPcr();
	//void StartThreadRemakeReelmapFromPcr();
	//static BOOL ThreadProcRemakeReelmapFromPcr(LPVOID lpContext);
	//void StopThreadRemakeReelmapFromPcr();
	//int GetProgressRemakeReelmapFromPcr();
	//BOOL IsDoneRemakeReelmapFromPcr();

	void SetLastSerialOnOffline(int nSerial);
	CString GetRmapPathOnOffline(int nRmap);
	BOOL WriteOnOffline(int nSerial);
	BOOL WriteLastRmapInfoOnOffline();
	int GetFirstShotFromPcr();
	int GetLastShotFromPcr();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CReelMap)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CReelMap();

	// Generated message map functions
protected:
	//{{AFX_MSG(CReelMap)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_REELMAP_H__F5AB69D1_0DF4_4D15_985A_1421D64577D1__INCLUDED_)
