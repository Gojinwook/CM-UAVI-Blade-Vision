// MatchingDlg.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "uScan.h"
#include "MatchingDlg.h"
#include "IniFileCS.h"
#include "afxdialogex.h"


// CMatchingDlg 대화 상자입니다.
CMatchingDlg* CMatchingDlg::m_pInstance = NULL;

CMatchingDlg* CMatchingDlg::GetInstance(BOOL bShowFlag)
{
	if (!m_pInstance) {
		m_pInstance = new CMatchingDlg();
		if(!m_pInstance->m_hWnd) {
			CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
			m_pInstance->Create(IDD_MATCHING_DLG, pFrame->GetActiveView());
			if (bShowFlag) m_pInstance->Show();
		}
	}
	return m_pInstance;
}

void CMatchingDlg::DeleteInstance()
{
	if(m_pInstance->GetSafeHwnd())
		m_pInstance->DestroyWindow();
	SAFE_DELETE(m_pInstance);
}

void CMatchingDlg::Show()
{
	ShowWindow(SW_HIDE);
	MoveWindow(m_ScreenRect.left, m_ScreenRect.top, m_ScreenRect.right, m_ScreenRect.bottom);
	ShowWindow(SW_SHOW);
}
IMPLEMENT_DYNAMIC(CMatchingDlg, CDialog)

CMatchingDlg::CMatchingDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CMatchingDlg::IDD, pParent)
{
	SetPosition(VIEW1_DLG3_LEFT+VIEW1_DLG3_WIDTH-420, VIEW1_DLG3_TOP+40+50+VIEW1_DLG3_HEIGHT+270-100,VIEW1_DLG3_WIDTH-610, VIEW1_DLG3_HEIGHT+300);

	m_iRefContCol = 570;
	m_iRefContRow = 640;
	m_iRefContRad1 = 410;
	m_iRefContRad2 = 360;
	m_iRefContRad3 = 325;
	m_iRefContRad4 = 190;
	m_dContRatio23 = 0.91;
	m_dContRatio45 = 0.83;
	m_iRadioBlackMatchingImageType = BLACK_MATCHING_IMAGE_TYPE_LENS1;
	m_bCheckUseMatchingAngle = FALSE;
	m_bCheckUseMatchingScale = TRUE;
	m_iEditMatchingAngleRange = 10;
	m_iEditMatchingScaleMax = 0;
	m_iEditMatchingScaleMin = 0;
}

CMatchingDlg::~CMatchingDlg()
{
}

void CMatchingDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_MATCH_EDIT_CENTER_X, m_iRefContCol);
	DDX_Text(pDX, IDC_MATCH_EDIT_CENTER_Y, m_iRefContRow);
	DDX_Text(pDX, IDC_MATCH_EDIT_RADIUS1, m_iRefContRad1);
	DDX_Text(pDX, IDC_MATCH_EDIT_RADIUS2, m_iRefContRad2);
	DDX_Text(pDX, IDC_MATCH_EDIT_RADIUS3, m_iRefContRad3);
	DDX_Text(pDX, IDC_MATCH_EDIT_RADIUS4, m_iRefContRad4);
	DDX_Text(pDX, IDC_MATCH_EDIT_RADIUS_23, m_dContRatio23);
	DDX_Text(pDX, IDC_MATCH_EDIT_RADIUS_45, m_dContRatio45);

	DDX_Control(pDX, IDC_BN_CREATE_DONTCARE_ROI, m_bnCreateROI);
	DDX_Control(pDX, IDC_BN_DELETE_DONTCARE_ROI, m_bnDeleteROI);
	DDX_Control(pDX, IDC_BN_CREATE_POLYGON_DONTCARE_ROI, m_bnCreatePolygonROI);
	DDX_Control(pDX, IDC_BN_CREATE_POLYGON_DONTCARE_EDIT_ROI, m_bnCreatePolygonEditROI);
	DDX_Control(pDX, IDC_BN_RESET_POLYGON_DONTCARE_ROI, m_bnResetPolygonROI);
	DDX_Control(pDX, IDC_COMBO_CONTOUR_TYPE, m_cbContourType);
	DDX_Radio(pDX, IDC_RADIO_BLACK_MATCHING_IMAGE_BSURFACE, m_iRadioBlackMatchingImageType);
	DDX_Check(pDX, IDC_CHECK_USE_MATCHING_ANGLE, m_bCheckUseMatchingAngle);
	DDX_Check(pDX, IDC_CHECK_USE_MATCHING_SCALE, m_bCheckUseMatchingScale);
	DDX_Text(pDX, IDC_EDIT_MATCHING_ANGLE_RANGE, m_iEditMatchingAngleRange);
	DDX_Text(pDX, IDC_EDIT_MATCHING_SCALE_MAX, m_iEditMatchingScaleMax);
	DDX_Text(pDX, IDC_EDIT_MATCHING_SCALE_MIN, m_iEditMatchingScaleMin);
}


