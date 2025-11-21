// LoadModelDlg.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "uScan.h"
#include "LoadModelDlg.h"
#include "afxdialogex.h"
#include "IniFileCS.h"

// CLoadModelDlg 대화 상자입니다.

IMPLEMENT_DYNAMIC(CLoadModelDlg, CDialog)

CLoadModelDlg::CLoadModelDlg(CWnd *pParent /*=NULL*/)
	: CDialog(CLoadModelDlg::IDD, pParent)
{

	m_EditSelectedModel = _T("");
	m_EditModelDescription = _T("");
}

CLoadModelDlg::~CLoadModelDlg()
{
}

void CLoadModelDlg::DoDataExchange(CDataExchange *pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_MODELNAMES, m_ModelList);
	DDX_Text(pDX, IDC_EDIT_MODELNAME, m_EditSelectedModel);
	DDX_Text(pDX, IDC_EDIT_MODEL_DESCRIPTION, m_EditModelDescription);
}

BEGIN_MESSAGE_MAP(CLoadModelDlg, CDialog)
ON_LBN_SELCHANGE(IDC_LIST_MODELNAMES, &CLoadModelDlg::OnSelchangeListModelnames)
ON_BN_CLICKED(ID_LM_LOAD, &CLoadModelDlg::OnBnClickedLmLoad)
ON_BN_CLICKED(ID_LM_CANCEL, &CLoadModelDlg::OnBnClickedLmCancel)
ON_BN_CLICKED(ID_LM_OPEN_MODELFOLDER, &CLoadModelDlg::OnClickedLmOpenModelfolder)
END_MESSAGE_MAP()

void CLoadModelDlg::LoadModelList()
{
	m_ModelList.ResetContent();

	CFileFind ff;
	CString strModelFile;
	if (!THEAPP.Struct_PreferenceStruct.m_bUseAbsolutePathModel) // Ver2629
		strModelFile = THEAPP.GetWorkingDirectory() + "\\Model\\*.*";
	else
		strModelFile = THEAPP.GetWorkingEvmsDirectory() + "\\Model\\*.*";

	if (!ff.FindFile(strModelFile, 0))
		return;

	BOOL bExist = ff.FindNextFile();

	int nCnt = 0, nNameLen;
	CString strExt, strName;

	while (TRUE)
	{
		if (ff.IsDirectory())
		{
			strExt = ff.GetFileName();
			if (strExt.GetLength() > 2)
			{
				m_ModelList.AddString(strExt);
			}
		}
		if (!bExist)
			break;
		bExist = ff.FindNextFile();
	}
}
// CLoadModelDlg 메시지 처리기입니다.

BOOL CLoadModelDlg::PreTranslateMessage(MSG *pMsg)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_ESCAPE)
		return TRUE;
	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN)
		return TRUE;
	return CDialog::PreTranslateMessage(pMsg);
}

BOOL CLoadModelDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	m_EditSelectedModel = _T("");
	m_EditModelDescription = _T("");
	// TODO:  여기에 추가 초기화 작업을 추가합니다.
	LoadModelList();
	UpdateData(FALSE);
	return TRUE; // return TRUE unless you set the focus to a control
				 // 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

