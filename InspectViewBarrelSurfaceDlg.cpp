// InspectViewDlg1.cpp : implementation file
//
#include "stdafx.h"
#include "uscan.h"
#include "InspectViewBarrelSurfaceDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CInspectViewBarrelSurfaceDlg dialog

CInspectViewBarrelSurfaceDlg* CInspectViewBarrelSurfaceDlg::m_pInstance = NULL;

CInspectViewBarrelSurfaceDlg* CInspectViewBarrelSurfaceDlg::GetInstance(BOOL bShowFlag)
{
	if (!m_pInstance) {
		m_pInstance = new CInspectViewBarrelSurfaceDlg();
		if(!m_pInstance->m_hWnd) {
			CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
			m_pInstance->Create(IDD_INSPECT_VIEW_BARREL_SURFACE_DLG, pFrame->GetActiveView());
			if (bShowFlag) m_pInstance->Show();
		}
	}
	return m_pInstance;
}

void CInspectViewBarrelSurfaceDlg::DeleteInstance()
{
	if(m_pInstance->GetSafeHwnd())
		m_pInstance->DestroyWindow();
	SAFE_DELETE(m_pInstance);
}

void CInspectViewBarrelSurfaceDlg::Show()
{
	ShowWindow(SW_HIDE);
	MoveWindow(m_ScreenRect.left, m_ScreenRect.top, m_ScreenRect.right, m_ScreenRect.bottom);
	ShowWindow(SW_SHOW);
}

CInspectViewBarrelSurfaceDlg::CInspectViewBarrelSurfaceDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CInspectViewBarrelSurfaceDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CInspectViewBarrelSurfaceDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	SetPosition(VIEW1_DLG1_LEFT, VIEW1_DLG1_TOP, (VIEW1_DLG1_WIDTH+290)/2, (VIEW1_DLG1_HEIGHT+370)/2);
	
	m_lWindowID = -1;
	m_iToolBarOffset = 0;
	m_iScrBarWidth = 16;
	
	m_bOnPaintNow = FALSE;
	m_bDrawActiveTRegion = FALSE;

	ClearAll();
}

void CInspectViewBarrelSurfaceDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CInspectViewBarrelSurfaceDlg)
	DDX_Control(pDX, IDC_VSCRBAR, m_VScrBar);
	DDX_Control(pDX, IDC_HSCRBAR, m_HScrBar);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CInspectViewBarrelSurfaceDlg, CDialog)
	//{{AFX_MSG_MAP(CInspectViewBarrelSurfaceDlg)
	ON_WM_ERASEBKGND()
	ON_WM_PAINT()
	ON_WM_HSCROLL()
	ON_WM_VSCROLL()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_DESTROY()
	ON_WM_SIZE()
	ON_WM_MOUSEWHEEL()
	ON_WM_SETCURSOR()
	ON_COMMAND(ID_TB_DRAW, OnTbDraw)
	ON_COMMAND(ID_TB_MOVE, OnTbMove)
	ON_COMMAND(ID_TB_SELECT_PART, OnTbSelectPart)
	ON_COMMAND(ID_TB_ZOOM_IN, OnTbZoomIn)
	ON_COMMAND(ID_TB_ZOOM_OUT, OnTbZoomOut)
	ON_COMMAND(ID_TB_FIT_WIDTH, OnTbFitWidth)
	ON_COMMAND(ID_TB_FIT_HEIGHT, OnTbFitHeight)
	ON_WM_CREATE()
	//}}AFX_MSG_MAP
	
	ON_MESSAGE(UM_2DFRAME_READY, On2DFrameReady)
	ON_MESSAGE(UM_RS_GOTO_DEFECT, OnEventGoToDefect)	
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CInspectViewBarrelSurfaceDlg message handlers

void CInspectViewBarrelSurfaceDlg::ClearAll()
{
	m_dZoomRatio = 0.4;
	
	if (THEAPP.Struct_PreferenceStruct.m_iCamFOVType == CAM_FOV_CROP)
	{
		m_iImageWidth = GRAB_X_MAX;
		m_iImageHeight = GRAB_Y_MAX;
	}
	else
	{
		m_iImageWidth = THEAPP.m_pCameraManager->GetCamImageWidth();
		m_iImageHeight = THEAPP.m_pCameraManager->GetCamImageHeight();
	}

	mpActiveTRegion = NULL;
	mpSelectPartTRegion = NULL;
	

	
	m_ToolBarState = TS_SELECT_PART;
	m_bOnMoving = FALSE;
	m_bDisplayImage = FALSE;
	m_iInspectingFrameIndex = -1;
	
	m_iDefectIdx = -1;
}

LRESULT CInspectViewBarrelSurfaceDlg::OnEventPcbModelChanaged(WPARAM wParam, LPARAM lParam)
{
	Reset();

	m_bDisplayImage = TRUE;

	InitViewRect();
	UpdateViewportManager();	// CSKIM
	ScrollBarSet();
	OnTbFitHeight();

	InvalidateRect(&m_ClientRect, TRUE);
	return 0;
}