BEGIN_MESSAGE_MAP(CMatchingDlg, CDialog)
	ON_BN_CLICKED(IDC_BUTTON_MAKE_MATCHING_FILE, &CMatchingDlg::OnBnClickedButtonMakeMatchingFile)
	ON_BN_CLICKED(IDC_MATCH_BTN_PRESET_CONTOUR, &CMatchingDlg::OnBnClickedMatchBtnPresetContour)
	ON_BN_CLICKED(IDC_BUTTON_UNIFORMITY, &CMatchingDlg::OnBnClickedButtonUniformity)
	ON_BN_CLICKED(IDC_BN_SET_EXTRAPARAMETER, &CMatchingDlg::OnBnClickedBnSetExtraparameter)
	ON_BN_CLICKED(IDC_MATCH_BTN_SET_RATIIO, &CMatchingDlg::OnBnClickedMatchBtnSetRatio)
	ON_BN_CLICKED(IDC_BN_CREATE_DONTCARE_ROI, &CMatchingDlg::OnBnClickedBnCreateDontcareRoi)
	ON_BN_CLICKED(IDC_BN_DELETE_DONTCARE_ROI, &CMatchingDlg::OnBnClickedBnDeleteDontcareRoi)
	ON_BN_CLICKED(IDC_RADIO_BLACK_MATCHING_IMAGE_BSURFACE, &CMatchingDlg::OnBnClickedRadioBlackMatchingImageBsurface)
	ON_BN_CLICKED(IDC_RADIO_BLACK_MATCHING_IMAGE_LENS1, &CMatchingDlg::OnBnClickedRadioBlackMatchingImageLens1)
	ON_BN_CLICKED(IDC_CHECK_USE_MATCHING_ANGLE, &CMatchingDlg::OnBnClickedCheckUseMatchingAngle)
	ON_BN_CLICKED(IDC_CHECK_USE_MATCHING_SCALE, &CMatchingDlg::OnBnClickedCheckUseMatchingScale)
	ON_BN_CLICKED(IDC_RADIO_BLACK_MATCHING_IMAGE_BEDGE, &CMatchingDlg::OnBnClickedRadioBlackMatchingImageBedge)
	ON_BN_CLICKED(IDC_RADIO_BLACK_MATCHING_IMAGE_LENS2, &CMatchingDlg::OnBnClickedRadioBlackMatchingImageLens2)
	ON_BN_CLICKED(IDC_BUTTON_SET_CONTOUR_CONTRAST, &CMatchingDlg::OnBnClickedButtonSetContourContrast)
	ON_BN_CLICKED(IDC_BN_CREATE_POLYGON_DONTCARE_EDIT_ROI, &CMatchingDlg::OnBnClickedBnCreatePolygonDontcareEditRoi)
	ON_BN_CLICKED(IDC_BN_CREATE_POLYGON_DONTCARE_ROI, &CMatchingDlg::OnBnClickedBnCreatePolygonDontcareRoi)
	ON_BN_CLICKED(IDC_BN_RESET_POLYGON_DONTCARE_ROI, &CMatchingDlg::OnBnClickedBnResetPolygonDontcareRoi)
END_MESSAGE_MAP()


