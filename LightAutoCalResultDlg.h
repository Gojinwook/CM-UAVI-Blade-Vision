#pragma once


// CLightAutoCalResultDlg 대화 상자입니다.

class CLightAutoCalResultDlg : public CDialog
{
	DECLARE_DYNAMIC(CLightAutoCalResultDlg)

public:
	CLightAutoCalResultDlg(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CLightAutoCalResultDlg();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_LIGHT_AUTOCAL_RESULT_DLG };
	static CLightAutoCalResultDlg	*m_pInstance;


public:
	static	CLightAutoCalResultDlg* GetInstance(BOOL bShowFlag=FALSE);
	void	DeleteInstance();
	void	Show();
	void	Hide();

	CRect			m_ScreenRect;
	CRect  GetPosition() { return m_ScreenRect; }
	void   SetPosition(int left, int top, int right, int bottom) { m_ScreenRect = CRect(left, top, right, bottom); }

	void LoadViewParam();
	void UpdateResultView();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	CButtonCS m_bnOK;
	CButtonCS m_bnCancel;
	CButtonCS m_bnAutoCalTestLight;
	CButtonCS m_bnSaveSetting;

	CStaticCS m_LabelTitleValue_1;
	CStaticCS m_LabelTitleLV_1;
	CStaticCS m_LabelTitleGV_1;

	CStaticCS m_LabelTitleValue_2;
	CStaticCS m_LabelTitleLV_2;
	CStaticCS m_LabelTitleGV_2;

	CStaticCS m_LabelTitleValue_3;
	CStaticCS m_LabelTitleLV_3;
	CStaticCS m_LabelTitleGV_3;

	CStaticCS m_LabelTitleCH1;
	CStaticCS m_LabelTitleCH2;
	CStaticCS m_LabelTitleCH3;
	CStaticCS m_LabelTitleCH4;
	CStaticCS m_LabelTitleCH5;
	CStaticCS m_LabelTitleTotal;

	CStaticCS m_LabelTeachCH_LV[MAX_LIGHT_CHANNEL];
	CStaticCS m_LabelTeachCH_GV[MAX_LIGHT_CHANNEL];
	CStaticCS m_LabelInspectCH_LV[MAX_LIGHT_CHANNEL];
	CStaticCS m_LabelInspectCH_GV[MAX_LIGHT_CHANNEL];
	CStaticCS m_LabelJudgeCH[MAX_LIGHT_CHANNEL];

	CStaticCS m_LabelTeachTOTAL_LV;
	CStaticCS m_LabelTeachTOTAL_GV;
	CStaticCS m_LabelInspectTOTAL_LV;
	CStaticCS m_LabelInspectTOTAL_GV;
	CStaticCS m_LabelJudgeTOTAL;

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	int m_iEditLightValueStart;
	int m_iEditLightValueEnd;
	int m_iEditLightValueInterval;
	int m_iEditLightValueInTol;
	int m_iEditTotalImageValueTol;

	CProgressCtrl m_ctrlProgressAutoCal;
	afx_msg void OnBnClickedButtonSaveSetting();
	afx_msg void OnBnClickedButtonLightCalTest();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
};
