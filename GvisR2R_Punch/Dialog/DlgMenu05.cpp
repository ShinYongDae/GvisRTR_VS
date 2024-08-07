// DlgMenu05.cpp : implementation file
//

#include "stdafx.h"
#include "../gvisr2r_punch.h"
#include "DlgMenu05.h"
#include "DlgKeyNum.h"
#include "DlgMyKeypad.h"
#include "../Process/DataFile.h"
#include "DlgProgress.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#include "../MainFrm.h"
#include "../GvisR2R_PunchDoc.h"
#include "../GvisR2R_PunchView.h"

extern CMainFrame* pFrm;
extern CGvisR2R_PunchDoc* pDoc;
extern CGvisR2R_PunchView* pView;

/////////////////////////////////////////////////////////////////////////////
// CDlgMenu05 dialog


CDlgMenu05::CDlgMenu05(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgMenu05::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgMenu05)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT

	m_pRect = NULL;
	m_sModel = _T(""); m_sLot = _T(""); m_sLayer = _T("");
	m_nCntModelNames = 0;
	m_nCntLotNames = 0;
	m_sEditRst = _T("");
	m_sRmapPath = _T("");
	m_nSerialSt = 0;
	m_nCompletedShot = m_nMarkedShot = m_nSerialEd = 0;

	m_pDlgUtil01 = NULL;
	m_bLoadImg = FALSE;
}

CDlgMenu05::~CDlgMenu05()
{
	DelImg();

	if(m_pRect)
	{
		delete m_pRect;
		m_pRect = NULL;
	}

	if(m_pDlgUtil01 != NULL) 
	{
		delete m_pDlgUtil01;
		m_pDlgUtil01 = NULL;
	}
}

BOOL CDlgMenu05::Create()
{
	return CDialog::Create(CDlgMenu05::IDD);
}

void CDlgMenu05::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgMenu05)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgMenu05, CDialog)
	//{{AFX_MSG_MAP(CDlgMenu05)
	ON_WM_SHOWWINDOW()
	ON_CBN_SELCHANGE(IDC_COMBO_MODEL, OnSelchangeComboModel)
	ON_LBN_SELCHANGE(IDC_LIST_LOT, OnSelchangeListLot)
	ON_BN_CLICKED(IDC_BTN_SEARCH, OnBtnSearch)
	ON_BN_CLICKED(IDC_CHK_REELMAP, OnChkReelmap)
	ON_BN_CLICKED(IDC_STC_LOT, OnStcLot)
	ON_CBN_SELCHANGE(IDC_COMBO_LAYER, OnSelchangeComboLayer)
	ON_BN_CLICKED(IDC_BTN_SAVE, OnBtnSave)
	ON_BN_CLICKED(IDC_STC_SERIAL_ST, OnStcSerialSt)
	ON_BN_CLICKED(IDC_STC_SERIAL_ED, OnStcSerialEd)
	ON_BN_CLICKED(IDC_STC_PROC, OnStcProc)
	ON_BN_CLICKED(IDC_BTN_SAVE4, OnBtnSave4)
	ON_BN_CLICKED(IDC_BTN_SAVE3, OnBtnSave3)
	ON_BN_CLICKED(IDC_BTN_SAVE2, OnBtnSave2)
	ON_BN_CLICKED(IDC_CHECK1, OnCheck1)
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BTN_SAVE5, &CDlgMenu05::OnBnClickedBtnSave5)
	ON_STN_CLICKED(IDC_STC_ITS, &CDlgMenu05::OnStnClickedStcIts)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgMenu05 message handlers

void CDlgMenu05::OnShowWindow(BOOL bShow, UINT nStatus) 
{
	CDialog::OnShowWindow(bShow, nStatus);
	
	// TODO: Add your message handler code here
	if(!m_pRect)
	{
		m_pRect = new CRect;
		
		this->GetClientRect(m_pRect);
		m_pRect->top = 75 + 2;
		m_pRect->bottom += 75 + 2;
		m_pRect->left = 3;
		m_pRect->right += 3;
		this->MoveWindow(m_pRect, TRUE);
	}

	if(bShow)
	{
		AtDlgShow();
	}
	else
	{
		AtDlgHide();
	}	
}

void CDlgMenu05::AtDlgShow()
{
	LoadImg();
	ModifyModelData();
	((CComboBox*)GetDlgItem(IDC_COMBO_MODEL))->SelectString(0, m_sModel);
	((CComboBox*)GetDlgItem(IDC_COMBO_MODEL))->SetWindowText(m_sModel);
	ModifyLotData();

	DispTestMode();
	//DispItsCode(); // For VS
}

void CDlgMenu05::AtDlgHide()
{

}

void CDlgMenu05::InitLabel()
{
	myLabel[0].SubclassDlgItem(IDC_STC_SIG00, this);	// ����
	myLabel[1].SubclassDlgItem(IDC_STC_SIG01, this);	// ����

	for (int i = 0; i < MAX_MENU05_LABEL; i++)
	{
		myLabel[i].SetFontName(_T("Arial"));
		myLabel[i].SetFontSize(18);
		myLabel[i].SetFontBold(TRUE);
		myLabel[i].SetTextColor(RGB_DARKRED);
		myLabel[i].SetImageBk(LBL_IMG_UP);
	}
}

void CDlgMenu05::LoadImg()
{
	if (m_bLoadImg)
		return;
	m_bLoadImg = TRUE;

	int i;
	for (i = 0; i < MAX_MENU05_LABEL; i++)
	{
		myLabel[i].LoadImage(ICO_LED_GRY_DlgFrameHigh, LBL_IMG_UP, CSize(20, 20), LBL_POS_CENTER);
		myLabel[i].LoadImage(ICO_LED_BLU_DlgFrameHigh, LBL_IMG_DN, CSize(20, 20), LBL_POS_CENTER);
	}
}

void CDlgMenu05::DelImg()
{
	if (!m_bLoadImg)
		return;
	m_bLoadImg = FALSE;

	int i;

	for (i = 0; i < MAX_MENU05_LABEL; i++)
		myLabel[i].DelImgList();
}

void CDlgMenu05::OnSelchangeComboModel() 
{
	// TODO: Add your control notification handler code here
	int nIndex = ((CComboBox*)GetDlgItem(IDC_COMBO_MODEL))->GetCurSel();
	if (nIndex != LB_ERR)
	{
		((CComboBox*)GetDlgItem(IDC_COMBO_MODEL))->GetLBText(nIndex, m_sModel); // pDoc->WorkingInfo.LastJob.sModel
		ModifyLotData();
	}
}

void CDlgMenu05::OnSelchangeListLot() 
{
	// TODO: Add your control notification handler code here
// 	DisplayResultData();
	int nIndex = m_nCurSelLotIdx = ((CListBox*)GetDlgItem(IDC_LIST_LOT))->GetCurSel();
	if (nIndex != LB_ERR)
	{
		((CListBox*)GetDlgItem(IDC_LIST_LOT))->GetText(nIndex, m_sLot);
		ModifyLayerData();
		nIndex = GetIdxTopLayer();
		//if (nIndex > -1)
		//{
		//	SelchangeComboLayer(nIndex);
		//}
	}
}

int CDlgMenu05::GetIdxTopLayer()
{
	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;
	int nIdx = 0;
	((CComboBox*)GetDlgItem(IDC_COMBO_LAYER))->GetLBText(nIdx, m_sLayer);

	CFileFind finder;
	CString sPath, sVal, sLayer;
	sPath.Format(_T("%s%s\\%s\\%s"), pDoc->WorkingInfo.System.sPathOldFile,
		m_sModel, m_sLot, m_sLayer);
	if (bDualTest)
	{
		m_sRmapPath.Format(_T("%s\\ReelMapDataAll.txt"), sPath);
		if (finder.FindFile(sPath))
		{
			TCHAR szData[MAX_PATH];
			if (0 < ::GetPrivateProfileString(_T("Info"), _T("��鷹�̾�"), NULL, szData, sizeof(szData), m_sRmapPath))
				sLayer = CString(szData);
			else
				sLayer = _T("");

			if (m_sLayer == sLayer)
			{
				return 0;
			}
			else if (sLayer.IsEmpty())
			{
				return -1;
			}
			else
			{
				nIdx = 1;
				((CComboBox*)GetDlgItem(IDC_COMBO_LAYER))->GetLBText(nIdx, m_sLayer);
				if (m_sLayer == sLayer)
					return 1;

				m_sLayer = _T("");
				return -1;
			}
		}
	}
	else
	{
		m_sRmapPath.Format(_T("%s\\ReelMapDataUp.txt"), sPath);
		if (finder.FindFile(sPath))
		{
			return 0;
		}
		else
			return -1;
	}

	return nIdx;
}

void CDlgMenu05::InitModel()
{
	m_sModel = pDoc->WorkingInfo.LastJob.sModelUp;
	m_sLot = pDoc->WorkingInfo.LastJob.sLotUp;
	m_sLayer = pDoc->WorkingInfo.LastJob.sLayerUp;
}

BOOL CDlgMenu05::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	LOGFONT lf;
	memset(&lf, 0, sizeof(LOGFONT));
	lf.lfHeight = 22; // 22 point
	lf.lfWeight = FW_EXTRABOLD;
	//strcpy(lf.lfFaceName, "����"); // ����ü�� font setting
	wsprintf(lf.lfFaceName, TEXT("%s"), TEXT("����"));

	m_FontOfListCtrl.CreateFontIndirect(&lf); 

	CComboBox* pCtlComboModel = (CComboBox*)GetDlgItem(IDC_COMBO_MODEL);
	pCtlComboModel->SetFont((CFont*)&m_FontOfListCtrl, TRUE);

	CListBox* pCtlListLot = (CListBox*)GetDlgItem(IDC_LIST_LOT);
	pCtlListLot->SetFont((CFont*)&m_FontOfListCtrl, TRUE);

	CComboBox* pCtlComboLayer = (CComboBox*)GetDlgItem(IDC_COMBO_LAYER);
	pCtlComboLayer->SetFont((CFont*)&m_FontOfListCtrl, TRUE);

	InitLabel();
	InitStc();
	InitModel();

	GetDlgItem(IDC_BTN_MES)->ShowWindow(SW_HIDE);
	//GetDlgItem(IDC_BTN_SAVE4)->ShowWindow(SW_HIDE);

	ShowDlg(IDD_DLG_UTIL_01);
	OnCheck1();

// 	GetDlgItem(IDC_CHK_REELMAP)->ShowWindow(SW_HIDE);
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgMenu05::InitStc()
{
	myStcLot.SubclassDlgItem(IDC_STC_LOT, this);
	myStcLot.SetFontName(_T("Arial"));
	myStcLot.SetFontSize(14);
	myStcLot.SetFontBold(TRUE);
	myStcLot.SetTextColor(RGB_BLACK);
	myStcLot.SetBkColor(RGB_WHITE);

	myStcSerialSt.SubclassDlgItem(IDC_STC_SERIAL_ST, this);
	myStcSerialSt.SetFontName(_T("Arial"));
	myStcSerialSt.SetFontSize(20);
	myStcSerialSt.SetFontBold(TRUE);
	myStcSerialSt.SetTextColor(RGB_RED);
	myStcSerialSt.SetBkColor(RGB_WHITE);
	
	myStcSerialEd.SubclassDlgItem(IDC_STC_SERIAL_ED, this);
	myStcSerialEd.SetFontName(_T("Arial"));
	myStcSerialEd.SetFontSize(20);
	myStcSerialEd.SetFontBold(TRUE);
	myStcSerialEd.SetTextColor(RGB_RED);
	myStcSerialEd.SetBkColor(RGB_WHITE);
	
	myStcFromTo.SubclassDlgItem(IDC_STATIC_FROMTO, this);
	myStcFromTo.SetFontName(_T("Arial"));
	myStcFromTo.SetFontSize(26);
	myStcFromTo.SetFontBold(TRUE);
	myStcFromTo.SetTextColor(RGB_BLACK);
	myStcFromTo.SetBkColor(RGB_DLG_FRM2);
	
	myStcProc.SubclassDlgItem(IDC_STC_PROC, this);
	myStcProc.SetFontName(_T("Arial"));
	myStcProc.SetFontSize(14);
	myStcProc.SetFontBold(TRUE);
	myStcProc.SetTextColor(RGB_BLACK);
	myStcProc.SetBkColor(RGB_WHITE);

	myStcIts.SubclassDlgItem(IDC_STC_ITS, this);
	myStcIts.SetFontName(_T("Arial"));
	myStcIts.SetFontSize(14);
	myStcIts.SetFontBold(TRUE);
	myStcIts.SetTextColor(RGB_BLACK);
	myStcIts.SetBkColor(RGB_WHITE);
} 

int CDlgMenu05::CntModelNames()
{
	int nCntModelNames = 0;
	CFileFind cFile;
	CString strModelName, strPath;
	BOOL bExist;

	strPath = pDoc->WorkingInfo.System.sPathOldFile + _T("\\*.*");
	
	bExist = cFile.FindFile(strPath);
	while(bExist)
	{
		bExist = cFile.FindNextFile();
		if(cFile.IsDots()) continue;
		if(cFile.IsDirectory())
		{
			nCntModelNames++;
		}
	}

	return nCntModelNames;
}

int CDlgMenu05::CntLotNamesInModelFolder()
{
	int nCntLotNames = 0;
	CFileFind cFile;
	CString strLayerName, strPath;
	BOOL bExist;

	strPath = pDoc->WorkingInfo.System.sPathOldFile;
	strPath += m_sModel; // pDoc->WorkingInfo.LastJob.sModel
	strPath += _T("\\*.*");
	
	bExist = cFile.FindFile(strPath);
	while(bExist)
	{
		bExist = cFile.FindNextFile();
		if(cFile.IsDots()) continue;
		if(cFile.IsDirectory())
		{
			nCntLotNames++;
		}
	}

	return nCntLotNames;
}

void CDlgMenu05::ModifyModelData()
{
	((CComboBox*)GetDlgItem(IDC_COMBO_MODEL))->ResetContent();

	//char FN[100];
	//sprintf(FN, "%s*.*", pDoc->WorkingInfo.System.sPathOldFile);
	TCHAR FN[100];
	_stprintf(FN, _T("%s*.*"), pDoc->WorkingInfo.System.sPathOldFile);//sprintf

	((CComboBox*)GetDlgItem(IDC_COMBO_MODEL))->Dir(0x8010, FN);
	int t=0;
	
//	"[..]"�� ���� 
	((CComboBox*)GetDlgItem(IDC_COMBO_MODEL))->DeleteString(0);
	int nIndex = ((CComboBox*)GetDlgItem(IDC_COMBO_MODEL))->FindStringExact(-1, _T("[..]"));
	((CComboBox*)GetDlgItem(IDC_COMBO_MODEL))->DeleteString(nIndex);

	int nCount  = ((CComboBox*)GetDlgItem(IDC_COMBO_MODEL))->GetCount();

	CString strBuf, strBuf2;
	for (int i = 0; i < nCount; i++)
	{
		((CComboBox*)GetDlgItem(IDC_COMBO_MODEL))->GetLBText(i, strBuf);

		if (strBuf.GetLength() < 3)
			continue;
//		�����̸����� "["�� ���� 
		CString strBuf2 = strBuf.Mid(1, strBuf.GetLength() - 2);
		((CComboBox*)GetDlgItem(IDC_COMBO_MODEL))->InsertString(t, strBuf2);
		((CComboBox*)GetDlgItem(IDC_COMBO_MODEL))->DeleteString(t + 1);
		t++;
	}
}

void CDlgMenu05::ModifyLotData()
{
	((CListBox*)GetDlgItem(IDC_LIST_LOT))->ResetContent();

	CString Dir, strFileName;
	Dir = pDoc->WorkingInfo.System.sPathOldFile + m_sModel + _T("\\");

	//char FN[100];
	//sprintf(FN, "%s*.*", Dir);
	TCHAR FN[100];
	_stprintf(FN, _T("%s*.*"), Dir);

	((CListBox*)GetDlgItem(IDC_LIST_LOT))->Dir(0x8010, FN);
	int t=0;
	
//	"[..]"�� ���� 
	((CListBox*)GetDlgItem(IDC_LIST_LOT))->DeleteString(0);
	int nIndex = ((CListBox*)GetDlgItem(IDC_LIST_LOT))->FindStringExact(-1, _T("[..]"));
	((CListBox*)GetDlgItem(IDC_LIST_LOT))->DeleteString(nIndex);

	int nCount  = ((CListBox*)GetDlgItem(IDC_LIST_LOT))->GetCount();

	CString strBuf, strBuf2;
	int i;

	for (i = nCount; i > 0 ; i--)
	{
		((CListBox*)GetDlgItem(IDC_LIST_LOT))->GetText(i-1, strBuf);

		if (strBuf.GetLength() < 3)
			continue;
//		�����̸����� "[]"�� ���� 
		CString strBuf2 = strBuf.Mid(1, strBuf.GetLength() - 2);
		((CListBox*)GetDlgItem(IDC_LIST_LOT))->InsertString(t+nCount, strBuf2);	//090211-ndy
		t++;
	}

	for (i = 0; i < nCount ; i++)
		((CListBox*)GetDlgItem(IDC_LIST_LOT))->DeleteString(0);
}

void CDlgMenu05::DispTestMode()
{
	//m_pPcr[0][nIdx]->m_sItsCode

	BOOL bOn;

	bOn = (pDoc->GetTestMode() == MODE_INNER) ? TRUE : FALSE; // ���� �˻� ���
	if (bOn && myLabel[0].GetImageBk() != LBL_IMG_DN)
		myLabel[0].SetImageBk(LBL_IMG_DN);
	else if (!bOn && myLabel[0].GetImageBk() != LBL_IMG_UP)
		myLabel[0].SetImageBk(LBL_IMG_UP);

	bOn = (pDoc->GetTestMode() == MODE_OUTER) ? TRUE : FALSE; // ���� �˻� ���
	if (bOn && myLabel[1].GetImageBk() != LBL_IMG_DN)
		myLabel[1].SetImageBk(LBL_IMG_DN);
	else if (!bOn && myLabel[1].GetImageBk() != LBL_IMG_UP)
		myLabel[1].SetImageBk(LBL_IMG_UP);

}

void CDlgMenu05::ModifyLayerData()
{
	((CComboBox*)GetDlgItem(IDC_COMBO_LAYER))->ResetContent();

	CString Dir, strFileName;
	Dir = pDoc->WorkingInfo.System.sPathOldFile + m_sModel + _T("\\") + m_sLot + _T("\\");

	//char FN[100];
	//sprintf(FN, "%s*.*"), Dir);
	TCHAR FN[100];
	_stprintf(FN, _T("%s*.*"), Dir);

	((CComboBox*)GetDlgItem(IDC_COMBO_LAYER))->Dir(0x8010, FN);
	int t=0;
	
//	"[..]"�� ���� 
	((CComboBox*)GetDlgItem(IDC_COMBO_LAYER))->DeleteString(0);
	int nIndex = ((CComboBox*)GetDlgItem(IDC_COMBO_LAYER))->FindStringExact(-1, _T("[..]"));
	((CComboBox*)GetDlgItem(IDC_COMBO_LAYER))->DeleteString(nIndex);

	int nCount  = ((CComboBox*)GetDlgItem(IDC_COMBO_LAYER))->GetCount();

	CString strBuf, strBuf2;
	for (int i = 0; i < nCount; i++)
	{
		((CComboBox*)GetDlgItem(IDC_COMBO_LAYER))->GetLBText(i, strBuf);

		if (strBuf.GetLength() < 3)
			continue;
//		�����̸����� "["�� ���� 
		CString strBuf2 = strBuf.Mid(1, strBuf.GetLength() - 2);
		((CComboBox*)GetDlgItem(IDC_COMBO_LAYER))->InsertString(t, strBuf2);
		((CComboBox*)GetDlgItem(IDC_COMBO_LAYER))->DeleteString(t + 1);
		t++;

		//if (strBuf2.Find(_T("TOP")) >= 0)
		//{
		//	m_sLayerUp = strBuf2;
		//}
		//else if (strBuf2.Find(_T("BOTTOM")) >= 0)
		//{
		//	m_sLayerDn = strBuf2;
		//}
	}
}

// void CDlgMenu05::ModifyLayerData()
// {
// 	((CComboBox*)GetDlgItem(IDC_COMBO_LAYER))->ResetContent();
// 
// 	CString Dir, strFileName;
// 	Dir = pDoc->WorkingInfo.System.sPathOldFile + m_sModel + "\\" + m_sLot + "\\");
// 
// 	char FN[100];
// 	sprintf(FN, "%s*.*"), Dir);
// 
// 	((CComboBox*)GetDlgItem(IDC_COMBO_LAYER))->Dir(0x8010, FN);
// 	int t=0;
// 	
// //	"[..]"�� ���� 
// 	((CComboBox*)GetDlgItem(IDC_COMBO_LAYER))->DeleteString(0);
// 	int nIndex = ((CComboBox*)GetDlgItem(IDC_COMBO_LAYER))->FindStringExact(-1, "[..]");
// 	((CComboBox*)GetDlgItem(IDC_COMBO_LAYER))->DeleteString(nIndex);
// 
// 	int nCount  = ((CComboBox*)GetDlgItem(IDC_COMBO_LAYER))->GetCount();
// 
// 	CString strBuf, strBuf2;
// 	int i;
// 
// 	for (i = nCount; i > 0 ; i--)
// 	{
// 		((CComboBox*)GetDlgItem(IDC_COMBO_LAYER))->GetText(i-1, strBuf);
// 
// 		if (strBuf.GetLength() < 3)
// 			continue;
// //		�����̸����� "[]"�� ���� 
// 		CString strBuf2 = strBuf.Mid(1, strBuf.GetLength() - 2);
// 		((CComboBox*)GetDlgItem(IDC_COMBO_LAYER))->InsertString(t+nCount, strBuf2);	//090211-ndy
// 		t++;
// 	}
// 
// // 	for (i = 0; i < nCount ; i++)
// // 		((CComboBox*)GetDlgItem(IDC_COMBO_LAYER))->DeleteString(0);
// }

// CString CDlgMenu05::SearchLayerName()
// {
// 	CString strPanelFacePath, strPath;
// 
// 	strPanelFacePath.Format(_T("%s%s\\%s\\%s"), pDoc->WorkingInfo.System.sPathOldFile, 
// 											m_sModel, m_sLot, m_sLayer);
// 
// 	int nTemp, nFileSize, nRSize;
// 	CString strFileData, strHeaderErrorInfo, strModel, strLayer, strLot, strTotalBadPieceNum, strMsg;
// 	char FileD[200];
// 	CFileFind findfile;
// 	FILE *fp;
// 	char *FileData;
// 	
// 	strPath.Format(_T("%s\\%04d.pcr"), strPanelFacePath, 1); // ù��° ���� ����.
// 
// 	if(findfile.FindFile(strPath)) //find file.
// 	{
// 		strcpy(FileD, strPath);
// 		if((fp = fopen((LPCTSTR)FileD, "r")) != NULL)
// 		{
// 			fseek(fp, 0, SEEK_END);
// 			nFileSize = ftell(fp);
// 			fseek(fp, 0, SEEK_SET);
// 
// 			/* Allocate space for a path name */
// 			//FileData = (char*)malloc( nFileSize );
// 			FileData = (char*)calloc(nFileSize+1, sizeof(char));
// 			
// 			nRSize = fread(FileData, sizeof(char), nFileSize, fp);
// 			strFileData.Format(_T("%s"), FileData);
// 			fclose(fp);
// 			free( FileData );
// 		}
// 		else
// 		{
// 			strMsg.Format(_T("%s ��ġ�� ������ ���µ��� �ʽ��ϴ�."), strPath);
// 			AfxMessageBox(strMsg,MB_ICONWARNING|MB_OK);
// 			strLayer = _T("");
// 		}
// 
// 		// Error Code
// 		nTemp = strFileData.Find(',', 0);
// 		strHeaderErrorInfo = strFileData.Left(nTemp);
// 		strFileData.Delete(0, nTemp+1);
// 		nFileSize = nFileSize - nTemp - 1;
// 		
// 		// Model
// 		nTemp = strFileData.Find(',', 0);
// 		strModel = strFileData.Left(nTemp);
// 		strFileData.Delete(0, nTemp+1);
// 		nFileSize = nFileSize - nTemp - 1;
// 
// 		// Layer
// 		nTemp = strFileData.Find(',', 0);
// 		strLayer = strFileData.Left(nTemp);
// 		strFileData.Delete(0, nTemp+1);
// 		nFileSize = nFileSize - nTemp - 1;
// 
// 		// Lot
// 		nTemp = strFileData.Find('\n', 0);
// 		strLot = strFileData.Left(nTemp);
// 		strFileData.Delete(0, nTemp+1);
// 		nFileSize = nFileSize - nTemp - 1;
// 
// 		//strTotalBadPieceNum = strFileData;
// 		nTemp = strFileData.Find('\n', 0);
// 		strTotalBadPieceNum = strFileData.Left(nTemp);;
// 		strFileData.Delete(0, nTemp+1);
// 		nFileSize = nFileSize - nTemp - 1;
// 		
// 	}
// 	else
// 	{
// 		strMsg.Format(_T("%s ��ġ�� ������ �����ϴ�."), strPath);
// 		AfxMessageBox(strMsg,MB_ICONWARNING|MB_OK);
// 		strLayer = _T("");
// 	}
// 
// 	return strLayer;
// }

BOOL CDlgMenu05::PreTranslateMessage(MSG* pMsg) 
{
	// TODO: Add your specialized code here and/or call the base class
	if (pMsg->message != WM_KEYDOWN)
		return CDialog::PreTranslateMessage(pMsg);

	if ((pMsg->lParam & 0x40000000) == 0)
	{
		switch(pMsg->wParam)
		{
		case VK_RETURN:
			return TRUE;
		case VK_ESCAPE:
			return TRUE;
		case 'S':
		case 's':
			if(GetKeyState(VK_CONTROL) < 0) // Ctrl Ű�� ������ ����
			{
				WINDOWPLACEMENT wndPlace;
				AfxGetMainWnd()->GetWindowPlacement(&wndPlace);
				wndPlace.showCmd |= SW_MAXIMIZE; 
				AfxGetMainWnd()->SetWindowPlacement(&wndPlace);	
			}
			break;
		}
	}
	
	return CDialog::PreTranslateMessage(pMsg);
}

void CDlgMenu05::DisplayResultData()
{
	GetResult();
	m_sEditRst = DisplayData();
	GetDlgItem(IDC_EDIT_RESULT)->SetWindowText(m_sEditRst);
	((CButton*)GetDlgItem(IDC_CHK_REELMAP))->SetCheck(FALSE);
}

BOOL CDlgMenu05::GetResult() // TRUE: Make Result, FALSE: Load Result or Failed.
{
	//������ �о��. ======================================================================

	// �۾� �ð��� �о��.
// 	LoadLotWorkingTime();

	// �ҷ����� ����Ÿ�� �о��.
	int i, k;
	CString strAddedDefectFP;
	CString strPath, strMsg, strData, strMenu, strItem;
	CFileFind findfile;
	TCHAR szData[MAX_PATH];

	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;

	m_nEntireStripNum = 0;

	if(m_sModel.IsEmpty())
		pView->MsgBox(_T("�� ������ �����ϴ�."));
		//AfxMessageBox(_T("�� ������ �����ϴ�."));

	if(m_sLot.IsEmpty())
		pView->MsgBox(_T("��Ʈ ������ �����ϴ�."));
		//AfxMessageBox(_T("��Ʈ ������ �����ϴ�."));

	CFileFind cFile;
	CString sPathPcr;

	strPath.Format(_T("%s%s\\%s\\%s\\OFFLINE\\ReelMapDataAll.txt"), pDoc->WorkingInfo.System.sPathOldFile,
		m_sModel, m_sLot, m_sLayerUp);

	BOOL bExist = cFile.FindFile(strPath);
	if (bExist)
	{
		bDualTest = TRUE;
		strAddedDefectFP = strPath;
	}
	else
	{
		strPath.Format(_T("%s%s\\%s\\%s\\OFFLINE\\ReelMapDataUp.txt"), pDoc->WorkingInfo.System.sPathOldFile,
			m_sModel, m_sLot, m_sLayerUp);
		bExist = cFile.FindFile(strPath);
		if (bExist)
		{
			bDualTest = FALSE;
			strAddedDefectFP = strPath;
		}
		else
		{
			strPath.Format(_T("%s%s\\%s\\%s\\ReelMapDataAll.txt"), pDoc->WorkingInfo.System.sPathOldFile,
				m_sModel, m_sLot, m_sLayerUp);
			bExist = cFile.FindFile(strPath);
			if (bExist)
			{
				bDualTest = TRUE;
				strAddedDefectFP = strPath;
			}
			else
			{
				strPath.Format(_T("%s%s\\%s\\%s\\ReelMapDataUp.txt"), pDoc->WorkingInfo.System.sPathOldFile,
					m_sModel, m_sLot, m_sLayerUp);
				bExist = cFile.FindFile(strPath);
				if (bExist)
				{
					bDualTest = FALSE;
					strAddedDefectFP = strPath;
				}
				else
					return FALSE; // pcr������ �������� ����.
			}
		}
	}
	

	if(!findfile.FindFile(strAddedDefectFP)) // Can not find file.
	{
		strMsg.Format(_T("%s file open is failed"), strAddedDefectFP);
		pView->ClrDispMsg();
		AfxMessageBox(strMsg);
	}
	else
	{
		if (0 < ::GetPrivateProfileString(_T("Info"), _T("Lot Start"), NULL, szData, sizeof(szData), strAddedDefectFP))
			m_strLotStartTime = CString(szData); 
		else
			m_strLotStartTime = _T("");

		if (0 < ::GetPrivateProfileString(_T("Info"), _T("Lot Run"), NULL, szData, sizeof(szData), strAddedDefectFP))
			m_strLotWorkingTime = CString(szData); 
		else
			m_strLotWorkingTime = _T("");

		if (0 < ::GetPrivateProfileString(_T("Info"), _T("Lot End"), NULL, szData, sizeof(szData), strAddedDefectFP))
			m_strLotEndTime = CString(szData); 
		else
			m_strLotEndTime = _T("");

		// ���� ����Ÿ�� �о��.

		// Read Piece infomation..............
		if (0 < ::GetPrivateProfileString(_T("Info"), _T("Total Pcs"), NULL, szData, sizeof(szData), strAddedDefectFP))
			m_nEntirePieceNum = _tstoi(szData);
		else
			m_nEntirePieceNum = 0;

		if (0 < ::GetPrivateProfileString(_T("Info"), _T("Good Pcs"), NULL, szData, sizeof(szData), strAddedDefectFP))
			m_nGoodPieceNum = _tstoi(szData);
		else
			m_nGoodPieceNum = 0;

		if (0 < ::GetPrivateProfileString(_T("Info"), _T("Bad Pcs"), NULL, szData, sizeof(szData), strAddedDefectFP))
			m_nDefectPieceNum = _tstoi(szData);
		else
			m_nDefectPieceNum = 0;

		m_nTotStOut = 0;
		for(k=0; k<MAX_STRIP_NUM; k++)
		{
			strMenu.Format(_T("Strip%d"), k);
			if (0 < ::GetPrivateProfileString(_T("Info"), strMenu, NULL, szData, sizeof(szData), strAddedDefectFP))
				m_nDefStrip[k] = _tstoi(szData);
			else
				m_nDefStrip[k] = 0;

			strMenu.Format(_T("%d"), k);
			if (0 < ::GetPrivateProfileString(_T("StripOut"), strMenu, NULL, szData, sizeof(szData), strAddedDefectFP))
				m_nStripOut[k] = _tstoi(szData);
			else
				m_nStripOut[k] = 0;

			m_nTotStOut += m_nStripOut[k];

			for(i=1; i<MAX_DEF; i++)
			{
				strItem.Format(_T("Strip%d"), k);
				strMenu.Format(_T("%d"), i);
				if (0 < ::GetPrivateProfileString(strItem, strMenu, NULL, szData, sizeof(szData), strAddedDefectFP))
					m_nDefPerStrip[k][i] = _tstoi(szData);
				else
					m_nDefPerStrip[k][i] = 0;
			}
		}

		for(i=1; i<MAX_DEF; i++)
		{
			strMenu.Format(_T("%d"), i);
			if (0 < ::GetPrivateProfileString(_T("Info"), strMenu, NULL, szData, sizeof(szData), strAddedDefectFP))
				m_nEntireAddedDefect[i] = _tstoi(szData);  // �ҷ��̸��� �ҷ����� ���Ͽ��� �о��.
			else
			{
				m_nEntireAddedDefect[i] = 0;
			}
		}

		m_nEntireStripNum = (m_nSerialEd - m_nSerialSt + 1) * 4;

	}

	return TRUE;
}


