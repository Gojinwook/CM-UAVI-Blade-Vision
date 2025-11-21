#ifndef _GLEADFRAME_H_
#define _GLEADFRAME_H_

#include "PArray.hpp"
#include "PList.hpp"
#include "PBC.h"

#include "halconcpp.h"

// system option
#define GTD_SS_LEFTTOP        0
#define GTD_SS_RIGHTTOP       1
#define GTD_SS_LEFTBOTTOM     2  
#define GTD_SS_RIGHTBOTTOM    3

#define GTD_SD_HORIZONTAL     0
#define GTD_SD_VERTICAL       1

#define GTD_SM_REPEAT         0
#define GTD_SM_ZIGZAG         1

#define GTD_MSCD_LEFT           0
#define GTD_MSCD_TOP            1
#define GTD_MSCD_RIGHT          2
#define GTD_MSCD_BOTTOM         3
// system option end

#define GTD_TIF_EMPTY                  0
#define GTD_TIF_GRAB_COMPLETE          1 
#define GTD_TIF_INSPECTION_COMPLETE    2

class GTriggerData
{ 
  public:
    PArray<bool> mIStripFlagTable;
    PArray<int> mIStripIndexTable;

    int miSX;
    int miSY;
    int miXCount;
    int miYCount;

    bool mbIResult;

    Hobject mHTestImage;
    int miTestImageFlag;

    GTriggerData();
    ~GTriggerData();

    void Duplicate(GTriggerData **ppTriggerData);    

    void CreateIStripTable(int iXCount, int iYCount, int iMSCDirection);
    bool GetIStripFlag(int iX, int iY);
    void SetIStripFlag(int iX, int iY, bool bInspection);
    int GetIStripIndex(int iX, int iY);
    void SetIStripIndex(int iX, int iY, int iIStripIndex);
    void GetTableIndex(int iIStripIndex, int *ipX, int *ipY);   

    void Save(HANDLE hFile);
    void Load(HANDLE hFile);
};


#define GLF_DM_TRIGGER       1
#define GLF_DM_IRESULTS      2

// system option
#define GLF_DD_ORIGINAL      0
#define GLF_DD_M90           1
#define GLF_DD_P90           2
// system option end

#define GLF_RF_EMPTY        -2
#define GLF_RF_GOOD         -1

// interface option 
#define GLF_LT1_GOOD_LF                0
#define GLF_LT1_ERROR_LF               1
#define GLF_LT1_GOOD_ERROR_LF          2

#define GLF_LT2_RESULT_ONLY            0
#define GLF_LT2_RESULT_IMAGE_MEMORY    1
#define GLF_LT2_RESULT_IMAGE_FILE      2

#define GLF_LT3_ERROR_IMAGE            0
#define GLF_LT3_ALL_IMAGE              1

struct GLogIResult
{  
  PArray<int> mIResults;  
};

struct GLogImage
{
  int miLogLFIndex;
  int miTrigger;
  Hobject mHLogImage;
};

class GLeadFrame
{
public://2002/09/12 KJW
    PArray<bool> mValidTable;
    PArray<int> mIResults;
    PArray<unsigned short> mIResultFlags;
    PArray<int> mDrawIResults;
    PContentList<GLogIResult> mLogIResultList;
    PContentList<GLogImage> mLogImageList;

    friend class GLeadFrame;

  public:
    PContentList<GTriggerData> mTriggerDataList;

		//2002/09/12 KJW
		void GetIResultsTrigger(int iTriggerIndex, int iIStripIndex, unsigned short *sIResultFlag);

    int miXCount;
    int miYCount;

    int miPkgXCount;
    int miPkgYCount;
    int miBlankCount;
    int miBlockCount;

    int miCurTrigger;   

    PString mTrainDataFileName;
    unsigned short msHour;
    unsigned short msMinute;
    unsigned short msSecond;
    unsigned short msMilliSecond;

    GLeadFrame();
    ~GLeadFrame();

    void Duplicate(GLeadFrame **ppLeadFrame);

    void ClearAll();

    int GetLogIResultCount();
    void AddLogIResult(char *cpLogLFImageDir, int iLogType1, int iLogType2, int iLogType3, int iLogLimit);    
    void DeleteFirstLogIResult();
    void ClearAllLogIResult(char *cpLogLFImageDir);
    GLogIResult *GetLogIResult(int iIndex);
    void UpdateLogIResultToDraw(int iIndex);    

    int GetLogImageCount();
    void AddLogImage(GLogImage *pLogImage);
    void DeleteLogImage(int iIndex);
    void ClearAllLogImage();
    GLogImage *GetLogImage(int iIndex);
    bool GetLogHImage(char *cpLogLFImageDir, int iLogType2, int iLogLFIndex, int iTrigger, Hobject *pHImage);

    void CreateLeadFrame(int iPkgXCount, int iPkgYCount, int iBlankCount, int iBlockCount);
    bool GetValid(int iX, int iY);
    void SetValid(int iX, int iY, bool bValid);
    bool GetIResults(int iX, int iY); 
    unsigned short GetIResultFlags(int iX, int iY);    
    void SetIResults(int iX, int iY, int iIResult, unsigned short sIResultFlag);
    bool GetValidTrigger(int iTriggerIndex, int iIStripIndex);
    void SetIResultsTrigger(int iTriggerIndex, int iIStripIndex, int iIResult, unsigned short sIResultFlag);
    void GetSendIResults(PArray<bool> &rSendIResults, int iSendIResultsIndex, 
                         int iRSStart, int iRSDirection, int iRSMethod, int iRSSendCount);
    void GetSendIResultFlags(PArray<unsigned short> &rSendIResultFlags, int iSendIResultsIndex,
                         int iRSStart, int iRSDirection, int iRSMethod, int iRSSendCount);
    void ResetIResults();
    void UpdateDrawIResults();

    void CreateTriggerData(int iMSStart, int iMSDirection, int iMSMethod, 
                                   int iIStripXCount, int iIStripYCount, int iMSCDirection);

    int GetInspectionTestImageIndex();
    void ClearTestImageFlag();
    int GetTriggerIndexPoint(POINT Point, int iSX, int iSY, int iWidth, int iHeight, int iDrawDirection, int *ipIStrip);
    unsigned short GetIResultFlagPoint(POINT Point, int iSX, int iSY, int iWidth, int iHeight, int iDrawDirection);

    void Draw(HDC hDC, int iSX, int iSY, int iWidth, int iHeight, int iDrawMode, int iDrawDirection, PArray<COLORREF> &rErrorColors);

    void Save(HANDLE hFile);
    void Load(HANDLE hFile);

    void SaveIResultFlag(HANDLE hFile, char *cpTrainDataFileName);
    void LoadIResultFlag(HANDLE hFile);
};

#endif