// stdafx.cpp : source file that includes just the standard includes
//	uScan.pch will be the pre-compiled header
//	stdafx.obj will contain the pre-compiled type information

#include "stdafx.h"

int g_iSaveImageIndex = 0;

BOOL PeekAndPump()
{
  static MSG msg;

  while(::PeekMessage(&msg,NULL,0,0,PM_NOREMOVE))
  {
	  if(!AfxGetApp()->PumpMessage())
    {
		  ::PostQuitMessage(0);
		  return FALSE;
	  }	
  }

  return TRUE;
}

double similarity(const Hobject& XldOrg, const Hobject& XldTgt)
{
	//////////////////////////////////////////////////////////////////////////
	// 입력 xld validity 검사
	//////////////////////////////////////////////////////////////////////////

	HTuple CntOrg;

	count_obj(XldOrg, &CntOrg);

	if(CntOrg <= 0)
		return 0.0;
	int i;
	for( i = 0 ; i < CntOrg ; i++)
	{
		Hobject SelectedXld;
		select_obj(XldOrg, &SelectedXld, i + 1);

		HTuple Length;
		length_xld(SelectedXld, &Length);

		if(Length > 0.0)
			goto PASS1;
	}

	return 0.0;

PASS1:

	HTuple CntTgt;

	count_obj(XldTgt, &CntTgt);

	if(CntTgt <= 0)
		return 0.0;

	for(i = 0 ; i < CntTgt ; i++)
	{
		Hobject SelectedXld;
		select_obj(XldTgt, &SelectedXld, i + 1);

		HTuple Length;
		length_xld(SelectedXld, &Length);

		if(Length > 0.0)
			goto PASS2;
	}

	return 0.0;

PASS2:

	//////////////////////////////////////////////////////////////////////////
	// similarity estimation
	//////////////////////////////////////////////////////////////////////////

	HTuple ModelID;

	Herror err = create_shape_model_xld(XldOrg, "auto", 0.0, 0.0, "auto", "point_reduction_high", "ignore_local_polarity", 5, &ModelID);
	//Herror err = create_shape_model_xld(XldOrg, "auto", -0.39, 0.79, "auto", "auto", "ignore_local_polarity", 5, &ModelID);
	
	if(err != H_MSG_TRUE)
		return 0.0;

	Hobject ImageTgt;
	gen_bin_image_xld_crop(XldTgt, &ImageTgt);



	HTuple Score;
	find_shape_model(ImageTgt, ModelID, 0.0, 0.0, 0.0, 1, 0.5, "least_squares", 0, 0.9, _, _, _, &Score);

	clear_shape_model(ModelID);

	if(Score.Num() <= 0)
		return 0.0;

	return Score[0].D();
}

void gen_bin_image_xld_crop(const Hobject& Xld, Hobject* pImage, long lMarginX, long lMarginY)
{
	if(!CGFunction::ValidHXLD(Xld))
		return;
	
	//////////////////////////////////////////////////////////////////////////
	// Create image
	//////////////////////////////////////////////////////////////////////////
	
	HTuple Row1, Col1, Row2, Col2;
	smallest_rectangle1_xld(Xld, &Row1, &Col1, &Row2, &Col2);
	
	tuple_min(Row1, &Row1);
	tuple_min(Col1, &Col1);
	tuple_max(Row2, &Row2);
	tuple_max(Col2, &Col2);
	
	HTuple HomMat2DIdentity;
	hom_mat2d_identity(&HomMat2DIdentity);
	
	HTuple MarginX = HTuple(lMarginX);
	HTuple MarginY = HTuple(lMarginY);
	
	HTuple HomMat2DTranslate;
	hom_mat2d_translate(HomMat2DIdentity, -(Row1 - MarginY), -(Col1 - MarginX), &HomMat2DTranslate);
	
	Hobject XldTrans;
	affine_trans_contour_xld(Xld, &XldTrans, HomMat2DTranslate);
	
	DPOINT WHPoint;
	WHPoint.x = Col2[0].D() - Col1[0].D();
	WHPoint.y = Row2[0].D() - Row1[0].D();
	
	Hobject ImageOrg;
	gen_image_const(&ImageOrg, "byte", WHPoint.x + lMarginX * 2, WHPoint.y + lMarginY * 2);
	paint_xld(XldTrans, ImageOrg, pImage, 255);
}

void gen_bin_image_xld(const Hobject& Xld, Hobject* pImage, long lMarginRight, long lMarginBottom)
{
	if(!CGFunction::ValidHXLD(Xld))
		return;
	
	//////////////////////////////////////////////////////////////////////////
	// Create image
	//////////////////////////////////////////////////////////////////////////
	
	HTuple Row1, Col1, Row2, Col2;
	smallest_rectangle1_xld(Xld, &Row1, &Col1, &Row2, &Col2);
	
	tuple_min(Row1, &Row1);
	tuple_min(Col1, &Col1);
	tuple_max(Row2, &Row2);
	tuple_max(Col2, &Col2);
	
	Hobject Image;
	gen_image_const(&Image, "byte", Col2 + lMarginRight, Row2 + lMarginBottom);
	paint_xld(Xld, Image, pImage, 255);
	
}

Herror intersection_contours_lc (const Hobject& Region, const Hobject& Contours, Hobject* pContoursIntersection)
{
	Herror err = H_MSG_TRUE;

	HTuple Number;
	count_obj (Contours, &Number);

	for(int i = 1 ; i <= Number ; i++)
	{
		Hobject ObjectSelected;
	    select_obj (Contours, &ObjectSelected, i);

		HTuple Row, Col;
		get_contour_xld (ObjectSelected, &Row, &Col);

		HTuple Length;
		tuple_length (Row, &Length);

		HTuple RowSelected = HTuple();
		HTuple ColSelected = HTuple();

		pContoursIntersection->Reset();
		gen_empty_obj(pContoursIntersection);

		for(int Index = 0 ; Index < Length ; Index++)
		{
			HTuple IsInside;
	        test_region_point (Region, (HTuple)Row[Index], (HTuple)Col[Index], &IsInside);

			if(IsInside == 1)
			{
				tuple_concat (RowSelected, Row[Index], &RowSelected);
				tuple_concat (ColSelected, Col[Index], &ColSelected);

				if(Index != Length - 1)
					continue;
			}

			if(RowSelected.Num() == 0)
				continue;

			Hobject ContourNew;

			gen_contour_polygon_xld (&ContourNew, RowSelected, ColSelected);
			concat_obj (*pContoursIntersection, ContourNew, pContoursIntersection);

			RowSelected = HTuple();
			ColSelected = HTuple();
		}
	}

	return err;
}
