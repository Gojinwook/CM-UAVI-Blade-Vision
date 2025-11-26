// BarcodeDlg.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "uScan.h"
#include "BarcodeDlg.h"
#include "afxdialogex.h"

// CBarcodeDlg 대화 상자입니다.

IMPLEMENT_DYNAMIC(CBarcodeDlg, CDialog)

CBarcodeDlg::CBarcodeDlg(CWnd *pParent /*=NULL*/)
	: CDialog(CBarcodeDlg::IDD, pParent)
{
	m_HImage.Reset();
	m_HBarcodeDetectRgn.Reset();
	m_HROIRgn.Reset();

	m_bOnOff = FALSE;
	m_bLive = FALSE;

	m_iLightValue = 500;
	m_iEditBarcodeLT_X = 0;
	m_iEditBarcodeLT_Y = 0;
	m_iEditBarcodeRB_X = 0;
	m_iEditBarcodeRB_Y = 0;

	m_iRadioDispCamera = 0;
}

CBarcodeDlg::~CBarcodeDlg()
{
}

void CBarcodeDlg::Reset()
{
	m_iScanImageWidth = PGCAM_IMAGE_WIDTH;
	m_iScanImageHeight = PGCAM_IMAGE_HEIGHT;
}

void CBarcodeDlg::SetViewportManager()
{

	mViewportManager.miImageWidth = m_iScanImageWidth;
	mViewportManager.miImageHeight = m_iScanImageHeight;

	mViewportManager.miStartXPos = 0;
	mViewportManager.miStartYPos = 0;
	mViewportManager.miViewWidth = m_ViewRect.Width();
	mViewportManager.miViewHeight = m_ViewRect.Height();

	if (m_iScanImageWidth >= m_iScanImageHeight)
		m_dFitZoomRatio = (double)m_ViewRect.Width() / mViewportManager.miImageWidth;
	else
		m_dFitZoomRatio = (double)m_ViewRect.Height() / mViewportManager.miImageHeight;

	mViewportManager.mdZoomRatio = m_dFitZoomRatio;
}

void CBarcodeDlg::UpdateViewportManager()
{
	POINT WHPoint;

	mViewportManager.GetImageWHPoint(&WHPoint);

	if (mViewportManager.miStartXPos + WHPoint.x > mViewportManager.miImageWidth)
		mViewportManager.miStartXPos = mViewportManager.miImageWidth - WHPoint.x;
	if (mViewportManager.miStartYPos + WHPoint.y > mViewportManager.miImageHeight)
		mViewportManager.miStartYPos = mViewportManager.miImageHeight - WHPoint.y;

	if (mViewportManager.miStartXPos < 0)
		mViewportManager.miStartXPos = 0;
	if (mViewportManager.miStartYPos < 0)
		mViewportManager.miStartYPos = 0;

	InvalidateRect(m_ViewRect, TRUE);
}

void CBarcodeDlg::DoDataExchange(CDataExchange *pDX)
{
	CDialog::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_IMAGE_PROCESSING_PARAM_DLG_STATIC_IMAGEVIEW, m_NoiseControlView);
	DDX_SliderButtonCtrl(pDX, IDC_EDIT_BARCODE_LIGHT, m_sLightValue, 0);
	DDX_Text(pDX, IDC_EDIT_BARCODE_LT_X, m_iEditBarcodeLT_X);
	DDX_Text(pDX, IDC_EDIT_BARCODE_LT_Y, m_iEditBarcodeLT_Y);
	DDX_Text(pDX, IDC_EDIT_BARCODE_RB_X, m_iEditBarcodeRB_X);
	DDX_Text(pDX, IDC_EDIT_BARCODE_RB_Y, m_iEditBarcodeRB_Y);

	DDX_Radio(pDX, IDC_RADIO_CAMERA_1, m_iRadioDispCamera);
}

