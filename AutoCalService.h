#pragma once

#include "Algorithm.h"


class CAutoCalService
{
public:
	CAutoCalService(void);
	~CAutoCalService(void);

	static CAutoCalService *m_pInstance;
public:
	static CAutoCalService *GetInstance();
	void DeleteInstance();

	Algorithm *m_pInspectAlgorithm;

	int m_iTeachingStep;

	bool m_bAutoCal_Done[MAX_LIGHT_CHANNEL];

	int m_iTeachCH_LV[MAX_LIGHT_CHANNEL];
	int m_iTeachCH_GV[MAX_LIGHT_CHANNEL];
	int m_iInspectCH_LV[MAX_LIGHT_CHANNEL];
	int m_iInspectCH_GV[MAX_LIGHT_CHANNEL];
	bool m_bJudgeCH[MAX_LIGHT_CHANNEL];

	int m_iTeachTOTAL_LV;
	int m_iTeachTOTAL_GV;
	int m_iInspectTOTAL_LV;
	int m_iInspectTOTAL_GV;
	bool m_bJudgeTOTAL;

	bool m_bLightAutoCalResult;
	int m_iLightAutoCalProgressPercent;

	void InitAutoCalResult();
	void AutoCalLightStart(int iTeachingStep);
};

