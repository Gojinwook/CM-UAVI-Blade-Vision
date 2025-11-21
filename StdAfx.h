// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//
//#pragma comment(linker, "/entry:WinMainCRTStartup /subsystem:console")

#if !defined(AFX_STDAFX_H__A17A74E6_03C5_40B8_986D_80E599544869__INCLUDED_)
#define AFX_STDAFX_H__A17A74E6_03C5_40B8_986D_80E599544869__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#include <afxdisp.h>        // MFC Automation classes
#include <afxdtctl.h>		// MFC support for Internet Explorer 4 Common Controls
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC support for Windows Common Controls
#endif 						// _AFX_NO_AFXCMN_SUPPORT

#include <afxsock.h>		// MFC socket extensions

#define THEAPP			(*((CuScanApp*) AfxGetApp()))
#define SAFE_DELETE(p) if(p) { delete p; p = NULL; }

//#define LOOPINSPECTION
#include "mil.h"
#pragma comment(lib, "mil.lib")

// C3 1Side Build Version Automation - 250917, jhkim
// 세부 사항은 아래 헤더파일 참조 ///////////////////////////////// 
// 구성관리자 Debug <-> Release에 따라 필요 전처리기 자동 변경되어 수정 불필요
 #include "ConfigBuildVersion.h"

//////////////////////////////////////////////////////////////

#define BARCODE_CAM_CALLBACK_USE

#ifdef BARCODE_CAM_POINTGRAY_USE
#include "FlyCapture2.h"
using namespace FlyCapture2;
#endif

#ifdef BARCODE_CAM_CREVIS_USE
#include "VirtualFG40.h"
#pragma comment (lib, "VirtualFG40.lib")
#endif

#include "Define.h"

#include "Delegate.h"
#include "MainFrm.h"

#include "PBC.h"	// PClass
#include "halconcpp.h"
#include "GFunction.h"

#include "PFun.h"

using namespace Halcon;
#if H_VERSION > 10
	#pragma comment (lib, "halconcpp10xl.lib")
#else
	#pragma comment (lib, "halconcppxl.lib")
#endif

//Added by PIH 2005.11.18
#include <deque>
using namespace std;

// MFC Multi-Thread
#include <afxmt.h>

// Modified By Go
#include "AlgorithmParam.h"

// added by ParkJH
#include <vector>

//RYU 2007/02/21
#include "TransClientCoff.h"

// KJM 2D Image, ROI Debug
extern int g_iSaveImageIndex;

BOOL PeekAndPump();
double similarity(const Hobject& XldOrg, const Hobject& XldTgt);
void gen_bin_image_xld_crop(const Hobject& Xld, Hobject* Image, long lMarginX = 100, long lMarginY = 100);
void gen_bin_image_xld(const Hobject& Xld, Hobject* Image, long lMarginRight = 100, long lMarginBottom = 100);
Herror intersection_contours_lc (const Hobject& Region, const Hobject& Contours, Hobject* pContoursIntersection);

// TODO : need to select appropriate version of logging function - 250916, jhkim
void DoubleLogOut(const char *format, ...);
#define LOGGING(X)			{ TRACE(X + CString("\n")); }

#endif // !defined(AFX_STDAFX_H__A17A74E6_03C5_40B8_986D_80E599544869__INCLUDED_)

#include <afxcontrolbars.h>
#include <afxdlgs.h>
#include <afxwin.h>

#include "CSControls.h"
#include <afxcontrolbars.h>
#include <afxsock.h>

#ifdef _DEBUG
#pragma comment (lib, "CSControlsD.lib")
#else
#pragma comment (lib, "CSControlsR.lib")
#endif

#ifdef _DEBUG
#pragma comment (lib, "opencv_world310d.lib")
#else
#pragma comment (lib, "opencv_world310.lib")
#endif

#ifndef NONE
	#define NONE 0
#endif
