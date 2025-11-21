// TrayImagePB.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "uScan.h"
#include "TrayImagePB.h"


// CTrayImagePB




IMPLEMENT_DYNAMIC(CTrayImagePB, CStatic)

CTrayImagePB::CTrayImagePB()
{
	
		OkNg = NULL;
		ClickRegion = NULL;
	
	WrongPointClick = FALSE;

	bOnDrawing = FALSE;
	
}

CTrayImagePB::~CTrayImagePB()
{

	try
	{
		if (WindowHandle > -1)
		{
			close_window(WindowHandle);
			WindowHandle = -1;
		}
	}
	catch (HException &except) {}
}


BEGIN_MESSAGE_MAP(CTrayImagePB, CStatic)
	ON_WM_LBUTTONDOWN()
	ON_WM_PAINT()
	ON_MESSAGE(WM_DISPLAY_TRAY,OnDispTray)
END_MESSAGE_MAP()



// CTrayImagePB 메시지 처리기입니다.
LRESULT CTrayImagePB::OnDispTray(WPARAM wParam,LPARAM lParam)
{
	ReDraw();
	return 0;
}


void CTrayImagePB::OnLButtonDown(UINT nFlags, CPoint point)
{

try {

	HANDLE hFindFile;
	WIN32_FIND_DATA FindFileData;
	CString sReadFileName;
	CString strImageFileFullName;

	if(THEAPP.m_iModeSwitch==MODE_INSPECT_VIEW_ADMIN)
	{
		// 24.07.31 - v2656 - RAWIMAGE 축소, 합성 저장 기능 추가 - LeeGW
		BYTE *pImageData;
		char type[30];
		Hlong lImageWidth, lImageHeight;

		int iCamImageWidth = THEAPP.m_pCameraManager->GetCamImageWidth();
		int iCamImageHeight = THEAPP.m_pCameraManager->GetCamImageHeight();
		// 24.07.31 - v2656 - RAWIMAGE 축소, 합성 저장 기능 추가 - LeeGW

		for(int i=0; i<THEAPP.m_pModelDataManager->m_iTrayModuleMax; i++)
		{
			ClickRegion[i] = FALSE;
		}
		
		union1(RegionErosion, &RegionErosion);
		connection(RegionErosion, &RegionErosion);

		Hobject Hrect, Hintersection;
		HTuple  HInterArea, HInterRow ,HInterColumn;
		get_mposition(WindowHandle, &Row, &Column, &Button);
		get_region_index(RegionErosion, Row, Column, &Index);
		Hobject sel;
		select_obj(RegionErosion, &sel,Index);
		HTuple Row1, Row2, Cloumn1, Cloumn2;
		smallest_rectangle1(sel,&Row1,&Cloumn1,&Row2,&Cloumn2);
		set_color(WindowHandle,"yellow");
		disp_rectangle1(WindowHandle,Row1,Cloumn1,Row2,Cloumn2);
		ClickRegion[Index[0].I()-1] = TRUE;
		//Invalidate(FALSE);
		ReDraw();

		if(THEAPP.m_pTrayAdminViewDlg->m_bPrevTray==FALSE && THEAPP.m_pModelDataManager->m_viPosInspNo[Index[0].I()-1] >= 0)
		{
			CString FileName, FileNameGrab, FileNameTeach;
			Hobject HImage;
			int iModuleNr = THEAPP.m_pModelDataManager->m_viModuleNo[THEAPP.m_pModelDataManager->m_viPosInspNo[Index[0].I()-1]];

			if(THEAPP.Struct_PreferenceStruct.m_bSaveRawImageMerge == FALSE)
			{
				for (int iImageIdx = 0; iImageIdx < MAX_IMAGE_TAB; iImageIdx++)
				{
					if (iImageIdx >= THEAPP.m_iMaxInspImageNo)
						break;

					FileName.Format("\\Module%d_%s*", iModuleNr, g_sImageName[THEAPP.m_iMachineInspType][iImageIdx]);
					sReadFileName.Format("%s", THEAPP.m_FileBase.m_strOriImageFolderPrev + FileName);

					if ((hFindFile = FindFirstFile(sReadFileName, &FindFileData)) != INVALID_HANDLE_VALUE)
					{
						strImageFileFullName.Format("%s\\%s", THEAPP.m_FileBase.m_strOriImageFolderPrev, (LPCTSTR)FindFileData.cFileName);

						FindClose(hFindFile);

						read_image(&HImage, strImageFileFullName);

						// 24.07.31 - v2656 - RAWIMAGE 축소, 합성 저장 기능 추가 - LeeGW
						get_image_pointer1(HImage, (Hlong*)&pImageData, type, &lImageWidth, &lImageHeight);

						if (lImageWidth != iCamImageWidth || lImageHeight != iCamImageHeight)
							zoom_image_size(HImage, &HImage, iCamImageWidth, iCamImageHeight, "constant");
						// 24.07.31 - v2656 - RAWIMAGE 축소, 합성 저장 기능 추가 - LeeGW

						if (!THEAPP.Struct_PreferenceStruct.m_bUseAbsolutePathModel) //Ver2629
						{
							FileNameTeach.Format("%s\\Model\\%s\\SW\\TeachImage\\TeachingImage", THEAPP.GetWorkingDirectory(), THEAPP.m_pModelDataManager->m_sModelName);
							FileNameGrab.Format("%s\\Model\\%s\\SW\\TeachImage\\Grab_%s", THEAPP.GetWorkingDirectory(), THEAPP.m_pModelDataManager->m_sModelName, g_sImageName[THEAPP.m_iMachineInspType][iImageIdx]);
							
						}
						else
						{
							FileNameTeach.Format("%s\\Model\\%s\\SW\\TeachImage\\TeachingImage", THEAPP.GetWorkingEvmsDirectory(), THEAPP.m_pModelDataManager->m_sModelName);
							FileNameGrab.Format("%s\\Model\\%s\\SW\\TeachImage\\Grab_%s", THEAPP.GetWorkingEvmsDirectory(), THEAPP.m_pModelDataManager->m_sModelName, g_sImageName[THEAPP.m_iMachineInspType][iImageIdx]);
						}

						write_image(HImage, "bmp", 0, FileNameTeach);
						write_image(HImage, "bmp", 0, FileNameGrab);


						switch (iImageIdx)
						{
						case TRIGGER_SURFACE:
							copy_image(HImage, &THEAPP.m_pInspectViewBarrelSurfaceDlg->m_pHImage);
							break;
						case TRIGGER_EDGE:
							copy_image(HImage, &THEAPP.m_pInspectViewBarrelEdgeDlg->m_pHImage);
							break;
						case TRIGGER_LENS1:
							copy_image(HImage, &THEAPP.m_pInspectViewLensInnerDlg->m_pHImage);
							break;
						case TRIGGER_LENS2:
							copy_image(HImage, &THEAPP.m_pInspectViewLensOuterDlg->m_pHImage);
							break;
						case TRIGGER_EXTRA1:
							copy_image(HImage, &THEAPP.m_pInspectViewExtra1Dlg->m_pHImage);
							break;
						case TRIGGER_EXTRA2:
							copy_image(HImage, &THEAPP.m_pInspectViewExtra2Dlg->m_pHImage);
							break;
						}
						
					}
					else
						return;
				}

			}
			else	
			{
				int iNoInspectImage = THEAPP.m_iMaxInspImageNo;

				Hobject HRawImage;
				int iNoImageYDir, iModular;
				int iCropImageSizeX, iCropImageSizeY;

				iNoImageYDir = iNoInspectImage / MAX_COMBINE_IMAGE_NUMBER;
				iModular = iNoInspectImage % MAX_COMBINE_IMAGE_NUMBER;
				if (iModular > 0)
					iNoImageYDir += 1;
				
				FileName.Format("\\Module%d_Combine*", iModuleNr);
				sReadFileName.Format("%s",THEAPP.m_FileBase.m_strOriImageFolderPrev+FileName);

				if((hFindFile = FindFirstFile(sReadFileName, &FindFileData ) ) != INVALID_HANDLE_VALUE )
				{
					strImageFileFullName.Format("%s\\%s", THEAPP.m_FileBase.m_strOriImageFolderPrev, (LPCTSTR)FindFileData.cFileName);

					FindClose(hFindFile);

					read_image(&HImage, strImageFileFullName);

					if (THEAPP.m_pGFunction->ValidHImage(HImage) == TRUE)
					{
						char cImageType[128];
						Hlong lWidth, lHeight;
						unsigned char *cPtr;
						get_image_pointer1(HImage, (Hlong*)&cPtr, cImageType, &lWidth, &lHeight);

						int wd = (int)lWidth;
						int ht = (int)lHeight;

						iCropImageSizeX = wd / MAX_COMBINE_IMAGE_NUMBER;
						iCropImageSizeY = ht / iNoImageYDir;

						POINT CropLTPoint, CropRBPoint;
						int iImageIndexX, iImageIndexY;

						for (int i = 0; i < iNoInspectImage; i++)
						{
							iImageIndexX = i % MAX_COMBINE_IMAGE_NUMBER;
							iImageIndexY = i / MAX_COMBINE_IMAGE_NUMBER;

							CropLTPoint.x = iImageIndexX * iCropImageSizeX;
							CropRBPoint.x = iImageIndexX * iCropImageSizeX + iCropImageSizeX - 1;
							CropLTPoint.y = iImageIndexY * iCropImageSizeY;
							CropRBPoint.y = iImageIndexY * iCropImageSizeY + iCropImageSizeY - 1;

							gen_empty_obj(&HRawImage);
							crop_rectangle1(HImage, &HRawImage, CropLTPoint.y, CropLTPoint.x, CropRBPoint.y, CropRBPoint.x);

							if (THEAPP.m_pGFunction->ValidHImage(HRawImage))
							{
								get_image_pointer1(HRawImage, (Hlong*)&pImageData, type, &lImageWidth, &lImageHeight);

								if (lImageWidth != iCamImageWidth || lImageHeight != iCamImageHeight)
									zoom_image_size(HRawImage, &HRawImage, iCamImageWidth, iCamImageHeight, "constant");

								
								if (!THEAPP.Struct_PreferenceStruct.m_bUseAbsolutePathModel) //Ver2629
								{
									if (i == TRIGGER_SURFACE)
										write_image(HRawImage, "bmp", 0, THEAPP.GetWorkingDirectory() + "\\Model\\" + THEAPP.m_pModelDataManager->m_sModelName + "\\SW\\TeachImage\\" + "TeachingImage");

									write_image(HRawImage, "bmp", 0, THEAPP.GetWorkingDirectory() + "\\Model\\" + THEAPP.m_pModelDataManager->m_sModelName + "\\SW\\TeachImage\\" + "Grab_" + g_sImageName[THEAPP.m_iMachineInspType][i]);
								}
								else
								{
									if (i == TRIGGER_SURFACE)
										write_image(HRawImage, "bmp", 0, THEAPP.GetWorkingEvmsDirectory() + "\\Model\\" + THEAPP.m_pModelDataManager->m_sModelName + "\\SW\\TeachImage\\" + "TeachingImage");
									
									write_image(HRawImage, "bmp", 0, THEAPP.GetWorkingEvmsDirectory() + "\\Model\\" + THEAPP.m_pModelDataManager->m_sModelName + "\\SW\\TeachImage\\" + "Grab_" + g_sImageName[THEAPP.m_iMachineInspType][i]);
								}

								switch (i)
								{
								case TRIGGER_SURFACE:
									copy_image(HRawImage, &THEAPP.m_pInspectViewBarrelSurfaceDlg->m_pHImage);
									break;
								case TRIGGER_EDGE:
									copy_image(HRawImage, &THEAPP.m_pInspectViewBarrelEdgeDlg->m_pHImage);
									break;
								case TRIGGER_LENS1:
									copy_image(HRawImage, &THEAPP.m_pInspectViewLensInnerDlg->m_pHImage);
									break;
								case TRIGGER_LENS2:
									copy_image(HRawImage, &THEAPP.m_pInspectViewLensOuterDlg->m_pHImage);
									break;
								case TRIGGER_EXTRA1:
									copy_image(HRawImage, &THEAPP.m_pInspectViewExtra1Dlg->m_pHImage);
									break;
								case TRIGGER_EXTRA2:
									copy_image(HRawImage, &THEAPP.m_pInspectViewExtra2Dlg->m_pHImage);
									break;
								default:
									return;
								}

							}
							else
								return;

						}	// for (int i = 0; i < iNoInspectImage; i++)
					}
					else
						return;
				}
				else
					return;
			}

			THEAPP.m_pInspectViewBarrelSurfaceDlg->InvalidateRect(false);
			THEAPP.m_pInspectViewBarrelEdgeDlg->InvalidateRect(false);
			THEAPP.m_pInspectViewLensInnerDlg->InvalidateRect(false);
			THEAPP.m_pInspectViewLensOuterDlg->InvalidateRect(false);

			if (THEAPP.m_iMachineInspType == MACHINE_SIDEFILL ||
				THEAPP.m_iMachineInspType == MACHINE_FIDUCIAL ||
				THEAPP.m_iMachineInspType == MACHINE_BRACKET)
			{
				THEAPP.m_pInspectViewExtra1Dlg->InvalidateRect(false);
				THEAPP.m_pInspectViewExtra2Dlg->InvalidateRect(false);
			}
			////////////////////////////////// changed for CMI3000 2000 ====> ///////////////////////////////////////////////////
			
			// 24.05.16 - v2647 - 이미지별 불량 표기를 위해 추가 - LeeGW

			CResultViewSelectDlg resultViewDlg;
			resultViewDlg.DoModal();

			FileName.Format("\\Module%d_Result_%s*", iModuleNr, g_sImageName[THEAPP.m_iMachineInspType][resultViewDlg.m_iSelectedResultView]);
			// 24.05.16 - v2647 - 이미지별 불량 표기를 위해 추가 - LeeGW End


			sReadFileName.Format("%s",THEAPP.m_FileBase.m_strOverayImageFolderPrev+FileName);

			if((hFindFile = FindFirstFile(sReadFileName, &FindFileData ) ) != INVALID_HANDLE_VALUE )
			{
				strImageFileFullName.Format("%s\\%s", THEAPP.m_FileBase.m_strOverayImageFolderPrev, (LPCTSTR)FindFileData.cFileName);

				FindClose(hFindFile);

				read_image(&HImage, strImageFileFullName);
				copy_image(HImage, &THEAPP.m_pInspectViewOverayImageDlg->m_pHImage);
			}
			else
				return;

			get_image_pointer1(HImage, (Hlong*)&pImageData, type, &lImageWidth, &lImageHeight);
			THEAPP.m_pInspectViewOverayImageDlg->m_iImageWidth = (int)lImageWidth;
			THEAPP.m_pInspectViewOverayImageDlg->m_iImageHeight = (int)lImageHeight;	

			THEAPP.m_pInspectViewOverayImageDlg->InvalidateRect(false);
			THEAPP.m_pInspectViewOverayImageDlg->CallFitImageHeight();

			////////////////////////////////// <==== changed for CMI3000 2000 ///////////////////////////////////////////////////
		}
	}

	CStatic::OnLButtonDown(nFlags, point);
}
catch(HException &except)
{
	CString str;
	str.Format("Halcon Exception [CTrayImagePB::OnLButtonDown] : %s", except.message);
	WrongPointClick = TRUE;
	ReDraw();
}
}


