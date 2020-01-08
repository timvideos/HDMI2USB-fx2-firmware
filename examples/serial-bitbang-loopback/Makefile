TARGET    = serial-bitbang-loopback
LIBFX2DIR = ../../third_party/libfx2
LIBRARIES = fx2 fx2usb fx2isrs
FLAGS     = -I../../common -DDEBUG
SOURCES   = main descriptors ../../common/cdc ../../common/uart

FLAGS += -DUART_SOFTWARE_BITBANG=1 \
		 -DUART_TX_PIN=PB0 -DUART_RX_PIN=PA0 \
		 -DUART_TX_QUEUE_SIZE=50 -DUART_RX_QUEUE_SIZE=50

include ../../common/common.mk
