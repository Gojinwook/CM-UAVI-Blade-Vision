// LightControl.h: interface for the CLightControl class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_LIGHTCONTROL_H__4BD05FCC_6179_432D_BBDA_FEF22AC1420B__INCLUDED_)
#define AFX_LIGHTCONTROL_H__4BD05FCC_6179_432D_BBDA_FEF22AC1420B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ComThread.h"

#define COM_PORT_1								0
#define COM_PORT_2								1
#define COM_PORT_3								2
#define COM_PORT_4								3
#define COM_PORT_5								4
#define COM_PORT_6								5
#define COM_PORT_7								6
#define COM_PORT_8								7
#define COM_PORT_9								8
#define COM_PORT_10								9	

#define STROBE_CHANNEL_1						0
#define STROBE_CHANNEL_2						1
#define STROBE_CHANNEL_3						2
#define STROBE_CHANNEL_4						3

#define LIGHTCTRL_PAGE_COUNT	6



struct SPage
{
	unsigned int uiChannel1;
	unsigned int uiChannel2;
	unsigned int uiChannel3;
	unsigned int uiChannel4;
	unsigned int uiChannel5;
	unsigned int uiChannel6;
};


class CLightControl  
{
public:
	CLightControl();
	virtual ~CLightControl();

	void SetIllumination( unsigned int uiPageIndex = 0 );
	void WriteIllumination( unsigned int uiPageIndex = 0 );
	void ReadIllumination( unsigned int uiPageIndex = 0 );

	void SetIllumination_6CH(unsigned int uiPageIndex = 0);

	SPage m_Page[LIGHTCTRL_PAGE_COUNT];
	CCommThread m_ComPort;

protected:



};

#endif // !defined(AFX_LIGHTCONTROL_H__4BD05FCC_6179_432D_BBDA_FEF22AC1420B__INCLUDED_)
