// LocalAlignPartCheckDlg.cpp : 구현 파일입니다.
// 24.02.27 Local Align 추가 - LeeGW Create

#include "stdafx.h"
#include "uScan.h"
#include "LocalAlignPartCheckDlg.h"
#include "afxdialogex.h"


// CLocalAlignPartCheckDlg 대화 상자입니다.

IMPLEMENT_DYNAMIC(CLocalAlignPartCheckDlg, CDialog)

CLocalAlignPartCheckDlg::CLocalAlignPartCheckDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CLocalAlignPartCheckDlg::IDD, pParent)
{
	m_iSelectedImageType = 0;
	m_iInspectionType = 0;
	m_pSelectedROI = NULL;

	m_iEditPartCheckMatchingAngleRange = 0;
	m_iEditPartCheckMatchingSearchMarginX = 0;
	m_iEditPartCheckMatchingSearchMarginX2 = 0;
	m_iEditPartCheckMatchingSearchMarginY = 0;
	m_iEditPartCheckMatchingSearchMarginY2 = 0;

	m_bCheckLocalAlignPartCheckUse = FALSE;

}

CLocalAlignPartCheckDlg::~CLocalAlignPartCheckDlg()
{
}

void CLocalAlignPartCheckDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Check(pDX, IDC_CHECK_USE_PART_CHECK, m_bCheckLocalAlignPartCheckUse);
	DDX_Text(pDX, IDC_EDIT_PART_CHECK_ANGLE_RANGE, m_iEditPartCheckMatchingAngleRange);
	DDX_Text(pDX, IDC_EDIT_PART_CHECK_MATCHING_SEARCH_MARGIN_X, m_iEditPartCheckMatchingSearchMarginX);
	DDX_Text(pDX, IDC_EDIT_PART_CHECK_MATCHING_SEARCH_MARGIN_X2, m_iEditPartCheckMatchingSearchMarginX2);
	DDX_Text(pDX, IDC_EDIT_PART_CHECK_MATCHING_SEARCH_MARGIN_Y, m_iEditPartCheckMatchingSearchMarginY);
	DDX_Text(pDX, IDC_EDIT_PART_CHECK_MATCHING_SEARCH_MARGIN_Y2, m_iEditPartCheckMatchingSearchMarginY2);
	DDX_Check(pDX, IDC_CHECK_PART_CHECK_MODEL_EXIST, m_bCheckPartCheckModelExist);

}


BEGIN_MESSAGE_MAP(CLocalAlignPartCheckDlg, CDialog)
	ON_BN_CLICKED(IDC_BUTTON_LOCAL_ALIGN_DELETE_MATCHING_MODEL, &CLocalAlignPartCheckDlg::OnBnClickedButtonDeleteMatchingModel)
	ON_BN_CLICKED(IDC_BUTTON_LOCAL_ALIGN_CREATE_MATCHING_MODEL, &CLocalAlignPartCheckDlg::OnBnClickedButtonCreateMatchingModel)
	ON_BN_CLICKED(IDC_BUTTON_LOCAL_ALIGN_TEST_PART_CHECK, &CLocalAlignPartCheckDlg::OnBnClickedButtonTestMatchingModel)
END_MESSAGE_MAP()


// CLocalAlignPartCheckDlg 메시지 처리기입니다.

void CLocalAlignPartCheckDlg::SetSelectedInspection(int iImageType, GTRegion *pSelectedROI, int iInspectionType)
{ 
	m_iSelectedImageType = iImageType;
	m_iInspectionType = iInspectionType;
	m_pSelectedROI = pSelectedROI;


	if (pSelectedROI == NULL || pSelectedROI->miInspectionType != INSPECTION_TYPE_LOCAL_ALIGN)
	{
		GetDlgItem(IDC_CHECK_USE_PART_CHECK)->EnableWindow(FALSE);
		GetDlgItem(IDC_BUTTON_LOCAL_ALIGN_CREATE_MATCHING_MODEL)->EnableWindow(FALSE);
		GetDlgItem(IDC_BUTTON_LOCAL_ALIGN_DELETE_MATCHING_MODEL)->EnableWindow(FALSE);
		GetDlgItem(IDC_BUTTON_LOCAL_ALIGN_TEST_PART_CHECK)->EnableWindow(FALSE);
	}

	else
	{
		GetDlgItem(IDC_CHECK_USE_PART_CHECK)->EnableWindow(TRUE);
		GetDlgItem(IDC_BUTTON_LOCAL_ALIGN_CREATE_MATCHING_MODEL)->EnableWindow(TRUE);
		GetDlgItem(IDC_BUTTON_LOCAL_ALIGN_DELETE_MATCHING_MODEL)->EnableWindow(TRUE);
		GetDlgItem(IDC_BUTTON_LOCAL_ALIGN_TEST_PART_CHECK)->EnableWindow(TRUE);
	}

}

