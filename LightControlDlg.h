#pragma once


// CLightControlDlg 대화 상자입니다.
#include "LightControl.h"
#include "LightAutoCalResultDlg.h"
#include "MatchingLocalDlg.h"
#include "LightAverageValueDlg.h"

class CLightControlDlg : public CDialog
{
	DECLARE_DYNAMIC(CLightControlDlg)

public:
	CLightControlDlg(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CLightControlDlg();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_LIGHT_CONTROL_DLG };
	static CLightControlDlg	*m_pInstance;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()

public:
	static	CLightControlDlg* GetInstance(BOOL bShowFlag=FALSE);
	void	DeleteInstance();
	void	Show();

	CRect			m_ScreenRect;
	CRect  GetPosition() { return m_ScreenRect; }
	void   SetPosition(int left, int top, int right, int bottom) { m_ScreenRect = CRect(left, top, right, bottom); }

	int m_iTeachingGrab;
	int m_iAcceptRange_Uniformity;

	int m_iEditLight[MAX_LIGHT_CHANNEL];

	CComboBox m_iLightPageNo[MAX_LIGHT_PAGE];	// 조명 페이지 설정 추가 - LeeGW
	CSliderCtrl m_SliderLight[MAX_LIGHT_CHANNEL];
	CLightControl m_LightControl[2];

	void UpdateLightValue(int iTabIdx);

	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	
	afx_msg void OnPaint();
	afx_msg void OnBnClickedGrab();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnBnClickedButtonChangeLightvalue();
	afx_msg void OnBnClickedButtonUuniformity();
	afx_msg void OnBnClickedButtonAverageLight();
	afx_msg void OnBnClickedCalTest();
	afx_msg void OnBnClickedButtonSetMatchingParam();
	afx_msg void OnBnClickedButtonImageAlign();
};
