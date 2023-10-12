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

	//if (charTemp == '\n'){
		//putchar('\r');
	//}
	HAL_UART_Transmit(&huart2, (uint8_t*)&charTemp, 1, -1);
	return ch;
}