CString CDlgMenu05::DisplayData()
{
	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;

	CString strFileData, strData;
	int nStripNumY = 4;

	//����Ʈ �ۼ�. =====================================================================

	strFileData = _T("");
	strFileData += _T("1. �Ϲ� ����\r\n");
	
	strFileData += _T("\t�� �� : \t");
	strFileData += m_sModel;
	strFileData += _T("\r\n");
	strFileData += _T("\t�� Ʈ : \t");
	strFileData += m_sLot;
	strFileData += _T("\r\n");
	strFileData += _T("\t��� ���̾� : \t");
	strFileData += pDoc->WorkingInfo.LastJob.sLayerUp;
	strFileData += _T("\r\n");
	if(bDualTest)
	{
		strFileData += _T("\t�ϸ� ���̾� : \t");
		strFileData += pDoc->WorkingInfo.LastJob.sLayerDn;
		strFileData += _T("\r\n");
	}
	strFileData += _T("\t���۽ð� : \t");
	strFileData += m_strLotStartTime;
	strFileData += _T("\r\n");
	strFileData += _T("\t����ð� : \t");
	strFileData += m_strLotWorkingTime;
	strFileData += _T("\r\n");
	strFileData += _T("\t����ð� : \t");
	strFileData += m_strLotEndTime;
	strFileData += _T("\r\n");
	strFileData += _T("\r\n");

	strFileData += _T("2. ��ǰ �˻� ���\r\n");

	strFileData += _T("\t----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------\r\n");
	strFileData += _T("\t  ����  \t\t\t\t\t�� ��  �� ��\t\r\n");
	strFileData += _T("\t----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------\r\n");
	strFileData += _T("\t        \t\t�� �� ��\t\t�� ǰ ��\t\t�� �� ��\t\t�� ǰ ��\t\t�� �� ��\t\r\n");
	strFileData += _T("\t----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------\r\n");
	strData.Format(_T("\t Strip  \t\t%8d\t\t%8d\t\t%8d\t\t%8.2f\t\t%8.2f\t\r\n"), m_nEntireStripNum, m_nEntireStripNum - m_nTotStOut, m_nTotStOut,
					100.0*(double)(m_nEntireStripNum-m_nTotStOut)/(double)m_nEntireStripNum, 100.0*(double)m_nTotStOut/(double)m_nEntireStripNum);
	strFileData += strData;
	strFileData += _T("\t----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------\r\n");
	strData.Format(_T("\t  Unit  \t\t%8d\t\t%8d\t\t%8d\t\t%8.2f\t\t%8.2f\t\r\n"), m_nEntirePieceNum, m_nGoodPieceNum, m_nDefectPieceNum,
											(m_nEntirePieceNum>0)?100.0*(double)(m_nGoodPieceNum)/(double)(m_nEntirePieceNum):0.0, 
											(m_nEntirePieceNum>0)?100.0*(double)(m_nDefectPieceNum)/(double)(m_nEntirePieceNum):0.0);
	strFileData += strData;
	strFileData += _T("\t----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------\r\n");
	strFileData += _T("\r\n");


	strFileData += _T("3. ���� �˻� ���\r\n");

	strFileData += _T("\t-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------\r\n");
	strFileData += _T("\t\t  ����  \t        \t\t  1 ��  \t\t  2 ��  \t\t  3 ��  \t\t  4 ��  \t\r\n");
	strFileData += _T("\t-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------\r\n");

	strData.Format(_T("\t�� �� ��\t\t Strip  \t\t%8d\t\t%8d\t\t%8d\t\t%8d\t\r\n"), m_nEntireStripNum/nStripNumY, m_nEntireStripNum/nStripNumY, m_nEntireStripNum/nStripNumY, m_nEntireStripNum/nStripNumY);
	strFileData += strData;
	strFileData += _T("\t        \t\t-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------\r\n");
	strData.Format(_T("\t        \t\t  Unit  \t\t%8d\t\t%8d\t\t%8d\t\t%8d\t\r\n"), (m_nEntirePieceNum)/nStripNumY, (m_nEntirePieceNum)/nStripNumY, (m_nEntirePieceNum)/nStripNumY, (m_nEntirePieceNum)/nStripNumY);
	strFileData += strData;
	strFileData += _T("\t-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------\r\n");

	strData.Format(_T("\t�� ǰ ��\t\t Strip  \t\t%8d\t\t%8d\t\t%8d\t\t%8d\t\r\n"), m_nEntireStripNum/nStripNumY - m_nStripOut[0], m_nEntireStripNum/nStripNumY - m_nStripOut[1], m_nEntireStripNum/nStripNumY - m_nStripOut[2], m_nEntireStripNum/nStripNumY - m_nStripOut[3]); // MAX_STRIP_NUM
	strFileData += strData;
	strFileData += _T("\t        \t\t-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------\r\n");
	strData.Format(_T("\t        \t\t  Unit  \t\t%8d\t\t%8d\t\t%8d\t\t%8d\t\r\n"), m_nEntirePieceNum/nStripNumY - m_nDefStrip[0], m_nEntirePieceNum/nStripNumY - m_nDefStrip[1], m_nEntirePieceNum/nStripNumY - m_nDefStrip[2], m_nEntirePieceNum/nStripNumY - m_nDefStrip[3]); // MAX_STRIP_NUM
	strFileData += strData;
	strFileData += _T("\t-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------\r\n");

	strData.Format(_T("\t�� �� ��\t\t Strip  \t\t%8d\t\t%8d\t\t%8d\t\t%8d\t\r\n"), m_nStripOut[0], m_nStripOut[1], m_nStripOut[2], m_nStripOut[3]); // MAX_STRIP_NUM
	strFileData += strData;
	strFileData += _T("\t        \t\t-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------\r\n");
	strData.Format(_T("\t        \t\t  Unit  \t\t%8d\t\t%8d\t\t%8d\t\t%8d\t\r\n"), (m_nDefStrip[0]), (m_nDefStrip[1]), (m_nDefStrip[2]), (m_nDefStrip[3])); // MAX_STRIP_NUM
	strFileData += strData;
	strFileData += _T("\t-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------\r\n");

	strData.Format(_T("\t�� ǰ ��\t\t Strip  \t\t%8.2f\t\t%8.2f\t\t%8.2f\t\t%8.2f\t\r\n"), 100.0*(double)(m_nEntireStripNum/nStripNumY - m_nStripOut[0])/(double)(m_nEntireStripNum/nStripNumY), 100.0*(double)(m_nEntireStripNum/nStripNumY - m_nStripOut[1])/(double)(m_nEntireStripNum/nStripNumY),
						100.0*(double)(m_nEntireStripNum/nStripNumY - m_nStripOut[2])/(double)(m_nEntireStripNum/nStripNumY), 100.0*(double)(m_nEntireStripNum/nStripNumY - m_nStripOut[3])/(double)(m_nEntireStripNum/nStripNumY)); // MAX_STRIP_NUM
	strFileData += strData;
	strFileData += _T("\t        \t\t-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------\r\n");
	strData.Format(_T("\t      %%   \t\t  Unit  \t\t%8.2f\t\t%8.2f\t\t%8.2f\t\t%8.2f\t\r\n"), 100.0*(double)(m_nEntirePieceNum/nStripNumY - m_nDefStrip[0])/(double)((m_nEntirePieceNum)/nStripNumY), 100.0*(double)(m_nEntirePieceNum/nStripNumY - m_nDefStrip[1])/(double)((m_nEntirePieceNum)/nStripNumY), 
						100.0*(double)(m_nEntirePieceNum/nStripNumY - m_nDefStrip[2])/(double)((m_nEntirePieceNum)/nStripNumY), 100.0*(double)(m_nEntirePieceNum/nStripNumY - m_nDefStrip[3])/(double)((m_nEntirePieceNum)/nStripNumY)); // MAX_STRIP_NUM
	strFileData += strData;
	strFileData += _T("\t-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------\r\n");

	strData.Format(_T("\t�� �� ��\t\t Strip  \t\t%8.2f\t\t%8.2f\t\t%8.2f\t\t%8.2f\t\r\n"), 100.0*(double)m_nStripOut[0]/(double)(m_nEntireStripNum/nStripNumY), 100.0*(double)m_nStripOut[1]/(double)(m_nEntireStripNum/nStripNumY),
						100.0*(double)m_nStripOut[2]/(double)(m_nEntireStripNum/nStripNumY), 100.0*(double)m_nStripOut[3]/(double)(m_nEntireStripNum/nStripNumY)); // MAX_STRIP_NUM
	strFileData += strData;
	strFileData += _T("\t        \t\t-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------\r\n");
	strData.Format(_T("\t      %%   \t\t  Unit  \t\t%8.2f\t\t%8.2f\t\t%8.2f\t\t%8.2f\t\r\n"), 100.0*(double)(m_nDefStrip[0])/(double)((m_nEntirePieceNum)/nStripNumY), 100.0*(double)(m_nDefStrip[1])/(double)((m_nEntirePieceNum)/nStripNumY), 
						100.0*(double)(m_nDefStrip[2])/(double)((m_nEntirePieceNum)/nStripNumY), 100.0*(double)(m_nDefStrip[3])/(double)((m_nEntirePieceNum)/nStripNumY) ); // MAX_STRIP_NUM
	strFileData += strData;
	strFileData += _T("\t-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------\r\n");

	strFileData += _T("\r\n");
	

	strFileData += _T("4. �ҷ� ����\r\n");
	strFileData += _T("\t-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------\r\n");
	strFileData += _T("\t��ȣ\t�ҷ�����\t1��\t\t2��\t\t3��\t\t4��\t\t������ �ҷ���\t\r\n");
	strFileData += _T("\t-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------\r\n");

	strData.Format(_T("\t1\t    %s    \t"), pDoc->m_pReelMap->m_sKorDef[DEF_OPEN]);
	strFileData += strData; // "\t 1\t    ����    \t");
	//strData.Format(_T("%13d"), m_nEntireAddedDefect[DEF_OPEN]);
	strData.Format(_T("%3d"), m_nDefPerStrip[0][DEF_OPEN]);
	strFileData += strData;
	strFileData += _T("\t\t");
	strData.Format(_T("%3d"), m_nDefPerStrip[1][DEF_OPEN]);
	strFileData += strData;
	strFileData += _T("\t\t");
	strData.Format(_T("%3d"), m_nDefPerStrip[2][DEF_OPEN]);
	strFileData += strData;
	strFileData += _T("\t\t");
	strData.Format(_T("%3d"), m_nDefPerStrip[3][DEF_OPEN]); // MAX_STRIP_NUM
	strFileData += strData;
	strFileData += _T("\t\t");
	strData.Format(_T("%13d"), m_nEntireAddedDefect[DEF_OPEN]);
	strFileData += strData;
	strFileData += _T("\r\n");

	strFileData += _T("\t-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------\r\n");

	strData.Format(_T("\t2\t    %s    \t"), pDoc->m_pReelMap->m_sKorDef[DEF_SHORT]);
	strFileData += strData;
	strData.Format(_T("%3d"), m_nDefPerStrip[0][DEF_SHORT]);
	strFileData += strData;
	strFileData += _T("\t\t");
	strData.Format(_T("%3d"), m_nDefPerStrip[1][DEF_SHORT]);
	strFileData += strData;
	strFileData += _T("\t\t");
	strData.Format(_T("%3d"), m_nDefPerStrip[2][DEF_SHORT]);
	strFileData += strData;
	strFileData += _T("\t\t");
	strData.Format(_T("%3d"), m_nDefPerStrip[3][DEF_SHORT]); // MAX_STRIP_NUM
	strFileData += strData;
	strFileData += _T("\t\t");
	strData.Format(_T("%13d"), m_nEntireAddedDefect[DEF_SHORT]);
	strFileData += strData;
	strFileData += _T("\r\n");

	strData.Format(_T("\t3\t    %s    \t"), pDoc->m_pReelMap->m_sKorDef[DEF_USHORT]);
	strFileData += strData;
	strData.Format(_T("%3d"), m_nDefPerStrip[0][DEF_USHORT]);
	strFileData += strData;
	strData.Format(_T("\t%3d"), m_nDefPerStrip[0][DEF_SHORT] + m_nDefPerStrip[0][DEF_USHORT]);
	strFileData += strData;
	strFileData += _T("\t");
	strData.Format(_T("%3d"), m_nDefPerStrip[1][DEF_USHORT]);
	strFileData += strData;
	strData.Format(_T("\t%3d"), m_nDefPerStrip[1][DEF_SHORT] + m_nDefPerStrip[1][DEF_USHORT]);
	strFileData += strData;
	strFileData += _T("\t");
	strData.Format(_T("%3d"), m_nDefPerStrip[2][DEF_USHORT]);
	strFileData += strData;
	strData.Format(_T("\t%3d"), m_nDefPerStrip[2][DEF_SHORT] + m_nDefPerStrip[2][DEF_USHORT]);
	strFileData += strData;
	strFileData += _T("\t");
	strData.Format(_T("%3d"), m_nDefPerStrip[3][DEF_USHORT]);
	strFileData += strData;
	strData.Format(_T("\t%3d"), m_nDefPerStrip[3][DEF_SHORT] + m_nDefPerStrip[3][DEF_USHORT]); // MAX_STRIP_NUM
	strFileData += strData;
	strFileData += _T("\t");
	strData.Format(_T("%13d"), m_nEntireAddedDefect[DEF_USHORT]);
	strFileData += strData;
	strData.Format(_T("  \t%3d"), m_nEntireAddedDefect[DEF_SHORT] + m_nEntireAddedDefect[DEF_USHORT]);
	strFileData += strData;
	strFileData += _T("\r\n");

	strFileData += _T("\t-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------\r\n");

	strData.Format(_T("\t4\t    %s    \t"), pDoc->m_pReelMap->m_sKorDef[DEF_NICK]);
	strFileData += strData; // "\t 4\t    ���    \t");
	strData.Format(_T("%3d"), m_nDefPerStrip[0][DEF_NICK]);
	strFileData += strData;
	strFileData += _T("\t\t");
	strData.Format(_T("%3d"), m_nDefPerStrip[1][DEF_NICK]);
	strFileData += strData;
	strFileData += _T("\t\t");
	strData.Format(_T("%3d"), m_nDefPerStrip[2][DEF_NICK]);
	strFileData += strData;
	strFileData += _T("\t\t");
	strData.Format(_T("%3d"), m_nDefPerStrip[3][DEF_NICK]); // MAX_STRIP_NUM
	strFileData += strData;
	strFileData += _T("\t\t");
	strData.Format(_T("%13d"), m_nEntireAddedDefect[DEF_NICK]);
	strFileData += strData;
	strFileData += _T("\r\n");

	strFileData += _T("\t-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------\r\n");

	strData.Format(_T("\t5\t    %s    \t"), pDoc->m_pReelMap->m_sKorDef[DEF_SPACE]);
	strFileData += strData; // "\t 5\t  ������  \t");
	strData.Format(_T("%3d"), m_nDefPerStrip[0][DEF_SPACE]);
	strFileData += strData;
	strFileData += _T("\t\t");
	strData.Format(_T("%3d"), m_nDefPerStrip[1][DEF_SPACE]);
	strFileData += strData;
	strFileData += _T("\t\t");
	strData.Format(_T("%3d"), m_nDefPerStrip[2][DEF_SPACE]);
	strFileData += strData;
	strFileData += _T("\t\t");
	strData.Format(_T("%3d"), m_nDefPerStrip[3][DEF_SPACE]); // MAX_STRIP_NUM
	strFileData += strData;
	strFileData += _T("\t\t");
	strData.Format(_T("%13d"), m_nEntireAddedDefect[DEF_SPACE]);
	strFileData += strData;
	strFileData += _T("\r\n");

	strData.Format(_T("\t6\t    %s    \t"), pDoc->m_pReelMap->m_sKorDef[DEF_EXTRA]);
	strFileData += strData; // "\t 6\t    �ܵ�    \t");
	strData.Format(_T("%3d"), m_nDefPerStrip[0][DEF_EXTRA]);
	strFileData += strData;
	strFileData += _T("\t\t");
	strData.Format(_T("%3d"), m_nDefPerStrip[1][DEF_EXTRA]);
	strFileData += strData;
	strFileData += _T("\t\t");
	strData.Format(_T("%3d"), m_nDefPerStrip[2][DEF_EXTRA]);
	strFileData += strData;
	strFileData += _T("\t\t");
	strData.Format(_T("%3d"), m_nDefPerStrip[3][DEF_EXTRA]); // MAX_STRIP_NUM
	strFileData += strData;
	strFileData += _T("\t\t");
	strData.Format(_T("%13d"), m_nEntireAddedDefect[DEF_EXTRA]);
	strFileData += strData;
	strFileData += _T("\r\n");

	strData.Format(_T("\t7\t    %s    \t"), pDoc->m_pReelMap->m_sKorDef[DEF_PROTRUSION]);
	strFileData += strData; // "\t 7\t    ����    \t");
	strData.Format(_T("%3d"), m_nDefPerStrip[0][DEF_PROTRUSION]);
	strFileData += strData;
	strData.Format(_T("\t%3d"), m_nDefPerStrip[0][DEF_SPACE] + m_nDefPerStrip[0][DEF_EXTRA] + m_nDefPerStrip[0][DEF_PROTRUSION]);
	strFileData += strData;
	strFileData += _T("\t");
	strData.Format(_T("%3d"), m_nDefPerStrip[1][DEF_PROTRUSION]);
	strFileData += strData;
	strData.Format(_T("\t%3d"), m_nDefPerStrip[1][DEF_SPACE] + m_nDefPerStrip[1][DEF_EXTRA] + m_nDefPerStrip[1][DEF_PROTRUSION]);
	strFileData += strData;
	strFileData += _T("\t");
	strData.Format(_T("%3d"), m_nDefPerStrip[2][DEF_PROTRUSION]);
	strFileData += strData;
	strData.Format(_T("\t%3d"), m_nDefPerStrip[2][DEF_SPACE] + m_nDefPerStrip[2][DEF_EXTRA] + m_nDefPerStrip[2][DEF_PROTRUSION]);
	strFileData += strData;
	strFileData += _T("\t");
	strData.Format(_T("%3d"), m_nDefPerStrip[3][DEF_PROTRUSION]);
	strFileData += strData;
	strData.Format(_T("\t%3d"), m_nDefPerStrip[3][DEF_SPACE] + m_nDefPerStrip[3][DEF_EXTRA] + m_nDefPerStrip[3][DEF_PROTRUSION]); // MAX_STRIP_NUM
	strFileData += strData;
	strFileData += _T("\t");
	strData.Format(_T("%13d"), m_nEntireAddedDefect[DEF_PROTRUSION]);
	strFileData += strData;
	strData.Format(_T("  \t%3d"), m_nEntireAddedDefect[DEF_SPACE] + m_nEntireAddedDefect[DEF_EXTRA] + m_nEntireAddedDefect[DEF_PROTRUSION]);
	strFileData += strData;
	strFileData += _T("\r\n");

	strFileData += _T("\t-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------\r\n");

	strData.Format(_T("\t8\t    %s    \t"), pDoc->m_pReelMap->m_sKorDef[DEF_PINHOLE]);
	strFileData += strData; // "\t 8\t    ��Ȧ    \t");
	strData.Format(_T("%3d"), m_nDefPerStrip[0][DEF_PINHOLE]);
	strFileData += strData;
	strFileData += _T("\t\t");
	strData.Format(_T("%3d"), m_nDefPerStrip[1][DEF_PINHOLE]);
	strFileData += strData;
	strFileData += _T("\t\t");
	strData.Format(_T("%3d"), m_nDefPerStrip[2][DEF_PINHOLE]);
	strFileData += strData;
	strFileData += _T("\t\t");
	strData.Format(_T("%3d"), m_nDefPerStrip[3][DEF_PINHOLE]); // MAX_STRIP_NUM
	strFileData += strData;
	strFileData += _T("\t\t");
	strData.Format(_T("%13d"), m_nEntireAddedDefect[DEF_PINHOLE]);
	strFileData += strData;
	strFileData += _T("\r\n");

	strData.Format(_T("\t9\t    %s    \t"), pDoc->m_pReelMap->m_sKorDef[DEF_PAD]);
	strFileData += strData; // "\t 9\t�е����\t");
	strData.Format(_T("%3d"), m_nDefPerStrip[0][DEF_PAD]);
	strFileData += strData;
	strData.Format(_T("\t%3d"), m_nDefPerStrip[0][DEF_PINHOLE] + m_nDefPerStrip[0][DEF_PAD]);
	strFileData += strData;
	strFileData += _T("\t");
	strData.Format(_T("%3d"), m_nDefPerStrip[1][DEF_PAD]);
	strFileData += strData;
	strData.Format(_T("\t%3d"), m_nDefPerStrip[1][DEF_PINHOLE] + m_nDefPerStrip[1][DEF_PAD]);
	strFileData += strData;
	strFileData += _T("\t");
	strData.Format(_T("%3d"), m_nDefPerStrip[2][DEF_PAD]);
	strFileData += strData;
	strData.Format(_T("\t%3d"), m_nDefPerStrip[2][DEF_PINHOLE] + m_nDefPerStrip[2][DEF_PAD]);
	strFileData += strData;
	strFileData += _T("\t");
	strData.Format(_T("%3d"), m_nDefPerStrip[3][DEF_PAD]);
	strFileData += strData;
	strData.Format(_T("\t%3d"), m_nDefPerStrip[3][DEF_PINHOLE] + m_nDefPerStrip[3][DEF_PAD]); // MAX_STRIP_NUM
	strFileData += strData;
	strFileData += _T("\t");
	strData.Format(_T("%13d"), m_nEntireAddedDefect[DEF_PAD]);
	strFileData += strData;
	strData.Format(_T("  \t%3d"), m_nEntireAddedDefect[DEF_PINHOLE] + m_nEntireAddedDefect[DEF_PAD]);
	strFileData += strData;
	strFileData += _T("\r\n");

	strFileData += _T("\t-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------\r\n");

	strData.Format(_T("\t10\t    %s    \t"), pDoc->m_pReelMap->m_sKorDef[DEF_HOLE_OPEN]);
	strFileData += strData; // "\t10\t  Ȧ����  \t");
	strData.Format(_T("%3d"), m_nDefPerStrip[0][DEF_HOLE_OPEN]);
	strFileData += strData;
	strFileData += _T("\t\t");
	strData.Format(_T("%3d"), m_nDefPerStrip[1][DEF_HOLE_OPEN]);
	strFileData += strData;
	strFileData += _T("\t\t");
	strData.Format(_T("%3d"), m_nDefPerStrip[2][DEF_HOLE_OPEN]);
	strFileData += strData;
	strFileData += _T("\t\t");
	strData.Format(_T("%3d"), m_nDefPerStrip[3][DEF_HOLE_OPEN]); // MAX_STRIP_NUM
	strFileData += strData;
	strFileData += _T("\t\t");
	strData.Format(_T("%13d"), m_nEntireAddedDefect[DEF_HOLE_OPEN]);
	strFileData += strData;
	strFileData += _T("\r\n");

	strFileData += _T("\t-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------\r\n");

	strData.Format(_T("\t11\t    %s    \t"), pDoc->m_pReelMap->m_sKorDef[DEF_HOLE_MISS]);
	strFileData += strData; // "\t11\t  Ȧ����  \t");
	strData.Format(_T("%3d"), m_nDefPerStrip[0][DEF_HOLE_MISS]);
	strFileData += strData;
	strFileData += _T("\t\t");
	strData.Format(_T("%3d"), m_nDefPerStrip[1][DEF_HOLE_MISS]);
	strFileData += strData;
	strFileData += _T("\t\t");
	strData.Format(_T("%3d"), m_nDefPerStrip[2][DEF_HOLE_MISS]);
	strFileData += strData;
	strFileData += _T("\t\t");
	strData.Format(_T("%3d"), m_nDefPerStrip[3][DEF_HOLE_MISS]); // MAX_STRIP_NUM
	strFileData += strData;
	strFileData += _T("\t\t");
	strData.Format(_T("%13d"), m_nEntireAddedDefect[DEF_HOLE_MISS]);
	strFileData += strData;
	strFileData += _T("\r\n");

	strData.Format(_T("\t12\t    %s    \t"), pDoc->m_pReelMap->m_sKorDef[DEF_HOLE_POSITION]);
	strFileData += strData; // "\t12\t  Ȧ���  \t");
	strData.Format(_T("%3d"), m_nDefPerStrip[0][DEF_HOLE_POSITION]);
	strFileData += strData;
	strFileData += _T("\t\t");
	strData.Format(_T("%3d"), m_nDefPerStrip[1][DEF_HOLE_POSITION]);
	strFileData += strData;
	strFileData += _T("\t\t");
	strData.Format(_T("%3d"), m_nDefPerStrip[2][DEF_HOLE_POSITION]);
	strFileData += strData;
	strFileData += _T("\t\t");
	strData.Format(_T("%3d"), m_nDefPerStrip[3][DEF_HOLE_POSITION]); // MAX_STRIP_NUM
	strFileData += strData;
	strFileData += _T("\t\t");
	strData.Format(_T("%13d"), m_nEntireAddedDefect[DEF_HOLE_POSITION]);
	strFileData += strData;
	strFileData += _T("\r\n");

	strData.Format(_T("\t13\t    %s    \t"), pDoc->m_pReelMap->m_sKorDef[DEF_HOLE_DEFECT]);
	strFileData += strData; // "\t13\tȦ���ҷ�\t");
	strData.Format(_T("%3d"), m_nDefPerStrip[0][DEF_HOLE_DEFECT]);
	strFileData += strData;
	strData.Format(_T("\t%3d"), m_nDefPerStrip[0][DEF_HOLE_MISS] + m_nDefPerStrip[0][DEF_HOLE_POSITION] + m_nDefPerStrip[0][DEF_HOLE_DEFECT]);
	strFileData += strData;
	strFileData += _T("\t");
	strData.Format(_T("%3d"), m_nDefPerStrip[1][DEF_HOLE_DEFECT]);
	strFileData += strData;
	strData.Format(_T("\t%3d"), m_nDefPerStrip[1][DEF_HOLE_MISS] + m_nDefPerStrip[1][DEF_HOLE_POSITION] + m_nDefPerStrip[1][DEF_HOLE_DEFECT]);
	strFileData += strData;
	strFileData += _T("\t");
	strData.Format(_T("%3d"), m_nDefPerStrip[2][DEF_HOLE_DEFECT]);
	strFileData += strData;
	strData.Format(_T("\t%3d"), m_nDefPerStrip[2][DEF_HOLE_MISS] + m_nDefPerStrip[2][DEF_HOLE_POSITION] + m_nDefPerStrip[2][DEF_HOLE_DEFECT]);
	strFileData += strData;
	strFileData += _T("\t");
	strData.Format(_T("%3d"), m_nDefPerStrip[3][DEF_HOLE_DEFECT]);
	strFileData += strData;
	strData.Format(_T("\t%3d"), m_nDefPerStrip[3][DEF_HOLE_MISS] + m_nDefPerStrip[3][DEF_HOLE_POSITION] + m_nDefPerStrip[3][DEF_HOLE_DEFECT]); // MAX_STRIP_NUM
	strFileData += strData;
	strFileData += _T("\t");
	strData.Format(_T("%13d"), m_nEntireAddedDefect[DEF_HOLE_DEFECT]);
	strFileData += strData;
	strData.Format(_T("  \t%3d"), m_nEntireAddedDefect[DEF_HOLE_MISS] + m_nEntireAddedDefect[DEF_HOLE_POSITION] + m_nEntireAddedDefect[DEF_HOLE_DEFECT]);
	strFileData += strData;
	strFileData += _T("\r\n");

	strFileData += _T("\t-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------\r\n");

	strData.Format(_T("\t14\t      %s      \t"), pDoc->m_pReelMap->m_sKorDef[DEF_POI]);
	strFileData += strData; // "\t14\t      POI      \t");
	strData.Format(_T("%3d"), m_nDefPerStrip[0][DEF_POI]);
	strFileData += strData;
	strFileData += _T("\t\t");
	strData.Format(_T("%3d"), m_nDefPerStrip[1][DEF_POI]);
	strFileData += strData;
	strFileData += _T("\t\t");
	strData.Format(_T("%3d"), m_nDefPerStrip[2][DEF_POI]);
	strFileData += strData;
	strFileData += _T("\t\t");
	strData.Format(_T("%3d"), m_nDefPerStrip[3][DEF_POI]);
	strFileData += strData;
	strFileData += _T("\t\t");
	strData.Format(_T("%13d"), m_nEntireAddedDefect[DEF_POI]);
	strFileData += strData;
	strFileData += _T("\r\n");

	strFileData += _T("\t-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------\r\n");

	strData.Format(_T("\t15\t    %s    \t"), pDoc->m_pReelMap->m_sKorDef[DEF_VH_OPEN]);
	strFileData += strData; // "\t15\t  VH����  \t");
	strData.Format(_T("%3d"), m_nDefPerStrip[0][DEF_VH_OPEN]);
	strFileData += strData;
	strFileData += _T("\t\t");
	strData.Format(_T("%3d"), m_nDefPerStrip[1][DEF_VH_OPEN]);
	strFileData += strData;
	strFileData += _T("\t\t");
	strData.Format(_T("%3d"), m_nDefPerStrip[2][DEF_VH_OPEN]);
	strFileData += strData;
	strFileData += _T("\t\t");
	strData.Format(_T("%3d"), m_nDefPerStrip[3][DEF_VH_OPEN]); // MAX_STRIP_NUM
	strFileData += strData;
	strFileData += _T("\t\t");
	strData.Format(_T("%13d"), m_nEntireAddedDefect[DEF_VH_OPEN]);
	strFileData += strData;
	strFileData += _T("\r\n");

	strFileData += _T("\t-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------\r\n");

	strData.Format(_T("\t16\t    %s    \t"), pDoc->m_pReelMap->m_sKorDef[DEF_VH_MISS]);
	strFileData += strData; // "\t16\t  VH����  \t");
	strData.Format(_T("%3d"), m_nDefPerStrip[0][DEF_VH_MISS]);
	strFileData += strData;
	strFileData += _T("\t\t");
	strData.Format(_T("%3d"), m_nDefPerStrip[1][DEF_VH_MISS]);
	strFileData += strData;
	strFileData += _T("\t\t");
	strData.Format(_T("%3d"), m_nDefPerStrip[2][DEF_VH_MISS]);
	strFileData += strData;
	strFileData += _T("\t\t");
	strData.Format(_T("%3d"), m_nDefPerStrip[3][DEF_VH_MISS]); // MAX_STRIP_NUM
	strFileData += strData;
	strFileData += _T("\t\t");
	strData.Format(_T("%13d"), m_nEntireAddedDefect[DEF_VH_MISS]);
	strFileData += strData;
	strFileData += _T("\r\n");

	strData.Format(_T("\t17\t    %s    \t"), pDoc->m_pReelMap->m_sKorDef[DEF_VH_POSITION]);
	strFileData += strData; // "\t17\t  VH���  \t");
	strData.Format(_T("%3d"), m_nDefPerStrip[0][DEF_VH_POSITION]);
	strFileData += strData;
	strFileData += _T("\t\t");
	strData.Format(_T("%3d"), m_nDefPerStrip[1][DEF_VH_POSITION]);
	strFileData += strData;
	strFileData += _T("\t\t");
	strData.Format(_T("%3d"), m_nDefPerStrip[2][DEF_VH_POSITION]);
	strFileData += strData;
	strFileData += _T("\t\t");
	strData.Format(_T("%3d"), m_nDefPerStrip[3][DEF_VH_POSITION]); // MAX_STRIP_NUM
	strFileData += strData;
	strFileData += _T("\t\t");
	strData.Format(_T("%13d"), m_nEntireAddedDefect[DEF_VH_POSITION]);
	strFileData += strData;
	strFileData += _T("\r\n");

	strData.Format(_T("\t18\t    %s    \t"), pDoc->m_pReelMap->m_sKorDef[DEF_VH_DEF]);
	strFileData += strData; // "\t18\t  VH����  \t");
	strData.Format(_T("%3d"), m_nDefPerStrip[0][DEF_VH_DEF]);
	strFileData += strData;
	strData.Format(_T("\t%3d"), m_nDefPerStrip[0][DEF_VH_MISS] + m_nDefPerStrip[0][DEF_VH_POSITION] + m_nDefPerStrip[0][DEF_VH_DEF]);
	strFileData += strData;
	strFileData += _T("\t");
	strData.Format(_T("%3d"), m_nDefPerStrip[1][DEF_VH_DEF]);
	strFileData += strData;
	strData.Format(_T("\t%3d"), m_nDefPerStrip[1][DEF_VH_MISS] + m_nDefPerStrip[1][DEF_VH_POSITION] + m_nDefPerStrip[1][DEF_VH_DEF]);
	strFileData += strData;
	strFileData += _T("\t");
	strData.Format(_T("%3d"), m_nDefPerStrip[2][DEF_VH_DEF]);
	strFileData += strData;
	strData.Format(_T("\t%3d"), m_nDefPerStrip[2][DEF_VH_MISS] + m_nDefPerStrip[2][DEF_VH_POSITION] + m_nDefPerStrip[2][DEF_VH_DEF]);
	strFileData += strData;
	strFileData += _T("\t");
	strData.Format(_T("%3d"), m_nDefPerStrip[3][DEF_VH_DEF]);
	strFileData += strData;
	strData.Format(_T("\t%3d"), m_nDefPerStrip[3][DEF_VH_MISS] + m_nDefPerStrip[3][DEF_VH_POSITION] + m_nDefPerStrip[3][DEF_VH_DEF]); // MAX_STRIP_NUM
	strFileData += strData;
	strFileData += _T("\t");
	strData.Format(_T("%13d"), m_nEntireAddedDefect[DEF_VH_DEF]);
	strFileData += strData;
	strData.Format(_T("  \t%3d"), m_nEntireAddedDefect[DEF_VH_MISS] + m_nEntireAddedDefect[DEF_VH_POSITION] + m_nEntireAddedDefect[DEF_VH_DEF]);
	strFileData += strData;
	strFileData += _T("\r\n");

	strFileData += _T("\t-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------\r\n");

	strData.Format(_T("\t19\t    %s    \t"), pDoc->m_pReelMap->m_sKorDef[DEF_LIGHT]);
	strFileData += strData; // "\t19\t    �뱤    \t");
	strData.Format(_T("%3d"), m_nDefPerStrip[0][DEF_LIGHT]);
	strFileData += strData;
	strFileData += _T("\t\t");
	strData.Format(_T("%3d"), m_nDefPerStrip[1][DEF_LIGHT]);
	strFileData += strData;
	strFileData += _T("\t\t");
	strData.Format(_T("%3d"), m_nDefPerStrip[2][DEF_LIGHT]);
	strFileData += strData;
	strFileData += _T("\t\t");
	strData.Format(_T("%3d"), m_nDefPerStrip[3][DEF_LIGHT]); // MAX_STRIP_NUM
	strFileData += strData;
	strFileData += _T("\t\t");
	strData.Format(_T("%13d"), m_nEntireAddedDefect[DEF_LIGHT]);
	strFileData += strData;
	strFileData += _T("\r\n");

	strFileData += _T("\t-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------\r\n");
	strFileData += _T("\r\n");

	return strFileData;
}


