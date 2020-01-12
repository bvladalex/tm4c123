#ifndef INC_TM4C123_I2C_DRV_HEADER
#define INC_TM4C123_I2C_DRV_HEADER

#include "TM4C123GH6PM_mcu1.h"

typedef struct{
	uint8_t i2c_mode;
	uint8_t i2c_clock_set;
  uint8_t i2c_slave_address;
	uint8_t i2c_master_slave;
}I2C_Config_t;


typedef struct{
	I2C0_Type *pI2Cx;
	I2C_Config_t I2CConfig;
	uint8_t *pRxBuffer;
	uint8_t *pTxBuffer;
	uint32_t TxLen;
	uint32_t RxLen;
	uint8_t TxRxState;
	uint8_t DevAddress;
	uint32_t RxSize;
	uint8_t Sr;
}I2C_Handle_t;

/////I2C possible states////

#define I2C_IDLE 					0
#define I2C_BUSY_IN_TX		1
#define I2C_BUSY_IN_RX		2

#define I2C_HS_MODE				1U
#define I2C_NON_HS_MODE		0

#define I2C_MODE_MASTER 	1U<<4
#define I2C_MODE_SLAVE		1U<<5

#define I2C_MODE_WRITE 		0
#define I2C_MODE_READ 		1U


#define I2C_TPR_VAL_1 		1U
#define I2C_TPR_VAL_2 		1U<<1
#define I2C_TPR_VAL_4 		1U<<2
#define I2C_TPR_VAL_8 		1U<<3
#define I2C_TPR_VAL_16		1U<<4
#define I2C_TPR_VAL_32		1U<<5

//#define I2C_MODULE_NO_0   1U<<0
//#define I2C_MODULE_NO_1   1U<<1
//#define I2C_MODULE_NO_2   1U<<2
//#define I2C_MODULE_NO_3   1U<<3

//////I2C slave macros////////////////
#define I2C_SLV_RREQ		1U
#define I2C_SLV_TREQ		1U<<1

/////////MACROS for I2C configuration/////////


