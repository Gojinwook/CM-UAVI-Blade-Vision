// TeachParamDlg.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "uScan.h"
#include "TeachParamDlg.h"
#include "afxdialogex.h"


// CTeachParamDlg 대화 상자입니다.
IMPLEMENT_DYNAMIC(CTeachParamDlg, CDialog)

CTeachParamDlg::CTeachParamDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CTeachParamDlg::IDD, pParent)
{
	m_iImageIdx = 0;
	m_iPrevInspectTabIndex = 0;

	SetPosition(VIEW1_DLG3_LEFT+100, VIEW1_DLG3_TOP+40+50+VIEW1_DLG3_HEIGHT+270-100, VIEW1_DLG3_WIDTH-110, VIEW1_DLG3_HEIGHT+110);

	for (int i = 0; i < MAX_TEACHING_TAB; i++)
	{
		m_pTeachParamDlg_Tab[i] = NULL;
	}

	for (int i = 0; i < MAX_ROI_TEACHING_TAB; i++)
	{
		m_pTeachParamROIDlg_Tab[i] = NULL;
	}
	
	if(!m_hWnd)
	{
		CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
		Create(IDD_TEACH_PARAM_DLG, pFrame->GetActiveView());
	}
}

CTeachParamDlg::~CTeachParamDlg()
{
	for (int i = 0; i < MAX_TEACHING_TAB; i++)
	{
		SAFE_DELETE(m_pTeachParamDlg_Tab[i]);
	}

	for (int i = 0; i < MAX_ROI_TEACHING_TAB; i++)
	{
		SAFE_DELETE(m_pTeachParamROIDlg_Tab[i]);
	}
}

void CTeachParamDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_ALGORITHM_TAB, m_AlgorithmTabCtrl);
	DDX_Control(pDX, IDC_BUTTON_TAB_TEST, m_bnAlgorithmTest);
}


BEGIN_MESSAGE_MAP(CTeachParamDlg, CDialog)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_NOTIFY(TCN_SELCHANGE, IDC_ALGORITHM_TAB, &CTeachParamDlg::OnSelchangeAlgorithmTab)
	ON_BN_CLICKED(IDC_BUTTON_TAB_TEST, &CTeachParamDlg::OnBnClickedButtonTabTest)

	ON_MESSAGE(UM_TS_ROI_SELECTED, OnEventROISelected)
	ON_MESSAGE(UM_TS_ROI_INSPECTION_TAB_SAVE, OnEventROIInspectionTabSave)
END_MESSAGE_MAP()


void CTeachParamDlg::Show()
{
	ShowWindow(SW_HIDE);
	MoveWindow(m_ScreenRect.left, m_ScreenRect.top, m_ScreenRect.right, m_ScreenRect.bottom);
	ShowWindow(SW_SHOW);
}

// CTeachParamDlg 메시지 처리기입니다.

int CTeachParamDlg::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDialog::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  여기에 특수화된 작성 코드를 추가합니다.

	return 0;
}


void CTeachParamDlg::OnDestroy()
{
	CDialog::OnDestroy();

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	THEAPP.m_pInspectAdminViewDlg->delegateROISelected[m_iImageIdx] -= this;
	THEAPP.m_pInspectAdminViewDlg->delegateCurROITabParamSave[m_iImageIdx] -= this;
}


BOOL CTeachParamDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.

	if(pMsg->message==WM_KEYDOWN&&pMsg->wParam==VK_ESCAPE)
		return TRUE;
	if(pMsg->message==WM_KEYDOWN&&pMsg->wParam==VK_RETURN)
		return TRUE;
	return CDialog::PreTranslateMessage(pMsg);
}

