#pragma once

#include "TeachAlgorithmListCtrl.h"

// CTeachParamROIDlg_Tab 대화 상자입니다.

class CTeachParamROIDlg_Tab : public CDialog
{
	DECLARE_DYNAMIC(CTeachParamROIDlg_Tab)

public:
	CTeachParamROIDlg_Tab(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CTeachParamROIDlg_Tab();
	void UpDate(BOOL bFlag);
	void UpdateList();

	CTeachAlgorithmListCtrl m_UsedAlgorithmList;

	// 대화 상자 데이터입니다.
	enum { IDD = IDD_TEACH_PARAM_ROI_DLG_TAB };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	int m_iTabIdx;
	int m_iImageIdx;

	BOOL m_bCheckInspect;
	CComboBox m_cbDefectType;

	BOOL m_bCheckArea1;
	int m_iEditArea1Contour1Margin;
	int m_iEditArea1Contour2Margin;
	BOOL m_bCheckArea2;
	int m_iEditArea2Contour1Margin;
	int m_iEditArea2Contour2Margin;
	BOOL m_bCheckArea3;
	int m_iEditArea3Contour1Margin;
	int m_iEditArea3Contour2Margin;


	BOOL m_bCheckSpecialNG;

	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedButtonRoiUpdate();
	afx_msg void OnBnClickedButtonROIAlgorithm();
	afx_msg void OnBnClickedButtonParameterTest();

protected:
	CButtonCS m_bnUpdateROI;
	CButtonCS m_bnParamTest;
	CButtonCS m_bnAlgorithm;
	CButtonCS m_bnROIAlgorithm;

	CStaticCS m_LabelInspect;
	CStaticCS m_LabelDefect;
	CStaticCS m_LabelArea;
	CStaticCS m_LabelAlgorithm;
	CStaticCS m_LabelROIAlgorithm;
	CStaticCS m_LabelSpecialNG;

public:

};