// CMatchingDlg 메시지 처리기입니다.
void CMatchingDlg::OnBnClickedButtonMakeMatchingFile()
{
try {

	///////////////////////////// Added for CMI3000 2000 ====> ////////////////////////////////////////////////////////////////////////
	///////// 피팅라인 매칭에 참고할 reference image를 얻는다 //////////
	if(THEAPP.m_pModelDataManager->m_sModelName == ".") 
		return;

	int nRes = 0;
	nRes = AfxMessageBox("현재 설정으로 매칭 파일을 새로이 생성하시겠습니까?", MB_YESNO|MB_SYSTEMMODAL);

	if ((nRes!=IDYES))
		return;

	UpdateData();

	CString strImageName;
	CString strImageFolder;
	if(!THEAPP.Struct_PreferenceStruct.m_bUseAbsolutePathModel) //Ver2629
		strImageFolder = THEAPP.GetWorkingDirectory()+"\\Model\\"+THEAPP.m_pModelDataManager->m_sModelName + "\\SW\\TeachImage\\";
	else
		strImageFolder = THEAPP.GetWorkingEvmsDirectory()+"\\Model\\"+THEAPP.m_pModelDataManager->m_sModelName + "\\SW\\TeachImage\\";

	THEAPP.m_FileBase.CreatePath(strImageFolder ,TRUE); //Ver2629

	Hobject HImgRef1, HImgRef2, HImgRef, HImgEdge, HImgLens2;
	gen_empty_obj(&HImgRef1); gen_empty_obj(&HImgRef2);
	gen_empty_obj(&HImgEdge); gen_empty_obj(&HImgLens2);

	// v3000
	strImageName = "Grab_Surface.bmp";
	read_image(&HImgRef1, strImageFolder+strImageName);
	strImageName = "Grab_Lens1.bmp";
	read_image(&HImgRef2, strImageFolder+strImageName);
	strImageName = "Grab_Edge.bmp";
	read_image(&HImgEdge, strImageFolder+strImageName);
	strImageName = "Grab_Lens2.bmp";
	read_image(&HImgLens2, strImageFolder+strImageName);

	//sub_image (HImgRef2, HImgRef1, &HImgRef, 1, 30);

	CString strModelFolder;
	if(!THEAPP.Struct_PreferenceStruct.m_bUseAbsolutePathModel) //Ver2629
		strModelFolder = THEAPP.GetWorkingDirectory()+"\\Model\\"+THEAPP.m_pModelDataManager->m_sModelName;
	else
		strModelFolder = THEAPP.GetWorkingEvmsDirectory()+"\\Model\\"+THEAPP.m_pModelDataManager->m_sModelName;

	CIniFileCS INI(strModelFolder+"\\SW\\ModelBase.ini");
	CString strSection = "Model Base";

	if (THEAPP.m_pInspectSummary->m_bUseAccelMatching == TRUE && THEAPP.m_pModelDataManager->m_bOneBarrel == FALSE)
	{
		THEAPP.m_pModelDataManager->m_dContRatio23 = m_dContRatio23;
		INI.Set_Double(strSection, "ContRatio23", m_dContRatio23);
	}
	THEAPP.m_pModelDataManager->m_dContRatio45 = m_dContRatio45;
	INI.Set_Double(strSection, "ContRatio45", m_dContRatio45);

	THEAPP.m_pModelDataManager->m_iBlackMatchingImageType = m_iRadioBlackMatchingImageType;
	INI.Set_Integer(strSection, "BlackBarrelMatchingImage", m_iRadioBlackMatchingImageType);

	// Add by go
	THEAPP.m_pModelDataManager->m_iRefContRow = m_iRefContRow;
	INI.Set_Integer(strSection, "MatchingContourCenterY", m_iRefContRow);
	THEAPP.m_pModelDataManager->m_iRefContCol = m_iRefContCol;
	INI.Set_Integer(strSection, "MatchingContourCenterX", m_iRefContCol);
	THEAPP.m_pModelDataManager->m_iRefContRad1 = m_iRefContRad1;
	INI.Set_Integer(strSection, "MatchingContour1Radius", m_iRefContRad1);
	THEAPP.m_pModelDataManager->m_iRefContRad2 = m_iRefContRad2;
	INI.Set_Integer(strSection, "MatchingContour2Radius", m_iRefContRad2);
	THEAPP.m_pModelDataManager->m_iRefContRad3 = m_iRefContRad3;
	INI.Set_Integer(strSection, "MatchingContour3Radius", m_iRefContRad3);
	THEAPP.m_pModelDataManager->m_iRefContRad4 = m_iRefContRad4;
	INI.Set_Integer(strSection, "MatchingContour4Radius", m_iRefContRad4);

	THEAPP.m_pModelDataManager->m_bUseMatchingAngle = m_bCheckUseMatchingAngle;
	INI.Set_Bool(strSection, "UseContour1MatchingAngle", m_bCheckUseMatchingAngle);

	THEAPP.m_pModelDataManager->m_bUseMatchingScale = m_bCheckUseMatchingScale;
	INI.Set_Bool(strSection, "UseScaleMatching", m_bCheckUseMatchingScale);

	THEAPP.m_pModelDataManager->m_iMatchingScaleMin = m_iEditMatchingScaleMin;
	INI.Set_Integer(strSection, "m_iMatchingScaleMin", m_iEditMatchingScaleMin);
	THEAPP.m_pModelDataManager->m_iMatchingScaleMax = m_iEditMatchingScaleMax;
	INI.Set_Integer(strSection, "m_iMatchingScaleMax", m_iEditMatchingScaleMax);

	THEAPP.m_pModelDataManager->m_iMatchingAngleRange = m_iEditMatchingAngleRange;
	INI.Set_Integer(strSection, "Contour1MatchingAngleRange", m_iEditMatchingAngleRange);

	// Matching Contrast
	INI.Set_Integer(strSection, "MatchingMinContrastContour1", THEAPP.m_pModelDataManager->m_iMatchingContrastC1);
	INI.Set_Integer(strSection, "MatchingMinContrastContour2", THEAPP.m_pModelDataManager->m_iMatchingContrastC2);
	INI.Set_Integer(strSection, "MatchingMinContrastContour3", THEAPP.m_pModelDataManager->m_iMatchingContrastC3);
	INI.Set_Integer(strSection, "MatchingMinContrastContour4", THEAPP.m_pModelDataManager->m_iMatchingContrastC4);

	double dAngleRangeRad;
	tuple_rad((double)THEAPP.m_pModelDataManager->m_iMatchingAngleRange, &dAngleRangeRad);

	if (THEAPP.m_pModelDataManager->m_bOneBarrel==FALSE)
	{
		if (THEAPP.m_pModelDataManager->m_bUseMatchingScale)
		{
			double dScaleMin = double(THEAPP.m_pModelDataManager->m_iMatchingScaleMin) * 0.01;
			double dScaleMax = double(THEAPP.m_pModelDataManager->m_iMatchingScaleMax) * 0.01;

			if (THEAPP.m_pModelDataManager->m_bUseMatchingAngle)
				create_scaled_shape_model_xld(THEAPP.m_pModelDataManager->ContoursOri[MATCHING_CONTOUR_1], "auto", -dAngleRangeRad, dAngleRangeRad*2.0, "auto", dScaleMin, dScaleMax, "auto", "auto", "ignore_local_polarity", THEAPP.m_pModelDataManager->m_iMatchingContrastC1, &THEAPP.m_pModelDataManager->ModelID[MATCHING_MODEL_1]);
			else
				create_scaled_shape_model_xld(THEAPP.m_pModelDataManager->ContoursOri[MATCHING_CONTOUR_1], "auto", 0, 0, "auto", dScaleMin, dScaleMax, "auto", "auto", "ignore_local_polarity", THEAPP.m_pModelDataManager->m_iMatchingContrastC1, &THEAPP.m_pModelDataManager->ModelID[MATCHING_MODEL_1]);

			create_scaled_shape_model_xld(THEAPP.m_pModelDataManager->ContoursOri[MATCHING_CONTOUR_2], "auto", 0, 0, "auto", dScaleMin, dScaleMax, "auto", "auto", "ignore_local_polarity", THEAPP.m_pModelDataManager->m_iMatchingContrastC2, &THEAPP.m_pModelDataManager->ModelID[MATCHING_MODEL_2]);

			create_scaled_shape_model_xld (THEAPP.m_pModelDataManager->ContoursOri[MATCHING_CONTOUR_3], "auto", 0, 0, "auto", dScaleMin, dScaleMax, "auto", "auto", "ignore_local_polarity", THEAPP.m_pModelDataManager->m_iMatchingContrastC3, &THEAPP.m_pModelDataManager->ModelID[MATCHING_MODEL_3]);
			create_scaled_shape_model_xld (THEAPP.m_pModelDataManager->ContoursOri[MATCHING_CONTOUR_4], "auto", 0, 0, "auto", dScaleMin, dScaleMax, "auto", "auto", "ignore_local_polarity", THEAPP.m_pModelDataManager->m_iMatchingContrastC4, &THEAPP.m_pModelDataManager->ModelID[MATCHING_MODEL_4]);

		}
		else
		{
			if (THEAPP.m_pModelDataManager->m_bUseMatchingAngle)
				create_shape_model_xld(THEAPP.m_pModelDataManager->ContoursOri[MATCHING_CONTOUR_1], "auto", -dAngleRangeRad, dAngleRangeRad*2.0, "auto", "auto", "ignore_local_polarity", THEAPP.m_pModelDataManager->m_iMatchingContrastC1, &THEAPP.m_pModelDataManager->ModelID[MATCHING_MODEL_1]);
			else
				create_shape_model_xld(THEAPP.m_pModelDataManager->ContoursOri[MATCHING_CONTOUR_1], "auto", 0, 0, "auto", "auto", "ignore_local_polarity", THEAPP.m_pModelDataManager->m_iMatchingContrastC1, &THEAPP.m_pModelDataManager->ModelID[MATCHING_MODEL_1]);

			create_shape_model_xld(THEAPP.m_pModelDataManager->ContoursOri[MATCHING_CONTOUR_2], "auto", 0, 0, "auto", "auto", "ignore_local_polarity", THEAPP.m_pModelDataManager->m_iMatchingContrastC2, &THEAPP.m_pModelDataManager->ModelID[MATCHING_MODEL_2]);

			create_shape_model_xld(THEAPP.m_pModelDataManager->ContoursOri[MATCHING_CONTOUR_3], "auto", 0, 0, "auto", "auto", "ignore_local_polarity", THEAPP.m_pModelDataManager->m_iMatchingContrastC3, &THEAPP.m_pModelDataManager->ModelID[MATCHING_MODEL_3]);
			create_shape_model_xld(THEAPP.m_pModelDataManager->ContoursOri[MATCHING_CONTOUR_4], "auto", 0, 0, "auto", "auto", "ignore_local_polarity", THEAPP.m_pModelDataManager->m_iMatchingContrastC4, &THEAPP.m_pModelDataManager->ModelID[MATCHING_MODEL_4]);

		}
	}
	else
	{
		if (THEAPP.m_pModelDataManager->m_bUseMatchingScale)
		{
			double dScaleMin = double(THEAPP.m_pModelDataManager->m_iMatchingScaleMin) * 0.01;
			double dScaleMax = double(THEAPP.m_pModelDataManager->m_iMatchingScaleMax) * 0.01;

			if (THEAPP.m_pModelDataManager->m_bUseMatchingAngle)
				create_scaled_shape_model_xld (THEAPP.m_pModelDataManager->ContoursOri[MATCHING_CONTOUR_3], "auto", -dAngleRangeRad, dAngleRangeRad*2.0, "auto", dScaleMin, dScaleMax, "auto", "auto", "ignore_local_polarity", THEAPP.m_pModelDataManager->m_iMatchingContrastC3, &THEAPP.m_pModelDataManager->ModelID[MATCHING_MODEL_3]);
			else
				create_scaled_shape_model_xld (THEAPP.m_pModelDataManager->ContoursOri[MATCHING_CONTOUR_3], "auto", 0, 0, "auto", dScaleMin, dScaleMax, "auto", "auto", "ignore_local_polarity", THEAPP.m_pModelDataManager->m_iMatchingContrastC3, &THEAPP.m_pModelDataManager->ModelID[MATCHING_MODEL_3]);

			create_scaled_shape_model_xld (THEAPP.m_pModelDataManager->ContoursOri[MATCHING_CONTOUR_4], "auto", 0, 0, "auto", dScaleMin, dScaleMax, "auto", "auto", "ignore_local_polarity", THEAPP.m_pModelDataManager->m_iMatchingContrastC4, &THEAPP.m_pModelDataManager->ModelID[MATCHING_MODEL_4]);
		}
		else
		{
			if (THEAPP.m_pModelDataManager->m_bUseMatchingAngle)
				create_shape_model_xld(THEAPP.m_pModelDataManager->ContoursOri[MATCHING_CONTOUR_3], "auto", -dAngleRangeRad, dAngleRangeRad*2.0, "auto", "auto", "ignore_local_polarity", THEAPP.m_pModelDataManager->m_iMatchingContrastC3, &THEAPP.m_pModelDataManager->ModelID[MATCHING_MODEL_3]);
			else
				create_shape_model_xld(THEAPP.m_pModelDataManager->ContoursOri[MATCHING_CONTOUR_3], "auto", 0, 0, "auto", "auto", "ignore_local_polarity", THEAPP.m_pModelDataManager->m_iMatchingContrastC3, &THEAPP.m_pModelDataManager->ModelID[MATCHING_MODEL_3]);

			create_shape_model_xld(THEAPP.m_pModelDataManager->ContoursOri[MATCHING_CONTOUR_4], "auto", 0, 0, "auto", "auto", "ignore_local_polarity", THEAPP.m_pModelDataManager->m_iMatchingContrastC4, &THEAPP.m_pModelDataManager->ModelID[MATCHING_MODEL_4]);
		}
	}

	double dTStart_depart = 0, dTEnd_depart = 0;
	dTStart_depart = GetTickCount();

	double dContourCenterX, dContourCenterY;
	dContourCenterX = dContourCenterY = -1;

	BOOL bMchSuccess = THEAPP.m_pAlgorithm->MatchingAlgorithm(  // 매칭 
		HImgRef1, HImgRef2, HImgEdge, HImgLens2,
		&THEAPP.m_pModelDataManager->ContoursOri[MATCHING_CONTOUR_1],
		&THEAPP.m_pModelDataManager->ContoursOri[MATCHING_CONTOUR_2],
		&THEAPP.m_pModelDataManager->ContoursOri[MATCHING_CONTOUR_3],
		&THEAPP.m_pModelDataManager->ContoursOri[MATCHING_CONTOUR_4],
		&THEAPP.m_pModelDataManager->ContoursOri[MATCHING_CONTOUR_5],
		&THEAPP.m_pModelDataManager->ModelID[MATCHING_MODEL_1],
		&THEAPP.m_pModelDataManager->ModelID[MATCHING_MODEL_2],
		&THEAPP.m_pModelDataManager->ModelID[MATCHING_MODEL_3],
		&THEAPP.m_pModelDataManager->ModelID[MATCHING_MODEL_4],
		&dContourCenterX,
		&dContourCenterY);
	THEAPP.SaveLog("매칭 끝");

	CString sInspectTime;
	dTEnd_depart = GetTickCount();
	sInspectTime.Format("  *-- 티칭 매칭 시간 %.0lf ms", dTEnd_depart-dTStart_depart);
	THEAPP.SaveLog(sInspectTime);

	if(bMchSuccess == TRUE)
	{
		for (int cont = 0; cont < MAX_CONTOUR_NUM; cont++)
		{
			if (THEAPP.m_pModelDataManager->m_bOneBarrel && cont < MATCHING_CONTOUR_3)
			{
				THEAPP.m_pInspectAdminViewDlg->ContoursAffinTrans[cont].Reset();
				continue;
			}

			copy_obj(THEAPP.m_pModelDataManager->ContoursOri[cont], &THEAPP.m_pInspectAdminViewDlg->ContoursAffinTrans[cont], 1, 1);
		}

		THEAPP.m_pInspectAdminViewDlg->InvalidateRect(false);

		///////////////////////////////////////////////////////////////matching model 저장
		Hobject HImage;
		CString strFolder;
		if(!THEAPP.Struct_PreferenceStruct.m_bUseAbsolutePathModel) //Ver2629
			strFolder = THEAPP.GetWorkingDirectory() + "\\Model\\";
		else
			strFolder = THEAPP.GetWorkingEvmsDirectory() + "\\Model\\";

		CString strMatchingModelFolder = strFolder + THEAPP.m_pModelDataManager->m_sModelName + "\\SW\\MatchingModel\\";
		CString FileName1, FileName2, FileName3, FileName4, FileName5;
		FileName1 = "MatModel_1.xld";
		FileName2 = "MatModel_2.xld";
		FileName3 = "MatModel_3.xld";
		FileName4 = "MatModel_4.xld";
		FileName5 = "MatModel_5.xld";

		if(THEAPP.m_pModelDataManager->m_bOneBarrel==FALSE)
		{
			write_contour_xld_arc_info(THEAPP.m_pInspectAdminViewDlg->ContoursAffinTrans[MATCHING_CONTOUR_1], strMatchingModelFolder+FileName1);
			write_contour_xld_arc_info(THEAPP.m_pInspectAdminViewDlg->ContoursAffinTrans[MATCHING_CONTOUR_2], strMatchingModelFolder+FileName2);
		}
		write_contour_xld_arc_info(THEAPP.m_pInspectAdminViewDlg->ContoursAffinTrans[MATCHING_CONTOUR_3], strMatchingModelFolder+FileName3);
		write_contour_xld_arc_info(THEAPP.m_pInspectAdminViewDlg->ContoursAffinTrans[MATCHING_CONTOUR_4], strMatchingModelFolder+FileName4);
		if(THEAPP.m_pGFunction->ValidHXLD(THEAPP.m_pInspectAdminViewDlg->ContoursAffinTrans[MATCHING_CONTOUR_5]))
			write_contour_xld_arc_info(THEAPP.m_pInspectAdminViewDlg->ContoursAffinTrans[MATCHING_CONTOUR_5], strMatchingModelFolder+FileName5);
		///////////////////////////////////////////////////////////////matching model 저장
			
		THEAPP.m_pInspectAdminViewDlg->m_pThresRgn.Reset();
		THEAPP.m_pInspectAdminViewDlg->m_HCropMatchingImage.Reset();
		gen_empty_obj(&THEAPP.m_pInspectAdminViewDlg->m_pThresRgn);
		gen_empty_obj(&THEAPP.m_pInspectAdminViewDlg->m_HCropMatchingImage);
		THEAPP.m_pInspectAdminViewDlg->InvalidateRect(false);
		MessageBox("매칭 성공");
	}
	else
	{
		THEAPP.SaveLog("매칭실패");
		MessageBox("매칭실패. 매칭을 다시 해주세요.");
	}
}
catch (HException &except)
{
	CString str;
	str.Format("Halcon Exception [OnBnClickedButtonMakeMatchingFile] : %s", except.message);
	THEAPP.SaveLog(str);
	MessageBox("매칭실패. 매칭을 다시 해주세요.");
}
}