BEGIN_MESSAGE_MAP(CBarcodeDlg, CDialog)
ON_WM_DESTROY()
ON_WM_LBUTTONDOWN()
ON_WM_MOUSEMOVE()
ON_WM_LBUTTONUP()
ON_WM_CONTEXTMENU()
ON_WM_PAINT()
ON_EN_CHANGE(IDC_EDIT_BARCODE_LIGHT, OnChangeEditLightValue)
ON_WM_CLOSE()
ON_WM_MOUSEWHEEL()
ON_BN_CLICKED(IDC_CHECK_LIVE, &CBarcodeDlg::OnBnClickedCheckLive)
ON_WM_TIMER()
ON_BN_CLICKED(IDC_BUTTON_CHANGE_LIGHTVALUE, &CBarcodeDlg::OnBnClickedButtonChangeLightvalue)
ON_BN_CLICKED(IDC_GRAB, &CBarcodeDlg::OnBnClickedGrab)
ON_BN_CLICKED(IDOK, &CBarcodeDlg::OnBnClickedOk)
ON_BN_CLICKED(IDC_BUTTON_BARCODE_TEST, &CBarcodeDlg::OnBnClickedButtonBarcodeTest)
ON_BN_CLICKED(IDC_BUTTON_BARCODE_SHOW_ROI, &CBarcodeDlg::OnBnClickedButtonBarcodeShowRoi)
ON_BN_CLICKED(IDC_BARCODE_SINGLE_IMAGE_LOAD, &CBarcodeDlg::OnBnClickedBarcodeSingleImageLoad)
ON_BN_CLICKED(IDC_RADIO_CAMERA_1, &CBarcodeDlg::OnBnClickedRadioCamera1)
ON_BN_CLICKED(IDC_RADIO_CAMERA_2, &CBarcodeDlg::OnBnClickedRadioCamera2)
END_MESSAGE_MAP()

void CBarcodeDlg::OnChangeEditLightValue()
{
	m_iLightValue = m_sLightValue.GetPos();
}

// CBarcodeDlg 메시지 처리기입니다.

BOOL CBarcodeDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.

	SetWindowPos(&wndTopMost, 0, 0, 0, 0, SWP_NOSIZE);

	Reset();

	m_NoiseControlView.CreateHWindow();
	m_NoiseControlView.GetWindowRect(&m_ViewRect);
	this->ScreenToClient(&m_ViewRect);

	m_ViewOffset.x = m_ViewRect.left;
	m_ViewOffset.y = m_ViewRect.top;

	SetViewportManager();

	m_sLightValue.SetRange(0, 999);
	m_sLightValue.SetPos(m_iLightValue);

	m_iEditBarcodeLT_X = THEAPP.m_pModelDataManager->m_iBarcodeLTPointX;
	m_iEditBarcodeLT_Y = THEAPP.m_pModelDataManager->m_iBarcodeLTPointY;
	m_iEditBarcodeRB_X = THEAPP.m_pModelDataManager->m_iBarcodeRBPointX;
	m_iEditBarcodeRB_Y = THEAPP.m_pModelDataManager->m_iBarcodeRBPointY;

	UpdateData(FALSE);

	gen_rectangle1(&m_HROIRgn, m_iEditBarcodeLT_Y, m_iEditBarcodeLT_X, m_iEditBarcodeRB_Y, m_iEditBarcodeRB_X);

	return TRUE; // return TRUE unless you set the focus to a control
				 // 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

BOOL CBarcodeDlg::PreTranslateMessage(MSG *pMsg)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.

	if (pMsg->message == WM_KEYDOWN && (pMsg->wParam == VK_RETURN || pMsg->wParam == VK_ESCAPE) || (pMsg->message == WM_SYSKEYDOWN && pMsg->wParam == VK_F4))
		return TRUE;

	return CDialog::PreTranslateMessage(pMsg);
}

void CBarcodeDlg::OnDestroy()
{
	CDialog::OnDestroy();

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.

	m_NoiseControlView.DestroyHWindow();
}

void CBarcodeDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.

	if (!m_ViewRect.PtInRect(point))
	{
		CDialog::OnLButtonDown(nFlags, point); //?
		return;
	}

	CPoint ClientViewPoint1 = point;

	ClientToScreen(&ClientViewPoint1);
	m_NoiseControlView.ScreenToClient(&ClientViewPoint1);

	POINT IPoint = ClientViewPoint1;
	maMVPoints[0] = point;
	mViewportManager.VPtoIP(&IPoint, 1);
	maMIPoints[0] = IPoint;

	SetCapture();

	//	InvalidateRect(m_ViewRect,FALSE);

	CDialog::OnLButtonDown(nFlags, point);
}

