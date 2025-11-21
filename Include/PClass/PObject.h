#ifndef _POBJECT_H_
#define _POBJECT_H_

#include "PString.h"
#include "PList.hpp"
class PObject;

// Property Mask Macro
#define GET_PROPERTY(type, variable) type Get##variable(){ return variable; } 
#define SET_PROPERTY(variable, type) void Set##variable(type arg){ if(_mbPM##variable) variable = arg; } 

#define PROPERTY(type, variable) \
  protected: \
    type variable; \
  public: \
    bool _mbPM##variable; \
    GET_PROPERTY(type, variable) \
    SET_PROPERTY(variable, type) 

#define GET_PROPERTYMASK(variable) _mbPM##variable
#define SET_PROPERTYMASK(variable, boolean) _mbPM##variable = boolean


class 
//#ifdef _MAKEDLL
  AFX_EXT_CLASS
//#endif
PDynClass
{
  public:
    PString mName;
    PObject* (*mpFCreateObject)();  
    PDynClass *mpPrev;

    PDynClass(char *cpName, PObject* (*pFCreateObject)());

    PObject *CreateObject();
};

// DynClass Macro
#define DECLARE_DYNCLASS(class_name) \
  public: \
    static PDynClass _DynClass##class_name; \
    static PObject *CreateObject(); \
    virtual PDynClass *GetDynClass();

#define IMPLEMENT_DYNCLASS(class_name) \
  PObject *class_name::CreateObject() \
  { return (PObject*)new class_name; } \
  PDynClass class_name::_DynClass##class_name(#class_name, class_name::CreateObject); \
  PDynClass *class_name::GetDynClass() \
  { return DYNCLASS(class_name); } \

// pre version : #define DYNCLASS(class_name) (PDynClass*)(&class_name::_DynClass##class_name)

// GBFGetDynClass(char *cpClassName)을 호출해서 해당 DynClss를 넘겨준다.
// DLL 함수이므로 EXE에서도 DLL Class를 사용할 수 있다.
#define DYNCLASS(class_name) PGetDynClass(#class_name) 

class
//#ifdef _MAKEDLL
  AFX_EXT_CLASS
//#endif
PObject
{
  DECLARE_DYNCLASS(PObject)

  public:
    PObject();
    virtual ~PObject();
};

extern "C" _declspec(dllimport)
PDynClass *PGetDynClass(char *cpClassName);

#endif