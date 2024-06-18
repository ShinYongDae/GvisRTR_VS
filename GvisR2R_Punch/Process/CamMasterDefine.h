#pragma once

// Allocation for HeasungDS RTR  ========================================================================================
#define RGN_STRIP_VARIABLE_NUM	23		// REGION_STRIP 구조체에서 배열을 제외한 변수의 갯수
#define MAX_STRIP				4
#define MAX_NodeNumX			100		//200
#define MAX_NodeNumY			100		//200
#define MAX_NMSWATH				100

#define MAX_PCS					10000

#define MAX_PIECE_NODE_X_NUM	100		//600		
#define MAX_PIECE_NODE_Y_NUM	100		//600

#define MAX_FRAME_RGN_NUM		100//4		//1000		// Maximum number of Strip regions
#define MAX_STRIP_X_NUM			10//1		//10			
#define MAX_STRIP_Y_NUM			10//4		//10			
#define MAX_PIECE_RGN_NUM		(MAX_PIECE_NODE_X_NUM * MAX_PIECE_NODE_Y_NUM)		// Maximum number of Cell regions
//=======================================================================================================================

struct stMasterInfo
{
	double dPixelSize;							// [MACHINE] PixelSize [um]
	CString strMasterLocation, strCADImgPath, strCADImgBackUpPath, strTwoMetalOppLayer;
	int nImageCompression, nNumOfAlignPoint;	// [SPEC] NumOfAlignPoint
	BOOL bTwoMetalInspection;
	double dTestRgnW, dTestRgnH;				// [PANEL INFO] InspectionWidth, InspectionHeight [mm]
	double dTestRgnLeft, dTestRgnTop;			// [ORIGIN COORD] MX, MY [mm]
	double dPinPosX, dPinPosY;					// [ORIGIN COORD] PX, PY [mm]
	int nActionCode, nOutFileOnAoi;

	stMasterInfo()
	{
		dPixelSize = 0.0;
		strMasterLocation = _T(""); strCADImgPath = _T(""); strCADImgBackUpPath = _T(""); strTwoMetalOppLayer = _T("");
		nImageCompression = 0; nNumOfAlignPoint = 0;
		bTwoMetalInspection = FALSE;
		dTestRgnW = 0.0; dTestRgnH = 0.0;				// [PANEL INFO] InspectionWidth, InspectionHeight [mm]
		dTestRgnLeft = 0.0; dTestRgnTop = 0.0;			// [ORIGIN COORD] MX, MY [mm]
		dPinPosX = 0.0; dPinPosY = 0.0;					// [ORIGIN COORD] PX, PY [mm]
		nActionCode = 0; // 0 : Rotation / Mirror 적용 없음(CAM Data 원본), 1 : 좌우 미러, 2 : 상하 미러, 3 : 180 회전, 4 : 270 회전(CCW), 5 : 90 회전(CW)
		nOutFileOnAoi = -1;	// -1 : AOI 설정 없음, 0 : Old , 1 : New
	}
};

typedef struct {
	int nMode;											// 0 : Full Mode, 1 : Strip Mode
	int nMSwath;										// 모터가 움직일 회수.
	int NodeNumX, NodeNumY;								// 각 축당 셀 갯수
	int PanelPixelX, PanelPixelY;						// 전체 판넬 이미지의 크기
	int OvrXPix, OvrYPix;								// 각 축으로 오버랩되는 부분의 크기
	int ProcSizeX, ProcSizeY;							// 각 축당 셀의 크기
	int nCell;											// 전체 셀 갯수
	int nIPU;											// AOI의 IPU 갯수
	int CameraValidPixel;								// 실제 검사하기 위해 사용되는 이미지의 X축 크기
	int nScanMarginX, nScanMarginY;						// 실제 사용되는 이미지에 각 축방향으로 더해지는 여분 이미지의 크기 Ex) nScanMarginX = (8192 ? CameraValidPixel)/2
	int nRegionX, nRegionY;								// Reserved Data
	int nCellDivideX, nCellDivideY;						// 한 스트립당 각 축 방향 셀 갯수
	int nCADPinPosPixX, nCADPinPosPixY;					// Pin Position X, y
	int nProcMarginX, nProcMarginY;						// Reserved Data
	int StPosX[MAX_NodeNumX * MAX_NodeNumY];			// 각 셀의 왼쪽 윗부분의 X 위치
	int StPosY[MAX_NodeNumX * MAX_NodeNumY];			// 각 셀의 왼쪽 윗부분의 Y 위치
	int EdPosX[MAX_NodeNumX * MAX_NodeNumY];			// 각 셀의 오른쪽 아래부분의 X 위치
	int EdPosY[MAX_NodeNumX * MAX_NodeNumY];			// 각 셀의 오른쪽 아래부분의 Y 위치
	int XSwathPixPos[MAX_NMSWATH];						// 모터가 움직이기 시작하는 위치
} REGION_STRIP;		//Total 100476 Byte

