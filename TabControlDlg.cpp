// TabControlDlg.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "uScan.h"
#include "TabControlDlg.h"
#include "afxdialogex.h"

// CTabControlDlg 대화 상자입니다.
CTabControlDlg* CTabControlDlg::m_pInstance = NULL;

CTabControlDlg* CTabControlDlg::GetInstance(BOOL bShowFlag)
{
	if (!m_pInstance) {
		m_pInstance = new CTabControlDlg();
		
		if(!m_pInstance->m_hWnd) {
			CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
			m_pInstance->Create(IDD_TEACHING_TAB_DLG, pFrame->GetActiveView());
			
			if (bShowFlag) m_pInstance->Show();
		}
	}
	return m_pInstance;
}

void CTabControlDlg::DeleteInstance()
{
	m_pThresholdHistogramDlg->DeleteInstance();
	m_pLightControlDlg->DeleteInstance();
	m_pJogSetDlg->DeleteInstance();
	m_pInspectionConditionTabDlg->DeleteInstance();
	m_pMatchingDlg->DeleteInstance();
	m_pAutoFocusDlg->DeleteInstance();

	for (int i = 0; i < MAX_IMAGE_TAB; i++)
	{
		if (i >= THEAPP.m_iMaxInspImageNo)
			break;

		SAFE_DELETE(m_pTeachParamDlg[i]);
	}

	m_pLightAutoCalResultDlg->DeleteInstance();

	if(m_pInstance->GetSafeHwnd())
		m_pInstance->DestroyWindow();
	SAFE_DELETE(m_pInstance);
}

void CTabControlDlg::Show()
{
	ShowWindow(SW_HIDE);
	MoveWindow(m_ScreenRect.left, m_ScreenRect.top, m_ScreenRect.right, m_ScreenRect.bottom);
	ShowWindow(SW_SHOW);
}
IMPLEMENT_DYNAMIC(CTabControlDlg, CDialog)

CTabControlDlg::CTabControlDlg(CWnd* pParent /*=NULL*/)
: CDialog(CTabControlDlg::IDD, pParent)
{
	SetPosition(VIEW1_DLG3_LEFT+60, VIEW1_DLG3_TOP+30, VIEW1_DLG3_WIDTH+830, VIEW1_DLG3_HEIGHT+820);
	m_iCurrentTab = 0;

	m_pAlgorithmDlg = NULL;
	m_pROIAlgorithmParamDlg = NULL;
}

CTabControlDlg::~CTabControlDlg()
{
}

void CTabControlDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC__TEACHING_TAB, m_TabControl);
}


BEGIN_MESSAGE_MAP(CTabControlDlg, CDialog)
	ON_NOTIFY(TCN_SELCHANGE, IDC__TEACHING_TAB, &CTabControlDlg::OnSelchangeTeachingTab)
	ON_WM_DESTROY()
	ON_MESSAGE(UM_INIT_TAB_CONTROL_DLG_DONE, &CTabControlDlg::OnTeachingTabDlgInitDone)
END_MESSAGE_MAP()

// CTabControlDlg 메시지 처리기입니다.

BOOL CTabControlDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	
	m_iCurrentTab = 0;
	m_TabControl.InsertItem(0,"기본정보/Model");

	m_pThresholdHistogramDlg = CThresholdHistogramDlg::GetInstance();
	m_pLightControlDlg = CLightControlDlg::GetInstance();
	m_pJogSetDlg = CJogSetDlg::GetInstance();
	m_pInspectionConditionTabDlg = CInspectionConditionTabDlg::GetInstance();
	m_pMatchingDlg = CMatchingDlg::GetInstance();
	m_pAutoFocusDlg = CAutoFocusDlg::GetInstance();


	m_pAlgorithmDlg = CAlgorithmDlg::GetInstance();
	HideAlgorithmDlg();

	// 24.02.28 Local Align 추가 - LeeGW Start
	m_pROIAlgorithmParamDlg = CROIAlgorithmTabDlg::GetInstance();
	HideROIAlgorithmParamDlg();
	// 24.02.28 Local Align 추가 - LeeGW End

	m_pLightAutoCalResultDlg = CLightAutoCalResultDlg::GetInstance();
	HideLightAutoCalResultDlg();

	// PostMessage(UM_INIT_TAB_CONTROL_DLG_DONE, 0, 0);
	SendMessage(UM_INIT_TAB_CONTROL_DLG_DONE, 0, 0);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

afx_msg LRESULT CTabControlDlg::OnTeachingTabDlgInitDone(WPARAM wParam, LPARAM lParam)
{
	// 검사유형 통합을 위한 동적생성 - LeeGW
	for (int i = 1; i < MAX_IMAGE_TAB + 1; i++)
	{
		if (i >= THEAPP.m_iMaxInspImageNo+1)
			break;

		m_TabControl.InsertItem(i, g_sImageName[THEAPP.m_iMachineInspType][i-1]);
	}

	// 검사유형 통합을 위한 동적생성 - LeeGW
	for (int i = 0; i < MAX_IMAGE_TAB; i++)
	{
		if (i >= THEAPP.m_iMaxInspImageNo)
			break;

		m_pTeachParamDlg[i] = new CTeachParamDlg();
		m_pTeachParamDlg[i]->SetImageIdx(i);
		m_pTeachParamDlg[i]->SetDelegate();
	}

	return 0;
}

