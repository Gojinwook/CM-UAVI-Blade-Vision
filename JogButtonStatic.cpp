// JogButtonStatic.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "uScan.h"
#include "JogButtonStatic.h"


// CJogButtonStatic

IMPLEMENT_DYNAMIC(CJogButtonStatic, CStatic)

CJogButtonStatic::CJogButtonStatic()
{

}

CJogButtonStatic::~CJogButtonStatic()
{
}


BEGIN_MESSAGE_MAP(CJogButtonStatic, CStatic)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
END_MESSAGE_MAP()



// CJogButtonStatic 메시지 처리기입니다.




void CJogButtonStatic::OnLButtonDown(UINT nFlags, CPoint point)
{
//Set_JMoveRequest(int XDirection,int YDirection)//조그이동 (1 : +이동, -1 : -이동, 0: 정지)(""이면 움직이지 않는다)	

	CStatic::OnLButtonDown(nFlags, point);
}


void CJogButtonStatic::OnLButtonUp(UINT nFlags, CPoint point)
{
	CStatic::OnLButtonUp(nFlags, point);
}
