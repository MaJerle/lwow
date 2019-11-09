/**
 * \page            page_appnote Application note
 * \tableofcontents
 *
 * \section         sect_getting_started Getting started
 *
 * Repository <a href="https://github.com/MaJerle/onewire_uart"><b>onewire_uart is hosted on Github</b></a>. It combines source code and example projects.
 *
 * \subsection      sect_clone Clone repository
 *
 * \par             First-time clone
 *
 *  - Download and install `git` if not already
 *  - Open console and navigate to path in the system to clone repository to. Use command `cd your_path`
 *  - Run `git clone --recurse-submodules https://github.com/MaJerle/onewire_uart` command to clone repository including submodules
 *  - Navigate to `examples` directory and run favourite example
 *
 * \par             Already cloned, update to latest version
 *
 *  - Open console and navigate to path in the system where your resources repository is. Use command `cd your_path`
 *  - Run `git pull origin master --recurse-submodules` command to pull latest changes and to fetch latest changes from submodules
 *  - Run `git submodule foreach git pull origin master` to update & merge all submodules
 *
 * \section         sect_project_examples Example projects
 *
 * Several examples are available to show application use cases. These are split and can be tested on different systems.
 *
 * \subsection      sect_project_examples_arm_embedded ARM Cortex-M examples
 *
 * Library is independant from CPU architecture, meaning we can also run it on embedded systems. 
 * Different ports for `FreeRTOS` operating system and `STM32` based microcontrollers are available too.
 *
 *  <table>
 *      <caption>STM32 boards and pinouts for tests</caption>
 *      <tr><th> 						<th colspan="3">OneWire settings<th colspan="5">Debug settings
 *      <tr><th>Board name				<th>UART 	<th>MTX <th>MRX 	<th>UART    <th>MDTX<th>MDRX<th>DBD     <th>Comment
 *      <tr><td>\b STM32L496G-Discovery <td>USART1 	<td>PB6 <td>PG10	<td>USART2  <td>PA2 <td>PD6 <td>921600  <td>OBSTL
 *      <tr><td>\b STM32F429ZI-Nucleo   <td>USART1 	<td>PA9 <td>PA10	<td>USART3  <td>PD8 <td>PD9 <td>921600  <td>OBSTL
 *  </table>
 *
 * 	- \b MTX: MCU TX pin, connected to OneWire network data pin (together with MCU RX pin)
 * 	- \b MRX: MCU RX pin, connected to OneWire network data pin (together with MCU TX pin)
 * 	- \b MDTX: MCU Debug TX pin, other device RX pin
 * 	- \b MDRX: MCU Debug RX pin, other device TX pin
 * 	- \b DBD: Debug UART baudrate
 * 	- \b OBSTL: On-Board ST-Link USB virtual COM port
 *
 * \note            All examples for STM32 come with ST's official free development studio STM32CubeIDE, available at st.com
 *
 * \section         sect_background Brief protocol introduction
 *
 * 1-Wire protocol is very well and clearly defined in terms of timings, how to read/write single bit and byte on interface.
 * 
 * \note			Detailed protocol specifications are available on
 *					<a href="https://www.maximintegrated.com/en/app-notes/index.mvp/id/126 " target="_new">Maxim website</a>.
 *
 * Based on timings, we can with few lines of code emulate protocol with single GPIO pin and software delay.
 * While this may work good, we have some major issues on modern microcontrollers:
 *
 *  - <b>Interrupts:</b> If you have fixed delay and interrupt jumps in, there is additional time in our wait routine and timings is incorrect
 *	- <b>Operating system:</b> Most of advanced MCUs (high-speed, ARM-Cortex-M based) use RTOS systems in the application. 
 *		While delay may work with well on milliseconds based resolution, task switching and other interrupts will introduce additional
 *		delay in timings.
 * 
 * \note			Both problems may be eliminated with interrupt driven software, which requires many software skills
 *
 * Instead of taking care of timings from software point of view, we can use hardware (if available) to handle timings on silicon level.
 * One option is UART protocol which is available on (99.9%) every microcontroller on a market.
 * Many of them have only `1` interface which is normally used for other communication (or debugging),
 * thus we have to search for one with more than `1` UART interface.
 *
 * 1-Wire uses `2` different important timings and these are:
 *
 *  - <b>Reset pulse:</b> Used to reset all devices to initial states. By 1-Wire timing specs, it maches `9600` bauds on UART level
 *  - <b>Read/Write single bit:</b> Used to read or write single bit to/from slave device. By 1-Wire timing specs, it matches `115200` bauds on UART level.
 *
 * \note			1-Wire specs match UART timing at `9600` or `115200` bauds only when following UART configuration is used: <b>1 STOP bit, LSB bit first, no parity</b>
 *
 * \section         sect_config Library configuration
 *
 * To make library as efficient as possible, different configuration parameters are available
 * to make sure all the requirements are met for different purposes as possible.
 *
 * A list of all configurations can be found in \ref OW_CONFIG section.
 *
 * \subsection      sect_conf_file Project configuration file
 *
 * Library comes with `2` configuration files:
 *
 *  - Default configuration file `ow_config_default.h`
 *  - Project template configuration file `ow_config_template.h`
 *
 * When project is started, user has to rename template file to `ow_config.h`
 * and if required, it should override default settings in this file.
 *
 * Default template file comes with something like this:
 *
 * \include         _ow_config_template.h
 *
 * If bigger buffer is required, modification must be made like following:
 *
 * \include         _ow_config.h
 *
 * \note            Always modify default settings by overriding them in user's custom `ow_config.h` file
 *                      which was previously renamed from `ow_config_template.h`
 *
 * \section         sect_hw_connection Hardware connection
 * 
 * Devices must be connected to host (master) device via single-wire (that's why protocol is 1-Wire) + VCC + GND of course.
 * UART protocol uses `2` async lines for communication. We need to transfer `2` wire transfer to single wire.
 *
 * To do it successfully, we need our `TX` pin in `open-drain` mode.
 * Most of MCUs or USB-to-UART transceivers don't have option for open-drain mode TX pin (STM32 family has this option).
 *
 * \image html 1w_pp2od.svg 1-Wire connection with push-pull to open-drain converter
 *
 * \image html 1w_od.svg 1-Wire connection with TX/RX pins in open-drain mode
 *
 * Image above explains how to convert TX pin in `push-pull` mode into `open-drain` mode and how to connect it to 1-Wire devices.
 * If your host master device supports `open-drain` mode, you may skipp circuit with `2 transistors and 2 resistors` and:
 *
 *  - Connect `TX` and `RX` pins together
 *  - Use `TX` (or `RX`, doesn't matter as they are connected together) as data wire for slave devices
 *
 * \par 			Circuit explanation
 *
 * <b>`TX pin high`:</b> Left NMOS is open, its drain pin is connected to GND.
 *		Gate of right NMOS is also connected to GND in this case, right NMOS is closed (current flow is not possible).
 *		Drain of right NMOS is high because of right pull-up resistor.
 *		All slaves see logical `1` on bus which is the same as we outputted on `TX` pin with push-pull mode.
 *		If any of slaves tries to set bus low, there is no issue with short circuit because of right resistor.
 *		
 * <b>`TX pin low`:</b> Left NMOS is closed, its drain is connected to VCC with left pull-up resistor.
 *		Gate of right NMOS is connected to VCC with left pull-up resistor this right NMOS is open.
 *		Now drain for right NMOS is tied to GND and bus value is forced low.
 *
 * We achieved desired functionality. When setting `TX` pin high, slaves can set bus to low if required,
 * but when `TX` pin is low, bus is forced to low
 *
 * \note  			Open-Drain output mode prevents short circuit on bus when master wants to output logical `1` and slave wants to output logical `0` at the same time. 
 *					The same applies between `2` different slaves.
 *					Open-Drain mode allows you only to put bus low or be disconnected from bus. Bus is set high using external resistor.
 *					If TX pin would be in push-pull configuration and output is high while slave wants to put it low, there is short circuit on bus and communication would collapse
 *
 * \par 			TX versus RX pin
 *
 * As explained earlier, we need to send UART data on `TX` pin and read it at the same time from `RX` pin.
 * This may be called loop-back functionality with one important feature.
 *
 * When we output logical `1` on `TX` pin, we expect bus to be high, but slave can set it to low.
 * Since RX pin is connected on bus directly, we will see this change on `RX` pin coming back to MCU.
 *
 * Imagine reset sequence in 1-Wire protocol. UART must be set at `9600` bauds and byte value `0xF0` must be transmitted on TX line.
 * We expect bus to be low half of byte and high half of byte. By 1-Wire specs, slaves must respond by setting bus low when TX pin is high.
 *
 * \note            Sending `0xF0` and we receive back `0xF0` means there are no slaves connected on a bus,
 *                  because no slave set bus to low and `TX` value is just reflected to `RX`.
 *
 * \section         sect_byte_bit Byte, bit and timing relation
 *
 * Single byte is represented with 8-bits stream. By 1-Wire specs, sending 1-bit of data, specific sequence must be send over UART at `115200` bauds.
 * 
 *  - <b>Write logical high:</b> To send logical low bit, bus requires UART constant value `0xFF`.
 *                  Start bit in UART sequence will take care of short low pulse, which indicates `start of frame` for 1-Wire bit transfer.
 *                  The rest of the time bus must be high (`0xFF part`).
 *
 * 	- <b>Write logical low:</b> To send logical low bit, bus requires UART constant value `0x00` which forces bus to be low on full period, except STOP bit of UART.
 *
 *  - <b>Read bit</b>: Since master must initialize every transaction, reading bit value is the same as writing logical 1.
 *                  It starts with short low period (start bit of UART frame) and follows with bus high. 
 *                  Slave is responsible to force bus low in case of logical low or keep it high in case of logical high.
 *
 * <a href="https://www.maximintegrated.com/en/app-notes/index.mvp/id/214" target="_new">
 *						Detailed correlation between 1-Wire timing and UART can be found here.</a>
 *
 * It is worth to know, important timing starts when master initializes bus to read/write bit. 
 * When read/write sequence of single byte finishes, master does not need to start new sequence
 * for new bit immediately thus there is advantage for user, if host MCU or other device does not have DMA, because:
 *
 *  - User sets UART output data and waits for transmission completed for `TX` and also for `RX`
 *      (user sends data on `TX`, reads back on `RX` side)
 *  - If interrupt happens (or task switch in RTOS mode), UART HW will take care of proper timing and set 
 *      status flags indicating transmission of byte completed, etc.
 *  - When original task start execution again (or interrupt finishes), user can read previous byte
 *      and send new one for next bit
 *  - Conclusion: No matter how complicated our system is (how many interrupts, tasks, etc), 
 *      timings for every bit will be correct, but timing between `2` bits will vary and this is not an issue by 1-Wire specification
 *      - Very important advantage comparing to software driven timings
 *
 * \image html 1w_bit_byte.svg To send `3` bits on 1-Wire level, user must send `3` bytes on UART level. Blue and Green parts show timing part which is not critical
 */