// For Release
LRESULT CInspectViewBarrelSurfaceDlg::OnEventFrameInspectDone(WPARAM wParam, LPARAM lParam)
{
	m_iInspectingFrameIndex = (int)wParam;
	
	return 0;
}

BOOL CInspectViewBarrelSurfaceDlg::PreTranslateMessage(MSG* pMsg) 
{
	switch (pMsg->message) {
	case WM_NCLBUTTONDOWN :
		SetActiveWindow();
		return TRUE;
	case WM_KEYDOWN:
		if (pMsg->wParam == VK_ESCAPE || pMsg->wParam == VK_RETURN) return TRUE;
		break;
	case WM_SYSKEYDOWN:
		if (pMsg->wParam == VK_F4) return TRUE;
		break;
	}
	return CDialog::PreTranslateMessage(pMsg);
}

// Scan Select ComboBox
int CInspectViewBarrelSurfaceDlg::Get_CurScanTab()
{
	return m_ViewToolbar.Get_CurComboBox();
}

void CInspectViewBarrelSurfaceDlg::Set_CurScanTab(int nScanNum)
{ 
	CString strScan;
	strScan.Format("SCAN_%d", nScanNum);
	m_ViewToolbar.Set_CurComboBox(strScan);
}

void CInspectViewBarrelSurfaceDlg::Add_ScanTab(int nScanNum)
{
	CString strScan;
	strScan.Format("SCAN_%d", nScanNum);
	m_ViewToolbar.Add_ComboBox(strScan);
}

void CInspectViewBarrelSurfaceDlg::Clear_ScanTab()
{
	m_ViewToolbar.Clear_ComboBox();
}

int CInspectViewBarrelSurfaceDlg::Get_ScanTabCount()
{
	return m_ViewToolbar.Get_ComboBoxCount();
}

int CInspectViewBarrelSurfaceDlg::Get_ScanTabIndex()
{
	return m_ViewToolbar.Get_ComboBoxIndex();
}

void CInspectViewBarrelSurfaceDlg::Set_ScanTabIndex(int nIdx)
{
	m_ViewToolbar.Set_ComboBoxIndex(nIdx);
}

BOOL CInspectViewBarrelSurfaceDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	// Toolbar
	CRect ToolbarBorder(3, 3, 3, 3);
	if (!m_ViewToolbar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_BORDER | WS_VISIBLE | CBRS_TOP | CBRS_TOOLTIPS, ToolbarBorder) || 
		!m_ViewToolbar.LoadToolBar(IDR_INSPECT_VIEW_TOOLBAR)) {
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}

	CRect    rcClientOld; // Old Client Rect
	CRect    rcClientNew; // New Client Rect with Tollbar Added
	GetClientRect(rcClientOld); // Retrive the Old Client WindowSize
	RepositionBars(AFX_IDW_CONTROLBAR_FIRST, AFX_IDW_CONTROLBAR_LAST, 0, reposQuery, rcClientNew);
 
	m_iToolBarOffset = rcClientNew.top - rcClientOld.top;
  
    RepositionBars(AFX_IDW_CONTROLBAR_FIRST, AFX_IDW_CONTROLBAR_LAST, 0);

	m_ViewToolbar.SetButtonStyle(0, TBBS_CHECKGROUP);
	m_ViewToolbar.SetButtonStyle(1, TBBS_CHECKGROUP);
	m_ViewToolbar.SetButtonStyle(2, TBBS_CHECKGROUP);

	CToolBarCtrl &toolbarctrl = m_ViewToolbar.GetToolBarCtrl();
	toolbarctrl.CheckButton(ID_TB_SELECT_PART, FALSE);

	GetClientRect(&m_ClientRect);
	m_bDisplayImage = TRUE;
	Reset();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

BOOL CInspectViewBarrelSurfaceDlg::OnEraseBkgnd(CDC *pDC)
{
 	pDC->FillSolidRect(m_ClientRect, TS_COLOR_EXTRA_BG);
	return TRUE;
}

void CInspectViewBarrelSurfaceDlg::OnPaint() 
{
	CPaintDC dc(this); // device context for painting

 	if (m_lWindowID < 0 || !m_bDisplayImage || m_bOnPaintNow) return;

	m_bOnPaintNow = TRUE;

	CDC* pDC = GetDC();
	
	POINT ClientOffset;
	ClientOffset.x = 0;
	ClientOffset.y = m_iToolBarOffset;
	pDC->SetViewportOrg(ClientOffset);

	dc.OffsetViewportOrg(0, m_iToolBarOffset);

	HTuple lDC = (INT)(pDC->m_hDC);
	set_window_dc ( m_lWindowID, lDC ) ;

	CRgn ClipRgn;
	ClipRgn.CreateRectRgn(m_ClientRect.left ,m_ClientRect.top, m_ClientRect.right, m_ClientRect.bottom);
	pDC->SelectClipRgn(&ClipRgn);

	
	
	if(THEAPP.m_pGFunction->ValidHImage(m_pHImage))
		disp_image(m_pHImage, m_lWindowID);
	
	set_window_dc(m_lWindowID, 0);
	
	ReleaseDC(pDC);
	
	m_bOnPaintNow = FALSE;

	// Do not call CDialog::OnPaint() for painting messages
}

void CInspectViewBarrelSurfaceDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	if (m_lWindowID < 0 || !m_bDisplayImage) return;
	
	long CurPos = m_HScrBar.GetScrollPos();
	SCROLLINFO Info;
	m_HScrBar.GetScrollInfo(&Info, SIF_ALL);
	
	int nViewWidth = RECTWIDTH(&m_ViewRect);
	
	switch (nSBCode) {
	case SB_LEFT:					//Scroll to far left.
		CurPos = 0;
		break;
	case SB_RIGHT:					//Scroll to far right.
		CurPos = m_iImageWidth - nViewWidth;
		break;
	case SB_LINELEFT:				//Scroll left.
		if (CurPos > 0) CurPos -= 50;
		break;
	case SB_LINERIGHT:				//Scroll right.
		if (CurPos < m_iImageWidth - nViewWidth) CurPos += 50;
		break;
	case SB_PAGELEFT:				//Scroll one page left.
		if (CurPos > 0) CurPos = max(0, (int)CurPos - (int)Info.nPage);
		break;
	case SB_PAGERIGHT:				//Scroll one page right.
		if (CurPos < m_iImageWidth - nViewWidth) CurPos = min((int)m_iImageWidth - (int)nViewWidth, (int)CurPos + (int)Info.nPage);
		break;
	case SB_THUMBTRACK:
		CurPos = Info.nTrackPos;
		break;
	}
	m_HScrBar.SetScrollPos(CurPos);
	m_ViewRect.right = CurPos + nViewWidth;
	m_ViewRect.left = CurPos;
	
	UpdateViewportManager();
	InvalidateRect(NULL, TRUE);
	
	CDialog::OnHScroll(nSBCode, nPos, pScrollBar);
}

void CInspectViewBarrelSurfaceDlg::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	if (m_lWindowID < 0 || !m_bDisplayImage) return;

	long CurPos = m_VScrBar.GetScrollPos();
	SCROLLINFO Info;
	m_VScrBar.GetScrollInfo(&Info, SIF_ALL);
	
	int nViewHeight = RECTHEIGHT(&m_ViewRect);
	
	switch (nSBCode) {
	case SB_TOP:				//Scroll to top. 
		CurPos = m_iImageHeight - nViewHeight;
		break;
	case SB_BOTTOM:				//Scroll to bottom.
		CurPos = 0;
		break;
	case SB_LINEUP:				//Scroll one line up.
		if (CurPos > 0) CurPos -= 50;
		break;
	case SB_LINEDOWN:			//Scroll one line down.
		if (CurPos < m_iImageHeight - nViewHeight) CurPos += 50;
		break;
	case SB_PAGEUP:				//Scroll one page up.
		if (CurPos > 0) CurPos = max(0, (int)CurPos - (int)Info.nPage);
		break;
	case SB_PAGEDOWN:			//Scroll one page down.
		if(CurPos < m_iImageHeight - nViewHeight) CurPos = min((int)m_iImageHeight - (int)nViewHeight, (int)CurPos + (int)Info.nPage);
		break;
	case SB_THUMBTRACK:
		CurPos = Info.nTrackPos;
		break;
	}
	m_VScrBar.SetScrollPos(CurPos);
	m_ViewRect.bottom = CurPos + nViewHeight;
	m_ViewRect.top = CurPos;
	
	UpdateViewportManager();
	InvalidateRect(NULL, TRUE);

	CDialog::OnVScroll(nSBCode, nPos, pScrollBar);
}

void CInspectViewBarrelSurfaceDlg::OnLButtonDown(UINT nFlags, CPoint point) 
{
	if (!m_bDisplayImage) return;
	
	POINT VPoint = point;
	POINT IPoint = point;
	
	VPoint.y -= m_iToolBarOffset;	// Toolbar offset
	IPoint.y -= m_iToolBarOffset;	// Toolbar offset
	
	m_ViewportManager.VPtoIP(&IPoint, 1);
	
	maMVPoints[0] = VPoint;
	maMIPoints[0] = IPoint;
	
	SetCapture();

	switch (m_ToolBarState) {
	case TS_DRAW:

		break;

	case TS_MOVE:
		m_bOnMoving = TRUE;
		PostMessage(WM_SETCURSOR);
		break;

	case TS_SELECT_PART:
		mpSelectPartTRegion = new CSelectPartRegion;
		if (mpSelectPartTRegion) {
			mpSelectPartTRegion->SetLTPoint(IPoint, THEAPP.m_pCalDataService);
			mpSelectPartTRegion->SetRBPoint(IPoint, THEAPP.m_pCalDataService);

			mpSelectPartTRegion->SetSelect(TRUE, FALSE);
			DrawSelectPartTRegion();
		}
		break;
	}
	CDialog::OnLButtonDown(nFlags, point);
}

