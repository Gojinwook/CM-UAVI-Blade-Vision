#pragma once

#include "stdafx.h"
#include "SuaKit.h"
#include "SuaKitClass.h"
#include "uScan.h"
#include <random>
#include <ctime>
#include <fstream>
#include <math.h>

const int nWidthHeight = 760;

CSuaKit::CSuaKit(void)
{
#ifdef USE_SUAKIT
	m_bIsIntialized = FALSE;
#endif
}

CSuaKit::~CSuaKit(void)
{
}
#ifdef USE_SUAKIT

BOOL CSuaKit::Initialize()
{
#ifdef USE_SUAKIT
	CString strSuaFailLog;

	GetSuaParamFromINI();

	DeviceDescriptor device = DeviceDescriptor::GetDefaultDevice();

	m_iResult = 0;

	for ( int i = 0; i < MAX_CLASSIFICATION_NUM; i++ )
	{

		CString cstrPath = "./deeplearningmodels/" + m_strDeepLearningModelName[ i ];
		wstring strPath = CStringW( cstrPath );

		switch ( i )
		{
		case SURFACETYPE :
			{
				m_clsSurface = new ClassificationEvaluator(strPath.c_str(),
					device,
					nWidthHeight,
					nWidthHeight, 
					1,
					1,
					(SuaKIT::API::SINGLE),
					(SuaKIT_UInt64)1,
					true
					);
				Status status = m_clsSurface->GetStatus();
				if (status != Status::SUCCESS)
				{
					strSuaFailLog.Format("SuaKIT Initialize Fail Model Path : %s",cstrPath);
					THEAPP.SaveLog(strSuaFailLog);
					m_bIsIntialized = FALSE;
					SAFE_DELETE(m_clsSurface);
					return FALSE;
				}
				break;
			}
		case EDGETYPE :
			{
				m_clsEdge = new ClassificationEvaluator(strPath.c_str(),
					device,
					nWidthHeight,
					nWidthHeight, 
					1,
					1,
					(SuaKIT::API::SINGLE),
					(SuaKIT_UInt64)1,
					true
					);
				Status status = m_clsEdge->GetStatus();
				if (status != Status::SUCCESS)
				{
					strSuaFailLog.Format("SuaKIT Initialize Fail Model Path : %s",cstrPath);
					THEAPP.SaveLog(strSuaFailLog);
					m_bIsIntialized = FALSE;
					SAFE_DELETE(m_clsEdge);
					return FALSE;
				}

				break;
			}
		case LENS1TYPE :
			{
				m_clsLens1 = new ClassificationEvaluator(strPath.c_str(),
					device,
					nWidthHeight,
					nWidthHeight, 
					1,
					1,
					(SuaKIT::API::SINGLE),
					(SuaKIT_UInt64)1,
					true
					);
				Status status = m_clsLens1->GetStatus();
				if (status != Status::SUCCESS)
				{
					strSuaFailLog.Format("SuaKIT Initialize Fail Model Path : %s",cstrPath);
					THEAPP.SaveLog(strSuaFailLog);
					m_bIsIntialized = FALSE;
					SAFE_DELETE(m_clsLens1);
					return FALSE;
				}

				break;
			}
		case LENS2TYPE :
			{
				m_clsLens2 = new ClassificationEvaluator(strPath.c_str(),
					device,
					nWidthHeight,
					nWidthHeight, 
					1,
					1,
					(SuaKIT::API::SINGLE),
					(SuaKIT_UInt64)1,
					true
					);
				Status status = m_clsLens2->GetStatus();
				if (status != Status::SUCCESS)
				{
					strSuaFailLog.Format("SuaKIT Initialize Fail Model Path : %s",cstrPath);
					THEAPP.SaveLog(strSuaFailLog);
					m_bIsIntialized = FALSE;
					SAFE_DELETE(m_clsLens2);
					return FALSE;
				}

				break;
			}

		}

	}
#else
	THEAPP.SaveLog("No Use SuaKIT ;");
	m_bIsIntialized = FALSE;
	return FALSE;
#endif

	THEAPP.SaveLog("SuaKIT Initialize Success;");
	m_bIsIntialized = TRUE;
	return TRUE;
}

