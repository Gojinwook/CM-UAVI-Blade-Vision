#ifndef _PCALIBRATOR_H_
#define _PCALIBRATOR_H_

#include "GTRegion.h"
#include "PList.hpp"
#include "PMatrix.hpp"

#include "halconcpp.h"

#define PCP_CROSS_LENGTH     10

#define PCP_T_PRESENCE_POINT  1
#define PCP_T_MAKE_POINT      2

class PCalPoint
{
  public:
    int miType;
    double mdXp;
    double mdYp;        

    PCalPoint();
    ~PCalPoint();   
};

#define PC_T_BALLPLATE    1
#define PC_T_SQUAREPLATE  2

class PCalibrator
{ 
  public:
    int miType;           
    double mdOriginXp;
    double mdOriginYp;    
    double mdAngle;
    
    double mdFOVWidthm;
    double mdFOVHeightm;
    double mdWPixelSizem;
    double mdHPixelSizem;
    double mdPixelSizem;
    double mdPitchm;
    GTRegion mFOVRegion;

    PContentList<PCalPoint> mCalPointList;
    PPointerMatrix<PCalPoint> mCalPointMatrix;    

    PCalibrator();
    ~PCalibrator();

    void Duplicate(PCalibrator **ppCalibrator);

    void InitCameraParam(char *cpCameraParamFile, int iCameraSequence);
    void RunCalibration();

    bool IsValid();
    void ClearAll();

    int GetCalPointIndex(PCalPoint *pCalPoint);

    void MakeCalPoint(Hobject &HCalImage, int iType);
    bool MakeCalPointMatrix(Hobject &HCalImage, double dPitch);
    void Draw(HDC hDC, GViewportManager *pVManager);
    
    void IPtoRP(double dXp, double dYp, double *dpXm, double *dpYm);
    void RPtoIP(double dXp, double dYp, double *dpXm, double *dpYm);
    void ILtoRL(double dLengthp, double *dpLengthm);
    void RLtoIL(double dLengthm, double *dpLengthp);
    double GetMeanError();

    void Save(HANDLE hFile);
    void Load(HANDLE hFile);

    void SaveFile(char *cpFileName);
    void LoadFile(char *cpFileName);
};


#endif