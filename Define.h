/////////// MAX_DEF /////////////////// CMI 3000
#define MAX_DEF			(999999999)

// File Access - jhkim
// File existence check macros
#ifndef F_OK
#define FILE_EXIST_OK 0
#endif
// File run check macros
#ifndef X_OK
#define FILE_RUN_OK 1
#endif
// File write check macros
#ifndef W_OK
#define FILE_WRITE_OK 2
#endif
// File read check macros
#ifndef R_OK
#define FILE_READ_OK 4
#endif
// oflag 재정의
#define READ_ONLY    0x0000 // 읽기 전용
#define WRITE_ONLY    0x0001 // 쓰기 전용
#define RDWR_ONLY      0x0002 // 읽기/쓰기 가능
// pmode 재정의
#define MY_S_IREAD     0x0100 // 읽기 권한
#define MY_S_IWRITE    0x0080 // 쓰기 권한


//Window cordinations...
#ifndef DEFINE_HEADER
#define DEFINE_HEADER
#define SCREEN_WIDTH	1920
#define SCREEN_HEIGHT	1080

#define LR_MARGIN		2
#define STATUS_HEIGHT	33

#define BOOL_SAVE_LAS	TRUE	// 24.06.26 - v2649 - Las Default TRUE - LeeGW
#define LOG_EVENT_CUT_TIME	6	// 24.06.26 - v2649 - Las Default TRUE - LeeGW

// For Inspection ...
#define VIEW1_DLG1_LEFT		(LR_MARGIN)								// 1.Inspect View1
#define VIEW1_DLG2_LEFT		(LR_MARGIN + 700)						// 2.Inspect View2
#define VIEW1_DLG3_LEFT		(LR_MARGIN + 900)						// 3.Inspect Summary
#define VIEW1_DLG4_LEFT		(LR_MARGIN + 900)						// 4.FM1
#define VIEW1_DLG5_LEFT		(VIEW1_DLG3_LEFT + SCREEN_WIDTH) / 2	// 5.FM2
#define VIEW1_DLG6_LEFT		(LR_MARGIN + 900)						// 6.Pad Image
#define VIEW1_DLG7_LEFT		(LR_MARGIN)								// 7.Inspect Result

#define VIEW1_DLG1_TOP		(150)
#define VIEW1_DLG2_TOP		(160)
#define VIEW1_DLG3_TOP		(110)
#define VIEW1_DLG4_TOP		(310)
#define VIEW1_DLG5_TOP		(310)
#define VIEW1_DLG6_TOP		(595)
#define VIEW1_DLG7_TOP		(700)

#define VIEW1_DLG1_WIDTH	(VIEW1_DLG2_LEFT - VIEW1_DLG1_LEFT-10)
#define VIEW1_DLG2_WIDTH	(VIEW1_DLG1_WIDTH)
#define VIEW1_DLG3_WIDTH	(SCREEN_WIDTH - VIEW1_DLG3_LEFT)
#define VIEW1_DLG4_WIDTH	(VIEW1_DLG5_LEFT - VIEW1_DLG4_LEFT)
#define VIEW1_DLG5_WIDTH	(VIEW1_DLG4_WIDTH)
#define VIEW1_DLG6_WIDTH	(VIEW1_DLG3_WIDTH)
#define VIEW1_DLG7_WIDTH	(VIEW1_DLG3_LEFT - VIEW1_DLG7_LEFT)

#define VIEW1_DLG1_HEIGHT	(VIEW1_DLG7_TOP - VIEW1_DLG1_TOP)
#define VIEW1_DLG2_HEIGHT	(VIEW1_DLG1_HEIGHT)
#define VIEW1_DLG3_HEIGHT	(VIEW1_DLG4_TOP - VIEW1_DLG3_TOP)
#define VIEW1_DLG4_HEIGHT	(VIEW1_DLG6_TOP - VIEW1_DLG4_TOP)
#define VIEW1_DLG5_HEIGHT	(VIEW1_DLG4_HEIGHT)
#define VIEW1_DLG6_HEIGHT	(SCREEN_HEIGHT - VIEW1_DLG6_TOP - STATUS_HEIGHT)
#define VIEW1_DLG7_HEIGHT	(SCREEN_HEIGHT - VIEW1_DLG7_TOP - STATUS_HEIGHT)

// For Teaching ...
#define VIEW2_DLG1_LEFT		(LR_MARGIN)			// 1.Image Display
#define VIEW2_DLG2_LEFT		(LR_MARGIN + 880)	// 2.PCB/PAD Sheet (TeachingService)
#define VIEW2_DLG3_LEFT		(LR_MARGIN)			// 3.Inspect Condition

#define VIEW2_DLG1_TOP		(110)
#define VIEW2_DLG2_TOP		(110)
#define VIEW2_DLG3_TOP		(800)

#define VIEW2_DLG1_WIDTH	(VIEW2_DLG2_LEFT - VIEW2_DLG1_LEFT)
#define VIEW2_DLG2_WIDTH	(SCREEN_WIDTH - VIEW2_DLG2_LEFT)
#define VIEW2_DLG3_WIDTH	(VIEW2_DLG1_WIDTH)

#define VIEW2_DLG1_HEIGHT	(VIEW2_DLG3_TOP - VIEW2_DLG1_TOP)
#define VIEW2_DLG2_HEIGHT	(SCREEN_HEIGHT - VIEW2_DLG1_TOP - STATUS_HEIGHT)
#define VIEW2_DLG3_HEIGHT	(SCREEN_HEIGHT - VIEW2_DLG3_TOP - STATUS_HEIGHT)

// ex)
// SetPosition(VIEW1_DLG1_LEFT, VIEW1_DLG1_TOP, VIEW1_DLG1_WIDTH, VIEW1_DLG1_HEIGHT);

#define WM_INSPECTION_LOOP 8604
#define WM_DISPLAY_TRAY 2184

#define PGCAM_IMAGE_WIDTH		1280
#define PGCAM_IMAGE_HEIGHT		960
#define PGCAM_PIXEL_RESOLUTION	0.015625

#define CAMERA_PARAM_RESOLUTION		1280

//#define PIXEL_SIZE_PMODE	0.01		//SEMI
//#define PIXEL_SIZE_PMODE	0.00625		//FULL

#ifdef WELDING_CAM
#define PIXEL_SIZE_PMODE	0.006811	//WELDING
#else
#define PIXEL_SIZE_PMODE	0.006875	//NEW
#endif

#define GRAB_X_MAX		1152	// 
#define GRAB_Y_MAX		1152	// 
#define GRAB_X_OFFSET	224		// 
#define GRAB_Y_OFFSET	224		// 

#define CAM_FOV_CROP	0
#define CAM_FOV_FULL	1

#define MAX_INSPECTION_NUMBER	10
#define MAX_SCAN_TAB			3
#define NOT_USED_CRITERION		-1

//////////////////////////////////////////////////////////////////////////

#define UM_SHEET_CLOSED WM_USER+300

#define UM_SCANDONE		WM_USER+200

#define UM_2DFRAME_READY WM_USER+102

#define UM_FMARK_DISPLAY		WM_USER+511

#define UMSG_SAVE_IMAGE_DONE	WM_USER + 512
#define UM_RS_GOTO_DEFECT		WM_USER + 513

#define UM_TS_ROI_SELECTED		WM_USER + 514
#define UM_TS_ROI_INSPECTION_TAB_SAVE	WM_USER + 515

#define UM_UPDATE_PROGRESS_START			WM_USER + 516
#define UM_UPDATE_PROGRESS					WM_USER + 517
#define UM_UPDATE_PROGRESS_DONE				WM_USER + 518
// for Optimized initialize - 251104, jhkim
#define UM_INIT_TAB_CONTROL_DLG_DONE		WM_USER + 519
#define UM_LOAD_MODEL_START					WM_USER + 520
#define UM_LOAD_MODEL_BASE_DATA_DONE		WM_USER + 521
#define UM_LOAD_MODEL_DONE					WM_USER + 522
#define UM_LOAD_MODEL_CONTOUR_DATA_DONE		WM_USER + 523

//////////////////////////////////////////////////////////////////////////
//	Threshold Reference Peak Method
#define TRPM_MAX_FREQUENCY	0
#define TRPM_BOUNDARY		1
#define TRPM_FIRST_PEAK		2

//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//	Inspection Method
#define IM_MANUAL_THRESHOLD	0
#define IM_AUTO_THRESHOLD	1
#define IM_EDGE_DETECTION	2
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//	Review Process Done
#define RPD_3DVIEW		0
#define RPD_2DVIEW		1
#define RPD_PADINFO		2
#define RPD_IMAGEVIEW	3

