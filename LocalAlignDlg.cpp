// LocalAlignDlg.cpp : 구현 파일입니다.
// 24.02.27 Local Align 추가 - LeeGW Create

#include "stdafx.h"
#include "uScan.h"
#include "LocalAlignDlg.h"
#include "afxdialogex.h"


// CLocalAlignDlg 대화 상자입니다.
IMPLEMENT_DYNAMIC(CLocalAlignDlg, CDialog)

CLocalAlignDlg* CLocalAlignDlg::m_pInstance = NULL;

CLocalAlignDlg* CLocalAlignDlg::GetInstance(BOOL bShowFlag)
{
	if (m_pInstance == NULL)
	{
		m_pInstance = new CLocalAlignDlg;

		if(!m_pInstance->m_hWnd)
		{
			CMainFrame* pFrame = (CMainFrame*) AfxGetMainWnd();

			m_pInstance->Create(IDD_LOCAL_ALIGN_DLG, pFrame->GetActiveView());
		}
	}

	return m_pInstance;
}

void CLocalAlignDlg::DeleteInstance()
{
	if(m_pInstance->GetSafeHwnd())
		m_pInstance->DestroyWindow();

	SAFE_DELETE(m_pInstance);
}

void CLocalAlignDlg::Show()
{
	// 24.03.27 배럴, 렌즈 이미지마다 Shape 매칭으로 로컬 얼라인 제외 재추가 Start - LeeGW
	if (m_pSelectedROI == NULL)
		return;
	// End

	m_LocalAlignTabCtrl.SetCurSel(0);
	HideWindowAll();
	m_pLocalAlignApplyDlg->ShowWindow(SW_SHOW);
	
	SetWindowPos(&wndTopMost, 1000, 30, 0, 0, SWP_NOSIZE);
	ShowWindow(SW_SHOW);

	m_bShowDlg = TRUE;
}

void CLocalAlignDlg::Hide()
{
	ShowWindow(SW_HIDE);

	m_bShowDlg = FALSE;
}

CLocalAlignDlg::CLocalAlignDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CLocalAlignDlg::IDD, pParent)
{
	m_pLocalAlignApplyDlg = NULL;
	m_pLocalAlignPartCheckDlg = NULL;

	m_iSelectedImageType = 0;
	m_iInspectionType = 0;
	m_pSelectedROI = NULL;
}

CLocalAlignDlg::~CLocalAlignDlg()
{
	SAFE_DELETE(m_pLocalAlignApplyDlg)
	SAFE_DELETE(m_pLocalAlignPartCheckDlg)
	SAFE_DELETE(m_pLocalAlignEdgeMeasureDlg)
}

void CLocalAlignDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LOCAL_ALIGN_TAB, m_LocalAlignTabCtrl);
	DDX_Control(pDX, IDC_BUTTON_SAVE_AND_CLOSE, m_bnSaveClose);
	DDX_Control(pDX, IDC_BUTTON_CLOSE, m_bnClose);
}


BEGIN_MESSAGE_MAP(CLocalAlignDlg, CDialog)
	ON_NOTIFY(TCN_SELCHANGE, IDC_LOCAL_ALIGN_TAB, &CLocalAlignDlg::OnSelchangeAlgorithmTab)
	ON_BN_CLICKED(IDC_BUTTON_SAVE_AND_CLOSE, &CLocalAlignDlg::OnBnClickedButtonSaveAndClose)
	ON_BN_CLICKED(IDC_BUTTON_CLOSE, &CLocalAlignDlg::OnBnClickedButtonClose)
END_MESSAGE_MAP()


// CLocalAlignDlg 메시지 처리기입니다.


BOOL CLocalAlignDlg::PreTranslateMessage(MSG* pMsg)
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

BOOL CLocalAlignDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.
	SetWindowPos(&wndTopMost, 1000, 30, 0, 0, SWP_NOSIZE);

	m_LocalAlignTabCtrl.DeleteAllItems();

	for(long n=0; n < LOCAL_ALIGN_END ; n++)
		m_LocalAlignTabCtrl.InsertItem(n, g_strLocalAlignName[n]);
	CRect DlgRect;

	m_pLocalAlignApplyDlg = new CLocalAlignApplyDlg;
	m_pLocalAlignApplyDlg->Create(IDD_LOCAL_ALIGN_APPLY_DLG, &m_LocalAlignTabCtrl);
	m_pLocalAlignApplyDlg->GetWindowRect(&DlgRect);
	m_pLocalAlignApplyDlg->MoveWindow(5, 30, DlgRect.Width(), DlgRect.Height());
	m_pLocalAlignApplyDlg->ShowWindow(SW_SHOW);

	m_pLocalAlignPartCheckDlg = new CLocalAlignPartCheckDlg;
	m_pLocalAlignPartCheckDlg->Create(IDD_LOCAL_ALIGN_PART_CHECK_DLG, &m_LocalAlignTabCtrl);
	m_pLocalAlignPartCheckDlg->GetWindowRect(&DlgRect);
	m_pLocalAlignPartCheckDlg->MoveWindow(5, 30, DlgRect.Width(), DlgRect.Height());
	m_pLocalAlignPartCheckDlg->ShowWindow(SW_HIDE);

	m_pLocalAlignEdgeMeasureDlg = new CLocalAlignEdgeMeasureDlg;
	m_pLocalAlignEdgeMeasureDlg->Create(IDD_LOCAL_ALIGN_EDGE_MEASURE_DLG, &m_LocalAlignTabCtrl);
	m_pLocalAlignEdgeMeasureDlg->GetWindowRect(&DlgRect);
	m_pLocalAlignEdgeMeasureDlg->MoveWindow(5, 30, DlgRect.Width(), DlgRect.Height());
	m_pLocalAlignEdgeMeasureDlg->ShowWindow(SW_HIDE);

	m_bnSaveClose.SetInit(0, 0, 0, _T("저장 후 닫기"));
	m_bnClose.SetInit(0, 0, 0, _T("닫기"));

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}


