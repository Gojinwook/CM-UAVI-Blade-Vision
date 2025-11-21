// LightAutoCalResultDlg.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "uScan.h"
#include "LightAutoCalResultDlg.h"
#include "afxdialogex.h"


// CLightAutoCalResultDlg 대화 상자입니다.

CLightAutoCalResultDlg* CLightAutoCalResultDlg::m_pInstance = NULL;

CLightAutoCalResultDlg* CLightAutoCalResultDlg::GetInstance(BOOL bShowFlag)
{
	if (!m_pInstance) {
		m_pInstance = new CLightAutoCalResultDlg();
		if(!m_pInstance->m_hWnd)
		{
			CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
			m_pInstance->Create(IDD_LIGHT_AUTOCAL_RESULT_DLG, pFrame->GetActiveView());
			if (bShowFlag) m_pInstance->Show();
		}
	}

	return m_pInstance;
}

void CLightAutoCalResultDlg::Show()
{
	ShowWindow(SW_HIDE);
	MoveWindow(m_ScreenRect.left, m_ScreenRect.top, m_ScreenRect.right, m_ScreenRect.bottom);
	ShowWindow(SW_SHOW);
}

void CLightAutoCalResultDlg::Hide()
{
	ShowWindow(SW_HIDE);
}


void CLightAutoCalResultDlg::DeleteInstance()
{
	if(m_pInstance->GetSafeHwnd())
		m_pInstance->DestroyWindow();
	SAFE_DELETE(m_pInstance);
}

IMPLEMENT_DYNAMIC(CLightAutoCalResultDlg, CDialog)

CLightAutoCalResultDlg::CLightAutoCalResultDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CLightAutoCalResultDlg::IDD, pParent)
{

	m_iEditLightValueStart = 0;
	m_iEditLightValueEnd = 0;
	m_iEditLightValueInterval = 0;
	m_iEditLightValueInTol = 0;
	m_iEditTotalImageValueTol = 0;

	SetPosition(VIEW1_DLG3_LEFT+100+VIEW1_DLG3_WIDTH-600, VIEW1_DLG3_TOP+100, VIEW1_DLG3_WIDTH-570, VIEW1_DLG3_HEIGHT+280);
}

CLightAutoCalResultDlg::~CLightAutoCalResultDlg()
{
}

void CLightAutoCalResultDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	DDX_Control(pDX, IDOK, m_bnOK);
	DDX_Control(pDX, IDCANCEL, m_bnCancel);

	DDX_Text(pDX, IDC_EDIT_LIGHT_VALUE_START, m_iEditLightValueStart);
	DDX_Text(pDX, IDC_EDIT_LIGHT_VALUE_END, m_iEditLightValueEnd);
	DDX_Text(pDX, IDC_EDIT_LIGHT_VALUE_INTERVAL, m_iEditLightValueInterval);
	DDX_Text(pDX, IDC_EDIT_LIGHT_VALUE_TOL, m_iEditLightValueInTol);
	DDX_Text(pDX, IDC_EDIT_TOTALIMAGE_VALUE_TOL, m_iEditTotalImageValueTol);
	DDX_Control(pDX, IDC_PROGRESS_AUTO_CAL, m_ctrlProgressAutoCal);
	DDX_Control(pDX, IDC_BUTTON_LIGHT_CAL_TEST, m_bnAutoCalTestLight);
	DDX_Control(pDX, IDC_BUTTON_SAVE_SETTING, m_bnSaveSetting);

	DDX_Control(pDX, IDC_STATIC_TITLE_VALUE_1, m_LabelTitleValue_1);
	DDX_Control(pDX, IDC_STATIC_TITLE_LV_1, m_LabelTitleLV_1);
	DDX_Control(pDX, IDC_STATIC_TITLE_GV_1, m_LabelTitleGV_1);

	DDX_Control(pDX, IDC_STATIC_TITLE_VALUE_2, m_LabelTitleValue_2);
	DDX_Control(pDX, IDC_STATIC_TITLE_LV_2, m_LabelTitleLV_2);
	DDX_Control(pDX, IDC_STATIC_TITLE_GV_2, m_LabelTitleGV_2);

	DDX_Control(pDX, IDC_STATIC_TITLE_VALUE_3, m_LabelTitleValue_3);
	DDX_Control(pDX, IDC_STATIC_TITLE_LV_3, m_LabelTitleLV_3);
	DDX_Control(pDX, IDC_STATIC_TITLE_GV_3, m_LabelTitleGV_3);

	DDX_Control(pDX, IDC_STATIC_TITLE_CH1, m_LabelTitleCH1);
	DDX_Control(pDX, IDC_STATIC_TITLE_CH2, m_LabelTitleCH2);
	DDX_Control(pDX, IDC_STATIC_TITLE_CH3, m_LabelTitleCH3);
	DDX_Control(pDX, IDC_STATIC_TITLE_CH4, m_LabelTitleCH4);
	DDX_Control(pDX, IDC_STATIC_TITLE_CH5, m_LabelTitleCH5);
	DDX_Control(pDX, IDC_STATIC_TITLE_TOTAL, m_LabelTitleTotal);

	DDX_Control(pDX, IDC_STATIC_TEACH_CH1_LV, m_LabelTeachCH_LV[0]);
	DDX_Control(pDX, IDC_STATIC_TEACH_CH1_GV, m_LabelTeachCH_GV[0]);
	DDX_Control(pDX, IDC_STATIC_INSPECT_CH1_LV, m_LabelInspectCH_LV[0]);
	DDX_Control(pDX, IDC_STATIC_INSPECT_CH1_GV, m_LabelInspectCH_GV[0]);
	DDX_Control(pDX, IDC_STATIC_JUDGE_CH1, m_LabelJudgeCH[0]);

	DDX_Control(pDX, IDC_STATIC_TEACH_CH2_LV, m_LabelTeachCH_LV[1]);
	DDX_Control(pDX, IDC_STATIC_TEACH_CH2_GV, m_LabelTeachCH_GV[1]);
	DDX_Control(pDX, IDC_STATIC_INSPECT_CH2_LV, m_LabelInspectCH_LV[1]);
	DDX_Control(pDX, IDC_STATIC_INSPECT_CH2_GV, m_LabelInspectCH_GV[1]);
	DDX_Control(pDX, IDC_STATIC_JUDGE_CH2, m_LabelJudgeCH[1]);

	DDX_Control(pDX, IDC_STATIC_TEACH_CH3_LV, m_LabelTeachCH_LV[2]);
	DDX_Control(pDX, IDC_STATIC_TEACH_CH3_GV, m_LabelTeachCH_GV[2]);
	DDX_Control(pDX, IDC_STATIC_INSPECT_CH3_LV, m_LabelInspectCH_LV[2]);
	DDX_Control(pDX, IDC_STATIC_INSPECT_CH3_GV, m_LabelInspectCH_GV[2]);
	DDX_Control(pDX, IDC_STATIC_JUDGE_CH3, m_LabelJudgeCH[2]);

	DDX_Control(pDX, IDC_STATIC_TEACH_CH4_LV, m_LabelTeachCH_LV[3]);
	DDX_Control(pDX, IDC_STATIC_TEACH_CH4_GV, m_LabelTeachCH_GV[3]);
	DDX_Control(pDX, IDC_STATIC_INSPECT_CH4_LV, m_LabelInspectCH_LV[3]);
	DDX_Control(pDX, IDC_STATIC_INSPECT_CH4_GV, m_LabelInspectCH_GV[3]);
	DDX_Control(pDX, IDC_STATIC_JUDGE_CH4, m_LabelJudgeCH[3]);

	DDX_Control(pDX, IDC_STATIC_TEACH_CH5_LV, m_LabelTeachCH_LV[4]);
	DDX_Control(pDX, IDC_STATIC_TEACH_CH5_GV, m_LabelTeachCH_GV[4]);
	DDX_Control(pDX, IDC_STATIC_INSPECT_CH5_LV, m_LabelInspectCH_LV[4]);
	DDX_Control(pDX, IDC_STATIC_INSPECT_CH5_GV, m_LabelInspectCH_GV[4]);
	DDX_Control(pDX, IDC_STATIC_JUDGE_CH5, m_LabelJudgeCH[4]);

	DDX_Control(pDX, IDC_STATIC_TEACH_TOTAL_LV, m_LabelTeachTOTAL_LV);
	DDX_Control(pDX, IDC_STATIC_TEACH_TOTAL_GV, m_LabelTeachTOTAL_GV);
	DDX_Control(pDX, IDC_STATIC_INSPECT_TOTAL_LV, m_LabelInspectTOTAL_LV);
	DDX_Control(pDX, IDC_STATIC_INSPECT_TOTAL_GV, m_LabelInspectTOTAL_GV);
	DDX_Control(pDX, IDC_STATIC_JUDGE_TOTAL, m_LabelJudgeTOTAL);
}



