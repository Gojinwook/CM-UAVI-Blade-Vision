// SelectPartRegion.h: interface for the CSelectPartRegion class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SELECTPARTREGION_H__23746C61_0CDC_47A0_9989_5AC7AD847E68__INCLUDED_)
#define AFX_SELECTPARTREGION_H__23746C61_0CDC_47A0_9989_5AC7AD847E68__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "GTRegion.h"

class CSelectPartRegion : public GTRegion
{
	DECLARE_DYNCLASS(CSelectPartRegion) 

public:
	CSelectPartRegion();
	virtual ~CSelectPartRegion();

	void Duplicate(GTRegion **ppTRegion);
};

#endif // !defined(AFX_SELECTPARTREGION_H__23746C61_0CDC_47A0_9989_5AC7AD847E68__INCLUDED_)
