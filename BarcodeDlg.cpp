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
	m_nDxHandlerPosX_B = 165.5;
	m_iEditBarcodeLT_X = 0;
	m_iEditBarcodeLT_Y = 0;
	m_iEditBarcodeRB_X = 0;
	m_iEditBarcodeRB_Y = 0;

	// Barcode Hole Center Start
	m_bCheckUseBarcodeShiftHoleCenter = FALSE;
	m_iEditBarcodeShiftHoleCenterDilation = 100;
	m_iEditBarcodeShiftHoleCenterGvMin = 80;
	m_iEditBarcodeShiftHoleCenterGvMax = 255;
	gen_empty_obj(&m_HDispHoleRgn);
	gen_empty_obj(&m_HHoleRgn);
	gen_empty_obj(&m_HDispHoleSearchRgn);
	gen_empty_obj(&m_HDispBarcodeCenterXLD);
	gen_empty_obj(&m_HDispHoleCenterXLD);
	m_dBarcodeCenterX = -9999.0;
	m_dBarcodeCenterY = -9999.0;
	m_dHoleCenterX = -9999.0;
	m_dHoleCenterY = -9999.0;
	m_iEditBarcodeShiftHoleCenterGvClosing = 10;
	// Barcode Hole Center End
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
	DDX_Text(pDX, IDC_EDIT_CURRENT_POSITION_X, m_nDxHandlerPosX_B);
	DDX_Text(pDX, IDC_EDIT_BARCODE_LT_X, m_iEditBarcodeLT_X);
	DDX_Text(pDX, IDC_EDIT_BARCODE_LT_Y, m_iEditBarcodeLT_Y);
	DDX_Text(pDX, IDC_EDIT_BARCODE_RB_X, m_iEditBarcodeRB_X);
	DDX_Text(pDX, IDC_EDIT_BARCODE_RB_Y, m_iEditBarcodeRB_Y);

	// Barcode Hole Center Start
	DDX_Check(pDX, IDC_CHECK_USE_BARCODE_SHIFT_HOLECENTER, m_bCheckUseBarcodeShiftHoleCenter);
	DDX_Text(pDX, IDC_EDIT_BARCODE_SHIFT_HOLECENTER_DILATION, m_iEditBarcodeShiftHoleCenterDilation);
	DDX_Text(pDX, IDC_EDIT_BARCODE_SHIFT_HOLECENTER_GV_MIN, m_iEditBarcodeShiftHoleCenterGvMin);
	DDX_Text(pDX, IDC_EDIT_BARCODE_SHIFT_HOLECENTER_GV_MAX, m_iEditBarcodeShiftHoleCenterGvMax);
	DDX_Text(pDX, IDC_EDIT_BARCODE_SHIFT_HOLECENTER_GV_CLOSING, m_iEditBarcodeShiftHoleCenterGvClosing);
	// Barcode Hole Center End
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
ON_BN_CLICKED(IDC_BUTTON_MOVETO_POSITION, &CBarcodeDlg::OnBnClickedButtonMovetoPosition)
ON_BN_CLICKED(IDC_BUTTON_SET_START_POSITION, &CBarcodeDlg::OnBnClickedButtonSetStartPosition)
ON_BN_CLICKED(IDC_BUTTON_MAKE_HANDLER_POSITION_MAP, &CBarcodeDlg::OnBnClickedButtonMakeHandlerPositionMap)
ON_BN_CLICKED(IDC_BUTTON_BARCODE_TEST, &CBarcodeDlg::OnBnClickedButtonBarcodeTest)
ON_BN_CLICKED(IDC_BUTTON_BARCODE_SHOW_ROI, &CBarcodeDlg::OnBnClickedButtonBarcodeShowRoi)
ON_BN_CLICKED(IDC_BARCODE_SINGLE_IMAGE_LOAD, &CBarcodeDlg::OnBnClickedBarcodeSingleImageLoad)
ON_BN_CLICKED(IDC_BARCODE_MULTIPLE_IMAGE_LOAD_INSPECTION, &CBarcodeDlg::OnBnClickedBarcodeMultipleImageLoadInspection)
ON_BN_CLICKED(IDC_BUTTON_BARCODE_SHIFT_HOLECENTER_DISP, &CBarcodeDlg::OnBnClickedButtonBarcodeShiftHolecenterDisp)
ON_BN_CLICKED(IDC_BUTTON_BARCODE_SHIFT_HOLECENTER_NODISP, &CBarcodeDlg::OnBnClickedButtonBarcodeShiftHolecenterNodisp)
ON_BN_CLICKED(IDC_BUTTON_BARCODE_SHIFT_HOLECENTER_SAVE, &CBarcodeDlg::OnBnClickedButtonBarcodeShiftHolecenterSave)
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

	// Barcode Hole Center Start
	m_bCheckUseBarcodeShiftHoleCenter = THEAPP.m_pModelDataManager->m_bUseBarcodeShiftHoleCenter;
	m_iEditBarcodeShiftHoleCenterDilation = THEAPP.m_pModelDataManager->m_iBarcodeShiftHoleCenterDilation;
	m_iEditBarcodeShiftHoleCenterGvMin = THEAPP.m_pModelDataManager->m_iBarcodeShiftHoleCenterGvMin;
	m_iEditBarcodeShiftHoleCenterGvMax = THEAPP.m_pModelDataManager->m_iBarcodeShiftHoleCenterGvMax;
	m_iEditBarcodeShiftHoleCenterGvClosing = THEAPP.m_pModelDataManager->m_iBarcodeShiftHoleCenterGvClosing;
	// Barcode Hole Center End

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

		// Barcode Hole Center Start
		if (THEAPP.m_pGFunction->ValidHRegion(m_HDispHoleSearchRgn))
		{
			set_color(m_NoiseControlView.mlWindowHandle, "cyan");
			set_draw(m_NoiseControlView.mlWindowHandle, "margin");
			disp_obj(m_HDispHoleSearchRgn, m_NoiseControlView.mlWindowHandle);
		}

		if (THEAPP.m_pGFunction->ValidHRegion(m_HDispHoleRgn))
		{
			set_color(m_NoiseControlView.mlWindowHandle, "white");
			set_draw(m_NoiseControlView.mlWindowHandle, "margin");
			disp_obj(m_HDispHoleRgn, m_NoiseControlView.mlWindowHandle);
		}

		if (THEAPP.m_pGFunction->ValidHXLD(m_HDispBarcodeCenterXLD))
		{
			set_color(m_NoiseControlView.mlWindowHandle, "yellow");
			set_draw(m_NoiseControlView.mlWindowHandle, "margin");
			disp_obj(m_HDispBarcodeCenterXLD, m_NoiseControlView.mlWindowHandle);
		}

		if (THEAPP.m_pGFunction->ValidHXLD(m_HDispHoleCenterXLD))
		{
			set_color(m_NoiseControlView.mlWindowHandle, "blue");
			set_draw(m_NoiseControlView.mlWindowHandle, "margin");
			disp_obj(m_HDispHoleCenterXLD, m_NoiseControlView.mlWindowHandle);
		}
		// Barcode Hole Center End
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
				THEAPP.m_pCameraManager->GrabBarcodeImage(&m_HImage);
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

		THEAPP.m_pCameraManager->GrabBarcodeImage(&m_HImage);
		InvalidateRect(m_ViewRect, FALSE);

		CString FolderName;
		if (!THEAPP.Struct_PreferenceStruct.m_bUseAbsolutePathModel) // Ver2629
			FolderName = THEAPP.GetWorkingDirectory() + "\\Model\\" + THEAPP.m_pModelDataManager->m_sModelName + "\\SW\\TeachImage\\";
		else
			FolderName = THEAPP.GetWorkingEvmsDirectory() + "\\Model\\" + THEAPP.m_pModelDataManager->m_sModelName + "\\SW\\TeachImage\\";

		THEAPP.m_FileBase.CreatePath(FolderName, TRUE); // Ver2629

		CString FileName;
		FileName = FolderName + "TeachingBarcode";
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
			FileName = FolderName + "TeachingBarcode";
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