////////////////////////////////// Added for CMI3000 2000 ====> ///////////////////////////////////////////////////
// Fitting line 을 메뉴얼로 대략 그린다
void CMatchingDlg::OnBnClickedMatchBtnPresetContour()
{
	UpdateData(TRUE);

	if (THEAPP.Struct_PreferenceStruct.m_iCamFOVType == CAM_FOV_CROP)
	{
		if (m_iRefContCol < 0) m_iRefContCol = 0; if (m_iRefContCol > GRAB_X_MAX) m_iRefContCol = GRAB_X_MAX;
		if (m_iRefContRow < 0) m_iRefContRow = 0; if (m_iRefContRow > GRAB_Y_MAX) m_iRefContRow = GRAB_Y_MAX;
	}
	else
	{
		if (m_iRefContCol < 0) m_iRefContCol = 0; if (m_iRefContCol > THEAPP.m_pCameraManager->GetCamImageWidth()) m_iRefContCol = THEAPP.m_pCameraManager->GetCamImageWidth();
		if (m_iRefContRow < 0) m_iRefContRow = 0; if (m_iRefContRow > THEAPP.m_pCameraManager->GetCamImageHeight()) m_iRefContRow = THEAPP.m_pCameraManager->GetCamImageHeight();
	}
	if (m_iRefContRad1 < 0) m_iRefContRad1 = 0;
	if (m_iRefContRad2 < 0) m_iRefContRad2 = 0;
	if (m_iRefContRad3 < 0) m_iRefContRad3 = 0;
	if (m_iRefContRad4 < 0) m_iRefContRad4 = 0;
	UpdateData(FALSE);

try
{
	///////// 피팅라인을 얻는다 ///////////////////////////////////////////////////////////////////
	gen_circle_contour_xld (&THEAPP.m_pModelDataManager->ContoursOri[MATCHING_CONTOUR_1], (double)m_iRefContRow, (double)m_iRefContCol, (double)m_iRefContRad1, 0.0, 6.28318, "positive", 1.0);
	gen_circle_contour_xld (&THEAPP.m_pModelDataManager->ContoursOri[MATCHING_CONTOUR_2], (double)m_iRefContRow, (double)m_iRefContCol, (double)m_iRefContRad2, 0.0, 6.28318, "positive", 1.0);
	gen_circle_contour_xld (&THEAPP.m_pModelDataManager->ContoursOri[MATCHING_CONTOUR_3], (double)m_iRefContRow, (double)m_iRefContCol, (double)m_iRefContRad3, 0.0, 6.28318, "positive", 1.0);
	gen_circle_contour_xld (&THEAPP.m_pModelDataManager->ContoursOri[MATCHING_CONTOUR_4], (double)m_iRefContRow, (double)m_iRefContCol, (double)m_iRefContRad4, 0.0, 6.28318, "positive", 1.0);

	if (THEAPP.m_pModelDataManager->m_bOneBarrel==FALSE)
	{
		copy_obj(THEAPP.m_pModelDataManager->ContoursOri[MATCHING_CONTOUR_1], &THEAPP.m_pInspectAdminViewDlg->ContoursAffinTrans[MATCHING_CONTOUR_1], 1, 1);
		copy_obj(THEAPP.m_pModelDataManager->ContoursOri[MATCHING_CONTOUR_2], &THEAPP.m_pInspectAdminViewDlg->ContoursAffinTrans[MATCHING_CONTOUR_2], 1, 1);
	}
	else
	{
		THEAPP.m_pInspectAdminViewDlg->ContoursAffinTrans[MATCHING_CONTOUR_1].Reset();
		THEAPP.m_pInspectAdminViewDlg->ContoursAffinTrans[MATCHING_CONTOUR_2].Reset();
	}

	copy_obj(THEAPP.m_pModelDataManager->ContoursOri[MATCHING_CONTOUR_3], &THEAPP.m_pInspectAdminViewDlg->ContoursAffinTrans[MATCHING_CONTOUR_3], 1, 1);
	copy_obj(THEAPP.m_pModelDataManager->ContoursOri[MATCHING_CONTOUR_4], &THEAPP.m_pInspectAdminViewDlg->ContoursAffinTrans[MATCHING_CONTOUR_4], 1, 1);

	THEAPP.m_pInspectAdminViewDlg->InvalidateRect(false);
}
catch (HException &except)
{
	CString str;
	str.Format("Halcon Exception [Make_Matching Model: OnBnClickedMatchBtnPresetContour] : %s", except.message);
	THEAPP.SaveLog(str);
}
}


