/*GPIO driver definition for TM4C123GH6PM tica c board*/

#include "tm4c123_gpio_driver.h"

/**********************Template for API documentation******************
*@fn             --                                                   *
*                --                                                   *
*@info           --                                                   *
*                --                                                   *
*@param1         --                                                   *
*....            --                                                   *
*@paramx         --                                                   *
*                --                                                   *
*@return         --                                                   *
*                --                                                   *
*@Note           --                                                   *
*                --                                                   *
*                                                                     *
***********************************************************************/

/**********************Peripheral clock en/dis for GPIOs**************
*@fn             --     GPIO_PeriClockControl                                                
*                --                                                   
*@info           --     This function en or dis clock for GPIOS                                              
*                --                                                   
*@param1         --     base address of target GPIO                                              
*@param2         --     en or sid bit                                                 
*                --                                                   
*@return         --     none                                              
*                --                                                   
*@Note           --     none                                              
*                --                                                   
*                                                                     
***********************************************************************/


void GPIO_PeriClockControl(GPIOA_Type *pGPIOx, uint8_t EnorDi){
	if(EnorDi){
		if(pGPIOx==GPIOA){
			GPIOA_PCLK_EN();
		}else if(pGPIOx==GPIOB){
			GPIOB_PCLK_EN();
		}else if(pGPIOx==GPIOC){
			GPIOC_PCLK_EN();
		}else if(pGPIOx==GPIOD){
			GPIOD_PCLK_EN();
		}else if(pGPIOx==GPIOE){
			GPIOE_PCLK_EN();
		}else if(pGPIOx==GPIOF){
			GPIOF_PCLK_EN();
		}
	}else{
		if(pGPIOx==GPIOA){
			GPIOA_PCLK_DIS();
		}else if(pGPIOx==GPIOB){
			GPIOB_PCLK_DIS();
		}else if(pGPIOx==GPIOC){
			GPIOC_PCLK_DIS();
		}else if(pGPIOx==GPIOD){
			GPIOD_PCLK_DIS();
		}else if(pGPIOx==GPIOE){
			GPIOE_PCLK_DIS();
		}else if(pGPIOx==GPIOF){
			GPIOF_PCLK_DIS();
		}
	}
		
}

void GPIO_Init(GPIO_Handle_T *pGPIOHandle){
	uint32_t temp=0;
	if(pGPIOHandle->GPIO_PinConfig.GPIO_PinMode <= GPIO_MODE_ANALOG)
	{
	temp = (pGPIOHandle->GPIO_PinConfig.GPIO_PinMode << (pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber));
	pGPIOHandle->pGPIOx->DIR&=~(0x1<<pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber);
	pGPIOHandle->pGPIOx->DIR|=temp;
	
	}else{
	//int mode
	}
	//////////////drive strength
	
	
	if(pGPIOHandle->GPIO_PinConfig.GPIO_PinSpeed==0)
		pGPIOHandle->pGPIOx->DR2R|= (0x1<<pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber);
	else if (pGPIOHandle->GPIO_PinConfig.GPIO_PinSpeed==1)
		pGPIOHandle->pGPIOx->DR4R|= (0x1<<pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber);
	else if (pGPIOHandle->GPIO_PinConfig.GPIO_PinSpeed==2)
		pGPIOHandle->pGPIOx->DR8R|= (0x1<<pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber);
	
	///////////////PU/PD config
	
	
	if(pGPIOHandle->GPIO_PinConfig.GPIO_PinPuPdControl==GPIO_NO_PUPD)
		{
		pGPIOHandle->pGPIOx->PUR&=~(1<<pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber);
		pGPIOHandle->pGPIOx->PUR&=~(1<<pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber);
		}
	if(pGPIOHandle->GPIO_PinConfig.GPIO_PinPuPdControl==GPIO_PIN_PU)
		pGPIOHandle->pGPIOx->PUR|=(1<<pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber);
	if(pGPIOHandle->GPIO_PinConfig.GPIO_PinPuPdControl==GPIO_PIN_PD)
		pGPIOHandle->pGPIOx->PDR|=(1<<pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber);
	
	
	//////////////output type
	
	
	if(pGPIOHandle->GPIO_PinConfig.GPIO_PinOPType==GPIO_OP_TYPE_PP){
  pGPIOHandle->pGPIOx->ODR&=~(0x1<<pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber);
	pGPIOHandle->pGPIOx->DEN&=~(1<<pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber);
	}
	else if (pGPIOHandle->GPIO_PinConfig.GPIO_PinOPType==GPIO_OP_TYPE_OD){
	pGPIOHandle->pGPIOx->ODR|=(1<<pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber);
	pGPIOHandle->pGPIOx->DEN|=(1<<pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber);
	}
	
	//////////////alt function
	if(pGPIOHandle->GPIO_PinConfig.GPIO_PinMode==GPIO_MODE_ALTFN){
		pGPIOHandle->pGPIOx->AFSEL|=(1<<pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber);
		pGPIOHandle->pGPIOx->PCTL&=~(0xf << (4 * pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber));
		pGPIOHandle->pGPIOx->PCTL|=(pGPIOHandle->GPIO_PinConfig.GPIO_PinAltFun_Mode << (4 * pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber));
	}
	
	///////////////////////analog type/////
	////   tbd
	
}

/**********************GPIO reset**************************************
*@fn             --     GPIO_DeInit                                                
*                --                                                   
*@info           --     This function resets GPIOS peri                                             
*                --                                                   
*@param1         --     base address of target GPIO                                                 
*                --                                                   
*@return         --     none                                              
*                --                                                   
*@Note           --     none                                              
*                --                                                   
*                                                                     
***********************************************************************/


///////////This method only disables GPIOs, does not really reset the regs. Should really implement proper one.///////////

void GPIO_DeInit(GPIOA_Type *pGPIOx){
	if(pGPIOx==GPIOA)
		GPIOA_REG_RESET();
	if(pGPIOx==GPIOB)
		GPIOB_REG_RESET();
	if(pGPIOx==GPIOC)
		GPIOC_REG_RESET();
	if(pGPIOx==GPIOD)
		GPIOD_REG_RESET();
	if(pGPIOx==GPIOE)
		GPIOE_REG_RESET();
	if(pGPIOx==GPIOF)
		GPIOF_REG_RESET();
}

uint8_t GPIO_ReadFromInputPin(GPIOA_Type *pGPIOx, uint8_t PinNumber){
	uint8_t value;
	value=(uint8_t)((pGPIOx->DATA>>PinNumber)&0x1);
	return value;

}

uint8_t GPIO_ReadFromInputPort(GPIOA_Type *pGPIOx){
	uint8_t value;
	value=(uint8_t)(pGPIOx->DATA);
	return value;

}

void GPIO_WriteToOutputPin(GPIOA_Type *pGPIOx, uint8_t PinNumber, uint8_t value){
	
	if(value==1)
		pGPIOx->DATA|=(1<<PinNumber);
	else if(value==0)
		pGPIOx->DATA&=~(1<<PinNumber);

}

void GPIO_WriteToOutputPort(GPIOA_Type *pGPIOx, uint8_t value){
	pGPIOx->DATA=value;
}
	
void GPIO_ToggleOutputPin(GPIOA_Type *pGPIOx, uint8_t PinNumber){
	pGPIOx->DATA^=(1<<PinNumber);
}

void GPIO_IRQConfig(uint8_t IRQNumber, uint8_t IRQPriority, uint8_t EnorDis);
void GPIO_IRQHandling(uint8_t PinNumber);