void CInspectViewBarrelSurfaceDlg::OnLButtonUp(UINT nFlags, CPoint point) 
{
	if (!m_bDisplayImage) return;

	ReleaseCapture();

	DPOINT dLTPoint, dRBPoint;
	POINT WHPoint;
// 	POINT LTPoint;
// 	BOOL bRet;

	switch (m_ToolBarState) {
	case TS_MOVE:
		m_bOnMoving = FALSE;
		break;

	case TS_SELECT_PART:
		if (mpSelectPartTRegion) {
			mpSelectPartTRegion->GetWHPoint(&WHPoint, THEAPP.m_pCalDataService);

			if (WHPoint.x < 10 || WHPoint.y < 10) {
				delete mpSelectPartTRegion;
				mpSelectPartTRegion = NULL;
				
				InvalidateRect(&m_ClientRect, FALSE);
				break;
			}

			mpSelectPartTRegion->ArrangePoints();
			
			mpSelectPartTRegion->GetLTPointD(&dLTPoint, THEAPP.m_pCalDataService);
			mpSelectPartTRegion->GetRBPointD(&dRBPoint, THEAPP.m_pCalDataService);

			delete mpSelectPartTRegion;
			mpSelectPartTRegion = NULL;

			if (dLTPoint.x<0 || dLTPoint.x>=m_iImageWidth || dLTPoint.y<0 || dLTPoint.y>=m_iImageHeight ||
				dRBPoint.x<0 || dRBPoint.x>=m_iImageWidth || dRBPoint.y<0 || dRBPoint.y>=m_iImageHeight)
				break;

			int iSelectImageWidth, iSelectImageHeight;
			double dZoomRatio;

			if (dLTPoint.x<0) iSelectImageWidth = (int)(dRBPoint.x + 1);
			else if (dRBPoint.x>=m_iImageWidth) iSelectImageWidth = (int)(m_iImageWidth - dLTPoint.x);
			else
				iSelectImageWidth = (int)(dRBPoint.x - dLTPoint.x + 1);

			if (dLTPoint.y<0) iSelectImageHeight = (int)(dRBPoint.y + 1);
			else if (dRBPoint.y>=m_iImageHeight) iSelectImageHeight = (int)(m_iImageHeight - dLTPoint.y);
			else
				iSelectImageHeight = (int)(dRBPoint.y - dLTPoint.y + 1);

			if (iSelectImageHeight<=iSelectImageWidth)			// 20080129 Eunsung
				dZoomRatio = (float)RECTWIDTH(&m_ClientRect)/(float)iSelectImageWidth;
			else
				dZoomRatio = (float)RECTHEIGHT(&m_ClientRect)/(float)iSelectImageHeight;

			ZoomInSelectedPart(dZoomRatio, (int)dLTPoint.x, (int)dLTPoint.y);
		}
		break;
	}

	int iViewWidth, iViewHeight;

	iViewWidth = (int)(m_iImageWidth * m_ViewportManager.mdZoomRatio +0.5);
	iViewHeight = (int)(m_iImageHeight * m_ViewportManager.mdZoomRatio + 0.5);

	if (iViewWidth<m_ClientRect.Width() || iViewHeight<m_ClientRect.Height())
		InvalidateRect(&m_ClientRect, TRUE);
	else
		InvalidateRect(&m_ClientRect, FALSE);			
	
	CDialog::OnLButtonUp(nFlags, point);
}

