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
driver must link these functions to single driver structure of type :cpp:type:`ow_ll_drv_t`,
later used during instance initialization.

.. tip::
	Check :ref:`api_ow_ll` for function prototypes.

Implement system functions
^^^^^^^^^^^^^^^^^^^^^^^^^^

System functions are required only if operating system mode is enabled, with :c:macro:`OW_CFG_OS`.

Its implementation structure is not the same as for low-level driver,
customer needs to implement fixed functions, with pre-defined name, starting with ``ow_sys_`` name.

System function must only support OS mutex management and has to provide:

* :cpp:func:`ow_sys_mutex_create` function to create new mutex
* :cpp:func:`ow_sys_mutex_delete` function to delete existing mutex
* :cpp:func:`ow_sys_mutex_wait` function to wait for mutex to be available
* :cpp:func:`ow_sys_mutex_release` function to release (give) mutex back

.. warning::
	Application must define :c:macro:`OW_CFG_OS_MUTEX_HANDLE` for mutex type.
	This shall be done in ``ow_config.h`` file.

.. tip::
	Check :ref:`api_ow_sys` for function prototypes.

Example: Low-level driver for WIN32
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Example code for low-level porting on `WIN32` platform.
It uses native *Windows* features to open *COM* port and read/write from/to it.

.. literalinclude:: ../../onewire_uart/src/system/ow_ll_win32.c
    :language: c
    :linenos:
    :caption: Actual implementation of low-level driver for WIN32

Example: Low-level driver for STM32
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Example code for low-level porting on `STM32` platform.

.. literalinclude:: ../../onewire_uart/src/system/ow_ll_stm32.c
    :language: c
    :linenos:
    :caption: Actual implementation of low-level driver for STM32

Example: System functions for WIN32
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. literalinclude:: ../../onewire_uart/src/system/ow_sys_win32.c
    :language: c
    :linenos:
    :caption: Actual implementation of system functions for WIN32

Example: System functions for CMSIS-OS
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. literalinclude:: ../../onewire_uart/src/system/ow_sys_cmsis_os.c
    :language: c
    :linenos:
    :caption: Actual implementation of system functions for CMSIS-OS


.. toctree::
    :maxdepth: 2