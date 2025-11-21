#ifndef __AXT_DIO_H__
#define __AXT_DIO_H__

#include "AxtLIBDef.h"
#include "DIODef.h"

/*------------------------------------------------------------------------------------------------*
    AXTDIO Library - Digital Input/Ouput module
    적용제품
        SIO-DI32  - 입력 32점
        SIO-DO32P - 출력 32점, 포토커플러 출력타입
        SIO-DO32T - 출력 32점, 파워TR 출력타입
        SIO-DB32P - 입력 16점 / 출력 32점, 포토커플러 출력타입
        SIO-DB32T - 입력 16점 / 출력 32점, 파워TR 출력타입
 *------------------------------------------------------------------------------------------------*/

#ifdef __cplusplus
extern "C" {
#endif    //__cplusplus

//-----------공통적으로 사용하는 인자(Parameter) ---------------------------------------------------
//  nBoardNo : 베이스보드번호, 검출된 순서대로 0부터 할당된다
//  nModuleNo: DIO모듈 번호, DIO모듈의 종류에 관계없이 검출된 순서대로 0부터 할당된다
//--------------------------------------------------------------------------------------------------

//========== 초기화 함수군 =========================================================================
    // DIO모듈을 초기화한다. 열려있는 모든베이스보드에서 DIO모듈을 검색하여 초기화한다
    BOOL    PASCAL EXPORT InitializeDIO();
    // DIO모듈을 사용할 수 있도록 라이브러리가 초기화되었는가 ?
    BOOL    PASCAL EXPORT DIOIsInitialized();
    //void  PASCAL EXPORT DIOStopService();

//========== 인터럽트 관련 함수군 ==================================================================
    // 인터럽트 메세지 및 핸들러를 설정한다.
    void    PASCAL EXPORT DIOSetWindowMessage(HANDLE hWnd, UINT16 wMsg, AXT_DIO_INTERRUPT_PROC proc);
    // 지정한 모듈의 인터럽트를 허용한다.
    void    PASCAL EXPORT DIOEnableInterrupt(INT16 nModuleNo);
    // 지정한 모듈의 인터럽트를 금지한다.
    void    PASCAL EXPORT DIODisableInterrupt(INT16 nModuleNo);
    // 모듈이 인터럽트 허용상태인지를 확인한다.
    BOOL    PASCAL EXPORT DIOIsInterruptEnabled(INT16 nModuleNo);

//========== 보드 및 모듈 정보 함수군 ==============================================================
    // 지정한 베이스보드가 열려있는지(오픈되었는지)를 확인한다
    BOOL    PASCAL EXPORT DIOIsOpenBoard(INT16 nBoardNo);
    // 지정한 DIO모듈이 열려있는지(오픈되었는지)를 확인한다
    BOOL    PASCAL EXPORT DIOIsOpenModule(INT16 nModuleNo);
    // 유효한 모듈번호인지를 확인한다
    BOOL    PASCAL EXPORT DIOIsValidModuleNo(INT16 nModuleNo);
    // DIO모듈이 장착된 베이스보드의 갯수를 리턴한다
    UINT16  PASCAL EXPORT DIOget_board_count();
    // DIO모듈의 갯수를 리턴한다.
    UINT16  PASCAL EXPORT DIOget_module_count();
    // 지정한 번지를 사용하는 베이스보드 번호를 리턴한다
    INT16   PASCAL EXPORT DIOget_boardno(UINT32 address);
    // address : 절대번지
    // 리턴값
    //    0.. = 베이스보드-1
    //    -1  = 유효하지 않는 번지
    // 지정한 모듈의 모델번호를 리턴한다
    // 리턴값
    //    97h(AXT_SIO_DI32)  = SIO-DI32
    //    98h(AXT_SIO_DO32P) = SIO-DO32P
    //    99h(AXT_SIO_DB32P) = SIO-DB32P
    //    9Eh(AXT_SIO_DO32T) = SIO-DO32T
    //    9Fh(AXT_SIO_DB32T) = SIO-DB32T
    //    00h = 유효하지 않는 모듈번호
    UINT16  PASCAL EXPORT DIOget_module_id(INT16 nModuleNo);
    // 지정한 모듈의 베이스보드내 모듈 위치를 리턴한다.
    INT16   PASCAL EXPORT DIOget_module_pos(INT16 nModuleNo);
    // 지정한 모듈의 출력점수를 리턴한다.
    UINT16  PASCAL EXPORT DIOget_output_number(INT16 nModuleNo);
    // 지정한 모듈의 입력점수를 리턴한다.
    UINT16  PASCAL EXPORT DIOget_input_number(INT16 nModuleNo);
    // 지정한 베이스보드의 모듈위치에 있는 DIO모듈의 모듈번호를 리턴한다.
    UINT16  PASCAL EXPORT DIOget_module_number(INT16 nBoardNo, INT16 nModulePos);
    // 지정한 모듈번호의 베이스보드번호와 모듈위치를 리턴한다
    BOOL    PASCAL EXPORT DIOget_module_info(INT16 nModuleNo, INT16 *pBoardNo, INT16 *nModulePos);
    // 지정한 모델의 모듈의 갯수를 리턴한다.
    // ModuleID
    //    97h(AXT_SIO_DI32)  : SIO-DI32
    //    98h(AXT_SIO_DO32P) : SIO-DO32P
    //    99h(AXT_SIO_DB32P) : SIO-DB32P
    //    9Eh(AXT_SIO_DO32T) : SIO-DO32T
    //    9Fh(AXT_SIO_DB32T) : SIO-DB32T
    //    00h(DIO_MODULE_ALL): 모든 DIO모듈
    // 리턴값: 모듈의 갯수를 리턴한다
    UINT16  PASCAL EXPORT DIOget_open_module_count(INT16 ModuleID);

//========== Write port(Register) 함수군 ===========================================================
    // offset
    //    DIOwrite_outport(),       DIOread_outport()      : 0부터 총출력점수-1까지 사용 가능
    //    DIOwrite_outport_bit(),   DIOread_outport_bit()  : SIO-DI32:사용불가, SIO-DB32:0..15, SIO-DO32:0..31
    //    DIOwrite_outport_byte(),  DIOread_outport_byte() : SIO-DI32:사용불가, SIO-DB32:0..1,  SIO-DO32:0..3
    //    DIOwrite_outport_word(),  DIOread_outport_word() : SIO-DI32:사용불가, SIO-DB32:0,     SIO-DO32:0..1
    //    DIOwrite_outport_dword(), DIOread_outport_dword(): SIO-DI32:사용불가, SIO-DB32:0,     SIO-DO32:0
    // 리턴값
    //    DIOread_outport()      : 0(OFF), 1(ON)
    //    DIOread_outport_bit()  : 0(OFF), 1(ON)
    //    DIOread_outport_byte() : 00h .. FFh
    //    DIOread_outport_word() : 0000h .. FFFFh
    //    DIOread_outport_dword(): 00000000h .. FFFFFFFFh

    // 출력(Output) 포트로에 1비트의 데이터를 써넣는다. 점수 단위
    void    PASCAL EXPORT DIOwrite_outport(UINT16 offset, BOOL bValue);
    // 출력(Output) 포트로에 1비트의 데이터를 써넣는다. 지정한 모듈의 점수 단위
    void    PASCAL EXPORT DIOwrite_outport_bit(INT16 nModuleNo, UINT16 offset, BOOL bValue);
    // 출력(Output) 포트로에 1바이트의 데이터를 써넣는다. 지정한 모듈의 바이트 단위
    void    PASCAL EXPORT DIOwrite_outport_byte(INT16 nModuleNo, UINT16 offset, UINT8 byValue);
    // 출력(Output) 포트로에 2바이트의 데이터를 써넣는다. 지정한 모듈의 워드 단위
    void    PASCAL EXPORT DIOwrite_outport_word(INT16 nModuleNo, UINT16 offset, UINT16 wValue);
    // 출력(Output) 포트로에 4바이트의 데이터를 써넣는다. 지정한 모듈의 더블워드 단위
    void    PASCAL EXPORT DIOwrite_outport_dword(INT16 nModuleNo, UINT16 offset, UINT32 lValue);
    // 출력(Output) 포트로부터 1비트의 데이터를 읽어들인다, 점수 단위
    BOOL    PASCAL EXPORT DIOread_outport(UINT16 offset);
    // 출력(Output) 포트로부터 1비트의 데이터를 읽어들인다. 지정한 모듈의 점수 단위
    BOOL    PASCAL EXPORT DIOread_outport_bit(INT16 nModuleNo, UINT16 offset);
    // 출력(Output) 포트로부터 1바이트의 데이터를 읽어들인다, 지정한 모듈의 바이트 단위
    UINT8   PASCAL EXPORT DIOread_outport_byte(INT16 nModuleNo, UINT16 offset);
    // 출력(Output) 포트로부터 2바이트의 데이터를 읽어들인다, 지정한 모듈의 워드 단위
    UINT16  PASCAL EXPORT DIOread_outport_word(INT16 nModuleNo, UINT16 offset);
    // 출력(Output) 포트로부터 4바이트의 데이터를 읽어들인다, 지정한 모듈의 더블워드 단위
    UINT32  PASCAL EXPORT DIOread_outport_dword(INT16 nModuleNo, UINT16 offset);

//========== Input port 함수군 - 읽기 전용 포트 ====================================================
    // offset
    //    DIOread_inport()      : 0부터 총입력점수-1까지 사용 가능
    //    DIOread_inport_bit()  : SIO-DI32:0..31, SIO-DB32:0..15, SIO-DO32:사용불가
    //    DIOread_inport_byte() : SIO-DI32:0..3,  SIO-DB32:0..1,  SIO-DO32:사용불가
    //    DIOread_inport_word() : SIO-DI32:0..1,  SIO-DB32:0,     SIO-DO32:사용불가
    //    DIOread_inport_dword(): SIO-DI32:0,     SIO-DB32:0,     SIO-DO32:사용불가
    // 리턴값
    //    DIOread_inport()      : 0(OFF), 1(ON)
    //    DIOread_inport_bit()  : 0(OFF), 1(ON)
    //    DIOread_inport_byte() : 00h .. FFh
    //    DIOread_inport_word() : 0000h .. FFFFh
    //    DIOread_inport_dword(): 00000000h .. FFFFFFFFh

    // 입력(Input) 포트로부터 1비트의 데이터를 읽어들인다. 점수 단위
    BOOL    PASCAL EXPORT DIOread_inport(UINT16 offset);
    // 입력(Input) 포트로부터 1비트의 데이터를 읽어들인다. 지정한 모듈의 점수 단위
    BOOL    PASCAL EXPORT DIOread_inport_bit(INT16 nModuleNo, UINT16 offset);
    // 입력(Input) 포트로부터 1바이트의 데이터를 읽어들인다. 지정한 모듈의 바이트 단위
    UINT8   PASCAL EXPORT DIOread_inport_byte(INT16 nModuleNo, UINT16 offset);
    // 입력(Input) 포트로부터 2바이트의 데이터를 읽어들인다. 지정한 모듈의 워드 단위
    UINT16  PASCAL EXPORT DIOread_inport_word(INT16 nModuleNo, UINT16 offset);
    // 입력(Input) 포트로부터 4바이트의 데이터를 읽어들인다. 지정한 모듈의 더블워드 단위
    UINT32  PASCAL EXPORT DIOread_inport_dword(INT16 nModuleNo, UINT16 offset);

//========== Interrupt Up-edge port(Register) 함수군 ===============================================
    // offset
    //    DIOwrite_upedge(),       DIOread_upedge()      : 0부터 총입력점수-1까지 사용 가능
    //    DIOwrite_upedge_bit(),   DIOread_upedge_bit()  : SIO-DI32:0..31, SIO-DB32:0..15, SIO-DO32:사용불가
    //    DIOwrite_upedge_byte(),  DIOread_upedge_byte() : SIO-DI32:0..3,  SIO-DB32:0..1,  SIO-DO32:사용불가
    //    DIOwrite_upedge_word(),  DIOread_upedge_word() : SIO-DI32:0..1,  SIO-DB32:0,     SIO-DO32:사용불가
    //    DIOwrite_upedge_dword(), DIOread_upedge_dword(): SIO-DI32:0,     SIO-DB32:0,     SIO-DO32:사용불가
    // 리턴값
    //    DIOread_upedge()      : 0(OFF), 1(ON)
    //    DIOread_upedge_bit()  : 0(OFF), 1(ON)
    //    DIOread_upedge_byte() : 00h .. FFh
    //    DIOread_upedge_word() : 0000h .. FFFFh
    //    DIOread_upedge_dword(): 00000000h .. FFFFFFFFh

    // 상승에지(Upedge) 포트로에 1비트의 데이터를 써넣는다. 점수 단위
    void    PASCAL EXPORT DIOwrite_upedge(UINT16 offset, BOOL bValue);
    // 상승에지(Upedge) 포트로에 1비트의 데이터를 써넣는다. 지정한 모듈의 점수 단위
    void    PASCAL EXPORT DIOwrite_upedge_bit(INT16 nModuleNo, UINT16 offset, BOOL bValue);
    // 상승에지(Upedge) 포트로에 1바이트의 데이터를 써넣는다. 지정한 모듈의 바이트 단위
    void    PASCAL EXPORT DIOwrite_upedge_byte(INT16 nModuleNo, UINT16 offset, UINT8 byValue);
    // 상승에지(Upedge) 포트로에 2바이트의 데이터를 써넣는다. 지정한 모듈의 워드 단위
    void    PASCAL EXPORT DIOwrite_upedge_word(INT16 nModuleNo, UINT16 offset, UINT16 wValue);
    // 상승에지(Upedge) 포트로에 4바이트의 데이터를 써넣는다. 지정한 모듈의 더블워드 단위
    void    PASCAL EXPORT DIOwrite_upedge_dword(INT16 nModuleNo, UINT16 offset, UINT32 lValue);
    // 상승에지(Upedge) 포트로부터 1비트의 데이터를 읽어들인다, 점수 단위
    BOOL    PASCAL EXPORT DIOread_upedge(UINT16 offset);
    // 상승에지(Upedge) 포트로부터 1비트의 데이터를 읽어들인다. 지정한 모듈의 점수 단위
    BOOL    PASCAL EXPORT DIOread_upedge_bit(INT16 nModuleNo, UINT16 offset);
    // 상승에지(Upedge) 포트로부터 1바이트의 데이터를 읽어들인다, 지정한 모듈의 바이트 단위
    UINT8   PASCAL EXPORT DIOread_upedge_byte(INT16 nModuleNo, UINT16 offset);
    // 상승에지(Upedge) 포트로부터 2바이트의 데이터를 읽어들인다, 지정한 모듈의 워드 단위
    UINT16  PASCAL EXPORT DIOread_upedge_word(INT16 nModuleNo, UINT16 offset);
    // 상승에지(Upedge) 포트로부터 4바이트의 데이터를 읽어들인다, 지정한 모듈의 더블워드 단위
    UINT32  PASCAL EXPORT DIOread_upedge_dword(INT16 nModuleNo, UINT16 offset);

//========== Interrupt Down-edge port(Register) 함수군 =============================================
    // offset
    //    DIOwrite_downedge(),       DIOread_downedge()      : 0부터 총입력점수-1까지 사용 가능
    //    DIOwrite_downedge_bit(),   DIOread_downedge_bit()  : SIO-DI32:0..31, SIO-DB32:0..15, SIO-DO32:사용불가
    //    DIOwrite_downedge_byte(),  DIOread_downedge_byte() : SIO-DI32:0..3,  SIO-DB32:0..1,  SIO-DO32:사용불가
    //    DIOwrite_downedge_word(),  DIOread_downedge_word() : SIO-DI32:0..1,  SIO-DB32:0,     SIO-DO32:사용불가
    //    DIOwrite_downedge_dword(), DIOread_downedge_dword(): SIO-DI32:0,     SIO-DB32:0,     SIO-DO32:사용불가
    // 리턴값
    //    DIOread_downedge()      : 0(OFF), 1(ON)
    //    DIOread_downedge_bit()  : 0(OFF), 1(ON)
    //    DIOread_downedge_byte() : 00h .. FFh
    //    DIOread_downedge_word() : 0000h .. FFFFh
    //    DIOread_downedge_dword(): 00000000h .. FFFFFFFFh

    // 하강에지(Downedge) 포트로에 1비트의 데이터를 써넣는다. 점수 단위
    void    PASCAL EXPORT DIOwrite_downedge(UINT16 offset, BOOL bValue);
    // 하강에지(Downedge) 포트로에 1비트의 데이터를 써넣는다. 지정한 모듈의 점수 단위
    void    PASCAL EXPORT DIOwrite_downedge_bit(INT16 nModuleNo, UINT16 offset, BOOL bValue);
    // 하강에지(Downedge) 포트로에 1바이트의 데이터를 써넣는다. 지정한 모듈의 바이트 단위
    void    PASCAL EXPORT DIOwrite_downedge_byte(INT16 nModuleNo, UINT16 offset, UINT8 byValue);
    // 하강에지(Downedge) 포트로에 2바이트의 데이터를 써넣는다. 지정한 모듈의 워드 단위
    void    PASCAL EXPORT DIOwrite_downedge_word(INT16 nModuleNo, UINT16 offset, UINT16 wValue);
    // 하강에지(Downedge) 포트로에 4바이트의 데이터를 써넣는다. 지정한 모듈의 더블워드 단위
    void    PASCAL EXPORT DIOwrite_downedge_dword(INT16 nModuleNo, UINT16 offset, UINT32 lValue);
    // 하강에지(Downedge) 포트로부터 1비트의 데이터를 읽어들인다, 점수 단위
    BOOL    PASCAL EXPORT DIOread_downedge(UINT16 offset);
    // 하강에지(Downedge) 포트로부터 1비트의 데이터를 읽어들인다. 지정한 모듈의 점수 단위
    BOOL    PASCAL EXPORT DIOread_downedge_bit(INT16 nModuleNo, UINT16 offset);
    // 하강에지(Downedge) 포트로부터 1바이트의 데이터를 읽어들인다, 지정한 모듈의 바이트 단위
    UINT8   PASCAL EXPORT DIOread_downedge_byte(INT16 nModuleNo, UINT16 offset);
    // 하강에지(Downedge) 포트로부터 2바이트의 데이터를 읽어들인다, 지정한 모듈의 워드 단위
    UINT16  PASCAL EXPORT DIOread_downedge_word(INT16 nModuleNo, UINT16 offset);
    // 하강에지(Downedge) 포트로부터 4바이트의 데이터를 읽어들인다, 지정한 모듈의 더블워드 단위
    UINT32  PASCAL EXPORT DIOread_downedge_dword(INT16 nModuleNo, UINT16 offset);

//========== Interrupt flag port(Register) 함수군 ==================================================
    // offset
    //    DIOread_flag()      : 0부터 총입력점수-1까지 사용 가능
    //    DIOread_flag_bit()  : SIO-DI32:0..31, SIO-DB32:0..15, SIO-DO32:사용불가
    //    DIOread_flag_byte() : SIO-DI32:0..3,  SIO-DB32:0..1,  SIO-DO32:사용불가
    //    DIOread_flag_word() : SIO-DI32:0..1,  SIO-DB32:0,     SIO-DO32:사용불가
    //    DIOread_flag_dword(): SIO-DI32:0,     SIO-DB32:0,     SIO-DO32:사용불가
    // 리턴값
    //    DIOread_flag()      : 0(OFF), 1(ON)
    //    DIOread_flag_bit()  : 0(OFF), 1(ON)
    //    DIOread_flag_byte() : 00h .. FFh
    //    DIOread_flag_word() : 0000h .. FFFFh
    //    DIOread_flag_dword(): 00000000h .. FFFFFFFFh

    // 인터럽트플래그(Flag) 포트로부터 1비트의 데이터를 읽어들인다, 점수 단위
    BOOL    PASCAL EXPORT DIOread_flag(UINT16 offset);
    // 인터럽트플래그(Flag) 포트로부터 1비트의 데이터를 읽어들인다. 지정한 모듈의 점수 단위
    BOOL    PASCAL EXPORT DIOread_flag_bit(INT16 nModuleNo, UINT16 offset);
    // 인터럽트플래그(Flag) 포트로부터 1바이트의 데이터를 읽어들인다, 지정한 모듈의 바이트 단위
    UINT8   PASCAL EXPORT DIOread_flag_byte(INT16 nModuleNo, UINT16 offset);
    // 인터럽트플래그(Flag) 포트로부터 2바이트의 데이터를 읽어들인다, 지정한 모듈의 워드 단위
    UINT16  PASCAL EXPORT DIOread_flag_word(INT16 nModuleNo, UINT16 offset);
    // 인터럽트플래그(Flag) 포트로부터 4바이트의 데이터를 읽어들인다, 지정한 모듈의 더블워드 단위
    UINT32  PASCAL EXPORT DIOread_flag_dword(INT16 nModuleNo, UINT16 offset);

//========== 에러코드 읽기 함수군 ==================================================================
    // 마지막 에러코드를 읽는다.
    INT16   PASCAL EXPORT DIOget_error_code();
    // 에러코드의 원인을 문자로 반환한다.
    char*   PASCAL EXPORT DIOget_error_msg(INT16 ErrorCode);

#ifdef __cplusplus
}
#endif    //__cplusplus

#endif    //__AXT_DIO_H__
