// InspectViewDlg1.cpp : implementation file
//
#include "stdafx.h"
#include "uscan.h"
#include "InspectAdminViewHideDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char * THIS_FILE = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CInspectAdminViewHideDlg dialog

CInspectAdminViewHideDlg* CInspectAdminViewHideDlg::m_pInstance = NULL;

CInspectAdminViewHideDlg* CInspectAdminViewHideDlg::GetInstance(BOOL bShowFlag)
{
	if (!m_pInstance) {
		m_pInstance = new CInspectAdminViewHideDlg();
		if(!m_pInstance->m_hWnd) {
			CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
			m_pInstance->Create(IDD_INSPECT_ADMIN_VIEW_HIDE_DLG, pFrame->GetActiveView());
			if (bShowFlag) m_pInstance->Show();
		}
	}
	return m_pInstance;
}

void CInspectAdminViewHideDlg::DeleteInstance()
{
	if(m_pInstance->GetSafeHwnd())
		m_pInstance->DestroyWindow();
	SAFE_DELETE(m_pInstance);
}

void CInspectAdminViewHideDlg::Show()
{
	ShowWindow(SW_HIDE);
	MoveWindow(m_ScreenRect.left, m_ScreenRect.top, m_ScreenRect.right, m_ScreenRect.bottom);
	ShowWindow(SW_SHOW);
}

CInspectAdminViewHideDlg::CInspectAdminViewHideDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CInspectAdminViewHideDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CInspectAdminViewHideDlg)
	// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	SetPosition(VIEW1_DLG1_LEFT, VIEW1_DLG1_TOP, VIEW1_DLG1_WIDTH+280, VIEW1_DLG1_HEIGHT+380);
	HideWindowHandle = -1;
	m_iToolBarOffset = 0;
	m_iScrBarWidth = 16;

	m_bOnPaintNow = FALSE;
	m_bDrawActiveTRegion = FALSE;
	StartTime1 = 0;
	EndTime1 = 0;
	m_bLive = FALSE;
	m_bTestThread = FALSE;

	m_sImageFormat = ".bmp";

	m_bIsChangeResultFromADJ = FALSE;

	LensOkNg = NULL;
	ClearAll();
}

void CInspectAdminViewHideDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CInspectAdminViewHideDlg)
	DDX_Control(pDX, IDC_VSCRBAR, m_VScrBar);
	DDX_Control(pDX, IDC_HSCRBAR, m_HScrBar);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CInspectAdminViewHideDlg, CDialog)
	//{{AFX_MSG_MAP(CInspectAdminViewHideDlg)
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
	ON_COMMAND(ID_TB_LIVE, OnTbLive)

	ON_WM_CREATE()
	//}}AFX_MSG_MAP

	ON_MESSAGE(UM_2DFRAME_READY, On2DFrameReady)
	ON_MESSAGE(UM_RS_GOTO_DEFECT, OnEventGoToDefect)	
	ON_WM_TIMER()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CInspectAdminViewHideDlg message handlers

void CInspectAdminViewHideDlg::ClearAll()
{

	if (THEAPP.Struct_PreferenceStruct.m_iCamFOVType == CAM_FOV_CROP)
	{
		m_iImageWidth = GRAB_X_MAX;
		m_iImageHeight = GRAB_Y_MAX;
		m_dZoomRatio = 0.75;
	}
	else
	{
		m_iImageWidth = THEAPP.m_pCameraManager->GetCamImageWidth();
		m_iImageHeight = THEAPP.m_pCameraManager->GetCamImageHeight();
		m_dZoomRatio = 0.55;
	}

	mpActiveTRegion = NULL;
	mpSelectPartTRegion = NULL;


	m_ToolBarState = TS_SELECT_PART;
	m_bOnMoving = FALSE;
	m_bDisplayImage = FALSE;
	m_iInspectingFrameIndex = -1;

	m_iDefectIdx = -1;


	m_pThresRgn.Reset();
	m_pHImage.Reset();
	m_HCropMatchingImage.Reset();

	gen_empty_obj(&m_pThresRgn);
	gen_empty_obj(&m_pHImage);
	gen_empty_obj(&m_HCropMatchingImage);
}

double CInspectAdminViewHideDlg::Distance(const Point& p1, const Point& p2)
{
	double dblDistance = 0.0;

	// 피타고라스의 정리 
	dblDistance = sqrt(pow(double(p1.x - p2.x), 2) + pow(double(p1.y - p2.y), 2));

	return dblDistance;
}

LRESULT CInspectAdminViewHideDlg::OnEventPcbModelChanaged(WPARAM wParam, LPARAM lParam)
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
LRESULT CInspectAdminViewHideDlg::OnEventFrameInspectDone(WPARAM wParam, LPARAM lParam)
{
	m_iInspectingFrameIndex = (int)wParam;

	return 0;
}

BOOL CInspectAdminViewHideDlg::PreTranslateMessage(MSG* pMsg) 
{
	switch(pMsg->message) {
	case WM_NCLBUTTONDOWN :
	case WM_NCRBUTTONDOWN :
		return TRUE;
	case WM_KEYDOWN:
		if((int)pMsg->wParam == VK_ESCAPE || (int)pMsg->wParam == VK_RETURN)
		{
			return TRUE;
		}
	case WM_SYSKEYDOWN:
		if(pMsg->wParam == VK_F4)
			return TRUE;
	}

	return CDialog::PreTranslateMessage(pMsg);
}

// Scan Select ComboBox
int CInspectAdminViewHideDlg::Get_CurScanTab()
{
	return m_ViewToolbar.Get_CurComboBox();
}

void CInspectAdminViewHideDlg::Set_CurScanTab(int nScanNum)
{ 
	CString strScan;
	strScan.Format("SCAN_%d", nScanNum);
	m_ViewToolbar.Set_CurComboBox(strScan);
}

void CInspectAdminViewHideDlg::Add_ScanTab(int nScanNum)
{
	CString strScan;
	strScan.Format("SCAN_%d", nScanNum);
	m_ViewToolbar.Add_ComboBox(strScan);
}

void CInspectAdminViewHideDlg::Clear_ScanTab()
{
	m_ViewToolbar.Clear_ComboBox();
}

int CInspectAdminViewHideDlg::Get_ScanTabCount()
{
	return m_ViewToolbar.Get_ComboBoxCount();
}

int CInspectAdminViewHideDlg::Get_ScanTabIndex()
{
	return m_ViewToolbar.Get_ComboBoxIndex();
}

void CInspectAdminViewHideDlg::Set_ScanTabIndex(int nIdx)
{
	m_ViewToolbar.Set_ComboBoxIndex(nIdx);
}

BOOL CInspectAdminViewHideDlg::OnInitDialog() 
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
	m_bInspectRunDisplay = FALSE;
	m_bDisplayImage = TRUE;

	Reset();

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

BOOL CInspectAdminViewHideDlg::OnEraseBkgnd(CDC *pDC)
{
	pDC->FillSolidRect(m_ClientRect, TS_COLOR_EXTRA_BG);
	return TRUE;
}



void CInspectAdminViewHideDlg::OnPaint() 
{
	CPaintDC dc(this); // device context for painting



	// Do not call CDialog::OnPaint() for painting messages
}

void CInspectAdminViewHideDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	if (HideWindowHandle < 0 || !m_bDisplayImage) return;

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

void CInspectAdminViewHideDlg::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	if (HideWindowHandle < 0 || !m_bDisplayImage) return;

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

void CInspectAdminViewHideDlg::OnLButtonDown(UINT nFlags, CPoint point) 
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

	switch (m_ToolBarState) 
	{
	case TS_DRAW:
		{
			if(THEAPP.m_iModeSwitch!=MODE_ADMIN_TEACH_VIEW){return;}
			if(THEAPP.m_pTabControlDlg->m_TabControl.GetCurSel()!=0){return;}

			mpActiveTRegion = new GTRegion;		// 엑티브 리전 생성.
			if (mpActiveTRegion) 
			{
				mpActiveTRegion->SetLineColor(RGB(255,0,0));
				mpActiveTRegion->SetLTPoint(IPoint, THEAPP.m_pCalDataService);
				mpActiveTRegion->SetRBPoint(IPoint, THEAPP.m_pCalDataService);
				mpActiveTRegion->SetSelect(TRUE, FALSE);
				DrawActiveTRegion();
			}
			break;
		}

	case TS_MOVE:
		{
			m_bOnMoving = TRUE;
			PostMessage(WM_SETCURSOR);
			break;
		}

	case TS_SELECT_PART:
		{
			mpSelectPartTRegion = new CSelectPartRegion;
			if (mpSelectPartTRegion) 
			{
				mpSelectPartTRegion->SetLTPoint(IPoint, THEAPP.m_pCalDataService);
				mpSelectPartTRegion->SetRBPoint(IPoint, THEAPP.m_pCalDataService);

				mpSelectPartTRegion->SetSelect(TRUE, FALSE);
				DrawSelectPartTRegion();
			}
			break;
		}
	}
	CDialog::OnLButtonDown(nFlags, point);
}

void CInspectAdminViewHideDlg::OnLButtonUp(UINT nFlags, CPoint point) 
{
	if (!m_bDisplayImage) return;

	ReleaseCapture();

	DPOINT dLTPoint, dRBPoint;
	POINT WHPoint;
	// 	POINT LTPoint;
	// 	BOOL bRet;



	switch (m_ToolBarState) {
	case TS_DRAW:
		{
			if(THEAPP.m_iModeSwitch!=MODE_ADMIN_TEACH_VIEW){return;}
			if(THEAPP.m_pTabControlDlg->m_TabControl.GetCurSel()!=0){return;}
			mpActiveTRegion->GetLTPointD(&dLTPoint, THEAPP.m_pCalDataService);
			mpActiveTRegion->GetRBPointD(&dRBPoint, THEAPP.m_pCalDataService);

			SAFE_DELETE(mpActiveTRegion);
			CropLTPoint.x = (long)dLTPoint.x;
			CropLTPoint.y = (long)dLTPoint.y;
			CropRBPoint.x = (long)dRBPoint.x;
			CropRBPoint.y = (long)dRBPoint.y;

			m_dLTPoint.x = dLTPoint.x;
			m_dLTPoint.y = dLTPoint.y;
			m_dRBPoint.x = dRBPoint.x;
			m_dRBPoint.y = dRBPoint.y;


			crop_rectangle1(m_pHImage, &m_HCropMatchingImage, CropLTPoint.y, CropLTPoint.x, CropRBPoint.y, CropRBPoint.x);

			break;
		}
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

void CInspectAdminViewHideDlg::OnMouseMove(UINT nFlags, CPoint point) 
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
	OutTxt.Format("Strip View [Top Camera] - (%d, %d), Zoom(%d%%)", (int)((point.x/m_dZoomRatio)+m_ViewRect.left), (int)((point.y/m_dZoomRatio)+m_ViewRect.top), (int)(m_dZoomRatio*100.));
	SetWindowText((LPCTSTR)OutTxt);

	int iWidth, iHeight;
	long HCurPos, VCurPos;
	CPoint MVDiffPoint, MIDiffPoint;

	if (IS_SETFLAG(nFlags, MK_LBUTTON)) {
		switch (m_ToolBarState) {
		case TS_DRAW:
			{
				if(THEAPP.m_iModeSwitch!=MODE_ADMIN_TEACH_VIEW){return;}
				if(THEAPP.m_pTabControlDlg->m_TabControl.GetCurSel()!=0){return;}
				DrawActiveTRegion();
				mpActiveTRegion->SetRBPoint(IPoint, THEAPP.m_pCalDataService);
				DrawActiveTRegion();
				break;
			}
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

			set_part(HideWindowHandle, m_ViewportManager.miStartYPos, m_ViewportManager.miStartXPos, m_ViewportManager.miStartYPos + iHeight, m_ViewportManager.miStartXPos + iWidth);

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

void CInspectAdminViewHideDlg::OnDestroy() 
{
	CDialog::OnDestroy();

	SAFE_DELETE(mpActiveTRegion);



	if(HideWindowHandle > 0)
	{
		close_window(HideWindowHandle);
		HideWindowHandle = -1;
	}
}

void CInspectAdminViewHideDlg::OnSize(UINT nType, int cx, int cy) 
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

BOOL CInspectAdminViewHideDlg::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt) 
{
	if (zDelta <= 0) SendMessage(WM_VSCROLL, SB_PAGEDOWN, 0);
	else SendMessage(WM_VSCROLL, SB_PAGEUP, 0);

	return CDialog::OnMouseWheel(nFlags, zDelta, pt);
}

BOOL CInspectAdminViewHideDlg::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message) 
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

void CInspectAdminViewHideDlg::Reset()
{
	InitViewRect();
	UpdateViewportManager();

//	OnTbFitHeight();

	m_iInspectingFrameIndex = -1;
	m_iDefectIdx = -1;
}

void CInspectAdminViewHideDlg::InitViewRect() 
{
	m_ViewRect.left = 0;
	m_ViewRect.top = 0;
	m_ViewRect.bottom = (int)(RECTHEIGHT(&m_ClientRect) / m_dZoomRatio);
	m_ViewRect.right = (int)(RECTWIDTH(&m_ClientRect) / m_dZoomRatio);
}

void CInspectAdminViewHideDlg::UpdateViewportManager()
{
	m_ViewportManager.miStartXPos = m_ViewRect.left;
	m_ViewportManager.miStartYPos = m_ViewRect.top;
	m_ViewportManager.miViewWidth = (int)RECTWIDTH(&m_ClientRect);
	m_ViewportManager.miViewHeight = (int)RECTHEIGHT(&m_ClientRect);
	m_ViewportManager.miImageWidth = m_iImageWidth;
	m_ViewportManager.miImageHeight = m_iImageHeight;
	m_ViewportManager.mdZoomRatio = m_dZoomRatio;

	if (HideWindowHandle>0) {
		set_part(HideWindowHandle, m_ViewRect.top, m_ViewRect.left, m_ViewRect.bottom, m_ViewRect.right);
		set_window_extents(HideWindowHandle, 0, 0, m_ClientRect.Width(), m_ClientRect.Height());
	}
}

void CInspectAdminViewHideDlg::OverlaySetViewportManager(int iLeft, int iTop, double dZoomRatio)
{
	m_ViewRect.left = iLeft;
	m_ViewRect.top = iTop;
	m_dZoomRatio = dZoomRatio;
	ViewRectSet();

	m_ViewportManager.miStartXPos = m_ViewRect.left;
	m_ViewportManager.miStartYPos = m_ViewRect.top;
	m_ViewportManager.miViewWidth = (int)RECTWIDTH(&m_ClientRect);
	m_ViewportManager.miViewHeight = (int)RECTHEIGHT(&m_ClientRect);
	m_ViewportManager.miImageWidth = m_iImageWidth;
	m_ViewportManager.miImageHeight = m_iImageHeight;
	m_ViewportManager.mdZoomRatio = m_dZoomRatio;

	if (HideWindowHandle>0) {
		set_part(HideWindowHandle, m_ViewRect.top, m_ViewRect.left, m_ViewRect.bottom, m_ViewRect.right);
		set_window_extents(HideWindowHandle, 0, 0, m_ClientRect.Width(), m_ClientRect.Height());
	}
}

void CInspectAdminViewHideDlg::ViewRectSet() 
{
	m_ViewRect.bottom = m_ViewRect.top + (int)(RECTHEIGHT(&m_ClientRect) / m_dZoomRatio);
	m_ViewRect.right = m_ViewRect.left + (int)(RECTWIDTH(&m_ClientRect) / m_dZoomRatio);
}

void CInspectAdminViewHideDlg::ScrollBarSet()
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

void CInspectAdminViewHideDlg::ScrollBarPosSet()
{
	m_VScrBar.SetWindowPos(&wndTop, m_ClientRect.right, m_ClientRect.top, m_iScrBarWidth, m_ClientRect.bottom-m_iToolBarOffset, SWP_SHOWWINDOW);
	m_VScrBar.ShowScrollBar(TRUE);
	m_HScrBar.SetWindowPos(&wndTop, m_ClientRect.left, m_ClientRect.bottom, m_ClientRect.right, m_iScrBarWidth, SWP_SHOWWINDOW);
	m_HScrBar.ShowScrollBar(TRUE);
}

void CInspectAdminViewHideDlg::DrawActiveTRegion()
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

void CInspectAdminViewHideDlg::DrawSelectPartTRegion()
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

void CInspectAdminViewHideDlg::ZoomInSelectedPart(double dZoomRatio, int iStartPosX, int iStartPosY) 
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
		OutTxt.Format("Strip View [Top Camera] - Zoom(%d%%)", (int)(m_dZoomRatio * 100.));
		SetWindowText(OutTxt);

		InvalidateRect(&m_ClientRect, TRUE);
	}	
}

void CInspectAdminViewHideDlg::OnTbDraw() 
{
	m_ToolBarState = TS_DRAW;
}

void CInspectAdminViewHideDlg::OnTbMove() 
{
	m_ToolBarState = TS_MOVE;
}

void CInspectAdminViewHideDlg::OnTbSelectPart() 
{
	m_ToolBarState = TS_SELECT_PART;
}

void CInspectAdminViewHideDlg::OnTbZoomIn() 
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
		OutTxt.Format("Strip View [Top Camera] - Zoom(%d%%)", (int)(m_dZoomRatio*100.));
		SetWindowText(OutTxt);

		InvalidateRect(&m_ClientRect, TRUE);
	}		
}

void CInspectAdminViewHideDlg::OnTbZoomOut() 
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
		OutTxt.Format("Strip View [Top Camera] - Zoom(%d%%)", (int)(m_dZoomRatio*100.));
		SetWindowText(OutTxt);

		InvalidateRect(&m_ClientRect, TRUE);
	}		
}

void CInspectAdminViewHideDlg::OnTbFitWidth() 
{
	if (m_bDisplayImage) {
		m_dZoomRatio = (double)RECTWIDTH(&m_ClientRect) / (double)m_iImageWidth;

		InitViewRect();
		UpdateViewportManager();	// CSKIM
		ScrollBarSet();

		m_ViewportManager.mdZoomRatio = m_dZoomRatio;
		UpdateViewportManager();

		CString OutTxt;
		OutTxt.Format("Strip View [Top Camera] - Zoom(%d%%)", (int)(m_dZoomRatio * 100.));
		SetWindowText(OutTxt);

		InvalidateRect(&m_ClientRect, TRUE);
	}			
}

void CInspectAdminViewHideDlg::OnTbFitHeight() 
{
	if (m_bDisplayImage) {
		m_dZoomRatio = (double)RECTHEIGHT(&m_ClientRect) / (double)m_iImageHeight;

		InitViewRect();
		UpdateViewportManager();	// CSKIM
		ScrollBarSet();

		m_ViewportManager.mdZoomRatio = m_dZoomRatio;
		UpdateViewportManager();

		CString OutTxt;
		OutTxt.Format("Strip View [Top Camera] - Zoom(%d%%)", (int)(m_dZoomRatio * 100.));
		SetWindowText(OutTxt);

		InvalidateRect(&m_ClientRect, TRUE);
	}			
}

BOOL CInspectAdminViewHideDlg::ShowWindow(int nCmdShow)
{
	return CDialog::ShowWindow(nCmdShow);
}

void CInspectAdminViewHideDlg::InitializeToolBar()
{
	CToolBarCtrl &toolbarctrl = m_ViewToolbar.GetToolBarCtrl();
	toolbarctrl.CheckButton(ID_TB_SELECT_PART, TRUE);
	m_ToolBarState = TS_SELECT_PART;
}

