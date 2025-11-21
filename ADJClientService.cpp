// ADJClientService.cpp : 구현 파일입니다.
//
#include "stdafx.h"
#include "uScan.h"

#include "ADJClientService.h"

#include <tlhelp32.h>
#include <psapi.h>

#pragma comment(lib, "Ws2_32.lib")

#define DEFAULT_PORT "27015"
#define SERVER_IP "127.0.0.1" // 서버 IP 주소

void SendImageAndDefectName(SOCKET ConnectSocket, const cv::Mat& img, const std::string& defectName);
// CADJClientService

IMPLEMENT_DYNAMIC(CADJClientService, CWnd)

std::string ReceiveInspectionResult(SOCKET ConnectSocket);

// OpenCV의 Mat 객체를 JPEG 형식으로 인코딩하여 소켓을 통해 전송하는 함수
void SendImageAndDefectName(SOCKET ConnectSocket, const cv::Mat& img, const std::string& defectName) {
	// 이미지 데이터를 버퍼로 인코딩
	std::vector<uchar> buf;
	cv::imencode(".jpg", img, buf);
	int imgSize = buf.size();
	int defectNameSize = defectName.size();

	// 불량명 크기와 불량명 전송
	send(ConnectSocket, (char*)&defectNameSize, sizeof(defectNameSize), 0);
	send(ConnectSocket, defectName.c_str(), defectNameSize, 0);

	// 이미지 크기와 이미지 데이터 전송
	send(ConnectSocket, (char*)&imgSize, sizeof(imgSize), 0);
	send(ConnectSocket, (char*)buf.data(), imgSize, 0);
}

// 소켓에서 검사 결과를 수신하는 함수
std::string ReceiveInspectionResult(SOCKET ConnectSocket) {
	char buffer[1 + sizeof(float)]; // 'G'/'N'과 float 값을 저장하기 위한 버퍼
	int bytesReceived = recv(ConnectSocket, buffer, sizeof(buffer), 0);
	if (bytesReceived > 0) {
		// 'G' 또는 'N' 문자 추출
		std::string inspectionResult(1, buffer[0]);

		// float 값 추출
		float number;
		// float 값이 저장된 메모리 위치로부터 float 값 읽기
		memcpy(&number, buffer + 1, sizeof(float));

		// float 값을 문자열로 변환하여 inspectionResult에 추가
		inspectionResult += " " + std::to_string(number);

		return inspectionResult;
	}
	return "Error";
}

CADJClientService::CADJClientService()
{
	m_bConnect = FALSE;
	m_bTCPThreadRun = FALSE;
	m_bRealConnect = FALSE;

	m_strClientIP = "";
	m_nTimeOutCnt = 0;

	m_nADJDefectCnt = 0;
}

CADJClientService::~CADJClientService()
{
}

BEGIN_MESSAGE_MAP(CADJClientService, CWnd)
END_MESSAGE_MAP()

