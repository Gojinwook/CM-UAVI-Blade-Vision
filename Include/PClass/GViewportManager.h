#ifndef _GVIEWPORTMANAGER_H_
#define _GVIEWPORTMANAGER_H_

#include "PBC.h"

class GViewportManager
{
  public:
    int miStartXPos;
    int miStartYPos;
    int miViewWidth;
    int miViewHeight;   

    double miRealOriginX;
    double miRealOriginY;
    double mdRealWidth;
    double mdRealHeight;
    int miRealViewWidth; // IP
    int miRealViewHeight;

    double mdZoomRatio;

    GViewportManager();
    ~GViewportManager();

    void GetImageWHPoint(POINT *pIPoint);
    void IPtoVP(POINT *pPoints, int iCount);
    void VPtoIP(POINT *pPoints, int iCount);     

    void RPtoIP(DPOINT *pDPoints, int iCount);
    void IPtoRP(DPOINT *pDPoints, int iCount);
};

#endif