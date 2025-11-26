// JogSetDlg.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "uScan.h"
#include "AutoFocusDlg.h"
#include "afxdialogex.h"
#include "IniFileCS.h"


// CAutoFocusDlg 대화 상자입니다.
CAutoFocusDlg* CAutoFocusDlg::m_pInstance = NULL;

CAutoFocusDlg* CAutoFocusDlg::GetInstance(BOOL bShowFlag)
{
	if (!m_pInstance) {
		m_pInstance = new CAutoFocusDlg();
		if(!m_pInstance->m_hWnd) {
			CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
			m_pInstance->Create(IDD_AUTO_FOCUS_DLG, pFrame->GetActiveView());
			if (bShowFlag) m_pInstance->Show();
		}
	}
	return m_pInstance;
}

void CAutoFocusDlg::DeleteInstance()
{
	if(m_pInstance->GetSafeHwnd())
		m_pInstance->DestroyWindow();
	SAFE_DELETE(m_pInstance);
}

void CAutoFocusDlg::Show()
{
	ShowWindow(SW_HIDE);
	MoveWindow(m_ScreenRect.left, m_ScreenRect.top, m_ScreenRect.right, m_ScreenRect.bottom);
	ShowWindow(SW_SHOW);
}

IMPLEMENT_DYNAMIC(CAutoFocusDlg, CDialog)

CAutoFocusDlg::CAutoFocusDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CAutoFocusDlg::IDD, pParent)
{
	SetPosition(VIEW1_DLG3_LEFT+120+VIEW1_DLG3_WIDTH-540, VIEW1_DLG3_TOP+40+50, VIEW1_DLG3_WIDTH-610, VIEW1_DLG3_HEIGHT+170);

	m_dEditAMovePosition = 5.0;
	m_dEditJMovePosition = 0.1;

	m_dEditAFStartPosition = 4.0;
	m_dEditAFEndPosition = 6.0;
	m_dEditAFMoveInterval = 0.1;

	m_dCurZPosition = 5.0;
}

CAutoFocusDlg::~CAutoFocusDlg()
{
}

void CAutoFocusDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_AMOVE_POSITION, m_dEditAMovePosition);
	DDX_Text(pDX, IDC_EDIT_JMOVE_POSITION, m_dEditJMovePosition);
	DDX_Text(pDX, IDC_EDIT_AF_START_POSITION, m_dEditAFStartPosition);
	DDX_Text(pDX, IDC_EDIT_AF_END_POSITION, m_dEditAFEndPosition);
	DDX_Text(pDX, IDC_EDIT_AF_MOVE_INTERVAL, m_dEditAFMoveInterval);

	DDX_Control(pDX, IDC_BUTTON_GET_CURRENT_POSITION, m_bnGetCurrentPos);
	DDX_Control(pDX, IDC_BUTTON_AMOVE, m_bnAMove);
	DDX_Control(pDX, IDC_BUTTON_JMOVE_PLUS, m_bnJMovePlus);
	DDX_Control(pDX, IDC_BUTTON_JMOVE_MINUS, m_bnJMoveMinus);
	DDX_Control(pDX, IDC_BUTTON_ROI_AF, m_bnAFMakeROI);
	DDX_Control(pDX, IDC_BUTTON_START_AF, m_bnAFStart);
}


BEGIN_MESSAGE_MAP(CAutoFocusDlg, CDialog)
	ON_BN_CLICKED(IDC_BUTTON_GET_CURRENT_POSITION, &CAutoFocusDlg::OnBnClickedButtonGetCurrentPosition)
	ON_BN_CLICKED(IDC_BUTTON_AMOVE, &CAutoFocusDlg::OnBnClickedButtonAmove)
	ON_BN_CLICKED(IDC_BUTTON_JMOVE_PLUS, &CAutoFocusDlg::OnBnClickedButtonJmovePlus)
	ON_BN_CLICKED(IDC_BUTTON_JMOVE_MINUS, &CAutoFocusDlg::OnBnClickedButtonJmoveMinus)
	ON_BN_CLICKED(IDC_BUTTON_START_AF, &CAutoFocusDlg::OnBnClickedButtonStartAf)
	ON_BN_CLICKED(IDC_BUTTON_ROI_AF, &CAutoFocusDlg::OnBnClickedButtonRoiAf)
END_MESSAGE_MAP()


// CAutoFocusDlg 메시지 처리기입니다.

