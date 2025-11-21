// MatchingLocalDlg.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "uScan.h"
#include "MatchingLocalDlg.h"
#include "afxdialogex.h"


// CMatchingLocalDlg 대화 상자입니다.

IMPLEMENT_DYNAMIC(CMatchingLocalDlg, CDialog)

CMatchingLocalDlg::CMatchingLocalDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CMatchingLocalDlg::IDD, pParent)
{
	m_bCheckUseMatchingShape = TRUE;
	m_bCheckUseMatchingNCC = FALSE;

	m_iEditMatchingPyramidLevel = 0;
	m_iEditMatchingAngleRange = 5.0;
	m_dEditMatchingScaleMin = 0.98;
	m_dEditMatchingScaleMax = 1.02;
	m_iEditMatchingTeachingContrast = 40;
	m_iEditMatchingTeachingContrastLow = 40;
	m_iEditMatchingTeachingContrastMinSize = 1;
	m_dEditMatchingScore = 0.3;
	m_iEditMatchingInspectionMinContrast = 15;
	m_iEditMatchingSearchMarginX = 300;
	m_iEditMatchingSearchMarginY = 300;

	for (int i=0; i<MAX_IMAGE_TAB; i++)
		m_bCheckAlignImage[i] = FALSE;

	m_iCurrentShapeModel = ALIGN_MATCHING_SHAPE;

	m_dEditDeltaX = 0.0;
	m_dEditDeltaY = 0.0;
	m_dEditRotationAngle = 0;

	m_bCheckUseMatchingAffineConstant = FALSE;
}

CMatchingLocalDlg::~CMatchingLocalDlg()
{
}

void CMatchingLocalDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_MATCHING_ANGLE_RANGE, m_iEditMatchingAngleRange);
	DDX_Text(pDX, IDC_EDIT_MATCHING_P_LEVEL, m_iEditMatchingPyramidLevel);
	DDX_Text(pDX, IDC_EDIT_MATCHING_TEACHING_CONTRAST, m_iEditMatchingTeachingContrast);
	DDX_Text(pDX, IDC_EDIT_MATCHING_TEACHING_CONTRAST_LOW, m_iEditMatchingTeachingContrastLow);
	DDX_Text(pDX, IDC_EDIT_MATCHING_TEACHING_CONTRAST_MIN_SIZE, m_iEditMatchingTeachingContrastMinSize);
	DDX_Text(pDX, IDC_EDIT_MATCHING_SCORE, m_dEditMatchingScore);
	DDX_Text(pDX, IDC_EDIT_MATCHING_INSPECTION_MIN_CONTRAST, m_iEditMatchingInspectionMinContrast);
	DDX_Text(pDX, IDC_EDIT_MATCHING_SEARCH_MARGIN_X, m_iEditMatchingSearchMarginX);
	DDX_Text(pDX, IDC_EDIT_MATCHING_SEARCH_MARGIN_Y, m_iEditMatchingSearchMarginY);
	DDX_Text(pDX, IDC_EDIT_MATCHING_SCALE_MIN, m_dEditMatchingScaleMin);
	DDX_Text(pDX, IDC_EDIT_MATCHING_SCALE_MAX2, m_dEditMatchingScaleMax);

	DDX_Check(pDX, IDC_CHECK_IMAGE_1, m_bCheckAlignImage[0]);
	DDX_Check(pDX, IDC_CHECK_IMAGE_2, m_bCheckAlignImage[1]);
	DDX_Check(pDX, IDC_CHECK_IMAGE_3, m_bCheckAlignImage[2]);
	DDX_Check(pDX, IDC_CHECK_IMAGE_4, m_bCheckAlignImage[3]);
	DDX_Check(pDX, IDC_CHECK_IMAGE_5, m_bCheckAlignImage[4]);
	DDX_Check(pDX, IDC_CHECK_IMAGE_6, m_bCheckAlignImage[5]);
	DDX_Check(pDX, IDC_CHECK_IMAGE_7, m_bCheckAlignImage[6]);

	DDX_Check(pDX, IDC_CHECK_USE_SHAPE_MATCHING, m_bCheckUseMatchingShape);
	DDX_Check(pDX, IDC_CHECK_USE_TEMPLATE_MATCHING, m_bCheckUseMatchingNCC);
	DDX_Text(pDX, IDC_EDIT_DELTA_X, m_dEditDeltaX);
	DDX_Text(pDX, IDC_EDIT_DELTA_Y, m_dEditDeltaY);
	DDX_Text(pDX, IDC_EDIT_ROTATION_ANGLE, m_dEditRotationAngle);

	DDX_Check(pDX, IDC_CHECK_USE_MATCHING_AFFINE_CONSTANT, m_bCheckUseMatchingAffineConstant);
}


BEGIN_MESSAGE_MAP(CMatchingLocalDlg, CDialog)
	ON_BN_CLICKED(IDC_BUTTON_CREATE_MATCHING_MODEL, &CMatchingLocalDlg::OnBnClickedButtonCreateMatchingModel)
	ON_BN_CLICKED(IDC_BUTTON_SHOW_CONTRAST, &CMatchingLocalDlg::OnBnClickedButtonShowContrast)
	ON_BN_CLICKED(IDC_BUTTON_CHANGE_SCORE, &CMatchingLocalDlg::OnBnClickedButtonChangeScore)
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_BUTTON_TEST_MATCHING_MODEL, &CMatchingLocalDlg::OnBnClickedButtonTestMatchingModel)
	ON_BN_CLICKED(IDC_CHECK_USE_SHAPE_MATCHING, &CMatchingLocalDlg::OnBnClickedCheckUseShapeMatching)
	ON_BN_CLICKED(IDC_CHECK_USE_TEMPLATE_MATCHING, &CMatchingLocalDlg::OnClickedCheckUseTemplateMatching)
	ON_BN_CLICKED(IDC_BUTTON_IMAGE_ROTATION, &CMatchingLocalDlg::OnBnClickedButtonImageRotation)
	ON_BN_CLICKED(IDC_BUTTON_AFFINE_IMAGE, &CMatchingLocalDlg::OnBnClickedButtonAffineImage)
END_MESSAGE_MAP()


// CMatchingLocalDlg 메시지 처리기입니다.

BOOL CMatchingLocalDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	if(pMsg->message==WM_KEYDOWN&&pMsg->wParam==VK_ESCAPE)
		return TRUE;
	if(pMsg->message==WM_KEYDOWN&&pMsg->wParam==VK_RETURN)
		return TRUE;
	return CDialog::PreTranslateMessage(pMsg);
}

BOOL CMatchingLocalDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	if (THEAPP.m_iMachineInspType == MACHINE_BRACKET)
	{
		GetDlgItem(IDC_CHECK_IMAGE_5)->EnableWindow(TRUE);
		GetDlgItem(IDC_CHECK_IMAGE_6)->EnableWindow(TRUE);
		GetDlgItem(IDC_CHECK_IMAGE_7)->EnableWindow(TRUE);
	}
	else if (THEAPP.m_iMachineInspType == MACHINE_SIDEFILL ||
		THEAPP.m_iMachineInspType == MACHINE_FIDUCIAL )

	{
		GetDlgItem(IDC_CHECK_IMAGE_5)->EnableWindow(TRUE);
		GetDlgItem(IDC_CHECK_IMAGE_6)->EnableWindow(TRUE);
		GetDlgItem(IDC_CHECK_IMAGE_7)->EnableWindow(FALSE);
	}
	else
	{
		GetDlgItem(IDC_CHECK_IMAGE_5)->EnableWindow(FALSE);
		GetDlgItem(IDC_CHECK_IMAGE_6)->EnableWindow(FALSE);
		GetDlgItem(IDC_CHECK_IMAGE_7)->EnableWindow(FALSE);
	}

	m_iCurImageIndex = THEAPP.m_pTabControlDlg->m_iCurrentTab - 1;

	m_bCheckAlignImage[m_iCurImageIndex] = TRUE;

	LoadViewParam();

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