int CInspectAdminViewHideDlg::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CDialog::OnCreate(lpCreateStruct) == -1) return -1;

	//HTuple tWnd = (INT)(m_hWnd) ;
	//new_extern_window(tWnd, 0, 0, m_iImageWidth, m_iImageHeight, &HideWindowHandle) ;
	
	//open_window(0, 0, m_iImageWidth, m_iImageHeight, 0, "buffer", "opengl", &HideWindowHandle);
	GetClientRect(&m_ClientRect);
	open_window(0, 0, m_ClientRect.right - m_ClientRect.left + 1, m_ClientRect.bottom - m_ClientRect.top + 1, 0, "buffer", "opengl", &HideWindowHandle);
	
	return 0;
}

LRESULT CInspectAdminViewHideDlg::On2DFrameReady(WPARAM wParam, LPARAM lParam)
{

	return 0;
}

LRESULT CInspectAdminViewHideDlg::OnEventGoToDefect(WPARAM wParam, LPARAM lParam)	
{


	InvalidateRect(&m_ClientRect, TRUE);
	return 0;
}

void CInspectAdminViewHideDlg::LoadScanImage(int nScanNum) 
{

}

void CInspectAdminViewHideDlg::MoveToDefectArea()
{

}

void CInspectAdminViewHideDlg::RedrawView()
{
	InvalidateRect(NULL, FALSE);
}

void CInspectAdminViewHideDlg::RedrawRect()
{
	InvalidateRect(&m_ClientRect, FALSE);
}

BOOL CInspectAdminViewHideDlg::SelectAllPackage()
{
	return FALSE;
}

BOOL CInspectAdminViewHideDlg::SelectPackageChange() 
{
	return FALSE;
}

BOOL CInspectAdminViewHideDlg::SelectPackageUndo() 
{
	return FALSE;
}

void CInspectAdminViewHideDlg::UnSelectAll()
{

}
void CInspectAdminViewHideDlg::UpdateThresRegion(int Min,int Max)
{

}
void CInspectAdminViewHideDlg::DrawMatchingROI(int iLowVal, int iHighVal)
{

}
void CInspectAdminViewHideDlg::DrawFrameActiveTRegion()
{
	CDC *pDC = GetDC();

	POINT ClientOffset;
	ClientOffset.x = 0;
	ClientOffset.y = m_iToolBarOffset;
	pDC->SetViewportOrg(ClientOffset);

	CRgn ClipRgn;

	if (mpActiveTRegion) {
		ClipRgn.CreateRectRgn(m_ClientRect.left ,m_ClientRect.top, m_ClientRect.right, m_ClientRect.bottom);
		pDC->SelectClipRgn(&ClipRgn);
		mpActiveTRegion->Draw(pDC->m_hDC, &m_ViewportManager, GTR_DS_ACTIVE, FALSE, THEAPP.m_pCalDataService);
	}
	ReleaseDC(pDC);
}
void CInspectAdminViewHideDlg::OnTbLive()
{
	return;

#ifdef INLINE_MODE
	if(m_bLive==FALSE)
	{
		SetTimer(0,100,NULL);
		m_bLive = TRUE;
	}
	else
	{
		KillTimer(0);
		m_bLive = FALSE;
	}
#endif
}

void CInspectAdminViewHideDlg::OnTimer(UINT_PTR nIDEvent)
{
	CDialog::OnTimer(nIDEvent);
}

void CInspectAdminViewHideDlg::set_display_font (Halcon::HTuple WindowHandle, Halcon::HTuple Size, Halcon::HTuple Font, 
	Halcon::HTuple Bold, Halcon::HTuple Slant)
{

	// Local control variables 
	HTuple  OS, Exception, AllowedFontSizes, Distances;
	HTuple  Indices, Result;


	get_system("operating_system", &OS);
	if ((Size==HTuple())||(Size==(-1)))
	{
		Size = 16;
	}
	if ((OS.Substring(0,2))==HTuple("Win"))
	{
		//set font on Windows systems
		if (((Font==HTuple("mono"))||(Font==HTuple("Courier")))||(Font==HTuple("courier")))
		{
			Font = "Courier New";
		}
		else if (Font==HTuple("sans"))
		{
			Font = "Arial";
		}
		else if (Font==HTuple("serif"))
		{
			Font = "Times New Roman";
		}
		if (Bold==HTuple("true"))
		{
			Bold = 1;
		}
		else if (Bold==HTuple("false"))
		{
			Bold = 0;
		}
		else
		{
			Exception = "Wrong value of control parameter Bold";
			throw HException(Exception);
		}
		if (Slant==HTuple("true"))
		{
			Slant = 1;
		}
		else if (Slant==HTuple("false"))
		{
			Slant = 0;
		}
		else
		{
			Exception = "Wrong value of control parameter Slant";
			throw HException(Exception);
		}
		try
		{
			set_font(WindowHandle, ((((((("-"+Font)+"-")+Size)+"-*-")+Slant)+"-*-*-")+Bold)+"-");
		}
		// catch (Exception) 
		catch (HException &HDevExpDefaultException)
		{
			HDevExpDefaultException.ToHTuple(&Exception);
			throw HException(Exception);
		}
	}
	else
	{
		//set font for UNIX systems
		Size = Size*1.25;
		AllowedFontSizes.Reset();
		AllowedFontSizes.Append((((HTuple(11).Concat(14)).Concat(17)).Concat(20)).Concat(25));
		AllowedFontSizes.Append(34);
		if ((AllowedFontSizes.Find(Size))==(-1))
		{
			Distances = (AllowedFontSizes-Size).Abs();
			tuple_sort_index(Distances, &Indices);
			Size = AllowedFontSizes[HTuple(Indices[0])];
		}
		if ((Font==HTuple("mono"))||(Font==HTuple("Courier")))
		{
			Font = "courier";
		}
		else if (Font==HTuple("sans"))
		{
			Font = "helvetica";
		}
		else if (Font==HTuple("serif"))
		{
			Font = "times";
		}
		if (Bold==HTuple("true"))
		{
			Bold = "bold";
		}
		else if (Bold==HTuple("false"))
		{
			Bold = "medium";
		}
		else
		{
			Exception = "Wrong value of control parameter Bold";
			throw HException(Exception);
		}
		if (Slant==HTuple("true"))
		{
			Slant = "i";
		}
		else if (Slant==HTuple("false"))
		{
			Slant = "r";
		}
		else
		{
			Exception = "Wrong value of control parameter Slant";
			throw HException(Exception);
		}
		Result = ((((((("-*-"+Font)+"-")+Bold)+"-")+Slant)+"-normal-*-")+Size)+"-*-*-*-*-*-iso8859-1";
		try
		{
			set_font(WindowHandle, ((((((("-*-"+Font)+"-")+Bold)+"-")+Slant)+"-normal-*-")+Size)+"-*-*-*-*-*-iso8859-1");
		}
		// catch (Exception) 
		catch (HException &HDevExpDefaultException)
		{
			HDevExpDefaultException.ToHTuple(&Exception);
			throw HException(Exception);
		}
	}
	return;
}

// HDefectRegion: 알고리즘에서 찾은 불량 영역 전체

int CInspectAdminViewHideDlg::WriteResultFileExtra(int iImageNo, Hobject HImgRef, Hobject HDefectRegion, Hobject HROIInspectAreaRgn, CString sBarcodeResult, CString* pStrDefectPosition, double* pdblArea, double *pdDefectCenterX, double *pdDefectCenterY)
{
	try{
		int iDefectType = DEFECT_TYPE_GOOD;		// 양품(불량 no counting)

		double tInspectStart_total, tInspectEnd_total;
		tInspectStart_total = GetTickCount();

		CDC* pDC = GetDC();
		HTuple lDC = (INT)(pDC->m_hDC);
		clear_window(HideWindowHandle);

		CString strLensTopBottom = _T("");
		double	dblArea = 0.0;

		disp_obj(HImgRef, HideWindowHandle);
		set_line_style(HideWindowHandle, HTuple());
		set_line_width(HideWindowHandle, 1);
		set_color(HideWindowHandle, "blue");
		set_draw(HideWindowHandle, "margin");

		copy_obj(HDefectRegion, &m_HResultDefectRgn, 1, -1);

		// m_HResultDefectRgn: 알고리즘에서 찾은 불량 영역 전체 (Barrel or Lens)
		

		///////////** 모듈의 검사결과를 그려준다 **///////////////////////////////////////////
		CString tempstr; tempstr.Format("WriteResultFileExtra %d 들어옴", m_iModuleNo); THEAPP.SaveLog(tempstr);

		Hobject DefectRegion;
		double dDefectCenterX, dDefectCenterY;

		int iImageIdx;

		if (THEAPP.m_pGFunction->ValidHRegion(m_HResultDefectRgn))		// 알고리즘으로 검출된 Defect가 있으면
		{
			copy_obj(m_HResultDefectRgn, &DefectRegion, 1, -1);

			Hobject SelectRgn;
			HTuple OriSize, OriArea, Row, Column;
			HTuple CountDefect;
			Hobject ConnectionsRegion;

			connection(DefectRegion, &ConnectionsRegion);
			count_obj(ConnectionsRegion, &CountDefect);		// Total Defect Blobs

			set_display_font(HideWindowHandle, OVERLAY_IMAGE_EXTRA_FONT_SIZE + THEAPP.Struct_PreferenceStruct.m_iResultTextSize, "mono", "true", "false");
			set_color(HideWindowHandle, "green");

			// ************ Defect 이 겹치게 그려짐을 막기 위해 Flag 배열을 만듬 ********
			int* nDefectFlag = new int[CountDefect[0].I()];

			// 그린 것에 대해 Flag 를 on 시킴
			for (int i = 0; i < CountDefect[0].I(); i++)
			{
				nDefectFlag[i] = 0;
			}

			int nRow = 0, nColumn = 0;
			int nCompRow = 0, nCompColumn = 0;

			Point pt1, pt2, DefectCenterPos;
			double dMeanGV1, dMeanGV2, dTemp1, dTemp2;
			int iDispGV;

			pt1.x = 0, pt1.y = 0;
			pt2.x = 0, pt2.y = 0;

			double dblDist = 0.0;
			HTuple CompSize, CompArea, CompRow, CompColumn;

			for (int i = 0; i < CountDefect[0].I(); i++)
			{
				select_obj(ConnectionsRegion, &SelectRgn, i + 1);

				min_max_gray(SelectRgn, HImgRef, 0, &dMeanGV1, &dTemp1, &dTemp2);

				area_center(SelectRgn, &OriSize, &Row, &Column);

				dDefectCenterX = Column[0].D();
				dDefectCenterY = Row[0].D();

				Row += 5; Column += 5;

				nRow = Row[0].I();
				nColumn = Column[0].I();

				pt1.x = nRow; pt1.y = nColumn;

				for (int j = 0; j < CountDefect[0].I(); j++)
				{
					if (i != j)
					{
						select_obj(ConnectionsRegion, &SelectRgn, j + 1);

						min_max_gray(SelectRgn, HImgRef, 0, &dMeanGV2, &dTemp1, &dTemp2);

						area_center(SelectRgn, &CompSize, &CompRow, &CompColumn);
						CompRow += 5; CompColumn += 5;

						nCompRow = CompRow[0].I();
						nCompColumn = CompColumn[0].I();

						pt2.x = nCompRow; pt2.y = nCompColumn;

						DefectCenterPos.x = pt2.x - 5;
						DefectCenterPos.y = pt2.y - 5;

						dblDist = Distance(pt1, pt2);

						// 중심 간의 거리가 설정 값 보다 작다면
						if (dblDist < THEAPP.Struct_PreferenceStruct.m_iDefectDispDist)
						{
							// 어떠한 것도 string 을 쓰지 않았다면
							if (nDefectFlag[j] == 0)
							{
								// 좌측으로 이동시 0 이하의 값이 나오면
								if ((CompRow - THEAPP.Struct_PreferenceStruct.m_iDefectDispDist) > 0)
									CompRow = CompRow - THEAPP.Struct_PreferenceStruct.m_iDefectDispDist;
								else
									CompRow = 0;

								set_tposition(HideWindowHandle, CompRow, CompColumn);
								//CompArea = CompSize[0].D() * PXLAREA /1000000;
								CompArea = ceil((CompSize[0].D() * PXLAREA) / 1000) / 1000;
								set_color(HideWindowHandle, "green");

								iDispGV = (int)dMeanGV2;

								write_string(HideWindowHandle, CompArea + "mm2/" + "(" + DefectCenterPos.y + "," + DefectCenterPos.x + ")/" + iDispGV);

								nDefectFlag[j] = 1;
							}
						}
					}
				}

				// 어떠한 것도 string 을 쓰지 않았다면 
				if (nDefectFlag[i] == 0)
				{
					set_tposition(HideWindowHandle, Row, Column);
					OriArea = ceil((OriSize[0].D() * PXLAREA) / 1000) / 1000;
					set_color(HideWindowHandle, "green");

					DefectCenterPos.x = pt1.x - 5;
					DefectCenterPos.y = pt1.y - 5;

					iDispGV = (int)dMeanGV1;

					set_display_font(HideWindowHandle, 12, "mono", "true", "false");

					write_string(HideWindowHandle, OriArea + "mm2/" + "(" + DefectCenterPos.y + "," + DefectCenterPos.x + ")/" + iDispGV);

					set_display_font(HideWindowHandle, OVERLAY_IMAGE_EXTRA_FONT_SIZE + THEAPP.Struct_PreferenceStruct.m_iResultTextSize, "mono", "true", "false");
					set_color(HideWindowHandle, "green");

					nDefectFlag[i] = 1;
				}

				// ********* 가장 큰 Area 만 뽑는다. *******

				if (OriArea[0].D() > dblArea && OriArea[0].D() > *pdblArea)
				{
					dblArea = OriArea[0].D();
					*pdblArea = dblArea;
					*pdDefectCenterX = dDefectCenterX;
					*pdDefectCenterY = dDefectCenterY;
				}
				// ******************************************
			}

			delete(nDefectFlag);

			// Display

			// m_HResultDefectRgn: Final Result (All Defects)
			union1(DefectRegion, &m_HResultDefectRgn);

			///////////////// Changed for CMI3000 2000 ====> ///////////////////////////////////////////////////

			set_color(HideWindowHandle, "red");
			set_draw(HideWindowHandle, "margin");
			disp_obj(HDefectRegion, HideWindowHandle);		// 모든 알고리즘 검출 Defect 디스플레이: 불량으로 검출 안된 영역도 표시함...


			// Extra 검사 불량 분리 - LeeGW
			Hobject HDisplayEpoxyHoleRgn;
			Hobject HDisplaySpringNGRgn;

			gen_empty_obj(&HDisplayEpoxyHoleRgn);
			gen_empty_obj(&HDisplaySpringNGRgn);

			m_iInspectResult = IR_GOOD;

			if (THEAPP.m_pGFunction->ValidHRegion(m_HResultDefectRgn))
			{

				// Sidefill Epoxy Hole 검사
				if (THEAPP.m_pGFunction->ValidHRegion(m_HRgnEpoxyHole))
				{
					intersection(m_HResultDefectRgn, m_HRgnEpoxyHole, &HDisplayEpoxyHoleRgn);

					if (THEAPP.m_pGFunction->ValidHRegion(HDisplayEpoxyHoleRgn) == TRUE)
					{
						disp_obj(HDisplayEpoxyHoleRgn, HideWindowHandle);

						//Multiple Defect
						copy_obj(HDisplayEpoxyHoleRgn,
							&THEAPP.m_stMultipleDefectInfo[THEAPP.m_stVirtualMagazineNo.GetVirtualMagazineNo(THEAPP.m_pInspectService->m_sLotID_H)][DEFECT_NAME_EPOXYHOLE].m_HMultipleDefectRgn[THEAPP.m_pInspectService->m_iTrayNo_H - 1][m_iModuleNo - 1],
							1, -1); 

						m_iInspectResult = IR_EPOXYHOLE;
					}
				}

				// SpringNG 검사
				if (THEAPP.m_pGFunction->ValidHRegion(m_HRgnNorthSpringNG))
				{
					intersection(m_HResultDefectRgn, m_HRgnNorthSpringNG, &HDisplaySpringNGRgn);

					if (THEAPP.m_pGFunction->ValidHRegion(HDisplaySpringNGRgn) == TRUE)
					{
						disp_obj(HDisplaySpringNGRgn, HideWindowHandle);

						copy_obj(HDisplaySpringNGRgn,
							&THEAPP.m_stMultipleDefectInfo[THEAPP.m_stVirtualMagazineNo.GetVirtualMagazineNo(THEAPP.m_pInspectService->m_sLotID_H)][DEFECT_NAME_NORTH_SPRINGNG].m_HMultipleDefectRgn[THEAPP.m_pInspectService->m_iTrayNo_H - 1][m_iModuleNo - 1],
							1, -1); //Multiple Defect

						THEAPP.SaveDefectFeatureLog(THEAPP.Struct_PreferenceStruct.m_bSaveLasLog, THEAPP.m_pInspectService->m_sLotID_H, THEAPP.m_pInspectService->m_iTrayNo_H, m_iModuleNo, sBarcodeResult, g_sDefectName[DEFECT_NAME_NORTH_SPRINGNG], HImgRef, HDisplaySpringNGRgn);
						m_iInspectResult = IR_SPRINGNG;
					}
				}

				if (THEAPP.m_pGFunction->ValidHRegion(m_HRgnEastSpringNG))
				{
					intersection(m_HResultDefectRgn, m_HRgnEastSpringNG, &HDisplaySpringNGRgn);

					if (THEAPP.m_pGFunction->ValidHRegion(HDisplaySpringNGRgn) == TRUE)
					{
						disp_obj(HDisplaySpringNGRgn, HideWindowHandle);

						copy_obj(HDisplaySpringNGRgn,
							&THEAPP.m_stMultipleDefectInfo[THEAPP.m_stVirtualMagazineNo.GetVirtualMagazineNo(THEAPP.m_pInspectService->m_sLotID_H)][DEFECT_NAME_EAST_SPRINGNG].m_HMultipleDefectRgn[THEAPP.m_pInspectService->m_iTrayNo_H - 1][m_iModuleNo - 1],
							1, -1); //Multiple Defect

						THEAPP.SaveDefectFeatureLog(THEAPP.Struct_PreferenceStruct.m_bSaveLasLog, THEAPP.m_pInspectService->m_sLotID_H, THEAPP.m_pInspectService->m_iTrayNo_H, m_iModuleNo, sBarcodeResult, g_sDefectName[DEFECT_NAME_EAST_SPRINGNG], HImgRef, HDisplaySpringNGRgn);
						m_iInspectResult = IR_SPRINGNG;
					}
				}

				m_bExtraDefectOKNG[m_iModuleNo - 1] = TRUE;

			}

			/////////// 결과를 text로 표기 /////////////////////////
			set_display_font(HideWindowHandle, OVERLAY_IMAGE_RESULT_FONT_SIZE + THEAPP.Struct_PreferenceStruct.m_iResultTextSize, "mono", "true", "false");
			set_color(HideWindowHandle, "red");
			set_tposition(HideWindowHandle, m_ViewRect.top + 50 + THEAPP.Struct_PreferenceStruct.m_iResultTextPosY, m_ViewRect.left + 50 + THEAPP.Struct_PreferenceStruct.m_iResultTextPosX);

			HTuple HDefectName;

			switch (m_iInspectResult)
			{
			case IR_GOOD: HDefectName = "양품"; break;
			case IR_NG: HDefectName = "불량"; break;
			case IR_SPRINGNG: HDefectName = "Spring NG"; break;
			case IR_EPOXYHOLE: HDefectName = "Epoxy Hole"; break;
			default: HDefectName = "양품"; break;
			}

			if (m_iInspectResult != IR_GOOD)
			{
				write_string(HideWindowHandle, HDefectName);
				strLensTopBottom.Format("%s", HDefectName);
				*pStrDefectPosition = strLensTopBottom;

				// SpringNG
				if (m_iInspectResult == IR_SPRINGNG)
				{
					iDefectType = DEFECT_TYPE_SPRINGNG;
					THEAPP.m_pTrayAdminViewDlg->m_TrayImagePB.OkNg[m_iModuleNo - 1] = DEFECT_TYPE_SPRINGNG;
					THEAPP.m_pTrayOperatorViewNowDlg->m_TrayImagePB.OkNg[m_iModuleNo - 1] = DEFECT_TYPE_SPRINGNG;
				}
				// Sidefill Epoxy Hole
				else if (m_iInspectResult == IR_EPOXYHOLE)
				{
					iDefectType = DEFECT_TYPE_EPOXYHOLE;
					THEAPP.m_pTrayAdminViewDlg->m_TrayImagePB.OkNg[m_iModuleNo - 1] = DEFECT_TYPE_EPOXYHOLE;
					THEAPP.m_pTrayOperatorViewNowDlg->m_TrayImagePB.OkNg[m_iModuleNo - 1] = DEFECT_TYPE_EPOXYHOLE;
				}
			}
			else	// 양품
			{
				set_color(HideWindowHandle,"blue");
				write_string(HideWindowHandle,"양품");
				THEAPP.m_pTrayAdminViewDlg->m_TrayImagePB.OkNg[m_iModuleNo-1] = DEFECT_TYPE_GOOD;
				THEAPP.m_pTrayOperatorViewNowDlg->m_TrayImagePB.OkNg[m_iModuleNo-1] = DEFECT_TYPE_GOOD;
			}

		} // if(THEAPP.m_pGFunction->ValidHRegion(m_HResultDefectRgn))		==> Sidefill Epoxy Hole 불량이 있을 때
		else		// 양품
		{
			set_display_font(HideWindowHandle, OVERLAY_IMAGE_RESULT_FONT_SIZE + THEAPP.Struct_PreferenceStruct.m_iResultTextSize, "mono", "true", "false");
			set_color(HideWindowHandle,"blue");
			set_tposition(HideWindowHandle,m_ViewRect.top+50 + THEAPP.Struct_PreferenceStruct.m_iResultTextPosY, m_ViewRect.left+50 + THEAPP.Struct_PreferenceStruct.m_iResultTextPosX);

			write_string(HideWindowHandle,"양품");

			if(!((THEAPP.m_pTrayAdminViewDlg->m_TrayImagePB.OkNg[m_iModuleNo-1]==DEFECT_TYPE_BARREL)||
				(THEAPP.m_pTrayAdminViewDlg->m_TrayImagePB.OkNg[m_iModuleNo-1]==DEFECT_TYPE_LENS)||
				(THEAPP.m_pTrayAdminViewDlg->m_TrayImagePB.OkNg[m_iModuleNo - 1] == DEFECT_TYPE_SPRINGNG)||
				(THEAPP.m_pTrayAdminViewDlg->m_TrayImagePB.OkNg[m_iModuleNo - 1] == DEFECT_TYPE_EPOXYHOLE)))
			{
				THEAPP.m_pTrayAdminViewDlg->m_TrayImagePB.OkNg[m_iModuleNo-1] = DEFECT_TYPE_GOOD;
				THEAPP.m_pTrayOperatorViewNowDlg->m_TrayImagePB.OkNg[m_iModuleNo-1] = DEFECT_TYPE_GOOD;
			}
		}

		Hobject HImgDump, HImgDump_save;
		gen_empty_obj(&HImgDump);
		gen_empty_obj(&HImgDump_save);

		Sleep(1);

		dump_window_image(&HImgDump_save, HideWindowHandle);

		SYSTEMTIME time;
		GetLocalTime(&time);

		if (THEAPP.Struct_PreferenceStruct.m_bSaveResultImage == TRUE)
		{
			CString Filename;
			if (iDefectType == DEFECT_TYPE_GOOD)
				Filename.Format("Module%d_Result_%s_%s_OK", m_iModuleNo, g_sImageName[THEAPP.m_iMachineInspType][iImageNo], sBarcodeResult);
			else
				Filename.Format("Module%d_Result_%s_%s_NG", m_iModuleNo, g_sImageName[THEAPP.m_iMachineInspType][iImageNo], sBarcodeResult);

			if (THEAPP.Struct_PreferenceStruct.m_bSaveBMP == TRUE)
				THEAPP.m_pSaveManager->SaveImage(HImgDump_save, "bmp", THEAPP.m_FileBase.m_strOverayImageFolderPrev + "\\" + Filename);
			else
				THEAPP.m_pSaveManager->SaveImage(HImgDump_save, "jpg", THEAPP.m_FileBase.m_strOverayImageFolderPrev + "\\" + Filename);
		}	

		clear_window(HideWindowHandle);
		ReleaseDC(pDC);

		CString sInspectTime;
		tInspectEnd_total = GetTickCount();

		sInspectTime.Format("* Module %2d 영상 판정 시간 Extra Inspection: %.0lf ms", m_iModuleNo, tInspectEnd_total - tInspectStart_total);
		THEAPP.SaveDetectLog(sInspectTime);
		THEAPP.SaveLog(sInspectTime);

		return iDefectType;
	}
	catch(HException &except)
	{

		CString str;
		str.Format("Halcon Exception [CInspectAdminViewHideDlg WriteResultFileExtra] : %s", except.message);
		THEAPP.SaveLog(str);

		return -1;
	}
}

