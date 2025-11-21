#ifndef DllUnitH
#define DllUnitH

#ifdef __DLL__
#define INPEXP __declspec(dllexport)
#else
#define INPEXP __declspec(dllimport)
#endif

extern "C"
{
///------------------------------------------------------------------------------------
///         Trend & Histogram Chart Functions
///------------------------------------------------------------------------------------
    bool INPEXP ChartDlgShow(RECT RectSize, HWND hWnd); // 차트 생성, Dlg의 위치와 크기지정, Parent의 핸들
    bool INPEXP ChartDlgDestroy(void);                  // 파괴 
    bool INPEXP ChartDlgGetSize(int *left, int *top, int *width, int *height);
///    void INPEXP ChartDlgSetSize(int left, int top, int width, int height);
    bool INPEXP ChartDlgAddXY(int Sel, double Xval, double Yval);   // (X, Y)한쌍의 데이타 그리기
    bool INPEXP ChartDlgClear(int Sel);                             //  차트를 Clear 
    bool INPEXP ChartDlgBeginUpdate(int Sel);       // for high speed
    bool INPEXP ChartDlgEndUpdate(int Sel);         // for high speed
    bool INPEXP ChartDlgSetAxisLimit(int Sel, int Axistype, double cMin, double cMax, double Increment); // 축 스케일, 증가치 지정
    bool INPEXP ChartDlgSetAxisAuto(int Sel, int Axistype);         // 축 스케일 자동
    bool INPEXP ChartDlgSetUCLLCL(int Sel, double UCLval, double LCLval, bool bDraw);   // UCL, LCL값 지정
    bool INPEXP ChartDlgSetStandard(int Sel, double Std, bool bDraw);                   // Standard값 지정
    bool INPEXP ChartDlgCalHistoUp(int Grade);      //   위쪽 히스토그램 그레이드 단계 지정해서 보여주기
    bool INPEXP ChartDlgCalHistoDown(int Grade);    // 아래쪽
    bool INPEXP ChartDlgDeleteIndex(int Sel, int Index);  // 인덱스의 그래프 데이타 지우기
    int  INPEXP ChartDlgGetDataCount(int Sel);            // 선택된 그래프의 데이타 총개수
    bool INPEXP ChartDlgGetMinData(int Sel, double *Xval, double *Yval, double *Zval);   // 선택 그래프의 데이타최소값
    bool INPEXP ChartDlgGetMaxData(int Sel, double *Xval, double *Yval, double *Zval);   // 선택 그래프의 데이타최대값
    bool INPEXP ChartDlgPointPage(int MaxPoints, int PointPerPage);

//------------ Title function ------------------
    bool INPEXP ChartDlgTitle(int Sel, char Title[]);                    // 차트 전체 타이틀을 표시 
    bool INPEXP ChartDlgAxisTitle(int Sel, int AxisType, char Title[]);     // 각 축 타이틀을 표시 
    bool INPEXP ChartDlgCalculateAvgSD(int Sel, double *Avg, double *Sdev);       // Avg, SD 계산합니다. 내부 멤버 Update 
    bool INPEXP ChartDlgDispAvgSD(int Sel, bool bDraw);                           // 내부 멤버인 Avg, SD를 차트우측하단에 Display
/// ---- Add ver 1.5 : invert axis direction
    bool INPEXP ChartDlgAxisInvert(int AxisType, bool bInverted);

///------------------------------------------------------------------------------------
///         Scatter Chart Functions
///------------------------------------------------------------------------------------

    bool INPEXP ScatterDlgShow(RECT RectSize, HWND hWnd);       // 생성, 사이즈와 부모핸들 
    bool INPEXP ScatterDlgDestroy(void);                        //  파괴 
    bool INPEXP ScatterDlgGetSize(int *left, int *top, int *width, int *height);
///    void INPEXP ScatterDlgSetSize(int left, int top, int width, int height);
    bool INPEXP ScatterDlgAddXY(double Xval, double Yval);      // 한쌍의 데이타 그리기 
    bool INPEXP ScatterDlgClear(void);                          // 차트 Clear 
    bool INPEXP ScatterDlgBeginUpdate(void);        // for high speed
    bool INPEXP ScatterDlgEndUpdate(void);          // for high speed
    bool INPEXP ScatterDlgSetAxisLimit(int Axistype, double cMin, double cMax, double Increment); // 축의 최소, 최대, 증가치 지정 
    bool INPEXP ScatterDlgSetAxisAuto(int Axistype);                    // 축..자동 
    bool INPEXP ScatterDlgSetUCLLCL(int Axis, double UCLval, double LCLval, bool bDraw);
    bool INPEXP ScatterDlgDeleteIndex(int Index);                       // 인덱스의 데이타 한쌍 지우기
    int  INPEXP ScatterDlgGetDataCount(void);                           // 데이타의 총개수 
    bool INPEXP ScatterDlgGetMinData(double *Xval, double *Yval, double *Zval);    // 각축의 최소데이타값 
    bool INPEXP ScatterDlgGetMaxData(double *Xval, double *Yval, double *Zval);    // 각축의 최대 데이타값 
//------------ Title function ------------------
    bool INPEXP ScatterDlgTitle(char Title[]);                              // 차트 전체 타이틀을 표시 
    bool INPEXP ScatterDlgAxisTitle(int AxisType, char Title[]);            // 각 축 타이틀을 표시 
    bool INPEXP ScatterDlgDispAvgPoint(bool bDraw);               // 차트 데이타의 평균X, Y를 계산 Cp를 십자표시, 수치는 우측하단표시

/// ---- Add ver 1.5 : invert axis direction
    bool INPEXP ScatterDlgAxisInvert(int AxisType, bool bInverted);
}

#endif
