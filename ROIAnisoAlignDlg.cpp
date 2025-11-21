// AlgorithmROIAnisoAlignDlg.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "uScan.h"
#include "ROIAnisoAlignDlg.h"
#include "afxdialogex.h"


// CAlgorithmROIAnisoAlignDlg 대화 상자입니다.

IMPLEMENT_DYNAMIC(CROIAnisoAlignDlg, CDialog)

CROIAnisoAlignDlg::CROIAnisoAlignDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CROIAnisoAlignDlg::IDD, pParent)
{
	m_iSelectedImageType = 0;
	m_pSelectedROI = NULL;
	m_iSelectedInspectionTabIndex = 0;

	m_bCheckUseROIAnisoAlign = FALSE;
	m_iEditROIAnisoAlignXSearchMargin = 0;
	m_iEditROIAnisoAlignYSearchMargin = 0;
	m_iEditROIAnisoAlignMatchingAngleRange = 0;
	m_dEditROIAnisoAlignMatchingScaleMinY = 1.0;
	m_dEditROIAnisoAlignMatchingScaleMaxY = 1.0;
	m_dEditROIAnisoAlignMatchingScaleMinX = 1.0;
	m_dEditROIAnisoAlignMatchingScaleMaxX = 1.0;
	m_iEditROIAnisoAlignMatchingMinContrast = 0;
	m_bCheckROIAnisoAlignUsePosX = FALSE;
	m_bCheckROIAnisoAlignUsePosY = FALSE;
	m_bCheckROIAnisoAlignInspectShift = FALSE;
	m_dEditROIAnisoAlignShiftTop = 0.0;
	m_dEditROIAnisoAlignShiftBottom = 0.0;
	m_dEditROIAnisoAlignShiftLeft = 0.0;
	m_dEditROIAnisoAlignShiftRight = 0.0;
	m_bCheckROIAnisoAlignLocalAlignUse = FALSE;
	m_dEditROIAnisoAlignMatchingScore = 0.0;
	m_bROIAnisoAlignModelExist = FALSE;
}

CROIAnisoAlignDlg::~CROIAnisoAlignDlg()
{
}

void CROIAnisoAlignDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Check(pDX, IDC_CHECK_USE_ROI_ALIGN, m_bCheckUseROIAnisoAlign);
	DDX_Text(pDX, IDC_EDIT_ROI_ALIGN_X_SEARCH_MARGIN, m_iEditROIAnisoAlignXSearchMargin);
	DDX_Text(pDX, IDC_EDIT_ROI_ALIGN_Y_SEARCH_MARGIN, m_iEditROIAnisoAlignYSearchMargin);
	DDX_Text(pDX, IDC_EDIT_ROI_ALIGN_MATCHING_ANGLE_RANGE, m_iEditROIAnisoAlignMatchingAngleRange);
	DDX_Text(pDX, IDC_EDIT_ROI_ALIGN_MATCHING_SCALE_MIN_Y, m_dEditROIAnisoAlignMatchingScaleMinY);
	DDX_Text(pDX, IDC_EDIT_ROI_ALIGN_MATCHING_SCALE_MAX_Y, m_dEditROIAnisoAlignMatchingScaleMaxY);
	DDX_Text(pDX, IDC_EDIT_ROI_ALIGN_MATCHING_SCALE_MIN_X, m_dEditROIAnisoAlignMatchingScaleMinX);
	DDX_Text(pDX, IDC_EDIT_ROI_ALIGN_MATCHING_SCALE_MAX_X, m_dEditROIAnisoAlignMatchingScaleMaxX);
	DDX_Text(pDX, IDC_EDIT_ROI_ALIGN_MATCHING_MIN_CONTRAST, m_iEditROIAnisoAlignMatchingMinContrast);
	DDX_Check(pDX, IDC_CHECK_ROI_ALIGN_USE_POS_X, m_bCheckROIAnisoAlignUsePosX);
	DDX_Check(pDX, IDC_CHECK_ROI_ALIGN_USE_POS_Y, m_bCheckROIAnisoAlignUsePosY);
	DDX_Check(pDX, IDC_CHECK_ROI_ALIGN_INSPECT_SHIFT, m_bCheckROIAnisoAlignInspectShift);
	DDX_Text(pDX, IDC_EDIT_ROI_ALIGN_SHIFT_TOP, m_dEditROIAnisoAlignShiftTop);
	DDX_Text(pDX, IDC_EDIT_ROI_ALIGN_SHIFT_BOTTOM, m_dEditROIAnisoAlignShiftBottom);
	DDX_Text(pDX, IDC_EDIT_ROI_ALIGN_SHIFT_LEFT, m_dEditROIAnisoAlignShiftLeft);
	DDX_Text(pDX, IDC_EDIT_ROI_ALIGN_SHIFT_RIGHT, m_dEditROIAnisoAlignShiftRight);
	DDX_Check(pDX, IDC_CHECK_ROI_ALIGN_LOCAL_ALIGN_USE, m_bCheckROIAnisoAlignLocalAlignUse);
	DDX_Text(pDX, IDC_EDIT_ROI_ALIGN_MATCHING_SCORE, m_dEditROIAnisoAlignMatchingScore);
	DDX_Check(pDX, IDC_CHECK_ANISO_ALIGN_MODEL_EXIST, m_bROIAnisoAlignModelExist);
}

