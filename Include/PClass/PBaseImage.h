// -----------------------------------------------------------------------------
// 작성자        : 박지수
// 작성날짜      : 1999/1/7
// 최종수정날짜  : 1999/3/2
// 파일이름      : PImage.h
// 파일 설명     : 2D Image(Bitmap)구현의 핵심 Class-Inline Assemblyer로
//                 Blt, AlphaBlt을 지원한다.
// 관련 파일     : PImage.cpp
// 포함된 클래스 : PImage
// -----------------------------------------------------------------------------
#ifndef _PBASEIMAGE_H_
#define _PBASEIMAGE_H_

#include <windows.h>

// PImage ROP
#define PIROP_SRCCOPY         1
#define PIROP_SRCTRANSPARENT  2  

// PImage File Format
#define PIFF_BMP     1

// PImage
// 2D Image(Bitmap)구현의 핵심 클레스로 PImage의 Parent Class이다.
class 
//#ifdef _MAKEDLL
  AFX_EXT_CLASS
//#endif
PBaseImage
{
  protected:    
    COLORREF dwColorKey;        
     
    void AsmSetPixel24(void *vpDest, unsigned int iColor);
    void AsmFillRect24(void *vpDest, int iWidth, int iHeight, int iAddDPos, unsigned int iColor);   

    void AsmBlt24SrcCopy(void *vpDest, void *vpSrc, int iWidth, int iHeight,
                         int iAddDPos, int iAddSPos);
    void AsmBlt24SrcTransparent(void *vpDest, void *vpSrc, int iWidth, int iHeight,
                                int iAddDPos, int iAddSPos, unsigned int iColorKey);
    void AsmAlphaBlt24SrcCopy(void *vpDest, void *vpSrc, int iWidth, int iHeight,
                              int iAddDPos, int iAddSPos,BYTE BAlpha);
    void AsmAlphaBlt24SrcTransparent(void *vpDest, void *vpSrc, int iWidth, int iHeight,
                                     int iAddDPos, int iAddSPos, BYTE BAlpha, unsigned int iColorKey);

  public:   
    BITMAPINFO *mpBitmapInfo;
                
    PBaseImage();
    ~PBaseImage();
    virtual void Clear();
    
    int GetWidth();
    int GetHeight();           
};

#endif