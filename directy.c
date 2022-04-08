#include "directy.h"

/*
 * DEFINITIONS
 */
#define VIDEO_DBGPRINT(STRING)		DbgPrint STRING

#define GMODE_SCREEN_WIDTH			320
#define GMODE_SCREEN_HEIGHT			200
#define GMODE_COLOR_DEPTH			256
#define GMODE_BITS_PER_PIXEL		8
#define GMODE_MEM_SIZE				(GMODE_SCREEN_WIDTH*GMODE_SCREEN_HEIGHT)

#pragma pack(1)

typedef struct _BITMAP_HEADER {
	WORD	Type;
	DWORD	Size;
	DWORD	Reserved;
	DWORD	Offset;
	DWORD	headerSize;
	DWORD	Width;
	DWORD	Height;
	WORD	Planes;
	WORD	BitsPerPixel;
	DWORD	Compression;
	DWORD	SizeImage;
	DWORD	XPixelsPerMeter;
	DWORD	YPixelsPerMeter;
	DWORD	ColorsUsed;
	DWORD	ColorsImportant;
} BITMAP_HEADER, *PBITMAP_HEADER;

typedef struct _BITMAP_CONTROL_BLOCK {
	BITMAP_INFO			bitmap_info;
	RGB_COLOR			*pt_palette;
	BYTE				*pt_bitmap_data;
} BITMAP_CONTROL_BLOCK, *PBITMAP_CONTROL_BLOCK;

#pragma pack()

/*
 * GLOBAL FUNCTIONS
 */
BOOL DyInitializeLibrary(VOID);

/*
 * INTERNEL FUNCTIONS
 */
static VOID DypAdjustBitmapPalette(RGB_COLOR *pPalette);
static VOID DypAdjustBitmapData(BYTE *pSrcBitmap, BYTE *pDestBitmap, DWORD bitmapWidth, DWORD bitmapHeight);

/*
 * VARIABLES
 */
static DY_VIDEO_MODE m_CurrentVideoMode; // video mode
static BYTE *m_pGModeMemAddr;


/**********************************************************************************************************
 *                                             GLOBAL FUNTIONS                                            *
 **********************************************************************************************************/
BOOL DyInitializeLibrary(VOID)
{
	m_CurrentVideoMode = TEXT_MODE; // default
	m_pGModeMemAddr = NULL;

	return TRUE;
}

/**********************************************************************************************************
 *                                           EXPORTED FUNTIONS                                            *
 **********************************************************************************************************/
KERNELAPI BOOL DySetVideoMode(DY_VIDEO_MODE mode, HANDLE hPalette)
{
	BOOL bResult = FALSE;
	RGB_COLOR *pPalette = (RGB_COLOR *)hPalette;

	if(mode == TEXT_MODE) {
		bResult = VgaSetVideoMode(TEXT_MODE_80x25);
	} else { /* video mode */
		if(pPalette == NULL)
			return FALSE;

		bResult = VgaSetVideoMode(VIDEO_MODE_320x200x256);
		if(bResult) {
			m_pGModeMemAddr = VgaGetVideoMem();
			VgaSetPalette(pPalette, GMODE_COLOR_DEPTH);
		}
	}

	if(bResult)
		m_CurrentVideoMode = mode;

	return bResult;
}

KERNELAPI DY_VIDEO_MODE DyGetCurrentVideoMode(VOID)
{
	return m_CurrentVideoMode;
}

KERNELAPI VOID DyPutPixel(WORD scrX, WORD scrY, RGB_COLOR color)
{

}

KERNELAPI VOID DyDrawLine(WORD startX, WORD startY, WORD endX, WORD endY, RGB_COLOR color)
{

}

KERNELAPI VOID DyDrawRect(WORD left, WORD top, WORD right, WORD bottom, RGB_COLOR lineColor)
{

}

KERNELAPI VOID DyFillRect(WORD left, WORD top, WORD right, WORD bottom, RGB_COLOR fillColor)
{

}