// HDefectRegion: 알고리즘에서 찾은 불량 영역 전체
int CInspectAdminViewHideDlg::WriteResultFileFAI(Hobject* HImage, int iTrayNo, int iModuleNo, CString sBarcode)
{
	double tInspectStart_total, tInspectEnd_total;
	tInspectStart_total = GetTickCount();

	CDC* pDC = GetDC();
	HTuple lDC = (INT)(pDC->m_hDC);
	clear_window(HideWindowHandle);

	set_draw(HideWindowHandle, "margin");
	set_display_font(HideWindowHandle, OVERLAY_IMAGE_RESULT_FONT_SIZE + THEAPP.Struct_PreferenceStruct.m_iResultTextSize, "mono", "true", "false");

	set_line_width(HideWindowHandle, HTuple(2));
	set_line_style(HideWindowHandle, (HTuple(5).Append(3)));

	CString Filename, strFAIValue;
	Hobject HContourRgn;
	Hobject HImgDump_save;
	gen_empty_obj(&HImgDump_save);

	bIsFAISpecialNG = FALSE;
	for (int i = 0; i < MAX_FAI_ITEM; i++)
	{
		if (THEAPP.m_bUseFAI[i] == FALSE)
			continue;


		if (THEAPP.m_StructFaiMeasure[iTrayNo - 1][iModuleNo - 1].m_dFAIMeasureValue[i][0] > THEAPP.m_pModelDataManager->m_dFAISpecMax[i] ||
			THEAPP.m_StructFaiMeasure[iTrayNo - 1][iModuleNo - 1].m_dFAIMeasureValue[i][0] < THEAPP.m_pModelDataManager->m_dFAISpecMin[i])
		{
			if (THEAPP.m_pModelDataManager->m_bFAINGUse[i] == TRUE)
			{
				THEAPP.m_StructFaiMeasure[iTrayNo - 1][iModuleNo - 1].m_bFAI_ResultNG[i][0] = TRUE;
				THEAPP.m_StructFaiMeasure[iTrayNo - 1][iModuleNo - 1].m_strFAI_OKNG[i][0] = "NG";
			}
			else
			{

				THEAPP.m_StructFaiMeasure[iTrayNo - 1][iModuleNo - 1].m_bFAI_ResultNG[i][0] = FALSE;
				THEAPP.m_StructFaiMeasure[iTrayNo - 1][iModuleNo - 1].m_strFAI_OKNG[i][0] = "OK";
			}

			if (THEAPP.m_pModelDataManager->m_bFAISpecialNGUse[i] == TRUE)
				bIsFAISpecialNG = TRUE;
		}
		else
		{
			THEAPP.m_StructFaiMeasure[iTrayNo - 1][iModuleNo - 1].m_bFAI_ResultNG[i][0] = FALSE;
			THEAPP.m_StructFaiMeasure[iTrayNo - 1][iModuleNo - 1].m_strFAI_OKNG[i][0] = "OK";
		}

		for (int ii = 0; ii < MAX_ONE_FAI_MEASURE_VALUE; ii++)
		{
			if (THEAPP.Struct_FAI_ReviewInfo[i][ii].m_bSaveReview == FALSE)
				continue;

			clear_window(HideWindowHandle);
			gen_empty_obj(&HContourRgn);

			if (THEAPP.m_pGFunction->ValidHXLD(m_HReviewXLD_FAI_Item[i][ii]) && THEAPP.Struct_FAI_ReviewInfo[i][ii].m_iImageNo < THEAPP.m_iMaxInspImageNo)
			{
				THEAPP.m_pInspectAdminViewHideDlg->OverlaySetViewportManager(THEAPP.Struct_FAI_ReviewInfo[i][ii].m_iStartX, THEAPP.Struct_FAI_ReviewInfo[i][ii].m_iStartY, THEAPP.Struct_FAI_ReviewInfo[i][ii].m_fZoomRatio);
				set_tposition(HideWindowHandle, m_ViewRect.top + 50 + THEAPP.Struct_PreferenceStruct.m_iResultTextPosY, m_ViewRect.left + 50 + THEAPP.Struct_PreferenceStruct.m_iResultTextPosX);
				disp_obj(HImage[THEAPP.Struct_FAI_ReviewInfo[i][ii].m_iImageNo], HideWindowHandle);

				int iii = 0;
				for (; iii <= THEAPP.Struct_FAI_ReviewInfo[i][ii].m_iMergeCnt; iii++)
				{
					set_color(HideWindowHandle, THEAPP.Struct_FAI_ReviewInfo[i][ii+iii].m_sColor);
					disp_obj(m_HReviewXLD_FAI_Item[i][ii], HideWindowHandle);
				}

				if (THEAPP.m_StructFaiMeasure[iTrayNo - 1][iModuleNo - 1].m_strFAI_OKNG[i][0] == "OK")
					set_color(HideWindowHandle, "blue");
				else
					set_color(HideWindowHandle, "red");

				strFAIValue.Format("FAI-%d : %.8f / %s", i, THEAPP.m_StructFaiMeasure[iTrayNo - 1][iModuleNo - 1].m_dFAIMeasureValue[i][0], THEAPP.m_StructFaiMeasure[iTrayNo - 1][iModuleNo - 1].m_strFAI_OKNG[i][0]);
				write_string(HideWindowHandle, HTuple((LPSTR)(LPCSTR)strFAIValue));

				dump_window_image(&HImgDump_save, HideWindowHandle);

				if (THEAPP.Struct_PreferenceStruct.m_bSaveFAIImage == TRUE)
				{
					CString Filename;
					Filename.Format("Module%d_Result_%s_%s_%s", iModuleNo, THEAPP.Struct_FAI_ReviewInfo[i][ii].m_sFaiName, sBarcode, THEAPP.m_StructFaiMeasure[iTrayNo - 1][iModuleNo - 1].m_strFAI_OKNG[i][0]);

					if (THEAPP.Struct_PreferenceStruct.m_bSaveBMP == TRUE)
						THEAPP.m_pSaveManager->SaveImage(HImgDump_save, "bmp", THEAPP.m_FileBase.m_strFAIImageFolder + "\\" + Filename);
					else
						THEAPP.m_pSaveManager->SaveImage(HImgDump_save, "jpg", THEAPP.m_FileBase.m_strFAIImageFolder + "\\" + Filename);
				}

				ii += iii-1;
			}
		}
	}

	clear_window(HideWindowHandle);
	ReleaseDC(pDC);

	return TRUE;
}

