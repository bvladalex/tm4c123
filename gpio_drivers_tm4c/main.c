/*main.c*/

#include "TM4C123GH6PM_mcu1.h"
#include "tm4c123_gpio_driver.h"

void delay(void){
	for(uint32_t i =0; i<=500000; i++);
}

int main(void){
	
	GPIO_Handle_T GPIO_LED1;
	
	GPIO_LED1.pGPIOx=GPIOF;
	
	GPIO_PeriClockControl(GPIOF,ENABLE);
	
	GPIO_LED1.GPIO_PinConfig.GPIO_PinNumber=GPIO_PIN_NO_3;
	GPIO_LED1.GPIO_PinConfig.GPIO_PinMode=GPIO_MODE_OUTPUT;
	GPIO_LED1.GPIO_PinConfig.GPIO_PinSpeed=GPIO_SPEED_MEDIUM;
	GPIO_LED1.GPIO_PinConfig.GPIO_PinPuPdControl=GPIO_PIN_PU;
	GPIO_LED1.GPIO_PinConfig.GPIO_PinOPType=GPIO_OP_TYPE_OD;
	
	GPIO_Init(&GPIO_LED1);
	while(1){
	
		delay();
		GPIO_ToggleOutputPin(GPIOF,GPIO_PIN_NO_3);

	}
	return 0;
}
