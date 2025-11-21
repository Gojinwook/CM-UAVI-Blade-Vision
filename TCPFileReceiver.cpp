#include "StdAfx.h"
#include "uScan.h"
#include "TCPFileReceiver.h"
#include "ModelLoadProgressDlg.h"
#include "Define.h"

#include <thread>

// 로그 메시지를 두 군데(디버그 출력 창, LogEvent.txt)에 동시에 출력 - 250916, jhkim
void DoubleLogOut(const char *format, ...);

void ThmRecipeUpdater(CString strFilePath)
{
	if (!strFilePath.IsEmpty())
	{
		THEAPP.m_pModelDataManager->thmUpdateRecipe.Add(thread(
			[strFilePath]
			{
				THEAPP.m_pModelDataManager->LoadRmsParamData(strFilePath);
			}
		));		
	}
	else
		return;
}

// need 바이트를 dst에 모두 채우도록 시도.
// filled에 실제 채운 바이트 수를 반환.
BOOL CTCPFileReceiver::RecvAllTo(void *dst, size_t need, size_t &filled)
{
	BOOL bRet = FALSE;
	filled = 0;
	char *p = (char *)dst;

	for (;;)
	{
		// 64kb 단위로 나누어 수신 시도
		// (한 번에 너무 크게 수신 시도하면 WSAEFAULT 발생 가능성 있음)
		// (단, 최초 수신되는 16Byte는 Header 로 고정하여 읽는다.)
		int toRead = (int)min((size_t)(TCP_BUFFER_SIZE), need - filled);

		int iRecv = Receive(p + filled, toRead);
		if (iRecv == SOCKET_ERROR)
		{
			int iErrCode = WSAGetLastError();
			if (iErrCode == WSAEWOULDBLOCK)
			{
				bRet = TRUE;
				break; // 나중에 다시 시도
			}

			m_RecvState = RECV_ERR;
			bRet = FALSE;

			DoubleLogOut("[TCPFileReceiver:OnReceive] Receive() 오류 : %d ", iErrCode);
			break;
		}
		if (iRecv == 0)
		{
			bRet = TRUE; // peer closed

			DoubleLogOut("[TCPFileReceiver:OnReceive] Peer closed. ");
			ThmRecipeUpdater(m_strSavePath);

			break;
		}

		filled += iRecv;
		if (filled >= need)
		{
			bRet = TRUE;

			// DoubleLogOut("[TCPFileReceiver:OnReceive] RecvAllTo() 완료 : %d bytes", filled);
			break;
		}
	}

	return bRet;
}
// File Name 로그 한번만 로그 남기기위한 변수
static BOOL g_bIsNameLogged = FALSE;
// CTCPFileReceiver 멤버 함수
BOOL CTCPFileReceiver::EnsureOpenFile()
{
	BOOL bRet = FALSE;

	if (m_FileOut.m_hFile != CFile::hFileNull)
	{
		bRet = TRUE;
	}
	else
	{
		CString filename(m_NameBuf.c_str());
		if (filename.IsEmpty())
		{
			filename = _T("noname.bin");
			DoubleLogOut("[TCPFileReceiver::OnReceive] filename is empty. set \"noname.bin\"");
		}
		else
		{
			if (!g_bIsNameLogged)
			{
				DoubleLogOut("[TCPFileReceiver:OnReceive] Name Recv OK. Name : %s", filename);
				g_bIsNameLogged = TRUE;
			}
		}
		m_strFileName = filename;

		// 저장 폴더가 지정되지 않았으면 기본 폴더로 설정
		if (m_strSaveFolderPath.IsEmpty())
		{
			m_strSaveFolderPath = THEAPP.Struct_PreferenceStruct.m_strBasic_FolderPath;
			DoubleLogOut("[TCPFileReceiver::OnReceive] SaveFolderPath is empty. set :" + m_strSaveFolderPath);
		}
		else
		{
			// Path 설정, 끝에 \가 없으면 붙임
			if (m_strSaveFolderPath.Right(1) != _T("\\"))
				m_strSaveFolderPath += _T("\\");

			DoubleLogOut("[TCPFileReceiver::OnReceive] SaveFolderPath is set :" + m_strSaveFolderPath);
		}

		// 최종 경로
		m_strSavePath = m_strSaveFolderPath + filename;

		// 1) 기존 파일이 있으면 읽기전용 해제 후 삭제 시도(덮어쓰기 보장)
		DWORD attr = ::GetFileAttributes(m_strSavePath);
		if (attr != INVALID_FILE_ATTRIBUTES)
		{
			if (attr & FILE_ATTRIBUTE_READONLY)
			{
				// 읽기전용이면 해제
				::SetFileAttributes(m_strSavePath, attr & ~FILE_ATTRIBUTE_READONLY);
			}
			// 삭제 실패해도 Open에서 truncate 시도할 것이나,
			// 공유 충돌을 줄이기 위해 우선 삭제를 시도함.
			::DeleteFile(m_strSavePath);
		}

		// 2) 새로 생성(없으면 생성, 있으면 잘라내기) + 공유 거부 완화
		CFileException ex;
		UINT openFlags = CFile::modeCreate | CFile::modeWrite | CFile::modeNoTruncate | CFile::typeBinary | CFile::shareDenyNone; // 공유 충돌 완화

		if (!m_FileOut.Open(m_strSavePath, openFlags, &ex))
		{
			// 혹시 공유 충돌이면 한 번 더 강하게 시도: 파일 제거 후 재시도
			if (ex.m_cause == CFileException::sharingViolation ||
				ex.m_cause == CFileException::accessDenied)
			{
				::DeleteFile(m_strSavePath);
				if (!m_FileOut.Open(m_strSavePath, openFlags, &ex))
				{
					m_RecvState = RECV_ERR;

					DoubleLogOut("[TCPFileReceiver:OnReceive] 파일 열기 실패 : " + m_strSavePath);
					bRet = FALSE;
				}
			}
			else
			{
				DoubleLogOut("[TCPFileReceiver:OnReceive] 파일 열기 실패(권한 충돌) : " + m_strSavePath);

				m_RecvState = RECV_ERR;
				bRet = FALSE;
			}
		}
	}

	return bRet;
}

