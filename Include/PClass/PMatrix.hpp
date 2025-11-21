#ifndef _PMATRIX_HPP_
#define _PMATRIX_HPP_

//#include <windows.h>

#define PMNO_POINTER     1
#define PMNO_CONTENT     2


template <class Type>
class PMatrix
{
  private:
    Type **mppItems;

  public:    
    int miNewOption;
    int miRowCount;  
    int miColumnCount;    

    PMatrix(int iNewOption);
    ~PMatrix();

    void Create(int iRowCount, int iColumnCount);
    void ChangeSize(int iRowCount, int iColumnCount);

    void AddRow();
    void InsertRow(int iRow);
    void DeleteRow(int iRow);

    void AddColumn();
    void InsertColumn(int iColumn);
    void DeleteColumn(int iColumn);
    
    Type *Get(int iRow, int iColumn); 
    void Set(int iRow, int iColumn, Type *pItem);

    void ClearAll();
};


template <class Type>
PMatrix<Type>::PMatrix(int iNewOption)
{
  miNewOption = iNewOption;
  mppItems = 0;
  miRowCount = 0;
  miColumnCount = 0;
}

template <class Type>
PMatrix<Type>::~PMatrix()
{
  if(miNewOption == PMNO_CONTENT)
  {
    for(int i = 0;i < miRowCount * miColumnCount;i++)
     if(mppItems[i]) delete mppItems[i];
  }  
  if(mppItems) LocalFree(mppItems);
  mppItems = 0;
  miRowCount = 0;
  miColumnCount = 0;
}

template <class Type>
void PMatrix<Type>::Create(int iRowCount, int iColumnCount)
{
  ClearAll();  

  if(miRowCount == iRowCount && miColumnCount == iColumnCount) return;
   
  miRowCount = iRowCount;
  miColumnCount = iColumnCount;
  mppItems = (Type**)LocalAlloc(LMEM_FIXED | LMEM_ZEROINIT, sizeof(Type*) * (miRowCount * miColumnCount));

  if(miNewOption == PMNO_CONTENT)
    for(int i = 0;i < miRowCount * miColumnCount;i++) mppItems[i] = new Type;
}


template <class Type>
void PMatrix<Type>::ChangeSize(int iRowCount, int iColumnCount)
{
  Type **ppTempItems;
  int CopyRow, CopyColumn;

  if(miRowCount == iRowCount && miColumnCount == iColumnCount) return;

  ppTempItems = (Type**)LocalAlloc(LMEM_FIXED | LMEM_ZEROINIT, sizeof(Type*) * (iRowCount * iColumnCount));
  if(iRowCount > miRowCount) CopyRow = miRowCount;
  else CopyRow = iRowCount;
  if(iColumnCount > miColumnCount) CopyColumn = miColumnCount;
  else CopyColumn = iColumnCount;
  for(int i = 0;i < CopyRow;i++)
  {
    CopyMemory(ppTempItems + i * iColumnCount, mppItems + i * miColumnCount, sizeof(Type*) * CopyColumn);
    ZeroMemory(mppItems + i * miColumnCount, sizeof(Type*) * CopyColumn);
  }

  if(miNewOption == PMNO_CONTENT)
  {
    // 전의 배열의 내용중 새로운 배열로 복사되지 않은 내용은 지운다.
    for(int i = 0;i < miRowCount * miColumnCount;i++)      
    {
      if(mppItems[i]) delete mppItems[i];
    }
    // 새로운 배열중 복사되지 않은 내용은 새로 생성한다.
    for(i = 0;i < iRowCount * iColumnCount;i++)
    {
      if(!ppTempItems[i]) ppTempItems[i] = new Type;
    }
  }   

  LocalFree(mppItems);
  mppItems = ppTempItems;
  miRowCount = iRowCount;
  miColumnCount = iColumnCount;
}

