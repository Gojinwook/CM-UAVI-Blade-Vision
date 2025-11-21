#ifndef _GTREGION_H_
#define _GTREGION_H_

#include "PBC.h"
#include "GViewportManager.h"
#include "PList.hpp"
#include "halconcpp.h"
#include "CalDataService.h"

//Region Style
#define GTR_RS_FILL					1
#define GTR_RS_NULL					2

// Draw Style
#define GTR_DS_NORMAL				0
#define GTR_DS_ACTIVE				1
#define GTR_DS_SELECTPART			2

// Control Point Half Length
#define GTR_CPHL					5

// Selection Point Half Length
#define GTR_SPHL					3

// Control Point Index
#define GTR_CP_NONE					-3
#define GTR_CP_SEGMENT				-2
#define GTR_CP_REGION				-1
#define GTR_CP_LT					0
#define GTR_CP_T					1
#define GTR_CP_RT					2
#define GTR_CP_R					3
#define GTR_CP_RB					4
#define GTR_CP_B					5
#define GTR_CP_LB					6
#define GTR_CP_L					7
#define GTR_CP_KEY_LEFT						8
#define GTR_CP_KEY_RIGHT					9
#define GTR_CP_KEY_UP						10
#define GTR_CP_KEY_DOWN						11
#define GTR_CP_CKEY_LEFT					12
#define GTR_CP_CKEY_RIGHT					13
#define GTR_CP_CKEY_UP						14
#define GTR_CP_CKEY_DOWN					15
#define GTR_CP_SKEY_LEFT					16
#define GTR_CP_SKEY_RIGHT					17
#define GTR_CP_SKEY_UP						18
#define GTR_CP_SKEY_DOWN					19

// Rotation Point Position
#define RPP_LEFT_TOP		0
#define RPP_RIGHT_TOP		1
#define RPP_RIGHT_BOTTOM	2
#define RPP_LEFT_BOTTOM		3

// Rotation Margin Type
#define RMT_NO_MARGIN		0
#define RMT_OUTER_MARGIN	1
#define RMT_INNER_MARGIN	2

class GTRegion : public PObject	// dynclass때문에 PObject에서 파생되었다.
{
	DECLARE_DYNCLASS(GTRegion)

public:
	GTRegion();
	virtual ~GTRegion();
	
protected:
	BOOL mbVisible;
	BOOL mbSelectable;
	BOOL mbMovable;
	BOOL mbSizable;
	BOOL mbSelect;

	DPOINT maPoints[2];
	DPOINT mOrgLTPointM;
	DPOINT mOrgRBPointM;

	void Reset();

	BOOL PtInCPoint(int iVX, int iVY, POINT VPoint);
	BOOL PtInSegment(POINT VPoint, GViewportManager *pVManager, CCalDataService *pCalDataService);

	GTRegion		*mpParent;
	PList<GTRegion> *m_pChildTRegionList;

public:
	int		m_iTeachImageIndex;	// (영상탭: 1~16), 기본정보:0, Surface:1, Edge:2 .....
	int		m_iInspectionType;	// Align, Inspection
	BOOL	m_bRegionROI;
	Hobject m_HTeachPolygonRgn;

	int m_iLocalAlignID;		// 24.02.29 Local Align 추가 - LeeGW
	int m_iFAIOuterCircleID;
	int m_iFAIInnerCircleID;
	int m_iFAIDatumCircleID;
	int m_iFAIDatumEllipseID;
	int m_iFAIWeldingSpotID;
	int m_iFAIWeldingPocketID;
	int m_iFAIBracketID;

	
	BOOL mbLastSelected;

	int		miPadID;
	BOOL	mbGerberSelected;
	BOOL	mbRealPad;
	BOOL	mbShape;	// Added by Gojw	2005/11/25	FALSE:Rectangle, TRUE:Circle

	int			miLineStyle; 
	int			miLineThickness;
	COLORREF	mLineColor;
	COLORREF	mBackColor;
	int			miBkMode;
	int			miRegionStyle;
	COLORREF	mRegionColor;
	
		// Align Info   티칭에서 얻은 정보를 이요해 Dont care 영역을 검사에 적용.
	double	mdAlignCenterX;
	double	mdAlignCenterY;
	double	mdAlignAngle;
	double	mdAlignScale;
	double	mdAlignScore;

