#pragma once

/**
 * C3 1Side Build Version Automation - 250917, jhkim
 * 구성 관리자로부터 매크로를 받아서 빌드 버전을 자동으로 설정.
 */

 /**
  * [WELDING]
  */
#ifdef WELDING
#define M10
#define RX
#define PG
#define VT
#define WELDING_CAM
 /**
  * [M10_RD_PG_HT]
  */
#elif M10_RD_PG_HT
#define M10
#define RD
#define PG
#define HT
/**
 * [M10_RD_PG_VT]
 */
#elif M10_RD_PG_VT
#define M10
#define RD
#define PG
#define VT
/**
 * [M10_SO_PG_VT]
 */
#elif M10_SO_PG_VT
#define M10
#define SO
#define PG
#define VT
/**
 * [M9_SO_PG_HT] 사용 호기 - update 250917, jhkim
 * BWI 1Side #110 : 169.254.15.180 (다른 장비와 망이 다름, 접속 불가)
 */
#elif M9_SO_PG_HT
#define M9
#define SO
#define PG
#define HT
#endif

#ifndef DEBUG
// INLINE_MODE
#define INLINE_MODE

// 그 외 무조건 INLINE_MODE 에서 켜져있는 매크로
#define AJIN_BOARD_USE
#define IO_USE
#define RS232_USE
#define MOTION_USE
#define HANDLER_USE
#define GRAB_USE

/**
 * Matrox MIL 버전 설명
 *
 * C3에 Matrox 버전 다른 기기가 하나 있음.
 * 해당 기기는 빌드 시 MIL버전을 9.0 으로 맞춰줘야 함
 * 그 외에는 10.0 버전 사용.
 *
 * M10 or M9 두 종류.
 */

// 현재 Mil 버전은 프로젝트 속성 -> 추가 포함 디렉토리에서 따로 설정하고 있음. (프로젝트 속성 참고)
// 아래 구문은 맥락상 추가한 부분. Please Dont Care.
#ifdef M10 // MIL_VERSION_9
#else      // MIL_VERSION_9
#endif

/**
 * 그래버 보드 설명
 * GRABBER_RAPIXOCXP_USE : 거의 웰딩에만 씀, C3에는 없음.
 *                         6면은 대부분 이거 씀. (해당 장비가 이미지 버퍼가 커서 그런듯)
 *                         웰딩장비 이미지 크기는 4k x 4k 라서 이거 씀.
 * GRABBER_RADIENT_USE   : 대부분 C3는 해당 매크로 사용. (그랩 속도나 버퍼등등 스펙이 좀 다른듯)
 * GRABBER_SOLIOS_USE    : 단종되서 거의 사라짐. 지금 두 대밖에 없음.
 */

#ifdef RX
#define GRABBER_RAPIXOCXP_USE // RX
#endif
#ifdef RD
#define GRABBER_RADIENT_USE // RD
#endif
#ifdef SO
#define GRABBER_SOLIOS_USE // SO
#endif

/**
 * 바코드 카메라 설명
 * BARCODE_CAM_POINTGRAY_USE : C3 대부분에 있는 포인트 그레이 바코드 카메라.
 * BARCODE_CAM_CREVIS_USE    : 크레비스 바코드 카메라. 주로 베트남에 있음.
 */
#ifdef PG
#define BARCODE_CAM_POINTGRAY_USE // PG
#else                             // CV
#define BARCODE_CAM_CREVIS_USE    // CV
#endif
#endif

/**
 * Trigger 설명
 * Trigger 는 두 종류
 * Handler Trigger : Vision Trigger 주석처리 되면 Handler Trigger.
 *                   Handler 에 IO 가 연결되서 조명, 등등 처리. (통신딜레이가 대략 25ms 정도)
 *
 * Vision Trigger : Vision 에서 IO 처리하는 것. (통신 딜레이 대략 7ms 정도)
 *                  대부분의 장비는 Vision Trigger 사용 중
 */
#ifdef VT
#define VISION_TRIGGER // VT or HT
#endif
