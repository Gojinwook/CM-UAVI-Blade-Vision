#pragma once

// FAI 치수 측정 - LeeGW
// CFAISettingDlg 대화 상자


class CFAISettingDlg : public CDialog
{
	DECLARE_DYNAMIC(CFAISettingDlg)

public:
	CFAISettingDlg(CWnd* pParent = nullptr);   // 표준 생성자입니다.
	virtual ~CFAISettingDlg();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_FAI_SETTING_DLG };


protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();

	double m_dFAIMultiple[MAX_FAI_ITEM];
	double m_dFAIOffset[MAX_FAI_ITEM];
	double m_dFAISpecMin[MAX_FAI_ITEM];
	double m_dFAISpecMax[MAX_FAI_ITEM];
	BOOL m_bFAINGUse[MAX_FAI_ITEM];
	BOOL m_bFAISpecialNGUse[MAX_FAI_ITEM];
	BOOL m_bFAIUniqueOffsetUse[MAX_FAI_ITEM];
	double m_dFAIUniqueMultiple[MAX_FAI_ITEM][MAX_CIRCULAR_TABLE_POCKET];
	double m_dFAIUniqueOffset[MAX_FAI_ITEM][MAX_CIRCULAR_TABLE_POCKET];

	int m_iFAIInnerCircleFitType;
	int m_iFAIOuterCircleFitType;
	int m_iFAIDatumCircleFitType;
	int m_iFAISimulPocketNum;
	virtual BOOL OnInitDialog();
};
