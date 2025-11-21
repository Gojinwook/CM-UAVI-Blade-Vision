#ifndef _PImage_H_
#define _PImage_H_

#include <windows.h>
#include "PBaseImage.h"

class 
//#ifdef _MAKEDLL
  AFX_EXT_CLASS
//#endif
PImage : public PBaseImage
{
   private:       
     void GetBMObject(BITMAP *pBitmap);
     void MakeValidRect(BITMAP *pDestBM, BITMAP *pSrcBM, RECT *pDestRect, RECT *pSrcRect);
     void FillRect24(void *vpDest, int iDestHeight, int iDestPitch, RECT DestRect, int iColor);        
     void Blt24BM(BITMAP *pDestBM, BITMAP *pSrcBM, RECT DestRect, RECT SrcRect, int iROP);     
     void AlphaBlt24BM(BITMAP *pDestBM, BITMAP *pSrcBM, RECT DestRect, RECT SrcRect, BYTE BAlpha, int iROP);

     void ReverseBmBits(void *vpBits, void *vpReverseBits, int iPitch, int iHeight);

     friend class PImage;
     friend class PRegion;

   public:
     HBITMAP mhBitmap;

     PImage();
     ~PImage();

     bool IsValid(){ return (mhBitmap) ? true : false; }
     
     COLORREF GetColorKey();
     void SetColorKey(COLORREF ColorKey);
     
     HDC GetDC(HWND hWnd);
     void ReleaseDC(HDC hDC);
     
     void CreatePaletteTable();
     void Create(int iWidth, int iHeight,int iBitCount, void *vpBits); 
     void Clear();
     void FillZero();
     void SetPixel(int iX, int iY, COLORREF Color);
     void FillRect(int iLeft, int iTop, int iRight, int iBottom, COLORREF Color);

     void ZoomImage(PImage *pZoomImage, double dXFactor, double dYFactor);

     void DCBlt(HDC hDestDC, int iDX, int iDY, unsigned int iROP);
     void DCStretchBlt(HDC hDestDC, int iDX, int iDY, int iWidht, int iHeight, unsigned int iROP);
     void DCStretchBlt(HDC hDestDC, int iDX, int iDY, int iDWidht, int iDHeight, 
                       int iSX, int iSY, int iSWidth, int iSHeight, unsigned int iROP);
     void Blt(PImage &rDestImage, int iDX, int iDY, int iROP);
     void Blt(PImage &rDestImage, int iDX, int iDY, int iWidth, int iHeight, int iSX, int iSY, int iROP);
     //void StretchBlt(HDC hDestDC, int iDX, int iDY, int iWidht, int iHeight, unsigned int iROP);
     //void StretchBlt(HDC hDestDC, int iDX, int iDY, int iDWidht, int iDHeight, 
     //                  int iSX, int iSY, int iSWidth, int iSHeight, unsigned int iROP);
     void AlphaBlt(PImage &rDestImage, int iDX, int iDY, BYTE BAlpha, int iROP);
     void AlphaBlt(PImage &rDestImage, int iDX, int iDY, int iWidth, int iHeight, int iSX, int iSY, BYTE BAlpha, int iROP);

     bool LoadBMP(HANDLE hFile);
     bool SaveBMP(HANDLE hFile);

     bool LoadImage(char *cpFileName);
     bool SaveImage(char *cpFileName, int iPIFF);
};

#endif