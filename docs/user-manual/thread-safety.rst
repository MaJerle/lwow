.. _um_thread_safety:

Thread safety
=============

With default configuration, library is *not* thread safe.
This means whenever it is used with operating system, user must resolve it with care.

Library has locking mechanism support for thread safety, which needs to be enabled.

.. tip::
    To enable thread-safety support, parameter ``OW_CFG_OS`` must be set to ``1``.
    Please check :ref:`api_ow_opt` for more information about other options.

After thread-safety features has been enabled, it is necessary to implement
``4`` low-level system functions.

.. tip::
    System function template example is available in ``onewire_uart/src/system/`` folder.

Example code for ``CMSIS-OS V2``

.. note::
    Check :ref:`api_ow_sys` section for function description

.. literalinclude:: ../../onewire_uart/src/system/ow_sys_cmsis_os.c
    :language: c
    :linenos:
    :caption: System functions for CMSIS-OS based operating system

.. toctree::
    :maxdepth: 2