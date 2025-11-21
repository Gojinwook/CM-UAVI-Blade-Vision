// ROIEdgeMeasureDlg.cpp : 구현 파일입니다.

#include "stdafx.h"
#include "uScan.h"
#include "ROIEdgeMeasureDlg.h"
#include "afxdialogex.h"


// CROIEdgeMeasureDlg 대화 상자입니다.

IMPLEMENT_DYNAMIC(CROIEdgeMeasureDlg, CDialog)

CROIEdgeMeasureDlg::CROIEdgeMeasureDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CROIEdgeMeasureDlg::IDD, pParent)
{
	m_iSelectedImageType = 0;
	m_iInspectionType = 0;
	m_pSelectedROI = NULL;


	m_bCheckEdgeMeasureUse = FALSE;
	m_bCheckEdgeMeasureLocalAlignUse = FALSE;
	m_iEditEdgeMeasureMultiPointNumber = 8;
	m_bCheckEdgeMeasureEndPoint = FALSE;
	m_iEditEdgeMeasureStartLength = 100;
	m_iEditEdgeMeasureEndLength = 500;
	m_iRadioEdgeMeasureDir = 0;
	m_iRadioEdgeMeasureGv = 0;
	m_iRadioEdgeMeasurePos = 0;
	m_dEditEdgeMeasureSmFactor = 1.0;
	m_iEditEdgeMeasureEdgeStr = 10;
	m_dEditEdgeMeasureRotateAngle = 0.0;
}

CROIEdgeMeasureDlg::~CROIEdgeMeasureDlg()
{
}

void CROIEdgeMeasureDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Check(pDX, IDC_CHECK_EDGE_MEASURE_USE, m_bCheckEdgeMeasureUse);
	DDX_Check(pDX, IDC_CHECK_EDGE_MEASURE_LOCAL_ALIGN_USE, m_bCheckEdgeMeasureLocalAlignUse);
	DDX_Text(pDX, IDC_EDIT_EDGE_MEASURE_MULTI_POINT_NUMBER, m_iEditEdgeMeasureMultiPointNumber);
	DDX_Check(pDX, IDC_CHECK_EDGE_MEASURE_END_POINT, m_bCheckEdgeMeasureEndPoint);
	DDX_Text(pDX, IDC_EDIT_EDGE_MEASURE_START_LENGTH, m_iEditEdgeMeasureStartLength);
	DDX_Text(pDX, IDC_EDIT_EDGE_MEASURE_END_LENGTH, m_iEditEdgeMeasureEndLength);
	DDX_Radio(pDX, IDC_RADIO_EDGE_MEASURE_DIR_X, m_iRadioEdgeMeasureDir);
	DDX_Radio(pDX, IDC_RADIO_EDGE_MEASURE_GV_ANY, m_iRadioEdgeMeasureGv);
	DDX_Radio(pDX, IDC_RADIO_EDGE_MEASURE_POS_BEST, m_iRadioEdgeMeasurePos);
	DDX_Text(pDX, IDC_EDIT_EDGE_MEASURE_SMOOTHING_FACTOR, m_dEditEdgeMeasureSmFactor);
	DDX_Text(pDX, IDC_EDIT_EDGE_MEASURE_EDGE_STRENGTH, m_iEditEdgeMeasureEdgeStr);
	DDX_Text(pDX, IDC_EDIT_ROI_ROTATE_ANGLE, m_dEditEdgeMeasureRotateAngle);


}


BEGIN_MESSAGE_MAP(CROIEdgeMeasureDlg, CDialog)
	ON_BN_CLICKED(IDC_BUTTON_TEST_EDGE_MEASURE, &CROIEdgeMeasureDlg::OnBnClickedButtonTestEdgeMeasure)
	ON_BN_CLICKED(IDC_BUTTON_ROI_AFFINE_TRANS, &CROIEdgeMeasureDlg::OnBnClickedButtonRoiAffineTrans)
END_MESSAGE_MAP()


// CROIEdgeMeasureDlg 메시지 처리기입니다.
void CROIEdgeMeasureDlg::SetSelectedInspection(int iImageType, GTRegion *pSelectedROI, int iInspectionType)
{ 
	m_iSelectedImageType = iImageType;
	m_iInspectionType = iInspectionType;
	m_pSelectedROI = pSelectedROI;

	GetDlgItem(IDC_CHECK_EDGE_MEASURE_LOCAL_ALIGN_USE)->EnableWindow(TRUE);
	GetDlgItem(IDC_BUTTON_TEST_EDGE_MEASURE)->EnableWindow(TRUE);

}

