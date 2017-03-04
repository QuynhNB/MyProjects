#ifndef RS485_H
#define RS485_H

/* Definition for USARTx clock resources */
#define USARTx                           USART1
#define USARTx_CLK_ENABLE()              __HAL_RCC_USART1_CLK_ENABLE()
#define USARTx_RX_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOA_CLK_ENABLE()
#define USARTx_TX_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOA_CLK_ENABLE()

#define USARTx_FORCE_RESET()             __HAL_RCC_USART1_FORCE_RESET()
#define USARTx_RELEASE_RESET()           __HAL_RCC_USART1_RELEASE_RESET()

/* Definition for USARTx Pins */
#define USARTx_TX_PIN                    GPIO_PIN_9
#define USARTx_TX_GPIO_PORT              GPIOA
#define USARTx_TX_AF                     GPIO_AF1_USART1
#define USARTx_RX_PIN                    GPIO_PIN_10
#define USARTx_RX_GPIO_PORT              GPIOA
#define USARTx_RX_AF                     GPIO_AF1_USART1

/* Definition for USARTx's NVIC */
#define USARTx_IRQn                      USART1_IRQn
#define USARTx_IRQHandler                USART1_IRQHandler

//#define RS485_CTRL_PORT                 GPIOB
//#define RS485_CTRL_PIN                  GPIO_PIN_1
//#define RS485_GPIO_CLK_ENABLE()         __HAL_RCC_GPIOB_CLK_ENABLE()
//#define RS485_RECV_ENABLE()             HAL_GPIO_WritePin(RS485_CTRL_PORT, RS485_CTRL_PIN, GPIO_PIN_RESET)
//#define RS485_SEND_ENABLE()             HAL_GPIO_WritePin(RS485_CTRL_PORT, RS485_CTRL_PIN, GPIO_PIN_SET)



//#define RS485_MAX_RX_SIZE   128
//#define RS485_SOH_CHAR      '*'
//#define RS485_EOT_CHAR      '#'

//void RS485_Init(void);
//void RS485_Listen(void);
//void RS485_GetRequest(void);
//void RS485_GetChar(void);
//void RS485_SendResponse(unsigned char rs485_id, unsigned char cmd);
//unsigned char* RS485_get_aircon_data(unsigned char *data);


#include "main.h"

void M_Data_Start();          // Truyen 3 byte de lam sach khung truyen
void M_Pulse_Start();         // Truyen 1 bit bat dau khung truyen         
void M_Pulse_Finish();        // Truyen 1 bit ket thuc khung truyen
void M_Sent_Bit(unsigned char i);            // Ham truyen 1 bit du lieu
void M_Sent_Byte(unsigned char byte);        // Ham truyen 1 byte du lieu
void M_Sent_Array(unsigned char *array,unsigned char length);    // Ham truyen nhieu byte du lieu
void M_Sent_Frame(unsigned char *data,unsigned char length);     // Ham truyen khung truyen 


#endif /* RS485_H */
