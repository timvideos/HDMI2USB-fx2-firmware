# serial-loopback

This example shows how to use bitbang UART to send and receive serial data.
It sends a single predefined message over UART, by pushing it to TX queue,
and then waiting until whole message is received on UART RX queue. The
received message is then sent over USB CDC-ACM to the host.

To run the example the pins used for UART TX and RX must be externally connected.

The *Makefile* provided configures proper compilation flags.
By default bitbanged UART implementation is used.
To use hardware UART, run with `make` with `BITBANG=0`.

UART baudrate can be set using `BAUDRATE` *Makefile* variable.

## UART Configuration

UART implementation uses two statically allocated queues to store data,
their size has to be defined during compilation,
e.g. `-DUART_TX_QUEUE_SIZE=50 -DUART_RX_QUEUE_SIZE=50`.

UART can use either hardware implementation using Serial Port 0,
or a software bitbaned implementation can be used.
To use software UART, define `-DUART_SOFTWARE_BITBANG=1` during compilation,
and use e.g. `-DUART_TX_PIN=PB0 -DUART_RX_PIN=PA0` to set pins that should be
used.

> At the moment PA0 is hardcoded, as external interrupt on this pin is used
> for software UART RX synchronization.
