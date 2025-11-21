#pragma once

#include "PBC.h"

class GViewportManager
{
public:
	GViewportManager();
	virtual ~GViewportManager();
	
	int miStartXPos;
	int miStartYPos;
	int miViewWidth;
	int miViewHeight;

	int miImageWidth;
	int miImageHeight;

	double mdZoomRatio;

	void IPtoVP(POINT *pPoints, int iCount);
	void VPtoIP(POINT *pPoints, int iCount);

	void GetImageWHPoint(POINT *pIPoint);
};