	Hobject m_xld_data;
	Hobject m_CopyDontCare;

	virtual BOOL Load(HANDLE hFile, int iModelVersion);
	virtual void Save(HANDLE hFile);

	virtual void Duplicate(GTRegion **ppTRegion);
	virtual GTRegion& operator=(GTRegion& data);
	
	void GetLTPoint(POINT *pIPoint, CCalDataService *pCalDataService);
	void GetRBPoint(POINT *pIPoint, CCalDataService *pCalDataService, int iImageWidth = -1, int iImageHeight = -1);	//이미지 사이즈를 고려하려면 iImageWidth, iImageHeight를 입력해준다. iImageWidth, iImageHeight를 벗어나면 최대값으로 대치해준다. 입력하지 않으면 이미지 사이즈를 고려하지 않은 방식(이전방식)대로 돌아간다
	void GetWHPoint(POINT *pIPoint, CCalDataService *pCalDataService);
	void GetCenterPoint(POINT *pIPoint, CCalDataService *pCalDataService);
	void SetLTPoint(POINT Point, CCalDataService *pCalDataService);
	void SetRBPoint(POINT Point, CCalDataService *pCalDataService);

	void GetLTPointD(DPOINT *pIPoint, CCalDataService *pCalDataService);
	void GetRBPointD(DPOINT *pIPoint, CCalDataService *pCalDataService, int iImageWidth = -1, int iImageHeight = -1);	//이미지 사이즈를 고려하려면 iImageWidth, iImageHeight를 입력해준다. iImageWidth, iImageHeight를 벗어나면 최대값으로 대치해준다. 입력하지 않으면 이미지 사이즈를 고려하지 않은 방식(이전방식)대로 돌아간다
	void GetWHPointD(DPOINT *pIPoint, CCalDataService *pCalDataService);
	void GetCenterPointD(DPOINT *pIPoint, CCalDataService *pCalDataService);
	void SetLTPointD(DPOINT DPoint, CCalDataService *pCalDataService);
	void SetRBPointD(DPOINT DPoint, CCalDataService *pCalDataService);

	void GetLTPointM(DPOINT *pMPoint);
	void GetRBPointM(DPOINT *pMPoint);
	void GetWHPointM(DPOINT *pWHPoint);
	void GetCenterPointM(DPOINT *pMCPoint);
	void SetLTPointM(DPOINT MPoint);
	void SetRBPointM(DPOINT MPoint);
	void SetCenterPointM(DPOINT CenterPoint);

	void ArrangePoints();
	void UpdatePolygonROIBoundary();

	int GetChildTRegionCount();
	void AddChildTRegion(GTRegion *pTRegion);
	void InsertChildTRegion(int iIndex, GTRegion *pTRegion);
	void DeleteChildTRegion(int iIndex);
	void DeleteChildTRegionPtr(GTRegion *pTRegion, BOOL bChildLink);
	void ClearAllChildTRegion();
	void ClearAllSelectChildTRegion();
	void SwapChildTRegion(int a, int b);
	GTRegion *GetChildTRegion(int iIndex);

	BOOL RgnInRegion(GTRegion *pTRegion);
	BOOL PtInRegion(POINT VPoint, GViewportManager *pVManager, CCalDataService *pCalDataService);
	void DrawControlPoint(HDC hDestDC, int iVX, int iVY);
	void DrawSelectionPoint(HDC hDestDC, int iVX, int iVY);

	BOOL GetVisible();
	BOOL GetSelectable();
	BOOL GetMovable();
	BOOL GetSizable();
	BOOL GetSelect();
	BOOL GetShape();		// Added by Gojw	2005/11/25

	void SetLineColor(COLORREF lineColor);				// GJW (2005/6/20)
	void SetVisible(BOOL bVisible, BOOL bChildLink);
	void SetSelectable(BOOL bSelectable, BOOL bChildLink);
	void SetMovable(BOOL bMovable, BOOL bChildLink);
	void SetSizable(BOOL bSizable, BOOL bChildLink);
	void SetSelect(BOOL bSelect, BOOL bChildLink);
	void SetShape(BOOL bShape, BOOL bChildLink);		// Added by Gojw	2005/11/25
	void ClearAllSelect();
	void ClearAllLastSelected();

	void SetOrgLTPointM(DPOINT MPoint);
	void SetOrgRBPointM(DPOINT MPoint);

