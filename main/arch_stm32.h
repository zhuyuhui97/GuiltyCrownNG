#ifdef STM32
#ifndef ARCH_STM32_H
#define ARCH_STM32_H

#include "arch_stm32.h"
#include "stm32f1xx_hal.h"

#define SK6812_DEAD_PERIOD 64
#define SK6812_TIM_PERIOD 89
#define UART_BUFSIZE 8
#define UART_DEBUG
#define LEDBUF_TYPE uint8_t

#define LEDBUF_CLRROW(LED_ROWNUM) (~(0x01<<row))
#define LEDBUF_SETROW(DATA, LED_ROWNUM) ((((DATA<<i) & 0x80)>>7)<<LED_ROWNUM)
#define TIME_GETMS  HAL_GetTick()


extern uint8_t uart1rx_buf[UART_BUFSIZE];
extern uint8_t uart2tx_buf[UART_BUFSIZE];

void uart_init(void);
void uart_check(void);

void DMA_TransferCompleteHandler(DMA_HandleTypeDef *DmaHandle);
void TIM2_IRQHandler_Impl(void);

#endif //ARCH_STM32_H
#endif //STM32