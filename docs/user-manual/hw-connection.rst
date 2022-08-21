.. _hw_connection:

Hardware UART connection with sensor
====================================

To be able to successfully use sensors and other devices with embedded systems, these needs to be physically wired with embedded system (or PC).

Target devices (usually sensors or memory devices) are connected to master host device using single wire (from here protocol name *One Wire*) for communication only. There are also voltage and ground lines, marked as *VCC* and *GND*, respectively.

At this point, we assume you are familiar with UART protocol and you understand it has ``2`` independent lines, one for transmitting data (*TX*) and second to receive data (*RX*).

For successful communication with sensors, bi-directional support is necessary to be implemented, but there is only ``1`` wire available to do so. It might sound complicated at this point.

OneWire data line is by default in *open-drain* mode. This means that:

* Any device connected to data line can at any time pull line to *GND* without fear of short circuit
* None of the devices are allowed to force high state on the line. Application must use external *pull-up* resistor to do so.

How to send data over *TX* pin if application cannot force high level on the line? There are ``2`` options:

* Configure UART TX pin to *open-drain* mode
* Use *push-pull* to *open-drain* converter using *2 mosfets and 1 resistor*

.. figure:: ../static/images/1w_pp2od.svg
	:align: center
	:alt: Push-pull to open-drain converter

	Push-pull to open-drain converter

Since many latest embedded systems allow you to configure *TX* pin to open-drain mode natively, you may consider second option instead.

.. figure:: ../static/images/1w_od.svg
	:align: center
	:alt: Embedded system with native open-drain TX pin support

	Embedded system with native open-drain TX pin support

.. warning::
	Application must assure that *TX* pin is always configured to open-drain mode, either with *push-pull to open-drain* converter or directly configured in the system.

TX and RX pins
""""""""""""""

Every communication starts by master initiating it. To transfer data over UART, application uses TX pin and RX pin is used to read data. With 1-Wire protocol, application needs to transfer data and read them back in real-time. This is also called *loop-back* mode.

Let's take reset sequence as an example. By specifications, UART has to be configured in ``9600`` bauds and master needs to send single UART byte with value of ``0xF0``. If there is any slave connected, slave must pull line to GND during transmision 0f ``0xF`` part of byte. Master needs to identify this by using *RX* pin of the UART.

.. note:: 
	Please check `official document on Maxim website <https://www.maximintegrated.com/en/design/technical-documents/tutorials/2/214.html>`_ to understand why ``0xF0`` and ``9600`` bauds.

Hardware connection with single pin
===================================

As the protocol name suggests, one-wire is a single wire protocol.
When UART is not available in the system, and when timing constraints may not be a problem, it is possible to use library in classic mode with single GPIO manipulation.

Pin shall be in open-drain mode (when available) and include external pull-up resistor.

.. toctree::
    :maxdepth: 2