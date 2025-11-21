// ThresholdHistogramDlg.cpp : implementation file
//

#include "stdafx.h"
#include "uscan.h"
#include "ThresholdHistogramDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char* THIS_FILE = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CThresholdHistogramDlg dialog
CThresholdHistogramDlg* CThresholdHistogramDlg::m_pInstance = NULL;

CThresholdHistogramDlg* CThresholdHistogramDlg::GetInstance(BOOL bShowFlag)
{
	if (!m_pInstance) {
		m_pInstance = new CThresholdHistogramDlg();
		if(!m_pInstance->m_hWnd) {
			CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
			m_pInstance->Create(IDD_THRESHOLD_HISTOGRAM_DIALOG, pFrame->GetActiveView());
			if (bShowFlag) m_pInstance->Show();
		}
	}
	return m_pInstance;
}

void CThresholdHistogramDlg::DeleteInstance()
{
	if(m_pInstance->GetSafeHwnd())
		m_pInstance->DestroyWindow();
	SAFE_DELETE(m_pInstance);
}

void CThresholdHistogramDlg::Show()
{
	ShowWindow(SW_HIDE);
	MoveWindow(m_ScreenRect.left, m_ScreenRect.top, m_ScreenRect.right, m_ScreenRect.bottom);
	ShowWindow(SW_SHOW);
}

CThresholdHistogramDlg::CThresholdHistogramDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CThresholdHistogramDlg::IDD, pParent)
{

	SetPosition(VIEW1_DLG3_LEFT+100, VIEW1_DLG3_TOP+40+50, VIEW1_DLG3_WIDTH-520, VIEW1_DLG3_HEIGHT+170);

	//{{AFX_DATA_INIT(CThresholdHistogramDlg)
	m_nEditLower = 0;
	m_nEditUpper = 0;
	//}}AFX_DATA_INIT

	m_BackRect.top = 0;
	m_BackRect.bottom = 0;
	m_BackRect.left = 0;
	m_BackRect.right = 0;

	m_DrawRect.top = 0;
	m_DrawRect.bottom = 0;
	m_DrawRect.left = 0;
	m_DrawRect.right = 0;

	m_nLowerBar = 0;
	m_nUpperBar = 255; 

	m_iSelectedLine = -1;

	m_nPeakHistoValue = 0;
	m_nPeakThreshold = -1;
	m_bUseDarkArea = FALSE;
	gen_empty_obj(&m_HProcessingImage);
	m_bDrawBar = TRUE;

	m_bCheckMinBlob = TRUE;
	m_bCheckMaxArea = FALSE;
	m_bCheckFillUp = FALSE;
	m_bCheckConvex = FALSE;
	m_bCheckRect = FALSE;
	m_bCheckOpening = FALSE;
	m_bCheckClosing = FALSE;
	m_bCheckOpeningClosingInverse = FALSE;
	m_bCheckDifference = FALSE;
	m_bCheckBoundary = FALSE;
	m_iEditMinBlobSize = 100;
	m_iEditOpeningSize = 1;
	m_iEditClosingSize = 1;
	m_iEditBoundaryDilationSize = 12;
	m_bCheckDilation = FALSE;
	m_iEditDilationSize = 5;
}

void CThresholdHistogramDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CThresholdHistogramDlg)
	DDX_Text(pDX, IDC_EDIT_LOWER, m_nEditLower);
	DDX_Text(pDX, IDC_EDIT_UPPER, m_nEditUpper);
	//}}AFX_DATA_MAP
	DDX_Control(pDX, IDC_BUTTON_MAKE_REGION_ROI, m_bnMakeRegionROI);
	DDX_Control(pDX, IDC_BUTTON_TEST_REGION, m_bnTestRegionROI);
	DDX_Check(pDX, IDC_CHECK_MIN_BLOB, m_bCheckMinBlob);
	DDX_Check(pDX, IDC_CHECK_MAX_AREA, m_bCheckMaxArea);
	DDX_Check(pDX, IDC_CHECK_FILLUP, m_bCheckFillUp);
	DDX_Check(pDX, IDC_CHECK_CONVEX, m_bCheckConvex);
	DDX_Check(pDX, IDC_CHECK_RECT, m_bCheckRect);
	DDX_Check(pDX, IDC_CHECK_OPENING, m_bCheckOpening);
	DDX_Check(pDX, IDC_CHECK_CLOSING, m_bCheckClosing);
	DDX_Check(pDX, IDC_CHECK_OPENING_CLOSING_INVERSE, m_bCheckOpeningClosingInverse);
	DDX_Check(pDX, IDC_CHECK_DIFFERENCE, m_bCheckDifference);
	DDX_Text(pDX, IDC_EDIT_MIN_BLOB_SIZE, m_iEditMinBlobSize);
	DDX_Text(pDX, IDC_EDIT_OPENING_SIZE, m_iEditOpeningSize);
	DDX_Text(pDX, IDC_EDIT_CLOSING_SIZE, m_iEditClosingSize);
	DDX_Check(pDX, IDC_CHECK_BOUNDARY, m_bCheckBoundary);
	DDX_Text(pDX, IDC_EDIT_BOUNDARY_MARGIN, m_iEditBoundaryDilationSize);
	DDX_Check(pDX, IDC_CHECK_DILATION, m_bCheckDilation);
	DDX_Text(pDX, IDC_EDIT_DILATION_SIZE, m_iEditDilationSize);
}

