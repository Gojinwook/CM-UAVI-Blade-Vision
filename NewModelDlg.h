#pragma once


// CNewModelDlg 대화 상자입니다.

class CNewModelDlg : public CDialog
{
	DECLARE_DYNAMIC(CNewModelDlg)

public:
	CNewModelDlg(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CNewModelDlg();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_NEW_MODEL_DLG };

public:

	BOOL CheckInfomation();



protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonOk();
	afx_msg void OnBnClickedButtonCancel();
	CString m_Edit_strModelName;
	double m_Edit_dModulePitchX;
	double m_Edit_dModulePitchY;
	int m_Edit_iTrayArrayX;
	int m_Edit_iTrayArrayY;
	virtual BOOL OnInitDialog();
	CEdit m_edtModelName;
	BOOL m_Edit_bBlackCoating;
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	int m_iRadioBarrelType;
};
