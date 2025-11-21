#pragma once

#include "ConditionGridCtrl.h"

// CConditionLensDlg 대화 상자입니다.

class CConditionLensDlg : public CDialog
{
	DECLARE_DYNAMIC(CConditionLensDlg)

public:
	CConditionLensDlg(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CConditionLensDlg();

	CConditionGridCtrl m_Grid;

// 대화 상자 데이터입니다.
	enum { IDD = IDD_CONDITION_LENS };

	virtual BOOL OnInitDialog();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
};