void CMatchingLocalDlg::LoadViewParam()
{
	m_bCheckUseMatchingShape = FALSE;
	m_bCheckUseMatchingNCC = FALSE;

	int iTemp;
	iTemp = THEAPP.m_pModelDataManager->m_iLocalMatchingMethod[m_iCurImageIndex];

	if (iTemp==ALIGN_MATCHING_SHAPE)
		m_bCheckUseMatchingShape = TRUE;
	else if (iTemp==ALIGN_MATCHING_TEMPLATE)
		m_bCheckUseMatchingNCC = TRUE;

	m_iCurrentShapeModel = iTemp;

	m_iEditMatchingPyramidLevel = THEAPP.m_pModelDataManager->m_iLocalMatchingPyramidLevel[m_iCurImageIndex];
	m_iEditMatchingAngleRange = THEAPP.m_pModelDataManager->m_iLocalMatchingAngleRange[m_iCurImageIndex];
	m_dEditMatchingScaleMin = THEAPP.m_pModelDataManager->m_dLocalMatchingScaleMin[m_iCurImageIndex];
	m_dEditMatchingScaleMax = THEAPP.m_pModelDataManager->m_dLocalMatchingScaleMax[m_iCurImageIndex];
	m_iEditMatchingTeachingContrast = THEAPP.m_pModelDataManager->m_iLocalMatchingTeachingContrast[m_iCurImageIndex];
	m_iEditMatchingTeachingContrastLow = THEAPP.m_pModelDataManager->m_iLocalMatchingTeachingContrastLow[m_iCurImageIndex];
	m_iEditMatchingTeachingContrastMinSize = THEAPP.m_pModelDataManager->m_iLocalMatchingTeachingContrastMinSize[m_iCurImageIndex];
	m_dEditMatchingScore = THEAPP.m_pModelDataManager->m_dLocalMatchingScore[m_iCurImageIndex];
	m_iEditMatchingInspectionMinContrast = THEAPP.m_pModelDataManager->m_iLocalMatchingInspectionMinContrast[m_iCurImageIndex];
	m_iEditMatchingSearchMarginX = THEAPP.m_pModelDataManager->m_iLocalMatchingSearchMarginX[m_iCurImageIndex];
	m_iEditMatchingSearchMarginY = THEAPP.m_pModelDataManager->m_iLocalMatchingSearchMarginY[m_iCurImageIndex];
	m_bCheckUseMatchingAffineConstant = THEAPP.m_pModelDataManager->m_bUseMatchingAffineConstant[m_iCurImageIndex];

	for (int i = 0; i < MAX_IMAGE_TAB; i++)
	{
		if (i >= THEAPP.m_iMaxInspImageNo)
			break;

		m_bCheckAlignImage[i] = THEAPP.m_pModelDataManager->m_bLocalAlignImage[m_iCurImageIndex][i];
	}

	SetControlState(m_iCurrentShapeModel);

	UpdateData(FALSE);
}

void CMatchingLocalDlg::SetControlState(int iMatchingType)
{
	if (iMatchingType==ALIGN_MATCHING_SHAPE)
	{
		GetDlgItem(IDC_EDIT_MATCHING_SCALE_MIN)->EnableWindow(TRUE);
		GetDlgItem(IDC_EDIT_MATCHING_SCALE_MAX2)->EnableWindow(TRUE);

		GetDlgItem(IDC_EDIT_MATCHING_TEACHING_CONTRAST)->EnableWindow(TRUE);
		GetDlgItem(IDC_EDIT_MATCHING_TEACHING_CONTRAST_LOW)->EnableWindow(TRUE);
		GetDlgItem(IDC_EDIT_MATCHING_TEACHING_CONTRAST_MIN_SIZE)->EnableWindow(TRUE);

		GetDlgItem(IDC_BUTTON_SHOW_CONTRAST)->EnableWindow(TRUE);
		GetDlgItem(IDC_EDIT_MATCHING_INSPECTION_MIN_CONTRAST)->EnableWindow(TRUE);

		GetDlgItem(IDC_BUTTON_IMAGE_ROTATION)->EnableWindow(TRUE);
	}
	else if (iMatchingType==ALIGN_MATCHING_TEMPLATE)
	{
		GetDlgItem(IDC_EDIT_MATCHING_SCALE_MIN)->EnableWindow(FALSE);
		GetDlgItem(IDC_EDIT_MATCHING_SCALE_MAX2)->EnableWindow(FALSE);

		GetDlgItem(IDC_EDIT_MATCHING_TEACHING_CONTRAST)->EnableWindow(FALSE);
		GetDlgItem(IDC_EDIT_MATCHING_TEACHING_CONTRAST_LOW)->EnableWindow(FALSE);
		GetDlgItem(IDC_EDIT_MATCHING_TEACHING_CONTRAST_MIN_SIZE)->EnableWindow(FALSE);

		GetDlgItem(IDC_BUTTON_SHOW_CONTRAST)->EnableWindow(FALSE);
		GetDlgItem(IDC_EDIT_MATCHING_INSPECTION_MIN_CONTRAST)->EnableWindow(FALSE);

		GetDlgItem(IDC_BUTTON_IMAGE_ROTATION)->EnableWindow(FALSE);
	}
}

