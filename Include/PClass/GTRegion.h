#ifndef _GTREGION_H_
#define _GTREGION_H_

#include "PBC.h"
#include "GViewportManager.h"
#include "PList.hpp"
#include "halconcpp.h"

//Region Style
#define GTR_RS_FILL        1  
#define GTR_RS_NULL        2

// Draw Style
#define GTR_DS_NORMAL      0
#define GTR_DS_ACTIVE      1

// Control Point Half Length
#define GTR_CPHL           4

// Control Point Index
#define GTR_CP_NONE       -3
#define GTR_CP_SEGMENT    -2
#define GTR_CP_REGION     -1
#define GTR_CP_LT          0
#define GTR_CP_T           1 
#define GTR_CP_RT          2
#define GTR_CP_R           3
#define GTR_CP_RB          4
#define GTR_CP_B           5
#define GTR_CP_LB          6
#define GTR_CP_L           7

//2002/02/23 KJW 주석을 달았음... 
class GTRegion : public PObject //dynclass때문에 PObject에서 파생되었다.
{
  DECLARE_DYNCLASS(GTRegion)

  protected:
    bool mbVisible;
    bool mbSelectable;
    bool mbMovable;
    bool mbSizable;
    bool mbSelect;        

    bool PtInCPoint(int iVX, int iVY, POINT VPoint);
    bool PtInSegment(POINT VPoint, GViewportManager *pVManager);    
    bool PtInRegion(POINT VPoint, GViewportManager *pVManager);

    PList<GTRegion> *mpChildTRegionList;

  public:                
    GTRegion *mpParent;
     
    int miLineStyle; 
    int miLineThickness;
    COLORREF mLineColor;
    COLORREF mBackColor;
    int miBkMode;
    int miRegionStyle;
    COLORREF mRegionColor;
    DPOINT maPoints[2];    
    
    GTRegion();
    ~GTRegion();

    void GetLTPoint(POINT *pIPoint);
    void GetRBPoint(POINT *pIPoint);
    void GetWHPoint(POINT *pIPoint);
    void GetCenterPoint(POINT *pIPoint);
    void GetCenterDistance(GTRegion *pTRegion, POINT *pPoint);    
    void SetWHPoint(POINT IPoint);
    void SetCenterPosition(int iX, int iY);
    void ArrangePoints();

    int GetChildTRegionCount();
    void AddChildTRegion(GTRegion *pTRegion);
    void InsertChildTRegion(int iIndex, GTRegion *pTRegion);
    void DeleteChildTRegion(int iIndex);
    void DeleteChildTRegionPtr(GTRegion *pTRegion);
    void ClearAllChildTRegion();
    void ClearAllSelectChildTRegion();
    GTRegion *GetChildTRegion(int iIndex);
    void GetChildTRegionList(PList<GTRegion> *pTRegionList, char *cpClassName);

    void GetHRegion(Hobject *pHRegion, int iDX = 0, int iDY = 0);
    void SetHRegion(Hobject &HRegion);
    bool RgnInRegion(GTRegion *pTRegion);
    void DrawControlPoint(HDC hDestDC, int iVX, int iVY);

    bool GetVisible();
    bool GetSelectable();
    bool GetMovable();
    bool GetSizable();
    bool GetSelect();
    
    void SetVisible(bool bVisible, bool bChildLink);
    void SetSelectable(bool bSelectable, bool bChildLink);
    void SetMovable(bool bMovable, bool bChildLink);
    void SetSizable(bool bSizable, bool bChildLink);
    void SetSelect(bool bSelect, bool bChildLink);
    void ClearAllSelect();
    
    virtual void Duplicate(GTRegion **ppTRegion);
    virtual GTRegion *GetTopTRegion(POINT VPoint, GViewportManager *pVManager);
    virtual int GetCPointIndex(POINT VPoint, GViewportManager *pVManager);
    virtual void SetCPoint(int iCPointIndex, POINT *pMIPoints);

    virtual void Move(double dXMove, double dYMove, bool bChildLink);
    virtual void Scale(POINT ILTPoint, double dXScale, double dYScale, bool bChildLink);
    
    virtual void Draw(HDC hDestDC, GViewportManager *pVManager, int iDrawStyle, bool bDrawControlPoint);

    virtual void Save(HANDLE hFile);
    virtual void Load(HANDLE hFile);

    void SaveList(HANDLE hFile);
    void SaveListData(char *cpFileName);
};

void BltImage(Hobject &HDestImage, Hobject &HSrcImage, int iX, int iY);

#endif