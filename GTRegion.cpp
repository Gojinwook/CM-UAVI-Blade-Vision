#include "stdafx.h"
#include "math.h"
#include "GTRegion.h"
#include "uScan.h"

IMPLEMENT_DYNCLASS(GTRegion)

GTRegion::GTRegion()
{
	Reset();
}

GTRegion::~GTRegion()
{
	if (m_HPartModelID >= 0)
	{
		clear_ncc_model(m_HPartModelID);
		m_HPartModelID = -1;
	}

	if (m_HAnisoModelID >= 0)
	{
		clear_shape_model(m_HAnisoModelID);
		m_HAnisoModelID = -1;
	}

	gen_empty_obj(&m_HTeachPolygonRgn);

	SAFE_DELETE(m_pChildTRegionList);
}

void GTRegion::Reset()
{
	m_pChildTRegionList = new PList<GTRegion>(PLNO_CONTENT);

	mbVisible = TRUE;
	mbSelectable = TRUE;
	mbMovable = TRUE;
	mbSizable = TRUE;
	mbSelect = FALSE;
	mbShape = FALSE; // Added by Gojw	2005/11/25

	miPadID = -1; // Added by Gojw	2005/12/08
	miLineStyle = PS_SOLID;
	miLineThickness = 1;
	mLineColor = RGB(0, 255, 0);
	mBackColor = RGB(0, 0, 0);
	miBkMode = TRANSPARENT;

	miRegionStyle = GTR_RS_NULL;
	mRegionColor = RGB(255, 255, 255);

	ZeroMemory(maPoints, sizeof(DPOINT) * 2);

	maPoints[0].x = maPoints[0].y = 0;
	maPoints[1].x = maPoints[1].y = 0;

	mbGerberSelected = FALSE; // Added by Gojw	2005/11/25
	mbRealPad = FALSE;		  // Added by Gojw	2005/11/25

	mbLastSelected = FALSE;

	m_iTeachImageIndex = -1;
	m_iInspectionType = -1;
	m_bRegionROI = FALSE;
	gen_empty_obj(&m_HTeachPolygonRgn);

	// 24.02.29 Local Align 추가 - LeeGW Start
	m_iLocalAlignID = -1;
	gen_empty_obj(&m_HGenerateResultRgn);
	for (int i = 0; i < MAX_MODULE_ONE_TRAY; i++)
		ResetLocalAlignResult(i);

	// FAI 추가 - LeeGW Start
	m_iFAIOuterCircleID = -1;
	m_iFAIInnerCircleID = -1;
	m_iFAIDatumCircleID = -1;
	m_iFAIDatumEllipseID = -1;
	m_iFAIWeldingSpotID = -1;
	m_iFAIWeldingPocketID = -1;
	m_iFAIBracketID = -1;
}

BOOL GTRegion::Load(HANDLE hFile, int iModelVersion)
{
	try
	{
		DWORD dwBytesRead;

		ReadFile(hFile, &m_iTeachImageIndex, sizeof(int), &dwBytesRead, NULL);
		ReadFile(hFile, &m_iInspectionType, sizeof(int), &dwBytesRead, NULL);
		ReadFile(hFile, &m_bRegionROI, sizeof(BOOL), &dwBytesRead, NULL);

		BOOL bValidRgn;
		
		ReadFile(hFile, &bValidRgn, sizeof(BOOL), &dwBytesRead, NULL);
		if (bValidRgn)
		{
			int iRegionPointNum;
			long lTupleLength;
			HTuple HRow = HTuple();
			HTuple HColumnBegin = HTuple();
			HTuple HColumnEnd = HTuple();

			ReadFile(hFile, &iRegionPointNum, sizeof(int), &dwBytesRead, NULL);
			for (int i = 0; i < iRegionPointNum; i++)
			{
				ReadFile(hFile, &lTupleLength, sizeof(long), &dwBytesRead, NULL);
				tuple_concat(HRow, lTupleLength, &HRow);
				ReadFile(hFile, &lTupleLength, sizeof(long), &dwBytesRead, NULL);
				tuple_concat(HColumnBegin, lTupleLength, &HColumnBegin);
				ReadFile(hFile, &lTupleLength, sizeof(long), &dwBytesRead, NULL);
				tuple_concat(HColumnEnd, lTupleLength, &HColumnEnd);
			}

			if (iRegionPointNum > 0)
			{
				gen_region_runs(&m_HTeachPolygonRgn, HRow, HColumnBegin, HColumnEnd);
			}
		}

		ReadFile(hFile, &mbVisible, sizeof(BOOL), &dwBytesRead, NULL);
		ReadFile(hFile, &mbSelectable, sizeof(BOOL), &dwBytesRead, NULL);
		ReadFile(hFile, &mbMovable, sizeof(BOOL), &dwBytesRead, NULL);
		ReadFile(hFile, &mbSizable, sizeof(BOOL), &dwBytesRead, NULL);

		ReadFile(hFile, &miPadID, sizeof(int), &dwBytesRead, NULL);
		ReadFile(hFile, &miLineStyle, sizeof(int), &dwBytesRead, NULL);
		ReadFile(hFile, &miLineThickness, sizeof(int), &dwBytesRead, NULL);
		ReadFile(hFile, &mLineColor, sizeof(int), &dwBytesRead, NULL);
		ReadFile(hFile, &mBackColor, sizeof(int), &dwBytesRead, NULL);
		ReadFile(hFile, &miBkMode, sizeof(int), &dwBytesRead, NULL);
		ReadFile(hFile, &miRegionStyle, sizeof(int), &dwBytesRead, NULL);
		ReadFile(hFile, &mRegionColor, sizeof(int), &dwBytesRead, NULL);

		ReadFile(hFile, &maPoints[0].x, sizeof(double), &dwBytesRead, NULL);
		ReadFile(hFile, &maPoints[0].y, sizeof(double), &dwBytesRead, NULL);
		ReadFile(hFile, &maPoints[1].x, sizeof(double), &dwBytesRead, NULL);
		ReadFile(hFile, &maPoints[1].y, sizeof(double), &dwBytesRead, NULL);

		ReadFile(hFile, &mbRealPad, sizeof(BOOL), &dwBytesRead, NULL);
		ReadFile(hFile, &mbShape, sizeof(BOOL), &dwBytesRead, NULL);

		ReadFile(hFile, &m_iLocalAlignID, sizeof(int), &dwBytesRead, NULL); // 24.03.02 Local Align 추가 - LeeGW Start

		// FAI 추가 - LeeGW
		ReadFile(hFile, &m_iFAIOuterCircleID, sizeof(int), &dwBytesRead, NULL);
		ReadFile(hFile, &m_iFAIInnerCircleID, sizeof(int), &dwBytesRead, NULL);
		ReadFile(hFile, &m_iFAIDatumCircleID, sizeof(int), &dwBytesRead, NULL);
		if (iModelVersion >= 3005)
		{
			ReadFile(hFile, &m_iFAIDatumEllipseID, sizeof(int), &dwBytesRead, NULL);
		}

		for (int i = 0; i < MAX_ROI_TEACHING_TAB; i++)
			m_AlgorithmParam[i].Load(hFile, iModelVersion);
			
		// BUG : Halcon Exception -> Wrong value of control parameter: 6 in operator create_aniso_shape_model_xld
		for (int i = 0; i < MAX_ROI_TEACHING_TAB; i++)
		{
			THEAPP.DoEvents();

			if (m_AlgorithmParam[i].m_bUseROIAnisoAlign) // 1st 검사탭
			{
				if (m_HAnisoModelID >= 0)
				{
					clear_shape_model(m_HAnisoModelID);
					m_HAnisoModelID = -1;
				}

				Hobject HInspectAreaRgn;
				HInspectAreaRgn = GetROIHRegion(THEAPP.m_pCalDataService);

				Hobject HInspectAreaXLD;
				gen_contour_region_xld(HInspectAreaRgn, &HInspectAreaXLD, "border_holes");

				if (m_AlgorithmParam[i].m_bUseROIAnisoAlign)
				{
					HTuple dAngleRangeRad;
					tuple_rad((double)m_AlgorithmParam[i].m_iROIAnisoAlignMatchingAngleRange, &dAngleRangeRad);

					// BUG : Halcon Exception -> Wrong value of control parameter: 6 in operator create_aniso_shape_model_xld
					// m_dROIAnisoAlignMatchingScaleMinY -> 2.121995791459e-313#DEN 
					create_aniso_shape_model_xld
					(
						HInspectAreaXLD, 
						"auto", 
						-dAngleRangeRad,									   //AngleStart  : Smallest rotation of the pattern.
						(dAngleRangeRad * 2.0),								   //AngleExtent : Extent of the rotation angles.
						"auto", 
						m_AlgorithmParam[i].m_dROIAnisoAlignMatchingScaleMinY, //ScaleRMin : Minimum scale of the pattern in the row direction.
						m_AlgorithmParam[i].m_dROIAnisoAlignMatchingScaleMaxY, //ScaleRMax : Maximum scale of the pattern in the row direction.
						"auto", 
						m_AlgorithmParam[i].m_dROIAnisoAlignMatchingScaleMinX, //ScaleCMin : Minimum scale of the pattern in the column direction.
						m_AlgorithmParam[i].m_dROIAnisoAlignMatchingScaleMaxX, //ScaleCMax : Maximum scale of the pattern in the column direction.
						"auto", "auto", "ignore_local_polarity", 
						m_AlgorithmParam[i].m_iROIAnisoAlignMatchingMinContrast, //MinContrast : Minimum contrast of the objects in the search images.
						&m_HAnisoModelID
					);
				}
			}
		}

		// Load Local Align Model
		BOOL bPartModelExist = FALSE;
		ReadFile(hFile, &bPartModelExist, sizeof(BOOL), &dwBytesRead, NULL);

		if (bPartModelExist)
		{
			CString szTemplateFolder, sTemplateFolder;
			szTemplateFolder = THEAPP.GetWorkingDirectory() + "\\Model\\" + THEAPP.m_pModelDataManager->m_sModelName + "\\SW\\MatchingModel";

			sTemplateFolder.Format("%s\\PartCheckModel_%d_%d", szTemplateFolder, m_iTeachImageIndex, m_iLocalAlignID);
			read_ncc_model(HTuple(sTemplateFolder), &m_HPartModelID);
		}

		BOOL bAnisoModelExist = FALSE;
		ReadFile(hFile, &bAnisoModelExist, sizeof(BOOL), &dwBytesRead, NULL);

		int iChildTRegionCount;
		ReadFile(hFile, &iChildTRegionCount, sizeof(int), &dwBytesRead, NULL);

		if (iModelVersion >= 3012)
		{
			ReadFile(hFile, &m_iFAIWeldingSpotID, sizeof(int), &dwBytesRead, NULL);
			ReadFile(hFile, &m_iFAIWeldingPocketID, sizeof(int), &dwBytesRead, NULL);
			ReadFile(hFile, &m_iFAIBracketID, sizeof(int), &dwBytesRead, NULL);
		}

		GTRegion *pTRegion;
		PString ClassName;
		for (int i = 0; i < iChildTRegionCount; i++)
		{
			ClassName.Load(hFile);
			pTRegion = (GTRegion *)PGetDynClass(ClassName.mcpStr)->CreateObject();
			if (!pTRegion->Load(hFile, iModelVersion))
				return FALSE;
			AddChildTRegion(pTRegion);
		}
	}
	catch (HException& except)
	{
		CString strErr;
		strErr.Format("Halcon Exception [GTRegion::Load] : %s", except.message);
		THEAPP.DoubleLogOut(strErr);
	}
	return TRUE;
}

