// InspectViewDlg1.cpp : implementation file
//
#include "stdafx.h"
#include "uscan.h"
#include "InspectAdminViewDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char* THIS_FILE = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CInspectAdminViewDlg dialog

CInspectAdminViewDlg* CInspectAdminViewDlg::m_pInstance = NULL;

CInspectAdminViewDlg* CInspectAdminViewDlg::GetInstance(BOOL bShowFlag)
{
	if (!m_pInstance) {
		m_pInstance = new CInspectAdminViewDlg();
		if(!m_pInstance->m_hWnd) {
			CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
			m_pInstance->Create(IDD_INSPECT_ADMIN_VIEW_DLG, pFrame->GetActiveView());
			if (bShowFlag) m_pInstance->Show();
		}
	}
	return m_pInstance;
}

void CInspectAdminViewDlg::DeleteInstance()
{
	if(m_pInstance->GetSafeHwnd())
		m_pInstance->DestroyWindow();
	SAFE_DELETE(m_pInstance);
}

void CInspectAdminViewDlg::Show()
{
	ShowWindow(SW_HIDE);
	MoveWindow(m_ScreenRect.left, m_ScreenRect.top, m_ScreenRect.right, m_ScreenRect.bottom);
	ShowWindow(SW_SHOW);
}

CInspectAdminViewDlg::CInspectAdminViewDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CInspectAdminViewDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CInspectAdminViewDlg)
	// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	SetPosition(VIEW1_DLG1_LEFT, VIEW1_DLG1_TOP, VIEW1_DLG1_WIDTH + 280, VIEW1_DLG1_HEIGHT + 380);;
	m_lWindowID = -1;
	m_iToolBarOffset = 0;
	m_iScrBarWidth = 16;

	m_bOnPaintNow = FALSE;
	StartTime1 = 0;
	EndTime1 = 0;
	m_bLiveGrabAvailable = TRUE;
	m_bLive = FALSE;
	m_bDispCrossBar = FALSE;
	m_bTestThread = FALSE;

	m_bDisplayFitLine = TRUE;
	m_bDisplayInspectArea = TRUE;
	m_bDisplayThresholdArea = TRUE;
	m_bDisplayDefectArea = TRUE;
	m_bROILock = TRUE;

	ClearAll();
}

void CInspectAdminViewDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CInspectAdminViewDlg)
	DDX_Control(pDX, IDC_VSCRBAR, m_VScrBar);
	DDX_Control(pDX, IDC_HSCRBAR, m_HScrBar);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CInspectAdminViewDlg, CDialog)
	//{{AFX_MSG_MAP(CInspectAdminViewDlg)
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
	ON_COMMAND(ID_TB_ROI_RECT, OnTbROIRect)
	ON_COMMAND(ID_TB_ROI_CIRCLE, OnTbROICircle)
	ON_COMMAND(ID_TB_ROI_POLYGON, OnTbROIPolygon)
	ON_COMMAND(ID_TB_ROI_DELETE, OnTbROIDelete)
	ON_COMMAND(ID_TB_ZOOM_IN, OnTbZoomIn)
	ON_COMMAND(ID_TB_ZOOM_OUT, OnTbZoomOut)
	ON_COMMAND(ID_TB_FIT_WIDTH, OnTbFitWidth)
	ON_COMMAND(ID_TB_FIT_HEIGHT, OnTbFitHeight)
	ON_COMMAND(ID_TB_LIVE, OnTbLive)

	ON_COMMAND(ID_TB_FIT_LINE_DISP, OnTbFitLineDisplay)
	ON_COMMAND(ID_TB_ROI_DISP, OnTbInspectAreaDisplay)
	ON_COMMAND(ID_TB_THRES_DISP, OnTbThresholdAreaDisplay)
	ON_COMMAND(ID_TB_DEFECT_DISP, OnTbDefectAreaDisplay)
	ON_COMMAND(ID_TB_CALIPER, OnTb2pCaliper)
	ON_COMMAND(ID_TB_AVERAGE_GRAY, OnTbAverageGray)
	ON_COMMAND(ID_TB_ROI_LOCK, OnTbROILock)
	ON_COMMAND(ID_TB_CROSS_BAR, OnTbDispCrossBar)

	ON_COMMAND(ID_MENU_ROI_COPY_IMAGE_1, OnMenuRoiCopyImage1)
	ON_COMMAND(ID_MENU_ROI_COPY_IMAGE_2, OnMenuRoiCopyImage2)
	ON_COMMAND(ID_MENU_ROI_COPY_IMAGE_3, OnMenuRoiCopyImage3)
	ON_COMMAND(ID_MENU_ROI_COPY_IMAGE_4, OnMenuRoiCopyImage4)
	ON_COMMAND(ID_MENU_ROI_COPY_IMAGE_5, OnMenuRoiCopyImage5)
	ON_COMMAND(ID_MENU_ROI_COPY_IMAGE_6, OnMenuRoiCopyImage6)
	ON_COMMAND(ID_MENU_ROI_COPY_IMAGE_7, OnMenuRoiCopyImage7)
	ON_COMMAND(ID_MENU_ROI_TYPE_CHANGE, OnMenuROITypeChange)
	ON_COMMAND(ID_MENU_ROI_ID_CHANGE, OnMenuROIIDChange)

	ON_WM_CREATE()
	//}}AFX_MSG_MAP

	ON_MESSAGE(UM_2DFRAME_READY, On2DFrameReady)
	ON_MESSAGE(UM_RS_GOTO_DEFECT, OnEventGoToDefect)	
	ON_WM_TIMER()
	ON_WM_RBUTTONDOWN()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CInspectAdminViewDlg message handlers

void CInspectAdminViewDlg::ClearAll()
{
	m_dZoomRatio = 0.8;

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
	m_pLastSelectedRegion = NULL;	// 24.02.28 Local Align 추가 - LeeGW

	m_ToolBarState = TS_SELECT_PART;
	m_bOnMoving = FALSE;
	m_bDisplayImage = FALSE;
	m_iInspectingFrameIndex = -1;

	m_iDefectIdx = -1;

	m_HAutoFocusCheckRgn.Reset();

	m_HCrossBarXLD.Reset();
	m_pThresRgn.Reset();
	m_HInspectAreaRgn.Reset();
	m_pHImage.Reset();
	m_HShapeModelContrastRgn.Reset();
	m_HCropMatchingImage.Reset();

	for (int cont = 0; cont < MAX_CONTOUR_NUM; cont++)
	{
		ContoursAffinTrans[cont].Reset();
		gen_empty_obj(&ContoursAffinTrans[cont]);
	}

	// 24.04.04 - v2645 - 이미지별 매칭 디스플레이 - LeeGW Star
	int tab;
	for(tab = 0; tab < BARREL_LENS_IMAGE_TAB; tab++)
	{
		for (int cont = 0; cont < MAX_CONTOUR_NUM; cont++)
		{
			Contours[tab][cont].Reset();
			gen_empty_obj(&Contours[tab][cont]);
		}
	}
	// 24.04.04 - v2645 - 이미지별 매칭 디스플레이 - LeeGW End
	
	int i;

	for (i=0; i<MAX_IMAGE_TAB; i++)
	{
		m_HTeachingImage[i].Reset();
		gen_empty_obj(m_HTeachingImage+i);
	}

	gen_empty_obj(&m_HCrossBarXLD);
	gen_empty_obj(&m_HAutoFocusCheckRgn);
	gen_empty_obj(&m_pThresRgn);
	gen_empty_obj(&m_pHImage);
	gen_empty_obj(&m_HShapeModelContrastRgn);
	gen_empty_obj(&m_HCropMatchingImage);

	m_caliper_control_flag = 0;
	m_AverageGray_control_flag = 0;

	m_iNoCaliperPoint = 0;
	mpLineSP = CPoint(0,0);
	mpLineEP = CPoint(0,0);

	m_bPolygonMode = FALSE;
	m_iPolygonMode = 0;
	m_iPolygonCnt = 0;

	for (i = 0; i < MAX_MODULE_ONE_TRAY; i++)
	{
		m_iBarcodeGrabStatus[i] = BARCODE_GRAB_STATUS_NOT_READY;
		m_iInspectGrabStatus[i] = INSPECT_GRAB_STATUS_NOT_READY;
	}
}

LRESULT CInspectAdminViewDlg::OnEventPcbModelChanaged(WPARAM wParam, LPARAM lParam)
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
LRESULT CInspectAdminViewDlg::OnEventFrameInspectDone(WPARAM wParam, LPARAM lParam)
{
	m_iInspectingFrameIndex = (int)wParam;

	return 0;
}

BOOL CInspectAdminViewDlg::PreTranslateMessage(MSG* pMsg) 
{
	switch (pMsg->message) 
	{
	case WM_NCLBUTTONDOWN :
		SetActiveWindow();
		return TRUE;
	case WM_KEYDOWN:
		{
			if( (pMsg->wParam == VK_RETURN || pMsg->wParam == VK_ESCAPE)
				|| (pMsg->wParam == VK_F4) )
				return TRUE;

			if( (pMsg->wParam==VK_LEFT || pMsg->wParam==VK_RIGHT || pMsg->wParam==VK_UP || pMsg->wParam==VK_DOWN)==TRUE
				&& (mpActiveTRegion && m_bROILock==FALSE)==FALSE)
				return FALSE;

			if(GetKeyState(VK_CONTROL) < 0)
			{
				if (mpActiveTRegion && m_bROILock==FALSE)
				{
					switch(pMsg->wParam)
					{
					case VK_UP:
						ScaleActiveRegion(ROI_KEY_UP, TRUE);
						UpdateView(FALSE);
						return TRUE;
					case VK_DOWN:
						ScaleActiveRegion(ROI_KEY_DOWN, TRUE);
						UpdateView(FALSE);
						return TRUE;
					case VK_LEFT:
						ScaleActiveRegion(ROI_KEY_LEFT, TRUE);
						UpdateView(FALSE);
						return TRUE;
					case VK_RIGHT:
						ScaleActiveRegion(ROI_KEY_RIGHT, TRUE);
						UpdateView(FALSE);
						return TRUE;
					}
				}
			}

			if(GetKeyState(VK_SHIFT) < 0)
			{
				if (mpActiveTRegion && m_bROILock==FALSE)
				{
					switch(pMsg->wParam)
					{
					case VK_UP:
						ScaleActiveRegion(ROI_KEY_UP, FALSE);
						UpdateView(FALSE);
						return TRUE;
					case VK_DOWN:
						ScaleActiveRegion(ROI_KEY_DOWN, FALSE);
						UpdateView(FALSE);
						return TRUE;
					case VK_LEFT:
						ScaleActiveRegion(ROI_KEY_LEFT, FALSE);
						UpdateView(FALSE);
						return TRUE;
					case VK_RIGHT:
						ScaleActiveRegion(ROI_KEY_RIGHT, FALSE);
						UpdateView(FALSE);
						return TRUE;
					}
				}
			}

			if (mpActiveTRegion && m_bROILock==FALSE)
			{
				switch(pMsg->wParam)
				{
				case VK_UP:
					MoveActiveRegion(ROI_KEY_UP);
					UpdateView(FALSE);
					return TRUE;
				case VK_DOWN:
					MoveActiveRegion(ROI_KEY_DOWN);
					UpdateView(FALSE);
					return TRUE;
				case VK_LEFT:
					MoveActiveRegion(ROI_KEY_LEFT);
					UpdateView(FALSE);
					return TRUE;
				case VK_RIGHT:
					MoveActiveRegion(ROI_KEY_RIGHT);
					UpdateView(FALSE);
					return TRUE;
				}
			}

			break;
		}

	}
	return CDialog::PreTranslateMessage(pMsg);
}

// Scan Select ComboBox
int CInspectAdminViewDlg::Get_CurScanTab()
{
	return m_ViewToolbar.Get_CurComboBox();
}

void CInspectAdminViewDlg::Set_CurScanTab(int nScanNum)
{ 
	CString strScan;
	strScan.Format("SCAN_%d", nScanNum);
	m_ViewToolbar.Set_CurComboBox(strScan);
}

void CInspectAdminViewDlg::Add_ScanTab(int nScanNum)
{
	CString strScan;
	strScan.Format("SCAN_%d", nScanNum);
	m_ViewToolbar.Add_ComboBox(strScan);
}

void CInspectAdminViewDlg::Clear_ScanTab()
{
	m_ViewToolbar.Clear_ComboBox();
}

int CInspectAdminViewDlg::Get_ScanTabCount()
{
	return m_ViewToolbar.Get_ComboBoxCount();
}

int CInspectAdminViewDlg::Get_ScanTabIndex()
{
	return m_ViewToolbar.Get_ComboBoxIndex();
}

void CInspectAdminViewDlg::Set_ScanTabIndex(int nIdx)
{
	m_ViewToolbar.Set_ComboBoxIndex(nIdx);
}

BOOL CInspectAdminViewDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	// Toolbar
	CRect ToolbarBorder(3, 3, 3, 3);
	if (!m_ViewToolbar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_BORDER | WS_VISIBLE | CBRS_TOP | CBRS_TOOLTIPS, ToolbarBorder) || 
		!m_ViewToolbar.LoadToolBar(IDR_ADMIN_VIEW_TOOLBAR)) {
			TRACE0("Failed to create toolbar\n");
			return -1;      // fail to create
	}

	CRect    rcClientOld; // Old Client Rect
	CRect    rcClientNew; // New Client Rect with Tollbar Added
	GetClientRect(rcClientOld); // Retrive the Old Client WindowSize
	RepositionBars(AFX_IDW_CONTROLBAR_FIRST, AFX_IDW_CONTROLBAR_LAST, 0, reposQuery, rcClientNew);

	m_iToolBarOffset = rcClientNew.top - rcClientOld.top;

	RepositionBars(AFX_IDW_CONTROLBAR_FIRST, AFX_IDW_CONTROLBAR_LAST, 0);

	m_ViewToolbar.SetButtonStyle(0, TBBS_CHECKGROUP);		// Selection
	m_ViewToolbar.SetButtonStyle(1, TBBS_CHECKGROUP);		// Grab
	m_ViewToolbar.SetButtonStyle(2, TBBS_CHECKGROUP);		// Selected Zoom
	m_ViewToolbar.SetButtonStyle(3, TBBS_CHECKGROUP);		// Rect ROI
	m_ViewToolbar.SetButtonStyle(4, TBBS_CHECKGROUP);		// Circle ROI
	m_ViewToolbar.SetButtonStyle(5, TBBS_CHECKGROUP);		// Polygon ROI

	m_ViewToolbar.SetButtonStyle(15, TBBS_CHECKBOX);		// Live

	m_ViewToolbar.SetButtonStyle(17, TBBS_CHECKBOX);		// ROI
	m_ViewToolbar.SetButtonStyle(18, TBBS_CHECKBOX);		// Margin
	m_ViewToolbar.SetButtonStyle(19, TBBS_CHECKBOX);		// Threshold
	m_ViewToolbar.SetButtonStyle(20, TBBS_CHECKBOX);		// NG

	m_ViewToolbar.SetButtonStyle(24, TBBS_CHECKBOX);		// Average GV
	m_ViewToolbar.SetButtonStyle(25, TBBS_CHECKBOX);		// ROI Lock

	CToolBarCtrl &toolbarctrl = m_ViewToolbar.GetToolBarCtrl();
	toolbarctrl.CheckButton(ID_TB_SELECT_PART, FALSE);
	toolbarctrl.CheckButton(ID_TB_LIVE, FALSE);

	m_bLiveGrabAvailable = TRUE;
	m_bLive = FALSE;

	toolbarctrl.CheckButton(ID_TB_FIT_LINE_DISP, TRUE);
	toolbarctrl.CheckButton(ID_TB_ROI_DISP, TRUE);
	toolbarctrl.CheckButton(ID_TB_THRES_DISP, TRUE);
	toolbarctrl.CheckButton(ID_TB_DEFECT_DISP, TRUE);

	toolbarctrl.CheckButton(ID_TB_CALIPER, FALSE);
	toolbarctrl.CheckButton(ID_TB_AVERAGE_GRAY, FALSE);
	toolbarctrl.CheckButton(ID_TB_ROI_LOCK, m_bROILock);
	toolbarctrl.CheckButton(ID_TB_CROSS_BAR, FALSE);

	m_caliper_control_flag = 0;
	m_AverageGray_control_flag = 0;
	m_bDispCrossBar = FALSE;

	m_iNoCaliperPoint = 0;
	mpLineSP = CPoint(0,0);
	mpLineEP = CPoint(0,0);

	toolbarctrl.SetState(ID_TB_LIVE, TBSTATE_HIDDEN);

	toolbarctrl.SetState(ID_TB_FIT_LINE_DISP, TBSTATE_HIDDEN);
	toolbarctrl.SetState(ID_TB_ROI_DISP, TBSTATE_HIDDEN);
	toolbarctrl.SetState(ID_TB_THRES_DISP, TBSTATE_HIDDEN);
	toolbarctrl.SetState(ID_TB_DEFECT_DISP, TBSTATE_HIDDEN);
	toolbarctrl.SetState(ID_TB_CALIPER, TBSTATE_HIDDEN);
	toolbarctrl.SetState(ID_TB_AVERAGE_GRAY, TBSTATE_HIDDEN);
	toolbarctrl.SetState(ID_TB_CROSS_BAR, TBSTATE_HIDDEN);

	GetClientRect(&m_ClientRect);
	m_bInspectRunDisplay = FALSE;
	Reset();
	m_bDisplayImage = TRUE;

	m_HIconSelectedPad = AfxGetApp()->LoadIcon(IDI_SELECTED_PAD_ARROW);

	m_Menu.LoadMenu(IDR_MENU_INSPECT_ADMIN_VIEW);

	gen_cross_contour_xld(&m_HCrossBarXLD, CAMERA_PARAM_RESOLUTION/2, CAMERA_PARAM_RESOLUTION/2, (double)CAMERA_PARAM_RESOLUTION*2.0, 0.0);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

BOOL CInspectAdminViewDlg::OnEraseBkgnd(CDC *pDC)
{
	pDC->FillSolidRect(m_ClientRect, TS_COLOR_EXTRA_BG);
	return TRUE;
}



