// InspectionParameterExtraDlg.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "uScan.h"
#include "InspectionParameterExtraDlg.h"
#include "afxdialogex.h"


// CInspectionParameterExtraDlg 대화 상자입니다.

IMPLEMENT_DYNAMIC(CInspectionParameterExtraDlg, CDialog)

CInspectionParameterExtraDlg::CInspectionParameterExtraDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CInspectionParameterExtraDlg::IDD, pParent)
{
	m_iTopBarrelInner_RoiOuterMargin = 0;
	m_iTopBarrelInner_RoiInnerMargin = 0;

	m_bCheckInspectBarcodeShift = FALSE;
	m_dEditBarcodeShiftRefX = 0.0;
	m_dEditBarcodeShiftRefY = 0.0;
	m_dEditBarcodeShiftToleranceX = 0.0;
	m_dEditBarcodeShiftToleranceY = 0.0;
	m_dEditBlackCoatingRef = 0.0;
	m_dEditBlackCoatingTolerance = 0.0;
	m_iEditBlackCoatingMaxGray = 0;
	m_iComboBlackCoatingContour = 0;
	m_iComboBlackCoatingImage = 0;
	m_iComboDatumContour = 0;
	m_iComboDatumImage = 0;
	m_iEditBlackCoatingInnerMargin = 0;
	m_iEditBlackCoatingMinGray = 0;
	m_iEditBlackCoatingOuterMargin = 0;
	m_iEditDatumInnerMargin = 0;
	m_iEditDatumMaxGray = 0;
	m_iEditDatumOuterMargin = 0;
	m_iEditDatumMinGray = 0;
	m_dEditBlackCoatingOffset = 0.0;
	m_iRadioInspectionType = 0;
	m_iEditBlackCoatingDiffGray = 0;
	m_iEditBlackCoatingHeight = 0;
	m_iEditBlackCoatingWidth = 0;
	m_iEditDatumDiffGray = 0;
	m_iEditDatumHeight = 0;
	m_iEditDatumWidth = 0;
	m_iRadioBlackCoatingTransition = 0;
	m_iRadioDatumTransition = 0;

	//Barcode Rotation Inspection
	m_bCheckInspectBarcodeRotation = FALSE;
	m_dEditBarcodeShiftRefRotation = 0.0;
	m_dEditBarcodeShiftToleranceRotation = 3.0;
}

CInspectionParameterExtraDlg::~CInspectionParameterExtraDlg()
{
}

void CInspectionParameterExtraDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_TOPBARREL_INNER_ROI_OUTER_MARGIN, m_iTopBarrelInner_RoiOuterMargin);
	DDX_Text(pDX, IDC_EDIT_TOPBARREL_INNER_ROI_INNER_MARGIN, m_iTopBarrelInner_RoiInnerMargin);
	DDX_Check(pDX, IDC_CHECK_INSPECT_BARCODE_SHIFT, m_bCheckInspectBarcodeShift);
	DDX_Text(pDX, IDC_EDIT_BARCODE_SHIFT_REF_X, m_dEditBarcodeShiftRefX);
	DDX_Text(pDX, IDC_EDIT_BARCODE_SHIFT_REF_Y, m_dEditBarcodeShiftRefY);
	DDX_Text(pDX, IDC_EDIT_BARCODE_SHIFT_TOLERANCE_X, m_dEditBarcodeShiftToleranceX);
	DDX_Text(pDX, IDC_EDIT_BARCODE_SHIFT_TOLERANCE_Y, m_dEditBarcodeShiftToleranceY);
	DDX_Check(pDX, IDC_CHECK_INSPECT_DIAMETER, m_bCheckInspectBlackCoatingDiameter);
	DDX_Text(pDX, IDC_EDIT_BLACKCOATING_DIAMETER, m_dEditBlackCoatingRef);
	DDX_Text(pDX, IDC_EDIT_BLACKCOATING_TOLERANCE, m_dEditBlackCoatingTolerance);
	DDX_Text(pDX, IDC_EDIT_BLACKCOATING_MAX_GRAY, m_iEditBlackCoatingMaxGray);
	DDX_CBIndex(pDX, IDC_COMBO_BLACKCOATING_CONTOUR, m_iComboBlackCoatingContour);
	DDX_CBIndex(pDX, IDC_COMBO_BLACKCOATING_IMAGE, m_iComboBlackCoatingImage);
	DDX_CBIndex(pDX, IDC_COMBO_DATUM_CONTOUR, m_iComboDatumContour);
	DDX_CBIndex(pDX, IDC_COMBO_DATUM_IMAGE, m_iComboDatumImage);
	DDX_Text(pDX, IDC_EDIT_BLACKCOATING_INNER_MARGIN, m_iEditBlackCoatingInnerMargin);
	DDX_Text(pDX, IDC_EDIT_BLACKCOATING_MIM_GRAY, m_iEditBlackCoatingMinGray);
	DDX_Text(pDX, IDC_EDIT_BLACKCOATING_OUTER_MARGIN, m_iEditBlackCoatingOuterMargin);
	DDX_Text(pDX, IDC_EDIT_DATUM_INNER_MARGIN, m_iEditDatumInnerMargin);
	DDX_Text(pDX, IDC_EDIT_DATUM_MAX_GRAY, m_iEditDatumMaxGray);
	DDX_Text(pDX, IDC_EDIT_DATUM_OUTER_MARGIN, m_iEditDatumOuterMargin);
	DDX_Text(pDX, IDC_EDIT_DATUM_MIM_GRAY, m_iEditDatumMinGray);
	DDX_Text(pDX, IDC_EDIT_BLACKCOATING_DIAMETER_OFFSET, m_dEditBlackCoatingOffset);
	DDX_Check(pDX, IDC_CHECK_USE_IMAGE_SCALING_BC, m_bCheckUseImageScaling_BC);
	DDX_Check(pDX, IDC_CHECK_USE_IMAGE_SCALING_DT, m_bCheckUseImageScaling_DT);
	DDX_Radio(pDX, IDC_RADIO_IMAGE_SCALE_AUTO_BC, m_iRadioImageScaleMethod_BC);
	DDX_Radio(pDX, IDC_RADIO_IMAGE_SCALE_AUTO_DT, m_iRadioImageScaleMethod_DT);
	DDX_Text(pDX, IDC_EDIT_IMAGE_SCAN_MAX_BC, m_iEditImageScaleMax_BC);
	DDX_Text(pDX, IDC_EDIT_IMAGE_SCAN_MAX_DT, m_iEditImageScaleMax_DT);
	DDX_Text(pDX, IDC_EDIT_IMAGE_SCAN_MIN_BC, m_iEditImageScaleMin_BC);
	DDX_Text(pDX, IDC_EDIT_IMAGE_SCAN_MIN_DT, m_iEditImageScaleMin_DT);
	DDX_Radio(pDX, IDC_RADIO_INSPTYPE_GRAY, m_iRadioInspectionType);
	DDX_Text(pDX, IDC_EDIT_BLACKCOATING_DIFF_GRAY, m_iEditBlackCoatingDiffGray);
	DDX_Text(pDX, IDC_EDIT_BLACKCOATING_HEIGHT, m_iEditBlackCoatingHeight);
	DDX_Text(pDX, IDC_EDIT_BLACKCOATING_WIDTH, m_iEditBlackCoatingWidth);
	DDX_Text(pDX, IDC_EDIT_DATUM_DIFF_GRAY, m_iEditDatumDiffGray);
	DDX_Text(pDX, IDC_EDIT_DATUM_HEIGHT, m_iEditDatumHeight);
	DDX_Text(pDX, IDC_EDIT_DATUM_WIDTH, m_iEditDatumWidth);
	DDX_Radio(pDX, IDC_RADIO_BLACKCOATING_POSITIVE, m_iRadioBlackCoatingTransition);
	DDX_Radio(pDX, IDC_RADIO_DATUM_POSITIVE, m_iRadioDatumTransition);

	//Barcode Rotation Inspection
	DDX_Check(pDX, IDC_CHECK_INSPECT_BARCODE_ROTATION, m_bCheckInspectBarcodeRotation);
	DDX_Text(pDX, IDC_EDIT_BARCODE_SHIFT_REF_ROTATION, m_dEditBarcodeShiftRefRotation);
	DDX_Text(pDX, IDC_EDIT_BARCODE_SHIFT_TOLERANCE_ROTATION, m_dEditBarcodeShiftToleranceRotation);
}