int CInspectAdminViewHideDlg::WriteResultFile (int BarrelOrlens, Hobject HImgRef, Hobject HDefectRegion, Hobject InspectContour1, Hobject InspectContour3
	, Hobject InspectContour4, Hobject InspectContour5, CString sBarcodeResult, double dBarcodePosOffsetX, double dBarcodePosOffsetY, BOOL bBarcodeShiftResult, double dBarcodePosOffsetRotation, BOOL bBarcodeRotationResult, BOOL bBarcodeModuleMixResult
	, double dBlackCoatingDiameter, double dBlackCoatingPosX, double dBlackCoatingPosY, double dO1O2Distance, double dDiameterMin, BOOL bBlackCoatingResult
	, CString* pStrDefectPosition, double* pdblArea, double *pdDefectCenterX, double *pdDefectCenterY
	,int iExtraResult, double* pdExtraArea, double *pdExtraDefectCenterX, double *pdExtraDefectCenterY
	,BOOL bDefectState, BOOL bIsBarrelDefect, BOOL bIsLensDefect, BOOL bBarcodeError, BOOL bMatchingError, BOOL bLightDisorder, BOOL bModuleEmpty) //Barcode Rotation Log
{

	try{
		int iDefectCodeRet = DEFECT_CODE_GOOD;		// 양품(불량 no counting)

		double tInspectStart_total, tInspectEnd_total;
		tInspectStart_total = GetTickCount();

		CDC* pDC = GetDC();
		HTuple lDC = (INT)(pDC->m_hDC);
		//set_window_dc (HideWindowHandle, lDC ) ;
		clear_window(HideWindowHandle);

		disp_obj(HImgRef, HideWindowHandle);
		set_line_style(HideWindowHandle, HTuple());

		CString strLensTopBottom = _T("");
		double	dblArea = 0.0;

		Hobject HDefectRgnforLens;	// LeeGW
		copy_obj(HDefectRegion, &HDefectRgnforLens, 1, -1);	// LeeGW
		copy_obj(HDefectRegion, &m_HResultDefectRgn, 1, -1);

		// m_HResultDefectRgn: 알고리즘에서 찾은 불량 영역 전체 (Barrel or Lens)		

		///////////** 모듈의 검사결과를 그려준다 **///////////////////////////////////////////
		CString tempstr; tempstr.Format("WriteResultFile %d 들어옴", m_iModuleNo); THEAPP.SaveLog(tempstr);
		CString strImageResult;

		int iLensDefectNumber;

		Hobject DefectRegion;
		double dDefectCenterX, dDefectCenterY;

		int iImageIdx;

		if (THEAPP.m_iMachineInspType != MACHINE_WELDING)
		{
			if (THEAPP.m_pGFunction->ValidHRegion(m_HResultDefectRgn) && (bIsBarrelDefect || bIsLensDefect))		// 알고리즘으로 검출된 Defect가 있으면
			{
				strImageResult = _T("NG");

				if (bIsBarrelDefect)		// 불량영역이 베럴일 때
				{
					dblArea = *pdblArea;
					dDefectCenterY = *pdDefectCenterX;
					dDefectCenterY = *pdDefectCenterY;

					gen_empty_obj(&DefectRegion);	// Total Defects

					// m_HRgnDirt: 검사조건을 통과한 Dirt 불량
					if (iDirtDefectSortingNumber >= 0)
						concat_obj(DefectRegion, m_HRgnDirt, &DefectRegion);

					// m_HRgnScratch: 검사조건을 통과한 Scratch 불량
					if (iScratchDefectSortingNumber >= 0)
						concat_obj(DefectRegion, m_HRgnScratch, &DefectRegion);

					// m_HRgnStain: 검사조건을 통과한 Stain 불량
					if (iStainDefectSortingNumber >= 0)
						concat_obj(DefectRegion, m_HRgnStain, &DefectRegion);

					// m_HRgnDent: 검사조건을 통과한 Dent 불량
					if (iDentDefectSortingNumber >= 0)
						concat_obj(DefectRegion, m_HRgnDent, &DefectRegion);

					// m_HRgnChipping: 검사조건을 통과한 Chipping 불량
					if (iChippingDefectSortingNumber >= 0)
						concat_obj(DefectRegion, m_HRgnChipping, &DefectRegion);

					// m_HRgnEpoxy: 검사조건을 통과한 Epoxy 불량
					if (iEpoxyDefectSortingNumber >= 0)
						concat_obj(DefectRegion, m_HRgnEpoxy, &DefectRegion);

					// m_HRgnOutsideCT: 검사조건을 통과한 Res1 불량
					if (iOutsideCTDefectSortingNumber >= 0)
						concat_obj(DefectRegion, m_HRgnOutsideCT, &DefectRegion);

					// m_HRgnPeelOff: 검사조건을 통과한 Res2 불량
					if (iPeelOffDefectSortingNumber >= 0)
						concat_obj(DefectRegion, m_HRgnPeelOff, &DefectRegion);

					// m_HRgnWhiteDot: 검사조건을 통과한 WhiteDot 불량
					if (iWhiteDotDefectSortingNumber >= 0)
						concat_obj(DefectRegion, m_HRgnWhiteDot, &DefectRegion);

					Hobject SelectRgn;
					HTuple OriSize, OriArea, Row, Column;
					HTuple CountDefect;
					Hobject ConnectionsRegion;

					connection(DefectRegion, &ConnectionsRegion);
					count_obj(ConnectionsRegion, &CountDefect);		// Total Defect Blobs

					set_display_font(HideWindowHandle, 12, "mono", "true", "false");
					set_color(HideWindowHandle, "green");

					// ************ Defect 이 겹치게 그려짐을 막기 위해 Flag 배열을 만듬 ********
					int* nDefectFlag = new int[CountDefect[0].I()];

					// 그린 것에 대해 Flag 를 on 시킴
					for (int i = 0; i < CountDefect[0].I(); i++)
					{
						nDefectFlag[i] = 0;
					}

					int nRow = 0, nColumn = 0;
					int nCompRow = 0, nCompColumn = 0;

					Point pt1, pt2;
					pt1.x = 0, pt1.y = 0;
					pt2.x = 0, pt2.y = 0;

					double dblDist = 0.0;
					HTuple CompSize, CompArea, CompRow, CompColumn;

					for (int i = 0; i < CountDefect[0].I(); i++)
					{
						select_obj(ConnectionsRegion, &SelectRgn, i + 1);
						area_center(SelectRgn, &OriSize, &Row, &Column);

						dDefectCenterX = Column[0].D();
						dDefectCenterY = Row[0].D();

						Row += 5; Column += 5;

						nRow = Row[0].I();
						nColumn = Column[0].I();

						pt1.x = nRow; pt1.y = nColumn;

						for (int j = 0; j < CountDefect[0].I(); j++)
						{
							if (i != j)
							{
								select_obj(ConnectionsRegion, &SelectRgn, j + 1);
								area_center(SelectRgn, &CompSize, &CompRow, &CompColumn);
								CompRow += 5; CompColumn += 5;

								nCompRow = CompRow[0].I();
								nCompColumn = CompColumn[0].I();

								pt2.x = nCompRow; pt2.y = nCompColumn;

								dblDist = Distance(pt1, pt2);

								// 중심 간의 거리가 설정 값 보다 작다면
								if (dblDist < THEAPP.Struct_PreferenceStruct.m_iDefectDispDist)
								{
									// 어떠한 것도 string 을 쓰지 않았다면
									if (nDefectFlag[j] == 0)
									{
										// 좌측으로 이동시 0 이하의 값이 나오면
										if ((CompRow - THEAPP.Struct_PreferenceStruct.m_iDefectDispDist) > 0)
											CompRow = CompRow - THEAPP.Struct_PreferenceStruct.m_iDefectDispDist;
										else
											CompRow = 0;

										set_tposition(HideWindowHandle, CompRow, CompColumn);
										CompArea = ceil((CompSize[0].D() * PXLAREA) / 1000) / 1000;
										set_color(HideWindowHandle, "green");
										write_string(HideWindowHandle, CompArea + "mm2");

										nDefectFlag[j] = 1;
									}
								}
							}
						}

						// 어떠한 것도 string 을 쓰지 않았다면 
						if (nDefectFlag[i] == 0)
						{
							set_tposition(HideWindowHandle, Row, Column);
							OriArea = ceil((OriSize[0].D() * PXLAREA) / 1000) / 1000;
							set_color(HideWindowHandle, "green");
							write_string(HideWindowHandle, OriArea + "mm2");

							nDefectFlag[i] = 1;
						}

						// ********* 가장 큰 Area 만 뽑는다. *******
						if (OriArea[0].D() > dblArea)
						{
							dblArea = OriArea[0].D();
							*pdblArea = dblArea;
							*pdDefectCenterX = dDefectCenterX;
							*pdDefectCenterY = dDefectCenterY;
						}
						// ******************************************
					}

					delete(nDefectFlag);

					// Display
					set_color(HideWindowHandle, "slate blue");
					set_draw(HideWindowHandle, "margin");
					set_line_width(HideWindowHandle, 1);

					disp_obj(HDefectRegion, HideWindowHandle);		// 모든 알고리즘 검출 Defect 디스플레이: 불량으로 검출 안된 영역도 표시함...

					CString strImageFileName;

					Hobject HDisplayDirtRgn;
					Hobject HDisplayScratchRgn;
					Hobject HDisplayStainRgn;
					Hobject HDisplayDentRgn;
					Hobject HDisplayChippingRgn;
					Hobject HDisplayEpoxyRgn;
					Hobject HDisplayOutsideCTRgn;
					Hobject HDisplayPeelOffRgn;
					Hobject HDisplayWhiteDotRgn;	// White Dot - LeeGW	
					DWORD color;

					gen_empty_obj(&HDisplayDirtRgn);
					gen_empty_obj(&HDisplayScratchRgn);
					gen_empty_obj(&HDisplayStainRgn);
					gen_empty_obj(&HDisplayDentRgn);
					gen_empty_obj(&HDisplayChippingRgn);
					gen_empty_obj(&HDisplayEpoxyRgn);
					gen_empty_obj(&HDisplayOutsideCTRgn);
					gen_empty_obj(&HDisplayPeelOffRgn);
					gen_empty_obj(&HDisplayWhiteDotRgn);

					m_iInspectResult = IR_GOOD;
					m_iDirt = IR_GOOD;
					m_iScratch = IR_GOOD;
					m_iStain = IR_GOOD;
					m_iDent = IR_GOOD;
					m_iChipping = IR_GOOD;
					m_iEpoxy = IR_GOOD;
					m_iOutsideCT = IR_GOOD;
					m_iPeelOff = IR_GOOD;
					m_iWhiteDot = IR_GOOD;	// White Dot - LeeGW

					if (THEAPP.m_pGFunction->ValidHRegion(m_HResultDefectRgn) && THEAPP.m_pGFunction->ValidHRegion(m_HRgnDirt))
					{
						intersection(m_HResultDefectRgn, m_HRgnDirt, &HDisplayDirtRgn);

						if (THEAPP.m_pGFunction->ValidHRegion(HDisplayDirtRgn) == TRUE)
						{
							color = THEAPP.Struct_PreferenceStruct.lNGColor[DEFECT_NAME_DIRT];
							set_rgb(HideWindowHandle, GetRValue(color), GetGValue(color), GetBValue(color));
							disp_obj(HDisplayDirtRgn, HideWindowHandle);

							if (iDirtDefectSortingNumber > 0)
							{
								m_iInspectResult = IR_DIRT;
								m_iDirt = IR_DIRT;		// Bug Fixed

								// 파라미터 로그 저장
								auto& vec = THEAPP.m_vDetectParamInfo[THEAPP.m_pInspectService->m_iTrayNo_H - 1][m_iModuleNo - 1][DEFECT_NAME_DIRT];
								if (!vec.empty())
								{
									vec.erase(remove_if(vec.begin(), vec.end(),
										[&](const auto& param) {
											Hobject HDetectParamRgn;
											intersection(HDisplayDirtRgn, param.HRegion, &HDetectParamRgn);
											if (THEAPP.m_pGFunction->ValidHRegion(HDetectParamRgn) == TRUE)
											{
												THEAPP.SaveDefectParamLog(THEAPP.Struct_PreferenceStruct.m_bSaveLasLog, param.bBarrelLens, THEAPP.m_pInspectService->m_sLotID_H, THEAPP.m_pInspectService->m_iTrayNo_H, m_iModuleNo, sBarcodeResult, param.iImageIdx, param.iTabIdx, param.AlgorithmParam);
												THEAPP.SaveDefectFeatureLog(THEAPP.Struct_PreferenceStruct.m_bSaveLasLog, THEAPP.m_pInspectService->m_sLotID_H, THEAPP.m_pInspectService->m_iTrayNo_H, m_iModuleNo, sBarcodeResult, param.iImageIdx, param.iTabIdx, DEFECT_NAME_DIRT, HImgRef, HDetectParamRgn);
												return true;
											}
											return false;
										}), vec.end());
								}
							}
						}
					}

					if (THEAPP.m_pGFunction->ValidHRegion(m_HResultDefectRgn) && THEAPP.m_pGFunction->ValidHRegion(m_HRgnScratch))
					{
						intersection(m_HResultDefectRgn, m_HRgnScratch, &HDisplayScratchRgn);

						if (THEAPP.m_pGFunction->ValidHRegion(HDisplayScratchRgn) == TRUE)
						{
							color = THEAPP.Struct_PreferenceStruct.lNGColor[DEFECT_NAME_SCRATCH];
							set_rgb(HideWindowHandle, GetRValue(color), GetGValue(color), GetBValue(color));
							disp_obj(HDisplayScratchRgn, HideWindowHandle);

							if (iScratchDefectSortingNumber > 0)
							{
								m_iInspectResult = IR_SCRATCH;
								m_iScratch = IR_SCRATCH;		// Bug Fixed

								// 파라미터 로그 저장
								auto& vec = THEAPP.m_vDetectParamInfo[THEAPP.m_pInspectService->m_iTrayNo_H - 1][m_iModuleNo - 1][DEFECT_NAME_SCRATCH];
								if (!vec.empty())
								{
									vec.erase(remove_if(vec.begin(), vec.end(),
										[&](const auto& param) {
											Hobject HDetectParamRgn;
											intersection(HDisplayScratchRgn, param.HRegion, &HDetectParamRgn);
											if (THEAPP.m_pGFunction->ValidHRegion(HDetectParamRgn) == TRUE)
											{
												THEAPP.SaveDefectParamLog(THEAPP.Struct_PreferenceStruct.m_bSaveLasLog, param.bBarrelLens, THEAPP.m_pInspectService->m_sLotID_H, THEAPP.m_pInspectService->m_iTrayNo_H, m_iModuleNo, sBarcodeResult, param.iImageIdx, param.iTabIdx, param.AlgorithmParam);
												THEAPP.SaveDefectFeatureLog(THEAPP.Struct_PreferenceStruct.m_bSaveLasLog, THEAPP.m_pInspectService->m_sLotID_H, THEAPP.m_pInspectService->m_iTrayNo_H, m_iModuleNo, sBarcodeResult, param.iImageIdx, param.iTabIdx, DEFECT_NAME_SCRATCH, HImgRef, HDetectParamRgn);
												return true;
											}
											return false;
										}), vec.end());
								}
							}
						}
					}

					if (THEAPP.m_pGFunction->ValidHRegion(m_HResultDefectRgn) && THEAPP.m_pGFunction->ValidHRegion(m_HRgnStain))
					{
						intersection(m_HResultDefectRgn, m_HRgnStain, &HDisplayStainRgn);

						if (THEAPP.m_pGFunction->ValidHRegion(HDisplayStainRgn) == TRUE)
						{
							color = THEAPP.Struct_PreferenceStruct.lNGColor[DEFECT_NAME_STAIN];
							set_rgb(HideWindowHandle, GetRValue(color), GetGValue(color), GetBValue(color));
							disp_obj(HDisplayStainRgn, HideWindowHandle);

							if (iStainDefectSortingNumber > 0)
							{
								m_iInspectResult = IR_STAIN;
								m_iStain = IR_STAIN;

								// 파라미터 로그 저장
								auto& vec = THEAPP.m_vDetectParamInfo[THEAPP.m_pInspectService->m_iTrayNo_H - 1][m_iModuleNo - 1][DEFECT_NAME_STAIN];
								if (!vec.empty())
								{
									vec.erase(remove_if(vec.begin(), vec.end(),
										[&](const auto& param) {
											Hobject HDetectParamRgn;
											intersection(HDisplayStainRgn, param.HRegion, &HDetectParamRgn);
											if (THEAPP.m_pGFunction->ValidHRegion(HDetectParamRgn) == TRUE)
											{
												THEAPP.SaveDefectParamLog(THEAPP.Struct_PreferenceStruct.m_bSaveLasLog, param.bBarrelLens, THEAPP.m_pInspectService->m_sLotID_H, THEAPP.m_pInspectService->m_iTrayNo_H, m_iModuleNo, sBarcodeResult, param.iImageIdx, param.iTabIdx, param.AlgorithmParam);
												THEAPP.SaveDefectFeatureLog(THEAPP.Struct_PreferenceStruct.m_bSaveLasLog, THEAPP.m_pInspectService->m_sLotID_H, THEAPP.m_pInspectService->m_iTrayNo_H, m_iModuleNo, sBarcodeResult, param.iImageIdx, param.iTabIdx, DEFECT_NAME_STAIN, HImgRef, HDetectParamRgn);
												return true;
											}
											return false;
										}), vec.end());
								}
							}
						}
					}

					if (THEAPP.m_pGFunction->ValidHRegion(m_HResultDefectRgn) && THEAPP.m_pGFunction->ValidHRegion(m_HRgnDent))
					{
						intersection(m_HResultDefectRgn, m_HRgnDent, &HDisplayDentRgn);

						if (THEAPP.m_pGFunction->ValidHRegion(HDisplayDentRgn) == TRUE)
						{
							color = THEAPP.Struct_PreferenceStruct.lNGColor[DEFECT_NAME_DENT];
							set_rgb(HideWindowHandle, GetRValue(color), GetGValue(color), GetBValue(color));
							disp_obj(HDisplayDentRgn, HideWindowHandle);

							if (iDentDefectSortingNumber > 0)
							{
								m_iInspectResult = IR_DENT;
								m_iDent = IR_DENT;

								// 파라미터 로그 저장
								auto& vec = THEAPP.m_vDetectParamInfo[THEAPP.m_pInspectService->m_iTrayNo_H - 1][m_iModuleNo - 1][DEFECT_NAME_DENT];
								if (!vec.empty())
								{
									vec.erase(remove_if(vec.begin(), vec.end(),
										[&](const auto& param) {
											Hobject HDetectParamRgn;
											intersection(HDisplayDentRgn, param.HRegion, &HDetectParamRgn);
											if (THEAPP.m_pGFunction->ValidHRegion(HDetectParamRgn) == TRUE)
											{
												THEAPP.SaveDefectParamLog(THEAPP.Struct_PreferenceStruct.m_bSaveLasLog, param.bBarrelLens, THEAPP.m_pInspectService->m_sLotID_H, THEAPP.m_pInspectService->m_iTrayNo_H, m_iModuleNo, sBarcodeResult, param.iImageIdx, param.iTabIdx, param.AlgorithmParam);
												THEAPP.SaveDefectFeatureLog(THEAPP.Struct_PreferenceStruct.m_bSaveLasLog, THEAPP.m_pInspectService->m_sLotID_H, THEAPP.m_pInspectService->m_iTrayNo_H, m_iModuleNo, sBarcodeResult, param.iImageIdx, param.iTabIdx, DEFECT_NAME_DENT, HImgRef, HDetectParamRgn);
												return true;
											}
											return false;
										}), vec.end());
								}
							}
						}
					}

					if (THEAPP.m_pGFunction->ValidHRegion(m_HResultDefectRgn) && THEAPP.m_pGFunction->ValidHRegion(m_HRgnChipping))
					{
						intersection(m_HResultDefectRgn, m_HRgnChipping, &HDisplayChippingRgn);

						if (THEAPP.m_pGFunction->ValidHRegion(HDisplayChippingRgn) == TRUE)
						{
							color = THEAPP.Struct_PreferenceStruct.lNGColor[DEFECT_NAME_CHIPPING];
							set_rgb(HideWindowHandle, GetRValue(color), GetGValue(color), GetBValue(color));
							disp_obj(HDisplayChippingRgn, HideWindowHandle);

							if (iChippingDefectSortingNumber > 0)
							{
								m_iInspectResult = IR_CHIPPING;
								m_iChipping = IR_CHIPPING;

								// 파라미터 로그 저장
								auto& vec = THEAPP.m_vDetectParamInfo[THEAPP.m_pInspectService->m_iTrayNo_H - 1][m_iModuleNo - 1][DEFECT_NAME_CHIPPING];
								if (!vec.empty())
								{
									vec.erase(remove_if(vec.begin(), vec.end(),
										[&](const auto& param) {
											Hobject HDetectParamRgn;
											intersection(HDisplayChippingRgn, param.HRegion, &HDetectParamRgn);
											if (THEAPP.m_pGFunction->ValidHRegion(HDetectParamRgn) == TRUE)
											{
												THEAPP.SaveDefectParamLog(THEAPP.Struct_PreferenceStruct.m_bSaveLasLog, param.bBarrelLens, THEAPP.m_pInspectService->m_sLotID_H, THEAPP.m_pInspectService->m_iTrayNo_H, m_iModuleNo, sBarcodeResult, param.iImageIdx, param.iTabIdx, param.AlgorithmParam);
												THEAPP.SaveDefectFeatureLog(THEAPP.Struct_PreferenceStruct.m_bSaveLasLog, THEAPP.m_pInspectService->m_sLotID_H, THEAPP.m_pInspectService->m_iTrayNo_H, m_iModuleNo, sBarcodeResult, param.iImageIdx, param.iTabIdx, DEFECT_NAME_CHIPPING, HImgRef, HDetectParamRgn);
												return true;
											}
											return false;
										}), vec.end());
								}
							}
						}
					}

					if (THEAPP.m_pGFunction->ValidHRegion(m_HResultDefectRgn) && THEAPP.m_pGFunction->ValidHRegion(m_HRgnEpoxy))
					{
						intersection(m_HResultDefectRgn, m_HRgnEpoxy, &HDisplayEpoxyRgn);

						if (THEAPP.m_pGFunction->ValidHRegion(HDisplayEpoxyRgn) == TRUE)
						{
							color = THEAPP.Struct_PreferenceStruct.lNGColor[DEFECT_NAME_EPOXY];
							set_rgb(HideWindowHandle, GetRValue(color), GetGValue(color), GetBValue(color));
							disp_obj(HDisplayEpoxyRgn, HideWindowHandle);

							if (iEpoxyDefectSortingNumber > 0)
							{
								m_iInspectResult = IR_EPOXY;
								m_iEpoxy = IR_EPOXY;

								// 파라미터 로그 저장
								auto& vec = THEAPP.m_vDetectParamInfo[THEAPP.m_pInspectService->m_iTrayNo_H - 1][m_iModuleNo - 1][DEFECT_NAME_EPOXY];
								if (!vec.empty())
								{
									vec.erase(remove_if(vec.begin(), vec.end(),
										[&](const auto& param) {
											Hobject HDetectParamRgn;
											intersection(HDisplayEpoxyRgn, param.HRegion, &HDetectParamRgn);
											if (THEAPP.m_pGFunction->ValidHRegion(HDetectParamRgn) == TRUE)
											{
												THEAPP.SaveDefectParamLog(THEAPP.Struct_PreferenceStruct.m_bSaveLasLog, param.bBarrelLens, THEAPP.m_pInspectService->m_sLotID_H, THEAPP.m_pInspectService->m_iTrayNo_H, m_iModuleNo, sBarcodeResult, param.iImageIdx, param.iTabIdx, param.AlgorithmParam);
												THEAPP.SaveDefectFeatureLog(THEAPP.Struct_PreferenceStruct.m_bSaveLasLog, THEAPP.m_pInspectService->m_sLotID_H, THEAPP.m_pInspectService->m_iTrayNo_H, m_iModuleNo, sBarcodeResult, param.iImageIdx, param.iTabIdx, DEFECT_NAME_EPOXY, HImgRef, HDetectParamRgn);
												return true;
											}
											return false;
										}), vec.end());
								}
							}
						}
					}

					if (THEAPP.m_pGFunction->ValidHRegion(m_HResultDefectRgn) && THEAPP.m_pGFunction->ValidHRegion(m_HRgnOutsideCT))
					{
						intersection(m_HResultDefectRgn, m_HRgnOutsideCT, &HDisplayOutsideCTRgn);

						if (THEAPP.m_pGFunction->ValidHRegion(HDisplayOutsideCTRgn) == TRUE)
						{
							color = THEAPP.Struct_PreferenceStruct.lNGColor[DEFECT_NAME_OUTSIDE_CT];
							set_rgb(HideWindowHandle, GetRValue(color), GetGValue(color), GetBValue(color));
							disp_obj(HDisplayOutsideCTRgn, HideWindowHandle);

							if (iOutsideCTDefectSortingNumber > 0)
							{
								m_iInspectResult = IR_OUTSIDE_CT;
								m_iOutsideCT = IR_OUTSIDE_CT;

								// 파라미터 로그 저장
								auto& vec = THEAPP.m_vDetectParamInfo[THEAPP.m_pInspectService->m_iTrayNo_H - 1][m_iModuleNo - 1][DEFECT_NAME_OUTSIDE_CT];
								if (!vec.empty())
								{
									vec.erase(remove_if(vec.begin(), vec.end(),
										[&](const auto& param) {
											Hobject HDetectParamRgn;
											intersection(HDisplayOutsideCTRgn, param.HRegion, &HDetectParamRgn);
											if (THEAPP.m_pGFunction->ValidHRegion(HDetectParamRgn) == TRUE)
											{
												THEAPP.SaveDefectParamLog(THEAPP.Struct_PreferenceStruct.m_bSaveLasLog, param.bBarrelLens, THEAPP.m_pInspectService->m_sLotID_H, THEAPP.m_pInspectService->m_iTrayNo_H, m_iModuleNo, sBarcodeResult, param.iImageIdx, param.iTabIdx, param.AlgorithmParam);
												THEAPP.SaveDefectFeatureLog(THEAPP.Struct_PreferenceStruct.m_bSaveLasLog, THEAPP.m_pInspectService->m_sLotID_H, THEAPP.m_pInspectService->m_iTrayNo_H, m_iModuleNo, sBarcodeResult, param.iImageIdx, param.iTabIdx, DEFECT_NAME_OUTSIDE_CT, HImgRef, HDetectParamRgn);
												return true;
											}
											return false;
										}), vec.end());
								}
							}
						}
					}

					if (THEAPP.m_pGFunction->ValidHRegion(m_HResultDefectRgn) && THEAPP.m_pGFunction->ValidHRegion(m_HRgnPeelOff))
					{
						intersection(m_HResultDefectRgn, m_HRgnPeelOff, &HDisplayPeelOffRgn);

						if (THEAPP.m_pGFunction->ValidHRegion(HDisplayPeelOffRgn) == TRUE)
						{
							color = THEAPP.Struct_PreferenceStruct.lNGColor[DEFECT_NAME_PEEL_OFF];
							set_rgb(HideWindowHandle, GetRValue(color), GetGValue(color), GetBValue(color));
							disp_obj(HDisplayPeelOffRgn, HideWindowHandle);

							if (iPeelOffDefectSortingNumber > 0)
							{
								m_iInspectResult = IR_PEEL_OFF;
								m_iPeelOff = IR_PEEL_OFF;

								// 파라미터 로그 저장
								auto& vec = THEAPP.m_vDetectParamInfo[THEAPP.m_pInspectService->m_iTrayNo_H - 1][m_iModuleNo - 1][DEFECT_NAME_PEEL_OFF];
								if (!vec.empty())
								{
									vec.erase(remove_if(vec.begin(), vec.end(),
										[&](const auto& param) {
											Hobject HDetectParamRgn;
											intersection(HDisplayPeelOffRgn, param.HRegion, &HDetectParamRgn);
											if (THEAPP.m_pGFunction->ValidHRegion(HDetectParamRgn) == TRUE)
											{
												THEAPP.SaveDefectParamLog(THEAPP.Struct_PreferenceStruct.m_bSaveLasLog, param.bBarrelLens, THEAPP.m_pInspectService->m_sLotID_H, THEAPP.m_pInspectService->m_iTrayNo_H, m_iModuleNo, sBarcodeResult, param.iImageIdx, param.iTabIdx, param.AlgorithmParam);
												THEAPP.SaveDefectFeatureLog(THEAPP.Struct_PreferenceStruct.m_bSaveLasLog, THEAPP.m_pInspectService->m_sLotID_H, THEAPP.m_pInspectService->m_iTrayNo_H, m_iModuleNo, sBarcodeResult, param.iImageIdx, param.iTabIdx, DEFECT_NAME_PEEL_OFF, HImgRef, HDetectParamRgn);
												return true;
											}
											return false;
										}), vec.end());
								}
							}
						}
					}

					if (THEAPP.m_pGFunction->ValidHRegion(m_HResultDefectRgn) && THEAPP.m_pGFunction->ValidHRegion(m_HRgnWhiteDot))
					{
						intersection(m_HResultDefectRgn, m_HRgnWhiteDot, &HDisplayWhiteDotRgn);

						if (THEAPP.m_pGFunction->ValidHRegion(HDisplayWhiteDotRgn) == TRUE)
						{
							color = THEAPP.Struct_PreferenceStruct.lNGColor[DEFECT_NAME_WHITEDOT];
							set_rgb(HideWindowHandle, GetRValue(color), GetGValue(color), GetBValue(color));
							disp_obj(HDisplayWhiteDotRgn, HideWindowHandle);

							if (iWhiteDotDefectSortingNumber > 0)
							{
								m_iInspectResult = IR_WHITEDOT;
								m_iWhiteDot = IR_WHITEDOT;

								// 파라미터 로그 저장
								auto& vec = THEAPP.m_vDetectParamInfo[THEAPP.m_pInspectService->m_iTrayNo_H - 1][m_iModuleNo - 1][DEFECT_NAME_WHITEDOT];
								if (!vec.empty())
								{
									vec.erase(remove_if(vec.begin(), vec.end(),
										[&](const auto& param) {
											Hobject HDetectParamRgn;
											intersection(HDisplayWhiteDotRgn, param.HRegion, &HDetectParamRgn);
											if (THEAPP.m_pGFunction->ValidHRegion(HDetectParamRgn) == TRUE)
											{
												THEAPP.SaveDefectParamLog(THEAPP.Struct_PreferenceStruct.m_bSaveLasLog, param.bBarrelLens, THEAPP.m_pInspectService->m_sLotID_H, THEAPP.m_pInspectService->m_iTrayNo_H, m_iModuleNo, sBarcodeResult, param.iImageIdx, param.iTabIdx, param.AlgorithmParam);
												THEAPP.SaveDefectFeatureLog(THEAPP.Struct_PreferenceStruct.m_bSaveLasLog, THEAPP.m_pInspectService->m_sLotID_H, THEAPP.m_pInspectService->m_iTrayNo_H, m_iModuleNo, sBarcodeResult, param.iImageIdx, param.iTabIdx, DEFECT_NAME_WHITEDOT, HImgRef, HDetectParamRgn);
												return true;
											}
											return false;
										}), vec.end());
								}
							}
						}
					}

					/////////// 결과를 text로 표기 /////////////////////////
					set_display_font(HideWindowHandle, OVERLAY_IMAGE_RESULT_FONT_SIZE + THEAPP.Struct_PreferenceStruct.m_iResultTextSize, "mono", "true", "false");
					set_color(HideWindowHandle, "red");

					set_tposition(HideWindowHandle, m_ViewRect.top + 50 + THEAPP.Struct_PreferenceStruct.m_iResultTextPosY, m_ViewRect.left + 50 + THEAPP.Struct_PreferenceStruct.m_iResultTextPosX);

					DecideInspectResult();

					HTuple HDefectName;

					if (bBarcodeError || bBarcodeShiftResult) //Ver 2592 modify(230109)
					{
						set_tposition(HideWindowHandle, m_ViewRect.top + 95, m_ViewRect.left + 50);

						m_iInspectResult = IR_NG;
						if (bBarcodeError)
							HDefectName = "Barcode Error";
						else if (bBarcodeShiftResult)
							HDefectName = "Barcode Shift Error";

						write_string(HideWindowHandle, HDefectName);
						*pStrDefectPosition = "";
					}
					else
					{
						switch (m_iInspectResult)
						{
						case IR_GOOD: HDefectName = "양품"; break;
						case IR_NG: HDefectName = "불량"; break;
						case IR_DIRT: HDefectName = "이물"; break;
						case IR_SCRATCH: HDefectName = "스크래치"; break;
						case IR_STAIN: HDefectName = "얼룩"; break;
						case IR_DENT: HDefectName = "Dent"; break;
						case IR_CHIPPING: HDefectName = "칩핑"; break;
						case IR_EPOXY: HDefectName = "에폭시"; break;
						case IR_OUTSIDE_CT: HDefectName = "외곽오염"; break;
						case IR_PEEL_OFF: HDefectName = "Peel Off"; break;
						case IR_WHITEDOT: HDefectName = "WhiteDot"; break;
						default: HDefectName = "양품"; break;
						}

						if (m_iInspectResult == IR_DIRT && iDirtDefectSortingNumber > 0)
						{
							switch (iDirtDefectSortingNumber)
							{
							case DP_INNER_BARREL:
							{
								write_string(HideWindowHandle, HDefectName + "(Inner Barrel NG)");
								strLensTopBottom.Format("Dirt");
								*pStrDefectPosition = strLensTopBottom;
								break;
							}
							case DP_TOP_BARREL:
							{
								write_string(HideWindowHandle, HDefectName + "(Top Barrel NG)");
								strLensTopBottom.Format("Dirt");
								*pStrDefectPosition = strLensTopBottom;
								break;
							}
							case DP_BOTTOM_BARREL:
							{
								write_string(HideWindowHandle, HDefectName + "(Bottom Barrel NG)");
								strLensTopBottom.Format("Dirt");
								*pStrDefectPosition = strLensTopBottom;
								break;
							}
							}

							THEAPP.m_pTrayAdminViewDlg->m_TrayImagePB.OkNg[m_iModuleNo - 1] = DEFECT_TYPE_BARREL;
							THEAPP.m_pTrayOperatorViewNowDlg->m_TrayImagePB.OkNg[m_iModuleNo - 1] = DEFECT_TYPE_BARREL;
							m_bBarrelDefectOKNG[m_iModuleNo - 1] = TRUE;
						}
						else if (m_iInspectResult == IR_SCRATCH && iScratchDefectSortingNumber > 0)
						{
							switch (iScratchDefectSortingNumber)
							{
							case DP_INNER_BARREL:
							{
								write_string(HideWindowHandle, HDefectName + "(Inner Barrel NG)");
								strLensTopBottom.Format("Scratch");
								*pStrDefectPosition = strLensTopBottom;
								break;
							}
							case DP_TOP_BARREL:
							{
								write_string(HideWindowHandle, HDefectName + "(Top Barrel NG)");
								strLensTopBottom.Format("Scratch");
								*pStrDefectPosition = strLensTopBottom;
								break;
							}
							case DP_BOTTOM_BARREL:
							{
								write_string(HideWindowHandle, HDefectName + "(Bottom Barrel NG)");
								strLensTopBottom.Format("Scratch");
								*pStrDefectPosition = strLensTopBottom;
								break;
							}
							}

							THEAPP.m_pTrayAdminViewDlg->m_TrayImagePB.OkNg[m_iModuleNo - 1] = DEFECT_TYPE_BARREL;
							THEAPP.m_pTrayOperatorViewNowDlg->m_TrayImagePB.OkNg[m_iModuleNo - 1] = DEFECT_TYPE_BARREL;
							m_bBarrelDefectOKNG[m_iModuleNo - 1] = TRUE;
						}
						else if (m_iInspectResult == IR_STAIN && iStainDefectSortingNumber > 0)
						{
							switch (iStainDefectSortingNumber)
							{
							case DP_INNER_BARREL:
							{
								write_string(HideWindowHandle, HDefectName + "(Inner Barrel NG)");
								strLensTopBottom.Format("Stain");
								*pStrDefectPosition = strLensTopBottom;
								break;
							}
							case DP_TOP_BARREL:
							{
								write_string(HideWindowHandle, HDefectName + "(Top Barrel NG)");
								strLensTopBottom.Format("Stain");
								*pStrDefectPosition = strLensTopBottom;
								break;
							}
							case DP_BOTTOM_BARREL:
							{
								write_string(HideWindowHandle, HDefectName + "(Bottom Barrel NG)");
								strLensTopBottom.Format("Stain");
								*pStrDefectPosition = strLensTopBottom;
								break;
							}
							}

							THEAPP.m_pTrayAdminViewDlg->m_TrayImagePB.OkNg[m_iModuleNo - 1] = DEFECT_TYPE_BARREL;
							THEAPP.m_pTrayOperatorViewNowDlg->m_TrayImagePB.OkNg[m_iModuleNo - 1] = DEFECT_TYPE_BARREL;
							m_bBarrelDefectOKNG[m_iModuleNo - 1] = TRUE;
						}
						else if (m_iInspectResult == IR_DENT && iDentDefectSortingNumber > 0)
						{
							switch (iDentDefectSortingNumber)
							{
							case DP_INNER_BARREL:
							{
								write_string(HideWindowHandle, HDefectName + "(Inner Barrel NG)");
								strLensTopBottom.Format("Dent");
								*pStrDefectPosition = strLensTopBottom;
								break;
							}
							case DP_TOP_BARREL:
							{
								write_string(HideWindowHandle, HDefectName + "(Top Barrel NG)");
								strLensTopBottom.Format("Dent");
								*pStrDefectPosition = strLensTopBottom;
								break;
							}
							case DP_BOTTOM_BARREL:
							{
								write_string(HideWindowHandle, HDefectName + "(Bottom Barrel NG)");
								strLensTopBottom.Format("Dent");
								*pStrDefectPosition = strLensTopBottom;
								break;
							}
							}

							THEAPP.m_pTrayAdminViewDlg->m_TrayImagePB.OkNg[m_iModuleNo - 1] = DEFECT_TYPE_BARREL;
							THEAPP.m_pTrayOperatorViewNowDlg->m_TrayImagePB.OkNg[m_iModuleNo - 1] = DEFECT_TYPE_BARREL;
							m_bBarrelDefectOKNG[m_iModuleNo - 1] = TRUE;
						}
						else if (m_iInspectResult == IR_CHIPPING && iChippingDefectSortingNumber > 0)
						{
							switch (iChippingDefectSortingNumber)
							{
							case DP_INNER_BARREL:
							{
								write_string(HideWindowHandle, HDefectName + "(Inner Barrel NG)");
								strLensTopBottom.Format("Chipping");
								*pStrDefectPosition = strLensTopBottom;
								break;
							}
							case DP_TOP_BARREL:
							{
								write_string(HideWindowHandle, HDefectName + "(Top Barrel NG)");
								strLensTopBottom.Format("Chipping");
								*pStrDefectPosition = strLensTopBottom;
								break;
							}
							case DP_BOTTOM_BARREL:
							{
								write_string(HideWindowHandle, HDefectName + "(Bottom Barrel NG)");
								strLensTopBottom.Format("Chipping");
								*pStrDefectPosition = strLensTopBottom;
								break;
							}
							}

							THEAPP.m_pTrayAdminViewDlg->m_TrayImagePB.OkNg[m_iModuleNo - 1] = DEFECT_TYPE_BARREL;
							THEAPP.m_pTrayOperatorViewNowDlg->m_TrayImagePB.OkNg[m_iModuleNo - 1] = DEFECT_TYPE_BARREL;
							m_bBarrelDefectOKNG[m_iModuleNo - 1] = TRUE;
						}
						else if (m_iInspectResult == IR_EPOXY && iEpoxyDefectSortingNumber > 0)
						{
							switch (iEpoxyDefectSortingNumber)
							{
							case DP_INNER_BARREL:
							{
								write_string(HideWindowHandle, HDefectName + "(Inner Barrel NG)");
								strLensTopBottom.Format("Epoxy");
								*pStrDefectPosition = strLensTopBottom;
								break;
							}
							case DP_TOP_BARREL:
							{
								write_string(HideWindowHandle, HDefectName + "(Top Barrel NG)");
								strLensTopBottom.Format("Epoxy");
								*pStrDefectPosition = strLensTopBottom;
								break;
							}
							case DP_BOTTOM_BARREL:
							{
								write_string(HideWindowHandle, HDefectName + "(Bottom Barrel NG)");
								strLensTopBottom.Format("Epoxy");
								*pStrDefectPosition = strLensTopBottom;
								break;
							}
							}

							THEAPP.m_pTrayAdminViewDlg->m_TrayImagePB.OkNg[m_iModuleNo - 1] = DEFECT_TYPE_BARREL;
							THEAPP.m_pTrayOperatorViewNowDlg->m_TrayImagePB.OkNg[m_iModuleNo - 1] = DEFECT_TYPE_BARREL;
							m_bBarrelDefectOKNG[m_iModuleNo - 1] = TRUE;
						}
						else if (m_iInspectResult == IR_OUTSIDE_CT && iOutsideCTDefectSortingNumber > 0)
						{
							switch (iOutsideCTDefectSortingNumber)
							{
							case DP_INNER_BARREL:
							{
								write_string(HideWindowHandle, HDefectName + "(Inner Barrel NG)");
								strLensTopBottom.Format("Outside Contamination");
								*pStrDefectPosition = strLensTopBottom;
								break;
							}
							case DP_TOP_BARREL:
							{
								write_string(HideWindowHandle, HDefectName + "(Top Barrel NG)");
								strLensTopBottom.Format("Outside Contamination");
								*pStrDefectPosition = strLensTopBottom;
								break;
							}
							case DP_BOTTOM_BARREL:
							{
								write_string(HideWindowHandle, HDefectName + "(Bottom Barrel NG)");
								strLensTopBottom.Format("Outside Contamination");
								*pStrDefectPosition = strLensTopBottom;
								break;
							}
							}

							THEAPP.m_pTrayAdminViewDlg->m_TrayImagePB.OkNg[m_iModuleNo - 1] = DEFECT_TYPE_BARREL;
							THEAPP.m_pTrayOperatorViewNowDlg->m_TrayImagePB.OkNg[m_iModuleNo - 1] = DEFECT_TYPE_BARREL;
							m_bBarrelDefectOKNG[m_iModuleNo - 1] = TRUE;
						}
						else if (m_iInspectResult == IR_PEEL_OFF && iPeelOffDefectSortingNumber > 0)
						{
							switch (iPeelOffDefectSortingNumber)
							{
							case DP_INNER_BARREL:
							{
								write_string(HideWindowHandle, HDefectName + "(Inner Barrel NG)");
								strLensTopBottom.Format("Peel Off");
								*pStrDefectPosition = strLensTopBottom;
								break;
							}
							case DP_TOP_BARREL:
							{
								write_string(HideWindowHandle, HDefectName + "(Top Barrel NG)");
								strLensTopBottom.Format("Peel Off");
								*pStrDefectPosition = strLensTopBottom;
								break;
							}
							case DP_BOTTOM_BARREL:
							{
								write_string(HideWindowHandle, HDefectName + "(Bottom Barrel NG)");
								strLensTopBottom.Format("Peel Off");
								*pStrDefectPosition = strLensTopBottom;
								break;
							}
							}

							THEAPP.m_pTrayAdminViewDlg->m_TrayImagePB.OkNg[m_iModuleNo - 1] = DEFECT_TYPE_BARREL;
							THEAPP.m_pTrayOperatorViewNowDlg->m_TrayImagePB.OkNg[m_iModuleNo - 1] = DEFECT_TYPE_BARREL;
							m_bBarrelDefectOKNG[m_iModuleNo - 1] = TRUE;
						}
						else if (m_iInspectResult == IR_WHITEDOT && iWhiteDotDefectSortingNumber > 0)
						{
							switch (iWhiteDotDefectSortingNumber)
							{
							case DP_INNER_BARREL:
							{
								write_string(HideWindowHandle, HDefectName + "(Inner Barrel NG)");
								strLensTopBottom.Format("WhiteDot");
								*pStrDefectPosition = strLensTopBottom;
								break;
							}
							case DP_TOP_BARREL:
							{
								write_string(HideWindowHandle, HDefectName + "(Top Barrel NG)");
								strLensTopBottom.Format("WhiteDot");
								*pStrDefectPosition = strLensTopBottom;
								break;
							}
							case DP_BOTTOM_BARREL:
							{
								write_string(HideWindowHandle, HDefectName + "(Bottom Barrel NG)");
								strLensTopBottom.Format("WhiteDot");
								*pStrDefectPosition = strLensTopBottom;
								break;
							}
							}

							THEAPP.m_pTrayAdminViewDlg->m_TrayImagePB.OkNg[m_iModuleNo - 1] = DEFECT_TYPE_BARREL;
							THEAPP.m_pTrayOperatorViewNowDlg->m_TrayImagePB.OkNg[m_iModuleNo - 1] = DEFECT_TYPE_BARREL;
							m_bBarrelDefectOKNG[m_iModuleNo - 1] = TRUE;
						}
						else	// 양품
						{
							set_color(HideWindowHandle, "blue");
							write_string(HideWindowHandle, "Barrel 양품");

							if (!((THEAPP.m_pTrayAdminViewDlg->m_TrayImagePB.OkNg[m_iModuleNo - 1] == DEFECT_TYPE_BARREL) ||
								(THEAPP.m_pTrayAdminViewDlg->m_TrayImagePB.OkNg[m_iModuleNo - 1] == DEFECT_TYPE_LENS) ||
								(THEAPP.m_pTrayAdminViewDlg->m_TrayImagePB.OkNg[m_iModuleNo - 1] == DEFECT_TYPE_EPOXYHOLE) ||
								(THEAPP.m_pTrayAdminViewDlg->m_TrayImagePB.OkNg[m_iModuleNo - 1] == DEFECT_TYPE_SPRINGNG)))
							{
								THEAPP.m_pTrayAdminViewDlg->m_TrayImagePB.OkNg[m_iModuleNo - 1] = DEFECT_TYPE_GOOD;
								THEAPP.m_pTrayOperatorViewNowDlg->m_TrayImagePB.OkNg[m_iModuleNo - 1] = DEFECT_TYPE_GOOD;
							}
						}
					}
				}// if (BarrelOrlens == 1)

				if (bIsLensDefect)		// 불량영역이 Lens 일 때	// 24.06.07 - v2647 - 이미지별 불량 표기를 위해 조건 3 추가 - LeeGW
				{
					// 배럴 검사 결과와 연동
					dblArea = *pdblArea;
					dDefectCenterY = *pdDefectCenterX;
					dDefectCenterY = *pdDefectCenterY;
					//
					CString strImageFileName;

					Hobject HImgLens;
					HTuple HContour1Row, HContour1Column, HContour1Radius;

					Hobject HRgnInspect;
					gen_empty_obj(&HRgnInspect);

					THEAPP.m_pAlgorithm->GetInspectArea_Circle(InspectContour5, 0, &HRgnInspect);

					reduce_domain(HImgRef, HRgnInspect, &HImgLens);

					if (THEAPP.m_pGFunction->ValidHImage(HImgLens))
					{
						// DefectRegion: 검사조건 MinArea를 통과한 렌즈 불량 (렌즈 스크래치 검사조건 적용)

						iLensDefectNumber = THEAPP.m_pAlgorithm->ApplyLensInspectionCondition(HDefectRgnforLens, &m_HRgnLensCT, &m_HRgnLensScratch, &m_HRgnLensDirt, &m_HRgnLensWhiteDot, HImgLens, &m_HRgnLensDefectMin);
						//Multiple Defect start
						int nVirtualMagazineNo = THEAPP.m_stVirtualMagazineNo.GetVirtualMagazineNo(THEAPP.m_pInspectService->m_sLotID_H);
						if (THEAPP.m_pGFunction->ValidHRegion(m_HRgnLensDefectMin))
						{


							switch (iLensDefectNumber)
							{
							case 1:
							case 2:
							case 3:   //LENS CONTAMINATION NG
							{
								copy_obj(m_HRgnLensDefectMin,
									&THEAPP.m_stMultipleDefectInfo[nVirtualMagazineNo][DEFECT_NAME_LENS_CONTAMINATION].m_HMultipleDefectRgn[THEAPP.m_pInspectService->m_iTrayNo_H - 1][m_iModuleNo - 1],
									1, -1); //Multiple Defect

								// 파라미터 로그 저장
								auto& vec = THEAPP.m_vDetectParamInfo[THEAPP.m_pInspectService->m_iTrayNo_H - 1][m_iModuleNo - 1][DEFECT_NAME_LENS_CONTAMINATION];
								if (!vec.empty())
								{
									vec.erase(remove_if(vec.begin(), vec.end(),
										[&](const auto& param) {
											Hobject HDetectParamRgn;
											intersection(m_HRgnLensDefectMin, param.HRegion, &HDetectParamRgn);
											if (THEAPP.m_pGFunction->ValidHRegion(HDetectParamRgn) == TRUE)
											{
												THEAPP.SaveDefectParamLog(THEAPP.Struct_PreferenceStruct.m_bSaveLasLog, param.bBarrelLens, THEAPP.m_pInspectService->m_sLotID_H, THEAPP.m_pInspectService->m_iTrayNo_H, m_iModuleNo, sBarcodeResult, param.iImageIdx, param.iTabIdx, param.AlgorithmParam);
												THEAPP.SaveDefectFeatureLog(THEAPP.Struct_PreferenceStruct.m_bSaveLasLog, THEAPP.m_pInspectService->m_sLotID_H, THEAPP.m_pInspectService->m_iTrayNo_H, m_iModuleNo, sBarcodeResult, param.iImageIdx, param.iTabIdx, DEFECT_NAME_LENS_CONTAMINATION, HImgRef, HDetectParamRgn);
												return true;
											}
											return false;
										}), vec.end());
								}
								break;
							}
							case 4: //LENS SCRATCH
							{
								copy_obj(m_HRgnLensDefectMin,
									&THEAPP.m_stMultipleDefectInfo[nVirtualMagazineNo][DEFECT_NAME_LENS_SCRATCH].m_HMultipleDefectRgn[THEAPP.m_pInspectService->m_iTrayNo_H - 1][m_iModuleNo - 1],
									1, -1); //Multiple Defect
								
								// 파라미터 로그 저장
								auto& vec = THEAPP.m_vDetectParamInfo[THEAPP.m_pInspectService->m_iTrayNo_H - 1][m_iModuleNo - 1][DEFECT_NAME_LENS_SCRATCH];
								if (!vec.empty())
								{
									vec.erase(remove_if(vec.begin(), vec.end(),
										[&](const auto& param) {
											Hobject HDetectParamRgn;
											intersection(m_HRgnLensDefectMin, param.HRegion, &HDetectParamRgn);
											if (THEAPP.m_pGFunction->ValidHRegion(HDetectParamRgn) == TRUE)
											{
												THEAPP.SaveDefectParamLog(THEAPP.Struct_PreferenceStruct.m_bSaveLasLog, param.bBarrelLens, THEAPP.m_pInspectService->m_sLotID_H, THEAPP.m_pInspectService->m_iTrayNo_H, m_iModuleNo, sBarcodeResult, param.iImageIdx, param.iTabIdx, param.AlgorithmParam);
												THEAPP.SaveDefectFeatureLog(THEAPP.Struct_PreferenceStruct.m_bSaveLasLog, THEAPP.m_pInspectService->m_sLotID_H, THEAPP.m_pInspectService->m_iTrayNo_H, m_iModuleNo, sBarcodeResult, param.iImageIdx, param.iTabIdx, DEFECT_NAME_LENS_SCRATCH, HImgRef, HDetectParamRgn);
												return true;
											}
											return false;
										}), vec.end());
								}
								break;
							}
							case 5: //LENS DIRT NG
							{
								copy_obj(m_HRgnLensDefectMin,
									&THEAPP.m_stMultipleDefectInfo[nVirtualMagazineNo][DEFECT_NAME_LENS_DIRT].m_HMultipleDefectRgn[THEAPP.m_pInspectService->m_iTrayNo_H - 1][m_iModuleNo - 1],
									1, -1); //Multiple Defect

								// 파라미터 로그 저장
								auto& vec = THEAPP.m_vDetectParamInfo[THEAPP.m_pInspectService->m_iTrayNo_H - 1][m_iModuleNo - 1][DEFECT_NAME_LENS_DIRT];
								if (!vec.empty())
								{
									vec.erase(remove_if(vec.begin(), vec.end(),
										[&](const auto& param) {
											Hobject HDetectParamRgn;
											intersection(m_HRgnLensDefectMin, param.HRegion, &HDetectParamRgn);
											if (THEAPP.m_pGFunction->ValidHRegion(HDetectParamRgn) == TRUE)
											{
												THEAPP.SaveDefectParamLog(THEAPP.Struct_PreferenceStruct.m_bSaveLasLog, param.bBarrelLens, THEAPP.m_pInspectService->m_sLotID_H, THEAPP.m_pInspectService->m_iTrayNo_H, m_iModuleNo, sBarcodeResult, param.iImageIdx, param.iTabIdx, param.AlgorithmParam);
												THEAPP.SaveDefectFeatureLog(THEAPP.Struct_PreferenceStruct.m_bSaveLasLog, THEAPP.m_pInspectService->m_sLotID_H, THEAPP.m_pInspectService->m_iTrayNo_H, m_iModuleNo, sBarcodeResult, param.iImageIdx, param.iTabIdx, DEFECT_NAME_LENS_DIRT, HImgRef, HDetectParamRgn);
												return true;
											}
											return false;
										}), vec.end());
								}
								break;
							}
							case 6: //LENS WHITEDOT NG
							{
								copy_obj(m_HRgnLensDefectMin,
									&THEAPP.m_stMultipleDefectInfo[nVirtualMagazineNo][DEFECT_NAME_LENS_WHITEDOT].m_HMultipleDefectRgn[THEAPP.m_pInspectService->m_iTrayNo_H - 1][m_iModuleNo - 1],
									1, -1); //Multiple Defect

								// 파라미터 로그 저장
								auto& vec = THEAPP.m_vDetectParamInfo[THEAPP.m_pInspectService->m_iTrayNo_H - 1][m_iModuleNo - 1][DEFECT_NAME_LENS_WHITEDOT];
								if (!vec.empty())
								{
									vec.erase(remove_if(vec.begin(), vec.end(),
										[&](const auto& param) {
											Hobject HDetectParamRgn;
											intersection(m_HRgnLensDefectMin, param.HRegion, &HDetectParamRgn);
											if (THEAPP.m_pGFunction->ValidHRegion(HDetectParamRgn) == TRUE)
											{
												THEAPP.SaveDefectParamLog(THEAPP.Struct_PreferenceStruct.m_bSaveLasLog, param.bBarrelLens, THEAPP.m_pInspectService->m_sLotID_H, THEAPP.m_pInspectService->m_iTrayNo_H, m_iModuleNo, sBarcodeResult, param.iImageIdx, param.iTabIdx, param.AlgorithmParam);
												THEAPP.SaveDefectFeatureLog(THEAPP.Struct_PreferenceStruct.m_bSaveLasLog, THEAPP.m_pInspectService->m_sLotID_H, THEAPP.m_pInspectService->m_iTrayNo_H, m_iModuleNo, sBarcodeResult, param.iImageIdx, param.iTabIdx, DEFECT_NAME_LENS_WHITEDOT, HImgRef, HDetectParamRgn);
												return true;
											}
											return false;
										}), vec.end());
								}
								break;
							}
							default:
							{
								break;
							}
							}
						}
						//Multiple Defect end

						Hobject SelectRgn;
						HTuple OriArea, OriSize, Row, Column;
						HTuple CountDefect;
						Hobject ConnectionsRegion;

						connection(m_HRgnLensDefectMin, &ConnectionsRegion);
						count_obj(ConnectionsRegion, &CountDefect);

						set_display_font(HideWindowHandle, 12, "mono", "true", "false");
						set_color(HideWindowHandle, "green");


						// ************ Defect 이 겹치게 그려짐을 막기 위해 Flag 배열을 만듬 ********
						int* nDefectFlag = new int[CountDefect[0].I()];

						// 그린 것에 대해 Flag 를 on 시킴
						for (int i = 0; i < CountDefect[0].I(); i++)
						{
							nDefectFlag[i] = 0;
						}

						int nRow = 0, nColumn = 0;
						int nCompRow = 0, nCompColumn = 0;

						Point pt1, pt2;
						pt1.x = 0, pt1.y = 0;
						pt2.x = 0, pt2.y = 0;

						double dblDist = 0.0;
						HTuple CompArea, CompSize, CompRow, CompColumn;

						for (int i = 0; i < CountDefect[0].I(); i++)
						{
							select_obj(ConnectionsRegion, &SelectRgn, i + 1);
							area_center(SelectRgn, &OriSize, &Row, &Column);

							dDefectCenterX = Column[0].D();
							dDefectCenterY = Row[0].D();

							Row += 5; Column += 5;

							nRow = Row[0].I();
							nColumn = Column[0].I();

							pt1.x = nRow; pt1.y = nColumn;

							for (int j = 0; j < CountDefect[0].I(); j++)
							{
								if (i != j)
								{
									select_obj(ConnectionsRegion, &SelectRgn, j + 1);
									area_center(SelectRgn, &CompSize, &CompRow, &CompColumn);
									CompRow += 5; CompColumn += 5;

									nCompRow = CompRow[0].I();
									nCompColumn = CompColumn[0].I();

									pt2.x = nCompRow; pt2.y = nCompColumn;

									dblDist = Distance(pt1, pt2);

									// 중심 간의 거리가 설정 값 보다 작다면 
									if (dblDist < THEAPP.Struct_PreferenceStruct.m_iDefectDispDist)
									{
										// 어떠한 것도 string 을 쓰지 않았다면 
										if (nDefectFlag[j] == 0)
										{
											// 좌측이로 이동시 0 이하의 값이 나오면
											if ((CompRow - THEAPP.Struct_PreferenceStruct.m_iDefectDispDist) > 0)
												CompRow = CompRow - THEAPP.Struct_PreferenceStruct.m_iDefectDispDist;
											else
												CompRow = 0;

											set_tposition(HideWindowHandle, CompRow, CompColumn);
											CompArea = ceil((CompSize[0].D() * PXLAREA) / 1000) / 1000;
											set_color(HideWindowHandle, "green");
											write_string(HideWindowHandle, CompArea + "mm2");

											nDefectFlag[j] = 1;
										}
									}
								}
							}

							// 어떠한 것도 string 을 쓰지 않았다면 
							if (nDefectFlag[i] == 0)
							{
								set_tposition(HideWindowHandle, Row, Column);
								OriArea = ceil((OriSize[0].D() * PXLAREA) / 1000) / 1000;
								write_string(HideWindowHandle, OriArea + "mm2");

								nDefectFlag[i] = 1;
							}

							// ********* 가장 큰 Area 만 뽑는다. *******
							if (OriArea[0].D() > dblArea)
							{
								dblArea = OriArea[0].D();
								*pdblArea = dblArea;
								*pdDefectCenterX = dDefectCenterX;
								*pdDefectCenterY = dDefectCenterY;
							}
							// ******************************************

						}

						delete(nDefectFlag);

						///////////////// Changed for CMI3000 2000 ====> ///////////////////////////////////////////////////
						set_color(HideWindowHandle, "slate blue");
						set_draw(HideWindowHandle, "margin");
						set_line_width(HideWindowHandle, 1);
						disp_obj(HDefectRegion, HideWindowHandle);
						///////////////// <==== Changed for CMI3000 2000 ///////////////////////////////////////////////////

						set_color(HideWindowHandle, "red");
						set_draw(HideWindowHandle, "margin");
						copy_obj(m_HRgnLensDefectMin, &m_HResultDefectRgn, 1, -1);		// 검사 Spec 적용된 불량 영역
						disp_obj(m_HResultDefectRgn, HideWindowHandle);


						///////////////// 결과 항목 글자로 표기 ///////////////////
						set_display_font(HideWindowHandle, OVERLAY_IMAGE_RESULT_FONT_SIZE + THEAPP.Struct_PreferenceStruct.m_iResultTextSize, "mono", "true", "false");
						set_color(HideWindowHandle, "red");

						set_tposition(HideWindowHandle, m_ViewRect.top + 50 + THEAPP.Struct_PreferenceStruct.m_iResultTextPosY + 100, m_ViewRect.left + 50 + THEAPP.Struct_PreferenceStruct.m_iResultTextPosX);

						if (iLensDefectNumber > 0)
						{
							m_bLensDefectOKNG[m_iModuleNo - 1] = TRUE;
						}

						switch (iLensDefectNumber)
						{
						case 1:
						case 2:
						case 3:
						{
							write_string(HideWindowHandle, "Lens 오염 NG");	// 24.05.09 - v2646 - 불량명 변경 - LeeGW
							strLensTopBottom.Format("Contamination");
							*pStrDefectPosition = strLensTopBottom;	// 24.05.09 - v2646 - 불량명 변경 - LeeGW

							m_bLensDefectOKNG[m_iModuleNo - 1] = TRUE;
							THEAPP.m_pTrayAdminViewDlg->m_TrayImagePB.OkNg[m_iModuleNo - 1] = DEFECT_TYPE_LENS;
							THEAPP.m_pTrayOperatorViewNowDlg->m_TrayImagePB.OkNg[m_iModuleNo - 1] = DEFECT_TYPE_LENS;
							break;
						}
						case 4:
						{
							write_string(HideWindowHandle, "Lens 스크래치 NG");
							strLensTopBottom.Format("Scratch");	// 24.05.09 - v2646 - 불량명 변경 - LeeGW
							*pStrDefectPosition = strLensTopBottom;

							m_bLensDefectOKNG[m_iModuleNo - 1] = TRUE;
							THEAPP.m_pTrayAdminViewDlg->m_TrayImagePB.OkNg[m_iModuleNo - 1] = DEFECT_TYPE_LENS;
							THEAPP.m_pTrayOperatorViewNowDlg->m_TrayImagePB.OkNg[m_iModuleNo - 1] = DEFECT_TYPE_LENS;
							break;
						}
						case 5:
						{
							write_string(HideWindowHandle, "Lens 이물 NG");	// 24.05.09 - v2646 - 불량명 변경 - LeeGW
							strLensTopBottom.Format("Dirt");
							*pStrDefectPosition = strLensTopBottom;	// 24.05.09 - v2646 - 불량명 변경 - LeeGW

							m_bLensDefectOKNG[m_iModuleNo - 1] = TRUE;
							THEAPP.m_pTrayAdminViewDlg->m_TrayImagePB.OkNg[m_iModuleNo - 1] = DEFECT_TYPE_LENS;
							THEAPP.m_pTrayOperatorViewNowDlg->m_TrayImagePB.OkNg[m_iModuleNo - 1] = DEFECT_TYPE_LENS;
							break;
						}
						case 6:
						{
							write_string(HideWindowHandle, "Lens WhiteDot NG");
							strLensTopBottom.Format("WhiteDot");
							*pStrDefectPosition = strLensTopBottom;	// 24.05.09 - v2646 - 불량명 변경 - LeeGW

							m_bLensDefectOKNG[m_iModuleNo - 1] = TRUE;
							THEAPP.m_pTrayAdminViewDlg->m_TrayImagePB.OkNg[m_iModuleNo - 1] = DEFECT_TYPE_LENS;
							THEAPP.m_pTrayOperatorViewNowDlg->m_TrayImagePB.OkNg[m_iModuleNo - 1] = DEFECT_TYPE_LENS;
							break;
						}
						default:
						{
							set_color(HideWindowHandle, "blue");
							write_string(HideWindowHandle, "Lens 양품");

							if ((THEAPP.m_pTrayAdminViewDlg->m_TrayImagePB.OkNg[m_iModuleNo - 1] == DEFECT_TYPE_BARREL) ||
								(THEAPP.m_pTrayAdminViewDlg->m_TrayImagePB.OkNg[m_iModuleNo - 1] == DEFECT_TYPE_LENS) ||
								(THEAPP.m_pTrayAdminViewDlg->m_TrayImagePB.OkNg[m_iModuleNo - 1] == DEFECT_TYPE_EPOXYHOLE) ||
								(THEAPP.m_pTrayAdminViewDlg->m_TrayImagePB.OkNg[m_iModuleNo - 1] == DEFECT_TYPE_SPRINGNG))
							{
								break;
							}
							if ((THEAPP.m_pTrayOperatorViewNowDlg->m_TrayImagePB.OkNg[m_iModuleNo - 1] == DEFECT_TYPE_BARREL) ||
								(THEAPP.m_pTrayOperatorViewNowDlg->m_TrayImagePB.OkNg[m_iModuleNo - 1] == DEFECT_TYPE_LENS) ||
								(THEAPP.m_pTrayOperatorViewNowDlg->m_TrayImagePB.OkNg[m_iModuleNo - 1] == DEFECT_TYPE_EPOXYHOLE) ||
								(THEAPP.m_pTrayOperatorViewNowDlg->m_TrayImagePB.OkNg[m_iModuleNo - 1] == DEFECT_TYPE_SPRINGNG))
							{
								break;
							}

							THEAPP.m_pTrayAdminViewDlg->m_TrayImagePB.OkNg[m_iModuleNo - 1] = DEFECT_TYPE_GOOD;
							THEAPP.m_pTrayOperatorViewNowDlg->m_TrayImagePB.OkNg[m_iModuleNo - 1] = DEFECT_TYPE_GOOD;
							break;
						}
						}
					}
					else
					{
						THEAPP.SaveLog("렌즈 이미지 없음");
					}
				}	// else if (BarrelOrlens == 2)		// 불량영역이 Lens 일 때

			} // if(THEAPP.m_pGFunction->ValidHRegion(m_HResultDefectRgn))		==> Barrel or Lens 불량이 있을 때
			else		// 양품
			{
				strImageResult = _T("OK");

				set_tposition(HideWindowHandle, m_ViewRect.top + 50 + THEAPP.Struct_PreferenceStruct.m_iResultTextPosY, m_ViewRect.left + 50 + THEAPP.Struct_PreferenceStruct.m_iResultTextPosX);

				set_display_font(HideWindowHandle, OVERLAY_IMAGE_RESULT_FONT_SIZE + THEAPP.Struct_PreferenceStruct.m_iResultTextSize, "mono", "true", "false");
				set_color(HideWindowHandle, "blue");

				write_string(HideWindowHandle, "Barrel 양품");

				set_tposition(HideWindowHandle, m_ViewRect.top + 50 + THEAPP.Struct_PreferenceStruct.m_iResultTextPosY + 100, m_ViewRect.left + 50 + THEAPP.Struct_PreferenceStruct.m_iResultTextPosX);

				set_display_font(HideWindowHandle, OVERLAY_IMAGE_RESULT_FONT_SIZE + THEAPP.Struct_PreferenceStruct.m_iResultTextSize, "mono", "true", "false");
				set_color(HideWindowHandle, "blue");

				write_string(HideWindowHandle, "Lens 양품");

				if (!((THEAPP.m_pTrayAdminViewDlg->m_TrayImagePB.OkNg[m_iModuleNo - 1] == DEFECT_TYPE_BARREL) ||
					(THEAPP.m_pTrayAdminViewDlg->m_TrayImagePB.OkNg[m_iModuleNo - 1] == DEFECT_TYPE_LENS) ||
					(THEAPP.m_pTrayAdminViewDlg->m_TrayImagePB.OkNg[m_iModuleNo - 1] == DEFECT_TYPE_EPOXYHOLE) ||
					(THEAPP.m_pTrayAdminViewDlg->m_TrayImagePB.OkNg[m_iModuleNo - 1] == DEFECT_TYPE_SPRINGNG)))
				{
					THEAPP.m_pTrayAdminViewDlg->m_TrayImagePB.OkNg[m_iModuleNo - 1] = DEFECT_TYPE_GOOD;
					THEAPP.m_pTrayOperatorViewNowDlg->m_TrayImagePB.OkNg[m_iModuleNo - 1] = DEFECT_TYPE_GOOD;
				}
			}

			// 24.05.16 - v2602 - 이미지별 불량 표기를 위해 Barrel Surface/Edge, Lens1/2 분리 - LeeGW
			Hobject HImgDump, HImgDump_save;
			gen_empty_obj(&HImgDump);
			gen_empty_obj(&HImgDump_save);

			dump_window_image(&HImgDump_save, HideWindowHandle);

			if (THEAPP.Struct_PreferenceStruct.m_bSaveResultImage)
			{
				CString Filename;
				Filename.Format("Module%d_Result_%s_%s_%s", m_iModuleNo, g_sImageName[THEAPP.m_iMachineInspType][BarrelOrlens], sBarcodeResult, strImageResult);

				if (THEAPP.Struct_PreferenceStruct.m_bSaveBMP == TRUE)
					THEAPP.m_pSaveManager->SaveImage(HImgDump_save, "bmp", THEAPP.m_FileBase.m_strOverayImageFolderPrev + "\\" + Filename);
				else
					THEAPP.m_pSaveManager->SaveImage(HImgDump_save, "jpg", THEAPP.m_FileBase.m_strOverayImageFolderPrev + "\\" + Filename);
			}
			
		}

		//////////////////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////////////
		//
		//  결과 저장
		//
		//////////////////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////////////


		CString strBarrelLens;
		CString strResult;
		strResult.Format("OK");

		CString strBarrelOKNG;
		CString strLensOKNG;
		BOOL bIsGood = FALSE;	// Barrel and Lens 에 대한 판정 결과
		CString sLASResult;

		BOOL bFAIResultNG = FALSE;
		CString sFAIResult = _T("OK");
		CString sFAIResultOkNg = _T("OK");

		Hobject HSelectedDefectRgn;
		gen_empty_obj(&HSelectedDefectRgn);

		Hlong lSelectedDefectArea;
		double dSelectedDefectAreaMM2;
		double dSelectedDefectCenterX, dSelectedDefectCenterY;
		BOOL bBarrelLensDefect = FALSE;
		int iXPosOffset, iYPosOffset;
		BOOL bDefectWarning = FALSE;
		CString sDefectWarning = _T("");

		// 24.07.22 - v2655 - 결과 로그에 동일한 바코드가 2줄씩 남는 현상 개선(조건변경)  - LeeGW
		if((THEAPP.Struct_PreferenceStruct.m_bSaveResultMerge==TRUE && BarrelOrlens== BARREL_LENS_IMAGE_TAB/2)||
			(THEAPP.Struct_PreferenceStruct.m_bSaveResultMerge==FALSE && BarrelOrlens==BARREL_LENS_IMAGE_TAB-1))		// 불량영역이 Lens 일 때	
		{
			//FAI 결과 반영 - 나중에 우선순위 확인  - LeeGW
			for (int i = MAX_FAI_ITEM - 1; i >= 0; i--)
			{
				if (THEAPP.m_StructFaiMeasure[THEAPP.m_pInspectService->m_iTrayNo_H - 1][m_iModuleNo - 1].m_bFAI_ResultNG[i][0])
				{
					sFAIResult.Format("FAI-%d NG", i);
					bFAIResultNG = TRUE;
					THEAPP.m_StructFaiMeasure[THEAPP.m_pInspectService->m_iTrayNo_H - 1][m_iModuleNo - 1].m_bInspResultNG = TRUE;
					THEAPP.m_StructFaiMeasure[THEAPP.m_pInspectService->m_iTrayNo_H - 1][m_iModuleNo - 1].m_iNGFAICode = i;
				}

			}

			// 순위 변경용 나중에 확인
			if (bFAIResultNG == TRUE)
			{
				THEAPP.m_pTrayAdminViewDlg->m_TrayImagePB.OkNg[m_iModuleNo - 1] = DEFECT_TYPE_FAING;
				THEAPP.m_pTrayOperatorViewNowDlg->m_TrayImagePB.OkNg[m_iModuleNo - 1] = DEFECT_TYPE_FAING;
			}

			if ((m_bBarrelDefectOKNG[m_iModuleNo - 1] == TRUE) && (m_bLensDefectOKNG[m_iModuleNo - 1] == TRUE))
			{
				strBarrelOKNG.Format("NG");
				strLensOKNG.Format("NG");

				strBarrelLens.Format("Barrel/Lens");
				strResult.Format("NG");
				THEAPP.m_pTrayAdminViewDlg->m_TrayImagePB.OkNg[m_iModuleNo - 1] = DEFECT_TYPE_BARREL_LENS;
				THEAPP.m_pTrayOperatorViewNowDlg->m_TrayImagePB.OkNg[m_iModuleNo - 1] = DEFECT_TYPE_BARREL_LENS;
			}
			else if ((m_bBarrelDefectOKNG[m_iModuleNo - 1] == FALSE) && (m_bLensDefectOKNG[m_iModuleNo - 1] == TRUE))
			{
				strBarrelOKNG.Format("OK");
				strLensOKNG.Format("NG");

				strBarrelLens.Format("Lens");
				strResult.Format("NG");
			}
			else if ((m_bBarrelDefectOKNG[m_iModuleNo - 1] == TRUE) && (m_bLensDefectOKNG[m_iModuleNo - 1] == FALSE))
			{
				strBarrelOKNG.Format("NG");
				strLensOKNG.Format("OK");

				strBarrelLens.Format("Barrel");
				strResult.Format("NG");
			}
			else if ((m_bBarrelDefectOKNG[m_iModuleNo - 1] == FALSE) && (m_bLensDefectOKNG[m_iModuleNo - 1] == FALSE))		// Barrel & Lens 양품인 경우
			{
				strBarrelOKNG.Format("OK");
				strLensOKNG.Format("OK");

				strBarrelLens.Format("양품");
				strResult.Format("OK");

				bIsGood = TRUE;
			}

			if (iExtraResult != DEFECT_TYPE_GOOD)
			{
				THEAPP.m_pTrayAdminViewDlg->m_TrayImagePB.OkNg[m_iModuleNo - 1] = iExtraResult;
				THEAPP.m_pTrayOperatorViewNowDlg->m_TrayImagePB.OkNg[m_iModuleNo - 1] = iExtraResult;
			}

			//Ver 2630 Barcode NG priority change
			if (bBarcodeShiftResult==TRUE)	// Barcode Shift Error
			{
				THEAPP.m_pTrayAdminViewDlg->m_TrayImagePB.OkNg[m_iModuleNo-1] = DEFECT_TYPE_BARCODE_SHIFT;
				THEAPP.m_pTrayOperatorViewNowDlg->m_TrayImagePB.OkNg[m_iModuleNo-1] = DEFECT_TYPE_BARCODE_SHIFT;
			}

			//////////////////////////////////////////////////////////////////////////
			// ResultData_%04d%02d%02d.txt


			CString sModuleMixResult;
			sModuleMixResult = _T("");
			if (bBarcodeModuleMixResult)
				sModuleMixResult.Format("Module Mix");

			CString sBarcodeShiftResult;
			sBarcodeShiftResult = _T("");
			if (bBarcodeShiftResult)
				sBarcodeShiftResult.Format("BarcodeShift");

			CString sBarcodeRotationResult;
			sBarcodeRotationResult = _T("");
			if (bBarcodeRotationResult)
				sBarcodeRotationResult.Format("BarcodeRotation");

			CString sBlackCoatingResult;
			sBlackCoatingResult = _T("");
			if (bBlackCoatingResult)
				sBlackCoatingResult.Format("BlackCoating NG");

			CString sBarcodeErrorResult;
			sBarcodeErrorResult = _T("");
			if (bBarcodeError)
				sBarcodeErrorResult.Format("BarcodeError");

			CString sMatchingErrorResult;
			sMatchingErrorResult = _T("");
			if (bMatchingError)
				sMatchingErrorResult.Format("MatchingError");

			CString sLightErrorResult;
			sLightErrorResult = _T("");
			if (bLightDisorder)
				sLightErrorResult.Format("LightError");


			CString sExResult;
			sExResult = _T("");
			if (iExtraResult == DEFECT_CODE_GOOD)
				sExResult.Format("OK");
			else
				sExResult.Format("NG");

			CString sExInspType;
			sExInspType.Format("%s", g_sImageName[THEAPP.m_iMachineInspType][TRIGGER_EXTRA1]);
			sExInspType.Delete(sExInspType.GetLength() - 1);

			CString sExDefectType;
			sExDefectType = _T("");
			if (iExtraResult == DEFECT_TYPE_SPRINGNG)
				sExDefectType.Format("Spring NG");
			else if (iExtraResult==DEFECT_TYPE_EPOXYHOLE)
				sExDefectType.Format("Epoxy Hole");


			CString sFinalResult;
			if (bIsGood==TRUE)
			{
				if (bBarcodeShiftResult==TRUE || iExtraResult != DEFECT_CODE_GOOD || bFAIResultNG == TRUE)
					sFinalResult.Format("NG");
				else
					sFinalResult.Format("OK");
			}
			else
				sFinalResult.Format("NG");

			if (bBarcodeModuleMixResult==TRUE)
				sFinalResult.Format("NG");


			double dCMInspectTime;
			CString strInspSave;
			CString strDefectItem = _T("");
			CString strDefectDetail = _T("");

			dCMInspectTime = THEAPP.m_InspectTime[m_iModuleNo];

			if (strBarrelOKNG=="NG")
				strDefectItem.Format("Barrel");
			else if (strLensOKNG=="NG")
				strDefectItem.Format("Lens");

			if (strBarrelOKNG=="NG")
				strDefectDetail = *pStrDefectPosition;
			else if (strLensOKNG=="NG")
				strDefectDetail = *pStrDefectPosition;	// 24.05.09 - v2646 - 불량명 변경 - LeeGW
			else
				strDefectDetail.Format("G");

			//////////////////////////////////////////////////////////////////////////
			//if (bModuleEmpty == TRUE)
			//{
			//	iDefectCodeRet = DEFECT_CODE_EMPTY;
			//}
			//else if (bBarcodeModuleMixResult==TRUE)	// Module Mix
			//{
			//	iDefectCodeRet = DEFECT_CODE_MODULE_MIX_ERROR;
			//}
			//else if (bLightDisorder == TRUE)	// LE
			//{
			//	iDefectCodeRet = DEFECT_CODE_LIGHT_ERROR;
			//}
			//else if (bMatchingError == TRUE)	// MC
			//{
			//	iDefectCodeRet = DEFECT_CODE_MATCHING_ERROR;
			//}
			//else if (bBarcodeError == TRUE)	// Barcode Shift NG
			//{
			//	iDefectCodeRet = DEFECT_CODE_BARCODE_ERROR;
			//}
			//else if (bBarcodeShiftResult == TRUE)	// Barcode Shift NG
			//{
			//	iDefectCodeRet = DEFECT_CODE_BARCODE_SHIFT;
			//}
			//else if (iExtraResult == DEFECT_TYPE_SPRINGNG)
			//{
			//	iDefectCodeRet = DEFECT_CODE_SPRINGNG;
			//}
			//else if (iExtraResult == DEFECT_TYPE_EPOXYHOLE)	// Sidefill Epoxy Hole
			//{
			//	iDefectCodeRet = DEFECT_CODE_EPOXYHOLE;
			//}
			if (strLensOKNG == "NG")	// Lens NG
			{
				// 24.05.20 - v2602 - 로그 렌즈 불량명 추가 - LeeGW Start
				if (strDefectDetail == "Contamination")
				{
					iDefectCodeRet = DEFECT_CODE_LENS_CONTAMINATION;
					if (THEAPP.m_pGFunction->ValidHRegion(m_HRgnLensDefectMin))
						concat_obj(HSelectedDefectRgn, m_HRgnLensDefectMin, &HSelectedDefectRgn);
				}
				else if (strDefectDetail == "Scratch")
				{
					iDefectCodeRet = DEFECT_CODE_LENS_SCRATCH;
					if (THEAPP.m_pGFunction->ValidHRegion(m_HRgnLensDefectMin))
						concat_obj(HSelectedDefectRgn, m_HRgnLensDefectMin, &HSelectedDefectRgn);
				}
				else if (strDefectDetail == "Dirt")
				{
					iDefectCodeRet = DEFECT_CODE_LENS_DIRT;
					if (THEAPP.m_pGFunction->ValidHRegion(m_HRgnLensDefectMin))
						concat_obj(HSelectedDefectRgn, m_HRgnLensDefectMin, &HSelectedDefectRgn);
				}
				else if (strDefectDetail == "WhiteDot")
				{
					iDefectCodeRet = DEFECT_CODE_LENS_WHITEDOT;
					if (THEAPP.m_pGFunction->ValidHRegion(m_HRgnLensDefectMin))
						concat_obj(HSelectedDefectRgn, m_HRgnLensDefectMin, &HSelectedDefectRgn);
				}
				// 24.05.20 - v2602 - 로그 렌즈 불량명 추가 - LeeGW End
			}
			else if (strBarrelOKNG=="NG")		// Barrel NG
			{
				if (strDefectDetail=="Dirt")
				{
					iDefectCodeRet = DEFECT_CODE_DIRT;
					if (THEAPP.m_pGFunction->ValidHRegion(m_HRgnDirt))
						concat_obj(HSelectedDefectRgn, m_HRgnDirt, &HSelectedDefectRgn);
				}
				else if (strDefectDetail=="Scratch")
				{
					iDefectCodeRet = DEFECT_CODE_SCRATCH;
					if (THEAPP.m_pGFunction->ValidHRegion(m_HRgnScratch))
						concat_obj(HSelectedDefectRgn, m_HRgnScratch, &HSelectedDefectRgn);
				}
				else if (strDefectDetail=="Stain")
				{
					iDefectCodeRet = DEFECT_CODE_STAIN;
					if (THEAPP.m_pGFunction->ValidHRegion(m_HRgnStain))
						concat_obj(HSelectedDefectRgn, m_HRgnStain, &HSelectedDefectRgn);
				}
				else if (strDefectDetail=="Dent")
				{
					iDefectCodeRet = DEFECT_CODE_DENT;
					if (THEAPP.m_pGFunction->ValidHRegion(m_HRgnDent))
						concat_obj(HSelectedDefectRgn, m_HRgnDent, &HSelectedDefectRgn);
				}
				else if (strDefectDetail=="Chipping")
				{
					iDefectCodeRet = DEFECT_CODE_CHIPPING;
					if (THEAPP.m_pGFunction->ValidHRegion(m_HRgnChipping))
						concat_obj(HSelectedDefectRgn, m_HRgnChipping, &HSelectedDefectRgn);
				}
				else if (strDefectDetail=="Epoxy")
				{
					iDefectCodeRet = DEFECT_CODE_EPOXY;
					if (THEAPP.m_pGFunction->ValidHRegion(m_HRgnEpoxy))
						concat_obj(HSelectedDefectRgn, m_HRgnEpoxy, &HSelectedDefectRgn);
				}
				else if (strDefectDetail=="Outside Contamination")
				{
					iDefectCodeRet = DEFECT_CODE_OUTSIDE_CT;
					if (THEAPP.m_pGFunction->ValidHRegion(m_HRgnOutsideCT))
						concat_obj(HSelectedDefectRgn, m_HRgnOutsideCT, &HSelectedDefectRgn);
				}
				else if (strDefectDetail=="Peel Off")
				{
					iDefectCodeRet = DEFECT_CODE_PEEL_OFF;
					if (THEAPP.m_pGFunction->ValidHRegion(m_HRgnPeelOff))
						concat_obj(HSelectedDefectRgn, m_HRgnPeelOff, &HSelectedDefectRgn);
				}
				else if (strDefectDetail=="WhiteDot")
				{
					iDefectCodeRet = DEFECT_CODE_WHITEDOT;
					if (THEAPP.m_pGFunction->ValidHRegion(m_HRgnWhiteDot))
						concat_obj(HSelectedDefectRgn, m_HRgnWhiteDot, &HSelectedDefectRgn);
				}

			}

			if (THEAPP.m_pGFunction->ValidHRegion(HSelectedDefectRgn))	// 대표불량영역 중 가장 큰 Blob의 면적, 중심위치
			{
				union1(HSelectedDefectRgn, &HSelectedDefectRgn);
				connection (HSelectedDefectRgn, &HSelectedDefectRgn);
				select_shape_std (HSelectedDefectRgn, &HSelectedDefectRgn, "max_area", 70);

				area_center(HSelectedDefectRgn, &lSelectedDefectArea, &dSelectedDefectCenterY, &dSelectedDefectCenterX);

				dSelectedDefectAreaMM2 = ceil(((double)lSelectedDefectArea * PXLAREA)/1000)/1000;
				*pdblArea = dSelectedDefectAreaMM2;
				*pdDefectCenterX = dSelectedDefectCenterX;
				*pdDefectCenterY = dSelectedDefectCenterY;

				THEAPP.m_iModuleDefectCenterX[THEAPP.iModuleCountOneLot] = (int)*pdDefectCenterX;
				THEAPP.m_iModuleDefectCenterY[THEAPP.iModuleCountOneLot] = (int)*pdDefectCenterY;
				THEAPP.m_sModuleDefectName[THEAPP.iModuleCountOneLot] = strDefectDetail;

				bBarrelLensDefect = TRUE;
			}
			else
			{
				*pdblArea = 0;
				*pdDefectCenterX = 0;
				*pdDefectCenterY = 0;

				bBarrelLensDefect = FALSE;
			}

			// Warning Decision
			if (THEAPP.iModuleCountOneLot>=THEAPP.Struct_PreferenceStruct.m_iDefectWarningDefectCount && bBarrelLensDefect)
			{
				for (int iW=1; iW<THEAPP.Struct_PreferenceStruct.m_iDefectWarningDefectCount; iW++)
				{
					iXPosOffset = abs(THEAPP.m_iModuleDefectCenterX[THEAPP.iModuleCountOneLot-iW] - THEAPP.m_iModuleDefectCenterX[THEAPP.iModuleCountOneLot]);
					iYPosOffset = abs(THEAPP.m_iModuleDefectCenterY[THEAPP.iModuleCountOneLot-iW] - THEAPP.m_iModuleDefectCenterY[THEAPP.iModuleCountOneLot]);

					if (iXPosOffset<=THEAPP.Struct_PreferenceStruct.m_iDefectWarningDefectDistance &&
						iYPosOffset<=THEAPP.Struct_PreferenceStruct.m_iDefectWarningDefectDistance &&
						THEAPP.m_sModuleDefectName[THEAPP.iModuleCountOneLot] == THEAPP.m_sModuleDefectName[THEAPP.iModuleCountOneLot-iW])
					{
						bDefectWarning = TRUE;
					}
					else
					{
						bDefectWarning = FALSE;
						break;
					}
				}
			}

			THEAPP.iModuleCountOneLot += 1;

			//나중에 블랙코팅 삭제 예정
			THEAPP.SaveLotResultLog(THEAPP.Struct_PreferenceStruct.m_bSaveLasLog, THEAPP.m_pInspectService->m_sLotID_H, THEAPP.m_pInspectService->m_iTrayNo_H, m_iModuleNo, sBarcodeResult,
				dCMInspectTime, sFinalResult, strBarrelOKNG, strLensOKNG, strDefectItem, strDefectDetail, *pdblArea, (int)*pdDefectCenterX, (int)*pdDefectCenterY,
				sExResult, sExInspType, sExDefectType, *pdExtraArea, (int)*pdExtraDefectCenterX, (int)* pdExtraDefectCenterY,
				sFAIResult, sBarcodeShiftResult, sBarcodeRotationResult, dBarcodePosOffsetX, dBarcodePosOffsetY, dBarcodePosOffsetRotation,
				sBlackCoatingResult, dBlackCoatingDiameter, dBlackCoatingPosX, dBlackCoatingPosY, dO1O2Distance, dDiameterMin,
				sModuleMixResult, sBarcodeErrorResult, sMatchingErrorResult, sLightErrorResult, iDefectCodeRet);

			// ******************* FAI Save ******************************
			THEAPP.SaveFAIResultLog(THEAPP.Struct_PreferenceStruct.m_bSaveLasLog, THEAPP.m_pInspectService->m_sLotID_H, THEAPP.m_pInspectService->m_iTrayNo_H, m_iModuleNo, sBarcodeResult);
		}

		Sleep(1);

		if ((THEAPP.Struct_PreferenceStruct.m_bSaveResultMerge==TRUE && BarrelOrlens==BARREL_LENS_IMAGE_TAB/2)
			|| (THEAPP.Struct_PreferenceStruct.m_bSaveResultMerge==FALSE && BarrelOrlens== BARREL_LENS_IMAGE_TAB-1))
		{
			if (THEAPP.Struct_PreferenceStruct.m_bSaveNGImage)
			{

				if (iDefectCodeRet>= DEFECT_CODE_GOOD)
				{
	
					CString sNgFileName, sNgFileFullPath, sCopyFileFullPath;

					sNgFileName.Format("Module%d_Barcode_%s.jpg", m_iModuleNo, sBarcodeResult);

					THEAPP.m_pSaveManager->Copy(sNgFileName, THEAPP.m_FileBase.m_strOriImageFolderPrev, THEAPP.m_FileBase.m_strNGOriImageFolderPrev, FALSE);
					

					for (int imgIdx = 0; imgIdx < MAX_IMAGE_TAB; imgIdx++)
					{
						if (imgIdx >= THEAPP.m_iMaxInspImageNo)
							break;

						if (THEAPP.Struct_PreferenceStruct.m_bSaveRawImageMerge)
						{
							if (imgIdx == 0)
							{
								sNgFileName.Format("Module%d_Combine_%s*", m_iModuleNo, sBarcodeResult);
								THEAPP.m_pSaveManager->Copy(sNgFileName, THEAPP.m_FileBase.m_strOriImageFolderPrev, THEAPP.m_FileBase.m_strNGOriImageFolderPrev, FALSE);
							}
						}
						else
						{
							sNgFileName.Format("Module%d_%s_%s*", m_iModuleNo, g_sImageName[THEAPP.m_iMachineInspType][imgIdx], sBarcodeResult);
							THEAPP.m_pSaveManager->Copy(sNgFileName, THEAPP.m_FileBase.m_strOriImageFolderPrev, THEAPP.m_FileBase.m_strNGOriImageFolderPrev, FALSE);
						}

						sNgFileName.Format("Module%d_Result_%s_%s*", m_iModuleNo, g_sImageName[THEAPP.m_iMachineInspType][imgIdx], sBarcodeResult);
						THEAPP.m_pSaveManager->Copy(sNgFileName, THEAPP.m_FileBase.m_strOverayImageFolderPrev, THEAPP.m_FileBase.m_strNGOverayImageFolderPrev, FALSE);

						sNgFileName.Format("Module%d__%s_%s*", m_iModuleNo, g_sImageName[THEAPP.m_iMachineInspType][imgIdx], sBarcodeResult);
						THEAPP.m_pSaveManager->Copy(sNgFileName, THEAPP.m_FileBase.m_strReviewImageFolder, THEAPP.m_FileBase.m_strNGReviewImageFolder, FALSE);

					}
					//////////////////////////////////////////////////////////////////////////
				}
			}

			Sleep(1);

			//WCS 12/20
			if(THEAPP.Struct_PreferenceStruct.m_bSaveLasServerImage)
			{
				HANDLE hFindFile;
				WIN32_FIND_DATA FindFileData;

				CString sLotID = THEAPP.m_pInspectService->m_sLotID_H;
				CString sBarcode = sBarcodeResult;
				CString sTime;

				SYSTEMTIME time;
				GetLocalTime(&time);
				sTime.Format("%04d%02d%02d%02d%02d%02d", time.wYear, time.wMonth, time.wDay, time.wHour, time.wMinute, time.wSecond);

				CString sLasFileName, sLasFileFullPath, sOrgFileName, sCopyFileFullPath;
				CString strImgType;	// LeeGW
				BOOL bIsSaveEnd = FALSE;

				// FAI Image Las 업로드
				if (THEAPP.Struct_PreferenceStruct.m_bSaveFAIImage == TRUE)
				{
					for (int i = 0; i < MAX_FAI_ITEM; i++)
					{
						for (int ii = 0; ii < MAX_ONE_FAI_MEASURE_VALUE; ii++)
						{
							if (THEAPP.m_bUseFAI[i] == FALSE)
								continue;

							if (THEAPP.Struct_FAI_ReviewInfo[i][ii].m_bSaveReview == FALSE)
								continue;


							sOrgFileName.Format("Module%d_Result_%s_%s*", m_iModuleNo, THEAPP.Struct_FAI_ReviewInfo[i][ii].m_sFaiName, sBarcodeResult);
							sCopyFileFullPath.Format("%s\\%s", THEAPP.m_FileBase.m_strFAIImageFolder, sOrgFileName);
							if ((hFindFile = FindFirstFile(sCopyFileFullPath, &FindFileData)) != INVALID_HANDLE_VALUE)
							{
								sCopyFileFullPath.Format("%s\\%s", THEAPP.m_FileBase.m_strFAIImageFolder, (LPCTSTR)FindFileData.cFileName);
								FindClose(hFindFile);


								// 24.05.16 - v2602 - 이미지별 불량 표기를 위해 Barrel Surface/Edge, Lens1/2 분리 - LeeGW
								if (THEAPP.Struct_PreferenceStruct.m_bSaveBMP == TRUE)
									sLasFileName.Format("%s__%s_%s_%s_Module%d_Result_%s.bmp", sLotID, THEAPP.Struct_FAI_ReviewInfo[i][ii].m_sFaiName, sBarcode, sTime, m_iModuleNo, sFAIResultOkNg);
								else
									sLasFileName.Format("%s__%s_%s_%s_Module%d_Result_%s.jpg", sLotID, THEAPP.Struct_FAI_ReviewInfo[i][ii].m_sFaiName, sBarcode, sTime, m_iModuleNo, sFAIResultOkNg);

								sLasFileFullPath = THEAPP.GetLASImageDirectory() + "\\" + sLasFileName;


								if (THEAPP.Struct_PreferenceStruct.m_bSaveLasDataZip)	// LeeGW
									THEAPP.m_pSaveManager->SaveLasZip(sLasFileName, sCopyFileFullPath, sLotID, sBarcode, bIsSaveEnd);
								else
									THEAPP.m_pSaveManager->Copy(sCopyFileFullPath, sLasFileFullPath, false);
							}
						}

					}

					if (THEAPP.Struct_PreferenceStruct.m_bSaveResultImage == FALSE && THEAPP.Struct_PreferenceStruct.m_bSaveLasDataZip)
					{
						bIsSaveEnd = TRUE;
						THEAPP.m_pSaveManager->SaveLasZip(sLasFileName, sCopyFileFullPath, sLotID, sBarcode, bIsSaveEnd);
					}			
				}

				// Result Image Las 업로드
				if (THEAPP.Struct_PreferenceStruct.m_bSaveResultImage == TRUE)
				{
					for (int imgIdx = 0; imgIdx < MAX_IMAGE_TAB; imgIdx++)
					{
						if (imgIdx == THEAPP.m_iMaxInspImageNo - 1)
							bIsSaveEnd = TRUE;
						else if (imgIdx >= THEAPP.m_iMaxInspImageNo)
							break;

						// Result
						sOrgFileName.Format("Module%d_Result_%s_%s*", m_iModuleNo, g_sImageName[THEAPP.m_iMachineInspType][imgIdx], sBarcodeResult);
						sCopyFileFullPath.Format("%s\\%s", THEAPP.m_FileBase.m_strOverayImageFolderPrev, sOrgFileName);
						if ((hFindFile = FindFirstFile(sCopyFileFullPath, &FindFileData)) != INVALID_HANDLE_VALUE)
						{
							sCopyFileFullPath.Format("%s\\%s", THEAPP.m_FileBase.m_strOverayImageFolderPrev, (LPCTSTR)FindFileData.cFileName);
							FindClose(hFindFile);

							if (THEAPP.Struct_PreferenceStruct.m_bSaveResultMerge == TRUE)
							{
								if (imgIdx < TRIGGER_LENS1)
									strImgType.Format("Barrel");
								else if (imgIdx < TRIGGER_EXTRA1)
									strImgType.Format("Lens");
								else
								{
									strImgType.Format("%s", g_sImageName[THEAPP.m_iMachineInspType][TRIGGER_EXTRA1]);
									strImgType.Delete(strImgType.GetLength() - 1);
								}
							}
							else
							{
								strImgType.Format("%s", g_sImageName[THEAPP.m_iMachineInspType][imgIdx]);
							}

							// 24.05.16 - v2602 - 이미지별 불량 표기를 위해 Barrel Surface/Edge, Lens1/2 분리 - LeeGW
							if (THEAPP.Struct_PreferenceStruct.m_bSaveBMP == TRUE)
								sLasFileName.Format("%s__%s_%s_%s_Module%d_Result_%s.bmp", sLotID, strImgType, sBarcode, sTime, m_iModuleNo, strImageResult);
							else
								sLasFileName.Format("%s__%s_%s_%s_Module%d_Result_%s.jpg", sLotID, strImgType, sBarcode, sTime, m_iModuleNo, strImageResult);

							sLasFileFullPath = THEAPP.GetLASImageDirectory() + "\\" + sLasFileName;

							if (THEAPP.Struct_PreferenceStruct.m_bSaveLasDataZip)	// LeeGW
								THEAPP.m_pSaveManager->SaveLasZip(sLasFileName, sCopyFileFullPath, sLotID, sBarcode, bIsSaveEnd);
							else
								THEAPP.m_pSaveManager->Copy(sCopyFileFullPath, sLasFileFullPath, false);

						}
					}
				}
			}
		}

		clear_window(HideWindowHandle);
		//set_window_dc ( HideWindowHandle, 0 ) ;
		ReleaseDC(pDC);

		CString sInspectTime;
		tInspectEnd_total = GetTickCount();
				switch(BarrelOrlens) {
		case 0:	sInspectTime.Format("* Module %2d 영상 판정 시간 Surface: %.0lf ms", m_iModuleNo, tInspectEnd_total - tInspectStart_total); break;
		case 1:	sInspectTime.Format("* Module %2d 영상 판정 시간 Edge: %.0lf ms", m_iModuleNo, tInspectEnd_total - tInspectStart_total); break;
		case 2:	sInspectTime.Format("* Module %2d 영상 판정 시간 Lens1: %.0lf ms", m_iModuleNo, tInspectEnd_total - tInspectStart_total); break;
		case 3:	sInspectTime.Format("* Module %2d 영상 판정 시간 Lens2: %.0lf ms", m_iModuleNo, tInspectEnd_total - tInspectStart_total); break;
		}
		THEAPP.SaveDetectLog(sInspectTime);
		THEAPP.SaveLog(sInspectTime);

		return iDefectCodeRet;
	}
	catch(HException &except)
	{

		CString str;
		str.Format("Halcon Exception [CInspectAdminViewHideDlg WriteResultFile] : %s", except.message);
		THEAPP.SaveLog(str);

		return -1;
	}
}