struct stAlignMark2
{
	float X0, Y0;
	float X1, Y1;
	float X2, Y2;
	float X3, Y3;

	stAlignMark2()
	{
		X0 = 0.0; Y0 = 0.0;
		X1 = 0.0; Y1 = 0.0;
		X2 = 0.0; Y2 = 0.0;
		X3 = 0.0; Y3 = 0.0;
	}
};

struct stAlignMark
{
	float X0, Y0;
	float X1, Y1;

	stAlignMark()
	{
		X0 = 0.0; Y0 = 0.0;
		X1 = 0.0; Y1 = 0.0;
	}
};

struct stPieceMark
{
	float X, Y;

	stPieceMark()
	{
		X = 0.0; Y = 0.0;
	}
};

typedef struct {
	int iStartX, iStartY;
	int iEndX, iEndY;
	int FMirror;	//0 : 원본 1 : 상하미러  2 : 좌퓖E肩?
	int FRotate;	//0 : 0도  1 : 90도  2 : 180도  3 : 270도
} REGIONS_FRAME;


typedef struct {
	int nId;
	int Col;
	int Row;
} REGIONS_FRAME_ID;

//typedef struct {
//	int nWidth;
//	int nHeight;
//} REGIONS_PIECE_ID;


// for DTS
struct _Point {
	double dX, dY;

	_Point()
	{
		dX = 0.0; dY = 0.0;
	}
};

struct _Rect {
	double dLeft, dTop, dRight, dBottom;

	_Rect()
	{
		dLeft = 0.0; dTop = 0.0; dRight = 0.0; dBottom = 0.0;
	}
};

struct _MasterPiece {
	int nMstPcsIdx, nMstStripIdx;
	int nMstStripRow, nMstStripCol;
	int nMstPcsRow, nMstPcsCol;
	_Point MkPos;
	_Rect Area;

	_MasterPiece()
	{
		nMstPcsIdx = -1; nMstStripIdx = -1;		// from 0 ~
		nMstStripRow = -1; nMstStripCol = -1;	// from 0 ~
		nMstPcsRow = -1; nMstPcsCol = -1;		// from 0 ~
		MkPos.dX = 0.0; MkPos.dY = 0.0;			// [mm]
		Area.dLeft = 0.0; Area.dTop = 0.0; Area.dRight = 0.0; Area.dBottom = 0.0; // [mm]
	}
};

struct _MasterStrip {
	int nMstStripIdx;
	int nTotalPiece;
	int nTotalPieceRow, nTotalPieceCol;
	_Rect Area;
	_MasterPiece Piece[MAX_NodeNumY][MAX_NodeNumX]; // Piece[Row][Col] 

	_MasterStrip()
	{
		nMstStripIdx = -1;
		nTotalPiece = 0;
		nTotalPieceRow = 0; nTotalPieceCol = 0;
		Area.dLeft = 0.0; Area.dTop = 0.0; Area.dRight = 0.0; Area.dBottom = 0.0; // [mm]
	}
};

struct _MasterPanel {
	int nTotalStrip; // Strip Index : 0 ~ (TotalStrip-1)
	int nTotalStripRow, nTotalStripCol;
	int nTotalPiece, nPcsCorner;
	int nTotalAlignPos;
	_Point AlignPos[4];
	_Point PinPos;
	_Rect Area;
	CString sPathPinImg, sPathPcsImg, sPathAlignImg[4]; // * CadlinkImg는 검사용 Cell Image임.
	CString sPath2ptAlignAndMkPos, sPath4ptAlignAndMkPos;
	CString sPathPinPos;								// [ORIGIN COORD] (원점): PX, PY ; (검사영역 좌상): MX, MY
														// [PANEL INFO] (검사영역 W, H): InspectionWidth, InspectionHeight

