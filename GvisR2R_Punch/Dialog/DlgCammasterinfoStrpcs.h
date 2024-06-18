
// DlgCammasterinfoStrpcs.h : ��� ����
//

#pragma once

#include "../Process/CamMasterDefine.h"
#include "../Global/GlobalDefine.h"


// CDlgCammasterinfoStrpcs ��ȭ ����
class CDlgCammasterinfoStrpcs : public CDialogEx
{
	_MasterPanel MstPnl;

	short FrameRgnNum;
	int PieceRgnNum;
	int m_nRow, m_nCol;
	REGIONS_FRAME_ID FrameRgnID[MAX_FRAME_RGN_NUM];
	int nPieceNum[MAX_STRIP];
	REGIONS_PIECE_2 PieceRgnPix[MAX_PIECE_RGN_NUM];

	int m_MkOrder2PnlPcsIdx[MAX_PIECE_RGN_NUM];			// ��ŷ������ �ǽ� �ε��� (�»�ܺ��� ������׷� ��ŷ)
	int m_PnlPcsIdx2MkOrder[MAX_PIECE_RGN_NUM];			// ķ������ �ǽ� �ε����� ��ŷ����

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

// �����Դϴ�.
public:
	CDlgCammasterinfoStrpcs(CWnd* pParent = NULL);	// ǥ�� �������Դϴ�.

// ��ȭ ���� �������Դϴ�.
#ifdef AFX_DESIGN_TIME
	enum {IDD = IDD_DLG_CAMMASTERINFO_STRPCS};
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV �����Դϴ�.


// �����Դϴ�.
protected:
	HICON m_hIcon;

	// ������ �޽��� �� �Լ�
	virtual BOOL OnInitDialog();
	//afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButton1();
};
