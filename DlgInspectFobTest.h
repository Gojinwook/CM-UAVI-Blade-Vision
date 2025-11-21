#pragma once


// CDlgInspectFobTest 대화 상자입니다.

class CDlgInspectFobTest : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgInspectFobTest)

public:
	CDlgInspectFobTest(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CDlgInspectFobTest();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_FOB_SETTING };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	int m_iDxGPro;
	int m_iDxNPro;
	int m_iDxEPro;
	afx_msg void OnBnClickedBnSetGood();
	afx_msg void OnBnClickedBnSetNg();
	afx_msg void OnBnClickedBnSetEmpty();
	afx_msg void OnBnClickedBnSetAllgood();
	afx_msg void OnBnClickedBnSetAllng();
	afx_msg void OnBnClickedBnSetAllempty();
	afx_msg void OnBnClickedOk();
	virtual BOOL OnInitDialog();
};
