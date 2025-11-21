#ifndef _PBASEREGION_H_
#define _PBASEREGION_H_

#include "PBC.h"

class
//#ifdef _MAKEDLL
  AFX_EXT_CLASS
//#endif
PBaseRegion
{
  protected:
    void AsmThreshold8(void *vpDest, void *vpSrc, void *vpMask, int iWidth, int iHeight, 
      int iAddDPos, int iAddSPos, int iMin, int iMax);
    void AsmRgnBlt24(void *vpDest, void *vpSrc, int iWidth, int iHeight, 
      int iAddDPos, int iAddSPos, unsigned int iColor);
  
  public:
    PStringList mTimes;

    PBaseRegion();
    ~PBaseRegion();
};

#endif;