void CInspectAdminViewDlg::OnPaint() 
{
	CPaintDC dc(this); // device context for painting


	if (m_lWindowID < 0 || !m_bDisplayImage || m_bOnPaintNow) 
		return;

	m_bOnPaintNow = TRUE;

	CDC* pDC = GetDC();

	POINT ClientOffset;
	ClientOffset.x = 0;
	ClientOffset.y = m_iToolBarOffset;
	pDC->SetViewportOrg(ClientOffset);

	dc.OffsetViewportOrg(0, m_iToolBarOffset);

	HTuple lDC = (INT)(pDC->m_hDC);
	set_window_dc ( m_lWindowID, lDC );

	CRgn ClipRgn;
	ClipRgn.CreateRectRgn(m_ClientRect.left ,m_ClientRect.top, m_ClientRect.right, m_ClientRect.bottom);
	pDC->SelectClipRgn(&ClipRgn);

	if(THEAPP.m_pGFunction->ValidHImage(m_pHImage))
	{
		disp_obj(m_pHImage, m_lWindowID);
	}

	//WCS 2019/04/24
	if(m_AverageGray_control_flag == 1)
	{
		if(THEAPP.m_pGFunction->ValidHImage(m_pHImage))
		{
			Hobject HGetdomain;
			get_domain(m_pHImage, &HGetdomain);

			double Mean, Deviation;
			intensity(HGetdomain,m_pHImage,&Mean,&Deviation);

			CString Str;
			Str.Format("평균 밝기 : %d", (int)Mean);

			set_display_font(m_lWindowID, 60, "mono", "true", "false");
			set_color(m_lWindowID,"red");

			set_tposition(m_lWindowID, 50, 50);
			write_string(m_lWindowID,  (LPCTSTR)Str);
		}
	}

	if(m_bInspectRunDisplay==TRUE)
	{
		if(THEAPP.m_pGFunction->ValidHRegion(m_HResultDefectRgn))
		{
			set_color(m_lWindowID, "red");
			set_draw(m_lWindowID, "margin");
			disp_obj(m_HResultDefectRgn,m_lWindowID);
		}
	}
	
	if(THEAPP.m_iModeSwitch==MODE_ADMIN_TEACH_VIEW)
	{
		switch(THEAPP.m_pTabControlDlg->m_iCurrentTab)
		{
		case 0:
			{
				
				if(THEAPP.m_pGFunction->ValidHImage(m_pHImage))
				{
					if(m_bLive==FALSE)
						disp_obj(m_pHImage, m_lWindowID);
				}
				
				if(THEAPP.m_pInspectSummary->m_bUniformityCheckMode==TRUE)
				{
					if(THEAPP.m_pGFunction->ValidHXLD(ContoursAffinTrans1_Uniformity))
					{
						if(m_bLive==FALSE)
						{
							set_color(m_lWindowID, "red");
							set_draw(m_lWindowID, "margin");
							disp_obj(ContoursAffinTrans1_Uniformity,m_lWindowID);
						}
					}
				}

				if(THEAPP.m_pGFunction->ValidHXLD(m_HBlackCoatingCircleXLD))
				{
					set_color(m_lWindowID, "magenta");
					set_draw(m_lWindowID, "margin");
					disp_obj(m_HBlackCoatingCircleXLD,m_lWindowID);

					Hobject HCrossBarXLD;
					gen_cross_contour_xld(&HCrossBarXLD, m_dBlackCoatingCenterRow, m_dBlackCoatingCenterCol, 15, 0.0);
					disp_obj(HCrossBarXLD,m_lWindowID);
				}

				if(THEAPP.m_pGFunction->ValidHXLD(m_HDatumCircleXLD))
				{
					set_color(m_lWindowID, "cyan");
					set_draw(m_lWindowID, "margin");
					disp_obj(m_HDatumCircleXLD,m_lWindowID);

					Hobject HCrossBarXLD;
					gen_cross_contour_xld(&HCrossBarXLD, m_dDatumCenterRow, m_dDatumCenterCol, 15, 0.0);
					disp_obj(HCrossBarXLD,m_lWindowID);
				}

				if(THEAPP.m_pGFunction->ValidHXLD(m_HBlackCoatingCircleXLD) && THEAPP.m_pGFunction->ValidHXLD(m_HDatumCircleXLD))
				{
					set_color(m_lWindowID, "red");
					set_draw(m_lWindowID, "margin");
					disp_obj(m_HDiamSBLineXLD, m_lWindowID);

					set_display_font(m_lWindowID, 8, "mono", "true", "false");
					set_color(m_lWindowID,"red");

					CString strArea;
					strArea.Format ("Diam Min :%.03lf mm", m_dDiameterMin);

					set_tposition(m_lWindowID, (int)m_dDatumCenterRow, (int)m_dDatumCenterCol);
					write_string(m_lWindowID,  (LPCTSTR)strArea);
				}

				
				if(THEAPP.m_pGFunction->ValidHRegion(m_HBlackCoatingEdgePointRgn))
				{
					set_color(m_lWindowID, "red");
					set_draw(m_lWindowID, "margin");
					disp_obj(m_HBlackCoatingEdgePointRgn,m_lWindowID);
				}
				if(THEAPP.m_pGFunction->ValidHRegion(m_HDatumEdgePointRgn))
				{
					set_color(m_lWindowID, "red");
					set_draw(m_lWindowID, "margin");
					disp_obj(m_HDatumEdgePointRgn,m_lWindowID);
				}

				break;
			}
		case 1:
			{
				if(THEAPP.m_pInspectSummary->m_bUniformityCheckMode==TRUE)
				{
					if(THEAPP.m_pGFunction->ValidHXLD(ContoursAffinTrans1_Uniformity))
					{
						if(m_bLive==FALSE)
						{
							set_color(m_lWindowID, "red");
							set_draw(m_lWindowID, "margin");
							disp_obj(ContoursAffinTrans1_Uniformity,m_lWindowID);
							if(THEAPP.m_pGFunction->ValidHRegion(Partition_Region))
							{
								disp_obj(Partition_Region,m_lWindowID);
								UniformityDisplay();
							}
						}
					}
				}
				if(THEAPP.m_pGFunction->ValidHImage(m_HTeachingImage[THEAPP.m_pTabControlDlg->m_iCurrentTab-1]))
				{
					if(m_bLive==FALSE)
						disp_obj(m_HTeachingImage[THEAPP.m_pTabControlDlg->m_iCurrentTab-1], m_lWindowID);
				}
				break;
			}
		case 2:
			{
				if(THEAPP.m_pInspectSummary->m_bUniformityCheckMode==TRUE)
				{
				if(THEAPP.m_pGFunction->ValidHXLD(ContoursAffinTrans1_Uniformity))
				{
					if(m_bLive==FALSE)
					{
						set_color(m_lWindowID, "red");
						set_draw(m_lWindowID, "margin");
						disp_obj(ContoursAffinTrans1_Uniformity,m_lWindowID);
						if(THEAPP.m_pGFunction->ValidHRegion(Partition_Region))
						{
							disp_obj(Partition_Region,m_lWindowID);
							UniformityDisplay();
						}
					}
				}
				}
				if(THEAPP.m_pGFunction->ValidHImage(m_HTeachingImage[THEAPP.m_pTabControlDlg->m_iCurrentTab-1]))
				{
					if(m_bLive==FALSE)
						disp_obj(m_HTeachingImage[THEAPP.m_pTabControlDlg->m_iCurrentTab-1], m_lWindowID);
				}
				break;
			}
		case 3:
			{
				if(THEAPP.m_pInspectSummary->m_bUniformityCheckMode==TRUE)
				{
				if(THEAPP.m_pGFunction->ValidHXLD(ContoursAffinTrans1_Uniformity))
				{
					if(m_bLive==FALSE)
					{
						set_color(m_lWindowID, "red");
						set_draw(m_lWindowID, "margin");
						disp_obj(ContoursAffinTrans1_Uniformity,m_lWindowID);
						if(THEAPP.m_pGFunction->ValidHRegion(Partition_Region))
						{
							disp_obj(Partition_Region,m_lWindowID);
							UniformityDisplay();
						}
						
					}
				}
				}
				if(THEAPP.m_pGFunction->ValidHImage(m_HTeachingImage[THEAPP.m_pTabControlDlg->m_iCurrentTab-1]))
				{
					if(m_bLive==FALSE)
						disp_obj(m_HTeachingImage[THEAPP.m_pTabControlDlg->m_iCurrentTab-1], m_lWindowID);
				}
				break;
			}
		case 4:
			{
				if(THEAPP.m_pInspectSummary->m_bUniformityCheckMode==TRUE)
				{
				if(THEAPP.m_pGFunction->ValidHXLD(ContoursAffinTrans1_Uniformity))
				{
					if(m_bLive==FALSE)
					{
						set_color(m_lWindowID, "red");
						set_draw(m_lWindowID, "margin");
						disp_obj(ContoursAffinTrans1_Uniformity,m_lWindowID);
						if(THEAPP.m_pGFunction->ValidHRegion(Partition_Region))
						{
							disp_obj(Partition_Region,m_lWindowID);
							UniformityDisplay();
						}
					}
				}
				}
				if(THEAPP.m_pGFunction->ValidHImage(m_HTeachingImage[THEAPP.m_pTabControlDlg->m_iCurrentTab-1]))
				{
					if(m_bLive==FALSE)
						disp_obj(m_HTeachingImage[THEAPP.m_pTabControlDlg->m_iCurrentTab-1], m_lWindowID);
				}
				break;
			}
		case 5:
			{
				if(THEAPP.m_pInspectSummary->m_bUniformityCheckMode==TRUE)
				{
					if(THEAPP.m_pGFunction->ValidHXLD(ContoursAffinTrans1_Uniformity))
					{
						if(m_bLive==FALSE)
						{
							set_color(m_lWindowID, "red");
							set_draw(m_lWindowID, "margin");
							disp_obj(ContoursAffinTrans1_Uniformity,m_lWindowID);
							if(THEAPP.m_pGFunction->ValidHRegion(Partition_Region))
							{
								disp_obj(Partition_Region,m_lWindowID);
								UniformityDisplay();
							}
						}
					}
				}
				if(THEAPP.m_pGFunction->ValidHImage(m_HTeachingImage[THEAPP.m_pTabControlDlg->m_iCurrentTab-1]))
				{
					if(m_bLive==FALSE)
						disp_obj(m_HTeachingImage[THEAPP.m_pTabControlDlg->m_iCurrentTab-1], m_lWindowID);
				}
				break;
			}
		case 6:
			{
				if(THEAPP.m_pInspectSummary->m_bUniformityCheckMode==TRUE)
				{
					if(THEAPP.m_pGFunction->ValidHXLD(ContoursAffinTrans1_Uniformity))
					{
						if(m_bLive==FALSE)
						{
							set_color(m_lWindowID, "red");
							set_draw(m_lWindowID, "margin");
							disp_obj(ContoursAffinTrans1_Uniformity,m_lWindowID);
							if(THEAPP.m_pGFunction->ValidHRegion(Partition_Region))
							{
								disp_obj(Partition_Region,m_lWindowID);
								UniformityDisplay();
							}
						}
					}
				}
				if(THEAPP.m_pGFunction->ValidHImage(m_HTeachingImage[THEAPP.m_pTabControlDlg->m_iCurrentTab-1]))
				{
					if(m_bLive==FALSE)
						disp_obj(m_HTeachingImage[THEAPP.m_pTabControlDlg->m_iCurrentTab-1], m_lWindowID);
				}
				break;
			}
		}

		// 24.04.04 - v2645 - 이미지별 매칭 디스플레이 - LeeGW Start
		if (THEAPP.m_pTabControlDlg->m_iCurrentTab > 0 && THEAPP.m_pTabControlDlg->m_iCurrentTab <= BARREL_LENS_IMAGE_TAB)
		{
			for (int cont = 0; cont < MAX_CONTOUR_NUM; cont++)
			{
				if (THEAPP.m_pGFunction->ValidHXLD(THEAPP.m_pInspectAdminViewDlg->Contours[THEAPP.m_pTabControlDlg->m_iCurrentTab - 1][cont]))
					copy_obj(THEAPP.m_pInspectAdminViewDlg->Contours[THEAPP.m_pTabControlDlg->m_iCurrentTab - 1][cont], &THEAPP.m_pInspectAdminViewDlg->ContoursAffinTrans[cont], 1, 1);
			}			
		}
		// 24.04.04 - v2645 - 이미지별 매칭 디스플레이 - LeeGW End

		// Thresholding Arrea Display
		if (m_bDisplayThresholdArea)
		{
			if(THEAPP.m_pGFunction->ValidHRegion(m_pThresRgn))
			{
				set_color(m_lWindowID, "red");
				set_draw(m_lWindowID, "fill");
				disp_obj(m_pThresRgn,m_lWindowID);
			}
		}

		// Inspection Area Display
		if (m_bDisplayInspectArea)
		{
			if(THEAPP.m_pGFunction->ValidHRegion(m_HInspectAreaRgn))
			{
				set_color(m_lWindowID, "cyan");
				set_draw(m_lWindowID, "margin");
				disp_obj(m_HInspectAreaRgn,m_lWindowID);
			}
		}

		if(THEAPP.m_pGFunction->ValidHImage(m_HCropMatchingImage))
		{
			set_color(m_lWindowID, "red");
			set_draw(m_lWindowID, "margin");
			disp_rectangle1(m_lWindowID, CropLTPoint.y, CropLTPoint.x, CropRBPoint.y, CropRBPoint.x);
		}

		// Fit Line Display

		if(m_bDisplayFitLine)
		{
			if (THEAPP.m_pTabControlDlg->m_iCurrentTab>=0 && THEAPP.m_pTabControlDlg->m_iCurrentTab<=BARREL_LENS_IMAGE_TAB)
			{
				for (int cont = 0; cont < MAX_CONTOUR_NUM; cont++)
				{

					if (THEAPP.m_pModelDataManager->m_bOneBarrel && cont < MATCHING_CONTOUR_3)
						continue;

					if (THEAPP.m_pGFunction->ValidHXLD(ContoursAffinTrans[cont]))
					{
						if (m_bLive == FALSE)
						{
							set_color(m_lWindowID, "red");
							set_draw(m_lWindowID, "margin");
							disp_obj(ContoursAffinTrans[cont], m_lWindowID);
						}
					}
				}	
			}	// if (THEAPP.m_pTabControlDlg->m_iCurrentTab>=0 && THEAPP.m_pTabControlDlg->m_iCurrentTab<=4)
			// 24.03.18 else 삭제 - LeeGW
			if(m_bLive==FALSE)
			{
				THEAPP.m_pModelDataManager->m_pInspectionArea->Draw(pDC->m_hDC, &m_ViewportManager, GTR_DS_NORMAL, FALSE, THEAPP.m_pCalDataService, THEAPP.m_pTabControlDlg->m_iCurrentTab);

				// Region ROI
				GTRegion *pRegion;
				for (int k=0; k<THEAPP.m_pModelDataManager->m_pInspectionArea->GetChildTRegionCount(); k++)
				{
					pRegion = THEAPP.m_pModelDataManager->m_pInspectionArea->GetChildTRegion(k);
					if (pRegion == NULL)
						continue;

					if (pRegion->GetVisible())
					{
						if (pRegion->m_bRegionROI)
						{
							if (pRegion->m_iTeachImageIndex==THEAPP.m_pTabControlDlg->m_iCurrentTab)	// 24.03.18 pRegion->m_iTeachImageIndex>=5 && 삭제 - LeeGW
							{
								set_draw(m_lWindowID, "margin");

								if (pRegion->m_iInspectionType==INSPECTION_TYPE_INSPECTION)		// Inspection ROI
								{
									if (pRegion->mbLastSelected)
										set_color(m_lWindowID, "red");
									else
										set_color(m_lWindowID, "green");
								}
								else if (pRegion->m_iInspectionType == INSPECTION_TYPE_AI_INSPECTION)	// AI Inspection ROI
								{
									if (pRegion->mbLastSelected)
										set_color(m_lWindowID, "red");
									else
										set_color(m_lWindowID, "yellow");
								}
								else		// Teaching ROI
								{
									if (pRegion->m_iInspectionType == INSPECTION_TYPE_ALIGN)
										set_color(m_lWindowID, "cyan");
									if (pRegion->m_iInspectionType == INSPECTION_TYPE_DELETE)
										set_color(m_lWindowID, "blue");
									else if (pRegion->m_iInspectionType == INSPECTION_TYPE_ADD)
										set_color(m_lWindowID, "orange");
									else if (pRegion->m_iInspectionType == INSPECTION_TYPE_LOCAL_ALIGN)	// 24.02.29 Local Align 추가 - LeeGW
										set_color(m_lWindowID, "magenta");
									else if ((pRegion->m_iInspectionType >= INSPECTION_TYPE_FAI_START && pRegion->m_iInspectionType <= INSPECTION_TYPE_FAI_END))
										set_color(m_lWindowID, "orange");
								}

								if (pRegion == mpActiveTRegion)
								{
									set_line_width(m_lWindowID, 2);
									set_line_style(m_lWindowID, (HTuple(20).Append(10)));
								}

								if(THEAPP.m_pGFunction->ValidHRegion(pRegion->m_HTeachPolygonRgn))	
								{
									disp_obj(pRegion->m_HTeachPolygonRgn, m_lWindowID);	
								}

								if (pRegion == mpActiveTRegion)
								{
									set_line_width(m_lWindowID, 1);
									set_line_style(m_lWindowID, HTuple());
								}
							}
						}	// if (pRegion->m_bRegionROI)

					}	// if (pRegion->GetVisible())
				}

				if (mpActiveTRegion)
					mpActiveTRegion->Draw(pDC->m_hDC, &m_ViewportManager, GTR_DS_NORMAL, TRUE, THEAPP.m_pCalDataService, THEAPP.m_pTabControlDlg->m_iCurrentTab);
			}
		}

		if (m_bDisplayDefectArea)
		{
			if(THEAPP.m_pGFunction->ValidHRegion(m_HParameterDefectRgn))
			{
				set_color(m_lWindowID, "red");
				set_draw(m_lWindowID, "margin");
				disp_obj(m_HParameterDefectRgn,m_lWindowID);

				HFONT CurrFont, OldFont;
				CurrFont = CreateFont(17, 0, 0, 0, 700, 0, 0, 0, ANSI_CHARSET, 0, 0, 0, 0, "Arial");
				OldFont = (HFONT)SelectObject(pDC->m_hDC, CurrFont);
				SetBkMode(pDC->m_hDC, TRANSPARENT);
				SetTextColor(pDC->m_hDC, RGB(255,0,0));

				CString strArea;
				Hobject HDefectConnRgn, HSelectedRgn;
				Hlong lNoDefects = 0;
				Hlong lArea;
				double dCompArea;
				double dCenterX, dCenterY;
				POINT CenterPoint;

				connection(m_HParameterDefectRgn, &HDefectConnRgn);
				count_obj(HDefectConnRgn, &lNoDefects);

				for (int iii=0; iii<lNoDefects; iii++)
				{
					select_obj(HDefectConnRgn, &HSelectedRgn, iii+1);

					if (THEAPP.m_pGFunction->ValidHRegion(HSelectedRgn)==FALSE)
						continue;

					area_center(HSelectedRgn, &lArea, &dCenterY, &dCenterX);

					dCompArea = ceil(((double)lArea * PXLAREA)/1000)/1000;

					CenterPoint.x = (long)dCenterX;
					CenterPoint.y = (long)dCenterY;
					
					m_ViewportManager.IPtoVP(&CenterPoint, 1);

					strArea.Format ("%.03lf mm2 (%d)", dCompArea, lArea);
					TextOut(pDC->m_hDC, CenterPoint.x, CenterPoint.y, (LPCTSTR)strArea.GetBuffer(0), strlen((LPCTSTR)strArea));
				}

				SelectObject(pDC->m_hDC, OldFont);
				DeleteObject(CurrFont);
			}

			if(THEAPP.m_pGFunction->ValidHXLD(m_HParameterDefectXLD))
			{
				set_color(m_lWindowID, "yellow");
				disp_obj(m_HParameterDefectXLD,m_lWindowID);
			}
		}



		/////////////////////////////////////////////////////////
		// Caliper
		/////////////////////////////////////////////////////////

		if (m_caliper_control_flag == 1)
		{
			if (m_iNoCaliperPoint==2)
				DrawArrow(pDC);
			else if (m_iNoCaliperPoint==1)
			{
				THEAPP.m_pGFunction->DisplayCross(pDC->m_hDC, &m_ViewportManager, mpLineSP.x, mpLineSP.y, RGB(0,255,0), 50);

				POINT FirstCaliperPoint;
				FirstCaliperPoint.x = (long)mpLineSP.x;
				FirstCaliperPoint.y = (long)mpLineSP.y;

				m_ViewportManager.IPtoVP(&FirstCaliperPoint, 1);
				DrawIcon(pDC->m_hDC, FirstCaliperPoint.x, FirstCaliperPoint.y-m_iToolBarOffset, m_HIconSelectedPad);
			}
		}

		// CrossBar
		if (m_bDispCrossBar)
		{
			set_color(m_lWindowID, "red");
			set_draw(m_lWindowID, "margin");

			disp_obj(m_HCrossBarXLD,m_lWindowID);
		}

		/////////////////////////////////////////////////////////

		if (m_iPolygonMode == 1)
		{
			for (int iii=0; iii<m_iPolygonCnt; iii++)
			{
				THEAPP.m_pGFunction->DisplayCross(pDC->m_hDC, &m_ViewportManager, m_PolygonPt[iii].x, m_PolygonPt[iii].y, RGB(0,255,0), 25);
			}
		}
		else if (m_iPolygonMode == 2)
		{
			set_color(m_lWindowID, "magenta");
			set_draw(m_lWindowID, "margin");
			disp_obj(mHUnionPolygonRgn,m_lWindowID);
		}

		if(m_bDisplayFitLine)
		{
			if(m_bLive==FALSE)
			{
				if (m_bPolygonMode)
				{
					for (int iii=0; iii<m_iPolygonCnt; iii++)
					{
						THEAPP.m_pGFunction->DisplayCross(m_lWindowID, m_PolygonPt[iii].x, m_PolygonPt[iii].y, RGB(0,255,0), 25);
					}
				}
			}
		}

		// AutoFocus Area Display

		if (THEAPP.m_pTabControlDlg->m_iCurrentTab == 0)	// Align Tab
		{
			if (THEAPP.m_pGFunction->ValidHRegion(m_HAutoFocusCheckRgn))
			{
				set_color(m_lWindowID, "magenta");
				set_draw(m_lWindowID, "margin");
				disp_obj(m_HAutoFocusCheckRgn,m_lWindowID);
			}
		}

		if (THEAPP.m_pGFunction->ValidHRegion(m_HShapeModelContrastRgn))
		{
			set_color(m_lWindowID, "red");
			set_draw(m_lWindowID, "fill");
			disp_obj(m_HShapeModelContrastRgn,m_lWindowID);
		}

	}


	set_window_dc ( m_lWindowID, 0 );
	m_bOnPaintNow = FALSE;
	ReleaseDC(pDC);


	// Do not call CDialog::OnPaint() for painting messages
}

void CInspectAdminViewDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
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

void CInspectAdminViewDlg::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
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