// CADJClientService 메시지 처리기입니다.
void CADJClientService::DoDeeplearningInspection(int iThreadNo, CString strLotID, int iTrayNo, int iModuleNo, CString sBarcodeID)
{
	try
	{
		if (THEAPP.Struct_PreferenceStruct.m_strADJModelName == "")
			return;

		CInspectService* pInspectService = THEAPP.m_pInspectService;

		m_nADJDefectCnt = 0;

		int result = 1;
	
		int iImageChannelADJ = 1;	// 단일 채널이라 일단 1로 설정

		int iCamImageWidth = THEAPP.m_pCameraManager->GetCamImageWidth();
		int iCamImageHeight = THEAPP.m_pCameraManager->GetCamImageHeight();

		CString sLog, sendLog;
	
		CString strModuleNo, strTrayNo, strDeepModelName, strDeepModel;
		strModuleNo.Format("%d", iModuleNo);
		strTrayNo.Format("%d", iTrayNo);
		strDeepModelName = THEAPP.Struct_PreferenceStruct.m_strADJModelName;
		strDeepModel.Format("DeepLearning_Model_%s", strDeepModelName);

		BOOL bApplyADJ = FALSE;
		BOOL bIsTimeOut = FALSE;

		HTuple HNoDefect;
		Hlong lNoDefectCount;

		Hobject HSelectedRgn, HIntersectRgn, HConnRgn, HDisplayRgn, HTempRgn1, HTempRgn2;
		Hobject HADJFilteredDefectRgn, HTotalDefectRgn, HImageDefectRgn[MAX_IMAGE_TAB];
		Hlong lBlobArea, lMaxBlobArea, lMaxArea, lMaxAreaImageIndex, iMaxDefectTypeIndex;

		double dBlobCenterX, dBlobCenterY;
		HTuple HlROIArea, HlBlobArea, HdBlobCenterX, HdBlobCenterY;
		Hlong lLTPointY, lLTPointX, lRBPointY, lRBPointX;
		HTuple HlLTPointY, HlLTPointX, HlRBPointY, HlRBPointX;
		Hobject HMaxAreaBlobRgn;
		HTuple HlArea, HdCenterX, HdCenterY;

		Hobject HADJSaveDefectRgn, HADJSaveImage, HADJSaveRegion;

		gen_empty_obj(&HADJFilteredDefectRgn);
		gen_empty_obj(&HTotalDefectRgn);
		gen_empty_obj(&HMaxAreaBlobRgn);

		// 전체 불량 합치기 - LeeGW
		for (int iImageIdx = 0; iImageIdx < MAX_IMAGE_TAB; iImageIdx++)
		{
			gen_empty_obj(&HImageDefectRgn[iImageIdx]);

			for (int iDefectType = 0; iDefectType < MAX_DEFECT_NAME; iDefectType++)
			{
				if (THEAPP.m_pGFunction->ValidHRegion(pInspectService->m_pInspectAlgorithm[iThreadNo].m_HTypeAIDefectRgn[iImageIdx][iDefectType])==FALSE)
					continue;

				concat_obj(HImageDefectRgn[iImageIdx], pInspectService->m_pInspectAlgorithm[iThreadNo].m_HTypeAIDefectRgn[iImageIdx][iDefectType], &HImageDefectRgn[iImageIdx]);
				concat_obj(HTotalDefectRgn, pInspectService->m_pInspectAlgorithm[iThreadNo].m_HTypeAIDefectRgn[iImageIdx][iDefectType], &HTotalDefectRgn);
			
			}


			// AI 미검사 블랍 영역에서 AI검사 블랍 영역 제외 - LeeGW
			union1(HImageDefectRgn[iImageIdx], &HImageDefectRgn[iImageIdx]);
			union1(pInspectService->m_pInspectAlgorithm[iThreadNo].m_HImageAIDefectRgn[iImageIdx], &pInspectService->m_pInspectAlgorithm[iThreadNo].m_HImageAIDefectRgn[iImageIdx]);
			difference(pInspectService->m_pInspectAlgorithm[iThreadNo].m_HImageAIDefectRgn[iImageIdx], HImageDefectRgn[iImageIdx], &pInspectService->m_pInspectAlgorithm[iThreadNo].m_HImageAIDefectRgn[iImageIdx]);		
		}

		if (THEAPP.m_pGFunction->ValidHRegion(HTotalDefectRgn) == FALSE)
			return ;

		union1(HTotalDefectRgn, &HTotalDefectRgn);
		connection(HTotalDefectRgn, &HTotalDefectRgn);

		count_obj(HTotalDefectRgn, &HNoDefect);
		lNoDefectCount = HNoDefect[0].I();

		sLog.Format("DoDeeplearningInspection Start: Defect Total %d", lNoDefectCount);
		THEAPP.SaveLog(sLog);

		////////// 블랍 별 이미지, 불량항목 확인
		for (int DefectIndex = 1; DefectIndex <= lNoDefectCount; DefectIndex++)
		{
			lMaxArea = 0;
			lMaxBlobArea = 0;
			lMaxAreaImageIndex = -1;
			iMaxDefectTypeIndex = -1;

			bApplyADJ = FALSE;
			select_obj(HTotalDefectRgn, &HSelectedRgn, DefectIndex);

			if (THEAPP.m_pGFunction->ValidHRegion(HSelectedRgn) == FALSE) {	// add
				sLog.Format("DoDeeplearningInspection: ValidHRegion(HSelectedRgn) FALSE");
				THEAPP.SaveLog(sLog);
				continue;
			}

			area_center(HSelectedRgn, &HlArea, &HdCenterY, &HdCenterX);

			// 공통 영역 블랍 중 가장 큰 블랍을 가진 이미지 확인
			for (int iImageIdx = 0; iImageIdx < MAX_IMAGE_TAB; iImageIdx++)
			{
				if (THEAPP.m_pGFunction->ValidHRegion(HImageDefectRgn[iImageIdx]) == FALSE)
					continue;

				connection(HImageDefectRgn[iImageIdx], &HTempRgn1);

				select_shape_proto(HTempRgn1, HSelectedRgn, &HTempRgn2, "overlaps_abs", 1, MAX_DEF);

				if (THEAPP.m_pGFunction->ValidHRegion(HTempRgn2) == FALSE)
					continue;

				union1(HTempRgn2, &HTempRgn2);

				area_center(HTempRgn2, &HlBlobArea, &HdBlobCenterY, &HdBlobCenterX);
				lBlobArea = HlBlobArea[0].L();

				if (lBlobArea > lMaxArea)
				{
					lMaxArea = lBlobArea;
					lMaxAreaImageIndex = iImageIdx;
				}
			}

			if (lMaxAreaImageIndex < 0) {
				sLog.Format("DoDeeplearningInspection: lMaxAreaImageIndex < 0, %d", lMaxAreaImageIndex);
				THEAPP.SaveLog(sLog);
				continue;
			}

			// 공통 영역 블랍 중 가장 큰 블랍을 가진 불량항목 확인
			for (int iDefectType = 0; iDefectType < MAX_DEFECT_NAME; iDefectType++)
			{
				if (THEAPP.m_pGFunction->ValidHRegion(pInspectService->m_pInspectAlgorithm[iThreadNo].m_HTypeAIDefectRgn[lMaxAreaImageIndex][iDefectType]) == FALSE)
					continue;

				intersection(HSelectedRgn, pInspectService->m_pInspectAlgorithm[iThreadNo].m_HTypeAIDefectRgn[lMaxAreaImageIndex][iDefectType], &HIntersectRgn);
				connection(pInspectService->m_pInspectAlgorithm[iThreadNo].m_HTypeAIDefectRgn[lMaxAreaImageIndex][iDefectType], &HConnRgn);

				select_shape_proto(HConnRgn, HIntersectRgn, &HDisplayRgn, "overlaps_abs", 1, MAX_DEF);

				if (THEAPP.m_pGFunction->ValidHRegion(HDisplayRgn) == FALSE)
					continue;

				union1(HDisplayRgn, &HDisplayRgn);

				area_center(HDisplayRgn, &HlBlobArea, &HdBlobCenterY, &HdBlobCenterX);
				lBlobArea = HlBlobArea[0].L();

				if (lBlobArea > lMaxBlobArea)
				{
					HMaxAreaBlobRgn = HDisplayRgn;	
					lMaxBlobArea = lBlobArea;

					iMaxDefectTypeIndex = iDefectType;
				}

			}

			if (iMaxDefectTypeIndex == -1) {	
				sLog.Format("DoDeeplearningInspection: iMaxDefectDefectTypeIndex == %d", iMaxDefectTypeIndex);
				THEAPP.SaveLog(sLog);
				continue;
			}

			bApplyADJ = TRUE; // 나중에 불량 검사 항목 확인 필요시 추가

			if (bApplyADJ)
			{
				gen_empty_obj(&HADJSaveImage);
				gen_empty_obj(&HADJSaveRegion);

				HADJSaveRegion = HMaxAreaBlobRgn;
				strDeepModelName = g_sDefectFileName[iMaxDefectTypeIndex];

				// 이미지 크롭
				area_center(HMaxAreaBlobRgn, &HlBlobArea, &HdBlobCenterY, &HdBlobCenterX);

				lLTPointY = (int)HdBlobCenterY[0].D() - THEAPP.Struct_PreferenceStruct.m_iAICropImageHeight / 2;
				lLTPointX = (int)HdBlobCenterX[0].D() - THEAPP.Struct_PreferenceStruct.m_iAICropImageWidth / 2;
				lRBPointY = (int)HdBlobCenterY[0].D() + THEAPP.Struct_PreferenceStruct.m_iAICropImageHeight / 2 - 1;
				lRBPointX = (int)HdBlobCenterX[0].D() + THEAPP.Struct_PreferenceStruct.m_iAICropImageWidth / 2 - 1;

				if (lLTPointX < 0)
				{
					lLTPointX = 0;
					lRBPointX = THEAPP.Struct_PreferenceStruct.m_iAICropImageWidth - 1;
				}

				if (lRBPointX >= (iCamImageWidth - 1))
				{
					lRBPointX = iCamImageWidth - 1;
					lLTPointX = iCamImageWidth - THEAPP.Struct_PreferenceStruct.m_iAICropImageWidth;
				}

				if (lLTPointY < 0)
				{
					lLTPointY = 0;
					lRBPointY = THEAPP.Struct_PreferenceStruct.m_iAICropImageHeight - 1;
				}

				if (lRBPointY >= (iCamImageHeight - 1))
				{
					lRBPointY = iCamImageHeight - 1;
					lLTPointY = iCamImageHeight - 1 - THEAPP.Struct_PreferenceStruct.m_iAICropImageHeight + 1;
				}

				crop_rectangle1(pInspectService->m_pInspectAlgorithm[iThreadNo].m_HAlgoInspectImage[lMaxAreaImageIndex], &HADJSaveImage, lLTPointY, lLTPointX, lRBPointY, lRBPointX);

				// 이미지 리사이징
				if (THEAPP.Struct_PreferenceStruct.m_bUseAIResizeImageSave)
				{
					int iWidth = THEAPP.Struct_PreferenceStruct.m_iAIResizeImageWidth;
					int iHeight = THEAPP.Struct_PreferenceStruct.m_iAIResizeImageHeight;

					if (iWidth > 0 && iHeight > 0)
						zoom_image_size(HADJSaveImage, &HADJSaveImage, iWidth, iHeight, "none");
				}


				if (m_nADJDefectCnt == 1000)
					m_nADJDefectCnt = 0;


				// Opencv 형식으로 변환
				BYTE* pbyteImgData = NULL;
				BYTE* pByteR;
				BYTE* pByteG;
				BYTE* pByteB;
				char cImageType[128];
				Hlong lImageWidth, lImageHeight;

				if (iImageChannelADJ == 1)
				{
					get_image_pointer1(HADJSaveImage, (Hlong*)&pByteR, cImageType, &lImageWidth, &lImageHeight);
					pbyteImgData = (BYTE*)malloc(sizeof(BYTE) * lImageWidth * lImageHeight);
				}
				else
				{
					get_image_pointer3(HADJSaveImage, (Hlong*)&pByteR, (Hlong*)&pByteG, (Hlong*)&pByteB, cImageType, &lImageWidth, &lImageHeight);
					pbyteImgData = (BYTE*)malloc(sizeof(BYTE) * lImageWidth * lImageHeight * 3);
				}

				if (pbyteImgData == NULL)
				{
					sLog.Format("DoDeeplearningInspection: pbyteImgData is Null");
					THEAPP.SaveLog(sLog);
					continue;
				}

				if (iImageChannelADJ == 1)
				{
					memcpy(pbyteImgData, (BYTE*)pByteR, sizeof(BYTE) * lImageWidth * lImageHeight);
				}
				else
				{
					pByteR = (BYTE*)pByteR;
					pByteG = (BYTE*)pByteG;
					pByteB = (BYTE*)pByteB;

					register BYTE* pR, * pG, * pB;

					for (int j = 0; j < lImageHeight; j++)
					{
						pB = pbyteImgData + j * lImageWidth * 3;
						pG = pbyteImgData + j * lImageWidth * 3 + 1;
						pR = pbyteImgData + j * lImageWidth * 3 + 2;

						for (int i = 0; i < lImageWidth; i++)
						{
							*(pB + i * 3) = *(pByteB)++;  //halcon에서 3채널 이미지 데이터 -> opencv형태(수아킷도 opencv형태)의 3채널 이미지 데이터로 변환
							*(pG + i * 3) = *(pByteG)++;
							*(pR + i * 3) = *(pByteR)++;
						}
					}
				}
				cv::Mat OpenCVADJImage, DeepLearningImage;
				if (iImageChannelADJ == 1)
					OpenCVADJImage.create(lImageHeight, lImageWidth, CV_8UC1);
				else
					OpenCVADJImage.create(lImageHeight, lImageWidth, CV_8UC3);

				std::memcpy(OpenCVADJImage.data, pbyteImgData, lImageWidth*lImageHeight*iImageChannelADJ);

				vector<uchar> jpgData;
				vector<int> compression_params;
				compression_params.push_back(cv::IMWRITE_JPEG_QUALITY);
				compression_params.push_back(100);

				imencode(".jpg", OpenCVADJImage, jpgData, compression_params);

				DeepLearningImage = cv::imdecode(jpgData, cv::IMREAD_COLOR);

				//검사 시작
				EnterCriticalSection(&THEAPP.m_csSendADJ);

				sendLog.Format("[ADJ] Send Data Lot ID:%s		Model ID:%s		Tray NO:%s		Module NO:%s		Defect Cnt:%d",
					strLotID,
					strDeepModelName,
					strTrayNo,
					strModuleNo,
					m_nADJDefectCnt);
				THEAPP.SaveLog(sendLog);

				DWORD startT;
				startT = GetTickCount();
				
				result = m_DeepLearningProcessing(string(CT2CA(strDeepModelName)), DeepLearningImage);	// 결과 보내기

				Sleep(THEAPP.Struct_PreferenceStruct.m_iADJDelayTime);

				m_nADJDefectCnt++;

				free(pbyteImgData);
				OpenCVADJImage.release();
				DeepLearningImage.release();

				LeaveCriticalSection(&THEAPP.m_csSendADJ);

				// 결과 저장 시작
				CString strImageType, strDefectType, strDefectName, strADJImageName;
				strImageType = g_sImageName[THEAPP.m_iMachineInspType][lMaxAreaImageIndex];
				strDefectType = g_sDefectFileName[iMaxDefectTypeIndex];
				strDefectName = g_sDefectName[iMaxDefectTypeIndex];

				CString strResult;
				if (result == 0)
				{
					if (THEAPP.m_pGFunction->ValidHRegion(HADJSaveRegion))
						concat_obj(HADJFilteredDefectRgn, HADJSaveRegion, &HADJFilteredDefectRgn);

					strResult = "OK";
					strADJImageName.Format("Module%s_%s_%s_%s_OK.jpg", strModuleNo, strImageType, sBarcodeID, strDefectType);
					THEAPP.m_pSaveManager->SaveImage(HADJSaveImage, "jpeg 100", THEAPP.m_FileBase.m_strADJOKRstPrev + "\\" + strADJImageName);

				}
				else if (result == 1)
				{
					// 25.02.12 - v3008 - Special NG 분리배출 추가 - LeeGW START
					if (pInspectService->m_pInspectAlgorithm[iThreadNo].m_bAISpecialNGSortFlag = TRUE)
						pInspectService->m_pInspectAlgorithm[iThreadNo].m_bSpecialNGSortFlag = TRUE;
					// 25.02.12 - v3008 - Special NG 분리배출 추가 - LeeGW END

					strResult = g_sDefectName[iMaxDefectTypeIndex];
					strADJImageName.Format("Module%s_%s_%s_%s_NG.jpg", strModuleNo, strImageType, sBarcodeID, strDefectType);
					THEAPP.m_pSaveManager->SaveImage(HADJSaveImage, "jpeg 100", THEAPP.m_FileBase.m_strADJNGRstPrev + "\\" + strADJImageName);
				}
				else
				{
					// 25.02.12 - v3008 - Special NG 분리배출 추가 - LeeGW START
					if (pInspectService->m_pInspectAlgorithm[iThreadNo].m_bAISpecialNGSortFlag = TRUE)
						pInspectService->m_pInspectAlgorithm[iThreadNo].m_bSpecialNGSortFlag = TRUE;
					// 25.02.12 - v3008 - Special NG 분리배출 추가 - LeeGW END

					strResult = g_sDefectName[iMaxDefectTypeIndex];
					strADJImageName.Format("Module%s_%s_%s_Code%d_ERROR.jpg", strModuleNo, strImageType, sBarcodeID, result);
					THEAPP.m_pSaveManager->SaveImage(HADJSaveImage, "jpeg 100", THEAPP.m_FileBase.m_strADJSkipRstPrev + "\\" + strADJImageName);
					bIsTimeOut = TRUE;
				}

				BOOL bDeepResultNG = (result != 0);

				CString sAddStr = "";

				THEAPP.SaveADJDaySummaryINI("ADJ_TOTAL_IMAGE", bDeepResultNG, bIsTimeOut);
				THEAPP.SaveADJDaySummaryINI(strDeepModel, bDeepResultNG, bIsTimeOut);
				THEAPP.SaveOnlyADJLotResultLog(THEAPP.Struct_PreferenceStruct.m_bSaveLasLog, strLotID, iTrayNo, sBarcodeID, iModuleNo, strDeepModelName, strDefectName, strResult, sAddStr);
			}	

			if (bIsTimeOut)
				break;		
		}

		if (THEAPP.Struct_PreferenceStruct.m_bIsApplyAISimulation == TRUE)
		{
			for (int iImageIdx = 0; iImageIdx < MAX_IMAGE_TAB; iImageIdx++)
			{
				for (int iDefectType = 0; iDefectType < MAX_DEFECT_NAME; iDefectType++)
				{
					if(g_sDefectFileName[iDefectType] == strDeepModelName)
						gen_empty_obj(&pInspectService->m_pInspectAlgorithm[iThreadNo].m_HTypeAIDefectRgn[iImageIdx][iDefectType]);
				}
			}
		}
		else if (bIsTimeOut == FALSE && THEAPP.Struct_PreferenceStruct.m_bIsApplyAIResult)		// ADJ 양품 판정
		{
			// AI 검사 결과 적용
			for (int iImageIdx = 0; iImageIdx < MAX_IMAGE_TAB; iImageIdx++)
			{
				for (int iDefectType = 0; iDefectType < MAX_DEFECT_NAME; iDefectType++)
				{
					if (THEAPP.m_pGFunction->ValidHRegion(pInspectService->m_pInspectAlgorithm[iThreadNo].m_HTypeAIDefectRgn[iImageIdx][iDefectType]))
					{
						if (THEAPP.m_pGFunction->ValidHRegion(HADJFilteredDefectRgn))
							difference(pInspectService->m_pInspectAlgorithm[iThreadNo].m_HTypeAIDefectRgn[iImageIdx][iDefectType], HADJFilteredDefectRgn, &pInspectService->m_pInspectAlgorithm[iThreadNo].m_HTypeAIDefectRgn[iImageIdx][iDefectType]);

						// 필터링 이후 불량 블랍 추가
						if (THEAPP.m_pGFunction->ValidHRegion(pInspectService->m_pInspectAlgorithm[iThreadNo].m_HTypeAIDefectRgn[iImageIdx][iDefectType]))
							concat_obj(pInspectService->m_pInspectAlgorithm[iThreadNo].m_HImageAIDefectRgn[iImageIdx], pInspectService->m_pInspectAlgorithm[iThreadNo].m_HTypeAIDefectRgn[iImageIdx][iDefectType], &pInspectService->m_pInspectAlgorithm[iThreadNo].m_HImageAIDefectRgn[iImageIdx]);
					}
				}

				if (THEAPP.m_pGFunction->ValidHRegion(pInspectService->m_pInspectAlgorithm[iThreadNo].m_HImageAIDefectRgn[iImageIdx]))
					union1(pInspectService->m_pInspectAlgorithm[iThreadNo].m_HImageAIDefectRgn[iImageIdx], &pInspectService->m_pInspectAlgorithm[iThreadNo].m_HImageAIDefectRgn[iImageIdx]);
			}
		}
		else // 비전 결과 적용
		{
			for (int iImageIdx = 0; iImageIdx < MAX_IMAGE_TAB; iImageIdx++)
			{
				for (int iDefectType = 0; iDefectType < MAX_DEFECT_NAME; iDefectType++)
				{
					if (THEAPP.m_pGFunction->ValidHRegion(pInspectService->m_pInspectAlgorithm[iThreadNo].m_HTypeAIDefectRgn[iImageIdx][iDefectType]))
					{
						concat_obj(pInspectService->m_pInspectAlgorithm[iThreadNo].m_HImageAIDefectRgn[iImageIdx], pInspectService->m_pInspectAlgorithm[iThreadNo].m_HTypeAIDefectRgn[iImageIdx][iDefectType], &pInspectService->m_pInspectAlgorithm[iThreadNo].m_HImageAIDefectRgn[iImageIdx]);
					}
				}

				if (THEAPP.m_pGFunction->ValidHRegion(pInspectService->m_pInspectAlgorithm[iThreadNo].m_HImageAIDefectRgn[iImageIdx]))
					union1(pInspectService->m_pInspectAlgorithm[iThreadNo].m_HImageAIDefectRgn[iImageIdx], &pInspectService->m_pInspectAlgorithm[iThreadNo].m_HImageAIDefectRgn[iImageIdx]);
			}
		}

	}
	catch (HException& except)
	{
		HTuple HExp;
		HTuple HOperatorName, HErrMsg;
		CString sOperatorName, sErrMsg;
		except.ToHTuple(&HExp);
		except.GetExceptionData(HExp, "operator", &HOperatorName);
		except.GetExceptionData(HExp, "error_message", &HErrMsg);
		sOperatorName = HOperatorName[0].S();
		sErrMsg = HErrMsg[0].S();

		CString str;
		str.Format("Halcon Exception [CADJClientService::DoDeeplearningInspection] : <%s>%s", sOperatorName, sErrMsg);
		THEAPP.SaveLog(str);
	}
}