void CTabControlDlg::ShowFirstPage()
{
	try{

		if (THEAPP.m_pModelDataManager->m_bOneBarrel == TRUE)
		{
			m_pMatchingDlg->GetDlgItem(IDC_MATCH_EDIT_RADIUS1)->EnableWindow(FALSE);
			m_pMatchingDlg->GetDlgItem(IDC_MATCH_EDIT_RADIUS2)->EnableWindow(FALSE);
			m_pMatchingDlg->GetDlgItem(IDC_MATCH_EDIT_RADIUS_23)->EnableWindow(FALSE);
		}
		else
		{
			m_pMatchingDlg->GetDlgItem(IDC_MATCH_EDIT_RADIUS1)->EnableWindow(TRUE);
			m_pMatchingDlg->GetDlgItem(IDC_MATCH_EDIT_RADIUS2)->EnableWindow(TRUE);
			m_pMatchingDlg->GetDlgItem(IDC_MATCH_EDIT_RADIUS_23)->EnableWindow(TRUE);
		}

		if (THEAPP.m_pInspectSummary->m_bUseAccelMatching == FALSE)
			m_pMatchingDlg->GetDlgItem(IDC_MATCH_EDIT_RADIUS_23)->EnableWindow(FALSE);
		else
			m_pMatchingDlg->GetDlgItem(IDC_MATCH_EDIT_RADIUS_23)->EnableWindow(TRUE);

		m_pThresholdHistogramDlg->Show();
		m_pThresholdHistogramDlg->SetImage(THEAPP.m_pInspectAdminViewDlg->m_pHImage);
		m_pThresholdHistogramDlg->UpdateBar(0,1);
		m_pLightControlDlg->ShowWindow(SW_HIDE);
		m_pJogSetDlg->Show();
		m_pMatchingDlg->Show();
		m_pAutoFocusDlg->Show();
		m_iCurrentTab = 0;

		m_pLightControlDlg->UpdateLightValue(m_iCurrentTab);
	
		int i;
		for (i=0; i<MAX_IMAGE_TAB; i++)
			gen_empty_obj(THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage+i);

		gen_empty_obj(&THEAPP.m_pInspectAdminViewDlg->m_pHImage);

		CString strModelName, strFolder, strImageFolder, strImageName;

		if(THEAPP.m_pModelDataManager->m_sModelName != ".") 
		{
			strModelName = THEAPP.m_pModelDataManager->m_sModelName;
			if(!THEAPP.Struct_PreferenceStruct.m_bUseAbsolutePathModel) //Ver2629
				strFolder = THEAPP.GetWorkingDirectory()+"\\Model\\";
			else
				strFolder = THEAPP.GetWorkingEvmsDirectory()+"\\Model\\";

			strImageFolder = strFolder+strModelName + "\\SW\\TeachImage\\";

			THEAPP.m_FileBase.CreatePath(strImageFolder ,TRUE); //Ver2629

			// 검사유형 통합을 위한 동적생성 - LeeGW
			for (int k = 0; k < MAX_IMAGE_TAB; k++)
			{
				if (k >= THEAPP.m_iMaxInspImageNo)
					break;

				strImageName.Format("Grab_%s.bmp", g_sImageName[THEAPP.m_iMachineInspType][k]);
				try
				{
					read_image(&THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[k], strImageFolder + strImageName);
				}
				catch (HException& e)
				{
					;
				}
			}
			
			//0523 1st Image
			if (THEAPP.m_pModelDataManager->m_iBlackMatchingImageType==BLACK_MATCHING_IMAGE_TYPE_BSURFACE)
			{
				if (THEAPP.m_pGFunction->ValidHImage(THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[TRIGGER_SURFACE]))
					copy_obj (THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[TRIGGER_SURFACE], &THEAPP.m_pInspectAdminViewDlg->m_pHImage, 1, 1);
			}
			else if (THEAPP.m_pModelDataManager->m_iBlackMatchingImageType==BLACK_MATCHING_IMAGE_TYPE_BEDGE)
			{
				if (THEAPP.m_pGFunction->ValidHImage(THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[TRIGGER_EDGE]))
					copy_obj(THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[TRIGGER_EDGE], &THEAPP.m_pInspectAdminViewDlg->m_pHImage, 1, 1);
			}
			else if (THEAPP.m_pModelDataManager->m_iBlackMatchingImageType==BLACK_MATCHING_IMAGE_TYPE_LENS1)
			{
				if (THEAPP.m_pGFunction->ValidHImage(THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[TRIGGER_LENS1]))
					copy_obj(THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[TRIGGER_LENS1], &THEAPP.m_pInspectAdminViewDlg->m_pHImage, 1, 1);
			}
			else if (THEAPP.m_pModelDataManager->m_iBlackMatchingImageType==BLACK_MATCHING_IMAGE_TYPE_LENS2)
			{
				if (THEAPP.m_pGFunction->ValidHImage(THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[TRIGGER_LENS2]))
					copy_obj(THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[TRIGGER_LENS2], &THEAPP.m_pInspectAdminViewDlg->m_pHImage, 1, 1);
			}

			THEAPP.m_pInspectAdminViewDlg->InvalidateRect(false);
		}
		
		// Image Align

		//BOOL bMatchingSuccess = FALSE;
		BOOL bGlobalAlignExist = FALSE;
		int iMatchingFailImageIndex = -1;

		double dRotationAngleDeg = 0.0;
		double dDeltaX = 0.0;
		double dDeltaY = 0.0;
		
		if (THEAPP.m_bShowReviewWindow)
		{
			for (i=0; i<MAX_IMAGE_TAB; i++)
			{
				if (i >= THEAPP.m_iMaxInspImageNo)
					break;

				if (THEAPP.m_pModelDataManager->m_bUseLocalAlignMatching[i]==TRUE)
				{
					bGlobalAlignExist = TRUE;

					THEAPP.m_pAlgorithm->m_bMatchingSuccess = THEAPP.m_pAlgorithm->ImageAlignShapeMatching(
						THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[i],
						THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage,
						&(THEAPP.m_pModelDataManager->m_lLAlignModelID[i]),
						THEAPP.m_pModelDataManager->m_bLocalAlignImage[i],
						THEAPP.m_pModelDataManager->m_iLocalMatchingAngleRange[i],
						THEAPP.m_pModelDataManager->m_dLocalMatchingScaleMin[i],
						THEAPP.m_pModelDataManager->m_dLocalMatchingScaleMax[i],
						THEAPP.m_pModelDataManager->m_dLocalMatchingScore[i],
						THEAPP.m_pModelDataManager->m_dLocalTeachAlignRefX[i],
						THEAPP.m_pModelDataManager->m_dLocalTeachAlignRefY[i],
						THEAPP.m_pModelDataManager->m_iLocalMatchingMethod[i],
						THEAPP.m_pModelDataManager->m_iLocalMatchingTeachingRectLTX[i],
						THEAPP.m_pModelDataManager->m_iLocalMatchingTeachingRectLTY[i],
						THEAPP.m_pModelDataManager->m_iLocalMatchingTeachingRectRBX[i],
						THEAPP.m_pModelDataManager->m_iLocalMatchingTeachingRectRBY[i],
						THEAPP.m_pModelDataManager->m_iLocalMatchingSearchMarginX[i],
						THEAPP.m_pModelDataManager->m_iLocalMatchingSearchMarginY[i],
						THEAPP.m_pModelDataManager->m_bUseMatchingAffineConstant[i],
						&dRotationAngleDeg,
						&dDeltaX,
						&dDeltaY);

					if (THEAPP.m_pAlgorithm->m_bMatchingSuccess == FALSE)
					{
						iMatchingFailImageIndex = i;
						break;
					}
				}
			}

			if (bGlobalAlignExist)
			{
				if (THEAPP.m_pAlgorithm->m_bMatchingSuccess)
					THEAPP.SaveLog("매칭 성공");
				else
				{
					CString sMsg;
					sMsg.Format("매칭 실패: 영상 탭 %d번", iMatchingFailImageIndex + 1);
					AfxMessageBox(sMsg, MB_SYSTEMMODAL);
				}
			}

			CString TeachImageFileName, strTeachFileFullName;

			if (bGlobalAlignExist && THEAPP.m_pAlgorithm->m_bMatchingSuccess)	// Align 이후 결과 이미지를 매칭 성공 시에만 티칭 이미지 파일로 저장
			{
				for (i=0; i<MAX_IMAGE_TAB; i++)
				{
					if (i >= THEAPP.m_iMaxInspImageNo)
						break;

					if (THEAPP.m_pGFunction->ValidHImage(THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[i])==FALSE)
						continue;

					CString sFileName;
					if(!THEAPP.Struct_PreferenceStruct.m_bUseAbsolutePathModel) //Ver2629
						sFileName.Format("%s\\Model\\%s\\SW\\TeachImage\\Grab_%s", THEAPP.GetWorkingDirectory(), THEAPP.m_pModelDataManager->m_sModelName, g_sImageName[THEAPP.m_iMachineInspType][i]);
					else
						sFileName.Format("%s\\Model\\%s\\SW\\TeachImage\\Grab_%s", THEAPP.GetWorkingEvmsDirectory(), THEAPP.m_pModelDataManager->m_sModelName, g_sImageName[THEAPP.m_iMachineInspType][i]);
						
					write_image(THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[i], "bmp", 0, sFileName);
					
				}

				//0523 1st Image
				if (THEAPP.m_pModelDataManager->m_iBlackMatchingImageType == BLACK_MATCHING_IMAGE_TYPE_BSURFACE)
				{
					if (THEAPP.m_pGFunction->ValidHImage(THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[TRIGGER_SURFACE]))
						copy_obj(THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[TRIGGER_SURFACE], &THEAPP.m_pInspectAdminViewDlg->m_pHImage, 1, 1);
				}
				else if (THEAPP.m_pModelDataManager->m_iBlackMatchingImageType == BLACK_MATCHING_IMAGE_TYPE_BEDGE)
				{
					if (THEAPP.m_pGFunction->ValidHImage(THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[TRIGGER_EDGE]))
						copy_obj(THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[TRIGGER_EDGE], &THEAPP.m_pInspectAdminViewDlg->m_pHImage, 1, 1);
				}
				else if (THEAPP.m_pModelDataManager->m_iBlackMatchingImageType == BLACK_MATCHING_IMAGE_TYPE_LENS1)
				{
					if (THEAPP.m_pGFunction->ValidHImage(THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[TRIGGER_LENS1]))
						copy_obj(THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[TRIGGER_LENS1], &THEAPP.m_pInspectAdminViewDlg->m_pHImage, 1, 1);
				}
				else if (THEAPP.m_pModelDataManager->m_iBlackMatchingImageType == BLACK_MATCHING_IMAGE_TYPE_LENS2)
				{
					if (THEAPP.m_pGFunction->ValidHImage(THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[TRIGGER_LENS2]))
						copy_obj(THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[TRIGGER_LENS2], &THEAPP.m_pInspectAdminViewDlg->m_pHImage, 1, 1);
				}
			}
		}

		if (THEAPP.m_iMachineInspType != MACHINE_WELDING)
		{
			// 24.04.04 - v2645 - 이미지별 매칭 디스플레이 - LeeGW Start
			double dContourCenterX, dContourCenterY;
			dContourCenterX = dContourCenterY = -1;

			THEAPP.m_pAlgorithm->m_iBlackMatchingImageType = THEAPP.m_pModelDataManager->m_iBlackMatchingImageType;

			THEAPP.m_pAlgorithm->m_bMatchingSuccess = THEAPP.m_pAlgorithm->MatchingAlgorithm(  // 매칭 
				THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[TRIGGER_SURFACE],
				THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[TRIGGER_LENS1],
				THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[TRIGGER_EDGE],
				THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[TRIGGER_LENS2],
				&THEAPP.m_pModelDataManager->ContoursOri[MATCHING_CONTOUR_1],
				&THEAPP.m_pModelDataManager->ContoursOri[MATCHING_CONTOUR_2],
				&THEAPP.m_pModelDataManager->ContoursOri[MATCHING_CONTOUR_3],
				&THEAPP.m_pModelDataManager->ContoursOri[MATCHING_CONTOUR_4],
				&THEAPP.m_pModelDataManager->ContoursOri[MATCHING_CONTOUR_5],
				&THEAPP.m_pModelDataManager->ModelID[MATCHING_MODEL_1],
				&THEAPP.m_pModelDataManager->ModelID[MATCHING_MODEL_2],
				&THEAPP.m_pModelDataManager->ModelID[MATCHING_MODEL_3],
				&THEAPP.m_pModelDataManager->ModelID[MATCHING_MODEL_4],
				&dContourCenterX, &dContourCenterY);

			if (THEAPP.m_pAlgorithm->m_bMatchingSuccess == TRUE)
			{
				Hobject HMatchingContour;

				for (int imgIdx = 0; imgIdx < BARREL_LENS_IMAGE_TAB; imgIdx++)
				{
					for (int cont = 0; cont < MAX_CONTOUR_NUM; cont++)
					{
						if (THEAPP.m_pModelDataManager->m_bOneBarrel && cont < MATCHING_CONTOUR_3)
						{
							THEAPP.m_pInspectAdminViewDlg->Contours[imgIdx][cont].Reset();
							continue;
						}
						copy_obj(THEAPP.m_pModelDataManager->ContoursOri[cont], &THEAPP.m_pInspectAdminViewDlg->Contours[imgIdx][cont], 1, -1);
					}


					// 24.07.03 - v2651 - ROI Templet Matching Align 추가 - LeeGW START
					// Template ROI Align
					if (THEAPP.m_pModelDataManager->m_bUseBLROIAlign[imgIdx] == TRUE)
					{
						if (THEAPP.m_pGFunction->ValidHImage(THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[imgIdx]) == FALSE)
							continue;

						for (int cont = 0; cont < MAX_CONTOUR_NUM; cont++)
						{
							if (THEAPP.m_pModelDataManager->m_bUseBLROIAlignContour[imgIdx][cont] == TRUE)
							{
								if (THEAPP.m_pModelDataManager->m_iBLROIAlignMatchingModel[imgIdx][cont] < 0)
									continue;

								copy_obj(THEAPP.m_pInspectAdminViewDlg->Contours[imgIdx][cont], &HMatchingContour, 1, 1);

								THEAPP.m_pAlgorithm->TemplateMatchingAlgorithm(THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[imgIdx], &HMatchingContour, &THEAPP.m_pModelDataManager->m_iBLROIAlignMatchingModel[imgIdx][cont]);

								copy_obj(HMatchingContour, &THEAPP.m_pInspectAdminViewDlg->Contours[imgIdx][cont], 1, 1);
							}

						}
					}
				}
			
				// 24.07.03 - v2651 - ROI Templet Matching Align 추가 - LeeGW END
				for (int cont = 0; cont < MAX_CONTOUR_NUM; cont++)
				{
					if (THEAPP.m_pModelDataManager->m_bOneBarrel && cont < MATCHING_CONTOUR_3)
					{
						THEAPP.m_pInspectAdminViewDlg->ContoursAffinTrans[cont].Reset();
						continue;
					}
					copy_obj(THEAPP.m_pInspectAdminViewDlg->Contours[THEAPP.m_pAlgorithm->m_iBlackMatchingImageType][cont], &THEAPP.m_pInspectAdminViewDlg->ContoursAffinTrans[cont], 1, 1);
				}
			}

		}
		m_TabControl.SetCurSel(0);
		THEAPP.m_pInspectAdminViewDlg->ClearUpdateView();
		m_pLightControlDlg->UpdateData(FALSE); // 변수에 적힌 값을 control 에 다시 표시. 
		UpdateData(FALSE);
	}
	catch(HException &except)
	{
		CString str;
		str.Format("Halcon Exception [ShowFirstPage] : %s", except.message);
		THEAPP.SaveLog(str);
		return ;
	}
}

