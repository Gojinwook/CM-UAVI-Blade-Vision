#pragma once

enum ModelType {SURFACETYPE ,EDGETYPE ,LENS1TYPE ,LENS2TYPE};

#define MULTI_CLASSIFICATION 0
#define MAX_CLASSIFICATION_NUM 4

class CSuaKit
{
public:
	CSuaKit(void);
	virtual ~CSuaKit(void);
	BOOL Initialize();
	BOOL UnInitialize();
	void GetSuaParamFromINI();
	void ResetResult();

#ifdef USE_SUAKIT
	BOOL ModuleClassification(SuaKIT::API::ImageData imgData, int nType);
#endif

	int GetResult();
	BOOL GetInitialized() { return m_bIsIntialized; }
	BOOL GetIsMultiClassification() { return m_bIsMultiClassification; }

	BOOL m_bIsChangeResult;

	int m_iCropImageSizeWidth;
	int m_iCropImageSizeHeight;

	int m_iCropImageMargin;

	BOOL GetIsJPGInspection() { return m_bIsJPGInspection; }
	BOOL GetUseCropImage() { return m_bUseCropImage; }	//WCS 2019/07/19


	float GetProb1(){ return m_fProb1; }
	float GetProb2(){ return m_fProb2; }

private:
	BOOL m_bIsIntialized;

	
	float m_fProb1;
	float m_fProb2;

	//suakit param
	CString m_strDeepLearningModelName[ MAX_CLASSIFICATION_NUM ];
	int m_nSuaResultOK;
	BOOL m_bIsMultiClassification;
	BOOL m_bIsJPGInspection;
	BOOL m_bUseCropImage;

	/*int m_nNetworkHeight;
	int m_nNetworkWidth;
	int m_nNetworkChannel;
	int m_nMultiImageNo;*/
	//int m_nBatchSize;
//#endif

protected:

#ifdef USE_SUAKIT

	ClassificationEvaluator *m_clsSurface;
	ClassificationEvaluator *m_clsEdge;
	ClassificationEvaluator *m_clsLens1;
	ClassificationEvaluator *m_clsLens2;

private:

	SuaKIT_Int64 m_iResult;

#endif
};