void CMatchingLocalDlg::OnBnClickedButtonCreateMatchingModel()
{
try 
{
	if (THEAPP.m_pModelDataManager->m_pInspectionArea==NULL)
		return;

	int iNoInspectROI = 0;
	iNoInspectROI = THEAPP.m_pModelDataManager->m_pInspectionArea->GetChildTRegionCount();
	if (iNoInspectROI<=0)
		return;

	if (THEAPP.m_pGFunction->ValidHImage(THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[m_iCurImageIndex])==FALSE)
		return;

	int nRes = 0;
	nRes = AfxMessageBox("현재 설정으로 매칭 모델을 새로이 생성하시겠습니까?", MB_YESNO|MB_SYSTEMMODAL);

	if ((nRes!=IDYES))
		return;

	UpdateData();

	// Create Matching Model

	GTRegion *pInspectROIRgn;
	Hobject HAlignROIRgn, HROIHRegion;

	gen_empty_obj(&HAlignROIRgn);

	for (int iIndex=0; iIndex<iNoInspectROI; iIndex++)
	{
		pInspectROIRgn = THEAPP.m_pModelDataManager->m_pInspectionArea->GetChildTRegion(iIndex);
		if (pInspectROIRgn == NULL)
			continue;

		// 0번 영상에 있는 Align ROI만 추가

		if (pInspectROIRgn->m_iTeachImageIndex!=(m_iCurImageIndex+1))
			continue;

		if (pInspectROIRgn->m_iInspectionType!=INSPECTION_TYPE_ALIGN)
			continue;

		HROIHRegion = pInspectROIRgn->GetROIHRegion(THEAPP.m_pCalDataService);

		if (THEAPP.m_pGFunction->ValidHRegion(HROIHRegion))
			concat_obj(HAlignROIRgn, HROIHRegion, &HAlignROIRgn);
		else
			continue;
	}
	
	if (THEAPP.m_pGFunction->ValidHRegion(HAlignROIRgn))
		union1(HAlignROIRgn, &HAlignROIRgn);
	else
	{
		AfxMessageBox("매칭 모델 실패: Align ROI가 존재하지 않습니다. Align ROI를 티칭해 주세요.", MB_SYSTEMMODAL);
		return;
	}

	if (THEAPP.m_pModelDataManager->m_lLAlignModelID[m_iCurImageIndex] >= 0) 
	{
		if (m_iCurrentShapeModel==ALIGN_MATCHING_SHAPE)
			clear_shape_model(THEAPP.m_pModelDataManager->m_lLAlignModelID[m_iCurImageIndex]);
		else if (m_iCurrentShapeModel==ALIGN_MATCHING_TEMPLATE)
			clear_ncc_model(THEAPP.m_pModelDataManager->m_lLAlignModelID[m_iCurImageIndex]);

		THEAPP.m_pModelDataManager->m_lLAlignModelID[m_iCurImageIndex] = -1;
	}

	// Model Ref: G-Center of Domain
	double dTeachAlignRefX, dTeachAlignRefY;
	Hlong lArea;
	area_center(HAlignROIRgn, &lArea, &dTeachAlignRefY, &dTeachAlignRefX);

	if (lArea<500)
	{
		AfxMessageBox("매칭 모델 실패: Align ROI가 너무 작습니다. Align ROI를 추가해 주세요.", MB_SYSTEMMODAL);
		return;
	}

	Hobject HImageReduced;
	reduce_domain(THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[m_iCurImageIndex], HAlignROIRgn, &HImageReduced);

	HTuple dAngleRangeRad;
	tuple_rad(THEAPP.m_pModelDataManager->m_iLocalMatchingAngleRange[m_iCurImageIndex], &dAngleRangeRad);

	if (m_bCheckUseMatchingNCC)
	{
		if (m_iEditMatchingPyramidLevel==0)
			create_ncc_model(HImageReduced, "auto", -dAngleRangeRad, dAngleRangeRad*2.0, "auto", "use_polarity", &(THEAPP.m_pModelDataManager->m_lLAlignModelID[m_iCurImageIndex]));
		else
			create_ncc_model(HImageReduced, m_iEditMatchingPyramidLevel, -dAngleRangeRad, dAngleRangeRad*2.0, "auto", "use_polarity", &(THEAPP.m_pModelDataManager->m_lLAlignModelID[m_iCurImageIndex]));
	}
	else if (m_bCheckUseMatchingShape)
	{
		HTuple HContrast;
		tuple_gen_const(0, 0, &HContrast);
		tuple_concat(HContrast, m_iEditMatchingTeachingContrastLow, &HContrast);
		tuple_concat(HContrast, m_iEditMatchingTeachingContrast, &HContrast);
		tuple_concat(HContrast, m_iEditMatchingTeachingContrastMinSize, &HContrast);

		if (m_iEditMatchingPyramidLevel==0)
			create_scaled_shape_model(HImageReduced, "auto", -dAngleRangeRad, dAngleRangeRad*2.0, "auto", m_dEditMatchingScaleMin, m_dEditMatchingScaleMax, "auto", "auto", "use_polarity", HContrast, m_iEditMatchingInspectionMinContrast, &(THEAPP.m_pModelDataManager->m_lLAlignModelID[m_iCurImageIndex]));
		else
			create_scaled_shape_model(HImageReduced, m_iEditMatchingPyramidLevel, -dAngleRangeRad, dAngleRangeRad*2.0, "auto", m_dEditMatchingScaleMin, m_dEditMatchingScaleMax, "auto", "auto", "use_polarity", HContrast, m_iEditMatchingInspectionMinContrast, &(THEAPP.m_pModelDataManager->m_lLAlignModelID[m_iCurImageIndex]));
	}

	Hlong lNoFoundNumber;
	HTuple Row, Column, Angle, Scale, Score;
	HTuple HomMat2D;

	if (THEAPP.m_pModelDataManager->m_lLAlignModelID[m_iCurImageIndex] >= 0)
	{
		Row = HTuple(0.0);
		Column = HTuple(0.0);
		Angle = HTuple(0.0);
		Scale = HTuple(0.0);
		Score = HTuple(0.0);

		if (m_bCheckUseMatchingNCC)
		{
			find_ncc_model(THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[m_iCurImageIndex],
				THEAPP.m_pModelDataManager->m_lLAlignModelID[m_iCurImageIndex],	// Handle of the model
				-dAngleRangeRad,							// Smallest rotation of the model
				dAngleRangeRad*2.0,							// Extent of the rotation angles.
				m_dEditMatchingScore,						// Minumum score of the instances of the model to be found
				1,											// Number of instances of the model to be found
				0.5,										// Maximum overlap of the instances of the model to be found
				"true",										// Subpixel accuracy
				0,											// Number of pyramid levels used in the matching
				&Row,										// Row coordinate of the found instances of the model.
				&Column,									// Column coordinate of the found instances of the model.
				&Angle,										// Rotation angle of the found instances of the model. (radian)
				&Score);									// Score of the found instances of the model.
		}
		else if (m_bCheckUseMatchingShape)
		{
			find_scaled_shape_model(THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[m_iCurImageIndex],
				THEAPP.m_pModelDataManager->m_lLAlignModelID[m_iCurImageIndex],	// Handle of the model
				-dAngleRangeRad,							// Smallest rotation of the model
				dAngleRangeRad*2.0,							// Extent of the rotation angles.
				m_dEditMatchingScaleMin,
				m_dEditMatchingScaleMax,
				m_dEditMatchingScore,						// Minumum score of the instances of the model to be found
				1,											// Number of instances of the model to be found
				0.5,										// Maximum overlap of the instances of the model to be found
				"interpolation",							// Subpixel accuracy if not equal to 'none'. 'none', 'interpolation', 'least_squares', 'least_squares_high', 'least_squares_very_high'
				0,											// Number of pyramid levels used in the matching
				0.9,										// Greediness'' of the search heuristic (0: safe but slow; 1: fast but matches may be missed).
				&Row,										// Row coordinate of the found instances of the model.
				&Column,									// Column coordinate of the found instances of the model.
				&Angle,										// Rotation angle of the found instances of the model. (radian)
				&Scale,
				&Score);									// Score of the found instances of the model.
		}

		tuple_length(Score, &lNoFoundNumber);

		if (lNoFoundNumber>0)
		{
			if (m_bCheckUseMatchingNCC)
				m_iCurrentShapeModel = ALIGN_MATCHING_TEMPLATE;
			else if (m_bCheckUseMatchingShape)
				m_iCurrentShapeModel = ALIGN_MATCHING_SHAPE;

			// Save Parameter

			CString strModelFolder;
			if(!THEAPP.Struct_PreferenceStruct.m_bUseAbsolutePathModel) //Ver2629
				strModelFolder = THEAPP.GetWorkingDirectory()+"\\Model\\"+THEAPP.m_pModelDataManager->m_sModelName;
			else
				strModelFolder = THEAPP.GetWorkingEvmsDirectory()+"\\Model\\"+THEAPP.m_pModelDataManager->m_sModelName;

			CString sModelBasePath;
			sModelBasePath = strModelFolder+"\\SW\\ModelBase.ini";

			CIniFileCS INI(sModelBasePath);
			CString strSection = "Local Align";

			CString sTemp;

			THEAPP.m_pModelDataManager->m_iLocalMatchingMethod[m_iCurImageIndex] = m_iCurrentShapeModel;
			sTemp.Format("m_iLocalMatchingMethod_%d", m_iCurImageIndex+1);
			INI.Set_Integer(strSection, sTemp, m_iCurrentShapeModel);

			//////////////////////////////////////////////////////////////////////////
			/// For NCC
			Hobject HModelRectRgn;
			Hlong lRectLTY, lRectLTX, lRectRBY, lRectRBX;

			shape_trans(HAlignROIRgn, &HModelRectRgn, "rectangle1");
			smallest_rectangle1(HModelRectRgn, &lRectLTY, &lRectLTX, &lRectRBY, &lRectRBX);

			THEAPP.m_pModelDataManager->m_iLocalMatchingTeachingRectLTX[m_iCurImageIndex] = lRectLTX;
			sTemp.Format("m_iLocalMatchingTeachingRectLTX_%d", m_iCurImageIndex+1);
			INI.Set_Integer(strSection, sTemp, lRectLTX);
			THEAPP.m_pModelDataManager->m_iLocalMatchingTeachingRectLTY[m_iCurImageIndex] = lRectLTY;
			sTemp.Format("m_iLocalMatchingTeachingRectLTY_%d", m_iCurImageIndex+1);
			INI.Set_Integer(strSection, sTemp, lRectLTY);
			THEAPP.m_pModelDataManager->m_iLocalMatchingTeachingRectRBX[m_iCurImageIndex] = lRectRBX;
			sTemp.Format("m_iLocalMatchingTeachingRectRBX_%d", m_iCurImageIndex+1);
			INI.Set_Integer(strSection, sTemp, lRectRBX);
			THEAPP.m_pModelDataManager->m_iLocalMatchingTeachingRectRBY[m_iCurImageIndex] = lRectRBY;
			sTemp.Format("m_iLocalMatchingTeachingRectRBY_%d", m_iCurImageIndex+1);
			INI.Set_Integer(strSection, sTemp, lRectRBY);
			//////////////////////////////////////////////////////////////////////////

			THEAPP.m_pModelDataManager->m_iLocalMatchingPyramidLevel[m_iCurImageIndex] = m_iEditMatchingPyramidLevel;
			sTemp.Format("m_iLocalMatchingPyramidLevel_%d", m_iCurImageIndex+1);
			INI.Set_Integer(strSection, sTemp, m_iEditMatchingPyramidLevel);
			THEAPP.m_pModelDataManager->m_iLocalMatchingAngleRange[m_iCurImageIndex] = m_iEditMatchingAngleRange;
			sTemp.Format("m_iLocalMatchingAngleRange_%d", m_iCurImageIndex+1);
			INI.Set_Double(strSection, sTemp, m_iEditMatchingAngleRange);
			THEAPP.m_pModelDataManager->m_dLocalMatchingScaleMin[m_iCurImageIndex] = m_dEditMatchingScaleMin;
			sTemp.Format("m_dLocalMatchingScaleMin_%d", m_iCurImageIndex+1);
			INI.Set_Double(strSection, sTemp, m_dEditMatchingScaleMin);
			THEAPP.m_pModelDataManager->m_dLocalMatchingScaleMax[m_iCurImageIndex] = m_dEditMatchingScaleMax;
			sTemp.Format("m_dLocalMatchingScaleMax_%d", m_iCurImageIndex+1);
			INI.Set_Double(strSection, sTemp, m_dEditMatchingScaleMax);
			
			THEAPP.m_pModelDataManager->m_iLocalMatchingTeachingContrast[m_iCurImageIndex] = m_iEditMatchingTeachingContrast;
			sTemp.Format("m_iLocalMatchingTeachingContrast_%d", m_iCurImageIndex+1);
			INI.Set_Integer(strSection, sTemp, m_iEditMatchingTeachingContrast);
			THEAPP.m_pModelDataManager->m_iLocalMatchingTeachingContrastLow[m_iCurImageIndex] = m_iEditMatchingTeachingContrastLow;
			sTemp.Format("m_iLocalMatchingTeachingContrastLow_%d", m_iCurImageIndex+1);
			INI.Set_Integer(strSection, sTemp, m_iEditMatchingTeachingContrastLow);
			THEAPP.m_pModelDataManager->m_iLocalMatchingTeachingContrastMinSize[m_iCurImageIndex] = m_iEditMatchingTeachingContrastMinSize;
			sTemp.Format("m_iLocalMatchingTeachingContrastMinSize_%d", m_iCurImageIndex+1);
			INI.Set_Integer(strSection, sTemp, m_iEditMatchingTeachingContrastMinSize);

			THEAPP.m_pModelDataManager->m_dLocalMatchingScore[m_iCurImageIndex] = m_dEditMatchingScore;
			sTemp.Format("m_dLocalMatchingScore_%d", m_iCurImageIndex+1);
			INI.Set_Double(strSection, sTemp, m_dEditMatchingScore);
			THEAPP.m_pModelDataManager->m_iLocalMatchingInspectionMinContrast[m_iCurImageIndex] = m_iEditMatchingInspectionMinContrast;
			sTemp.Format("m_iLocalMatchingInspectionMinContrast_%d", m_iCurImageIndex+1);
			INI.Set_Integer(strSection, sTemp, m_iEditMatchingInspectionMinContrast);

			THEAPP.m_pModelDataManager->m_iLocalMatchingSearchMarginX[m_iCurImageIndex] = m_iEditMatchingSearchMarginX;
			sTemp.Format("m_iLocalMatchingSearchMarginX_%d", m_iCurImageIndex+1);
			INI.Set_Integer(strSection, sTemp, m_iEditMatchingSearchMarginX);
			THEAPP.m_pModelDataManager->m_iLocalMatchingSearchMarginY[m_iCurImageIndex] = m_iEditMatchingSearchMarginY;
			sTemp.Format("m_iLocalMatchingSearchMarginY_%d", m_iCurImageIndex+1);
			INI.Set_Integer(strSection, sTemp, m_iEditMatchingSearchMarginY);

			THEAPP.m_pModelDataManager->m_dLocalTeachAlignRefX[m_iCurImageIndex] = dTeachAlignRefX;
			sTemp.Format("m_dLocalTeachAlignRefX_%d", m_iCurImageIndex+1);
			INI.Set_Double(strSection, sTemp, dTeachAlignRefX);
			THEAPP.m_pModelDataManager->m_dLocalTeachAlignRefY[m_iCurImageIndex] = dTeachAlignRefY;
			sTemp.Format("m_dLocalTeachAlignRefY_%d", m_iCurImageIndex+1);
			INI.Set_Double(strSection, sTemp, dTeachAlignRefY);

			THEAPP.m_pModelDataManager->m_bUseMatchingAffineConstant[m_iCurImageIndex] = m_bCheckUseMatchingAffineConstant;
			sTemp.Format("m_bUseMatchingAffineConstant_%d", m_iCurImageIndex+1);
			INI.Set_Bool(strSection, sTemp, m_bCheckUseMatchingAffineConstant);

			for (int j=0; j<MAX_IMAGE_TAB; j++)
			{
				if (j >= THEAPP.m_iMaxInspImageNo)
					break;

				THEAPP.m_pModelDataManager->m_bLocalAlignImage[m_iCurImageIndex][j] = m_bCheckAlignImage[j];

				sTemp.Format("m_bLocalAlignImage_%d_%d", m_iCurImageIndex+1, j+1);
				INI.Set_Bool(strSection, sTemp, m_bCheckAlignImage[j]);
			}

			//////////////////////////////////////////////////////////////////////////
			// Save Global Align Model
			CString szTemplateFolder, sTemplateFolder;

			szTemplateFolder = strModelFolder+"\\SW\\MatchingModel";

			sTemplateFolder.Format("%s\\LocalAlignModel_%d", szTemplateFolder, m_iCurImageIndex+1);

			if (THEAPP.m_pModelDataManager->m_lLAlignModelID[m_iCurImageIndex]>=0)
			{
				if (m_bCheckUseMatchingNCC)
					write_ncc_model(THEAPP.m_pModelDataManager->m_lLAlignModelID[m_iCurImageIndex], HTuple(sTemplateFolder));
				else if (m_bCheckUseMatchingShape)
					write_shape_model(THEAPP.m_pModelDataManager->m_lLAlignModelID[m_iCurImageIndex], HTuple(sTemplateFolder));
			}

			AfxMessageBox("매칭 모델 생성 성공.", MB_SYSTEMMODAL);
		}
		else
		{
			AfxMessageBox("매칭 모델 생성 실패: 인식 실패", MB_SYSTEMMODAL);
		}
	}
	else
		AfxMessageBox("매칭 모델 생성 실패: 모델 생성 실패", MB_SYSTEMMODAL);
}
catch (HException &except)
{
	CString str; str.Format("Halcon Exception [CMatchingLocalDlg::OnBnClickedButtonCreateMatchingModel] : %s", except.message); THEAPP.SaveLog(str);
	
	AfxMessageBox("매칭 모델 생성 실패: 모델 생성 실패", MB_SYSTEMMODAL);
}
}

