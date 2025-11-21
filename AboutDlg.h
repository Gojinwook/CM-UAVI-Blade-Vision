#pragma once
#include "uScan.h"

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

	BOOL GetAppVersion(void);
	void GetAppModify(void);

	CString GetFileDateStr(CString FilePath);
	void	ShowReleaseInformation();

	CBrush m_EditBGBrush;

	// Dialog Data
		//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	CEdit	m_EditRelease;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
	virtual BOOL OnInitDialog();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
};

