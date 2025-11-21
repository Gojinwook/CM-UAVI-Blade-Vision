#include "stdafx.h"
#include "GViewportManager.h"

GViewportManager::GViewportManager()
{
	miStartXPos = 0;
	miStartYPos = 0;
	mdZoomRatio = 1.0;  
}

GViewportManager::~GViewportManager()
{
}

// Image 상의 점을 Viewport점으로 바꾸는 과정...
void GViewportManager::IPtoVP(POINT *pPoints, int iCount)
{
	for(int i = 0; i < iCount; i++) {
		pPoints[i].x = (int)((pPoints[i].x - miStartXPos) * mdZoomRatio + 0.5);
		pPoints[i].y = (int)((pPoints[i].y - miStartYPos) * mdZoomRatio + 0.5);
	}
}

// Viewport점에서 Image상의 점으로 바꾸는 과정...
void GViewportManager::VPtoIP(POINT *pPoints, int iCount)
{
	for (int i = 0; i < iCount; i++) {
		pPoints[i].x = (int)(pPoints[i].x / mdZoomRatio + miStartXPos + 0.5);
		pPoints[i].y = (int)(pPoints[i].y / mdZoomRatio + miStartYPos + 0.5);
	}
}

void GViewportManager::GetImageWHPoint(POINT *pIPoint)
{
	pIPoint->x = (long)((double)miViewWidth / mdZoomRatio + 0.5);
	pIPoint->y = (long)((double)miViewHeight / mdZoomRatio + 0.5);
}