void CInspectAdminViewDlg::OnLButtonDown(UINT nFlags, CPoint point) 
{
	if (!m_bDisplayImage){
		CDialog::OnLButtonDown(nFlags, point);
		return;
	}

	//POINT VPoint = point;
	//POINT IPoint = point;

	//VPoint.y -= m_iToolBarOffset;	// Toolbar offset
	//IPoint.y -= m_iToolBarOffset;	// Toolbar offset

	//m_ViewportManager.VPtoIP(&IPoint, 1);

	//maMVPoints[0] = VPoint;
	//maMIPoints[0] = IPoint;

	point.y -= m_iToolBarOffset;	// Toolbar offset

	POINT IPoint = point;

	m_ViewportManager.VPtoIP(&IPoint, 1);

	maMVPoints[0] = point;
	maMIPoints[0] = IPoint;


	SetCapture();

	if (m_ToolBarState==TS_MOVE){

		m_bOnMoving = TRUE;

		PostMessage(WM_SETCURSOR);

		CDialog::OnLButtonDown(nFlags, point);
		return;
	}

	if (m_ToolBarState==TS_SELECT_PART || m_ToolBarState==TS_CREATE_AF_ROI)
	{
		mpSelectPartTRegion = new CSelectPartRegion;

		if (mpSelectPartTRegion)
		{
			mpSelectPartTRegion->SetLTPoint(IPoint, THEAPP.m_pCalDataService);
			mpSelectPartTRegion->SetRBPoint(IPoint, THEAPP.m_pCalDataService);

			mpSelectPartTRegion->SetSelect(TRUE, FALSE);
			DrawSelectPartTRegion();
		}

		CDialog::OnLButtonDown(nFlags, point);
		return;
	}

	if (m_ToolBarState==NC_TS_EDIT_XLD || m_ToolBarState==NC_TS_EDIT_POLYGON_XLD)
	{
		if(THEAPP.m_iModeSwitch!=MODE_ADMIN_TEACH_VIEW)
		{
			OnTbMove();
			return;
		}

		mpSelectPartTRegion = new CSelectPartRegion;
		if (mpSelectPartTRegion) 
		{
			mpSelectPartTRegion->SetLTPoint(IPoint, THEAPP.m_pCalDataService);
			mpSelectPartTRegion->SetRBPoint(IPoint, THEAPP.m_pCalDataService);

			mpSelectPartTRegion->SetSelect(TRUE, FALSE);
			DrawSelectPartTRegion();
		}

		CDialog::OnLButtonDown(nFlags, point);
		return;
	}

	if(THEAPP.m_iModeSwitch!=MODE_ADMIN_TEACH_VIEW)
	{
		CDialog::OnLButtonDown(nFlags, point);
		return;
	}

	ClearAllSelectRegion();

	if (m_ToolBarState==TS_DRAW)	// Cross-Bar
	{
		if (m_bDispCrossBar)
		{
			CDialog::OnLButtonDown(nFlags, point);
			return;
		}
	}

	switch (m_ToolBarState) 
	{
	case TS_DRAW:
		{
			if (m_caliper_control_flag == 1)
			{
				if (m_iNoCaliperPoint==0 || m_iNoCaliperPoint==2)
				{
					mpLineEP = mpLineSP = IPoint;
					m_iNoCaliperPoint = 1;
				}
				else 
				{
					mpLineEP = IPoint;
					m_iNoCaliperPoint = 2;
				}

				InvalidateRect(&m_ClientRect, FALSE);

				CDialog::OnLButtonDown(nFlags, point);
				return;
			}

			mpActiveTRegion = NULL;

			if (!mpActiveTRegion && THEAPP.m_pModelDataManager->m_pInspectionArea) 
			{
				mpActiveTRegion = THEAPP.m_pModelDataManager->m_pInspectionArea->GetTopTRegion(point, &m_ViewportManager, THEAPP.m_pCalDataService, THEAPP.m_pTabControlDlg->m_iCurrentTab);
			}

			if (!mpActiveTRegion)
			{
				m_pLastSelectedRegion = NULL;
				return;
			}

			if (mpActiveTRegion->m_bRegionROI)
			{
				HTuple IsInside;
				test_region_point (mpActiveTRegion->m_HTeachPolygonRgn, (HTuple)IPoint.y, (HTuple)IPoint.x, &IsInside);

				if (IsInside==1)
					miActiveCPointIndex = GTR_CP_REGION;
				else
					miActiveCPointIndex = GTR_CP_NONE;
			}
			else
			{
				miActiveCPointIndex = mpActiveTRegion->GetCPointIndex(point, &m_ViewportManager, THEAPP.m_pCalDataService);
			}

			if (miActiveCPointIndex != GTR_CP_NONE)
			{
				ClearAllLastSelectRegion();
				mpActiveTRegion->SetSelect(TRUE, FALSE);

				// 24.02.28 Local Align 추가 - LeeGW Start
				if (mpActiveTRegion!=NULL)
				{
					delegateCurROITabParamSave[THEAPP.m_pTabControlDlg->m_iCurrentTab-1].FireEvent((WPARAM)THEAPP.m_pTabControlDlg->m_iCurrentTab-1);

					ResetInspectionResult();

					m_pLastSelectedRegion = mpActiveTRegion;
					
					delegateROISelected[THEAPP.m_pTabControlDlg->m_iCurrentTab-1].FireEvent((WPARAM)THEAPP.m_pTabControlDlg->m_iCurrentTab-1);
				}
				// 24.02.28 Local Align 추가 - LeeGW End

				DrawActiveTRegion();
			}
			else
				mpActiveTRegion = NULL;

			break;
		}

	case TS_CREATE_ROI_RECT:
	case TS_CREATE_ROI_CIRCLE:
		{
			mpActiveTRegion = new GTRegion;

			if (mpActiveTRegion)
			{
				mpActiveTRegion->SetLTPoint(IPoint, THEAPP.m_pCalDataService);
				mpActiveTRegion->SetRBPoint(IPoint, THEAPP.m_pCalDataService);

				mpActiveTRegion->SetSelect(TRUE, FALSE);
				DrawActiveTRegion();
			}

			break;
		}

	case TS_CREATE_ROI_POLYGON:
		{
			mpActiveTRegion = new GTRegion;

			if (mpActiveTRegion)
			{
				mpActiveTRegion->SetLTPoint(IPoint, THEAPP.m_pCalDataService);
				mpActiveTRegion->SetRBPoint(IPoint, THEAPP.m_pCalDataService);

				mpActiveTRegion->SetSelect(TRUE, FALSE);
				DrawActiveTRegion();
			}

			break;
		}

	}

	CDialog::OnLButtonDown(nFlags, point);
}

void CInspectAdminViewDlg::OnLButtonUp(UINT nFlags, CPoint point) 
{
	if (!m_bDisplayImage) 
		return;

	ReleaseCapture();

	DPOINT dLTPoint, dRBPoint;
	POINT WHPoint;
	POINT LTPoint, RBPoint;

	CInspectionTypeDlg InspectionTypeDlg;

	if (m_ToolBarState==TS_MOVE || m_ToolBarState==TS_SELECT_PART || m_ToolBarState==TS_CREATE_AF_ROI || m_ToolBarState==NC_TS_EDIT_XLD || m_ToolBarState==NC_TS_EDIT_POLYGON_XLD)
	{
		switch (m_ToolBarState) 
		{
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

		case TS_CREATE_AF_ROI:
			if (mpSelectPartTRegion) 
			{
				mpSelectPartTRegion->GetWHPoint(&WHPoint, THEAPP.m_pCalDataService);

				if (WHPoint.x < 5 || WHPoint.y < 5) {
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

				gen_rectangle1(&m_HAutoFocusCheckRgn, dLTPoint.y, dLTPoint.x, dRBPoint.y, dRBPoint.x);

				m_ToolBarState = m_PrevToolBarState;
			}
			break;

		case NC_TS_EDIT_XLD:
			if(THEAPP.m_iModeSwitch!=MODE_ADMIN_TEACH_VIEW)
			{
				OnTbMove();
				return;
			}

			if (mpSelectPartTRegion) {
				mpSelectPartTRegion->GetWHPoint(&WHPoint, THEAPP.m_pCalDataService);

				if (WHPoint.x < 5 || WHPoint.y < 5) {
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

				EditXldData(dLTPoint.y, dLTPoint.x, dRBPoint.y, dRBPoint.x);

				OnTbMove();
			}
			break;

		case NC_TS_EDIT_POLYGON_XLD:

			if(THEAPP.m_iModeSwitch!=MODE_ADMIN_TEACH_VIEW)
			{
				OnTbMove();
				return;
			}

			if (mpSelectPartTRegion)
			{
				mpSelectPartTRegion->ArrangePoints();

				mpSelectPartTRegion->GetLTPoint(&LTPoint, THEAPP.m_pCalDataService);
				mpSelectPartTRegion->GetRBPoint(&RBPoint, THEAPP.m_pCalDataService);

				delete mpSelectPartTRegion;
				mpSelectPartTRegion = NULL;

				m_PolygonPt[m_iPolygonCnt].x = LTPoint.x;
				m_PolygonPt[m_iPolygonCnt].y = LTPoint.y;
				++m_iPolygonCnt;

			}
			break;

		}
	}
	else
	{
		if(THEAPP.m_iModeSwitch!=MODE_ADMIN_TEACH_VIEW)
		{
			mpActiveTRegion = NULL;
			m_pLastSelectedRegion = NULL;	// 24.02.28 Local Align 추가 - LeeGW
			return;
		}

		POINT LTPoint, RBPoint;

		Hobject HROIHRegion;
		BOOL bDontCareExist, bAddExist;
		int iLastID;

		GTRegion *pInspectROI;

		if (mpActiveTRegion)
		{
			switch (m_ToolBarState) 
			{
			case TS_DRAW:
				{
					mpActiveTRegion->ArrangePoints();

					if (mpActiveTRegion->GetShape()==TRUE)	// Circle
					{
						mpActiveTRegion->GetLTPoint(&LTPoint, THEAPP.m_pCalDataService);
						mpActiveTRegion->GetRBPoint(&RBPoint, THEAPP.m_pCalDataService);

						if ((RBPoint.x - LTPoint.x) >= (RBPoint.y - LTPoint.y)){
							RBPoint.x = LTPoint.x + (RBPoint.x - LTPoint.x);
							RBPoint.y = LTPoint.y + (RBPoint.x - LTPoint.x);
						}
						else {
							RBPoint.x = LTPoint.x + (RBPoint.y - LTPoint.y);
							RBPoint.y = LTPoint.y + (RBPoint.y - LTPoint.y);
						}

						mpActiveTRegion->SetLTPoint(LTPoint, THEAPP.m_pCalDataService);
						mpActiveTRegion->SetRBPoint(RBPoint, THEAPP.m_pCalDataService);
					}

					break;
				}

			case TS_CREATE_ROI_RECT:
			case TS_CREATE_ROI_CIRCLE:

				mpActiveTRegion->GetWHPoint(&WHPoint, THEAPP.m_pCalDataService);

				if (WHPoint.x < 5 || WHPoint.y < 5) 
				{
					SAFE_DELETE(mpActiveTRegion);
					break;
				}

				mpActiveTRegion->ArrangePoints();

				mpActiveTRegion->GetLTPoint(&LTPoint, THEAPP.m_pCalDataService);
				mpActiveTRegion->GetRBPoint(&RBPoint, THEAPP.m_pCalDataService);

				SAFE_DELETE(mpActiveTRegion);


				if (THEAPP.m_pTabControlDlg->m_iCurrentTab <= TRIGGER_SURFACE)
					break;

				if (InspectionTypeDlg.DoModal() == IDCANCEL)
					break;

				pInspectROI = new GTRegion;

				if (m_ToolBarState==TS_CREATE_ROI_RECT)
					pInspectROI->SetShape(FALSE,FALSE);
				else if (m_ToolBarState==TS_CREATE_ROI_CIRCLE)
				{
					if ((RBPoint.x - LTPoint.x) >= (RBPoint.y - LTPoint.y)){
						RBPoint.x = LTPoint.x + (RBPoint.x - LTPoint.x);
						RBPoint.y = LTPoint.y + (RBPoint.x - LTPoint.x);
					}
					else {
						RBPoint.x = LTPoint.x + (RBPoint.y - LTPoint.y);
						RBPoint.y = LTPoint.y + (RBPoint.y - LTPoint.y);
					}

					pInspectROI->SetShape(TRUE,FALSE);
				}

				pInspectROI->SetLTPoint(LTPoint, THEAPP.m_pCalDataService);
				pInspectROI->SetRBPoint(RBPoint, THEAPP.m_pCalDataService);

				pInspectROI->m_iTeachImageIndex = THEAPP.m_pTabControlDlg->m_iCurrentTab;
				pInspectROI->m_iInspectionType = InspectionTypeDlg.m_iRadioInspectionType;

				pInspectROI->SetSelectable(TRUE,FALSE);
				pInspectROI->SetMovable(TRUE,FALSE);
				pInspectROI->SetSizable(TRUE,FALSE);		
				
				if (pInspectROI->m_iInspectionType == INSPECTION_TYPE_ALIGN)
					pInspectROI->SetLineColor(RGB(0, 255, 255));
				else if (pInspectROI->m_iInspectionType == INSPECTION_TYPE_DELETE)
					pInspectROI->SetLineColor(RGB(0, 0, 255));
				else if (pInspectROI->m_iInspectionType == INSPECTION_TYPE_ADD)
					pInspectROI->SetLineColor(RGB(255, 128, 0));
				else if (pInspectROI->m_iInspectionType == INSPECTION_TYPE_LOCAL_ALIGN)	// 24.02.29 Local Align 추가 - LeeGW
					pInspectROI->SetLineColor(RGB(255, 0, 255));
				else if (pInspectROI->m_iInspectionType == INSPECTION_TYPE_AI_INSPECTION)	// 24.10.30 AI 검사 - LeeGW
					pInspectROI->SetLineColor(RGB(255, 255, 0));
				else if ((pInspectROI->m_iInspectionType >= INSPECTION_TYPE_FAI_START && pInspectROI->m_iInspectionType <= INSPECTION_TYPE_FAI_END))
					pInspectROI->SetLineColor(RGB(255, 165, 0));
				else
					pInspectROI->SetLineColor(RGB(0,255,0));

				pInspectROI->miLineStyle = PS_SOLID;

				if ( (pInspectROI->m_iInspectionType == INSPECTION_TYPE_DELETE || pInspectROI->m_iInspectionType == INSPECTION_TYPE_ADD)==FALSE)
				{
					HROIHRegion = pInspectROI->GetROIHRegion(THEAPP.m_pCalDataService);

					bDontCareExist = FALSE;
					bDontCareExist = CheckDontCareInclusion(HROIHRegion);

					if (bDontCareExist)
					{
						int nRes = 0;
						nRes = AfxMessageBox("새로운 ROI 내에 삭제 ROI가 존재합니다. 비검사 ROI 영역을 제외하시겠습니까?", MB_YESNO|MB_SYSTEMMODAL);

						if ((nRes==IDYES))
						{
							GetRegionROIAfterDontCareRemoval(pInspectROI);
						}
					}

					bAddExist = FALSE;
					bAddExist = CheckAddInclusion(HROIHRegion);

					if (bAddExist)
					{
						int nRes = 0;
						nRes = AfxMessageBox("새로운 ROI 내에 추가 ROI가 존재합니다. ROI 영역을 추가하시겠습니까?", MB_YESNO|MB_SYSTEMMODAL);

						if ((nRes==IDYES))
						{
							GetRegionROIAfterAddRemoval(pInspectROI);
						}
					}
				}

				iLastID = THEAPP.m_pModelDataManager->GetLastPadID();
				pInspectROI->miPadID = iLastID + 1;

				// 24.02.29 Local Align 추가 - LeeGW
				if (pInspectROI->m_iInspectionType == INSPECTION_TYPE_LOCAL_ALIGN)
				{
					int iLastLocalAlignID = 0;
					iLastLocalAlignID = THEAPP.m_pModelDataManager->GetLastLocalAlignID(THEAPP.m_pTabControlDlg->m_iCurrentTab);
					pInspectROI->m_iLocalAlignID = iLastLocalAlignID + 1;
				}
				// FAI 추가 - LeeGW
				else if (pInspectROI->m_iInspectionType ==  INSPECTION_TYPE_FAI_OUTER_CIRCLE)
				{
					int iFAIOuterCircleID = 0;
					iFAIOuterCircleID = THEAPP.m_pModelDataManager->GetLastFAIOuterCircleID(THEAPP.m_pTabControlDlg->m_iCurrentTab);
					pInspectROI->m_iFAIOuterCircleID = iFAIOuterCircleID + 1;
				}
				else if (pInspectROI->m_iInspectionType == INSPECTION_TYPE_FAI_INNER_CIRCLE)
				{
					int iFAIInnerCircleID = 0;
					iFAIInnerCircleID = THEAPP.m_pModelDataManager->GetLastFAIInnerCircleID(THEAPP.m_pTabControlDlg->m_iCurrentTab);
					pInspectROI->m_iFAIInnerCircleID = iFAIInnerCircleID + 1;
				}
				else if (pInspectROI->m_iInspectionType == INSPECTION_TYPE_FAI_DATUM_CIRCLE)
				{
					int iFAIDatumCircleID = 0;
					iFAIDatumCircleID = THEAPP.m_pModelDataManager->GetLastFAIDatumCircleID(THEAPP.m_pTabControlDlg->m_iCurrentTab);
					pInspectROI->m_iFAIDatumCircleID = iFAIDatumCircleID + 1;
				}
				else if (pInspectROI->m_iInspectionType == INSPECTION_TYPE_FAI_DATUM_ELLIPSE)
				{
					int iFAIDatumEllipseID = 0;
					iFAIDatumEllipseID = THEAPP.m_pModelDataManager->GetLastFAIDatumEllipseID(THEAPP.m_pTabControlDlg->m_iCurrentTab);
					pInspectROI->m_iFAIDatumEllipseID = iFAIDatumEllipseID + 1;
				}
				else if (pInspectROI->m_iInspectionType == INSPECTION_TYPE_FAI_WELDING_SPOT)
				{
					int iFAIWeldingSpotID = 0;
					iFAIWeldingSpotID = THEAPP.m_pModelDataManager->GetLastFAIWeldingSpotID(THEAPP.m_pTabControlDlg->m_iCurrentTab);
					pInspectROI->m_iFAIWeldingSpotID = iFAIWeldingSpotID + 1;
				}
				else if (pInspectROI->m_iInspectionType == INSPECTION_TYPE_FAI_WELDING_POCKET)
				{
					int iFAIWeldingPocketID = 0;
					iFAIWeldingPocketID = THEAPP.m_pModelDataManager->GetLastFAIWeldingSpotID(THEAPP.m_pTabControlDlg->m_iCurrentTab);
					pInspectROI->m_iFAIWeldingPocketID = iFAIWeldingPocketID + 1;
				}
				else if (pInspectROI->m_iInspectionType == INSPECTION_TYPE_FAI_BRACKET)
				{
					int iFAIBracketID = 0;
					iFAIBracketID = THEAPP.m_pModelDataManager->GetLastFAIBracketID(THEAPP.m_pTabControlDlg->m_iCurrentTab);
					pInspectROI->m_iFAIBracketID = iFAIBracketID + 1;
				}

				THEAPP.m_pModelDataManager->m_pInspectionArea->AddChildTRegion(pInspectROI);

				SetToolBarStateDraw();

				break;

			case TS_CREATE_ROI_POLYGON:
				mpActiveTRegion->ArrangePoints();
				mpActiveTRegion->GetLTPoint(&LTPoint, THEAPP.m_pCalDataService);

				SAFE_DELETE(mpActiveTRegion);

				m_PolygonPt[m_iPolygonCnt].x = LTPoint.x;
				m_PolygonPt[m_iPolygonCnt].y = LTPoint.y;
				++m_iPolygonCnt;

				break;
			}
		}	// if (mpActiveTRegion)
		else	// Cross Bar Position
		{
			if (m_bDispCrossBar)
			{
				if (m_ToolBarState == TS_DRAW)
				{
					point.y -= m_iToolBarOffset;	// Toolbar offset
					POINT IPoint = point;
					m_ViewportManager.VPtoIP(&IPoint, 1);
					gen_empty_obj(&m_HCrossBarXLD);
					gen_cross_contour_xld(&m_HCrossBarXLD, (double)IPoint.y, (double)IPoint.x, (double)2560*2.0, 0.0);
				}
			}
		}

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

void CInspectAdminViewDlg::OnMouseMove(UINT nFlags, CPoint point) 
{
	if (!m_bDisplayImage) 
		return;

	int GrayValue=0;
	COLORREF Color;
	CDC* pDC = GetDC();
	Color = pDC->GetPixel(point.x, point.y);
	GrayValue = (int)((GetRValue(Color) + GetGValue(Color) + GetBValue(Color))/3.0);
	ReleaseDC(pDC);

	point.y -= m_iToolBarOffset;	// Toolbar offset

	POINT VPoint = point;
	POINT IPoint = point;


	m_ViewportManager.VPtoIP(&IPoint, 1);

	maMVPoints[1] = VPoint;
	maMIPoints[1] = IPoint;

	CString OutTxt;
	OutTxt.Format("Strip View [Top Camera] - (%d, %d) = %03d, Zoom(%d%%)", (int)((point.x/m_dZoomRatio)+m_ViewRect.left), (int)((point.y/m_dZoomRatio)+m_ViewRect.top), GrayValue, (int)(m_dZoomRatio*100.));
	SetWindowText((LPCTSTR)OutTxt);

	int iWidth, iHeight;
	long HCurPos, VCurPos;
	CPoint MVDiffPoint, MIDiffPoint;

	if (IS_SETFLAG(nFlags, MK_LBUTTON)) 
	{
		switch (m_ToolBarState) 
		{
		case TS_DRAW:
			{
				if (THEAPP.m_iModeSwitch!=MODE_ADMIN_TEACH_VIEW)
					return;

				if (!mpActiveTRegion)
					return;

				if (mpActiveTRegion->m_bRegionROI)
				{
					if(THEAPP.m_pGFunction->ValidHRegion(mpActiveTRegion->m_HTeachPolygonRgn))
					{
						POINT MLTPoint_B, MLTPoint_A;

						DrawActiveTRegion();

						mpActiveTRegion->GetLTPoint(&MLTPoint_B, THEAPP.m_pCalDataService);
						mpActiveTRegion->SetCPoint(miActiveCPointIndex, maMIPoints, THEAPP.m_pCalDataService);
						mpActiveTRegion->GetLTPoint(&MLTPoint_A, THEAPP.m_pCalDataService);

						MIDiffPoint.x = MLTPoint_A.x - MLTPoint_B.x;
						MIDiffPoint.y = MLTPoint_A.y - MLTPoint_B.y;

						move_region(mpActiveTRegion->m_HTeachPolygonRgn, &(mpActiveTRegion->m_HTeachPolygonRgn), MIDiffPoint.y, MIDiffPoint.x);

						DrawActiveTRegion();
					}
				}
				else
				{
					DrawActiveTRegion();
					mpActiveTRegion->SetCPoint(miActiveCPointIndex, maMIPoints, THEAPP.m_pCalDataService);
					DrawActiveTRegion();
				}

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
		case TS_CREATE_AF_ROI:
			if (!mpSelectPartTRegion) return;

			DrawSelectPartTRegion();

			mpSelectPartTRegion->SetRBPoint(IPoint, THEAPP.m_pCalDataService);

			DrawSelectPartTRegion();

			break;

		case NC_TS_EDIT_XLD:

			if(THEAPP.m_iModeSwitch!=MODE_ADMIN_TEACH_VIEW)
			{
				OnTbMove();
				return;
			}

			if (!mpSelectPartTRegion) return;

			DrawSelectPartTRegion();

			mpSelectPartTRegion->SetRBPoint(IPoint, THEAPP.m_pCalDataService);

			DrawSelectPartTRegion();

			break;

		case TS_CREATE_ROI_RECT:
		case TS_CREATE_ROI_CIRCLE:

			if(THEAPP.m_iModeSwitch==MODE_ADMIN_TEACH_VIEW)
			{
				if (!mpActiveTRegion)
					return;

				DrawActiveTRegion();

				mpActiveTRegion->SetRBPoint(IPoint, THEAPP.m_pCalDataService);

				DrawActiveTRegion();
			}

			break;


		default:
			break;
		}
	}

	maMVPoints[0] = maMVPoints[1];
	maMIPoints[0] = maMIPoints[1];

	CDialog::OnMouseMove(nFlags, point);
}

void CInspectAdminViewDlg::OnDestroy() 
{
	CDialog::OnDestroy();

	SAFE_DELETE(mpActiveTRegion);
	
	if(m_lWindowID > 0)
	{
		close_window(m_lWindowID);
		m_lWindowID = -1;
	}

	m_Menu.DestroyMenu();
}

void CInspectAdminViewDlg::OnSize(UINT nType, int cx, int cy) 
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

BOOL CInspectAdminViewDlg::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt) 
{
	if(THEAPP.m_iModeSwitch!=MODE_ADMIN_TEACH_VIEW)
	{
		CDialog::OnMouseWheel(nFlags, zDelta, pt);
	}

	ScreenToClient(&pt);

	if (!m_ClientRect.PtInRect(pt))
	{
		return CDialog::OnMouseWheel(nFlags, zDelta, pt);
	}

	if (zDelta <= 0) 
	{                            
		OnTbZoomIn();
	} 
	else
	{
		OnTbZoomOut();
	}

	return CDialog::OnMouseWheel(nFlags, zDelta, pt);
}

BOOL CInspectAdminViewDlg::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message) 
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

void CInspectAdminViewDlg::Reset()
{

	//	m_pFMDataContainer = THEAPP.m_pModelDataManager->GetFMDataContainer(1);

	InitViewRect();
	UpdateViewportManager();

	m_iInspectingFrameIndex = -1;
	m_iDefectIdx = -1;
}

void CInspectAdminViewDlg::InitViewRect() 
{
	m_ViewRect.left = 0;
	m_ViewRect.top = 0;
	m_ViewRect.bottom = (int)(RECTHEIGHT(&m_ClientRect) / m_dZoomRatio);
	m_ViewRect.right = (int)(RECTWIDTH(&m_ClientRect) / m_dZoomRatio);
}

void CInspectAdminViewDlg::UpdateViewportManager()
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

void CInspectAdminViewDlg::ViewRectSet() 
{
	m_ViewRect.bottom = m_ViewRect.top + (int)(RECTHEIGHT(&m_ClientRect) / m_dZoomRatio);
	m_ViewRect.right = m_ViewRect.left + (int)(RECTWIDTH(&m_ClientRect) / m_dZoomRatio);
}

void CInspectAdminViewDlg::ScrollBarSet()
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

void CInspectAdminViewDlg::ScrollBarPosSet()
{
	m_VScrBar.SetWindowPos(&wndTop, m_ClientRect.right, m_ClientRect.top, m_iScrBarWidth, m_ClientRect.bottom-m_iToolBarOffset, SWP_SHOWWINDOW);
	m_VScrBar.ShowScrollBar(TRUE);
	m_HScrBar.SetWindowPos(&wndTop, m_ClientRect.left, m_ClientRect.bottom, m_ClientRect.right, m_iScrBarWidth, SWP_SHOWWINDOW);
	m_HScrBar.ShowScrollBar(TRUE);
}

void CInspectAdminViewDlg::DrawActiveTRegion()
{
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
		mpActiveTRegion->Draw(pDC->m_hDC, &m_ViewportManager, GTR_DS_ACTIVE, FALSE, THEAPP.m_pCalDataService, THEAPP.m_pTabControlDlg->m_iCurrentTab);
	}
	ReleaseDC(pDC);

}

void CInspectAdminViewDlg::DrawSelectPartTRegion()
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
		mpSelectPartTRegion->Draw(pDC->m_hDC, &m_ViewportManager, GTR_DS_SELECTPART, FALSE, THEAPP.m_pCalDataService, THEAPP.m_pTabControlDlg->m_iCurrentTab);
	}
	ReleaseDC(pDC);
}