void CLocalAlignPartCheckDlg::SetParam(CAlgorithmParam LocalAlignParam)
{
	m_iEditPartCheckMatchingAngleRange = LocalAlignParam.m_iLocalAlignPartCheckMatchingAngleRange;
	m_iEditPartCheckMatchingSearchMarginX = LocalAlignParam.m_iLocalAlignPartCheckMatchingSearchMarginX;
	m_iEditPartCheckMatchingSearchMarginY = LocalAlignParam.m_iLocalAlignPartCheckMatchingSearchMarginY;
	m_iEditPartCheckMatchingSearchMarginX2 = LocalAlignParam.m_iLocalAlignPartCheckMatchingSearchMarginX2;
	m_iEditPartCheckMatchingSearchMarginY2 = LocalAlignParam.m_iLocalAlignPartCheckMatchingSearchMarginY2;
	m_bCheckLocalAlignPartCheckUse = LocalAlignParam.m_bLocalAlignPartCheckUse;
	
	if (m_pSelectedROI->m_HPartModelID>=0)
		m_bCheckPartCheckModelExist = TRUE;
	else
		m_bCheckPartCheckModelExist = FALSE;

	UpdateData(FALSE);
}

void CLocalAlignPartCheckDlg::GetParam(CAlgorithmParam *pLocalAlignParam)
{
	UpdateData();

	pLocalAlignParam->m_iLocalAlignPartCheckMatchingAngleRange = m_iEditPartCheckMatchingAngleRange;
	pLocalAlignParam->m_iLocalAlignPartCheckMatchingSearchMarginX = m_iEditPartCheckMatchingSearchMarginX;
	pLocalAlignParam->m_iLocalAlignPartCheckMatchingSearchMarginY = m_iEditPartCheckMatchingSearchMarginY;
	pLocalAlignParam->m_iLocalAlignPartCheckMatchingSearchMarginX2 = m_iEditPartCheckMatchingSearchMarginX2;
	pLocalAlignParam->m_iLocalAlignPartCheckMatchingSearchMarginY2 = m_iEditPartCheckMatchingSearchMarginY2;
	pLocalAlignParam->m_bLocalAlignPartCheckUse = m_bCheckLocalAlignPartCheckUse;
}

BOOL CLocalAlignPartCheckDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}



