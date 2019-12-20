#include "tm4c123_spi_driver.h"
#include "TM4C123GH6PM_mcu1.h"

void SPI_PeriClockControl(SSI0_Type *pSPIx, uint8_t EnorDi){
	if(EnorDi){
		//pSPIx->CR1|=(1U<<1);
	if (pSPIx==SSI0)
		SPI0_PCLK_EN();
	else if(pSPIx==SSI1)
		SPI1_PCLK_EN();
	else if(pSPIx==SSI2)
		SPI2_PCLK_EN();
	else if(pSPIx==SSI3)
		SPI3_PCLK_EN();
	}
	else{
		//pSPIx->CR1&=~(1U<<1);
	if (pSPIx==SSI0)
		SPI0_PCLK_DIS();
	else if(pSPIx==SSI1)
		SPI1_PCLK_DIS();
	else if(pSPIx==SSI2)
		SPI2_PCLK_DIS();
	else if(pSPIx==SSI3)
		SPI3_PCLK_DIS();
	}
}

void SPI_Init(SPI_Handle_t *pSPIHandle){
	//Enable SSI Module
	SPI_PeriClockControl(pSPIHandle->pSPIx,1);
	pSPIHandle->pSPIx->CR1|=(1U<<1);
	//Configure Master/Slave Mode
	pSPIHandle->pSPIx->CR1|=((pSPIHandle->SPIConfig.SPI_DeviceMode)<<2);
	//Configure clock divider
	pSPIHandle->pSPIx->CPSR&=~(0xff);
	pSPIHandle->pSPIx->CPSR|=pSPIHandle->SPIConfig.SPI_SclkSpeed;
	//Configure frame format: 8bit/16bit
	pSPIHandle->pSPIx->CR0&=~(0xf);
	pSPIHandle->pSPIx->CR0|=7;//pSPIHandle->SPIConfig.SPI_DFF;
	//Configure CPHA and CPOL
	pSPIHandle->pSPIx->CR0&=~(3U<<6);
	pSPIHandle->pSPIx->CR0|=((pSPIHandle->SPIConfig.SPI_CPHA<<7)|(pSPIHandle->SPIConfig.SPI_CPOL<<6));
	//Configure clock source for SSi peripheral
	pSPIHandle->pSPIx->CC&=~(0xf);
	pSPIHandle->pSPIx->CC|=pSPIHandle->SPIConfig.SPI_CLKS;
}


void SPI_DeInit(SSI0_Type *pGPIOx){
pGPIOx->CR1&=~(1U<<1);
}

void SPI_disable(SSI0_Type *pSPIx){
	pSPIx->CR1&=~(1U<<1);

}
void SPI_enable(SSI0_Type *pSPIx){
	pSPIx->CR1|=(1U<<1);
}


uint8_t SPI_GetFlagStatus(SSI0_Type *pSPIx, uint32_t FlagName){
		if(pSPIx->SR & FlagName)
				return FLAG_SET;
		else
				return FLAG_RESET;
}

//Data send and data receive

void SPI_SendData(SSI0_Type *pSPIx, uint8_t *pTxBuffer, uint32_t Len){
	while(Len>0){
	  while(SPI_GetFlagStatus(pSPIx, SPI_TFE_FLAG) == FLAG_RESET);
		//Send according to DFF
		if((pSPIx->CR0&(0xf))==SPI_DSS_16BITS)
		//DFF=16 bits case
		{
			pSPIx->DR=*((uint16_t*)pTxBuffer);
			Len-=2; // sent 2 bytes of data; Len is reffering to how many bytes we want to send
			(uint16_t*)pTxBuffer++;
			
		}
		else if((pSPIx->CR0&(0xf))==SPI_DSS_8BITS){
			//DFF=8 bits case
			pSPIx->DR=*pTxBuffer;
			Len--;
			pTxBuffer++;
		}
	}


}

void SPI_ReceiveData(SSI0_Type *pSPIx, uint8_t *pRxBuffer, uint32_t Len);


//IRQ Configuration and ISR handling

void SPI_IRQInterruptConfig(uint8_t IRQNumber, uint8_t EnorDis);
void SPI_IRQHandling(SPI_Handle_t *pHandle);
void SPI_IRQPriorityConfig(uint8_t IRQNumber,uint8_t IRQPriority);
