#include "vgadrv.h"
#include "fonts.h"

/* these codes are inspired from a nice stuff http://my.execpc.com/CE/AC/geezer/osd/graphics/modes.c */

/*
/* VGA PORTS
*/
#define	VGA_AC_INDEX				(PUCHAR)0x3C0
#define	VGA_AC_WRITE				(PUCHAR)0x3C0
#define	VGA_AC_READ					(PUCHAR)0x3C1
#define	VGA_MISC_WRITE				(PUCHAR)0x3C2
#define VGA_SEQ_INDEX				(PUCHAR)0x3C4
#define VGA_SEQ_DATA				(PUCHAR)0x3C5
#define	VGA_DAC_READ_INDEX			(PUCHAR)0x3C7
#define	VGA_DAC_WRITE_INDEX			(PUCHAR)0x3C8
#define	VGA_DAC_DATA				(PUCHAR)0x3C9
#define	VGA_MISC_READ				(PUCHAR)0x3CC
#define VGA_GC_INDEX 				(PUCHAR)0x3CE
#define VGA_GC_DATA 				(PUCHAR)0x3CF

//									COLOR emulation		MONO emulation
#define VGA_CRTC_INDEX				(PUCHAR)0x3D4		// 0x3B4
#define VGA_CRTC_DATA				(PUCHAR)0x3D5		// 0x3B5
#define	VGA_INSTAT_READ				(PUCHAR)0x3DA

#define	VGA_NUM_SEQ_REGS			5
#define	VGA_NUM_CRTC_REGS			25
#define	VGA_NUM_GC_REGS				9
#define	VGA_NUM_AC_REGS				21
#define	VGA_NUM_REGS				(1 + VGA_NUM_SEQ_REGS + VGA_NUM_CRTC_REGS + VGA_NUM_GC_REGS + VGA_NUM_AC_REGS)

/*
 * VIDEO MODE
 */
#define TMODE_80X25_MEM_ADDR					0xb8000
#define VMODE_320X200X256_MEM_ADDR				0xa0000

/*
 * GLOBAL FUNCTIONS
 */
BOOL VgaInitializeDriver(VOID);

/*
 * INTERNEL FUNCTIONS
 */
static BOOL VgapWriteParams(UCHAR *pParams);
static void VgapWriteFont(unsigned char *buf, unsigned font_height);
static void VgapSetPlane(unsigned p);
static void VgapBackupTextPalette(void);
static void VgapRestoreTextPalette(void);

/*
 * A GROUP OF VIDEO MODE PARAMETERS
 */
static UCHAR m_80x25_Params[] =
{
/* MISC */
	0x67,
/* SEQ */
	0x03, 0x00, 0x03, 0x00, 0x02,
/* CRTC */
	0x5F, 0x4F, 0x50, 0x82, 0x55, 0x81, 0xBF, 0x1F,
	0x00, 0x4F, 0x0D, 0x0E, 0x00, 0x00, 0x00, 0x50,
	0x9C, 0x0E, 0x8F, 0x28, 0x1F, 0x96, 0xB9, 0xA3,
	0xFF,
/* GC */
	0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 0x0E, 0x00,
	0xFF,
/* AC */
	0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x14, 0x07,
	0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D, 0x3E, 0x3F,
	0x0C, 0x00, 0x0F, 0x08, 0x00
};

static UCHAR m_320x200x256_Params[] =
{
/* MISC */
	0x63,
/* SEQ */
	0x03, 0x01, 0x0F, 0x00, 0x0E,
/* CRTC */
	0x5F, 0x4F, 0x50, 0x82, 0x54, 0x80, 0xBF, 0x1F,
	0x00, 0x41, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x9C, 0x0E, 0x8F, 0x28,	0x40, 0x96, 0xB9, 0xA3,
	0xFF,
/* GC */
	0x00, 0x00, 0x00, 0x00, 0x00, 0x40, 0x05, 0x0F,
	0xFF,
/* AC */
	0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
	0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
	0x41, 0x00, 0x0F, 0x00,	0x00
};

/*
 * VARIABLES
 */
static VGA_VIDEO_MODE m_CurrentVideoMode;
static BYTE m_TextPaletteIndex[16] = {0, 1, 2, 3, 4, 5, 20, 7, 56, 57, 58, 59, 60, 61, 62, 63};
static RGB_COLOR m_TextPalette[16];


/**********************************************************************************************************
 *                                             GLOBAL FUNTIONS                                            *
 **********************************************************************************************************/
