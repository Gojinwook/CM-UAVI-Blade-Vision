#pragma once
#include "TCPDefine.h"

#define TCP_BUFFER_SIZE (64 * 1024)

void DoubleLogOut(const char *format, ...);

class CTCPFileReceiver : public CAsyncSocket
{
public:
	// 수신 헤더 열거형
	enum RECV_STATE
	{
		RECV_HEADER,
		RECV_NAME,
		RECV_DATA,
		RECV_DONE,
		RECV_ERR,
		RECV_IDLE
	};

	CTCPFileReceiver();
	virtual ~CTCPFileReceiver();


	void SetSaveFolder(const CString &folder) { m_strSaveFolderPath = folder; }
	CString GetSaveFolder() { return m_strSaveFolderPath; }

protected:
	virtual void OnReceive(int nErrorCode);
	virtual void OnClose(int nErrorCode);

private:
	// 수신 상태 변수
	RECV_STATE m_RecvState;

	// 헤더 관련 변수
	struct stHeader
	{
		uint64_t fileSizeN;	  // 파일 크기
		uint32_t nameLengthN; // 파일 이름 길이
	} m_stHeader;
	size_t m_HdrRecv;

	// 파일 이름 관련 변수
	std::string m_NameBuf;
	size_t m_NameRecv;

	// TCP 전송 파일 관련 변수
	uint64_t m_FileSize;
	uint64_t m_FileRecv;
	CFile m_FileOut;
	CString m_strFileName;
	CString m_strSaveFolderPath;
	CString m_strSavePath;

private:
	// 파일 열기 시도. 이미 열려 있으면 바로 TRUE 반환.
	BOOL EnsureOpenFile();
	// 지정한 크기만큼 데이터를 모두 수신할 때까지 반복 수신
	BOOL RecvAllTo(void *dst, size_t need, size_t &filled);
};
