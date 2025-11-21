// TrayOperatorViewDlg2.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "uScan.h"
#include "TrayOperatorViewDlg2.h"
#include "afxdialogex.h"


// CTrayOperatorViewDlg2 대화 상자입니다.
CTrayOperatorViewDlg2* CTrayOperatorViewDlg2::m_pInstance = NULL;

CTrayOperatorViewDlg2* CTrayOperatorViewDlg2::GetInstance(BOOL bShowFlag)
{
	if (!m_pInstance) {
		m_pInstance = new CTrayOperatorViewDlg2();
		if(!m_pInstance->m_hWnd) {
			CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
			m_pInstance->Create(IDD_TRAYVIEW_OPERATOR_DLG2, pFrame->GetActiveView());
			if (bShowFlag) m_pInstance->Show();
		}
	}
	return m_pInstance;
}
void CTrayOperatorViewDlg2::DeleteInstance()
{
	if(m_pInstance->GetSafeHwnd())
		m_pInstance->DestroyWindow();
	SAFE_DELETE(m_pInstance);
}

void CTrayOperatorViewDlg2::Show()
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
IMPLEMENT_DYNAMIC(CTrayOperatorViewDlg2, CDialog)

CTrayOperatorViewDlg2::CTrayOperatorViewDlg2(CWnd* pParent /*=NULL*/)
	: CDialog(CTrayOperatorViewDlg2::IDD, pParent)
{
	SetPosition(VIEW1_DLG1_LEFT+VIEW1_DLG1_WIDTH+15, VIEW1_DLG1_TOP, VIEW1_DLG1_WIDTH+25, VIEW1_DLG1_HEIGHT+380);
}

CTrayOperatorViewDlg2::~CTrayOperatorViewDlg2()
{
}

void CTrayOperatorViewDlg2::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STATIC_DISPLAY_VIEW, m_TrayImagePB);
}


BEGIN_MESSAGE_MAP(CTrayOperatorViewDlg2, CDialog)
	ON_WM_PAINT()
	ON_WM_CTLCOLOR()
END_MESSAGE_MAP()


// CTrayOperatorViewDlg2 메시지 처리기입니다.
void CTrayOperatorViewDlg2::UpdateTrayDisplay()
{

}

void CTrayOperatorViewDlg2::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	// 그리기 메시지에 대해서는 CDialog::OnPaint()을(를) 호출하지 마십시오.

}


BOOL CTrayOperatorViewDlg2::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.

	switch(pMsg->message) {
	case WM_NCLBUTTONDOWN :
	case WM_NCRBUTTONDOWN :
		SetActiveWindow();
		return TRUE;
	case WM_KEYDOWN:
		if((int)pMsg->wParam == VK_ESCAPE || (int)pMsg->wParam == VK_RETURN)
		{
			return TRUE;
		}
	case WM_SYSKEYDOWN:
		if(pMsg->wParam == VK_F4)
			return TRUE;
	}
	return CDialog::PreTranslateMessage(pMsg);
}


HBRUSH CTrayOperatorViewDlg2::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
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
				pDC->SetTextColor(RGB(0, 0, 255));
				pDC->SetBkColor(RGB(240, 240, 240));
				break;
			}
		}

	}
	return hbr;
}
CFont* CTrayOperatorViewDlg2::GetFont(LONG lfWidth, LONG lfHeight, LONG lfWeight)
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
void CTrayOperatorViewDlg2::MakeTrayRectangle()
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

	CFont* pFont;
    LONG lfWidth = 15, lfHeight = 40, lfWeight = FW_SEMIBOLD;
    pFont = GetFont(lfWidth, lfHeight, lfWeight);
	GetDlgItem(IDC_STATIC)->SetFont(pFont);
	pFont->DeleteObject();
    pFont->Detach();
    delete pFont;   
}