void CROIEdgeMeasureDlg::SetParam(CAlgorithmParam ROIAlgorithmParam)
{
	m_bCheckEdgeMeasureUse = ROIAlgorithmParam.m_bROIEdgeMeasureUse;
	m_bCheckEdgeMeasureLocalAlignUse = ROIAlgorithmParam.m_bROIEdgeMeasureLocalAlignUse;
	m_iEditEdgeMeasureMultiPointNumber = ROIAlgorithmParam.m_iROIEdgeMeasureMultiPointNumber;
	m_bCheckEdgeMeasureEndPoint = ROIAlgorithmParam.m_bROIEdgeMeasureEndPoint;
	m_iEditEdgeMeasureStartLength = ROIAlgorithmParam.m_iROIEdgeMeasureStartLength;
	m_iEditEdgeMeasureEndLength = ROIAlgorithmParam.m_iROIEdgeMeasureEndLength;
	m_iRadioEdgeMeasureDir = ROIAlgorithmParam.m_iROIEdgeMeasureDir;
	m_iRadioEdgeMeasureGv = ROIAlgorithmParam.m_iROIEdgeMeasureGv;
	m_iRadioEdgeMeasurePos = ROIAlgorithmParam.m_iROIEdgeMeasurePos;
	m_dEditEdgeMeasureSmFactor = ROIAlgorithmParam.m_dROIEdgeMeasureSmFactor;
	m_iEditEdgeMeasureEdgeStr = ROIAlgorithmParam.m_iROIEdgeMeasureEdgeStr;
	m_dEditEdgeMeasureRotateAngle = ROIAlgorithmParam.m_dROIEdgeMeasureRotateAngle;

	UpdateData(FALSE);
}

void CROIEdgeMeasureDlg::GetParam(CAlgorithmParam *pROIAlgorithmParam)
{
	UpdateData();

	pROIAlgorithmParam->m_bROIEdgeMeasureUse = m_bCheckEdgeMeasureUse;
	pROIAlgorithmParam->m_bROIEdgeMeasureLocalAlignUse = m_bCheckEdgeMeasureLocalAlignUse;
	pROIAlgorithmParam->m_iROIEdgeMeasureMultiPointNumber = m_iEditEdgeMeasureMultiPointNumber;
	pROIAlgorithmParam->m_bROIEdgeMeasureEndPoint = m_bCheckEdgeMeasureEndPoint;
	pROIAlgorithmParam->m_iROIEdgeMeasureStartLength = m_iEditEdgeMeasureStartLength;
	pROIAlgorithmParam->m_iROIEdgeMeasureEndLength = m_iEditEdgeMeasureEndLength;
	pROIAlgorithmParam->m_iROIEdgeMeasureDir = m_iRadioEdgeMeasureDir;
	pROIAlgorithmParam->m_iROIEdgeMeasureGv = m_iRadioEdgeMeasureGv;
	pROIAlgorithmParam->m_iROIEdgeMeasurePos = m_iRadioEdgeMeasurePos;
	pROIAlgorithmParam->m_dROIEdgeMeasureSmFactor = m_dEditEdgeMeasureSmFactor;
	pROIAlgorithmParam->m_iROIEdgeMeasureEdgeStr = m_iEditEdgeMeasureEdgeStr;
}

BOOL CROIEdgeMeasureDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

