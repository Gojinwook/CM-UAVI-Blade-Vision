// StrobeData.cpp: implementation of the CStrobeData class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "StrobeData.h"

#ifdef _DEBUG
#undef THIS_FILE
static char* THIS_FILE=__FILE__;
#define new DEBUG_NEW
#endif

CStrobeData* CStrobeData::m_pInstance = NULL;

CStrobeData* CStrobeData::GetInstance(BOOL bShowFlag)
{
	if (!m_pInstance) {
		m_pInstance = new CStrobeData();
	}
	return m_pInstance;
}

void CStrobeData::DeleteInstance()
{
	if(m_pInstance) delete m_pInstance;
		m_pInstance = NULL;
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CStrobeData::CStrobeData()
{
	Reset();
}

CStrobeData::~CStrobeData()
{

}

void CStrobeData::Reset()
{
	for(int i = 0 ; i < STROBE_CHANNEL_NUM ; i++)
	{
		mData.iMode = 1;
		mData.iTriggerType[i] = 1;
		mData.iDelayTime[i] = 0;
		mData.iOnTime[i] = 0;
	}

	mHSerialHandle.Reset();
	miTimeOut = 100;
	miReadDataNum = 100;
	mbPortOpen = FALSE;
	miPortNum = 1;
}

void CStrobeData::Load(HANDLE hFile)
{
	DWORD dwBytesRead;

	ReadFile(hFile, &mData.iMode, sizeof(int), &dwBytesRead, NULL);

	for(int i = 0 ; i < STROBE_CHANNEL_NUM ; i++)
	{
		ReadFile(hFile, &mData.iTriggerType[i], sizeof(int), &dwBytesRead, NULL);
		ReadFile(hFile, &mData.iDelayTime[i], sizeof(int), &dwBytesRead, NULL);
		ReadFile(hFile, &mData.iOnTime[i], sizeof(int), &dwBytesRead, NULL);
	}
}

void CStrobeData::Save(HANDLE hFile)
{
	DWORD dwBytesWritten;

	WriteFile(hFile, &mData.iMode, sizeof(int), &dwBytesWritten, NULL);

	for(int i = 0 ; i < STROBE_CHANNEL_NUM ; i++)
	{
		WriteFile(hFile, &mData.iTriggerType[i], sizeof(int), &dwBytesWritten, NULL);
		WriteFile(hFile, &mData.iDelayTime[i], sizeof(int), &dwBytesWritten, NULL);
		WriteFile(hFile, &mData.iOnTime[i], sizeof(int), &dwBytesWritten, NULL);
	}
}

bool CStrobeData::WriteROM(int iPortNum)
{
	HTuple HData;
	CString ReadData, ReadDataWithoutCheckSum;
	bool bWritten = FALSE;
	int iCount = 0;

	miPortNum = iPortNum;

	OpenPort();

	if(!mbPortOpen)
	{
		CString message;
		message.Format("Fail to open the serial port!\nPlease check the COM%d port.\nIllumination settings will not be changed.", miPortNum);
		AfxMessageBox(message);
		return FALSE;
	}

	CString StringToSend = EncodeString();

	while(!bWritten)
	{
		write_serial(mHSerialHandle, HTuple(StringToSend).Ords());
		read_serial(mHSerialHandle, miReadDataNum, &HData);

//		int length = ReadData.GetLength();
		int length = HData.Num();

		if(length == 5) //RYU 2003/12/29 응답 문자 길이는 5이다
		{
			ReadData = (LPCTSTR)(HData.Chrt())[0].S();

			ReadDataWithoutCheckSum = ReadData.Left(length - 1);

			if(ReadData[1] == STROBE_WRITE_ACK &&
				ReadData[2] == '1' &&
				ReadData[4] == CheckSum(ReadDataWithoutCheckSum.GetBuffer(0)))
				bWritten = TRUE;
		}

		if(++iCount >= 3)
			break;
	}

	ClosePort();

	return bWritten;
}

bool CStrobeData::OpenPort()
{
	HTuple PortName;

	switch(miPortNum)
	{
	case 1:
		PortName = HTuple("COM1");
		break;
	case 2:
		PortName = HTuple("COM2");
		break;
	case 3:
		PortName = HTuple("COM3");
		break;
	case 4:
		PortName = HTuple("COM4");
		break;
	default :
		PortName = HTuple("COM1");
		break;
	}

	if(open_serial(PortName, &mHSerialHandle) != H_MSG_TRUE)
	{
		mbPortOpen = FALSE;
		return FALSE;
	}

	set_serial_param(mHSerialHandle, 9600, 8, "none", "none", 1, miTimeOut, "unchanged");
	mbPortOpen = TRUE;

	return TRUE;
}

void CStrobeData::ClosePort()
{
	close_serial(mHSerialHandle);
	mbPortOpen = FALSE;
}

char CStrobeData::CheckSum(char *pString)
{
	char ret = pString[0];

	for(int i = 1 ; i < (int) strlen(pString) ; i++)
		ret ^= pString[i];

	return ret;
}

bool CStrobeData::ReadROM(int iPortNum)
{
	HTuple HData;
	CString ReadData, ReadDataWithoutCheckSum;
	bool bRead = FALSE;
	int iCount = 0;

	miPortNum = iPortNum;

	OpenPort();

	if(!mbPortOpen)
	{
		CString message;
		message.Format("Fail to open the serial port!\nPlease check the COM%d port.\nIllumination settings will not be saved.", miPortNum);
		AfxMessageBox(message);
		return FALSE;
	}

	CString StringToSend;
	StringToSend.Format("%c%c%c", STROBE_STX, STROBE_READ, STROBE_ETX);

	StringToSend += CheckSum((char *)StringToSend.GetBuffer(0));

	while(!bRead)
	{
		write_serial(mHSerialHandle, HTuple(StringToSend).Ords());
		read_serial(mHSerialHandle, miReadDataNum, &HData);

//		int length = ReadData.GetLength();
		int length = HData.Num();

		if(length >= 4) //RYU 2003/12/29 응답 문자 길이는 최소 4이다
		{
			ReadData = (LPCTSTR)(HData.Chrt())[0].S();

			ReadDataWithoutCheckSum = ReadData.Left(length - 1);

			if(ReadData[length - 1] == CheckSum(ReadDataWithoutCheckSum.GetBuffer(0)) &&
				ReadData[1] == STROBE_READ_ACK)
			{
				if(!DecodeString(ReadData))
					bRead = FALSE;
				else
					bRead = TRUE;
			}
		}

		if(++iCount >= 3)
			break;
	}

	ClosePort();

	return bRead;
}

CString CStrobeData::EncodeString()
{
	CString StringToSend;
	CString Parameters = "";
	CString String;

	String.Format("%d", mData.iMode);
	Parameters += String;

	for(int i = 0 ; i < 4 ; i++)
	{
		String.Format("%d%05d%05d", mData.iTriggerType[i], mData.iDelayTime[i], mData.iOnTime[i]);
		Parameters += String;
	}

	StringToSend.Format("%c%c%s%c", STROBE_STX, STROBE_WRITE, Parameters, STROBE_ETX);

	StringToSend += CheckSum((char *)StringToSend.GetBuffer(0));

	return StringToSend;
}

bool CStrobeData::DecodeString(CString &ReadData)
{
	bool bResult;

	int length = ReadData.GetLength();

	ReadData.SetAt(length - 1, 0);
	ReadData.Remove(STROBE_READ_ACK);
	ReadData.Remove(STROBE_STX);
	ReadData.Remove(STROBE_ETX);

	char temp[MAX_TERM_LENGTH];

	memset(temp, 0, MAX_TERM_LENGTH);
	memcpy(temp, ReadData.GetBuffer(0), length);

	if(strlen(temp) == MAX_TERM_LENGTH)
		bResult = TRUE;
	else
	{
		bResult = FALSE;
		return bResult;
	}

	int i = STROBE_CHANNEL_NUM - 1;

	do {
		mData.iOnTime[i]			= atoi(&temp[strlen(temp) - DistanceOfNum[2]]);
		temp[strlen(temp) - DistanceOfNum[2]] = 0;
		mData.iDelayTime[i]		= atoi(&temp[strlen(temp) - DistanceOfNum[1]]);
		temp[strlen(temp) - DistanceOfNum[1]] = 0;
		mData.iTriggerType[i]	= atoi(&temp[strlen(temp) - DistanceOfNum[0]]);
		temp[strlen(temp) - DistanceOfNum[0]] = 0;
	} while(i--);

	mData.iMode = atoi(&temp[0]);

	return bResult;
}

bool CStrobeData::LoadFile(char *cpFileName)
{
	FILE *fp = fopen(cpFileName, "r");

	if(!fp)
	{
		CString message;
		message.Format("%s file is not found\nIlluminaton settings will not be changed", cpFileName);
		AfxMessageBox(message);
		return FALSE;
	}

	int temp;

	fscanf(fp, "[COMMON]\nMode=%d\n", &mData.iMode);
	fscanf(fp, "Comport=%d\n", &temp);

	for(int i = 0 ; i < 4 ; i++)
	{
		fscanf(fp, "[Channel%d]\n", &temp);
		fscanf(fp, "TriggerType=%d\n", &mData.iTriggerType[i]);
		fscanf(fp, "DelayTime=%d\n", &mData.iDelayTime[i]);
		fscanf(fp, "OnTime=%d\n", &mData.iOnTime[i]);
	}

	fclose(fp);
	return TRUE;
}
