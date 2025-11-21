// MatchingParamAdvancedDlg.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "uScan.h"
#include "MatchingParamAdvancedDlg.h"
#include "afxdialogex.h"


// CMatchingParamAdvancedDlg 대화 상자입니다.

IMPLEMENT_DYNAMIC(CMatchingParamAdvancedDlg, CDialog)

CMatchingParamAdvancedDlg::CMatchingParamAdvancedDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CMatchingParamAdvancedDlg::IDD, pParent)
{
	m_iEditMatchingContrastC1 = 0;
	m_iEditMatchingContrastC2 = 0;
	m_iEditMatchingContrastC3 = 0;
	m_iEditMatchingContrastC4 = 0;

	// 24.06.28 - v2651 - ROI Templet Matching Align 추가 - LeeGW START
	m_bCheckUseBLROIAlign = FALSE;

	for (int cont = 0; cont < MAX_CONTOUR_NUM; cont++)
	{
		m_bCheckUseBLROIAlignContour[cont] = FALSE;
	}

	m_iEditBLROIAlignXOffset = 0;
	m_iEditBLROIAlignYOffset = 0;
	m_iEditBLROIAlignInnerMargin = 0;
	m_iEditBLROIAlignOuterMargin = 0;
	m_dEditBLROIAlignMatchingScore = 0.1;
	m_iEditBLROIAlignSearchingMargin = 0;

	// 24.07.01 - v2651 - ROI Templet Matching Align 추가 - LeeGW START
	for (int img = 0; img < BARREL_LENS_IMAGE_TAB; img++)
	{
		m_bUseBLROIAlign[img] = FALSE;

		for (int cont = 0; cont < MAX_CONTOUR_NUM; cont++)
		{
			m_bUseBLROIAlignContour[img][cont] = FALSE;
		}

		m_iBLROIAlignInnerMargin[img] = 0;
		m_iBLROIAlignOuterMargin[img] = 0;
	}

	m_iBeforeBLROIAlignImage = 0;
	// 24.06.28 - v2651 - ROI Templet Matching Align 추가 - LeeGW END
}

CMatchingParamAdvancedDlg::~CMatchingParamAdvancedDlg()
{
}

void CMatchingParamAdvancedDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_MATCHING_CONTRAST_C1, m_iEditMatchingContrastC1);
	DDX_Text(pDX, IDC_EDIT_MATCHING_CONTRAST_C2, m_iEditMatchingContrastC2);
	DDX_Text(pDX, IDC_EDIT_MATCHING_CONTRAST_C3, m_iEditMatchingContrastC3);
	DDX_Text(pDX, IDC_EDIT_MATCHING_CONTRAST_C4, m_iEditMatchingContrastC4);
	DDX_Check(pDX, IDC_CHECK_ROI_ALIGN_USE, m_bCheckUseBLROIAlign);
	DDX_Check(pDX, IDC_CHECK_ROI_ALIGN_CONTOUR1, m_bCheckUseBLROIAlignContour[0]);
	DDX_Check(pDX, IDC_CHECK_ROI_ALIGN_CONTOUR2, m_bCheckUseBLROIAlignContour[1]);
	DDX_Check(pDX, IDC_CHECK_ROI_ALIGN_CONTOUR3, m_bCheckUseBLROIAlignContour[2]);
	DDX_Check(pDX, IDC_CHECK_ROI_ALIGN_CONTOUR4, m_bCheckUseBLROIAlignContour[3]);
	DDX_Check(pDX, IDC_CHECK_ROI_ALIGN_CONTOUR5, m_bCheckUseBLROIAlignContour[4]);
	DDX_Text(pDX, IDC_EDIT_ROI_ALIGN_X_OFFSET, m_iEditBLROIAlignXOffset);
	DDX_Text(pDX, IDC_EDIT_ROI_ALIGN_Y_OFFSET, m_iEditBLROIAlignYOffset);
	DDX_Text(pDX, IDC_EDIT_ROI_ALIGN_INNER_MARGIN, m_iEditBLROIAlignInnerMargin);
	DDX_Text(pDX, IDC_EDIT_ROI_ALIGN_OUTER_MARGIN, m_iEditBLROIAlignOuterMargin);
	DDX_Text(pDX, IDC_EDIT_ROI_ALIGN_MATCHING_SCORE, m_dEditBLROIAlignMatchingScore);
	DDX_Text(pDX, IDC_EDIT_ROI_ALIGN_SEARCHING_MARGIN, m_iEditBLROIAlignSearchingMargin);
	DDX_Control(pDX, IDC_COMBO_ROI_ALIGN_IMAGE, m_cbBLROIAlignImage);
	
	// 24.07.01 - v2651 - ROI Templet Matching Align 추가 - LeeGW END

}


