// GFunction.cpp : implementation file
//

#include "stdafx.h"
#include "uScan.H"
#include "GFunction.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CGFunction

CGFunction* CGFunction::m_pInstance = NULL;

CGFunction* CGFunction::GetInstance(BOOL bShowFlag)
{
	if (!m_pInstance) {
		m_pInstance = new CGFunction();
		if (!m_pInstance->m_hWnd) {
			CRect r = m_pInstance->GetPosition();
			m_pInstance->CreateEx(0, AfxRegisterWndClass(0), "CGFunction", 0, r, NULL, 0, NULL);
		}
	}
	return m_pInstance;
}

void CGFunction::DeleteInstance()
{
	if(m_pInstance->m_hWnd)
		m_pInstance->DestroyWindow();

	SAFE_DELETE(m_pInstance);
}

CGFunction::CGFunction()
{
}

CGFunction::~CGFunction()
{
}

BOOL CGFunction::ValidHImage(const Hobject &rHObject)
{
	try
	{
		Hlong lObjectNum = 0;
		count_obj(rHObject, &lObjectNum);

		if (rHObject.Id() != H_EMPTY_REGION && lObjectNum > 0)
		{
			HTuple HObjectNum;
			count_obj(rHObject, &HObjectNum);

			if (HObjectNum > 0)
			{
				return TRUE;
			}
		}

		return FALSE;
	}
	catch (HException &except)
	{
		return FALSE;
	}
}

BOOL CGFunction::ValidHRegion(const Hobject &rHObject)
{
	try
	{
		Hlong lObjectNum = 0;
		count_obj(rHObject, &lObjectNum);

		if (rHObject.Id() != H_EMPTY_REGION && lObjectNum > 0)
		{
			HTuple Area, Row, Column;

			area_center(rHObject, &Area, &Row, &Column);

			for (int i = 0; i < lObjectNum; i++)
			{
				if (Area[i].L() > 0)
					return TRUE;
			}
		}

		return FALSE;
	}
	catch (HException &except)
	{
		return FALSE;
	}
}

BOOL CGFunction::ValidHXLD(const Hobject &rHObject)
{
	try
	{
		Hlong lObjectNum = 0;
		count_obj(rHObject, &lObjectNum);

		if (rHObject.Id() != H_EMPTY_REGION && lObjectNum > 0)
		{
			HTuple ContLength;

			length_xld(rHObject, &ContLength);

			if (ContLength > 0)
				return TRUE;
		}

		return FALSE;
	}
	catch (HException &except)
	{
		return FALSE;
	}
}


void CGFunction::DisplayCross(HTuple lWindowHandle, double dXPos, double dYPos, COLORREF Color, double dRadius)
{
	set_rgb(lWindowHandle, GetRValue(Color),GetGValue(Color),GetBValue(Color));

	disp_line(lWindowHandle, dYPos, dXPos-dRadius, dYPos, dXPos+dRadius);
	disp_line(lWindowHandle, dYPos-dRadius, dXPos, dYPos+dRadius, dXPos);
}

void CGFunction::DisplayCross(HDC hDestDC, GViewportManager *pVManager, double dXPos, double dYPos, COLORREF Color, long lRadiusPxl)
{
	POINT CPoint, sPoint;
	POINT Left, Right, Top, Bottom;
	
	CPoint.x = (long)(dXPos+0.5);
	CPoint.y = (long)(dYPos+0.5);

	Left.x = CPoint.x - lRadiusPxl;
	Left.y = CPoint.y;
	Right.x = CPoint.x + lRadiusPxl;
	Right.y = CPoint.y;
	Top.x = CPoint.x;
	Top.y = CPoint.y - lRadiusPxl;
	Bottom.x = CPoint.x;
	Bottom.y = CPoint.y + lRadiusPxl;

	pVManager->IPtoVP(&Left, 1);
	pVManager->IPtoVP(&Right, 1);
	pVManager->IPtoVP(&Top, 1);
	pVManager->IPtoVP(&Bottom, 1);

	HPEN hOldPen, hPen;
	hPen = CreatePen(PS_SOLID, 1, Color);
	hOldPen = (HPEN)SelectObject(hDestDC, hPen);

	MoveToEx(hDestDC,Left.x,Left.y,&sPoint);
	LineTo(hDestDC,Right.x,Right.y);

	MoveToEx(hDestDC,Top.x,Top.y,&sPoint);
	LineTo(hDestDC,Bottom.x,Bottom.y);

	SelectObject(hDestDC, hOldPen);
	DeleteObject(hPen);
}

