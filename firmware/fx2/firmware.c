/**
 * Copyright (C) 2009 Ubixum, Inc. 
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 **/

#ifdef DEBUG 
#include "softserial.h"
#include <stdio.h>
#define putchar soft_putchar
#define getchar soft_getchar
#else
#define printf(...)
#endif

#include <fx2macros.h>
#include <fx2ints.h>
#include <autovector.h>
#include <delay.h>
#include <setupdat.h>

#include "cdc.h"

#define SYNCDELAY SYNCDELAY4

volatile __bit dosud=FALSE;
volatile __bit dosuspend=FALSE;

// custom functions
extern void main_loop();
extern void main_init();


void main() {
 printf("\n\n----\n\n");
 EA=0;

#ifdef DEBUG
 SETCPUFREQ(CLK_48M); // required for sio0_init 
 // main_init can still set this to whatever you want.
 soft_sio0_init(57600); // needed for printf if debug defined
#endif

 main_init();

 printf("1\n");

 // set up interrupts.
 USE_USB_INTS();
 
 printf("2\n");
 ENABLE_SUDAV();
 ENABLE_USBRESET();
 ENABLE_HISPEED(); 
 ENABLE_SUSPEND();
 ENABLE_RESUME();

 printf("3\n");
 EA=1;

 printf("4\n");
// iic files (c2 load) don't need to renumerate/delay
// trm 3.6
#ifndef NORENUM
 RENUMERATE();
#else
 USBCS &= ~bmDISCON;
#endif
 
 while(TRUE) {
     printf("*\n");

     main_loop();
     if (dosud) {
       dosud=FALSE;
       handle_setupdata();
     }

 } // end while

} // end main

void resume_isr() __interrupt RESUME_ISR {
 printf("e\n");
 CLEAR_RESUME();
}
  
void sudav_isr() __interrupt SUDAV_ISR {
 printf("s\n");
 dosud=TRUE;
 CLEAR_SUDAV();
}
void usbreset_isr() __interrupt USBRESET_ISR {
 printf("r\n");
 handle_hispeed(FALSE);
 CLEAR_USBRESET();
}
void hispeed_isr() __interrupt HISPEED_ISR {
 printf("h\n");
 handle_hispeed(TRUE);
 CLEAR_HISPEED();
}

void suspend_isr() __interrupt SUSPEND_ISR {
 printf("^\n");
 dosuspend=TRUE;
 CLEAR_SUSPEND();
}

void ISR_USART0(void) __interrupt TI_0_ISR __critical {
 printf("u\n");
}
/*
	if (RI) {
		RI=0;
		if (!cdc_can_send()) {
			// Mark overflow
		} else {
			cdc_queue_data(SBUF0);
		}
		// FIXME: Should use a timer, rather then sending one byte at a
		// time.
		cdc_send_queued_data();
	}
	if (TI) {
		TI=0;
//		transmit();
	}
}
*/
