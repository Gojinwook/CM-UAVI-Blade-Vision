// MainFrm.cpp : implementation of the CMainFrame class
//

#include "stdafx.h"
#include "uScan.h"

#include "MainFrm.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMainFrame

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWnd)

	BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
		//{{AFX_MSG_MAP(CMainFrame)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code !
		ON_WM_CREATE()
		//}}AFX_MSG_MAP
		ON_MESSAGE(WM_END_REVIEW, OnEventEndReview)
		ON_MESSAGE(WM_LIGHT_REVIEW, OnEventLightReview)
		ON_MESSAGE(WM_SAVEIMAGE_DONE, OnEventReviewCamSave)
		ON_WM_GETMINMAXINFO()
	END_MESSAGE_MAP()

	static UINT indicators[] =
	{
		ID_SEPARATOR,           // status line indicator
		ID_INDICATOR_CAPS,
		ID_INDICATOR_NUM,
		ID_INDICATOR_SCRL,
	};

	/////////////////////////////////////////////////////////////////////////////
	// CMainFrame construction/destruction

	CMainFrame::CMainFrame()
	{
		// TODO: add member initialization code here

	}

	CMainFrame::~CMainFrame()
	{
	}

	int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
	{
		if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
			return -1;
		/*
		if (!m_wndToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP
		| CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_wndToolBar.LoadToolBar(IDR_MAINFRAME))
		{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
		}

		if (!m_wndStatusBar.Create(this) ||
		!m_wndStatusBar.SetIndicators(indicators,
		sizeof(indicators)/sizeof(UINT)))
		{
		TRACE0("Failed to create status bar\n");
		return -1;      // fail to create
		}

		// TODO: Delete these three lines if you don't want the toolbar to
		//  be dockable

		m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
		EnableDocking(CBRS_ALIGN_ANY);
		DockControlBar(&m_wndToolBar);
		*/

		SetWindowText(_T("CMI"));

		return 0;
	}

	BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
	{
		if( !CFrameWnd::PreCreateWindow(cs) )
			return FALSE;

		// TODO: Modify the Window class or styles here by modifying
		//  the CREATESTRUCT cs
		cs.cx = ::GetSystemMetrics(SM_CXSCREEN);
		cs.cy = ::GetSystemMetrics(SM_CYSCREEN);

		cs.style ^= WS_BORDER | WS_DLGFRAME | WS_CAPTION | WS_POPUP | FWS_ADDTOTITLE;
		cs.dwExStyle ^= WS_EX_WINDOWEDGE;

		cs.style &= ~WS_THICKFRAME;     //크기를 늘렸다 줄렸다하는 기능을 비활성화

		cs.hMenu = NULL;

		return TRUE;
	}

	/////////////////////////////////////////////////////////////////////////////
	// CMainFrame diagnostics

#ifdef _DEBUG
	void CMainFrame::AssertValid() const
	{
		CFrameWnd::AssertValid();
	}

	void CMainFrame::Dump(CDumpContext& dc) const
	{
		CFrameWnd::Dump(dc);
	}

#endif //_DEBUG

	/////////////////////////////////////////////////////////////////////////////
	// CMainFrame message handlers

	LRESULT CMainFrame::OnEventEndReview(WPARAM wParam, LPARAM lParam)
	{


		return 0;
	}

	LRESULT CMainFrame::OnEventLightReview(WPARAM wParam, LPARAM lParam)
	{



		return 0;
	}

	LRESULT CMainFrame::OnEventReviewCamSave(WPARAM wParam, LPARAM lParam)
	{

		return 0;
	}



	void CMainFrame::OnGetMinMaxInfo(MINMAXINFO* lpMMI)
	{
		// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
		lpMMI->ptMinTrackSize.x = 1920;
		lpMMI->ptMinTrackSize.y = 1080;

		lpMMI->ptMaxTrackSize.x = 1920;
		lpMMI->ptMaxTrackSize.y = 1080;

		CFrameWnd::OnGetMinMaxInfo(lpMMI);
	}
