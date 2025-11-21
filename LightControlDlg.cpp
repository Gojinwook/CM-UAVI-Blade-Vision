// LightControlDlg.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "uScan.h"
#include "LightControlDlg.h"
#include "afxdialogex.h"
#include "IniFileCS.h"

// CLightControlDlg 대화 상자입니다.
CLightControlDlg* CLightControlDlg::m_pInstance = NULL;

CLightControlDlg* CLightControlDlg::GetInstance(BOOL bShowFlag)
{
	if (!m_pInstance) {
		m_pInstance = new CLightControlDlg();
		if(!m_pInstance->m_hWnd) {
			CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
			m_pInstance->Create(IDD_LIGHT_CONTROL_DLG, pFrame->GetActiveView());
			if (bShowFlag) m_pInstance->Show();
		}
	}
	return m_pInstance;
}

void CLightControlDlg::DeleteInstance()
{
	THEAPP.m_pTabControlDlg->m_pLightAutoCalResultDlg->DeleteInstance();

	if (m_pInstance->GetSafeHwnd())
		m_pInstance->DestroyWindow();
	SAFE_DELETE(m_pInstance);
}

void CLightControlDlg::Show()
{
	ShowWindow(SW_HIDE);
	MoveWindow(m_ScreenRect.left, m_ScreenRect.top, m_ScreenRect.right, m_ScreenRect.bottom);
	ShowWindow(SW_SHOW);
}
IMPLEMENT_DYNAMIC(CLightControlDlg, CDialog)

CLightControlDlg::CLightControlDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CLightControlDlg::IDD, pParent)
{
	SetPosition(VIEW1_DLG3_LEFT+VIEW1_DLG3_WIDTH-420, VIEW1_DLG3_TOP+40+50, VIEW1_DLG3_WIDTH-608, VIEW1_DLG3_HEIGHT+170);

	m_iTeachingGrab = -1;

	m_iAcceptRange_Uniformity = 5;
}

CLightControlDlg::~CLightControlDlg()
{
}

void CLightControlDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_SLIDER_LIGHT1, m_SliderLight[0]);
	DDX_Control(pDX, IDC_SLIDER_LIGHT2, m_SliderLight[1]);
	DDX_Control(pDX, IDC_SLIDER_LIGHT3, m_SliderLight[2]);
	DDX_Control(pDX, IDC_SLIDER_LIGHT4, m_SliderLight[3]);
	DDX_Control(pDX, IDC_SLIDER_LIGHT5, m_SliderLight[4]);
	DDX_Text(pDX, IDC_EDIT_LIGHT1, m_iEditLight[0]);
	DDX_Text(pDX, IDC_EDIT_LIGHT2, m_iEditLight[1]);
	DDX_Text(pDX, IDC_EDIT_LIGHT3, m_iEditLight[2]);
	DDX_Text(pDX, IDC_EDIT_LIGHT4, m_iEditLight[3]);
	DDX_Text(pDX, IDC_EDIT_LIGHT5, m_iEditLight[4]);
	DDX_Text(pDX, IDC_EDIT_ACCEPTRANGE_UNIFORMITY, m_iAcceptRange_Uniformity);

	// 조명 페이지 설정 추가 - LeeGW
	DDX_Control(pDX, IDC_COMBO_PAGE_LIGHT_CTRL_1, m_iLightPageNo[0]);
	DDX_Control(pDX, IDC_COMBO_PAGE_LIGHT_CTRL_2, m_iLightPageNo[1]);
}