// CTCPFileReceiver 생성자
CTCPFileReceiver::CTCPFileReceiver()
	: m_RecvState(RECV_HEADER),
	  m_HdrRecv(0),
	  m_NameRecv(0),
	  m_FileSize(0),
	  m_FileRecv(0)
{
	if (m_FileOut.m_hFile != CFile::hFileNull)
	{
		try
		{
			m_FileOut.Close();
		}
		catch (CFileException *e)
		{
			DoubleLogOut("[TCPFileReceiver:OnReceive] Error closing file: %d", e->m_cause);
			e->Delete();
		}
	}
	m_NameBuf.clear();
	m_strSavePath.Empty();

	m_stHeader.fileSizeN = 0;
	m_stHeader.nameLengthN = 0;
}

CTCPFileReceiver::~CTCPFileReceiver()
{
	if (m_FileOut.m_hFile != CFile::hFileNull)
		m_FileOut.Close();
}

void CTCPFileReceiver::OnReceive(int nErrorCode)
{
	// nErrorCode : ==0(성공), !=0 (오류)
	if (nErrorCode != 0)
	{
		m_RecvState = RECV_ERR;
		CAsyncSocket::OnReceive(nErrorCode);

		DoubleLogOut("[TCPFileReceiver:OnReceive] OnReceive 오류 : %d ", nErrorCode);
	}
	else
	{
		CString strLogMsg;

		for (;;)
		{

			if (m_RecvState == RECV_HEADER)
			{
				BYTE *pHdr = (BYTE *)&m_stHeader;
				size_t filled = 0;
				BOOL bRecv = RecvAllTo(pHdr + m_HdrRecv, sizeof(m_stHeader) - m_HdrRecv, filled);
				if (!bRecv)
					break;

				m_HdrRecv += filled;
				if (m_HdrRecv < sizeof(m_stHeader))
					break;

				/**
				 * 250911, jhkim
				 * ntohl, ntohll : 네트워크 바이트 순서(빅 엔디언)를 호스트 바이트 순서로 변환
				 * htonl, htonll : 호스트 바이트 순서(리틀 엔디언)를 네트워크 바이트 순서로 변환
				 *
				 * 네트워크 바이트 순서는 빅 엔디언(big-endian) 방식
				 * 호스트 바이트 순서는 사용하는 시스템에 따라 다름(리틀 엔디언(little-endian) 방식이 일반적임)
				 * ll   : long long (64bit)
				 * l    : long (32bit)
				 */
				m_FileSize = ntohll(m_stHeader.fileSizeN);
				uint32_t nameLen = ntohl(m_stHeader.nameLengthN);

				DoubleLogOut("[TCPFileReceiver:OnReceive] Header Recv OK. FileSize : %d, NameLen : %d", m_FileSize, nameLen);

				if (nameLen > (1024 * 1024))
				{
					DoubleLogOut("[TCPFileReceiver:OnReceive] ERROR : File name is too long. File Name Length : %d", nameLen);
					m_RecvState = RECV_ERR;

					break;
				}

				m_NameBuf.resize(nameLen);
				m_NameRecv = 0;
				m_RecvState = RECV_NAME;
			}
			else if (m_RecvState == RECV_NAME)
			{
				if (!m_NameBuf.empty())
				{
					while (m_NameRecv < m_NameBuf.size())
					{
						size_t filled = 0;

						if (!RecvAllTo((void *)(m_NameBuf.data() + m_NameRecv), (m_NameBuf.size() - m_NameRecv), filled))
							break;

						m_NameRecv += filled;
						if (filled == 0)
							break;
					}
					if (m_NameRecv < m_NameBuf.size())
						break;
				}

				if (!EnsureOpenFile())
				{
					// DoubleLogOut("[TCPFileReceiver:OnReceive] EnsureOpenFile() 실패");
					break;
				}

				m_RecvState = RECV_DATA;
				DoubleLogOut("[TCPFileReceiver::OnReceive] file save path : " + m_strSavePath);
			}
			else if (m_RecvState == RECV_DATA)
			{
				if (!EnsureOpenFile())
				{
					// DoubleLogOut("[TCPFileReceiver:OnReceive] EnsureOpenFile() 실패");
					break;
				}

				// 64kb 단위로 수신
				BYTE buf[TCP_BUFFER_SIZE];
				// 남은 용량 계산
				uint64_t remain = (m_FileSize >= m_FileRecv) ? (m_FileSize - m_FileRecv) : 0;
				// 이번에 수신할 용량
				uint64_t want64 = (remain < (uint64_t)sizeof(buf)) ? remain : (uint64_t)sizeof(buf);
				// int로 변환
				int want = (int)want64;

				// 수신 시도
				// SOCKET_ERROR/-1: 오류, 0: 연결 종료, 양수: 수신 바이트 수
				int ret = Receive(buf, want);
				if (ret == SOCKET_ERROR)
				{
					int iErrCode = GetLastError();
					// WSAEWOULDBLOCK : 논블로킹 소켓에서 데이터가 아직 도착하지 않음
					// 에러가 아님, 나중에 다시 시도
					if (iErrCode == WSAEWOULDBLOCK)
						break;
					else
					{
						DoubleLogOut("[TCPFileReceiver:OnReceive] Receive() 오류 : %d ", iErrCode);
						m_RecvState = RECV_ERR;
						break;
					}
				}

				// 연결 종료됨.
				if (ret == 0)
				{
					// 데이터가 모두 수신된 상태에서 종료된 것인지 확인.
					// 모두 수신된 상태이면 RECV_DONE으로 상태 전환.

					if (m_FileRecv == m_FileSize)
						m_RecvState = RECV_DONE;
					else
						DoubleLogOut("[TCPFileReceiver:OnReceive] Peer closed. Incomplete file recv. FileSize=%d, RecvSize=%d", m_FileSize, m_FileRecv);

					break;
				}

				// 파일에 기록
				try
				{
					m_FileOut.Write(buf, ret);
				}
				catch (CFileException *e)
				{
					DoubleLogOut("[TCPFileReceiver:OnReceive] File Write Error: %d", e->m_cause);
					m_RecvState = RECV_ERR;
					e->Delete();
					break;
				}
				m_FileRecv += (uint64_t)ret;

				strLogMsg.Format("[TCPFileReceiver:OnReceive] 파일 수신중... %d / %d (Bytes)", m_FileRecv, m_FileSize);
				THEAPP.m_pLogDlg->WriteLog(strLogMsg, LOG_COLOR_BLUE);

				// 모두 수신되었는지 확인
				if (m_FileRecv >= m_FileSize)
				{
					break;
				}
			}
			else
			{
				DoubleLogOut("[TCPFileReceiver:OnReceive] Invalid State. m_RecvState : %d", m_RecvState);
				break;
			}
		}

		if ((m_FileRecv >= m_FileSize) || (m_RecvState == RECV_DONE))
		{
			CString logMsg;
			logMsg.Format("[TCPFileReceiver:OnReceive] 파일 수신 완료. \n 파일명 : %s \n 파일크기 : %d(kb) \n 저장경로 : %s", m_strFileName, (m_FileSize/1024), m_strSavePath);
			THEAPP.m_pLogDlg->WriteLog(logMsg, LOG_COLOR_BLUE);
			DoubleLogOut(logMsg);
			if (m_FileOut.m_hFile != CFile::hFileNull)
			{
				m_FileOut.Close();

				ThmRecipeUpdater(m_strSavePath);

				m_strSavePath.Empty();
			}

			m_RecvState = RECV_HEADER;
			m_HdrRecv = 0;
			m_stHeader.fileSizeN = 0;
			m_stHeader.nameLengthN = 0;
			m_NameBuf.clear();
			m_NameRecv = 0;
			m_FileSize = 0;
			m_FileRecv = 0;
		}
	}

	CAsyncSocket::OnReceive(nErrorCode);
}

