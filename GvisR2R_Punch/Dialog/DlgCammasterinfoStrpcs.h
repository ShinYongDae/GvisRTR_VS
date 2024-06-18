
// DlgCammasterinfoStrpcs.h : 헤더 파일
//

#pragma once

#include "../Process/CamMasterDefine.h"
#include "../Global/GlobalDefine.h"


// CDlgCammasterinfoStrpcs 대화 상자
class CDlgCammasterinfoStrpcs : public CDialogEx
{
	_MasterPanel MstPnl;

	short FrameRgnNum;
	int PieceRgnNum;
	int m_nRow, m_nCol;
	REGIONS_FRAME_ID FrameRgnID[MAX_FRAME_RGN_NUM];
	int nPieceNum[MAX_STRIP];
	REGIONS_PIECE_2 PieceRgnPix[MAX_PIECE_RGN_NUM];

	int m_MkOrder2PnlPcsIdx[MAX_PIECE_RGN_NUM];			// 마킹순서별 피스 인덱스 (좌상단부터 지그재그로 마킹)
	int m_PnlPcsIdx2MkOrder[MAX_PIECE_RGN_NUM];			// 캠마스터 피스 인덱스의 마킹순서

	int GetTotPcs();
	void SetShotRowCol(int nR, int nC);
	void GetShotRowCol(int& nR, int& nC);
	void InitOrederingMk();
	CString WriteOrederingMk();
	BOOL GetMkMatrix(int nPcsId, int &nC, int &nR);	// nC:0~ , nR:0~
	void SetMasterPanelInfo();
	void StringToChar(CString str, char *szStr);
	char* StringToChar(CString str); // char* returned must be deleted... 

	BOOL FileBrowse(CString& sPath);
	BOOL LoadStrpcsFromCam(CString& sPath, CString& sData);

	void AllocPolygonRgnData(CPoint**& PolygonPoints, int nCornerNum, int PieceRgnNum);
	void FreePolygonRgnData(CPoint	**PolygonPoints);

// 생성입니다.
public:
	CDlgCammasterinfoStrpcs(CWnd* pParent = NULL);	// 표준 생성자입니다.

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum {IDD = IDD_DLG_CAMMASTERINFO_STRPCS};
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 지원입니다.


// 구현입니다.
protected:
	HICON m_hIcon;

	// 생성된 메시지 맵 함수
	virtual BOOL OnInitDialog();
	//afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButton1();
};