/*Macros and config options for I2C Master/ Control reg///

-> Status Mode bit config 
+-------+--------+------+--------+--------+--------+-------+------+
|   B7  |   B6   |  B5  |   B4   |   B3   |   B2   |   B1  |  B0  |
+-------+--------+------+--------+--------+--------+-------+------+
| CLKTO | BUSBSY | IDLE | ARBLST | DATACK | ADRACK | ERROR | BUSY |
+-------+--------+------+--------+--------+--------+-------+------+
		|				|				|				|					|				|				|				|Controller Busy
		|				|				|				|					|				|				|Error on last op.
		|				|				|				|					|				|Transmitted addr was ACKed		
		|				|				|				|					|Transmitted data was ACKed
		|				|				|				|I2C controler won arbitration
		|				|				|I2C controller is idle
		|				|I2C bus is busy
		|Clock timeout error
		

		
-> Control Mode bit config

+-----+-----+-----+----+-----+------+-------+-----+
|  B7 |  B6 |  B5 | B4 |  B3 |  B2  |   B1  |  B0 |
+-----+-----+-----+----+-----+------+-------+-----+
| RES | RES | RES | HS | ACK | STOP | START | RUN |
+-----+-----+-----+----+-----+------+-------+-----+


+---------+-----------+--------------------------+------------------------------------------------------+
| Current | I2CMSA[0] | I2CMCS[3:0]              | Description                                          |
| state   +-----------+-----+------+-------+-----+                                                      |
|         | R/S       | ACK | STOP | START | RUN |                                                      |
+---------+-----------+-----+------+-------+-----+------------------------------------------------------+
| Idle    | 0         | X   | 0    | 1     | 1   | Start condition followed by Master transmit          |
|         +-----------+-----+------+-------+-----+------------------------------------------------------+
|         | 0         | X   | 1    | 1     | 1   | Start condition followed by Master transmit          |
|         |           |     |      |       |     | then STOP                                            |
|         +-----------+-----+------+-------+-----+------------------------------------------------------+
|         | 1         | 0   | 0    | 1     | 1   | Start condition followed by receive and then neg ACK |
|         +-----------+-----+------+-------+-----+------------------------------------------------------+
|         | 1         | 0   | 1    | 1     | 1   | Start condition followed by receive and then STOP    |
|         +-----------+-----+------+-------+-----+------------------------------------------------------+
|         | 1         | 1   | 0    | 1     | 1   | Start condition followed by receive                  |
|         +-----------+-----+------+-------+-----+------------------------------------------------------+
|         | 1         | 1   | 1    | 1     | 1   | Illegal                                              |
|         +-----------+-----+------+-------+-----+------------------------------------------------------+
|         | Combinations not listed are NOP      | NOP                                                  |
+---------+--------------------------------------+------------------------------------------------------+


+---------+-----------+--------------------------+------------------------------------------------------+
| Current | I2CMSA[0] | I2CMCS[3:0]              | Description                                          |
| state   +-----------+-----+------+-------+-----+                                                      |
|         | R/S       | ACK | STOP | START | RUN |                                                      |
+---------+-----------+-----+------+-------+-----+------------------------------------------------------+
| TX      | x         | x   | 0    | 0     | 1   | Transmit ->          
|         +-----------+-----+------+-------+-----+------------------------------------------------------+
|         | x         | x   | 1    | 0     | 0   | STOP ->                                            
|         +-----------+-----+------+-------+-----+------------------------------------------------------+
|         | x         | x   | 1    | 0     | 1   | Transmit -> STOP 
|         +-----------+-----+------+-------+-----+------------------------------------------------------+
|         | 0         | x   | 0    | 1     | 1   | Repeated START -> Transmit   
|         +-----------+-----+------+-------+-----+------------------------------------------------------+
|         | 0         | x   | 1    | 1     | 1   | Repeated START -> Transmit -> STOP                
|         +-----------+-----+------+-------+-----+------------------------------------------------------+
|         | 1         | 0   | 0    | 1     | 1   | Repeated START -> Receive -> NACK                                    
|         +-----------+-----+------+-------+-----+------------------------------------------------------+
|         | 1         | 0   | 1    | 1     | 1   | Repeated START -> Transmit -> STOP                                   
|         +-----------+-----+------+-------+-----+------------------------------------------------------+
|         | 1         | 1   | 0    | 1     | 1   | Repeated START -> Receive                                            
|         +-----------+-----+------+-------+-----+------------------------------------------------------+
|         | 1         | 1   | 1    | 1     | 1   | Illegal                                              
+---------+--------------------------------------+------------------------------------------------------+

+---------+-----------+--------------------------+------------------------------------------------------+
| Current | I2CMSA[0] | I2CMCS[3:0]              | Description                                          |
| state   +-----------+-----+------+-------+-----+                                                      |
|         | R/S       | ACK | STOP | START | RUN |                                                      |
+---------+-----------+-----+------+-------+-----+------------------------------------------------------+
| RX      | x         | 0   | 0    | 0     | 1   | Receive -> NACK           
|         +-----------+-----+------+-------+-----+------------------------------------------------------+
|         | x         | x   | 1    | 0     | 0   | STOP ->                                            
|         +-----------+-----+------+-------+-----+------------------------------------------------------+
|         | x         | 0   | 1    | 0     | 1   | Receive -> STOP 
|         +-----------+-----+------+-------+-----+------------------------------------------------------+
|         | x         | 1   | 0    | 0     | 1   | Receive   
|         +-----------+-----+------+-------+-----+------------------------------------------------------+
|         | x         | 1   | 1    | 0     | 1   | Illegal                
|         +-----------+-----+------+-------+-----+------------------------------------------------------+
|         | 1         | 0   | 0    | 1     | 1   | Repeated START -> Receive -> NACK                                    
|         +-----------+-----+------+-------+-----+------------------------------------------------------+
|         | 1         | 0   | 1    | 1     | 1   | Repeated START -> Transmit -> STOP                                   
|         +-----------+-----+------+-------+-----+------------------------------------------------------+
|         | 1         | 1   | 0    | 1     | 1   | Repeated START -> Receive                                            
|         +-----------+-----+------+-------+-----+------------------------------------------------------+
|         | 0         | x   | 0    | 1     | 1   | Repeated START -> Transmit      
|         +-----------+-----+------+-------+-----+------------------------------------------------------+
|         | 0         | x   | 1    | 1     | 1   | Repeated START -> Transmit -> STOP  
+---------+--------------------------------------+------------------------------------------------------+


*/
///////////////Flag macros////////////////
#define I2C_BSY_FLAG     	(1U<<0)
#define I2C_ERROR_FLAG    (1U<<1)
#define I2C_ADRACK_FLAG		(1U<<2)
#define I2C_DATACK_FLAG		(1U<<3)
#define I2C_ARBLST_FLAG		(1U<<4)
#define I2C_IDLE_FLAG			(1U<<5)
#define I2C_BUSBSY_FLAG		(1U<<6)
#define I2C_CLKTO_FLAG		(1U<<7)