//////////////////////////////////////////////////////////////////////////
#define WM_START_REVIEW		5000
#define WM_CAPTURE_REVIEW	5001
#define WM_END_REVIEW		5002
#define WM_LIGHT_REVIEW		5003
#define WM_GET_DEFECT_NAME 5004
#define WM_SAVEIMAGE_DONE  5005
#define WM_DESTROY_DONE 5006

#define PSI1000_MOLD		0
#define PSI1000_PCB			1
#define PSI1000_3D			2

#define OPTION_ADR_SAVEIMG_REVIEW 1


/////////////////////////////////////////////////
#define SCAN_CAL	1
#define SCAN_RR		2
/////////////////////////////////////////////////

// DefectData.h
#define NOT_MEASURED		-1
#define DT_GOOD			0
#define DT_AREA			1
#define DT_LENGTH		2
#define DT_WIDTH		3
#define DT_HEIGHT		4
#define DT_RA			5
#define DT_RB			6
#define DT_CONTLENGTH	7
#define DT_WH_RATIO		8
#define DT_RAB_RATIO	9

// gjw
#define INSPECTION_ERR_DELAY_TIME	20000
#define REVIEW_ERR_DELAY_TIME	10000


//#define TS_COLOR_PAD					RGB(225,235,211)
#define TS_COLOR_PAD					RGB(255,255,150)
#define TS_COLOR_BG						RGB( 38,132, 68)
#define TS_COLOR_FIDUCIAL				RGB(  0,  0,255)
#define TS_COLOR_MOLD					RGB(238, 82,238)

//#define TS_COLOR_SELECTED_PAD			RGB(  0,  0, 64)
#define TS_COLOR_SELECTED_PAD			RGB(255,  0,128)
#define TS_COLOR_REALPAD				RGB(128,255,255)
#define TS_COLOR_MANUALPAD				RGB(  0,255,  0)

//#define TS_COLOR_REALPAD				RGB(255,128, 64)
#define TS_COLOR_INSPECTIING_FRAME		RGB( 52,184, 95)
#define TS_COLOR_PADINFO_TEXT			RGB(  0,  0,  0)
#define TS_COLOR_PADINFO_BG				RGB(255,255,255)

///////////////  Inspection View Gerber Color Definition  (2006/02/12)
//#define TS_COLOR_INSPECTING_PAD			RGB(255, 255, 255)
#define TS_COLOR_INSPECTING_PAD			RGB(128,255,255)
#define TS_COLOR_RESULT_GOOD			RGB(  0,255,  0)
#define TS_COLOR_RESULT_ERROR			RGB(255,  0,  0)
#define TS_COLOR_RESULT_WARNING			RGB(255,128,  0)
#define TS_COLOR_RESULT_INSPECT_ERROR	RGB(  0,  0,  0)
#define TS_COLOR_RESULT_NO_PUNCH_ERROR	RGB(255,  0,  0)
#define TS_COLOR_EXTRA_BG				RGB( 75, 75, 75)
#define TS_INSPECT_PCB_MARGIN			50
/////////////////


///////////// Image Dimension /////////////////////////////////////
#define CROP_IMG_SIZE_RESULT	(800)
#define FULL_IMG_SIZE_RESULT	(1250)


#ifdef WELDING_CAM
#define PXLLEN			(6.811)				// Real length of a pixel = 5/0.7272			==> NEW, um		
#define PXLAREA			(46.389721)			// Real Area of a pixel = (CMOS width/Magnificance)^2 = (5/0.7272)^2, um2
#define PXLAREAMM		(0.000046389721)			// Real Area of a pixel = (CMOS width/Magnificance)^2 = (5/0.7272)^2, um2
#else
#define PXLLEN			(6.875)				// Real length of a pixel = 5/0.7272			==> NEW, um		
#define PXLAREA			(47.265625)			// Real Area of a pixel = (CMOS width/Magnificance)^2 = (5/0.7272)^2, um2
#define PXLAREAMM		(0.000047265625)			// Real Area of a pixel = (CMOS width/Magnificance)^2 = (5/0.7272)^2, um2
#endif

#define WELDING_CAM_BARCODE_PAGE_IDX	5

////////////// Light Control ////////////////////
//#define LIGHT_BRIGHT_MAX 2500
#define LIGHT_BRIGHT_MAX 999
#define LIGHT_SHOT_CNT	(5)
#define LIGHT_CH_CNT	(6)


///////////////  Camera Module Type
#define CMT_K12	12
#define CMT_K14	14
#define CMT_K15	15

#define CMT_K51	51
#define CMT_K52	52
#define CMT_K53	53
#define CMT_K55	55


///////////////  Barrel Type
#define BT_TOP							0
#define BT_TOP_BOTTOM					1

///////////////  Mode Switch
#define MODE_INSPECT_VIEW_ADMIN			1
#define MODE_INSPECT_VIEW_OPERATOR		2
#define MODE_ADMIN_TEACH_VIEW			3

///////////////  Image Type
#define IMAGE_TYPE_SURFACE				1
#define IMAGE_TYPE_EDGE					2
#define IMAGE_TYPE_LENS1				3
#define IMAGE_TYPE_LENS2				4
#define IMAGE_TYPE_EDGE_SUB_SURFACE		5

///////////////  Defect Type
#define DEFECT_TYPE_PROCESSING_ERROR		0
#define DEFECT_TYPE_GOOD					1
#define DEFECT_TYPE_BARREL					2
#define DEFECT_TYPE_LENS					3
#define DEFECT_TYPE_BARREL_LENS				4
#define DEFECT_TYPE_LIGHT_ERROR				7
#define DEFECT_TYPE_EMPTY					8
#define DEFECT_TYPE_ALIGN_ERROR				9
#define DEFECT_TYPE_BARCODE_ERROR			10
#define DEFECT_TYPE_BARCODE_SHIFT			11
#define DEFECT_TYPE_BLACK_COATING_DIAMETER	12
#define DEFECT_TYPE_MODULE_MIX_ERROR		13
#define DEFECT_TYPE_EPOXYHOLE				14
#define DEFECT_TYPE_SPRINGNG				15
#define DEFECT_TYPE_FAING					16

///////////////  Defect Name

// 검사유형 통합 - LeeGW START
enum {
	DEFECT_NAME_DIRT=0,
	DEFECT_NAME_SCRATCH,
	DEFECT_NAME_STAIN,
	DEFECT_NAME_DENT,
	DEFECT_NAME_CHIPPING,
	DEFECT_NAME_EPOXY,
	DEFECT_NAME_OUTSIDE_CT,
	DEFECT_NAME_PEEL_OFF,
	DEFECT_NAME_WHITEDOT,		// 2024.09.30 - v2660 - WhiteDot 추가 - LEEGW
	DEFECT_NAME_LENS_CONTAMINATION,
	DEFECT_NAME_LENS_SCRATCH,
	DEFECT_NAME_LENS_DIRT,
	DEFECT_NAME_LENS_WHITEDOT,
	DEFECT_NAME_EPOXYHOLE,
	DEFECT_NAME_NORTH_SPRINGNG,
	DEFECT_NAME_EAST_SPRINGNG,
	DEFECT_NAME_LOCAL_ALIGN,
	DEFECT_NAME_EXCLUSION_AREA,
	DEFECT_NAME_FAI_26,
	DEFECT_NAME_FAI_51,
	DEFECT_NAME_FAI_14,
	DEFECT_NAME_FAI_ROTATION,
	DEFECT_NAME_FAI_EA,
	DEFECT_NAME_FAI_DIAMETER,
	DEFECT_NAME_FAI_POSITION,
	MAX_DEFECT_NAME
};

static const CString g_sDefectName[] =
{
	_T("Barrel Dirt"),
	_T("Barrel Scratch"),
	_T("Barrel Stain"),
	_T("Barrel Dent"),
	_T("Barrel Chipping"),
	_T("Barrel Epoxy"),
	_T("Barrel Outside CT"),
	_T("Barrel Peel Off"),
	_T("Barrel WhiteDot"),
	_T("Lens Contamination"),
	_T("Lens Scratch"),
	_T("Lens Dirt"),
	_T("Lens WhiteDot"),
	_T("Epoxy Hole"),
	_T("North Spring NG"),
	_T("East Spring NG"),
	_T("Local Align"),
	_T("Exclusion Area"),
	_T("FAI-26"),
	_T("FAI-51"),
	_T("FAI-14"),
	_T("FAI-Rotation"),
	_T("FAI-Spot-EA"),
	_T("FAI-Spot-Diameter"),
	_T("FAI-Spot-Position")
};