void GTRegion::Save(HANDLE hFile)
{
	DWORD dwBytesWritten;

	WriteFile(hFile, &m_iTeachImageIndex, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iInspectionType, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_bRegionROI, sizeof(BOOL), &dwBytesWritten, NULL);

	BOOL bValidRgn;
	HTuple HRow, HColumnBegin, HColumnEnd;
	int iRegionPointNum;
	Hlong lTemp;

	if (THEAPP.m_pGFunction->ValidHRegion(m_HTeachPolygonRgn))
	{
		bValidRgn = TRUE;
		WriteFile(hFile, &bValidRgn, sizeof(BOOL), &dwBytesWritten, NULL);

		get_region_runs(m_HTeachPolygonRgn, &HRow, &HColumnBegin, &HColumnEnd);
		tuple_length(HRow, &lTemp);
		iRegionPointNum = lTemp;
		if (iRegionPointNum <= 0)
			iRegionPointNum = 0;

		WriteFile(hFile, &iRegionPointNum, sizeof(int), &dwBytesWritten, NULL);
		for (int i = 0; i < iRegionPointNum; i++)
		{
			lTemp = HRow[i].L();
			WriteFile(hFile, &lTemp, sizeof(long), &dwBytesWritten, NULL);

			lTemp = HColumnBegin[i].L();
			WriteFile(hFile, &lTemp, sizeof(long), &dwBytesWritten, NULL);

			lTemp = HColumnEnd[i].L();
			WriteFile(hFile, &lTemp, sizeof(long), &dwBytesWritten, NULL);
		}
	}
	else
	{
		bValidRgn = FALSE;
		WriteFile(hFile, &bValidRgn, sizeof(BOOL), &dwBytesWritten, NULL);
	}

	WriteFile(hFile, &mbVisible, sizeof(BOOL), &dwBytesWritten, NULL);
	WriteFile(hFile, &mbSelectable, sizeof(BOOL), &dwBytesWritten, NULL);
	WriteFile(hFile, &mbMovable, sizeof(BOOL), &dwBytesWritten, NULL);
	WriteFile(hFile, &mbSizable, sizeof(BOOL), &dwBytesWritten, NULL);

	WriteFile(hFile, &miPadID, sizeof(int), &dwBytesWritten, NULL);

	WriteFile(hFile, &miLineStyle, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &miLineThickness, sizeof(int), &dwBytesWritten, NULL);

	WriteFile(hFile, &mLineColor, sizeof(int), &dwBytesWritten, NULL);

	WriteFile(hFile, &mBackColor, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &miBkMode, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &miRegionStyle, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &mRegionColor, sizeof(int), &dwBytesWritten, NULL);

	WriteFile(hFile, &maPoints[0].x, sizeof(double), &dwBytesWritten, NULL);
	WriteFile(hFile, &maPoints[0].y, sizeof(double), &dwBytesWritten, NULL);
	WriteFile(hFile, &maPoints[1].x, sizeof(double), &dwBytesWritten, NULL);
	WriteFile(hFile, &maPoints[1].y, sizeof(double), &dwBytesWritten, NULL);

	WriteFile(hFile, &mbRealPad, sizeof(BOOL), &dwBytesWritten, NULL);
	WriteFile(hFile, &mbShape, sizeof(BOOL), &dwBytesWritten, NULL);

	WriteFile(hFile, &m_iLocalAlignID, sizeof(int), &dwBytesWritten, NULL); // 24.03.02 Local Align 추가 - LeeGW Start

	// FAI 추가 - LeeGW
	WriteFile(hFile, &m_iFAIOuterCircleID, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iFAIInnerCircleID, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iFAIDatumCircleID, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iFAIDatumEllipseID, sizeof(int), &dwBytesWritten, NULL);

	for (int i = 0; i < MAX_ROI_TEACHING_TAB; i++)
		m_AlgorithmParam[i].Save(hFile);

	// Save Local Align Model
	BOOL bPartModelExist;
	if (m_HPartModelID >= 0)
	{
		bPartModelExist = TRUE;
		WriteFile(hFile, &bPartModelExist, sizeof(BOOL), &dwBytesWritten, NULL);

		CString szTemplateFolder, sTemplateFolder, strModelFolder;
		strModelFolder = THEAPP.GetWorkingDirectory() + "\\Model\\";
		szTemplateFolder = strModelFolder + THEAPP.m_pModelDataManager->m_sModelName + "\\SW\\MatchingModel";
		sTemplateFolder.Format("%s\\PartCheckModel_%d_%d", szTemplateFolder, m_iTeachImageIndex, m_iLocalAlignID);

		write_ncc_model(m_HPartModelID, HTuple(sTemplateFolder));
	}
	else
	{
		bPartModelExist = FALSE;
		WriteFile(hFile, &bPartModelExist, sizeof(BOOL), &dwBytesWritten, NULL);
	}

	//// 비등방 추가 - LeeGW
	BOOL bAnisoModelExist = FALSE;
	WriteFile(hFile, &bAnisoModelExist, sizeof(BOOL), &dwBytesWritten, NULL);

	int iChildTRegionCount = GetChildTRegionCount();
	WriteFile(hFile, &iChildTRegionCount, sizeof(int), &dwBytesWritten, NULL);

	WriteFile(hFile, &m_iFAIWeldingSpotID, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iFAIWeldingPocketID, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iFAIBracketID, sizeof(int), &dwBytesWritten, NULL);

	GTRegion *pTRegion;
	for (int i = 0; i < iChildTRegionCount; i++)
	{
		pTRegion = GetChildTRegion(i);

		if (pTRegion == NULL)
			continue;

		pTRegion->GetDynClass()->mName.Save(hFile);
		pTRegion->Save(hFile);
	}
}

void GTRegion::Duplicate(GTRegion **ppTRegion)
{
	GTRegion *pTRegion, *pTRegionDup;

	if (GetDynClass()->mName == "GTRegion")
		*ppTRegion = new GTRegion;

	// 24.03.04 Local Align 추가 - LeeGW Start
	for (int i = 0; i < MAX_ROI_TEACHING_TAB; i++)
		(*ppTRegion)->m_AlgorithmParam[i] = m_AlgorithmParam[i];

	// 24.03.04 Local Align 추가 - LeeGW End

	(*ppTRegion)->m_iTeachImageIndex = m_iTeachImageIndex;
	(*ppTRegion)->m_iInspectionType = m_iInspectionType;
	(*ppTRegion)->m_bRegionROI = m_bRegionROI;
	(*ppTRegion)->m_HTeachPolygonRgn = m_HTeachPolygonRgn;

	(*ppTRegion)->mbVisible = mbVisible;
	(*ppTRegion)->mbSelectable = mbSelectable;
	(*ppTRegion)->mbSizable = mbSizable;
	(*ppTRegion)->mbMovable = mbMovable;
	(*ppTRegion)->mbSelect = mbSelect;
	(*ppTRegion)->m_iLocalAlignID = m_iLocalAlignID;
	for (int ii = 0; ii < MAX_MODULE_ONE_TRAY; ii++)
	{
		(*ppTRegion)->m_HLocalAlignShapeRgn[ii] = m_HLocalAlignShapeRgn[ii];
		(*ppTRegion)->m_iLocalAlignDeltaX[ii] = m_iLocalAlignDeltaX[ii];
		(*ppTRegion)->m_iLocalAlignDeltaY[ii] = m_iLocalAlignDeltaY[ii];
		(*ppTRegion)->m_dLocalAlignDeltaAngle[ii] = m_dLocalAlignDeltaAngle[ii];
		(*ppTRegion)->m_dLocalAlignDeltaAngleFixedPointX[ii] = m_dLocalAlignDeltaAngleFixedPointX[ii];
		(*ppTRegion)->m_dLocalAlignDeltaAngleFixedPointY[ii] = m_dLocalAlignDeltaAngleFixedPointY[ii];
		(*ppTRegion)->m_iLocalAlignLineFitXPos[ii] = m_iLocalAlignLineFitXPos[ii];
		(*ppTRegion)->m_iLocalAlignLineFitYPos[ii] = m_iLocalAlignLineFitYPos[ii];
		(*ppTRegion)->m_iLocalAlignLineFitXPos2[ii] = m_iLocalAlignLineFitXPos2[ii];
		(*ppTRegion)->m_iLocalAlignLineFitYPos2[ii] = m_iLocalAlignLineFitYPos2[ii];
		(*ppTRegion)->m_dLocalAlignAngleFitXPos[ii] = m_dLocalAlignAngleFitXPos[ii];
		(*ppTRegion)->m_dLocalAlignAngleFitYPos[ii] = m_dLocalAlignAngleFitYPos[ii];
		(*ppTRegion)->m_dLocalAlignAngleFitAngle[ii] = m_dLocalAlignAngleFitAngle[ii];

		(*ppTRegion)->m_dEdgeCenterX[ii] = m_dEdgeCenterX[ii];
		(*ppTRegion)->m_dEdgeCenterY[ii] = m_dEdgeCenterY[ii];
		(*ppTRegion)->m_dLineStartX[ii] = m_dLineStartX[ii];
		(*ppTRegion)->m_dLineStartY[ii] = m_dLineStartY[ii];
		(*ppTRegion)->m_dLineEndX[ii] = m_dLineEndX[ii];
		(*ppTRegion)->m_dLineEndY[ii] = m_dLineEndY[ii];
	}

	(*ppTRegion)->m_HGenerateResultRgn = m_HGenerateResultRgn;

	// FAI 추가 - LeeGW Start
	(*ppTRegion)->m_iFAIOuterCircleID = m_iFAIOuterCircleID;
	(*ppTRegion)->m_iFAIInnerCircleID = m_iFAIInnerCircleID;
	(*ppTRegion)->m_iFAIDatumCircleID = m_iFAIDatumCircleID;
	(*ppTRegion)->m_iFAIDatumEllipseID = m_iFAIDatumEllipseID;
	(*ppTRegion)->m_iFAIWeldingSpotID = m_iFAIWeldingSpotID;
	(*ppTRegion)->m_iFAIWeldingPocketID = m_iFAIWeldingPocketID;
	(*ppTRegion)->m_iFAIBracketID = m_iFAIBracketID;

	// Part Check
	if (m_HPartModelID >= 0)
	{
		CString sModelPath = THEAPP.GetWorkingDirectory() + "\\CopyModel";
		write_ncc_model(m_HPartModelID, HTuple(sModelPath));
		read_ncc_model(HTuple(sModelPath), &((*ppTRegion)->m_HPartModelID));
	}

	// 비등방 추가 - LeeGW
	if (m_HAnisoModelID >= 0)
	{
		CString sModelPath = THEAPP.GetWorkingDirectory() + "\\CopyModel";
		write_shape_model(m_HAnisoModelID, HTuple(sModelPath));
		read_shape_model(HTuple(sModelPath), &((*ppTRegion)->m_HAnisoModelID));
	}

	// 24.02.29 Local Align 추가 - LeeGW Start
	(*ppTRegion)->mbLastSelected = mbLastSelected;
	(*ppTRegion)->mbShape = mbShape;

	(*ppTRegion)->miPadID = miPadID;
	(*ppTRegion)->miLineStyle = miLineStyle;
	(*ppTRegion)->miLineThickness = miLineThickness;
	(*ppTRegion)->mLineColor = mLineColor;
	(*ppTRegion)->mBackColor = mBackColor;
	(*ppTRegion)->miBkMode = miBkMode;
	(*ppTRegion)->miRegionStyle = miRegionStyle;
	(*ppTRegion)->mRegionColor = mRegionColor;

	(*ppTRegion)->maPoints[0] = maPoints[0];
	(*ppTRegion)->maPoints[1] = maPoints[1];

	(*ppTRegion)->mbGerberSelected = mbGerberSelected;
	(*ppTRegion)->mbRealPad = mbRealPad;
}

