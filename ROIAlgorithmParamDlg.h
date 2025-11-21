// 24.02.27 Local Align 추가 - LeeGW Create

#pragma once

#include "ROIPreprocessingDlg.h"
#include "ROIPartCheckDlg.h"
#include "ROIEdgeMeasureDlg.h"
#include "ROIAnisoAlignDlg.h"
#include "ROIAlgorithmCommonDlg.h"

// CROIAlgorithmParamDlg 대화 상자입니다.

class CROIAlgorithmParamDlg : public CDialog
{
	DECLARE_DYNAMIC(CROIAlgorithmParamDlg)

public:
	CROIAlgorithmParamDlg(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CROIAlgorithmParamDlg();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual BOOL OnInitDialog();

	static	CROIAlgorithmParamDlg* GetInstance(BOOL bShowFlag=FALSE);
	void	DeleteInstance();

	void	Show();
	void	Hide();
	void	HideWindowAll();
	BOOL	GetShowStatus() { return m_bShowDlg; }
	
	void	SetSelectedInspection(int iImageType,GTRegion *pSelectedROI, int iInspectionType);
	void	SetParam();

	CTabCtrl m_ROIAlgorithmTabCtrl;
	CROIPreprocessingDlg *m_pROIPreprocessingDlg;
	CROIPartCheckDlg *m_pROIPartCheckDlg;
	CROIEdgeMeasureDlg *m_pROIEdgeMeasureDlg;
	CROIAnisoAlignDlg* m_pROIAnisoAlignDlg;
	CROIAlgorithmCommonDlg* m_pROIInspAlgorithmDlg;


// 대화 상자 데이터입니다.
	enum { IDD = IDD_TEACH_ROI_ALORITHM_PARAM_DLG };

private:
	void	SetPosition(int left, int top, int right, int bottom) { m_ScreenRect = CRect(left, top, right, bottom); }
	CRect	m_ScreenRect;

	static CROIAlgorithmParamDlg*	m_pInstance;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()

	BOOL		m_bShowDlg;

	CButtonCS m_bnClose;
	CButtonCS m_bnSaveClose;

	int			m_iSelectedImageType;
	int			m_iInspectionType;
	GTRegion	*m_pSelectedROI;
	void		GetParam();

	Hobject m_HOrgScanImage;

public:
	afx_msg void OnBnClickedButtonSaveAndClose();
	afx_msg void OnBnClickedButtonClose();
	afx_msg void OnSelchangeAlgorithmTab(NMHDR *pNMHDR, LRESULT *pResult);

};