BEGIN_MESSAGE_MAP(CThresholdHistogramDlg, CDialog)
	//{{AFX_MSG_MAP(CThresholdHistogramDlg)
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDOWN()
	ON_WM_PAINT()
	ON_WM_DESTROY()
	ON_EN_CHANGE(IDC_EDIT_LOWER, OnChangeEditLower)
	ON_EN_CHANGE(IDC_EDIT_UPPER, OnChangeEditUpper)
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BUTTON_MAKE_REGION_ROI, &CThresholdHistogramDlg::OnBnClickedButtonMakeRegionRoi)
	ON_BN_CLICKED(IDC_BUTTON_TEST_REGION, &CThresholdHistogramDlg::OnBnClickedButtonTestRegion)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CThresholdHistogramDlg message handlers

BOOL CThresholdHistogramDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	GetClientRect(&m_BackRect);

	m_DrawRect.left = CCSD_EDGE_X - 2;
	m_DrawRect.top = CCSD_EDGE_Y - 2;
	m_DrawRect.right = CCSD_EDGE_X + CCSD_DRAWWIDTH + 2;
	m_DrawRect.bottom = CCSD_EDGE_Y + CCSD_DRAWHEIGHT + 2;

	m_bnMakeRegionROI.Init_Ctrl(_T("Arial"), 9, FALSE, BLACK, ROYAL_BLUE, 0, 0);
	m_bnTestRegionROI.Init_Ctrl(_T("Arial"), 9, FALSE, BLACK, ROYAL_BLUE, 0, 0);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CThresholdHistogramDlg::OnMouseMove(UINT nFlags, CPoint point) 
{
	if (IS_SETFLAG(nFlags, MK_LBUTTON)) {
		if (m_DrawRect.PtInRect(point)) {
			if (m_iSelectedLine == 0) {
				m_nLowerBar = ((double)point.x - CCSD_EDGE_X) / CCSD_DRAWWIDTH * 255;
				
				if (m_nLowerBar > m_nUpperBar - 1)	m_nLowerBar = m_nUpperBar - 1;
				if (m_nLowerBar < 0) m_nLowerBar = 0;

				UpdateEdit();  
				InvalidateRect(&m_DrawRect, false);

			} else if (m_iSelectedLine == 1) {
				m_nUpperBar = ((double)point.x - CCSD_EDGE_X) / CCSD_DRAWWIDTH * 255;
				
				if (m_nUpperBar < m_nLowerBar + 1)	m_nUpperBar = m_nLowerBar + 1;
				if (m_nUpperBar > 255) m_nUpperBar = 255;

				UpdateEdit();  
				InvalidateRect(&m_DrawRect, false);
			} 
		}
	}
	CDialog::OnMouseMove(nFlags, point);
}

void CThresholdHistogramDlg::OnLButtonDown(UINT nFlags, CPoint point) 
{
	if (m_DrawRect.PtInRect(point)) {
		int iDistanceFromLeftBar = abs(m_nLowerBar - ((double)point.x - CCSD_EDGE_X) / CCSD_DRAWWIDTH * 255);
		int iDistanceFromRightBar = abs(m_nUpperBar - ((double)point.x - CCSD_EDGE_X) / CCSD_DRAWWIDTH * 255);
		
		int iShortestDistance = iDistanceFromLeftBar;

		m_iSelectedLine = 0;
		if (iShortestDistance > iDistanceFromRightBar) {
			iShortestDistance = iDistanceFromRightBar;
			m_iSelectedLine = 1;
		}

		if (m_iSelectedLine == 0) {
			m_nLowerBar = ((double)point.x - CCSD_EDGE_X) / CCSD_DRAWWIDTH * 255;
			if (m_nLowerBar > m_nUpperBar - 1)	m_nLowerBar = m_nUpperBar - 1;
			if (m_nLowerBar < 0) m_nLowerBar = 0;

		} else if (m_iSelectedLine == 1) {
			m_nUpperBar = ((double)point.x - CCSD_EDGE_X) / CCSD_DRAWWIDTH * 255;
			if (m_nUpperBar < m_nLowerBar + 1)	m_nUpperBar = m_nLowerBar + 1;
			if (m_nUpperBar > 255) m_nUpperBar = 255;
		} 
		UpdateEdit();
		InvalidateRect(&m_DrawRect, FALSE);
	}
	CDialog::OnLButtonDown(nFlags, point);
}