void CLocalAlignPartCheckDlg::OnBnClickedButtonCreateMatchingModel()
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

		if (m_pSelectedROI->m_HPartModelID>= 0) 
		{
			clear_ncc_model(m_pSelectedROI->m_HPartModelID);
			m_pSelectedROI->m_HPartModelID = -1;
		}

		// Model Ref: G-Center of Domain
		double dTeachAlignRefX, dTeachAlignRefY;
		Hlong lArea;
		HTuple HlArea, HdTeachAlignRefY, HdTeachAlignRefX;
		area_center(HInspectAreaRgn, &HlArea, &HdTeachAlignRefY, &HdTeachAlignRefX);
		lArea = HlArea[0].L();
		dTeachAlignRefY = HdTeachAlignRefY[0].D();
		dTeachAlignRefX = HdTeachAlignRefX[0].D();

		Hobject HImageReduced;
		reduce_domain(THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[m_iSelectedImageType], HInspectAreaRgn, &HImageReduced);

		HTuple dAngleRangeRad;
		tuple_rad(m_iEditPartCheckMatchingAngleRange, &dAngleRangeRad);

		if (dAngleRangeRad<0)
			dAngleRangeRad = dAngleRangeRad * -1.0;

		create_ncc_model(HImageReduced, "auto", -dAngleRangeRad, dAngleRangeRad*2.0, "auto", "use_polarity", &(m_pSelectedROI->m_HPartModelID));

		if (m_pSelectedROI->m_HPartModelID==-1)
		{
			AfxMessageBox("모델 생성 실패.", MB_SYSTEMMODAL);
			return;
		}

		// Test
		int iSearchMarginLeft = m_iEditPartCheckMatchingSearchMarginX;
		if (iSearchMarginLeft<0)
			iSearchMarginLeft = 0;
		int iSearchMarginRight = m_iEditPartCheckMatchingSearchMarginX2;
		if (iSearchMarginRight<0)
			iSearchMarginRight = 0;
		int iSearchMarginTop = m_iEditPartCheckMatchingSearchMarginY;
		if (iSearchMarginTop<0)
			iSearchMarginTop = 0;
		int iSearchMarginBottom = m_iEditPartCheckMatchingSearchMarginY2;
		if (iSearchMarginBottom<0)
			iSearchMarginBottom = 0;

		Hobject HInspectAreaDilatedRgn;
		shape_trans(HInspectAreaRgn, &HInspectAreaDilatedRgn, "rectangle1");

		Hlong lLTPointY, lLTPointX, lRBPointY, lRBPointX;
		HTuple HlLTPointY, HlLTPointX, HlRBPointY, HlRBPointX;
		smallest_rectangle1(HInspectAreaDilatedRgn, &HlLTPointY, &HlLTPointX, &HlRBPointY, &HlRBPointX);
		lLTPointY = HlLTPointY[0].L();
		lLTPointX = HlLTPointX[0].L();
		lRBPointY = HlRBPointY[0].L();
		lRBPointX = HlRBPointX[0].L();

		gen_rectangle1(&HInspectAreaDilatedRgn, lLTPointY-iSearchMarginTop, lLTPointX-iSearchMarginLeft, lRBPointY+iSearchMarginBottom, lRBPointX+iSearchMarginRight);

		Hobject HMatchingImageReduced;
		reduce_domain(THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[m_iSelectedImageType], HInspectAreaDilatedRgn, &HMatchingImageReduced);

		Hlong lNoFoundNumber;
		HTuple Row, Column, Angle, Score;
		//double dMatchingScore;
		//dMatchingScore = (double)m_iEditPartCheckMatchingScore * 0.01;

		Row = HTuple(0.0);
		Column = HTuple(0.0);
		Angle = HTuple(0.0);
		Score = HTuple(0.0);

		find_ncc_model(HMatchingImageReduced,
			m_pSelectedROI->m_HPartModelID,				// Handle of the model
			-dAngleRangeRad,							// Smallest rotation of the model
			dAngleRangeRad*2.0,							// Extent of the rotation angles.
			0.1,										// Minumum score of the instances of the model to be found
			1,											// Number of instances of the model to be found
			0.5,										// Maximum overlap of the instances of the model to be found
			"true",										// Subpixel accuracy
			0,											// Number of pyramid levels used in the matching
			&Row,										// Row coordinate of the found instances of the model.
			&Column,									// Column coordinate of the found instances of the model.
			&Angle,										// Rotation angle of the found instances of the model. (radian)
			&Score);									// Score of the found instances of the model.

		HTuple HlNoFoundNumber;
		tuple_length(Score, &HlNoFoundNumber);
		lNoFoundNumber = HlNoFoundNumber[0].L();

		if (lNoFoundNumber<=0)
		{
			AfxMessageBox("매칭 실패.", MB_SYSTEMMODAL);
			return;
		}

		double dDeltaX, dDeltaY, dAngle, dScore;
		dDeltaX = Column[0].D();
		dDeltaY = Row[0].D();
		dAngle = Angle[0].D();
		dScore = Score[0].D();

		HTuple HomMat2DRotate;
		vector_angle_to_rigid(dTeachAlignRefY, dTeachAlignRefX, 0, dDeltaY, dDeltaX, dAngle, &HomMat2DRotate);

		gen_empty_obj(&(THEAPP.m_pInspectAdminViewDlg->m_HParameterDefectXLD));

		Hobject HResultXLD;
		affine_trans_region(HInspectAreaRgn, &HResultXLD, HomMat2DRotate, "false");
		gen_contour_region_xld(HResultXLD, &HResultXLD, "border");

		if (THEAPP.m_pGFunction->ValidHXLD(HResultXLD))
			concat_obj(THEAPP.m_pInspectAdminViewDlg->m_HParameterDefectXLD, HResultXLD, &(THEAPP.m_pInspectAdminViewDlg->m_HParameterDefectXLD));

		THEAPP.m_pInspectAdminViewDlg->UpdateView();

		double dTransX, dTransY, dRotationAngle;
		dTransX = Column[0].D() - dTeachAlignRefX;
		dTransY = Row[0].D() - dTeachAlignRefY;
		tuple_deg(Angle, &Angle);
		dRotationAngle = Angle[0].D();

		CString sMsg;
		sMsg.Format("(X,Y)위치편차(픽셀)=(%d,%d), 회전각도(Deg)=(%.2lf), 매칭율(%%)=%d", (int)dTransX, (int)dTransY, dRotationAngle, (int)(dScore*100.0));
		AfxMessageBox(sMsg, MB_SYSTEMMODAL|MB_ICONINFORMATION);

		m_bCheckPartCheckModelExist = TRUE;
		UpdateData(FALSE);
	}
	catch (HException& except)
	{
		HTuple HExp;
		HTuple HOperatorName, HErrMsg;
		except.ToHTuple(&HExp);
		except.GetExceptionData(HExp, "operator", &HOperatorName);
		except.GetExceptionData(HExp, "error_message", &HErrMsg);

		CString str; 
		str.Format("Halcon Exception [CAlgorithmPartCheckDlg::OnBnClickedButtonCreateMatchingModel] : <%s>%s", HOperatorName[0].S(), HErrMsg[0].S());
		THEAPP.SaveLog(str);
	}
}