BOOL CAutoFocusDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_bnGetCurrentPos.Init_Ctrl(_T("Arial"), 9, FALSE, BLACK, ROYAL_BLUE, 0, 0);
	m_bnAMove.Init_Ctrl(_T("Arial"), 9, FALSE, BLACK, ROYAL_BLUE, 0, 0);
	m_bnJMovePlus.Init_Ctrl(_T("Arial"), 9, FALSE, BLACK, ROYAL_BLUE, 0, 0);
	m_bnJMoveMinus.Init_Ctrl(_T("Arial"), 9, FALSE, BLACK, ROYAL_BLUE, 0, 0);
	m_bnAFMakeROI.Init_Ctrl(_T("Arial"), 9, FALSE, BLACK, ROYAL_BLUE, 0, 0);
	m_bnAFStart.Init_Ctrl(_T("Arial"), 9, FALSE, BLACK, ROYAL_BLUE, 0, 0);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

UINT AMoveRequest_Thread(LPVOID lp)
{
	CAutoFocusDlg* pJogSetDlg = (CAutoFocusDlg*)lp;

	if (THEAPP.m_pModelDataManager->m_sModelName == ".") 
		return 0;

	THEAPP.m_pHandlerService->m_bMotionMoveComplete = FALSE;
	THEAPP.m_pHandlerService->Set_ZMoveRequest(pJogSetDlg->m_dCurZPosition);
	while(!THEAPP.m_pHandlerService->m_bMotionMoveComplete) // 무브 컴플리트 기다림
	{
		Sleep(1);
	}

	THEAPP.m_pHandlerService->Set_PositionRequest();

	return 1;
}

UINT AutoFocus_Thread(LPVOID lp)
{
	CAutoFocusDlg* pJogSetDlg = (CAutoFocusDlg*)lp;

	double dStartPos;
	double dEndPos;
	double dStep = fabs(pJogSetDlg->m_dEditAFMoveInterval);

	if (pJogSetDlg->m_dEditAFStartPosition >= pJogSetDlg->m_dEditAFEndPosition)	// small -> big
	{
		dStartPos = pJogSetDlg->m_dEditAFEndPosition;
		dEndPos = pJogSetDlg->m_dEditAFStartPosition;
	}
	else
	{
		dStartPos = pJogSetDlg->m_dEditAFStartPosition;
		dEndPos = pJogSetDlg->m_dEditAFEndPosition;
	}

	Hobject HImageReduced, HEdgeAmp;

	DWORD dwGrabStart = 0, dwGrabEnd = 0;
	DWORD dwGrabElapsedTime = 0;

	double dPeakContrastZPos = -1;
	double dPeakContrastAmp = -1;
	double dCurZPos = dStartPos;

	double dMeanAmp, dSdAmp;

	THEAPP.m_pInspectAdminViewDlg->m_bLive = TRUE;

	while (dCurZPos)
	{
		THEAPP.m_pHandlerService->m_bMotionMoveComplete = FALSE;
		THEAPP.m_pHandlerService->Set_ZMoveRequest(dCurZPos);
		while(!THEAPP.m_pHandlerService->m_bMotionMoveComplete) // 무브 컴플리트 기다림
		{
			Sleep(1);
		}

		THEAPP.m_pHandlerService->Set_PositionRequest();

		THEAPP.m_pCameraManager->CallHookFunction();

		if (THEAPP.m_pModelDataManager->m_iBlackMatchingImageType < TRIGGER_EXTRA1)
			THEAPP.m_pTriggerManager->FireTrigger(VISION_TYPE_INSPECTION, THEAPP.m_pModelDataManager->m_iBlackMatchingImageType);

		Sleep(THEAPP.Struct_PreferenceStruct.m_iTriggerSleepTime);

		dwGrabStart = GetTickCount();
		while (1)
		{
			if (THEAPP.m_pCameraManager->m_bGrabDone)
			{
				break;
			}

			dwGrabEnd = GetTickCount();

			if((dwGrabEnd-dwGrabStart) > MAX_GRAB_TIMEOUT)
			{
				THEAPP.m_pCameraManager->GrabErrorPostProcess();
				break;
			}
		}

		reduce_domain(THEAPP.m_pInspectAdminViewDlg->m_pHImage, THEAPP.m_pInspectAdminViewDlg->m_HAutoFocusCheckRgn, &HImageReduced);

		sobel_amp(HImageReduced, &HEdgeAmp, "sum_abs", 3);
		intensity(THEAPP.m_pInspectAdminViewDlg->m_HAutoFocusCheckRgn, HEdgeAmp, &dMeanAmp, &dSdAmp);

		if (dMeanAmp>dPeakContrastAmp)
		{
			dPeakContrastAmp = dMeanAmp;
			dPeakContrastZPos = dCurZPos;
		}

		dCurZPos += dStep;

		if (dCurZPos>dEndPos)
			break;
	}

	if (dPeakContrastAmp>0)
	{
		THEAPP.m_pHandlerService->m_bMotionMoveComplete = FALSE;
		THEAPP.m_pHandlerService->Set_ZMoveRequest(dPeakContrastZPos);
		while(!THEAPP.m_pHandlerService->m_bMotionMoveComplete) // 무브 컴플리트 기다림
		{
			Sleep(1);
		}

		THEAPP.m_pHandlerService->Set_PositionRequest();

		THEAPP.m_pCameraManager->CallHookFunction();
	
		if (THEAPP.m_pModelDataManager->m_iBlackMatchingImageType < TRIGGER_EXTRA1)
			THEAPP.m_pTriggerManager->FireTrigger(VISION_TYPE_INSPECTION, THEAPP.m_pModelDataManager->m_iBlackMatchingImageType);

		Sleep(THEAPP.Struct_PreferenceStruct.m_iTriggerSleepTime);

		dwGrabStart = GetTickCount();
		while (1)
		{
			if (THEAPP.m_pCameraManager->m_bGrabDone)
			{
				break;
			}

			dwGrabEnd = GetTickCount();

			if((dwGrabEnd-dwGrabStart) > MAX_GRAB_TIMEOUT)
			{
				THEAPP.m_pCameraManager->GrabErrorPostProcess();
				break;
			}
		}

		CString sMsg;
		sMsg.Format("AutoFocusing 완료!!! Best Focus Z 위치: %.3lf", dPeakContrastZPos);

		AfxMessageBox(sMsg, MB_ICONINFORMATION|MB_SYSTEMMODAL);
	}
	else
	{
		AfxMessageBox("AutoFocusing 완료.", MB_ICONINFORMATION|MB_SYSTEMMODAL);
	}

	THEAPP.m_pInspectAdminViewDlg->m_bLive = FALSE;
	THEAPP.m_pInspectAdminViewDlg->StartLive();

	return 0;
}

