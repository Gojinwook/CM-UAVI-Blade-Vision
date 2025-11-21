#pragma once


// CJogButtonStatic

class CJogButtonStatic : public CStatic
{
	DECLARE_DYNAMIC(CJogButtonStatic)

public:
	CJogButtonStatic();
	virtual ~CJogButtonStatic();

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);

	int m_nType;
};