	_MasterStrip Strip[MAX_STRIP];	// Strip[Row] - CamMaster의 Row, Col 순으로
	_MasterPiece Piece[MAX_PCS];	// CamMaster의 인덱스 순으로

	_MasterPanel()
	{
		nTotalStrip = 0; // Strip Index : 0 ~ (TotalStrip-1)
		nTotalStripRow = 0; nTotalStripCol = 0;
		nTotalPiece = 0;	nPcsCorner = 0;
		nTotalAlignPos = 0;
		AlignPos[0].dX = 0.0; AlignPos[1].dX = 0.0; AlignPos[2].dX = 0.0; AlignPos[3].dX = 0.0; // [mm]
		AlignPos[0].dY = 0.0; AlignPos[1].dY = 0.0; AlignPos[2].dY = 0.0; AlignPos[3].dY = 0.0; // [mm]
		PinPos.dX = 0.0; PinPos.dY = 0.0; // [mm]
		Area.dLeft = 0.0; Area.dTop = 0.0; Area.dRight = 0.0; Area.dBottom = 0.0; // [mm]
		sPathPinImg = _T(""); sPathPcsImg = _T("");
		sPathAlignImg[0] = _T(""); sPathAlignImg[1] = _T(""); sPathAlignImg[2] = _T(""); sPathAlignImg[3] = _T("");
		sPath2ptAlignAndMkPos = _T(""); sPath4ptAlignAndMkPos = _T(""); sPathPinPos = _T("");
	}
};


// DlgCammasterinfoMst.h : 헤더 파일        

#ifndef RGN_STRIP_VARIABLE_NUM
#define RGN_STRIP_VARIABLE_NUM	23		// REGION_STRIP 구조체에서 배열을 제외한 변수의 갯수
#endif

#ifndef MAX_NodeNumX
#define MAX_NodeNumX			200
#endif

#ifndef MAX_NodeNumY
#define MAX_NodeNumY			200
#endif

#ifndef MAX_NMSWATH
#define MAX_NMSWATH				100
#endif

//#ifndef REGION_STRIP
//typedef struct {
//	int nMode;											// 0 : Full Mode, 1 : Strip Mode
//	int nMSwath;										// 모터가 움직일 회수.
//	int NodeNumX, NodeNumY;								// 각 축당 셀 갯수
//	int PanelPixelX, PanelPixelY;						// 전체 판넬 이미지의 크기
//	int OvrXPix, OvrYPix;								// 각 축으로 오버랩되는 부분의 크기
//	int ProcSizeX, ProcSizeY;							// 각 축당 셀의 크기
//	int nCell;											// 전체 셀 갯수
//	int nIPU;											// AOI의 IPU 갯수
//	int CameraValidPixel;								// 실제 검사하기 위해 사용되는 이미지의 X축 크기
//	int nScanMarginX, nScanMarginY;						// 실제 사용되는 이미지에 각 축방향으로 더해지는 여분 이미지의 크기 Ex) nScanMarginX = (8192 ? CameraValidPixel)/2
//	int nRegionX, nRegionY;								// Reserved Data
//	int nCellDivideX, nCellDivideY;						// 한 스트립당 각 축 방향 셀 갯수
//	int nCADPinPosPixX, nCADPinPosPixY;					// Pin Position X, y
//	int nProcMarginX, nProcMarginY;						// Reserved Data
//	int StPosX[MAX_NodeNumX * MAX_NodeNumY];			// 각 셀의 왼쪽 윗부분의 X 위치
//	int StPosY[MAX_NodeNumX * MAX_NodeNumY];			// 각 셀의 왼쪽 윗부분의 Y 위치
//	int EdPosX[MAX_NodeNumX * MAX_NodeNumY];			// 각 셀의 오른쪽 아래부분의 X 위치
//	int EdPosY[MAX_NodeNumX * MAX_NodeNumY];			// 각 셀의 오른쪽 아래부분의 Y 위치
//	int XSwathPixPos[MAX_NMSWATH];						// 모터가 움직이기 시작하는 위치
//} REGION_STRIP;		//Total 100476 Byte
//#endif

//-------------


// DlgCammasterinfoPch.h : 헤더 파일

#ifndef MAX_PCS
#define MAX_PCS 10000
#endif

