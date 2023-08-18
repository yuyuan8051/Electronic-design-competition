#ifndef __OLED_H
#define __OLED_H			  	 
#include "stdint.h"	   
#include "main.h"	    						  
		     
#define OLED_CMD  0	//д����
#define OLED_DATA 1	//д����

//OLED���Ŷ���
//#define OLED_DC		OLED_DC_Pin
//#define OLED_CS		OLED_CS_Pin
//#define OLED_RES 	OLED_RES_Pin

void OLED_Write(uint8_t Data,uint8_t Cmd);
void OLED_Display_On(void);
void OLED_Display_Off(void);
void OLED_Refresh_Gram(void);
void OLED_Init(void);
void OLED_Clear(void);
void OLED_DrawPoint(uint8_t x,uint8_t y,uint8_t t);
void OLED_Fill(uint8_t Fill_Data);
void OLED_ShowChar(uint8_t x,uint8_t y,uint8_t chr,uint8_t size,uint8_t mode);
void OLED_ShowNum(uint8_t x,uint8_t y,uint32_t num,uint8_t len,uint8_t size);
void OLED_ShowString(uint8_t x,uint8_t y,const uint8_t *p,uint8_t size);	
void OLED_ShowChinese(uint8_t x, uint8_t y, const char *ch, uint8_t size, uint8_t mode);
void OLED_ShowChineseX(uint8_t x, uint8_t y, const char *p, uint8_t size, uint8_t mode);
void OLED_Showdecimal(uint8_t x, uint8_t y, float num, uint8_t Z_len, uint8_t X_len, uint8_t size);
void OLED_BrightnessSet(uint8_t V);
void OLED_Show_Picture(uint8_t x, uint8_t y, uint8_t width, uint8_t height, const uint8_t *pic, uint8_t mode);
void OLED_Moveleft(void);
void OLED_Moveright(void);
void OLED_ShowDouble(uint8_t x, uint8_t y, double num, uint8_t Z_len, uint8_t X_len, uint8_t size);

#endif  
	 







 

