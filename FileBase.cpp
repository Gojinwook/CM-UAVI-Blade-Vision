// FileBase.cpp : implementation file
//
#include "stdafx.h"
#include "FileBase.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char *THIS_FILE = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CFileBase
CFileBase::CFileBase()
{
	m_pFile = NULL;
	m_bOpened = FALSE;
}

CFileBase::~CFileBase()
{
	CloseDataFile();
}

/////////////////////////////////////////////////////////////////////////////
// CFileBase message handlers

void CFileBase::SetFilePtr(CFile* pFile)
{
	m_pFile = pFile;
}
void CFileBase::CreateFolder(CString szFolder)
{
	WIN32_FIND_DATA FindData;
	HANDLE hFind = FindFirstFile(szFolder, &FindData);
	
	if (hFind == INVALID_HANDLE_VALUE)
		CreateDirectory(szFolder, NULL);

	// SW Down Bug Fix
	FindClose(hFind);
}
void CFileBase::DeleteFolder(CString szFolder)
{
	RemoveDirectory(szFolder);
}
void CFileBase::SetFolderFile(CString szFolder, CString szFile)
{
	WIN32_FIND_DATA FindData;
	HANDLE hFind = FindFirstFile(szFolder, &FindData);
	
	if (hFind == INVALID_HANDLE_VALUE)
		CreateDirectory(szFolder, NULL);

	FindClose(hFind);

	m_szFilePath = szFolder + "\\" + szFile;	
}

BOOL CFileBase::OpenDataFile(UINT nOpenFlags)
{
	m_bOpened = FALSE;
	if (m_pFile)
		if (m_pFile->Open(m_szFilePath, nOpenFlags))
			m_bOpened = TRUE;

	return m_bOpened;
}

void CFileBase::CloseDataFile()
{
	if (m_pFile && m_bOpened)
		m_pFile->Close();
	m_bOpened = FALSE;
}

//Multiple Defect
void CFileBase::CreatePath(CString csPath, BOOL bFileNameExist)
{
	if (bFileNameExist)
		csPath = csPath.Left(csPath.ReverseFind('\\'));

	WIN32_FIND_DATA FindData;
	HANDLE hFind = FindFirstFile(csPath, &FindData);

	if (hFind == INVALID_HANDLE_VALUE)
	{
		CString csToken(_T(""));
		CString csResultPath(_T(""));

		csResultPath = "";

		int nStart = 0, nEnd;

		BOOL bNetworkIP = FALSE;

		if (csPath.Find("\\\\", 0) == 0)
		{
			bNetworkIP = TRUE;
		}

		if (bNetworkIP)
		{
			csPath.Delete(0, 2);
		}

		while ((nEnd = csPath.Find('\\', nStart)) >= 0)
		{
			AfxExtractSubString(csToken, csPath, nStart, '\\');
			if (!csToken.Compare("")) break;
			if (nStart != 0)
				csResultPath += ('\\' + csToken);
			else
			{
				if (bNetworkIP)
				{
					csResultPath += "\\\\";
				}
				csResultPath += (csToken);
			}

			if (GetFileAttributes(csResultPath) == -1)
				CreateDirectory(csResultPath, NULL);

			nStart += 1;
		}
	}

	// SW Down Bug Fix
	FindClose(hFind);
}