void CADJClientService::Initialize()
{
	CString strADJ_IP;
	int     nADJ_Port;
	CString strClientIP = THEAPP.Struct_PreferenceStruct.m_strCurrentADJ_IP;

	if (m_arrClient.GetLogPath() == "")
	{
		m_arrClient.SetLogPath("./Data/TCPLog");
		m_arrClient.CreateDirForLog();
		m_arrClient.SetTotalByteTokenPos(8);
	}

	strADJ_IP = THEAPP.Struct_PreferenceStruct.m_strADJIPAddress;
	nADJ_Port = THEAPP.Struct_PreferenceStruct.m_iADJPortNo;

	if (!m_arrClient.TCPClientInit(strADJ_IP, nADJ_Port, strClientIP))
	{
		CString strErrorLog;
		strErrorLog.Format("[ ADJ ]TCP Initialize Error IP %s Port %d", strADJ_IP, nADJ_Port);
		THEAPP.SaveLog(strErrorLog);
	}

	//클라이언트 쪽 (자기 자신의 IP)
	char carrName[255];
	char* cpIp;
	PHOSTENT host;

	if (gethostname(carrName, sizeof(carrName)) == 0)
	{
		if ((host = gethostbyname(carrName)) != NULL)
		{
			cpIp = inet_ntoa(*(struct in_addr*)*host->h_addr_list);
			m_strClientIP = (const char*)cpIp;
		}
		else
		{
			THEAPP.SaveLog("[ADJ]gethostbyname fail.. please check network connection");
		}
	}
	else
	{
		THEAPP.SaveLog("[ADJ]gethostname fail.. please check network connection");
	}

	if (m_bTCPThreadRun == FALSE)
	{
		m_bTCPThreadRun = TRUE;
		AfxBeginThread(TCPPostProcessing_Client1, this);
	}
}