void CBarcodeDlg::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.

	if (!m_ViewRect.PtInRect(point))
	{
		CDialog::OnMouseMove(nFlags, point); //?
		return;
	}

	CPoint viewPoint1 = point;

	ClientToScreen(&viewPoint1);
	m_NoiseControlView.ScreenToClient(&viewPoint1);

	POINT IPoint = viewPoint1;
	maMVPoints[1] = point;
	mViewportManager.VPtoIP(&IPoint, 1);
	maMIPoints[1] = IPoint;

	double dX, dY;

	if (IS_SETFLAG(nFlags, MK_LBUTTON))
	{
		dX = ((maMVPoints[0].x - maMVPoints[1].x) / mViewportManager.mdZoomRatio);

		if (fabs(dX) >= 1.0)
		{
			dY = ((maMVPoints[0].y - maMVPoints[1].y) / mViewportManager.mdZoomRatio);

			mViewportManager.miStartXPos += (int)(dX);
			mViewportManager.miStartYPos += (int)(dY);

			UpdateViewportManager();

			maMVPoints[0] = maMVPoints[1];
			maMIPoints[0] = maMIPoints[1];
		}
	}
	else
	{
		CGFunction *pGFun = THEAPP.m_pGFunction;

		if (pGFun->ValidHImage(m_HImage) && IPoint.x >= 0 && IPoint.y >= 0 && IPoint.x < m_iScanImageWidth && IPoint.y < m_iScanImageHeight)
		{
			CString str;
			double dGrayValue;

			get_grayval(m_HImage, IPoint.y, IPoint.x, &dGrayValue);
			str.Format("바코드 티칭 [(%d, %d)=%d]", IPoint.x, IPoint.y, (int)dGrayValue);
			SetWindowText(str);
		}
	}

	CDialog::OnMouseMove(nFlags, point);
}

void CBarcodeDlg::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.

	ReleaseCapture();

	if (!m_ViewRect.PtInRect(point))
	{
		CDialog::OnLButtonUp(nFlags, point); //?
		return;
	}

	CDialog::OnLButtonUp(nFlags, point);
}

void CBarcodeDlg::OnContextMenu(CWnd * /*pWnd*/, CPoint /*point*/)
{
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
}

void CBarcodeDlg::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	// 그리기 메시지에 대해서는 CDialog::OnPaint()을(를) 호출하지 마십시오.

	POINT WHPoint;
	mViewportManager.GetImageWHPoint(&WHPoint);

	if (THEAPP.m_pGFunction->ValidHImage(m_HImage))
	{
		set_part(m_NoiseControlView.mlWindowHandle, mViewportManager.miStartYPos, mViewportManager.miStartXPos, mViewportManager.miStartYPos + WHPoint.y, mViewportManager.miStartXPos + WHPoint.x);
		set_window_extents(m_NoiseControlView.mlWindowHandle, 0, 0, mViewportManager.miViewWidth, mViewportManager.miViewHeight);
		disp_image(m_HImage, m_NoiseControlView.mlWindowHandle);

		if (THEAPP.m_pGFunction->ValidHRegion(m_HBarcodeDetectRgn))
		{
			set_color(m_NoiseControlView.mlWindowHandle, "red");
			set_draw(m_NoiseControlView.mlWindowHandle, "margin");
			disp_obj(m_HBarcodeDetectRgn, m_NoiseControlView.mlWindowHandle);
		}

		if (THEAPP.m_pGFunction->ValidHRegion(m_HROIRgn))
		{
			set_color(m_NoiseControlView.mlWindowHandle, "green");
			set_draw(m_NoiseControlView.mlWindowHandle, "margin");
			disp_obj(m_HROIRgn, m_NoiseControlView.mlWindowHandle);
		}
	}

	CDC *pDC = GetDC();

	CRgn ClipRgn;
	ClipRgn.CreateRectRgn(0, 0, mViewportManager.miViewWidth, mViewportManager.miViewHeight);
	pDC->SelectClipRgn(&ClipRgn);

	ReleaseDC(pDC);
}

void CBarcodeDlg::OnClose()
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.

	m_HImage.Reset();

	CDialog::OnClose();
}

void CBarcodeDlg::ZoomIn()
{
	mViewportManager.mdZoomRatio = mViewportManager.mdZoomRatio + 0.05;
	if (mViewportManager.mdZoomRatio > 5)
		mViewportManager.mdZoomRatio = 5.0;

	UpdateViewportManager();
}