//#ifndef stAlignMark2
//struct stAlignMark2
//{
//	float X0, Y0;
//	float X1, Y1;
//	float X2, Y2;
//	float X3, Y3;
//
//	stAlignMark2()
//	{
//		X0 = 0.0; Y0 = 0.0;
//		X1 = 0.0; Y1 = 0.0;
//		X2 = 0.0; Y2 = 0.0;
//		X3 = 0.0; Y3 = 0.0;
//	}
//};
//#endif

//#ifndef stAlignMark
//struct stAlignMark
//{
//	float X0, Y0;
//	float X1, Y1;
//
//	stAlignMark()
//	{
//		X0 = 0.0; Y0 = 0.0;
//		X1 = 0.0; Y1 = 0.0;
//	}
//};
//#endif

//#ifndef stPieceMark
//struct stPieceMark
//{
//	float X, Y;
//
//	stPieceMark()
//	{
//		X = 0.0; Y = 0.0;
//	}
//};
//#endif

//--------------

// DlgCammasterinfoStrpcs.h : 헤더 파일

#ifndef MAX_PIECE_NODE_X_NUM
#define MAX_PIECE_NODE_X_NUM	600		// 130319 jsy edit
#endif

#ifndef MAX_PIECE_NODE_Y_NUM
#define MAX_PIECE_NODE_Y_NUM	600
#endif

#ifndef MAX_FRAME_RGN_NUM
#define MAX_FRAME_RGN_NUM		1000		// Maximum number of Strip regions
#endif

#ifndef MAX_STRIP_X_NUM
#define MAX_STRIP_X_NUM			10			//2012.08.07 hyk
#endif

#ifndef MAX_STRIP_Y_NUM
#define MAX_STRIP_Y_NUM			10			//2012.08.07 hyk
#endif

#ifndef MAX_PIECE_RGN_NUM
#define MAX_PIECE_RGN_NUM		(MAX_PIECE_NODE_X_NUM * MAX_PIECE_NODE_Y_NUM)		// Maximum number of Cell regions
#endif

//#ifndef REGIONS_FRAME
//typedef struct {
//	int iStartX, iStartY;
//	int iEndX, iEndY;
//	int FMirror;	//0 : 원본 1 : 상하미러  2 : 좌우미러
//	int FRotate;	//0 : 0도  1 : 90도  2 : 180도  3 : 270도
//} REGIONS_FRAME;
//#endif


//#ifndef REGIONS_FRAME_ID
//typedef struct {
//	int nId;
//	int Col;
//	int Row;
//} REGIONS_FRAME_ID;
//#endif

//#ifndef REGIONS_PIECE
//typedef struct
//{
//	int iStartX, iStartY;
//	int iEndX, iEndY;
//	int FMirror;	//0 : Defult 1 : Up to Down Mirroring  2 : Left to Right Mirroring
//	int FRotate;	//0 : 0  1 : 90  2 : 180  3 : 270 [Degree]
//} REGIONS_PIECE;
//#endif

//#ifndef REGIONS_PIECE_2
//typedef struct {
//	int nId;
//	int iStartX, iStartY;
//	int iEndX, iEndY;
//	int FMirror;	//0 : 원본 1 : 상하미러  2 : 좌퓖E肩?
//	int FRotate;	//0 : 0도  1 : 90도  2 : 180도  3 : 270도
//	int Row;
//	int Col;
//} REGIONS_PIECE_2;
//#endif


// for DTS

#ifndef PATH_ORDERING_Mk
#define PATH_ORDERING_Mk	_T("C:\\R2RSet\\Test\\WriteOrederingMkRotate90ccw.txt")
#endif

#ifndef MAX_PCE_ROW
#define MAX_PCE_ROW			100
#endif

#ifndef MAX_PCE_COL
#define MAX_PCE_COL			100
#endif

#ifndef MAX_STRIP
#define MAX_STRIP			4
#endif

#ifndef MAX_STRIP_NUM
#define MAX_STRIP_NUM		4
#endif

#ifndef MAX_PCS
#define MAX_PCS				10000
#endif

#ifndef MAX_NodeNumY
#define MAX_NodeNumY		100
#endif

#ifndef MAX_NodeNumX
#define MAX_NodeNumX		100
#endif