BEGIN_MESSAGE_MAP(CInspectionParameterExtraDlg, CDialog)
	ON_BN_CLICKED(IDC_BN_OK, &CInspectionParameterExtraDlg::OnBnClickedBnOk)
	ON_BN_CLICKED(IDC_BN_CANCLE, &CInspectionParameterExtraDlg::OnBnClickedBnCancle)
	ON_BN_CLICKED(IDC_BUTTON_SHOW_AREA, &CInspectionParameterExtraDlg::OnBnClickedButtonShowArea)
	ON_BN_CLICKED(IDC_BUTTON_SHOW_DIAMETER, &CInspectionParameterExtraDlg::OnBnClickedButtonShowDiameter)
	ON_BN_CLICKED(IDC_BUTTON_DISPLAY_SCALED_IMAGE_BC, &CInspectionParameterExtraDlg::OnBnClickedButtonDisplayScaledImageBc)
	ON_BN_CLICKED(IDC_BUTTON_DISPLAY_SCALED_IMAGE_DT, &CInspectionParameterExtraDlg::OnBnClickedButtonDisplayScaledImageDt)
END_MESSAGE_MAP()


// CInspectionParameterExtraDlg 메시지 처리기입니다.


void CInspectionParameterExtraDlg::OnBnClickedBnOk()
{
	UpdateData(TRUE);
	GetInspectParam();

	//////////////////////////////////////////////////////////////////////////
	// Save Shift Parameter
	CString strModelFolder;
	if(!THEAPP.Struct_PreferenceStruct.m_bUseAbsolutePathModel) //Ver2629
		strModelFolder = THEAPP.GetWorkingDirectory()+"\\Model\\"+THEAPP.m_pModelDataManager->m_sModelName;
	else
		strModelFolder = THEAPP.GetWorkingEvmsDirectory()+"\\Model\\"+THEAPP.m_pModelDataManager->m_sModelName;

	CIniFileCS INI(strModelFolder+"\\SW\\ModelBase.ini");
	CString strSection;

	strSection = "Barcode Shift";
	INI.Set_Bool(strSection, "m_bInspectBarcodeShift", THEAPP.m_pModelDataManager->m_bInspectBarcodeShift);
	INI.Set_Double(strSection, "m_dBarcodeShiftRefX", THEAPP.m_pModelDataManager->m_dBarcodeShiftRefX);
	INI.Set_Double(strSection, "m_dBarcodeShiftRefY", THEAPP.m_pModelDataManager->m_dBarcodeShiftRefY);
	INI.Set_Double(strSection, "m_dBarcodeShiftToleranceX", THEAPP.m_pModelDataManager->m_dBarcodeShiftToleranceX);
	INI.Set_Double(strSection, "m_dBarcodeShiftToleranceY", THEAPP.m_pModelDataManager->m_dBarcodeShiftToleranceY);

	//Save //Barcode Rotation Inspection
	strSection = "Barcode Rotation";
	INI.Set_Bool(strSection, "m_bInspectBarcodeRotation", THEAPP.m_pModelDataManager->m_bInspectBarcodeRotation);
	INI.Set_Double(strSection, "m_dBarcodeShiftToleranceRotation", THEAPP.m_pModelDataManager->m_dBarcodeShiftToleranceRotation);
	INI.Set_Double(strSection, "m_dBarcodeShiftRefRotation", THEAPP.m_pModelDataManager->m_dBarcodeShiftRefRotation);
	//////////////////////////////////////////////////////////////////////////

	strSection = "Circle Diameter";
	INI.Set_Bool(strSection, "m_bInspectBlackCoatingDiameter", THEAPP.m_pModelDataManager->m_bInspectBlackCoatingDiameter);
	INI.Set_Double(strSection, "m_dBlackCoatingRef", THEAPP.m_pModelDataManager->m_dBlackCoatingRef);
	INI.Set_Double(strSection, "m_dBlackCoatingTolerance", THEAPP.m_pModelDataManager->m_dBlackCoatingTolerance);
	INI.Set_Double(strSection, "m_dBlackCoatingOffset", THEAPP.m_pModelDataManager->m_dBlackCoatingOffset);

	INI.Set_Integer(strSection, "m_iBlackCoatingImage", THEAPP.m_pModelDataManager->m_iBlackCoatingImage);
	INI.Set_Integer(strSection, "m_iBlackCoatingContour", THEAPP.m_pModelDataManager->m_iBlackCoatingContour);
	INI.Set_Integer(strSection, "m_iBlackCoatingOuterMargin", THEAPP.m_pModelDataManager->m_iBlackCoatingOuterMargin);
	INI.Set_Integer(strSection, "m_iBlackCoatingInnerMargin", THEAPP.m_pModelDataManager->m_iBlackCoatingInnerMargin);
	INI.Set_Integer(strSection, "m_iBlackCoatingMinGray", THEAPP.m_pModelDataManager->m_iBlackCoatingMinGray);
	INI.Set_Integer(strSection, "m_iBlackCoatingMaxGray", THEAPP.m_pModelDataManager->m_iBlackCoatingMaxGray);

	INI.Set_Integer(strSection, "m_iDatumImage", THEAPP.m_pModelDataManager->m_iDatumImage);
	INI.Set_Integer(strSection, "m_iDatumContour", THEAPP.m_pModelDataManager->m_iDatumContour);
	INI.Set_Integer(strSection, "m_iDatumOuterMargin", THEAPP.m_pModelDataManager->m_iDatumOuterMargin);
	INI.Set_Integer(strSection, "m_iDatumInnerMargin", THEAPP.m_pModelDataManager->m_iDatumInnerMargin);
	INI.Set_Integer(strSection, "m_iDatumMinGray", THEAPP.m_pModelDataManager->m_iDatumMinGray);
	INI.Set_Integer(strSection, "m_iDatumMaxGray", THEAPP.m_pModelDataManager->m_iDatumMaxGray);

	INI.Set_Bool(strSection, "m_bUseImageScaling_BC", THEAPP.m_pModelDataManager->m_bUseImageScaling_BC);
	INI.Set_Bool(strSection, "m_bUseImageScaling_DT", THEAPP.m_pModelDataManager->m_bUseImageScaling_DT);
	INI.Set_Integer(strSection, "m_iImageScaleMethod_BC", THEAPP.m_pModelDataManager->m_iImageScaleMethod_BC);
	INI.Set_Integer(strSection, "m_iImageScaleMethod_DT", THEAPP.m_pModelDataManager->m_iImageScaleMethod_DT);
	INI.Set_Integer(strSection, "m_iImageScaleMax_BC", THEAPP.m_pModelDataManager->m_iImageScaleMax_BC);
	INI.Set_Integer(strSection, "m_iImageScaleMax_DT", THEAPP.m_pModelDataManager->m_iImageScaleMax_DT);
	INI.Set_Integer(strSection, "m_iImageScaleMin_BC", THEAPP.m_pModelDataManager->m_iImageScaleMin_BC);
	INI.Set_Integer(strSection, "m_iImageScaleMin_DT", THEAPP.m_pModelDataManager->m_iImageScaleMin_DT);
	INI.Set_Integer(strSection, "m_iInspectionType", THEAPP.m_pModelDataManager->m_iInspectionType);

	INI.Set_Integer(strSection, "m_iBlackCoatingWidth", THEAPP.m_pModelDataManager->m_iBlackCoatingWidth);
	INI.Set_Integer(strSection, "m_iBlackCoatingHeight", THEAPP.m_pModelDataManager->m_iBlackCoatingHeight);
	INI.Set_Integer(strSection, "m_iBlackCoatingDiffGray", THEAPP.m_pModelDataManager->m_iBlackCoatingDiffGray);
	INI.Set_Integer(strSection, "m_iBlackCoatingTransition", THEAPP.m_pModelDataManager->m_iBlackCoatingTransition);
	INI.Set_Integer(strSection, "m_iDatumWidth", THEAPP.m_pModelDataManager->m_iDatumWidth);
	INI.Set_Integer(strSection, "m_iDatumHeight", THEAPP.m_pModelDataManager->m_iDatumHeight);
	INI.Set_Integer(strSection, "m_iDatumDiffGray", THEAPP.m_pModelDataManager->m_iDatumDiffGray);
	INI.Set_Integer(strSection, "m_iDatumTransition", THEAPP.m_pModelDataManager->m_iDatumTransition);

	THEAPP.m_pInspectAdminViewDlg->m_HInspectAreaRgn.Reset();
	THEAPP.m_pInspectAdminViewDlg->m_HBlackCoatingCircleXLD.Reset();
	THEAPP.m_pInspectAdminViewDlg->m_HDatumCircleXLD.Reset();
	THEAPP.m_pInspectAdminViewDlg->m_HDiamSBLineXLD.Reset();

	THEAPP.m_pInspectAdminViewDlg->m_HBlackCoatingEdgePointRgn.Reset();
	THEAPP.m_pInspectAdminViewDlg->m_HDatumEdgePointRgn.Reset();

	if( THEAPP.m_pGFunction->ValidHImage(THEAPP.m_pInspectAdminViewDlg->m_pHImage) )
		copy_image(THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[THEAPP.m_pModelDataManager->m_iBlackMatchingImageType], &THEAPP.m_pInspectAdminViewDlg->m_pHImage);

	THEAPP.m_pInspectAdminViewDlg->UpdateView();

	CDialog::OnOK();
}