	Hobject GetROIHRegion(CCalDataService *pCalDataService);

	virtual GTRegion *GetTopTRegion(POINT VPoint, GViewportManager *pVManager, CCalDataService *pCalDataService, int iCurTeachingTabIndex=-1);
	virtual GTRegion *GetTopTRegionArea(POINT VPoint, GViewportManager *pVManager, CCalDataService *pCalDataService, int iCurTeachingTabIndex, long *plArea);

	virtual int GetCPointIndex(POINT VPoint, GViewportManager *pVManager, CCalDataService *pCalDataService);
	virtual void SetCPoint(int iCPointIndex, POINT *pMIPoints, CCalDataService *pCalDataService);

	virtual void Move(double dXMove, double dYMove, BOOL bChildLink, CCalDataService *pCalDataService);

	// Drawing Functions
	virtual void Draw(HDC hDestDC, GViewportManager *pVManager, int iDrawStyle, BOOL bDrawControlPoint, CCalDataService *pCalDataService, int iCurTeachingTabIndex=-1);

	//cheol add

	Hobject m_xld_land_inspect;		// 검사 데이터 ( for land alignment ) , not need to save
	int		miXldPointNum;

	// Gerber Real Data
	double	mdGerberSizeX;
	double	mdGerberSizeY;
	double	mdGerberArea;
	double	mdGerberCenterX;		// 20081222	Gojw, Offset btw Centers
	double	mdGerberCenterY;		// 20081222 Gojw, Offset btw Centers
	double  mdAvgSolderArea;		// 20070820 Gojw

	void	SetGerberWidth(double dWidth) { mdGerberSizeX = dWidth; }
	void	SetGerberHeight(double dHeight) { mdGerberSizeY = dHeight; }
	void	SetGerberArea(double dArea) { mdGerberArea = dArea; }
	void	SetGerberCenterX(double dCenterX) { mdGerberCenterX = dCenterX; }
	void	SetGerberCenterY(double dCenterY) { mdGerberCenterY = dCenterY; }

	CAlgorithmParam m_AlgorithmParam[MAX_ROI_TEACHING_TAB];

	// Local Align Result
	void ResetLocalAlignResult(int iThreadIdx);
	Hobject m_HLocalAlignShapeRgn[MAX_MODULE_ONE_TRAY];
	int	m_iLocalAlignDeltaX[MAX_MODULE_ONE_TRAY];
	int	m_iLocalAlignDeltaY[MAX_MODULE_ONE_TRAY];
	double	m_dLocalAlignDeltaAngle[MAX_MODULE_ONE_TRAY];
	double	m_dLocalAlignDeltaAngleFixedPointX[MAX_MODULE_ONE_TRAY];
	double	m_dLocalAlignDeltaAngleFixedPointY[MAX_MODULE_ONE_TRAY];
	int m_iLocalAlignLineFitXPos[MAX_MODULE_ONE_TRAY];
	int m_iLocalAlignLineFitYPos[MAX_MODULE_ONE_TRAY];
	int m_iLocalAlignLineFitXPos2[MAX_MODULE_ONE_TRAY];
	int m_iLocalAlignLineFitYPos2[MAX_MODULE_ONE_TRAY];
	double m_dLocalAlignAngleFitXPos[MAX_MODULE_ONE_TRAY];
	double m_dLocalAlignAngleFitYPos[MAX_MODULE_ONE_TRAY];
	double m_dLocalAlignAngleFitAngle[MAX_MODULE_ONE_TRAY];

	double m_dEdgeCenterX[MAX_MODULE_ONE_TRAY];
	double m_dEdgeCenterY[MAX_MODULE_ONE_TRAY];
	double m_dLineStartX[MAX_MODULE_ONE_TRAY];
	double m_dLineStartY[MAX_MODULE_ONE_TRAY];
	double m_dLineEndX[MAX_MODULE_ONE_TRAY];
	double m_dLineEndY[MAX_MODULE_ONE_TRAY];

	double m_dEdgeStrength[MAX_MODULE_ONE_TRAY]; // 241120 PSW

	// Generate Rgn
	Hobject m_HGenerateResultRgn;


	//// Part Check
	HTuple m_HPartModelID;

	//// Aniso Align
	HTuple m_HAnisoModelID;


	// 240227 Local Align 추가 - LeeGW End
};

#endif