void CBarcodeDlg::ZoomOut()
{
	mViewportManager.mdZoomRatio = mViewportManager.mdZoomRatio - 0.05;

	if (mViewportManager.mdZoomRatio < m_dFitZoomRatio)
		mViewportManager.mdZoomRatio = m_dFitZoomRatio;

	UpdateViewportManager();
}

BOOL CBarcodeDlg::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.

	ScreenToClient(&pt);

	if (!m_ViewRect.PtInRect(pt))
	{
		return CDialog::OnMouseWheel(nFlags, zDelta, pt);
	}

	if (zDelta <= 0)
	{
		ZoomIn();
	}
	else
	{
		ZoomOut();
	}

	return CDialog::OnMouseWheel(nFlags, zDelta, pt);
}

void CBarcodeDlg::OnBnClickedCheckLive()
{
	if (m_bLive == FALSE)
	{
		m_bLive = TRUE;
		SetTimer(1, 100, NULL);
	}
	else
	{
		m_bLive = FALSE;
		KillTimer(1);
	}
}

void CBarcodeDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.

	if (nIDEvent == 1)
	{
		if (m_bLive == TRUE)
		{
			if (m_bOnOff == TRUE)
			{
				THEAPP.m_pCameraManager->GrabBarcodeImage(m_iRadioDispCamera, &m_HImage);
				InvalidateRect(m_ViewRect, FALSE);
				m_bOnOff = FALSE;
			}
			else
			{
				m_bOnOff = TRUE;
			}
		}
	}

	CDialog::OnTimer(nIDEvent);
}

void CBarcodeDlg::OnBnClickedButtonChangeLightvalue()
{
	if (m_iLightValue < 0)
		m_iLightValue = 0;
	if (m_iLightValue > LIGHT_BRIGHT_MAX)
		m_iLightValue = LIGHT_BRIGHT_MAX;

#ifdef INLINE_MODE
	// New Light Controller

	if (THEAPP.m_iMachineInspType == MACHINE_WELDING)
	{
		int iPageIdx = WELDING_CAM_BARCODE_PAGE_IDX;

		THEAPP.m_pModelDataManager->m_iBarcodeLightBright = m_iLightValue;

		// Light 1
		m_LightControl.m_Page[iPageIdx].uiChannel1 = 0;
		m_LightControl.m_Page[iPageIdx].uiChannel2 = 0;
		m_LightControl.m_Page[iPageIdx].uiChannel3 = 0;
		m_LightControl.m_Page[iPageIdx].uiChannel4 = 0;
		m_LightControl.m_Page[iPageIdx].uiChannel5 = m_iLightValue;
		m_LightControl.m_Page[iPageIdx].uiChannel6 = 0;
		if (m_LightControl.m_ComPort.OpenPort(1, 19200)) // Com1
		{
			m_LightControl.SetIllumination_6CH(iPageIdx);
			m_LightControl.m_ComPort.ClosePort();
		}
	}
	else
	{
		int iPageIdx = 0;

		THEAPP.m_pModelDataManager->m_iBarcodeLightBright = m_iLightValue;

		// Light 1
		m_LightControl.m_Page[iPageIdx].uiChannel1 = m_iLightValue;
		m_LightControl.m_Page[iPageIdx].uiChannel2 = 0;
		m_LightControl.m_Page[iPageIdx].uiChannel3 = 0;
		m_LightControl.m_Page[iPageIdx].uiChannel4 = 0;
		if (m_LightControl.m_ComPort.OpenPort(4, 19200)) // Com4
		{
			m_LightControl.SetIllumination(iPageIdx);
			m_LightControl.m_ComPort.ClosePort();
		}
	}

#endif

	//**********  조명 값을 파일에 써줌.
	CString strModelFolder;										 // Ver2629
	if (!THEAPP.Struct_PreferenceStruct.m_bUseAbsolutePathModel) // Ver2629
		CString strModelFolder = THEAPP.GetWorkingDirectory() + "\\Model\\" + THEAPP.m_pModelDataManager->m_sModelName;
	else
		CString strModelFolder = THEAPP.GetWorkingEvmsDirectory() + "\\Model\\" + THEAPP.m_pModelDataManager->m_sModelName;

	THEAPP.m_pModelDataManager->SaveLightInfo(strModelFolder + "\\HW\\InspectLightInfo.ini");
}

