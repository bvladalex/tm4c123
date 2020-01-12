#ifndef INC_TM4C123_UART_DRV_HEADER
#define INC_TM4C123_UART_DRV_HEADER

#include "TM4C123GH6PM_mcu1.h"

typedef struct{
	uint8_t uart_mode;
	uint8_t uart_baud;
	uint8_t wd_len;
	uint8_t parity;
	uint8_t stop_bits;
	uint8_t clk_src;
}UART_Config_t;

typedef struct{
	UART0_Type *pUARTx;
	UART_Config_t UARTConfig;
}UART_Handle_t;

/////uart rx/tx/both modes///

#define UART_MODE_TX 			1
#define UART_MODE_RX 			2
#define UART_MODE_TX_RX		3

///////baud rates///
/*
BRD = BRDI + BRDF = UARTSysClk / (ClkDiv * Baud Rate)
UARTFBRD[DIVFRAC] = integer(BRDF * 64 + 0.5)
*/
#define UART_BAUD_RATE_9600				1U	
//for 9600 write 325 in UARTIBRD and 33 in UARTFBRD
#define UART_BAUD_RATE_14400		 	2U
#define UART_BAUD_RATE_19200	   	3U
#define UART_BAUD_RATE_38400	   	4U
#define UART_BAUD_RATE_57600	   	6U
#define UART_BAUD_RATE_115200	   	6U

//////word lenghts/////

#define UART_WD_LEN_5  	(0U)
#define UART_WD_LEN_6		(1U)
#define UART_WD_LEN_7		(2U)
#define UART_WD_LEN_8		(3U)

/////parity///

#define UART_PARITY_EN		(1U<<1)

#define UART_PARITY_ODD		(0U)
#define UART_PARITY_EVEN	(1U)
#define UART_NO_PARITY		(3U)

#define UART_1_STP_BITS		0
#define UART_2_STP_BITS		(1U)

///clock source////

#define UART_CLK_SYSCLK 	0x0
#define UART_CLK_PIOSC		0x5

////flags////

#define UART_FLAG_TXFE		(1U<<7)
#define UART_FLAG_RXFF    (1U<<6)
#define UART_FLAG_TXFF    (1U<<5)
#define UART_FLAG_RXFE    (1U<<4)
#define UART_FLAG_BUSY    (1U<<3)


//////////UART peripheral APIs prototypes/////

uint8_t UART_GetFlagStatus(UART0_Type *pUARTx, uint8_t FlagName);

void UART_Init(UART_Handle_t *pUARTHandle);

void UART_PeriClockControl(UART0_Type *pUARTx, uint8_t EnorDi);

void UART_SendData(UART_Handle_t *pUARTHandle, uint8_t *pTxBuffer, uint32_t Len);


#endif //TM4C123_UART_DRV_HEADER


