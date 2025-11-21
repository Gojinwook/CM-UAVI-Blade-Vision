// LocalAlignApplyDlg.cpp : 구현 파일입니다.
// 24.02.18 Local Align 추가 - LeeGW Create

#include "stdafx.h"
#include "uScan.h"
#include "LocalAlignApplyDlg.h"
#include "afxdialogex.h"


// CLocalAlignApplyDlg 대화 상자입니다.

IMPLEMENT_DYNAMIC(CLocalAlignApplyDlg, CDialog)

CLocalAlignApplyDlg::CLocalAlignApplyDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CLocalAlignApplyDlg::IDD, pParent)
{

	m_iSelectedImageType = 0;
	m_iInspectionType = 0;
	m_pSelectedROI = NULL;

	m_bCheckUseLocalAlign = FALSE;
	m_iEditLocalAlignROINo = 1;
	m_bCheckLocalAlignPosX = TRUE;
	m_bCheckLocalAlignPosY = TRUE;
	m_bCheckLocalAlignPosAngle = TRUE;
	m_iRadioLocalAlignFitPos = 0;
	m_iRadioLocalAlignType = 0;
	m_iRadioMatchingApplyMethod = 0;
	m_iRadioLocalAlignFitPos = 0;
	
}

CLocalAlignApplyDlg::~CLocalAlignApplyDlg()
{

	for (int i=0; i<MAX_IMAGE_TAB; i++)
	{
		gen_empty_obj(&(m_HLocalAlignImage[i]));
	}
}

void CLocalAlignApplyDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Check(pDX, IDC_CHECK_USE_LOCAL_ALIGN, m_bCheckUseLocalAlign);
	DDX_Control(pDX, IDC_COMBO_LOCAL_ALIGN_IMAGE, m_cbLocalAlignImageIndex);
	DDX_Text(pDX, IDC_EDIT_LOCAL_ALIGN_ROI, m_iEditLocalAlignROINo);
	DDX_Radio(pDX, IDC_RADIO_USE_PART_CHECK, m_iRadioLocalAlignType);
	DDX_Radio(pDX, IDC_RADIO_USE_MATCHING_SHAPE, m_iRadioMatchingApplyMethod);
	DDX_Check(pDX, IDC_CHECK_USE_DELTA_X, m_bCheckLocalAlignPosX);
	DDX_Check(pDX, IDC_CHECK_USE_DELTA_Y, m_bCheckLocalAlignPosY);
	DDX_Check(pDX, IDC_CHECK_USE_ANGLE, m_bCheckLocalAlignPosAngle);
	DDX_Radio(pDX, IDC_RADIO_LINE_FIT_TOP, m_iRadioLocalAlignFitPos);
}


BEGIN_MESSAGE_MAP(CLocalAlignApplyDlg, CDialog)
	ON_BN_CLICKED(IDC_BUTTON_LOCAL_ALIGN_APPLY_TEST, &CLocalAlignApplyDlg::OnBnClickedButtonLocalAlignApplyTest)
	ON_BN_CLICKED(IDC_RADIO_USE_PART_CHECK, &CLocalAlignApplyDlg::OnBnClickedRadioUsePartCheck)
	ON_BN_CLICKED(IDC_RADIO_USE_EDGE_MEASURE, &CLocalAlignApplyDlg::OnBnClickedRadioUseEdgeMeasure)
	ON_BN_CLICKED(IDC_RADIO_USE_MATCHING_SHAPE, &CLocalAlignApplyDlg::OnBnClickedRadioUseMatchingShape)
	ON_BN_CLICKED(IDC_RADIO_USE_MATCHING_POS, &CLocalAlignApplyDlg::OnBnClickedRadioUseMatchingPos)
	ON_BN_CLICKED(IDC_RADIO_USE_LINE_FIT, &CLocalAlignApplyDlg::OnBnClickedRadioUseLineFit)
END_MESSAGE_MAP()


// CLocalAlignApplyDlg 메시지 처리기입니다.
void CLocalAlignApplyDlg::SetSelectedInspection(int iImageType, GTRegion *pSelectedROI, int iInspectionType)
{ 

	m_iSelectedImageType = iImageType;
	m_iInspectionType = iInspectionType;
	m_pSelectedROI = pSelectedROI;

	for (int i=0; i<MAX_IMAGE_TAB; i++)
	{
		if (THEAPP.m_pGFunction->ValidHImage(THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[i]))
			copy_image(THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[i], &(m_HLocalAlignImage[i]));
	}

}