void CROIEdgeMeasureDlg::OnBnClickedButtonTestEdgeMeasure()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	
	UpdateData();

	int iEdgeMeasureAlignPosX, iEdgeMeasureAlignPosY;
	double dEdgeCenterX, dEdgeCenterY;
	double dEdgeLineStartX, dEdgeLineStartY, dEdgeLineEndX, dEdgeLineEndY;

	iEdgeMeasureAlignPosX = iEdgeMeasureAlignPosY = INVALID_ALIGN_RESULT;
	dEdgeLineStartX = dEdgeLineStartY = dEdgeLineEndX = dEdgeLineEndY = INVALID_ALIGN_RESULT;

	Hobject HInspImage = THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[m_iSelectedImageType];
	Hobject HInspectAreaRgn = m_pSelectedROI->GetROIHRegion(THEAPP.m_pCalDataService);
	Hobject HLineFitXLD;
	gen_empty_obj(&HLineFitXLD);

	if (m_bCheckEdgeMeasureUse)
	{
		try
		{
			dEdgeCenterX = dEdgeCenterY = -1;

			BYTE* pImageData;
			char type[30];
			Hlong lImageWidth, lImageHeight;

			get_image_pointer1(HInspImage, (Hlong*)&pImageData, type, &lImageWidth, &lImageHeight);

			if (m_iEditEdgeMeasureMultiPointNumber <= 0)
				m_iEditEdgeMeasureMultiPointNumber = 2;

			if (m_bCheckEdgeMeasureEndPoint)
				m_iEditEdgeMeasureMultiPointNumber = 2;

			// Edge Measure

			double dEdgeMeasureCenterX, dEdgeMeasureCenterY;
			int iROISizeX, iROISizeY;
			
			Hlong lEdgePosY, lEdgePosX;
			Hlong lROIRow1, lROIRow2, lROICol1, lROICol2;
			HTuple HlROIRow1, HlROIRow2, HlROICol1, HlROICol2;
			HTuple HROIRow, HROICol;
			tuple_gen_const(0, 0, &HROIRow);
			tuple_gen_const(0, 0, &HROICol);

			HTuple  MeasureHandle;
			HTuple RowEdge, ColumnEdge, Amplitude, Distance, Indices, Inverted;;
			Hlong NoEdge, MaxAmpEdgeIndex;

			double dEgePosX, dEgePosY;
			int iEdgePosX, iEdgePosY;
			Hobject RegressContour;

			double dMeasureAngleRad = 0;
			HTuple HAngleRad;
			tuple_rad(m_dEditEdgeMeasureRotateAngle, &HAngleRad);
			dMeasureAngleRad = HAngleRad[0].D();

			HTuple HEdgePosX, HEdgePosY;
			tuple_gen_const(0, 0, &HEdgePosX);
			tuple_gen_const(0, 0, &HEdgePosY);

			smallest_rectangle1(HInspectAreaRgn, &HlROIRow1, &HlROICol1, &HlROIRow2, &HlROICol2);

			lROIRow1 = HlROIRow1[0].L();
			lROICol1 = HlROICol1[0].L();
			lROIRow2 = HlROIRow2[0].L();
			lROICol2 = HlROICol2[0].L();

			if (m_iEditEdgeMeasureMultiPointNumber == 1)
			{
				dEdgeMeasureCenterX = (double)(lROICol1 + lROICol2) * 0.5;
				dEdgeMeasureCenterY = (double)(lROIRow1 + lROIRow2) * 0.5;

				iROISizeX = lROICol2 - lROICol1;
				iROISizeY = lROIRow2 - lROIRow1;

				if (m_iRadioEdgeMeasureDir == 1)		// Y Dir
				{
					gen_measure_rectangle2(dEdgeMeasureCenterY, dEdgeMeasureCenterX, 3.14 / 2 + dMeasureAngleRad, iROISizeY / 2, iROISizeX / 2, lImageWidth,
						lImageHeight, "nearest_neighbor", &MeasureHandle);

					if (m_iRadioEdgeMeasurePos == 0)
					{
						if (m_iRadioEdgeMeasureGv == 0)
							measure_pos(HInspImage, MeasureHandle, m_dEditEdgeMeasureSmFactor, (double)m_iEditEdgeMeasureEdgeStr, "all",
								"all", &RowEdge, &ColumnEdge, &Amplitude, &Distance);
						else if (m_iRadioEdgeMeasureGv == 1)
							measure_pos(HInspImage, MeasureHandle, m_dEditEdgeMeasureSmFactor, (double)m_iEditEdgeMeasureEdgeStr, "negative",
								"all", &RowEdge, &ColumnEdge, &Amplitude, &Distance);
						else if (m_iRadioEdgeMeasureGv == 2)
							measure_pos(HInspImage, MeasureHandle, m_dEditEdgeMeasureSmFactor, (double)m_iEditEdgeMeasureEdgeStr, "positive",
								"all", &RowEdge, &ColumnEdge, &Amplitude, &Distance);
					}
					else if (m_iRadioEdgeMeasurePos == 1)
					{
						if (m_iRadioEdgeMeasureGv == 0)
							measure_pos(HInspImage, MeasureHandle, m_dEditEdgeMeasureSmFactor, (double)m_iEditEdgeMeasureEdgeStr, "all",
								"first", &RowEdge, &ColumnEdge, &Amplitude, &Distance);
						else if (m_iRadioEdgeMeasureGv == 1)
							measure_pos(HInspImage, MeasureHandle, m_dEditEdgeMeasureSmFactor, (double)m_iEditEdgeMeasureEdgeStr, "negative",
								"first", &RowEdge, &ColumnEdge, &Amplitude, &Distance);
						else if (m_iRadioEdgeMeasureGv == 2)
							measure_pos(HInspImage, MeasureHandle, m_dEditEdgeMeasureSmFactor, (double)m_iEditEdgeMeasureEdgeStr, "positive",
								"first", &RowEdge, &ColumnEdge, &Amplitude, &Distance);
					}
					else if (m_iRadioEdgeMeasurePos == 2)
					{
						if (m_iRadioEdgeMeasureGv == 0)
							measure_pos(HInspImage, MeasureHandle, m_dEditEdgeMeasureSmFactor, (double)m_iEditEdgeMeasureEdgeStr, "all",
								"last", &RowEdge, &ColumnEdge, &Amplitude, &Distance);
						else if (m_iRadioEdgeMeasureGv == 1)
							measure_pos(HInspImage, MeasureHandle, m_dEditEdgeMeasureSmFactor, (double)m_iEditEdgeMeasureEdgeStr, "negative",
								"last", &RowEdge, &ColumnEdge, &Amplitude, &Distance);
						else if (m_iRadioEdgeMeasureGv == 2)
							measure_pos(HInspImage, MeasureHandle, m_dEditEdgeMeasureSmFactor, (double)m_iEditEdgeMeasureEdgeStr, "positive",
								"last", &RowEdge, &ColumnEdge, &Amplitude, &Distance);
					}
				}
				else		// X Dir
				{
					gen_measure_rectangle2(dEdgeMeasureCenterY, dEdgeMeasureCenterX, 3.14 + dMeasureAngleRad, iROISizeX / 2, iROISizeY / 2, lImageWidth,
						lImageHeight, "nearest_neighbor", &MeasureHandle);

					if (m_iRadioEdgeMeasurePos == 0)
					{
						if (m_iRadioEdgeMeasureGv == 0)
							measure_pos(HInspImage, MeasureHandle, m_dEditEdgeMeasureSmFactor, (double)m_iEditEdgeMeasureEdgeStr, "all",
								"all", &RowEdge, &ColumnEdge, &Amplitude, &Distance);
						else if (m_iRadioEdgeMeasureGv == 1)
							measure_pos(HInspImage, MeasureHandle, m_dEditEdgeMeasureSmFactor, (double)m_iEditEdgeMeasureEdgeStr, "negative",
								"all", &RowEdge, &ColumnEdge, &Amplitude, &Distance);
						else if (m_iRadioEdgeMeasureGv == 2)
							measure_pos(HInspImage, MeasureHandle, m_dEditEdgeMeasureSmFactor, (double)m_iEditEdgeMeasureEdgeStr, "positive",
								"all", &RowEdge, &ColumnEdge, &Amplitude, &Distance);
					}
					else if (m_iRadioEdgeMeasurePos == 1)
					{
						if (m_iRadioEdgeMeasureGv == 0)
							measure_pos(HInspImage, MeasureHandle, m_dEditEdgeMeasureSmFactor, (double)m_iEditEdgeMeasureEdgeStr, "all",
								"first", &RowEdge, &ColumnEdge, &Amplitude, &Distance);
						else if (m_iRadioEdgeMeasureGv == 1)
							measure_pos(HInspImage, MeasureHandle, m_dEditEdgeMeasureSmFactor, (double)m_iEditEdgeMeasureEdgeStr, "negative",
								"first", &RowEdge, &ColumnEdge, &Amplitude, &Distance);
						else if (m_iRadioEdgeMeasureGv == 2)
							measure_pos(HInspImage, MeasureHandle, m_dEditEdgeMeasureSmFactor, (double)m_iEditEdgeMeasureEdgeStr, "positive",
								"first", &RowEdge, &ColumnEdge, &Amplitude, &Distance);
					}
					else if (m_iRadioEdgeMeasurePos == 2)
					{
						if (m_iRadioEdgeMeasureGv == 0)
							measure_pos(HInspImage, MeasureHandle, m_dEditEdgeMeasureSmFactor, (double)m_iEditEdgeMeasureEdgeStr, "all",
								"last", &RowEdge, &ColumnEdge, &Amplitude, &Distance);
						else if (m_iRadioEdgeMeasureGv == 1)
							measure_pos(HInspImage, MeasureHandle, m_dEditEdgeMeasureSmFactor, (double)m_iEditEdgeMeasureEdgeStr, "negative",
								"last", &RowEdge, &ColumnEdge, &Amplitude, &Distance);
						else if (m_iRadioEdgeMeasureGv == 2)
							measure_pos(HInspImage, MeasureHandle, m_dEditEdgeMeasureSmFactor, (double)m_iEditEdgeMeasureEdgeStr, "positive",
								"last", &RowEdge, &ColumnEdge, &Amplitude, &Distance);
					}
				}

				close_measure(MeasureHandle);

				tuple_length(Amplitude, &NoEdge);

				if (NoEdge <= 0)
				{
					m_pSelectedROI->m_dEdgeCenterX[MAX_MODULE_ONE_TRAY-1] = INVALID_ALIGN_RESULT;
					m_pSelectedROI->m_dEdgeCenterY[MAX_MODULE_ONE_TRAY - 1] = INVALID_ALIGN_RESULT;
					m_pSelectedROI->m_dLineStartX[MAX_MODULE_ONE_TRAY - 1] = INVALID_ALIGN_RESULT;
					m_pSelectedROI->m_dLineStartY[MAX_MODULE_ONE_TRAY - 1] = INVALID_ALIGN_RESULT;
					m_pSelectedROI->m_dLineEndX[MAX_MODULE_ONE_TRAY - 1] = INVALID_ALIGN_RESULT;
					m_pSelectedROI->m_dLineEndY[MAX_MODULE_ONE_TRAY - 1] = INVALID_ALIGN_RESULT;

					return;
				}
				else
				{
					tuple_abs(Amplitude, &Amplitude);
					tuple_sort_index(Amplitude, &Indices);
					tuple_inverse(Indices, &Inverted);
					MaxAmpEdgeIndex = Inverted[0].L();

					if (m_iRadioEdgeMeasureDir == 1)		// Y Dir
					{
						dEgePosY = RowEdge[MaxAmpEdgeIndex].D();
						lEdgePosY = (int)dEgePosY;

						dEgePosX = ColumnEdge[MaxAmpEdgeIndex].D();
						lEdgePosX = (int)dEgePosX;

						m_pSelectedROI->m_dEdgeCenterX[MAX_MODULE_ONE_TRAY - 1] = dEgePosX;
						m_pSelectedROI->m_dEdgeCenterY[MAX_MODULE_ONE_TRAY - 1] = dEgePosY;
						m_pSelectedROI->m_dLineStartX[MAX_MODULE_ONE_TRAY - 1] = m_pSelectedROI->m_dEdgeCenterX[MAX_MODULE_ONE_TRAY - 1] - ONE_POINT_EDGE_EXTEND_PXL;
						m_pSelectedROI->m_dLineStartY[MAX_MODULE_ONE_TRAY - 1] = m_pSelectedROI->m_dEdgeCenterY[MAX_MODULE_ONE_TRAY - 1];
						m_pSelectedROI->m_dLineEndX[MAX_MODULE_ONE_TRAY - 1] = m_pSelectedROI->m_dEdgeCenterX[MAX_MODULE_ONE_TRAY - 1] + ONE_POINT_EDGE_EXTEND_PXL;
						m_pSelectedROI->m_dLineEndY[MAX_MODULE_ONE_TRAY - 1] = m_pSelectedROI->m_dEdgeCenterY[MAX_MODULE_ONE_TRAY - 1];

						gen_cross_contour_xld(&RegressContour, m_pSelectedROI->m_dEdgeCenterY[MAX_MODULE_ONE_TRAY - 1], m_pSelectedROI->m_dEdgeCenterX[MAX_MODULE_ONE_TRAY - 1], 30.0, 0);
					}
					else
					{

						dEgePosY = RowEdge[MaxAmpEdgeIndex].D();
						lEdgePosY = (int)dEgePosY;

						dEgePosX = ColumnEdge[MaxAmpEdgeIndex].D();
						lEdgePosX = (int)dEgePosX;

						m_pSelectedROI->m_dEdgeCenterX[MAX_MODULE_ONE_TRAY - 1] = dEgePosX;
						m_pSelectedROI->m_dEdgeCenterY[MAX_MODULE_ONE_TRAY - 1] = dEgePosY;
						m_pSelectedROI->m_dLineStartX[MAX_MODULE_ONE_TRAY - 1] = m_pSelectedROI->m_dEdgeCenterX[MAX_MODULE_ONE_TRAY - 1];
						m_pSelectedROI->m_dLineStartY[MAX_MODULE_ONE_TRAY - 1] = m_pSelectedROI->m_dEdgeCenterY[MAX_MODULE_ONE_TRAY - 1] - ONE_POINT_EDGE_EXTEND_PXL;
						m_pSelectedROI->m_dLineEndX[MAX_MODULE_ONE_TRAY - 1] = m_pSelectedROI->m_dEdgeCenterX[MAX_MODULE_ONE_TRAY - 1];
						m_pSelectedROI->m_dLineEndY[MAX_MODULE_ONE_TRAY - 1] = m_pSelectedROI->m_dEdgeCenterY[MAX_MODULE_ONE_TRAY - 1] + ONE_POINT_EDGE_EXTEND_PXL;

						gen_cross_contour_xld(&RegressContour, m_pSelectedROI->m_dEdgeCenterY[MAX_MODULE_ONE_TRAY - 1], m_pSelectedROI->m_dEdgeCenterX[MAX_MODULE_ONE_TRAY - 1], 30.0, 0);
					}

					concat_obj(HLineFitXLD, RegressContour, &HLineFitXLD);

				}
	
			}
			else
			{
				int iPitch;

				if (m_iRadioEdgeMeasureDir == 1)		// Y Dir
					iPitch = (lROICol2 - lROICol1) / m_iEditEdgeMeasureMultiPointNumber;
				else
					iPitch = (lROIRow2 - lROIRow1) / m_iEditEdgeMeasureMultiPointNumber;

				int iSubLTPointX, iSubLTPointY, iSubRBPointX, iSubRBPointY;
				int i;

				for (i = 0; i < m_iEditEdgeMeasureMultiPointNumber; i++)
				{
					if (m_bCheckEdgeMeasureEndPoint)
					{
						if (i == 0)	// Start Position
						{
							if (m_iRadioEdgeMeasureDir == 1)		// Y Dir
							{
								iSubLTPointX = lROICol1;
								iSubRBPointX = lROICol1 + m_iEditEdgeMeasureStartLength;
								iSubLTPointY = lROIRow1;
								iSubRBPointY = lROIRow2;
							}
							else
							{
								iSubLTPointX = lROICol1;
								iSubRBPointX = lROICol2;
								iSubLTPointY = lROIRow1;
								iSubRBPointY = lROIRow1 + m_iEditEdgeMeasureStartLength;
							}
						}			// End Position
						else
						{
							if (m_iRadioEdgeMeasureDir == 1)		// Y Dir
							{
								iSubLTPointX = lROICol2 - 1 - m_iEditEdgeMeasureEndLength;
								iSubRBPointX = lROICol2 - 1;
								iSubLTPointY = lROIRow1;
								iSubRBPointY = lROIRow2;
							}
							else
							{
								iSubLTPointX = lROICol1;
								iSubRBPointX = lROICol2;
								iSubLTPointY = lROIRow2 - 1 - m_iEditEdgeMeasureEndLength;
								iSubRBPointY = lROIRow2 - 1;
							}
						}
					}
					else
					{
						if (m_iRadioEdgeMeasureDir == 1)		// Y Dir
						{
							iSubLTPointX = lROICol1 + i * iPitch;
							iSubRBPointX = lROICol1 + (i + 1) * iPitch;
							iSubLTPointY = lROIRow1;
							iSubRBPointY = lROIRow2;
						}
						else
						{
							iSubLTPointX = lROICol1;
							iSubRBPointX = lROICol2;
							iSubLTPointY = lROIRow1 + i * iPitch;
							iSubRBPointY = lROIRow1 + (i + 1) * iPitch;
						}
					}

					dEdgeMeasureCenterX = (double)(iSubLTPointX + iSubRBPointX) * 0.5;
					dEdgeMeasureCenterY = (double)(iSubLTPointY + iSubRBPointY) * 0.5;

					iROISizeX = iSubRBPointX - iSubLTPointX;
					iROISizeY = iSubRBPointY - iSubLTPointY;

					if (m_iRadioEdgeMeasureDir == 1)		// Y Dir
					{
						gen_measure_rectangle2(dEdgeMeasureCenterY, dEdgeMeasureCenterX, 3.14 / 2 + dMeasureAngleRad, iROISizeY / 2, iROISizeX / 2, lImageWidth,
							lImageHeight, "nearest_neighbor", &MeasureHandle);

						if (m_iRadioEdgeMeasurePos == 0)
						{
							if (m_iRadioEdgeMeasureGv == 0)
								measure_pos(HInspImage, MeasureHandle, m_dEditEdgeMeasureSmFactor, (double)m_iEditEdgeMeasureEdgeStr, "all",
									"all", &RowEdge, &ColumnEdge, &Amplitude, &Distance);
							else if (m_iRadioEdgeMeasureGv == 1)
								measure_pos(HInspImage, MeasureHandle, m_dEditEdgeMeasureSmFactor, (double)m_iEditEdgeMeasureEdgeStr, "negative",
									"all", &RowEdge, &ColumnEdge, &Amplitude, &Distance);
							else if (m_iRadioEdgeMeasureGv == 2)
								measure_pos(HInspImage, MeasureHandle, m_dEditEdgeMeasureSmFactor, (double)m_iEditEdgeMeasureEdgeStr, "positive",
									"all", &RowEdge, &ColumnEdge, &Amplitude, &Distance);
						}
						else if (m_iRadioEdgeMeasurePos == 1)
						{
							if (m_iRadioEdgeMeasureGv == 0)
								measure_pos(HInspImage, MeasureHandle, m_dEditEdgeMeasureSmFactor, (double)m_iEditEdgeMeasureEdgeStr, "all",
									"first", &RowEdge, &ColumnEdge, &Amplitude, &Distance);
							else if (m_iRadioEdgeMeasureGv == 1)
								measure_pos(HInspImage, MeasureHandle, m_dEditEdgeMeasureSmFactor, (double)m_iEditEdgeMeasureEdgeStr, "negative",
									"first", &RowEdge, &ColumnEdge, &Amplitude, &Distance);
							else if (m_iRadioEdgeMeasureGv == 2)
								measure_pos(HInspImage, MeasureHandle, m_dEditEdgeMeasureSmFactor, (double)m_iEditEdgeMeasureEdgeStr, "positive",
									"first", &RowEdge, &ColumnEdge, &Amplitude, &Distance);
						}
						else if (m_iRadioEdgeMeasurePos == 2)
						{
							if (m_iRadioEdgeMeasureGv == 0)
								measure_pos(HInspImage, MeasureHandle, m_dEditEdgeMeasureSmFactor, (double)m_iEditEdgeMeasureEdgeStr, "all",
									"last", &RowEdge, &ColumnEdge, &Amplitude, &Distance);
							else if (m_iRadioEdgeMeasureGv == 1)
								measure_pos(HInspImage, MeasureHandle, m_dEditEdgeMeasureSmFactor, (double)m_iEditEdgeMeasureEdgeStr, "negative",
									"last", &RowEdge, &ColumnEdge, &Amplitude, &Distance);
							else if (m_iRadioEdgeMeasureGv == 2)
								measure_pos(HInspImage, MeasureHandle, m_dEditEdgeMeasureSmFactor, (double)m_iEditEdgeMeasureEdgeStr, "positive",
									"last", &RowEdge, &ColumnEdge, &Amplitude, &Distance);
						}
					}
					else		// X Dir
					{
						gen_measure_rectangle2(dEdgeMeasureCenterY, dEdgeMeasureCenterX, 3.14 + dMeasureAngleRad, iROISizeX / 2, iROISizeY / 2, lImageWidth,
							lImageHeight, "nearest_neighbor", &MeasureHandle);

						if (m_iRadioEdgeMeasurePos == 0)
						{
							if (m_iRadioEdgeMeasureGv == 0)
								measure_pos(HInspImage, MeasureHandle, m_dEditEdgeMeasureSmFactor, (double)m_iEditEdgeMeasureEdgeStr, "all",
									"all", &RowEdge, &ColumnEdge, &Amplitude, &Distance);
							else if (m_iRadioEdgeMeasureGv == 1)
								measure_pos(HInspImage, MeasureHandle, m_dEditEdgeMeasureSmFactor, (double)m_iEditEdgeMeasureEdgeStr, "negative",
									"all", &RowEdge, &ColumnEdge, &Amplitude, &Distance);
							else if (m_iRadioEdgeMeasureGv == 2)
								measure_pos(HInspImage, MeasureHandle, m_dEditEdgeMeasureSmFactor, (double)m_iEditEdgeMeasureEdgeStr, "positive",
									"all", &RowEdge, &ColumnEdge, &Amplitude, &Distance);
						}
						else if (m_iRadioEdgeMeasurePos == 1)
						{
							if (m_iRadioEdgeMeasureGv == 0)
								measure_pos(HInspImage, MeasureHandle, m_dEditEdgeMeasureSmFactor, (double)m_iEditEdgeMeasureEdgeStr, "all",
									"first", &RowEdge, &ColumnEdge, &Amplitude, &Distance);
							else if (m_iRadioEdgeMeasureGv == 1)
								measure_pos(HInspImage, MeasureHandle, m_dEditEdgeMeasureSmFactor, (double)m_iEditEdgeMeasureEdgeStr, "negative",
									"first", &RowEdge, &ColumnEdge, &Amplitude, &Distance);
							else if (m_iRadioEdgeMeasureGv == 2)
								measure_pos(HInspImage, MeasureHandle, m_dEditEdgeMeasureSmFactor, (double)m_iEditEdgeMeasureEdgeStr, "positive",
									"first", &RowEdge, &ColumnEdge, &Amplitude, &Distance);
						}
						else if (m_iRadioEdgeMeasurePos == 2)
						{
							if (m_iRadioEdgeMeasureGv == 0)
								measure_pos(HInspImage, MeasureHandle, m_dEditEdgeMeasureSmFactor, (double)m_iEditEdgeMeasureEdgeStr, "all",
									"last", &RowEdge, &ColumnEdge, &Amplitude, &Distance);
							else if (m_iRadioEdgeMeasureGv == 1)
								measure_pos(HInspImage, MeasureHandle, m_dEditEdgeMeasureSmFactor, (double)m_iEditEdgeMeasureEdgeStr, "negative",
									"last", &RowEdge, &ColumnEdge, &Amplitude, &Distance);
							else if (m_iRadioEdgeMeasureGv == 2)
								measure_pos(HInspImage, MeasureHandle, m_dEditEdgeMeasureSmFactor, (double)m_iEditEdgeMeasureEdgeStr, "positive",
									"last", &RowEdge, &ColumnEdge, &Amplitude, &Distance);
						}
					}

					close_measure(MeasureHandle);

					tuple_length(Amplitude, &NoEdge);

					if (NoEdge <= 0)
					{
						continue;
					}
					else
					{
						tuple_abs(Amplitude, &Amplitude);
						tuple_sort_index(Amplitude, &Indices);
						tuple_inverse(Indices, &Inverted);
						MaxAmpEdgeIndex = Inverted[0].L();

						if (m_iRadioEdgeMeasureDir == 1)		// Y Dir
						{
							dEgePosY = RowEdge[MaxAmpEdgeIndex].D();
							dEgePosX = (double)(iSubLTPointX + iSubRBPointX) * 0.5;
						}
						else
						{
							dEgePosX = ColumnEdge[MaxAmpEdgeIndex].D();
							dEgePosY = (double)(iSubLTPointY + iSubRBPointY) * 0.5;
						}

						tuple_concat(HEdgePosX, dEgePosX, &HEdgePosX);
						tuple_concat(HEdgePosY, dEgePosY, &HEdgePosY);

					}

				} // for (i = 0; i < Param.m_iEdgeMeasureMultiPointNumber; i++)

				HTuple HlNoFoundNumber;
				Hlong lNoFoundNumber;
				tuple_length(HEdgePosY, &HlNoFoundNumber);
				lNoFoundNumber = HlNoFoundNumber[0].L();

				Hobject HEdgePolygonContour;

				if (lNoFoundNumber >= 2)
				{
					HTuple RowBegin, ColBegin, RowEnd, ColEnd, Nr, Nc, Dist;

					if (m_bCheckEdgeMeasureEndPoint)
					{
						if (m_pSelectedROI->m_dLineStartX != NULL)
						{
							m_pSelectedROI->m_dLineStartX[MAX_MODULE_ONE_TRAY - 1] = HEdgePosX[0].D();
							m_pSelectedROI->m_dLineStartY[MAX_MODULE_ONE_TRAY - 1] = HEdgePosY[0].D();
							m_pSelectedROI->m_dLineEndX[MAX_MODULE_ONE_TRAY - 1] = HEdgePosX[lNoFoundNumber - 1].D();
							m_pSelectedROI->m_dLineEndY[MAX_MODULE_ONE_TRAY - 1] = HEdgePosY[lNoFoundNumber - 1].D();
						}

						tuple_concat(HEdgePosY[0], HEdgePosY[lNoFoundNumber - 1], &HEdgePosY);
						tuple_concat(HEdgePosX[0], HEdgePosX[lNoFoundNumber - 1], &HEdgePosX);

						gen_contour_polygon_xld(&RegressContour, HEdgePosY, HEdgePosX);

						concat_obj(HLineFitXLD, RegressContour, &HLineFitXLD);
					}
					else
					{
						gen_contour_polygon_xld(&HEdgePolygonContour, HEdgePosY, HEdgePosX);
						fit_line_contour_xld(HEdgePolygonContour, "regression", -1, 0, 5, 2, &RowBegin, &ColBegin, &RowEnd, &ColEnd, &Nr, &Nc, &Dist);

						tuple_length(RowBegin, &HlNoFoundNumber);
						lNoFoundNumber = HlNoFoundNumber[0].L();

						if (lNoFoundNumber > 0)
						{
							if (m_pSelectedROI->m_dLineStartX != NULL)
							{
								m_pSelectedROI->m_dLineStartX[MAX_MODULE_ONE_TRAY - 1] = ColBegin[0].D();
								m_pSelectedROI->m_dLineStartY[MAX_MODULE_ONE_TRAY - 1] = RowBegin[0].D();
								m_pSelectedROI->m_dLineEndX[MAX_MODULE_ONE_TRAY - 1] = ColEnd[0].D();
								m_pSelectedROI->m_dLineEndY[MAX_MODULE_ONE_TRAY - 1] = RowEnd[0].D();
							}

							tuple_concat(RowBegin[0], RowEnd[0], &RowBegin);
							tuple_concat(ColBegin[0], ColEnd[0], &ColBegin);

							gen_contour_polygon_xld(&RegressContour, RowBegin, ColBegin);

							concat_obj(HLineFitXLD, RegressContour, &HLineFitXLD);
						}
					}
				} // if (lNoFoundNumber >= 2)
				else
				{
					m_pSelectedROI->m_dEdgeCenterX[MAX_MODULE_ONE_TRAY - 1] = INVALID_ALIGN_RESULT;
					m_pSelectedROI->m_dEdgeCenterY[MAX_MODULE_ONE_TRAY - 1] = INVALID_ALIGN_RESULT;
					m_pSelectedROI->m_dLineStartX[MAX_MODULE_ONE_TRAY - 1] = INVALID_ALIGN_RESULT;
					m_pSelectedROI->m_dLineStartY[MAX_MODULE_ONE_TRAY - 1] = INVALID_ALIGN_RESULT;
					m_pSelectedROI->m_dLineEndX[MAX_MODULE_ONE_TRAY - 1] = INVALID_ALIGN_RESULT;
					m_pSelectedROI->m_dLineEndY[MAX_MODULE_ONE_TRAY - 1] = INVALID_ALIGN_RESULT;

					return;
				}
			}

			if (THEAPP.m_pGFunction->ValidHXLD(HLineFitXLD))
				concat_obj(THEAPP.m_pInspectAdminViewDlg->m_HParameterDefectXLD, HLineFitXLD, &(THEAPP.m_pInspectAdminViewDlg->m_HParameterDefectXLD));

			THEAPP.m_pInspectAdminViewDlg->UpdateView();

		}
		catch (HException& except)
		{
			CString str; str.Format("Halcon Exception [Algorithm::EdgeMeasureAlgorithm] : %s", except.message); THEAPP.SaveLog(str);
		}

	}
}


void CROIEdgeMeasureDlg::OnBnClickedButtonRoiAffineTrans()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	UpdateData();

	double dAngle = m_dEditEdgeMeasureRotateAngle - m_pSelectedROI->m_AlgorithmParam->m_dROIEdgeMeasureRotateAngle;

	THEAPP.m_pInspectAdminViewDlg->GetRegionROIAfterRotation(dAngle);

	m_pSelectedROI->m_AlgorithmParam->m_dROIEdgeMeasureRotateAngle = m_dEditEdgeMeasureRotateAngle;

	UpdateData(FALSE);
}