void CMatchingLocalDlg::OnBnClickedButtonShowContrast()
{
	try
	{
		BOOL bDebugSave = FALSE;

		if (THEAPP.m_pModelDataManager->m_pInspectionArea==NULL)
			return;

		int iNoInspectROI = 0;
		iNoInspectROI = THEAPP.m_pModelDataManager->m_pInspectionArea->GetChildTRegionCount();
		if (iNoInspectROI<=0)
			return;

		if (THEAPP.m_pGFunction->ValidHImage(THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[m_iCurImageIndex])==FALSE)
			return;
		
		UpdateData();

		GTRegion *pInspectROIRgn;
		Hobject HAlignROIRgn, HROIHRegion;

		gen_empty_obj(&HAlignROIRgn);

		for (int iIndex=0; iIndex<iNoInspectROI; iIndex++)
		{
			pInspectROIRgn = THEAPP.m_pModelDataManager->m_pInspectionArea->GetChildTRegion(iIndex);
			if (pInspectROIRgn == NULL)
				continue;

			if (pInspectROIRgn->m_iTeachImageIndex!=(m_iCurImageIndex+1))
				continue;

			if (pInspectROIRgn->m_iInspectionType!=INSPECTION_TYPE_ALIGN)
				continue;

			HROIHRegion = pInspectROIRgn->GetROIHRegion(THEAPP.m_pCalDataService);

			if (THEAPP.m_pGFunction->ValidHRegion(HROIHRegion))
				concat_obj(HAlignROIRgn, HROIHRegion, &HAlignROIRgn);
			else
				continue;
		}

		if (THEAPP.m_pGFunction->ValidHRegion(HAlignROIRgn))
			union1(HAlignROIRgn, &HAlignROIRgn);
		else
		{
			AfxMessageBox("매칭 모델 실패: Align ROI가 존재하지 않습니다. Align ROI를 티칭해 주세요.", MB_SYSTEMMODAL);
			return;
		}

		BOOL bViewUpdate = FALSE;

		if (m_iEditMatchingTeachingContrast<5)
		{
			m_iEditMatchingTeachingContrast = 5;
			bViewUpdate = TRUE;
		}

		if (m_iEditMatchingTeachingContrastLow>m_iEditMatchingTeachingContrast)
		{
			m_iEditMatchingTeachingContrastLow = m_iEditMatchingTeachingContrast;
			bViewUpdate = TRUE;
		}

		if (m_iEditMatchingTeachingContrastMinSize<0)
		{
			m_iEditMatchingTeachingContrastMinSize = 1;
			bViewUpdate = TRUE;
		}

		if (bViewUpdate)
			UpdateData(FALSE);

		if (bDebugSave)
		{
			write_image(THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[m_iCurImageIndex], "bmp", 0, "c:\\DualTest\\InspectImage");
			write_region(HAlignROIRgn, "c:\\DualTest\\InspectAreaRgn");
		}

		Hobject HFeatureImage, HImageReduced;

		reduce_domain(THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[m_iCurImageIndex], HAlignROIRgn, &HImageReduced);

		gen_empty_obj(&(THEAPP.m_pInspectAdminViewDlg->m_HShapeModelContrastRgn));

		HTuple HContrast;
		tuple_gen_const(0, 0, &HContrast);
		tuple_concat(HContrast, m_iEditMatchingTeachingContrastLow, &HContrast);
		tuple_concat(HContrast, m_iEditMatchingTeachingContrast, &HContrast);
		tuple_concat(HContrast, m_iEditMatchingTeachingContrastMinSize, &HContrast);

		inspect_shape_model(HImageReduced, &HFeatureImage, &(THEAPP.m_pInspectAdminViewDlg->m_HShapeModelContrastRgn), 1, HTuple(m_iEditMatchingTeachingContrast));

		THEAPP.m_pInspectAdminViewDlg->UpdateView();
	}
	catch(HException &except)
	{
		CString str; str.Format("Halcon Exception [CMatchingLocalDlg::OnBnClickedButtonShowContrast] : %s", except.message); THEAPP.SaveLog(str);
	}
}

