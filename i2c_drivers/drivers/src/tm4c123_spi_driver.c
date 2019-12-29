#include "tm4c123_spi_driver.h"
#include "TM4C123GH6PM_mcu1.h"
//#include <stdint.h>
#include <stddef.h>


static void spi_txe_int_handle(SPI_Handle_t *pSPIHandle);
static void spi_rxne_int_handle(SPI_Handle_t *pSPIHandle);
static void spi_ovr_err_interrupt_handle(SPI_Handle_t *pSPIHandle);

static void spi_txe_int_handle(SPI_Handle_t *pSPIHandle){
	
	if((pSPIHandle->pSPIx->CR0&(0xf))==SPI_DSS_16BITS)
		//DFF=16 bits case
		{
			pSPIHandle->pSPIx->DR=*((uint16_t*)pSPIHandle->pTxBuffer);
			pSPIHandle->TxLen-=2; // sent 2 bytes of data; Len is reffering to how many bytes we want to send
			(uint16_t*)pSPIHandle->pTxBuffer++;
			
		}
		else if((pSPIHandle->pSPIx->CR0&(0xf))==SPI_DSS_8BITS){
			//DFF=8 bits case
			pSPIHandle->pSPIx->DR=*(pSPIHandle->pTxBuffer);
			pSPIHandle->TxLen--;
			pSPIHandle->pTxBuffer++;
		}
		if(pSPIHandle->TxLen==0){
			pSPIHandle->pSPIx->IM&=~(1<<SSI_INT_TX);
			pSPIHandle->pTxBuffer=NULL;
			pSPIHandle->TxLen=0;
			pSPIHandle->TxState=SPI_READY;
			SPI_ApplicationEventCallback(pSPIHandle, SPI_EVENT_TX_CMPLT);
		}
}


static void spi_rxne_int_handle(SPI_Handle_t *pSPIHandle){
	if((pSPIHandle->pSPIx->CR0&(0xf))==SPI_DSS_16BITS)
		//DFF=16 bits case
		{
			*((uint16_t*)pSPIHandle->pRxBuffer)=pSPIHandle->pSPIx->DR;
			pSPIHandle->RxLen-=2; // sent 2 bytes of data; Len is reffering to how many bytes we want to send
			(uint16_t*)pSPIHandle->pRxBuffer++;
			
		}
		else if((pSPIHandle->pSPIx->CR0&(0xf))==SPI_DSS_8BITS){
			//DFF=8 bits case
			*(pSPIHandle->pRxBuffer)=pSPIHandle->pSPIx->DR;
			pSPIHandle->RxLen--;
			pSPIHandle->pRxBuffer++;
		}
		if(pSPIHandle->RxLen==0){
			pSPIHandle->pSPIx->IM&=~(1<<SSI_INT_RX);
			pSPIHandle->pRxBuffer=NULL;
			pSPIHandle->RxLen=0;
			pSPIHandle->RxState=SPI_READY;
			SPI_ApplicationEventCallback(pSPIHandle, SPI_EVENT_RX_CMPLT);
		}
}

void spi_ovr_err_interrupt_handle(SPI_Handle_t *pSPIHandle){
	uint8_t temp;
	//clear ovr flag
	if(pSPIHandle->TxState!=SPI_BUSY_IN_TX){
		temp=pSPIHandle->pSPIx->DR;
		temp=pSPIHandle->pSPIx->MIS;//////////////////////////////////NOT SURE!!!!!!!!!!!!!!!!!
	}
	SPI_ApplicationEventCallback(pSPIHandle,SPI_EVENT_OVR_ERR);
}

void SPI_ClearOVRFlag(SSI0_Type *pSPIx){
	pSPIx->DR;
	pSPIx->MIS;

}

//void SPI_IRQHandling(SPI_Handle_t *pHandle);

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

void SPI_ReceiveData(SSI0_Type *pSPIx, uint8_t *pRxBuffer, uint32_t Len){
	while(Len>0){
	  while(SPI_GetFlagStatus(pSPIx, SPI_RNE_FLAG) == FLAG_RESET);
		//Send according to DFF
		if((pSPIx->CR0&(0xf))==SPI_DSS_16BITS)
		//DFF=16 bits case
		{
			*((uint16_t*)pRxBuffer)=pSPIx->DR;
			Len-=2; // sent 2 bytes of data; Len is reffering to how many bytes we want to send
			(uint16_t*)pRxBuffer++;
			
		}
		else if((pSPIx->CR0&(0xf))==SPI_DSS_8BITS){
			//DFF=8 bits case
			*pRxBuffer=pSPIx->DR;
			Len--;
			pRxBuffer++;
		}
	}
}

////Sendign and receiving with ITs////