GTRegion &GTRegion::operator=(GTRegion &data)
{
	if (this == &data)
		return *this;

	m_iTeachImageIndex = data.m_iTeachImageIndex;
	m_iInspectionType = data.m_iInspectionType;
	m_bRegionROI = data.m_bRegionROI;
	m_HTeachPolygonRgn = data.m_HTeachPolygonRgn;

	mbVisible = data.mbVisible;
	mbSelectable = data.mbSelectable;
	mbSizable = data.mbSizable;
	mbMovable = data.mbMovable;
	mbSelect = data.mbSelect;
	m_iLocalAlignID = data.m_iLocalAlignID;
	for (int ii = 0; ii < MAX_MODULE_ONE_TRAY; ii++)
	{
		m_HLocalAlignShapeRgn[ii] = data.m_HLocalAlignShapeRgn[ii];
		m_iLocalAlignDeltaX[ii] = data.m_iLocalAlignDeltaX[ii];
		m_iLocalAlignDeltaY[ii] = data.m_iLocalAlignDeltaY[ii];
		m_dLocalAlignDeltaAngle[ii] = data.m_dLocalAlignDeltaAngle[ii];
		m_dLocalAlignDeltaAngleFixedPointX[ii] = data.m_dLocalAlignDeltaAngleFixedPointX[ii];
		m_dLocalAlignDeltaAngleFixedPointY[ii] = data.m_dLocalAlignDeltaAngleFixedPointY[ii];
		m_iLocalAlignLineFitXPos[ii] = data.m_iLocalAlignLineFitXPos[ii];
		m_iLocalAlignLineFitYPos[ii] = data.m_iLocalAlignLineFitYPos[ii];
		m_iLocalAlignLineFitXPos2[ii] = data.m_iLocalAlignLineFitXPos2[ii];
		m_iLocalAlignLineFitYPos2[ii] = data.m_iLocalAlignLineFitYPos2[ii];
		m_dLocalAlignAngleFitXPos[ii] = data.m_dLocalAlignAngleFitXPos[ii];
		m_dLocalAlignAngleFitYPos[ii] = data.m_dLocalAlignAngleFitYPos[ii];
		m_dLocalAlignAngleFitAngle[ii] = data.m_dLocalAlignAngleFitAngle[ii];

		m_dEdgeCenterX[ii] = data.m_dEdgeCenterX[ii];
		m_dEdgeCenterY[ii] = data.m_dEdgeCenterY[ii];
		m_dLineStartX[ii] = data.m_dLineStartX[ii];
		m_dLineStartY[ii] = data.m_dLineStartY[ii];
		m_dLineEndX[ii] = data.m_dLineEndX[ii];
		m_dLineEndY[ii] = data.m_dLineEndY[ii];
	}

	m_HGenerateResultRgn = data.m_HGenerateResultRgn;

	// FAI 추가 - LeeGW
	m_iFAIOuterCircleID = data.m_iFAIOuterCircleID;
	m_iFAIInnerCircleID = data.m_iFAIInnerCircleID;
	m_iFAIDatumCircleID = data.m_iFAIDatumCircleID;
	m_iFAIDatumEllipseID = data.m_iFAIDatumEllipseID;
	m_iFAIWeldingSpotID = data.m_iFAIWeldingSpotID;
	m_iFAIWeldingPocketID = data.m_iFAIWeldingPocketID;
	m_iFAIBracketID = data.m_iFAIBracketID;

	// 24.02.29 Local Align 추가 - LeeGW End
	mbLastSelected = data.mbLastSelected;
	mbShape = data.mbShape;

	miPadID = data.miPadID;
	miLineStyle = data.miLineStyle;
	miLineThickness = data.miLineThickness;
	mLineColor = data.mLineColor;
	mBackColor = data.mBackColor;
	miBkMode = data.miBkMode;
	miRegionStyle = data.miRegionStyle;
	mRegionColor = data.mRegionColor;

	maPoints[0] = data.maPoints[0];
	maPoints[1] = data.maPoints[1];

	mbGerberSelected = data.mbGerberSelected;
	mbRealPad = data.mbRealPad;

	ClearAllChildTRegion();

	for (int i = 0; i < data.GetChildTRegionCount(); i++)
	{
		GTRegion *pTRegion = new GTRegion;
		GTRegion *pTRegionDup = data.GetChildTRegion(i);
		*pTRegion = *pTRegionDup;
		AddChildTRegion(pTRegion);
	}
	return *this;
}

/////////////////////////////////////////////////////////////////////////////

void GTRegion::GetLTPoint(POINT *pIPoint, CCalDataService *pCalDataService)
{
	double x = min(maPoints[0].x, maPoints[1].x);
	double y = min(maPoints[0].y, maPoints[1].y);

	DPOINT dIPoint;
	pCalDataService->RPtoIP(x, y, &dIPoint.x, &dIPoint.y);

	pIPoint->x = (dIPoint.x < 0) ? 0 : (long)(dIPoint.x + 0.5);
	pIPoint->y = (dIPoint.y < 0) ? 0 : (long)(dIPoint.y + 0.5);
}

void GTRegion::GetRBPoint(POINT *pIPoint, CCalDataService *pCalDataService, int iImageWidth, int iImageHeight)
{
	double x = max(maPoints[0].x, maPoints[1].x);
	double y = max(maPoints[0].y, maPoints[1].y);

	DPOINT dIPoint;
	pCalDataService->RPtoIP(x, y, &dIPoint.x, &dIPoint.y);

	if (iImageWidth == -1)
		iImageWidth = (long)dIPoint.x + 0.5;
	if (iImageHeight == -1)
		iImageHeight = (long)dIPoint.y + 0.5;

	pIPoint->x = (dIPoint.x > iImageWidth) ? iImageWidth - 1 : (long)(dIPoint.x + 0.5);
	pIPoint->y = (dIPoint.y > iImageHeight) ? iImageHeight - 1 : (long)(dIPoint.y + 0.5);
}

void GTRegion::GetWHPoint(POINT *pIPoint, CCalDataService *pCalDataService)
{
	double x = fabs(maPoints[1].x - maPoints[0].x);
	double y = fabs(maPoints[1].y - maPoints[0].y);

	DPOINT dWHPoint;
	pCalDataService->RPtoIP(x, y, &dWHPoint.x, &dWHPoint.y);

	pIPoint->x = (long)(dWHPoint.x + 0.5);
	pIPoint->y = (long)(dWHPoint.y + 0.5);
}

void GTRegion::GetCenterPoint(POINT *pIPoint, CCalDataService *pCalDataService)
{
	double x = (maPoints[0].x + maPoints[1].x) / 2.0;
	double y = (maPoints[0].y + maPoints[1].y) / 2.0;

	DPOINT dIPoint;
	pCalDataService->RPtoIP(x, y, &dIPoint.x, &dIPoint.y);

	pIPoint->x = (long)(dIPoint.x + 0.5);
	pIPoint->y = (long)(dIPoint.y + 0.5);
}

void GTRegion::SetLTPoint(POINT Point, CCalDataService *pCalDataService)
{
	DPOINT dRPoint;
	pCalDataService->IPtoRP((double)Point.x, (double)Point.y, &dRPoint.x, &dRPoint.y);

	maPoints[0].x = dRPoint.x;
	maPoints[0].y = dRPoint.y;
}

