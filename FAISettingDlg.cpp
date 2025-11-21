// FAISettingDlg.cpp: 구현 파일
//

#include "stdafx.h"
#include "uScan.h"
#include "FAISettingDlg.h"
#include "afxdialogex.h"

// FAI 치수 측정 - LeeGW
// CFAISettingDlg 대화 상자

IMPLEMENT_DYNAMIC(CFAISettingDlg, CDialog)

CFAISettingDlg::CFAISettingDlg(CWnd* pParent /*=nullptr*/)
	: CDialog(IDD_FAI_SETTING_DLG, pParent)
{
	for (int i = 0; i < MAX_FAI_ITEM; i++)
	{
		m_dFAIMultiple[i] = 1.0;
		m_dFAIOffset[i] = 0.0;
		m_dFAISpecMin[i] = 0.0;
		m_dFAISpecMax[i] = 1.0;
		m_bFAINGUse[i] = FALSE;
		m_bFAISpecialNGUse[i] = FALSE;

		m_bFAIUniqueOffsetUse[i] = FALSE;

		for (int ii = 0; ii < MAX_CIRCULAR_TABLE_POCKET; ii++)
		{
			m_dFAIUniqueMultiple[i][ii] = 1.0;
			m_dFAIUniqueOffset[i][ii] = 0.0;
		}
	}

	m_iFAIInnerCircleFitType = 0;
	m_iFAIOuterCircleFitType = 0;
	m_iFAIDatumCircleFitType = 0;
	
	m_iFAISimulPocketNum = 0;
}

CFAISettingDlg::~CFAISettingDlg()
{
}

void CFAISettingDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	
	int iFAINum;
	// FAI-26

	iFAINum = 26;
	DDX_Text(pDX, IDC_EDIT_MULTIPLE_FAI_26, m_dFAIMultiple[iFAINum]);
	DDX_Text(pDX, IDC_EDIT_OFFSET_FAI_26, m_dFAIOffset[iFAINum]);
	DDX_Text(pDX, IDC_EDIT_SPEC_MIN_FAI_26, m_dFAISpecMin[iFAINum]);
	DDX_Text(pDX, IDC_EDIT_SPEC_MAX_FAI_26, m_dFAISpecMax[iFAINum]);
	DDX_Check(pDX, IDC_CHECK_USE_NG_FAI_26, m_bFAINGUse[iFAINum]);
	DDX_Check(pDX, IDC_CHECK_USE_SPECIAL_NG_FAI_26, m_bFAISpecialNGUse[iFAINum]);
	DDX_Check(pDX, IDC_CHECK_USE_UNIQUE_OFFSET_FAI_26, m_bFAIUniqueOffsetUse[iFAINum]);

	iFAINum = 27;
	DDX_Text(pDX, IDC_EDIT_MULTIPLE_FAI_27, m_dFAIMultiple[iFAINum]);
	DDX_Text(pDX, IDC_EDIT_OFFSET_FAI_27, m_dFAIOffset[iFAINum]);
	DDX_Text(pDX, IDC_EDIT_SPEC_MIN_FAI_27, m_dFAISpecMin[iFAINum]);
	DDX_Text(pDX, IDC_EDIT_SPEC_MAX_FAI_27, m_dFAISpecMax[iFAINum]);
	DDX_Check(pDX, IDC_CHECK_USE_NG_FAI_27, m_bFAINGUse[iFAINum]);
	DDX_Check(pDX, IDC_CHECK_USE_SPECIAL_NG_FAI_27, m_bFAISpecialNGUse[iFAINum]);
	DDX_Check(pDX, IDC_CHECK_USE_UNIQUE_OFFSET_FAI_27, m_bFAIUniqueOffsetUse[iFAINum]);
	
	iFAINum = 28;
	DDX_Text(pDX, IDC_EDIT_MULTIPLE_FAI_28, m_dFAIMultiple[iFAINum]);
	DDX_Text(pDX, IDC_EDIT_OFFSET_FAI_28, m_dFAIOffset[iFAINum]);
	DDX_Text(pDX, IDC_EDIT_SPEC_MIN_FAI_28, m_dFAISpecMin[iFAINum]);
	DDX_Text(pDX, IDC_EDIT_SPEC_MAX_FAI_28, m_dFAISpecMax[iFAINum]);
	DDX_Check(pDX, IDC_CHECK_USE_NG_FAI_28, m_bFAINGUse[iFAINum]);
	DDX_Check(pDX, IDC_CHECK_USE_SPECIAL_NG_FAI_28, m_bFAISpecialNGUse[iFAINum]);
	DDX_Check(pDX, IDC_CHECK_USE_UNIQUE_OFFSET_FAI_28, m_bFAIUniqueOffsetUse[iFAINum]);

	// FAI-51
	iFAINum = 51;
	DDX_Text(pDX, IDC_EDIT_MULTIPLE_FAI_51, m_dFAIMultiple[iFAINum]);
	DDX_Text(pDX, IDC_EDIT_OFFSET_FAI_51, m_dFAIOffset[iFAINum]);
	DDX_Text(pDX, IDC_EDIT_SPEC_MIN_FAI_51, m_dFAISpecMin[iFAINum]);
	DDX_Text(pDX, IDC_EDIT_SPEC_MAX_FAI_51, m_dFAISpecMax[iFAINum]);
	DDX_Check(pDX, IDC_CHECK_USE_NG_FAI_51, m_bFAINGUse[iFAINum]);
	DDX_Check(pDX, IDC_CHECK_USE_SPECIAL_NG_FAI_51, m_bFAISpecialNGUse[iFAINum]);
	DDX_Check(pDX, IDC_CHECK_USE_UNIQUE_OFFSET_FAI_51, m_bFAIUniqueOffsetUse[iFAINum]);

	iFAINum = 52;
	DDX_Text(pDX, IDC_EDIT_MULTIPLE_FAI_52, m_dFAIMultiple[iFAINum]);
	DDX_Text(pDX, IDC_EDIT_OFFSET_FAI_52, m_dFAIOffset[iFAINum]);
	DDX_Text(pDX, IDC_EDIT_SPEC_MIN_FAI_52, m_dFAISpecMin[iFAINum]);
	DDX_Text(pDX, IDC_EDIT_SPEC_MAX_FAI_52, m_dFAISpecMax[iFAINum]);
	DDX_Check(pDX, IDC_CHECK_USE_NG_FAI_52, m_bFAINGUse[iFAINum]);
	DDX_Check(pDX, IDC_CHECK_USE_SPECIAL_NG_FAI_52, m_bFAISpecialNGUse[iFAINum]);
	DDX_Check(pDX, IDC_CHECK_USE_UNIQUE_OFFSET_FAI_52, m_bFAIUniqueOffsetUse[iFAINum]);

	iFAINum = 53;
	DDX_Text(pDX, IDC_EDIT_MULTIPLE_FAI_53, m_dFAIMultiple[iFAINum]);
	DDX_Text(pDX, IDC_EDIT_OFFSET_FAI_53, m_dFAIOffset[iFAINum]);
	DDX_Text(pDX, IDC_EDIT_SPEC_MIN_FAI_53, m_dFAISpecMin[iFAINum]);
	DDX_Text(pDX, IDC_EDIT_SPEC_MAX_FAI_53, m_dFAISpecMax[iFAINum]);
	DDX_Check(pDX, IDC_CHECK_USE_NG_FAI_53, m_bFAINGUse[iFAINum]);
	DDX_Check(pDX, IDC_CHECK_USE_SPECIAL_NG_FAI_53, m_bFAISpecialNGUse[iFAINum]);
	DDX_Check(pDX, IDC_CHECK_USE_UNIQUE_OFFSET_FAI_53, m_bFAIUniqueOffsetUse[iFAINum]);

	// FAI-14
	iFAINum = 14;
	DDX_Text(pDX, IDC_EDIT_MULTIPLE_FAI_14, m_dFAIMultiple[iFAINum]);
	DDX_Text(pDX, IDC_EDIT_OFFSET_FAI_14, m_dFAIOffset[iFAINum]);
	DDX_Text(pDX, IDC_EDIT_SPEC_MIN_FAI_14, m_dFAISpecMin[iFAINum]);
	DDX_Text(pDX, IDC_EDIT_SPEC_MAX_FAI_14, m_dFAISpecMax[iFAINum]);
	DDX_Check(pDX, IDC_CHECK_USE_NG_FAI_14, m_bFAINGUse[iFAINum]);
	DDX_Check(pDX, IDC_CHECK_USE_SPECIAL_NG_FAI_14, m_bFAISpecialNGUse[iFAINum]);
	DDX_Check(pDX, IDC_CHECK_USE_UNIQUE_OFFSET_FAI_14, m_bFAIUniqueOffsetUse[iFAINum]);

	iFAINum = 15;
	DDX_Text(pDX, IDC_EDIT_MULTIPLE_FAI_15, m_dFAIMultiple[iFAINum]);
	DDX_Text(pDX, IDC_EDIT_OFFSET_FAI_15, m_dFAIOffset[iFAINum]);
	DDX_Text(pDX, IDC_EDIT_SPEC_MIN_FAI_15, m_dFAISpecMin[iFAINum]);
	DDX_Text(pDX, IDC_EDIT_SPEC_MAX_FAI_15, m_dFAISpecMax[iFAINum]);
	DDX_Check(pDX, IDC_CHECK_USE_NG_FAI_15, m_bFAINGUse[iFAINum]);
	DDX_Check(pDX, IDC_CHECK_USE_SPECIAL_NG_FAI_15, m_bFAISpecialNGUse[iFAINum]);
	DDX_Check(pDX, IDC_CHECK_USE_UNIQUE_OFFSET_FAI_15, m_bFAIUniqueOffsetUse[iFAINum]);

	iFAINum = 16;
	DDX_Text(pDX, IDC_EDIT_MULTIPLE_FAI_16, m_dFAIMultiple[iFAINum]);
	DDX_Text(pDX, IDC_EDIT_OFFSET_FAI_16, m_dFAIOffset[iFAINum]);
	DDX_Text(pDX, IDC_EDIT_SPEC_MIN_FAI_16, m_dFAISpecMin[iFAINum]);
	DDX_Text(pDX, IDC_EDIT_SPEC_MAX_FAI_16, m_dFAISpecMax[iFAINum]);
	DDX_Check(pDX, IDC_CHECK_USE_NG_FAI_16, m_bFAINGUse[iFAINum]);
	DDX_Check(pDX, IDC_CHECK_USE_SPECIAL_NG_FAI_16, m_bFAISpecialNGUse[iFAINum]);
	DDX_Check(pDX, IDC_CHECK_USE_UNIQUE_OFFSET_FAI_16, m_bFAIUniqueOffsetUse[iFAINum]);

	// Rotation
	iFAINum = 123;
	DDX_Text(pDX, IDC_EDIT_MULTIPLE_FAI_123, m_dFAIMultiple[iFAINum]);
	DDX_Text(pDX, IDC_EDIT_OFFSET_FAI_123, m_dFAIOffset[iFAINum]);
	DDX_Text(pDX, IDC_EDIT_SPEC_MIN_FAI_123, m_dFAISpecMin[iFAINum]);
	DDX_Text(pDX, IDC_EDIT_SPEC_MAX_FAI_123, m_dFAISpecMax[iFAINum]);
	DDX_Check(pDX, IDC_CHECK_USE_NG_FAI_123, m_bFAINGUse[iFAINum]);
	DDX_Check(pDX, IDC_CHECK_USE_SPECIAL_NG_FAI_123, m_bFAISpecialNGUse[iFAINum]);
	DDX_Check(pDX, IDC_CHECK_USE_UNIQUE_OFFSET_FAI_123, m_bFAIUniqueOffsetUse[iFAINum]);

	// EA
	iFAINum = 124;
	DDX_Text(pDX, IDC_EDIT_MULTIPLE_FAI_124, m_dFAIMultiple[iFAINum]);
	DDX_Text(pDX, IDC_EDIT_OFFSET_FAI_124, m_dFAIOffset[iFAINum]);
	DDX_Text(pDX, IDC_EDIT_SPEC_MIN_FAI_124, m_dFAISpecMin[iFAINum]);
	DDX_Text(pDX, IDC_EDIT_SPEC_MAX_FAI_124, m_dFAISpecMax[iFAINum]);
	DDX_Check(pDX, IDC_CHECK_USE_NG_FAI_124, m_bFAINGUse[iFAINum]);
	DDX_Check(pDX, IDC_CHECK_USE_SPECIAL_NG_FAI_124, m_bFAISpecialNGUse[iFAINum]);
	DDX_Check(pDX, IDC_CHECK_USE_UNIQUE_OFFSET_FAI_124, m_bFAIUniqueOffsetUse[iFAINum]);

	// FAI-125~130
	iFAINum = 125;
	DDX_Text(pDX, IDC_EDIT_MULTIPLE_FAI_125, m_dFAIMultiple[iFAINum]);
	DDX_Text(pDX, IDC_EDIT_OFFSET_FAI_125, m_dFAIOffset[iFAINum]);
	DDX_Text(pDX, IDC_EDIT_SPEC_MIN_FAI_125, m_dFAISpecMin[iFAINum]);
	DDX_Text(pDX, IDC_EDIT_SPEC_MAX_FAI_125, m_dFAISpecMax[iFAINum]);
	DDX_Check(pDX, IDC_CHECK_USE_NG_FAI_125, m_bFAINGUse[iFAINum]);
	DDX_Check(pDX, IDC_CHECK_USE_SPECIAL_NG_FAI_125, m_bFAISpecialNGUse[iFAINum]);
	DDX_Check(pDX, IDC_CHECK_USE_UNIQUE_OFFSET_FAI_125, m_bFAIUniqueOffsetUse[iFAINum]);

	iFAINum = 126;
	DDX_Text(pDX, IDC_EDIT_MULTIPLE_FAI_126, m_dFAIMultiple[iFAINum]);
	DDX_Text(pDX, IDC_EDIT_OFFSET_FAI_126, m_dFAIOffset[iFAINum]);
	DDX_Text(pDX, IDC_EDIT_SPEC_MIN_FAI_126, m_dFAISpecMin[iFAINum]);
	DDX_Text(pDX, IDC_EDIT_SPEC_MAX_FAI_126, m_dFAISpecMax[iFAINum]);
	DDX_Check(pDX, IDC_CHECK_USE_NG_FAI_126, m_bFAINGUse[iFAINum]);
	DDX_Check(pDX, IDC_CHECK_USE_SPECIAL_NG_FAI_126, m_bFAISpecialNGUse[iFAINum]);
	DDX_Check(pDX, IDC_CHECK_USE_UNIQUE_OFFSET_FAI_126, m_bFAIUniqueOffsetUse[iFAINum]);

	iFAINum = 127;
	DDX_Text(pDX, IDC_EDIT_MULTIPLE_FAI_127, m_dFAIMultiple[iFAINum]);
	DDX_Text(pDX, IDC_EDIT_OFFSET_FAI_127, m_dFAIOffset[iFAINum]);
	DDX_Text(pDX, IDC_EDIT_SPEC_MIN_FAI_127, m_dFAISpecMin[iFAINum]);
	DDX_Text(pDX, IDC_EDIT_SPEC_MAX_FAI_127, m_dFAISpecMax[iFAINum]);
	DDX_Check(pDX, IDC_CHECK_USE_NG_FAI_127, m_bFAINGUse[iFAINum]);
	DDX_Check(pDX, IDC_CHECK_USE_SPECIAL_NG_FAI_127, m_bFAISpecialNGUse[iFAINum]);
	DDX_Check(pDX, IDC_CHECK_USE_UNIQUE_OFFSET_FAI_127, m_bFAIUniqueOffsetUse[iFAINum]);

	iFAINum = 128;
	DDX_Text(pDX, IDC_EDIT_MULTIPLE_FAI_128, m_dFAIMultiple[iFAINum]);
	DDX_Text(pDX, IDC_EDIT_OFFSET_FAI_128, m_dFAIOffset[iFAINum]);
	DDX_Text(pDX, IDC_EDIT_SPEC_MIN_FAI_128, m_dFAISpecMin[iFAINum]);
	DDX_Text(pDX, IDC_EDIT_SPEC_MAX_FAI_128, m_dFAISpecMax[iFAINum]);
	DDX_Check(pDX, IDC_CHECK_USE_NG_FAI_128, m_bFAINGUse[iFAINum]);
	DDX_Check(pDX, IDC_CHECK_USE_SPECIAL_NG_FAI_128, m_bFAISpecialNGUse[iFAINum]);
	DDX_Check(pDX, IDC_CHECK_USE_UNIQUE_OFFSET_FAI_128, m_bFAIUniqueOffsetUse[iFAINum]);

	iFAINum = 129;
	DDX_Text(pDX, IDC_EDIT_MULTIPLE_FAI_129, m_dFAIMultiple[iFAINum]);
	DDX_Text(pDX, IDC_EDIT_OFFSET_FAI_129, m_dFAIOffset[iFAINum]);
	DDX_Text(pDX, IDC_EDIT_SPEC_MIN_FAI_129, m_dFAISpecMin[iFAINum]);
	DDX_Text(pDX, IDC_EDIT_SPEC_MAX_FAI_129, m_dFAISpecMax[iFAINum]);
	DDX_Check(pDX, IDC_CHECK_USE_NG_FAI_129, m_bFAINGUse[iFAINum]);
	DDX_Check(pDX, IDC_CHECK_USE_SPECIAL_NG_FAI_129, m_bFAISpecialNGUse[iFAINum]);
	DDX_Check(pDX, IDC_CHECK_USE_UNIQUE_OFFSET_FAI_129, m_bFAIUniqueOffsetUse[iFAINum]);

	iFAINum = 130;
	DDX_Text(pDX, IDC_EDIT_MULTIPLE_FAI_130, m_dFAIMultiple[iFAINum]);
	DDX_Text(pDX, IDC_EDIT_OFFSET_FAI_130, m_dFAIOffset[iFAINum]);
	DDX_Text(pDX, IDC_EDIT_SPEC_MIN_FAI_130, m_dFAISpecMin[iFAINum]);
	DDX_Text(pDX, IDC_EDIT_SPEC_MAX_FAI_130, m_dFAISpecMax[iFAINum]);
	DDX_Check(pDX, IDC_CHECK_USE_NG_FAI_130, m_bFAINGUse[iFAINum]);
	DDX_Check(pDX, IDC_CHECK_USE_SPECIAL_NG_FAI_130, m_bFAISpecialNGUse[iFAINum]);
	DDX_Check(pDX, IDC_CHECK_USE_UNIQUE_OFFSET_FAI_130, m_bFAIUniqueOffsetUse[iFAINum]);

	DDX_Radio(pDX, IDC_RADIO_INNER_CIRCLE_P8, m_iFAIInnerCircleFitType);
	DDX_Radio(pDX, IDC_RADIO_OUTER_CIRCLE_P8, m_iFAIOuterCircleFitType);
	DDX_Radio(pDX, IDC_RADIO_DATUM_CIRCLE_P8, m_iFAIDatumCircleFitType);

	DDX_Text(pDX, IDC_EDIT_FAI_SIMUL_POCKET_NUM, m_iFAISimulPocketNum);
}


