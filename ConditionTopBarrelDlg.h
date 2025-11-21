#pragma once

#include "ConditionGridCtrl.h"

// CConditionTopBarrelDlg 대화 상자입니다.

class CConditionTopBarrelDlg : public CDialog
{
	DECLARE_DYNAMIC(CConditionTopBarrelDlg)

public:
	CConditionTopBarrelDlg(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CConditionTopBarrelDlg();

	CConditionGridCtrl m_Grid;

// 대화 상자 데이터입니다.
	enum { IDD = IDD_CONDITION_TOP_BARREL };

	virtual BOOL OnInitDialog();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
};
