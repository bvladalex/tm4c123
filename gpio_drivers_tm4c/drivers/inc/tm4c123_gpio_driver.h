/* This is the device specifici header file for TM4C123GH6PM micrcocontroller*/

#ifndef INC_TM4C123_GPIO_DRV_HEADER
#define INC_TM4C123_GPIO_DRV_HEADER

#include "TM4C123GH6PM_mcu1.h"

typedef struct
{
	uint8_t GPIO_PinNumber;
	uint8_t GPIO_PinMode;
	uint8_t GPIO_PinSpeed;
	uint8_t GPIO_PinPuPdControl;
	uint8_t GPIO_PinOPType;
	uint8_t GPIO_PinAltFun_Mode;

}GPIO_Pin_Config_t;

typedef struct{
	GPIOA_Type *pGPIOx;
	GPIO_Pin_Config_t GPIO_PinConfig;
}GPIO_Handle_T;

//GPIO pin numbers

#define GPIO_PIN_NO_0 0
#define GPIO_PIN_NO_1 1
#define GPIO_PIN_NO_2 2
#define GPIO_PIN_NO_3 3
#define GPIO_PIN_NO_4 4
#define GPIO_PIN_NO_5 5
#define GPIO_PIN_NO_6 6
#define GPIO_PIN_NO_7 7

//GPIO possible mode

#define GPIO_MODE_INPUT       0
#define GPIO_MODE_OUTPUT      1


#define GPIO_MODE_ALTFN       2
#define GPIO_MODE_ANALOG      3

#define GPIO_MODE_IT_FT       4
#define GPIO_MODE_IT_RT       5
#define GPIO_MODE_IT_RFT      6

//GPIO output type

#define GPIO_OP_TYPE_PP  			0
#define GPIO_OP_TYPE_OD  			1

//GPIO drive strenght

#define GPIO_SPEED_LOW        0
#define GPIO_SPEED_MEDIUM     1
#define GPIO_SPEED_HIGH       2

//GPIO PU/PD configuration

#define GPIO_NO_PUPD          0
#define GPIO_PIN_PU           1
#define GPIO_PIN_PD           2

//

void GPIO_PeriClockControl(GPIOA_Type *pGPIOx, uint8_t EnorDi);
void GPIO_Init(GPIO_Handle_T *pGPIOHandle);

void GPIO_DeInit(GPIOA_Type *pGPIOx);
uint8_t GPIO_ReadFromInputPin(GPIOA_Type *pGPIOx, uint8_t PinNumber);
uint8_t GPIO_ReadFromInputPort(GPIOA_Type *pGPIOx);
void GPIO_WriteToOutputPin(GPIOA_Type *pGPIOx, uint8_t PinNumber, uint8_t value);
void GPIO_WriteToOutputPort(GPIOA_Type *pGPIOx, uint8_t value);
void GPIO_ToggleOutputPin(GPIOA_Type *pGPIOx, uint8_t PinNumber);

void GPIO_IRQConfig(uint8_t IRQNumber, uint8_t IRQPriority, uint8_t EnorDis);
void GPIO_IRQHandling(uint8_t PinNumber);

//#define FLASH_ADDRESS		 0x00000000
//#define ROM_ADDRESS			 0x10000000
//#define SRAM_ADDRESS		 0x20000000 


#endif /*INC_TM4C123_GPIO_DRV_HEADER */
