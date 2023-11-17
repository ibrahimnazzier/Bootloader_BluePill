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
#define BL_ENABLE_CAN_DEBUG_MESSAGE			0x02
#define BL_DEBUG_METHOD (BL_ENABLE_UART_DEBUG_MESSAGE)

/* Use UART 3 for debugging with HOST */  
#define BL_DEBUG_UART 						&huart3

/* use UART 2 for communication with HOST */
#define BL_HOST_COMMUNICATION_UART		    &huart2

/* which communication Protocol used for debugging */
#define BL_ENABLE_UART_DEBUG_MESSAGE	0x00
#define BL_ENABLE_SPI_DEBUG_MESSAGE 	0x01
#define BL_ENABLE_CAN_DEBUG_MESSAGE		0x02
#define BL_DEBUG_METHOD (BL_ENABLE_UART_DEBUG_MESSAGE)

/* CRC Object */
#define CRC_ENGINE_OBJ								&hcrc

/* Enable/Disable Debug info for HOST */
#define DEBUG_INFO_DISABLE						    0
#define DEBUG_INFO_ENABLE									1
#define BL_DEBUG_ENABLE								DEBUG_INFO_ENABLE

/* Bootloader Version Info */
#define CBL_VENDOR_ID							100
#define	CBL_SW_MAJOR_VERSION				1
#define	CBL_SW_MINOR_VERSION				2
#define	CBL_SW_PATCH_VERSION				0

/* CRC Type Size */
#define CRC_TYPE_SIZE_BYTE					4


#define CRC_VERIFICATION_FAILED			0x00
#define CRC_VERIFICATION_PASSED			0x01


#define BL_HOST_BUFFER_RX_LENGTH 			200


#define CBL_GET_VER_CMD              0x10
#define CBL_GET_HELP_CMD             0x11
#define CBL_GET_CID_CMD              0x12
/* Get Read Protection Status */
#define CBL_GET_RDP_STATUS_CMD       0x13
#define CBL_GO_TO_ADDR_CMD           0x14
#define CBL_FLASH_ERASE_CMD          0x15
#define CBL_MEM_WRITE_CMD            0x16
/* Enable/Disable Write Protection */
#define CBL_ED_W_PROTECT_CMD         0x17
#define CBL_MEM_READ_CMD             0x18
/* Get Sector Read/Write Protection Status */
#define CBL_READ_SECTOR_STATUS_CMD   0x19
#define CBL_OTP_READ_CMD             0x20
/* Change Read Out Protection Level */
#define CBL_CHANGE_ROP_Level_CMD     0x21

/* ACK & NACK values defined by HOST */ 
#define CBL_SEND_NACK 							0xAB
#define CBL_SEND_ACK								0XCD

	/*CBL_GET_RDP_STATUS_CMD*/
#define POR_LEVEL_READ_INVALID	0x00
#define POR_LEVEL_READ_VALID		0x01

	/* CBL_GO_TO_ADDR_CMD */
#define ADDRESS_IS_VALID 							0X01
#define ADDRESS_IS_INVALID 						0X00
 
 /* sram range of adresses */ 

#define STM32f103_SRAM_SIZE							(20*1024)
#define STM32f103_FLASH_SIZE						(64*1024)

#define STM32f103_SRAM_END							(SRAM_BASE + STM32f103_SRAM_SIZE)
#define STM32f103_FLASH_END							(FLASH_BASE + STM32f103_FLASH_SIZE)

		/* erase command defines */

#define CBL_FLASH_MAX_SECTOR_NUMBER 		0x40

#define INVALID_SECTOR_NUMBER 					0X01
#define VALID_SECTOR_NUMBER 						0X02
#define UNSUCCESSFUL_ERASE 							0x02
#define SUCCESSFUL_ERASE 								0x03

#define BL_FLASH_MASS_ERASE							0XFF

#define HAL_SUCCESSFUL_ERASE 						0xFFFFFFFF

/*  CBL_MEM_WRITE_CMD */
#define FLASH_PAYLOAD_WRITE_FAILED 		0x00
#define FLASH_PAYLOAD_WRITE_PASSED 		0x01 
/******************** Macro Decleration Ends ********************************************************/



/******************** Macro Function Decleration Starts *********************************************/

/******************** Macro Function Decleration Ends ***********************************************/




/******************** Data Type Decleration Starts **************************************************/
typedef enum{
BL_NACk = 0,
BL_OK
}BL_Status;

/* typedef for void pointer for function that take void */
typedef void (*pMainApp)(void);

typedef void (*Jump_Ptr)(void);
/******************** Data Type Decleration Ends ****************************************************/



/******************** Software Interfaces Decleration Starts ****************************************/
/**
 \brief Print Message over UART
 \details set send message through UART used for Debugging 
 \param [in] format specifiy the behavior of the variadic function
 \param [...] Variadic function.
 **/

void BL_Print_Message(char *format, ...);

/**
 \brief Recieve messages from user 
 \details Receive Commands from user using UART 
 \return Return Enum as operation done (BL_ok) or not (BL_NAK)  
 **/
BL_Status BL_UART_Host_Fetch_Command(void);


/******************** Software Interfaces Decleration Ends ****************************************/



#endif /* BOOTLOADER_H */
 