// 다른 라인을 참조하여 부가적인 Fitting line 을 생성한다
void CMatchingDlg::OnBnClickedMatchBtnSetRatio()
{
	UpdateData(TRUE);

try
{
	if (THEAPP.m_pInspectSummary->m_bUseAccelMatching == TRUE && THEAPP.m_pModelDataManager->m_bOneBarrel == FALSE)		// 매칭 가속 옵션 사용 시 하단 베럴 내곽라인(contour2)은 contour3의 비례값으로 만든다
	{
		Hobject HRgn2, HRgn3;
		double d3CenterR = 0, d3CenterC = 0, d3Radius = 0;

		gen_region_contour_xld (THEAPP.m_pInspectAdminViewDlg->ContoursAffinTrans[MATCHING_CONTOUR_3], &HRgn3, "filled");
		smallest_circle (HRgn3, &d3CenterR, &d3CenterC, &d3Radius);
		
		double d2Radius = d3Radius/m_dContRatio23, d3Margin = d2Radius - d3Radius;
		dilation_circle (HRgn3, &HRgn2, d3Margin);
		gen_contour_region_xld (HRgn2, &THEAPP.m_pInspectAdminViewDlg->ContoursAffinTrans[MATCHING_CONTOUR_2], "border");
	}


	Hobject HRgn4, HRgn5;
	double d4CenterR = 0, d4CenterC = 0, d4Radius = 0;

	gen_region_contour_xld (THEAPP.m_pInspectAdminViewDlg->ContoursAffinTrans[MATCHING_CONTOUR_4], &HRgn4, "filled");
	smallest_circle (HRgn4, &d4CenterR, &d4CenterC, &d4Radius);
		
	double d5Radius = d4Radius*m_dContRatio45, d4Margin = d4Radius - d5Radius;
	erosion_circle (HRgn4, &HRgn5, d4Margin);

	smallest_circle (HRgn5, &d4CenterR, &d4CenterC, &d4Radius);
	gen_circle(&HRgn5, d4CenterR, d4CenterC, d4Radius);

	Hobject AAAA; gen_empty_obj(&AAAA);
	gen_contour_region_xld (HRgn5, &AAAA, "border");
	copy_obj(AAAA, &THEAPP.m_pInspectAdminViewDlg->ContoursAffinTrans[MATCHING_CONTOUR_5], 1, 1);


	THEAPP.m_pInspectAdminViewDlg->InvalidateRect(false);
}
catch (HException &except)
{
	CString str;
	str.Format("Halcon Exception [Make_Matching Model: OnBnClickedMatchBtnPresetContour] : %s", except.message);
	THEAPP.SaveLog(str);
}
}


