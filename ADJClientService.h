#pragma once
#include "TCPClient.h"
#include "StdAfx.h"

#include <opencv2/opencv.hpp>
#include <iostream>
#include <ws2tcpip.h>
#include <string>
#include <sstream> 

using namespace Halcon;

#define RECV_INSPECT_RESULT 1
#define RECV_WATCHDOG 2

// #pragma pack( push,1 ) : Memory Alignment, 구조체 멤버를 1byte 단위로 정렬
#pragma pack( push,1 )
struct stTCPPacket
{
	// Byte 배열로 고정 길이 패킷 구성 (PACKET_STRING_SIZE = 60)
	BYTE byteLotID[PACKET_STRING_SIZE];
	BYTE byteTrayNo[PACKET_STRING_SIZE];
	BYTE byteModuleNo[PACKET_STRING_SIZE];
	BYTE byteSuaModelID[PACKET_STRING_SIZE];
	BYTE byteIP[PACKET_STRING_SIZE];
	BYTE byteSuaResult[PACKET_STRING_SIZE];

	// ---------Used Olny In Header VARIABLES--------- - 250910, jhkim
	BYTE byteCommand[PACKET_STRING_SIZE];
	BYTE byteOperator[PACKET_STRING_SIZE];
	BYTE bytePort[PACKET_STRING_SIZE];
	BYTE byteFileName[PACKET_STRING_SIZE];
	BYTE byteImageWidth[PACKET_STRING_SIZE];
	BYTE byteImageHeight[PACKET_STRING_SIZE];
	BYTE *pByteSurfaceImageData;
	BYTE *pByteEdgeImageData;
	BYTE *pByteLens1ImageData;
	BYTE *pByteLens2ImageData;
	int nDataSize;
	// ------------------------------------------------

	//구조체 초기화.
	stTCPPacket()
	{
		memset(byteLotID, 0, PACKET_STRING_SIZE);
		memset(byteTrayNo, 0, PACKET_STRING_SIZE);
		memset(byteModuleNo, 0, PACKET_STRING_SIZE);
		memset(byteSuaModelID, 0, PACKET_STRING_SIZE);
		memset(byteIP, 0, PACKET_STRING_SIZE);
		memset(byteSuaResult, 0, PACKET_STRING_SIZE);
		// ---------Used Olny In Header VARIABLES--------- - 250910, jhkim
		memset(byteCommand, 0, PACKET_STRING_SIZE);
		memset(byteOperator, 0, PACKET_STRING_SIZE);
		memset(bytePort, 0, PACKET_STRING_SIZE);
		memset(byteFileName, 0, PACKET_STRING_SIZE);
		memset(byteImageWidth, 0, PACKET_STRING_SIZE);
		memset(byteImageHeight, 0, PACKET_STRING_SIZE);
		pByteSurfaceImageData = nullptr;
		pByteEdgeImageData = nullptr;
		pByteLens1ImageData = nullptr;
		pByteLens2ImageData = nullptr;
		nDataSize = 0;
		// ------------------------------------------------
	}

	//실제 패킷으로 합치는 메쏘드
	void MakeImagePacket(BYTE* pbyteEmpty, int iImageWidth, int iImageHeight)
	{
		int curIdx = 0;
		pbyteEmpty[curIdx] = '@';
		CString strDataSize = "";

		curIdx++;

		memcpy(this->byteCommand, (const char*)"INSPECT", PACKET_STRING_SIZE);
		memcpy(this->byteOperator, (const char*)"REQUEST", PACKET_STRING_SIZE);

		memcpy(&pbyteEmpty[curIdx], this->byteCommand, PACKET_STRING_SIZE);
		curIdx += PACKET_STRING_SIZE;

		pbyteEmpty[curIdx] = ',';
		curIdx++;

		memcpy(&pbyteEmpty[curIdx], this->byteOperator, PACKET_STRING_SIZE);
		curIdx += PACKET_STRING_SIZE;

		pbyteEmpty[curIdx] = ',';
		curIdx++;

		memcpy(&pbyteEmpty[curIdx], this->byteLotID, PACKET_STRING_SIZE);
		curIdx += PACKET_STRING_SIZE;

		pbyteEmpty[curIdx] = ',';
		curIdx++;

		memcpy(&pbyteEmpty[curIdx], this->byteTrayNo, PACKET_STRING_SIZE);
		curIdx += PACKET_STRING_SIZE;

		pbyteEmpty[curIdx] = ',';
		curIdx++;

		memcpy(&pbyteEmpty[curIdx], this->byteModuleNo, PACKET_STRING_SIZE);
		curIdx += PACKET_STRING_SIZE;

		pbyteEmpty[curIdx] = ',';
		curIdx++;

		memcpy(&pbyteEmpty[curIdx], this->byteSuaModelID, PACKET_STRING_SIZE);
		curIdx += PACKET_STRING_SIZE;

		pbyteEmpty[curIdx] = ',';
		curIdx++;

		memcpy(&pbyteEmpty[curIdx], this->byteIP, PACKET_STRING_SIZE);
		curIdx += PACKET_STRING_SIZE;

		pbyteEmpty[curIdx] = ',';
		curIdx++;

		memcpy(&pbyteEmpty[curIdx], this->bytePort, PACKET_STRING_SIZE);
		curIdx += PACKET_STRING_SIZE;

		pbyteEmpty[curIdx] = ',';
		curIdx++;

		memcpy(&pbyteEmpty[curIdx], this->byteFileName, PACKET_STRING_SIZE);
		curIdx += PACKET_STRING_SIZE;

		pbyteEmpty[curIdx] = ',';
		curIdx++;

		memcpy(&pbyteEmpty[curIdx], this->byteImageWidth, PACKET_STRING_SIZE);
		curIdx += PACKET_STRING_SIZE;

		pbyteEmpty[curIdx] = ',';
		curIdx++;

		memcpy(&pbyteEmpty[curIdx], this->byteImageHeight, PACKET_STRING_SIZE);
		curIdx += PACKET_STRING_SIZE;

		pbyteEmpty[curIdx] = ',';
		curIdx++;

		strDataSize.Format("%d", curIdx + 1 + PACKET_STRING_SIZE + (iImageWidth * iImageHeight * 4));
		memcpy(&pbyteEmpty[curIdx], strDataSize, PACKET_STRING_SIZE);
		curIdx += PACKET_STRING_SIZE;

		pbyteEmpty[curIdx] = ',';
		curIdx++;

		memcpy(&pbyteEmpty[curIdx], this->pByteSurfaceImageData, iImageWidth * iImageHeight);
		curIdx += iImageWidth * iImageHeight;

		memcpy(&pbyteEmpty[curIdx], this->pByteEdgeImageData, iImageWidth * iImageHeight);
		curIdx += iImageWidth * iImageHeight;

		memcpy(&pbyteEmpty[curIdx], this->pByteLens1ImageData, iImageWidth * iImageHeight);
		curIdx += iImageWidth * iImageHeight;

		memcpy(&pbyteEmpty[curIdx], this->pByteLens2ImageData, iImageWidth * iImageHeight);
		curIdx += iImageWidth * iImageHeight;

		nDataSize = curIdx;
	}

