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
/*Array of uint8_t to receive data from host with length @BL_HOST_BUFFER_RX_LENGTH  */
static uint8_t BL_Host_Buffer[BL_HOST_BUFFER_RX_LENGTH];

/*Static array with 12 elements holding hex value of the existing commands  */
static uint8_t Bootloader_Supported_CMDs[12] = {
    CBL_GET_VER_CMD,
    CBL_GET_HELP_CMD,
    CBL_GET_CID_CMD,
    CBL_GET_RDP_STATUS_CMD,
    CBL_GO_TO_ADDR_CMD,
    CBL_FLASH_ERASE_CMD,
    CBL_MEM_WRITE_CMD,
    CBL_ED_W_PROTECT_CMD,
    CBL_MEM_READ_CMD,
    CBL_READ_SECTOR_STATUS_CMD,
    CBL_OTP_READ_CMD,
    CBL_CHANGE_ROP_Level_CMD
};

/******************* static function declerations ***************************************************/
 /**************************** utility functions **************************************/
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
\brief Get_RDP_Level summarise code for gitting RDP in this function
\return returns decimal number indecating RDP level.
**/
static uint8_t CBL_STM32F103_Get_RDP_Level();

 /**
 \brief utility function for erasing specific sectors or erasing all flash
 \details This utility uses the address specified from Bootloader_Jump_To_Address() and go to it 
 \in uint32_t jump_address --> the required jump address specified by host 
 \return ADDRESS_IS_VALID macro to specify if jump process done successfully  
 \return ADDRESS_IS_INVALID macro to specify if jump process faild
 **/
static uint8_t HOST_Jump_Address_Verification( uint32_t jump_address);

  /**
 \brief utility function for jumping for specific address 
 \details This utility uses the address and number of sectors specified by Bootloader_Erase_Flash()
 \details and start erasing from this address to the specified sector if address is 0xff full erase performed 
 \in uint8_t Sector_Number number of sector to start erase from (changed internally to specific address)  
 \in uint8_t Number_Of_Sector for the number of sectors in case of sector erase (not full erase)
 \return SUCCESSFUL_ERASE macro to specify if erase process done successfully  
 \return UNSUCCESSFUL_ERASE macro to specify if erase process faild
 \return INVALID_SECTOR_NUMBER macro if sector number is out of range
 **/
static uint8_t Perform_Flash_Erase(uint8_t Sector_Number, uint8_t Number_Of_Sector);
  /**
 \brief utility function for writing specific address 
 \details This utility take the address of first 16 byte of payload and write that sequentially in memory
 \in uint16_t* Host_payload pointer to the address of the first 16 byte in payload
 \in uint32_t Payload_Start_address holds the address of the first 16 byte 
 \in uint16_t Payload_Len size of payload in bytes
 \return FLASH_PAYLOAD_WRITE_PASSED macro to specify if write process done successfully  
 \return FLASH_PAYLOAD_WRITE_FAILED macro to specify if write process faild
 **/
static uint8_t Flash_Memory_Write_Payload(uint16_t* Host_payload, uint32_t Payload_Start_address, uint16_t Payload_Len);

/************************************* boot loader commands *********************************************************************/
/**
 \brief First Command for getting Bootloader version 
 \details Recieve command from HOST and Replay with Current bootloader version
 \in uint8_t* Host_Buffer A buffer holds the command information 
 **/
static void Bootloader_Get_Version(uint8_t* Host_Buffer);

/**
 \brief Second Command for helping user   
 \details Recieve command from HOST and Replay with existing commands in the Bootloader
 \in uint8_t* Host_Buffer A buffer holds the command information 
 **/
static void Bootloader_Get_Help(uint8_t* Host_Buffer);

/**
 \brief Third Command for Getting chip identification number   
 \details Recieve command from HOST and Replay with chip identification number for the MCU
 \in uint8_t* Host_Buffer A buffer holds the command information 
 **/
static void Bootloader_Get_Chip_Identification_Number(uint8_t *Host_Buffer);