void CInspectViewBarrelSurfaceDlg::OnMouseMove(UINT nFlags, CPoint point) 
{
	if (!m_bDisplayImage) return;
	
	POINT VPoint = point;
	POINT IPoint = point;
	
	VPoint.y -= m_iToolBarOffset;
	IPoint.y -= m_iToolBarOffset;
	
	m_ViewportManager.VPtoIP(&IPoint, 1);
	
	maMVPoints[1] = VPoint;
	maMIPoints[1] = IPoint;
	
	CString OutTxt;
	OutTxt.Format("Barrel Surface - (%d, %d), Zoom(%d%%)", (int)((point.x/m_dZoomRatio)+m_ViewRect.left), (int)((point.y/m_dZoomRatio)+m_ViewRect.top), (int)(m_dZoomRatio*100.));
	SetWindowText((LPCTSTR)OutTxt);

	int iWidth, iHeight;
	long HCurPos, VCurPos;
	CPoint MVDiffPoint, MIDiffPoint;

	if (IS_SETFLAG(nFlags, MK_LBUTTON)) {
		switch (m_ToolBarState) {
		case TS_MOVE:
			MVDiffPoint.x = (long)((double)(maMVPoints[0].x - maMVPoints[1].x) / m_ViewportManager.mdZoomRatio);	// 20080129 Eunsung
			MVDiffPoint.y = (long)((double)(maMVPoints[0].y - maMVPoints[1].y) / m_ViewportManager.mdZoomRatio);

			m_ViewportManager.miStartXPos += MVDiffPoint.x;
			m_ViewportManager.miStartYPos += MVDiffPoint.y;

			iWidth = (int) ((double)RECTWIDTH(&m_ClientRect) / m_ViewportManager.mdZoomRatio);
			iHeight = (int) ((double)RECTHEIGHT(&m_ClientRect) / m_ViewportManager.mdZoomRatio);

			if(m_ViewportManager.miStartXPos + iWidth > m_iImageWidth) m_ViewportManager.miStartXPos = m_iImageWidth - iWidth;
			if(m_ViewportManager.miStartYPos + iHeight > m_iImageHeight) m_ViewportManager.miStartYPos = m_iImageHeight - iHeight;

			if(m_ViewportManager.miStartXPos < 0) m_ViewportManager.miStartXPos = 0;
			if(m_ViewportManager.miStartYPos < 0) m_ViewportManager.miStartYPos = 0;
			
			set_part(m_lWindowID, m_ViewportManager.miStartYPos, m_ViewportManager.miStartXPos, m_ViewportManager.miStartYPos + iHeight, m_ViewportManager.miStartXPos + iWidth);

			m_ViewRect.top = m_ViewportManager.miStartYPos;
			m_ViewRect.left = m_ViewportManager.miStartXPos;
			m_ViewRect.bottom = m_ViewRect.top + iHeight;
			m_ViewRect.right = m_ViewRect.left + iWidth;
			
			HCurPos = m_HScrBar.GetScrollPos();
			HCurPos = HCurPos + MVDiffPoint.x;
			if (HCurPos<0) HCurPos = 0;
			if (HCurPos > m_iImageWidth-RECTWIDTH(&m_ViewRect)) HCurPos = m_iImageWidth-RECTWIDTH(&m_ViewRect);
			m_HScrBar.SetScrollPos(HCurPos);

			VCurPos = m_VScrBar.GetScrollPos();
			VCurPos = VCurPos + MVDiffPoint.y;
			if (VCurPos<0) VCurPos = 0;
			if (VCurPos > m_iImageHeight-RECTHEIGHT(&m_ViewRect)) VCurPos = m_iImageHeight-RECTHEIGHT(&m_ViewRect);
			m_VScrBar.SetScrollPos(VCurPos);

			InvalidateRect(&m_ClientRect, FALSE);

			break;

		case TS_SELECT_PART:

			if (!mpSelectPartTRegion) return;
			
			DrawSelectPartTRegion();

			mpSelectPartTRegion->SetRBPoint(IPoint, THEAPP.m_pCalDataService);

			DrawSelectPartTRegion();

			break;

		default:
			break;
		}
	}

	maMVPoints[0] = maMVPoints[1];
	maMIPoints[0] = maMIPoints[1];

	CDialog::OnMouseMove(nFlags, point);
}

void CInspectViewBarrelSurfaceDlg::OnDestroy() 
{
	CDialog::OnDestroy();
	
	SAFE_DELETE(mpActiveTRegion);
	
	

	if(m_lWindowID > 0)
	{
		close_window(m_lWindowID);
		m_lWindowID = -1;
	}
}

void CInspectViewBarrelSurfaceDlg::OnSize(UINT nType, int cx, int cy) 
{
	CDialog::OnSize(nType, cx, cy);
	
	GetClientRect(&m_ClientRect);

	m_ClientRect.top = m_iToolBarOffset;
	m_ClientRect.right = m_ClientRect.right - m_iScrBarWidth; 
	m_ClientRect.bottom = m_ClientRect.bottom - m_iScrBarWidth;	

	InitViewRect();
	UpdateViewportManager();	// CSKIM
	ScrollBarSet();
	ScrollBarPosSet();

	InvalidateRect(&m_ClientRect, TRUE);	
}

BOOL CInspectViewBarrelSurfaceDlg::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt) 
{
	if (zDelta <= 0) SendMessage(WM_VSCROLL, SB_PAGEDOWN, 0);
	else SendMessage(WM_VSCROLL, SB_PAGEUP, 0);

	return CDialog::OnMouseWheel(nFlags, zDelta, pt);
}

BOOL CInspectViewBarrelSurfaceDlg::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message) 
{
	switch (m_ToolBarState) {
	case TS_MOVE:
		if (m_bOnMoving)
			::SetCursor(AfxGetApp()->LoadCursor(IDC_CURSOR_HANDGRAB));
		else
			::SetCursor(AfxGetApp()->LoadCursor(IDC_CURSOR_HAND));
		return TRUE;

	case TS_SELECT_PART:
		::SetCursor(AfxGetApp()->LoadCursor(IDC_CURSOR_SELECTPART));
		return TRUE;

// 	case TS_SELECT_PAD:
// 		break;
	}	
	return CDialog::OnSetCursor(pWnd, nHitTest, message);
}

void CInspectViewBarrelSurfaceDlg::Reset()
{
 
//	m_pFMDataContainer = THEAPP.m_pModelDataManager->GetFMDataContainer(1);

	InitViewRect();
	UpdateViewportManager();
	
	m_iInspectingFrameIndex = -1;
	m_iDefectIdx = -1;
}

