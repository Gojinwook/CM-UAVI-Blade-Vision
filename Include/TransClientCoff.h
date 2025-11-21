#ifndef ClientDllH
#define ClientDllH

#define INPEXP __declspec(dllimport)

extern "C"
{
    void INPEXP SetSQLServer(void);          // SQL 서버를 설정한다. 초기에 한번만 가능하다.

    void INPEXP ClearSQLBuffer(void);          // 전송버퍼를 초기화한다.
    void INPEXP AddSQLBuffer(char *BStr);      // 전송버퍼에 내용을 추가한다.
    void INPEXP TransSQLBuffer(void);          // 전송버퍼의 내용을 서버로 전송한다. 내용은 보존된다.
}

#endif


