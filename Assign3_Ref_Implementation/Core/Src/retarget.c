/**
 * @file 	retarget.c
 * @author	Clemens Koernyefalvy
 * @date	27/02/2023
 * @version	1.0
 * @brief	Retargeting the stdio.h printf() function towards the UART interface
 * 			on the STM32L4xx microcontroller.
 */

/**************** INCLUDES ****************/
#include "retarget.h"

/**************** PROTOTYPES ****************/

/**************** GLOBALS ****************/

/**************** FUNCTION DEFINITIONS ****************/
/**
  * @brief _write retargets the stdio.h printf() function with UART functionality.
  * @param 	file 	...	file descriptor
  * @param	ptr		... pointer to the data buffer.
  * @param	len 	...	length of the buffer in bytes.
  * @retval (int)	... Successfully transmitted bytes.
  */
int _write(int file, char *ptr, int len){
	while(len--){
		putchar(*ptr++);
	}
	return len;
}
/**
 *
 * @param ch		... Character to be transmitted.
 * @return	(int) 	... Returns the character.
 */
int	putchar (int ch){
	char charTemp = (char) ch;
	/*
	 * I've commented out the if-statement below, because it causes issues:
	 * When using printf() with only '\n' the output would not be sent to UART
	 * Instead, the output got buffered, until eventually it was full enough, so everything got flushed at once.
	 * When providing "\r\n" directly in the call to printf() it would immediatly flush.
	 * This is due to the implementation of printf() in newlib.
	 * This implicit workaround is unnecessary and circumvents the weird internal flush behaviour of printf()
	 * Normally '\n' triggers a flush, but on STM32 it appearantly is the '\r' which triggers it.
	 * Either that, or both '\r' and '\n' need to be provided.
	 */
//	if (charTemp == '\n'){
//		putchar('\r');
//	}
	HAL_UART_Transmit(&huart2, (uint8_t*)&charTemp, 1, -1);
	return ch;
}

/*
 * Some information about this retargeting code:
 * https://sourceware.org/newlib/libc.html#index-write
 * Appearantly there are problems when using this with FreeRTOS:
 * https://forum.digikey.com/t/easily-use-printf-on-stm32/20157/4 (see posts below article)
 * and also similar problems on microchip:
 * https://microchip.my.site.com/s/article/Execution-crashes-when-using-sprintf-in-a-FreeRTOS-task
 *
 * Also this thread was informative in this regard:
 * https://sourceware.org/pipermail/newlib/2009/007739.html
 *
 * It took me quite a long time to figure out, that this retarget.c is very problematic on FreeRTOS.
 * But as long as stack size is appropriately sized and float is not enabled, it is not a problem.
 */