////////////////////////////////// <==== Added for CMI3000 2000 ///////////////////////////////////////////////////


BOOL CMatchingDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	if(pMsg->message==WM_KEYDOWN&&pMsg->wParam==VK_ESCAPE)
		return TRUE;
	if(pMsg->message==WM_KEYDOWN&&pMsg->wParam==VK_RETURN)
		return TRUE;
	return CDialog::PreTranslateMessage(pMsg);
}


void CMatchingDlg::OnBnClickedButtonUniformity()
{
	try{

	}
	catch(HException &except)
	{
		CString str;
		str.Format("Halcon Exception [Make_Matching Model] : %s", except.message);
		THEAPP.SaveLog(str);
		MessageBox("매칭실패. 매칭을 다시 해주세요.");
	}
}
#include "InspectionParameterExtraDlg.h"

void CMatchingDlg::OnBnClickedBnSetExtraparameter()
{
	CInspectionParameterExtraDlg dlg;
	dlg.DoModal();
}

void CMatchingDlg::OnBnClickedBnCreateDontcareRoi()
{
	THEAPP.m_pInspectAdminViewDlg->SetXLDEditMode(m_cbContourType.GetCurSel());
}


void CMatchingDlg::OnBnClickedBnDeleteDontcareRoi()
{
	int nRes = 0;
	nRes = AfxMessageBox("현재 표시된 최외곽 Contour 라인을 매칭라인으로 저장?", MB_YESNO|MB_SYSTEMMODAL|MB_ICONWARNING);

	if ((nRes!=IDYES))
		return;

	if (THEAPP.m_pModelDataManager->m_bOneBarrel==FALSE)
		copy_obj(THEAPP.m_pInspectAdminViewDlg->ContoursAffinTrans[MATCHING_CONTOUR_1], &THEAPP.m_pModelDataManager->ContoursOri[MATCHING_CONTOUR_1], 1, 1);
	else
	{
		copy_obj(THEAPP.m_pInspectAdminViewDlg->ContoursAffinTrans[MATCHING_CONTOUR_3], &THEAPP.m_pModelDataManager->ContoursOri[MATCHING_CONTOUR_3], 1, 1);
		copy_obj(THEAPP.m_pInspectAdminViewDlg->ContoursAffinTrans[MATCHING_CONTOUR_4], &THEAPP.m_pModelDataManager->ContoursOri[MATCHING_CONTOUR_4], 1, 1);
	}
}


