#ifndef _CHOBITS_API_H_
#define _CHOBITS_API_H_

/*
 * INCLUDE FILES
 */
#include "../../types.h"
#include "../../video_def.h"		/* for direct y */

#include "../../syscall_type.h"
#include "../../key_def.h"			/* key definitions */
#include "../../string.h"			/* import string library : strlen, etc... */
#include "../../vsprintf.h"			/* sprintf, vsprintf */

/*
 * API CALLs
 */

/* system api */
VOID API_ExitProgram(VOID);
DWORD API_GetTickCount(VOID);
VOID API_Delay(DWORD MilliSec); /* 1000 = 1sec */
VOID API_ShowTSWatchdogClock(VOID);
VOID API_HideTSWatchdogClock(VOID);

/* screen associated */
VOID API_ClearScreen(VOID);
VOID API_PrintText(BYTE *pText);
VOID API_PrintTextXY(BYTE *pText, WORD x, WORD y);
VOID API_PrintTextWithAttr(BYTE *pText, BYTE Attr);
VOID API_PrintTextXYWithAttr(BYTE *pText, WORD x, WORD y, BYTE Attr);

/* keyboard api */
BOOL API_HasKey(VOID);
BOOL API_GetKey(KBD_KEY_DATA *pKeyData);
VOID API_FlushKbdBuf(VOID);

/* direct y api */
BOOL			API_SetVideoMode(DY_VIDEO_MODE mode, HANDLE hPalette);
DY_VIDEO_MODE	API_GetCurrentVideoMode(VOID);

HANDLE			API_LoadBitmap(char *pFilename);
HANDLE			API_GetPaletteHandle(HANDLE hBitmap);
BOOL			API_GetBitmapInfo(HANDLE hBitmap, BITMAP_INFO *pBitmapInfo);
BOOL			API_BitBlt(HANDLE hBitmap, WORD scrX, WORD scrY, DWORD startImgX, DWORD startImgY,
						   DWORD cxToBeDisplayed, DWORD cyToBeDisplayed, RGB_COLOR *pMaskColor); /* pMaskColor = optional */
VOID			API_CloseBitmapHandle(HANDLE hBitmap);


#endif /* #ifndef _CHOBITS_API_H_ */