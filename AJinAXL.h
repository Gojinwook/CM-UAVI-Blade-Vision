// AJinAXL.h : 헤더 파일
//
#pragma once

// *** AJin Configuration **********************************
// DO Board        : SIO-DO32P => 2EA
// Library Version : EzSoftwareUC V4.1.1.4033, 64Bit Version
// *********************************************************

const int DO_COUNT = 64;		// Output Count

typedef union tag_DY_0 {	// Vision1
	DWORD nValue;
	struct {
		DWORD oTrigger: 1;		// Y000
		DWORD oLight1: 1;		// Y001
		DWORD oLight2: 1;		// Y002
		DWORD oLight3: 1;		// Y003
		DWORD oLight4: 1;		// Y004
		DWORD oLight5: 1;		// Y005
		DWORD oLight6: 1;		// Y006
		DWORD oLight7: 1;		// Y007

		DWORD oLight8: 1;		// Y008
		DWORD oLight9: 1;		// Y009
		DWORD oLight10: 1;		// Y010
		DWORD o011: 1;			// Y011
		DWORD o012: 1;			// Y012
		DWORD o013: 1;			// Y013
		DWORD o014: 1;			// Y014
		DWORD o015: 1;			// Y015
	};
} DY_0;

typedef union tag_DY_1 {	// Vision2
	DWORD nValue;
	struct {
		DWORD oTrigger: 1;		// Y100
		DWORD oLight1: 1;		// Y101
		DWORD oLight2: 1;		// Y102
		DWORD oLight3: 1;		// Y103
		DWORD oLight4: 1;		// Y104
		DWORD oLight5: 1;		// Y105
		DWORD oLight6: 1;		// Y106
		DWORD oLight7: 1;		// Y107

		DWORD oLight8: 1;		// Y108
		DWORD oLight9: 1;		// Y109
		DWORD oLight10: 1;		// Y110
		DWORD o111: 1;			// Y111
		DWORD o112: 1;			// Y112
		DWORD o113: 1;			// Y113
		DWORD o114: 1;			// Y114
		DWORD o115: 1;			// Y115
	};
} DY_1;

typedef union tag_DY_2 {	// Vision3
	DWORD nValue;
	struct {
		DWORD oTrigger: 1;		// Y200
		DWORD oLight1: 1;		// Y201
		DWORD oLight2: 1;		// Y202
		DWORD oLight3: 1;		// Y203
		DWORD oLight4: 1;		// Y204
		DWORD oLight5: 1;		// Y205
		DWORD oLight6: 1;		// Y206
		DWORD oLight7: 1;		// Y207

		DWORD oLight8: 1;		// Y208
		DWORD oLight9: 1;		// Y209
		DWORD oLight10: 1;		// Y210
		DWORD o211: 1;			// Y211
		DWORD o212: 1;			// Y212
		DWORD o213: 1;			// Y213
		DWORD o214: 1;			// Y214
		DWORD o215: 1;			// Y215
	};
} DY_2;

typedef union tag_DY_3 {	// Vision4
	DWORD nValue;
	struct {
		DWORD oTrigger: 1;		// Y300
		DWORD oLight1: 1;		// Y301
		DWORD oLight2: 1;		// Y302
		DWORD oLight3: 1;		// Y303
		DWORD oLight4: 1;		// Y304
		DWORD oLight5: 1;		// Y305
		DWORD oLight6: 1;		// Y306
		DWORD oLight7: 1;		// Y307

		DWORD oLight8: 1;		// Y308
		DWORD oLight9: 1;		// Y309
		DWORD oLight10: 1;		// Y310
		DWORD o311: 1;			// Y311
		DWORD o312: 1;			// Y312
		DWORD o313: 1;			// Y313
		DWORD o314: 1;			// Y314
		DWORD o315: 1;			// Y315
	};
} DY_3;

class CAJinAXL
{
public:
	CAJinAXL();
	virtual ~CAJinAXL();

private:
	DY_0	m_DY0;	// Output (Y000 - Y015)
	DY_1	m_DY1;	// Output (Y100 - Y115)
	DY_2	m_DY2;	// Output (Y200 - Y215)
	DY_3	m_DY3;	// Output (Y300 - Y315)

	void Init_Trigger(BOOL bOn);

public:
	BOOL Initialize();
	void Terminate();

	void Set_Trigger(int nPort, BOOL* bLight, int nS, int iPageOffset = 0, BOOL bSharedPage = FALSE);
	void Special_Set_Trigger(int nPort, BOOL* bLight, int nS);

	// Bit Test
	BOOL Get_Output(int nPort, int nIdx);
	void Set_Output(int nPort, int nIdx, BOOL bOut);

	// uSleep
 	LONGLONG	m_nFreq;	// Frequence
 	void DoEvents();
 	void uSleep(int msec);
	void Delay(int msec);
};

extern CAJinAXL g_objAJinAXL;

///////////////////////////////////////////////////////////////////////////////
