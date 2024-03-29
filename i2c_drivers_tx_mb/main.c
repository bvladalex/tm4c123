/*main.c*/

#include "TM4C123GH6PM_mcu1.h"
#include "tm4c123_gpio_driver.h"
#include "tm4c123_spi_driver.h"
#include "tm4c123_i2c_driver.h"

#include <string.h>
#include <stdio.h>

/*this program will transmit data through I2C interface no. 1 of uC
PA6=I2C1SCL
PA7=I2C1SDA
Alt fn number for all pins is 3
*/

////global declarations////

I2C_Handle_t I2C_Conf_handle;
uint8_t my_data[]="We are testing I2c comms!\n";
uint8_t slv_addr=0X68;

void delay_debounce(void){
for(int i=0; i<1500000; i++);
}

void I2Cperiph_init(void){
	
	
	I2C_Conf_handle.pI2Cx=I2C1;
	I2C_Conf_handle.I2CConfig.i2c_clock_set=I2C_TPR_VAL_8;
	I2C_Conf_handle.I2CConfig.i2c_mode=I2C_NON_HS_MODE;
	I2C_Conf_handle.I2CConfig.i2c_slave_address=0x00;
	I2C_Conf_handle.I2CConfig.i2c_master_slave=I2C_MODE_MASTER;
	
	I2C_Init(&I2C_Conf_handle);
}

void GPIOpins_I2Cinit(void){
	GPIO_Handle_T GPIO4I2C;
	
	GPIO4I2C.pGPIOx=GPIOA;
	
	GPIO4I2C.GPIO_PinConfig.GPIO_PinMode=GPIO_MODE_ALTFN;
	GPIO4I2C.GPIO_PinConfig.GPIO_PinAltFun_Mode=3;
	GPIO4I2C.GPIO_PinConfig.GPIO_PinOPType=GPIO_OP_TYPE_PP;
	GPIO4I2C.GPIO_PinConfig.GPIO_PinPuPdControl=GPIO_PIN_PU;
	GPIO4I2C.GPIO_PinConfig.GPIO_PinSpeed=GPIO_SPEED_HIGH;
	
	//init for I2CSCL
	GPIO4I2C.GPIO_PinConfig.GPIO_PinNumber=6;
	GPIO_Init(&GPIO4I2C);
	
	//init for I2CSDA
	GPIO4I2C.GPIO_PinConfig.GPIO_PinOPType=GPIO_OP_TYPE_OD;
	GPIO4I2C.GPIO_PinConfig.GPIO_PinNumber=7;
	GPIO_Init(&GPIO4I2C);

}

void GPIO_i2c_send_init(void){
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
	
	GPIOpins_I2Cinit();
	I2Cperiph_init();
	GPIO_i2c_send_init();
	
	while(1){
	
	while (GPIO_ReadFromInputPin(GPIOF,GPIO_PIN_NO_4));
	
	delay_debounce();
	I2C_SendData(&I2C_Conf_handle, my_data, strlen((char*)my_data), slv_addr);
	
	}
	
	return 0;
}
