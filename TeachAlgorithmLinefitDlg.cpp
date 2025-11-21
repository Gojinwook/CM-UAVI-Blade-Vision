// TeachAlgorithmLinefitDlg.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "uScan.h"
#include "TeachAlgorithmLinefitDlg.h"
#include "afxdialogex.h"


// CTeachAlgorithmLinefitDlg 대화 상자입니다.

IMPLEMENT_DYNAMIC(CTeachAlgorithmLinefitDlg, CDialog)

CTeachAlgorithmLinefitDlg::CTeachAlgorithmLinefitDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CTeachAlgorithmLinefitDlg::IDD, pParent)
{
	m_iSelectedImageType = 0;
	m_iSelectedInspection = 0;
	m_pSelectedROI = NULL;


	m_iEditEdgeMeasureMultiPointNumber = 8;
	m_bCheckEdgeMeasureEndPoint = FALSE;
	m_iEditEdgeMeasureStartLength = 100;
	m_iEditEdgeMeasureEndLength = 500;
	m_iRadioEdgeMeasureDir = 0;
	m_iRadioEdgeMeasureGv = 0;
	m_iRadioEdgeMeasurePos = 0;
	m_dEditEdgeMeasureSmFactor = 1.0;
	m_iEditEdgeMeasureEdgeStr = 10;
}

CTeachAlgorithmLinefitDlg::~CTeachAlgorithmLinefitDlg()
{
}

void CTeachAlgorithmLinefitDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_EDGE_MEASURE_MULTI_POINT_NUMBER, m_iEditEdgeMeasureMultiPointNumber);
	DDX_Check(pDX, IDC_CHECK_EDGE_MEASURE_END_POINT, m_bCheckEdgeMeasureEndPoint);
	DDX_Text(pDX, IDC_EDIT_EDGE_MEASURE_START_LENGTH, m_iEditEdgeMeasureStartLength);
	DDX_Text(pDX, IDC_EDIT_EDGE_MEASURE_END_LENGTH, m_iEditEdgeMeasureEndLength);
	DDX_Radio(pDX, IDC_RADIO_EDGE_MEASURE_DIR_X, m_iRadioEdgeMeasureDir);
	DDX_Radio(pDX, IDC_RADIO_EDGE_MEASURE_GV_ANY, m_iRadioEdgeMeasureGv);
	DDX_Radio(pDX, IDC_RADIO_EDGE_MEASURE_POS_BEST, m_iRadioEdgeMeasurePos);
	DDX_Text(pDX, IDC_EDIT_EDGE_MEASURE_SMOOTHING_FACTOR, m_dEditEdgeMeasureSmFactor);
	DDX_Text(pDX, IDC_EDIT_EDGE_MEASURE_EDGE_STRENGTH, m_iEditEdgeMeasureEdgeStr);
}


BEGIN_MESSAGE_MAP(CTeachAlgorithmLinefitDlg, CDialog)
	ON_BN_CLICKED(IDOK, &CTeachAlgorithmLinefitDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CTeachAlgorithmLinefitDlg::OnBnClickedCancel)
END_MESSAGE_MAP()


// CTeachAlgorithmLinefitDlg 메시지 처리기입니다.

BOOL CTeachAlgorithmLinefitDlg::PreTranslateMessage(MSG* pMsg)
{
	switch(pMsg->message) {		
	case WM_NCLBUTTONDOWN :
		SetActiveWindow();
		return FALSE;

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


BOOL CTeachAlgorithmLinefitDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	CAlgorithmParam AlgorithmParam;

	if (m_pSelectedROI == NULL)
		AlgorithmParam = THEAPP.m_pModelDataManager->m_AlgorithmParam[m_iSelectedImageType][m_iSelectedInspection];
	else
		AlgorithmParam = m_pSelectedROI->m_AlgorithmParam[m_iSelectedInspection];

	m_iEditEdgeMeasureMultiPointNumber = AlgorithmParam.m_iEdgeMeasureMultiPointNumber;
	m_bCheckEdgeMeasureEndPoint = AlgorithmParam.m_bEdgeMeasureEndPoint;
	m_iEditEdgeMeasureStartLength = AlgorithmParam.m_iEdgeMeasureStartLength;
	m_iEditEdgeMeasureEndLength = AlgorithmParam.m_iEdgeMeasureEndLength;
	m_iRadioEdgeMeasureDir = AlgorithmParam.m_iEdgeMeasureDir;
	m_iRadioEdgeMeasureGv = AlgorithmParam.m_iEdgeMeasureGv;
	m_iRadioEdgeMeasurePos = AlgorithmParam.m_iEdgeMeasurePos;
	m_dEditEdgeMeasureSmFactor = AlgorithmParam.m_dEdgeMeasureSmFactor;
	m_iEditEdgeMeasureEdgeStr = AlgorithmParam.m_iEdgeMeasureEdgeStr;

	UpdateData(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

void CTeachAlgorithmLinefitDlg::OnBnClickedOk()
{
	UpdateData();

	CAlgorithmParam AlgorithmParam;

	if (m_pSelectedROI == NULL)
		AlgorithmParam = THEAPP.m_pModelDataManager->m_AlgorithmParam[m_iSelectedImageType][m_iSelectedInspection];
	else
		AlgorithmParam = m_pSelectedROI->m_AlgorithmParam[m_iSelectedInspection];

	AlgorithmParam.m_iEdgeMeasureMultiPointNumber = m_iEditEdgeMeasureMultiPointNumber;
	AlgorithmParam.m_bEdgeMeasureEndPoint = m_bCheckEdgeMeasureEndPoint;
	AlgorithmParam.m_iEdgeMeasureStartLength = m_iEditEdgeMeasureStartLength;
	AlgorithmParam.m_iEdgeMeasureEndLength = m_iEditEdgeMeasureEndLength;
	AlgorithmParam.m_iEdgeMeasureDir = m_iRadioEdgeMeasureDir;
	AlgorithmParam.m_iEdgeMeasureGv = m_iRadioEdgeMeasureGv;
	AlgorithmParam.m_iEdgeMeasurePos = m_iRadioEdgeMeasurePos;
	AlgorithmParam.m_dEdgeMeasureSmFactor = m_dEditEdgeMeasureSmFactor;
	AlgorithmParam.m_iEdgeMeasureEdgeStr = m_iEditEdgeMeasureEdgeStr;


	if (m_pSelectedROI == NULL)
		THEAPP.m_pModelDataManager->m_AlgorithmParam[m_iSelectedImageType][m_iSelectedInspection] = AlgorithmParam;
	else
		 m_pSelectedROI->m_AlgorithmParam[m_iSelectedInspection] = AlgorithmParam;

	CDialog::OnOK();
}


void CTeachAlgorithmLinefitDlg::OnBnClickedCancel()
{
	m_pSelectedROI = NULL;

	CDialog::OnCancel();
}