void GTRegion::SetRBPoint(POINT Point, CCalDataService *pCalDataService)
{
	DPOINT dRPoint;
	pCalDataService->IPtoRP((double)Point.x, (double)Point.y, &dRPoint.x, &dRPoint.y);

	maPoints[1].x = dRPoint.x;
	maPoints[1].y = dRPoint.y;
}

void GTRegion::GetLTPointD(DPOINT *pIPoint, CCalDataService *pCalDataService)
{
	double x = min(maPoints[0].x, maPoints[1].x);
	double y = min(maPoints[0].y, maPoints[1].y);

	DPOINT dIPoint;
	pCalDataService->RPtoIP(x, y, &dIPoint.x, &dIPoint.y);

	pIPoint->x = (dIPoint.x < 0) ? 0 : dIPoint.x;
	pIPoint->y = (dIPoint.y < 0) ? 0 : dIPoint.y;
}

void GTRegion::GetRBPointD(DPOINT *pIPoint, CCalDataService *pCalDataService, int iImageWidth, int iImageHeight)
{
	double x = max(maPoints[0].x, maPoints[1].x);
	double y = max(maPoints[0].y, maPoints[1].y);

	DPOINT dIPoint;
	pCalDataService->RPtoIP(x, y, &dIPoint.x, &dIPoint.y);

	if (iImageWidth == -1)
		iImageWidth = (long)dIPoint.x + 0.5;
	if (iImageHeight == -1)
		iImageHeight = (long)dIPoint.y + 0.5;

	pIPoint->x = (dIPoint.x > iImageWidth) ? (double)(iImageWidth - 1) : dIPoint.x;
	pIPoint->y = (dIPoint.y > iImageHeight) ? (double)(iImageHeight - 1) : dIPoint.y;
}

void GTRegion::GetWHPointD(DPOINT *pIPoint, CCalDataService *pCalDataService)
{
	double x = fabs(maPoints[1].x - maPoints[0].x);
	double y = fabs(maPoints[1].y - maPoints[0].y);

	DPOINT dWHPoint;
	pCalDataService->RPtoIP(x, y, &dWHPoint.x, &dWHPoint.y);

	pIPoint->x = dWHPoint.x;
	pIPoint->y = dWHPoint.y;
}

void GTRegion::GetCenterPointD(DPOINT *pIPoint, CCalDataService *pCalDataService)
{
	double x = (maPoints[0].x + maPoints[1].x) / 2.0;
	double y = (maPoints[0].y + maPoints[1].y) / 2.0;

	DPOINT dIPoint;
	pCalDataService->RPtoIP(x, y, &dIPoint.x, &dIPoint.y);

	pIPoint->x = dIPoint.x;
	pIPoint->y = dIPoint.y;
}

void GTRegion::SetLTPointD(DPOINT DPoint, CCalDataService *pCalDataService)
{
	DPOINT dRPoint;
	pCalDataService->IPtoRP(DPoint.x, DPoint.y, &dRPoint.x, &dRPoint.y);

	maPoints[0].x = dRPoint.x;
	maPoints[0].y = dRPoint.y;
}

void GTRegion::SetRBPointD(DPOINT DPoint, CCalDataService *pCalDataService)
{
	DPOINT dRPoint;
	pCalDataService->IPtoRP(DPoint.x, DPoint.y, &dRPoint.x, &dRPoint.y);

	maPoints[1].x = dRPoint.x;
	maPoints[1].y = dRPoint.y;
}

void GTRegion::GetLTPointM(DPOINT *pMPoint)
{
	pMPoint->x = maPoints[0].x;
	pMPoint->y = maPoints[0].y;
}

void GTRegion::GetRBPointM(DPOINT *pMPoint)
{
	pMPoint->x = maPoints[1].x;
	pMPoint->y = maPoints[1].y;
}

void GTRegion::GetWHPointM(DPOINT *pWHPoint)
{
	pWHPoint->x = fabs(maPoints[1].x - maPoints[0].x);
	pWHPoint->y = fabs(maPoints[1].y - maPoints[0].y);
}

void GTRegion::GetCenterPointM(DPOINT *pMCPoint)
{
	pMCPoint->x = (maPoints[0].x + maPoints[1].x) / 2.0;
	pMCPoint->y = (maPoints[0].y + maPoints[1].y) / 2.0;
}

void GTRegion::SetLTPointM(DPOINT MPoint)
{
	maPoints[0].x = MPoint.x;
	maPoints[0].y = MPoint.y;
}

void GTRegion::SetRBPointM(DPOINT MPoint)
{
	maPoints[1].x = MPoint.x;
	maPoints[1].y = MPoint.y;
}

void GTRegion::SetCenterPointM(DPOINT CenterPoint)
{
	DPOINT dHalfWHPoint;
	dHalfWHPoint.x = fabs(maPoints[1].x - maPoints[0].x) * 0.5;
	dHalfWHPoint.y = fabs(maPoints[1].y - maPoints[0].y) * 0.5;

	maPoints[0].x = CenterPoint.x - dHalfWHPoint.x;
	maPoints[0].y = CenterPoint.y - dHalfWHPoint.y;

	maPoints[1].x = CenterPoint.x + dHalfWHPoint.x;
	maPoints[1].y = CenterPoint.y + dHalfWHPoint.y;
}

void GTRegion::ArrangePoints()
{
	double dTemp;

	if (maPoints[0].x > maPoints[1].x)
		dTemp = maPoints[0].x, maPoints[0].x = maPoints[1].x, maPoints[1].x = dTemp;
	if (maPoints[0].y > maPoints[1].y)
		dTemp = maPoints[0].y, maPoints[0].y = maPoints[1].y, maPoints[1].y = dTemp;
}

int GTRegion::GetChildTRegionCount()
{
	return m_pChildTRegionList->miCount;
}

void GTRegion::AddChildTRegion(GTRegion *pTRegion)
{
	pTRegion->mpParent = this;
	m_pChildTRegionList->Add(pTRegion);
}

void GTRegion::InsertChildTRegion(int iIndex, GTRegion *pTRegion)
{
	pTRegion->mpParent = this;
	m_pChildTRegionList->Insert(iIndex, pTRegion);
}

void GTRegion::DeleteChildTRegion(int iIndex)
{
	m_pChildTRegionList->Delete(iIndex);
}

void GTRegion::DeleteChildTRegionPtr(GTRegion *pTRegion, BOOL bChildLink)
{
	for (int i = 0; i < GetChildTRegionCount(); i++)
	{
		if (GetChildTRegion(i) == pTRegion)
		{
			m_pChildTRegionList->Delete(i);
			return;
		}
		if (bChildLink)
			GetChildTRegion(i)->DeleteChildTRegionPtr(pTRegion, bChildLink);
	}
}

void GTRegion::ClearAllChildTRegion()
{
	m_pChildTRegionList->ClearAll();
}

void GTRegion::ClearAllSelectChildTRegion()
{
	PList<GTRegion> *pDeleteTRegionList = new PList<GTRegion>(PLNO_POINTER);
	int i;
	for (i = 0; i < GetChildTRegionCount(); i++)
	{
		GTRegion *pTRegion = GetChildTRegion(i);
		pTRegion->ClearAllSelectChildTRegion();
		if (pTRegion->GetSelect())
			pDeleteTRegionList->Add(pTRegion);
	}

	for (i = 0; i < pDeleteTRegionList->miCount; i++)
	{
		GTRegion *pTRegion = pDeleteTRegionList->Get(i);
		DeleteChildTRegionPtr(pTRegion, TRUE);
	}

	delete pDeleteTRegionList;
}

void GTRegion::SwapChildTRegion(int a, int b)
{
	m_pChildTRegionList->Swap(a, b);
}

GTRegion *GTRegion::GetChildTRegion(int iIndex)
{
	if (iIndex >= m_pChildTRegionList->miCount)
		return NULL;
	return m_pChildTRegionList->Get(iIndex);
}

BOOL GTRegion::RgnInRegion(GTRegion *pTRegion)
{
	ArrangePoints();
	pTRegion->ArrangePoints();

	if (maPoints[0].x < pTRegion->maPoints[0].x && maPoints[0].y < pTRegion->maPoints[0].y &&
		maPoints[1].x > pTRegion->maPoints[1].x && maPoints[1].y > pTRegion->maPoints[1].y)
		return TRUE;
	else
		return FALSE;
}

BOOL GTRegion::PtInCPoint(int iVX, int iVY, POINT VPoint)
{
	// GTR_CPHL => Control Point Half Length
	if (VPoint.x >= iVX - GTR_CPHL && VPoint.x <= iVX + GTR_CPHL &&
		VPoint.y >= iVY - GTR_CPHL && VPoint.y <= iVY + GTR_CPHL)
		return TRUE;
	return FALSE;
}

BOOL GTRegion::PtInSegment(POINT VPoint, GViewportManager *pVManager, CCalDataService *pCalDataService)
{
	POINT LTPoint, RBPoint;

	GetLTPoint(&LTPoint, pCalDataService);
	GetRBPoint(&RBPoint, pCalDataService);

	POINT aPoints[2];

	aPoints[0].x = min(LTPoint.x, RBPoint.x);
	aPoints[0].y = min(LTPoint.y, RBPoint.y);
	aPoints[1].x = max(LTPoint.x, RBPoint.x);
	aPoints[1].y = max(LTPoint.y, RBPoint.y);

	pVManager->IPtoVP(aPoints, 2);

	// top
	if (VPoint.x >= aPoints[0].x && VPoint.x <= aPoints[1].x && VPoint.y >= aPoints[0].y - GTR_CPHL && VPoint.y <= aPoints[0].y + GTR_CPHL)
		return TRUE;
	// bottom
	if (VPoint.x >= aPoints[0].x && VPoint.x <= aPoints[1].x && VPoint.y >= aPoints[1].y - GTR_CPHL && VPoint.y <= aPoints[1].y + GTR_CPHL)
		return TRUE;
	// left
	if (VPoint.y >= aPoints[0].y && VPoint.y <= aPoints[1].y && VPoint.x >= aPoints[0].x - GTR_CPHL && VPoint.x <= aPoints[0].x + GTR_CPHL)
		return TRUE;
	// right
	if (VPoint.y >= aPoints[0].y && VPoint.y <= aPoints[1].y && VPoint.x >= aPoints[1].x - GTR_CPHL && VPoint.x <= aPoints[1].x + GTR_CPHL)
		return TRUE;

	return FALSE;
}

