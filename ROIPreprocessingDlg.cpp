// ROIPreprocessingDlg.cpp : 구현 파일입니다.
// 24.02.18 Local Align 추가 - LeeGW Create

#include "stdafx.h"
#include "uScan.h"
#include "ROIPreprocessingDlg.h"
#include "afxdialogex.h"


// CROIPreprocessingDlg 대화 상자입니다.

IMPLEMENT_DYNAMIC(CROIPreprocessingDlg, CDialog)

CROIPreprocessingDlg::CROIPreprocessingDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CROIPreprocessingDlg::IDD, pParent)
{

	m_iSelectedImageType = 0;
	m_iInspectionType = 0;
	m_pSelectedROI = NULL;

	m_bCheckUseLocalAlign = FALSE;
	m_iEditLocalAlignROINo = 1;
	m_iRadioLocalAlignType = 0;
	m_iRadioMatchingApplyMethod = 0;
	m_bCheckLocalAlignPosX = TRUE;
	m_bCheckLocalAlignPosY = TRUE;
	m_bCheckLocalAlignPosAngle = TRUE;
	m_iRadioLocalAlignFitPos = 0;

	m_bCheckUseImageProcessFilter = FALSE;
	m_iEditImageProcessFilterType1X = 0;
	m_iEditImageProcessFilterType2X = 0;
	m_iEditImageProcessFilterType3X = 0;
	m_iEditImageProcessFilterType1Y = 0;
	m_iEditImageProcessFilterType2Y = 0;
	m_iEditImageProcessFilterType3Y = 0;
}

CROIPreprocessingDlg::~CROIPreprocessingDlg()
{

	for (int i=0; i<MAX_IMAGE_TAB; i++)
	{
		gen_empty_obj(&(m_HROIAlgorithmImage[i]));
	}
}

void CROIPreprocessingDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Check(pDX, IDC_CHECK_USE_LOCAL_ALIGN, m_bCheckUseLocalAlign);
	DDX_Text(pDX, IDC_EDIT_LOCAL_ALIGN_ROI, m_iEditLocalAlignROINo);
	DDX_Radio(pDX, IDC_RADIO_USE_PART_CHECK, m_iRadioLocalAlignType);
	DDX_Radio(pDX, IDC_RADIO_USE_MATCHING_SHAPE, m_iRadioMatchingApplyMethod);
	DDX_Check(pDX, IDC_CHECK_USE_DELTA_X, m_bCheckLocalAlignPosX);
	DDX_Check(pDX, IDC_CHECK_USE_DELTA_Y, m_bCheckLocalAlignPosY);
	DDX_Check(pDX, IDC_CHECK_USE_ANGLE, m_bCheckLocalAlignPosAngle);
	DDX_Radio(pDX, IDC_RADIO_LINE_FIT_TOP, m_iRadioLocalAlignFitPos);

	DDX_Check(pDX, IDC_CHECK_USE_IMAGE_PROCESS_FILTER, m_bCheckUseImageProcessFilter);
	DDX_Text(pDX, IDC_EDIT_IMAGE_PROCESS_FILTER_X_SIZE_1, m_iEditImageProcessFilterType1X);
	DDX_Text(pDX, IDC_EDIT_IMAGE_PROCESS_FILTER_X_SIZE_2, m_iEditImageProcessFilterType2X);
	DDX_Text(pDX, IDC_EDIT_IMAGE_PROCESS_FILTER_X_SIZE_3, m_iEditImageProcessFilterType3X);
	DDX_Text(pDX, IDC_EDIT_IMAGE_PROCESS_FILTER_Y_SIZE_1, m_iEditImageProcessFilterType1Y);
	DDX_Text(pDX, IDC_EDIT_IMAGE_PROCESS_FILTER_Y_SIZE_2, m_iEditImageProcessFilterType2Y);
	DDX_Text(pDX, IDC_EDIT_IMAGE_PROCESS_FILTER_Y_SIZE_3, m_iEditImageProcessFilterType3Y);

	DDX_Control(pDX, IDC_COMBO_IMAGE_PROCESS_FILTER_TYPE_1, m_cbImageProcessFilterType1);
	DDX_Control(pDX, IDC_COMBO_IMAGE_PROCESS_FILTER_TYPE_2, m_cbImageProcessFilterType2);
	DDX_Control(pDX, IDC_COMBO_IMAGE_PROCESS_FILTER_TYPE_3, m_cbImageProcessFilterType3);
	DDX_Control(pDX, IDC_COMBO_LOCAL_ALIGN_IMAGE, m_cbLocalAlignImageIndex);
}


BEGIN_MESSAGE_MAP(CROIPreprocessingDlg, CDialog)
	ON_BN_CLICKED(IDC_BUTTON_LOCAL_ALIGN_APPLY_TEST, &CROIPreprocessingDlg::OnBnClickedButtonLocalAlignApplyTest)
	ON_BN_CLICKED(IDC_RADIO_USE_PART_CHECK, &CROIPreprocessingDlg::OnBnClickedRadioUsePartCheck)
	ON_BN_CLICKED(IDC_RADIO_USE_EDGE_MEASURE, &CROIPreprocessingDlg::OnBnClickedRadioUseEdgeMeasure)
	ON_BN_CLICKED(IDC_RADIO_USE_MATCHING_SHAPE, &CROIPreprocessingDlg::OnBnClickedRadioUseMatchingShape)
	ON_BN_CLICKED(IDC_RADIO_USE_MATCHING_POS, &CROIPreprocessingDlg::OnBnClickedRadioUseMatchingPos)
	ON_BN_CLICKED(IDC_RADIO_USE_LINE_FIT, &CROIPreprocessingDlg::OnBnClickedRadioUseLineFit)
	ON_BN_CLICKED(IDC_BUTTON_DISPLAY_PROCESS_IMAGE, &CROIPreprocessingDlg::OnBnClickedButtonDisplayProcessImage)

