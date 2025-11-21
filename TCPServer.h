// TCPServer.h : header file
//
#pragma once
#define INLINE_LOCALHOST _T("192.168.1.12")
#define TCP_SERVER_DEFAULT_PORT 21000
#define LOCALHOST _T("127.0.0.1")

// File Liestener Socket Class
class CTCPServer : public CAsyncSocket
{
public:
	CTCPServer();
	virtual ~CTCPServer();

	
	// TCP 서버 열기 및 닫기
	BOOL OpenTCPServer(int port, LPCTSTR ip = NULL);
	void CloseTCPServer();
	void SetSaveFolder(const CString &folder) { m_strSaveFolderPath = folder; }
	CString GetSaveFolder() { return m_strSaveFolderPath; }
	void SetPortNum(int iPort) { m_iPort = iPort; }
	int GetPortNum() { return m_iPort; }

protected:
	virtual void OnAccept(int nErrorCode);

private:
	// CFileReceiverSocket m_pFileReceiverSocket;
	CString m_strSaveFolderPath;
	int m_iPort;

};