.. _um_how_it_works:

How it works
============

OneWire-UART library tends to use *UART* hardware of any microcontroller/embedded system,
to generate timing clock for OneWire signals.

Nowaways embedded systems allow many UART ports, usually many more vs requirements for the final application.
OneWire protocol needs precise timings and embedded systems (in 99.9%) do not have specific hardware to handle communication of this type.

Fortunately, OneWire timings match with UART timings at ``9600`` and ``115200`` bauds.

.. note::
	Check `detailed tutorial from Maxim <https://www.maximintegrated.com/en/design/technical-documents/tutorials/2/214.html>`_ for more information.

.. toctree::
    :maxdepth: 2
