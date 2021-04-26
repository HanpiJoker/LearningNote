#ifndef __OLED_H_
#define __OLED_H_

void OLED_Init(void);void OLED_Print(int col, int page, char *str);void OLED_Put_char(int page, int col, char c);void OLED_Set_Pos(int page, int col);void OLED_Clear_Page(int page);#endif