void CADJClientService::ReStart(void)
{

	HANDLE* hEvent = m_arrClient.GetTCPReceiveHanlde();
	SetEvent(hEvent[RESTART_EVENT]);
}

int CADJClientService::SendImagetoADJ(stTCPPacket stPacket, int nWaitMilsec, int iImageWidth, int iImageHeight)
{
	BYTE* pbytePacket = new BYTE[MAX_BUF_SIZE];
	//패킷 만들어 보냄.
	stPacket.MakeImagePacket(pbytePacket, iImageWidth, iImageHeight);

	//Sleep(3);

	m_arrClient.TCPSend(pbytePacket, stPacket.GetDataSize());

	int dwBytes;

	int nModuleTimeOut = 0;

	delete pbytePacket;
	pbytePacket = nullptr;

	nModuleTimeOut = THEAPP.Struct_PreferenceStruct.m_iADJDelayTime * 1000;

	int nSuaModelID = atoi((const char*)stPacket.byteSuaModelID) - 1;	//주의 : 수아 모델 ID 는 1~16 모듈넘버 1~42, 트레이넘버 1부터 시작
	int nModuleNo = atoi((const char*)stPacket.byteModuleNo) - 1;

	if (THEAPP.Struct_PreferenceStruct.m_bIsUseAIInsp)
	{
		while (TRUE)
		{
			Sleep(3);

			if (THEAPP.m_nADJResult[nSuaModelID][nModuleNo] >= 0)
			{
				int ADJ_Reuslt = THEAPP.m_nADJResult[nSuaModelID][nModuleNo];
				THEAPP.m_nADJResult[nSuaModelID][nModuleNo] = RCV_WAIT;
				return ADJ_Reuslt;
			}
			if (GetTickCount() - nWaitMilsec > nModuleTimeOut)
			{
				THEAPP.m_nADJResult[nSuaModelID][nModuleNo] = RCV_TIMEOUT;
				//AfxMessageBox( "Logical time out ");
				return RCV_TIMEOUT;
			}
		}
	}
	else
		return RCV_TIMEOUT;

}