BEGIN_MESSAGE_MAP(CFAISettingDlg, CDialog)
	ON_BN_CLICKED(IDOK, &CFAISettingDlg::OnBnClickedOk)
END_MESSAGE_MAP()


// CFAISettingDlg 메시지 처리기


void CFAISettingDlg::OnBnClickedOk()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	UpdateData();

	for (int i = 0; i < MAX_FAI_ITEM; i++)
	{
		if (THEAPP.m_bUseFAI[i] == FALSE)
			continue;

		THEAPP.m_pModelDataManager->m_dFAIMultiple[i] = m_dFAIMultiple[i];
		THEAPP.m_pModelDataManager->m_dFAIOffset[i] = m_dFAIOffset[i];
		THEAPP.m_pModelDataManager->m_dFAISpecMin[i] = m_dFAISpecMin[i];
		THEAPP.m_pModelDataManager->m_dFAISpecMax[i] = m_dFAISpecMax[i];
		THEAPP.m_pModelDataManager->m_bFAINGUse[i] = m_bFAINGUse[i];
		THEAPP.m_pModelDataManager->m_bFAISpecialNGUse[i] = m_bFAISpecialNGUse[i];

		THEAPP.m_pModelDataManager->m_bFAIUniqueOffsetUse[i] = m_bFAIUniqueOffsetUse[i];

		//for (int ii = 0; ii < MAX_CIRCULAR_TABLE_POCKET; ii++)
		//{
		//	THEAPP.m_pModelDataManager->m_dFAIUniqueMultiple[i][ii] = m_dFAIUniqueMultiple[i][ii];
		//	THEAPP.m_pModelDataManager->m_dFAIUniqueOffset[i][ii] = m_dFAIUniqueOffset[i][ii];
		//}
	}

	THEAPP.m_pModelDataManager->m_iFAIInnerCircleFitType = m_iFAIInnerCircleFitType;
	THEAPP.m_pModelDataManager->m_iFAIOuterCircleFitType = m_iFAIOuterCircleFitType;
	THEAPP.m_pModelDataManager->m_iFAIDatumCircleFitType = m_iFAIDatumCircleFitType;

	THEAPP.m_pModelDataManager->m_iFAISimulPocketNum = m_iFAISimulPocketNum;

	CString strModelFolder;
	if (!THEAPP.Struct_PreferenceStruct.m_bUseAbsolutePathModel) //Ver2629
		strModelFolder = THEAPP.GetWorkingDirectory() + "\\Model\\" + THEAPP.m_pModelDataManager->m_sModelName;
	else
		strModelFolder = THEAPP.GetWorkingEvmsDirectory() + "\\Model\\" + THEAPP.m_pModelDataManager->m_sModelName;

	CIniFileCS INI(strModelFolder + "\\SW\\InspectParam_FAI.ini");
	CString strSection, strTemp, strValue;

	strSection = "FAI_SET_OPTION";
	for (int i = 0; i < MAX_FAI_ITEM; i++)
	{
		if (THEAPP.m_bUseFAI[i] == FALSE)
			continue;

		strValue.Format("MULTIPLE_FAI_%d", i);
		INI.Set_Double(strSection, strValue, m_dFAIMultiple[i]);

		strValue.Format("OFFSET_FAI_%d", i);
		INI.Set_Double(strSection, strValue, m_dFAIOffset[i]);

		strValue.Format("SPEC_MIN_FAI_%d", i);
		INI.Set_Double(strSection, strValue, m_dFAISpecMin[i]);

		strValue.Format("SPEC_MAX_FAI_%d", i);
		INI.Set_Double(strSection, strValue, m_dFAISpecMax[i]);

		strValue.Format("USE_NG_FAI_%d", i);
		INI.Set_Bool(strSection, strValue, m_bFAINGUse[i]);

		strValue.Format("USE_SPECIAL_NG_FAI_%d", i);
		INI.Set_Bool(strSection, strValue, m_bFAISpecialNGUse[i]);

		strValue.Format("USE_UNIQUE_OFFSET_FAI_%d", i);
		INI.Set_Bool(strSection, strValue, m_bFAIUniqueOffsetUse[i]);

		//for (int ii = 0; ii < MAX_CIRCULAR_TABLE_POCKET; ii++)
		//{
		//	strValue.Format("MULTIPLE_FAI_%d_%d", i, ii + 1);
		//	INI.Set_Double(strSection, strValue, m_dFAIUniqueMultiple[i][ii]);

		//	strValue.Format("OFFSET_FAI_%d_%d", i, ii + 1);
		//	INI.Set_Double(strSection, strValue, m_dFAIUniqueOffset[i][ii]);
		//}
	}

	strValue.Format("FAI_INNER_CIRCLE_TYPE");
	INI.Set_Integer(strSection, strValue, m_iFAIInnerCircleFitType);

	strValue.Format("FAI_OUTER_CIRCLE_TYPE");
	INI.Set_Integer(strSection, strValue, m_iFAIOuterCircleFitType);

	strValue.Format("FAI_DATUM_CIRCLE_TYPE");
	INI.Set_Integer(strSection, strValue, m_iFAIDatumCircleFitType);

	strValue.Format("FAI_SIMUL_POCKET_NUM");
	INI.Set_Integer(strSection, strValue, m_iFAISimulPocketNum);

	AfxMessageBox("저장 완료", MB_SYSTEMMODAL);

	CDialog::OnOK();
}