END_MESSAGE_MAP()


// CROIPreprocessingDlg 메시지 처리기입니다.
void CROIPreprocessingDlg::SetSelectedInspection(int iImageType, GTRegion *pSelectedROI, int iInspectionType)
{ 

	m_iSelectedImageType = iImageType;
	m_iInspectionType = iInspectionType;
	m_pSelectedROI = pSelectedROI;

	for (int i=0; i<MAX_IMAGE_TAB; i++)
	{
		if (i >= THEAPP.m_iMaxInspImageNo)
			break;

		if (THEAPP.m_pGFunction->ValidHImage(THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[i]))
			copy_image(THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[i], &(m_HROIAlgorithmImage[i]));
	}

}


void CROIPreprocessingDlg::SetParam(CAlgorithmParam ROIAlgorithmParam)
{
	m_bCheckUseLocalAlign = ROIAlgorithmParam.m_bUseLocalAlign;
	m_cbLocalAlignImageIndex.SetCurSel(ROIAlgorithmParam.m_iLocalAlignImageIndex);
	m_iEditLocalAlignROINo = ROIAlgorithmParam.m_iLocalAlignROINo;
	m_iRadioLocalAlignType = ROIAlgorithmParam.m_iLocalAlignType;
	m_iRadioMatchingApplyMethod = ROIAlgorithmParam.m_iLocalAlignApplyMethod;
	m_bCheckLocalAlignPosX = ROIAlgorithmParam.m_bLocalAlignPosX;
	m_bCheckLocalAlignPosY = ROIAlgorithmParam.m_bLocalAlignPosY;
	m_bCheckLocalAlignPosAngle = ROIAlgorithmParam.m_bLocalAlignPosAngle;
	m_iRadioLocalAlignFitPos = ROIAlgorithmParam.m_iLocalAlignFitPos;
	
	m_bCheckUseImageProcessFilter = ROIAlgorithmParam.m_bUseImageProcessFilter;
	m_cbImageProcessFilterType1.SetCurSel(ROIAlgorithmParam.m_iImageProcessFilterType1);
	m_cbImageProcessFilterType2.SetCurSel(ROIAlgorithmParam.m_iImageProcessFilterType2);
	m_cbImageProcessFilterType3.SetCurSel(ROIAlgorithmParam.m_iImageProcessFilterType3);
	m_iEditImageProcessFilterType1X = ROIAlgorithmParam.m_iImageProcessFilterType1X;
	m_iEditImageProcessFilterType2X = ROIAlgorithmParam.m_iImageProcessFilterType2X;
	m_iEditImageProcessFilterType3X = ROIAlgorithmParam.m_iImageProcessFilterType3X;
	m_iEditImageProcessFilterType1Y = ROIAlgorithmParam.m_iImageProcessFilterType1Y;
	m_iEditImageProcessFilterType2Y = ROIAlgorithmParam.m_iImageProcessFilterType2Y;
	m_iEditImageProcessFilterType3Y = ROIAlgorithmParam.m_iImageProcessFilterType3Y;

	if (m_iRadioLocalAlignType == 0)
	{
		GetDlgItem(IDC_RADIO_USE_LINE_FIT)->EnableWindow(FALSE);
		GetDlgItem(IDC_RADIO_USE_MATCHING_SHAPE)->EnableWindow(TRUE);
		if (m_iRadioMatchingApplyMethod == 1)
		{
			GetDlgItem(IDC_CHECK_USE_DELTA_X)->EnableWindow(TRUE);
			GetDlgItem(IDC_CHECK_USE_DELTA_Y)->EnableWindow(TRUE);
			GetDlgItem(IDC_CHECK_USE_ANGLE)->EnableWindow(TRUE);
			GetDlgItem(IDC_RADIO_LINE_FIT_TOP)->EnableWindow(FALSE);
			GetDlgItem(IDC_RADIO_LINE_FIT_BOTTOM)->EnableWindow(FALSE);
			GetDlgItem(IDC_RADIO_LINE_FIT_LEFT)->EnableWindow(FALSE);
			GetDlgItem(IDC_RADIO_LINE_FIT_RIGHT)->EnableWindow(FALSE);
		}
		else if (m_iRadioMatchingApplyMethod == 2)
		{
			GetDlgItem(IDC_CHECK_USE_DELTA_X)->EnableWindow(FALSE);
			GetDlgItem(IDC_CHECK_USE_DELTA_Y)->EnableWindow(FALSE);
			GetDlgItem(IDC_CHECK_USE_ANGLE)->EnableWindow(FALSE);
			GetDlgItem(IDC_RADIO_LINE_FIT_TOP)->EnableWindow(TRUE);
			GetDlgItem(IDC_RADIO_LINE_FIT_BOTTOM)->EnableWindow(TRUE);
			GetDlgItem(IDC_RADIO_LINE_FIT_LEFT)->EnableWindow(TRUE);
			GetDlgItem(IDC_RADIO_LINE_FIT_RIGHT)->EnableWindow(TRUE);
		}
		else
		{
			GetDlgItem(IDC_CHECK_USE_DELTA_X)->EnableWindow(FALSE);
			GetDlgItem(IDC_CHECK_USE_DELTA_Y)->EnableWindow(FALSE);
			GetDlgItem(IDC_CHECK_USE_ANGLE)->EnableWindow(FALSE);
			GetDlgItem(IDC_RADIO_LINE_FIT_TOP)->EnableWindow(FALSE);
			GetDlgItem(IDC_RADIO_LINE_FIT_BOTTOM)->EnableWindow(FALSE);
			GetDlgItem(IDC_RADIO_LINE_FIT_LEFT)->EnableWindow(FALSE);
			GetDlgItem(IDC_RADIO_LINE_FIT_RIGHT)->EnableWindow(FALSE);
		}
	}
	else
	{

		GetDlgItem(IDC_RADIO_USE_LINE_FIT)->EnableWindow(TRUE);
		GetDlgItem(IDC_RADIO_USE_MATCHING_SHAPE)->EnableWindow(FALSE);
		if (m_iRadioMatchingApplyMethod == 1)
		{
			GetDlgItem(IDC_CHECK_USE_DELTA_X)->EnableWindow(TRUE);
			GetDlgItem(IDC_CHECK_USE_DELTA_Y)->EnableWindow(TRUE);
			GetDlgItem(IDC_CHECK_USE_ANGLE)->EnableWindow(TRUE);
			GetDlgItem(IDC_RADIO_LINE_FIT_TOP)->EnableWindow(FALSE);
			GetDlgItem(IDC_RADIO_LINE_FIT_BOTTOM)->EnableWindow(FALSE);
			GetDlgItem(IDC_RADIO_LINE_FIT_LEFT)->EnableWindow(FALSE);
			GetDlgItem(IDC_RADIO_LINE_FIT_RIGHT)->EnableWindow(FALSE);
		}
		else if (m_iRadioMatchingApplyMethod == 2)
		{
			GetDlgItem(IDC_CHECK_USE_DELTA_X)->EnableWindow(FALSE);
			GetDlgItem(IDC_CHECK_USE_DELTA_Y)->EnableWindow(FALSE);
			GetDlgItem(IDC_CHECK_USE_ANGLE)->EnableWindow(FALSE);
			GetDlgItem(IDC_RADIO_LINE_FIT_TOP)->EnableWindow(TRUE);
			GetDlgItem(IDC_RADIO_LINE_FIT_BOTTOM)->EnableWindow(TRUE);
			GetDlgItem(IDC_RADIO_LINE_FIT_LEFT)->EnableWindow(TRUE);
			GetDlgItem(IDC_RADIO_LINE_FIT_RIGHT)->EnableWindow(TRUE);
		}
		else
		{
			GetDlgItem(IDC_CHECK_USE_DELTA_X)->EnableWindow(FALSE);
			GetDlgItem(IDC_CHECK_USE_DELTA_Y)->EnableWindow(FALSE);
			GetDlgItem(IDC_CHECK_USE_ANGLE)->EnableWindow(FALSE);
			GetDlgItem(IDC_RADIO_LINE_FIT_TOP)->EnableWindow(FALSE);
			GetDlgItem(IDC_RADIO_LINE_FIT_BOTTOM)->EnableWindow(FALSE);
			GetDlgItem(IDC_RADIO_LINE_FIT_LEFT)->EnableWindow(FALSE);
			GetDlgItem(IDC_RADIO_LINE_FIT_RIGHT)->EnableWindow(FALSE);
		}
	}

	UpdateData(FALSE);
}

