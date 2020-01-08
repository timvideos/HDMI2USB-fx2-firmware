# serial-cdc-loopback

This example shows how to use UART to send and receive serial data.
The application modifies all data received from CDC-ACM OUT endpoint
and then sends it to UART TX queue.
Any data received from UART RX queue is sent to host through CDC-ACM IN endpoint.

To run the example the pins used for UART TX and RX must be externally connected.
Alternatively, any data sent to UART RX pin will be passed over CDC-ACM, and any
data sent over USB will be sent over UART TX pin with some data modifications.

## UART Configuration

See the README for serial-loopback example. `../serial-loopback/README.md`.