BOOL GTRegion::PtInRegion(POINT VPoint, GViewportManager *pVManager, CCalDataService *pCalDataService)
{
	POINT LTPoint, RBPoint;

	GetLTPoint(&LTPoint, pCalDataService);
	GetRBPoint(&RBPoint, pCalDataService);

	POINT aPoints[2];

	aPoints[0].x = min(LTPoint.x, RBPoint.x);
	aPoints[0].y = min(LTPoint.y, RBPoint.y);
	aPoints[1].x = max(LTPoint.x, RBPoint.x);
	aPoints[1].y = max(LTPoint.y, RBPoint.y);

	RECT Rect = {min(aPoints[0].x, aPoints[1].x), min(aPoints[0].y, aPoints[1].y), max(aPoints[0].x, aPoints[1].x), max(aPoints[0].y, aPoints[1].y)};

	pVManager->IPtoVP((POINT *)&Rect, 2);

	return ::PtInRect(&Rect, VPoint);
}

BOOL GTRegion::GetVisible()
{
	return mbVisible;
}

BOOL GTRegion::GetSelectable()
{
	return mbSelectable;
}

BOOL GTRegion::GetMovable()
{
	return mbMovable;
}

BOOL GTRegion::GetSizable()
{
	return mbSizable;
}

BOOL GTRegion::GetSelect()
{
	return mbSelect;
}

BOOL GTRegion::GetShape()
{
	return mbShape;
}

void GTRegion::SetLineColor(COLORREF lineColor)
{
	mLineColor = lineColor;
}

void GTRegion::SetVisible(BOOL bVisible, BOOL bChildLink)
{
	if (bChildLink)
	{
		for (int i = 0; i < GetChildTRegionCount(); i++)
		{
			GTRegion *pTRegion = GetChildTRegion(i);
			pTRegion->SetVisible(bVisible, bChildLink);
		}
	}
	mbVisible = bVisible;
}

void GTRegion::SetSelectable(BOOL bSelectable, BOOL bChildLink)
{
	if (bChildLink)
	{
		for (int i = 0; i < GetChildTRegionCount(); i++)
		{
			GTRegion *pTRegion = GetChildTRegion(i);
			pTRegion->SetSelectable(bSelectable, bChildLink);
		}
	}
	mbSelectable = bSelectable;
}

void GTRegion::SetMovable(BOOL bMovable, BOOL bChildLink)
{
	if (bChildLink)
	{
		for (int i = 0; i < GetChildTRegionCount(); i++)
		{
			GTRegion *pTRegion = GetChildTRegion(i);
			pTRegion->SetMovable(bMovable, bChildLink);
		}
	}
	mbMovable = bMovable;
}

void GTRegion::SetSizable(BOOL bSizable, BOOL bChildLink)
{
	if (bChildLink)
	{
		for (int i = 0; i < GetChildTRegionCount(); i++)
		{
			GTRegion *pTRegion = GetChildTRegion(i);
			pTRegion->SetSizable(bSizable, bChildLink);
		}
	}
	mbSizable = bSizable;
}

void GTRegion::SetSelect(BOOL bSelect, BOOL bChildLink)
{
	if (bChildLink)
	{
		for (int i = 0; i < GetChildTRegionCount(); i++)
		{
			GTRegion *pTRegion = GetChildTRegion(i);
			if (pTRegion == NULL)
				continue;

			pTRegion->SetSelect(bSelect, bChildLink);
		}
	}
	mbSelect = bSelect;
	mbLastSelected = bSelect;
}

void GTRegion::SetShape(BOOL bShape, BOOL bChildLink)
{
	if (bChildLink)
	{
		for (int i = 0; i < GetChildTRegionCount(); i++)
		{
			GTRegion *pTRegion = GetChildTRegion(i);
			if (pTRegion == NULL)
				continue;

			pTRegion->SetShape(bShape, bChildLink);
		}
	}
	mbShape = bShape;
}

void GTRegion::ClearAllSelect()
{
	for (int i = 0; i < GetChildTRegionCount(); i++)
	{
		GTRegion *pTRegion = GetChildTRegion(i);
		if (pTRegion == NULL)
			continue;

		pTRegion->ClearAllSelect();
	}
	mbSelect = FALSE;
}

void GTRegion::ClearAllLastSelected()
{
	for (int i = 0; i < GetChildTRegionCount(); i++)
	{
		GTRegion *pTRegion = GetChildTRegion(i);
		if (pTRegion == NULL)
			continue;

		pTRegion->ClearAllLastSelected();
	}
	mbLastSelected = FALSE;
}

GTRegion *GTRegion::GetTopTRegion(POINT VPoint, GViewportManager *pVManager, CCalDataService *pCalDataService, int iCurTeachingTabIndex)
{
	if (m_iTeachImageIndex >= 0 && iCurTeachingTabIndex != m_iTeachImageIndex)
		return 0;

	int iRet = GetCPointIndex(VPoint, pVManager, pCalDataService);
	if (iRet != GTR_CP_NONE)
		return this;

	GTRegion *pMinAreaRegionClick = NULL;
	long lArea, lMinArea;

	lMinArea = 10000000000000;

	for (int i = GetChildTRegionCount() - 1; i >= 0; i--) // Used for Inspection ROI
	{
		GTRegion *pTRegion = GetChildTRegion(i);

		if (pTRegion == NULL)
			continue;

		if (pTRegion->m_iTeachImageIndex != iCurTeachingTabIndex)
			continue;

		GTRegion *pTopTRegion = pTRegion->GetTopTRegionArea(VPoint, pVManager, pCalDataService, iCurTeachingTabIndex, &lArea);

		if (pTopTRegion)
		{
			if (lArea > 0) // Region Click
			{
				if (lArea < lMinArea)
				{
					lMinArea = lArea;
					pMinAreaRegionClick = pTopTRegion;
				}
			}
			else
				return pTopTRegion;
		}
	}

	if (pMinAreaRegionClick != NULL)
		return pMinAreaRegionClick;

	return 0;
}

GTRegion *GTRegion::GetTopTRegionArea(POINT VPoint, GViewportManager *pVManager, CCalDataService *pCalDataService, int iCurTeachingTabIndex, long *plArea)
{
	*plArea = 0;

	if (m_iTeachImageIndex >= 0 && iCurTeachingTabIndex != m_iTeachImageIndex)
		return 0;

	int iRet = GetCPointIndex(VPoint, pVManager, pCalDataService);
	if (iRet != GTR_CP_NONE)
	{
		if (iRet == GTR_CP_REGION)
		{
			Hobject HROIRgn;

			if (m_bRegionROI)
			{
				HROIRgn = m_HTeachPolygonRgn;
			}
			else
			{
				POINT LTPoint, RBPoint;

				GetLTPoint(&LTPoint, pCalDataService);
				GetRBPoint(&RBPoint, pCalDataService);

				if (mbShape)
					gen_circle(&HROIRgn, (LTPoint.y + RBPoint.y) / 2, (LTPoint.x + RBPoint.x) / 2, (RBPoint.y - LTPoint.y) / 2);
				else
					gen_rectangle1(&HROIRgn, LTPoint.y, LTPoint.x, RBPoint.y, RBPoint.x);
			}

			POINT IPoint = VPoint;
			pVManager->VPtoIP(&IPoint, 1);

			Hlong IsInside;
			test_region_point(HROIRgn, (HTuple)IPoint.y, (HTuple)IPoint.x, &IsInside);

			if (IsInside != 1)
				return 0;

			Hlong lArea;
			double dCenterX, dCenterY;

			if (THEAPP.m_pGFunction->ValidHRegion(HROIRgn))
			{
				area_center(HROIRgn, &lArea, &dCenterY, &dCenterX);

				*plArea = lArea;
			}
		}

		return this;
	}

	return 0;
}

int GTRegion::GetCPointIndex(POINT VPoint, GViewportManager *pVManager, CCalDataService *pCalDataService)
{
	POINT aPoints[2];

	if (mbSelectable)
	{
		POINT LTPoint, RBPoint;

		GetLTPoint(&LTPoint, pCalDataService);
		GetRBPoint(&RBPoint, pCalDataService);

		aPoints[0].x = LTPoint.x;
		aPoints[0].y = LTPoint.y;
		aPoints[1].x = RBPoint.x;
		aPoints[1].y = RBPoint.y;

		pVManager->IPtoVP(aPoints, 2);

		if (PtInCPoint(aPoints[0].x, aPoints[0].y, VPoint))
			return GTR_CP_LT;
		else if (PtInCPoint(aPoints[0].x + (aPoints[1].x - aPoints[0].x) / 2, aPoints[0].y, VPoint))
			return GTR_CP_T;
		else if (PtInCPoint(aPoints[1].x, aPoints[0].y, VPoint))
			return GTR_CP_RT;
		else if (PtInCPoint(aPoints[1].x, aPoints[0].y + (aPoints[1].y - aPoints[0].y) / 2, VPoint))
			return GTR_CP_R;
		else if (PtInCPoint(aPoints[1].x, aPoints[1].y, VPoint))
			return GTR_CP_RB;
		else if (PtInCPoint(aPoints[0].x + (aPoints[1].x - aPoints[0].x) / 2, aPoints[1].y, VPoint))
			return GTR_CP_B;
		else if (PtInCPoint(aPoints[0].x, aPoints[1].y, VPoint))
			return GTR_CP_LB;
		else if (PtInCPoint(aPoints[0].x, aPoints[0].y + (aPoints[1].y - aPoints[0].y) / 2, VPoint))
			return GTR_CP_L;
		else if (PtInSegment(VPoint, pVManager, pCalDataService))
			return GTR_CP_SEGMENT;
		else if (PtInRegion(VPoint, pVManager, pCalDataService))
			return GTR_CP_REGION;
	}
	return GTR_CP_NONE;
}