static const CString g_sDefectFileName[] =
{
	_T("Barrel_Dirt"),
	_T("Barrel_Scratch"),
	_T("Barrel_Stain"),
	_T("Barrel_Dent"),
	_T("Barrel_Chipping"),
	_T("Barrel_Epoxy"),
	_T("Barrel_OutsideCT"),
	_T("Barrel_PeelOff"),
	_T("Barrel_WhiteDot"),
	_T("Lens_Contamination"),
	_T("Lens_Scratch"),
	_T("Lens_Dirt"),
	_T("Lens_WhiteDot"),
	_T("EpoxyHole"),
	_T("NorthSpringNG"),
	_T("EastSpringNG"),
	_T("LocalAlign"),
	_T("ExclusionArea"),	
	_T("FAI-26"),		
	_T("FAI-51"),
	_T("FAI-14"),
	_T("FAI-Rotation"),
	_T("FAI-Spot-EA"),
	_T("FAI-Spot-Diameter"),
	_T("FAI-Spot-Position")
};

const static char* g_sDefectPosName[MAX_DEFECT_NAME] =
{
	_T("Barrel"),
	_T("Barrel"),
	_T("Barrel"),
	_T("Barrel"),
	_T("Barrel"),
	_T("Barrel"),
	_T("Barrel"),
	_T("Barrel"),
	_T("Barrel"),
	_T("Lens"),
	_T("Lens"),
	_T("Lens"),
	_T("Lens"),
	_T("Sidefill"),
	_T("Spring"),
	_T("Spring"),
	_T("LocalAlign"),
	_T("ExclusionArea"),
	_T("FAI-26"),
	_T("FAI-51"),
	_T("FAI-14"),
	_T("FAI-Rotation"),
	_T("FAI-Spot-EA"),
	_T("FAI-Spot-Diameter"),
	_T("FAI-Spot-Position")
};
// 검사유형 통합 - LeeGW END

#define DEFECT_CODE_GOOD				1
#define DEFECT_CODE_DIRT				2
#define DEFECT_CODE_SCRATCH				3
#define DEFECT_CODE_STAIN				4
#define DEFECT_CODE_DENT				5
#define DEFECT_CODE_CHIPPING			6
#define DEFECT_CODE_EPOXY				7
#define DEFECT_CODE_OUTSIDE_CT			8
#define DEFECT_CODE_PEEL_OFF			9
#define DEFECT_CODE_WHITEDOT			10	
#define DEFECT_CODE_LENS_CONTAMINATION	11		
#define DEFECT_CODE_LENS_SCRATCH		12		
#define DEFECT_CODE_LENS_DIRT			13		
#define DEFECT_CODE_LENS_WHITEDOT		14
#define DEFECT_CODE_LENS_ETC			15
#define DEFECT_CODE_FAING				16
#define DEFECT_CODE_EPOXYHOLE			17
#define DEFECT_CODE_SPRINGNG			18
#define DEFECT_CODE_BLACKCOATING_ERROR	19
#define DEFECT_CODE_BARCODE_ERROR		20
#define DEFECT_CODE_BARCODE_SHIFT		21
#define DEFECT_CODE_MODULE_MIX_ERROR	22
#define DEFECT_CODE_MATCHING_ERROR		23
#define DEFECT_CODE_LIGHT_ERROR			24
#define DEFECT_CODE_EMPTY				25
#define MAX_DEFECT_NUMBER				26		// Max Defect Number

///////////////  Inspection Result
#define IR_GOOD			0
#define IR_DIRT			-1
#define IR_SCRATCH		-2
#define IR_STAIN		-3
#define IR_DENT			-4
#define IR_CHIPPING		-5
#define IR_EPOXY		-6
#define IR_OUTSIDE_CT	-7
#define IR_PEEL_OFF		-8
#define IR_WHITEDOT		-9
#define IR_EPOXYHOLE	-10
#define IR_SPRINGNG		-11
#define IR_NG			-99

#define MAX_GRAB_TIMEOUT	500

#define BLACK_MATCHING_IMAGE_TYPE_BSURFACE 0
#define BLACK_MATCHING_IMAGE_TYPE_BEDGE	1
#define BLACK_MATCHING_IMAGE_TYPE_LENS1 2
#define BLACK_MATCHING_IMAGE_TYPE_LENS2 3

////////////////////////////////////////////////////////////////////
/// New GUI (2016. 05. 29)

// 검사유형 통합 - LeeGW START
// Matching Contour
enum {
	MATCHING_CONTOUR_1 = 0,
	MATCHING_CONTOUR_2,
	MATCHING_CONTOUR_3,
	MATCHING_CONTOUR_4,
	MATCHING_CONTOUR_5,
	MAX_CONTOUR_NUM
};

enum {
	MATCHING_MODEL_1 = 0,
	MATCHING_MODEL_2,
	MATCHING_MODEL_3,
	MATCHING_MODEL_4,
	MAX_MATCHING_MODEL_NUM
};
// 검사유형 통합 - LeeGW END

//////////////////////////////////////////////////////////////////////////
/// Handler Comm.
#define VISION_TYPE_INSPECTION	0
#define VISION_TYPE_BARCODE		1

#define PICKER_TRAY_DIR_X    0
#define PICKER_TRAY_DIR_Y    1

#define MAX_MODULE_ONE_TRAY		80
#define MAX_MODULE_ONE_LOT      1800
#define MAX_TRAY_LOT	60		// 최대 Tray 갯수 / LOT

#define MAX_CIRCULAR_TABLE_INDEX	8
#define MAX_CIRCULAR_TABLE_POCKET	48

#define WARNING_DEFECT_NUMBER	5

#define MAX_REVIEW_SAMPLE		50	

//////////////////////////////////////////////////////////////////////////
/// Equipment Mode Status

#define MS_NONE		0
#define MS_INSPECT	1
#define MS_TEACH	2
#define MS_DEMO		3
#define MS_TEST		4
#define MS_DEBUG	5
#define MS_STEP		6
#define MS_HOME		7
#define MS_MANUAL	8

//////////////////////////////////////////////////////////////////////////
/// Handler Status
#define HS_NOT_READY	0
#define HS_READY		1
#define HS_RUN			2
#define HS_RESET		3
#define HS_PAUSE		4
#define HS_CYCLE_STOP	5
#define HS_STOP			6
//////////////////////////////////////////////////////////////////////////
/// Vision Status
#define VS_NOT_READY	HS_NOT_READY
#define VS_READY		HS_READY
#define VS_RUN			HS_RUN
#define VS_RESET		HS_RESET
#define VS_PAUSE		HS_PAUSE
#define VS_CYCLE_STOP	HS_CYCLE_STOP
#define VS_STOP			HS_STOP
//////////////////////////////////////////////////////////////////////////
/// Barrel Defect Position
#define DP_INNER_BARREL		1
#define DP_TOP_BARREL		2
#define DP_BOTTOM_BARREL	3

#define BARCODE_GRAB_STATUS_NOT_READY	-1
#define BARCODE_GRAB_STATUS_NO_GRAB		0
#define BARCODE_GRAB_STATUS_GRAB_DONE	1

#define INSPECT_GRAB_STATUS_NOT_READY	-1
#define INSPECT_GRAB_STATUS_GRAB_DONE	1

#define LOG_COLOR_BLACK    0
#define LOG_COLOR_RED      1
#define LOG_COLOR_YELLOW   2
#define LOG_COLOR_GREEN    3
#define LOG_COLOR_BLUE     4
#define LOG_COLOR_PURPLE   5
#define LOG_COLOR_ORANGE   6
#define LOG_COLOR_LIME     7
#define LOG_COLOR_CYAN     8
#define LOG_COLOR_NAVY     9 
#define LOG_COLOR_MAGENTA 10 

#define IMAGE_SCALE_AUTO	0
#define IMAGE_SCALE_MANUAL	1

// 검사유형 통합 - LeeGW START
enum {
	MACHINE_NORMAL = 0,
	MACHINE_SIDEFILL,
	MACHINE_FIDUCIAL,
	MACHINE_BRACKET,
	MACHINE_WELDING,
	MAX_MACHINE_TYPE
};

#define MAX_IMAGE_TAB				7
#define EXTRA2_IMAGE_TAB			6
#define WELDING_IMAGE_TAB			4
// MEMO : Auto Param. 1면 검사이미지 탭(BARREL_LENS_IMAGE_TAB)  - 250912, jhkim
#define BARREL_LENS_IMAGE_TAB		4
// MEMO : Auto Param. 1면 검사티칭 탭(MAX_TEACHING_TAB)  - 250912, jhkim
#define MAX_TEACHING_TAB			6
#define MAX_ROI_TEACHING_TAB		5	
#define MAX_BARCODE_GRAB			5


