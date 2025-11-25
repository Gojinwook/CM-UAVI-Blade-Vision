#pragma once

#include "TrayImagePB.h"
// CTrayAdminViewDlg 대화 상자입니다.

class CTrayAdminViewDlg : public CDialog
{
	DECLARE_DYNAMIC(CTrayAdminViewDlg)

public:
	CTrayAdminViewDlg(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CTrayAdminViewDlg();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_TRAYVIEW_ADMIN_DLG };
	static CTrayAdminViewDlg	*m_pInstance;

public:
	static	CTrayAdminViewDlg* GetInstance(BOOL bShowFlag=FALSE);
	void	DeleteInstance();
	void Show();

	CRect			m_ScreenRect;
	CRect  GetPosition() { return m_ScreenRect; }
	void   SetPosition(int left, int top, int right, int bottom) { m_ScreenRect = CRect(left, top, right, bottom); }
public:
	HTuple DispayViewWindowHandle;

	CRect DisplayRect;
	CTrayImagePB m_TrayImagePB;
	Hobject Image, Partitioned, RegionErosion;
	HTuple Width,Height;
	BOOL m_bLoadTray,m_bPrevTray;

	double* mp_dBlackCoatingOuterDiameter;
	double* mp_dBlackCoatingPosX;
	double* mp_dBlackCoatingPosY;
	double* mp_dO1O2Distance;
	double* mp_dDiameterMin;

	void UpdateTrayDisplay();
	void MakeTrayRectangle();
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
//	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnWindowPosChanging(WINDOWPOS* lpwndpos);
	afx_msg void OnBnClickedButtonopenPrevTray();
	afx_msg void OnPaint();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};
