#ifndef _PVARIANT_H_
#define _PVARIANT_H_

#include "PString.h"

#define PVT_INT       1
#define PVT_FLOAT     2
#define PVT_STRING    3

class 
//#ifdef _MAKEDLL
  AFX_EXT_CLASS
//#endif
PVariant
{
  private:    
    BYTE mBType;
    union
    {
      int miInt;
      float mfFloat;
      PString *mpString;
    };
    friend class PVariant;

  public:    
    PVariant();
    PVariant(const PVariant &rVariant);
    PVariant(int iInt);
    PVariant(float fFloat);
    PVariant(char *cpString);

    ~PVariant();

    BYTE GetType();

    int Int();
    float Float();
    PString &String();

    PVariant &operator=(const PVariant &rVariant);

    bool Load(HANDLE hFile);
    bool Save(HANDLE hFile);    
};

#endif