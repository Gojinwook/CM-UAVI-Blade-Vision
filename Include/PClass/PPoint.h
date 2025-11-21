//------------------------------------------------------------------------------
// 작성자           : 조규호
// 작성 날짜        : 1999/02/09
// 최종 수정 날짜   : 1999/02/10
// 파일 이름        : PPoint.h
// 설명             : PPoint 클래스 헤더 파일
// 관련 파일        : PPoint.Cpp
// 포함된 클래스    : PPoint
//------------------------------------------------------------------------------
#ifndef _PPOINT_H_
#define _PPOINT_H_

#include <windows.h>
//------------------------------------------------------------------------------
// 좌표값을 가진다. 좌표값은 int형 miX와 miY로 이루어진다.
// 현재 5가지의 오퍼레이터 오버로딩을 지원한다.
class 
//#ifdef _MAKEDLL
  AFX_EXT_CLASS
//#endif
PPoint : public tagPOINT
{
  public: 
    PPoint();
    PPoint(int iX,int iY);
    PPoint(POINT Point);
    ~PPoint();

    void Set(int iX, int iY);

    //Operator Overloading
    PPoint operator+(POINT Point);
    PPoint operator-(POINT Point);
    PPoint operator+=(POINT Point);
    PPoint operator-=(POINT Point);

    bool operator==(POINT Point);   
};
//------------------------------------------------------------------------------
#endif
