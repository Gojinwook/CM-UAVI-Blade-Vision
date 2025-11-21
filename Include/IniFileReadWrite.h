// IniFileReadWrite.h: interface for the CIniFileReadWrite class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_INIFILEREADWRITE_H__9AE4DE3D_A9FC_4A8E_BA70_B610E6913727__INCLUDED_)
#define AFX_INIFILEREADWRITE_H__9AE4DE3D_A9FC_4A8E_BA70_B610E6913727__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#pragma comment (lib, "iniFile.lib")

//class AFX_EXT_CLASS CIniFileReadWrite  
class __declspec(dllexport) CIniFileReadWrite  
{
public:
	CIniFileReadWrite();
	virtual ~CIniFileReadWrite();

public:	
	CString	ExtractStr;
	CString WantedStr,ResidueStr;
	LPTSTR lpExtractStr;
	int m_fg, position;
	DWORD return_char_num;

public:
    int  BaudRate;
	BOOL ParityCheck;
	int  DataBit;
	int  StopBit;

public:
	int  GetIntfromSingleValue(const char *Section, const char *KeyName, int Default, const char *FileName);
    int  GetIntfromMultipleValue(const char *Section, const char *KeyName, int Default, int BufSize, int Order, const char *FileName);
	float GetFloatfromSingleValue(const char *Section, const char *KeyName, float Default, int BufSize, const char *FileName);
    float GetFloatfromMultipleValue(const char *Section, const char *KeyName, float Dafault, int BufSize, int Order, const char *FileName);

	BOOL GetBOOLfromSingleValue(const char *Section, const char *KeyName, const char *DefaultStr, int BufSize, const char *FileName);
	char* GetStringfromSingleValue(const char *Section, const char *KeyName, const char *DefaultStr, int BufSize, const char *FileName);
	BOOL  COMPortGet(char *ComportName, const char *Section, const char *DefaultStr, int BufSize, const char *FileName);

	void WriteIntfromSingleValue(const char *Section, const char *KeyName, int WriteInt, const char *FileName);
	void WriteBOOLfromSingleValue(const char *Section, const char *KeyName, BOOL WriteBOOL, const char *FileName);
	void WriteStringfromSingleValue(const char *Section, const char *KeyName, CString WriteStr, const char *FileName);
	void COMPortSet(char *ComportName, const char *Section, LPSTR lpWriteDataStr, const char *FileName);
    //LPTSTR SetCOMPortData(char *ComPort);
    LPTSTR SetCOMPortDataL(char *ComPortName, int mBaudRate, BOOL mParityCheckYES, int mDataBit, int mStopBit);
	void WriteFloatfromMultipleValue(const char *Section, const char *KeyName, float Write_float, int BufSize, int Order, CString UnitStr, const char *FileName);
	void WriteFloatfromSingleValue(const char *Section, const char *KeyName, float Write_float, const char *FileName);

	// 새로 추가
	BOOL GetLEVELfromSingleValue(const char *Section, const char *KeyName, const char *DefaultStr, int BufSize, const char *FileName);
	int GetEVENTfromSingleValue(const char *Section, const char *KeyName, const char *DefaultStr, int BufSize, const char *FileName);
	int GetStepSpeedfromSingleValue(const char *Section, const char *KeyName, const char *DefaultStr, int BufSize, const char *FileName);
	int GetHomeIndexfromSingleValue(const char *Section, const char *KeyName, const char *DefaultStr, int BufSize, const char *FileName);
	int GetFeedbackfromSingleValue(const char *Section, const char *KeyName, const char *DefaultStr, int BufSize, const char *FileName);
	int GetIntegrationModefromSingleValue(const char *Section, const char *KeyName, const char *DefaultStr, int BufSize, const char *FileName);
};

#endif // !defined(AFX_INIFILEREADWRITE_H__9AE4DE3D_A9FC_4A8E_BA70_B610E6913727__INCLUDED_)
