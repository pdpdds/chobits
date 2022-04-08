/*
 *                       CHOBITS OPERATING SYSTEM
 *
 * Writer	: Yeori (Sun Kyung-Ryul)
 * E-mail	: alphamcu@hanmail.net
 * Web(kor)	: http://www.zap.pe.kr
 * Web(eng) : http://zap.pe.kr/eng
 *
 * Development Tools : # MS Visual C++ 7.0 with some tools of 6.0(previous version)
 *                     # NASM Assembler (GPL)
 *                     # MASM Assembler (MS)
 *
 * Comments : All source files are opend to public completely without any loyalty or license.
 *            I just want to share the information of computer technology. that's all! ^^
 *
 *
 */

#include "chobits.h"

/*
 * INTERNEL FUNCTIONS
 */
static void halt(char *pMsg);

/*
 * EXTERNEL FUNCTIONS
 */
extern BOOL KrnInitializeKernel(VOID);			/* kernel.c */
extern BOOL CrtInitializeDriver(VOID);			/* 6845crt.c */
extern BOOL KbdInitializeDriver(VOID);			/* kbddrv.c */
extern BOOL FddInitializeDriver(VOID);			/* fdddrv.c */
extern BOOL HshInitializeShell(VOID);			/* hshell.c */

/*
 * THE ENTRY POINT OF CHOBITS OPERATING
 * desc : This function will be excuted by 'jmp' instruction, programmed in "entry.asm".
 */
int chobits_init(void)
{
	/* Initialize device drivers - ALWAYS THIS DEVICE DRIVER MUST BE EXCUTED FIRSTLY. */
	if(!CrtInitializeDriver())		{ halt(NULL); }

	/* Initialize kernel */
	if(!KrnInitializeKernel())		{ halt("KrnInitializeKernel() returned an error.\r\n"); }

	/* PUT DOWN OTHER DEVICE DRIVERS' INITIALIZE ROUTINE!! */
	/* bla bla bla */
	if(!KbdInitializeDriver())		{ halt("KbdInitializeDriver() returned an error.\r\n"); }
	if(!FddInitializeDriver())		{ halt("FddInitializeDriver() returned an error.\r\n"); }

	/* FINALLY, LOAD THE MAIN SHELL, HIDEKI..! */
	if(!HshInitializeShell())		{ halt("HshInitializeShell() returned an error.\r\n"); }

	/* make the first task-switching */
	_asm {
		push	eax

		pushfd
		pop		eax
		or		ah, 40h ; nested
		push	eax
		popfd

		pop		eax
		iretd
	}

	/* pray to God that this function will not be excuted permanently. save us! */
	halt("Chobits OS booting ERROR!!\r\n");
	return 0;
}

static void halt(char *pMsg)
{
	if(pMsg != NULL) {
		DbgPrint(pMsg);
		DbgPrint("Halting system...\r\n");
	}
	while(1) ;
}