void CDlgMenu05::InsertLine(CString sPath)
{
	int i, nPos;
	CString sMsg;
	CDataFile *pDataFile = new CDataFile;

	if(!pDataFile->Open(sPath))
	{
		sMsg.Format(_T("%s File not found."), sPath);
		pView->MsgBox(sMsg);
		delete pDataFile;
		return;
	}
	CString sLine, sRemain, sNewLine;
	for(i=1; i<72; i++)
	{
		sLine = pDataFile->GetLineString(i);
		
		nPos = sLine.Find(_T("Start Serial="), 0);
		if(nPos >= 0)
			break;

		nPos = sLine.Find(_T("Total Pcs="), 0);
		if(nPos >= 0)
		{
			pDataFile->InsertLine(i-1, _T("End Serial=\r\n"));
			pDataFile->InsertLine(i-1, _T("Start Serial=\r\n"));
			pDataFile->InsertLine(i-1, _T("\r\n"));
			pDataFile->InsertLine(i-1, _T("// < ���� ���� > \r\n"));
			pDataFile->InsertLine(i-1, _T("\r\n"));
			pDataFile->Close();
			break;
		}
	}
	
	delete pDataFile;	

}

//void CDlgMenu05::DisplayReelMapData()
//{
//	//char FileD[MAX_PATH];
//	TCHAR FileD[MAX_PATH];
//	char *FileData;
//	CString strReelMapPath, strReelMapData, DsipMsg;
// 	int nFileSize, nRSize, i;	//, nSizeTemp	
//	FILE *fp;
//
//	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;
//
//	strReelMapData = _T("");
//	if(bDualTest)
//	{
//		strReelMapPath.Format(_T("%s%s\\%s\\%s\\ReelMapDataAll.txt"), pDoc->WorkingInfo.System.sPathOldFile, 
//															   m_sModel, m_sLot, m_sLayer);
//	}
//	else
//	{
//		strReelMapPath.Format(_T("%s%s\\%s\\%s\\ReelMapDataUp.txt"), pDoc->WorkingInfo.System.sPathOldFile, 
//															   m_sModel, m_sLot, m_sLayer);
//	}
//
//	InsertLine(strReelMapPath);
//
//	//������ �ҷ���. 
//	//strcpy(FileD, strReelMapPath);
//	wsprintf(FileD, TEXT("%s"), strReelMapPath);
//	char* pRtn = NULL;
//	if((fp = fopen(pRtn=TCHARToChar(FileD), "r")) != NULL)
//	{
//		fseek(fp, 0, SEEK_END);
//		nFileSize = ftell(fp);
//		fseek(fp, 0, SEEK_SET);
//
//		// Allocate space for a path name
//		FileData = (char*)calloc(nFileSize, sizeof(char));	
//		
//		nRSize = 0;
//		for( i=0; i < nFileSize ; i++ )
//		{
//			if( feof( fp ) != 0 )
//				break;
//
//			FileData[i] = fgetc(fp);
//
//			if(FileData[i] == '\n')
//			{
//				if(FileData[i-1] != '\r')
//				{
//					nFileSize++;
//					FileData = (char*)realloc(FileData, nFileSize);
//					FileData[i] = '\r';
//					i++;
//					nRSize++;
//					FileData[i] = '\n';
//				}
//			}
//
//			nRSize++;
//		}
//
//		strReelMapData.Format(_T("%s"), CharToString(FileData));
//
//
//		//fseek(fp, 0, SEEK_END);
//		//nFileSize = ftell(fp);
//		//fseek(fp, 0, SEEK_SET);
//		//
//		//// Allocate space for a path name
//		////FileData = (char*)malloc( nFileSize );
//		//FileData = (char*)calloc(nFileSize+1, sizeof(char));
//
//		//nRSize = fread(FileData, sizeof(char), nFileSize, fp);
//		//strReelMapData.Format(_T("%s"), FileData);
//
//		fclose(fp);
//		free( FileData );
//	}
//	else
//	{
//		DsipMsg.Format(_T("������ �������� �ʽ��ϴ�.\r\n%s"), strReelMapPath);
//		pView->ClrDispMsg();
//		AfxMessageBox(DsipMsg);
//	}
//
//	if(pRtn)
//		delete pRtn;
//	GetDlgItem(IDC_EDIT_RESULT)->SetWindowText(strReelMapData);
//}

void CDlgMenu05::DisplayReelMapUser()
{
	CString strReelMapPath, strReelMapData, sReelmapSrc;

	CString sModel, sLot, sLayer[2];
	//char szData[MAX_PATH];
	TCHAR szData[MAX_PATH];

	BOOL bSuccess[3] = {FALSE};
	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;

	if(bDualTest)
	{
		sReelmapSrc.Format(_T("%s%s\\%s\\%s\\ReelMapDataAll.txt"), pDoc->WorkingInfo.System.sPathOldFile, 
															   m_sModel, m_sLot, m_sLayer);
	}
	else
	{
		sReelmapSrc.Format(_T("%s%s\\%s\\%s\\ReelMapDataUp.txt"), pDoc->WorkingInfo.System.sPathOldFile, 
														   m_sModel, m_sLot, m_sLayer);
	}

	if (0 < ::GetPrivateProfileString(_T("Info"), _T("��      ��"), NULL, szData, sizeof(szData), sReelmapSrc))
		sModel = CString(szData); 
	else
	{
		pView->MsgBox(_T("Model ������ �����ϴ�."));
		//AfxMessageBox(_T("Model ������ �����ϴ�."));
		return;
	}

	if (0 < ::GetPrivateProfileString(_T("Info"), _T("��      Ʈ"), NULL, szData, sizeof(szData), sReelmapSrc))
		sLot = CString(szData); 
	else
	{
		pView->MsgBox(_T("Lot ������ �����ϴ�."));
		//AfxMessageBox(_T("Lot ������ �����ϴ�."));
		return;
	}
	
	if (0 < ::GetPrivateProfileString(_T("Info"), _T("��鷹�̾�"), NULL, szData, sizeof(szData), sReelmapSrc))
		sLayer[0] = CString(szData); 
	else
	{
		pView->MsgBox(_T("��鷹�̾� ������ �����ϴ�."));
		//AfxMessageBox(_T("��鷹�̾� ������ �����ϴ�."));
		return;
	}
	
	if(bDualTest)
	{
		if (0 < ::GetPrivateProfileString(_T("Info"), _T("�ϸ鷹�̾�"), NULL, szData, sizeof(szData), sReelmapSrc))
			sLayer[1] = CString(szData); 
		else
		{
			pView->MsgBox(_T("�ϸ鷹�̾� ������ �����ϴ�."));
			//AfxMessageBox(_T("�ϸ鷹�̾� ������ �����ϴ�."));
			return;
		}
	}

	if (0 >= ::GetPrivateProfileString(_T("Info"), _T("Marked Shot"), NULL, szData, sizeof(szData), sReelmapSrc))
	{
		pView->MsgBox(_T("���ʿ� Marked Shot ������ �����ϴ�."));
		//AfxMessageBox(_T("���ʿ� Marked Shot ������ �����ϴ�."));
		return;
	}

	double dRatio=0.0;
	CString sVal = _T("");
	CDlgProgress dlg;
	sVal.Format(_T("On remaking Reelmap."));
	dlg.Create(sVal);

	pDoc->m_pReelMapUp->m_bThreadAliveRemakeReelmap = FALSE;
	pDoc->m_pReelMapDn->m_bThreadAliveRemakeReelmap = FALSE;
	pDoc->m_pReelMapAllUp->m_bThreadAliveRemakeReelmap = FALSE;
	
//	pView->MyMsgBox(_T("On converting Reelmap....."));
	
	//sReelmapSrc.Format(_T("%s%s\\%s\\%s\\ReelMapDataUp.txt"), pDoc->WorkingInfo.System.sPathOldFile, 
	//													   sModel, sLot, sLayer[0]);
	if (pDoc->m_pReelMapUp)
		pDoc->m_pReelMapUp->StartThreadRemakeReelmap(); // RemakeReelmap(sReelmapSrc);

	if(bDualTest)
	{
		//sReelmapSrc.Format(_T("%s%s\\%s\\%s\\ReelMapDataDn.txt"), pDoc->WorkingInfo.System.sPathOldFile, 
		//													   m_sModel, sLot, sLayer[1]);
		if(pDoc->m_pReelMapDn)
			pDoc->m_pReelMapDn->StartThreadRemakeReelmap(); //RemakeReelmap(sReelmapSrc);

		//sReelmapSrc.Format(_T("%s%s\\%s\\%s\\ReelMapDataAll.txt"), pDoc->WorkingInfo.System.sPathOldFile, 
		//													   m_sModel, m_sLot, sLayer[0]);
		if(pDoc->m_pReelMapAllUp)
			pDoc->m_pReelMapAllUp->StartThreadRemakeReelmap(); // RemakeReelmap(sReelmapSrc);
	}

	DWORD dwSt = GetTickCount();
	do
	{
		if(dlg.GetSafeHwnd())
		{
			if(bDualTest)	
			{
				dRatio = pDoc->m_pReelMapAllUp->m_dProgressRatio + pDoc->m_pReelMapUp->m_dProgressRatio + pDoc->m_pReelMapDn->m_dProgressRatio;
				dRatio = (dRatio / 3.0);
			}
			else
				dRatio = pDoc->m_pReelMapUp->m_dProgressRatio;

			if(!(int(dRatio)%10))
				dlg.SetPos(int(dRatio));
		}	

		if(GetTickCount()-dwSt > 1200000)
			break;
	}while(pDoc->m_pReelMapUp->m_bThreadAliveRemakeReelmap || pDoc->m_pReelMapDn->m_bThreadAliveRemakeReelmap || pDoc->m_pReelMapAllUp->m_bThreadAliveRemakeReelmap);


	dlg.DestroyWindow();

	if(bDualTest)
	{
		bSuccess[0] = pDoc->m_pReelMapUp->m_bRtnThreadRemakeReelmap;	
		bSuccess[1] = pDoc->m_pReelMapDn->m_bRtnThreadRemakeReelmap;	
		bSuccess[2] = pDoc->m_pReelMapAllUp->m_bRtnThreadRemakeReelmap;	

		if(!bSuccess[0] || !bSuccess[1] || !bSuccess[2])
		{
			pView->MsgBox(_T("Reelmap Converting Failed."));
			return;
		}

		strReelMapPath.Format(_T("%s%s\\%s\\%s\\ReelMap\\ReelMap-TOTAL_SIDE.txt"), pDoc->WorkingInfo.System.sPathOldFile,
																			sModel, sLot, sLayer[0]);
	}
	else
	{
		if(!pDoc->m_pReelMapUp->m_bRtnThreadRemakeReelmap)
		{
			pView->MsgBox(_T("Reelmap Converting Failed."));
			return;
		}

		strReelMapPath.Format(_T("%s%s\\%s\\%s\\ReelMapDataUp.txt"), pDoc->WorkingInfo.System.sPathOldFile,
																			sModel, sLot, sLayer[0]);
	}

	strReelMapData = LoadFile(strReelMapPath);
	GetDlgItem(IDC_EDIT_RESULT)->SetWindowText(strReelMapData);

//	pView->MsgBox(_T("Done converting Reelmap....."));

}

CString CDlgMenu05::LoadFile(CString sPath)
{
	//char FileD[MAX_PATH];
	TCHAR FileD[MAX_PATH];
	char *FileData;
	CString DsipMsg=_T(""), sData=_T("");
 	int nFileSize, nRSize, i;	//, nSizeTemp	
	FILE *fp;

	CString sFile=_T(""), sTempPath=sPath;
	int nPos = sTempPath.ReverseFind('\\');
	if (nPos != -1)
	{
		sFile = sTempPath.Right(sTempPath.GetLength()-nPos-1);
		sTempPath.Delete(nPos, sPath.GetLength() - nPos);
	}

	double dRatio=0.0;
	CString sVal;
	CDlgProgress dlg;
	sVal.Format(_T("On loading [%s]"), sFile);
	dlg.Create(sVal);

	//������ �ҷ���. 
	//strcpy(FileD, sPath);
	_stprintf(FileD, TEXT("%s"), sPath);
	
	char* pRtn = NULL;
	if((fp = fopen(pRtn=TCHARToChar(FileD), "r")) != NULL)
	{
		fseek(fp, 0, SEEK_END);
		nFileSize = ftell(fp);
		fseek(fp, 0, SEEK_SET);

		// Allocate space for a path name
		FileData = (char*)calloc(nFileSize, sizeof(char));	
		
		nRSize = 0;
		for( i=0; i < nFileSize ; i++ )
		{
			if( feof( fp ) != 0 )
				break;

			dRatio = double(i+1)/double(nFileSize) * 100.0 ;
			if(!(int(dRatio)%10))
				dlg.SetPos(int(dRatio));

			FileData[i] = fgetc(fp);

			if(FileData[i] == '\n')
			{
				if(FileData[i-1] != '\r')
				{
					nFileSize++;
					FileData = (char*)realloc(FileData, nFileSize);
					FileData[i] = '\r';
					i++;
					nRSize++;
					FileData[i] = '\n';
				}
			}

			nRSize++;
		}

		sData.Format(_T("%s"), CharToString(FileData));

		fclose(fp);
		free( FileData );
	}
	else
	{
		DsipMsg.Format(_T("������ �������� �ʽ��ϴ�.\r\n%s"), sPath);
		pView->ClrDispMsg();
		AfxMessageBox(DsipMsg);
	}

	dlg.DestroyWindow();
	if(pRtn)
		delete pRtn;
	return sData;
}

void CDlgMenu05::OnBtnSearch() 
{
	// TODO: Add your control notification handler code here
	CString strLot;
	//char lpszItem[MAX_PATH];
	TCHAR lpszItem[MAX_PATH];

	GetDlgItem(IDC_STC_LOT)->GetWindowText(strLot);
	_stprintf(lpszItem, _T("%s"), strLot);	//strcpy(lpszItem, strLot);

	int nSel = ((CListBox*)GetDlgItem(IDC_LIST_LOT))->FindString(-1, lpszItem );
	if(LB_ERR == nSel)
	{
		pView->MsgBox(_T("�ش� ��Ʈ�� ã�� ���߽��ϴ�."));
		//AfxMessageBox(_T("�ش� ��Ʈ�� ã�� ���߽��ϴ�."));
		return;
	}
	((CListBox*)GetDlgItem(IDC_LIST_LOT))->SetCurSel(nSel);

// 	DisplayResultData();

	((CListBox*)GetDlgItem(IDC_LIST_LOT))->SetTopIndex(nSel);
}

void CDlgMenu05::OnChkReelmap() 
{
	// TODO: Add your control notification handler code here
    
	if(((CButton*)GetDlgItem(IDC_CHK_REELMAP))->GetCheck())
	{
		if(m_nCurSelLotIdx < 0)
		{
			pView->MsgBox(_T("��Ʈ�� ������ �ּ���."));
			//AfxMessageBox(_T("��Ʈ�� ������ �ּ���."));
			((CButton*)GetDlgItem(IDC_CHK_REELMAP))->SetCheck(FALSE);
			return;
		}
		if(m_nCurSelLayerIdx < 0)
		{
			pView->MsgBox(_T("���̾ ������ �ּ���."));
			//AfxMessageBox(_T("���̾ ������ �ּ���."));
			((CButton*)GetDlgItem(IDC_CHK_REELMAP))->SetCheck(FALSE);
			return;
		}
		((CListBox*)GetDlgItem(IDC_LIST_LOT))->SetCurSel(m_nCurSelLotIdx);
		((CComboBox*)GetDlgItem(IDC_COMBO_LAYER))->SetCurSel(m_nCurSelLayerIdx);
// 		DisplayReelMapData();
		DisplayReelMapUser();
	}
	else
	{
		DisplayResultData();
	}
}

BOOL CDlgMenu05::ShowKeypad(int nCtlID, CPoint ptSt, int nDir)
{
	BOOL bAdj = TRUE;
	CString strData, strPrev;
	GetDlgItemText(nCtlID, strData);
	strPrev = strData;

	CString strMin, strMax;
	CRect rect(0,0,0,0);
	CDlgKeyNum *pDlg = new CDlgKeyNum(&strData, &rect, ptSt, nDir);
	pDlg->m_strCurr = strData;
	pDlg->m_strMin = strMin = pDoc->GetMin(IDD_DLG_MENU_05, nCtlID);
	pDlg->m_strMax = strMax = pDoc->GetMax(IDD_DLG_MENU_05, nCtlID);
	pDlg->DoModal();
	delete pDlg;

	if(!strMin.IsEmpty() && !strMax.IsEmpty())
	{
		if(_tstof(strData) <_tstof(strMin) || 
			_tstof(strData) >_tstof(strMax))
		{
			SetDlgItemText(nCtlID, strPrev);
			pView->DispMsg(_T("�Է� ������ ������ϴ�."), _T("����"), RGB_YELLOW);
		}
		else
			SetDlgItemText(nCtlID, strData);
	}
	else
		SetDlgItemText(nCtlID, strData);
	
	return bAdj;
}

BOOL CDlgMenu05::ShowMyKeypad(int nCtlID, CPoint ptSt, int nDir)
{
	BOOL bAdj = TRUE;
	CString strData, strPrev;
	GetDlgItemText(nCtlID, strData);
	strPrev = strData;

	CString strMin, strMax;
	CRect rect(0,0,0,0);
	CDlgMyKeypad *pDlg = new CDlgMyKeypad(&strData, &rect, ptSt, nDir);
	pDlg->DoModal();
	delete pDlg;

	SetDlgItemText(nCtlID, strData);	
	return bAdj;
}

void CDlgMenu05::OnStcLot() 
{
	// TODO: Add your control notification handler code here
// 	myStcLot.SetBkColor(RGB_RED);
// 	myStcLot.RedrawWindow();
// 
// 	CPoint pt;	CRect rt;
// 	GetDlgItem(IDC_STC_LOT)->GetWindowRect(&rt);
// 	pt.x = rt.right; pt.y = rt.bottom;
// 	ShowKeypad(IDC_STC_LOT, pt, TO_BOTTOM|TO_RIGHT);
// 
// 	myStcLot.SetBkColor(RGB_WHITE);
// 	myStcLot.RedrawWindow();

	myStcLot.SetBkColor(RGB_RED);
	myStcLot.RedrawWindow();

	CPoint pt;	CRect rt;
	GetDlgItem(IDC_STC_LOT)->GetWindowRect(&rt);
	pt.x = rt.right; pt.y = rt.bottom;
	ShowMyKeypad(IDC_STC_LOT, pt, TO_BOTTOM|TO_RIGHT);

	myStcLot.SetBkColor(RGB_WHITE);
	myStcLot.RedrawWindow();	
}

void CDlgMenu05::SelchangeComboLayer(int nIndex)
{
	// TODO: Add your control notification handler code here
	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;

	CFileFind findfile;
		
	m_nCurSelLayerIdx = nIndex;
	((CComboBox*)GetDlgItem(IDC_COMBO_LAYER))->SetCurSel(nIndex);

	if (nIndex != LB_ERR)
	{
		((CComboBox*)GetDlgItem(IDC_COMBO_LAYER))->GetLBText(nIndex, m_sLayer);

		CString sRes, sLayer, sRemain;
		int nPos;

		if (m_sLayer.Find(_T("TOP")) >= 0)
		{
			m_sLayerUp = m_sLayer;
			sRemain = m_sLayer;
			nPos = m_sLayerUp.ReverseFind(_T('-'));
			if (nPos >= 0)
			{
				sRes = sRemain.Right(sRemain.GetLength() - nPos - 1);
				sRemain.Delete(nPos, sRemain.GetLength() - nPos);

				nPos = sRemain.ReverseFind(_T('-'));
				if (nPos >= 0)
				{
					sLayer = sRemain.Right(sRemain.GetLength() - nPos - 1);
					sRemain.Delete(nPos, sRemain.GetLength() - nPos);
				}

				if (sLayer == _T("1"))
				{
					m_sLayerDn.Format(_T("BOTTOM-2-%s"), sRes);
					if (pDoc->GetTestMode() != MODE_NONE)
					{
						pDoc->SetTestMode(MODE_INNER);
						DispTestMode();
					}
				}
				else
				{
					m_sLayerDn.Format(_T("BOTTOM-4-%s"), sRes);
					if (pDoc->GetTestMode() != MODE_NONE)
					{
						pDoc->SetTestMode(MODE_OUTER);
						DispTestMode();
					}
				}
			}
		}
		else if (m_sLayer.Find(_T("BOTTOM")) >= 0)
		{
			m_sLayerDn = m_sLayer;
			sRemain = m_sLayer;
			nPos = m_sLayerUp.ReverseFind(_T('-'));
			if (nPos >= 0)
			{
				sRes = sRemain.Right(sRemain.GetLength() - nPos - 1);
				sRemain.Delete(nPos, sRemain.GetLength() - nPos);

				nPos = sRemain.ReverseFind(_T('-'));
				if (nPos >= 0)
				{
					sLayer = sRemain.Right(sRemain.GetLength() - nPos - 1);
					sRemain.Delete(nPos, sRemain.GetLength() - nPos);
				}

				if (sLayer == _T("2"))
				{
					m_sLayerUp.Format(_T("TOP-1-%s"), sRes);
					if (pDoc->GetTestMode() != MODE_NONE)
					{
						pDoc->SetTestMode(MODE_INNER);
						DispTestMode();
					}
				}
				else
				{
					m_sLayerUp.Format(_T("TOP-3-%s"), sRes);
					if (pDoc->GetTestMode() != MODE_NONE)
					{
						pDoc->SetTestMode(MODE_OUTER);
						DispTestMode();
					}
				}
			}
		}


		CString sPath, sVal;

		sPath.Format(_T("%s%s\\%s\\%s"), pDoc->WorkingInfo.System.sPathOldFile,
			m_sModel, m_sLot, m_sLayerUp);

		m_sRmapPath.Format(_T("%s\\OFFLINE\\ReelMapDataAll.txt"), sPath);
		if (!findfile.FindFile(m_sRmapPath))
		{
			m_sRmapPath.Format(_T("%s\\OFFLINE\\ReelMapDataUp.txt"), sPath);
			if (!findfile.FindFile(m_sRmapPath))
			{
				if (bDualTest)
					m_sRmapPath.Format(_T("%s\\ReelMapDataAll.txt"), sPath);
				else
					m_sRmapPath.Format(_T("%s\\ReelMapDataUp.txt"), sPath);
			}
			else
			{
				bDualTest = FALSE;
			}
		}
		else
		{
			bDualTest = TRUE;
		}
		

		//char szData[MAX_PATH];
		TCHAR szData[MAX_PATH];
		if (0 < ::GetPrivateProfileString(_T("Info"), _T("Start Serial"), NULL, szData, sizeof(szData), m_sRmapPath))
			m_nSerialSt = _tstoi(szData);
		else
			m_nSerialSt = 1;

		sVal = _T("");
		if (m_nSerialSt > 0)
			sVal.Format(_T("%d"), m_nSerialSt);
		myStcSerialSt.SetText(sVal);

		if (0 < ::GetPrivateProfileString(_T("Info"), _T("Completed Shot"), NULL, szData, sizeof(szData), m_sRmapPath))
			m_nCompletedShot = _tstoi(szData);
		else
			m_nCompletedShot = 0; // Failed.

		if (0 < ::GetPrivateProfileString(_T("Info"), _T("Marked Shot"), NULL, szData, sizeof(szData), m_sRmapPath))
			m_nMarkedShot = _tstoi(szData);
		else
			m_nMarkedShot = 0; // Failed.

		if (0 < ::GetPrivateProfileString(_T("Info"), _T("End Serial"), NULL, szData, sizeof(szData), m_sRmapPath))
			m_nSerialEd = _tstoi(szData);
		else
			m_nSerialEd = (m_nMarkedShot > m_nCompletedShot) ? m_nMarkedShot : m_nCompletedShot;

		sVal = _T("");
		if (m_nSerialEd > 0)
			sVal.Format(_T("%d"), m_nSerialEd);
		myStcSerialEd.SetText(sVal);

		ReloadReelmap();
		DispProcCode(m_sRmapPath);

		if (((CButton*)GetDlgItem(IDC_CHK_REELMAP))->GetCheck())
		{
			if (m_nCurSelLotIdx < 0)
			{
				pView->MsgBox(_T("��Ʈ�� ������ �ּ���."));
				//AfxMessageBox(_T("��Ʈ�� ������ �ּ���."));
				((CButton*)GetDlgItem(IDC_CHK_REELMAP))->SetCheck(FALSE);
				return;
			}
			if (m_nCurSelLayerIdx < 0)
			{
				pView->MsgBox(_T("���̾ ������ �ּ���."));
				//AfxMessageBox(_T("���̾ ������ �ּ���."));
				((CButton*)GetDlgItem(IDC_CHK_REELMAP))->SetCheck(FALSE);
				return;
			}
			((CListBox*)GetDlgItem(IDC_LIST_LOT))->SetCurSel(m_nCurSelLotIdx);
			((CComboBox*)GetDlgItem(IDC_COMBO_LAYER))->SetCurSel(m_nCurSelLayerIdx);
			//DisplayReelMapData();
			DisplayReelMapUser();
		}
		else
		{
			DisplayResultData();
		}
	}
}