BOOL VgaInitializeDriver(VOID)
{
	m_CurrentVideoMode = TMODE_80X25_MEM_ADDR;
	VgapBackupTextPalette();

	return TRUE;
}

/**********************************************************************************************************
 *                                           EXPORTED FUNTIONS                                            *
 **********************************************************************************************************/
KERNELAPI BOOL VgaSetVideoMode(VGA_VIDEO_MODE mode)
{
	BOOL bResult = FALSE;

	if(mode == VIDEO_MODE_320x200x256) {
		m_CurrentVideoMode = VIDEO_MODE_320x200x256;
		bResult = VgapWriteParams(m_320x200x256_Params);
	} else if(mode == TEXT_MODE_80x25) {
		m_CurrentVideoMode = TEXT_MODE_80x25;
		bResult = VgapWriteParams(m_80x25_Params);
		VgapWriteFont(m_EngFont8x16, 16);
		VgapRestoreTextPalette();
	}

	return bResult;
}

KERNELAPI VOID VgaSetPalette(RGB_COLOR *pPalette, DWORD totalIndex)
{
	DWORD i;
	
	WRITE_PORT_UCHAR(VGA_DAC_WRITE_INDEX, 0);
	for(i=0; i<totalIndex; i++) {
		WRITE_PORT_UCHAR(VGA_DAC_DATA, pPalette[i].element.r);
		WRITE_PORT_UCHAR(VGA_DAC_DATA, pPalette[i].element.g);
		WRITE_PORT_UCHAR(VGA_DAC_DATA, pPalette[i].element.b);
	}
}

KERNELAPI VOID* VgaGetVideoMem(VOID)
{
	VOID *pt_addr = NULL;

	if(m_CurrentVideoMode == VIDEO_MODE_320x200x256) {
		pt_addr = (VOID*)VMODE_320X200X256_MEM_ADDR;
	} else if(m_CurrentVideoMode == TEXT_MODE_80x25) {
		pt_addr = (VOID*)TMODE_80X25_MEM_ADDR;
	}

	return pt_addr;
}

/**********************************************************************************************************
 *                                           INTERNEL FUNTIONS                                            *
 **********************************************************************************************************/
static BOOL VgapWriteParams(UCHAR *pParams)
{
	int i;

	// write MISCELLANEOUS reg
	WRITE_PORT_UCHAR(VGA_MISC_WRITE, *pParams);
	pParams++;

	// write SEQUENCER regs
	for(i = 0; i < VGA_NUM_SEQ_REGS; i++) {
		WRITE_PORT_UCHAR(VGA_SEQ_INDEX, i);
		WRITE_PORT_UCHAR(VGA_SEQ_DATA, *pParams);
		pParams++;
	}

	// unlock CRTC registers
	WRITE_PORT_UCHAR(VGA_CRTC_INDEX, 0x03);
	WRITE_PORT_UCHAR(VGA_CRTC_DATA, READ_PORT_UCHAR(VGA_CRTC_DATA) | 0x80);
	WRITE_PORT_UCHAR(VGA_CRTC_INDEX, 0x11);
	WRITE_PORT_UCHAR(VGA_CRTC_DATA, READ_PORT_UCHAR(VGA_CRTC_DATA) & ~0x80);

	// make sure they remain unlocked
	pParams[0x03] |= 0x80;
	pParams[0x11] &= ~0x80;

	// write CRTC regs
	for(i = 0; i < VGA_NUM_CRTC_REGS; i++) {
		WRITE_PORT_UCHAR(VGA_CRTC_INDEX, i);
		WRITE_PORT_UCHAR(VGA_CRTC_DATA, *pParams);
		pParams++;
	}

	// write GRAPHICS CONTROLLER regs
	for(i = 0; i < VGA_NUM_GC_REGS; i++) {
		WRITE_PORT_UCHAR(VGA_GC_INDEX, i);
		WRITE_PORT_UCHAR(VGA_GC_DATA, *pParams);
		pParams++;
	}

	// write ATTRIBUTE CONTROLLER regs
	for(i = 0; i < VGA_NUM_AC_REGS; i++) {
		READ_PORT_UCHAR(VGA_INSTAT_READ);
		WRITE_PORT_UCHAR(VGA_AC_INDEX, i);
		WRITE_PORT_UCHAR(VGA_AC_WRITE, *pParams);
		pParams++;
	}

	// lock 16-color palette and unblank display
	READ_PORT_UCHAR(VGA_INSTAT_READ);
	WRITE_PORT_UCHAR(VGA_AC_INDEX, 0x20);

	return TRUE;
}

