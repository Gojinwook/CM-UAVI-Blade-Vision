
#ifndef CVIEW7_H
#define CVIEW7_H
//////////////////// 헤더 안 내용을 쓴다.

#pragma once

#include "GViewPortManager.h"
#include "GTRegion.h"
#include "SelectPartRegion.h"
#include "ToolBarCombo.h"
// InspectViewDlg1 대화 상자입니다.
#define RECTWIDTH(lpRect)	((lpRect)->right - (lpRect)->left)
#define RECTHEIGHT(lpRect)	((lpRect)->bottom - (lpRect)->top)

struct Point
{
	int x, y;
};


class CInspectAdminViewHideDlg : public CDialog
{
	private:
	
	CRect	m_ScreenRect;
	static CInspectAdminViewHideDlg	*m_pInstance;

public:
	static	CInspectAdminViewHideDlg* GetInstance(BOOL bShowFlag=FALSE);
	void	DeleteInstance();
	
	typedef enum { TS_DRAW=0, TS_MOVE, TS_SELECT_PART } eTOOLBAR_STATE;
	void	SetPosition(int left, int top, int right, int bottom) { m_ScreenRect = CRect(left, top, right, bottom); }

	void OverlaySetViewportManager(int iLeft, int iTop, double dZoomRatio);

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CInspectAdminViewHideDlg)
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
	afx_msg void OnTbZoomIn();
	afx_msg void OnTbZoomOut();
	afx_msg void OnTbFitWidth();
	afx_msg void OnTbFitHeight();
	afx_msg void OnTbLive();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//}}AFX_MSG
	afx_msg LRESULT OnEventPcbModelChanaged(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnEventFrameInspectDone(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT On2DFrameReady(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnEventGoToDefect(WPARAM wParam, LPARAM lParam);	
	DECLARE_MESSAGE_MAP()

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CInspectAdminViewHideDlg)
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//}}AFX_VIRTUAL

protected:
	
	BOOL	m_bDrawActiveTRegion;
	
	// ToolBar
	CToolBarCombo	m_ViewToolbar;
	int				m_iToolBarOffset;
	eTOOLBAR_STATE	m_ToolBarState;
	BOOL			m_bOnMoving;
	
	// Model Data
	
	
	BOOL			m_bDisplayImage;
	int				m_iInspectingFrameIndex;
	
	GTRegion *mpSelectPartTRegion;
	GTRegion *mpActiveTRegion;
	
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
	
	void ViewRectSet();

	void ZoomInSelectedPart(double dZoomRatio, int iStartPosX, int iStartPosY);

	HICON	m_HIconSelectedDefect;
	int	m_iDefectIdx;

	void DecideInspectResult();

	int m_iInspectResult;
	int m_iDirt;
	int m_iScratch;
	int m_iStain;
	int m_iDent;
	int m_iChipping;
	int m_iEpoxy;
	int m_iOutsideCT;
	int m_iPeelOff;
	int m_iWhiteDot;



// Construction
public:
	CInspectAdminViewHideDlg(CWnd* pParent = NULL);   // standard constructor

	void DrawFrameActiveTRegion();
	void DrawMatchingROI (int iLowVal, int iHighVal);

	void UpdateThresRegion (int Min,int Max);
	void set_display_font (Halcon::HTuple WindowHandle, Halcon::HTuple Size, Halcon::HTuple Font, Halcon::HTuple Bold, Halcon::HTuple Slant);
	// USE_SUAKIT 사용시 CMI 결과 데이터를 알기 위해 함수 추가
	// 혹시 몰라 Result Image에 대한 함수도 추가함. 
int WriteResultFileExtra(int iImageNo, Hobject HImgRef, Hobject HDefectRegion, Hobject HROIInspectAreaRgn, CString sBarcodeResult, CString* pStrDefectPosition, double* pdblArea, double *pdDefectCenterX, double *pdDefectCenterY);

	int WriteResultFile (int BarrelOrlens, Hobject HImgRef, Hobject HDefectRegion,Hobject InspectContour1, Hobject InspectContour3, Hobject InspectContour4, Hobject InspectContour5, 
		CString sBarcodeResult, double dBarcodePosOffsetX, double dBarcodePosOffsetY, BOOL bBarcodeShiftResult,double dBarcodePosOffsetRotation, BOOL bBarcodeRotationResult, BOOL bBarcodeModuleMixResult, double dBlackCoatingDiameter, double dBlackCoatingPosX, double dBlackCoatingPosY,
		double dO1O2Distance, double dDiameterMin, BOOL bBlackCoatingResult, CString* pStrDefectPosition, double* pdblArea, double *pdDefectCenterX, double *pdDefectCenterY,
		int iExtraResult, double* pdExDefectArea, double *pdExDefectCenterX, double *pdExDefectCenterY,
		BOOL bDefectState, BOOL bIsBarrelDefect ,BOOL bIsLensDefect, BOOL bBarcodeError, BOOL bMatchingSuccess, BOOL bLightDisorder, BOOL bModuleEmpty); //Barcode Rotation Log
	
	int WriteResultFileFAI(Hobject* HImage, int iTrayNo, int iModuleNo, CString sBarcode);

	// 24.06.12 - v2604 - ReviewImage 저장을 위해 추가 - LEEGW
	int WriteReviewImage (Hobject HImage, Hobject HDefectRgn, CString strFileName); 

	double Distance(const Point& p1, const Point& p2);


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

	BOOL m_bOnPaintNow;

	BOOL m_bInspectRunDisplay;

	BOOL m_bTestThread;

	int StartTime1,EndTime1;

	BOOL* m_bBarrelDefectOKNG;
	BOOL* m_bLensDefectOKNG;
	BOOL* m_bExtraDefectOKNG;

	Hobject *Image1,*Image2,*Image3,*Image4;

	char* LensOkNg; // chlee 렌즈 결과값 추출
	int m_iModuleNo;

	Hobject m_HResultDefectRgn;
	BOOL m_bLive;
	Hobject m_pHImage;

	Hobject m_HParameterDefectRgn;

	Hobject m_HCropMatchingImage;
	POINT CropLTPoint,CropRBPoint;

	DPOINT m_dLTPoint, m_dRBPoint;

	BOOL bIsFAISpecialNG;

	int iDirtDefectSortingNumber;
	int iScratchDefectSortingNumber;
	int iStainDefectSortingNumber;
	int iDentDefectSortingNumber;
	int iChippingDefectSortingNumber;
	int iEpoxyDefectSortingNumber;
	int iOutsideCTDefectSortingNumber;
	int iPeelOffDefectSortingNumber;
	int iWhiteDotDefectSortingNumber;	// 2024.09.30 - v2660 - WhiteDot 추가 - LEEGW

	// 2024.09.30 - v2660 - WhiteDot 추가 - LEEGW
	Hobject m_HRgnDirt, m_HRgnScratch, m_HRgnStain, m_HRgnDent, m_HRgnChipping, m_HRgnEpoxy, m_HRgnOutsideCT, m_HRgnPeelOff, m_HRgnWhiteDot;		// MinArea 이상인(판정 조건 통과한) defect 영역들

	// 2024.09.30 - v2660 - WhiteDot 추가 - LEEGW
	Hobject m_HRgnLensDirt, m_HRgnLensScratch, m_HRgnLensCT, m_HRgnLensWhiteDot, m_HRgnLensDefectMin;						///// 렌즈 defect MinArea 이상인(판정 조건 통과한) 영역

	Hobject m_pThresRgn;
	Hlong HideWindowHandle;

	HTuple hide_windowwidth,hide_windowheight;

	CString m_sImageFormat;

	BOOL m_bIsChangeResultFromADJ;

	Hobject m_HRgnEpoxyHole, m_HRgnNorthSpringNG, m_HRgnEastSpringNG;	// 설비별 불량 통합을 위한 ROI 불량 항목 추가

	Hobject m_HReviewXLD_FAI_Item[MAX_FAI_ITEM][MAX_ONE_FAI_MEASURE_VALUE];	// FAI 측정 Review - LeeGW
	
	// Dialog Data
	//{{AFX_DATA(CInspectAdminViewHideDlg)
	enum { IDD = IDD_INSPECT_ADMIN_VIEW_HIDE_DLG };
	CScrollBar	m_VScrBar;
	CScrollBar	m_HScrBar;
	//}}AFX_DATA
};
#endif