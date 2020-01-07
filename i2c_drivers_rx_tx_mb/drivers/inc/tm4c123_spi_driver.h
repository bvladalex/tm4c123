#ifndef INC_TM4C123_SPI_DRV_HEADER
#define INC_TM4C123_SPI_DRV_HEADER

#include "TM4C123GH6PM_mcu1.h"

typedef struct{
	uint8_t SPI_DeviceMode;
	uint8_t SPI_SclkSpeed;
	uint8_t SPI_DFF;
	uint8_t SPI_CPOL;
	uint8_t SPI_CPHA;
	uint8_t SPI_CLKS;
}SPI_Config_t;

typedef struct{
	SSI0_Type *pSPIx;
	SPI_Config_t SPIConfig;
	uint8_t *pTxBuffer;
	uint8_t *pRxBuffer;
	uint32_t TxLen;
	uint32_t RxLen;
	uint8_t TxState;
	uint8_t RxState;
	}SPI_Handle_t;

////////Macros for SPI/////

#define SPI_DEVICE_MODE_MASTER 0
#define SPI_DEVICE_MODE_SLAVE 1

/*
#define SPI_BUSCONFIG_FULLDUPLEX 0
#define SPI_BUSCONFIG_HALFDUPLEX 0
#define SPI_BUSCONFIG_SIMPLEX_TXONLY 0
#define SPI_BUSCONFIG_SIMPLEX_RXONLY 0
*/


/*///Clock configuration/////
Clock is derived from following formula:
SSInClk = SysClk / (CPSDVSR * (1 + SCR))
where CPSDVSR is written in SSI clock prescaler SSix->CPSR [7:0], the value programmed into this register must be an even number between 2 and 254
and SCR is written in SSI clock rate reg SSIx->CR0 [15:8], 0->255
So, MAX value for SSI clock is SSInClk = SysClk / 2 when CPSDVSR is default 2 and SCR is default 0;

For the following configuration it is assumed that the SySclock div is set to 0 and you can change the PreScaler to one of the 4 pre-defined values
*/

#define SPI_SCLKSPEED_DIV2   (1U<<1)
#define SPI_SCLKSPEED_DIV4   (1U<<2)
#define SPI_SCLKSPEED_DIV8   (1U<<3)
#define SPI_SCLKSPEED_DIV16  (1U<<4)
#define SPI_SCLKSPEED_DIV32  (1U<<5)
#define SPI_SCLKSPEED_DIV64  (1U<<6)

////SPI peripheral Data Frame format [size] ///////////

#define SPI_DSS_8BITS   0x7
#define SPI_DSS_16BITS  0xf


/////SPI CPOL and CPHA

#define SPI_CPHA_FIRST   0   // first clock edge
#define SPI_CPHA_SECOND  1   // second clock edge

#define SPI_CPOL_HIGH 1
#define SPI_CPOL_LOW  0


////Clock source for SSi peripheral

#define SPI_CLK_SOURCE_PIOSC    0x5
#define SPI_CLK_SOURCE_SYSCLK   0x0

///Macro definitions for the flags used in TX and RX APIs

#define SPI_TFE_POS   0
#define SPI_RNE_POS		2
#define SPI_BSY_POS		4

#define SPI_TFE_FLAG (1<<SPI_TFE_POS)
#define SPI_RNE_FLAG (1<<SPI_RNE_POS)
#define SPI_BSY_FLAG (1<<SPI_BSY_POS)
//#define SPI_OR_FLAG (1<<SPI_BSY_POS)



////////////CMD Codes for bidirectional SPI communication////

#define COMMAND_LED_CTRL		0x50
#define COMMAND_SENSOR_READ	0x51
#define COMMAND_LED_READ		0x52
#define COMMAND_PRINT				0x53
#define COMMAND_ID_READ			0x54

#define LED_ON							1
#define LED_OFF							0

#define LED_PIN 						9
#define ANALOG_PIN0					0

//////////SSI interupt modes defines////////////////

#define SSI_INT_TX 					3
#define SSI_INT_RX 					2
#define SSI_TO_INT 					1
#define SSI_OR_INT 					0

#define SPI_READY						0
#define SPI_BUSY_IN_TX 			1
#define SPI_BUSY_IN_RX 			2

////possible SPI app events////
#define SPI_EVENT_TX_CMPLT	0
#define SPI_EVENT_RX_CMPLT	1
#define SPI_EVENT_OVR_ERR		2
#define SPI_EVENT_CRC_ERR		3

#define SPI_OR_FLAG        (1<<0)

//////////////////APIs for SPI peripheral section////////////////

//Peripheral clock setup
void SPI_PeriClockControl(SSI0_Type *pSPIx, uint8_t EnorDi);



//Init and De-init
void SPI_Init(SPI_Handle_t *pSPIHandle);
void SPI_DeInit(SSI0_Type *pSPIx);

void SPI_disable(SSI0_Type *pSPIx);
void SPI_enable(SSI0_Type *pSPIx);

uint8_t SPI_GetFlagStatus(SSI0_Type *pSPIx, uint32_t FlagName);

//Data send and data receive

void SPI_SendData(SSI0_Type *pSPIx, uint8_t *pTxBuffer, uint32_t Len);

void SPI_ReceiveData(SSI0_Type *pSPIx, uint8_t *pRxBuffer, uint32_t Len);

//Data send and data receive with interrupts

uint8_t SPI_SendDataIT(SPI_Handle_t *pSPIHandle, uint8_t *pTxBuffer, uint32_t Len);

uint8_t SPI_ReceiveDataIT(SPI_Handle_t *pSPIHandle, uint8_t *pRxBuffer, uint32_t Len);


//IRQ Configuration and ISR handling

void SPI_IRQInterruptConfig(uint8_t IRQNumber, uint8_t EnorDis);
void SPI_IRQPriorityConfig(uint8_t IRQNumber,uint8_t IRQPriority);
void SPI_IRQHandling(SPI_Handle_t *pHandle);
void SPI_ClearOVRFlag(SSI0_Type *pSPIx);
void SPI_CloseTransmission(SPI_Handle_t *pSPIHandle);
void SPI_CloseReception(SPI_Handle_t *pSPIHandle);

void SPI_ApplicationEventCallback(SPI_Handle_t *pSPIHandle,uint8_t AppEv);



#endif /*INC_TM4C123_GPIO_DRV_HEADER */
