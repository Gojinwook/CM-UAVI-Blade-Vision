#include "StdAfx.h"
#include "AboutDlg.h"

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
	ON_WM_CTLCOLOR()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	DDX_Control(pDX, IDC_EDIT_RELEASE, m_EditRelease);
	//}}AFX_DATA_MAP
}

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

BOOL CAboutDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO: Add extra initialization here
	m_EditBGBrush.CreateSolidBrush(RGB(255, 255, 255));

	GetAppVersion();
	GetAppModify();
	ShowReleaseInformation();

	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}

void CAboutDlg::ShowReleaseInformation()
{
	CString	sReleaseText;
	char pszPathName[_MAX_PATH * 2];
	::GetModuleFileName(::AfxGetInstanceHandle(), pszPathName, _MAX_PATH * 2);

	//PathRemoveFileSpec(pszPathName);
	pszPathName[strlen(pszPathName) - strlen(strrchr(pszPathName, '\\'))] = (char)'\0';

	strcat(pszPathName, "\\Release.txt");

	FILE *fp;

	if ((fp = fopen(pszPathName, "rt")) != NULL) {
		char caTempString[500];
		while (fgets(caTempString, 499, fp) != NULL) {
			caTempString[strlen(caTempString) - 1] = '\0';
			sReleaseText += caTempString;
			sReleaseText += "\r\n";
		}
		m_EditRelease.SetSel(0, -1);
		m_EditRelease.ReplaceSel(LPCTSTR(sReleaseText));
		m_EditRelease.SendMessage(WM_VSCROLL, MAKEWPARAM(SB_TOP, NULL), NULL);

		//		m_EditRelease.LineScroll(m_EditRelease.GetLineCount());
	}
}