void CThresholdHistogramDlg::OnPaint() 
{
	if (m_nPeakHistoValue < 0) 
		return;

	CPaintDC dc(this); // device context for painting
	
	// TODO: Add your message handler code here

	CPen *pOldPen, Pen;

	///////////////////////////////////////////////////////////////////////
	//배경 그리기 (검정)
	dc.SelectObject(CBrush::FromHandle((HBRUSH)GetStockObject(BLACK_BRUSH)));
	dc.Rectangle(0, 0, m_BackRect.right, m_BackRect.bottom-135);

	//가로선
	Pen.CreatePen(PS_SOLID, 2, RGB(255, 255, 255));
	pOldPen = dc.SelectObject(&Pen);
	dc.MoveTo(CCSD_EDGE_X, CCSD_EDGE_Y + CCSD_DRAWHEIGHT);
	dc.LineTo(CCSD_EDGE_X + CCSD_DRAWWIDTH, CCSD_EDGE_Y + CCSD_DRAWHEIGHT);
	dc.SelectObject(pOldPen);
	Pen.DeleteObject();

	//세로선
	Pen.CreatePen(PS_SOLID, 2, RGB(255, 255, 255));
	pOldPen = dc.SelectObject(&Pen);
	dc.MoveTo(CCSD_EDGE_X, CCSD_EDGE_Y);
	dc.LineTo(CCSD_EDGE_X, CCSD_EDGE_Y + CCSD_DRAWHEIGHT);   
	dc.SetBkMode(TRANSPARENT);  

	// Convert to Absolute Height
	CString strMinThres, strMidThres, strMaxThres, strMidHisto, strMaxHisto, strPeak;

	strMinThres.Format("%d", 0);
	strMidThres.Format("%d", 127);
	strMaxThres.Format("%d", 255);
	strMidHisto.Format("%d", m_nPeakHistoValue / 2);
	strMaxHisto.Format("%d", m_nPeakHistoValue);

	dc.SetTextColor(RGB(128, 255, 255));
	dc.TextOut(CCSD_EDGE_X, CCSD_EDGE_Y + CCSD_DRAWHEIGHT, strMinThres);
	dc.TextOut(CCSD_EDGE_X + CCSD_DRAWWIDTH / 2 , CCSD_EDGE_Y + CCSD_DRAWHEIGHT, strMidThres);
	dc.TextOut(CCSD_DRAWWIDTH + 10 , CCSD_EDGE_Y + CCSD_DRAWHEIGHT , strMaxThres);
	dc.TextOut(CCSD_EDGE_X - 25, (CCSD_EDGE_Y + CCSD_DRAWHEIGHT) / 2, strMidHisto);
	dc.TextOut(CCSD_EDGE_X - 25, CCSD_EDGE_Y - 3, strMaxHisto);

	int iPeakValuePosX;
	strPeak.Format("%d", m_nPeakThreshold);
	iPeakValuePosX = CCSD_EDGE_X + (int)((double)CCSD_DRAWWIDTH * ((double)m_nPeakThreshold/255.0) + 0.5);
	dc.SetTextColor(RGB(0, 255, 0));
	dc.TextOut(iPeakValuePosX , CCSD_EDGE_Y + CCSD_DRAWHEIGHT, strPeak);

	dc.SelectObject(pOldPen);
	Pen.DeleteObject();

	//히스토그램
	Pen.CreatePen(PS_SOLID, 1, RGB(0, 128, 255));
	pOldPen = dc.SelectObject(&Pen); 

	for (int i = 0; i < 256; i++) {
		dc.MoveTo(((CCSD_DRAWWIDTH - 2) / 256 * i) + CCSD_EDGE_X + 2, CCSD_EDGE_Y + CCSD_DRAWHEIGHT - 2);
 		dc.LineTo(((CCSD_DRAWWIDTH - 2) / 256 * i) + CCSD_EDGE_X + 2, CCSD_EDGE_Y + CCSD_DRAWHEIGHT - (long)m_iHistogram[i] * CCSD_DRAWHEIGHT / m_nPeakHistoValue);
 	}

	dc.SelectObject(pOldPen);
	Pen.DeleteObject();

	if (!m_bDrawBar) return;

	// Lower Bar Line
	if (m_nLowerBar >= 0 && m_nLowerBar < 255) {
		Pen.CreatePen(PS_SOLID, 1, RGB(0, 255, 0));
		pOldPen = dc.SelectObject(&Pen);
		dc.MoveTo(((CCSD_DRAWWIDTH - 2) / 255 * m_nLowerBar) + CCSD_EDGE_X + 2, CCSD_EDGE_Y - 2);
		dc.LineTo(((CCSD_DRAWWIDTH - 2) / 255 * m_nLowerBar) + CCSD_EDGE_X + 2, CCSD_EDGE_Y + CCSD_DRAWHEIGHT);
		dc.SelectObject(pOldPen);
		Pen.DeleteObject();
	}

	// Upper Bar Line
	if (m_nUpperBar > 0 && m_nUpperBar <= 255) {
		Pen.CreatePen(PS_SOLID, 1, RGB(255, 0, 0));
		pOldPen = dc.SelectObject(&Pen);
		dc.MoveTo(((CCSD_DRAWWIDTH - 2) / 255 * m_nUpperBar) + CCSD_EDGE_X + 2, CCSD_EDGE_Y - 2);
		dc.LineTo(((CCSD_DRAWWIDTH - 2) / 255 * m_nUpperBar) + CCSD_EDGE_X + 2, CCSD_EDGE_Y + CCSD_DRAWHEIGHT);
		dc.SelectObject(pOldPen);
		Pen.DeleteObject();
	}

	// Peak Bar Line
	if (m_nPeakThreshold >= 0 && m_nPeakThreshold <= 255) {
		Pen.CreatePen(PS_SOLID, 1, RGB(255, 255, 255));
		pOldPen = dc.SelectObject(&Pen);
		dc.MoveTo(((CCSD_DRAWWIDTH - 2) / 255 * m_nPeakThreshold) + CCSD_EDGE_X + 2, CCSD_EDGE_Y - 2);
		dc.LineTo(((CCSD_DRAWWIDTH - 2) / 255 * m_nPeakThreshold) + CCSD_EDGE_X + 2, CCSD_EDGE_Y + CCSD_DRAWHEIGHT);
		dc.SelectObject(pOldPen);
		Pen.DeleteObject();
	}
	
	// Do not call CDialog::OnPaint() for painting messages
}

