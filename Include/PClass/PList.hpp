// -----------------------------------------------------------------------------
// 작성자        : 박지수
// 작성날짜      : 1999/1/7
// 최종수정날짜  : 1999/1/8
// 파일이름      : PList.hpp
// 파일 설명     : 인덱싱이 가능한 동적 배열을 선언하고 구현 해놓은 파일
// 관련 파일     : 없음
// 포함된 클래스 : PList
// -----------------------------------------------------------------------------

#ifndef _PLIST_HPP_
#define _PLIST_HPP_

#include <windows.h>

// PList New Option
#define PLNO_POINTER      1
#define PLNO_CONTENT      2

// template class로 임의의 객체를 동적으로 인덱싱 할 수 있도록 해준다.
template<class Type>
class PList
{
  private:
    // Type형 객체를 관리하는 배열 포인터 (객체 포인터를 담아놓은 배열)
    Type **mppItems;

  public:
    int miNewOption;
    int miCount;

    PList(int iNewOption);
    ~PList();

    void Create(int iItemCount);
    void ChangeSize(int iItemCount);

    void Add(Type *pItem);
    void Insert(int iIndex, Type *pItem);
    void Delete(int iIndex);
    void DeleteMulti(int iIndex);
    void ClearAllMulti();

    void Swap(int iIndex1, int iIndex2);

    Type* Get(int iIndex);
    void Set(int iIndex, Type *pItem);

    void ClearAll();
};

// Constructor
// Description : mppItems = NULL, miCount = 0 을 설정한다.
template<class Type>
PList<Type>::PList(int iNewOption)
{
  miNewOption = iNewOption;
  mppItems = 0;
  miCount = 0;
}

// Distructor
// Description : 할당되어 있는 mppItems를 지운다.
template<class Type>
PList<Type>::~PList()
{
  if(miNewOption == PLNO_CONTENT)
  {
    for(int i = 0;i < miCount;i++)
	{
		if(mppItems[i])
			delete mppItems[i];
	}
  }

  if(mppItems) 
	  LocalFree(mppItems);
  mppItems = 0;
  miCount = 0;
}


template <class Type>
void PList<Type>::Create(int iItemCount)
{  
  ClearAll();
  miCount = iItemCount;
  mppItems = (Type**)LocalAlloc(LMEM_FIXED | LMEM_ZEROINIT, sizeof(Type*) * miCount);

  if(miNewOption == PLNO_CONTENT)
    for(int i = 0;i < miCount;i++) mppItems[i] = new Type;
}


template <class Type>
void PList<Type>::ChangeSize(int iItemCount)
{
  Type **ppTempItems;
  int CopyItems;

  if(iItemCount == miCount) return;
                                     
  ppTempItems = (Type**)LocalAlloc(LMEM_FIXED | LMEM_ZEROINIT, sizeof(Type*) * iItemCount);
  if(iItemCount > miCount) CopyItems = miCount;
  else CopyItems = iItemCount;
  CopyMemory(ppTempItems, mppItems, sizeof(Type*) * CopyItems);
  ZeroMemory(mppItems, sizeof(Type*) * CopyItems);

  if(miNewOption == PLNO_CONTENT)
  {    
    // 전의 배열의 내용중 새로운 배열로 복사되지 않은 내용은 지운다.
    for(int i = 0;i < miCount;i++) 
    {
      if(mppItems[i]) delete mppItems[i];
    }
    // 새로운 배열중 복사되지 않은 내용은 새로 생성한다.
    for(int i = 0;i < iItemCount;i++) 
    {
      if(!ppTempItems[i]) ppTempItems[i] = new Type;
    }
  }

  LocalFree(mppItems);
  mppItems = ppTempItems;
  miCount = iItemCount;
}


// PList에 객체를 추가한다.
// Description : PList에 CpItem를 추가한다. 
template<class Type>
void PList<Type>::Add(Type* pItem)
{
	//2002/02/14 KJW 주석넣었음...
  Type **ppTempItems;
  miCount++;

  ppTempItems = (Type**)LocalAlloc(LMEM_FIXED | LMEM_ZEROINIT, sizeof(Type*) * miCount);
  //원래 있던 데이터들을 임시 배열인 mppItems를 ppTempItems를 복사해둔다.
  CopyMemory(ppTempItems, mppItems, sizeof(Type*) * (miCount - 1));

  //그런데 새 데이터가 0이고 PLNO_CONTENT이면 빈 타입 하나를 메모리에 잡아 넣어둔다.
	//앞에 미리 miCount가 ++되어서 -1로 빼주었다.
  if(miNewOption == PLNO_CONTENT && pItem == 0) ppTempItems[miCount - 1] = new Type;
  else ppTempItems[miCount - 1] = pItem;

  LocalFree(mppItems);
  mppItems = ppTempItems;
}

