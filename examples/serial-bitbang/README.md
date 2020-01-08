# serial-bitbang

This example shows how to use bitbang UART to send and receive serial data.
Bitbang UART implementation uses two queues, one for TX and one for RX.
The application first sends the message to the TX queue and then reads it back
from RX queue. The message is then sent over USB CDC-ACM to host.

To run the example the pins used for UART TX and RX must be externally connected.
