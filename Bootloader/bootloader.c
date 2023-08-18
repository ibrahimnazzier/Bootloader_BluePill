/**
 *****************************************************************************************************
 * @file           : bootloader.c
 * @author         : iNazzier
 * @brief          : Contains 
 *****************************************************************************************************
 */
 
/******************** Include Starts	*************************************************************/
#include "bootloader.h"

 
/******************** Include Ends 		*************************************************************/



/******************** Macro Decleration Starts ******************************************************/

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

/******************** Software Interfaces Definition Starts ****************************************/

void BL_Print_Message(char *format, ...){
	char Message[100] = {0};
	va_list arg;
		/* Enable Access to variable Arguments */
	va_start(arg, format);
		/* Write formatted data from variable argument list to string */
	vsprintf(Message, format,arg);
#if (BL_DEBUG_METHOD == BL_ENABLE_UART_DEBUG_MESSAGE)
		/* Transmit formatted data through UART*/
	HAL_UART_Transmit(BL_DEBUG_UART,(uint8_t*)Message, sizeof(Message), HAL_MAX_DELAY); 
#elif (BL_DEBUG_METHOD == BL_ENABLE_SPI_DEBUG_MESSAGE)
			/* Transmit formatted data through SPI*/
#elif (BL_DEBUG_METHOD == BL_ENABLE_CAN_DEBUG_MESSAGE)
				/* Transmit formatted data through CAN*/
#endif
		/* Perform clean up for an ap object initialized by a call to va start*/
	va_end(arg);
}


/******************* global variables declerations ***************************************************/

/******************* static function declerations ***************************************************/

/******************* static function definition ***************************************************/

