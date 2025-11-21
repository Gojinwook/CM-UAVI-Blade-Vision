#pragma once


// CTrayBeamProjectViewDlg 대화 상자입니다.

class CTrayBeamProjectViewDlg : public CDialog
{
	DECLARE_DYNAMIC(CTrayBeamProjectViewDlg)

public:
	CTrayBeamProjectViewDlg(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CTrayBeamProjectViewDlg();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_TRAYVIEW_BEAMPROJECT_DLG };
		static CTrayBeamProjectViewDlg	*m_pInstance;

public:
	static	CTrayBeamProjectViewDlg* GetInstance(BOOL bShowFlag=FALSE);
	void	DeleteInstance();
	void Show();

	CRect			m_ScreenRect;
	CRect  GetPosition() { return m_ScreenRect; }
	void   SetPosition(int left, int top, int right, int bottom) { m_ScreenRect = CRect(left, top, right, bottom); }
public:
		HTuple DispayViewWindowHandle;

	CRect DisplayRect;
	CTrayImagePB m_TrayImagePB;
	Hobject Image,Partitioned, RegionErosion;
	HTuple Width,Height;
	CFont* GetFont(LONG lfWidth, LONG lfHeight, LONG lfWeight);
	void MakeTrayRectangle();
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
};