void CLocalAlignApplyDlg::SetParam(CAlgorithmParam LocalAlignParam)
{
	m_bCheckUseLocalAlign = LocalAlignParam.m_bUseLocalAlign;
	m_cbLocalAlignImageIndex.SetCurSel(LocalAlignParam.m_iLocalAlignImageIndex);
	m_iEditLocalAlignROINo = LocalAlignParam.m_iLocalAlignROINo;
	m_iRadioLocalAlignType = LocalAlignParam.m_iLocalAlignType;
	m_iRadioMatchingApplyMethod = LocalAlignParam.m_iLocalAlignApplyMethod;
	m_bCheckLocalAlignPosX = LocalAlignParam.m_bLocalAlignPosX;
	m_bCheckLocalAlignPosY = LocalAlignParam.m_bLocalAlignPosY;
	m_bCheckLocalAlignPosAngle = LocalAlignParam.m_bLocalAlignPosAngle;
	m_iRadioLocalAlignFitPos = LocalAlignParam.m_iLocalAlignFitPos;
	

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

void CLocalAlignApplyDlg::GetParam(CAlgorithmParam *pLocalAlignParam)
{
	UpdateData();

	pLocalAlignParam->m_bUseLocalAlign = m_bCheckUseLocalAlign;
	pLocalAlignParam->m_iLocalAlignImageIndex = m_cbLocalAlignImageIndex.GetCurSel();
	pLocalAlignParam->m_iLocalAlignROINo = m_iEditLocalAlignROINo;
	pLocalAlignParam->m_iLocalAlignType = m_iRadioLocalAlignType;
	pLocalAlignParam->m_iLocalAlignApplyMethod = m_iRadioMatchingApplyMethod;
	pLocalAlignParam->m_bLocalAlignPosX = m_bCheckLocalAlignPosX;
	pLocalAlignParam->m_bLocalAlignPosY = m_bCheckLocalAlignPosY;
	pLocalAlignParam->m_bLocalAlignPosAngle = m_bCheckLocalAlignPosAngle;
	pLocalAlignParam->m_iLocalAlignFitPos = m_iRadioLocalAlignFitPos;


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

BOOL CLocalAlignApplyDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.
	m_cbLocalAlignImageIndex.SetCurSel(0);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}


void CLocalAlignApplyDlg::OnBnClickedButtonLocalAlignApplyTest()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	UpdateData();

	BOOL bUseLocalAlign = FALSE;
	Hobject HROIHRegion;
	CAlgorithmParam AlgorithmParam;

	switch (m_iSelectedImageType)
	{
	case 0:
		if (m_pSelectedROI != NULL)
			AlgorithmParam = m_pSelectedROI->m_AlgorithmParamSurface[m_iInspectionType];
		else
			AlgorithmParam = THEAPP.m_pModelDataManager->m_LocalAlignParamSurface;
		break;
	case 1:
		if (m_pSelectedROI != NULL)
			AlgorithmParam = m_pSelectedROI->m_AlgorithmParamEdge[m_iInspectionType];
		else
			AlgorithmParam = THEAPP.m_pModelDataManager->m_LocalAlignParamEdge;
		break;
	case 2:
		if (m_pSelectedROI != NULL)
			AlgorithmParam = m_pSelectedROI->m_AlgorithmParamLens1[m_iInspectionType];
		else
			AlgorithmParam = THEAPP.m_pModelDataManager->m_LocalAlignParamLens1;
		break;	
	case 3:
		if (m_pSelectedROI != NULL)
			AlgorithmParam = m_pSelectedROI->m_AlgorithmParamLens2[m_iInspectionType];
		else
			AlgorithmParam = THEAPP.m_pModelDataManager->m_LocalAlignParamLens2;
		break;
	case 4:
		if (m_pSelectedROI == NULL)
			return;
		AlgorithmParam = m_pSelectedROI->m_AlgorithmParamEpoxy1[m_iInspectionType];
		break;
	case 5:
		if (m_pSelectedROI == NULL)
			return;
		AlgorithmParam = m_pSelectedROI->m_AlgorithmParamEpoxy2[m_iInspectionType];
		break;
	}

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

	Hobject HResultXLD;
	gen_empty_obj(&HResultXLD);
	if (m_pSelectedROI != NULL)
	{
		HROIHRegion = m_pSelectedROI->GetROIHRegion(THEAPP.m_pCalDataService);
		THEAPP.m_pAlgorithm->ApplyLocalAlignResult(AlgorithmParam, &HROIHRegion);

		gen_contour_region_xld(HROIHRegion, &HResultXLD, "border");
	
		if (THEAPP.m_pGFunction->ValidHXLD(HResultXLD))
			concat_obj(THEAPP.m_pInspectAdminViewDlg->m_HParameterDefectXLD, HResultXLD, &(THEAPP.m_pInspectAdminViewDlg->m_HParameterDefectXLD));
	}
	else
	{
		if(!THEAPP.m_pModelDataManager->m_bOneBarrel)
		{
			if(THEAPP.m_pGFunction->ValidHXLD(THEAPP.m_pInspectAdminViewDlg->ContoursAffinTrans1))
			{
				gen_region_contour_xld(THEAPP.m_pInspectAdminViewDlg->ContoursAffinTrans1, &HROIHRegion, "filled");
				THEAPP.m_pAlgorithm->ApplyLocalAlignResult(AlgorithmParam, &HROIHRegion);

				gen_contour_region_xld(HROIHRegion, &HResultXLD, "border");
	
				if (THEAPP.m_pGFunction->ValidHXLD(HResultXLD))
					concat_obj(THEAPP.m_pInspectAdminViewDlg->m_HParameterDefectXLD, HResultXLD, &(THEAPP.m_pInspectAdminViewDlg->m_HParameterDefectXLD));
			}
			if(THEAPP.m_pGFunction->ValidHXLD(THEAPP.m_pInspectAdminViewDlg->ContoursAffinTrans2))
			{
				gen_region_contour_xld(THEAPP.m_pInspectAdminViewDlg->ContoursAffinTrans2, &HROIHRegion, "filled");
				THEAPP.m_pAlgorithm->ApplyLocalAlignResult(AlgorithmParam, &HROIHRegion);

				gen_contour_region_xld(HROIHRegion, &HResultXLD, "border");
	
				if (THEAPP.m_pGFunction->ValidHXLD(HResultXLD))
					concat_obj(THEAPP.m_pInspectAdminViewDlg->m_HParameterDefectXLD, HResultXLD, &(THEAPP.m_pInspectAdminViewDlg->m_HParameterDefectXLD));
			}
		}

		if(THEAPP.m_pGFunction->ValidHXLD(THEAPP.m_pInspectAdminViewDlg->ContoursAffinTrans3))
		{
			gen_region_contour_xld(THEAPP.m_pInspectAdminViewDlg->ContoursAffinTrans3, &HROIHRegion, "filled");
			THEAPP.m_pAlgorithm->ApplyLocalAlignResult(AlgorithmParam, &HROIHRegion);

				gen_contour_region_xld(HROIHRegion, &HResultXLD, "border");
	
				if (THEAPP.m_pGFunction->ValidHXLD(HResultXLD))
					concat_obj(THEAPP.m_pInspectAdminViewDlg->m_HParameterDefectXLD, HResultXLD, &(THEAPP.m_pInspectAdminViewDlg->m_HParameterDefectXLD));
		}
		if(THEAPP.m_pGFunction->ValidHXLD(THEAPP.m_pInspectAdminViewDlg->ContoursAffinTrans4))
		{
			gen_region_contour_xld(THEAPP.m_pInspectAdminViewDlg->ContoursAffinTrans4, &HROIHRegion, "filled");
			THEAPP.m_pAlgorithm->ApplyLocalAlignResult(AlgorithmParam, &HROIHRegion);

				gen_contour_region_xld(HROIHRegion, &HResultXLD, "border");
	
				if (THEAPP.m_pGFunction->ValidHXLD(HResultXLD))
					concat_obj(THEAPP.m_pInspectAdminViewDlg->m_HParameterDefectXLD, HResultXLD, &(THEAPP.m_pInspectAdminViewDlg->m_HParameterDefectXLD));
		}
		if(THEAPP.m_pGFunction->ValidHXLD (THEAPP.m_pInspectAdminViewDlg->ContoursAffinTrans5))
		{
			gen_region_contour_xld(THEAPP.m_pInspectAdminViewDlg->ContoursAffinTrans5, &HROIHRegion, "filled");
			THEAPP.m_pAlgorithm->ApplyLocalAlignResult(AlgorithmParam, &HROIHRegion);

				gen_contour_region_xld(HROIHRegion, &HResultXLD, "border");
	
				if (THEAPP.m_pGFunction->ValidHXLD(HResultXLD))
					concat_obj(THEAPP.m_pInspectAdminViewDlg->m_HParameterDefectXLD, HResultXLD, &(THEAPP.m_pInspectAdminViewDlg->m_HParameterDefectXLD));
		}
	}

	THEAPP.m_pInspectAdminViewDlg->UpdateView();
	// 24.03.18 Local Align - LeeGW
}


void CLocalAlignApplyDlg::OnBnClickedRadioUsePartCheck()
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


void CLocalAlignApplyDlg::OnBnClickedRadioUseEdgeMeasure()
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


void CLocalAlignApplyDlg::OnBnClickedRadioUseMatchingShape()
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


void CLocalAlignApplyDlg::OnBnClickedRadioUseMatchingPos()
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


void CLocalAlignApplyDlg::OnBnClickedRadioUseLineFit()
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
