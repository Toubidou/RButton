#ifndef _BUTTON_H_
#define _BUTTON_H_

#include "main.h"
#include "rtthread.h"
#include <stdbool.h>

#define		LONG_PRESSED_TIME		1800
#define		BUTTON_NAME_LEN_MAX		11
#define		BUTTON_NUM_MAX			16
#define		BUTTON_COB_NUM_MAX		4
#define		BUTTON_0_SIG_NUM		5

enum 
{
	SCAN = 0, PRESSED =1, CONFIRM_PRESSED = 2, RELEASE = 3,  SHORT_KEY = 5, LONG_KEY=6
};

typedef struct _BUTTON
{
	uint8_t name[BUTTON_NAME_LEN_MAX];
	uint8_t id;           
	uint8_t state;
	bool (*get_fun)(void);       
	void (*short_event)(void);    
	void (*long_event)(void);     
	uint8_t press_time;
	uint8_t flag_long_pressed;
	rt_slist_t next;
	uint8_t cob_reg;
	uint8_t cob_ready_flag;
	uint8_t cob_trigger_flag;
}BUTTON;

typedef struct _ButtonCobInfo
{
	uint16_t mask_array[4];
	uint8_t flag;
	uint8_t cob_num;
}ButtonCobInfo;

BUTTON *button_create(int (*detect_func)(void), void (*short_evt)(void));
void button_scan(void *p);
void button_set_long_pressed_cb(BUTTON *btn, void (*long_event)(void));
rt_err_t button_cob_Reg(uint8_t btn_id0, uint8_t btn_id1, void (*cob_event)(void));
int button_get_id(BUTTON *btn);

#endif