void CInspectAdminViewDlg::ZoomInSelectedPart(double dZoomRatio, int iStartPosX, int iStartPosY) 
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

void CInspectAdminViewDlg::OnTbDraw() 
{
	m_ToolBarState = TS_DRAW;
}

void CInspectAdminViewDlg::OnTbMove() 
{
	m_ToolBarState = TS_MOVE;
}

void CInspectAdminViewDlg::OnTbSelectPart() 
{
	m_ToolBarState = TS_SELECT_PART;
}

void CInspectAdminViewDlg::OnTbROIRect() 
{
	m_ToolBarState = TS_CREATE_ROI_RECT;

	if (m_caliper_control_flag==1)
	{
		m_caliper_control_flag=0;
		m_iNoCaliperPoint = 0;
		mpLineSP = CPoint(0,0);
		mpLineEP = CPoint(0,0);
		InvalidateRect(&m_ClientRect, FALSE);
	}
}

void CInspectAdminViewDlg::OnTbROICircle() 
{
	m_ToolBarState = TS_CREATE_ROI_CIRCLE;

	if (m_caliper_control_flag==1)
	{
		m_caliper_control_flag=0;
		m_iNoCaliperPoint = 0;
		mpLineSP = CPoint(0,0);
		mpLineEP = CPoint(0,0);
		InvalidateRect(&m_ClientRect, FALSE);
	}
}

void CInspectAdminViewDlg::OnTbROIPolygon() 
{
	m_ToolBarState = TS_CREATE_ROI_POLYGON;

	m_bPolygonMode = TRUE;
	m_iPolygonCnt = 0;

	if (m_caliper_control_flag==1)
	{
		m_caliper_control_flag=0;
		m_iNoCaliperPoint = 0;
		mpLineSP = CPoint(0,0);
		mpLineEP = CPoint(0,0);
		InvalidateRect(&m_ClientRect, FALSE);
	}
}

void CInspectAdminViewDlg::OnTbROIDelete() 
{
	int nRes = 0;

	if (mpActiveTRegion)
	{
		if (THEAPP.m_pModelDataManager->m_pInspectionArea!=NULL)
		{
			if (THEAPP.m_pModelDataManager->m_pInspectionArea->GetChildTRegionCount()>0)
			{
				nRes = AfxMessageBox("ROI를 삭제하시겠습니까?", MB_YESNO|MB_SYSTEMMODAL);

				if ((nRes==IDYES))
				{
					THEAPP.m_pModelDataManager->m_pInspectionArea->DeleteChildTRegionPtr(mpActiveTRegion, FALSE);
				}
			}
		}

		InvalidateRect(&m_ClientRect, FALSE);
		mpActiveTRegion = NULL;
	}
}

void CInspectAdminViewDlg::SetXLDEditMode(int iContourType) 
{
	m_ToolBarState = NC_TS_EDIT_XLD;
	m_iTeachingContourType = iContourType;
}

void CInspectAdminViewDlg::SetPolygonXLDEditMode(int iContourType) 
{
	m_ToolBarState = NC_TS_EDIT_POLYGON_XLD;

	m_iPolygonMode = 1;
	m_iPolygonCnt = 0;

	m_iTeachingContourType = iContourType;
}

void CInspectAdminViewDlg::OnTbZoomIn() 
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

void CInspectAdminViewDlg::OnTbZoomOut() 
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

void CInspectAdminViewDlg::OnTbFitWidth() 
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

void CInspectAdminViewDlg::OnTbFitHeight() 
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

BOOL CInspectAdminViewDlg::ShowWindow(int nCmdShow)
{
	return CDialog::ShowWindow(nCmdShow);
}

void CInspectAdminViewDlg::InitializeToolBar()
{
	CToolBarCtrl &toolbarctrl = m_ViewToolbar.GetToolBarCtrl();
	toolbarctrl.CheckButton(ID_TB_SELECT_PART, TRUE);
	m_ToolBarState = TS_SELECT_PART;
}

int CInspectAdminViewDlg::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CDialog::OnCreate(lpCreateStruct) == -1) return -1;

	HTuple tWnd = (INT)(m_hWnd) ;
	new_extern_window(tWnd, 0, 0, m_iImageWidth, m_iImageHeight, &m_lWindowID) ;

	return 0;
}

LRESULT CInspectAdminViewDlg::On2DFrameReady(WPARAM wParam, LPARAM lParam)
{

	return 0;
}

LRESULT CInspectAdminViewDlg::OnEventGoToDefect(WPARAM wParam, LPARAM lParam)	
{


	InvalidateRect(&m_ClientRect, TRUE);
	return 0;
}

void CInspectAdminViewDlg::LoadScanImage(int nScanNum) 
{

}

void CInspectAdminViewDlg::MoveToDefectArea()
{

}

void CInspectAdminViewDlg::RedrawView()
{
	InvalidateRect(NULL, FALSE);
}

void CInspectAdminViewDlg::RedrawRect()
{
	InvalidateRect(&m_ClientRect, FALSE);
}

BOOL CInspectAdminViewDlg::SelectAllPackage()
{
	return FALSE;
}

BOOL CInspectAdminViewDlg::SelectPackageChange() 
{
	return FALSE;
}

BOOL CInspectAdminViewDlg::SelectPackageUndo() 
{
	return FALSE;
}

void CInspectAdminViewDlg::UnSelectAll()
{

}
void CInspectAdminViewDlg::UpdateThresRegion(int Min,int Max)
{
	switch(THEAPP.m_pTabControlDlg->m_iCurrentTab)
	{
	case 0:
		{
			if(THEAPP.m_pGFunction->ValidHImage(m_pHImage))
			{threshold(m_pHImage,&m_pThresRgn,Min,Max);}
			break;
		}
	case 1:
		{
			if(THEAPP.m_pGFunction->ValidHImage(m_HTeachingImage[THEAPP.m_pTabControlDlg->m_iCurrentTab-1]))
			{threshold(m_HTeachingImage[THEAPP.m_pTabControlDlg->m_iCurrentTab-1],&m_pThresRgn,Min,Max);}
			break;
		}
	case 2:
		{
			if(THEAPP.m_pGFunction->ValidHImage(m_HTeachingImage[THEAPP.m_pTabControlDlg->m_iCurrentTab-1]))
			{threshold(m_HTeachingImage[THEAPP.m_pTabControlDlg->m_iCurrentTab-1],&m_pThresRgn,Min,Max);}
			break;
		}
	case 3:
		{
			if(THEAPP.m_pGFunction->ValidHImage(m_HTeachingImage[THEAPP.m_pTabControlDlg->m_iCurrentTab-1]))
			{threshold(m_HTeachingImage[THEAPP.m_pTabControlDlg->m_iCurrentTab-1],&m_pThresRgn,Min,Max);}
			break;
		}
	case 4:
		{
			if(THEAPP.m_pGFunction->ValidHImage(m_HTeachingImage[THEAPP.m_pTabControlDlg->m_iCurrentTab-1]))
				threshold(m_HTeachingImage[THEAPP.m_pTabControlDlg->m_iCurrentTab-1],&m_pThresRgn,Min,Max);
			break;
		}
	case 5:
		{
			if(THEAPP.m_pGFunction->ValidHImage(m_HTeachingImage[THEAPP.m_pTabControlDlg->m_iCurrentTab-1]))
				threshold(m_HTeachingImage[THEAPP.m_pTabControlDlg->m_iCurrentTab-1],&m_pThresRgn,Min,Max);
			break;
		}
	case 6:
		{
			if(THEAPP.m_pGFunction->ValidHImage(m_HTeachingImage[THEAPP.m_pTabControlDlg->m_iCurrentTab-1]))
				threshold(m_HTeachingImage[THEAPP.m_pTabControlDlg->m_iCurrentTab-1],&m_pThresRgn,Min,Max);
			break;
		}
	}
	InvalidateRect(false);
}

void CInspectAdminViewDlg::OnTbLive()
{
#ifdef INLINE_MODE

	CToolBarCtrl &toolbarctrl = m_ViewToolbar.GetToolBarCtrl();

	if(m_bLive==FALSE)
	{
		toolbarctrl.CheckButton(ID_TB_LIVE,TRUE);
		m_bLive = TRUE;
		SetTimer(0, 100,NULL);
	}
	else
	{
		toolbarctrl.CheckButton(ID_TB_LIVE,FALSE);
		m_bLive = FALSE;
		KillTimer(0);
	}
#endif
}

void CInspectAdminViewDlg::OnTbFitLineDisplay()
{
	if (m_bDisplayFitLine)
		m_bDisplayFitLine = FALSE;
	else
		m_bDisplayFitLine = TRUE;

	InvalidateRect(&m_ClientRect, TRUE);
}

void CInspectAdminViewDlg::OnTbInspectAreaDisplay()
{
	if (m_bDisplayInspectArea)
		m_bDisplayInspectArea = FALSE;
	else
		m_bDisplayInspectArea = TRUE;

	InvalidateRect(&m_ClientRect, TRUE);
}

void CInspectAdminViewDlg::OnTbThresholdAreaDisplay()
{
	if (m_bDisplayThresholdArea)
		m_bDisplayThresholdArea = FALSE;
	else
		m_bDisplayThresholdArea = TRUE;

	InvalidateRect(&m_ClientRect, TRUE);
}

void CInspectAdminViewDlg::OnTbDefectAreaDisplay()
{
	if (m_bDisplayDefectArea)
		m_bDisplayDefectArea = FALSE;
	else
		m_bDisplayDefectArea = TRUE;

	InvalidateRect(&m_ClientRect, TRUE);
}

void CInspectAdminViewDlg::OnTb2pCaliper() 
{	
	// TODO: Add your command handler code here
	CToolBarCtrl &toolbarctrl = m_ViewToolbar.GetToolBarCtrl();

	if (m_caliper_control_flag==0)
	{
		toolbarctrl.CheckButton(ID_TB_CALIPER,TRUE);
		toolbarctrl.CheckButton(ID_TB_DRAW,TRUE);
		m_ToolBarState = TS_DRAW;
		toolbarctrl.CheckButton(ID_TB_MOVE,FALSE);
		toolbarctrl.CheckButton(ID_TB_SELECT_PART,FALSE);
		toolbarctrl.CheckButton(ID_TB_ROI_RECT,FALSE);
		toolbarctrl.CheckButton(ID_TB_ROI_CIRCLE,FALSE);
		toolbarctrl.CheckButton(ID_TB_ROI_POLYGON,FALSE);

		m_caliper_control_flag=1;
	}
	else if (m_caliper_control_flag==1)
	{
		toolbarctrl.CheckButton(ID_TB_CALIPER,FALSE);

		m_caliper_control_flag=0;
		m_iNoCaliperPoint = 0;
		mpLineSP = CPoint(0,0);
		mpLineEP = CPoint(0,0);
		InvalidateRect(&m_ClientRect, FALSE);
	}
}

void CInspectAdminViewDlg::OnTbAverageGray() 
{	
	// TODO: Add your command handler code here
	CToolBarCtrl &toolbarctrl = m_ViewToolbar.GetToolBarCtrl();

	if (m_AverageGray_control_flag==0)
	{
		toolbarctrl.CheckButton(ID_TB_AVERAGE_GRAY,TRUE);
		m_AverageGray_control_flag=1;
	}
	else if (m_AverageGray_control_flag==1)
	{
		toolbarctrl.CheckButton(ID_TB_AVERAGE_GRAY,FALSE);
		m_AverageGray_control_flag=0;

		InvalidateRect(&m_ClientRect, FALSE);
	}
}

void CInspectAdminViewDlg::OnTbROILock() 
{	
	if (m_bROILock)
		m_bROILock = FALSE;
	else
		m_bROILock = TRUE;

	BOOL bROIState;
	bROIState = !m_bROILock;

	GTRegion *pRegion;

	for (int i=0; i<THEAPP.m_pModelDataManager->m_pInspectionArea->GetChildTRegionCount(); i++)
	{
		pRegion = THEAPP.m_pModelDataManager->m_pInspectionArea->GetChildTRegion(i);
		if (pRegion == NULL)
			continue;

		pRegion->SetMovable(bROIState,FALSE);
		pRegion->SetSizable(bROIState,FALSE);		
	}
}

void CInspectAdminViewDlg::OnTbDispCrossBar() 
{	
	// TODO: Add your command handler code here
	CToolBarCtrl &toolbarctrl = m_ViewToolbar.GetToolBarCtrl();

	if(m_bDispCrossBar==FALSE)
	{
		toolbarctrl.CheckButton(ID_TB_CROSS_BAR,TRUE);
		m_bDispCrossBar = TRUE;
	}
	else
	{
		toolbarctrl.CheckButton(ID_TB_CROSS_BAR,FALSE);
		m_bDispCrossBar = FALSE;
	}

	mpActiveTRegion = NULL;

	InvalidateRect(NULL, TRUE);
}

void CInspectAdminViewDlg::OnMenuRoiCopyImage1()
{
	m_iRoiCopyImageIndex = 1;
	CopyInspectionROI();
}

void CInspectAdminViewDlg::OnMenuRoiCopyImage2()
{
	m_iRoiCopyImageIndex = 2;
	CopyInspectionROI();
}

void CInspectAdminViewDlg::OnMenuRoiCopyImage3()
{
	m_iRoiCopyImageIndex = 3;
	CopyInspectionROI();
}

void CInspectAdminViewDlg::OnMenuRoiCopyImage4()
{
	m_iRoiCopyImageIndex = 4;
	CopyInspectionROI();
}

void CInspectAdminViewDlg::OnMenuRoiCopyImage5() 
{
	m_iRoiCopyImageIndex = 5;
	CopyInspectionROI();
}

void CInspectAdminViewDlg::OnMenuRoiCopyImage6() 
{
	m_iRoiCopyImageIndex = 6;
	CopyInspectionROI();
}

void CInspectAdminViewDlg::OnMenuRoiCopyImage7()
{
	m_iRoiCopyImageIndex = 7;
	CopyInspectionROI();
}