BEGIN_MESSAGE_MAP(CMatchingParamAdvancedDlg, CDialog)
	ON_BN_CLICKED(IDC_BN_OK, &CMatchingParamAdvancedDlg::OnBnClickedBnOk)
	ON_BN_CLICKED(IDC_BN_CANCLE, &CMatchingParamAdvancedDlg::OnBnClickedBnCancle)
	ON_BN_CLICKED(IDC_BUTTON_ROI_ALIGN_CREATE_MODEL, &CMatchingParamAdvancedDlg::OnBnClickedButtonRoiAlignCreateModel)
	ON_BN_CLICKED(IDC_BUTTON_ROI_ALIGN_RESULT_CHECK, &CMatchingParamAdvancedDlg::OnBnClickedButtonRoiAlignResultCheck)
	ON_BN_CLICKED(IDC_BUTTON_ROI_ALIGN_AREA_CHECK, &CMatchingParamAdvancedDlg::OnBnClickedButtonRoiAlignAreaCheck)
	ON_CBN_SELCHANGE(IDC_COMBO_ROI_ALIGN_IMAGE, &CMatchingParamAdvancedDlg::OnSelchangeComboRoiAlignImage)
	ON_BN_CLICKED(IDC_BUTTON_ROI_ALIGN_DELETE_MODEL, &CMatchingParamAdvancedDlg::OnBnClickedButtonRoiAlignDeleteModel)
	ON_BN_CLICKED(IDC_BN_OK2, &CMatchingParamAdvancedDlg::OnBnClickedBnOk2)
END_MESSAGE_MAP()


// CMatchingParamAdvancedDlg 메시지 처리기입니다.


BOOL CMatchingParamAdvancedDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	
	SetInspectParam(m_iBeforeBLROIAlignImage);
	m_cbBLROIAlignImage.SetCurSel(m_iBeforeBLROIAlignImage);
	LoadImageContours(m_iBeforeBLROIAlignImage);
	UpdateData(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

void CMatchingParamAdvancedDlg::OnBnClickedBnOk()
{
	UpdateData(TRUE);
	GetInspectParam(m_iBeforeBLROIAlignImage);
	LoadImageContours(THEAPP.m_pModelDataManager->m_iBlackMatchingImageType);
	CDialog::OnOK();
}

void CMatchingParamAdvancedDlg::OnBnClickedBnCancle()
{
	LoadImageContours(THEAPP.m_pModelDataManager->m_iBlackMatchingImageType);
	CDialog::OnCancel();
}

void CMatchingParamAdvancedDlg::LoadImageContours(int iImageNo)
{
	if (THEAPP.m_pGFunction->ValidHImage(THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[iImageNo]))
		copy_obj(THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[iImageNo], &THEAPP.m_pInspectAdminViewDlg->m_pHImage, 1, 1);

	for (int cont = 0; cont < MAX_CONTOUR_NUM; cont++)
	{
		if (THEAPP.m_pModelDataManager->m_bOneBarrel && cont < MATCHING_CONTOUR_3)
		{
			THEAPP.m_pInspectAdminViewDlg->ContoursAffinTrans[cont].Reset();
			continue;
		}
		copy_obj(THEAPP.m_pInspectAdminViewDlg->Contours[iImageNo][cont], &THEAPP.m_pInspectAdminViewDlg->ContoursAffinTrans[cont], 1, 1);
	}

	gen_empty_obj(&(THEAPP.m_pInspectAdminViewDlg->m_HInspectAreaRgn));

	THEAPP.m_pInspectAdminViewDlg->UpdateView();
}

void CMatchingParamAdvancedDlg::GetInspectParam(int iImageNo)
{
	THEAPP.m_pModelDataManager->m_iMatchingContrastC1 = m_iEditMatchingContrastC1;
	THEAPP.m_pModelDataManager->m_iMatchingContrastC2 = m_iEditMatchingContrastC2;
	THEAPP.m_pModelDataManager->m_iMatchingContrastC3 = m_iEditMatchingContrastC3;
	THEAPP.m_pModelDataManager->m_iMatchingContrastC4 = m_iEditMatchingContrastC4;

	// 24.07.01 - v2651 - ROI Templet Matching Align 추가 - LeeGW START
	m_bUseBLROIAlign[iImageNo] = m_bCheckUseBLROIAlign;

	for (int cont = 0; cont < MAX_CONTOUR_NUM; cont++)
	{
		m_bUseBLROIAlignContour[iImageNo][cont] = m_bCheckUseBLROIAlignContour[cont];
	}

	if (THEAPP.m_pModelDataManager->m_sModelName == ".") return;

	CString strModelFolder;
	if (!THEAPP.Struct_PreferenceStruct.m_bUseAbsolutePathModel)
		strModelFolder = THEAPP.GetWorkingDirectory() + "\\Model\\" + THEAPP.m_pModelDataManager->m_sModelName;
	else
		strModelFolder = THEAPP.GetWorkingEvmsDirectory() + "\\Model\\" + THEAPP.m_pModelDataManager->m_sModelName;
	CIniFileCS INI(strModelFolder + "\\SW\\ModelBase.ini");
	CString strSection = "Model Base";

	strSection = "ROI Align";

	CString key;
	for (int img = 0; img < BARREL_LENS_IMAGE_TAB; img++)
	{
		key.Format("m_bUseBLROIAlign_%d", img + 1);
		INI.Set_Bool(strSection, key, m_bUseBLROIAlign[img]);
		THEAPP.m_pModelDataManager->m_bUseBLROIAlign[img] = m_bUseBLROIAlign[img];
	}

	THEAPP.m_pModelDataManager->m_dBLROIAlignMatchingScore = m_dEditBLROIAlignMatchingScore;
	INI.Set_Double(strSection, "m_dBLROIAlignMatchingScore", m_dEditBLROIAlignMatchingScore);

	THEAPP.m_pModelDataManager->m_iBLROIAlignSearchingMargin = m_iEditBLROIAlignSearchingMargin;
	INI.Set_Integer(strSection, "m_iBLROIAlignSearchingMargin", m_iEditBLROIAlignSearchingMargin);
	// 24.07.01 - v2651 - ROI Templet Matching Align 추가 - LeeGW END


}

void CMatchingParamAdvancedDlg::SetInspectParam(int iImageNo)
{
	m_iEditMatchingContrastC1 = THEAPP.m_pModelDataManager->m_iMatchingContrastC1;
	m_iEditMatchingContrastC2 = THEAPP.m_pModelDataManager->m_iMatchingContrastC2;
	m_iEditMatchingContrastC3 = THEAPP.m_pModelDataManager->m_iMatchingContrastC3;
	m_iEditMatchingContrastC4 = THEAPP.m_pModelDataManager->m_iMatchingContrastC4;

	// 24.07.01 - v2651 - ROI Templet Matching Align 추가 - LeeGW START
	for (int img=0; img<BARREL_LENS_IMAGE_TAB; img++)
	{
		m_bUseBLROIAlign[img] = THEAPP.m_pModelDataManager->m_bUseBLROIAlign[img];

		for (int cont = 0; cont < MAX_CONTOUR_NUM; cont++)
		{	
			m_bUseBLROIAlignContour[img][cont] = THEAPP.m_pModelDataManager->m_bUseBLROIAlignContour[img][cont];
		}

		m_iBLROIAlignInnerMargin[img] = THEAPP.m_pModelDataManager->m_iBLROIAlignInnerMargin[img];
		m_iBLROIAlignOuterMargin[img] = THEAPP.m_pModelDataManager->m_iBLROIAlignOuterMargin[img];
	}

	m_bCheckUseBLROIAlign = m_bUseBLROIAlign[iImageNo];

	for (int cont = 0; cont < MAX_CONTOUR_NUM; cont++)
	{
		m_bCheckUseBLROIAlignContour[cont] = m_bUseBLROIAlignContour[iImageNo][cont];
	}
	
	m_iEditBLROIAlignInnerMargin = m_iBLROIAlignInnerMargin[iImageNo];
	m_iEditBLROIAlignOuterMargin= m_iBLROIAlignOuterMargin[iImageNo];
	m_dEditBLROIAlignMatchingScore = THEAPP.m_pModelDataManager->m_dBLROIAlignMatchingScore;
	m_iEditBLROIAlignSearchingMargin = THEAPP.m_pModelDataManager->m_iBLROIAlignSearchingMargin;
	// 24.07.01 - v2651 - ROI Templet Matching Align 추가 - LeeGW END
}

// 24.07.01 - v2651 - ROI Templet Matching Align 추가 - LeeGW START
void CMatchingParamAdvancedDlg::OnBnClickedButtonRoiAlignCreateModel()
{
try 
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	//BOOL bDebugSave = FALSE;

	int nRes = 0;
	nRes = AfxMessageBox("현재 설정으로 매칭 모델을 새로이 생성하시겠습니까?", MB_YESNO|MB_SYSTEMMODAL);

	if ((nRes!=IDYES))
		return;

	UpdateData();

	BOOL bCreationResult = TRUE;
	CString strLog, strCreationResult="", sTemp;
	double dTStart_depart = 0, dTEnd_depart = 0;
	dTStart_depart = GetTickCount();

	int iBLROIAlignImage = m_cbBLROIAlignImage.GetCurSel();

	BOOL bUseROIAlign[MAX_CONTOUR_NUM];
	for (int cont = 0; cont < MAX_CONTOUR_NUM; cont++)
	{
		bUseROIAlign[cont] = m_bCheckUseBLROIAlignContour[cont];
	}

	double dMatchingScore = m_dEditBLROIAlignMatchingScore;;
	int iOuterMargin = m_iEditBLROIAlignOuterMargin;
	int iInnerMargin = m_iEditBLROIAlignInnerMargin;

	// Get Teaching Image
	CString strImageName;
	CString strImageFolder;
	if(!THEAPP.Struct_PreferenceStruct.m_bUseAbsolutePathModel) //Ver2629
		strImageFolder = THEAPP.GetWorkingDirectory()+"\\Model\\"+THEAPP.m_pModelDataManager->m_sModelName + "\\SW\\TeachImage\\";
	else
		strImageFolder = THEAPP.GetWorkingEvmsDirectory()+"\\Model\\"+THEAPP.m_pModelDataManager->m_sModelName + "\\SW\\TeachImage\\";

	THEAPP.m_FileBase.CreatePath(strImageFolder ,TRUE); //Ver2629

	Hobject HImgRef[BARREL_LENS_IMAGE_TAB];
	for (int iImageIdx = 0; iImageIdx < BARREL_LENS_IMAGE_TAB; iImageIdx++)
	{
		gen_empty_obj(&HImgRef[iImageIdx]);

		strImageName.Format("Grab_%s.bmp", g_sImageName[THEAPP.m_iMachineInspType][iImageIdx]);
		read_image(&HImgRef[iImageIdx], strImageFolder + strImageName);
	}

	Hobject HMatchingImage, HMatchingImage_Process, HMatchingImage_Reduced;
	Hobject HMatchingRgn, HMatchingRgn_Diff, HMatchingRgn_Inner, HMatchingRgn_Outer;
	Hobject HMatchingContour;

	if (m_bCheckUseBLROIAlign==TRUE)
	{
		if (THEAPP.m_pGFunction->ValidHImage(HImgRef[iBLROIAlignImage]) == FALSE)
			return;

		for  (int cont=0; cont<MAX_CONTOUR_NUM; cont++) 
		{
			copy_obj(HImgRef[iBLROIAlignImage], &HMatchingImage, 1, 1);

			if (bUseROIAlign[cont] ==TRUE)
			{
				if (THEAPP.m_pGFunction->ValidHXLD(THEAPP.m_pInspectAdminViewDlg->ContoursAffinTrans[cont]) == FALSE)
					continue;

				copy_obj(THEAPP.m_pInspectAdminViewDlg->ContoursAffinTrans[cont], &HMatchingContour, 1, 1);

				binomial_filter(HMatchingImage, &HMatchingImage_Process, 5, 5);

				gen_region_contour_xld(HMatchingContour, &HMatchingRgn, "filled");
						
				dilation_circle (HMatchingRgn, &HMatchingRgn_Outer, iOuterMargin + 0.5 );
				erosion_circle (HMatchingRgn, &HMatchingRgn_Inner, iInnerMargin + 0.5 );
				difference(HMatchingRgn_Outer, HMatchingRgn_Inner, &HMatchingRgn_Diff);
				reduce_domain(HMatchingImage_Process, HMatchingRgn_Diff, &HMatchingImage_Reduced);

				create_ncc_model(HMatchingImage_Reduced, "auto", -0.39, 0.79, "auto", "use_polarity", &(THEAPP.m_pModelDataManager->m_iBLROIAlignMatchingModel[iBLROIAlignImage][cont]));
				if (THEAPP.m_pModelDataManager->m_iBLROIAlignMatchingModel[iBLROIAlignImage][cont] >= 0)
				{
					HTuple HRow, HCol, HRad, HScore, HScale, HAngle;
					Hlong lNoFoundNumber;

					HRow = HTuple(0.0);
					HCol = HTuple(0.0);
					HAngle = HTuple(0.0);
					HScale = HTuple(0.0);
					HScore = HTuple(0.0);

					find_ncc_model(HMatchingImage_Process,
						THEAPP.m_pModelDataManager->m_iBLROIAlignMatchingModel[iBLROIAlignImage][cont],	// Handle of the model
						-0.39,											// Smallest rotation of the model
						0.79,											// Extent of the rotation angles.
						dMatchingScore,	// Minumum score of the instances of the model to be found
						1,												// Number of instances of the model to be found
						0.5,											// Maximum overlap of the instances of the model to be found
						"true",											// Subpixel accuracy
						0,												// Number of pyramid levels used in the matching
						&HRow,											// Row coordinate of the found instances of the model.
						&HCol,										// Column coordinate of the found instances of the model.
						&HAngle,											// Rotation angle of the found instances of the model. (radian)
						&HScore);										// Score of the found instances of the model.
						
					tuple_length(HScore, &lNoFoundNumber);

					if (lNoFoundNumber>0)
					{
						// Save Teaching Param
						CString strModelFolder;
						if(!THEAPP.Struct_PreferenceStruct.m_bUseAbsolutePathModel)
							strModelFolder = THEAPP.GetWorkingDirectory()+"\\Model\\"+THEAPP.m_pModelDataManager->m_sModelName;
						else
							strModelFolder = THEAPP.GetWorkingEvmsDirectory()+"\\Model\\"+THEAPP.m_pModelDataManager->m_sModelName;
								
						CIniFileCS INI(strModelFolder+"\\SW\\ModelBase.ini");

						CString strSection = "ROI Align";

						CString strKey;

						for (int cont=0; cont<MAX_CONTOUR_NUM; cont++)
						{
							THEAPP.m_pModelDataManager->m_bUseBLROIAlignContour[iBLROIAlignImage][cont] = bUseROIAlign[cont];
							strKey.Format("m_bUseBLROIAlignContour_%d_%d", iBLROIAlignImage + 1, cont + 1);
							INI.Set_Bool(strSection, strKey, THEAPP.m_pModelDataManager->m_bUseBLROIAlignContour[iBLROIAlignImage][cont]);
						}

						THEAPP.m_pModelDataManager->m_iBLROIAlignInnerMargin[iBLROIAlignImage] = iInnerMargin;
						strKey.Format("m_iBLROIAlignInnerMargin_%d", iBLROIAlignImage + 1);
						INI.Set_Integer(strSection, strKey, THEAPP.m_pModelDataManager->m_iBLROIAlignInnerMargin[iBLROIAlignImage]);

						THEAPP.m_pModelDataManager->m_iBLROIAlignOuterMargin[iBLROIAlignImage] = iOuterMargin;
						strKey.Format("m_iBLROIAlignOuterMargin_%d", iBLROIAlignImage + 1);
						INI.Set_Integer(strSection, strKey, THEAPP.m_pModelDataManager->m_iBLROIAlignOuterMargin[iBLROIAlignImage]);

						CString strMatchingModelFolder;
						strMatchingModelFolder.Format("%s\\SW\\MatchingModel\\BLROIAlignModel_%d_%d", strModelFolder, iBLROIAlignImage + 1, cont + 1);
						write_ncc_model(THEAPP.m_pModelDataManager->m_iBLROIAlignMatchingModel[iBLROIAlignImage][cont], HTuple(strMatchingModelFolder));

						strLog.Format("Matching Model Creation Success : Image%d, Contour%d", iBLROIAlignImage + 1, cont + 1);

						bCreationResult &= TRUE;
					}
					else
					{
						strLog.Format("Matching Model Creation Failed : Image%d, Contour%d", iBLROIAlignImage + 1, cont + 1);

						bCreationResult &= FALSE;
					}


					sTemp.Format("%s\n%s", strCreationResult, strLog);
					strCreationResult = sTemp;

					THEAPP.SaveLog(strLog);
				}
			}	//if (THEAPP.m_pModelDataManager->m_bUseROIAlignContour[cont]==TRUE)
		}	// for  (int cont=0; cont<MAX_CONTOUR_NUM; cont++) 
	}	// if (THEAPP.m_pModelDataManager->m_bUseROIAlignImage[img]==TRUE)				


	if (bCreationResult==TRUE)
	{
		sTemp.Format("Matching Model Creation Success!.\n%s", strCreationResult);
	}
	else
	{
		sTemp.Format("Matching Model Creation Failed!.\n%s", strCreationResult);
	}

	UpdateData(FALSE);

	dTEnd_depart = GetTickCount();
	strLog.Format("  *-- Teaching Time %.0lf ms", dTEnd_depart-dTStart_depart);
	THEAPP.SaveLog(strLog);

	AfxMessageBox(sTemp, MB_SYSTEMMODAL | MB_ICONINFORMATION);

}
catch (HException &except)
{
	CString str;
	str.Format("Halcon Exception [OnBnClickedButtonRoiAlignCreateModel] : %s",except.message);
	THEAPP.SaveLog(str);

	AfxMessageBox("Matching Model Creation Failed!", MB_SYSTEMMODAL);
}
}