void CMatchingLocalDlg::OnBnClickedButtonChangeScore()
{
try
{
	UpdateData();
	
	// Save Parameter

	CString strModelFolder;
	if(!THEAPP.Struct_PreferenceStruct.m_bUseAbsolutePathModel) //Ver2629
		strModelFolder = THEAPP.GetWorkingDirectory()+"\\Model\\"+THEAPP.m_pModelDataManager->m_sModelName;
	else
		strModelFolder = THEAPP.GetWorkingEvmsDirectory()+"\\Model\\"+THEAPP.m_pModelDataManager->m_sModelName;

	CString sModelBasePath;
	sModelBasePath = strModelFolder+"\\SW\\ModelBase.ini";

	CIniFileCS INI(sModelBasePath);
	CString strSection = "Local Align";

	CString sTemp;

	THEAPP.m_pModelDataManager->m_dLocalMatchingScore[m_iCurImageIndex] = m_dEditMatchingScore;
	sTemp.Format("m_dLocalMatchingScore_%d", m_iCurImageIndex+1);
	INI.Set_Double(strSection, sTemp, m_dEditMatchingScore);

	THEAPP.m_pModelDataManager->m_iLocalMatchingSearchMarginX[m_iCurImageIndex] = m_iEditMatchingSearchMarginX;
	sTemp.Format("m_iLocalMatchingSearchMarginX_%d", m_iCurImageIndex+1);
	INI.Set_Integer(strSection, sTemp, m_iEditMatchingSearchMarginX);
	THEAPP.m_pModelDataManager->m_iLocalMatchingSearchMarginY[m_iCurImageIndex] = m_iEditMatchingSearchMarginY;
	sTemp.Format("m_iLocalMatchingSearchMarginY_%d", m_iCurImageIndex+1);
	INI.Set_Integer(strSection, sTemp, m_iEditMatchingSearchMarginY);

	THEAPP.m_pModelDataManager->m_bUseMatchingAffineConstant[m_iCurImageIndex] = m_bCheckUseMatchingAffineConstant;
	sTemp.Format("m_bUseMatchingAffineConstant_%d", m_iCurImageIndex+1);
	INI.Set_Bool(strSection, sTemp, m_bCheckUseMatchingAffineConstant);

	for (int j=0; j<MAX_IMAGE_TAB; j++)
	{
		if (j >= THEAPP.m_iMaxInspImageNo)
			break;

		THEAPP.m_pModelDataManager->m_bLocalAlignImage[m_iCurImageIndex][j] = m_bCheckAlignImage[j];

		sTemp.Format("m_bLocalAlignImage_%d_%d", m_iCurImageIndex+1, j+1);
		INI.Set_Bool(strSection, sTemp, m_bCheckAlignImage[j]);
	}

	if (m_bCheckUseMatchingShape)	// for shape matching
	{
		if (THEAPP.m_pModelDataManager->m_lLAlignModelID[m_iCurImageIndex]>=0)
		{
			if (m_iCurrentShapeModel==ALIGN_MATCHING_SHAPE)
			{
				THEAPP.m_pModelDataManager->m_iLocalMatchingInspectionMinContrast[m_iCurImageIndex] = m_iEditMatchingInspectionMinContrast;
				sTemp.Format("m_iLocalMatchingInspectionMinContrast_%d", m_iCurImageIndex+1);
				INI.Set_Integer(strSection, sTemp, m_iEditMatchingInspectionMinContrast);

				set_shape_model_param(THEAPP.m_pModelDataManager->m_lLAlignModelID[m_iCurImageIndex], "min_contrast", m_iEditMatchingInspectionMinContrast);

				//////////////////////////////////////////////////////////////////////////
				// Save Global Align Model
				CString szTemplateFolder, sTemplateFolder;

				szTemplateFolder = strModelFolder+"\\SW\\MatchingModel";
				sTemplateFolder.Format("%s\\LocalAlignModel_%d", szTemplateFolder, m_iCurImageIndex+1);
				write_shape_model(THEAPP.m_pModelDataManager->m_lLAlignModelID[m_iCurImageIndex], HTuple(sTemplateFolder));
			}
			else
			{
				AfxMessageBox("현재 매칭 모델과 선택된 매칭 모델이 일치하지 않습니다. 매칭 모델 생성 후 재시도하십시요.", MB_SYSTEMMODAL|MB_ICONINFORMATION);
				return;
			}
		}
	}
}
catch(HException &except)
{
	CString str; str.Format("Halcon Exception [CMatchingLocalDlg::OnBnClickedButtonChangeScore] : %s", except.message); THEAPP.SaveLog(str);
}
}