uint8_t SPI_SendDataIT(SPI_Handle_t *pSPIHandle, uint8_t *pTxBuffer, uint32_t Len){
	uint8_t state = pSPIHandle->TxState;
	if(state!=SPI_BUSY_IN_TX){
		//Save TxBuf an len info in some global varaible//
		pSPIHandle->pTxBuffer = pTxBuffer;
		pSPIHandle->TxLen=Len;
		//Mark the TxState as busy so no other code can duse this until TX is over///
		pSPIHandle->TxState=SPI_BUSY_IN_TX;
		///en TX int//
		pSPIHandle->pSPIx->IM|=(1<<SSI_INT_TX);
	}
	
	return state;
}

uint8_t SPI_ReceiveDataIT(SPI_Handle_t *pSPIHandle, uint8_t *pRxBuffer, uint32_t Len){
	uint8_t state = pSPIHandle->RxState;
	if(state!=SPI_BUSY_IN_RX){
		//Save TxBuf an len info in some global varaible//
		pSPIHandle->pRxBuffer = pRxBuffer;
		pSPIHandle->RxLen=Len;
		//Mark the TxState as busy so no other code can duse this until TX is over///
		pSPIHandle->RxState=SPI_BUSY_IN_RX;
		///en TX int//
		pSPIHandle->pSPIx->IM|=(1<<SSI_INT_RX);
	}
	
	return state;
}

void SPI_CloseTransmission(SPI_Handle_t *pSPIHandle){
		pSPIHandle->pSPIx->IM&=~(1<<SSI_INT_TX);
		pSPIHandle->pTxBuffer=NULL;
		pSPIHandle->TxLen=0;
		pSPIHandle->TxState=SPI_READY;
}

void SPI_CloseReception(SPI_Handle_t *pSPIHandle){
		pSPIHandle->pSPIx->IM&=~(1<<SSI_INT_RX);
		pSPIHandle->pRxBuffer=NULL;
		pSPIHandle->RxLen=0;
		pSPIHandle->RxState=SPI_READY;
}


void SPI_IRQHandling(SPI_Handle_t *pHandle){
	uint8_t temp1, temp2;
	temp1=((pHandle->pSPIx->SR)&(SPI_TFE_FLAG));
	temp2=((pHandle->pSPIx->MIS)&(1<<SSI_INT_TX));
	if(temp1&&temp2){
		spi_txe_int_handle(pHandle);
	}
	
	///check for RX interupt
	temp1=((pHandle->pSPIx->SR)&(SPI_RNE_FLAG));
	temp2=((pHandle->pSPIx->MIS)&(1<<SSI_INT_RX));
	if(temp1&&temp2){
		spi_rxne_int_handle(pHandle);
	}
	
	///over run interrupt handling
	temp1=((pHandle->pSPIx->SR)&(SSI_OR_INT));
	temp2=((pHandle->pSPIx->MIS)&(1<<SPI_OR_FLAG));
	if(temp1&&temp2){
		spi_ovr_err_interrupt_handle(pHandle);
	}
	
	///timeout interrupt handling
	
}


//IRQ Configuration and ISR handling

void SPI_IRQInterruptConfig(uint8_t IRQNumber, uint8_t EnorDis){
if(EnorDis){
		if(IRQNumber<=31){
			*NVIC_ISER0|=(1<<IRQNumber);
		}
		else if(IRQNumber>31 && IRQNumber<=63){
			*NVIC_ISER1|=(1<<(IRQNumber%32));
		} 
		else if (IRQNumber>=63 && IRQNumber<=95){
			*NVIC_ISER2|=(1<<(IRQNumber%32));
		}
		else if(IRQNumber>=96 && IRQNumber<=127){
			*NVIC_ISER3|=(1<<(IRQNumber%32));
		} 
		else if (IRQNumber>=128 && IRQNumber<=159){
			*NVIC_ISER4|=(1<<(IRQNumber%32));
		}

}
	else{
		if(IRQNumber<=31){
			*NVIC_ICER0&=~(1<<IRQNumber);
		}
		else if(IRQNumber>31 && IRQNumber<=63){
			*NVIC_ICER1&=~(1<<(IRQNumber%32));
		} 
		else if (IRQNumber>=63 && IRQNumber<=95){
			*NVIC_ICER2&=~(1<<(IRQNumber%32));
		}
		else if(IRQNumber>=96 && IRQNumber<=127){
			*NVIC_ICER3&=~(1<<(IRQNumber%32));
		} 
		else if (IRQNumber>=128 && IRQNumber<=159){
			*NVIC_ICER4&=~(1<<(IRQNumber%32));
		}

}
	
}
void SPI_IRQHandling(SPI_Handle_t *pHandle);


void SPI_IRQPriorityConfig(uint8_t IRQNumber,uint8_t IRQPriority){
	*(NVIC_PR_BASE_ADDR+IRQNumber)|=(IRQPriority<<(8-TM4C_NO_PRIO_BITS));
}

__weak void SPI_ApplicationEventCallback(SPI_Handle_t *pSPIHandle,uint8_t AppEv)
{

	//This is a weak implementation . the user application may override this function.
}