void CLocalAlignDlg::OnSelchangeAlgorithmTab(NMHDR *pNMHDR, LRESULT *pResult)
{
	int iSelected = m_LocalAlignTabCtrl.GetCurSel();

	HideWindowAll();

	switch (iSelected)
	{

	case LOCAL_ALIGN_APPLY:
		m_pLocalAlignApplyDlg->ShowWindow(SW_SHOW);
		break;

	case LOCAL_ALIGN_PART_CHECK:
		m_pLocalAlignPartCheckDlg->ShowWindow(SW_SHOW);
		break;

	case LOCAL_ALIGN_EDGE_MEASURE:
		m_pLocalAlignEdgeMeasureDlg->ShowWindow(SW_SHOW);
		break;

	default:
		break;
	}

	*pResult = 0;
}


void CLocalAlignDlg::OnBnClickedButtonSaveAndClose()
{
	copy_image(m_HOrgScanImage, &(THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[m_iSelectedImageType]));

	gen_empty_obj(&(THEAPP.m_pInspectAdminViewDlg->m_HParameterDefectXLD));

	THEAPP.m_pInspectAdminViewDlg->UpdateView();

	GetParam();
	Hide();
}

void CLocalAlignDlg::OnBnClickedButtonClose()
{
	copy_image(m_HOrgScanImage, &(THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[m_iSelectedImageType]));

	gen_empty_obj(&(THEAPP.m_pInspectAdminViewDlg->m_HParameterDefectXLD));

	THEAPP.m_pInspectAdminViewDlg->UpdateView();

	Hide();
}

void CLocalAlignDlg::SetSelectedInspection(int iImageType, GTRegion *pSelectedROI, int iInspectionType)
{ 
	m_iSelectedImageType = iImageType;
	m_iInspectionType = iInspectionType;
	m_pSelectedROI = pSelectedROI;

	copy_image(THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[m_iSelectedImageType], &m_HOrgScanImage);

	m_pLocalAlignApplyDlg->SetSelectedInspection(m_iSelectedImageType, m_pSelectedROI, m_iInspectionType);
	m_pLocalAlignPartCheckDlg->SetSelectedInspection(m_iSelectedImageType, m_pSelectedROI, m_iInspectionType);
	m_pLocalAlignEdgeMeasureDlg->SetSelectedInspection(m_iSelectedImageType, m_pSelectedROI, m_iInspectionType);
}

void CLocalAlignDlg::SetParam()
{
	CAlgorithmParam LocalAlignParam;

	int i;
	switch (m_iSelectedImageType)
	{
		case 0:
			if (m_pSelectedROI != NULL)
				LocalAlignParam = m_pSelectedROI->m_AlgorithmParamSurface[m_iInspectionType];
			else
				LocalAlignParam = THEAPP.m_pModelDataManager->m_LocalAlignParamSurface;
			break;
		case 1:
			if (m_pSelectedROI != NULL)
				LocalAlignParam = m_pSelectedROI->m_AlgorithmParamEdge[m_iInspectionType];
			else
				LocalAlignParam = THEAPP.m_pModelDataManager->m_LocalAlignParamEdge;
			break;
		case 2:
			if (m_pSelectedROI != NULL)
				LocalAlignParam = m_pSelectedROI->m_AlgorithmParamLens1[m_iInspectionType];
			else
				LocalAlignParam = THEAPP.m_pModelDataManager->m_LocalAlignParamLens1;
			break;	
		case 3:
			if (m_pSelectedROI != NULL)
				LocalAlignParam = m_pSelectedROI->m_AlgorithmParamLens2[m_iInspectionType];
			else
				LocalAlignParam = THEAPP.m_pModelDataManager->m_LocalAlignParamLens2;
			break;
		case 4:
			LocalAlignParam = m_pSelectedROI->m_AlgorithmParamEpoxy1[m_iInspectionType];
			break;
		case 5:
			LocalAlignParam = m_pSelectedROI->m_AlgorithmParamEpoxy2[m_iInspectionType];
			break;
	}

	m_pLocalAlignApplyDlg->SetParam(LocalAlignParam);
	if (m_pSelectedROI != NULL)
	{
		m_pLocalAlignPartCheckDlg->SetParam(LocalAlignParam);
		m_pLocalAlignEdgeMeasureDlg->SetParam(LocalAlignParam);
	}
}

