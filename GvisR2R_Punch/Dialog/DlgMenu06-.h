#pragma once


// CDlgMenu06 ��ȭ �����Դϴ�.

class CDlgMenu06 : public CDialog
{
	DECLARE_DYNAMIC(CDlgMenu06)

public:
	CDlgMenu06(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~CDlgMenu06();

// ��ȭ ���� �������Դϴ�.
// Dialog Data
//{{AFX_DATA(CDlgMenu06)
	enum { IDD = IDD_DLG_MENU_06 };
	// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

//#ifdef AFX_DESIGN_TIME
//	enum { IDD = IDD_DLG_UTIL_08 };
//#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
public:
};
