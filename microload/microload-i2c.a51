

; I2CS Register
;
; Bit 7: START
; Bit 6: STOP
; Bit 5: LASTRD
; Bits 3 and 4: ID1, ID0
; Bit 2: BERR
; Bit 1: ACK
; Bit 0: DONE

; I2CDAT Register

; I2CTL Register
; Bit 1: STOPIE
; Bit 0: 400KHZ

; 13.5.3 Sending Data
; To send a multiple byte data record, follow these steps:
;
; 1. Set START=1. If BERR=1, start timer. 
;    The timeout should be at least as long as the longest expected
;    Startto-Stop interval on the bus.
; 2. Write the 7-bit peripheral address and the direction bit (0 for a write)
;    to I2DAT.
; 3. Wait for DONE=1 or for timer to expire*. If BERR=1, go to step 1.
; 4. If ACK=0, go to step 9.
; 5. Load I2DAT with a data byte.
; 6. Wait for DONE=1*. If BERR=1, go to step 1.
; 7. If ACK=0, go to step 9.
; 8. Repeat steps 5-7 for each byte until all bytes have been transferred.
; 9. Set STOP=1. Wait for STOP = 0 before initiating another transfer.


; 13.5.4 Receiving Data
; To read a multiple-byte data record, follow these steps:
;  1. Set START=1. If BERR = 1, start timer. The timeout should be at least as
;     long as the longest expected Startto-Stop interval on the bus.
;  2. Write the 7-bit peripheral address and the direction bit (1 for a read) to
;     I2DAT.
;  3. Wait for DONE=1 or for timer to expire*. If BERR=1, go to step 1.
;  4. If ACK=0, set STOP=1 and go to step 15.
;  5. Read I2DAT to initiate the first burst of nine SCL pulses to clock in the
;     first byte from the slave. Discard the value that was read from I2DAT.
;  6. Wait for DONE=1. If BERR=1, go to step 1.
;  7. Read the just-received byte of data from I2DAT. This read also initiates
;    the next read transfer.
;  8. Repeat steps 6 and 7 for each byte until ready to read the second-to-last
;     byte.
;  9. Wait for DONE=1. If BERR=1, go to step 1.
; 10. Before reading the second-to-last I2DAT byte, set LASTRD=1.
; 11. Read the second-to-last byte from I2DAT. With LASTRD=1, this initiates
;     the final byte read on the bus.
; 12. Wait for DONE=1. If BERR=1, go to step 1.
; 13. Set STOP=1.
; 14. Read the final byte from I2DAT immediately (the next instruction) after
;     setting the STOP bit. By reading I2DAT while the ‘stop’ condition is
;     being generated, the just received data byte is retrieved without
;     initiating an extra read transaction (nine more SCL pulses) on the
;     I2Cbus.
; 15. Wait for STOP = 0 before initiating another transfer.

	.equ	I2CS,	0xE678
	.equ	I2DAT,	0xE679
	.equ	I2CTL,	0xE67A

	.equ	MPAGE,	0x92
	.equ	DP0L,	0x82
	.equ	DP0H,	0x83

	.equ	PORTACFG, 0xe670

	.equ	OED,	0xb5
	.equ	PD0,	0xb0
	.equ	PD1,	0xb1
	.equ	PD2,	0xb2

	.org	0		; 4 bytes
	ljmp	_start		; 3 bytes


	.org	0x3f00		; 3 bytes
_start:
	mov	MPAGE, #0xE6	; 3 bytes
	; Cause the FX2 firmware to come up
	; USBCS 0xE680
	mov	r1, #0x80	; 2 bytes
	clr	A		; 1 byte
	movx	@r1, A		; 1 byte
	mov	A, r1		; 1 byte
	movx	@r1, A		; 1 byte

	mov	OED, #0xff	; DEBUG
	setb	PD0		; DEBUG
	clr	PD0		; DEBUG

	; I2 registers
	; movx with @R0 == I2CS
	; movx with @R1 == I2DAT
	; @R0 == I2CS
	mov	r0, #0x78	; 2 bytes
	; @R1 == I2DAT
	mov	r1, #0x79	; 2 bytes
	; End setup

	; ----------------------
	; Read from EEPROM
	; Start next I2C transaction, write 0x80 to I2CS
	;mov	A, #0x80	; 2 bytes
	movx	@r0, A		; 1 byte -- 3 bytes

	; Send (Address + READ)
	; 1000 000 1 -- Address: 0x40
	inc	A		; 1 byte
	movx	@r1, A		; 1 byte -- 2 bytes
	acall	i2c_wait4done	; 2 bytes

segment:
	setb	PD0		; DEBUG

	; Length L
	acall	i2c_wait4done	; 2 bytes
	rlc	A		; 1 byte
	jc	finished	; 2 bytes
	rr	A		; 1 byte
	mov	r5, A		; 1 byte  -- 6 bytes

	; Start Addr H
	acall	i2c_wait4done	; 2 bytes
	mov	DP0H, A		; 2 bytes -- 5 bytes

	; Start Addr L
	acall	i2c_wait4done	; 2 bytes
	mov	DP0L, A		; 2 bytes -- 5 bytes

	clr	PD0		; DEBUG

	; DATA!
load_data:
	setb	PD1		; DEBUG
	acall	i2c_wait4done	; 2 bytes
	movx	A, @r1		; 1 byte
	movx	@DPTR, A   	; 1 byte
	inc	DPTR       	; 1 byte
	djnz	r5, load_data	; 2 bytes
	clr	PD2		; DEBUG
	ajmp	segment

i2c_wait4done:
	; Wait for done
	movx	A, @R0		; 1 byte
	anl	A, #0x01	; 1 byte
	jz	i2c_wait4done	; 2 byte

	; Read I2DAT
	movx	A, @r1		; 1 byte
	ret			; 1 byte -- 6 bytes

finished:
	; Send stop condition
	mov	A, #0x40	; 2 bytes
	movx	@r0, A		; 1 byte -- 3 bytes

	ljmp	0x0000		; 3 bytes
_end:
	; Footer		; 4 bytes
