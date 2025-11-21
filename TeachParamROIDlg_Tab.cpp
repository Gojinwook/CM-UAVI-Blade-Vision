// TeachParamDlg_Tab.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "uScan.h"
#include "TeachParamROIDlg_Tab.h"
#include "afxdialogex.h"


// CTeachParamROIDlg_Tab 대화 상자입니다.

IMPLEMENT_DYNAMIC(CTeachParamROIDlg_Tab, CDialog)

CTeachParamROIDlg_Tab::CTeachParamROIDlg_Tab(CWnd* pParent /*=NULL*/)
	: CDialog(CTeachParamROIDlg_Tab::IDD, pParent)
{
	m_iTabIdx = 0;
	m_iImageIdx = 0;
	m_bCheckInspect = FALSE;

	m_bCheckArea1 = TRUE;
	m_iEditArea1Contour1Margin = 0;
	m_iEditArea1Contour2Margin = -9999;

	m_bCheckArea2 = FALSE;
	m_iEditArea2Contour1Margin = 0;
	m_iEditArea2Contour2Margin = -9999;

	m_bCheckArea3 = FALSE;
	m_iEditArea3Contour1Margin = 0;
	m_iEditArea3Contour2Margin = -9999;

	m_bCheckSpecialNG = FALSE;
}

CTeachParamROIDlg_Tab::~CTeachParamROIDlg_Tab()
{
}

void CTeachParamROIDlg_Tab::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Check(pDX, IDC_CHK_INSPECT, m_bCheckInspect);
	DDX_Control(pDX, IDC_COMBO_DEFECT_TYPE, m_cbDefectType);

	DDX_Check(pDX, IDC_CHK_AREA1, m_bCheckArea1);
	DDX_Text(pDX, IDC_EDIT_AREA1_CONTOUR1_MARGIN, m_iEditArea1Contour1Margin);
	DDX_Text(pDX, IDC_EDIT_AREA1_CONTOUR2_MARGIN, m_iEditArea1Contour2Margin);

	DDX_Check(pDX, IDC_CHK_AREA2, m_bCheckArea2);
	DDX_Text(pDX, IDC_EDIT_AREA2_CONTOUR1_MARGIN, m_iEditArea2Contour1Margin);
	DDX_Text(pDX, IDC_EDIT_AREA2_CONTOUR2_MARGIN, m_iEditArea2Contour2Margin);

	DDX_Check(pDX, IDC_CHK_AREA3, m_bCheckArea3);
	DDX_Text(pDX, IDC_EDIT_AREA3_CONTOUR1_MARGIN, m_iEditArea3Contour1Margin);
	DDX_Text(pDX, IDC_EDIT_AREA3_CONTOUR2_MARGIN, m_iEditArea3Contour2Margin);

	DDX_Control(pDX, IDC_BUTTON_ROI_UPDATE, m_bnUpdateROI);
	DDX_Control(pDX, IDC_BUTTON_PARAMETER_TEST, m_bnParamTest);

	DDX_Control(pDX, IDC_STATIC_INSPECT, m_LabelInspect);
	DDX_Control(pDX, IDC_STATIC_DEFECT, m_LabelDefect);
	DDX_Control(pDX, IDC_STATIC_ROI, m_LabelArea);

	DDX_Control(pDX, IDC_BUTTON_ROI_ALGORITHM, m_bnROIAlgorithm);	// LeeGW
	DDX_Control(pDX, IDC_STATIC_ROI_ALGORITHM, m_LabelROIAlgorithm);

	DDX_Control(pDX, IDC_STATIC_NG_SORT, m_LabelSpecialNG);
	DDX_Check(pDX, IDC_CHK_SPECIAL_NG, m_bCheckSpecialNG);

}

