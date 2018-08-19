
#ifndef SK6812RGBW_H
#define SK6812RGBW_H

#define LED_LEDNUM CONFIG_LED_COLNUM   //DIFF
#define LED_ROWNUM CONFIG_LED_ROWNUM    //DIFF
#define SK6812_BUFSIZE LED_LEDNUM * 32    //DIFF
//Include arch file here
#ifdef ESP32
#include "arch_esp32.h"
#elif STM32
#include "arch_stm32.h"
#endif 

#define COLOR_MAX 255
#define COLOR_MIN 0

#define DEFAULT_BPM 5

#define CMD_COLOR 0x01
#define CMD_BPM 0x02
#define CMD_LUM 0x03
#define CMD_ANIM 0x04
#define CMD_SWITCH 0x05
#define CMD_COLOR_FIELD_R 1
#define CMD_COLOR_FIELD_G 2
#define CMD_COLOR_FIELD_B 3
#define CMD_COLOR_FIELD_W 4
#define CMD_MAXLEN 50

#define __RGBW_DRAW_INITFUNC _init
#define __RGBW_ASSIGN_BPM(NEW_BPM) bpm=NEW_BPM; ms_per_beat=60000/bpm; 
#define __RGBW_ASSIGN_ANIFUNC(ANI_FUNC) rgbw_drawanimate=&ANI_FUNC; init_rgbw_drawanimate=&init_##ANI_FUNC; init_##ANI_FUNC();
//#define __RGBW_CLEAR() for (int RGBW_CLEAR_IDX=0;RGBW_CLEAR_IDX<SK6812_BUFSIZE;RGBW_CLEAR_IDX++) draw_buf[RGBW_CLEAR_IDX]=0;

extern volatile int trans_complete;
extern int deadtime_cnt;
extern volatile uint32_t systick;
extern volatile uint8_t theme_color[4];
extern volatile double bpm;
extern volatile double luminance;
extern volatile uint32_t ms_per_beat;
extern uint32_t level_LO;
extern uint32_t level_HI;

extern gpio_reg_t led_buf[SK6812_BUFSIZE];
extern gpio_reg_t draw_buf[SK6812_BUFSIZE];

extern const uint8_t LED_GPIOPORT_MAP[LED_ROWNUM];

extern volatile uint32_t sys_mstick;
extern volatile uint32_t last_time;
extern volatile uint32_t first_draw_time;
extern volatile uint32_t current_time;
extern volatile uint32_t elapsed_beats;
extern volatile uint32_t ms_in_beat;

/* Platform-specified functions */
void rgbw_sendbuf(void);
/* Platform-specified functions */

void rgbw_init(void);
void rgbw_clear(void);
void init_rgbw_animate_default(void);
void rgbw_animate_default(void);
void rgbw_buf_setPixel_rgbw(uint8_t row, uint16_t column, uint8_t red, uint8_t green, uint8_t blue, uint8_t white);
void rgbw_buf_setPixel_rgb(uint8_t row, uint16_t column, uint8_t red, uint8_t green, uint8_t blue);
void rgbw_sendbuf(void);
void rgbw_draw(void);
void rgbw_update_clk(void);
extern void (*rgbw_drawanimate)(void);
extern void (*init_rgbw_drawanimate)(void);
void check_cmd(char*, int);



#endif