BOOL CGFunction::GetIntersectionRegion(Hobject HInputRgn, Hobject HTestRgn, Hobject* pHIntersectRgn, long lMinCheckSize)
{
	pHIntersectRgn->Reset();
	gen_empty_obj(pHIntersectRgn);

	Hobject HConnectedRgn;
	connection(HInputRgn, &HConnectedRgn);
	select_shape(HConnectedRgn, &HConnectedRgn, "area", "and", lMinCheckSize, MAX_DEF);

	Hlong lCount = 0;
	count_obj(HConnectedRgn, &lCount);
	if (lCount<=0)
		return FALSE; 

	Hobject HTempRgn;
	Hlong lArea;
	double dRow, dCol;
	
	for (int i=0; i<lCount; i++)
	{
		select_obj(HConnectedRgn, &HTempRgn, i+1);
		intersection(HTempRgn, HInputRgn, &HTempRgn);

		lArea = 0;
		area_center(HTempRgn, &lArea, &dRow, &dCol);

		if (lArea>0)
		{
			*pHIntersectRgn = HTempRgn;
			return TRUE;
		}
	}

	return FALSE;
}

void CGFunction::ScaleImageRange (Hobject Image, Hobject *ImageScaled, HTuple Min, HTuple Max)
{
	// Local iconic variables 
	Hobject  SelectedChannel, LowerRegion, UpperRegion;


	// Local control variables 
	HTuple  LowerLimit, UpperLimit, Mult, Add, Channels;
	HTuple  Index, MinGray, MaxGray, ExpDefaultCtrlDummyVar;

	//Convenience procedure to scale the gray values of the
	//input image Image from the interval [Min,Max]
	//to the interval [0,255] (default).
	//Grey values < 0 or > 255 (after scaling) are clipped.
	//
	//If the image shall be scaled to an interval different from [0,255],
	//this can be achieved by passing tuples with 2 values [From, To]
	//as Min and Max.
	//Example:
	//scale_image_range(Image:ImageScaled:[100,50],[200,250])
	//maps the gray values of Image from the interval [100,200] to [50,250].
	//All other gray values will be clipped.
	//
	//input parameters:
	//Image: the input image
	//Min: the minimum gray value which will be mapped to 0
	//     If a tuple with two values is given, the first value will
	//     be mapped to the second value.
	//Max: The maximum gray value which will be mapped to 255
	//     If a tuple with two values is given, the first value will
	//     be mapped to the second value.
	//
	//output parameter:
	//ImageScale: the resulting scaled image
	//
	if ((Min.Num())==2)
	{
		LowerLimit = Min[1];
		Min = Min[0];
	}
	else
	{
		LowerLimit = 0.0;
	}
	if ((Max.Num())==2)
	{
		UpperLimit = Max[1];
		Max = Max[0];
	}
	else
	{
		UpperLimit = 255.0;
	}
	//
	//Calculate scaling parameters
	Mult = ((UpperLimit-LowerLimit).Real())/(Max-Min);
	Add = ((-Mult)*Min)+LowerLimit;
	//
	//Scale image
	scale_image(Image, &Image, Mult, Add);
	//
	//Clip gray values if necessary
	//This must be done for each channel separately
	count_channels(Image, &Channels);
	for (Index=1; Index<=Channels; Index+=1)
	{
		access_channel(Image, &SelectedChannel, Index);
		min_max_gray(SelectedChannel, SelectedChannel, 0, &MinGray, &MaxGray, &ExpDefaultCtrlDummyVar);
		threshold(SelectedChannel, &LowerRegion, (MinGray.Concat(LowerLimit)).Min(), 
			LowerLimit);
		threshold(SelectedChannel, &UpperRegion, UpperLimit, (UpperLimit.Concat(MaxGray)).Max());
		paint_region(LowerRegion, SelectedChannel, &SelectedChannel, LowerLimit, "fill");
		paint_region(UpperRegion, SelectedChannel, &SelectedChannel, UpperLimit, "fill");
		if (Index==1)
		{
			copy_obj(SelectedChannel, &(*ImageScaled), 1, 1);
		}
		else
		{
			append_channel((*ImageScaled), SelectedChannel, &(*ImageScaled));
		}
	}
	return;
}

DRECT::DRECT()
{
}

DRECT::~DRECT()
{
}

BOOL DRECT::PtInRect(DPOINT dPoint)
{
	if (dPoint.x>=left && dPoint.x<=right && dPoint.y>=top && dPoint.y<=bottom)
		return TRUE;
	else 
		return FALSE;
}

BEGIN_MESSAGE_MAP(CGFunction, CWnd)
	//{{AFX_MSG_MAP(CGFunction)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CGFunction message handlers