void CADJClientService::ThreeHandShakingRequest()
{
	int nTokenPos = 1 + ((PACKET_STRING_SIZE + 1) * 11); //ADJ의 GetTotalByteTokenPos()값은 10
	CString strByteLength = "675"; //672는 nTokenPos 3은 672자체의 길이

	BYTE byteData[1000] = { 0, };
	CString str = "";
	CString strRep;
	strRep.Format("@WATCHDOG                                                    ,%d", THEAPP.Struct_PreferenceStruct.m_iADJPortNo);
	memcpy(byteData, (const char*)strRep, strRep.GetLength() + 1);
	memcpy(&byteData[nTokenPos], (const char*)strByteLength, 3);

	m_arrClient.TCPSend(byteData, atoi(strByteLength));
}

void CADJClientService::ADJResetRequest()
{
	int nTokenPos = 1 + ((PACKET_STRING_SIZE + 1) * 11); //ADJ의 GetTotalByteTokenPos()값은 10
	CString strByteLength = "675"; //661은 nTokenPos 3은 611자체의 길이

	BYTE byteData[1000] = { 0, };
	CString str = "";
	CString strRep;
	strRep.Format("@ADJRESET                                                    ,%d", THEAPP.Struct_PreferenceStruct.m_iADJPortNo);

	memcpy(byteData, (const char*)strRep, strRep.GetLength() + 1);
	memcpy(&byteData[nTokenPos], (const char*)strByteLength, 3);

	m_arrClient.TCPSend(byteData, atoi(strByteLength));
}

