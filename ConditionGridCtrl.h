#pragma once

#include "ugctrl.h"

class CConditionGridCtrl :
	public CUGCtrl
{
public:
	CConditionGridCtrl(void);
	virtual ~CConditionGridCtrl(void);

	virtual void OnSetup();

	virtual BOOL IsItemChecked(long lIndex);
	virtual BOOL SetItemCheck(long lIndex, BOOL bChecked);

	virtual void OnDClicked(int col, long row, RECT *rect, POINT *point, BOOL processed);
	virtual int OnEditFinish( int col, long row, CWnd* edit, LPCTSTR string, BOOL cancelFlag);

protected:
	CFont m_NameFont;
};