/**
 \brief forth Command for Getting read protection level of flash    
 \details Recieve command from HOST and Replay with protection level specified in registers
 \in uint8_t* Host_Buffer A buffer holds the command information 
 **/
 static void Bootloader_Read_Protection_Level(uint8_t *Host_Buffer);
 
 /**
 \brief forth Command for Jumping to specific address     
 \details Recieve command from HOST specified address to jump and go to this address 
 \in uint8_t* Host_Buffer A buffer holds the command information 
 **/

 static void Bootloader_Jump_To_Address(uint8_t *Host_Buffer);

 /**
 \brief forth Command for Erasing flash at specific address      
 \details Recieve command from HOST specified address to Erase memory   
 \details starting from this address to the specified number of sectors
 \in uint8_t* Host_Buffer A buffer holds the command information 
 **/
static void Bootloader_Erase_Flash(uint8_t *Host_Buffer);
 
 /**
 \brief fifth Command for writing flash at specific address      
 \details Recieve command from HOST specified address to writne in specific address   
 \details Writing recieved data from payload to specific memory address 
 \in uint8_t* Host_Buffer A buffer holds the command information 
 **/
void Bootloader_Memory_Write(uint8_t* Host_Buffer);
/******************* static function definition ***************************************************/

 /**************************** utility functions **************************************/

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

uint8_t Bootloader_CRC_Verify(uint8_t* pData, uint32_t Data_Len, uint32_t Host_CRC)
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
static uint8_t CBL_STM32F103_Get_RDP_Level()
{
	FLASH_OBProgramInitTypeDef FLASH_OBProgram;
	HAL_FLASHEx_OBGetConfig(&FLASH_OBProgram);
	return (uint8_t)(FLASH_OBProgram.RDPLevel);
}
				/******************************************************/
uint8_t HOST_Jump_Address_Verification( uint32_t jump_address)
{
	uint8_t Address_Verification = ADDRESS_IS_INVALID;
if ((jump_address >= SRAM_BASE) && (jump_address <= STM32f103_SRAM_END))
	{
		Address_Verification = ADDRESS_IS_VALID;
	}
	else if ((jump_address >= FLASH_BASE) && (jump_address <= STM32f103_FLASH_END))
	{
		Address_Verification = ADDRESS_IS_VALID;
	}
	else 
	{
		Address_Verification = ADDRESS_IS_INVALID;
#if(BL_DEBUG_ENABLE == DEBUG_INFO_ENABLE)
	BL_Print_Message("Address Verification is not valid. \r\n");
#endif

	}
	return Address_Verification;
}
			/******************************************************/
static uint8_t Perform_Flash_Erase(uint8_t Sector_Number, uint8_t Number_Of_Sector)
{
	uint8_t Sector_Validity_Status = INVALID_SECTOR_NUMBER;
	FLASH_EraseInitTypeDef pEraseInit;
	uint8_t Remaining_Sectors = 0;
	HAL_StatusTypeDef HAL_Status = HAL_ERROR;
	uint32_t Sector_error = 0;

	if (Number_Of_Sector > CBL_FLASH_MAX_SECTOR_NUMBER)
	{
			Sector_Validity_Status = INVALID_SECTOR_NUMBER;	
			}
	
	else {
		if ((Sector_Number <= (CBL_FLASH_MAX_SECTOR_NUMBER-1)) || (BL_FLASH_MASS_ERASE == Sector_Number))
			{
				if (BL_FLASH_MASS_ERASE == Sector_Number)
				{
					pEraseInit.TypeErase = FLASH_TYPEERASE_MASSERASE;	/* Flash Mass Erase Activation  */ 
#if (BL_DEBUG_ENABLE == DEBUG_INFO_ENABLE)
					BL_Print_Message("Flash Mass Erase Activation \r\n");
#endif
				}
				else 
				{
					/*Sector Erase Mode */
#if (BL_DEBUG_ENABLE == DEBUG_INFO_ENABLE)
				BL_Print_Message("Flash Sector Erase Mode \r\n");
#endif
				Remaining_Sectors = CBL_FLASH_MAX_SECTOR_NUMBER - Sector_Number;
					if (Number_Of_Sector > Remaining_Sectors)
					{
						Number_Of_Sector = Remaining_Sectors;
					}
					else
				{
				/* nothing */
				}
				pEraseInit.TypeErase = FLASH_TYPEERASE_PAGES; /* page erase*/
				pEraseInit.PageAddress = Sector_Number*1024 + FLASH_BASE; 	/*!< PageAdress: Initial FLASH page address to erase when mass erase is disabled */
				pEraseInit.NbPages= Number_Of_Sector;			/* number of sectors to be erased */
				}
				
				pEraseInit.Banks = FLASH_BANK_1;
				
				/* unlock the flash control register access */ 
				HAL_Status = HAL_FLASH_Unlock();
				/* perform a mass erase or sector erase */ 
				HAL_Status	= HAL_FLASHEx_Erase(&pEraseInit, &Sector_error);
				if (HAL_SUCCESSFUL_ERASE == Sector_error)
					{
						Sector_Validity_Status = SUCCESSFUL_ERASE;
#if (BL_DEBUG_ENABLE == DEBUG_INFO_ENABLE)
				BL_Print_Message("Flash Sector Erase successful  \r\n");
#endif
					}
					else 
					{
						 Sector_Validity_Status = UNSUCCESSFUL_ERASE;
#if (BL_DEBUG_ENABLE == DEBUG_INFO_ENABLE)
				BL_Print_Message("Flash Sector Erase insuccessful  \r\n");
#endif
					}
				/* lock the flash control register access */ 
				HAL_Status = HAL_FLASH_Lock();
			}
	else 
			{
				Sector_Validity_Status = UNSUCCESSFUL_ERASE;
#if (BL_DEBUG_ENABLE == DEBUG_INFO_ENABLE)
				BL_Print_Message("Flash Sector Erase unsuccessful  \r\n");
#endif
			}

	}
	return Sector_Validity_Status;
}

