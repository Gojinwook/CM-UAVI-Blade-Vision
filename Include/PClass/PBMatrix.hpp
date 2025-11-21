#ifndef _GBMATRIX_HPP_
#define _GBMATRIX_HPP_

template <class Type>
class GBMatrix
{
  private:
    Type *mpItems;
    friend class GBMatrix;

  public:    
    int miRowCount;  
    int miColumnCount;    

    GBMatrix();
    GBMatrix(int iRowCount, int iColumnCount);
    GBMatrix(const GBMatrix<Type> &rBMatrix);
    ~GBMatrix();

    void Create(int iRowCount, int iColumnCount);
    void ChangeSize(int iRowCount, int iColumnCount);

    void AddRow();
    void InsertRow(int iRow);
    void DeleteRow(int iRow);

    void AddColumn();
    void InsertColumn(int iColumn);
    void DeleteColumn(int iColumn);
    
    Type Get(int iRow, int iColumn); 
    void Set(int iRow, int iColumn, Type Item);

    void ClearAll();

    GBMatrix<Type> GetInverse();   

    GBMatrix<Type> &operator=(const GBMatrix<Type> &rBMatrix);    
    GBMatrix<Type> operator*(const GBMatrix<Type> &rBMatrix);
};


template <class Type>
GBMatrix<Type>::GBMatrix()
{
  mpItems = 0;
  miRowCount = 0;
  miColumnCount = 0;
}

template <class Type>
GBMatrix<Type>::GBMatrix(int iRowCount, int iColumnCount)
{
  mpItems = 0;
  Create(iRowCount, iColumnCount);
}

template <class Type>
GBMatrix<Type>::GBMatrix(const GBMatrix<Type> &rBMatrix)
{
  Create(rBMatrix.miRowCount, rBMatrix.miColumnCount);
  CopyMemory(mpItems, rBMatrix.mpItems, sizeof(Type) * miRowCount * miColumnCount);
}

template <class Type>
GBMatrix<Type>::~GBMatrix()
{
  if(mpItems) LocalFree(mpItems);
  mpItems = 0;
  miRowCount = 0;
  miColumnCount = 0;
}

template <class Type>
void GBMatrix<Type>::Create(int iRowCount, int iColumnCount)
{
  ClearAll();     
  miRowCount = iRowCount;
  miColumnCount = iColumnCount;
  mpItems = (Type*)LocalAlloc(LMEM_FIXED | LMEM_ZEROINIT, sizeof(Type) * (miRowCount * miColumnCount));
}


template <class Type>
void GBMatrix<Type>::ChangeSize(int iRowCount, int iColumnCount)
{
  Type *pTempItems;
  int CopyRow, CopyColumn;

  if(miRowCount == iRowCount && miColumnCount == iColumnCount) return;

  pTempItems = (Type*)LocalAlloc(LMEM_FIXED | LMEM_ZEROINIT, sizeof(Type) * (iRowCount * iColumnCount));
  if(iRowCount > miRowCount) CopyRow = miRowCount;
  else CopyRow = iRowCount;
  if(iColumnCount > miColumnCount) CopyColumn = miColumnCount;
  else CopyColumn = iColumnCount;
  for(int i = 0;i < CopyRow;i++)
  {
    CopyMemory(pTempItems + i * iColumnCount, mpItems + i * miColumnCount, sizeof(Type) * CopyColumn);
    ZeroMemory(mpItems + i * miColumnCount, sizeof(Type) * CopyColumn);
  }

  LocalFree(mpItems);
  mpItems = pTempItems;
  miRowCount = iRowCount;
  miColumnCount = iColumnCount;
}

template <class Type>
void GBMatrix<Type>::AddRow()
{
  Type *pTempItems;

  if(miColumnCount == 0) miColumnCount = 1; // Column나 Row이 0인 Matrix는 있을 수 없다.
  miRowCount++;
  pTempItems = (Type*)LocalAlloc(LMEM_FIXED | LMEM_ZEROINIT, sizeof(Type) * (miRowCount * miColumnCount));
  CopyMemory(pTempItems, mpItems, sizeof(Type) * ((miRowCount-1) * miColumnCount));

  LocalFree(mpItems);
  mpItems = pTempItems;
}


template <class Type>
void GBMatrix<Type>::InsertRow(int iRow)
{
  Type *pTempItems;

  if(iRow < 0 || iRow > miRowCount) return;
  if(miColumnCount == 0) miColumnCount = 1; // Column나 Row이 0인 Matrix는 있을 수 없다.
  miRowCount++;
  pTempItems = (Type*)LocalAlloc(LMEM_FIXED | LMEM_ZEROINIT, sizeof(Type) * (miRowCount * miColumnCount));
  CopyMemory(pTempItems, mpItems, sizeof(Type) * ((iRow) * miColumnCount));
  CopyMemory(pTempItems + (iRow + 1) * miColumnCount, mpItems + iRow * miColumnCount, 
             sizeof(Type) * ((miRowCount - iRow - 1) * miColumnCount));

  LocalFree(mpItems);
  mpItems = pTempItems;
}

template <class Type>
void GBMatrix<Type>::DeleteRow(int iRow)
{
  Type *pTempItems;

  if(iRow < 0 || iRow >= miRowCount) return;
  if(miRowCount == 0 || miColumnCount == 0) return;

  miRowCount--;
  pTempItems = (Type*)LocalAlloc(LMEM_FIXED | LMEM_ZEROINIT, sizeof(Type) * (miRowCount * miColumnCount));
  CopyMemory(pTempItems, mpItems, sizeof(Type) * ((iRow) * miColumnCount));
  CopyMemory(pTempItems + iRow * miColumnCount, mpItems + (iRow+1) * miColumnCount, 
             sizeof(Type) * ((miRowCount - iRow) * miColumnCount));

  LocalFree(mpItems);
  mpItems = pTempItems;
}

