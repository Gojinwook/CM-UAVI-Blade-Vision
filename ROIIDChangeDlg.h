#pragma once


// CROIIDChangeDlg 대화 상자

class CROIIDChangeDlg : public CDialog
{
	DECLARE_DYNAMIC(CROIIDChangeDlg)

public:
	CROIIDChangeDlg(CWnd* pParent = nullptr);   // 표준 생성자입니다.
	virtual ~CROIIDChangeDlg();

	void SetROIType(int iType) { m_iROIType = iType; }

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ROI_ID_CHANGE };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()

	int m_iROIType;
public:
	int m_iEditId;
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};