void CAutoFocusDlg::UpdateViewParam()
{
	UpdateData(TRUE);
	UpdateData(FALSE);
}

void CAutoFocusDlg::OnBnClickedButtonGetCurrentPosition()
{
	UpdateData(TRUE);
	UpdateData(FALSE);

#ifdef INLINE_MODE
	THEAPP.m_pHandlerService->Set_PositionRequest();
#endif
}

void CAutoFocusDlg::OnBnClickedButtonAmove()
{
	UpdateData(TRUE);
	UpdateData(FALSE);

#ifdef INLINE_MODE

	UpdateData(TRUE);
	m_dCurZPosition = m_dEditAMovePosition;
	
	AfxBeginThread(AMoveRequest_Thread, this);

#endif
}

void CAutoFocusDlg::OnBnClickedButtonJmovePlus()
{

}


void CAutoFocusDlg::OnBnClickedButtonJmoveMinus()
{

}

void CAutoFocusDlg::OnBnClickedButtonRoiAf()
{
	UpdateData(TRUE);
	UpdateData(FALSE);

	AfxMessageBox("AutoFocusing을 계산할 영역을 그려주세요.", MB_ICONINFORMATION|MB_SYSTEMMODAL);

	THEAPP.m_pInspectAdminViewDlg->CreateAutoFocusROI();
}

void CAutoFocusDlg::OnBnClickedButtonStartAf()
{
	UpdateData(TRUE);
	UpdateData(FALSE);

#ifdef INLINE_MODE

	if (THEAPP.m_pGFunction->ValidHRegion(THEAPP.m_pInspectAdminViewDlg->m_HAutoFocusCheckRgn)==FALSE)
	{
		AfxMessageBox("먼저, AutoFocusing을 계산할 영역을 그려주세요.", MB_ICONINFORMATION|MB_SYSTEMMODAL);
		return;
	}

	int nRes = 0;
	nRes = AfxMessageBox("현재 설정으로 Auto Focusing 작업을 진행하시겠습니까?", MB_YESNO|MB_SYSTEMMODAL);

	if (nRes!=IDYES)
		return;

	UpdateData(TRUE);

	THEAPP.m_pInspectAdminViewDlg->StopLive();
	Sleep(100);

	AfxBeginThread(AutoFocus_Thread, this);

#endif
}

