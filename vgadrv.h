#ifndef _VGA_DRIVER_HEADER_FILE_
#define _VGA_DRIVER_HEADER_FILE_


/**********************************************************************************************************
 *                                                INCLUDE FILES                                           *
 **********************************************************************************************************/
#include "chobits.h"


/**********************************************************************************************************
 *                                               DRIVER FUNCTIONS                                         *
 **********************************************************************************************************/
KERNELAPI BOOL VgaSetVideoMode(VGA_VIDEO_MODE mode);
KERNELAPI VOID VgaSetPalette(RGB_COLOR *pPalette, DWORD totalIndex);
KERNELAPI VOID* VgaGetVideoMem(VOID);

#endif /* #ifndef _VGA_DRIVER_HEADER_FILE_ */