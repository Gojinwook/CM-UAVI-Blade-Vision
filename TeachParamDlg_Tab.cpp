// TeachParamDlg_Tab.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "uScan.h"
#include "TeachParamDlg_Tab.h"
#include "afxdialogex.h"


// CTeachParamDlg_Tab 대화 상자입니다.

IMPLEMENT_DYNAMIC(CTeachParamDlg_Tab, CDialog)

CTeachParamDlg_Tab::CTeachParamDlg_Tab(CWnd* pParent /*=NULL*/)
	: CDialog(CTeachParamDlg_Tab::IDD, pParent)
{
	m_iTabIdx = 0;
	m_iImageIdx = 0;
	m_bCheckInspect = FALSE;

	m_bCheckArea1 = TRUE;
	m_iEditArea1Contour1Margin = -2;
	m_iEditArea1Contour2Margin = 2;

	m_bCheckArea2 = FALSE;
	m_iEditArea2Contour1Margin = 0;
	m_iEditArea2Contour2Margin = 0;

	m_bCheckArea3 = FALSE;
	m_iEditArea3Contour1Margin = 0;
	m_iEditArea3Contour2Margin = 0;
}

CTeachParamDlg_Tab::~CTeachParamDlg_Tab()
{
}

void CTeachParamDlg_Tab::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Check(pDX, IDC_CHK_INSPECT, m_bCheckInspect);
	DDX_Control(pDX, IDC_COMBO_DEFECT_TYPE, m_cbDefectType);

	DDX_Check(pDX, IDC_CHK_AREA1, m_bCheckArea1);
	DDX_Control(pDX, IDC_COMBO_AREA1_CONTOUR1, m_cbArea1Contour1);
	DDX_Text(pDX, IDC_EDIT_AREA1_CONTOUR1_MARGIN, m_iEditArea1Contour1Margin);
	DDX_Control(pDX, IDC_COMBO_AREA1_CONTOUR2, m_cbArea1Contour2);
	DDX_Text(pDX, IDC_EDIT_AREA1_CONTOUR2_MARGIN, m_iEditArea1Contour2Margin);

	DDX_Check(pDX, IDC_CHK_AREA2, m_bCheckArea2);
	DDX_Control(pDX, IDC_COMBO_AREA2_CONTOUR1, m_cbArea2Contour1);
	DDX_Text(pDX, IDC_EDIT_AREA2_CONTOUR1_MARGIN, m_iEditArea2Contour1Margin);
	DDX_Control(pDX, IDC_COMBO_AREA2_CONTOUR2, m_cbArea2Contour2);
	DDX_Text(pDX, IDC_EDIT_AREA2_CONTOUR2_MARGIN, m_iEditArea2Contour2Margin);

	DDX_Check(pDX, IDC_CHK_AREA3, m_bCheckArea3);
	DDX_Control(pDX, IDC_COMBO_AREA3_CONTOUR1, m_cbArea3Contour1);
	DDX_Text(pDX, IDC_EDIT_AREA3_CONTOUR1_MARGIN, m_iEditArea3Contour1Margin);
	DDX_Control(pDX, IDC_COMBO_AREA3_CONTOUR2, m_cbArea3Contour2);
	DDX_Text(pDX, IDC_EDIT_AREA3_CONTOUR2_MARGIN, m_iEditArea3Contour2Margin);

	DDX_Control(pDX, IDC_BUTTON_ROI_UPDATE, m_bnUpdateROI);
	DDX_Control(pDX, IDC_BUTTON_PARAMETER_TEST, m_bnParamTest);
	DDX_Control(pDX, IDC_BUTTON_ALGORITHM, m_bnAlgorithm);

	DDX_Control(pDX, IDC_STATIC_INSPECT, m_LabelInspect);
	DDX_Control(pDX, IDC_STATIC_DEFECT, m_LabelDefect);
	DDX_Control(pDX, IDC_STATIC_ROI, m_LabelArea);
	DDX_Control(pDX, IDC_STATIC_ALGORITHM, m_LabelAlgorithm);
	DDX_Control(pDX, IDC_STATIC_NG_SORT, m_LabelSpecialNG);
	
	
}