template <class Type>
void PMatrix<Type>::AddRow()
{
  Type **ppTempItems;

  if(miColumnCount == 0) miColumnCount = 1; // Column나 Row이 0인 Matrix는 있을 수 없다.
  miRowCount++;
  ppTempItems = (Type**)LocalAlloc(LMEM_FIXED | LMEM_ZEROINIT, sizeof(Type*) * (miRowCount * miColumnCount));
  CopyMemory(ppTempItems, mppItems, sizeof(Type*) * ((miRowCount-1) * miColumnCount));

  if(miNewOption == PMNO_CONTENT)
  {
    for(int i = 0;i < miColumnCount;i++)
      if(!ppTempItems[(miRowCount-1) * miColumnCount + i]) ppTempItems[(miRowCount-1) * miColumnCount + i] = new Type; 
  }

  LocalFree(mppItems);
  mppItems = ppTempItems;
}


template <class Type>
void PMatrix<Type>::InsertRow(int iRow)
{
  Type **ppTempItems;

  if(iRow < 0 || iRow > miRowCount) return;
  if(miColumnCount == 0) miColumnCount = 1; // Column나 Row이 0인 Matrix는 있을 수 없다.
  miRowCount++;
  ppTempItems = (Type**)LocalAlloc(LMEM_FIXED | LMEM_ZEROINIT, sizeof(Type*) * (miRowCount * miColumnCount));
  CopyMemory(ppTempItems, mppItems, sizeof(Type*) * ((iRow) * miColumnCount));
  CopyMemory(ppTempItems + (iRow + 1) * miColumnCount, mppItems + iRow * miColumnCount, 
             sizeof(Type*) * ((miRowCount - iRow - 1) * miColumnCount));

  if(miNewOption == PMNO_CONTENT)
  {
    for(int i = 0;i < miColumnCount;i++)
      if(!ppTempItems[iRow * miColumnCount + i]) ppTempItems[iRow * miColumnCount + i] = new Type; 
  }

  LocalFree(mppItems);
  mppItems = ppTempItems;
}

template <class Type>
void PMatrix<Type>::DeleteRow(int iRow)
{
  Type **ppTempItems;

  if(iRow < 0 || iRow >= miRowCount) return;
  if(miRowCount == 0 || miColumnCount == 0) return;

  if(miNewOption == PMNO_CONTENT)
  {
    for(int i = 0;i < miColumnCount;i++)
      if(mppItems[iRow * miColumnCount + i]) delete mppItems[iRow * miColumnCount + i]; 
  }

  miRowCount--;
  ppTempItems = (Type**)LocalAlloc(LMEM_FIXED | LMEM_ZEROINIT, sizeof(Type*) * (miRowCount * miColumnCount));
  CopyMemory(ppTempItems, mppItems, sizeof(Type*) * ((iRow) * miColumnCount));
  CopyMemory(ppTempItems + iRow * miColumnCount, mppItems + (iRow+1) * miColumnCount, 
             sizeof(Type*) * ((miRowCount - iRow) * miColumnCount));

  LocalFree(mppItems);
  mppItems = ppTempItems;
}

template <class Type>
void PMatrix<Type>::AddColumn()
{
  Type **ppTempItems;

  if(miRowCount == 0) miRowCount = 1; // Column나 Row이 0인 Matrix는 있을 수 없다.
  miColumnCount++;
  ppTempItems = (Type**)LocalAlloc(LMEM_FIXED | LMEM_ZEROINIT, sizeof(Type*) * (miRowCount * miColumnCount));
  for(int i = 0;i < miRowCount;i++)
  {
    CopyMemory(ppTempItems + i * miColumnCount, mppItems + i * (miColumnCount-1), sizeof(Type*) * (miColumnCount-1));
  }

  if(miNewOption == PMNO_CONTENT)
  {
    for(int i = 0;i < miRowCount;i++)
      if(!ppTempItems[i * miColumnCount + (miColumnCount-1)]) ppTempItems[i * miColumnCount + (miColumnCount-1)] = new Type; 
  }

  LocalFree(mppItems);
  mppItems = ppTempItems;
}


