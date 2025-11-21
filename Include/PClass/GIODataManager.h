#ifndef _GIODATAMANAGER_H_
#define _GIODATAMANAGER_H_

#include "PStringList.h"

#define GIODM_ON      1
#define GIODM_OFF     0

#define GIODM_IO_COUNT    48

#define GIODM_UNDEFINED        "-------------"
#define GIODM_IF_ERROR_CLEAR   "Error Clear"
#define GIODM_IF_RESET         "Reset"
#define GIODM_IF_NEXT_DATA     "NextData"
#define GIODM_IF_MODEL_CODE0   "ModelCode0"
#define GIODM_IF_MODEL_CODE1   "ModelCode1"
#define GIODM_IF_MODEL_CODE2   "ModelCode2"
#define GIODM_IF_MODEL_CODE3   "ModelCode3"
#define GIODM_IF_MODEL_CODE4   "ModelCode4"
#define GIODM_IF_MODEL_CODE5   "ModelCode5"
#define GIODM_IF_MODEL_CODE6   "ModelCode6"
#define GIODM_IF_MODEL_CODE7   "ModelCode7"
#define GIODM_IF_TRIGGER1      "Trigger1"
#define GIODM_IF_TRIGGER2      "Trigger2"
#define GIODM_IF_TRIGGER3      "Trigger3"
#define GIODM_IF_TRIGGER4      "Trigger4"
#define GIODM_IF_TRIGGER5      "Trigger5"
#define GIODM_IF_TRIGGER6      "Trigger6"
#define GIODM_IF_TRIGGER7      "Trigger7"
#define GIODM_IF_TRIGGER8      "Trigger8"
#define GIODM_IF_AUTO_TRAIN1   "AutoTrain1"
#define GIODM_IF_AUTO_TRAIN2   "AutoTrain2"
#define GIODM_IF_AUTO_TRAIN3   "AutoTrain3"
#define GIODM_IF_AUTO_TRAIN4   "AutoTrain4"
#define GIODM_IF_AUTO_TRAIN5   "AutoTrain5"
#define GIODM_IF_AUTO_TRAIN6   "AutoTrain6"
#define GIODM_IF_AUTO_TRAIN7   "AutoTrain7"
#define GIODM_IF_AUTO_TRAIN8   "AutoTrain8"
#define GIODM_IF_AUTO_MODE1    "Auto Mode1"
#define GIODM_IF_AUTO_MODE2    "Auto Mode2"
#define GIODM_IF_AUTO_MODE3    "Auto Mode3"
#define GIODM_IF_AUTO_MODE4    "Auto Mode4"
#define GIODM_IF_BALL_DATA_AGAIN "BDA"
#define GIODM_IF_LOGLF_CLEAR   "LogLF Clear"
#define GIODM_IF_RESULT_SEND        "ResultSend"


//2002/05/29 KJW
#define GIODM_IF_START            "Start"


