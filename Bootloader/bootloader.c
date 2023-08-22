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

/******************* global variables declerations ***************************************************/
static uint8_t BL_Host_Buffer[BL_HOST_BUFFER_RX_LENGTH];


/******************** Software Interfaces Decleration Starts ****************************************/
/**
 \brief Print Message over UART
 \details set send message through UART used for Debugging 
 \param [in] format specifiy the behavior of the variadic function
 \param [...] Variadic function.
 **/
void BL_Print_Message(char *format, ...);


/**
 \brief Receive messages from user 
 \details Receive Commands from user using UART 
 \return Return Enum as operation done (BL_ok) or not (BL_NAK)  
 **/
BL_Status BL_UART_Host_Fetch_Command(void);

/******************** Software Interfaces Decleration Ends ****************************************/

/******************** Software Interfaces Definition Starts ****************************************/
/**
 \brief Print Message over UART
 \details set send message through UART used for Debugging 
 \param [in] format specifiy the behavior of the variadic function
 \param [...] Variadic function.
 **/

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

/********************************************************************************************/

/**
 \brief Recieve messages from user 
 \details Receive Commands from user using UART 
 \return Return Enum as operation done (BL_ok) or not (BL_NAK)  
 **/

BL_Status BL_UART_Host_Fetch_Command(void){
		BL_Status Status = BL_NACk;
		HAL_StatusTypeDef HAL_Status = HAL_ERROR; 
		uint8_t Data_Length = 0;
		memset(BL_Host_Buffer,0,BL_HOST_BUFFER_RX_LENGTH);
		
		HAL_Status = HAL_UART_Receive(BL_HOST_COMMUNICATION_UART, BL_Host_Buffer,1,HAL_MAX_DELAY);
		if (HAL_Status != HAL_OK)
		{
			Status = BL_NACk;

		}
		else {
			Data_Length = BL_Host_Buffer[0];
			HAL_UART_Receive(BL_HOST_COMMUNICATION_UART,&BL_Host_Buffer[1],Data_Length,HAL_MAX_DELAY);
			if (HAL_Status != HAL_OK)
			{
				Status = BL_NACk;
			}
			else {
			switch(BL_Host_Buffer[1])
				{		
					case CBL_GET_VER_CMD:
						BL_Print_Message("CBL_GET_VER_CMD \r\n");
						Status = BL_OK;
						break;
				
					case CBL_GET_HELP_CMD:
						BL_Print_Message("CBL_GET_HELP_CMD \r\n");
						break;
					
					case CBL_GET_CID_CMD:
						BL_Print_Message("CBL_GET_CID_CMD \r\n");

						break;
					
					case CBL_GET_RDP_STATUS_CMD:
						BL_Print_Message("CBL_GET_RDP_STATUS_CMD \r\n");
						break;
					
					case CBL_GO_TO_ADDR_CMD:
						BL_Print_Message("CBL_GO_TO_ADDR_CMD \r\n");
						break;
					
					case CBL_FLASH_ERASE_CMD:
						BL_Print_Message("CBL_FLASH_ERASE_CMD \r\n");
						break;	
					
					case CBL_MEM_WRITE_CMD:
						BL_Print_Message("CBL_MEM_WRITE_CMD \r\n");
						break;
					
					case CBL_ED_W_PROTECT_CMD:
						BL_Print_Message("CBL_ED_W_PROTECT_CMD \r\n");
						break;
				
					case CBL_MEM_READ_CMD:
						BL_Print_Message("CBL_MEM_READ_CMD \r\n");
						break;
					
					case CBL_READ_SECTOR_STATUS_CMD:
						BL_Print_Message("CBL_READ_SECTOR_STATUS_CMD \r\n");
						break;
				
					case CBL_OTP_READ_CMD:
						BL_Print_Message("CBL_OTP_READ_CMD \r\n");
						break;
					case CBL_CHANGE_ROP_Level_CMD:
						BL_Print_Message("CBL_CHANGE_ROP_Level_CMD \r\n");
						break;
					default:
						BL_Print_Message("Invalid Command code received from host!!\r\n");
						break;
				}
			}

		}
		return Status;
}



/******************* static function declerations ***************************************************/

/******************* static function definition ***************************************************/

