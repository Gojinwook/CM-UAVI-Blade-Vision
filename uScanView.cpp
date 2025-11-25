// uScanView.cpp : implementation of the CuScanView class
//

#include "stdafx.h"
#include "uScan.h"

#include "uScanDoc.h"
#include "uScanView.h"
#include "LoadModelDlg.h"

#include "AdminPasswordDlg.h"
#include "Operator_RegisterDlg.h"
#include "IniFileCS.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char* THIS_FILE = __FILE__;
#endif


UINT DiskCleanCheckThread(LPVOID lp)
{
	CuScanView* puScanView = (CuScanView*) lp;
	THEAPP.CleanDiskbyDay();

	return 0;
}

/////////////////////////////////////////////////////////////////////////////
// CuScanView

IMPLEMENT_DYNCREATE(CuScanView, CFormView)

BEGIN_MESSAGE_MAP(CuScanView, CFormView)
	//{{AFX_MSG_MAP(CuScanView)
	ON_WM_DESTROY()
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, CFormView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, CFormView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, CFormView::OnFilePrintPreview)
	ON_BN_CLICKED(IDC_MFCBUTTON_FORM_NEW, &CuScanView::OnBnClickedMfcbuttonFormNew)
	ON_BN_CLICKED(IDC_MFCBUTTON_FORM_LOAD, &CuScanView::OnBnClickedMfcbuttonFormLoad)
	ON_BN_CLICKED(IDC_MFCBUTTON_FORM_SAVE, &CuScanView::OnBnClickedMfcbuttonFormSave)
	ON_BN_CLICKED(IDC_MFCBUTTON_FORM_TEACHING, &CuScanView::OnBnClickedMfcbuttonFormTeaching)
	ON_BN_CLICKED(IDC_MFCBUTTON_FORM_PREFERENCE, &CuScanView::OnBnClickedMfcbuttonFormPreference)
	ON_BN_CLICKED(IDC_MFCBUTTON_FORM_EXIT, &CuScanView::OnBnClickedMfcbuttonFormExit)
	ON_BN_CLICKED(IDC_BUTTON_MODE_CHANGE, &CuScanView::OnBnClickedButtonModeChange)
	ON_BN_CLICKED(IDC_BUTTON_TRAY_LOADING, &CuScanView::OnBnClickedButtonTrayLoading)
	ON_BN_CLICKED(IDC_BUTTON_TRAY_UNLOADING, &CuScanView::OnBnClickedButtonTrayUnloading)
	ON_BN_CLICKED(IDC_MFCBUTTON_FORM_REGISTER, &CuScanView::OnBnClickedMfcbuttonFormRegister)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CuScanView construction/destruction

CuScanView::CuScanView()
	: CFormView(CuScanView::IDD)
{
	//{{AFX_DATA_INIT(CuScanView)
	//}}AFX_DATA_INIT
	// TODO: add construction code here
	m_bTeachingShow = FALSE;
}

CuScanView::~CuScanView()
{
		
}

void CuScanView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STATIC_SW_VERSION, m_StaticSPIVersion);
	DDX_Control(pDX, IDC_STATIC_FORM_CURRENT_STATUS, m_LabelCurMode);
}

void CuScanView::OnInitialUpdate()
{
	CFormView::OnInitialUpdate();
	GetParentFrame()->RecalcLayout();
	ResizeParentToFit();

	m_StartTime = CTime::GetCurrentTime();

	SetStatusText(2);	// Date Time
	SetStatusText(3);	// Run Time

	CRect rect;
	CStatic *pStaticMainTitle = (CStatic *)GetDlgItem(IDC_STATIC_MAIN_TITLE);
	pStaticMainTitle->GetWindowRect(rect);
	pStaticMainTitle->MoveWindow(rect.left, rect.top, 1918, rect.Height());
	pStaticMainTitle->ShowWindow(TRUE);

	SetTimer(0, 1000, NULL);
	SetTimer(3, 60000, NULL);

	ShowVersionText();

	m_LabelCurMode.Init_Ctrl(_T("Arial Black"), 20, TRUE, BLACK, WHITE);

	m_bStatusToggle = FALSE;
}

void CuScanView::ShowVersionText()
{
	char AppFilePath[_MAX_PATH*2];
	strcpy(AppFilePath, AfxGetApp()->m_pszHelpFilePath);

	int slength = strlen(AppFilePath);
	strcpy(AppFilePath+(slength-4), ".exe");

	DWORD Dummy = NULL;
	BOOL bResult = FALSE;
	HRESULT hResult = S_OK;
	DWORD dwLen = 0;
	DWORD dwError = 0;
	PVOID pVersionData = NULL;
	PVOID pVersionInfo = NULL;

	// 버전정보의 크기를 알아 내고 데이터 버퍼를 할당한다.
	// 파일의 버전정보의 크기가 파일마다 제각기 다르기 땜시...
	dwLen = ::GetFileVersionInfoSize(AppFilePath, &Dummy);
	if (dwLen == 0) 
	{
		return;
	}
	// 버전 정보를 담을 버퍼를 할당한다. (수천 바이트가 필요하다)
	pVersionData = ::HeapAlloc(::GetProcessHeap(), 0, dwLen);
	ZeroMemory(pVersionData, dwLen);

	// 버전 정보 데이터를 읽는다.
	bResult = ::GetFileVersionInfo(AppFilePath, NULL, dwLen, pVersionData);
	if (bResult == FALSE) 
	{
		dwError = GetLastError();
		::HeapFree(::GetProcessHeap(), 0, pVersionData);
		return;
	}

	// VS_FIXEDFILEINFO 로 부터 버전정보를 읽는다.
	// (숫자로 된 버전정보는 이 구조체 안에 정보가 담긴다)
	bResult = ::VerQueryValue(pVersionData, TEXT("\\"), &pVersionInfo, (PUINT)&dwLen);
	if (bResult == FALSE) 
	{
		dwError = GetLastError();
		::HeapFree(::GetProcessHeap(), 0, pVersionData);
		return;
	}
	VS_FIXEDFILEINFO *pFileInfo = (VS_FIXEDFILEINFO *)pVersionInfo;
	// 숫자로된 버전 정보를 알아냈으니 필요한 대로 쓰문 되겠다.
	DWORD dwMajorVer = HIWORD(pFileInfo->dwFileVersionMS);
	DWORD dwMinorVer = LOWORD(pFileInfo->dwFileVersionMS);
	DWORD dwReleaseVer = HIWORD(pFileInfo->dwFileVersionLS);
	DWORD dwBuildVer = LOWORD(pFileInfo->dwFileVersionLS);

	CString strVer;
	strVer.Format("Ver %d.%d.%d.%d (%s)", dwMajorVer, dwMinorVer, dwReleaseVer, dwBuildVer, THEAPP.Struct_PreferenceStruct.m_sProductName); 

	m_StaticSPIVersion.Init_Ctrl(_T("Arial"), 13, TRUE, WHITE, TRANSPARENT);

	m_StaticSPIVersion.SetWindowText(strVer);

}

/////////////////////////////////////////////////////////////////////////////
// CuScanView diagnostics

#ifdef _DEBUG
void CuScanView::AssertValid() const
{
	CFormView::AssertValid();
}

void CuScanView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}