void CTabControlDlg::HideAllPage()
{
	m_iCurrentTab = 0;
	m_pThresholdHistogramDlg->ShowWindow(SW_HIDE);
	m_pLightControlDlg->ShowWindow(SW_HIDE);
	m_pJogSetDlg->ShowWindow(SW_HIDE);
	m_pInspectionConditionTabDlg->ShowWindow(SW_HIDE);
	m_pMatchingDlg->ShowWindow(SW_HIDE);
	m_pAutoFocusDlg->ShowWindow(SW_HIDE);

	for (int i = 0; i < MAX_IMAGE_TAB; i++)
	{
		if (i >= THEAPP.m_iMaxInspImageNo)
			break;

		m_pTeachParamDlg[i]->ShowWindow(SW_HIDE);
	}
}

void CTabControlDlg::OnSelchangeTeachingTab(NMHDR *pNMHDR, LRESULT *pResult)
{
	int TabIndex = m_TabControl.GetCurSel();

	THEAPP.m_pTabControlDlg->HideROIAlgorithmParamDlg();	// 24.02.29 Local Align 추가 - LeeGW

	THEAPP.m_pInspectAdminViewDlg->Partition_Region.Reset();

	THEAPP.m_pInspectAdminViewDlg->m_pThresRgn.Reset();
	gen_empty_obj(&THEAPP.m_pInspectAdminViewDlg->m_pThresRgn);

	THEAPP.m_pInspectAdminViewDlg->m_HParameterDefectRgn.Reset();
	gen_empty_obj(&THEAPP.m_pInspectAdminViewDlg->m_HParameterDefectRgn);

	THEAPP.m_pInspectAdminViewDlg->m_HParameterDefectXLD.Reset();
	gen_empty_obj(&THEAPP.m_pInspectAdminViewDlg->m_HParameterDefectXLD);

	THEAPP.m_pInspectAdminViewDlg->SaveInspectTabOverLastSelectRegion();

	THEAPP.m_pInspectAdminViewDlg->ClearActiveTRegion();
	THEAPP.m_pInspectAdminViewDlg->ClearAllSelectRegion();
	THEAPP.m_pInspectAdminViewDlg->ClearAllLastSelectRegion();

	THEAPP.m_pInspectAdminViewDlg->m_pLastSelectedRegion = NULL;

	THEAPP.m_pInspectAdminViewDlg->ClearUpdateView();

	if (TabIndex == 0)
	{
		m_iCurrentTab = 0; // 현재 탭상태정보 (0=매칭, 1=배럴면, 2=배럴에지, 3=렌즈inner, 4=렌즈outer)
		ShowFirstPage();
	}
	else
	{
		m_iCurrentTab = TabIndex; // 현재 탭상태정보 (0=매칭, 1=배럴면, 2=배럴에지, 3=렌즈inner, 4=렌즈outer)
		m_pInspectionConditionTabDlg->Show();

		m_pJogSetDlg->ShowWindow(SW_HIDE);
		m_pMatchingDlg->ShowWindow(SW_HIDE);
		m_pAutoFocusDlg->ShowWindow(SW_HIDE);

		UpdateList(m_iCurrentTab - 1, 0);
		m_pTeachParamDlg[m_iCurrentTab - 1]->Show();

		for (int i = 0; i < MAX_IMAGE_TAB; i++)
		{
			if (i >= THEAPP.m_iMaxInspImageNo)
				break;

			if (m_iCurrentTab-1 != i)
			{
				m_pTeachParamDlg[i]->ShowWindow(SW_HIDE);
			}
		}
	
		Read_Image(m_iCurrentTab -1);

		m_pLightControlDlg->UpdateLightValue(m_iCurrentTab);

		m_pThresholdHistogramDlg->Show();
		m_pLightControlDlg->Show();

		m_pLightControlDlg->UpdateData(FALSE);
		
	}
	*pResult = 0;
}

void CTabControlDlg::Read_Image(int tab)
{
	try {
		if (THEAPP.m_pModelDataManager->m_sModelName == ".") return;

		copy_image(THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[tab], &THEAPP.m_pInspectAdminViewDlg->m_pHImage);

		if (THEAPP.m_pGFunction->ValidHXLD(THEAPP.m_pInspectAdminViewDlg->ContoursAffinTrans[MATCHING_CONTOUR_4]))
		{
			m_pThresholdHistogramDlg->SetImage(THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[tab]);
			m_pThresholdHistogramDlg->UpdateBar(0, 1);
		}

		THEAPP.m_pInspectAdminViewDlg->ClearUpdateView();
	}
	catch (HException& except)
	{
		THEAPP.m_pInspectAdminViewDlg->m_pHImage.Reset();
		gen_empty_obj(&THEAPP.m_pInspectAdminViewDlg->m_pHImage);
		THEAPP.m_pInspectAdminViewDlg->ClearUpdateView();
	}

}