void CTeachParamROIDlg_Tab::UpDate(BOOL bFlag)
{
	if (THEAPP.m_pInspectAdminViewDlg->m_pLastSelectedRegion == NULL)
		return;

	if (bFlag)	// GetParam
	{
		UpdateData(bFlag);
		THEAPP.m_pInspectAdminViewDlg->m_pLastSelectedRegion->m_AlgorithmParam[m_iTabIdx].m_bInspect = m_bCheckInspect;
		THEAPP.m_pInspectAdminViewDlg->m_pLastSelectedRegion->m_AlgorithmParam[m_iTabIdx].m_iDefectNameIdx = m_cbDefectType.GetCurSel();
		THEAPP.m_pInspectAdminViewDlg->m_pLastSelectedRegion->m_AlgorithmParam[m_iTabIdx].m_bUseInspectArea[0] = m_bCheckArea1;
		THEAPP.m_pInspectAdminViewDlg->m_pLastSelectedRegion->m_AlgorithmParam[m_iTabIdx].m_iInspectAreaContour1Margin[0] = m_iEditArea1Contour1Margin;
		THEAPP.m_pInspectAdminViewDlg->m_pLastSelectedRegion->m_AlgorithmParam[m_iTabIdx].m_iInspectAreaContour2Margin[0] = m_iEditArea1Contour2Margin;
		THEAPP.m_pInspectAdminViewDlg->m_pLastSelectedRegion->m_AlgorithmParam[m_iTabIdx].m_bUseInspectArea[1] = m_bCheckArea2;
		THEAPP.m_pInspectAdminViewDlg->m_pLastSelectedRegion->m_AlgorithmParam[m_iTabIdx].m_iInspectAreaContour1Margin[1] = m_iEditArea2Contour1Margin;
		THEAPP.m_pInspectAdminViewDlg->m_pLastSelectedRegion->m_AlgorithmParam[m_iTabIdx].m_iInspectAreaContour2Margin[1] = m_iEditArea2Contour2Margin;
		THEAPP.m_pInspectAdminViewDlg->m_pLastSelectedRegion->m_AlgorithmParam[m_iTabIdx].m_bUseInspectArea[2] = m_bCheckArea3;
		THEAPP.m_pInspectAdminViewDlg->m_pLastSelectedRegion->m_AlgorithmParam[m_iTabIdx].m_iInspectAreaContour1Margin[2] = m_iEditArea3Contour1Margin;
		THEAPP.m_pInspectAdminViewDlg->m_pLastSelectedRegion->m_AlgorithmParam[m_iTabIdx].m_iInspectAreaContour2Margin[2] = m_iEditArea3Contour2Margin;

		THEAPP.m_pInspectAdminViewDlg->m_pLastSelectedRegion->m_AlgorithmParam[m_iTabIdx].m_bSpecialNG = m_bCheckSpecialNG;
	}
	else		// SetParam
	{
		m_bCheckInspect = THEAPP.m_pInspectAdminViewDlg->m_pLastSelectedRegion->m_AlgorithmParam[m_iTabIdx].m_bInspect;
		m_cbDefectType.SetCurSel(THEAPP.m_pInspectAdminViewDlg->m_pLastSelectedRegion->m_AlgorithmParam[m_iTabIdx].m_iDefectNameIdx);

		m_bCheckArea1 = THEAPP.m_pInspectAdminViewDlg->m_pLastSelectedRegion->m_AlgorithmParam[m_iTabIdx].m_bUseInspectArea[0];
		m_iEditArea1Contour1Margin = THEAPP.m_pInspectAdminViewDlg->m_pLastSelectedRegion->m_AlgorithmParam[m_iTabIdx].m_iInspectAreaContour1Margin[0];
		m_iEditArea1Contour2Margin = THEAPP.m_pInspectAdminViewDlg->m_pLastSelectedRegion->m_AlgorithmParam[m_iTabIdx].m_iInspectAreaContour2Margin[0];
		m_bCheckArea2 = THEAPP.m_pInspectAdminViewDlg->m_pLastSelectedRegion->m_AlgorithmParam[m_iTabIdx].m_bUseInspectArea[1];
		m_iEditArea2Contour1Margin = THEAPP.m_pInspectAdminViewDlg->m_pLastSelectedRegion->m_AlgorithmParam[m_iTabIdx].m_iInspectAreaContour1Margin[1];
		m_iEditArea2Contour2Margin = THEAPP.m_pInspectAdminViewDlg->m_pLastSelectedRegion->m_AlgorithmParam[m_iTabIdx].m_iInspectAreaContour2Margin[1];
		m_bCheckArea3 = THEAPP.m_pInspectAdminViewDlg->m_pLastSelectedRegion->m_AlgorithmParam[m_iTabIdx].m_bUseInspectArea[2];
		m_iEditArea3Contour1Margin = THEAPP.m_pInspectAdminViewDlg->m_pLastSelectedRegion->m_AlgorithmParam[m_iTabIdx].m_iInspectAreaContour1Margin[2];
		m_iEditArea3Contour2Margin = THEAPP.m_pInspectAdminViewDlg->m_pLastSelectedRegion->m_AlgorithmParam[m_iTabIdx].m_iInspectAreaContour2Margin[2];

		m_bCheckSpecialNG = THEAPP.m_pInspectAdminViewDlg->m_pLastSelectedRegion->m_AlgorithmParam[m_iTabIdx].m_bSpecialNG;

		UpdateData(bFlag);
	}
}

