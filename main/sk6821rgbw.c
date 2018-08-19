//#define ESP32
#include <stdint.h>
#include "sk6812rgbw.h"
#include "myanimate.h"


/* -----Variables for LED strip----- */
volatile int trans_complete=0;
int deadtime_cnt=0;
double volatile bpm = DEFAULT_BPM;
volatile uint8_t theme_color[4]={0,0,0,64};
volatile double luminance=1;
volatile uint32_t ms_per_beat;
uint32_t level_LO=0x00000000;
uint32_t level_HI=0xFFFFFFFF;

//const uint8_t LED_GPIOPORT_MAP[LED_ROWNUM] = {19, 21, 22, 23};
const uint8_t LED_GPIOPORT_MAP[4] = {12, 13, 14, 15};

gpio_reg_t led_buf[SK6812_BUFSIZE];
gpio_reg_t draw_buf[SK6812_BUFSIZE];

/* -----Variables for animation----- */
/* In the default implementation, uwTick (see stm32f1xx_hal.c) is
 * incremented each 1ms in Systick ISR. We use this variable as
 * drawing timer.
 */
volatile uint32_t sys_mstick;
volatile uint32_t last_time = 0;
volatile uint32_t first_draw_time = 0;
volatile uint32_t current_time=0;
volatile uint32_t elapsed_beats=0;
volatile uint32_t ms_in_beat;
	
void (*rgbw_drawanimate)(void);
void (*init_rgbw_drawanimate)(void);

#define LEDBUF_CLRROW(LED_ROWNUM) (~(0x01<<LED_GPIOPORT_MAP[LED_ROWNUM]))
#define LEDBUF_SETROW(DATA, LED_ROWNUM) ((((DATA<<i) & 0x80)>>7)<<LED_GPIOPORT_MAP[LED_ROWNUM])

void rgbw_init(){
	
	/* Clear transfer complete flag */
	trans_complete=1;
	
	__RGBW_ASSIGN_BPM(DEFAULT_BPM);
	__RGBW_ASSIGN_ANIFUNC(rgbw_animate_default);
}

void rgbw_draw(){
	current_time = TIME_GETMS;
	if (current_time == last_time) return;
	
	elapsed_beats=(current_time-first_draw_time)/ms_per_beat;
	ms_in_beat = (current_time-first_draw_time)-(ms_per_beat*elapsed_beats);
	(*rgbw_drawanimate)();
	
	last_time = current_time;
}

void rgbw_clear(){
	for (int cidx=0;cidx<SK6812_BUFSIZE;cidx++)
		draw_buf[cidx]=0;
}


int activpixel, paddingpixel, startpixel, endpixel;
int period;
void init_rgbw_animate_default(){
	activpixel=LED_LEDNUM/4;
	paddingpixel=(LED_LEDNUM-activpixel)/2;
	activpixel=LED_LEDNUM-2*paddingpixel;
	startpixel=paddingpixel-1;
	endpixel=startpixel+activpixel-1;
	rgbw_clear();
	period=0;
}

void rgbw_animate_default(){
	int color=0;
	int time = current_time%4000;
	if (time>=0 && time<=2000) color=time/8;
	else if (time>2000 && time<4000) color=250-((time-2000)/8);
	for (int i=0;i<LED_ROWNUM;i++)
		for (int j=startpixel;j<endpixel;j++) rgbw_buf_setPixel_rgbw(i,j,0,0,0,color);	
}

void rgbw_buf_setPixel_rgbw(uint8_t row, uint16_t column, uint8_t red, uint8_t green, uint8_t blue, uint8_t white)
{
	//if (row > 1) row++; //No output pin for PB02, so skip 2nd row.
	//if (row > 14) return;//Total 15 rows
	//TODO Different mechanism between STM32 and ESP32?????
	if (!ROW_VALID_CHECK(row))
		return;
	if (red > COLOR_MAX) red = COLOR_MAX;
	if (green > COLOR_MAX) green = COLOR_MAX;
	if (blue > COLOR_MAX) blue = COLOR_MAX;
	if (white > COLOR_MAX) white = COLOR_MAX;
	if (red < COLOR_MIN) red = COLOR_MIN;
	if (green < COLOR_MIN) green = COLOR_MIN;
	if (blue < COLOR_MIN) blue = COLOR_MIN;
	if (white < COLOR_MIN) white = COLOR_MIN;
	/*green>>=2;
	red>>=2;
	blue>>=2;
	white>>=2;*/
	red=red*luminance;
	green=green*luminance;
	blue=blue*luminance;	
	white=white*luminance;
	uint8_t i;
	for (i = 0; i < 8; i++)
	{
		// clear the data for pixel 
		draw_buf[((column*32)+i)] &= LEDBUF_CLRROW(row); //~(0x01<<row);
		draw_buf[((column*32)+8+i)] &= LEDBUF_CLRROW(row); //~(0x01<<row);
		draw_buf[((column*32)+16+i)] &= LEDBUF_CLRROW(row); //~(0x01<<row);
		draw_buf[((column*32)+24+i)] &= LEDBUF_CLRROW(row); //~(0x01<<row);
		// write new data for pixel
		draw_buf[((column*32)+i)] |= LEDBUF_SETROW(green, row); //((((green<<i) & 0x80)>>7)<<row);
		draw_buf[((column*32)+8+i)] |= LEDBUF_SETROW(red, row); //((((red<<i) & 0x80)>>7)<<row);
		draw_buf[((column*32)+16+i)] |= LEDBUF_SETROW(blue, row); //((((blue<<i) & 0x80)>>7)<<row);
		draw_buf[((column*32)+24+i)] |= LEDBUF_SETROW(white, row); //((((white<<i) & 0x80)>>7)<<row);
	}
}