BOOL CTeachParamDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.
	m_AlgorithmTabCtrl.DeleteAllItems();
	for (int i = 0; i < MAX_TEACHING_TAB; i++)
	{
		CString temp;
		temp.Format(" 검사 %d ", i+1);

		m_AlgorithmTabCtrl.InsertItem(i, temp);
	}

	for (int i = 0; i < MAX_ROI_TEACHING_TAB; i++)
	{
		CString temp;
		temp.Format(" 검사 %d(ROI) ", i + 1);

		m_AlgorithmTabCtrl.InsertItem(MAX_TEACHING_TAB+i, temp);
	}

	CRect DlgRect;

	for (int i = 0; i < MAX_TEACHING_TAB; i++)
	{
		m_pTeachParamDlg_Tab[i] = new CTeachParamDlg_Tab;
		m_pTeachParamDlg_Tab[i]->Create(IDD_TEACH_PARAM_DLG_TAB, &m_AlgorithmTabCtrl);
		m_pTeachParamDlg_Tab[i]->GetWindowRect(&DlgRect);
		m_pTeachParamDlg_Tab[i]->MoveWindow(5, 25, DlgRect.Width(), DlgRect.Height());
		m_pTeachParamDlg_Tab[i]->ShowWindow(SW_HIDE);
		m_pTeachParamDlg_Tab[i]->m_iTabIdx = i;
		m_pTeachParamDlg_Tab[i]->m_iImageIdx = m_iImageIdx;
	}

	for (int i = 0; i < MAX_ROI_TEACHING_TAB; i++)
	{
		m_pTeachParamROIDlg_Tab[i] = new CTeachParamROIDlg_Tab;
		m_pTeachParamROIDlg_Tab[i]->Create(IDD_TEACH_PARAM_ROI_DLG_TAB, &m_AlgorithmTabCtrl);
		m_pTeachParamROIDlg_Tab[i]->GetWindowRect(&DlgRect);
		m_pTeachParamROIDlg_Tab[i]->MoveWindow(5, 25, DlgRect.Width(), DlgRect.Height());
		m_pTeachParamROIDlg_Tab[i]->ShowWindow(SW_HIDE);
		m_pTeachParamROIDlg_Tab[i]->m_iTabIdx = i;
		m_pTeachParamROIDlg_Tab[i]->m_iImageIdx = m_iImageIdx;
	}

	m_pTeachParamDlg_Tab[0]->ShowWindow(SW_SHOW);

	m_bnAlgorithmTest.Init_Ctrl(_T("Arial"), 9, FALSE, BLACK, ROYAL_BLUE, 0, 0);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

void CTeachParamDlg::OnSelchangeAlgorithmTab(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	int iSelected = m_AlgorithmTabCtrl.GetCurSel();

	if (m_iPrevInspectTabIndex != iSelected)
	{
		if (m_iPrevInspectTabIndex < MAX_TEACHING_TAB)
			m_pTeachParamDlg_Tab[m_iPrevInspectTabIndex]->UpDate(TRUE);
		else
			m_pTeachParamROIDlg_Tab[m_iPrevInspectTabIndex - MAX_TEACHING_TAB]->UpDate(TRUE);

		m_iPrevInspectTabIndex = iSelected;
	}

	THEAPP.m_pTabControlDlg->HideAlgorithmDlg();
	THEAPP.m_pInspectAdminViewDlg->m_HInspectAreaRgn.Reset();
	THEAPP.m_pInspectAdminViewDlg->m_HParameterDefectRgn.Reset();
	
	for (int i = 0; i < MAX_TEACHING_TAB+ MAX_ROI_TEACHING_TAB; i++)
	{
		if (i < MAX_TEACHING_TAB)
		{
			if (i == iSelected)
				m_pTeachParamDlg_Tab[i]->ShowWindow(SW_SHOW);
			else
				m_pTeachParamDlg_Tab[i]->ShowWindow(SW_HIDE);

		}
		else
		{
			if (i == iSelected)
				m_pTeachParamROIDlg_Tab[i- MAX_TEACHING_TAB]->ShowWindow(SW_SHOW);
			else
				m_pTeachParamROIDlg_Tab[i- MAX_TEACHING_TAB]->ShowWindow(SW_HIDE);
		}
	}

	if (iSelected < MAX_TEACHING_TAB)
		m_pTeachParamDlg_Tab[iSelected]->UpdateList();
	else
		m_pTeachParamROIDlg_Tab[iSelected- MAX_TEACHING_TAB]->UpdateList();

	*pResult = 0;
}

void CTeachParamDlg::SetInspectParam()
{
	for (int i = 0; i < MAX_TEACHING_TAB; i++)
		m_pTeachParamDlg_Tab[i]->UpDate(FALSE);

	for (int i = 0; i < MAX_ROI_TEACHING_TAB; i++)
		m_pTeachParamROIDlg_Tab[i]->UpDate(FALSE);

	UpdateData(FALSE);
}

void CTeachParamDlg::GetInspectParam()
{
	UpdateData(TRUE);

	for (int i = 0; i < MAX_TEACHING_TAB; i++)
		m_pTeachParamDlg_Tab[i]->UpDate(TRUE);


	for (int i = 0; i < MAX_ROI_TEACHING_TAB; i++)
		m_pTeachParamROIDlg_Tab[i]->UpDate(TRUE);
}

