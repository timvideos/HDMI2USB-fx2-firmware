
all: fifo i2c

fifo:
	as31 -Fhex -Ofifo.hex -l microload-fifo.a51

i2c:
	grep -v "; DEBUG" microload-i2c.a51 > microload-i2c-prod.a51
	as31 -Fhex -Oi2c-debug.hex -l microload-i2c.a51
	as31 -Fhex -Oi2c-prod.hex -l microload-i2c-prod.a51
	cat i2c-prod.hex

clean:
	rm microload-i2c-prod.a51
	rm *.hex *.lst

#	sdas8051 -l -o -s -p out.ihx microload.a51
#	objcopy -I ihex -O binary out.ihx out.hex
