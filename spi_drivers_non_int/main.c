/*main.c*/

#include "TM4C123GH6PM_mcu1.h"
#include "tm4c123_gpio_driver.h"
#include "tm4c123_spi_driver.h"

#include <string.h>
#include <stdio.h>

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
	//printf("Hi,welcome to the program");
	uint8_t dummy_byte=0xff;
	uint8_t args[2];
	uint8_t dummy_read=0;
	////initialise GPIO SW and GPIO alt fn (SPI) periph pins//////
	
	GPIOpins_SPIinit();
	SPIperiph_init();
	GPIO_spi_send_init();
	
	while(1)
	{
		//wait till button is pressed
		while(  GPIO_ReadFromInputPin(GPIOF,GPIO_PIN_NO_4) );

		//to avoid button de-bouncing related issues 200ms of delay
		delay_debounce();


	    //1. CMD_LED_CTRL  	<pin no(1)>     <value(1)>

		uint8_t commandcode = COMMAND_LED_CTRL;
		uint8_t ackbyte;
		uint8_t args[2];

		//send command
		SPI_SendData(SSI0,&commandcode,1);

		//do dummy read to clear off the RXNE
		SPI_ReceiveData(SSI0,&dummy_read,1);

		//Send some dummy bits (1 byte) fetch the response from the slave
		SPI_SendData(SSI0,&dummy_byte,1);

		//read the ack byte received
		SPI_ReceiveData(SSI0,&ackbyte,1);

		if( SPI_verifyResponse(ackbyte))
		{
			args[0] = LED_PIN;
			args[1] = LED_ON;

			//send arguments
			SPI_SendData(SSI0,args,2);
			//printf("COMMAND_LED_CTRL Executed\n");
		}
		//end of COMMAND_LED_CTRL
		SPI_ReceiveData(SSI0,&dummy_read,1);

		//2. CMD_SENOSR_READ   <analog pin number(1) >

		//wait till button is pressed
		while(  GPIO_ReadFromInputPin(GPIOF,GPIO_PIN_NO_4) );

		//to avoid button de-bouncing related issues 200ms of delay
		delay_debounce();

		commandcode = COMMAND_SENSOR_READ;

		//send command
		SPI_SendData(SSI0,&commandcode,1);

		//do dummy read to clear off the RXNE
		SPI_ReceiveData(SSI0,&dummy_read,1);

		//Send some dummy byte to fetch the response from the slave
		SPI_SendData(SSI0,&dummy_byte,1);
		SSI0->ICR&=~(0x3);

		//read the ack byte received
		SPI_ReceiveData(SSI0,&ackbyte,1);

		if( SPI_verifyResponse(ackbyte))
		{
			args[0] = ANALOG_PIN0;

			//send arguments
			SPI_SendData(SSI0,args,1); //sending one byte of

			//do dummy read to clear off the RXNE
			SPI_ReceiveData(SSI0,&dummy_read,1);

			//insert some delay so that slave can ready with the data
			delay_debounce();

			//Send some dummy bits (1 byte) fetch the response from the slave
			SPI_SendData(SSI0,&dummy_byte,1);

			uint8_t analog_read;
			SPI_ReceiveData(SSI0,&analog_read,1);
			//printf("COMMAND_SENSOR_READ %d\n",analog_read);
		}

		//3.  CMD_LED_READ 	 <pin no(1) >

		//wait till button is pressed
		while(  GPIO_ReadFromInputPin(GPIOF,GPIO_PIN_NO_4) );

		//to avoid button de-bouncing related issues 200ms of delay
		delay_debounce();

		commandcode = COMMAND_LED_READ;

		//send command
		SPI_SendData(SSI0,&commandcode,1);

		//do dummy read to clear off the RXNE
		SPI_ReceiveData(SSI0,&dummy_read,1);

		//Send some dummy byte to fetch the response from the slave
		SPI_SendData(SSI0,&dummy_byte,1);

		//read the ack byte received
		SPI_ReceiveData(SSI0,&ackbyte,1);

		if( SPI_verifyResponse(ackbyte))
		{
			args[0] = LED_PIN;

			//send arguments
			SPI_SendData(SSI0,args,1); //sending one byte of

			//do dummy read to clear off the RXNE
			SPI_ReceiveData(SSI0,&dummy_read,1);

			//insert some delay so that slave can ready with the data
			delay_debounce();

			//Send some dummy bits (1 byte) fetch the response from the slave
			SPI_SendData(SSI0,&dummy_byte,1);

			uint8_t led_status;
			SPI_ReceiveData(SSI0,&led_status,1);
			//printf("COMMAND_READ_LED %d\n",led_status);

		}

		//4. CMD_PRINT 		<len(2)>  <message(len) >

		//wait till button is pressed
		while(  GPIO_ReadFromInputPin(GPIOF,GPIO_PIN_NO_4) );

		//to avoid button de-bouncing related issues 200ms of delay
		delay_debounce();

		commandcode = COMMAND_PRINT;

		//send command
		SPI_SendData(SSI0,&commandcode,1);

		//do dummy read to clear off the RXNE
		SPI_ReceiveData(SSI0,&dummy_read,1);

		//Send some dummy byte to fetch the response from the slave
		SPI_SendData(SSI0,&dummy_byte,1);

		//read the ack byte received
		SPI_ReceiveData(SSI0,&ackbyte,1);

		uint8_t message[] = "Hello ! How are you ??";
		if( SPI_verifyResponse(ackbyte))
		{
			args[0] = strlen((char*)message);

			//send arguments
			SPI_SendData(SSI0,args,1); //sending length

			//send message
			SPI_SendData(SSI0,message,args[0]);

			//printf("COMMAND_PRINT Executed \n");

		}

		//5. CMD_ID_READ
		//wait till button is pressed
		while(  GPIO_ReadFromInputPin(GPIOF,GPIO_PIN_NO_4) );

		//to avoid button de-bouncing related issues 200ms of delay
		delay_debounce();

		commandcode = COMMAND_ID_READ;

		//send command
		SPI_SendData(SSI0,&commandcode,1);

		//do dummy read to clear off the RXNE
		SPI_ReceiveData(SSI0,&dummy_read,1);

		//Send some dummy byte to fetch the response from the slave
		SPI_SendData(SSI0,&dummy_byte,1);

		//read the ack byte received
		SPI_ReceiveData(SSI0,&ackbyte,1);

		uint8_t id[11];
		uint32_t i=0;
		if( SPI_verifyResponse(ackbyte))
		{
			//read 10 bytes id from the slave
			for(  i = 0 ; i < 10 ; i++)
			{
				//send dummy byte to fetch data from slave
				SPI_SendData(SSI0,&dummy_byte,1);
				SPI_ReceiveData(SSI0,&id[i],1);
			}

			id[11] = '\0';

			//printf("COMMAND_ID : %s \n",id);

		}




		//lets confirm SPI is not busy
		while( SPI_GetFlagStatus(SSI0,SPI_BSY_FLAG) );

		//Disable the SSI0 peripheral
		//SPI_PeripheralControl(SSI0,DISABLE);

		//printf("SPI Communication Closed\n");
	}

	
	
	return 0;
}