void CInspectAdminViewDlg::OnMenuROITypeChange()
{
	if (mpActiveTRegion == NULL)
	{
		AfxMessageBox("먼저, 변경할 ROI를 선택하여 주십시요.", MB_SYSTEMMODAL);
		return;
	}

	int iOrgInspectionType = mpActiveTRegion->m_iInspectionType;

	CInspectionTypeDlg InspectionTypeDlg;

	if (InspectionTypeDlg.DoModal() == IDCANCEL)
		return;

	mpActiveTRegion->m_iInspectionType = InspectionTypeDlg.m_iRadioInspectionType;
	if (mpActiveTRegion->m_iInspectionType == INSPECTION_TYPE_DELETE)
		mpActiveTRegion->SetLineColor(RGB(0, 0, 255));
	else if (mpActiveTRegion->m_iInspectionType == INSPECTION_TYPE_ALIGN)
		mpActiveTRegion->SetLineColor(RGB(0, 255, 255));
	else if (mpActiveTRegion->m_iInspectionType == INSPECTION_TYPE_ADD)
		mpActiveTRegion->SetLineColor(RGB(255, 128, 0));
	else if (mpActiveTRegion->m_iInspectionType == INSPECTION_TYPE_LOCAL_ALIGN)	// 24.02.29 Local Align 추가 - LeeGW
		mpActiveTRegion->SetLineColor(RGB(255, 0, 255));
	else if (mpActiveTRegion->m_iInspectionType == INSPECTION_TYPE_AI_INSPECTION)	// 24.10.30 AI 검사 - LeeGW
		mpActiveTRegion->SetLineColor(RGB(255, 255, 0));
	else if (mpActiveTRegion->m_iInspectionType == INSPECTION_TYPE_INSPECTION)
		mpActiveTRegion->SetLineColor(RGB(0, 255, 0));
	else if ((mpActiveTRegion->m_iInspectionType >= INSPECTION_TYPE_FAI_START && mpActiveTRegion->m_iInspectionType <= INSPECTION_TYPE_FAI_END))
		mpActiveTRegion->SetLineColor(RGB(255, 165, 0));

	BOOL bDontCareExist, bAddExist;
	Hobject HROIHRegion;

	if ( (mpActiveTRegion->m_iInspectionType == INSPECTION_TYPE_DELETE || mpActiveTRegion->m_iInspectionType == INSPECTION_TYPE_ADD)==FALSE)
	{
		HROIHRegion = mpActiveTRegion->GetROIHRegion(THEAPP.m_pCalDataService);

		bDontCareExist = FALSE;
		bDontCareExist = CheckDontCareInclusion(HROIHRegion);

		if (bDontCareExist)
		{
			int nRes = 0;
			nRes = AfxMessageBox("새로운 ROI 내에 삭제 ROI가 존재합니다. 삭제 ROI 영역을 제외하시겠습니까?", MB_YESNO|MB_SYSTEMMODAL);

			if ((nRes==IDYES))
			{
				GetRegionROIAfterDontCareRemoval(mpActiveTRegion);
			}
		}

		bAddExist = FALSE;
		bAddExist = CheckAddInclusion(HROIHRegion);

		if (bAddExist)
		{
			int nRes = 0;
			nRes = AfxMessageBox("새로운 ROI 내에 추가 ROI가 존재합니다. ROI 영역을 추가하시겠습니까?", MB_YESNO|MB_SYSTEMMODAL);

			if ((nRes==IDYES))
			{
				GetRegionROIAfterAddRemoval(mpActiveTRegion);
			}
		}
	}

	if (mpActiveTRegion->m_iInspectionType == INSPECTION_TYPE_LOCAL_ALIGN)
	{
		int iLastLocalAlignID = 0;
		iLastLocalAlignID = THEAPP.m_pModelDataManager->GetLastLocalAlignID(THEAPP.m_pTabControlDlg->m_iCurrentTab);
		mpActiveTRegion->m_iLocalAlignID = iLastLocalAlignID + 1;
	}
	// FAI 추가 - LeeGW
	else if (mpActiveTRegion->m_iInspectionType == INSPECTION_TYPE_FAI_OUTER_CIRCLE)
	{
		int iFAIOuterCircleID = 0;
		iFAIOuterCircleID = THEAPP.m_pModelDataManager->GetLastFAIOuterCircleID(THEAPP.m_pTabControlDlg->m_iCurrentTab);
		mpActiveTRegion->m_iFAIOuterCircleID = iFAIOuterCircleID + 1;
	}
	else if (mpActiveTRegion->m_iInspectionType == INSPECTION_TYPE_FAI_INNER_CIRCLE)
	{
		int iFAIInnerCircleID = 0;
		iFAIInnerCircleID = THEAPP.m_pModelDataManager->GetLastFAIInnerCircleID(THEAPP.m_pTabControlDlg->m_iCurrentTab);
		mpActiveTRegion->m_iFAIInnerCircleID = iFAIInnerCircleID + 1;
	}
	else if (mpActiveTRegion->m_iInspectionType == INSPECTION_TYPE_FAI_DATUM_CIRCLE)
	{
		int iFAIDatumCircleID = 0;
		iFAIDatumCircleID = THEAPP.m_pModelDataManager->GetLastFAIDatumCircleID(THEAPP.m_pTabControlDlg->m_iCurrentTab);
		mpActiveTRegion->m_iFAIDatumCircleID = iFAIDatumCircleID + 1;
	}
	else if (mpActiveTRegion->m_iInspectionType == INSPECTION_TYPE_FAI_DATUM_ELLIPSE)
	{
		int iFAIDatumEllipseID = 0;
		iFAIDatumEllipseID = THEAPP.m_pModelDataManager->GetLastFAIDatumEllipseID(THEAPP.m_pTabControlDlg->m_iCurrentTab);
		mpActiveTRegion->m_iFAIDatumEllipseID = iFAIDatumEllipseID + 1;
	}
	else if (mpActiveTRegion->m_iInspectionType == INSPECTION_TYPE_FAI_WELDING_SPOT)
	{
		int iFAIWeldingSpotID = 0;
		iFAIWeldingSpotID = THEAPP.m_pModelDataManager->GetLastFAIWeldingSpotID(THEAPP.m_pTabControlDlg->m_iCurrentTab);
		mpActiveTRegion->m_iFAIWeldingSpotID = iFAIWeldingSpotID + 1;
	}
	else if (mpActiveTRegion->m_iInspectionType == INSPECTION_TYPE_FAI_WELDING_POCKET)
	{
		int iFAIWeldingPocketID = 0;
		iFAIWeldingPocketID = THEAPP.m_pModelDataManager->GetLastFAIWeldingPocketID(THEAPP.m_pTabControlDlg->m_iCurrentTab);
		mpActiveTRegion->m_iFAIWeldingPocketID = iFAIWeldingPocketID + 1;
	}
	else if (mpActiveTRegion->m_iInspectionType == INSPECTION_TYPE_FAI_BRACKET)
	{
		int iFAIBracketID = 0;
		iFAIBracketID = THEAPP.m_pModelDataManager->GetLastFAIBracketID(THEAPP.m_pTabControlDlg->m_iCurrentTab);
		mpActiveTRegion->m_iFAIBracketID = iFAIBracketID + 1;
	}

	InvalidateRect(&m_ClientRect, FALSE);
}

#include "RoiIdChangeDlg.h"

void CInspectAdminViewDlg::OnMenuROIIDChange()
{
	if (mpActiveTRegion == NULL)
	{
		AfxMessageBox("먼저, ID번호를 변경할 ROI를 선택하여 주십시요.", MB_SYSTEMMODAL);
		return;
	}

	if (((mpActiveTRegion->m_iInspectionType == INSPECTION_TYPE_LOCAL_ALIGN) ||
		(mpActiveTRegion->m_iInspectionType >= INSPECTION_TYPE_FAI_START && mpActiveTRegion->m_iInspectionType <= INSPECTION_TYPE_FAI_END)) == FALSE)
	{
		AfxMessageBox("Local Align ROI, FAI ROI ID만 번호 변경 가능합니다.", MB_SYSTEMMODAL);
		return;
	}

	CROIIDChangeDlg dlg;

	dlg.SetROIType(mpActiveTRegion->m_iInspectionType);
	if (dlg.DoModal() == IDOK)
	{
		if (mpActiveTRegion->m_iInspectionType == INSPECTION_TYPE_LOCAL_ALIGN)
			mpActiveTRegion->m_iLocalAlignID = dlg.m_iEditId;
		else if (mpActiveTRegion->m_iInspectionType == INSPECTION_TYPE_FAI_OUTER_CIRCLE)
			mpActiveTRegion->m_iFAIOuterCircleID = dlg.m_iEditId;
		else if (mpActiveTRegion->m_iInspectionType == INSPECTION_TYPE_FAI_INNER_CIRCLE)
			mpActiveTRegion->m_iFAIInnerCircleID = dlg.m_iEditId;
		else if (mpActiveTRegion->m_iInspectionType == INSPECTION_TYPE_FAI_DATUM_CIRCLE)
			mpActiveTRegion->m_iFAIDatumCircleID = dlg.m_iEditId;
		else if (mpActiveTRegion->m_iInspectionType == INSPECTION_TYPE_FAI_DATUM_ELLIPSE)
			mpActiveTRegion->m_iFAIDatumEllipseID = dlg.m_iEditId;
		else if (mpActiveTRegion->m_iInspectionType == INSPECTION_TYPE_FAI_WELDING_SPOT)
			mpActiveTRegion->m_iFAIWeldingSpotID = dlg.m_iEditId;
		else if (mpActiveTRegion->m_iInspectionType == INSPECTION_TYPE_FAI_WELDING_POCKET)
			mpActiveTRegion->m_iFAIWeldingPocketID = dlg.m_iEditId;
		else if (mpActiveTRegion->m_iInspectionType == INSPECTION_TYPE_FAI_BRACKET)
			mpActiveTRegion->m_iFAIBracketID = dlg.m_iEditId;

		InvalidateRect(&m_ClientRect, FALSE);
	}
}


void CInspectAdminViewDlg::UpdateToolbarStatus(BOOL bTeachMode)
{
	CToolBarCtrl &toolbarctrl = m_ViewToolbar.GetToolBarCtrl();

	if (bTeachMode)
	{
		toolbarctrl.SetState(ID_TB_LIVE, TBSTATE_ENABLED);
		toolbarctrl.SetState(ID_TB_FIT_LINE_DISP, TBSTATE_ENABLED);
		toolbarctrl.SetState(ID_TB_ROI_DISP, TBSTATE_ENABLED);
		toolbarctrl.SetState(ID_TB_THRES_DISP, TBSTATE_ENABLED);
		toolbarctrl.SetState(ID_TB_DEFECT_DISP, TBSTATE_ENABLED);
		toolbarctrl.SetState(ID_TB_CALIPER, TBSTATE_ENABLED);
		toolbarctrl.SetState(ID_TB_AVERAGE_GRAY, TBSTATE_ENABLED);
		toolbarctrl.SetState(ID_TB_ROI_LOCK, TBSTATE_ENABLED);
		toolbarctrl.SetState(ID_TB_CROSS_BAR, TBSTATE_ENABLED);

		toolbarctrl.CheckButton(ID_TB_LIVE, FALSE);
		m_bLive = FALSE;
		toolbarctrl.CheckButton(ID_TB_FIT_LINE_DISP, m_bDisplayFitLine);
		toolbarctrl.CheckButton(ID_TB_ROI_DISP, m_bDisplayInspectArea);
		toolbarctrl.CheckButton(ID_TB_THRES_DISP, m_bDisplayThresholdArea);
		toolbarctrl.CheckButton(ID_TB_DEFECT_DISP, m_bDisplayDefectArea);
		toolbarctrl.CheckButton(ID_TB_CALIPER, FALSE);
		toolbarctrl.CheckButton(ID_TB_AVERAGE_GRAY, FALSE);
		toolbarctrl.CheckButton(ID_TB_ROI_LOCK, TRUE);
		toolbarctrl.CheckButton(ID_TB_CROSS_BAR, FALSE);
		m_caliper_control_flag = 0;
		m_AverageGray_control_flag = 0;
		m_bROILock = FALSE;
		OnTbROILock();

		m_bDispCrossBar = FALSE;

		m_iNoCaliperPoint = 0;
		mpLineSP = CPoint(0,0);
		mpLineEP = CPoint(0,0);
	}
	else
	{
		if (m_bLive)
		{
			toolbarctrl.CheckButton(ID_TB_LIVE,FALSE);
			m_bLive = FALSE;
			KillTimer(0);
		}

		toolbarctrl.SetState(ID_TB_LIVE, TBSTATE_HIDDEN);
		toolbarctrl.SetState(ID_TB_FIT_LINE_DISP, TBSTATE_HIDDEN);
		toolbarctrl.SetState(ID_TB_ROI_DISP, TBSTATE_HIDDEN);
		toolbarctrl.SetState(ID_TB_THRES_DISP, TBSTATE_HIDDEN);
		toolbarctrl.SetState(ID_TB_DEFECT_DISP, TBSTATE_HIDDEN);
		toolbarctrl.SetState(ID_TB_CALIPER, TBSTATE_HIDDEN);
		toolbarctrl.SetState(ID_TB_AVERAGE_GRAY, TBSTATE_HIDDEN);
		toolbarctrl.SetState(ID_TB_ROI_LOCK, TBSTATE_HIDDEN);
		toolbarctrl.SetState(ID_TB_CROSS_BAR, TBSTATE_HIDDEN);
	}
}

void CInspectAdminViewDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.

	DWORD dwGrabStart = 0, dwGrabEnd = 0;
	DWORD dwGrabElapsedTime = 0;

	switch(nIDEvent)
	{
	case 0:
		{
			if(THEAPP.m_iModeSwitch==MODE_ADMIN_TEACH_VIEW)		// 현재 탭의 셋팅으로 Live
			{
				if(m_bLive == TRUE)
				{
					if(m_bLiveGrabAvailable == TRUE)
					{
						m_bLiveGrabAvailable = FALSE;

						THEAPP.m_pCameraManager->m_bGrabDone = FALSE;
						THEAPP.m_pCameraManager->CallHookFunction();

						if (THEAPP.m_pTabControlDlg->m_iCurrentTab == 0)
						{
							THEAPP.m_pTriggerManager->FireTrigger(VISION_TYPE_INSPECTION, THEAPP.m_pModelDataManager->m_iBlackMatchingImageType);
						}
						else
						{
							THEAPP.m_pTriggerManager->FireTrigger(VISION_TYPE_INSPECTION, THEAPP.m_pTabControlDlg->m_iCurrentTab-1);
						}

						Sleep(THEAPP.Struct_PreferenceStruct.m_iTriggerSleepTime);
				
						dwGrabStart = GetTickCount();
						while (1)
						{
							if (THEAPP.m_pCameraManager->m_bGrabDone == TRUE)
								break;

							dwGrabEnd = GetTickCount();

							if((dwGrabEnd-dwGrabStart) > MAX_GRAB_TIMEOUT)
							{
								THEAPP.m_pCameraManager->GrabErrorPostProcess();
								break;
							}
						}

						m_bLiveGrabAvailable = TRUE;

					}  // if(m_bLiveGrabAvailable == TRUE)

				}  // if(m_bLive == TRUE)

			}  // if(THEAPP.m_iModeSwitch==MODE_ADMIN_TEACH_VIEW)

			break;
		}
	}

	CDialog::OnTimer(nIDEvent);
}