void CBarcodeDlg::OnBnClickedButtonMovetoPosition()
{
	UpdateData(TRUE);
	THEAPP.m_pHandlerService->Set_AMoveRequest(m_nDxHandlerPosX_B, 0, 1, VISION_TYPE_BARCODE);
}

void CBarcodeDlg::OnBnClickedButtonSetStartPosition()
{
	CString strXposition;

	GetDlgItemText(IDC_EDIT_CURRENT_POSITION_X, strXposition);
	SetDlgItemText(IDC_EDIT_SET_START_POSITION_X, strXposition);
}

UINT BarcodeMotionPositionTest_Thread(LPVOID lp)
{
	CBarcodeDlg *pJogSetDlg = (CBarcodeDlg *)lp;

	if (THEAPP.m_pModelDataManager->m_sModelName == ".")
		return 0;
	CString FolderName;
	if (!THEAPP.Struct_PreferenceStruct.m_bUseAbsolutePathModel) // Ver2629
		FolderName = THEAPP.GetWorkingDirectory() + "\\Model\\" + THEAPP.m_pModelDataManager->m_sModelName;
	else
		FolderName = THEAPP.GetWorkingEvmsDirectory() + "\\Model\\" + THEAPP.m_pModelDataManager->m_sModelName;

	CString FileName;
#ifdef INLINE_MODE
	FileName = FolderName + "\\HW\\BarcodeMotionMovingPosition.ini";
#endif
	CIniFileCS INI(FileName);

	CString strSection = "Start Position";
	double nStartPosX = INI.Get_Double(strSection, "X", 0.0);

	if (nStartPosX == 0)
	{
		return 0;
	}

	strSection = "Moving Position";
	CString strModuleNumber;

	int ArrayX = THEAPP.m_pModelDataManager->m_iTrayArrayX;
	if (THEAPP.Struct_PreferenceStruct.m_iPickerTrayDir == PICKER_TRAY_DIR_Y)
		ArrayX = THEAPP.m_pModelDataManager->m_iTrayArrayY;

	for (int i = 0; i < ArrayX; i++)
	{
		strModuleNumber.Format("X-%d", i);
		double AMoveX = INI.Get_Double(strSection, strModuleNumber, 0.0);
#ifdef INLINE_MODE

		THEAPP.m_pHandlerService->m_bBarcodeMoveCompleteFlag = FALSE;
		THEAPP.m_pHandlerService->Set_AMoveRequest(AMoveX, 0, 1, VISION_TYPE_BARCODE);

		while (!THEAPP.m_pHandlerService->m_bBarcodeMoveCompleteFlag) // 무브 컴플리트 기다림
		{
			if (THEAPP.m_pHandlerService->m_nInspectPCStatus >= VS_PAUSE)
			{
				return 0;
			}
			Sleep(1);
		}
		THEAPP.m_pHandlerService->Set_PositionRequest(VISION_TYPE_BARCODE);
		Sleep(200);
		THEAPP.m_pHandlerService->Set_BarcodeTriggerRequest(); // Barcode 영상
		Sleep(200);
#endif
	}
	//	}

	nStartPosX = INI.Get_Double(strSection, "X-0", 0.0);
	THEAPP.m_pHandlerService->m_bBarcodeMoveCompleteFlag = FALSE;
	THEAPP.m_pHandlerService->Set_AMoveRequest(nStartPosX, 0, 1, VISION_TYPE_BARCODE);
	// 무브 컴플리트 기다렸다가 좌표 표시
	while (!THEAPP.m_pHandlerService->m_bBarcodeMoveCompleteFlag)
	{
		Sleep(1);
	}
	THEAPP.m_pHandlerService->Set_PositionRequest(VISION_TYPE_BARCODE);

	return 0;
}

