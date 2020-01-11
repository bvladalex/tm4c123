#include "tm4c123_i2c_driver.h"
#include "TM4C123GH6PM_mcu1.h"
//#include <stdint.h>
#include <stddef.h>

/*///Clock equation: TPR = (System Clock/(2*(SCL_LP + SCL_HP)*SCL_CLK))-1;////
//OR SCL_PERIOD = 2×(1 + TPR)×(SCL_LP + SCL_HP)×CLK_PRD
//where: TPR=timer period
//SCL_LP=clock low period
//SCL_HP=clock high period
//basically TPR values represents how many times faster is the system clock compared to SCL
//TPR is between 1 and 127
*/

static void I2Cslave_addr_wr(I2C_Handle_t *pI2CHandle, uint8_t slave_addr);
static void I2Cslave_addr_rd(I2C_Handle_t *pI2CHandle, uint8_t slave_addr);
static void delay_tx(void);


static void I2Cslave_addr_wr(I2C_Handle_t *pI2CHandle, uint8_t slave_addr){
	uint8_t slave_address=(slave_addr<<1);
	pI2CHandle->pI2Cx->MSA=slave_address;
	pI2CHandle->pI2Cx->MSA&=~(1U);
}

static void I2Cslave_addr_rd(I2C_Handle_t *pI2CHandle, uint8_t slave_addr){
	uint8_t slave_address=(slave_addr<<1);
	pI2CHandle->pI2Cx->MSA=slave_address;
	pI2CHandle->pI2Cx->MSA|=(1U);
}

static void delay_tx(void){
for(int i=0; i<800; i++);
}

void I2C_PeriClockControl(I2C0_Type *pI2Cx, uint8_t EnorDi){
	if(EnorDi){
	if (pI2Cx==I2C0)
		I2C0_PCLK_EN();
	else if(pI2Cx==I2C1)
		I2C1_PCLK_EN();
	else if(pI2Cx==I2C2)
		I2C2_PCLK_EN();
	else if(pI2Cx==I2C3)
		I2C3_PCLK_EN();
	}
	else{
	if (pI2Cx==I2C0)
		I2C0_PCLK_DIS();
	else if(pI2Cx==I2C1)
		I2C1_PCLK_DIS();
	else if(pI2Cx==I2C2)
		I2C2_PCLK_DIS();
	else if(pI2Cx==I2C3)
		I2C3_PCLK_DIS();
	}
}

void I2C_master_disable(I2C0_Type *pI2Cx){
	pI2Cx->MCR&=~(I2C_MODE_MASTER);
}
void I2C_master_enable(I2C0_Type *pI2Cx){
	pI2Cx->MCR|=(I2C_MODE_MASTER);
}

void I2C_slave_disable(I2C0_Type *pI2Cx){
	pI2Cx->MCR&=~(I2C_MODE_SLAVE);
}
void I2C_slave_enable(I2C0_Type *pI2Cx){
	pI2Cx->MCR|=(I2C_MODE_SLAVE);
}

void I2C_Init(I2C_Handle_t *pI2CHandle){
	I2C_PeriClockControl(pI2CHandle->pI2Cx,ENABLE);
	pI2CHandle->pI2Cx->MCR=pI2CHandle->I2CConfig.i2c_master_slave;
	pI2CHandle->pI2Cx->MTPR=pI2CHandle->I2CConfig.i2c_clock_set;
	//pI2CHandle->pI2Cx->MSA=((pI2CHandle->I2CConfig.i2c_slave_address)<<1);
	if(pI2CHandle->I2CConfig.i2c_mode==I2C_HS_MODE){
		pI2CHandle->pI2Cx->MCS|=(1U<<4);
		pI2CHandle->pI2Cx->MTPR|=(1U<<7);
	}
	else if(pI2CHandle->I2CConfig.i2c_mode==I2C_NON_HS_MODE){
		pI2CHandle->pI2Cx->MCS&=~(1U<<4);
		pI2CHandle->pI2Cx->MTPR&=~(1U<<7);
	}
}

////////////////////Master send data function///////////////////////////

void I2C_SendData(I2C_Handle_t *pI2CHandle, uint8_t *pTxBuffer, uint32_t Len, uint8_t slave_addr, uint8_t i2c_enordis_sr){
	I2Cslave_addr_wr(pI2CHandle, slave_addr);
	pI2CHandle->pI2Cx->MDR=*pTxBuffer;
	if(Len==1){
		if (i2c_enordis_sr)
			pI2CHandle->pI2Cx->MCS=(I2C_STA_TX_STP&(~(1<<2)));
		else if(!i2c_enordis_sr)
			pI2CHandle->pI2Cx->MCS=I2C_STA_TX_STP;
		//pI2CHandle->pI2Cx->MCS=I2C_STA_TX_STP;
	}
	else if(Len>1){
		Len--;
		pTxBuffer++;
		pI2CHandle->pI2Cx->MCS=I2C_STA_TX;
		while(Len){
			while(I2C_GetFlagStatus(pI2CHandle->pI2Cx,I2C_BSY_FLAG)==FLAG_SET);
			while(I2C_GetFlagStatus(pI2CHandle->pI2Cx,I2C_ERROR_FLAG)==FLAG_SET);  /////// -> Implement routine
			pI2CHandle->pI2Cx->MDR=*pTxBuffer;
			pTxBuffer++;
			Len--;
			pI2CHandle->pI2Cx->MCS=I2C_TX;
		}
		if (i2c_enordis_sr)
			pI2CHandle->pI2Cx->MCS=(I2C_MB_FINISH&(~(1<<2)));
		else if(!i2c_enordis_sr)
			pI2CHandle->pI2Cx->MCS=I2C_MB_FINISH;
}
	while(I2C_GetFlagStatus(pI2CHandle->pI2Cx,I2C_BSY_FLAG)==FLAG_SET);
	while(I2C_GetFlagStatus(pI2CHandle->pI2Cx,I2C_ERROR_FLAG)==FLAG_SET);  /////// -> Implement routine
}

