// HWinPanel.cpp : implementation file
//

#include "stdafx.h"
#include "uScan.h"
#include "HWinPanel.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CHWinPanel

CHWinPanel::CHWinPanel()
{
	mhDC = 0;
	mpHWindow = 0;
	mlWindowHandle = 0;
}

CHWinPanel::~CHWinPanel()
{
	DestroyHWindow();
}

void CHWinPanel::CreateHWindow()
{
	RECT rect;
	mhDC = ::GetDC(m_hWnd);
	::GetClientRect(m_hWnd, &rect);
	window_width = rect.right - rect.left + 1;
	window_height = rect.bottom - rect.top + 1;

	new_extern_window((Hlong)m_hWnd, 0, 0, window_width, window_height, &mlWindowHandle);

	Hlong lDC;
	lDC = (Hlong)(mhDC);
	set_window_dc(mlWindowHandle, lDC);
}

void CHWinPanel::DestroyHWindow()
{
	if(mpHWindow)  ///Moon 20091130
	{
		delete mpHWindow;   
		mpHWindow = NULL;
	}

	// Add by jaehuns 2009.03.20 because Main S/W down
	::ReleaseDC(m_hWnd, mhDC);
}

BEGIN_MESSAGE_MAP(CHWinPanel, CStatic)
	//{{AFX_MSG_MAP(CHWinPanel)
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CHWinPanel message handlers

void CHWinPanel::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	
	// TODO: Add your message handler code here
	
	// Do not call CStatic::OnPaint() for painting messages
}