void CDlgMenu05::OnSelchangeComboLayer() 
{
	// TODO: Add your control notification handler code here
	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;
	int nIndex = m_nCurSelLayerIdx = ((CComboBox*)GetDlgItem(IDC_COMBO_LAYER))->GetCurSel();
	if (nIndex != LB_ERR)
	{
		((CComboBox*)GetDlgItem(IDC_COMBO_LAYER))->GetLBText(nIndex, m_sLayer);

		CString sRes, sLayer, sRemain;
		int nPos;

		if (m_sLayer.Find(_T("TOP")) >= 0)
		{
			m_sLayerUp = m_sLayer;
			sRemain = m_sLayer;
			nPos = m_sLayerUp.ReverseFind(_T('-'));
			if (nPos >= 0)
			{
				sRes = sRemain.Right(sRemain.GetLength() - nPos - 1);
				sRemain.Delete(nPos, sRemain.GetLength() - nPos);

				nPos = sRemain.ReverseFind(_T('-'));
				if (nPos >= 0)
				{
					sLayer = sRemain.Right(sRemain.GetLength() - nPos - 1);
					sRemain.Delete(nPos, sRemain.GetLength() - nPos);
				}

				if(sLayer == _T("1"))
					m_sLayerDn.Format(_T("BOTTOM-2-%s"), sRes);
				else
					m_sLayerDn.Format(_T("BOTTOM-4-%s"), sRes);
			}
		}
		else if (m_sLayer.Find(_T("BOTTOM")) >= 0)
		{
			m_sLayerDn = m_sLayer;
			sRemain = m_sLayer;
			nPos = m_sLayerDn.ReverseFind(_T('-'));
			if (nPos >= 0)
			{
				sRes = sRemain.Right(sRemain.GetLength() - nPos - 1);
				sRemain.Delete(nPos, sRemain.GetLength() - nPos);

				nPos = sRemain.ReverseFind(_T('-'));
				if (nPos >= 0)
				{
					sLayer = sRemain.Right(sRemain.GetLength() - nPos - 1);
					sRemain.Delete(nPos, sRemain.GetLength() - nPos);
				}

				if (sLayer == _T("2"))
					m_sLayerUp.Format(_T("TOP-1-%s"), sRes);
				else
					m_sLayerUp.Format(_T("TOP-3-%s"), sRes);
			}
		}


		CString sPath, sVal;
		sPath.Format(_T("%s%s\\%s\\%s"), pDoc->WorkingInfo.System.sPathOldFile, 
			m_sModel, m_sLot, m_sLayerUp);
		
		if(bDualTest)
			m_sRmapPath.Format(_T("%s\\ReelMapDataAll.txt"), sPath);
		else
			m_sRmapPath.Format(_T("%s\\ReelMapDataUp.txt"), sPath);

		//char szData[MAX_PATH];
		TCHAR szData[MAX_PATH];
		if (0 < ::GetPrivateProfileString(_T("Info"), _T("Start Serial"), NULL, szData, sizeof(szData), m_sRmapPath))
			m_nSerialSt = _tstoi(szData);
		else
			m_nSerialSt = 1;

		sVal = _T("");
		if(m_nSerialSt > 0)
			sVal.Format(_T("%d"), m_nSerialSt);
		myStcSerialSt.SetText(sVal);

		if (0 < ::GetPrivateProfileString(_T("Info"), _T("Completed Shot"), NULL, szData, sizeof(szData), m_sRmapPath))
			m_nCompletedShot = _tstoi(szData); 
		else
			m_nCompletedShot = 0; // Failed.

		if (0 < ::GetPrivateProfileString(_T("Info"), _T("Marked Shot"), NULL, szData, sizeof(szData), m_sRmapPath))
			m_nMarkedShot = _tstoi(szData); 
		else
			m_nMarkedShot = 0; // Failed.

		if (0 < ::GetPrivateProfileString(_T("Info"), _T("End Serial"), NULL, szData, sizeof(szData), m_sRmapPath))
			m_nSerialEd = _tstoi(szData);
		else
			m_nSerialEd = (m_nMarkedShot > m_nCompletedShot) ? m_nMarkedShot : m_nCompletedShot;

		sVal = _T("");
		if(m_nSerialEd > 0)
			sVal.Format(_T("%d"), m_nSerialEd);
		myStcSerialEd.SetText(sVal);

		ReloadReelmap();
		DispProcCode(m_sRmapPath);
		
		if(((CButton*)GetDlgItem(IDC_CHK_REELMAP))->GetCheck())
		{
			if(m_nCurSelLotIdx < 0)
			{
				pView->MsgBox(_T("��Ʈ�� ������ �ּ���."));
				//AfxMessageBox(_T("��Ʈ�� ������ �ּ���."));
				((CButton*)GetDlgItem(IDC_CHK_REELMAP))->SetCheck(FALSE);
				return;
			}
			if(m_nCurSelLayerIdx < 0)
			{
				pView->MsgBox(_T("���̾ ������ �ּ���."));
				//AfxMessageBox(_T("���̾ ������ �ּ���."));
				((CButton*)GetDlgItem(IDC_CHK_REELMAP))->SetCheck(FALSE);
				return;
			}
			((CListBox*)GetDlgItem(IDC_LIST_LOT))->SetCurSel(m_nCurSelLotIdx);
			((CComboBox*)GetDlgItem(IDC_COMBO_LAYER))->SetCurSel(m_nCurSelLayerIdx);
			//DisplayReelMapData();
			DisplayReelMapUser();
		}
		else
		{
			DisplayResultData();
		}

		//DispTestMode();

	}
}

void CDlgMenu05::OnBtnSave() 
{
	// TODO: Add your control notification handler code here
	if (pView->IsRun() || pView->IsBuffer())
	{
		pView->MsgBox(_T("�۾����Դϴ�."));
		return;
	}
//	if(IDNO == pView->DoMyMsgBox(_T("�˻����� �����Ͻðڽ��ϱ�?"), MB_YESNO))
	if(IDNO == pView->MsgBox(_T("�˻����� �����Ͻðڽ��ϱ�?"), 0, MB_YESNO))
		return;
	
	// File Save......
	CString strDestPath, strPath;

	CFileFind findfile;

	if (pDoc->m_pReelMapUp)
	{
		strPath = pDoc->m_pReelMapUp->GetRmapPathOnOffline(RMAP_ALLUP);
		if (!findfile.FindFile(strPath))
		{
			strPath = pDoc->m_pReelMapUp->GetRmapPathOnOffline(RMAP_UP);
			if (!findfile.FindFile(strPath))
			{
				strDestPath.Format(_T("%s%s\\%s\\%s.txt"), pDoc->WorkingInfo.System.sPathOldFile,
					m_sModel, m_sLot, _T("Result"));
			}
			else
			{
				strDestPath.Format(_T("%s%s\\%s\\%s.txt"), pDoc->WorkingInfo.System.sPathOldFile,
					m_sModel, m_sLot, _T("Result_VS"));
			}
		}
		else
		{
			strDestPath.Format(_T("%s%s\\%s\\%s.txt"), pDoc->WorkingInfo.System.sPathOldFile, 
												   m_sModel, m_sLot, _T("Result_VS"));
		}
	}
	else
		strDestPath.Format(_T("%s%s\\%s\\%s.txt"), pDoc->WorkingInfo.System.sPathOldFile, 
												   m_sModel, m_sLot, _T("Result"));

	CFileFind cFile;
	if(cFile.FindFile(strDestPath))
		DeleteFile(strDestPath);

	//////////////////////////////////////////////////////////
	// Directory location of Work file
	CString strData;
	strData = TxtDataMDS();
	TCHAR lpszCurDirPathFile[MAX_PATH];
	//char lpszCurDirPathFile[MAX_PATH];
	//strcpy(lpszCurDirPathFile, strDestPath);	
	_stprintf(lpszCurDirPathFile, _T("%s"), strDestPath);
	
	CFile file;
	CFileException pError;
	if(!file.Open(lpszCurDirPathFile,CFile::modeWrite,&pError))
	{
		if(!file.Open(lpszCurDirPathFile,CFile::modeCreate | CFile::modeWrite,&pError))
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
	return;
}

CString CDlgMenu05::TxtDataMDS()
{
	CString strFileData, strData;
	int nTot;
	nTot = m_nDefPerStrip[0][DEF_LIGHT] + m_nDefPerStrip[1][DEF_LIGHT] + m_nDefPerStrip[2][DEF_LIGHT] + m_nDefPerStrip[3][DEF_LIGHT];

	//����Ʈ �ۼ�. =====================================================================

	//strFileData = _T("\r\n\r\n");
	strFileData = _T("");
	strFileData += _T("1. �Ϲ� ����\r\n");
	strData.Format(_T("    �� �� �� :  %s\r\n"), m_sModel);
	strFileData += strData;
	strData.Format(_T("    �� Ʈ �� :  %s\r\n"), m_sLot);
	strFileData += strData;
	strData.Format(_T("    ���۽ð� :  %s\r\n"), m_strLotStartTime);
	strFileData += strData;
	strData.Format(_T("    ����ð� :  %s\r\n"), m_strLotWorkingTime);
	strFileData += strData;
	strData.Format(_T("    ����ð� :  %s\r\n"), m_strLotEndTime);
	strFileData += strData;
	strFileData += _T("\r\n");
	
	strFileData += _T("2. ��ǰ �˻� ���\r\n");
	strData.Format(_T("    �� �� �� Unit �� : %9d\r\n"), m_nEntirePieceNum);
	strFileData += strData;
	strData.Format(_T("    ��    ǰ Unit �� : %9d        ��  ǰ  ��(%%) : %9.2f\r\n"), m_nEntirePieceNum - m_nDefectPieceNum, 100.0*(double)(m_nEntirePieceNum-m_nDefectPieceNum)/(double)m_nEntirePieceNum);
	strFileData += strData;
	strData.Format(_T("    ��    �� Unit �� : %9d        ��  ��  ��(%%) : %9.2f\r\n"), m_nDefectPieceNum, 100.0*(double)m_nDefectPieceNum/(double)m_nEntirePieceNum);
	strFileData += strData;
	strFileData += _T("\r\n");

	strFileData += _T("3. ���� �˻� ����\r\n");
	strData.Format(_T("    1�� �˻� Unit �� : %9d\r\n"), m_nEntirePieceNum/ MAX_STRIP_NUM);
	strFileData += strData;
	strData.Format(_T("    1�� ��ǰ Unit �� : %9d        1�� ��ǰ��(%%) : %9.2f\r\n"), m_nEntirePieceNum/ MAX_STRIP_NUM - m_nDefStrip[0], 100.0*(double)(m_nEntirePieceNum/ MAX_STRIP_NUM - m_nDefStrip[0])/(double)(m_nEntirePieceNum/ MAX_STRIP_NUM));
	strFileData += strData;
	strData.Format(_T("    1�� �ҷ� Unit �� : %9d        1�� �ҷ���(%%) : %9.2f\r\n"), m_nDefStrip[0], 100.0*(double)m_nDefStrip[0]/(double)(m_nEntirePieceNum/ MAX_STRIP_NUM));
	strFileData += strData;
	strFileData += _T("\r\n");
	strData.Format(_T("    2�� �˻� Unit �� : %9d\r\n"), m_nEntirePieceNum/ MAX_STRIP_NUM);
	strFileData += strData;
	strData.Format(_T("    2�� ��ǰ Unit �� : %9d        2�� ��ǰ��(%%) : %9.2f\r\n"), m_nEntirePieceNum/ MAX_STRIP_NUM - m_nDefStrip[1], 100.0*(double)(m_nEntirePieceNum/ MAX_STRIP_NUM - m_nDefStrip[1])/(double)(m_nEntirePieceNum/ MAX_STRIP_NUM));
	strFileData += strData;
	strData.Format(_T("    2�� �ҷ� Unit �� : %9d        2�� �ҷ���(%%) : %9.2f\r\n"), m_nDefStrip[1], 100.0*(double)m_nDefStrip[1]/(double)(m_nEntirePieceNum/ MAX_STRIP_NUM));
	strFileData += strData;
	strFileData += _T("\r\n");
	strData.Format(_T("    3�� �˻� Unit �� : %9d\r\n"), m_nEntirePieceNum/ MAX_STRIP_NUM);
	strFileData += strData;
	strData.Format(_T("    3�� ��ǰ Unit �� : %9d        3�� ��ǰ��(%%) : %9.2f\r\n"), m_nEntirePieceNum/ MAX_STRIP_NUM - m_nDefStrip[2], 100.0*(double)(m_nEntirePieceNum/ MAX_STRIP_NUM - m_nDefStrip[2])/(double)(m_nEntirePieceNum/ MAX_STRIP_NUM));
	strFileData += strData;
	strData.Format(_T("    3�� �ҷ� Unit �� : %9d        3�� �ҷ���(%%) : %9.2f\r\n"), m_nDefStrip[2], 100.0*(double)m_nDefStrip[2]/(double)(m_nEntirePieceNum/ MAX_STRIP_NUM));
	strFileData += strData;
	strFileData += _T("\r\n");
	strData.Format(_T("    4�� �˻� Unit �� : %9d\r\n"), m_nEntirePieceNum/ MAX_STRIP_NUM);
	strFileData += strData;
	strData.Format(_T("    4�� ��ǰ Unit �� : %9d        4�� ��ǰ��(%%) : %9.2f\r\n"), m_nEntirePieceNum/ MAX_STRIP_NUM - m_nDefStrip[3], 100.0*(double)(m_nEntirePieceNum/ MAX_STRIP_NUM - m_nDefStrip[3])/(double)(m_nEntirePieceNum/ MAX_STRIP_NUM));
	strFileData += strData;
	strData.Format(_T("    4�� �ҷ� Unit �� : %9d        4�� �ҷ���(%%) : %9.2f\r\n"), m_nDefStrip[3], 100.0*(double)m_nDefStrip[3]/(double)(m_nEntirePieceNum/ MAX_STRIP_NUM));
	strFileData += strData;
	strFileData += _T("\r\n");

	strFileData += _T("4. �ҷ� ����\r\n");
	strFileData += _T("    -----------------------------------------------------------------------------\r\n");
	strFileData += _T("    ��ȣ     �ҷ�����       1��       2��       3��       4��       ������ �ҷ���\r\n");
	strFileData += _T("    -----------------------------------------------------------------------------\r\n");
	strData.Format(_T("     1         ����  %10d%10d%10d%10d%20d\r\n"), m_nDefPerStrip[0][DEF_OPEN], m_nDefPerStrip[1][DEF_OPEN], m_nDefPerStrip[2][DEF_OPEN], m_nDefPerStrip[3][DEF_OPEN], m_nEntireAddedDefect[DEF_OPEN]);
	strFileData += strData;
	strData.Format(_T("     2         ��Ʈ  %10d%10d%10d%10d%20d\r\n"), m_nDefPerStrip[0][DEF_SHORT], m_nDefPerStrip[1][DEF_SHORT], m_nDefPerStrip[2][DEF_SHORT], m_nDefPerStrip[3][DEF_SHORT], m_nEntireAddedDefect[DEF_SHORT]);
	strFileData += strData;
	strData.Format(_T("     3        U-��Ʈ %10d%10d%10d%10d%20d\r\n"), m_nDefPerStrip[0][DEF_USHORT], m_nDefPerStrip[1][DEF_USHORT], m_nDefPerStrip[2][DEF_USHORT], m_nDefPerStrip[3][DEF_USHORT], m_nEntireAddedDefect[DEF_USHORT]);
	strFileData += strData;
	strData.Format(_T("     4         ���  %10d%10d%10d%10d%20d\r\n"), m_nDefPerStrip[0][DEF_NICK], m_nDefPerStrip[1][DEF_NICK], m_nDefPerStrip[2][DEF_NICK], m_nDefPerStrip[3][DEF_NICK], m_nEntireAddedDefect[DEF_NICK]);
	strFileData += strData;
	strData.Format(_T("     5        ������ %10d%10d%10d%10d%20d\r\n"), m_nDefPerStrip[0][DEF_SPACE], m_nDefPerStrip[1][DEF_SPACE], m_nDefPerStrip[2][DEF_SPACE], m_nDefPerStrip[3][DEF_SPACE], m_nEntireAddedDefect[DEF_SPACE]);
	strFileData += strData;
	strData.Format(_T("     6         �ܵ�  %10d%10d%10d%10d%20d\r\n"), m_nDefPerStrip[0][DEF_EXTRA], m_nDefPerStrip[1][DEF_EXTRA], m_nDefPerStrip[2][DEF_EXTRA], m_nDefPerStrip[3][DEF_EXTRA], m_nEntireAddedDefect[DEF_EXTRA]);
	strFileData += strData;
	strData.Format(_T("     7         ����  %10d%10d%10d%10d%20d\r\n"), m_nDefPerStrip[0][DEF_PROTRUSION], m_nDefPerStrip[1][DEF_PROTRUSION], m_nDefPerStrip[2][DEF_PROTRUSION], m_nDefPerStrip[3][DEF_PROTRUSION], m_nEntireAddedDefect[DEF_PROTRUSION]);
	strFileData += strData;
	strData.Format(_T("     8         ��Ȧ  %10d%10d%10d%10d%20d\r\n"), m_nDefPerStrip[0][DEF_PINHOLE], m_nDefPerStrip[1][DEF_PINHOLE], m_nDefPerStrip[2][DEF_PINHOLE], m_nDefPerStrip[3][DEF_PINHOLE], m_nEntireAddedDefect[DEF_PINHOLE]);
	strFileData += strData;
	strData.Format(_T("     9       �е����%10d%10d%10d%10d%20d\r\n"), m_nDefPerStrip[0][DEF_PAD], m_nDefPerStrip[1][DEF_PAD], m_nDefPerStrip[2][DEF_PAD], m_nDefPerStrip[3][DEF_PAD], m_nEntireAddedDefect[DEF_PAD]);
	strFileData += strData;
	strData.Format(_T("    10        Ȧ���� %10d%10d%10d%10d%20d\r\n"), m_nDefPerStrip[0][DEF_HOLE_OPEN], m_nDefPerStrip[1][DEF_HOLE_OPEN], m_nDefPerStrip[2][DEF_HOLE_OPEN], m_nDefPerStrip[3][DEF_HOLE_OPEN], m_nEntireAddedDefect[DEF_HOLE_OPEN]);
	strFileData += strData;
	strData.Format(_T("    11        Ȧ���� %10d%10d%10d%10d%20d\r\n"), m_nDefPerStrip[0][DEF_HOLE_MISS], m_nDefPerStrip[1][DEF_HOLE_MISS], m_nDefPerStrip[2][DEF_HOLE_MISS], m_nDefPerStrip[3][DEF_HOLE_MISS], m_nEntireAddedDefect[DEF_HOLE_MISS]);
	strFileData += strData;
	strData.Format(_T("    12        Ȧ��� %10d%10d%10d%10d%20d\r\n"), m_nDefPerStrip[0][DEF_HOLE_POSITION], m_nDefPerStrip[1][DEF_HOLE_POSITION], m_nDefPerStrip[2][DEF_HOLE_POSITION], m_nDefPerStrip[3][DEF_HOLE_POSITION], m_nEntireAddedDefect[DEF_HOLE_POSITION]);
	strFileData += strData;
	strData.Format(_T("    13       Ȧ���ҷ�%10d%10d%10d%10d%20d\r\n"), m_nDefPerStrip[0][DEF_HOLE_DEFECT], m_nDefPerStrip[1][DEF_HOLE_DEFECT], m_nDefPerStrip[2][DEF_HOLE_DEFECT], m_nDefPerStrip[3][DEF_HOLE_DEFECT], m_nEntireAddedDefect[DEF_HOLE_DEFECT]);
	strFileData += strData;
	strData.Format(_T("    14          POI  %10d%10d%10d%10d%20d\r\n"), m_nDefPerStrip[0][DEF_POI], m_nDefPerStrip[1][DEF_POI], m_nDefPerStrip[2][DEF_POI], m_nDefPerStrip[3][DEF_POI], m_nEntireAddedDefect[DEF_POI]);
	strFileData += strData;
	strData.Format(_T("    15        VH���� %10d%10d%10d%10d%20d\r\n"), m_nDefPerStrip[0][DEF_VH_OPEN], m_nDefPerStrip[1][DEF_VH_OPEN], m_nDefPerStrip[2][DEF_VH_OPEN], m_nDefPerStrip[3][DEF_VH_OPEN], m_nEntireAddedDefect[DEF_VH_OPEN]);
	strFileData += strData;
	strData.Format(_T("    16        VH���� %10d%10d%10d%10d%20d\r\n"), m_nDefPerStrip[0][DEF_VH_MISS], m_nDefPerStrip[1][DEF_VH_MISS], m_nDefPerStrip[2][DEF_VH_MISS], m_nDefPerStrip[3][DEF_VH_MISS], m_nEntireAddedDefect[DEF_VH_MISS]);
	strFileData += strData;
	strData.Format(_T("    17        VH��� %10d%10d%10d%10d%20d\r\n"), m_nDefPerStrip[0][DEF_VH_POSITION], m_nDefPerStrip[1][DEF_VH_POSITION], m_nDefPerStrip[2][DEF_VH_POSITION], m_nDefPerStrip[3][DEF_VH_POSITION], m_nEntireAddedDefect[DEF_VH_POSITION]);
	strFileData += strData;
	strData.Format(_T("    18        VH���� %10d%10d%10d%10d%20d\r\n"), m_nDefPerStrip[0][DEF_VH_DEF], m_nDefPerStrip[1][DEF_VH_DEF], m_nDefPerStrip[2][DEF_VH_DEF], m_nDefPerStrip[3][DEF_VH_DEF], m_nEntireAddedDefect[DEF_VH_DEF]);
	strFileData += strData;
	strData.Format(_T("    19         �뱤  %10d%10d%10d%10d%20d\r\n"), m_nDefPerStrip[0][DEF_LIGHT], m_nDefPerStrip[1][DEF_LIGHT], m_nDefPerStrip[2][DEF_LIGHT], m_nDefPerStrip[3][DEF_LIGHT], nTot);
	strFileData += strData;
	strFileData += _T("    -----------------------------------------------------------------------------\r\n");
	strFileData += _T("                                                                                 \r\n");
	strFileData += _T("                                                                                 \r\n");
	strFileData += _T("                                                                                 \r\n");
	strFileData += _T("\r\n");

	return strFileData;
}

BOOL CDlgMenu05::ReloadReelmap()
{
	int nFirstShot, nLastShot;
	CFileFind findfile;
	CString sPath, sRmapPath, sMsg;
	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;

	sPath.Format(_T("%s%s\\%s\\%s"), pDoc->WorkingInfo.System.sPathOldFile, 
								 m_sModel, m_sLot, m_sLayerUp);

	m_sRmapPath.Format(_T("%s\\OFFLINE\\ReelMapDataAll.txt"), sPath);
	if (!findfile.FindFile(m_sRmapPath))
	{
		m_sRmapPath.Format(_T("%s\\OFFLINE\\ReelMapDataUp.txt"), sPath);
		if (!findfile.FindFile(m_sRmapPath))
		{
			if (bDualTest)
				m_sRmapPath.Format(_T("%s\\ReelMapDataAll.txt"), sPath);
			else
				m_sRmapPath.Format(_T("%s\\ReelMapDataUp.txt"), sPath);
		}
		else
		{
			bDualTest = FALSE;
		}
	}
	else
	{
		bDualTest = TRUE;
	}

	sRmapPath = m_sRmapPath;
	BOOL bExist = findfile.FindFile(sRmapPath);

	if(!bExist) // Can not find file.
	{
		sMsg.Format(_T("��� ������ ���� ���߽��ϴ�.\r\n%s"), sRmapPath);
		pView->ClrDispMsg();
		AfxMessageBox(sMsg);
		return FALSE; // Failed.
	}

	//char szData[MAX_PATH];
	TCHAR szData[MAX_PATH];

	if(m_nSerialSt > 0)
		nFirstShot = m_nSerialSt;
	else
		nFirstShot = 1;
	
	if(m_nSerialEd > 0)
		nLastShot = m_nSerialEd;
	else
	{
		if (0 < ::GetPrivateProfileString(_T("Info"), _T("Marked Shot"), NULL, szData, sizeof(szData), sRmapPath))
			nLastShot = _tstoi(szData); 
		else
		{
			nLastShot = 0; // Failed.
			return FALSE;
		}
	}


	int nNodeX=0;// = pDoc->m_pPcsRgn->nCol; // on Cam
	int nNodeY=0;// = pDoc->m_pPcsRgn->nRow; // on Cam
	
	if (0 < ::GetPrivateProfileString(_T("Info"), _T("CamPcsX"), NULL, szData, sizeof(szData), sRmapPath))
		nNodeX = _tstoi(szData); 
	if (0 < ::GetPrivateProfileString(_T("Info"), _T("CamPcsY"), NULL, szData, sizeof(szData), sRmapPath))
		nNodeY = _tstoi(szData); 


	int nTotalPcs = nNodeX * nNodeY;
	int nStripPcs = nTotalPcs / MAX_STRIP_NUM;
	int nX, nY, nSt=0;
	CString sPnl, sRow, sVal;
	if(!nNodeX || !nNodeY)
	{
		for(nY=0; nY<10000; nY++)
		{
			sPnl = _T("1");
			sRow.Format(_T("%02d"), nY);
			if(0 >= ::GetPrivateProfileString(sPnl, sRow, NULL, szData, sizeof(szData), sRmapPath))
			{
				m_nNodeX = nNodeX = nY;
				break;
			}
			else
			{
				if(!nNodeY)
				{
					CString sLine = CString(szData);
					for(nX=0; nX<100000; nX++)
					{
						nSt = sLine.Find(',', nSt+1);
						if(nSt < 0)
							break;
						else
							nNodeY++;
					}
					nNodeY++;
				}
			}
		}
	}
	m_nNodeY = nNodeY;


	int nDefStrip[4];
	int nPnl, nRow, nCol, nDefCode, i, k, nStrip, nC, nR;
	//char sep[] = {",/;\r\n\t"};
	TCHAR sep[] = { _T(",/;\r\n\t") };
	int nTotPcs, nGoodPcs, nBadPcs, nDef[MAX_DEF];	// [DefCode] : Total Num.

	nTotPcs = 0;
	nGoodPcs = 0;
	nBadPcs = 0;
	for(i=0; i<MAX_DEF; i++)
		nDef[i] = 0;

	m_nTotStOut = 0;
	for(k=0; k<MAX_STRIP_NUM; k++)
	{
		m_nStripOut[k] = 0;
		m_nDefStrip[k] = 0;
		for(i=1; i<MAX_DEF; i++)
			m_nDefPerStrip[k][i] = 0;
	}

	double dRatio=0.0;
	CDlgProgress dlg;

	if(bDualTest)
		sVal.Format(_T("On reloading [%s]"), _T("ReelMapDataAll.txt"));
	else
		sVal.Format(_T("On reloading [%s]"), _T("ReelMapDataUp.txt"));

	dlg.Create(sVal);

	for(nPnl=nFirstShot-1; nPnl<nLastShot; nPnl++)
	{
		nDefStrip[0] = 0; nDefStrip[1] = 0; nDefStrip[2] = 0; nDefStrip[3] = 0;
		
		dRatio = double(nPnl+1)/double(nLastShot) * 100.0;
		if(!(int(dRatio)%10))
			dlg.SetPos(int(dRatio));

		for(nRow=0; nRow<nNodeX; nRow++)
		{
			sPnl.Format(_T("%d"), nPnl+1);
			sRow.Format(_T("%02d"), nRow);
// 			sRow.Format(_T("%d"), nRow+1);
			if (0 < ::GetPrivateProfileString(sPnl, sRow, NULL, szData, sizeof(szData), sRmapPath))
			{
				for(nCol=0; nCol<nNodeY; nCol++)
				{
					if(nCol==0)
						sVal = _tcstok(szData,sep);
						//sVal = strtok(szData,sep);
					else
						sVal = _tcstok(NULL,sep);

					nDefCode = _tstoi(sVal);
 					//pPnlBuf[nPnl][nRow][nCol] = nDefCode;
					nDef[nDefCode]++;
					nTotPcs++;
					if(nDefCode > 0)
						nBadPcs++;
					else
						nGoodPcs++;
					
					nR = (nNodeY-1)-nCol;
					nC = nRow;

					nStrip = int(nR / (nNodeY / MAX_STRIP_NUM));
					if(nStrip > -1 && nStrip < MAX_STRIP_NUM)
					{
						if(nDefCode > 0)
						{
							nDefStrip[nStrip]++;
							m_nDefStrip[nStrip]++;
							m_nDefPerStrip[nStrip][nDefCode]++;
						}
					}

				}
			}
		}

		double dStOutRto =_tstof(pDoc->WorkingInfo.LastJob.sStripOutRatio) / 100.0;
		for(nStrip=0; nStrip<MAX_STRIP_NUM; nStrip++)
		{
			if(nDefStrip[nStrip] >= nStripPcs * dStOutRto)
				m_nStripOut[nStrip]++;
		}
	}

	dlg.DestroyWindow();


	// ���� ����Ÿ�� ������.

	// Piece infomation..............
	CString strData, strMenu, strItem;

	strData.Format(_T("%d"), nTotPcs);
	::WritePrivateProfileString(_T("Info"), _T("Total Pcs"), strData, sRmapPath);

	strData.Format(_T("%d"), nGoodPcs);
	::WritePrivateProfileString(_T("Info"), _T("Good Pcs"), strData, sRmapPath);

	strData.Format(_T("%d"), nBadPcs);
	::WritePrivateProfileString(_T("Info"), _T("Bad Pcs"), strData, sRmapPath);

	for(i=1; i<MAX_DEF; i++)
	{
		strMenu.Format(_T("%d"), i);
		strData.Format(_T("%d"), nDef[i]); // �ҷ��̸��� �ҷ���
		::WritePrivateProfileString(_T("Info"), strMenu, strData, sRmapPath);
	}

	int nTotStOut = 0;
	for(k=0; k<MAX_STRIP_NUM; k++)
	{
		strMenu.Format(_T("Strip%d"), k);
		strData.Format(_T("%d"), m_nDefStrip[k]);
		::WritePrivateProfileString(_T("Info"), strMenu, strData, sRmapPath);
		
		strMenu.Format(_T("%d"), k);
		strData.Format(_T("%d"), m_nStripOut[k]);
		::WritePrivateProfileString(_T("StripOut"), strMenu, strData, sRmapPath);
		nTotStOut += m_nStripOut[k];

		for(i=1; i<MAX_DEF; i++)
		{
			strItem.Format(_T("Strip%d"), k);
			strMenu.Format(_T("%d"), i);
			strData.Format(_T("%d"), m_nDefPerStrip[k][i]);
			::WritePrivateProfileString(strItem, strMenu, strData, sRmapPath);
		}			
	}
	strData.Format(_T("%d"), m_nTotStOut=nTotStOut);
	::WritePrivateProfileString(_T("StripOut"), _T("Total"), strData, sRmapPath);

	for(i=1; i<MAX_DEF; i++)
	{
		strMenu.Format(_T("%d"), i);
		strData.Format(_T("%d"), nDef[i]); // �ҷ��̸��� �ҷ���
		::WritePrivateProfileString(_T("Info"), strMenu, strData, sRmapPath);
	}

	return TRUE;
}


void CDlgMenu05::OnStcSerialSt() 
{
	// TODO: Add your control notification handler code here
	myStcSerialSt.SetBkColor(RGB_RED);
	myStcSerialSt.RedrawWindow();
	
	CPoint pt;	CRect rt;
	GetDlgItem(IDC_STC_SERIAL_ST)->GetWindowRect(&rt);
	pt.x = rt.right; pt.y = rt.bottom;
	ShowKeypad(IDC_STC_SERIAL_ST, pt, TO_BOTTOM|TO_RIGHT);
	
	myStcSerialSt.SetBkColor(RGB_WHITE);
	myStcSerialSt.RedrawWindow();
	
	CString sData;
	GetDlgItem(IDC_STC_SERIAL_ST)->GetWindowText(sData);

	if(!m_sRmapPath.IsEmpty() && !sData.IsEmpty())
		::WritePrivateProfileString(_T("Info"), _T("Start Serial"), sData, m_sRmapPath);
}

void CDlgMenu05::OnStcSerialEd() 
{
	// TODO: Add your control notification handler code here
	myStcSerialEd.SetBkColor(RGB_RED);
	myStcSerialEd.RedrawWindow();
	
	CPoint pt;	CRect rt;
	GetDlgItem(IDC_STC_SERIAL_ED)->GetWindowRect(&rt);
	pt.x = rt.right; pt.y = rt.bottom;
	ShowKeypad(IDC_STC_SERIAL_ED, pt, TO_BOTTOM|TO_RIGHT);
	
	myStcSerialEd.SetBkColor(RGB_WHITE);
	myStcSerialEd.RedrawWindow();
	
	CString sData;
	GetDlgItem(IDC_STC_SERIAL_ED)->GetWindowText(sData);
	
	if(!m_sRmapPath.IsEmpty() && !sData.IsEmpty())
		::WritePrivateProfileString(_T("Info"), _T("End Serial"), sData, m_sRmapPath);
}

void CDlgMenu05::OnStcProc() 
{
	// TODO: Add your control notification handler code here
	myStcProc.SetBkColor(RGB_RED);
	myStcProc.RedrawWindow();
	
	CPoint pt;	CRect rt;
	GetDlgItem(IDC_STC_PROC)->GetWindowRect(&rt);
	pt.x = rt.right; pt.y = rt.bottom;
	ShowMyKeypad(IDC_STC_PROC, pt, TO_TOP|TO_RIGHT);
	//ShowKeypad(IDC_STC_PROC, pt, TO_BOTTOM|TO_RIGHT);
	
	myStcProc.SetBkColor(RGB_WHITE);
	myStcProc.RedrawWindow();
	
	CString sData;
	GetDlgItem(IDC_STC_PROC)->GetWindowText(sData);
	

	CString sMsg;	

	if(m_sModel.IsEmpty())
		pView->MsgBox(_T("�� ������ �����ϴ�."));
		//AfxMessageBox(_T("�� ������ �����ϴ�."));

	if(m_sLot.IsEmpty())
		pView->MsgBox(_T("��Ʈ ������ �����ϴ�."));
		//AfxMessageBox(_T("��Ʈ ������ �����ϴ�."));

	if(sData.IsEmpty())
		pView->MsgBox(_T("�����ڵ� ������ �����ϴ�."));
		//AfxMessageBox(_T("�����ڵ� ������ �����ϴ�."));

	if(m_sProcessNum.Compare(sData)) // FALSE: Ideal Equal, TRUE: Different. 
	{
		sMsg.Format(_T("���� �����ڵ�: %s\r\n���ο� �����ڵ�: %s\r\n������ �����ڵ带 ���ο� �����ڵ�� �ٲٽðڽ��ϱ�?"), m_sProcessNum, sData);
//		if(IDYES == pView->DoMyMsgBox(sMsg, MB_YESNO))
		if(IDYES == pView->MsgBox(sMsg, 0, MB_YESNO))
		{
			CFileFind findfile;
			if(!findfile.FindFile(m_sRmapPath)) // Can not find file.
			{
				CFile f1;
				if( !f1.Open( m_sRmapPath, CFile::modeCreate | CFile::modeWrite ) )
				{
					sMsg.Format(_T("���� ������ ���� ���߽��ϴ�.\r\n%s"), m_sRmapPath);
					pView->ClrDispMsg();
					AfxMessageBox(sMsg);
					return;
				}
				f1.Close();
			}
			
			m_sProcessNum = sData;
			pDoc->UpdateProcessNum(m_sProcessNum);
			//::WritePrivateProfileString(_T("Info"), _T("Process Code"), m_sProcessNum, m_sRmapPath);
		}
		else
			myStcProc.SetText(m_sProcessNum);
	}
}

void CDlgMenu05::DispProcCode(CString sPath)
{
	myStcProc.SetText(GetProcCode(sPath));
}

CString CDlgMenu05::GetProcCode(CString sPath)
{
	CString str;
	//char szData[MAX_PATH];
	TCHAR szData[MAX_PATH];

	if (0 < ::GetPrivateProfileString(_T("Info"), _T("Process Code"), NULL, szData, sizeof(szData), sPath))
		str = CString(szData);
	else
		str = _T("");

	return str;
}

void CDlgMenu05::OnBtnSave4() 
{
	// TODO: Add your control notification handler code here
	if (pView->IsRun() || pView->IsBuffer())
	{
		pView->MsgBox(_T("�۾����Դϴ�."));
		return;
	}

	if(IDYES==pView->MsgBox(_T("ITS���� ������ �����Ͻðڽ��ϱ�?"), 0, MB_YESNO))
		MakeIts();
}

void CDlgMenu05::MakeIts()
{
	CString sItsPath = pDoc->WorkingInfo.System.sPathIts;

	if (sItsPath.IsEmpty())
		return;

	int pos = sItsPath.ReverseFind('\\');
	if (pos != -1)
		sItsPath.Delete(pos, sItsPath.GetLength() - pos);

	if (!pDoc->DirectoryExists(sItsPath))
		CreateDirectory(sItsPath, NULL);

	CFileFind cFile;
	CString sPathPcr;

	sPathPcr.Format(_T("%s%s\\%s\\%s\\OFFLINE\\*.pcr"), 
		pDoc->WorkingInfo.System.sPathOldFile, m_sModel, m_sLot, m_sLayer);

	BOOL bExist = cFile.FindFile(sPathPcr);
	if (!bExist)
	{
		sPathPcr.Format(_T("%s%s\\%s\\%s\\*.pcr"),
			pDoc->WorkingInfo.System.sPathOldFile, m_sModel, m_sLot, m_sLayer);

		bExist = cFile.FindFile(sPathPcr);
		if (!bExist)
		{
			return; // pcr������ �������� ����.
		}
	}

	int nLayer = -1; // RMAP_NONE = -1, RMAP_UP = 0, RMAP_DN = 1
	if (m_sLayer == m_sLayerUp)
	{
		if (pDoc->GetTestMode() == MODE_INNER)
			nLayer = RMAP_INNER_UP;
		else
			nLayer = RMAP_UP;
	}
	else if (m_sLayer == m_sLayerDn)
	{
		if (pDoc->GetTestMode() == MODE_INNER)
			nLayer = RMAP_INNER_DN;
		else
			nLayer = RMAP_DN;
	}

	//CString sPathReelmapUp=_T(""), sPathReelmapDn=_T("");
	//CString strPath;
	//
	//if (pDoc->m_pReelMapUp)
	//{
	//	strPath = pDoc->m_pReelMapUp->GetRmapPathOnOffline(RMAP_ALLUP);
	//	if (!cFile.FindFile(strPath))
	//	{
	//		strPath = pDoc->m_pReelMapUp->GetRmapPathOnOffline(RMAP_UP);
	//		if (!cFile.FindFile(strPath))
	//		{
	//			sPathReelmapUp.Format(_T("%s%s\\%s\\%s\\ReelMapDataUp.txt"),
	//				pDoc->WorkingInfo.System.sPathOldFile, m_sModel, m_sLot, m_sLayer);
	//			sPathReelmapDn.Format(_T("%s%s\\%s\\%s\\ReelMapDataDn.txt"),
	//				pDoc->WorkingInfo.System.sPathOldFile, m_sModel, m_sLot, m_sLayer);
	//		}
	//		else
	//		{
	//			sPathReelmapUp = pDoc->m_pReelMapUp->GetRmapPathOnOffline(RMAP_UP);
	//		}
	//	}
	//	else
	//	{
	//		sPathReelmapUp = pDoc->m_pReelMapUp->GetRmapPathOnOffline(RMAP_UP);
	//		sPathReelmapDn = pDoc->m_pReelMapUp->GetRmapPathOnOffline(RMAP_DN);
	//	}
	//}
	//else
	//{
	//	sPathReelmapUp.Format(_T("%s%s\\%s\\%s\\ReelMapDataUp.txt"),
	//		pDoc->WorkingInfo.System.sPathOldFile, m_sModel, m_sLot, m_sLayer);
	//	sPathReelmapDn.Format(_T("%s%s\\%s\\%s\\ReelMapDataDn.txt"),
	//		pDoc->WorkingInfo.System.sPathOldFile, m_sModel, m_sLot, m_sLayer);
	//}
	//
	//int nLayer = -1; // RMAP_NONE = -1, RMAP_UP = 0, RMAP_DN = 1
	//CFileFind cFile2;
	//bExist = cFile2.FindFile(sPathReelmapUp);
	//if (bExist)
	//{
	//	if (pDoc->GetTestMode() == MODE_INNER)
	//		nLayer = RMAP_INNER_UP;
	//	else
	//		nLayer = RMAP_UP;
	//}
	//else
	//{
	//	bExist = cFile2.FindFile(sPathReelmapDn);
	//	if (bExist)
	//	{
	//		if (pDoc->GetTestMode() == MODE_INNER)
	//			nLayer = RMAP_INNER_DN;
	//		else
	//			nLayer = RMAP_DN;
	//	}
	//	else
	//		return; // Layer�Ӽ��� ������ �������� ����.
	//}


	pDoc->m_Master[0].Init(pDoc->WorkingInfo.System.sPathCamSpecDir, m_sModel, m_sLayer);
	pDoc->m_Master[0].LoadMstInfo();


	int nPos, nSerial;

	CString sFileName, sSerial;
	CString sNewName;
	BOOL bNewModel;
	int nNewLot;

	int nTot = 0;
	while (bExist)
	{
		bExist = cFile.FindNextFile();
		if (cFile.IsDots()) continue;
		if (!cFile.IsDirectory())
		{
			sFileName = cFile.GetFileName();
			nPos = sFileName.ReverseFind('.');
			if (nPos > 0)
				sSerial = sFileName.Left(nPos);

			nSerial = _tstoi(sSerial);

			if(nLayer == RMAP_UP || nLayer == RMAP_INNER_UP)
				LoadPCRUpFromMk(nSerial);
			else
				LoadPCRDnFromMk(nSerial);

			MakeItsFile(nSerial, nLayer);

			nTot++;
		}
	}
}

void CDlgMenu05::MakeItsFile(int nSerial, int nLayer)
{
	CString sItsPath = pDoc->GetItsTargetPath(nSerial, nLayer);	// RMAP_UP, RMAP_DN, RMAP_INNER_UP, RMAP_INNER_DN
	CFileFind cFile;

	if (cFile.FindFile(sItsPath))
		DeleteFile(sItsPath);

	char FileName[MAX_PATH];
	StringToChar(sItsPath, FileName);

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
		pView->MsgBox(_T("It is trouble to MakeItsFile."));
		return;
	}

	fclose(fp);
}