void CInspectionParameterExtraDlg::OnBnClickedBnCancle()
{
	THEAPP.m_pInspectAdminViewDlg->m_HInspectAreaRgn.Reset();
	THEAPP.m_pInspectAdminViewDlg->m_HBlackCoatingCircleXLD.Reset();
	THEAPP.m_pInspectAdminViewDlg->m_HDatumCircleXLD.Reset();
	THEAPP.m_pInspectAdminViewDlg->m_HDiamSBLineXLD.Reset();

	THEAPP.m_pInspectAdminViewDlg->m_HBlackCoatingEdgePointRgn.Reset();
	THEAPP.m_pInspectAdminViewDlg->m_HDatumEdgePointRgn.Reset();

	//THEAPP.m_pModelDataManager->m_iBlackMatchingImageType
	if( THEAPP.m_pGFunction->ValidHImage(THEAPP.m_pInspectAdminViewDlg->m_pHImage) )
		copy_image(THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[THEAPP.m_pModelDataManager->m_iBlackMatchingImageType], &THEAPP.m_pInspectAdminViewDlg->m_pHImage);

	THEAPP.m_pInspectAdminViewDlg->UpdateView();

	CDialog::OnCancel();
}

void CInspectionParameterExtraDlg::GetInspectParam()
{
	THEAPP.m_pModelDataManager->m_iDecisonTopBarrelInner_RoiOuterMargin = m_iTopBarrelInner_RoiOuterMargin;
	THEAPP.m_pModelDataManager->m_iDecisonTopBarrelInner_RoiInnerMargin = m_iTopBarrelInner_RoiInnerMargin;

	THEAPP.m_pModelDataManager->m_bInspectBarcodeShift = m_bCheckInspectBarcodeShift;
	THEAPP.m_pModelDataManager->m_dBarcodeShiftRefX = m_dEditBarcodeShiftRefX;
	THEAPP.m_pModelDataManager->m_dBarcodeShiftRefY = m_dEditBarcodeShiftRefY;
	THEAPP.m_pModelDataManager->m_dBarcodeShiftToleranceX = m_dEditBarcodeShiftToleranceX;
	THEAPP.m_pModelDataManager->m_dBarcodeShiftToleranceY = m_dEditBarcodeShiftToleranceY;

	//Barcode Rotation Inspection
	THEAPP.m_pModelDataManager->m_bInspectBarcodeRotation = m_bCheckInspectBarcodeRotation;
	THEAPP.m_pModelDataManager->m_dBarcodeShiftRefRotation = m_dEditBarcodeShiftRefRotation;
	THEAPP.m_pModelDataManager->m_dBarcodeShiftToleranceRotation = m_dEditBarcodeShiftToleranceRotation;

	THEAPP.m_pModelDataManager->m_bInspectBlackCoatingDiameter = m_bCheckInspectBlackCoatingDiameter;
	THEAPP.m_pModelDataManager->m_dBlackCoatingRef = m_dEditBlackCoatingRef;
	THEAPP.m_pModelDataManager->m_dBlackCoatingTolerance = m_dEditBlackCoatingTolerance;
	THEAPP.m_pModelDataManager->m_dBlackCoatingOffset = m_dEditBlackCoatingOffset;

	THEAPP.m_pModelDataManager->m_iBlackCoatingImage = m_iComboBlackCoatingImage;
	THEAPP.m_pModelDataManager->m_iBlackCoatingContour = m_iComboBlackCoatingContour;
	THEAPP.m_pModelDataManager->m_iBlackCoatingOuterMargin = m_iEditBlackCoatingOuterMargin;
	THEAPP.m_pModelDataManager->m_iBlackCoatingInnerMargin = m_iEditBlackCoatingInnerMargin;
	THEAPP.m_pModelDataManager->m_iBlackCoatingMinGray = m_iEditBlackCoatingMinGray;
	THEAPP.m_pModelDataManager->m_iBlackCoatingMaxGray = m_iEditBlackCoatingMaxGray;

	THEAPP.m_pModelDataManager->m_iDatumImage = m_iComboDatumImage;
	THEAPP.m_pModelDataManager->m_iDatumContour = m_iComboDatumContour;
	THEAPP.m_pModelDataManager->m_iDatumOuterMargin = m_iEditDatumOuterMargin;
	THEAPP.m_pModelDataManager->m_iDatumInnerMargin = m_iEditDatumInnerMargin;
	THEAPP.m_pModelDataManager->m_iDatumMinGray = m_iEditDatumMinGray;
	THEAPP.m_pModelDataManager->m_iDatumMaxGray = m_iEditDatumMaxGray;

	THEAPP.m_pModelDataManager->m_bUseImageScaling_BC = m_bCheckUseImageScaling_BC;
	THEAPP.m_pModelDataManager->m_bUseImageScaling_DT = m_bCheckUseImageScaling_DT;
	THEAPP.m_pModelDataManager->m_iImageScaleMethod_BC = m_iRadioImageScaleMethod_BC;
	THEAPP.m_pModelDataManager->m_iImageScaleMethod_DT = m_iRadioImageScaleMethod_DT;
	THEAPP.m_pModelDataManager->m_iImageScaleMax_BC = m_iEditImageScaleMax_BC;
	THEAPP.m_pModelDataManager->m_iImageScaleMax_DT = m_iEditImageScaleMax_DT;
	THEAPP.m_pModelDataManager->m_iImageScaleMin_BC = m_iEditImageScaleMin_BC;
	THEAPP.m_pModelDataManager->m_iImageScaleMin_DT = m_iEditImageScaleMin_DT;

	THEAPP.m_pModelDataManager->m_iInspectionType = m_iRadioInspectionType;

	THEAPP.m_pModelDataManager->m_iBlackCoatingWidth = m_iEditBlackCoatingWidth;
	THEAPP.m_pModelDataManager->m_iBlackCoatingHeight = m_iEditBlackCoatingHeight;
	THEAPP.m_pModelDataManager->m_iBlackCoatingDiffGray = m_iEditBlackCoatingDiffGray;
	THEAPP.m_pModelDataManager->m_iBlackCoatingTransition = m_iRadioBlackCoatingTransition;
	THEAPP.m_pModelDataManager->m_iDatumWidth = m_iEditDatumWidth;
	THEAPP.m_pModelDataManager->m_iDatumHeight = m_iEditDatumHeight;
	THEAPP.m_pModelDataManager->m_iDatumDiffGray = m_iEditDatumDiffGray;
	THEAPP.m_pModelDataManager->m_iDatumTransition = m_iRadioDatumTransition;
}

