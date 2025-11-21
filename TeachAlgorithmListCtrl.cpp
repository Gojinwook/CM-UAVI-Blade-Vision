#include "stdafx.h"
#include "uScan.h"
#include "TeachAlgorithmListCtrl.h"

CTeachAlgorithmListCtrl::CTeachAlgorithmListCtrl(void)
{
}

CTeachAlgorithmListCtrl::~CTeachAlgorithmListCtrl(void)
{
}

void CTeachAlgorithmListCtrl::OnSetup()
{
	int iNoRows = 5;
	int iNoColumns = 3;

	CUGCell cell ;
	CString Temp;
	Temp="Arial";
	m_NameFont.CreateFont(14,0,0,0,600,0,0,0,ANSI_CHARSET,0,0,0,0,Temp);

	GetHeadingDefault(&cell);
	cell.SetAlignment(UG_ALIGNCENTER | UG_ALIGNVCENTER);
	cell.SetFont(&m_NameFont); 
	cell.SetBackColor(RGB(255,200,0));
	cell.SetTextColor(RGB(0,0,255)); 
	SetHeadingDefault(&cell); 

	GetGridDefault(&cell);
	cell.SetAlignment(UG_ALIGNCENTER | UG_ALIGNVCENTER);
	cell.SetFont(&m_NameFont); 
	SetGridDefault(&cell); 

	SetSH_Width(0);
	SetTH_Height(22);

	SetUniformRowHeight(TRUE); 
	SetCurrentCellMode(3);

	CString sTemp;

	SetNumberRows(iNoRows);
	SetNumberCols(iNoColumns);
	
	QuickSetText(0, -1, "         알고리즘         ");
	QuickSetText(1, -1, " 밝은 불량 ");
	QuickSetText(2, -1, " 어두운 불량 ");

	QuickSetText(0, 0, " 고정 Threshold ");
	QuickSetText(0, 1, " Dynamic Threshold ");
	QuickSetText(0, 2, " 균일도 검사 ");
	QuickSetText(0, 3, " Hysteresis Threshold ");
	QuickSetText(0, 4, " Edge 분석 ");

	BestFit(0, iNoColumns-1, -1, UG_BESTFIT_TOPHEADINGS);
	
	RedrawAll(); 
}

BOOL CTeachAlgorithmListCtrl::IsItemChecked(long lIndex)
{
	CUGCell cell;

	GetCell(0, lIndex, &cell);
	return cell.GetBool();
}

BOOL CTeachAlgorithmListCtrl::SetItemCheck(long lIndex, BOOL bChecked)
{
	CUGCell cell;
	if (GetCell(0, lIndex, &cell) != UG_SUCCESS) return FALSE;

	cell.SetBool(bChecked);
	if (SetCell(0, lIndex, &cell) != UG_SUCCESS) return FALSE;

	return TRUE;
}