BEGIN_MESSAGE_MAP(CROIAnisoAlignDlg, CDialog)
	ON_BN_CLICKED(IDC_BUTTON_TEST_MATCHING_MODEL, &CROIAnisoAlignDlg::OnBnClickedButtonTestMatchingModel)
	ON_BN_CLICKED(IDC_BUTTON_CREATE_MATCHING_MODEL, &CROIAnisoAlignDlg::OnBnClickedButtonCreateMatchingModel)
	ON_BN_CLICKED(IDC_BUTTON_DELETE_MATCHING_MODEL, &CROIAnisoAlignDlg::OnBnClickedButtonDeleteMatchingModel)
END_MESSAGE_MAP()


// CAlgorithmROIAnisoAlignDlg 메시지 처리기입니다.

void CROIAnisoAlignDlg::SetSelectedInspection(int iImageType, GTRegion *pSelectedROI, int iTabIndex)
{ 
	m_iSelectedImageType = iImageType;
	m_pSelectedROI = pSelectedROI;
	m_iSelectedInspectionTabIndex = iTabIndex; 
}

void CROIAnisoAlignDlg::SetParam(CAlgorithmParam AlgorithmParam)
{
	m_bCheckUseROIAnisoAlign = AlgorithmParam.m_bUseROIAnisoAlign;
	m_iEditROIAnisoAlignXSearchMargin = AlgorithmParam.m_iROIAnisoAlignXSearchMargin;
	m_iEditROIAnisoAlignYSearchMargin = AlgorithmParam.m_iROIAnisoAlignYSearchMargin;
	m_iEditROIAnisoAlignMatchingAngleRange = AlgorithmParam.m_iROIAnisoAlignMatchingAngleRange;
	m_dEditROIAnisoAlignMatchingScaleMinY = AlgorithmParam.m_dROIAnisoAlignMatchingScaleMinY;
	m_dEditROIAnisoAlignMatchingScaleMaxY = AlgorithmParam.m_dROIAnisoAlignMatchingScaleMaxY;
	m_dEditROIAnisoAlignMatchingScaleMinX = AlgorithmParam.m_dROIAnisoAlignMatchingScaleMinX;
	m_dEditROIAnisoAlignMatchingScaleMaxX = AlgorithmParam.m_dROIAnisoAlignMatchingScaleMaxX;
	m_iEditROIAnisoAlignMatchingMinContrast = AlgorithmParam.m_iROIAnisoAlignMatchingMinContrast;
	m_bCheckROIAnisoAlignUsePosX = AlgorithmParam.m_bROIAnisoAlignUsePosX;
	m_bCheckROIAnisoAlignUsePosY = AlgorithmParam.m_bROIAnisoAlignUsePosY;
	m_bCheckROIAnisoAlignInspectShift = AlgorithmParam.m_bROIAnisoAlignInspectShift;
	m_dEditROIAnisoAlignShiftTop = AlgorithmParam.m_dROIAnisoAlignShiftTop;
	m_dEditROIAnisoAlignShiftBottom = AlgorithmParam.m_dROIAnisoAlignShiftBottom;
	m_dEditROIAnisoAlignShiftLeft = AlgorithmParam.m_dROIAnisoAlignShiftLeft;
	m_dEditROIAnisoAlignShiftRight = AlgorithmParam.m_dROIAnisoAlignShiftRight;
	m_bCheckROIAnisoAlignLocalAlignUse = AlgorithmParam.m_bROIAnisoAlignLocalAlignUse;
	m_dEditROIAnisoAlignMatchingScore = AlgorithmParam.m_dROIAnisoAlignMatchingScore;
	m_bROIAnisoAlignModelExist = AlgorithmParam.m_bROIAnisoAlignModelExist;

	if(m_pSelectedROI->m_HAnisoModelID >= 0)
		m_bROIAnisoAlignModelExist = TRUE;
	else
		m_bROIAnisoAlignModelExist = FALSE;

	UpdateData(FALSE);
}

