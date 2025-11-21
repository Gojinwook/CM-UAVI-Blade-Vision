#include "stdafx.h"
#include "uScan.h"
#include "ConditionGridCtrl.h"

CConditionGridCtrl::CConditionGridCtrl(void)
{
}

CConditionGridCtrl::~CConditionGridCtrl(void)
{
}

void CConditionGridCtrl::OnSetup()
{
	int iNoRows = MAX_INSPECTION_NUMBER;
	int iNoColumns = 8;

	CUGCell cell ;

	GetHeadingDefault(&cell);
	cell.SetAlignment(UG_ALIGNCENTER | UG_ALIGNVCENTER);
	SetHeadingDefault(&cell); 

	GetGridDefault(&cell);
	cell.SetAlignment(UG_ALIGNCENTER | UG_ALIGNVCENTER);
	SetGridDefault(&cell); 

	SetSH_Width(0);
	SetTH_Height(30);

	SetUniformRowHeight(TRUE); 
	SetCurrentCellMode(3);

	CString sTemp;

	SetNumberRows(iNoRows);
	SetNumberCols(iNoColumns);
	
	QuickSetText(0, -1, "      검사항목      ");
	QuickSetText(2, -1, "최소 면적");
	QuickSetText(3, -1, "중간 면적");
	QuickSetText(4, -1, "최대 면적");
	QuickSetText(5, -1, "허용수량(면적<최소)");
	QuickSetText(6, -1, "허용수량(면적>=최소)");
	QuickSetText(7, -1, "허용수량(면적>=중간)");

	m_NameFont.CreateFont(14,0,0,0,FW_BOLD,FALSE,FALSE,0,0,0,0,0,0,"Arial");
	
	GetColDefault(0, &cell);
	cell.SetFont(&m_NameFont);
	cell.SetTextColor(RGB(0,0,255));
	SetColDefault( 0, &cell);
	
	BestFit(0, iNoColumns-1, -1, UG_BESTFIT_TOPHEADINGS);
	
	RedrawAll(); 
}

BOOL CConditionGridCtrl::IsItemChecked(long lIndex)
{
	CUGCell cell;

	GetCell(0, lIndex, &cell);
	return cell.GetBool();
}

BOOL CConditionGridCtrl::SetItemCheck(long lIndex, BOOL bChecked)
{
	CUGCell cell;
	if (GetCell(0, lIndex, &cell) != UG_SUCCESS) return FALSE;

	cell.SetBool(bChecked);
	if (SetCell(0, lIndex, &cell) != UG_SUCCESS) return FALSE;

	return TRUE;
}

void CConditionGridCtrl::OnDClicked(int col,long row,RECT *rect,POINT *point,BOOL processed)
{
	if (col == 1) {
		CUGCell cell;

		GetCell(col, row, &cell);
		COLORREF oriColor;
		oriColor = cell.GetBackColor();

		CColorDialog kColorDlg;

		kColorDlg.m_cc.Flags = kColorDlg.m_cc.Flags | CC_FULLOPEN | CC_ANYCOLOR; // 확장형으로 열자
		kColorDlg.m_cc.lpCustColors = &oriColor;

		if (kColorDlg.DoModal() == IDOK) {
			cell.SetBackColor((long)kColorDlg.GetColor());
			SetCell(col, row, &cell);
		}
		RedrawCell(col, row);
	} else {
		StartEdit();
	}
}

int CConditionGridCtrl::OnEditFinish( int col, long row, CWnd* edit, LPCTSTR string, BOOL cancelFlag)
{
	if (col == 0) {
		int iNoRow = GetNumberRows();
		BestFit(col, col, iNoRow, UG_BESTFIT_TOPHEADINGS);
		RedrawCol(col);
	}
	return TRUE; 
} 