#ifndef _SYSCALL_TYPE_H
#define _SYSCALL_TYPE_H

/*
 * INCLUDE FILES
 */
#include "types.h"

/*
 * DEFINITIONS
 */
typedef enum _SYSCALL_TYPES {
	/* system api */
	SYSCALL_TERMINATED=0,
	SYSCALL_DELAY,
	SYSCALL_GET_TICKCOUNT,
	SYSCALL_SHOW_TSWATCHDOG,
	SYSCALL_HIDE_TSWATCHDOG,
	/* screen */
	SYSCALL_CLEAR_SCREEN,
	SYSCALL_PRINT_TEXT,
	SYSCALL_PRINT_TEXT_XY,
	SYSCALL_PRINT_TEXT_ATTR,
	SYSCALL_PRINT_TEXT_XY_ATTR,
	/* keyboard */
	SYSCALL_HAS_KEY,
	SYSCALL_GET_KEYDATA,
	/* direct y */
	SYSCALL_SET_VIDEO_MODE,
	SYSCALL_GET_CURRENT_VIDEO_MODE,
	SYSCALL_LOAD_BITMAP,
	SYSCALL_GET_PALETTE_HANDLE,
	SYSCALL_GET_BITMAP_INFO,
	SYSCALL_BITBLT,
	SYSCALL_CLOSE_BITMAP_HANDLE,
} SYSCALL_TYPES;

typedef struct _SYSCALL_MSG {
	SYSCALL_TYPES		syscall_type;
	union {
		struct {
			WORD		x, y;
			BYTE		*pt_text;
			BYTE		attr;
		} PRINT_TEXT;

		struct {
			DWORD		milli_sec;
		} DELAY;

		struct {
			DY_VIDEO_MODE	mode;
			HANDLE			palette;
		} SET_VIDEO_MODE;

		struct {
			char			*pt_filename;
		} LOAD_BITMAP;

		struct {
			HANDLE			bitmap;
		} GET_PALETTE_HANDLE;

		struct {
			HANDLE			bitmap;
			BITMAP_INFO		*pt_bitmap_info;
		} GET_BITMAP_INFO;

		struct {
			HANDLE			bitmap;
			WORD			screen_x;
			WORD			screen_y;
			DWORD			start_img_x;
			DWORD			start_img_y;
			DWORD			cx_to_be_displayed;
			DWORD			cy_to_be_displayed;
			RGB_COLOR		*pt_mask_color;
		} BITBLT;

		struct {
			HANDLE			bitmap;
		} CLOSE_BITMAP_HANDLE;

	} parameters;
} SYSCALL_MSG, *PSYSCALL_MSG;

#endif /* #ifndef _SYSCALL_TYPE_H */