CString CDlgMenu05::GetItsFileData(int nSerial, int nLayer) // RMAP_UP, RMAP_DN, RMAP_INNER_UP, RMAP_INNER_DN
{
	//CString sItsCode = pDoc->m_sItsCode;
	//CString sPath = pDoc->GetItsPath(nSerial, nLayer);

	CString str = _T(""), sSide = _T(""), sTemp = _T(""), sItsData = _T("");
	//CString sItsCode = pDoc->WorkingInfo.LastJob.sEngItsCode;
	CString sItsCode = pDoc->m_sItsCode;
	//CString sItsCode = m_sLot;

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
		//if (pDoc->m_pPcrInner[0])
		//{
		//	if (pDoc->m_pPcrInner[0][nIdx])
		//		nTotDefPcs = pDoc->m_pPcrInner[0][nIdx]->m_nTotDef;
		//}
		if (pDoc->m_pPcr[nLayer])
		{
			if (pDoc->m_pPcr[nLayer][nIdx])
				nTotDefPcs = pDoc->m_pPcr[nLayer][nIdx]->m_nTotDef;
		}
		break;
	case RMAP_INNER_DN:
		nLayer = RMAP_DN;
		sSide = _T("B");
		//if (pDoc->m_pPcrInner[1])
		//{
		//	if (pDoc->m_pPcrInner[1][nIdx])
		//		nTotDefPcs = pDoc->m_pPcrInner[1][nIdx]->m_nTotDef;
		//}
		if (pDoc->m_pPcr[nLayer])
		{
			if (pDoc->m_pPcr[nLayer][nIdx])
				nTotDefPcs = pDoc->m_pPcr[nLayer][nIdx]->m_nTotDef;
		}
		break;
	}


	int nR, nRow, nCol, nDefCode;
	int nTotStrip[4] = { 0 }, nPcsId;

	int nTotVerifyed = 0;
	int nPcrLineNum;
	CString sStripA = _T("");
	CString sStripB = _T("");
	CString sStripC = _T("");
	CString sStripD = _T("");

	CfPoint ptPnt;

	// Strip A
	for (nRow = 0; nRow < nStripY; nRow++)
	{
		nR = nRow;
		for (nCol = 0; nCol < nNodeX; nCol++)
		{
			//pDoc->m_Master[0].m_pPcsRgn->GetMkPnt(nCol, nRow, nPcsId, ptPnt);
			nPcrLineNum = pDoc->m_pPcr[nLayer][nIdx]->m_arPcrLineNum[nR][nCol];
			if (nPcrLineNum > -1)
			{
				if (pDoc->m_pPcr[nLayer][nIdx]->m_pMk[nPcrLineNum] != -2) // -2 (NoMarking)
				{
					nDefCode = pDoc->m_pPcr[nLayer][nIdx]->m_arDefType[nR][nCol]; //m_pPnlBuf[nSerial - 1][nR][nCol]; // DefCode 3D Array : [nSerial][nRow][nCol] - �������� ������.
					if (nDefCode > 0)
					{
						nTotStrip[0]++;
						str.Format(_T("%s,%04d,%s,A,%d,%d,B%d\n"), sItsCode, nSerial, sSide, nCol + 1, nRow + 1, pDoc->GetItsDefCode(nDefCode));
						sStripA += str;
					}
				}
				else
				{
					nTotVerifyed++;
				}
			}
		}
	}

	// Strip B
	for (nRow = 0; nRow < nStripY; nRow++)
	{
		nR = nRow + nStripY;
		for (nCol = 0; nCol < nNodeX; nCol++)
		{
			nPcrLineNum = pDoc->m_pPcr[nLayer][nIdx]->m_arPcrLineNum[nR][nCol];
			if (nPcrLineNum > -1)
			{
				if (pDoc->m_pPcr[nLayer][nIdx]->m_pMk[nPcrLineNum] != -2) // -2 (NoMarking)
				{
					nDefCode = pDoc->m_pPcr[nLayer][nIdx]->m_arDefType[nR][nCol]; //m_pPnlBuf[nSerial - 1][nR][nCol]; // DefCode 3D Array : [nSerial][nRow][nCol] - �������� ������.
					if (nDefCode > 0)
					{
						nTotStrip[1]++;
						str.Format(_T("%s,%04d,%s,B,%d,%d,B%d\n"), sItsCode, nSerial, sSide, nCol + 1, nRow + 1, pDoc->GetItsDefCode(nDefCode));
						sStripB += str;
					}
				}
				else
				{
					nTotVerifyed++;
				}
			}
		}
	}

	// Strip C
	for (nRow = 0; nRow < nStripY; nRow++)
	{
		nR = nRow + nStripY * 2;
		for (nCol = 0; nCol < nNodeX; nCol++)
		{
			nPcrLineNum = pDoc->m_pPcr[nLayer][nIdx]->m_arPcrLineNum[nR][nCol];
			if (nPcrLineNum > -1)
			{
				if (pDoc->m_pPcr[nLayer][nIdx]->m_pMk[nPcrLineNum] != -2) // -2 (NoMarking)
				{
					nDefCode = pDoc->m_pPcr[nLayer][nIdx]->m_arDefType[nR][nCol]; //m_pPnlBuf[nSerial - 1][nR][nCol]; // DefCode 3D Array : [nSerial][nRow][nCol] - �������� ������.
					if (nDefCode > 0)
					{
						nTotStrip[2]++;
						str.Format(_T("%s,%04d,%s,C,%d,%d,B%d\n"), sItsCode, nSerial, sSide, nCol + 1, nRow + 1, pDoc->GetItsDefCode(nDefCode));
						sStripC += str;
					}
				}
				else
				{
					nTotVerifyed++;
				}
			}
		}
	}

	// Strip D
	for (nRow = 0; nRow < nStripY; nRow++)
	{
		nR = nRow + nStripY * 3;
		for (nCol = 0; nCol < nNodeX; nCol++)
		{
			nPcrLineNum = pDoc->m_pPcr[nLayer][nIdx]->m_arPcrLineNum[nR][nCol];
			if (nPcrLineNum > -1)
			{
				if (pDoc->m_pPcr[nLayer][nIdx]->m_pMk[nPcrLineNum] != -2) // -2 (NoMarking)
				{
					nDefCode = pDoc->m_pPcr[nLayer][nIdx]->m_arDefType[nR][nCol]; //m_pPnlBuf[nSerial - 1][nR][nCol]; // DefCode 3D Array : [nSerial][nRow][nCol] - �������� ������.
					if (nDefCode > 0)
					{
						nTotStrip[3]++;
						str.Format(_T("%s,%04d,%s,D,%d,%d,B%d\n"), sItsCode, nSerial, sSide, nCol + 1, nRow + 1, pDoc->GetItsDefCode(nDefCode));
						sStripD += str;
					}
				}
				else
				{
					nTotVerifyed++;
				}
			}
		}
	}
	// Strip A
	str.Format(_T("%d,%s,%04d\n"), nTotDefPcs - nTotVerifyed, sItsCode, nSerial);
	sItsData = str;
	str.Format(_T("%d,%s,%04d,%s,A\n"), nTotStrip[0], sItsCode, nSerial, sSide);
	sItsData += str;
	sItsData += sStripA;
	str.Format(_T("%s,%04d,%s,A,EOS\n"), sItsCode, nSerial, sSide);
	sItsData += str;

	// Strip B
	str.Format(_T("%d,%s,%04d,%s,B\n"), nTotStrip[1], sItsCode, nSerial, sSide);
	sItsData += str;
	sItsData += sStripB;
	str.Format(_T("%s,%04d,%s,B,EOS\n"), sItsCode, nSerial, sSide);
	sItsData += str;

	// Strip C
	str.Format(_T("%d,%s,%04d,%s,C\n"), nTotStrip[2], sItsCode, nSerial, sSide);
	sItsData += str;
	sItsData += sStripC;
	str.Format(_T("%s,%04d,%s,C,EOS\n"), sItsCode, nSerial, sSide);
	sItsData += str;

	// Strip D
	str.Format(_T("%d,%s,%04d,%s,D\n"), nTotStrip[3], sItsCode, nSerial, sSide);
	sItsData += str;
	sItsData += sStripD;
	str.Format(_T("%s,%04d,%s,D,EOS\n"), sItsCode, nSerial, sSide);
	sItsData += str;
	str.Format(_T("%s,%04d,%s,EOP\n"), sItsCode, nSerial, sSide);
	sItsData += str;

	return sItsData;
}


void CDlgMenu05::OnBtnSave3()
{
	// TODO: Add your control notification handler code here
	if (pView->IsRun() || pView->IsBuffer())
	{
		pView->MsgBox(_T("�۾����Դϴ�."));
		return;
	}

	//	if(IDYES==pView->DoMyMsgBox(_T("Sap3���� ������ �����Ͻðڽ��ϱ�?"), MB_YESNO))
	if (IDYES == pView->MsgBox(_T("Sap3���� ������ �����Ͻðڽ��ϱ�?"), 0, MB_YESNO))
		MakeSapp3();
}

void CDlgMenu05::MakeSapp3() // With ReelmapDataAll.txt���Ͽ��� ������ ������.
{
	FILE *fp = NULL;
	//char FileName[MAX_PATH];
	TCHAR FileName[MAX_PATH];
	CString sPath;
	TCHAR szData[MAX_PATH];

	if (m_sProcessNum.IsEmpty())
	{
		// �����ڵ�
		if (0 < ::GetPrivateProfileString(_T("Info"), _T("Process Code"), NULL, szData, sizeof(szData), m_sRmapPath))
			m_sProcessNum = CString(szData);
		else
			m_sProcessNum = _T("");

	}
	sPath.Format(_T("%s%9s_%4s_%5s.txt"), pDoc->WorkingInfo.System.sPathSapp3, m_sLot, m_sProcessNum, pDoc->WorkingInfo.System.sMcName);
	//strcpy(FileName, sPath);
	_stprintf(FileName, _T("%s"), sPath);
	char* pRtn = NULL;
	fp = fopen(pRtn=TCHARToChar(FileName), "w+");

	if (fp != NULL)
	{
		fprintf(fp, "%s\n", pRtn = StringToChar(Sapp3Data()));
	}
	else
	{
		CString strMsg;
		strMsg.Format(_T("It is trouble to open file.\r\n%s"), sPath);
		pView->ClrDispMsg();
		AfxMessageBox(strMsg);
	}
	if(pRtn)
		delete pRtn;
	fclose(fp);
}