void CBarcodeDlg::OnBnClickedButtonMakeHandlerPositionMap()
{

	UpdateData(TRUE);

	if (THEAPP.m_pModelDataManager->m_sModelName == ".")
		return;

	CString FolderName;
	if (!THEAPP.Struct_PreferenceStruct.m_bUseAbsolutePathModel) // Ver2629
		FolderName = THEAPP.GetWorkingDirectory() + "\\Model\\" + THEAPP.m_pModelDataManager->m_sModelName;
	else
		FolderName = THEAPP.GetWorkingEvmsDirectory() + "\\Model\\" + THEAPP.m_pModelDataManager->m_sModelName;

	////////////////////////////////// Added for CMI3000 2000 ====> ///////////////////////////////////////////////////
	////// 테이블 번호에 따라 포지션맵을 만든다 //////////////////////////////////////
	CString FileName;
#ifdef INLINE_MODE
	FileName = FolderName + "\\HW\\BarcodeMotionMovingPosition.ini";
#endif

	////////////////////////////////// <==== Added for CMI3000 2000 ///////////////////////////////////////////////////

	CIniFileCS INI(FileName);
	CString strSection = "Start Position";

	CString strXposition, strYposition;
	GetDlgItemText(IDC_EDIT_SET_START_POSITION_X, strXposition);
	double nStartPosX = atof(strXposition);

	INI.Set_Double(strSection, "X", nStartPosX);

	strSection = "XY Pitch";
	INI.Set_Double(strSection, "X", THEAPP.m_pModelDataManager->m_dModulePitchX);

	strSection = "Moving Position";

	int ArrayX = THEAPP.m_pModelDataManager->m_iTrayArrayX;
	if (THEAPP.Struct_PreferenceStruct.m_iPickerTrayDir == PICKER_TRAY_DIR_Y)
		ArrayX = THEAPP.m_pModelDataManager->m_iTrayArrayY;

	CString strModuleNumber;

	for (int i = 0; i < ArrayX; i++)
	{
		strModuleNumber.Format("X-%d", i);
		INI.Set_Double(strSection, strModuleNumber, nStartPosX - (i * THEAPP.m_pModelDataManager->m_dModulePitchX));
	}

	AfxBeginThread(BarcodeMotionPositionTest_Thread, this);
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
		// Barcode Rotation Inspection Start
		try
		{
			Hlong lArea;
			double dCenterY, dCenterX, dOffsetX, dOffsetY, dOffsetRotation;

			// Barcode Hole Center Start //if문 추가
			if (!THEAPP.m_pModelDataManager->m_bUseBarcodeShiftHoleCenter)
			{
				area_center(m_HBarcodeDetectRgn, &lArea, &dCenterY, &dCenterX);

				dOffsetX = (dCenterX - (double)PGCAM_IMAGE_WIDTH * 0.5) * PGCAM_PIXEL_RESOLUTION - THEAPP.m_pModelDataManager->m_dBarcodeShiftRefX;
				dOffsetY = ((double)PGCAM_IMAGE_HEIGHT * 0.5 - dCenterY) * PGCAM_PIXEL_RESOLUTION - THEAPP.m_pModelDataManager->m_dBarcodeShiftRefY;
			}
			else
			{
				try
				{
					double dBarcodeCenterX, dBarcodeCenterY, dHoleCenterX, dHoleCenterY;
					Hobject HHoleSearchRectRgn;
					Hobject HTempImage;
					Hobject HTempHoleRgn;
					gen_rectangle1(&HHoleSearchRectRgn, THEAPP.m_pModelDataManager->m_iBarcodeLTPointY, THEAPP.m_pModelDataManager->m_iBarcodeLTPointX, THEAPP.m_pModelDataManager->m_iBarcodeRBPointY, THEAPP.m_pModelDataManager->m_iBarcodeRBPointX);
					dilation_circle(HHoleSearchRectRgn, &HHoleSearchRectRgn, THEAPP.m_pModelDataManager->m_iBarcodeShiftHoleCenterDilation + 0.5);
					HTempImage = m_HImage;

					reduce_domain(HTempImage, HHoleSearchRectRgn, &HTempImage);
					threshold(HTempImage, &HTempHoleRgn, THEAPP.m_pModelDataManager->m_iBarcodeShiftHoleCenterGvMin, THEAPP.m_pModelDataManager->m_iBarcodeShiftHoleCenterGvMax);
					fill_up(HTempHoleRgn, &HTempHoleRgn);
					connection(HTempHoleRgn, &HTempHoleRgn);
					select_shape_std(HTempHoleRgn, &HTempHoleRgn, "max_area", 70);
					if (THEAPP.m_pModelDataManager->m_iBarcodeShiftHoleCenterGvClosing > 0)
						closing_circle(HTempHoleRgn, &HTempHoleRgn, THEAPP.m_pModelDataManager->m_iBarcodeShiftHoleCenterGvClosing + 0.5);

					area_center(m_HBarcodeDetectRgn, &lArea, &dBarcodeCenterY, &dBarcodeCenterX);
					area_center(HTempHoleRgn, &lArea, &dHoleCenterY, &dHoleCenterX);

					dOffsetX = (dBarcodeCenterX - dHoleCenterX) * PGCAM_PIXEL_RESOLUTION - THEAPP.m_pModelDataManager->m_dBarcodeShiftRefX;
					dOffsetY = (dBarcodeCenterY - dHoleCenterY) * PGCAM_PIXEL_RESOLUTION - THEAPP.m_pModelDataManager->m_dBarcodeShiftRefY;
				}
				catch (HException &except)
				{
					AfxMessageBox("Shift Inspection Error");
				}
			}

			HTuple htTempRow, htTempCol, htTempPhi, htTempLength1, htTempLength2, htDeg;
			smallest_rectangle2(m_HBarcodeDetectRgn, &htTempRow, &htTempCol, &htTempPhi, &htTempLength1, &htTempLength2);
			tuple_deg(htTempPhi, &dOffsetRotation);

			if (dOffsetRotation < -45)
			{
				dOffsetRotation = 90 + dOffsetRotation;
			}
			else if (dOffsetRotation > 45)
			{
				dOffsetRotation = 90 - dOffsetRotation;
			}

			dOffsetRotation = dOffsetRotation - THEAPP.m_pModelDataManager->m_dBarcodeShiftRefRotation;

			CString strResult;
			CString strBarcode = BARCODE_STATUS_DEFAULT;
			if (DecodedDataStrings.Num() > 0)
			{
				strBarcode = DecodedDataStrings[0].S();
			}
			strResult.Format("X offset     :%.2lf(mm)\rY offset     :%.2lf(mm)\rRotation   :%.2lf(degree)", dOffsetX, dOffsetY, dOffsetRotation);
			AfxMessageBox(strResult);
			strResult.Format("[Barcode Inspection]Barcode: %s\rX offset     :%.2lf(mm)\rY offset     :%.2lf(mm)\rRotation   :%.2lf(degree)", strBarcode, dOffsetX, dOffsetY, dOffsetRotation);
			THEAPP.SaveLog(strResult);
		}
		catch (HException &except)
		{
		}
		// Barcode Rotation Inspection End

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
		FileName = FolderName + "TeachingBarcode";
		write_image(m_HImage, "bmp", 0, FileName);
		InvalidateRect(m_ViewRect, FALSE);
	}
}

