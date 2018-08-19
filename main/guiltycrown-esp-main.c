#include <stdio.h>

#include "sk6812rgbw.h"
#ifdef ESP32
#include "arch_esp32.h"
#elif STM32
#include "arch_stm32.h"
#endif

void main_loop(){
    
}

void app_main()
{
    printf("Hello world!\n");

    /* Print chip information */
    esp_chip_info_t chip_info;
    esp_chip_info(&chip_info);
    printf("This is ESP32 chip with %d CPU cores, WiFi%s%s, ",
           chip_info.cores,
           (chip_info.features & CHIP_FEATURE_BT) ? "/BT" : "",
           (chip_info.features & CHIP_FEATURE_BLE) ? "/BLE" : "");

    printf("silicon revision %d, ", chip_info.revision);

    printf("%dMB %s flash\n", spi_flash_get_chip_size() / (1024 * 1024),
           (chip_info.features & CHIP_FEATURE_EMB_FLASH) ? "embedded" : "external");

    printf("Clock freq at %d\n", XTHAL_GET_CCOUNT());

    init_nvs();
    init_gpio();
    init_bt();
    rgbw_init();

    //BaseType_t task_create_ret = xTaskCreate(&main_loop, "main_loop", 2048, NULL, 1, NULL);
    while (1)
    {
        esp_task_wdt_feed();
        if (trans_complete)
        {
            for (int i = 0; i < SK6812_BUFSIZE; i++)
            {
                led_buf[i] = draw_buf[i];
            }
            BaseType_t task_create_ret = xTaskCreatePinnedToCore(&rgbw_sendbuf, "rgbw_sendbuf", 2048, NULL, 1, NULL, 1);
            rgbw_draw();
        }
    }
}
