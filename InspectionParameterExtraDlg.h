#pragma once


// CInspectionParameterExtraDlg 대화 상자입니다.

class CInspectionParameterExtraDlg : public CDialog
{
	DECLARE_DYNAMIC(CInspectionParameterExtraDlg)

public:
	CInspectionParameterExtraDlg(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CInspectionParameterExtraDlg();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_INSPECTION_PARAMETER_EXTRA };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()

private:
	afx_msg void OnBnClickedBnOk();
	afx_msg void OnBnClickedBnCancle();

public:
	int m_iTopBarrelInner_RoiOuterMargin;
	int m_iTopBarrelInner_RoiInnerMargin;

	void SetInspectParam();
	void GetInspectParam();
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedButtonShowArea();
	afx_msg void OnBnClickedButtonShowDiameter();

	BOOL m_bCheckInspectBarcodeShift;
	double m_dEditBarcodeShiftRefX;
	double m_dEditBarcodeShiftRefY;
	double m_dEditBarcodeShiftToleranceX;
	double m_dEditBarcodeShiftToleranceY;

	//Barcode Rotation Inspection
	BOOL m_bCheckInspectBarcodeRotation;
	double m_dEditBarcodeShiftRefRotation;
	double m_dEditBarcodeShiftToleranceRotation;

	BOOL m_bCheckInspectBlackCoatingDiameter;
	double m_dEditBlackCoatingRef;
	double m_dEditBlackCoatingTolerance;
	double m_dEditBlackCoatingOffset;

	int m_iComboBlackCoatingImage;
	int m_iComboBlackCoatingContour;
	int m_iEditBlackCoatingOuterMargin;
	int m_iEditBlackCoatingInnerMargin;
	int m_iEditBlackCoatingMinGray;
	int m_iEditBlackCoatingMaxGray;

	int m_iComboDatumImage;
	int m_iComboDatumContour;
	int m_iEditDatumOuterMargin;
	int m_iEditDatumInnerMargin;
	int m_iEditDatumMinGray;
	int m_iEditDatumMaxGray;

	BOOL m_bCheckUseImageScaling_BC;
	BOOL m_bCheckUseImageScaling_DT;
	int m_iRadioImageScaleMethod_BC;
	int m_iRadioImageScaleMethod_DT;
	int m_iEditImageScaleMax_BC;
	int m_iEditImageScaleMax_DT;
	int m_iEditImageScaleMin_BC;
	int m_iEditImageScaleMin_DT;
	int m_iRadioInspectionType;

	int m_iEditBlackCoatingWidth;
	int m_iEditBlackCoatingHeight;
	int m_iEditBlackCoatingDiffGray;
	int m_iRadioBlackCoatingTransition;
	int m_iEditDatumWidth;
	int m_iEditDatumHeight;
	int m_iEditDatumDiffGray;
	int m_iRadioDatumTransition;
	afx_msg void OnBnClickedButtonDisplayScaledImageBc();
	afx_msg void OnBnClickedButtonDisplayScaledImageDt();
};
