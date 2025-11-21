#pragma once


// CLightAverageValueDlg 대화 상자입니다.

class CLightAverageValueDlg : public CDialog
{
	DECLARE_DYNAMIC(CLightAverageValueDlg)

public:
	CLightAverageValueDlg(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CLightAverageValueDlg();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_LIGHT_AVERAGE_VALUE_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	int m_iEditLight1;
	int m_iEditLight2;
	int m_iEditLight3;
	int m_iEditLight4;
	int m_iEditLight5;
	int m_iEditLightTotal;

protected:
	CButtonCS m_bnOK;
	CButtonCS m_bnCancel;

public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	virtual BOOL OnInitDialog();
};