	int GetPacket(const BYTE* pByteMessage)
	{
		int curIdx = 1; //'@' -> start
		CString strCommand, strOperator;
		memcpy(this->byteCommand, &pByteMessage[curIdx], PACKET_STRING_SIZE);
		curIdx += PACKET_STRING_SIZE + 1; 

		memcpy(this->byteOperator, &pByteMessage[curIdx], PACKET_STRING_SIZE);
		curIdx += PACKET_STRING_SIZE + 1; 

		strCommand = (const char*)this->byteCommand;
		strOperator = (const char*)this->byteOperator;

		if (!strCommand.Compare("INSPECT") && !strOperator.Compare("REPLY"))
		{
			// NOTE : 1을 더해준 것은 세미콜론 처리를 위함.
			memcpy(this->byteLotID, &pByteMessage[curIdx], PACKET_STRING_SIZE);
			curIdx += PACKET_STRING_SIZE + 1;

			memcpy(this->byteTrayNo, &pByteMessage[curIdx], PACKET_STRING_SIZE);
			curIdx += PACKET_STRING_SIZE + 1;

			memcpy(this->byteModuleNo, &pByteMessage[curIdx], PACKET_STRING_SIZE);
			curIdx += PACKET_STRING_SIZE + 1;

			memcpy(this->byteSuaModelID, &pByteMessage[curIdx], PACKET_STRING_SIZE);
			curIdx += PACKET_STRING_SIZE + 1;

			memcpy(this->byteSuaResult, &pByteMessage[curIdx], PACKET_STRING_SIZE);
			curIdx += PACKET_STRING_SIZE + 1;

			memcpy(this->byteIP, &pByteMessage[curIdx], PACKET_STRING_SIZE);
			curIdx += PACKET_STRING_SIZE + 1;

			if (pByteMessage != nullptr)
				delete pByteMessage;

			return RECV_INSPECT_RESULT;
		}
		else
		{
			if (pByteMessage != nullptr)
				delete pByteMessage;

			return RECV_WATCHDOG;
		}
	}

	// Returns the total size of the data in the packet.
	int GetDataSize(void) { return nDataSize; }
};
// 구조체 멤버 Memory Alignment 원래대로 복구
#pragma pack( pop )

class CADJClientService : public CWnd
{
	DECLARE_DYNAMIC(CADJClientService)

public:
	CADJClientService();
	virtual ~CADJClientService();

	void Initialize(void);
	int  SendImagetoADJ(stTCPPacket stPacket, int nWaitMilsec, int iImageWidth, int iImageHeight);
	void ReStart(void);
	BOOL m_bConnect;
	BOOL m_bRealConnect;
	int m_nADJDefectCnt;

	// why "arr"Clinet? 
	// Maybe for future expansion to multiple ADJ clients? - 250910, jhkim
	CTCPClient m_arrClient;
	int m_nTimeOutCnt;
	CString m_strClientIP;
	BOOL m_bTCPThreadRun;
	void ThreeHandShakingRequest();
	void ADJResetRequest();
	int m_DeepLearningProcessing(string modelname, cv::Mat DLImg);

	//method for deeplearning inspection
	CString GetStringCamType(int iVisionCamType);

	//	CString GetSuaModelNameFromIndex(int nSuaModelID);
	//	int AssignSuaModel(int iClientNO, int iVisionCamType, CString sADJInspectionTypeCode, CString sADJDefectDetail);

	void ResetADJResult(int iSuaModelNO, int iModuleNo);
	void DoDeeplearningInspection(int iThreadNo, CString strLotID, int iTrayNo, int iModuleNo, CString sBarcodeID);
	
	BOOL IsADJRunning();
	BOOL ExecuteADJ();


	// Tcp alive check(2-hand-shake)
	void ADJConnectAliveCheck(void);

protected:
	DECLARE_MESSAGE_MAP()
};

static UINT TCPPostProcessing_Client1(LPVOID lParam);