// PList에 객체를 삽입한다.
// Description : PList에 iIndex지점에 CpItem를 추가한다.
template<class Type>
void PList<Type>::Insert(int iIndex, Type* pItem)
{
  Type **ppTempItems;

  if(iIndex < 0 || iIndex > miCount) return;

  miCount++;
  ppTempItems = (Type**)LocalAlloc(LMEM_FIXED | LMEM_ZEROINIT, sizeof(Type*) * miCount);
  CopyMemory(ppTempItems, mppItems, sizeof(Type*) * iIndex);

  if(miNewOption == PLNO_CONTENT && pItem == 0) ppTempItems[iIndex] = new Type;
  else ppTempItems[iIndex] = pItem;

  CopyMemory(ppTempItems + iIndex + 1, mppItems + iIndex, sizeof(Type*) * (miCount - iIndex - 1));

  LocalFree(mppItems);
  mppItems = ppTempItems;
}

// PList에서 객체를 지운다.
// Description : PList에서 iIndex지점의 객체 포인터를 PList에서 재거한다.
template<class Type>
void PList<Type>::Delete(int iIndex)
{
  Type **ppTempItems;

  if(iIndex < 0 || iIndex > miCount-1) return;
  
  if(miNewOption == PLNO_CONTENT) delete mppItems[iIndex]; 

  miCount--;
  ppTempItems = (Type**)LocalAlloc(LMEM_FIXED | LMEM_ZEROINIT, sizeof(Type*) * miCount);
  CopyMemory(ppTempItems, mppItems, sizeof(Type*) * iIndex);
  CopyMemory(ppTempItems + iIndex, mppItems + iIndex + 1, sizeof(Type*) * (miCount - iIndex));

  LocalFree(mppItems);
  mppItems = ppTempItems;
}

template<class Type>
void PList<Type>::DeleteMulti(int iIndex)
{
  if(iIndex < 0 || iIndex > miCount-1) return;
  
  if(miNewOption == PLNO_CONTENT) delete mppItems[iIndex];
  mppItems[iIndex] = 0;
}

template<class Type>
void PList<Type>::ClearAllMulti()
{
  int i, j;
  Type **ppTempItems;
  int iCount = 0;

  for(i = 0; i < miCount; i++)
  {
    if(mppItems[i] != 0) iCount++;
  }
  ppTempItems = (Type**)LocalAlloc(LMEM_FIXED | LMEM_ZEROINIT, sizeof(Type*) * iCount);

  j = 0;
  for(i = 0; i < miCount; i++)
  {
    if(mppItems[i] != 0)
    {
      ppTempItems[j] = mppItems[i];
      j++;
    }
  }

  miCount = iCount;
  LocalFree(mppItems);
  mppItems = ppTempItems; 
}

template<class Type>
void PList<Type>::Swap(int iIndex1, int iIndex2)
{
  Type *pTemp;

  if(iIndex1 < 0 || iIndex1 > miCount-1) return;
  if(iIndex2 < 0 || iIndex2 > miCount-1) return;

  pTemp = mppItems[iIndex1];
  mppItems[iIndex1] = mppItems[iIndex2];
  mppItems[iIndex2] = pTemp;
}


// PList에서 객체를 얻는다.
// Description : PList에서 iIndex지점의 객체 포인터를 돌려준다.
// Return : iIndex지점의 객체 포인터, iIndex의 범위가 벗어나면 0 을 Return
template<class Type>
Type *PList<Type>::Get(int iIndex)
{
  if(iIndex < 0 || iIndex > miCount-1) return 0;
  return mppItems[iIndex];
}


template <class Type>
void PList<Type>::Set(int iIndex, Type *pItem)
{
  if(iIndex < 0 || iIndex > miCount-1) return;  
  if(miNewOption == PLNO_CONTENT) if(mppItems[iIndex]) delete mppItems[iIndex];
  if(miNewOption == PLNO_CONTENT && pItem == 0) mppItems[iIndex] = new Type;
  else mppItems[iIndex] = pItem;
}


//2002/02/24 KJW
// PList 전체를 지운다.
// Description : PList객체의 mppItems의 메모리를 지우고 miCount = 0을 설정한다.
template <class Type>
void PList<Type>::ClearAll()
{
  if(miNewOption == PLNO_CONTENT)
  {
    for(int i = 0;i < miCount;i++) delete mppItems[i];
  }
  if(mppItems) LocalFree(mppItems);  
  mppItems = 0;
  miCount = 0;
}

// Power Content Class ----------------------------------------------------
template<class Type>
class PContentList : public PList<Type>
{
  public:
    PContentList();    
};

template<class Type>
PContentList<Type>::PContentList() : PList<Type>(PLNO_CONTENT)
{
  miNewOption = PLNO_CONTENT;
} 

// Power Pointer Class ----------------------------------------------------
template<class Type>
class PPointerList : public PList<Type>
{
  public:
    PPointerList();    
};

template<class Type>
PPointerList<Type>::PPointerList() : PList<Type>(PLNO_POINTER)
{
  miNewOption = PLNO_POINTER;
} 

#endif


