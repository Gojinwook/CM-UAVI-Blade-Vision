
#ifndef CVIEW1_H
#define CVIEW1_H
//////////////////// 헤더 안 내용을 쓴다.

#pragma once

#define ROI_KEY_UP		0
#define ROI_KEY_DOWN	1
#define ROI_KEY_LEFT	2
#define ROI_KEY_RIGHT	3

#include "GViewPortManager.h"
#include "GTRegion.h"
#include "SelectPartRegion.h"
#include "ToolBarCombo.h"
#include "InspectionTypeDlg.h"

// InspectViewDlg1 대화 상자입니다.
#define RECTWIDTH(lpRect)	((lpRect)->right - (lpRect)->left)
#define RECTHEIGHT(lpRect)	((lpRect)->bottom - (lpRect)->top)

class CInspectAdminViewDlg : public CDialog
{
	private:
	
	CRect	m_ScreenRect;
	static CInspectAdminViewDlg	*m_pInstance;

public:
	static	CInspectAdminViewDlg* GetInstance(BOOL bShowFlag=FALSE);
	void	DeleteInstance();

	void UpdateView(BOOL bUpdate=TRUE);
	void ClearUpdateView(BOOL bUpdate=TRUE);

	void StartLive();
	void StopLive();
	
	typedef enum { TS_DRAW=0, TS_MOVE, TS_SELECT_PART, NC_TS_EDIT_XLD, TS_CREATE_AF_ROI, NC_TS_EDIT_POLYGON_XLD, TS_CREATE_ROI_RECT, TS_CREATE_ROI_CIRCLE, TS_CREATE_ROI_POLYGON } eTOOLBAR_STATE;
	void	SetPosition(int left, int top, int right, int bottom) { m_ScreenRect = CRect(left, top, right, bottom); }
// Construction
public:
	CInspectAdminViewDlg(CWnd* pParent = NULL);   // standard constructor

	void SetXLDEditMode(int iContourType);
	void SetPolygonXLDEditMode(int iContourType);

	// Polygon
	BOOL m_bPolygonMode;

	int m_iPolygonMode;
	int m_iPolygonCnt;
	Hobject mHUnionPolygonRgn;
	POINT m_PolygonPt[100];
	void DiffPolygonROI();
	void AddPolygonROI();

	BOOL	m_bOnPaintNow;

	BOOL m_bInspectRunDisplay;

	BOOL m_bTestThread;


	int StartTime1,EndTime1;

	Hobject pModuleGrabImage[MAX_MODULE_ONE_TRAY][MAX_IMAGE_TAB];
	Hobject pHModuleBarcodeGrabImage[MAX_MODULE_ONE_TRAY];
	int m_iBarcodeGrabStatus[MAX_MODULE_ONE_TRAY];
	int m_iInspectGrabStatus[MAX_MODULE_ONE_TRAY];

	int m_pMolduleNo;
	int TrayModuduleNumber;

	BOOL m_bLiveGrabAvailable;
	BOOL m_bLive;
	Hobject m_HResultDefectRgn;
	Hobject m_pHImage;		// Current Display Image

	Hobject m_HTeachingImage[MAX_IMAGE_TAB];

	Hobject m_HParameterDefectRgn;

	Hobject m_HCropMatchingImage;
	POINT CropLTPoint,CropRBPoint;

	DPOINT m_dLTPoint, m_dRBPoint;

	Hobject Contours[BARREL_LENS_IMAGE_TAB][MAX_CONTOUR_NUM];		// 24.04.04 - v2645 - 이미지별 매칭 디스플레이 - LeeGW
	Hobject ContoursAffinTrans[MAX_CONTOUR_NUM];	// 매칭된 피팅라인 5개
	Hobject ContoursAffinTrans1_Uniformity;
	Hobject Partition_Region;
	void UniformityDisplay();
	Hobject m_pThresRgn;
	Hobject m_HInspectAreaRgn;
	Hlong m_lWindowID;
	
	Hobject m_HShapeModelContrastRgn;
	Hobject m_HParameterDefectXLD;

	Hobject m_HBlackCoatingCircleXLD;
	Hobject m_HDatumCircleXLD;
	Hobject m_HDiamSBLineXLD;

	Hobject m_HBlackCoatingEdgePointRgn;
	Hobject m_HDatumEdgePointRgn;

