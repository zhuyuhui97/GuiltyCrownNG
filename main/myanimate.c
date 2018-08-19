#include <stdint.h>
#include "sk6812rgbw.h"
#include "myanimate.h"
#include "math.h"


uint32_t myanimate_last_time;
double time_per_led;

void init_myanimate_wave(){
	//time_per_led=ms_per_beat/LED_LED_LEDNUM;
}

void myanimate_wave(){
	double x,y;
	time_per_led=ms_per_beat/LED_LEDNUM;//dirty-hack
	
	for (int idx=0;idx<LED_LEDNUM;idx++){
		/* Convert the coordinate system from time and position to "x (0<x<=20)".
		* But variable x can be larger than 20, so (x mod 20) can be the actuall 
		* number of it. We will do it later.
		*/
		x=(20.0*(ms_in_beat+idx*time_per_led)/ms_per_beat);
		/* We do mod here. We need 2 pulses per beat, so (x mod 10) */
		x=fmod(x,10);
		if (x>0&&x<=1) y=255*x;
		else if (x>1&&x<=9) y=288 -32*x;
		else y=0;
		uint8_t cval_g=(int)(theme_color[0]*y)>>(8);
		uint8_t cval_r=(int)(theme_color[1]*y)>>(8);
		uint8_t cval_b=(int)(theme_color[2]*y)>>(8);
		uint8_t cval_w=(int)(theme_color[3]*y)>>(8);
		for (int i=0;i<LED_ROWNUM;i++)
			rgbw_buf_setPixel_rgbw(i,idx,cval_r,cval_g,cval_b,cval_w);
	}
}

int midpos=-1;
int r_start=0;
int r_end=0;
int b_start=0;
int b_end=LED_LEDNUM-1;

void init_myanimate_pol(){
	//time_per_led_pol=500/LED_LEDNUM; //ms_per_beat=500
	if (LED_LEDNUM%2==1){
		midpos=LED_LEDNUM/2;
		r_end=midpos-1;
		b_start=midpos+1;
	}
	else{
		r_end=(LED_LEDNUM/2)-1;
		b_start=(LED_LEDNUM/2);
	}
}

void myanimate_pol(){
	if (((current_time%10000)>=0)&&((current_time%10000)<2000))
		myanimate_pol_wave();
	else if (((current_time%10000)>=2000)&&((current_time%10000)<4000))
		myanimate_pol_flash1(1);
	else if (((current_time%10000)>=4000)&&((current_time%10000)<6000))
		myanimate_pol_flash2(1);
	else if (((current_time%10000)>=6000)&&((current_time%10000)<8000))
		myanimate_pol_flash1(2);
	else if (((current_time%10000)>=8000)&&((current_time%10000)<10000))
		myanimate_pol_flash2(2);
}

void myanimate_pol_wave(){
//void myanimate_pol(){
	//double time_per_led=50;
	float colorval;
	volatile double x,y;
	int is_red;

	for (int idx=0;idx<LED_LEDNUM;idx++){
		/* Convert the coordinate system from time and position to "x (0<x<=20)".
		 * But variable x can be larger than 20, so (x mod 20) can be the actuall 
		 * number of it. We will do it later.
		 */
		//x=(20*(ms_in_beat+idx*time_per_led)/ms_per_beat);
		//ms_in_beat=current_time%500; ms_per_beat=500; time_per_led=500/LED_LEDNUM;
		x=(20*((current_time%500)+idx*(500.0/LED_LEDNUM))/500.0);
		is_red=(int)fmod(x,20)/10;
		/* We do mod here. We need 2 pulses per beat, so (x mod 10) */
		x=fmod(x,10);
		if (x>0&&x<=2) y=225*(x/2);
		else if (x>2&&x<=10) y=320-64*(x/2);
		else y=0;
		colorval=(int)y;
		if (is_red)
			for (int i=0;i<LED_ROWNUM;i++)
				rgbw_buf_setPixel_rgbw(i,idx,colorval,0,0,0);
		else
			for (int i=0;i<LED_ROWNUM;i++)
				rgbw_buf_setPixel_rgbw(i,idx,0,0,colorval,0);
	}
}

