// 저장 스레드 분리 - LeeGW
#include "stdafx.h"
#include "uScan.h"
#include "SaveManager.h"

CSaveManager* CSaveManager::m_pInstance = NULL;

CSaveManager* CSaveManager::GetInstance()
{
	if (!m_pInstance) {
		m_pInstance = new CSaveManager();
	}
	return m_pInstance;
}

void CSaveManager::DeleteInstance()
{
	SAFE_DELETE(m_pInstance);
}

CSaveManager::CSaveManager(void)
{
	m_bRun = TRUE;
	m_iSaveThreadOrder = 0;

	m_deSaveParam.clear();

	for (long lThread = 0; lThread < MAX_SAVE_THREAD; lThread++) {
		ResetEvent(lThread);
	}

	for (int i = 0; i < MAX_SAVE_THREAD; i++)
		m_bRunning[i] = FALSE;

	for (long lThread = 0; lThread < MAX_SAVE_THREAD; lThread++)
	{
		SAVE_THREAD_PARAM* pParam = new SAVE_THREAD_PARAM(lThread, this);
		m_thThread[lThread] = AfxBeginThread(SaveThread, LPVOID(pParam));
	}
}

CSaveManager::~CSaveManager(void)
{
	m_bRun = FALSE;

	for (int i = 0; i < MAX_SAVE_THREAD; i++) {
		SetEvent(); 
	}

	for (int i = 0; i < MAX_SAVE_THREAD; i++) {
		if (m_thThread[i]) {
			::WaitForSingleObject(m_thThread[i]->m_hThread, 500);
			delete m_thThread[i];
		}
	}

}