template <class Type>
void PMatrix<Type>::InsertColumn(int iColumn)
{
  Type **ppTempItems;

  if(iColumn < 0 || iColumn > miColumnCount) return;
  if(miRowCount == 0) miRowCount = 1; // Column나 Row이 0인 Matrix는 있을 수 없다.
  miColumnCount++;
  ppTempItems = (Type**)LocalAlloc(LMEM_FIXED | LMEM_ZEROINIT, sizeof(Type*) * (miRowCount * miColumnCount));
  for(int i = 0;i < miRowCount;i++)
  {
    CopyMemory(ppTempItems + i * miColumnCount, mppItems + i * (miColumnCount-1), sizeof(Type*) * (iColumn));
    CopyMemory(ppTempItems + i * miColumnCount + iColumn + 1, mppItems + i * (miColumnCount-1) + iColumn, sizeof(Type*) * (miColumnCount - iColumn -1));
  }

  if(miNewOption == PMNO_CONTENT)
  {
    for(int i = 0;i < miRowCount;i++)
      if(!ppTempItems[i * miColumnCount + iColumn]) ppTempItems[i * miColumnCount + iColumn] = new Type; 
  }

  LocalFree(mppItems);
  mppItems = ppTempItems;
}


template <class Type>
void PMatrix<Type>::DeleteColumn(int iColumn)
{
  Type **ppTempItems;

  if(iColumn < 0 || iColumn >= miColumnCount) return;
  if(miRowCount == 0) miRowCount = 1; // Column나 Row이 0인 Matrix는 있을 수 없다.

  if(miNewOption == PMNO_CONTENT)
  {
    for(int i = 0;i < miRowCount;i++)
      if(mppItems[i * miColumnCount + iColumn]) delete mppItems[i * miColumnCount + iColumn]; 
  }

  miColumnCount--;
  ppTempItems = (Type**)LocalAlloc(LMEM_FIXED | LMEM_ZEROINIT, sizeof(Type*) * (miRowCount * miColumnCount));
  for(int i = 0;i < miRowCount;i++)
  {
    CopyMemory(ppTempItems + i * miColumnCount, mppItems + i * (miColumnCount+1), sizeof(Type*) * (iColumn));
    CopyMemory(ppTempItems + i * miColumnCount + iColumn, mppItems + i * (miColumnCount+1) + iColumn + 1, sizeof(Type*) * (miColumnCount - iColumn));
  }

  LocalFree(mppItems);
  mppItems = ppTempItems;
}

template <class Type>
Type *PMatrix<Type>::Get(int iRow, int iColumn)
{
  if(iRow < 0 || iRow >= miRowCount || iColumn < 0 || iColumn >= miColumnCount) return 0;
  return mppItems[iRow * miColumnCount + iColumn];
}

template <class Type>
void PMatrix<Type>::Set(int iRow, int iColumn, Type *pItem)
{
  if(iRow < 0 || iRow >= miRowCount || iColumn < 0 || iColumn >= miColumnCount) return;

  if(miNewOption == PMNO_CONTENT) 
    if(mppItems[iRow * miColumnCount + iColumn]) delete mppItems[iRow * miColumnCount + iColumn];

  if(miNewOption == PMNO_CONTENT && pItem == 0) mppItems[iRow * miColumnCount + iColumn] = new Type;
  else mppItems[iRow * miColumnCount + iColumn] = pItem;
}

template <class Type>
void PMatrix<Type>::ClearAll()
{
  if(miNewOption == PMNO_CONTENT)
  {
    for(int i = 0;i < miRowCount * miColumnCount;i++)
     if(mppItems[i]) delete mppItems[i];
  }  

  if(mppItems) LocalFree(mppItems);
  mppItems = 0;
  miRowCount = 0;
  miColumnCount = 0;
}


// Power Content Class ----------------------------------------------------
template<class Type>
class PContentMatrix : public PMatrix<Type>
{
  public:
    PContentMatrix();    
};

template<class Type>
PContentMatrix<Type>::PContentMatrix() : PList<Type>(PMNO_CONTENT)
{
  miNewOption = PMNO_CONTENT;
} 

// Power Pointer Class ----------------------------------------------------
template<class Type>
class PPointerMatrix : public PMatrix<Type>
{
  public:
    PPointerMatrix();    
};

template<class Type>
PPointerMatrix<Type>::PPointerMatrix() : PMatrix<Type>(PMNO_POINTER)
{
  miNewOption = PMNO_POINTER;
} 

#endif