#ifndef _PVARIANTLIST_H_
#define _PVARIANTLIST_H_

#include "PVariant.h"
#include "PList.hpp"

class
//#ifdef _MAKEDLL
  AFX_EXT_CLASS
//#endif
PVariantList
{
  protected:
    PContentList<PVariant> mVariantList;

  public:
    PVariantList();
    ~PVariantList();
    
    int GetCount();
    void Add(const PVariant &rVariant);
    void Insert(int iIndex, const PVariant &rVariant);
    void Delete(int iIndex);
    void ClearAll();
    PVariant &GetVariant(int iIndex);

    PVariant &operator[](int iIndex);
};

#endif   