	double m_dBlackCoatingCenterRow, m_dBlackCoatingCenterCol;
	double m_dDatumCenterRow, m_dDatumCenterCol;

	double m_dBlackCoatingOuterDiameter;
	double m_dDatumOuterDiameter;
	double m_dO1O2Distance;
	double m_dDiamSB, m_dDiameterMin;
	
	HTuple hide_windowwidth,hide_windowheight;

	Hobject m_HAutoFocusCheckRgn;
	void CreateAutoFocusROI();
	void ClearAutoFocusROI();

	void UpdateThresRegion(int Min,int Max);
	void set_display_font (Halcon::HTuple WindowHandle, Halcon::HTuple Size, Halcon::HTuple Font, 
    Halcon::HTuple Bold, Halcon::HTuple Slant);

	void CopyInspectionROI();

	void ClearAlignTabTempRegion();

// Dialog Data
	//{{AFX_DATA(CInspectAdminViewDlg)
	enum { IDD = IDD_INSPECT_ADMIN_VIEW_DLG };
	CScrollBar	m_VScrBar;
	CScrollBar	m_HScrBar;
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CInspectAdminViewDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CInspectAdminViewDlg)
	virtual BOOL OnInitDialog();
	afx_msg BOOL OnEraseBkgnd(CDC *pDC);
	afx_msg void OnPaint();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnDestroy();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg void OnTbDraw();
	afx_msg void OnTbMove();
	afx_msg void OnTbSelectPart();
	afx_msg void OnTbROIRect();
	afx_msg void OnTbROICircle();
	afx_msg void OnTbROIPolygon();
	afx_msg void OnTbROIDelete();
	afx_msg void OnTbZoomIn();
	afx_msg void OnTbZoomOut();
	afx_msg void OnTbFitWidth();
	afx_msg void OnTbFitHeight();
	afx_msg void OnTbLive();
	afx_msg void OnTbFitLineDisplay();
	afx_msg void OnTbInspectAreaDisplay();
	afx_msg void OnTbThresholdAreaDisplay();
	afx_msg void OnTbDefectAreaDisplay();
	afx_msg void OnTb2pCaliper();
	afx_msg void OnTbAverageGray();
	afx_msg void OnTbROILock();
	afx_msg void OnTbDispCrossBar();
	afx_msg void OnMenuRoiCopyImage1();
	afx_msg void OnMenuRoiCopyImage2();
	afx_msg void OnMenuRoiCopyImage3();
	afx_msg void OnMenuRoiCopyImage4();
	afx_msg void OnMenuRoiCopyImage5();
	afx_msg void OnMenuRoiCopyImage6();
	afx_msg void OnMenuRoiCopyImage7();
	afx_msg void OnMenuROITypeChange();
	afx_msg void OnMenuROIIDChange();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//}}AFX_MSG
	afx_msg LRESULT OnEventPcbModelChanaged(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnEventFrameInspectDone(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT On2DFrameReady(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnEventGoToDefect(WPARAM wParam, LPARAM lParam);	
	DECLARE_MESSAGE_MAP()

protected:
	
	CMenu			m_Menu;		
	int				m_iRoiCopyImageIndex;
	
	// ToolBar
	CToolBarCombo	m_ViewToolbar;
	BOOL			m_bROILock;
	int				m_iToolBarOffset;
	eTOOLBAR_STATE	m_ToolBarState;
	eTOOLBAR_STATE	m_PrevToolBarState;
	BOOL			m_bOnMoving;
	
	int				m_iTeachingContourType;
	// Model Data
	
	
	BOOL			m_bDisplayImage;
	int				m_iInspectingFrameIndex;
	
	GTRegion *mpSelectPartTRegion;
	GTRegion *mpActiveTRegion;
	int miActiveCPointIndex;

	//	Hobject m_HImage;
	
	CPoint maMVPoints[2];	// View coordinate
	CPoint maMIPoints[2];	// Image coordinate
	
	void DrawSelectPartTRegion();
	void DrawActiveTRegion();
	
	int m_iImageWidth;						// Image Width
	int m_iImageHeight;						// Image Height
	
	int m_iGerberImageWidth;				// Gerber Image Width
	int m_iGerberImageHeight;				// Gerber Image Height
	
	double m_dZoomRatio;					// Image Zoom Ratio in Current View
	
	CRect m_ClientRect;						// Size of Current Dialog
	CRect m_ViewRect;						// Image Coordinate in Current View
	
	int m_iScrBarWidth;						// ScrollBar Width
	void ScrollBarSet();
	void ScrollBarPosSet();
	
	GViewportManager m_ViewportManager;		// Viewport Manager
	
	void InitViewRect();
	void UpdateViewportManager();
	
	void MoveActiveRegion(int iMoveDirection);
	void ScaleActiveRegion(int iMoveDirection, BOOL bScaleUp);
	
	void ViewRectSet();
	
	void ZoomInSelectedPart(double dZoomRatio, int iStartPosX, int iStartPosY);

	int	m_iDefectIdx;

	BOOL	m_bDisplayFitLine;
	BOOL	m_bDisplayInspectArea;
	BOOL	m_bDisplayThresholdArea;
	BOOL	m_bDisplayDefectArea;

	int			m_caliper_control_flag;
	int			m_AverageGray_control_flag;

	// CrossBar
	BOOL		m_bDispCrossBar;
	Hobject		m_HCrossBarXLD;

	HICON	m_HIconSelectedPad;
	int			m_iNoCaliperPoint;
	CPoint	mpLineSP;
	CPoint	mpLineEP;
	void		DrawArrow(CDC *pDC);
	double	edLineLength ( double sx, double sy, double ex, double ey );
	void EditXldData(int Row1, int Col1, int Row2, int Col2);

	// DontCare Process
	BOOL CheckDontCareInclusion(Hobject HNewRegion);
	void GetRegionROIAfterDontCareRemoval(GTRegion *pNewTRegion);

	// Add Process
	BOOL CheckAddInclusion(Hobject HNewRegion);
	void GetRegionROIAfterAddRemoval(GTRegion *pNewTRegion);

public:
	void ClearActiveTRegion();
	void ClearAllSelectRegion();
	void ClearAllLastSelectRegion();
	void SaveInspectTabOverLastSelectRegion();
	void SetToolBarStateDraw();

	void UpdateToolbarStatus(BOOL bTeachMode);

	void ResetInspectionResult();
	void AddRegionROI(int iThresMin, int iThresMax, BOOL bCheckMinBlob, BOOL bCheckMaxArea, BOOL bCheckFillUp, BOOL bCheckOpeningClosingInverse,
		BOOL bCheckOpening, BOOL bCheckClosing, BOOL bCheckConvex, BOOL bCheckRect, BOOL bCheckDifference, BOOL bCheckBoundary, BOOL bCheckDilation, int iEditMinBlobSize, int iEditOpeningSize, int iEditClosingSize, int iEditBoundaryDilationSize, int iEditDilationSize);
	void ShowRegionROI(int iThresMin, int iThresMax, BOOL bCheckMinBlob, BOOL bCheckMaxArea, BOOL bCheckFillUp, BOOL bCheckOpeningClosingInverse,
		BOOL bCheckOpening, BOOL bCheckClosing, BOOL bCheckConvex, BOOL bCheckRect, BOOL bCheckDifference, BOOL bCheckBoundary, BOOL bCheckDilation, int iEditMinBlobSize, int iEditOpeningSize, int iEditClosingSize, int iEditBoundaryDilationSize, int iEditDilationSize);

	void GetRegionROIAfterRotation(double dRotationAngle);	// LeeGW

	// Scan Select ComboBox
	int Get_CurScanTab();
	void Set_CurScanTab(int nScanNum);
	void Add_ScanTab(int nScanNum);
	void Clear_ScanTab();

	int Get_ScanTabCount();
	int Get_ScanTabIndex();
	void Set_ScanTabIndex(int nIdx);
	
	void Show();
	BOOL ShowWindow(int nCmdShow);
	void Reset();
	void ClearAll();
	void InitializeToolBar();
	
	void LoadScanImage(int nScanNum);
	void MoveToDefectArea();

	void RedrawView();
	void RedrawRect();
	BOOL SelectAllPackage();
	BOOL SelectPackageChange();
	BOOL SelectPackageUndo();
	void UnSelectAll();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);

	GTRegion *m_pLastSelectedRegion;	// 24.02.28 Local Align 추가 - LeeGW


	CDelegate delegateROISelected[MAX_IMAGE_TAB];	// 검사유형 통합(ROI기준 알고리즘 설정) - LeeGW
	CDelegate delegateCurROITabParamSave[MAX_IMAGE_TAB];	// 검사유형 통합(ROI기준 알고리즘 설정) - LeeGW
};
#endif