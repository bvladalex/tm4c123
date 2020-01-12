#include "tm4c123_uart_driver.h"
#include "TM4C123GH6PM_mcu1.h"
#include <stddef.h>

void UART_Init(UART_Handle_t *pUARTHandle){

	UART_PeriClockControl(pUARTHandle->pUARTx, ENABLE);
	//disable UART to implement configuration//
	pUARTHandle->pUARTx->CTL&=~(1U);
	//--------------Config section------------------------//
	if(pUARTHandle->UARTConfig.uart_baud==UART_BAUD_RATE_9600){
		pUARTHandle->pUARTx->IBRD=325;
		pUARTHandle->pUARTx->FBRD=33;
	}
	///parity config////
	if(pUARTHandle->UARTConfig.parity==UART_PARITY_ODD)
			pUARTHandle->pUARTx->LCRH|=(UART_PARITY_EN);
	else if(pUARTHandle->UARTConfig.parity==UART_PARITY_EVEN)
			pUARTHandle->pUARTx->LCRH|=(UART_PARITY_EN|(1U<<2));
	else if(pUARTHandle->UARTConfig.parity==UART_NO_PARITY)
			pUARTHandle->pUARTx->LCRH&=~(UART_PARITY_EN);
	///no of stop bits config///
	pUARTHandle->pUARTx->LCRH&=~(1U<<3);
	pUARTHandle->pUARTx->LCRH|=(pUARTHandle->UARTConfig.stop_bits<<3);
	//word legth config///
	pUARTHandle->pUARTx->LCRH&=~(3<<5);
	pUARTHandle->pUARTx->LCRH|=(pUARTHandle->UARTConfig.wd_len<<5);
	//mode config//
	pUARTHandle->pUARTx->CTL&=~(3U<<8);
	pUARTHandle->pUARTx->CTL|=(pUARTHandle->UARTConfig.uart_mode<<8);
	//clock source config
	pUARTHandle->pUARTx->CC&=~(15U);
	pUARTHandle->pUARTx->CC|=pUARTHandle->UARTConfig.clk_src;
	//---------------------------------------------------//
	//enable UART to implement configuration//
	pUARTHandle->pUARTx->CTL|=(1U);
}


void UART_PeriClockControl(UART0_Type *pUARTx, uint8_t EnorDi){
	if(EnorDi){
		if (pUARTx==UART0)
			UART0_PCLK_EN();
		else if(pUARTx==UART1)
			UART1_PCLK_EN();
		else if(pUARTx==UART2)
			UART2_PCLK_EN();
		else if(pUARTx==UART3)
			UART3_PCLK_EN();
		else if (pUARTx==UART4)
			UART4_PCLK_EN();
		else if(pUARTx==UART5)
			UART5_PCLK_EN();
		else if(pUARTx==UART6)
			UART6_PCLK_EN();
		else if(pUARTx==UART7)
			UART7_PCLK_EN();
		}
		else{
		if (pUARTx==UART0)
			UART0_PCLK_DIS();
		else if(pUARTx==UART1)
			UART1_PCLK_DIS();
		else if(pUARTx==UART2)
			UART2_PCLK_DIS();
		else if(pUARTx==UART3)
			UART3_PCLK_DIS();
		else if (pUARTx==UART4)
			UART4_PCLK_DIS();
		else if(pUARTx==UART5)
			UART5_PCLK_DIS();
		else if(pUARTx==UART6)
			UART6_PCLK_DIS();
		else if(pUARTx==UART7)
			UART7_PCLK_DIS();
		}
}

void UART_SendData(UART_Handle_t *pUARTHandle, uint8_t *pTxBuffer, uint32_t Len){
	//////IMPORTANT////ONLY 8 BIT WD LENGTH IS SUPPORTED IN THIS DRIVER AT THE MOMENT/////
	while(Len){
		while(!UART_GetFlagStatus(pUARTHandle->pUARTx,UART_FLAG_TXFE));
		pUARTHandle->pUARTx->DR=*pTxBuffer;
		Len--;
		pTxBuffer++;
		while(UART_GetFlagStatus(pUARTHandle->pUARTx,UART_FLAG_BUSY));
	}
}


uint8_t UART_GetFlagStatus(UART0_Type *pUARTx, uint8_t FlagName){
	if(pUARTx->FR & FlagName)
		return FLAG_SET;
	else
		return FLAG_RESET;
}