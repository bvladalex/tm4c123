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
for(int i=0; i<1500000; i++);
}

void GPIOpins_SPIinit(void){
	GPIO_Handle_T GPIO4SPI;
	
	GPIO4SPI.pGPIOx=GPIOA;
	
	GPIO4SPI.GPIO_PinConfig.GPIO_PinMode=GPIO_MODE_ALTFN;
	GPIO4SPI.GPIO_PinConfig.GPIO_PinAltFun_Mode=2;
	GPIO4SPI.GPIO_PinConfig.GPIO_PinOPType=GPIO_OP_TYPE_PP;
	GPIO4SPI.GPIO_PinConfig.GPIO_PinPuPdControl=GPIO_PIN_PD;
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
	SPI_conf_handle.SPIConfig.SPI_SclkSpeed=SPI_SCLKSPEED_DIV64;
	
	SPI_Init(&SPI_conf_handle);
}

uint8_t SPI_verifyResponse(uint8_t ackbyte){
if(ackbyte==0xf5)
	return 1;
else
	return 0;
}

int main(void){
	char user_data[]="How you doin'?";
	uint8_t dummy_byte=0xff;
	uint8_t args[2];
	uint8_t dummy_read;
	////initialise GPIO SW and GPIO alt fn (SPI) periph pins//////
	
	GPIOpins_SPIinit();
	SPIperiph_init();
	GPIO_spi_send_init();
	
	while(1){
	
	while (GPIO_ReadFromInputPin(GPIOF,GPIO_PIN_NO_4));
	
	delay_debounce();
		
	
	uint8_t commndcode = COMMAND_LED_CTRL;
	uint8_t ackbyte;	
	uint8_t analog_read_buff;
		
	SPI_SendData(SSI0,&commndcode,1);
		//dummy read to clear RXNE
	SPI_ReceiveData(SSI0,&dummy_read,1);
		
		
	//send a dummy byte to empty the RX FIFO in slave///
	SPI_SendData(SSI0,&dummy_byte,1);	
	SPI_ReceiveData(SSI0,&ackbyte,1);
		
	if(SPI_verifyResponse(ackbyte)){
		args[0]=LED_PIN;
		args[1]=LED_ON;
		SPI_SendData(SSI0,args,2);
	}
	//////END OF LED CTRL SECTION//////////
	//////START OF ADC READ SECTION/////
	while (GPIO_ReadFromInputPin(GPIOF,GPIO_PIN_NO_4));
	
	delay_debounce();
	
	commndcode = COMMAND_SENSOR_READ;
	SPI_SendData(SSI0,&commndcode,1);
	
	//Clear RX buf and fetch slave info///
	SPI_ReceiveData(SSI0,&dummy_read,1);
	SPI_SendData(SSI0,&dummy_byte,1);
	
	SPI_ReceiveData(SSI0,&ackbyte,1);
	
	if(SPI_verifyResponse(ackbyte)){
		args[0]=ANALOG_PIN0;
		SPI_SendData(SSI0,args,1);
		
		//Clear RX buf and fetch slave info///
		SPI_ReceiveData(SSI0,&dummy_read,1);
		///wait some ms for ARDUINO ADC conversion////
		delay_debounce();
		SPI_SendData(SSI0,&dummy_byte,1);	
		
		SPI_ReceiveData(SSI0,&analog_read_buff,1);
		uint8_t a=0;
		a=analog_read_buff;
	}
		
		
	}

	
	
	return 0;
}