void CTabControlDlg::LoadModelConditionParam()
{
	CString sTemp;

	// Top
	sTemp.Format("%.4f", THEAPP.m_pModelDataManager->m_dDirtMinArea_Top); m_pInspectionConditionTabDlg->m_pConditionTopBarrelDlg->m_Grid.QuickSetText(2, 0, sTemp);
	sTemp.Format("%.4f", THEAPP.m_pModelDataManager->m_dDirtMidArea_Top); m_pInspectionConditionTabDlg->m_pConditionTopBarrelDlg->m_Grid.QuickSetText(3, 0, sTemp);
	sTemp.Format("%.4f", THEAPP.m_pModelDataManager->m_dDirtMaxArea_Top); m_pInspectionConditionTabDlg->m_pConditionTopBarrelDlg->m_Grid.QuickSetText(4, 0, sTemp);
	sTemp.Format("%d", THEAPP.m_pModelDataManager->m_iDirtMinNumAccept_Top); m_pInspectionConditionTabDlg->m_pConditionTopBarrelDlg->m_Grid.QuickSetText(5, 0, sTemp);
	sTemp.Format("%d", THEAPP.m_pModelDataManager->m_iDirtMidNumAccept_Top); m_pInspectionConditionTabDlg->m_pConditionTopBarrelDlg->m_Grid.QuickSetText(6, 0, sTemp);
	sTemp.Format("%d", THEAPP.m_pModelDataManager->m_iDirtMaxNumAccept_Top); m_pInspectionConditionTabDlg->m_pConditionTopBarrelDlg->m_Grid.QuickSetText(7, 0, sTemp);

	sTemp.Format("%.4f", THEAPP.m_pModelDataManager->m_dScratchMinArea_Top); m_pInspectionConditionTabDlg->m_pConditionTopBarrelDlg->m_Grid.QuickSetText(2, 1, sTemp);
	sTemp.Format("%.4f", THEAPP.m_pModelDataManager->m_dScratchMidArea_Top); m_pInspectionConditionTabDlg->m_pConditionTopBarrelDlg->m_Grid.QuickSetText(3, 1, sTemp);
	sTemp.Format("%.4f", THEAPP.m_pModelDataManager->m_dScratchMaxArea_Top); m_pInspectionConditionTabDlg->m_pConditionTopBarrelDlg->m_Grid.QuickSetText(4, 1, sTemp);
	sTemp.Format("%d", THEAPP.m_pModelDataManager->m_iScratchMinNumAccept_Top); m_pInspectionConditionTabDlg->m_pConditionTopBarrelDlg->m_Grid.QuickSetText(5, 1, sTemp);
	sTemp.Format("%d", THEAPP.m_pModelDataManager->m_iScratchMidNumAccept_Top); m_pInspectionConditionTabDlg->m_pConditionTopBarrelDlg->m_Grid.QuickSetText(6, 1, sTemp);
	sTemp.Format("%d", THEAPP.m_pModelDataManager->m_iScratchMaxNumAccept_Top); m_pInspectionConditionTabDlg->m_pConditionTopBarrelDlg->m_Grid.QuickSetText(7, 1, sTemp);

	sTemp.Format("%.4f", THEAPP.m_pModelDataManager->m_dStainMinArea_Top); m_pInspectionConditionTabDlg->m_pConditionTopBarrelDlg->m_Grid.QuickSetText(2, 2, sTemp);
	sTemp.Format("%.4f", THEAPP.m_pModelDataManager->m_dStainMidArea_Top); m_pInspectionConditionTabDlg->m_pConditionTopBarrelDlg->m_Grid.QuickSetText(3, 2, sTemp);
	sTemp.Format("%.4f", THEAPP.m_pModelDataManager->m_dStainMaxArea_Top); m_pInspectionConditionTabDlg->m_pConditionTopBarrelDlg->m_Grid.QuickSetText(4, 2, sTemp);
	sTemp.Format("%d", THEAPP.m_pModelDataManager->m_iStainMinNumAccept_Top); m_pInspectionConditionTabDlg->m_pConditionTopBarrelDlg->m_Grid.QuickSetText(5, 2, sTemp);
	sTemp.Format("%d", THEAPP.m_pModelDataManager->m_iStainMidNumAccept_Top); m_pInspectionConditionTabDlg->m_pConditionTopBarrelDlg->m_Grid.QuickSetText(6, 2, sTemp);
	sTemp.Format("%d", THEAPP.m_pModelDataManager->m_iStainMaxNumAccept_Top); m_pInspectionConditionTabDlg->m_pConditionTopBarrelDlg->m_Grid.QuickSetText(7, 2, sTemp);

	sTemp.Format("%.4f", THEAPP.m_pModelDataManager->m_dDentMinArea_Top); m_pInspectionConditionTabDlg->m_pConditionTopBarrelDlg->m_Grid.QuickSetText(2, 3, sTemp);
	sTemp.Format("%.4f", THEAPP.m_pModelDataManager->m_dDentMidArea_Top); m_pInspectionConditionTabDlg->m_pConditionTopBarrelDlg->m_Grid.QuickSetText(3, 3, sTemp);
	sTemp.Format("%.4f", THEAPP.m_pModelDataManager->m_dDentMaxArea_Top); m_pInspectionConditionTabDlg->m_pConditionTopBarrelDlg->m_Grid.QuickSetText(4, 3, sTemp);
	sTemp.Format("%d", THEAPP.m_pModelDataManager->m_iDentMinNumAccept_Top); m_pInspectionConditionTabDlg->m_pConditionTopBarrelDlg->m_Grid.QuickSetText(5, 3, sTemp);
	sTemp.Format("%d", THEAPP.m_pModelDataManager->m_iDentMidNumAccept_Top); m_pInspectionConditionTabDlg->m_pConditionTopBarrelDlg->m_Grid.QuickSetText(6, 3, sTemp);
	sTemp.Format("%d", THEAPP.m_pModelDataManager->m_iDentMaxNumAccept_Top); m_pInspectionConditionTabDlg->m_pConditionTopBarrelDlg->m_Grid.QuickSetText(7, 3, sTemp);

	sTemp.Format("%.4f", THEAPP.m_pModelDataManager->m_dChipMinArea_Top); m_pInspectionConditionTabDlg->m_pConditionTopBarrelDlg->m_Grid.QuickSetText(2, 4, sTemp);
	sTemp.Format("%.4f", THEAPP.m_pModelDataManager->m_dChipMidArea_Top); m_pInspectionConditionTabDlg->m_pConditionTopBarrelDlg->m_Grid.QuickSetText(3, 4, sTemp);
	sTemp.Format("%.4f", THEAPP.m_pModelDataManager->m_dChipMaxArea_Top); m_pInspectionConditionTabDlg->m_pConditionTopBarrelDlg->m_Grid.QuickSetText(4, 4, sTemp);
	sTemp.Format("%d", THEAPP.m_pModelDataManager->m_iChipMinNumAccept_Top); m_pInspectionConditionTabDlg->m_pConditionTopBarrelDlg->m_Grid.QuickSetText(5, 4, sTemp);
	sTemp.Format("%d", THEAPP.m_pModelDataManager->m_iChipMidNumAccept_Top); m_pInspectionConditionTabDlg->m_pConditionTopBarrelDlg->m_Grid.QuickSetText(6, 4, sTemp);
	sTemp.Format("%d", THEAPP.m_pModelDataManager->m_iChipMaxNumAccept_Top); m_pInspectionConditionTabDlg->m_pConditionTopBarrelDlg->m_Grid.QuickSetText(7, 4, sTemp);

	sTemp.Format("%.4f", THEAPP.m_pModelDataManager->m_dEpoxyMinArea_Top); m_pInspectionConditionTabDlg->m_pConditionTopBarrelDlg->m_Grid.QuickSetText(2, 5, sTemp);
	sTemp.Format("%.4f", THEAPP.m_pModelDataManager->m_dEpoxyMidArea_Top); m_pInspectionConditionTabDlg->m_pConditionTopBarrelDlg->m_Grid.QuickSetText(3, 5, sTemp);
	sTemp.Format("%.4f", THEAPP.m_pModelDataManager->m_dEpoxyMaxArea_Top); m_pInspectionConditionTabDlg->m_pConditionTopBarrelDlg->m_Grid.QuickSetText(4, 5, sTemp);
	sTemp.Format("%d", THEAPP.m_pModelDataManager->m_iEpoxyMinNumAccept_Top); m_pInspectionConditionTabDlg->m_pConditionTopBarrelDlg->m_Grid.QuickSetText(5, 5, sTemp);
	sTemp.Format("%d", THEAPP.m_pModelDataManager->m_iEpoxyMidNumAccept_Top); m_pInspectionConditionTabDlg->m_pConditionTopBarrelDlg->m_Grid.QuickSetText(6, 5, sTemp);
	sTemp.Format("%d", THEAPP.m_pModelDataManager->m_iEpoxyMaxNumAccept_Top); m_pInspectionConditionTabDlg->m_pConditionTopBarrelDlg->m_Grid.QuickSetText(7, 5, sTemp);

	sTemp.Format("%.4f", THEAPP.m_pModelDataManager->m_dRes1MinArea_Top); m_pInspectionConditionTabDlg->m_pConditionTopBarrelDlg->m_Grid.QuickSetText(2, 6, sTemp);
	sTemp.Format("%.4f", THEAPP.m_pModelDataManager->m_dRes1MidArea_Top); m_pInspectionConditionTabDlg->m_pConditionTopBarrelDlg->m_Grid.QuickSetText(3, 6, sTemp);
	sTemp.Format("%.4f", THEAPP.m_pModelDataManager->m_dRes1MaxArea_Top); m_pInspectionConditionTabDlg->m_pConditionTopBarrelDlg->m_Grid.QuickSetText(4, 6, sTemp);
	sTemp.Format("%d", THEAPP.m_pModelDataManager->m_iRes1MinNumAccept_Top); m_pInspectionConditionTabDlg->m_pConditionTopBarrelDlg->m_Grid.QuickSetText(5, 6, sTemp);
	sTemp.Format("%d", THEAPP.m_pModelDataManager->m_iRes1MidNumAccept_Top); m_pInspectionConditionTabDlg->m_pConditionTopBarrelDlg->m_Grid.QuickSetText(6, 6, sTemp);
	sTemp.Format("%d", THEAPP.m_pModelDataManager->m_iRes1MaxNumAccept_Top); m_pInspectionConditionTabDlg->m_pConditionTopBarrelDlg->m_Grid.QuickSetText(7, 6, sTemp);

	sTemp.Format("%.4f", THEAPP.m_pModelDataManager->m_dRes2MinArea_Top); m_pInspectionConditionTabDlg->m_pConditionTopBarrelDlg->m_Grid.QuickSetText(2, 7, sTemp);
	sTemp.Format("%.4f", THEAPP.m_pModelDataManager->m_dRes2MidArea_Top); m_pInspectionConditionTabDlg->m_pConditionTopBarrelDlg->m_Grid.QuickSetText(3, 7, sTemp);
	sTemp.Format("%.4f", THEAPP.m_pModelDataManager->m_dRes2MaxArea_Top); m_pInspectionConditionTabDlg->m_pConditionTopBarrelDlg->m_Grid.QuickSetText(4, 7, sTemp);
	sTemp.Format("%d", THEAPP.m_pModelDataManager->m_iRes2MinNumAccept_Top); m_pInspectionConditionTabDlg->m_pConditionTopBarrelDlg->m_Grid.QuickSetText(5, 7, sTemp);
	sTemp.Format("%d", THEAPP.m_pModelDataManager->m_iRes2MidNumAccept_Top); m_pInspectionConditionTabDlg->m_pConditionTopBarrelDlg->m_Grid.QuickSetText(6, 7, sTemp);
	sTemp.Format("%d", THEAPP.m_pModelDataManager->m_iRes2MaxNumAccept_Top); m_pInspectionConditionTabDlg->m_pConditionTopBarrelDlg->m_Grid.QuickSetText(7, 7, sTemp);

	// WhiteDot - LeeGW
	sTemp.Format("%.4f", THEAPP.m_pModelDataManager->m_dWhiteDotMinArea_Top); m_pInspectionConditionTabDlg->m_pConditionTopBarrelDlg->m_Grid.QuickSetText(2, 8, sTemp);
	sTemp.Format("%.4f", THEAPP.m_pModelDataManager->m_dWhiteDotMidArea_Top); m_pInspectionConditionTabDlg->m_pConditionTopBarrelDlg->m_Grid.QuickSetText(3, 8, sTemp);
	sTemp.Format("%.4f", THEAPP.m_pModelDataManager->m_dWhiteDotMaxArea_Top); m_pInspectionConditionTabDlg->m_pConditionTopBarrelDlg->m_Grid.QuickSetText(4, 8, sTemp);
	sTemp.Format("%d", THEAPP.m_pModelDataManager->m_iWhiteDotMinNumAccept_Top); m_pInspectionConditionTabDlg->m_pConditionTopBarrelDlg->m_Grid.QuickSetText(5, 8, sTemp);
	sTemp.Format("%d", THEAPP.m_pModelDataManager->m_iWhiteDotMidNumAccept_Top); m_pInspectionConditionTabDlg->m_pConditionTopBarrelDlg->m_Grid.QuickSetText(6, 8, sTemp);
	sTemp.Format("%d", THEAPP.m_pModelDataManager->m_iWhiteDotMaxNumAccept_Top); m_pInspectionConditionTabDlg->m_pConditionTopBarrelDlg->m_Grid.QuickSetText(7, 8, sTemp);

	// Bottom
	sTemp.Format("%.4f", THEAPP.m_pModelDataManager->m_dDirtMinArea_Btm); m_pInspectionConditionTabDlg->m_pConditionBottomBarrelDlg->m_Grid.QuickSetText(2, 0, sTemp);
	sTemp.Format("%.4f", THEAPP.m_pModelDataManager->m_dDirtMidArea_Btm); m_pInspectionConditionTabDlg->m_pConditionBottomBarrelDlg->m_Grid.QuickSetText(3, 0, sTemp);
	sTemp.Format("%.4f", THEAPP.m_pModelDataManager->m_dDirtMaxArea_Btm); m_pInspectionConditionTabDlg->m_pConditionBottomBarrelDlg->m_Grid.QuickSetText(4, 0, sTemp);
	sTemp.Format("%d", THEAPP.m_pModelDataManager->m_iDirtMinNumAccept_Btm); m_pInspectionConditionTabDlg->m_pConditionBottomBarrelDlg->m_Grid.QuickSetText(5, 0, sTemp);
	sTemp.Format("%d", THEAPP.m_pModelDataManager->m_iDirtMidNumAccept_Btm); m_pInspectionConditionTabDlg->m_pConditionBottomBarrelDlg->m_Grid.QuickSetText(6, 0, sTemp);
	sTemp.Format("%d", THEAPP.m_pModelDataManager->m_iDirtMaxNumAccept_Btm); m_pInspectionConditionTabDlg->m_pConditionBottomBarrelDlg->m_Grid.QuickSetText(7, 0, sTemp);

	sTemp.Format("%.4f", THEAPP.m_pModelDataManager->m_dScratchMinArea_Btm); m_pInspectionConditionTabDlg->m_pConditionBottomBarrelDlg->m_Grid.QuickSetText(2, 1, sTemp);
	sTemp.Format("%.4f", THEAPP.m_pModelDataManager->m_dScratchMidArea_Btm); m_pInspectionConditionTabDlg->m_pConditionBottomBarrelDlg->m_Grid.QuickSetText(3, 1, sTemp);
	sTemp.Format("%.4f", THEAPP.m_pModelDataManager->m_dScratchMaxArea_Btm); m_pInspectionConditionTabDlg->m_pConditionBottomBarrelDlg->m_Grid.QuickSetText(4, 1, sTemp);
	sTemp.Format("%d", THEAPP.m_pModelDataManager->m_iScratchMinNumAccept_Btm); m_pInspectionConditionTabDlg->m_pConditionBottomBarrelDlg->m_Grid.QuickSetText(5, 1, sTemp);
	sTemp.Format("%d", THEAPP.m_pModelDataManager->m_iScratchMidNumAccept_Btm); m_pInspectionConditionTabDlg->m_pConditionBottomBarrelDlg->m_Grid.QuickSetText(6, 1, sTemp);
	sTemp.Format("%d", THEAPP.m_pModelDataManager->m_iScratchMaxNumAccept_Btm); m_pInspectionConditionTabDlg->m_pConditionBottomBarrelDlg->m_Grid.QuickSetText(7, 1, sTemp);

	sTemp.Format("%.4f", THEAPP.m_pModelDataManager->m_dStainMinArea_Btm); m_pInspectionConditionTabDlg->m_pConditionBottomBarrelDlg->m_Grid.QuickSetText(2, 2, sTemp);
	sTemp.Format("%.4f", THEAPP.m_pModelDataManager->m_dStainMidArea_Btm); m_pInspectionConditionTabDlg->m_pConditionBottomBarrelDlg->m_Grid.QuickSetText(3, 2, sTemp);
	sTemp.Format("%.4f", THEAPP.m_pModelDataManager->m_dStainMaxArea_Btm); m_pInspectionConditionTabDlg->m_pConditionBottomBarrelDlg->m_Grid.QuickSetText(4, 2, sTemp);
	sTemp.Format("%d", THEAPP.m_pModelDataManager->m_iStainMinNumAccept_Btm); m_pInspectionConditionTabDlg->m_pConditionBottomBarrelDlg->m_Grid.QuickSetText(5, 2, sTemp);
	sTemp.Format("%d", THEAPP.m_pModelDataManager->m_iStainMidNumAccept_Btm); m_pInspectionConditionTabDlg->m_pConditionBottomBarrelDlg->m_Grid.QuickSetText(6, 2, sTemp);
	sTemp.Format("%d", THEAPP.m_pModelDataManager->m_iStainMaxNumAccept_Btm); m_pInspectionConditionTabDlg->m_pConditionBottomBarrelDlg->m_Grid.QuickSetText(7, 2, sTemp);

	sTemp.Format("%.4f", THEAPP.m_pModelDataManager->m_dDentMinArea_Btm); m_pInspectionConditionTabDlg->m_pConditionBottomBarrelDlg->m_Grid.QuickSetText(2, 3, sTemp);
	sTemp.Format("%.4f", THEAPP.m_pModelDataManager->m_dDentMidArea_Btm); m_pInspectionConditionTabDlg->m_pConditionBottomBarrelDlg->m_Grid.QuickSetText(3, 3, sTemp);
	sTemp.Format("%.4f", THEAPP.m_pModelDataManager->m_dDentMaxArea_Btm); m_pInspectionConditionTabDlg->m_pConditionBottomBarrelDlg->m_Grid.QuickSetText(4, 3, sTemp);
	sTemp.Format("%d", THEAPP.m_pModelDataManager->m_iDentMinNumAccept_Btm); m_pInspectionConditionTabDlg->m_pConditionBottomBarrelDlg->m_Grid.QuickSetText(5, 3, sTemp);
	sTemp.Format("%d", THEAPP.m_pModelDataManager->m_iDentMidNumAccept_Btm); m_pInspectionConditionTabDlg->m_pConditionBottomBarrelDlg->m_Grid.QuickSetText(6, 3, sTemp);
	sTemp.Format("%d", THEAPP.m_pModelDataManager->m_iDentMaxNumAccept_Btm); m_pInspectionConditionTabDlg->m_pConditionBottomBarrelDlg->m_Grid.QuickSetText(7, 3, sTemp);

	sTemp.Format("%.4f", THEAPP.m_pModelDataManager->m_dChipMinArea_Btm); m_pInspectionConditionTabDlg->m_pConditionBottomBarrelDlg->m_Grid.QuickSetText(2, 4, sTemp);
	sTemp.Format("%.4f", THEAPP.m_pModelDataManager->m_dChipMidArea_Btm); m_pInspectionConditionTabDlg->m_pConditionBottomBarrelDlg->m_Grid.QuickSetText(3, 4, sTemp);
	sTemp.Format("%.4f", THEAPP.m_pModelDataManager->m_dChipMaxArea_Btm); m_pInspectionConditionTabDlg->m_pConditionBottomBarrelDlg->m_Grid.QuickSetText(4, 4, sTemp);
	sTemp.Format("%d", THEAPP.m_pModelDataManager->m_iChipMinNumAccept_Btm); m_pInspectionConditionTabDlg->m_pConditionBottomBarrelDlg->m_Grid.QuickSetText(5, 4, sTemp);
	sTemp.Format("%d", THEAPP.m_pModelDataManager->m_iChipMidNumAccept_Btm); m_pInspectionConditionTabDlg->m_pConditionBottomBarrelDlg->m_Grid.QuickSetText(6, 4, sTemp);
	sTemp.Format("%d", THEAPP.m_pModelDataManager->m_iChipMaxNumAccept_Btm); m_pInspectionConditionTabDlg->m_pConditionBottomBarrelDlg->m_Grid.QuickSetText(7, 4, sTemp);

	sTemp.Format("%.4f", THEAPP.m_pModelDataManager->m_dEpoxyMinArea_Btm); m_pInspectionConditionTabDlg->m_pConditionBottomBarrelDlg->m_Grid.QuickSetText(2, 5, sTemp);
	sTemp.Format("%.4f", THEAPP.m_pModelDataManager->m_dEpoxyMidArea_Btm); m_pInspectionConditionTabDlg->m_pConditionBottomBarrelDlg->m_Grid.QuickSetText(3, 5, sTemp);
	sTemp.Format("%.4f", THEAPP.m_pModelDataManager->m_dEpoxyMaxArea_Btm); m_pInspectionConditionTabDlg->m_pConditionBottomBarrelDlg->m_Grid.QuickSetText(4, 5, sTemp);
	sTemp.Format("%d", THEAPP.m_pModelDataManager->m_iEpoxyMinNumAccept_Btm); m_pInspectionConditionTabDlg->m_pConditionBottomBarrelDlg->m_Grid.QuickSetText(5, 5, sTemp);
	sTemp.Format("%d", THEAPP.m_pModelDataManager->m_iEpoxyMidNumAccept_Btm); m_pInspectionConditionTabDlg->m_pConditionBottomBarrelDlg->m_Grid.QuickSetText(6, 5, sTemp);
	sTemp.Format("%d", THEAPP.m_pModelDataManager->m_iEpoxyMaxNumAccept_Btm); m_pInspectionConditionTabDlg->m_pConditionBottomBarrelDlg->m_Grid.QuickSetText(7, 5, sTemp);

	sTemp.Format("%.4f", THEAPP.m_pModelDataManager->m_dRes1MinArea_Btm); m_pInspectionConditionTabDlg->m_pConditionBottomBarrelDlg->m_Grid.QuickSetText(2, 6, sTemp);
	sTemp.Format("%.4f", THEAPP.m_pModelDataManager->m_dRes1MidArea_Btm); m_pInspectionConditionTabDlg->m_pConditionBottomBarrelDlg->m_Grid.QuickSetText(3, 6, sTemp);
	sTemp.Format("%.4f", THEAPP.m_pModelDataManager->m_dRes1MaxArea_Btm); m_pInspectionConditionTabDlg->m_pConditionBottomBarrelDlg->m_Grid.QuickSetText(4, 6, sTemp);
	sTemp.Format("%d", THEAPP.m_pModelDataManager->m_iRes1MinNumAccept_Btm); m_pInspectionConditionTabDlg->m_pConditionBottomBarrelDlg->m_Grid.QuickSetText(5, 6, sTemp);
	sTemp.Format("%d", THEAPP.m_pModelDataManager->m_iRes1MidNumAccept_Btm); m_pInspectionConditionTabDlg->m_pConditionBottomBarrelDlg->m_Grid.QuickSetText(6, 6, sTemp);
	sTemp.Format("%d", THEAPP.m_pModelDataManager->m_iRes1MaxNumAccept_Btm); m_pInspectionConditionTabDlg->m_pConditionBottomBarrelDlg->m_Grid.QuickSetText(7, 6, sTemp);

	sTemp.Format("%.4f", THEAPP.m_pModelDataManager->m_dRes2MinArea_Btm); m_pInspectionConditionTabDlg->m_pConditionBottomBarrelDlg->m_Grid.QuickSetText(2, 7, sTemp);
	sTemp.Format("%.4f", THEAPP.m_pModelDataManager->m_dRes2MidArea_Btm); m_pInspectionConditionTabDlg->m_pConditionBottomBarrelDlg->m_Grid.QuickSetText(3, 7, sTemp);
	sTemp.Format("%.4f", THEAPP.m_pModelDataManager->m_dRes2MaxArea_Btm); m_pInspectionConditionTabDlg->m_pConditionBottomBarrelDlg->m_Grid.QuickSetText(4, 7, sTemp);
	sTemp.Format("%d", THEAPP.m_pModelDataManager->m_iRes2MinNumAccept_Btm); m_pInspectionConditionTabDlg->m_pConditionBottomBarrelDlg->m_Grid.QuickSetText(5, 7, sTemp);
	sTemp.Format("%d", THEAPP.m_pModelDataManager->m_iRes2MidNumAccept_Btm); m_pInspectionConditionTabDlg->m_pConditionBottomBarrelDlg->m_Grid.QuickSetText(6, 7, sTemp);
	sTemp.Format("%d", THEAPP.m_pModelDataManager->m_iRes2MaxNumAccept_Btm); m_pInspectionConditionTabDlg->m_pConditionBottomBarrelDlg->m_Grid.QuickSetText(7, 7, sTemp);

	// WhiteDot - LeeGW
	sTemp.Format("%.4f", THEAPP.m_pModelDataManager->m_dWhiteDotMinArea_Btm); m_pInspectionConditionTabDlg->m_pConditionBottomBarrelDlg->m_Grid.QuickSetText(2, 8, sTemp);
	sTemp.Format("%.4f", THEAPP.m_pModelDataManager->m_dWhiteDotMidArea_Btm); m_pInspectionConditionTabDlg->m_pConditionBottomBarrelDlg->m_Grid.QuickSetText(3, 8, sTemp);
	sTemp.Format("%.4f", THEAPP.m_pModelDataManager->m_dWhiteDotMaxArea_Btm); m_pInspectionConditionTabDlg->m_pConditionBottomBarrelDlg->m_Grid.QuickSetText(4, 8, sTemp);
	sTemp.Format("%d", THEAPP.m_pModelDataManager->m_iWhiteDotMinNumAccept_Btm); m_pInspectionConditionTabDlg->m_pConditionBottomBarrelDlg->m_Grid.QuickSetText(5, 8, sTemp);
	sTemp.Format("%d", THEAPP.m_pModelDataManager->m_iWhiteDotMidNumAccept_Btm); m_pInspectionConditionTabDlg->m_pConditionBottomBarrelDlg->m_Grid.QuickSetText(6, 8, sTemp);
	sTemp.Format("%d", THEAPP.m_pModelDataManager->m_iWhiteDotMaxNumAccept_Btm); m_pInspectionConditionTabDlg->m_pConditionBottomBarrelDlg->m_Grid.QuickSetText(7, 8, sTemp);

	// Inner
	sTemp.Format("%.4f", THEAPP.m_pModelDataManager->m_dDirtMinArea_Inner); m_pInspectionConditionTabDlg->m_pConditionInnerBarrelDlg->m_Grid.QuickSetText(2, 0, sTemp);
	sTemp.Format("%.4f", THEAPP.m_pModelDataManager->m_dDirtMidArea_Inner); m_pInspectionConditionTabDlg->m_pConditionInnerBarrelDlg->m_Grid.QuickSetText(3, 0, sTemp);
	sTemp.Format("%.4f", THEAPP.m_pModelDataManager->m_dDirtMaxArea_Inner); m_pInspectionConditionTabDlg->m_pConditionInnerBarrelDlg->m_Grid.QuickSetText(4, 0, sTemp);
	sTemp.Format("%d", THEAPP.m_pModelDataManager->m_iDirtMinNumAccept_Inner); m_pInspectionConditionTabDlg->m_pConditionInnerBarrelDlg->m_Grid.QuickSetText(5, 0, sTemp);
	sTemp.Format("%d", THEAPP.m_pModelDataManager->m_iDirtMidNumAccept_Inner); m_pInspectionConditionTabDlg->m_pConditionInnerBarrelDlg->m_Grid.QuickSetText(6, 0, sTemp);
	sTemp.Format("%d", THEAPP.m_pModelDataManager->m_iDirtMaxNumAccept_Inner); m_pInspectionConditionTabDlg->m_pConditionInnerBarrelDlg->m_Grid.QuickSetText(7, 0, sTemp);

	sTemp.Format("%.4f", THEAPP.m_pModelDataManager->m_dScratchMinArea_Inner); m_pInspectionConditionTabDlg->m_pConditionInnerBarrelDlg->m_Grid.QuickSetText(2, 1, sTemp);
	sTemp.Format("%.4f", THEAPP.m_pModelDataManager->m_dScratchMidArea_Inner); m_pInspectionConditionTabDlg->m_pConditionInnerBarrelDlg->m_Grid.QuickSetText(3, 1, sTemp);
	sTemp.Format("%.4f", THEAPP.m_pModelDataManager->m_dScratchMaxArea_Inner); m_pInspectionConditionTabDlg->m_pConditionInnerBarrelDlg->m_Grid.QuickSetText(4, 1, sTemp);
	sTemp.Format("%d", THEAPP.m_pModelDataManager->m_iScratchMinNumAccept_Inner); m_pInspectionConditionTabDlg->m_pConditionInnerBarrelDlg->m_Grid.QuickSetText(5, 1, sTemp);
	sTemp.Format("%d", THEAPP.m_pModelDataManager->m_iScratchMidNumAccept_Inner); m_pInspectionConditionTabDlg->m_pConditionInnerBarrelDlg->m_Grid.QuickSetText(6, 1, sTemp);
	sTemp.Format("%d", THEAPP.m_pModelDataManager->m_iScratchMaxNumAccept_Inner); m_pInspectionConditionTabDlg->m_pConditionInnerBarrelDlg->m_Grid.QuickSetText(7, 1, sTemp);

	sTemp.Format("%.4f", THEAPP.m_pModelDataManager->m_dStainMinArea_Inner); m_pInspectionConditionTabDlg->m_pConditionInnerBarrelDlg->m_Grid.QuickSetText(2, 2, sTemp);
	sTemp.Format("%.4f", THEAPP.m_pModelDataManager->m_dStainMidArea_Inner); m_pInspectionConditionTabDlg->m_pConditionInnerBarrelDlg->m_Grid.QuickSetText(3, 2, sTemp);
	sTemp.Format("%.4f", THEAPP.m_pModelDataManager->m_dStainMaxArea_Inner); m_pInspectionConditionTabDlg->m_pConditionInnerBarrelDlg->m_Grid.QuickSetText(4, 2, sTemp);
	sTemp.Format("%d", THEAPP.m_pModelDataManager->m_iStainMinNumAccept_Inner); m_pInspectionConditionTabDlg->m_pConditionInnerBarrelDlg->m_Grid.QuickSetText(5, 2, sTemp);
	sTemp.Format("%d", THEAPP.m_pModelDataManager->m_iStainMidNumAccept_Inner); m_pInspectionConditionTabDlg->m_pConditionInnerBarrelDlg->m_Grid.QuickSetText(6, 2, sTemp);
	sTemp.Format("%d", THEAPP.m_pModelDataManager->m_iStainMaxNumAccept_Inner); m_pInspectionConditionTabDlg->m_pConditionInnerBarrelDlg->m_Grid.QuickSetText(7, 2, sTemp);

	sTemp.Format("%.4f", THEAPP.m_pModelDataManager->m_dDentMinArea_Inner); m_pInspectionConditionTabDlg->m_pConditionInnerBarrelDlg->m_Grid.QuickSetText(2, 3, sTemp);
	sTemp.Format("%.4f", THEAPP.m_pModelDataManager->m_dDentMidArea_Inner); m_pInspectionConditionTabDlg->m_pConditionInnerBarrelDlg->m_Grid.QuickSetText(3, 3, sTemp);
	sTemp.Format("%.4f", THEAPP.m_pModelDataManager->m_dDentMaxArea_Inner); m_pInspectionConditionTabDlg->m_pConditionInnerBarrelDlg->m_Grid.QuickSetText(4, 3, sTemp);
	sTemp.Format("%d", THEAPP.m_pModelDataManager->m_iDentMinNumAccept_Inner); m_pInspectionConditionTabDlg->m_pConditionInnerBarrelDlg->m_Grid.QuickSetText(5, 3, sTemp);
	sTemp.Format("%d", THEAPP.m_pModelDataManager->m_iDentMidNumAccept_Inner); m_pInspectionConditionTabDlg->m_pConditionInnerBarrelDlg->m_Grid.QuickSetText(6, 3, sTemp);
	sTemp.Format("%d", THEAPP.m_pModelDataManager->m_iDentMaxNumAccept_Inner); m_pInspectionConditionTabDlg->m_pConditionInnerBarrelDlg->m_Grid.QuickSetText(7, 3, sTemp);

	sTemp.Format("%.4f", THEAPP.m_pModelDataManager->m_dChipMinArea_Inner); m_pInspectionConditionTabDlg->m_pConditionInnerBarrelDlg->m_Grid.QuickSetText(2, 4, sTemp);
	sTemp.Format("%.4f", THEAPP.m_pModelDataManager->m_dChipMidArea_Inner); m_pInspectionConditionTabDlg->m_pConditionInnerBarrelDlg->m_Grid.QuickSetText(3, 4, sTemp);
	sTemp.Format("%.4f", THEAPP.m_pModelDataManager->m_dChipMaxArea_Inner); m_pInspectionConditionTabDlg->m_pConditionInnerBarrelDlg->m_Grid.QuickSetText(4, 4, sTemp);
	sTemp.Format("%d", THEAPP.m_pModelDataManager->m_iChipMinNumAccept_Inner); m_pInspectionConditionTabDlg->m_pConditionInnerBarrelDlg->m_Grid.QuickSetText(5, 4, sTemp);
	sTemp.Format("%d", THEAPP.m_pModelDataManager->m_iChipMidNumAccept_Inner); m_pInspectionConditionTabDlg->m_pConditionInnerBarrelDlg->m_Grid.QuickSetText(6, 4, sTemp);
	sTemp.Format("%d", THEAPP.m_pModelDataManager->m_iChipMaxNumAccept_Inner); m_pInspectionConditionTabDlg->m_pConditionInnerBarrelDlg->m_Grid.QuickSetText(7, 4, sTemp);

	sTemp.Format("%.4f", THEAPP.m_pModelDataManager->m_dEpoxyMinArea_Inner); m_pInspectionConditionTabDlg->m_pConditionInnerBarrelDlg->m_Grid.QuickSetText(2, 5, sTemp);
	sTemp.Format("%.4f", THEAPP.m_pModelDataManager->m_dEpoxyMidArea_Inner); m_pInspectionConditionTabDlg->m_pConditionInnerBarrelDlg->m_Grid.QuickSetText(3, 5, sTemp);
	sTemp.Format("%.4f", THEAPP.m_pModelDataManager->m_dEpoxyMaxArea_Inner); m_pInspectionConditionTabDlg->m_pConditionInnerBarrelDlg->m_Grid.QuickSetText(4, 5, sTemp);
	sTemp.Format("%d", THEAPP.m_pModelDataManager->m_iEpoxyMinNumAccept_Inner); m_pInspectionConditionTabDlg->m_pConditionInnerBarrelDlg->m_Grid.QuickSetText(5, 5, sTemp);
	sTemp.Format("%d", THEAPP.m_pModelDataManager->m_iEpoxyMidNumAccept_Inner); m_pInspectionConditionTabDlg->m_pConditionInnerBarrelDlg->m_Grid.QuickSetText(6, 5, sTemp);
	sTemp.Format("%d", THEAPP.m_pModelDataManager->m_iEpoxyMaxNumAccept_Inner); m_pInspectionConditionTabDlg->m_pConditionInnerBarrelDlg->m_Grid.QuickSetText(7, 5, sTemp);

	sTemp.Format("%.4f", THEAPP.m_pModelDataManager->m_dRes1MinArea_Inner); m_pInspectionConditionTabDlg->m_pConditionInnerBarrelDlg->m_Grid.QuickSetText(2, 6, sTemp);
	sTemp.Format("%.4f", THEAPP.m_pModelDataManager->m_dRes1MidArea_Inner); m_pInspectionConditionTabDlg->m_pConditionInnerBarrelDlg->m_Grid.QuickSetText(3, 6, sTemp);
	sTemp.Format("%.4f", THEAPP.m_pModelDataManager->m_dRes1MaxArea_Inner); m_pInspectionConditionTabDlg->m_pConditionInnerBarrelDlg->m_Grid.QuickSetText(4, 6, sTemp);
	sTemp.Format("%d", THEAPP.m_pModelDataManager->m_iRes1MinNumAccept_Inner); m_pInspectionConditionTabDlg->m_pConditionInnerBarrelDlg->m_Grid.QuickSetText(5, 6, sTemp);
	sTemp.Format("%d", THEAPP.m_pModelDataManager->m_iRes1MidNumAccept_Inner); m_pInspectionConditionTabDlg->m_pConditionInnerBarrelDlg->m_Grid.QuickSetText(6, 6, sTemp);
	sTemp.Format("%d", THEAPP.m_pModelDataManager->m_iRes1MaxNumAccept_Inner); m_pInspectionConditionTabDlg->m_pConditionInnerBarrelDlg->m_Grid.QuickSetText(7, 6, sTemp);

	sTemp.Format("%.4f", THEAPP.m_pModelDataManager->m_dRes2MinArea_Inner); m_pInspectionConditionTabDlg->m_pConditionInnerBarrelDlg->m_Grid.QuickSetText(2, 7, sTemp);
	sTemp.Format("%.4f", THEAPP.m_pModelDataManager->m_dRes2MidArea_Inner); m_pInspectionConditionTabDlg->m_pConditionInnerBarrelDlg->m_Grid.QuickSetText(3, 7, sTemp);
	sTemp.Format("%.4f", THEAPP.m_pModelDataManager->m_dRes2MaxArea_Inner); m_pInspectionConditionTabDlg->m_pConditionInnerBarrelDlg->m_Grid.QuickSetText(4, 7, sTemp);
	sTemp.Format("%d", THEAPP.m_pModelDataManager->m_iRes2MinNumAccept_Inner); m_pInspectionConditionTabDlg->m_pConditionInnerBarrelDlg->m_Grid.QuickSetText(5, 7, sTemp);
	sTemp.Format("%d", THEAPP.m_pModelDataManager->m_iRes2MidNumAccept_Inner); m_pInspectionConditionTabDlg->m_pConditionInnerBarrelDlg->m_Grid.QuickSetText(6, 7, sTemp);
	sTemp.Format("%d", THEAPP.m_pModelDataManager->m_iRes2MaxNumAccept_Inner); m_pInspectionConditionTabDlg->m_pConditionInnerBarrelDlg->m_Grid.QuickSetText(7, 7, sTemp);

	// WhiteDot - LeeGW
	sTemp.Format("%.4f", THEAPP.m_pModelDataManager->m_dWhiteDotMinArea_Inner); m_pInspectionConditionTabDlg->m_pConditionInnerBarrelDlg->m_Grid.QuickSetText(2, 8, sTemp);
	sTemp.Format("%.4f", THEAPP.m_pModelDataManager->m_dWhiteDotMidArea_Inner); m_pInspectionConditionTabDlg->m_pConditionInnerBarrelDlg->m_Grid.QuickSetText(3, 8, sTemp);
	sTemp.Format("%.4f", THEAPP.m_pModelDataManager->m_dWhiteDotMaxArea_Inner); m_pInspectionConditionTabDlg->m_pConditionInnerBarrelDlg->m_Grid.QuickSetText(4, 8, sTemp);
	sTemp.Format("%d", THEAPP.m_pModelDataManager->m_iWhiteDotMinNumAccept_Inner); m_pInspectionConditionTabDlg->m_pConditionInnerBarrelDlg->m_Grid.QuickSetText(5, 8, sTemp);
	sTemp.Format("%d", THEAPP.m_pModelDataManager->m_iWhiteDotMidNumAccept_Inner); m_pInspectionConditionTabDlg->m_pConditionInnerBarrelDlg->m_Grid.QuickSetText(6, 8, sTemp);
	sTemp.Format("%d", THEAPP.m_pModelDataManager->m_iWhiteDotMaxNumAccept_Inner); m_pInspectionConditionTabDlg->m_pConditionInnerBarrelDlg->m_Grid.QuickSetText(7, 8, sTemp);

	// Lens
	sTemp.Format("%.4f", THEAPP.m_pModelDataManager->m_dLensMinArea); m_pInspectionConditionTabDlg->m_pConditionLensDlg->m_Grid.QuickSetText(2, 0, sTemp);
	sTemp.Format("%.4f", THEAPP.m_pModelDataManager->m_dLensMidArea); m_pInspectionConditionTabDlg->m_pConditionLensDlg->m_Grid.QuickSetText(3, 0, sTemp);
	sTemp.Format("%.4f", THEAPP.m_pModelDataManager->m_dLensMaxArea); m_pInspectionConditionTabDlg->m_pConditionLensDlg->m_Grid.QuickSetText(4, 0, sTemp);
	sTemp.Format("%d", THEAPP.m_pModelDataManager->m_iLensMinNumAccept); m_pInspectionConditionTabDlg->m_pConditionLensDlg->m_Grid.QuickSetText(5, 0, sTemp);
	sTemp.Format("%d", THEAPP.m_pModelDataManager->m_iLensMidNumAccept); m_pInspectionConditionTabDlg->m_pConditionLensDlg->m_Grid.QuickSetText(6, 0, sTemp);
	sTemp.Format("%d", THEAPP.m_pModelDataManager->m_iLensMaxNumAccept); m_pInspectionConditionTabDlg->m_pConditionLensDlg->m_Grid.QuickSetText(7, 0, sTemp);

	sTemp.Format("%.4f", THEAPP.m_pModelDataManager->m_dLensScratchMinArea); m_pInspectionConditionTabDlg->m_pConditionLensDlg->m_Grid.QuickSetText(2, 1, sTemp);
	sTemp.Format("%.4f", THEAPP.m_pModelDataManager->m_dLensScratchMidArea); m_pInspectionConditionTabDlg->m_pConditionLensDlg->m_Grid.QuickSetText(3, 1, sTemp);
	sTemp.Format("%.4f", THEAPP.m_pModelDataManager->m_dLensScratchMaxArea); m_pInspectionConditionTabDlg->m_pConditionLensDlg->m_Grid.QuickSetText(4, 1, sTemp);
	sTemp.Format("%d", THEAPP.m_pModelDataManager->m_iLensScratchMinNumAccept); m_pInspectionConditionTabDlg->m_pConditionLensDlg->m_Grid.QuickSetText(5, 1, sTemp);
	sTemp.Format("%d", THEAPP.m_pModelDataManager->m_iLensScratchMidNumAccept); m_pInspectionConditionTabDlg->m_pConditionLensDlg->m_Grid.QuickSetText(6, 1, sTemp);
	sTemp.Format("%d", THEAPP.m_pModelDataManager->m_iLensScratchMaxNumAccept); m_pInspectionConditionTabDlg->m_pConditionLensDlg->m_Grid.QuickSetText(7, 1, sTemp);

	sTemp.Format("%.4f", THEAPP.m_pModelDataManager->m_dLensDirtMinArea); m_pInspectionConditionTabDlg->m_pConditionLensDlg->m_Grid.QuickSetText(2, 2, sTemp);
	sTemp.Format("%.4f", THEAPP.m_pModelDataManager->m_dLensDirtMidArea); m_pInspectionConditionTabDlg->m_pConditionLensDlg->m_Grid.QuickSetText(3, 2, sTemp);
	sTemp.Format("%.4f", THEAPP.m_pModelDataManager->m_dLensDirtMaxArea); m_pInspectionConditionTabDlg->m_pConditionLensDlg->m_Grid.QuickSetText(4, 2, sTemp);
	sTemp.Format("%d", THEAPP.m_pModelDataManager->m_iLensDirtMinNumAccept); m_pInspectionConditionTabDlg->m_pConditionLensDlg->m_Grid.QuickSetText(5, 2, sTemp);
	sTemp.Format("%d", THEAPP.m_pModelDataManager->m_iLensDirtMidNumAccept); m_pInspectionConditionTabDlg->m_pConditionLensDlg->m_Grid.QuickSetText(6, 2, sTemp);
	sTemp.Format("%d", THEAPP.m_pModelDataManager->m_iLensDirtMaxNumAccept); m_pInspectionConditionTabDlg->m_pConditionLensDlg->m_Grid.QuickSetText(7, 2, sTemp);

	// white dot
	sTemp.Format("%.4f", THEAPP.m_pModelDataManager->m_dLensWhiteDotMinArea); m_pInspectionConditionTabDlg->m_pConditionLensDlg->m_Grid.QuickSetText(2, 3, sTemp);
	sTemp.Format("%.4f", THEAPP.m_pModelDataManager->m_dLensWhiteDotMidArea); m_pInspectionConditionTabDlg->m_pConditionLensDlg->m_Grid.QuickSetText(3, 3, sTemp);
	sTemp.Format("%.4f", THEAPP.m_pModelDataManager->m_dLensWhiteDotMaxArea); m_pInspectionConditionTabDlg->m_pConditionLensDlg->m_Grid.QuickSetText(4, 3, sTemp);
	sTemp.Format("%d", THEAPP.m_pModelDataManager->m_iLensWhiteDotMinNumAccept); m_pInspectionConditionTabDlg->m_pConditionLensDlg->m_Grid.QuickSetText(5, 3, sTemp);
	sTemp.Format("%d", THEAPP.m_pModelDataManager->m_iLensWhiteDotMidNumAccept); m_pInspectionConditionTabDlg->m_pConditionLensDlg->m_Grid.QuickSetText(6, 3, sTemp);
	sTemp.Format("%d", THEAPP.m_pModelDataManager->m_iLensWhiteDotMaxNumAccept); m_pInspectionConditionTabDlg->m_pConditionLensDlg->m_Grid.QuickSetText(7, 3, sTemp);

}