void CROIAnisoAlignDlg::GetParam(CAlgorithmParam *pAlgorithmParam)
{
	UpdateData();

	pAlgorithmParam->m_bUseROIAnisoAlign = m_bCheckUseROIAnisoAlign;
	pAlgorithmParam->m_iROIAnisoAlignXSearchMargin = m_iEditROIAnisoAlignXSearchMargin;
	pAlgorithmParam->m_iROIAnisoAlignYSearchMargin = m_iEditROIAnisoAlignYSearchMargin;
	pAlgorithmParam->m_iROIAnisoAlignMatchingAngleRange = m_iEditROIAnisoAlignMatchingAngleRange;
	pAlgorithmParam->m_dROIAnisoAlignMatchingScaleMinY = m_dEditROIAnisoAlignMatchingScaleMinY;
	pAlgorithmParam->m_dROIAnisoAlignMatchingScaleMaxY = m_dEditROIAnisoAlignMatchingScaleMaxY;
	pAlgorithmParam->m_dROIAnisoAlignMatchingScaleMinX = m_dEditROIAnisoAlignMatchingScaleMinX;
	pAlgorithmParam->m_dROIAnisoAlignMatchingScaleMaxX = m_dEditROIAnisoAlignMatchingScaleMaxX;
	pAlgorithmParam->m_iROIAnisoAlignMatchingMinContrast = m_iEditROIAnisoAlignMatchingMinContrast;
	pAlgorithmParam->m_bROIAnisoAlignUsePosX = m_bCheckROIAnisoAlignUsePosX;
	pAlgorithmParam->m_bROIAnisoAlignUsePosY = m_bCheckROIAnisoAlignUsePosY;
	pAlgorithmParam->m_bROIAnisoAlignInspectShift = m_bCheckROIAnisoAlignInspectShift;
	pAlgorithmParam->m_dROIAnisoAlignShiftTop = m_dEditROIAnisoAlignShiftTop;
	pAlgorithmParam->m_dROIAnisoAlignShiftBottom = m_dEditROIAnisoAlignShiftBottom;
	pAlgorithmParam->m_dROIAnisoAlignShiftLeft = m_dEditROIAnisoAlignShiftLeft;
	pAlgorithmParam->m_dROIAnisoAlignShiftRight = m_dEditROIAnisoAlignShiftRight;
	pAlgorithmParam->m_bROIAnisoAlignLocalAlignUse = m_bCheckROIAnisoAlignLocalAlignUse;
	pAlgorithmParam->m_dROIAnisoAlignMatchingScore = m_dEditROIAnisoAlignMatchingScore;
	pAlgorithmParam->m_bROIAnisoAlignModelExist = m_bROIAnisoAlignModelExist;
}

BOOL CROIAnisoAlignDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

