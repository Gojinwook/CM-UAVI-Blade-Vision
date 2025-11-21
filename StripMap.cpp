// StripMap.cpp: implementation of the CStripMap class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "uscan.h"
#include "StripMap.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CStripMap::CStripMap()
{
	Reset();
}

CStripMap::~CStripMap()
{
}

void CStripMap::Reset()
{
	m_LTPoint.x = m_LTPoint.y = 0;
	m_RBPoint.x = m_RBPoint.y = 0;
	m_nArrayIdx = -1;
	m_nPkgXIdx = -1;
	m_nPkgYIdx = -1;
// 	m_bChecked = FALSE;
}

void CStripMap::Load(CArchive& arIn)
{
	arIn >> m_LTPoint.x;
	arIn >> m_LTPoint.y;
	arIn >> m_RBPoint.x;
	arIn >> m_RBPoint.y;
	arIn >> m_nArrayIdx;
	arIn >> m_nPkgXIdx;
	arIn >> m_nPkgYIdx;
// 	arIn >> m_bChecked;
}

void CStripMap::Save(CArchive& arOut)
{
	arOut << m_LTPoint.x;
	arOut << m_LTPoint.y;
	arOut << m_RBPoint.x;
	arOut << m_RBPoint.y;
	arOut << m_nArrayIdx;
	arOut << m_nPkgXIdx;
	arOut << m_nPkgYIdx;
// 	arOut << m_bChecked;
}

CStripMap& CStripMap::operator=(CStripMap& data)
{
	if (this == &data) return *this;
	
	m_LTPoint = data.m_LTPoint;
	m_RBPoint = data.m_RBPoint;
	m_nArrayIdx = data.m_nArrayIdx;
	m_nPkgXIdx = data.m_nPkgXIdx;
	m_nPkgYIdx = data.m_nPkgYIdx;
// 	m_bChecked = data.m_bChecked;

	return *this;
}
