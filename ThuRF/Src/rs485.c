#include "main.h"
#include "rs485.h"
#include "ringbuf.h"
#include <string.h>
#include <stdlib.h>
#include "do.h"
#include "current_meas.h"

#define RS485_MSG_SIZE      64

#define RS485_SOH_CHAR      '*'
#define RS485_EOT_CHAR      '#'

#define RS485_READ_CMD      0x02
#define RS485_WRITE_CMD     0x01


UART_HandleTypeDef UartHandle;
uint8_t RS485RxBuf[RS485_MAX_RX_SIZE];
uint8_t getBuffer[RS485_MAX_RX_SIZE];
static uint8_t getBufferLen = 0;
static uint8_t gRS485RxChar = 0;
static struct ringbuf rxBuf;

static uint8_t decode_checksum(uint8_t high, uint8_t low)
{
    high = (high > 0x40) ? (high - 0x37) : (high - 0x30);
    low = (low > 0x40) ? (low - 0x37) : (low - 0x30);
    return ((high << 4) + low);
}

static void encode_checksum(uint8_t checksum, char *high, char *low)
{
    *high = (checksum & 0xF0) >> 4;
    *low = checksum & 0x0F;
    *high = (*high < 0x0A) ? (*high + 0x30) : (*high + 0x37);
    *low = (*low < 0x0A) ? (*low + 0x30) : (*low + 0x37);
}

static uint8_t rs485_cal_checksum(uint8_t *buf, uint32_t len)
{
    uint8_t checksum = 0;
    uint32_t i = 0;
    for (i = 0; i < len; i++)
    {
        checksum ^= buf[i];
    }
    return checksum;
}

static void convert_uint8_to_hexchar(uint8_t number, char *high, char *low)
{
    *high = (number & 0xF0) >> 4;
    *low = number & 0x0F;
    *high = (*high >= 0 && *high <= 9) ? (*high + 0x30) : (*high + 0x37);
    *low = (*low >= 0 && *low <= 9) ? (*low + 0x30) : (*low + 0x37);
}

static uint8_t convert_hexchar_to_uint8(uint8_t high, uint8_t low)
{
    uint8_t result = 0;

    result = (low >= '0' && low <= '9') ? (low - 0x30) : (low - 0x37);
    result |= (high >= '0' && high <= '9') ? ((high - 0x30) << 4) : ((high - 0x37) << 4);
    return result;
}

void RS485_Listen(void)
{
    RS485_RECV_ENABLE();
    HAL_UART_Receive_IT(&UartHandle, &gRS485RxChar, 1);
}
void RS485_Write(uint8_t *data, uint32_t len)
{
    RS485_SEND_ENABLE();
    HAL_UART_Transmit(&UartHandle, data, len, 0xFFFF);
    RS485_RECV_ENABLE();
}
void RS485_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    RS485_GPIO_CLK_ENABLE();

   ringbuf_init(&rxBuf, RS485RxBuf, RS485_MAX_RX_SIZE);
  /*##-1- Configure the UART peripheral ######################################*/
  /* Put the USART peripheral in the Asynchronous mode (UART Mode) */
  /* UART configured as follows:
      - Word Length = 8 Bits
      - Stop Bit = One Stop bit
      - Parity = None
      - BaudRate = 9600 baud
      - Hardware flow control disabled (RTS and CTS signals) */
    USARTx_CLK_ENABLE();
    UartHandle.Instance        = USARTx;

    UartHandle.Init.BaudRate   = 9600;
    UartHandle.Init.WordLength = UART_WORDLENGTH_8B;
    UartHandle.Init.StopBits   = UART_STOPBITS_1;
    UartHandle.Init.Parity     = UART_PARITY_NONE;
    UartHandle.Init.HwFlowCtl  = UART_HWCONTROL_NONE;
    UartHandle.Init.Mode       = UART_MODE_TX_RX;
    UartHandle.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
    if(HAL_UART_DeInit(&UartHandle) != HAL_OK)
    {
    }
    if(HAL_UART_Init(&UartHandle) != HAL_OK)
    {
    }
    /* CTRL485 pin */
    GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_HIGH;
	GPIO_InitStructure.Pin = RS485_CTRL_PIN  ;
    HAL_GPIO_Init(RS485_CTRL_PORT , &GPIO_InitStructure);
    RS485_Listen();
}
void RS485_GetChar(void)
{
   ringbuf_put(&rxBuf, gRS485RxChar);
}
void ConvertUINT8ToHEXCHAR(uint8_t *temp, uint8_t num)
{
    uint8_t high = num >> 4;
    uint8_t low = num & 0x0F;
    if (high >= 0 && high <= 9)
    {
        temp[0] = high + 0x30;
    }
    else
    {
        temp[0] = high + 0x37;
    }
    if (low >= 0 && low <= 9)
    {
        temp[1] = low + 0x30;
    }
    else
    {
        temp[1] = low + 0x37;
    }
}
uint8_t* RS485_get_aircon_data(uint8_t *data)
{
    uint32_t data_len = 0;
    uint32_t aircon1_current = AIRCON_1_GetCurrent();
    uint32_t aircon2_current = AIRCON_2_GetCurrent();

    sprintf(data, "%04d%04d",aircon1_current, aircon2_current);
    data_len = strlen(data);
    *(data + data_len) = 0;
    return data;
}
/*-----------------------------------------------
 | SOT | rs485_id | CMD | Data | Checksum | EOT |
 -----------------------------------------------*/