////////////////////Master receive data function///////////////////////////

void I2C_ReceiveData(I2C_Handle_t *pI2CHandle, uint8_t *pTxBuffer, uint32_t Len, uint8_t slave_addr){
	I2Cslave_addr_rd(pI2CHandle, slave_addr);
	if(Len==1){
		pI2CHandle->pI2Cx->MCS=I2C_STA_RX_STP;
		while(I2C_GetFlagStatus(pI2CHandle->pI2Cx,I2C_BSY_FLAG)==FLAG_SET);
		while(I2C_GetFlagStatus(pI2CHandle->pI2Cx,I2C_ERROR_FLAG)==FLAG_SET);
		*pTxBuffer=pI2CHandle->pI2Cx->MDR;
	}
	else if(Len>1){
		pI2CHandle->pI2Cx->MCS=I2C_STA_RX;
		while(Len-1){
			while(I2C_GetFlagStatus(pI2CHandle->pI2Cx,I2C_BSY_FLAG)==FLAG_SET);
			while(I2C_GetFlagStatus(pI2CHandle->pI2Cx,I2C_ERROR_FLAG)==FLAG_SET);
			*pTxBuffer=pI2CHandle->pI2Cx->MDR;
			pTxBuffer++;
			Len--;
			pI2CHandle->pI2Cx->MCS=I2C_RX;
		}
		pI2CHandle->pI2Cx->MCS=I2C_MB_FINISH;
		while(I2C_GetFlagStatus(pI2CHandle->pI2Cx,I2C_BSY_FLAG)==FLAG_SET);
		while(I2C_GetFlagStatus(pI2CHandle->pI2Cx,I2C_ERROR_FLAG)==FLAG_SET);
		*pTxBuffer=pI2CHandle->pI2Cx->MDR;
	}
}

////////////////////Slave send data function///////////////////////////
void I2C_SlaveSendData(I2C_Handle_t *pI2CHandle, uint8_t *pTxBuffer, uint8_t slave_addr){
	while((pI2CHandle->pI2Cx->SCSR)&I2C_SLV_TREQ){
		pI2CHandle->pI2Cx->SDR=*pTxBuffer;
		pTxBuffer++;
		delay_tx();
	}
}

////////////////////Slave receive data function///////////////////////////		 
void I2C_SlaveReceiveData(I2C_Handle_t *pI2CHandle, uint8_t *pRxBuffer, uint8_t slave_addr){
	while((pI2CHandle->pI2Cx->SCSR)&I2C_SLV_RREQ){
		*pRxBuffer=pI2CHandle->pI2Cx->SDR;
		pRxBuffer++;
	}
}

uint8_t I2C_GetFlagStatus(I2C0_Type *pI2Cx, uint8_t FlagName){
	if(pI2Cx->MCS & FlagName)
		return FLAG_SET;
	else
		return FLAG_RESET;
}


uint8_t I2C_SendDataIT(I2C_Handle_t *pI2CHandle, uint8_t *pTxBuffer, uint32_t Len, uint8_t slave_addr, uint8_t i2c_enordis_sr){

	uint8_t busystate=pI2CHandle->TxRxState;
	
	if( busystate!=I2C_BUSY_IN_RX && busystate!=I2C_BUSY_IN_TX){
		pI2CHandle->pTxBuffer=pTxBuffer;
		pI2CHandle->TxLen=Len;
		pI2CHandle->TxRxState=I2C_BUSY_IN_TX;
		pI2CHandle->DevAddress=slave_addr;
		pI2CHandle->Sr=i2c_enordis_sr;
		///Set START bit to raise interrupt flag///
		pI2CHandle->pI2Cx->MCS=(1U<<1);
	}
	
	return busystate;
}
		 
uint8_t I2C_ReceiveDataIT(I2C_Handle_t *pI2CHandle, uint8_t *pRxBuffer, uint32_t Len, uint8_t slave_addr);

//IRQ Configuration and ISR handling

void I2C_IRQInterruptConfig(uint8_t IRQNumber, uint8_t EnorDis){
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

void I2C_IRQHandling(I2C_Handle_t *pHandle);


void I2C_IRQPriorityConfig(uint8_t IRQNumber,uint8_t IRQPriority){
	*(NVIC_PR_BASE_ADDR+IRQNumber)|=(IRQPriority<<(8-TM4C_NO_PRIO_BITS));
}