///////////////Write commands/////////////
#define I2C_MB_FINISH     ((1U<<0)|(1U<<2))
#define I2C_TX						(1U<<0)
#define I2C_STA_TX				((1U<<0)|(1U<<1))
#define I2C_STA_TX_STP		((1U<<0)|(1U<<1)|(1U<<2))
///////////////Read commands//////////////
#define I2C_STA_RX_N_ACK	((1U<<0)|(1U<<1))
#define I2C_STA_RX_STP		((1U<<0)|(1U<<1)|(1U<<2))
#define I2C_STA_RX				((1U<<0)|(1U<<1)|(1U<<3))
#define I2C_RX						((1U<<0)|(1U<<3))

#define I2C_MODE_HS  			(1U<<7)

#define I2C_EN_SR					ENABLE
#define I2C_DIS_SR 				DISABLE

///////////////////API declaration section//////////

uint8_t I2C_GetFlagStatus(I2C0_Type *pI2Cx, uint8_t FlagName);
//Peripheral clock setup
void I2C_PeriClockControl(I2C0_Type *pI2Cx, uint8_t EnorDi);



//Init and De-init
void I2C_Init(I2C_Handle_t *pI2CHandle);
void I2C_DeInit(I2C0_Type *pI2Cx);
		 
void I2C_master_disable(I2C0_Type *pI2Cx);
void I2C_master_enable(I2C0_Type *pI2Cx);

void I2C_slave_disable(I2C0_Type *pI2Cx);
void I2C_slave_enable(I2C0_Type *pI2Cx);

//////Send and receive data////

void I2C_SendData(I2C_Handle_t *pI2CHandle, uint8_t *pTxBuffer, uint32_t Len, uint8_t slave_addr, uint8_t i2c_enordis_sr);
		 
void I2C_ReceiveData(I2C_Handle_t *pI2CHandle, uint8_t *pRxBuffer, uint32_t Len, uint8_t slave_addr);

//////Send and receive data as slave////

void I2C_SlaveSendData(I2C_Handle_t *pI2CHandle, uint8_t *pTxBuffer, uint8_t slave_addr);
		 
void I2C_SlaveReceiveData(I2C_Handle_t *pI2CHandle, uint8_t *pRxBuffer, uint8_t slave_addr);


//////Send and receive data with interrupts////

uint8_t I2C_SendDataIT(I2C_Handle_t *pI2CHandle, uint8_t *pTxBuffer, uint32_t Len, uint8_t slave_addr, uint8_t i2c_enordis_sr);
		 
uint8_t I2C_ReceiveDataIT(I2C_Handle_t *pI2CHandle, uint8_t *pRxBuffer, uint32_t Len, uint8_t slave_addr);

////Core interrupt configuration/////

void I2C_IRQInterruptConfig(uint8_t IRQNumber, uint8_t EnorDis);
void I2C_IRQPriorityConfig(uint8_t IRQNumber,uint8_t IRQPriority);

void I2C_EV_IRQHandling(I2C_Handle_t *pI2CHandle);
void I2C_ER_IRQHandling(I2C_Handle_t *pI2CHandle);

#endif //INC_TM4C123_I2C_DRV_HEADER