void CBarcodeDlg::OnBnClickedBarcodeMultipleImageLoadInspection() // Barcode Rotation Inspection
{
	CString PathName;
	CString strFileName;
	CString csTmp, csData;
	char szFileNameList[9999] = {
		0,
	};

	char szFilter[] = "All Files (*.*)|*.*|Bitmap Files (*.bmp)|*.bmp|JPEG Files (*.jpg)|*.jpg|IMA Files (*.ima)|*.ima|";

	CFileDialog FileDialog(TRUE, NULL, NULL, OFN_ALLOWMULTISELECT, szFilter, NULL);
	FileDialog.m_ofn.lpstrFile = szFileNameList;
	FileDialog.m_ofn.nMaxFile = sizeof(szFileNameList) - 1;
	FileDialog.m_ofn.lpstrTitle = _T("Load Image File");
	FileDialog.m_ofn.lpstrInitialDir = _T("");

	int nBarcodeInspPass = 0;
	int nBarcodeInspFail = 0;
	int nXShiftNG = 0;
	int nYShiftNG = 0;
	int nRotationNG = 0;
	int nNotLoadImage = 0;
	double dTotalOffsetX = 0.0;
	double dTotalOffsetY = 0.0;
	double dTotalOffsetRotation = 0.0;
	double dMaxOffsetX = 0.0;
	double dMaxOffsetY = 0.0;
	double dMaxOffsetRotation = 0.0;

	if (FileDialog.DoModal() == IDOK)
	{
		POSITION pos = FileDialog.GetStartPosition();
		while (pos)
		{
			PathName = (LPCTSTR)FileDialog.GetNextPathName(pos);
			try
			{
				read_image(&m_HImage, (LPCTSTR)PathName);
			}
			catch (HException &except)
			{
				nNotLoadImage++;
				continue;
			}
			if (THEAPP.m_pGFunction->ValidHImage(m_HImage) == FALSE)
			{
				nBarcodeInspFail++;
				continue;
			}

			Hobject HBarcodeImage;
			HTuple DataCodeHandle, ResultHandles, DecodedDataStrings;
			Hobject SymbolXLDs;

			BOOL bFindBarcodeData = TRUE;
			try
			{
				SymbolXLDs.Reset();

				crop_rectangle1(m_HImage, &HBarcodeImage, THEAPP.m_pModelDataManager->m_iBarcodeLTPointY, THEAPP.m_pModelDataManager->m_iBarcodeLTPointX,
								THEAPP.m_pModelDataManager->m_iBarcodeRBPointY, THEAPP.m_pModelDataManager->m_iBarcodeRBPointX);

				create_data_code_2d_model(HTuple("Data Matrix ECC 200"), HTuple(), HTuple(), &DataCodeHandle);
				set_data_code_2d_param(DataCodeHandle, HTuple("default_parameters"), HTuple("enhanced_recognition"));
				find_data_code_2d(HBarcodeImage, &SymbolXLDs, DataCodeHandle, HTuple(), HTuple(), &ResultHandles, &DecodedDataStrings);
				clear_data_code_2d_model(DataCodeHandle);

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
				nBarcodeInspFail++;
				continue;
			}

			CString szBarcodeResult;

			if (bFindBarcodeData)
			{
				try
				{
					Hlong lArea;
					double dCenterY, dCenterX, dOffsetX, dOffsetY, dOffsetRotation;

					if (!THEAPP.m_pModelDataManager->m_bUseBarcodeShiftHoleCenter) // Barcode Hole Center
					{
						area_center(m_HBarcodeDetectRgn, &lArea, &dCenterY, &dCenterX);

						dOffsetX = (dCenterX - (double)PGCAM_IMAGE_WIDTH * 0.5) * PGCAM_PIXEL_RESOLUTION - THEAPP.m_pModelDataManager->m_dBarcodeShiftRefX;
						dOffsetY = ((double)PGCAM_IMAGE_HEIGHT * 0.5 - dCenterY) * PGCAM_PIXEL_RESOLUTION - THEAPP.m_pModelDataManager->m_dBarcodeShiftRefY;
					}
					else
					{
						try
						{
							double dBarcodeCenterX, dBarcodeCenterY, dHoleCenterX, dHoleCenterY;
							Hobject HHoleSearchRectRgn;
							Hobject HTempImage;
							Hobject HTempHoleRgn;
							gen_rectangle1(&HHoleSearchRectRgn, THEAPP.m_pModelDataManager->m_iBarcodeLTPointY, THEAPP.m_pModelDataManager->m_iBarcodeLTPointX, THEAPP.m_pModelDataManager->m_iBarcodeRBPointY, THEAPP.m_pModelDataManager->m_iBarcodeRBPointX);
							dilation_circle(HHoleSearchRectRgn, &HHoleSearchRectRgn, THEAPP.m_pModelDataManager->m_iBarcodeShiftHoleCenterDilation + 0.5);
							HTempImage = m_HImage;

							reduce_domain(HTempImage, HHoleSearchRectRgn, &HTempImage);
							threshold(HTempImage, &HTempHoleRgn, THEAPP.m_pModelDataManager->m_iBarcodeShiftHoleCenterGvMin, THEAPP.m_pModelDataManager->m_iBarcodeShiftHoleCenterGvMax);
							fill_up(HTempHoleRgn, &HTempHoleRgn);
							connection(HTempHoleRgn, &HTempHoleRgn);
							select_shape_std(HTempHoleRgn, &HTempHoleRgn, "max_area", 70);
							if (THEAPP.m_pModelDataManager->m_iBarcodeShiftHoleCenterGvClosing > 0)
								closing_circle(HTempHoleRgn, &HTempHoleRgn, THEAPP.m_pModelDataManager->m_iBarcodeShiftHoleCenterGvClosing + 0.5);

							area_center(m_HBarcodeDetectRgn, &lArea, &dBarcodeCenterY, &dBarcodeCenterX);
							area_center(HTempHoleRgn, &lArea, &dHoleCenterY, &dHoleCenterX);

							dOffsetX = (dBarcodeCenterX - dHoleCenterX) * PGCAM_PIXEL_RESOLUTION - THEAPP.m_pModelDataManager->m_dBarcodeShiftRefX;
							dOffsetY = (dBarcodeCenterY - dHoleCenterY) * PGCAM_PIXEL_RESOLUTION - THEAPP.m_pModelDataManager->m_dBarcodeShiftRefY;
						}
						catch (HException &except)
						{
							nBarcodeInspFail++;
							continue;
						}
					}
					HTuple htTempRow, htTempCol, htTempPhi, htTempLength1, htTempLength2, htDeg;
					smallest_rectangle2(m_HBarcodeDetectRgn, &htTempRow, &htTempCol, &htTempPhi, &htTempLength1, &htTempLength2);
					tuple_deg(htTempPhi, &dOffsetRotation);

					if (dOffsetRotation < -45)
					{
						dOffsetRotation = 90 + dOffsetRotation;
					}
					else if (dOffsetRotation > 45)
					{
						dOffsetRotation = 90 - dOffsetRotation;
					}

					dOffsetRotation = dOffsetRotation - THEAPP.m_pModelDataManager->m_dBarcodeShiftRefRotation;

					// count shift, rotation NG Start
					if (abs(dOffsetX) >= THEAPP.m_pModelDataManager->m_dBarcodeShiftToleranceX)
					{
						nXShiftNG++;
					}

					if (abs(dOffsetY) >= THEAPP.m_pModelDataManager->m_dBarcodeShiftToleranceY)
					{
						nYShiftNG++;
					}

					if (abs(dOffsetRotation) >= THEAPP.m_pModelDataManager->m_dBarcodeShiftToleranceRotation)
					{
						nRotationNG++;
					}
					// count shift, rotation NG End

					// Calcuate Maximum Offset X,Y,Rotation Start
					if (abs(dOffsetX) >= abs(dMaxOffsetX))
					{
						dMaxOffsetX = dOffsetX;
					}

					if (abs(dOffsetY) >= abs(dMaxOffsetY))
					{
						dMaxOffsetY = dOffsetY;
					}

					if (abs(dOffsetRotation) >= abs(dMaxOffsetRotation))
					{
						dMaxOffsetRotation = dOffsetRotation;
					}
					// Calcuate Maximum Offset X,Y,Rotation End

					dTotalOffsetX += dOffsetX;
					dTotalOffsetY += dOffsetY;
					dTotalOffsetRotation += dOffsetRotation;
				}
				catch (HException &except)
				{
					nBarcodeInspFail++;
					continue;
				}

				if (DecodedDataStrings.Num() > 0)
				{
					szBarcodeResult = DecodedDataStrings[0].S();
					nBarcodeInspPass++;
				}
			}
			else
			{
				nBarcodeInspFail++;
				continue;
			}
		}

		CString strResult;
		strResult.Format("X offset Avg :%.2lf(mm)\rY offset Avg: %.2lf(mm)\rRotation Avg: %.2lf(degree)\rRecognition Fail: %d\rX Shift NG Count:%d\rY Shift NG Count:%d\rRotation NG Count:%d\rMax Offset X:%.2lf\rMax Offset Y:%.2lf\rMax Offset Rotation:%.2lf\rNot Load Image:%d\n",
						 dTotalOffsetX / nBarcodeInspPass,
						 dTotalOffsetY / nBarcodeInspPass,
						 dTotalOffsetRotation / nBarcodeInspPass,
						 nBarcodeInspFail,
						 nXShiftNG,
						 nYShiftNG,
						 nRotationNG,
						 dMaxOffsetX,
						 dMaxOffsetY,
						 dMaxOffsetRotation,
						 nNotLoadImage);
		AfxMessageBox(strResult);
	}
}

