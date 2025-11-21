// ROIAlgorithmParamDlg.cpp : 구현 파일입니다.

#include "stdafx.h"
#include "uScan.h"
#include "ROIAlgorithmParamDlg.h"
#include "afxdialogex.h"


// CROIAlgorithmParamDlg 대화 상자입니다.
IMPLEMENT_DYNAMIC(CROIAlgorithmParamDlg, CDialog)

CROIAlgorithmParamDlg* CROIAlgorithmParamDlg::m_pInstance = NULL;

CROIAlgorithmParamDlg* CROIAlgorithmParamDlg::GetInstance(BOOL bShowFlag)
{
	if (m_pInstance == NULL)
	{
		m_pInstance = new CROIAlgorithmParamDlg;

		if(!m_pInstance->m_hWnd)
		{
			CMainFrame* pFrame = (CMainFrame*) AfxGetMainWnd();

			m_pInstance->Create(IDD_ROI_ALGORITHM_PARAM_DLG, pFrame->GetActiveView());
		}
	}

	return m_pInstance;
}

void CROIAlgorithmParamDlg::DeleteInstance()
{
	if(m_pInstance->GetSafeHwnd())
		m_pInstance->DestroyWindow();

	SAFE_DELETE(m_pInstance);
}

void CROIAlgorithmParamDlg::Show()
{
	if (m_pSelectedROI == NULL)
		return;
	// End

	m_ROIAlgorithmTabCtrl.SetCurSel(0);
	HideWindowAll();
	m_pROIPreprocessingDlg->ShowWindow(SW_SHOW);
	
	SetWindowPos(&wndTopMost, 1000, 30, 0, 0, SWP_NOSIZE);
	ShowWindow(SW_SHOW);

	m_bShowDlg = TRUE;
}

void CROIAlgorithmParamDlg::Hide()
{
	ShowWindow(SW_HIDE);

	m_bShowDlg = FALSE;
}

CROIAlgorithmParamDlg::CROIAlgorithmParamDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CROIAlgorithmParamDlg::IDD, pParent)
{
	m_pROIPreprocessingDlg = NULL;
	m_pROIPartCheckDlg = NULL;

	m_iSelectedImageType = 0;
	m_iInspectionType = 0;
	m_pSelectedROI = NULL;
}

CROIAlgorithmParamDlg::~CROIAlgorithmParamDlg()
{
	SAFE_DELETE(m_pROIPreprocessingDlg)
	SAFE_DELETE(m_pROIPartCheckDlg)
	SAFE_DELETE(m_pROIEdgeMeasureDlg)
	SAFE_DELETE(m_pROIInspAlgorithmDlg)
}

void CROIAlgorithmParamDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_ROI_ALGORITHM_TAB, m_ROIAlgorithmTabCtrl);
	DDX_Control(pDX, IDC_BUTTON_SAVE_AND_CLOSE, m_bnSaveClose);
	DDX_Control(pDX, IDC_BUTTON_CLOSE, m_bnClose);
}


BEGIN_MESSAGE_MAP(CROIAlgorithmParamDlg, CDialog)
	ON_NOTIFY(TCN_SELCHANGE, IDC_ROI_ALGORITHM_TAB, &CROIAlgorithmParamDlg::OnSelchangeAlgorithmTab)
	ON_BN_CLICKED(IDC_BUTTON_SAVE_AND_CLOSE, &CROIAlgorithmParamDlg::OnBnClickedButtonSaveAndClose)
	ON_BN_CLICKED(IDC_BUTTON_CLOSE, &CROIAlgorithmParamDlg::OnBnClickedButtonClose)
END_MESSAGE_MAP()


// CROIAlgorithmParamDlg 메시지 처리기입니다.