void CInspectViewBarrelSurfaceDlg::InitViewRect() 
{
	m_ViewRect.left = 0;
	m_ViewRect.top = 0;
	m_ViewRect.bottom = (int)(RECTHEIGHT(&m_ClientRect) / m_dZoomRatio);
	m_ViewRect.right = (int)(RECTWIDTH(&m_ClientRect) / m_dZoomRatio);
}

void CInspectViewBarrelSurfaceDlg::UpdateViewportManager()
{
	m_ViewportManager.miStartXPos = m_ViewRect.left;
	m_ViewportManager.miStartYPos = m_ViewRect.top;
	m_ViewportManager.miViewWidth = (int)RECTWIDTH(&m_ClientRect);
	m_ViewportManager.miViewHeight = (int)RECTHEIGHT(&m_ClientRect);
	m_ViewportManager.miImageWidth = m_iImageWidth;
	m_ViewportManager.miImageHeight = m_iImageHeight;
	m_ViewportManager.mdZoomRatio = m_dZoomRatio;

	if (m_lWindowID>0) {
		set_part(m_lWindowID, m_ViewRect.top, m_ViewRect.left, m_ViewRect.bottom, m_ViewRect.right);
		set_window_extents(m_lWindowID, 0, 0, m_ClientRect.Width(), m_ClientRect.Height());
	}
}

void CInspectViewBarrelSurfaceDlg::ViewRectSet() 
{
	m_ViewRect.bottom = m_ViewRect.top + (int)(RECTHEIGHT(&m_ClientRect) / m_dZoomRatio);
	m_ViewRect.right = m_ViewRect.left + (int)(RECTWIDTH(&m_ClientRect) / m_dZoomRatio);
}

void CInspectViewBarrelSurfaceDlg::ScrollBarSet()
{
	SCROLLINFO Info;

	Info.cbSize = sizeof(SCROLLINFO);
	Info.fMask = SIF_ALL | SIF_DISABLENOSCROLL;
	Info.nMin = 0;

	if (m_bDisplayImage) {
		m_VScrBar.GetScrollInfo(&Info);
		Info.nMax = m_iImageHeight;
		Info.nPage = (int)(RECTHEIGHT(&m_ViewRect));
		Info.nPos = m_ViewRect.top;
	} else {
		Info.nMax = m_ClientRect.bottom; 
		Info.nPage = 10;
	}
	m_VScrBar.SetScrollInfo(&Info);
	
	if (m_bDisplayImage) {
		m_HScrBar.GetScrollInfo(&Info);
		Info.nMax = m_iImageWidth;
		Info.nPage = (int)(RECTWIDTH(&m_ViewRect));
		Info.nPos = m_ViewRect.left;
	} else {
		Info.nMax = m_ClientRect.right; 
		Info.nPage = 10;
	}
	m_HScrBar.SetScrollInfo(&Info);
}

void CInspectViewBarrelSurfaceDlg::ScrollBarPosSet()
{
	m_VScrBar.SetWindowPos(&wndTop, m_ClientRect.right, m_ClientRect.top, m_iScrBarWidth, m_ClientRect.bottom-m_iToolBarOffset, SWP_SHOWWINDOW);
	m_VScrBar.ShowScrollBar(TRUE);
	m_HScrBar.SetWindowPos(&wndTop, m_ClientRect.left, m_ClientRect.bottom, m_ClientRect.right, m_iScrBarWidth, SWP_SHOWWINDOW);
	m_HScrBar.ShowScrollBar(TRUE);
}

void CInspectViewBarrelSurfaceDlg::DrawActiveTRegion()
{
	if (m_bDrawActiveTRegion) return;

	m_bDrawActiveTRegion = TRUE;
	CDC *pDC = GetDC();

	POINT ClientOffset;
	ClientOffset.x = 0;
	ClientOffset.y = m_iToolBarOffset;
	pDC->SetViewportOrg(ClientOffset);

	CRgn ClipRgn;

	if (mpActiveTRegion)
	{
		ClipRgn.CreateRectRgn(m_ClientRect.left ,m_ClientRect.top, m_ClientRect.right, m_ClientRect.bottom);
		pDC->SelectClipRgn(&ClipRgn);
		mpActiveTRegion->Draw(pDC->m_hDC, &m_ViewportManager, GTR_DS_ACTIVE, FALSE, THEAPP.m_pCalDataService);
	}
	ReleaseDC(pDC);

	m_bDrawActiveTRegion = FALSE;
}

void CInspectViewBarrelSurfaceDlg::DrawSelectPartTRegion()
{
	CDC *pDC = GetDC();

	POINT ClientOffset;
	ClientOffset.x = 0;
	ClientOffset.y = m_iToolBarOffset;
	pDC->SetViewportOrg(ClientOffset);

	CRgn ClipRgn;

	if (mpSelectPartTRegion)
	{
		ClipRgn.CreateRectRgn(m_ClientRect.left ,m_ClientRect.top, m_ClientRect.right, m_ClientRect.bottom);
		pDC->SelectClipRgn(&ClipRgn);
		mpSelectPartTRegion->Draw(pDC->m_hDC, &m_ViewportManager, GTR_DS_SELECTPART, FALSE, THEAPP.m_pCalDataService);
	}
	ReleaseDC(pDC);
}

