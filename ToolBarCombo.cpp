// ToolBarCombo.cpp: implementation of the CToolBarCombo class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "uScan.h"
#include "ToolBarCombo.h"

#ifdef _DEBUG
#undef THIS_FILE
static char* THIS_FILE=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CToolBarCombo::CToolBarCombo()
{
}

CToolBarCombo::~CToolBarCombo()
{
}

void CToolBarCombo::Make_ComboBox(int nPos, UINT nCtrl, CSize csSize)
{
	SetButtonInfo(nPos, nCtrl, TBBS_SEPARATOR, csSize.cx);

	CRect rect;
	GetItemRect(nPos, &rect);

    rect.top = 1;
    rect.bottom = rect.top + csSize.cy;
    m_ComboBox.Create(WS_CHILD | CBS_DROPDOWNLIST | WS_VSCROLL |CBRS_SIZE_DYNAMIC, rect, this, nCtrl);
 
    m_ComboBox.ShowWindow(SW_SHOW);
}

int CToolBarCombo::Get_CurComboBox()
{
	int nIndex = 0;
	if (m_ComboBox.GetCount() > 0) {
		CString strText;
		m_ComboBox.GetLBText(m_ComboBox.GetCurSel(), strText);
		nIndex = atoi(strText.Right(1));
	}
	return nIndex;
}

void CToolBarCombo::Set_CurComboBox(CString strSet)
{
	m_ComboBox.SelectString(0, strSet);
}

void CToolBarCombo::Add_ComboBox(CString strAdd)
{
	m_ComboBox.AddString(strAdd);
	m_ComboBox.SelectString(0, strAdd);
}

void CToolBarCombo::Clear_ComboBox()
{
	m_ComboBox.ResetContent();
}

int CToolBarCombo::Get_ComboBoxCount()
{
	return m_ComboBox.GetCount();
}

int CToolBarCombo::Get_ComboBoxIndex()
{
	return m_ComboBox.GetCurSel();
}

void CToolBarCombo::Set_ComboBoxIndex(int nIdx)
{
	m_ComboBox.SetCurSel(nIdx);
}