void CMatchingLocalDlg::OnClose()
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.

	gen_empty_obj(&(THEAPP.m_pInspectAdminViewDlg->m_HShapeModelContrastRgn));

	CDialog::OnClose();
}


void CMatchingLocalDlg::OnBnClickedButtonImageRotation()
{
	int nRes = 0;
	nRes = AfxMessageBox("티칭영상이 변경되오니 주의하십시요. 현재 설정으로 영상을 회전하시겠습니까?", MB_YESNO|MB_SYSTEMMODAL);

	if ((nRes!=IDYES))
		return;

	if (THEAPP.m_pModelDataManager->m_pInspectionArea==NULL)
		return;

	int iNoInspectROI = 0;
	iNoInspectROI = THEAPP.m_pModelDataManager->m_pInspectionArea->GetChildTRegionCount();
	if (iNoInspectROI<=0)
		return;

	if (THEAPP.m_pGFunction->ValidHImage(THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[m_iCurImageIndex])==FALSE)
		return;

	UpdateData();

	GTRegion *pInspectROIRgn;
	Hobject HAlignROIRgn, HROIHRegion;

	gen_empty_obj(&HAlignROIRgn);

	for (int iIndex=0; iIndex<iNoInspectROI; iIndex++)
	{
		pInspectROIRgn = THEAPP.m_pModelDataManager->m_pInspectionArea->GetChildTRegion(iIndex);
		if (pInspectROIRgn == NULL)
			continue;

		if (pInspectROIRgn->m_iTeachImageIndex!=(m_iCurImageIndex+1))
			continue;

		if (pInspectROIRgn->m_iInspectionType!=INSPECTION_TYPE_ALIGN)
			continue;

		HROIHRegion = pInspectROIRgn->GetROIHRegion(THEAPP.m_pCalDataService);

		if (THEAPP.m_pGFunction->ValidHRegion(HROIHRegion))
			concat_obj(HAlignROIRgn, HROIHRegion, &HAlignROIRgn);
		else
			continue;
	}

	if (THEAPP.m_pGFunction->ValidHRegion(HAlignROIRgn))
		union1(HAlignROIRgn, &HAlignROIRgn);
	else
	{
		AfxMessageBox("Align ROI가 존재하지 않습니다. Align ROI를 티칭해 주세요.", MB_SYSTEMMODAL);
		return;
	}

	double dRect2CenterY, dRect2CenterX, dPhi, dLength1, dLength2;

	smallest_rectangle2(HAlignROIRgn, &dRect2CenterY, &dRect2CenterX, &dPhi, &dLength1, &dLength2);

	if (dPhi >= PI/4)
		dPhi = dPhi - PI/2;
	else if (dPhi < -PI/4)
		dPhi = dPhi + PI/2;

	HTuple HomMat2DIdentity, HomMat2DRotate;
	hom_mat2d_identity(&HomMat2DIdentity);
	hom_mat2d_rotate(HomMat2DIdentity, -dPhi, dRect2CenterY, dRect2CenterX, &HomMat2DRotate);

	CString strModelName, strFolder, strImageFolder, strImageName;

	strModelName = THEAPP.m_pModelDataManager->m_sModelName;
	if(!THEAPP.Struct_PreferenceStruct.m_bUseAbsolutePathModel) //Ver2629
		strFolder = THEAPP.GetWorkingDirectory()+"\\Model\\";
	else
		strFolder = THEAPP.GetWorkingEvmsDirectory()+"\\Model\\";

	strImageFolder = strFolder+strModelName + "\\SW\\TeachImage\\";

	THEAPP.m_FileBase.CreatePath(strImageFolder ,TRUE); //Ver2629

	CString TeachImageFileName, strTeachFileFullName;
	int i;

	for (i=0; i<MAX_IMAGE_TAB; i++)
	{
		if (i >= THEAPP.m_iMaxInspImageNo)
			break;

		if (THEAPP.m_pGFunction->ValidHImage(THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[i])==FALSE)
			continue;

		if (m_bCheckAlignImage[i]==TRUE)
		{
			if (m_bCheckUseMatchingAffineConstant)
				affine_trans_image(THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[i], &(THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[i]), HomMat2DRotate, "constant", "false");
			else
				affine_trans_image(THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[i], &(THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[i]), HomMat2DRotate, "nearest_neighbor", "false");
					
			TeachImageFileName.Format("Grab_%s", g_sImageName[THEAPP.m_iMachineInspType][i]);

			strTeachFileFullName = strImageFolder + "\\" + TeachImageFileName;
			write_image(THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[i], "bmp", 0, HTuple(strTeachFileFullName));
		}
	}
	
	THEAPP.m_pInspectAdminViewDlg->UpdateView();
}