BEGIN_MESSAGE_MAP(CLightAutoCalResultDlg, CDialog)
	ON_BN_CLICKED(IDC_BUTTON_SAVE_SETTING, &CLightAutoCalResultDlg::OnBnClickedButtonSaveSetting)
	ON_BN_CLICKED(IDC_BUTTON_LIGHT_CAL_TEST, &CLightAutoCalResultDlg::OnBnClickedButtonLightCalTest)
	ON_BN_CLICKED(IDOK, &CLightAutoCalResultDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CLightAutoCalResultDlg::OnBnClickedCancel)
END_MESSAGE_MAP()


// CLightAutoCalResultDlg 메시지 처리기입니다.


BOOL CLightAutoCalResultDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.
	m_bnOK.Init_Ctrl(_T("Arial"), 10, FALSE, BLACK, ROYAL_BLUE, 0, 0);
	m_bnCancel.Init_Ctrl(_T("Arial"), 10, FALSE, BLACK, ROYAL_BLUE, 0, 0);
	m_bnAutoCalTestLight.Init_Ctrl(_T("Arial"), 10, FALSE, BLACK, ROYAL_BLUE, 0, 0);
	m_bnSaveSetting.Init_Ctrl(_T("Arial"), 10, FALSE, BLACK, ROYAL_BLUE, 0, 0);

	m_ctrlProgressAutoCal.SetRange(0, 100);

	m_LabelTitleValue_1.Init_Ctrl(_T("Arial Black"), 10, FALSE, WHITE, BLACK);
	m_LabelTitleLV_1.Init_Ctrl(_T("Arial Black"), 10, FALSE, WHITE, BLACK);
	m_LabelTitleGV_1.Init_Ctrl(_T("Arial Black"), 10, FALSE, WHITE, BLACK);
	m_LabelTitleValue_2.Init_Ctrl(_T("Arial Black"), 10, FALSE, WHITE, BLACK);
	m_LabelTitleLV_2.Init_Ctrl(_T("Arial Black"), 10, FALSE, WHITE, BLACK);
	m_LabelTitleGV_2.Init_Ctrl(_T("Arial Black"), 10, FALSE, WHITE, BLACK);
	m_LabelTitleValue_3.Init_Ctrl(_T("Arial Black"), 10, FALSE, WHITE, BLACK);
	m_LabelTitleLV_3.Init_Ctrl(_T("Arial Black"), 10, FALSE, WHITE, BLACK);
	m_LabelTitleGV_3.Init_Ctrl(_T("Arial Black"), 10, FALSE, WHITE, BLACK);
	m_LabelTitleCH1.Init_Ctrl(_T("Arial Black"), 10, FALSE, WHITE, BLACK);
	m_LabelTitleCH2.Init_Ctrl(_T("Arial Black"), 10, FALSE, WHITE, BLACK);
	m_LabelTitleCH3.Init_Ctrl(_T("Arial Black"), 10, FALSE, WHITE, BLACK);
	m_LabelTitleCH4.Init_Ctrl(_T("Arial Black"), 10, FALSE, WHITE, BLACK);
	m_LabelTitleCH5.Init_Ctrl(_T("Arial Black"), 10, FALSE, WHITE, BLACK);
	m_LabelTitleTotal.Init_Ctrl(_T("Arial Black"), 10, FALSE, WHITE, BLACK);

	for (int i=0; i<MAX_LIGHT_CHANNEL; i++)
	{
		m_LabelTeachCH_LV[i].Init_Ctrl(_T("Arial Black"), 10, FALSE, BLACK, WHITE);
		m_LabelTeachCH_GV[i].Init_Ctrl(_T("Arial Black"), 10, FALSE, BLACK, WHITE);
		m_LabelInspectCH_LV[i].Init_Ctrl(_T("Arial Black"), 10, FALSE, BLACK, WHITE);
		m_LabelInspectCH_GV[i].Init_Ctrl(_T("Arial Black"), 10, FALSE, BLACK, WHITE);
		m_LabelJudgeCH[i].Init_Ctrl(_T("Arial Black"), 10, FALSE, BLACK, WHITE);

	}

	m_LabelTeachTOTAL_LV.Init_Ctrl(_T("Arial Black"), 10, FALSE, BLACK, WHITE);
	m_LabelTeachTOTAL_GV.Init_Ctrl(_T("Arial Black"), 10, FALSE, BLACK, WHITE);
	m_LabelInspectTOTAL_LV.Init_Ctrl(_T("Arial Black"), 10, FALSE, BLACK, WHITE);
	m_LabelInspectTOTAL_GV.Init_Ctrl(_T("Arial Black"), 10, FALSE, BLACK, WHITE);
	m_LabelJudgeTOTAL.Init_Ctrl(_T("Arial Black"), 10, FALSE, BLACK, WHITE);


	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}


