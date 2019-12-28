.. _api_ow_sys:

System functions
================

System function are used in conjunction with thread safety.
These are required when operating system is used and multiple threads
want to access to the same OneWire instance.

.. tip::
	Check :ref:`um_thread_safety` and :ref:`um_porting_guide` for instructions on how to port.

Below is a list of function prototypes and its implementation details.

.. doxygengroup:: OW_SYS
