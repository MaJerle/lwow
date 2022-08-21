.. _um_porting_guide:

Porting guide
=============

Implement low-level driver
^^^^^^^^^^^^^^^^^^^^^^^^^^

Implementation of low-level driver is an essential part.
It links middleware with actual hardware design of the device.

Its implementation must provide ``4`` functions:

* To open/configure UART hardware
* To set UART baudrate on the fly
* To transmit/receive data over UART
* To close/de-init UART hardware

After these functions have been implemented (check below for references),
driver must link these functions to single driver structure of type :cpp:type:`lwow_ll_drv_t`,
later used during instance initialization.

.. tip::
	Check :ref:`api_lwow_ll` for function prototypes.

Implement system functions
^^^^^^^^^^^^^^^^^^^^^^^^^^

System functions are required only if operating system mode is enabled, with :c:macro:`LWOW_CFG_OS`.

Its implementation structure is not the same as for low-level driver,
customer needs to implement fixed functions, with pre-defined name, starting with ``lwow_sys_`` name.

System function must only support OS mutex management and has to provide:

* :cpp:func:`lwow_sys_mutex_create` function to create new mutex
* :cpp:func:`lwow_sys_mutex_delete` function to delete existing mutex
* :cpp:func:`lwow_sys_mutex_wait` function to wait for mutex to be available
* :cpp:func:`lwow_sys_mutex_release` function to release (give) mutex back

.. warning::
	Application must define :c:macro:`LWOW_CFG_OS_MUTEX_HANDLE` for mutex type.
	This shall be done in ``lwow_opts.h`` file.

.. tip::
	Check :ref:`api_lwow_sys` for function prototypes.

Example: Low-level driver for WIN32
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Example code for low-level porting on `WIN32` platform.
It uses native *Windows* features to open *COM* port and read/write from/to it.

.. literalinclude:: ../../lwow/src/system/lwow_ll_win32.c
    :language: c
    :linenos:
    :caption: Actual implementation of low-level driver for WIN32

Example: Low-level driver for STM32
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Example code for low-level porting on `STM32` platform.

.. literalinclude:: ../../lwow/src/system/lwow_ll_stm32.c
    :language: c
    :linenos:
    :caption: Actual implementation of low-level driver for STM32

Example: System functions for WIN32
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. literalinclude:: ../../lwow/src/system/lwow_sys_win32.c
    :language: c
    :linenos:
    :caption: Actual implementation of system functions for WIN32

Example: System functions for CMSIS-OS
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. literalinclude:: ../../lwow/src/system/lwow_sys_cmsis_os.c
    :language: c
    :linenos:
    :caption: Actual implementation of system functions for CMSIS-OS

Low-Level driver for STM32 with STM32CubeMX
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Specific low-level driver has been implemented for STM32 series of microcontrollers,
to allow easy and simple link of LwOW library with projects generated with STM32CubeMX or STM32CubeIDE development tools.

Driver is based on HAL (Hardware Abstraction Layer) and it uses interrupt configuration to transmit/receive data.
When customer starts a new project using CubeMX, it must:

- Configure specific UART IP as async mode both directions
- UART must have enabled global interrupts, to allow transmitting/receiving data using interrupts
- Application must pass pointer to UART handle when calling ``lwow_init`` function

.. tip::
	Special example has been developed to demonstrate how can application use
	multiple OneWire instances on multiple UART ports at the same time.
	It uses custom argument to determine which UART handle shall be used for data transmit.
	Check ``/examples/stm32/`` folder for actual implementation.

.. literalinclude:: ../../lwow/src/system/lwow_ll_stm32_hal.c
    :language: c
    :linenos:
    :caption: Actual implementation of low-level driver for STM32 with HAL drivers

Low-Level driver for manual GPIO control
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

it is possible to use LwOW library even without available UARTs in the device (or if UARTs are being used for something else).
Demo driver, that manipulates GPIO toggling is available in the repository.

.. literalinclude:: ../../lwow/src/system/lwow_ll_stm32_single_gpio_driver.c
    :language: c
    :linenos:
    :caption: LwOW low-level driver for manual GPIO control without UART

.. toctree::
    :maxdepth: 2