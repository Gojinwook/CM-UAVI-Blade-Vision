#pragma once


// CLoadModelDlg 대화 상자입니다.

class CLoadModelDlg : public CDialog
{
	DECLARE_DYNAMIC(CLoadModelDlg)

public:
	CLoadModelDlg(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CLoadModelDlg();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_LOAD_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()


public:
	void LoadModelList();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual BOOL OnInitDialog();
	CListBox m_ModelList;
	CString m_EditSelectedModel;
	CString m_EditModelDescription;
	afx_msg void OnSelchangeListModelnames();
	afx_msg void OnBnClickedLmLoad();
	afx_msg void OnBnClickedLmCancel();
	afx_msg void OnClickedLmOpenModelfolder();
};