void CROIPreprocessingDlg::GetParam(CAlgorithmParam *pROIAlgorithmParam)
{
	UpdateData();

	pROIAlgorithmParam->m_bUseLocalAlign = m_bCheckUseLocalAlign;
	pROIAlgorithmParam->m_iLocalAlignImageIndex = m_cbLocalAlignImageIndex.GetCurSel();
	pROIAlgorithmParam->m_iLocalAlignROINo = m_iEditLocalAlignROINo;
	pROIAlgorithmParam->m_iLocalAlignType = m_iRadioLocalAlignType;
	pROIAlgorithmParam->m_iLocalAlignApplyMethod = m_iRadioMatchingApplyMethod;
	pROIAlgorithmParam->m_bLocalAlignPosX = m_bCheckLocalAlignPosX;
	pROIAlgorithmParam->m_bLocalAlignPosY = m_bCheckLocalAlignPosY;
	pROIAlgorithmParam->m_bLocalAlignPosAngle = m_bCheckLocalAlignPosAngle;
	pROIAlgorithmParam->m_iLocalAlignFitPos = m_iRadioLocalAlignFitPos;

	pROIAlgorithmParam->m_bUseImageProcessFilter = m_bCheckUseImageProcessFilter;
	pROIAlgorithmParam->m_iImageProcessFilterType1 = m_cbImageProcessFilterType1.GetCurSel();
	pROIAlgorithmParam->m_iImageProcessFilterType2 = m_cbImageProcessFilterType2.GetCurSel();
	pROIAlgorithmParam->m_iImageProcessFilterType3 = m_cbImageProcessFilterType3.GetCurSel();
	pROIAlgorithmParam->m_iImageProcessFilterType1X = m_iEditImageProcessFilterType1X;
	pROIAlgorithmParam->m_iImageProcessFilterType2X = m_iEditImageProcessFilterType2X;
	pROIAlgorithmParam->m_iImageProcessFilterType3X = m_iEditImageProcessFilterType3X;
	pROIAlgorithmParam->m_iImageProcessFilterType1Y = m_iEditImageProcessFilterType1Y;
	pROIAlgorithmParam->m_iImageProcessFilterType2Y = m_iEditImageProcessFilterType2Y;
	pROIAlgorithmParam->m_iImageProcessFilterType3Y = m_iEditImageProcessFilterType3Y;

	if (m_iRadioLocalAlignType == 0)
	{
		GetDlgItem(IDC_RADIO_USE_LINE_FIT)->EnableWindow(FALSE);
		GetDlgItem(IDC_RADIO_USE_MATCHING_SHAPE)->EnableWindow(TRUE);
		if (m_iRadioMatchingApplyMethod == 1)
		{
			GetDlgItem(IDC_CHECK_USE_DELTA_X)->EnableWindow(TRUE);
			GetDlgItem(IDC_CHECK_USE_DELTA_Y)->EnableWindow(TRUE);
			GetDlgItem(IDC_CHECK_USE_ANGLE)->EnableWindow(TRUE);
			GetDlgItem(IDC_RADIO_LINE_FIT_TOP)->EnableWindow(FALSE);
			GetDlgItem(IDC_RADIO_LINE_FIT_BOTTOM)->EnableWindow(FALSE);
			GetDlgItem(IDC_RADIO_LINE_FIT_LEFT)->EnableWindow(FALSE);
			GetDlgItem(IDC_RADIO_LINE_FIT_RIGHT)->EnableWindow(FALSE);
		}
		else if (m_iRadioMatchingApplyMethod == 2)
		{
			GetDlgItem(IDC_CHECK_USE_DELTA_X)->EnableWindow(FALSE);
			GetDlgItem(IDC_CHECK_USE_DELTA_Y)->EnableWindow(FALSE);
			GetDlgItem(IDC_CHECK_USE_ANGLE)->EnableWindow(FALSE);
			GetDlgItem(IDC_RADIO_LINE_FIT_TOP)->EnableWindow(TRUE);
			GetDlgItem(IDC_RADIO_LINE_FIT_BOTTOM)->EnableWindow(TRUE);
			GetDlgItem(IDC_RADIO_LINE_FIT_LEFT)->EnableWindow(TRUE);
			GetDlgItem(IDC_RADIO_LINE_FIT_RIGHT)->EnableWindow(TRUE);
		}
		else
		{
			GetDlgItem(IDC_CHECK_USE_DELTA_X)->EnableWindow(FALSE);
			GetDlgItem(IDC_CHECK_USE_DELTA_Y)->EnableWindow(FALSE);
			GetDlgItem(IDC_CHECK_USE_ANGLE)->EnableWindow(FALSE);
			GetDlgItem(IDC_RADIO_LINE_FIT_TOP)->EnableWindow(FALSE);
			GetDlgItem(IDC_RADIO_LINE_FIT_BOTTOM)->EnableWindow(FALSE);
			GetDlgItem(IDC_RADIO_LINE_FIT_LEFT)->EnableWindow(FALSE);
			GetDlgItem(IDC_RADIO_LINE_FIT_RIGHT)->EnableWindow(FALSE);
		}
	}
	else
	{

		GetDlgItem(IDC_RADIO_USE_LINE_FIT)->EnableWindow(TRUE);
		GetDlgItem(IDC_RADIO_USE_MATCHING_SHAPE)->EnableWindow(FALSE);
		if (m_iRadioMatchingApplyMethod == 1)
		{
			GetDlgItem(IDC_CHECK_USE_DELTA_X)->EnableWindow(TRUE);
			GetDlgItem(IDC_CHECK_USE_DELTA_Y)->EnableWindow(TRUE);
			GetDlgItem(IDC_CHECK_USE_ANGLE)->EnableWindow(TRUE);
			GetDlgItem(IDC_RADIO_LINE_FIT_TOP)->EnableWindow(FALSE);
			GetDlgItem(IDC_RADIO_LINE_FIT_BOTTOM)->EnableWindow(FALSE);
			GetDlgItem(IDC_RADIO_LINE_FIT_LEFT)->EnableWindow(FALSE);
			GetDlgItem(IDC_RADIO_LINE_FIT_RIGHT)->EnableWindow(FALSE);
		}
		else if (m_iRadioMatchingApplyMethod == 2)
		{
			GetDlgItem(IDC_CHECK_USE_DELTA_X)->EnableWindow(FALSE);
			GetDlgItem(IDC_CHECK_USE_DELTA_Y)->EnableWindow(FALSE);
			GetDlgItem(IDC_CHECK_USE_ANGLE)->EnableWindow(FALSE);
			GetDlgItem(IDC_RADIO_LINE_FIT_TOP)->EnableWindow(TRUE);
			GetDlgItem(IDC_RADIO_LINE_FIT_BOTTOM)->EnableWindow(TRUE);
			GetDlgItem(IDC_RADIO_LINE_FIT_LEFT)->EnableWindow(TRUE);
			GetDlgItem(IDC_RADIO_LINE_FIT_RIGHT)->EnableWindow(TRUE);
		}
		else
		{
			GetDlgItem(IDC_CHECK_USE_DELTA_X)->EnableWindow(FALSE);
			GetDlgItem(IDC_CHECK_USE_DELTA_Y)->EnableWindow(FALSE);
			GetDlgItem(IDC_CHECK_USE_ANGLE)->EnableWindow(FALSE);
			GetDlgItem(IDC_RADIO_LINE_FIT_TOP)->EnableWindow(FALSE);
			GetDlgItem(IDC_RADIO_LINE_FIT_BOTTOM)->EnableWindow(FALSE);
			GetDlgItem(IDC_RADIO_LINE_FIT_LEFT)->EnableWindow(FALSE);
			GetDlgItem(IDC_RADIO_LINE_FIT_RIGHT)->EnableWindow(FALSE);
		}
	}
}