//#ifndef _Point
//struct _Point {
//	double dX, dY;
//
//	_Point()
//	{
//		dX = 0.0; dY = 0.0;
//	}
//};
//#endif

//#ifndef _Rect
//struct _Rect {
//	double dLeft, dTop, dRight, dBottom;
//
//	_Rect()
//	{
//		dLeft = 0.0; dTop = 0.0; dRight = 0.0; dBottom = 0.0;
//	}
//};
//#endif

//#ifndef _MasterPiece
//struct _MasterPiece {
//	int nMstPcsIdx, nMstStripIdx;
//	int nMstStripRow, nMstStripCol;
//	int nMstPcsRow, nMstPcsCol;
//	_Point MkPos;
//	_Rect Area;
//
//	_MasterPiece()
//	{
//		nMstPcsIdx = -1; nMstStripIdx = -1;		// from 0 ~
//		nMstStripRow = -1; nMstStripCol = -1;	// from 0 ~
//		nMstPcsRow = -1; nMstPcsCol = -1;		// from 0 ~
//		MkPos.dX = 0.0; MkPos.dY = 0.0;			// [mm]
//		Area.dLeft = 0.0; Area.dTop = 0.0; Area.dRight = 0.0; Area.dBottom = 0.0; // [mm]
//	}
//};
//#endif

//#ifndef _MasterStrip
//struct _MasterStrip {
//	int nMstStripIdx;
//	int nTotalPiece;
//	int nTotalPieceRow, nTotalPieceCol;
//	_Rect Area;
//	_MasterPiece Piece[MAX_NodeNumY][MAX_NodeNumX]; // Piece[Row][Col] 
//
//	_MasterStrip()
//	{
//		nMstStripIdx = -1;
//		nTotalPiece = 0;
//		nTotalPieceRow = 0; nTotalPieceCol = 0;
//		Area.dLeft = 0.0; Area.dTop = 0.0; Area.dRight = 0.0; Area.dBottom = 0.0; // [mm]
//	}
//};
//#endif

//#ifndef _MasterPanel
//struct _MasterPanel {
//	int nTotalStrip; // Strip Index : 0 ~ (TotalStrip-1)
//	int nTotalStripRow, nTotalStripCol;
//	int nTotalPiece, nPcsCorner;
//	int nTotalAlignPos;
//	_Point AlignPos[4];
//	_Point PinPos;
//	_Rect Area;
//	CString sPathPinImg, sPathPcsImg, sPathAlignImg[4]; // * CadlinkImg는 검사용 Cell Image임.
//	CString sPath2ptAlignAndMkPos, sPath4ptAlignAndMkPos;
//	CString sPathPinPos;								// [ORIGIN COORD] (원점): PX, PY ; (검사영역 좌상): MX, MY
//														// [PANEL INFO] (검사영역 W, H): InspectionWidth, InspectionHeight
//
//	_MasterStrip Strip[MAX_STRIP];	// Strip[Row] - CamMaster의 Row, Col 순으로
//	_MasterPiece Piece[MAX_PCS];	// CamMaster의 인덱스 순으로
//
//	_MasterPanel()
//	{
//		nTotalStrip = 0; // Strip Index : 0 ~ (TotalStrip-1)
//		nTotalStripRow = 0; nTotalStripCol = 0;
//		nTotalPiece = 0;	nPcsCorner = 0;
//		nTotalAlignPos = 0;
//		AlignPos[0].dX = 0.0; AlignPos[1].dX = 0.0; AlignPos[2].dX = 0.0; AlignPos[3].dX = 0.0; // [mm]
//		AlignPos[0].dY = 0.0; AlignPos[1].dY = 0.0; AlignPos[2].dY = 0.0; AlignPos[3].dY = 0.0; // [mm]
//		PinPos.dX = 0.0; PinPos.dY = 0.0; // [mm]
//		Area.dLeft = 0.0; Area.dTop = 0.0; Area.dRight = 0.0; Area.dBottom = 0.0; // [mm]
//		sPathPinImg = _T(""); sPathPcsImg = _T("");
//		sPathAlignImg[0] = _T(""); sPathAlignImg[1] = _T(""); sPathAlignImg[2] = _T(""); sPathAlignImg[3] = _T("");
//		sPath2ptAlignAndMkPos = _T(""); sPath4ptAlignAndMkPos = _T(""); sPathPinPos = _T("");
//	}
//};
//#endif


//--------------