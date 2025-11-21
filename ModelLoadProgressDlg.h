#pragma once

// CModelLoadProgressDlg 대화 상자

class CModelLoadProgressDlg : public CDialog
{
	DECLARE_DYNAMIC(CModelLoadProgressDlg)

public:
	CModelLoadProgressDlg(CWnd *pParent = nullptr): CDialog
	(CModelLoadProgressDlg::IDD, pParent){};
	virtual ~CModelLoadProgressDlg();

	static CModelLoadProgressDlg* GetInstance(BOOL bShowFlag = FALSE);
	
	CProgressCtrl *GetProgressCtrl() { return &m_ProgressModelLoadDlg; }
// 대화 상자 데이터입니다.
enum
{
	IDD = IDD_DIALOGBAR_MODEL_LOAD
};

protected:
	virtual void DoDataExchange(CDataExchange *pDX); // DDX/DDV 지원입니다.
	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()

	// 모델 로드 진행도 UI
	CProgressCtrl m_ProgressModelLoadDlg;

	afx_msg LRESULT OnUpdateProgress(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnUpdateProgressDone(WPARAM wParam, LPARAM lParam);
public:
	afx_msg void OnNMCustomdrawProgressModelLoad(NMHDR *pNMHDR, LRESULT *pResult);
};
