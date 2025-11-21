#ifndef _PMAPFILECOMM_H_
#define _PMAPFILECOMM_H_

#include "PArray.hpp"

class
//#ifdef _MAKEDLL
  AFX_EXT_CLASS
//#endif
PMapFileComm
{
  public:
    HANDLE mhMFile;
    void *mvpMMem;
    
    int miConnectionPage;
    int miPageSize;
    int miPageCount;
    int miBlockCount;
    PArray<bool> mConnectionState;

    PMapFileComm();
    ~PMapFileComm();

    void Create(char *cpMFileName, int iPageSize, int iPageCount, int iBlockCount);    
    bool OpenPage(int iPage);
    bool ClosePage(int iPage);
    void *GetBlock(int iPage, int iBlock);
    bool ConnectMapFileOpenPage(char *cpMFileName);
    void Close();

    void AddString(int iPage, int iBlock, char *cpMsgString);
    void DeleteString(int iPage, int iBlock, char *cpMsgString);
    void WriteString(int iPage, int iBlock, char *cpMsgString);
    char *ReadString(int iPage, int iBlock);
    bool WaitString(int iPage, int iBlock, char *cpMsgString, int iTimeOut);   
};


#endif