void CTeachParamDlg::OnBnClickedButtonTabTest()
{
	if (THEAPP.m_pModelDataManager->m_sModelName == ".") return;

	GetInspectParam();
	UpdateData(TRUE);

	Hobject HDefectAllRgn, HDefectRgn, HResultAllXld;

	gen_empty_obj(&HDefectAllRgn);
	gen_empty_obj(&HResultAllXld);

	Hobject HEpoxyDontCareRgn;
	gen_empty_obj(&HEpoxyDontCareRgn);


	for (int i = 0; i < MAX_TEACHING_TAB; i++)
	{
		if (THEAPP.m_pModelDataManager->m_AlgorithmParam[m_iImageIdx][i].m_bInspect)
		{
			if (THEAPP.m_pModelDataManager->m_AlgorithmParam[m_iImageIdx][i].m_iDefectNameIdx == DEFECT_NAME_EPOXY)
				HDefectRgn = THEAPP.m_pAlgorithm->CommonAlgorithm(THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[m_iImageIdx], THEAPP.m_pInspectAdminViewDlg->ContoursAffinTrans[MATCHING_CONTOUR_1], THEAPP.m_pInspectAdminViewDlg->ContoursAffinTrans[MATCHING_CONTOUR_2],
					THEAPP.m_pInspectAdminViewDlg->ContoursAffinTrans[MATCHING_CONTOUR_3], THEAPP.m_pInspectAdminViewDlg->ContoursAffinTrans[MATCHING_CONTOUR_4], THEAPP.m_pInspectAdminViewDlg->ContoursAffinTrans[MATCHING_CONTOUR_5], THEAPP.m_pModelDataManager->m_AlgorithmParam[m_iImageIdx][i], &HEpoxyDontCareRgn, &(THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[TRIGGER_EDGE]));

			if (THEAPP.m_pGFunction->ValidHRegion(HDefectRgn) == TRUE)
				concat_obj(HDefectAllRgn, HDefectRgn, &HDefectAllRgn);
		}
	}

	for (int i = 0; i < MAX_TEACHING_TAB; i++)
	{
		if (THEAPP.m_pModelDataManager->m_AlgorithmParam[m_iImageIdx][i].m_bInspect)
		{
			if (THEAPP.m_pModelDataManager->m_AlgorithmParam[m_iImageIdx][i].m_iDefectNameIdx == DEFECT_NAME_EPOXY)
				continue;

			HDefectRgn = THEAPP.m_pAlgorithm->CommonAlgorithm(THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[m_iImageIdx], THEAPP.m_pInspectAdminViewDlg->ContoursAffinTrans[MATCHING_CONTOUR_1], THEAPP.m_pInspectAdminViewDlg->ContoursAffinTrans[MATCHING_CONTOUR_2],
				THEAPP.m_pInspectAdminViewDlg->ContoursAffinTrans[MATCHING_CONTOUR_3], THEAPP.m_pInspectAdminViewDlg->ContoursAffinTrans[MATCHING_CONTOUR_4], THEAPP.m_pInspectAdminViewDlg->ContoursAffinTrans[MATCHING_CONTOUR_5], THEAPP.m_pModelDataManager->m_AlgorithmParam[m_iImageIdx][i], &HEpoxyDontCareRgn);

			if (THEAPP.m_pGFunction->ValidHRegion(HDefectRgn) == TRUE)
				concat_obj(HDefectAllRgn, HDefectRgn, &HDefectAllRgn);
		}
	}

	int iNoInspectROI = 0;
	iNoInspectROI = THEAPP.m_pModelDataManager->m_pInspectionArea->GetChildTRegionCount();

	if (iNoInspectROI > 0)
	{
		GTRegion* pInspectROIRgn;
		for (int iROIIndex = 0; iROIIndex < iNoInspectROI; iROIIndex++)
		{
			pInspectROIRgn = THEAPP.m_pModelDataManager->m_pInspectionArea->GetChildTRegion(iROIIndex);
			pInspectROIRgn->ResetLocalAlignResult(MAX_MODULE_ONE_TRAY - 1);
		}

		GetInspectParam();

		CAlgorithmParam Param;
		Hobject HROIFoundInspectAreaRgn, HROIDefectAreaRgn, HROIResultXld;
		Hobject HFoundInspectAreaRgn, HDefectAreaRgn, HResultXld;

		Hobject HPartCheckAlignRgn, HPartCheckResultRgn;
		gen_empty_obj(&HPartCheckAlignRgn);
		gen_empty_obj(&HPartCheckResultRgn);

		BOOL bAnisoAlignShiftResult;
		Hobject HAnisoAlignRgn;
		gen_empty_obj(&HAnisoAlignRgn);

		gen_empty_obj(&HROIFoundInspectAreaRgn);
		gen_empty_obj(&HROIDefectAreaRgn);
		gen_empty_obj(&HROIResultXld);

		int iImgCnt;

		for (int iIndex = 0; iIndex < iNoInspectROI; iIndex++)
		{
			pInspectROIRgn = THEAPP.m_pModelDataManager->m_pInspectionArea->GetChildTRegion(iIndex);
			if (pInspectROIRgn == NULL)
				continue;

			if (pInspectROIRgn->m_iTeachImageIndex != (m_iImageIdx + 1))
				continue;

			if (pInspectROIRgn->m_iInspectionType != INSPECTION_TYPE_LOCAL_ALIGN)
				continue;

			for (int tab = 0; tab < MAX_ROI_TEACHING_TAB; tab++)
			{
				Hobject HInspectAreaRgn = pInspectROIRgn->GetROIHRegion(THEAPP.m_pCalDataService);
				Param = pInspectROIRgn->m_AlgorithmParam[tab];

				if (Param.m_bInspect)
				{
					// Part Check
					if (Param.m_bROIPartCheckUse && Param.m_bROIPartCheckLocalAlignUse)
					{
						HPartCheckResultRgn = THEAPP.m_pAlgorithm->PartCheckAlgorithm(pInspectROIRgn->m_HPartModelID, THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[m_iImageIdx], HInspectAreaRgn, Param, &pInspectROIRgn->m_iLocalAlignDeltaX[MAX_MODULE_ONE_TRAY - 1], &pInspectROIRgn->m_iLocalAlignDeltaY[MAX_MODULE_ONE_TRAY - 1], &pInspectROIRgn->m_dLocalAlignDeltaAngle[MAX_MODULE_ONE_TRAY - 1], &pInspectROIRgn->m_dLocalAlignDeltaAngleFixedPointX[MAX_MODULE_ONE_TRAY - 1], &pInspectROIRgn->m_dLocalAlignDeltaAngleFixedPointY[MAX_MODULE_ONE_TRAY - 1], &HPartCheckAlignRgn);

						if (THEAPP.m_pGFunction->ValidHRegion(HPartCheckAlignRgn))
							pInspectROIRgn->m_HLocalAlignShapeRgn[MAX_MODULE_ONE_TRAY-1] = HPartCheckAlignRgn;
					}

					// 비등방 얼라인
					if (Param.m_bUseROIAnisoAlign && Param.m_bROIAnisoAlignLocalAlignUse)
					{
						HAnisoAlignRgn = THEAPP.m_pAlgorithm->AnisoAlignAlgorithm(pInspectROIRgn->m_HAnisoModelID, THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[m_iImageIdx], HInspectAreaRgn, Param, &bAnisoAlignShiftResult, &pInspectROIRgn->m_iLocalAlignDeltaX[MAX_MODULE_ONE_TRAY - 1], &pInspectROIRgn->m_iLocalAlignDeltaY[MAX_MODULE_ONE_TRAY - 1], &pInspectROIRgn->m_dLocalAlignDeltaAngle[MAX_MODULE_ONE_TRAY - 1], &pInspectROIRgn->m_dLocalAlignDeltaAngleFixedPointX[MAX_MODULE_ONE_TRAY - 1], &pInspectROIRgn->m_dLocalAlignDeltaAngleFixedPointY[MAX_MODULE_ONE_TRAY - 1]);

						if (THEAPP.m_pGFunction->ValidHRegion(HAnisoAlignRgn))
							pInspectROIRgn->m_HLocalAlignShapeRgn[MAX_MODULE_ONE_TRAY - 1] = HAnisoAlignRgn;
					}
				}
			}
		}

		for (int iIndex = 0; iIndex < iNoInspectROI; iIndex++)
		{
			pInspectROIRgn = THEAPP.m_pModelDataManager->m_pInspectionArea->GetChildTRegion(iIndex);
			if (pInspectROIRgn == NULL)
				continue;

			if (pInspectROIRgn->m_iTeachImageIndex != (m_iImageIdx + 1))
				continue;

			if (pInspectROIRgn->m_iInspectionType != INSPECTION_TYPE_INSPECTION &&
				pInspectROIRgn->m_iInspectionType < INSPECTION_TYPE_AI_INSPECTION)
				continue;

			for (int tab = 0; tab < MAX_ROI_TEACHING_TAB; tab++)
			{

				Param = pInspectROIRgn->m_AlgorithmParam[tab];

				if (Param.m_bInspect)
				{		
					gen_empty_obj(&HDefectAreaRgn);
					gen_empty_obj(&HFoundInspectAreaRgn);
					gen_empty_obj(&HResultXld);

					HDefectAreaRgn = THEAPP.m_pAlgorithm->CommonAlgorithmROI(THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[m_iImageIdx], pInspectROIRgn, Param, &HFoundInspectAreaRgn, &HResultXld, MAX_MODULE_ONE_TRAY - 1);

					concat_obj(HROIFoundInspectAreaRgn, HFoundInspectAreaRgn, &HROIFoundInspectAreaRgn);
					concat_obj(HROIDefectAreaRgn, HDefectAreaRgn, &HROIDefectAreaRgn);
					concat_obj(HROIResultXld, HResultXld, &HROIResultXld);
				}
			}
		}

		concat_obj(HDefectAllRgn, HROIDefectAreaRgn, &HDefectAllRgn);
		concat_obj(HResultAllXld, HROIResultXld, &HResultAllXld);
	}

	if (THEAPP.m_pGFunction->ValidHRegion(HDefectAllRgn) == TRUE)
		union1(HDefectAllRgn, &(THEAPP.m_pInspectAdminViewDlg->m_HParameterDefectRgn));

	if (THEAPP.m_pGFunction->ValidHXLD(HResultAllXld) == TRUE)
		THEAPP.m_pInspectAdminViewDlg->m_HParameterDefectXLD = HResultAllXld;

	THEAPP.m_pInspectAdminViewDlg->UpdateView();
}