void CTeachParamDlg_Tab::UpDate(BOOL bFlag)
{
	if (bFlag)	// GetParam
	{
		UpdateData(bFlag);

		THEAPP.m_pModelDataManager->m_AlgorithmParam[m_iImageIdx][m_iTabIdx].m_bInspect = m_bCheckInspect;
		THEAPP.m_pModelDataManager->m_AlgorithmParam[m_iImageIdx][m_iTabIdx].m_iDefectNameIdx = m_cbDefectType.GetCurSel();
		THEAPP.m_pModelDataManager->m_AlgorithmParam[m_iImageIdx][m_iTabIdx].m_bUseInspectArea[0] = m_bCheckArea1;
		THEAPP.m_pModelDataManager->m_AlgorithmParam[m_iImageIdx][m_iTabIdx].m_iInspectAreaContour1Number[0] = m_cbArea1Contour1.GetCurSel();
		THEAPP.m_pModelDataManager->m_AlgorithmParam[m_iImageIdx][m_iTabIdx].m_iInspectAreaContour1Margin[0] = m_iEditArea1Contour1Margin;
		THEAPP.m_pModelDataManager->m_AlgorithmParam[m_iImageIdx][m_iTabIdx].m_iInspectAreaContour2Number[0] = m_cbArea1Contour2.GetCurSel();
		THEAPP.m_pModelDataManager->m_AlgorithmParam[m_iImageIdx][m_iTabIdx].m_iInspectAreaContour2Margin[0] = m_iEditArea1Contour2Margin;
		THEAPP.m_pModelDataManager->m_AlgorithmParam[m_iImageIdx][m_iTabIdx].m_bUseInspectArea[1] = m_bCheckArea2;
		THEAPP.m_pModelDataManager->m_AlgorithmParam[m_iImageIdx][m_iTabIdx].m_iInspectAreaContour1Number[1] = m_cbArea2Contour1.GetCurSel();
		THEAPP.m_pModelDataManager->m_AlgorithmParam[m_iImageIdx][m_iTabIdx].m_iInspectAreaContour1Margin[1] = m_iEditArea2Contour1Margin;
		THEAPP.m_pModelDataManager->m_AlgorithmParam[m_iImageIdx][m_iTabIdx].m_iInspectAreaContour2Number[1] = m_cbArea2Contour2.GetCurSel();
		THEAPP.m_pModelDataManager->m_AlgorithmParam[m_iImageIdx][m_iTabIdx].m_iInspectAreaContour2Margin[1] = m_iEditArea2Contour2Margin;
		THEAPP.m_pModelDataManager->m_AlgorithmParam[m_iImageIdx][m_iTabIdx].m_bUseInspectArea[2] = m_bCheckArea3;
		THEAPP.m_pModelDataManager->m_AlgorithmParam[m_iImageIdx][m_iTabIdx].m_iInspectAreaContour1Number[2] = m_cbArea3Contour1.GetCurSel();
		THEAPP.m_pModelDataManager->m_AlgorithmParam[m_iImageIdx][m_iTabIdx].m_iInspectAreaContour1Margin[2] = m_iEditArea3Contour1Margin;
		THEAPP.m_pModelDataManager->m_AlgorithmParam[m_iImageIdx][m_iTabIdx].m_iInspectAreaContour2Number[2] = m_cbArea3Contour2.GetCurSel();
		THEAPP.m_pModelDataManager->m_AlgorithmParam[m_iImageIdx][m_iTabIdx].m_iInspectAreaContour2Margin[2] = m_iEditArea3Contour2Margin;
	}
	else		// SetParam
	{
		m_bCheckInspect = THEAPP.m_pModelDataManager->m_AlgorithmParam[m_iImageIdx][m_iTabIdx].m_bInspect;
		m_cbDefectType.SetCurSel(THEAPP.m_pModelDataManager->m_AlgorithmParam[m_iImageIdx][m_iTabIdx].m_iDefectNameIdx);

		m_bCheckArea1 = THEAPP.m_pModelDataManager->m_AlgorithmParam[m_iImageIdx][m_iTabIdx].m_bUseInspectArea[0];
		m_cbArea1Contour1.SetCurSel(THEAPP.m_pModelDataManager->m_AlgorithmParam[m_iImageIdx][m_iTabIdx].m_iInspectAreaContour1Number[0]);
		m_iEditArea1Contour1Margin = THEAPP.m_pModelDataManager->m_AlgorithmParam[m_iImageIdx][m_iTabIdx].m_iInspectAreaContour1Margin[0];
		m_cbArea1Contour2.SetCurSel(THEAPP.m_pModelDataManager->m_AlgorithmParam[m_iImageIdx][m_iTabIdx].m_iInspectAreaContour2Number[0]);
		m_iEditArea1Contour2Margin = THEAPP.m_pModelDataManager->m_AlgorithmParam[m_iImageIdx][m_iTabIdx].m_iInspectAreaContour2Margin[0];
		m_bCheckArea2 = THEAPP.m_pModelDataManager->m_AlgorithmParam[m_iImageIdx][m_iTabIdx].m_bUseInspectArea[1];
		m_cbArea2Contour1.SetCurSel(THEAPP.m_pModelDataManager->m_AlgorithmParam[m_iImageIdx][m_iTabIdx].m_iInspectAreaContour1Number[1]);
		m_iEditArea2Contour1Margin = THEAPP.m_pModelDataManager->m_AlgorithmParam[m_iImageIdx][m_iTabIdx].m_iInspectAreaContour1Margin[1];
		m_cbArea2Contour2.SetCurSel(THEAPP.m_pModelDataManager->m_AlgorithmParam[m_iImageIdx][m_iTabIdx].m_iInspectAreaContour2Number[1]);
		m_iEditArea2Contour2Margin = THEAPP.m_pModelDataManager->m_AlgorithmParam[m_iImageIdx][m_iTabIdx].m_iInspectAreaContour2Margin[1];
		m_bCheckArea3 = THEAPP.m_pModelDataManager->m_AlgorithmParam[m_iImageIdx][m_iTabIdx].m_bUseInspectArea[2];
		m_cbArea3Contour1.SetCurSel(THEAPP.m_pModelDataManager->m_AlgorithmParam[m_iImageIdx][m_iTabIdx].m_iInspectAreaContour1Number[2]);
		m_iEditArea3Contour1Margin = THEAPP.m_pModelDataManager->m_AlgorithmParam[m_iImageIdx][m_iTabIdx].m_iInspectAreaContour1Margin[2];
		m_cbArea3Contour2.SetCurSel(THEAPP.m_pModelDataManager->m_AlgorithmParam[m_iImageIdx][m_iTabIdx].m_iInspectAreaContour2Number[2]);
		m_iEditArea3Contour2Margin = THEAPP.m_pModelDataManager->m_AlgorithmParam[m_iImageIdx][m_iTabIdx].m_iInspectAreaContour2Margin[2];

		UpdateData(bFlag);
	}
}