BOOL CROIAlgorithmParamDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.

	switch(pMsg->message) {		
	case WM_NCLBUTTONDOWN :
		SetActiveWindow();
		return FALSE;

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

BOOL CROIAlgorithmParamDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.
	SetWindowPos(&wndTopMost, 1000, 10, 0, 0, SWP_NOSIZE);

	m_ROIAlgorithmTabCtrl.DeleteAllItems();

	for(long n=0; n < ROI_ALGORITHM_END ; n++)
		m_ROIAlgorithmTabCtrl.InsertItem(n, g_sROIAlgorithmDlgName[n]);

	CRect DlgRect;

	m_pROIPreprocessingDlg = new CROIPreprocessingDlg;
	m_pROIPreprocessingDlg->Create(IDD_ROI_PREPROCESSING_DLG, &m_ROIAlgorithmTabCtrl);
	m_pROIPreprocessingDlg->GetWindowRect(&DlgRect);
	m_pROIPreprocessingDlg->MoveWindow(5, 30, DlgRect.Width(), DlgRect.Height());
	m_pROIPreprocessingDlg->ShowWindow(SW_SHOW);

	m_pROIPartCheckDlg = new CROIPartCheckDlg;
	m_pROIPartCheckDlg->Create(IDD_ROI_PART_CHECK_DLG, &m_ROIAlgorithmTabCtrl);
	m_pROIPartCheckDlg->GetWindowRect(&DlgRect);
	m_pROIPartCheckDlg->MoveWindow(5, 30, DlgRect.Width(), DlgRect.Height());
	m_pROIPartCheckDlg->ShowWindow(SW_HIDE);

	m_pROIEdgeMeasureDlg = new CROIEdgeMeasureDlg;
	m_pROIEdgeMeasureDlg->Create(IDD_ROI_EDGE_MEASURE_DLG, &m_ROIAlgorithmTabCtrl);
	m_pROIEdgeMeasureDlg->GetWindowRect(&DlgRect);
	m_pROIEdgeMeasureDlg->MoveWindow(5, 30, DlgRect.Width(), DlgRect.Height());
	m_pROIEdgeMeasureDlg->ShowWindow(SW_HIDE);

	m_pROIAnisoAlignDlg = new CROIAnisoAlignDlg;
	m_pROIAnisoAlignDlg->Create(IDD_ROI_ANISO_ALIGN_DLG, &m_ROIAlgorithmTabCtrl);
	m_pROIAnisoAlignDlg->GetWindowRect(&DlgRect);
	m_pROIAnisoAlignDlg->MoveWindow(5, 30, DlgRect.Width(), DlgRect.Height());
	m_pROIAnisoAlignDlg->ShowWindow(SW_HIDE);

	m_pROIInspAlgorithmDlg = new CROIAlgorithmCommonDlg;
	m_pROIInspAlgorithmDlg->Create(IDD_ROI_INSP_ALGORITHM_DLG, &m_ROIAlgorithmTabCtrl);
	m_pROIInspAlgorithmDlg->GetWindowRect(&DlgRect);
	m_pROIInspAlgorithmDlg->MoveWindow(5, 30, DlgRect.Width(), DlgRect.Height());
	m_pROIInspAlgorithmDlg->ShowWindow(SW_HIDE);

	m_bnSaveClose.Init_Ctrl(_T("Arial"), 9, FALSE, BLACK, ROYAL_BLUE, 0, 0);
	m_bnClose.Init_Ctrl(_T("Arial"), 9, FALSE, BLACK, ROYAL_BLUE, 0, 0);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}


void CROIAlgorithmParamDlg::OnSelchangeAlgorithmTab(NMHDR *pNMHDR, LRESULT *pResult)
{
	int iSelected = m_ROIAlgorithmTabCtrl.GetCurSel();

	HideWindowAll();

	switch (iSelected)
	{

	case ROI_PREPROCESSING:
		m_pROIPreprocessingDlg->ShowWindow(SW_SHOW);
		break;

	case ROI_PART_CHECK:
		m_pROIPartCheckDlg->ShowWindow(SW_SHOW);
		break;

	case ROI_EDGE_MEASURE:
		m_pROIEdgeMeasureDlg->ShowWindow(SW_SHOW);
		break;

	case ROI_ANISO_ALIGN:
		m_pROIAnisoAlignDlg->ShowWindow(SW_SHOW);
		break;

	case ROI_INSP_ALGORITHM:
		m_pROIInspAlgorithmDlg->ShowWindow(SW_SHOW);
		break;

	default:
		break;
	}

	*pResult = 0;
}