UINT CSaveManager::SaveThread(LPVOID lp)
{
	SAVE_THREAD_PARAM* pThreadItem = (SAVE_THREAD_PARAM*)lp;

	int iThreadNo = pThreadItem->iThreadIdx;
	CSaveManager *pSaveManager = pThreadItem->pInstance;

	CSingleLock csEvent(&pSaveManager->m_evSaveDone[iThreadNo]);
	CSingleLock csSave(&pSaveManager->m_muSave);

	while (TRUE)
	{	
		if (!pSaveManager->m_bRun)
			break;

		// 이벤트 대기
		csEvent.Lock(INFINITE);

		// 실행 상태 설정
		pSaveManager->m_bRunning[iThreadNo] = TRUE;

		// 큐가 빌 때까지 계속 처리
		while (1)
		{
			csSave.Lock();

			if (pSaveManager->IsEmptySaveParam())
			{
				csSave.Unlock();
				break;
			}

			auto data = make_unique<SaveParam>(pSaveManager->PopSaveParam());

			if (data->iSaveType == SAVE_TYPE_LOG)
			{

				CFile file;
				if (file.Open(data->sPath, CFile::modeCreate | CFile::modeNoTruncate | CFile::modeWrite))
				{
					try {
						file.SeekToEnd();
						file.Write(data->sLog, data->sLog.GetLength());
						file.Close();
					}
					catch (CFileException* pEx) {
						pEx->Delete();
					}
				}
			}
			else if (data->iSaveType == SAVE_TYPE_RESULT)
			{
				CFile file;
				if (file.Open(data->sPath, CFile::modeCreate | CFile::modeNoTruncate | CFile::modeWrite))
				{
					try {
						CString strSave;
						file.SeekToEnd();
						if (file.GetPosition() == 0)
						{
							strSave.Format("%s\r\n", data->sHeader);
							file.Write(strSave, strSave.GetLength());
						}

						strSave.Format("%s\r\n", data->sLog);

						file.Write(strSave, strSave.GetLength());
						file.Close();
					}
					catch (CFileException* pEx) {
						pEx->Delete();
					}
				}
			}
			else if (data->iSaveType == SAVE_TYPE_IMAGE)
			{
				try
				{
					if (THEAPP.m_pGFunction->ValidHImage(data->HImage))
						write_image(data->HImage, data->sExtension, 0, data->sPath);
				}
				catch (HException &except)
				{
					CString str; str.Format("Halcon Exception [SaveImage] : %s", except.message);
					THEAPP.SaveLog(str);
				}

				data->HImage.Reset();
				gen_empty_obj(&data->HImage);
			}
			else if (data->iSaveType == SAVE_TYPE_COPY)
			{
				HANDLE hFindFile;
				WIN32_FIND_DATA FindFileData;

				CString sSrcPath, sFilename, sDstPath;

				try
				{
					if (data->sFilename != _T(""))
					{
						sSrcPath.Format("%s\\%s", data->sSource, data->sFilename);

						if ((hFindFile = FindFirstFile(sSrcPath, &FindFileData)) != INVALID_HANDLE_VALUE)
						{
							FindClose(hFindFile);

							sFilename = (LPCTSTR)FindFileData.cFileName;
							sSrcPath.Format("%s\\%s", data->sSource, sFilename);
							sDstPath.Format("%s\\%s", data->sDestination, sFilename);
							CopyFile(sSrcPath, sDstPath, FALSE);
						}
					}
					else
					{
						CopyFile(data->sSource, data->sDestination, FALSE);
					}
				}
				catch (HException &except)
				{
					CString str; str.Format("Halcon Exception [CopyFile] : %s", except.message);
					THEAPP.SaveLog(str);
				}

			}
			else if (data->iSaveType == SAVE_TYPE_LAS_ZIP)
			{
				CString sFilePath, sFilename, sIndex1, sIndex2;
				sFilename = data->sFilename;
				sFilePath = data->sPath;
				sIndex1 = data->sIndex1;
				sIndex2 = data->sIndex2;
				BOOL bIsEnd = data->bEtcParam;

				HANDLE hFindFile;
				WIN32_FIND_DATA FindFileData;

				CString sSrcPath, sDstPath;
				sSrcPath.Format("%s\\%s__Result_%s*", THEAPP.GetLASTempDirectory(), sIndex1, sIndex2);

				try
				{
					if ((hFindFile = FindFirstFile(sSrcPath, &FindFileData)) == INVALID_HANDLE_VALUE)
					{
						THEAPP.CreateLasZip(sIndex1, sIndex2);
					}

					THEAPP.AddLasZip(sIndex2, sFilePath, sFilename);

					if (bIsEnd)
					{
						csSave.Unlock();
						Sleep(THEAPP.Struct_PreferenceStruct.m_iSaveLasDataZipWaitTime);
						csSave.Lock();

						THEAPP.CloseLasZip(sIndex2);
						THEAPP.MoveLasZip(sIndex1, sIndex2);
					}
				}
				catch (HException &except)
				{
					CString str; str.Format("Halcon Exception [LasZip] : %s", except.message);
					THEAPP.SaveLog(str);
				}

			}
			else if (data->iSaveType == SAVE_TYPE_LAS_SECTION)
			{
				if (data->sFilename != _T(""))
				{
					CFileFind finder;
					BOOL bContinue = TRUE;

					CString sSrcPath, sDstPath, sFilename;
					sSrcPath.Format("%s\\*%s*", data->sSource, data->sFilename);
					bContinue = finder.FindFile(sSrcPath);
				
					while (bContinue)
					{
						try
						{
							bContinue = finder.FindNextFile();
							if (!finder.IsDots() && !finder.IsDirectory()) // Ignore this item.
							{
								sFilename = (LPCTSTR)finder.GetFileName();

								sSrcPath.Format("%s\\%s", data->sSource, sFilename);
								sDstPath.Format("%s\\%s", data->sDestination, sFilename);

								if ((data->bEtcParam == FALSE) && (sFilename.Find("Defect_Shape_Feature") != -1 || sFilename.Find("Detect_Vision_Para") != -1))
								{
									CopyFile(sSrcPath, sDstPath, FALSE);
								}
							}
						}
						catch (HException &except)
						{
							CString str; str.Format("Halcon Exception [LasZip] : %s", except.message);
							THEAPP.SaveLog(str);
						}
					}

					finder.Close();
				}
			}
			else if (data->iSaveType == SAVE_TYPE_LAS_EVENT)
			{

				if (data->sFilename != _T(""))
				{
					CFileFind finder;
					BOOL bContinue = TRUE;

					CString sSrcPath, sDstPath, sFilename;
					sSrcPath.Format("%s\\*%s*", data->sSource, data->sFilename);
					bContinue = finder.FindFile(sSrcPath);

					while (bContinue)
					{
						try
						{
							bContinue = finder.FindNextFile();
							if (!finder.IsDots() && !finder.IsDirectory()) // Ignore this item.
							{
								BOOL bIsMoveSuccess = FALSE;

								sFilename = (LPCTSTR)finder.GetFileName();

								sSrcPath.Format("%s\\%s", data->sSource, sFilename);
								sDstPath.Format("%s\\%s", data->sDestination, sFilename);

								// MEMO : Auto Param. EVENT 폴더에 Log 복사하는 부분
								// MEMO : data->bEtcParam -> What for? - 250912, jhkim
								// TODO : 고정 문자열이 아니라, 매크로 혹은 설정값 등으로 처리해야 할 듯 하다. - 251111, jhkim
								if ((data->bEtcParam == FALSE) && (sFilename.Find("Defect_Shape_Feature") != -1))
								{
									sDstPath.Replace("Defect_Shape_Feature", "AVI_MFG_DEFECT_AVIMONITOR_RESULT");
									bIsMoveSuccess = MoveFile(sSrcPath, sDstPath);
									LogFileMoveMsg(bIsMoveSuccess, sSrcPath, sDstPath);
								}
								else if ((data->bEtcParam == FALSE) && (sFilename.Find("AVI_MFG_SPEC_AVIMONITOR_RESULT") != -1))
								{
									bIsMoveSuccess = MoveFile(sSrcPath, sDstPath);
									LogFileMoveMsg(bIsMoveSuccess, sSrcPath, sDstPath);
								}
							}
						}
						catch (HException &except)
						{
							CString str; str.Format("Halcon Exception [LasEvent] : %s", except.message);
							THEAPP.SaveLog(str);
						}
					}

					finder.Close();
				}
			}

			csSave.Unlock();
		}

		pSaveManager->m_bRunning[iThreadNo] = FALSE;
		csEvent.Unlock();
		Sleep(1);
	}	// 	while (TRUE)

	return 0;
}

