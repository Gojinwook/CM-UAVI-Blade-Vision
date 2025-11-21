// -----------------------------------------------------------------------------
// 작성자        : 박지수
// 작성날짜      : 1999/1/7
// 최종수정날짜  : 1999/1/8
// 파일이름      : PString.h
// 파일 설명     : Weter에서 제공하는 system 기본 class의 헤더 파일
// 관련 파일     : PString.cpp GList.hpp
// 포함된 클래스 : PString
// -----------------------------------------------------------------------------

#ifndef _PSTRING_H_
#define _PSTRING_H_

#include "PList.hpp"

// PString =====================================================================
// 가변형 PString Class이다.
// 무제한의 PString 길이를 자랑한다.
class 
//#ifdef _MAKEDLL
  AFX_EXT_CLASS
//#endif
PString
{
  public:
    char *mcpStr;

    PString();
    PString(const char *cpStr);
    PString(const PString &rPStr);
    PString(const char *cpStr, int iLen);
    PString(int iSrc);
    PString(unsigned int iSrc);
    PString(long lSrc);
    PString(unsigned long lSrc);
    PString(float fSrc);
    PString(double dSrc);

    ~PString();

    int Length();
    int HLength();
    int HGetIndex(int iIndex);
    void InsertSubString(int iIndex, const PString &rPStr);
    void DeleteSubString(int iIndex, int iCount);
    void DeleteSubStrings(const PString &rPStr);
    PString GetSubString(int iIndex, int iCount);    
    int Compare(const PString &rPStr, int iLength = 0);

    int ToInt();
    long ToLong();
    float ToFloat();
    double ToDouble();

    // operator overload
    bool operator==(const PString &rPStr);
    bool operator!=(const PString &rPStr);
    char operator[](int iIndex);    
    PString &operator=(const PString &rPStr);
    PString operator+(const PString &rPStr) const;
    friend PString operator+(const char *cpStr, const PString &rPStr);

    void Save(HANDLE hFile);
    void SaveText(HANDLE hFile);
    void Load(HANDLE hFile);
};

#endif