void GTRegion::SetCPoint(int iCPointIndex, POINT *pMIPoints, CCalDataService *pCalDataService)
{
	double dAvgCalData;
	dAvgCalData = pCalDataService->GetPixelSize();

	DPOINT DiffPoint;
	DiffPoint.x = (pMIPoints[1].x - pMIPoints[0].x) * dAvgCalData;
	DiffPoint.y = (pMIPoints[1].y - pMIPoints[0].y) * dAvgCalData;

	switch (iCPointIndex)
	{
	case GTR_CP_SEGMENT:
	case GTR_CP_REGION:
		if (mbMovable)
		{
			maPoints[0].x += DiffPoint.x;
			maPoints[0].y += DiffPoint.y;
			maPoints[1].x += DiffPoint.x;
			maPoints[1].y += DiffPoint.y;
		}
		break;
	case GTR_CP_LT:
		if (mbSizable)
		{
			maPoints[0].x += DiffPoint.x;
			maPoints[0].y += DiffPoint.y;
		}
		break;
	case GTR_CP_T:
		if (mbSizable)
		{
			maPoints[0].y += DiffPoint.y;
		}
		break;
	case GTR_CP_RT:
		if (mbSizable)
		{
			maPoints[1].x += DiffPoint.x;
			maPoints[0].y += DiffPoint.y;
		}
		break;
	case GTR_CP_R:
		if (mbSizable)
		{
			maPoints[1].x += DiffPoint.x;
		}
		break;
	case GTR_CP_RB:
		if (mbSizable)
		{
			maPoints[1].x += DiffPoint.x;
			maPoints[1].y += DiffPoint.y;
		}
		break;
	case GTR_CP_B:
		if (mbSizable)
		{
			maPoints[1].y += DiffPoint.y;
		}
		break;
	case GTR_CP_LB:
		if (mbSizable)
		{
			maPoints[0].x += DiffPoint.x;
			maPoints[1].y += DiffPoint.y;
		}
		break;
	case GTR_CP_L:
		if (mbSizable)
		{
			maPoints[0].x += DiffPoint.x;
		}
		break;
	}
}

void GTRegion::Move(double dXMove, double dYMove, BOOL bChildLink, CCalDataService *pCalDataService)
{
	// 	int iCameraPixelSize;
	// 	double dMmPerPixel;
	// 	pCalDataService->GetResData(&iCameraPixelSize, &dMmPerPixel);
	//
	// 	DPOINT dMovePoint;
	//
	// 	pCalDataService->ILtoRL(dYMove, &(dMovePoint.y));
	// 	dMovePoint.x = dXMove * dMmPerPixel;

	double dXCalData, dYCalData, dAvgCalData;
	pCalDataService->GetPixelSize(&dXCalData, &dYCalData, &dAvgCalData);

	DPOINT dMovePoint;
	dMovePoint.x = dXMove * dXCalData;
	dMovePoint.y = dYMove * dYCalData;

	if (bChildLink)
	{
		for (int i = 0; i < GetChildTRegionCount(); i++)
		{
			GTRegion *pTRegion = GetChildTRegion(i);
			pTRegion->Move(dXMove, dYMove, bChildLink, pCalDataService);
		}
	}

	maPoints[0].x += dMovePoint.x;
	maPoints[0].y += dMovePoint.y;
	maPoints[1].x += dMovePoint.x;
	maPoints[1].y += dMovePoint.y;
}

void GTRegion::DrawControlPoint(HDC hDestDC, int iVX, int iVY)
{
	Rectangle(hDestDC, iVX - GTR_CPHL, iVY - GTR_CPHL, iVX + GTR_CPHL, iVY + GTR_CPHL);
}

void GTRegion::DrawSelectionPoint(HDC hDestDC, int iVX, int iVY)
{
	Rectangle(hDestDC, iVX - GTR_SPHL, iVY - GTR_SPHL, iVX + GTR_SPHL, iVY + GTR_SPHL);
}