void rgbw_buf_setPixel_rgb(uint8_t row, uint16_t column, uint8_t red, uint8_t green, uint8_t blue)
{	
	int max=0, min=255, white;
	if (row>16) return;
	if (column>LED_LEDNUM) return;
	
	
	if (min > red) min = red;
	if (min > green) min = green;
	if (min > blue) min = blue;
	if (max < red) max = red;
	if (max < green) max = green;
	if (max < blue) max = blue;
	
	// Convertion method mentioned in https://stackoverflow.com/questions/40312216/converting-rgb-to-rgbw
	/*
	int luminance = max + min - 255;
	green -= luminance;
	red -= luminance;
	blue -= luminance;
	white = luminance;
	*/
	
	// Convertion method mentioned in https://stackoverflow.com/questions/21117842/converting-an-rgbw-color-to-a-standard-rgb-hsb-representation
	double rf=red;
	double gf=green;
	double bf=blue;
	double maxf=max;
	double minf=min;
	double wf;
	wf = (maxf/minf<0.5) ? ( (minf*maxf) / (maxf-minf) ) : maxf;
	float q = 255;
	float k = (wf + maxf) / minf;
	red = (int)(((k*rf)-wf)/q);
	green = (int)(((k*gf)-wf)/q);
	blue = (int)(((k*bf)-wf)/q);
	white = (int)wf;
	
	// My own convertion method
	/*if (min > red) min = red;
	if (min > green) min = green;
	if (min > blue) min = blue;
	red-=min;
	green-=min;
	blue-=min;
	white=min;*/
	
	
	/*if (red > COLOR_MAX) red = COLOR_MAX;
	if (green > COLOR_MAX) green = COLOR_MAX;
	if (blue > COLOR_MAX) blue = COLOR_MAX;
	if (white > COLOR_MAX) white = COLOR_MAX;
	if (red < COLOR_MIN) red = COLOR_MIN;
	if (green < COLOR_MIN) green = COLOR_MIN;
	if (blue < COLOR_MIN) blue = COLOR_MIN;
	if (white < COLOR_MIN) white = COLOR_MIN;
	*/
	uint8_t i;
	for (i = 0; i < 8; i++)
	{
		// clear the data for pixel 
		draw_buf[((column*32)+i)] &= LEDBUF_CLRROW(row); //~(0x01<<row);
		draw_buf[((column*32)+8+i)] &= LEDBUF_CLRROW(row); //~(0x01<<row);
		draw_buf[((column*32)+16+i)] &= LEDBUF_CLRROW(row); //~(0x01<<row);
		draw_buf[((column*32)+24+i)] &= LEDBUF_CLRROW(row); //~(0x01<<row);
		// write new data for pixel
		draw_buf[((column*32)+i)] |= LEDBUF_SETROW(green, row); //((((green<<i) & 0x80)>>7)<<row);
		draw_buf[((column*32)+8+i)] |= LEDBUF_SETROW(red, row); //((((red<<i) & 0x80)>>7)<<row);
		draw_buf[((column*32)+16+i)] |= LEDBUF_SETROW(blue, row); //((((blue<<i) & 0x80)>>7)<<row);
		draw_buf[((column*32)+24+i)] |= LEDBUF_SETROW(white, row); //((((white<<i) & 0x80)>>7)<<row);
	
	}
	

}

void check_cmd(char* cmdbuf, int cmdbuf_len){
	// TODO the program don't check if the command is transmitted completely. fix it.

	if (cmdbuf[0]==0)
		return;
	switch (cmdbuf[0]){
		case CMD_COLOR:
			printf("CMD_COLOR\n");
			theme_color[0]=cmdbuf[CMD_COLOR_FIELD_G];
			theme_color[1]=cmdbuf[CMD_COLOR_FIELD_R];
			theme_color[2]=cmdbuf[CMD_COLOR_FIELD_B];
			theme_color[3]=cmdbuf[CMD_COLOR_FIELD_W];
			break;
		case CMD_BPM:
			printf("CMD_BPM\n");
			__RGBW_ASSIGN_BPM(cmdbuf[1]+cmdbuf[2]/100.0);
			(*init_rgbw_drawanimate)();
			break;
		case CMD_LUM:
			printf("CMD_LUM\n");
			luminance=(cmdbuf[1]/255.0);
			break;
		case CMD_ANIM:
			if (cmdbuf[1]==0) __RGBW_ASSIGN_ANIFUNC(rgbw_animate_default);
			if (cmdbuf[1]==1) __RGBW_ASSIGN_ANIFUNC(myanimate_color);
			if (cmdbuf[1]==2) __RGBW_ASSIGN_ANIFUNC(myanimate_wave);
			if (cmdbuf[1]==3) __RGBW_ASSIGN_ANIFUNC(myanimate_pol);
		  if (cmdbuf[1]==4) __RGBW_ASSIGN_ANIFUNC(myanimate_rywave);
			break;
		case CMD_SWITCH:
			break;
	}
	
}