void CTrayImagePB::OnPaint()
{
	CPaintDC dc(this); // device context for painting

	ReDraw();

}

void CTrayImagePB::ReDraw()
{
	try
	{
		bOnDrawing = TRUE;

		Hobject SelObj;
		HTuple HTuArea, HTuRow, HTuCol, HTuCNT;
		Hobject HRgn_conn;
		HTuple HTuNumber;
		//HTuple Row1, Cloumn1, Row2, Cloumn2;
		connection(RegionErosion, &HRgn_conn);
		set_font(WindowHandle, "-Courier New-18-*-*-*-*-1-");
		count_obj(HRgn_conn, &HTuNumber);
		if ((OkNg == NULL) || (ClickRegion == NULL)) { return; }

		for (int i = 0; i < HTuNumber; i++)
		{
			select_obj(HRgn_conn, &SelObj, i + 1);
			HTuple HTuRow1, HTuRow2, HTuCol1, HTuCol2;
			area_center(SelObj, &HTuArea, &HTuRow, &HTuCol);
			set_tposition(WindowHandle, HTuRow - 12, HTuCol - 8);
			if (!WrongPointClick)
			{
				if (!ClickRegion[i])
				{
					if (THEAPP.m_pModelDataManager->m_viPosInspNo[i] < 0) continue;
					smallest_rectangle1(SelObj, &HTuRow1, &HTuCol1, &HTuRow2, &HTuCol2);
					int idx = THEAPP.m_pModelDataManager->m_viModuleNo[THEAPP.m_pModelDataManager->m_viPosInspNo[i]] - 1;
					if (OkNg[idx] == DEFECT_TYPE_PROCESSING_ERROR)
					{
						set_color(WindowHandle, "white");
						disp_rectangle1(WindowHandle, HTuRow1, HTuCol1, HTuRow2, HTuCol2);
					}

					if (OkNg[idx] == DEFECT_TYPE_MODULE_MIX_ERROR)
					{
						set_color(WindowHandle, "yellow");
						disp_rectangle1(WindowHandle, HTuRow1, HTuCol1, HTuRow2, HTuCol2);
						set_color(WindowHandle, "black");
						disp_rectangle1(WindowHandle, HTuRow1 + 2, HTuCol1 + 2, HTuRow2 - 2, HTuCol2 - 2);

						set_color(WindowHandle, "yellow");
						write_string(WindowHandle, "MX");
					}
					else if (OkNg[idx] == DEFECT_TYPE_GOOD)
					{
						set_rgb(WindowHandle, THEAPP.Struct_PreferenceStruct.m_iTrayOK_R, THEAPP.Struct_PreferenceStruct.m_iTrayOK_G, THEAPP.Struct_PreferenceStruct.m_iTrayOK_B);
						disp_rectangle1(WindowHandle, HTuRow1 + 2, HTuCol1 + 2, HTuRow2 - 2, HTuCol2 - 2);

						set_tposition(WindowHandle, HTuRow - 12, HTuCol - 4);
						set_rgb(WindowHandle, THEAPP.Struct_PreferenceStruct.m_iTrayOKFont_R, THEAPP.Struct_PreferenceStruct.m_iTrayOKFont_G, THEAPP.Struct_PreferenceStruct.m_iTrayOKFont_B);
						write_string(WindowHandle, "O");
					}
					////////////////////////////////// added for CMI3000 2000 ====> ///////////////////////////////////////////////////
					else if (OkNg[idx] == DEFECT_TYPE_LIGHT_ERROR)
					{
						set_color(WindowHandle, "yellow");
						disp_rectangle1(WindowHandle, HTuRow1, HTuCol1, HTuRow2, HTuCol2);
						set_color(WindowHandle, "black");
						disp_rectangle1(WindowHandle, HTuRow1 + 2, HTuCol1 + 2, HTuRow2 - 2, HTuCol2 - 2);

						set_color(WindowHandle, "yellow");
						write_string(WindowHandle, "LE");
					}
					else if (OkNg[idx] == DEFECT_TYPE_BARCODE_ERROR)
					{
						set_color(WindowHandle, "orange");
						disp_rectangle1(WindowHandle, HTuRow1, HTuCol1, HTuRow2, HTuCol2);
						set_color(WindowHandle, "black");
						disp_rectangle1(WindowHandle, HTuRow1 + 2, HTuCol1 + 2, HTuRow2 - 2, HTuCol2 - 2);

						set_color(WindowHandle, "orange");
						write_string(WindowHandle, "BE");
					}
					else if (OkNg[idx] == DEFECT_TYPE_BARCODE_SHIFT)
					{
						set_color(WindowHandle, "orange");
						disp_rectangle1(WindowHandle, HTuRow1, HTuCol1, HTuRow2, HTuCol2);
						set_color(WindowHandle, "black");
						disp_rectangle1(WindowHandle, HTuRow1 + 2, HTuCol1 + 2, HTuRow2 - 2, HTuCol2 - 2);

						set_color(WindowHandle, "orange");
						write_string(WindowHandle, "BS");
					}
					else if (OkNg[idx] == DEFECT_TYPE_EMPTY)
					{
						set_color(WindowHandle, "medium slate blue");
						disp_rectangle1(WindowHandle, HTuRow1, HTuCol1, HTuRow2, HTuCol2);
						set_color(WindowHandle, "white");
						disp_rectangle1(WindowHandle, HTuRow1 + 2, HTuCol1 + 2, HTuRow2 - 2, HTuCol2 - 2);

						set_color(WindowHandle, "medium slate blue");
						write_string(WindowHandle, "EM");
					}
					////////////////////////////////// <==== added for CMI3000 2000 ///////////////////////////////////////////////////
					else if (OkNg[idx] == DEFECT_TYPE_ALIGN_ERROR)
					{
						set_color(WindowHandle, "magenta");
						disp_rectangle1(WindowHandle, HTuRow1, HTuCol1, HTuRow2, HTuCol2);
						set_color(WindowHandle, "white");
						disp_rectangle1(WindowHandle, HTuRow1 + 2, HTuCol1 + 2, HTuRow2 - 2, HTuCol2 - 2);

						set_color(WindowHandle, "red");
						write_string(WindowHandle, "MC");
					}
					else if (OkNg[idx] == DEFECT_TYPE_SPRINGNG)
					{
						set_color(WindowHandle, "red");
						disp_rectangle1(WindowHandle, HTuRow1, HTuCol1, HTuRow2, HTuCol2);
						set_color(WindowHandle, "white");
						disp_rectangle1(WindowHandle, HTuRow1 + 2, HTuCol1 + 2, HTuRow2 - 2, HTuCol2 - 2);

						set_color(WindowHandle, "red");
						write_string(WindowHandle, "SP");
					}
					else if (OkNg[idx] == DEFECT_TYPE_EPOXYHOLE)
					{
						set_color(WindowHandle, "red");
						disp_rectangle1(WindowHandle, HTuRow1, HTuCol1, HTuRow2, HTuCol2);
						set_color(WindowHandle, "white");
						disp_rectangle1(WindowHandle, HTuRow1 + 2, HTuCol1 + 2, HTuRow2 - 2, HTuCol2 - 2);

						set_color(WindowHandle, "red");
						write_string(WindowHandle, "EH");
					}
					else if (OkNg[idx] == DEFECT_TYPE_BARREL)
					{
						set_rgb(WindowHandle, THEAPP.Struct_PreferenceStruct.m_iTrayNGB_R, THEAPP.Struct_PreferenceStruct.m_iTrayNGB_G, THEAPP.Struct_PreferenceStruct.m_iTrayNGB_B);
						disp_rectangle1(WindowHandle, HTuRow1 + 2, HTuCol1 + 2, HTuRow2 - 2, HTuCol2 - 2);

						set_tposition(WindowHandle, HTuRow - 12, HTuCol - 4);
						set_rgb(WindowHandle, THEAPP.Struct_PreferenceStruct.m_iTrayNGFont_R, THEAPP.Struct_PreferenceStruct.m_iTrayNGFont_G, THEAPP.Struct_PreferenceStruct.m_iTrayNGFont_B);
						write_string(WindowHandle, "B");
					}
					else if (OkNg[idx] == DEFECT_TYPE_LENS)
					{
						set_rgb(WindowHandle, THEAPP.Struct_PreferenceStruct.m_iTrayNGL_R, THEAPP.Struct_PreferenceStruct.m_iTrayNGL_G, THEAPP.Struct_PreferenceStruct.m_iTrayNGL_B);
						disp_rectangle1(WindowHandle, HTuRow1 + 2, HTuCol1 + 2, HTuRow2 - 2, HTuCol2 - 2);

						set_tposition(WindowHandle, HTuRow - 12, HTuCol - 4);
						set_rgb(WindowHandle, THEAPP.Struct_PreferenceStruct.m_iTrayNGFont_R, THEAPP.Struct_PreferenceStruct.m_iTrayNGFont_G, THEAPP.Struct_PreferenceStruct.m_iTrayNGFont_B);
						write_string(WindowHandle, "L");
					}
					else if (OkNg[idx] == DEFECT_TYPE_BARREL_LENS)
					{
						set_rgb(WindowHandle, THEAPP.Struct_PreferenceStruct.m_iTrayNG_R, THEAPP.Struct_PreferenceStruct.m_iTrayNG_G, THEAPP.Struct_PreferenceStruct.m_iTrayNG_B);
						disp_rectangle1(WindowHandle, HTuRow1 + 2, HTuCol1 + 2, HTuRow2 - 2, HTuCol2 - 2);

						set_rgb(WindowHandle, THEAPP.Struct_PreferenceStruct.m_iTrayNGFont_R, THEAPP.Struct_PreferenceStruct.m_iTrayNGFont_G, THEAPP.Struct_PreferenceStruct.m_iTrayNGFont_B);
						write_string(WindowHandle, "BL");
					}
					else if (OkNg[idx] == DEFECT_TYPE_FAING)
					{
						set_color(WindowHandle, "red");
						disp_rectangle1(WindowHandle, HTuRow1, HTuCol1, HTuRow2, HTuCol2);
						set_color(WindowHandle, "yellow");
						disp_rectangle1(WindowHandle, HTuRow1 + 2, HTuCol1 + 2, HTuRow2 - 2, HTuCol2 - 2);

						set_color(WindowHandle, "red");
						write_string(WindowHandle, "FAI");
					}
					else if (OkNg[idx] == DEFECT_TYPE_BLACK_COATING_DIAMETER)
					{
						set_color(WindowHandle, "red");
						disp_rectangle1(WindowHandle, HTuRow1, HTuCol1, HTuRow2, HTuCol2);
						set_color(WindowHandle, "white");
						disp_rectangle1(WindowHandle, HTuRow1 + 2, HTuCol1 + 2, HTuRow2 - 2, HTuCol2 - 2);

						set_color(WindowHandle, "red");
						write_string(WindowHandle, "CD");
					}
				}
			}
			else
			{
				WrongPointClick = FALSE;
			}
			if (HTuCNT < 9)
				set_tposition(WindowHandle, HTuRow, HTuCol - 4);
			else
				set_tposition(WindowHandle, HTuRow, HTuCol - 8);
			set_color(WindowHandle, "blue");

			if (THEAPP.m_pModelDataManager->m_viPosInspNo[i] < 0) continue;
			tuple_int(THEAPP.m_pModelDataManager->m_viModuleNo[THEAPP.m_pModelDataManager->m_viPosInspNo[i]], &HTuCNT);
			write_string(WindowHandle, HTuCNT);
		}

		bOnDrawing = FALSE;
	}
	catch (HException& except)
	{
		HTuple HExp;
		HTuple HOperatorName, HErrMsg;
		except.ToHTuple(&HExp);
		except.GetExceptionData(HExp, "operator", &HOperatorName);
		except.GetExceptionData(HExp, "error_message", &HErrMsg);

		CString str;
		str.Format("Halcon Exception [CTrayImagePB::ReDraw] : <%s>%s", (const char *)HOperatorName[0].S(), (const char *)HErrMsg[0].S());
		THEAPP.SaveLog(str);

		bOnDrawing = FALSE;
	}
}




CRect CTrayImagePB::SetPictureBoxSize(CRect PbRect)
{
	CRect rect;
	rect.top = 0;
	rect.bottom = PbRect.Height() - 70;
	rect.left = 0;
	rect.right = PbRect.Width();
	MoveWindow(0, 30, PbRect.Width(), PbRect.Height() - 70);
	return rect;
}