void CMatchingParamAdvancedDlg::OnBnClickedButtonRoiAlignResultCheck()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	try
	{
		// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
		//BOOL bDebugSave = FALSE;

		UpdateData();

		CString sTemp, strLog, strCreationResult;
		BOOL bCreationResult = m_bCheckUseBLROIAlign;
		double dMatchingScore = m_dEditBLROIAlignMatchingScore;;
		int iSearchingMargin = m_iEditBLROIAlignSearchingMargin;

		int iBLROIAlignImage = m_cbBLROIAlignImage.GetCurSel();

		if (m_bCheckUseBLROIAlign == TRUE)
		{
			if (THEAPP.m_pGFunction->ValidHImage(THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[iBLROIAlignImage]) == FALSE)
				return;

			for (int cont = 0; cont < MAX_CONTOUR_NUM; cont++)
			{
				if (THEAPP.m_pModelDataManager->m_bUseBLROIAlignContour[iBLROIAlignImage][cont] == TRUE)
				{
					if (THEAPP.m_pGFunction->ValidHXLD(THEAPP.m_pInspectAdminViewDlg->ContoursAffinTrans[cont]) == FALSE)
						continue;

					if (THEAPP.m_pModelDataManager->m_iBLROIAlignMatchingModel[iBLROIAlignImage][cont] < 0)
					{
						sTemp.Format("There are no matching models. First, create a matching model");
						AfxMessageBox(sTemp, MB_SYSTEMMODAL);
						return;
					}

					Hlong lNoFoundNumber = 0;
					HTuple HRowRef, HColRef, HRadRef;
					HTuple HRow, HCol, HAngle, HScore;
					Hobject HMatchingContour, HMatchingRgn, HMatchingImage_Reduced;

					copy_obj(THEAPP.m_pInspectAdminViewDlg->ContoursAffinTrans[cont], &HMatchingContour, 1, 1);

					gen_region_contour_xld(HMatchingContour, &HMatchingRgn, "filled");
					smallest_circle(HMatchingRgn, &HRowRef, &HColRef, &HRadRef);
					union1(HMatchingRgn, &HMatchingRgn);
					shape_trans(HMatchingRgn, &HMatchingRgn, "rectangle1");
					dilation_rectangle1(HMatchingRgn, &HMatchingRgn, iSearchingMargin * 2 + 1, iSearchingMargin * 2 + 1);
					reduce_domain(THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[iBLROIAlignImage], HMatchingRgn, &HMatchingImage_Reduced);
					binomial_filter(HMatchingImage_Reduced, &HMatchingImage_Reduced, 5, 5);

					HRow = HTuple(0.0);
					HCol = HTuple(0.0);
					HAngle = HTuple(0.0);
					HScore = HTuple(0.0);

					find_ncc_model(HMatchingImage_Reduced,
						THEAPP.m_pModelDataManager->m_iBLROIAlignMatchingModel[iBLROIAlignImage][cont],		// Handle of the model
						-0.39,														// Smallest rotation of the model
						0.79,														// Extent of the rotation angles.
						dMatchingScore,															// Minumum score of the instances of the model to be found
						1,																		// Number of instances of the model to be found
						0.5,																	// Maximum overlap of the instances of the model to be found
						"true",																	// Subpixel accuracy
						0,																		// Number of pyramid levels used in the matching
						&HRow,																	// Row coordinate of the found instances of the model.
						&HCol,																// Column coordinate of the found instances of the model.
						&HAngle,																	// Rotation angle of the found instances of the model. (radian)
						&HScore);																// Score of the found instances of the model.

					tuple_length(HScore, &lNoFoundNumber);

					if (lNoFoundNumber > 0)
					{
						double dRowRef, dColRef;
						double dDeltaY, dDeltaX, dAngle, dScore;
						double dTransX, dTransY, dRotAngle;

						HTuple HomMat2DIdentity, HomMat2DTranslate, HomMat2DRotate;
						Hobject HResultXLD;

						//hom_mat2d_identity(&HomMat2DIdentity);
						//hom_mat2d_translate(HomMat2DIdentity, HRow, HCol, &HomMat2DTranslate);
						//hom_mat2d_rotate(HomMat2DTranslate, HAngle, HRow, HCol, &HomMat2DRotate);

						dRowRef = HRowRef[0].D();
						dColRef = HColRef[0].D();
						dDeltaY = HRow[0].D();
						dDeltaX = HCol[0].D();
						dAngle = HAngle[0].D();
						dScore = HScore[0].D();

						gen_region_contour_xld(HMatchingContour, &HMatchingRgn, "filled");
						vector_angle_to_rigid(dRowRef, dColRef, 0, dDeltaY, dDeltaX, dAngle, &HomMat2DRotate);
						affine_trans_region(HMatchingRgn, &HResultXLD, HomMat2DRotate, "false");
						gen_contour_region_xld(HResultXLD, &HResultXLD, "border");

						copy_obj(HResultXLD, &THEAPP.m_pInspectAdminViewDlg->ContoursAffinTrans[cont], 1, 1);

						dTransX = dDeltaX - dColRef;
						dTransY = dDeltaY - dRowRef;
						tuple_deg(HAngle, &HAngle);
						dRotAngle = HAngle[0].D();

						strLog.Format("Matching Model Test Success : Image%d, Contour%d [(X,Y)Delta(px)=(%d,%d), Rotation(Deg)=(%.2lf), Score(%%)=%d]", iBLROIAlignImage + 1, cont + 1, (int)dTransX, (int)dTransY, dRotAngle, (int)(dScore*100.0));
						bCreationResult &= TRUE;
					}
					else
					{
						strLog.Format("Matching Model Test Failed : Image%d, Contour%d", iBLROIAlignImage + 1, cont + 1);
						bCreationResult &= FALSE;
					}

					sTemp.Format("%s\n%s", strCreationResult, strLog);
					strCreationResult = sTemp;

					THEAPP.SaveLog(strLog);

				}
			}
		}

		if (bCreationResult == TRUE)
		{
			sTemp.Format("Matching Model Test Success!.\n%s", strCreationResult);
		}
		else
		{
			sTemp.Format("Matching Model Test Failed!.\n%s", strCreationResult);
		}

		THEAPP.m_pInspectAdminViewDlg->UpdateView();
		THEAPP.m_pInspectAdminViewDlg->InvalidateRect(false);
		UpdateData(FALSE);

		AfxMessageBox(sTemp, MB_SYSTEMMODAL | MB_ICONINFORMATION);
	}
	catch (HException &except)
	{
		CString str;
		str.Format("Halcon Exception [OnBnClickedButtonRoiAlignMatchingTest] : %s", except.message);
		THEAPP.SaveLog(str);

		AfxMessageBox("Matching Failed!", MB_SYSTEMMODAL);
	}
}


