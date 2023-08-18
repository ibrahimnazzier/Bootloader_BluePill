/**
 *****************************************************************************************************
 * @file           : bootloader.h
 * @author         : iNazzier
 * @brief          : Contains Bootloader utilities
 *****************************************************************************************************
 */

#ifndef BOOTLOADER_H 
#define BOOTLOADER_H

/******************** Include Starts	*************************************************************/
#include <string.h>
#include <stdio.h>
#include <stdarg.h>
#include "usart.h"
#include "crc.h"
/******************** Include Ends 		*************************************************************/



/******************** Macro Decleration Starts ******************************************************/
#define BL_ENABLE_UART_DEBUG_MESSAGE		0x00
#define BL_ENABLE_SPI_DEBUG_MESSAGE 		0x01
#define BL_ENABLE_CAN_DEBUG_MESSAGE		0x02
#define BL_DEBUG_METHOD (BL_ENABLE_UART_DEBUG_MESSAGE)

#define BL_DEBUG_UART 				&huart3

/******************** Macro Decleration Ends ********************************************************/



/******************** Macro Function Decleration Starts *********************************************/

/******************** Macro Function Decleration Ends ***********************************************/



/******************** Data Type Decleration Starts **************************************************/

/******************** Data Type Decleration Ends ****************************************************/



/******************** Software Interfaces Decleration Starts ****************************************/
/**
 \brief Print Message over UART
 \details set send message through UART used for Debugging 
 \param [in] format specifiy the behavior of the variadic function
 \param [...] Variadic function.
 **/

void BL_Print_Message(char *format, ...);


/******************** Software Interfaces Decleration Ends ****************************************/



#endif /* BOOTLOADER_H */
 