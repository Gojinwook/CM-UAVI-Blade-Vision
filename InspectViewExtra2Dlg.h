
#ifndef CVIEW8_H
#define CVIEW8_H

#include "GViewPortManager.h"
#include "GTRegion.h"
#include "SelectPartRegion.h"
#include "ToolBarCombo.h"
// InspectViewDlg1 대화 상자입니다.
#define RECTWIDTH(lpRect)	((lpRect)->right - (lpRect)->left)
#define RECTHEIGHT(lpRect)	((lpRect)->bottom - (lpRect)->top)
class CInspectViewExtra2Dlg : public CDialog
{
	private:
	void	SetPosition(int left, int top, int right, int bottom) { m_ScreenRect = CRect(left, top, right, bottom); }
	CRect	m_ScreenRect;
	static CInspectViewExtra2Dlg	*m_pInstance;

public:
	static	CInspectViewExtra2Dlg* GetInstance(BOOL bShowFlag=FALSE);
	void	DeleteInstance();
	
	typedef enum { TS_DRAW=0, TS_MOVE, TS_SELECT_PART } eTOOLBAR_STATE;
	
// Construction
public:
	CInspectViewExtra2Dlg(CWnd* pParent = NULL);   // standard constructor



	Hobject m_pHImage;

// Dialog Data
	//{{AFX_DATA(CInspectViewExtra2Dlg)
	enum { IDD = IDD_INSPECT_VIEW_EXTRA2_DLG };
	CScrollBar	m_VScrBar;
	CScrollBar	m_HScrBar;
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CInspectViewExtra2Dlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CInspectViewExtra2Dlg)
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
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//}}AFX_MSG
	afx_msg LRESULT OnEventPcbModelChanaged(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnEventFrameInspectDone(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT On2DFrameReady(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnEventGoToDefect(WPARAM wParam, LPARAM lParam);	
	DECLARE_MESSAGE_MAP()

protected:
	BOOL	m_bOnPaintNow;
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
	
	Hlong m_lWindowID;
	
	void ViewRectSet();

	void ZoomInSelectedPart(double dZoomRatio, int iStartPosX, int iStartPosY);

	HICON	m_HIconSelectedDefect;
	int	m_iDefectIdx;

public:
	// Scan Select ComboBox
	Hlong GetDispWindowID();
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
};
#endif