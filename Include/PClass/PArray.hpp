// -----------------------------------------------------------------------------
// 작성자        : 박지수
// 작성날짜      : 1999/1/7
// 최종수정날짜  : 1999/1/8
// 파일이름      : PArray.hpp
// 파일 설명     : 인덱싱이 가능한 동적 배열을 선언하고 구현 해놓은 파일
// 관련 파일     : 없음
// 포함된 클래스 : PArray
// -----------------------------------------------------------------------------

#ifndef _PARRAY_HPP_
#define _PARRAY_HPP_

// template class로 임의의 객체를 동적으로 인덱싱 할 수 있도록 해준다.
template<class Type>
class PArray
{
  protected:
    friend class PArray;

  public:
    Type *mpItems;
    int miCount;

    PArray();
    ~PArray();

    void Create(int iCount);
    void ChangeSize(int iCount);

    void Add(Type Item);
    void Insert(int iIndex, Type Item);
    void Delete(int iIndex);

    Type &Get(int iIndex);    
    void Set(int iIndex, Type Item);

    void ClearAll();
    void SetMemory(BYTE BValue);    

    Type &operator[](int iIndex);
    void operator=(PArray<Type> &rArray);

    void Save(HANDLE hFile);
    void Load(HANDLE hFile);
};

// Constructor
// Description : mpItems = NULL, miCount = 0 을 설정한다.
template<class Type>
PArray<Type>::PArray()
{
  mpItems = 0;
  miCount = 0;
}

// Distructor
// Description : 할당되어 있는 mpItems를 지운다.
template<class Type>
PArray<Type>::~PArray()
{
  if(mpItems) LocalFree(mpItems);
  mpItems = 0;
  miCount = 0;
}


template <class Type>
void PArray<Type>::Create(int iCount)
{  
  if(mpItems) LocalFree(mpItems);
  miCount = iCount;
  mpItems = (Type*)LocalAlloc(LMEM_FIXED | LMEM_ZEROINIT, sizeof(Type) * miCount);
}


template <class Type>
void PArray<Type>::ChangeSize(int iCount)
{
  Type *pTempItems;
  int CopyItems;

  if(iCount == miCount) return;
                                     
  pTempItems = (Type*)LocalAlloc(LMEM_FIXED | LMEM_ZEROINIT, sizeof(Type) * iItemCount);
  if(iItemCount > miCount) CopyItems = miCount;
  else CopyItems = iItemCount;
  CopyMemory(pTempItems, mpItems, sizeof(Type) * CopyItems);
  ZeroMemory(mpItems, sizeof(Type) * CopyItems);

  LocalFree(mpItems);
  mpItems = pTempItems;
  miCount = iItemCount;
}


// PArray에 객체를 추가한다.
// Description : PArray에 Item를 추가한다. 
template<class Type>
void PArray<Type>::Add(Type Item)
{
	//2002/02/14 KJW PList와 다른 것은 PArray는 주소값이 아니라 직접 값을 받는다.
  Type *pTempItems;
  miCount++;

  pTempItems = (Type*)LocalAlloc(LMEM_FIXED | LMEM_ZEROINIT, sizeof(Type) * miCount);
  CopyMemory(pTempItems, mpItems, sizeof(Type) * (miCount - 1));

  pTempItems[miCount - 1] = Item;

  LocalFree(mpItems);
  mpItems = pTempItems;
}

// PArray에 객체를 삽입한다.
// Description : PArray에 iIndex지점에 CpItem를 추가한다.
template<class Type>
void PArray<Type>::Insert(int iIndex, Type Item)
{
  Type *pTempItems;

  if(iIndex < 0 || iIndex > miCount) return;

  miCount++;
  pTempItems = (Type*)LocalAlloc(LMEM_FIXED | LMEM_ZEROINIT, sizeof(Type) * miCount);
  CopyMemory(pTempItems, mpItems, sizeof(Type) * iIndex);

  pTempItems[iIndex] = Item;

  CopyMemory(pTempItems + iIndex + 1, mpItems + iIndex, sizeof(Type) * (miCount - iIndex - 1));

  LocalFree(mpItems);
  mpItems = pTempItems;
}

// PArray에서 객체를 지운다.
// Description : PArray에서 iIndex지점의 객체 포인터를 PArray에서 재거한다.
template<class Type>
void PArray<Type>::Delete(int iIndex)
{
  Type *pTempItems;

  if(iIndex < 0 || iIndex > miCount-1) return;
  
  miCount--;
  pTempItems = (Type*)LocalAlloc(LMEM_FIXED | LMEM_ZEROINIT, sizeof(Type) * miCount);
  CopyMemory(pTempItems, mpItems, sizeof(Type) * iIndex);
  CopyMemory(pTempItems + iIndex, mpItems + iIndex + 1, sizeof(Type) * (miCount - iIndex));

  LocalFree(mpItems);
  mpItems = pTempItems;
}


// PArray에서 객체를 얻는다.
// Description : PArray에서 iIndex지점의 객체 포인터를 돌려준다.
// Return : iIndex지점의 객체 포인터, iIndex의 법위가 벗어나면 0 을 Return
template<class Type>
Type &PArray<Type>::Get(int iIndex)
{
  return mpItems[iIndex];
}


template <class Type>
void PArray<Type>::Set(int iIndex, Type Item)
{
  if(iIndex < 0 || iIndex > miCount-1) return;  
  mpItems[iIndex] = Item;
}

// PArray 전체를 지운다.
// Description : PArray객체의 mpItems의 메모리를 지우고 miCount = 0을 설정한다.
template <class Type>
void PArray<Type>::ClearAll()
{
  if(mpItems) LocalFree(mpItems);
  mpItems = 0;
  miCount = 0;
}

template <class Type>
void PArray<Type>::SetMemory(BYTE BValue)
{
  ::FillMemory(mpItems, sizeof(Type) * miCount, BValue);
}

template<class Type>
Type &PArray<Type>::operator[](int iIndex)
{
  return mpItems[iIndex];
}

template<class Type>
void PArray<Type>::operator=(PArray<Type> &rArray)
{
  Create(rArray.miCount);
  CopyMemory(mpItems, rArray.mpItems, sizeof(Type) * miCount);
}

template<class Type>
void PArray<Type>::Save(HANDLE hFile)
{
  DWORD dwBytesWritten;

  WriteFile(hFile, &miCount, 4, &dwBytesWritten, NULL);
  for(int i = 0; i < miCount; i++)
  {
    WriteFile(hFile, mpItems + i, sizeof(Type), &dwBytesWritten, NULL);
  }
}

template<class Type>
void PArray<Type>::Load(HANDLE hFile)
{
  DWORD dwBytesRead;

  ReadFile(hFile, &miCount, 4, &dwBytesRead, NULL);
  Create(miCount);
  for(int i = 0; i < miCount; i++)
  {
    ReadFile(hFile, mpItems + i, sizeof(Type), &dwBytesRead, NULL);
  }
}

#endif


