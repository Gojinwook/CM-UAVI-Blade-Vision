// TCPServer.cpp: 구현 파일
//

#include "stdafx.h"
#include "uScan.h"
#include "TCPDefine.h"
#include "TCPServer.h"
#include "TCPFileReceiver.h"

CTCPServer::CTCPServer()
{
    m_strSaveFolderPath = _T("");
}
CTCPServer::~CTCPServer()
{
}

void CTCPServer::OnAccept(int nErrorCode)
{
    // nErrorCode : ==0(성공), !=0 (오류)
    if (nErrorCode)
    {
        CAsyncSocket::OnAccept(nErrorCode);

        DoubleLogOut("[TCPServer:OnAccept] OnAccept() 오류 : %d ", nErrorCode);
    }
    else
    {
        CTCPFileReceiver *m_pFileReceiverSocket = new CTCPFileReceiver();

        // Save 폴더 TCPServer 에서 받아서 설정
        // 현재는 기본값은 RMS 폴더
		m_pFileReceiverSocket->SetSaveFolder(this->GetSaveFolder());

        
		DoubleLogOut("[TCPServer:OnAccept] 소켓생성 시도.");
		if (Accept(*m_pFileReceiverSocket))
        {
			SOCKADDR_IN sockAddr; // Use SOCKADDR_IN for IPv4
			int nAddrLen = sizeof(sockAddr);

			m_pFileReceiverSocket->AsyncSelect(FD_READ | FD_CLOSE);
			// Call the underlying API function
            CString strClientIP = _T("");
			if ((m_pFileReceiverSocket->GetPeerName((SOCKADDR*)&sockAddr, &nAddrLen) != SOCKET_ERROR))
			{
				// Convert the IP address and port to a readable format
				UINT nClientPort;

				// Convert the binary IP address to a string
				strClientIP = inet_ntoa(sockAddr.sin_addr);
				// Convert the network-byte-order port to host-byte-order
				nClientPort = ntohs(sockAddr.sin_port); 
				
				// 클라이언트 포트번호는 의미 없는거 같아서 주석처리 - 250919, jhkim
				//DoubleLogOut("[TCPServer:OnAccept] 소켓생성 성공. 클라이언트 IP: %s, 포트: %d", strClientIP, nClientPort);
				DoubleLogOut("[TCPServer:OnAccept] 소켓생성 성공. 클라이언트 IP: %s", strClientIP);
			}
			else
				DoubleLogOut("[TCPServer:OnAccept] 소켓생성 성공. 클라이언트 정보획득 실패");

            THEAPP.m_pLogDlg->WriteLog(_T("[TCPServer:OnAccept] 파일 수신 대기 소켓 생성 완료."));
            if (strClientIP != _T(""))
                THEAPP.m_pLogDlg->WriteLog(_T("[TCPServer:OnAccept] 클라이언트 IP: ") + strClientIP);

			DoubleLogOut("[TCPFileReceiver] File READ 이벤트 대기");
		}
        else
        {
            delete m_pFileReceiverSocket;
            m_pFileReceiverSocket = nullptr;

            DoubleLogOut("[TCPServer:OnAccept] 소켓생성 실패 : %d ", WSAGetLastError());
        }
    }

    CAsyncSocket::OnAccept(nErrorCode);
}

BOOL CTCPServer::OpenTCPServer(int iPort, LPCTSTR strIp)
{

#ifdef INLINE_MODE
    strIp = INLINE_LOCALHOST;
#else
    strIp = NULL;
#endif

    BOOL bRet = FALSE;

    if (!AfxSocketInit())
    {
        int iLastError = WSAGetLastError();
        DoubleLogOut("[TCPServer::OpenTCPServer()] Winsock 초기화 실패 / code : %d", iLastError);
        
        bRet = FALSE;
    }
    else
    {
		if (iPort == 0)
        {
            iPort = TCP_SERVER_DEFAULT_PORT;
			THEAPP.Struct_PreferenceStruct.m_iTCPServerPortNo = iPort;
            DoubleLogOut("[TCPServer::OpenTCPServer()] Port 번호가 0으로 설정되어 있어 기본값(%d)으로 설정합니다.", iPort);
        }

        // TCP 서버 소켓 생성, FD_ACCEPT 이벤트 등록
        DoubleLogOut("[TCPServer::OpenTCPServer()] TCP Server Opening\t%s:%d", strIp, iPort);
        // IPAddr NULL : 모든 IP 주소에서 접속을 승인
        BOOL bSocketFlag = Create(iPort, SOCK_STREAM, FD_ACCEPT, strIp);

        // Auto Parameter update - 250910, jhkim
        if (bSocketFlag)
        {
            DoubleLogOut("[TCPServer::OpenTCPServer()] TCP Server Opened\t%s:%d", strIp, iPort);
            THEAPP.m_pLogDlg->WriteLog(_T("[TCPServer::OpenTCPServer()] TCP 서버 소켓 열기 완료."));
            if (strIp != NULL)
                THEAPP.m_pLogDlg->WriteLog(_T("[TCPServer::OpenTCPServer()] IP 주소 : ") + CString(strIp));
            THEAPP.m_pLogDlg->WriteLog(_T("[TCPServer::OpenTCPServer()] 포트 번호 : ") + CString(std::to_string(iPort).c_str()));
			SetPortNum(iPort);

            // TCP 서버 소켓을 논블로킹 모드로 설정하고, 수신 및 종료 이벤트를 감지하도록 설정
            bSocketFlag = Listen();
            if (bSocketFlag)
            {
                DoubleLogOut("[TCPServer::OpenTCPServer()] Listen() 성공");
				SetSaveFolder(THEAPP.Struct_PreferenceStruct.m_strRmsSaveFolderPath);
				DoubleLogOut("[TCPServer:OpenTCPServer()] Target RMS Folder Path set : " + this->GetSaveFolder());

                bRet = TRUE;
            }
            else
            {
                DoubleLogOut("[TCPServer::OpenTCPServer()] TCP Server Listen 실패 / code : %d", WSAGetLastError());

                bRet = FALSE;
            }
        }
        else
        {
            int iLastError = GetLastError();
            switch (iLastError)
            {
            case WSAEACCES:
                DoubleLogOut("[TCPServer::OpenTCPServer()] ERROR : Permission denied");
                break;
            case WSAEADDRINUSE:
                DoubleLogOut("[TCPServer::OpenTCPServer()] ERROR : Address already used.\t%s:%d", strIp, iPort);
                break;
            case WSAEADDRNOTAVAIL:
                DoubleLogOut("[OpenTCPServer()] ERROR : Cannot assign requested address.\t%s:%d", strIp, iPort);
                break;
            default:
                DoubleLogOut("[OpenTCPServer()] ERROR : Unknown error");
                break;
            }

            bRet = FALSE;
        }
    }

    return bRet;
}

void CTCPServer::CloseTCPServer()
{
    Close();

    DoubleLogOut("[TCPServer:Close] TCP Server (Listenr socket) Closed.");
}