void CTeachParamDlg::SetImageIdx(int iImageIdx)
{
	m_iImageIdx = iImageIdx;

	for (int i = 0; i < MAX_TEACHING_TAB; i++)
		m_pTeachParamDlg_Tab[i]->m_iImageIdx = iImageIdx;

	for (int i = 0; i < MAX_ROI_TEACHING_TAB; i++)
		m_pTeachParamROIDlg_Tab[i]->m_iImageIdx = iImageIdx;
}

void CTeachParamDlg::SetDelegate()
{
	THEAPP.m_pInspectAdminViewDlg->delegateROISelected[m_iImageIdx] += new CEventStub(this, UM_TS_ROI_SELECTED);
	THEAPP.m_pInspectAdminViewDlg->delegateCurROITabParamSave[m_iImageIdx] += new CEventStub(this, UM_TS_ROI_INSPECTION_TAB_SAVE);

}


LRESULT CTeachParamDlg::OnEventROISelected(WPARAM wParam, LPARAM lParam)
{
	int iSelectedROITabIndex;

	iSelectedROITabIndex = (int)wParam;

	if (iSelectedROITabIndex != m_iImageIdx)
		return 1;

	if (m_iPrevInspectTabIndex < MAX_TEACHING_TAB)
		m_pTeachParamDlg_Tab[m_iPrevInspectTabIndex]->UpdateList();
	else
		m_pTeachParamROIDlg_Tab[m_iPrevInspectTabIndex - MAX_TEACHING_TAB]->UpdateList();

	SetInspectParam();

	return 1;
}

LRESULT CTeachParamDlg::OnEventROIInspectionTabSave(WPARAM wParam, LPARAM lParam)
{
	int iSelectedROITabIndex;

	iSelectedROITabIndex = (int)wParam;

	if (iSelectedROITabIndex != m_iImageIdx)
		return 1;

	if (m_iPrevInspectTabIndex < MAX_TEACHING_TAB)
	{
		m_pTeachParamDlg_Tab[m_iPrevInspectTabIndex]->UpDate(TRUE);
	}
	else
	{
		m_pTeachParamROIDlg_Tab[m_iPrevInspectTabIndex - MAX_TEACHING_TAB]->UpDate(TRUE);
	}
;

	return 1;
}