#pragma once

#include "ResultViewSelectDlg.h"

// CTrayImagePB

class CTrayImagePB : public CStatic
{
	DECLARE_DYNAMIC(CTrayImagePB)

public:
	CTrayImagePB();
	
	virtual ~CTrayImagePB();
	char* OkNg;
	BOOL* ClickRegion;
	BOOL WrongPointClick;
	CRect SetPictureBoxSize(CRect PbRect);

	Hobject RegionErosion;
	HTuple WindowHandle;
	HTuple  Row, Column, Button,Index;

	BOOL bOnDrawing;
protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnPaint();

	LRESULT OnDispTray(WPARAM wParam,LPARAM lParam);
	void ReDraw();
};