void CBarcodeDlg::OnBnClickedGrab()
{
	try
	{
#ifdef INLINE_MODE

		if (THEAPP.m_pModelDataManager->m_sModelName == ".")
			return;

		if (m_bLive)
		{
			CButton *pButton;
			pButton = (CButton *)GetDlgItem(IDC_CHECK_LIVE);
			pButton->SetCheck(FALSE);

			m_bLive = FALSE;
			KillTimer(1);
			Sleep(500);
		}

		THEAPP.m_pCameraManager->GrabBarcodeImage(m_iRadioDispCamera, &m_HImage);
		InvalidateRect(m_ViewRect, FALSE);

		CString FolderName;
		if (!THEAPP.Struct_PreferenceStruct.m_bUseAbsolutePathModel) // Ver2629
			FolderName = THEAPP.GetWorkingDirectory() + "\\Model\\" + THEAPP.m_pModelDataManager->m_sModelName + "\\SW\\TeachImage\\";
		else
			FolderName = THEAPP.GetWorkingEvmsDirectory() + "\\Model\\" + THEAPP.m_pModelDataManager->m_sModelName + "\\SW\\TeachImage\\";

		THEAPP.m_FileBase.CreatePath(FolderName, TRUE); // Ver2629

		CString FileName;

		if (m_iRadioDispCamera==0)
			FileName = FolderName + "TeachingBarcode_Cam1";
		else
			FileName = FolderName + "TeachingBarcode_Cam2";

		write_image(m_HImage, "bmp", 0, FileName);
#else // Barcode Rotation Inspection Add Start (#else)
		PString PathName;
		CString strFileName;
		CString csTmp, csData;

		char szFilter[] = "All Files (*.*)|*.*|Bitmap Files (*.bmp)|*.bmp|JPEG Files (*.jpg)|*.jpg|IMA Files (*.ima)|*.ima|";

		CFileDialog FileDialog(TRUE, NULL, NULL, OFN_HIDEREADONLY, szFilter, NULL);
		FileDialog.m_ofn.lpstrTitle = _T("Load Image File");
		FileDialog.m_ofn.lpstrInitialDir = _T("");

		if (FileDialog.DoModal() == IDOK)
		{
			strFileName = FileDialog.GetFileName();
			PathName = (LPCTSTR)FileDialog.GetPathName();
			int nFindSpot = strFileName.Find(".");
			if (nFindSpot == -1)
				return;
			CString strFileExt = strFileName.Right(3); // 확장자를 얻어온다.
			strFileExt.MakeUpper();					   // 대문자를 만들고..
			read_image(&m_HImage, (LPCTSTR)FileDialog.GetPathName());

			CString FolderName;
			if (!THEAPP.Struct_PreferenceStruct.m_bUseAbsolutePathModel) // Ver2629
				FolderName = THEAPP.GetWorkingDirectory() + "\\Model\\" + THEAPP.m_pModelDataManager->m_sModelName + "\\SW\\TeachImage\\";
			else
				FolderName = THEAPP.GetWorkingEvmsDirectory() + "\\Model\\" + THEAPP.m_pModelDataManager->m_sModelName + "\\SW\\TeachImage\\";

			THEAPP.m_FileBase.CreatePath(FolderName, TRUE); // Ver2629

			CString FileName;
			if (m_iRadioDispCamera == 0)
				FileName = FolderName + "TeachingBarcode_Cam1";
			else
				FileName = FolderName + "TeachingBarcode_Cam2";
			write_image(m_HImage, "bmp", 0, FileName);
			InvalidateRect(m_ViewRect, FALSE);
		}
		// Barcode Rotation Inspection Add End (#else)

#endif

		return;
	}
	catch (HException &except)
	{
		CString str;
		str.Format("Halcon Exception [CBarcodeDlg::OnBnClickedGrab] : %s", except.message);
		THEAPP.SaveLog(str);
		return;
	}
}

void CBarcodeDlg::OnBnClickedOk()
{
	CDialog::OnOK();
}

