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

void I2C_SendData(I2C_Handle_t *pI2CHandle, uint8_t *pTxBuffer, uint32_t Len, uint8_t slave_addr){
	I2Cslave_addr_wr(pI2CHandle, slave_addr);
	pI2CHandle->pI2Cx->MDR=*pTxBuffer;
	if(Len==1)
		pI2CHandle->pI2Cx->MCS=I2C_STA_TX_STP;
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
	}
	else if(Len>1){
		pI2CHandle->pI2Cx->MCS=I2C_STA_RX;
		while(Len-1){
			while(I2C_GetFlagStatus(pI2CHandle->pI2Cx,I2C_BSY_FLAG)==FLAG_SET);
			while(I2C_GetFlagStatus(pI2CHandle->pI2Cx,I2C_ERROR_FLAG)==FLAG_SET);
			*pTxBuffer=pI2CHandle->pI2Cx->MDR;
			pTxBuffer++;
			Len--;
		}
		pI2CHandle->pI2Cx->MCS=I2C_MB_FINISH;
		while(I2C_GetFlagStatus(pI2CHandle->pI2Cx,I2C_BSY_FLAG)==FLAG_SET);
		while(I2C_GetFlagStatus(pI2CHandle->pI2Cx,I2C_ERROR_FLAG)==FLAG_SET);
		*pTxBuffer=pI2CHandle->pI2Cx->MDR;
	}
}


uint8_t I2C_GetFlagStatus(I2C0_Type *pI2Cx, uint8_t FlagName){
	if(pI2Cx->MCS & FlagName)
		return FLAG_SET;
	else
		return FLAG_RESET;
}
