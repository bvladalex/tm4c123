/*main.c*/

#include "TM4C123GH6PM_mcu1.h"
#include "tm4c123_gpio_driver.h"
#include "tm4c123_spi_driver.h"
#include "tm4c123_i2c_driver.h"
#include "tm4c123_uart_driver.h"
#include "system_TM4C123GH6PM.h"


#include <string.h>

/*this program will transmit data through UART interface no. 1 of uC
PB0=U1RX
PB1=U1TX
Alt fn number for all pins is 1
*/

////global declarations////

UART_Handle_t UART_Conf_handle;

char msg[1024] = "UART Tx testing...\n\r";
char rx_buf[1024];
//char msg[1024] = "AAAAA";


void delay_debounce(void){
for(int i=0; i<1500000; i++);
}


void UARTperiph_init(void){
	
	UART_Conf_handle.pUARTx=UART1;

	UART_Conf_handle.UARTConfig.uart_mode=UART_MODE_TX;
	UART_Conf_handle.UARTConfig.clk_src=UART_CLK_SYSCLK;
	UART_Conf_handle.UARTConfig.parity=UART_NO_PARITY;
	UART_Conf_handle.UARTConfig.stop_bits=UART_1_STP_BITS;
	UART_Conf_handle.UARTConfig.uart_baud=UART_BAUD_RATE_115200;
	UART_Conf_handle.UARTConfig.wd_len=UART_WD_LEN_8;
	
	UART_Init(&UART_Conf_handle);

}

void GPIOpins_UARTinit(void){
	GPIO_Handle_T GPIO4UART;
	
	GPIO4UART.pGPIOx=GPIOB;
	GPIO4UART.GPIO_PinConfig.GPIO_PinMode=GPIO_MODE_ALTFN;
	GPIO4UART.GPIO_PinConfig.GPIO_PinAltFun_Mode=1;
	GPIO4UART.GPIO_PinConfig.GPIO_PinOPType=GPIO_OP_TYPE_OD;
	GPIO4UART.GPIO_PinConfig.GPIO_PinPuPdControl=GPIO_PIN_PU;
	GPIO4UART.GPIO_PinConfig.GPIO_PinSpeed=GPIO_SPEED_HIGH;
	
	GPIO4UART.GPIO_PinConfig.GPIO_PinNumber=0;
	GPIO_Init(&GPIO4UART);
	
	GPIO4UART.GPIO_PinConfig.GPIO_PinNumber=1;
	GPIO_Init(&GPIO4UART);
}

void GPIO_uart_send_init(void){
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


int main(void){
	
	
	
	GPIOpins_UARTinit();
	UARTperiph_init();
	GPIO_uart_send_init();
	SystemCoreClockUpdate();
	
	
	while(1){
		while(GPIO_ReadFromInputPin(GPIOF, GPIO_PIN_NO_4));
		delay_debounce();	
		uint8_t len=strlen(msg);
		//UART_SendData(&UART_Conf_handle,(uint8_t *)msg,strlen(msg));
		//UART_ReceiveData(&UART_Conf_handle,(uint8_t *)rx_buf,strlen(msg));
		while(len){
			UART_SendData(&UART_Conf_handle,(uint8_t *)msg,1);
			(uint8_t *)msg++;
			UART_ReceiveData(&UART_Conf_handle,(uint8_t *)rx_buf,1);
			(uint8_t *)rx_buf++;
		}
		
		
		
	}
	
	return 0;
}