BEGIN_MESSAGE_MAP(CLightControlDlg, CDialog)
	ON_BN_CLICKED(IDC_GRAB, &CLightControlDlg::OnBnClickedGrab)
	ON_WM_HSCROLL()
	ON_WM_PAINT()
	ON_BN_CLICKED(IDC_BUTTON_CHANGE_LIGHTVALUE, &CLightControlDlg::OnBnClickedButtonChangeLightvalue)
	ON_BN_CLICKED(IDC_BUTTON_UUNIFORMITY, &CLightControlDlg::OnBnClickedButtonUuniformity)
	ON_BN_CLICKED(IDC_BUTTON_AVERAGE_LIGHT, &CLightControlDlg::OnBnClickedButtonAverageLight)
	ON_BN_CLICKED(IDC_CAL_TEST, &CLightControlDlg::OnBnClickedCalTest)
	ON_BN_CLICKED(IDC_BUTTON_SET_MATCHING_PARAM, &CLightControlDlg::OnBnClickedButtonSetMatchingParam)
	ON_BN_CLICKED(IDC_BUTTON_IMAGE_ALIGN, &CLightControlDlg::OnBnClickedButtonImageAlign)
END_MESSAGE_MAP()


// CLightControlDlg 메시지 처리기입니다.


void CLightControlDlg::OnBnClickedGrab()
{
#ifdef INLINE_MODE

	THEAPP.m_pInspectAdminViewDlg->StopLive();
	Sleep(100);

	DWORD dwGrabStart = 0, dwGrabEnd = 0;
	DWORD dwGrabElapsedTime = 0;

	if (THEAPP.m_pTabControlDlg->m_iCurrentTab == 0)
		return;

	m_iTeachingGrab = THEAPP.m_pTabControlDlg->m_iCurrentTab;
	THEAPP.m_pCameraManager->CallHookFunction();

	THEAPP.m_pTriggerManager->FireTrigger(VISION_TYPE_INSPECTION, m_iTeachingGrab-1);

	Sleep(THEAPP.Struct_PreferenceStruct.m_iTriggerSleepTime);

	dwGrabStart = GetTickCount();
	while (1)
	{
		if (THEAPP.m_pCameraManager->m_bGrabDone)
			break;

		dwGrabEnd = GetTickCount();

		if ((dwGrabEnd - dwGrabStart) > MAX_GRAB_TIMEOUT)
		{
			THEAPP.m_pCameraManager->GrabErrorPostProcess();
			break;
		}
	}

	copy_image(THEAPP.m_pInspectAdminViewDlg->m_pHImage, &(THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[m_iTeachingGrab - 1]));

	THEAPP.m_pInspectAdminViewDlg->InvalidateRect(false);

#endif
}


void CLightControlDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.


	switch(nSBCode)
	{
	case SB_LINELEFT:
		pScrollBar->SetScrollPos(pScrollBar->GetScrollPos()-1);
		break;
	case SB_LINERIGHT:
		pScrollBar->SetScrollPos(pScrollBar->GetScrollPos()+1);
		break;

	case SB_PAGELEFT:
		pScrollBar->SetScrollPos(pScrollBar->GetScrollPos()-1);
		break;
	case SB_PAGERIGHT:
		pScrollBar->SetScrollPos(pScrollBar->GetScrollPos()+1);
		break;
	case SB_THUMBTRACK:
		pScrollBar->SetScrollPos(nPos);
		break;

	}
	InvalidateRect(false);
	CDialog::OnHScroll(nSBCode, nPos, pScrollBar);
}


void CLightControlDlg::OnPaint()
{
	CPaintDC dc(this); // device context for painting


	CSliderCtrl  *pLightslider[MAX_LIGHT_CHANNEL];
	pLightslider[0] = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_LIGHT1);
	pLightslider[1] =(CSliderCtrl *)GetDlgItem(IDC_SLIDER_LIGHT2);
	pLightslider[2] =(CSliderCtrl *)GetDlgItem(IDC_SLIDER_LIGHT3);
	pLightslider[3] =(CSliderCtrl *)GetDlgItem(IDC_SLIDER_LIGHT4);
	pLightslider[4] =(CSliderCtrl *)GetDlgItem(IDC_SLIDER_LIGHT5);

	int strPos1,Pos2,Pos3,Pos4,Pos5;


	SetDlgItemInt(IDC_EDIT_LIGHT1,pLightslider[0]->GetPos());
	SetDlgItemInt(IDC_EDIT_LIGHT2,pLightslider[1]->GetPos());
	SetDlgItemInt(IDC_EDIT_LIGHT3,pLightslider[2]->GetPos());
	SetDlgItemInt(IDC_EDIT_LIGHT4,pLightslider[3]->GetPos());
	SetDlgItemInt(IDC_EDIT_LIGHT5,pLightslider[4]->GetPos());

}


