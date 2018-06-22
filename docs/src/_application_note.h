/**
 * \page            page_appnote Application note
 * \tableofcontents
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
 * \section         sect_hw_connection Hardware connection
 * 
 * Devices must be connected to host (master) device via single-wire (that's why protocol is 1-Wire) + VCC + GND of course.
 * UART protocol uses `2` async lines for communication. We need to transfer `2` wire transfer to single wire.
 *
 * To to it successfully, we need our `TX` pin in `open-drain` mode.
 * Most of MCUs or USB-to-UART transceivers don't have option for open-drain mode TX pin (STM32 family has this option).
 *
 * \image html 1w_pp2od.svg 1-Wire connection with push-pull to open-drain converter
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
 * \note 			If we send `0xF0` and we receive back `0xF0`, there were no slaves connected on bus,
 *					because nobody set bus to low and `TX` value is just reflected to `RX`.
 */