void CLightAutoCalResultDlg::OnBnClickedButtonSaveSetting()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if (THEAPP.m_pModelDataManager->m_sModelName == ".") 
		return;

	UpdateData();

	CString strModelFolder;
	if(!THEAPP.Struct_PreferenceStruct.m_bUseAbsolutePathModel) //Ver2629
		strModelFolder = THEAPP.GetWorkingDirectory() + "\\Model\\" + THEAPP.m_pModelDataManager->m_sModelName;
	else
		strModelFolder = THEAPP.GetWorkingEvmsDirectory() + "\\Model\\" + THEAPP.m_pModelDataManager->m_sModelName;

	CString FileName;
	FileName = strModelFolder+"\\HW\\LightAutoCal.ini";

	CIniFileCS INI(FileName);
	CString strSection;

	int i;
	CString sTemp;

	strSection = "Auto Cal Setting";

	THEAPP.m_pModelDataManager->m_iLightValueStart[THEAPP.m_pTabControlDlg->m_iCurrentTab-1] = m_iEditLightValueStart;
	sTemp.Format("m_iLightValueStart_%d", THEAPP.m_pTabControlDlg->m_iCurrentTab-1);
	INI.Set_Integer(strSection, sTemp, THEAPP.m_pModelDataManager->m_iLightValueStart[THEAPP.m_pTabControlDlg->m_iCurrentTab-1]);

	THEAPP.m_pModelDataManager->m_iLightValueEnd[THEAPP.m_pTabControlDlg->m_iCurrentTab-1] = m_iEditLightValueEnd;
	sTemp.Format("m_iLightValueEnd_%d", THEAPP.m_pTabControlDlg->m_iCurrentTab-1);
	INI.Set_Integer(strSection, sTemp, THEAPP.m_pModelDataManager->m_iLightValueEnd[THEAPP.m_pTabControlDlg->m_iCurrentTab-1]);

	THEAPP.m_pModelDataManager->m_iLightValueInterval[THEAPP.m_pTabControlDlg->m_iCurrentTab-1] = m_iEditLightValueInterval;
	sTemp.Format("m_iLightValueInterval_%d", THEAPP.m_pTabControlDlg->m_iCurrentTab-1);
	INI.Set_Integer(strSection, sTemp, THEAPP.m_pModelDataManager->m_iLightValueInterval[THEAPP.m_pTabControlDlg->m_iCurrentTab-1]);

	THEAPP.m_pModelDataManager->m_iLightValueInTol[THEAPP.m_pTabControlDlg->m_iCurrentTab-1] = m_iEditLightValueInTol;
	sTemp.Format("m_iLightValueInTol_%d", THEAPP.m_pTabControlDlg->m_iCurrentTab-1);
	INI.Set_Integer(strSection, sTemp, THEAPP.m_pModelDataManager->m_iLightValueInTol[THEAPP.m_pTabControlDlg->m_iCurrentTab-1]);

	THEAPP.m_pModelDataManager->m_iTotalImageValueTol[THEAPP.m_pTabControlDlg->m_iCurrentTab-1] = m_iEditTotalImageValueTol;
	sTemp.Format("m_iTotalImageValueTol_%d", THEAPP.m_pTabControlDlg->m_iCurrentTab-1);
	INI.Set_Integer(strSection, sTemp, THEAPP.m_pModelDataManager->m_iTotalImageValueTol[THEAPP.m_pTabControlDlg->m_iCurrentTab-1]);
}