static UINT TCPPostProcessing_Client1(LPVOID lParam)
{
	DWORD ret;

	CADJClientService* m_ClientService = (CADJClientService*)lParam;

	HANDLE* hRecvHandle = m_ClientService->m_arrClient.GetTCPReceiveHanlde();

	int nTryCnt = 0;

	while (TRUE)
	{
		ret = WaitForMultipleObjects(TCP_EVENT_NUM, hRecvHandle, FALSE, NET_EVENT_WAIT_TIME);

		if (ret == WAIT_FAILED)
			return 0;
		else if (ret == WAIT_ABANDONED)
		{
			continue;
		}
		else if (ret == WAIT_TIMEOUT)
		{
			continue;
		}
		else if (ret == WAIT_OBJECT_0 + CONNECT_EVENT)												//Connect Event
		{
			//TO DO: Accept
			THEAPP.SaveLog("[ADJ]Connect Success");

			m_ClientService->m_bConnect = TRUE;
			m_ClientService->m_arrClient.ResetTCPReceiveHandle(CONNECT_EVENT);		//ASSERT : To reset manual event
		}
		else if (ret == WAIT_OBJECT_0 + RECV_EVENT)													//Receive Event
		{
			m_ClientService->m_arrClient.ResetTCPReceiveHandle(RECV_EVENT);							//ASSERT : To reset manual event
			//AfxBeginThread( GetTCPReceiveData, (LPVOID)&m_ClientService->m_arrClient[ nClientNo ] );		//20191028 commented

			int nSuaModel = 0;

			CString tcpLog;

			BYTE* m_pbyteBuf = nullptr;

			while (!m_ClientService->m_arrClient.m_qRecvBuffer.IsEmpty())
			{
				m_pbyteBuf = m_ClientService->m_arrClient.m_qRecvBuffer.next();

				stTCPPacket m_pstTCPPacket;
				int nStatus = m_pstTCPPacket.GetPacket(m_pbyteBuf);

				if (nStatus == RECV_WATCHDOG)
				{
					THEAPP.SaveLog("[TCP] 3-hand-shaking success");
					m_ClientService->m_bRealConnect = TRUE;
					continue;
				}

				CString strIP = (const char*)m_pstTCPPacket.byteIP;
				int nSuaModel = atoi((const char*)m_pstTCPPacket.byteSuaModelID);
				int nModuleNO = atoi((const char*)m_pstTCPPacket.byteModuleNo);
				int nTrayNO = atoi((const char*)m_pstTCPPacket.byteTrayNo);
				int nSuaResult = atoi((const char*)m_pstTCPPacket.byteSuaResult);

				if (nSuaModel > 0)
				{
					if (THEAPP.m_nADJResult[nSuaModel - 1][nModuleNO - 1] == RCV_WAIT)
					{
						THEAPP.m_nADJResult[nSuaModel - 1][nModuleNO - 1] = nSuaResult;
						tcpLog.Format("[From ADJ] Rceived Data : Lot ID : %s\tTray NO:%d\tModule NO:%d\tSuaModel:%d\tResult:%d",
							(const char*)m_pstTCPPacket.byteLotID,
							nTrayNO, nModuleNO,
							nSuaModel,
							nSuaResult);
					}
					else
					{
						THEAPP.m_nADJResult[nSuaModel - 1][nModuleNO - 1] = RCV_WAIT;
						tcpLog.Format("[From ADJ] Time out Data is receveid Lot ID:%s\tTray NO:%d\tModule NO:%d\tSuaModel:%d\tResult:%d",
							(const char*)m_pstTCPPacket.byteLotID,
							nTrayNO, nModuleNO,
							nSuaModel,
							nSuaResult);
					}
				}
				else
				{
					THEAPP.m_nADJResult[nSuaModel - 1][nModuleNO - 1] = RCV_WAIT;
					tcpLog.Format("[Logical Error Sua Model missmatching From ADJ] Data : Lot ID:%s\tTray NO:%d\tModule NO:%d\tSuaModel:%d\tResult:%d",
						(const char*)m_pstTCPPacket.byteLotID,
						nTrayNO, nModuleNO,
						nSuaModel,
						nSuaResult);
				}

				THEAPP.SaveLog(tcpLog);
			}
		}
		else if (ret == WAIT_OBJECT_0 + SERVER_CLOSE_EVENT)											//Close Event
		{
			//TO DO: Clsoe Event Process
			m_ClientService->m_bConnect = m_ClientService->m_arrClient.GetTcpConnect();

			if (m_ClientService->m_bConnect)
			{
				THEAPP.m_ADJClientService.m_bConnect = TRUE;
			}
			else
			{
				THEAPP.m_ADJClientService.m_bConnect = FALSE;
			}
			m_ClientService->m_arrClient.ResetTCPReceiveHandle(SERVER_CLOSE_EVENT);	//ASSERT : To reset manual event
		}
		else if (ret == WAIT_OBJECT_0 + RESTART_EVENT)
		{
			m_ClientService->m_bTCPThreadRun = FALSE;
			m_ClientService->m_arrClient.ResetTCPReceiveHandle(RESTART_EVENT);	//ASSERT : To reset manual event
			return 0;
		}
	}

	return 1;
}


