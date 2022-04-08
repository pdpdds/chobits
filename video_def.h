#ifndef _VIDEO_DEFINITION_H_
#define _VIDEO_DEFINITION_H_

/**********************************************************************************************************
 *                                                 VGA DEFINITIONS                                        *
 **********************************************************************************************************/
typedef enum {
	TEXT_MODE_80x25,
	VIDEO_MODE_320x200x256,
} VGA_VIDEO_MODE;


/**********************************************************************************************************
 *                                                 CRT DEFINITIONS                                        *
 **********************************************************************************************************/
#define COLOR_BLACK				0
#define COLOR_BLUE				1
#define COLOR_GREEN				2
#define COLOR_CYAN				3
#define COLOR_RED				4
#define COLOR_MAGENTA			5
#define COLOR_BROWN				6
#define COLOR_WHITE				7
#define COLOR_GRAY				8
#define COLOR_LIGHT_BLUE		9
#define COLOR_LIGHT_GREEN		10
#define COLOR_LIGHT_CYAN		11
#define COLOR_LIGHT_RED			12
#define COLOR_LIGHT_MAGENTA		13
#define COLOR_YELLOW			14
#define COLOR_HIGH_WHITE		15	/* high intensity */


/**********************************************************************************************************
 *                                             DIRECT Y DEFINITIONS                                       *
 **********************************************************************************************************/
typedef enum _DY_VIDEO_MODE {
	TEXT_MODE,
	GRAPHIC_MODE
} DY_VIDEO_MODE;

#pragma pack(1)
typedef struct _BITMAP_INFO {
	DWORD		width;
	DWORD		height;
	WORD		color_depth;
} BITMAP_INFO, *PBITMAP_INFO;

typedef struct _RGB_COLOR {
	union {
		struct {
			BYTE	b;
			BYTE	g;
			BYTE	r;
			BYTE	alpha;
		} element;

		DWORD		rgb;
	};
} RGB_COLOR, *PRGB_COLOR;
#pragma pack()

#endif /* #ifndef _VIDEO_DEFINITION_H_ */