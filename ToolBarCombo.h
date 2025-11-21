// ToolBarCombo.h: interface for the CToolBarCombo class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TOOLBARCOMBO_H__DEF61458_4F1D_4152_B0CE_641B4D4949A4__INCLUDED_)
#define AFX_TOOLBARCOMBO_H__DEF61458_4F1D_4152_B0CE_641B4D4949A4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CToolBarCombo : public CToolBar  
{
public:
	CToolBarCombo();
	virtual ~CToolBarCombo();

protected:
	CComboBox m_ComboBox;

public:
	void Make_ComboBox(int nPos, UINT nCtrl, CSize csSize);

	int  Get_CurComboBox();
	void Set_CurComboBox(CString strSet);
	void Add_ComboBox(CString strAdd);
	void Clear_ComboBox();

	int Get_ComboBoxCount();
	int Get_ComboBoxIndex();
	void Set_ComboBoxIndex(int nIdx);
};

#endif // !defined(AFX_TOOLBARCOMBO_H__DEF61458_4F1D_4152_B0CE_641B4D4949A4__INCLUDED_)
