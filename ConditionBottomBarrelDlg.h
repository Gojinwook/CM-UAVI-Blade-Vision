#pragma once

#include "ConditionGridCtrl.h"

// CConditionBottomBarrelDlg 대화 상자입니다.

class CConditionBottomBarrelDlg : public CDialog
{
	DECLARE_DYNAMIC(CConditionBottomBarrelDlg)

public:
	CConditionBottomBarrelDlg(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CConditionBottomBarrelDlg();

	CConditionGridCtrl m_Grid;

// 대화 상자 데이터입니다.
	enum { IDD = IDD_CONDITION_BOTTOM_BARREL };

	virtual BOOL OnInitDialog();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
};
