// CModelLoadProgressDlg.cpp: 구현 파일
//

#include "StdAfx.h"
#include "uScan.h"
#include "Define.h"
#include "ModelLoadProgressDlg.h"
#include "ModelDataManager.h"

// CModelLoadProgressDlg 대화 상자

IMPLEMENT_DYNAMIC(CModelLoadProgressDlg, CDialog)

CModelLoadProgressDlg::~CModelLoadProgressDlg()
{
}

void CModelLoadProgressDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PROGRESS_MODEL_LOAD, m_ProgressModelLoadDlg);
}

BOOL CModelLoadProgressDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_ProgressModelLoadDlg.SetRange(0, 100);
	m_ProgressModelLoadDlg.SetPos(0);

	return TRUE; // return TRUE unless you set the focus to a control
				  // 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

BEGIN_MESSAGE_MAP(CModelLoadProgressDlg, CDialog)
	ON_MESSAGE(UM_UPDATE_PROGRESS_START, &CModelLoadProgressDlg::OnUpdateProgress)
	ON_MESSAGE(UM_UPDATE_PROGRESS, &CModelLoadProgressDlg::OnUpdateProgress)
	ON_MESSAGE(UM_UPDATE_PROGRESS_DONE, &CModelLoadProgressDlg::OnUpdateProgressDone)
	ON_WM_TIMER()
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_PROGRESS_MODEL_LOAD, &CModelLoadProgressDlg::OnNMCustomdrawProgressModelLoad)
END_MESSAGE_MAP()

afx_msg LRESULT CModelLoadProgressDlg::OnUpdateProgress(WPARAM wParam, LPARAM lParam)
{
	// m_ProgressModelLoadDlg.OffsetPos(10);
	// EndDialog(IDOK);

	return 0;
}

afx_msg LRESULT CModelLoadProgressDlg::OnUpdateProgressDone(WPARAM wParam, LPARAM lParam)
{
	// m_ProgressModelLoadDlg.SetPos(100);

	return 0;
}


void CModelLoadProgressDlg::OnNMCustomdrawProgressModelLoad(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	*pResult = 0;
}