BOOL CLightControlDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	if (THEAPP.m_iMachineInspType == MACHINE_WELDING)
	{
		GetDlgItem(IDC_STATIC_L1)->SetWindowTextA("Top 동축");
		GetDlgItem(IDC_STATIC_L2)->SetWindowTextA("Angle 35도 링");
		GetDlgItem(IDC_STATIC_L3)->SetWindowTextA("Angle 55도 링");
		GetDlgItem(IDC_STATIC_L4)->SetWindowTextA("Angle 80도 링");
		GetDlgItem(IDC_STATIC_L5)->SetWindowTextA("Bottom 동축");

		m_iLightPageNo[0].SetCurSel(0);
	}
	else
	{
		GetDlgItem(IDC_STATIC_L1)->SetWindowTextA("Top Dome");
		GetDlgItem(IDC_STATIC_L2)->SetWindowTextA("Top Ring");
		GetDlgItem(IDC_STATIC_L3)->SetWindowTextA("Mid 55도");
		GetDlgItem(IDC_STATIC_L4)->SetWindowTextA("Mid 65도");
		GetDlgItem(IDC_STATIC_L5)->SetWindowTextA("Mid 75도");

		GetDlgItem(IDC_COMBO_PAGE_LIGHT_CTRL_1)->EnableWindow(TRUE);
	}

	for (int iLightIdx = 0; iLightIdx < MAX_LIGHT_CHANNEL; iLightIdx++)
	{
		if (THEAPP.m_iMachineInspType == MACHINE_WELDING && 
			iLightIdx > 0 && iLightIdx < 5)
			m_SliderLight[iLightIdx].SetRange(0, 255);
		else
			m_SliderLight[iLightIdx].SetRange(0, LIGHT_BRIGHT_MAX);

	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}


