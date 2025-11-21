#pragma once

#include "ConditionTopBarrelDlg.h"
#include "ConditionBottomBarrelDlg.h"
#include "ConditionInnerBarrelDlg.h"
#include "ConditionLensDlg.h"

// CInspectionConditionTabDlg 대화 상자입니다.

class CInspectionConditionTabDlg : public CDialog
{
	DECLARE_DYNAMIC(CInspectionConditionTabDlg)

public:
	CInspectionConditionTabDlg(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CInspectionConditionTabDlg();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_INSPECTION_CONDITION_TAB };

	static CInspectionConditionTabDlg	*m_pInstance;

public:
	static	CInspectionConditionTabDlg* GetInstance(BOOL bShowFlag=FALSE);
	void	DeleteInstance();
	void Show();

	CRect			m_ScreenRect;
	CRect  GetPosition() { return m_ScreenRect; }
	void   SetPosition(int left, int top, int right, int bottom) { m_ScreenRect = CRect(left, top, right, bottom); }

	CTabCtrl m_InspectionConditionTabCtrl;
	CConditionTopBarrelDlg *m_pConditionTopBarrelDlg;
	CConditionBottomBarrelDlg *m_pConditionBottomBarrelDlg;
	CConditionInnerBarrelDlg *m_pConditionInnerBarrelDlg;
	CConditionLensDlg *m_pConditionLensDlg;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()

public:
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnSelchangeInspectionConditionTab(NMHDR *pNMHDR, LRESULT *pResult);

};
