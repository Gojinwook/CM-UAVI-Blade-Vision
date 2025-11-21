// DefectListDlg.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "uScan.h"
#include "DefectListDlg.h"
#include "afxdialogex.h"


// CDefectListDlg 대화 상자입니다.
CDefectListDlg* CDefectListDlg::m_pInstance = NULL;

CDefectListDlg* CDefectListDlg::GetInstance(BOOL bShowFlag)
{
	if (!m_pInstance) {
		m_pInstance = new CDefectListDlg();
		if(!m_pInstance->m_hWnd) {
			CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
			m_pInstance->Create(IDD_DEFECT_LIST_DLG, pFrame->GetActiveView());
			if (bShowFlag) m_pInstance->Show();
		}
	}
	return m_pInstance;
}
void CDefectListDlg::DeleteInstance()
{
	if(m_pInstance->GetSafeHwnd())
		m_pInstance->DestroyWindow();
	SAFE_DELETE(m_pInstance);
}

void CDefectListDlg::Show()
{
	ShowWindow(SW_HIDE);
	MoveWindow(m_ScreenRect.left, m_ScreenRect.top, m_ScreenRect.right, m_ScreenRect.bottom);
	ShowWindow(SW_SHOW);
}
IMPLEMENT_DYNAMIC(CDefectListDlg, CDialog)

CDefectListDlg::CDefectListDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CDefectListDlg::IDD, pParent)
{
	SetPosition(VIEW1_DLG3_LEFT+70, VIEW1_DLG3_TOP+40, VIEW1_DLG3_WIDTH-570, VIEW1_DLG3_HEIGHT+280);
}

CDefectListDlg::~CDefectListDlg()
{
}

void CDefectListDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_DEFECT_LIST, m_conDefectList);
}


BEGIN_MESSAGE_MAP(CDefectListDlg, CDialog)
	ON_WM_WINDOWPOSCHANGING()
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_DEFECT_LIST, &CDefectListDlg::OnLvnItemchangedDefectList)
END_MESSAGE_MAP()


// CDefectListDlg 메시지 처리기입니다.


BOOL CDefectListDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_conDefectList.DeleteAllItems();

	m_conDefectList.InsertColumn(0, _T("Module No."), LVCFMT_CENTER, 50,-1);
	m_conDefectList.InsertColumn(1, _T("검사항목"), LVCFMT_CENTER, 60,-1);	
	m_conDefectList.InsertColumn(2, _T("불량 유형"), LVCFMT_CENTER, 70,-1);
	m_conDefectList.InsertColumn(3, _T("면적"), LVCFMT_CENTER, 60,-1);
	m_conDefectList.InsertColumn(4, _T("폭"), LVCFMT_CENTER, 60,-1);	
	m_conDefectList.InsertColumn(5, _T("길이"), LVCFMT_CENTER, 60,-1);	
	m_conDefectList.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
	m_conDefectList.ModifyStyle(LVS_TYPEMASK,LVS_REPORT);

	
	m_bDefectListDisplay = FALSE;

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}


void CDefectListDlg::OnWindowPosChanging(WINDOWPOS* lpwndpos)
{
	CDialog::OnWindowPosChanging(lpwndpos);

	 MoveWindow(m_ScreenRect.left, m_ScreenRect.top, m_ScreenRect.right, m_ScreenRect.bottom);
	 lpwndpos->flags |= SWP_NOMOVE; 
 CDialog::OnWindowPosChanging(lpwndpos); 
}


void CDefectListDlg::DefectSummury(Hobject HDefectRgn, int No_Module, int iBarrelOrLens)
{
	try{
	////////////////////////////////////////////////// 리스트 컨트롤 예제
	
	m_bDefectListDisplay = TRUE;

	Hobject SelectRgn;
	HTuple Area,Row,Column;
	HTuple CountDefect;
	Hobject ConnectionsRegion;
	
	m_iBarrelOrLens = iBarrelOrLens;



	LV_ITEM Item;
	CString Temp;

	if(THEAPP.m_pGFunction->ValidHRegion(HDefectRgn))
	{
		connection(HDefectRgn,&ConnectionsRegion);
		count_obj(ConnectionsRegion,&CountDefect);

		for(int i=0;i<CountDefect[0].L();i++)
		{
			select_obj(ConnectionsRegion,&SelectRgn,i+1);
			area_center(SelectRgn,&Area,&Row,&Column);

			if(Area[0].L()>0)
			{
				Item.mask = LVIF_TEXT;
				Item.iItem = i;
				Item.iSubItem = 0;
				Temp.Format("%d",No_Module);
				Item.pszText = (LPTSTR)(LPCSTR)Temp;
				m_conDefectList.InsertItem(&Item);
			
				Item.mask = LVIF_TEXT;
				Item.iItem = i;
				Item.iSubItem = 1;
				if(iBarrelOrLens==1)
				{Temp.Format("배럴불량");}
				if(iBarrelOrLens==2)
				{Temp.Format("렌즈불량");}
				Item.pszText = (LPTSTR)(LPCSTR)Temp;
				m_conDefectList.SetItem(&Item);
				
				Item.mask = LVIF_TEXT;
				Item.iItem = i;
				Item.iSubItem = 3;
				Temp.Format("%d",Area[0].L());
				Item.pszText = (LPTSTR)(LPCSTR)Temp;
				m_conDefectList.SetItem(&Item);
			}
		}
	}
}
	catch(HException &except)
	{
		CString str;
		str.Format("Halcon Exception [DefectSummury Matching Model] : %s", except.message);
		THEAPP.SaveLog(str);
		return;
	}
}

void CDefectListDlg::OnLvnItemchangedDefectList(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	*pResult = 0;
}


BOOL CDefectListDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	if(pMsg->message==WM_KEYDOWN&&pMsg->wParam==VK_ESCAPE)
		return TRUE;
	if(pMsg->message==WM_KEYDOWN&&pMsg->wParam==VK_RETURN)
		return TRUE;
	return CDialog::PreTranslateMessage(pMsg);
}