BOOL CMatchingDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_bnCreateROI.Init_Ctrl(_T("Arial"), 9, FALSE, BLACK, ROYAL_BLUE, 0, 0);
	m_bnDeleteROI.Init_Ctrl(_T("Arial"), 9, FALSE, BLACK, ROYAL_BLUE, 0, 0);
	m_bnCreatePolygonROI.Init_Ctrl(_T("Arial"), 9, FALSE, BLACK, ROYAL_BLUE, 0, 0);
	m_bnCreatePolygonEditROI.Init_Ctrl(_T("Arial"), 9, FALSE, BLACK, ROYAL_BLUE, 0, 0);
	m_bnResetPolygonROI.Init_Ctrl(_T("Arial"), 9, FALSE, BLACK, ROYAL_BLUE, 0, 0);

	m_cbContourType.SetCurSel(0);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

//0523
void CMatchingDlg::OnBnClickedRadioBlackMatchingImageBsurface()
{
	if(THEAPP.m_pModelDataManager->m_sModelName == ".") return;

	CString strModelFolder;
	if(!THEAPP.Struct_PreferenceStruct.m_bUseAbsolutePathModel) //Ver2629
		strModelFolder = THEAPP.GetWorkingDirectory()+"\\Model\\"+THEAPP.m_pModelDataManager->m_sModelName;
	else
		strModelFolder = THEAPP.GetWorkingEvmsDirectory()+"\\Model\\"+THEAPP.m_pModelDataManager->m_sModelName;

	CIniFileCS INI(strModelFolder+"\\SW\\ModelBase.ini");
	CString strSection = "Model Base";

	THEAPP.m_pModelDataManager->m_iBlackMatchingImageType = BLACK_MATCHING_IMAGE_TYPE_BSURFACE;
	INI.Set_Integer(strSection, "BlackBarrelMatchingImage", THEAPP.m_pModelDataManager->m_iBlackMatchingImageType);

	if (THEAPP.m_pGFunction->ValidHImage(THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[TRIGGER_SURFACE]))
		copy_image (THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[TRIGGER_SURFACE], &THEAPP.m_pInspectAdminViewDlg->m_pHImage);
	else
		gen_empty_obj(&THEAPP.m_pInspectAdminViewDlg->m_pHImage);

	THEAPP.m_pInspectAdminViewDlg->InvalidateRect(false);
}

void CMatchingDlg::OnBnClickedRadioBlackMatchingImageBedge()
{
	if(THEAPP.m_pModelDataManager->m_sModelName == ".") return;

	CString strModelFolder;
	if(!THEAPP.Struct_PreferenceStruct.m_bUseAbsolutePathModel) //Ver2629
		strModelFolder = THEAPP.GetWorkingDirectory()+"\\Model\\"+THEAPP.m_pModelDataManager->m_sModelName;
	else
		strModelFolder = THEAPP.GetWorkingEvmsDirectory()+"\\Model\\"+THEAPP.m_pModelDataManager->m_sModelName;

	CIniFileCS INI(strModelFolder+"\\SW\\ModelBase.ini");
	CString strSection = "Model Base";

	THEAPP.m_pModelDataManager->m_iBlackMatchingImageType = BLACK_MATCHING_IMAGE_TYPE_BEDGE;
	INI.Set_Integer(strSection, "BlackBarrelMatchingImage", THEAPP.m_pModelDataManager->m_iBlackMatchingImageType);

	if (THEAPP.m_pGFunction->ValidHImage(THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[TRIGGER_EDGE]))
		copy_image (THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[TRIGGER_EDGE], &THEAPP.m_pInspectAdminViewDlg->m_pHImage);
	else
		gen_empty_obj(&THEAPP.m_pInspectAdminViewDlg->m_pHImage);

	THEAPP.m_pInspectAdminViewDlg->InvalidateRect(false);
}