void CInspectionParameterExtraDlg::SetInspectParam()
{
	m_iTopBarrelInner_RoiOuterMargin = THEAPP.m_pModelDataManager->m_iDecisonTopBarrelInner_RoiOuterMargin;
	m_iTopBarrelInner_RoiInnerMargin = THEAPP.m_pModelDataManager->m_iDecisonTopBarrelInner_RoiInnerMargin;

	m_bCheckInspectBarcodeShift = THEAPP.m_pModelDataManager->m_bInspectBarcodeShift;
	m_dEditBarcodeShiftRefX = THEAPP.m_pModelDataManager->m_dBarcodeShiftRefX;
	m_dEditBarcodeShiftRefY = THEAPP.m_pModelDataManager->m_dBarcodeShiftRefY;
	m_dEditBarcodeShiftToleranceX = THEAPP.m_pModelDataManager->m_dBarcodeShiftToleranceX;
	m_dEditBarcodeShiftToleranceY = THEAPP.m_pModelDataManager->m_dBarcodeShiftToleranceY;

	//Barcode Rotation Inspection
	m_bCheckInspectBarcodeRotation = THEAPP.m_pModelDataManager->m_bInspectBarcodeRotation;
	m_dEditBarcodeShiftToleranceRotation = THEAPP.m_pModelDataManager->m_dBarcodeShiftToleranceRotation;
	m_dEditBarcodeShiftRefRotation = THEAPP.m_pModelDataManager->m_dBarcodeShiftRefRotation;

	m_bCheckInspectBlackCoatingDiameter = THEAPP.m_pModelDataManager->m_bInspectBlackCoatingDiameter;
	m_dEditBlackCoatingRef = THEAPP.m_pModelDataManager->m_dBlackCoatingRef;
	m_dEditBlackCoatingTolerance = THEAPP.m_pModelDataManager->m_dBlackCoatingTolerance;
	m_dEditBlackCoatingOffset = THEAPP.m_pModelDataManager->m_dBlackCoatingOffset;

	m_iComboBlackCoatingImage = THEAPP.m_pModelDataManager->m_iBlackCoatingImage;
	m_iComboBlackCoatingContour = THEAPP.m_pModelDataManager->m_iBlackCoatingContour;
	m_iEditBlackCoatingOuterMargin = THEAPP.m_pModelDataManager->m_iBlackCoatingOuterMargin;
	m_iEditBlackCoatingInnerMargin = THEAPP.m_pModelDataManager->m_iBlackCoatingInnerMargin;
	m_iEditBlackCoatingMinGray = THEAPP.m_pModelDataManager->m_iBlackCoatingMinGray;
	m_iEditBlackCoatingMaxGray = THEAPP.m_pModelDataManager->m_iBlackCoatingMaxGray;

	m_iComboDatumImage = THEAPP.m_pModelDataManager->m_iDatumImage;
	m_iComboDatumContour = THEAPP.m_pModelDataManager->m_iDatumContour;
	m_iEditDatumOuterMargin = THEAPP.m_pModelDataManager->m_iDatumOuterMargin;
	m_iEditDatumInnerMargin = THEAPP.m_pModelDataManager->m_iDatumInnerMargin;
	m_iEditDatumMinGray = THEAPP.m_pModelDataManager->m_iDatumMinGray;
	m_iEditDatumMaxGray = THEAPP.m_pModelDataManager->m_iDatumMaxGray;

	m_bCheckUseImageScaling_BC = THEAPP.m_pModelDataManager->m_bUseImageScaling_BC;
	m_bCheckUseImageScaling_DT = THEAPP.m_pModelDataManager->m_bUseImageScaling_DT;
	m_iRadioImageScaleMethod_BC = THEAPP.m_pModelDataManager->m_iImageScaleMethod_BC;
	m_iRadioImageScaleMethod_DT = THEAPP.m_pModelDataManager->m_iImageScaleMethod_DT;
	m_iEditImageScaleMax_BC = THEAPP.m_pModelDataManager->m_iImageScaleMax_BC;
	m_iEditImageScaleMax_DT = THEAPP.m_pModelDataManager->m_iImageScaleMax_DT;
	m_iEditImageScaleMin_BC = THEAPP.m_pModelDataManager->m_iImageScaleMin_BC;
	m_iEditImageScaleMin_DT = THEAPP.m_pModelDataManager->m_iImageScaleMin_DT;
	m_iRadioInspectionType = THEAPP.m_pModelDataManager->m_iInspectionType;

	m_iEditBlackCoatingWidth = THEAPP.m_pModelDataManager->m_iBlackCoatingWidth;
	m_iEditBlackCoatingHeight = THEAPP.m_pModelDataManager->m_iBlackCoatingHeight;
	m_iEditBlackCoatingDiffGray = THEAPP.m_pModelDataManager->m_iBlackCoatingDiffGray;
	m_iRadioBlackCoatingTransition = THEAPP.m_pModelDataManager->m_iBlackCoatingTransition;
	m_iEditDatumWidth = THEAPP.m_pModelDataManager->m_iDatumWidth;
	m_iEditDatumHeight = THEAPP.m_pModelDataManager->m_iDatumHeight;
	m_iEditDatumDiffGray = THEAPP.m_pModelDataManager->m_iDatumDiffGray ;
	m_iRadioDatumTransition = THEAPP.m_pModelDataManager->m_iDatumTransition;
}

BOOL CInspectionParameterExtraDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	SetInspectParam();
	UpdateData(FALSE);

	return TRUE;
}


