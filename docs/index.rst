LwOW |version| documentation
============================

Welcome to the documentation for version |version|.

LwOW is lightweight, platform independent library for Onewire protocol for embedded systems.
Its primary focus is UART hardware for physical communication for sensors and other slaves.

.. image:: static/images/logo.svg
    :align: center

.. rst-class:: center
.. rst-class:: index_links

    :ref:`download_library` :ref:`getting_started` `Open Github <https://github.com/MaJerle/lwow>`_ `Donate <https://paypal.me/tilz0R>`_

Features
^^^^^^^^

* Written in ANSI C99
* Platform independent, uses custom low-level layer for device drivers
* 1-Wire protocol fits UART specifications at ``9600`` and ``115200`` bauds
* Allows standard one-wire single-gpio manual control (when UARTs are no more available by the system)
* Hardware is responsible for timing characteristics

  * Allows DMA on the high-performance microcontrollers

* Different device drivers included

  * DS18B20 temperature sensor is natively supported

* Works with operating system due to hardware timing management

  * Separate thread-safe API is available

* API for device scan, reading and writing single bits
* User friendly MIT license

Requirements
^^^^^^^^^^^^

* C compiler
* Platform dependent drivers
* Few *kB* of non-volatile memory

Contribute
^^^^^^^^^^

Fresh contributions are always welcome. Simple instructions to proceed:

#. Fork Github repository
#. Respect `C style & coding rules <https://github.com/MaJerle/c-code-style>`_ used by the library
#. Create a pull request to ``develop`` branch with new features or bug fixes

Alternatively you may:

#. Report a bug
#. Ask for a feature request

License
^^^^^^^

.. literalinclude:: ../LICENSE

Table of contents
^^^^^^^^^^^^^^^^^

.. toctree::
    :maxdepth: 2
    :caption: Contents

    self
    get-started/index
    user-manual/index
    api-reference/index
    examples/index

.. toctree::
    :maxdepth: 2
    :caption: Other projects
    :hidden:

    LwDTC - DateTimeCron <https://github.com/MaJerle/lwdtc>
    LwESP - ESP-AT library <https://github.com/MaJerle/lwesp>
    LwEVT - Event manager <https://github.com/MaJerle/lwevt>
    LwGPS - GPS NMEA parser <https://github.com/MaJerle/lwgps>
    LwGSM - GSM-AT library <https://github.com/MaJerle/lwgsm>
    LwJSON - JSON parser <https://github.com/MaJerle/lwjson>
    LwMEM - Memory manager <https://github.com/MaJerle/lwmem>
    LwOW - OneWire with UART <https://github.com/MaJerle/lwow>
    LwPKT - Packet protocol <https://github.com/MaJerle/lwpkt>
    LwPRINTF - Printf <https://github.com/MaJerle/lwprintf>
    LwRB - Ring buffer <https://github.com/MaJerle/lwrb>
    LwSHELL - Shell <https://github.com/MaJerle/lwshell>
    LwUTIL - Utility functions <https://github.com/MaJerle/lwutil>