void CLightControlDlg::OnBnClickedButtonChangeLightvalue()
{
	int nRes = 0;
	nRes = AfxMessageBox("조명 티칭 값이 변경되오니 주의하십시오.\n조명 티칭 값을 변경하시겠습니까?", MB_YESNO | MB_SYSTEMMODAL);

	if ((nRes != IDYES))
		return;

	UpdateData(TRUE);

	for (int iLightIdx = 0; iLightIdx < MAX_LIGHT_CHANNEL; iLightIdx++)
	{
		
		if (m_iEditLight[iLightIdx] < 0)
			m_iEditLight[iLightIdx] = 0;
		
		if (m_iEditLight[iLightIdx] > LIGHT_BRIGHT_MAX)
		{
			if (THEAPP.m_iMachineInspType == MACHINE_WELDING && 
				iLightIdx > 0 && iLightIdx < 5)
				m_iEditLight[iLightIdx] = 255;
			else
				m_iEditLight[iLightIdx] = LIGHT_BRIGHT_MAX;


		}
			

	}

	UpdateData(FALSE);

	for (int iLightIdx = 0; iLightIdx < MAX_LIGHT_CHANNEL; iLightIdx++)
	{
		m_SliderLight[iLightIdx].SetPos(m_iEditLight[iLightIdx]);
	}

#ifdef INLINE_MODE
	// New Light Controller

	if (THEAPP.m_iMachineInspType == MACHINE_WELDING)		// One Controller
	{
		// 조명 페이지 설정 추가 - LeeGW
		int iLightPageNo[MAX_LIGHT_PAGE];

		for (int iLightPage = 0; iLightPage < MAX_LIGHT_PAGE; iLightPage++)
		{
			if (iLightPageNo[iLightPage] = m_iLightPageNo[iLightPage].GetCurSel() == 0)
				iLightPageNo[iLightPage] = -1;
			else
				iLightPageNo[iLightPage] = m_iLightPageNo[iLightPage].GetCurSel() - 1;

			THEAPP.m_pModelDataManager->m_iLightPageNo[THEAPP.m_pTabControlDlg->m_iCurrentTab - 1][iLightPage] = iLightPageNo[iLightPage];
		}

		// Light 1~5
		if (iLightPageNo[1] >= 0)
		{
			THEAPP.m_pModelDataManager->m_iInspLightBright[THEAPP.m_pTabControlDlg->m_iCurrentTab - 1][0] = m_iEditLight[0];
			THEAPP.m_pModelDataManager->m_iInspLightBright[THEAPP.m_pTabControlDlg->m_iCurrentTab - 1][1] = m_iEditLight[1];
			THEAPP.m_pModelDataManager->m_iInspLightBright[THEAPP.m_pTabControlDlg->m_iCurrentTab - 1][2] = m_iEditLight[2];
			THEAPP.m_pModelDataManager->m_iInspLightBright[THEAPP.m_pTabControlDlg->m_iCurrentTab - 1][3] = m_iEditLight[3];
			THEAPP.m_pModelDataManager->m_iInspLightBright[THEAPP.m_pTabControlDlg->m_iCurrentTab - 1][4] = m_iEditLight[4];

			m_LightControl[1].m_Page[iLightPageNo[1]].uiChannel1 = m_iEditLight[0];
			m_LightControl[1].m_Page[iLightPageNo[1]].uiChannel2 = m_iEditLight[1];
			m_LightControl[1].m_Page[iLightPageNo[1]].uiChannel3 = m_iEditLight[2];
			m_LightControl[1].m_Page[iLightPageNo[1]].uiChannel4 = m_iEditLight[3];
			m_LightControl[1].m_Page[iLightPageNo[1]].uiChannel5 = m_iEditLight[4];
			m_LightControl[1].m_Page[iLightPageNo[1]].uiChannel6 = 0;
			if (m_LightControl[1].m_ComPort.OpenPort(1, 19200))	// Com1
			{
				m_LightControl[1].SetIllumination_6CH(iLightPageNo[1]);
				m_LightControl[1].m_ComPort.ClosePort();
			}
		}
	}
	else
	{
		// 조명 페이지 설정 추가 - LeeGW
		int iLightPageNo[MAX_LIGHT_PAGE];
		for (int iLightPage = 0; iLightPage < MAX_LIGHT_PAGE; iLightPage++)
		{

			if (iLightPageNo[iLightPage] = m_iLightPageNo[iLightPage].GetCurSel() == 0)
				iLightPageNo[iLightPage] = -1;
			else
				iLightPageNo[iLightPage] = m_iLightPageNo[iLightPage].GetCurSel() - 1;

			THEAPP.m_pModelDataManager->m_iLightPageNo[THEAPP.m_pTabControlDlg->m_iCurrentTab - 1][iLightPage] = iLightPageNo[iLightPage];
		}

		// Light 1
		if (iLightPageNo[0] >= 0)
		{
			THEAPP.m_pModelDataManager->m_iInspLightBright[THEAPP.m_pTabControlDlg->m_iCurrentTab - 1][0] = m_iEditLight[0];

			m_LightControl[0].m_Page[iLightPageNo[0]].uiChannel1 = m_iEditLight[0];
			m_LightControl[0].m_Page[iLightPageNo[0]].uiChannel2 = 0;
			m_LightControl[0].m_Page[iLightPageNo[0]].uiChannel3 = 0;
			m_LightControl[0].m_Page[iLightPageNo[0]].uiChannel4 = 0;
			if (m_LightControl[0].m_ComPort.OpenPort(1, 19200))	// Com1
			{
				m_LightControl[0].SetIllumination(iLightPageNo[0]);
				m_LightControl[0].m_ComPort.ClosePort();
			}
		}

		// Light 2~5
		if (iLightPageNo[1] >= 0)
		{
			THEAPP.m_pModelDataManager->m_iInspLightBright[THEAPP.m_pTabControlDlg->m_iCurrentTab - 1][1] = m_iEditLight[1];
			THEAPP.m_pModelDataManager->m_iInspLightBright[THEAPP.m_pTabControlDlg->m_iCurrentTab - 1][2] = m_iEditLight[2];
			THEAPP.m_pModelDataManager->m_iInspLightBright[THEAPP.m_pTabControlDlg->m_iCurrentTab - 1][3] = m_iEditLight[3];
			THEAPP.m_pModelDataManager->m_iInspLightBright[THEAPP.m_pTabControlDlg->m_iCurrentTab - 1][4] = m_iEditLight[4];

			m_LightControl[1].m_Page[iLightPageNo[1]].uiChannel1 = m_iEditLight[1];
			m_LightControl[1].m_Page[iLightPageNo[1]].uiChannel2 = m_iEditLight[2];
			m_LightControl[1].m_Page[iLightPageNo[1]].uiChannel3 = m_iEditLight[3];
			m_LightControl[1].m_Page[iLightPageNo[1]].uiChannel4 = m_iEditLight[4];
			if (m_LightControl[1].m_ComPort.OpenPort(2, 19200))	// Com2
			{
				m_LightControl[1].SetIllumination(iLightPageNo[1]);
				m_LightControl[1].m_ComPort.ClosePort();
			}
		}
	}


#endif
	// Inspection Mode  
	if(!THEAPP.m_pInspectSummary->m_bUniformityCheckMode)
	{
		//**********  조명 값을 파일에 써줌.
		CString strModelFolder;
		if(!THEAPP.Struct_PreferenceStruct.m_bUseAbsolutePathModel) //Ver2629
			strModelFolder = THEAPP.GetWorkingDirectory()+"\\Model\\"+THEAPP.m_pModelDataManager->m_sModelName;
		else
			strModelFolder = THEAPP.GetWorkingEvmsDirectory()+"\\Model\\"+THEAPP.m_pModelDataManager->m_sModelName;

		THEAPP.m_pModelDataManager->SaveLightInfo(strModelFolder+"\\HW\\InspectLightInfo.ini");
	}

}