void CInspectAdminViewHideDlg::DecideInspectResult()
{
	if (m_iInspectResult==0)
		return;

	BOOL bResultSet;
	bResultSet = FALSE;

	int iDefectPriorityNum, iDefectIndex;

	for( iDefectPriorityNum=0; iDefectPriorityNum<MAX_DEFECT_NUMBER; iDefectPriorityNum++)
	{
		for( iDefectIndex=0; iDefectIndex<MAX_DEFECT_NUMBER; iDefectIndex++){

			if( THEAPP.Struct_PreferenceStruct.iDefectPriority[iDefectIndex] == iDefectPriorityNum)
			{
				switch( iDefectIndex)
				{
				case DEFECT_NAME_DIRT:
					if( m_iDirt<0){
						m_iInspectResult = m_iDirt;
						bResultSet = TRUE;
					}
					break;
				case DEFECT_NAME_SCRATCH:
					if ( m_iScratch<0){
						m_iInspectResult = m_iScratch;
						bResultSet = TRUE;
					}
					break;
				case DEFECT_NAME_STAIN:
					if ( m_iStain<0){
						m_iInspectResult = m_iStain;
						bResultSet = TRUE;
					}
					break;
				case DEFECT_NAME_DENT:
					if ( m_iDent<0){
						m_iInspectResult = m_iDent;
						bResultSet = TRUE;
					}
					break;
				case DEFECT_NAME_CHIPPING:
					if( m_iChipping<0){
						m_iInspectResult = m_iChipping;
						bResultSet = TRUE;
					}
					break;
				case DEFECT_NAME_EPOXY:
					if( m_iEpoxy<0){
						m_iInspectResult = m_iEpoxy;
						bResultSet = TRUE;
					}
					break;
				case DEFECT_NAME_OUTSIDE_CT:
					if( m_iOutsideCT<0){
						m_iInspectResult = m_iOutsideCT;
						bResultSet = TRUE;
					}
					break;
				case DEFECT_NAME_PEEL_OFF:
					if( m_iPeelOff<0){
						m_iInspectResult = m_iPeelOff;
						bResultSet = TRUE;
					}
					break;
				case DEFECT_NAME_WHITEDOT:
					if( m_iWhiteDot<0){
						m_iInspectResult = m_iWhiteDot;
						bResultSet = TRUE;
					}
					break;
				default:
					break;
				}
			}

			if ( bResultSet == TRUE) 
				break;
		}

		if ( bResultSet == TRUE) 
			break;
	}
}