static uint8_t Flash_Memory_Write_Payload(uint16_t* Host_payload, uint32_t Payload_Start_address, uint16_t Payload_Len)
{
	HAL_StatusTypeDef HAL_Status = HAL_ERROR;
	uint8_t Flash_Payload_Write_Status = FLASH_PAYLOAD_WRITE_FAILED;
	uint16_t Payload_Counter = 0;
	
	/* Unlock the flash control register access */ 
	HAL_Status = HAL_FLASH_Unlock();
		if(HAL_Status != HAL_OK)
		{
			Flash_Payload_Write_Status = FLASH_PAYLOAD_WRITE_FAILED;
		}
		else 
		{
			for(int PL_add = 0,Payload_Counter = 0; Payload_Len/2 > Payload_Counter; PL_add+=2,Payload_Counter++)
			{
				HAL_Status = HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD,(Payload_Start_address + PL_add), Host_payload[Payload_Counter]);
				if(HAL_Status != HAL_OK)
				{
					Flash_Payload_Write_Status = FLASH_PAYLOAD_WRITE_FAILED;
					break;
				}
				Flash_Payload_Write_Status = FLASH_PAYLOAD_WRITE_PASSED;
			}	
		}
	if ((FLASH_PAYLOAD_WRITE_PASSED == Flash_Payload_Write_Status) && (HAL_OK == HAL_Status))
	{
	/* Lock the flash control register access */ 
		HAL_Status = HAL_FLASH_Lock();
		if(HAL_Status != HAL_OK)
		{
			Flash_Payload_Write_Status = FLASH_PAYLOAD_WRITE_FAILED;
		}
		else{
			Flash_Payload_Write_Status = FLASH_PAYLOAD_WRITE_PASSED;
		}
	}
	else 
	{
		Flash_Payload_Write_Status = FLASH_PAYLOAD_WRITE_FAILED;
	}
	return Flash_Payload_Write_Status;
}

/************************************* boot loader commands *********************************************************************/

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