void CLightControlDlg::UpdateLightValue(int iTabIdx)
{
	int iImageIdx;

	if (iTabIdx==0 || iTabIdx==1)
		iImageIdx = 0;
	else
		iImageIdx = iTabIdx - 1;

	for (int iLightIdx = 0; iLightIdx < MAX_LIGHT_CHANNEL; iLightIdx++)
	{
		m_iEditLight[iLightIdx] = THEAPP.m_pModelDataManager->m_iInspLightBright[iImageIdx][iLightIdx];
		m_SliderLight[iLightIdx].SetPos(m_iEditLight[iLightIdx]);
	}

	// 조명 페이지 설정 추가 - LeeGW

	if (THEAPP.m_iMachineInspType == MACHINE_WELDING)
	{
		m_iLightPageNo[0].SetCurSel(0);

		int iCtrlPageIdx = 1;
		m_iLightPageNo[iCtrlPageIdx].SetCurSel(THEAPP.m_pModelDataManager->m_iLightPageNo[iImageIdx][iCtrlPageIdx] + 1);
	}
	else
	{
		for (int iCtrlPageIdx = 0; iCtrlPageIdx < MAX_LIGHT_PAGE; iCtrlPageIdx++)
		{
			m_iLightPageNo[iCtrlPageIdx].SetCurSel(THEAPP.m_pModelDataManager->m_iLightPageNo[iImageIdx][iCtrlPageIdx] + 1);
		}
	}

	UpdateData(FALSE);
}

BOOL CLightControlDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	if(pMsg->message==WM_KEYDOWN&&pMsg->wParam==VK_ESCAPE)
		return TRUE;
	if(pMsg->message==WM_KEYDOWN&&pMsg->wParam==VK_RETURN)
		return TRUE;
	return CDialog::PreTranslateMessage(pMsg);
}