CString CDlgMenu05::Sapp3Data()
{
	CString strFileData = _T("");
	CString strPanelFacePath, strPath, strMsg;
	TCHAR szData[MAX_PATH];
	CFileFind findfile;
	double dEntireSpeed;

	if (pDoc->m_pReelMapUp)
	{
		strPath = pDoc->m_pReelMapUp->GetRmapPathOnOffline(RMAP_ALLUP);
		if (!findfile.FindFile(strPath))
		{
			strPath = pDoc->m_pReelMapUp->GetRmapPathOnOffline(RMAP_UP);
			if (!findfile.FindFile(strPath))
			{
				strPath = m_sRmapPath;
			}
		}
	}
	else
		strPath = m_sRmapPath;

	// �����ڵ带 AOI���� �޾ƿ�.
	if(findfile.FindFile(strPath)) //find file.
	{
		// �����ڵ�
		if (0 < ::GetPrivateProfileString(_T("Info"), _T("Process Code"), NULL, szData, sizeof(szData), strPath))
			m_sProcessNum = CString(szData);
		else
			m_sProcessNum = _T("");

		// �ӵ�
		if (0 < ::GetPrivateProfileString(_T("Info"), _T("Entire Speed"), NULL, szData, sizeof(szData), strPath))
			dEntireSpeed =_tstof(szData);
		else
			dEntireSpeed = 0.0;

		// ���̾�
		// m_sLayer;
 		//if (0 < ::GetPrivateProfileString(_T("WorkInfo"), _T("LAYER_NAME"), NULL, szData, sizeof(szData), strPath))
 		//	m_strLayer = CString(szData);
 		//else
 		//	m_strLayer = SearchLayerName();
	}
	else
	{
		m_sProcessNum = _T("");
		dEntireSpeed = 0.0;
	}

	
	CString strData;//strFileData, 
	int nSum, nStripPcs;
	double dRateBeforeVerify, dRateAfterVerify;
	nStripPcs = m_nEntirePieceNum/ MAX_STRIP_NUM;

	// ���� �̸�.
	strFileData.Format(_T("FileName : %9s_%4s_%5s.txt\r\n\r\n"), m_sLot, m_sProcessNum, pDoc->WorkingInfo.System.sMcName);

  
	if(!pDoc->WorkingInfo.LastJob.bUse2Layer)
	{
		// ���� ����/�ϼ�/���� Data.
		strFileData += _T("1Q\r\n");
		dRateBeforeVerify = 100.0 * (nStripPcs - m_nDefStrip[0]) / nStripPcs;
		dRateAfterVerify = 100.0 * (nStripPcs - m_nDefStrip[0]) / nStripPcs;
		strData.Format(_T("%d,%d,%.1f,%.1f\r\n"), nStripPcs, nStripPcs-m_nDefStrip[0], dRateBeforeVerify, dRateAfterVerify); // ���Լ���, �ϼ�����, Verify�� ����, Verify�� ����
		strFileData += strData;

		strFileData += _T("2Q\r\n");
		dRateBeforeVerify = 100.0 * (nStripPcs - m_nDefStrip[1]) / nStripPcs;
		dRateAfterVerify = 100.0 * (nStripPcs - m_nDefStrip[1]) / nStripPcs;
		strData.Format(_T("%d,%d,%.1f,%.1f\r\n"), nStripPcs, nStripPcs-m_nDefStrip[1], dRateBeforeVerify, dRateAfterVerify); // ���Լ���, �ϼ�����, Verify�� ����, Verify�� ����
		strFileData += strData;

		strFileData += _T("3Q\r\n");
		dRateBeforeVerify = 100.0 * (nStripPcs - m_nDefStrip[2]) / nStripPcs;
		dRateAfterVerify = 100.0 * (nStripPcs - m_nDefStrip[2]) / nStripPcs;
		strData.Format(_T("%d,%d,%.1f,%.1f\r\n"), nStripPcs, nStripPcs-m_nDefStrip[2], dRateBeforeVerify, dRateAfterVerify); // ���Լ���, �ϼ�����, Verify�� ����, Verify�� ����
		strFileData += strData;

		strFileData += _T("4Q\r\n");
		dRateBeforeVerify = 100.0 * (nStripPcs - m_nDefStrip[3]) / nStripPcs;
		dRateAfterVerify = 100.0 * (nStripPcs - m_nDefStrip[3]) / nStripPcs;
		strData.Format(_T("%d,%d,%.1f,%.1f\r\n"), nStripPcs, nStripPcs-m_nDefStrip[3], dRateBeforeVerify, dRateAfterVerify); // ���Լ���, �ϼ�����, Verify�� ����, Verify�� ����
		strFileData += strData;


		strFileData += _T("\r\n");
			

		// ���� �ҷ� Data.
		strFileData += _T("1X\r\n");

		if(m_nDefPerStrip[0][DEF_OPEN] > 0 && pDoc->m_nSapp3Code[SAPP3_OPEN] > 0)
		{
			strData.Format(_T("B%d,%d\r\n"), pDoc->m_nSapp3Code[SAPP3_OPEN], m_nDefPerStrip[0][DEF_OPEN]); // ����(B102)
			strFileData += strData;
		}

		nSum = m_nDefPerStrip[0][DEF_SHORT];// + m_nDefPerStrip[0][DEF_USHORT];
		if(nSum > 0 && pDoc->m_nSapp3Code[SAPP3_SHORT] > 0)
		{
			strData.Format(_T("B%d,%d\r\n"), pDoc->m_nSapp3Code[SAPP3_SHORT], nSum); // ��Ʈ(B129) // +u��Ʈ
			strFileData += strData;
		}

		nSum = m_nDefPerStrip[0][DEF_USHORT];
		if(nSum > 0 && pDoc->m_nSapp3Code[SAPP3_USHORT] > 0)
		{
			strData.Format(_T("B%d,%d\r\n"), pDoc->m_nSapp3Code[SAPP3_USHORT], nSum); // u��Ʈ(B314)
			strFileData += strData;
		}
		
		if(m_nDefPerStrip[0][DEF_NICK] > 0 && pDoc->m_nSapp3Code[SAPP3_NICK] > 0)
		{
			strData.Format(_T("B%d,%d\r\n"), pDoc->m_nSapp3Code[SAPP3_NICK], m_nDefPerStrip[0][DEF_NICK]); // ���(B137)
			strFileData += strData;
		}

		nSum = m_nDefPerStrip[0][DEF_SPACE] + m_nDefPerStrip[0][DEF_EXTRA] + m_nDefPerStrip[0][DEF_PROTRUSION];
		if(nSum > 0 && pDoc->m_nSapp3Code[SAPP3_SPACE_EXTRA_PROTRUSION] > 0)
		{
			strData.Format(_T("B%d,%d\r\n"), pDoc->m_nSapp3Code[SAPP3_SPACE_EXTRA_PROTRUSION], nSum); // ������+�ܵ�+����(B160)
			strFileData += strData;
		}

 		//nSum = m_nDefPerStrip[0][DEF_PINHOLE] + m_nDefPerStrip[0][DEF_PAD];
 		//if(nSum > 0 && pDoc->m_nSapp3Code[SAPP3_PINHOLE_PAD] > 0)
 		//{
 		//	strData.Format(_T("B%d,%d\r\n"), pDoc->m_nSapp3Code[SAPP3_PINHOLE_PAD], nSum); // ��Ȧ+�е�(B134)
 		//	strFileData += strData;
 		//}

		nSum = m_nDefPerStrip[0][DEF_PINHOLE];
		if(nSum > 0 && pDoc->m_nSapp3Code[SAPP3_PINHOLE] > 0)
		{
			strData.Format(_T("B%d,%d\r\n"), pDoc->m_nSapp3Code[SAPP3_PINHOLE], nSum); // ��Ȧ(B134)
			strFileData += strData;
		}

		nSum = m_nDefPerStrip[0][DEF_PAD];
		if(nSum > 0 && pDoc->m_nSapp3Code[SAPP3_PAD] > 0)
		{
			strData.Format(_T("B%d,%d\r\n"), pDoc->m_nSapp3Code[SAPP3_PAD], nSum); // �е�(B316)
			strFileData += strData;
		}

		if(nSum > 0 && pDoc->m_nSapp3Code[SAPP3_HOPEN] > 0)
		{
			strData.Format(_T("B%d,%d\r\n"),pDoc->m_nSapp3Code[SAPP3_HOPEN], m_nDefPerStrip[0][DEF_HOLE_OPEN]);
			strFileData += strData;
		}
		nSum = m_nDefPerStrip[0][DEF_HOLE_MISS] + m_nDefPerStrip[0][DEF_HOLE_POSITION] + m_nDefPerStrip[0][DEF_HOLE_DEFECT];
		if(nSum > 0 && pDoc->m_nSapp3Code[SAPP3_HMISS_HPOS_HBAD] > 0)
		{
			strData.Format(_T("B%d,%d\r\n"),pDoc->m_nSapp3Code[SAPP3_HMISS_HPOS_HBAD], nSum);
			strFileData += strData;
		}

		nSum = m_nDefPerStrip[0][DEF_VH_OPEN] + m_nDefPerStrip[0][DEF_VH_MISS] + m_nDefPerStrip[0][DEF_VH_POSITION] + m_nDefPerStrip[0][DEF_VH_DEF];
		if (nSum > 0 && pDoc->m_nSapp3Code[SAPP3_VHOPEN_NOVH_VHALIGN_VHDEF] > 0)
		{
			strData.Format(_T("B%d,%d\r\n"), pDoc->m_nSapp3Code[SAPP3_VHOPEN_NOVH_VHALIGN_VHDEF], nSum);
			strFileData += strData;
		}


		strFileData += _T("2X\r\n");

		if(m_nDefPerStrip[1][DEF_OPEN] > 0 && pDoc->m_nSapp3Code[SAPP3_OPEN] > 0)
		{
			strData.Format(_T("B%d,%d\r\n"), pDoc->m_nSapp3Code[SAPP3_OPEN], m_nDefPerStrip[1][DEF_OPEN]); // ����(B102)
			strFileData += strData;
		}

		nSum = m_nDefPerStrip[1][DEF_SHORT];// + m_nDefPerStrip[1][DEF_USHORT];
		if(nSum > 0 && pDoc->m_nSapp3Code[SAPP3_SHORT] > 0)
		{
			strData.Format(_T("B%d,%d\r\n"), pDoc->m_nSapp3Code[SAPP3_SHORT], nSum); // ��Ʈ(B129) // +u��Ʈ
			strFileData += strData;
		}

		nSum = m_nDefPerStrip[1][DEF_USHORT];
		if(nSum > 0 && pDoc->m_nSapp3Code[SAPP3_USHORT] > 0)
		{
			strData.Format(_T("B%d,%d\r\n"), pDoc->m_nSapp3Code[SAPP3_USHORT], nSum); // u��Ʈ(B314)
			strFileData += strData;
		}
		
		if(m_nDefPerStrip[1][DEF_NICK] > 0 && pDoc->m_nSapp3Code[SAPP3_NICK] > 0)
		{
			strData.Format(_T("B%d,%d\r\n"), pDoc->m_nSapp3Code[SAPP3_NICK], m_nDefPerStrip[1][DEF_NICK]); // ���(B137)
			strFileData += strData;
		}

		nSum = m_nDefPerStrip[1][DEF_SPACE] + m_nDefPerStrip[1][DEF_EXTRA] + m_nDefPerStrip[1][DEF_PROTRUSION];
		if(nSum > 0 && pDoc->m_nSapp3Code[SAPP3_SPACE_EXTRA_PROTRUSION] > 0)
		{
			strData.Format(_T("B%d,%d\r\n"), pDoc->m_nSapp3Code[SAPP3_SPACE_EXTRA_PROTRUSION], nSum); // ������+�ܵ�+����(B160)
			strFileData += strData;
		}

	// 			nSum = m_nDefPerStrip[1][DEF_PINHOLE] + m_nDefPerStrip[1][DEF_PAD];
	// 			if(nSum > 0 && pDoc->m_nSapp3Code[SAPP3_PINHOLE_PAD] > 0)
	// 			{
	// 				strData.Format(_T("B%d,%d\r\n"), pDoc->m_nSapp3Code[SAPP3_PINHOLE_PAD], nSum); // ��Ȧ+�е�(B134)
	// 				strFileData += strData;
	// 			}

		nSum = m_nDefPerStrip[1][DEF_PINHOLE];
		if(nSum > 0 && pDoc->m_nSapp3Code[SAPP3_PINHOLE] > 0)
		{
			strData.Format(_T("B%d,%d\r\n"), pDoc->m_nSapp3Code[SAPP3_PINHOLE], nSum); // ��Ȧ(B134)
			strFileData += strData;
		}

		nSum = m_nDefPerStrip[1][DEF_PAD];
		if(nSum > 0 && pDoc->m_nSapp3Code[SAPP3_PAD] > 0)
		{
			strData.Format(_T("B%d,%d\r\n"), pDoc->m_nSapp3Code[SAPP3_PAD], nSum); // �е�(B316)
			strFileData += strData;
		}

		if(nSum > 0 && pDoc->m_nSapp3Code[SAPP3_HOPEN] > 0)
		{
			strData.Format(_T("B%d,%d\r\n"),pDoc->m_nSapp3Code[SAPP3_HOPEN], m_nDefPerStrip[1][DEF_HOLE_OPEN]);
			strFileData += strData;
		}

		nSum = m_nDefPerStrip[1][DEF_HOLE_MISS] + m_nDefPerStrip[1][DEF_HOLE_POSITION] + m_nDefPerStrip[1][DEF_HOLE_DEFECT];
		if(nSum > 0 && pDoc->m_nSapp3Code[SAPP3_HMISS_HPOS_HBAD] > 0)
		{
			strData.Format(_T("B%d,%d\r\n"),pDoc->m_nSapp3Code[SAPP3_HMISS_HPOS_HBAD], nSum);
			strFileData += strData;
		}

		nSum = m_nDefPerStrip[1][DEF_VH_OPEN] + m_nDefPerStrip[1][DEF_VH_MISS] + m_nDefPerStrip[1][DEF_VH_POSITION] + m_nDefPerStrip[1][DEF_VH_DEF];
		if (nSum > 0 && pDoc->m_nSapp3Code[SAPP3_VHOPEN_NOVH_VHALIGN_VHDEF] > 0)
		{
			strData.Format(_T("B%d,%d\r\n"), pDoc->m_nSapp3Code[SAPP3_VHOPEN_NOVH_VHALIGN_VHDEF], nSum);
			strFileData += strData;
		}


		strFileData += _T("3X\r\n");

		if(m_nDefPerStrip[2][DEF_OPEN] > 0 && pDoc->m_nSapp3Code[SAPP3_OPEN] > 0)
		{
			strData.Format(_T("B%d,%d\r\n"), pDoc->m_nSapp3Code[SAPP3_OPEN], m_nDefPerStrip[2][DEF_OPEN]); // ����(B102)
			strFileData += strData;
		}

		nSum = m_nDefPerStrip[2][DEF_SHORT];// + m_nDefPerStrip[2][DEF_USHORT];
		if(nSum > 0 && pDoc->m_nSapp3Code[SAPP3_SHORT] > 0)
		{
			strData.Format(_T("B%d,%d\r\n"), pDoc->m_nSapp3Code[SAPP3_SHORT], nSum); // ��Ʈ(B129) // +u��Ʈ
			strFileData += strData;
		}

		nSum = m_nDefPerStrip[2][DEF_USHORT];
		if(nSum > 0 && pDoc->m_nSapp3Code[SAPP3_USHORT] > 0)
		{
			strData.Format(_T("B%d,%d\r\n"), pDoc->m_nSapp3Code[SAPP3_USHORT], nSum); // u��Ʈ(B314)
			strFileData += strData;
		}
		
		if(m_nDefPerStrip[2][DEF_NICK] > 0 && pDoc->m_nSapp3Code[SAPP3_NICK] > 0)
		{
			strData.Format(_T("B%d,%d\r\n"), pDoc->m_nSapp3Code[SAPP3_NICK], m_nDefPerStrip[2][DEF_NICK]); // ���(B137)
			strFileData += strData;
		}

		nSum = m_nDefPerStrip[2][DEF_SPACE] + m_nDefPerStrip[2][DEF_EXTRA] + m_nDefPerStrip[2][DEF_PROTRUSION];
		if(nSum > 0 && pDoc->m_nSapp3Code[SAPP3_SPACE_EXTRA_PROTRUSION] > 0)
		{
			strData.Format(_T("B%d,%d\r\n"), pDoc->m_nSapp3Code[SAPP3_SPACE_EXTRA_PROTRUSION], nSum); // ������+�ܵ�+����(B160)
			strFileData += strData;
		}

	// 			nSum = m_nDefPerStrip[2][DEF_PINHOLE] + m_nDefPerStrip[2][DEF_PAD];
	// 			if(nSum > 0 && pDoc->m_nSapp3Code[SAPP3_PINHOLE_PAD] > 0)
	// 			{
	// 				strData.Format(_T("B%d,%d\r\n"), pDoc->m_nSapp3Code[SAPP3_PINHOLE_PAD], nSum); // ��Ȧ+�е�(B134)
	// 				strFileData += strData;
	// 			}

		nSum = m_nDefPerStrip[2][DEF_PINHOLE];
		if(nSum > 0 && pDoc->m_nSapp3Code[SAPP3_PINHOLE] > 0)
		{
			strData.Format(_T("B%d,%d\r\n"), pDoc->m_nSapp3Code[SAPP3_PINHOLE], nSum); // ��Ȧ(B134)
			strFileData += strData;
		}

		nSum = m_nDefPerStrip[2][DEF_PAD];
		if(nSum > 0 && pDoc->m_nSapp3Code[SAPP3_PAD] > 0)
		{
			strData.Format(_T("B%d,%d\r\n"), pDoc->m_nSapp3Code[SAPP3_PAD], nSum); // �е�(B316)
			strFileData += strData;
		}

		if(nSum > 0 && pDoc->m_nSapp3Code[SAPP3_HOPEN] > 0)
		{
			strData.Format(_T("B%d,%d\r\n"),pDoc->m_nSapp3Code[SAPP3_HOPEN], m_nDefPerStrip[2][DEF_HOLE_OPEN]);
			strFileData += strData;
		}

		nSum = m_nDefPerStrip[2][DEF_HOLE_MISS] + m_nDefPerStrip[2][DEF_HOLE_POSITION] + m_nDefPerStrip[2][DEF_HOLE_DEFECT];
		if(nSum > 0 && pDoc->m_nSapp3Code[SAPP3_HMISS_HPOS_HBAD] > 0)
		{
			strData.Format(_T("B%d,%d\r\n"),pDoc->m_nSapp3Code[SAPP3_HMISS_HPOS_HBAD], nSum);
			strFileData += strData;
		}

		nSum = m_nDefPerStrip[2][DEF_VH_OPEN] + m_nDefPerStrip[2][DEF_VH_MISS] + m_nDefPerStrip[2][DEF_VH_POSITION] + m_nDefPerStrip[2][DEF_VH_DEF];
		if (nSum > 0 && pDoc->m_nSapp3Code[SAPP3_VHOPEN_NOVH_VHALIGN_VHDEF] > 0)
		{
			strData.Format(_T("B%d,%d\r\n"), pDoc->m_nSapp3Code[SAPP3_VHOPEN_NOVH_VHALIGN_VHDEF], nSum);
			strFileData += strData;
		}


		strFileData += _T("4X\r\n");

		if(m_nDefPerStrip[3][DEF_OPEN] > 0 && pDoc->m_nSapp3Code[SAPP3_OPEN] > 0)
		{
			strData.Format(_T("B%d,%d\r\n"), pDoc->m_nSapp3Code[SAPP3_OPEN], m_nDefPerStrip[3][DEF_OPEN]); // ����(B102)
			strFileData += strData;
		}

		nSum = m_nDefPerStrip[3][DEF_SHORT];// + m_nDefPerStrip[3][DEF_USHORT];
		if(nSum > 0 && pDoc->m_nSapp3Code[SAPP3_SHORT] > 0)
		{
			strData.Format(_T("B%d,%d\r\n"),pDoc->m_nSapp3Code[SAPP3_SHORT], nSum); // ��Ʈ(B129) // +u��Ʈ
			strFileData += strData;
		}

		nSum = m_nDefPerStrip[3][DEF_USHORT];
		if(nSum > 0 && pDoc->m_nSapp3Code[SAPP3_USHORT] > 0)
		{
			strData.Format(_T("B%d,%d\r\n"), pDoc->m_nSapp3Code[SAPP3_USHORT], nSum); // u��Ʈ(B314)
			strFileData += strData;
		}
		
		if(m_nDefPerStrip[3][DEF_NICK] > 0 && pDoc->m_nSapp3Code[SAPP3_NICK] > 0)
		{
			strData.Format(_T("B%d,%d\r\n"),pDoc->m_nSapp3Code[SAPP3_NICK], m_nDefPerStrip[3][DEF_NICK]); // ���(B137)
			strFileData += strData;
		}

		nSum = m_nDefPerStrip[3][DEF_SPACE] + m_nDefPerStrip[3][DEF_EXTRA] + m_nDefPerStrip[3][DEF_PROTRUSION];
		if(nSum > 0 && pDoc->m_nSapp3Code[SAPP3_SPACE_EXTRA_PROTRUSION] > 0)
		{
			strData.Format(_T("B%d,%d\r\n"),pDoc->m_nSapp3Code[SAPP3_SPACE_EXTRA_PROTRUSION], nSum); // ������+�ܵ�+����(B160)
			strFileData += strData;
		}

	// 			nSum = m_nDefPerStrip[3][DEF_PINHOLE] + m_nDefPerStrip[3][DEF_PAD];
	// 			if(nSum > 0 && pDoc->m_nSapp3Code[SAPP3_PINHOLE_PAD] > 0)
	// 			{
	// 				strData.Format(_T("B%d,%d\r\n"),pDoc->m_nSapp3Code[SAPP3_PINHOLE_PAD], nSum); // ��Ȧ+�е�(B134)
	// 				strFileData += strData;
	// 			}

		nSum = m_nDefPerStrip[3][DEF_PINHOLE];
		if(nSum > 0 && pDoc->m_nSapp3Code[SAPP3_PINHOLE] > 0)
		{
			strData.Format(_T("B%d,%d\r\n"),pDoc->m_nSapp3Code[SAPP3_PINHOLE], nSum); // ��Ȧ(B134)
			strFileData += strData;
		}

		nSum = m_nDefPerStrip[3][DEF_PAD];
		if(nSum > 0 && pDoc->m_nSapp3Code[SAPP3_PAD] > 0)
		{
			strData.Format(_T("B%d,%d\r\n"),pDoc->m_nSapp3Code[SAPP3_PAD], nSum); // �е�(B316)
			strFileData += strData;
		}

		if(nSum > 0 && pDoc->m_nSapp3Code[SAPP3_HOPEN] > 0)
		{
			strData.Format(_T("B%d,%d\r\n"),pDoc->m_nSapp3Code[SAPP3_HOPEN], m_nDefPerStrip[3][DEF_HOLE_OPEN]);
			strFileData += strData;
		}

		nSum = m_nDefPerStrip[3][DEF_HOLE_MISS] + m_nDefPerStrip[3][DEF_HOLE_POSITION] + m_nDefPerStrip[3][DEF_HOLE_DEFECT];
		if(nSum > 0 && pDoc->m_nSapp3Code[SAPP3_HMISS_HPOS_HBAD] > 0)
		{
			strData.Format(_T("B%d,%d\r\n"),pDoc->m_nSapp3Code[SAPP3_HMISS_HPOS_HBAD], nSum);
			strFileData += strData;
		}

		nSum = m_nDefPerStrip[3][DEF_VH_OPEN] + m_nDefPerStrip[3][DEF_VH_MISS] + m_nDefPerStrip[3][DEF_VH_POSITION] + m_nDefPerStrip[3][DEF_VH_DEF];
		if (nSum > 0 && pDoc->m_nSapp3Code[SAPP3_VHOPEN_NOVH_VHALIGN_VHDEF] > 0)
		{
			strData.Format(_T("B%d,%d\r\n"), pDoc->m_nSapp3Code[SAPP3_VHOPEN_NOVH_VHALIGN_VHDEF], nSum);
			strFileData += strData;
		}
	}
	else
	{
		// ���� ����/�ϼ�/���� Data.
		strFileData += _T("1Q\r\n");
		dRateBeforeVerify = 100.0 * (nStripPcs - m_nDefStrip[3]) / nStripPcs;
		dRateAfterVerify = 100.0 * (nStripPcs - m_nDefStrip[3]) / nStripPcs;
		strData.Format(_T("%d,%d,%.1f,%.1f\r\n"), nStripPcs, nStripPcs-m_nDefStrip[3], dRateBeforeVerify, dRateAfterVerify); // ���Լ���, �ϼ�����, Verify�� ����, Verify�� ����
		strFileData += strData;

		strFileData += _T("2Q\r\n");
		dRateBeforeVerify = 100.0 * (nStripPcs - m_nDefStrip[2]) / nStripPcs;
		dRateAfterVerify = 100.0 * (nStripPcs - m_nDefStrip[2]) / nStripPcs;
		strData.Format(_T("%d,%d,%.1f,%.1f\r\n"), nStripPcs, nStripPcs-m_nDefStrip[2], dRateBeforeVerify, dRateAfterVerify); // ���Լ���, �ϼ�����, Verify�� ����, Verify�� ����
		strFileData += strData;

		strFileData += _T("3Q\r\n");
		dRateBeforeVerify = 100.0 * (nStripPcs - m_nDefStrip[1]) / nStripPcs;
		dRateAfterVerify = 100.0 * (nStripPcs - m_nDefStrip[1]) / nStripPcs;
		strData.Format(_T("%d,%d,%.1f,%.1f\r\n"), nStripPcs, nStripPcs-m_nDefStrip[1], dRateBeforeVerify, dRateAfterVerify); // ���Լ���, �ϼ�����, Verify�� ����, Verify�� ����
		strFileData += strData;

		strFileData += _T("4Q\r\n");
		dRateBeforeVerify = 100.0 * (nStripPcs - m_nDefStrip[0]) / nStripPcs;
		dRateAfterVerify = 100.0 * (nStripPcs - m_nDefStrip[0]) / nStripPcs;
		strData.Format(_T("%d,%d,%.1f,%.1f\r\n"), nStripPcs, nStripPcs-m_nDefStrip[0], dRateBeforeVerify, dRateAfterVerify); // ���Լ���, �ϼ�����, Verify�� ����, Verify�� ����
		strFileData += strData;


		strFileData += _T("\r\n");
			

		// ���� �ҷ� Data.
		strFileData += _T("1X\r\n");

		if(m_nDefPerStrip[3][DEF_OPEN] > 0 && pDoc->m_nSapp3Code[SAPP3_OPEN] > 0)
		{
			strData.Format(_T("B%d,%d\r\n"), pDoc->m_nSapp3Code[SAPP3_OPEN], m_nDefPerStrip[3][DEF_OPEN]); // ����(B102)
			strFileData += strData;
		}

		nSum = m_nDefPerStrip[3][DEF_SHORT];// + m_nDefPerStrip[0][DEF_USHORT];
		if(nSum > 0 && pDoc->m_nSapp3Code[SAPP3_SHORT] > 0)
		{
			strData.Format(_T("B%d,%d\r\n"), pDoc->m_nSapp3Code[SAPP3_SHORT], nSum); // ��Ʈ(B129) // +u��Ʈ
			strFileData += strData;
		}

		nSum = m_nDefPerStrip[3][DEF_USHORT];
		if(nSum > 0 && pDoc->m_nSapp3Code[SAPP3_USHORT] > 0)
		{
			strData.Format(_T("B%d,%d\r\n"), pDoc->m_nSapp3Code[SAPP3_USHORT], nSum); // u��Ʈ(B314)
			strFileData += strData;
		}
		
		if(m_nDefPerStrip[3][DEF_NICK] > 0 && pDoc->m_nSapp3Code[SAPP3_NICK] > 0)
		{
			strData.Format(_T("B%d,%d\r\n"), pDoc->m_nSapp3Code[SAPP3_NICK], m_nDefPerStrip[3][DEF_NICK]); // ���(B137)
			strFileData += strData;
		}

		nSum = m_nDefPerStrip[3][DEF_SPACE] + m_nDefPerStrip[3][DEF_EXTRA] + m_nDefPerStrip[3][DEF_PROTRUSION];
		if(nSum > 0 && pDoc->m_nSapp3Code[SAPP3_SPACE_EXTRA_PROTRUSION] > 0)
		{
			strData.Format(_T("B%d,%d\r\n"), pDoc->m_nSapp3Code[SAPP3_SPACE_EXTRA_PROTRUSION], nSum); // ������+�ܵ�+����(B160)
			strFileData += strData;
		}

		nSum = m_nDefPerStrip[3][DEF_VH_OPEN] + m_nDefPerStrip[3][DEF_VH_MISS] + m_nDefPerStrip[3][DEF_VH_POSITION] + m_nDefPerStrip[3][DEF_VH_DEF];
		if (nSum > 0 && pDoc->m_nSapp3Code[SAPP3_VHOPEN_NOVH_VHALIGN_VHDEF] > 0)
		{
			strData.Format(_T("B%d,%d\r\n"), pDoc->m_nSapp3Code[SAPP3_VHOPEN_NOVH_VHALIGN_VHDEF], nSum);
			strFileData += strData;
		}

		nSum = m_nDefPerStrip[3][DEF_PINHOLE];
		if(nSum > 0 && pDoc->m_nSapp3Code[SAPP3_PINHOLE] > 0)
		{
			strData.Format(_T("B%d,%d\r\n"), pDoc->m_nSapp3Code[SAPP3_PINHOLE], nSum); // ��Ȧ(B134)
			strFileData += strData;
		}

		nSum = m_nDefPerStrip[3][DEF_PAD];
		if(nSum > 0 && pDoc->m_nSapp3Code[SAPP3_PAD] > 0)
		{
			strData.Format(_T("B%d,%d\r\n"), pDoc->m_nSapp3Code[SAPP3_PAD], nSum); // �е�(B316)
			strFileData += strData;
		}

		if(nSum > 0 && pDoc->m_nSapp3Code[SAPP3_HOPEN] > 0)
		{
			strData.Format(_T("B%d,%d\r\n"),pDoc->m_nSapp3Code[SAPP3_HOPEN], m_nDefPerStrip[3][DEF_HOLE_OPEN]);
			strFileData += strData;
		}
		nSum = m_nDefPerStrip[3][DEF_HOLE_MISS] + m_nDefPerStrip[3][DEF_HOLE_POSITION] + m_nDefPerStrip[3][DEF_HOLE_DEFECT];
		if(nSum > 0 && pDoc->m_nSapp3Code[SAPP3_HMISS_HPOS_HBAD] > 0)
		{
			strData.Format(_T("B%d,%d\r\n"),pDoc->m_nSapp3Code[SAPP3_HMISS_HPOS_HBAD], nSum);
			strFileData += strData;
		}

		nSum = m_nDefPerStrip[3][DEF_VH_OPEN] + m_nDefPerStrip[3][DEF_VH_MISS] + m_nDefPerStrip[3][DEF_VH_POSITION] + m_nDefPerStrip[3][DEF_VH_DEF];
		if (nSum > 0 && pDoc->m_nSapp3Code[SAPP3_VHOPEN_NOVH_VHALIGN_VHDEF] > 0)
		{
			strData.Format(_T("B%d,%d\r\n"), pDoc->m_nSapp3Code[SAPP3_VHOPEN_NOVH_VHALIGN_VHDEF], nSum);
			strFileData += strData;
		}


		strFileData += _T("2X\r\n");

		if(m_nDefPerStrip[2][DEF_OPEN] > 0 && pDoc->m_nSapp3Code[SAPP3_OPEN] > 0)
		{
			strData.Format(_T("B%d,%d\r\n"), pDoc->m_nSapp3Code[SAPP3_OPEN], m_nDefPerStrip[2][DEF_OPEN]); // ����(B102)
			strFileData += strData;
		}

		nSum = m_nDefPerStrip[2][DEF_SHORT];// + m_nDefPerStrip[2][DEF_USHORT];
		if(nSum > 0 && pDoc->m_nSapp3Code[SAPP3_SHORT] > 0)
		{
			strData.Format(_T("B%d,%d\r\n"), pDoc->m_nSapp3Code[SAPP3_SHORT], nSum); // ��Ʈ(B129) // +u��Ʈ
			strFileData += strData;
		}

		nSum = m_nDefPerStrip[2][DEF_USHORT];
		if(nSum > 0 && pDoc->m_nSapp3Code[SAPP3_USHORT] > 0)
		{
			strData.Format(_T("B%d,%d\r\n"), pDoc->m_nSapp3Code[SAPP3_USHORT], nSum); // u��Ʈ(B314)
			strFileData += strData;
		}
		
		if(m_nDefPerStrip[2][DEF_NICK] > 0 && pDoc->m_nSapp3Code[SAPP3_NICK] > 0)
		{
			strData.Format(_T("B%d,%d\r\n"), pDoc->m_nSapp3Code[SAPP3_NICK], m_nDefPerStrip[2][DEF_NICK]); // ���(B137)
			strFileData += strData;
		}

		nSum = m_nDefPerStrip[2][DEF_SPACE] + m_nDefPerStrip[2][DEF_EXTRA] + m_nDefPerStrip[2][DEF_PROTRUSION];
		if(nSum > 0 && pDoc->m_nSapp3Code[SAPP3_SPACE_EXTRA_PROTRUSION] > 0)
		{
			strData.Format(_T("B%d,%d\r\n"), pDoc->m_nSapp3Code[SAPP3_SPACE_EXTRA_PROTRUSION], nSum); // ������+�ܵ�+����(B160)
			strFileData += strData;
		}

	// 			nSum = m_nDefPerStrip[2][DEF_PINHOLE] + m_nDefPerStrip[2][DEF_PAD];
	// 			if(nSum > 0 && pDoc->m_nSapp3Code[SAPP3_PINHOLE_PAD] > 0)
	// 			{
	// 				strData.Format(_T("B%d,%d\r\n"), pDoc->m_nSapp3Code[SAPP3_PINHOLE_PAD], nSum); // ��Ȧ+�е�(B134)
	// 				strFileData += strData;
	// 			}

		nSum = m_nDefPerStrip[2][DEF_PINHOLE];
		if(nSum > 0 && pDoc->m_nSapp3Code[SAPP3_PINHOLE] > 0)
		{
			strData.Format(_T("B%d,%d\r\n"), pDoc->m_nSapp3Code[SAPP3_PINHOLE], nSum); // ��Ȧ(B134)
			strFileData += strData;
		}

		nSum = m_nDefPerStrip[2][DEF_PAD];
		if(nSum > 0 && pDoc->m_nSapp3Code[SAPP3_PAD] > 0)
		{
			strData.Format(_T("B%d,%d\r\n"), pDoc->m_nSapp3Code[SAPP3_PAD], nSum); // �е�(B316)
			strFileData += strData;
		}

		if(nSum > 0 && pDoc->m_nSapp3Code[SAPP3_HOPEN] > 0)
		{
			strData.Format(_T("B%d,%d\r\n"),pDoc->m_nSapp3Code[SAPP3_HOPEN], m_nDefPerStrip[2][DEF_HOLE_OPEN]);
			strFileData += strData;
		}

		nSum = m_nDefPerStrip[2][DEF_HOLE_MISS] + m_nDefPerStrip[2][DEF_HOLE_POSITION] + m_nDefPerStrip[2][DEF_HOLE_DEFECT];
		if(nSum > 0 && pDoc->m_nSapp3Code[SAPP3_HMISS_HPOS_HBAD] > 0)
		{
			strData.Format(_T("B%d,%d\r\n"),pDoc->m_nSapp3Code[SAPP3_HMISS_HPOS_HBAD], nSum);
			strFileData += strData;
		}

		nSum = m_nDefPerStrip[2][DEF_VH_OPEN] + m_nDefPerStrip[2][DEF_VH_MISS] + m_nDefPerStrip[2][DEF_VH_POSITION] + m_nDefPerStrip[2][DEF_VH_DEF];
		if (nSum > 0 && pDoc->m_nSapp3Code[SAPP3_VHOPEN_NOVH_VHALIGN_VHDEF] > 0)
		{
			strData.Format(_T("B%d,%d\r\n"), pDoc->m_nSapp3Code[SAPP3_VHOPEN_NOVH_VHALIGN_VHDEF], nSum);
			strFileData += strData;
		}


		strFileData += _T("3X\r\n");

		if(m_nDefPerStrip[1][DEF_OPEN] > 0 && pDoc->m_nSapp3Code[SAPP3_OPEN] > 0)
		{
			strData.Format(_T("B%d,%d\r\n"), pDoc->m_nSapp3Code[SAPP3_OPEN], m_nDefPerStrip[1][DEF_OPEN]); // ����(B102)
			strFileData += strData;
		}

		nSum = m_nDefPerStrip[1][DEF_SHORT];// + m_nDefPerStrip[1][DEF_USHORT];
		if(nSum > 0 && pDoc->m_nSapp3Code[SAPP3_SHORT] > 0)
		{
			strData.Format(_T("B%d,%d\r\n"), pDoc->m_nSapp3Code[SAPP3_SHORT], nSum); // ��Ʈ(B129) // +u��Ʈ
			strFileData += strData;
		}

		nSum = m_nDefPerStrip[1][DEF_USHORT];
		if(nSum > 0 && pDoc->m_nSapp3Code[SAPP3_USHORT] > 0)
		{
			strData.Format(_T("B%d,%d\r\n"), pDoc->m_nSapp3Code[SAPP3_USHORT], nSum); // u��Ʈ(B314)
			strFileData += strData;
		}
		
		if(m_nDefPerStrip[1][DEF_NICK] > 0 && pDoc->m_nSapp3Code[SAPP3_NICK] > 0)
		{
			strData.Format(_T("B%d,%d\r\n"), pDoc->m_nSapp3Code[SAPP3_NICK], m_nDefPerStrip[1][DEF_NICK]); // ���(B137)
			strFileData += strData;
		}

		nSum = m_nDefPerStrip[1][DEF_SPACE] + m_nDefPerStrip[1][DEF_EXTRA] + m_nDefPerStrip[1][DEF_PROTRUSION];
		if(nSum > 0 && pDoc->m_nSapp3Code[SAPP3_SPACE_EXTRA_PROTRUSION] > 0)
		{
			strData.Format(_T("B%d,%d\r\n"), pDoc->m_nSapp3Code[SAPP3_SPACE_EXTRA_PROTRUSION], nSum); // ������+�ܵ�+����(B160)
			strFileData += strData;
		}

	// 			nSum = m_nDefPerStrip[1][DEF_PINHOLE] + m_nDefPerStrip[1][DEF_PAD];
	// 			if(nSum > 0 && pDoc->m_nSapp3Code[SAPP3_PINHOLE_PAD] > 0)
	// 			{
	// 				strData.Format(_T("B%d,%d\r\n"), pDoc->m_nSapp3Code[SAPP3_PINHOLE_PAD], nSum); // ��Ȧ+�е�(B134)
	// 				strFileData += strData;
	// 			}

		nSum = m_nDefPerStrip[1][DEF_PINHOLE];
		if(nSum > 0 && pDoc->m_nSapp3Code[SAPP3_PINHOLE] > 0)
		{
			strData.Format(_T("B%d,%d\r\n"), pDoc->m_nSapp3Code[SAPP3_PINHOLE], nSum); // ��Ȧ(B134)
			strFileData += strData;
		}

		nSum = m_nDefPerStrip[1][DEF_PAD];
		if(nSum > 0 && pDoc->m_nSapp3Code[SAPP3_PAD] > 0)
		{
			strData.Format(_T("B%d,%d\r\n"), pDoc->m_nSapp3Code[SAPP3_PAD], nSum); // �е�(B316)
			strFileData += strData;
		}

		if(nSum > 0 && pDoc->m_nSapp3Code[SAPP3_HOPEN] > 0)
		{
			strData.Format(_T("B%d,%d\r\n"),pDoc->m_nSapp3Code[SAPP3_HOPEN], m_nDefPerStrip[1][DEF_HOLE_OPEN]);
			strFileData += strData;
		}

		nSum = m_nDefPerStrip[1][DEF_HOLE_MISS] + m_nDefPerStrip[1][DEF_HOLE_POSITION] + m_nDefPerStrip[1][DEF_HOLE_DEFECT];
		if(nSum > 0 && pDoc->m_nSapp3Code[SAPP3_HMISS_HPOS_HBAD] > 0)
		{
			strData.Format(_T("B%d,%d\r\n"),pDoc->m_nSapp3Code[SAPP3_HMISS_HPOS_HBAD], nSum);
			strFileData += strData;
		}

		nSum = m_nDefPerStrip[1][DEF_VH_OPEN] + m_nDefPerStrip[1][DEF_VH_MISS] + m_nDefPerStrip[1][DEF_VH_POSITION] + m_nDefPerStrip[1][DEF_VH_DEF];
		if (nSum > 0 && pDoc->m_nSapp3Code[SAPP3_VHOPEN_NOVH_VHALIGN_VHDEF] > 0)
		{
			strData.Format(_T("B%d,%d\r\n"), pDoc->m_nSapp3Code[SAPP3_VHOPEN_NOVH_VHALIGN_VHDEF], nSum);
			strFileData += strData;
		}


		strFileData += _T("4X\r\n");

		if(m_nDefPerStrip[0][DEF_OPEN] > 0 && pDoc->m_nSapp3Code[SAPP3_OPEN] > 0)
		{
			strData.Format(_T("B%d,%d\r\n"), pDoc->m_nSapp3Code[SAPP3_OPEN], m_nDefPerStrip[0][DEF_OPEN]); // ����(B102)
			strFileData += strData;
		}

		nSum = m_nDefPerStrip[0][DEF_SHORT];// + m_nDefPerStrip[3][DEF_USHORT];
		if(nSum > 0 && pDoc->m_nSapp3Code[SAPP3_SHORT] > 0)
		{
			strData.Format(_T("B%d,%d\r\n"),pDoc->m_nSapp3Code[SAPP3_SHORT], nSum); // ��Ʈ(B129) // +u��Ʈ
			strFileData += strData;
		}

		nSum = m_nDefPerStrip[0][DEF_USHORT];
		if(nSum > 0 && pDoc->m_nSapp3Code[SAPP3_USHORT] > 0)
		{
			strData.Format(_T("B%d,%d\r\n"), pDoc->m_nSapp3Code[SAPP3_USHORT], nSum); // u��Ʈ(B314)
			strFileData += strData;
		}
		
		if(m_nDefPerStrip[0][DEF_NICK] > 0 && pDoc->m_nSapp3Code[SAPP3_NICK] > 0)
		{
			strData.Format(_T("B%d,%d\r\n"),pDoc->m_nSapp3Code[SAPP3_NICK], m_nDefPerStrip[0][DEF_NICK]); // ���(B137)
			strFileData += strData;
		}

		nSum = m_nDefPerStrip[0][DEF_SPACE] + m_nDefPerStrip[0][DEF_EXTRA] + m_nDefPerStrip[0][DEF_PROTRUSION];
		if(nSum > 0 && pDoc->m_nSapp3Code[SAPP3_SPACE_EXTRA_PROTRUSION] > 0)
		{
			strData.Format(_T("B%d,%d\r\n"),pDoc->m_nSapp3Code[SAPP3_SPACE_EXTRA_PROTRUSION], nSum); // ������+�ܵ�+����(B160)
			strFileData += strData;
		}

	// 			nSum = m_nDefPerStrip[0][DEF_PINHOLE] + m_nDefPerStrip[0][DEF_PAD];
	// 			if(nSum > 0 && pDoc->m_nSapp3Code[SAPP3_PINHOLE_PAD] > 0)
	// 			{
	// 				strData.Format(_T("B%d,%d\r\n"),pDoc->m_nSapp3Code[SAPP3_PINHOLE_PAD], nSum); // ��Ȧ+�е�(B134)
	// 				strFileData += strData;
	// 			}

		nSum = m_nDefPerStrip[0][DEF_PINHOLE];
		if(nSum > 0 && pDoc->m_nSapp3Code[SAPP3_PINHOLE] > 0)
		{
			strData.Format(_T("B%d,%d\r\n"),pDoc->m_nSapp3Code[SAPP3_PINHOLE], nSum); // ��Ȧ(B134)
			strFileData += strData;
		}

		nSum = m_nDefPerStrip[0][DEF_PAD];
		if(nSum > 0 && pDoc->m_nSapp3Code[SAPP3_PAD] > 0)
		{
			strData.Format(_T("B%d,%d\r\n"),pDoc->m_nSapp3Code[SAPP3_PAD], nSum); // �е�(B316)
			strFileData += strData;
		}

		if(nSum > 0 && pDoc->m_nSapp3Code[SAPP3_HOPEN] > 0)
		{
			strData.Format(_T("B%d,%d\r\n"),pDoc->m_nSapp3Code[SAPP3_HOPEN], m_nDefPerStrip[0][DEF_HOLE_OPEN]);
			strFileData += strData;
		}

		nSum = m_nDefPerStrip[0][DEF_HOLE_MISS] + m_nDefPerStrip[0][DEF_HOLE_POSITION] + m_nDefPerStrip[0][DEF_HOLE_DEFECT];
		if(nSum > 0 && pDoc->m_nSapp3Code[SAPP3_HMISS_HPOS_HBAD] > 0)
		{
			strData.Format(_T("B%d,%d\r\n"),pDoc->m_nSapp3Code[SAPP3_HMISS_HPOS_HBAD], nSum);
			strFileData += strData;
		}

		nSum = m_nDefPerStrip[0][DEF_VH_OPEN] + m_nDefPerStrip[0][DEF_VH_MISS] + m_nDefPerStrip[0][DEF_VH_POSITION] + m_nDefPerStrip[0][DEF_VH_DEF];
		if (nSum > 0 && pDoc->m_nSapp3Code[SAPP3_VHOPEN_NOVH_VHALIGN_VHDEF] > 0)
		{
			strData.Format(_T("B%d,%d\r\n"), pDoc->m_nSapp3Code[SAPP3_VHOPEN_NOVH_VHALIGN_VHDEF], nSum);
			strFileData += strData;
		}
	}


	// �ӵ�.
	strFileData += _T("\r\nS\r\n");
	strData.Format(_T("%.2f"), dEntireSpeed);
	strFileData += strData;
	strFileData += _T("\r\n");


	return strFileData;
}