BEGIN_MESSAGE_MAP(CTeachParamDlg_Tab, CDialog)
	ON_BN_CLICKED(IDC_BUTTON_ROI_UPDATE, &CTeachParamDlg_Tab::OnBnClickedButtonRoiUpdate)
	ON_BN_CLICKED(IDC_BUTTON_ALGORITHM, &CTeachParamDlg_Tab::OnBnClickedButtonAlgorithm)
	ON_BN_CLICKED(IDC_BUTTON_PARAMETER_TEST, &CTeachParamDlg_Tab::OnBnClickedButtonParameterTest)
END_MESSAGE_MAP()


// CTeachParamDlg_Tab 메시지 처리기입니다.


BOOL CTeachParamDlg_Tab::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_UsedAlgorithmList.AttachGrid(this, IDC_STATIC_ALGORITHM_LIST);

	m_bnUpdateROI.Init_Ctrl(_T("Arial"), 9, FALSE, BLACK, ROYAL_BLUE, 0, 0);
	m_bnParamTest.Init_Ctrl(_T("Arial"), 9, FALSE, BLACK, ROYAL_BLUE, 0, 0);
	m_bnAlgorithm.Init_Ctrl(_T("Arial"), 9, FALSE, BLACK, OLIVE_GREEN, 0, 0);

	m_LabelInspect.Init_Ctrl(_T("Arial Black"), 8, FALSE, WHITE, BLACK);
	m_LabelDefect.Init_Ctrl(_T("Arial Black"), 8, FALSE, WHITE, BLACK);
	m_LabelArea.Init_Ctrl(_T("Arial Black"), 8, FALSE, WHITE, BLACK);
	m_LabelAlgorithm.Init_Ctrl(_T("Arial Black"), 8, FALSE, WHITE, BLACK);
	m_LabelSpecialNG.Init_Ctrl(_T("Arial Black"), 8, FALSE, WHITE, BLACK);

	m_cbDefectType.Clear();
	for (int i = 0; i < MAX_DEFECT_NAME; i++)
	{
		m_cbDefectType.AddString(g_sDefectName[i]);
	}
	m_cbDefectType.SetCurSel(DEFECT_NAME_DIRT);

	for (int i = 0; i < MAX_CONTOUR_NUM; i++)
	{
		CString temp;
		temp.Format("%d", i + 1);

		m_cbArea1Contour1.AddString(temp);
		m_cbArea1Contour2.AddString(temp);
		m_cbArea2Contour1.AddString(temp);
		m_cbArea2Contour2.AddString(temp);
		m_cbArea3Contour1.AddString(temp);
		m_cbArea3Contour2.AddString(temp);
	}
	m_cbArea1Contour1.SetCurSel(MATCHING_CONTOUR_3);
	m_cbArea1Contour2.SetCurSel(MATCHING_CONTOUR_4);
	m_cbArea2Contour1.SetCurSel(MATCHING_CONTOUR_3);
	m_cbArea2Contour2.SetCurSel(MATCHING_CONTOUR_3);
	m_cbArea3Contour1.SetCurSel(MATCHING_CONTOUR_4);
	m_cbArea3Contour2.SetCurSel(MATCHING_CONTOUR_4);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}