CString CADJClientService::GetStringCamType(int iVisionCamType)
{
	CString strVisionCamType = "";

	switch (iVisionCamType)
	{
	case VISION_CAM_1:
		strVisionCamType.Format("T1");
		break;
	case VISION_CAM_2:
		strVisionCamType.Format("T2");
		break;
	case VISION_CAM_3:
		strVisionCamType.Format("T3");
		break;
	case VISION_CAM_4:
		strVisionCamType.Format("T4");
		break;
	default:
		strVisionCamType.Format("UNKNWON");
		break;
	}

	return strVisionCamType;
}

void CADJClientService::ADJConnectAliveCheck()
{
	int nTryCnt = 0;
	BOOL bReset = FALSE;

	THEAPP.m_ADJClientService.m_bRealConnect = FALSE;

	if (THEAPP.m_ADJClientService.m_bConnect)
	{
		for (int nTryCnt = 0; nTryCnt < 3; nTryCnt++)
		{
			THEAPP.m_ADJClientService.ThreeHandShakingRequest();

			Sleep(200);

			if (THEAPP.m_ADJClientService.m_bConnect == FALSE)
				break;

			if (THEAPP.m_ADJClientService.m_bRealConnect == TRUE)
			{
				break;
			}
		}
	}

	if (THEAPP.m_ADJClientService.m_bConnect && !THEAPP.m_ADJClientService.m_bRealConnect)
	{
		CString strLog;
		strLog.Format("[TCP] hand-shake Faile Close Try");
		THEAPP.SaveLog(strLog);

		Sleep(100);
		THEAPP.m_ADJClientService.m_arrClient.TCPonClose();
		THEAPP.m_ADJClientService.m_bConnect = FALSE;

		strLog.Format("[TCP] hand-shake Faile Close Success");
		THEAPP.SaveLog(strLog);

		bReset = TRUE;
	}

	if (bReset)
	{
		THEAPP.SaveLog("[TCP] Reset On : reset try");
		THEAPP.m_ADJClientService.ReStart();
		THEAPP.SaveLog("[TCP] Reset On : reset Success Initialize Try");
		THEAPP.m_ADJClientService.Initialize();									//통신쪽 초기화.
		THEAPP.SaveLog("[TCP] Reset On : reset Success Initialize Success");
	}
}

