#ifdef ESP32
#ifndef ARCH_ESP32_H
#define ARCH_ESP32_H

#include "stdint.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_spi_flash.h"
#include "xtensa/core-macros.h"
#include "esp_int_wdt.h"
#include "nvs_flash.h"
#include "esp_task_wdt.h"
#include "btutil.h"

#define DELAY_24CLK asm volatile("nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" \
                                 "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" \
                                 "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" )

#define DELAY_24CLK_CCNT 24


#if (ESP32_DEFAULT_CPU_FREQ_MHZ == ESP32_DEFAULT_CPU_FREQ_240) 
    #define DELAY_300NS DELAY_24CLK;DELAY_24CLK;DELAY_24CLK 
    #define DELAY_300NS_CCNT (DELAY_24CLK_CCNT*3)
    #define ESP32_CPU_FREQ 240000000
#elif (ESP32_DEFAULT_CPU_FREQ_MHZ == ESP32_DEFAULT_CPU_FREQ_80)
    #define DELAY_300NS DELAY_24CLK
    #define DELAY_300NS_CCNT DELAY_24CLK_CCNT
    #define ESP32_CPU_FREQ 80000000
#elif (ESP32_DEFAULT_CPU_FREQ_MHZ == ESP32_DEFAULT_CPU_FREQ_160)
    #define DELAY_300NS DELAY_24CLK;DELAY_24CLK
    #define DELAY_300NS_CCNT (DELAY_24CLK_CCNT*2)
    #define ESP32_CPU_FREQ 160000000
#endif

#define TIME_GETMS (esp_timer_get_time()/1000)

typedef uint32_t gpio_reg_t;

#define LED_GPIO_MASK ((1<<12) | (1<<13) | (1<<14) | (1<<15))
#define ROW_VALID_CHECK(x) true

int init_gpio(void);
void gen_wave(void);



#endif //ARCH_ESP32_H
#endif //ESP32