void CSaveManager::SetEvent()
{
	// 실행 중인 스레드가 있는지 확인
	BOOL bAnyRunning = FALSE;
	for (int i = 0; i < MAX_SAVE_THREAD; i++)
	{
		if (m_bRunning[i])
		{
			bAnyRunning = TRUE;
			break;
		}
	}

	// 모든 스레드가 쉬고 있고 큐에 작업이 있으면
	if (!bAnyRunning && !m_deSaveParam.empty())
	{
		CSingleLock cs(&m_muSave);
		cs.Lock();

		// 작업량에 따라 깨울 스레드 수 결정
		int iQueueSize = m_deSaveParam.size();
		int iThreadsToWake = min(iQueueSize, MAX_SAVE_THREAD);

		for (int i = 0; i < iThreadsToWake; i++)
		{
			int iThreadIdx = (m_iSaveThreadOrder + i) % MAX_SAVE_THREAD;
			m_evSaveDone[iThreadIdx].SetEvent();
		}

		m_iSaveThreadOrder = (m_iSaveThreadOrder + iThreadsToWake) % MAX_SAVE_THREAD;

		cs.Unlock();
	}
}

void CSaveManager::ResetEvent(int iThreadNo)
{
	CSingleLock cs(&m_muSave);

	cs.Lock();

	m_evSaveDone[iThreadNo].ResetEvent();

	cs.Unlock();
}


BOOL CSaveManager::SaveImage(Hobject HImage, CString Extension, CString Path)
{
	SaveParam SaveParamIn(SAVE_TYPE_IMAGE, HImage, Extension, Path);

	PushSaveParam(SaveParamIn);

	SetEvent();

	return TRUE;
}

BOOL CSaveManager::SaveLog(CString Log, CString Path)
{
	SaveParam SaveParamIn(SAVE_TYPE_LOG, Log, Path);

	PushSaveParam(SaveParamIn);

	SetEvent();

	return TRUE;
}

BOOL CSaveManager::SaveResult(CString Header, CString Log, CString Path)
{
	SaveParam SaveParamIn(SAVE_TYPE_RESULT, Header, Log, Path);

	PushSaveParam(SaveParamIn);

	SetEvent();

	return TRUE;
}

BOOL CSaveManager::Copy(CString Filename, CString Source, CString Destination, BOOL bEtcParam)
{
	SaveParam SaveParamIn(SAVE_TYPE_COPY, Filename, Source, Destination, bEtcParam);

	PushSaveParam(SaveParamIn);

	SetEvent();

	return TRUE;
}


BOOL CSaveManager::Copy(CString Source, CString Destination, BOOL bEtcParam)
{
	SaveParam SaveParamIn(SAVE_TYPE_COPY, Source, Destination, bEtcParam);

	PushSaveParam(SaveParamIn);

	SetEvent();

	return TRUE;
}

BOOL CSaveManager::SaveLasZip(CString sFileName, CString sFilePath, CString Index1, CString Index2, BOOL IsEnd)
{
	SaveParam SaveParamIn(SAVE_TYPE_LAS_ZIP, sFileName, sFilePath, Index1, Index2, IsEnd);

	PushSaveParam(SaveParamIn);

	SetEvent();

	return TRUE;
}

