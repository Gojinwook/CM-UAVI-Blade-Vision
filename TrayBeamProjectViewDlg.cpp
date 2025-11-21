// TrayBeamProjectViewDlg.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "uScan.h"
#include "TrayBeamProjectViewDlg.h"
#include "afxdialogex.h"

// CTrayBeamProjectViewDlg 대화 상자입니다.
CTrayBeamProjectViewDlg* CTrayBeamProjectViewDlg::m_pInstance = NULL;

CTrayBeamProjectViewDlg* CTrayBeamProjectViewDlg::GetInstance(BOOL bShowFlag)
{
	if (!m_pInstance) {
		m_pInstance = new CTrayBeamProjectViewDlg();
		if(!m_pInstance->m_hWnd) {
			CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
			m_pInstance->Create(IDD_TRAYVIEW_BEAMPROJECT_DLG, pFrame->GetActiveView());
			if (bShowFlag) m_pInstance->Show();
		}
	}
	return m_pInstance;
}
void CTrayBeamProjectViewDlg::DeleteInstance()
{
	if(m_pInstance->GetSafeHwnd())
		m_pInstance->DestroyWindow();
	SAFE_DELETE(m_pInstance);
}

void CTrayBeamProjectViewDlg::Show()
{
	ShowWindow(SW_HIDE);

	
	SetPosition(VIEW1_DLG1_LEFT, VIEW1_DLG1_TOP-150, THEAPP.m_pModelDataManager->m_iTrayHESizeX, THEAPP.m_pModelDataManager->m_iTrayHESizeY);
	MoveWindow(m_ScreenRect.left, m_ScreenRect.top, m_ScreenRect.right, m_ScreenRect.bottom);


	this->GetClientRect( &DisplayRect );
	DisplayRect.NormalizeRect();
	this->ClientToScreen( &DisplayRect );	//picture control size
	ScreenToClient( &DisplayRect);

	DisplayRect = m_TrayImagePB.SetPictureBoxSize(DisplayRect);

	MakeTrayRectangle();

	



	ShowWindow(SW_SHOW);
}
IMPLEMENT_DYNAMIC(CTrayBeamProjectViewDlg, CDialog)

CTrayBeamProjectViewDlg::CTrayBeamProjectViewDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CTrayBeamProjectViewDlg::IDD, pParent)
{
		SetPosition(VIEW1_DLG1_LEFT, VIEW1_DLG1_TOP, VIEW1_DLG1_WIDTH+20, VIEW1_DLG1_HEIGHT+380);
}

CTrayBeamProjectViewDlg::~CTrayBeamProjectViewDlg()
{
}

void CTrayBeamProjectViewDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STATIC_DISPLAY_VIEW, m_TrayImagePB);
}


BEGIN_MESSAGE_MAP(CTrayBeamProjectViewDlg, CDialog)
	ON_WM_CTLCOLOR()
END_MESSAGE_MAP()


// CTrayBeamProjectViewDlg 메시지 처리기입니다.


BOOL CTrayBeamProjectViewDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	if(pMsg->message==WM_KEYDOWN&&pMsg->wParam==VK_ESCAPE)
		return TRUE;
	if(pMsg->message==WM_KEYDOWN&&pMsg->wParam==VK_RETURN)
		return TRUE;
	return CDialog::PreTranslateMessage(pMsg);
}


HBRUSH CTrayBeamProjectViewDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);

	CRect rect; 
	UINT nID = pWnd->GetDlgCtrlID();

	if(nCtlColor == CTLCOLOR_STATIC)

	{     
		switch(nID)
		{
		case IDC_STATIC:
			{
				pDC->SetTextColor(RGB(255, 0, 255));
				pDC->SetBkColor(RGB(240, 240, 240));
				break;
			}
		}

	}
	return hbr;
}
CFont* CTrayBeamProjectViewDlg::GetFont(LONG lfWidth, LONG lfHeight, LONG lfWeight)
 {
        CFont* pFont;
        LOGFONT logFont;

        pFont = new CFont();
        memset(&logFont, 0, sizeof(LOGFONT));
        logFont.lfWidth = lfWidth;
         logFont.lfHeight = lfHeight;
         logFont.lfWeight = lfWeight;
         _tcscpy( logFont.lfFaceName, _T("사용자글자") );
         pFont->CreateFontIndirectA(&logFont);
        return pFont;        
}
void CTrayBeamProjectViewDlg::MakeTrayRectangle()
{
		if(m_TrayImagePB.WindowHandle>-1)
	{
		close_window(m_TrayImagePB.WindowHandle);
	}
	
	

	double width,height;
	width =70*THEAPP.m_pModelDataManager->m_iTrayArrayX;
	height =  70*THEAPP.m_pModelDataManager->m_iTrayArrayY;

	

	open_window(0,0,DisplayRect.Width(), DisplayRect.Height(), (long)GetDlgItem(IDC_STATIC_DISPLAY_VIEW)->m_hWnd, "visible", "", &DispayViewWindowHandle);
	


	gen_image_const(&Image, "byte", width, height);

	partition_rectangle(Image, &Partitioned, width/THEAPP.m_pModelDataManager->m_iTrayArrayX, height/THEAPP.m_pModelDataManager->m_iTrayArrayY );
	erosion_rectangle1(Partitioned, &RegionErosion, 15, 15);

	set_part(DispayViewWindowHandle,0,0,height,width);

	disp_obj(RegionErosion, DispayViewWindowHandle);

	m_TrayImagePB.RegionErosion = RegionErosion;
	m_TrayImagePB.WindowHandle = DispayViewWindowHandle;
}