void CMatchingDlg::OnBnClickedRadioBlackMatchingImageLens1()
{
	if(THEAPP.m_pModelDataManager->m_sModelName == ".") return;

	CString strModelFolder;
	if(!THEAPP.Struct_PreferenceStruct.m_bUseAbsolutePathModel) //Ver2629
		strModelFolder = THEAPP.GetWorkingDirectory()+"\\Model\\"+THEAPP.m_pModelDataManager->m_sModelName;
	else
		strModelFolder = THEAPP.GetWorkingEvmsDirectory()+"\\Model\\"+THEAPP.m_pModelDataManager->m_sModelName;

	CIniFileCS INI(strModelFolder+"\\SW\\ModelBase.ini");
	CString strSection = "Model Base";

	THEAPP.m_pModelDataManager->m_iBlackMatchingImageType = BLACK_MATCHING_IMAGE_TYPE_LENS1;
	INI.Set_Integer(strSection, "BlackBarrelMatchingImage", THEAPP.m_pModelDataManager->m_iBlackMatchingImageType);

	if (THEAPP.m_pGFunction->ValidHImage(THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[TRIGGER_LENS1]))
		copy_image (THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[TRIGGER_LENS1], &THEAPP.m_pInspectAdminViewDlg->m_pHImage);
	else
		gen_empty_obj(&THEAPP.m_pInspectAdminViewDlg->m_pHImage);

	THEAPP.m_pInspectAdminViewDlg->InvalidateRect(false);
}

void CMatchingDlg::OnBnClickedRadioBlackMatchingImageLens2()
{
	if(THEAPP.m_pModelDataManager->m_sModelName == ".") return;

	CString strModelFolder;
	if(!THEAPP.Struct_PreferenceStruct.m_bUseAbsolutePathModel) //Ver2629
		strModelFolder = THEAPP.GetWorkingDirectory()+"\\Model\\"+THEAPP.m_pModelDataManager->m_sModelName;
	else
		strModelFolder = THEAPP.GetWorkingEvmsDirectory()+"\\Model\\"+THEAPP.m_pModelDataManager->m_sModelName;

	CIniFileCS INI(strModelFolder+"\\SW\\ModelBase.ini");
	CString strSection = "Model Base";

	THEAPP.m_pModelDataManager->m_iBlackMatchingImageType = BLACK_MATCHING_IMAGE_TYPE_LENS2;
	INI.Set_Integer(strSection, "BlackBarrelMatchingImage", THEAPP.m_pModelDataManager->m_iBlackMatchingImageType);

	if (THEAPP.m_pGFunction->ValidHImage(THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[TRIGGER_LENS2]))
		copy_image (THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[TRIGGER_LENS2], &THEAPP.m_pInspectAdminViewDlg->m_pHImage);
	else
		gen_empty_obj(&THEAPP.m_pInspectAdminViewDlg->m_pHImage);

	THEAPP.m_pInspectAdminViewDlg->InvalidateRect(false);
}


void CMatchingDlg::OnBnClickedCheckUseMatchingAngle()
{
	if(THEAPP.m_pModelDataManager->m_sModelName == ".") return;

	CString strModelFolder;
	if(!THEAPP.Struct_PreferenceStruct.m_bUseAbsolutePathModel) //Ver2629
		strModelFolder = THEAPP.GetWorkingDirectory()+"\\Model\\"+THEAPP.m_pModelDataManager->m_sModelName;
	else
		strModelFolder = THEAPP.GetWorkingEvmsDirectory()+"\\Model\\"+THEAPP.m_pModelDataManager->m_sModelName;

	CIniFileCS INI(strModelFolder+"\\SW\\ModelBase.ini");
	CString strSection = "Model Base";

	THEAPP.m_pModelDataManager->m_bUseMatchingAngle = !m_bCheckUseMatchingAngle;
	INI.Set_Bool(strSection, "UseContour1MatchingAngle", THEAPP.m_pModelDataManager->m_bUseMatchingAngle);
}

void CMatchingDlg::OnBnClickedCheckUseMatchingScale()
{
	if(THEAPP.m_pModelDataManager->m_sModelName == ".") return;

	CString strModelFolder;
	if(!THEAPP.Struct_PreferenceStruct.m_bUseAbsolutePathModel) //Ver2629
		strModelFolder = THEAPP.GetWorkingDirectory()+"\\Model\\"+THEAPP.m_pModelDataManager->m_sModelName;
	else
		strModelFolder = THEAPP.GetWorkingEvmsDirectory()+"\\Model\\"+THEAPP.m_pModelDataManager->m_sModelName;

	CIniFileCS INI(strModelFolder+"\\SW\\ModelBase.ini");
	CString strSection = "Model Base";

	THEAPP.m_pModelDataManager->m_bUseMatchingScale = !m_bCheckUseMatchingScale;
	INI.Set_Bool(strSection, "UseScaleMatching", THEAPP.m_pModelDataManager->m_bUseMatchingScale);

}

#include "MatchingParamAdvancedDlg.h"

void CMatchingDlg::OnBnClickedButtonSetContourContrast()
{
	if(THEAPP.m_pModelDataManager->m_sModelName == ".") 
		return;

	CMatchingParamAdvancedDlg dlg;
	dlg.DoModal();
}

void CMatchingDlg::OnBnClickedBnCreatePolygonDontcareEditRoi()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	if (THEAPP.m_pInspectAdminViewDlg->m_iPolygonMode == 2)
	{
		THEAPP.m_pInspectAdminViewDlg->DiffPolygonROI();

		THEAPP.m_pInspectAdminViewDlg->m_iPolygonMode = 0;
		THEAPP.m_pInspectAdminViewDlg->m_iPolygonCnt = 0;
		THEAPP.m_pInspectAdminViewDlg->mHUnionPolygonRgn.Reset();

		THEAPP.m_pInspectAdminViewDlg->InvalidateRect(false);
	}
}


void CMatchingDlg::OnBnClickedBnCreatePolygonDontcareRoi()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	THEAPP.m_pInspectAdminViewDlg->SetPolygonXLDEditMode(m_cbContourType.GetCurSel());
}


void CMatchingDlg::OnBnClickedBnResetPolygonDontcareRoi()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	THEAPP.m_pInspectAdminViewDlg->m_iPolygonMode = 0;
	THEAPP.m_pInspectAdminViewDlg->m_iPolygonCnt = 0;
	THEAPP.m_pInspectAdminViewDlg->mHUnionPolygonRgn.Reset();

	THEAPP.m_pInspectAdminViewDlg->InvalidateRect(false);
}