// 검사이미지
enum {
	TRIGGER_SURFACE=0,
	TRIGGER_EDGE,
	TRIGGER_LENS1,
	TRIGGER_LENS2,
	TRIGGER_EXTRA1,
	TRIGGER_EXTRA2,
	TRIGGER_EXTRA3,
	MAX_TRIGGER_TYPE
};

const static char* g_sImageName[MAX_MACHINE_TYPE][MAX_TRIGGER_TYPE] =
{
	// Normal
	{
		_T("Surface"),
		_T("Edge"),
		_T("Lens1"),
		_T("Lens2"),
		_T("Extra1"),
		_T("Extra2"),
		_T("Extra3"),
	},
	// Sidefill
	{
		_T("Surface"),
		_T("Edge"),
		_T("Lens1"),
		_T("Lens2"),
		_T("Sidefill1"),
		_T("Sidefill2"),
		_T("Sidefill3"),
	},
	// Fiducial
	{
		_T("Surface"),
		_T("Edge"),
		_T("Lens1"),
		_T("Lens2"),
		_T("Fiducial1"),
		_T("Fiducial2"),
		_T("Fiducial3"),
	},
	// Bracket
	{
		_T("Surface"),
		_T("Edge"),
		_T("Lens1"),
		_T("Lens2"),
		_T("Bracket1"),
		_T("Bracket2"),
		_T("Bracket3"),
	},
	// Welding
	{
		_T("Image1"),
		_T("Image2"),
		_T("Image3"),
		_T("Image4"),
		_T("Extra1"),
		_T("Extra2"),
		_T("Extra3"),
	}
};

enum {
	LIGHT_CHANNEL_1 = 0,
	LIGHT_CHANNEL_2,
	LIGHT_CHANNEL_3,
	LIGHT_CHANNEL_4,
	LIGHT_CHANNEL_5,
	MAX_LIGHT_CHANNEL
};

// PC별 그랩보드 인덱스
#define PC_VISION_GRABBER_1			0
#define PC_VISION_GRABBER_2			1
#define PC_VISION_GRABBER_NUMBER	2

#define MAX_LIGHT_PAGE	2
#define LIGHT_PAGE_NO_USE	-1
// 검사유형 통합 - LeeGW End

#define MAX_COMBINE_IMAGE_NUMBER	2

#define MAX_MODULE_NUM		60
#define DEEP_MODEL_NUM		16
#define RCV_TIMEOUT -1
#define RCV_WAIT -2
#define RCV_ERROR -3

// 카메라별 구분 인덱스
#define VISION_CAM_1			0
#define VISION_CAM_2			1
#define VISION_CAM_3			2
#define VISION_CAM_4			3

#define ADJ_TYPE_LOCAL			0
#define ADJ_TYPE_NETWORK		1

#define ADJ_INSP_TYPE_BLOB		0
#define ADJ_INSP_TYPE_ROI		1

#define INSPECTION_TYPE_ALIGN			0
#define INSPECTION_TYPE_INSPECTION		1
// define number can be changed....
#define INSPECTION_TYPE_DELETE			2
#define INSPECTION_TYPE_ADD				3
#define INSPECTION_TYPE_LOCAL_ALIGN			4	// 24.02.27 Local Align 추가 - LeeGW Start
#define INSPECTION_TYPE_AI_INSPECTION		5	// 24.10.30 AI 검사 추가 - LeeGW
#define INSPECTION_TYPE_FAI_OUTER_CIRCLE	6	// 24.10.30 FAI 추가 - LeeGW
#define INSPECTION_TYPE_FAI_INNER_CIRCLE	7	// 24.10.30 FAI 추가 - LeeGW
#define INSPECTION_TYPE_FAI_DATUM_CIRCLE	8	// 24.10.30 FAI 추가 - LeeGW
#define INSPECTION_TYPE_FAI_DATUM_ELLIPSE	9	// 24.10.30 FAI 추가 - LeeGW
#define INSPECTION_TYPE_FAI_WELDING_SPOT	10	// 25.05.13 FAI 추가 - LeeGW
#define INSPECTION_TYPE_FAI_WELDING_POCKET	11	// 25.05.13 FAI 추가 - LeeGW
#define INSPECTION_TYPE_FAI_BRACKET			12


#define INSPECTION_TYPE_FAI_START			INSPECTION_TYPE_FAI_OUTER_CIRCLE
#define INSPECTION_TYPE_FAI_END				INSPECTION_TYPE_FAI_BRACKET

enum eROIAlgorithmDlg
{
	ROI_PREPROCESSING	,
	ROI_ANISO_ALIGN		,
	ROI_PART_CHECK		,
	ROI_EDGE_MEASURE	,
	ROI_INSP_ALGORITHM	,
	ROI_ALGORITHM_END	,

};

static CString g_sROIAlgorithmDlgName[] =
{	
	_T("전처리 적용"),			//	ROI_PREPROCESSING		
	_T("비등방 얼라인"),			//	ROI_ANISO_ALIGN
	_T("Part 확인"),				//	ROI_PART_CHECK	
	_T("Edge 측정"),			//	ROI_EDGE_MEASURE
	_T("검사 알고리즘"),			//	ROI_INSP_ALGORITHM	
};		


#define LOCAL_ALIGN_MATCHING_ROI_TYPE_MATCHING	0
#define LOCAL_ALIGN_MATCHING_ROI_TYPE_EDGE_MEASURE	1

#define LOCAL_ALIGN_MATCHING_ROI_SHAPE		0
#define LOCAL_ALIGN_MATCHING_ROI_POSITION	1
#define LOCAL_ALIGN_LINE_FIT				2

#define LOCAL_ALIGN_FIT_LINE_ROI_TOP		0
#define LOCAL_ALIGN_FIT_LINE_ROI_BOTTOM		1
#define LOCAL_ALIGN_FIT_LINE_ROI_LEFT		2
#define LOCAL_ALIGN_FIT_LINE_ROI_RIGHT		3

#define POST_LOCAL_ALIGN_ROTATION_CENTER_MODELCENTER		0
#define POST_LOCAL_ALIGN_ROTATION_CENTER_LEFTTOP     		1
#define POST_LOCAL_ALIGN_ROTATION_CENTER_RIGHTTOP    		2
#define POST_LOCAL_ALIGN_ROTATION_CENTER_LEFTBOTTOM  		3
#define POST_LOCAL_ALIGN_ROTATION_CENTER_RIGHTBOTTOM		4

#define LINE_FIT_XDIR_OBJECT_TOP		0
#define LINE_FIT_XDIR_OBJECT_BOTTOM		1
#define LINE_FIT_YDIR_OBJECT_LEFT		2
#define LINE_FIT_YDIR_OBJECT_RIGHT		3
#define INVALID_ALIGN_RESULT	-99999
#define NOT_ANGLE_LINE	-99999
#define ONE_POINT_EDGE_EXTEND_PXL	20
// 24.02.27 Local Align 추가 - LeeGW End

// Image Filter
#define FILTER_TYPE_NOT_USED			0
#define FILTER_TYPE_MEAN				1
#define FILTER_TYPE_MEDIAN				2
#define FILTER_TYPE_GAUSS				3
#define FILTER_TYPE_SCALE_MAX			4
#define FILTER_TYPE_emphasize			5
#define FILTER_TYPE_GRAY_OPENING		6
#define FILTER_TYPE_GRAY_CLOSING		7
#define FILTER_TYPE_EXPONENT			8
#define FILTER_TYPE_LOG					9
#define FILTER_TYPE_ANISO_DIFFUSION		10
#define FILTER_TYPE_SHARPENING			11

// INSPECTION FAI define ParkSW 20240626
#define MAX_FAI_ITEM				141
#define FAI_MEASURE_ERROR_NUMBER	-9999
#define MAX_ONE_FAI_MEASURE_VALUE	10
#define MAX_FAI_CIRCLE_FIT_POINT	8
#define MAX_FAI_ELLIPSE_FIT_POINT	10
#define MAX_FAI_ROTATION_BRACKET_ROI 6
// ParkSW FAI Define End

enum ENUM_DEFECT_HEXA_TYPE { ENUM_SURFACE_NG_TYPE = 1 , ENUM_EDGE_NG_TYPE = 2, ENUM_LENS1_NG_TYPE = 4, ENUM_LENS2_NG_TYPE = 8, 
                      ENUM_SURFACE_OK_TYPE = 16 , ENUM_EDGE_OK_TYPE = 32, ENUM_LENS1_OK_TYPE = 64, ENUM_LENS2_OK_TYPE = 128 };

/// Matching Algorithm Type
#define ALIGN_MATCHING_SHAPE			0
#define ALIGN_MATCHING_TEMPLATE			1

