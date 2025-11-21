// uScanView.h : interface of the CuScanView class
//
/////////////////////////////////////////////////////////////////////////////
//{{AFX_INCLUDES()
//#include "powerbutton.h"
//#include "labelcontrol.h"
//}}AFX_INCLUDES
#if !defined(AFX_uScanVIEW_H__342A5925_9989_4EAC_AF58_961B02DF88E8__INCLUDED_)
#define AFX_uScanVIEW_H__342A5925_9989_4EAC_AF58_961B02DF88E8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "NewModelDlg.h"
#include "PreferenceDlg.h"
#include "afxwin.h"
class CuScanView : public CFormView
{
	DECLARE_DYNCREATE(CuScanView)

protected: // create from serialization only
	CuScanView();
	virtual ~CuScanView();

public:
	//{{AFX_DATA(CuScanView)
	enum { IDD = IDD_uScan_FORM };

	CPreferenceDlg PreferenceDlg;

	CNewModelDlg m_pNewModelDlg;

	CStaticCS m_StaticSPIVersion;

	//}}AFX_DATA

// Attributes
public:
	CuScanDoc* GetDocument();
	void SetStatusText(int idx, CString szMsg="");

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CuScanView)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void OnInitialUpdate(); // called first time after construct
	//}}AFX_VIRTUAL

// Implementation
public:
	void TeachingShow();
	void UpdateCurMode();

	void SetExitProgram();
	void SetTextPCBCount(CString strPCBCount);
	void SetButtonStatus(int nStatus);

	void CheckDiskSpace();

#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	CTime	m_StartTime;
	BOOL	m_bTeachingShow;

	CStaticCS m_LabelCurMode;
	BOOL m_bStatusToggle;

// Generated message map functions
protected:
	void SetTextCurrentMode(int nCurrentMode);
	void ShowVersionText();

	//{{AFX_MSG(CuScanView)
	afx_msg void OnDestroy();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	//}}AFX_MSG
	afx_msg LRESULT OnEventPcbModelChanaged(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnEventChangeMode(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnEventChangeStatus(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedMfcbuttonFormNew();
	afx_msg void OnBnClickedMfcbuttonFormLoad();
	afx_msg void OnBnClickedMfcbuttonFormSave();
	afx_msg void OnBnClickedMfcbuttonFormTeaching();
	afx_msg void OnBnClickedMfcbuttonFormPreference();
	afx_msg void OnBnClickedMfcbuttonFormExit();
	afx_msg void OnBnClickedButtonModeChange();
	afx_msg void OnBnClickedButtonTrayLoading();
	afx_msg void OnBnClickedButtonTrayUnloading();
	afx_msg void OnBnClickedMfcbuttonFormRegister();
};

#ifndef _DEBUG  // debug version in uScanView.cpp
inline CuScanDoc* CuScanView::GetDocument() { return (CuScanDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_uScanVIEW_H__342A5925_9989_4EAC_AF58_961B02DF88E8__INCLUDED_)
