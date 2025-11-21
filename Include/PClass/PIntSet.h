#ifndef _PINTSET_H_
#define _PINTSET_H_

class
//#ifdef _MAKEDLL
  AFX_EXT_CLASS
//#endif
PIntSet
{
  public:
    int *mpItems;
    int miCount;

    PIntSet();
    ~PIntSet();
    
    void Create(int iCount);

    void Add(int Item);
    void Insert(int iIndex, int Item);
    void Delete(int iIndex);

    int &Get(int iIndex);
    int GetMin();
    int GetMax();
    void Set(int iIndex, int Item);

    void ClearAll();    
    
    bool IsExist(int Item);
  	void Intersection(PIntSet &rIntSet, PIntSet *pIIntSet);
  	void Union(PIntSet &rIntSet, PIntSet *pIIntSet);

    int &operator[](int iIndex);
    void operator=(PIntSet &rIntSet);

    void Save(HANDLE hFile);
    void Load(HANDLE hFile);
};

#endif