#pragma once

#include "ugctrl.h"

class CTeachAlgorithmListCtrl :
	public CUGCtrl
{
public:
	CTeachAlgorithmListCtrl(void);
	virtual ~CTeachAlgorithmListCtrl(void);

	virtual void OnSetup();

	virtual BOOL IsItemChecked(long lIndex);
	virtual BOOL SetItemCheck(long lIndex, BOOL bChecked);

protected:
	CFont m_NameFont;
};

