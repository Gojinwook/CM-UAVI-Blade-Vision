#ifndef _PREGION_H_
#define _PREGION_H_

#include "PBaseRegion.h"
#include "PImage.h"
#include "PList.hpp"

class 
#ifdef _MAKEDLL
  AFX_EXT_CLASS
#endif
PRegion : public PBaseRegion
{
  private:    
    int miX;
    int miY;
    PImage mRgnImage;

    PContentList<PRegion> mRegionList;

    void MakeValidRect(BITMAP *pDestBM, BITMAP *pSrcBM, RECT *pDestRect, RECT *pSrcRect);

    friend class PRegion;

  public:
    PRegion();
    ~PRegion();

    int GetRegionCount();
    void AddRegion(PRegion *pRegion);
    void InsertRegion(int iIndex, PRegion *pRegion);
    void DeleteRegion(int iIndex);
    void ClearAllRegion();
    PRegion *GetRegion(int iIndex);

    bool IsValid();
    void Clear();

    void CreateRect1(int iX1, int iY1, int iX2, int iY2);
    void CreateRect2(int iCX, int iCY, double dAngle, int iWidth, int iHeight);

    void Threshold(PImage &rDestImage, PRegion &rMaskRegion, int iMin, int iMax);
    void Connection(PRegion *pRegion, PImage &rDImage);

    void Display(PImage &rDestImage, COLORREF Color);
};

#endif;
