// FileBase.h : header file
//
#pragma once

/////////////////////////////////////////////////////////////////////////////
// CFileBase window

class CFileBase : public CWnd
{
public:
	CFileBase();
	virtual ~CFileBase();

protected:
	CFile*		m_pFile;
	BOOL		m_bOpened;
	CString		m_szFilePath;


	

public:

	CString m_strOriImageFolder;
	CString m_strOverayImageFolder;
	CString m_strResultFolder;


	CString m_strTrayResultFolderPrev;
	CString m_strOriImageFolderPrev;
	CString m_strOverayImageFolderPrev;
	CString m_strNGOriImageFolderPrev;
	CString m_strNGOverayImageFolderPrev;
	CString m_strResultFolderPrev;
	CString m_strYesterDayResultFolderPrev;

	CString m_strReviewImageFolder;	// 24.06.12 - v2604 - Review Image 저장 - LeeGW
	CString m_strNGReviewImageFolder;	// 24.06.12 - v2604 - Review Image 저장 - LeeGW

	CString m_strFAIImageFolder;	// 24.06.12 - v2604 - Review Image 저장 - LeeGW

	CString m_strCurrentLotFolder;

	CString m_strADJRstPrev;
	CString m_strADJOKRstPrev;
	CString m_strADJNGRstPrev;
	CString m_strADJSkipRstPrev;


	void SetFilePtr(CFile* pFile);
	void SetFolderFile(CString szFolder, CString szFile);
	void CreateFolder(CString szFolder);
	void DeleteFolder(CString szFolder);

	virtual BOOL OpenDataFile(UINT nOpenFlags = CFile::modeRead);
	virtual void CloseDataFile();
	void CreatePath(CString csPath, BOOL bFileNameExist=FALSE); //Multiple Defect
};

/////////////////////////////////////////////////////////////////////////////
