#pragma once

class AFX_EXT_CLASS CSynControls : public CWnd
{
	DECLARE_DYNAMIC(CSynControls)

public:
	CSynControls();
	virtual ~CSynControls();

protected:
	DECLARE_MESSAGE_MAP()

public:
	static void Initialize(void);
	static void UnInitialize(void);
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// CSynButton

class AFX_EXT_CLASS CSynButton : public CMFCButton
{
	DECLARE_DYNAMIC(CSynButton)

public:
	CSynButton();
	virtual ~CSynButton();

protected:
	//afx_msg void OnTimer(UINT_PTR nIDEvent);
	DECLARE_MESSAGE_MAP()

	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	virtual void OnDrawBorder(CDC* pDC, CRect& rectClient, UINT uiState);

	void DrawBorder(CDC* pDC, CRect& rectClient, UINT uiState);

public:

	enum enTypeGradient{typeDefault = 0, typeBlue, typeOliveGreen, typeSilver, typeRed};
	enum enTypeInnerRect{typeInnerNone = 0, typeInnerLine, typeInner3D};
	enum enTypeGradientDirection{typeTopBottom = 0, typeLeftRight };

	int SetInit(UINT nOn, UINT nHighLight=0, UINT nOff=0, CString strToolTip = _T(""), int nInner=1 );

	int SetBackGradientColorType(int nType);
	int SetGradientType(int nType);

	int SetBackColor(COLORREF rgbBkgnd);
	int SetBackGradientColor(COLORREF rgb1, COLORREF rgb2, int nType=0);

	int SetHighLightBackColor(COLORREF rgbBkgnd);
	int SetHighLightBackGradientColor(COLORREF rgb1, COLORREF rgb2, int nType=0);

	int SetPushedBackColor(COLORREF rgbBkgnd);
	int SetPushedBackGradientColor(COLORREF rgb1, COLORREF rgb2, int nType=0);

	int SetDisableBackColor(COLORREF rgbBkgnd);
	int SetDisableBackGradientColor(COLORREF rgb1, COLORREF rgb2, int nType=0);

	int SetToggleButton();
	int SetToggleState(BOOL bToggle);
	int GetToggleState();
	int SetToggle();

	int SetInsertAfter(const CWnd* pWnd);
	
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);

protected:
	int DrawInnerRect(CDC* pDC, CRect rect, int nOffset =1);
	int DrawBackground(CDC* pDC, const COLORREF& rgb1, const COLORREF& rgb2, const int& nType);
	int DrawInner3DRect(CDC* pDC, CRect rect, UINT uiState);

	int SetAlphaBlend(CDC* pDC, UINT nX, UINT nY, COLORREF rgb, BYTE byAlpha);

	int m_nGradientType;
	int m_nInnerRectType;

	BOOL m_bToggleButton;
	BOOL m_bToggled;

	UINT m_nIDOn;
	UINT m_nIDOff;
	UINT m_nIDHighLight;

	BOOL m_bDisabled;

	CRect		m_rtNormalBk;
	CRect		m_rtHighLightBk;
	CRect		m_rtPushedBk;
	CRect		m_rtDisableBk;


	COLORREF	m_rgbNormal0;
	COLORREF	m_rgbNormal1;
	COLORREF	m_rgbHighLight0;
	COLORREF	m_rgbHighLight1;
	COLORREF	m_rgbPushed0;
	COLORREF	m_rgbPushed1;
	COLORREF	m_rgbDisable0;
	COLORREF	m_rgbDisable1;
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};

class AFX_EXT_CLASS CSynLButton : public CSynButton
{
	DECLARE_DYNAMIC(CSynLButton)

public:
	CSynLButton();
	virtual ~CSynLButton();

protected:
	DECLARE_MESSAGE_MAP()

	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);

public:
	int SetInit(UINT nOn, UINT nHighLight=0, UINT nOff=0, CString strToolTip = _T("") );
	
};

class AFX_EXT_CLASS CSynLED : public CMFCButton
{
	DECLARE_DYNAMIC(CSynLED)

public:
	CSynLED();
	virtual ~CSynLED();

