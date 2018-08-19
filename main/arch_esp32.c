#ifdef ESP32
#include "arch_esp32.h"
#include "sk6812rgbw.h"

void gen_wave(){
    portMUX_TYPE gen_wave_mux = portMUX_INITIALIZER_UNLOCKED;
    vTaskEnterCritical(&gen_wave_mux);
    volatile uint32_t target_time;
    for (int i = 0; i < SK6812_BUFSIZE; i++){
        target_time = XTHAL_GET_CCOUNT() + DELAY_300NS_CCNT - 45;
        GPIO.out_w1ts = LED_GPIO_MASK;
        while((int)(XTHAL_GET_CCOUNT() - target_time) < 0);
        GPIO.out_w1tc = ((~(led_buf[i])) & LED_GPIO_MASK);
        target_time += DELAY_300NS_CCNT + 45;
        while((int)(XTHAL_GET_CCOUNT() - target_time) < 0);
        GPIO.out_w1tc = LED_GPIO_MASK;
        target_time += (DELAY_300NS_CCNT * 2);
        while((int)(XTHAL_GET_CCOUNT() - target_time) < 0);
    }
    target_time += (DELAY_300NS_CCNT * 270);
    while((int)(XTHAL_GET_CCOUNT() - target_time) < 0);
    vTaskExitCritical(&gen_wave_mux);
}

int init_gpio(){
    for (int i = 0; i < LED_ROWNUM; i++){
        gpio_pad_select_gpio(LED_GPIOPORT_MAP[i]);
        gpio_set_direction(LED_GPIOPORT_MAP[i], GPIO_MODE_OUTPUT);
    }
    return 0;
}

void rgbw_sendbuf(){
    trans_complete = 0;
    gen_wave();
    trans_complete = 1;
    vTaskDelete(NULL);
}
#endif //ESP32