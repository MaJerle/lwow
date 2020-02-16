OneWire-UART |version| documentation
====================================

Welcome to the documentation for version |version|.

OneWire-UART is lightweight, platform independent library for Onewire protocol for embedded systems.

.. image:: static/images/logo.svg
    :align: center

.. rst-class:: center
.. rst-class:: index_links

    :ref:`download_library` · :ref:`getting_started` · `Open Github <https://github.com/MaJerle/onewire-uart>`_

Features
^^^^^^^^

* Written in ANSI C99
* Platform independent, uses custom low-level layer for device drivers
* 1-Wire protocol fits UART specifications at ``9600`` and ``115200`` bauds
* Hardware is responsible for timing characteristics

  * Allows DMA on the high-performance microcontrollers

* Different device drivers included

  * DS18x20 temperature sensor is natively supported

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

	self
	get-started/index
	user-manual/index
	api-reference/index
	examples/index
