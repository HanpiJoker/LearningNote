#ifndef _DISP_MANAGER_H
#define _DISP_MANAGER_H

typedef struct DispOpr {
	char *name;
	int iXres;
	int iYres;
	int iBpp;
	int (*ShowPixel)(int iPenX, int iPenY, unsigned int dwColor);
	int (*DeviceInit)(void);
	int (*CleanScreen)(unsigned int dwBackColor);
	struct DispOpr *ptNext;
} T_DispOpr, *PT_DispOpr; 

#endif