void CThresholdHistogramDlg::OnDestroy() 
{
	CDialog::OnDestroy();
	gen_empty_obj(&m_HProcessingImage);
}

/////////////////////////////////////////////////////////////////////////////
// CThresholdHistogramDlg message handlers

void CThresholdHistogramDlg::UpdateEdit()
{
	if (m_nUpperBar < 0) m_nUpperBar = 0;
	if (m_nLowerBar > 255) m_nLowerBar = 255;
	if (m_nUpperBar < m_nLowerBar) m_nUpperBar = m_nLowerBar;
	
	m_nEditLower = m_nLowerBar;
	m_nEditUpper = m_nUpperBar;

	THEAPP.m_pInspectAdminViewDlg->ResetInspectionResult();

	THEAPP.m_pInspectAdminViewDlg->UpdateThresRegion(m_nEditLower,m_nEditUpper);

	UpdateData(FALSE);
}

void CThresholdHistogramDlg::UpdateBar(int nLowerBar, int nUpperBar)
{
	m_nLowerBar = nLowerBar;
	m_nUpperBar = nUpperBar;
	
	UpdateEdit();
	InvalidateRect(&m_DrawRect, TRUE);
}

void CThresholdHistogramDlg::Updata_ThresholdData()
{
	///////////////////////////////////////////////////////////////////
	// 히스토그램을 그리기위해 Data 준비

	if (m_lImageHeight < 0 || m_lImageWidth < 0) return;

	Hobject HDomain;
	get_domain(m_HProcessingImage, &HDomain);

	HTuple HAbsHisto, HRelHisto;
	gray_histo(HDomain, m_HProcessingImage, &HAbsHisto, &HRelHisto);

	m_nPeakHistoValue = 0;
	m_nPeakThreshold = -1;
	for (int i = 0; i < 256; i++) {
		m_iHistogram[i] = HAbsHisto[i];
		if (m_nPeakHistoValue < m_iHistogram[i]) {
			m_nPeakHistoValue = m_iHistogram[i];
			m_nPeakThreshold = i;
		}
	}
	m_bDrawBar = TRUE;

	UpdateEdit();
	InvalidateRect(&m_DrawRect, FALSE);
}
int CThresholdHistogramDlg::Get_PeakToPeak()
{
	if (m_lImageHeight < 0 || m_lImageWidth < 0) return 0;
	
	Hobject HDomain;
	get_domain(m_HProcessingImage, &HDomain);
	
	HTuple HAbsHisto, HRelHisto;
	gray_histo(HDomain, m_HProcessingImage, &HAbsHisto, &HRelHisto);
	

	HTuple Hmin,Hmax;
	histo_to_thresh(HAbsHisto,(HTuple)6,&Hmin,&Hmax);

	double dReturnValue = Hmax[0].D();

	return (int)dReturnValue;
}