void CMatchingParamAdvancedDlg::OnBnClickedButtonRoiAlignAreaCheck()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	UpdateData(TRUE);

	gen_empty_obj(&(THEAPP.m_pInspectAdminViewDlg->m_HInspectAreaRgn));

	Hobject HMatchingContour, HMatchingRgn_Outer, HMatchingRgn_Inner, HMatchingRgn;
	HTuple HRow, HCol, HRad;

	int iBLROIAlignImage = m_cbBLROIAlignImage.GetCurSel();

	for (int cont = 0; cont < MAX_CONTOUR_NUM; cont++)
	{
		if (THEAPP.m_pGFunction->ValidHXLD(THEAPP.m_pInspectAdminViewDlg->Contours[iBLROIAlignImage][cont]))
			copy_obj(THEAPP.m_pInspectAdminViewDlg->Contours[iBLROIAlignImage][cont], &THEAPP.m_pInspectAdminViewDlg->ContoursAffinTrans[cont], 1, 1);

		if (m_bCheckUseBLROIAlignContour[cont] == TRUE)
		{
			if (THEAPP.m_pGFunction->ValidHXLD(THEAPP.m_pInspectAdminViewDlg->ContoursAffinTrans[cont]) == FALSE)
				continue;

			gen_region_contour_xld(THEAPP.m_pInspectAdminViewDlg->ContoursAffinTrans[cont], &HMatchingRgn, "filled");
			smallest_circle(HMatchingRgn, &HRow, &HCol, &HRad);
 			gen_circle_contour_xld(&HMatchingContour, HRow + (HTuple)m_iEditBLROIAlignYOffset, HCol + (HTuple)m_iEditBLROIAlignXOffset, HRad, 0.0, 6.28318, "positive", 1.0);

			gen_region_contour_xld(HMatchingContour, &HMatchingRgn, "filled");
			dilation_circle(HMatchingRgn, &HMatchingRgn_Outer, m_iEditBLROIAlignOuterMargin + 0.5);
			erosion_circle(HMatchingRgn, &HMatchingRgn_Inner, m_iEditBLROIAlignInnerMargin + 0.5);

			concat_obj(THEAPP.m_pInspectAdminViewDlg->m_HInspectAreaRgn, HMatchingRgn_Outer, &(THEAPP.m_pInspectAdminViewDlg->m_HInspectAreaRgn));
			concat_obj(THEAPP.m_pInspectAdminViewDlg->m_HInspectAreaRgn, HMatchingRgn_Inner, &(THEAPP.m_pInspectAdminViewDlg->m_HInspectAreaRgn));
			copy_obj(HMatchingContour, &THEAPP.m_pInspectAdminViewDlg->ContoursAffinTrans[cont], 1, 1);
		}
	}

	THEAPP.m_pInspectAdminViewDlg->UpdateView();
}

