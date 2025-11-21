#if !defined(AFX_GFUNCTION_H__1873C685_BB94_4358_BA13_FFFDF6AFF4F8__INCLUDED_)
#define AFX_GFUNCTION_H__1873C685_BB94_4358_BA13_FFFDF6AFF4F8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// GFunction.h : header file
//

#include "HalconCpp.h"

#ifdef H_CPP_H
	#if H_VERSION > 6
		using namespace Halcon;
	#endif
#endif

#include "GViewportManager.h"

/////////////////////////////////////////////////////////////////////////////
// CGFunction window

class CGFunction : public CWnd
{
// Construction
public:
	CGFunction();

	static CGFunction* GetInstance(BOOL bShowFlag=FALSE);
	void   DeleteInstance();

// Attributes
public:

// Operations
public:
	// Halcon Functions
	static BOOL ValidHImage(const Hobject &rHObject);
	static BOOL ValidHRegion(const Hobject &rHObject);
	static BOOL ValidHXLD(const Hobject &rHObject);
	void DisplayCross(HTuple lWindowHandle, double dXPos, double dYPos, COLORREF Color, double dRadius=5.0);
	void DisplayCross(HDC hDestDC, GViewportManager *pVManager, double dXPos, double dYPos, COLORREF Color, long lRadiusPxl=5);
	BOOL GetIntersectionRegion(Hobject HInputRgn, Hobject HTestRgn, Hobject* pHIntersectRgn, long lMinCheckSize=3);
	void ScaleImageRange (Hobject Image, Hobject *ImageScaled, HTuple Min, HTuple Max);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CGFunction)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CGFunction();

	// Generated message map functions
protected:
	//{{AFX_MSG(CGFunction)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	static CGFunction*	m_pInstance;
	CRect			m_ScreenRect;

	CRect  GetPosition() { return m_ScreenRect; }
	void   SetPosition(int left, int top, int right, int bottom) { m_ScreenRect = CRect(left, top, right, bottom); }
};

class DRECT  
{
public:
	DRECT();
	virtual ~DRECT();

	BOOL PtInRect(DPOINT dPoint);

	double left;
	double top;
	double right;
	double bottom;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_GFUNCTION_H__1873C685_BB94_4358_BA13_FFFDF6AFF4F8__INCLUDED_)