void CThresholdHistogramDlg::SetImage(Hobject HImage)
{
	m_HProcessingImage = HImage;
	if (THEAPP.m_pGFunction->ValidHImage(m_HProcessingImage)) {

		BYTE *pImageData;
		char type[30];
		Hlong lImageWidth, lImageHeight;
		get_image_pointer1(m_HProcessingImage, (Hlong*)&pImageData, type, &m_lImageWidth, &m_lImageHeight);

		Updata_ThresholdData();
		Invalidate(TRUE);
	}
}

void CThresholdHistogramDlg::UpdateViewParam()
{
	UpdateData(TRUE);
	UpdateData(FALSE);
}

BOOL CThresholdHistogramDlg::PreTranslateMessage(MSG* pMsg) 
{
	if (pMsg->message == WM_KEYDOWN && (pMsg->wParam == VK_RETURN || pMsg->wParam == VK_ESCAPE)
		|| (pMsg->message == WM_SYSKEYDOWN && pMsg->wParam == VK_F4))
		return TRUE;
	
	return CDialog::PreTranslateMessage(pMsg);
}

void CThresholdHistogramDlg::OnChangeEditLower() 
{
	UpdateData(TRUE);

	if (m_nEditUpper < 0) m_nEditUpper = 0;
	if (m_nEditLower > 255) m_nEditLower = 255;
	if (m_nEditUpper < m_nEditLower) m_nEditUpper = m_nEditLower;

	UpdateBar(m_nEditLower, m_nEditUpper);
}

void CThresholdHistogramDlg::OnChangeEditUpper() 
{
	UpdateData(TRUE);

	if (m_nEditUpper < 0) m_nEditUpper = 0;
	if (m_nEditLower > 255) m_nEditLower = 255;
	if (m_nEditUpper < m_nEditLower) m_nEditUpper = m_nEditLower;

	UpdateBar(m_nEditLower, m_nEditUpper);
}

void CThresholdHistogramDlg::RedrawRect()
{
	InvalidateRect(&m_DrawRect, TRUE);
}


void CThresholdHistogramDlg::OnBnClickedButtonMakeRegionRoi()
{
	UpdateData(TRUE);

	THEAPP.m_pInspectAdminViewDlg->AddRegionROI(m_nEditLower, m_nEditUpper, 
		m_bCheckMinBlob, m_bCheckMaxArea, m_bCheckFillUp, m_bCheckOpeningClosingInverse, m_bCheckOpening, m_bCheckClosing, m_bCheckConvex, m_bCheckRect, m_bCheckDifference, m_bCheckBoundary, m_bCheckDilation,
		m_iEditMinBlobSize, m_iEditOpeningSize, m_iEditClosingSize, m_iEditBoundaryDilationSize, m_iEditDilationSize);
}


void CThresholdHistogramDlg::OnBnClickedButtonTestRegion()
{
	UpdateData(TRUE);

	THEAPP.m_pInspectAdminViewDlg->ShowRegionROI(m_nEditLower, m_nEditUpper, 
		m_bCheckMinBlob, m_bCheckMaxArea, m_bCheckFillUp, m_bCheckOpeningClosingInverse, m_bCheckOpening, m_bCheckClosing, m_bCheckConvex, m_bCheckRect, m_bCheckDifference, m_bCheckBoundary, m_bCheckDilation,
		m_iEditMinBlobSize, m_iEditOpeningSize, m_iEditClosingSize, m_iEditBoundaryDilationSize, m_iEditDilationSize);
}