BOOL CSaveManager::SaveLasSection(CString Filename, CString Source, CString Destination, BOOL bEtcParam)
{
	SaveParam SaveParamIn(SAVE_TYPE_LAS_SECTION, Filename, Source, Destination, bEtcParam);

	PushSaveParam(SaveParamIn);

	SetEvent();

	return TRUE;
}

BOOL CSaveManager::SaveLasEvent(CString Filename, CString Source, CString Destination, BOOL bEtcParam)
{

	SaveParam SaveParamIn(SAVE_TYPE_LAS_EVENT, Filename, Source, Destination, bEtcParam);

	PushSaveParam(SaveParamIn);

	SetEvent();

	return TRUE;
}

BOOL  CSaveManager::IsEmptySaveParam()
{
	BOOL bIsEmpty = TRUE;

	try {

		CSingleLock cs(&m_muQueue);

		cs.Lock();
		if (m_deSaveParam.empty() || m_deSaveParam.size() == 0)
			bIsEmpty = TRUE;
		else
			bIsEmpty = FALSE;
		cs.Unlock();

		return bIsEmpty;

	}
	catch (CException& except)
	{
		CString str;
		str.Format("Queue Exception [IsEmptySaveParam] : %s", (CString)(LPCTSTR)except.ReportError());
		THEAPP.SaveLog(str);

		return TRUE;
	}

}
BOOL CSaveManager::PushSaveParam(SaveParam Param)
{

	try {

		CSingleLock cs(&m_muQueue);

		cs.Lock();
		m_deSaveParam.push_front(Param);
		cs.Unlock();

		return TRUE;

	}
	catch (CException& except)
	{
		CString str;
		str.Format("Queue Exception [PushSaveParam] : %s", (CString)(LPCTSTR)except.ReportError());
		THEAPP.SaveLog(str);

		return FALSE;
	}


}

SaveParam CSaveManager::PopSaveParam()
{
	SaveParam SaveParamOut = SaveParam{};
	try {

		CSingleLock cs(&m_muQueue);
		cs.Lock();

		if (!m_deSaveParam.empty() && m_deSaveParam.size() != 0)
		{
			SaveParamOut = move(m_deSaveParam.back());
			m_deSaveParam.pop_back();
		}

		cs.Unlock();

		return SaveParamOut;
	}
	catch (CException& except)
	{
		CString str;
		str.Format("Queue Exception [PopSaveParam] : %s", (CString)(LPCTSTR)except.ReportError());
		THEAPP.SaveLog(str);
	}
}

void LogFileMoveMsg (BOOL bIsMoveSuccess, CString sSrcPath, CString sDstPath)
{
	if (bIsMoveSuccess == TRUE)
		DoubleLogOut("[SaveLasEvent] Move LOG Success. DstPath : %s", sDstPath);
	else
	{
		int iErr = GetLastError();
		CString strErrMsg;
		DoubleLogOut("[SaveLasEvent](1/3) MoveFile FAIL. Source : %s", sSrcPath);
		DoubleLogOut("[SaveLasEvent](2/3) MoveFile FAIL. Dest.  : %s", sDstPath);
		switch (iErr)
		{
		case ERROR_FILE_NOT_FOUND: // ERROR_FILE_NOT_FOUND
			strErrMsg = "The system cannot find the file specified.";
			break;
		case ERROR_PATH_NOT_FOUND: // ERROR_PATH_NOT_FOUND
			strErrMsg = "The system cannot find the path specified.";
			break;
		case ERROR_ACCESS_DENIED: // ERROR_ACCESS_DENIED
			strErrMsg = "Access is denied.";
			break;
		case ERROR_ALREADY_EXISTS: // ERROR_ALREADY_EXISTS
			strErrMsg = "The file already exists.";
			break;
		case ERROR_DISK_FULL: // ERROR_DISK_FULL
			strErrMsg = "There is not enough space on the disk.";
			break;
		case ERROR_SHARING_VIOLATION: // ERROR_SHARING_VIOLATION
			strErrMsg = "The process cannot access the file because it is being used by another process.";
			break;
		case ERROR_INVALID_PARAMETER: // ERROR_INVALID_PARAMETER
			strErrMsg = "The parameter is incorrect.";
			break;
		default:
			strErrMsg = "Unknown error.";
			break;
		}

		DoubleLogOut("[SaveLasEvent](3/3) MoveFile FAIL. ErrMsg : %s", (LPCTSTR)strErrMsg);
	}
}