void Bootloader_Get_Help(uint8_t* Host_Buffer)
{
	uint8_t Host_CMD_Packet_Len = 0;
	uint32_t Host_CRC32 = 0;

#if(BL_DEBUG_ENABLE == DEBUG_INFO_ENABLE)
	BL_Print_Message("Read the commands supported by bootloader \r\n");
#endif

	/* extract CRC and packet length sent by the host*/
	Host_CMD_Packet_Len = Host_Buffer[0] + 1;
	Host_CRC32 = *((uint32_t*)((Host_Buffer + Host_CMD_Packet_Len)-CRC_TYPE_SIZE_BYTE)); 
	/* CRC Verification */
if(CRC_VERIFICATION_PASSED == Bootloader_CRC_Verify((uint8_t*)&Host_Buffer[0],Host_CMD_Packet_Len - 4,Host_CRC32)){
#if(BL_DEBUG_ENABLE == DEBUG_INFO_ENABLE)
		BL_Print_Message("CRC Verivication Passed \r\n");
#endif
		Bootloader_Send_ACK(12);
		Bootloader_Send_Data_To_Host((uint8_t*)Bootloader_Supported_CMDs,12);
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
void Bootloader_Get_Chip_Identification_Number(uint8_t *Host_Buffer)
{
	uint16_t Host_CMD_Packet_Len = 0;
  uint32_t Host_CRC32 = 0;
	uint16_t MCU_Identification_Number = 0;
#if (BL_DEBUG_ENABLE == DEBUG_INFO_ENABLE)
	BL_Print_Message("Read the MCU chip identification number \r\n");
#endif
	/* Extract the CRC32 and packet length sent by the HOST */
	Host_CMD_Packet_Len = Host_Buffer[0] + 1;
	Host_CRC32 = *((uint32_t *)((Host_Buffer + Host_CMD_Packet_Len) - CRC_TYPE_SIZE_BYTE));
	/* CRC Verification */
	if(CRC_VERIFICATION_PASSED == Bootloader_CRC_Verify((uint8_t *)&Host_Buffer[0] , Host_CMD_Packet_Len - 4, Host_CRC32)){
#if (BL_DEBUG_ENABLE == DEBUG_INFO_ENABLE)
		BL_Print_Message("CRC Verification Passed \r\n");
#endif
		/* Get the MCU chip identification number */ 
		MCU_Identification_Number = (uint16_t)((DBGMCU->IDCODE) & 0x00000FFF);
		/* Report chip identification number to HOST */
		Bootloader_Send_ACK(2);
		Bootloader_Send_Data_To_Host((uint8_t *)&MCU_Identification_Number, 2);
	}
	else{
#if (BL_DEBUG_ENABLE == DEBUG_INFO_ENABLE)
		BL_Print_Message("CRC Verification Failed \r\n");
#endif
		Bootloader_Send_NACK();
	}
}
									/******************************************************/
static void Bootloader_Read_Protection_Level(uint8_t *Host_Buffer)
{
	uint16_t Host_CMD_Packet_Len = 0;
	uint32_t Host_CRC32 = 0;
	uint8_t RDP_Level = 0;
	uint8_t RDP_Level_Error_Status = POR_LEVEL_READ_INVALID;

#if (BL_DEBUG_ENABLE == DEBUG_INFO_ENABLE)
	BL_Print_Message("Read the Flash Read	Protection out level \r\n");
#endif
		/* Extract the CRC32 and packet length sent by the HOST */
	Host_CMD_Packet_Len = Host_Buffer[0] + 1;
	Host_CRC32 = *((uint32_t *)((Host_Buffer + Host_CMD_Packet_Len) - CRC_TYPE_SIZE_BYTE));
	/* CRC Verification */
	if(CRC_VERIFICATION_PASSED == Bootloader_CRC_Verify((uint8_t *)&Host_Buffer[0] , Host_CMD_Packet_Len - 4, Host_CRC32)){
#if (BL_DEBUG_ENABLE == DEBUG_INFO_ENABLE)
		BL_Print_Message("CRC Verification Passed \r\n");
#endif
		Bootloader_Send_ACK(1);
	/*Reading Protecton out level  */
		RDP_Level = CBL_STM32F103_Get_RDP_Level();
				/*Reading Valid Protecton out level  */
		Bootloader_Send_Data_To_Host((uint8_t*)&RDP_Level,1);
	}
	else{
#if (BL_DEBUG_ENABLE == DEBUG_INFO_ENABLE)
		BL_Print_Message("CRC Verification Failed \r\n");
#endif
	Bootloader_Send_NACK();	
	}
}


static void Bootloader_Jump_To_Address(uint8_t *Host_Buffer)
{
	uint8_t Host_CMD_Packet_Len = 0;
	uint32_t Host_CRC32 = 0;
	uint32_t  HOST_Jump_Address = 0;
	uint8_t Address_Verification = ADDRESS_IS_INVALID;

#if(BL_DEBUG_ENABLE == DEBUG_INFO_ENABLE)
	BL_Print_Message("Read the commands supported by bootloader \r\n");
#endif

	/* extract CRC and packet length sent by the host*/
	Host_CMD_Packet_Len = Host_Buffer[0] + 1;
	Host_CRC32 = *((uint32_t*)((Host_Buffer + Host_CMD_Packet_Len)-CRC_TYPE_SIZE_BYTE)); 
	/* CRC Verification */
	if(CRC_VERIFICATION_PASSED == Bootloader_CRC_Verify((uint8_t*)&Host_Buffer[0],Host_CMD_Packet_Len - 4,Host_CRC32)){
#if(BL_DEBUG_ENABLE == DEBUG_INFO_ENABLE)
		BL_Print_Message("CRC Verivication Passed \r\n");
#endif
		Bootloader_Send_ACK(1);		
		/* Extract The address from Host Packet */
		HOST_Jump_Address = *((uint32_t*)&Host_Buffer[2]);
		Address_Verification = HOST_Jump_Address_Verification(HOST_Jump_Address);
		if (ADDRESS_IS_VALID == Address_Verification)
				{
		#if(BL_DEBUG_ENABLE == DEBUG_INFO_ENABLE)
				BL_Print_Message("Address verification succeeded \r\n");
		#endif
					/* report address verification succeeded */ 
					Bootloader_Send_Data_To_Host((uint8_t*)&Address_Verification,1);
					/* Prepare the address to jump */ 
					Jump_Ptr Jump_Address = (Jump_Ptr)(HOST_Jump_Address + 1);
		#if(BL_DEBUG_ENABLE == DEBUG_INFO_ENABLE)
					BL_Print_Message("Jump to : %X \r\n", Jump_Address);
		#endif
					Jump_Address();
				}
				else{
					Bootloader_Send_Data_To_Host((uint8_t*)&Address_Verification,1);
				}
	}
	else 
	{
#if(BL_DEBUG_ENABLE == DEBUG_INFO_ENABLE)
		BL_Print_Message("CRC Verivication Faild \r\n");
#endif
		Bootloader_Send_NACK();
	}
}


static void Bootloader_Erase_Flash(uint8_t *Host_Buffer)
{
	uint16_t Host_CMD_Packet_Len = 0;
  uint32_t Host_CRC32 = 0;
	uint8_t Erase_Status = 0;
#if(BL_DEBUG_ENABLE == DEBUG_INFO_ENABLE)
	BL_Print_Message("Mass erase or sector erase of the user flash \r\n");
#endif
	
	/* extract CRC and packet length sent by the host*/
	Host_CMD_Packet_Len = Host_Buffer[0] + 1;
	Host_CRC32 = *((uint32_t*)((Host_Buffer + Host_CMD_Packet_Len)-CRC_TYPE_SIZE_BYTE)); 
	/* CRC Verification */
if(CRC_VERIFICATION_PASSED == Bootloader_CRC_Verify((uint8_t*)&Host_Buffer[0],Host_CMD_Packet_Len - 4,Host_CRC32)){
#if(BL_DEBUG_ENABLE == DEBUG_INFO_ENABLE)
		BL_Print_Message("CRC Verivication Passed \r\n");
#endif
	Bootloader_Send_ACK(1);
	Erase_Status =	Perform_Flash_Erase(Host_Buffer[2],Host_Buffer[3]);
	if (SUCCESSFUL_ERASE == Erase_Status)
	{
	  Bootloader_Send_Data_To_Host((uint8_t*)&Erase_Status,1);
	/* report erase status succeed */
#if (BL_DEBUG_ENABLE == DEBUG_INFO_ENABLE)
		BL_Print_Message("Successful Erase \r\n");
#endif
	}
	else {
			/* report erase status failed */
		Bootloader_Send_Data_To_Host((uint8_t*)&Erase_Status,1);
#if (BL_DEBUG_ENABLE == DEBUG_INFO_ENABLE)
			BL_Print_Message("Erase request failed !!\r\n");
#endif
	
			}
	}
else{
#if (BL_DEBUG_ENABLE == DEBUG_INFO_ENABLE)
		BL_Print_Message("CRC Verification Failed \r\n");
#endif
		Bootloader_Send_NACK();
	}
}

void Bootloader_Memory_Write(uint8_t* Host_Buffer)
{
	uint16_t Host_CMD_Packet_Len = 0;
  uint32_t Host_CRC32 = 0;
	uint32_t HOST_Address = 0;
	uint8_t Payload_Len = 0;
	uint8_t Address_Verification = ADDRESS_IS_INVALID;
	uint8_t Flash_Payload_Write_Status = FLASH_PAYLOAD_WRITE_FAILED;
	
#if(BL_DEBUG_ENABLE == DEBUG_INFO_ENABLE)
	BL_Print_Message("Write data to different memories of the MCU \r\n");
#endif
							/* extract CRC and packet length sent by the host*/
	Host_CMD_Packet_Len = Host_Buffer[0] + 1;
	Host_CRC32 = *((uint32_t*)((Host_Buffer + Host_CMD_Packet_Len)-CRC_TYPE_SIZE_BYTE)); 
											/* CRC Verification */
if(CRC_VERIFICATION_PASSED == Bootloader_CRC_Verify((uint8_t*)&Host_Buffer[0],Host_CMD_Packet_Len - 4,Host_CRC32)){
#if(BL_DEBUG_ENABLE == DEBUG_INFO_ENABLE)
		BL_Print_Message("CRC Verivication Passed \r\n");
#endif
		/* send ack to host */ 
		Bootloader_Send_ACK(1);
		/* Extract the start address from the Host packet */
		HOST_Address = *((uint32_t*)(&Host_Buffer[2]));
#if (BL_DEBUG_ENABLE == DEBUG_INFO_ENABLE)
		BL_Print_Message("HOST_Address = 0x%X \r\n", HOST_Address);
#endif
		/* Extract the Payload length from the Host packet */
		Payload_Len = *((uint32_t*)(&Host_Buffer[6]));
		Address_Verification = HOST_Jump_Address_Verification(HOST_Address);
		

		if (ADDRESS_IS_VALID == Address_Verification)
		{
			Flash_Payload_Write_Status = Flash_Memory_Write_Payload((uint16_t*)&Host_Buffer[7],HOST_Address,Payload_Len);
			
			if(FLASH_PAYLOAD_WRITE_PASSED == Flash_Payload_Write_Status)
			{
				/* Report Payload write passedd */
				Bootloader_Send_Data_To_Host((uint8_t*)&Flash_Payload_Write_Status,1);
#if (BL_DEBUG_ENABLE == DEBUG_INFO_ENABLE)
				BL_Print_Message("Payload Valid \r\n");
#endif
			}
			else {
				/* Report Payload write faild */
				Bootloader_Send_Data_To_Host((uint8_t*)&Flash_Payload_Write_Status,1);
#if (BL_DEBUG_ENABLE == DEBUG_INFO_ENABLE)
				BL_Print_Message("Payload InValid \r\n");
#endif
			}
		}
		else 
		{
			Bootloader_Send_Data_To_Host((uint8_t*)&Address_Verification,1);
			
		}
	}
else{
#if (BL_DEBUG_ENABLE == DEBUG_INFO_ENABLE)
		BL_Print_Message("CRC Verification Failed \r\n");
#endif
		Bootloader_Send_NACK();
	}
}

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
						Bootloader_Get_Version(BL_Host_Buffer);
						Status = BL_OK;
						break;
				
					case CBL_GET_HELP_CMD:
						Bootloader_Get_Help(BL_Host_Buffer);
						Status = BL_OK;
						break;
					
					case CBL_GET_CID_CMD:
						Bootloader_Get_Chip_Identification_Number(BL_Host_Buffer);
						BL_Print_Message("CBL_GET_CID_CMD \r\n");

						break;
					
					case CBL_GET_RDP_STATUS_CMD:
						Bootloader_Read_Protection_Level(BL_Host_Buffer);
						Status = BL_OK;
						break;
					
					case CBL_GO_TO_ADDR_CMD:
						Bootloader_Jump_To_Address(BL_Host_Buffer);
						Status = BL_OK;
						break;
					
					case CBL_FLASH_ERASE_CMD:
					  Bootloader_Erase_Flash(BL_Host_Buffer);
						Status = BL_OK;
						break;	
					
					case CBL_MEM_WRITE_CMD:
						Bootloader_Memory_Write(BL_Host_Buffer);
						Status = BL_OK;					
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