void CROIAnisoAlignDlg::OnBnClickedButtonTestMatchingModel()
{
	try
	{
		if (THEAPP.m_pGFunction->ValidHImage(THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[m_iSelectedImageType])==FALSE)
		{
			AfxMessageBox("티칭 영상이 존재하지 않습니다.", MB_SYSTEMMODAL);
			return;
		}

		UpdateData();

		Hobject HInspectAreaRgn;
		HInspectAreaRgn = m_pSelectedROI->GetROIHRegion(THEAPP.m_pCalDataService);

		HTuple dAngleRangeRad;
		tuple_rad((double)m_iEditROIAnisoAlignMatchingAngleRange, &dAngleRangeRad);

		if (dAngleRangeRad<0)
			dAngleRangeRad = dAngleRangeRad * -1.0;

		HTuple lROIAnisoAlignModelID;

		Hobject HInspectAreaXLD;
		gen_contour_region_xld(HInspectAreaRgn, &HInspectAreaXLD, "border_holes");

		lROIAnisoAlignModelID = -1;
	
		create_aniso_shape_model_xld(HInspectAreaXLD, "auto", -dAngleRangeRad, dAngleRangeRad*2.0, "auto",
		m_dEditROIAnisoAlignMatchingScaleMinY, m_dEditROIAnisoAlignMatchingScaleMaxY, "auto", 
		m_dEditROIAnisoAlignMatchingScaleMinX, m_dEditROIAnisoAlignMatchingScaleMaxX, "auto", 
		"auto", "ignore_local_polarity", m_iEditROIAnisoAlignMatchingMinContrast, &lROIAnisoAlignModelID);

		if (lROIAnisoAlignModelID<0)
		{
			AfxMessageBox("모델 생성 실패.", MB_SYSTEMMODAL);
			return;
		}

		Hlong lROIArea;
		double dROICenterX, dROICenterY;
		HTuple HlROIArea, HdROICenterX, HdROICenterY;
		Hobject HInspectAreaRectRgn;
		shape_trans(HInspectAreaRgn, &HInspectAreaRectRgn, "rectangle1");
		area_center(HInspectAreaRectRgn, &HlROIArea, &HdROICenterY, &HdROICenterX);
		HlROIArea = HlROIArea[0].L();
		dROICenterY = HdROICenterY[0].D();
		dROICenterX = HdROICenterX[0].D();

		int iSearchMarginX = m_iEditROIAnisoAlignXSearchMargin;
		if (iSearchMarginX<0)
			iSearchMarginX = 0;
		int iSearchMarginY = m_iEditROIAnisoAlignYSearchMargin;
		if (iSearchMarginY<0)
			iSearchMarginY = 0;

		Hobject HInspectAreaDilatedRgn;
		dilation_rectangle1(HInspectAreaRgn, &HInspectAreaDilatedRgn, iSearchMarginX*2+1, iSearchMarginY*2+1);

		Hobject HMatchingImageReduced;
		reduce_domain(THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[m_iSelectedImageType], HInspectAreaDilatedRgn, &HMatchingImageReduced);
		
		Hlong lNoFoundNumber = 0;
		HTuple Row, Column, Angle, ScaleX, ScaleY, Score;
		HTuple HomMat2DIdentity, HomMat2DTranslate, HomMat2DRotate, HomMat2DScale;

		double dTransX, dTransY;
		Hobject HModelContour;

		Row = HTuple(0.0);
		Column = HTuple(0.0);
		Angle = HTuple(0.0);
		ScaleX = HTuple(0.0);
		ScaleY = HTuple(0.0);
		Score = HTuple(0.0);

		find_aniso_shape_model(HMatchingImageReduced,
			lROIAnisoAlignModelID,							// Handle of the model
			-dAngleRangeRad,							// Smallest rotation of the model
			dAngleRangeRad*2.0,
			m_dEditROIAnisoAlignMatchingScaleMinY,
			m_dEditROIAnisoAlignMatchingScaleMaxY,
			m_dEditROIAnisoAlignMatchingScaleMinX,
			m_dEditROIAnisoAlignMatchingScaleMaxX,
			m_dEditROIAnisoAlignMatchingScore,				// Minumum score of the instances of the model to be found
			1,											// Number of instances of the model to be found
			0.5,										// Maximum overlap of the instances of the model to be found
			"interpolation",							// Subpixel accuracy if not equal to 'none'. 'none', 'interpolation', 'least_squares', 'least_squares_high', 'least_squares_very_high'
			0,											// Number of pyramid levels used in the matching
			0.9,										// Greediness'' of the search heuristic (0: safe but slow; 1: fast but matches may be missed).
			&Row,										// Row coordinate of the found instances of the model.
			&Column,									// Column coordinate of the found instances of the model.
			&Angle,										// Rotation angle of the found instances of the model. (radian)
			&ScaleY,
			&ScaleX,
			&Score);									// Score of the found instances of the model.

		HTuple HlNoFoundNumber;
		tuple_length(Score, &HlNoFoundNumber);
		lNoFoundNumber = HlNoFoundNumber[0].L();

		if (lNoFoundNumber<=0)
		{
			AfxMessageBox("실패: 경계 Edge 검출 파라미터를 변경 후 재시도 하십시요.", MB_SYSTEMMODAL);
			return;
		}

		double dDeltaX, dDeltaY, dAngle, dScaleY, dScaleX, dScore;
		dDeltaX = Column[0].D();
		dDeltaY = Row[0].D();
		dAngle = Angle[0].D();
		dScore = Score[0].D();
		dScaleY = ScaleY[0].D();
		dScaleX = ScaleX[0].D();
		double dAbs;
		Hobject HAlignRgn;
		Hobject HResultXLD;
		long lMoveX, lMoveY;

		if (m_iEditROIAnisoAlignMatchingAngleRange==0 && m_dEditROIAnisoAlignMatchingScaleMinY==1 && m_dEditROIAnisoAlignMatchingScaleMaxY==1&& m_dEditROIAnisoAlignMatchingScaleMinX==1 && m_dEditROIAnisoAlignMatchingScaleMaxX==1)
		{
			if (m_bCheckROIAnisoAlignUsePosX)
			{
				dTransX = dDeltaX - dROICenterX;
				dAbs = fabs(dTransX);
				if (dAbs > (double)iSearchMarginX)
					dTransX = 0;
			}
			else
			{
				dTransX = 0;
			}

			if (m_bCheckROIAnisoAlignUsePosY)
			{
				dTransY = dDeltaY - dROICenterY;
				dAbs = fabs(dTransY);
				if (dAbs > (double)iSearchMarginY)
					dTransY = 0;
			}
			else
			{
				dTransY = 0;
			}

			lMoveX = (long)(dTransX+0.5);
			lMoveY = (long)(dTransY+0.5);

			move_region(HInspectAreaRgn, &HAlignRgn, lMoveY, lMoveX);
			gen_contour_region_xld(HAlignRgn, &HResultXLD, "border_holes");
		}
		else
		{
			hom_mat2d_identity(&HomMat2DIdentity); // 확인 필요..CHL
			hom_mat2d_translate(HomMat2DIdentity, dDeltaY, dDeltaX, &HomMat2DTranslate);
			hom_mat2d_rotate(HomMat2DTranslate, dAngle, dDeltaY, dDeltaX, &HomMat2DRotate);
			hom_mat2d_scale(HomMat2DRotate, dScaleY, dScaleX, dDeltaY, dDeltaX, &HomMat2DScale);

			get_shape_model_contours(&HModelContour, lROIAnisoAlignModelID, 1);
			affine_trans_contour_xld(HModelContour, &HResultXLD, HomMat2DScale);
		}

		gen_empty_obj(&(THEAPP.m_pInspectAdminViewDlg->m_HParameterDefectXLD));

		if (THEAPP.m_pGFunction->ValidHXLD(HResultXLD))
			concat_obj(THEAPP.m_pInspectAdminViewDlg->m_HParameterDefectXLD, HResultXLD, &(THEAPP.m_pInspectAdminViewDlg->m_HParameterDefectXLD));

		THEAPP.m_pInspectAdminViewDlg->UpdateView();

		clear_shape_model(lROIAnisoAlignModelID);

		// For Message
		dTransX = dDeltaX - dROICenterX;
		dTransY = dDeltaY - dROICenterY;
		lMoveX = (long)(dTransX+0.5);
		lMoveY = (long)(dTransY+0.5);

		CString sMsg;
		sMsg.Format("(X,Y)위치편차(픽셀)=(%d,%d), (Angle)각도편차(Radian)=(%.3lf), (X,Y)Scale=(%.2lf,%.2lf), 매칭율(%%)=%d", lMoveX, lMoveY, dAngle, dScaleX, dScaleY, (int)(dScore*100.0));
		AfxMessageBox(sMsg, MB_SYSTEMMODAL|MB_ICONINFORMATION);

	}
	catch(HException &except)
	{
		HTuple HExp;
		HTuple HOperatorName, HErrMsg;
		CString sOperatorName, sErrMsg;
		except.ToHTuple(&HExp);
		except.GetExceptionData(HExp, "operator", &HOperatorName);
		except.GetExceptionData(HExp, "error_message", &HErrMsg);
		sOperatorName = HOperatorName[0].S();
		sErrMsg = HErrMsg[0].S();

		CString str; 
		str.Format("Halcon Exception [CAlgorithmROIAnisoAlignDlg::OnBnClickedButtonTestMatchingModel] : <%s>%s", sOperatorName, sErrMsg);
		THEAPP.SaveLog(str);

		return;
	}
}

