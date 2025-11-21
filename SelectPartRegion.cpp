// SelectPartRegion.cpp: implementation of the CSelectPartRegion class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "uScan.H"
#include "SelectPartRegion.h"

#ifdef _DEBUG
#undef THIS_FILE
static char* THIS_FILE=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNCLASS(CSelectPartRegion)

CSelectPartRegion::CSelectPartRegion()
{
	miLineStyle = PS_DOT;
	miLineThickness = 0.1;
	mLineColor = RGB(255, 0, 0);
}

CSelectPartRegion::~CSelectPartRegion()
{
}

void CSelectPartRegion::Duplicate(GTRegion **ppTRegion)
{
	if (GetDynClass()->mName == "CSelectPartRegion") *ppTRegion = new CSelectPartRegion;
	GTRegion::Duplicate(ppTRegion);
}