#define OVERLAY_IMAGE_RESULT_FONT_SIZE			40
#define OVERLAY_IMAGE_EXTRA_FONT_SIZE	16

#define LINE_FIT_APPLY_POS_TOP		0
#define LINE_FIT_APPLY_POS_BOTTOM	1
#define LINE_FIT_APPLY_POS_LEFT		2
#define LINE_FIT_APPLY_POS_RIGHT	3

//Multiple Defect modify start
#define MAX_VIRTUAL_MAGAZINE_NO                2 
#define MAX_MULTIPLE_DEFECT_NUMBER             MAX_DEFECT_NAME
#define HANDLER_ALARM_MULTIPLE_DEFECT          9950
//Multiple Defect modify end

//FOB Mode Change
#define FOB_MODE_OPERATOR 0
#define FOB_MODE_ADMIN    1

#define HANDLER_ALARM_BARCODE_NG 9960 //Barcode Rotation Inspection (Barcode Shift NG)

// 로그 정리 - LeeGW START
// LotResult
enum eLotResultLogHeader
{
	LOT_RESULT_HEADER_DATE,
	LOT_RESULT_HEADER_TIME,
	LOT_RESULT_HEADER_STATION,
	LOT_RESULT_HEADER_MACHINE_CODE,
	LOT_RESULT_HEADER_LOGIN_MODE,
	LOT_RESULT_HEADER_OPERATOR,
	LOT_RESULT_HEADER_SW_VERSION,
	LOT_RESULT_HEADER_LOTID,
	LOT_RESULT_HEADER_RECIPE,
	LOT_RESULT_HEADER_RECIPE_VERSION,
	LOT_RESULT_HEADER_TRAYNO,
	LOT_RESULT_HEADER_BARCODEID,
	LOT_RESULT_HEADER_MODULENNO,
	LOT_RESULT_HEADER_CYCLE_TIME,
	LOT_RESULT_HEADER_FINAL_RESULT,
	LOT_RESULT_HEADER_BARREL_RESULT,
	LOT_RESULT_HEADER_LENS_RESULT,
	LOT_RESULT_HEADER_BL_INSP_TYPE,
	LOT_RESULT_HEADER_BL_DEFECT_TYPE,
	LOT_RESULT_HEADER_BL_DEFECT_AREA,
	LOT_RESULT_HEADER_BL_DEFECT_CENTER_X,
	LOT_RESULT_HEADER_BL_DEFECT_CENTER_Y,
	LOT_RESULT_HEADER_EXTRA_RESULT,
	LOT_RESULT_HEADER_EXTRA_INSP_TYPE,
	LOT_RESULT_HEADER_EXTRA_DEFECT_TYPE,
	LOT_RESULT_HEADER_EXTRA_DEFECT_AREA,
	LOT_RESULT_HEADER_EXTRA_DEFECT_CENTER_X,
	LOT_RESULT_HEADER_EXTRA_DEFECT_CENTER_Y,
	LOT_RESULT_HEADER_FAING_RESULT,
	LOT_RESULT_HEADER_BARCODE_SHIFT_RESULT,
	LOT_RESULT_HEADER_BARCODE_ROTATION_RESULT,
	LOT_RESULT_HEADER_BARCODE_OFFSET_X,
	LOT_RESULT_HEADER_BARCODE_OFFSET_Y,
	LOT_RESULT_HEADER_BARCODE_OFFSET_R,
	LOT_RESULT_HEADER_BLACK_COATING_RESULT,
	LOT_RESULT_HEADER_BLACK_COATING_DIAMETER,
	LOT_RESULT_HEADER_BLACK_COATING_CENTER_X,
	LOT_RESULT_HEADER_BLACK_COATING_CENTER_Y,
	LOT_RESULT_HEADER_O1O2_DISTANCE,
	LOT_RESULT_HEADER_DIAMETER_MIN,
	LOT_RESULT_HEADER_MODULE_MIX,
	LOT_RESULT_HEADER_BARCODE_ERRROR,
	LOT_RESULT_HEADER_MATCHING_ERRROR,
	LOT_RESULT_HEADER_LIGHT_ERRROR,
	LOT_RESULT_HEADER_DEFECT_CODE,
	LOT_RESULT_HEADER_END
};

static const CString g_sLotResultLogHeader[] =
{
	_T("Date")			,
	_T("Time")			,
	_T("Station")		,
	_T("Machine_Code")	,
	_T("Login_Mode")	,
	_T("Operator")		,
	_T("SW_Version")	,
	_T("LotNum")			,
	_T("Recipe")		,
	_T("Recipe_Version"),
	_T("TrayNo")		,
	_T("Barcode")		,
	_T("ModuleNo")		,
	_T("Cycle_Time")	,
	_T("Final_Result")	,
	_T("Barrel_Result")	,
	_T("Lens_Result")	,
	_T("Insp_Type(B/L)")	,
	_T("Defect_Type(B/L)")	,
	_T("Defect_Area(B/L)")	,
	_T("Defect_CenterX(B/L)")	,
	_T("Defect_CenterY(B/L)")	,
	_T("Ex_Result")		,
	_T("Insp_Type(EX)")	,
	_T("Defect_Type(EX)")	,
	_T("Defect_Area(EX)")	,
	_T("Defect_CenterX(EX)")	,
	_T("Defect_CenterY(EX)")	,
	_T("FAI_Result")		,
	_T("Barcode_Shift_Result")		,
	_T("Barcode_Rotation_Result")	,
	_T("Barcode_OffsetX")	,
	_T("Barcode_OffsetY")	,
	_T("Barcode_OffsetR")	,
	_T("BlackCoating_Result")	,
	_T("BlackCoating_Diameter")	,
	_T("BlackCoating_CenterX")		,
	_T("BlackCoating_CenterY")		,
	_T("O1O2_Distance")		,
	_T("DiameterMin")		,
	_T("ModuleMix")		,
	_T("BarcodeError")		,
	_T("MatchingError")		,
	_T("LightError")		,
	_T("Code")		,
};

// LotSummary
enum eLotSummaryLogHeader
{
	LOT_SUMMARY_HEADER_DATE,
	LOT_SUMMARY_HEADER_TIME,
	LOT_SUMMARY_HEADER_START_TIME,
	LOT_SUMMARY_HEADER_END_TIME,
	LOT_SUMMARY_HEADER_STATION,
	LOT_SUMMARY_HEADER_SW_VERSION,
	LOT_SUMMARY_HEADER_PARAM_VERSION,
	LOT_SUMMARY_HEADER_CONFIG,
	LOT_SUMMARY_HEADER_LOTID,
	LOT_SUMMARY_HEADER_CYCLE_TIME,
	LOT_SUMMARY_HEADER_INSP_COUNT,
	LOT_SUMMARY_HEADER_OK_COUNT,
	LOT_SUMMARY_HEADER_OK_RATIO,
	LOT_SUMMARY_HEADER_NG_COUNT,
	LOT_SUMMARY_HEADER_NG_RATIO,
	LOT_SUMMARY_HEADER_NG_BARREL,
	LOT_SUMMARY_HEADER_NG_LENS,
	LOT_SUMMARY_HEADER_NG_BARRELLENS,
	LOT_SUMMARY_HEADER_NG_BARREL_DIRT,
	LOT_SUMMARY_HEADER_NG_BARREL_SCRATCH,
	LOT_SUMMARY_HEADER_NG_BARREL_STAIN,
	LOT_SUMMARY_HEADER_NG_BARREL_DENT,
	LOT_SUMMARY_HEADER_NG_BARREL_CHIPPING,
	LOT_SUMMARY_HEADER_NG_BARREL_EPOXY,
	LOT_SUMMARY_HEADER_NG_BARREL_OUTSIDECT,
	LOT_SUMMARY_HEADER_NG_BARREL_PEELOFF,
	LOT_SUMMARY_HEADER_NG_BARREL_WHITEDOT,
	LOT_SUMMARY_HEADER_NG_LENS_CONTAMINATION,
	LOT_SUMMARY_HEADER_NG_LENS_SCRATCH,
	LOT_SUMMARY_HEADER_NG_LENS_DIRT,
	LOT_SUMMARY_HEADER_NG_LENS_WHITEDOT,
	LOT_SUMMARY_HEADER_NG_EPOXYHOLE,
	LOT_SUMMARY_HEADER_NG_SPRINGNG,
	LOT_SUMMARY_HEADER_NG_FAING,
	LOT_SUMMARY_HEADER_NG_BLACKCOATINGNG,
	LOT_SUMMARY_HEADER_NG_MODULEMIX,
	LOT_SUMMARY_HEADER_NG_BARCODEERROR,
	LOT_SUMMARY_HEADER_NG_BARCODESHIFT,
	LOT_SUMMARY_HEADER_NG_MATCHINGERROR,
	LOT_SUMMARY_HEADER_NG_LIGHTERROR,
	LOT_SUMMARY_HEADER_END
};

