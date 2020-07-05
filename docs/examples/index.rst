.. _examples:

Examples and demos
==================

Various examples are provided for fast library evaluation on embedded systems. These are optimized prepared and maintained for ``2`` platforms, but could be easily extended to more platforms:

* WIN32 examples, prepared as `Visual Studio Community <https://visualstudio.microsoft.com/vs/community/>`_ projects
* ARM Cortex-M examples for STM32, prepared as `STM32CubeIDE <https://www.st.com/en/development-tools/stm32cubeide.html>`_ GCC projects

.. warning::
	Library is platform independent and can be used on any platform.

Example architectures
^^^^^^^^^^^^^^^^^^^^^

There are many platforms available today on a market, however supporting them all would be tough task for single person.
Therefore it has been decided to support (for purpose of examples) ``2`` platforms only, `WIN32` and `STM32`.

WIN32
*****

Examples for *WIN32* are prepared as `Visual Studio Community <https://visualstudio.microsoft.com/vs/community/>`_ projects.
You can directly open project in the IDE, compile & debug.

To run examples on this architecture, external *USB to UART* converted would be necessary.
Application opens *COM port* and sends/receives data directly to there.

.. tip::
	Push-pull to open-drain external converter might be necessary.
	Check :ref:`hw_connection` for more information.

STM32
*****

Embedded market is supported by many vendors and STMicroelectronics is, with their `STM32 <https://www.st.com/en/microcontrollers-microprocessors/stm32-32-bit-arm-cortex-mcus.html>`_ series of microcontrollers, one of the most important players.
There are numerous amount of examples and topics related to this architecture.

Examples for *STM32* are natively supported with `STM32CubeIDE <https://www.st.com/en/development-tools/stm32cubeide.html>`_, an official development IDE from STMicroelectronics.

You can run examples on one of official development boards, available in repository examples.

.. table:: Supported development boards

	+----------------------+----------------------+----------------------+
	| Board name           | Onewire settings     | Debug settings       |
	|                      +--------+------+------+--------+------+------+
	|                      | UART   | MTX  | MRX  | UART   | MDTX | MDRX |
	+======================+========+======+======+========+======+======+
	| STM32L496G-Discovery | USART1 | PB6  | PG10 | USART2 | PA2  | PD6  |
	+----------------------+--------+------+------+--------+------+------+
	| STM32F429ZI-Nucleo   | USART1 | PA9  | PA10 | USART3 | PD8  | PD9  |
	+----------------------+--------+------+------+--------+------+------+

Pins to connect to 1-Wire sensor:

* *MTX*: MCU TX pin, connected to 1-Wire network data pin (together with MCU RX pin)
* *MRX*: MCU RX pin, connected to 1-Wire network data pin (together with MCU TX pin)

  * *TX* pin is configured as open-drain and can be safely connected directly with *RX* pin

Other pins are for your information and are used for debugging purposes on board.

* MDTX: MCU Debug TX pin, connected via on-board ST-Link to PC
* MDRX: MCU Debug RX pin, connected via on-board ST-Link to PC
* Baudrate is always set to ``921600`` bauds

Examples list
^^^^^^^^^^^^^

Here is a list of all examples coming with this library.

.. tip::
	Examples are located in ``/examples/`` folder in downloaded package.
	Check :ref:`download_library` section to get your package.

LwOW bare-metal
***************

Simple example, not using operating system, showing basic configuration of the library.
It can be also called `bare-metal` implementation for simple applications.

LwOW OS
*******

LwOW library as an example when multiple threads want to access to single LwOW core.

.. toctree::
	:maxdepth: 2