BEGIN_MESSAGE_MAP(CTeachParamROIDlg_Tab, CDialog)
	ON_BN_CLICKED(IDC_BUTTON_ROI_UPDATE, &CTeachParamROIDlg_Tab::OnBnClickedButtonRoiUpdate)
	ON_BN_CLICKED(IDC_BUTTON_PARAMETER_TEST, &CTeachParamROIDlg_Tab::OnBnClickedButtonParameterTest)
	ON_BN_CLICKED(IDC_BUTTON_ROI_ALGORITHM, &CTeachParamROIDlg_Tab::OnBnClickedButtonROIAlgorithm)
END_MESSAGE_MAP()


// CTeachParamROIDlg_Tab 메시지 처리기입니다.


BOOL CTeachParamROIDlg_Tab::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_UsedAlgorithmList.AttachGrid(this, IDC_STATIC_ALGORITHM_LIST);

	m_bnUpdateROI.Init_Ctrl(_T("Arial"), 9, FALSE, BLACK, ROYAL_BLUE, 0, 0);
	m_bnParamTest.Init_Ctrl(_T("Arial"), 9, FALSE, BLACK, ROYAL_BLUE, 0, 0);
	m_bnROIAlgorithm.Init_Ctrl(_T("Arial"), 9, FALSE, BLACK, OLIVE_GREEN, 0, 0);

	m_LabelInspect.Init_Ctrl(_T("Arial Black"), 8, FALSE, WHITE, BLACK);
	m_LabelDefect.Init_Ctrl(_T("Arial Black"), 8, FALSE, WHITE, BLACK);
	m_LabelArea.Init_Ctrl(_T("Arial Black"), 8, FALSE, WHITE, BLACK);
	m_LabelROIAlgorithm.Init_Ctrl(_T("Arial Black"), 8, FALSE, WHITE, BLACK);
	m_LabelSpecialNG.Init_Ctrl(_T("Arial Black"), 8, FALSE, WHITE, BLACK);

	m_cbDefectType.Clear();
	for (int i = 0; i < MAX_DEFECT_NAME; i++)
	{
		m_cbDefectType.AddString(g_sDefectName[i]);
	}
	m_cbDefectType.SetCurSel(DEFECT_NAME_DIRT);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}