template <class Type>
void GBMatrix<Type>::AddColumn()
{
  Type *pTempItems;

  if(miRowCount == 0) miRowCount = 1; // Column나 Row이 0인 Matrix는 있을 수 없다.
  miColumnCount++;
  pTempItems = (Type*)LocalAlloc(LMEM_FIXED | LMEM_ZEROINIT, sizeof(Type) * (miRowCount * miColumnCount));
  for(int i = 0;i < miRowCount;i++)
  {
    CopyMemory(pTempItems + i * miColumnCount, mpItems + i * (miColumnCount-1), sizeof(Type) * (miColumnCount-1));
  }

  LocalFree(mpItems);
  mpItems = pTempItems;
}


template <class Type>
void GBMatrix<Type>::InsertColumn(int iColumn)
{
  Type *pTempItems;

  if(iColumn < 0 || iColumn > miColumnCount) return;
  if(miRowCount == 0) miRowCount = 1; // Column나 Row이 0인 Matrix는 있을 수 없다.
  miColumnCount++;
  pTempItems = (Type*)LocalAlloc(LMEM_FIXED | LMEM_ZEROINIT, sizeof(Type) * (miRowCount * miColumnCount));
  for(int i = 0;i < miRowCount;i++)
  {
    CopyMemory(pTempItems + i * miColumnCount, mpItems + i * (miColumnCount-1), sizeof(Type) * (iColumn));
    CopyMemory(pTempItems + i * miColumnCount + iColumn + 1, mpItems + i * (miColumnCount-1) + iColumn, sizeof(Type) * (miColumnCount - iColumn -1));
  }

  LocalFree(mpItems);
  mpItems = pTempItems;
}


template <class Type>
void GBMatrix<Type>::DeleteColumn(int iColumn)
{
  Type *pTempItems;

  if(iColumn < 0 || iColumn >= miColumnCount) return;
  if(miRowCount == 0) miRowCount = 1; // Column나 Row이 0인 Matrix는 있을 수 없다.

  miColumnCount--;
  pTempItems = (Type*)LocalAlloc(LMEM_FIXED | LMEM_ZEROINIT, sizeof(Type) * (miRowCount * miColumnCount));
  for(int i = 0;i < miRowCount;i++)
  {
    CopyMemory(pTempItems + i * miColumnCount, mpItems + i * (miColumnCount+1), sizeof(Type) * (iColumn));
    CopyMemory(pTempItems + i * miColumnCount + iColumn, mpItems + i * (miColumnCount+1) + iColumn + 1, sizeof(Type) * (miColumnCount - iColumn));
  }

  LocalFree(mpItems);
  mpItems = pTempItems;
}

template <class Type>
Type GBMatrix<Type>::Get(int iRow, int iColumn)
{
  if(iRow < 0 || iRow >= miRowCount || iColumn < 0 || iColumn >= miColumnCount) return 0;
  return mpItems[iRow * miColumnCount + iColumn];
}

template <class Type>
void GBMatrix<Type>::Set(int iRow, int iColumn, Type Item)
{
  if(iRow < 0 || iRow >= miRowCount || iColumn < 0 || iColumn >= miColumnCount) return;
  mpItems[iRow * miColumnCount + iColumn] = Item;
}

template <class Type>
void GBMatrix<Type>::ClearAll()
{
  if(mpItems) LocalFree(mpItems);
  mpItems = 0;
  miRowCount = 0;
  miColumnCount = 0;
}

template <class Type>
GBMatrix<Type> GBMatrix<Type>::GetInverse()
{
  double Det;
  Type Data;
  GBMatrix BMatrix;
  BMatrix = *this;
  Det = 1.0 / (double)(Get(0, 0) * Get(1, 1) - Get(0, 1) * Get(1, 0));
  Data = BMatrix.Get(0, 0);
  BMatrix.Set(0, 0, BMatrix.Get(1, 1));
  BMatrix.Set(1, 1, Data);
  BMatrix.Set(0, 1, -BMatrix.Get(0, 1));
  BMatrix.Set(1, 0, -BMatrix.Get(1, 0));
  for(int j = 0; j < miRowCount; j++)
  {
    for(int i = 0; i < miColumnCount; i++)
    {
      Data = BMatrix.Get(j, i);
      BMatrix.Set(j, i, (Type)((double)Data * Det));
    }
  }
  return BMatrix;
}

template <class Type>
GBMatrix<Type> &GBMatrix<Type>::operator=(const GBMatrix<Type> &rBMatrix)
{
  Create(rBMatrix.miRowCount, rBMatrix.miColumnCount);
  CopyMemory(mpItems, rBMatrix.mpItems, sizeof(Type) * miRowCount * miColumnCount);

  return *this;
}

template <class Type>
GBMatrix<Type> GBMatrix<Type>::operator*(const GBMatrix<Type> &rBMatrix)
{
  Type Data;
  GBMatrix BMatrix;
  BMatrix.Create(miRowCount, rBMatrix.miColumnCount);
  for(int j = 0; j < miRowCount; j++)
  {
    for(int i = 0; i < rBMatrix.miColumnCount; i++)
    {
      Data = 0;
      for(int k = 0; k < miColumnCount; k++)
      {
        Data += Get(j, k) * rBMatrix.mpItems[k * rBMatrix.miColumnCount + i];
      }
      BMatrix.Set(j, i, Data);
    }
  }
  return BMatrix;
}

#endif