#define GIODM_OF_SYSTEM_ON        "System On"
#define GIODM_OF_AUTO_MODE        "Auto Mode"
#define GIODM_OF_AUTO_MODE1       "Auto Mode1"
#define GIODM_OF_AUTO_MODE2       "Auto Mode2"
#define GIODM_OF_AUTO_MODE3       "Auto Mode3"
#define GIODM_OF_AUTO_MODE4       "Auto Mode4"
#define GIODM_OF_AUTO_MODE_STOP   "AM Stop"
#define GIODM_OF_READY            "Ready"
#define GIODM_OF_BUSY             "Busy"
#define GIODM_OF_TRIGGER_COMPLETE "T Complete"
#define GIODM_OF_COMPLETE         "Complete"
#define GIODM_OF_GOOD1            "Good1"
#define GIODM_OF_GOOD2            "Good2"
#define GIODM_OF_GOOD3            "Good3"
#define GIODM_OF_GOOD4            "Good4"
#define GIODM_OF_GOOD5            "Good5"
#define GIODM_OF_GOOD6            "Good6"
#define GIODM_OF_GOOD7            "Good7"
#define GIODM_OF_GOOD8            "Good8"
#define GIODM_OF_GOOD9            "Good9"
#define GIODM_OF_GOOD10           "Good10" 
#define GIODM_OF_GOOD11           "Good11" 
#define GIODM_OF_GOOD12           "Good12" 
#define GIODM_OF_GOOD13           "Good13" 
#define GIODM_OF_GOOD14           "Good14" 
#define GIODM_OF_GOOD15           "Good15"
#define GIODM_OF_ERROR1           "Error1"
#define GIODM_OF_ERROR2           "Error2"
#define GIODM_OF_ERROR3           "Error3"
#define GIODM_OF_ERROR4           "Error4"
#define GIODM_OF_ERROR5           "Error5"
#define GIODM_OF_ERROR6           "Error6"
#define GIODM_OF_ERROR7           "Error7"
#define GIODM_OF_ERROR8           "Error8"
#define GIODM_OF_ERROR9           "Error9"
#define GIODM_OF_ERROR10          "Error10" 
#define GIODM_OF_ERROR11          "Error11" 
#define GIODM_OF_ERROR12          "Error12" 
#define GIODM_OF_ERROR13          "Error13" 
#define GIODM_OF_ERROR14          "Error14" 
#define GIODM_OF_ERROR15          "Error15"
#define GIODM_OF_MACHINE_STOP     "MachineStop" 
#define GIODM_OF_BUZZER           "Buzzer"
#define GIODM_OF_LIGHT1           "Light1"
#define GIODM_OF_LIGHT2           "Light2"
#define GIODM_OF_LIGHT3           "Light3"
#define GIODM_OF_LIGHT4           "Light4"
#define GIODM_OF_LIGHT5           "Light5"
#define GIODM_OF_LAMP1            "Lamp1"
#define GIODM_OF_LAMP2            "Lamp2"
#define GIODM_OF_LAMP3            "Lamp3"
#define GIODM_OF_LAMP4            "Lamp4"
#define GIODM_OF_LAMP5            "Lamp5"
#define GIODM_OF_MODEL_CODE0      "ModelCode0"
#define GIODM_OF_MODEL_CODE1      "ModelCode1"
#define GIODM_OF_MODEL_CODE2      "ModelCode2"
#define GIODM_OF_MODEL_CODE3      "ModelCode3"
#define GIODM_OF_MODEL_CODE4      "ModelCode4"
#define GIODM_OF_MODEL_CODE5      "ModelCode5"
#define GIODM_OF_MODEL_CODE6      "ModelCode6"
#define GIODM_OF_MODEL_CODE7      "ModelCode7"
#define GIODM_OF_DATA_TYPE1       "DataType1"
#define GIODM_OF_DATA_TYPE2       "DataType2"
#define GIODM_OF_DATA_TYPE3       "DataType3"
#define GIODM_OF_DATA_TYPE4       "DataType4"
#define GIODM_OF_DATA_TYPE5       "DataType5"
#define GIODM_OF_DATA_CODE0       "DataCode0"
#define GIODM_OF_DATA_CODE1       "DataCode1"
#define GIODM_OF_DATA_CODE2       "DataCode2"
#define GIODM_OF_DATA_CODE3       "DataCode3"
#define GIODM_OF_DATA_CODE4       "DataCode4"
#define GIODM_OF_DATA_CODE5       "DataCode5"
#define GIODM_OF_DATA_CODE6       "DataCode6"
#define GIODM_OF_DATA_CODE7       "DataCode7"
#define GIODM_OF_DATA_CODE8       "DataCode8"
#define GIODM_OF_DATA_CODE9       "DataCode9"
#define GIODM_OF_DATA_CODE10      "DataCode10"
#define GIODM_OF_DATA_CODE11      "DataCode11"
#define GIODM_OF_DATA_CODE12      "DataCode12"
#define GIODM_OF_DATA_CODE13      "DataCode13"
#define GIODM_OF_DATA_CODE14      "DataCode14"
#define GIODM_OF_DATA_CODE15      "DataCode15"
#define GIODM_OF_BALL_DATA_AGAIN_READY "BDAReady"

//2002/05/29 KJW
#define GIODM_OF_PASS              "Pass"
#define GIODM_OF_FAIL              "Fail"
#define GIODM_OF_NOTCH             "Notch"
#define GIODM_OF_LEAD              "Lead"
#define GIODM_OF_MARK              "Mark"
#define GIODM_OF_DEVICE            "Device No Found"

class GIODataManager
{
  public:
    PString mLoadCardName;
    PString mDIOCardName;
    bool mbOutputInverse;
    int miInputBaseAddress;
    int miOutputBaseAddress;
    int miInputAddressStep;
    int miOutputAddressStep;

		//2002/05/30 KJW 
		int miDevNum;

    PString mISignal[GIODM_IO_COUNT];
    PString mOSignal[GIODM_IO_COUNT];

    int miaRefCount[GIODM_IO_COUNT];
    bool mbaIState[GIODM_IO_COUNT];
    bool mbaOState[GIODM_IO_COUNT];

    // Contec
    HANDLE mhDrv;

    HANDLE mhRS232C;
    bool mbRS232CConnection;
    int miLastReadTickCount;
    char mcaReadBuf[1024];    
    OVERLAPPED mORead;   

    GIODataManager();
    ~GIODataManager();

    // DIO
    void Initialize();
    BYTE InInput(int iAddress);
    BYTE OutInput(int iAddress);
    void Output(int iAddress, BYTE BOutput);
    void GetInputState();
    void GetOutputState();

    int GetInputPortState(char *cpSignal);
    int GetOutputPortState(char *cpSignal);
    void Output(char *cpSignal, int iSignal);
    void OutputPort(int iIndex, int iSignal);
    void OutputAll(char *cpSignal, int iSignal);
    void OutputAll(int iSignal);

    void GetCode(char *cpCodeSignal, int iBitCount, PString *pCode);
    void OutCode(char *cpCodeSignal, short sCode);

    // RS232C
    void StartRS232C(char *cpComport, int iXOnChar, int iXOffChar);
    void StopRS232C();
    void WriteRS232C(void *vpData, int iCount);

    void Save(char *cpFileName);
    void Load(char *cpFileName);
};


#endif