BOOL CSuaKit::UnInitialize()
{
#ifdef USE_SUAKIT
	THEAPP.SaveLog( "Suakit UnInitialize");
	
	m_clsSurface->Finalize();
	SAFE_DELETE(m_clsSurface);
	
	m_clsEdge->Finalize();
	SAFE_DELETE(m_clsEdge);
	
	m_clsLens1->Finalize();
	SAFE_DELETE(m_clsLens1);

	m_clsLens2->Finalize();
	SAFE_DELETE(m_clsLens2);


#endif
	m_bIsIntialized = FALSE;
	return TRUE;
}

void CSuaKit::ResetResult()
{
	m_iResult = 0;
}

BOOL CSuaKit::ModuleClassification(unsigned char* pucImageData, int nType)
{
#ifdef USE_SUAKIT
	ImageData ImgData(pucImageData, nWidthHeight, nWidthHeight, nWidthHeight, 1);
	
	FloatArray resultProbMapArray;
	
	SuaKIT::API::Status suaStatus;
	
	switch ( nType )
	{
	case SURFACETYPE :
		{
			suaStatus = m_clsSurface->Evaluate(ImgData, m_iResult, resultProbMapArray);
			break;
		}
	case EDGETYPE :
		{
			suaStatus = m_clsEdge->Evaluate(ImgData, m_iResult, resultProbMapArray);
			break;
		}
	
	case LENS1TYPE :
		{
			suaStatus = m_clsLens1->Evaluate(ImgData, m_iResult, resultProbMapArray);
			break;
		}

	case LENS2TYPE :
		{
			suaStatus = m_clsLens2->Evaluate(ImgData, m_iResult, resultProbMapArray);
			break;
		}
	}

	//수아랩 김정훈씨가 0이 Good 1이 NG라고 해서 ini로 설정할수 있게 했음.
	//또한 시냅스 설비는 1이 양품 0이 불량
	m_iResult = ( m_iResult == m_nSuaResultOK) ? 1 : 0;
			
	if(suaStatus != SuaKIT::API::Status::SUCCESS)
		m_iResult = 0;
#endif
	return TRUE;
}

int CSuaKit::GetResult()
{
	return (int)m_iResult;
}
#endif

#ifdef USE_SUAKIT
void CSuaKit::GetSuaParamFromINI()
{

	CString strRegisterFolder = THEAPP.GetWorkingDirectory()+"\\Data\\";
	CIniFileCS INI(strRegisterFolder + "\\DeepLearning.ini");

	/*m_nNetworkHeight = INI.Get_Integer("DeepLearningModelInfo", "Height", 440 );
	m_nNetworkWidth  = INI.Get_Integer("DeepLearningModelInfo", "Width", 440 );
	m_nNetworkChannel= INI.Get_Integer("DeepLearningModelInfo", "Channel", 1 );
	m_nMultiImageNo  = INI.Get_Integer("DeepLearningModelInfo", "MultiImageNumber", 4 );*/
	m_nSuaResultOK   = INI.Get_Integer("Result Class Number", "OK", 0 );
	//m_nBatchSize     = INI.Get_Integer("DeepLearningModelInfo", "BatchSize", 1 );

	m_strDeepLearningModelName[ SURFACETYPE ] = INI.Get_String("Model File Name", "Surface", "Surface.net" );
	m_strDeepLearningModelName[ EDGETYPE ]    = INI.Get_String("Model File Name", "Edge", "Edge.net" );
	m_strDeepLearningModelName[ LENS1TYPE ]   = INI.Get_String("Model File Name", "Lens1", "Lens1.net" );
	m_strDeepLearningModelName[ LENS2TYPE ]   = INI.Get_String("Model File Name", "Lens2", "Lens2.net" );

	m_iCropImageSizeWidth   = INI.Get_Integer("Image Size", "Width", 760);
	m_iCropImageSizeHeight   = INI.Get_Integer("Image Size", "Height", 760);
}
#endif