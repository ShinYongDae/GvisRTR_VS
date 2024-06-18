
// DlgCammasterinfoMst.h : 헤더 파일
//

#pragma once

#include "../Process/CamMasterDefine.h"
#include "../Global/GlobalDefine.h"

// CDlgCammasterinfoMst 대화 상자
class CDlgCammasterinfoMst : public CDialogEx
{
	BOOL FileBrowse(CString& sPath);
	BOOL LoadStripRgnFromCam(CString sPath, CString& sData);

// 생성입니다.
public:
	CDlgCammasterinfoMst(CWnd* pParent = NULL);	// 표준 생성자입니다.

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum {IDD = IDD_DLG_CAMMASTERINFO_MST};
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
