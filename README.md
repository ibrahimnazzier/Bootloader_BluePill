# Bootloader Project for ARM STM32F103C8 MCU

Welcome to the bootloader project for the ARM STM32F103C8 Microcontroller Unit (MCU). This project aims to provide a robust and efficient bootloader that can handle various tasks essential for the operation and maintenance of the MCU.

## Features

The bootloader supports the following commands:

1. **Get Version**: This command allows you to retrieve the current version of the bootloader.
2. **Help Command**: This command provides a list of all available commands along with a brief description of what each command does.
3. **Get Chip Identification Number**: This command retrieves the unique identification number of the chip. This can be useful for inventory management and device tracking.
4. **Get Read Protection Level**: This command returns the current read protection level of the flash memory. It helps in understanding the security level of the MCU.
5. **Jump to Specific Address**: This command allows the bootloader to jump to a specific address in the memory. This is useful for tasks like executing user applications.
6. **Perform Flash Erase**: This command erases the flash memory. It's an essential step before writing new data or programs to the memory.
7. **Bootloader Write Memory**: This command writes data to a specific location in the memory. It's used for tasks like updating the firmware or user applications.

This project is designed to be a starting point for anyone looking to understand the workings of a bootloader for the ARM STM32F103C8 MCU. It provides a solid foundation that can be built upon to create more complex and feature-rich bootloaders. Enjoy exploring and happy coding! 🚀.

# Components Used

The following components are used in this project:

1. **STM32f103C8 ARM MCU**: The main microcontroller unit where the bootloader resides and operates.
2. **ST-link V2**: Used for debugging and programming the MCU.
3. **2 * FTDI Ft232rl**: One is used for receiving commands from the host, and the other is used for serial debugging.
4. **Host Computer**: Runs a Python script to interact with the bootloader.

# Operation

The operation of the bootloader involves the following steps:

1. **UART Configuration**: Two UART ports are configured to connect with the FTDI modules. One port is used for debugging, and the other is used for sending commands to the STM32 MCU.
2. **Power Connection**: The 5V source of the FTDI modules is connected to the 5V source of the STM32 MCU.
3. **Ground Connection**: The ground of the FTDI modules is connected to the ground of the STM32 MCU.
4. **Host Connection**: The FTDI modules are connected to the host computer using a USB port. The host computer runs a Python script to send commands to the STM32 MCU through the FTDI module.