BOOL CFAISettingDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.

	for (int i = 0; i < MAX_FAI_ITEM; i++)
	{
		m_dFAIMultiple[i] = THEAPP.m_pModelDataManager->m_dFAIMultiple[i];
		m_dFAIOffset[i] = THEAPP.m_pModelDataManager->m_dFAIOffset[i];
		m_dFAISpecMin[i] = THEAPP.m_pModelDataManager->m_dFAISpecMin[i];
		m_dFAISpecMax[i] = THEAPP.m_pModelDataManager->m_dFAISpecMax[i];
		m_bFAINGUse[i] = THEAPP.m_pModelDataManager->m_bFAINGUse[i];
		m_bFAISpecialNGUse[i] = THEAPP.m_pModelDataManager->m_bFAISpecialNGUse[i];

		m_bFAIUniqueOffsetUse[i] = THEAPP.m_pModelDataManager->m_bFAIUniqueOffsetUse[i];

		for (int ii = 0; ii < MAX_CIRCULAR_TABLE_POCKET; ii++)
		{
			m_dFAIUniqueMultiple[i][ii] = THEAPP.m_pModelDataManager->m_dFAIUniqueMultiple[i][ii];
			m_dFAIUniqueOffset[i][ii] = THEAPP.m_pModelDataManager->m_dFAIUniqueOffset[i][ii];
		}
	}
	m_iFAIInnerCircleFitType = THEAPP.m_pModelDataManager->m_iFAIInnerCircleFitType;
	m_iFAIOuterCircleFitType = THEAPP.m_pModelDataManager->m_iFAIOuterCircleFitType;
	m_iFAIDatumCircleFitType = THEAPP.m_pModelDataManager->m_iFAIDatumCircleFitType;

	m_iFAISimulPocketNum = THEAPP.m_pModelDataManager->m_iFAISimulPocketNum;

	UpdateData(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}