void CADJClientService::ResetADJResult(int iSuaModelNO, int iModuleNo)
{
	THEAPP.m_nADJResult[iSuaModelNO - 1][iModuleNo - 1] = RCV_WAIT;
}

int CADJClientService::m_DeepLearningProcessing(string modelname, cv::Mat DLImg)
{
	CString cstrLog, log;
	bool camEnable = false;
	bool enable_logits = false;
	bool enable_features = false;
	float focal_weight = 0.0;
	int batch_size = 1;
	int judge, judge_list=1;
	float score;

	BOOL bIsADJRuning = TRUE;

	// ADJ 실행 확인 - LeeGW
	if (THEAPP.Struct_PreferenceStruct.m_bIsUseADJRunCheck)
	{
		bIsADJRuning = IsADJRunning();
		if (!bIsADJRuning)
		{
			bIsADJRuning = ExecuteADJ();

			Sleep(5000);
		}
	}

	if (bIsADJRuning)
	{
		// 판정 실행
		//KDH

		WSADATA wsaData;
		SOCKET ConnectSocket = INVALID_SOCKET;
		struct addrinfo* result = NULL, * ptr = NULL, hints;

		cv::Mat image = DLImg;
		string defectName = modelname;

		// Winsock 초기화
		WSAStartup(MAKEWORD(2, 2), &wsaData);

		ZeroMemory(&hints, sizeof(hints));
		hints.ai_family = AF_UNSPEC;
		hints.ai_socktype = SOCK_STREAM;
		hints.ai_protocol = IPPROTO_TCP;

		// 서버 주소와 포트 연결
		getaddrinfo(SERVER_IP, DEFAULT_PORT, &hints, &result);

		// 연결 시도
		for (ptr = result; ptr != NULL; ptr = ptr->ai_next) {
			ConnectSocket = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);
			if (ConnectSocket == INVALID_SOCKET) continue;
			if (connect(ConnectSocket, ptr->ai_addr, (int)ptr->ai_addrlen) == SOCKET_ERROR) {
				closesocket(ConnectSocket);
				ConnectSocket = INVALID_SOCKET;
				continue;
			}
			break;
		}

		freeaddrinfo(result);

		if (ConnectSocket == INVALID_SOCKET) {
			std::cerr << "Unable to connect to server!" << std::endl;
			WSACleanup();
		}

		// 이미지와 불량명 전송
		SendImageAndDefectName(ConnectSocket, image, defectName);

		// 검사 결과 수신
		string inspectionResult = ReceiveInspectionResult(ConnectSocket);
		cout << "Inspection Result: " << inspectionResult << std::endl;

		istringstream iss(inspectionResult); // 입력 문자열로부터 스트림 생성

		string letter; // 문자를 저장할 변수
		float number;

		// 스트림에서 문자열과 실수를 차례대로 읽어들임
		iss >> letter >> number;

		// 결과 출력
		cout << "Letter: " << letter << std::endl;
		cout << "Number: " << number << std::endl;

		// 정리
		closesocket(ConnectSocket);
		WSACleanup();

		//KDH

		if (letter == "G")
		{
			judge_list = 0;
			score = number;
		}
		else
		{
			judge_list = 1;
			score = number;
		}
	}	
	else // if (bIsADJRuning)
	{
		judge_list = -99;
		score = 0;
	}


	return judge_list;
}

// ADJ 실행 확인 - LeeGW 
BOOL CADJClientService::IsADJRunning() {
	HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (hSnapshot == INVALID_HANDLE_VALUE) {
		return FALSE;
	}

	PROCESSENTRY32 processEntry;
	processEntry.dwSize = sizeof(PROCESSENTRY32);

	if (Process32First(hSnapshot, &processEntry)) {
		do {
			TCHAR exePath[MAX_PATH];
			HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, processEntry.th32ProcessID);
			if (hProcess)
			{
				if (GetModuleFileNameEx(hProcess, NULL, exePath, MAX_PATH))
				{
					if (CString(exePath).CompareNoCase(THEAPP.Struct_PreferenceStruct.m_strADJExePath) == 0)
					{
						CloseHandle(hProcess);
						CloseHandle(hSnapshot);
						return TRUE;
					}
				}
				CloseHandle(hProcess);
			}
		} while (Process32Next(hSnapshot, &processEntry));
	}

	CloseHandle(hSnapshot);
	return FALSE;
}

BOOL CADJClientService::ExecuteADJ()
{
	CString exePath = THEAPP.Struct_PreferenceStruct.m_strADJExePath;
	CString exeDirectory = exePath.Left(exePath.ReverseFind('\\'));

	if (exePath.IsEmpty())
		return FALSE;

	STARTUPINFO si = { sizeof(STARTUPINFO) };
	PROCESS_INFORMATION pi = { 0 };

	//si.dwFlags = STARTF_USESHOWWINDOW;
	//si.wShowWindow = SW_SHOWNORMAL;

	char * commandLine = (LPSTR)(LPCTSTR)exePath;

	BOOL result = CreateProcess(NULL, commandLine, NULL, NULL, FALSE, 0, NULL, exeDirectory, &si, &pi);

	if (result) {
		CloseHandle(pi.hProcess);
		CloseHandle(pi.hThread);
		return TRUE;
	}
	else {
		return FALSE;
	}
}