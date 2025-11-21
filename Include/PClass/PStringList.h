// -----------------------------------------------------------------------------
// 작성자        : 박지수
// 작성날짜      : 1999/1/7
// 최종수정날짜  : 1999/1/8
// 파일이름      : PStringList.h
// 파일 설명     : Weter에서 제공하는 system 기본 class의 헤더 파일
// 관련 파일     : PString.h PStringList.cpp GList.hpp
// 포함된 클래스 : PStringList
// -----------------------------------------------------------------------------

#ifndef _PSTRINGLIST_H_
#define _PSTRINGLIST_H_

#include "PList.hpp"
#include "PString.h"

// PStringList==================================================================
// PString 의 List Class이다
// PString 의 메모리를 자체적으로 보유하고 관리 삭제 시킨다.
class 
//#ifdef _MAKEDLL
  AFX_EXT_CLASS
//#endif
PStringList
{
  private:
    PContentList<PString> mStringList;

    bool IsToken(char cChar,char *cpToken);

  public:
    PStringList();
    ~PStringList();

    int GetCount(){ return mStringList.miCount; }

    void Create(int iCount);
    void ChangeSize(int iCount);
    void Add(const PString &rPStr);
    void Insert(int iIndex,const PString &rPStr);
    void Delete(int iIndex);
    void ClearAll();

    void SplitByToken(const PString &rPStr,char *Token);
    int SearchString(const PString &rPStr);

    // operator overload
    PStringList &operator=(PStringList &rPStrList);
    PStringList &operator+=(PStringList &rPStrList);
    PString &operator[](int iIndex);

    bool Save(HANDLE hFile);
    bool Load(HANDLE hFile);

    bool SaveFile(const char *cpFileName);
    bool LoadFile(const char *cpFileName);
    bool SaveText(const char *cpFileName);
    bool LoadText(const char *cpFileName);
};


#endif
