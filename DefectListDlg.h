#pragma once


// CDefectListDlg 대화 상자입니다.

class CDefectListDlg : public CDialog
{
	DECLARE_DYNAMIC(CDefectListDlg)

public:
	CDefectListDlg(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CDefectListDlg();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_DEFECT_LIST_DLG };
		static CDefectListDlg	*m_pInstance;

public:
	static	CDefectListDlg* GetInstance(BOOL bShowFlag=FALSE);
	void	DeleteInstance();
	void Show();

	CRect			m_ScreenRect;
	CRect  GetPosition() { return m_ScreenRect; }
	void   SetPosition(int left, int top, int right, int bottom) { m_ScreenRect = CRect(left, top, right, bottom); }
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()


public:
	void DefectSummury(Hobject HDefectRgn,int No_Module,int iBarrelOrLens);
	BOOL m_bDefectListDisplay;
	int m_iBarrelOrLens;
public:
	CListCtrl m_conDefectList;
	virtual BOOL OnInitDialog();
	afx_msg void OnWindowPosChanging(WINDOWPOS* lpwndpos);
	afx_msg void OnLvnItemchangedDefectList(NMHDR *pNMHDR, LRESULT *pResult);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};