void CTeachParamROIDlg_Tab::OnBnClickedButtonRoiUpdate()
{
	if (THEAPP.m_pInspectAdminViewDlg->m_pLastSelectedRegion == NULL)
		return;

	UpdateData(TRUE);

	CAlgorithmParam AlgorithmParam;
	AlgorithmParam = THEAPP.m_pInspectAdminViewDlg->m_pLastSelectedRegion->m_AlgorithmParam[m_iTabIdx];

	AlgorithmParam.m_bUseInspectArea[0] = m_bCheckArea1;
	AlgorithmParam.m_iInspectAreaContour1Margin[0] = m_iEditArea1Contour1Margin;
	AlgorithmParam.m_iInspectAreaContour2Margin[0] = m_iEditArea1Contour2Margin;
	AlgorithmParam.m_bUseInspectArea[1] = m_bCheckArea2;
	AlgorithmParam.m_iInspectAreaContour1Margin[1] = m_iEditArea2Contour1Margin;;
	AlgorithmParam.m_iInspectAreaContour2Margin[1] = m_iEditArea2Contour2Margin;
	AlgorithmParam.m_bUseInspectArea[2] = m_bCheckArea3;
	AlgorithmParam.m_iInspectAreaContour1Margin[2] = m_iEditArea3Contour1Margin;
	AlgorithmParam.m_iInspectAreaContour2Margin[2] = m_iEditArea3Contour2Margin;

	Hobject HInspectAreaRgn;
	HInspectAreaRgn = THEAPP.m_pInspectAdminViewDlg->m_pLastSelectedRegion->GetROIHRegion(THEAPP.m_pCalDataService);
	if (THEAPP.m_pGFunction->ValidHRegion(HInspectAreaRgn) == FALSE)
		return;

	int iNoInspectROI = THEAPP.m_pModelDataManager->m_pInspectionArea->GetChildTRegionCount();

	GTRegion* pInspectROIRgn;

	for (int iROIIndex = 0; iROIIndex < iNoInspectROI; iROIIndex++)
	{
		pInspectROIRgn = THEAPP.m_pModelDataManager->m_pInspectionArea->GetChildTRegion(iROIIndex);
		pInspectROIRgn->ResetLocalAlignResult(MAX_MODULE_ONE_TRAY - 1);
	}

	Hobject HPartCheckAlignRgn, HPartCheckResultRgn;
	gen_empty_obj(&HPartCheckAlignRgn);
	gen_empty_obj(&HPartCheckResultRgn);

	BOOL bAnisoAlignShiftResult;
	Hobject HAnisoAlignRgn;
	gen_empty_obj(&HAnisoAlignRgn);

	for (int iIndex = 0; iIndex < iNoInspectROI; iIndex++)
	{
		pInspectROIRgn = THEAPP.m_pModelDataManager->m_pInspectionArea->GetChildTRegion(iIndex);
		if (pInspectROIRgn == NULL)
			continue;

		if (pInspectROIRgn->m_iTeachImageIndex != (m_iImageIdx + 1))
			continue;

		if (pInspectROIRgn->m_iInspectionType != INSPECTION_TYPE_LOCAL_ALIGN)
			continue;

		for (int tab = 0; tab < MAX_ROI_TEACHING_TAB; tab++)
		{
			CAlgorithmParam Param = pInspectROIRgn->m_AlgorithmParam[tab];
			Hobject HInspectAreaRgn = pInspectROIRgn->GetROIHRegion(THEAPP.m_pCalDataService);

			if (Param.m_bInspect)
			{
				// Part Check
				if (Param.m_bROIPartCheckUse && Param.m_bROIPartCheckLocalAlignUse)
				{

					HPartCheckResultRgn = THEAPP.m_pAlgorithm->PartCheckAlgorithm(pInspectROIRgn->m_HPartModelID, THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[m_iImageIdx], HInspectAreaRgn, Param, &pInspectROIRgn->m_iLocalAlignDeltaX[MAX_MODULE_ONE_TRAY - 1], &pInspectROIRgn->m_iLocalAlignDeltaY[MAX_MODULE_ONE_TRAY - 1], &pInspectROIRgn->m_dLocalAlignDeltaAngle[MAX_MODULE_ONE_TRAY - 1], &pInspectROIRgn->m_dLocalAlignDeltaAngleFixedPointX[MAX_MODULE_ONE_TRAY - 1], &pInspectROIRgn->m_dLocalAlignDeltaAngleFixedPointY[MAX_MODULE_ONE_TRAY - 1], &HPartCheckAlignRgn);

					if (THEAPP.m_pGFunction->ValidHRegion(HPartCheckAlignRgn))
						pInspectROIRgn->m_HLocalAlignShapeRgn[MAX_MODULE_ONE_TRAY - 1] = HPartCheckAlignRgn;
				}

				// 비등방 얼라인
				if (Param.m_bUseROIAnisoAlign && Param.m_bROIAnisoAlignLocalAlignUse)
				{
					HAnisoAlignRgn = THEAPP.m_pAlgorithm->AnisoAlignAlgorithm(pInspectROIRgn->m_HAnisoModelID, THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[m_iImageIdx], HInspectAreaRgn, Param, &bAnisoAlignShiftResult, &pInspectROIRgn->m_iLocalAlignDeltaX[MAX_MODULE_ONE_TRAY - 1], &pInspectROIRgn->m_iLocalAlignDeltaY[MAX_MODULE_ONE_TRAY - 1], &pInspectROIRgn->m_dLocalAlignDeltaAngle[MAX_MODULE_ONE_TRAY - 1], &pInspectROIRgn->m_dLocalAlignDeltaAngleFixedPointX[MAX_MODULE_ONE_TRAY - 1], &pInspectROIRgn->m_dLocalAlignDeltaAngleFixedPointY[MAX_MODULE_ONE_TRAY - 1]);

					if (THEAPP.m_pGFunction->ValidHRegion(HAnisoAlignRgn))
						pInspectROIRgn->m_HLocalAlignShapeRgn[MAX_MODULE_ONE_TRAY - 1] = HAnisoAlignRgn;
				}
			}
		}
	}


	//////////////////////////////////////////////////////////////////////////
	//  Flexible Inspection ROI
	BOOL bRet;
	bRet = THEAPP.m_pAlgorithm->GetInspectAreaROI(THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[m_iImageIdx], HInspectAreaRgn, AlgorithmParam, &(THEAPP.m_pInspectAdminViewDlg->m_HInspectAreaRgn), MAX_MODULE_ONE_TRAY-1);

	if (bRet)
		THEAPP.m_pInspectAdminViewDlg->UpdateView();
	else
		AfxMessageBox("Inspection Area Search Fail.", MB_SYSTEMMODAL | MB_ICONWARNING);
}

