
	.equ	I2CS,	0xE678
	.equ	I2DAT,	0xE679
	.equ	I2CTL,	0xE67A

	.equ	MPAGE,	0x92
	.equ	DP0L,	0x82
	.equ	DP0H,	0x83

	.equ	OEA,	0xB2
	.equ	OEB,	0xB3
	.equ	IOA,	0x80
	.equ	IOB,	0x90
	.equ	PB7,	0x97

	.equ	ar4,	0x04
	.equ	ar5,	0x05

; Header
	.org	0
	;  8 Length H
	;  9 Length L
	; 10 Start Addr H
	; 11 Start Addr L
	; XX N Data bytes
;	.db	0x00		; 1 byte
;	.db	_end-_start	; 1 byte
;	.db	0x01		; 1 byte
;	.db	0x00		; 1 byte -- 4 bytes

_start:
	clr	A		; 1 byte
	mov	MPAGE, A	; 2 bytes
	; Port B == Data,  Input
	mov	OEB, A		; 2 bytes
	; Port A.0 == Clock, Output
	inc	A		; 1 byte
	mov	OEA, A		; 2 bytes

	mov	r0, #IOA	; 2 bytes
	mov	r1, #IOB	; 2 bytes
	dec	@r0		; 1 byte
	; End setup

	; Length H -- r4
	inc	@r0		; 1 byte
	jb	PB7, finished	; 3 bytes
	mov	ar4, @r1	; 2 bytes
	dec	@r0		; 1 byte  -- 7 bytes

	; Length L -- r5
	inc	@r0		; 1 byte
	mov	ar5, @r1	; 2 byte
	dec	@r0		; 1 byte  -- 4 bytes

	; Start Addr H
	inc	@r0		; 1 byte
	mov	DP0H, @r1	; 2 bytes
	dec	@r0		; 1 byte  -- 4 bytes

	; Start Addr L
	inc	@r0		; 1 byte
	mov	DP0L, @r1	; 2 bytes
	dec	@r0		; 1 byte  -- 4 bytes

	; DATA!
load_data:
	inc	@r0		; 1 byte
	mov	A, @r1		; 1 byte
	movx	@DPTR, A   	; 1 byte
	inc	DPTR       	; 1 byte
	dec	@r0		; 1 byte
	djnz	r5, load_data	; 2 bytes
	djnz	r4, load_data	; 2 bytes -- 9 bytes

finished:
	lcall	0x0000		; 3 bytes
	
_end:

; Footer
	; 0x80 - Length H 100000000
	; 0x01 - Length L
	; 0xE6 - Start Addr H
	; 0x00 - Start Addr L
	; 0x00 - 1 Data byte
	.db	0x80		; 1 byte
	.db	0x01		; 1 byte
	.db	0xE6		; 1 byte
	.db	0x00		; 1 byte
	.db	0x00		; 1 byte -- 5 bytes
