#if !defined(AFX_HWINPANEL_H__A5D6FC3E_1CB7_43D0_A02E_E216093336B6__INCLUDED_)
#define AFX_HWINPANEL_H__A5D6FC3E_1CB7_43D0_A02E_E216093336B6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// HWinPanel.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CHWinPanel window

class CHWinPanel : public CStatic
{
// Construction
public:
	CHWinPanel();

	HDC mhDC;
	HWindow *mpHWindow;
	Hlong mlWindowHandle;
	Hlong window_width, window_height;

// Attributes
public:

// Operations
public:
	void CreateHWindow();
	void DestroyHWindow();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CHWinPanel)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CHWinPanel();

	// Generated message map functions
protected:
	//{{AFX_MSG(CHWinPanel)
	afx_msg void OnPaint();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_HWINPANEL_H__A5D6FC3E_1CB7_43D0_A02E_E216093336B6__INCLUDED_)