void GTRegion::Draw(HDC hDestDC, GViewportManager *pVManager, int iDrawStyle, BOOL bDrawControlPoint, CCalDataService *pCalDataService, int iCurTeachingTabIndex)
{
	if (m_iTeachImageIndex >= 0 && iCurTeachingTabIndex != m_iTeachImageIndex)
		return;

	if (m_bRegionROI)
	{
		if (mbVisible)
		{
			if (iDrawStyle == GTR_DS_NORMAL && bDrawControlPoint == FALSE)
			{
				if (m_iInspectionType == INSPECTION_TYPE_LOCAL_ALIGN)
				{
					Hlong lArea;
					double dCenterX, dCenterY;

					HTuple HArea, HCenterX, HCenterY;
					area_center(m_HTeachPolygonRgn, &HArea, &HCenterY, &HCenterX);
					lArea = HArea[0].L();
					dCenterX = HCenterX[0].D();
					dCenterY = HCenterY[0].D();

					POINT CenterVPoint;
					CenterVPoint.x = (long)dCenterX;
					CenterVPoint.y = (long)dCenterY;

					pVManager->IPtoVP(&CenterVPoint, 1);

					CString sInspectionTypeName;
					sInspectionTypeName.Format("%d", m_iLocalAlignID);

					SetBkMode(hDestDC, TRANSPARENT);
					SetTextColor(hDestDC, RGB(255, 0, 255));
					TextOut(hDestDC, CenterVPoint.x, CenterVPoint.y, (LPCTSTR)sInspectionTypeName.GetBuffer(0), strlen((LPCTSTR)sInspectionTypeName));
				}
				else if (m_iInspectionType == INSPECTION_TYPE_FAI_OUTER_CIRCLE)
				{
					Hlong lArea;
					double dCenterX, dCenterY;

					HTuple HArea, HCenterX, HCenterY;
					area_center(m_HTeachPolygonRgn, &HArea, &HCenterY, &HCenterX);
					lArea = HArea[0].L();
					dCenterX = HCenterX[0].D();
					dCenterY = HCenterY[0].D();

					POINT CenterVPoint;
					CenterVPoint.x = (long)dCenterX;
					CenterVPoint.y = (long)dCenterY;

					pVManager->IPtoVP(&CenterVPoint, 1);

					CString sInspectionTypeName;
					sInspectionTypeName.Format("FAI Outer Cicle %d", m_iFAIOuterCircleID);

					SetBkMode(hDestDC, TRANSPARENT);
					SetTextColor(hDestDC, RGB(255, 165, 0));
					TextOut(hDestDC, CenterVPoint.x, CenterVPoint.y, (LPCTSTR)sInspectionTypeName.GetBuffer(0), strlen((LPCTSTR)sInspectionTypeName));
				}
				else if (m_iInspectionType == INSPECTION_TYPE_FAI_INNER_CIRCLE)
				{
					Hlong lArea;
					double dCenterX, dCenterY;

					HTuple HArea, HCenterX, HCenterY;
					area_center(m_HTeachPolygonRgn, &HArea, &HCenterY, &HCenterX);
					lArea = HArea[0].L();
					dCenterX = HCenterX[0].D();
					dCenterY = HCenterY[0].D();

					POINT CenterVPoint;
					CenterVPoint.x = (long)dCenterX;
					CenterVPoint.y = (long)dCenterY;

					pVManager->IPtoVP(&CenterVPoint, 1);

					CString sInspectionTypeName;
					sInspectionTypeName.Format("FAI Inner Cicle %d", m_iFAIInnerCircleID);

					SetBkMode(hDestDC, TRANSPARENT);
					SetTextColor(hDestDC, RGB(255, 165, 0));
					TextOut(hDestDC, CenterVPoint.x, CenterVPoint.y, (LPCTSTR)sInspectionTypeName.GetBuffer(0), strlen((LPCTSTR)sInspectionTypeName));
				}
				else if (m_iInspectionType == INSPECTION_TYPE_FAI_DATUM_CIRCLE)
				{
					Hlong lArea;
					double dCenterX, dCenterY;

					HTuple HArea, HCenterX, HCenterY;
					area_center(m_HTeachPolygonRgn, &HArea, &HCenterY, &HCenterX);
					lArea = HArea[0].L();
					dCenterX = HCenterX[0].D();
					dCenterY = HCenterY[0].D();

					POINT CenterVPoint;
					CenterVPoint.x = (long)dCenterX;
					CenterVPoint.y = (long)dCenterY;

					pVManager->IPtoVP(&CenterVPoint, 1);

					CString sInspectionTypeName;
					sInspectionTypeName.Format("FAI Datum Cicle %d", m_iFAIDatumCircleID);

					SetBkMode(hDestDC, TRANSPARENT);
					SetTextColor(hDestDC, RGB(255, 165, 0));
					TextOut(hDestDC, CenterVPoint.x, CenterVPoint.y, (LPCTSTR)sInspectionTypeName.GetBuffer(0), strlen((LPCTSTR)sInspectionTypeName));
				}
				else if (m_iInspectionType == INSPECTION_TYPE_FAI_DATUM_ELLIPSE)
				{
					Hlong lArea;
					double dCenterX, dCenterY;

					HTuple HArea, HCenterX, HCenterY;
					area_center(m_HTeachPolygonRgn, &HArea, &HCenterY, &HCenterX);
					lArea = HArea[0].L();
					dCenterX = HCenterX[0].D();
					dCenterY = HCenterY[0].D();

					POINT CenterVPoint;
					CenterVPoint.x = (long)dCenterX;
					CenterVPoint.y = (long)dCenterY;

					pVManager->IPtoVP(&CenterVPoint, 1);

					CString sInspectionTypeName;
					sInspectionTypeName.Format("FAI Datum Ellipse %d", m_iFAIDatumEllipseID);

					SetBkMode(hDestDC, TRANSPARENT);
					SetTextColor(hDestDC, RGB(255, 165, 0));
					TextOut(hDestDC, CenterVPoint.x, CenterVPoint.y, (LPCTSTR)sInspectionTypeName.GetBuffer(0), strlen((LPCTSTR)sInspectionTypeName));
				}
				else if (m_iInspectionType == INSPECTION_TYPE_FAI_WELDING_SPOT)
				{
					Hlong lArea;
					double dCenterX, dCenterY;

					HTuple HArea, HCenterX, HCenterY;
					area_center(m_HTeachPolygonRgn, &HArea, &HCenterY, &HCenterX);
					lArea = HArea[0].L();
					dCenterX = HCenterX[0].D();
					dCenterY = HCenterY[0].D();

					POINT CenterVPoint;
					CenterVPoint.x = (long)dCenterX;
					CenterVPoint.y = (long)dCenterY;

					pVManager->IPtoVP(&CenterVPoint, 1);

					CString sInspectionTypeName;
					sInspectionTypeName.Format("FAI Welding Spot %d", m_iFAIWeldingSpotID);

					SetBkMode(hDestDC, TRANSPARENT);
					SetTextColor(hDestDC, RGB(255, 165, 0));
					TextOut(hDestDC, CenterVPoint.x, CenterVPoint.y, (LPCTSTR)sInspectionTypeName.GetBuffer(0), strlen((LPCTSTR)sInspectionTypeName));
				}
				else if (m_iInspectionType == INSPECTION_TYPE_FAI_WELDING_POCKET)
				{
					Hlong lArea;
					double dCenterX, dCenterY;

					HTuple HArea, HCenterX, HCenterY;
					area_center(m_HTeachPolygonRgn, &HArea, &HCenterY, &HCenterX);
					lArea = HArea[0].L();
					dCenterX = HCenterX[0].D();
					dCenterY = HCenterY[0].D();

					POINT CenterVPoint;
					CenterVPoint.x = (long)dCenterX;
					CenterVPoint.y = (long)dCenterY;

					pVManager->IPtoVP(&CenterVPoint, 1);

					CString sInspectionTypeName;
					sInspectionTypeName.Format("FAI Welding Pocket %d", m_iFAIWeldingPocketID);

					SetBkMode(hDestDC, TRANSPARENT);
					SetTextColor(hDestDC, RGB(255, 165, 0));
					TextOut(hDestDC, CenterVPoint.x, CenterVPoint.y, (LPCTSTR)sInspectionTypeName.GetBuffer(0), strlen((LPCTSTR)sInspectionTypeName));
				}
				else if (m_iInspectionType == INSPECTION_TYPE_FAI_BRACKET)
				{
					Hlong lArea;
					double dCenterX, dCenterY;

					HTuple HArea, HCenterX, HCenterY;
					area_center(m_HTeachPolygonRgn, &HArea, &HCenterY, &HCenterX);
					lArea = HArea[0].L();
					dCenterX = HCenterX[0].D();
					dCenterY = HCenterY[0].D();

					POINT CenterVPoint;
					CenterVPoint.x = (long)dCenterX;
					CenterVPoint.y = (long)dCenterY;

					pVManager->IPtoVP(&CenterVPoint, 1);

					CString sInspectionTypeName;
					sInspectionTypeName.Format("FAI Bracket %d", m_iFAIBracketID);

					SetBkMode(hDestDC, TRANSPARENT);
					SetTextColor(hDestDC, RGB(255, 165, 0));
					TextOut(hDestDC, CenterVPoint.x, CenterVPoint.y, (LPCTSTR)sInspectionTypeName.GetBuffer(0), strlen((LPCTSTR)sInspectionTypeName));
				}
				else if (m_iInspectionType == INSPECTION_TYPE_INSPECTION || m_iInspectionType == INSPECTION_TYPE_AI_INSPECTION)
				{
					Hlong lArea;
					double dCenterX, dCenterY;

					area_center(m_HTeachPolygonRgn, &lArea, &dCenterY, &dCenterX);

					POINT CenterVPoint;
					CenterVPoint.x = (long)dCenterX;
					CenterVPoint.y = (long)dCenterY;

					pVManager->IPtoVP(&CenterVPoint, 1);
				}
			}
		}

		return;
	}

	HPEN hOldPen, hPen;
	HBRUSH hOldBrush, hBrush;
	POINT aPoints[2];

	POINT LTPoint, RBPoint;

	if (mbVisible && !(iDrawStyle == GTR_DS_ACTIVE && mbSelect == FALSE))
	{

		if (iDrawStyle == GTR_DS_NORMAL)
		{
			SetROP2(hDestDC, R2_COPYPEN);
			SetBkMode(hDestDC, miBkMode);
			SetBkColor(hDestDC, mBackColor);

			if (mbLastSelected)
			{
				if (m_iInspectionType == INSPECTION_TYPE_INSPECTION ||
					m_iInspectionType == INSPECTION_TYPE_AI_INSPECTION)
					hPen = CreatePen(miLineStyle, miLineThickness, RGB(255, 0, 0));
				else
					hPen = CreatePen(miLineStyle, miLineThickness, mLineColor);
			}
			else
				hPen = CreatePen(miLineStyle, miLineThickness, mLineColor);

			hOldPen = (HPEN)SelectObject(hDestDC, hPen);
			if (miRegionStyle == GTR_RS_NULL || bDrawControlPoint)
				hBrush = (HBRUSH)GetStockObject(NULL_BRUSH);
			else
				hBrush = CreateSolidBrush(mRegionColor);
			hOldBrush = (HBRUSH)SelectObject(hDestDC, hBrush);
		}
		else if (iDrawStyle == GTR_DS_ACTIVE)
		{
			SetROP2(hDestDC, R2_XORPEN);
			hPen = CreatePen(PS_DOT, 1, RGB(0, 0, 0));
			hOldPen = (HPEN)SelectObject(hDestDC, hPen);
			hBrush = (HBRUSH)GetStockObject(NULL_BRUSH);
			hOldBrush = (HBRUSH)SelectObject(hDestDC, hBrush);
		}
		else if (iDrawStyle == GTR_DS_SELECTPART)
		{
			SetROP2(hDestDC, R2_XORPEN);
			hPen = CreatePen(miLineStyle, miLineThickness, mLineColor);
			hOldPen = (HPEN)SelectObject(hDestDC, hPen);
			hBrush = (HBRUSH)GetStockObject(NULL_BRUSH);
			hOldBrush = (HBRUSH)SelectObject(hDestDC, hBrush);
		}

		GetLTPoint(&LTPoint, pCalDataService);
		GetRBPoint(&RBPoint, pCalDataService);

		aPoints[0].x = LTPoint.x;
		aPoints[0].y = LTPoint.y;
		aPoints[1].x = RBPoint.x;
		aPoints[1].y = RBPoint.y;

		pVManager->IPtoVP(aPoints, 2);

		int iRectWidth, iRectHeight;

		if (mbShape)
		{ // Circle
			iRectWidth = aPoints[1].x - aPoints[0].x + 1;
			iRectHeight = aPoints[1].y - aPoints[0].y + 1;
			if (iRectWidth >= iRectHeight)
			{
				aPoints[1].x = aPoints[0].x + iRectWidth;
				aPoints[1].y = aPoints[0].y + iRectWidth;
			}
			else
			{
				aPoints[1].x = aPoints[0].x + iRectHeight;
				aPoints[1].y = aPoints[0].y + iRectHeight;
			}

			if ((aPoints[0].x == aPoints[1].x) || (aPoints[0].y == aPoints[1].y))
				Ellipse(hDestDC, aPoints[0].x, aPoints[0].y, aPoints[1].x + 1, aPoints[1].y + 1);
			else
				Ellipse(hDestDC, aPoints[0].x, aPoints[0].y, aPoints[1].x, aPoints[1].y);
		}
		else
		{
			if ((aPoints[0].x == aPoints[1].x) || (aPoints[0].y == aPoints[1].y))
				Rectangle(hDestDC, aPoints[0].x, aPoints[0].y, aPoints[1].x + 1, aPoints[1].y + 1);
			else
				Rectangle(hDestDC, aPoints[0].x, aPoints[0].y, aPoints[1].x, aPoints[1].y);
		}

		if (iDrawStyle == GTR_DS_NORMAL && bDrawControlPoint)
		{
			if (mbSelect)
			{
				SelectObject(hDestDC, hOldPen);
				DeleteObject(hPen);
				SelectObject(hDestDC, hOldBrush);
				DeleteObject(hBrush);

				hPen = CreatePen(PS_SOLID, 1, mLineColor);
				hOldPen = (HPEN)SelectObject(hDestDC, hPen);
				hBrush = CreateSolidBrush(RGB(0, 0, 0));
				hOldBrush = (HBRUSH)SelectObject(hDestDC, hBrush);
				DrawControlPoint(hDestDC, aPoints[0].x, aPoints[0].y);
				DrawControlPoint(hDestDC, aPoints[0].x + (aPoints[1].x - aPoints[0].x) / 2, aPoints[0].y);
				DrawControlPoint(hDestDC, aPoints[1].x, aPoints[0].y);
				DrawControlPoint(hDestDC, aPoints[1].x, aPoints[0].y + (aPoints[1].y - aPoints[0].y) / 2);
				DrawControlPoint(hDestDC, aPoints[1].x, aPoints[1].y);
				DrawControlPoint(hDestDC, aPoints[0].x + (aPoints[1].x - aPoints[0].x) / 2, aPoints[1].y);
				DrawControlPoint(hDestDC, aPoints[0].x, aPoints[1].y);
				DrawControlPoint(hDestDC, aPoints[0].x, aPoints[0].y + (aPoints[1].y - aPoints[0].y) / 2);
			}
		}

		SelectObject(hDestDC, hOldPen);
		DeleteObject(hPen);
		SelectObject(hDestDC, hOldBrush);
		DeleteObject(hBrush);

		SetROP2(hDestDC, R2_COPYPEN);

		if (iDrawStyle == GTR_DS_NORMAL && bDrawControlPoint == FALSE)
		{
			if (m_iInspectionType == INSPECTION_TYPE_INSPECTION ||
				m_iInspectionType == INSPECTION_TYPE_AI_INSPECTION)
			{
				POINT CenterVPoint;
				GetCenterPoint(&CenterVPoint, pCalDataService);

				pVManager->IPtoVP(&CenterVPoint, 1);
			}
			else if (m_iInspectionType == INSPECTION_TYPE_LOCAL_ALIGN)
			{
				POINT CenterVPoint;
				GetCenterPoint(&CenterVPoint, pCalDataService);

				pVManager->IPtoVP(&CenterVPoint, 1);

				CString sInspectionTypeName;
				sInspectionTypeName.Format("%d", m_iLocalAlignID);

				SetTextColor(hDestDC, RGB(255, 0, 255));
				TextOut(hDestDC, CenterVPoint.x, CenterVPoint.y, (LPCTSTR)sInspectionTypeName.GetBuffer(0), strlen((LPCTSTR)sInspectionTypeName));
			}
			else if (m_iInspectionType == INSPECTION_TYPE_FAI_OUTER_CIRCLE)
			{
				POINT CenterVPoint;
				GetCenterPoint(&CenterVPoint, pCalDataService);

				pVManager->IPtoVP(&CenterVPoint, 1);

				CString sInspectionTypeName;
				sInspectionTypeName.Format("FAI Outer Circle %d", m_iFAIOuterCircleID);

				SetTextColor(hDestDC, RGB(255, 165, 0));
				TextOut(hDestDC, CenterVPoint.x, CenterVPoint.y, (LPCTSTR)sInspectionTypeName.GetBuffer(0), strlen((LPCTSTR)sInspectionTypeName));
			}
			else if (m_iInspectionType == INSPECTION_TYPE_FAI_INNER_CIRCLE)
			{
				POINT CenterVPoint;
				GetCenterPoint(&CenterVPoint, pCalDataService);

				pVManager->IPtoVP(&CenterVPoint, 1);

				CString sInspectionTypeName;
				sInspectionTypeName.Format("FAI Inner Circle %d", m_iFAIInnerCircleID);

				SetTextColor(hDestDC, RGB(255, 165, 0));
				TextOut(hDestDC, CenterVPoint.x, CenterVPoint.y, (LPCTSTR)sInspectionTypeName.GetBuffer(0), strlen((LPCTSTR)sInspectionTypeName));
			}
			else if (m_iInspectionType == INSPECTION_TYPE_FAI_DATUM_CIRCLE)
			{
				POINT CenterVPoint;
				GetCenterPoint(&CenterVPoint, pCalDataService);

				pVManager->IPtoVP(&CenterVPoint, 1);

				CString sInspectionTypeName;
				sInspectionTypeName.Format("FAI Datum Circle %d", m_iFAIDatumCircleID);

				SetTextColor(hDestDC, RGB(255, 165, 0));
				TextOut(hDestDC, CenterVPoint.x, CenterVPoint.y, (LPCTSTR)sInspectionTypeName.GetBuffer(0), strlen((LPCTSTR)sInspectionTypeName));
			}
			else if (m_iInspectionType == INSPECTION_TYPE_FAI_DATUM_ELLIPSE)
			{
				POINT CenterVPoint;
				GetCenterPoint(&CenterVPoint, pCalDataService);

				pVManager->IPtoVP(&CenterVPoint, 1);

				CString sInspectionTypeName;
				sInspectionTypeName.Format("FAI Datum Ellipse %d", m_iFAIDatumEllipseID);

				SetTextColor(hDestDC, RGB(255, 165, 0));
				TextOut(hDestDC, CenterVPoint.x, CenterVPoint.y, (LPCTSTR)sInspectionTypeName.GetBuffer(0), strlen((LPCTSTR)sInspectionTypeName));
			}
			else if (m_iInspectionType == INSPECTION_TYPE_FAI_WELDING_SPOT)
			{
				POINT CenterVPoint;
				GetCenterPoint(&CenterVPoint, pCalDataService);

				pVManager->IPtoVP(&CenterVPoint, 1);

				CString sInspectionTypeName;
				sInspectionTypeName.Format("FAI Welding Spot %d", m_iFAIWeldingSpotID);

				SetTextColor(hDestDC, RGB(255, 165, 0));
				TextOut(hDestDC, CenterVPoint.x, CenterVPoint.y, (LPCTSTR)sInspectionTypeName.GetBuffer(0), strlen((LPCTSTR)sInspectionTypeName));
			}
			else if (m_iInspectionType == INSPECTION_TYPE_FAI_WELDING_POCKET)
			{
				POINT CenterVPoint;
				GetCenterPoint(&CenterVPoint, pCalDataService);

				pVManager->IPtoVP(&CenterVPoint, 1);

				CString sInspectionTypeName;
				sInspectionTypeName.Format("FAI Welding Pocket %d", m_iFAIWeldingPocketID);

				SetTextColor(hDestDC, RGB(255, 165, 0));
				TextOut(hDestDC, CenterVPoint.x, CenterVPoint.y, (LPCTSTR)sInspectionTypeName.GetBuffer(0), strlen((LPCTSTR)sInspectionTypeName));
			}
			else if (m_iInspectionType == INSPECTION_TYPE_FAI_BRACKET)
			{
				POINT CenterVPoint;
				GetCenterPoint(&CenterVPoint, pCalDataService);

				pVManager->IPtoVP(&CenterVPoint, 1);

				CString sInspectionTypeName;
				sInspectionTypeName.Format("FAI Bracket %d", m_iFAIBracketID);

				SetTextColor(hDestDC, RGB(255, 165, 0));
				TextOut(hDestDC, CenterVPoint.x, CenterVPoint.y, (LPCTSTR)sInspectionTypeName.GetBuffer(0), strlen((LPCTSTR)sInspectionTypeName));
			}
		}
	}

	// Child Link
	for (int i = 0; i < GetChildTRegionCount(); i++)
	{
		GTRegion *pChildTRegion = GetChildTRegion(i);
		if (pChildTRegion == NULL)
			continue;

		pChildTRegion->Draw(hDestDC, pVManager, iDrawStyle, FALSE, pCalDataService, iCurTeachingTabIndex);
	}
}