static const CString g_sLotSummaryLogHeader[] =
{
	_T("Date")					,
	_T("Time")			,
	_T("Start_Time")			,
	_T("End_Time")				,
	_T("Station")				,
	_T("SW_Version")			,
	_T("Param_Version")			,
	_T("Config")				,
	_T("LotNum")					,
	_T("Cycle_Time")			,
	_T("InspectedCount")		,
	_T("OKCount")				,
	_T("OKRatio")				,
	_T("NGCount")				,
	_T("NGRatio")				,
	_T("BarrelNG")				,
	_T("LensNG")				,
	_T("BarrelLensNG")			,
	_T("Barrel_Dirt")			,
	_T("Barrel_Scratch")		,
	_T("Barrel_Stain")			,
	_T("Barrel_Dent")			,
	_T("Barrel_Chipping")		,
	_T("Barrel_Epoxy")			,
	_T("Barrel_OutsideCT")		,
	_T("Barrel_PeelOff")		,
	_T("Barrel_WhiteDot")		,
	_T("Lens_Contamination")	,
	_T("Lens_Scratch")			,
	_T("Lens_Dirt")				,
	_T("Lens_WhiteDot")			,
	_T("EpoxyHole")				,
	_T("SpringNG")				,
	_T("FAING")					,
	_T("BlackCoatingNG")		,
	_T("ModuleMix")				,
	_T("BarcodeError")			,
	_T("BarcodeShift")			,
	_T("MatchingError")			,
	_T("LightError")			,

};

enum eDefectFeatureLogHeader
{
	DSF_HEADER_DATE,
	DSF_HEADER_TIME,
	DSF_HEADER_STATION,
	DSF_HEADER_MACHINE,
	DSF_HEADER_LOTID,
	DSF_HEADER_CONFIG,
	DSF_HEADER_TRAYNO,
	DSF_HEADER_BARCODEID,
	DSF_HEADER_MODULENNO,
	DSF_HEADER_VISION,
	__SHAPE__,
	DSF_HEADER_END = 86
};

// MEMO : Auto Param. 86개 파라미터  - 250912, jhkim
// MEMO : 아래 항목 늘린 후 DSF_HEADER_END 업데이트 해야 함
static const CString g_sDefectFeatureLogHeader[] =
{
	_T("Date")			,
	_T("Time")			,
	_T("Station")		,
	_T("Machine_Code")	,
	_T("LotNum")		,
	_T("Config")		,  
	_T("TrayNo")		,
	_T("Barcode")		,
	_T("ModuleNo")		,
	_T("Vision")		,
	// Auto Param. Reorder - 250912, jhkim///////////
	_T("NG Area")		,
	_T("Image Type")	,
	_T("NG Defect Name")			,
	// (Auto Param.) Image No 추가 - 250912, jhkim
	_T("Image No")			,
	_T("Inspection Tab No")			,
	/////////////////////////////////////////////////
	_T("gv_min")		,
	_T("gv_max")		,
	_T("gv_median")		,
	_T("gv_mean")		,
	_T("gv_dev")		,
	_T("area")			,
	_T("row")			,
	_T("column")		,
	_T("length_x")			,
	_T("length_y")			,
	_T("row1")				,
	_T("column1")			,
	_T("row2")				,
	_T("column2")			,
	_T("center_x")			,
	_T("center_y")			,
	_T("circularity")		,
	_T("compactness")		,
	_T("contlength")		,
	_T("convexity")			,
	_T("rectangularity")	,
	_T("ra")				,
	_T("rb")				,
	_T("phi")				,
	_T("anisometry")		,
	_T("bulkiness")			,
	_T("struct_factor")		,
	_T("width")				,
	_T("height")			,
	_T("w/h")				,
	_T("inner_width")		,
	_T("inner_height")		,
	_T("dist_mean")			,
	_T("dist_deviation")	,
	_T("roundness")			,
	_T("num_sides")			,
	_T("connect_num")		,
	_T("holes_num")			,
	_T("area_holes")		,
	_T("max_diameter")		,
	_T("orientation")		,
	_T("euler_number")		,
	_T("rect2_phi")			,
	_T("rect2_len1")		,
	_T("rect2_len2")		,
	_T("moments_m11")			,
	_T("moments_m20")			,
	_T("moments_m02")			,
	_T("moments_ia")			,
	_T("moments_ib")			,
	_T("moments_m11_invar")			,
	_T("moments_m20_invar")			,
	_T("moments_m02_invar")			,
	_T("moments_phi1")			,
	_T("moments_phi2")			,
	_T("moments_m21")			,
	_T("moments_m12")			,
	_T("moments_m03")			,
	_T("moments_m30")			,
	_T("moments_m21_invar")			,
	_T("moments_m12_invar")			,
	_T("moments_m03_invar")			,
	_T("moments_m30_invar")			,
	_T("moments_i1")			,
	_T("moments_i2")			,
	_T("moments_i3")			,
	_T("moments_i4")			,
	_T("moments_psi1")			,
	_T("moments_psi2")			,
	_T("moments_psi3")			,
	_T("moments_psi4")			,
};

enum eFAILogHeader
{
	FAI_RESULT_HEADER_DATE,
	FAI_RESULT_HEADER_TIME,
	FAI_RESULT_HEADER_STATION,
	FAI_RESULT_HEADER_MACHINE,
	FAI_RESULT_HEADER_LOTID,
	FAI_RESULT_HEADER_MODEL,
	FAI_RESULT_HEADER_TRAYNO,
	FAI_RESULT_HEADER_MODULENO,
	FAI_RESULT_HEADER_INDEXNO,
	FAI_RESULT_HEADER_POCKETNO,
	FAI_RESULT_HEADER_BARCODEID,
	FAI_RESULT_HEADER_26_RESULT,
	FAI_RESULT_HEADER_26,
	FAI_RESULT_HEADER_27,
	FAI_RESULT_HEADER_27_ORG,
	FAI_RESULT_HEADER_28,
	FAI_RESULT_HEADER_28_ORG,
	FAI_RESULT_HEADER_51,
	FAI_RESULT_HEADER_52,
	FAI_RESULT_HEADER_52_ORG,
	FAI_RESULT_HEADER_53,
	FAI_RESULT_HEADER_53_ORG,
	FAI_RESULT_HEADER_INNER_X,
	FAI_RESULT_HEADER_INNER_Y,
	FAI_RESULT_HEADER_OUTER_X,
	FAI_RESULT_HEADER_OUTER_Y,
	FAI_RESULT_HEADER_DATUMA_X,
	FAI_RESULT_HEADER_DATUMA_Y,
	FAI_RESULT_HEADER_DATUMB_X,
	FAI_RESULT_HEADER_DATUMB_Y,
	FAI_RESULT_HEADER_INNER_DIAMETER,
	FAI_RESULT_HEADER_OUTER_DIAMETER,
	FAI_RESULT_HEADER_DATUMA_DIAMETER,
	FAI_RESULT_HEADER_INNER_EDGESTRENGTH_P1,
	FAI_RESULT_HEADER_INNER_EDGESTRENGTH_P2,
	FAI_RESULT_HEADER_INNER_EDGESTRENGTH_P3,
	FAI_RESULT_HEADER_INNER_EDGESTRENGTH_P4,
	FAI_RESULT_HEADER_INNER_EDGESTRENGTH_P5,
	FAI_RESULT_HEADER_INNER_EDGESTRENGTH_P6,
	FAI_RESULT_HEADER_INNER_EDGESTRENGTH_P7,
	FAI_RESULT_HEADER_INNER_EDGESTRENGTH_P8,
	FAI_RESULT_HEADER_OUTER_EDGESTRENGTH_P1,
	FAI_RESULT_HEADER_OUTER_EDGESTRENGTH_P2,
	FAI_RESULT_HEADER_OUTER_EDGESTRENGTH_P3,
	FAI_RESULT_HEADER_OUTER_EDGESTRENGTH_P4,
	FAI_RESULT_HEADER_OUTER_EDGESTRENGTH_P5,
	FAI_RESULT_HEADER_OUTER_EDGESTRENGTH_P6,
	FAI_RESULT_HEADER_OUTER_EDGESTRENGTH_P7,
	FAI_RESULT_HEADER_OUTER_EDGESTRENGTH_P8,
	FAI_RESULT_HEADER_DATUMA_EDGESTRENGTH_P1,
	FAI_RESULT_HEADER_DATUMA_EDGESTRENGTH_P2,
	FAI_RESULT_HEADER_DATUMA_EDGESTRENGTH_P3,
	FAI_RESULT_HEADER_DATUMA_EDGESTRENGTH_P4,
	FAI_RESULT_HEADER_DATUMA_EDGESTRENGTH_P5,
	FAI_RESULT_HEADER_DATUMA_EDGESTRENGTH_P6,
	FAI_RESULT_HEADER_DATUMA_EDGESTRENGTH_P7,
	FAI_RESULT_HEADER_DATUMA_EDGESTRENGTH_P8,
	FAI_RESULT_HEADER_END
};