//////////////////
// Get file version info for a given module
// Allocates storage for all info, fills "this" with
// VS_FIXEDFILEINFO, and sets codepage.
//
BOOL CAboutDlg::GetAppVersion(void)
{
	char AppFilePath[_MAX_PATH * 2];
	// 실행파일의 풀패스 경로를 구한다.  
	strcpy(AppFilePath, AfxGetApp()->m_pszHelpFilePath);
	int slength = strlen(AppFilePath);
	strcpy(AppFilePath + (slength - 4), ".exe");

	//
	// 주어진 파일의 버전 정보를 읽는다. (이렇게 길어지리라 생각 못했다... )
	//
	DWORD Dummy = NULL;
	BOOL bResult = FALSE;
	HRESULT hResult = S_OK;
	DWORD dwLen = 0;
	DWORD dwError = 0;
	PVOID pVersionData = NULL;
	PVOID pVersionInfo = NULL;

	// 버전정보의 크기를 알아 내고 데이터 버퍼를 할당한다.
	// 파일의 버전정보의 크기가 파일마다 제각기 다르기 땜시...
	dwLen = ::GetFileVersionInfoSize(AppFilePath, &Dummy);
	if (dwLen == 0)
	{
		return GetLastError();
	}
	// 버전 정보를 담을 버퍼를 할당한다. (수천 바이트가 필요하다)
	pVersionData = ::HeapAlloc(::GetProcessHeap(), 0, dwLen);
	ZeroMemory(pVersionData, dwLen);

	// 버전 정보 데이터를 읽는다.
	bResult = ::GetFileVersionInfo(AppFilePath, NULL, dwLen, pVersionData);
	if (bResult == FALSE)
	{
		dwError = GetLastError();
		::HeapFree(::GetProcessHeap(), 0, pVersionData);
		return dwError;
	}

	// VS_FIXEDFILEINFO 로 부터 버전정보를 읽는다.
	// (숫자로 된 버전정보는 이 구조체 안에 정보가 담긴다)
	bResult = ::VerQueryValue(pVersionData, TEXT("\\"), &pVersionInfo, (PUINT)&dwLen);
	if (bResult == FALSE)
	{
		dwError = GetLastError();
		::HeapFree(::GetProcessHeap(), 0, pVersionData);
		return dwError;
	}
	VS_FIXEDFILEINFO *pFileInfo = (VS_FIXEDFILEINFO *)pVersionInfo;
	// 숫자로된 버전 정보를 알아냈으니 필요한 대로 쓰문 되겠다.
	DWORD dwMajorVer = HIWORD(pFileInfo->dwFileVersionMS);
	DWORD dwMinorVer = LOWORD(pFileInfo->dwFileVersionMS);
	DWORD dwReleaseVer = HIWORD(pFileInfo->dwFileVersionLS);
	DWORD dwBuildVer = LOWORD(pFileInfo->dwFileVersionLS);

	CString strVer;
	strVer.Format("CMI System - Version %d.%d.%d.%d", dwMajorVer, dwMinorVer, dwReleaseVer, dwBuildVer);

	GetDlgItem(IDC_STATIC_VERSION)->SetWindowText(strVer);

	// pVersionInfo 변수가 문자열 버전정보를 갖고 있다.
	// 쓰고 잡은 대로 쓰문 되긋다.
	// 숫자정보만 표시하고 말자. 
	// 필요로 하다면 추가적인 파일 버전 정보(저작권, 제품명, 기타 등등)를 읽고자 하면
	// 바로 위 코드를 반복적으로 사용하되 FileVersion 대신 ProductName, CompanyName 등을
	// 사용하면 되겠다. 랭귀지 코드ㅡ 알아야 한다.
	// 사용 다한 메모리는 해제해 줘야 함다...
	::HeapFree(::GetProcessHeap(), 0, pVersionData);

	return TRUE;
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void CAboutDlg::GetAppModify(void)
{
	char AppFilePath[_MAX_PATH];
	// 실행파일의 풀패스 경로를 구한다.  
	strcpy(AppFilePath, AfxGetApp()->m_pszHelpFilePath);
	int slength = strlen(AppFilePath);
	strcpy(AppFilePath + (slength - 4), ".exe");
	// 파일정보중 수정된 날짜만 가지고 온다. 
	CString ModifyText;
	ModifyText = "프로그램 생성 시간: " + GetFileDateStr(AppFilePath);

	GetDlgItem(IDC_STATIC_FILEINFO)->SetWindowText(ModifyText);
}
//---------------------------------------------------------------------------
CString CAboutDlg::GetFileDateStr(CString FilePath)
{
	HANDLE hfilehandle;		 // identifies the file
	FILETIME CreationTime;	 // address of creation time
	FILETIME LastAccessTime; // address of last access time
	FILETIME LastWriteTime;	 // address of last write time
	CString FileDateStr = _T("----.--.--, --:--:--");
	// 파일의 경로명을 주고 핸들을 얻는다.
	hfilehandle = CreateFile(FilePath, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	// 얻어온 핸들로 부터 파일의 시간정보 3가지 알수 있다. 생성시간, 마지막접근시간, 마지막수정시간
	// 이중 마지막 수정시간을 사용한다.
	int bRet = GetFileTime(hfilehandle, &CreationTime, &LastAccessTime, &LastWriteTime);
	// 핸들다 썼으니 닫아줘야한다.
	CloseHandle(hfilehandle);
	// 정보를 제대로 얻어왔다면 리턴값이 0이 아니다.
	if (bRet)
	{
		CTime FileTime = CTime(LastWriteTime);
		// 날자 및 시간 정보를 보기 좋게 출력한다.
		FileDateStr.Format("%4d-%02d-%02d, %02d:%02d:%02d", FileTime.GetYear(), FileTime.GetMonth(),
			FileTime.GetDay(), FileTime.GetHour(), FileTime.GetMinute(), FileTime.GetSecond());
	}
	else // 정보를 제대로 얻어오지 못하면 알수 없음을 출력한다.
		FileDateStr = "Unknown";

	return FileDateStr;
}
//---------------------------------------------------------------------------

HBRUSH CAboutDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);

	// TODO: Change any attributes of the DC here

	CWnd *pEditWnd = (CWnd *)GetDlgItem(IDC_EDIT_RELEASE);

	if (pEditWnd == pWnd) {
		pDC->SetTextColor(RGB(0, 0, 0));
		pDC->SetBkColor(RGB(255, 255, 255));

		return m_EditBGBrush;
	}
	// TODO: Return a different brush if the default is not desired
	return hbr;
}