void CInspectionParameterExtraDlg::OnBnClickedButtonShowArea()
{
	if(THEAPP.m_pModelDataManager->m_sModelName == ".") return;

	if (THEAPP.m_pGFunction->ValidHXLD(THEAPP.m_pInspectAdminViewDlg->ContoursAffinTrans[MATCHING_CONTOUR_4])==FALSE)
	{
		AfxMessageBox("4번 기준선이 존재하지 않습니다.", MB_SYSTEMMODAL|MB_ICONINFORMATION);
		return;
	}

	UpdateData(TRUE);

	THEAPP.m_pAlgorithm->GetInspectArea_Inner (THEAPP.m_pInspectAdminViewDlg->ContoursAffinTrans[MATCHING_CONTOUR_4],
		m_iTopBarrelInner_RoiInnerMargin, m_iTopBarrelInner_RoiOuterMargin, &(THEAPP.m_pInspectAdminViewDlg->m_HInspectAreaRgn));

	THEAPP.m_pInspectAdminViewDlg->UpdateView();

}


void CInspectionParameterExtraDlg::OnBnClickedButtonShowDiameter()
{
	UpdateData(TRUE);

	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	THEAPP.m_pInspectAdminViewDlg->m_HBlackCoatingCircleXLD.Reset();
	THEAPP.m_pInspectAdminViewDlg->m_HDatumCircleXLD.Reset();
	THEAPP.m_pInspectAdminViewDlg->m_HDiamSBLineXLD.Reset();

	THEAPP.m_pInspectAdminViewDlg->m_HBlackCoatingEdgePointRgn.Reset();
	THEAPP.m_pInspectAdminViewDlg->m_HDatumEdgePointRgn.Reset();

	Hobject HBlackCoatingContour, HDatumContour;
	Hobject HBlackCoatingImage, HDatumImage;

	copy_obj(THEAPP.m_pInspectAdminViewDlg->ContoursAffinTrans[m_iComboBlackCoatingContour], &HBlackCoatingContour, 1, 1);
	copy_image(THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[m_iComboBlackCoatingImage], &HBlackCoatingImage);

	copy_obj(THEAPP.m_pInspectAdminViewDlg->ContoursAffinTrans[m_iComboDatumContour], &HDatumContour, 1, 1);
	copy_image(THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[m_iComboDatumImage], &HDatumImage);

	if (THEAPP.m_pGFunction->ValidHXLD(HBlackCoatingContour) && THEAPP.m_pGFunction->ValidHXLD(HDatumContour))
	{
		Hobject HBlackCoatingCircleRgn, HBlackCoatingReduceImage, HBlackCoatingCircleXLD;
		Hobject HDatumCircleRgn, HDatumReduceImage, HDatumCircleXLD, HDiamSBLineRgn;
		HTuple  ARow, AColumn, APhi, ARadius1, ARadius2, AStartPhi, AEndPhi, APointOrder;
		HTuple Row, Column, Radius, StartPhi, EndPhi, PointOrder;
		double dBlackCoatingCenterRow = 0.0, dBlackCoatingCenterCol = 0.0, dBlackCoatingOuterDiameter = 0.0;
		double dDatumCenterRow = 0.0, dDatumCenterCol = 0.0, dDatumOuterDiameter = 0.0;
		double dCenterRow, dCenterCol;

		/////////////////////////////////////////////////////////////////////////////////////////////////////////////
		///Black Coating Circle 찾기
		/////////////////////////////////////////////////////////////////////////////////////////////////////////////
		THEAPP.m_pAlgorithm->GetCircleFitInspectArea(HBlackCoatingContour, m_iEditBlackCoatingOuterMargin, m_iEditBlackCoatingInnerMargin, &HBlackCoatingCircleRgn);

		if(m_bCheckUseImageScaling_BC==TRUE)
		{
			Hobject HScaleImage;
			THEAPP.m_pAlgorithm->GetScaledImage(HBlackCoatingImage, HBlackCoatingCircleRgn, m_iRadioImageScaleMethod_BC, m_iEditImageScaleMax_BC, m_iEditImageScaleMin_BC, &HScaleImage);
			reduce_domain(HScaleImage, HBlackCoatingCircleRgn, &HBlackCoatingReduceImage);
		}
		else
		{
			reduce_domain(HBlackCoatingImage, HBlackCoatingCircleRgn, &HBlackCoatingReduceImage);
		}

		threshold(HBlackCoatingReduceImage, &HBlackCoatingCircleRgn, m_iEditBlackCoatingMinGray, m_iEditBlackCoatingMaxGray);
		opening_circle(HBlackCoatingCircleRgn, &HBlackCoatingCircleRgn, 1.5);
		connection(HBlackCoatingCircleRgn, &HBlackCoatingCircleRgn);

		select_shape_std (HBlackCoatingCircleRgn, &HBlackCoatingCircleRgn, "max_area", 100);
		fill_up(HBlackCoatingCircleRgn, &HBlackCoatingCircleRgn);

		gen_contour_region_xld(HBlackCoatingCircleRgn, &HBlackCoatingCircleXLD, "border");
		fit_ellipse_contour_xld(HBlackCoatingCircleXLD, "fitzgibbon", -1, 0, 0, 200, 5, 2, &ARow, &AColumn, &APhi, &ARadius1, &ARadius2, &AStartPhi, &AEndPhi, &APointOrder);
		gen_ellipse_contour_xld(&THEAPP.m_pInspectAdminViewDlg->m_HBlackCoatingCircleXLD, ARow, AColumn, APhi, ARadius1, ARadius2, AStartPhi, AEndPhi, APointOrder, 1.5);
		gen_region_contour_xld(THEAPP.m_pInspectAdminViewDlg->m_HBlackCoatingCircleXLD, &HBlackCoatingCircleRgn, "filled");

		if(ARow.Num() > 0 && AColumn.Num() > 0)
		{
			dBlackCoatingCenterRow = ARow[0].D();
			dBlackCoatingCenterCol = AColumn[0].D();

			THEAPP.m_pInspectAdminViewDlg->m_dBlackCoatingCenterRow = ARow[0].D();
			THEAPP.m_pInspectAdminViewDlg->m_dBlackCoatingCenterCol = AColumn[0].D();

			dBlackCoatingOuterDiameter = (ARadius2[0].D()  * 2) * PIXEL_SIZE_PMODE;
			THEAPP.m_pInspectAdminViewDlg->m_dBlackCoatingOuterDiameter = dBlackCoatingOuterDiameter;
		}

		if(m_iRadioInspectionType == 1 && (ARow.Num() > 0 && AColumn.Num() > 0))
		{
			Hlong Row1, Column1, Row2, Column2;
			Hobject MeasurePosRgn, AffineTransRgn, HPointRgn;
			HTuple HomMat2DIdentity, HomMat2DRotate;
			double dPhi, dHeight, dWidth;
			double dMRRow, dMRColumn, dAngle;
			HTuple MeasureHandle;
			HTuple RowEdge, ColumnEdge, Amplitude, Distance;
			PString Interpolation = "nearest_neighbor";

			HTuple ConcatRowEdge, ConcatColumnEdge;

			char cImageType[128];
			Hlong lWidth, lHeight;
			unsigned char *cPtr;
			get_image_pointer1(HBlackCoatingReduceImage, (Hlong*)&cPtr, cImageType, &lWidth, &lHeight);

			Row1 = ARow[0].D() - m_iEditBlackCoatingWidth;
			Column1 = AColumn[0].D()  - ARadius2[0].D() - m_iEditBlackCoatingHeight;
			Row2 = ARow[0].D() + m_iEditBlackCoatingWidth ;
			Column2 = AColumn[0].D()  - ARadius2[0].D() + m_iEditBlackCoatingHeight;

			PString sTransition;
			if(m_iRadioBlackCoatingTransition == 0)
				sTransition= "positive";
			else
				sTransition= "negative";

			gen_rectangle1(&MeasurePosRgn, Row1, Column1, Row2, Column2);

			for(double d = 22.5; d <= 157.5; d += 45)
			{
				//////////////////////////////////////////////////////////////////////////////////////////
				dAngle = CONV_RADIAN(d);

				hom_mat2d_identity(&HomMat2DIdentity);
				hom_mat2d_rotate(HomMat2DIdentity, dAngle, ARow[0].D(), AColumn[0].D(), &HomMat2DRotate);

				affine_trans_region(MeasurePosRgn, &AffineTransRgn, HomMat2DRotate, "false");

				smallest_rectangle2(AffineTransRgn, &dMRRow, &dMRColumn, &dPhi, &dHeight, &dWidth);
				
				if(dMRRow > 100 && dMRRow < lHeight - 100 && dMRColumn > 100 && dMRColumn < lWidth - 100)
				{
					gen_measure_rectangle2(dMRRow, dMRColumn, dAngle - P_PI, dHeight, dWidth, lWidth, lHeight, Interpolation.mcpStr, &MeasureHandle);
					measure_pos(HBlackCoatingReduceImage, MeasureHandle, 0.4, m_iEditBlackCoatingDiffGray, sTransition.mcpStr, "first", 
						&RowEdge, &ColumnEdge, &Amplitude, &Distance);

					if(RowEdge.Num() > 0 && ColumnEdge.Num() > 0)
					{
						tuple_concat(ConcatRowEdge, RowEdge, &ConcatRowEdge);
						tuple_concat(ConcatColumnEdge, ColumnEdge, &ConcatColumnEdge);
					}

					close_measure(MeasureHandle);
					concat_obj(THEAPP.m_pInspectAdminViewDlg->m_HBlackCoatingEdgePointRgn, AffineTransRgn, &THEAPP.m_pInspectAdminViewDlg->m_HBlackCoatingEdgePointRgn);

					gen_region_points(&HPointRgn, RowEdge, ColumnEdge);
					concat_obj(THEAPP.m_pInspectAdminViewDlg->m_HBlackCoatingEdgePointRgn, HPointRgn, &THEAPP.m_pInspectAdminViewDlg->m_HBlackCoatingEdgePointRgn);
				}


				//////////////////////////////////////////////////////////////////////////////////////////
				dAngle = CONV_RADIAN(-d);

				hom_mat2d_identity(&HomMat2DIdentity);
				hom_mat2d_rotate(HomMat2DIdentity, dAngle, ARow[0].D(), AColumn[0].D(), &HomMat2DRotate);

				affine_trans_region(MeasurePosRgn, &AffineTransRgn, HomMat2DRotate, "false");

				smallest_rectangle2(AffineTransRgn, &dMRRow, &dMRColumn, &dPhi, &dHeight, &dWidth);					

				if(dMRRow > 100 && dMRRow < lHeight - 100 && dMRColumn > 100 && dMRColumn < lWidth - 100)
				{
					gen_measure_rectangle2(dMRRow, dMRColumn, P_PI + dAngle, dHeight, dWidth, lWidth, lHeight, Interpolation.mcpStr, &MeasureHandle);
					measure_pos(HBlackCoatingReduceImage, MeasureHandle, 0.4, m_iEditBlackCoatingDiffGray, sTransition.mcpStr, "first", 
						&RowEdge, &ColumnEdge, &Amplitude, &Distance);

					if(RowEdge.Num() > 0 && ColumnEdge.Num() > 0)
					{
						tuple_concat(ConcatRowEdge, RowEdge, &ConcatRowEdge);
						tuple_concat(ConcatColumnEdge, ColumnEdge, &ConcatColumnEdge);
					}

					close_measure(MeasureHandle);
					concat_obj(THEAPP.m_pInspectAdminViewDlg->m_HBlackCoatingEdgePointRgn, AffineTransRgn, &THEAPP.m_pInspectAdminViewDlg->m_HBlackCoatingEdgePointRgn);

					gen_region_points(&HPointRgn, RowEdge, ColumnEdge);
					concat_obj(THEAPP.m_pInspectAdminViewDlg->m_HBlackCoatingEdgePointRgn, HPointRgn, &THEAPP.m_pInspectAdminViewDlg->m_HBlackCoatingEdgePointRgn);
				}
			}

			if(ConcatRowEdge.Num() >= 8 && ConcatColumnEdge.Num() >= 8)
			{
				double dNormalRadius = 0;
				for(int i = 0; i < ConcatRowEdge.Num(); i++)
				{
					double dDistance;
					distance_pp(ARow[0].D(), AColumn[0].D(), ConcatRowEdge[i].D(), ConcatColumnEdge[i].D(), &dDistance);
					dNormalRadius += dDistance;
				}

				dNormalRadius /= 8.0;

				THEAPP.m_pAlgorithm->InspectCircleFitting(ConcatColumnEdge, ConcatRowEdge, dNormalRadius, dBlackCoatingCenterCol, dBlackCoatingCenterRow, dBlackCoatingOuterDiameter);

				if(dBlackCoatingCenterCol > 0 && dBlackCoatingCenterRow > 0)
				{
					gen_circle_contour_xld(&THEAPP.m_pInspectAdminViewDlg->m_HBlackCoatingCircleXLD, dBlackCoatingCenterRow, dBlackCoatingCenterCol, dBlackCoatingOuterDiameter, 0, 6.28318, "positive", 1);
					gen_region_contour_xld(THEAPP.m_pInspectAdminViewDlg->m_HBlackCoatingCircleXLD, &HBlackCoatingCircleRgn, "filled");

					THEAPP.m_pInspectAdminViewDlg->m_dBlackCoatingCenterRow = dBlackCoatingCenterRow;
					THEAPP.m_pInspectAdminViewDlg->m_dBlackCoatingCenterCol = dBlackCoatingCenterCol;
					THEAPP.m_pInspectAdminViewDlg->m_dBlackCoatingOuterDiameter = dBlackCoatingOuterDiameter;
				}
			}
		}

		/////////////////////////////////////////////////////////////////////////////////////////////////////////////
		///Datum Circle 찾기
		/////////////////////////////////////////////////////////////////////////////////////////////////////////////
		THEAPP.m_pAlgorithm->GetCircleFitInspectArea(HDatumContour, m_iEditDatumOuterMargin, m_iEditDatumInnerMargin, &HDatumCircleRgn);
	
		if(m_bCheckUseImageScaling_DT==TRUE)
		{
			Hobject HScaleImage;
			THEAPP.m_pAlgorithm->GetScaledImage(HDatumImage, HDatumCircleRgn, m_iRadioImageScaleMethod_DT, m_iEditImageScaleMax_DT, m_iEditImageScaleMin_DT, &HScaleImage);
			reduce_domain(HScaleImage, HDatumCircleRgn, &HDatumReduceImage);
		}
		else
		{
			reduce_domain(HDatumImage, HDatumCircleRgn, &HDatumReduceImage);
		}

		threshold(HDatumReduceImage, &HDatumCircleRgn, m_iEditDatumMinGray, m_iEditDatumMaxGray);
		opening_circle(HDatumCircleRgn, &HDatumCircleRgn, 1.5);
		connection(HDatumCircleRgn, &HDatumCircleRgn);

		select_shape_std (HDatumCircleRgn, &HDatumCircleRgn, "max_area", 100);
		fill_up(HDatumCircleRgn, &HDatumCircleRgn);

		gen_contour_region_xld(HDatumCircleRgn, &HDatumCircleXLD, "border");
		fit_circle_contour_xld(HDatumCircleXLD, "algebraic", -1, 0, 0, 3, 2, &Row, &Column, &Radius, &StartPhi, &EndPhi, &PointOrder);

		if(Row.Num() > 0 && Column.Num() > 0)
		{
			dDatumCenterRow = Row[0].D();
			dDatumCenterCol = Column[0].D();

			THEAPP.m_pInspectAdminViewDlg->m_dDatumCenterRow = Row[0].D();
			THEAPP.m_pInspectAdminViewDlg->m_dDatumCenterCol = Column[0].D();

			gen_circle_contour_xld(&THEAPP.m_pInspectAdminViewDlg->m_HDatumCircleXLD, Row, Column, Radius, 0, 6.28318, "positive", 1);
		}


		if(m_iRadioInspectionType == 1 && (Row.Num() > 0 && Column.Num() > 0))
		{
			Hlong Row1, Column1, Row2, Column2;
			Hobject MeasurePosRgn, AffineTransRgn, HPointRgn;
			HTuple HomMat2DIdentity, HomMat2DRotate;
			double dPhi, dHeight, dWidth;
			double dMRRow, dMRColumn, dAngle;
			HTuple MeasureHandle;
			HTuple RowEdge, ColumnEdge, Amplitude, Distance;
			PString Interpolation = "nearest_neighbor";

			HTuple ConcatRowEdge, ConcatColumnEdge;

			char cImageType[128];
			Hlong lWidth, lHeight;
			unsigned char *cPtr;
			get_image_pointer1(HDatumImage, (Hlong*)&cPtr, cImageType, &lWidth, &lHeight);

			Row1 = Row[0].D() - m_iEditDatumWidth;
			Column1 = Column[0].D()  - Radius[0].D() - m_iEditDatumHeight;
			Row2 = Row[0].D() + m_iEditDatumWidth ;
			Column2 = Column[0].D()  - Radius[0].D() + m_iEditDatumHeight;

			PString sTransition;
			if(m_iRadioDatumTransition == 0)
				sTransition= "positive";
			else
				sTransition= "negative";

			gen_rectangle1(&MeasurePosRgn, Row1, Column1, Row2, Column2);

			for(double d = 22.5; d <= 157.5; d += 45)
			{
				//////////////////////////////////////////////////////////////////////////////////////////
				dAngle = CONV_RADIAN(d);

				hom_mat2d_identity(&HomMat2DIdentity);
				hom_mat2d_rotate(HomMat2DIdentity, dAngle, Row[0].D(), Column[0].D(), &HomMat2DRotate);

				affine_trans_region(MeasurePosRgn, &AffineTransRgn, HomMat2DRotate, "false");

				smallest_rectangle2(AffineTransRgn, &dMRRow, &dMRColumn, &dPhi, &dHeight, &dWidth);					

				if(dMRRow > 100 && dMRRow < lHeight - 100 && dMRColumn > 100 && dMRColumn < lWidth - 100)
				{
					gen_measure_rectangle2(dMRRow, dMRColumn, dAngle - P_PI, dHeight, dWidth, lWidth, lHeight, Interpolation.mcpStr, &MeasureHandle);
					measure_pos(HDatumImage, MeasureHandle, 0.4, m_iEditDatumDiffGray, sTransition.mcpStr, "first", 
						&RowEdge, &ColumnEdge, &Amplitude, &Distance);

					if(RowEdge.Num() > 0 && ColumnEdge.Num() > 0)
					{
						tuple_concat(ConcatRowEdge, RowEdge, &ConcatRowEdge);
						tuple_concat(ConcatColumnEdge, ColumnEdge, &ConcatColumnEdge);
					}

					close_measure(MeasureHandle);
					concat_obj(THEAPP.m_pInspectAdminViewDlg->m_HDatumEdgePointRgn, AffineTransRgn, &THEAPP.m_pInspectAdminViewDlg->m_HDatumEdgePointRgn);

					gen_region_points(&HPointRgn, RowEdge, ColumnEdge);
					concat_obj(THEAPP.m_pInspectAdminViewDlg->m_HDatumEdgePointRgn, HPointRgn, &THEAPP.m_pInspectAdminViewDlg->m_HDatumEdgePointRgn);
				}
				//////////////////////////////////////////////////////////////////////////////////////////

				dAngle = CONV_RADIAN(-d);

				hom_mat2d_identity(&HomMat2DIdentity);
				hom_mat2d_rotate(HomMat2DIdentity, dAngle, Row[0].D(), Column[0].D(), &HomMat2DRotate);

				affine_trans_region(MeasurePosRgn, &AffineTransRgn, HomMat2DRotate, "false");

				smallest_rectangle2(AffineTransRgn, &dMRRow, &dMRColumn, &dPhi, &dHeight, &dWidth);					

				if(dMRRow > 100 && dMRRow < lHeight - 100 && dMRColumn > 100 && dMRColumn < lWidth - 100)
				{
					gen_measure_rectangle2(dMRRow, dMRColumn, P_PI + dAngle, dHeight, dWidth, lWidth, lHeight, Interpolation.mcpStr, &MeasureHandle);
					measure_pos(HDatumImage, MeasureHandle, 0.4, m_iEditDatumDiffGray, sTransition.mcpStr, "first", 
						&RowEdge, &ColumnEdge, &Amplitude, &Distance);

					if(RowEdge.Num() > 0 && ColumnEdge.Num() > 0)
					{
						tuple_concat(ConcatRowEdge, RowEdge, &ConcatRowEdge);
						tuple_concat(ConcatColumnEdge, ColumnEdge, &ConcatColumnEdge);
					}

					close_measure(MeasureHandle);
					concat_obj(THEAPP.m_pInspectAdminViewDlg->m_HDatumEdgePointRgn, AffineTransRgn, &THEAPP.m_pInspectAdminViewDlg->m_HDatumEdgePointRgn);

					gen_region_points(&HPointRgn, RowEdge, ColumnEdge);
					concat_obj(THEAPP.m_pInspectAdminViewDlg->m_HDatumEdgePointRgn, HPointRgn, &THEAPP.m_pInspectAdminViewDlg->m_HDatumEdgePointRgn);
				}
			}

			if(ConcatRowEdge.Num() >= 8 && ConcatColumnEdge.Num() >= 8)
			{
				double dNormalRadius = 0;
				for(int i = 0; i < ConcatRowEdge.Num(); i++)
				{
					double dDistance;
					distance_pp(Row[0].D(), Column[0].D(), ConcatRowEdge[i].D(), ConcatColumnEdge[i].D(), &dDistance);
					dNormalRadius += dDistance;
				}
				dNormalRadius /= 8.0;

				THEAPP.m_pAlgorithm->InspectCircleFitting(ConcatColumnEdge, ConcatRowEdge, dNormalRadius, dDatumCenterCol, dDatumCenterRow, dDatumOuterDiameter);

				if(dDatumCenterCol > 0 && dDatumCenterRow > 0)
				{
					gen_circle_contour_xld(&THEAPP.m_pInspectAdminViewDlg->m_HDatumCircleXLD, dDatumCenterRow, dDatumCenterCol, dDatumOuterDiameter, 0, 6.28318, "positive", 1);

					THEAPP.m_pInspectAdminViewDlg->m_dDatumCenterRow = dDatumCenterRow;
					THEAPP.m_pInspectAdminViewDlg->m_dDatumCenterCol = dDatumCenterCol;
					THEAPP.m_pInspectAdminViewDlg->m_dDatumOuterDiameter = dDatumOuterDiameter;
				}
			}
		}

		double dLeftRow, dLeftCol, dRightRow, dRightCol;
		Hlong lPointOrder;
		Hobject HO1O2line;
		double dO1O2line, dBlobLength;
		Hlong lRow1, lRow2, lCol1, lCol2;

		if(dBlackCoatingCenterRow > 0 && dBlackCoatingCenterCol > 0 && dDatumCenterRow > 0 && dDatumCenterCol > 0)
		{
			distance_pp(dBlackCoatingCenterRow, dBlackCoatingCenterCol, dDatumCenterRow, dDatumCenterCol, &dO1O2line);

			long iHeight = THEAPP.m_pCameraManager->GetCamImageHeight();
			long iWidth = THEAPP.m_pCameraManager->GetCamImageWidth();

			if(dO1O2line <= 1.0)
			{
				intersection_ll(dDatumCenterRow, dDatumCenterCol - 10, dDatumCenterRow, dDatumCenterCol + 10, 5, 5, iHeight-5, 5, &dLeftRow, &dLeftCol, &lPointOrder);
				intersection_ll(dDatumCenterRow, dDatumCenterCol - 10, dDatumCenterRow, dDatumCenterCol + 10, 5, iWidth-5, iHeight-5, iWidth-5, &dRightRow, &dRightCol, &lPointOrder);
			}
			else
			{
				intersection_ll(dBlackCoatingCenterRow, dBlackCoatingCenterCol, dDatumCenterRow, dDatumCenterCol, 5, 5, iHeight-5, 5, &dLeftRow, &dLeftCol, &lPointOrder);
				intersection_ll(dBlackCoatingCenterRow, dBlackCoatingCenterCol, dDatumCenterRow, dDatumCenterCol, 5, iWidth-5, iHeight-5, iHeight-5, &dRightRow, &dRightCol, &lPointOrder);
			}
		
			gen_region_line(&HO1O2line, dLeftRow, dLeftCol, dRightRow, dRightCol);
			intersection(HO1O2line, HBlackCoatingCircleRgn, &HDiamSBLineRgn);
			diameter_region(HDiamSBLineRgn, &lRow1, &lCol1, &lRow2, &lCol2, &dBlobLength);

			if(lRow1!=lRow2 || lCol1!=lCol2)
			{
				HTuple HRow, HCol;
				tuple_concat (HRow, lRow1, &HRow);
				tuple_concat (HRow, lRow2, &HRow);

				tuple_concat (HCol, lCol1, &HCol);
				tuple_concat (HCol, lCol2, &HCol);

				gen_contour_polygon_xld(&THEAPP.m_pInspectAdminViewDlg->m_HDiamSBLineXLD, HRow, HCol);
			}

			THEAPP.m_pInspectAdminViewDlg->m_dO1O2Distance = dO1O2line * PIXEL_SIZE_PMODE;

			THEAPP.m_pInspectAdminViewDlg->m_dDiamSB = dBlobLength;
			THEAPP.m_pInspectAdminViewDlg->m_dDiameterMin = ((THEAPP.m_pInspectAdminViewDlg->m_dDiamSB - (dO1O2line * 2)) * PIXEL_SIZE_PMODE) + THEAPP.m_pModelDataManager->m_dBlackCoatingOffset;
		}

		copy_image(THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[THEAPP.m_pModelDataManager->m_iBlackMatchingImageType], &THEAPP.m_pInspectAdminViewDlg->m_pHImage);

		THEAPP.m_pInspectAdminViewDlg->UpdateView();
	}
}


