#ifndef _PFUN_H_
#define _PFUN_H_

#include "PBC.h"
#include "HalconCpp.h"

void WriteData(PStringList &rStringList, char *cpDataName, void *vpData, char *cpType, int iLength);
void ReadData(PStringList &rStringList, char *cpDataName, void *vpData, char *cpType);

#define WRITE_DATA(stringlist, variable, type, length) \
  WriteData(stringlist, #variable, &variable, type, length);

#define READ_DATA(stringlist, variable, type) \
  ReadData(stringlist, #variable, &variable, type);  

bool ValidHobject(Hobject &rHObject);

#endif