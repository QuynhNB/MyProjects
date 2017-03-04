#ifndef DO_H
#define DO_H

#include "main.h"

#define AIRCON1_PORT                   GPIOA
#define AIRCON1_PIN                    GPIO_PIN_7
#define AIRCON1_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOA_CLK_ENABLE()
#define AIRCON1_ON()                   HAL_GPIO_WritePin(AIRCON1_PORT, AIRCON1_PIN, GPIO_PIN_SET)
#define AIRCON1_OFF()                  HAL_GPIO_WritePin(AIRCON1_PORT, AIRCON1_PIN, GPIO_PIN_RESET)
#define AIRCON1_OUT_STATE()            (GPIOA->ODR & AIRCON1_PIN)

#define AIRCON2_PORT                   GPIOA
#define AIRCON2_PIN                    GPIO_PIN_6
#define AIRCON2_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOA_CLK_ENABLE()
#define AIRCON2_ON()                   HAL_GPIO_WritePin(AIRCON2_PORT, AIRCON2_PIN, GPIO_PIN_SET)
#define AIRCON2_OFF()                  HAL_GPIO_WritePin(AIRCON2_PORT, AIRCON2_PIN, GPIO_PIN_RESET)
#define AIRCON2_OUT_STATE()            (GPIOA->ODR & AIRCON2_PIN)




void AIR_Init(void);
void DO_SetValue(uint8_t value);
uint8_t DO_GetValue(void);
#endif /* DO_H */
