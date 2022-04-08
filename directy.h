#ifndef _DIRECT_Y_H_
#define _DIRECT_Y_H_

/**********************************************************************************************************
 *                                                INCLUDE FILES                                           *
 **********************************************************************************************************/
#include "chobits.h"


/**********************************************************************************************************
 *                                               EXPORTED FUNCTIONS                                       *
 **********************************************************************************************************/
KERNELAPI BOOL			DySetVideoMode(DY_VIDEO_MODE mode, HANDLE hPalette);
KERNELAPI DY_VIDEO_MODE	DyGetCurrentVideoMode(VOID);

KERNELAPI VOID			DyPutPixel(WORD scrX, WORD scrY, RGB_COLOR color);
KERNELAPI VOID			DyDrawLine(WORD startX, WORD startY, WORD endX, WORD endY, RGB_COLOR color);
KERNELAPI VOID			DyDrawRect(WORD left, WORD top, WORD right, WORD bottom, RGB_COLOR lineColor);
KERNELAPI VOID			DyFillRect(WORD left, WORD top, WORD right, WORD bottom, RGB_COLOR fillColor);

KERNELAPI HANDLE		DyLoadBitmap(char *pFilename);
KERNELAPI HANDLE		DyGetPaletteHandle(HANDLE hBitmap);
KERNELAPI BOOL			DyGetBitmapInfo(HANDLE hBitmap, BITMAP_INFO *pBitmapInfo);
KERNELAPI BOOL			DyBitBlt(HANDLE hBitmap, WORD scrX, WORD scrY, DWORD startImgX, DWORD startImgY,
								 DWORD cxToBeDisplayed, DWORD cyToBeDisplayed, RGB_COLOR *pMaskColor); /* pMaskColor = optional */
KERNELAPI VOID			DyCloseBitmapHandle(HANDLE hBitmap);


#endif // #ifndef _DIRECT_Y_H_