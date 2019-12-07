.. _uart_timing:

UART and 1-Wire timing relation
================================

This part is explaining how UART and 1-Wire timings are connected together and what is important to take into consideration for stable and reliable communication.

1-Wire protocol specification match UART protocol specification when baudrate is configured at ``115200`` bauds.
Going into the details about 1-Wire protocol, we can identify that:

* To send ``1`` logical *bit* at 1-Wire level, application needs to transmit ``1`` byte at UART level with speed of ``115200`` bauds
* To send ``1`` logical *byte* at 1-Wire level, application must transmit ``8`` bytes at UART level with speed of ``115200`` bauds

.. figure:: ../static/images/1w_bit_byte.svg
	:align: center
	:alt: UART byte time is equivalent to 1 bit at 1-Wire level

	UART byte time is equivalent to 1 bit at 1-Wire level

Timing for each bit is very clearly defined by 1-Wire specification (not purpose to go into these details) and needs to respect all low and high level states for reliable communication. Each bit at 1-Wire level starts with master pulling line low for specific amount of time. Until master initiates communication, line is in *idle* mode.

Image above shows relation between UART and 1-Wire timing. It represents transmission of ``3`` bits on 1-Wire level or ``3`` bytes at UART level. *Green* and *blue* rectangles show different times between ending of one bit and start of new bit.

.. note::
	By 1-Wire specification, it is important to match bit timing. It is less important to match idle timings as these are not defined. Effectively this allows master to use UART to initiate byte transfer where UART takes care of proper timing.

Different timings (*green* vs *blue*) may happen if application uses many interrupts, but uses UART in polling mode to transmit data. This is very important for operating systems where context switch may disable interrupts. Fortunately, it is not a problem for reliable communication due to:

* When UART starts transmission, hardware takes care of timing
* If application gets preempted with more important task, 1-Wire line will be in idle state for longer time. This is not an issue by 1-Wire specification

More advanced embedded systems implement DMA controllers to support next level of transfers.

.. toctree::
    :maxdepth: 2