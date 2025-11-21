#pragma once

#include "TeachParamDlg_Tab.h"
#include "TeachParamROIDlg_Tab.h"

// CTeachParamDlg 대화 상자입니다.

class CTeachParamDlg : public CDialog
{
	DECLARE_DYNAMIC(CTeachParamDlg)

public:
	CTeachParamDlg(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CTeachParamDlg();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_TEACH_PARAM_DLG };

	void Show();

	CRect m_ScreenRect;
	CRect GetPosition() { return m_ScreenRect; }
	void SetPosition(int left, int top, int right, int bottom) { m_ScreenRect = CRect(left, top, right, bottom); }

	CTabCtrl m_AlgorithmTabCtrl;
	CTeachParamDlg_Tab *m_pTeachParamDlg_Tab[MAX_TEACHING_TAB];
	CTeachParamROIDlg_Tab* m_pTeachParamROIDlg_Tab[MAX_ROI_TEACHING_TAB];

	void SetInspectParam();
	void GetInspectParam();

	CButtonCS m_bnAlgorithmTest;

	int m_iImageIdx;
	int m_iPrevInspectTabIndex;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()

public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual BOOL OnInitDialog();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnSelchangeAlgorithmTab(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedButtonTabTest();

	afx_msg LRESULT OnEventROISelected(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnEventROIInspectionTabSave(WPARAM wParam, LPARAM lParam);
	
	void SetImageIdx(int iImageIdx);
	void SetDelegate();
};