void CROIAlgorithmParamDlg::OnBnClickedButtonSaveAndClose()
{
	copy_image(m_HOrgScanImage, &(THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[m_iSelectedImageType]));

	gen_empty_obj(&(THEAPP.m_pInspectAdminViewDlg->m_HParameterDefectXLD));

	THEAPP.m_pInspectAdminViewDlg->UpdateView();

	GetParam();
	Hide();
}

void CROIAlgorithmParamDlg::OnBnClickedButtonClose()
{
	copy_image(m_HOrgScanImage, &(THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[m_iSelectedImageType]));

	gen_empty_obj(&(THEAPP.m_pInspectAdminViewDlg->m_HParameterDefectXLD));

	THEAPP.m_pInspectAdminViewDlg->UpdateView();

	Hide();
}

void CROIAlgorithmParamDlg::SetSelectedInspection(int iImageType, GTRegion *pSelectedROI, int iInspectionType)
{ 
	m_iSelectedImageType = iImageType;
	m_iInspectionType = iInspectionType;
	m_pSelectedROI = pSelectedROI;

	copy_image(THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[m_iSelectedImageType], &m_HOrgScanImage);

	m_pROIPreprocessingDlg->SetSelectedInspection(m_iSelectedImageType, m_pSelectedROI, m_iInspectionType);
	m_pROIPartCheckDlg->SetSelectedInspection(m_iSelectedImageType, m_pSelectedROI, m_iInspectionType);
	m_pROIEdgeMeasureDlg->SetSelectedInspection(m_iSelectedImageType, m_pSelectedROI, m_iInspectionType);
	m_pROIAnisoAlignDlg->SetSelectedInspection(m_iSelectedImageType, m_pSelectedROI, m_iInspectionType);
	m_pROIInspAlgorithmDlg->SetSelectedInspection(m_iSelectedImageType, m_pSelectedROI, m_iInspectionType);
}

void CROIAlgorithmParamDlg::SetParam()
{
	if (m_pSelectedROI == NULL)
		return;

	CAlgorithmParam ROIAlgorithmParam = m_pSelectedROI->m_AlgorithmParam[m_iInspectionType];

	m_pROIPreprocessingDlg->SetParam(ROIAlgorithmParam);
	m_pROIPartCheckDlg->SetParam(ROIAlgorithmParam);
	m_pROIEdgeMeasureDlg->SetParam(ROIAlgorithmParam);
	m_pROIAnisoAlignDlg->SetParam(ROIAlgorithmParam);
	m_pROIInspAlgorithmDlg->SetParam(ROIAlgorithmParam);
}

void CROIAlgorithmParamDlg::GetParam()
{
	if (m_pSelectedROI == NULL)
		return;

	CAlgorithmParam ROIAlgorithmParam = m_pSelectedROI->m_AlgorithmParam[m_iInspectionType];

	m_pROIPreprocessingDlg->GetParam(&ROIAlgorithmParam);
	m_pROIPartCheckDlg->GetParam(&ROIAlgorithmParam);
	m_pROIEdgeMeasureDlg->GetParam(&ROIAlgorithmParam);
	m_pROIAnisoAlignDlg->GetParam(&ROIAlgorithmParam);
	m_pROIInspAlgorithmDlg->GetParam(&ROIAlgorithmParam);
	
	m_pSelectedROI->m_AlgorithmParam[m_iInspectionType] = ROIAlgorithmParam;

	THEAPP.m_pTabControlDlg->UpdateList(m_iSelectedImageType, m_iInspectionType);
}

void CROIAlgorithmParamDlg::HideWindowAll()
{
	m_pROIPreprocessingDlg->ShowWindow(SW_HIDE);
	m_pROIPartCheckDlg->ShowWindow(SW_HIDE);
	m_pROIEdgeMeasureDlg->ShowWindow(SW_HIDE);
	m_pROIAnisoAlignDlg->ShowWindow(SW_HIDE);
	m_pROIInspAlgorithmDlg->ShowWindow(SW_HIDE);
}