void CTeachParamROIDlg_Tab::OnBnClickedButtonParameterTest()
{
	if (THEAPP.m_pInspectAdminViewDlg->m_pLastSelectedRegion == NULL)
		return;

	UpdateData(TRUE);

	CAlgorithmParam AlgorithmParam;
	Hobject HAllFoundInspectAreaRgn, HAllDefectAreaRgn, HAllResultXld;
	Hobject HFoundInspectAreaRgn, HDefectAreaRgn, HResultXld;

	gen_empty_obj(&HAllFoundInspectAreaRgn);
	gen_empty_obj(&HAllDefectAreaRgn);
	gen_empty_obj(&HAllResultXld);

	int iNoInspectROI = THEAPP.m_pModelDataManager->m_pInspectionArea->GetChildTRegionCount();

	GTRegion* pInspectROIRgn;
	for (int iROIIndex = 0; iROIIndex < iNoInspectROI; iROIIndex++)
	{
		pInspectROIRgn = THEAPP.m_pModelDataManager->m_pInspectionArea->GetChildTRegion(iROIIndex);
		pInspectROIRgn->ResetLocalAlignResult(MAX_MODULE_ONE_TRAY - 1);
	}

	Hobject HPartCheckAlignRgn, HPartCheckResultRgn;
	gen_empty_obj(&HPartCheckAlignRgn);
	gen_empty_obj(&HPartCheckResultRgn);

	BOOL bAnisoAlignShiftResult;
	Hobject HAnisoAlignRgn;
	gen_empty_obj(&HAnisoAlignRgn);

	for (int iIndex = 0; iIndex < iNoInspectROI; iIndex++)
	{
		pInspectROIRgn = THEAPP.m_pModelDataManager->m_pInspectionArea->GetChildTRegion(iIndex);
		if (pInspectROIRgn == NULL)
			continue;

		if (pInspectROIRgn->m_iTeachImageIndex != (m_iImageIdx + 1))
			continue;

		if (pInspectROIRgn->m_iInspectionType != INSPECTION_TYPE_LOCAL_ALIGN)
			continue;

		for (int tab = 0; tab < MAX_ROI_TEACHING_TAB; tab++)
		{
			CAlgorithmParam Param = pInspectROIRgn->m_AlgorithmParam[tab];
			Hobject HInspectAreaRgn = pInspectROIRgn->GetROIHRegion(THEAPP.m_pCalDataService);

			if (Param.m_bInspect)
			{
				// Part Check
				if (Param.m_bROIPartCheckUse && Param.m_bROIPartCheckLocalAlignUse)
				{
					HPartCheckResultRgn = THEAPP.m_pAlgorithm->PartCheckAlgorithm(pInspectROIRgn->m_HPartModelID, THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[m_iImageIdx], HInspectAreaRgn, Param, &pInspectROIRgn->m_iLocalAlignDeltaX[MAX_MODULE_ONE_TRAY - 1], &pInspectROIRgn->m_iLocalAlignDeltaY[MAX_MODULE_ONE_TRAY - 1], &pInspectROIRgn->m_dLocalAlignDeltaAngle[MAX_MODULE_ONE_TRAY - 1], &pInspectROIRgn->m_dLocalAlignDeltaAngleFixedPointX[MAX_MODULE_ONE_TRAY - 1], &pInspectROIRgn->m_dLocalAlignDeltaAngleFixedPointY[MAX_MODULE_ONE_TRAY - 1], &HPartCheckAlignRgn);

					if (THEAPP.m_pGFunction->ValidHRegion(HPartCheckAlignRgn))
						pInspectROIRgn->m_HLocalAlignShapeRgn[MAX_MODULE_ONE_TRAY - 1] = HPartCheckAlignRgn;
				}

				// 비등방 얼라인
				if (Param.m_bUseROIAnisoAlign && Param.m_bROIAnisoAlignLocalAlignUse)
				{
					HAnisoAlignRgn = THEAPP.m_pAlgorithm->AnisoAlignAlgorithm(pInspectROIRgn->m_HAnisoModelID, THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[m_iImageIdx], HInspectAreaRgn, Param, &bAnisoAlignShiftResult, &pInspectROIRgn->m_iLocalAlignDeltaX[MAX_MODULE_ONE_TRAY - 1], &pInspectROIRgn->m_iLocalAlignDeltaY[MAX_MODULE_ONE_TRAY - 1], &pInspectROIRgn->m_dLocalAlignDeltaAngle[MAX_MODULE_ONE_TRAY - 1], &pInspectROIRgn->m_dLocalAlignDeltaAngleFixedPointX[MAX_MODULE_ONE_TRAY - 1], &pInspectROIRgn->m_dLocalAlignDeltaAngleFixedPointY[MAX_MODULE_ONE_TRAY - 1]);

					if (THEAPP.m_pGFunction->ValidHRegion(HAnisoAlignRgn))
						pInspectROIRgn->m_HLocalAlignShapeRgn[MAX_MODULE_ONE_TRAY - 1] = HAnisoAlignRgn;
				}
			}
		}
	}

	for (int tab = 0; tab < MAX_ROI_TEACHING_TAB; tab++)
	{

		AlgorithmParam = THEAPP.m_pInspectAdminViewDlg->m_pLastSelectedRegion->m_AlgorithmParam[tab];

		AlgorithmParam.m_bUseInspectArea[0] = m_bCheckArea1;
		AlgorithmParam.m_iInspectAreaContour1Margin[0] = m_iEditArea1Contour1Margin;
		AlgorithmParam.m_iInspectAreaContour2Margin[0] = m_iEditArea1Contour2Margin;
		AlgorithmParam.m_bUseInspectArea[1] = m_bCheckArea2;
		AlgorithmParam.m_iInspectAreaContour1Margin[1] = m_iEditArea2Contour1Margin;;
		AlgorithmParam.m_iInspectAreaContour2Margin[1] = m_iEditArea2Contour2Margin;
		AlgorithmParam.m_bUseInspectArea[2] = m_bCheckArea3;
		AlgorithmParam.m_iInspectAreaContour1Margin[2] = m_iEditArea3Contour1Margin;
		AlgorithmParam.m_iInspectAreaContour2Margin[2] = m_iEditArea3Contour2Margin;

		gen_empty_obj(&HDefectAreaRgn);
		gen_empty_obj(&HFoundInspectAreaRgn);

		HDefectAreaRgn = THEAPP.m_pAlgorithm->CommonAlgorithmROI(THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[m_iImageIdx], THEAPP.m_pInspectAdminViewDlg->m_pLastSelectedRegion, AlgorithmParam, &HFoundInspectAreaRgn, &HResultXld, MAX_MODULE_ONE_TRAY - 1);

		concat_obj(HAllFoundInspectAreaRgn, HFoundInspectAreaRgn, &HAllFoundInspectAreaRgn);
		concat_obj(HAllDefectAreaRgn, HDefectAreaRgn, &HAllDefectAreaRgn);
		concat_obj(HAllResultXld, HResultXld, &HAllResultXld);
	}

	THEAPP.m_pInspectAdminViewDlg->m_HParameterDefectXLD = HAllResultXld;
	THEAPP.m_pInspectAdminViewDlg->m_HParameterDefectRgn = HAllDefectAreaRgn;
	THEAPP.m_pInspectAdminViewDlg->UpdateView();
}

