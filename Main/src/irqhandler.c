#include "irqhandler.h"

// 数据传输完成，产生中断，检查是否还有没有传输的数据，继续传输
void DMA2_Stream7_IRQHandler(void) {
	if (DMA_GetITStatus(DMA2_Stream7, DMA_IT_TCIF7) == SET) {
		DMA_ClearFlag(DMA2_Stream7, DMA_IT_TCIF7);	// 清除中断标志
		DMA_ClearITPendingBit(DMA2_Stream7, DMA_IT_TCIF7);
	}
}

// 数据传输完成,产生中断,检查是否还有没有传输的数据，继续传输
void DMA1_Stream6_IRQHandler(void) {
	if (DMA_GetITStatus(DMA1_Stream6, DMA_IT_TCIF6) == SET) {
		DMA_ClearFlag(DMA1_Stream6, DMA_IT_TCIF6);	// 清除中断标志
		DMA_ClearITPendingBit(DMA1_Stream6, DMA_IT_TCIF6);
	}
}

void USART1_IRQHandler(void) {
	u8 temp;
	if (USART_GetITStatus(USART1, USART_IT_RXNE) != RESET) {
		USART_ClearFlag(USART1, USART_IT_RXNE);
		USART_ClearITPendingBit(USART1, USART_IT_RXNE);
		temp = USART_ReceiveData(USART1);
		// blueToothReceive(temp);
	}
}

void USART2_IRQHandler(void) {
	u8 temp;
	if (USART_GetITStatus(USART2, USART_IT_ORE_RX) != RESET) {
		temp = USART_ReceiveData(USART2);
		USART_ClearFlag(USART2, USART_FLAG_ORE);	// 清除溢出中断
	}
	if (USART_GetITStatus(USART2, USART_IT_RXNE) != RESET) {
		USART_ClearFlag(USART2, USART_FLAG_RXNE);
		USART_ClearITPendingBit(USART2, USART_IT_RXNE);
		temp = USART_ReceiveData(USART2);
		yesenseReceiveHandler(&robot.yesense, temp);
	}
}

// 1ms
void TIM2_IRQHandler(void) {
	if (TIM_GetITStatus(TIM2, TIM_IT_Update) == SET) {
		GlobalTimer++;	// 全局计时器 代码的全局时间可都靠这个
		canSend(0x3);
	}
	TIM_ClearITPendingBit(TIM2, TIM_IT_Update);	 // 清除中断标志位
}

// 100ms
void TIM3_IRQHandler(void) {
	if (TIM_GetITStatus(TIM3, TIM_IT_Update) == SET) {
		DJmotorMonitor(djmotor);
		TmotorMonitor(tmotor);
		RobotStateUpdate(&robotstate);

		// robot 状态检测
		robotTorqueMonitor();
	}
	TIM_ClearITPendingBit(TIM3, TIM_IT_Update);
}

void CAN1_RX0_IRQHandler(void) {
	CAN_ClearITPendingBit(CAN1, CAN_IT_FMP0);
	CAN_ClearFlag(CAN1, CAN_IT_FMP0);
	if (CAN_GetITStatus(CAN1, CAN_IT_FMP0) != RESET) {
		CanRxMsg rxMsg;
		CAN_Receive(CAN1, CAN_FIFO0, &rxMsg);
		TmotorreceiveHandle(tmotor, rxMsg);
	}
}

void CAN2_RX0_IRQHandler(void) {
	CAN_ClearITPendingBit(CAN2, CAN_IT_FMP0);
	CAN_ClearFlag(CAN2, CAN_IT_FMP0);
	if (CAN_GetITStatus(CAN2, CAN_IT_FMP0) != RESET) {
		CanRxMsg rxMsg;
		CAN_Receive(CAN2, CAN_FIFO0, &rxMsg);
		DJmotorreceiveHandle(djmotor, rxMsg);
	}
}