void CLightAutoCalResultDlg::OnBnClickedButtonLightCalTest()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	THEAPP.m_pAutoCalService->InitAutoCalResult();
	THEAPP.m_pAutoCalService->AutoCalLightStart(THEAPP.m_pTabControlDlg->m_iCurrentTab-1);
}


void CLightAutoCalResultDlg::OnBnClickedOk()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CDialog::OnOK();
}


void CLightAutoCalResultDlg::OnBnClickedCancel()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CDialog::OnCancel();
}

void CLightAutoCalResultDlg::LoadViewParam()
{
	if(THEAPP.m_pModelDataManager->m_sModelName == ".") 
		return;

	m_iEditLightValueStart = THEAPP.m_pModelDataManager->m_iLightValueStart[THEAPP.m_pTabControlDlg->m_iCurrentTab-1];
	m_iEditLightValueEnd = THEAPP.m_pModelDataManager->m_iLightValueEnd[THEAPP.m_pTabControlDlg->m_iCurrentTab-1];
	m_iEditLightValueInterval = THEAPP.m_pModelDataManager->m_iLightValueInterval[THEAPP.m_pTabControlDlg->m_iCurrentTab-1];
	m_iEditLightValueInTol = THEAPP.m_pModelDataManager->m_iLightValueInTol[THEAPP.m_pTabControlDlg->m_iCurrentTab-1];
	m_iEditTotalImageValueTol = THEAPP.m_pModelDataManager->m_iTotalImageValueTol[THEAPP.m_pTabControlDlg->m_iCurrentTab-1];

	UpdateData(FALSE);
}