void CBarcodeDlg::OnBnClickedButtonBarcodeTest()
{
	if (THEAPP.m_pGFunction->ValidHImage(m_HImage) == FALSE)
		return;

	Hobject HBarcodeImage;
	HTuple DataCodeHandle, ResultHandles, DecodedDataStrings;
	Hobject SymbolXLDs;

	double StartTime, EndTime;
	CString strLog;

	BOOL bFindBarcodeData = TRUE;
	try
	{
		SymbolXLDs.Reset();

		StartTime = GetTickCount();

		crop_rectangle1(m_HImage, &HBarcodeImage, THEAPP.m_pModelDataManager->m_iBarcodeLTPointY, THEAPP.m_pModelDataManager->m_iBarcodeLTPointX,
						THEAPP.m_pModelDataManager->m_iBarcodeRBPointY, THEAPP.m_pModelDataManager->m_iBarcodeRBPointX);

		create_data_code_2d_model(HTuple("Data Matrix ECC 200"), HTuple(), HTuple(), &DataCodeHandle);
		set_data_code_2d_param(DataCodeHandle, HTuple("default_parameters"), HTuple("enhanced_recognition"));
		find_data_code_2d(HBarcodeImage, &SymbolXLDs, DataCodeHandle, HTuple(), HTuple(), &ResultHandles, &DecodedDataStrings);
		clear_data_code_2d_model(DataCodeHandle);
		EndTime = GetTickCount();

		strLog.Format("티칭 바코드 검사 시간: %.0lf ms", EndTime - StartTime);
		THEAPP.SaveLog(strLog);

		if (THEAPP.m_pGFunction->ValidHXLD(SymbolXLDs))
		{
			gen_region_contour_xld(SymbolXLDs, &m_HBarcodeDetectRgn, "filled");

			move_region(m_HBarcodeDetectRgn, &m_HBarcodeDetectRgn, THEAPP.m_pModelDataManager->m_iBarcodeLTPointY, THEAPP.m_pModelDataManager->m_iBarcodeLTPointX);

			dilation_rectangle1(m_HBarcodeDetectRgn, &m_HBarcodeDetectRgn, 11, 11);
		}
	}
	catch (HException &e)
	{
		bFindBarcodeData = FALSE;
		m_HBarcodeDetectRgn.Reset();
	}

	CString szBarcodeResult;

	if (bFindBarcodeData)
	{
		if (DecodedDataStrings.Num() > 0)
		{
			szBarcodeResult = DecodedDataStrings[0].S();
		}
	}
	else
		szBarcodeResult = _T("인식 실패!!");

	GetDlgItem(IDC_STATIC_BARCODE_RESULT)->SetWindowText(szBarcodeResult);
	InvalidateRect(m_ViewRect, FALSE);
}

void CBarcodeDlg::SetImage(Hobject *pHTeachImage)
{
	copy_image(*pHTeachImage, &m_HImage);
}

void CBarcodeDlg::OnBnClickedButtonBarcodeShowRoi()
{
	UpdateData(TRUE);

	CString strModelFolder;
	if (!THEAPP.Struct_PreferenceStruct.m_bUseAbsolutePathModel) // Ver2629
		strModelFolder = THEAPP.GetWorkingDirectory() + "\\Model\\" + THEAPP.m_pModelDataManager->m_sModelName;
	else
		strModelFolder = THEAPP.GetWorkingEvmsDirectory() + "\\Model\\" + THEAPP.m_pModelDataManager->m_sModelName;

	CIniFileCS INI(strModelFolder + "\\SW\\ModelBase.ini");
	CString strSection = "Model Base";

	THEAPP.m_pModelDataManager->m_iBarcodeLTPointX = m_iEditBarcodeLT_X;
	THEAPP.m_pModelDataManager->m_iBarcodeLTPointY = m_iEditBarcodeLT_Y;
	THEAPP.m_pModelDataManager->m_iBarcodeRBPointX = m_iEditBarcodeRB_X;
	THEAPP.m_pModelDataManager->m_iBarcodeRBPointY = m_iEditBarcodeRB_Y;

	INI.Set_Integer(strSection, "m_iBarcodeLTPointX", THEAPP.m_pModelDataManager->m_iBarcodeLTPointX);
	INI.Set_Integer(strSection, "m_iBarcodeLTPointY", THEAPP.m_pModelDataManager->m_iBarcodeLTPointY);
	INI.Set_Integer(strSection, "m_iBarcodeRBPointX", THEAPP.m_pModelDataManager->m_iBarcodeRBPointX);
	INI.Set_Integer(strSection, "m_iBarcodeRBPointY", THEAPP.m_pModelDataManager->m_iBarcodeRBPointY);

	gen_rectangle1(&m_HROIRgn, m_iEditBarcodeLT_Y, m_iEditBarcodeLT_X, m_iEditBarcodeRB_Y, m_iEditBarcodeRB_X);

	InvalidateRect(m_ViewRect, FALSE);
}

