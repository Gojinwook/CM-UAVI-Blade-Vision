#pragma once


// CTabControlDlg 대화 상자입니다.
#include "ThresholdHistogramDlg.h"
#include "LightControlDlg.h"
#include "JogSetDlg.h"
#include "InspectionConditionTabDlg.h"
#include "MatchingDlg.h"
#include "AutoFocusDlg.h"
#include "TeachParamDlg.h"

#include "ROIAlgorithmTabDlg.h"	// 24.02.28 Local Align 추가 - LeeGW 
#include "AlgorithmDlg.h"

class CTabControlDlg : public CDialog
{
	DECLARE_DYNAMIC(CTabControlDlg)

public:
	CTabControlDlg(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CTabControlDlg();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_TEACHING_TAB_DLG };
	static CTabControlDlg	*m_pInstance;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()

public:
	static CTabControlDlg* GetInstance(BOOL bShowFlag=FALSE);
	void DeleteInstance();
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);

	afx_msg void OnSelchangeTeachingTab(NMHDR *pNMHDR, LRESULT *pResult);

	void Show();
	void ShowFirstPage();
	void HideAllPage();

	void Read_Image(int tab);
	void LoadModelConditionParam();

	void SetPosition(int left, int top, int right, int bottom) { m_ScreenRect = CRect(left, top, right, bottom); }

	CRect m_ScreenRect;
	CRect GetPosition() { return m_ScreenRect; }

	CThresholdHistogramDlg* m_pThresholdHistogramDlg;
	CLightControlDlg*  m_pLightControlDlg;
	CJogSetDlg* m_pJogSetDlg;
	CInspectionConditionTabDlg* m_pInspectionConditionTabDlg;
	CMatchingDlg* m_pMatchingDlg;
	CAutoFocusDlg* m_pAutoFocusDlg;

	CTeachParamDlg* m_pTeachParamDlg[MAX_IMAGE_TAB];	// 검사유형 통합 - LeeGW

	CAlgorithmDlg *m_pAlgorithmDlg;

	CLightAutoCalResultDlg* m_pLightAutoCalResultDlg;

	// 검사유형 통합(ROI기준 알고리즘 설정) - LeeGW START
	CROIAlgorithmTabDlg *m_pROIAlgorithmParamDlg;	
	void ShowROIAlgorithmWnd(int iImageType, GTRegion *pROIRegion, int iInspectionType);
	void ShowROIAlgorithmParamDlg();
	void HideROIAlgorithmParamDlg();
	// 검사유형 통합(ROI기준 알고리즘 설정) - LeeGW END

	void ShowAlgorithmWnd(int iImageType, int iInspectionType);
	void UpdateList(int iImageType, int iInspectionType);
	void UpdateROIList(int iImageType, int iInspectionType);
	void ShowAlgorithmDlg();
	void HideAlgorithmDlg();
	void ShowLightAutoCalResultDlg();
	void HideLightAutoCalResultDlg();

	CTabCtrl m_TabControl;

	int m_iCurrentTab;
	afx_msg void OnDestroy();
protected:
	afx_msg LRESULT OnTeachingTabDlgInitDone(WPARAM wParam, LPARAM lParam);
};
