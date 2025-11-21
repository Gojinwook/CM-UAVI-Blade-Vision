// LightControl.cpp: implementation of the CLightControl class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "uScan.h"
#include "LightControl.h"

#ifdef _DEBUG
#undef THIS_FILE
static char* THIS_FILE=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CLightControl::CLightControl()
{
	for ( int i = 0; i < LIGHTCTRL_PAGE_COUNT; i++ )
	{
		m_Page[i].uiChannel1 = 0;
		m_Page[i].uiChannel2 = 0;
		m_Page[i].uiChannel3 = 0;
		m_Page[i].uiChannel4 = 0;
	}
}

CLightControl::~CLightControl()
{

}


void CLightControl::SetIllumination( unsigned int uiPageIndex )	
{
	//조명 밝기를 변경한다. iIllum은 0 ~ 255	
	CString SendData;
	BYTE BuffLow, BuffHigh;
	BYTE BufData[15];
	SPage& Page = m_Page[uiPageIndex];
	
	if(Page.uiChannel1 < 0) 
		Page.uiChannel1 = 0;
	if(Page.uiChannel2 < 0) 
		Page.uiChannel2 = 0;
	if(Page.uiChannel3 < 0) 
		Page.uiChannel3 = 0;
	if(Page.uiChannel4 < 0) 
		Page.uiChannel4 = 0;
	
	
	// Start
	BufData[0] = 0xEF;
	BufData[1] = 0xEF;

	// Command
	BufData[2] = 0x00; 

	// Page 
	BufData[3] = uiPageIndex & 0xFF;
	
	// Light Brightness Data
	//Channel1 
	BufData[4] = (BYTE)( (Page.uiChannel1 & 0xFF00) >> 8 );
	BufData[5] = (BYTE)Page.uiChannel1 & 0xFF;
	
	// Channel2 
	BufData[6] = (BYTE)( (Page.uiChannel2 & 0xFF00) >> 8 );
	BufData[7] = (BYTE)Page.uiChannel2 & 0xFF;
	
	// Channel3
	BufData[8] = (BYTE)( (Page.uiChannel3 & 0xFF00) >> 8 );
	BufData[9] = (BYTE)Page.uiChannel3 & 0xFF;
	
	// Channel4
	BufData[10] = (BYTE)( (Page.uiChannel4 & 0xFF00) >> 8 );
	BufData[11] = (BYTE)Page.uiChannel4 & 0xFF;
	
	// End
	BufData[12] = 0x00;
	BufData[13] = 0xEE;
	BufData[14] = 0xEE;
	
	m_ComPort.WriteComm(BufData, 15 );			
	Sleep( 100 );

}

void CLightControl::WriteIllumination( unsigned int uiPageIndex )
{
	//조명 밝기를 변경한다. iIllum은 0 ~ 255	
	CString SendData;
	BYTE BuffLow, BuffHigh;
	BYTE BufData[15];
	SPage& Page = m_Page[uiPageIndex];
	
	if(Page.uiChannel1 < 0) Page.uiChannel1 = 0;
	if(Page.uiChannel2 < 0) Page.uiChannel2 = 0;
	if(Page.uiChannel3 < 0) Page.uiChannel3 = 0;
	if(Page.uiChannel4 < 0) Page.uiChannel4 = 0;
	
	
	// Start
	BufData[0] = 0xEF;
	BufData[1] = 0xEF;

	// Command
	BufData[2] = 0x57; 

	// Page
	BufData[3] = uiPageIndex & 0xFF;
	
	// Light Brightness Data
	//Channel1 
	BufData[4] = (BYTE)( (Page.uiChannel1 & 0xFF00) >> 8 );
	BufData[5] = (BYTE)Page.uiChannel1 & 0xFF;
	
	// Channel2 
	BufData[6] = (BYTE)( (Page.uiChannel2 & 0xFF00) >> 8 );
	BufData[7] = (BYTE)Page.uiChannel2 & 0xFF;
	
	// Channel3
	BufData[8] = (BYTE)( (Page.uiChannel3 & 0xFF00) >> 8 );
	BufData[9] = (BYTE)Page.uiChannel3 & 0xFF;
	
	// Channel4
	BufData[10] = (BYTE)( (Page.uiChannel4 & 0xFF00) >> 8 );
	BufData[11] = (BYTE)Page.uiChannel4 & 0xFF;
	
	// End
	BufData[12] = 0x00;
	BufData[13] = 0xEE;
	BufData[14] = 0xEE;
	
	m_ComPort.WriteComm(BufData, 15 );		
}


