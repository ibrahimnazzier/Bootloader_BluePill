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

/******************* static function declerations ***************************************************/
 
 /**
 \brief send NACK to the host 
 \details Utility function send NACK to the host
 **/
static void Bootloader_Send_NACK();

/**
 \brief send ACK to the host 
 \details Send ACK to the host, take the replay length from bootloader
 \details to inform the host the length of data that will be sent
 \in uint8_t Replay_length holds the lenght of packet
 **/
static void Bootloader_Send_ACK(uint8_t Replay_length);

/**
 \brief send Data to the host 
 \details Utility function to end Data to the host using UART
 \in uint32_t Data_len length of data that will be sent 
 \in uint8_t* Host_Buffer the buffer that holds the data that will be sent  
 **/
static void Bootloader_Send_Data_To_Host(uint8_t* Host_Buffer, uint32_t Data_len);

/**
 \brief Check if CRC value sent by user did not corrupted during the trasmission 
 \details	Check specific bytes in the command recieved by user and perforrm CRC
 calculation on the data part and calculate CRC and compare HOST value with its value
 \in uint8_t* pData pointer to char (string) to hold the data that CRC will be calculated in
 \in uint32_t Data_Len lenght of sent data 
 \in uint32_t Host_CRC Host calculated CRC to compare with MCU calculated
 \return A macro that inform that CRC verification status
 **/
static uint8_t Bootloader_CRC_Verify(uint8_t* pData, uint32_t Data_Len, uint32_t Host_CRC);


/**
 \brief First Command for getting Bootloader version 
 \details Recieve command from HOST and Replay with Current bootloader version
 \in uint8_t* Host_Buffer A buffer holds the command information 
 **/
static void Bootloader_Get_Version(uint8_t* Host_Buffer);
/******************* static function definition ***************************************************/

void Bootloader_Send_NACK()
{
	uint8_t Ack_value = CBL_SEND_NACK;
	HAL_UART_Transmit(BL_HOST_COMMUNICATION_UART,&Ack_value, 1 ,HAL_MAX_DELAY);
}
				/******************************************************/
 void Bootloader_Send_ACK(uint8_t Replay_length)
{
	uint8_t Ack_value[2] = {0};
	Ack_value[0] = CBL_SEND_ACK;
	Ack_value[1] = Replay_length;

	HAL_UART_Transmit(BL_HOST_COMMUNICATION_UART,(uint8_t*)Ack_value, 2 ,HAL_MAX_DELAY);
}
				/******************************************************/

void Bootloader_Send_Data_To_Host(uint8_t* Host_Buffer, uint32_t Data_len)
{
	HAL_UART_Transmit(BL_HOST_COMMUNICATION_UART,(uint8_t*)Host_Buffer, Data_len ,HAL_MAX_DELAY);

}
				/******************************************************/

int8_t Bootloader_CRC_Verify(uint8_t* pData, uint32_t Data_Len, uint32_t Host_CRC)
{
uint8_t CRC_Status = CRC_VERIFICATION_FAILED;
uint32_t MCU_CRC_Calculated = 0;
uint8_t Data_Counter = 0;
uint32_t Data_Buffer = 0;
for (;Data_Counter<Data_Len; Data_Counter++)
	{
		Data_Buffer = (uint32_t)pData[Data_Counter];
	MCU_CRC_Calculated = HAL_CRC_Accumulate(CRC_ENGINE_OBJ,&Data_Buffer,1);
	}
	
	__HAL_CRC_DR_RESET(CRC_ENGINE_OBJ);
	
if (MCU_CRC_Calculated == Host_CRC)	
	{
	 CRC_Status = CRC_VERIFICATION_PASSED;
		
	}
	else 
	{
		CRC_Status = CRC_VERIFICATION_FAILED;
	}
return CRC_Status;
}
				/******************************************************/

void Bootloader_Get_Version(uint8_t* Host_Buffer)
{
	uint8_t BL_Version[4] = {CBL_VENDOR_ID,CBL_SW_MAJOR_VERSION,CBL_SW_MINOR_VERSION,CBL_SW_PATCH_VERSION};
uint8_t Host_CMD_Packet_Len = 0;
uint32_t Host_CRC32 = 0;

#if(BL_DEBUG_ENABLE == DEBUG_INFO_ENABLE)
	BL_Print_Message("Get the boot loader version from ECU \r\n");
#endif
/* extract CRC and packet length sent by the host*/
Host_CMD_Packet_Len = Host_Buffer[0] + 1;
Host_CRC32 = *((uint32_t*)((Host_Buffer + Host_CMD_Packet_Len)-CRC_TYPE_SIZE_BYTE)); 

/* CRC Verification */
if(CRC_VERIFICATION_PASSED == Bootloader_CRC_Verify((uint8_t*)&Host_Buffer[0],Host_CMD_Packet_Len - 4,Host_CRC32)){
#if(BL_DEBUG_ENABLE == DEBUG_INFO_ENABLE)
		BL_Print_Message("CRC Verivication Passed \r\n");
#endif
		Bootloader_Send_ACK(4);
		Bootloader_Send_Data_To_Host((uint8_t*)BL_Version,4);
	}
else 
	{
#if(BL_DEBUG_ENABLE == DEBUG_INFO_ENABLE)
		BL_Print_Message("CRC Verivication Faild \r\n");
#endif
		Bootloader_Send_NACK();
	}

}
				/******************************************************/

/******************** Software Interfaces Decleration Starts ****************************************/
/**
 \brief Print Message over UART
 \details send message through UART used for Debugging 
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
						static void Bootloader_Get_Version(uint8_t* Host_Buffer);
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