// 24.06.12 - v2604 - ReviewImage 저장을 위해 추가 - LEEGW START 
int CInspectAdminViewHideDlg::WriteReviewImage (Hobject HImage, Hobject HDefectRgn, CString strFileName)
{
	if (THEAPP.Struct_PreferenceStruct.m_bSaveReviewImage == FALSE)
		return -1;

	try
	{
		double dSaveReviewStart, dSaveReviewEnd;
		dSaveReviewStart = GetTickCount();

		double dRow,dColumn, dRad, dRow2, dColumn2;
		Hobject HDefectCircleRgn, HDefectCircleContour, HDefectCropRgn, HReviewImage;
		Hlong lWidth, lHeigh;
		HTuple HArea, HCenterY, HCenterX;

		CDC* pDC = GetDC();
		HTuple lDC = (INT)(pDC->m_hDC);
		//set_window_dc(HideWindowHandle, lDC);
		clear_window(HideWindowHandle);

		union1(HDefectRgn, &HDefectRgn);
		connection(HDefectRgn, &HDefectRgn);

		area_center(HDefectRgn, &HArea, &HCenterY, &HCenterX);

		tuple_sort_index(HArea, &HArea);
		tuple_inverse(HArea, &HArea);

		select_obj(HDefectRgn, &HDefectRgn, HArea[0].L() + 1);
		inner_circle(HDefectRgn, &dRow, &dColumn, &dRad);
		shape_trans(HDefectRgn, &HDefectCircleRgn, "outer_circle");
		dilation_circle(HDefectCircleRgn, &HDefectCircleRgn, 50);
		
		gen_contour_region_xld (HDefectCircleRgn, &HDefectCircleContour, "border");

		m_dZoomRatio = 2.0;
		m_ViewRect.left =  (Hlong)dColumn - (int)(RECTHEIGHT(&m_ClientRect) / m_dZoomRatio * 0.5);
		m_ViewRect.top =  (Hlong)dRow - (int)(RECTWIDTH(&m_ClientRect) / m_dZoomRatio * 0.5);

		if (m_ViewRect.left < 0 )
			m_ViewRect.left = 0;
		if (m_ViewRect.top < 0)
			m_ViewRect.top = 0;

		m_ViewRect.bottom = m_ViewRect.top + (int)(RECTHEIGHT(&m_ClientRect) / m_dZoomRatio);
		m_ViewRect.right = m_ViewRect.left + (int)(RECTWIDTH(&m_ClientRect) / m_dZoomRatio);

		m_ViewportManager.miStartXPos = m_ViewRect.left;
		m_ViewportManager.miStartYPos = m_ViewRect.top;
		m_ViewportManager.miViewWidth = (int)RECTWIDTH(&m_ClientRect);
		m_ViewportManager.miViewHeight = (int)RECTHEIGHT(&m_ClientRect);
		m_ViewportManager.miImageWidth = m_iImageWidth;
		m_ViewportManager.miImageHeight = m_iImageHeight;
		m_ViewportManager.mdZoomRatio = m_dZoomRatio;

		if (HideWindowHandle > 0) {
			set_part(HideWindowHandle, m_ViewRect.top, m_ViewRect.left, m_ViewRect.bottom, m_ViewRect.right);
			set_window_extents(HideWindowHandle, 0, 0, m_ClientRect.Width(), m_ClientRect.Height());
		}

		disp_obj(HImage, HideWindowHandle);
		set_color(HideWindowHandle,"red");
		set_draw(HideWindowHandle, "margin");
		set_line_width(HideWindowHandle, 5);

		disp_obj(HDefectCircleContour, HideWindowHandle);

		Sleep(1);

		dump_window_image(&HReviewImage, HideWindowHandle);

		THEAPP.m_pSaveManager->SaveImage(HReviewImage, "jpg", THEAPP.m_FileBase.m_strReviewImageFolder + "\\" + strFileName);

		dSaveReviewEnd = GetTickCount();

		CString str;
		str.Format("WriteReviewImage 성공! : %.0lf ms", dSaveReviewEnd - dSaveReviewStart);
		THEAPP.SaveLog(str);

		clear_window(HideWindowHandle);
		//set_window_dc(HideWindowHandle, 0);
		ReleaseDC(pDC);

	}
	catch(HException &except)
	{
		CString str;
		str.Format("Halcon Exception [ CInspectAdminViewHideDlg::WriteReviewImage] : %s", except.message);
		THEAPP.SaveLog(str);

		return FALSE;
	}

}
// 24.06.12 - v2604 - ReviewImage 저장을 위해 추가 - LEEGW END