static void VgapWriteFont(unsigned char *buf, unsigned font_height)
{
	unsigned char seq2, seq4, gc4, gc5, gc6;
	unsigned i;

	/* save registers set_plane() modifies GC 4 and SEQ 2, so save them as well */
	WRITE_PORT_UCHAR(VGA_SEQ_INDEX, 2);
	seq2 = READ_PORT_UCHAR(VGA_SEQ_DATA);

	WRITE_PORT_UCHAR(VGA_SEQ_INDEX, 4);
	seq4 = READ_PORT_UCHAR(VGA_SEQ_DATA);

	/* turn off even-odd addressing (set flat addressing) assume: chain-4 addressing already off */
	WRITE_PORT_UCHAR(VGA_SEQ_DATA, seq4 | 0x04);

	WRITE_PORT_UCHAR(VGA_GC_INDEX, 4);
	gc4 = READ_PORT_UCHAR(VGA_GC_DATA);

	WRITE_PORT_UCHAR(VGA_GC_INDEX, 5);
	gc5 = READ_PORT_UCHAR(VGA_GC_DATA);

	/* turn off even-odd addressing */
	WRITE_PORT_UCHAR(VGA_GC_DATA, gc5 & ~0x10);

	WRITE_PORT_UCHAR(VGA_GC_INDEX, 6);
	gc6 = READ_PORT_UCHAR(VGA_GC_DATA);

	/* turn off even-odd addressing */
	WRITE_PORT_UCHAR(VGA_GC_DATA, gc6 & ~0x02);

	/* write font to plane P4 */
	VgapSetPlane(2);

	/* write font 0 */
	for(i = 0; i < 256; i++)
	{
		memcpy((void *)(TMODE_80X25_MEM_ADDR + i*32), buf, font_height);
		buf += font_height;
	}

	/* restore registers */
	WRITE_PORT_UCHAR(VGA_SEQ_INDEX, 2);
	WRITE_PORT_UCHAR(VGA_SEQ_DATA, seq2);
	WRITE_PORT_UCHAR(VGA_SEQ_INDEX, 4);
	WRITE_PORT_UCHAR(VGA_SEQ_DATA, seq4);
	WRITE_PORT_UCHAR(VGA_GC_INDEX, 4);
	WRITE_PORT_UCHAR(VGA_GC_DATA, gc4);
	WRITE_PORT_UCHAR(VGA_GC_INDEX, 5);
	WRITE_PORT_UCHAR(VGA_GC_DATA, gc5);
	WRITE_PORT_UCHAR(VGA_GC_INDEX, 6);
	WRITE_PORT_UCHAR(VGA_GC_DATA, gc6);
}

static void VgapSetPlane(unsigned p)
{
	unsigned char pmask;

	p &= 3;
	pmask = 1 << p;

	/* set read plane */
	WRITE_PORT_UCHAR(VGA_GC_INDEX, 4);
	WRITE_PORT_UCHAR(VGA_GC_DATA, p);

	/* set write plane */
	WRITE_PORT_UCHAR(VGA_SEQ_INDEX, 2);
	WRITE_PORT_UCHAR(VGA_SEQ_DATA, pmask);
}

static void VgapBackupTextPalette(void)
{
	DWORD i;

	for(i=0; i<16; i++) {
		WRITE_PORT_UCHAR(VGA_DAC_READ_INDEX, m_TextPaletteIndex[i]);
		m_TextPalette[i].element.r = READ_PORT_UCHAR(VGA_DAC_DATA);
		m_TextPalette[i].element.g = READ_PORT_UCHAR(VGA_DAC_DATA);
		m_TextPalette[i].element.b = READ_PORT_UCHAR(VGA_DAC_DATA);
	}
}

static void VgapRestoreTextPalette(void)
{
	DWORD i;
	
	for(i=0; i<16; i++) {
		WRITE_PORT_UCHAR(VGA_DAC_WRITE_INDEX, m_TextPaletteIndex[i]);
		WRITE_PORT_UCHAR(VGA_DAC_DATA, m_TextPalette[i].element.r);
		WRITE_PORT_UCHAR(VGA_DAC_DATA, m_TextPalette[i].element.g);
		WRITE_PORT_UCHAR(VGA_DAC_DATA, m_TextPalette[i].element.b);
	}
}