BOOL CTabControlDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	if(pMsg->message==WM_KEYDOWN&&pMsg->wParam==VK_ESCAPE)
		return TRUE;
	if(pMsg->message==WM_KEYDOWN&&pMsg->wParam==VK_RETURN)
		return TRUE;
	return CDialog::PreTranslateMessage(pMsg);
}

void CTabControlDlg::OnDestroy()
{
	CDialog::OnDestroy();

	m_pAlgorithmDlg->DeleteInstance();
	m_pAlgorithmDlg = NULL;

	m_pROIAlgorithmParamDlg->DeleteInstance();
	m_pROIAlgorithmParamDlg = NULL;
}

// 24.02.28 Local Align 추가 - LeeGW Start
void CTabControlDlg::HideROIAlgorithmParamDlg()
{
	if (m_pROIAlgorithmParamDlg->m_hWnd)
	{
		m_pROIAlgorithmParamDlg->Hide();
	}
}

void CTabControlDlg::ShowROIAlgorithmParamDlg()
{
	m_pROIAlgorithmParamDlg->Show();
}

void CTabControlDlg::ShowROIAlgorithmWnd(int iImageType, GTRegion *pROIRegion, int iInspectionType)
{
	try
	{
		if(THEAPP.m_pModelDataManager->m_sModelName == ".")
			return;

		m_pROIAlgorithmParamDlg->SetSelectedInspection(iImageType, pROIRegion, iInspectionType);
		m_pROIAlgorithmParamDlg->SetParam();

		ShowROIAlgorithmParamDlg();
	}
	catch (HException& except)
	{
		CString str;
		str.Format("Halcon Exception [CTabControlDlg::ShowShowLocalAlignWnd] : %s", except.message);
		THEAPP.SaveLog(str);
	}
}