void CROIAnisoAlignDlg::OnBnClickedButtonCreateMatchingModel()
{
	UpdateData();

	if (m_pSelectedROI->m_HAnisoModelID >= 0)
	{
		clear_shape_model(m_pSelectedROI->m_HAnisoModelID);
		m_pSelectedROI->m_HAnisoModelID = -1;
	}

	Hobject HInspectAreaRgn;
	HInspectAreaRgn = m_pSelectedROI->GetROIHRegion(THEAPP.m_pCalDataService);

	HTuple dAngleRangeRad;
	tuple_rad((double)m_iEditROIAnisoAlignMatchingAngleRange, &dAngleRangeRad);

	if (dAngleRangeRad<0)
		dAngleRangeRad = dAngleRangeRad * - 1.0;

	Hobject HInspectAreaXLD;
	gen_contour_region_xld(HInspectAreaRgn, &HInspectAreaXLD, "border_holes");
	
	create_aniso_shape_model_xld(HInspectAreaXLD, "auto", -dAngleRangeRad, dAngleRangeRad*2.0, "auto", m_dEditROIAnisoAlignMatchingScaleMinY, m_dEditROIAnisoAlignMatchingScaleMaxY, "auto", m_dEditROIAnisoAlignMatchingScaleMinX, m_dEditROIAnisoAlignMatchingScaleMaxX, "auto", "auto", "ignore_local_polarity", m_iEditROIAnisoAlignMatchingMinContrast, &(m_pSelectedROI->m_HAnisoModelID));

	if (m_pSelectedROI->m_HAnisoModelID <0)
		AfxMessageBox("모델 생성 실패.", MB_SYSTEMMODAL);
	else
		AfxMessageBox("모델 생성 성공.", MB_SYSTEMMODAL);

	m_bROIAnisoAlignModelExist = TRUE;
	UpdateData(FALSE);
}

void CROIAnisoAlignDlg::OnBnClickedButtonDeleteMatchingModel()
{
	if (AfxMessageBox("현재 매칭 모델을 삭제하시겠습니까?", MB_OKCANCEL) == IDOK)
	{
		if (m_pSelectedROI->m_HAnisoModelID >= 0)
		{
			clear_ncc_model(m_pSelectedROI->m_HAnisoModelID);
			m_pSelectedROI->m_HAnisoModelID = -1;

			m_bROIAnisoAlignModelExist = FALSE;
			UpdateData(FALSE);
		}
	}
}
