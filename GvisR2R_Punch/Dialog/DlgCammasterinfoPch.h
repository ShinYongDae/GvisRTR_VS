
// DlgCammasterinfoPch.h : 헤더 파일
//

#pragma once

#include "../Process/CamMasterDefine.h"
#include "../Global/GlobalDefine.h"


// CDlgCammasterinfoPch 대화 상자
class CDlgCammasterinfoPch : public CDialogEx
{
	BOOL FileBrowse(CString& sPath);
	BOOL LoadPchFromCam(CString sPath, CString& sData);
	BOOL LoadCadMk2PntAlign(CString sPath, CString& sData);
	BOOL LoadCadMk4PntAlign(CString sPath, CString& sData);

// 생성입니다.
public:
	CDlgCammasterinfoPch(CWnd* pParent = NULL);	// 표준 생성자입니다.

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum {IDD = IDD_DLG_CAMMASTERINFO_PCH};
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
