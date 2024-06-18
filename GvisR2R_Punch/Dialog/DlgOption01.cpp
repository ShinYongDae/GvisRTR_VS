// DlgOption01.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "../GvisR2R_Punch.h"
#include "DlgOption01.h"
//#include "afxdialogex.h"


#include "../GvisR2R_PunchDoc.h"
#include "../GvisR2R_PunchView.h"

extern CGvisR2R_PunchDoc* pDoc;
extern CGvisR2R_PunchView* pView;


// CDlgOption01 ��ȭ �����Դϴ�.

//IMPLEMENT_DYNAMIC(CDlgOption01, CDialog)

CDlgOption01::CDlgOption01(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgOption01::IDD, pParent)
{
	m_pRect = NULL;
}

CDlgOption01::~CDlgOption01()
{
	if (m_pRect)
	{
		delete m_pRect;
		m_pRect = NULL;
	}
}

BOOL CDlgOption01::Create()
{
	return CDialog::Create(CDlgOption01::IDD);
}

void CDlgOption01::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDlgOption01, CDialog)
	ON_WM_SHOWWINDOW()
	ON_BN_CLICKED(IDC_CHECK1, &CDlgOption01::OnBnClickedCheck1)
	ON_BN_CLICKED(IDC_CHECK2, &CDlgOption01::OnBnClickedCheck2)
END_MESSAGE_MAP()


// CDlgOption01 �޽��� ó�����Դϴ�.


void CDlgOption01::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CDialog::OnShowWindow(bShow, nStatus);

	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰��մϴ�.
	if (!m_pRect)
	{
		m_pRect = new CRect;

		this->GetClientRect(m_pRect);
		m_pRect->top = 375;
		m_pRect->bottom += 375 + GetSystemMetrics(SM_CYSIZE);
		m_pRect->left = 3;
		m_pRect->right += 3;
		this->MoveWindow(m_pRect, TRUE);
	}

	if (bShow)
	{
		AtDlgShow();
	}
	else
	{
		AtDlgHide();
	}
}

void CDlgOption01::AtDlgShow()
{
	((CButton*)GetDlgItem(IDC_CHECK1))->SetCheck(pDoc->m_bOffLogAuto);
	((CButton*)GetDlgItem(IDC_CHECK2))->SetCheck(pDoc->m_bOffLogPLC);
}

void CDlgOption01::AtDlgHide()
{

}


BOOL CDlgOption01::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  ���⿡ �߰� �ʱ�ȭ �۾��� �߰��մϴ�.

	return TRUE;  // return TRUE unless you set the focus to a control
				  // ����: OCX �Ӽ� �������� FALSE�� ��ȯ�ؾ� �մϴ�.
}


void CDlgOption01::OnBnClickedCheck1()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	BOOL bOn = ((CButton*)GetDlgItem(IDC_CHECK1))->GetCheck();
	pDoc->m_bOffLogAuto = bOn;

	if(bOn)
	{
		::WritePrivateProfileString(_T("System"), _T("OffLogAuto"), _T("1"), PATH_WORKING_INFO);
	}
	else
	{
		::WritePrivateProfileString(_T("System"), _T("OffLogAuto"), _T("0"), PATH_WORKING_INFO);
	}
}


void CDlgOption01::OnBnClickedCheck2()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	BOOL bOn = ((CButton*)GetDlgItem(IDC_CHECK2))->GetCheck();
	pDoc->m_bOffLogPLC = bOn;

	if (bOn)
	{
		::WritePrivateProfileString(_T("System"), _T("OffLogPLC"), _T("1"), PATH_WORKING_INFO);
	}
	else
	{
		::WritePrivateProfileString(_T("System"), _T("OffLogPLC"), _T("0"), PATH_WORKING_INFO);
	}
}
