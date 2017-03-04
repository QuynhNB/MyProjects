#include "do.h"


void AIR_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;  

    __HAL_RCC_GPIOA_CLK_ENABLE();
    
    GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_HIGH;
	GPIO_InitStructure.Pin = AIRCON1_PIN  ;
    HAL_GPIO_Init(AIRCON1_PORT , &GPIO_InitStructure);  

    GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_HIGH;
	GPIO_InitStructure.Pin = AIRCON2_PIN  ;
    HAL_GPIO_Init(AIRCON2_PORT , &GPIO_InitStructure); 
 

    AIRCON1_OFF();
    AIRCON2_OFF();

}


void DO_SetValue(uint8_t value)
{
    if (value & 0x01)
    {
        AIRCON1_ON();
    }
    else 
    {
        AIRCON1_OFF();
    }
    
    if (value & 0x02)
    {
        AIRCON2_ON();
    }
    else 
    {
        AIRCON2_OFF();
    }
   
    
}

uint8_t DO_GetValue(void)
{
    uint8_t result = 0;
    
    if (AIRCON1_OUT_STATE())
    {
        result |= 0x01;
    }
    if (AIRCON2_OUT_STATE())
    {
        result |= 0x02;
    } 
    return result;
}