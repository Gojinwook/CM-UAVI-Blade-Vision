// IniFileCS.cpp: implementation of the CIniFileCS class.
//
#include "stdafx.h"
#include "IniFileCS.h"

#ifdef _DEBUG
#undef THIS_FILE
static char* THIS_FILE=__FILE__;
#define new DEBUG_NEW
#endif

CIniFileCS::CIniFileCS(CString strFile)
{
	m_strFile = strFile;
}

CIniFileCS::~CIniFileCS()
{
}

/////////////////////////////////////////////////////////////////////////////
// INI File Check

BOOL CIniFileCS::Check_File()
{
	CFileFind find;

	if (find.FindFile(m_strFile)) return TRUE;
	else return FALSE;
}

/////////////////////////////////////////////////////////////////////////////
// Get Function

// Parameters:
//   strApp    - The section name in the INI file.
//   strKey    - The key name whose value is to be retrieved.
//   nDefault  - The default value to return if the key is not found.
BOOL CIniFileCS::Get_Bool(CString strApp, CString strKey, BOOL bDefault)
{
	char cReturn[100];
	CString strDefault;
	
	if (bDefault) strDefault = "TRUE";
	else strDefault = "FALSE";

	GetPrivateProfileString(strApp, strKey, strDefault, cReturn, sizeof(cReturn), m_strFile);
	
	if ((CString)cReturn == "TRUE") return TRUE;
	else return FALSE;
}

int CIniFileCS::Get_Integer(CString strApp, CString strKey, int nDefault)
{
	return GetPrivateProfileInt(strApp, strKey, nDefault, m_strFile);
}

long CIniFileCS::Get_Long(CString strApp, CString strKey, long lDefault)
{
	int nDefault = (int)lDefault;

	int nRet = GetPrivateProfileInt(strApp, strKey, nDefault, m_strFile);
	
	return (long)nRet;
}

float CIniFileCS::Get_Float(CString strApp, CString strKey, float fDefault)
{
	char cReturn[100];
	CString strDefault;

	strDefault.Format("%f", fDefault);
	
	GetPrivateProfileString(strApp, strKey, strDefault, cReturn, sizeof(cReturn), m_strFile);
	
	return (float)atof(cReturn);
}

double CIniFileCS::Get_Double(CString strApp, CString strKey, double dDefault)
{
	char cReturn[100];
	CString strDefault;

	strDefault.Format("%lf", dDefault);
	
	GetPrivateProfileString(strApp, strKey, strDefault, cReturn, sizeof(cReturn), m_strFile);
	
	return atof(cReturn);
}

char* CIniFileCS::UTF8toANSI(char *pszCode)
{
	BSTR    bstrWide;
	char*   pszAnsi;
	int     nLength;

	// Get nLength of the Wide Char buffer   
	nLength = MultiByteToWideChar(CP_UTF8, 0, pszCode, lstrlen(pszCode) + 1, NULL, NULL);
	bstrWide = SysAllocStringLen(NULL, nLength);

	// Change UTF-8 to Unicode (UTF-16)   
	MultiByteToWideChar(CP_UTF8, 0, pszCode, lstrlen(pszCode) + 1, bstrWide, nLength);


	// Get nLength of the multi byte buffer    
	nLength = WideCharToMultiByte(CP_ACP, 0, bstrWide, -1, NULL, 0, NULL, NULL);
	pszAnsi = new char[nLength];


	// Change from unicode to mult byte   
	WideCharToMultiByte(CP_ACP, 0, bstrWide, -1, pszAnsi, nLength, NULL, NULL);

	SysFreeString(bstrWide);


	return pszAnsi;
}

CString CIniFileCS::Get_String(CString strApp, CString strKey, CString strDefault)
{
	char cReturn[1024];
	//char* cReturn = new char[1024];
	for (int i = 0; i < 1024; i++)
		cReturn[i] = 0;

	GetPrivateProfileString(strApp, strKey, strDefault, cReturn, sizeof(cReturn), m_strFile);

	CString sReturn;
	sReturn = _T("");

	sReturn.Format("%s", cReturn);

	return sReturn;
}

CString CIniFileCS::Get_String_Korean(CString strApp, CString strKey, CString strDefault)
{
	char cReturn[1024];
	char* cReturnKor;
	for (int i = 0; i < 1024; i++)
		cReturn[i] = 0;

	GetPrivateProfileString(strApp, strKey, strDefault, cReturn, sizeof(cReturn), m_strFile);

	CString sReturn;
	sReturn = _T("");
	
	cReturnKor = UTF8toANSI(cReturn);
	sReturn.Empty();
	sReturn.Insert(0, cReturnKor);
	
	delete cReturnKor;

	return sReturn;
}

/////////////////////////////////////////////////////////////////////////////
// Set Function

void CIniFileCS::Set_Bool(CString strApp, CString strKey, BOOL bValue)
{
	CString strValue;

	if (bValue) strValue = "TRUE";
	else strValue = "FALSE";
	
	WritePrivateProfileString(strApp, strKey, strValue, m_strFile);
}

void CIniFileCS::Set_Integer(CString strApp, CString strKey, int nValue)
{
	CString strValue;

	strValue.Format("%d", nValue);
	
	WritePrivateProfileString(strApp, strKey, strValue, m_strFile);
}

void CIniFileCS::Set_Long(CString strApp, CString strKey, long lValue)
{
	CString strValue;

	strValue.Format("%d", lValue);
	
	WritePrivateProfileString(strApp, strKey, strValue, m_strFile);
}

void CIniFileCS::Set_Float(CString strApp, CString strKey, float fValue, CString strFormat)
{
	CString strValue;

	strValue.Format(strFormat, fValue);

	WritePrivateProfileString(strApp, strKey, strValue, m_strFile);
}

void CIniFileCS::Set_Double(CString strApp, CString strKey, double dValue, CString strFormat)
{
	CString strValue;

	strValue.Format(strFormat, dValue);
	
	WritePrivateProfileString(strApp, strKey, strValue, m_strFile);
}

void CIniFileCS::Set_String(CString strApp, CString strKey, CString strValue)
{
	WritePrivateProfileString(strApp, strKey, strValue, m_strFile);
}

/////////////////////////////////////////////////////////////////////////////