BOOL CROIPreprocessingDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.
	CString sImgNo;
	for (int i = 0; i < MAX_IMAGE_TAB; i++)
	{
		if (i >= THEAPP.m_iMaxInspImageNo)
			break;

		sImgNo.Format("영상 %d", i + 1);
		m_cbLocalAlignImageIndex.AddString(sImgNo);
	}

	m_cbLocalAlignImageIndex.SetCurSel(0);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}


void CROIPreprocessingDlg::OnBnClickedButtonLocalAlignApplyTest()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if (m_pSelectedROI == NULL)
		return;

	UpdateData();

	BOOL bUseLocalAlign = FALSE;
	Hobject HROIHRegion;
	CAlgorithmParam AlgorithmParam;

	AlgorithmParam = m_pSelectedROI->m_AlgorithmParam[m_iInspectionType];

	AlgorithmParam.m_bUseLocalAlign = m_bCheckUseLocalAlign;
	AlgorithmParam.m_iLocalAlignImageIndex = m_cbLocalAlignImageIndex.GetCurSel();
	AlgorithmParam.m_iLocalAlignROINo = m_iEditLocalAlignROINo;
	AlgorithmParam.m_iLocalAlignType = m_iRadioLocalAlignType;
	AlgorithmParam.m_iLocalAlignApplyMethod = m_iRadioMatchingApplyMethod;
	AlgorithmParam.m_bLocalAlignPosX = m_bCheckLocalAlignPosX;
	AlgorithmParam.m_bLocalAlignPosY = m_bCheckLocalAlignPosY;
	AlgorithmParam.m_bLocalAlignPosAngle = m_bCheckLocalAlignPosAngle;
	AlgorithmParam.m_iLocalAlignFitPos = m_iRadioLocalAlignFitPos;


	// 24.03.18 Local Align - LeeGW
	if (!AlgorithmParam.m_bUseLocalAlign)
		return;

	int iNoInspectROI = 0;
	iNoInspectROI = THEAPP.m_pModelDataManager->m_pInspectionArea->GetChildTRegionCount();

	GTRegion* pInspectROIRgn;
	for (int iROIIndex = 0; iROIIndex < iNoInspectROI; iROIIndex++)
	{
		pInspectROIRgn = THEAPP.m_pModelDataManager->m_pInspectionArea->GetChildTRegion(iROIIndex);
		pInspectROIRgn->ResetLocalAlignResult(MAX_MODULE_ONE_TRAY - 1);
	}

	Hobject HResultXLD;
	gen_empty_obj(&HResultXLD);

	HROIHRegion = m_pSelectedROI->GetROIHRegion(THEAPP.m_pCalDataService);
	THEAPP.m_pAlgorithm->ApplyLocalAlignResult(&HROIHRegion, AlgorithmParam, MAX_MODULE_ONE_TRAY - 1);

	gen_contour_region_xld(HROIHRegion, &HResultXLD, "border");
	
	if (THEAPP.m_pGFunction->ValidHXLD(HResultXLD))
		concat_obj(THEAPP.m_pInspectAdminViewDlg->m_HParameterDefectXLD, HResultXLD, &(THEAPP.m_pInspectAdminViewDlg->m_HParameterDefectXLD));

	THEAPP.m_pInspectAdminViewDlg->UpdateView();
	// 24.03.18 Local Align - LeeGW
}