void CInspectViewBarrelSurfaceDlg::ZoomInSelectedPart(double dZoomRatio, int iStartPosX, int iStartPosY) 
{
	if (m_bDisplayImage) {
		m_dZoomRatio = dZoomRatio;

		m_ViewRect.top = iStartPosY;
		m_ViewRect.left = iStartPosX;
		
		ViewRectSet();

		if (m_ViewRect.right >= m_iImageWidth) {
			m_ViewRect.right = m_iImageWidth - 1;
			m_ViewRect.left = m_ViewRect.right - (int)(RECTWIDTH(&m_ClientRect) / m_dZoomRatio);
			if (m_ViewRect.left < 0) {
				m_ViewRect.left = 0;
				m_ViewRect.right = (int)(RECTWIDTH(&m_ClientRect) / m_dZoomRatio);
			}
		}
		
		if (m_ViewRect.bottom >= m_iImageHeight) {
			m_ViewRect.bottom = m_iImageHeight - 1;
			m_ViewRect.top = m_ViewRect.bottom - (int)(RECTHEIGHT(&m_ClientRect) / m_dZoomRatio);
			if (m_ViewRect.top < 0) {
				m_ViewRect.top = 0;
				m_ViewRect.bottom = (int)(RECTHEIGHT(&m_ClientRect) / m_dZoomRatio);
			}
		}

		ScrollBarSet();

//		m_ViewportManager.mdZoomRatio = m_dZoomRatio;
		UpdateViewportManager();

		CString OutTxt;
		OutTxt.Format("Barrel Surface - Zoom(%d%%)", (int)(m_dZoomRatio * 100.));
		SetWindowText(OutTxt);

		InvalidateRect(&m_ClientRect, TRUE);
	}	
}

void CInspectViewBarrelSurfaceDlg::OnTbDraw() 
{
	m_ToolBarState = TS_DRAW;
}

void CInspectViewBarrelSurfaceDlg::OnTbMove() 
{
 	m_ToolBarState = TS_MOVE;
}

void CInspectViewBarrelSurfaceDlg::OnTbSelectPart() 
{
 	m_ToolBarState = TS_SELECT_PART;
}

void CInspectViewBarrelSurfaceDlg::OnTbZoomIn() 
{
	if (m_bDisplayImage) {
		m_dZoomRatio = (int)(m_dZoomRatio * 10) * 0.1;
		
		if (m_dZoomRatio < 1.0) m_dZoomRatio += 0.1;
		else if (m_dZoomRatio < 2.0) m_dZoomRatio += 0.5;
		else if (m_dZoomRatio < 10.0) m_dZoomRatio += 1.0;
		else m_dZoomRatio = 10.0;

		ViewRectSet();
		ScrollBarSet();

//		m_ViewportManager.mdZoomRatio = m_dZoomRatio;
		UpdateViewportManager();

		CString OutTxt;
		OutTxt.Format("Barrel Surface - Zoom(%d%%)", (int)(m_dZoomRatio*100.));
		SetWindowText(OutTxt);

		InvalidateRect(&m_ClientRect, TRUE);
	}		
}

void CInspectViewBarrelSurfaceDlg::OnTbZoomOut() 
{
	if (m_bDisplayImage) {
		m_dZoomRatio = (int)(m_dZoomRatio * 10) * 0.1;
		
		if (m_dZoomRatio > 2.0) m_dZoomRatio -= 1.0;
		else if (m_dZoomRatio > 1.0) m_dZoomRatio -= 0.5;
		else if (m_dZoomRatio > 0.1) m_dZoomRatio -= 0.1;
		else m_dZoomRatio = 0.1;

		CRect TempViewRect;

		TempViewRect = m_ViewRect;
	
		TempViewRect.bottom = TempViewRect.top + (int)(RECTHEIGHT(&m_ClientRect) / m_dZoomRatio);
		TempViewRect.right = TempViewRect.left + (int)(RECTWIDTH(&m_ClientRect) / m_dZoomRatio);
		
		if (TempViewRect.right >= m_iImageWidth || TempViewRect.bottom >= m_iImageHeight) {
			if (TempViewRect.right >= m_iImageWidth) {
				m_ViewRect.right = m_iImageWidth - 1;
				m_ViewRect.left = m_ViewRect.right - (int)(RECTWIDTH(&m_ClientRect) / m_dZoomRatio);
				if (m_ViewRect.left < 0) {
					m_dZoomRatio = (float)RECTWIDTH(&m_ClientRect)/(float)m_iImageWidth;
					m_ViewRect.left = 0;
				}
				
				if (TempViewRect.bottom >= m_iImageHeight) {
					m_ViewRect.bottom = m_iImageHeight-1;
					m_ViewRect.top = m_ViewRect.bottom - (int)(RECTHEIGHT(&m_ClientRect) / m_dZoomRatio);
				} else {
					m_ViewRect.bottom = m_ViewRect.top + (int)(RECTHEIGHT(&m_ClientRect) / m_dZoomRatio);
				}
			}
			
			if (TempViewRect.bottom >= m_iImageHeight) {
				m_ViewRect.bottom = m_iImageHeight - 1;
				m_ViewRect.top = m_ViewRect.bottom - (int)(RECTHEIGHT(&m_ClientRect) / m_dZoomRatio);
				
				if (m_ViewRect.top < 0) {
					m_dZoomRatio = (float)RECTHEIGHT(&m_ClientRect)/(float)m_iImageHeight;
					m_ViewRect.top = 0;
				}
				
				if (TempViewRect.right >= m_iImageWidth) {
					m_ViewRect.right = m_iImageWidth - 1;
					m_ViewRect.left = m_ViewRect.right - (int)(RECTWIDTH(&m_ClientRect) / m_dZoomRatio);
				} else {
					m_ViewRect.right = m_ViewRect.left + (int)(RECTWIDTH(&m_ClientRect) / m_dZoomRatio);
				}
			}
		} else {
			ViewRectSet();
		}

		ScrollBarSet();

		m_ViewportManager.mdZoomRatio = m_dZoomRatio;
		UpdateViewportManager();

		CString OutTxt;
		OutTxt.Format("Barrel Surface - Zoom(%d%%)", (int)(m_dZoomRatio*100.));
		SetWindowText(OutTxt);

		InvalidateRect(&m_ClientRect, TRUE);
	}		
}