void CDlgMenu05::OnBtnSave2() 
{
	// TODO: Add your control notification handler code here
	if (pView->IsRun() || pView->IsBuffer())
	{
		pView->MsgBox(_T("�۾����Դϴ�."));
		return;
	}

	Print(AfxGetMainWnd()->GetSafeHwnd());
}

void CDlgMenu05::Print(HWND hWndMain)
{
	HDC hPrtdc;
	int xpage, ypage;
	DOCINFO doc;
	int Result;
	PRINTDLG pd;

// �����Ϳ� ���� ������ ���ϰ� DC�� �����.
	memset(&pd,0,sizeof(PRINTDLG));
	pd.lStructSize=sizeof(PRINTDLG);
	pd.Flags=PD_RETURNDC;
	pd.hwndOwner=hWndMain;
	pd.nFromPage=1;
	pd.nToPage=1;
	pd.nMinPage=1;
	pd.nMaxPage=1;
	pd.nCopies=1;
	PrintDlg(&pd);
	hPrtdc=pd.hDC;
	if (hPrtdc == NULL) return;

	// �μ� �۾��� �����Ѵ�.
	doc.cbSize=sizeof(DOCINFO);
	doc.lpszDocName=_T("Test Document");
	doc.lpszOutput=NULL;
	doc.lpszDatatype=NULL;
	doc.fwType=0;
	Result=StartDoc(hPrtdc, &doc);
	if (Result <= 0) goto end;
	Result=StartPage(hPrtdc);
	if (Result <= 0) goto end;

	// ����� ������.
	xpage = GetDeviceCaps(hPrtdc, HORZRES);
	ypage = GetDeviceCaps(hPrtdc, VERTRES);
	Rectangle(hPrtdc,0,0,xpage,ypage);
	SetTextAlign(hPrtdc,TA_LEFT);
	PrintData(hPrtdc, xpage, ypage);

	// ����� �����Ѵ�.
	Result=EndPage(hPrtdc);
	if (Result <= 0) goto end;

	// �μ� �۾��� ������.
	Result=EndDoc(hPrtdc);
	if (Result <= 0) goto end;

end:
	if (pd.hDevMode) GlobalFree(pd.hDevMode);
	if (pd.hDevNames) GlobalFree(pd.hDevNames);
	DeleteDC(hPrtdc);
	return;
}


void CDlgMenu05::PrintData(HDC hPrtdc, int xpage, int ypage)
{
	CString strFileData, strData;
	int nCharHight = ypage / 70; // ���������� 70 Line���� ����.
	int nHorizOffset = xpage / 48; // ��Line�� 48����� Offset ����.

	int nStripNumY = MAX_STRIP_NUM; // pGlobalDoc->m_aryStripRegion.NodeNumY/pGlobalDoc->m_aryStripRegion.nCellDivideY; // 4
	int nEntireStripNum = m_nMarkedShot*nStripNumY;
	int nTot;

	//����Ʈ �ۼ�. =====================================================================

	strFileData = _T("1. �Ϲ� ����");
	TextOut(hPrtdc,nHorizOffset,5*nCharHight,strFileData,strFileData.GetLength());
	
	strFileData.Format(_T("    �� �� �� :  %s"), m_sModel);
	TextOut(hPrtdc,nHorizOffset,6*nCharHight,strFileData,strFileData.GetLength());

	strFileData.Format(_T("    �� Ʈ �� :  %s"), m_sLot);
	TextOut(hPrtdc,nHorizOffset,7*nCharHight,strFileData,strFileData.GetLength());

	strFileData.Format(_T("    ���۽ð� :  %s"), m_strLotStartTime);
	TextOut(hPrtdc,nHorizOffset,8*nCharHight,strFileData,strFileData.GetLength());

	strFileData.Format(_T("    ����ð� :  %s"), m_strLotWorkingTime);
	TextOut(hPrtdc,nHorizOffset,9*nCharHight,strFileData,strFileData.GetLength());

	strFileData.Format(_T("    ����ð� :  %s"), m_strLotEndTime);
	TextOut(hPrtdc,nHorizOffset,10*nCharHight,strFileData,strFileData.GetLength());


	strFileData = _T("2. ��ǰ �˻� ���");
	TextOut(hPrtdc,nHorizOffset,12*nCharHight,strFileData,strFileData.GetLength());

	strFileData.Format(_T("    �� �� �� Unit �� : %9d        �� �� �� Strip �� : %9d"), (m_nEntirePieceNum), nEntireStripNum);
	TextOut(hPrtdc,nHorizOffset,13*nCharHight,strFileData,strFileData.GetLength());

	strFileData.Format(_T("    ��    ǰ Unit �� : %9d        ��    ǰ Strip �� : %9d"), m_nEntirePieceNum - m_nDefectPieceNum, nEntireStripNum - m_nTotStOut);
	TextOut(hPrtdc,nHorizOffset,14*nCharHight,strFileData,strFileData.GetLength());

	strFileData.Format(_T("    ��  ǰ  Unit ��(%%) : %9.2f        ��  ǰ  Strip ��(%%) : %9.2f"), 100.0*(double)(m_nEntirePieceNum-m_nDefectPieceNum)/(double)(m_nEntirePieceNum), 100.0*(double)(nEntireStripNum-m_nTotStOut)/(double)nEntireStripNum);
	TextOut(hPrtdc,nHorizOffset,15*nCharHight,strFileData,strFileData.GetLength());

	strFileData.Format(_T("    ��    �� Unit �� : %9d        ��    �� Strip �� : %9d"), (m_nDefectPieceNum), m_nTotStOut);
	TextOut(hPrtdc,nHorizOffset,16*nCharHight,strFileData,strFileData.GetLength());

	strFileData.Format(_T("    ��  ��  Unit ��(%%) : %9.2f        ��  ��  Strip ��(%%) : %9.2f"), 100.0*(double)(m_nDefectPieceNum)/(double)(m_nEntirePieceNum), 100.0*(double)m_nTotStOut/(double)nEntireStripNum);
	TextOut(hPrtdc,nHorizOffset,17*nCharHight,strFileData,strFileData.GetLength());


	strFileData = _T("3. ���� �˻� ����");
	TextOut(hPrtdc,nHorizOffset,19*nCharHight,strFileData,strFileData.GetLength());

	strFileData.Format(_T("    1�� �˻� Unit �� : %9d        1�� �˻� Strip �� : %9d"), (m_nEntirePieceNum)/nStripNumY, nEntireStripNum/nStripNumY);
	TextOut(hPrtdc,nHorizOffset,20*nCharHight,strFileData,strFileData.GetLength());

	strFileData.Format(_T("    1�� ��ǰ Unit �� : %9d        1�� ��ǰ Strip �� : %9d"), m_nEntirePieceNum/nStripNumY - m_nDefStrip[0], nEntireStripNum/nStripNumY - m_nStripOut[0]);
	TextOut(hPrtdc,nHorizOffset,21*nCharHight,strFileData,strFileData.GetLength());

	strFileData.Format(_T("    1�� ��ǰ Unit ��(%%) : %9.2f        1�� ��ǰ Strip ��(%%) : %9.2f"), 100.0*(double)(m_nEntirePieceNum/nStripNumY - m_nDefStrip[0])/(double)((m_nEntirePieceNum)/nStripNumY), 100.0*(double)(nEntireStripNum/nStripNumY - m_nStripOut[0])/(double)(nEntireStripNum/nStripNumY));
	TextOut(hPrtdc,nHorizOffset,22*nCharHight,strFileData,strFileData.GetLength());

	strFileData.Format(_T("    1�� �ҷ� Unit �� : %9d        1�� �ҷ� Strip �� : %9d"), m_nDefStrip[0], m_nStripOut[0]);
	TextOut(hPrtdc,nHorizOffset,23*nCharHight,strFileData,strFileData.GetLength());

	strFileData.Format(_T("    1�� �ҷ� Unit ��(%%) : %9.2f        1�� �ҷ� Strip ��(%%) : %9.2f"), 100.0*(double)(m_nDefStrip[0])/(double)((m_nEntirePieceNum)/nStripNumY), 100.0*(double)m_nStripOut[0]/(double)(nEntireStripNum/nStripNumY));
	TextOut(hPrtdc,nHorizOffset,24*nCharHight,strFileData,strFileData.GetLength());


	strFileData.Format(_T("    2�� �˻� Unit �� : %9d        2�� �˻� Unit �� : %9d"), (m_nEntirePieceNum)/nStripNumY, nEntireStripNum/nStripNumY);
	TextOut(hPrtdc,nHorizOffset,26*nCharHight,strFileData,strFileData.GetLength());

	strFileData.Format(_T("    2�� ��ǰ Unit �� : %9d        2�� ��ǰ Strip �� : %9d"), m_nEntirePieceNum/nStripNumY - m_nDefStrip[1], nEntireStripNum/nStripNumY - m_nStripOut[1]);
	TextOut(hPrtdc,nHorizOffset,27*nCharHight,strFileData,strFileData.GetLength());

	strFileData.Format(_T("    2�� ��ǰ Unit ��(%%) : %9.2f        2�� ��ǰ Strip ��(%%) : %9.2f"), 100.0*(double)(m_nEntirePieceNum/nStripNumY - m_nDefStrip[1])/(double)((m_nEntirePieceNum)/nStripNumY), 100.0*(double)(nEntireStripNum/nStripNumY - m_nStripOut[1])/(double)(nEntireStripNum/nStripNumY));
	TextOut(hPrtdc,nHorizOffset,28*nCharHight,strFileData,strFileData.GetLength());

	strFileData.Format(_T("    2�� �ҷ� Unit �� : %9d        2�� �ҷ� Strip �� : %9d"), (m_nDefStrip[1]), m_nStripOut[1]);
	TextOut(hPrtdc,nHorizOffset,29*nCharHight,strFileData,strFileData.GetLength());

	strFileData.Format(_T("    2�� �ҷ� Unit ��(%%) : %9.2f        2�� �ҷ� Strip ��(%%) : %9.2f"), 100.0*(double)(m_nDefStrip[1])/(double)((m_nEntirePieceNum)/nStripNumY), 100.0*(double)m_nStripOut[1]/(double)(nEntireStripNum/nStripNumY));
	TextOut(hPrtdc,nHorizOffset,30*nCharHight,strFileData,strFileData.GetLength());


	strFileData.Format(_T("    3�� �˻� Unit �� : %9d        3�� �˻� Unit �� : %9d"), (m_nEntirePieceNum)/nStripNumY, nEntireStripNum/nStripNumY);
	TextOut(hPrtdc,nHorizOffset,32*nCharHight,strFileData,strFileData.GetLength());

	strFileData.Format(_T("    3�� ��ǰ Unit �� : %9d        3�� ��ǰ Strip �� : %9d"), m_nEntirePieceNum/nStripNumY - m_nDefStrip[2], nEntireStripNum/nStripNumY - m_nStripOut[2]);
	TextOut(hPrtdc,nHorizOffset,33*nCharHight,strFileData,strFileData.GetLength());

	strFileData.Format(_T("    3�� ��ǰ Unit ��(%%) : %9.2f        3�� ��ǰ Strip ��(%%) : %9.2f"), 100.0*(double)(m_nEntirePieceNum/nStripNumY - m_nDefStrip[2])/(double)((m_nEntirePieceNum)/nStripNumY), 100.0*(double)(nEntireStripNum/nStripNumY - m_nStripOut[2])/(double)(nEntireStripNum/nStripNumY));
	TextOut(hPrtdc,nHorizOffset,34*nCharHight,strFileData,strFileData.GetLength());

	strFileData.Format(_T("    3�� �ҷ� Unit �� : %9d        3�� �ҷ� Strip �� : %9d"), (m_nDefStrip[2]), m_nStripOut[2]);
	TextOut(hPrtdc,nHorizOffset,35*nCharHight,strFileData,strFileData.GetLength());

	strFileData.Format(_T("    3�� �ҷ� Unit ��(%%) : %9.2f        3�� �ҷ� Strip ��(%%) : %9.2f"), 100.0*(double)(m_nDefStrip[2])/(double)((m_nEntirePieceNum)/nStripNumY), 100.0*(double)m_nStripOut[2]/(double)(nEntireStripNum/nStripNumY));
	TextOut(hPrtdc,nHorizOffset,36*nCharHight,strFileData,strFileData.GetLength());


	strFileData.Format(_T("    4�� �˻� Unit �� : %9d        4�� �˻� Unit �� : %9d"), (m_nEntirePieceNum)/nStripNumY, nEntireStripNum/nStripNumY);
	TextOut(hPrtdc,nHorizOffset,38*nCharHight,strFileData,strFileData.GetLength());

	strFileData.Format(_T("    4�� ��ǰ Unit �� : %9d        4�� ��ǰ Strip �� : %9d"), m_nEntirePieceNum/nStripNumY - m_nDefStrip[3], nEntireStripNum/nStripNumY - m_nStripOut[3]);
	TextOut(hPrtdc,nHorizOffset,39*nCharHight,strFileData,strFileData.GetLength());

	strFileData.Format(_T("    4�� ��ǰ Unit ��(%%) : %9.2f        4�� ��ǰ Strip ��(%%) : %9.2f"), 100.0*(double)(m_nEntirePieceNum/nStripNumY - m_nDefStrip[3])/(double)((m_nEntirePieceNum)/nStripNumY), 100.0*(double)(nEntireStripNum/nStripNumY - m_nStripOut[3])/(double)(nEntireStripNum/nStripNumY));
	TextOut(hPrtdc,nHorizOffset,40*nCharHight,strFileData,strFileData.GetLength());

	strFileData.Format(_T("    4�� �ҷ� Unit �� : %9d        4�� �ҷ� Strip �� : %9d"), (m_nDefStrip[3]), m_nStripOut[3]);
	TextOut(hPrtdc,nHorizOffset,41*nCharHight,strFileData,strFileData.GetLength());

	strFileData.Format(_T("    4�� �ҷ� Unit ��(%%) : %9.2f        4�� �ҷ� Strip ��(%%) : %9.2f"), 100.0*(double)(m_nDefStrip[3])/(double)((m_nEntirePieceNum)/nStripNumY), 100.0*(double)m_nStripOut[3]/(double)(nEntireStripNum/nStripNumY));
	TextOut(hPrtdc,nHorizOffset,42*nCharHight,strFileData,strFileData.GetLength());

	
	strFileData = _T("4. �ҷ� ����");
	TextOut(hPrtdc,nHorizOffset,44*nCharHight,strFileData,strFileData.GetLength());
	strFileData = _T("    -----------------------------------------------------------------------------");
	TextOut(hPrtdc,nHorizOffset,45*nCharHight,strFileData,strFileData.GetLength());
	strFileData = _T("    ��ȣ     �ҷ�����       1��       2��       3��       4��       ������ �ҷ���");
	TextOut(hPrtdc,nHorizOffset,46*nCharHight,strFileData,strFileData.GetLength());
	strFileData = _T("    -----------------------------------------------------------------------------");
	TextOut(hPrtdc,nHorizOffset,47*nCharHight,strFileData,strFileData.GetLength());

	strFileData.Format(_T("     1         ����  %10d%10d%10d%10d%20d"), m_nDefPerStrip[0][DEF_OPEN], m_nDefPerStrip[1][DEF_OPEN], m_nDefPerStrip[2][DEF_OPEN], m_nDefPerStrip[3][DEF_OPEN], m_nEntireAddedDefect[DEF_OPEN]);
	TextOut(hPrtdc,nHorizOffset,48*nCharHight,strFileData,strFileData.GetLength());

	strFileData.Format(_T("     2         ��Ʈ  %10d%10d%10d%10d%20d"), m_nDefPerStrip[0][DEF_SHORT], m_nDefPerStrip[1][DEF_SHORT], m_nDefPerStrip[2][DEF_SHORT], m_nDefPerStrip[3][DEF_SHORT], m_nEntireAddedDefect[DEF_SHORT]);
	TextOut(hPrtdc,nHorizOffset,49*nCharHight,strFileData,strFileData.GetLength());

	strFileData.Format(_T("     3        U-��Ʈ %10d%10d%10d%10d%20d"), m_nDefPerStrip[0][DEF_USHORT], m_nDefPerStrip[1][DEF_USHORT], m_nDefPerStrip[2][DEF_USHORT], m_nDefPerStrip[3][DEF_USHORT], m_nEntireAddedDefect[DEF_USHORT]);
	TextOut(hPrtdc,nHorizOffset,50*nCharHight,strFileData,strFileData.GetLength());

	strFileData.Format(_T("     4         ���  %10d%10d%10d%10d%20d"), m_nDefPerStrip[0][DEF_NICK], m_nDefPerStrip[1][DEF_NICK], m_nDefPerStrip[2][DEF_NICK], m_nDefPerStrip[3][DEF_NICK], m_nEntireAddedDefect[DEF_NICK]);
	TextOut(hPrtdc,nHorizOffset,51*nCharHight,strFileData,strFileData.GetLength());

	strFileData.Format(_T("     5        ������ %10d%10d%10d%10d%20d"), m_nDefPerStrip[0][DEF_SPACE], m_nDefPerStrip[1][DEF_SPACE], m_nDefPerStrip[2][DEF_SPACE], m_nDefPerStrip[3][DEF_SPACE], m_nEntireAddedDefect[DEF_SPACE]);
	TextOut(hPrtdc,nHorizOffset,52*nCharHight,strFileData,strFileData.GetLength());

	strFileData.Format(_T("     6         �ܵ�  %10d%10d%10d%10d%20d"), m_nDefPerStrip[0][DEF_EXTRA], m_nDefPerStrip[1][DEF_EXTRA], m_nDefPerStrip[2][DEF_EXTRA], m_nDefPerStrip[3][DEF_EXTRA], m_nEntireAddedDefect[DEF_EXTRA]);
	TextOut(hPrtdc,nHorizOffset,53*nCharHight,strFileData,strFileData.GetLength());

	strFileData.Format(_T("     7         ����  %10d%10d%10d%10d%20d"), m_nDefPerStrip[0][DEF_PROTRUSION], m_nDefPerStrip[1][DEF_PROTRUSION], m_nDefPerStrip[2][DEF_PROTRUSION], m_nDefPerStrip[3][DEF_PROTRUSION], m_nEntireAddedDefect[DEF_PROTRUSION]);
	TextOut(hPrtdc,nHorizOffset,54*nCharHight,strFileData,strFileData.GetLength());

	strFileData.Format(_T("     8         ��Ȧ  %10d%10d%10d%10d%20d"), m_nDefPerStrip[0][DEF_PINHOLE], m_nDefPerStrip[1][DEF_PINHOLE], m_nDefPerStrip[2][DEF_PINHOLE], m_nDefPerStrip[3][DEF_PINHOLE], m_nEntireAddedDefect[DEF_PINHOLE]);
	TextOut(hPrtdc,nHorizOffset,55*nCharHight,strFileData,strFileData.GetLength());

	strFileData.Format(_T("     9       �е����%10d%10d%10d%10d%20d"), m_nDefPerStrip[0][DEF_PAD], m_nDefPerStrip[1][DEF_PAD], m_nDefPerStrip[2][DEF_PAD], m_nDefPerStrip[3][DEF_PAD], m_nEntireAddedDefect[DEF_PAD]);
	TextOut(hPrtdc,nHorizOffset,56*nCharHight,strFileData,strFileData.GetLength());

	strFileData.Format(_T("    10        Ȧ���� %10d%10d%10d%10d%20d"), m_nDefPerStrip[0][DEF_HOLE_OPEN], m_nDefPerStrip[1][DEF_HOLE_OPEN], m_nDefPerStrip[2][DEF_HOLE_OPEN], m_nDefPerStrip[3][DEF_HOLE_OPEN], m_nEntireAddedDefect[DEF_HOLE_OPEN]);
	TextOut(hPrtdc,nHorizOffset,57*nCharHight,strFileData,strFileData.GetLength());

	strFileData.Format(_T("    11        Ȧ���� %10d%10d%10d%10d%20d"), m_nDefPerStrip[0][DEF_HOLE_MISS], m_nDefPerStrip[1][DEF_HOLE_MISS], m_nDefPerStrip[2][DEF_HOLE_MISS], m_nDefPerStrip[3][DEF_HOLE_MISS], m_nEntireAddedDefect[DEF_HOLE_MISS]);
	TextOut(hPrtdc,nHorizOffset,58*nCharHight,strFileData,strFileData.GetLength());

	strFileData.Format(_T("    12        Ȧ��� %10d%10d%10d%10d%20d"), m_nDefPerStrip[0][DEF_HOLE_POSITION], m_nDefPerStrip[1][DEF_HOLE_POSITION], m_nDefPerStrip[2][DEF_HOLE_POSITION], m_nDefPerStrip[3][DEF_HOLE_POSITION], m_nEntireAddedDefect[DEF_HOLE_POSITION]);
	TextOut(hPrtdc,nHorizOffset,59*nCharHight,strFileData,strFileData.GetLength());

	strFileData.Format(_T("    13       Ȧ���ҷ�%10d%10d%10d%10d%20d"), m_nDefPerStrip[0][DEF_HOLE_DEFECT], m_nDefPerStrip[1][DEF_HOLE_DEFECT], m_nDefPerStrip[2][DEF_HOLE_DEFECT], m_nDefPerStrip[3][DEF_HOLE_DEFECT], m_nEntireAddedDefect[DEF_HOLE_DEFECT]);
	TextOut(hPrtdc,nHorizOffset,60*nCharHight,strFileData,strFileData.GetLength());

	strFileData.Format(_T("    14          POI  %10d%10d%10d%10d%20d"), m_nDefPerStrip[0][DEF_POI], m_nDefPerStrip[1][DEF_POI], m_nDefPerStrip[2][DEF_POI], m_nDefPerStrip[3][DEF_POI], m_nEntireAddedDefect[DEF_POI]);
	TextOut(hPrtdc,nHorizOffset,61*nCharHight,strFileData,strFileData.GetLength());

	strFileData.Format(_T("    15        VH���� %10d%10d%10d%10d%20d"), m_nDefPerStrip[0][DEF_VH_OPEN], m_nDefPerStrip[1][DEF_VH_OPEN], m_nDefPerStrip[2][DEF_VH_OPEN], m_nDefPerStrip[3][DEF_VH_OPEN], m_nEntireAddedDefect[DEF_VH_OPEN]);
	TextOut(hPrtdc,nHorizOffset,62*nCharHight,strFileData,strFileData.GetLength());

	strFileData.Format(_T("    16        VH���� %10d%10d%10d%10d%20d"), m_nDefPerStrip[0][DEF_VH_MISS], m_nDefPerStrip[1][DEF_VH_MISS], m_nDefPerStrip[2][DEF_VH_MISS], m_nDefPerStrip[3][DEF_VH_MISS], m_nEntireAddedDefect[DEF_VH_MISS]);
	TextOut(hPrtdc,nHorizOffset,63*nCharHight,strFileData,strFileData.GetLength());

	strFileData.Format(_T("    17        VH��� %10d%10d%10d%10d%20d"), m_nDefPerStrip[0][DEF_VH_POSITION], m_nDefPerStrip[1][DEF_VH_POSITION], m_nDefPerStrip[2][DEF_VH_POSITION], m_nDefPerStrip[3][DEF_VH_POSITION], m_nEntireAddedDefect[DEF_VH_POSITION]);
	TextOut(hPrtdc,nHorizOffset,64*nCharHight,strFileData,strFileData.GetLength());

	strFileData.Format(_T("    18        VH���� %10d%10d%10d%10d%20d"), m_nDefPerStrip[0][DEF_VH_DEF], m_nDefPerStrip[1][DEF_VH_DEF], m_nDefPerStrip[2][DEF_VH_DEF], m_nDefPerStrip[3][DEF_VH_DEF], m_nEntireAddedDefect[DEF_VH_DEF]);
	TextOut(hPrtdc,nHorizOffset,65*nCharHight,strFileData,strFileData.GetLength());

	nTot = m_nDefPerStrip[0][DEF_LIGHT]+m_nDefPerStrip[1][DEF_LIGHT]+m_nDefPerStrip[2][DEF_LIGHT]+m_nDefPerStrip[3][DEF_LIGHT];
	strFileData.Format(_T("    19         �뱤  %10d%10d%10d%10d%20d"), m_nDefPerStrip[0][DEF_LIGHT], m_nDefPerStrip[1][DEF_LIGHT], m_nDefPerStrip[2][DEF_LIGHT], m_nDefPerStrip[3][DEF_LIGHT], nTot);
	TextOut(hPrtdc,nHorizOffset,66*nCharHight,strFileData,strFileData.GetLength());

	strFileData = _T("    -----------------------------------------------------------------------------");
	TextOut(hPrtdc,nHorizOffset,67*nCharHight,strFileData,strFileData.GetLength());

}