CuScanDoc* CuScanView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CuScanDoc)));
	return (CuScanDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CuScanView message handlers

LRESULT CuScanView::OnEventPcbModelChanaged(WPARAM wParam, LPARAM lParam)
{
	return 0;
}

LRESULT CuScanView::OnEventChangeMode(WPARAM wParam, LPARAM lParam)
{

	return 0;
}

LRESULT CuScanView::OnEventChangeStatus(WPARAM wParam, LPARAM lParam)
{

	return 0;
}

void CuScanView::SetStatusText(int idx, CString szMsg)
{
	CTime time = CTime::GetCurrentTime();

	switch (idx) {	// Modified Go 200511/21
	case 0:		// Model
		szMsg.Format("Model: %s", szMsg);
		GetDlgItem(IDC_STATIC_FORM_CURRENT_MODEL)->SetWindowText(szMsg);
		break;
	case 1:		// PCB Count
		szMsg.Format("%s", szMsg);
		SetTextPCBCount(szMsg);
		break;
	case 2:		// Current time
		szMsg = time.Format("%Y.%m.%d  %H:%M:%S");
		GetDlgItem(IDC_STATIC_FORM_DATE_TIME)->SetWindowText(szMsg);
		break;
	case 3:		// Runnig time
		CTimeSpan timespan = time - m_StartTime;
		szMsg.Format("Running time : %4ld:%02d:%02d", timespan.GetTotalHours(), timespan.GetMinutes(), timespan.GetSeconds());
		GetDlgItem(IDC_STATIC_FORM_RUN_TIME)->SetWindowText(szMsg);
		break;
	}
}

void CuScanView::OnDestroy() 
{
	CFormView::OnDestroy();


}

void CuScanView::SetButtonStatus(int nStatus)
{

}

void CuScanView::SetTextPCBCount(CString strPCBCount)
{


}

void CuScanView::SetExitProgram()
{
	try
	{
		try
		{
			KillTimer(0);
			KillTimer(3);

			AfxGetMainWnd()->DestroyWindow();
		}
		catch(CException &except)
		{
		}
	}
	catch(std::exception &except2)
	{}
}

void CuScanView::TeachingShow()
{	

}

void CuScanView::UpdateCurMode()
{
	if (THEAPP.m_pInspectSummary->m_bUseFobTest)
	{
		SetDlgItemText(IDC_STATIC_FORM_CURRENT_STATUS, "FOB 모드");
		SetTimer(2, 1000, NULL);
	}
	else
	{
		KillTimer(2);

		m_LabelCurMode.Init_Ctrl(_T("Arial Black"), 20, TRUE, BLACK, WHITE);

		SetDlgItemText(IDC_STATIC_FORM_CURRENT_STATUS, "검사 모드");
	}
}

void CuScanView::OnTimer(UINT_PTR nIDEvent) 
{
	switch(nIDEvent)
	{
	case 0:
		{
			SetStatusText(2);	// Current Time
			SetStatusText(3);	// Runnig time
			break;
		}
	case 1:
		{
		
		}
	case 2:
		{
			if (m_bStatusToggle)
			{
				m_LabelCurMode.Init_Ctrl(_T("Arial Black"), 20, TRUE, WHITE, RED);
				m_bStatusToggle = FALSE;
			}
			else
			{
				m_LabelCurMode.Init_Ctrl(_T("Arial Black"), 20, TRUE, RED, WHITE);
				m_bStatusToggle = TRUE;
			}

			break;
		}
	case 3:	// Disk Check
		{
			CheckDiskSpace();

			break;
		}
	}

	CFormView::OnTimer(nIDEvent);
}

void CuScanView::OnBnClickedMfcbuttonFormNew()
{
	if(m_pNewModelDlg.DoModal()==IDOK)
	{
		if(THEAPP.m_iModeSwitch==MODE_INSPECT_VIEW_ADMIN)  // 관리자 모드 아니면 화면 전환 안됨.
		{
			THEAPP.m_iModeSwitch = MODE_ADMIN_TEACH_VIEW;
			THEAPP.m_pInspectAdminViewDlg->ClearAll();
			THEAPP.m_pInspectAdminViewDlg->SetPosition(VIEW1_DLG1_LEFT, VIEW1_DLG1_TOP, VIEW1_DLG1_WIDTH+280, VIEW1_DLG1_HEIGHT+380);
			THEAPP.m_pInspectAdminViewDlg->Show();
			THEAPP.m_pInspectAdminViewHideDlg->ShowWindow(SW_HIDE);
			THEAPP.m_pDefectListDlg->ShowWindow(SW_HIDE);
			THEAPP.m_pTrayAdminViewDlg->ShowWindow(SW_HIDE);
			THEAPP.m_pLogDlg->ShowWindow(SW_HIDE);
			THEAPP.m_pInspectSummary->ShowWindow(SW_HIDE);
			THEAPP.m_pInspectResultDlg->ShowWindow(SW_HIDE);
			THEAPP.m_pTabControlDlg->Show();
			THEAPP.m_pTabControlDlg->ShowFirstPage();
			THEAPP.m_pTabControlDlg->LoadModelConditionParam();
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
			THEAPP.m_pTrayAdminViewDlg->m_bPrevTray=TRUE;
			THEAPP.m_bShowReviewWindow=FALSE;

#ifdef INLINE_MODE
			THEAPP.m_pHandlerService->Set_ModeUpdate(MS_TEACH);
#endif
		}
	}
}



void CuScanView::OnBnClickedMfcbuttonFormLoad()
{
	if(THEAPP.m_iModeSwitch==MODE_INSPECT_VIEW_ADMIN) // 관리자모드 아니면 로드안됨.
	{
		CLoadModelDlg dlgLoadModelDlg;
		if(dlgLoadModelDlg.DoModal()==IDOK)
		{
			THEAPP.m_pInspectSummary->GetDlgItem(IDC_MFCBUTTON_INSPECT_START)->EnableWindow(FALSE);

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
			THEAPP.m_pTrayAdminViewDlg->m_bPrevTray=TRUE;
			THEAPP.m_pModelDataManager->m_sModelName = dlgLoadModelDlg.m_EditSelectedModel;
			THEAPP.m_pModelDataManager->SaveChangeParamHistory(FALSE);
			THEAPP.m_pModelDataManager->LoadModel();
			THEAPP.m_bShowReviewWindow=FALSE;

			THEAPP.m_pInspectSummary->GetDlgItem(IDC_MFCBUTTON_INSPECT_START)->EnableWindow(TRUE);
			MessageBox("Model Load 완료");
		}
	}
}


void CuScanView::OnBnClickedMfcbuttonFormSave()
{
	if(THEAPP.m_iModeSwitch==MODE_ADMIN_TEACH_VIEW) // 티칭모드 아니면 저장안됨.
	{
		THEAPP.m_pModelDataManager->UpdatePolygonROIBoundary();

		//0531
		//THEAPP.m_pModelDataManager->UpdateParamChange();

		THEAPP.m_pInspectAdminViewDlg->ClearAutoFocusROI();
		THEAPP.m_pInspectAdminViewDlg->m_pThresRgn.Reset();
		THEAPP.m_pInspectAdminViewDlg->m_HInspectAreaRgn.Reset();
		THEAPP.m_pInspectAdminViewDlg->m_HParameterDefectRgn.Reset();
		THEAPP.m_pInspectAdminViewDlg->ClearAlignTabTempRegion();

		THEAPP.m_pInspectAdminViewDlg->SaveInspectTabOverLastSelectRegion();

		THEAPP.m_pInspectAdminViewDlg->ClearActiveTRegion();
		THEAPP.m_pInspectAdminViewDlg->ClearAllSelectRegion();
		THEAPP.m_pInspectAdminViewDlg->ClearAllLastSelectRegion();
		THEAPP.m_pInspectAdminViewDlg->m_pLastSelectedRegion = NULL;

		THEAPP.m_pInspectAdminViewDlg->InvalidateRect(false);
		THEAPP.m_pInspectAdminViewHideDlg->ShowWindow(SW_SHOWNA);
		THEAPP.m_pInspectAdminViewDlg->SetPosition(VIEW1_DLG1_LEFT, VIEW1_DLG1_TOP, VIEW1_DLG1_WIDTH+280, VIEW1_DLG1_HEIGHT+380);
		THEAPP.m_pInspectAdminViewDlg->UpdateToolbarStatus(FALSE);
		THEAPP.m_pInspectAdminViewDlg->Show();
		THEAPP.m_pTrayAdminViewDlg->Show();
		THEAPP.m_pDefectListDlg->ShowWindow(SW_SHOW);
		THEAPP.m_pLogDlg->Show();

		THEAPP.m_pTrayOperatorViewPrevDlg->ShowWindow(SW_HIDE);
		THEAPP.m_pTrayOperatorViewNowDlg->ShowWindow(SW_HIDE);
		THEAPP.m_pInspectSummary->ShowWindow(SW_SHOW);
		THEAPP.m_pInspectResultDlg->ShowWindow(SW_SHOW);

		THEAPP.m_pTabControlDlg->ShowWindow(SW_HIDE);	
		THEAPP.m_pTabControlDlg->HideAllPage();

		GetDlgItem(IDC_MFCBUTTON_FORM_NEW)->EnableWindow(TRUE);
		GetDlgItem(IDC_MFCBUTTON_FORM_LOAD)->EnableWindow(TRUE);
		GetDlgItem(IDC_MFCBUTTON_FORM_SAVE)->EnableWindow(TRUE);
		GetDlgItem(IDC_MFCBUTTON_FORM_TEACHING)->EnableWindow(TRUE);
		GetDlgItem(IDC_MFCBUTTON_FORM_PREFERENCE)->EnableWindow(TRUE);
		GetDlgItem(IDC_MFCBUTTON_FORM_REGISTER)->EnableWindow(TRUE);
		GetDlgItem(IDC_MFCBUTTON_FORM_LOAD)->EnableWindow(TRUE);
		THEAPP.m_iModeSwitch = MODE_INSPECT_VIEW_ADMIN;
#ifdef INLINE_MODE
		THEAPP.m_pHandlerService->Set_ModeUpdate(MS_INSPECT);
#endif


		if(AfxMessageBox("저장하시겠습니까?",MB_OKCANCEL|MB_SYSTEMMODAL)==IDOK)
		{
			THEAPP.m_pModelDataManager->SaveChangeParamHistory(TRUE);
			THEAPP.m_pModelDataManager->SaveModelData();

			AfxMessageBox("저장 완료.", MB_ICONINFORMATION|MB_SYSTEMMODAL);
		}
	}
}


void CuScanView::OnBnClickedMfcbuttonFormTeaching()
{
	if(THEAPP.m_pModelDataManager->m_sModelName=="."){return;}

	if(THEAPP.m_iModeSwitch==MODE_INSPECT_VIEW_ADMIN)  // 관리자 모드 아니면 화면 전환 안됨.
	{
		THEAPP.m_iModeSwitch = MODE_ADMIN_TEACH_VIEW;
		THEAPP.m_pInspectAdminViewDlg->SetPosition(VIEW1_DLG1_LEFT, VIEW1_DLG1_TOP, VIEW1_DLG1_WIDTH + 280, VIEW1_DLG1_HEIGHT + 380);
		THEAPP.m_pInspectAdminViewDlg->UpdateToolbarStatus(TRUE);
		THEAPP.m_pInspectAdminViewDlg->Show();
		THEAPP.m_pInspectAdminViewHideDlg->ShowWindow(SW_HIDE);
		THEAPP.m_pDefectListDlg->ShowWindow(SW_HIDE);
		THEAPP.m_pTrayAdminViewDlg->ShowWindow(SW_HIDE);
		THEAPP.m_pLogDlg->ShowWindow(SW_HIDE);
		THEAPP.m_pInspectSummary->ShowWindow(SW_HIDE);
		THEAPP.m_pInspectResultDlg->ShowWindow(SW_HIDE);

		THEAPP.m_pTabControlDlg->Show();
		THEAPP.m_pTabControlDlg->m_pJogSetDlg->LoadViewParam();

		THEAPP.m_pTabControlDlg->ShowFirstPage();
		THEAPP.m_pTabControlDlg->LoadModelConditionParam();
		GetDlgItem(IDC_MFCBUTTON_FORM_NEW)->EnableWindow(FALSE);
		GetDlgItem(IDC_MFCBUTTON_FORM_LOAD)->EnableWindow(FALSE);
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
		THEAPP.m_pTrayAdminViewDlg->m_bPrevTray=TRUE;

		THEAPP.m_bShowReviewWindow=FALSE;

#ifdef INLINE_MODE
		THEAPP.m_pHandlerService->Set_ModeUpdate(MS_TEACH);
#endif
	}
}


void CuScanView::OnBnClickedMfcbuttonFormPreference()
{
	PreferenceDlg.m_strEquipNo = THEAPP.Struct_PreferenceStruct.m_strEquipNo;

	PreferenceDlg.m_bCheckBeamProject =  THEAPP.Struct_PreferenceStruct.m_bCheckBeamProject;
	PreferenceDlg.m_bSaveBMP = THEAPP.Struct_PreferenceStruct.m_bSaveBMP;
	PreferenceDlg.m_bSaveJPG = THEAPP.Struct_PreferenceStruct.m_bSaveJPG;
	PreferenceDlg.m_strBasic_FolderPath = THEAPP.Struct_PreferenceStruct.m_strBasic_FolderPath;
	PreferenceDlg.m_strDebug_FolderPath = THEAPP.Struct_PreferenceStruct.m_strDebug_FolderPath;

	PreferenceDlg.m_iBarcodeCamSerialNo = THEAPP.Struct_PreferenceStruct.m_iBarcodeCamSerial;

	PreferenceDlg.m_iTrayOK_B		= THEAPP.Struct_PreferenceStruct.m_iTrayOK_B;
	PreferenceDlg.m_iTrayOK_G		= THEAPP.Struct_PreferenceStruct.m_iTrayOK_G;
	PreferenceDlg.m_iTrayOK_R		= THEAPP.Struct_PreferenceStruct.m_iTrayOK_R;
	PreferenceDlg.m_iTrayOKFont_B	= THEAPP.Struct_PreferenceStruct.m_iTrayOKFont_B;
	PreferenceDlg.m_iTrayOKFont_G	= THEAPP.Struct_PreferenceStruct.m_iTrayOKFont_G;
	PreferenceDlg.m_iTrayOKFont_R	= THEAPP.Struct_PreferenceStruct.m_iTrayOKFont_R;

	PreferenceDlg.m_iTrayNG_B		= THEAPP.Struct_PreferenceStruct.m_iTrayNG_B;
	PreferenceDlg.m_iTrayNG_G		= THEAPP.Struct_PreferenceStruct.m_iTrayNG_G;
	PreferenceDlg.m_iTrayNG_R		= THEAPP.Struct_PreferenceStruct.m_iTrayNG_R;
	PreferenceDlg.m_iTrayNGB_B		= THEAPP.Struct_PreferenceStruct.m_iTrayNGB_B;
	PreferenceDlg.m_iTrayNGB_G		= THEAPP.Struct_PreferenceStruct.m_iTrayNGB_G;
	PreferenceDlg.m_iTrayNGB_R		= THEAPP.Struct_PreferenceStruct.m_iTrayNGB_R;
	PreferenceDlg.m_iTrayNGL_B		= THEAPP.Struct_PreferenceStruct.m_iTrayNGL_B;
	PreferenceDlg.m_iTrayNGL_G		= THEAPP.Struct_PreferenceStruct.m_iTrayNGL_G;
	PreferenceDlg.m_iTrayNGL_R		= THEAPP.Struct_PreferenceStruct.m_iTrayNGL_R;
	PreferenceDlg.m_iTrayNGFont_B	= THEAPP.Struct_PreferenceStruct.m_iTrayNGFont_B;
	PreferenceDlg.m_iTrayNGFont_G	= THEAPP.Struct_PreferenceStruct.m_iTrayNGFont_G;
	PreferenceDlg.m_iTrayNGFont_R	= THEAPP.Struct_PreferenceStruct.m_iTrayNGFont_R;

	PreferenceDlg.m_iResultFont_B = THEAPP.Struct_PreferenceStruct.m_iResultFont_B;
	PreferenceDlg.m_iResultFont_G = THEAPP.Struct_PreferenceStruct.m_iResultFont_G;
	PreferenceDlg.m_iResultFont_R = THEAPP.Struct_PreferenceStruct.m_iResultFont_R;


	PreferenceDlg.m_iEditGrabDelayTime = THEAPP.Struct_PreferenceStruct.m_iGrabDelayTime;
	PreferenceDlg.m_iEditGrabDoneWaitTime = THEAPP.Struct_PreferenceStruct.m_iGrabDoneWaitTime;
	PreferenceDlg.m_iDefectDispDist = THEAPP.Struct_PreferenceStruct.m_iDefectDispDist;
	PreferenceDlg.m_bDxLogDetail = THEAPP.Struct_PreferenceStruct.m_bDxLogDetail;
	PreferenceDlg.m_bChkSaveDefectFeatureLog = THEAPP.Struct_PreferenceStruct.m_bSaveDefectFeatureLog;	// 불량 Feature 정보 추가 - LeeGW
	PreferenceDlg.m_bChkSaveDetectParamLog = THEAPP.Struct_PreferenceStruct.m_bSaveDetectParamLog;		// 검출 Param 로그 추가 - LeeGW

	PreferenceDlg.m_iEditSaveLasDataZipWaitTime = THEAPP.Struct_PreferenceStruct.m_iSaveLasDataZipWaitTime;
	PreferenceDlg.m_bCheckSaveLasDataZip = THEAPP.Struct_PreferenceStruct.m_bSaveLasDataZip;
	PreferenceDlg.m_bCheckSaveLasLog = THEAPP.Struct_PreferenceStruct.m_bSaveLasLog;
	PreferenceDlg.m_bCheckSaveLasImage = THEAPP.Struct_PreferenceStruct.m_bSaveLasImage;
	PreferenceDlg.m_bCheckSaveLasServerImage = THEAPP.Struct_PreferenceStruct.m_bSaveLasServerImage;

	PreferenceDlg.m_bCheckSaveRawImage = THEAPP.Struct_PreferenceStruct.m_bSaveRawImage;
	PreferenceDlg.m_bCheckSaveNGImage = THEAPP.Struct_PreferenceStruct.m_bSaveNGImage;
	PreferenceDlg.m_bCheckSaveReviewImage = THEAPP.Struct_PreferenceStruct.m_bSaveReviewImage;	// Review 이미지 저장 추가 - LeeGW
	PreferenceDlg.m_bCheckSaveFAIImage = THEAPP.Struct_PreferenceStruct.m_bSaveFAIImage;		// Review 이미지 저장 추가 - LeeGW
	PreferenceDlg.m_bCheckSaveResultImage = THEAPP.Struct_PreferenceStruct.m_bSaveResultImage;	// Result 이미지 저장 추가 - LeeGW
	PreferenceDlg.m_bCheckSaveResultMerge = THEAPP.Struct_PreferenceStruct.m_bSaveResultMerge;	// 24.07.08 - v2652 - ResultImage 통합 저장 기능 추가 - LeeGW

	PreferenceDlg.m_iDiskMax = THEAPP.Struct_PreferenceStruct.m_iDiskMax;
	PreferenceDlg.m_iDiskLean = THEAPP.Struct_PreferenceStruct.m_iDiskLean;
	PreferenceDlg.m_iDiskTerm = THEAPP.Struct_PreferenceStruct.m_iDiskTerm;
	PreferenceDlg.m_iDiskTimeH = THEAPP.Struct_PreferenceStruct.m_iDiskTimeH;
	PreferenceDlg.m_iDiskTimeM = THEAPP.Struct_PreferenceStruct.m_iDiskTimeM;
	PreferenceDlg.m_bDiskTermUse = THEAPP.Struct_PreferenceStruct.m_bDiskTermUse;

	PreferenceDlg.m_iEditEmptyCircleRadius = THEAPP.Struct_PreferenceStruct.m_iEmptyCircleRadius;
	PreferenceDlg.m_iEditEmptyMaxEdgePoint = THEAPP.Struct_PreferenceStruct.m_iEmptyMaxEdgePoint;

	PreferenceDlg.m_iRadioBarcodeNoReadImageType = THEAPP.Struct_PreferenceStruct.m_iBarcodeNoReadImageType;

	PreferenceDlg.m_sEditProductName = THEAPP.Struct_PreferenceStruct.m_sProductName;

	//0524
	PreferenceDlg.m_bCheckUseGrabErrFIltering = THEAPP.Struct_PreferenceStruct.m_bUseGrabErrFIltering;
	PreferenceDlg.m_iEditGrabErrDarkAreaGVLimit = THEAPP.Struct_PreferenceStruct.m_iGrabErrDarkAreaGVLimit;
	PreferenceDlg.m_iEditGrabErrBrightAreaGVLimit = THEAPP.Struct_PreferenceStruct.m_iGrabErrBrightAreaGVLimit;
	PreferenceDlg.m_iEditGrabErrRetryNo = THEAPP.Struct_PreferenceStruct.m_iGrabErrRetryNo;
	PreferenceDlg.m_iEditTriggerSleepTime = THEAPP.Struct_PreferenceStruct.m_iTriggerSleepTime;

	PreferenceDlg.m_iEditLightErrorMinLimit = THEAPP.Struct_PreferenceStruct.m_iLightErrorMinLimit;
	PreferenceDlg.m_iEditLightErrorMaxLimit = THEAPP.Struct_PreferenceStruct.m_iLightErrorMaxLimit;

	PreferenceDlg.m_iEditDefectWarningDefectCount = THEAPP.Struct_PreferenceStruct.m_iDefectWarningDefectCount;
	PreferenceDlg.m_iEditDefectWarningDefectDistance = THEAPP.Struct_PreferenceStruct.m_iDefectWarningDefectDistance;

	PreferenceDlg.m_iRadioCamFOVType = THEAPP.Struct_PreferenceStruct.m_iCamFOVType;
	PreferenceDlg.m_iRadioPickerTrayDir = THEAPP.Struct_PreferenceStruct.m_iPickerTrayDir;

	PreferenceDlg.m_iEditLensResultImageX = THEAPP.Struct_PreferenceStruct.m_iLensResultImageX;
	PreferenceDlg.m_iEditLensResultImageY = THEAPP.Struct_PreferenceStruct.m_iLensResultImageY;
	PreferenceDlg.m_dEditLensResultImageZoom = THEAPP.Struct_PreferenceStruct.m_dLensResultImageZoom;
	PreferenceDlg.m_iEditExtraResultImageX = THEAPP.Struct_PreferenceStruct.m_iExtraResultImageX;
	PreferenceDlg.m_iEditExtraResultImageY = THEAPP.Struct_PreferenceStruct.m_iExtraResultImageY;
	PreferenceDlg.m_dEditExtraResultImageZoom = THEAPP.Struct_PreferenceStruct.m_dExtraResultImageZoom;
	
	PreferenceDlg.m_bIsUseAIInsp = THEAPP.Struct_PreferenceStruct.m_bIsUseAIInsp;
	PreferenceDlg.m_bIsApplyAISimulation = THEAPP.Struct_PreferenceStruct.m_bIsApplyAISimulation;
	PreferenceDlg.m_bCheckUseAIResizeImageSave = THEAPP.Struct_PreferenceStruct.m_bUseAIResizeImageSave;
	PreferenceDlg.m_iEditAICropImageWidth = THEAPP.Struct_PreferenceStruct.m_iAICropImageWidth;
	PreferenceDlg.m_iEditAICropImageHeight = THEAPP.Struct_PreferenceStruct.m_iAICropImageHeight;
	PreferenceDlg.m_iEditAIResizeImageWidth = THEAPP.Struct_PreferenceStruct.m_iAIResizeImageWidth;
	PreferenceDlg.m_iEditAIResizeImageHeight = THEAPP.Struct_PreferenceStruct.m_iAIResizeImageHeight;

	PreferenceDlg.m_strADJIPAddress = THEAPP.Struct_PreferenceStruct.m_strADJIPAddress;
	PreferenceDlg.m_iADJPortNo = THEAPP.Struct_PreferenceStruct.m_iADJPortNo;
	PreferenceDlg.m_iADJImageCropType = THEAPP.Struct_PreferenceStruct.m_iADJImageCropType;
	PreferenceDlg.m_strADJModelName = THEAPP.Struct_PreferenceStruct.m_strADJModelName;
	PreferenceDlg.m_iADJDelayTime = THEAPP.Struct_PreferenceStruct.m_iADJDelayTime;

	PreferenceDlg.m_bIsUseADJRunCheck = THEAPP.Struct_PreferenceStruct.m_bIsUseADJRunCheck;
	PreferenceDlg.m_strADJExePath = THEAPP.Struct_PreferenceStruct.m_strADJExePath;

	//Multiple Defect Start
	PreferenceDlg.m_bCheckUseLotDefectAlarm = THEAPP.Struct_PreferenceStruct.m_bUseMultipleDefectAlarm;
	PreferenceDlg.m_bCheckUseLotDefectAlarmAreaSimilarity = THEAPP.Struct_PreferenceStruct.m_bUseMultipleDefectAlarmAreaSimilarity;
	PreferenceDlg.m_bCheckUseMultipleDefectSendAlarm = THEAPP.Struct_PreferenceStruct.m_bUseMultipleDefectSendAlarm;
	PreferenceDlg.m_iEditLotDefectAlarmSamePosTolerance = THEAPP.Struct_PreferenceStruct.m_iMultipleDefectAlarmSamePosTolerance;
	PreferenceDlg.m_iEditLotDefectAlarmAreaSimilarityTolerance = THEAPP.Struct_PreferenceStruct.m_iMultipleDefectAlarmAreaSimilarityTolerance;
	PreferenceDlg.m_iEditLotDefectAlarmDefectRatio = THEAPP.Struct_PreferenceStruct.m_iMultipleDefectAlarmDefectRatio;
	PreferenceDlg.m_iEditLotDefectAlarmMinimumNum = THEAPP.Struct_PreferenceStruct.m_iMultipleDefectAlarmMinimumNum;
	//Multiple Defect End

	PreferenceDlg.m_bCheckChangeEvmsDirectory = THEAPP.Struct_PreferenceStruct.m_bChangeEvmsDirectory; //Ver2629
	PreferenceDlg.m_bCheckUseAbsolutePathModel = THEAPP.Struct_PreferenceStruct.m_bUseAbsolutePathModel; //Ver2629

	PreferenceDlg.m_strEditRmsSaveFolderPath = THEAPP.Struct_PreferenceStruct.m_strRmsSaveFolderPath; //RMS

	PreferenceDlg.m_iEditResultTextPosX = THEAPP.Struct_PreferenceStruct.m_iResultTextPosX; //Result Text
	PreferenceDlg.m_iEditResultTextPosY = THEAPP.Struct_PreferenceStruct.m_iResultTextPosY; //Result Text
	PreferenceDlg.m_iEditResultTextSize = THEAPP.Struct_PreferenceStruct.m_iResultTextSize; //Result Text

	PreferenceDlg.m_bCheckSpecialNGSort = THEAPP.Struct_PreferenceStruct.m_bSpecialNGSort;	// 25.02.12 - v3008 - Special NG 분리배출 추가 - LeeGW

	PreferenceDlg.m_bCheckSaveRawImageResize =  THEAPP.Struct_PreferenceStruct.m_bSaveRawImageResize;	// 24.07.31 - v2656 - RAWIMAGE 축소, 합성 저장 기능 추가 - LeeGW
	PreferenceDlg.m_dEditSaveRawImageResizeRatio = THEAPP.Struct_PreferenceStruct.m_dSaveRawImageResizeRatio;	// 24.07.31 - v2656 - RAWIMAGE 축소, 합성 저장 기능 추가 - LeeGW
	PreferenceDlg.m_bCheckSaveRawImageMerge = THEAPP.Struct_PreferenceStruct.m_bSaveRawImageMerge;	// 24.07.31 - v2656 - RAWIMAGE 축소, 합성 저장 기능 추가 - LeeGW

	PreferenceDlg.m_bCheckUseGrabErrFIltering2 = THEAPP.Struct_PreferenceStruct.m_bUseGrabErrFIltering2;	// 24.08.09 - v2658 - 잘린 영상 필터링 추가 - LeeGW
	PreferenceDlg.m_iEditGrabErrSubGVLimit = THEAPP.Struct_PreferenceStruct.m_iGrabErrSubGVLimit;	// 24.08.09 - v2658 - 잘린 영상 필터링 추가 - LeeGW

	PreferenceDlg.m_iRadioMachineInspType = THEAPP.Struct_PreferenceStruct.m_iMachineInspType;

	PreferenceDlg.m_bCheckUseProcessRestart = THEAPP.Struct_PreferenceStruct.m_bUseProcessRestart;					// 24.12.02 - v3005 - 프로세스 초기화 추가 - LeeGW
	PreferenceDlg.m_dEditProcessRestartMemoryLimit = THEAPP.Struct_PreferenceStruct.m_dProcessRestartMemoryLimit;	// 24.12.02 - v3005 - 프로세스 초기화 추가 - LeeGW
	PreferenceDlg.m_iEditProcessRestartTimeMin = THEAPP.Struct_PreferenceStruct.m_iProcessRestartTimeMin;			// 24.12.02 - v3005 - 프로세스 초기화 추가 - LeeGW
	PreferenceDlg.m_iEditProcessRestartTimeMax = THEAPP.Struct_PreferenceStruct.m_iProcessRestartTimeMax;			// 24.12.02 - v3005 - 프로세스 초기화 추가 - LeeGW		
	PreferenceDlg.m_iEditProcessRestartDelayTime = THEAPP.Struct_PreferenceStruct.m_iProcessRestartDelayTime;			// 24.12.02 - v3005 - 프로세스 초기화 추가 - LeeGW			

	PreferenceDlg.m_bCheckUseMboMode = THEAPP.Struct_PreferenceStruct.m_bUseMboMode;	// 25.05.13 - LeeGW
	PreferenceDlg.m_iEditMboModeCount = THEAPP.Struct_PreferenceStruct.m_iMboModeCount;		// 25.05.13 - LeeGW

	PreferenceDlg.m_bCheckUseHandlerRetry = THEAPP.Struct_PreferenceStruct.m_bUseHandlerRetry;	// 25.06.24 - LeeGW
	PreferenceDlg.m_iEditHandlerRetryCount = THEAPP.Struct_PreferenceStruct.m_iHandlerRetryCount;	// 25.06.24 - LeeGW
	PreferenceDlg.m_iEditHandlerRetryWaitTime = THEAPP.Struct_PreferenceStruct.m_iHandlerRetryWaitTime;	// 25.06.24 - LeeGW
	PreferenceDlg.m_iEditHandlerReplyWaitTime = THEAPP.Struct_PreferenceStruct.m_iHandlerReplyWaitTime;	// 25.06.24 - LeeGW

	if(PreferenceDlg.DoModal()==IDOK)
	{
		if(AfxMessageBox("저장하시겠습니까?",MB_OKCANCEL|MB_SYSTEMMODAL)==IDOK)
		{
			THEAPP.Struct_PreferenceStruct.m_strDebug_FolderPath = PreferenceDlg.m_strDebug_FolderPath;
				
			CString strModelFolder = THEAPP.GetWorkingDirectory()+"\\Data\\";

			//Ver2629 add start
			if( THEAPP.m_bUseEvms == FALSE && PreferenceDlg.m_bCheckChangeEvmsDirectory == TRUE )
			{
				CString strCheckEvmsFoler = THEAPP.GetWorkingDirectory() + "\\..\\..\\..\\..\\ENV";
				int nEvmsExist = GetFileAttributes(strCheckEvmsFoler);
				if( nEvmsExist != -1) //EVMS파일 존재
				{
					strModelFolder = strCheckEvmsFoler + "\\Data\\";
				}
			}				
			//Ver2629 add end

			CIniFileCS INI(strModelFolder+"\\Prefecrence.ini");
			CString strSection = "Result Display";
			INI.Set_Bool(strSection,"Beam Project",PreferenceDlg.m_bCheckBeamProject);
			strSection = "SaveImage File Extension";
			INI.Set_Bool(strSection,"BMP",PreferenceDlg.m_bSaveBMP);
			INI.Set_Bool(strSection,"JPG",PreferenceDlg.m_bSaveJPG);

			strSection = "Folder Path";
			INI.Set_String(strSection,"Save Result Folder Path",PreferenceDlg.m_strBasic_FolderPath);
			INI.Set_String(strSection,"Debug Folder Path",PreferenceDlg.m_strDebug_FolderPath);

			strSection = "Disk Cleaner";
			INI.Set_Integer(strSection, "m_iDiskMax", PreferenceDlg.m_iDiskMax);
			INI.Set_Integer(strSection, "m_iDiskLean", PreferenceDlg.m_iDiskLean);
			INI.Set_Integer(strSection, "m_iDiskTerm", PreferenceDlg.m_iDiskTerm);
			INI.Set_Integer(strSection, "m_iDiskTimeH", PreferenceDlg.m_iDiskTimeH);
			INI.Set_Integer(strSection, "m_iDiskTimeM", PreferenceDlg.m_iDiskTimeM);
			INI.Set_Bool(strSection, "m_bDiskTermUse", PreferenceDlg.m_bDiskTermUse);

			strSection = "EQUIPMENT NO";
			INI.Set_String(strSection,"EquipNo", PreferenceDlg.m_strEquipNo);

			strSection = "BARCODE CAMERA";
			INI.Set_Integer(strSection,"SerialNumber",PreferenceDlg.m_iBarcodeCamSerialNo);

			strSection = "Color";
			INI.Set_Integer(strSection,"TrayOK_R",PreferenceDlg.m_iTrayOK_R);
			INI.Set_Integer(strSection,"TrayOK_G",PreferenceDlg.m_iTrayOK_G);
			INI.Set_Integer(strSection,"TrayOK_B",PreferenceDlg.m_iTrayOK_B);

			INI.Set_Integer(strSection,"TrayOKFont_R",PreferenceDlg.m_iTrayOKFont_R);
			INI.Set_Integer(strSection,"TrayOKFont_G",PreferenceDlg.m_iTrayOKFont_G);
			INI.Set_Integer(strSection,"TrayOKFont_B",PreferenceDlg.m_iTrayOKFont_B);

			INI.Set_Integer(strSection,"TrayNG_R",PreferenceDlg.m_iTrayNG_R);
			INI.Set_Integer(strSection,"TrayNG_G",PreferenceDlg.m_iTrayNG_G);
			INI.Set_Integer(strSection,"TrayNG_B",PreferenceDlg.m_iTrayNG_B);

			INI.Set_Integer(strSection,"TrayNGB_R",PreferenceDlg.m_iTrayNGB_R);
			INI.Set_Integer(strSection,"TrayNGB_G",PreferenceDlg.m_iTrayNGB_G);
			INI.Set_Integer(strSection,"TrayNGB_B",PreferenceDlg.m_iTrayNGB_B);

			INI.Set_Integer(strSection,"TrayNGL_R",PreferenceDlg.m_iTrayNGL_R);
			INI.Set_Integer(strSection,"TrayNGL_G",PreferenceDlg.m_iTrayNGL_G);
			INI.Set_Integer(strSection,"TrayNGL_B",PreferenceDlg.m_iTrayNGL_B);

			INI.Set_Integer(strSection,"TrayNGFont_R",PreferenceDlg.m_iTrayNGFont_R);
			INI.Set_Integer(strSection,"TrayNGFont_G",PreferenceDlg.m_iTrayNGFont_G);
			INI.Set_Integer(strSection,"TrayNGFont_B",PreferenceDlg.m_iTrayNGFont_B);

			INI.Set_Integer(strSection,"ResultFont_R",PreferenceDlg.m_iResultFont_R);
			INI.Set_Integer(strSection,"ResultFont_G",PreferenceDlg.m_iResultFont_G);
			INI.Set_Integer(strSection,"ResultFont_B",PreferenceDlg.m_iResultFont_B);

			strSection = "Option_Etc";
			INI.Set_Integer(strSection,"GRAB_DELAY_TIME", PreferenceDlg.m_iEditGrabDelayTime);
			INI.Set_Integer(strSection,"GRAB_DONE_WAIT_TIME", PreferenceDlg.m_iEditGrabDoneWaitTime);
			INI.Set_Integer(strSection,"Defect_Display_Dist", PreferenceDlg.m_iDefectDispDist);
			INI.Set_Bool(strSection, "LOG_DETAIL", PreferenceDlg.m_bDxLogDetail);
			INI.Set_Bool(strSection, "SAVE_DSF_LOG", PreferenceDlg.m_bChkSaveDefectFeatureLog);	// 불량 Feature 정보 추가 - LeeGW
			INI.Set_Bool(strSection, "SAVE_PARAM_LOG", PreferenceDlg.m_bChkSaveDetectParamLog);	// 검출 Param 로그 추가 - LeeGW
			
			
			INI.Set_Integer(strSection, "SAVE_LAS_DATA_ZIP_WAIT_TIME", PreferenceDlg.m_iEditSaveLasDataZipWaitTime);
			INI.Set_Bool(strSection, "SAVE_LAS_DATA_ZIP", PreferenceDlg.m_bCheckSaveLasDataZip);
			INI.Set_Bool(strSection, "SAVE_LAS_LOG", PreferenceDlg.m_bCheckSaveLasLog);
			INI.Set_Bool(strSection, "SAVE_LAS_IMAGE", PreferenceDlg.m_bCheckSaveLasImage);
			INI.Set_Bool(strSection, "SAVE_LAS_SERVER_IMAGE", PreferenceDlg.m_bCheckSaveLasServerImage);

			INI.Set_Bool(strSection, "SAVE_RAW_IMAGE", PreferenceDlg.m_bCheckSaveRawImage);
			INI.Set_Bool(strSection, "SAVE_NG_IMAGE", PreferenceDlg.m_bCheckSaveNGImage);
			INI.Set_Bool(strSection, "SAVE_REVIEW_IMAGE", PreferenceDlg.m_bCheckSaveReviewImage);	// Review 이미지 저장 추가 - LeeGW
			INI.Set_Bool(strSection, "SAVE_FAI_IMAGE", PreferenceDlg.m_bCheckSaveFAIImage);	// FAI 이미지 저장 추가 - LeeGW
			INI.Set_Bool(strSection, "SAVE_RESULT_IMAGE", PreferenceDlg.m_bCheckSaveResultImage);	// Result 이미지 저장 추가 - LeeGW
			INI.Set_Bool(strSection, "RESULT_IMAGE_MERGE", PreferenceDlg.m_bCheckSaveResultMerge);	// 24.07.08 - v2652 - ResultImage 통합 저장 기능 추가 - LeeGW
	

			INI.Set_Integer(strSection,"EM_CIRCLE_RADIUS", PreferenceDlg.m_iEditEmptyCircleRadius);
			INI.Set_Integer(strSection,"EM_MAX_EDGE_POINT", PreferenceDlg.m_iEditEmptyMaxEdgePoint);

			INI.Set_Integer(strSection,"BARCODE_NO_READ_IMAGE_TYPE", PreferenceDlg.m_iRadioBarcodeNoReadImageType);

			INI.Set_String(strSection,"PRODUCT NAME",PreferenceDlg.m_sEditProductName);

			//0524
			INI.Set_Bool(strSection, "USE_GRAB_ERROR_FILTERING", PreferenceDlg.m_bCheckUseGrabErrFIltering);
			INI.Set_Integer(strSection,"GRAB_ERROR_DARK_AREA_GV_LIMIT", PreferenceDlg.m_iEditGrabErrDarkAreaGVLimit);
			INI.Set_Integer(strSection,"GRAB_ERROR_BRIGHT_AREA_GV_LIMIT", PreferenceDlg.m_iEditGrabErrBrightAreaGVLimit);
			INI.Set_Integer(strSection,"GRAB_ERROR_RETRY_NUMBER", PreferenceDlg.m_iEditGrabErrRetryNo);
			INI.Set_Integer(strSection,"TRIGGER_SLEEP_TIME", PreferenceDlg.m_iEditTriggerSleepTime);

			INI.Set_Integer(strSection,"LIGHT_ERROR_MIN_LIMIT", PreferenceDlg.m_iEditLightErrorMinLimit);
			INI.Set_Integer(strSection,"LIGHT_ERROR_MAX_LIMIT", PreferenceDlg.m_iEditLightErrorMaxLimit);

			INI.Set_Integer(strSection,"DEFECT_WARNING_DEFECT_COUNT", PreferenceDlg.m_iEditDefectWarningDefectCount);
			INI.Set_Integer(strSection,"DEFECT_WARNING_DEFECT_DISTANCE", PreferenceDlg.m_iEditDefectWarningDefectDistance);

			INI.Set_Integer(strSection,"CAM_FOV_TYPE", PreferenceDlg.m_iRadioCamFOVType);

			INI.Set_Integer(strSection,"PICKER_TRAY_DIR", PreferenceDlg.m_iRadioPickerTrayDir);

			INI.Set_Bool(strSection, "SPECIAL_NG_SORT", PreferenceDlg.m_bCheckSpecialNGSort);	// 25.02.12 - v3008 - Special NG 분리배출 추가 - LeeGW
			INI.Set_Bool(strSection, "USE_RAWIMAGE_RESIZE", PreferenceDlg.m_bCheckSaveRawImageResize);	// 24.07.31 - v2656 - RAWIMAGE 축소, 합성 저장 기능 추가 - LeeGW
			INI.Set_Double(strSection, "USE_RAWIMAGE_RESIZE_RATIO", PreferenceDlg.m_dEditSaveRawImageResizeRatio);	// 24.07.31 - v2656 - RAWIMAGE 축소, 합성 저장 기능 추가 - LeeGW
			INI.Set_Bool(strSection, "USE_RAWIMAGE_MERGE", PreferenceDlg.m_bCheckSaveRawImageMerge);	// 24.07.31 - v2656 - RAWIMAGE 축소, 합성 저장 기능 추가 - LeeGW

			INI.Set_Bool(strSection, "USE_GRAB_ERROR_FILTERING2", PreferenceDlg.m_bCheckUseGrabErrFIltering2);	// 24.08.09 - v2658 - 잘린 영상 필터링 추가 - LeeGW
			INI.Set_Integer(strSection,"GRAB_ERROR_SUB_GV_MAX_LIMIT", PreferenceDlg.m_iEditGrabErrSubGVLimit);	// 24.08.09 - v2658 - 잘린 영상 필터링 추가 - LeeGW

			INI.Set_Integer(strSection, "MACHINE_INSPECTION_TYPE", PreferenceDlg.m_iRadioMachineInspType);

			INI.Set_Bool(strSection, "USE_PROCESS_RESTART", PreferenceDlg.m_bCheckUseProcessRestart);						// 24.12.02 - v3005 - 프로세스 초기화 추가 - LeeGW
			INI.Set_Double(strSection, "PROCESS_RESTART_MEMORY_LIMIT", PreferenceDlg.m_dEditProcessRestartMemoryLimit);	// 24.12.02 - v3005 - 프로세스 초기화 추가 - LeeGW
			INI.Set_Integer(strSection, "PROCESS_RESTART_TIME_MIN", PreferenceDlg.m_iEditProcessRestartTimeMin);			// 24.12.02 - v3005 - 프로세스 초기화 추가 - LeeGW
			INI.Set_Integer(strSection, "PROCESS_RESTART_TIME_MAX", PreferenceDlg.m_iEditProcessRestartTimeMax);			// 24.12.02 - v3005 - 프로세스 초기화 추가 - LeeGW
			INI.Set_Integer(strSection, "PROCESS_RESTART_DELAY_TIME", PreferenceDlg.m_iEditProcessRestartDelayTime);			// 24.12.02 - v3005 - 프로세스 초기화 추가 - LeeGW

			INI.Set_Bool(strSection, "USE_MBO_MODE", PreferenceDlg.m_bCheckUseMboMode);				
			INI.Set_Integer(strSection, "MBO_MODE_COUNT", PreferenceDlg.m_iEditMboModeCount);

			INI.Set_Bool(strSection, "USE_HANDLER_RETRY", PreferenceDlg.m_bCheckUseHandlerRetry);	// 25.06.24 - LeeGW
			INI.Set_Integer(strSection, "HANDLER_RETRY_COUNT", PreferenceDlg.m_iEditHandlerRetryCount);	// 25.06.24 - LeeGW
			INI.Set_Integer(strSection, "HANDLER_RETRY_WAITTIME", PreferenceDlg.m_iEditHandlerRetryWaitTime);	// 25.06.24 - LeeGW
			INI.Set_Integer(strSection, "HANDLER_REPLY_WAITTIME", PreferenceDlg.m_iEditHandlerReplyWaitTime);	// 25.06.24 - LeeGW

			strSection = "Option_Result";

			INI.Set_Integer(strSection,"LENS_RESULT_START_X", PreferenceDlg.m_iEditLensResultImageX);
			INI.Set_Integer(strSection,"LENS_RESULT_START_Y", PreferenceDlg.m_iEditLensResultImageY);
			INI.Set_Double(strSection, "LENS_RESULT_ZOOM_RATIO", PreferenceDlg.m_dEditLensResultImageZoom);
			INI.Set_Integer(strSection,"EXTRA_RESULT_START_X", PreferenceDlg.m_iEditExtraResultImageX);
			INI.Set_Integer(strSection,"EXTRA_RESULT_START_Y", PreferenceDlg.m_iEditExtraResultImageY);
			INI.Set_Double(strSection, "EXTRA_RESULT_ZOOM_RATIO", PreferenceDlg.m_dEditExtraResultImageZoom);

			strSection = "Option_ADJ";
			INI.Set_Bool(strSection, "USE_AI_INSP", PreferenceDlg.m_bIsUseAIInsp);
			INI.Set_Bool(strSection, "APPLY_AI_RESULT", PreferenceDlg.m_bIsApplyAIResult);
			INI.Set_Bool(strSection, "APPLY_AI_SIMULATION", PreferenceDlg.m_bIsApplyAISimulation);
			INI.Set_Bool(strSection, "USE_AI_RESIZE_IMAGE_SAVE", PreferenceDlg.m_bCheckUseAIResizeImageSave);
			INI.Set_Integer(strSection, "AI_CROP_IMAGE_WIDTH", PreferenceDlg.m_iEditAICropImageWidth);
			INI.Set_Integer(strSection, "AI_CROP_IMAGE_HEIGHT", PreferenceDlg.m_iEditAICropImageHeight);
			INI.Set_Integer(strSection, "AI_RESIZE_IMAGE_WIDTH", PreferenceDlg.m_iEditAIResizeImageWidth);
			INI.Set_Integer(strSection, "AI_RESIZE_IMAGE_HEIGHT", PreferenceDlg.m_iEditAIResizeImageHeight);

			INI.Set_String(strSection, "ADJ_IP", PreferenceDlg.m_strADJIPAddress);
			INI.Set_Integer(strSection, "ADJ_PORT", PreferenceDlg.m_iADJPortNo);
			INI.Set_Integer(strSection, "ADJ_IMAGE_CROP_TYPE", PreferenceDlg.m_iADJImageCropType);
			INI.Set_String(strSection, "ADJ_MODEL_NAME", PreferenceDlg.m_strADJModelName);
			INI.Set_Integer(strSection, "ADJ_DELAY_TIME", PreferenceDlg.m_iADJDelayTime);

			INI.Set_Bool(strSection, "USE_ADJ_RUN_CHECK", PreferenceDlg.m_bIsUseADJRunCheck);
			INI.Set_String(strSection, "ADJ_EXE_PATH", PreferenceDlg.m_strADJExePath);

			//Multiple Defect Start
			strSection = "Multiple_Defect";
			INI.Set_Bool(strSection,    "MD_USE_ALARM",              PreferenceDlg.m_bCheckUseLotDefectAlarm);
			INI.Set_Bool(strSection,    "MD_USE_AREA_SIMILARITY",    PreferenceDlg.m_bCheckUseLotDefectAlarmAreaSimilarity);
			INI.Set_Bool(strSection,    "MD_USE_SEND_HANDLER_ALARM", PreferenceDlg.m_bCheckUseMultipleDefectSendAlarm);
			INI.Set_Integer(strSection, "MD_POS_TOLERANCE",          PreferenceDlg.m_iEditLotDefectAlarmSamePosTolerance);
			INI.Set_Integer(strSection, "MD_AREA_TOLERANCE",         PreferenceDlg.m_iEditLotDefectAlarmAreaSimilarityTolerance);
			INI.Set_Integer(strSection, "MD_DefectRatio",            PreferenceDlg.m_iEditLotDefectAlarmDefectRatio);
			INI.Set_Integer(strSection, "MD_MIN_INSPECTION_NUM",     PreferenceDlg.m_iEditLotDefectAlarmMinimumNum);

			THEAPP.Struct_PreferenceStruct.m_bUseMultipleDefectAlarm = PreferenceDlg.m_bCheckUseLotDefectAlarm;
			THEAPP.Struct_PreferenceStruct.m_bUseMultipleDefectAlarmAreaSimilarity = PreferenceDlg.m_bCheckUseLotDefectAlarmAreaSimilarity;
			THEAPP.Struct_PreferenceStruct.m_bUseMultipleDefectSendAlarm = PreferenceDlg.m_bCheckUseMultipleDefectSendAlarm;
			THEAPP.Struct_PreferenceStruct.m_iMultipleDefectAlarmSamePosTolerance = PreferenceDlg.m_iEditLotDefectAlarmSamePosTolerance;
			THEAPP.Struct_PreferenceStruct.m_iMultipleDefectAlarmAreaSimilarityTolerance = PreferenceDlg.m_iEditLotDefectAlarmAreaSimilarityTolerance;
			THEAPP.Struct_PreferenceStruct.m_iMultipleDefectAlarmDefectRatio = PreferenceDlg.m_iEditLotDefectAlarmDefectRatio;
			THEAPP.Struct_PreferenceStruct.m_iMultipleDefectAlarmMinimumNum = PreferenceDlg.m_iEditLotDefectAlarmMinimumNum;
			//Multiple Defect End

			//Ver2629 Start
			strSection = "EVMS_Option";
			INI.Set_Bool(strSection,    "EVMS_USE_CHANGE_DIRECTORY",    PreferenceDlg.m_bCheckChangeEvmsDirectory);
			INI.Set_Bool(strSection,    "EVMS_USE_ABSOLUTE_MODEL_PATH",    PreferenceDlg.m_bCheckUseAbsolutePathModel);
			//Ver2629 End

			//Result Text Start
			strSection = "RESULT_TEXT";
			INI.Set_Integer(strSection, "RESULT_TEXT_POS_X", PreferenceDlg.m_iEditResultTextPosX);
			INI.Set_Integer(strSection, "RESULT_TEXT_POS_Y", PreferenceDlg.m_iEditResultTextPosY);
			INI.Set_Integer(strSection, "RESULT_TEXT_SIZE", PreferenceDlg.m_iEditResultTextSize);
			//Result Text End

			//RMS start
			strSection = "RMS_Option";
			INI.Set_String(strSection, "RMS_SAVE_FOLDER_PATH", PreferenceDlg.m_strEditRmsSaveFolderPath);
			//RMS end

			THEAPP.Struct_PreferenceStruct.m_bCheckBeamProject = PreferenceDlg.m_bCheckBeamProject;
			THEAPP.Struct_PreferenceStruct.m_bSaveBMP = PreferenceDlg.m_bSaveBMP;
			THEAPP.Struct_PreferenceStruct.m_bSaveJPG = PreferenceDlg.m_bSaveJPG;
			THEAPP.Struct_PreferenceStruct.m_strBasic_FolderPath = PreferenceDlg.m_strBasic_FolderPath;
			THEAPP.Struct_PreferenceStruct.m_strDebug_FolderPath = PreferenceDlg.m_strDebug_FolderPath;

			THEAPP.Struct_PreferenceStruct.m_iTrayOK_B=	PreferenceDlg.m_iTrayOK_B;
			THEAPP.Struct_PreferenceStruct.m_iTrayOK_G=	PreferenceDlg.m_iTrayOK_G;
			THEAPP.Struct_PreferenceStruct.m_iTrayOK_R=	PreferenceDlg.m_iTrayOK_R;
			THEAPP.Struct_PreferenceStruct.m_iTrayOKFont_B=	PreferenceDlg.m_iTrayOKFont_B;
			THEAPP.Struct_PreferenceStruct.m_iTrayOKFont_G=	PreferenceDlg.m_iTrayOKFont_G;
			THEAPP.Struct_PreferenceStruct.m_iTrayOKFont_R=	PreferenceDlg.m_iTrayOKFont_R;

			THEAPP.Struct_PreferenceStruct.m_iTrayNG_B=	PreferenceDlg.m_iTrayNG_B; 
			THEAPP.Struct_PreferenceStruct.m_iTrayNG_G=	PreferenceDlg.m_iTrayNG_G;
			THEAPP.Struct_PreferenceStruct.m_iTrayNG_R=	PreferenceDlg.m_iTrayNG_R;
			THEAPP.Struct_PreferenceStruct.m_iTrayNGB_B = PreferenceDlg.m_iTrayNGB_B;
			THEAPP.Struct_PreferenceStruct.m_iTrayNGB_G = PreferenceDlg.m_iTrayNGB_G;
			THEAPP.Struct_PreferenceStruct.m_iTrayNGB_R = PreferenceDlg.m_iTrayNGB_R;
			THEAPP.Struct_PreferenceStruct.m_iTrayNGL_B = PreferenceDlg.m_iTrayNGL_B;
			THEAPP.Struct_PreferenceStruct.m_iTrayNGL_G = PreferenceDlg.m_iTrayNGL_G;
			THEAPP.Struct_PreferenceStruct.m_iTrayNGL_R = PreferenceDlg.m_iTrayNGL_R;
			THEAPP.Struct_PreferenceStruct.m_iTrayNGFont_B=	PreferenceDlg.m_iTrayNGFont_B;
			THEAPP.Struct_PreferenceStruct.m_iTrayNGFont_G=	PreferenceDlg.m_iTrayNGFont_G;
			THEAPP.Struct_PreferenceStruct.m_iTrayNGFont_R=	PreferenceDlg.m_iTrayNGFont_R;

			THEAPP.Struct_PreferenceStruct.m_iResultFont_B=	PreferenceDlg.m_iResultFont_B; 
			THEAPP.Struct_PreferenceStruct.m_iResultFont_G=	PreferenceDlg.m_iResultFont_G; 
			THEAPP.Struct_PreferenceStruct.m_iResultFont_R=	PreferenceDlg.m_iResultFont_R; 

			THEAPP.Struct_PreferenceStruct.m_iGrabDelayTime = PreferenceDlg.m_iEditGrabDelayTime;
			THEAPP.Struct_PreferenceStruct.m_iGrabDoneWaitTime = PreferenceDlg.m_iEditGrabDoneWaitTime;
			THEAPP.Struct_PreferenceStruct.m_iDefectDispDist = PreferenceDlg.m_iDefectDispDist;
			THEAPP.Struct_PreferenceStruct.m_bDxLogDetail = PreferenceDlg.m_bDxLogDetail;
			THEAPP.Struct_PreferenceStruct.m_bSaveDefectFeatureLog = PreferenceDlg.m_bChkSaveDefectFeatureLog;	// 불량 Feature 정보 추가 - LeeGW
			THEAPP.Struct_PreferenceStruct.m_bSaveDetectParamLog = PreferenceDlg.m_bChkSaveDetectParamLog;		// 검출 Param 로그 추가 - LeeGW

			THEAPP.Struct_PreferenceStruct.m_iSaveLasDataZipWaitTime = PreferenceDlg.m_iEditSaveLasDataZipWaitTime;
			THEAPP.Struct_PreferenceStruct.m_bSaveLasDataZip = PreferenceDlg.m_bCheckSaveLasDataZip;
			THEAPP.Struct_PreferenceStruct.m_bSaveLasLog = PreferenceDlg.m_bCheckSaveLasLog;
			THEAPP.Struct_PreferenceStruct.m_bSaveLasImage = PreferenceDlg.m_bCheckSaveLasImage;
			THEAPP.Struct_PreferenceStruct.m_bSaveLasServerImage = PreferenceDlg.m_bCheckSaveLasServerImage;

			THEAPP.Struct_PreferenceStruct.m_bSaveRawImage = PreferenceDlg.m_bCheckSaveRawImage;
			THEAPP.Struct_PreferenceStruct.m_bSaveNGImage = PreferenceDlg.m_bCheckSaveNGImage;
			THEAPP.Struct_PreferenceStruct.m_bSaveReviewImage = PreferenceDlg.m_bCheckSaveReviewImage;	// Review 이미지 저장 추가 - LeeGW
			THEAPP.Struct_PreferenceStruct.m_bSaveFAIImage = PreferenceDlg.m_bCheckSaveFAIImage;		// Review 이미지 저장 추가 - LeeGW
			THEAPP.Struct_PreferenceStruct.m_bSaveResultImage = PreferenceDlg.m_bCheckSaveResultImage;	// Result 이미지 저장 추가 - LeeGW
			THEAPP.Struct_PreferenceStruct.m_bSaveResultMerge = PreferenceDlg.m_bCheckSaveResultMerge;	// 24.07.08 - v2652 - ResultImage 통합 저장 기능 추가 - LeeGW

			THEAPP.Struct_PreferenceStruct.m_iDiskMax = PreferenceDlg.m_iDiskMax;
			THEAPP.Struct_PreferenceStruct.m_iDiskLean = PreferenceDlg.m_iDiskLean;
			THEAPP.Struct_PreferenceStruct.m_iDiskTerm = PreferenceDlg.m_iDiskTerm;
			THEAPP.Struct_PreferenceStruct.m_iDiskTimeH = PreferenceDlg.m_iDiskTimeH;
			THEAPP.Struct_PreferenceStruct.m_iDiskTimeM = PreferenceDlg.m_iDiskTimeM;
			THEAPP.Struct_PreferenceStruct.m_bDiskTermUse = PreferenceDlg.m_bDiskTermUse;

			THEAPP.Struct_PreferenceStruct.m_iEmptyCircleRadius = PreferenceDlg.m_iEditEmptyCircleRadius;
			THEAPP.Struct_PreferenceStruct.m_iEmptyMaxEdgePoint = PreferenceDlg.m_iEditEmptyMaxEdgePoint;

			THEAPP.Struct_PreferenceStruct.m_iBarcodeNoReadImageType = PreferenceDlg.m_iRadioBarcodeNoReadImageType;

			THEAPP.Struct_PreferenceStruct.m_sProductName = PreferenceDlg.m_sEditProductName;

			//0524
			THEAPP.Struct_PreferenceStruct.m_bUseGrabErrFIltering = PreferenceDlg.m_bCheckUseGrabErrFIltering;
			THEAPP.Struct_PreferenceStruct.m_iGrabErrDarkAreaGVLimit = PreferenceDlg.m_iEditGrabErrDarkAreaGVLimit;
			THEAPP.Struct_PreferenceStruct.m_iGrabErrBrightAreaGVLimit = PreferenceDlg.m_iEditGrabErrBrightAreaGVLimit;
			THEAPP.Struct_PreferenceStruct.m_iGrabErrRetryNo = PreferenceDlg.m_iEditGrabErrRetryNo;
			THEAPP.Struct_PreferenceStruct.m_iTriggerSleepTime = PreferenceDlg.m_iEditTriggerSleepTime;

			//0524
			THEAPP.m_pCameraManager->m_bUseGrabErrFIltering = THEAPP.Struct_PreferenceStruct.m_bUseGrabErrFIltering;
			THEAPP.m_pCameraManager->m_iGrabErrDarkAreaGVLimit = THEAPP.Struct_PreferenceStruct.m_iGrabErrDarkAreaGVLimit;
			THEAPP.m_pCameraManager->m_iGrabErrBrightAreaGVLimit = THEAPP.Struct_PreferenceStruct.m_iGrabErrBrightAreaGVLimit;

			THEAPP.Struct_PreferenceStruct.m_iLightErrorMinLimit = PreferenceDlg.m_iEditLightErrorMinLimit;
			THEAPP.Struct_PreferenceStruct.m_iLightErrorMaxLimit = PreferenceDlg.m_iEditLightErrorMaxLimit;

			THEAPP.Struct_PreferenceStruct.m_iDefectWarningDefectCount = PreferenceDlg.m_iEditDefectWarningDefectCount;
			THEAPP.Struct_PreferenceStruct.m_iDefectWarningDefectDistance = PreferenceDlg.m_iEditDefectWarningDefectDistance;

			THEAPP.Struct_PreferenceStruct.m_iCamFOVType = PreferenceDlg.m_iRadioCamFOVType;
			THEAPP.Struct_PreferenceStruct.m_iPickerTrayDir = PreferenceDlg.m_iRadioPickerTrayDir;

			THEAPP.Struct_PreferenceStruct.m_iLensResultImageX = PreferenceDlg.m_iEditLensResultImageX;
			THEAPP.Struct_PreferenceStruct.m_iLensResultImageY = PreferenceDlg.m_iEditLensResultImageY;
			THEAPP.Struct_PreferenceStruct.m_dLensResultImageZoom = PreferenceDlg.m_dEditLensResultImageZoom;
			THEAPP.Struct_PreferenceStruct.m_iExtraResultImageX = PreferenceDlg.m_iEditExtraResultImageX;
			THEAPP.Struct_PreferenceStruct.m_iExtraResultImageY = PreferenceDlg.m_iEditExtraResultImageY;
			THEAPP.Struct_PreferenceStruct.m_dExtraResultImageZoom = PreferenceDlg.m_dEditExtraResultImageZoom;

			THEAPP.Struct_PreferenceStruct.m_bIsUseAIInsp = PreferenceDlg.m_bIsUseAIInsp;
			THEAPP.Struct_PreferenceStruct.m_bIsApplyAIResult = PreferenceDlg.m_bIsApplyAIResult;
			THEAPP.Struct_PreferenceStruct.m_bIsApplyAISimulation = PreferenceDlg.m_bIsApplyAISimulation;
			THEAPP.Struct_PreferenceStruct.m_bUseAIResizeImageSave = PreferenceDlg.m_bCheckUseAIResizeImageSave;
			THEAPP.Struct_PreferenceStruct.m_iAICropImageWidth = PreferenceDlg.m_iEditAICropImageWidth;
			THEAPP.Struct_PreferenceStruct.m_iAICropImageHeight = PreferenceDlg.m_iEditAICropImageHeight;
			THEAPP.Struct_PreferenceStruct.m_iAIResizeImageWidth = PreferenceDlg.m_iEditAIResizeImageWidth;
			THEAPP.Struct_PreferenceStruct.m_iAIResizeImageHeight = PreferenceDlg.m_iEditAIResizeImageHeight;

			THEAPP.Struct_PreferenceStruct.m_strADJIPAddress = PreferenceDlg.m_strADJIPAddress;
			THEAPP.Struct_PreferenceStruct.m_iADJPortNo = PreferenceDlg.m_iADJPortNo;
			THEAPP.Struct_PreferenceStruct.m_iADJImageCropType = PreferenceDlg.m_iADJImageCropType;
			THEAPP.Struct_PreferenceStruct.m_strADJModelName = PreferenceDlg.m_strADJModelName;
			THEAPP.Struct_PreferenceStruct.m_iADJDelayTime = PreferenceDlg.m_iADJDelayTime;

			THEAPP.Struct_PreferenceStruct.m_bIsUseADJRunCheck = PreferenceDlg.m_bIsUseADJRunCheck;
			THEAPP.Struct_PreferenceStruct.m_strADJExePath = PreferenceDlg.m_strADJExePath;

			//Multiple Defect Start
			THEAPP.Struct_PreferenceStruct.m_bUseMultipleDefectAlarm = PreferenceDlg.m_bCheckUseLotDefectAlarm;
			THEAPP.Struct_PreferenceStruct.m_bUseMultipleDefectAlarmAreaSimilarity = PreferenceDlg.m_bCheckUseLotDefectAlarmAreaSimilarity;
			THEAPP.Struct_PreferenceStruct.m_bUseMultipleDefectSendAlarm = PreferenceDlg.m_bCheckUseMultipleDefectSendAlarm;
			THEAPP.Struct_PreferenceStruct.m_iMultipleDefectAlarmSamePosTolerance = PreferenceDlg.m_iEditLotDefectAlarmSamePosTolerance;
			THEAPP.Struct_PreferenceStruct.m_iMultipleDefectAlarmAreaSimilarityTolerance = PreferenceDlg.m_iEditLotDefectAlarmAreaSimilarityTolerance;
			THEAPP.Struct_PreferenceStruct.m_iMultipleDefectAlarmDefectRatio = PreferenceDlg.m_iEditLotDefectAlarmDefectRatio;
			THEAPP.Struct_PreferenceStruct.m_iMultipleDefectAlarmMinimumNum = PreferenceDlg.m_iEditLotDefectAlarmMinimumNum;
			//Multiple Defect End

			THEAPP.Struct_PreferenceStruct.m_bChangeEvmsDirectory = PreferenceDlg.m_bCheckChangeEvmsDirectory; //Ver2629
			THEAPP.m_bUseEvms = THEAPP.Struct_PreferenceStruct.m_bChangeEvmsDirectory; //Ver2629
			THEAPP.Struct_PreferenceStruct.m_bUseAbsolutePathModel = PreferenceDlg.m_bCheckUseAbsolutePathModel; //Ver2629
			THEAPP.Struct_PreferenceStruct.m_bUseAbsolutePathModel = FALSE;

			THEAPP.Struct_PreferenceStruct.m_iResultTextPosX = PreferenceDlg.m_iEditResultTextPosX; //Result Text
			THEAPP.Struct_PreferenceStruct.m_iResultTextPosY = PreferenceDlg.m_iEditResultTextPosY; //Result Text
			THEAPP.Struct_PreferenceStruct.m_iResultTextSize = PreferenceDlg.m_iEditResultTextSize; //Result Text

			THEAPP.Struct_PreferenceStruct.m_bSpecialNGSort = PreferenceDlg.m_bCheckSpecialNGSort; // 25.02.12 - v3008 - Special NG 분리배출 추가 - LeeGW

			THEAPP.Struct_PreferenceStruct.m_bSaveRawImageResize = PreferenceDlg.m_bCheckSaveRawImageResize;	// 24.07.31 - v2656 - RAWIMAGE 축소, 합성 저장 기능 추가 - LeeGW
			THEAPP.Struct_PreferenceStruct.m_dSaveRawImageResizeRatio = PreferenceDlg.m_dEditSaveRawImageResizeRatio;	// 24.07.31 - v2656 - RAWIMAGE 축소, 합성 저장 기능 추가 - LeeGW
			THEAPP.Struct_PreferenceStruct.m_bSaveRawImageMerge = PreferenceDlg.m_bCheckSaveRawImageMerge;	// 24.07.31 - v2656 - RAWIMAGE 축소, 합성 저장 기능 추가 - LeeGW

			THEAPP.Struct_PreferenceStruct.m_bUseGrabErrFIltering2 = PreferenceDlg.m_bCheckUseGrabErrFIltering2;	// 24.08.09 - v2658 - 잘린 영상 필터링 추가 - LeeGW
			THEAPP.Struct_PreferenceStruct.m_iGrabErrSubGVLimit = PreferenceDlg.m_iEditGrabErrSubGVLimit;	// 24.08.09 - v2658 - 잘린 영상 필터링 추가 - LeeGW

			THEAPP.Struct_PreferenceStruct.m_iMachineInspType = PreferenceDlg.m_iRadioMachineInspType;

			THEAPP.Struct_PreferenceStruct.m_bUseProcessRestart = PreferenceDlg.m_bCheckUseProcessRestart;					// 24.12.02 - v3005 - 프로세스 초기화 추가 - LeeGW
			THEAPP.Struct_PreferenceStruct.m_dProcessRestartMemoryLimit = PreferenceDlg.m_dEditProcessRestartMemoryLimit;	// 24.12.02 - v3005 - 프로세스 초기화 추가 - LeeGW
			THEAPP.Struct_PreferenceStruct.m_iProcessRestartTimeMin = PreferenceDlg.m_iEditProcessRestartTimeMin;			// 24.12.02 - v3005 - 프로세스 초기화 추가 - LeeGW
			THEAPP.Struct_PreferenceStruct.m_iProcessRestartTimeMax = PreferenceDlg.m_iEditProcessRestartTimeMax;			// 24.12.02 - v3005 - 프로세스 초기화 추가 - LeeGW	
			THEAPP.Struct_PreferenceStruct.m_iProcessRestartDelayTime = PreferenceDlg.m_iEditProcessRestartDelayTime;			// 24.12.02 - v3005 - 프로세스 초기화 추가 - LeeGW			

			THEAPP.Struct_PreferenceStruct.m_bUseMboMode = PreferenceDlg.m_bCheckUseMboMode;					// 24.12.02 - v3005 - 프로세스 초기화 추가 - LeeGW
			THEAPP.Struct_PreferenceStruct.m_iMboModeCount = PreferenceDlg.m_iEditMboModeCount;	// 24.12.02 - v3005 - 프로세스 초기화 추가 - LeeGW

			THEAPP.Struct_PreferenceStruct.m_bUseMboMode = PreferenceDlg.m_bCheckUseMboMode;					// 24.12.02 - v3005 - 프로세스 초기화 추가 - LeeGW
			THEAPP.Struct_PreferenceStruct.m_iMboModeCount = PreferenceDlg.m_iEditMboModeCount;	// 24.12.02 - v3005 - 프로세스 초기화 추가 - LeeGW

			THEAPP.Struct_PreferenceStruct.m_bUseHandlerRetry = PreferenceDlg.m_bCheckUseHandlerRetry;	// 25.06.24 - LeeGW
			THEAPP.Struct_PreferenceStruct.m_iHandlerRetryCount = PreferenceDlg.m_iEditHandlerRetryCount;	// 25.06.24 - LeeGW
			THEAPP.Struct_PreferenceStruct.m_iHandlerRetryWaitTime = PreferenceDlg.m_iEditHandlerRetryWaitTime;	// 25.06.24 - LeeGW
			THEAPP.Struct_PreferenceStruct.m_iHandlerReplyWaitTime = PreferenceDlg.m_iEditHandlerReplyWaitTime;	// 25.06.24 - LeeGW

			THEAPP.Struct_PreferenceStruct.m_strRmsSaveFolderPath = PreferenceDlg.m_strEditRmsSaveFolderPath; //RMS

		}
		else
		{
			return;
		}
	}
	else
	{
		return;
	}

	//////////// $$$ 이미지 저장 포맷 결정 //////////////////////////////////
	THEAPP.m_pInspectAdminViewHideDlg->m_sImageFormat = ".bmp";
	if (PreferenceDlg.m_bSaveBMP) THEAPP.m_pInspectAdminViewHideDlg->m_sImageFormat = ".bmp";
	else THEAPP.m_pInspectAdminViewHideDlg->m_sImageFormat = ".jpg";
}


void CuScanView::OnBnClickedMfcbuttonFormExit()
{
	SetExitProgram();
}


void CuScanView::OnBnClickedButtonModeChange()
{
	if(THEAPP.m_iModeSwitch == MODE_INSPECT_VIEW_OPERATOR)
	{
		CAdminPasswordDlg dlg;
		if(dlg.DoModal()!=IDOK)
		{
			return;
		}
	}
	switch(THEAPP.m_iModeSwitch)
	{
	case MODE_INSPECT_VIEW_ADMIN: /// 작업자 Mode 화면 전환
		{

			THEAPP.m_pInspectAdminViewDlg->ShowWindow(SW_SHOWNA);
			THEAPP.m_pDefectListDlg->ShowWindow(SW_HIDE);
			THEAPP.m_pTrayAdminViewDlg->ShowWindow(SW_HIDE);
			THEAPP.m_pLogDlg->SetPosition(VIEW1_DLG3_LEFT+520, VIEW1_DLG1_TOP+480, VIEW1_DLG3_WIDTH-530, 440);
			THEAPP.m_pLogDlg->Show();

			THEAPP.m_pTrayOperatorViewPrevDlg->Show();
			THEAPP.m_pTrayOperatorViewNowDlg->Show();

			GetDlgItem(IDC_MFCBUTTON_FORM_NEW)->EnableWindow(FALSE);
			GetDlgItem(IDC_MFCBUTTON_FORM_LOAD)->EnableWindow(FALSE);
			GetDlgItem(IDC_MFCBUTTON_FORM_SAVE)->EnableWindow(FALSE);
			GetDlgItem(IDC_MFCBUTTON_FORM_TEACHING)->EnableWindow(FALSE);
			GetDlgItem(IDC_MFCBUTTON_FORM_PREFERENCE)->EnableWindow(FALSE);
			GetDlgItem(IDC_MFCBUTTON_FORM_REGISTER)->EnableWindow(FALSE);


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
			THEAPP.m_pTrayAdminViewDlg->m_bPrevTray=TRUE;
			THEAPP.m_bShowReviewWindow=FALSE;

			THEAPP.m_pInspectResultDlg->GetDlgItem(IDC_EDIT_RESET_TIME_HR)->EnableWindow(FALSE);
			THEAPP.m_pInspectResultDlg->GetDlgItem(IDC_BUTTON_SET_RESET_TIME)->EnableWindow(FALSE);
			if(THEAPP.Struct_PreferenceStruct.m_bCheckBeamProject==TRUE)
			{
				THEAPP.m_pTrayBeamProjectViewDlg->Show();/// beamproject 설치 후
			}

			CString strRegisterFolder = THEAPP.GetWorkingDirectory()+"\\Data\\";
			CIniFileCS INI(strRegisterFolder + "\\RegisterInfo.ini");
			INI.Set_String("Login Mode", "Mode", "Op");
			INI.Set_String("Operator", "Operator", "AVI");
				
			THEAPP.m_iModeSwitch = MODE_INSPECT_VIEW_OPERATOR;
			break;
		}
	case MODE_INSPECT_VIEW_OPERATOR: /// 관리자 Mode 화면 전환
		{
			THEAPP.m_pInspectAdminViewDlg->ShowWindow(SW_SHOW);
			THEAPP.m_pDefectListDlg->ShowWindow(SW_SHOW);
			THEAPP.m_pLogDlg->SetPosition(VIEW1_DLG3_LEFT+70, VIEW1_DLG1_TOP+480, VIEW1_DLG3_WIDTH-570, 440);

			THEAPP.m_pTrayOperatorViewPrevDlg->ShowWindow(SW_HIDE);
			THEAPP.m_pTrayOperatorViewNowDlg->ShowWindow(SW_HIDE);

			THEAPP.m_pTrayAdminViewDlg->Show();
			THEAPP.m_pLogDlg->Show();

			if (THEAPP.m_pInspectService->m_bInspectRun == FALSE)
			{
				GetDlgItem(IDC_MFCBUTTON_FORM_NEW)->EnableWindow(TRUE);
				GetDlgItem(IDC_MFCBUTTON_FORM_LOAD)->EnableWindow(TRUE);
				GetDlgItem(IDC_MFCBUTTON_FORM_SAVE)->EnableWindow(TRUE);
				GetDlgItem(IDC_MFCBUTTON_FORM_TEACHING)->EnableWindow(TRUE);
			}
			GetDlgItem(IDC_MFCBUTTON_FORM_PREFERENCE)->EnableWindow(TRUE);
			GetDlgItem(IDC_MFCBUTTON_FORM_REGISTER)->EnableWindow(TRUE);


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
			THEAPP.m_pTrayAdminViewDlg->m_bPrevTray=TRUE;
			THEAPP.m_bShowReviewWindow=FALSE;

			THEAPP.m_pInspectResultDlg->GetDlgItem(IDC_EDIT_RESET_TIME_HR)->EnableWindow(TRUE);
			THEAPP.m_pInspectResultDlg->GetDlgItem(IDC_BUTTON_SET_RESET_TIME)->EnableWindow(TRUE);

			THEAPP.m_iModeSwitch = MODE_INSPECT_VIEW_ADMIN;
			break;
		}
	}
}


UINT LoadingThread(LPVOID lp)
{
	CuScanView* puScanView = (CuScanView*) lp;
	THEAPP.m_pHandlerService->Set_AMoveRequest(40.0, 139.0, 1);
	THEAPP.m_pHandlerService->m_bMotionMoveComplete = FALSE;
	while (!THEAPP.m_pHandlerService->m_bMotionMoveComplete) // 무브 컴플리트 기다림
	{
		// HS_PAUSE = 4, HS_ERROR = 5, HS_STOP = 6
		if (THEAPP.m_pHandlerService->m_nInspectPCStatus >= HS_PAUSE)
		{
			THEAPP.m_pInspectAdminViewDlg->m_bInspectRunDisplay = FALSE;
			THEAPP.m_pInspectAdminViewHideDlg->m_bInspectRunDisplay = FALSE;

			return 0;
		}
		Sleep(1);
	}
	THEAPP.m_pHandlerService->Set_PositionRequest();
	THEAPP.m_pHandlerService->m_bMotionMoveComplete = FALSE;
	return 0;
}


UINT UnLoadingThread(LPVOID lp)
{
	THEAPP.m_pHandlerService->Set_MoveToUnload();
	THEAPP.m_pHandlerService->m_bMotionMoveComplete = FALSE;
	while(!THEAPP.m_pHandlerService->m_bMotionMoveComplete) // 무브 컴플리트 기다림
	{
		// HS_PAUSE = 4, HS_ERROR = 5, HS_STOP = 6
		if (THEAPP.m_pHandlerService->m_nInspectPCStatus >= HS_PAUSE)
		{
			THEAPP.m_pInspectAdminViewDlg->m_bInspectRunDisplay = FALSE;
			THEAPP.m_pInspectAdminViewHideDlg->m_bInspectRunDisplay = FALSE;

			return 0;
		}
		Sleep(1);
	}
	THEAPP.m_pHandlerService->Set_PositionRequest();
	THEAPP.m_pHandlerService->m_bMotionMoveComplete = FALSE;
	return 0;
}

void CuScanView::OnBnClickedButtonTrayLoading()
{
	return;

#ifdef INLINE_MODE
	AfxBeginThread(LoadingThread,this);
#endif
}


void CuScanView::OnBnClickedButtonTrayUnloading()
{
	return;

#ifdef INLINE_MODE
	AfxBeginThread(UnLoadingThread,this);
#endif
}

void CuScanView::CheckDiskSpace()
{
	if (THEAPP.Struct_PreferenceStruct.m_bDiskTermUse==FALSE)
		return;

	SYSTEMTIME time;
	GetLocalTime(&time);

	BOOL bDiskClean = FALSE;
	if (time.wHour == THEAPP.Struct_PreferenceStruct.m_iDiskTimeH && time.wMinute == THEAPP.Struct_PreferenceStruct.m_iDiskTimeM)
		bDiskClean = TRUE;

#ifdef INLINE_MODE

	if (bDiskClean)
		AfxBeginThread(DiskCleanCheckThread, this);

#endif
}

void CuScanView::OnBnClickedMfcbuttonFormRegister()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	COperator_RegisterDlg dlg;

	if(dlg.DoModal() != IDOK)
	{
		return;
	}

	THEAPP.m_pModelDataManager->m_strLogin_Mode = dlg.m_strLogin_Mode;
	THEAPP.m_pModelDataManager->m_strOperator	= dlg.m_strOperator;
	THEAPP.m_pModelDataManager->m_strPwd		= dlg.m_strPwd;
}