void CInspectAdminViewDlg::set_display_font (Halcon::HTuple WindowHandle, Halcon::HTuple Size, Halcon::HTuple Font, 
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

void CInspectAdminViewDlg::UniformityDisplay()
{
	CString strFolder = THEAPP.GetWorkingDirectory()+"\\Data\\";
	CIniFileCS INI(strFolder+"\\LightUniformityData.txt");
	CString strSection = "Mean";

	HTuple Number;
	opening_circle(Partition_Region,&Partition_Region,10);
	count_obj(Partition_Region,&Number);
	for(int i=0;i<Number;i++)
	{
		Hobject Select_Region;
		HTuple Mean, Deviation;
		HTuple Area, Row, Column;
		select_obj(Partition_Region,&Select_Region,i+1);
		intensity(Select_Region,m_pHImage,&Mean,&Deviation);
		area_center(Select_Region,&Area,&Row,&Column);
		set_tposition(m_lWindowID,Row,Column);
		set_display_font(m_lWindowID,12, "mono", "true", "false");
		set_color(m_lWindowID,"green");
		write_string(m_lWindowID,Mean[0].L());

		CString partname;
		partname.Format("Tab%d-Mean%d", THEAPP.m_pTabControlDlg->m_iCurrentTab, i+1);

		switch(THEAPP.m_pTabControlDlg->m_iCurrentTab)
		{
		case 1:
			{
				INI.Set_Long(strSection,partname, Mean[0].L());
				break;
			}
		case 2:
			{
				INI.Set_Long(strSection,partname, Mean[0].L());
				break;
			}
		case 3:
			{
				INI.Set_Long(strSection,partname, Mean[0].L());
				break;
			}
		case 4:
			{
				INI.Set_Long(strSection,partname, Mean[0].L());
				break;
			}
		}
	}

	strSection = "AcceptRange";
	INI.Set_Long(strSection,"Range",THEAPP.m_pTabControlDlg->m_pLightControlDlg->m_iAcceptRange_Uniformity);
}

void CInspectAdminViewDlg::UpdateView(BOOL bUpdate)
{
	InvalidateRect(&m_ClientRect, bUpdate);
}

void CInspectAdminViewDlg::ClearUpdateView(BOOL bUpdate)
{
	clear_window(m_lWindowID);
	InvalidateRect(&m_ClientRect, bUpdate);
}

void CInspectAdminViewDlg::DrawArrow(CDC *pDC)
{
	set_color(m_lWindowID, "red");
	set_draw(m_lWindowID, "fill");

	double dArrowSize;
	dArrowSize = 1.0/m_ViewportManager.mdZoomRatio;
	if (dArrowSize<0.645)
		dArrowSize = 0.645;
	else if (dArrowSize<0.87)
		dArrowSize = 0.87;

	disp_arrow ( m_lWindowID, mpLineSP.y, mpLineSP.x, mpLineEP.y, mpLineEP.x, dArrowSize ) ;
	disp_arrow ( m_lWindowID, mpLineEP.y, mpLineEP.x, mpLineSP.y, mpLineSP.x, dArrowSize ) ;

	DPOINT RealSP, RealEP ;

	int iCameraPixelSize;
	double dMmPerPixel = PIXEL_SIZE_PMODE;
	DPOINT dMPoint;
	double dMx;

	dMx = (double)mpLineSP.x;
	THEAPP.m_pCalDataService->IPtoRP(dMx, 0.0, &(dMPoint.x), &(dMPoint.y));	
	RealSP.x = dMPoint.x;
	RealSP.y = (double)mpLineSP.y * dMmPerPixel;
	//	
	dMx = (double)mpLineEP.x;
	THEAPP.m_pCalDataService->IPtoRP(dMx, 0.0, &(dMPoint.x), &(dMPoint.y));	
	RealEP.x = dMPoint.x;
	RealEP.y = (double)mpLineEP.y * dMmPerPixel;

	double dPixelLength = edLineLength(mpLineSP.x, mpLineSP.y, mpLineEP.x, mpLineEP.y);

	CString cstrDistance;
	cstrDistance.Format ( "길이: %.1lf um, %d 픽셀", edLineLength(RealSP.x, RealSP.y, RealEP.x, RealEP.y)*1000.0, (int)dPixelLength);
	HFONT CurrFont, OldFont;
	CurrFont = CreateFont(20, 0, 0, 0, 700, 0, 0, 0, ANSI_CHARSET, 0, 0, 0, 0, "Arial");
	OldFont = (HFONT)SelectObject(pDC->m_hDC, CurrFont);

	SetTextColor(pDC->m_hDC, RGB(255,0,0));
	TextOut(pDC->m_hDC, 5, 5, (LPCTSTR)cstrDistance.GetBuffer(0), strlen((LPCTSTR)cstrDistance));

	SelectObject(pDC->m_hDC, OldFont);
	DeleteObject(CurrFont);
}

double CInspectAdminViewDlg::edLineLength ( double sx, double sy, double ex, double ey )
{
	double num = ((ex - sx)*(ex - sx)) + ((ey - sy)*(ey - sy)) ;
	return sqrt ( num ) ;
}

void CInspectAdminViewDlg::EditXldData(int Row1, int Col1, int Row2, int Col2)
{
	try
	{
		if(!THEAPP.m_pModelDataManager->m_bOneBarrel)
		{
			if(THEAPP.m_pGFunction->ValidHXLD(ContoursAffinTrans[MATCHING_CONTOUR_1])==FALSE)
				return;

			Hobject SelXld;
			SelXld = ContoursAffinTrans[MATCHING_CONTOUR_1];

			if(!THEAPP.m_pGFunction->ValidHXLD(SelXld))
				return;

			HTuple HRow, HCol;
			get_contour_xld(SelXld, &HRow, &HCol);

			Hlong length;
			tuple_length(HRow, &length);
			for(int i=0; i<length; i++)
			{
				double dRow, dCol;
				dRow=HRow[i].D();
				dCol=HCol[i].D();
				if(dRow>Row1 && dRow<Row2 && dCol>Col1 && dCol<Col2)
				{
					tuple_remove(HRow, i, &HRow);
					tuple_remove(HCol, i, &HCol);
					i--;
					length--;
				}
			}

			tuple_length(HRow, &length);

			HTuple HSRow, HSCol, HERow, HECol;

			HSRow=HRow[0];
			HSCol=HCol[0];
			HERow=HRow[length-1];
			HECol=HCol[length-1];

			if(HSRow!=HERow || HSCol!=HECol)
			{
				tuple_concat(HRow, HSRow, &HRow);
				tuple_concat(HCol, HSCol, &HCol);
			}

			gen_contour_polygon_xld(&ContoursAffinTrans[MATCHING_CONTOUR_1], HRow, HCol);
		}
		else		// One Barrel
		{
			if(m_iTeachingContourType == 0)
			{
				if(THEAPP.m_pGFunction->ValidHXLD(ContoursAffinTrans[MATCHING_CONTOUR_3])==FALSE)
					return;

				Hobject SelXld;
				SelXld = ContoursAffinTrans[MATCHING_CONTOUR_3];

				if(!THEAPP.m_pGFunction->ValidHXLD(SelXld))
					return;

				HTuple HRow, HCol;
				get_contour_xld(SelXld, &HRow, &HCol);

				Hlong length;
				tuple_length(HRow, &length);
				for(int i=0; i<length; i++)
				{
					double dRow, dCol;
					dRow=HRow[i].D();
					dCol=HCol[i].D();
					if(dRow>Row1 && dRow<Row2 && dCol>Col1 && dCol<Col2)
					{
						tuple_remove(HRow, i, &HRow);
						tuple_remove(HCol, i, &HCol);
						i--;
						length--;
					}
				}

				tuple_length(HRow, &length);

				HTuple HSRow, HSCol, HERow, HECol;

				HSRow=HRow[0];
				HSCol=HCol[0];
				HERow=HRow[length-1];
				HECol=HCol[length-1];

				if(HSRow!=HERow || HSCol!=HECol)
				{
					tuple_concat(HRow, HSRow, &HRow);
					tuple_concat(HCol, HSCol, &HCol);
				}

				gen_contour_polygon_xld(&ContoursAffinTrans[MATCHING_CONTOUR_3], HRow, HCol);

			}
			else if(m_iTeachingContourType == 1)
			{
				if(THEAPP.m_pGFunction->ValidHXLD(ContoursAffinTrans[MATCHING_CONTOUR_4])==FALSE)
					return;

				Hobject SelXld;
				SelXld = ContoursAffinTrans[MATCHING_CONTOUR_4];

				if(!THEAPP.m_pGFunction->ValidHXLD(SelXld))
					return;

				HTuple HRow, HCol;
				get_contour_xld(SelXld, &HRow, &HCol);

				Hlong length;
				tuple_length(HRow, &length);
				for(int i=0; i<length; i++)
				{
					double dRow, dCol;
					dRow=HRow[i].D();
					dCol=HCol[i].D();
					if(dRow>Row1 && dRow<Row2 && dCol>Col1 && dCol<Col2)
					{
						tuple_remove(HRow, i, &HRow);
						tuple_remove(HCol, i, &HCol);
						i--;
						length--;
					}
				}

				tuple_length(HRow, &length);

				HTuple HSRow, HSCol, HERow, HECol;

				HSRow=HRow[0];
				HSCol=HCol[0];
				HERow=HRow[length-1];
				HECol=HCol[length-1];

				if(HSRow!=HERow || HSCol!=HECol)
				{
					tuple_concat(HRow, HSRow, &HRow);
					tuple_concat(HCol, HSCol, &HCol);
				}

				gen_contour_polygon_xld(&ContoursAffinTrans[MATCHING_CONTOUR_4], HRow, HCol);

			}
		}
	}

	catch(HException &except)
	{
		CString str;
		str.Format("Halcon Exception [EditXldData] : %s", except.message);
		THEAPP.SaveLog(str);
	}
}

void CInspectAdminViewDlg::StartLive()
{
	if (m_bLive==FALSE)
	{
		CToolBarCtrl &toolbarctrl = m_ViewToolbar.GetToolBarCtrl();

		toolbarctrl.CheckButton(ID_TB_LIVE,TRUE);
		m_bLive = TRUE;
		SetTimer(0, 100, NULL);
	}
}

void CInspectAdminViewDlg::StopLive()
{
	if (m_bLive)
	{
		CToolBarCtrl &toolbarctrl = m_ViewToolbar.GetToolBarCtrl();

		toolbarctrl.CheckButton(ID_TB_LIVE,FALSE);
		m_bLive = FALSE;
		KillTimer(0);
	}
}

void CInspectAdminViewDlg::CreateAutoFocusROI() 
{
	m_PrevToolBarState = m_ToolBarState;
	m_ToolBarState = TS_CREATE_AF_ROI;
}

void CInspectAdminViewDlg::ClearAutoFocusROI()
{
	m_HAutoFocusCheckRgn.Reset();
}

void CInspectAdminViewDlg::DiffPolygonROI()
{
	Hobject SelXld;

	if(m_iTeachingContourType == 0)
	{
		SelXld = ContoursAffinTrans[MATCHING_CONTOUR_3];
		if(THEAPP.m_pGFunction->ValidHXLD(SelXld))
		{
			Hobject HSelRegion;

			gen_region_contour_xld(SelXld, &HSelRegion, "filled");
			difference(HSelRegion, mHUnionPolygonRgn, &HSelRegion);

			gen_contour_region_xld (HSelRegion, &ContoursAffinTrans[MATCHING_CONTOUR_3], "border");
		}
	}
	else if(m_iTeachingContourType == 1)
	{
		SelXld = ContoursAffinTrans[MATCHING_CONTOUR_4];
		if(THEAPP.m_pGFunction->ValidHXLD(SelXld))
		{
			Hobject HSelRegion;

			gen_region_contour_xld(SelXld, &HSelRegion, "filled");
			difference(HSelRegion, mHUnionPolygonRgn, &HSelRegion);

			gen_contour_region_xld (HSelRegion, &ContoursAffinTrans[MATCHING_CONTOUR_4], "border");
		}
	}
}

void CInspectAdminViewDlg::OnRButtonDown(UINT nFlags, CPoint point)
{
	if (!m_bDisplayImage){
		CDialog::OnRButtonDown(nFlags, point);
		return;
	}

	if (m_iPolygonMode == 1 && m_ToolBarState == NC_TS_EDIT_POLYGON_XLD)
	{
		if (m_iPolygonCnt<=2)
			return;

		Hobject HPolygonRgn;
		HTuple HRows, HCols;

		tuple_gen_const(0, 0, &HRows);
		tuple_gen_const(0, 0, &HCols);

		for (int i=0; i<m_iPolygonCnt; i++)
		{
			tuple_concat(HRows, m_PolygonPt[i].y, &HRows);
			tuple_concat(HCols, m_PolygonPt[i].x, &HCols);
		}

		gen_region_polygon_filled(&HPolygonRgn, HRows, HCols);

		union2(mHUnionPolygonRgn, HPolygonRgn, &mHUnionPolygonRgn);

		m_iPolygonMode = 2;

		InvalidateRect(&m_ClientRect, FALSE);
	}
	else if (m_bPolygonMode && m_ToolBarState==TS_CREATE_ROI_POLYGON)
	{
		AddPolygonROI();

		m_bPolygonMode = FALSE;
		m_iPolygonCnt = 0;

		SetToolBarStateDraw();

		InvalidateRect(&m_ClientRect, FALSE);
	}
	else
	{
		CMenu *pSubMenu;
		pSubMenu = m_Menu.GetSubMenu(0);

		CPoint ClientPoint;
		ClientPoint = point;
		ClientToScreen(&ClientPoint);

		pSubMenu->TrackPopupMenu(TPM_LEFTALIGN, ClientPoint.x, ClientPoint.y, this);		
	}

	CDialog::OnRButtonDown(nFlags, point);
}

void CInspectAdminViewDlg::MoveActiveRegion(int iMoveDirection)
{
	if (mpActiveTRegion==NULL)
		return;

	DPOINT dCenterPointM;

	if (mpActiveTRegion->m_bRegionROI)
	{
		if (iMoveDirection==ROI_KEY_UP)
			move_region(mpActiveTRegion->m_HTeachPolygonRgn, &(mpActiveTRegion->m_HTeachPolygonRgn), -1, 0);
		else if (iMoveDirection==ROI_KEY_DOWN)
			move_region(mpActiveTRegion->m_HTeachPolygonRgn, &(mpActiveTRegion->m_HTeachPolygonRgn), 1, 0);
		else if (iMoveDirection==ROI_KEY_LEFT)
			move_region(mpActiveTRegion->m_HTeachPolygonRgn, &(mpActiveTRegion->m_HTeachPolygonRgn), 0, -1);
		else if (iMoveDirection==ROI_KEY_RIGHT)
			move_region(mpActiveTRegion->m_HTeachPolygonRgn, &(mpActiveTRegion->m_HTeachPolygonRgn), 0, 1);

		mpActiveTRegion->UpdatePolygonROIBoundary();
	}
	else
	{
		mpActiveTRegion->GetCenterPointM(&dCenterPointM);

		if (iMoveDirection==ROI_KEY_UP)
			dCenterPointM.y -= THEAPP.m_pCalDataService->GetPixelSize();
		else if (iMoveDirection==ROI_KEY_DOWN)
			dCenterPointM.y += THEAPP.m_pCalDataService->GetPixelSize();
		else if (iMoveDirection==ROI_KEY_LEFT)
			dCenterPointM.x -= THEAPP.m_pCalDataService->GetPixelSize();
		else if (iMoveDirection==ROI_KEY_RIGHT)
			dCenterPointM.x += THEAPP.m_pCalDataService->GetPixelSize();

		mpActiveTRegion->SetCenterPointM(dCenterPointM);
	}
}

void CInspectAdminViewDlg::ScaleActiveRegion(int iMoveDirection, BOOL bScaleUp)
{
	if (mpActiveTRegion==NULL)
		return;

	DPOINT dLTPointM, dRBPointM;

	if (mpActiveTRegion->m_bRegionROI==FALSE)
	{
		if (mpActiveTRegion->GetShape()==FALSE)	// Rect
		{
			mpActiveTRegion->GetLTPointM(&dLTPointM);
			mpActiveTRegion->GetRBPointM(&dRBPointM);

			if (iMoveDirection==ROI_KEY_UP)
			{
				if (bScaleUp)
					dLTPointM.y -= THEAPP.m_pCalDataService->GetPixelSize();
				else
					dLTPointM.y += THEAPP.m_pCalDataService->GetPixelSize();
			}
			else if (iMoveDirection==ROI_KEY_DOWN)
			{
				if (bScaleUp)
					dRBPointM.y += THEAPP.m_pCalDataService->GetPixelSize();
				else
					dRBPointM.y -= THEAPP.m_pCalDataService->GetPixelSize();
			}
			else if (iMoveDirection==ROI_KEY_LEFT)
			{
				if (bScaleUp)
					dLTPointM.x -= THEAPP.m_pCalDataService->GetPixelSize();
				else
					dLTPointM.x += THEAPP.m_pCalDataService->GetPixelSize();
			}
			else if (iMoveDirection==ROI_KEY_RIGHT)
			{
				if (bScaleUp)
					dRBPointM.x += THEAPP.m_pCalDataService->GetPixelSize();
				else
					dRBPointM.x -= THEAPP.m_pCalDataService->GetPixelSize();
			}

			mpActiveTRegion->SetLTPointM(dLTPointM);
			mpActiveTRegion->SetRBPointM(dRBPointM);
		}
		else	// Circle
		{
			mpActiveTRegion->GetLTPointM(&dLTPointM);
			mpActiveTRegion->GetRBPointM(&dRBPointM);

			if (iMoveDirection==ROI_KEY_UP)
			{
				if (bScaleUp)
				{
					dLTPointM.y -= THEAPP.m_pCalDataService->GetPixelSize();
					dLTPointM.x -= THEAPP.m_pCalDataService->GetPixelSize();
				}
				else
				{
					dLTPointM.y += THEAPP.m_pCalDataService->GetPixelSize();
					dLTPointM.x += THEAPP.m_pCalDataService->GetPixelSize();
				}
			}
			else if (iMoveDirection==ROI_KEY_DOWN)
			{
				if (bScaleUp)
				{
					dRBPointM.y += THEAPP.m_pCalDataService->GetPixelSize();
					dRBPointM.x += THEAPP.m_pCalDataService->GetPixelSize();
				}
				else
				{
					dRBPointM.y -= THEAPP.m_pCalDataService->GetPixelSize();
					dRBPointM.x -= THEAPP.m_pCalDataService->GetPixelSize();
				}
			}
			else if (iMoveDirection==ROI_KEY_LEFT)
			{
				if (bScaleUp)
				{
					dLTPointM.x -= THEAPP.m_pCalDataService->GetPixelSize();
					dLTPointM.y -= THEAPP.m_pCalDataService->GetPixelSize();
				}
				else
				{
					dLTPointM.x += THEAPP.m_pCalDataService->GetPixelSize();
					dLTPointM.y += THEAPP.m_pCalDataService->GetPixelSize();
				}
			}
			else if (iMoveDirection==ROI_KEY_RIGHT)
			{
				if (bScaleUp)
				{
					dRBPointM.x += THEAPP.m_pCalDataService->GetPixelSize();
					dRBPointM.y += THEAPP.m_pCalDataService->GetPixelSize();
				}
				else
				{
					dRBPointM.x -= THEAPP.m_pCalDataService->GetPixelSize();
					dRBPointM.y -= THEAPP.m_pCalDataService->GetPixelSize();
				}
			}

			mpActiveTRegion->SetLTPointM(dLTPointM);
			mpActiveTRegion->SetRBPointM(dRBPointM);
		}
	}
}

void CInspectAdminViewDlg::ClearActiveTRegion()
{
	mpActiveTRegion = NULL;
}

void CInspectAdminViewDlg::ClearAllSelectRegion()
{
	if (THEAPP.m_pModelDataManager->m_pInspectionArea)
		THEAPP.m_pModelDataManager->m_pInspectionArea->ClearAllSelect();
}

void CInspectAdminViewDlg::ClearAllLastSelectRegion()
{
	if (THEAPP.m_pModelDataManager->m_pInspectionArea)
		THEAPP.m_pModelDataManager->m_pInspectionArea->ClearAllLastSelected();
}

void CInspectAdminViewDlg::SaveInspectTabOverLastSelectRegion()
{
	if (m_pLastSelectedRegion)
		delegateCurROITabParamSave[THEAPP.m_pTabControlDlg->m_iCurrentTab-1].FireEvent((WPARAM)THEAPP.m_pTabControlDlg->m_iCurrentTab-1);
	
}

void CInspectAdminViewDlg::SetToolBarStateDraw()
{
	m_ToolBarState = TS_DRAW;

	CToolBarCtrl &toolbarctrl = m_ViewToolbar.GetToolBarCtrl();

	toolbarctrl.CheckButton(ID_TB_DRAW,TRUE);
	toolbarctrl.CheckButton(ID_TB_MOVE,FALSE);
	toolbarctrl.CheckButton(ID_TB_SELECT_PART,FALSE);
	toolbarctrl.CheckButton(ID_TB_ROI_RECT,FALSE);
	toolbarctrl.CheckButton(ID_TB_ROI_CIRCLE,FALSE);
	toolbarctrl.CheckButton(ID_TB_ROI_POLYGON,FALSE);
}

void CInspectAdminViewDlg::AddPolygonROI()
{
	if (m_iPolygonCnt<=2)
		return;

	if (THEAPP.m_pTabControlDlg->m_iCurrentTab <= TRIGGER_SURFACE)
		return;

	CInspectionTypeDlg InspectionTypeDlg;
	if (InspectionTypeDlg.DoModal()==IDCANCEL)
		return;

	HTuple HRows, HCols;

	tuple_gen_const(0, 0, &HRows);
	tuple_gen_const(0, 0, &HCols);

	for (int i=0; i<m_iPolygonCnt; i++)
	{
		tuple_concat(HRows, m_PolygonPt[i].y, &HRows);
		tuple_concat(HCols, m_PolygonPt[i].x, &HCols);
	}

	Hobject HPolygonRgn;
	gen_region_polygon_filled(&HPolygonRgn, HRows, HCols);

	POINT LTPoint, RBPoint;
	Hlong lRow1, lRow2, lCol1, lCol2;

	smallest_rectangle1(HPolygonRgn, &lRow1, &lCol1, &lRow2, &lCol2);

	LTPoint.x = lCol1;
	LTPoint.y = lRow1;
	RBPoint.x = lCol2;
	RBPoint.y = lRow2;

	GTRegion *pPolygonRgn;

	pPolygonRgn = new GTRegion;      
	pPolygonRgn->SetLTPoint(LTPoint, THEAPP.m_pCalDataService);
	pPolygonRgn->SetRBPoint(RBPoint, THEAPP.m_pCalDataService);            
	pPolygonRgn->SetVisible(TRUE, FALSE);
	pPolygonRgn->SetSelectable(TRUE, FALSE);
	pPolygonRgn->SetMovable(TRUE, FALSE);
	pPolygonRgn->SetSizable(FALSE, FALSE);

	pPolygonRgn->m_iTeachImageIndex = THEAPP.m_pTabControlDlg->m_iCurrentTab;
	pPolygonRgn->m_iInspectionType = InspectionTypeDlg.m_iRadioInspectionType;

	if (pPolygonRgn->m_iInspectionType == INSPECTION_TYPE_ALIGN)
		pPolygonRgn->SetLineColor(RGB(0,255,255));
	else if (pPolygonRgn->m_iInspectionType == INSPECTION_TYPE_DELETE)
		pPolygonRgn->SetLineColor(RGB(0,0,255));
	else if (pPolygonRgn->m_iInspectionType == INSPECTION_TYPE_ADD)
		pPolygonRgn->SetLineColor(RGB(255,128,0));
	else if (pPolygonRgn->m_iInspectionType == INSPECTION_TYPE_LOCAL_ALIGN)	// 24.02.29 Local Align 추가 - LeeGW
		pPolygonRgn->SetLineColor(RGB(255,0,255));
	else if (pPolygonRgn->m_iInspectionType == INSPECTION_TYPE_AI_INSPECTION)	// 24.10.30 AI 검사 - LeeGW
		pPolygonRgn->SetLineColor(RGB(255, 250, 0));
	else if ((pPolygonRgn->m_iInspectionType >= INSPECTION_TYPE_FAI_START && pPolygonRgn->m_iInspectionType <= INSPECTION_TYPE_FAI_END))
		pPolygonRgn->SetLineColor(RGB(255, 165, 0));
	else
		pPolygonRgn->SetLineColor(RGB(0,255,0));

	pPolygonRgn->m_bRegionROI = TRUE;
	copy_obj(HPolygonRgn, &(pPolygonRgn->m_HTeachPolygonRgn), 1, -1);

	Hobject HROIHRegion;
	BOOL bDontCareExist, bAddExist;

	if ( (pPolygonRgn->m_iInspectionType == INSPECTION_TYPE_DELETE || pPolygonRgn->m_iInspectionType == INSPECTION_TYPE_ADD)==FALSE)
	{
		HROIHRegion = pPolygonRgn->GetROIHRegion(THEAPP.m_pCalDataService);

		bDontCareExist = FALSE;
		bDontCareExist = CheckDontCareInclusion(HROIHRegion);

		if (bDontCareExist)
		{
			int nRes = 0;
			nRes = AfxMessageBox("새로운 ROI 내에 삭제 ROI가 존재합니다. 삭제 ROI 영역을 제외하시겠습니까?", MB_YESNO|MB_SYSTEMMODAL);

			if ((nRes==IDYES))
			{
				GetRegionROIAfterDontCareRemoval(pPolygonRgn);
			}
		}

		bAddExist = FALSE;
		bAddExist = CheckAddInclusion(HROIHRegion);

		if (bAddExist)
		{
			int nRes = 0;
			nRes = AfxMessageBox("새로운 ROI 내에 추가 ROI가 존재합니다. ROI 영역을 추가하시겠습니까?", MB_YESNO|MB_SYSTEMMODAL);

			if ((nRes==IDYES))
			{
				GetRegionROIAfterAddRemoval(pPolygonRgn);
			}
		}
	}

	int iLastID = THEAPP.m_pModelDataManager->GetLastPadID();
	pPolygonRgn->miPadID = iLastID + 1;

	// 24.02.29 Local Align 추가 - LeeGW
	if (pPolygonRgn->m_iInspectionType == INSPECTION_TYPE_LOCAL_ALIGN)
	{
		int iLastLocalAlignID = 0;
		iLastLocalAlignID = THEAPP.m_pModelDataManager->GetLastLocalAlignID(THEAPP.m_pTabControlDlg->m_iCurrentTab);
		pPolygonRgn->m_iLocalAlignID = iLastLocalAlignID + 1;
	}
	// FAI 추가 - LeeGW
	else if (pPolygonRgn->m_iInspectionType == INSPECTION_TYPE_FAI_OUTER_CIRCLE)
	{
		int iFAIOuterCircleID = 0;
		iFAIOuterCircleID = THEAPP.m_pModelDataManager->GetLastFAIOuterCircleID(THEAPP.m_pTabControlDlg->m_iCurrentTab);
		pPolygonRgn->m_iFAIOuterCircleID = iFAIOuterCircleID + 1;
	}
	else if (pPolygonRgn->m_iInspectionType == INSPECTION_TYPE_FAI_INNER_CIRCLE)
	{
		int iFAIInnerCircleID = 0;
		iFAIInnerCircleID = THEAPP.m_pModelDataManager->GetLastFAIInnerCircleID(THEAPP.m_pTabControlDlg->m_iCurrentTab);
		pPolygonRgn->m_iFAIInnerCircleID = iFAIInnerCircleID + 1;
	}
	else if (pPolygonRgn->m_iInspectionType == INSPECTION_TYPE_FAI_DATUM_CIRCLE)
	{
		int iFAIDatumCircleID = 0;
		iFAIDatumCircleID = THEAPP.m_pModelDataManager->GetLastFAIDatumCircleID(THEAPP.m_pTabControlDlg->m_iCurrentTab);
		pPolygonRgn->m_iFAIDatumCircleID = iFAIDatumCircleID + 1;
	}
	else if (pPolygonRgn->m_iInspectionType == INSPECTION_TYPE_FAI_DATUM_ELLIPSE)
	{
		int iFAIDatumEllipseID = 0;
		iFAIDatumEllipseID = THEAPP.m_pModelDataManager->GetLastFAIDatumEllipseID(THEAPP.m_pTabControlDlg->m_iCurrentTab);
		pPolygonRgn->m_iFAIDatumEllipseID = iFAIDatumEllipseID + 1;
	}
	else if (pPolygonRgn->m_iInspectionType == INSPECTION_TYPE_FAI_WELDING_SPOT)
	{
		int iFAIWeldingSpotID = 0;
		iFAIWeldingSpotID = THEAPP.m_pModelDataManager->GetLastFAIWeldingSpotID(THEAPP.m_pTabControlDlg->m_iCurrentTab);
		pPolygonRgn->m_iFAIWeldingSpotID = iFAIWeldingSpotID + 1;
	}
	else if (pPolygonRgn->m_iInspectionType == INSPECTION_TYPE_FAI_WELDING_POCKET)
	{
		int iFAIWeldingPocketID = 0;
		iFAIWeldingPocketID = THEAPP.m_pModelDataManager->GetLastFAIWeldingPocketID(THEAPP.m_pTabControlDlg->m_iCurrentTab);
		pPolygonRgn->m_iFAIWeldingPocketID = iFAIWeldingPocketID + 1;
	}
	else if (pPolygonRgn->m_iInspectionType == INSPECTION_TYPE_FAI_BRACKET)
	{
		int iFAIBracketID = 0;
		iFAIBracketID = THEAPP.m_pModelDataManager->GetLastFAIBracketID(THEAPP.m_pTabControlDlg->m_iCurrentTab);
		pPolygonRgn->m_iFAIBracketID = iFAIBracketID + 1;
	}

	THEAPP.m_pModelDataManager->m_pInspectionArea->AddChildTRegion(pPolygonRgn);
}

void CInspectAdminViewDlg::CopyInspectionROI()
{
	if (mpActiveTRegion==NULL)
	{
		AfxMessageBox("먼저, 복사할 검사 ROI를 선택하여 주십시요.", MB_SYSTEMMODAL);
		return;
	}

	GTRegion *pCopyRegion;
	Hlong lRow1, lCol1, lRow2, lCol2;

	mpActiveTRegion->Duplicate(&pCopyRegion);

	pCopyRegion->m_iTeachImageIndex = m_iRoiCopyImageIndex;

	if (THEAPP.m_pTabControlDlg->m_iCurrentTab == m_iRoiCopyImageIndex)		// 동일 영상에 복사
	{
		DPOINT dCenterPoint, dWHPoint, dNewCenterPoint;

		if (pCopyRegion->m_bRegionROI)
		{
			POINT LTPoint, RBPoint;
			Hlong lRow1, lRow2, lCol1, lCol2;

			move_region(pCopyRegion->m_HTeachPolygonRgn, &(pCopyRegion->m_HTeachPolygonRgn), 50, 50);

			smallest_rectangle1(pCopyRegion->m_HTeachPolygonRgn, &lRow1, &lCol1, &lRow2, &lCol2);

			LTPoint.x = lCol1;
			LTPoint.y = lRow1;
			RBPoint.x = lRow2;
			RBPoint.x = lCol2;

			pCopyRegion->SetLTPoint(LTPoint, THEAPP.m_pCalDataService);
			pCopyRegion->SetRBPoint(RBPoint, THEAPP.m_pCalDataService);            
		}
		else
		{
			pCopyRegion->GetCenterPointM(&dCenterPoint);
			pCopyRegion->GetWHPointM(&dWHPoint);

			dNewCenterPoint.x = dCenterPoint.x + dWHPoint.x/2;
			dNewCenterPoint.y = dCenterPoint.y + dWHPoint.y/2;

			pCopyRegion->SetCenterPointM(dNewCenterPoint);
		}
	}

	int iLastID = THEAPP.m_pModelDataManager->GetLastPadID();
	pCopyRegion->miPadID = iLastID + 1;
	
	// 24.02.29 Local Align 추가 - LeeGW
	if (pCopyRegion->m_iInspectionType == INSPECTION_TYPE_LOCAL_ALIGN)
	{
		int iLastLocalAlignID = 0;
		iLastLocalAlignID = THEAPP.m_pModelDataManager->GetLastLocalAlignID(pCopyRegion->m_iTeachImageIndex);
		pCopyRegion->m_iLocalAlignID = iLastLocalAlignID + 1;
	}// FAI 추가 - LeeGW
	else if (pCopyRegion->m_iInspectionType == INSPECTION_TYPE_FAI_OUTER_CIRCLE)
	{
		int iFAIOuterCircleID = 0;
		iFAIOuterCircleID = THEAPP.m_pModelDataManager->GetLastFAIOuterCircleID(pCopyRegion->m_iTeachImageIndex);
		pCopyRegion->m_iFAIOuterCircleID = iFAIOuterCircleID + 1;
	}
	else if (pCopyRegion->m_iInspectionType == INSPECTION_TYPE_FAI_INNER_CIRCLE)
	{
		int iFAIInnerCircleID = 0;
		iFAIInnerCircleID = THEAPP.m_pModelDataManager->GetLastFAIInnerCircleID(pCopyRegion->m_iTeachImageIndex);
		pCopyRegion->m_iFAIInnerCircleID = iFAIInnerCircleID + 1;
	}
	else if (pCopyRegion->m_iInspectionType == INSPECTION_TYPE_FAI_DATUM_CIRCLE)
	{
		int iFAIDatumCircleID = 0;
		iFAIDatumCircleID = THEAPP.m_pModelDataManager->GetLastFAIDatumCircleID(pCopyRegion->m_iTeachImageIndex);
		pCopyRegion->m_iFAIDatumCircleID = iFAIDatumCircleID + 1;
	}
	else if (pCopyRegion->m_iInspectionType == INSPECTION_TYPE_FAI_DATUM_ELLIPSE)
	{
		int iFAIDatumEllipseID = 0;
		iFAIDatumEllipseID = THEAPP.m_pModelDataManager->GetLastFAIDatumEllipseID(pCopyRegion->m_iTeachImageIndex);
		pCopyRegion->m_iFAIDatumEllipseID = iFAIDatumEllipseID + 1;
	}
	else if (pCopyRegion->m_iInspectionType == INSPECTION_TYPE_FAI_WELDING_SPOT)
	{
		int iFAIWeldingSpotID = 0;
		iFAIWeldingSpotID = THEAPP.m_pModelDataManager->GetLastFAIWeldingSpotID(pCopyRegion->m_iTeachImageIndex);
		pCopyRegion->m_iFAIWeldingSpotID = iFAIWeldingSpotID + 1;
	}
	else if (pCopyRegion->m_iInspectionType == INSPECTION_TYPE_FAI_WELDING_POCKET)
	{
		int iFAIWeldingPocketID = 0;
		iFAIWeldingPocketID = THEAPP.m_pModelDataManager->GetLastFAIWeldingPocketID(pCopyRegion->m_iTeachImageIndex);
		pCopyRegion->m_iFAIWeldingPocketID = iFAIWeldingPocketID + 1;
	}
	else if (pCopyRegion->m_iInspectionType == INSPECTION_TYPE_FAI_BRACKET)
	{
		int iFAIBracketID = 0;
		iFAIBracketID = THEAPP.m_pModelDataManager->GetLastFAIBracketID(pCopyRegion->m_iTeachImageIndex);
		pCopyRegion->m_iFAIBracketID = iFAIBracketID + 1;
	}

	THEAPP.m_pModelDataManager->m_pInspectionArea->AddChildTRegion(pCopyRegion);

	InvalidateRect(&m_ClientRect, TRUE);

	CString sMsg;
	if (m_iRoiCopyImageIndex==0)
		sMsg.Format("기본정보 탭에 복사되었습니다.");
	else
		sMsg.Format("영상 %d 탭에 복사되었습니다.", m_iRoiCopyImageIndex);

	AfxMessageBox(sMsg, MB_ICONINFORMATION|MB_SYSTEMMODAL);
}

void CInspectAdminViewDlg::ClearAlignTabTempRegion()
{
	gen_empty_obj(&m_HShapeModelContrastRgn);
}

BOOL CInspectAdminViewDlg::CheckDontCareInclusion(Hobject HNewRegion)
{
	if (THEAPP.m_pGFunction->ValidHRegion(HNewRegion)==FALSE)
		return FALSE;

	if (THEAPP.m_pModelDataManager->m_pInspectionArea==NULL)
		return FALSE;

	// Region ROI

	Hobject HDontCareRgn;
	gen_empty_obj(&HDontCareRgn);

	Hobject HROIHRegion;

	GTRegion *pRegion;
	for (int k=0; k<THEAPP.m_pModelDataManager->m_pInspectionArea->GetChildTRegionCount(); k++)
	{
		pRegion = THEAPP.m_pModelDataManager->m_pInspectionArea->GetChildTRegion(k);
		if (pRegion == NULL)
			continue;

		if (pRegion->m_iTeachImageIndex>=0 && pRegion->m_iTeachImageIndex==THEAPP.m_pTabControlDlg->m_iCurrentTab)
		{
			if (pRegion->m_iInspectionType == INSPECTION_TYPE_DELETE)
			{
				HROIHRegion = pRegion->GetROIHRegion(THEAPP.m_pCalDataService);
				if (THEAPP.m_pGFunction->ValidHRegion(HROIHRegion))
					concat_obj(HDontCareRgn, HROIHRegion, &HDontCareRgn);
			}
		}
	}

	if (THEAPP.m_pGFunction->ValidHRegion(HDontCareRgn))
	{
		Hobject HIntersectRgn;

		union1(HDontCareRgn, &HDontCareRgn);
		intersection(HDontCareRgn, HNewRegion, &HIntersectRgn);

		if (THEAPP.m_pGFunction->ValidHRegion(HIntersectRgn))
			return TRUE;
		else
			return FALSE;
	}
	else
		return FALSE;
}

void CInspectAdminViewDlg::GetRegionROIAfterDontCareRemoval(GTRegion *pNewTRegion)
{
	int i;

	Hobject HPolygonRgn;
	HPolygonRgn = pNewTRegion->GetROIHRegion(THEAPP.m_pCalDataService);

	Hobject HDontCareRgn;
	gen_empty_obj(&HDontCareRgn);

	PList<GTRegion> *pDeleteTRegionList = new PList<GTRegion>(PLNO_POINTER);

	Hobject HROIHRegion;
	Hobject HIntersectRgn;

	GTRegion *pRegion;
	for (int k=0; k<THEAPP.m_pModelDataManager->m_pInspectionArea->GetChildTRegionCount(); k++)
	{
		pRegion = THEAPP.m_pModelDataManager->m_pInspectionArea->GetChildTRegion(k);
		if (pRegion == NULL)
			continue;

		if (pRegion->m_iTeachImageIndex>=0 && pRegion->m_iTeachImageIndex==THEAPP.m_pTabControlDlg->m_iCurrentTab)
		{
			if (pRegion->m_iInspectionType == INSPECTION_TYPE_DELETE)
			{
				HROIHRegion = pRegion->GetROIHRegion(THEAPP.m_pCalDataService);
				if (THEAPP.m_pGFunction->ValidHRegion(HROIHRegion))
				{
					intersection(HROIHRegion, HPolygonRgn, &HIntersectRgn);
					if (THEAPP.m_pGFunction->ValidHRegion(HIntersectRgn))
					{
						concat_obj(HDontCareRgn, HROIHRegion, &HDontCareRgn);
						pDeleteTRegionList->Add(pRegion);
					}
				}
			}
		}
	}

	// DontCare TRegion 제거
	for (i = 0; i < pDeleteTRegionList->miCount; i++) 
	{
		GTRegion *pDeleteTRegion = pDeleteTRegionList->Get(i);
		THEAPP.m_pModelDataManager->m_pInspectionArea->DeleteChildTRegionPtr(pDeleteTRegion, TRUE);
	}
	delete pDeleteTRegionList;

	if (THEAPP.m_pGFunction->ValidHRegion(HDontCareRgn))
	{
		union1(HDontCareRgn, &HDontCareRgn);
		difference(HPolygonRgn, HDontCareRgn, &HPolygonRgn);
	}

	POINT LTPoint, RBPoint;
	Hlong lRow1, lRow2, lCol1, lCol2;
	
	smallest_rectangle1(HPolygonRgn, &lRow1, &lCol1, &lRow2, &lCol2);

	LTPoint.x = lCol1;
	LTPoint.y = lRow1;
	RBPoint.x = lCol2;
	RBPoint.y = lRow2;

	pNewTRegion->SetLTPoint(LTPoint, THEAPP.m_pCalDataService);
	pNewTRegion->SetRBPoint(RBPoint, THEAPP.m_pCalDataService);            
	pNewTRegion->SetVisible(TRUE, FALSE);
	pNewTRegion->SetSelectable(TRUE, FALSE);
	pNewTRegion->SetMovable(TRUE, FALSE);
	pNewTRegion->SetSizable(FALSE, FALSE);

	pNewTRegion->m_bRegionROI = TRUE;
	copy_obj(HPolygonRgn, &(pNewTRegion->m_HTeachPolygonRgn), 1, -1);
}

BOOL CInspectAdminViewDlg::CheckAddInclusion(Hobject HNewRegion)
{
	if (THEAPP.m_pGFunction->ValidHRegion(HNewRegion)==FALSE)
		return FALSE;

	if (THEAPP.m_pModelDataManager->m_pInspectionArea==NULL)
		return FALSE;

	// Region ROI

	Hobject HAddRgn;
	gen_empty_obj(&HAddRgn);

	Hobject HROIHRegion;

	GTRegion *pRegion;
	for (int k=0; k<THEAPP.m_pModelDataManager->m_pInspectionArea->GetChildTRegionCount(); k++)
	{
		pRegion = THEAPP.m_pModelDataManager->m_pInspectionArea->GetChildTRegion(k);
		if (pRegion == NULL)
			continue;

		if (pRegion->m_iTeachImageIndex>=0 && pRegion->m_iTeachImageIndex==THEAPP.m_pTabControlDlg->m_iCurrentTab)
		{
			if (pRegion->m_iInspectionType == INSPECTION_TYPE_ADD)
			{
				HROIHRegion = pRegion->GetROIHRegion(THEAPP.m_pCalDataService);
				if (THEAPP.m_pGFunction->ValidHRegion(HROIHRegion))
					concat_obj(HAddRgn, HROIHRegion, &HAddRgn);
			}
		}
	}

	if (THEAPP.m_pGFunction->ValidHRegion(HAddRgn))
	{
		Hobject HIntersectRgn;

		union1(HAddRgn, &HAddRgn);
		intersection(HAddRgn, HNewRegion, &HIntersectRgn);

		if (THEAPP.m_pGFunction->ValidHRegion(HIntersectRgn))
			return TRUE;
		else
			return FALSE;
	}
	else
		return FALSE;
}

void CInspectAdminViewDlg::GetRegionROIAfterAddRemoval(GTRegion *pNewTRegion)
{
	int i;

	Hobject HPolygonRgn;
	HPolygonRgn = pNewTRegion->GetROIHRegion(THEAPP.m_pCalDataService);

	Hobject HAddRgn;
	gen_empty_obj(&HAddRgn);

	PList<GTRegion> *pDeleteTRegionList = new PList<GTRegion>(PLNO_POINTER);

	Hobject HROIHRegion;
	Hobject HIntersectRgn;

	GTRegion *pRegion;
	for (int k=0; k<THEAPP.m_pModelDataManager->m_pInspectionArea->GetChildTRegionCount(); k++)
	{
		pRegion = THEAPP.m_pModelDataManager->m_pInspectionArea->GetChildTRegion(k);
		if (pRegion == NULL)
			continue;

		if (pRegion->m_iTeachImageIndex>=0 && pRegion->m_iTeachImageIndex==THEAPP.m_pTabControlDlg->m_iCurrentTab)
		{
			if (pRegion->m_iInspectionType == INSPECTION_TYPE_ADD)
			{
				HROIHRegion = pRegion->GetROIHRegion(THEAPP.m_pCalDataService);
				if (THEAPP.m_pGFunction->ValidHRegion(HROIHRegion))
				{
					intersection(HROIHRegion, HPolygonRgn, &HIntersectRgn);
					if (THEAPP.m_pGFunction->ValidHRegion(HIntersectRgn))
					{
						concat_obj(HAddRgn, HROIHRegion, &HAddRgn);
						pDeleteTRegionList->Add(pRegion);
					}
				}
			}
		}
	}

	// Add TRegion 제거
	for (i = 0; i < pDeleteTRegionList->miCount; i++) 
	{
		GTRegion *pDeleteTRegion = pDeleteTRegionList->Get(i);
		THEAPP.m_pModelDataManager->m_pInspectionArea->DeleteChildTRegionPtr(pDeleteTRegion, TRUE);
	}
	delete pDeleteTRegionList;

	if (THEAPP.m_pGFunction->ValidHRegion(HAddRgn))
	{
		union1(HAddRgn, &HAddRgn);
		union2(HPolygonRgn, HAddRgn, &HPolygonRgn);
	}

	POINT LTPoint, RBPoint;
	Hlong lRow1, lRow2, lCol1, lCol2;

	smallest_rectangle1(HPolygonRgn, &lRow1, &lCol1, &lRow2, &lCol2);

	LTPoint.x = lCol1;
	LTPoint.y = lRow1;
	RBPoint.x = lCol2;
	RBPoint.y = lRow2;

	pNewTRegion->SetLTPoint(LTPoint, THEAPP.m_pCalDataService);
	pNewTRegion->SetRBPoint(RBPoint, THEAPP.m_pCalDataService);            
	pNewTRegion->SetVisible(TRUE, FALSE);
	pNewTRegion->SetSelectable(TRUE, FALSE);
	pNewTRegion->SetMovable(TRUE, FALSE);
	pNewTRegion->SetSizable(FALSE, FALSE);

	pNewTRegion->m_bRegionROI = TRUE;
	copy_obj(HPolygonRgn, &(pNewTRegion->m_HTeachPolygonRgn), 1, -1);
}

void CInspectAdminViewDlg::ResetInspectionResult()
{
	gen_empty_obj(&m_HInspectAreaRgn);
	gen_empty_obj(&m_HParameterDefectRgn);
	gen_empty_obj(&m_HParameterDefectXLD);
}

void CInspectAdminViewDlg::AddRegionROI(int iThresMin, int iThresMax, BOOL bCheckMinBlob, BOOL bCheckMaxArea, BOOL bCheckFillUp, BOOL bCheckOpeningClosingInverse,
	BOOL bCheckOpening, BOOL bCheckClosing, BOOL bCheckConvex, BOOL bCheckRect, BOOL bCheckDifference, BOOL bCheckBoundary, BOOL bCheckDilation, int iEditMinBlobSize, int iEditOpeningSize, int iEditClosingSize, int iEditBoundaryDilationSize, int iEditDilationSize)
{
	try
	{
		if (!m_bDisplayImage)
			return;

		if (mpActiveTRegion)
		{
			CInspectionTypeDlg InspectionTypeDlg;
			if (InspectionTypeDlg.DoModal()==IDCANCEL)
				return;

			Hobject HReducedImage, HROIRgn, HThresRgn;

			if (mpActiveTRegion->m_bRegionROI)
			{
				HROIRgn = mpActiveTRegion->m_HTeachPolygonRgn;
			}
			else
			{
				POINT LTPoint, RBPoint;

				mpActiveTRegion->GetLTPoint(&LTPoint, THEAPP.m_pCalDataService);
				mpActiveTRegion->GetRBPoint(&RBPoint, THEAPP.m_pCalDataService);

				if (mpActiveTRegion->GetShape()==TRUE)
					gen_circle(&HROIRgn, (LTPoint.y+RBPoint.y)/2, (LTPoint.x+RBPoint.x)/2, (RBPoint.y-LTPoint.y)/2);
				else
					gen_rectangle1(&HROIRgn, LTPoint.y, LTPoint.x, RBPoint.y, RBPoint.x);
			}

			reduce_domain(m_HTeachingImage[THEAPP.m_pTabControlDlg->m_iCurrentTab-1], HROIRgn, &HReducedImage);

			threshold(HReducedImage, &HThresRgn, iThresMin, iThresMax);

			Hobject HSelectedRgn, HHoleRgn, HFillRgn, HBoundaryRgn;
			Hlong lNoBlob;

			if (THEAPP.m_pGFunction->ValidHRegion(HThresRgn))
			{
				if (bCheckMinBlob)
				{
					connection(HThresRgn, &HThresRgn);
					select_shape(HThresRgn, &HThresRgn,  "area", "and", iEditMinBlobSize, 9999999999);
					union1(HThresRgn, &HThresRgn);
				}

				if (bCheckMaxArea)
				{
					connection(HThresRgn, &HThresRgn);
					select_shape_std(HThresRgn, &HThresRgn, HTuple("max_area"), HTuple(70));
				}

				if (bCheckFillUp)
				{
					fill_up(HThresRgn, &HThresRgn);
				}

				if (bCheckOpeningClosingInverse)
				{
					if (bCheckClosing)
						closing_circle(HThresRgn, &HThresRgn, (double)iEditClosingSize+0.5);;
					if (bCheckOpening)
						opening_circle(HThresRgn, &HThresRgn, (double)iEditOpeningSize+0.5);
				}
				else
				{
					if (bCheckOpening)
						opening_circle(HThresRgn, &HThresRgn, (double)iEditOpeningSize+0.5);
					if (bCheckClosing)
						closing_circle(HThresRgn, &HThresRgn, (double)iEditClosingSize+0.5);;
				}

				if (bCheckConvex)
				{
					if (bCheckMinBlob)
					{
						connection(HThresRgn, &HThresRgn);

						HTuple HlNoBlob;
						count_obj(HThresRgn, &lNoBlob);

						gen_empty_obj(&HHoleRgn);

						for (int i=1; i<=lNoBlob; i++)
						{
							select_obj(HThresRgn, &HSelectedRgn, i);

							fill_up(HSelectedRgn, &HFillRgn);
							difference(HFillRgn, HSelectedRgn, &HSelectedRgn);

							if (THEAPP.m_pGFunction->ValidHRegion(HSelectedRgn))
							{
								shape_trans(HSelectedRgn, &HSelectedRgn, "convex");
								concat_obj(HHoleRgn, HSelectedRgn, &HHoleRgn);
							}
						}

						union1(HHoleRgn, &HHoleRgn);

						shape_trans(HThresRgn, &HThresRgn, "convex");
						union1(HThresRgn, &HThresRgn);	

						difference(HThresRgn, HHoleRgn, &HThresRgn);
					}
					else
						shape_trans(HThresRgn, &HThresRgn, "convex");
				}

				if (bCheckRect)
				{
					if (bCheckMinBlob)
					{
						connection(HThresRgn, &HThresRgn);
						shape_trans(HThresRgn, &HThresRgn, "rectangle1");
						union1(HThresRgn, &HThresRgn);	
					}
					else
					{
						shape_trans(HThresRgn, &HThresRgn, "rectangle1");
					}
				}

				if (bCheckDifference)
				{
					difference(HROIRgn, HThresRgn, &HThresRgn);
				}

				if (bCheckBoundary)
				{
					connection(HThresRgn, &HThresRgn);

					HTuple HlNoBlob;
					count_obj(HThresRgn, &lNoBlob);

					gen_empty_obj(&HBoundaryRgn);

					for (int i=1; i<=lNoBlob; i++)
					{
						select_obj(HThresRgn, &HSelectedRgn, i);

						fill_up(HSelectedRgn, &HSelectedRgn);
						boundary(HSelectedRgn, &HSelectedRgn, "inner");
						dilation_circle(HSelectedRgn, &HSelectedRgn, (double)iEditBoundaryDilationSize+0.5);

						if (THEAPP.m_pGFunction->ValidHRegion(HSelectedRgn))
						{
							concat_obj(HBoundaryRgn, HSelectedRgn, &HBoundaryRgn);
						}
					}

					union1(HBoundaryRgn, &HThresRgn);
				}

				if (bCheckDilation)
				{
					if (iEditDilationSize >= 0)
					{
						dilation_circle(HThresRgn, &HThresRgn, (double)iEditDilationSize+0.5);
					}
					else
					{
						int iMargin = abs(iEditDilationSize);
						erosion_circle(HThresRgn, &HThresRgn, (double)iMargin + 0.5);
					}
				}
			}

			if (THEAPP.m_pGFunction->ValidHRegion(HThresRgn))
			{
				if (THEAPP.m_pModelDataManager->m_pInspectionArea!=NULL)
				{
					if (THEAPP.m_pModelDataManager->m_pInspectionArea->GetChildTRegionCount()>0)
					{
						THEAPP.m_pModelDataManager->m_pInspectionArea->DeleteChildTRegionPtr(mpActiveTRegion, FALSE);
						mpActiveTRegion = NULL;
					}

					POINT LTPoint, RBPoint;
					Hlong lRow1, lRow2, lCol1, lCol2;

					smallest_rectangle1(HThresRgn, &lRow1, &lCol1, &lRow2, &lCol2);

					LTPoint.x = lCol1;
					LTPoint.y = lRow1;
					RBPoint.x = lCol2;
					RBPoint.y = lRow2;

					GTRegion *pPolygonRgn;

					pPolygonRgn = new GTRegion;      
					pPolygonRgn->SetLTPoint(LTPoint, THEAPP.m_pCalDataService);
					pPolygonRgn->SetRBPoint(RBPoint, THEAPP.m_pCalDataService);            
					pPolygonRgn->SetVisible(TRUE, FALSE);
					pPolygonRgn->SetSelectable(TRUE, FALSE);
					pPolygonRgn->SetMovable(TRUE, FALSE);
					pPolygonRgn->SetSizable(FALSE, FALSE);

					pPolygonRgn->m_iTeachImageIndex = THEAPP.m_pTabControlDlg->m_iCurrentTab;

					pPolygonRgn->m_iInspectionType = InspectionTypeDlg.m_iRadioInspectionType;

					if (pPolygonRgn->m_iInspectionType == INSPECTION_TYPE_DELETE)
						pPolygonRgn->SetLineColor(RGB(0,0,255));
					else if (pPolygonRgn->m_iInspectionType == INSPECTION_TYPE_ALIGN)
						pPolygonRgn->SetLineColor(RGB(0,255,255));
					else if (pPolygonRgn->m_iInspectionType == INSPECTION_TYPE_ADD)
						pPolygonRgn->SetLineColor(RGB(255,128,0));
					else if (pPolygonRgn->m_iInspectionType == INSPECTION_TYPE_LOCAL_ALIGN)	// 24.02.29 Local Align 추가 - LeeGW
						pPolygonRgn->SetLineColor(RGB(255,0,255));
					else if (pPolygonRgn->m_iInspectionType == INSPECTION_TYPE_AI_INSPECTION)	// 24.10.30 AI 검사 - LeeGW
						pPolygonRgn->SetLineColor(RGB(255, 255, 0));
					else if ((pPolygonRgn->m_iInspectionType >= INSPECTION_TYPE_FAI_START && pPolygonRgn->m_iInspectionType <= INSPECTION_TYPE_FAI_END))
						pPolygonRgn->SetLineColor(RGB(255, 165, 0));
					else
						pPolygonRgn->SetLineColor(RGB(0,255,0));

					pPolygonRgn->m_bRegionROI = TRUE;
					copy_obj(HThresRgn, &(pPolygonRgn->m_HTeachPolygonRgn), 1, -1);

					Hobject HROIHRegion;
					BOOL bDontCareExist, bAddExist;

					if ( (pPolygonRgn->m_iInspectionType == INSPECTION_TYPE_DELETE || pPolygonRgn->m_iInspectionType == INSPECTION_TYPE_ADD)==FALSE)
					{
						HROIHRegion = pPolygonRgn->GetROIHRegion(THEAPP.m_pCalDataService);

						bDontCareExist = FALSE;
						bDontCareExist = CheckDontCareInclusion(HROIHRegion);

						if (bDontCareExist)
						{
							int nRes = 0;
							nRes = AfxMessageBox("새로운 ROI 내에 삭제 ROI가 존재합니다. 삭제 ROI 영역을 제외하시겠습니까?", MB_YESNO|MB_SYSTEMMODAL);

							if ((nRes==IDYES))
							{
								GetRegionROIAfterDontCareRemoval(pPolygonRgn);
							}
						}

						bAddExist = FALSE;
						bAddExist = CheckAddInclusion(HROIHRegion);

						if (bAddExist)
						{
							int nRes = 0;
							nRes = AfxMessageBox("새로운 ROI 내에 추가 ROI가 존재합니다. ROI 영역을 추가하시겠습니까?", MB_YESNO|MB_SYSTEMMODAL);

							if ((nRes==IDYES))
							{
								GetRegionROIAfterAddRemoval(pPolygonRgn);
							}
						}
					}

					int iLastID = THEAPP.m_pModelDataManager->GetLastPadID();
					pPolygonRgn->miPadID = iLastID + 1;

					// 24.02.29 Local Align 추가 - LeeGW
					if (pPolygonRgn->m_iInspectionType == INSPECTION_TYPE_LOCAL_ALIGN)
					{
						int iLastLocalAlignID = 0;
						iLastLocalAlignID = THEAPP.m_pModelDataManager->GetLastLocalAlignID(THEAPP.m_pTabControlDlg->m_iCurrentTab);
						pPolygonRgn->m_iLocalAlignID = iLastLocalAlignID + 1;
					}	
					// FAI 추가 - LeeGW
					else if (pPolygonRgn->m_iInspectionType == INSPECTION_TYPE_FAI_OUTER_CIRCLE)
					{
						int iFAIOuterCircleID = 0;
						iFAIOuterCircleID = THEAPP.m_pModelDataManager->GetLastFAIOuterCircleID(THEAPP.m_pTabControlDlg->m_iCurrentTab);
						pPolygonRgn->m_iFAIOuterCircleID = iFAIOuterCircleID + 1;
					}
					else if (pPolygonRgn->m_iInspectionType == INSPECTION_TYPE_FAI_INNER_CIRCLE)
					{
						int iFAIInnerCircleID = 0;
						iFAIInnerCircleID = THEAPP.m_pModelDataManager->GetLastFAIInnerCircleID(THEAPP.m_pTabControlDlg->m_iCurrentTab);
						pPolygonRgn->m_iFAIInnerCircleID = iFAIInnerCircleID + 1;
					}
					else if (pPolygonRgn->m_iInspectionType == INSPECTION_TYPE_FAI_DATUM_CIRCLE)
					{
						int iFADatumCircleID = 0;
						iFADatumCircleID = THEAPP.m_pModelDataManager->GetLastFAIDatumCircleID(THEAPP.m_pTabControlDlg->m_iCurrentTab);
						pPolygonRgn->m_iFAIDatumCircleID = iFADatumCircleID + 1;
					}
					else if (pPolygonRgn->m_iInspectionType == INSPECTION_TYPE_FAI_DATUM_ELLIPSE)
					{
						int iFAIDatumEllipseID = 0;
						iFAIDatumEllipseID = THEAPP.m_pModelDataManager->GetLastFAIDatumEllipseID(THEAPP.m_pTabControlDlg->m_iCurrentTab);
						pPolygonRgn->m_iFAIDatumEllipseID = iFAIDatumEllipseID + 1;
					}
					else if (pPolygonRgn->m_iInspectionType == INSPECTION_TYPE_FAI_WELDING_SPOT)
					{
						int iFAIWeldingSpotID = 0;
						iFAIWeldingSpotID = THEAPP.m_pModelDataManager->GetLastFAIWeldingSpotID(THEAPP.m_pTabControlDlg->m_iCurrentTab);
						pPolygonRgn->m_iFAIWeldingSpotID = iFAIWeldingSpotID + 1;
					}
					else if (pPolygonRgn->m_iInspectionType == INSPECTION_TYPE_FAI_WELDING_POCKET)
					{
						int iFAIWeldingPocketID = 0;
						iFAIWeldingPocketID = THEAPP.m_pModelDataManager->GetLastFAIWeldingPocketID(THEAPP.m_pTabControlDlg->m_iCurrentTab);
						pPolygonRgn->m_iFAIWeldingPocketID = iFAIWeldingPocketID + 1;
					}
					else if (pPolygonRgn->m_iInspectionType == INSPECTION_TYPE_FAI_BRACKET)
					{
						int iFAIBracketID = 0;
						iFAIBracketID = THEAPP.m_pModelDataManager->GetLastFAIBracketID(THEAPP.m_pTabControlDlg->m_iCurrentTab);
						pPolygonRgn->m_iFAIBracketID = iFAIBracketID + 1;
					}

					THEAPP.m_pModelDataManager->m_pInspectionArea->AddChildTRegion(pPolygonRgn);

					gen_empty_obj(&m_pThresRgn);
				}				
			}

			InvalidateRect(&m_ClientRect, FALSE);
		}
	}
	catch(HException &except)
	{
		CString str;
		str.Format("Halcon Exception [AddRegionROI]");
		THEAPP.SaveLog(str);
	}
}

void CInspectAdminViewDlg::ShowRegionROI(int iThresMin, int iThresMax, BOOL bCheckMinBlob, BOOL bCheckMaxArea, BOOL bCheckFillUp, BOOL bCheckOpeningClosingInverse,
	BOOL bCheckOpening, BOOL bCheckClosing, BOOL bCheckConvex, BOOL bCheckRect, BOOL bCheckDifference, BOOL bCheckBoundary, BOOL bCheckDilation, int iEditMinBlobSize, int iEditOpeningSize, int iEditClosingSize, int iEditBoundaryDilationSize, int iEditDilationSize)
{
	try
	{
		if (!m_bDisplayImage)
			return;

		if (mpActiveTRegion)
		{
			Hobject HReducedImage, HROIRgn, HThresRgn;

			if (mpActiveTRegion->m_bRegionROI)
			{
				HROIRgn = mpActiveTRegion->m_HTeachPolygonRgn;
			}
			else
			{
				POINT LTPoint, RBPoint;

				mpActiveTRegion->GetLTPoint(&LTPoint, THEAPP.m_pCalDataService);
				mpActiveTRegion->GetRBPoint(&RBPoint, THEAPP.m_pCalDataService);

				if (mpActiveTRegion->GetShape()==TRUE)
					gen_circle(&HROIRgn, (LTPoint.y+RBPoint.y)/2, (LTPoint.x+RBPoint.x)/2, (RBPoint.y-LTPoint.y)/2);
				else
					gen_rectangle1(&HROIRgn, LTPoint.y, LTPoint.x, RBPoint.y, RBPoint.x);
			}

			reduce_domain(m_HTeachingImage[THEAPP.m_pTabControlDlg->m_iCurrentTab-1], HROIRgn, &HReducedImage);

			threshold(HReducedImage, &HThresRgn, iThresMin, iThresMax);

			Hobject HSelectedRgn, HHoleRgn, HFillRgn, HBoundaryRgn;
			Hlong lNoBlob;

			if (THEAPP.m_pGFunction->ValidHRegion(HThresRgn))
			{
				if (bCheckMinBlob)
				{
					connection(HThresRgn, &HThresRgn);
					select_shape(HThresRgn, &HThresRgn,  "area", "and", iEditMinBlobSize, 9999999999);
					union1(HThresRgn, &HThresRgn);
				}

				if (bCheckMaxArea)
				{
					connection(HThresRgn, &HThresRgn);
					select_shape_std(HThresRgn, &HThresRgn, HTuple("max_area"), HTuple(70));
				}

				if (bCheckFillUp)
				{
					fill_up(HThresRgn, &HThresRgn);
				}

				if (bCheckOpeningClosingInverse)
				{
					if (bCheckClosing)
						closing_circle(HThresRgn, &HThresRgn, (double)iEditClosingSize+0.5);;
					if (bCheckOpening)
						opening_circle(HThresRgn, &HThresRgn, (double)iEditOpeningSize+0.5);
				}
				else
				{
					if (bCheckOpening)
						opening_circle(HThresRgn, &HThresRgn, (double)iEditOpeningSize+0.5);
					if (bCheckClosing)
						closing_circle(HThresRgn, &HThresRgn, (double)iEditClosingSize+0.5);;
				}

				if (bCheckConvex)
				{
					if (bCheckMinBlob)
					{
						connection(HThresRgn, &HThresRgn);

						HTuple HlNoBlob;
						count_obj(HThresRgn, &lNoBlob);

						gen_empty_obj(&HHoleRgn);

						for (int i=1; i<=lNoBlob; i++)
						{
							select_obj(HThresRgn, &HSelectedRgn, i);

							fill_up(HSelectedRgn, &HFillRgn);
							difference(HFillRgn, HSelectedRgn, &HSelectedRgn);

							if (THEAPP.m_pGFunction->ValidHRegion(HSelectedRgn))
							{
								shape_trans(HSelectedRgn, &HSelectedRgn, "convex");
								concat_obj(HHoleRgn, HSelectedRgn, &HHoleRgn);
							}
						}

						union1(HHoleRgn, &HHoleRgn);

						shape_trans(HThresRgn, &HThresRgn, "convex");
						union1(HThresRgn, &HThresRgn);	

						difference(HThresRgn, HHoleRgn, &HThresRgn);
					}
					else
						shape_trans(HThresRgn, &HThresRgn, "convex");
				}

				if (bCheckRect)
				{
					if (bCheckMinBlob)
					{
						connection(HThresRgn, &HThresRgn);
						shape_trans(HThresRgn, &HThresRgn, "rectangle1");
						union1(HThresRgn, &HThresRgn);	
					}
					else
					{
						shape_trans(HThresRgn, &HThresRgn, "rectangle1");
					}
				}

				if (bCheckDifference)
				{
					difference(HROIRgn, HThresRgn, &HThresRgn);
				}

				if (bCheckBoundary)
				{
					connection(HThresRgn, &HThresRgn);

					HTuple HlNoBlob;
					count_obj(HThresRgn, &lNoBlob);

					gen_empty_obj(&HBoundaryRgn);

					for (int i=1; i<=lNoBlob; i++)
					{
						select_obj(HThresRgn, &HSelectedRgn, i);

						fill_up(HSelectedRgn, &HSelectedRgn);
						boundary(HSelectedRgn, &HSelectedRgn, "inner");
						dilation_circle(HSelectedRgn, &HSelectedRgn, (double)iEditBoundaryDilationSize+0.5);

						if (THEAPP.m_pGFunction->ValidHRegion(HSelectedRgn))
						{
							concat_obj(HBoundaryRgn, HSelectedRgn, &HBoundaryRgn);
						}
					}

					union1(HBoundaryRgn, &HThresRgn);
				}

				if (bCheckDilation)
				{
					if (iEditDilationSize >= 0)
					{
						dilation_circle(HThresRgn, &HThresRgn, (double)iEditDilationSize+0.5);
					}
					else
					{
						int iMargin = abs(iEditDilationSize);
						erosion_circle(HThresRgn, &HThresRgn, (double)iMargin + 0.5);
					}
				}
			}

			if (THEAPP.m_pGFunction->ValidHRegion(HThresRgn))
			{
				m_pThresRgn = HThresRgn;
			}
			else
				gen_empty_obj(&m_pThresRgn);

			InvalidateRect(&m_ClientRect, FALSE);
		}
	}
	catch(HException &except)
	{
		CString str;
		str.Format("Halcon Exception [ShowRegionROI]");
		THEAPP.SaveLog(str);
	}
}

// ROI 회전 - LeeGW
void CInspectAdminViewDlg::GetRegionROIAfterRotation(double dRotationAngle)
{
	GTRegion* pNewTRegion = mpActiveTRegion;

	if (pNewTRegion == NULL)
	{
		AfxMessageBox("먼저, 변경할 ROI를 선택하여 주십시요.", MB_SYSTEMMODAL);
		return;
	}

	int i;

	Hobject HPolygonRgn;
	HPolygonRgn = pNewTRegion->GetROIHRegion(THEAPP.m_pCalDataService);

	HTuple HAngleRad;

	tuple_rad(HTuple(dRotationAngle), &HAngleRad);

	HTuple HArea, HCenterX, HCenterY;
	area_center(HPolygonRgn, &HArea, &HCenterY, &HCenterX);

	HTuple HomMat2DIdentity, HomMat2DRotate;
	hom_mat2d_identity(&HomMat2DIdentity);
	hom_mat2d_rotate(HomMat2DIdentity, HAngleRad, HCenterY, HCenterX, &HomMat2DRotate);

	affine_trans_region(HPolygonRgn, &HPolygonRgn, HomMat2DRotate, "false");

	POINT LTPoint, RBPoint;
	Hlong lRow1, lRow2, lCol1, lCol2;

	HTuple HlRow1, HlRow2, HlCol1, HlCol2;

	smallest_rectangle1(HPolygonRgn, &HlRow1, &HlCol1, &HlRow2, &HlCol2);

	lRow1 = HlRow1[0].L();
	lCol1 = HlCol1[0].L();
	lRow2 = HlRow2[0].L();
	lCol2 = HlCol2[0].L();

	LTPoint.x = lCol1;
	LTPoint.y = lRow1;
	RBPoint.x = lCol2;
	RBPoint.y = lRow2;

	pNewTRegion->SetLTPoint(LTPoint, THEAPP.m_pCalDataService);
	pNewTRegion->SetRBPoint(RBPoint, THEAPP.m_pCalDataService);
	pNewTRegion->SetVisible(TRUE, FALSE);
	pNewTRegion->SetSelectable(TRUE, FALSE);
	pNewTRegion->SetMovable(TRUE, FALSE);
	pNewTRegion->SetSizable(FALSE, FALSE);

	pNewTRegion->m_bRegionROI = TRUE;
	copy_obj(HPolygonRgn, &(pNewTRegion->m_HTeachPolygonRgn), 1, -1);

	InvalidateRect(&m_ClientRect, FALSE);
}