static const CString g_sFAIResultLogHeader[] =
{
	_T("Date")			,
	_T("Time")			,
	_T("Station")		,
	_T("Machine_Code")	,
	_T("LotNum")			,
	_T("Model")			,
	_T("TrayNo")		,
	_T("ModuleNo")		,
	_T("IndexNo")		,
	_T("PocketNo")		,
	_T("Barcode")		,
	_T("FAI-26 Result")		,
	_T("FAI-26(mm)")		,
	_T("FAI-26-X(mm)")		,
	_T("FAI-26-X-Org(mm)")	,
	_T("FAI-26-Y(mm)")		,
	_T("FAI-26-Y-Org(mm)")	,
	_T("FAI-51(mm)")		,
	_T("FAI-51-X(mm)")		,
	_T("FAI-51-X-Org(mm)")	,
	_T("FAI-51-Y(mm)")		,
	_T("FAI-51-Y-Org(mm)")	,
	_T("Inner CenterX(px)"),
	_T("Inner CenterY(px)"),
	_T("Outer CenterX(px)"),
	_T("Outer CenterY(px)"),
	_T("DatumA CenterX(px)"),
	_T("DatumA CenterY(px)"),
	_T("DatumB CenterX(px)"),
	_T("DatumB CenterY(px)"),
	_T("Inner Diameter(mm)"),
	_T("Outer Diameter(mm)"),
	_T("DatumA Diameter(mm)"),
	_T("Inner1 ES"),
	_T("Inner2 ES"),
	_T("Inner3 ES"),
	_T("Inner4 ES"),
	_T("Inner5 ES"),
	_T("Inner6 ES"),
	_T("Inner7 ES"),
	_T("Inner8 ES"),
	_T("Outer1 ES"),
	_T("Outer2 ES"),
	_T("Outer3 ES"),
	_T("Outer4 ES"),
	_T("Outer5 ES"),
	_T("Outer6 ES"),
	_T("Outer7 ES"),
	_T("Outer8 ES"),
	_T("DatumA1 ES"),
	_T("DatumA2 ES"),
	_T("DatumA3 ES"),
	_T("DatumA4 ES"),
	_T("DatumA5 ES"),
	_T("DatumA6 ES"),
	_T("DatumA7 ES"),
	_T("DatumA8 ES"),
	 
};

enum eFAILogHeader_Welding
{
	FAI_WD_RESULT_HEADER_DATE,
	FAI_WD_RESULT_HEADER_TIME,
	FAI_WD_RESULT_HEADER_STATION,
	FAI_WD_RESULT_HEADER_MACHINE,
	FAI_WD_RESULT_HEADER_LOTID,
	FAI_WD_RESULT_HEADER_MODEL,
	FAI_WD_RESULT_HEADER_TRAYNO,
	FAI_WD_RESULT_HEADER_MODULENO,
	FAI_WD_RESULT_HEADER_INDEXNO,
	FAI_WD_RESULT_HEADER_POCKETNO,
	FAI_WD_RESULT_HEADER_BARCODEID,
	FAI_WD_RESULT_HEADER_14_RESULT,
	FAI_WD_RESULT_HEADER_14,
	FAI_WD_RESULT_HEADER_14_X,
	FAI_WD_RESULT_HEADER_14_X_ORG,
	FAI_WD_RESULT_HEADER_14_Y,
	FAI_WD_RESULT_HEADER_14_Y_ORG,
	FAI_WD_RESULT_HEADER_ROTATION_RESULT,
	FAI_WD_RESULT_HEADER_ROTATION,
	FAI_WD_RESULT_HEADER_ROTATION_ORG,
	FAI_WD_RESULT_HEADER_WELDING_EA_RESULT,
	FAI_WD_RESULT_HEADER_WELDING_EA,
	FAI_WD_RESULT_HEADER_WELDING_DIAMETER_RESULT,
	FAI_WD_RESULT_HEADER_WELDING_S1_DIAMETER,
	FAI_WD_RESULT_HEADER_WELDING_S2_DIAMETER,
	FAI_WD_RESULT_HEADER_WELDING_S3_DIAMETER,
	FAI_WD_RESULT_HEADER_WELDING_S4_DIAMETER,
	FAI_WD_RESULT_HEADER_WELDING_S1_POSITION,
	FAI_WD_RESULT_HEADER_WELDING_S2_POSITION,
	FAI_WD_RESULT_HEADER_WELDING_S3_POSITION,
	FAI_WD_RESULT_HEADER_WELDING_S4_POSITION,
	FAI_WD_RESULT_HEADER_WELDING_S5_POSITION,
	FAI_WD_RESULT_HEADER_LENS_RAD,
	FAI_WD_RESULT_HEADER_LENS_X,
	FAI_WD_RESULT_HEADER_LENS_Y,
	FAI_WD_RESULT_HEADER_CHASIS_NORTH_X,
	FAI_WD_RESULT_HEADER_CHASIS_NORTH_Y,
	FAI_WD_RESULT_HEADER_CHASIS_SOUTH_X,
	FAI_WD_RESULT_HEADER_CHASIS_SOUTH_Y,
	FAI_WD_RESULT_HEADER_INNER_EDGESTRENGTH_P1,
	FAI_WD_RESULT_HEADER_INNER_EDGESTRENGTH_P2,
	FAI_WD_RESULT_HEADER_INNER_EDGESTRENGTH_P3,
	FAI_WD_RESULT_HEADER_INNER_EDGESTRENGTH_P4,
	FAI_WD_RESULT_HEADER_INNER_EDGESTRENGTH_P5,
	FAI_WD_RESULT_HEADER_INNER_EDGESTRENGTH_P6,
	FAI_WD_RESULT_HEADER_INNER_EDGESTRENGTH_P7,
	FAI_WD_RESULT_HEADER_INNER_EDGESTRENGTH_P8,
	FAI_WD_RESULT_HEADER_END
};

static const CString g_sFAIResultLogHeader_Welding[] =
{
	_T("Date")			,
	_T("Time")			,
	_T("Station")		,
	_T("Machine_Code")	,
	_T("LotNum")			,
	_T("Model")			,
	_T("TrayNo")		,
	_T("ModuleNo")		,
	_T("IndexNo")		,
	_T("PocketNo")		,
	_T("Barcode")		,
	_T("동심도 Result")		,
	_T("동심도(mm)")			,
	_T("동심도 X(mm)")		,
	_T("동심도 X Org(mm)")	,
	_T("동심도 Y(mm)")		,
	_T("동심도 Y Org(mm)")	,
	_T("Rotation Result")	,
	_T("Rotation(deg)")	,
	_T("Rotation Org(deg)")	,
	_T("Welding Spot EA Result"),
	_T("Welding Spot EA")		,
	_T("Welding Spot Dia Result"),
	_T("Welding S1 Dia"),
	_T("Welding S2 Dia"),
	_T("Welding S3 Dia"),
	_T("Welding S4 Dia"),
	_T("Welding S1 Pos"),
	_T("Welding S2 Pos"),
	_T("Welding S3 Pos"),
	_T("Welding S4 Pos"),
	_T("Welding S5 Pos"),
	_T("Lens-Rad(mm)")	,
	_T("Lens-X(px)")		,
	_T("Lens-Y(px)")		,
	_T("Chasis-North-X(px)"),
	_T("Chasis-North-Y(px)"),
	_T("Chasis-South-X(px)"),
	_T("Chasis-South-Y(px)"),
	_T("Inner1 ES"),
	_T("Inner2 ES"),
	_T("Inner3 ES"),
	_T("Inner4 ES"),
	_T("Inner5 ES"),
	_T("Inner6 ES"),
	_T("Inner7 ES"),
	_T("Inner8 ES")
};

