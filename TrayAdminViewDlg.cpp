// TrayViewDlg1.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "uScan.h"
#include "TrayAdminViewDlg.h"
#include "afxdialogex.h"


// CTrayAdminViewDlg 대화 상자입니다.

CTrayAdminViewDlg* CTrayAdminViewDlg::m_pInstance = NULL;

CTrayAdminViewDlg* CTrayAdminViewDlg::GetInstance(BOOL bShowFlag)
{
	if (!m_pInstance) {
		m_pInstance = new CTrayAdminViewDlg();
		if(!m_pInstance->m_hWnd) {
			CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
			m_pInstance->Create(IDD_TRAYVIEW_ADMIN_DLG, pFrame->GetActiveView());
			if (bShowFlag) m_pInstance->Show();
		}
	}
	return m_pInstance;
}
void CTrayAdminViewDlg::DeleteInstance()
{
	if(m_pInstance->GetSafeHwnd())
		m_pInstance->DestroyWindow();
	SAFE_DELETE(m_pInstance);
}

void CTrayAdminViewDlg::Show()
{
	ShowWindow(SW_HIDE);
	MoveWindow(m_ScreenRect.left, m_ScreenRect.top, m_ScreenRect.right, m_ScreenRect.bottom);


	this->GetClientRect( &DisplayRect );
	DisplayRect.NormalizeRect();
	this->ClientToScreen( &DisplayRect );	//picture control size
	ScreenToClient( &DisplayRect);
	DisplayRect = m_TrayImagePB.SetPictureBoxSize(DisplayRect);
	
	MakeTrayRectangle();

	ShowWindow(SW_SHOW);
}


IMPLEMENT_DYNAMIC(CTrayAdminViewDlg, CDialog)

CTrayAdminViewDlg::CTrayAdminViewDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CTrayAdminViewDlg::IDD, pParent)
{
	SetPosition(VIEW1_DLG3_LEFT+520, VIEW1_DLG1_TOP+480, VIEW1_DLG3_WIDTH-530, VIEW1_DLG1_HEIGHT-110);
}

CTrayAdminViewDlg::~CTrayAdminViewDlg()
{

}

void CTrayAdminViewDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STATIC_DISPLAY_VIEW, m_TrayImagePB);
	
}


BEGIN_MESSAGE_MAP(CTrayAdminViewDlg, CDialog)
//	ON_WM_LBUTTONDOWN()
	ON_WM_WINDOWPOSCHANGING()
	ON_BN_CLICKED(IDC_BUTTONOPEN_PREV_TRAY, &CTrayAdminViewDlg::OnBnClickedButtonopenPrevTray)
	ON_WM_PAINT()
END_MESSAGE_MAP()


// CTrayAdminViewDlg 메시지 처리기입니다.


BOOL CTrayAdminViewDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_bLoadTray = TRUE;
	m_bPrevTray = TRUE;

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

void CTrayAdminViewDlg::OnWindowPosChanging(WINDOWPOS* lpwndpos)
{
		 MoveWindow(m_ScreenRect.left, m_ScreenRect.top, m_ScreenRect.right, m_ScreenRect.bottom);
	 lpwndpos->flags |= SWP_NOMOVE; 
 CDialog::OnWindowPosChanging(lpwndpos); 
}

void CTrayAdminViewDlg::OnBnClickedButtonopenPrevTray()
{
	if(m_bLoadTray==TRUE)
	{
		if(m_bPrevTray)
		{
			THEAPP.m_pInspectAdminViewDlg->ShowWindow(SW_SHOWNA);
			THEAPP.m_pInspectViewBarrelSurfaceDlg->Show();
			THEAPP.m_pInspectViewBarrelEdgeDlg->Show();
			THEAPP.m_pInspectViewLensInnerDlg->Show();
			THEAPP.m_pInspectViewLensOuterDlg->Show();

			if (THEAPP.m_iMachineInspType == MACHINE_SIDEFILL ||
				THEAPP.m_iMachineInspType == MACHINE_FIDUCIAL ||
				THEAPP.m_iMachineInspType == MACHINE_BRACKET)
			{
				THEAPP.m_pInspectViewExtra1Dlg->Show();
				THEAPP.m_pInspectViewExtra2Dlg->Show();
			}

			THEAPP.m_pInspectViewOverayImageDlg->Show();
			m_bPrevTray = FALSE;

			THEAPP.m_bShowReviewWindow = TRUE;

		}
		else
		{
			THEAPP.m_pInspectAdminViewDlg->ShowWindow(SW_SHOWNA);
			THEAPP.m_pInspectViewBarrelSurfaceDlg->ShowWindow(SW_HIDE);
			THEAPP.m_pInspectViewBarrelEdgeDlg->ShowWindow(SW_HIDE);
			THEAPP.m_pInspectViewLensInnerDlg->ShowWindow(SW_HIDE);
			THEAPP.m_pInspectViewLensOuterDlg->ShowWindow(SW_HIDE);

			if (THEAPP.m_iMachineInspType == MACHINE_SIDEFILL ||
				THEAPP.m_iMachineInspType == MACHINE_FIDUCIAL ||
				THEAPP.m_iMachineInspType == MACHINE_BRACKET)
			{
				THEAPP.m_pInspectViewExtra1Dlg->ShowWindow(SW_HIDE);
				THEAPP.m_pInspectViewExtra2Dlg->ShowWindow(SW_HIDE);
			}

			THEAPP.m_pInspectViewOverayImageDlg->ShowWindow(SW_HIDE);
			m_bPrevTray = TRUE;

			THEAPP.m_bShowReviewWindow = FALSE;

		}
	}
}
void CTrayAdminViewDlg::UpdateTrayDisplay()
{
	InvalidateRect(false);
}

void CTrayAdminViewDlg::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	// 그리기 메시지에 대해서는 CDialog::OnPaint()을(를) 호출하지 마십시오.
}


BOOL CTrayAdminViewDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	if(pMsg->message==WM_KEYDOWN&&pMsg->wParam==VK_ESCAPE)
		return TRUE;
	if(pMsg->message==WM_KEYDOWN&&pMsg->wParam==VK_RETURN)
		return TRUE;
	return CDialog::PreTranslateMessage(pMsg);
}

void CTrayAdminViewDlg::MakeTrayRectangle()
{
	if(m_TrayImagePB.WindowHandle>-1)
	{
		close_window(m_TrayImagePB.WindowHandle);
	}

	double width,height;
	width = 70*THEAPP.m_pModelDataManager->m_iTrayArrayX;
	height = 70*THEAPP.m_pModelDataManager->m_iTrayArrayY;

	open_window(0,0,DisplayRect.Width(), DisplayRect.Height(), (long)GetDlgItem(IDC_STATIC_DISPLAY_VIEW)->m_hWnd, "visible", "", &DispayViewWindowHandle);
	
	gen_image_const(&Image, "byte", width, height);
	partition_rectangle(Image, &Partitioned, width/THEAPP.m_pModelDataManager->m_iTrayArrayX, height/THEAPP.m_pModelDataManager->m_iTrayArrayY );
	erosion_rectangle1(Partitioned, &RegionErosion, 15, 15);

	set_part(DispayViewWindowHandle,0,0,height,width);

	disp_obj(RegionErosion, DispayViewWindowHandle);

	m_TrayImagePB.RegionErosion = RegionErosion;
	m_TrayImagePB.WindowHandle = DispayViewWindowHandle;
}
