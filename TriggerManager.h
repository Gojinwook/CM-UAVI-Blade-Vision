#pragma once

#include "AJinAXL.h"

class CTriggerManager
{
public:

	static CTriggerManager	*m_pInstance;
	static	CTriggerManager* GetInstance(BOOL bShowFlag=FALSE);
	void	DeleteInstance();
	CTriggerManager(void);
	~CTriggerManager(void);

	BOOL Initialize();
	void SetTriggerSleepTime(int iMsec) { m_iSleepTime = iMsec; }

	void FireTrigger(int iCamIdx, int iTriggerPageIndex);

	void Delay(int msec);

protected:
	CAJinAXL m_AJinAXL;
	int		m_iSleepTime;
};