void CInspectionParameterExtraDlg::OnBnClickedButtonDisplayScaledImageBc()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	UpdateData(TRUE);

	Hobject HBlackCoatingImage, HBlackCoatingContour;

	copy_image(THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[m_iComboBlackCoatingImage], &HBlackCoatingImage);
	copy_obj(THEAPP.m_pInspectAdminViewDlg->ContoursAffinTrans[m_iComboBlackCoatingContour], &HBlackCoatingContour, 1, 1);

	Hobject HBlackCoatingCircleRgn;
	BOOL bRet;
	Hobject HScaleImage;

	THEAPP.m_pAlgorithm->GetCircleFitInspectArea(HBlackCoatingContour, m_iEditBlackCoatingOuterMargin, m_iEditBlackCoatingInnerMargin, &HBlackCoatingCircleRgn);
	bRet = THEAPP.m_pAlgorithm->GetScaledImage(HBlackCoatingImage, HBlackCoatingCircleRgn, m_iRadioImageScaleMethod_BC, m_iEditImageScaleMax_BC, m_iEditImageScaleMin_BC, &HScaleImage);

	if (bRet)
	{
		copy_image(HScaleImage, &THEAPP.m_pInspectAdminViewDlg->m_pHImage);
		THEAPP.m_pInspectAdminViewDlg->UpdateView();
	}
}


