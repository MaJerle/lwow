/**
 * \addtogroup      OW_LL
 * \{
 *
 * Low-Level layer is platform dependant and must be implemented by user for specific architecture.
 *
 * The library architecture is designed to allow UART communication with 1-Wire device, thus user must implement these functions:
 *
 *  - \ref ow_ll_init
 *  - \ref ow_ll_deinit
 *  - \ref ow_ll_set_baudrate
 *  - \ref ow_ll_transmit_receive
 *
 * Every function includes different parameters, except one common `void* arg`.
 * This parameter is used to distinguis between multiple 1-Wire ports used in application (2 1-Wire groups of sensors at the same time).
 * Argument value is the same as passed to \ref ow_init function on initialization of the application part.
 *
 * \par             ow_ll_init function
 *
 * \ref ow_ll_init must implement GPIO initialization part together with UART hardware (if used).
 * It is called on beginning for every \ref ow_init high-level function call.
 *
 * \par             ow_ll_deinit function
 *
 * \ref ow_ll_deinit must implement deinitialization part of hardware.
 * It is called on \ref ow_deinit application part
 *
 * \par             ow_ll_set_baudrate function
 *
 * Since 1-Wire works with `2` different baudrate speeds, we need to have an option to change the baudrate on request by application.
 *
 * \ref ow_ll_set_baudrate is called each time we need to change baudrate. Possible values include:
 *  
 *  - `9600 bauds`: Used for reset line purpose
 *  - `115200 bauds`: Used for single bit transmit/receive procedure
 *
 * \note            If UART hardware is not used, this function may do nothing.
 *
 * \par             ow_ll_transmit_receive function
 *
 * Most of the low-level execution code is done in this function as it must implement transmit and receive of data.
 * Data on input array are prepared to be transmitted directly over UART hardware and they may have these values:
 *
 *  - `0xF0`, used together with `9600 bauds` for reset pulse
 *  - `0x00`, used together with `115200 bauds` for logical bit `0`
 *  - `0xFF`, used together with `115200 bauds` for logical bit `1`
 *
 * \note            If UART hardware is not used, user may use known possible values and implement software GPIO toggling
 *
 * \}
 *
 */