void CROIPreprocessingDlg::OnBnClickedRadioUsePartCheck()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	UpdateData();
	UpdateData(FALSE);
	if (m_iRadioLocalAlignType == 0)
	{
		GetDlgItem(IDC_RADIO_USE_LINE_FIT)->EnableWindow(FALSE);
		GetDlgItem(IDC_RADIO_USE_MATCHING_SHAPE)->EnableWindow(TRUE);
		if (m_iRadioMatchingApplyMethod == 1)
		{
			GetDlgItem(IDC_CHECK_USE_DELTA_X)->EnableWindow(TRUE);
			GetDlgItem(IDC_CHECK_USE_DELTA_Y)->EnableWindow(TRUE);
			GetDlgItem(IDC_CHECK_USE_ANGLE)->EnableWindow(TRUE);
			GetDlgItem(IDC_RADIO_LINE_FIT_TOP)->EnableWindow(FALSE);
			GetDlgItem(IDC_RADIO_LINE_FIT_BOTTOM)->EnableWindow(FALSE);
			GetDlgItem(IDC_RADIO_LINE_FIT_LEFT)->EnableWindow(FALSE);
			GetDlgItem(IDC_RADIO_LINE_FIT_RIGHT)->EnableWindow(FALSE);
		}
		else if (m_iRadioMatchingApplyMethod == 2)
		{
			GetDlgItem(IDC_CHECK_USE_DELTA_X)->EnableWindow(FALSE);
			GetDlgItem(IDC_CHECK_USE_DELTA_Y)->EnableWindow(FALSE);
			GetDlgItem(IDC_CHECK_USE_ANGLE)->EnableWindow(FALSE);
			GetDlgItem(IDC_RADIO_LINE_FIT_TOP)->EnableWindow(TRUE);
			GetDlgItem(IDC_RADIO_LINE_FIT_BOTTOM)->EnableWindow(TRUE);
			GetDlgItem(IDC_RADIO_LINE_FIT_LEFT)->EnableWindow(TRUE);
			GetDlgItem(IDC_RADIO_LINE_FIT_RIGHT)->EnableWindow(TRUE);
		}
		else
		{
			GetDlgItem(IDC_CHECK_USE_DELTA_X)->EnableWindow(FALSE);
			GetDlgItem(IDC_CHECK_USE_DELTA_Y)->EnableWindow(FALSE);
			GetDlgItem(IDC_CHECK_USE_ANGLE)->EnableWindow(FALSE);
			GetDlgItem(IDC_RADIO_LINE_FIT_TOP)->EnableWindow(FALSE);
			GetDlgItem(IDC_RADIO_LINE_FIT_BOTTOM)->EnableWindow(FALSE);
			GetDlgItem(IDC_RADIO_LINE_FIT_LEFT)->EnableWindow(FALSE);
			GetDlgItem(IDC_RADIO_LINE_FIT_RIGHT)->EnableWindow(FALSE);
		}
	}
	else
	{

		GetDlgItem(IDC_RADIO_USE_LINE_FIT)->EnableWindow(TRUE);
		GetDlgItem(IDC_RADIO_USE_MATCHING_SHAPE)->EnableWindow(FALSE);
		if (m_iRadioMatchingApplyMethod == 1)
		{
			GetDlgItem(IDC_CHECK_USE_DELTA_X)->EnableWindow(TRUE);
			GetDlgItem(IDC_CHECK_USE_DELTA_Y)->EnableWindow(TRUE);
			GetDlgItem(IDC_CHECK_USE_ANGLE)->EnableWindow(TRUE);
			GetDlgItem(IDC_RADIO_LINE_FIT_TOP)->EnableWindow(FALSE);
			GetDlgItem(IDC_RADIO_LINE_FIT_BOTTOM)->EnableWindow(FALSE);
			GetDlgItem(IDC_RADIO_LINE_FIT_LEFT)->EnableWindow(FALSE);
			GetDlgItem(IDC_RADIO_LINE_FIT_RIGHT)->EnableWindow(FALSE);
		}
		else if (m_iRadioMatchingApplyMethod == 2)
		{
			GetDlgItem(IDC_CHECK_USE_DELTA_X)->EnableWindow(FALSE);
			GetDlgItem(IDC_CHECK_USE_DELTA_Y)->EnableWindow(FALSE);
			GetDlgItem(IDC_CHECK_USE_ANGLE)->EnableWindow(FALSE);
			GetDlgItem(IDC_RADIO_LINE_FIT_TOP)->EnableWindow(TRUE);
			GetDlgItem(IDC_RADIO_LINE_FIT_BOTTOM)->EnableWindow(TRUE);
			GetDlgItem(IDC_RADIO_LINE_FIT_LEFT)->EnableWindow(TRUE);
			GetDlgItem(IDC_RADIO_LINE_FIT_RIGHT)->EnableWindow(TRUE);
		}
		else
		{
			GetDlgItem(IDC_CHECK_USE_DELTA_X)->EnableWindow(FALSE);
			GetDlgItem(IDC_CHECK_USE_DELTA_Y)->EnableWindow(FALSE);
			GetDlgItem(IDC_CHECK_USE_ANGLE)->EnableWindow(FALSE);
			GetDlgItem(IDC_RADIO_LINE_FIT_TOP)->EnableWindow(FALSE);
			GetDlgItem(IDC_RADIO_LINE_FIT_BOTTOM)->EnableWindow(FALSE);
			GetDlgItem(IDC_RADIO_LINE_FIT_LEFT)->EnableWindow(FALSE);
			GetDlgItem(IDC_RADIO_LINE_FIT_RIGHT)->EnableWindow(FALSE);
		}
	}
}