KERNELAPI HANDLE DyLoadBitmap(char *pFilename)
{
	HANDLE hFile = NULL;
	BITMAP_HEADER bmp_header;
	BITMAP_CONTROL_BLOCK *pt_bmp_block = NULL;
	BYTE *pt_bitmap = NULL;

	/* check parameters */
	if(pFilename == NULL)
		return NULL;

	/* open bitmap file */
	hFile = FsOpenFile(pFilename, OF_READ_ONLY);
	if(!hFile) {
		VIDEO_DBGPRINT(("ERROR: '%s' is not exist! \r\n", pFilename));
		goto $error;
	}

	/* read bitmap header */
	if(!FsReadFile(hFile, (BYTE *)&bmp_header, sizeof(BITMAP_HEADER))) {
		VIDEO_DBGPRINT(("bmp_header reading error!\r\n"));
		goto $error;
	}

	/* 256 color bitmap? */
	if(bmp_header.BitsPerPixel != GMODE_BITS_PER_PIXEL) {
		VIDEO_DBGPRINT(("not 256 color bitmap! \r\n"));
		goto $error;
	}
	
	/* allocate memory */
	pt_bmp_block = (PBITMAP_CONTROL_BLOCK)MmAllocateNonCachedMemory(sizeof(BITMAP_CONTROL_BLOCK));
	pt_bmp_block->pt_palette = (PRGB_COLOR)MmAllocateNonCachedMemory(sizeof(RGB_COLOR)*GMODE_COLOR_DEPTH);
	pt_bmp_block->pt_bitmap_data = (PBYTE)MmAllocateNonCachedMemory(bmp_header.Width*bmp_header.Height);
	pt_bmp_block->bitmap_info.width = bmp_header.Width;
	pt_bmp_block->bitmap_info.height = bmp_header.Height;
	pt_bmp_block->bitmap_info.color_depth = GMODE_COLOR_DEPTH;
	if(pt_bmp_block == NULL || pt_bmp_block->pt_palette == NULL || pt_bmp_block->pt_bitmap_data == NULL) {
		VIDEO_DBGPRINT(("memory allocation error! \r\n"));
		goto $error;
	}

	/* read palette */
	if(!FsReadFile(hFile, (PBYTE)pt_bmp_block->pt_palette, bmp_header.ColorsUsed*sizeof(RGB_COLOR))) {
		VIDEO_DBGPRINT(("palette reading error! \r\n"));
		goto $error;
	}

	/* read bitmap data */
	pt_bitmap = (PBYTE)MmAllocateNonCachedMemory(bmp_header.Width*bmp_header.Height);
	if(!pt_bitmap) {
		VIDEO_DBGPRINT(("bitmap memory alloc error! \r\n"));
		goto $error;
	}

	if(!FsReadFile(hFile, pt_bitmap, bmp_header.Width*bmp_header.Height)) {
		VIDEO_DBGPRINT(("bitmap data reading error! \r\n"));
		goto $error;
	}

	/* close file handle */
	FsCloseFile(hFile);

	/* adjust bitmap data */
	DypAdjustBitmapPalette(pt_bmp_block->pt_palette);
	DypAdjustBitmapData(pt_bitmap, pt_bmp_block->pt_bitmap_data,
		pt_bmp_block->bitmap_info.width, pt_bmp_block->bitmap_info.height);
	MmFreeNonCachedMemory(pt_bitmap);

	return (HANDLE)pt_bmp_block;

$error:
	if(hFile != NULL)
		FsCloseFile(hFile);
	if(pt_bmp_block) {
		if(pt_bmp_block->pt_palette)
			MmFreeNonCachedMemory(pt_bmp_block->pt_palette);
		if(pt_bmp_block->pt_bitmap_data)
			MmFreeNonCachedMemory(pt_bmp_block->pt_bitmap_data);
		MmFreeNonCachedMemory(pt_bmp_block);
	}
	if(pt_bitmap)
		MmFreeNonCachedMemory(pt_bitmap);
	return NULL;

}

KERNELAPI HANDLE DyGetPaletteHandle(HANDLE hBitmap)
{
	PBITMAP_CONTROL_BLOCK pBmpBlock = (PBITMAP_CONTROL_BLOCK)hBitmap;

	if(pBmpBlock == NULL)
		return NULL;

	return (HANDLE)(pBmpBlock->pt_palette);
}