void CLightControlDlg::OnBnClickedButtonUuniformity()
{

try
{
	UpdateData(TRUE);
	Hobject HParameterDefectRgn;

	Hobject TempInspectImage[BARREL_LENS_IMAGE_TAB];

	CString strModelName = THEAPP.m_pModelDataManager->m_sModelName;
	CString strFolder;
	if(!THEAPP.Struct_PreferenceStruct.m_bUseAbsolutePathModel) //Ver2629
		strFolder = THEAPP.GetWorkingDirectory()+"\\Model\\";
	else
		strFolder = THEAPP.GetWorkingEvmsDirectory()+"\\Model\\";

	CString strImageFolder = strFolder+strModelName + "\\SW\\TeachImage\\";

	THEAPP.m_FileBase.CreatePath(strImageFolder ,TRUE); //Ver2629

	Hobject Reduced_InspectImage[BARREL_LENS_IMAGE_TAB], reduceedRegion;
	HTuple FitRow, FitColumn, FitRadius, FitStartPhi, FitEndPhi, FitPointOrder;

	Hobject Domain_InspectImage[BARREL_LENS_IMAGE_TAB], Partition_Region[BARREL_LENS_IMAGE_TAB], Rectangle[BARREL_LENS_IMAGE_TAB];
	HTuple Row1[BARREL_LENS_IMAGE_TAB], Column1[BARREL_LENS_IMAGE_TAB], Row2[BARREL_LENS_IMAGE_TAB], Column2[BARREL_LENS_IMAGE_TAB];

	fit_circle_contour_xld(THEAPP.m_pInspectAdminViewDlg->ContoursAffinTrans1_Uniformity, "algebraic", -1, 0, 0, 3, 2, &FitRow, &FitColumn, &FitRadius, &FitStartPhi, &FitEndPhi, &FitPointOrder);
	gen_region_contour_xld(THEAPP.m_pInspectAdminViewDlg->ContoursAffinTrans1_Uniformity, &reduceedRegion, "filled");

	CString strImageName;
	for (int iImgIdx = 0; iImgIdx < BARREL_LENS_IMAGE_TAB; iImgIdx++)
	{
		strImageName.Format("Grab_%s.bmp", g_sImageName[THEAPP.m_iMachineInspType][iImgIdx]);
		read_image(&TempInspectImage[iImgIdx], strImageFolder + strImageName);

		reduce_domain(TempInspectImage[iImgIdx], reduceedRegion, &Reduced_InspectImage[iImgIdx]);
		get_domain(Reduced_InspectImage[iImgIdx], &Domain_InspectImage[iImgIdx]);

		inner_rectangle1(Domain_InspectImage[iImgIdx], &Row1[iImgIdx], &Column1[iImgIdx], &Row2[iImgIdx], &Column2[iImgIdx]);
		gen_rectangle1(&Domain_InspectImage[iImgIdx], Row1[iImgIdx], Column1[iImgIdx], Row2[iImgIdx], Column2[iImgIdx]);

		partition_rectangle(Domain_InspectImage[iImgIdx], &Partition_Region[iImgIdx], (Column2[iImgIdx] - Column1[iImgIdx]) / 3, (Row2[iImgIdx] - Row1[iImgIdx]) / 3);

		connection(Partition_Region[iImgIdx], &Partition_Region[iImgIdx]);
	}

}
catch (HException &except)
{
	CString str;
	str.Format("Halcon Exception [OnBnClickedButtonUniformity] : %s", except.message);
	THEAPP.SaveLog(str);
	return ;
}
}

void CLightControlDlg::OnBnClickedButtonAverageLight()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if(THEAPP.m_pModelDataManager->m_sModelName==".")
		return;

	CLightAverageValueDlg dlg;
	dlg.DoModal();
}


void CLightControlDlg::OnBnClickedCalTest()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if(THEAPP.m_pModelDataManager->m_sModelName==".")
		return;

	THEAPP.m_pTabControlDlg->ShowLightAutoCalResultDlg();

}


