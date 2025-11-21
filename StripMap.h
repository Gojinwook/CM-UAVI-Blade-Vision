// StripMap.h: interface for the CStripMap class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_STRIPMAP_H__BF56944B_A511_40E4_80F0_91C52842EC5A__INCLUDED_)
#define AFX_STRIPMAP_H__BF56944B_A511_40E4_80F0_91C52842EC5A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CStripMap  
{
public:
	CStripMap();
	virtual ~CStripMap();

	POINT	m_LTPoint;
	POINT	m_RBPoint;
	int		m_nArrayIdx;
	int		m_nPkgXIdx;
	int		m_nPkgYIdx;
// 	BOOL	m_bChecked;

	void Reset();
	void Load(CArchive& arIn);
	void Save(CArchive& arOut);
	CStripMap& operator=(CStripMap& data);
};

#endif // !defined(AFX_STRIPMAP_H__BF56944B_A511_40E4_80F0_91C52842EC5A__INCLUDED_)