void CLocalAlignDlg::GetParam()
{
	CAlgorithmParam LocalAlignParam;

	int i;
	switch (m_iSelectedImageType)
	{
		case 0:
			if (m_pSelectedROI != NULL)
				LocalAlignParam = m_pSelectedROI->m_AlgorithmParamSurface[m_iInspectionType];
			else
				LocalAlignParam = THEAPP.m_pModelDataManager->m_LocalAlignParamSurface;
			break;
		case 1:
			if (m_pSelectedROI != NULL)
				LocalAlignParam = m_pSelectedROI->m_AlgorithmParamEdge[m_iInspectionType];
			else
				LocalAlignParam = THEAPP.m_pModelDataManager->m_LocalAlignParamEdge;
			break;
		case 2:
			if (m_pSelectedROI != NULL)
				LocalAlignParam = m_pSelectedROI->m_AlgorithmParamLens1[m_iInspectionType];
			else
				LocalAlignParam = THEAPP.m_pModelDataManager->m_LocalAlignParamLens1;
			break;	
		case 3:
			if (m_pSelectedROI != NULL)
				LocalAlignParam = m_pSelectedROI->m_AlgorithmParamLens2[m_iInspectionType];
			else
				LocalAlignParam = THEAPP.m_pModelDataManager->m_LocalAlignParamLens2;
			break;
		case 4:
			LocalAlignParam = m_pSelectedROI->m_AlgorithmParamEpoxy1[m_iInspectionType];
			break;
		case 5:
			LocalAlignParam = m_pSelectedROI->m_AlgorithmParamEpoxy2[m_iInspectionType];
			break;
	}

	m_pLocalAlignApplyDlg->GetParam(&LocalAlignParam);
	if (m_pSelectedROI != NULL)
	{
		m_pLocalAlignPartCheckDlg->GetParam(&LocalAlignParam);
		m_pLocalAlignEdgeMeasureDlg->GetParam(&LocalAlignParam);
	}
	
	switch (m_iSelectedImageType)
	{
		case 0:
			if (m_pSelectedROI != NULL)
				for (i=0; i<MAX_TEACHING_TAB; i++)
					m_pSelectedROI->m_AlgorithmParamSurface[i] = LocalAlignParam;
			else
				THEAPP.m_pModelDataManager->m_LocalAlignParamSurface = LocalAlignParam;
			break;
		case 1:
			if (m_pSelectedROI != NULL)
				for (i=0; i<MAX_TEACHING_TAB; i++)
					m_pSelectedROI->m_AlgorithmParamEdge[i] = LocalAlignParam;
			else
				THEAPP.m_pModelDataManager->m_LocalAlignParamEdge = LocalAlignParam;
			break;
		case 2:
			if (m_pSelectedROI != NULL)
				for (i=0; i<MAX_TEACHING_TAB; i++)
					m_pSelectedROI->m_AlgorithmParamLens1[i] = LocalAlignParam;
			else
				THEAPP.m_pModelDataManager->m_LocalAlignParamLens1 = LocalAlignParam;
			break;
		case 3:
			if (m_pSelectedROI != NULL)
				for (i=0; i<MAX_TEACHING_LENS2_TAB; i++)
					m_pSelectedROI->m_AlgorithmParamLens2[i] = LocalAlignParam;
			else
				THEAPP.m_pModelDataManager->m_LocalAlignParamLens2 = LocalAlignParam;
			break;
		case 4:
			for (i=0; i<MAX_ROI_TEACHING_TAB; i++)
				m_pSelectedROI->m_AlgorithmParamEpoxy1[i] = LocalAlignParam;
			break;
		case 5:
			for (i=0; i<MAX_ROI_TEACHING_TAB; i++)
				m_pSelectedROI->m_AlgorithmParamEpoxy2[i] = LocalAlignParam;
			break;
	}	


	THEAPP.m_pTabControlDlg->UpdateList(m_iSelectedImageType, m_iInspectionType);
}

void CLocalAlignDlg::HideWindowAll()
{
	m_pLocalAlignApplyDlg->ShowWindow(SW_HIDE);
	m_pLocalAlignPartCheckDlg->ShowWindow(SW_HIDE);
	m_pLocalAlignEdgeMeasureDlg->ShowWindow(SW_HIDE);
}