void CBarcodeDlg::OnBnClickedButtonBarcodeShiftHolecenterDisp() // Barcode Hole Center
{
	UpdateData(TRUE);

	try // Make Hole Search Region
	{
		if (THEAPP.m_pGFunction->ValidHRegion(m_HROIRgn))
		{
			dilation_circle(m_HROIRgn, &m_HDispHoleSearchRgn, m_iEditBarcodeShiftHoleCenterDilation + 0.5);
		}
	}
	catch (HException &except)
	{
		gen_empty_obj(&m_HDispHoleSearchRgn);
	}

	try // Make Hole Threshold Region
	{
		if (THEAPP.m_pGFunction->ValidHRegion(m_HDispHoleSearchRgn))
		{
			if (THEAPP.m_pGFunction->ValidHImage(m_HImage))
			{
				reduce_domain(m_HImage, m_HDispHoleSearchRgn, &m_HImage);
				threshold(m_HImage, &m_HHoleRgn, m_iEditBarcodeShiftHoleCenterGvMin, m_iEditBarcodeShiftHoleCenterGvMax);
				fill_up(m_HHoleRgn, &m_HHoleRgn);
				connection(m_HHoleRgn, &m_HHoleRgn);
				select_shape_std(m_HHoleRgn, &m_HHoleRgn, "max_area", 70);
				if (m_iEditBarcodeShiftHoleCenterGvClosing > 0)
					closing_circle(m_HHoleRgn, &m_HHoleRgn, m_iEditBarcodeShiftHoleCenterGvClosing + 0.5);
				m_HDispHoleRgn = m_HHoleRgn;
			}
		}
	}
	catch (HException &except)
	{
		gen_empty_obj(&m_HHoleRgn);
		gen_empty_obj(&m_HDispHoleRgn);
	}

	try // Make Cross(Hole Center)
	{
		if (THEAPP.m_pGFunction->ValidHRegion(m_HDispHoleRgn))
		{
			Hlong hlArea;
			area_center(m_HDispHoleRgn, &hlArea, &m_dHoleCenterY, &m_dHoleCenterX);
			gen_cross_contour_xld(&m_HDispHoleCenterXLD, m_dHoleCenterY, m_dHoleCenterX, 20, 0);
		}
	}
	catch (HException &except)
	{
		gen_empty_obj(&m_HDispHoleCenterXLD);
	}

	try // Make Cross(Barcode Center)
	{
		if (THEAPP.m_pGFunction->ValidHImage(m_HImage))
		{

			Hobject HBarcodeImage;
			HTuple DataCodeHandle, ResultHandles, DecodedDataStrings;
			Hobject SymbolXLDs;

			CString strLog;

			BOOL bFindBarcodeData = TRUE;
			try
			{
				SymbolXLDs.Reset();

				crop_rectangle1(m_HImage, &HBarcodeImage, THEAPP.m_pModelDataManager->m_iBarcodeLTPointY, THEAPP.m_pModelDataManager->m_iBarcodeLTPointX,
								THEAPP.m_pModelDataManager->m_iBarcodeRBPointY, THEAPP.m_pModelDataManager->m_iBarcodeRBPointX);

				create_data_code_2d_model(HTuple("Data Matrix ECC 200"), HTuple(), HTuple(), &DataCodeHandle);
				set_data_code_2d_param(DataCodeHandle, HTuple("default_parameters"), HTuple("enhanced_recognition"));
				find_data_code_2d(HBarcodeImage, &SymbolXLDs, DataCodeHandle, HTuple(), HTuple(), &ResultHandles, &DecodedDataStrings);
				clear_data_code_2d_model(DataCodeHandle);

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
				gen_empty_obj(&m_HDispBarcodeCenterXLD);
			}

			// CString szBarcodeResult;

			if (bFindBarcodeData)
			{
				Hlong hlArea;
				area_center(m_HBarcodeDetectRgn, &hlArea, &m_dBarcodeCenterY, &m_dBarcodeCenterX);
				gen_cross_contour_xld(&m_HDispBarcodeCenterXLD, m_dBarcodeCenterY, m_dBarcodeCenterX, 20, 0);
			}
		}
	}
	catch (HException &except)
	{
		gen_empty_obj(&m_HDispBarcodeCenterXLD);
	}
	InvalidateRect(m_ViewRect, FALSE);
}