	virtual BOOL PreTranslateMessage(MSG* pMsg);

protected:
	DECLARE_MESSAGE_MAP()

public:
	int SetInit(UINT nOn, UINT nHighLight=0, UINT nOff=0, CString strToolTip = _T("") );
	int SetInsertAfter(const CWnd* pWnd);

	int SetOn();
	virtual int SetOff();

	int SetToggle();

protected:
	UINT m_nIDOn;
	UINT m_nIDOff;
	UINT m_nIDHighLight;

	BOOL m_bToggleState;

};


class AFX_EXT_CLASS CSynPCB : public CSynLED
{
	DECLARE_DYNAMIC(CSynPCB)

public:
	CSynPCB();
	virtual ~CSynPCB();

protected:
	DECLARE_MESSAGE_MAP()

public:
	virtual int SetOff();
	int SetHighLight();
	int SetGood();
	int SetNG();
};

class AFX_EXT_CLASS CSynToggle : public CMFCButton
{
	DECLARE_DYNAMIC(CSynToggle)

public:
	CSynToggle();
	virtual ~CSynToggle();

	virtual BOOL PreTranslateMessage(MSG* pMsg);

protected:
	DECLARE_MESSAGE_MAP()

public:
	int SetInit(UINT nOn, UINT nHighLight=0, UINT nOff=0, CString strToolTip = _T("") );
	int SetInsertAfter(const CWnd* pWnd);
	int SetToggleState(int nOn);
	int GetToggleState();
	int SetToggle();

protected:
	int		m_nToggleState;
	UINT m_nIDOn;
	UINT m_nIDOff;
	UINT m_nIDHighLight;

};


class AFX_EXT_CLASS CSynCheckBox : public CButton
{
	DECLARE_DYNAMIC(CSynCheckBox)

public:
	CSynCheckBox();
	virtual ~CSynCheckBox();

	virtual BOOL PreTranslateMessage(MSG* pMsg);

	int SetFontSize(int nHeight, CString strFontName=_T(""));
	int SetForeColor(COLORREF rgbFont);


protected:
	afx_msg LRESULT OnSetText(WPARAM,LPARAM);
	afx_msg HBRUSH CtlColor(CDC* /*pDC*/, UINT /*nCtlColor*/);
	DECLARE_MESSAGE_MAP()

	LOGFONT		m_LogFont;
	CFont		m_Font;
	COLORREF	m_rgbText;
};
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



// CSynStatic

class AFX_EXT_CLASS CSynStatic : public CStatic
{
	DECLARE_DYNAMIC(CSynStatic)

public:
	CSynStatic();
	virtual ~CSynStatic();

	virtual BOOL PreTranslateMessage(MSG* pMsg);

protected:
	afx_msg LRESULT OnSetText(WPARAM,LPARAM);
	afx_msg HBRUSH CtlColor(CDC* /*pDC*/, UINT /*nCtlColor*/);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	//afx_msg void OnTimer(UINT_PTR nIDEvent);
	DECLARE_MESSAGE_MAP()

//	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);


public:
	enum enType{typeTB=0, typeLR, typeTCB, typeLCR};
	enum enTypeGradient{typeDefault = 0, typeBlue, typeOliveGreen, typeSilver, typeRed};

	int SetFontSize(int nHeight, CString strFontName=_T(""));
	int SetForeColor(COLORREF rgbFont);
	int SetBackColor(COLORREF rgbBkgnd);
	int SetBackGradientColor(COLORREF rgb1, COLORREF rgb2, int nType=0);

	int SetBackGradientColorType(int nType);

	int AddChild(CWnd* pWnd);
	int RemoveChild(CWnd* pWnd);
	int RemoveAllChild();

protected:
	LOGFONT		m_LogFont;
	CFont		m_Font;

	COLORREF	m_rgbText;
	COLORREF	m_rgbBkgnd;
	COLORREF	m_rgbBkgnd2;

	CRect		m_rtBk;
	CRect		m_rtDraw;

	long		m_lDC;

	int			m_nGradientType;

	int CreateBkBmp(CDC* pDC);
	int CreateBkGradientBmp(CDC* pDC);

	CDC*		m_pOldDC;
	CRect		m_rtWnd;

	CPtrList	m_listWnd;

	int InvalidateChild();

private:
   CBitmap m_Bmp;
   BOOL		m_bDraw;
public:
	afx_msg void OnDestroy();
};


