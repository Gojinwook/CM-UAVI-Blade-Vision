// DefectPriorityDlg.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "uScan.h"
#include "DefectPriorityDlg.h"
#include "afxdialogex.h"


// CDefectPriorityDlg 대화 상자입니다.

IMPLEMENT_DYNAMIC(CDefectPriorityDlg, CDialog)

	CDefectPriorityDlg::CDefectPriorityDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CDefectPriorityDlg::IDD, pParent)
{
	int i;

	for(i=0; i<MAX_DEFECT_NUMBER; i++)
	{
		pUserColor[i] = RGB(255,255,255);
	}

	for (i=0; i<MAX_DEFECT_NUMBER; i++)
	{
		m_iPriority[i] = -1;
	}
}

CDefectPriorityDlg::~CDefectPriorityDlg()
{
}

void CDefectPriorityDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	DDX_CBIndex(pDX, IDC_COMBO_PRIORITY_1, m_iPriority[0]);
	DDX_CBIndex(pDX, IDC_COMBO_PRIORITY_2, m_iPriority[1]);
	DDX_CBIndex(pDX, IDC_COMBO_PRIORITY_3, m_iPriority[2]);
	DDX_CBIndex(pDX, IDC_COMBO_PRIORITY_4, m_iPriority[3]);
	DDX_CBIndex(pDX, IDC_COMBO_PRIORITY_5, m_iPriority[4]);
	DDX_CBIndex(pDX, IDC_COMBO_PRIORITY_6, m_iPriority[5]);
	DDX_CBIndex(pDX, IDC_COMBO_PRIORITY_7, m_iPriority[6]);
	DDX_CBIndex(pDX, IDC_COMBO_PRIORITY_8, m_iPriority[7]);

}


BEGIN_MESSAGE_MAP(CDefectPriorityDlg, CDialog)
	ON_BN_CLICKED(IDOK, &CDefectPriorityDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CDefectPriorityDlg::OnBnClickedCancel)

	ON_CBN_SELCHANGE(IDC_COMBO_PRIORITY_1, OnSelchangeComboPriority_1)
	ON_CBN_SELCHANGE(IDC_COMBO_PRIORITY_2, OnSelchangeComboPriority_2)
	ON_CBN_SELCHANGE(IDC_COMBO_PRIORITY_3, OnSelchangeComboPriority_3)
	ON_CBN_SELCHANGE(IDC_COMBO_PRIORITY_4, OnSelchangeComboPriority_4)
	ON_CBN_SELCHANGE(IDC_COMBO_PRIORITY_5, OnSelchangeComboPriority_5)
	ON_CBN_SELCHANGE(IDC_COMBO_PRIORITY_6, OnSelchangeComboPriority_6)
	ON_CBN_SELCHANGE(IDC_COMBO_PRIORITY_7, OnSelchangeComboPriority_7)
	ON_CBN_SELCHANGE(IDC_COMBO_PRIORITY_8, OnSelchangeComboPriority_8)

	ON_BN_CLICKED(IDC_BUTTON_REVIEW_COLOR_1, &CDefectPriorityDlg::OnClickedButtonReviewColor1)
	ON_BN_CLICKED(IDC_BUTTON_REVIEW_COLOR_2, &CDefectPriorityDlg::OnClickedButtonReviewColor2)
	ON_BN_CLICKED(IDC_BUTTON_REVIEW_COLOR_3, &CDefectPriorityDlg::OnClickedButtonReviewColor3)
	ON_BN_CLICKED(IDC_BUTTON_REVIEW_COLOR_4, &CDefectPriorityDlg::OnClickedButtonReviewColor4)
	ON_BN_CLICKED(IDC_BUTTON_REVIEW_COLOR_5, &CDefectPriorityDlg::OnClickedButtonReviewColor5)
	ON_BN_CLICKED(IDC_BUTTON_REVIEW_COLOR_6, &CDefectPriorityDlg::OnBnClickedButtonReviewColor6)
	ON_BN_CLICKED(IDC_BUTTON_REVIEW_COLOR_7, &CDefectPriorityDlg::OnBnClickedButtonReviewColor7)
	ON_BN_CLICKED(IDC_BUTTON_REVIEW_COLOR_8, &CDefectPriorityDlg::OnBnClickedButtonReviewColor8)

	ON_WM_DRAWITEM()