void CBarcodeDlg::OnBnClickedBarcodeSingleImageLoad() // Barcode Rotation Inspection
{
	PString PathName;
	CString strFileName;
	CString csTmp, csData;

	char szFilter[] = "All Files (*.*)|*.*|Bitmap Files (*.bmp)|*.bmp|JPEG Files (*.jpg)|*.jpg|IMA Files (*.ima)|*.ima|";

	CFileDialog FileDialog(TRUE, NULL, NULL, OFN_HIDEREADONLY, szFilter, NULL);
	FileDialog.m_ofn.lpstrTitle = _T("Load Image File");
	FileDialog.m_ofn.lpstrInitialDir = _T("");

	if (FileDialog.DoModal() == IDOK)
	{
		strFileName = FileDialog.GetFileName();
		PathName = (LPCTSTR)FileDialog.GetPathName();
		int nFindSpot = strFileName.Find(".");
		if (nFindSpot == -1)
			return;
		CString strFileExt = strFileName.Right(3); // 확장자를 얻어온다.
		strFileExt.MakeUpper();					   // 대문자를 만들고..
		read_image(&m_HImage, (LPCTSTR)FileDialog.GetPathName());

		CString FolderName;
		if (!THEAPP.Struct_PreferenceStruct.m_bUseAbsolutePathModel) // Ver2629
			FolderName = THEAPP.GetWorkingDirectory() + "\\Model\\" + THEAPP.m_pModelDataManager->m_sModelName + "\\SW\\TeachImage\\";
		else
			FolderName = THEAPP.GetWorkingEvmsDirectory() + "\\Model\\" + THEAPP.m_pModelDataManager->m_sModelName + "\\SW\\TeachImage\\";

		THEAPP.m_FileBase.CreatePath(FolderName, TRUE); // Ver2629

		CString FileName;
		if (m_iRadioDispCamera == 0)
			FileName = FolderName + "TeachingBarcode_Cam1";
		else
			FileName = FolderName + "TeachingBarcode_Cam2";
		write_image(m_HImage, "bmp", 0, FileName);
		InvalidateRect(m_ViewRect, FALSE);
	}
}


void CBarcodeDlg::OnBnClickedRadioCamera1()
{
	try
	{
#ifndef INLINE_MODE
		CString FolderName;
		if (!THEAPP.Struct_PreferenceStruct.m_bUseAbsolutePathModel) // Ver2629
			FolderName = THEAPP.GetWorkingDirectory() + "\\Model\\" + THEAPP.m_pModelDataManager->m_sModelName + "\\SW\\TeachImage\\";
		else
			FolderName = THEAPP.GetWorkingEvmsDirectory() + "\\Model\\" + THEAPP.m_pModelDataManager->m_sModelName + "\\SW\\TeachImage\\";

		CString FileName;
		FileName = FolderName + "TeachingBarcode_Cam1";
		read_image(&m_HImage, (LPCTSTR)FileName);
		InvalidateRect(m_ViewRect, FALSE);
#endif
		return;
	}
	catch (HException &except)
	{
		CString str;
		str.Format("Halcon Exception [CBarcodeDlg::OnBnClickedRadioCamera1] : %s", except.message);
		THEAPP.SaveLog(str);
		return;
	}
}


void CBarcodeDlg::OnBnClickedRadioCamera2()
{
	try
	{
#ifndef INLINE_MODE
		CString FolderName;
		if (!THEAPP.Struct_PreferenceStruct.m_bUseAbsolutePathModel) // Ver2629
			FolderName = THEAPP.GetWorkingDirectory() + "\\Model\\" + THEAPP.m_pModelDataManager->m_sModelName + "\\SW\\TeachImage\\";
		else
			FolderName = THEAPP.GetWorkingEvmsDirectory() + "\\Model\\" + THEAPP.m_pModelDataManager->m_sModelName + "\\SW\\TeachImage\\";

		CString FileName;
		FileName = FolderName + "TeachingBarcode_Cam2";
		read_image(&m_HImage, (LPCTSTR)FileName);
		InvalidateRect(m_ViewRect, FALSE);
#endif
		return;
	}
	catch (HException &except)
	{
		CString str;
		str.Format("Halcon Exception [CBarcodeDlg::OnBnClickedRadioCamera2] : %s", except.message);
		THEAPP.SaveLog(str);
		return;
	}
}
