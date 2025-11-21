#pragma once


// CInspectionTypeDlg 대화 상자입니다.

class CInspectionTypeDlg : public CDialog
{
	DECLARE_DYNAMIC(CInspectionTypeDlg)

public:
	CInspectionTypeDlg(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CInspectionTypeDlg();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_COSMETIC_INSPECTION_TYPE_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()

public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual BOOL OnInitDialog();
	int m_iRadioInspectionType;
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();

};