void CInspectViewBarrelSurfaceDlg::OnTbFitWidth() 
{
	if (m_bDisplayImage) {
		m_dZoomRatio = (double)RECTWIDTH(&m_ClientRect) / (double)m_iImageWidth;

		InitViewRect();
		UpdateViewportManager();	// CSKIM
		ScrollBarSet();

		m_ViewportManager.mdZoomRatio = m_dZoomRatio;
		UpdateViewportManager();

		CString OutTxt;
		OutTxt.Format("Barrel Surface - Zoom(%d%%)", (int)(m_dZoomRatio * 100.));
		SetWindowText(OutTxt);

		InvalidateRect(&m_ClientRect, TRUE);
	}			
}

void CInspectViewBarrelSurfaceDlg::OnTbFitHeight() 
{
	if (m_bDisplayImage) {
		m_dZoomRatio = (double)RECTHEIGHT(&m_ClientRect) / (double)m_iImageHeight;

		InitViewRect();
		UpdateViewportManager();	// CSKIM
		ScrollBarSet();

		m_ViewportManager.mdZoomRatio = m_dZoomRatio;
		UpdateViewportManager();

		CString OutTxt;
		OutTxt.Format("Barrel Surface - Zoom(%d%%)", (int)(m_dZoomRatio * 100.));
		SetWindowText(OutTxt);

		InvalidateRect(&m_ClientRect, TRUE);
	}			
}

BOOL CInspectViewBarrelSurfaceDlg::ShowWindow(int nCmdShow)
{
	return CDialog::ShowWindow(nCmdShow);
}

void CInspectViewBarrelSurfaceDlg::InitializeToolBar()
{
	CToolBarCtrl &toolbarctrl = m_ViewToolbar.GetToolBarCtrl();
	toolbarctrl.CheckButton(ID_TB_SELECT_PART, TRUE);
	m_ToolBarState = TS_SELECT_PART;
}

int CInspectViewBarrelSurfaceDlg::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CDialog::OnCreate(lpCreateStruct) == -1) return -1;
	
	HTuple tWnd = (INT)(m_hWnd) ;
	new_extern_window(tWnd, 0, 0, m_iImageWidth, m_iImageHeight, &m_lWindowID) ;
	
	return 0;
}

LRESULT CInspectViewBarrelSurfaceDlg::On2DFrameReady(WPARAM wParam, LPARAM lParam)
{

	return 0;
}

LRESULT CInspectViewBarrelSurfaceDlg::OnEventGoToDefect(WPARAM wParam, LPARAM lParam)	
{

	
	InvalidateRect(&m_ClientRect, TRUE);
	return 0;
}

void CInspectViewBarrelSurfaceDlg::LoadScanImage(int nScanNum) 
{

}

void CInspectViewBarrelSurfaceDlg::MoveToDefectArea()
{
	
}

void CInspectViewBarrelSurfaceDlg::RedrawView()
{
	InvalidateRect(NULL, FALSE);
}

void CInspectViewBarrelSurfaceDlg::RedrawRect()
{
	InvalidateRect(&m_ClientRect, FALSE);
}

BOOL CInspectViewBarrelSurfaceDlg::SelectAllPackage()
{
	return FALSE;
}

BOOL CInspectViewBarrelSurfaceDlg::SelectPackageChange() 
{
	return FALSE;
}

BOOL CInspectViewBarrelSurfaceDlg::SelectPackageUndo() 
{
	return FALSE;
}

void CInspectViewBarrelSurfaceDlg::UnSelectAll()
{

}