void CLocalAlignPartCheckDlg::OnBnClickedButtonTestMatchingModel()
{
	try
	{
		if (THEAPP.m_pGFunction->ValidHImage(THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[m_iSelectedImageType])==FALSE)
		{
			AfxMessageBox("티칭 영상이 존재하지 않습니다.", MB_SYSTEMMODAL);
			return;
		}

		if (m_pSelectedROI->m_HPartModelID<0)
		{
			AfxMessageBox("매칭 모델이 생성되지 않았습니다. 먼저 매칭 모델을 생성해 주세요.", MB_SYSTEMMODAL);
			return;
		}

		UpdateData();

		Hobject HInspectAreaRgn;
		HInspectAreaRgn = m_pSelectedROI->GetROIHRegion(THEAPP.m_pCalDataService);

		// Model Ref: G-Center of Domain
		double dTeachAlignRefX, dTeachAlignRefY;
		Hlong lArea;
		HTuple HlArea, HdTeachAlignRefY, HdTeachAlignRefX;
		area_center(HInspectAreaRgn, &HlArea, &HdTeachAlignRefY, &HdTeachAlignRefX);
		lArea = HlArea[0].L();
		dTeachAlignRefY = HdTeachAlignRefY[0].D();
		dTeachAlignRefX = HdTeachAlignRefX[0].D();

		// Test
		int iSearchMarginLeft = m_iEditPartCheckMatchingSearchMarginX;
		if (iSearchMarginLeft<0)
			iSearchMarginLeft = 0;
		int iSearchMarginRight = m_iEditPartCheckMatchingSearchMarginX2;
		if (iSearchMarginRight<0)
			iSearchMarginRight = 0;
		int iSearchMarginTop = m_iEditPartCheckMatchingSearchMarginY;
		if (iSearchMarginTop<0)
			iSearchMarginTop = 0;
		int iSearchMarginBottom = m_iEditPartCheckMatchingSearchMarginY2;
		if (iSearchMarginBottom<0)
			iSearchMarginBottom = 0;

		Hobject HInspectAreaDilatedRgn;
		shape_trans(HInspectAreaRgn, &HInspectAreaDilatedRgn, "rectangle1");

		Hlong lLTPointY, lLTPointX, lRBPointY, lRBPointX;
		HTuple HlLTPointY, HlLTPointX, HlRBPointY, HlRBPointX;
		smallest_rectangle1(HInspectAreaDilatedRgn, &HlLTPointY, &HlLTPointX, &HlRBPointY, &HlRBPointX);
		lLTPointY = HlLTPointY[0].L();
		lLTPointX = HlLTPointX[0].L();
		lRBPointY = HlRBPointY[0].L();
		lRBPointX = HlRBPointX[0].L();

		gen_rectangle1(&HInspectAreaDilatedRgn, lLTPointY-iSearchMarginTop, lLTPointX-iSearchMarginLeft, lRBPointY+iSearchMarginBottom, lRBPointX+iSearchMarginRight);

		Hobject HMatchingImageReduced;
		reduce_domain(THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[m_iSelectedImageType], HInspectAreaDilatedRgn, &HMatchingImageReduced);

		HTuple dAngleRangeRad;
		tuple_rad(m_iEditPartCheckMatchingAngleRange, &dAngleRangeRad);

		if (dAngleRangeRad<0)
			dAngleRangeRad = dAngleRangeRad*-1.0;

		Hlong lNoFoundNumber;
		HTuple Row, Column, Angle, Score;
		//double dMatchingScore;
		//dMatchingScore = (double)m_iEditPartCheckMatchingScore * 0.01;

		Row = HTuple(0.0);
		Column = HTuple(0.0);
		Angle = HTuple(0.0);
		Score = HTuple(0.0);

		find_ncc_model(HMatchingImageReduced,
			m_pSelectedROI->m_HPartModelID,				// Handle of the model
			-dAngleRangeRad,							// Smallest rotation of the model
			dAngleRangeRad*2.0,							// Extent of the rotation angles.
			0.1,										// Minumum score of the instances of the model to be found
			1,											// Number of instances of the model to be found
			0.5,										// Maximum overlap of the instances of the model to be found
			"true",										// Subpixel accuracy
			0,											// Number of pyramid levels used in the matching
			&Row,										// Row coordinate of the found instances of the model.
			&Column,									// Column coordinate of the found instances of the model.
			&Angle,										// Rotation angle of the found instances of the model. (radian)
			&Score);									// Score of the found instances of the model.

		HTuple HlNoFoundNumber;
		tuple_length(Score, &HlNoFoundNumber);
		lNoFoundNumber = HlNoFoundNumber[0].L();

		if (lNoFoundNumber<=0)
		{
			AfxMessageBox("매칭 실패", MB_SYSTEMMODAL);
			return;
		}

		double dDeltaX, dDeltaY, dAngle, dScore;
		dDeltaX = Column[0].D();
		dDeltaY = Row[0].D();
		dAngle = Angle[0].D();
		dScore = Score[0].D();

		HTuple HomMat2DRotate;
		vector_angle_to_rigid(dTeachAlignRefY, dTeachAlignRefX, 0, dDeltaY, dDeltaX, dAngle, &HomMat2DRotate);

		gen_empty_obj(&(THEAPP.m_pInspectAdminViewDlg->m_HParameterDefectXLD));

		Hobject HResultXLD;
		affine_trans_region(HInspectAreaRgn, &HResultXLD, HomMat2DRotate, "false");
		gen_contour_region_xld(HResultXLD, &HResultXLD, "border");

		if (THEAPP.m_pGFunction->ValidHXLD(HResultXLD))
			concat_obj(THEAPP.m_pInspectAdminViewDlg->m_HParameterDefectXLD, HResultXLD, &(THEAPP.m_pInspectAdminViewDlg->m_HParameterDefectXLD));

		THEAPP.m_pInspectAdminViewDlg->UpdateView();

		double dTransX, dTransY, dRotationAngle;
		dTransX = Column[0].D() - dTeachAlignRefX;
		dTransY = Row[0].D() - dTeachAlignRefY;
		tuple_deg(Angle, &Angle);
		dRotationAngle = Angle[0].D();

		CString sMsg;
		sMsg.Format("(X,Y)위치편차(픽셀)=(%d,%d), 회전각도(Deg)=(%.2lf), 매칭율(%%)=%d", (int)dTransX, (int)dTransY, dRotationAngle, (int)(dScore*100.0));
		AfxMessageBox(sMsg, MB_SYSTEMMODAL|MB_ICONINFORMATION);

	}
	catch (HException& except)
	{
		HTuple HExp;
		HTuple HOperatorName, HErrMsg;
		except.ToHTuple(&HExp);
		except.GetExceptionData(HExp, "operator", &HOperatorName);
		except.GetExceptionData(HExp, "error_message", &HErrMsg);

		CString str; 
		str.Format("Halcon Exception [CAlgorithmPartCheckDlg::OnBnClickedButtonTestMatchingModel] : <%s>%s", HOperatorName[0].S(), HErrMsg[0].S());
		THEAPP.SaveLog(str);
	}
}

void CLocalAlignPartCheckDlg::OnBnClickedButtonDeleteMatchingModel()
{
	if(AfxMessageBox("현재 매칭 모델을 삭제하시겠습니까?", MB_OKCANCEL) == IDOK)
	{
		if (m_pSelectedROI->m_HPartModelID>=0)
		{
			clear_ncc_model(m_pSelectedROI->m_HPartModelID);
			m_pSelectedROI->m_HPartModelID = -1;

			m_bCheckPartCheckModelExist = FALSE;
			UpdateData(FALSE);
		}
	}
}