END_MESSAGE_MAP()


// CDefectPriorityDlg 메시지 처리기입니다.


BOOL CDefectPriorityDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.

	for (int i=0; i<MAX_DEFECT_NUMBER; i++)
	{
		m_lNGColor[i] = THEAPP.Struct_PreferenceStruct.lNGColor[i];
		m_iPriority[i] = THEAPP.Struct_PreferenceStruct.iDefectPriority[i];
	}

	UpdateData(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}


void CDefectPriorityDlg::OnBnClickedOk()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	UpdateData();

	for (int i=0; i<MAX_DEFECT_NUMBER; i++)
	{
		THEAPP.Struct_PreferenceStruct.lNGColor[i] = m_lNGColor[i];
		THEAPP.Struct_PreferenceStruct.iDefectPriority[i] = m_iPriority[i];
	}

	CString strModelFolder = THEAPP.GetWorkingDirectory()+"\\Data\\";
	CIniFileCS INI(strModelFolder+"\\Prefecrence.ini");
	CString strSection, strTemp, strValue;

	strSection = "Defect_Priority";
	for (int i = 0; i < MAX_DEFECT_NUMBER; i++)
	{
		strValue.Format("NG_PRIORITY_%d", i + 1);
		INI.Set_Integer(strSection, strValue, m_iPriority[i]);
	}

	strSection = "Defect_Color";
	for (int i = 0; i < MAX_DEFECT_NUMBER; i++)
	{
		strValue.Format("COLOR_DEFECT_%d", i + 1);
		strTemp.Format("%ld", m_lNGColor[i]);
		INI.Set_String(strSection, strValue, strTemp);
	}

	CDialog::OnOK();
}


void CDefectPriorityDlg::OnBnClickedCancel()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CDialog::OnCancel();
}

void CDefectPriorityDlg::RGBToHSV( double r, double g, double b, double *h, double *s, double *v)
{
	if( r > 255.) r = 255.;
	if( g > 255.) g = 255.;
	if( b > 255.) b = 255.;

	unsigned char minval = MIN(r, MIN(g, b));
	unsigned char maxval = MAX(r, MAX(g, b));
	double mdiff  = double(maxval) - double(minval);
	double msum   = double(maxval) + double(minval);

	*v = msum / 510.0;

	if (maxval == minval) 
	{
		*s = 0.0f;
		*h = 0.0f; 
	}   
	else 
	{ 
		double rnorm = (maxval - r  ) / mdiff;      
		double gnorm = (maxval - g) / mdiff;
		double bnorm = (maxval - b ) / mdiff;   

		*s = (*v <= 0.5) ? (mdiff / msum) : (mdiff / (510.0 - msum));

		if (r   == maxval) *h = 60.0 * (6.0 + bnorm - gnorm);
		if (g == maxval) *h = 60.0 * (2.0 + rnorm - bnorm);
		if (b  == maxval) *h = 60.0 * (4.0 + gnorm - rnorm);
		if (*h > 360.0) *h = *h - 360.0;
	}
}

void CDefectPriorityDlg::HSVToRGB( double h, double s, double v, double *r, double *g, double *b)
{
	if (h<0.0)
		h = 0.0;
	if (h>360.0)
		h = 360.0;

	if (s<0.0)
		s = 0.0;
	if (s>1.0)
		s = 1.0;

	if (v<0.0)
		v = 0.0;
	if (v>1.0)
		v = 1.0;

	if (s == 0.0) // Grauton, einfacher Fall
	{
		*r = *g = *b = unsigned char(v * 255.0);
	}
	else
	{
		double rm1, rm2;

		if (v <= 0.5) rm2 = v + v * s;  
		else                     rm2 = v + s - v * s;

		rm1 = 2.0f * v - rm2;   
		*r   = ToRGB1(rm1, rm2, h + 120.0);   
		*g = ToRGB1(rm1, rm2, h);
		*b  = ToRGB1(rm1, rm2, h - 120.0);
	}
}