void CMatchingParamAdvancedDlg::OnSelchangeComboRoiAlignImage()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	UpdateData();

	int iBLROIAlignImage = m_cbBLROIAlignImage.GetCurSel();

	m_bUseBLROIAlign[m_iBeforeBLROIAlignImage] = m_bCheckUseBLROIAlign;

	for (int cont = 0; cont < MAX_CONTOUR_NUM; cont++)
	{
		m_bUseBLROIAlignContour[m_iBeforeBLROIAlignImage][cont] = m_bCheckUseBLROIAlignContour[cont];
	}

	m_iBLROIAlignOuterMargin[m_iBeforeBLROIAlignImage] = m_iEditBLROIAlignOuterMargin;
	m_iBLROIAlignInnerMargin[m_iBeforeBLROIAlignImage] = m_iEditBLROIAlignInnerMargin;

	LoadImageContours(iBLROIAlignImage);

	m_bCheckUseBLROIAlign = m_bUseBLROIAlign[iBLROIAlignImage];

	for (int cont = 0; cont < MAX_CONTOUR_NUM; cont++)
	{
		m_bCheckUseBLROIAlignContour[cont] = m_bUseBLROIAlignContour[iBLROIAlignImage][cont];
	}

	m_iEditBLROIAlignOuterMargin = m_iBLROIAlignOuterMargin[iBLROIAlignImage];
	m_iEditBLROIAlignInnerMargin = m_iBLROIAlignInnerMargin[iBLROIAlignImage];

	UpdateData(FALSE);

	m_iBeforeBLROIAlignImage = iBLROIAlignImage;
}

