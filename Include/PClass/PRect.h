//------------------------------------------------------------------------------
// 작성자           : 조규호
// 작성 날짜        : 1999/02/14
// 최종 수정 날짜   : 1999/02/14
// 파일 이름        : PRect.h
// 설명             : PRect 클래스 헤더 & 구현 파일
// 관련 파일        : 
// 포함된 클래스    : PRect
//------------------------------------------------------------------------------
#ifndef _PRECT_H_
#define _PRECT_H_

#include <windows.h>             //for RECT struct
//------------------------------------------------------------------------------
class 
//#ifdef _MAKEDLL
  AFX_EXT_CLASS
//#endif
PRect : public tagRECT
{
  public:
    PRect();
    PRect(int iLeft, int iTop, int iRight, int iBottom);
    PRect(const RECT &Rect);
  
    void SetRect(int iLeft, int iTop, int iRight, int iBottom);
    bool IntersectRect(LPCRECT pRect1, LPCRECT pRect2);
    bool PtInRect(POINT Point);

    PRect &operator=(const RECT &rRect);
    bool operator==(const RECT &rRect);
    bool operator!=(const RECT &rRect);
};
//-------------------------------------------------------------------------------
#endif