unsigned char CDefectPriorityDlg::ToRGB1(double rm1, double rm2, double rh)
{
	if      (rh > 360.0) rh -= 360.0;
	else if (rh <   0.0) rh += 360.0;

	if      (rh <  60.0) rm1 = rm1 + (rm2 - rm1) * rh / 60.0;   
	else if (rh < 180.0) rm1 = rm2;
	else if (rh < 240.0) rm1 = rm1 + (rm2 - rm1) * (240.0 - rh) / 60.0;      

	return static_cast<unsigned char>(rm1 * 255);
}

void CDefectPriorityDlg::DWORDToRGB( DWORD color, DWORD *r, DWORD *g, DWORD *b)
{
	*b = (color << 8) >> 24;
	*g = (color << 16) >> 24;
	*r = (color << 24) >> 24;
}

void CDefectPriorityDlg::RGBToDWORD( DWORD r, DWORD g, DWORD b, DWORD *color)
{	
	if( r > 255.) r = 255.;
	if( g > 255.) g = 255.;
	if( b > 255.) b = 255.;

	*color = (b << 16) + (g << 8) + (r);
}

void CDefectPriorityDlg::OnSelchangeComboPriority_1() 
{
	ChangePriority(0);
}

void CDefectPriorityDlg::OnSelchangeComboPriority_2() 
{
	ChangePriority(1);
}

void CDefectPriorityDlg::OnSelchangeComboPriority_3() 
{
	ChangePriority(2);
}

void CDefectPriorityDlg::OnSelchangeComboPriority_4() 
{
	ChangePriority(3);
}

void CDefectPriorityDlg::OnSelchangeComboPriority_5() 
{
	ChangePriority(4);
}

void CDefectPriorityDlg::OnSelchangeComboPriority_6() 
{
	ChangePriority(5);
}

void CDefectPriorityDlg::OnSelchangeComboPriority_7() 
{
	ChangePriority(6);
}

void CDefectPriorityDlg::OnSelchangeComboPriority_8() 
{
	ChangePriority(7);
}

void CDefectPriorityDlg::ChangePriority(int iSelectedIndex)
{
	int i;
	int temp[MAX_DEFECT_NUMBER];

	for (i=0; i<MAX_DEFECT_NUMBER; i++)
	{
		if (i==iSelectedIndex)
			continue;
		temp[i] = m_iPriority[i];
	}

	for (i=0; i<MAX_DEFECT_NUMBER; i++)
	{
		if (i==iSelectedIndex)
			continue;

		if ( temp[i] >= m_iPriority[iSelectedIndex]) 
			--(temp[i]);
	}

	UpdateData(TRUE);

	for (i=0; i<MAX_DEFECT_NUMBER; i++)
	{
		if (i==iSelectedIndex)
			continue;

		if ( temp[i] >= m_iPriority[iSelectedIndex]) 
			++(temp[i]);
	}

	for (i=0; i<MAX_DEFECT_NUMBER; i++)
	{
		if (i==iSelectedIndex)
			continue;

		m_iPriority[i] = temp[i];
	}

	UpdateData( FALSE);
}

void CDefectPriorityDlg::OnClickedButtonReviewColor1()
{
	CColorDialog kColorDlg;

	kColorDlg.m_cc.Flags = kColorDlg.m_cc.Flags | CC_FULLOPEN | CC_ANYCOLOR; // 확장형으로 열자
	kColorDlg.m_cc.lpCustColors = pUserColor;

	if(kColorDlg.DoModal() == IDOK) {
		m_lNGColor[0] = (long)kColorDlg.GetColor();
		Invalidate();
	}
}

void CDefectPriorityDlg::OnClickedButtonReviewColor2()
{
	CColorDialog kColorDlg;

	kColorDlg.m_cc.Flags = kColorDlg.m_cc.Flags | CC_FULLOPEN | CC_ANYCOLOR; // 확장형으로 열자
	kColorDlg.m_cc.lpCustColors = pUserColor;

	if(kColorDlg.DoModal() == IDOK) {
		m_lNGColor[1] = (long)kColorDlg.GetColor();
		Invalidate();
	}
}