void CMatchingLocalDlg::OnBnClickedButtonTestMatchingModel()
{
try
{
	UpdateData();

	double dStartTick = 0, dEndTick = 0;

	dStartTick = GetTickCount();

	if (m_bCheckUseMatchingNCC)
	{
		if (THEAPP.m_pModelDataManager->m_pInspectionArea==NULL)
			return;

		int iNoInspectROI = 0;
		iNoInspectROI = THEAPP.m_pModelDataManager->m_pInspectionArea->GetChildTRegionCount();
		if (iNoInspectROI<=0)
			return;

		if (THEAPP.m_pGFunction->ValidHImage(THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[m_iCurImageIndex])==FALSE)
			return;

		if (THEAPP.m_pModelDataManager->m_lLAlignModelID[m_iCurImageIndex]<0)
		{
			AfxMessageBox("현재 영상 탭에 대한 매칭 모델이 존재하지 않습니다. 먼저 매칭 모델을 생성해 주세요.", MB_SYSTEMMODAL);
			return;
		}

		HTuple dAngleRangeRad;
		tuple_rad(THEAPP.m_pModelDataManager->m_iLocalMatchingAngleRange[m_iCurImageIndex], &dAngleRangeRad);

		double dMatchingScore, dTeachAlignRefX, dTeachAlignRefY;
		dMatchingScore = THEAPP.m_pModelDataManager->m_dLocalMatchingScore[m_iCurImageIndex];
		dTeachAlignRefX = THEAPP.m_pModelDataManager->m_dLocalTeachAlignRefX[m_iCurImageIndex];
		dTeachAlignRefY = THEAPP.m_pModelDataManager->m_dLocalTeachAlignRefY[m_iCurImageIndex];

		int iMatchingSearchMarginX = THEAPP.m_pModelDataManager->m_iLocalMatchingSearchMarginX[m_iCurImageIndex];
		int iMatchingSearchMarginY = THEAPP.m_pModelDataManager->m_iLocalMatchingSearchMarginY[m_iCurImageIndex];

		Hlong lNoFoundNumber = 0;
		HTuple Row, Column, Angle, Score;
		HTuple HomMat2DIdentity, HomMat2DTranslate, HomMat2DRotate;

		double dTransX, dTransY, dRotationAngle;
		Hobject HModelContour, HModelRgn, HMatchingImageReduced;

		GTRegion *pInspectROIRgn;
		Hobject HAlignROIRgn, HROIHRegion;

		gen_empty_obj(&HAlignROIRgn);

		for (int iIndex=0; iIndex<iNoInspectROI; iIndex++)
		{
			pInspectROIRgn = THEAPP.m_pModelDataManager->m_pInspectionArea->GetChildTRegion(iIndex);
			if (pInspectROIRgn == NULL)
				continue;

			// 0번 영상에 있는 Align ROI만 추가

			if (pInspectROIRgn->m_iTeachImageIndex!=(m_iCurImageIndex+1))
				continue;

			if (pInspectROIRgn->m_iInspectionType!=INSPECTION_TYPE_ALIGN)
				continue;

			HROIHRegion = pInspectROIRgn->GetROIHRegion(THEAPP.m_pCalDataService);

			if (THEAPP.m_pGFunction->ValidHRegion(HROIHRegion))
				concat_obj(HAlignROIRgn, HROIHRegion, &HAlignROIRgn);
			else
				continue;
		}

		if (THEAPP.m_pGFunction->ValidHRegion(HAlignROIRgn))
			union1(HAlignROIRgn, &HAlignROIRgn);
		else
		{
			AfxMessageBox("Align ROI가 존재하지 않습니다. Align ROI를 티칭해 주세요.", MB_SYSTEMMODAL);
			return;
		}

		shape_trans(HAlignROIRgn, &HModelRgn, "rectangle1");
		dilation_rectangle1(HModelRgn, &HModelRgn, iMatchingSearchMarginX*2+1, iMatchingSearchMarginY*2+1);
		reduce_domain(THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[m_iCurImageIndex], HModelRgn, &HMatchingImageReduced);

		Row = HTuple(0.0);
		Column = HTuple(0.0);
		Angle = HTuple(0.0);
		Score = HTuple(0.0);

		find_ncc_model(HMatchingImageReduced,
			THEAPP.m_pModelDataManager->m_lLAlignModelID[m_iCurImageIndex],		// Handle of the model
			-dAngleRangeRad,							// Smallest rotation of the model
			dAngleRangeRad*2.0,							// Extent of the rotation angles.
			dMatchingScore,								// Minumum score of the instances of the model to be found
			1,											// Number of instances of the model to be found
			0.5,										// Maximum overlap of the instances of the model to be found
			"true",										// Subpixel accuracy
			0,											// Number of pyramid levels used in the matching
			&Row,										// Row coordinate of the found instances of the model.
			&Column,									// Column coordinate of the found instances of the model.
			&Angle,										// Rotation angle of the found instances of the model. (radian)
			&Score);									// Score of the found instances of the model.

		tuple_length(Score, &lNoFoundNumber);

		if (lNoFoundNumber<=0)
		{
			AfxMessageBox("매칭 실패: 파라미터를 변경하시거나 새로운 매칭 모델을 등록해주세요.", MB_SYSTEMMODAL);
			return;
		}

		double dDeltaX, dDeltaY, dAngle, dScore;
		dDeltaX = Column[0].D();
		dDeltaY = Row[0].D();
		dAngle = Angle[0].D();
		dScore = Score[0].D();

		vector_angle_to_rigid(dTeachAlignRefY, dTeachAlignRefX, 0, dDeltaY, dDeltaX, dAngle, &HomMat2DRotate);

		gen_empty_obj(&(THEAPP.m_pInspectAdminViewDlg->m_HParameterDefectXLD));

		Hobject HResultXLD;
		affine_trans_region(HAlignROIRgn, &HResultXLD, HomMat2DRotate, "false");
		gen_contour_region_xld(HResultXLD, &HResultXLD, "border");

		if (THEAPP.m_pGFunction->ValidHXLD(HResultXLD))
			concat_obj(THEAPP.m_pInspectAdminViewDlg->m_HParameterDefectXLD, HResultXLD, &(THEAPP.m_pInspectAdminViewDlg->m_HParameterDefectXLD));

		THEAPP.m_pInspectAdminViewDlg->UpdateView();

		dTransX = Column[0].D() - dTeachAlignRefX;
		dTransY = Row[0].D() - dTeachAlignRefY;
		tuple_deg(Angle, &Angle);
		dRotationAngle = Angle[0].D();

		dEndTick = GetTickCount();

		CString sMsg;
		sMsg.Format("(X,Y)위치편차(픽셀)=(%d,%d), 회전각도(Deg)=(%.2lf), 매칭율(%%)=%.1lf, 매칭시간=%.0lf ms", (int)dTransX, (int)dTransY, dRotationAngle, dScore*100.0, dEndTick-dStartTick);
		AfxMessageBox(sMsg, MB_SYSTEMMODAL|MB_ICONINFORMATION);
	}
	else if (m_bCheckUseMatchingShape)
	{
		if (THEAPP.m_pModelDataManager->m_pInspectionArea==NULL)
			return;

		int iNoInspectROI = 0;
		iNoInspectROI = THEAPP.m_pModelDataManager->m_pInspectionArea->GetChildTRegionCount();
		if (iNoInspectROI<=0)
			return;

		if (THEAPP.m_pGFunction->ValidHImage(THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[m_iCurImageIndex])==FALSE)
			return;

		if (THEAPP.m_pModelDataManager->m_lLAlignModelID[m_iCurImageIndex]<0)
		{
			AfxMessageBox("현재 영상 탭에 대한 매칭 모델이 존재하지 않습니다. 먼저 매칭 모델을 생성해 주세요.", MB_SYSTEMMODAL);
			return;
		}

		HTuple dAngleRangeRad;
		tuple_rad(THEAPP.m_pModelDataManager->m_iLocalMatchingAngleRange[m_iCurImageIndex], &dAngleRangeRad);

		double dMatchingScaleMin, dMatchingScaleMax, dMatchingScore, dTeachAlignRefX, dTeachAlignRefY;
		dMatchingScaleMin = THEAPP.m_pModelDataManager->m_dLocalMatchingScaleMin[m_iCurImageIndex];
		dMatchingScaleMax = THEAPP.m_pModelDataManager->m_dLocalMatchingScaleMax[m_iCurImageIndex];
		dMatchingScore = THEAPP.m_pModelDataManager->m_dLocalMatchingScore[m_iCurImageIndex];
		dTeachAlignRefX = THEAPP.m_pModelDataManager->m_dLocalTeachAlignRefX[m_iCurImageIndex];
		dTeachAlignRefY = THEAPP.m_pModelDataManager->m_dLocalTeachAlignRefY[m_iCurImageIndex];

		int iMatchingSearchMarginX = THEAPP.m_pModelDataManager->m_iLocalMatchingSearchMarginX[m_iCurImageIndex];
		int iMatchingSearchMarginY = THEAPP.m_pModelDataManager->m_iLocalMatchingSearchMarginY[m_iCurImageIndex];

		Hlong lNoFoundNumber = 0;
		HTuple Row, Column, Angle, Scale, Score;
		HTuple HomMat2DIdentity, HomMat2DTranslate, HomMat2DRotate;

		double dTransX, dTransY, dRotationAngle;
		Hobject HModelContour, HModelRgn, HMatchingImageReduced;

		get_shape_model_contours(&HModelContour, THEAPP.m_pModelDataManager->m_lLAlignModelID[m_iCurImageIndex], 1);
		gen_region_contour_xld(HModelContour, &HModelRgn, "filled");
		union1(HModelRgn, &HModelRgn);
		shape_trans(HModelRgn, &HModelRgn, "rectangle1");
		dilation_rectangle1(HModelRgn, &HModelRgn, iMatchingSearchMarginX*2+1, iMatchingSearchMarginY*2+1);
		move_region(HModelRgn, &HModelRgn, (Hlong)dTeachAlignRefY, (Hlong)dTeachAlignRefX);
		reduce_domain(THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[m_iCurImageIndex], HModelRgn, &HMatchingImageReduced);

		Row = HTuple(0.0);
		Column = HTuple(0.0);
		Angle = HTuple(0.0);
		Scale = HTuple(0.0);
		Score = HTuple(0.0);

		find_scaled_shape_model(HMatchingImageReduced,
			THEAPP.m_pModelDataManager->m_lLAlignModelID[m_iCurImageIndex],		// Handle of the model
			-dAngleRangeRad,							// Smallest rotation of the model
			dAngleRangeRad*2.0,							// Extent of the rotation angles.
			dMatchingScaleMin,
			dMatchingScaleMax,
			dMatchingScore,								// Minumum score of the instances of the model to be found
			1,											// Number of instances of the model to be found
			0.5,										// Maximum overlap of the instances of the model to be found
			"interpolation",							// Subpixel accuracy if not equal to 'none'. 'none', 'interpolation', 'least_squares', 'least_squares_high', 'least_squares_very_high'
			0,											// Number of pyramid levels used in the matching
			0.9,										// Greediness'' of the search heuristic (0: safe but slow; 1: fast but matches may be missed).
			&Row,										// Row coordinate of the found instances of the model.
			&Column,									// Column coordinate of the found instances of the model.
			&Angle,										// Rotation angle of the found instances of the model. (radian)
			&Scale,
			&Score);									// Score of the found instances of the model.

		tuple_length(Score, &lNoFoundNumber);

		if (lNoFoundNumber<=0)
		{
			AfxMessageBox("매칭 실패: 파라미터를 변경하시거나 새로운 매칭 모델을 등록해주세요.", MB_SYSTEMMODAL);
			return;
		}

		double dDeltaX, dDeltaY, dAngle, dScore;
		dDeltaX = Column[0].D();
		dDeltaY = Row[0].D();
		dAngle = Angle[0].D();
		dScore = Score[0].D();

		hom_mat2d_identity(&HomMat2DIdentity);
		hom_mat2d_translate(HomMat2DIdentity, dDeltaY, dDeltaX, &HomMat2DTranslate);
		hom_mat2d_rotate(HomMat2DTranslate, dAngle, dDeltaY, dDeltaX, &HomMat2DRotate);

		gen_empty_obj(&(THEAPP.m_pInspectAdminViewDlg->m_HParameterDefectXLD));

		Hobject HResultXLD;
		affine_trans_contour_xld(HModelContour, &HResultXLD, HomMat2DRotate);

		if (THEAPP.m_pGFunction->ValidHXLD(HResultXLD))
			concat_obj(THEAPP.m_pInspectAdminViewDlg->m_HParameterDefectXLD, HResultXLD, &(THEAPP.m_pInspectAdminViewDlg->m_HParameterDefectXLD));

		THEAPP.m_pInspectAdminViewDlg->UpdateView();

		dTransX = Column[0].D() - dTeachAlignRefX;
		dTransY = Row[0].D() - dTeachAlignRefY;
		tuple_deg(Angle, &Angle);
		dRotationAngle = Angle[0].D();

		dEndTick = GetTickCount();

		CString sMsg;
		sMsg.Format("(X,Y)위치편차(픽셀)=(%d,%d), 회전각도(Deg)=(%.2lf), 매칭율(%%)=%.1lf, 매칭시간=%.0lf ms", (int)dTransX, (int)dTransY, dRotationAngle, dScore*100.0, dEndTick-dStartTick);
		AfxMessageBox(sMsg, MB_SYSTEMMODAL|MB_ICONINFORMATION);
	}

}
catch(HException &except)
{
	CString str; str.Format("Halcon Exception [CMatchingLocalDlg::OnBnClickedButtonTestMatchingModel] : %s", except.message); THEAPP.SaveLog(str);
}
}

