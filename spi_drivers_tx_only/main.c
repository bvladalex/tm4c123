/*main.c*/

#include "TM4C123GH6PM_mcu1.h"
#include "tm4c123_gpio_driver.h"
#include "tm4c123_spi_driver.h"

#include <string.h>

/*this program will transmit data through SPI interface no. 0 of uC
PA2=SSI0Clk
PA3=SSI0Fss
PA4=SSI0Rx
PA5=SSI0Tx
Alt fn number for all pins is 2
*/

void delay_debounce(void){
for(int i=0; i<100000; i++);
}

void GPIOpins_SPIinit(void){
	GPIO_Handle_T GPIO4SPI;
	
	GPIO4SPI.pGPIOx=GPIOA;
	
	GPIO4SPI.GPIO_PinConfig.GPIO_PinMode=GPIO_MODE_ALTFN;
	GPIO4SPI.GPIO_PinConfig.GPIO_PinAltFun_Mode=2;
	GPIO4SPI.GPIO_PinConfig.GPIO_PinOPType=GPIO_OP_TYPE_PP;
	GPIO4SPI.GPIO_PinConfig.GPIO_PinPuPdControl=GPIO_NO_PUPD;
	GPIO4SPI.GPIO_PinConfig.GPIO_PinSpeed=GPIO_SPEED_HIGH;
	
	//init for SPI clk
	GPIO4SPI.GPIO_PinConfig.GPIO_PinNumber=2;
	GPIO_Init(&GPIO4SPI);
	
	//init for SPI clk
	GPIO4SPI.GPIO_PinConfig.GPIO_PinNumber=3;
	GPIO_Init(&GPIO4SPI);
	
	//init for SPI Rx
	GPIO4SPI.GPIO_PinConfig.GPIO_PinNumber=4;
	GPIO_Init(&GPIO4SPI);
	
	//init for SPI TX
	GPIO4SPI.GPIO_PinConfig.GPIO_PinNumber=5;
	GPIO_Init(&GPIO4SPI);

}

void GPIO_spi_send_init(void){
	GPIO_Handle_T GPIO_SW1;
	GPIO_PeriClockControl(GPIOF,ENABLE);
	GPIO_SW1.pGPIOx=GPIOF;
	
	//GPIO_PeriClockControl(GPIOF,ENABLE);
	
	GPIO_SW1.GPIO_PinConfig.GPIO_PinNumber=GPIO_PIN_NO_4;
	GPIO_SW1.GPIO_PinConfig.GPIO_PinMode=GPIO_MODE_INPUT;
	GPIO_SW1.GPIO_PinConfig.GPIO_PinSpeed=GPIO_SPEED_MEDIUM;
	GPIO_SW1.GPIO_PinConfig.GPIO_PinPuPdControl=GPIO_PIN_PU;
	GPIO_SW1.GPIO_PinConfig.GPIO_PinOPType=GPIO_OP_TYPE_OD;
	
	GPIO_Init(&GPIO_SW1);

}

void SPIperiph_init(void){
	SPI_Handle_t SPI_conf_handle;
	
	SPI_conf_handle.pSPIx=SSI0;
	
	SPI_conf_handle.SPIConfig.SPI_CLKS=SPI_CLK_SOURCE_SYSCLK;
	SPI_conf_handle.SPIConfig.SPI_CPHA=SPI_CPHA_FIRST;
	SPI_conf_handle.SPIConfig.SPI_CPOL=SPI_CPOL_LOW;
	SPI_conf_handle.SPIConfig.SPI_DeviceMode=SPI_DEVICE_MODE_MASTER;
	SPI_conf_handle.SPIConfig.SPI_DFF=7; //SPI_DSS_8BITS;
	SPI_conf_handle.SPIConfig.SPI_SclkSpeed=SPI_SCLKSPEED_DIV16;
	
	SPI_Init(&SPI_conf_handle);
}

int main(void){
	char user_data[]="How you doin'?";
	////initialise GPIO SW and GPIO alt fn (SPI) periph pins//////
	
	GPIOpins_SPIinit();
	SPIperiph_init();
	GPIO_spi_send_init();
	
	while(1){
	
	while (GPIO_ReadFromInputPin(GPIOF,GPIO_PIN_NO_4));
	
	delay_debounce();
	
	
		
	/////first send length info/////
	uint8_t data_length=strlen(user_data);
	SPI_SendData(SSI0,&data_length,1);
		
	////send actual data///////////
	
	SPI_SendData(SSI0,(uint8_t*)user_data,strlen(user_data));
	
	//while(SPI_GetFlagStatus(SSI0,SPI_BSY_FLAG));
	//SPI_disable(SSI0);
	
	}
	return 0;
}
