#ifndef _CLSER__H_
#define _CLSER__H_

#ifndef _CLSER__EXPORT
	#define CLSER__EXPORT	__declspec(dllimport)
#endif

#ifndef _CLSER__CC
	#define CLSER__CC		__cdecl
#endif

#ifdef __cplusplus
extern "C" {
#endif

CLSER__EXPORT	int CLSER__CC
clSerialInit(unsigned long serialIndex, void** serialRefPtr); 

CLSER__EXPORT	int CLSER__CC
clSerialRead(void* SerialRef, char* buffer, unsigned long* bufferSize, unsigned long serialTimeout); 

CLSER__EXPORT	int CLSER__CC
clSerialWrite(void* SerialRef, char* buffer, unsigned long* bufferSize, unsigned long serialTimeout); 

CLSER__EXPORT	int CLSER__CC
clSerialClose(void* serialRef); 

CLSER__EXPORT	int CLSER__CC
clSetBaudRate(void* serialRef, unsigned int baudRate);

CLSER__EXPORT	int CLSER__CC
clSerialSetMode(void* serialRef, unsigned int Mode);		 

#ifdef __cplusplus
}
#endif

#endif