void CDlgMenu05::OnCheck1() 
{
	// TODO: Add your control notification handler code here
	BOOL bOn = ((CButton*)GetDlgItem(IDC_CHECK1))->GetCheck();

	if(bOn)
	{
		CRect rt;		
//		GetDlgItem(IDC_STATIC_MK_TEST)->GetWindowRect(&rt);
		if(m_pDlgUtil01)
		{
			if(!m_pDlgUtil01->IsWindowVisible())
			{
				m_pDlgUtil01->ShowWindow(SW_SHOW);
			}
//			m_pDlgUtil01->MoveWindow(rt, TRUE);
		}
	}
	else
	{
		if(m_pDlgUtil01)
		{
			if(m_pDlgUtil01->IsWindowVisible())
			{
				m_pDlgUtil01->ShowWindow(SW_HIDE);
			}
		}
	}	
}


void CDlgMenu05::ShowDlg(int nID)
{
	switch(nID) 
	{
	case IDD_DLG_UTIL_01:
		if(!m_pDlgUtil01)
		{
			m_pDlgUtil01 = new CDlgUtil01(this);
			if(m_pDlgUtil01->GetSafeHwnd() == 0)
			{
				m_pDlgUtil01->Create();
				m_pDlgUtil01->ShowWindow(SW_SHOW);
			}
		}
		else
		{
			m_pDlgUtil01->ShowWindow(SW_SHOW);
		}
		break;	
	}
}


int CDlgMenu05::LoadPCRUpFromMk(int nSerial)	// return : 2(Failed), 1(����), -1(Align Error, �뱤�ҷ�), -2(Lot End)
{
	FILE *fp;
	char FileD[200];
	size_t nFileSize, nRSize;
	char *FileData;
	CString strFileData;
	int nTemp, i, nC, nR;
	CString strHeaderErrorInfo, strModel, strLayer, strLot, sItsCode, strTotalBadPieceNum;
	CString strCamID, strPieceID, strBadPointPosX, strBadPointPosY, strBadName,
		strCellNum, strImageSize, strImageNum, strMarkingCode;

	if (nSerial <= 0)
	{
		strFileData.Format(_T("PCR������ �������� �ʾҽ��ϴ�."));
		pView->MsgBox(strFileData);
		// 		AfxMessageBox(strFileData);
		return(2);
	}

	if (!pDoc->m_pPcr[0])
	{
		strFileData.Format(_T("PCR[0]���� �޸𸮰� �Ҵ���� �ʾҽ��ϴ�."));
		pView->MsgBox(strFileData);
		// 		AfxMessageBox(strFileData);
		return(2);
	}

	int nIdx;
	nIdx = pDoc->GetPcrIdx0(nSerial);

	CString sPath, strPath;
	CFileFind findfile;

	sPath.Format(_T("%s%s\\%s\\%s\\OFFLINE\\%04d.pcr"),
		pDoc->WorkingInfo.System.sPathOldFile, m_sModel, m_sLot, m_sLayer, nSerial);

	if (!findfile.FindFile(sPath))
	{
		sPath.Format(_T("%s%s\\%s\\%s\\%04d.pcr"),
			pDoc->WorkingInfo.System.sPathOldFile, m_sModel, m_sLot, m_sLayer, nSerial);
		if (!findfile.FindFile(sPath))
		{
			return(2);
		}
	}

	//strcpy(FileD, sPath);
	//_tcscpy(FileD, sPath);
	StringToChar(sPath, FileD);

	if ((fp = fopen(FileD, "r")) != NULL)
	{
		fseek(fp, 0, SEEK_END);
		nFileSize = ftell(fp);
		fseek(fp, 0, SEEK_SET);

		/* Allocate space for a path name */
		//FileData = (char*)malloc( nFileSize );
		FileData = (char*)calloc(nFileSize + 1, sizeof(char));

		nRSize = fread(FileData, sizeof(char), nFileSize, fp);
		//strFileData.Format(_T("%s"), CharToString(FileData));
		strFileData = CharToString(FileData);
		fclose(fp);
		free(FileData);
	}
	else
	{
		strFileData.Format(_T("PCR[Up] ������ �������� �ʽ��ϴ�.\r\n%s"), sPath);
		pView->MsgBox(strFileData);
		// 		AfxMessageBox(strFileData);
		return(2);
	}

	if (!pDoc->m_pPcr[0])
		return(2);
	if (!pDoc->m_pPcr[0][nIdx])
		return(2);

	BOOL bResetMkInfo = FALSE;

	pDoc->m_pPcr[0][nIdx]->m_nIdx = nIdx;					// m_nIdx : From 0 to nTot.....
	pDoc->m_pPcr[0][nIdx]->m_nSerial = nSerial;

	// Error Code											// 1(����), -1(Align Error, �뱤�ҷ�), -2(Lot End)
	nTemp = strFileData.Find(',', 0);
	strHeaderErrorInfo = strFileData.Left(nTemp);
	strFileData.Delete(0, nTemp + 1);
	nFileSize = nFileSize - nTemp - 1;
	pDoc->m_pPcr[0][nIdx]->m_nErrPnl = _tstoi(strHeaderErrorInfo);


	// Model
	nTemp = strFileData.Find(',', 0);
	strModel = strFileData.Left(nTemp);
	strFileData.Delete(0, nTemp + 1);
	nFileSize = nFileSize - nTemp - 1;
	pDoc->m_pPcr[0][nIdx]->m_sModel = strModel;

	// Layer
	nTemp = strFileData.Find(',', 0);
	strLayer = strFileData.Left(nTemp);
	strFileData.Delete(0, nTemp + 1);
	nFileSize = nFileSize - nTemp - 1;
	pDoc->m_pPcr[0][nIdx]->m_sLayer = strLayer;

	if (pDoc->WorkingInfo.System.bUseITS)
	{
		// Lot
		nTemp = strFileData.Find(',', 0);
		strLot = strFileData.Left(nTemp);
		strFileData.Delete(0, nTemp + 1);
		nFileSize = nFileSize - nTemp - 1;
		pDoc->m_pPcr[0][nIdx]->m_sLot = strLot;

		// Its Code
		nTemp = strFileData.Find('\n', 0);
		sItsCode = strFileData.Left(nTemp);
		strFileData.Delete(0, nTemp + 1);
		nFileSize = nFileSize - nTemp - 1;
		pDoc->m_pPcr[0][nIdx]->m_sItsCode = sItsCode;
	}
	else
	{
		// Lot
		nTemp = strFileData.Find(',', 0);
		strLot = strFileData.Left(nTemp);
		strFileData.Delete(0, nTemp + 1);
		nFileSize = nFileSize - nTemp - 1;
		pDoc->m_pPcr[0][nIdx]->m_sLot = strLot;

		// Lot
		nTemp = strFileData.Find('\n', 0);
		strLot = strFileData.Left(nTemp);
		strFileData.Delete(0, nTemp + 1);
		nFileSize = nFileSize - nTemp - 1;
		pDoc->m_pPcr[0][nIdx]->m_sLot = strLot;
	}

	nTemp = strFileData.Find('\n', 0);
	strTotalBadPieceNum = strFileData.Left(nTemp);;
	strFileData.Delete(0, nTemp + 1);
	nFileSize = nFileSize - nTemp - 1;

	if (pDoc->GetTestMode() == MODE_INNER || pDoc->GetTestMode() == MODE_OUTER)
	{
		pDoc->m_sEngModel = strModel;
		pDoc->m_sEngLotNum = strLot;
		pDoc->m_sEngLayerUp = strLayer;
		//pDoc->m_sEngLayerDn = strModel;
		if (pDoc->WorkingInfo.System.bUseITS)
			pDoc->m_sItsCode = sItsCode;
	}


	int nTotDef = _tstoi(strTotalBadPieceNum);

	pDoc->m_pPcr[0][nIdx]->Init(nSerial, nTotDef);

	if (nTotDef > 0)
	{
		for (i = 0; i < nTotDef; i++)
		{
			// Cam ID
			nTemp = strFileData.Find(',', 0);
			strCamID = strFileData.Left(nTemp);
			strFileData.Delete(0, nTemp + 1);
			nFileSize = nFileSize - nTemp - 1;
			pDoc->m_pPcr[0][nIdx]->m_nCamId = _tstoi(strCamID);

			// Piece Number
			nTemp = strFileData.Find(',', 0);
			strPieceID = strFileData.Left(nTemp);
			strFileData.Delete(0, nTemp + 1);
			nFileSize = nFileSize - nTemp - 1;

			// LoadStripPieceRegion_Binary()�� ���� PCS Index�� ������.
			if (pDoc->WorkingInfo.System.bStripPcsRgnBin)	// DTS��
			{
				pDoc->m_pPcr[0][nIdx]->m_pDefPcs[i] = _tstoi(strPieceID);

				//switch (pDoc->m_Master[0].MasterInfo.nActionCode)	// 0 : Rotation / Mirror ���� ����(CAM Data ����), 1 : �¿� �̷�, 2 : ���� �̷�, 3 : 180 ȸ��, 4 : 270 ȸ��(CCW), 5 : 90 ȸ��(CW)
				//{
				//case 0:
				//	pDoc->m_pPcr[0][nIdx]->m_pDefPcs[i] = _tstoi(strPieceID);
				//	break;
				//case 1:
				//	pDoc->m_pPcr[0][nIdx]->m_pDefPcs[i] = pDoc->MirrorLR(_tstoi(strPieceID));
				//	break;
				//case 3:
				//	pDoc->m_pPcr[0][nIdx]->m_pDefPcs[i] = pDoc->Rotate180(_tstoi(strPieceID));
				//	break;
				//default:
				//	pDoc->m_pPcr[0][nIdx]->m_pDefPcs[i] = _tstoi(strPieceID);
				//	break;
				//}
			}
			else
				pDoc->m_pPcr[0][nIdx]->m_pDefPcs[i] = _tstoi(strPieceID);

			pDoc->m_pPcr[0][nIdx]->m_pLayer[i] = 0; // Up

											  // BadPointPosX
			nTemp = strFileData.Find(',', 0);
			strBadPointPosX = strFileData.Left(nTemp);
			strFileData.Delete(0, nTemp + 1);
			nFileSize = nFileSize - nTemp - 1;
			pDoc->m_pPcr[0][nIdx]->m_pDefPos[i].x = (long)_tstoi(strBadPointPosX);

			// BadPointPosY
			nTemp = strFileData.Find(',', 0);
			strBadPointPosY = strFileData.Left(nTemp);
			strFileData.Delete(0, nTemp + 1);
			nFileSize = nFileSize - nTemp - 1;
			pDoc->m_pPcr[0][nIdx]->m_pDefPos[i].y = (long)_tstoi(strBadPointPosY);

			// BadName
			nTemp = strFileData.Find(',', 0);
			strBadName = strFileData.Left(nTemp);
			strFileData.Delete(0, nTemp + 1);
			nFileSize = nFileSize - nTemp - 1;
			pDoc->m_pPcr[0][nIdx]->m_pDefType[i] = _tstoi(strBadName);

			pDoc->m_Master[0].m_pPcsRgn->GetMkMatrix(pDoc->m_pPcr[0][nIdx]->m_pDefPcs[i], nC, nR);
			pDoc->m_pPcr[0][nIdx]->m_arDefType[nR][nC] = pDoc->m_pPcr[0][nIdx]->m_pDefType[i];
			pDoc->m_pPcr[0][nIdx]->m_arPcrLineNum[nR][nC] = i;

			// CellNum
			nTemp = strFileData.Find(',', 0);
			strCellNum = strFileData.Left(nTemp);
			strFileData.Delete(0, nTemp + 1);
			nFileSize = nFileSize - nTemp - 1;
			pDoc->m_pPcr[0][nIdx]->m_pCell[i] = _tstoi(strCellNum);

			// ImageSize
			nTemp = strFileData.Find(',', 0);
			strImageSize = strFileData.Left(nTemp);
			strFileData.Delete(0, nTemp + 1);
			nFileSize = nFileSize - nTemp - 1;
			pDoc->m_pPcr[0][nIdx]->m_pImgSz[i] = _tstoi(strImageSize);

			// ImageNum
			nTemp = strFileData.Find(',', 0);
			strImageNum = strFileData.Left(nTemp);
			strFileData.Delete(0, nTemp + 1);
			nFileSize = nFileSize - nTemp - 1;
			pDoc->m_pPcr[0][nIdx]->m_pImg[i] = _tstoi(strImageNum);

			// strMarkingCode : -2 (NoMarking)
			nTemp = strFileData.Find('\n', 0);
			strMarkingCode = strFileData.Left(nTemp);
			strFileData.Delete(0, nTemp + 1);
			nFileSize = nFileSize - nTemp - 1;
			pDoc->m_pPcr[0][nIdx]->m_pMk[i] = _tstoi(strMarkingCode);

		}
	}

	return (1); // 1(����)
				// 	return(m_pPcr[0][nIdx]->m_nErrPnl);
}

int CDlgMenu05::LoadPCRDnFromMk(int nSerial)	// return : 2(Failed), 1(����), -1(Align Error, �뱤�ҷ�), -2(Lot End)
{
	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;
	if (!bDualTest)
		return 1;

	FILE *fp;
	char FileD[200];
	size_t nFileSize, nRSize;
	char *FileData;
	CString strFileData;
	int nTemp, i, nC, nR;
	CString strHeaderErrorInfo, strModel, strLayer, strLot, sItsCode, strTotalBadPieceNum;
	CString strCamID, strPieceID, strBadPointPosX, strBadPointPosY, strBadName,
		strCellNum, strImageSize, strImageNum, strMarkingCode;

	if (nSerial <= 0)
	{
		strFileData.Format(_T("PCR������ �������� �ʾҽ��ϴ�."));
		pView->MsgBox(strFileData);
		//AfxMessageBox(strFileData);
		return(2);
	}

	if (!pDoc->m_pPcr[1])
	{
		strFileData.Format(_T("PCR[1]���� �޸𸮰� �Ҵ���� �ʾҽ��ϴ�."));
		pView->MsgBox(strFileData);
		//AfxMessageBox(strFileData);
		return(2);
	}

	int nIdx;
	nIdx = pDoc->GetPcrIdx1(nSerial);

	CString sPath, strPath;
	CFileFind findfile;

	sPath.Format(_T("%s%s\\%s\\%s\\OFFLINE\\%04d.pcr"),
		pDoc->WorkingInfo.System.sPathOldFile, m_sModel, m_sLot, m_sLayer, nSerial);

	if (!findfile.FindFile(sPath))
	{
		sPath.Format(_T("%s%s\\%s\\%s\\%04d.pcr"),
			pDoc->WorkingInfo.System.sPathOldFile, m_sModel, m_sLot, m_sLayer, nSerial);
		if (!findfile.FindFile(sPath))
		{
			return(2);
		}
	}

	//strcpy(FileD, sPath);
	//_tcscpy(FileD, sPath);
	StringToChar(sPath, FileD);

	if ((fp = fopen(FileD, "r")) != NULL)
	{
		fseek(fp, 0, SEEK_END);
		nFileSize = ftell(fp);
		fseek(fp, 0, SEEK_SET);

		/* Allocate space for a path name */
		//FileData = (char*)malloc( nFileSize );
		FileData = (char*)calloc(nFileSize + 1, sizeof(char));

		nRSize = fread(FileData, sizeof(char), nFileSize, fp);
		//strFileData.Format(_T("%s"), CharToString(FileData));
		strFileData = CharToString(FileData);
		fclose(fp);
		free(FileData);
	}
	else
	{
		strFileData.Format(_T("PCR[Dn] ������ �������� �ʽ��ϴ�.\r\n%s"), sPath);
		pView->MsgBox(strFileData);
		//		AfxMessageBox(strFileData);
		return(2);
	}

	if (!pDoc->m_pPcr[1])
		return(2);
	if (!pDoc->m_pPcr[1][nIdx])
		return(2);

	pDoc->m_pPcr[1][nIdx]->m_nIdx = nIdx;							// m_nIdx : From 0 to nTot.....
	pDoc->m_pPcr[1][nIdx]->m_nSerial = nSerial;

	// Error Code													// 1(����), -1(Align Error, �뱤�ҷ�), -2(Lot End)
	nTemp = strFileData.Find(',', 0);
	strHeaderErrorInfo = strFileData.Left(nTemp);
	strFileData.Delete(0, nTemp + 1);
	nFileSize = nFileSize - nTemp - 1;
	pDoc->m_pPcr[1][nIdx]->m_nErrPnl = _tstoi(strHeaderErrorInfo);

	//if (m_pPcrInner[1][nIdx]->m_nErrPnl == -1)
	//{
	//	int syd = 1;
	//}


	// Model
	nTemp = strFileData.Find(',', 0);
	strModel = strFileData.Left(nTemp);
	strFileData.Delete(0, nTemp + 1);
	nFileSize = nFileSize - nTemp - 1;
	pDoc->m_pPcr[1][nIdx]->m_sModel = strModel;

	// Layer
	nTemp = strFileData.Find(',', 0);
	strLayer = strFileData.Left(nTemp);
	strFileData.Delete(0, nTemp + 1);
	nFileSize = nFileSize - nTemp - 1;
	pDoc->m_pPcr[1][nIdx]->m_sLayer = strLayer;

	if (pDoc->WorkingInfo.System.bUseITS)
	{
		// Lot
		nTemp = strFileData.Find(',', 0);
		strLot = strFileData.Left(nTemp);
		strFileData.Delete(0, nTemp + 1);
		nFileSize = nFileSize - nTemp - 1;
		pDoc->m_pPcr[1][nIdx]->m_sLot = strLot;

		// Its Code
		nTemp = strFileData.Find('\n', 0);
		sItsCode = strFileData.Left(nTemp);
		strFileData.Delete(0, nTemp + 1);
		nFileSize = nFileSize - nTemp - 1;
		pDoc->m_pPcr[1][nIdx]->m_sItsCode = sItsCode;
	}
	else
	{
		// Lot
		nTemp = strFileData.Find(',', 0);
		strLot = strFileData.Left(nTemp);
		strFileData.Delete(0, nTemp + 1);
		nFileSize = nFileSize - nTemp - 1;
		pDoc->m_pPcr[1][nIdx]->m_sLot = strLot;

		// Lot
		nTemp = strFileData.Find('\n', 0);
		strLot = strFileData.Left(nTemp);
		strFileData.Delete(0, nTemp + 1);
		nFileSize = nFileSize - nTemp - 1;
		pDoc->m_pPcr[1][nIdx]->m_sLot = strLot;
	}

	nTemp = strFileData.Find('\n', 0);
	strTotalBadPieceNum = strFileData.Left(nTemp);;
	strFileData.Delete(0, nTemp + 1);
	nFileSize = nFileSize - nTemp - 1;


	if (pDoc->GetTestMode() == MODE_INNER || pDoc->GetTestMode() == MODE_OUTER)
	{
		pDoc->m_sEngModel = strModel;
		pDoc->m_sEngLotNum = strLot;
		//pDoc->m_sEngLayerUp = strLayer;
		pDoc->m_sEngLayerDn = strLayer;
		if (pDoc->WorkingInfo.System.bUseITS)
			pDoc->m_sItsCode = sItsCode;
	}

	int nTotDef = _tstoi(strTotalBadPieceNum);

	pDoc->m_pPcr[1][nIdx]->Init(nSerial, nTotDef);

	if (nTotDef > 0)
	{
		for (i = 0; i < nTotDef; i++)
		{
			// Cam ID
			nTemp = strFileData.Find(',', 0);
			strCamID = strFileData.Left(nTemp);
			strFileData.Delete(0, nTemp + 1);
			nFileSize = nFileSize - nTemp - 1;
			pDoc->m_pPcr[1][nIdx]->m_nCamId = _tstoi(strCamID);

			// Piece Number
			nTemp = strFileData.Find(',', 0);
			strPieceID = strFileData.Left(nTemp);
			strFileData.Delete(0, nTemp + 1);
			nFileSize = nFileSize - nTemp - 1;

			// LoadStripPieceRegion_Binary()�� ���� PCS Index�� ������.
			if (pDoc->WorkingInfo.System.bStripPcsRgnBin)	// DTS��
			{
				pDoc->m_pPcr[1][nIdx]->m_pDefPcs[i] = _tstoi(strPieceID);

				//switch (pDoc->m_Master[1].MasterInfo.nActionCode)	// 0 : Rotation / Mirror ���� ����(CAM Data ����), 1 : �¿� �̷�, 2 : ���� �̷�, 3 : 180 ȸ��, 4 : 270 ȸ��(CCW), 5 : 90 ȸ��(CW)
				//{
				//case 0:
				//	pDoc->m_pPcr[1][nIdx]->m_pDefPcs[i] = _tstoi(strPieceID);
				//	break;
				//case 1:
				//	pDoc->m_pPcr[1][nIdx]->m_pDefPcs[i] = pDoc->MirrorLR(_tstoi(strPieceID));
				//	break;
				//case 3:
				//	pDoc->m_pPcr[1][nIdx]->m_pDefPcs[i] = pDoc->Rotate180(_tstoi(strPieceID));
				//	break;
				//default:
				//	pDoc->m_pPcr[1][nIdx]->m_pDefPcs[i] = _tstoi(strPieceID);
				//	break;
				//}
			}
			else
				pDoc->m_pPcr[1][nIdx]->m_pDefPcs[i] = pDoc->MirrorLR(_tstoi(strPieceID));	// �ʱ� ���˻���

			pDoc->m_pPcr[1][nIdx]->m_pLayer[i] = 1; // Dn

											  // BadPointPosX
			nTemp = strFileData.Find(',', 0);
			strBadPointPosX = strFileData.Left(nTemp);
			strFileData.Delete(0, nTemp + 1);
			nFileSize = nFileSize - nTemp - 1;
			pDoc->m_pPcr[1][nIdx]->m_pDefPos[i].x = (long)_tstoi(strBadPointPosX);

			// BadPointPosY
			nTemp = strFileData.Find(',', 0);
			strBadPointPosY = strFileData.Left(nTemp);
			strFileData.Delete(0, nTemp + 1);
			nFileSize = nFileSize - nTemp - 1;
			pDoc->m_pPcr[1][nIdx]->m_pDefPos[i].y = (long)_tstoi(strBadPointPosY);

			// BadName
			nTemp = strFileData.Find(',', 0);
			strBadName = strFileData.Left(nTemp);
			strFileData.Delete(0, nTemp + 1);
			nFileSize = nFileSize - nTemp - 1;
			pDoc->m_pPcr[1][nIdx]->m_pDefType[i] = _tstoi(strBadName);

			// Temp for ITS - m_pPcr[0][nIdx]->m_pDefPcs[i] = Rotate180(_tstoi(strPieceID));
			//pDoc->m_Master[0].m_pPcsRgn->GetMkMatrix(pDoc->Rotate180(pDoc->m_pPcr[1][nIdx]->m_pDefPcs[i]), nC, nR);
			pDoc->m_Master[0].m_pPcsRgn->GetMkMatrix(pDoc->m_pPcr[1][nIdx]->m_pDefPcs[i], nC, nR);
			pDoc->m_pPcr[1][nIdx]->m_arDefType[nR][nC] = pDoc->m_pPcr[1][nIdx]->m_pDefType[i];
			pDoc->m_pPcr[1][nIdx]->m_arPcrLineNum[nR][nC] = i;

			// CellNum
			nTemp = strFileData.Find(',', 0);
			strCellNum = strFileData.Left(nTemp);
			strFileData.Delete(0, nTemp + 1);
			nFileSize = nFileSize - nTemp - 1;
			pDoc->m_pPcr[1][nIdx]->m_pCell[i] = _tstoi(strCellNum);

			// ImageSize
			nTemp = strFileData.Find(',', 0);
			strImageSize = strFileData.Left(nTemp);
			strFileData.Delete(0, nTemp + 1);
			nFileSize = nFileSize - nTemp - 1;
			pDoc->m_pPcr[1][nIdx]->m_pImgSz[i] = _tstoi(strImageSize);

			// ImageNum
			nTemp = strFileData.Find(',', 0);
			strImageNum = strFileData.Left(nTemp);
			strFileData.Delete(0, nTemp + 1);
			nFileSize = nFileSize - nTemp - 1;
			pDoc->m_pPcr[1][nIdx]->m_pImg[i] = _tstoi(strImageNum);

			// strMarkingCode : -2 (NoMarking)
			nTemp = strFileData.Find('\n', 0);
			strMarkingCode = strFileData.Left(nTemp);
			strFileData.Delete(0, nTemp + 1);
			nFileSize = nFileSize - nTemp - 1;
			pDoc->m_pPcr[1][nIdx]->m_pMk[i] = _tstoi(strMarkingCode);

		}
	}

	return (1); // 1(����)
				//return(m_pPcr[1][nIdx]->m_nErrPnl);
}


void CDlgMenu05::OnBnClickedBtnSave5()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	//if (pView->IsRun() || pView->IsBuffer())
	//{
	//	pView->MsgBox(_T("�۾����Դϴ�."));
	//	return;
	//}

	if (m_sModel.IsEmpty())
	{
		pView->MsgBox(_T("Model���� �����ϴ�."));
		return;
	}
	else if(m_sLayerUp.IsEmpty())
	{
		pView->MsgBox(_T("Top�� ���̾���� �����ϴ�."));
		return;
	}
	else if(m_sLayerDn.IsEmpty())
	{
		pView->MsgBox(_T("Bottom�� ���̾���� �����ϴ�."));
	}

	if (IDYES == pView->MsgBox(_T("Reelmap�� �ٽ� �����Ͻðڽ��ϱ�?"), 0, MB_YESNO))
		pView->RemakeReelmapFromPcr(m_sModel, m_sLot, m_sLayerUp, m_sLayerDn);
}


void CDlgMenu05::OnStnClickedStcIts()
{
	// TODO: Add your control notification handler code here
	CString sPath = pDoc->WorkingInfo.System.sPathEngCurrInfo;

	myStcIts.SetBkColor(RGB_RED);
	myStcIts.RedrawWindow();

	CPoint pt;	CRect rt;
	GetDlgItem(IDC_STC_ITS)->GetWindowRect(&rt);
	pt.x = rt.right; pt.y = rt.bottom;
	ShowMyKeypad(IDC_STC_ITS, pt, TO_TOP | TO_RIGHT);
	//ShowKeypad(IDC_STC_PROC, pt, TO_BOTTOM|TO_RIGHT);

	myStcIts.SetBkColor(RGB_WHITE);
	myStcIts.RedrawWindow();

	CString sData;
	GetDlgItem(IDC_STC_ITS)->GetWindowText(sData);


	CString sMsg;

	//if (m_sModel.IsEmpty())
	//	pView->MsgBox(_T("�� ������ �����ϴ�."));

	//if (m_sLot.IsEmpty())
	//	pView->MsgBox(_T("��Ʈ ������ �����ϴ�."));

	if (sData.IsEmpty())
		pView->MsgBox(_T("ITS�ڵ� ������ �����ϴ�."));

	if (pDoc->m_sItsCode.Compare(sData)) // FALSE: Ideal Equal, TRUE: Different. 
	{
		sMsg.Format(_T("���� ITS�ڵ�: %s\r\n���ο� ITS�ڵ�: %s\r\n������ ITS�ڵ带 ���ο� ITS�ڵ�� �ٲٽðڽ��ϱ�?"), pDoc->m_sItsCode, sData);

		if (IDYES == pView->MsgBox(sMsg, 0, MB_YESNO))
		{
			pDoc->m_sItsCode = sData;
			::WritePrivateProfileString(_T("Infomation"), _T("Its Code"), pDoc->m_sItsCode, sPath);
		}
		else
			myStcIts.SetText(pDoc->m_sItsCode);
	}
}

void CDlgMenu05::DispItsCode()
{
	myStcIts.SetText(GetItsCode());
}

CString CDlgMenu05::GetItsCode()
{
	CString str;
	//char szData[MAX_PATH];
	TCHAR szData[MAX_PATH];

	if (0 < ::GetPrivateProfileString(_T("Infomation"), _T("Its Code"), NULL, szData, sizeof(szData), pDoc->WorkingInfo.System.sPathEngCurrInfo))
		str = CString(szData);
	else
		str = _T("");

	return str;
}