void CMatchingLocalDlg::OnBnClickedCheckUseShapeMatching()
{
	m_bCheckUseMatchingShape = TRUE;
	m_bCheckUseMatchingNCC = FALSE;
	
	UpdateData(FALSE);

	SetControlState(ALIGN_MATCHING_SHAPE);
}

void CMatchingLocalDlg::OnClickedCheckUseTemplateMatching()
{
	m_bCheckUseMatchingShape = FALSE;
	m_bCheckUseMatchingNCC = TRUE;

	UpdateData(FALSE);

	SetControlState(ALIGN_MATCHING_TEMPLATE);
}

void CMatchingLocalDlg::OnBnClickedButtonAffineImage()
{
	int nRes = 0;
	nRes = AfxMessageBox("티칭영상이 변경되오니 주의하십시요. 현재 설정으로 영상을 이동하시겠습니까?", MB_YESNO|MB_SYSTEMMODAL);

	if ((nRes!=IDYES))
		return;

	if (THEAPP.m_pGFunction->ValidHImage(THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[m_iCurImageIndex])==FALSE)
		return;

	UpdateData();

	HTuple HomMat2DIdentity, HomMat2DTranslate, HomMat2DRotate;

	hom_mat2d_identity(&HomMat2DIdentity);
	hom_mat2d_translate(HomMat2DIdentity, m_dEditDeltaY, m_dEditDeltaX, &HomMat2DTranslate);

	HTuple HRadian, HWidth, HHeight;
	tuple_rad(m_dEditRotationAngle, &HRadian);

	BYTE *pImageData;
	char type[30];
	Hlong lImageWidth, lImageHeight;
	get_image_pointer1(THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[m_iCurImageIndex], (Hlong*)&pImageData, type, &lImageWidth, &lImageHeight);

	hom_mat2d_rotate(HomMat2DTranslate, HRadian, lImageHeight/2, lImageWidth/2, &HomMat2DRotate);

	CString strModelName, strFolder, strImageFolder, strImageName;

	strModelName = THEAPP.m_pModelDataManager->m_sModelName;
	if(!THEAPP.Struct_PreferenceStruct.m_bUseAbsolutePathModel) //Ver2629
		strFolder = THEAPP.GetWorkingDirectory()+"\\Model\\";
	else
		strFolder = THEAPP.GetWorkingEvmsDirectory()+"\\Model\\";

	strImageFolder = strFolder+strModelName + "\\SW\\TeachImage\\";

	THEAPP.m_FileBase.CreatePath(strImageFolder ,TRUE); //Ver2629

	CString TeachImageFileName, strTeachFileFullName;
	int i;

	for (i=0; i<MAX_IMAGE_TAB; i++)
	{
		if (i >= THEAPP.m_iMaxInspImageNo)
			break;

		if (THEAPP.m_pGFunction->ValidHImage(THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[i])==FALSE)
			continue;

		if (m_bCheckAlignImage[i]==TRUE)
		{
			if (m_bCheckUseMatchingAffineConstant)
				affine_trans_image(THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[i], &(THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[i]), HomMat2DRotate, "constant", "false");
			else
				affine_trans_image(THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[i], &(THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[i]), HomMat2DRotate, "nearest_neighbor", "false");

			TeachImageFileName.Format("Grab_%s", g_sImageName[THEAPP.m_iMachineInspType][i]);

			strTeachFileFullName = strImageFolder + "\\" + TeachImageFileName;
			write_image(THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[i], "bmp", 0, HTuple(strTeachFileFullName));
		}
	}

	THEAPP.m_pInspectAdminViewDlg->UpdateView();
}