void myanimate_pol_flash1(int speed){
//void myanimate_pol(){
	//int atime=systick;
	int dummy_time=(current_time*speed)%1000;
	for (int i=0;i<LED_ROWNUM;i++)
		rgbw_buf_setPixel_rgbw(i,midpos,0,0,0,0);	//Disable LED in the middle
	if ((dummy_time%1000)<(500)){
		for (int i=0;i<LED_ROWNUM;i++){
			for (int j=r_start;j<=r_end;j++)
				rgbw_buf_setPixel_rgbw(i,j,255,0,0,0);
			for (int j=b_start;j<=b_end;j++)
				rgbw_buf_setPixel_rgbw(i,j,0,0,0,0);
		}
	}else{
		for (int i=0;i<LED_ROWNUM;i++){
			for (int j=r_start;j<=r_end;j++)
				rgbw_buf_setPixel_rgbw(i,j,0,0,0,0);
			for (int j=b_start;j<=b_end;j++)
				rgbw_buf_setPixel_rgbw(i,j,0,0,255,0);
		}
	}
}
void myanimate_pol_flash2(int speed){
	int dummy_time=(current_time*speed)%1000;
	int flag_r=(((dummy_time%1000)>=0)&&((dummy_time%1000)<62))||(((dummy_time%1000)>=125)&&((dummy_time%1000)<188))||(((dummy_time%1000)>=250)&&((dummy_time%1000)<313));
	int flag_b=(((dummy_time%1000)>=500)&&((dummy_time%1000)<562))||(((dummy_time%1000)>=625)&&((dummy_time%1000)<688))||(((dummy_time%1000)>=750)&&((dummy_time%1000)<813));
	if (flag_r){
		for (int i=0;i<LED_ROWNUM;i++){
			for (int j=r_start;j<=r_end;j++)
				rgbw_buf_setPixel_rgbw(i,j,255,0,0,0);
			for (int j=b_start;j<=b_end;j++)
				rgbw_buf_setPixel_rgbw(i,j,0,0,0,0);
		}
	}
	else if (flag_b){
		for (int i=0;i<LED_ROWNUM;i++){
			for (int j=r_start;j<=r_end;j++)
				rgbw_buf_setPixel_rgbw(i,j,0,0,0,0);
			for (int j=b_start;j<=b_end;j++)
				rgbw_buf_setPixel_rgbw(i,j,0,0,255,0);
		}
	}
	else{
		for (int i=0;i<LED_ROWNUM;i++){
			for (int j=0;j<LED_LEDNUM;j++)
				rgbw_buf_setPixel_rgbw(i,j,0,0,0,0);
		}
	}
}

void init_myanimate_color(){
	time_per_led=ms_per_beat/LED_LEDNUM;
}

void myanimate_color(){
	float colorval;
	float x,y;

	for (int idx=0;idx<LED_LEDNUM;idx++){
		uint8_t cval_g=theme_color[0];
		uint8_t cval_r=theme_color[1];
		uint8_t cval_b=theme_color[2];
		uint8_t cval_w=theme_color[3];
		for (int i=0;i<LED_ROWNUM;i++)
			rgbw_buf_setPixel_rgbw(i,idx,cval_r,cval_g,cval_b,cval_w);
	}
}

void init_myanimate_rywave(){}
	

void myanimate_rywave(){
	int flag_time=((current_time%(ms_per_beat*2))<ms_per_beat);
	double prefix_base=(double)ms_in_beat/ms_per_beat;
	int pos=0;
	for (pos=0;pos<LED_LEDNUM;pos++){
		int flag_pos=(pos%6)<3;
		int flag=flag_time^flag_pos;
		if (flag<0.5){
			for (int i=0;i<LED_ROWNUM;i++)
				rgbw_buf_setPixel_rgbw(i,pos,255,0,0,0);
		}else{
			for (int i=0;i<LED_ROWNUM;i++)
					rgbw_buf_setPixel_rgbw(i,pos,150,80,0,0);
		}
	}

}