void CTeachParamDlg_Tab::OnBnClickedButtonRoiUpdate()
{

	UpdateData(TRUE);

	CAlgorithmParam AlgorithmParam;

	AlgorithmParam.m_bUseInspectArea[0] = m_bCheckArea1;
	AlgorithmParam.m_iInspectAreaContour1Number[0] = m_cbArea1Contour1.GetCurSel();
	AlgorithmParam.m_iInspectAreaContour1Margin[0] = m_iEditArea1Contour1Margin;
	AlgorithmParam.m_iInspectAreaContour2Number[0] = m_cbArea1Contour2.GetCurSel();
	AlgorithmParam.m_iInspectAreaContour2Margin[0] = m_iEditArea1Contour2Margin;
	AlgorithmParam.m_bUseInspectArea[1] = m_bCheckArea2;
	AlgorithmParam.m_iInspectAreaContour1Number[1] = m_cbArea2Contour1.GetCurSel();
	AlgorithmParam.m_iInspectAreaContour1Margin[1] = m_iEditArea2Contour1Margin;
	AlgorithmParam.m_iInspectAreaContour2Number[1] = m_cbArea2Contour2.GetCurSel();
	AlgorithmParam.m_iInspectAreaContour2Margin[1] = m_iEditArea2Contour2Margin;
	AlgorithmParam.m_bUseInspectArea[2] = m_bCheckArea3;
	AlgorithmParam.m_iInspectAreaContour1Number[2] = m_cbArea3Contour1.GetCurSel();
	AlgorithmParam.m_iInspectAreaContour1Margin[2] = m_iEditArea3Contour1Margin;
	AlgorithmParam.m_iInspectAreaContour2Number[2] = m_cbArea3Contour2.GetCurSel();
	AlgorithmParam.m_iInspectAreaContour2Margin[2] = m_iEditArea3Contour2Margin;

	THEAPP.m_pAlgorithm->GetInspectArea(THEAPP.m_pInspectAdminViewDlg->ContoursAffinTrans[MATCHING_CONTOUR_1], THEAPP.m_pInspectAdminViewDlg->ContoursAffinTrans[MATCHING_CONTOUR_2],
		THEAPP.m_pInspectAdminViewDlg->ContoursAffinTrans[MATCHING_CONTOUR_3], THEAPP.m_pInspectAdminViewDlg->ContoursAffinTrans[MATCHING_CONTOUR_4], THEAPP.m_pInspectAdminViewDlg->ContoursAffinTrans[MATCHING_CONTOUR_5],
		AlgorithmParam, &(THEAPP.m_pInspectAdminViewDlg->m_HInspectAreaRgn));

	THEAPP.m_pInspectAdminViewDlg->UpdateView();
}


void CTeachParamDlg_Tab::OnBnClickedButtonAlgorithm()
{
	THEAPP.m_pTabControlDlg->ShowAlgorithmWnd(m_iImageIdx, m_iTabIdx);
}