void CTeachParamROIDlg_Tab::UpdateList()
{
	if (THEAPP.m_pInspectAdminViewDlg->m_pLastSelectedRegion == NULL)
		return;

	if (THEAPP.m_pModelDataManager->m_sModelName == ".")
		return;

	CAlgorithmParam AlgorithmParam;
	AlgorithmParam = THEAPP.m_pInspectAdminViewDlg->m_pLastSelectedRegion->m_AlgorithmParam[m_iTabIdx];

	if (AlgorithmParam.m_bUseBrightFixedThres)
		m_UsedAlgorithmList.QuickSetText(1, 0, " O ");
	else
		m_UsedAlgorithmList.QuickSetText(1, 0, "");
	if (AlgorithmParam.m_bUseDarkFixedThres)
		m_UsedAlgorithmList.QuickSetText(2, 0, " O ");
	else
		m_UsedAlgorithmList.QuickSetText(2, 0, "");

	if (AlgorithmParam.m_bUseBrightDT)
		m_UsedAlgorithmList.QuickSetText(1, 1, " O ");
	else
		m_UsedAlgorithmList.QuickSetText(1, 1, "");
	if (AlgorithmParam.m_bUseDarkDT)
		m_UsedAlgorithmList.QuickSetText(2, 1, " O ");
	else
		m_UsedAlgorithmList.QuickSetText(2, 1, "");

	if (AlgorithmParam.m_bUseBrightUniformityCheck)
		m_UsedAlgorithmList.QuickSetText(1, 2, " O ");
	else
		m_UsedAlgorithmList.QuickSetText(1, 2, "");
	if (AlgorithmParam.m_bUseDarkUniformityCheck)
		m_UsedAlgorithmList.QuickSetText(2, 2, " O ");
	else
		m_UsedAlgorithmList.QuickSetText(2, 2, "");

	if (AlgorithmParam.m_bUseBrightHystThres)
		m_UsedAlgorithmList.QuickSetText(1, 3, " O ");
	else
		m_UsedAlgorithmList.QuickSetText(1, 3, "");
	if (AlgorithmParam.m_bUseDarkHystThres)
		m_UsedAlgorithmList.QuickSetText(2, 3, " O ");
	else
		m_UsedAlgorithmList.QuickSetText(2, 3, "");

	if (AlgorithmParam.m_bUseEdge)
		m_UsedAlgorithmList.QuickSetText(1, 4, " O ");
	else
		m_UsedAlgorithmList.QuickSetText(1, 4, "");
	if (AlgorithmParam.m_bUseEdge)
		m_UsedAlgorithmList.QuickSetText(2, 4, " O ");
	else
		m_UsedAlgorithmList.QuickSetText(2, 4, "");

	m_UsedAlgorithmList.RedrawAll();
}


void CTeachParamROIDlg_Tab::OnBnClickedButtonROIAlgorithm()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if (THEAPP.m_pInspectAdminViewDlg->m_pLastSelectedRegion == NULL)
		return;

	if (THEAPP.m_pModelDataManager->m_sModelName == ".")
		return;

	UpDate(TRUE);		// 화면 파라미터 저장

	THEAPP.m_pInspectAdminViewDlg->SetToolBarStateDraw();
	THEAPP.m_pTabControlDlg->ShowROIAlgorithmWnd(m_iImageIdx, THEAPP.m_pInspectAdminViewDlg->m_pLastSelectedRegion, m_iTabIdx);
}