void CLightControl::ReadIllumination( unsigned int uiPageIndex )
{
	//조명 밝기를 변경한다. iIllum은 0 ~ 255	
	CString SendData;
	BYTE BuffLow, BuffHigh;
	BYTE BufData[15];
	SPage& Page = m_Page[uiPageIndex];
	
	if(Page.uiChannel1 < 0) Page.uiChannel1 = 0;
	if(Page.uiChannel2 < 0) Page.uiChannel2 = 0;
	if(Page.uiChannel3 < 0) Page.uiChannel3 = 0;
	if(Page.uiChannel4 < 0) Page.uiChannel4 = 0;
	
	
	// Start
	BufData[0] = 0xEF;
	BufData[1] = 0xEF;
	
	// Command
	BufData[2] = 0x53; 
	
	// Page	
	BufData[3] = uiPageIndex & 0xFF;
	
	// Light Brightness Data
	//Channel1 
	BufData[4] = (BYTE)( (Page.uiChannel1 & 0xFF00) >> 8 );
	BufData[5] = (BYTE)Page.uiChannel1 & 0xFF;
	
	// Channel2 
	BufData[6] = (BYTE)( (Page.uiChannel2 & 0xFF00) >> 8 );
	BufData[7] = (BYTE)Page.uiChannel2 & 0xFF;
	
	// Channel3
	BufData[8] = (BYTE)( (Page.uiChannel3 & 0xFF00) >> 8 );
	BufData[9] = (BYTE)Page.uiChannel3 & 0xFF;
	
	// Channel4
	BufData[10] = (BYTE)( (Page.uiChannel4 & 0xFF00) >> 8 );
	BufData[11] = (BYTE)Page.uiChannel4 & 0xFF;
	
	// End
	BufData[12] = 0x00;
	BufData[13] = 0xEE;
	BufData[14] = 0xEE;
	
	m_ComPort.WriteComm(BufData, 15 );			
}

void CLightControl::SetIllumination_6CH(unsigned int uiPageIndex)
{
	//조명 밝기를 변경한다. iIllum은 0 ~ 255	
	BYTE BufData[19];
	SPage& Page = m_Page[uiPageIndex];

	if (Page.uiChannel1 < 0)
		Page.uiChannel1 = 0;
	if (Page.uiChannel2 < 0)
		Page.uiChannel2 = 0;
	if (Page.uiChannel3 < 0)
		Page.uiChannel3 = 0;
	if (Page.uiChannel4 < 0)
		Page.uiChannel4 = 0;
	if (Page.uiChannel5 < 0)
		Page.uiChannel5 = 0;
	if (Page.uiChannel6 < 0)
		Page.uiChannel6 = 0;

	// Start
	BufData[0] = 0x3A;
	BufData[1] = 0x3A;

	// Command
	BufData[2] = 0x57;

	// Page 
	BufData[3] = uiPageIndex & 0xFF;

	// Light Brightness Data
	//Channel1 
	BufData[4] = (BYTE)((Page.uiChannel1 & 0xFF00) >> 8);
	BufData[5] = (BYTE)Page.uiChannel1 & 0xFF;

	// Channel2 
	BufData[6] = (BYTE)((Page.uiChannel2 & 0xFF00) >> 8);
	BufData[7] = (BYTE)Page.uiChannel2 & 0xFF;

	// Channel3
	BufData[8] = (BYTE)((Page.uiChannel3 & 0xFF00) >> 8);
	BufData[9] = (BYTE)Page.uiChannel3 & 0xFF;

	// Channel4
	BufData[10] = (BYTE)((Page.uiChannel4 & 0xFF00) >> 8);
	BufData[11] = (BYTE)Page.uiChannel4 & 0xFF;

	// Channel5
	BufData[12] = (BYTE)((Page.uiChannel5 & 0xFF00) >> 8);
	BufData[13] = (BYTE)Page.uiChannel5 & 0xFF;

	// Channel6
	BufData[14] = (BYTE)((Page.uiChannel6 & 0xFF00) >> 8);
	BufData[15] = (BYTE)Page.uiChannel6 & 0xFF;

	// Check Sum
	BufData[16] = BufData[2] ^ BufData[3] ^ BufData[4] ^ BufData[5] ^ BufData[6] ^ BufData[7] ^ BufData[8] ^ BufData[9] ^ BufData[10] ^ BufData[11] ^ BufData[12] ^ BufData[13] ^ BufData[14] ^ BufData[15];

	// End
	BufData[17] = 0xEE;
	BufData[18] = 0xEE;

	m_ComPort.WriteComm(BufData, 19);
	Sleep(100);
}