void CTCPFileReceiver::OnClose(int nErrorCode)
{
	if (m_FileOut.m_hFile != CFile::hFileNull)
		m_FileOut.Close();

	DoubleLogOut("[TCPFileReceiver:OnClose] TCP Server (File Reciever socket) Closed.");
	THEAPP.m_pLogDlg->WriteLog(_T("[TCPFileReceiver:OnClose] TCP 파일 소켓 종료."), LOG_COLOR_BLUE);

	CAsyncSocket::OnClose(nErrorCode);
}

// 로그 메시지를 두 군데(디버그 출력 창, LogEvent.txt)에 동시에 출력 - 250916, jhkim
void DoubleLogOut(const char *format, ...)
{
	CString strOutMsg;
	va_list args;

	// va_start는 가변 인자 목록을 초기화합니다.
	// 두 번째 인자는  바로 앞에 위치한 마지막 고정 인자입니다.
	va_start(args, format);

	// 로그 메시지를 저장할 버퍼를 정의합니다.
	// 충분히 큰 크기로 설정하거나 동적으로 할당할 수 있습니다.
	std::vector<char> buffer(512);

	// vprintf와 유사한 vsnprintf를 사용하여 안전하게 문자열을 포맷팅합니다.
	// 이는 버퍼 오버플로우를 방지하는 데 유용합니다.
	int nWritten = vsnprintf(buffer.data(), buffer.size(), format, args);

	// vsnprintf가 -1을 반환하면 버퍼 크기가 충분하지 않았음을 의미합니다.
	if (nWritten < 0)
	{
		// 더 큰 버퍼로 다시 시도하거나, 오류를 출력합니다.
	}
	else
	{
		strOutMsg.Format("%s", buffer.data());
	}

	// va_end는 가변 인자 목록 사용을 종료하고 정리합니다.
	va_end(args);

	AfxTrace(strOutMsg + "\n");

	THEAPP.SaveLog(strOutMsg);
}