void CInspectionParameterExtraDlg::OnBnClickedButtonDisplayScaledImageDt()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	UpdateData(TRUE);

	Hobject HDatumContour, HDatumImage;
	switch (m_iComboDatumContour)
	{
	case MATCHING_CONTOUR_1:
		copy_obj(THEAPP.m_pInspectAdminViewDlg->ContoursAffinTrans[MATCHING_CONTOUR_1], &HDatumContour, 1, 1);
		break;

	case MATCHING_CONTOUR_2:
		copy_obj(THEAPP.m_pInspectAdminViewDlg->ContoursAffinTrans[MATCHING_CONTOUR_2], &HDatumContour, 1, 1);
		break;

	case MATCHING_CONTOUR_3:
		copy_obj(THEAPP.m_pInspectAdminViewDlg->ContoursAffinTrans[MATCHING_CONTOUR_3], &HDatumContour, 1, 1);
		break;

	case MATCHING_CONTOUR_4:
		copy_obj(THEAPP.m_pInspectAdminViewDlg->ContoursAffinTrans[MATCHING_CONTOUR_4], &HDatumContour, 1, 1);
		break;

	case MATCHING_CONTOUR_5:
		copy_obj(THEAPP.m_pInspectAdminViewDlg->ContoursAffinTrans[MATCHING_CONTOUR_5], &HDatumContour, 1, 1);
		break;
	}

	copy_image(THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[m_iComboDatumImage], &HDatumImage);

	Hobject HDatumCircleRgn;
	BOOL bRet;
	Hobject HScaleImage;

	THEAPP.m_pAlgorithm->GetCircleFitInspectArea(HDatumContour, m_iEditDatumOuterMargin, m_iEditDatumInnerMargin, &HDatumCircleRgn);
	bRet = THEAPP.m_pAlgorithm->GetScaledImage(HDatumImage, HDatumCircleRgn, m_iRadioImageScaleMethod_DT, m_iEditImageScaleMax_DT, m_iEditImageScaleMin_DT, &HScaleImage);

	if (bRet)
	{
		copy_image(HScaleImage, &THEAPP.m_pInspectAdminViewDlg->m_pHImage);
		THEAPP.m_pInspectAdminViewDlg->UpdateView();
	}
}