void RS485_SendResponse(uint8_t rs485_id, uint8_t cmd)
{
    char buf[RS485_MSG_SIZE];
    uint32_t buf_len = 0;
    uint8_t checksum = 0;

    buf[0] = RS485_SOH_CHAR;
    convert_uint8_to_hexchar(rs485_id, &buf[1], &buf[2]);
    convert_uint8_to_hexchar(cmd, &buf[3], &buf[4]);
    RS485_get_aircon_data(&buf[5]);
    buf_len = strlen(buf);
    checksum = rs485_cal_checksum((uint8_t *)&buf[5], buf_len);
    encode_checksum(checksum, &buf[buf_len + 1], &buf[buf_len + 2]);
    buf[buf_len + 3] = RS485_EOT_CHAR;
    buf_len += 3;
    RS485_Write((uint8_t *)buf, buf_len);
}


/*-----------------------------------------------
 | SOT | rs485_id | CMD | Data | Checksum | EOT |
 -----------------------------------------------*/
void RS485_GetRequest(void)
{
    uint8_t c = 0;
    static uint8_t startOfMsg = 0;
    if (ringbuf_elements(&rxBuf)) {
         c = ringbuf_get(&rxBuf);
        if (c == RS485_SOH_CHAR) {
            getBufferLen = 0;
            startOfMsg = 1;
            return;
        }
        if (startOfMsg) {
            getBuffer[getBufferLen++] = c;
            if (getBufferLen >= RS485_MAX_RX_SIZE) {
                getBufferLen = 0;
                startOfMsg = 0;
                return;
            }
            if (c == RS485_EOT_CHAR && getBufferLen ) {
                if (getBufferLen) {
                    /* replace PROTO_EOT_CHAR by NULL */
                    getBuffer[getBufferLen - 1] = 0;
                    /* call handler */
                    RS485_InMessageHandler(getBuffer, getBufferLen);
                }
                /* end of msg */
                getBufferLen = 0;
                startOfMsg = 0;
            }
        }
    }
}

int32_t RS485_InMessageHandler(char *msg, uint8_t len)
{
    uint8_t temp[3] = {0};
    uint8_t ctrl = 0;
    uint8_t rs485_id = 0;
    uint8_t cmd = 0;
    uint8_t checksum = 0;
    uint8_t cal_checksum = 0;
    uint8_t *data = 0;
    uint16_t data_len = 0;
    uint16_t i;

    temp[0] = msg[0];
    temp[1] = msg[1];
    temp[2] = 0;
    rs485_id = atoi((char *)temp);
    temp[0] = msg[2];
    temp[1] = msg[3];
    temp[2] = 0;
    cmd = atoi((char *)temp);
    data = &msg[4];
    data_len = strlen((char *)data); /* Data with 2 bytes checksum */
    /* Decode checksum byte */
    checksum = decode_checksum(msg[data_len - 2], msg[data_len - 1]);

    /* Calculate checksum byte from received data */
    for (i = 0; i < data_len; i++)
    {
        cal_checksum ^= msg[i];
    }
    if (checksum != cal_checksum)
    {
        return -1;
    }

    if (rs485_id == 0x02)
    {
        /* node is AIRCON */
        switch (cmd)
        {
            case 1:
                /* write msg */
                temp[0] = msg[3];
                temp[1] = msg[4];
                temp[2] = 0;
                ctrl = atoi((char *)temp);
                DO_SetValue(ctrl);
                /* set response */
                break;
            case 2:
                /* read msg */
                // ctrl = DO_GetValue();
                // ConvertUINT8ToHEXCHAR(temp, ctrl);
                /* set response */
                RS485_SendResponse(rs485_id, cmd);
                break;
        }
    }
    return 0;
}