void CLoadModelDlg::OnSelchangeListModelnames()
{
	int index = m_ModelList.GetCurSel();
	m_ModelList.GetText(index, m_EditSelectedModel);

	m_EditModelDescription = "";
	UpdateData(FALSE);

	CString strModelName;
	int iCurrent_TrayArrayX, iCurrent_TrayArrayY, iCurrent_TrayModuleAmt;
	double dCurrent_ModulePitchX, dCurrent_ModulePitchY;
	BOOL bCurrent_OneBarrel;

	CString strModelFolder;
	if (!THEAPP.Struct_PreferenceStruct.m_bUseAbsolutePathModel) // Ver2629
		strModelFolder = THEAPP.GetWorkingDirectory() + "\\Model\\" + m_EditSelectedModel;
	else
		strModelFolder = THEAPP.GetWorkingEvmsDirectory() + "\\Model\\" + m_EditSelectedModel;

	CIniFileCS INI(strModelFolder + "\\SW\\ModelBase.ini");

	CString strSection = "Model Base";

	strModelName = INI.Get_String(strSection, "MODEL NAME", "DEFAULT");

	iCurrent_TrayArrayX = INI.Get_Integer(strSection, "TrayArrayX", 6);
	iCurrent_TrayArrayY = INI.Get_Integer(strSection, "TrayArrayY", 6);
	iCurrent_TrayModuleAmt = INI.Get_Integer(strSection, "TrayLotModuleAmt", iCurrent_TrayArrayX * iCurrent_TrayArrayY);

	dCurrent_ModulePitchX = INI.Get_Double(strSection, "ModulePitchX", 1.0);
	dCurrent_ModulePitchY = INI.Get_Double(strSection, "ModulePitchY", 1.0);

	bCurrent_OneBarrel = INI.Get_Bool(strSection, "OneBarrel", FALSE);

	m_EditModelDescription += "[Model Name] : " + strModelName;
	CString Temp;
	Temp.Format("\r\n[TrayArrayX] : %d", iCurrent_TrayArrayX);
	m_EditModelDescription += Temp;
	Temp.Format("\r\n[TrayArrayY] : %d", iCurrent_TrayArrayY);
	m_EditModelDescription += Temp;
	Temp.Format("\r\n[ModulePitchX] : %.4lf", dCurrent_ModulePitchX);
	m_EditModelDescription += Temp;
	Temp.Format("\r\n[ModulePitchY] : %.4lf", dCurrent_ModulePitchY);
	m_EditModelDescription += Temp;

	if (bCurrent_OneBarrel)
	{
		Temp.Format("\r\n[OneBarrel] : TRUE");
		m_EditModelDescription += Temp;
	}
	else
	{
		Temp.Format("\r\n[OneBarrel] : FALSE");
		m_EditModelDescription += Temp;
	}

	UpdateData(FALSE);
}

void CLoadModelDlg::OnBnClickedLmLoad()
{
	CDialog::OnOK();
}

void CLoadModelDlg::OnBnClickedLmCancel()
{
	CDialog::OnCancel();
}

void CLoadModelDlg::OnClickedLmOpenModelfolder()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	int index = m_ModelList.GetCurSel();
	if (index < 0)
	{
		AfxMessageBox(_T("모델을 선택하세요."), MB_ICONWARNING);
		return;
	}

	m_ModelList.GetText(index, m_EditSelectedModel);
	CString strModelFolderPath;
	if (!THEAPP.Struct_PreferenceStruct.m_bUseAbsolutePathModel) // Ver2629
		strModelFolderPath = THEAPP.GetWorkingDirectory() + "\\Model\\" + m_EditSelectedModel;
	else
		strModelFolderPath = THEAPP.GetWorkingEvmsDirectory() + "\\Model\\" + m_EditSelectedModel;

	// 2. ShellExecute 함수를 사용하여 폴더를 엽니다.
	// ----------------------------------------------------
	// ShellExecute(
	//     hWnd,                 // 윈도우 핸들 (보통 NULL 또는 GetSafeHwnd())
	//     lpOperation,          // 수행할 작업: L"open" 또는 _T("open")
	//     lpFile,               // 열고자 하는 파일 또는 폴더 경로
	//     lpParameters,         // 매개변수 (폴더 열기 시에는 NULL)
	//     lpDirectory,          // 작업 디렉터리 (NULL 사용)
	//     nShowCmd              // 창 표시 방식 (SW_SHOWNORMAL 사용)
	// );
	// ----------------------------------------------------

	// 3. 실행!
	// 폴더 경로는 CString::GetString() 또는 (LPCTSTR) 캐스팅으로 전달합니다.
	// GetSafeHwnd() : 현재 대화 상자의 윈도우 핸들을 반환합니다. (유효성 검사 포함된 함수)
	HINSTANCE hInst = ShellExecute(
		GetSafeHwnd(),					// 현재 윈도우 핸들
		_T("open"),						// 탐색기로 열기 명령
		strModelFolderPath.GetString(), // 열고자 하는 폴더 경로
		NULL,							// 매개변수 없음
		NULL,							// 작업 디렉터리 없음
		SW_SHOWNORMAL					// 일반 창으로 표시
	);

	// 4. 실행 결과 확인 및 오류 처리 (선택 사항)
	// ShellExecute의 반환 값은 32보다 커야 성공을 의미합니다.
	if ((int)hInst <= 32)
	{
		// 32 이하의 값은 오류 코드를 의미합니다 (예: 파일 없음, 권한 부족 등)
		CString strError;
		strError.Format(_T("폴더 열기에 실패했습니다. 오류 코드: %d"), (int)hInst);
		AfxMessageBox(strError, MB_ICONERROR);

		return;
	}
}