KERNELAPI BOOL DyGetBitmapInfo(HANDLE hBitmap, BITMAP_INFO *pBitmapInfo)
{
	PBITMAP_CONTROL_BLOCK pBmpBlock = (PBITMAP_CONTROL_BLOCK)hBitmap;

	if(pBmpBlock == NULL)
		return FALSE;

	memcpy(pBitmapInfo, &pBmpBlock->bitmap_info, sizeof(BITMAP_INFO));

	return TRUE;
}

KERNELAPI BOOL DyBitBlt(HANDLE hBitmap, WORD scrX, WORD scrY, DWORD startImgX, DWORD startImgY,
						DWORD cxToBeDisplayed, DWORD cyToBeDisplayed,
						RGB_COLOR *pMaskColor) /* pMaskColor will be ignored. don care~ */
{
	PBITMAP_CONTROL_BLOCK pBmpBlock = (PBITMAP_CONTROL_BLOCK)hBitmap;
	DWORD scr_start_x, scr_end_x, scr_start_y, scr_end_y;
	DWORD img_width, img_height;
	DWORD y_pos;
	BYTE *pt_bitmap;

	if(pBmpBlock == NULL || m_pGModeMemAddr == NULL)
		return FALSE;

	pt_bitmap = pBmpBlock->pt_bitmap_data;
	img_width = pBmpBlock->bitmap_info.width;
	img_height = pBmpBlock->bitmap_info.height;

	if(startImgX+cxToBeDisplayed > img_width)
		cxToBeDisplayed -= ((startImgX+cxToBeDisplayed)-img_width);
	if(startImgY+cyToBeDisplayed > img_height)
		cyToBeDisplayed -= ((startImgY+cyToBeDisplayed)-img_height);

	scr_start_x	= scrX;
	scr_end_x	= (scr_start_x+cxToBeDisplayed>GMODE_SCREEN_WIDTH ? GMODE_SCREEN_WIDTH-1 : scr_start_x+cxToBeDisplayed-1);
	scr_start_y = scrY;
	scr_end_y	= (scr_start_y+cyToBeDisplayed>GMODE_SCREEN_HEIGHT ? GMODE_SCREEN_HEIGHT-1 : scr_start_y+cyToBeDisplayed-1);

	for(y_pos=scr_start_y; y_pos<=scr_end_y; y_pos++) {
		memcpy(
			m_pGModeMemAddr + y_pos*GMODE_SCREEN_WIDTH + scr_start_x,
			pt_bitmap + (startImgY+y_pos-scr_start_y)*img_width + startImgX,
			scr_end_x-scr_start_x+1);
	}

	return TRUE;
}

KERNELAPI VOID DyCloseBitmapHandle(HANDLE hBitmap)
{
	PBITMAP_CONTROL_BLOCK pt_bmp_block = (PBITMAP_CONTROL_BLOCK)hBitmap;

	if(pt_bmp_block) {
		if(pt_bmp_block->pt_palette)
			MmFreeNonCachedMemory(pt_bmp_block->pt_palette);
		if(pt_bmp_block->pt_bitmap_data)
			MmFreeNonCachedMemory(pt_bmp_block->pt_bitmap_data);
		MmFreeNonCachedMemory(pt_bmp_block);
	}
}

/**********************************************************************************************************
 *                                           INTERNEL FUNTIONS                                            *
 **********************************************************************************************************/
static VOID DypAdjustBitmapPalette(RGB_COLOR *pPalette)
{
	int i;

	for(i=0; i<GMODE_COLOR_DEPTH; i++) {
		pPalette[i].element.r >>= 2;
		pPalette[i].element.g >>= 2;
		pPalette[i].element.b >>= 2;
	}
}

static VOID DypAdjustBitmapData(BYTE *pSrcBitmap, BYTE *pDestBitmap, DWORD bitmapWidth, DWORD bitmapHeight)
{
	DWORD y_pos;

	for(y_pos = 0; y_pos<bitmapHeight; y_pos++) {
		memcpy(
			pDestBitmap + y_pos*bitmapWidth,
			pSrcBitmap + bitmapHeight*bitmapWidth - (y_pos+1)*bitmapWidth,
			bitmapWidth);
	}
}