#pragma once

#include "HWinPanel.h"
#include "wcSliderButton.h"
#include "LightControl.h"

// CBarcodeDlg 대화 상자입니다.

class CBarcodeDlg : public CDialog
{
	DECLARE_DYNAMIC(CBarcodeDlg)

public:
	CBarcodeDlg(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CBarcodeDlg();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_BARCODE_DLG };
	CHWinPanel	m_NoiseControlView;

	CLightControl m_LightControl;

	void SetLightValue(int iLightValue) { m_iLightValue = iLightValue; }
	void SetImage(Hobject *pHTeachImage);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()

	wcSliderButton m_sLightValue;
	int m_iLightValue;

	Hobject				m_HImage;
	Hobject				m_HBarcodeDetectRgn;
	Hobject				m_HROIRgn;

	CRect				m_ViewRect;
	POINT				m_ViewOffset;
	GViewportManager	mViewportManager;
	double				m_dFitZoomRatio;

	POINT				maMIPoints[2];
	POINT				maMVPoints[2];

	int			m_iScanImageWidth;
	int			m_iScanImageHeight;

	void Reset();
	void SetViewportManager();
	void UpdateViewportManager();

	void ZoomIn();
	void ZoomOut();

	BOOL m_bLive;
	BOOL m_bOnOff;

public:
	virtual BOOL OnInitDialog();
	afx_msg void OnChangeEditLightValue();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnDestroy();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnContextMenu(CWnd* /*pWnd*/, CPoint /*point*/);
	afx_msg void OnPaint();
	afx_msg void OnClose();
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnBnClickedCheckLive();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnBnClickedButtonChangeLightvalue();
	afx_msg void OnBnClickedGrab();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedButtonMovetoPosition();
	double m_nDxHandlerPosX_B;
	afx_msg void OnBnClickedButtonSetStartPosition();
	afx_msg void OnBnClickedButtonMakeHandlerPositionMap();
	afx_msg void OnBnClickedButtonBarcodeTest();
	int m_iEditBarcodeLT_X;
	int m_iEditBarcodeLT_Y;
	int m_iEditBarcodeRB_X;
	int m_iEditBarcodeRB_Y;
	afx_msg void OnBnClickedButtonBarcodeShowRoi();
	afx_msg void OnBnClickedBarcodeSingleImageLoad();
	afx_msg void OnBnClickedBarcodeMultipleImageLoadInspection();
	afx_msg void OnBnClickedButtonBarcodeShiftHolecenterDisp();
	afx_msg void OnBnClickedButtonBarcodeShiftHolecenterNodisp();
	afx_msg void OnBnClickedButtonBarcodeShiftHolecenterSave();

	//Barcode Hole Center Start
	BOOL m_bCheckUseBarcodeShiftHoleCenter;
	int m_iEditBarcodeShiftHoleCenterDilation;
	int m_iEditBarcodeShiftHoleCenterGvMin;
	int m_iEditBarcodeShiftHoleCenterGvMax;
	int m_iEditBarcodeShiftHoleCenterGvClosing;
	Hobject m_HDispHoleRgn;
	Hobject m_HHoleRgn;
	Hobject m_HDispHoleSearchRgn;
	Hobject m_HDispBarcodeCenterXLD;
	Hobject m_HDispHoleCenterXLD;
	double m_dBarcodeCenterX;
	double m_dBarcodeCenterY;
	double m_dHoleCenterX;
	double m_dHoleCenterY;
	//Barcode Hole Center End
};