// 24.02.28 Local Align 추가 - LeeGW End

void CTabControlDlg::ShowAlgorithmDlg()
{
	m_pAlgorithmDlg->Show();
}

void CTabControlDlg::HideAlgorithmDlg()
{
	if (m_pAlgorithmDlg->m_hWnd)
	{
		m_pAlgorithmDlg->Hide();
	}
}

void CTabControlDlg::ShowLightAutoCalResultDlg()
{
	m_pLightAutoCalResultDlg->LoadViewParam();
	m_pLightAutoCalResultDlg->Show();
}

void CTabControlDlg::HideLightAutoCalResultDlg()
{
	if (m_pLightAutoCalResultDlg->m_hWnd)
	{
		m_pLightAutoCalResultDlg->Hide();
	}
}

void CTabControlDlg::ShowAlgorithmWnd(int iImageType, int iInspectionType)
{
	try
	{
		if(THEAPP.m_pModelDataManager->m_sModelName == ".") return;

		m_pAlgorithmDlg->SetSelectedInspection(iImageType, iInspectionType);
		m_pAlgorithmDlg->SetParam();

		ShowAlgorithmDlg();
	}
	catch (HException& except)
	{
		CString str;
		str.Format("Halcon Exception [CTabControlDlg::ShowAlgorithmWnd] : %s", except.message);
		THEAPP.SaveLog(str);
	}
}

void CTabControlDlg::UpdateList(int iImageType, int iInspectionType)
{

	m_pTeachParamDlg[iImageType]->m_pTeachParamDlg_Tab[iInspectionType]->UpdateList();
}

void CTabControlDlg::UpdateROIList(int iImageType, int iInspectionType)
{

	m_pTeachParamDlg[iImageType]->m_pTeachParamROIDlg_Tab[iInspectionType]->UpdateList();
}