// MEMO : Auto Param.  - 250912, jhkim
enum eDetectParamLogHeader
{
	PARAM_HEADER_TIME,
	PARAM_HEADER_STATION,
	PARAM_HEADER_MACHINE,
	PARAM_HEADER_LOGIN_MODE,
	PARAM_HEADER_OPERATOR,
	PARAM_HEADER_RECIPE,
	PARAM_HEADER_SW_VERSION,
	PARAM_HEADER_LOTID,
	PARAM_HEADER_TRAYNO,
	PARAM_HEADER_MODULENNO,
	PARAM_HEADER_SENSORID,
	PARAM_HEADER_BARCODEID,
	PARAM_HEADER_POSITION,
	PARAM_HEADER_IMAGETYPE,
	__PARAM__,
	PARAM_HEADER_END = 159
};

// MEMO : Auto Param. - 250912, jhkim
// MEMO : 아래 헤더 추가 한 후 PARAM_HEADER_END 업데이트 해야 함 - 250912, jhkim
static const CString g_sDetectParamLogHeader[] =
{
	_T("Time")			,
	_T("Station")		,
	_T("Machine_Code")	,
	_T("Login_Mode")	,
	_T("Operator")		,
	_T("Recipe")		,
	_T("SW_Version")	,
	_T("LotNum")		,
	_T("TrayNo")		,
	_T("ModuleNo")		,
	_T("SensorID")		,
	_T("Barcode")		,
	// Auto Param. Reorder - 250912, jhkim //////////
	_T("NG Area")		,
	_T("Image Type")	,
	_T("NG Defect Name")			,
	// (Auto Param.) Image No 추가 - 250912, jhkim
	_T("Image No")					,
	_T("Inspection Tab No")			,
	/////////////////////////////////////////////////
	_T("Z Axis")					,
	_T("Light Top Dome Page")		,
	_T("Light Top/Angle Ring Page")	,
	_T("Light Top Dome")			,
	_T("Light Top Ring")			,
	_T("Light 55 Ring")				,
	_T("Light 65 Ring")				,
	_T("Light 75 Ring")				,
	_T("B/L ROI Refer X")			,
	_T("B/L ROI Refer Y")			,
	_T("Barrel ROI Refer Rad")		,
	_T("Lens ROI Refer Rad")		,
	_T("B/L ROI Scale Use")			,
	_T("B/L ROI Scale Min")			,
	_T("B/L ROI Scale Max")			,
	_T("B/L ROI Matching Image")			,
	_T("Barrel ROI Matching Contrast")		,
	_T("Lens ROI Matching Contrast")		,
	_T("Inner Barrel Margin Outer")			,
	_T("Inner Barrel Margin Inner")			,
	_T("Barcode Shift Insp Use")			,
	_T("Barcode Shift Refer X")				,
	_T("Barcode Shift Tol X")				,
	_T("Barcode Shift Refer Y")				,
	_T("Barcode Shift Tol Y")				,
	_T("Barcode Rotation Insp Use")			,
	_T("Barcode Rotation Refer R")			,
	_T("Barcode Rotation Tol R")			,
	_T("Image Align Use")					,
	_T("Image Align Target")				,
	_T("Image Align Pyramid")				,
	_T("Image Align Angle")					,
	_T("Image Align Score")					,
	_T("Image Align Search X")				,
	_T("Image Align Search Y")				,
	_T("Insp Area 1 Use")					,
	_T("Insp Area 1 ROI 1")					,
	_T("Insp Area 1 Margin 1")				,
	_T("Insp Area 1 ROI 2")					,
	_T("Insp Area 1 Margin 2")				,
	_T("Insp Area 2 Use")					,
	_T("Insp Area 2 ROI 1")					,
	_T("Insp Area 2 Margin 1")				,
	_T("Insp Area 2 ROI 2")					,
	_T("Insp Area 2 Margin 2")				,
	_T("Insp Area 3 Use")					,
	_T("Insp Area 3 ROI 1")					,
	_T("Insp Area 3 Margin 1")				,
	_T("Insp Area 3 ROI 2")					,
	_T("Insp Area 3 Margin 2")				,
	_T("Bright Fixed Thres Use")			,
	_T("Bright Fixed Thres Lower")			,
	_T("Bright Fixed Thres Upper")			,
	_T("Bright Dyn Thres Use")				,
	_T("Bright Dyn Thres Nom Use")			,
	_T("Bright Dyn Thres Median Size")		,
	_T("Bright Dyn Thres Closing Size")		,
	_T("Bright Dyn Thres Filter Size")		,
	_T("Bright Dyn Thres Offset")			,
	_T("Bright Uni Thres Use")				,
	_T("Bright Uni Thres Offset")			,
	_T("Bright Uni Thres Hyst Length")		,
	_T("Bright Uni Thres Hyst Offset")		,
	_T("Bright Hyst Thres Use")				,
	_T("Bright Hyst Thres Scure")			,
	_T("Bright Hyst Thres Potential")		,
	_T("Bright Hyst Thres Length")			,
	_T("Dark Fixed Thres Use")				,
	_T("Dark Fixed Thres Lower")			,
	_T("Dark Fixed Thres Upper")			,
	_T("Dark Dyn Thres Use")				,
	_T("Dark Dyn Thres Nom Use")			,
	_T("Dark Dyn Thres Median Size")		,
	_T("Dark Dyn Thres Closing Size")		,
	_T("Dark Dyn Thres Filter Size")		,
	_T("Dark Dyn Thres Offset")				,
	_T("Dark Uni Thres Use")				,
	_T("Dark Uni Thres Offset")				,
	_T("Dark Uni Thres Hyst Length")		,
	_T("Dark Uni Thres Hyst Offset")		,
	_T("Dark Hyst Thres Use")				,
	_T("Dark Hyst Thres Scure")				,
	_T("Dark Hyst Thres Potential")			,
	_T("Dark Hyst Thres Length")			,
	_T("Combine Bright Dark Blob Use")		,
	_T("Opening Size")						,
	_T("Closing Size")						,
	_T("Fill Up Blob Use")					,
	_T("Connection Blob Use")				,
	_T("Connection Min Size")				,
	_T("Connection Length")					,
	_T("Connection Angle Use")				,
	_T("Connection Max Width")				,
	_T("Condition Area Use")				,
	_T("Condition Area Min")				,
	_T("Condition Area Max")				,
	_T("Condition Length Use")				,
	_T("Condition Length Min")				,
	_T("Condition Length Max")				,
	_T("Condition Width Use")				,
	_T("Condition Width Min")				,
	_T("Condition Width Max")				,
	_T("Condition X Size Use")				,
	_T("Condition X Size Min")				,
	_T("Condition X Size Max")				,
	_T("Condition Y Size Use")				,
	_T("Condition Y Size Min")				,
	_T("Condition Y Size Max")				,
	_T("Condition GV Mean Use")				,
	_T("Condition GV Mean Min")				,
	_T("Condition GV Mean Max")				,
	_T("Condition GV Std Use")				,
	_T("Condition GV Std Min")				,
	_T("Condition GV Std Max")				,
	_T("Condition Length/Width Use")		,
	_T("Condition Length/Width Min")		,
	_T("Condition Length/Width Max")		,
	_T("Condition Circularity Use")			,
	_T("Condition Circularity Min")			,
	_T("Condition Circularity Max")			,
	_T("Condition Convexity Use")			,
	_T("Condition Convexity Min")			,
	_T("Condition Convexity Max")			,
	_T("Condition Major/Minor Use")			,
	_T("Condition Major/Minor Min")			,
	_T("Condition Major/Minor Max")			,
	_T("Condition Contrast Use")			,
	_T("Condition Contrast Min")			,
	_T("Condition Contrast Max")			,
	_T("Condition Outer Dist Use")			,
	_T("Condition Outer Dist Min")			,
	_T("Condition Inner Dist Use")			,
	_T("Condition Inner Dist Min")			,
	_T("FT Connected Only Use")				,
	_T("FT Connected Only Area Use")		,
	_T("FT Connected Only Area Min")		,
	_T("FT Connected Only Length Use")		,
	_T("FT Connected Only Length Min")		,
	_T("FT Connected Only Width Use")		,
	_T("FT Connected Only Width Min")		,
	_T("Dont Care Apply Use")				,
	_T("Dont Care Use")						,
};

#define BARCODE_MAX_NUM	10

#define ROYAL_BLUE	RGB(0x86, 0xAC, 0xE3)
#define ROYAL_BLUE	RGB(0x86, 0xAC, 0xE3)
#define OLIVE_GREEN RGB(0xB5, 0xC4, 0x91)
#define TRANSPARENT RGB(0x00, 0x00, 0x00)
#define BLACK		RGB(1, 1, 1)
#define WHITE		RGB(255, 255, 255)
#define RED			RGB(255, 1, 1)
#define CYAN		RGB(0, 255, 255)
#define MAGENTA		RGB(255, 0, 255)
#define MAGENTA		RGB(255, 255, 0)
#define DARK_GRAY	RGB(169, 169, 169)

#endif

