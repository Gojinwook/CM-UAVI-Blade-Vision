// StrobeData.h: interface for the CStrobeData class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_STROBEDATA_H__D21CD6D8_85DD_4444_8E6D_7FCF07D18922__INCLUDED_)
#define AFX_STROBEDATA_H__D21CD6D8_85DD_4444_8E6D_7FCF07D18922__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define STROBE_STX					0x02 //시작 문자
#define STROBE_ETX					0x03 //끝문자
#define STROBE_WRITE				'W' //Write 명령어
#define STROBE_WRITE_ACK			'w' //Write 명령어에 대한 reponse
#define STROBE_READ					'R' //Read 명령어
#define STROBE_READ_ACK				'r' //Read 명령어에 대한 response
#define STROBE_CONTROL				'T' //Control 명령어
#define STROBE_CONTROL_ACK			't' //Control 명령어에 대한 response

#define STROBE_CHANNEL_NUM			4 //RYU 2004/1/29
#define MAX_DISTANCEOFNUM_COUNT	3
#define MAX_TERM_LENGTH					STROBE_CHANNEL_NUM*11 + 1

static int DistanceOfNum[MAX_DISTANCEOFNUM_COUNT] = {1, 5, 5};

typedef struct StrobeData
{
	int iMode;
	int iTriggerType[4];
	int iDelayTime[4];
	int iOnTime[4];
} StrobeData;

class CStrobeData  
{
public:
	CStrobeData();
	virtual ~CStrobeData();
	static CStrobeData* GetInstance(BOOL bShowFlag=FALSE);
	void DeleteInstance();
	bool LoadFile(char *cpFileName); //RYU 2004/1/29
	void Load(HANDLE hFile); //RYU 2004/1/29
	void Save(HANDLE hFile);
	bool ReadROM(int iPortNum); //RYU 2004/1/29
	bool WriteROM(int iPortNum); //RYU 2004/1/29
	void Reset();
	
	StrobeData mData;
	static CStrobeData * m_pInstance;
	
protected:
	bool DecodeString(CString &Source);
	CString EncodeString();
	int miReadDataNum;
	HTuple mHSerialHandle;
	int miTimeOut;
	bool mbPortOpen;
	int miPortNum;
	
	bool OpenPort();
	void ClosePort();
	char CheckSum(char *pString);
};

#endif // !defined(AFX_STROBEDATA_H__D21CD6D8_85DD_4444_8E6D_7FCF07D18922__INCLUDED_)