void CROIPreprocessingDlg::OnBnClickedRadioUseEdgeMeasure()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	UpdateData();
	UpdateData(FALSE);
	if (m_iRadioLocalAlignType == 0)
	{
		GetDlgItem(IDC_RADIO_USE_LINE_FIT)->EnableWindow(FALSE);
		GetDlgItem(IDC_RADIO_USE_MATCHING_SHAPE)->EnableWindow(TRUE);
		if (m_iRadioMatchingApplyMethod == 1)
		{
			GetDlgItem(IDC_CHECK_USE_DELTA_X)->EnableWindow(TRUE);
			GetDlgItem(IDC_CHECK_USE_DELTA_Y)->EnableWindow(TRUE);
			GetDlgItem(IDC_CHECK_USE_ANGLE)->EnableWindow(TRUE);
			GetDlgItem(IDC_RADIO_LINE_FIT_TOP)->EnableWindow(FALSE);
			GetDlgItem(IDC_RADIO_LINE_FIT_BOTTOM)->EnableWindow(FALSE);
			GetDlgItem(IDC_RADIO_LINE_FIT_LEFT)->EnableWindow(FALSE);
			GetDlgItem(IDC_RADIO_LINE_FIT_RIGHT)->EnableWindow(FALSE);
		}
		else if (m_iRadioMatchingApplyMethod == 2)
		{
			GetDlgItem(IDC_CHECK_USE_DELTA_X)->EnableWindow(FALSE);
			GetDlgItem(IDC_CHECK_USE_DELTA_Y)->EnableWindow(FALSE);
			GetDlgItem(IDC_CHECK_USE_ANGLE)->EnableWindow(FALSE);
			GetDlgItem(IDC_RADIO_LINE_FIT_TOP)->EnableWindow(TRUE);
			GetDlgItem(IDC_RADIO_LINE_FIT_BOTTOM)->EnableWindow(TRUE);
			GetDlgItem(IDC_RADIO_LINE_FIT_LEFT)->EnableWindow(TRUE);
			GetDlgItem(IDC_RADIO_LINE_FIT_RIGHT)->EnableWindow(TRUE);
		}
		else
		{
			GetDlgItem(IDC_CHECK_USE_DELTA_X)->EnableWindow(FALSE);
			GetDlgItem(IDC_CHECK_USE_DELTA_Y)->EnableWindow(FALSE);
			GetDlgItem(IDC_CHECK_USE_ANGLE)->EnableWindow(FALSE);
			GetDlgItem(IDC_RADIO_LINE_FIT_TOP)->EnableWindow(FALSE);
			GetDlgItem(IDC_RADIO_LINE_FIT_BOTTOM)->EnableWindow(FALSE);
			GetDlgItem(IDC_RADIO_LINE_FIT_LEFT)->EnableWindow(FALSE);
			GetDlgItem(IDC_RADIO_LINE_FIT_RIGHT)->EnableWindow(FALSE);
		}
	}
	else
	{

		GetDlgItem(IDC_RADIO_USE_LINE_FIT)->EnableWindow(TRUE);
		GetDlgItem(IDC_RADIO_USE_MATCHING_SHAPE)->EnableWindow(FALSE);
		if (m_iRadioMatchingApplyMethod == 1)
		{
			GetDlgItem(IDC_CHECK_USE_DELTA_X)->EnableWindow(TRUE);
			GetDlgItem(IDC_CHECK_USE_DELTA_Y)->EnableWindow(TRUE);
			GetDlgItem(IDC_CHECK_USE_ANGLE)->EnableWindow(TRUE);
			GetDlgItem(IDC_RADIO_LINE_FIT_TOP)->EnableWindow(FALSE);
			GetDlgItem(IDC_RADIO_LINE_FIT_BOTTOM)->EnableWindow(FALSE);
			GetDlgItem(IDC_RADIO_LINE_FIT_LEFT)->EnableWindow(FALSE);
			GetDlgItem(IDC_RADIO_LINE_FIT_RIGHT)->EnableWindow(FALSE);
		}
		else if (m_iRadioMatchingApplyMethod == 2)
		{
			GetDlgItem(IDC_CHECK_USE_DELTA_X)->EnableWindow(FALSE);
			GetDlgItem(IDC_CHECK_USE_DELTA_Y)->EnableWindow(FALSE);
			GetDlgItem(IDC_CHECK_USE_ANGLE)->EnableWindow(FALSE);
			GetDlgItem(IDC_RADIO_LINE_FIT_TOP)->EnableWindow(TRUE);
			GetDlgItem(IDC_RADIO_LINE_FIT_BOTTOM)->EnableWindow(TRUE);
			GetDlgItem(IDC_RADIO_LINE_FIT_LEFT)->EnableWindow(TRUE);
			GetDlgItem(IDC_RADIO_LINE_FIT_RIGHT)->EnableWindow(TRUE);
		}
		else
		{
			GetDlgItem(IDC_CHECK_USE_DELTA_X)->EnableWindow(FALSE);
			GetDlgItem(IDC_CHECK_USE_DELTA_Y)->EnableWindow(FALSE);
			GetDlgItem(IDC_CHECK_USE_ANGLE)->EnableWindow(FALSE);
			GetDlgItem(IDC_RADIO_LINE_FIT_TOP)->EnableWindow(FALSE);
			GetDlgItem(IDC_RADIO_LINE_FIT_BOTTOM)->EnableWindow(FALSE);
			GetDlgItem(IDC_RADIO_LINE_FIT_LEFT)->EnableWindow(FALSE);
			GetDlgItem(IDC_RADIO_LINE_FIT_RIGHT)->EnableWindow(FALSE);
		}
	}
}


