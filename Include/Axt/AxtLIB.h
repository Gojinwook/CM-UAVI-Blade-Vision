#ifndef __AXT_LIBRARY_H__
#define __AXT_LIBRARY_H__

#include "AxtLIBDef.h"

/*------------------------------------------------------------------------------------------------*
    AXTLIB Library - 통합라이브러리 및 베이스보드 관리
    적용제품
        BIHR - ISA Half size, 2 module
        BIFR - ISA Full size, 4 module
        BPHR - PCI Half size, 2 module
        BPFR - PCI Full size, 4 module
        BV3R - VME 3U size, 2 module
        BV6R - VME 6U size, 4 module
        BC3R - CompactPCI 3U size, 2 module
        BC6R - CompactPCI 6U size, 4 module
 *------------------------------------------------------------------------------------------------*/


#ifdef __cplusplus
extern "C" {
#endif    //__cplusplus

//========== 통합라이브러리 초기화 및 종료 =========================================================
    // 통합 라이브러리를 초기화 한다..
    BOOL    PASCAL EXPORT AxtInitialize(HANDLE hWnd, INT16 nIrqNo);
    // 통합 라이브러리가 사용 가능하지 (초기화가 되었는지)를 확인한다
    BOOL    PASCAL EXPORT AxtIsInitialized();
    // 통합 라이브러리의 사용을 종료한다.
    void    PASCAL EXPORT AxtClose();

//========== 베이스보드 오픈 및 닫기 ===============================================================
    // 지정한 버스(ISA, PCI, VME, CompactPCI)가 초기화 되었는지를 확인한다
    INT16   PASCAL EXPORT AxtIsInitializedBus(INT16 BusType);
    // 새로운 베이스보드를 통합라이브러리에 추가한다.
    INT16   PASCAL EXPORT AxtOpenDevice(INT16 BusType, UINT32 dwBaseAddr);
    // 새로운 베이스보드를 배열을 이용하여 한꺼번에 통합라이브러리에 추가한다.
    INT16   PASCAL EXPORT AxtOpenDeviceAll(INT16 BusType, INT16 nLen, UINT32 *dwBaseAddr);
    // 새로운 베이스보드를 자동으로 통합라이브러리에 추가한다.
    INT16   PASCAL EXPORT AxtOpenDeviceAuto(INT16 BusType);
    // 추가된 베이스보드를 전부 닫는다
    void    PASCAL EXPORT AxtCloseDeviceAll();

//========== 베이스보드의인터럽트 사용의 허가 및 금지 ==============================================
    // 베이스보드의 인터럽트의 사용을 허가한다
    void    PASCAL EXPORT AxtEnableInterrupt(INT16 nBoardNo);
    // 베이스보드의 인터럽트가 사용 가능한지를 확인한다
    BOOL    PASCAL EXPORT AxtIsEnableInterrupt(INT16 nBoardNo);
    // 베이스보드의 인터럽트의 사용을 금지한다
    void    PASCAL EXPORT AxtDisableInterrupt(INT16 nBoardNo);

//========== 베이스보드의 인터럽트 마스크 및 플래그 레지스터 =======================================
    // 베이스보드의 인터럽트 플래그 레지스터를 클리어 한다
    void    PASCAL EXPORT AxtInterruptFlagClear(INT16 nBoardNo);
    // 베이스보드에 장착된 각 모듈의 인터럽트를 사용할 수 있도록 해당 핀의 사용을 허용한다
    void    PASCAL EXPORT AxtWriteInterruptMaskModule(INT16 nBoardNo, UINT8 Mask);
    // 설정된 인터럽트 마스크 레지스터를 읽는다
    UINT8   PASCAL EXPORT AxtReadInterruptMaskModule(INT16 nBoardNo);
    // 베이스보드의 인터럽트 플래그 레지스터의 내용을 읽는다
    UINT8   PASCAL EXPORT AxtReadInterruptFlagModule(INT16 nBoardNo);

//========== 보드 정보 =============================================================================
    // 지정한 버스의 (오픈된) 베이스보드 갯수를 리턴한다
    INT16   PASCAL EXPORT AxtGetBoardCounts();
    // (오픈된) 모든 베이스보드 갯수를 리턴한다
    INT16   PASCAL EXPORT AxtGetBoardCountsBus(INT16 nBusType);
    // 지정한 베이스보드에 장착된 모듈의 ID 및 모듈의 갯수를 리턴한다
    INT16   PASCAL EXPORT AxtGetModuleCounts(INT16 nBoardNo, UINT8 *ModuleID);
    // 지정한 베이스보드에 장착된 모듈중 지정한 모듈 ID의 갯수를 리턴한다
    INT16   PASCAL EXPORT AxtGetModelCounts(INT16 nBoardNo, UINT8 ModuleID);
    // 모든 베이스보드에 장착된 모듈중 지정한 모듈ID를 가진 모듈의 갯수를 리턴한다
    INT16   PASCAL EXPORT AxtGetModelCountsAll(UINT8 ModuleID);
    // 지정한 베이스보드의 ID를 리턴한다
    INT16   PASCAL EXPORT AxtGetBoardID(INT16 nBoardNo);
    // 지정한 베이스보드의 Adress를 리턴한다.
    UINT32  PASCAL EXPORT AxtGetBoardAddress(INT16 nBoardNo);

    // Log Level을 설정한다.
    void    PASCAL EXPORT AxtSetLogLevel(INT16 nLogLevel);
    // Log Level을 확인한다.
    INT16   PASCAL EXPORT AxtGetLogLevel();

    /// Library Version Infomation
    char*   PASCAL EXPORT AxtGetLibVersion();
    char*   PASCAL EXPORT AxtGetLibDate();

//========== 에러코드 읽기 함수군 ==================================================================
    // 마지막 에러코드를 읽는다.
    INT16   PASCAL EXPORT Axtget_error_code();
    // 에러코드의 원인을 문자로 반환한다.
    char *  PASCAL EXPORT Axtget_error_msg(INT16 ErrorCode);

#ifdef __cplusplus
}
#endif    //__cplusplus

#endif    //__AXT_LIBRARY_H__