void CDefectPriorityDlg::OnClickedButtonReviewColor3()
{
	CColorDialog kColorDlg;

	kColorDlg.m_cc.Flags = kColorDlg.m_cc.Flags | CC_FULLOPEN | CC_ANYCOLOR; // 확장형으로 열자
	kColorDlg.m_cc.lpCustColors = pUserColor;

	if(kColorDlg.DoModal() == IDOK) {
		m_lNGColor[2] = (long)kColorDlg.GetColor();
		Invalidate();
	}
}

void CDefectPriorityDlg::OnClickedButtonReviewColor4()
{
	CColorDialog kColorDlg;

	kColorDlg.m_cc.Flags = kColorDlg.m_cc.Flags | CC_FULLOPEN | CC_ANYCOLOR; // 확장형으로 열자
	kColorDlg.m_cc.lpCustColors = pUserColor;

	if(kColorDlg.DoModal() == IDOK) {
		m_lNGColor[3] = (long)kColorDlg.GetColor();
		Invalidate();
	}
}

void CDefectPriorityDlg::OnClickedButtonReviewColor5()
{
	CColorDialog kColorDlg;

	kColorDlg.m_cc.Flags = kColorDlg.m_cc.Flags | CC_FULLOPEN | CC_ANYCOLOR; // 확장형으로 열자
	kColorDlg.m_cc.lpCustColors = pUserColor;

	if(kColorDlg.DoModal() == IDOK) {
		m_lNGColor[4] = (long)kColorDlg.GetColor();
		Invalidate();
	}
}

void CDefectPriorityDlg::OnBnClickedButtonReviewColor6()
{
	CColorDialog kColorDlg;

	kColorDlg.m_cc.Flags = kColorDlg.m_cc.Flags | CC_FULLOPEN | CC_ANYCOLOR; // 확장형으로 열자
	kColorDlg.m_cc.lpCustColors = pUserColor;

	if(kColorDlg.DoModal() == IDOK) {
		m_lNGColor[5] = (long)kColorDlg.GetColor();
		Invalidate();
	}
}


void CDefectPriorityDlg::OnBnClickedButtonReviewColor7()
{
	CColorDialog kColorDlg;

	kColorDlg.m_cc.Flags = kColorDlg.m_cc.Flags | CC_FULLOPEN | CC_ANYCOLOR; // 확장형으로 열자
	kColorDlg.m_cc.lpCustColors = pUserColor;

	if(kColorDlg.DoModal() == IDOK) {
		m_lNGColor[6] = (long)kColorDlg.GetColor();
		Invalidate();
	}
}


void CDefectPriorityDlg::OnBnClickedButtonReviewColor8()
{
	CColorDialog kColorDlg;

	kColorDlg.m_cc.Flags = kColorDlg.m_cc.Flags | CC_FULLOPEN | CC_ANYCOLOR; // 확장형으로 열자
	kColorDlg.m_cc.lpCustColors = pUserColor;

	if(kColorDlg.DoModal() == IDOK) {
		m_lNGColor[7] = (long)kColorDlg.GetColor();
		Invalidate();
	}
}