void CROIPreprocessingDlg::OnBnClickedRadioUseMatchingShape()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	UpdateData();
	UpdateData(FALSE);
	if (m_iRadioMatchingApplyMethod == 1)
	{
		GetDlgItem(IDC_CHECK_USE_DELTA_X)->EnableWindow(TRUE);
		GetDlgItem(IDC_CHECK_USE_DELTA_Y)->EnableWindow(TRUE);
		GetDlgItem(IDC_CHECK_USE_ANGLE)->EnableWindow(TRUE);
		GetDlgItem(IDC_RADIO_LINE_FIT_TOP)->EnableWindow(FALSE);
		GetDlgItem(IDC_RADIO_LINE_FIT_BOTTOM)->EnableWindow(FALSE);
		GetDlgItem(IDC_RADIO_LINE_FIT_LEFT)->EnableWindow(FALSE);
		GetDlgItem(IDC_RADIO_LINE_FIT_RIGHT)->EnableWindow(FALSE);
	}
	else if (m_iRadioMatchingApplyMethod == 2)
	{
		GetDlgItem(IDC_CHECK_USE_DELTA_X)->EnableWindow(FALSE);
		GetDlgItem(IDC_CHECK_USE_DELTA_Y)->EnableWindow(FALSE);
		GetDlgItem(IDC_CHECK_USE_ANGLE)->EnableWindow(FALSE);
		GetDlgItem(IDC_RADIO_LINE_FIT_TOP)->EnableWindow(TRUE);
		GetDlgItem(IDC_RADIO_LINE_FIT_BOTTOM)->EnableWindow(TRUE);
		GetDlgItem(IDC_RADIO_LINE_FIT_LEFT)->EnableWindow(TRUE);
		GetDlgItem(IDC_RADIO_LINE_FIT_RIGHT)->EnableWindow(TRUE);
	}
	else
	{
		GetDlgItem(IDC_CHECK_USE_DELTA_X)->EnableWindow(FALSE);
		GetDlgItem(IDC_CHECK_USE_DELTA_Y)->EnableWindow(FALSE);
		GetDlgItem(IDC_CHECK_USE_ANGLE)->EnableWindow(FALSE);
		GetDlgItem(IDC_RADIO_LINE_FIT_TOP)->EnableWindow(FALSE);
		GetDlgItem(IDC_RADIO_LINE_FIT_BOTTOM)->EnableWindow(FALSE);
		GetDlgItem(IDC_RADIO_LINE_FIT_LEFT)->EnableWindow(FALSE);
		GetDlgItem(IDC_RADIO_LINE_FIT_RIGHT)->EnableWindow(FALSE);
	}
}


void CROIPreprocessingDlg::OnBnClickedRadioUseMatchingPos()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	UpdateData();
	UpdateData(FALSE);
	if (m_iRadioMatchingApplyMethod == 1)
	{
		GetDlgItem(IDC_CHECK_USE_DELTA_X)->EnableWindow(TRUE);
		GetDlgItem(IDC_CHECK_USE_DELTA_Y)->EnableWindow(TRUE);
		GetDlgItem(IDC_CHECK_USE_ANGLE)->EnableWindow(TRUE);
		GetDlgItem(IDC_RADIO_LINE_FIT_TOP)->EnableWindow(FALSE);
		GetDlgItem(IDC_RADIO_LINE_FIT_BOTTOM)->EnableWindow(FALSE);
		GetDlgItem(IDC_RADIO_LINE_FIT_LEFT)->EnableWindow(FALSE);
		GetDlgItem(IDC_RADIO_LINE_FIT_RIGHT)->EnableWindow(FALSE);
	}
	else if (m_iRadioMatchingApplyMethod == 2)
	{
		GetDlgItem(IDC_CHECK_USE_DELTA_X)->EnableWindow(FALSE);
		GetDlgItem(IDC_CHECK_USE_DELTA_Y)->EnableWindow(FALSE);
		GetDlgItem(IDC_CHECK_USE_ANGLE)->EnableWindow(FALSE);
		GetDlgItem(IDC_RADIO_LINE_FIT_TOP)->EnableWindow(TRUE);
		GetDlgItem(IDC_RADIO_LINE_FIT_BOTTOM)->EnableWindow(TRUE);
		GetDlgItem(IDC_RADIO_LINE_FIT_LEFT)->EnableWindow(TRUE);
		GetDlgItem(IDC_RADIO_LINE_FIT_RIGHT)->EnableWindow(TRUE);
	}
	else
	{
		GetDlgItem(IDC_CHECK_USE_DELTA_X)->EnableWindow(FALSE);
		GetDlgItem(IDC_CHECK_USE_DELTA_Y)->EnableWindow(FALSE);
		GetDlgItem(IDC_CHECK_USE_ANGLE)->EnableWindow(FALSE);
		GetDlgItem(IDC_RADIO_LINE_FIT_TOP)->EnableWindow(FALSE);
		GetDlgItem(IDC_RADIO_LINE_FIT_BOTTOM)->EnableWindow(FALSE);
		GetDlgItem(IDC_RADIO_LINE_FIT_LEFT)->EnableWindow(FALSE);
		GetDlgItem(IDC_RADIO_LINE_FIT_RIGHT)->EnableWindow(FALSE);
	}
}