void GTRegion::SetOrgLTPointM(DPOINT MPoint)
{
	mOrgLTPointM.x = MPoint.x;
	mOrgLTPointM.y = MPoint.y;
}

void GTRegion::SetOrgRBPointM(DPOINT MPoint)
{
	mOrgRBPointM.x = MPoint.x;
	mOrgRBPointM.y = MPoint.y;
}

Hobject GTRegion::GetROIHRegion(CCalDataService *pCalDataService)
{
	try
	{
		Hobject HROIRgn;
		gen_empty_obj(&HROIRgn);

		if (m_bRegionROI)
		{
			HROIRgn = m_HTeachPolygonRgn;
		}
		else
		{
			POINT LTPoint, RBPoint;

			GetLTPoint(&LTPoint, pCalDataService);
			GetRBPoint(&RBPoint, pCalDataService);

			if (mbShape)
				gen_circle(&HROIRgn, (LTPoint.y + RBPoint.y) / 2, (LTPoint.x + RBPoint.x) / 2, (RBPoint.y - LTPoint.y) / 2);
			else
				gen_rectangle1(&HROIRgn, LTPoint.y, LTPoint.x, RBPoint.y, RBPoint.x);
		}

		return HROIRgn;
	}
	catch (HException &except)
	{
		CString str;
		str.Format("Halcon Exception [GTRegion::GetROIHRegion] : %s", except.message);
		THEAPP.SaveLog(str);

		Hobject HROIRgn;
		gen_empty_obj(&HROIRgn);
		return HROIRgn;
	}
}

void GTRegion::UpdatePolygonROIBoundary()
{
	POINT LTPoint, RBPoint;
	Hlong lRow1, lRow2, lCol1, lCol2;

	if (m_bRegionROI)
	{
		if (THEAPP.m_pGFunction->ValidHRegion(m_HTeachPolygonRgn))
		{
			smallest_rectangle1(m_HTeachPolygonRgn, &lRow1, &lCol1, &lRow2, &lCol2);
			LTPoint.x = lCol1;
			LTPoint.y = lRow1;
			RBPoint.x = lCol2;
			RBPoint.y = lRow2;

			SetLTPoint(LTPoint, THEAPP.m_pCalDataService);
			SetRBPoint(RBPoint, THEAPP.m_pCalDataService);
		}
	}
}

// 24.03.02 Local Align 추가 - LeeGW Start
void GTRegion::ResetLocalAlignResult(int iThreadIdx)
{
	gen_empty_obj(&m_HLocalAlignShapeRgn[iThreadIdx]);
	m_iLocalAlignDeltaX[iThreadIdx] = INVALID_ALIGN_RESULT;
	m_iLocalAlignDeltaY[iThreadIdx] = INVALID_ALIGN_RESULT;
	m_dLocalAlignDeltaAngle[iThreadIdx] = INVALID_ALIGN_RESULT;
	m_dLocalAlignDeltaAngleFixedPointX[iThreadIdx] = INVALID_ALIGN_RESULT;
	m_dLocalAlignDeltaAngleFixedPointY[iThreadIdx] = INVALID_ALIGN_RESULT;
	m_dLocalAlignDeltaAngleFixedPointX[iThreadIdx] = INVALID_ALIGN_RESULT;
	m_dLocalAlignDeltaAngleFixedPointY[iThreadIdx] = INVALID_ALIGN_RESULT;
	m_iLocalAlignLineFitXPos[iThreadIdx] = INVALID_ALIGN_RESULT;
	m_iLocalAlignLineFitYPos[iThreadIdx] = INVALID_ALIGN_RESULT;
	m_iLocalAlignLineFitXPos2[iThreadIdx] = INVALID_ALIGN_RESULT;
	m_iLocalAlignLineFitYPos2[iThreadIdx] = INVALID_ALIGN_RESULT;
	m_dLocalAlignAngleFitXPos[iThreadIdx] = INVALID_ALIGN_RESULT;
	m_dLocalAlignAngleFitYPos[iThreadIdx] = INVALID_ALIGN_RESULT;
	m_dLocalAlignAngleFitAngle[iThreadIdx] = INVALID_ALIGN_RESULT;

	m_dEdgeCenterX[iThreadIdx] = INVALID_ALIGN_RESULT;
	m_dEdgeCenterY[iThreadIdx] = INVALID_ALIGN_RESULT;
	m_dLineStartX[iThreadIdx] = INVALID_ALIGN_RESULT;
	m_dLineStartY[iThreadIdx] = INVALID_ALIGN_RESULT;
	m_dLineEndX[iThreadIdx] = INVALID_ALIGN_RESULT;
	m_dLineEndY[iThreadIdx] = INVALID_ALIGN_RESULT;

	m_dEdgeStrength[iThreadIdx] = INVALID_ALIGN_RESULT;
}
// 24.03.02 Local Align 추가 - LeeGW End