void CMatchingParamAdvancedDlg::OnBnClickedButtonRoiAlignDeleteModel()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	UpdateData();

	int iBLROIAlignImage = m_cbBLROIAlignImage.GetCurSel();

	if (AfxMessageBox("현재 매칭 모델을 삭제하시겠습니까?", MB_OKCANCEL) == IDOK)
	{
		// Save Teaching Param
		CString strModelFolder;
		if (!THEAPP.Struct_PreferenceStruct.m_bUseAbsolutePathModel)
			strModelFolder = THEAPP.GetWorkingDirectory() + "\\Model\\" + THEAPP.m_pModelDataManager->m_sModelName;
		else
			strModelFolder = THEAPP.GetWorkingEvmsDirectory() + "\\Model\\" + THEAPP.m_pModelDataManager->m_sModelName;

		CIniFileCS INI(strModelFolder + "\\SW\\ModelBase.ini");
		CString strSection = "ROI Align";
		CString strKey;

		for (int cont = 0; cont < MAX_CONTOUR_NUM; cont++)
		{
			if (THEAPP.m_pModelDataManager->m_iBLROIAlignMatchingModel[iBLROIAlignImage][cont] > 0)
			{
				clear_ncc_model(THEAPP.m_pModelDataManager->m_iBLROIAlignMatchingModel[iBLROIAlignImage][cont]);

				THEAPP.m_pModelDataManager->m_iBLROIAlignMatchingModel[iBLROIAlignImage][cont] = -1;
			}

			THEAPP.m_pModelDataManager->m_bUseBLROIAlignContour[iBLROIAlignImage][cont] = m_bCheckUseBLROIAlignContour[cont] = FALSE;
			strKey.Format("m_bUseBLROIAlignContour_%d_%d", iBLROIAlignImage + 1, cont + 1);
			INI.Set_Bool(strSection, strKey, THEAPP.m_pModelDataManager->m_bUseBLROIAlignContour[iBLROIAlignImage][cont]);
		}

		THEAPP.m_pModelDataManager->m_iBLROIAlignInnerMargin[iBLROIAlignImage] = m_iBLROIAlignInnerMargin[iBLROIAlignImage] = 0;
		strKey.Format("m_iBLROIAlignInnerMargin_%d", iBLROIAlignImage + 1);
		INI.Set_Integer(strSection, strKey, THEAPP.m_pModelDataManager->m_iBLROIAlignInnerMargin[iBLROIAlignImage]);

		THEAPP.m_pModelDataManager->m_iBLROIAlignOuterMargin[iBLROIAlignImage] = m_iBLROIAlignOuterMargin[iBLROIAlignImage] = 0;
		strKey.Format("m_iBLROIAlignOuterMargin_%d", iBLROIAlignImage + 1);
		INI.Set_Integer(strSection, strKey, THEAPP.m_pModelDataManager->m_iBLROIAlignOuterMargin[iBLROIAlignImage]);
	}

	UpdateData(FALSE);
}


void CMatchingParamAdvancedDlg::OnBnClickedBnOk2()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	THEAPP.m_pTabControlDlg->ShowFirstPage();

	SetFocus();

	int iBLROIAlignImage = m_cbBLROIAlignImage.GetCurSel();
	LoadImageContours(iBLROIAlignImage);	
}
