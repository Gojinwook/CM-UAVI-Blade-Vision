// 저장 스레드 분리 - LeeGW

#pragma once

#define MAX_SAVE_THREAD	4

void LogFileMoveMsg (BOOL bIsMoveFAIL, CString sSrcPath, CString sDstPath);

enum enSaveType
{
	SAVE_TYPE_LOG = 0,
	SAVE_TYPE_RESULT,
	SAVE_TYPE_IMAGE,
	SAVE_TYPE_COPY,
	SAVE_TYPE_LAS_ZIP,
	SAVE_TYPE_LAS_SECTION,
	SAVE_TYPE_LAS_EVENT,
	MAX_SAVE_TYPE
};

typedef struct _SaveParam
{
	// 로그 저장용
	_SaveParam(int iSaveType, CString sLog, CString sPath)
	{
		this->iSaveType = iSaveType;
		this->sLog = sLog;
		this->sPath = sPath;

		gen_empty_obj(&this->HImage);
	}

	// 결과 로그 저장용
	_SaveParam(int iSaveType, CString sHeader, CString sLog, CString sPath)
	{
		this->iSaveType = iSaveType;
		this->sHeader = sHeader;
		this->sLog = sLog;
		this->sPath = sPath;
	}

	// 파일 복사용
	_SaveParam(int iSaveType, CString sFilename, CString sSource, CString sDestination, BOOL bEtcParam)
	{
		this->iSaveType = iSaveType;
		this->sFilename = sFilename;
		this->sSource = sSource;
		this->sDestination = sDestination;
		this->bEtcParam = bEtcParam;
	}

	// 파일 복사용
	_SaveParam(int iSaveType, CString sSource, CString sDestination, BOOL bEtcParam)
	{
		this->iSaveType = iSaveType;
		this->sSource = sSource;
		this->sDestination = sDestination;
		this->bEtcParam = bEtcParam;
		this->sFilename = _T("");
	}

	// ZIP 저장용
	_SaveParam(int iSaveType, CString sFilename, CString sFilepath, CString sIndex1, CString sIndex2, BOOL bEtcParam)
	{
		this->iSaveType = iSaveType;
		this->sFilename = sFilename;
		this->sPath = sFilepath;
		this->sIndex1 = sIndex1;
		this->sIndex2 = sIndex2;
		this->bEtcParam = bEtcParam;
	}

	// 이미지 저장용
	_SaveParam(int iSaveType, Hobject HImage, CString sExtension, CString sPath)
	{
		this->iSaveType = iSaveType;
		this->sExtension = sExtension;
		this->sPath = sPath;

		copy_obj(HImage, &this->HImage, 1, 1);
	}

	// 결과 로그 저장용
	_SaveParam(int iSaveType, Hobject HRegion, CString sHeader, CString sLog, CString sPath)
	{
		this->iSaveType = iSaveType;
		this->sHeader = sHeader;
		this->sLog = sLog;
		this->sPath = sPath;

		copy_obj(HRegion, &this->HRegion, 1, 1);
	}

	// 기본
	_SaveParam()
	{
	}

	int iSaveType = -1;

	CString sLog = _T("");
	CString sHeader = _T("");
	CString sPath = _T("");
	CString sExtension = _T("");
	Hobject HImage = NULL;
	Hobject HRegion = NULL;

	// 파일 복사용
	CString sSource = _T("");
	CString sDestination = _T("");
	CString sFilename = _T("");
	BOOL bEtcParam = FALSE;

	// ZIP 저장용
	CString sIndex1 = _T("");
	CString sIndex2 = _T("");

} SaveParam;


class CSaveManager
{
public:

	static CSaveManager* m_pInstance;
	static	CSaveManager* GetInstance();
	void	DeleteInstance();
	CSaveManager(void);
	~CSaveManager(void);

	BOOL SaveLog(CString Log, CString Path);
	BOOL SaveImage(Hobject HImage, CString Extension, CString Path);
	BOOL Copy(CString Filename, CString Source, CString Destination, BOOL bEtcParam);
	BOOL Copy( CString Source, CString Destination, BOOL bEtcParam);
	BOOL SaveResult(CString Header, CString Log, CString Path);
	BOOL SaveLasZip(CString sFileName, CString sFilePath, CString Index1, CString Index2, BOOL IsEnd);
	BOOL SaveLasSection(CString Filename, CString Source, CString Destination, BOOL bEtcParam);
	BOOL SaveLasEvent(CString Filename, CString Source, CString Destination, BOOL bEtcParam);

	void SetEvent();
	void ResetEvent(int iThreadNo);

	BOOL IsEmptySaveParam();
	BOOL PushSaveParam(SaveParam Param);
	SaveParam PopSaveParam();

private:
	volatile BOOL m_bRunning[MAX_SAVE_THREAD];

	BOOL m_bRun;
	int m_iSaveThreadOrder;
	CMutex m_muQueue;
	CMutex m_muSave;
	CEvent m_evSaveDone[MAX_SAVE_THREAD];	
	CWinThread* m_thThread[MAX_SAVE_THREAD];

	deque<SaveParam> m_deSaveParam;

	static UINT SaveThread(LPVOID lp);
};

typedef struct _SAVE_THREAD_PARAM {

	_SAVE_THREAD_PARAM(int iThreadIdx, CSaveManager* pInstance)
	{
		this->iThreadIdx = iThreadIdx;
		this->pInstance = pInstance;
	};

	int iThreadIdx;
	CSaveManager* pInstance;

} SAVE_THREAD_PARAM;
