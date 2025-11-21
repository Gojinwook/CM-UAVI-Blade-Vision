// 24.02.27 Local Align 추가 - LeeGW Create

#pragma once

#include "LocalAlignApplyDlg.h"
#include "LocalAlignPartCheckDlg.h"
#include "LocalAlignEdgeMeasureDlg.h"

// CLocalAlignDlg 대화 상자입니다.

class CLocalAlignDlg : public CDialog
{
	DECLARE_DYNAMIC(CLocalAlignDlg)

public:
	CLocalAlignDlg(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CLocalAlignDlg();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual BOOL OnInitDialog();

	static	CLocalAlignDlg* GetInstance(BOOL bShowFlag=FALSE);
	void	DeleteInstance();

	void	Show();
	void	Hide();
	void	HideWindowAll();
	BOOL	GetShowStatus() { return m_bShowDlg; }
	
	void	SetSelectedInspection(int iImageType,GTRegion *pSelectedROI, int iInspectionType);
	void	SetParam();

	CTabCtrl m_LocalAlignTabCtrl;
	CLocalAlignApplyDlg *m_pLocalAlignApplyDlg;
	CLocalAlignPartCheckDlg *m_pLocalAlignPartCheckDlg;
	CLocalAlignEdgeMeasureDlg *m_pLocalAlignEdgeMeasureDlg;


// 대화 상자 데이터입니다.
	enum { IDD = IDD_LOCAL_ALIGN_DLG };

private:
	void	SetPosition(int left, int top, int right, int bottom) { m_ScreenRect = CRect(left, top, right, bottom); }
	CRect	m_ScreenRect;

	static CLocalAlignDlg*	m_pInstance;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()

	BOOL		m_bShowDlg;

	CSynLButton m_bnClose;
	CSynLButton m_bnSaveClose;

	int			m_iSelectedImageType;
	int			m_iInspectionType;
	GTRegion	*m_pSelectedROI;
	void		GetParam();

	Hobject m_HOrgScanImage;

public:
	afx_msg void OnBnClickedButtonSaveAndClose();
	afx_msg void OnBnClickedButtonClose();
	afx_msg void OnSelchangeAlgorithmTab(NMHDR *pNMHDR, LRESULT *pResult);

};