void CROIPreprocessingDlg::OnBnClickedRadioUseLineFit()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	UpdateData();
	UpdateData(FALSE);
	if (m_iRadioMatchingApplyMethod == 1)
	{
		GetDlgItem(IDC_CHECK_USE_DELTA_X)->EnableWindow(TRUE);
		GetDlgItem(IDC_CHECK_USE_DELTA_Y)->EnableWindow(TRUE);
		GetDlgItem(IDC_CHECK_USE_ANGLE)->EnableWindow(TRUE);
		GetDlgItem(IDC_RADIO_LINE_FIT_TOP)->EnableWindow(FALSE);
		GetDlgItem(IDC_RADIO_LINE_FIT_BOTTOM)->EnableWindow(FALSE);
		GetDlgItem(IDC_RADIO_LINE_FIT_LEFT)->EnableWindow(FALSE);
		GetDlgItem(IDC_RADIO_LINE_FIT_RIGHT)->EnableWindow(FALSE);
	}
	else if (m_iRadioMatchingApplyMethod == 2)
	{
		GetDlgItem(IDC_CHECK_USE_DELTA_X)->EnableWindow(FALSE);
		GetDlgItem(IDC_CHECK_USE_DELTA_Y)->EnableWindow(FALSE);
		GetDlgItem(IDC_CHECK_USE_ANGLE)->EnableWindow(FALSE);
		GetDlgItem(IDC_RADIO_LINE_FIT_TOP)->EnableWindow(TRUE);
		GetDlgItem(IDC_RADIO_LINE_FIT_BOTTOM)->EnableWindow(TRUE);
		GetDlgItem(IDC_RADIO_LINE_FIT_LEFT)->EnableWindow(TRUE);
		GetDlgItem(IDC_RADIO_LINE_FIT_RIGHT)->EnableWindow(TRUE);
	}
	else
	{
		GetDlgItem(IDC_CHECK_USE_DELTA_X)->EnableWindow(FALSE);
		GetDlgItem(IDC_CHECK_USE_DELTA_Y)->EnableWindow(FALSE);
		GetDlgItem(IDC_CHECK_USE_ANGLE)->EnableWindow(FALSE);
		GetDlgItem(IDC_RADIO_LINE_FIT_TOP)->EnableWindow(FALSE);
		GetDlgItem(IDC_RADIO_LINE_FIT_BOTTOM)->EnableWindow(FALSE);
		GetDlgItem(IDC_RADIO_LINE_FIT_LEFT)->EnableWindow(FALSE);
		GetDlgItem(IDC_RADIO_LINE_FIT_RIGHT)->EnableWindow(FALSE);
	}
}

void CROIPreprocessingDlg::OnBnClickedButtonDisplayProcessImage()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CAlgorithmParam AlgorithmParam;

	AlgorithmParam = m_pSelectedROI->m_AlgorithmParam[m_iInspectionType];

	UpdateData(TRUE);

	AlgorithmParam.m_bUseImageProcessFilter = m_bCheckUseImageProcessFilter;
	AlgorithmParam.m_iImageProcessFilterType1 = m_cbImageProcessFilterType1.GetCurSel();
	AlgorithmParam.m_iImageProcessFilterType2 = m_cbImageProcessFilterType2.GetCurSel();
	AlgorithmParam.m_iImageProcessFilterType3 = m_cbImageProcessFilterType3.GetCurSel();
	AlgorithmParam.m_iImageProcessFilterType1X = m_iEditImageProcessFilterType1X;
	AlgorithmParam.m_iImageProcessFilterType2X = m_iEditImageProcessFilterType2X;
	AlgorithmParam.m_iImageProcessFilterType3X = m_iEditImageProcessFilterType3X;
	AlgorithmParam.m_iImageProcessFilterType1Y = m_iEditImageProcessFilterType1Y;
	AlgorithmParam.m_iImageProcessFilterType2Y = m_iEditImageProcessFilterType2Y;
	AlgorithmParam.m_iImageProcessFilterType3Y = m_iEditImageProcessFilterType3Y;

	BOOL bRet;
	Hobject HProcessingImage;

	Hobject HROIHRegion;
	HROIHRegion = m_pSelectedROI->GetROIHRegion(THEAPP.m_pCalDataService);

	for (int i = 0; i < MAX_IMAGE_TAB; i++)
	{
		if (THEAPP.m_pGFunction->ValidHImage(m_HROIAlgorithmImage[i]))
			copy_image(m_HROIAlgorithmImage[i], &(THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[i]));
	}

	bRet = THEAPP.m_pAlgorithm->GetPreprocessImage(TRUE, m_HROIAlgorithmImage[m_iSelectedImageType], HROIHRegion, AlgorithmParam, &HProcessingImage);

	if (bRet)
	{
		if (THEAPP.m_pGFunction->ValidHImage(HProcessingImage))
		{

			copy_image(HProcessingImage, &THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[m_iSelectedImageType]);
		}
	}

	THEAPP.m_pInspectAdminViewDlg->ClearUpdateView();
}
