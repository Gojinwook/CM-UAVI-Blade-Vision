#pragma once

#include "afxcmn.h"
// CInspectSummary 대화 상자입니다.

class CInspectSummary : public CDialog
{
	DECLARE_DYNAMIC(CInspectSummary)

public:
	CInspectSummary(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CInspectSummary();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_INSPECT_SUMMARY };
	static CInspectSummary	*m_pInstance;


public:
	static CInspectSummary* GetInstance(BOOL bShowFlag=FALSE);
	void DeleteInstance();
	void Show();

	CRect m_ScreenRect;
	CRect GetPosition() { return m_ScreenRect; }
	void SetPosition(int left, int top, int right, int bottom) { m_ScreenRect = CRect(left, top, right, bottom); }

	void SetBtnEnable(BOOL bEnable);

protected:
	CCheckCS m_chkUseBLInsp;

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnBnClickedMfcbuttonInspectStart();
	afx_msg void OnWindowPosChanging(WINDOWPOS* lpwndpos);
	afx_msg void OnBnClickedMfcbuttonInspectPass();
	afx_msg void OnBnClickedMfcbuttonInspectPause();
	afx_msg void OnBnClickedMfcbuttonStatusCheck();

	afx_msg void OnBnClickedBnFobset();
	afx_msg void OnClickedChkFobtest();
	afx_msg void OnBnClickedMfcbuttonInspectCyclestop();
	afx_msg void OnBnClickedChkAllignImg();
	afx_msg void OnBnClickedChkAccelMatching();

	void CallbuttonInspectStart();
	void CheckPrevSaveFolder();
	void CheckPrevSaveFolder(CString sLotID, CString sTrayNo);
	void CheckLastTrayFolder();

	void Initialize_one_line();
	void Initailize_for_Inspection();
//	Algorithm Algo;
	//int CNT;
	void CheckLotIDAndChangeModel(CString LotID);
	void ShowDiskCapacity();


	LRESULT OnInspectionLoop(WPARAM wParam,LPARAM lParam);
	CProgressCtrl m_staticProgress;

	// 조명 밝기 확인 모드
	BOOL m_bUniformityCheckMode;
	int m_iDxTrayNo;
	int m_iDxLineNo;
	BOOL m_bDxAllignImg;
	BOOL m_bUseAllignImg;
	BOOL m_bDxAccelMatching;
	BOOL m_bUseAccelMatching;
	BOOL m_bUseBLInsp;
	BOOL m_bDxFobTest;
	BOOL m_bUseFobTest;
	CProgressCtrl m_ctrlProgressHddC;
	CProgressCtrl m_ctrlProgressHddD;
	afx_msg void OnDeltaposSpinTrayNo(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnDeltaposSpinStageNo(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	virtual BOOL OnInitDialog();
	
	BOOL m_bHandlerReloadDone;
	
	CStatic m_lbIndexNo;
	CStatic m_lbPocketNo;
	CStatic m_lbInspStatus;

	int m_iIndexNo;
	int m_iPocketNo;
	CString m_sInspStatus;

	BOOL m_bRestartBtnClicked;

	BOOL bIsRestart;
	double dProcessStartTime;
	BOOL bIsSatusCheckRetry;

afx_msg void OnBnClickedChkBlInspUse();
};