void CLightAutoCalResultDlg::UpdateResultView()
{
	CString sTemp,sAutoCalResultLog;
	sAutoCalResultLog = _T("");
	
	for (int i=0; i<MAX_LIGHT_CHANNEL; i++)
	{
		if(THEAPP.m_pAutoCalService->m_bAutoCal_Done[i])
		{
			sTemp.Format("영상 %d",i);
			sTemp+="\t";
			sAutoCalResultLog+=sTemp;

			sTemp.Format("%d", THEAPP.m_pAutoCalService->m_iTeachCH_LV[i]);
			m_LabelTeachCH_LV[i].SetWindowText(sTemp);
			sTemp+="\t";
			sAutoCalResultLog+=sTemp;
			sTemp.Format("%d", THEAPP.m_pAutoCalService->m_iTeachCH_GV[i]);
			m_LabelTeachCH_GV[i].SetWindowText(sTemp);
			sTemp+="\t";
			sAutoCalResultLog+=sTemp;
			sTemp.Format("%d", THEAPP.m_pAutoCalService->m_iInspectCH_LV[i]);
			m_LabelInspectCH_LV[i].SetWindowText(sTemp);
			sTemp+="\t";
			sAutoCalResultLog+=sTemp;
			sTemp.Format("%d", THEAPP.m_pAutoCalService->m_iInspectCH_GV[i]);
			m_LabelInspectCH_GV[i].SetWindowText(sTemp);
			sTemp+="\t";
			sAutoCalResultLog+=sTemp;
			if(THEAPP.m_pAutoCalService->m_bJudgeCH[i])
			{
				m_LabelJudgeCH[i].Init_Ctrl(_T("Arial Black"), 10, FALSE, WHITE, BLACK);
				m_LabelJudgeCH[i].SetWindowText("G");
				sTemp+="G\t";
				sAutoCalResultLog+=sTemp;
			}
			else
			{
				m_LabelJudgeCH[i].Init_Ctrl(_T("Arial Black"), 10, FALSE, WHITE, BLACK);
				m_LabelJudgeCH[i].SetWindowText("NG");
				sTemp+="NG\t";
				sAutoCalResultLog+=sTemp;
			}	
		}
	}

	sTemp.Format("%d", THEAPP.m_pAutoCalService->m_iTeachTOTAL_LV);
	m_LabelTeachTOTAL_LV.SetWindowText(sTemp);
	sTemp+="\t";
	sAutoCalResultLog+=sTemp;
	sTemp.Format("%d", THEAPP.m_pAutoCalService->m_iTeachTOTAL_GV);
	m_LabelTeachTOTAL_GV.SetWindowText(sTemp);
	sTemp+="\t";
	sAutoCalResultLog+=sTemp;
	sTemp.Format("%d", THEAPP.m_pAutoCalService->m_iInspectTOTAL_LV);
	m_LabelInspectTOTAL_LV.SetWindowText(sTemp);
	sTemp+="\t";
	sAutoCalResultLog+=sTemp;
	sTemp.Format("%d", THEAPP.m_pAutoCalService->m_iInspectTOTAL_GV);
	m_LabelInspectTOTAL_GV.SetWindowText(sTemp);
	sTemp+="\t";
	sAutoCalResultLog+=sTemp;
	if(THEAPP.m_pAutoCalService->m_bJudgeTOTAL)
	{
		m_LabelJudgeTOTAL.Init_Ctrl(_T("Arial Black"), 10, FALSE, WHITE, BLACK);
		m_LabelJudgeTOTAL.SetWindowText("G");
		sTemp+="G";
		sAutoCalResultLog+=sTemp;
	}
	else
	{
		m_LabelJudgeTOTAL.Init_Ctrl(_T("Arial Black"), 10, FALSE, RED, BLACK);
		m_LabelJudgeTOTAL.SetWindowText("NG");
		sTemp+="NG";
		sAutoCalResultLog+=sTemp;
	}

	//THEAPP.SaveAutoCalLightResultLog(sAutoCalResultLog, THEAPP.m_pModelDataManager->GetModelIdx());
	//sColorLog.Format("%s\t%s\t%s\t%d\t%d\t%s\t%d\t%d\t%d\t%d\t%d\t%d",sDay,sTime,THEAPP.m_pInspectResultDlg->m_sCurrentLotID, iTrayNo, 
	//				iModuleNo, sCamName, iImageIdx, iColorInfpROIID, iAvgValueR,iAvgValueG,iAvgValueB,iAvgValueI);
}