void CDefectPriorityDlg::OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.

	if (nIDCtl==IDC_BUTTON_REVIEW_COLOR_1)
	{
		CDC dc;

		dc.Attach(lpDrawItemStruct ->hDC);
		RECT rect;
		rect= lpDrawItemStruct ->rcItem;

		dc.Draw3dRect(&rect,RGB(255,255,255),RGB(0,0,0));
		dc.FillSolidRect(&rect,m_lNGColor[0]);
		UINT state=lpDrawItemStruct->itemState;

		if((state & ODS_SELECTED))
		{
			dc.DrawEdge(&rect,EDGE_SUNKEN,BF_RECT);

		}
		else
		{
			dc.DrawEdge(&rect,EDGE_RAISED,BF_RECT);
		}

		dc.SetBkColor(m_lNGColor[0]);
		dc.SetTextColor(RGB(0,0,0));

		TCHAR buffer[MAX_PATH];
		ZeroMemory(buffer,MAX_PATH );
		::GetWindowText(lpDrawItemStruct->hwndItem,buffer,MAX_PATH);
		dc.DrawText(buffer,&rect,DT_CENTER|DT_VCENTER|DT_SINGLELINE);

		dc.Detach();
	}

	if (nIDCtl==IDC_BUTTON_REVIEW_COLOR_2)
	{
		CDC dc;

		dc.Attach(lpDrawItemStruct ->hDC);
		RECT rect;
		rect= lpDrawItemStruct ->rcItem;

		dc.Draw3dRect(&rect,RGB(255,255,255),RGB(0,0,0));
		dc.FillSolidRect(&rect,m_lNGColor[1]);
		UINT state=lpDrawItemStruct->itemState;

		if((state & ODS_SELECTED))
		{
			dc.DrawEdge(&rect,EDGE_SUNKEN,BF_RECT);

		}
		else
		{
			dc.DrawEdge(&rect,EDGE_RAISED,BF_RECT);
		}

		dc.SetBkColor(m_lNGColor[1]);
		dc.SetTextColor(RGB(0,0,0));

		TCHAR buffer[MAX_PATH];
		ZeroMemory(buffer,MAX_PATH );
		::GetWindowText(lpDrawItemStruct->hwndItem,buffer,MAX_PATH);
		dc.DrawText(buffer,&rect,DT_CENTER|DT_VCENTER|DT_SINGLELINE);

		dc.Detach();
	}

	if (nIDCtl==IDC_BUTTON_REVIEW_COLOR_3)
	{
		CDC dc;

		dc.Attach(lpDrawItemStruct ->hDC);
		RECT rect;
		rect= lpDrawItemStruct ->rcItem;

		dc.Draw3dRect(&rect,RGB(255,255,255),RGB(0,0,0));
		dc.FillSolidRect(&rect,m_lNGColor[2]);
		UINT state=lpDrawItemStruct->itemState;

		if((state & ODS_SELECTED))
		{
			dc.DrawEdge(&rect,EDGE_SUNKEN,BF_RECT);

		}
		else
		{
			dc.DrawEdge(&rect,EDGE_RAISED,BF_RECT);
		}

		dc.SetBkColor(m_lNGColor[2]);
		dc.SetTextColor(RGB(0,0,0));

		TCHAR buffer[MAX_PATH];
		ZeroMemory(buffer,MAX_PATH );
		::GetWindowText(lpDrawItemStruct->hwndItem,buffer,MAX_PATH);
		dc.DrawText(buffer,&rect,DT_CENTER|DT_VCENTER|DT_SINGLELINE);

		dc.Detach();
	}

	if (nIDCtl==IDC_BUTTON_REVIEW_COLOR_4)
	{
		CDC dc;

		dc.Attach(lpDrawItemStruct ->hDC);
		RECT rect;
		rect= lpDrawItemStruct ->rcItem;

		dc.Draw3dRect(&rect,RGB(255,255,255),RGB(0,0,0));
		dc.FillSolidRect(&rect,m_lNGColor[3]);
		UINT state=lpDrawItemStruct->itemState;

		if((state & ODS_SELECTED))
		{
			dc.DrawEdge(&rect,EDGE_SUNKEN,BF_RECT);

		}
		else
		{
			dc.DrawEdge(&rect,EDGE_RAISED,BF_RECT);
		}

		dc.SetBkColor(m_lNGColor[3]);
		dc.SetTextColor(RGB(0,0,0));

		TCHAR buffer[MAX_PATH];
		ZeroMemory(buffer,MAX_PATH );
		::GetWindowText(lpDrawItemStruct->hwndItem,buffer,MAX_PATH);
		dc.DrawText(buffer,&rect,DT_CENTER|DT_VCENTER|DT_SINGLELINE);

		dc.Detach();
	}

	if (nIDCtl==IDC_BUTTON_REVIEW_COLOR_5)
	{
		CDC dc;

		dc.Attach(lpDrawItemStruct ->hDC);
		RECT rect;
		rect= lpDrawItemStruct ->rcItem;

		dc.Draw3dRect(&rect,RGB(255,255,255),RGB(0,0,0));
		dc.FillSolidRect(&rect,m_lNGColor[4]);
		UINT state=lpDrawItemStruct->itemState;

		if((state & ODS_SELECTED))
		{
			dc.DrawEdge(&rect,EDGE_SUNKEN,BF_RECT);

		}
		else
		{
			dc.DrawEdge(&rect,EDGE_RAISED,BF_RECT);
		}

		dc.SetBkColor(m_lNGColor[4]);
		dc.SetTextColor(RGB(0,0,0));

		TCHAR buffer[MAX_PATH];
		ZeroMemory(buffer,MAX_PATH );
		::GetWindowText(lpDrawItemStruct->hwndItem,buffer,MAX_PATH);
		dc.DrawText(buffer,&rect,DT_CENTER|DT_VCENTER|DT_SINGLELINE);

		dc.Detach();
	}

	if (nIDCtl==IDC_BUTTON_REVIEW_COLOR_6)
	{
		CDC dc;

		dc.Attach(lpDrawItemStruct ->hDC);
		RECT rect;
		rect= lpDrawItemStruct ->rcItem;

		dc.Draw3dRect(&rect,RGB(255,255,255),RGB(0,0,0));
		dc.FillSolidRect(&rect,m_lNGColor[5]);
		UINT state=lpDrawItemStruct->itemState;

		if((state & ODS_SELECTED))
		{
			dc.DrawEdge(&rect,EDGE_SUNKEN,BF_RECT);

		}
		else
		{
			dc.DrawEdge(&rect,EDGE_RAISED,BF_RECT);
		}

		dc.SetBkColor(m_lNGColor[5]);
		dc.SetTextColor(RGB(0,0,0));

		TCHAR buffer[MAX_PATH];
		ZeroMemory(buffer,MAX_PATH );
		::GetWindowText(lpDrawItemStruct->hwndItem,buffer,MAX_PATH);
		dc.DrawText(buffer,&rect,DT_CENTER|DT_VCENTER|DT_SINGLELINE);

		dc.Detach();
	}

	if (nIDCtl==IDC_BUTTON_REVIEW_COLOR_7)
	{
		CDC dc;

		dc.Attach(lpDrawItemStruct ->hDC);
		RECT rect;
		rect= lpDrawItemStruct ->rcItem;

		dc.Draw3dRect(&rect,RGB(255,255,255),RGB(0,0,0));
		dc.FillSolidRect(&rect,m_lNGColor[6]);
		UINT state=lpDrawItemStruct->itemState;

		if((state & ODS_SELECTED))
		{
			dc.DrawEdge(&rect,EDGE_SUNKEN,BF_RECT);

		}
		else
		{
			dc.DrawEdge(&rect,EDGE_RAISED,BF_RECT);
		}

		dc.SetBkColor(m_lNGColor[6]);
		dc.SetTextColor(RGB(0,0,0));

		TCHAR buffer[MAX_PATH];
		ZeroMemory(buffer,MAX_PATH );
		::GetWindowText(lpDrawItemStruct->hwndItem,buffer,MAX_PATH);
		dc.DrawText(buffer,&rect,DT_CENTER|DT_VCENTER|DT_SINGLELINE);

		dc.Detach();
	}

	if (nIDCtl==IDC_BUTTON_REVIEW_COLOR_8)
	{
		CDC dc;

		dc.Attach(lpDrawItemStruct ->hDC);
		RECT rect;
		rect= lpDrawItemStruct ->rcItem;

		dc.Draw3dRect(&rect,RGB(255,255,255),RGB(0,0,0));
		dc.FillSolidRect(&rect,m_lNGColor[7]);
		UINT state=lpDrawItemStruct->itemState;

		if((state & ODS_SELECTED))
		{
			dc.DrawEdge(&rect,EDGE_SUNKEN,BF_RECT);

		}
		else
		{
			dc.DrawEdge(&rect,EDGE_RAISED,BF_RECT);
		}

		dc.SetBkColor(m_lNGColor[7]);
		dc.SetTextColor(RGB(0,0,0));

		TCHAR buffer[MAX_PATH];
		ZeroMemory(buffer,MAX_PATH );
		::GetWindowText(lpDrawItemStruct->hwndItem,buffer,MAX_PATH);
		dc.DrawText(buffer,&rect,DT_CENTER|DT_VCENTER|DT_SINGLELINE);

		dc.Detach();
	}

	CDialog::OnDrawItem(nIDCtl, lpDrawItemStruct);
}