void CLightControlDlg::OnBnClickedButtonSetMatchingParam()
{
	if(THEAPP.m_pModelDataManager->m_sModelName == ".") 
		return;

	int iCurImageIndex = THEAPP.m_pTabControlDlg->m_iCurrentTab-1;

	if (THEAPP.m_pGFunction->ValidHImage(THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[iCurImageIndex])==FALSE)
	{
		AfxMessageBox("티칭 영상 Grab 후 진행해 주세요.", MB_SYSTEMMODAL);
		return;
	}

	CMatchingLocalDlg dlg;
	dlg.DoModal();
}


void CLightControlDlg::OnBnClickedButtonImageAlign()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	int nRes = 0;
	nRes = AfxMessageBox("티칭영상이 변경되오니 주의하십시오.\n영상을 이동하시겠습니까?", MB_YESNO | MB_SYSTEMMODAL);

	if ((nRes != IDYES))
		return;

	int iImagIdx = THEAPP.m_pTabControlDlg->m_iCurrentTab - 1;
	if (THEAPP.m_pGFunction->ValidHImage(THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[iImagIdx]) == FALSE)
		return;

	if (THEAPP.m_pModelDataManager->m_bUseLocalAlignMatching[iImagIdx] == TRUE)
	{

		BOOL bMatchingSuccess = FALSE;
		double dRotationAngleDeg, dDeltaX, dDeltaY;
		double dStartTick, dEndTick;

		dStartTick = GetTickCount();
	
		bMatchingSuccess = THEAPP.m_pAlgorithm->ImageAlignShapeMatching(
			THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[iImagIdx],
			THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage,
			&(THEAPP.m_pModelDataManager->m_lLAlignModelID[iImagIdx]),
			THEAPP.m_pModelDataManager->m_bLocalAlignImage[iImagIdx],
			THEAPP.m_pModelDataManager->m_iLocalMatchingAngleRange[iImagIdx],
			THEAPP.m_pModelDataManager->m_dLocalMatchingScaleMin[iImagIdx],
			THEAPP.m_pModelDataManager->m_dLocalMatchingScaleMax[iImagIdx],
			THEAPP.m_pModelDataManager->m_dLocalMatchingScore[iImagIdx],
			THEAPP.m_pModelDataManager->m_dLocalTeachAlignRefX[iImagIdx],
			THEAPP.m_pModelDataManager->m_dLocalTeachAlignRefY[iImagIdx],
			THEAPP.m_pModelDataManager->m_iLocalMatchingMethod[iImagIdx],
			THEAPP.m_pModelDataManager->m_iLocalMatchingTeachingRectLTX[iImagIdx],
			THEAPP.m_pModelDataManager->m_iLocalMatchingTeachingRectLTY[iImagIdx],
			THEAPP.m_pModelDataManager->m_iLocalMatchingTeachingRectRBX[iImagIdx],
			THEAPP.m_pModelDataManager->m_iLocalMatchingTeachingRectRBY[iImagIdx],
			THEAPP.m_pModelDataManager->m_iLocalMatchingSearchMarginX[iImagIdx],
			THEAPP.m_pModelDataManager->m_iLocalMatchingSearchMarginY[iImagIdx],
			THEAPP.m_pModelDataManager->m_bUseMatchingAffineConstant[iImagIdx],
			&dRotationAngleDeg,
			&dDeltaX,
			&dDeltaY);

		THEAPP.m_pInspectAdminViewDlg->UpdateView();

		dEndTick = GetTickCount();

		if (bMatchingSuccess)
		{
			CString sMsg;
			sMsg.Format("(X,Y)Location(px)=(%d,%d), Angle(deg)=(%.2lf), Duration=%.3lfms", (int)dDeltaX, (int)dDeltaY, dRotationAngleDeg, dEndTick - dStartTick);
			AfxMessageBox(sMsg, MB_SYSTEMMODAL | MB_ICONINFORMATION);
		}
		else
		{
			CString sMsg;
			sMsg.Format("매칭 실패");
			AfxMessageBox(sMsg, MB_SYSTEMMODAL | MB_ICONINFORMATION);
		}
	}

}
