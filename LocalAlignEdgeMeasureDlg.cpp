// LocalAlignEdgeMeasureDlg.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "uScan.h"
#include "LocalAlignEdgeMeasureDlg.h"
#include "afxdialogex.h"


// CLocalAlignEdgeMeasureDlg 대화 상자입니다.

IMPLEMENT_DYNAMIC(CLocalAlignEdgeMeasureDlg, CDialog)

CLocalAlignEdgeMeasureDlg::CLocalAlignEdgeMeasureDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CLocalAlignEdgeMeasureDlg::IDD, pParent)
	, m_bCheckUseEdgeMeasure(false)
{
	m_iSelectedImageType = 0;
	m_iInspectionType = 0;
	m_pSelectedROI = NULL;

	m_bCheckUseEdgeMeasure = FALSE;
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

CLocalAlignEdgeMeasureDlg::~CLocalAlignEdgeMeasureDlg()
{
}

void CLocalAlignEdgeMeasureDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Check(pDX, IDC_CHECK_EDGE_MEASURE_LOCAL_ALIGN_USE, m_bCheckUseEdgeMeasure);
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


BEGIN_MESSAGE_MAP(CLocalAlignEdgeMeasureDlg, CDialog)
	ON_BN_CLICKED(IDC_BUTTON_TEST_EDGE_MEASURE, &CLocalAlignEdgeMeasureDlg::OnBnClickedButtonTestEdgeMeasure)
END_MESSAGE_MAP()


// CLocalAlignEdgeMeasureDlg 메시지 처리기입니다.
void CLocalAlignEdgeMeasureDlg::SetSelectedInspection(int iImageType, GTRegion *pSelectedROI, int iInspectionType)
{ 
	m_iSelectedImageType = iImageType;
	m_iInspectionType = iInspectionType;
	m_pSelectedROI = pSelectedROI;

	if (pSelectedROI == NULL || pSelectedROI->miInspectionType != INSPECTION_TYPE_LOCAL_ALIGN)
	{
		GetDlgItem(IDC_CHECK_EDGE_MEASURE_LOCAL_ALIGN_USE)->EnableWindow(FALSE);
		GetDlgItem(IDC_BUTTON_TEST_EDGE_MEASURE)->EnableWindow(FALSE);

	}
	else
	{
		GetDlgItem(IDC_CHECK_EDGE_MEASURE_LOCAL_ALIGN_USE)->EnableWindow(TRUE);
		GetDlgItem(IDC_BUTTON_TEST_EDGE_MEASURE)->EnableWindow(TRUE);
	}

}

void CLocalAlignEdgeMeasureDlg::SetParam(CAlgorithmParam LocalAlignParam)
{
	m_bCheckUseEdgeMeasure = LocalAlignParam.m_bLocalAlignEdgeMeasureUse;
	m_iEditEdgeMeasureMultiPointNumber = LocalAlignParam.m_iLocalAlignEdgeMeasureMultiPointNumber;
	m_bCheckEdgeMeasureEndPoint = LocalAlignParam.m_bLocalAlignEdgeMeasureEndPoint;
	m_iEditEdgeMeasureStartLength = LocalAlignParam.m_iLocalAlignEdgeMeasureStartLength;
	m_iEditEdgeMeasureEndLength = LocalAlignParam.m_iLocalAlignEdgeMeasureEndLength;
	m_iRadioEdgeMeasureDir = LocalAlignParam.m_iLocalAlignEdgeMeasureDir;
	m_iRadioEdgeMeasureGv = LocalAlignParam.m_iLocalAlignEdgeMeasureGv;
	m_iRadioEdgeMeasurePos = LocalAlignParam.m_iLocalAlignEdgeMeasurePos;
	m_dEditEdgeMeasureSmFactor = LocalAlignParam.m_dLocalAlignEdgeMeasureSmFactor;
	m_iEditEdgeMeasureEdgeStr = LocalAlignParam.m_iLocalAlignEdgeMeasureEdgeStr;

	UpdateData(FALSE);
}

void CLocalAlignEdgeMeasureDlg::GetParam(CAlgorithmParam *pLocalAlignParam)
{
	UpdateData();

	pLocalAlignParam->m_bLocalAlignEdgeMeasureUse = m_bCheckUseEdgeMeasure;
	pLocalAlignParam->m_iLocalAlignEdgeMeasureMultiPointNumber = m_iEditEdgeMeasureMultiPointNumber;
	pLocalAlignParam->m_bLocalAlignEdgeMeasureEndPoint = m_bCheckEdgeMeasureEndPoint;
	pLocalAlignParam->m_iLocalAlignEdgeMeasureStartLength = m_iEditEdgeMeasureStartLength;
	pLocalAlignParam->m_iLocalAlignEdgeMeasureEndLength = m_iEditEdgeMeasureEndLength;
	pLocalAlignParam->m_iLocalAlignEdgeMeasureDir = m_iRadioEdgeMeasureDir;
	pLocalAlignParam->m_iLocalAlignEdgeMeasureGv = m_iRadioEdgeMeasureGv;
	pLocalAlignParam->m_iLocalAlignEdgeMeasurePos = m_iRadioEdgeMeasurePos;
	pLocalAlignParam->m_dLocalAlignEdgeMeasureSmFactor = m_dEditEdgeMeasureSmFactor;
	pLocalAlignParam->m_iLocalAlignEdgeMeasureEdgeStr = m_iEditEdgeMeasureEdgeStr;
}

BOOL CLocalAlignEdgeMeasureDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

void CLocalAlignEdgeMeasureDlg::OnBnClickedButtonTestEdgeMeasure()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	//Hobject HEdgeMeasureAlgoDefectRgn;
	//int iEdgeMeasureAlignPosX, iEdgeMeasureAlignPosY;
	//double dEdgeCenterX, dEdgeCenterY;
	//double dEdgeLineStartX, dEdgeLineStartY, dEdgeLineEndX, dEdgeLineEndY;

	//iEdgeMeasureAlignPosX = iEdgeMeasureAlignPosY = INVALID_ALIGN_RESULT;
	//dEdgeLineStartX = dEdgeLineStartY = dEdgeLineEndX = dEdgeLineEndY = INVALID_ALIGN_RESULT;

	//if (m_bCheckUseEdgeMeasure)
	//{
	//	dEdgeCenterX = dEdgeCenterY = -1;

	//	HEdgeMeasureAlgoDefectRgn = THEAPP.m_pAlgorithm->EdgeMeasureAlgorithm(HImage, &(pHInspectImage[AlgorithmParam.m_iEdgeMeasureImageIndexOther]), HROIRgn, AlgorithmParam, pHResultXLD, &iEdgeMeasureAlignPosX, &iEdgeMeasureAlignPosY, &dEdgeCenterX, &dEdgeCenterY, &dEdgeLineStartX, &dEdgeLineStartY, &dEdgeLineEndX, &dEdgeLineEndY);

	//	if (AlgorithmParam.m_iEdgeMeasureOnePoint == 1)		// Line
	//	{
	//		if (dEdgeLineStartX == INVALID_ALIGN_RESULT)	// Fail
	//		{
	//			pMeasureData->m_dEdgeLineStartX = -1;
	//			pMeasureData->m_dEdgeLineStartY = -1;
	//			pMeasureData->m_dEdgeLineEndX = -1;
	//			pMeasureData->m_dEdgeLineEndY = -1;
	//		}
	//		else
	//		{
	//			pMeasureData->m_dEdgeLineStartX = dEdgeLineStartX;
	//			pMeasureData->m_dEdgeLineStartY = dEdgeLineStartY;
	//			pMeasureData->m_dEdgeLineEndX = dEdgeLineEndX;
	//			pMeasureData->m_dEdgeLineEndY = dEdgeLineEndY;
	//		}
	//	}
	//	else   // Point
	//	{
	//		if (dEdgeCenterX == -1)	// Fail
	//		{
	//			pMeasureData->m_dEdgeLineStartX = -1;
	//			pMeasureData->m_dEdgeLineStartY = -1;
	//			pMeasureData->m_dEdgeLineEndX = -1;
	//			pMeasureData->m_dEdgeLineEndY = -1;
	//		}
	//		else
	//		{
	//			if (AlgorithmParam.m_iEdgeMeasureDir == 1)		// Y Dir
	//			{
	//				pMeasureData->m_dEdgeLineStartX = dEdgeCenterX - ONE_POINT_EDGE_EXTEND_PXL;
	//				pMeasureData->m_dEdgeLineStartY = dEdgeCenterY;
	//				pMeasureData->m_dEdgeLineEndX = dEdgeCenterX + ONE_POINT_EDGE_EXTEND_PXL;
	//				pMeasureData->m_dEdgeLineEndY = dEdgeCenterY;
	//			}
	//			else
	//			{
	//				pMeasureData->m_dEdgeLineStartX = dEdgeCenterX;
	//				pMeasureData->m_dEdgeLineStartY = dEdgeCenterY - ONE_POINT_EDGE_EXTEND_PXL;
	//				pMeasureData->m_dEdgeLineEndX = dEdgeCenterX;
	//				pMeasureData->m_dEdgeLineEndY = dEdgeCenterY + ONE_POINT_EDGE_EXTEND_PXL;
	//			}
	//		}
	//	}

	//	if (THEAPP.m_pGFunction->ValidHRegion(HEdgeMeasureAlgoDefectRgn) == TRUE)
	//		ConcatObj(HDefectAllRgn, HEdgeMeasureAlgoDefectRgn, &HDefectAllRgn);
	//}

}