void CBarcodeDlg::OnBnClickedButtonBarcodeShiftHolecenterNodisp() // Barcode Hole Center
{
	gen_empty_obj(&m_HDispHoleSearchRgn);
	gen_empty_obj(&m_HDispHoleRgn);
	gen_empty_obj(&m_HDispBarcodeCenterXLD);
	gen_empty_obj(&m_HDispHoleCenterXLD);
	InvalidateRect(m_ViewRect, FALSE);
}

void CBarcodeDlg::OnBnClickedButtonBarcodeShiftHolecenterSave() // Barcode Hole Center
{
	UpdateData(TRUE);

	if (m_bCheckUseBarcodeShiftHoleCenter)
	{

		if (IDYES == AfxMessageBox("바코드-홀 간 Offset 티칭: 예\r해당 창의 값만 저장 : 아니오", MB_YESNO))
		{
			BOOL bSavePossible = TRUE;
			try // Make Hole Search Region
			{
				if (THEAPP.m_pGFunction->ValidHRegion(m_HROIRgn))
				{
					dilation_circle(m_HROIRgn, &m_HDispHoleSearchRgn, m_iEditBarcodeShiftHoleCenterDilation + 0.5);
				}
				else
				{
					bSavePossible = FALSE;
					AfxMessageBox("저장 실패(홀 검출 영역 생성 실패)");
				}
			}
			catch (HException &except)
			{
				gen_empty_obj(&m_HDispHoleSearchRgn);
				bSavePossible = FALSE;
				AfxMessageBox("저장 실패(홀 검출 영역 생성 실패)");
			}

			try // Make Hole Threshold Region
			{
				if (THEAPP.m_pGFunction->ValidHRegion(m_HDispHoleSearchRgn))
				{
					if (THEAPP.m_pGFunction->ValidHImage(m_HImage))
					{
						reduce_domain(m_HImage, m_HDispHoleSearchRgn, &m_HImage);
						threshold(m_HImage, &m_HHoleRgn, m_iEditBarcodeShiftHoleCenterGvMin, m_iEditBarcodeShiftHoleCenterGvMax);
						fill_up(m_HHoleRgn, &m_HHoleRgn);
						connection(m_HHoleRgn, &m_HHoleRgn);
						select_shape_std(m_HHoleRgn, &m_HHoleRgn, "max_area", 70);
						if (m_iEditBarcodeShiftHoleCenterGvClosing > 0)
							closing_circle(m_HHoleRgn, &m_HHoleRgn, m_iEditBarcodeShiftHoleCenterGvClosing + 0.5);
						m_HDispHoleRgn = m_HHoleRgn;
					}
					else
					{
						bSavePossible = FALSE;
						AfxMessageBox("저장 실패(로드된 이미지가 없음)");
					}
				}
				else
				{
					bSavePossible = FALSE;
					AfxMessageBox("저장 실패(홀 영역 검색 실패)");
				}
			}
			catch (HException &except)
			{
				gen_empty_obj(&m_HHoleRgn);
				gen_empty_obj(&m_HDispHoleRgn);
				bSavePossible = FALSE;
				AfxMessageBox("저장 실패(홀 영역 검색 실패)");
			}

			try // Make Cross(Hole Center)
			{
				if (THEAPP.m_pGFunction->ValidHRegion(m_HDispHoleRgn))
				{
					Hlong hlArea;
					area_center(m_HDispHoleRgn, &hlArea, &m_dHoleCenterY, &m_dHoleCenterX);
					gen_cross_contour_xld(&m_HDispHoleCenterXLD, m_dHoleCenterY, m_dHoleCenterX, 20, 0);
				}
				else
				{
					bSavePossible = FALSE;
					AfxMessageBox("저장 실패(홀 영역이 없어 홀 중심 찾기 실패)");
				}
			}
			catch (HException &except)
			{
				gen_empty_obj(&m_HDispHoleCenterXLD);
				bSavePossible = FALSE;
				AfxMessageBox("저장 실패(홀 중심 찾기 실패)");
			}

			try // Make Cross(Barcode Center)
			{
				if (THEAPP.m_pGFunction->ValidHImage(m_HImage))
				{

					Hobject HBarcodeImage;
					HTuple DataCodeHandle, ResultHandles, DecodedDataStrings;
					Hobject SymbolXLDs;

					CString strLog;

					BOOL bFindBarcodeData = TRUE;
					try
					{
						SymbolXLDs.Reset();

						crop_rectangle1(m_HImage, &HBarcodeImage, THEAPP.m_pModelDataManager->m_iBarcodeLTPointY, THEAPP.m_pModelDataManager->m_iBarcodeLTPointX,
										THEAPP.m_pModelDataManager->m_iBarcodeRBPointY, THEAPP.m_pModelDataManager->m_iBarcodeRBPointX);

						create_data_code_2d_model(HTuple("Data Matrix ECC 200"), HTuple(), HTuple(), &DataCodeHandle);
						set_data_code_2d_param(DataCodeHandle, HTuple("default_parameters"), HTuple("enhanced_recognition"));
						find_data_code_2d(HBarcodeImage, &SymbolXLDs, DataCodeHandle, HTuple(), HTuple(), &ResultHandles, &DecodedDataStrings);
						clear_data_code_2d_model(DataCodeHandle);

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
						gen_empty_obj(&m_HDispBarcodeCenterXLD);
					}

					// CString szBarcodeResult;

					if (bFindBarcodeData)
					{
						Hlong hlArea;
						area_center(m_HBarcodeDetectRgn, &hlArea, &m_dBarcodeCenterY, &m_dBarcodeCenterX);
						gen_cross_contour_xld(&m_HDispBarcodeCenterXLD, m_dBarcodeCenterY, m_dBarcodeCenterX, 20, 0);
					}
					else
					{
						bSavePossible = FALSE;
						AfxMessageBox("저장 실패(바코드 중심 찾기 실패)");
					}
				}
			}
			catch (HException &except)
			{
				bSavePossible = FALSE;
				AfxMessageBox("저장 실패(바코드 중심 찾기 실패)");
				gen_empty_obj(&m_HDispBarcodeCenterXLD);
			}

			if (bSavePossible)
			{
				CString strModelFolder;
				if (!THEAPP.Struct_PreferenceStruct.m_bUseAbsolutePathModel) // Ver2629
					strModelFolder = THEAPP.GetWorkingDirectory() + "\\Model\\" + THEAPP.m_pModelDataManager->m_sModelName;
				else
					strModelFolder = THEAPP.GetWorkingEvmsDirectory() + "\\Model\\" + THEAPP.m_pModelDataManager->m_sModelName;

				CIniFileCS INI(strModelFolder + "\\SW\\ModelBase.ini");
				CString strSection = "Model Base";
				// Save
				strSection = "Barcode Hole Center";
				THEAPP.m_pModelDataManager->m_bUseBarcodeShiftHoleCenter = m_bCheckUseBarcodeShiftHoleCenter;
				THEAPP.m_pModelDataManager->m_iBarcodeShiftHoleCenterGvMin = m_iEditBarcodeShiftHoleCenterGvMin;
				THEAPP.m_pModelDataManager->m_iBarcodeShiftHoleCenterGvMax = m_iEditBarcodeShiftHoleCenterGvMax;
				THEAPP.m_pModelDataManager->m_iBarcodeShiftHoleCenterGvClosing = m_iEditBarcodeShiftHoleCenterGvClosing;
				THEAPP.m_pModelDataManager->m_iBarcodeShiftHoleCenterDilation = m_iEditBarcodeShiftHoleCenterDilation;
				INI.Set_Bool(strSection, "m_bUseBarcodeShiftHoleCenter", THEAPP.m_pModelDataManager->m_bUseBarcodeShiftHoleCenter);
				INI.Set_Integer(strSection, "m_iBarcodeShiftHoleCenterGvMin", THEAPP.m_pModelDataManager->m_iBarcodeShiftHoleCenterGvMin);
				INI.Set_Integer(strSection, "m_iBarcodeShiftHoleCenterGvMax", THEAPP.m_pModelDataManager->m_iBarcodeShiftHoleCenterGvMax);
				INI.Set_Integer(strSection, "m_iBarcodeShiftHoleCenterGvClosing", THEAPP.m_pModelDataManager->m_iBarcodeShiftHoleCenterGvClosing);
				INI.Set_Integer(strSection, "m_iBarcodeShiftHoleCenterDilation", THEAPP.m_pModelDataManager->m_iBarcodeShiftHoleCenterDilation);

				strSection = "Barcode Shift";
				THEAPP.m_pModelDataManager->m_dBarcodeShiftRefX = (m_dBarcodeCenterX - m_dHoleCenterX) * PGCAM_PIXEL_RESOLUTION;
				THEAPP.m_pModelDataManager->m_dBarcodeShiftRefY = (m_dBarcodeCenterY - m_dHoleCenterY) * PGCAM_PIXEL_RESOLUTION;
				INI.Set_Double(strSection, "m_dBarcodeShiftRefX", THEAPP.m_pModelDataManager->m_dBarcodeShiftRefX);
				INI.Set_Double(strSection, "m_dBarcodeShiftRefY", THEAPP.m_pModelDataManager->m_dBarcodeShiftRefY);
				AfxMessageBox("저장 성공\r(주의:기타 파라미터의 Shift Center Offset이 변경)");
			}
		}
		else if (IDNO)

		{
			CString strModelFolder;
			if (!THEAPP.Struct_PreferenceStruct.m_bUseAbsolutePathModel) // Ver2629
				strModelFolder = THEAPP.GetWorkingDirectory() + "\\Model\\" + THEAPP.m_pModelDataManager->m_sModelName;
			else
				strModelFolder = THEAPP.GetWorkingEvmsDirectory() + "\\Model\\" + THEAPP.m_pModelDataManager->m_sModelName;

			CIniFileCS INI(strModelFolder + "\\SW\\ModelBase.ini");
			CString strSection = "Model Base";
			// Save
			strSection = "Barcode Hole Center";
			THEAPP.m_pModelDataManager->m_bUseBarcodeShiftHoleCenter = m_bCheckUseBarcodeShiftHoleCenter;
			THEAPP.m_pModelDataManager->m_iBarcodeShiftHoleCenterGvMin = m_iEditBarcodeShiftHoleCenterGvMin;
			THEAPP.m_pModelDataManager->m_iBarcodeShiftHoleCenterGvMax = m_iEditBarcodeShiftHoleCenterGvMax;
			THEAPP.m_pModelDataManager->m_iBarcodeShiftHoleCenterGvClosing = m_iEditBarcodeShiftHoleCenterGvClosing;
			THEAPP.m_pModelDataManager->m_iBarcodeShiftHoleCenterDilation = m_iEditBarcodeShiftHoleCenterDilation;
			INI.Set_Bool(strSection, "m_bUseBarcodeShiftHoleCenter", THEAPP.m_pModelDataManager->m_bUseBarcodeShiftHoleCenter);
			INI.Set_Integer(strSection, "m_iBarcodeShiftHoleCenterGvMin", THEAPP.m_pModelDataManager->m_iBarcodeShiftHoleCenterGvMin);
			INI.Set_Integer(strSection, "m_iBarcodeShiftHoleCenterGvMax", THEAPP.m_pModelDataManager->m_iBarcodeShiftHoleCenterGvMax);
			INI.Set_Integer(strSection, "m_iBarcodeShiftHoleCenterGvClosing", THEAPP.m_pModelDataManager->m_iBarcodeShiftHoleCenterGvClosing);
			INI.Set_Integer(strSection, "m_iBarcodeShiftHoleCenterDilation", THEAPP.m_pModelDataManager->m_iBarcodeShiftHoleCenterDilation);
		}
	}
	else
	{
		CString strModelFolder;
		if (!THEAPP.Struct_PreferenceStruct.m_bUseAbsolutePathModel) // Ver2629
			strModelFolder = THEAPP.GetWorkingDirectory() + "\\Model\\" + THEAPP.m_pModelDataManager->m_sModelName;
		else
			strModelFolder = THEAPP.GetWorkingEvmsDirectory() + "\\Model\\" + THEAPP.m_pModelDataManager->m_sModelName;

		CIniFileCS INI(strModelFolder + "\\SW\\ModelBase.ini");
		CString strSection = "Model Base";
		// Save
		strSection = "Barcode Hole Center";
		THEAPP.m_pModelDataManager->m_bUseBarcodeShiftHoleCenter = m_bCheckUseBarcodeShiftHoleCenter;
		THEAPP.m_pModelDataManager->m_iBarcodeShiftHoleCenterGvMin = m_iEditBarcodeShiftHoleCenterGvMin;
		THEAPP.m_pModelDataManager->m_iBarcodeShiftHoleCenterGvMax = m_iEditBarcodeShiftHoleCenterGvMax;
		THEAPP.m_pModelDataManager->m_iBarcodeShiftHoleCenterGvClosing = m_iEditBarcodeShiftHoleCenterGvClosing;
		THEAPP.m_pModelDataManager->m_iBarcodeShiftHoleCenterDilation = m_iEditBarcodeShiftHoleCenterDilation;
		INI.Set_Bool(strSection, "m_bUseBarcodeShiftHoleCenter", THEAPP.m_pModelDataManager->m_bUseBarcodeShiftHoleCenter);
		INI.Set_Integer(strSection, "m_iBarcodeShiftHoleCenterGvMin", THEAPP.m_pModelDataManager->m_iBarcodeShiftHoleCenterGvMin);
		INI.Set_Integer(strSection, "m_iBarcodeShiftHoleCenterGvMax", THEAPP.m_pModelDataManager->m_iBarcodeShiftHoleCenterGvMax);
		INI.Set_Integer(strSection, "m_iBarcodeShiftHoleCenterGvClosing", THEAPP.m_pModelDataManager->m_iBarcodeShiftHoleCenterGvClosing);
		INI.Set_Integer(strSection, "m_iBarcodeShiftHoleCenterDilation", THEAPP.m_pModelDataManager->m_iBarcodeShiftHoleCenterDilation);
		gen_empty_obj(&m_HDispHoleSearchRgn);
		gen_empty_obj(&m_HDispHoleRgn);
		gen_empty_obj(&m_HDispBarcodeCenterXLD);
		gen_empty_obj(&m_HDispHoleCenterXLD);
	}

	InvalidateRect(m_ViewRect, FALSE);
}