void CTeachParamDlg_Tab::OnBnClickedButtonParameterTest()
{
	UpdateData(TRUE);

	CAlgorithmParam AlgorithmParam;
	AlgorithmParam = THEAPP.m_pModelDataManager->m_AlgorithmParam[m_iImageIdx][m_iTabIdx];

	AlgorithmParam.m_bUseInspectArea[0] = m_bCheckArea1;
	AlgorithmParam.m_iInspectAreaContour1Number[0] = m_cbArea1Contour1.GetCurSel();
	AlgorithmParam.m_iInspectAreaContour1Margin[0] = m_iEditArea1Contour1Margin;
	AlgorithmParam.m_iInspectAreaContour2Number[0] = m_cbArea1Contour2.GetCurSel();
	AlgorithmParam.m_iInspectAreaContour2Margin[0] = m_iEditArea1Contour2Margin;
	AlgorithmParam.m_bUseInspectArea[1] = m_bCheckArea2;
	AlgorithmParam.m_iInspectAreaContour1Number[1] = m_cbArea2Contour1.GetCurSel();
	AlgorithmParam.m_iInspectAreaContour1Margin[1] = m_iEditArea2Contour1Margin;
	AlgorithmParam.m_iInspectAreaContour2Number[1] = m_cbArea2Contour2.GetCurSel();
	AlgorithmParam.m_iInspectAreaContour2Margin[1] = m_iEditArea2Contour2Margin;
	AlgorithmParam.m_bUseInspectArea[2] = m_bCheckArea3;
	AlgorithmParam.m_iInspectAreaContour1Number[2] = m_cbArea3Contour1.GetCurSel();
	AlgorithmParam.m_iInspectAreaContour1Margin[2] = m_iEditArea3Contour1Margin;
	AlgorithmParam.m_iInspectAreaContour2Number[2] = m_cbArea3Contour2.GetCurSel();
	AlgorithmParam.m_iInspectAreaContour2Margin[2] = m_iEditArea3Contour2Margin;

	Hobject HEpoxyDontCareRgn;
	gen_empty_obj(&HEpoxyDontCareRgn);

	int iCurDefectType = m_cbDefectType.GetCurSel();
	if (iCurDefectType==DEFECT_NAME_EPOXY)
		THEAPP.m_pInspectAdminViewDlg->m_HParameterDefectRgn = THEAPP.m_pAlgorithm->CommonAlgorithm(THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[m_iImageIdx], THEAPP.m_pInspectAdminViewDlg->ContoursAffinTrans[MATCHING_CONTOUR_1], THEAPP.m_pInspectAdminViewDlg->ContoursAffinTrans[MATCHING_CONTOUR_2],
		THEAPP.m_pInspectAdminViewDlg->ContoursAffinTrans[MATCHING_CONTOUR_3], THEAPP.m_pInspectAdminViewDlg->ContoursAffinTrans[MATCHING_CONTOUR_4], THEAPP.m_pInspectAdminViewDlg->ContoursAffinTrans[MATCHING_CONTOUR_5],	AlgorithmParam, &HEpoxyDontCareRgn, &(THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[TRIGGER_EDGE]));
	else
		THEAPP.m_pInspectAdminViewDlg->m_HParameterDefectRgn = THEAPP.m_pAlgorithm->CommonAlgorithm(THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[m_iImageIdx], THEAPP.m_pInspectAdminViewDlg->ContoursAffinTrans[MATCHING_CONTOUR_1], THEAPP.m_pInspectAdminViewDlg->ContoursAffinTrans[MATCHING_CONTOUR_2],
		THEAPP.m_pInspectAdminViewDlg->ContoursAffinTrans[MATCHING_CONTOUR_3], THEAPP.m_pInspectAdminViewDlg->ContoursAffinTrans[MATCHING_CONTOUR_4], THEAPP.m_pInspectAdminViewDlg->ContoursAffinTrans[MATCHING_CONTOUR_5],	AlgorithmParam, &HEpoxyDontCareRgn);

	THEAPP.m_pInspectAdminViewDlg->UpdateView();
}

void CTeachParamDlg_Tab::UpdateList()
{
	if(THEAPP.m_pModelDataManager->m_sModelName == ".") return;

	CAlgorithmParam AlgorithmParam;
	AlgorithmParam = THEAPP.m_pModelDataManager->m_AlgorithmParam[m_iImageIdx][m_iTabIdx];

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