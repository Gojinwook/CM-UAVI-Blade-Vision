#pragma once

#include "ConditionGridCtrl.h"

// CConditionInnerBarrelDlg 대화 상자입니다.

class CConditionInnerBarrelDlg : public CDialog
{
	DECLARE_DYNAMIC(CConditionInnerBarrelDlg)

public:
	CConditionInnerBarrelDlg(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CConditionInnerBarrelDlg();

	CConditionGridCtrl m_Grid;

// 대화 상자 데이터입니다.
	enum { IDD = IDD_CONDITION_INNER_BARREL };

	virtual BOOL OnInitDialog();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
};
