// IniFileCS.h: interface for the CIniFileCS class.
//
#pragma once

class CIniFileCS  
{
public:
	CIniFileCS(CString strFile);
	virtual ~CIniFileCS();

private:
	CString	m_strFile;
	
public:
	
	BOOL	Check_File();

	BOOL	Get_Bool(CString strApp, CString strKey, BOOL bDefault);
	int		Get_Integer(CString strApp, CString strKey, int nDefault);
	long	Get_Long(CString strApp, CString strKey, long lDefault);
	float	Get_Float(CString strApp, CString strKey, float fDefault);
	double	Get_Double(CString strApp, CString strKey, double dDefault);
	CString Get_String(CString strApp, CString strKey, CString nDefault);

	void	Set_Bool(CString strApp, CString strKey, BOOL bValue);
	void	Set_Integer(CString strApp, CString strKey, int nValue);
	void	Set_Long(CString strApp, CString strKey, long lValue);
	void	Set_Float(CString strApp, CString strKey, float fValue, CString strFormat = "%f");
	void	Set_Double(CString strApp, CString strKey, double dValue, CString strFormat = "%lf");
	void	Set_String(CString strApp, CString strKey, CString strValue);
	CString Get_String_Korean(CString strApp, CString strKey, CString strDefault);
	char*   UTF8toANSI(char *pszCode);
};

/////////////////////////////////////////////////////////////////////////////
